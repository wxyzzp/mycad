/* D:\DEV\PRJ\ICAD\ICADCUSBUTTON.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

#include "icad.h"/*DNT*/
#include "IcadCusButton.h"/*DNT*/

WNDPROC ICAD_OrigWndProcT;
WNDPROC ICAD_OrigWndProcH;

LRESULT CALLBACK ICAD_Hbutton(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam) {
	RECT		Rect;		  // Holds the edit boxe's coords.
	PAINTSTRUCT Ps; 		  // Holds the paint structure.
  	HDC 		hDC;		  // Handle to the device context.
	static BOOL TState=TRUE;
	short		x,
				y;

	switch(uMsg) {
   		case WM_MOUSEMOVE:
			switch(wParam) {
				case MK_LBUTTON | MK_RBUTTON:
					GetClientRect(hWnd,&Rect);
					// Set the X pos. and the Y pos of cursor.
					// EBATECH(CNBR) 2001-06-24 need unsigned short -> short
					x=(short)LOWORD(lParam); y=(short)HIWORD(lParam);
					if ((x<Rect.left || y<Rect.top || x>Rect.right || y>Rect.bottom) && TState) {
						hDC=GetDC(hWnd);
						ICAD_HUp(hWnd,hDC);
						ReleaseDC(hWnd,hDC);
						TState	 =FALSE;
					} else if ((x>Rect.left && y>Rect.top && x<Rect.right && y<Rect.bottom) && !TState) {
						hDC=GetDC(hWnd);
						ICAD_HDown(hWnd,hDC);
						ReleaseDC(hWnd,hDC);
						TState	 =TRUE;
					}
					break;
			}
			break;
		case WM_LBUTTONUP:
			hDC=GetDC(hWnd);
			ICAD_HUp(hWnd,hDC);
			ReleaseDC(hWnd,hDC);
			TState=TRUE;
			break;
		case WM_LBUTTONDOWN:
			// Capture the mouse input.
			SetCapture(hWnd);
			// Set the focus to the current item.
			SetFocus(hWnd);
			// Set the device context ready for painting.
			hDC=GetDC(hWnd);
			ICAD_HDown(hWnd,hDC);
			// Release the evice context.
			ReleaseDC(hWnd,hDC);
			break;
		case WM_PAINT:
   			// Set the device context ready for painting.
			hDC=BeginPaint(hWnd,&Ps);
			ICAD_HUp(hWnd,hDC);
			// End the paint device context.
			EndPaint(hWnd,&Ps);
			break;
	}
	return(CallWindowProc((WNDPROC)(long (__stdcall *)(void))ICAD_OrigWndProcH,hWnd,uMsg,wParam,lParam));
}

LRESULT CALLBACK ICAD_Tbutton(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam) {
	RECT		Rect;		  // Holds the edit boxe's coords.
	PAINTSTRUCT Ps; 		  // Holds the paint structure.
  	HDC 		hDC;		  // Handle to the device context.
	static BOOL TState=TRUE;
	static BOOL TFlag =FALSE;
	short		x,
				y;

	switch(uMsg) {
		case ICAD_CUS_CREATE:
			TState=TRUE;
			TFlag=FALSE;
			break;
   		case WM_MOUSEMOVE:
			switch(wParam) {
				case MK_LBUTTON | MK_RBUTTON:
					GetClientRect(hWnd,&Rect);
					// Set the X pos. and the Y pos of cursor.
					// EBATECH(CNBR) 2001-06-24 need unsigned short -> short
					x=(short)LOWORD(lParam); y=(short)HIWORD(lParam);
					if ((x<Rect.left || y<Rect.top || x>Rect.right || y>Rect.bottom) && TState) {
						hDC=GetDC(hWnd);
						ICAD_TUp(hWnd,hDC,TFlag);
						ReleaseDC(hWnd,hDC);
						TState	 =FALSE;
					} else if ((x>Rect.left && y>Rect.top && x<Rect.right && y<Rect.bottom) && !TState) {
						hDC=GetDC(hWnd);
						ICAD_TDown(hWnd,hDC,TFlag);
						ReleaseDC(hWnd,hDC);
						TState	 =TRUE;
					}
					break;
			}
			break;
		case WM_LBUTTONUP:
			if (TState) TFlag=(TFlag)?FALSE:TRUE;
			hDC=GetDC(hWnd);
			ICAD_TUp(hWnd,hDC,TFlag);
			ReleaseDC(hWnd,hDC);
			break;
		case WM_LBUTTONDOWN:
			// Capture the mouse input.
			SetCapture(hWnd);
			// Set the focus to the current item.
			SetFocus(hWnd);
			// Set the device context ready for painting.
			hDC=GetDC(hWnd);
			ICAD_TDown(hWnd,hDC,TFlag);
			// Release the evice context.
			ReleaseDC(hWnd,hDC);
			break;
		case WM_PAINT:
   			// Set the device context ready for painting.
			hDC=BeginPaint(hWnd,&Ps);
			ICAD_TUp(hWnd,hDC,TFlag);
			// End the paint device context.
			EndPaint(hWnd,&Ps);
			break;
	}
	return(CallWindowProc((WNDPROC)(long (__stdcall *)(void))ICAD_OrigWndProcT,hWnd,uMsg,wParam,lParam));
}

