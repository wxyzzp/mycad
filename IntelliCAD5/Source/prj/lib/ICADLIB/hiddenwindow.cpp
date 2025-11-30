/* HIDDENWINDOW.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * 
 */ 
#define STRICT
#include <windows.h>
#include "icadlib.h" /*DNT*/
#include "hiddenwindow.h" /*DNT*/
 
// ***************************************************************
// This Windowproc simply redispatches to the object's window proc
//
//
//
static LRESULT CALLBACK
hiddenWindowProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
	ASSERT( ::IsWindow( hWnd ) );

	HiddenWindow *pWindow = HiddenWindow::getObjectPointer( hWnd );

	// pWindow will be NULL during window creation and shutdown, so
	// we need to handle that here
	//
	if ( pWindow != NULL )
		{
		return pWindow->WindowProc( msg, wParam, lParam );
		}
	else
		{
		return ::DefWindowProc( hWnd, msg, wParam, lParam );
		}
	}




// *******************************************************
// CLASS IMPLEMENTATION
//
// HiddenWindow
//
//	

// *****************
// CONSTANTS
//
const LPSTR HiddenWindow::szCLASSNAME = "IntelliCAD Hidden Window Class";
const LPSTR HiddenWindow::szWINDOWNAME = "IntelliCAD Hidden Window Name";


// *****************
// CLASS MEMBERS
//
HINSTANCE HiddenWindow::hInstance = NULL;

// ******************************************************************
// CONSTRUCTOR
//
//
HiddenWindow::HiddenWindow( void )
	{
	m_hWnd = NULL;
	}

// *******************************************************************
// DESTRUCTOR
//
//
HiddenWindow::~HiddenWindow( void )
	{
	Close();
	m_hWnd = NULL;
	}


// *******************************************************************
// PUBLIC METHOD
//
// WindowProc
//
//
LRESULT
HiddenWindow::WindowProc( UINT msg, WPARAM wParam, LPARAM lParam )
	{
	if ( msg == WM_QUIT )
		{
		::DestroyWindow( m_hWnd );
		m_hWnd = NULL;
		return 0;
		}

	return ::DefWindowProc( GetHWND(), msg, wParam, lParam );
	}

// *******************************************************
// PUBLIC CLASS METHOD -- class HiddenWindow
//
// InitializeClass
//
//
bool
HiddenWindow::InitializeClass( HINSTANCE hInstance )
	{
	bool bRetval = false;

	HiddenWindow::hInstance = hInstance;
	
	if ( HiddenWindow::registerWindowClass( hInstance ) )
		{
		bRetval = true;
		}

	return bRetval;
	}

// *******************************************************
// PRIVATE CLASS METHOD -- class HiddenWindow
//
// RegisterWindowClass
// 
//
BOOL 
HiddenWindow::registerWindowClass( HINSTANCE hInstance ) 
	{ 

    WNDCLASSEX wcx; 
 
    // Fill in the window class structure with parameters 
    // that describe the main window. 
 
    wcx.cbSize = sizeof(wcx);          // size of structure 
    wcx.style = 0;
    wcx.lpfnWndProc = ::hiddenWindowProc;     // points to window procedure 
    wcx.cbClsExtra = 0;                // no extra class memory 
    wcx.cbWndExtra = sizeof( HiddenWindow *);                // no extra window memory 
    wcx.hInstance = hInstance;         // handle of instance 
    wcx.hIcon = NULL;
    wcx.hCursor = LoadCursor(NULL, IDC_ARROW);  // predefined arrow 
    wcx.hbrBackground = (HBRUSH)::GetStockObject(WHITE_BRUSH);  // white background brush 
    wcx.lpszMenuName = NULL;    // name of menu resource 
    wcx.lpszClassName = HiddenWindow::szCLASSNAME;  // name of window class 
    wcx.hIconSm = NULL;
 
    // Register the window class. 
 
    return ::RegisterClassEx(&wcx); 
	} 

// *****************************************************
// PRIVATE CLASS HELPER METHOD
//
//
HiddenWindow *
HiddenWindow::getObjectPointer( HWND hWnd )
	{
	ASSERT( ::IsWindow( hWnd ) );
	
	HiddenWindow *pRetval = NULL;
	if ( ::IsWindow( hWnd ) )
		{
		pRetval = (HiddenWindow *)::GetWindowLong( hWnd, GWL_USERDATA );
		
		// pRetval WILL be NULL during Window creation
		//
		if ( pRetval != NULL )
			{
			// sanity checking
			//
			ASSERT( pRetval->IsValid() );
			ASSERT( pRetval->GetHWND() == hWnd );
			}
		}
	
	return pRetval;	
	}

// ******************************************************8
// PUBLIC METHOD -- class HiddenWindow
//
// Create
//
//
bool
HiddenWindow::Create( void ) 
	{ 
    HWND hwnd; 
    // Create the main window. 
 
    hwnd = ::CreateWindow( 
		HiddenWindow::szCLASSNAME,        // name of window class 
		HiddenWindow::szWINDOWNAME,
        WS_OVERLAPPED, // top-level window 
        -500,       // default horizontal position 
        -500,       // default vertical position 
        0,       // default width 
        0,       // default height 
        (HWND) NULL,         // no owner window 
        (HMENU) NULL,        // use class menu 
        HiddenWindow::hInstance,
        (LPVOID) NULL);      // no window-creation data 
 
    if (hwnd) 
		{
		this->m_hWnd = hwnd;
		::SetWindowLong( hwnd, GWL_USERDATA, (LONG)this);
	    ::ShowWindow(hwnd, SW_HIDE); 
		}
	else
		{
		LONG err = ::GetLastError();
		ASSERT( false );
		}

    return (hwnd != NULL); 
 
	} 
 

// **********************************************************************
// PUBLIC METHOD -- class HiddenWindow
// 
// Close
//
//
bool
HiddenWindow::Close( void )
	{
	bool bRetval = false;

	if ( this->m_hWnd != NULL )
		{
		m_hWnd = NULL;
		bRetval = true;
		}

	return bRetval;
	}

 
// ***************************
// Validity checking method
//
bool
HiddenWindow::isValid( void )
	{
	if ( !CHECKSTRUCTPTR( this ) )
		{
		return false;
		}

	if ( !::IsWindow( m_hWnd ) )
		{
		return false;
		}

	return true;
	}
 
