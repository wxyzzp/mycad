/* D:\ICADDEV\PRJ\ICAD\OSNAPMANAGERIMP.CPP
 * Abstract
 *
 * COsnapManagerImp class
 *
 */
#pragma warning(disable : 4786)
#include "stdAfx.h"

#include "osnapManagerImp.h"
#include "osnapModeStd.h"
#include "OsnapMarkerStd.h"
#include "OsnapModeDefTan.H"
#include "icadDef.h"
#include "cmds.h"
#include "Preferences.h"
#include "icadView.h"
#include "icadDoc.h"
#include "sdsapplication.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern signed char SDS_OSNAP_TANPER_IGNORE;

sds_real sds_dist2d (sds_point p1,sds_point p2);//proto for f'n found in sds1


const COsnapManagerImp::SStdOsnapModeStruct COsnapManagerImp::s_stdOsnapModeArray[]=
{
	{"_END", IDC_SDS_OSNAP_END_0,  IDC_SDS_OSNAP_END_TIP, IC_OSMODE_END,		new COsnapMarkerStdEnd()},
	{"_MID", IDC_SDS_OSNAP_MID_1,  IDC_SDS_OSNAP_MID_TIP, IC_OSMODE_MID,		new COsnapMarkerStdMid()},
	{"_CEN", IDC_SDS_OSNAP_CEN_2,  IDC_SDS_OSNAP_CEN_TIP, IC_OSMODE_CENTER,		new COsnapMarkerStdCen()},
	{"_NOD", IDC_SDS_OSNAP_NOD_3,  IDC_SDS_OSNAP_NOD_TIP, IC_OSMODE_NODE,		new COsnapMarkerStdNod()},
	{"_QUA", IDC_SDS_OSNAP_QUA_5,  IDC_SDS_OSNAP_QUA_TIP, IC_OSMODE_QUADRANT,	new COsnapMarkerStdQua()},
	{"_INT", IDC_SDS_OSNAP_INT_6,  IDC_SDS_OSNAP_INT_TIP, IC_OSMODE_INT,		new COsnapMarkerStdInt()},
	{"_INS", IDC_SDS_OSNAP_INS_7,  IDC_SDS_OSNAP_INS_TIP, IC_OSMODE_INS,		new COsnapMarkerStdIns()},
	{"_PER", IDC_SDS_OSNAP_PER_8,  IDC_SDS_OSNAP_PER_TIP, IC_OSMODE_PERP,		new COsnapMarkerStdPer()},
	{"_TAN", IDC_SDS_OSNAP_TAN_9,  IDC_SDS_OSNAP_TAN_TIP, IC_OSMODE_TAN,		new COsnapMarkerStdTan()},
	{"_NEA", IDC_SDS_OSNAP_NEA_10, IDC_SDS_OSNAP_NEA_TIP, IC_OSMODE_NEAR,		new COsnapMarkerStdNea()},
	{"_QUI", IDC_SDS_OSNAP_QUI_11, IDC_SDS_OSNAP_QUI_TIP, IC_OSMODE_QUICK,		new COsnapMarkerStdQui()},
	{"_APP", IDC_SDS_OSNAP_APP_12, IDC_SDS_OSNAP_APP_TIP, IC_OSMODE_APP,		new COsnapMarkerStdApp()},
	{"_PLA", IDC_SDS_OSNAP_PLA_14, IDC_SDS_OSNAP_PLA_TIP, IC_OSMODE_PLAN,		new COsnapMarkerStdPla()}
};
int COsnapManagerImp::s_stdModeArraySize = 0;//initialized in initializeStandardModes()


#define SYNCRONIZE_ACCESS SingleThreadGuard guard( &m_crSection )


COsnapManagerImp::COsnapManagerImp()
{
	m_bReset = true;
	m_bValidData = false;
	m_iLockCounter = 0;
	m_iRecIdx = -1;
	m_pView = NULL;
	m_nDopStdMode = 0;
}