BOOL ICAD_TAB(RECT Rect,HDC hDC) {
	HPEN	hPen;

  	// Set the pen to white.
	if (SelectObject(hDC,GetStockObject(WHITE_PEN))==(HGDIOBJ)NULL) return FALSE;
	MoveToEx(hDC,Rect.left,(Rect.bottom-2),NULL);
	LineTo(hDC,Rect.left,(Rect.top+2));
	LineTo(hDC,(Rect.left+2),Rect.top);
	LineTo(hDC,(Rect.right-1),Rect.top);

	if ((hPen=CreatePen(PS_SOLID,0,GetSysColor(COLOR_BTNSHADOW)))==NULL) return FALSE;
	if ((SelectObject(hDC,hPen))==(HGDIOBJ)NULL) return FALSE;
	LineTo(hDC,(Rect.right-1),(Rect.bottom-1));

	DeleteObject(hPen);

	// Set the pen to black.
	if (SelectObject(hDC,GetStockObject(BLACK_PEN))==(HGDIOBJ)NULL) return FALSE;
	MoveToEx(hDC,Rect.right,(Rect.top+2),NULL);
	LineTo(hDC,Rect.right,(Rect.bottom-1));

	return TRUE;
}

BOOL ICAD_HUp(HWND hWnd,HDC hDC) {
	RECT	Rect;	// Holds the edit boxe's coords.
	CDC* pcDC;

	CDC* pcCDC = new CDC();
	CBitmap* pbm = new CBitmap();

	if (GetClientRect(hWnd,&Rect)==FALSE) {
		delete pcCDC;
		delete pbm;
		return FALSE;
	}

	ICAD_EraseBkGround(Rect,hDC);

	// Display the image.
	pcDC=pcDC->FromHandle(hDC);
	pcCDC->CreateCompatibleDC(pcDC);
	pbm->LoadMappedBitmap(EXP_HELPMARK);
	pcCDC->SelectObject(pbm);
	pcDC->BitBlt((Rect.left+1),(Rect.top+2),Rect.right,Rect.bottom,pcCDC,Rect.left,Rect.top,SRCCOPY);

	ICAD_BorderUp(Rect,hDC);

	delete pcCDC;
	delete pbm;

	return TRUE;
}


BOOL ICAD_HDown(HWND hWnd,HDC hDC) {
	RECT	Rect;	// Holds the edit boxe's coords.
	CDC* pcDC;

	CDC* pcCDC = new CDC();
	CBitmap* pbm = new CBitmap();

	if (GetClientRect(hWnd,&Rect)==FALSE) {
		delete pcCDC;
		delete pbm;
		return FALSE;
	}

	ICAD_EraseBkGround(Rect,hDC);

	pcDC=pcDC->FromHandle(hDC);
	pcCDC->CreateCompatibleDC(pcDC);
	pbm->LoadMappedBitmap(EXP_HELPMARK);
	pcCDC->SelectObject(pbm);
	pcDC->BitBlt((Rect.left+2),(Rect.top+2),(Rect.right-2),(Rect.bottom-2),pcCDC,Rect.left,Rect.top,SRCCOPY);

	ICAD_BorderDown(Rect,hDC);

	delete pcCDC;
	delete pbm;

	return TRUE;
}

BOOL ICAD_TDown(HWND hWnd,HDC hDC,BOOL Flag) {
	RECT	Rect;	// Holds the edit boxe's coords.
	CDC* pcDC;

	CDC* pcCDC = new CDC();
	CBitmap* pbm = new CBitmap();

	if (GetClientRect(hWnd,&Rect)==FALSE) {
		delete pcCDC;
		delete pbm;
		return FALSE;
	}

	ICAD_EraseBkGround(Rect,hDC);

	pcDC=pcDC->FromHandle(hDC);
	pcCDC->CreateCompatibleDC(pcDC);
	pbm->LoadMappedBitmap((!Flag)?EXP_TACKUP:EXP_TACKDOWN);
	pcCDC->SelectObject(pbm);
	pcDC->BitBlt((Rect.left+1),(Rect.top+1),Rect.right,Rect.bottom,pcCDC,Rect.left,Rect.top,SRCCOPY);

	ICAD_BorderDown(Rect,hDC);

	delete pcCDC;
	delete pbm;

	return TRUE;
}


