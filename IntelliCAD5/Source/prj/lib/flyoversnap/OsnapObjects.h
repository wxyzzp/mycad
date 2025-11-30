/* \PRJ\ICAD\OSNAPOBJECTS.H
 * Abstract
 * 
 * Osnap classes declaration 
 * 
 */ 

#pragma once

#ifndef _OSNAPOBJECTS_H
#define _OSNAOBJECTSE_H


#include "sds.h"


class COsnapMode;
class COsnapManager;
class COsnapMarker;

/*-----------------------------------------------------------------------------
This is is a base class for Osnap marker objects
@author Andrey Grafov
-------------------------------------------------------------------------------*/

class COsnapMarker
{
public:

	/*----------------------------------------------------------------------------*//**
	Sets a point (in pixels) for futher use
	@author Andrey Grafov
	@param  pt => point to be set
	*//*----------------------------------------------------------------------------*/
	virtual void setLocation( CPoint &pt ) = 0;
	
	/*----------------------------------------------------------------------------*//**
	Sets size of marker (in pixels) for futher use
	@author Andrey Grafov
	@param  iSize => size to be set
	@param  iThickness => thickness to be set
	*//*----------------------------------------------------------------------------*/
	virtual void setSize( int iSize, int iThickness ) = 0;

	/*----------------------------------------------------------------------------*//**
	Draws a marker to the given DC,should be overriden in descendants for actual drawing
	@author Andrey Grafov
	@param  pDC => DC object to be drawn to
	*//*----------------------------------------------------------------------------*/
	virtual int Draw( CDC *pDC) const = 0;

	virtual long AddRef() = 0;
	virtual void Release() = 0;

protected:
	virtual ~COsnapMarker() {};

};


/*-----------------------------------------------------------------------------
This is is a base class for Osnap mode objects
@author Andrey Grafov
-------------------------------------------------------------------------------*/
class COsnapMode
{
public:
	/*----------------------------------------------------------------------------*//**
	Returns localized description of this mode
	@author Andrey Grafov
	*//*----------------------------------------------------------------------------*/
	virtual const char* localModeString() const = 0;

	/*----------------------------------------------------------------------------*//**
	Returns global description of this mode
	@author Andrey Grafov
	*//*----------------------------------------------------------------------------*/
	virtual const char* globalModeString() const = 0;

	/*----------------------------------------------------------------------------*//**
	Returns a string which will be used as tooltip string while getting input
	@author Andrey Grafov
	*//*----------------------------------------------------------------------------*/
	virtual const char* tooltipString() const = 0;

	/*----------------------------------------------------------------------------*//**
	Returns pointer of marker object which is used for this mode
	@author Andrey Grafov
	*//*----------------------------------------------------------------------------*/
	virtual COsnapMarker *marker() const = 0;

	/*----------------------------------------------------------------------------*//**
	Collects snapping point for this mode with given parameters
	@author Andrey Grafov
	@param inPtucs => cursor position point (in UCS)
	@param ptAp1ucs => first corner of apperture box (in UCS)
	@param ptAp2ucs => second corner of apperture box (in UCS)
	@param ppPointList <= resbuf containing list of all points found (in UCS)
	@return TRUE if success
	*//*----------------------------------------------------------------------------*/
	virtual int getOsnapPoints(resbuf *elist, sds_point inPtucs, sds_point ptAp1ucs, sds_point ptAp2ucs, sds_resbuf **ppPointList ) = 0;

	/*----------------------------------------------------------------------------*//**
	Checks custom condition. Condition is sufficient for enabling the mode.
	@author Alexey Malov
	@return TRUE if condition is satisfied, false - otherwise
	*//*----------------------------------------------------------------------------*/
	virtual bool customCondition() const {return false;}
	
	virtual long AddRef() = 0;
	virtual void Release() = 0;

protected:
	virtual ~COsnapMode(){};

};


/*-----------------------------------------------------------------------------
This class manages all snapping modes used for standard and custom snapping.
@author Andrey Grafov
-------------------------------------------------------------------------------*/
class COsnapManager
{
public:

	/*----------------------------------------------------------------------------*//**
	Adds new osnap mode
	@author Andrey Grafov
	@param pMode => pointer to the mode object to be added
	@retirn TRUE for success
	*//*----------------------------------------------------------------------------*/
	virtual int addOsnapMode( COsnapMode *pMode ) = 0;

	/*----------------------------------------------------------------------------*//**
	Removes osnap mode
	@author Andrey Grafov
	@param pMode => pointer to the mode object to be removed
	@retirn TRUE for success
	*//*----------------------------------------------------------------------------*/
	virtual int removeOsnapMode( COsnapMode *pMode ) = 0;

	/*----------------------------------------------------------------------------*//**
	Activates osnap mode
	@author Andrey Grafov
	@param modeStr => string described desired mode (either local or global one)
	@param bActivate => indicate what to do (activate/deactivate) with desired mode
	@retirn TRUE for success
	*//*----------------------------------------------------------------------------*/
	virtual int activateOsnapMode( const char *modeStr, BOOL bActivate ) = 0;

	/*----------------------------------------------------------------------------*//**
	Checks if desired mode is active
	@author Andrey Grafov
	@param modeStr => string described desired mode (either local or global one)
	@retirn TRUE if mode is found and active
	*//*----------------------------------------------------------------------------*/
	virtual int modeIsActive( const char *modeStr ) = 0;

	virtual ~COsnapManager(){};


};


/*----------------------------------------------------------------------------*//**
The following function is exported from Icad to be used in third-party applications
to maintain custom snapping
@author Andrey Grafov
@retirn pointer to the global instance of OsnapManager object
*//*----------------------------------------------------------------------------*/
extern "C" COsnapManager* osnapManager();

#endif
