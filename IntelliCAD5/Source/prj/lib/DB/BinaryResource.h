/* G:\ICADDEV\PRJ\LIB\DB\BINARYRESOURCE.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Abstraction to create istream from binary resource data
 * 
 */ 

#ifndef	_BINARYRESOURCE_H
#define _BINARYRESOURCE_H

#ifndef	_INC_STRSTREAM
#include <strstrea.h>
#endif

#ifndef _DBDLL_H
#include "dbdll.h"
#endif

class BinaryResource
	{
public:
				BinaryResource( int _id)	: id( _id) {}
	istream		*NewIstream()
					{
									// locate the resource
					HRSRC	defaultFont = FindResource( ModuleInstance, MAKEINTRESOURCE( id), RT_BINDATA);
					HGLOBAL	fontHandle = LoadResource( ModuleInstance, defaultFont);
					char	*fontData = (char *)fontHandle;
					DWORD	size = SizeofResource( ModuleInstance, defaultFont);

									// create a stream from the resource
					if ( !(fontData && size) )
						return NULL;
					
									// caller must delete stream
					return new istrstream( fontData, size);
					}
private:
	int		id;		
	};

#endif