void COsnapManagerImp::initializeStandardModes( SDSApplication *pEngineApp )
{
	int i = 0;
	COsnapMode *pTmpMode = NULL;
	s_stdModeArraySize = sizeof(s_stdOsnapModeArray)/sizeof(SStdOsnapModeStruct);
	for( i = 0; i < s_stdModeArraySize; i++ )
	{
		int iModeID;
		CString localString, tooltipString;
		const char *globalString = s_stdOsnapModeArray[i].globalName;
		localString.LoadString( s_stdOsnapModeArray[i].nLocalNameID );
		tooltipString.LoadString( s_stdOsnapModeArray[i].nTooltipID );
		COsnapMarker *pOsnapMarker = s_stdOsnapModeArray[i].pMarker;
		iModeID = s_stdOsnapModeArray[i].mode;
		ASSERT( iModeID );	// must not be 0 for standard modes

		pTmpMode = new COsnapModeStd( localString, globalString, tooltipString, pOsnapMarker, iModeID );
		if( pTmpMode )
		{
			COsnapModeRecord modeRecord( pTmpMode, pEngineApp, false );	// m_bIsActive = false, m_bIsCustom = false(second argument)
			m_modeArray.Add( modeRecord );
			pTmpMode = NULL;
		}
	}

	//<alm: perhaps deferred tangent snapping mode should be placed into standard mode array?>
	{
		COsnapModeDefTan* pMode = new COsnapModeDefTan;
		if (pMode)
		{
			COsnapModeRecord modeRecord(pMode, pEngineApp, false);
			m_modeArray.Add(modeRecord);
			++m_nDopStdMode;
		}
	}
}

int COsnapManagerImp::addOsnapMode( COsnapMode *pMode )
{
	SYNCRONIZE_ACCESS;
	ASSERT( pMode );
	if( !pMode )
		return 0;

	int res = 1;
	// check if there's a mode with the same strings
	CString local = pMode->localModeString();
	CString global = pMode->globalModeString();

	int len = m_modeArray.GetSize();
	while( len-- )
	{
		COsnapMode *pTmpMode = m_modeArray[len].m_pMode;
		ASSERT( pTmpMode );
		if( !local.CompareNoCase( pTmpMode->localModeString() ) || !global.CompareNoCase( pTmpMode->globalModeString() ) )
		{
			res = 0;
			break;
		}
	}

	if( res )
	{
		SDSApplication *pApp = SDSApplication::GetActiveApplication();
		COsnapModeRecord modeRecord( pMode, pApp );	// m_bIsActive = false, m_bIsCustom = true(default value of the second argument)
		m_modeArray.Add( modeRecord );
		pMode->AddRef();
	}

	return res;
}

// To do: Protect access methods with critical section !!!!!!!!!!!!!!!!

int COsnapManagerImp::removeOsnapMode( COsnapMode *pMode )
{
	SYNCRONIZE_ACCESS;
	int res = 0;

	ASSERT( pMode );
	if( !pMode )
		return res;
	reset( NULL );	// Just for safety
	int len = m_modeArray.GetSize();
	while( len-- )
	{
		COsnapMode *pTmpMode = m_modeArray[len].m_pMode;
		ASSERT( pTmpMode );
		if( pTmpMode == pMode )
		{
			m_modeArray.RemoveAt( len );
			pMode->Release();
			res = 1;
			break;
		}
	}

	return res;
}

int COsnapManagerImp::activateOsnapMode( const char *modeStr, BOOL bActivate )
{
	SYNCRONIZE_ACCESS;
	int idx;
	int res = 0;
	COsnapMode *pTmpMode = findMode( modeStr, idx );
	if( pTmpMode )
	{
		m_modeArray.ElementAt( idx ).m_bIsActive = ( bActivate ? true : false );
		res = 1;
	}
	return res;
}

int COsnapManagerImp::modeIsActive( const char *modeStr )
{
	SYNCRONIZE_ACCESS;
	int idx;
	int res = 0;
	COsnapMode *pTmpMode = findMode( modeStr, idx );
	if( pTmpMode )
		res = ( m_modeArray[idx].m_bIsActive ? TRUE : FALSE );

	return res;
}

COsnapMode* COsnapManagerImp::findMode( const char *modeStr, int& idx ) const
{
	int res = 0;
	COsnapMode *pTmpMode = NULL;
	if( !modeStr || !*modeStr )
		return NULL;

	CString mode( modeStr );
	int len = m_modeArray.GetSize();
	while( len-- )
	{
		pTmpMode = m_modeArray[len].m_pMode;
		ASSERT( pTmpMode );
		if( !mode.CompareNoCase( pTmpMode->localModeString() ) || !mode.CompareNoCase( pTmpMode->globalModeString() ) )
		{
			res = 1;
			idx = len;
			break;
		}
	}

	return ( res ? pTmpMode : NULL );
}

