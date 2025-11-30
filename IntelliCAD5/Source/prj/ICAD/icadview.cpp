/*	C:\DEV\PRJ\ICAD\ICADVIEW.CPP - CIcadView class supporting functions
 *	Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 *	Abstract
 *	This source file contains most of the functions used to manipulate the
 *	current view, such as mouse feedback, view creation and destruction, etc.
 *
 */

/////////////////////////////////////////////////////////////////////
//



#include "Icad.h"/*DNT*/
#include "sdsapplication.h" /*DNT*/
#include "threadcontroller.h" /*DNT*/
#include "inputeventqueue.h"
#include "DDPlaneBuffer.h"/*DNT*/
#include "DDOpenGL.h"
#include "PrinterDrawDevice.h"/*DNT*/
#include "commandqueue.h" /*DNT*/

#include "IcadCmdBar.h"
#include "IcadChildWnd.h"
#include "IcadCntrItem.h"/*DNT*/
#include "IcadCursor.h"
#include "IcadApp.h"/*DNT*/
#include "IcadPrintDia.h"/*DNT*/
#include "cmds.h"/*DNT*/
#include "IcadOleRes.h"/*DNT*/
#include "IcadPreviewView.h"/*DNT*/
#include "IcadRectTracker.h"/*DNT*/
#include "IcadToolBarMain.h"/*DNT*/
#include "IcadMenu.h"/*DNT*/
#include "IcadMenuMain.h"/*DNT*/
#include "realtime.h" /*DNT*/
#include <oledlg.h>
#include "PrintDialog.h" /*DNT*/
#include "checkfp.h"/*DNT*/
#include "IcadView.h"
#include "Preferences.h"
#include "cmdsInet.h"
#include "CloneHelper.h"
#include "icadgridiconsnap.h"
#include "HideAndShade.h"
//#include "vxtabrec.h"	// for needHLrendering
#include "LayoutTabs.h"

#include "DropActions.h"

typedef	CComPtr< IIcadPlotProperties>	PIIcadPlotProperties;
static bool
ScrollBarsOn( void )
	{
	struct resbuf rb;
	SDS_getvar(NULL,DB_QWNDLSCRL,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

	return( rb.resval.rint EQ 1 );
	}

#define	ICAD_VIEW_PGSIZE 0.95	//% of screen change when paging w/scroll bar
#define	ICAD_VIEW_LNSIZE 0.05	//% of screen change when line-changing on scoll bar
#define ICAD_VIEW_BSIZMAX 98	//button size to use when zoomed at extents.  scroll
								//	range is 100, but you can't set button size to this
								//	Set @ 98 so we can scroll both directions

//*** For the OLE client item context menu
#define ICAD_OLEITEMMNU_START		100
#define ICAD_OLEITEMMNU_VERBSTART	104
#define ICAD_OLEITEMMNU_END			120

extern int SDS_IsDragging;
extern int SDS_AtMouseMove;
extern sds_point SDS_PreGridPoint;
extern int SDS_AtCopyMoveCommand;
extern int SDS_DidDraggenOnce;
extern int SDS_AtCmdLine;
extern int SDS_AtEntGet;
extern bool cmd_InsideOnPaint;
//BugZilla No. 78243; 05-08-2002
extern bool cmd_InsideTilemodeCmd;
extern bool cmd_InsideWtile;
extern bool cmd_InsideRedrawVPpaint;
extern bool	SDS_SetAUserCallback;
//extern RGBQUAD		   SDS_PaletteColors[256]; EBATECH(CNBR) 2001-06-24 OEM App.
extern bool g_Vba6Installed;

// SystemMetrix(shiba)-[06.10.2001
//   FIX: During command(zoom, regen etc) execution, moving mouse (wheel), crash.
extern bool	SDS_RedrawDrawing_Doing;
extern bool	SDS_BuildRegenList_Doing;
// ]-SystemMetrix(shiba) 06.10.2001

#define CONTROL_SHIFT_FLAGS(n)	((n & (MK_SHIFT|MK_CONTROL)) == (MK_SHIFT|MK_CONTROL))
#define CONTROL_FLAGS(n)  ((n & MK_CONTROL) == MK_CONTROL)
#define SHIFT_FLAGS(n)	((n & MK_SHIFT) == MK_SHIFT)

sds_name	SDS_EditPropsSS_DblClk = {0L, 0L};	/*D.G.*/// Use it for calling EntProp dialog on mouse double-click.

//4M Spiros Item:31->
extern CString	MdMacro;
//4M Spiros Item:31<-
const UINT RealTimeMouseWheelTimerID = 2000;    // some ID
const UINT RealTimeMouseWheelElapseTime = 300;	// elapse time

// // Modified CyberAge VSB 12/05/2001
// SNAPSTRUCT SnapStructLastSnap;	//Stores info of last flyover snap drawn, extern in sds_get.cpp
// sds_point SnapPt;				//Mouse Cordinate where last snap was drawn, used while TABbing in Flyover Snapping
// int g_nSnappedType;				//extern in sds_osnap, used to find the type of snap returned by sds_osnap()
// int Snap_DrawInOneWindow = 0;	//extern in icadapi, used to show flyover snap in all views
// extern int SDS_BoxModeCursor;	//declared in icadapi.cpp, used to where a "selecting rectangle box" is being drawn
// // Modified CyberAge VSB 20/05/2001
// extern BOOL bGetPoint ;			// if waiting for point.
// extern int SDS_PointDisplayMode;// if waiting for point.
/////////////////////////////////////////////////////////////////////
// CIcadView message map and member (huh-huh) functions

IMPLEMENT_DYNCREATE(CIcadView, CView)

BEGIN_MESSAGE_MAP(CIcadView, CView)
	//{{AFX_MSG_MAP(CIcadView)
	ON_WM_CHAR()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
		ON_WM_RBUTTONDBLCLK() // SMC(Maeda)
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEWHEEL()
	ON_WM_TIMER()
	ON_WM_QUERYNEWPALETTE()
	ON_WM_PALETTECHANGED()
	//}}AFX_MSG_MAP
	// Ole sh...tuff.
	ON_WM_SETFOCUS()
	ON_WM_CAPTURECHANGED()
	ON_COMMAND(ID_OLE_INSERT_NEW, OnInsertObject)
	ON_COMMAND(ID_CANCEL_EDIT_CNTR, OnCancelEditCntr)
	ON_COMMAND(ID_CANCEL_EDIT_SRVR, OnCancelEditSrvr)
	ON_COMMAND_RANGE(ICAD_OLEITEMMNU_START,ICAD_OLEITEMMNU_END,OnOleContextMenuPick)
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipText)
END_MESSAGE_MAP()

CIcadView::CIcadView()
{
	m_pDocument = NULL;
	m_iViewMode = kNormal;
	m_bNeedRedraw=1;
	m_pClipRectDC=NULL;
	m_pClipRectMemDC=NULL;
	m_pZoomPrevB=m_pZoomPrevC=NULL;
	m_pZoomPsPrevB=m_pZoomPsPrevC=NULL;
	m_iPrevViews=0;
	m_iPrevPsViews=0;
	m_fUseRegenList=1;
	m_bEatNextMouse=FALSE;
	m_pViewsDoc=NULL;
	m_pVportTabHip=NULL;
	m_CursorOn=0;
	m_pCurDwgView=NULL;
	m_idcCursor=IDC_ICAD_PICK5; //*** Default ID for the cursor in the view window.
	m_bAltView=FALSE;
	m_pPolyPts=NULL;
	m_nPolyPts=0;
	m_bMakingSld=0;
	m_pSelection=NULL;
	m_pDragVects=NULL;
	m_bDontDelInTab=FALSE;
	m_bReScaleOnce=FALSE;
	m_LastCol=m_LastHl=m_LastRop=m_LastWid=-2;
	m_FillLastCol=m_FillLastHl=m_FillLastRop=-2;
	m_iWinX=m_iWinY=0;
	m_prevCursor=(::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW)));
	m_pMain=(CMainWindow*)((CIcadApp*)AfxGetApp())->m_pIcadMainWnd;
	if(m_pMain==NULL) return;
	m_bIsTempView=m_pMain->m_bUsingTempView;
//	m_pMain->m_bUsingTempView = FALSE;	/*D.G.*/// Commented out: it used it in EntityRenderer::drawDisplayObjects & EntityRenderer::DrawEntity.
	m_bSizeInPlace=FALSE;
	m_bInPlaceDeactivated=TRUE;
	m_dPreInPlaceViewSize=0.0;
	m_dViewSizeAtLastRegen=0.0;
	m_bNeedRestoreView=FALSE;
#ifdef _USE_OPENGL_
	m_pFrameBufferDrawDevice = new CDDOpenGL;
#else
	m_pFrameBufferDrawDevice = new CDDPlaneBuffer;
#endif
	m_RectBitBltPt[0].x = m_RectBitBltPt[0].y = 0;
	m_RectBitBltPt[1].x = m_RectBitBltPt[1].y = 0;
	m_showScrollBars = false;

	m_printerDrawDevice = NULL;
	m_oldPrinterPalette = NULL;

	// EBATECH(shiba)-[2001-06-12 Clear afterimage by command interrupt RTZOOMPAN etc.
	m_bMButtonClick = false;
	// ]-EBATECH(shiba) 2001-06-12

	if(m_bIsTempView)
	{
		m_pMain->m_nOpenViews++;
	}
	else
	{
		if(m_pMain->m_nIsClosing==2)
		{
			m_pMain->m_nOpenViews++;
			m_pMain->m_nIsClosing=0;
			return;
		}
		if(m_pMain->m_nOpenViews==0)
		{
			m_pMain->m_pCurFrame=NULL;
			m_pMain->m_pCurView=NULL;
			m_pViewsDoc=m_pMain->m_pCurDoc=NULL;
			struct resbuf rb;
			rb.restype=SDS_RTSHORT;
			rb.resval.rint=m_pMain->m_nLastMDIState;
			SDS_setvar(NULL,DB_QWNDLMDI,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
			//*** Update the menus and toolbars.
			m_pMain->IcadWndAction(ICAD_WNDACTION_NEWMENUS);
		}
		m_pMain->m_nOpenViews++;

		if( m_pMain &&
			m_pMain->m_pCurView &&
			ScrollBarsOn() )
			{
#ifdef _LAYOUTTABS_
			m_pMain->m_pCurView->EnableScrollBarCtrl(SB_BOTH, FALSE);
#else
			m_pMain->m_pCurView->EnableScrollBar(SB_BOTH,ESB_DISABLE_BOTH);
#endif
			}
	}

	m_pCurrentPrinterDC = NULL;

//4M Bugzilla 77987 19/02/02
/*
  m_pRealTime = NULL;
	m_RTMotion = NO;
*/
  SetRealTime(NULL);
  SetRTMotion(NO);
//<-4M 19/02/02
}

CIcadView::~CIcadView()
{
	delete [] m_pPolyPts;
	CIcadDoc *pDoc=(CIcadDoc *)GetDocument();

	if(pDoc) --pDoc->m_nOpenViews;

	if(pDoc && pDoc->m_dbDrawing && !m_bIsTempView) {
		struct resbuf rb;
		SDS_getvar(NULL,DB_QTILEMODE,&rb,pDoc->m_dbDrawing,&SDS_CURCFG,&SDS_CURSES);
		if(rb.resval.rint==1) {
			if(pDoc->m_dbDrawing->ret_ntabrecs(DB_VPORTTAB,0)>1) {
				if(!m_bDontDelInTab && !cmd_InsideCreateNewViewports && m_pVportTabHip) {
					m_pVportTabHip->set_2(""/*DNT*/);
					pDoc->m_dbDrawing->delhanditem(NULL,m_pVportTabHip,0);
				}
			}
		}
		if(pDoc->m_RegenListView==this) pDoc->m_RegenListView=NULL;
	}

	gr_freeviewll(m_pCurDwgView);

    struct SDS_prevview *pCur, *pToDel;
    for(pCur = m_pZoomPrevB; pCur != NULL;)
	{
		pToDel = pCur; 
		pCur = pCur->next;  
		delete pToDel;
    }
	m_pZoomPrevB=NULL;
    for(pCur = m_pZoomPsPrevB; pCur != NULL;)
	{
		pToDel = pCur; 
		pCur = pCur->next;
		delete pToDel;
    }
	m_pZoomPsPrevB = NULL;

	m_pCurDwgView=NULL;

								// don't leave curView dangling
	if ( this EQ m_pMain->m_pCurView )
		{
		m_pMain->m_pCurView=NULL;
		m_pMain->m_pCurFrame=NULL;
		}

	m_pMain->m_nOpenViews--;

		// this used to check m_pMain, too, but after referencing it
		// if it were ever actually NULL we would have crashed
	if( m_bIsTempView )
		goto exit;

	if(m_pMain->m_nOpenViews==0)
	{
		m_pViewsDoc=m_pMain->m_pCurDoc=NULL;
		if(m_pMain->m_nIsClosing==1)
			goto exit;

		struct resbuf rb;

		db_drawing *flp=NULL;
		if(m_pDocument)	flp=((CIcadDoc*)m_pDocument)->m_dbDrawing;

	  	SDS_getvar(NULL,DB_QWNDLMDI,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
		m_pMain->m_nLastMDIState=rb.resval.rint;
		rb.resval.rint=0;
		SDS_setvar(NULL,DB_QWNDLMDI,&rb,flp,&SDS_CURCFG,&SDS_CURSES,0);
		//*** Update the menus and toolbars.
//		m_pMain->IcadWndAction(ICAD_WNDACTION_NEWMENUS); //*** Moved this to the destructor of CIcadDoc.
	}
// the only effect of this line is to leak memory
//	if(m_pMain->m_nIsClosing==1) return;

exit:
	if(m_pClipRectDC) {
		delete m_pClipRectDC;
		m_pClipRectDC=NULL;
	}
	if(m_pClipRectMemDC) {
		delete m_pClipRectMemDC;
		m_pClipRectMemDC=NULL;
	}
	delete m_pFrameBufferDrawDevice;
	m_pFrameBufferDrawDevice = NULL;

//4M Bugzilla 77987 19/02/02->
/*
    if(m_pRealTime)
	{
		KillTimer(RealTimeTimer_id);
		delete m_pRealTime;
		m_pRealTime = NULL;
	}
*/
  DeleteRealTime();
//<-4M 19/02/02

}

BOOL CIcadView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style|=(CS_BYTEALIGNCLIENT|CS_BYTEALIGNWINDOW|CS_PARENTDC);
	return CView::PreCreateWindow(cs);
}

afx_msg int CIcadView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	m_pMain->m_pCurFrame=(CIcadChildWnd*)GetParentFrame();
	m_pMain->m_pCurView=this;

	if(m_pMain->m_nOpenViews==1) {
		m_pMain->m_pCurFrame->GetParent()->GetWindowRect(&m_pMain->m_rectMDIWin);
	}

//	  m_pViewsDoc=m_pMain->m_pCurDoc=GetDocument();

	if(CView::OnCreate(lpCreateStruct)==(-1))
	   return -1;

    //*** Create the off screen device context
    CDC* pDC=GetDC();
	m_pFrameBufferDrawDevice->create(pDC, CDrawDevice::eDOUBLE_BUFFER, lpCreateStruct->cx, lpCreateStruct->cy, (HPALETTE)SDS_CMainWindow->m_pPalette->GetSafeHandle());
    ReleaseDC(pDC);

	//*** Register the view as an OLE drop target
	m_dropTarget.Register(this);

	if(GetDocument())
		SDS_SetUndo(IC_UNDO_CICADVIEW_CREATE,(void *)this,NULL,NULL,GetDocument()->m_dbDrawing);

	return 0;
}

afx_msg void CIcadView::OnSize(UINT nType, int cx, int cy)
{
	ASSERT_VALID(this);

//4M Bugzilla 77987 19/02/02->
/*
    if(m_pRealTime)
	{
		KillTimer(RealTimeTimer_id);
		delete m_pRealTime;
		m_pRealTime = NULL;
	}
*/
	DeleteRealTime();
//<-4M 19/02/02

	// DP: invalidate last cursor position to prevent garbage cursor on the screen
	m_LastCursorPos.x = -1;
	m_LastCursorPos.y = -1;

// ]-EBATECH(shiba)

	sds_point DummyPt;
	SDS_EndDraggingSS(DummyPt, true);

	if(SDS_IsDragging && m_pMain->m_pDragVarsCur && m_pMain->m_pDragVarsCur->firstdrag) 
	{
		if(m_pMain->m_pDragVarsCur->scnf) 
		{
			int ret;
			if(RTERROR == (ret = (*m_pMain->m_pDragVarsCur->scnf)(m_pMain->m_pDragVarsCur->pt1, m_pMain->m_pDragVarsCur->matx)))
			m_pMain->m_pDragVarsCur->breakout = ret;
		}
		if(SDS_AtCopyMoveCommand) 
			sds_grdraw(m_pMain->m_pDragVarsCur->pt1, m_pMain->m_pDragVarsCur->pt3, -1, 0);
		SDS_DrawDragVectors(NULL);
		m_pMain->m_pDragVarsCur->firstdrag = 0;
	}

	if(m_pMain->m_pCurView != this) 
	{
		extern int SDS_InsideGrread;
		if(SDS_InsideGrread)
			return;
		if(SDSApplication::GetActiveApplication()->GetInputControlBits() & SDS_RSG_NOVIEWCHG)
			return;
		if((SDSApplication::GetActiveApplication()->GetInputControlBits() & SDS_RSG_NODOCCHG) &&
			m_pMain->m_pCurDoc!=GetDocument())
			return;
	}

	CView::OnSize(nType, cx, cy);

	if(cx == 0 || cy == 0) 
		return;

	//*** If we are currently or about to be in-place editing, make sure the
	//*** item doesn't go off the screen because of the toolbar swap.
	struct sds_resbuf rb;

	CIcadDoc* pDoc = (CIcadDoc*)GetDocument();
	if(NULL!=pDoc && (m_bSizeInPlace/* || cmd_InsideOpenCommand || cmd_InsideNewCommand*/))
	{
		SDS_getvar(NULL, DB_QVIEWSIZE, &rb, pDoc->m_dbDrawing, &SDS_CURCFG, &SDS_CURSES);
		if(m_bInPlaceDeactivated)
		{
			rb.resval.rreal = m_dPreInPlaceViewSize;
			m_dPreInPlaceViewSize=0.0;
			m_bSizeInPlace=FALSE;
		}
		else
		{
			if(icadRealEqual(m_dPreInPlaceViewSize, 0.0))
				m_dPreInPlaceViewSize = rb.resval.rreal;
			rb.resval.rreal *= ((((double)cy) / (double)m_iWinY));
			if(cx < m_iWinX)
				rb.resval.rreal *= (((double)m_iWinX) / (double)cx);
		}
		SDS_setvar(NULL, DB_QVIEWSIZE, &rb, pDoc->m_dbDrawing, &SDS_CURCFG,&SDS_CURSES, 1);
		SDS_VarToTab(pDoc->m_dbDrawing, m_pVportTabHip);
	}

	//4M Elias Item:96->
	// Perform a resize unless we are building a new view with VPORTS command,
	//BugZilla No. 78266; 19-08-2002
	COleClientItem* pActiveItem = NULL;
	if(pDoc)
	{
		pActiveItem = pDoc->GetInPlaceActiveItem(SDS_CURVIEW);
		if(pActiveItem != NULL)
			SDS_CURVIEW->m_bLastInPlaceActiveItem = true;
	}	
	//BugZilla No. 78243; 05-08-2002
	//if (!cmd_InsideViewPorts)
	if(!cmd_InsideViewPorts && !cmd_InsideOpenCommand && !cmd_InsideTilemodeCmd && !m_bLastInPlaceActiveItem)
	{
		ASSERT( cy > 0 );
		if(pDoc && pDoc->m_dbDrawing && (m_iWinY > 0))
		{
			if (PrevDrawing == pDoc->m_dbDrawing)
			{
				struct resbuf rb1;
				// drawing resize was controlled only by Height, so ratio depends only on Y
				int OldRatio = m_iWinY; //min( m_iWinX, m_iWinY );
				int Ratio    = cy;		//min ( cx, cy );
				double factor = (double)Ratio / OldRatio;
				
				SDS_TabToVar(pDoc->m_dbDrawing, m_pVportTabHip);
				SDS_getvar(NULL, DB_QVIEWSIZE, &rb1, pDoc->m_dbDrawing, &SDS_CURCFG, &SDS_CURSES);
				rb1.resval.rreal = factor * rb1.resval.rreal;
				SDS_setvar(NULL, DB_QVIEWSIZE, &rb1, pDoc->m_dbDrawing, &SDS_CURCFG, &SDS_CURSES, 1);
				SDS_VarToTab(pDoc->m_dbDrawing, m_pVportTabHip);
			}
		}
		//BugZilla No. 78266; 19-08-2002
		m_bLastInPlaceActiveItem = false;
	}
	//4M Elias Item:96<-
    if((m_iWinX != cx || m_iWinY != cy) && !cmd_InsideOpenCommand && !cmd_InsideNewCommand)
		m_bNeedRedraw = 1;

	if((m_iWinX != cx) || (m_iWinY != cy))
	{
		// Off screen device context stuff.
	    CDC* pDC = GetDC();
		m_pFrameBufferDrawDevice->resize(pDC, cx, cy);
	    ReleaseDC(pDC);
	}

	m_iWinX = cx;
	m_iWinY = cy;

//4M Item:96->
	PrevDrawing=pDoc->m_dbDrawing;
//<-4M Item:96
    if(pDoc && pDoc->m_dbDrawing && m_pCurDwgView)
	{
		struct resbuf rb;
		rb.resval.rpoint[0] = m_iWinX;
		rb.resval.rpoint[1] = m_iWinY;
		rb.resval.rpoint[2] = 0.0;
		rb.restype = RTPOINT;
		SDS_setvar(NULL, DB_QSCREENSIZE, &rb, pDoc->m_dbDrawing, &SDS_CURCFG, &SDS_CURSES, 0);

		gr_initview(pDoc->m_dbDrawing, &SDS_CURCFG, &m_pCurDwgView, NULL);
	}
	// If we are building a new view with VPORTS command.
	// We don't want to have this function do anything in the
	// OnSize message.	It messes up the window focus.
	if(!cmd_InsideViewPorts)
		SetNewView(FALSE, FALSE);

	RECT rect;
	CBrush brush;
	rect.left=0;
	rect.top=0;
	rect.right=cx;
	rect.bottom=cy;
	GetFrameBufferDrawDevice()->fillRect(rect, 256);

	CalcViewPoints(1);

// 	// Modified CyberAge VSB 12/05/2001
// 	// Reason: Implementation of Flyover Snapping.
// 	// Tooltip rectangle to be changed with size. [
// 	if ( ::IsWindow( m_ctlToolTip.m_hWnd ))
// 	{
// 		CRect ClientRect;
// 		GetClientRect(&ClientRect);
// 		m_ctlToolTip.SetToolRect(this,ID_FLYOVER_TOOLTIP,&ClientRect);
// 	}
// 	// Modified CyberAge VSB 12/05/2001 ]
//
	// EBATECH(shiba)-[ 2001-06-12 Clear afterimage by command interrupt OnSize.
	if(SDS_CMainWindow->m_pDragVarsCur)
	{
		CIcadView *pView = SDS_CURVIEW;
		CIcadDoc  *pDoc = SDS_CURDOC;

		POSITION pos=pDoc->GetFirstViewPosition();
		while(pos != NULL)
		{
			pView = (CIcadView *)pDoc->GetNextView(pos);
			if(pView == NULL)
				break;

			if (pView != SDS_CURVIEW)
				SDS_RedrawDrawing(SDS_CURDWG, pView,NULL,1);
		}

		if(m_pMain->m_pDragVarsCur && m_pMain->m_pDragVarsCur->firstdrag != 0)
			m_pMain->m_pDragVarsCur->firstdrag = 0;
		SDS_DidDraggenOnce = 0;
	}
	// ]-EBATECH(shiba) 2001-06-12
}

HDC CIcadView::GetFrameBufferDC()
{
	// DP: These functions exists for compatibility only. Do not use. Use instead GetDrawDevice() and ReleaseDrawDevice()
#ifdef _USE_OPENGL_
	HDC hdc = m_pFrameBufferDrawDevice->getHdc();
#else
	// DP: To be sure that device context of back buffer will be returned.
	int drawBuffer = m_pFrameBufferDrawDevice->setDrawBuffer(CDrawDevice::eBACK);
	HDC hdc = m_pFrameBufferDrawDevice->getHdc();
	m_pFrameBufferDrawDevice->setDrawBuffer((CDrawDevice::EDrawBuffer)drawBuffer);
#endif
	return hdc;
}

CDC* CIcadView::GetFrameBufferCDC()
{
	// DP: These functions exists for compatibility only. Do not use. Use instead GetDrawDevice() and ReleaseDrawDevice()
#ifdef _USE_OPENGL_
	CDC* pDC = m_pFrameBufferDrawDevice->getCDC();
#else
	// DP: To be sure that device context of back buffer will be returned.
	int drawBuffer = m_pFrameBufferDrawDevice->setDrawBuffer(CDrawDevice::eBACK);
	CDC* pDC = m_pFrameBufferDrawDevice->getCDC();
	m_pFrameBufferDrawDevice->setDrawBuffer((CDrawDevice::EDrawBuffer)drawBuffer);
#endif
	return pDC;
}

CDC *
CIcadView::GetTargetDeviceCDC( void )
	{
	if ( IsPrinting() )
		return getPrinterDC();
	else
		return GetFrameBufferCDC();
	}

void CIcadView::CalcViewPoints(int UpdateTab)
{
	if(this==NULL) return;

	ASSERT_VALID( this );

	if(cmd_InsideOpenCommand || cmd_InsideNewCommand || cmd_InsideCreateNewViewports) return;

	db_drawing *flp=NULL;
	if(m_pDocument)	flp=((CIcadDoc*)m_pDocument)->m_dbDrawing;

	// Setup the ll and ur variables in the vport table
	if(flp && m_pVportTabHip) {
		sds_point pt1,pt2;
		RECT rectParent,rectThis;

		if(GetParentFrame()->IsZoomed() ||
			(m_pDocument && ((CIcadDoc*)m_pDocument)->m_nOpenViews==1 && !cmd_InsideViewPorts)) {
			// Keep the MDI window rect up to date.
			if(!cmd_CalcChildRect && GetParentFrame()->IsZoomed()) {
//				GetParentFrame()->GetWindowRect(&SDS_CMainWindow->m_rectMDIWin);
				GetWindowRect(&SDS_CMainWindow->m_rectMDIWin);
				SDS_CMainWindow->m_rectMDIWin.left+=2;
				SDS_CMainWindow->m_rectMDIWin.bottom+=2;
				SDS_CMainWindow->m_rectMDIWin.right+=2;
				SDS_CMainWindow->m_rectMDIWin.top+=2;
			}

			pt1[0]=0.0;
			pt1[1]=0.0;
			pt1[2]=0.0;
			pt2[0]=1.0;
			pt2[1]=1.0;
			pt2[2]=0.0;
		} else {
			// Keep the MDI window rect up to date.
			if(!cmd_CalcChildRect) {
				GetParentFrame()->GetParent()->GetWindowRect(&SDS_CMainWindow->m_rectMDIWin);
				SDS_CMainWindow->m_rectMDIWin.left+=4;
				SDS_CMainWindow->m_rectMDIWin.top+=4;
			}

			GetParentFrame()->GetWindowRect(&rectThis);
			GetParentFrame()->GetParent()->GetWindowRect(&rectParent);

			// if the borders are within 5 pixels consiter them the same.
			if(abs(rectThis.left  -rectParent.left)<=5)   rectThis.left=rectParent.left;
			if(abs(rectThis.bottom-rectParent.bottom)<=5) rectThis.bottom=rectParent.bottom;
			if(abs(rectThis.right -rectParent.right)<=5)  rectThis.right=rectParent.right;
			if(abs(rectThis.top   -rectParent.top)<=5)	  rectThis.top=rectParent.top;

			pt1[0]=((double)(rectThis.left-rectParent.left)/(double)(rectParent.right-rectParent.left));
			pt1[1]=1.0-(((double)(rectThis.bottom-rectParent.top)/(double)(rectParent.bottom-rectParent.top)));
			pt1[2]=0.0;

			// Just make sure that the new window fits within the enclosing window
			// Besides they will just throw an exception if they are over 1.0
			pt1[0]= (pt1[0]>1.0) ? 1.0 : pt1[0];
			pt1[1]= (pt1[1]>1.0) ? 1.0 : pt1[1];

			pt2[0]=((double)(rectThis.right-rectParent.left)/(double)(rectParent.right-rectParent.left));
			pt2[1]=1.0-(((double)(rectThis.top-rectParent.top)/(double)(rectParent.bottom-rectParent.top)));
			pt2[2]=0.0;

			// Just make sure that the new window fits within the enclosing window
			// Besides they will just throw an exception if they are over 1.0
			pt2[0]= (pt2[0]>1.0) ? 1.0 : pt2[0];
			pt2[1]= (pt2[1]>1.0) ? 1.0 : pt2[1];

		}
		struct resbuf tilemode;
		SDS_getvar(NULL,DB_QTILEMODE,&tilemode,flp,&SDS_CURCFG,&SDS_CURSES);
		if(UpdateTab && tilemode.resval.rint==1 && !cmd_InsideCreateNewViewports) {
			m_pVportTabHip->set_10(pt1);
			m_pVportTabHip->set_11(pt2);
		}
	}
}

/////////////////////////////////////////////////////////////////////
// CIcadView drawing

afx_msg BOOL CIcadView::OnEraseBkgnd(CDC* pDC)
{
	return 1;
}

afx_msg void CIcadView::OnPaint(void)
{
	CRect rectUpdate;
	PAINTSTRUCT ps;
	CDC *pDC;

	ASSERT_VALID( this );

	if(!IsWindowVisible()) return;

	if(SDS_IsDragging && m_pMain->m_pDragVarsCur && m_pMain->m_pDragVarsCur->firstdrag && m_bNeedRedraw) {
		if(m_pMain->m_pDragVarsCur->scnf) {
			int ret;
			if(RTERROR==(ret=(*m_pMain->m_pDragVarsCur->scnf)(m_pMain->m_pDragVarsCur->pt1,m_pMain->m_pDragVarsCur->matx)))
			m_pMain->m_pDragVarsCur->breakout=ret;
		}
		if(SDS_AtCopyMoveCommand) sds_grdraw(m_pMain->m_pDragVarsCur->pt1,m_pMain->m_pDragVarsCur->pt3,-1,0);
		SDS_DrawDragVectors(NULL);
		m_pMain->m_pDragVarsCur->firstdrag=0;
	}

	if(GetUpdateRect(rectUpdate)!=0) {
		SDS_SetClipInDC(this,m_pVportTabHip,TRUE);

		pDC=BeginPaint(&ps);

//		CPalette *oldpal1=pDC->SelectPalette(SDS_CMainWindow->m_pPalette,FALSE);
//		CPalette *oldpal2=m_pMemDC->SelectPalette(SDS_CMainWindow->m_pPalette,FALSE);

		if(m_pMain->m_pCurView==NULL || m_pDocument==NULL || SDS_CMainWindow->m_pCurDoc==NULL) {
			m_pDocument=SDS_CMainWindow->m_pCurDoc=GetDocument();
			m_pMain->m_pCurFrame=(CIcadChildWnd*)GetParentFrame();
			m_pMain->m_pCurView=this;
		}

		if(!m_pDocument || !((CIcadDoc*)m_pDocument)->m_dbDrawing) {
			EndPaint(&ps);
			return;
		}

		cmd_InsideOnPaint=TRUE;

#ifdef _USE_OPENGL_  
		m_bNeedRedraw=1;
#endif // _USE_OPENGL_
		if(m_bNeedRedraw) {
			if(!(GetStyle()&(WS_HSCROLL|WS_VSCROLL))) 
				UpdateScrollBars();
			m_bNeedRedraw=0;

			if(cmd_InsideRedrawVPpaint) return;

			struct resbuf tilemode;
			struct resbuf cvport;
			SDS_getvar(NULL,DB_QCVPORT,&cvport,((CIcadDoc*)m_pDocument)->m_dbDrawing,&SDS_CURCFG,&SDS_CURSES);
			SDS_getvar(NULL,DB_QTILEMODE,&tilemode,((CIcadDoc*)m_pDocument)->m_dbDrawing,&SDS_CURCFG,&SDS_CURSES);
			if(tilemode.resval.rint==0 && cvport.resval.rint>1 && m_pVportTabHip->ret_type()==DB_VIEWPORT) {
				SDS_VPeedToView(((CIcadDoc*)m_pDocument)->m_dbDrawing,m_pVportTabHip,&m_pCurDwgView,this);
				SDS_SetClipInDC(this,m_pVportTabHip,FALSE);
				cmd_pspace();
					if (m_iViewMode == kNormal) {
					if(RTERROR==SDS_RedrawDrawing(((CIcadDoc*)m_pDocument)->m_dbDrawing,this,(CIcadDoc*)m_pDocument,1))
						return;
				}
				else {
					CWindowHLRenderer hlRenderer(this);
					hlRenderer.render();
				}
				cmd_mspace();
			} else {
				struct gr_viewvars viewvarsp;
				SDS_ViewToVars2Use(this,((CIcadDoc*)m_pDocument)->m_dbDrawing,&viewvarsp);
				gr_initview(((CIcadDoc*)m_pDocument)->m_dbDrawing,&SDS_CURCFG,&m_pCurDwgView,&viewvarsp);
				if (m_iViewMode == kNormal) {
					if(RTERROR==SDS_RedrawDrawing(((CIcadDoc*)m_pDocument)->m_dbDrawing,this,(CIcadDoc*)m_pDocument,1))
						return;
				}
				else {
					CWindowHLRenderer hlRenderer(this);
					hlRenderer.render();
				}
			}

		} else {

			SDS_SetClipInDC(this, m_pVportTabHip, TRUE);
			m_pFrameBufferDrawDevice->beginDraw(pDC, m_pCurDwgView);
			m_pFrameBufferDrawDevice->copyToFront(rectUpdate.left, rectUpdate.top, rectUpdate.Width(), rectUpdate.Height());
			m_pFrameBufferDrawDevice->endDraw();
		}

//		pDC->SelectPalette(oldpal1,TRUE);
//		m_pMemDC->SelectPalette(oldpal2,TRUE);
		SDS_SetClipInDC(this,m_pVportTabHip,FALSE);
		EndPaint(&ps);
		cmd_InsideOnPaint=FALSE;
	}
	return;
}

void CIcadView::OnDraw(CDC* pDC)
{
	ASSERT_VALID( this );

	if(!IsWindowVisible()) return;

	CRect rectUpdate;
	SDS_SetClipInDC(this,m_pVportTabHip,FALSE);
	CPalette *oldpal=pDC->SelectPalette(SDS_CMainWindow->m_pPalette,TRUE);
	GetWindowRect(rectUpdate);

	if(m_pMain->m_pCurView==NULL || m_pDocument==NULL || SDS_CMainWindow->m_pCurDoc==NULL) {
		m_pDocument=SDS_CMainWindow->m_pCurDoc=GetDocument();
		m_pMain->m_pCurFrame=(CIcadChildWnd*)GetParentFrame();
		m_pMain->m_pCurView=this;
	}

	if(!m_pDocument || !((CIcadDoc*)m_pDocument)->m_dbDrawing) return;

	if(m_bNeedRedraw) {
		if(!(GetStyle()&(WS_HSCROLL|WS_VSCROLL))) 
			UpdateScrollBars();
		m_bNeedRedraw=0;

		struct resbuf tilemode;
		struct resbuf cvport;
		SDS_getvar(NULL,DB_QCVPORT,&cvport,((CIcadDoc*)m_pDocument)->m_dbDrawing,&SDS_CURCFG,&SDS_CURSES);
		SDS_getvar(NULL,DB_QTILEMODE,&tilemode,((CIcadDoc*)m_pDocument)->m_dbDrawing,&SDS_CURCFG,&SDS_CURSES);
		if(tilemode.resval.rint==0 && cvport.resval.rint>1 && m_pVportTabHip->ret_type()==DB_VIEWPORT) {
			SDS_VPeedToView(((CIcadDoc*)m_pDocument)->m_dbDrawing,m_pVportTabHip,&m_pCurDwgView,this);
			SDS_SetClipInDC(this,m_pVportTabHip,FALSE);
		} else {
			struct gr_viewvars viewvarsp;
			SDS_ViewToVars2Use(this,((CIcadDoc*)m_pDocument)->m_dbDrawing,&viewvarsp);
			gr_initview(((CIcadDoc*)m_pDocument)->m_dbDrawing,&SDS_CURCFG,&m_pCurDwgView,&viewvarsp);
		}
		SDS_RedrawDrawing(((CIcadDoc*)m_pDocument)->m_dbDrawing,this,(CIcadDoc*)m_pDocument,1);
	} else {
		SDS_SetClipInDC(this, m_pVportTabHip, TRUE);
		m_pFrameBufferDrawDevice->beginDraw(pDC, m_pCurDwgView);
		m_pFrameBufferDrawDevice->copyToFront(rectUpdate.left, rectUpdate.top, rectUpdate.Width(), rectUpdate.Height());
		m_pFrameBufferDrawDevice->endDraw();			
	}
	SDS_SetClipInDC(this,m_pVportTabHip,FALSE);
}

