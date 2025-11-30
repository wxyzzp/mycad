/* G:\ICADDEV\PRJ\LIB\ICADLIB\SMARTPOINTER.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "icadlib.h"
#include "SmartPointer.h"

SmartBuffer:: SmartBuffer( size_t size, const void *data, BOOL _copyOnWrite)
		: smartData( true), copyOnWrite( _copyOnWrite)
	{
	smartData->data = new char[ size];
	if ( smartData->data )
		{
		smartData->size = size;
		if ( data )
			memcpy( smartData->data, data, size);
		}
	}


char *
SmartBuffer:: Memcpy( const void *source, size_t count, size_t offset)
	{
	size_t	newSize = offset + count;

										// don't shrink allocations
	if ( newSize < smartData->size )
		newSize = smartData->size;

									// need to copy to new buffer
									// if current too small
									// or copy-on-write
	if ( 
			newSize > smartData->size
			|| copyOnWrite && smartData.IsShared() 
			)
		{
		SmartBuffer		newBuffer( newSize);

		newBuffer.Memcpy( smartData->data, smartData->size);
		smartData = newBuffer.smartData;
		}

									// now copy new text to buffer
	if ( count && source)
		memcpy( smartData->data + offset, source, count);

	return( smartData->data + offset);
	}


	// append data to buffer
char *
SmartBuffer:: Strcat( const void *source, size_t count)
	{
	return Memcpy( source, count, smartData->size);
	}


