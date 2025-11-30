/* D:\ICADDEV\PRJ\ICAD\OSNAPMODESTD.CPP
 * Abstract
 * 
 * COsnapModeStd class 
 * 
 */ 

#include "stdAfx.h"

#ifndef _OSNAPMODESTD_H
#include "osnapModeStd.h"
#endif

#include "OsnapObjects.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

COsnapModeStd::COsnapModeStd( const char* localModeString, const char* globalModeString, const char* tooltipString, COsnapMarker *pMarker, int iModeID )
{
	ASSERT( pMarker );
	m_localModeString = localModeString;
	m_globalModeString = globalModeString;
	m_tooltipString = tooltipString;
	m_pMarker = pMarker;
	m_iModeID = iModeID;

	m_lRefCount = 1;
}


const char* COsnapModeStd::localModeString() const
{
	return m_localModeString;
}

const char* COsnapModeStd::globalModeString() const 
{
	return m_globalModeString; 
}

const char* COsnapModeStd::tooltipString() const 
{
	return m_tooltipString; 
}

COsnapMarker* COsnapModeStd::marker() const
{
	return m_pMarker; 
}

int COsnapModeStd::getOsnapPoints(resbuf*, sds_point, sds_point, sds_point, sds_resbuf **ppPointList )
{
	*ppPointList = NULL;
	return 0;
}


long COsnapModeStd::AddRef()
{
	return InterlockedIncrement( &m_lRefCount );
}

void COsnapModeStd::Release()
{
	if( !InterlockedDecrement( &m_lRefCount ) )
		delete this;
}

COsnapModeStd::~COsnapModeStd()
{
	if( m_pMarker )
		m_pMarker->Release();
}
