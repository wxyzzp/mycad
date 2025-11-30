/* D:\DEV\PRJ\ICAD\ICADRECTTRACKER.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

//	IcadRectTracker.cpp
//	Copyright (C) 1994,1995,1996,1997 by Visio Corporation.
//  Code copied from MFC source was copied from DOCKCONT.CPP 10/30/96 12:41 PM
//

//** Includes
#include "Icad.h"/*DNT*/
#include "IcadApi.h"
#include "IcadRectTracker.h"/*DNT*/


/////////////////////////////////////////////////////////////////////////////
// CIcadRectTracker

CIcadRectTracker::CIcadRectTracker()
{
	//*** Create the pens
	m_pSolidPen = new CPen(PS_SOLID,0,SDS_PenColorFromACADColor(-1));
	m_pDottedPen = new CPen(PS_DOT,0,SDS_PenColorFromACADColor(256));

	// create the hatch pattern + bitmap
	m_hatchBrush=NULL;
	WORD hatchPattern[8];
	WORD wPattern = 0x1111;
	for(int i=0; i<4; i++)
	{
		hatchPattern[i] = wPattern;
		hatchPattern[i+4] = wPattern;
		wPattern <<= 1;
	}
	HBITMAP hatchBitmap = CreateBitmap(8, 8, 1, 1, &hatchPattern);
	if(hatchBitmap!=NULL)
	{
		// create black hatched brush
		m_hatchBrush = CreatePatternBrush(hatchBitmap);
		DeleteObject(hatchBitmap);
	}
}

CIcadRectTracker::~CIcadRectTracker()
{
	//*** Delete the pens
	if(NULL!=m_pSolidPen)
	{
		delete m_pSolidPen;
		m_pSolidPen=NULL;
	}
	if(NULL!=m_pDottedPen)
	{
		delete m_pDottedPen;
		m_pDottedPen=NULL;
	}
	if(NULL!=m_hatchBrush)
	{
		::DeleteObject(m_hatchBrush);
		m_hatchBrush=NULL;
	}
}

void CIcadRectTracker::Draw(CDC* pDC)
{
//*** Most of this function was copied from MFC source TRCKRECT.CPP.

	// set initial DC state
	VERIFY(pDC->SaveDC() != 0);
	pDC->SetMapMode(MM_TEXT);
	pDC->SetViewportOrg(0, 0);
	pDC->SetWindowOrg(0, 0);

	// get normalized rectangle
	CRect rect = m_rect;
	rect.NormalizeRect();

	CPen* pOldPen = NULL;
	CBrush* pOldBrush = NULL;
	CGdiObject* pTemp;
	int nOldROP;

	// draw lines
	if ((m_nStyle & (dottedLine|solidLine)) != 0)
	{
		if (m_nStyle & dottedLine)
		{
			pOldPen = pDC->SelectObject(m_pDottedPen);
		}
		else
		{
			pOldPen = pDC->SelectObject(m_pSolidPen);
		}
		pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
		nOldROP = pDC->SetROP2(R2_COPYPEN);
		rect.InflateRect(+1, +1);   // borders are one pixel outside
		pDC->Rectangle(rect.left, rect.top, rect.right, rect.bottom);
		pDC->SetROP2(nOldROP);
	}

	// if m_hatchBrush is going to be used, need to unrealize it
	if ((m_nStyle & (hatchInside|hatchedBorder)) != 0)
		UnrealizeObject(m_hatchBrush);

	// hatch inside
	if ((m_nStyle & hatchInside) != 0)
	{
		pTemp = pDC->SelectStockObject(NULL_PEN);
		if (pOldPen == NULL)
			pOldPen = (CPen*)pTemp;
		pTemp = pDC->SelectObject(CBrush::FromHandle(m_hatchBrush));
		if (pOldBrush == NULL)
			pOldBrush = (CBrush*)pTemp;
		pDC->SetBkMode(TRANSPARENT);
#ifndef _MAC
		nOldROP = pDC->SetROP2(R2_MASKNOTPEN);
#else _MAC
		nOldROP = pDC->SetROP2(R2_MERGENOTPEN);
#endif // _MAC
		pDC->Rectangle(rect.left+1, rect.top+1, rect.right, rect.bottom);
		pDC->SetROP2(nOldROP);
	}

	// draw hatched border
	if ((m_nStyle & hatchedBorder) != 0)
	{
		pTemp = pDC->SelectObject(CBrush::FromHandle(m_hatchBrush));
		if (pOldBrush == NULL)
			pOldBrush = (CBrush*)pTemp;
		pDC->SetBkMode(OPAQUE);
		CRect rectTrue;
		GetTrueRect(&rectTrue);
		pDC->PatBlt(rectTrue.left, rectTrue.top, rectTrue.Width(),
			rect.top-rectTrue.top, 0x000F0001 /* Pn */);
		pDC->PatBlt(rectTrue.left, rect.bottom,
			rectTrue.Width(), rectTrue.bottom-rect.bottom, 0x000F0001 /* Pn */);
		pDC->PatBlt(rectTrue.left, rect.top, rect.left-rectTrue.left,
			rect.Height(), 0x000F0001 /* Pn */);
		pDC->PatBlt(rect.right, rect.top, rectTrue.right-rect.right,
			rect.Height(), 0x000F0001 /* Pn */);
	}

	// draw resize handles
	if ((m_nStyle & (resizeInside|resizeOutside)) != 0)
	{
		UINT mask = GetHandleMask();
		for (int i = 0; i < 8; ++i)
		{
			if (mask & (1<<i))
			{
				GetHandleRect((TrackerHit)i, &rect);
				struct sds_resbuf rb;
				SDS_getvar(NULL,DB_QGRIPCOLOR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				pDC->FillSolidRect(rect,SDS_BrushColorFromACADColor(rb.resval.rint));
			}
		}
	}

	// cleanup pDC state
	if (pOldPen != NULL)
		pDC->SelectObject(pOldPen);
	if (pOldBrush != NULL)
		pDC->SelectObject(pOldBrush);
	VERIFY(pDC->RestoreDC(-1));
}



