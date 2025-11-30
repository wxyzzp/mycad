/* D:\ICADDEV\PRJ\ICAD\OSNAPLOCKER.CPP
 * Abstract
 * 
 * COsnapLocker class serves as protector from calling snap functions 
 * 
 */ 

#include "stdAfx.h"

#include "osnapLocker.h"
#include "icad.h"
#include "osnapManagerImp.h"
#include "sdsapplication.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

COsnapLocker::COsnapLocker( bool bInitialize )
{
	m_bInitialized = false;
	if( bInitialize )
		initialize();
}

COsnapLocker::~COsnapLocker()
{
	if( m_bInitialized )
		m_pOsnapManager->decrementLock();
}

void COsnapLocker::initialize()
{
	if( !m_bInitialized )
	{
		m_bInitialized = true;
		m_pOsnapManager = ((CEngineApplication*)SDSApplication::GetEngineApplication())->getOsnapManager();
		m_pOsnapManager->incrementLock();
	}
}

COsnapStore::COsnapStore()
{
	m_bStore = false;
	m_iStoreOsnapMode = 0;
}

COsnapStore::~COsnapStore()
{
	restore();
}

void COsnapStore::store( int tmpOsmode )
{
	struct resbuf rb;
	
	// get an old value and store it
	if( !m_bStore )
	{
		sds_getvar( "OSMODE", &rb );
		m_bStore = true;
		m_iStoreOsnapMode = rb.resval.rint;
	}

	// set new temporary value
	rb.restype = RTSHORT;
	rb.resval.rint = tmpOsmode;
	sds_setvar( "OSMODE", &rb );
}

void COsnapStore::restore()
{
	if( m_bStore )
	{
		struct resbuf rb;
		rb.restype = RTSHORT;
		rb.resval.rint = m_iStoreOsnapMode;
		sds_setvar( "OSMODE", &rb );

		m_bStore = false;
		m_iStoreOsnapMode = 0;
	}
}
