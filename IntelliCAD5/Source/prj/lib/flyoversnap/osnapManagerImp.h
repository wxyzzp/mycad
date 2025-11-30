/* \PRJ\ICAD\OSNAPMANAGERIMP.H
 * Abstract
 * 
 * COsnapManagerImp class 
 * 
 */ 

#ifndef _OSNAPMANAGERIMP_H
#define _OSNAPMANAGERIMP_H

#include "osnapObjects.h"
#include "osnapManagerIcad.h"
#include "osnaphelpers.h"
#include "hiparray.h"

class CIcadView;
class COsnapMode;
class SDSApplication;

struct SOsnapDataRecord
{
	COsnapMode *m_pMode;
	sds_point m_point;
	sds_real m_distance;
	bool m_bInPickBox;
	SOsnapDataRecord() : m_pMode( NULL ), m_distance( -1.0 ), m_bInPickBox( false ) {};
};



/*-----------------------------------------------------------------------------
Osnap Manager class. This class perform all snapping job. Could be extended to 
process custom snapping modes.
@author Andrey Grafov
-------------------------------------------------------------------------------*/
class COsnapManagerImp : public COsnapManager, public COsnapManagerIcad, private CArray<SOsnapDataRecord, SOsnapDataRecord&>
{

public:
	struct SStdOsnapModeStruct
	{
		char *globalName;
		int nLocalNameID;
		int nTooltipID;
		int mode;
		COsnapMarker *pMarker;
	};

	class COsnapModeRecord
	{
	public:
		COsnapMode *m_pMode;
		SDSApplication *m_pApp;		// application this mode is assotiated with
		bool m_bIsActive;
		bool m_bIsCustom;
		COsnapModeRecord() : m_pMode( NULL ), m_pApp( NULL ), m_bIsActive( false ), m_bIsCustom( false ) {}
		COsnapModeRecord( COsnapMode *pMode, SDSApplication *pApp, bool bCustom = true ) : m_pMode( pMode ), m_pApp( pApp ), m_bIsActive( false ), m_bIsCustom( bCustom ) {}
		COsnapModeRecord( COsnapModeRecord& rec ) : m_pMode( rec.m_pMode ), m_pApp( rec.m_pApp ), m_bIsActive( rec.m_bIsActive ), m_bIsCustom( rec.m_bIsCustom ) {}
		~COsnapModeRecord(){}
	};

public:
	COsnapManagerImp();
	virtual ~COsnapManagerImp(){};

	// COsnapManager methods
	int addOsnapMode( COsnapMode *pMode );
	int removeOsnapMode( COsnapMode *pMode );
	int activateOsnapMode( const char *modeStr, BOOL bActivate );
	int modeIsActive( const char *modeStr );

	// COsnapManagerIcad methods
	/*----------------------------------------------------------------------------*//**
	Initializes standart osnap modes
	@author Andrey Grafov
	*//*----------------------------------------------------------------------------*/
	void initializeStandardModes( SDSApplication *pEngineApplication );

	/*----------------------------------------------------------------------------*//**
	Resets internal state of OsnapManager object on WM_MOUSEMOVE, WM_LBUTTONDOWN
	or WM_CHAR message.
	@author Andrey Grafov
	@param  pView => current view to be set
	*//*----------------------------------------------------------------------------*/
	void reset( CIcadView *pView, bool bUndrawMarker = true );
	
	/*----------------------------------------------------------------------------*//**
	Checks if there's some data available for snapping(snap record array is not empty)
	@author Andrey Grafov
	@return TRUE if not empty, FALSE otherwise
	*//*----------------------------------------------------------------------------*/
	int isData() const { return ( (m_iRecIdx != -1) ? TRUE : FALSE ); }
	
	/*----------------------------------------------------------------------------*//**
	Returns tooltip string for current snap point
	@author Andrey Grafov
	@return tooltip string for success or NULL
	*//*----------------------------------------------------------------------------*/
	const char* getOsnapString();
	
	/*----------------------------------------------------------------------------*//**
	Gets current snap point if there's one available in SnapManager. Called from
	SDS_AskForPoint().
	@author Andrey Grafov
	@param  pt => point to be returned
	@return 1 for success, 0 if there's no point, -1 if flyover snapping is disabled
	*//*----------------------------------------------------------------------------*/
	int getPoint( sds_point pt );

	/*----------------------------------------------------------------------------*//**
	Just reads the valid snapped point. Does not change the object. There is a call in icadapi.cpp 
	for recording correct point in logfile and script file. 
	@author Vishwanath Burkule ( Cybage Software Pvt Ltd)
	@param  pt => point to be returned
	@return 1 for success, 0 if there's no point, -1 if flyover snapping is disabled
	Bugzilla::78435; Wrong points are recorded in script if snapping is used.
	*//*----------------------------------------------------------------------------*/
 	int readSnapPoint( sds_point pt ) ;

	/*----------------------------------------------------------------------------*//**
	Draws a marker for the next osnap point from OsnapRecordArray. 
	@author Andrey Grafov
	@return TRUE for success FALSE otherwise
	*//*----------------------------------------------------------------------------*/
	int nextOsnapPoint();

	/*----------------------------------------------------------------------------*//**
	Sets snap point from OsnapRecordArray to be current and resets state(clears array)
	Called from WM_LBUTTONDOWN handler.
	@author Andrey Grafov
	@return TRUE for success(if array was not empty)
	*//*----------------------------------------------------------------------------*/
	int setPoint();

	/*----------------------------------------------------------------------------*//**
	Processes an input point. Called from WM_MOUSEMOVE handler.
	@author Andrey Grafov
	@param pView => current view
	@param  curPointRP => point to be processed
	@return TRUE for success(if array was not empty)
	*//*----------------------------------------------------------------------------*/
	int processPoint( CIcadView *pView, sds_point curPointRP );