BOOL ICAD_TUp(HWND hWnd,HDC hDC,BOOL Flag) {
	RECT Rect;	 // Holds the edit boxe's coords.
	CDC* pcDC;

	CDC* pcCDC = new CDC();
	CBitmap* pbm = new CBitmap();

	if (GetClientRect(hWnd,&Rect)==FALSE) {
		delete pcCDC;
		delete pbm;
		return FALSE;
	}

	ICAD_EraseBkGround(Rect,hDC);

	pcDC=pcDC->FromHandle(hDC);
	pcCDC->CreateCompatibleDC(pcDC);

	if (!Flag) {
		// Display the image.
		pbm->LoadMappedBitmap(EXP_TACKUP);
		pcCDC->SelectObject(pbm);
		pcDC->BitBlt((Rect.left+1),(Rect.top+1),Rect.right,Rect.bottom,pcCDC,Rect.left,Rect.top,SRCCOPY);
		ICAD_BorderUp(Rect,hDC);
	} else {
		// Display the image.
		pbm->LoadMappedBitmap(EXP_TACKDOWN);
		pcCDC->SelectObject(pbm);
		pcDC->BitBlt((Rect.left+1),(Rect.top+1),Rect.right,Rect.bottom,pcCDC,Rect.left,Rect.top,SRCCOPY);
		ICAD_BorderDown(Rect,hDC);
	}

	delete pcCDC;
	delete pbm;

	return TRUE;
}

BOOL ICAD_EraseBkGround(RECT Rect,HDC hDC) {
	HPEN	hPen;
	HBRUSH	hBrush;

	if ((hPen=CreatePen(PS_SOLID,0,GetSysColor(COLOR_BTNFACE)))==NULL) return FALSE;
	if ((hBrush=CreateSolidBrush(GetSysColor(COLOR_BTNFACE)))==NULL) return FALSE;

	if ((SelectObject(hDC,hPen))==(HGDIOBJ)NULL) return FALSE;
	if ((SelectObject(hDC,hBrush))==(HGDIOBJ)NULL) return FALSE;

	// Erase all paint on button to COLOR_BTNFACE.
	Rectangle(hDC,Rect.left,Rect.top,Rect.right,Rect.bottom);

	DeleteObject(hPen);
	DeleteObject(hBrush);

	return TRUE;
}

BOOL ICAD_BorderDown(RECT Rect,HDC hDC) {
	HPEN	hPen;

	// Set the pen to white.
	if (SelectObject(hDC,GetStockObject(BLACK_PEN))==(HGDIOBJ)NULL) return FALSE;
	MoveToEx(hDC,Rect.right,Rect.top,NULL);
	LineTo(hDC,Rect.left,Rect.top);
	LineTo(hDC,Rect.left,Rect.bottom);

	// Set the pen to black.
	if (SelectObject(hDC,GetStockObject(WHITE_PEN))==(HGDIOBJ)NULL) return FALSE;
	LineTo(hDC,Rect.right,Rect.bottom);
	LineTo(hDC,Rect.right,(Rect.top-1));

	if ((hPen=CreatePen(PS_SOLID,0,GetSysColor(COLOR_BTNSHADOW)))==NULL) return FALSE;
	if ((SelectObject(hDC,hPen))==(HGDIOBJ)NULL) return FALSE;
	MoveToEx(hDC,(Rect.right-2),(Rect.top+1),NULL);
	LineTo(hDC,(Rect.left+1),(Rect.top+1));
	LineTo(hDC,(Rect.left+1),(Rect.bottom-1));

	DeleteObject(hPen);

	return TRUE;
}

BOOL ICAD_BorderUp(RECT Rect,HDC hDC) {
	HPEN	hPen;

	// Set the pen to white.
	if (SelectObject(hDC,GetStockObject(WHITE_PEN))==(HGDIOBJ)NULL) return FALSE;
	MoveToEx(hDC,(Rect.right-1),Rect.top,NULL);
	LineTo(hDC,Rect.left,Rect.top);
	LineTo(hDC,Rect.left,Rect.bottom);

	// Set the pen to black.
	if (SelectObject(hDC,GetStockObject(BLACK_PEN))==(HGDIOBJ)NULL) return FALSE;
	LineTo(hDC,Rect.right,Rect.bottom);
	LineTo(hDC,Rect.right,(Rect.top-1));

	if ((hPen=CreatePen(PS_SOLID,0,GetSysColor(COLOR_BTNSHADOW)))==NULL) return FALSE;
	if ((SelectObject(hDC,hPen))==(HGDIOBJ)NULL) return FALSE;
	MoveToEx(hDC,(Rect.right-1),(Rect.top+1),NULL);
	LineTo(hDC,(Rect.right-1),(Rect.bottom-1));
	LineTo(hDC,Rect.left,Rect.bottom-1);

	DeleteObject(hPen);

	return TRUE;
}

