/* \PRJ\ICAD\OSNAPMANAGERICAD.H
 * Abstract
 * 
 * COsnapManagerIcad class 
 * 
 */ 

#ifndef _OSNAPMANAGERICAD_H
#define _OSNAPMANAGERICAD_H


class CIcadView;
class SDSApplication;



/*-----------------------------------------------------------------------------
This class is used in icad internally to perform flyover snapping job. Default
implementation of all methods is just a stub.
@author Andrey Grafov
-------------------------------------------------------------------------------*/
class COsnapManagerIcad
{
public:
	virtual ~COsnapManagerIcad(){};

public:

	/*----------------------------------------------------------------------------*//**
	Initializes standart osnap modes
	@author Andrey Grafov
	*//*----------------------------------------------------------------------------*/
	virtual void initializeStandardModes( SDSApplication *pEngineApplication ){};

	/*----------------------------------------------------------------------------*//**
	Resets internal state of OsnapManager object on WM_MOUSEMOVE, WM_LBUTTONDOWN
	or WM_CHAR message.
	@author Andrey Grafov
	@param  pView => current view to be set
	*//*----------------------------------------------------------------------------*/
	virtual void reset( CIcadView *pView, bool bUndrawMarker = true ){};
	
	/*----------------------------------------------------------------------------*//**
	Checks if there's some data available for snapping(snap record array is not empty)
	@author Andrey Grafov
	@return TRUE if not empty, FALSE otherwise
	*//*----------------------------------------------------------------------------*/
	virtual int isData() const { return FALSE; }
	
	/*----------------------------------------------------------------------------*//**
	Returns tooltip string for current snap point
	@author Andrey Grafov
	@return tooltip string for success or NULL
	*//*----------------------------------------------------------------------------*/
	virtual const char* getOsnapString(){ return NULL;}
	
	/*----------------------------------------------------------------------------*//**
	Gets current snap point if there's one available in SnapManager. Called from
	SDS_AskForPoint().
	@author Andrey Grafov
	@param  pt => point to be returned
	@return 1 for success, 0 if there's no point, -1 if flyover snapping is disabled
	*//*----------------------------------------------------------------------------*/
	virtual int getPoint( sds_point pt ){ return -1; }

	/*----------------------------------------------------------------------------*//**
	Just reads the valid snapped point. Does not change the object. There is a call in icadapi.cpp 
	for recording correct point in logfile and script file. 
	@author Vishwanath Burkule ( Cybage Software Pvt Ltd)
	@param  pt => point to be returned
	@return 1 for success, 0 if there's no point, -1 if flyover snapping is disabled
	For : Bugzilla::78435; Wrong points are recorded in script if snapping is used.
	*//*----------------------------------------------------------------------------*/
	virtual int readSnapPoint( sds_point pt ){ return -1; }

	/*----------------------------------------------------------------------------*//**
	Draws a marker for the next osnap point from OsnapRecordArray. 
	@author Andrey Grafov
	@return TRUE for success FALSE otherwise
	*//*----------------------------------------------------------------------------*/
	virtual int nextOsnapPoint(){ return FALSE; }

	/*----------------------------------------------------------------------------*//**
	Sets snap point from OsnapRecordArray to be current and resets state(clears array)
	Called from WM_LBUTTONDOWN handler.
	@author Andrey Grafov
	@return TRUE for success(if array was not empty)
	*//*----------------------------------------------------------------------------*/
	virtual int setPoint(){ return FALSE; }

	/*----------------------------------------------------------------------------*//**
	Processes an input point. Called from WM_MOUSEMOVE handler.
	@author Andrey Grafov
	@param pView => current view
	@param  curPointRP => point to be processed
	@return TRUE for success(if array was not empty)
	*//*----------------------------------------------------------------------------*/
	virtual int processPoint( CIcadView *pView, sds_point curPointRP ){ return FALSE; }

	/*----------------------------------------------------------------------------*//**
	Increments snap lock counter
	@author Andrey Grafov
	*//*----------------------------------------------------------------------------*/
	
	virtual void incrementLock(){};
	/*----------------------------------------------------------------------------*//**
	Decrements snap lock counter
	@author Andrey Grafov
	*//*----------------------------------------------------------------------------*/
	virtual void decrementLock(){};

	virtual int isLocked(){ return FALSE; }

	/*----------------------------------------------------------------------------*//**
	Removes CustomOsnap modes assotiated with particular application when this
	application is unloaded.
	@author Andrey Grafov
	@param pApp => application to be unloaded
	*//*----------------------------------------------------------------------------*/
	virtual void removeAppModes( SDSApplication *pApp ){};


};

#endif
