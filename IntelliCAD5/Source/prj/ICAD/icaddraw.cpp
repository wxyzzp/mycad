/* C:\DEV\PRJ\ICAD\ICADDRAW.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Moved out of ICADAPI.CPP functions having to do with drawing entities
 * 
 */ 
// THIS FILE HAS BEEN GLOBALIZED!

// ** Includes

#include "Icad.h"/*DNT*/
#include "IcadDoc.h"
#include "DDPlaneBuffer.h"/*DNT*/
#include "IcadEntityRenderer.h"/*DNT*/
#include "IcadRaster.h"/*DNT*/
#include "IcadPrintDia.h"/*DNT*/
#include "lisp.h"/*DNT*/
#include "cmds.h"/*DNT*/
#include "vxtabrec.h"
#include "IcadView.h"
#include <fstream.h>
#if defined(USE_SMARTHEAP)
	#include "smrtheap.h"/*DNT*/
#endif

extern bool  cmd_InsideRedrawAll;
extern bool  SDS_DontBitBlt;
// SystemMetrix(shiba)-[06.10.2001 
//   FIX: During command(zoom, regen etc) execution, moving mouse (wheel), crash.
bool	SDS_RedrawDrawing_Doing	= false;
// ]-SystemMetrix(shiba) 06.10.2001

void getViewPortClippingRectangle(CIcadView *pView, db_viewport *pViewport, gr_view *pgrView, gr_view *pgrViewVP, 
								  sds_point ptMin, sds_point ptMax)
{
	int x1,y1,x2,y2;
	// this function returns y1 and y2 backwards...??
	SDS_GetRectForVP( pView, pViewport, &x1, &x2, &y2, &y1 );
	
	// Intersect with the screen rectangle
	//
	// *******************************
	// This is kinda ugly, but it works
	//
	if ( x1 < 0 )
		x1 = 0;
	if ( x1 >= pView->m_iWinX )
		x1 = pView->m_iWinX - 1;
	if ( x2 < 0 )
		x2 = 0;
	if ( x2 >= pView->m_iWinX )
		x2 = pView->m_iWinX - 1;
	if ( y1 < 0 )
		y1 = 0;
	if ( y1 >= pView->m_iWinY )
		y1 = pView->m_iWinY - 1;
	if ( y2 < 0 )
		y2 = 0;
	if ( y2 >= pView->m_iWinY )
		y2 = pView->m_iWinY - 1;

	x1 += 1;
	y1 -= 1;
	x2 -= 1;
	y2 += 1;
	gr_pix2rp( pgrViewVP, x1, y1, &ptMin[0], &ptMin[1] );
	gr_pix2rp( pgrViewVP, x2, y2, &ptMax[0], &ptMax[1] );
	ptMin[2] = 0.0;
	ptMax[2] = 0.0;
}

// *********************************************
// PUBLIC FUNCTION
//
// SDS_DrawEntity
// DP: TODO: remove this function as obsolete
void SDS_DrawEntity(db_handitem* pEntity, CDC* pDC, db_drawing* pDrawing, CIcadView* pView)
{
	//CDrawDevice* pDrawDevice = ::CreateDCDrawDevice(cdc->GetSafeHdc());
	CDrawDevice* pDrawDevice = new CDDPlaneBuffer;
	pDrawDevice->create(pDC, CDrawDevice::eSINGLE_BUFFER);
	if(pDrawDevice != NULL)
	{
		pDrawDevice->beginDraw(pDC, pView->m_pCurDwgView);
		int drawBuffer = pDrawDevice->setDrawBuffer(CDrawDevice::eFRONT);
		// *********************************
		// TODO TODO TODO
		// if tilemode == 0
		// Should determine viewport for VPLAYER data

		// DP: using width 1 as was here leads to unexpected results (comparing with ACAD) when draw to metafile
		int width = 0;
		CIcadEntityRenderer renderer(pDrawing, pView);
		renderer.setDrawDevice(pDrawDevice);
        renderer.DrawEntity(pEntity, pView->m_pCurDwgView, NULL, NULL, 0, 0, 0, 0, 0, NULL, width);

		pDrawDevice->setDrawBuffer((CDrawDevice::EDrawBuffer)drawBuffer);
		pDrawDevice->endDraw();
		delete pDrawDevice;
	}
}

// ***************************************************
// PUBLIC FUNCTION
//
// SDS_RedrawEntity
//
// Main entry point into this module for drawing a single entity
//
int SDS_RedrawEntity(const sds_name ent, int mode, CIcadView* arg_pView, CIcadDoc* pDoc, int BitBlt2Scr)
{
	int width = 1;
	
	CIcadView* pView = arg_pView;
	if(pView == NULL) 
		pView = SDS_CURVIEW;
	if(pView == NULL)
		return(RTERROR);
	
	if(pDoc == NULL)
		pDoc = SDS_CURDOC;
	if(pDoc == NULL) 
		return(RTERROR);
	
	if(pView->m_pCurDwgView == NULL)
		return(RTERROR);
	pView->m_iViewMode = 0;
	
	if(ent[0] == 0L || ent[1] == 0L)
		return(RTERROR);
	ASSERT(pDoc->m_dbDrawing == (db_drawing*)ent[1]);
	
	if(SDS_CMainWindow->m_bPrintPreview || (SDS_CURVIEW && SDS_CURVIEW->IsPrinting())) 
	{
		CIcadEntityRenderer renderer(pDoc, SDS_CURVIEW);
		renderer.drawEntity((db_handitem*)ent[0], mode, !!BitBlt2Scr, width);
	}
	else
	{
		if(arg_pView != NULL)
		{
			CIcadEntityRenderer renderer(pDoc, arg_pView);
			renderer.drawEntity((db_handitem*)ent[0], mode, !!BitBlt2Scr, width);
		}
		else
		{
			CIcadEntityRenderer renderer(pDoc, NULL);
			renderer.drawEntity((db_handitem*)ent[0], mode, !!BitBlt2Scr, width);
		}
	}
	return(RTNORM);
}

