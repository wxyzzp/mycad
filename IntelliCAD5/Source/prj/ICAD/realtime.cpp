/* REALTIME.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 * CRealTimexxx classes : implementation
 *
 */
#include "Icad.h" /*DNT*/
#include "realtime.h" /*DNT*/
#include "IcadView.h"
#include "IcadDoc.h"
#include "cmds.h"/*DNT*/

#include "threadcontroller.h" /*DNT*/
#include "IcadCursor.h"

#define SDS_ZOOMRATIO_MIN 2.75e-14 //NOTE:  Acad uses 2.98e-14
#define SDS_ZOOMRATIO_MAX 1.0e300

// ***********************************
// CLASSES IMPLEMENTATION
//

//
// CRealTime
//
CRealTime::CRealTime(CIcadView* pView, POINT ptStart)
{
	m_pView = pView;

	m_ptLast.x = ptStart.x;
	m_ptLast.y = ptStart.y;

	m_bContinuousOrbitStarted = false;

	resbuf rb;
	SDS_getvar(NULL, DB_QVIEWMODE, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
	m_iViewMode = rb.resval.rint;
	SDS_getvar(NULL, DB_QTILEMODE, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
	m_iTileMode = rb.resval.rint;
	SDS_getvar(NULL, DB_QCVPORT, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
	m_iCVPort = rb.resval.rint;

	SDS_SetUndo(IC_UNDO_COMMAND_BEGIN, (void *)"RT"/*DNT*/,	NULL, NULL, SDS_CURDWG);

	SDS_FreeNodeList(SDS_CURDOC);

	SDS_SaveDataForPreviousOrUndo(m_pView, SDS_CURDWG, m_iTileMode, m_iCVPort);

	SDS_EventMask(SDS_EVM_MOUSEXYZ|SDS_EVM_MOUSEBUTTON);

	// undraw cursor
	if(m_pView->m_CursorOn && (m_pView->m_rectMspace.PtInRect(ptStart) || !m_pView->m_pClipRectDC))
	{
		IcadCursor::AppCursor().DrawCursor(m_pView->m_LastCursorPos, m_pView->m_LastCursorType, m_pView);
		m_pView->m_CursorOn=0;
	}
}

CRealTime::~CRealTime(void)
{
//4M Bugzilla 77987 19/02/02->
//	m_pView->m_pRealTime = NULL;	/*D.G.*/// This code was added only for drawing attached raster images
	m_pView->SetRealTime( NULL );	/*D.G.*/// This code was added only for drawing attached raster images
//<-4M 19/02/02
    RedrawAfter();                  // after real-time operations
//4M Bugzilla 77987 19/02/02->
/*
	m_pView->m_pRealTime = this;	// (see IcadEntityRenderer::drawImageObject).
*/
	m_pView->SetRealTime(this);	// (see IcadEntityRenderer::drawImageObject).
//<-4M 19/02/02


	// EBATECH(shiba)-[ 2001-06-12 For view synchronize
	if (SDS_CMainWindow->m_pDragVarsCur)
	{
		CIcadView *pView = SDS_CURVIEW;
		CIcadDoc  *pDoc=SDS_CURDOC;

		POSITION pos=pDoc->GetFirstViewPosition();
		while (pos!=NULL)
		{
			pView=(CIcadView *)pDoc->GetNextView(pos);
			if(pView == NULL)
				break;
			if (pView != SDS_CURVIEW)
				SDS_RedrawDrawing(SDS_CURDWG, pView,NULL,1);
		}
	}
	// ]-EBATECH(shiba) 2001-06-12

	SDS_SetCursor(m_iPrevCursorMode);

	/*D.G.*/// The following call of SDS_SaveDataForPreviousOrUndo was added to create an undo unit
	// for REDO command (we should zoom after changing sysvars). It's a temporary decision and may not
	// work in some cases (nested commands, etc.) and it's not nice. Really non-invertable undo-groups
	// should be implemented. (See also view_Ctrl::OnOK function.)
	SDS_SaveDataForPreviousOrUndo(m_pView, SDS_CURDWG, m_iTileMode, m_iCVPort);
	SDS_SetUndo(IC_UNDO_COMMAND_END, (void *)"RT"/*DNT*/, NULL, NULL, SDS_CURDWG);

	/*D.G.*/// Redraw a selection set and its grips.
	// If some problems appears with it, then call sds_ssfree(m_pView->GetDocument()->m_pGripSelection)
	// instead of it to clear the selection set.
	CIcadDoc*	pCurrentDoc = m_pView->GetDocument();
	long		EntNum = 0L;
	sds_name	EntName;
	resbuf		rb;
	SDS_getvar(NULL, DB_QGRIPS, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
	while(sds_ssname(pCurrentDoc->m_pGripSelection, EntNum++, EntName) == RTNORM)
	{
        sds_redraw(EntName, IC_REDRAW_HILITE);
		if(rb.resval.rint)
			SDS_AddGripNodes(pCurrentDoc, EntName, 1);
	}
//4M Item:28->
   SDS_DrawGripNodes(pCurrentDoc);
//<-4M Item:28

	// draw cursor
	IcadCursor::AppCursor().DrawCursor(m_ptLast, m_pView->m_pMain->m_ViewCursorType, m_pView);
	m_pView->m_CursorOn = 1;
	m_pView->m_LastCursorType = m_pView->m_pMain->m_ViewCursorType;
	m_pView->m_LastCursorPos = m_ptLast;

	// EBATECH(shiba)-[ reset event mask (set up by constructor)
	SDS_EventMask(SDS_EVM_KEYCHAR|SDS_EVM_MOUSEBUTTON|SDS_EVM_MENUCOMMAND|SDS_EVM_POPUPCOMMAND);
	// ]-EBATECH(shiba)
}

bool
CRealTime::Continue(POINT ptLatest)
{
	// Parse out multiple mouse moves to get just the latest one
	//
	bool	bFoundNewerMouseMove = false;
	MSG		msgLookAhead;
	HWND	hWnd = m_pView->GetSafeHwnd();

	while(::PeekMessage(&msgLookAhead, hWnd, WM_MOUSEMOVE, WM_MOUSEMOVE, PM_REMOVE))
		bFoundNewerMouseMove = true;	// Just get the latest mouse move

	if(bFoundNewerMouseMove)
	{
		ptLatest.x = LOWORD(msgLookAhead.lParam);
		ptLatest.y = HIWORD(msgLookAhead.lParam);
	}

	return true;
}

bool
// EBATECH(shiba)-[changed
//CRealTime::Continue(short Delta)
CRealTime::Continue(short Delta, CPoint pt)
// ]-EBATECH(shiba)
{
	return false;
}

void
CRealTime::RedrawAfter()
{
	// TO DO. Why panning and zooming in ortho mode are more quickly with SDS_RedrawAfterZoom (the 2nd way)?
	// Also, we can use the 1st way for them and don't SDS_CURDOC->m_RegenListView = NULL for acceleration,
	// but in this case other real-time operations (including panning & zooming in persp. mode) don't work.

	//BugZilla No. 78264; 09-09-2002 [	
	/*if( m_iViewMode ||
		m_Motion == ROT_CYL || m_Motion == ROT_CYL_X || m_Motion == ROT_CYL_Y || m_Motion == ROT_CYL_Z || m_Motion == ROT_SPH )*/
	if( m_iViewMode == 1 ||
		m_Motion == ROT_CYL || m_Motion == ROT_CYL_X || m_Motion == ROT_CYL_Y || m_Motion == ROT_CYL_Z || m_Motion == ROT_SPH )
	//BugZilla No. 78264; 09-09-2002 ]
	{
		SDS_VarToTab(SDS_CURDWG, m_pView->m_pVportTabHip);
		SDS_CURDOC->m_RegenListView = NULL;
		SDS_InitThisView(m_pView);

		switch(m_pView->m_iViewMode)
		{
			case 2:
				cmd_shade();
				break;
			case 1:
				cmd_hide();
				break;
			default: 
				// DP: use here explicit call
				SDS_RedrawDrawing(SDS_CURDWG, NULL, NULL, 0x03);
				//sds_redraw(NULL, IC_REDRAW_DONTCARE);
		}
	}
	else
		SDS_RedrawAfterZoom(m_pView, SDS_CURDWG, m_iTileMode, m_iCVPort, NULL);

	SDS_DBModified(IC_DBMOD_VIEW_MODIFIED);
}


//
// CRealTimeZoom
//
CRealTimeZoom::CRealTimeZoom(CIcadView* pView, POINT ptStart) : CRealTime(pView, ptStart)
{
	m_iPrevCursorMode = SDS_SetCursor(IDC_ZOOM);
	m_Motion = ZOOM;
}

bool
CRealTimeZoom::Continue(POINT ptLatest)
{
	if(CRealTime::Continue(ptLatest))
	{
		int  DeltaY = (ptLatest.y - m_ptLast.y);

		resbuf  rb;

		//BugZilla No. 78264; 09-09-2002 	
		//SDS_getvar(NULL, m_iViewMode ? DB_QLENSLENGTH : DB_QVIEWSIZE, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
		SDS_getvar(NULL, (m_iViewMode == 1) ? DB_QLENSLENGTH : DB_QVIEWSIZE, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);

		//BugZilla No. 78264; 09-09-2002 	
		//if(m_iViewMode)
		if(m_iViewMode == 1)
		{
			if(DeltaY >= 0)
				rb.resval.rreal /= 1.0 + (0.01 * DeltaY);
			else
				rb.resval.rreal *= 1.0 - (0.01 * DeltaY);

			sds_setvar("LENSLENGTH"/*DNT*/, &rb);	/*D.G.*/// We haven't camera lens length in SDS_prevview struct. So, save it to undo stack.
		}
		else
		{
			if(DeltaY >= 0)
				rb.resval.rreal *= 1.0 + (0.01 * DeltaY);
			else
				rb.resval.rreal /= 1.0 - (0.01 * DeltaY);

			SDS_setvar(NULL, DB_QVIEWSIZE, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES, 0);
		}

		m_ptLast.x = ptLatest.x;
		m_ptLast.y = ptLatest.y;

		RedrawAfter();

		return true;
	}
	else
		return false;
}

bool
// EBATECH(shiba)-[changed
//CRealTimeZoom::Continue(short Delta)
CRealTimeZoom::Continue(short Delta, CPoint pt)
// ]-EBATECH(shiba)
{
	resbuf  rb;
	double	ZoomFactor;

	Delta /= WHEEL_DELTA;
	SDS_getvar(NULL, DB_QZOOMFACTOR, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
	if(Delta > 0)
		ZoomFactor = (1.0 + rb.resval.rint * Delta / 100.0);
	else
		ZoomFactor = (1.0 - rb.resval.rint * abs(Delta) / 200.0);

    if(isEqualZeroWithin(ZoomFactor))
        return false; // EBATECH(CNBR) 2004/2/29

	SDS_getvar(NULL, (m_iViewMode == 1) ? DB_QLENSLENGTH : DB_QVIEWSIZE, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);

	double oldViewSize = rb.resval.rreal;
	if(m_iViewMode == 1)
		rb.resval.rreal *= ZoomFactor;
	else
		rb.resval.rreal /= ZoomFactor;
	double newViewSize = rb.resval.rreal;

	m_pView->ScreenToClient(&pt);
	CRect	rectView;
	struct resbuf tilemode;
	struct resbuf cvport;
	SDS_getvar(NULL, DB_QCVPORT, &cvport, m_pView->m_pViewsDoc->m_dbDrawing, &SDS_CURCFG, &SDS_CURSES);
	SDS_getvar(NULL, DB_QTILEMODE, &tilemode, m_pView->m_pViewsDoc->m_dbDrawing, &SDS_CURCFG, &SDS_CURSES);
	if(tilemode.resval.rint == 0 && cvport.resval.rint > 1 && m_pView->m_pVportTabHip->ret_type() == DB_VIEWPORT)
	{
		int x1, x2, y1, y2;
		SDS_GetRectForVP(m_pView, m_pView->m_pVportTabHip,&x1, &x2, &y1, &y2);
		SDS_TabToVar(m_pView->m_pViewsDoc->m_dbDrawing, m_pView->m_pVportTabHip);
		rectView.left = x1;
		rectView.top = y1;
		rectView.right = x2;
		rectView.bottom = y2;
	}
	else
		m_pView->GetClientRect(rectView);

	if(pt.x < rectView.left)
		pt.x = rectView.left;
	else
		if(pt.x > rectView.right)
			pt.x = rectView.right;
	if(pt.y < rectView.top)
		pt.y = rectView.top;
	else
		if(pt.y > rectView.bottom)
			pt.y = rectView.bottom;

	CPoint centerView;
	centerView.x = (rectView.right + rectView.left) / 2;
	centerView.y = (rectView.bottom + rectView.top) / 2;

	sds_point center;
	struct gr_view *pViewData = m_pView->m_pCurDwgView;

	// vector from pViewData center to zoom point (on Screen Coordinate)
	CPoint vecCenter2MouseOld = pt - centerView;
	CPoint vecCenter2MouseNew = vecCenter2MouseOld;

	double	wkdbl[2];
	wkdbl[0] = (double)vecCenter2MouseNew.x;
	wkdbl[1] = (double)vecCenter2MouseNew.y;
	wkdbl[0] *= (newViewSize / oldViewSize);
	wkdbl[1] *= (newViewSize / oldViewSize);
	vecCenter2MouseNew.x = (long)wkdbl[0];
	vecCenter2MouseNew.y = (long)wkdbl[1];

	CPoint	vecMove = vecCenter2MouseOld - vecCenter2MouseNew;
	// move pViewData center point
	centerView += vecMove;
	gr_pix2rp(pViewData, centerView.x, centerView.y, &center[0], &center[1]);
	gr_rp2ucs(center, center, pViewData);

	// FIX: Real time zoom in on mouse wheel to a limit,crash.
	//      check to see that they're not zooming in too far...
	SDS_getvar(NULL, DB_QSCREENSIZE, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
    // EBATECH(CNBR) -[ 2004/2/29
	//if((newViewSize)/rb.resval.rpoint[1]<SDS_ZOOMRATIO_MIN)
	if(icadIsZero(newViewSize/rb.resval.rpoint[1]))
	{
		newViewSize = rb.resval.rpoint[1] * SDS_ZOOMRATIO_MIN;
		cmd_ErrorPrompt(CMD_ERR_ZOOMIN,0);
        return false;
	}
	else if(newViewSize / rb.resval.rpoint[1] > SDS_ZOOMRATIO_MAX)
	{
		newViewSize = rb.resval.rpoint[1] * SDS_ZOOMRATIO_MAX;
		cmd_ErrorPrompt(CMD_ERR_ZOOMOUT, 0);
		return false; 
	}
    // EBATECH(CNBR) ]- 
	// FIX: REGENMODE:OFF crash.(Ignore REGENMODE, but not down.)
	rb.restype = RTREAL;
	rb.resval.rreal = fabs(newViewSize);
	SDS_setvar(NULL, DB_QVIEWSIZE, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES, 0);
	rb.restype = RT3DPOINT;
	ic_ptcpy(rb.resval.rpoint,center);
	SDS_setvar(NULL, DB_QVIEWCTR, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES, 0);
	RedrawAfter();

	m_ptLast.x = pt.x;
	m_ptLast.y = pt.y;
	IcadCursor::AppCursor().DrawCursor(m_ptLast, m_pView->m_LastCursorType, m_pView);
	m_pView->m_CursorOn = 1;
	return true;
}

//
// CRealTimePan
//
CRealTimePan::CRealTimePan(CIcadView* pView, POINT ptStart) : CRealTime(pView, ptStart)
{
	m_iPrevCursorMode = SDS_SetCursor(IDC_ICAD_GRABHAND);
	m_Motion = PAN;
}

bool
CRealTimePan::Continue(POINT ptLatest)
{
	if(CRealTime::Continue(ptLatest))
	{
		resbuf		rb;
		sds_point	PtPan;

		PtPan[0] = (m_ptLast.x - ptLatest.x) * SDS_CURGRVW->GetPixelWidth();
		PtPan[1] = (m_ptLast.y - ptLatest.y) * SDS_CURGRVW->GetPixelHeight();
		PtPan[2] = 0.0;

		SDS_getvar(NULL, m_iViewMode ? DB_QTARGET : DB_QVIEWCTR, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);

		resbuf rbucs, rbdcs;

		rbdcs.restype = rbucs.restype = RTSHORT;
		rbdcs.resval.rint = 2;
		rbucs.resval.rint = 1;
		sds_trans(PtPan, &rbdcs, &rbucs, 1, PtPan);
		rb.resval.rpoint[0] += PtPan[0];
		rb.resval.rpoint[1] += PtPan[1];
		rb.resval.rpoint[2] += PtPan[2];

//		SDS_ZoomIt(NULL, ViewMode ? 1 : 2, rb.resval.rpoint, NULL, NULL, NULL, NULL, NULL);	/*D.G.*/// If you want to see undo/redo more slow (some cartoon:) then uncomment this.

		SDS_setvar(NULL, m_iViewMode ? DB_QTARGET : DB_QVIEWCTR, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES, 0);

		m_ptLast.x = ptLatest.x;
		m_ptLast.y = ptLatest.y;

		RedrawAfter();
		return true;
	}
	else
		return false;
}


//
// CRealTimeRotate_Cylinder
//
CRealTimeRotate_Cylinder::CRealTimeRotate_Cylinder(CIcadView* pView, POINT ptStart, Axes theAxis)
						: CRealTime(pView, ptStart)
{
	m_Motion = ROT_CYL;
	Axis = theAxis;
	switch (Axis)
	{
		case X : m_iPrevCursorMode = SDS_SetCursor(IDC_RT_CYLINDER_X);
				 break;
		case Y : m_iPrevCursorMode = SDS_SetCursor(IDC_RT_CYLINDER_Y);
				 break;
		case Z : m_iPrevCursorMode = SDS_SetCursor(IDC_RT_CYLINDER_Z);
	}

	m_CosRotAngle = 1.0;
	m_SinRotAngle = 0.0;
	m_RotAngle = 0.0;
}

bool
CRealTimeRotate_Cylinder::Continue(POINT ptLatest)
{
	if(CRealTime::Continue(ptLatest))
	{
		resbuf		rb, rbwcs, rbucs, rbdcs;
		sds_point	ViewCtr, OldViewDir, NewViewDir, p1, p2;
		sds_real	ViewDirLen, NewViewTwist;

		rbwcs.rbnext = rbucs.rbnext = rbdcs.rbnext = NULL;
		rbwcs.restype = rbucs.restype = rbdcs.restype = RTSHORT;
		rbwcs.resval.rint = 0;
		rbucs.resval.rint = 1;
		rbdcs.resval.rint = 2;

		if(m_iViewMode)
		{
			SDS_getvar(NULL, DB_QTARGET, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
			ic_ptcpy(ViewCtr, rb.resval.rpoint);
		}
		else
		{
			SDS_getvar(NULL, DB_QVIEWCTR, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
			ic_ptcpy(ViewCtr, rb.resval.rpoint);
		}

		if(!m_bContinuousOrbitStarted)
		{
			CalculateRotation(ptLatest);
			m_ptLast.x = ptLatest.x;
			m_ptLast.y = ptLatest.y;
		}

		switch(Axis)
		{
		case X:
			// for viewdir
			NewViewDir[0] = 0.0;
			NewViewDir[1] = m_SinRotAngle;
			NewViewDir[2] = m_CosRotAngle;

			// for viewtwist
			p1[0] = p1[1] = 0.0; p1[2] = 1.0;
			sds_trans(p1, &rbwcs, &rbdcs, 1, p2);
			p1[0] = p2[0];
			p1[1] = p2[1] * m_CosRotAngle - p2[2] * m_SinRotAngle;
			p1[2] = p2[2] * m_CosRotAngle + p2[1] * m_SinRotAngle;

			break;

		case Y:
			// for viewdir
			NewViewDir[0] = m_SinRotAngle;
			NewViewDir[1] = 0.0;
			NewViewDir[2] = m_CosRotAngle;

			// for viewtwist
			p1[0] = p1[1] = 0.0; p1[2] = 1.0;
			sds_trans(p1, &rbwcs, &rbdcs, 1, p2);
			p1[1] = p2[1];
			p1[0] = p2[0] * m_CosRotAngle - p2[2] * m_SinRotAngle;
			p1[2] = p2[2] * m_CosRotAngle + p2[0] * m_SinRotAngle;

			break;

		case Z:
			SDS_getvar(NULL, DB_QVIEWTWIST, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
			rb.resval.rreal += m_RotAngle;
			ic_normang(&rb.resval.rreal, NULL);
			sdsengine_setvar("VIEWTWIST"/*DNT*/, &rb);

			rb.restype = RT3DPOINT;
			ic_ptcpy(rb.resval.rpoint, ViewCtr);
			sdsengine_setvar("VIEWCTR"/*DNT*/, &rb);
		} // switch

		if(Axis != Z)
		{
			// for viewdir
			sds_trans(NewViewDir, &rbdcs, &rbucs, 1, NewViewDir);
			if(fabs(NewViewDir[0]) < IC_ZRO)
				NewViewDir[0] = 0.0;
			if(fabs(NewViewDir[1]) < IC_ZRO)
				NewViewDir[1] = 0.0;
			if(fabs(NewViewDir[2]) < IC_ZRO)
				NewViewDir[2] = 0.0;

			SDS_getvar(NULL, DB_QVIEWDIR, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
			ic_ptcpy(OldViewDir, rb.resval.rpoint);
			ViewDirLen = sqrt(OldViewDir[0] * OldViewDir[0] +
							  OldViewDir[1] * OldViewDir[1] +
							  OldViewDir[2] * OldViewDir[2]);
			NewViewDir[0] *= ViewDirLen;
			NewViewDir[1] *= ViewDirLen;
			NewViewDir[2] *= ViewDirLen;

			// for viewtwist
			if(fabs(p1[0]) < IC_ZRO && fabs(p1[1]) < IC_ZRO)
			{
				//if we'll now be looking parallel to the WCS Z axis, use Y axis for viewtwist
				SDS_getvar(NULL, DB_QUCSYDIR, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
				ic_ptcpy(p1, rb.resval.rpoint);
				sds_trans(p1, &rbwcs, &rbdcs, 1, p2);
				if(Axis == X)
				{
					p1[0] = p2[0];
					p1[1] = p2[1] * m_CosRotAngle - p2[2] * m_SinRotAngle;
					p1[2] = p2[2] * m_CosRotAngle + p2[1] * m_SinRotAngle;
				}
				else
				{
					p1[1] = p2[1];
					p1[0] = p2[0] * m_CosRotAngle - p2[2] * m_SinRotAngle;
					p1[2] = p2[2] * m_CosRotAngle + p2[0] * m_SinRotAngle;
				}
			}
			NewViewTwist = atan2(p1[1], p1[0]);
			if(NewViewTwist < (IC_PI / 2.0))
				NewViewTwist += IC_TWOPI;
			NewViewTwist -= IC_PI / 2.0;

			// set vars
			rb.restype = RT3DPOINT;
			ic_ptcpy(rb.resval.rpoint, NewViewDir);
			sdsengine_setvar("VIEWDIR"/*DNT*/, &rb);

			rb.restype = RTREAL;
			rb.resval.rreal = NewViewTwist;
			sdsengine_setvar("VIEWTWIST"/*DNT*/, &rb);

			rb.restype = RT3DPOINT;
			ic_ptcpy(rb.resval.rpoint, ViewCtr);
			sdsengine_setvar("VIEWCTR"/*DNT*/, &rb);
		} // if(Axis != Z)

		RedrawAfter();

		return true;
	}
	else
		return false;
}

void
CRealTimeRotate_Cylinder::CalculateRotation(POINT ptLatest)
{
	POINT		WinCenter;
	sds_point	StartVector, EndVector;
	sds_real	CylinderRadius_2, StartVectorLength, EndVectorLength;

	WinCenter.x = m_pView->m_iWinX / 2;
	WinCenter.y = m_pView->m_iWinY / 2;

	switch(Axis)
	{
	case X:
		if(m_ptLast.y == ptLatest.y)
			return;

		CylinderRadius_2 = WinCenter.y * WinCenter.y;

		StartVector[1] = m_ptLast.y - WinCenter.y;
		StartVector[0] = sqrt(CylinderRadius_2 - StartVector[1] * StartVector[1]);
		EndVector[1] = ptLatest.y - WinCenter.y;
		EndVector[0] = sqrt(CylinderRadius_2 - EndVector[1] * EndVector[1]);

		// (a*b)=|a||b|cos(a,b)
		m_CosRotAngle = (StartVector[0] * EndVector[0] + StartVector[1] * EndVector[1]) /
						CylinderRadius_2;
		m_SinRotAngle = sqrt(1.0 - m_CosRotAngle * m_CosRotAngle);
		if(ptLatest.y < m_ptLast.y)
			m_SinRotAngle = - m_SinRotAngle;

		break;

	case Y:
		if(m_ptLast.x == ptLatest.x)
			return;

		CylinderRadius_2 = WinCenter.x * WinCenter.x;

		StartVector[1] = m_ptLast.x - WinCenter.x;
		StartVector[0] = sqrt(CylinderRadius_2 - StartVector[1] * StartVector[1]);
		EndVector[1] = ptLatest.x - WinCenter.x;
		EndVector[0] = sqrt(CylinderRadius_2 - EndVector[1] * EndVector[1]);

		// (a*b)=|a||b|cos(a,b)
		m_CosRotAngle = (StartVector[0] * EndVector[0] + StartVector[1] * EndVector[1]) /
						CylinderRadius_2;
		m_SinRotAngle = sqrt(1.0 - m_CosRotAngle * m_CosRotAngle);
		if(ptLatest.x > m_ptLast.x)
			m_SinRotAngle = - m_SinRotAngle;

		break;

	case Z:
		StartVector[0] = m_ptLast.x - WinCenter.x;
		StartVector[1] = m_ptLast.y - WinCenter.y;
		EndVector[0] = ptLatest.x - WinCenter.x;
		EndVector[1] = ptLatest.y - WinCenter.y;
		StartVectorLength = sqrt(StartVector[0] * StartVector[0] + StartVector[1] * StartVector[1]);
		EndVectorLength = sqrt(EndVector[0] * EndVector[0] + EndVector[1] * EndVector[1]);

		if(StartVectorLength < 5.0 || EndVectorLength < 5.0)	// some ugly empiricals
			return;

		// (a*b)=|a||b|cos(a,b)
		m_CosRotAngle = (StartVector[0] * EndVector[0] + StartVector[1] * EndVector[1]) /
						(StartVectorLength * EndVectorLength);

		if( (EndVector[0]) * (StartVector[1]) -
			(EndVector[1]) * (StartVector[0]) > 0 )
			m_RotAngle = acos(m_CosRotAngle);
		else
			m_RotAngle = - acos(m_CosRotAngle);
	}
}


//
// CRealTimeRotate_Sphere
//
CRealTimeRotate_Sphere::CRealTimeRotate_Sphere(CIcadView* pView, POINT ptStart) : CRealTime(pView, ptStart)
{
	m_iPrevCursorMode = SDS_SetCursor(IDC_RT_SPHERE);
	m_Motion = ROT_SPH;

	for(int i = 0; i < 4; i++)
		for(int j = 0; j < 4; j++)
			m_RotationMatrix[i][j] = m_RotationMatrixInv[i][j] = 0.0;
	for(i = 0; i < 4; i++)
		m_RotationMatrix[i][i] = m_RotationMatrixInv[i][i] = 1.0;
}

bool
CRealTimeRotate_Sphere::Continue(POINT ptLatest)
{
	if(CRealTime::Continue(ptLatest))
	{
		resbuf		rb, rbwcs, rbucs, rbdcs;
		sds_point	ViewCtr, OldViewDir, NewViewDir, Pt;
		sds_real	ViewDirLen, ViewTwist;

		rbwcs.rbnext = rbucs.rbnext = rbdcs.rbnext = NULL;
		rbwcs.restype = rbucs.restype = rbdcs.restype = RTSHORT;
		rbwcs.resval.rint = 0;
		rbucs.resval.rint = 1;
		rbdcs.resval.rint = 2;

		SDS_getvar(NULL, DB_QVIEWMODE, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
		if(m_iViewMode)
		{
			SDS_getvar(NULL, DB_QTARGET, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
			ic_ptcpy(ViewCtr, rb.resval.rpoint);
		}
		else
		{
			SDS_getvar(NULL, DB_QVIEWCTR, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
			ic_ptcpy(ViewCtr, rb.resval.rpoint);
		}

		if(!m_bContinuousOrbitStarted)
		{
			CalculateRotation(ptLatest);
			m_ptLast.x = ptLatest.x;
			m_ptLast.y = ptLatest.y;
		}

		// viewdir
		NewViewDir[0] = NewViewDir[1] = 0.0; NewViewDir[2] = 1.0;
		ic_xformpt(NewViewDir, NewViewDir, m_RotationMatrix);

		sds_trans(NewViewDir, &rbdcs, &rbucs, 1, NewViewDir);
		if(fabs(NewViewDir[0]) < IC_ZRO)
			NewViewDir[0] = 0.0;
		if(fabs(NewViewDir[1]) < IC_ZRO)
			NewViewDir[1] = 0.0;
		if(fabs(NewViewDir[2]) < IC_ZRO)
			NewViewDir[2] = 0.0;

		SDS_getvar(NULL, DB_QVIEWDIR, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
		ic_ptcpy(OldViewDir, rb.resval.rpoint);
		ViewDirLen = sqrt(OldViewDir[0] * OldViewDir[0] +
						  OldViewDir[1] * OldViewDir[1] +
						  OldViewDir[2] * OldViewDir[2]);
		NewViewDir[0] *= ViewDirLen;
		NewViewDir[1] *= ViewDirLen;
		NewViewDir[2] *= ViewDirLen;

		// viewtwist
		Pt[0] = Pt[1] = 0.0; Pt[2] = 1.0;
		sds_trans(Pt, &rbwcs, &rbdcs, 1, Pt);
		ic_xformpt(Pt, Pt, m_RotationMatrixInv);

		if(fabs(Pt[0]) < IC_ZRO && fabs(Pt[1]) < IC_ZRO)
		{
			//if we'll now be looking parallel to the WCS Z axis, use Y axis for viewtwist
			SDS_getvar(NULL, DB_QUCSYDIR, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
			ic_ptcpy(Pt, rb.resval.rpoint);
			sds_trans(Pt, &rbwcs, &rbdcs, 1, Pt);
			ic_xformpt(Pt, Pt, m_RotationMatrixInv);
		}
		ViewTwist = atan2(Pt[1], Pt[0]);
		if(ViewTwist < (IC_PI / 2.0))
			ViewTwist += IC_TWOPI;
		ViewTwist -= IC_PI / 2.0;

		rb.restype = RT3DPOINT;
		ic_ptcpy(rb.resval.rpoint, NewViewDir);
		sdsengine_setvar("VIEWDIR"/*DNT*/, &rb);

		rb.restype = RTREAL;
		rb.resval.rreal = ViewTwist;
		sdsengine_setvar("VIEWTWIST"/*DNT*/, &rb);

		rb.restype = RT3DPOINT;
		ic_ptcpy(rb.resval.rpoint, ViewCtr);
		sdsengine_setvar("VIEWCTR"/*DNT*/, &rb);

		RedrawAfter();

		return true;
	}
	else
		return false;
}

void
CRealTimeRotate_Sphere::CalculateRotation(POINT ptLatest)
{
	POINT		WinCenter;
	sds_point	StartVector, EndVector, RotationAxis, ZeroPt;
	sds_real	SphereRadius_2, CosRotAngle;

	WinCenter.x = m_pView->m_iWinX / 2;
	WinCenter.y = m_pView->m_iWinY / 2;

	SphereRadius_2 = WinCenter.x * WinCenter.x + WinCenter.y * WinCenter.y;

	StartVector[0] = m_ptLast.x - WinCenter.x;
	StartVector[1] = - (m_ptLast.y - WinCenter.y);
	StartVector[2] = sqrt(SphereRadius_2 - StartVector[0] * StartVector[0] - StartVector[1] * StartVector[1]);

	EndVector[0] = ptLatest.x - WinCenter.x;
	EndVector[1] = - (ptLatest.y - WinCenter.y);
	EndVector[2] = sqrt(SphereRadius_2 - EndVector[0] * EndVector[0] - EndVector[1] * EndVector[1]);

	// (a*b)=|a||b|cos(a,b)
	CosRotAngle = (StartVector[0] * EndVector[0] + StartVector[1] * EndVector[1] + StartVector[2] * EndVector[2]) /
					SphereRadius_2;

	RotationAxis[0] = StartVector[1] * EndVector[2] - StartVector[2] * EndVector[1];
	RotationAxis[1] = StartVector[2] * EndVector[0] - StartVector[0] * EndVector[2];
	RotationAxis[2] = StartVector[0] * EndVector[1] - StartVector[1] * EndVector[0];

	ZeroPt[0] = ZeroPt[1] = ZeroPt[2] = 0.0;

	ic_rotxform(ZeroPt, RotationAxis, - acos(CosRotAngle), m_RotationMatrix);
	ic_rotxform(ZeroPt, RotationAxis, acos(CosRotAngle), m_RotationMatrixInv);
}
