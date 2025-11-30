/*	ADSTREAM.CPP - xxx
 *	Copyright (C) 1997 Visio Corporation. All rights reserved.
 *
 *	Abstract
 *
 *	Provides a replacement for IO.H open/close/lseek/read/write, which
 *	supports OLE IStream data. 
 *
 */

#include "adinclud.h"
#include "adstream.h"
#include "filestream.h"
#include <errno.h>



//extern	int	errno = 0;


	// Check if path is actually a psuedopath... create wrapper around file if not 	
IStream * 
db_streamWrapper:: GetIStream( LPCTSTR path)
	{
	if ( _tcsicmp( path, SIGNALPATH) == 0 )
		{
		db_streamWrapper	*wrapper = (db_streamWrapper *)path;

		assert( wrapper->stream);
		return wrapper->stream;
		}

	return NULL;
	}


	//	close down a handle and associated stream
int
ad_close(int handle)
	{
	IStream	*stream = (IStream *)handle;

	if ( stream )
		stream->Release();

	return 0;
	}


	//	create a new handle from the file (or IStream if fname is a wrapper)
int
ad_open(const char *fname, int flags, ...)
	{
	IStream	*stream = db_streamWrapper::GetIStream( fname);

	if ( stream )
		stream->AddRef();
	else
		{
		HRESULT	result = FileStream::FileStreamFactory( &stream, fname, flags);
		}

	return (int)stream;
	}


	//	Following translate read/write/lseek equivalents to IStream analogs

long
ad_length( int handle)
	{
	IStream	*stream = (IStream *)handle;
	HRESULT	result;
	STATSTG	stats;
	
	result = stream->Stat( &stats, STATFLAG_NONAME);
	if ( SUCCEEDED( result) )
		return stats.cbSize.LowPart;

	return -1;
	}


long
ad_lseek(int handle, long position, int mode)
	{
	IStream	*stream = (IStream *)handle;

	if ( stream )
		{
		ULARGE_INTEGER	finalPosition;
		HRESULT			result;

		switch ( mode )
			{
			default:
				{
				result = E_INVALIDARG;
				}
			case SEEK_CUR:
				{
				result = stream->Seek( StreamPosition( position), STREAM_SEEK_CUR, &finalPosition);
				break;
				}
			case SEEK_END:
				{
				result = stream->Seek( StreamPosition( position), STREAM_SEEK_END, &finalPosition);
				break;
				}
			case SEEK_SET:
				{
				result = stream->Seek( StreamPosition( position), STREAM_SEEK_SET, &finalPosition);
				break;
				}
			}

		if ( result != S_OK )
			{
			errno = EINVAL;
			return -1;
			}

		return StreamPosition( finalPosition);
		}

	errno = EBADF;
	return -1;
	}


int
ad_eof( int handle)
	{
	return( ad_lseek( handle, 0, SEEK_CUR) >= ad_length( handle) );
	}


int
ad_read(int handle, void *buffer, unsigned int count)
	{
	IStream	*stream = (IStream *)handle;

	if ( stream )
		{
		ULONG	bytesRead;

		HRESULT	result = stream->Read( buffer, count, &bytesRead);
		if ( result == S_OK )
			return bytesRead;
		}

	errno = EBADF;
	return -1;
	}


int
ad_write(int handle, const void *buffer, unsigned int count)
	{
	IStream	*stream = (IStream *)handle;

	if ( stream )
		{
		ULONG	bytesWritten;

		HRESULT	result = stream->Write( buffer, count, &bytesWritten);
		if ( result == S_OK )
			return bytesWritten;
		}

	errno = EBADF;
	return -1;
	}



static
int
get_accessflags( const char *accessmodes)
	{
	//	These are the only modes currently in use

	if ( _tcscmp( accessmodes, "rb") == 0 )
		return( O_RDONLY | O_BINARY );
	else if ( _tcscmp( accessmodes, "wb") == 0 )
		return( O_RDWR | O_BINARY);
	else if ( _tcscmp( accessmodes, "w+b") == 0 )
		return( O_RDWR | O_BINARY | O_CREAT);
	else
		return( 0);
	}



// *****************************************************************************
// Definition of the functions that AD wants to do IO
//

// **************************************
// WRITE




void *         	
odiow_fopen(void *path,char *accessmodes)
	{
	return (void *)ad_open( (const char *)path, get_accessflags( accessmodes));
	}

int		
odiow_fclose(void *f)
	{
	return ad_close( (int)f);
	}

int            	
odiow_fread( void *buf, unsigned int size, unsigned int num, void *f)
	{
	return ad_read( (int)f, buf, size * num );
	}

