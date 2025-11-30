/* D:\ICADDEV\PRJ\LIB\ICADLIB\RESOURCESTRING.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "db.h"
#include "icadlib.h"
#include "icadWindows.h"
#include <tchar.h>




//inline
__ResourceString:: __ResourceString( int resourceID, LPCTSTR t) 
		: m_string( t), m_loaded( NULL), m_id( resourceID)
	{
	}


//inline this for performance if necessary
//makes rebuilds easier for now if it isn't inlined
__ResourceString:: operator LPCTSTR()
	{
	if ( !m_loaded )
		{
		TCHAR	buffer[1000];

										// load string into static
		int	size = ::LoadString( IcadSharedGlobals::GetIcadResourceInstance(), m_id, buffer, sizeof( buffer));
		ASSERT( size < sizeof( buffer)-1 );

										// allocate buffer if immediate too small
		LPTSTR	dest;
		if ( size < sizeof( m_buffer) )
			dest = m_buffer;
		else
			dest = new TCHAR[ size+1];

										// copy the string value
										// (dest is a non-const alias)
		if ( dest )
			_tcscpy( dest, buffer);

		m_loaded = dest;

#ifdef USBUILD
		ASSERT( strsame( m_string, m_loaded) );
#endif
		}

	return m_loaded;
	}

__ResourceString:: ~__ResourceString()
	{
	if ( m_loaded != m_buffer )
		delete[] const_cast<LPTSTR>(m_loaded);
	}


