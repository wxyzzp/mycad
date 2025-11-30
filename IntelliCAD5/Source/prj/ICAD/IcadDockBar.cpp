/* D:\DEV\PRJ\ICAD\ICADDOCKBAR.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

//	IcadDockBar.cpp
//	Copyright (C) 1994,1995,1996,1997 by Visio Corporation.
//  Code copied from MFC source was copied from BARDOCK.CPP 10/30/96 12:41 PM
//

//** Includes
#include "Icad.h"/*DNT*/
#include "IcadDockBar.h"/*DNT*/

/////////////////////////////////////////////////////////////////////////////
// CIcadDockBar

//*** This entire function was copied unchanged from MFC source just to override
//*** the call to Insert().
void CIcadDockBar::DockControlBar(CControlBar* pBar, LPCRECT lpRect)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pBar);
	ASSERT_KINDOF(CControlBar, pBar);

	CRect rectBar;
	pBar->GetWindowRect(&rectBar);
	if (pBar->m_pDockBar == this && (lpRect == NULL || rectBar == *lpRect))
	{
		// already docked and no change in position
		return;
	}

	// set CBRS_FLOAT_MULTI style if docking bar has it
	if (m_bFloating && (pBar->m_dwDockStyle & CBRS_FLOAT_MULTI))
		m_dwStyle |= CBRS_FLOAT_MULTI;

	m_dwStyle &= ~(CBRS_SIZE_FIXED | CBRS_SIZE_DYNAMIC);
	m_dwStyle |= pBar->m_dwStyle & (CBRS_SIZE_FIXED | CBRS_SIZE_DYNAMIC);

	if (!(m_dwStyle & CBRS_FLOAT_MULTI))
	{
		TCHAR szTitle[_MAX_PATH];
		pBar->GetWindowText(szTitle, _countof(szTitle));
		AfxSetWindowText(m_hWnd, szTitle);
	}

	// align correctly and turn on all borders
	DWORD dwStyle = pBar->GetBarStyle();
	dwStyle &= ~(CBRS_ALIGN_ANY);
	dwStyle |=  (m_dwStyle & CBRS_ALIGN_ANY) | CBRS_BORDER_ANY;

	if (m_bFloating)
		dwStyle |= CBRS_FLOATING;
	else
		dwStyle &= ~CBRS_FLOATING;

	pBar->SetBarStyle(dwStyle);

	// hide first if changing to a new docking site to avoid flashing
	BOOL bShow = FALSE;
	if (pBar->m_pDockBar != this && pBar->IsWindowVisible())
	{
		pBar->SetWindowPos(NULL, 0, 0, 0, 0,
			SWP_NOSIZE|SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_HIDEWINDOW);
		bShow = TRUE;
	}

	int nPos = -1;
	if (lpRect != NULL)
	{
		// insert into appropriate row
		CRect rect(lpRect);
		ScreenToClient(&rect);
		CPoint ptMid(rect.left + rect.Width()/2, rect.top + rect.Height()/2);
		nPos = Insert(pBar, rect, ptMid);

		// position at requested position
		pBar->SetWindowPos(NULL, rect.left, rect.top, rect.Width(),
			rect.Height(), SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOCOPYBITS);
	}
	else
	{
		// always add on current row, then create new one
		m_arrBars.Add(pBar);
		m_arrBars.Add(NULL);

		// align off the edge initially
		pBar->SetWindowPos(NULL, -afxData.cxBorder2, -afxData.cyBorder2, 0, 0,
			SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOCOPYBITS);
	}

	// attach it to the docking site
	if (pBar->GetParent() != this)
		pBar->SetParent(this);
	if (pBar->m_pDockBar == this)
		pBar->m_pDockBar->RemoveControlBar(pBar, nPos);
	else if (pBar->m_pDockBar != NULL)
		pBar->m_pDockBar->RemoveControlBar(pBar, -1, m_bFloating && !pBar->m_pDockBar->m_bFloating);
	pBar->m_pDockBar = this;

	if (bShow)
	{
		ASSERT(!pBar->IsWindowVisible());
		pBar->SetWindowPos(NULL, 0, 0, 0, 0,
			SWP_NOSIZE|SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_SHOWWINDOW);
	}

	// remove any place holder for pBar in this dockbar
	RemovePlaceHolder(pBar);

	// get parent frame for recalc layout
	CFrameWnd* pFrameWnd = GetDockingFrame();
	pFrameWnd->DelayRecalcLayout();
}