CIcadCntrItem* CIcadView::HitTestItems(CPoint point)
{
	CIcadDoc* pDoc = GetDocument();
	CIcadCntrItem* pItemHit = NULL;
	POSITION pos = pDoc->GetStartPosition();
	CRect rect;
	while(pos!=NULL)
	{
		CIcadCntrItem* pItem = (CIcadCntrItem*)pDoc->GetNextItem(pos);
		if(NULL==pItem || !pItem->IsKindOf(RUNTIME_CLASS(CIcadCntrItem)) ||
			NULL==pItem->m_pHandItem)
		{
			continue;
		}
		pItem->GetCurRect(this,&rect);
		if(rect.PtInRect(point))
		{
			pItemHit = pItem;
		}
	}
	return pItemHit;	// return top item at point
}

void CIcadView::SetSelection(CIcadCntrItem* pItem, BOOL bForceRedrawAll)
{
	//*** Don't try to select an item that has not been created yet.
	if(NULL!=pItem && NULL==pItem->m_pHandItem)
	{
		return;
	}
	BOOL bDidRedrawAll = FALSE;
	// close in-place active item
	if(pItem==NULL || m_pSelection!=pItem)
	{
		CIcadDoc* pDoc = (CIcadDoc*)GetDocument();
		if(NULL!=pDoc)
		{
			COleClientItem* pActiveItem = pDoc->GetInPlaceActiveItem(this);
			if(pActiveItem != NULL && pActiveItem != pItem)
			{
				pActiveItem->Close();
				return;
			}
			//*** Update the grip selection set.
			sds_name ename;
			SDS_FreeNodeList(pDoc); //*** FYI - This function does a redraw.
			bDidRedrawAll=TRUE;
			sds_ssfree(pDoc->m_pGripSelection);
			if(NULL!=pItem)
			{
				ename[0]=(long)pItem->m_pHandItem;
				ename[1]=(long)pDoc->m_dbDrawing;
				sds_ssadd(ename,NULL,pDoc->m_pGripSelection);
			}
		}
	}
	CIcadCntrItem* pOldItem = m_pSelection;
	m_pSelection = pItem;
	if(pOldItem!=NULL && pOldItem!=m_pSelection)
	{
		//*** Redraw the item being deselected.
		pOldItem->Draw(this,GetFrameBufferCDC());
	}
	if(bForceRedrawAll && !bDidRedrawAll)
	{
		//*** Force a redraw of all entities including any behind the object.
		sds_redraw(NULL,IC_REDRAW_DONTCARE);
	}
	else
	{
		//*** Just update the view.
		Invalidate();
	}
}

/////////////////////////////////////////////////////////////////////
// Change the cursor when in the view

afx_msg BOOL CIcadView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if(nHitTest != HTCLIENT)
	{
//4M Bugzilla 77987 19/02/02
/*
        if(m_pRealTime)
		{
			KillTimer(RealTimeTimer_id);
			delete m_pRealTime;
			m_pRealTime = NULL;
		}
		m_RTMotion = NO;
*/
// We leave the behaviour for ZOOM_WHEEL intact
     if (m_RTMotion==ZOOM_WHEEL){
        DeleteRealTime();
        SetRTMotion(NO);
     }
//<-4M 19/02/02

        sds_point DummyPt;
		SDS_EndDraggingSS(DummyPt, true);
	}

	if(SDS_CURVIEW!=this || !m_pMain->m_bIsAppActive)
	{
		m_prevCursor=(::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW)));
	}
	else if(HTCLIENT==nHitTest && m_idcCursor)
	{
		POINT point;
		GetCursorPos(&point);
		ScreenToClient(&point);
		CIcadCntrItem* pItemHit = HitTestItems(point);
		if(NULL!=m_pSelection && m_pSelection==pItemHit)
		{
			//*** If we are in a command or IPA, dont worry about the cursor.
			if(m_pSelection->IsInPlaceActive() || SDS_AtCmdLine==0)
			{
				return FALSE;
			}
//			m_prevCursor=(::SetCursor(AfxGetApp()->LoadCursor(IDC_ICAD_OLEMOVE)));
			m_pSelection->m_pTracker->SetCursor(pWnd,nHitTest);
		}
		else
		{
			bool bShowCursor = true;
			if(bool(UserPreference::SnapShowOsnapCursorDecorationPreference) == false)
			{
				switch(m_idcCursor) {
					case IDC_ICAD_SNAP:
					case IDC_ICAD_CROSS:
					case IDC_CENTER:
					case IDC_ENDPOINT:
					case IDC_INSERT:
					case IDC_INTERSECT:
					case IDC_PERPEN:
					case IDC_MIDPOINT:
					case IDC_NEAR:
					case IDC_NODE:
					case IDC_QUAD:
					case IDC_TANGENT:
					case IDC_ICAD_OS_INT3D:
					case IDC_ICAD_OS_INTVS:
					case IDC_ICAD_OS_INT2D:
						bShowCursor = false;
				}
			}
			m_prevCursor=::SetCursor(bShowCursor ? AfxGetApp()->LoadCursor(m_idcCursor) : NULL);
		}
	}
	else if(HTHSCROLL==nHitTest || HTVSCROLL==nHitTest || m_idcCursor==0)
	{
		m_prevCursor=(::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW)));
	}
	if(!m_pMain->m_pCmdBar->IsWindowVisible())
	{
		m_pMain->m_StatusBar.TextOut(m_pMain->m_pCmdStr->GetBuffer(0));
	}
	else
	{
//4M Spiros Item:31->
	if (MdMacro.GetLength())
		m_pMain->m_StatusBar.TextOut(MdMacro);
	else
//4M Spiros Item:31<-
        m_pMain->m_StatusBar.TextOut(ResourceString(IDC_ICADVIEW_READY_1, "Ready" ));
	}
	return TRUE;
}

afx_msg void CIcadView::OnCaptureChanged(CWnd* pWnd)
{
	if(NULL!=m_prevCursor)
	{
		::SetCursor(m_prevCursor);
		m_prevCursor=NULL;
	}

//4M Bugzilla 77987 19/02/02->
/*
    if(m_pRealTime)
	{
		KillTimer(RealTimeTimer_id);
		delete m_pRealTime;
		m_pRealTime = NULL;
	}
	m_RTMotion = NO;
*/
// We leave the bahaviour for ZOOM_WHEEL intact
  if (m_RTMotion==ZOOM_WHEEL){
     DeleteRealTime();
     SetRTMotion(NO);
  }
//<-4M 19/02/02
    sds_point DummyPt;
	SDS_EndDraggingSS(DummyPt, true);
}

/////////////////////////////////////////////////////////////////////
// Mouse move events and character input

afx_msg void CIcadView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(nChar == 27)
	{
//4M Bugzilla 77987 19/02/02->
/*
        if(m_pRealTime)
		{
			KillTimer(RealTimeTimer_id);
			delete m_pRealTime;
			m_pRealTime = NULL;
		}
		m_RTMotion = NO;

*/
//We leave behaviour for ZOOM_WHEEL intact
     if (GetRTMotion()==ZOOM_WHEEL){
        DeleteRealTime();
        SetRTMotion(NO);
     }
//<-4M 19/02/02
        processOsnapChar( nChar );
	}
	else if(nChar == 9 ) // TAB
	{
		processOsnapChar( nChar );
	}

	if(NULL!=m_pMain && m_pMain->m_bSentChar==FALSE)
		SDS_SendMessage(WM_CHAR,nChar,nFlags);
}

void CIcadView::SetNewView(bool bSetFocus, bool callback)
{
	ASSERT_VALID(this);

	int dragflag = 0;
	if(m_pMain->m_pCurDoc != GetDocument() &&
		SDS_IsDragging && IDS_GetThreadController()->GetAtWaitLoop() &&
	   m_pMain->m_pDragVarsCur && m_pMain->m_pDragVarsCur->mode != -5 && SDS_AtCopyMoveCommand)
	{
		if(m_pMain->m_pDragVarsCur->vects)
		{
			dragflag = 1;
			sds_grdraw(SDS_CMainWindow->m_pDragVarsCur->pt1, SDS_CMainWindow->m_pDragVarsCur->pt3, -1, 0);
			SDS_DrawDragVectors(NULL);
//			m_pMain->m_pDragVarsCur->firstdrag=0;
		}
	}

	if(!m_pMain->m_nIsClosing && m_pMain->m_pCurDoc)
	{
		CString strDrawingName = m_pMain->m_pCurDoc->GetPathName();
		if(strDrawingName.IsEmpty())
			strDrawingName = m_pMain->m_pCurDoc->GetTitle();
		SDS_CallUserCallbackFunc(SDS_CBVIEWDOCCHG, (void*)(char*)(LPCTSTR)strDrawingName, (void*)GetSafeHwnd(), NULL);
	}

	if(ScrollBarsOn() && !m_bIsTempView && !m_pMain->m_nIsClosing && IsWindow(m_pMain->m_pCurView->GetSafeHwnd()))
#ifdef _LAYOUTTABS_
		m_pMain->m_pCurView->EnableScrollBarCtrl(SB_BOTH, FALSE);
#else
		m_pMain->m_pCurView->EnableScrollBar(SB_BOTH,ESB_DISABLE_BOTH);
#endif

	if(!m_bIsTempView && bSetFocus)
		SetFocus();

	if(NULL == m_pMain->m_pCurView || !IsWindow(GetSafeHwnd()) || !IsWindow(m_pMain->m_pCurView->GetSafeHwnd()) || GetDocument() == NULL)
		return;

	if(!m_bIsTempView)
	{
		// Undraw the cursor in the last window.
		if(m_pMain->m_pCurView->m_CursorOn && m_pMain->m_pCurView->GetFrameBufferCDC()) 
		{
			IcadCursor::AppCursor().DrawCursor(m_pMain->m_pCurView->m_LastCursorPos, m_pMain->m_pCurView->m_LastCursorType, m_pMain->m_pCurView);
			m_pMain->m_pCurView->m_CursorOn = 0;
		}
		m_pMain->m_pCurView->m_idcCursor=IDC_ICAD_PICKBOX;
	}
	// We could be switching to a new drawing so change flp here.
	SDS_SetClipInDC(m_pMain->m_pCurView, m_pMain->m_pCurView->m_pVportTabHip, TRUE);
	m_pMain->m_pCurFrame = (CIcadChildWnd*)GetParentFrame();
	m_pMain->m_pCurView = this;
	if(m_pViewsDoc == NULL && GetDocument()) 
		++GetDocument()->m_nOpenViews;
	m_pViewsDoc = m_pMain->m_pCurDoc = GetDocument();
	db_drawing *flp=flp=GetDocument()->m_dbDrawing;
	if(flp == NULL)
		return;

	// This new callback turns out to be more useful than SDS_CBDOCVIEWCHG above
	//
	// arg1 = new drawing name
	// arg2 = new HWND
	// arg3 = old HWND (one we switched away from)
	//
	if(callback && !m_pMain->m_nIsClosing && m_pMain->m_pCurDoc)
		m_pMain->DocChangeCallback(this);

	struct sds_resbuf rb;
	struct resbuf tilemode;
	SDS_getvar(NULL, DB_QTILEMODE, &tilemode, flp, &SDS_CURCFG, &SDS_CURSES);
	struct resbuf cvport;
	SDS_getvar(NULL, DB_QCVPORT, &cvport, flp, &SDS_CURCFG, &SDS_CURSES);

	if(m_pVportTabHip == NULL)
	{
		BOOL bCreated;
		if(!SDS_CMainWindow->m_rectCreateView)
		{
			SDS_CMainWindow->m_rectCreateView = new CRect(0, 0, 1, 1);
			bCreated = TRUE;
		}
		SDS_OnFirstView(this, 0);
		if (bCreated)
		{
			delete SDS_CMainWindow->m_rectCreateView;
			SDS_CMainWindow->m_rectCreateView = NULL;
		}
	}

	SDS_TabToVar(flp, m_pVportTabHip);

	if(SDS_AtCopyMoveCommand && SDS_CMainWindow->m_pDragVarsCur && m_pDragVects == NULL)
	{
		POSITION pos = m_pViewsDoc->GetFirstViewPosition();
		while (pos != NULL)
		{
			CIcadView* pView = (CIcadView*)m_pViewsDoc->GetNextView(pos);
			ASSERT_KINDOF(CIcadView, pView);

			SDS_CMainWindow->m_pDragVarsCur->vects = SDS_SSToVects(SDS_CMainWindow->m_pDragVarsCur->nmSelSet, flp, pView->m_pCurDwgView);
			pView->m_pDragVects = SDS_CMainWindow->m_pDragVarsCur->vects;
		}
	}

	// We have to se the SCREENSIZE outside of SDS_TabToVar()
	// The screen size is NOT saved in each ACAD vport.
	rb.restype = RTPOINT;
	if(tilemode.resval.rint == 0 && cvport.resval.rint > 1)
	{
		int x1, x2, y1, y2;
		SDS_GetRectForVP(this, m_pVportTabHip, &x1, &x2, &y1, &y2);
		rb.resval.rpoint[0] = x2 - x1;
		rb.resval.rpoint[1] = y2 - y1;
	}
	else
	{
		rb.resval.rpoint[0] = m_iWinX;
		rb.resval.rpoint[1] = m_iWinY;
	}
	rb.resval.rpoint[2] = 0.0;
	SDS_setvar(NULL, DB_QSCREENSIZE, &rb, flp, &SDS_CURCFG, &SDS_CURSES, 0);

	if(tilemode.resval.rint == 0 && m_pVportTabHip && (m_pVportTabHip->ret_type() == DB_VIEWPORT))
		cmd_MakeVpActive(this, m_pVportTabHip, flp);

	m_pMain->m_pCurFrame = (CIcadChildWnd*)GetParentFrame();
	m_pMain->m_pCurView = this;
	m_pViewsDoc = m_pMain->m_pCurDoc = GetDocument();
	m_idcCursor = m_pMain->m_ViewCursorType;

	if(!m_bIsTempView)
	{
		if (ScrollBarsOn())
#ifdef _LAYOUTTABS_
			EnableScrollBarCtrl(SB_BOTH, TRUE);
#else
			EnableScrollBar(SB_BOTH,ESB_ENABLE_BOTH);
#endif
		extern char *SDS_UpdateSetvar;	/*D.Gavrilov*/// We'll use it in some of
		SDS_UpdateSetvar = NULL;			// the following calls of IcadWndAction
		// Update the status bar.
		m_pMain->IcadWndAction(ICAD_WNDACTION_UDSTLAY);
		m_pMain->IcadWndAction(ICAD_WNDACTION_UDSTLTP);
		m_pMain->IcadWndAction(ICAD_WNDACTION_UDSTCOL);
		m_pMain->IcadWndAction(ICAD_WNDACTION_UDSTSTY);		// EBATECH (Style)
		m_pMain->IcadWndAction(ICAD_WNDACTION_UDSTDIM);		// EBATECH (DimStyle)
		m_pMain->IcadWndAction(ICAD_WNDACTION_UDSTLW);		// EBATECH (LineWeight)
//		m_pMain->m_StatusBar.UpdateVariablePanes(NULL);
		m_pMain->IcadWndAction(ICAD_WNDACTION_STATCHANGE);	// use it instead of m_StatusBar (for code style)
		// Also menuchecks and toolbars
		m_pMain->IcadWndAction(ICAD_WNDACTION_UDCHKSTAT);
		m_pMain->IcadWndAction(ICAD_WNDACTION_UDTBSTATE);

		if(dragflag) 
		{
			sds_grdraw(SDS_CMainWindow->m_pDragVarsCur->pt1, SDS_CMainWindow->m_pDragVarsCur->pt3, -1, 0);
			SDS_DrawDragVectors(NULL);
		}
		SDS_SetClipInDC(this, m_pVportTabHip, FALSE);

	}
}