int            	
odiow_fseek( void *f, long offset, int whence)
	{
	return ad_lseek( (int)f, offset, whence);
	}

int            	
odiow_fwrite(void *b,int size,int count, void *f)
	{
	return ad_write( (int)f, b, size * count);
	}


int            	
odiow_fputs(const char *str,void *f)
	{
	int		length = strlen( str);
	int		result = odiow_fwrite( (void *)str, 1, length, f);
	
	if ( result == length && str[length-1] != '\n' )
		result += odiow_fwrite( "\n", 1, 1, f);

	return result;
	}


long           	
odiow_ftell( void *f)
	{
	return odiow_fseek( f, 0, SEEK_CUR);
	}


// **********************************************************
// READ



/* This is a set of optimized read routines which does least-recently-used
   buffering of the file data, allowing for frequently accessed data to be
   kept available.  They also use low level file I/O (open, read, close),
   rather than the high level routines (fopen, etc.) for increased
   performance.  Our experience shows approximately a 3x increase in speed
   when these routines (or derivatives of them) are used.

   The downside is that these routines are not completely portable, although
   with minor changes they will run on UNIX platforms.  We may implement an
   OD_FAST_READ_UNIX #define which will handle those platforms in the future.

   The same types of operations shown here are also portable to Macintosh.
*/


#include <io.h>

#define READFLAGS  (O_RDONLY | O_BINARY)
#define WRITEFLAGS (O_WRONLY | O_BINARY)
#define MAXODIOBLOCKS 16



#ifndef ODIOFILESTRUDEFINED
struct odioblockstru {
  char *buf;        /* this buffer */
  long  startaddr;  /* address from which it came in the file */
  short validbytes; /* number of valid bytes it holds */
  long  lru;        /* least recently used counter */
} ;

typedef struct ODIO_FILESTRU {
  long physfilepos;        /* where the file pointer is */
  long bufpos;             /* position from which buf was filled */
  int handle;              /* file handle */
  int lvl;                 /* bytes left in buf */
  unsigned short bufbytes; /* valid bytes read into buffer */
  char *nexchptr;          /* pointer to next char in buffer */
  char *cbuf;              /* pointer to the buffer currently being used */
  short eofflag;           /* 1 if filbuf fails */
  short usingblock;        /* which block is currently in use */
  struct odioblockstru datablock[MAXODIOBLOCKS];  /* the data being held */
} ODIO_FILE;
#define ODIOFILESTRUDEFINED
#endif

short odioreadbufs=min(8,MAXODIOBLOCKS);   /* number of read buffers */
short odioblocksize=8192;                  /* size of each read buffer */
long  odiopositionmask=~(8192-1);          /* mask to allow position check */
/* a full implementation should probably reset odiolru to 0L every
   time adInitAd2() is called. */
long  odiolru=0L;                          /* counter to track use of blocks */

#define odior_getc(f) \
  ((f)->datablock[(f)->usingblock].lru=odiolru++, \
  (--((f)->lvl) >= 0) ? (*(f)->nexchptr++) : \
     odior_fgetc(f))



void *			
odior_fopen(void *path)
	{
	ODIO_FILE *f;
	short i;

	if ((f=(ODIO_FILE *)odmem_malloc(sizeof(ODIO_FILE)))==NULL)
		{
		return(NULL);
		}

	if ((f->handle=ad_open((const char *)path,READFLAGS))==NULL)
		{
		odmem_free( f );
		return(NULL);
		}

	f->eofflag=f->bufbytes=f->lvl=0;
	f->bufpos=0L;  /* to start reads from 0L */
	f->cbuf=NULL;
	f->nexchptr=(char *)f->cbuf;
	f->usingblock = -1;
	f->physfilepos=0L;

	for (i=0; i<odioreadbufs; i++)
		{
	    f->datablock[i].buf=NULL;
		}
	for (i=0; i<odioreadbufs; i++) 
		{
		if ( (f->datablock[i].buf = (char *)odmem_malloc(odioblocksize))==NULL ) 
			{
			odior_fclose(f);
			return NULL;
		    }
		f->datablock[i].validbytes=0;
		f->datablock[i].lru = -1L;
		f->datablock[i].startaddr = -1L;
		}
	odior_fseek(f,0L,0);  /* initial seek, gets a buffer & stuff */
	if (f->eofflag) 
		{   /* file is empty */
		odior_fclose(f);
	    return(NULL);
		}
	return(f);
	}

