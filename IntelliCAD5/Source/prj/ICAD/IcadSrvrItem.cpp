/* D:\DEV\PRJ\ICAD\ICADSRVRITEM.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

// IcadSrvrItem.cpp : implementation of the CIcadSrvrItem class
//

#include "Icad.h"/*DNT*/
#include "IcadDoc.h"
#include "IcadSrvrItem.h"/*DNT*/
#include "IcadCntrItem.h"/*DNT*/
#include "IcadView.h"

#ifdef _DEBUG

	#if !defined(USE_SMARTHEAP)
	#define new DEBUG_NEW
	#endif

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIcadSrvrItem implementation

IMPLEMENT_DYNAMIC(CIcadSrvrItem, COleServerItem)

CIcadSrvrItem::CIcadSrvrItem(CIcadDoc* pContainerDoc)
	: COleServerItem(pContainerDoc, TRUE)
{
	//*** Initialize the size of the embed.
	m_pView=SDS_CURVIEW;
	//save extents in pixels - not HIMETRIC
	m_sizeExtent.cx = 500;
	m_sizeExtent.cy = 300;
}

CIcadSrvrItem::~CIcadSrvrItem()
{
}

void CIcadSrvrItem::Serialize(CArchive& ar)
{
	// CIcadSrvrItem::Serialize will be called by the framework if
	//  the item is copied to the clipboard.  This can happen automatically
	//  through the OLE callback OnGetClipboardData.  A good default for
	//  the embedded item is simply to delegate to the document's Serialize
	//  function.  If you support links, then you will want to serialize
	//  just a portion of the document.

	if (!IsLinkedItem())
	{
		CIcadDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		pDoc->Serialize(ar);
	}
}

BOOL CIcadSrvrItem::OnSetExtent(DVASPECT dwDrawAspect, const CSize& size)
{
	ASSERT_VALID(this);

	if(dwDrawAspect==DVASPECT_CONTENT)
	{
		m_sizeExtent = size;    // simply remember the extent
		CClientDC dcClient(NULL);
		dcClient.HIMETRICtoDP(&m_sizeExtent);
		return TRUE;
	}
	return FALSE;   // not implemented for that dwDrawAspect
}

BOOL CIcadSrvrItem::OnGetExtent(DVASPECT dwDrawAspect, CSize& rSize)
{
	// Most applications, like this one, only handle drawing the content
	//  aspect of the item.  If you wish to support other aspects, such
	//  as DVASPECT_THUMBNAIL (by overriding OnDrawEx), then this
	//  implementation of OnGetExtent should be modified to handle the
	//  additional aspect(s).

	if (dwDrawAspect != DVASPECT_CONTENT)
		return COleServerItem::OnGetExtent(dwDrawAspect, rSize);

	CClientDC dcClient(NULL);
	//*** Convert the size to HIMETRIC units.
	CIcadDoc* pDoc = (CIcadDoc*)GetDocument();
	if(NULL!=pDoc)
	{
		rSize=m_sizeExtent;
		dcClient.DPtoHIMETRIC(&rSize);
	}

	return TRUE;
}