afx_msg void CIcadView::OnMouseMove(UINT nFlags, CPoint point)
	{
	DWORD lParam;
	static db_drawing *Lastflp;
	struct resbuf rb;
	db_drawing *flp=NULL;
	extern bool SDS_bDraggingSS;

	// Parse out multiple mouse moves to get just the latest one
	bool	bFoundNewerMouseMove = false;
	MSG		msgLookAhead;
	HWND	hWnd = GetSafeHwnd();

	while(::PeekMessage(&msgLookAhead, hWnd, WM_MOUSEMOVE, WM_MOUSEMOVE, PM_REMOVE))
		bFoundNewerMouseMove = true;	// Just get the latest mouse move

	if(bFoundNewerMouseMove)
	{
		// EBATECH(CNBR) 2001-06-24 add CAST operator
		nFlags = (UINT)msgLookAhead.wParam;
		point.x = (int)(short)LOWORD(msgLookAhead.lParam);
		point.y = (int)(short)HIWORD(msgLookAhead.lParam);
	}

	if(!(nFlags & MK_LBUTTON) && SDS_bDraggingSS)
	{
		sds_point	DummyPt;
		SDS_EndDraggingSS(DummyPt, true);
	}

	if(SDS_EditPropsSS_DblClk[0])
	{
		sds_ssfree(SDS_EditPropsSS_DblClk);	/*D.G.*/// allocated in OnLButtonDown by SDS_sscpy
		SDS_EditPropsSS_DblClk[0] = 0L;
	}

	if(m_pDocument)
		{
		flp=((CIcadDoc*)m_pDocument)->m_dbDrawing;
		}

	m_bEatNextMouse=FALSE;
	m_bHaveMagneticGrip = false;

	if( m_pMain->m_pCurView==NULL ||
		m_pMain->m_pCurDoc==NULL )
		{
		m_pViewsDoc=m_pMain->m_pCurDoc=GetDocument();
		m_pMain->m_pCurFrame=(CIcadChildWnd*)GetParentFrame();
		m_pMain->m_pCurView=this;
		}

	if( m_pMain->m_pDragVarsCur &&
		m_pMain->m_pCurDoc!=GetDocument())
		{
		return;
		}

	struct gr_view *view=m_pCurDwgView;
	if(view==NULL)
		{
		return;
		}

	if( m_pMain->m_pCurView!=this ||
		!m_pMain->m_bIsAppActive)
		{
		return;
		}

	if(m_RTMotion == ZOOM_WHEEL)
	{
//4M Bugzilla 77987 19/02/02->
/*
        KillTimer(RealTimeMouseWheelTimerID);
		delete m_pRealTime;
		m_pRealTime = NULL;
		m_RTMotion = NO;
*/
     DeleteRealTime(RealTimeMouseWheelTimerID);
     SetRTMotion(NO);
//<-4M 19/02/02
    }


//4M Bugzilla 77987 19/02/02->
/*
    if(m_pRealTime && !m_pRealTime->m_bContinuousOrbitStarted && m_pRealTime->Continue(point))
*/
	if(GetRealTime() && !GetRealTime()->m_bContinuousOrbitStarted && GetRealTime()->Continue(point))
//<-4M 19/02/02
    // EBATECH(shiba)-[2001-06-12 Clear afterimage by command interrupt RTZOOMPAN etc.
//		return;
	{
		if ( m_pMain->m_pDragVarsCur && m_pMain->m_pDragVarsCur->firstdrag != 0)
		{
			m_pMain->m_pDragVarsCur->firstdrag = 0;
		}
		SDS_DidDraggenOnce = 0;
		return;
	}
	// ]-EBATECH(shiba) 2001-06-12

	SDS_AtMouseMove=1;

	if( this!=(((CMDIFrameWnd*)AfxGetApp()->m_pMainWnd)->GetActiveFrame())->GetActiveView())
		{
		((CIcadChildWnd*)GetParentFrame())->ActivateFrame();
		}

	sds_point pt;
	int ret=RTNORM;

	CPoint point_real(point);	/*D.Gavrilov*/// We'll change it if we are jump to a grip point.

	bool bCursorUndraw = false;

	if( !m_pMain->m_bCustomize &&
		GetActiveCommandQueue()->IsEmpty() )
		{
		if(RTERROR==SDS_CallUserCallbackFunc(SDS_CBMOUSEMOVE, &point.x, &point.y,NULL))
			{
			return;
			}

		// No need to go through all this if there are no callbacks registered.
		if (SDS_SetAUserCallback == TRUE)
			{
			// Set up callback for UCS coord.
			sds_point pt;
			struct resbuf rb;
			db_drawing *flp=NULL;
			struct gr_view *view=m_pCurDwgView;
			if(m_pDocument)
				{
				flp=((CIcadDoc*)m_pDocument)->m_dbDrawing;
				}

			if(view!=NULL)
				{
				SDS_getvar(NULL,DB_QCOORDS,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
				gr_pix2rp(view,point.x,point.y,&pt[0],&pt[1]);
				gr_rp2ucs(pt,pt,view);

				if(RTERROR==SDS_CallUserCallbackFunc(SDS_CBMOUSEMOVEUCS,(void *)&pt[0],(void *)&pt[1],(void *)&pt[2]))
					{
					return;
					}
				}
			}



		// UnDraw the Cursor.
		if(IDS_GetThreadController()->GetAtWaitLoop() &&
			m_CursorOn &&
			(m_rectMspace.PtInRect(point) || !m_pClipRectDC))
			{
			IcadCursor::AppCursor().DrawCursor(m_LastCursorPos,m_LastCursorType,this);
			m_CursorOn=0;
			bCursorUndraw = true;
			}

	/*D.Gavrilov*/// If we are not far from some grip point let's jump to it.
	sds_name	entityName;
	sds_point	gripPt;
	int			CaptureDistance = IcadCursor::AppCursor().GetAttractionDistance();
	if(IcadCursor::AppCursor().isEnableGripsAttraction())
	{
		m_pViewsDoc->m_nodeList.begin();
		while(m_pViewsDoc->m_nodeList.getPair(entityName, gripPt))
		{
			int			x, y;
			sds_point	gripPt_rp;
			gr_ucs2rp(gripPt, gripPt_rp, view);
			gr_rp2pix(view, gripPt_rp[0], gripPt_rp[1], &x, &y);
			if(abs(point.x - x) < CaptureDistance && abs(point.y - y) < CaptureDistance)
			{
				point_real.x = x;
				point_real.y = y;
				ic_ptcpy(pt, gripPt);
				ic_ptcpy(m_magneticGrip, pt);
				m_bHaveMagneticGrip = true;
				break;
			}
		}
	}

	if(!m_bHaveMagneticGrip)
	{
		gr_pix2rp(view, point.x, point.y, &pt[0], &pt[1]);
		gr_rp2ucs(pt, pt, view);
	}

		if( !SDS_AtEntGet &&
			!SDS_AtCmdLine)
		{
			SDS_SnapPt(pt,pt);
			/*D.Gavrilov*/// Let the cursor snap too.
			sds_point pt_rp;
			gr_ucs2rp(pt, pt_rp, view);
			gr_rp2pix(view, pt_rp[0], pt_rp[1], (int*)&point_real.x, (int*)&point_real.y);
			//Bugzilla No. 78366 ; 03-03-2003 [
			struct resbuf rbOsmode;
			SDS_getvar(NULL,DB_QOSMODE,&rbOsmode,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			if( IDS_GetInputEventQueue()->IsWaiting() && rbOsmode.resval.rint && !(rbOsmode.resval.rint & IC_OSMODE_OFF))
			//if( IDS_GetInputEventQueue()->IsWaiting()) // call snapping only if we're waiting for the input
			//Bugzilla No. 78366 ; 03-03-2003 ]
			{
				COsnapManagerIcad *pOsnapManager = ((CEngineApplication*)SDSApplication::GetEngineApplication())->getOsnapManager();
				pOsnapManager->processPoint( this, pt_rp );
			}
		}

		if(m_pMain->m_fIsCtrlDown==0)
			{
			ic_ptcpy(m_pMain->m_pKeyDragPt,pt);
			}


		if( m_rectMspace.PtInRect(point_real) ||
			!m_pClipRectDC)
			{
			SDS_getvar(NULL,DB_QCOORDS,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
			if(rb.resval.rint)
				{
				UpdateCoords(pt,rb.resval.rint);
				}
			}

		if( SDS_IsDragging &&
			(m_rectMspace.PtInRect(point_real) || !m_pClipRectDC) &&
			bCursorUndraw &&
			m_pMain->m_pDragVarsCur &&
			(m_pMain->m_pDragVarsCur->mode!=-5))
			{

			if( m_pMain->m_pDragVarsCur->vects==NULL &&
				cmd_InsideSds_draggen )
				{
				if( m_pMain->m_pDragVarsCur->nmSelSet[0] &&
					m_pMain->m_pDragVarsCur->nmSelSet[1])
					{
					POSITION pos=SDS_CURDOC->GetFirstViewPosition();
					while (pos!=NULL)
						{
						CIcadView *pView=(CIcadView *)SDS_CURDOC->GetNextView(pos);
						ASSERT_KINDOF(CIcadView, pView);

						m_pMain->m_pDragVarsCur->vects=SDS_SSToVects(m_pMain->m_pDragVarsCur->nmSelSet,SDS_CURDWG,pView->m_pCurDwgView);
						pView->m_pDragVects=m_pMain->m_pDragVarsCur->vects;
						}
					}
				}

			if(!m_pMain->m_pDragVarsCur->firstdrag)
				{
				if(m_pMain->m_pDragVarsCur->vects)
					{
					if(SDS_DidDraggenOnce)
						{
						SDS_DrawDragVectors(NULL);
						}
					// EBATECH(shiba)-[2001-06-12 Clear afterimage by command interrupt RTZOOMPAN etc.
					else
						{
							extern cmd_drag_globalpacket	cmd_drag_gpak;
							cmd_drag_globalpacket*	gpak = &cmd_drag_gpak;
							if(gpak->rband)
								sds_grdraw(gpak->pt1,gpak->lastpt,-1,0);
						}
					// ]-EBATECH(shiba) 2001-06-12

					SDS_DidDraggenOnce=1;
					}
				}

			if(m_pMain->m_pDragVarsCur)
				{
				ic_ptcpy(m_pMain->m_pDragVarsCur->LastDrag,pt);
				}

			if( RTERROR==(ret=(*m_pMain->m_pDragVarsCur->scnf)(pt,m_pMain->m_pDragVarsCur->matx)))
				{
				m_pMain->m_pDragVarsCur->breakout=ret;
				}

			if(m_pMain->m_pDragVarsCur->vects)
				{
				SDS_DrawDragVectors(NULL);
				ic_ptcpy(m_pMain->m_pDragVarsCur->pt3,pt);
				}
			} // end of SDS_IsDragging etc....



		if(	(m_rectMspace.PtInRect(point_real) || !m_pClipRectDC) &&
			bCursorUndraw )
			{
			IcadCursor::AppCursor().DrawCursor(point_real,m_pMain->m_ViewCursorType,this);
			m_CursorOn=1;
			m_LastCursorType=m_pMain->m_ViewCursorType;
			m_LastCursorPos=point_real;
			if(m_bHaveMagneticGrip)
				{	/*D.Gavrilov*/// Now let's jump...
				CPoint CursorPos(point_real);
				ClientToScreen(&CursorPos);
				SetCursorPos(CursorPos.x, CursorPos.y);
				}
			}

		if(RTERROR==SDS_CallUserCallbackFunc( SDS_CBENDMOUSEMOVE, &point_real.x, &point_real.y, (void*)SDS_IsDragging))
			{
			return;
			}

		lParam=(((DWORD)point_real.y) <<16) | ((DWORD)point_real.x);
		SDS_SendMessage(WM_MOUSEMOVE,(WPARAM)nFlags,lParam);

		} // end of if !customize and commandqueue is empty

	Lastflp=flp;
	SDS_AtMouseMove=0;
	if( ret!=RTNORM &&
		m_pMain->m_pDragVarsCur &&
		m_pMain->m_pDragVarsCur->breakout &&
		(m_rectMspace.PtInRect(point_real) || !m_pClipRectDC))
		{
		SDS_SendMessage(WM_LBUTTONDOWN,(WPARAM)nFlags,lParam);
		}

// 	// Modified CyberAge VSB 12/05/2001
// 	// Reason: Implementation of Flyover Snapping.
// 	// Start point of Flyover Snapping. Get snap point using sds_osnap() and draw the snap.[
// 	try
// 	{
// 		resbuf rb = { 0 };
// 		sds_getvar( "OSMODE", &rb );
// 		int osmode = rb.resval.rint;
//
// 		sds_getvar( "AUTOSNAP", &rb ) ;
// 		int autosnap	 = rb.resval.rint ;
//
// 		if( ( ( bGetPoint || SDS_PointDisplayMode )&& !SDS_BoxModeCursor && !(osmode & IC_OSMODE_OFF ) && autosnap) )
// 		{
// 			if ( m_ptPrevMouseMovePt != point )
// 			{
// 				m_ptPrevMouseMovePt = point;
// 				SnapObject.m_gr_view = view;
//
// 				SNAPSTRUCT SnapStruct = SnapObject.TryEntitySnap(point);
//
// 				if ( !SnapStruct.bValidSnap && SnapObject.m_nSnapDrawn == NULL)
// 					return;
//
//
// 				CIcadView *pView = SDS_CURVIEW;
// 				CIcadDoc  *pDoc=SDS_CURDOC;
//
// 				POSITION pos=pDoc->GetFirstViewPosition();
// 				while (pos!=NULL)
// 				{
// 					if(Snap_DrawInOneWindow)
// 					{
// 						pView=SDS_CURVIEW;
// 					}
// 					else
// 					{
// 						pView=(CIcadView *)pDoc->GetNextView(pos);
// 					}
//
// 					if(pView==NULL)
// 					{
// 						return;
// 					}
//
// 					pView->SnapObject.GetLatestValues();
//
// 					// Modified CyberAge VSB 20/05/2001
// 					if ( !SDS_IsDragging )
// 					{
//
// 						int nWidth = 30;
// 						pView->m_RectBitBlt = 1;
// 						pView->m_RectBitBltPt[0].x = pView->m_iFlyOverSnapX - nWidth;
// 						pView->m_RectBitBltPt[0].y = pView->m_iFlyOverSnapY - nWidth;
// 						pView->m_RectBitBltPt[1].x = pView->m_iFlyOverSnapX + nWidth;
// 						pView->m_RectBitBltPt[1].y = pView->m_iFlyOverSnapY + nWidth;
// 						SDS_BitBlt2View(1,(CIcadView *)pView);
//
// 					}
//
// 					ASSERT_KINDOF(CIcadView, pView);
//
// 					ASSERT( !pView->IsPrinting() );
//
// 					pView->SnapObject.m_gr_view  =pView->m_pCurDwgView;
// 					if ( SnapStructLastSnap.bValidSnap )
// 					{
// 						pView->m_iFlyOverSnapX= pView->SnapObject.m_LastSnapPtPixX;
// 						pView->m_iFlyOverSnapY= pView->SnapObject.m_LastSnapPtPixY;
//
// 					}
// 					if ( SnapStruct.bValidSnap )
// 					{
// 						pView->SnapObject.pDC =  pView->GetDC();
// 						int	rop = ::SetROP2( pView->SnapObject.pDC->m_hDC,R2_XORPEN);
// 						pView->SnapObject.DrawSnap(SnapStruct, !SDS_IsDragging );
// 						::SetROP2( pView->SnapObject.pDC->m_hDC,rop);
// 						pView->ReleaseDC( pView->SnapObject.pDC );
// 					}
// 					else
// 					{
// 						pView->SnapObject.pDC =  pView->GetDC();
// 						int rop = ::SetROP2( pView->SnapObject.pDC->m_hDC,R2_XORPEN);
// 						pView->SnapObject.DeleteSnap();
// 						::SetROP2( pView->SnapObject.pDC->m_hDC,rop);
// 						pView->ReleaseDC( pView->SnapObject.pDC );
// 						if ( pView->SnapObject.m_nSnapDrawn == -1)
// 						{
// 							pView->SnapObject.m_nSnapDrawn = NULL;
// 							pView->Invalidate();
// 						}
// 					}
// 					if(Snap_DrawInOneWindow)
// 						break;
// 				}
// 				m_ctlToolTip.Pop();
// 			}
// 		}
// 		else
// 		{
// 			if ( SnapObject.m_nSnapDrawn != NULL )
// 			{
// 				SnapObject.pDC =  this->GetDC();
// 				// Modified CyberAge VSB 20/05/2001
// 				// Now using XOR pen to draw snap
// 				int rop = ::SetROP2( SnapObject.pDC->m_hDC,R2_XORPEN);
// 				SnapObject.DeleteSnap();
// 				::SetROP2( SnapObject.pDC->m_hDC,rop);
// 				this->ReleaseDC( SnapObject.pDC );
// 				if ( SnapObject.m_nSnapDrawn )
// 				{
// 					SnapObject.m_nSnapDrawn = NULL;
// 					Invalidate();
// 				}
// 			}
// 		}
// 	}
// 	catch(...)
// 	{
//
// 	}
// 	// Modified CyberAge VSB 12/05/2001 ]
//

	}

afx_msg void CIcadView::UpdateCoords(sds_point pt,int coords)
{
	extern int SDS_PointDisplayMode;
    //EBATECH(FUTA) 2001-8-12 Fix coords in StatusBar
    //extern sds_point SDS_PointDisplay;
	char cst[3][50];

	sds_point SDS_PointDisplay_ucs;	/*D.Gavrilov*/// We must convert SDS_PointDisplay
									// to UCS (where pt is) and use SDS_PointDisplay_ucs
									// instead of SDS_PointDisplay.
	//EBATECH(FUTA) 2001-8-12 -[
    //gr_rp2ucs(SDS_PointDisplay, SDS_PointDisplay_ucs, m_pCurDwgView);
	struct resbuf rbucs,rbwcs,rb;
	rbucs.restype=rbwcs.restype=RTSHORT;
	rbucs.resval.rint=1;
	rbwcs.resval.rint=0;
	if (SDS_getvar(NULL,DB_QLASTPOINT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) {
		SDS_PointDisplay_ucs[0]=SDS_PointDisplay_ucs[1]=SDS_PointDisplay_ucs[2]=0.0;
	} else {
		ic_ptcpy(SDS_PointDisplay_ucs,rb.resval.rpoint);
	}
	// ]- EBATECH(FUTA) 2001-8-12

	sds_point pt_real;	/*D.Gavrilov*/// We should change pt if ORTHO mode is ON.
	ic_ptcpy(pt_real, pt);
	//EBATECH(FUTA) 2001-8-12 -[
    //sds_resbuf rb;
    //SDS_getvar(NULL,DB_QORTHOMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
    //if(rb.resval.rint)
    //{
    //    sds_real Angle = sds_angle(SDS_PointDisplay_ucs, pt);
    //    if(fabs(cos(Angle)) < fabs(sin(Angle)))
    //        pt_real[0] = SDS_PointDisplay_ucs[0];
    //    else
    //        pt_real[1] = SDS_PointDisplay_ucs[1];
    //}
	SDS_getvar(NULL,DB_QSNAPANG,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	double sang=rb.resval.rreal;
	SDS_getvar(NULL,DB_QORTHOMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if(((rb.resval.rint && !SDS_CMainWindow->m_fIsShiftDown) ||
		(!rb.resval.rint && SDS_CMainWindow->m_fIsShiftDown)) && (SDS_PointDisplayMode > 0))
	{
		sds_real ang1 = sds_angle(SDS_PointDisplay_ucs, pt_real);
		if( ( fabs(sin(sang)) < IC_ZRO ) || ( fabs(cos(sang)) < IC_ZRO ) ){
			if( fabs(cos(ang1)) < fabs(sin(ang1)) )
				pt_real[0] = SDS_PointDisplay_ucs[0];
			else
				pt_real[1] = SDS_PointDisplay_ucs[1];
		}else{
			if(fabs(cos(ang1 - sang)) < fabs(sin(ang1-sang)))
				sang -= IC_PI / 2.0;
			pt_real[0] = pt_real[0] + sin(sang) * (pt_real[1] - SDS_PointDisplay_ucs[1] - tan(sang)
						 * (pt_real[0] - SDS_PointDisplay_ucs[0])) * cos(sang);
			pt_real[1] = SDS_PointDisplay_ucs[1] + tan(sang) * (pt_real[0] - SDS_PointDisplay_ucs[0]);
		}
		pt_real[2] = SDS_PointDisplay_ucs[2];
	}
	// ]- EBATECH(FUTA) 2001-08-12

	if(SDS_PointDisplayMode==1) {
		double dAngle = sds_angle( SDS_PointDisplay_ucs, pt_real);
		sds_angtos( dAngle,-1,-1,cst[0]);
		if(coords==2) {
			sds_rtos(sds_distance(SDS_PointDisplay_ucs, pt_real),-1,-1,cst[1]);
			_snprintf(m_pMain->m_strCoords,sizeof(m_pMain->m_strCoords)-1,ResourceString(IDC_ICADVIEW_LENGTH___S___AN_2, "Length: %s @ Angle: %s" ),cst[1],cst[0]);
		} else {
			_snprintf(m_pMain->m_strCoords,sizeof(m_pMain->m_strCoords)-1,ResourceString(IDC_ICADVIEW_ANGLE___S_3, "Angle: %s" ),cst[0]);
		}
	} else if(SDS_PointDisplayMode==2) {
		sds_rtos(sds_distance(SDS_PointDisplay_ucs, pt_real),-1,-1,cst[0]);
		if(coords==2) 
		{
			//sds_rtos(sds_angle(SDS_PointDisplay_ucs, pt_real),-1,-1,cst[1]);
			sds_angtos(sds_angle(SDS_PointDisplay_ucs, pt_real),-1,-1,cst[1]);
			if(strisame(cst[1],"360")) 
				strncpy(cst[1],"0",strlen("0"));

			_snprintf(m_pMain->m_strCoords,sizeof(m_pMain->m_strCoords)-1,ResourceString(IDC_ICADVIEW_LENGTH___S___AN_2, "Length: %s @ Angle: %s" ),cst[0],cst[1]);
		} else {
			if(m_pMain->m_pDragVarsCur && m_pMain->m_pDragVarsCur->mode==-9) {
				sds_rtos(sds_distance(SDS_PointDisplay_ucs, pt_real)*2.0,-1,-1,cst[0]);
				_snprintf(m_pMain->m_strCoords,sizeof(m_pMain->m_strCoords)-1,ResourceString(IDC_ICADVIEW_AXIS_LENGTH___S_4, "Axis length: %s" ),cst[0]);
			} else {
				_snprintf(m_pMain->m_strCoords,sizeof(m_pMain->m_strCoords)-1,ResourceString(IDC_ICADVIEW_LENGTH___S_5, "Length: %s" ),cst[0]);
			}
		}
	} else if(SDS_PointDisplayMode==3) {
		for(int fi1=0; fi1<2; fi1++) {
			sds_rtos(pt_real[fi1]-SDS_PointDisplay_ucs[fi1], -1, -1, cst[fi1]);
		}
		_snprintf(m_pMain->m_strCoords,sizeof(m_pMain->m_strCoords)-1,ResourceString(IDC_ICADVIEW_SCALE__X__S_Y___6, "Scale: X=%s,Y=%s" ),cst[0],cst[1]);
	} else if(SDS_PointDisplayMode==4) {
		sds_rtos(sds_distance(SDS_PointDisplay_ucs, pt_real),-1,-1,cst[0]);
		_snprintf(m_pMain->m_strCoords,sizeof(m_pMain->m_strCoords)-1,ResourceString(IDC_ICADVIEW_SCALE__Z__S_7, "Scale: Z=%s" ),cst[0]);
	} else if(SDS_PointDisplayMode==5) {
		if(coords==2) {
			sds_angtos(sds_angle(SDS_PointDisplay_ucs, pt_real),-1,-1,cst[0]);
			sds_rtos(sds_distance(SDS_PointDisplay_ucs, pt_real),-1,-1,cst[1]);
			
			if(strisame(cst[0],"360"))  // do not show 360° on the status bar
				strncpy(cst[0],"0",strlen("0"));

			_snprintf(m_pMain->m_strCoords,sizeof(m_pMain->m_strCoords)-1,ResourceString(IDC_ICADVIEW_LENGTH___S___AN_2, "Length: %s @ Angle: %s" ),cst[1],cst[0]);
		} else {
			for(int fi1=0; fi1<3; fi1++) {
				//EBATECH(FUTA) 2001-08-12 -[
                //sds_rtos(fabs(pt[fi1])<CMD_FUZZ ? 0.0 : pt_real[fi1],-1,-1,cst[fi1]);
                sds_rtos(fabs(pt_real[fi1])<CMD_FUZZ ? 0.0 : pt_real[fi1],-1,-1,cst[fi1]);
				// ]-
			}
			_snprintf(m_pMain->m_strCoords,sizeof(m_pMain->m_strCoords)-1,ResourceString(IDC_ICADVIEW__S__S__S_8, "%s,%s,%s" ),cst[0],cst[1],cst[2]);
		}
	} else {
		for(int fi1=0; fi1<3; fi1++) {
			//EBATECH(FUTA) 2001-08-12 -[
            //sds_rtos(fabs(pt[fi1])<CMD_FUZZ ? 0.0 : pt_real[fi1],-1,-1,cst[fi1]);
            sds_rtos(fabs(pt_real[fi1])<CMD_FUZZ ? 0.0 : pt_real[fi1],-1,-1,cst[fi1]);
			// ]-
		}
		_snprintf(m_pMain->m_strCoords,sizeof(m_pMain->m_strCoords)-1,ResourceString(IDC_ICADVIEW__S__S__S_8, "%s,%s,%s" ),cst[0],cst[1],cst[2]);
	}

	if((int)strlen(m_pMain->m_strCoords)>m_pMain->m_nMaxCoordsLen) {
		TEXTMETRIC tm;
		HGDIOBJ hOldFont=NULL;
		CClientDC dcScreen(NULL);
		CString strTmp(m_pMain->m_strCoords);
		if(m_pMain->m_StatusBar.m_hFont!=NULL) hOldFont=dcScreen.SelectObject(m_pMain->m_StatusBar.m_hFont);
		dcScreen.GetTextMetrics(&tm);

		m_pMain->m_nMaxCoordsLen=strlen(m_pMain->m_strCoords);

		m_pMain->m_StatusBar.SetCoordsPaneWidth(tm.tmAveCharWidth * m_pMain->m_nMaxCoordsLen + 60);	//*** Width of the COORDS pane

		if(hOldFont!=NULL) dcScreen.SelectObject(hOldFont);
		//*** Resize the tooltips.
		m_pMain->m_StatusBar.ResizeToolTips();
	}

	m_pMain->m_StatusBar.SetCoordsText(m_pMain->m_strCoords);

}
afx_msg void CIcadView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CIcadCntrItem* pItemHit = NULL;

	COsnapManagerIcad *pOsnapManager = ((CEngineApplication*)SDSApplication::GetEngineApplication())->getOsnapManager();
	pOsnapManager->setPoint();

//4M Bugzilla 77987 19/02/02->
/*
    if(m_pRealTime)
	{
		KillTimer(RealTimeTimer_id);
		delete m_pRealTime;
		m_pRealTime = NULL;
	}

*/
  DeleteRealTime();
//<-4M 19/02/02

    // Add by SystemMetrix(Maeda) 2001.09.03 -[ check CTRLMOUSE system value
    bool bCtrlMouse = TRUE;
    resbuf rb;
    if (sds_getvar("CTRLMOUSE", &rb) == RTNORM)
      {
        bCtrlMouse = (rb.resval.rint == 0)? FALSE : TRUE;
      }
    if (CONTROL_SHIFT_FLAGS(nFlags) && bCtrlMouse)          // ]- SystemMetrix(Maeda) 2001.09.03
	{
//4M Bugzilla 77987 19/02/02->
/*
        m_RTMotion = NO;
		m_pRealTime = new CRealTimeZoom(this, point);
*/
     SetRTMotion(NO);
     SetRealTime(new CRealTimeZoom(this, point));
//<-4M 19/02/02
        return;
	}

	long SSLen;
	sds_sslength(SDS_CURDOC->m_pGripSelection, &SSLen);
	if(SSLen > 0L)
	{
		/*D.G.*/// store a selection for OnLButtonDblClk
		sds_ssfree(SDS_EditPropsSS_DblClk);
		SDS_EditPropsSS_DblClk[0] = 0L;
		SDS_sscpy(SDS_EditPropsSS_DblClk, SDS_CURDOC->m_pGripSelection);
	}
    else if(CONTROL_FLAGS(nFlags) && bCtrlMouse)    // Modify by SystemMetrix(Maeda) 2001.09.03
		{
//4M Bugzilla 77987 19/02/02->
/*
            m_RTMotion = NO;
			m_pRealTime = new CRealTimeRotate_Sphere(this, point);
*/
        SetRTMotion(NO);
			SetRealTime (new CRealTimeRotate_Sphere(this, point));
//<-4M 19/02/02
            return;
		}


	switch(m_RTMotion)
	{
		case ZOOM :
//4M Bugzilla 77987 19/02/02->
/*
            m_pRealTime = new CRealTimeZoom(this, point);
*/
			SetRealTime (new CRealTimeZoom(this, point));
//<-4M 19/02/02
            return;
		case PAN :
//4M Bugzilla 77987 19/02/02->
/*
            m_pRealTime = new CRealTimePan(this, point);
*/
			SetRealTime (new CRealTimePan(this, point));
//<-4M 19/02/02
            return;
		case ROT_CYL_X :
//4M Bugzilla 77987 19/02/02->
/*
            m_pRealTime = new CRealTimeRotate_Cylinder(this, point, CRealTimeRotate_Cylinder::Axes::X);
*/
			SetRealTime (new CRealTimeRotate_Cylinder(this, point, CRealTimeRotate_Cylinder::Axes::X));
//<-4M 19/02/02
            return;
		case ROT_CYL_Y :
//4M Bugzilla 77987 19/02/02->
/*
            m_pRealTime = new CRealTimeRotate_Cylinder(this, point, CRealTimeRotate_Cylinder::Axes::Y);
*/
			SetRealTime (new CRealTimeRotate_Cylinder(this, point, CRealTimeRotate_Cylinder::Axes::Y));
//<-4M 19/02/02
            return;
		case ROT_CYL_Z :
//4M Bugzilla 77987 19/02/02->
/*
            m_pRealTime = new CRealTimeRotate_Cylinder(this, point, CRealTimeRotate_Cylinder::Axes::Z);
*/
			SetRealTime (new CRealTimeRotate_Cylinder(this, point, CRealTimeRotate_Cylinder::Axes::Z));
//<-4M 19/02/02
            return;
		case ROT_SPH :
//4M Bugzilla 77987 19/02/02->
/*
            m_pRealTime = new CRealTimeRotate_Sphere(this, point);
*/
			SetRealTime (new CRealTimeRotate_Sphere(this, point));
//<-4M 19/02/02
            return;
	}

	//*** If we are not in a command, do the hit test.
	if(SDS_AtCmdLine)
	{
		pItemHit=HitTestItems(point);
	}

	if(m_pMain->m_pCurView==NULL || m_pMain->m_pCurDoc==NULL) {
		m_pViewsDoc=m_pMain->m_pCurDoc=GetDocument();
		m_pMain->m_pCurFrame=(CIcadChildWnd*)GetParentFrame();
		m_pMain->m_pCurView=this;
	}

	if(!m_pMain->m_bIsAppActive) return;

	POINT	callbackPoint = point;
	if(RTERROR==SDS_CallUserCallbackFunc(SDS_CBLBUTTONDN, &point.x, &point.y,NULL)) {
		return;
	}

	// if callback changed the point, we must move the cursor
	if ( point != callbackPoint )
		{
		POINT	client = point;

		ClientToScreen( &client);
		SetCursorPos(client.x, client.y);
		}

	// No need to go through all this if there are no callbacks registered.
	if (SDS_SetAUserCallback == TRUE) {
		// Set up callback for UCS coord.
		sds_point pt;
		struct resbuf rb;
		db_drawing *flp=NULL;
		struct gr_view *view=m_pCurDwgView;
		if(m_pDocument)	flp=((CIcadDoc*)m_pDocument)->m_dbDrawing;
		if(view!=NULL) {
			SDS_getvar(NULL,DB_QCOORDS,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
		if(m_bHaveMagneticGrip)
			ic_ptcpy(pt, m_magneticGrip);
		else
		{
			gr_pix2rp(view, point.x, point.y, &pt[0], &pt[1]);
			gr_rp2ucs(pt, pt, view);
		}

			if(RTERROR==SDS_CallUserCallbackFunc(SDS_CBLBUTTONDNUCS,(void *)&pt[0],(void *)&pt[1],(void *)&pt[2])) {
				return;
			}
		}
	}

	if(m_pMain->m_pCurView!=this) {
		if(SDSApplication::GetActiveApplication()->GetInputControlBits() & SDS_RSG_NOVIEWCHG)
			{
			goto out;
			}
		if( (SDSApplication::GetActiveApplication()->GetInputControlBits() & SDS_RSG_NODOCCHG) &&
			m_pMain->m_pCurDoc!=GetDocument())
			{
			goto out;
			}
		SetNewView(TRUE);
	} else {
		db_drawing *flp=NULL;
		if(m_pDocument)	flp=((CIcadDoc*)m_pDocument)->m_dbDrawing;

		if(!m_bEatNextMouse && pItemHit!=NULL &&
			pItemHit->GetItemState()!=COleClientItem::openState &&
			pItemHit->GetItemState()!=COleClientItem::activeUIState)
		{
			CIcadRectTracker tracker;
			CRect rectItem;
			pItemHit->GetCurRect(this,&rectItem);
			pItemHit->SetupTracker(this,rectItem,&tracker);
			if(tracker.Track(this, point))
			{
				//*** Set the extent of the item before drawing it.
				CSize sizeExtent(tracker.m_rect.Size());
				if(rectItem.Size()!=sizeExtent)
				{
					pItemHit->SetServerSize(this,tracker.m_rect);
				}
				pItemHit->UpdateBounds(this,tracker.m_rect);
				SetSelection(pItemHit);
				//*** Redraw all entities including any that were behind the object.
				sds_redraw(NULL,IC_REDRAW_DONTCARE);
			}
			else
			{
				//*** Redraw only the entity getting selected.
				SetSelection(pItemHit);
				pItemHit->Draw(this,GetFrameBufferCDC());
			}
		}
		else
		{
			BOOL bEatMouse=FALSE;
			if(NULL!=m_pSelection && !m_bEatNextMouse)
			{
				//*** De-select the selected ole item.
				CIcadCntrItem* pItem = m_pSelection;
				SetSelection(NULL);
				//*** Redraw all entities including any that may be behind the object.
				sds_redraw(NULL,IC_REDRAW_DONTCARE);
				//*** Eat the point if we are not in a command.
				if(SDS_AtCmdLine)
				{
					bEatMouse=TRUE;
				}
			}
			if(!bEatMouse && !m_bEatNextMouse)
			{
				DWORD lParam;
				sds_point pt;
				struct gr_view *view=m_pCurDwgView;
				struct resbuf rb;
				if(view==NULL) return;
				SDS_getvar(NULL,DB_QCOORDS,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
				if(m_bHaveMagneticGrip)
					ic_ptcpy(pt, m_magneticGrip);
				else
				{
					gr_pix2rp(view, point.x, point.y, &pt[0], &pt[1]);
					gr_rp2ucs(pt, pt, view);
				}
				ic_ptcpy(SDS_PreGridPoint,pt);
				if(!SDS_AtEntGet && !SDS_AtCmdLine) SDS_SnapPt(pt,pt);
				UpdateCoords(pt,rb.resval.rint);
				m_CursorDownPos=point;

				lParam=(((DWORD)point.y) <<16) | ((DWORD)point.x);
				SDS_SendMessage(WM_LBUTTONDOWN,(WPARAM)nFlags,lParam);
			}
			m_bEatNextMouse=FALSE;
		}
	}

	out: ;

	CView::OnLButtonDown(nFlags, point);
}

afx_msg void CIcadView::OnLButtonUp(UINT nFlags, CPoint point)
{
	DWORD lParam;
	struct resbuf rb;
	sds_point pt;

//4M Bugzilla 77987 19/02/02->
/*
    if(m_pRealTime)
	{
		if( bool(UserPreference::ContinuousMotionPreference) &&
			(m_pRealTime->m_Motion == ROT_CYL || m_pRealTime->m_Motion == ROT_SPH) )
		{
			m_pRealTime->m_bContinuousOrbitStarted = true;
			SetTimer(RealTimeTimer_id, RealTimeTimer_time, NULL);
		}
		else
		{
			KillTimer(RealTimeTimer_id);
			delete m_pRealTime;
			m_pRealTime = NULL;
		}
		return;
	}
*/
  if(GetRealTime()){
		if( bool(UserPreference::ContinuousMotionPreference) &&
			(GetRealTime()->m_Motion == ROT_CYL || GetRealTime()->m_Motion == ROT_SPH) )
		{
			GetRealTime()->m_bContinuousOrbitStarted = true;
			SetTimer(RealTimeTimer_id, RealTimeTimer_time, NULL);
		}
		else
		{
        DeleteRealTime();
		}
		return;
	}
//<-4M 19/02/02

	extern bool SDS_bDraggingSS;
	if(SDS_bDraggingSS)
	{
		sds_point	Pt;
		gr_pix2rp(m_pCurDwgView, point.x, point.y, &Pt[0], &Pt[1]);
		gr_rp2ucs(Pt, Pt, m_pCurDwgView);
		SDS_EndDraggingSS(Pt);
	}

	db_drawing *flp=NULL;
	if(m_pDocument)	flp=((CIcadDoc*)m_pDocument)->m_dbDrawing;

	POINT	callbackPoint = point;
	if(RTERROR==SDS_CallUserCallbackFunc(SDS_CBLBUTTONUP, &point.x, &point.y,NULL)) {
		return;
	}

	// if callback changed the point, we must move the cursor
	if ( point != callbackPoint )
		{
		POINT	client = point;

		ClientToScreen( &client);
		SetCursorPos(client.x, client.y);
		}

	struct gr_view *view=m_pCurDwgView;
	if(view==NULL) return;
	SDS_getvar(NULL,DB_QCOORDS,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
	if(m_bHaveMagneticGrip)
		ic_ptcpy(pt, m_magneticGrip);
	else
	{
		gr_pix2rp(view, point.x, point.y, &pt[0], &pt[1]);
		gr_rp2ucs(pt, pt, view);
	}

	if(RTERROR==SDS_CallUserCallbackFunc(SDS_CBLBUTTONUPUCS,(void *)&pt[0],(void *)&pt[1],(void *)&pt[2])) {
		return;
	}
/*
	DP: old code
	if(m_CursorDownPos.x==point.x && m_CursorDownPos.y==point.y) return;
	SDS_getvar(NULL,DB_QPICKDRAG,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
	if(rb.resval.rint==0 || !SDS_IsDragging) return;
*/
	extern int SDS_GetCornerMode;
	SDS_getvar(NULL,DB_QPICKDRAG,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
	if(SDS_GetCornerMode && rb.resval.rint == 1)
	{
		if(!SDS_AtEntGet && !SDS_AtCmdLine) SDS_SnapPt(pt,pt);
		UpdateCoords(pt,rb.resval.rint);

		lParam=(((DWORD)point.y) <<16) | ((DWORD)point.x);
		SDS_SendMessage(WM_LBUTTONDOWN,(WPARAM)nFlags,lParam);
	}
}

afx_msg void CIcadView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
//4M Bugzilla 77987 19/02/02->
/*
    if(m_pRealTime)
	{
		KillTimer(RealTimeTimer_id);
		delete m_pRealTime;
		m_pRealTime = NULL;
	}
	m_RTMotion = NO;
*/
// We leave behaviour for ZOOM_WHEEL intact
  if (GetRTMotion()==ZOOM_WHEEL){
     DeleteRealTime();
     SetRTMotion(NO);
  }
//<-4M 19/02/02

	sds_point DummyPt;
	SDS_EndDraggingSS(DummyPt, true);

	/*D.G.*/// Let a user view/edit properties of selected entities.
	long SSLen;
	sds_sslength(SDS_CURDOC->m_pGripSelection, &SSLen);
	if(SSLen == 0L)
	{
		sds_ssfree(SDS_CURDOC->m_pGripSelection);
		SDS_sscpy(SDS_CURDOC->m_pGripSelection, SDS_EditPropsSS_DblClk);
		sds_ssfree(SDS_EditPropsSS_DblClk);
		SDS_EditPropsSS_DblClk[0] = 0L;
	}
	sds_sslength(SDS_CURDOC->m_pGripSelection, &SSLen);

	// Set up callback for pixel location.
	POINT	callbackPoint = point;
	int		DoNotShowEntityPropertiesDialog = 0;

	if(RTERROR == SDS_CallUserCallbackFunc(SDS_CBLBUTTONDBLCLK, &point.x, &point.y, (SSLen != 0L && !SDS_IsDragging) ? &DoNotShowEntityPropertiesDialog : NULL))
		return;

	// if callback changed the point, we must move the cursor
	if ( point != callbackPoint )
		{
		POINT	client = point;

		ClientToScreen( &client);
		SetCursorPos(client.x, client.y);
		}

	// No need to go through all this if there are no callbacks registered.
	if (SDS_SetAUserCallback == TRUE) {
		// Set up callback for UCS coord.
		sds_point pt;
		struct resbuf rb;
		db_drawing *flp=NULL;
		struct gr_view *view=m_pCurDwgView;
		if(m_pDocument)	flp=((CIcadDoc*)m_pDocument)->m_dbDrawing;
		if(view!=NULL) {
			SDS_getvar(NULL,DB_QCOORDS,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
			gr_pix2rp(view,point.x,point.y,&pt[0],&pt[1]);
			gr_rp2ucs(pt,pt,view);

			if(RTERROR==SDS_CallUserCallbackFunc(SDS_CBLBUTTONDBLCLKUCS,(void *)&pt[0],(void *)&pt[1],(void *)&pt[2])) {
				return;
			}
		}
	}

	
	/*DG - 9.7.2002*/// The fix below is reversed (it's commented out) because it's not correct and
	// doesn't fix the real reason of the bug (to reproduce the bug: execute (while(princ(grread 1))) and
	// do something during it's running - double click or switch to another window, and this lisp
	// expression will not print mouse-move points).
	if(!DoNotShowEntityPropertiesDialog && SSLen != 0L && !SDS_IsDragging)
		SDS_SendMessage(WM_MENUSELECT, 0, (LPARAM)"_ENTPROP"/*DNT*/);

//		//EBATECH(FUTA) 2001-8-11
//		//if(!DoNotShowEntityPropertiesDialog && SSLen != 0L && !SDS_IsDragging)
//		if(!DoNotShowEntityPropertiesDialog && SSLen != 0L && !SDS_IsDragging && TRUE)
//			SDS_SendMessage(WM_MENUSELECT, 0, (LPARAM)"_ENTPROP"/*DNT*/);
//
//		OnLButtonDown(nFlags, point);//EBATECH(FUTA) 2001-8-11
	

    if (m_pSelection != NULL)
    {
        m_pSelection->DoVerb(GetKeyState(VK_CONTROL) < 0 ?
            OLEIVERB_OPEN : OLEIVERB_PRIMARY, this);
    }

    CView::OnLButtonDblClk(nFlags, point);
}

// SMC(Maeda)-[2001-08-11 Add CallbackFunc
afx_msg void CIcadView::OnRButtonDblClk(UINT nFlags, CPoint point)
{
//4M Bugzilla 77987 19/02/02->
/*
    if(m_pRealTime)
    {
        KillTimer(RealTimeTimer_id);
        delete m_pRealTime;
        m_pRealTime = NULL;
    }
    m_RTMotion = NO;
*/
//We leave behaviour for ZOOM_WHEEL intact
   if (GetRTMotion()==ZOOM_WHEEL){
       DeleteRealTime();
       SetRTMotion(NO);
   }
//<-4M 19/02/02

    sds_point DummyPt;
    SDS_EndDraggingSS(DummyPt, true);

    /*D.G.*/// Let a user view/edit properties of selected entities.
    long SSLen;
    sds_sslength(SDS_CURDOC->m_pGripSelection, &SSLen);
    if(SSLen == 0L)
    {
        sds_ssfree(SDS_CURDOC->m_pGripSelection);
        SDS_sscpy(SDS_CURDOC->m_pGripSelection, SDS_EditPropsSS_DblClk);
        sds_ssfree(SDS_EditPropsSS_DblClk);
        SDS_EditPropsSS_DblClk[0] = 0L;
    }
    sds_sslength(SDS_CURDOC->m_pGripSelection, &SSLen);

    // Set up callback for pixel location.
    POINT   callbackPoint = point;
    int     DoNotShowEntityPropertiesDialog = 0;

    if(RTERROR == SDS_CallUserCallbackFunc(SDS_CBRBUTTONDBLCLK, &point.x, &point.y, (SSLen != 0L && !SDS_IsDragging) ? &DoNotShowEntityPropertiesDialog : NULL))
        return;

    // if callback changed the point, we must move the cursor
    if ( point != callbackPoint )
        {
        POINT   client = point;

        ClientToScreen( &client);
        SetCursorPos(client.x, client.y);
        }

    // No need to go through all this if there are no callbacks registered.
    if (SDS_SetAUserCallback == TRUE) {
        // Set up callback for UCS coord.
        sds_point pt;
        struct resbuf rb;
        db_drawing *flp=NULL;
        struct gr_view *view=m_pCurDwgView;
        if(m_pDocument) flp=((CIcadDoc*)m_pDocument)->m_dbDrawing;
        if(view!=NULL) {
            SDS_getvar(NULL,DB_QCOORDS,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
            gr_pix2rp(view,point.x,point.y,&pt[0],&pt[1]);
            gr_rp2ucs(pt,pt,view);

            if(RTERROR==SDS_CallUserCallbackFunc(SDS_CBRBUTTONDBLCLKUCS,(void *)&pt[0],(void *)&pt[1],(void *)&pt[2])) {
                return;
            }
        }
    }

	if(!DoNotShowEntityPropertiesDialog && SSLen != 0L && !SDS_IsDragging)
		SDS_SendMessage(WM_MENUSELECT, 0, (LPARAM)"_ENTPROP"/*DNT*/);

	if (m_pSelection != NULL)
	{
		m_pSelection->DoVerb(GetKeyState(VK_CONTROL) < 0 ?
			OLEIVERB_OPEN : OLEIVERB_PRIMARY, this);
	}

    CView::OnRButtonDblClk(nFlags, point);
}
// ]-SMC(Maeda) 2001-08-11

afx_msg void CIcadView::OnRButtonDown(UINT nFlags, CPoint point)
{
	DWORD lParam;

	COsnapManagerIcad *pOsnapManager = ((CEngineApplication*)SDSApplication::GetEngineApplication())->getOsnapManager();
	pOsnapManager->reset( this );
	updateToolTip();

//4M Bugzilla 77987 19/02/02->
/*
    if(m_pRealTime)
	{
		KillTimer(RealTimeTimer_id);
		delete m_pRealTime;
		m_pRealTime = NULL;
	}

	m_RTMotion = NO;
*/
//We leave behaviour for ZOOM_WHEEL intact
  if (GetRTMotion()==ZOOM_WHEEL){
     DeleteRealTime();
     SetRTMotion(NO);
  }
//<-4M 19/02/02
    sds_point DummyPt;
	SDS_EndDraggingSS(DummyPt, true);

    // Add by SystemMetrix(Maeda) 2001.09.03 -[ check CTRLMOUSE system value
    bool bCtrlMouse = TRUE;
    resbuf rb;
    if (sds_getvar("CTRLMOUSE", &rb) == RTNORM)
      {
        bCtrlMouse = (rb.resval.rint == 0)? FALSE : TRUE;
      }
    if (CONTROL_SHIFT_FLAGS(nFlags) && bCtrlMouse)          // ]- SystemMetrix(Maeda) 2001.09.03
	{
//4M Bugzilla 77987 19/02/02->
/*
        m_pRealTime = new CRealTimePan(this, point);
*/
       SetRealTime(new CRealTimePan(this, point));
//<-4M 19/02/02
        return;
	}
    else if(CONTROL_FLAGS(nFlags) && bCtrlMouse)    // Modify by SystemMetrix(Maeda) 2001.09.03
		{
//4M Bugzilla 77987 19/02/02->
/*
            m_pRealTime = new CRealTimeRotate_Cylinder(this, point, CRealTimeRotate_Cylinder::Axes::Z);
*/
       SetRealTime(new CRealTimeRotate_Cylinder(this, point, CRealTimeRotate_Cylinder::Axes::Z));
//<-4M 19/02/02
            return;
		}

	// Set up callback for pixel position.
	POINT	callbackPoint = point;
	if(RTERROR==SDS_CallUserCallbackFunc(SDS_CBRBUTTONDN, &point.x, &point.y,NULL)) {
		return;
	}

	// if callback changed the point, we must move the cursor
	if ( point != callbackPoint )
		{
		POINT	client = point;

		ClientToScreen( &client);
		SetCursorPos(client.x, client.y);
		}

	// No need to go through all this if there are no callbacks
	// registered.
	if (SDS_SetAUserCallback == TRUE) {
		// Set up callback for UCS coordinates.
		sds_point pt;
		struct resbuf rb;
		db_drawing *flp=NULL;
		struct gr_view *view=m_pCurDwgView;
		if(m_pDocument)	flp=((CIcadDoc*)m_pDocument)->m_dbDrawing;
		if(view!=NULL) {
			SDS_getvar(NULL,DB_QCOORDS,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
			gr_pix2rp(view,point.x,point.y,&pt[0],&pt[1]);
			gr_rp2ucs(pt,pt,view);

			if(RTERROR==SDS_CallUserCallbackFunc(SDS_CBRBUTTONDNUCS,(void *)&pt[0],(void *)&pt[1],(void *)&pt[2])) {
				return;
			}
		}
	}
	if(m_pMain->m_pDragVarsCur && m_pMain->m_pCurDoc!=GetDocument()) return;
	//*** Create a context menu for a selected OLE item.
	CIcadCntrItem* pItemHit = HitTestItems(point);
	if(SDS_AtCmdLine!=0 && NULL!=m_pSelection && NULL==pItemHit)
	{
		SetSelection(NULL);
	}
	if(SDS_AtCmdLine!=0 && NULL!=pItemHit &&
		pItemHit->GetItemState()!=COleClientItem::openState &&
		pItemHit->GetItemState()!=COleClientItem::activeUIState)
	{
		SetSelection(pItemHit);
		pItemHit->Draw(this,GetFrameBufferCDC());
		OleContextMenu(point);
		return;
	}

	if(nFlags==(MK_SHIFT | MK_RBUTTON))
	{
		// Undraw old cursor
		if( m_CursorOn )
		{
			CursorOn( false );
			m_CursorOn = 0;
		}
		// invoke EsnapContextMenu
		EsnapContextMenu(point);

		// Set new point for cursor
		struct gr_view *grView = m_pCurDwgView;
		if( !SDS_AtEntGet && !SDS_AtCmdLine && grView )
		{
			// set new point
			if( ::GetCursorPos( &point ) )
			{
				CRect wndRect;
				GetWindowRect( &wndRect );
				if( wndRect.PtInRect( point ) )
				{
					ScreenToClient( &point );
					sds_point pt;
					gr_pix2rp( grView, point.x, point.y, &pt[0],&pt[1] );
					gr_rp2ucs(pt, pt, grView);
					SDS_SnapPt( pt, pt );
					gr_ucs2rp(pt, pt, grView);
					gr_rp2pix(grView, pt[0], pt[1], (int*)&point.x, (int*)&point.y);

					// Draw cursor
					m_LastCursorPos = point;
					CursorOn( true );
					m_CursorOn = 1;
				}
			}
		}
		return;
	}

	lParam=(((DWORD)point.y) <<16) | ((DWORD)point.x);
	SDS_SendMessage(WM_RBUTTONDOWN,(WPARAM)nFlags,lParam);
}

afx_msg void CIcadView::OnMButtonDown(UINT nFlags, CPoint point)
{
	// EBATECH(shiba)-[2001-06-12 Clear afterimage by command interrupt RTZOOMPAN etc.
	m_bMButtonClick = true;
	m_lastMBDownPoint = point;
	// ]-EBATECH(shiba) 2001-06-12

	COsnapManagerIcad *pOsnapManager = ((CEngineApplication*)SDSApplication::GetEngineApplication())->getOsnapManager();
	pOsnapManager->reset( this );
	updateToolTip();

//4M Bugzilla 77987 19/02/02->
/*
    if(m_pRealTime)
	{
		KillTimer(RealTimeTimer_id);
		delete m_pRealTime;
		m_pRealTime = NULL;
	}

	m_RTMotion = NO;

*/
//We leave behaviour for ZOOM_WHEEL intact
  if (GetRTMotion()==ZOOM_WHEEL){
     DeleteRealTime();
     SetRTMotion(NO);
  }
//<-4M 19/02/02
    sds_point DummyPt;
	SDS_EndDraggingSS(DummyPt, true);

	resbuf rb;
	if(sds_getvar("MBUTTONPAN", &rb) == RTNORM)
		if(rb.resval.rint)
//4M Bugzilla 77987 19/02/02->
/*
            m_pRealTime = new CRealTimePan(this, point);
*/
     {
			SetRealTime(new CRealTimePan(this, point));
     }

//<-4M 19/02/02
        else
			EsnapContextMenu(point);

    // SMC(Maeda)-[2001-08-11 Add CallbackFunc

    // Set up callback for pixel location.
    POINT   callbackPoint = point;
    int     DoNotShowEntityPropertiesDialog = 0;

    if(RTERROR==SDS_CallUserCallbackFunc(SDS_CBMBUTTONDN, &point.x, &point.y,NULL)) {
        return;
    }

    // if callback changed the point, we must move the cursor
    if ( point != callbackPoint )
        {
        POINT   client = point;

        ClientToScreen( &client);
        SetCursorPos(client.x, client.y);
        }

    // No need to go through all this if there are no callbacks registered.
    if (SDS_SetAUserCallback == TRUE) {
        // Set up callback for UCS coord.
        sds_point pt;
        struct resbuf rb;
        db_drawing *flp=NULL;
        struct gr_view *view=m_pCurDwgView;
        if(m_pDocument) flp=((CIcadDoc*)m_pDocument)->m_dbDrawing;
        if(view!=NULL) {
            SDS_getvar(NULL,DB_QCOORDS,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
            gr_pix2rp(view,point.x,point.y,&pt[0],&pt[1]);
            gr_rp2ucs(pt,pt,view);

            if(RTERROR==SDS_CallUserCallbackFunc(SDS_CBMBUTTONDNUCS,(void *)&pt[0],(void *)&pt[1],(void *)&pt[2])) {
                return;
            }
        }
    }
    // ]-SMC(Maeda) 2001-08-11
	CView::OnMButtonDown(nFlags, point);
}

afx_msg void CIcadView::OnMButtonUp(UINT nFlags, CPoint point)
{
//4M Bugzilla 77987 19/02/02->
/*
    if(m_pRealTime)
	{
		KillTimer(RealTimeTimer_id);
		delete m_pRealTime;
		m_pRealTime = NULL;
	}
*/
  SetRTMotion(NO);
  DeleteRealTime();
//<-4M 19/02/02
    // EBATECH(shiba)-[ 2001-06-12 Clear afterimage by command interrupt RTZOOMPAN etc.
	if (m_bMButtonClick)
	{
		m_bMButtonClick = false;
		if (point == m_lastMBDownPoint)
		{
			if ( m_pMain->m_pDragVarsCur && m_pMain->m_pDragVarsCur->firstdrag != 0)
			{
				m_pMain->m_pDragVarsCur->firstdrag = 0;
			}
			SDS_DidDraggenOnce = 0;
		}
	}
	// ]-EBATECH(shiba) 2001-06-12

    // SMC(Maeda)-[2001-08-11 Add CallbackFunc

    // Set up callback for pixel location.
    POINT   callbackPoint = point;
    int     DoNotShowEntityPropertiesDialog = 0;

    if(RTERROR==SDS_CallUserCallbackFunc(SDS_CBMBUTTONUP, &point.x, &point.y,NULL)) {
        return;
    }

    // if callback changed the point, we must move the cursor
    if ( point != callbackPoint )
        {
        POINT   client = point;

        ClientToScreen( &client);
        SetCursorPos(client.x, client.y);
        }

    // No need to go through all this if there are no callbacks registered.
    if (SDS_SetAUserCallback == TRUE) {
        // Set up callback for UCS coord.
        sds_point pt;
        struct resbuf rb;
        db_drawing *flp=NULL;
        struct gr_view *view=m_pCurDwgView;
        if(m_pDocument) flp=((CIcadDoc*)m_pDocument)->m_dbDrawing;
        if(view!=NULL) {
            SDS_getvar(NULL,DB_QCOORDS,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
            gr_pix2rp(view,point.x,point.y,&pt[0],&pt[1]);
            gr_rp2ucs(pt,pt,view);

            if(RTERROR==SDS_CallUserCallbackFunc(SDS_CBMBUTTONUPUCS,(void *)&pt[0],(void *)&pt[1],(void *)&pt[2])) {
                return;
            }
        }
    }
    // ]-SMC(Maeda) 2001-08-11
	CView::OnMButtonUp(nFlags, point);
}

afx_msg void CIcadView::OnMButtonDblClk(UINT nFlags, CPoint point)
{
//4M Bugzilla 77987 19/02/02->
/*
    if(m_pRealTime)
	{
		KillTimer(RealTimeTimer_id);
		delete m_pRealTime;
		m_pRealTime = NULL;
	}
*/
  DeleteRealTime();
  SetRTMotion(NO);
//<-4M 19/02/02
    // EBATECH(shiba)-[2001-06-12 Clear afterimage by command interrupt RTZOOM 'E' etc.
	if ( m_pMain->m_pDragVarsCur && m_pMain->m_pDragVarsCur->firstdrag != 0)
	{
		m_pMain->m_pDragVarsCur->firstdrag = 0;
	}
	SDS_DidDraggenOnce = 0;
	OnMouseMove(nFlags, point);
	// ]-EBATECH(shiba) 2001-06-12

    // SMC(Maeda)-[2001-08-11 Add CallbackFunc

    // Set up callback for pixel location.
    POINT   callbackPoint = point;
    int     DoNotShowEntityPropertiesDialog = 0;

    if(RTERROR==SDS_CallUserCallbackFunc(SDS_CBMBUTTONDBLCLK, &point.x, &point.y,NULL)) {
        return;
    }

    // if callback changed the point, we must move the cursor
    if ( point != callbackPoint )
        {
        POINT   client = point;

        ClientToScreen( &client);
        SetCursorPos(client.x, client.y);
        }

    // No need to go through all this if there are no callbacks registered.
    if (SDS_SetAUserCallback == TRUE) {
        // Set up callback for UCS coord.
        sds_point pt;
        struct resbuf rb;
        db_drawing *flp=NULL;
        struct gr_view *view=m_pCurDwgView;
        if(m_pDocument) flp=((CIcadDoc*)m_pDocument)->m_dbDrawing;
        if(view!=NULL) {
            SDS_getvar(NULL,DB_QCOORDS,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
            gr_pix2rp(view,point.x,point.y,&pt[0],&pt[1]);
            gr_rp2ucs(pt,pt,view);

            if(RTERROR==SDS_CallUserCallbackFunc(SDS_CBMBUTTONDBLCLKUCS,(void *)&pt[0],(void *)&pt[1],(void *)&pt[2])) {
                return;
            }
        }
    }
    // ]-SMC(Maeda) 2001-08-11

	/*D.G.*/// We have already deleted m_pRealTime but haven't made set event mask (in SDS_AskForPoint)
	SDS_EventMask(SDS_EVM_KEYCHAR|SDS_EVM_MOUSEBUTTON|SDS_EVM_MENUCOMMAND|SDS_EVM_POPUPCOMMAND);

	SDS_SendMessage(WM_MENUSELECT, 0, (LPARAM)"'.ZOOM;_E;"/*DNT*/);
	CView::OnMButtonDblClk(nFlags, point);
}

afx_msg void CIcadView::OnRButtonUp(UINT nFlags, CPoint point)
{

//4M Bugzilla 77987 19/02/02->
/*
    if(m_pRealTime)
	{
		if( bool(UserPreference::ContinuousMotionPreference) &&
			(m_pRealTime->m_Motion == ROT_CYL || m_pRealTime->m_Motion == ROT_SPH) )
		{
			m_pRealTime->m_bContinuousOrbitStarted = true;
			SetTimer(RealTimeTimer_id, RealTimeTimer_time, NULL);
		}
		else
		{
			KillTimer(RealTimeTimer_id);
			delete m_pRealTime;
			m_pRealTime = NULL;
		}
		return;
	}
*/
	if(GetRealTime())
	{
		if( bool(UserPreference::ContinuousMotionPreference) &&
			(GetRealTime()->m_Motion == ROT_CYL || GetRealTime()->m_Motion == ROT_SPH) )
		{
			GetRealTime()->m_bContinuousOrbitStarted = true;
			SetTimer(RealTimeTimer_id, RealTimeTimer_time, NULL);
		}
		else
		{
        DeleteRealTime();
		}
		return;
	}
//<-4M 19/02/02
    POINT   callbackPoint = point;
	if(RTERROR==SDS_CallUserCallbackFunc(SDS_CBRBUTTONUP, &point.x, &point.y,NULL)) {
		return;
	}

	// if callback changed the point, we must move the cursor
	if ( point != callbackPoint )
		{
		POINT	client = point;

		ClientToScreen( &client);
		SetCursorPos(client.x, client.y);
		}

	// No need to go through all this if there are no callbacks
	// registered.
	if (SDS_SetAUserCallback == TRUE) {
		// Set up callback for UCS coords.
		sds_point pt;
		struct resbuf rb;
		db_drawing *flp=NULL;
		struct gr_view *view=m_pCurDwgView;
		if(m_pDocument)	flp=((CIcadDoc*)m_pDocument)->m_dbDrawing;
		if(view!=NULL) {
			SDS_getvar(NULL,DB_QCOORDS,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
			gr_pix2rp(view,point.x,point.y,&pt[0],&pt[1]);
			gr_rp2ucs(pt,pt,view);

			if(RTERROR==SDS_CallUserCallbackFunc(SDS_CBRBUTTONUPUCS,(void *)&pt[0],(void *)&pt[1],(void *)&pt[2])) {
				return;
			}
		}
	}

	return;
}

BOOL CIcadView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
//4M Bugzilla 77987 19/02/02->
  if ((GetRTMotion()!=NO)&&(GetRTMotion()!=ZOOM_WHEEL))
     return FALSE;
//<-4M 19/02/02
    sds_point   DummyPt;
	SDS_EndDraggingSS(DummyPt, true);

	if(m_pMain->m_bPrintPreview)	/*D.G.*/
		return TRUE;

	// EBATECH(shiba)-[ 2001-06-12 Clear afterimage by command interrupt RTZOOM etc.
	if ( m_pMain->m_pDragVarsCur && m_pMain->m_pDragVarsCur->firstdrag != 0)
	{
		m_pMain->m_pDragVarsCur->firstdrag = 0;
	}
	SDS_DidDraggenOnce = 0;
	// ]-EBATECH(shiba) 2001-06-12

//4M Bugzilla 77987 19/02/02->
/*
    if(m_pRealTime && m_RTMotion != ZOOM_WHEEL)
	{
		KillTimer(RealTimeTimer_id);
		delete m_pRealTime;
		m_pRealTime = NULL;
		m_RTMotion = NO;
	}

	if(m_RTMotion != ZOOM_WHEEL)
	{
		POINT	DummyPt = {0, 0};
		m_pRealTime = new CRealTimeZoom(this, DummyPt);
		m_RTMotion = ZOOM_WHEEL;
	}
*/
	if(GetRealTime() && GetRTMotion() != ZOOM_WHEEL)
	{
     DeleteRealTime();
     SetRTMotion(NO);
	}
	if(GetRTMotion() != ZOOM_WHEEL)
	{
		POINT	DummyPt = {0, 0};
		SetRealTime ( new CRealTimeZoom(this, DummyPt));
     SetRTMotion(ZOOM_WHEEL);
	}
//<-4M 19/02/02
    else
		KillTimer(RealTimeMouseWheelTimerID);

	if(!zDelta)
		return TRUE;

//4M Bugzilla 77987 19/02/02->
/*
    if(m_pRealTime)
	{
		// EBATECH(shiba)-[changed
//		m_pRealTime->Continue(zDelta);
		m_pRealTime->Continue(zDelta, pt);
		// ]-EBATECH(shiba)
		SetTimer(RealTimeMouseWheelTimerID, RealTimeMouseWheelElapseTime, NULL);
	}

*/
	if(GetRealTime())
	{
		// EBATECH(shiba)-[changed
//		m_pRealTime->Continue(zDelta);
		GetRealTime()->Continue(zDelta, pt);
		// ]-EBATECH(shiba)
		SetTimer(RealTimeMouseWheelTimerID, RealTimeMouseWheelElapseTime, NULL);
	}
//<-4M 19/02/02
    return TRUE;
}

// Hide or show the scrollbars on per view basis
// MFC should have made this virtual
void CIcadView:: ShowScrollBar(UINT nBar, BOOL bShow)
{
	m_showScrollBars = !!bShow;
#ifdef _LAYOUTTABS_
	ASSERT(nBar == SB_BOTH);
	if(m_showScrollBars)
	{
		GetScrollBarCtrl(SB_HORZ)->ShowWindow(SW_SHOW);
		GetScrollBarCtrl(SB_VERT)->ShowWindow(SW_SHOW);
	}
	else
	{
		GetScrollBarCtrl(SB_HORZ)->ShowWindow(SW_HIDE);
		GetScrollBarCtrl(SB_VERT)->ShowWindow(SW_HIDE);
	}
	GetParentFrame()->RecalcLayout();
#else
	CWnd::ShowScrollBar(nBar, m_showScrollBars);
#endif
}

/////////////////////////////////////////////////////////////////////
// OnScroll functions
void CIcadView::UpdateScrollBars()
{
	if(!ScrollBarsOn())
		return;

	int bposx, bposy, bsizx, bsizy;	//scroll button positions & sizes (both ranges 0-100)
	struct resbuf rb, rbucs, rbwcs;
	int fi1;
	sds_point vctr, rpmin, rpmax, p1, p2;
	sds_real fr1, vsiz, hsiz;
	sds_name elast;
	double fd1;
	rbucs.restype = rbwcs.restype = RTSHORT;
	rbucs.resval.rint = 1;
	rbwcs.resval.rint = 0;

	if(m_pVportTabHip == NULL || m_pCurDwgView == NULL)
		return;

	int tilemode, cvport;
	SDS_getvar(NULL, DB_QTILEMODE, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
	tilemode = rb.resval.rint;
	SDS_getvar(NULL, DB_QCVPORT, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
	cvport = rb.resval.rint;

	if(tilemode)
	{
		m_pVportTabHip->get_71(&fi1);
		//if(fi1&1) m_pVportTabHip->get_17(rb.resval.rpoint);
		//else	  m_pVportTabHip->get_12(rb.resval.rpoint);//I think this is RP, so
		//sds_trans(rb.resval.rpoint,&rbwcs,&rbucs,0,p1);  // I'm replacing it w/code below
		if(fi1 & 1)
			m_pVportTabHip->get_17(vctr);
		else
			m_pVportTabHip->get_12(vctr);
		m_pVportTabHip->get_40(&vsiz);
	} 
	else
	{
		SDS_getvar(NULL, DB_QVIEWCTR, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
		gr_ucs2rp(rb.resval.rpoint, vctr, m_pCurDwgView);
		SDS_getvar(NULL, DB_QVIEWSIZE, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
		vsiz = rb.resval.rreal;
	}
	//gr_ucs2rp(p1,vctr,m_pCurDwgView);//moved into else{ above

	hsiz = vsiz * m_iWinX / m_iWinY;
	if(RTNORM != sds_entlast(elast))
	{	//if drawing is empty...
		SDS_getvar(NULL, ((tilemode == 0 && cvport == 1) ? DB_QP_LIMMIN : DB_QLIMMIN), &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
		ic_ptcpy(p1, rb.resval.rpoint);
		SDS_getvar(NULL, ((tilemode == 0 && cvport == 1) ? DB_QP_LIMMAX : DB_QLIMMAX), &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
		ic_ptcpy(p2, rb.resval.rpoint);
	}
	else
	{
		SDS_getvar(NULL, DB_QEXTMIN, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
		ic_ptcpy(p1,rb.resval.rpoint);
		SDS_getvar(NULL, DB_QEXTMAX, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
		ic_ptcpy(p2, rb.resval.rpoint);
	}
	SDS_rpextents(p1, p2, rpmin, rpmax, NULL);
	//we now have vctr, vsiz, hsiz, rpmin, & rpmax
	if(hsiz / vsiz > (rpmax[0] - rpmin[0]) / (rpmax[1] - rpmin[1]))
	{
		fd1 = rpmax[0];
		rpmax[0] = (rpmax[0] + rpmin[0]) / 2.0 + (rpmax[1] - rpmin[1]) * 0.5 * hsiz / vsiz;
		rpmin[0] = (fd1 + rpmin[0]) / 2.0 - (rpmax[1] - rpmin[1]) * 0.5 * hsiz / vsiz;
	}
	else
	{
		fd1 = rpmax[1];
		rpmax[1] = (rpmax[1] + rpmin[1]) / 2.0 + (rpmax[0] - rpmin[0]) * 0.5 * vsiz / hsiz;
		rpmin[1] = (fd1 + rpmin[1]) / 2.0 - (rpmax[0] - rpmin[0]) * 0.5 * vsiz / hsiz;
	}
	fr1 = 100.0 * vsiz / (rpmax[1] - rpmin[1]);//don't add 0.5 - user can't tell if size is off
	modf(fr1, &fd1);
	if(fd1 < 0.0)
		fd1 = 0.0;
	if(fd1 > 100.0)
		fd1 = 100.0;
	bsizy = (int)fd1;
	if(bsizy > ICAD_VIEW_BSIZMAX)
		bsizy = ICAD_VIEW_BSIZMAX;
	fr1 = 100.0 * hsiz / (rpmax[0] - rpmin[0]);
	modf(fr1, &fd1);
	if(fd1 < 0.0)
		fd1 = 0.0;
	if(fd1 > 100.0)
		fd1 = 100.0;
	bsizx = (int)fd1;
	if(bsizx > ICAD_VIEW_BSIZMAX)
		bsizx = ICAD_VIEW_BSIZMAX;
	fr1 = 0.5 + (100.0 - bsizy) * (rpmax[1] - vctr[1]) / (rpmax[1] - rpmin[1]);	//add 0.5 to get correct rounding & assure
	//fr1=(100.0)*(rpmax[1]-vctr[1])/(rpmax[1]-rpmin[1]);			// correct appearance when zoomed to ctr of dwg
	if(fr1 < 0.0)
		fr1 = 0.0;
	if(fr1 > 100.0)
		fr1 = 100.0;
	modf(fr1, &fd1);
	bposy = (int)fd1;
	if(bposy > (100 - bsizy))
		bposy = 100-bsizy;
	fr1 = 0.5 + (100.0 - bsizx) * (vctr[0] - rpmin[0]) / (rpmax[0] - rpmin[0]);
	//fr1=(100.0)*(vctr[0]-rpmin[0])/(rpmax[0]-rpmin[0]);
	if(fr1 > 100.0)
		fr1 = 100.0;
	if(fr1 < 0.0)
		fr1 = 0.0;
	modf(fr1, &fd1);
	bposx = (int)fd1;
	if(bposx > (100-bsizx))
		bposx = 100 - bsizx;
	//*** Setup the scroll bar
	SCROLLINFO si;
	si.fMask = SIF_ALL;
	si.nMin = 0;
	si.nMax = 100;
	si.nPos = bposy;
	si.nPage = bsizy;
	SetScrollInfo(SB_VERT, &si, TRUE);
	si.nPos = bposx;
	si.nPage = bsizx;
	SetScrollInfo(SB_HORZ, &si, TRUE);
}

void CIcadView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if(m_pVportTabHip==NULL || m_pCurDwgView==NULL || SDS_IsDragging) return;

//4M Bugzilla 77987 19/02/02->
/*
    if(m_pRealTime)
	{
		KillTimer(RealTimeTimer_id);
		delete m_pRealTime;
		m_pRealTime = NULL;
	}
*/
  DeleteRealTime();
//<-4M 19/02/02
    sds_point DummyPt;
	SDS_EndDraggingSS(DummyPt, true);

	struct resbuf rb,rbucs,rbdcs;
	sds_real fr1;
	sds_point p1,p2,p3,p4;
	struct gr_view *view=m_pCurDwgView;
	rbucs.restype=rbdcs.restype=RTSHORT;
	rbucs.resval.rint=1;
	rbdcs.resval.rint=2;

	sds_point viewctr;
	double viewsize;
	int tilemode,cvport;

	SDS_getvar(NULL,DB_QVIEWMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if(rb.resval.rint&1){
		cmd_ErrorPrompt(CMD_ERR_PERSPECTIVE,0);
		return;
	}
	SDS_getvar(NULL,DB_QTILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	tilemode=rb.resval.rint;
	SDS_getvar(NULL,DB_QCVPORT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	cvport=rb.resval.rint;

	if(tilemode) {
		m_pVportTabHip->get_12(rb.resval.rpoint); // VIEWCTR
		sds_trans(rb.resval.rpoint,&rbdcs,&rbucs,0,p1);
		fr1=(double)m_iWinX/(double)m_iWinY;
		m_pVportTabHip->get_40(&viewsize); // VIEWSIZE
	} else {
		SDS_getvar(NULL,DB_QVIEWCTR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		ic_ptcpy(p1,rb.resval.rpoint);
		SDS_getvar(NULL,DB_QVIEWSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		viewsize=rb.resval.rreal;
	}
	gr_ucs2rp(p1,viewctr,view);

	SDS_SetUndo(IC_UNDO_COMMAND_BEGIN,(void *)"View scroll"/*DNT*/,NULL,NULL,SDS_CURDWG);
	switch(nSBCode) {
		case SB_PAGEDOWN:
			viewctr[1]-=(viewsize*ICAD_VIEW_PGSIZE);
			gr_rp2ucs(viewctr,p2,view);
			SDS_ZoomIt(NULL,2,p2,NULL,NULL,NULL,NULL,NULL);
			break;
		case SB_PAGEUP:
			viewctr[1]+=(viewsize*ICAD_VIEW_PGSIZE);
			gr_rp2ucs(viewctr,p2,view);
			SDS_ZoomIt(NULL,2,p2,NULL,NULL,NULL,NULL,NULL);
			break;
		case SB_LINEDOWN:
			viewctr[1]-=(viewsize*ICAD_VIEW_LNSIZE);
			gr_rp2ucs(viewctr,p2,view);
			SDS_ZoomIt(NULL,2,p2,NULL,NULL,NULL,NULL,NULL);
			break;
		case SB_LINEUP:
			viewctr[1]+=(viewsize*ICAD_VIEW_LNSIZE);
			gr_rp2ucs(viewctr,p2,view);
			SDS_ZoomIt(NULL,2,p2,NULL,NULL,NULL,NULL,NULL);
			break;
		//case SB_THUMBTRACK:
		case SB_THUMBPOSITION:
			int fi1;
			sds_name elast;

			if(sds_entlast(elast)==RTERROR)fi1=1;
			else fi1=0;

			SDS_getvar(NULL,fi1 ? ((tilemode==0 && cvport==1) ? DB_QP_LIMMIN : DB_QLIMMIN) : DB_QEXTMIN,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			ic_ptcpy(p1,rb.resval.rpoint);
			SDS_getvar(NULL,fi1 ? ((tilemode==0 && cvport==1) ? DB_QP_LIMMAX : DB_QLIMMAX) : DB_QEXTMAX,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			ic_ptcpy(p2,rb.resval.rpoint);
			if(icadRealEqual(p1[0],p2[0])){
				SDS_getvar(NULL,!fi1 ? ((tilemode==0 && cvport==1) ? DB_QP_LIMMIN : DB_QLIMMIN) : DB_QEXTMIN,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				p1[0]=rb.resval.rpoint[0];
				SDS_getvar(NULL,!fi1 ? ((tilemode==0 && cvport==1) ? DB_QP_LIMMAX : DB_QLIMMAX) : DB_QEXTMAX,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				p2[0]=rb.resval.rpoint[0];
			}
			if(icadRealEqual(p1[1],p2[1])){
				SDS_getvar(NULL,!fi1 ? ((tilemode==0 && cvport==1) ? DB_QP_LIMMIN : DB_QLIMMIN) : DB_QEXTMIN,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				p1[1]=rb.resval.rpoint[1];
				SDS_getvar(NULL,!fi1 ? ((tilemode==0 && cvport==1) ? DB_QP_LIMMAX : DB_QLIMMAX) : DB_QEXTMAX,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				p2[1]=rb.resval.rpoint[1];
			}

			p1[0]=min(p1[0],p2[0]);
			p1[1]=min(p1[1],p2[1]);
			p2[0]=max(p1[0],p2[0]);
			p2[1]=max(p1[1],p2[1]);

			SDS_rpextents(p1,p2,p3,p4,NULL);
			//p3 & p4 are extmin & extmax projection
			//p3 & p4 are extmin & extmax projection
			if(icadRealEqual(viewsize,p4[1]-p3[1])){
				//This is where you're zoomed to extents and user tries to drag bar
				if(nPos<(100-ICAD_VIEW_BSIZMAX)/2)
					viewctr[1]+=ICAD_VIEW_LNSIZE*(p4[1]-p3[1]);//dragging left
				else
					viewctr[1]-=ICAD_VIEW_LNSIZE*(p4[1]-p3[1]);//dragging right
			}else{
				//drag of bar when zoomed in to a location on drawing
				fr1=nPos/(100.0-(100.0*viewsize/(p4[1]-p3[1])));
				viewctr[1]=p4[1]-fr1*(p4[1]-p3[1]);
			}
			gr_rp2ucs(viewctr,p2,view);
			if(nSBCode==SB_THUMBPOSITION)
				SDS_ZoomIt(NULL,2,p2,NULL,NULL,NULL,NULL,NULL);
			else
				SDS_ZoomIt(NULL,3,p2,NULL,NULL,NULL,NULL,NULL);
			break;
	}
	SDS_SetUndo(IC_UNDO_COMMAND_END,(void *)"View scroll"/*DNT*/,NULL,NULL,SDS_CURDWG);

	CView::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CIcadView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if(m_pVportTabHip==NULL || m_pCurDwgView==NULL || SDS_IsDragging) return;

//4M Bugzilla 77987 19/02/02->
/*
    if(m_pRealTime)
	{
		KillTimer(RealTimeTimer_id);
		delete m_pRealTime;
		m_pRealTime = NULL;
	}
*/
  DeleteRealTime();
//<-4M 19/02/02
    sds_point DummyPt;
	SDS_EndDraggingSS(DummyPt, true);

	struct resbuf rb,rbucs,rbdcs;
	sds_point p1,p2,p3,p4;
	sds_real fr1;

	struct gr_view *view=m_pCurDwgView;
	db_drawing *flp=NULL;
	if(m_pDocument)	flp=((CIcadDoc*)m_pDocument)->m_dbDrawing;

	rbucs.restype=rbdcs.restype=RTSHORT;
	rbucs.resval.rint=1;
	rbdcs.resval.rint=2;

	sds_point viewctr;
	double viewsize;
	int tilemode,cvport;

	SDS_getvar(NULL,DB_QVIEWMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if(rb.resval.rint&1){
		cmd_ErrorPrompt(CMD_ERR_PERSPECTIVE,0);
		return;
	}
	SDS_getvar(NULL,DB_QTILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	tilemode=rb.resval.rint;
	SDS_getvar(NULL,DB_QCVPORT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	cvport=rb.resval.rint;

	if(tilemode) {
		m_pVportTabHip->get_12(rb.resval.rpoint); // VIEWCTR
		sds_trans(rb.resval.rpoint,&rbdcs,&rbucs,0,p1);
		fr1=(double)m_iWinX/(double)m_iWinY;
		m_pVportTabHip->get_40(&viewsize); // VIEWSIZE
	} else {
		SDS_getvar(NULL,DB_QVIEWCTR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		ic_ptcpy(p1,rb.resval.rpoint);
		SDS_getvar(NULL,DB_QVIEWSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		viewsize=rb.resval.rreal;
		SDS_getvar(NULL,DB_QSCREENSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		fr1=rb.resval.rpoint[0]/rb.resval.rpoint[1];
	}
	gr_ucs2rp(p1,viewctr,view);

	SDS_SetUndo(IC_UNDO_COMMAND_BEGIN,(void *)"View scroll"/*DNT*/,NULL,NULL,SDS_CURDWG);
	switch(nSBCode) {
		case SB_PAGELEFT:
			fr1*=viewsize;
			viewctr[0]-=(fr1*ICAD_VIEW_PGSIZE);
			gr_rp2ucs(viewctr,p2,view);
			SDS_ZoomIt(NULL,2,p2,NULL,NULL,NULL,NULL,NULL);
	 		break;
		case SB_PAGERIGHT:
			fr1*=viewsize;
			viewctr[0]+=(fr1*ICAD_VIEW_PGSIZE);
			gr_rp2ucs(viewctr,p2,view);
			SDS_ZoomIt(NULL,2,p2,NULL,NULL,NULL,NULL,NULL);
			break;
		case SB_LINELEFT:
			fr1*=viewsize;
			viewctr[0]-=(fr1*ICAD_VIEW_LNSIZE);
			gr_rp2ucs(viewctr,p2,view);
			SDS_ZoomIt(NULL,2,p2,NULL,NULL,NULL,NULL,NULL);
			break;
		case SB_LINERIGHT:
			fr1*=viewsize;
			viewctr[0]+=(fr1*ICAD_VIEW_LNSIZE);
			gr_rp2ucs(viewctr,p2,view);
			SDS_ZoomIt(NULL,2,p2,NULL,NULL,NULL,NULL,NULL);
			break;
		case SB_THUMBPOSITION:
			int fi1;
			sds_name elast;

			if(sds_entlast(elast)==RTERROR)fi1=1;
			else fi1=0;

			SDS_getvar(NULL,fi1 ? ((tilemode==0 && cvport==1) ? DB_QP_LIMMIN : DB_QLIMMIN) : DB_QEXTMIN,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			ic_ptcpy(p1,rb.resval.rpoint);
			SDS_getvar(NULL,fi1 ? ((tilemode==0 && cvport==1) ? DB_QP_LIMMAX : DB_QLIMMAX) : DB_QEXTMAX,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			ic_ptcpy(p2,rb.resval.rpoint);
			if(icadRealEqual(p1[0],p2[0])){
				SDS_getvar(NULL,!fi1 ? ((tilemode==0 && cvport==1) ? DB_QP_LIMMIN : DB_QLIMMIN) : DB_QEXTMIN,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				p1[0]=rb.resval.rpoint[0];
				SDS_getvar(NULL,!fi1 ? ((tilemode==0 && cvport==1) ? DB_QP_LIMMAX : DB_QLIMMAX) : DB_QEXTMAX,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				p2[0]=rb.resval.rpoint[0];
			}
			if(icadRealEqual(p1[1],p2[1])){
				SDS_getvar(NULL,!fi1 ? ((tilemode==0 && cvport==1) ? DB_QP_LIMMIN : DB_QLIMMIN) : DB_QEXTMIN,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				p1[1]=rb.resval.rpoint[1];
				SDS_getvar(NULL,!fi1 ? ((tilemode==0 && cvport==1) ? DB_QP_LIMMAX : DB_QLIMMAX) : DB_QEXTMAX,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				p2[1]=rb.resval.rpoint[1];
			}
			p1[0]=min(p1[0],p2[0]);
			p1[1]=min(p1[1],p2[1]);
			p2[0]=max(p1[0],p2[0]);
			p2[1]=max(p1[1],p2[1]);

			SDS_rpextents(p1,p2,p3,p4,NULL);
			//p3 & p4 are extmin & extmax projection
			if(icadRealEqual(viewsize*p2[0],p2[1]*(p4[0]-p3[0]))){
				//This is where you're zoomed to extents and user tries to drag bar
				if(nPos<(100-ICAD_VIEW_BSIZMAX)/2)
					viewctr[0]-=ICAD_VIEW_LNSIZE*(p4[0]-p3[0]);//dragging left
				else
					viewctr[0]+=ICAD_VIEW_LNSIZE*(p4[0]-p3[0]);//dragging right
			}else{
				//zoomed in to a spot on dwg and user drags bar
				fr1=nPos/(100-(100.0*viewsize*p2[0]/(p2[1]*(p4[0]-p3[0]))));
				viewctr[0]=p3[0]+fr1*(p4[0]-p3[0]);
			}
			gr_rp2ucs(viewctr,p2,view);
			if(nSBCode==SB_THUMBPOSITION)
				SDS_ZoomIt(NULL,2,p2,NULL,NULL,NULL,NULL,NULL);
			else
				SDS_ZoomIt(NULL,3,p2,NULL,NULL,NULL,NULL,NULL);

			break;
	}
	SDS_SetUndo(IC_UNDO_COMMAND_END,(void *)"View scroll"/*DNT*/,NULL,NULL,SDS_CURDWG);

	CView::OnHScroll(nSBCode, nPos, pScrollBar);
}

//Similar to MFCs private function : _AfxOleGetObjectDescriptor
HGLOBAL IcadOleGetObjectDescriptorData(
	LPOLEOBJECT 	lpOleObj,
	LPCOLESTR		lpszSrcOfCopy,
	DWORD			dwDrawAspect,
	POINTL			pointl,
	LPSIZEL 		lpSizelHim)
{
	CLSID			clsid;
	LPOLESTR		lpszFullUserTypeName = NULL;
	LPMONIKER		lpSrcMonikerOfCopy = NULL;
	IBindCtx		*pbc = NULL;
	SCODE			sc;
	SIZEL			sizel;
	BOOL			bFreeSrcOfCopy = FALSE;
	LPOLESTR		lpszBuf = NULL;
	DWORD			dwStatus = 0;

	// Get CLSID
	sc = lpOleObj->GetUserClassID(&clsid);
	if (sc != S_OK)
		clsid = CLSID_NULL;

	// Get FullUserTypeName
	sc = lpOleObj->GetUserType(USERCLASSTYPE_FULL, &lpszFullUserTypeName);

	if (lpszSrcOfCopy == NULL)
	{
		sc = lpOleObj->GetMoniker(OLEGETMONIKER_TEMPFORUSER, OLEWHICHMK_OBJFULL, &lpSrcMonikerOfCopy);
		if (sc == S_OK)
		{
			CreateBindCtx(0, &pbc);
			lpSrcMonikerOfCopy->GetDisplayName(pbc, NULL,(LPOLESTR*)&lpszSrcOfCopy);
			pbc->Release();
			bFreeSrcOfCopy = TRUE;
		}
	}

	if (lpSizelHim)
	{
		// Use extents passed by the caller
		sizel = *lpSizelHim;
	}
	else
	{
		sizel.cx = sizel.cy = 0;
	}

	// Get dwStatus
	sc = lpOleObj->GetMiscStatus(dwDrawAspect, &dwStatus);
	if (sc != S_OK)
		dwStatus = 0;

	HGLOBAL 	hMem = NULL;
	LPOBJECTDESCRIPTOR lpOD;
	DWORD		dwObjectDescSize, dwFullUserTypeNameLen, dwSrcOfCopyLen;

	// Get the length of Full User Type Name; Add 1 for the null terminator
	dwFullUserTypeNameLen = lpszFullUserTypeName ? ocslen(lpszFullUserTypeName)+1 : 0;

	// Get the Source of Copy string and it's length;
	//	Add 1 for the null terminator
	if (lpszSrcOfCopy && lpszSrcOfCopy[0] != '\0')
	   dwSrcOfCopyLen = ocslen(lpszSrcOfCopy)+1;
	else
	{
	   // No src moniker so use user type name as source string.
	   lpszSrcOfCopy =	lpszFullUserTypeName;
	   dwSrcOfCopyLen = dwFullUserTypeNameLen;
	}

	// Allocate space for OBJECTDESCRIPTOR and the additional string data
	dwObjectDescSize = sizeof(OBJECTDESCRIPTOR);
	hMem = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE | GMEM_ZEROINIT,
	   dwObjectDescSize + (dwFullUserTypeNameLen + dwSrcOfCopyLen) *
		sizeof(OLECHAR));
	if (!hMem)
		return NULL;

	lpOD = (LPOBJECTDESCRIPTOR)GlobalLock(hMem);

	// Set the FullUserTypeName offset and copy the string
	if (lpszFullUserTypeName)
	{
		lpOD->dwFullUserTypeName = dwObjectDescSize;
		ocscpy((LPOLESTR)((LPBYTE)lpOD+lpOD->dwFullUserTypeName), lpszFullUserTypeName);
	}
	else
		lpOD->dwFullUserTypeName = 0;  // zero offset indicates that string is not present

	// Set the SrcOfCopy offset and copy the string
	if (lpszSrcOfCopy)
	{
		lpOD->dwSrcOfCopy = dwObjectDescSize + dwFullUserTypeNameLen * sizeof(OLECHAR);
		ocscpy((LPOLESTR)((LPBYTE)lpOD+lpOD->dwSrcOfCopy), lpszSrcOfCopy);
	}
	else
		lpOD->dwSrcOfCopy = 0;	// zero offset indicates that string is not present

	// Initialize the rest of the OBJECTDESCRIPTOR
	lpOD->cbSize	   = dwObjectDescSize +
		(dwFullUserTypeNameLen + dwSrcOfCopyLen) * sizeof(OLECHAR);
	lpOD->clsid 	   = clsid;
	lpOD->dwDrawAspect = dwDrawAspect;
	lpOD->sizel 	   = sizel;
	lpOD->pointl	   = pointl;
	lpOD->dwStatus	   = dwStatus;


	// Clean up
	CoTaskMemFree(lpszFullUserTypeName);
	if (bFreeSrcOfCopy)
		CoTaskMemFree((LPOLESTR)lpszSrcOfCopy);
	if (lpSrcMonikerOfCopy) lpSrcMonikerOfCopy->Release();

	GlobalUnlock(hMem);
	return hMem;
}

void CIcadView::CopyToClipboard()
{

	CIcadDoc* pDoc = GetDocument();
	if(NULL==pDoc) return;
	CIcadApp* pApp = (CIcadApp*)AfxGetApp();
	if(NULL==pApp) return;

	POINTL	pointl = { 0, 0 };
	SIZEL	sizel;
	UINT	cfObjectDescriptor ;
	HGLOBAL hGlobal = NULL;
	LPOLEOBJECT pOleObj = NULL;

	sds_name ssToCopy;

	//////////////////////////////////////////////////////////////////////////////
	//If one OLE item has been selected, copy it to the Clipboard and return
	//*** For now, just copy any selected OLE items to the clipboard.
	if(NULL!=m_pSelection)
	{
		m_pSelection->CopyToClipboard(TRUE);
		return;
	}

	//////////////////////////////////////////////////////////////////////////////
	// Get the object descriptor format

	sizel.cx = 0;
	sizel.cy = 0;
	pOleObj = (LPOLEOBJECT) pDoc->GetInterface (&IID_IOleObject);
	cfObjectDescriptor= ::RegisterClipboardFormat(_T(ResourceString(IDC_ICADVIEW_OBJECT_DESCRIP_10, "Object Descriptor" )));
	if (pOleObj)
		hGlobal = IcadOleGetObjectDescriptorData (pOleObj, NULL, DVASPECT_CONTENT, pointl, &sizel);

	if (hGlobal == NULL)
		AfxThrowMemoryException();


	//////////////////////////////////////////////////////////////////////////////
	// Get the Bitmap, Metafile and Enhanced Metafile formats,
	// Also get the IntelliCAD entities format

	HBITMAP			hBmp = NULL;
	HENHMETAFILE	hEnhMetaFile = NULL;
	HANDLE			hIcadData = NULL;
	HANDLE			hMfp = NULL;
	HMETAFILE		hMf = NULL;
	METAFILEPICT	*pmfp = NULL;

	struct resbuf rb;
	SDS_getvar(NULL,DB_QFILEDIA,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	int filedia=rb.resval.rint;

	// Copy m_pGripSelection to ssToCopy
	SDS_sscpy( ssToCopy, pDoc->m_pGripSelection );
	// Send notification to apps with ss of entities to be copied
	SDS_CallUserCallbackFunc( SDS_CBBEGINCLONE, (void *)ssToCopy, NULL, NULL );

	//note - trying to generate a metafile using CreatePictureFromSS has problems.
	//It is better to let it create the enhanced metafile, and then extract the metafile from it.
	if(!CreatePictureFromSS(ssToCopy,&hBmp,&hEnhMetaFile, NULL,
		&hIcadData,FALSE,TRUE))
		{
		if (filedia)
			sds_alert(ResourceString(IDC_ICADVIEW_CANNOT_GET_THE_11, "Cannot get the display formats." ));
		else
			sds_printf(ResourceString(IDC_ICADVIEW_CANNOT_GET_THE_11, "Cannot get the display formats." ));
		}

	if (hEnhMetaFile)	//convert this to a regular metafile
		{
		HDC thisDC = ::GetDC(m_hWnd);
		UINT Size = 0;
		LPBYTE Data;
		RECT ClientRect;

		//HIMETRIC would be preferrable, but it appears that AutoCAD does not like HIMETRIC
		//metafiles - they have to be ANISOTROPIC for AutoCAD to recognize it.
		Size = ::GetWinMetaFileBits (hEnhMetaFile, 0, NULL, /*MM_HIMETRIC*/MM_ANISOTROPIC, thisDC);
		if (Size && (Data = (LPBYTE) ::LocalAlloc (LMEM_FIXED, Size)))
			{
			Size = ::GetWinMetaFileBits (hEnhMetaFile, Size, Data, /*MM_HIMETRIC*/MM_ANISOTROPIC, thisDC);
			hMf = ::SetMetaFileBitsEx (Size, (LPBYTE) Data);
			::LocalFree(Data);
			}
		if (hMf)
			{
			hMfp = ::GlobalAlloc(GHND | GMEM_DDESHARE, sizeof(METAFILEPICT));
			if (hMfp)
				{
				DWORD x, y;
				pmfp = (METAFILEPICT *) ::GlobalLock(hMfp);
				pmfp->mm = /*MM_HIMETRIC*/MM_ANISOTROPIC;

				GetClientRect(&ClientRect);
				x = ClientRect.right - ClientRect.left;
				x *= 2540;
				x /= GetDeviceCaps(thisDC, LOGPIXELSX);
				pmfp->xExt = x; 							   // ie. in 0.01mm

				y = ClientRect.bottom - ClientRect.top;
				y *= 2540;
				y /= GetDeviceCaps(thisDC, LOGPIXELSY);
				pmfp->yExt = y; 							   // ie. in 0.01mm

				pmfp->hMF = ::CopyMetaFile (hMf, NULL);
				::GlobalUnlock(hMfp);
				}
			}
		}


	//////////////////////////////////////////////////////////////////////////////
	// Get the AutoCAD.R14 format and the OLE Embed Source format.

	LPSTORAGE	pStg = NULL;
	HGLOBAL		hGlobalACAD = NULL;

	///create a temporary file on disk and save the dwg to it
	char szPathName[IC_FILESIZE+IC_PATHSIZE], *pszPathName;
	if( ( pszPathName = _tempnam( ResourceString(IDC_ICADVIEW_C___TMP_12, "c:\\tmp" ), ResourceString(DNT_ICADVIEW_DWG_13, "DWG" ) ) ) == NULL )
		{
		sds_alert(ResourceString(IDC_ICADVIEW_CANNOT_CREATE__14, "Cannot create a unique filename." ));
		}
	else
		{
		strncpy(szPathName,pszPathName,sizeof(szPathName));
		IC_FREE(pszPathName);
		ic_setext(szPathName,"dwg"/*DNT*/);
		CCloneHelper ch(IC_DWG, IC_ACAD2004);
		if (ch.wblock(ssToCopy, szPathName) == 0)
			{
			if (filedia)
				sds_alert (ResourceString(IDC_ICADVIEW_FAILED_TO_CREA_16, "Failed to create compatible clipboard format." ));
			else
				sds_printf (ResourceString(IDC_ICADVIEW_FAILED_TO_CREA_16, "Failed to create compatible clipboard format." ));
			}
		else
			{
			struct _stat buf;
			void *buffer, *buffer2 = NULL;
			int bufsize = 0;
			//read the data from the temp file into a buffer
			if (!_stat( szPathName, &buf ))
				bufsize = buf.st_size;
			hGlobalACAD = ::GlobalAlloc (GHND | GMEM_SHARE, bufsize);
			buffer2 = new BYTE [ bufsize];

			//get data for the AutoCAD format
			if (hGlobalACAD)
				{
				buffer = ::GlobalLock(hGlobalACAD);
				FILE *fp = fopen (szPathName, "rb"/*DNT*/);
				if (fp)
					{
					int n = 0;
					n = fread (buffer, 1, bufsize, fp);
					ASSERT (n == bufsize);
					if (buffer2)
						{
						fseek (fp, 0, SEEK_SET);
						n = fread (buffer2, 1, bufsize, fp);
						ASSERT (n == bufsize);
						}
					fclose (fp);
					}
				unlink(szPathName);
				::GlobalUnlock(hGlobalACAD);
				}

			//get data for the OLE Embed Source Format
			if (buffer2)
				{
				LPSTREAM	lpStream = NULL;
				ILockBytes	*pLkbytes = NULL;
				HRESULT		hr;
				hr = ::CreateILockBytesOnHGlobal(NULL, TRUE, &pLkbytes);
				if (SUCCEEDED(hr) && pLkbytes)
					{
					hr = ::StgCreateDocfileOnILockBytes(pLkbytes,
						STGM_SHARE_EXCLUSIVE|STGM_CREATE|STGM_READWRITE, 0, &pStg);
					if (SUCCEEDED(hr) && pStg)
						{
						hr = ::WriteClassStg (pStg, CLSID_Document);
						// ToDo GH OLESTR Return Code for Macro
						//hr = pStg->CreateStream( OLESTR(OleResourceString(IDC_ICADVIEW_CONTENTS_17, "Contents" )),
						hr = pStg->CreateStream( OLESTR("Contents"),
							STGM_DIRECT | STGM_CREATE | STGM_READWRITE | STGM_SHARE_EXCLUSIVE,
							0, 0, &lpStream	);
						if (SUCCEEDED(hr) && lpStream)
							{
							LARGE_INTEGER lint = {0,0};
							unsigned long size;
							hr = lpStream->Seek( lint, STREAM_SEEK_SET, NULL);
							if (SUCCEEDED(hr))
								{
								hr = lpStream->Write( buffer2, bufsize, &size);
								}
							lpStream->Release();
							}
						}
					pLkbytes->Release();
					}
				IC_FREE(buffer2);
				}
			}
		}


/*	//This technique was the old way, and was really nice, but does not support the
	//OLE formats - so we has to switch to using the COleDataSource.  The nice thing
	//about using this format was that the OS would extract additional formats like
	//Metafile (based on EnhMetafile) and CF_DIB (based on Bitmap).  Now that we are
	//using the COleDataSource object, we have to explicitly set the Metafile format.

	OpenClipboard();
	EmptyClipboard();

	if(hGlobalACAD)
	{
		SetClipboardData(::RegisterClipboardFormat(_T("AutoCAD.r14")), hGlobalACAD);
	}
	SetClipboardData(CF_BITMAP,hBmp);
	SetClipboardData(CF_ENHMETAFILE,hEnhMetaFile);
	SetClipboardData(pApp->m_uIcadClipFormat,hIcadData);
	if(hGlobal)
	{
		SetClipboardData((CLIPFORMAT)cfObjectDescriptor,hGlobal);
	}

	CloseClipboard();
*/
/*
	IMPORTANT NOTE:  Pasting into ACAD imposes some special requirements.  The CF_METAFILEPICT
	format HAS to be present - else it will not recognize other formats like AutoCAD.r14.
	Also this Metafile cannot be in HIMETRIC map mode.
*/


	//////////////////////////////////////////////////////////////////////////////
	//Set data to the Clipboard

	//Create the MFC Data Source object - we do not have to delete it - OLE takes care of it.
	COleDataSource *pDataSource = new COleDataSource;
	if (pDataSource)
		{
		pDataSource->Empty();

		STGMEDIUM stgm1, stgm2, stgm3, stgm4;

		//Cache the AutoCAD format
		if (hGlobalACAD)
			{
			pDataSource->CacheGlobalData (::RegisterClipboardFormat(_T("AutoCAD.r14"/*DNT*/)), hGlobalACAD);
			}

		//Cache the OLE format - Embed Source
		if (pStg)
			{
			stgm1.tymed = TYMED_ISTORAGE;
			stgm1.pstg = pStg;
			stgm1.pUnkForRelease = NULL;
			pDataSource->CacheData (::RegisterClipboardFormat(_T(ResourceString(IDC_ICADVIEW_EMBED_SOURCE_18, "Embed Source" ))), &stgm1);
			}

		//Cache the IntelliCAD native format
		if (hIcadData)
			{
			pDataSource->CacheGlobalData (pApp->m_uIcadClipFormat, hIcadData);
			}

		//Cache the Bitmap
		if (hBmp)
			{
			stgm2.tymed = TYMED_GDI;
			stgm2.hBitmap = hBmp;
			stgm2.pUnkForRelease = NULL;
			pDataSource->CacheData (CF_BITMAP, &stgm2);
			}

		//Cache the Enhanced Metafile
		if (hEnhMetaFile)
			{
			stgm3.tymed = TYMED_ENHMF;
			stgm3.hEnhMetaFile = hEnhMetaFile; //::CopyEnhMetaFile (hEnhMetaFile, NULL);
			stgm3.pUnkForRelease = NULL;
			pDataSource->CacheData (CF_ENHMETAFILE, &stgm3);
			}

		if (hMfp)
			{
			stgm4.tymed = TYMED_MFPICT;
			stgm4.hMetaFilePict = hMfp;
			stgm4.pUnkForRelease = NULL;
			pDataSource->CacheData (CF_METAFILEPICT, &stgm4);
			}


		//Cache the Object Descriptor format
		if(hGlobal)
			{
			pDataSource->CacheGlobalData ((CLIPFORMAT)cfObjectDescriptor, hGlobal);
			}

		//Set the data to the clipboard
		pDataSource->SetClipboard();
		pDataSource->FlushClipboard();
		}

	//Clean up
	sds_ssfree( ssToCopy );
}


int
CIcadView::SaveBitmapToBuffer( char **ppBuffer, long *bytes, bool ChgBackgnd )
	{
	return ::SDS_SaveBitmapToBuffer( GetFrameBufferCDC()->GetCurrentBitmap(), ppBuffer, bytes, ChgBackgnd );
	}

BOOL CIcadView::CreatePictureFromSS(sds_name ssEnts, HBITMAP* phBmp,
	HENHMETAFILE* phEnhMetaFile, ICADWMFINFO* pWmfInfo, HANDLE* phIcadData,
	BOOL bPaintBkgnd, BOOL bHIMETRIC)
{
	//*** Get the extents of the entities.
	long lLenSelSet=0L;
	sds_name ename;
	CRect rectBmp;
	GetClientRect(rectBmp);
	CIcadDoc* pDoc = GetDocument();
	if(RTNORM!=sds_sslength(ssEnts,&lLenSelSet) || lLenSelSet<=0L || NULL==pDoc)
	{
		return FALSE;
	}
	BeginWaitCursor();
	double dSaveProg;
	SDS_ProgressStart();
	m_pMain->m_bMapToRGBColor=TRUE;

	// Modified Cybage VSB 05/04/2001 (dd/mm/yy)[
	// Reason: Fix for the EMF bug reported Ben Thum email dated 03/04/2001
	/*
	//Switch the background to white temporarily - this will enable correct creation of metafiles etc.
	//Before switching the background, also temporarily turn off the WNDLMDI variable - to prevent a redraw to the screen
	resbuf rbmdi;
	SDS_getvar(NULL,DB_QWNDLMDI,&rbmdi,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	int oldLmdi = rbmdi.resval.rint;
	rbmdi.resval.rint=0;
	SDS_setvar(NULL,DB_QWNDLMDI,&rbmdi,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);

	resbuf rbBkgnd;
	sds_getvar("BKGCOLOR"/*DNT* /,&rbBkgnd);
	int oldBackColor = rbBkgnd.resval.rint;
	rbBkgnd.resval.rint = 7;
	sds_setvar("BKGCOLOR"/*DNT * /,&rbBkgnd);
	*/
	// Modified Cybage VSB 05/04/2001  ]


	//*** TODO - Set the size of the extents of the entities in the selection set.
	//*** Set the size to the size of the view.
	CClientDC dcClient(this);
	CDC* pMemDC = NULL;
	CBitmap* pbmMem = NULL;
	CBitmap* pbmOld = NULL;
	HPEN hOldPenBmp = NULL;
	HBRUSH hOldBrushBmp = NULL;
	CBrush bkgndBrush(SDS_BrushColorFromACADColor(256)); //*** Background color
	if(NULL!=phBmp)
	{
		//*** Create the bitmap memory device context
		pMemDC = new CDC();
		pMemDC->CreateCompatibleDC(&dcClient);
		//*** Create the compatible bitmap.
		pbmMem = new CBitmap();
		pbmMem->CreateCompatibleBitmap(&dcClient,rectBmp.Width(),rectBmp.Height());
		pbmOld = pMemDC->SelectObject(pbmMem);
		//*** Select a temporary pen and brush into the device context.
		hOldPenBmp = (HPEN)::SelectObject(pMemDC->GetSafeHdc(),
			::CreatePen(PS_SOLID,1,RGB(255,255,255)));
		hOldBrushBmp = (HBRUSH)::SelectObject(pMemDC->GetSafeHdc(),
			::CreateSolidBrush(RGB(0,0,0)));
		//*** Always paint the background of a bitmap.
		::FillRect(pMemDC->GetSafeHdc(),rectBmp,bkgndBrush);
	}
	//*** Convert the extents to HIMETRIC.
	CMetaFileDC* pdcEnhMetaFile = NULL;
	HPEN hOldPenEnhWmf = NULL;
	HBRUSH hOldBrushEnhWmf = NULL;
	if(NULL!=phEnhMetaFile)
	{
		//*** Create the windows metafile.
		pdcEnhMetaFile = new CMetaFileDC();
		CSize sizeExtents;
		sizeExtents=rectBmp.Size();
		if(bHIMETRIC)
		{
			int nPrevMapMode = dcClient.SetMapMode(MM_HIMETRIC);
			dcClient.DPtoHIMETRIC(&sizeExtents);
			dcClient.SetMapMode(nPrevMapMode);
		}
		CRect rectExtents(0,0,sizeExtents.cx,sizeExtents.cy);
		pdcEnhMetaFile->CreateEnhanced(&dcClient,NULL,rectExtents,NULL);
		//*** Select a temporary pen and brush into the device context.
		hOldPenEnhWmf = (HPEN)::SelectObject(pdcEnhMetaFile->m_hDC,
			::CreatePen(PS_SOLID,1,RGB(255,255,255)));
		hOldBrushEnhWmf = (HBRUSH)::SelectObject(pdcEnhMetaFile->m_hDC,
			::CreateSolidBrush(RGB(0,0,0)));
		if(bPaintBkgnd)
		{
			//*** Paint the background.
			::FillRect(pdcEnhMetaFile->m_hDC,rectExtents,bkgndBrush);
		}
		else
		{
			//*** TODO - Draw XOR so white entities show up on a white background.
//			::SetROP2(pdcEnhMetaFile->m_hDC,R2_XORPEN); // Didn't work
		}
		pdcEnhMetaFile->m_bPrinting = TRUE;
	}
	CMetaFileDC* pdcMetaFile = NULL;
	HPEN hOldPenWmf = NULL;
	HBRUSH hOldBrushWmf = NULL;
	if(NULL!=pWmfInfo)
	{
		//*** Create the windows metafile.
		pdcMetaFile = new CMetaFileDC();
		pdcMetaFile->Create(NULL);
		//*** Select a temporary pen and brush into the device context.
		hOldPenWmf = (HPEN)::SelectObject(pdcMetaFile->m_hDC,
			::CreatePen(PS_SOLID,1,RGB(255,255,255)));
		hOldBrushWmf = (HBRUSH)::SelectObject(pdcMetaFile->m_hDC,
			::CreateSolidBrush(RGB(0,0,0)));
		if(bPaintBkgnd)
		{
			//*** Paint the background.
			CPoint pt(0,0);
			CRect rectExtents(pt,rectBmp.Size());
			::FillRect(pdcMetaFile->m_hDC,rectExtents,bkgndBrush);
			pWmfInfo->rectWmf.right=rectExtents.right;
			pWmfInfo->rectWmf.bottom=rectExtents.bottom;
		}
		pWmfInfo->inch=dcClient.GetDeviceCaps(LOGPIXELSX);
		pdcMetaFile->m_bPrinting = TRUE;
	}

	//*** Draw the entities on the device contexts.
	long lIndex=0L;
	struct sds_resbuf *elist=NULL,*tmprb;
	long* plIcadData = NULL;
	//*** Get a count of entities including subents
	if(NULL!=phIcadData)
	{
		int nCount=0;
		for(lIndex=0L; RTNORM==sds_ssname(ssEnts,lIndex,ename); lIndex++)
		{
			elist=sds_entget(ename);
			ic_assoc(elist,0);
			tmprb=ic_rbassoc;
			BOOL bCheckSeqEnd = FALSE;
			if(strsame("POLYLINE",tmprb->resval.rstring))
			{
				bCheckSeqEnd=TRUE;
			}
			if(strsame("INSERT",tmprb->resval.rstring))
			{
				//*** Make sure the attributes follow flag is set
				if(ic_assoc(elist,66)==0 && ic_rbassoc->resval.rint)
				{
					bCheckSeqEnd=TRUE;
				}
			}
			if(bCheckSeqEnd)
			{
				for(;;)
				{
					if(RTNORM!=sds_entnext_noxref(ename,ename))
					{
						break;
					}
					nCount++;
					IC_RELRB(elist);
					elist=sds_entget(ename);
					ic_assoc(elist,0);
					tmprb=ic_rbassoc;
					if(strsame("SEQEND",tmprb->resval.rstring))
					{
						break;
					}
				}
				IC_RELRB(elist);
			}
			nCount++;
			IC_RELRB(elist);
		}
		//*** Setup the handle to the icad data for a selection set.
		(*phIcadData) = GlobalAlloc(GHND | GMEM_DDESHARE,(nCount+4)*sizeof(long));
		plIcadData = (long*)GlobalLock(*phIcadData);
		plIcadData[0]=(long)m_pMain;
		plIcadData[1]=nCount;
		plIcadData[2]=(long)pDoc->m_dbDrawing;
		plIcadData[3]=GetCurrentProcessId();//add the process id as the 4th entry in this data, to prevent copying across 2 ICAD processes.  AM 11/6/98
	}

	//*** Draw the selected entities.
	long lidxArray=4L;
	lIndex=0L;
	while(RTNORM==sds_ssname(ssEnts,lIndex,ename))
	{
		if(NULL!=pMemDC)
		{
			//*** Bitmap
			m_LastCol=m_LastHl=m_LastRop=m_LastWid=(-2);
			m_FillLastCol=m_FillLastHl=m_FillLastRop=(-2);
			SDS_DrawEntity((db_handitem*)ename[0],pMemDC,pDoc->m_dbDrawing,this);
			//if the item is an insert, draw any attributes it might have
			if(DB_INSERT==IC_TYPE_FROM_ENAME(ename))
				{
				//*** Make sure the attributes follow flag is set
				int attflag;
				sds_name enameAtt;
				enameAtt[0] = ename[0];
				enameAtt[1] = ename[1];
				if((((db_handitem*)enameAtt[0])->get_66(&attflag)) && attflag)
					{
					for(;;)
						{
						if(RTNORM!=sds_entnext_noxref(enameAtt,enameAtt))
							break;
						if(DB_SEQEND==IC_TYPE_FROM_ENAME(enameAtt))
							break;
						SDS_DrawEntity((db_handitem*)enameAtt[0],pMemDC,pDoc->m_dbDrawing,this);
						}
					}
				}
		}
		if(NULL!=pdcEnhMetaFile)
		{
			//*** Enhanced Metafile
			m_LastCol=m_LastHl=m_LastRop=m_LastWid=(-2);
			m_FillLastCol=m_FillLastHl=m_FillLastRop=(-2);
			SDS_DrawEntity((db_handitem*)ename[0],pdcEnhMetaFile,pDoc->m_dbDrawing,this);
			//if the item is an insert, draw any attributes it might have
			if(DB_INSERT==IC_TYPE_FROM_ENAME(ename))
				{
				//*** Make sure the attributes follow flag is set
				int attflag;
				sds_name enameAtt;
				enameAtt[0] = ename[0];
				enameAtt[1] = ename[1];
				if((((db_handitem*)enameAtt[0])->get_66(&attflag)) && attflag)
					{
					for(;;)
						{
						if(RTNORM!=sds_entnext_noxref(enameAtt,enameAtt))
							break;
						if(DB_SEQEND==IC_TYPE_FROM_ENAME(enameAtt))
							break;
						SDS_DrawEntity((db_handitem*)enameAtt[0],pdcEnhMetaFile,pDoc->m_dbDrawing,this);
						}
					}
				}
		}
		if(NULL!=pdcMetaFile)
		{
			//*** Windows Metafile
			m_LastCol=m_LastHl=m_LastRop=m_LastWid=(-2);
			m_FillLastCol=m_FillLastHl=m_FillLastRop=(-2);
			SDS_DrawEntity((db_handitem*)ename[0],pdcMetaFile,pDoc->m_dbDrawing,this);
			//if the item is an insert, draw any attributes it might have
			if(DB_INSERT==IC_TYPE_FROM_ENAME(ename))
				{
				//*** Make sure the attributes follow flag is set
				int attflag;
				sds_name enameAtt;
				enameAtt[0] = ename[0];
				enameAtt[1] = ename[1];
				if((((db_handitem*)enameAtt[0])->get_66(&attflag)) && attflag)
					{
					for(;;)
						{
						if(RTNORM!=sds_entnext_noxref(enameAtt,enameAtt))
							break;
						if(DB_SEQEND==IC_TYPE_FROM_ENAME(enameAtt))
							break;
						SDS_DrawEntity((db_handitem*)enameAtt[0],pdcMetaFile,pDoc->m_dbDrawing,this);
						}
					}
				}
		}
		if(NULL!=plIcadData)
		{
			//*** Icad data, keeping subents in mind.
			plIcadData[lidxArray++]=ename[0];
			elist=sds_entget(ename);
			ic_assoc(elist,0);
			tmprb=ic_rbassoc;
			BOOL bCheckSeqEnd = FALSE;
			if(strsame("POLYLINE",tmprb->resval.rstring))
			{
				bCheckSeqEnd=TRUE;
			}
			if(strsame("INSERT",tmprb->resval.rstring))
			{
				//*** Make sure the attributes follow flag is set
				if(ic_assoc(elist,66)==0 && ic_rbassoc->resval.rint)
				{
					bCheckSeqEnd=TRUE;
				}
			}
			if(bCheckSeqEnd)
			{
				for(;;)
				{
					if(RTNORM!=sds_entnext_noxref(ename,ename))
					{
						break;
					}
					plIcadData[lidxArray++]=ename[0];
					IC_RELRB(elist);
					elist=sds_entget(ename);
					ic_assoc(elist,0);
					tmprb=ic_rbassoc;
					if(strsame("SEQEND",tmprb->resval.rstring))
					{
						break;
					}
				}
				IC_RELRB(elist);
			}
			IC_RELRB(elist);
		}

		dSaveProg = (((double)lIndex/(double)lLenSelSet)*100);
		SDS_ProgressPercent((int)dSaveProg);
		lIndex++;
	}

	// Modified Cybage VSB 05/04/2001 (dd/mm/yy)[
	// Reason: Fix for the EMF bug reported Ben Thum email dated 03/04/2001
	/*
	//reset the WNDLMDI to its original value
	rbmdi.resval.rint=oldLmdi;
	SDS_setvar(NULL,DB_QWNDLMDI,&rbmdi,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);

	//reset the Background color to its original value
	rbBkgnd.resval.rint = oldBackColor;
	sds_setvar("BKGCOLOR"/*DNT* /,&rbBkgnd);
	*/
	// Modified Cybage VSB 05/04/2001 (dd/mm/yy)]
	if(NULL!=pbmMem)
	{
		(*phBmp)=(HBITMAP)pbmMem->Detach();
		::DeleteObject(SelectObject(pMemDC->m_hDC,hOldPenBmp));
		::DeleteObject(SelectObject(pMemDC->m_hDC,hOldBrushBmp));
		pMemDC->SelectObject(pbmOld);
		delete pbmMem;
		delete pMemDC;
	}
	if(NULL!=pdcEnhMetaFile)
	{
		::DeleteObject(SelectObject(pdcEnhMetaFile->m_hDC,hOldPenEnhWmf));
		::DeleteObject(SelectObject(pdcEnhMetaFile->m_hDC,hOldBrushEnhWmf));
		pdcEnhMetaFile->m_bPrinting=FALSE;
		(*phEnhMetaFile)=pdcEnhMetaFile->CloseEnhanced();
		delete pdcEnhMetaFile;
	}
	if(NULL!=pdcMetaFile)
	{
		::DeleteObject(SelectObject(pdcMetaFile->m_hDC,hOldPenWmf));
		::DeleteObject(SelectObject(pdcMetaFile->m_hDC,hOldBrushWmf));
		pdcMetaFile->m_bPrinting=FALSE;
		pWmfInfo->hMetaFile=pdcMetaFile->Close();
		delete pdcMetaFile;
	}
	if(NULL!=plIcadData)
	{
		GlobalUnlock(*phIcadData);
	}

	m_LastCol=m_LastHl=m_LastRop=m_LastWid=(-2);
	m_FillLastCol=m_FillLastHl=m_FillLastRop=(-2);
	m_pMain->m_bMapToRGBColor=FALSE;
	SDS_ProgressStop();
	EndWaitCursor();
	return TRUE;
}

/////////////////////////////////////////////////////////////////////
// CIcadView printing

/*-------------------------------------------------------------------------*//**
Checks some things (ideally, the db) to decide if hidden lines rendering is
needed for some viewports.
NOTE: currently it checks only for tilemode/off mode; uncomment (and possibly change)
      the code once we support per-viewport hidden-line printing.

@author	Dmitry Gavrilov
@param	pDrw		=> the drawing attached to the current view
@param	renderMode	=> the current view's render mode (enum CIcadView::ViewMode),
						this param possibly will be removed in the future when
						some db object will have this flag (VIEWPORT, etc.)
@return	'true' if hl rendering is needed
*//*--------------------------------------------------------------------------*/
bool needHLrendering(db_drawing* pDrw, int renderMode)
{
	if(renderMode != CIcadView::kNormal)
		return true;
	else
	{
		/*if(!pDrw)
		{
			ASSERT(0);
			return false;
		}
		// check floating (paperspace) viewports: maybe HL rendering is needed in some of them
		short			renderMode;
		db_handitem*	pTabrec = 0;
		db_viewport*	pVp = 0;
		for(pDrw->get_tabrecllends(DB_VXTABREC, &pTabrec, 0); pTabrec; pTabrec = pTabrec->next)
			if(!pTabrec->ret_deleted() && (pVp = ((db_vxtabrec*)pTabrec)->ret_vpehip()) && !pVp->ret_deleted())
			{
				pVp->get_281(&renderMode);
				if(renderMode == IC_RENDERMODE_HIDDENLINE || renderMode == IC_RENDERMODE_FLAT_WITH_WIREFRAME)
					return true;
			}*/
		return false;
	}
}

//4M Item:104->
// When viewpoint!=(0,0,1) && Select Print Area is selected we should change ucs to view
// in order to print correctly
long  cmd_ucszaxis(const sds_point pp1,const sds_point pp2);
long cmd_ucsprev(void);
//<-4M Item:104

void CIcadView::FilePrint(BOOL bShowPrintDlg)
{
	CIcadApp*	pApp = (CIcadApp*)AfxGetApp();
	if(!pApp)
		return;
	db_drawing*	pDrw = m_pDocument ? ((CIcadDoc*)m_pDocument)->m_dbDrawing : 0;
	if(!pDrw)
		return;

	int	nPrintFlags = PRINTDLG_PRINTING;

	if(bShowPrintDlg)
		nPrintFlags |= PRINTDLG_DISPLAY;

	if(!pApp->IcadPrintDlg(nPrintFlags))
		return;

	/*DG - 24.6.2003*/// warn the user if hidden lines rendering is needed, and ask him/her to continue
	resbuf	expert;
	SDS_getvar(NULL, DB_QEXPERT, &expert, pDrw, &SDS_CURCFG, &SDS_CURSES);
	if( expert.resval.rint > 3 &&
		needHLrendering(pDrw, m_iViewMode) &&
		MessageBox(ResourceString(ID_HL_PRINTING_WARNING, "Hidden lines detected. Screen will be printed as is, not using pen maps.\nThe filesize can become very large.\nContinue?"),
					ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD"),
					MB_YESNO | MB_SETFOREGROUND) == IDNO )
		return;

	CheckFPControlWord();

// commented out the following code to fix the bug 78427 - SWH 01/12/2003
//
//4M Item:104->
// When viewpoint!=(0,0,1) && Select Print Area is selected we should change ucs to view
// in order to print correctly
//    struct resbuf rb;
//    SDS_getvar(NULL,DB_QVIEWDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
//	cmd_ucszaxis(rb.resval.rpoint,NULL);  
//<-4M Item:104
//        cmd_redraw();

	CRect	rect;
	GetClientRect(rect);
	m_sizePrintView = rect.Size();

	CDocument*	pDoc = GetDocument();
	CString		strOldTitle;
	if(pDoc)
		// To set the document name back.
		strOldTitle = pDoc->GetTitle();

#ifdef USE_ICAD_SPOOLING
	customOnFilePrint();
#else
	CView::OnFilePrint();
#endif

	// printer settings could change during printing because of e.g. absence of the required printer
	// (in this case the system puts default system Printer Settings to application's DEVMODE/DEVNAMES)
	pApp->putPrinterSettingsToPlotcfg();

	CheckFPControlWord();

	if(pDoc)
		// Set the document name back.
		pDoc->SetTitle(strOldTitle);

	// Free the selection set of ents printed
	if(m_pMain && m_pMain->m_pIcadPrintDlg)
	{
		sds_name	ename;
		// Un-hilight the selected entities.
		for(long ssct = 0L; sds_ssname(m_pMain->m_pIcadPrintDlg->m_scaleViewTab.m_ssPrintEnt, ssct, ename) == RTNORM; ++ssct)
			sds_redraw(ename,IC_REDRAW_UNHILITE);

		sds_ssfree(m_pMain->m_pIcadPrintDlg->m_scaleViewTab.m_ssPrintEnt);
		sds_ssadd(NULL, NULL, m_pMain->m_pIcadPrintDlg->m_scaleViewTab.m_ssPrintEnt);
	}

// commented out the following code to fix the bug 78427 - SWH 01/12/2003
	//
//4M Item:104->
// When viewpoint!=(0,0,1) && Select Print Area is selected we should change ucs to view
// in order to print correctly
//   cmd_ucsprev(); 
//<-4M Item:104
//    cmd_redrawall();
}


#ifdef USE_ICAD_SPOOLING

// Author: Dmitry Gavrilov
// All the following code of _myAfxWinState, CPrintingDialog and _AfxAbortProc is copied from MFC's sources
// because it's necessary for the following CIcadView::customOnFilePrint function but is not exported by MFC.
PROCESS_LOCAL(_AFX_WIN_STATE, _myAfxWinState)

class CPrintingDialog : public CDialog
{
public:
	//{{AFX_DATA(CPrintingDialog)
	enum { IDD = AFX_IDD_PRINTDLG };
	//}}AFX_DATA
	CPrintingDialog::CPrintingDialog(CWnd* pParent)
		{
			Create(CPrintingDialog::IDD, pParent);      // modeless !
			_myAfxWinState->m_bUserAbort = FALSE;
		}
	virtual ~CPrintingDialog() { }

	virtual BOOL OnInitDialog();
	virtual void OnCancel();
};

BOOL CPrintingDialog::OnInitDialog()
{
	SetWindowText(AfxGetAppName());
	CenterWindow();
	return CDialog::OnInitDialog();
}

void CPrintingDialog::OnCancel()
{
	_myAfxWinState->m_bUserAbort = TRUE;  // flag that user aborted print
	CDialog::OnCancel();
}

BOOL CALLBACK _AfxAbortProc(HDC, int)
{
	_AFX_WIN_STATE* pWinState = _myAfxWinState;
	MSG msg;
	while (!pWinState->m_bUserAbort &&
		::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE))
	{
		if (!AfxGetThread()->PumpMessage())
			return FALSE;   // terminate if WM_QUIT received
	}
	return !pWinState->m_bUserAbort;
}

// Author: Dmitry Gavrilov
// This function generates PRN full filename using the document title (strTitle) and stores
// it in strOutput. It uses user preference value to get output folder.
void generatePrnFileName(CString& strTitle, CString& strOutput)
{
	const char*		ext = ".prn"/*DNT*/;
	CString			fileName;

#if 1 // use 123102_150000 for date-time format
	char	buff[9];
	fileName = _strdate(buff);
	fileName.Remove('/');
	fileName += '_';
	fileName += _strtime(buff);
	fileName.Remove(':');
#else // use Tue_Dec_31_15_00_00_2002 for date-time format
	time_t	datetime;
	time(&datetime);
	fileName = ctime(&datetime);
	fileName.Replace('.', '_');
	fileName.Replace(' ', '_');
	fileName.Replace(':', '_');
	fileName.Remove('\n');
#endif

	//process dwgname (or dxf) to remove extension
	strTitle.MakeUpper();
	{
		int trimmedlength =strTitle.GetLength()- 4;
		if (trimmedlength>0 && (strTitle.Right(4)==".DWG" || strTitle.Right(4)==".DXF"))
			strTitle=strTitle.Left(trimmedlength);
	}

	char	iStr[33];
	for(unsigned long i = 0L; ; ++i)
	{
		if(i)
			strOutput = fileName + '_' + _ultoa(i, iStr, 10);
		else
			strOutput = fileName ;

	if( ((CString)UserPreference::s_outputFolder).IsEmpty() ||
		((LPCTSTR)((CString)UserPreference::s_outputFolder))[((CString)UserPreference::s_outputFolder).GetLength() - 1] == '\\'/*DNT*/ )
		strOutput = ((CString)UserPreference::s_outputFolder) + strOutput + '_' + strTitle + ext;
	else
		strOutput = ((CString)UserPreference::s_outputFolder) + '\\' + strOutput + '_' + strTitle + ext;

	if(_access((LPCTSTR)strOutput, 0))
		break;
	}
}

// Author: Dmitry Gavrilov
// This function is copied from MFC's CView::OnFilePrint. The differences are:
// 1. This function allows icad's spooling features (printing to a PRN file directly w/o iteraction with user and
// using a folder for storing PRN files set by the user), i.e. overriding system default printer settings.
// 2. Since icad uses its own print dialog w/o print-to-file checkbox, then printInfo.m_pPD->m_pd.Flags & PD_PRINTTOFILE
// is 0 always, I didn't copy according code from CView::OnFilePrint.
void CIcadView::customOnFilePrint()
{
	// get default print info
	CPrintInfo printInfo;
	ASSERT(printInfo.m_pPD != NULL);    // must be set

	if (LOWORD(GetCurrentMessage()->wParam) == ID_FILE_PRINT_DIRECT)
	{
		CCommandLineInfo* pCmdInfo = AfxGetApp()->m_pCmdInfo;

		if (pCmdInfo != NULL)
		{
			if (pCmdInfo->m_nShellCommand == CCommandLineInfo::FilePrintTo)
			{
				printInfo.m_pPD->m_pd.hDC = ::CreateDC(pCmdInfo->m_strDriverName,
					pCmdInfo->m_strPrinterName, pCmdInfo->m_strPortName, NULL);
				if (printInfo.m_pPD->m_pd.hDC == NULL)
				{
					AfxMessageBox(AFX_IDP_FAILED_TO_START_PRINT);
					return;
				}
			}
		}

		printInfo.m_bDirect = TRUE;
	}

	if (OnPreparePrinting(&printInfo))
	{
		const char*	c_strFILE = "FILE:"/*DNT*/;
		// hDC must be set (did you remember to call DoPreparePrinting?)
		ASSERT(printInfo.m_pPD->m_pd.hDC != NULL);

		// set up document info and start the document printing process
		CString strTitle, strOutput;
		CDocument* pDoc = GetDocument();
		if (pDoc != NULL)
			strTitle = pDoc->GetTitle();
		else
			GetParentFrame()->GetWindowText(strTitle);

		CString	strPortName = printInfo.m_pPD->GetPortName();
		if((bool)UserPreference::s_bUseOutputFolder && ((bool)UserPreference::s_bForcePrintToFile || !strPortName.CompareNoCase(c_strFILE)))
			generatePrnFileName(strTitle, strOutput);
		else
			if((bool)UserPreference::s_bForcePrintToFile)
				strPortName = c_strFILE;

		if (strTitle.GetLength() > 31)
			strTitle.ReleaseBuffer(31);
		DOCINFO docInfo;
		memset(&docInfo, 0, sizeof(DOCINFO));
		docInfo.cbSize = sizeof(DOCINFO);
		docInfo.lpszDocName = strTitle;
		int nFormatID;
		if(strOutput.IsEmpty() && !(bool)UserPreference::s_bForcePrintToFile)
		{
			docInfo.lpszOutput = NULL;
			strPortName = printInfo.m_pPD->GetPortName();
			nFormatID = AFX_IDS_PRINTONPORT;
		}
		else
		{
			if(strOutput.IsEmpty())
				docInfo.lpszOutput = c_strFILE;
			else
			{
				docInfo.lpszOutput = strOutput;
				AfxGetFileTitle(strOutput, strPortName.GetBuffer(_MAX_PATH), _MAX_PATH);
			}
			nFormatID = AFX_IDS_PRINTTOFILE;
		}

		// setup the printing DC
		CDC dcPrint;
		if (!printInfo.m_bDocObject)
		{
			dcPrint.Attach(printInfo.m_pPD->m_pd.hDC);  // attach printer dc
			dcPrint.m_bPrinting = TRUE;
		}
		OnBeginPrinting(&dcPrint, &printInfo);

		if (!printInfo.m_bDocObject)
			dcPrint.SetAbortProc(_AfxAbortProc);

		// disable main window while printing & init printing status dialog
		AfxGetMainWnd()->EnableWindow(FALSE);
		CPrintingDialog dlgPrintStatus(this);

		CString strTemp;
		dlgPrintStatus.SetDlgItemText(AFX_IDC_PRINT_DOCNAME, strTitle);
		dlgPrintStatus.SetDlgItemText(AFX_IDC_PRINT_PRINTERNAME,
			printInfo.m_pPD->GetDeviceName());
		AfxFormatString1(strTemp, nFormatID, strPortName);
		dlgPrintStatus.SetDlgItemText(AFX_IDC_PRINT_PORTNAME, strTemp);
		dlgPrintStatus.ShowWindow(SW_SHOW);
		dlgPrintStatus.UpdateWindow();

		// start document printing process
		if (!printInfo.m_bDocObject && dcPrint.StartDoc(&docInfo) == SP_ERROR)
		{
			// enable main window before proceeding
			AfxGetMainWnd()->EnableWindow(TRUE);

			// cleanup and show error message
			OnEndPrinting(&dcPrint, &printInfo);
			dlgPrintStatus.DestroyWindow();
			dcPrint.Detach();   // will be cleaned up by CPrintInfo destructor
			AfxMessageBox(AFX_IDP_FAILED_TO_START_PRINT);
			return;
		}

		// Guarantee values are in the valid range
		UINT nEndPage = printInfo.GetToPage();
		UINT nStartPage = printInfo.GetFromPage();

		if (nEndPage < printInfo.GetMinPage())
			nEndPage = printInfo.GetMinPage();
		if (nEndPage > printInfo.GetMaxPage())
			nEndPage = printInfo.GetMaxPage();

		if (nStartPage < printInfo.GetMinPage())
			nStartPage = printInfo.GetMinPage();
		if (nStartPage > printInfo.GetMaxPage())
			nStartPage = printInfo.GetMaxPage();

		int nStep = (nEndPage >= nStartPage) ? 1 : -1;
		nEndPage = (nEndPage == 0xffff) ? 0xffff : nEndPage + nStep;

		VERIFY(strTemp.LoadString(AFX_IDS_PRINTPAGENUM));

		// If it's a doc object, we don't loop page-by-page
		// because doc objects don't support that kind of levity.

		BOOL bError = FALSE;
		if (printInfo.m_bDocObject)
		{
			OnPrepareDC(&dcPrint, &printInfo);
			OnPrint(&dcPrint, &printInfo);
		}
		else
		{
			// begin page printing loop
			for (printInfo.m_nCurPage = nStartPage;
				printInfo.m_nCurPage != nEndPage; printInfo.m_nCurPage += nStep)
			{
				OnPrepareDC(&dcPrint, &printInfo);

				// check for end of print
				if (!printInfo.m_bContinuePrinting)
					break;

				// write current page
				TCHAR szBuf[80];
				wsprintf(szBuf, strTemp, printInfo.m_nCurPage);
				dlgPrintStatus.SetDlgItemText(AFX_IDC_PRINT_PAGENUM, szBuf);

				// set up drawing rect to entire page (in logical coordinates)
				printInfo.m_rectDraw.SetRect(0, 0,
					dcPrint.GetDeviceCaps(HORZRES),
					dcPrint.GetDeviceCaps(VERTRES));
				dcPrint.DPtoLP(&printInfo.m_rectDraw);

				// attempt to start the current page
				if (dcPrint.StartPage() < 0)
				{
					bError = TRUE;
					break;
				}

				// must call OnPrepareDC on newer versions of Windows because
				// StartPage now resets the device attributes.
				if (afxData.bMarked4)
					OnPrepareDC(&dcPrint, &printInfo);

				ASSERT(printInfo.m_bContinuePrinting);

				// page successfully started, so now render the page
				OnPrint(&dcPrint, &printInfo);
				if (dcPrint.EndPage() < 0 || !_AfxAbortProc(dcPrint.m_hDC, 0))
				{
					bError = TRUE;
					break;
				}
			}
		}

		// cleanup document printing process
		if (!printInfo.m_bDocObject)
		{
			if (!bError)
				dcPrint.EndDoc();
			else
				dcPrint.AbortDoc();
		}

		AfxGetMainWnd()->EnableWindow();    // enable main window

		OnEndPrinting(&dcPrint, &printInfo);    // clean up after printing
		dlgPrintStatus.DestroyWindow();

		dcPrint.Detach();   // will be cleaned up by CPrintInfo destructor
	}
}

#endif // USE_ICAD_SPOOLING

void CIcadView::FilePrintPreview(void)
{
	/*DG - 25.6.2003*/// warn the user if hidden lines rendering is needed, and ask him/her to continue
	db_drawing*	pDrw = m_pDocument ? ((CIcadDoc*)m_pDocument)->m_dbDrawing : 0;
	if(!pDrw)
		return;
	resbuf	expert;
	SDS_getvar(NULL, DB_QEXPERT, &expert, pDrw, &SDS_CURCFG, &SDS_CURSES);
	if( expert.resval.rint > 3 &&
		needHLrendering(pDrw, m_iViewMode) &&
		MessageBox(ResourceString(ID_HL_PRINTPREVIEW_WARNING, "Hidden lines detected. Screen will be printed as is, not using pen maps.\nMemory and time usage can become very large.\nContinue?"),
					ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD"),
					MB_YESNO | MB_SETFOREGROUND) == IDNO )
		return;

	PRINTDLG	defaultPrinter;
	CIcadApp*	pApp = (CIcadApp*)AfxGetApp();
	if(!pApp || !pApp->GetPrinterDeviceDefaults(&defaultPrinter))
	{
		AfxMessageBox(ResourceString( IDS_PREVIEW_NOPRINTER, "Print Preview requires a default printer."));
		return;
	}

	if(!pApp->IcadPrintDlg(PRINTDLG_PREVIEW))
		return;

	m_pMain->m_bPrintPreview = TRUE;

	CRect	rect;
	GetClientRect(rect);
	m_sizePrintView = rect.Size();
	CPrintPreviewState*	pState = new CPrintPreviewState;
	// This code replaces the call to DoPrintPreview
	int	nIDResource = IDD_PRINT_PREVIEW;

	CFrameWnd*		pParent = m_pMain;
	CCreateContext	context;
	context.m_pCurrentFrame = pParent;
	context.m_pCurrentDoc = GetDocument();
	context.m_pLastView = this;

	// Create the preview view object
	CIcadPreviewView*	pPreviewView = new CIcadPreviewView();
	if(!pPreviewView)
	{
		TRACE0(ResourceString(IDC_ICADVIEW_ERROR__FAILED__23, "Error: Failed to create preview view.\n"));
		delete pState;
		return;
	}
	ASSERT_KINDOF(CPreviewView, pPreviewView);
	pPreviewView->SetPreviewState(pState);		// save pointer

	pParent->OnSetPreviewMode(TRUE, pState);	// Take over Frame Window

	// Create the toolbar from the dialog resource
	CDialogBar*	pToolBar = new CDialogBar;
	pPreviewView->SetToolBar(pToolBar);
	if(!pToolBar->Create(pParent, MAKEINTRESOURCE(nIDResource), CBRS_TOP, AFX_IDW_PREVIEW_BAR))
	{
		TRACE0(ResourceString(IDC_ICADVIEW_ERROR__PREVIEW_24, "Error: Preview could not create toolbar dialog.\n"));
		goto failurecleanup;
	}

	// set icon on help button
	CButton*	helpWnd;
	if(helpWnd = (CButton*)pToolBar->GetDlgItem(IDC_PRNPREV_HELP))
		helpWnd->SetIcon(AfxGetApp()->LoadIcon(HELP_ICON));

	// Create the preview view as a child of the App Main Window.  This
	// is a sibling of this view if this is an SDI app.  This is NOT a sibling
	// if this is an MDI app.

	if(!pPreviewView->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(0,0,0,0), pParent, AFX_IDW_PANE_FIRST, &context))
	{
		TRACE0(ResourceString(IDC_ICADVIEW_ERROR__COULDN__25, "Error: couldn't create preview view for frame.\n"));
		goto failurecleanup;
	}

	// Preview window shown now

	pState->pViewActiveOld = pParent->GetActiveView();
	//Modified Cybage MM 5/11/2001 DD/MM/YYYY[
	//Reason: Fix for bug no 77909 from BugZilla
	if(SDS_CMainWindow)
		SDS_CMainWindow->m_pCurView->m_bNeedRedraw = 1;
	//Modified Cybage MM 5/11/2001 DD/MM/YYYY]

	if(!pPreviewView->SetPrintView(this))
		goto failurecleanup;

	pToolBar->m_bAutoDelete = TRUE;			// automatic cleanup
	pParent->SetActiveView(pPreviewView);	// set active view - even for MDI

	// update toolbar and redraw everything
	pToolBar->SendMessage(WM_IDLEUPDATECMDUI, (WPARAM)TRUE);
	pParent->RecalcLayout();	// position and size everything
	pParent->UpdateWindow();

	return;

failurecleanup:
	delete pToolBar;	// not autodestruct yet
	pPreviewView->SetToolBar(NULL);
	pParent->OnSetPreviewMode(FALSE, pState);	// restore Frame Window
	pPreviewView->SetPreviewState(NULL);		// do not delete state structure
	delete pPreviewView;
	delete pState;
}


//Added Cybage AW 15/11/2001[
//Bug no : 77870 from BugZilla (Crash during print preview )
void CIcadView::OnEndPrintPreview( CDC* pDC, CPrintInfo* pInfo, POINT point, CPreviewView* pView )
{
	if(NULL==SDS_CMainWindow)
		return;
	if(NULL!=SDS_CMainWindow->m_pIcadPrintDlg)
	{
		sds_ssfree(SDS_CMainWindow->m_pIcadPrintDlg->m_scaleViewTab.m_ssPrintEnt);
		sds_ssadd(NULL,NULL,SDS_CMainWindow->m_pIcadPrintDlg->m_scaleViewTab.m_ssPrintEnt);
	}
	CView::OnEndPrintPreview(pDC, pInfo, point, pView );
}
//Added Cybage AW 15/11/2001]

afx_msg BOOL CIcadView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	pInfo->m_bDirect=TRUE;
	return DoPreparePrinting(pInfo);
}

afx_msg void CIcadView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	sds_point ptAltViewCtr;
	CPrintDialogSheet* pPrintDlg = m_pMain->m_pIcadPrintDlg;
	struct resbuf rb;
	int rc=0;
	db_drawing *flp = NULL;

	if(m_pDocument)
	{
		flp=((CIcadDoc*)m_pDocument)->m_dbDrawing;
	}

	m_ptPrintScreenSize[0]=m_iWinX;
	m_ptPrintScreenSize[1]=m_iWinY;
	m_ptPrintScreenSize[2]=0.0;

	ICPRINTINFO pi;
	memset(&pi,0,sizeof(pi));
	pi.pView = this;
	pi.pDC = pDC;
	if(!pPrintDlg->GetCurrentPrintInfo(&pi))
	{
		rc=(-__LINE__);
		goto out;
	}
	ic_ptcpy(m_ptLwrLeft,pi.ptLwrLeft);
	ic_ptcpy(m_ptUprRight,pi.ptUprRight);

	m_iWinX=(int)pi.dHorzRes;
	m_iWinY=(int)pi.dAdjustedHeight;

	rb.restype=RTPOINT;
	rb.resval.rpoint[0]=m_iWinX;
	rb.resval.rpoint[1]=m_iWinY;
	rb.resval.rpoint[2]=0.0;
	SDS_setvar(NULL,DB_QSCREENSIZE,&rb,((CIcadDoc*)m_pDocument)->m_dbDrawing,&SDS_CURCFG,&SDS_CURSES,0);

	//*** Set the alternate view center.
	ptAltViewCtr[0]=(m_ptLwrLeft[0]+m_ptUprRight[0])/2.0;
	ptAltViewCtr[1]=(m_ptLwrLeft[1]+m_ptUprRight[1])/2.0;
	ptAltViewCtr[2]=0.0;

	//*** Adjust the alternate view center if there is a header or footer.
	if(!icadRealEqual(pi.dHeaderAdjust,0.0) && icadRealEqual(pi.dFooterAdjust,0.0))
	{
		ptAltViewCtr[1] += (pi.dHeaderAdjust/pi.dPixPerDU);
	}
	else if(!icadRealEqual(pi.dFooterAdjust,0.0) && icadRealEqual(pi.dHeaderAdjust,0.0))
	{
		ptAltViewCtr[1] -= (pi.dFooterAdjust/pi.dPixPerDU);
	}
	//*** Adjust the alternate view center to the specified origin
	sds_point ptOrigin;
	ptOrigin[0]=0.0; ptOrigin[1]=0.0; ptOrigin[2]=0.0;
	
	//BugZilla No. 78572; 04-08-2003 [
	if(pPrintDlg->m_advancedTab.m_Center != 1)
	{
	if(!pPrintDlg->m_advancedTab.m_OriginX.IsEmpty())
	{
		sds_distof(pPrintDlg->m_advancedTab.m_OriginX,-1,&ptOrigin[0]);
	}
	if(!pPrintDlg->m_advancedTab.m_OriginY.IsEmpty())
	{
		sds_distof(pPrintDlg->m_advancedTab.m_OriginY,-1,&ptOrigin[1]);
	}
	}
	//BugZilla No. 78572; 04-08-2003 ]
//4M Item:99->
// Prints upside down
// Not correct computation regarding Origin point
/*
    ptAltViewCtr[0]-=ptOrigin[0];
    ptAltViewCtr[1]-=ptOrigin[1];
*/
    ptAltViewCtr[0]+=ptOrigin[0];
    ptAltViewCtr[1]+=ptOrigin[1];
//<-4M



	if(pPrintDlg->m_advancedTab.m_Center == 0)
	{
		//*** Make sure the drawing is pushed to the lower left corner.
		double dPageWidthDU = pi.dAdjustedWidth/pi.dPixPerDU;
		double cx = fabs(m_ptUprRight[0]-m_ptLwrLeft[0]);
		double cy = fabs(m_ptUprRight[1]-m_ptLwrLeft[1]);
		ptAltViewCtr[1] += ((pi.dViewSize-cy)/2.0);
		ptAltViewCtr[0] += ((dPageWidthDU-cx)/2.0);
	}

	int tilemode,cvport;
	SDS_getvar(NULL,DB_QCVPORT,&rb,((CIcadDoc*)m_pDocument)->m_dbDrawing,&SDS_CURCFG,&SDS_CURSES);
	cvport=rb.resval.rint;
	SDS_getvar(NULL,DB_QTILEMODE,&rb,((CIcadDoc*)m_pDocument)->m_dbDrawing,&SDS_CURCFG,&SDS_CURSES);
	tilemode=rb.resval.rint;

	if(tilemode==0) {
		SDS_getvar(NULL,DB_QVIEWSIZE,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
		m_rPrintViewSize=rb.resval.rreal;
		rb.resval.rreal=pi.dViewSize;
		SDS_setvar(NULL,DB_QVIEWSIZE,&rb,flp,&SDS_CURCFG,&SDS_CURSES,0);

		SDS_getvar(NULL,DB_QVIEWCTR,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
		ic_ptcpy(m_ptPrintViewCtr,rb.resval.rpoint);

		gr_rp2ucs(ptAltViewCtr,ptAltViewCtr,m_pCurDwgView);
		ic_ptcpy(rb.resval.rpoint,ptAltViewCtr);
		SDS_setvar(NULL,DB_QVIEWCTR,&rb,flp,&SDS_CURCFG,&SDS_CURSES,0);

		SDS_VarToTab(flp,m_pVportTabHip);
	} else {
		//*** Cache the current view size.
		m_pVportTabHip->get_40(&m_rPrintViewSize); // VIEWSIZE
		//*** Set the current view size to the alternate view size.
		m_pVportTabHip->set_40(pi.dViewSize); // VIEWSIZE
		//*** Cache the current view center.
		m_pVportTabHip->get_12(m_ptPrintViewCtr); // VIEWCTR
		//*** Set the current view center to the alternate view center.
	 	m_pVportTabHip->set_12(ptAltViewCtr); // VIEWCTR
	}

	if(tilemode==0 && cvport>1 && m_pVportTabHip->ret_type()==DB_VIEWPORT) {
		gr_initview(((CIcadDoc*)m_pDocument)->m_dbDrawing,&SDS_CURCFG,&m_pCurDwgView,NULL);
	} else if(tilemode==0 && cvport==1 && m_pVportTabHip->ret_type()==DB_VIEWPORT) {
		gr_initview(((CIcadDoc*)m_pDocument)->m_dbDrawing,&SDS_CURCFG,&m_pCurDwgView,NULL);
	} else {
		struct gr_viewvars viewvarsp;
		SDS_ViewToVars2Use(this,((CIcadDoc*)m_pDocument)->m_dbDrawing,&viewvarsp);
		gr_initview(((CIcadDoc*)m_pDocument)->m_dbDrawing,&SDS_CURCFG,&m_pCurDwgView,&viewvarsp);
	}

	m_pCurDwgView->isprinting=1;
//4M Item:99->
// Prints upside down
if (pPrintDlg->m_advancedTab.m_Rotate && (!m_pMain->m_bPrintPreview)){
m_pCurDwgView->isprinting=2;
}
//<-4M

    //*** Setup the clipping rectangle.
	int x1,y1,x2,y2;
	if((pPrintDlg->m_scaleViewTab.m_View!=PRN_OPT_EXTENTS) && (pPrintDlg->m_scaleViewTab.m_ClipMode > 0))
	{
//4M Item:99->
// Prints upside down
/*
        gr_rp2pix(m_pCurDwgView,m_ptLwrLeft[0],m_ptUprRight[1],&x1,&y1);
		gr_rp2pix(m_pCurDwgView,m_ptUprRight[0],m_ptLwrLeft[1],&x2,&y2);
*/

  sds_point p0,p1;
  p0[0]=m_ptLwrLeft[0];
  p0[1]=m_ptUprRight[1];
  p1[0]=m_ptUprRight[0];
  p1[1]=m_ptLwrLeft[1];
  int res[4];
  gr_vect_rp2pix(p0,p1,res,m_pCurDwgView);
  if (m_pCurDwgView->isprinting==2){
     x1=res[0];
     y1=res[1];
     x2=res[2]-1;
     y2=res[3]-1;
  }
  else{
     x1=res[0];
     y1=res[1];
     x2=res[2]+1;
     y2=res[3]+1;
  }
//<-4M
    }
	else
	{
		int nOffsetX = (int)((pi.dHorzRes - pi.dAdjustedWidth)/2.0);
		int nOffsetY = (int)((pi.dVertRes - pi.dAdjustedHeight)/2.0);
//4M Item:99->
//Prints upside down
// Header Footer correction. Must be checked further
/*
	y1 = (nOffsetY + ((int)pi.dHeaderAdjust));
*/
	y1 = (nOffsetY);
//<-4M
        x1 = nOffsetX;
//4M Item:99->
//Prints upside down
// Header Footer correction. Must be checked further
/*
	y2 = (y1 + (int)(pi.dAdjustedHeight-pi.dFooterAdjust) +1);
*/
	y2 = (y1 + (int)(pi.dAdjustedHeight) +1);
//<-4M
        x2 = (x1 + (int)pi.dAdjustedWidth +1);
    }
	m_PrintClipRect.SetRect(x1,y1,x2+1,y2+1);

	/*DG - 24.3.2003*/// Regenerate display lists for printing.
	SDS_BuildRegenList(flp, m_pCurDwgView, this, (CIcadDoc*)m_pDocument);

out:

	if(rc<0)
	{
		CString strError;
		strError.Format(ResourceString(IDC_ICADVIEW_INTERNAL_ERROR_26, "Internal Error: File=%s Line=%d." ),__FILE__,-rc);
		AfxMessageBox(strError,MB_OK | MB_ICONSTOP);
	}
	return;
}

afx_msg void CIcadView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	//*** Set these variables because I was told to.
	m_LastCol=m_LastHl=m_LastRop=m_LastWid=(-2);
	m_FillLastCol=m_FillLastHl=m_FillLastRop=(-2);

	//*** Print the header and/or footer
	if(!m_pMain->m_pIcadPrintDlg->m_advancedTab.m_Header.IsEmpty())
	{
		PrintHeaderOrFooter(pDC,TRUE);
	}
	if(!m_pMain->m_pIcadPrintDlg->m_advancedTab.m_Footer.IsEmpty())
	{
		PrintHeaderOrFooter(pDC,FALSE);
	}
	//*** Draw the view.
	CPrintDialogSheet* pPrintDlg = m_pMain->m_pIcadPrintDlg;
	long ssct;
	sds_name ename;

	pDC->IntersectClipRect(m_PrintClipRect);

	//*** Prepare the device context for the calls to redraw
						// save currnet RGB flag
	bool	rgbColor = !!m_pMain->m_bMapToRGBColor;
	setPrinterDC( pDC );

	CheckFPControlWord();

	db_drawing *flp=NULL;
	if(m_pDocument)
		{
		flp=((CIcadDoc*)m_pDocument)->m_dbDrawing;
		}

	if(pPrintDlg->m_scaleViewTab.m_SelMode==PRN_OPT_SELENTS &&
		sds_sslength(pPrintDlg->m_scaleViewTab.m_ssPrintEnt,&ssct)==RTNORM && ssct>0L)
		{
		for(long ssct=0L; sds_ssname(pPrintDlg->m_scaleViewTab.m_ssPrintEnt,ssct,ename)==RTNORM; ssct++)
			{
			SDS_RedrawEntity(ename,IC_REDRAW_DRAW,this,(CIcadDoc*)m_pDocument,0);
			}
		}
	else
		{
		struct resbuf rb;
		int tilemode,cvport;
		SDS_getvar(NULL,DB_QCVPORT,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
		cvport=rb.resval.rint;
		SDS_getvar(NULL,DB_QTILEMODE,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
		tilemode=rb.resval.rint;
		if(tilemode==0 && cvport>1 && m_pVportTabHip->ret_type()==DB_VIEWPORT)
			{
			sds_name ename;
			ename[0]=(long)m_pVportTabHip;
			ename[1]=(long)flp;
			extern bool SDS_DrawVPGutsOnly;
			SDS_DrawVPGutsOnly=TRUE;
			if (this->m_iViewMode == kNormal)
				SDS_RedrawDrawing(flp,this,(CIcadDoc*)m_pDocument,0);
			else {
				CBitmapHLRenderer hlRenderer(this, pDC, pInfo);
				hlRenderer.render();
			}
			SDS_DrawVPGutsOnly=FALSE;
			}
		else
			{
			    if (this->m_iViewMode == kNormal)
					SDS_RedrawDrawing(flp,this,(CIcadDoc*)m_pDocument,0);
				else {
					CBitmapHLRenderer hlRenderer(this, pDC, pInfo);
					hlRenderer.render();
				}
			}
		}

	clearPrinterDC();

	CheckFPControlWord();
								// restore RGB setting
	m_pMain->m_bMapToRGBColor = rgbColor;
}

afx_msg void CIcadView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	//sds_name ename;
	int rc=0;
	struct resbuf rb;

	db_drawing *flp=NULL;
	if(m_pDocument)	flp=((CIcadDoc*)m_pDocument)->m_dbDrawing;

	m_pMain->m_bPrintPreview=FALSE;
	m_iWinX=(int)m_ptPrintScreenSize[0];
	m_iWinY=(int)m_ptPrintScreenSize[1];

	rb.restype=RTPOINT;
	rb.resval.rpoint[0]=m_iWinX;
	rb.resval.rpoint[1]=m_iWinY;
	rb.resval.rpoint[2]=0.0;
	SDS_setvar(NULL,DB_QSCREENSIZE,&rb,((CIcadDoc*)m_pDocument)->m_dbDrawing,&SDS_CURCFG,&SDS_CURSES,0);

	int tilemode,cvport;
	SDS_getvar(NULL,DB_QCVPORT,&rb,((CIcadDoc*)m_pDocument)->m_dbDrawing,&SDS_CURCFG,&SDS_CURSES);
	cvport=rb.resval.rint;
	SDS_getvar(NULL,DB_QTILEMODE,&rb,((CIcadDoc*)m_pDocument)->m_dbDrawing,&SDS_CURCFG,&SDS_CURSES);
	tilemode=rb.resval.rint;

	if(tilemode==0) {
		rb.restype=RTREAL;
		rb.resval.rreal=m_rPrintViewSize;
		SDS_setvar(NULL,DB_QVIEWSIZE,&rb,flp,&SDS_CURCFG,&SDS_CURSES,0);

		rb.restype=RT3DPOINT;
		ic_ptcpy(rb.resval.rpoint,m_ptPrintViewCtr);
		SDS_setvar(NULL,DB_QVIEWCTR,&rb,flp,&SDS_CURCFG,&SDS_CURSES,0);

		SDS_VarToTab(flp,m_pVportTabHip);
	} else {
		//*** Set the current view size back to the original view size.
		m_pVportTabHip->set_40(m_rPrintViewSize); // VIEWSIZE
		//*** Set the current view center back to the original view center.
	 	m_pVportTabHip->set_12(m_ptPrintViewCtr); // VIEWCTR
	}

	if(m_pMain->m_pIcadPrintDlg->m_scaleViewTab.m_View==PRN_OPT_SAVEDVIEW)
	{
		RestorePrintViewInfo();
	}

	if(tilemode==0 && cvport>1 && m_pVportTabHip->ret_type()==DB_VIEWPORT) {
		SDS_VPeedToView(((CIcadDoc*)m_pDocument)->m_dbDrawing,m_pVportTabHip,&m_pCurDwgView,this);
		SDS_SetClipInDC(this,m_pVportTabHip,FALSE);
	} else {
		struct gr_viewvars viewvarsp;
		SDS_ViewToVars2Use(this,((CIcadDoc*)m_pDocument)->m_dbDrawing,&viewvarsp);
		gr_initview(((CIcadDoc*)m_pDocument)->m_dbDrawing,&SDS_CURCFG,&m_pCurDwgView,&viewvarsp);
	}

//	for(long ssct=0L; sds_ssname(m_pMain->m_pIcadPrintDlg->m_scaleViewTab.m_ssPrintEnt,ssct,ename)==RTNORM; ssct++)
//	{
//		//*** Un-hilight the selected entities.
//		SDS_RedrawEntity(ename,IC_REDRAW_DRAW,this,(CIcadDoc*)m_pDocument,0);
//	}

	SetNewView(TRUE);

	if(rc<0)
	{
		CString strError;
		strError.Format(ResourceString(IDC_ICADVIEW_INTERNAL_ERROR_26, "Internal Error: File=%s Line=%d." ),__FILE__,-rc);
		AfxMessageBox(strError,MB_OK | MB_ICONSTOP);
	}

	//*** Set these variables because I was told to.
	m_LastCol=m_LastHl=m_LastRop=m_LastWid=(-2);
	m_FillLastCol=m_FillLastHl=m_FillLastRop=(-2);

	/*DG - 24.3.2003*/// This will cause regenerating display lists after printing.
	SDS_CURDOC->m_RegenListView = NULL;

	return;
}

void CIcadView::PrintHeaderOrFooter(CDC* pDC, BOOL bPrintHeader)
{
//*** The bPrintHeader varible should be set to TRUE to print the header and
//*** set to FALSE to print the footer.
	int ct;
	HGLOBAL hGlobal;
	_TCHAR *pStr,*ptr1;
	CString str1,str2,str3;
	str1.Empty(); str2.Empty(); str3.Empty();

	//*** Get the horizontal and vertical sizes of the printer.
	int nHorzSize = pDC->GetDeviceCaps(HORZRES);
	int nVertSize = pDC->GetDeviceCaps(VERTRES);
	//*** Create a temporary string to parse.
	if(bPrintHeader)
	{
		hGlobal=GlobalAlloc(GPTR,m_pMain->m_pIcadPrintDlg->m_advancedTab.m_Header.GetLength()+sizeof(_TCHAR));
		pStr=(_TCHAR*)GlobalLock(hGlobal);
		_tcscpy(pStr,m_pMain->m_pIcadPrintDlg->m_advancedTab.m_Header);
	}
	else
	{
		hGlobal=GlobalAlloc(GPTR,m_pMain->m_pIcadPrintDlg->m_advancedTab.m_Footer.GetLength()+sizeof(_TCHAR));
		pStr=(_TCHAR*)GlobalLock(hGlobal);
		_tcscpy(pStr,m_pMain->m_pIcadPrintDlg->m_advancedTab.m_Footer);
	}
	//*** Print the text.
	ptr1=_tcstok(pStr,_T(ResourceString(IDC_ICADVIEW___27, "," )));
	for(ct=0; ptr1!=NULL; ct++)
	{
		switch(ct)
		{
			case 0: //*** Left Justify
				if(*ptr1!=_T('\0')) str1=ptr1;
				break;
			case 1: //*** Center Justify
				if(*ptr1!=_T('\0')) str2=ptr1;
				break;
			case 2: //*** Right Justify
				if(*ptr1!=_T('\0')) str3=ptr1;
				break;
			default:
				ptr1=NULL;
				continue;
		}
		ptr1=_tcstok(NULL,_T(ResourceString(IDC_ICADVIEW___27, "," )));
	}
//4M Item:99->
// Prints upside down
HFONT hOldFont,hNewFont;
int HeaderY=0;
int FooterY=nVertSize;
int FirstJust=TA_LEFT;
int LastJust=TA_RIGHT;
int MidJust=TA_CENTER;
CPrintDialogSheet* pPrintDlg = m_pMain->m_pIcadPrintDlg;
BOOL PrintsUpsideDown=pPrintDlg->m_advancedTab.m_Rotate && (!m_pMain->m_bPrintPreview);
if (PrintsUpsideDown){
  HeaderY=nVertSize;
  FooterY=0;
  FirstJust=TA_RIGHT;
  LastJust=TA_LEFT;

  LOGFONT lf;
  memset(&lf, 0, sizeof(LOGFONT));
  lf.lfEscapement=1800;
  lf.lfOrientation=1800;
  hNewFont=::CreateFontIndirect(&lf);
  hOldFont=(HFONT)pDC->SelectObject(hNewFont);
}
//<-4M

	if(!str1.IsEmpty() && str2.IsEmpty() && str3.IsEmpty())
	{
		//*** Center the text.
		if(bPrintHeader)
		{
			//*** Print Header
//4M Item:99->
// Prints upside down
/*
		pDC->SetTextAlign(TA_CENTER | TA_TOP);
		pDC->TextOut((nHorzSize/2),0,str1);
*/
		pDC->SetTextAlign(MidJust | TA_TOP);
		pDC->TextOut((nHorzSize/2),HeaderY,str1);
//<-4M
        }
		else
		{
			//*** Print Footer
//4M Item:99->
// Prints upside down
/*
		pDC->SetTextAlign(TA_CENTER | TA_BOTTOM);
		pDC->TextOut((nHorzSize/2),nVertSize,str1);
*/
		pDC->SetTextAlign(MidJust | TA_BOTTOM);
		pDC->TextOut((nHorzSize/2),FooterY,str1);
//<-4M
        }
	}
	else if(!str1.IsEmpty() && !str2.IsEmpty() && str3.IsEmpty())
	{
		//*** If there is one comma, center the first string and right justify the
		//*** second string like Excel.
//4M Item:99->
// Prints upside down
       if (PrintsUpsideDown){
         CString strtemp=str1;
         str1=str2;
         str2=strtemp;
        }
//<-4M
        if(bPrintHeader)
		{
			//*** Print Header
//4M Item:99->
// Prints upside down
/*
		pDC->SetTextAlign(TA_CENTER | TA_TOP);
		pDC->TextOut((nHorzSize/2),0,str1);
		pDC->SetTextAlign(TA_RIGHT | TA_TOP);
		pDC->TextOut(nHorzSize,0,str2);
*/
		pDC->SetTextAlign(MidJust | TA_TOP);
		pDC->TextOut((nHorzSize/2),HeaderY,str1);
		pDC->SetTextAlign(LastJust | TA_TOP);
		pDC->TextOut(nHorzSize,HeaderY,str2);
//<-4M
        }
		else
		{
			//*** Print Footer
//4M Item:99->
// Prints upside down
/*
		pDC->SetTextAlign(TA_CENTER | TA_BOTTOM);
		pDC->TextOut((nHorzSize/2),nVertSize,str1);
		pDC->SetTextAlign(TA_RIGHT | TA_BOTTOM);
		pDC->TextOut(nHorzSize,nVertSize,str2);
*/
		pDC->SetTextAlign(MidJust | TA_BOTTOM);
		pDC->TextOut((nHorzSize/2),FooterY,str1);
		pDC->SetTextAlign(LastJust | TA_BOTTOM);
		pDC->TextOut(nHorzSize,FooterY,str2);
//<-4M
        }
	}
	else
	{
//4M Item:99->
// Prints upside down
       if (PrintsUpsideDown){
         CString strtemp=str1;
         str1=str3;
         str3=strtemp;
         }
//<-4M
        if(bPrintHeader)
		{
			//*** Print Header
//4M Item:99->
// Prints upside down
/*
		pDC->SetTextAlign(TA_LEFT | TA_TOP);
		pDC->TextOut(0,0,str1);
		pDC->SetTextAlign(TA_CENTER | TA_TOP);
		pDC->TextOut((nHorzSize/2),0,str2);
		pDC->SetTextAlign(TA_RIGHT | TA_TOP);
		pDC->TextOut(nHorzSize,0,str3);
*/
		pDC->SetTextAlign(FirstJust | TA_TOP);
		pDC->TextOut(0,HeaderY,str1);
		pDC->SetTextAlign(MidJust | TA_TOP);
		pDC->TextOut((nHorzSize/2),HeaderY,str2);
		pDC->SetTextAlign(LastJust | TA_TOP);
		pDC->TextOut(nHorzSize,HeaderY,str3);
//<-4M
        }
		else
		{
			//*** Print Footer
//4M Item:99->
// Prints upside down
/*
		pDC->SetTextAlign(TA_LEFT | TA_BOTTOM);
		pDC->TextOut(0,nVertSize,str1);
		pDC->SetTextAlign(TA_CENTER | TA_BOTTOM);
		pDC->TextOut((nHorzSize/2),nVertSize,str2);
		pDC->SetTextAlign(TA_RIGHT | TA_BOTTOM);
		pDC->TextOut(nHorzSize,nVertSize,str3);
*/
		pDC->SetTextAlign(FirstJust | TA_BOTTOM);
		pDC->TextOut(0,FooterY,str1);
		pDC->SetTextAlign(MidJust | TA_BOTTOM);
		pDC->TextOut((nHorzSize/2),FooterY,str2);
		pDC->SetTextAlign(LastJust | TA_BOTTOM);
		pDC->TextOut(nHorzSize,FooterY,str3);
//<-4M
        }
	}
//4M Item:99->
// Prints upside down
if (PrintsUpsideDown){
  pDC->SelectObject(hOldFont);
  ::DeleteObject(hNewFont);
}
//<-4M
    GlobalFree(hGlobal);
}

void CIcadView::SavePrintViewInfo(struct sds_resbuf* pRb)
{
	sds_point ptAltViewDir,ptAltTarget,ptAltViewCtr;
	sds_real rAltLensLength,rAltFrontZ,rAltBackZ,rAltViewTwist;
	int rc=0,nAltViewMode;
	struct sds_resbuf rb;

	db_drawing *flp=NULL;
	if(m_pDocument)	flp=((CIcadDoc*)m_pDocument)->m_dbDrawing;

	//*** Previous view info needs to be restored before setting it again.
	RestorePrintViewInfo();
	//*** Set the alternate view mode.
	if(ic_assoc(pRb,71)==0) nAltViewMode=ic_rbassoc->resval.rint;
	//*** Cache the current view mode.
	if(SDS_getvar(NULL,DB_QVIEWMODE,&rb,flp,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
	{
		rc=(-__LINE__);
		goto out;
	}
	m_nPrintViewMode=rb.resval.rint;
	//*** Set the current view mode to the alternate view mode.
	rb.resval.rint=nAltViewMode;
	if(SDS_setvar(NULL,DB_QVIEWMODE,&rb,flp,&SDS_CURCFG,&SDS_CURSES,0)!=RTNORM)
	{
		rc=(-__LINE__);
		goto out;
	}

	//*** Set the alternate view direction.
	if(ic_assoc(pRb,11)==0) ic_ptcpy(ptAltViewDir,ic_rbassoc->resval.rpoint);
	//*** Cache the current view direction.
	if(SDS_getvar(NULL,DB_QVIEWDIR,&rb,flp,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
	{
		rc=(-__LINE__);
		goto out;
	}
	ic_ptcpy(m_ptPrintViewDir,rb.resval.rpoint);
	//*** Set the current view direction to the alternate view direction.
	ic_ptcpy(rb.resval.rpoint,ptAltViewDir);
	if(SDS_setvar(NULL,DB_QVIEWDIR,&rb,flp,&SDS_CURCFG,&SDS_CURSES,1)!=RTNORM)
	{
		rc=(-__LINE__);
		goto out;
	}

	gr_initview(flp,&SDS_CURCFG,&m_pCurDwgView,NULL);

	//*** Set the alternate target point.
	if(ic_assoc(pRb,12)==0) ic_ptcpy(ptAltTarget,ic_rbassoc->resval.rpoint);
	//*** Cache the current target point.
	if(SDS_getvar(NULL,DB_QTARGET,&rb,flp,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
	{
		rc=(-__LINE__);
		goto out;
	}
	ic_ptcpy(m_ptPrintTarget,rb.resval.rpoint);
	//*** Set the current target point to the alternate target point.
	ic_ptcpy(rb.resval.rpoint,ptAltTarget);
	if(SDS_setvar(NULL,DB_QTARGET,&rb,flp,&SDS_CURCFG,&SDS_CURSES,1)!=RTNORM)
	{
		rc=(-__LINE__);
		goto out;
	}

	//*** Set the alternate target point.
	if(ic_assoc(pRb,10)==0) ic_ptcpy(ptAltViewCtr,ic_rbassoc->resval.rpoint);

	gr_rp2ucs(ptAltViewCtr,ptAltViewCtr,m_pCurDwgView);

	//*** Set the alternate lens length.
	if(ic_assoc(pRb,42)==0) rAltLensLength=ic_rbassoc->resval.rreal;
	//*** Cache the current lens length.
	if(SDS_getvar(NULL,DB_QLENSLENGTH,&rb,flp,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
	{
		rc=(-__LINE__);
		goto out;
	}
	m_rPrintLensLength=rb.resval.rreal;
	//*** Set the current lens length to the alternate lens length.
	rb.resval.rreal=rAltLensLength;
	if(SDS_setvar(NULL,DB_QLENSLENGTH,&rb,flp,&SDS_CURCFG,&SDS_CURSES,0)!=RTNORM)
	{
		rc=(-__LINE__);
		goto out;
	}

	//*** Set the alternate Front clipping plane.
	if(ic_assoc(pRb,43)==0) rAltFrontZ=ic_rbassoc->resval.rreal;
	//*** Cache the current Front clipping plane.
	if(SDS_getvar(NULL,DB_QFRONTZ,&rb,flp,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
	{
		rc=(-__LINE__);
		goto out;
	}
	m_rPrintFrontZ=rb.resval.rreal;
	//*** Set the current Front clipping plane to the alternate Front
	//*** clipping plane.
	rb.resval.rreal=rAltFrontZ;
	if(SDS_setvar(NULL,DB_QFRONTZ,&rb,flp,&SDS_CURCFG,&SDS_CURSES,0)!=RTNORM)
	{
		rc=(-__LINE__);
		goto out;
	}

	//*** Set the alternate Back clipping plane.
	if(ic_assoc(pRb,44)==0) rAltBackZ=ic_rbassoc->resval.rreal;
	//*** Cache the current Back clipping plane.
	if(SDS_getvar(NULL,DB_QBACKZ,&rb,flp,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
	{
		rc=(-__LINE__);
		goto out;
	}
	m_rPrintBackZ=rb.resval.rreal;
	//*** Set the current Back clipping plane to the alternate Back
	//*** clipping plane.
	rb.resval.rreal=rAltBackZ;
	if(SDS_setvar(NULL,DB_QBACKZ,&rb,flp,&SDS_CURCFG,&SDS_CURSES,0)!=RTNORM)
	{
		rc=(-__LINE__);
		goto out;
	}

	//*** Set the alternate Twist angle.
	if(ic_assoc(pRb,50)==0) rAltViewTwist=ic_rbassoc->resval.rreal;
	//*** Cache the current Twist angle.
	if(SDS_getvar(NULL,DB_QVIEWTWIST,&rb,flp,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
	{
		rc=(-__LINE__);
		goto out;
	}
	m_rPrintViewTwist=rb.resval.rreal;
	//*** Set the current Twist angle to the alternate Twist angle.
	rb.resval.rreal=rAltViewTwist;
	if(SDS_setvar(NULL,DB_QVIEWTWIST,&rb,flp,&SDS_CURCFG,&SDS_CURSES,0)!=RTNORM)
	{
		rc=(-__LINE__);
		goto out;
	}

	SDS_VarToTab(flp,m_pVportTabHip);
	gr_initview(flp,&SDS_CURCFG,&m_pCurDwgView,NULL);
	m_bNeedRestoreView=TRUE;

out:

	if(rc<0)
	{
		CString strError;
		strError.Format(ResourceString(IDC_ICADVIEW_INTERNAL_ERROR_26, "Internal Error: File=%s Line=%d." ),__FILE__,-rc);
		AfxMessageBox(strError,MB_OK | MB_ICONSTOP);
	}
	return;
}

void CIcadView::RestorePrintViewInfo()
{
	struct resbuf rb;
	int rc=0;

	db_drawing *flp=NULL;
	if(m_pDocument)	flp=((CIcadDoc*)m_pDocument)->m_dbDrawing;

	if(!m_bNeedRestoreView)
		return;
	//*** Set the current view direction back to the original view direction.
	rb.restype=RT3DPOINT;
	ic_ptcpy(rb.resval.rpoint,m_ptPrintViewDir);
	if(SDS_setvar(NULL,DB_QVIEWDIR,&rb,flp,&SDS_CURCFG,&SDS_CURSES,0)!=RTNORM)
	{
		rc=(-__LINE__);
		goto out;
	}

	gr_initview(flp,&SDS_CURCFG,&m_pCurDwgView,NULL);

	//*** Set the current target point back to the original target direction.
	rb.restype=RT3DPOINT;
	ic_ptcpy(rb.resval.rpoint,m_ptPrintTarget);
	if(SDS_setvar(NULL,DB_QTARGET,&rb,flp,&SDS_CURCFG,&SDS_CURSES,0)!=RTNORM)
	{
		rc=(-__LINE__);
		goto out;
	}
	//*** Set the current lens length back to the original lens length.
	rb.restype=RTREAL;
	rb.resval.rreal=m_rPrintLensLength;
	if(SDS_setvar(NULL,DB_QLENSLENGTH,&rb,flp,&SDS_CURCFG,&SDS_CURSES,0)!=RTNORM)
	{
		rc=(-__LINE__);
		goto out;
	}
	//*** Set the current Front clipping plane back to the original Front clipping plane.
	rb.restype=RTREAL;
	rb.resval.rreal=m_rPrintFrontZ;
	if(SDS_setvar(NULL,DB_QFRONTZ,&rb,flp,&SDS_CURCFG,&SDS_CURSES,0)!=RTNORM)
	{
		rc=(-__LINE__);
		goto out;
	}
	//*** Set the current Back clipping plane back to the original Back clipping plane.
	rb.restype=RTREAL;
	rb.resval.rreal=m_rPrintBackZ;
	if(SDS_setvar(NULL,DB_QBACKZ,&rb,flp,&SDS_CURCFG,&SDS_CURSES,0)!=RTNORM)
	{
		rc=(-__LINE__);
		goto out;
	}
	//*** Set the current twist angle back to the original twist angle.
	rb.restype=RTREAL;
	rb.resval.rreal=m_rPrintViewTwist;
	if(SDS_setvar(NULL,DB_QVIEWTWIST,&rb,flp,&SDS_CURCFG,&SDS_CURSES,0)!=RTNORM)
	{
		rc=(-__LINE__);
		goto out;
	}
	//*** Set the current twist angle back to the original twist angle.
	rb.restype=RTSHORT;
	rb.resval.rint=m_nPrintViewMode;
	if(SDS_setvar(NULL,DB_QVIEWMODE,&rb,flp,&SDS_CURCFG,&SDS_CURSES,0)!=RTNORM)
	{
		rc=(-__LINE__);
		goto out;
	}

	SDS_VarToTab(flp,m_pVportTabHip);
	m_bNeedRestoreView=FALSE;

out:

	if(rc<0)
	{
		CString strError;
		strError.Format(ResourceString(IDC_ICADVIEW_INTERNAL_ERROR_26, "Internal Error: File=%s Line=%d." ),__FILE__,-rc);
		AfxMessageBox(strError,MB_OK | MB_ICONSTOP);
	}
	return;
}

CIcadDoc* CIcadView::GetDocument()
{
	return (CIcadDoc*)m_pDocument;
}

db_drawing *
CIcadView::GetDrawing()
	{
	db_drawing *pRetval = NULL;
	if ( GetDocument() != NULL )
		{
		pRetval = GetDocument()->m_dbDrawing;
		}

	return pRetval;
	}


void
CIcadView::CursorDown( CPoint point)
	{
	m_CursorDownPos = point;
	}


UINT
CIcadView::CursorID( UINT id)
	{
	UINT	previous = m_idcCursor;

	m_idcCursor = id;
	return previous;
	}


bool
CIcadView::CursorOn( bool state)
	{
	bool	previous = m_CursorOn;

	m_CursorOn	= state;

	// if changing cursor state, XOR the cursor
	if ( previous != m_CursorOn )
		IcadCursor::AppCursor().DrawCursor( m_LastCursorPos, m_LastCursorType, this);

	return previous;
	}

void CIcadView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
	{
	if( m_pMain->m_pCurView EQ this )
		{
		struct resbuf	rb;

		if ( bActivate )
			{
			rb.resval.rint = m_showScrollBars;
			rb.restype=RTSHORT;
			sds_setvar( "WNDLSCRL", &rb);
			}
		else
			{
									// save scrollbar setting
			SDS_getvar(NULL,DB_QWNDLSCRL,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			m_showScrollBars = (rb.resval.rint EQ 1);

//4M Bugzilla 77987 19/02/02->
/*
            if(m_pRealTime)
				{
				KillTimer(RealTimeTimer_id);
				delete m_pRealTime;
				m_pRealTime = NULL;
				}

			m_RTMotion = NO;

*/
//We leave behaviour for ZOOM_WHEEL intact
        if (GetRTMotion()==ZOOM_WHEEL){
           DeleteRealTime();
           SetRTMotion(NO);
        }
//<-4M 19/02/02
            sds_point DummyPt;
			SDS_EndDraggingSS(DummyPt, true);
			// EBATECH(shiba)-[
			// FIX:Not display the garbage (afterimage of a figure)
			//	   during drawing of an entity (command interruption),
			//	   when a focus is moved to view of another document.
			if(SDS_IsDragging && m_pMain->m_pDragVarsCur && m_pMain->m_pDragVarsCur->firstdrag)
				{
				if(m_pMain->m_pDragVarsCur->scnf)
					(*m_pMain->m_pDragVarsCur->scnf)(m_pMain->m_pDragVarsCur->pt1,m_pMain->m_pDragVarsCur->matx);
				}
			// ]-EBATECH(shiba)

			}

		// If we're already active, no need to go farther
		//
		return;
		}

	if ( bActivate )
		{
		if( m_pMain->m_pCurView &&
			m_pMain->m_pCurView!=this &&
			IsWindow(m_pMain->m_pCurView->GetSafeHwnd()))
			{
			if(SDSApplication::GetActiveApplication()->GetInputControlBits() & SDS_RSG_NOVIEWCHG)
				{
				if ( bActivate )
					{
					((CIcadChildWnd*)m_pMain->m_pCurView->GetParentFrame())->ActivateFrame();
					return;
					}
				}
			if( (SDSApplication::GetActiveApplication()->GetInputControlBits() & SDS_RSG_NODOCCHG) &&
				m_pMain->m_pCurDoc!=GetDocument())
				{
				if ( bActivate )
					{
					((CIcadChildWnd*)m_pMain->m_pCurView->GetParentFrame())->ActivateFrame();
					return;
					}
				}
			}
		m_pMain->SetPalette(m_pMain->m_pPalette);
		SetNewView(FALSE);
		}

	if (g_Vba6Installed)
		{
		if (bActivate)
			{
			CIcadDoc* pDoc = GetDocument();
			pDoc->ApcProjectItem->Activate();
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////
// OLE Client support and commands

BOOL CIcadView::IsSelected(const CObject* pDocItem) const
{
	// The implementation below is adequate if your selection consists of
	//	only CIcadCntrItem objects.  To handle different selection
	//	mechanisms, the implementation here should be replaced.

	// TODO: implement this function that tests for a selected OLE client item

	return pDocItem == m_pSelection;
}

void CIcadView::OnInsertObject(LPCTSTR pszFileName)
{
	if(NULL==pszFileName || *pszFileName==0)
	{
		OnInsertObject();
		return;
	}
	//*** Insert as an embedded OLE object.
	CIcadDoc* pDoc = GetDocument();
	if(NULL==pDoc)
	{
		return;
	}
	// We have to do a special set undo here because this
	// WndAction is done witt a PostThreadMessage().  OUCH!
	SDS_SetUndo(IC_UNDO_COMMAND_BEGIN,(void *)"INSERTOBJ"/*DNT*/,NULL,NULL,SDS_CURDWG);
	//*** Create new item connected to this document.
	CIcadCntrItem* pItem = new CIcadCntrItem(pDoc,NULL);
	if(NULL!=pItem)
	{
		if(!pItem->CreateFromFile(pszFileName))
		{
			delete pItem;
		}
		else
		{
			pItem->UpdateHandItem(this);
			pItem->UpdateFromServerExtent(this);
			SetSelection(pItem);
			pItem->Draw(this,GetFrameBufferCDC());
			SDS_SetUndo(IC_UNDO_COMMAND_END,(void *)"INSERTOBJ"/*DNT*/,NULL,NULL,SDS_CURDWG);
			return;
		}
	}
	SDS_SetUndo(IC_UNDO_COMMAND_END,(void *)"INSERTOBJ"/*DNT*/,NULL,NULL,SDS_CURDWG);
}

void CIcadView::OnInsertObject()
{
	//*** The Insert Object dialog gets called in CreateOleItem()
	CIcadDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->CreateOleItem(this,NULL);
}

// The following command handler provides the standard keyboard
//	user interface to cancel an in-place editing session.  Here,
//	the container (not the server) causes the deactivation.
void CIcadView::OnCancelEditCntr()
{
	// Close any in-place active item on this view.
	COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
	if (pActiveItem != NULL)
	{
		pActiveItem->Close();
	}
	ASSERT(GetDocument()->GetInPlaceActiveItem(this) == NULL);
}

// Special handling of OnSetFocus and OnSize are required for a container
//	when an object is being edited in-place.
void CIcadView::OnSetFocus(CWnd* pOldWnd)
{


	if(m_pMain->m_pCurView!=this) {

		if(SDS_IsDragging && m_pMain->m_pDragVarsCur && m_pMain->m_pDragVarsCur->firstdrag) {
			if(m_pMain->m_pDragVarsCur->scnf) {

				// EBATECH(shiba)-[
				// FIX:Not display the garbage (afterimage of a figure)
				//	   during drawing of an entity (command interruption),
				//	   when a focus is moved to view of another document.
				if (m_pMain->m_pCurView->m_pViewsDoc != m_pViewsDoc)
					m_pMain->m_pDragVarsCur->firstdrag=0;
				// ]-EBATECH(shiba)

				int ret;
				if(RTERROR==(ret=(*m_pMain->m_pDragVarsCur->scnf)(m_pMain->m_pDragVarsCur->pt1,m_pMain->m_pDragVarsCur->matx)))
				m_pMain->m_pDragVarsCur->breakout=ret;
			}
			if(SDS_AtCopyMoveCommand) sds_grdraw(m_pMain->m_pDragVarsCur->pt1,m_pMain->m_pDragVarsCur->pt3,-1,0);
			SDS_DrawDragVectors(NULL);
			m_pMain->m_pDragVarsCur->firstdrag=0;
		}

		if(SDSApplication::GetActiveApplication()->GetInputControlBits() & SDS_RSG_NOVIEWCHG)
			{
			return;
			}
		if( (SDSApplication::GetActiveApplication()->GetInputControlBits() & SDS_RSG_NODOCCHG) &&
			m_pMain->m_pCurDoc!=GetDocument())
			{
			return;
			}
		m_bEatNextMouse=TRUE;
	}

	COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
	if (pActiveItem != NULL &&
		pActiveItem->GetItemState() == COleClientItem::activeUIState)
	{
		// need to set focus to this item if it is in the same view
		CWnd* pWnd = pActiveItem->GetInPlaceWindow();
		if (pWnd != NULL)
		{
			pWnd->SetFocus();	// don't call the base class
		}
	}

	if(m_pMain->m_pCurView!=this || m_pViewsDoc==NULL) SetNewView(FALSE);

	CView::OnSetFocus(pOldWnd);
}

DROPEFFECT CIcadView::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	DROPEFFECT dropRet = DROPEFFECT_NONE;
	if(MK_LBUTTON==dwKeyState || ((MK_LBUTTON | MK_ALT)==dwKeyState))
	{
		dropRet=DROPEFFECT_MOVE;
	}
	else if((MK_LBUTTON | MK_CONTROL)==dwKeyState)
	{
		dropRet=DROPEFFECT_COPY;
	}
	if((MK_LBUTTON | MK_CONTROL | MK_SHIFT)==dwKeyState)
	{
		dropRet=DROPEFFECT_LINK;
	}
	return dropRet;
}

DROPEFFECT CIcadView::OnDropEx(COleDataObject* pDataObject, DROPEFFECT dropDefault,
	DROPEFFECT dropList, CPoint point)
{
	if(NULL==pDataObject)
	{
		return DROPEFFECT_NONE;
	}

	// Idrop Handler 
 	CDropActions dropAct;
 	if (DROPEFFECT_COPY == dropAct.DropIDrop(pDataObject))
	{
 	   SetForegroundWindow();
 	   dropAct.Execute();
 	   return DROPEFFECT_NONE;
 	}
	// Idrop Handler 

	if(pDataObject->IsDataAvailable(CF_HDROP))
	{
		//*** If files were dropped, call the default and CMainWindow will deal.
		return CView::OnDropEx(pDataObject,dropDefault,dropList,point);
	}
	CIcadDoc* pDoc=(CIcadDoc*)GetDocument();
	if(NULL==pDoc)
	{
		return DROPEFFECT_NONE;
	}
	//*** Create new item connected to this document.
	CIcadCntrItem* pItem = new CIcadCntrItem(pDoc,NULL);
	if(NULL==pItem)
	{
		return DROPEFFECT_NONE;
	}
	if(!pItem->CreateFromData(pDataObject))
	{
		delete pItem;
		return DROPEFFECT_NONE;
	}
	//*** Initialize the item.
	pItem->UpdateHandItem(this);
	pItem->UpdateFromServerExtent(this);
	//*** Move the item to the point is was dropped at.
	CRect rect;
	pItem->GetCurRect(this,&rect);
	rect.OffsetRect((point.x-rect.left),(point.y-rect.top));
	pItem->UpdateBounds(this,rect);
	//*** Draw the item
	SetSelection(pItem);
	pItem->Draw(this,GetFrameBufferCDC());
	return dropDefault;
}

void CIcadView::EsnapContextMenu(CPoint point)
{

	SDS_CMainWindow->m_fIsShiftDown=FALSE;

	long lflgVisibility=ICAD_MN_RCO;
	struct sds_resbuf rb;
	SDS_getvar(NULL,DB_QEXPLEVEL,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	switch(rb.resval.rint)
	{
		case 1:
			lflgVisibility|=ICAD_MN_BEG;
			break;
		case 2:
			lflgVisibility|=ICAD_MN_INT;
			break;
		case 3:
			lflgVisibility|=ICAD_MN_EXP;
			break;
	}

	// Modified Cybage MM 26/11/2001 (DD/MM/YYYY)
	// Reason: Fix for bug no 77908 from BugZilla
	//if( !SDS_AtEntGet && !SDS_AtCmdLine )
	//	lflgVisibility|=ICAD_MN_TOM;

	CIcadMenu* pIcadMenu = NULL;
	if(NULL!=m_pMain && NULL!=m_pMain->m_pMenuMain)
	{
		pIcadMenu=m_pMain->m_pMenuMain->GetCurrentMenu();
	}

	if(NULL==pIcadMenu)
	{
		return;
	}

	// Modified Cybage MM 26/11/2001 (DD/MM/YYYY)[
	// Reason: Fix for bug no 77908 from BugZilla
	if((pIcadMenu->m_pMenuArray!=NULL && !pIcadMenu->m_pMenuArray->fileName.IsEmpty()) ||
	   (!SDS_AtEntGet && !SDS_AtCmdLine))
		lflgVisibility|=ICAD_MN_TOM;
	// Modified Cybage MM 26/11/2001 (DD/MM/YYYY)]


	int m_idxEsnapMenu = pIcadMenu->m_idxEsnapMenu;
	if(m_idxEsnapMenu==(-1))
	{
		return;
	}
	CMenu* pPopup = new CMenu();
	HMENU hSubMenu = pIcadMenu->SubMenuCreator(&m_idxEsnapMenu,lflgVisibility,0);
	if(hSubMenu!=NULL)
	{
		pPopup->Attach(hSubMenu);

		// AG: update check state for osnap mode menu(if it's not a temporary osnap settings)
		if( !(lflgVisibility & ICAD_MN_TOM) )
			pIcadMenu->UpdateCheckState( "OSMODE", pPopup );

		ClientToScreen(&point);
		pPopup->TrackPopupMenu(TPM_RIGHTBUTTON,point.x,point.y,m_pMain);
	}
	delete pPopup;
	return;
}

void CIcadView::OleContextMenu(CPoint point)
{
	if(NULL==m_pSelection)
	{
		return;
	}
	//*** Create the popup menu
	CMenu* pMenu = new CMenu();
	pMenu->CreatePopupMenu();

	pMenu->AppendMenu(MF_STRING,ICAD_OLEITEMMNU_START,ResourceString(IDC_ICADVIEW_CUT_29, "Cut" ));
	pMenu->AppendMenu(MF_STRING,ICAD_OLEITEMMNU_START+1,ResourceString(IDC_ICADVIEW_COPY_30, "Copy" ));
	pMenu->AppendMenu(MF_SEPARATOR);
	pMenu->AppendMenu(MF_STRING,ICAD_OLEITEMMNU_START+2,ResourceString(IDC_ICADVIEW_DELETE_OBJECT_31, "Delete Object" ));
	pMenu->AppendMenu(MF_SEPARATOR);
	//*** List the verbs available for the object.
	pMenu->AppendMenu(MF_STRING,ICAD_OLEITEMMNU_START+3,ResourceString(IDC_ICADVIEW_OLE_VERBS_HERE_32, "OLE Verbs here" ));
	HMENU hVerbMenu=NULL;
	::OleUIAddVerbMenu(m_pSelection->m_lpObject,NULL,pMenu->GetSafeHmenu(),
		5,ICAD_OLEITEMMNU_VERBSTART,ICAD_OLEITEMMNU_END,FALSE,0,
		&hVerbMenu);

	//*** Convert the point to screen coords and change the cursor.
	ClientToScreen(&point);
	m_prevCursor=(::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW)));

	//*** Display the menu
	pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON,
		point.x,point.y,
		this);

	//*** Delete the menu
	delete pMenu;
	return;
}

void CIcadView::OnOleContextMenuPick(UINT nId)
{
	int idMenu = (nId-ICAD_OLEITEMMNU_START);
	switch(idMenu)
	{
		case 0:		//*** Cut
			SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)ResourceString(IDC_ICADVIEW__C_C_CCUTCLIP_33, "^C^C^C_CUTCLIP" ));
			break;
		case 1:		//*** Copy
			SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)ResourceString(IDC_ICADVIEW__C_C_CCOPYCLIP_34, "^C^C^C_COPYCLIP" ));
			break;
		case 2:		//*** Delete
			SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)ResourceString(IDC_ICADVIEW__C_C_CDELETE_35, "^C^C^C_DELETE" ));
			break;
		case 3:		//*** Default OLE verb
			if(NULL!=m_pSelection)
			{
				m_pSelection->DoVerb(0L,this);
			}
			break;
		default:	//*** OLE verbs
			if(NULL!=m_pSelection)
			{
				//*** Execute the verb
				int idVerb = (nId-ICAD_OLEITEMMNU_VERBSTART);
				m_pSelection->DoVerb(idVerb,this);
			}
			break;
	}
}

/////////////////////////////////////////////////////////////////////////////
// OLE Server support

// The following command handler provides the standard keyboard
//	user interface to cancel an in-place editing session.  Here,
//	the server (not the container) causes the deactivation.
void CIcadView::OnCancelEditSrvr()
{
	GetDocument()->OnDeactivateUI(FALSE);
}

BOOL CIcadView::OnQueryNewPalette()
{
	// CG: This function was added by the Palette Support component

	if (SDS_CMainWindow->m_pPalette == NULL)
		return FALSE;

	CClientDC dc(this);
	CPalette* pOldPalette1 = dc.SelectPalette(SDS_CMainWindow->m_pPalette,
		GetCurrentMessage()->message == WM_PALETTECHANGED);
	UINT nChanged1 = dc.RealizePalette();

	GetFrameBufferDrawDevice()->selectPalette(*SDS_CMainWindow->m_pPalette,
		GetCurrentMessage()->message == WM_PALETTECHANGED);
	UINT nChanged2 = GetFrameBufferDrawDevice()->realizePalette();

	Invalidate();

	return TRUE;
}

void CIcadView::OnPaletteChanged(CWnd* pFocusWnd)
{
	// CG: This function was added by the Palette Support component

	if (pFocusWnd == this || IsChild(pFocusWnd))
		return;

	OnQueryNewPalette();
}

// ***********************************************
// PRINTER FUNCTIONS
//
// When printing, the code stores a Printer DC
// in the CIcadView class and these methods access it
//
// private method
void
CIcadView::setPrinterDC( CDC *pDC )
	{
	m_pCurrentPrinterDC = pDC;
								// set RGB flag according to device capabilities
	m_pMain->m_bMapToRGBColor = ( pDC->GetDeviceCaps( BITSPIXEL ) > 8 );
								// if not using RGB create appropriate palette
	if ( !m_pMain->m_bMapToRGBColor )
		{
		CPalette	*palette = SDS_GetNewPalette();
		if ( palette )
			{
			m_oldPrinterPalette = pDC->SelectPalette( palette, false);
			pDC->RealizePalette();
			}
		}
	}

// private method
void
CIcadView::clearPrinterDC( void )
	{
	if ( m_oldPrinterPalette )
		delete m_pCurrentPrinterDC->SelectPalette( m_oldPrinterPalette, false);

	m_pCurrentPrinterDC = NULL;
	m_oldPrinterPalette = NULL;
	}

// private method
CDC		*
CIcadView::getPrinterDC( void )
	{
	return m_pCurrentPrinterDC;
	}


// *************************
// PUBLIC METHOD
//
bool
CIcadView::IsPrinting( void )
	{
	bool bRetval = false;
	if ( getPrinterDC() != NULL )
		{
		ASSERT( getPrinterDC()->IsPrinting() );
		bRetval = true;
		}
	return bRetval;

	}

CDrawDevice* CIcadView::GetDrawDevice()
{
	CDrawDevice* pRetval = NULL;
	
	// printing has its own device
	if(getPrinterDC())
	{
		if(!m_printerDrawDevice)
			m_printerDrawDevice = new CDDPrinter(m_pMain->m_pIcadPrintDlg->GetPlotProperties());

		m_printerDrawDevice->create(getPrinterDC(), CDrawDevice::eSINGLE_BUFFER);
		pRetval = m_printerDrawDevice;
		pRetval->beginDraw(getPrinterDC(), m_pCurDwgView);
		//pRetval->beginDraw(getPrinterDC(), NULL);
	}
	else
	{
		pRetval = m_pFrameBufferDrawDevice;
	}
	
	CheckFPControlWord();
	return pRetval;
}

bool CIcadView::ReleaseDrawDevice(CDrawDevice* pDevice)
{
	// need to hang on to printer device for redrawing print preview
	if(pDevice == m_printerDrawDevice )
	{
		pDevice->endDraw();
		delete m_printerDrawDevice;
		m_printerDrawDevice = NULL;
	}
	
	return true;
}

void
CIcadView::OnTimer(UINT nIDEvent)
{
	POINT DummyPt = {0, 0};		// dummy point

//4M Bugzilla 77987 19/02/02->
/*
    if(nIDEvent == RealTimeTimer_id && m_pRealTime && m_pRealTime->m_bContinuousOrbitStarted)
		m_pRealTime->Continue(DummyPt);
*/
	if(nIDEvent == RealTimeTimer_id && GetRealTime() && GetRealTime()->m_bContinuousOrbitStarted)
		GetRealTime()->Continue(DummyPt);
//<-4M 19/02/02
    else
		if(nIDEvent == RealTimeMouseWheelTimerID)
		{
//4M Bugzilla 77987 19/02/02->
/*
            KillTimer(RealTimeMouseWheelTimerID);
			delete m_pRealTime;
			m_pRealTime = NULL;
			m_RTMotion = NO;
*/
        DeleteRealTime(RealTimeMouseWheelTimerID);
        SetRTMotion(NO);
//<-4M 19/02/02
        }

	CView::OnTimer(nIDEvent);
}

//*** ToolTip functions
int CIcadView::OnToolHitTest( CPoint pt, TOOLINFO* pTI ) const
{
	int nHit = -1;
	if(pTI == NULL)
		return nHit;

	COsnapManagerIcad *pOsnapManager = ((CEngineApplication*)SDSApplication::GetEngineApplication())->getOsnapManager();
	if(pOsnapManager->isData())
	{
		bool bEnableFlyOver = UserPreference::SnapFlyOverEnablePreference;
		bool bDisplayTooltips = UserPreference::SnapTooltipsPreference;

		if(bEnableFlyOver && bDisplayTooltips)
		{
			nHit = MAKELONG( pt.x, pt.y );
			pTI->hwnd = m_hWnd;
			pTI->uId = nHit;
			pTI->rect = CRect(CPoint(pt.x-1,pt.y-1),CSize(2,2));
			pTI->lpszText = LPSTR_TEXTCALLBACK;
			pTI->uFlags |= TTF_NOTBUTTON;
		}
	}
	else
	{
		if(!(bool)UserPreference::s_bEnableHyperlinkTooltip)
			return nHit;

		resbuf pb;
		SDS_getvar(NULL,DB_QPICKBOX, &pb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);

		sds_point pt1rp, pt2rp, pt1ucs, pt2ucs;
		gr_pix2rp(m_pCurDwgView, pt.x, pt.y, &pt1rp[0], &pt1rp[1]);
		gr_pix2rp(m_pCurDwgView, pt.x + pb.resval.rint, pt.y + pb.resval.rint, &pt2rp[0], &pt2rp[1]);

		gr_rp2ucs(pt1rp, pt1ucs, m_pCurDwgView);
		gr_rp2ucs(pt2rp, pt2ucs, m_pCurDwgView);

		sds_name ss;
		sds_name entity;
		long l;

		// PICKSTYLE should be cleared before because of groups
		int pickstyle;
		struct resbuf ps;
		SDS_getvar(NULL, DB_QPICKSTYLE, &ps, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
		pickstyle = ps.resval.rint;
		ps.resval.rint = 0;
		SDS_setvar(NULL, DB_QPICKSTYLE, &ps, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES, 0);

		SDS_PickObjects("CROSSING", pt1ucs, pt2ucs, NULL, ss, 1, SDS_CURDWG, false, false, false, false);

		// restore PICKSTYLE
		ps.resval.rint = pickstyle;
		SDS_setvar(NULL, DB_QPICKSTYLE, &ps, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES, 0);

		sds_sslength(ss, &l);
		if(l == 1)
		{
			sds_ssname(ss, 0, entity);
			char url[IC_ACADBUF] = "";
			if(cmd_getEntityHL(entity, &url[0], m_toolTipText.GetBuffer(IC_ACADBUF), NULL))
			{
				m_toolTipText.ReleaseBuffer();
				//if the url has no description, show the url instead
				if (m_toolTipText=="")
				{
					m_toolTipText=url;
				}
				nHit = MAKELONG(pt.x, pt.y);
				pTI->hwnd = m_hWnd;
				pTI->uId = nHit;
				pTI->rect = CRect(CPoint(pt.x, pt.y), CSize(pb.resval.rint, pb.resval.rint));
				pTI->lpszText = LPSTR_TEXTCALLBACK;
				pTI->uFlags |= TTF_NOTBUTTON;
			}
			else
			{
				m_toolTipText.ReleaseBuffer();
				m_toolTipText.Empty();
			}
		}
		sds_ssfree(ss);
	}
	return nHit;
}

BOOL CIcadView::OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult)
{
	TOOLTIPTEXT *pTTT = (TOOLTIPTEXT*)pNMHDR;
	if( pTTT )
	{
		COsnapManagerIcad *pOsnapManager = ((CEngineApplication*)SDSApplication::GetEngineApplication())->getOsnapManager();
		const char *tooltipText = pOsnapManager->getOsnapString();
		if(tooltipText)
			strcpy(pTTT->szText, tooltipText);
		else
			if(!m_toolTipText.IsEmpty())
				_tcscpy(pTTT->szText, (LPCTSTR)m_toolTipText);

// 		// Modified CyberAge VSB 12/05/2001
// 		// Reason: Implementation of Flyover Snapping.
// 		//Get the Tooltip text for drawn snap [
// 		if ( pTTT->hdr.idFrom== ID_FLYOVER_TOOLTIP && SnapObject.showtooltips && SnapObject.m_nSnapDrawn )
// 		{
// 			SnapObject.GetToolTipText(pTTT->szText,SnapObject.m_nSnapDrawn);
// 		} // Modified CyberAge VSB 12/05/2001 ]

		else
			pTTT->szText[0] = 0;
	}
	*pResult = 0;
	return TRUE;
}

// // Modified CyberAge VSB 12/05/2001
// // Reason: Implementation of Flyover Snapping.
// // Handing TAB and ESC buttons pressed during flyover snapping. [
void CIcadView::processOsnapChar( UINT nChar )
{
	bool bEnableFlyOver = UserPreference::SnapFlyOverEnablePreference;
	if( !bEnableFlyOver )
		return;

	COsnapManagerIcad *pOsnapManager = ((CEngineApplication*)SDSApplication::GetEngineApplication())->getOsnapManager();
	switch( nChar )
	{
	case 27:
		pOsnapManager->reset( this );
		break;
	case 9:
		{
			pOsnapManager->nextOsnapPoint();
			m_toolTip.Update();
			break;
		}
// 	if( nChar == 27)
// 	{
// 		try
// 		{
// 			resbuf rb = { 0 };
// 			sds_getvar( "OSMODE", &rb );
// 			int osmode = rb.resval.rint;
//
// 			sds_getvar( "AUTOSNAP", &rb ) ;
// 			int autosnap	 = rb.resval.rint ;
//
// 			if( ( ( bGetPoint || SDS_PointDisplayMode )&& !SDS_BoxModeCursor && !(osmode & IC_OSMODE_OFF ) && autosnap) )
// 			{
// 				SDS_BitBlt2Scr(0);
// 				m_ctlToolTip.Pop();
// 			}
// 		}
// 		catch(...)
// 		{
//
// 		}
// 	}
// 	else if( nChar == 9 )
// 	{
// 		try
// 		{
// 			resbuf rb = { 0 };
// 			sds_getvar( "OSMODE", &rb );
// 			int osmode = rb.resval.rint;
//
// 			sds_getvar( "AUTOSNAP", &rb ) ;
// 			int autosnap	 = rb.resval.rint ;
//
// 			if( ( ( bGetPoint || SDS_PointDisplayMode )&& !SDS_BoxModeCursor && !(osmode & IC_OSMODE_OFF ) && autosnap) )
// 			{
//
// 				SNAPSTRUCT SnapStruct = SnapObject.GetNextSnapData();
//
// 				if ( !SnapStruct.bValidSnap && SnapObject.m_nSnapDrawn == NULL)
// 					return;
//
// 				SDS_BitBlt2Scr(0);
// 				SnapObject.GetLatestValues();
// 				CIcadView *pView = SDS_CURVIEW;
// 				CIcadDoc  *pDoc=SDS_CURDOC;
//
// 				POSITION pos=pDoc->GetFirstViewPosition();
// 				while (pos!=NULL)
// 				{
// 					if(Snap_DrawInOneWindow)
// 					{
// 						pView=SDS_CURVIEW;
// 					}
// 					else
// 					{
// 						pView=(CIcadView *)pDoc->GetNextView(pos);
// 					}
//
// 					if(pView==NULL)
// 					{
// 						return;
// 					}
//
// 					ASSERT_KINDOF(CIcadView, pView);
//
// 					ASSERT( !pView->IsPrinting() );
//
// 					pView->SnapObject.m_gr_view  =pView->m_pCurDwgView;
// 					if ( SnapStructLastSnap.bValidSnap )
// 					{
// 						pView->m_iFlyOverSnapX= pView->SnapObject.m_LastSnapPtPixX;
// 						pView->m_iFlyOverSnapY= pView->SnapObject.m_LastSnapPtPixY;
//
// 					}
// 					if ( SnapStruct.bValidSnap )
// 					{
// 						pView->SnapObject.pDC =  pView->GetDC();
// 						int	rop = ::SetROP2( pView->SnapObject.pDC->m_hDC,R2_XORPEN);
// 						pView->SnapObject.DrawSnap(SnapStruct, 1);
// 						::SetROP2( pView->SnapObject.pDC->m_hDC,rop);
// 						pView->ReleaseDC( pView->SnapObject.pDC );
// 					}
// 					else
// 					{
// 						pView->SnapObject.pDC =  pView->GetDC();
// 						int rop = ::SetROP2( pView->SnapObject.pDC->m_hDC,R2_XORPEN);
// 						pView->SnapObject.DeleteSnap();
// 						::SetROP2( pView->SnapObject.pDC->m_hDC,rop);
// 						pView->ReleaseDC( pView->SnapObject.pDC );
// 						if ( pView->SnapObject.m_nSnapDrawn == -1)
// 						{
// 							pView->SnapObject.m_nSnapDrawn = NULL;
// 							pView->Invalidate();
// 						}
// 					}
// 					if(Snap_DrawInOneWindow)
// 						break;
// 				}
// 				if ( !SDS_IsDragging )
// 					m_ctlToolTip.Update();
// 				else
// 					m_ctlToolTip.Pop();
// 			}
// 			else
// 			{
// 				if ( SnapObject.m_nSnapDrawn != NULL )
// 				{
// 					SnapObject.pDC =  this->GetDC();
// 					int rop = ::SetROP2( SnapObject.pDC->m_hDC,R2_XORPEN);
// 					SnapObject.DeleteSnap();
// 					::SetROP2( SnapObject.pDC->m_hDC,rop);
// 					this->ReleaseDC( SnapObject.pDC );
// 					if ( SnapObject.m_nSnapDrawn )
// 					{
// 						SnapObject.m_nSnapDrawn = NULL;
// 						Invalidate();
// 					}
// 				}
// 			}
// 		}
// 		catch(...)
// 		{
//
// 		}
//
// 	}
// 	return;
// }
// // Modified CyberAge VSB 12/05/2001 ]
//
	default:
	 	break;
	}
}

void CIcadView::OnInitialUpdate()
{
	// just create tooltip window here
	m_toolTip.Create(this, TTS_ALWAYSTIP);
	ASSERT( ::IsWindow(m_toolTip.m_hWnd) );
#ifdef _LAYOUTTABS_
	((CIcadChildWnd*)GetParentFrame())->UpdateLayoutTabs(GetDrawing(), TRUE);
	if(!m_showScrollBars)
	{
		GetScrollBarCtrl(SB_HORZ)->ShowWindow(SW_HIDE);
		GetScrollBarCtrl(SB_VERT)->ShowWindow(SW_HIDE);
	}
#endif
//		// Modified CyberAge VSB 12/05/2001
//		// Reason: Implementation of Flyover Snapping.
//		// Creating tooltip ctrl for flyover snapping. [
//
//		m_ctlToolTip.Create(this,TTS_ALWAYSTIP);
//		ASSERT( ::IsWindow(m_ctlToolTip.m_hWnd) );
//		CRect rect;
//		GetClientRect(&rect);
//		m_ctlToolTip.AddTool(this,LPSTR_TEXTCALLBACK,rect,ID_FLYOVER_TOOLTIP);
//
//		Snap_DrawInOneWindow = 0;
//		SnapObject.GetLatestValues();
//
//		// Modified CyberAge VSB 12/05/2001 ]
//
}


// AG: this function is overriden to allow processing of tooltips the way we need it
// it's slightly different from CWnd::PreTranslateMessage()
BOOL CIcadView::PreTranslateMessage(MSG* pMsg)
{
	// SystemMetrix(shiba)-[06.10.2001
	//   FIX: During command(zoom, regen etc) execution, moving mouse (wheel), crash.
    resbuf rb;
    if (sds_getvar("CMDACTIVE", &rb) == RTNORM && rb.resval.rint)
      {
		if (SDS_RedrawDrawing_Doing || SDS_BuildRegenList_Doing){
			UINT message = pMsg->message;
			if (
				 (message == WM_LBUTTONDOWN || message == WM_LBUTTONDBLCLK) ||
				 (message == WM_RBUTTONDOWN || message == WM_RBUTTONDBLCLK) ||
				 (message == WM_MBUTTONDOWN || message == WM_MBUTTONDBLCLK) ||
				 (message == WM_NCLBUTTONDOWN || message == WM_NCLBUTTONDBLCLK) ||
				 (message == WM_NCRBUTTONDOWN || message == WM_NCRBUTTONDBLCLK) ||
				 (message == WM_NCMBUTTONDOWN || message == WM_NCMBUTTONDBLCLK) ||
				 (message == WM_MOUSEMOVE || message == WM_NCMOUSEMOVE) ||
				 (message == WM_LBUTTONUP || message == WM_RBUTTONUP) ||
				 (message == WM_MBUTTONUP) ||
				 (message == WM_NCLBUTTONUP || message == WM_NCRBUTTONUP) ||
				 (message == WM_NCMBUTTONUP) ||
				 (message == WM_MOUSEWHEEL) 		){
				return TRUE;
			}
		}
	}
	// ]-SystemMetrix(shiba) 06.10.2001

//		// Modified CyberAge VSB 12/05/2001
//		// Reason: Implementation of Flyover Snapping.[
//		if (::IsWindow(m_ctlToolTip.m_hWnd) )
//		{
//			m_ctlToolTip.RelayEvent( pMsg );
//		}
//		// Modified CyberAge VSB 12/05/2001 ]
	if (::IsWindow(m_toolTip.m_hWnd) && pMsg->hwnd == m_hWnd)
	{
		FilterToolTipMessage( pMsg );
	}
	return FALSE;	// no processing except for tooltips
}

/*
AG: the implementation of these two functions are taken from standard MFC ones, but customized a bit
to change tooltip controls behaviour. I did it 'cause I had some problems while using standard MFC
per-thread CToolTipCtrl control (which is accessible by CWnd::EnableToolTips() )
*/

void CIcadView::RelayToolTipMessage(const MSG *pMsg)
{
	MSG msg;
	::memcpy( &msg, pMsg, sizeof(MSG) );

	// I'm not sure if we should do the code below, so I commented it out.
/*
	// transate the message based on TTM_WINDOWFROMPOINT
	msg.hwnd = (HWND)m_toolTip.SendMessage(TTM_WINDOWFROMPOINT, 0, (LPARAM)&msg.pt);
	CPoint pt = msg.pt;
	if (msg.message >= WM_MOUSEFIRST && msg.message <= WM_MOUSELAST)
		::ScreenToClient(msg.hwnd, &pt);
	msg.lParam = MAKELONG(pt.x, pt.y);
*/
	// relay mouse event before deleting old tool
	m_toolTip.RelayEvent( &msg );
}

void CIcadView::FilterToolTipMessage(MSG *pMsg)
{
	// this CWnd has tooltips enabled
	UINT message = pMsg->message;
	if ((message == WM_MOUSEMOVE || message == WM_NCMOUSEMOVE ||
		 message == WM_LBUTTONUP || message == WM_RBUTTONUP ||
		 message == WM_MBUTTONUP) &&
		(GetKeyState(VK_LBUTTON) >= 0 && GetKeyState(VK_RBUTTON) >= 0 &&
		 GetKeyState(VK_MBUTTON) >= 0))
	{

		TOOLINFO ti;
		memset(&ti, 0, sizeof(TOOLINFO));

		// determine which tool was hit
		CPoint point = pMsg->pt;
		::ScreenToClient(m_hWnd, &point);
		TOOLINFO tiHit;
		memset(&tiHit, 0, sizeof(TOOLINFO));
		tiHit.cbSize = sizeof(AFX_OLDTOOLINFO);
		int nHit = OnToolHitTest(point, &tiHit);

		// build new toolinfo and if different than current, register it
		if( m_toolTip.m_nLastHit != nHit )
		{
			if (nHit != -1)
			{
				// add new tool and activate the tip
				ti = tiHit;
				ti.uFlags &= ~(TTF_NOTBUTTON|TTF_ALWAYSTIP);

				VERIFY(m_toolTip.SendMessage(TTM_ADDTOOL, 0, (LPARAM)&ti));
				if ((tiHit.uFlags & TTF_ALWAYSTIP) || IsTopParentActive())
				{
					// allow the tooltip to popup when it should
					m_toolTip.Activate( TRUE );

					// bring the tooltip window above other popup windows
					::SetWindowPos(m_toolTip.m_hWnd, HWND_TOP, 0, 0, 0, 0,
						SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE|SWP_NOOWNERZORDER);
				}
			}
			else
			{
				m_toolTip.Activate( FALSE );
			}

			// relay mouse event before deleting old tool
			RelayToolTipMessage(pMsg);

			// to delete an old tool we should pass TOOLINFO structure in LPARAM
			// tool is identified by hwnd and uId (which is nHit in our case)
			// cbSize specifies an actual size of this structure
			// to avoid problems (???) we're checking it before call SendMessage
			if( m_toolTip.m_lastInfo.cbSize >= sizeof(AFX_OLDTOOLINFO))
				m_toolTip.SendMessage(TTM_DELTOOL, 0, (LPARAM)&m_toolTip.m_lastInfo);

			m_toolTip.m_nLastHit = nHit;
			m_toolTip.m_lastInfo = tiHit;

		}
		else
		{
			// relay mouse events through the tooltip
			if (nHit != -1)
				RelayToolTipMessage(pMsg);
		}

		if ((tiHit.lpszText != LPSTR_TEXTCALLBACK) && (tiHit.hinst == 0))
			IC_FREE(tiHit.lpszText);
	}
	else
	{
		if (
			 (message == WM_LBUTTONDOWN || message == WM_LBUTTONDBLCLK) ||
			 (message == WM_RBUTTONDOWN || message == WM_RBUTTONDBLCLK) ||
			 (message == WM_MBUTTONDOWN || message == WM_MBUTTONDBLCLK) ||
			 (message == WM_NCLBUTTONDOWN || message == WM_NCLBUTTONDBLCLK) ||
			 (message == WM_NCRBUTTONDOWN || message == WM_NCRBUTTONDBLCLK) ||
			 (message == WM_NCMBUTTONDOWN || message == WM_NCMBUTTONDBLCLK)
			 )
		{
			m_toolTip.Activate( FALSE );
		}
	}
}

//4M Bugzilla 77987 19/02/02->
void CIcadView::SetRealTime(CRealTime * rt)
{
	m_pRealTime=rt;
}

void CIcadView::DeleteRealTime(UINT id)
{
	if (m_pRealTime)
	{
		KillTimer(id);
	  delete m_pRealTime;
	  m_pRealTime=NULL;
	}
}

void CIcadView::SetRTMotion(RealTimeMotions mt)
{
	if ((mt!=m_RTMotion)&&(mt!=ZOOM_WHEEL)&&(m_RTMotion!=ZOOM_WHEEL))
	{
	  char prompt[15];
	  switch(mt)
	  {
		 case NO        :
			 strcpy(prompt,"NO");
			 break;
		 case ZOOM      :
			 strcpy(prompt,"ZOOM");
			 break;
		 case PAN       :
			 strcpy(prompt,"PAN");
			 break;
		 case ROT_CYL_X :
			 strcpy(prompt,"ROT_CYL_X");
			 break;
		 case ROT_CYL_Y :
			 strcpy(prompt,"ROT_CYL_Y");
			 break;
		 case ROT_CYL_Z :
			 strcpy(prompt,"ROT_CYL_Z");
			 break;
		 case ROT_SPH   :
			 strcpy(prompt,"ROT_SPH");
			 break;
		 case ZOOM_WHEEL:
			 strcpy(prompt,"ZOOM_WHEEL");
			 break;
	  }
	  if (strcmp(prompt, "NO"))	// Shut down some of the superfluous spew
		sds_printf("<RTMotion %s>",prompt);
	}
	m_RTMotion=mt;
}
//<-4M 19/02/02
//  // Modified CyberAge VSB 12/05/2001
//	// Reason: Implementation of Flyover Snapping.
//	// Sets the latest values to the member variables of SnapObject which is member variable of
//	// variable of IcadView. This is required as we can have Flyover Snap in multiple views.[
//	void SnapInfo::GetLatestValues()
//	{
//		resbuf rb = { 0 } ;
//
//		sds_getvar( "SNAPSIZE", &rb ) ;
//		snapsize = rb.resval.rint ;
//
//		sds_getvar( "SNAPTHICKNESS", &rb ) ;
//		thickness = rb.resval.rint ;
//
//		sds_getvar( "BKGCOLOR", &rb ) ;
//		bkgcolor = rb.resval.rint ;
//
//		sds_getvar( "SNAPCOLOR", &rb ) ;
//		snapcolor = rb.resval.rint ;
//
//		sds_getvar( "OSMODE", &rb ) ;
//		osmode = rb.resval.rint;
//
//		sds_getvar( "AUTOSNAP", &rb ) ;
//		autosnap	 = rb.resval.rint ;
//		marker		 = rb.resval.rint & 1;
//		showtooltips = rb.resval.rint & 2;
//
//	}
//
//	// Modified CyberAge VSB 12/05/2001
//	// Reason: Implementation of Flyover Snapping.
//	// Get the string from the OSMODE variable which is used for sds_osnap().
//	void SnapInfo::GetSnapString(char *snapstr,int *osnapmode)
//	{
//		*snapstr=0;
//
//		if (!(*osnapmode)) return;
//
//		if ((*osnapmode) & IC_OSMODE_END) {
//			if (*snapstr) strcat(snapstr,","/*DNT*/);
//			strcat(snapstr,ResourceString(IDC_SDS_OSNAP_END_0, "END" ));
//		}
//		if ((*osnapmode) & IC_OSMODE_MID) {
//			if (*snapstr) strcat(snapstr,","/*DNT*/);
//			strcat(snapstr,ResourceString(IDC_SDS_OSNAP_MID_1, "MID" ));
//		}
//		if ((*osnapmode) & IC_OSMODE_CENTER) {
//			if (*snapstr) strcat(snapstr,","/*DNT*/);
//			strcat(snapstr,ResourceString(IDC_SDS_OSNAP_CEN_2, "CEN" ));
//		}
//		if ((*osnapmode) & IC_OSMODE_NODE) {
//			if (*snapstr) strcat(snapstr,","/*DNT*/);
//			strcat(snapstr,ResourceString(IDC_SDS_OSNAP_NOD_3, "NOD" ));
//		}
//		if ((*osnapmode) & IC_OSMODE_QUADRANT) {
//			if (*snapstr) strcat(snapstr,","/*DNT*/);
//			strcat(snapstr,ResourceString(IDC_SDS_OSNAP_QUA_5, "QUA" ));
//		}
//		if ((*osnapmode) & IC_OSMODE_INT) {
//			if (*snapstr) strcat(snapstr,","/*DNT*/);
//			strcat(snapstr,ResourceString(IDC_SDS_OSNAP_INT_6, "INT" ));
//		}
//		if ((*osnapmode) & IC_OSMODE_INS) {
//			if (*snapstr) strcat(snapstr,","/*DNT*/);
//			strcat(snapstr,ResourceString(IDC_SDS_OSNAP_INS_7, "INS" ));
//		}
//		if ((*osnapmode) & IC_OSMODE_PERP) {
//			if (*snapstr) strcat(snapstr,","/*DNT*/);
//			strcat(snapstr,ResourceString(IDC_SDS_OSNAP_PER_8, "PER" ));
//		}
//		if ((*osnapmode) & IC_OSMODE_TAN) {
//			if (*snapstr) strcat(snapstr,","/*DNT*/);
//			strcat(snapstr,ResourceString(IDC_SDS_OSNAP_TAN_9, "TAN" ));
//		}
//		if ((*osnapmode) & IC_OSMODE_NEAR) {
//			if (*snapstr) strcat(snapstr,","/*DNT*/);
//			strcat(snapstr,"NEA"/*DNT*/);
//		}
//		if ((*osnapmode) & IC_OSMODE_QUICK) {
//			if (*snapstr) strcat(snapstr,","/*DNT*/);
//			strcat(snapstr,ResourceString(IDC_SDS_OSNAP_QUI_11, "QUI" ));
//		}
//		if ((*osnapmode) & IC_OSMODE_APP) {
//			if (*snapstr) strcat(snapstr,","/*DNT*/);
//			strcat(snapstr,ResourceString(IDC_SDS_OSNAP_VIS_13, "VIS" ));
//		}
//		if ((*osnapmode) & IC_OSMODE_PLAN) {
//			if (*snapstr) strcat(snapstr,","/*DNT*/);
//			strcat(snapstr,ResourceString(IDC_SDS_OSNAP_PLA_14, "PLA" ));
//		}
//	}
//
//	// Modified CyberAge VSB 12/05/2001
//	// Reason: Implementation of Flyover Snapping.
//	// Main function which calls sds_osnap() and return the snapinfo and according snap is
//	// drawn or deleted.The point passes is the MoveMove point.
//	SNAPSTRUCT	SnapInfo::TryEntitySnap(CPoint &point)
//	{
//		TempSnapStruct.bValidSnap = FALSE;
//
//		sds_point ptFrom;
//		sds_point pt = {0};
//		gr_pix2rp(m_gr_view , point.x, point.y, &pt[0], &pt[1] ) ;
//		gr_rp2ucs( pt, SnapPt, m_gr_view) ;
//
//		char snapstr[75];
//		GetSnapString(snapstr,&osmode);
//		g_nSnappedType = 0;
//		int ostype = sds_osnap( SnapPt,snapstr, ptFrom) ;
//
//		if ( ostype == RTNORM )
//		{
//			if ( g_nSnappedType  != 0 )
//			{
//				TempSnapStruct.nSnappedType = g_nSnappedType ;
//				TempSnapStruct.bValidSnap = TRUE;
//				ic_ptcpy( TempSnapStruct.pt,ptFrom);
//
//				SnapStructLastSnap.nSnappedType = g_nSnappedType ;
//				SnapStructLastSnap.bValidSnap = TRUE;
//				ic_ptcpy( SnapStructLastSnap.pt,ptFrom);
//			}
//		}
//		else
//		{
//			TempSnapStruct.bValidSnap = FALSE;
//			SnapStructLastSnap.bValidSnap = FALSE;
//		}
//
//		nNextSnap = 0;
//		return TempSnapStruct;
//	}
//
//	// Modified CyberAge VSB 12/05/2001
//	// Reason: Implementation of Flyover Snapping.
//	// This function is used while TABbing in flyover snap. This function calls sds_osnap() and return the snapinfo and according snap is
//	// drawn or deleted.The point used the last MoveMove point stored when last snap was drawn.
//	SNAPSTRUCT SnapInfo::GetNextSnapData()
//	{
//		const int MAXSNAP = 11;
//		static int	SnapVal [MAXSNAP]	 = {0 ,IC_OSMODE_END,IC_OSMODE_NEAR,IC_OSMODE_PERP,IC_OSMODE_QUADRANT,IC_OSMODE_INS,IC_OSMODE_CENTER,IC_OSMODE_MID,IC_OSMODE_NODE,IC_OSMODE_TAN,IC_OSMODE_INT};
//		static char SnapData[][4]		 = {"","END"		,"NEA"		  ,"PER"		 ,"QUA"				,"INS"		  ,"CEN"		   ,"MID"		 ,"NOD"			,"TAN"		  ,"INT"};
//
//		if ( !SnapStructLastSnap.bValidSnap )
//			return TempSnapStruct;
//
//		sds_resbuf rb;
//		sds_getvar( "OSMODE", &rb ) ;
//		osmode = rb.resval.rint;
//
//		if ( !osmode || autosnap == 0 )
//		{
//			return TempSnapStruct;
//		}
//
//		TempSnapStruct.bValidSnap = FALSE;
//
//		char Snapstr[4];
//		int ostype = RTERROR;
//		do {
//
//			nNextSnap++;
//
//			if ( nNextSnap >= MAXSNAP )
//			{
//				nNextSnap = 1;
//			}
//
//			strcpy(Snapstr, SnapData[nNextSnap]);
//
//			ostype = RTERROR;
//			if ( osmode & SnapVal[nNextSnap] )
//			{
//				sds_point ptFrom = {0};
//
//				ostype = sds_osnap(SnapPt,Snapstr, ptFrom) ;
//
//				if ( ostype == RTNORM )
//				{
//					TempSnapStruct.nSnappedType = SnapVal[nNextSnap] ;
//					TempSnapStruct.bValidSnap = TRUE;
//					ic_ptcpy( TempSnapStruct.pt,ptFrom);
//
//					SnapStructLastSnap.nSnappedType = SnapVal[nNextSnap] ;
//					SnapStructLastSnap.bValidSnap = TRUE;
//					ic_ptcpy( SnapStructLastSnap.pt,ptFrom);
//					break;
//				}
//				else
//				{
//					TempSnapStruct.bValidSnap = FALSE;
//				}
//			}
//
//		}while ( ostype == RTERROR );
//
//		return TempSnapStruct;
//	}
//
//	// Modified CyberAge VSB 12/05/2001
//	// Reason: Implementation of Flyover Snapping.
//	//Deletes the Snap drawn previously.
//	void SnapInfo::DeleteSnap()
//	{
//		if ( m_nSnapDrawn  == NULL )
//			return;
//
//		if ( SDS_IsDragging )
//		{
//			m_nSnapDrawn  = -1;
//			m_nSnapDrawn  = NULL;
//			return;
//		}
//		if ( marker )
//		{
//			bool checkBackgrd = true;
//			// Modified CyberAge VSB 20/05/2001
//			// Now using XOR pen to draw snap
//			//int color = bkgcolor;
//			int color = snapcolor;
//			resbuf rb;
//			SDS_getvar(NULL, DB_QBKGCOLOR, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
//			CPen pen;
//			if (checkBackgrd && (rb.resval.rint == color))
//			{
//				pen.CreatePen(PS_SOLID,thickness,SDS_XORPenColorFromACADColor(256));
//			}
//			else
//			{
//				if (color == 256)
//					pen.CreatePen(PS_SOLID,thickness,SDS_PenColorFromACADColor(7));
//				else
//				{
//					pen.CreatePen(PS_SOLID,thickness,SDS_XORPenColorFromACADColor(color));
//				}
//			}
//
//			switch ( m_nSnapDrawn )
//			{
//			case IC_OSMODE_END:
//				DrawSnapForEND(m_lastsnappoint[0] , m_lastsnappoint[1] ,  pen);
//				break;
//			case IC_OSMODE_NEAR :
//				DrawSnapForNEA(m_lastsnappoint[0] , m_lastsnappoint[1] ,  pen);
//				break;
//			case IC_OSMODE_PERP :
//				DrawSnapForPER(m_lastsnappoint[0] , m_lastsnappoint[1] ,  pen);
//				break;
//			case IC_OSMODE_QUADRANT :
//				DrawSnapForQUA(m_lastsnappoint[0] , m_lastsnappoint[1] ,  pen);
//				break;
//			case IC_OSMODE_INS :
//				DrawSnapForINS(m_lastsnappoint[0] , m_lastsnappoint[1] ,  pen);
//				break;
//			case IC_OSMODE_CENTER:
//				DrawSnapForCEN(m_lastsnappoint[0] , m_lastsnappoint[1] ,  pen);
//				break;
//			case IC_OSMODE_MID :
//				DrawSnapForMID(m_lastsnappoint[0] , m_lastsnappoint[1] ,  pen);
//				break;
//			case IC_OSMODE_NODE:
//				DrawSnapForNOD(m_lastsnappoint[0] , m_lastsnappoint[1] ,  pen );
//				break;
//			case IC_OSMODE_TAN:
//				DrawSnapForTAN(m_lastsnappoint[0] , m_lastsnappoint[1] ,  pen);
//				break;
//			case IC_OSMODE_INT:
//				DrawSnapForINT(m_lastsnappoint[0] , m_lastsnappoint[1] ,  pen);
//				break;
//			}
//			pen.DeleteObject();
//
//		}
//		m_nSnapDrawn = -1;
//	}
//
//	// Modified CyberAge VSB 12/05/2001
//	// Reason: Implementation of Flyover Snapping.
//	// Draws the snap
//	void SnapInfo::DrawSnap(SNAPSTRUCT &SnapData, int iDragging )
//	{
//		int ptx ;
//		int pty ;
//		sds_point resultpt = { 0 } ;
//
//		resultpt[0] = SnapData.pt[0];
//		resultpt[1] = SnapData.pt[1];
//		resultpt[2] = SnapData.pt[2];
//
//		gr_ucs2rp( resultpt, resultpt, m_gr_view ) ;
//		gr_rp2pix( m_gr_view , resultpt[0], resultpt[1], &ptx, &pty ) ;
//		m_LastSnapPtPixX = ptx;
//		m_LastSnapPtPixY = pty;
//		if ( marker && iDragging )
//		{
//			bool checkBackgrd = false;
//			int color = snapcolor;
//			resbuf rb;
//			SDS_getvar(NULL, DB_QBKGCOLOR, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
//			CPen pen;
//			// Modified CyberAge VSB 20/05/2001
//			// Now using XOR pen to draw snap
//			if (checkBackgrd && (rb.resval.rint == color))
//			{
//				pen.CreatePen(PS_SOLID,thickness,SDS_XORPenColorFromACADColor(256));
//			}
//			else
//			{
//				if (color == 256)
//				{
//					pen.CreatePen(PS_SOLID,thickness,SDS_PenColorFromACADColor(7));
//				}
//				else
//				{
//					pen.CreatePen(PS_SOLID,thickness,SDS_XORPenColorFromACADColor(color));
//				}
//			}
//			switch( SnapData.nSnappedType  )
//			{
//				case IC_OSMODE_END:
//							DrawSnapForEND(ptx, pty,  pen);
//							break;
//				case IC_OSMODE_NEAR :
//							DrawSnapForNEA(ptx, pty,  pen);
//							break;
//				case IC_OSMODE_PERP :
//							DrawSnapForPER(ptx, pty, pen);
//							break;
//				case IC_OSMODE_QUADRANT :
//							DrawSnapForQUA(ptx, pty,  pen);
//							break;
//				case IC_OSMODE_INS :
//							DrawSnapForINS(ptx, pty,  pen);
//							break;
//				case IC_OSMODE_CENTER:
//							DrawSnapForCEN(ptx, pty,  pen);
//							break;
//				case IC_OSMODE_MID:
//							DrawSnapForMID(ptx, pty,  pen);
//							break;
//				case IC_OSMODE_NODE:
//							DrawSnapForNOD(ptx, pty,  pen);
//							break;
//				case IC_OSMODE_TAN:
//							DrawSnapForTAN(ptx, pty,  pen);
//							break;
//				case IC_OSMODE_INT:
//							DrawSnapForINT(ptx, pty,  pen);
//							break;
//				default:
//							break;
//				}
//			pen.DeleteObject();
//		}
//
//		m_lastsnappoint[0] = ptx;
//		m_lastsnappoint[1] = pty;
//		m_nSnapDrawn = SnapData.nSnappedType;
//	}
//	// Modified CyberAge VSB 12/05/2001
//	// Reason: Implementation of Flyover Snapping.
//	// Draws Perpendiclar snap
//	void SnapInfo::DrawSnapForPER(int ptx, int pty, CPen & pen)
//	{
//		HGDIOBJ SavObj = NULL ;
//		POINT DrPt[3] = { 0 } ;
//
//		DrPt[0].x = ptx - snapsize ; DrPt[0].y = pty + snapsize ;
//		DrPt[1].x = ptx - snapsize ; DrPt[1].y = pty - snapsize ;
//		DrPt[2].x = ptx + snapsize ; DrPt[2].y = pty + snapsize ;
//
//		SavObj=SelectObject(pDC->m_hDC,pen);
//
//		pDC->MoveTo( DrPt[0] ) ;
//		pDC->LineTo( DrPt[1] ) ;
//		pDC->MoveTo( DrPt[0] ) ;
//		pDC->LineTo( DrPt[2] ) ;
//
//		DrPt[0].x = ptx ; DrPt[0].y = pty + snapsize ;
//		DrPt[1].x = ptx ; DrPt[1].y = pty ;
//		DrPt[2].x = ptx - snapsize ; DrPt[2].y = pty ;
//
//		pDC->MoveTo( DrPt[0] ) ;
//		pDC->LineTo( DrPt[1] ) ;
//		pDC->LineTo( DrPt[2] ) ;
//
//		DeleteObject( SelectObject(pDC->m_hDC,SavObj) ) ;
//	}
//	// Modified CyberAge VSB 12/05/2001
//	// Reason: Implementation of Flyover Snapping.
//	// Draws Perpendiclar Insertion snap
//	void SnapInfo::DrawSnapForINS(int ptx, int pty, CPen &pen)
//	{
//		HGDIOBJ SavObj = NULL ;
//		POINT DrPt[9] = { 0 } ;
//
//		SavObj=SelectObject(pDC->m_hDC,pen);
//
//		DrPt[0].x = ptx + 1			; DrPt[0].y = pty - 1 ;
//		DrPt[1].x = ptx + snapsize	; DrPt[1].y = pty - 1 ;
//		DrPt[2].x = ptx + snapsize	; DrPt[2].y = pty + snapsize ;
//		DrPt[3].x = ptx - 1			; DrPt[3].y = pty + snapsize ;
//		DrPt[4].x = ptx - 1			; DrPt[4].y = pty + 1 ;
//		DrPt[5].x = ptx - snapsize	; DrPt[5].y = pty + 1 ;
//		DrPt[6].x = ptx - snapsize	; DrPt[6].y = pty - snapsize ;
//		DrPt[7].x = ptx + 1			; DrPt[7].y = pty - snapsize ;
//		DrPt[8].x = ptx + 1			; DrPt[8].y = pty - 1 ;
//
//		::Polyline(pDC->m_hDC,DrPt,9);
//		DeleteObject( SelectObject(pDC->m_hDC,SavObj) ) ;
//
//	}
//
//	// Modified CyberAge VSB 12/05/2001
//	// Reason: Implementation of Flyover Snapping.
//	// Draws Node snap
//	void SnapInfo::DrawSnapForNOD(int ptx, int pty, CPen &pen)
//	{
//		HGDIOBJ SavObj = NULL ;
//		POINT DrPt[5] = { 0 } ;
//
//		DrPt[0].x = ptx - snapsize ; DrPt[0].y = pty - snapsize ;
//		DrPt[1].x = ptx	+ snapsize ; DrPt[1].y = pty + snapsize ;
//		DrPt[2].x = ptx - snapsize ; DrPt[2].y = pty + snapsize ;
//		DrPt[3].x = ptx	+ snapsize ; DrPt[3].y = pty - snapsize ;
//
//		SavObj=SelectObject(pDC->m_hDC,pen);
//		CGdiObject *oldbrush = pDC->SelectStockObject( HOLLOW_BRUSH ) ;
//		pDC->Ellipse( ptx - snapsize, pty - snapsize, ptx + snapsize, pty + snapsize ) ;
//		pDC->MoveTo( DrPt[0] ) ;
//		pDC->LineTo( DrPt[1] ) ;
//		pDC->MoveTo( DrPt[2] ) ;
//		pDC->LineTo( DrPt[3] ) ;
//		DeleteObject( SelectObject(pDC->m_hDC,SavObj) ) ;
//		pDC->SelectObject( oldbrush ) ;
//
//	}
//
//	// Modified CyberAge VSB 12/05/2001
//	// Reason: Implementation of Flyover Snapping.
//	// Draws END snap
//	void SnapInfo::DrawSnapForEND(int ptx, int pty, CPen &pen)
//	{
//		HGDIOBJ SavObj = NULL ;
//		POINT DrPt[5] = { 0 } ;
//
//		DrPt[0].x = ptx + snapsize ; DrPt[0].y = pty + snapsize ;
//		DrPt[1].x = ptx - snapsize ; DrPt[1].y = pty + snapsize ;
//		DrPt[2].x = ptx - snapsize ; DrPt[2].y = pty - snapsize ;
//		DrPt[3].x = ptx + snapsize ; DrPt[3].y = pty - snapsize ;
//		DrPt[4].x = ptx + snapsize ; DrPt[4].y = pty + snapsize ;
//
//		SavObj=SelectObject(pDC->m_hDC,pen);
//		::Polyline(pDC->m_hDC,DrPt,5);
//		DeleteObject( SelectObject(pDC->m_hDC,SavObj) ) ;
//	}
//
//	// Modified CyberAge VSB 12/05/2001
//	// Reason: Implementation of Flyover Snapping.
//	// Draws MID snap
//	void SnapInfo::DrawSnapForMID(int ptx, int pty, CPen &pen)
//	{
//		HGDIOBJ SavObj = NULL ;
//		POINT DrPt[4] = { 0 } ;
//
//		DrPt[0].x = ptx + snapsize ; DrPt[0].y = pty + snapsize ;
//		DrPt[1].x = ptx - snapsize ; DrPt[1].y = pty + snapsize ;
//		DrPt[2].x = ptx ;			 DrPt[2].y = pty - snapsize ;
//		DrPt[3].x = ptx + snapsize ; DrPt[3].y = pty + snapsize ;
//
//		SavObj=SelectObject(pDC->m_hDC,pen);
//		::Polyline(pDC->m_hDC,DrPt,4);
//		DeleteObject( SelectObject(pDC->m_hDC,SavObj) ) ;
//	}
//
//	// Modified CyberAge VSB 12/05/2001
//	// Reason: Implementation of Flyover Snapping.
//	// Draws Intersection snap
//	void SnapInfo::DrawSnapForINT(int ptx, int pty, CPen &pen)
//	{
//		HGDIOBJ SavObj = NULL ;
//		POINT DrPt[4] = { 0 } ;
//
//		DrPt[0].x = ptx - snapsize ; DrPt[0].y = pty - snapsize ;
//		DrPt[1].x = ptx + snapsize ; DrPt[1].y = pty + snapsize ;
//		DrPt[2].x = ptx + snapsize ; DrPt[2].y = pty - snapsize ;
//		DrPt[3].x = ptx - snapsize ; DrPt[3].y = pty + snapsize ;
//
//		SavObj=SelectObject(pDC->m_hDC,pen);
//		pDC->MoveTo( DrPt[0].x , DrPt[0].y ) ;
//		pDC->LineTo( DrPt[1].x , DrPt[1].y ) ;
//		pDC->MoveTo( DrPt[2].x , DrPt[2].y ) ;
//		pDC->LineTo( DrPt[3].x , DrPt[3].y ) ;
//		DeleteObject( SelectObject(pDC->m_hDC,SavObj) ) ;
//	}
//	// Modified CyberAge VSB 12/05/2001
//	// Reason: Implementation of Flyover Snapping.
//	// Draws Nearest snap
//	void SnapInfo::DrawSnapForNEA(int ptx, int pty, CPen &pen )
//	{
//
//		HGDIOBJ SavObj = NULL ;
//		POINT DrPt[5] = { 0 } ;
//
//		DrPt[0].x = ptx - snapsize ; DrPt[0].y = pty - snapsize ;
//		DrPt[1].x = ptx + snapsize ; DrPt[1].y = pty - snapsize ;
//		DrPt[2].x = ptx - snapsize ; DrPt[2].y = pty + snapsize ;
//		DrPt[3].x = ptx + snapsize ; DrPt[3].y = pty + snapsize ;
//		DrPt[4].x = ptx - snapsize ; DrPt[4].y = pty - snapsize ;
//
//		SavObj=SelectObject(pDC->m_hDC,pen);
//		::Polyline( pDC->m_hDC, DrPt, 5) ;
//
//		DeleteObject( SelectObject( pDC->m_hDC, SavObj ) ) ;
//	}
//
//	// Modified CyberAge VSB 12/05/2001
//	// Reason: Implementation of Flyover Snapping.
//	// Draws Center snap
//	void SnapInfo::DrawSnapForCEN(int ptx, int pty, CPen &pen)
//	{
//		HGDIOBJ SavObj = NULL ;
//
//		SavObj=SelectObject(pDC->m_hDC,pen);
//		CGdiObject *oldbrush = pDC->SelectStockObject( HOLLOW_BRUSH ) ;
//		pDC->Ellipse( ptx - snapsize, pty - snapsize, ptx + snapsize, pty + snapsize ) ;
//
//		DeleteObject( SelectObject( pDC->m_hDC, SavObj ) ) ;
//		pDC->SelectObject( oldbrush ) ;
//
//	}
//
//	// Modified CyberAge VSB 12/05/2001
//	// Reason: Implementation of Flyover Snapping.
//	// Draws Quadrant snap
//	void SnapInfo::DrawSnapForQUA(int ptx, int pty, CPen &pen)
//	{
//		HGDIOBJ SavObj = NULL ;
//		POINT DrPt[5] = { 0 } ;
//
//		DrPt[0].x = ptx + snapsize	; DrPt[0].y = pty ;
//		DrPt[1].x = ptx				; DrPt[1].y = pty + snapsize ;
//		DrPt[2].x = ptx - snapsize	; DrPt[2].y = pty ;
//		DrPt[3].x = ptx				; DrPt[3].y = pty - snapsize ;
//		DrPt[4].x = ptx + snapsize	; DrPt[4].y = pty ;
//
//		SavObj=SelectObject(pDC->m_hDC,pen);
//		::Polyline(pDC->m_hDC,DrPt,5);
//
//		DeleteObject( SelectObject(pDC->m_hDC,SavObj) ) ;
//
//	}
//
//	// Modified CyberAge VSB 12/05/2001
//	// Reason: Implementation of Flyover Snapping.
//	// Draws Tangent snap
//	void SnapInfo::DrawSnapForTAN(int ptx, int pty, CPen &pen)
//	{
//		HGDIOBJ SavObj = NULL ;
//		POINT DrPt[2] = { 0 } ;
//
//		DrPt[0].x = ptx - snapsize - 1 ; DrPt[0].y = pty - snapsize ;
//		DrPt[1].x = ptx	+ snapsize + 1 ; DrPt[1].y = pty - snapsize ;
//
//		SavObj=SelectObject(pDC->m_hDC,	pen);
//		CGdiObject *oldbrush = pDC->SelectStockObject( HOLLOW_BRUSH ) ;
//		pDC->Ellipse( ptx - snapsize, pty - snapsize, ptx + snapsize, pty + snapsize ) ;
//		pDC->MoveTo( DrPt[0] ) ;
//		pDC->LineTo( DrPt[1] ) ;
//
//		DeleteObject( SelectObject(pDC->m_hDC,SavObj) ) ;
//		pDC->SelectObject( oldbrush ) ;
//	}
//
//	// Modified CyberAge VSB 12/05/2001
//	// Reason: Implementation of Flyover Snapping.
//	// Set the tooltip text according to snap drawn
//	void SnapInfo::GetToolTipText(char * szText, int nSnapDrawn)
//	{
//		if ( szText == NULL )
//			return;
//
//		*szText = 0;
//		CString strToolTip;
//		switch ( nSnapDrawn )
//		{
//		case IC_OSMODE_END :
//			strToolTip = ResourceString(TOOLTIPSTR_END, "EndPoint" );
//			strcpy(szText,strToolTip);
//			break;
//		case IC_OSMODE_NEAR :
//			strToolTip = ResourceString(TOOLTIPSTR_NEA, "Nearest" );
//			strcpy(szText,strToolTip);
//			break;
//		case IC_OSMODE_PERP :
//			strToolTip = ResourceString(TOOLTIPSTR_PER, "Perpendicular" );
//			strcpy(szText,strToolTip);
//			break;
//		case IC_OSMODE_QUADRANT :
//			strToolTip = ResourceString(TOOLTIPSTR_QUA, "Quadrant" );
//			strcpy(szText,strToolTip);
//			break;
//		case IC_OSMODE_INS :
//			strToolTip = ResourceString(TOOLTIPSTR_INS, "Insertion" );
//			strcpy(szText,strToolTip);
//			break;
//		case IC_OSMODE_CENTER:
//			strToolTip = ResourceString(TOOLTIPSTR_CEN, "Center" );
//			strcpy(szText,strToolTip);
//			break;
//		case IC_OSMODE_MID	:
//			strToolTip = ResourceString(TOOLTIPSTR_MID, "MidPoint" );
//			strcpy(szText,strToolTip);
//			break;
//		case IC_OSMODE_NODE:
//			strToolTip = ResourceString(TOOLTIPSTR_NOD, "Node" );
//			strcpy(szText,strToolTip);
//			break;
//		case IC_OSMODE_TAN:
//			strToolTip = ResourceString(TOOLTIPSTR_TAN, "Tangent" );
//			strcpy(szText,strToolTip);
//			break;
//		case IC_OSMODE_INT:
//			strToolTip = ResourceString(TOOLTIPSTR_INT, "Intersection" );
//			strcpy(szText,strToolTip);
//			break;
//		default :
//			szText[0] = 0;
//		}
//	}
//	// Modified CyberAge VSB 12/05/2001 ]
