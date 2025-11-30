/* D:\ICADDEV\PRJ\ICAD\OSNAPLOCKER.H
 * Abstract
 * 
 * COsnapLocker class 
 * 
 */ 

#ifndef _OSNAPLOCKER_H
#define _OSNAPLOCKER_H

#pragma once

class COsnapManagerIcad;

/*-----------------------------------------------------------------------------
Lock class for snapping purposes (in some cases we should avoid snap-processing
of objects while moving cursor - while calling sds_getstring() e.g.
Used as local variable to increment/decrement counter in OsnapManager in
constructor/destructor
@author Andrey Grafov
-------------------------------------------------------------------------------*/
class COsnapLocker
{
public:
	COsnapLocker( bool bInitialize = true );
	~COsnapLocker();
	void initialize();

private:
	COsnapManagerIcad *m_pOsnapManager;
	bool m_bInitialized;
};


class COsnapStore
{
public:
	COsnapStore();
	~COsnapStore();
	void store( int tmpOsmode);
	void restore();
private:
	bool m_bStore;
	int m_iStoreOsnapMode;
};

#endif