// *************************************************
// PUBLIC FUNCTION
//
// SDS_RedrawDrawing
//
// Main entry point into this module for drawing an entire
// drawing
//
int SDS_RedrawDrawing(db_drawing* pDrawing, CIcadView* pView, CIcadDoc* pDoc, int BitBlt2Scr)
{
	if(pView == NULL) 
		pView = SDS_CURVIEW;
	if(pView == NULL)
		return(RTERROR);
	
	if(pDoc == NULL)
		pDoc = SDS_CURDOC;
	if(pDoc == NULL) 
		return(RTERROR);
	
	if(pView->m_pCurDwgView == NULL)
		return(RTERROR);

	if(pDrawing == NULL)
		pDrawing = SDS_CURDWG;
	if(pDrawing == NULL)
		return(RTERROR);
	
	// Build the regen list only once.
#ifdef _USE_DISP_OBJ_PTS_3D_
	const bool bRegen = (pDoc->m_RegenListView == NULL && ((BitBlt2Scr & 0x02) == 0));
#else
	const bool bRegen = (pDoc->m_RegenListView == NULL);
#endif
	
	if(pDoc->m_drawHelper.isEnable())
	{
		pDoc->m_drawHelper.add(pView, bRegen);
		return RTNORM;
	}

	pView->m_iViewMode = 0;

#ifdef BMP_USE_HIDESHADE_STATUS
	// clear dxf281 of all VIEWPORTs
	db_handitem	*pTabrec = NULL, *pViewport = NULL;
	// NOTE: If using get_tabrecllends() to walk through the table, we need to fillet out the records 
	// marked as deleted(). Otherwise, the viewports deleted by UNDO will be painted on the new layout
	// again so you will see the image flushed the new layout after ater UNDO a viewport in a layout and 
	// then switch to new layoout immediately.       SWH - 30/9/2004
	for(pTabrec=pDrawing->tblnext(DB_VXTAB,1); pTabrec!=NULL; pTabrec=SDS_CURDWG->tblnext(DB_VXTAB,0))
		if((pViewport = ((db_vxtabrec*)pTabrec)->ret_vpehip()) && pViewport->ret_type() == DB_VIEWPORT)
			((db_viewport*)pViewport)->set_281(IC_RENDERMODE_CLASSIC2D);	// TO DO: undo/redo of the change
#endif

	ASSERT(pDrawing == pDoc->m_dbDrawing);
	// SystemMetrix(shiba)-[06.10.2001 
	//   FIX: During command(zoom, regen etc) execution, moving mouse (wheel), crash.
	//
	//  Attention!! Don't return without SDS_RedrawDrawing_Doing:false
	// 
	SDS_RedrawDrawing_Doing	= true;
	// ]-SystemMetrix(shiba) 06.10.2001
	if(bRegen)
	{
		if(RTERROR == SDS_BuildRegenList(pDrawing, pView->m_pCurDwgView, pView, pDoc))
		{
			// SystemMetrix(shiba)-[06.10.2001 
			//   FIX: During command(zoom, regen etc) execution, moving mouse (wheel), crash.
			SDS_RedrawDrawing_Doing	= false;
			// ]-SystemMetrix(shiba) 06.10.2001
			return(RTERROR);
		}
	}
	SDS_SetRegenListFlag(pView, pDoc);

	//Now set scroll bar parameters for new view
	ExecuteUIAction(ICAD_WNDACTION_UDSBARS);
	SDS_CallUserCallbackFunc(SDS_CBVIEWCHANGE, (void*)1, NULL, NULL);
	
    CIcadEntityRenderer Renderer(pDrawing, pView);
	Renderer.DrawDrawing(0, NULL, NULL);
	// SystemMetrix(shiba)-[06.10.2001 
	//   FIX: During command(zoom, regen etc) execution, moving mouse (wheel), crash.
	SDS_RedrawDrawing_Doing	= false;
	// ]-SystemMetrix(shiba) 06.10.2001
	return RTNORM;
}

// CDrawHelper implementation
static bool findView(CIcadDoc* pDoc, CIcadView* pView)
{
	POSITION pos = pDoc->GetFirstViewPosition();
	while(pos)
	{
		if((CIcadView*)pDoc->GetNextView(pos) == pView)
			return true;
	}
	return false;
}

void CDrawHelper::process(CIcadDoc* pDoc)
{
	setEnable(false);
	for(MMap::iterator it = m_data.begin(); it != m_data.end(); ++it)
	{
		CIcadView* pView = it->first;
		if(!findView(pDoc, pView))
			continue;
		if(it->second.first) //regen
		{
			pDoc->m_RegenListView = NULL;
			SDS_RedrawDrawing(pDoc->m_dbDrawing, pView, pDoc, 1);
		}
		else if(it->second.second)
			SDS_RedrawDrawing(pDoc->m_dbDrawing, pView, pDoc, 0x03);
		it->second.first = it->second.second = 0; // in case of exception
	}
	reset();
}

void CDrawHelper::add(CIcadView* pView, bool bRegen)
{
	ASSERT(pView);
	MMap::iterator it = m_data.find(pView);
	if(it == m_data.end())
	{
		std::pair<int, int>& ref = m_data[pView];

		ref.first = bRegen ? 1 : 0;
		ref.second = bRegen ? 0 : 1;
	}
	else
	{
		if(bRegen) it->second.first++ ; else it->second.second++;
	}
}
//

