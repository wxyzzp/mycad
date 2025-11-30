/* D:\ICADDEV\PRJ\ICAD\OSNAPMODESTD.H
 * Abstract
 * 
 * COsnapModeStd class 
 * 
 */ 

#ifndef _OSNAPMODESTD_H
#define _OSNAPMODESTD_H

#include "osnapObjects.h"

/*-----------------------------------------------------------------------------
This is is a base class for Osnap mode objects
@author Andrey Grafov
-------------------------------------------------------------------------------*/
class COsnapModeStd : public COsnapMode
{
public:
	COsnapModeStd( const char* localModeString, const char* globalModeString, const char* tooltipString, COsnapMarker *pMarker, int iModeID = 0 );

	virtual const char* localModeString() const;
	virtual const char* globalModeString() const;
	virtual const char* tooltipString() const;
	virtual COsnapMarker *marker() const;
	virtual int getOsnapPoints(resbuf *elist, sds_point inPtucs, sds_point ptAp1ucs, sds_point ptAp2ucs, sds_resbuf **ppPointList );
	
	virtual long AddRef();
	virtual void Release();

	virtual ~COsnapModeStd();

public:
	int modeID() const { return m_iModeID; }

private:
	CString m_localModeString;
	CString m_globalModeString;
	CString m_tooltipString;
	COsnapMarker *m_pMarker;

	int m_iModeID;

	long m_lRefCount;

};

#endif
