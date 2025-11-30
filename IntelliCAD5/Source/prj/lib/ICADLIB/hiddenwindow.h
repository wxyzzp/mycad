/* HIDDENWINDOW.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * 
 */ 
#ifndef _HIDDENWINDOW_H
#define _HIDDENWINDOW_H


class HiddenWindow
	{

	
	// This pretty much has to be a friend.  In some ways it is a class method, it just exists
	// because Windows doesn't know about C++ classes
	//
	friend LRESULT CALLBACK hiddenWindowProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// CTORS and DTORS
	//
	public:
		HiddenWindow( void );
		virtual ~HiddenWindow( void );
	
	// ------------------
	// PUBLIC OPERATIONS
	//
	public:
		virtual bool 	Create( void );
		virtual bool 	Close( void );
		virtual LRESULT WindowProc( UINT msg, WPARAM wParam, LPARAM lParam );
		HWND	GetHWND( void )
			{
			return m_hWnd;
			}
		LRESULT SendMessage( UINT msg, WPARAM wParam, LPARAM lParam )
			{
			ASSERT( ::IsWindow( m_hWnd ) );
			if ( ::IsWindow( m_hWnd ) )
				{
				return ::SendMessage( m_hWnd, msg, wParam, lParam );
				}
			return 0;
			}
		void PostMessage( UINT msg, WPARAM wParam, LPARAM lParam )
			{
			ASSERT( ::IsWindow( m_hWnd ) );
			if ( ::IsWindow( m_hWnd ) )
				{
				::PostMessage( m_hWnd, msg, wParam, lParam );
				}
			}
		void SendNotifyMessage( UINT msg, WPARAM wParam, LPARAM lParam )
			{
			ASSERT( ::IsWindow( m_hWnd ) );
			if ( ::IsWindow( m_hWnd ) )
				{
				::SendNotifyMessage( m_hWnd, msg, wParam, lParam );
				}
			}



	
	// ---------------------
	// Class initialization
	//
	public:
		static bool InitializeClass( HINSTANCE hInstance );
		static HINSTANCE hInstance;
		
	// --------------------
	// helper methods
	//
	private:
		static BOOL registerWindowClass( HINSTANCE hInstance );
		static HiddenWindow *getObjectPointer( HWND hWnd );


		
	// ------------------
	// validity operations
	//
	public:	
		// Is this a properly formed instance?
		//
		bool IsValid()
			{
			bool bRetval = false;
			if ( this != NULL )
				{
				bRetval = this->isValid();
				}
			return bRetval;
			}
	protected:
		virtual bool isValid( void );	
		
				
  	// *****************************
	// Constants
	//
	private:
		static const LPSTR szCLASSNAME;
		static const LPSTR szWINDOWNAME;
		
	// --------------------------
	// internal data
	//	
	private:
		HWND		m_hWnd;
	};
		
		
		
	
	
	



	
#endif // _HIDDENWINDOW_H