COsnapMode* COsnapManagerImp::findMode( int iModeID ) const
{
	int res = 0;
	COsnapMode *pTmpMode = NULL;
	if( !iModeID || iModeID > 4096 )	// if it's a custom mode or standard mode which has
		return NULL;					// not implemented yet in ICAD

	int len = sizeof(s_stdOsnapModeArray)/sizeof(SStdOsnapModeStruct);
	if( len > m_modeArray.GetSize() )	// precaution if not all std modes are in the array
		len = m_modeArray.GetSize();	// (is it possible???)

	for( int i = 0; i < len; i++ )
	{
		pTmpMode = m_modeArray[i].m_pMode;
		ASSERT( pTmpMode );

		if( m_modeArray[i].m_bIsCustom )
			continue;

		COsnapModeStd *pStdMode = static_cast<COsnapModeStd*>(pTmpMode);

		if( pStdMode->modeID() == iModeID )
		{
			res = 1;
			break;
		}
	}

	return ( res ? pTmpMode : NULL );
}

void COsnapManagerImp::reset( CIcadView *pView, bool bUndrawMarker )
{
	SYNCRONIZE_ACCESS;
	m_bReset = true;

	if( bUndrawMarker )
		drawMarker();

	m_iRecIdx = -1;
	m_pView = pView;

	RemoveAll();
}

const char* COsnapManagerImp::getOsnapString()
{
	SYNCRONIZE_ACCESS;
	const char* osnapString = NULL;
	if( isData()  )
		osnapString = ElementAt( m_iRecIdx ).m_pMode->tooltipString();

	return osnapString;
}

