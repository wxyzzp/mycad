/* 
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Misc functions that help with options dialog
 * 
 */ 

#include "icad.h"
#include "IcadApi.h"
#include "optionsfuncts.h"


const SquareSize = 10;

// Paints a color swatch on the left side of a color button
// If checkBackgrd it will paint the color specified no matter what the background is.
// If checkBackgrd is false the XORed color will be used instead when the background and color are the same.
void PaintColorSwatch (CWnd *window, int id, int color, bool checkBackgrd)
{
	CRect rect;
	CBrush brush;
    CWnd *hItemWnd = window->GetDlgItem(id);
    CDC *buttonDC = hItemWnd->GetDC();
	CPalette *oldpal= buttonDC->SelectPalette(SDS_CMainWindow->m_pPalette,TRUE);

	buttonDC->RealizePalette();

	resbuf rb;
	SDS_getvar(NULL, DB_QBKGCOLOR, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);

	if (checkBackgrd && (rb.resval.rint == color))
	{
		brush.CreateSolidBrush(SDS_BrushColorFromACADColor(256));
	}
	else
	{
		if (color == 256)
			brush.CreateSolidBrush(RGB(0,0,0));

		else
			brush.CreateSolidBrush(SDS_BrushColorFromACADColor(color));
	}

    hItemWnd->GetClientRect(&rect);

	int vertDist = (rect.bottom - rect.top - SquareSize) / 2;

    rect.left += 6;
	rect.top += vertDist;
	rect.bottom -= vertDist;
	rect.right = rect.left + SquareSize;
	
	buttonDC->FillRect(&rect, &brush);
 	brush.DeleteObject();
	brush.CreateSolidBrush(RGB(0,0,0));
	buttonDC->FrameRect(&rect, &brush);
	brush.DeleteObject();

//	buttonDC->SelectPalette(oldpal, TRUE);
	hItemWnd->ReleaseDC(buttonDC);
}

//Draws a color button with a color swatch
//Doesn't handle disabled buttons since this feature is not needed currently.
// If checkBackgrd is true it will paint the color specified no matter what the background is.
// If checkBackgrd is false the XORed color will be used when the background and color are the same.
void DrawColorButton (LPDRAWITEMSTRUCT lpDrawItemStruct, CWnd *window, int id, int color, bool checkBackgrd)
{
	ASSERT(lpDrawItemStruct != NULL);		
	
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);	
	CRect rect, rect2, focusRect;

	// get the rectangle portion
	rect.CopyRect(&lpDrawItemStruct->rcItem);
	rect2.CopyRect(rect);
	
	rect2.left += 1;
	rect2.top += 1;
	rect2.bottom -= 1;
	rect2.right -= 1;

	focusRect.left = rect.left + 3;
	focusRect.top = rect.top + 3;
	focusRect.bottom = rect.bottom - 3;
	focusRect.right = rect.right - 3;
	
	// draw the up/down/focused/disabled state	
	UINT state = lpDrawItemStruct->itemState;    
	
	if (state & ODS_SELECTED) 
	{		
		// draw the down edges
		pDC->Draw3dRect (rect, GetSysColor(COLOR_3DDKSHADOW), GetSysColor(COLOR_3DHILIGHT));
		pDC->Draw3dRect (rect2, GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_3DHILIGHT));
	}	
	else
	{	
		// draw the up edges		
		pDC->Draw3dRect (rect, GetSysColor(COLOR_3DHILIGHT), GetSysColor(COLOR_3DDKSHADOW));    
		pDC->Draw3dRect (rect2, GetSysColor(COLOR_3DHILIGHT), GetSysColor(COLOR_3DSHADOW));    
	}

	if (lpDrawItemStruct->itemAction & ODA_FOCUS)
	{
		pDC->DrawFocusRect(focusRect);
	}

	// add the text
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(0, 0, 0));
	pDC->TextOut(rect.left + 10 + SquareSize, rect.top + 5, ResourceString(IDC_ICAD_COLORBUTTON, "Color..."));

	PaintColorSwatch(window, id, color, checkBackgrd);
}




