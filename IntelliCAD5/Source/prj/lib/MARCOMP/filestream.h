/*	FileStream.h - IStream access to disk files.
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

#ifndef	_FILESTREAM_H
#define	_FILESTREAM_H

#ifndef _INC_FCNTL
#include <fcntl.h>
#endif

#ifndef _INC_STAT
#include <sys\stat.h>
#endif

#include <assert.h>
#include <tchar.h>

									// pseudo path indicating IStream present
#define	SIGNALPATH	__TEXT("STREAM:")


struct IStream;
class db_streamWrapper
	{
public:
	db_streamWrapper( IStream *_stream)
		{
		_tcscpy( pseudoPath, SIGNALPATH);
		stream = _stream;

		assert( stream);
		stream->AddRef();
		}

	~db_streamWrapper()
		{
		stream->Release();
		}

	operator LPCTSTR() const
		{
		assert( stream);
		return pseudoPath;
		}

	operator IStream *() const
		{
		return stream;
		}

	static IStream *GetIStream( LPCTSTR path);

private:
	TCHAR					pseudoPath[8];
	IStream					*stream;
	};


class FileStream : public IStream
	{
public:
										// flags suitable for open
										// O_RDONLY, etc.
	static HRESULT FileStreamFactory( LPSTREAM *stream, const TCHAR *fname, int flags)
		{
		int error = 0;
		FileStream	*object = new FileStream( fname, flags, error);

										// implicit QueryInterface for IStream
		if (object && !error)
			{
			*stream = object;
			(*stream)->AddRef();
			return S_OK;
			}
		else
			*stream = NULL;

		return STG_E_FILENOTFOUND;
		}

	STDMETHOD_(	ULONG, AddRef)();
	STDMETHOD_(	ULONG, Release)();
	STDMETHOD(	QueryInterface)( REFIID, void**);
	STDMETHOD(	Read)( void*, ULONG, ULONG *);
	STDMETHOD(	Write)( const void*, ULONG, ULONG *);
	STDMETHOD(	Seek)( LARGE_INTEGER, DWORD, ULARGE_INTEGER*);
	STDMETHOD(	SetSize)( ULARGE_INTEGER);
	STDMETHOD(	CopyTo)( IStream *, ULARGE_INTEGER, ULARGE_INTEGER*, ULARGE_INTEGER*);
	STDMETHOD(	Commit)( DWORD);
	STDMETHOD(	Revert)();
	STDMETHOD(	LockRegion)( ULARGE_INTEGER, ULARGE_INTEGER, DWORD);
	STDMETHOD(	UnlockRegion)( ULARGE_INTEGER, ULARGE_INTEGER, DWORD);
	STDMETHOD(	Stat)( STATSTG*, DWORD);
	STDMETHOD(	Clone)( IStream **);
private:
	FileStream( const TCHAR *fname, int flags, int &error);
	~FileStream();

	HANDLE			handle;
	LONG			references;
	BOOL			writing;
	FILETIME		atime;
	FILETIME		ctime;
	FILETIME		mtime;
	ULARGE_INTEGER	size;
	};


//	StreamPosition is a conversion tool for U/LARGE_INTEGER handling
//	Only files of < 2Gb are supported.

struct StreamPosition
	{
	StreamPosition( long _position) : position( _position)
		{
		}
	StreamPosition( LARGE_INTEGER _position)
		{
		assert( 
				   _position.HighPart ==  0 && (long)_position.LowPart >= 0
				|| _position.HighPart == -1 && (long)_position.LowPart <  0
				);

		position = _position.LowPart;
		}
	StreamPosition( ULARGE_INTEGER _position)
		{
		assert( _position.HighPart == 0 );

		position = _position.LowPart;
		}
	operator ULARGE_INTEGER()
		{
		ULARGE_INTEGER	result = {position, 0};
		return result;
		}
	operator LARGE_INTEGER()
		{
		LARGE_INTEGER	result;

		if ( position < 0 )
			{
			result.LowPart = position;
			result.HighPart = -1;
			}
		else
			{
			result.LowPart = position;
			result.HighPart = 0;
			}

		return result;
		}
	operator long()
		{
		return position;
		}

	long position;
	};


#endif	//_FILESTREAM_H