/* Here are some explanations about working of COsnapManagerImp class:
processPoint() is the only entry point to this snapping pipeline. It is called from WM_MOUSEMOVE handler of
CIcadView class. Maybe it's more correctly to implement this processing at the Event-queue processing level,
but it would require some changes, 'cause WM_MOUSEMOVE events are generally not processed at this level( except
for the realtime processing). And this would certainly require us to be careful working with drawing from another
thread (event processing is done in Engine thread generally) using MFC objects(CIcadView and others).
Processing of a point to find appropriate snapping could be very expensive operation, so we do not have to process
all WM_MOUSEMOVE messages. We don't have to process it if:
1) We're at the command line input loop (there's certainly no reasons to process mouse movements in this case)
   This is checked in CIcadView::OnMouseMove() handler.
2) We're at the entsel input loop(we should allow the mouse to walk wherever it wants ;-)
   This is checked in CIcadView::OnMouseMove() handler.
3) The point in WM_MOUSEMOVE message is the same as in previous one(there could be a lot of WM_MOUSEMOVE messages
   with the same point, especially if CToolTipCtrl controls are enabled).
   This is checked here in processPoint() method.
4) We're in sds_getstring(), sds_getcorner() or sds_pmtssget() input loop(no reason to snap anywhere).
   This is checked here. To check it we use special class COsnapLocked which increment/decrement lock
   counter in appropriate places.
5) We're not at the waiting loop. This case is when we're not exactly at the command line waiting(we got all
   input we needed, but we have not returned to the command waiting - somewhere in the command executing stage).
   Some trics in IntelliCAD (like ExecuteUIAction) could force processing of other messages
   in the message queue, but if we are not waiting for event, there's no reason to process it.
   This is checked in CIcadView::OnMouseMove() handler by asking IcadInputEventQueue::IsWaiting().


To make a decision if we should process a point we check global vars SDS_AtCmdLine and SDS_AtEntGet and
use special "locker" class which is set in some cases if we don't want to process a point.

*/
int COsnapManagerImp::processPoint( CIcadView *pView, sds_point curPointRP )
{
	SYNCRONIZE_ACCESS;	/// ???????? can we avoid of using it here???
	int len, osmode;
	sds_real aperture;
	sds_name objSnapSS;
	CHipArray tmpHipArray;	// here should be temporary objects, which must be deleted ASAP after processing
	SOsnapDataRecord undrawOsnapRec;

	int res = 0;
	bool bEnableFlyOver = UserPreference::SnapFlyOverEnablePreference;
	if( !bEnableFlyOver )
		return res;

	bool bNeedToUndraw = false;
	long snapSSLength = 0;
	int stdModArraySize = sizeof(s_stdOsnapModeArray)/sizeof(SStdOsnapModeStruct);


	ASSERT( pView );

	if( !m_bReset && pView == m_pView && icadPointEqual(curPointRP,m_curPointRP) )
		return res;		// we don't have to process it cause nothing was changed
	else
	{
		// save data to undraw marker later on
		bool bDisplayMarker = UserPreference::SnapMarkerPreference;
		if( bDisplayMarker && isData() )
		{
			bNeedToUndraw = true;
			undrawOsnapRec = GetAt( m_iRecIdx );;
		}

		reset( pView, false );		// reset state
		ic_ptcpy( m_curPointRP, curPointRP );
	}

	{	// "mark" the scope of executing to avoid goto-related errors

	if( m_iLockCounter )
		goto Clean_Up;		// do not process if snapping is "locked"

	sds_name_clear( objSnapSS );

	gr_view *grView = m_pView->m_pCurDwgView;
	db_drawing *dbDrawing = ((CIcadDoc*)(m_pView->GetDocument()))->m_dbDrawing;
	if( !grView || !dbDrawing )
		goto Clean_Up;

	struct resbuf rb;
	if( SDS_getvar( NULL, DB_QOSMODE, &rb, dbDrawing, &SDS_CURCFG, &SDS_CURSES ) != RTNORM )
		goto Clean_Up;

	m_osmode = osmode = rb.resval.rint;
	len = m_modeArray.GetSize();

	// no active standard modes or they are diabled and no custom modes
	if( ((osmode & IC_OSMODE_OFF) || !osmode) && (len <= stdModArraySize + m_nDopStdMode) ) // all standard modes are not active and there's no custom mode
        goto Clean_Up;

	// no more reasons to avoid snapping (except for errors in sds_getvar()). clear m_bReset flag here!!!
	m_bReset = false;

	// Let's get a window for crossing
	if( SDS_getvar( NULL, DB_QVIEWSIZE, &rb, dbDrawing, &SDS_CURCFG, &SDS_CURSES ) != RTNORM )
		goto Clean_Up;
	aperture = rb.resval.rreal;

	if( (SDS_getvar( NULL, DB_QSCREENSIZE, &rb, dbDrawing, &SDS_CURCFG, &SDS_CURSES ) != RTNORM)  ||
		icadRealEqual(rb.resval.rpoint[1],0.00000))
		goto Clean_Up;
	aperture /= rb.resval.rpoint[1];

	if( (SDS_getvar(NULL,DB_QAPERTURE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES) != RTNORM ) || rb.resval.rint == 0 )
		goto Clean_Up;
	aperture *= rb.resval.rint;

	sds_point ptAp1rp, ptAp2rp, ptAp1ucs, ptAp2ucs, ptucs;
	// in RP
	ptAp1rp[0] = m_curPointRP[0] - aperture;
	ptAp1rp[1] = m_curPointRP[1] - aperture;
	ptAp2rp[0] = m_curPointRP[0] + aperture;
	ptAp2rp[1] = m_curPointRP[1] + aperture;
	ptAp1rp[2] = ptAp2rp[2] = 0.0;

	// translate to UCS
	gr_rp2ucs( ptAp1rp, ptAp1ucs, grView );
	gr_rp2ucs( ptAp2rp, ptAp2ucs, grView );
	gr_rp2ucs( m_curPointRP, ptucs, grView );

	SDS_PickObjects( "CROSSING", ptAp1ucs, ptAp2ucs, NULL, objSnapSS, 1, dbDrawing, false, false, true, false );

	sds_sslength( objSnapSS, &snapSSLength );
	if( snapSSLength && snapSSLength <= OSNAP_MAX_NUMBER ) // selection set is not empty and not to big to be processed further
		if( preprocessSS( ptAp1rp, ptAp2rp, objSnapSS, tmpHipArray ) )
			processSS( ptAp1rp, ptAp2rp, osmode, objSnapSS );

	//<alm>: processing of custome modes, moved to processSS
#if 0
	// Process custom modes now
	for( int i = stdModArraySize; i < len; i++ )
	{
		if( !m_modeArray[i].m_bIsActive )
			continue;

		COsnapMode *pMode = m_modeArray[i].m_pMode;
		ASSERT( pMode );

		sds_resbuf *pPointList = NULL;
		if( pMode->getOsnapPoints( ptucs, ptAp1ucs, ptAp2ucs, &pPointList ) )
		{
			// add those points into the record array
			for( sds_resbuf *tmpRb = pPointList; tmpRb; tmpRb = tmpRb->rbnext )
			{
				if( tmpRb->restype != RT3DPOINT )
					continue;

				sds_point ptRp;
				gr_ucs2rp( tmpRb->resval.rpoint, ptRp, grView );
				addOsnapRecord( pMode, tmpRb->resval.rpoint, ptRp, m_curPointRP, ptAp1rp, ptAp2rp );
			}
		}
		// free these array funally
		sds_relrb( pPointList );
	}
#endif
	if( GetSize() )
	{
		m_iRecIdx = 0;
		res = 1;

		const SOsnapDataRecord& rec = ElementAt( m_iRecIdx );
		if( bNeedToUndraw )
		{
			if( rec.m_pMode != undrawOsnapRec.m_pMode || !icadPointEqual(const_cast<double*>(rec.m_point),undrawOsnapRec.m_point ) )
			{
				drawMarker( undrawOsnapRec );
				drawMarker();
			}
			bNeedToUndraw = false;
		}
		else
			drawMarker();
	}

	}	// end of "scope" mark

Clean_Up:
	if( bNeedToUndraw )
	{
		drawMarker( undrawOsnapRec );
	}
	sds_ssfree( objSnapSS );
	tmpHipArray.Clear();

	return res;
}

