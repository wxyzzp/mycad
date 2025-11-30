/* D:\DEV\PRJ\ICAD\ICADSPLASH.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

// CG: This file was added by the Splash Screen component.
// Splash.cpp : implementation file
//

#include "Icad.h"/*DNT*/
#include "IcadApp.h"/*DNT*/
#include "IcadSplash.h"/*DNT*/  // e.g. splash.h
#include "Authenticator.h"
#include "Authrc.h"
#include "configure.h"

#ifdef _DEBUG

	#if !defined(USE_SMARTHEAP)
	#define new DEBUG_NEW
	#endif

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
//    Splash Screen class

BOOL CSplashWnd::c_bShowSplashWnd;
CSplashWnd* CSplashWnd::c_pSplashWnd;
CSplashWnd::CSplashWnd()
{
}

CSplashWnd::~CSplashWnd()
{
	// Clear the static window pointer.
	ASSERT(c_pSplashWnd == this);
	c_pSplashWnd = NULL;
}

BEGIN_MESSAGE_MAP(CSplashWnd, CWnd)
	//{{AFX_MSG_MAP(CSplashWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CSplashWnd::EnableSplashScreen(BOOL bEnable /*= TRUE*/)
{
	c_bShowSplashWnd = bEnable;
}

void CSplashWnd::ShowSplashScreen(CWnd* pParentWnd /*= NULL*/)
{
	if (!c_bShowSplashWnd || c_pSplashWnd != NULL)
		return;

	// Allocate a new splash screen, and create the window.
	c_pSplashWnd = new CSplashWnd;
	if (!c_pSplashWnd->Create(pParentWnd))
		delete c_pSplashWnd;
	else
		c_pSplashWnd->UpdateWindow();
}

void CSplashWnd::PreTranslateAppMessage(MSG* pMsg)
{
	if (c_pSplashWnd == NULL)
		return;

	// If we get a keyboard or mouse message, hide the splash screen.
	if (pMsg->message == WM_KEYDOWN ||
	    pMsg->message == WM_SYSKEYDOWN ||
	    pMsg->message == WM_LBUTTONDOWN ||
	    pMsg->message == WM_RBUTTONDOWN ||
	    pMsg->message == WM_MBUTTONDOWN ||
	    pMsg->message == WM_NCLBUTTONDOWN ||
	    pMsg->message == WM_NCRBUTTONDOWN ||
	    pMsg->message == WM_NCMBUTTONDOWN)
	{
		c_pSplashWnd->HideSplashScreen();
	}
}


BOOL CSplashWnd::Create(CWnd* pParentWnd /*= NULL*/)
{
//AVH Bricsnet 2/7/2001 merged concurrent changes(Cybage, Tesis) into new algorithm:
//If icadsplash.bmp is available, load it. Else load bmp from resource.
//This allows the splash to be overridden.

//#if defined (BUILD_WITH_LICENSED_COMPONENTS)
//	if(m_bitmap.Attach(LoadImage(NULL, "splash.bmp"/*DNT*/, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE)))
	if(!m_bitmap.Attach(LoadImage(NULL, "IcadSplash.bmp"/*DNT*/, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE)))
	{
		HINSTANCE	authentication = AuthenticationInstance();
		if (!m_bitmap.Attach( ::LoadBitmap( authentication, MAKEINTRESOURCE(IDB_SPLASH))))
			return FALSE;
	}
//#else
//	if ( m_bitmap.Attach( ::LoadBitmap( authentication, MAKEINTRESOURCE(IDB_SPLASH_LITE))) )
//#endif
	BITMAP bm;
	m_bitmap.GetBitmap(&bm);

	return CreateEx(0,
		AfxRegisterWndClass(0, AfxGetApp()->LoadStandardCursor(IDC_ARROW)),
		NULL, WS_POPUP | WS_VISIBLE, 0, 0, bm.bmWidth, bm.bmHeight, pParentWnd->GetSafeHwnd(), NULL);
}

void CSplashWnd::HideSplashScreen()
{
	if (!c_pSplashWnd)
		return;

//4M Item:85->
   c_pSplashWnd->m_bitmap.DeleteObject();
//<-4M Item:85
    CWnd *hMainWnd;
	// Destroy the window, and update the mainframe.
	c_pSplashWnd->DestroyWindow();
	hMainWnd = AfxGetMainWnd();
	if (hMainWnd)
		hMainWnd->UpdateWindow();
}

void CSplashWnd::PostNcDestroy()
{
	// Free the C++ class.
	delete this;
}

int CSplashWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Center the window.
	CenterWindow();

	// Set a timer to destroy the splash screen.
	SetTimer(1, 750, NULL);

	return 0;
}

void CSplashWnd::OnPaint()
{
	CPaintDC dc(this);

	CDC dcImage;
	if (!dcImage.CreateCompatibleDC(&dc))
		return;

	BITMAP bm;
	m_bitmap.GetBitmap(&bm);

	// Paint the image.
	CBitmap* pOldBitmap = dcImage.SelectObject(&m_bitmap);
	dc.BitBlt(0, 0, bm.bmWidth, bm.bmHeight, &dcImage, 0, 0, SRCCOPY);
	dcImage.SelectObject(pOldBitmap);
}

void CSplashWnd::OnTimer(UINT nIDEvent)
{
	// Destroy the splash screen window.
	HideSplashScreen();
}