	/*----------------------------------------------------------------------------*//**
	Increments snap lock counter
	@author Andrey Grafov
	*//*----------------------------------------------------------------------------*/
	
	void incrementLock(){ m_iLockCounter++; }
	/*----------------------------------------------------------------------------*//**
	Decrements snap lock counter
	@author Andrey Grafov
	*//*----------------------------------------------------------------------------*/
	void decrementLock(){ m_iLockCounter--; }

	int isLocked(){ return m_iLockCounter; }

	/*----------------------------------------------------------------------------*//**
	Removes CustomOsnap modes assotiated with particular application when this
	application is unloaded.
	@author Andrey Grafov
	@param pApp => application to be unloaded
	*//*----------------------------------------------------------------------------*/
	void removeAppModes( SDSApplication *pApp );

private:

	/*----------------------------------------------------------------------------*//**
	Finds osnap mode in OsnapManager mode's array
	@author Andrey Grafov
	@param iModeID => osnap mode value (for standard modes)
	@return COsnapMode pointer for success, NULL otherwise
	*//*----------------------------------------------------------------------------*/
	COsnapMode *findMode( int iModeID ) const; // to find standart mode by ID (OSMODE value)

	/*----------------------------------------------------------------------------*//**
	Finds osnap mode in OsnapManager array
	@author Andrey Grafov
	@param modeStr => osnap mode string to find(either global or local osnap string)
	@return COsnapMode pointer for success, NULL otherwise
	*//*----------------------------------------------------------------------------*/
	COsnapMode *findMode( const char *modeStr, int& idx ) const;

	/*----------------------------------------------------------------------------*//**
	Adds osnap record to array
	@author Andrey Grafov
	@return TRUE for success (if record was added), FALSE otherwise
	*//*----------------------------------------------------------------------------*/
	int addOsnapRecord( COsnapMode *pMode, sds_point snapPt, sds_point snapPtRP,
		sds_point inPtRP, sds_point ptAp1rp, sds_point ptAp2rp );

	/*----------------------------------------------------------------------------*//**
	Preprocesses initial selection set (explodes inserts, etc.) and adds insert and
	intersection osnap records
	@author Andrey Grafov
	@param ptAp1rp => 1st corner of osnap selector
	@param ptAp2rp => 2nd corner of osnap selector
	@param objSnapSS <=> selection set of objects to process
	@param tmpHipArray <= array of temporary objects, created while processing inserts. 
	@return TRUE for success, FALSE otherwise
	*//*----------------------------------------------------------------------------*/
	int preprocessSS( sds_point ptAp1rp, sds_point ptAp2rp, sds_name objSnapSS, CHipArray &tmpHipArray );
	
	/*----------------------------------------------------------------------------*//**
	Processes selection set and adds all other osnap records.
	@author Andrey Grafov
	@param ptAp1rp => 1st corner of osnap selector
	@param ptAp2rp => 2nd corner of osnap selector
	@param osmode => osnap mode value (only for standard modes)
	@param objSnapSS => selection set of objects to process
	@return TRUE for success, FALSE otherwise
	*//*----------------------------------------------------------------------------*/
	int processSS( sds_point ptAp1rp, sds_point ptAp2rp,int osmode, sds_name objSnapSS );

	/*----------------------------------------------------------------------------*//**
	Draws a marker for current point from OsnapRecordArray
	@author Andrey Grafov
	@return TRUE for success, FALSE otherwise
	*//*----------------------------------------------------------------------------*/
	int drawMarker();

	int drawMarker( const SOsnapDataRecord &rec ) const;

	int drawMarker( CIcadView *pView, COsnapMarker *pMarker, const sds_point pt ) const;

	/*----------------------------------------------------------------------------*//**
	Finds a position for a new OsnapRecordArray to be inserting refering to its
	closeness to cursor point and mode priority (nearest has lower priority than others
	if there's any in pickbox)
	@author Andrey Grafov
	@param rec => record to find position for
	@return TRUE for success, FALSE otherwise
	*//*----------------------------------------------------------------------------*/
	int find( SOsnapDataRecord &rec );
	
	/*----------------------------------------------------------------------------*//**
	Inserts a new record (if there's no one with such parameters)to appropriate
	position in array concerning to priortity (uses find method for these purposes)
	@author Andrey Grafov
	@param rec => record to be inserted
	@return TRUE for success, FALSE otherwise
	*//*----------------------------------------------------------------------------*/
	int insertSorted( SOsnapDataRecord &rec );


private:
	// state
	bool m_bReset;

	// flag which indicates that m_snapPoint was set and could be obtained on the following getPoint()
	bool m_bValidData;

	// internal osnap "lock" counter
	int m_iLockCounter;

	// index of current osnap record or -1 if there's no
	int m_iRecIdx;

	// input point in RP
	sds_point m_curPointRP;
	
	// result point in UCS
	sds_point m_snapPoint;

	// current view
	CIcadView *m_pView;

	// array of osnap modes
	CArray<COsnapModeRecord, COsnapModeRecord&> m_modeArray;

	// array of structures to initialize standard modes
	static const SStdOsnapModeStruct s_stdOsnapModeArray[];
	static int s_stdModeArraySize;

	// number extensions of s_stdOsnapModeArray
	int m_nDopStdMode;

	// critical section (to avoid race condition while accessing OsnapManager from different threads)
	SingleThreadSection m_crSection;

	// temporary storage for current osnap mode
	int m_osmode;


	friend void osnap_helper_addPtRec( int osmode, sds_point snapPt, sds_point snapPtRP, sds_point inPtRP, sds_point ptAp1rp, sds_point ptAp2rp );
};

#endif