BOOL CIcadSrvrItem::OnDraw(CDC* pDC, CSize& rSize)
{
	CIcadDoc* pDoc = (CIcadDoc*)GetDocument();
	if(NULL==pDoc)
	{
		return FALSE;
	}
	CIcadView *pView = m_pView;
	if(NULL==pView)
	{
		return FALSE;
	}

	if (pDoc->m_bInIPResize) return TRUE;

	//*** Set the size of the metafile.
	CRect rectMetaFile(0,0,m_sizeExtent.cx,m_sizeExtent.cy);

	//*** While we draw the metafile, temporarily resize the view to the desired
	//*** size of the metafile.
	CRect rectWnd,rectClient;
	pView->GetClientRect(rectClient);
	pView->GetWindowRect(rectWnd);
	int cx = (m_sizeExtent.cx + (rectWnd.Width()-rectClient.Width()));
	int cy = (m_sizeExtent.cy + (rectWnd.Height()-rectClient.Height()));
	pView->SetWindowPos(NULL,0,0,cx,cy,
		SWP_HIDEWINDOW | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE);

	//*** Prepare the view for drawing (Similar to CIcadView::OnPaint()).
	pView->UpdateScrollBars();
	struct resbuf tilemode;
	struct resbuf cvport;
	SDS_getvar(NULL,DB_QCVPORT,&cvport,pDoc->m_dbDrawing,&SDS_CURCFG,&SDS_CURSES);
	SDS_getvar(NULL,DB_QTILEMODE,&tilemode,pDoc->m_dbDrawing,&SDS_CURCFG,&SDS_CURSES);
	if(tilemode.resval.rint==0 && cvport.resval.rint>1 &&
		pView->m_pVportTabHip->ret_type()==DB_VIEWPORT)
	{
		SDS_VPeedToView(pDoc->m_dbDrawing,pView->m_pVportTabHip,&pView->m_pCurDwgView,pView);
		SDS_SetClipInDC(pView,pView->m_pVportTabHip,FALSE);
	} else {
		struct gr_viewvars viewvarsp;
		SDS_ViewToVars2Use(pView,pDoc->m_dbDrawing,&viewvarsp);
		gr_initview(pDoc->m_dbDrawing,&SDS_CURCFG,&pView->m_pCurDwgView,&viewvarsp);
	}

	// Set mapping mode and extent
	//  (The extent is usually the same as the size returned from OnGetExtent)
	CPoint point(0,0);
	pDC->SetMapMode(MM_ANISOTROPIC);
	pDC->SetWindowOrg(point);
	pDC->SetWindowExt(m_sizeExtent);
	::SetBkMode(pDC->m_hDC,TRANSPARENT);

	SDS_CMainWindow->m_bMapToRGBColor=TRUE;
	SDS_CMainWindow->m_bPrintPreview=TRUE;

	// Fill metafile rect with brush to keep background color in object representation
	// current background color of DWG file
	CBrush brush;
	brush.CreateSolidBrush(SDS_BrushColorFromACADColor(256));
	pDC->FillRect( &rectMetaFile, &brush );
	brush.DeleteObject();

	//Switch the background to white temporarily - this will enable correct creation of metafiles etc.
	//Before switching the background, also temporarily turn off the WNDLMDI variable - to prevent a redraw to the screen
	resbuf rbmdi;
	SDS_getvar(NULL,DB_QWNDLMDI,&rbmdi,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	int oldLmdi = rbmdi.resval.rint;
	rbmdi.resval.rint=0;
	SDS_setvar(NULL,DB_QWNDLMDI,&rbmdi,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);

//	resbuf rb;
//	sds_getvar("BKGCOLOR"/*DNT*/,&rb);
//	int oldBackColor = rb.resval.rint;
//	rb.resval.rint = 7;
//	sds_setvar("BKGCOLOR"/*DNT*/,&rb);

	//*** Prepare the device context for SDS_DrawEntity().  Select a
	//*** temporary pen and brush into the device context because
	//*** SDS_DrawEntity() deletes these in the device context.
	HPEN hTmpPen = ::CreatePen(PS_SOLID,1,RGB(0,0,0));
	HPEN hOldPen = (HPEN)::SelectObject(pDC->m_hDC,hTmpPen);
	HBRUSH hTmpBrush = ::CreateSolidBrush(SDS_BrushColorFromACADColor(256));
	HBRUSH hOldBrush = (HBRUSH)::SelectObject(pDC->m_hDC,hTmpBrush);
	//*** Draw all entities.
	db_handitem* hip = pDoc->m_dbDrawing->entnext_noxref(NULL);
	pView->m_LastCol=pView->m_LastHl=pView->m_LastRop=pView->m_LastWid=(-2);
	pView->m_FillLastCol=pView->m_FillLastHl=pView->m_FillLastRop=(-2);
	while(hip!=NULL)
	{
		//*** Draw in the metafile
		SDS_DrawEntity(hip,pDC,pDoc->m_dbDrawing,pView);
		//*** Next entity
		hip=pDoc->m_dbDrawing->entnext_noxref(hip);
	}
	//*** Select the original pen and brush back in and delete the pen and brush
	//*** that SDS_DrawEntity() was using last.
	::DeleteObject(SelectObject(pDC->m_hDC,hOldPen));
	::DeleteObject(SelectObject(pDC->m_hDC,hOldBrush));
	SDS_CMainWindow->m_bMapToRGBColor=FALSE;
	SDS_CMainWindow->m_bPrintPreview=FALSE;
	pView->m_LastCol=pView->m_LastHl=pView->m_LastRop=pView->m_LastWid=(-2);
	pView->m_FillLastCol=pView->m_FillLastHl=pView->m_FillLastRop=(-2);

	//reset the WNDLMDI to its original value
	rbmdi.resval.rint=oldLmdi;
	SDS_setvar(NULL,DB_QWNDLMDI,&rbmdi,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);

	//reset the Background color to its original value
//	rb.resval.rint = oldBackColor;
//	sds_setvar("BKGCOLOR"/*DNT*/,&rb);

	if(NULL!=pView)
	{
		//*** Restore the window size.
		pView->SetWindowPos(NULL,0,0,rectWnd.Width(),rectWnd.Height(),
			SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE);
	}

	return TRUE;
}

void CIcadSrvrItem::OnOpen()
{
	COleServerItem::OnOpen();
	return;
}

void CIcadSrvrItem::OnSaveEmbedding(LPSTORAGE lpStorage)
{
	CIcadDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	//*** Delegate to the documents SaveEmbedding, I guess.
	pDoc->OnSaveEmbedding(lpStorage);
	return;
}

/////////////////////////////////////////////////////////////////////////////
// CIcadSrvrItem diagnostics

#ifdef _DEBUG
void CIcadSrvrItem::AssertValid() const
{
	COleServerItem::AssertValid();
}

void CIcadSrvrItem::Dump(CDumpContext& dc) const
{
	COleServerItem::Dump(dc);
}
#endif

/////////////////////////////////////////////////////////////////////////////