//*** This function was copied from MFC source, see comments below for changes.
int CIcadDockBar::Insert(CControlBar* pBarIns, CRect rect, CPoint ptMid)
{
	ASSERT_VALID(this);
	ASSERT(pBarIns != NULL);

	int nPos = 0;
	int nPosInsAfter = 0;
	int nWidth = 0;
	int nTotalWidth = 0;
	BOOL bHorz = m_dwStyle & CBRS_ORIENT_HORZ ? TRUE : FALSE;

	BOOL bFoundVis = FALSE;
	for (nPos = 0; nPos < m_arrBars.GetSize(); nPos++)
	{
		CControlBar* pBar = GetDockedControlBar(nPos);
		if (pBar != NULL)
		{
			if(pBar->IsVisible())
			{
				CRect rectBar;
				pBar->GetWindowRect(&rectBar);
				ScreenToClient(&rectBar);
				nWidth = max(nWidth,
					bHorz ? rectBar.Size().cy : rectBar.Size().cx - 1);
				if (bHorz ? rect.left > rectBar.left : rect.top > rectBar.top)
					nPosInsAfter = nPos;
				bFoundVis=TRUE;
			}
		}
		if(pBar==NULL || !pBar->IsVisible())
//		else // end of row because pBar == NULL
		{
			//*** This block of code is the reason for the files IcadDockBar
			//*** and IcadDockContext.  This fixes the docking bug.
			//*** See BARDOCK.CPP, 10/30/96 12:41 PM, line 704
//			if(NULL==pBar)
			if(bFoundVis==TRUE)
			{
				nTotalWidth += nWidth - afxData.cyBorder2;
			}
			else
			{
				nTotalWidth += nWidth;
			}
			bFoundVis=FALSE;
			//***
			nWidth = 0;
			if ((bHorz ? ptMid.y : ptMid.x) < nTotalWidth)
			{
				if (nPos == 0) // first section
					m_arrBars.InsertAt(nPosInsAfter+1, (CObject*)NULL);
				m_arrBars.InsertAt(nPosInsAfter+1, pBarIns);
				return nPosInsAfter+1;
			}
			nPosInsAfter = nPos;
		}
	}

	// create a new row
	m_arrBars.InsertAt(nPosInsAfter+1, (CObject*)NULL);
	m_arrBars.InsertAt(nPosInsAfter+1, pBarIns);

	return nPosInsAfter+1;
}

//*** For Office 97 toolbar look.
CSize CIcadDockBar::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
	ASSERT_VALID(this);

	CSize sizeFixed = CControlBar::CalcFixedLayout(bStretch, bHorz);

	// get max size
	CSize sizeMax;
	if (!m_rectLayout.IsRectEmpty())
		sizeMax = m_rectLayout.Size();
	else
	{
		CRect rectFrame;
		CFrameWnd* pFrame = GetParentFrame();
		pFrame->GetClientRect(&rectFrame);
		sizeMax = rectFrame.Size();
	}

	// prepare for layout
	AFX_SIZEPARENTPARAMS layout;
	layout.hDWP = m_bLayoutQuery ?
		NULL : ::BeginDeferWindowPos(m_arrBars.GetSize());
	CPoint pt(-afxData.cxBorder2, -afxData.cyBorder2);
	int nWidth = 0;

	BOOL bWrapped = FALSE;

#ifdef ICAD_OLDTOOLBARS
	int cxBorder = afxData.cxBorder2, cyBorder = afxData.cyBorder2;
#else
	int cxBorder = 0, cyBorder = 0;