int				
odior_fclose(void *f2)
	{
	ODIO_FILE *f=(ODIO_FILE *)f2;
	short retval,i;

	/* indicate buffers no longer in use */
	for (i=0; i<odioreadbufs; i++) 
		{
		if (f->datablock[i].buf!=NULL)
			{
			odmem_free(f->datablock[i].buf);
			}
		f->datablock[i].lru = -1L;
		f->datablock[i].validbytes=0;
		f->datablock[i].startaddr = -1L;
	  }
	retval=ad_close(f->handle);

	odmem_free(f);
	return(retval);
	}



void odior_filbuf( ODIO_FILE *f )
	{
	short i,minindex;
	long minlru;
	struct odioblockstru *minptr;
	char readerror;

	f->usingblock = -1;
	/* see if we are holding it already */
	for ( i=0; i < odioreadbufs; i++ ) 
		{
		if (f->datablock[i].startaddr == f->bufpos)
			{
			break;
			}
		}

	if ( i < odioreadbufs ) 
		{   
		/* we are already holding this part of the file */
		f->cbuf=f->datablock[i].buf;
		f->bufpos=f->datablock[i].startaddr;
		f->lvl=f->bufbytes=f->datablock[i].validbytes;
		f->nexchptr=(char *)f->cbuf;
		f->datablock[i].lru=odiolru++;
		f->usingblock=i;
		return;
		}

	/* not holding it, so look for a buffer to read into */
	/* first see if any are not yet loaded */
	minptr=NULL;
	minindex=0;

	for ( i = 0; i < odioreadbufs; i++ ) 
		{
		if ( f->datablock[i].startaddr == -1L ) 
			{
			minindex=i;
			minptr=&f->datablock[i];
			break;
			}
		}

	/* if all were used, then look for the least recently used one */
	if (minptr==NULL) 
		{
		minlru=0x7FFFFFFF;
		minptr=NULL;
		minindex=0;

		for ( i = 0; i < odioreadbufs; i++ ) 
			{

			if (f->datablock[i].lru<0L) 
				{
				f->datablock[i].lru=0L;
				}

			if (f->datablock[i].lru<minlru) 
				{
				minlru=f->datablock[i].lru;
				minptr=&f->datablock[i];
				minindex=i;
				}
			}
		}

	if (minptr==NULL)
		{
		return;  /* couldn't find one */
		}



	/* if we are not already physically at the read location, move there */
	/* then read into the buffer */

	readerror=0;
	do 
		{

		if (f->physfilepos!=f->bufpos || readerror) 
			{
			ad_lseek(f->handle,f->bufpos,0);
			}

		readerror=0;
		minptr->validbytes = ad_read(f->handle,minptr->buf,odioblocksize);

		/* check for error */
	    if (minptr->validbytes==-1)
			{
			readerror=1;
			}

	  } while (readerror);

  f->physfilepos=f->bufpos+minptr->validbytes;

  minptr->startaddr=f->bufpos;
  minptr->lru = odiolru++;
  f->bufbytes = f->lvl = minptr->validbytes;
  f->cbuf=minptr->buf;


  if (f->lvl==0)
	  {
	  f->eofflag=1;
	  }
  else 
	  {
	  f->eofflag=0;
	  }

  f->nexchptr=(char *)f->cbuf;
  f->usingblock=minindex;

}


int				
odior_fread(void *bufvoid, unsigned int size, unsigned int num, void *f2)
	{
	ODIO_FILE *f=(ODIO_FILE *)f2;
	long bytesleft;
	unsigned short bytestoread;
	unsigned char *buf=(unsigned char *)bufvoid;

	f->datablock[f->usingblock].lru = odiolru++;
	bytesleft=(long)size*(long)num;

	while ( (bytesleft > 0L) && 
			!odior_feof( f ) ) 
		{
	    if (f->lvl <= 0) 
			{
			f->bufpos += f->bufbytes;
			odior_filbuf( f );  /* get something in here */
		    }

	    if ((long)f->lvl<bytesleft)
			{
			bytestoread=(unsigned short)f->lvl;
			}
	    else 
			{
			bytestoread=(unsigned short)bytesleft;
			}

		memcpy( buf, f->nexchptr, bytestoread);
		f->lvl -= bytestoread;
		f->nexchptr += bytestoread;
		buf += bytestoread;
		bytesleft -= bytestoread;
		}
	return(num);

	}