int COsnapManagerImp::drawMarker()
{
	int res = FALSE;
	bool bDisplayMarker = UserPreference::SnapMarkerPreference;
	if( !bDisplayMarker || !isData() )	// we don't have to draw a marker or there was nothing to draw
		return res;

	const SOsnapDataRecord& rec = ElementAt( m_iRecIdx );
	return drawMarker( rec );
}

int COsnapManagerImp::drawMarker( const SOsnapDataRecord &rec ) const
{
	int res = FALSE;

	// Let's check view objects before drawing to it
	if( !m_pView || !IsWindow( m_pView->m_hWnd ) )
		return res;

	COsnapMarker *pMarker = rec.m_pMode->marker();
	if( !pMarker )
		return res;

	bool bDrawInAllViews = UserPreference::SnapMarkerDrawInAllViews;

	if( !bDrawInAllViews )
		res = drawMarker( m_pView, pMarker, rec.m_point );
	else
	{
		CIcadDoc *pDoc = (CIcadDoc*)m_pView->GetDocument();
		if( !pDoc )
			return res;

		POSITION pos = pDoc->GetFirstViewPosition();
		while ( pos != NULL )
		{
			CIcadView *pView = (CIcadView *)pDoc->GetNextView( pos );
			if( pView )
				res = drawMarker( pView, pMarker, rec.m_point);
		}
	}
	return res;
}

int COsnapManagerImp::drawMarker( CIcadView *pView, COsnapMarker *pMarker, const sds_point pt ) const
{
	int rop;
	int size, thickness, color;
	CPen pen, *savePen = NULL;
	CPalette *savePal = NULL;
	int res = FALSE;

	ASSERT( pView );	// should not be NULL
	ASSERT( pMarker );

	struct gr_view *view = pView->m_pCurDwgView;
	if( !view )
		return res;

	sds_point ptRP;
	CPoint ptPix;
	gr_ucs2rp(const_cast<double*>(pt), ptRP, view );
	gr_rp2pix( view, ptRP[0], ptRP[1], (int*)&ptPix.x, (int*)&ptPix.y );
	pMarker->setLocation( ptPix );

	size = UserPreference::SnapMarkerSizePreference;
	thickness = UserPreference::SnapMarkerThicknessPreference;
	pMarker->setSize( size, thickness );

	color = UserPreference::SnapMarkerColorPreference;
	if( !pen.CreatePen(PS_SOLID, thickness, SDS_XORPenColorFromACADColor(color)) )
		return res;

	// Draw directly to IcadView DC
	CDC *pDC = pView->GetDC();
	if(pView->m_pClipRectDC)
		pDC->IntersectClipRect(pView->m_pClipRectDC);

	rop = pDC->SetROP2( R2_XORPEN );
	savePen = pDC->SelectObject( &pen );
	savePal = pDC->SelectPalette( SDS_CMainWindow->m_pPalette, TRUE );
	res = pMarker->Draw( pDC );
	pDC->SelectPalette( savePal, TRUE );
	pDC->SelectObject( savePen );
	pDC->SetROP2( rop );

	pView->ReleaseDC( pDC );

	// Draw to FrameBuffer DC
	CDC *pFbDC = pView->GetFrameBufferCDC();
	if(pView->m_pClipRectMemDC)
		pFbDC->IntersectClipRect(pView->m_pClipRectMemDC);

	rop = pFbDC->SetROP2( R2_XORPEN );
	savePen = pFbDC->SelectObject( &pen );
	savePal = pFbDC->SelectPalette( SDS_CMainWindow->m_pPalette, TRUE );
	res = pMarker->Draw( pFbDC );
	pFbDC->SelectPalette( savePal, TRUE );
	pFbDC->SelectObject( savePen );
	pFbDC->SetROP2( rop );

	// clean up
	pen.DeleteObject();
	return res;
}