#endif
	// layout all the control bars
	for (int nPos = 0; nPos < m_arrBars.GetSize(); nPos++)
	{
		CControlBar* pBar = GetDockedControlBar(nPos);
		void* pVoid = m_arrBars[nPos];

		if (pBar != NULL)
		{
			if (pBar->IsVisible())
			{
				// get ideal rect for bar
				DWORD dwMode = 0;
				if ((pBar->m_dwStyle & CBRS_SIZE_DYNAMIC) &&
					(pBar->m_dwStyle & CBRS_FLOATING))
					dwMode |= LM_HORZ | LM_MRUWIDTH;
				else if (pBar->m_dwStyle & CBRS_ORIENT_HORZ)
					dwMode |= LM_HORZ | LM_HORZDOCK;
				else
					dwMode |=  LM_VERTDOCK;

				CSize sizeBar = pBar->CalcDynamicLayout(-1, dwMode);

				CRect rect(pt, sizeBar);

				// get current rect for bar
				CRect rectBar;
				pBar->GetWindowRect(&rectBar);
				ScreenToClient(&rectBar);

				if (bHorz)
				{
					// Offset Calculated Rect out to Actual
					if (rectBar.left > rect.left && !m_bFloating)
						rect.OffsetRect(rectBar.left - rect.left, 0);

					// If ControlBar goes off the right, then right justify
					if (rect.right > sizeMax.cx && !m_bFloating)
					{
						int x = rect.Width() - afxData.cxBorder2;
						x = max(sizeMax.cx - x, pt.x);
						rect.OffsetRect(x - rect.left, 0);
					}

					// If ControlBar has been wrapped, then left justify
					if (bWrapped)
					{
						bWrapped = FALSE;
						rect.OffsetRect(-(rect.left + afxData.cxBorder2), 0);
					}
					// If ControlBar is completely invisible, then wrap it
					else if ((rect.left >= (sizeMax.cx - afxData.cxBorder2)) &&
						(nPos > 0) && (m_arrBars[nPos - 1] != NULL))
					{
						m_arrBars.InsertAt(nPos, (CObject*)NULL);
						pBar = NULL; pVoid = NULL;
						bWrapped = TRUE;
					}
					if (!bWrapped)
					{
						if (rect != rectBar)
						{
							if (!m_bLayoutQuery &&
								!(pBar->m_dwStyle & CBRS_FLOATING))
							{
								pBar->m_pDockContext->m_rectMRUDockPos = rect;
							}
							AfxRepositionWindow(&layout, pBar->m_hWnd, &rect);
						}
//						pt.x = rect.left + sizeBar.cx - afxData.cxBorder2;
						pt.x = rect.left + sizeBar.cx - cxBorder;
						nWidth = max(nWidth, sizeBar.cy);
					}
				}
				else
				{
					// Offset Calculated Rect out to Actual
					if (rectBar.top > rect.top && !m_bFloating)
						rect.OffsetRect(0, rectBar.top - rect.top);

					// If ControlBar goes off the bottom, then bottom justify
					if (rect.bottom > sizeMax.cy && !m_bFloating)
					{
						int y = rect.Height() - afxData.cyBorder2;
						y = max(sizeMax.cy - y, pt.y);
						rect.OffsetRect(0, y - rect.top);
					}

					// If ControlBar has been wrapped, then top justify
					if (bWrapped)
					{
						bWrapped = FALSE;
						rect.OffsetRect(0, -(rect.top + afxData.cyBorder2));
					}
					// If ControlBar is completely invisible, then wrap it
					else if ((rect.top >= (sizeMax.cy - afxData.cyBorder2)) &&
						(nPos > 0) && (m_arrBars[nPos - 1] != NULL))
					{
						m_arrBars.InsertAt(nPos, (CObject*)NULL);
						pBar = NULL; pVoid = NULL;
						bWrapped = TRUE;
					}
					if (!bWrapped)
					{
						if (rect != rectBar)
						{
							if (!m_bLayoutQuery &&
								!(pBar->m_dwStyle & CBRS_FLOATING))
							{
								pBar->m_pDockContext->m_rectMRUDockPos = rect;
							}
							AfxRepositionWindow(&layout, pBar->m_hWnd, &rect);
						}
//						pt.y = rect.top + sizeBar.cy - afxData.cyBorder2;
						pt.y = rect.top + sizeBar.cy - cyBorder;
						nWidth = max(nWidth, sizeBar.cx);
					}
				}
			}
			if (!bWrapped)
			{
				// handle any delay/show hide for the bar
				pBar->RecalcDelayShow(&layout);
			}
		}
		if (pBar == NULL && pVoid == NULL && nWidth != 0)
		{
			// end of row because pBar == NULL
			if (bHorz)
			{
//				pt.y += nWidth - afxData.cyBorder2;
				pt.y += nWidth - cyBorder;
				sizeFixed.cx = max(sizeFixed.cx, pt.x);
				sizeFixed.cy = max(sizeFixed.cy, pt.y);
				pt.x = -afxData.cxBorder2;
			}
			else
			{
//				pt.x += nWidth - afxData.cxBorder2;
				pt.x += nWidth - cxBorder;
				sizeFixed.cx = max(sizeFixed.cx, pt.x);
				sizeFixed.cy = max(sizeFixed.cy, pt.y);
				pt.y = -afxData.cyBorder2;
			}
			nWidth = 0;
		}
	}
#ifndef ICAD_OLDTOOLBARS
	//*** Remove one pixel from the bottom or right so there isn't a double border.
	if(bHorz)
	{
		if(sizeFixed.cy>0)
		{
			sizeFixed.cy--;
		}
	}
	else
	{
		if(sizeFixed.cx>0)
		{
			sizeFixed.cx--;
		}
	}
#endif
	if (!m_bLayoutQuery)
	{
		// move and resize all the windows at once!
		if (layout.hDWP == NULL || !::EndDeferWindowPos(layout.hDWP))
			TRACE0(ResourceString(IDC_ICADDOCKBAR_WARNING__DEF_0, "Warning: DeferWindowPos failed - low system resources.\n" ));
	}

	// adjust size for borders on the dock bar itself
	CRect rect;
	rect.SetRectEmpty();
	CalcInsideRect(rect, bHorz);

	if ((!bStretch || !bHorz) && sizeFixed.cx != 0)
		sizeFixed.cx += -rect.right + rect.left;
	if ((!bStretch || bHorz) && sizeFixed.cy != 0)
		sizeFixed.cy += -rect.bottom + rect.top;

	return sizeFixed;
}