int				
odior_fseek(void *f2, long offset, int whence)
	{
	ODIO_FILE *f=(ODIO_FILE *)f2;
	unsigned short bytestoadvance;

	assert( whence != 2 );

	if (whence==1) 
		{  
		/* change it to 0 -- note, whence==2 not supported */
		offset+=(f->bufpos+(f->nexchptr - f->cbuf));
		}
	/* from here on assume whence is 0 */

	/* moved eof set up here -- if filbuf hits eof, it hits eof */
	f->eofflag=0;

	/* if it's not in the area we're holding, seek to it */
	if ( (offset < f->bufpos) || 
		(offset >= f->bufpos + (long)f->bufbytes)) 
		{
		f->bufpos=offset & odiopositionmask;
		odior_filbuf(f);  /* locates it if we're already holding in another block */
		}

	f->nexchptr=(char *)(f->cbuf + (bytestoadvance=(unsigned short)(offset - f->bufpos)));
	f->lvl = f->bufbytes - bytestoadvance;

	return(0);
	}

short			
odior_feof(void *f)
	{
	return(((ODIO_FILE *)f)->eofflag);
	}

unsigned char	
odior_fgetc(void *f2)
	{
	ODIO_FILE *f=(ODIO_FILE *)f2;
	long origbufpos;

	f->datablock[f->usingblock].lru = odiolru++;

	if ((f->lvl)<=0) 
		{
		origbufpos=f->bufpos;
		f->bufpos+=f->bufbytes;
		odior_filbuf(f);


		/* this is to catch the situation where AD is already holding the block
		   in which this buffer is being held (so filbuf just sets it up and
		   comes back), but obviously we are trying to read past where we were
		   in the buffer, which means that the file is truncated */
		if (f->bufpos==origbufpos) 
			{
			f->eofflag=1;
			return(0);
			}
		}

	f->lvl--;
	return(*f->nexchptr++);

	}

unsigned char	
odior_fungetc(char ch, void *f2)
	{
	ODIO_FILE *f=(ODIO_FILE *)f2;

	f->nexchptr--;
	f->lvl++;
	f->eofflag=0;
	*(f)->nexchptr = ch;

	return(1);
	}

#define OD_CR 13
#define OD_LF 10

char *odior_fgetdxfline(char *s, unsigned int n, void *f)
{
  char lastchar=0;
  int onemorech;
  char *origs=s;

  if (!n || odior_feof(f)) return(NULL);
  while (n && lastchar!=OD_CR && lastchar!=OD_LF && !odior_feof(f)) {
    *s++ = lastchar = odior_fgetc(f);
    n--;
  }
  if (odior_feof(f) || n==0) s--;
  *s=0;

/* we stopped on either \r or \n (or feof); we read one more
   character if it was \r and not eof to determine whether we had a cr/lf
   pair.  If we didn't, put the character back.  This fixes MAC text files
   which end in cr only */
  if (lastchar==OD_CR && !odior_feof(f)) {
    onemorech=odior_fgetc(f);
    if (!odior_feof(f))  /* not really eof yet, next one is */
      if (onemorech!=OD_LF) odior_fungetc(onemorech,f);
  }
/* added this to deal with the absurd case of lines terminated with \n\r */
  else if (lastchar==OD_LF && !odior_feof(f)) {
    onemorech=odior_fgetc(f);
    if (!odior_feof(f))  /* not really eof yet, next one is */
      if (onemorech!=OD_CR) odior_fungetc(onemorech,f);
  }
  return(origs);
}

long			
odior_flength(void *f2)
	{
	ODIO_FILE *f=(ODIO_FILE *)f2;
	return ad_length( (int)f->handle );
	}

long			
odior_ftell(void *f2)
	{

	ODIO_FILE *f=(ODIO_FILE *)f2;
	return(f->bufpos + (f->nexchptr - f->cbuf));

	}





// *****************************
// MEMORY


void *			
odmem_malloc(int size)
	{
	//return malloc( size);
	return new BYTE [size];
	}

void			
odmem_free(void *loc)
	{
	//free( loc);
	delete [] loc;
	}




short           
odvm_init(void)
	{
	return(1);
	}



short           
odvm_term(void)
	{
	return(1);
	}


short           
odvm_readbytes(char *ptr,AD_VMADDR vmloc,unsigned short bytes)
	{
	memcpy( ptr, vmloc, bytes);
	return(1);
	}

short           
odvm_writebytes(AD_VMADDR vmloc,char *ptr,unsigned short bytes)
	{
	memcpy( vmloc, ptr, bytes);
	return(1);
	}

AD_VMADDR       
odvm_malloc(unsigned num_bytes)
	{	
	return (AD_VMADDR) new BYTE [num_bytes];
	}

short			
odvm_free(AD_VMADDR ptr)
	{	
	delete [] ptr;
	return(1);
	}