int COsnapManagerImp::nextOsnapPoint()
{
	SYNCRONIZE_ACCESS;
	int res = FALSE;
	// undraw the old marker and change current record
	if( drawMarker() )
	{
		int size = GetSize();
		if( size )
		{
			m_iRecIdx = (++m_iRecIdx) % size;
			res = drawMarker();
		}
	}
	return res;
}

int COsnapManagerImp::getPoint( sds_point pt )
{
	SYNCRONIZE_ACCESS;
	bool bEnableFlyOver = UserPreference::SnapFlyOverEnablePreference;
	if( !bEnableFlyOver )
		return -1;

	int res = m_bValidData;
	if( res )
	{
		sds_point_set( m_snapPoint, pt );
		m_bValidData = false;
	}
	return res;
}

//Bugzilla::78435; 14-04-03
int COsnapManagerImp::readSnapPoint( sds_point pt )
{
	SYNCRONIZE_ACCESS;
	bool bEnableFlyOver = UserPreference::SnapFlyOverEnablePreference;
	if( !bEnableFlyOver )
		return -1;

	int res = m_bValidData;
	if( res )
	{
		sds_point_set( m_snapPoint, pt );		
	}
	return res;
}

bool SDS_SelectedDefTan = false;
int COsnapManagerImp::setPoint()
{
	SYNCRONIZE_ACCESS;
	int res = 0;
	bool bEnableFlyOver = UserPreference::SnapFlyOverEnablePreference;
	if( !bEnableFlyOver )
		return res;

	res = isData();
	if( res )
	{
		sds_point_set( ElementAt( m_iRecIdx ).m_point, m_snapPoint );
		m_bValidData = true;
		//<alm>: check if snap point for deferred tangent mode
#if 0
		if (0 == strcmp("_DEFTAN", ElementAt( m_iRecIdx ).m_pMode->globalModeString()))
			SDS_SelectedDefTan = true;
		else
			SDS_SelectedDefTan = false;
#else
		if (SDS_OSNAP_TANPER_IGNORE == -1)
		{
			SDS_OSNAP_TANPER_IGNORE = 0;
//			activateOsnapMode("_DEFTAN", FALSE);
			if (0 == strcmp("_DEFTAN", ElementAt( m_iRecIdx ).m_pMode->globalModeString()))
			{
				if (m_osmode & IC_OSMODE_PERP)
				{
	    			SDS_OSNAP_TANPER_IGNORE |= 1;//we'll set bit back to 0 if nothing ultimately found
				}
				if (m_osmode & IC_OSMODE_TAN)
    			{
		    		SDS_OSNAP_TANPER_IGNORE |= 2;
				}
			}
		}
#endif
	}
	else
	{
		SDS_SelectedDefTan = false;
		//</alm>
	}
	reset( m_pView );	// reset state and undraw current marker
	return res;
}

int COsnapManagerImp::preprocessSS( sds_point ptAp1rp, sds_point ptAp2rp, sds_name objSnapSS, CHipArray &tmpHipArray )
{
	return osnap_helper_preprocessSS( ptAp1rp, ptAp2rp, objSnapSS, tmpHipArray );
}
//<alm>: moved into OsnapHelpers.cpp
#if 0
int COsnapManagerImp::processSS(  sds_point ptAp1rp, sds_point ptAp2rp,int osmode, sds_name objSnapSS )
{
	return ( (osnap_helper_processSS( osmode, m_curPointRP, ptAp1rp, ptAp2rp, objSnapSS ) == RTNORM) ? TRUE : FALSE );
}
#endif

