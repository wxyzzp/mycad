/*	FileStream.cpp - IStream access to disk files.
 *	Copyright (C) 1997 Visio Corporation. All rights reserved.
 *
 *	Abstract
 *
 *	FileStream provides an (incomplete) IStream interface to a disk file.
 *	The interface is obtained via a factory method, ensuring that it can
 *	be properly disposed of via the COM AddRef/Release mechanism.
 *
 *	Actual I/O is performed by IO.H open.
 *
 */

#include <ole2.h>
#include "filestream.h"
#include <assert.h>
#include <io.h>
#include <errno.h>
#include <tchar.h>


#define	EQ		==
#define	ASSERT	assert


	// open file when constructing object. 
FileStream:: FileStream( const TCHAR *fname, int flags, int &error)
		: references(0)
	{
	int accessflag = GENERIC_READ;
	int createflag = OPEN_EXISTING;

	if (flags & O_CREAT)
		createflag = OPEN_ALWAYS;

	if (flags & O_RDWR)
		accessflag |= GENERIC_WRITE;

	else if (flags & O_WRONLY)
		accessflag = GENERIC_WRITE;


	handle = CreateFile( 
						fname, 
						accessflag,
						FILE_SHARE_READ | FILE_SHARE_WRITE,
						NULL,
						createflag,
						FILE_ATTRIBUTE_NORMAL,
						NULL
						);

	if (handle != INVALID_HANDLE_VALUE)
		{
		::GetFileTime( handle, &ctime, &atime, &mtime);
		size.LowPart = ::GetFileSize( handle, &size.HighPart);
		error = 0;
		}

	else
		{
//		DWORD errorcode = GetLastError();
		error = 1;
		}

	writing = (flags & (O_WRONLY | O_RDWR)) != 0;
	}


FileStream:: ~FileStream()
	{
	if (handle != INVALID_HANDLE_VALUE)
		CloseHandle(handle);
	}


	// Support COM reference counting model
STDMETHODIMP_( ULONG)
FileStream:: AddRef()
	{
	return InterlockedIncrement( &references);
	}


STDMETHODIMP_( ULONG)
FileStream:: Release()
	{
	LONG	current = InterlockedDecrement( &references);

	if ( current EQ 0 )
		delete this;

	return current;
	}


	// Only supports IStream and IUnkown
STDMETHODIMP
FileStream:: QueryInterface( REFIID requestedGUID, void** interfacePointer)
	{
	if ( IsEqualIID( requestedGUID, IID_IUnknown) || IsEqualIID( requestedGUID, IID_IStream) )
		{
		AddRef();
		*(IStream **)interfacePointer = this;
		return NOERROR;
		}

	return E_NOINTERFACE;
	}


	// Read and Write map directly to STDIO routines

STDMETHODIMP
FileStream:: Read( void* buffer, ULONG count, ULONG *countRead)
	{
	if (handle == INVALID_HANDLE_VALUE)
		return E_UNEXPECTED;

	if (ReadFile(handle, buffer, count, countRead, NULL))
		return NOERROR;
	
	return E_UNEXPECTED;
	}


STDMETHODIMP
FileStream:: Write( const void*buffer, ULONG count, ULONG *countWritten)
	{
	if (handle == INVALID_HANDLE_VALUE)
		return E_UNEXPECTED;

	if(WriteFile(handle, buffer, count, countWritten, NULL))
		if ( *countWritten EQ count )
			return NOERROR;
	
//	DWORD error = GetLastError();
	return E_UNEXPECTED;
	}


	//	Seek maps to STDIO lseek with appropriate U/LARGE_INTEGER conversions

STDMETHODIMP
FileStream:: Seek( LARGE_INTEGER delta, DWORD origin, ULARGE_INTEGER *finalPosition)
	{
	if (handle == INVALID_HANDLE_VALUE)
		return E_UNEXPECTED;

	ULARGE_INTEGER	position;

	switch ( origin )
		{
		case STREAM_SEEK_SET:
			position = StreamPosition( SetFilePointer(handle, StreamPosition(delta),0, FILE_BEGIN));
			break;
		case STREAM_SEEK_CUR:
			position = StreamPosition( SetFilePointer(handle, StreamPosition(delta), 0, FILE_CURRENT));
			break;
		case STREAM_SEEK_END:
			position = StreamPosition( SetFilePointer(handle, StreamPosition(delta), 0, FILE_END));
			break;
		default:
			return E_INVALIDARG;
		}

	if ( finalPosition )
		*finalPosition = position;

	return NOERROR;
	}


STDMETHODIMP
FileStream:: CopyTo( IStream *target, ULARGE_INTEGER count, ULARGE_INTEGER* read, ULARGE_INTEGER*written)
	{
	HRESULT	result = S_OK;
	DWORD	actual = 0;
	BYTE	buffer[0x1000];
	ULONG	remaining;
	ULONG	totalRead = 0;
	ULONG	totalWritten = 0;

										// no support for huge files yet (but allow MAXIMUM)
	ASSERT( count.HighPart EQ 0 || (count.LowPart EQ 0XFFFFFFFF && count.HighPart EQ 0XFFFFFFFF) );		
	remaining = count.LowPart;
										// copy blocks of data and update counts
	if ( target )
		while ( remaining && result EQ S_OK )
			{
			ULONG	bufferRead;
			ULONG	bufferWritten;

										// read remaining or another buffer
			if ( sizeof( buffer) < remaining )
				result = Read( buffer, sizeof( buffer), &bufferRead);
			else
				result = Read( buffer, remaining, &bufferRead);

			totalRead		+= bufferRead;
			remaining		-= bufferRead;
										
			if ( bufferRead )			// copy anything written
				{
				result = target->Write( buffer, bufferRead, &bufferWritten);
				totalWritten += bufferWritten;
				}
			
			else
				remaining = 0;			// EOF reached
			}
	else
		result = STG_E_INVALIDPOINTER;

	if ( read )
		{
		read->HighPart = 0;
		read->LowPart = totalRead;
		}
	if ( written )
		{
		written->HighPart = 0;
		written->LowPart = totalWritten;
		}

	return result;
	}


STDMETHODIMP
FileStream:: Stat( STATSTG* statstg, DWORD mode)
	{
	if ( !statstg )
		return E_INVALIDARG;
	else if ( mode != STATFLAG_NONAME )
		return E_NOTIMPL;

	statstg->pwcsName = NULL;
	statstg->atime = atime;
	statstg->ctime = ctime;
	statstg->mtime = mtime;
	statstg->cbSize = size;

	return S_OK;
	}


	///////////////////////////////////////////
	// Remaining interfaces are not implemented

STDMETHODIMP
FileStream:: SetSize( ULARGE_INTEGER)
	{
	return E_NOTIMPL;
	}


STDMETHODIMP
FileStream:: Commit( DWORD)
	{
	return E_NOTIMPL;
	}


STDMETHODIMP
FileStream:: Revert()
	{
	return E_NOTIMPL;
	}


STDMETHODIMP
FileStream:: LockRegion( ULARGE_INTEGER, ULARGE_INTEGER, DWORD)
	{
	return E_NOTIMPL;
	}


STDMETHODIMP
FileStream:: UnlockRegion( ULARGE_INTEGER, ULARGE_INTEGER, DWORD)
	{
	return E_NOTIMPL;
	}


STDMETHODIMP
FileStream:: Clone( IStream **)
	{
	return E_NOTIMPL;
	}