int COsnapManagerImp::addOsnapRecord( COsnapMode *pMode, sds_point snapPt, sds_point snapPtRP,
		sds_point inPtRP, sds_point ptAp1rp, sds_point ptAp2rp )
{
	SYNCRONIZE_ACCESS;
	// Added point here is in UCS
	if( !pMode )
		return 0;

	SOsnapDataRecord rec;
	rec.m_pMode = pMode;
	ic_ptcpy( rec.m_point, snapPt );
	rec.m_distance = sds_dist2d( snapPtRP, inPtRP );

	// check if the point is inside a Aperture box
	sds_real minX = __min( ptAp1rp[0], ptAp2rp[0] );
	sds_real maxX = __max( ptAp1rp[0], ptAp2rp[0] );
	sds_real minY = __min( ptAp1rp[1], ptAp2rp[1] );
	sds_real maxY = __max( ptAp1rp[1], ptAp2rp[1] );

	if( snapPtRP[0] > minX &&
		snapPtRP[0] < maxX &&
		snapPtRP[1] > minY &&
		snapPtRP[1] < maxY
		)
		rec.m_bInPickBox = true;

	return( (insertSorted( rec ) == -1) ? FALSE : TRUE );
}

/************************************************************************************************/
int COsnapManagerImp::find( SOsnapDataRecord &rec )
{
	bool bNearestMode = false;
	bool bUpperNearest = false;
	bool bLowerNearest = false;
	int size = GetSize();
	if( !size )
		return 0;

	COsnapMode *pModeNearest = findMode( IC_OSMODE_NEAR );

	sds_real distance = rec.m_distance;
	bool bInPickBox = rec.m_bInPickBox;

	if( rec.m_pMode == pModeNearest )
		bNearestMode = true;

	int lower = 0;
	int upper = size - 1;
	const SOsnapDataRecord& Lower = ElementAt( lower );
	const SOsnapDataRecord& Upper = ElementAt( upper );

	if( Upper.m_pMode == pModeNearest )
		bUpperNearest = true;

	if( Lower.m_pMode == pModeNearest )
		bLowerNearest = true;

	if( (distance > Upper.m_distance) && (!bInPickBox || bNearestMode || !bUpperNearest ) )
		return size;
	if( ((distance <= Lower.m_distance) && (!bNearestMode || bLowerNearest || !Lower.m_bInPickBox)) ||
		( bInPickBox && !bNearestMode && bLowerNearest) )
		return 0;

	while( upper - lower > 1 )
	{
		bool bMiddleNearest = false;
        int  middle = (lower+upper)/2;
		const SOsnapDataRecord& Middle = ElementAt( middle );

		if( Middle.m_pMode == pModeNearest )
			bMiddleNearest = true;

		if( ((distance > Middle.m_distance) && (!bInPickBox || bNearestMode || !bMiddleNearest)) ||
			(bNearestMode && !bMiddleNearest && bInPickBox && Middle.m_bInPickBox) )
			lower = middle;
		else
			upper = middle;
	}
	return upper;
}

int COsnapManagerImp::insertSorted( SOsnapDataRecord &rec )
{
	BOOL bExist = FALSE;
	int size = GetSize();
	int index = find( rec );
	if( index < size )
	{
		// check if there's a point with the same coordinates in the array
		for( int i = index; i < size; i++ )
		{
			const SOsnapDataRecord& tmpRec = ElementAt( i );
			if( fabs(rec.m_distance - tmpRec.m_distance) < IC_ZRO )
			{
				if( icadPointEqual(rec.m_point, const_cast<double*>(tmpRec.m_point)) )
					return -1;
			}
			else
				break;
		}
	}

	InsertAt( index, rec );
	return index;
}

void COsnapManagerImp::removeAppModes( SDSApplication *pApp )
{
	SYNCRONIZE_ACCESS;
	ASSERT( pApp );
	reset( NULL );
	int len = m_modeArray.GetSize() - 1;
	for( int i = len; i > -1; --i )
	{
		COsnapModeRecord& rec = m_modeArray.ElementAt(i);
		if( rec.m_pApp == pApp )
		{
			rec.m_pMode->Release();
			m_modeArray.RemoveAt(i);
		}
	}
}


/************************************************************************************************/
COsnapManager* osnapManager()
{
	return static_cast<COsnapManager*>( static_cast<COsnapManagerImp*>( (((CEngineApplication*)SDSApplication::GetEngineApplication())->getOsnapManager() )));
}
