/* SDSTHREADJOBSINK.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Implements the SDSThreadJobSink interface
 */ 
#include "Icad.h" /*DNT*/
#include "sdsthreadjob.h" /*DNT*/
#include "sdsthreadjobsink.h" /*DNT*/
#include "hiddenwindow.h" /*DNT*/




// ***********************************************
// We use a window to handle the interthread communication
//
//
class JobHandlerWindow : public HiddenWindow
	{
	
		
		// ---------------------------------
		// INTERFACE
		//
		public:
			LRESULT WindowProc( UINT msg, WPARAM wParam, LPARAM lParam );
		
		
		public:
			static UINT GetJobWindowMessage( void );
			
	
		
	};
	
	
UINT 
JobHandlerWindow::GetJobWindowMessage( void )
	{
	
	return ::RegisterWindowMessage( "IntelliCADJobWindowMessage" );
	}
	
LRESULT
JobHandlerWindow::WindowProc( UINT msg, WPARAM wParam, LPARAM lParam )
	{
	
	if ( msg == GetJobWindowMessage() )
		{
		SDSThreadJob *pJob = (SDSThreadJob *)wParam;
		if ( pJob != NULL )
			{
			ASSERT( pJob->IsValid() );
			
			int iRetval = pJob->Execute();
			
			// Don't forget to delete the object
			//
			delete pJob;
			
			return (LRESULT)iRetval;
			}
		
		ASSERT( false );	
		return 0;			
		}
	else
		{
		return HiddenWindow::WindowProc( msg, wParam, lParam );	
		}
	}	

	
	
// ***********************************************
// CONCRETE CLASS
//
// This actually provides an implementation for
// SDSThreadJobSink
//
class SDSThreadJobSinkImpl : public SDSThreadJobSink
	{
	
		// --------------------------
		// CTORS and DTORS
		//
		public:
			SDSThreadJobSinkImpl( void );
			virtual ~SDSThreadJobSinkImpl( void );
			
		// --------------------------
		// INTERFACE
		//
		public:
			virtual int		SendJob( SDSThreadJob *pJob );
			virtual void	PostJob( SDSThreadJob *pJob );
			
		// ---------------------------
		// internal data
		//
		private:
			JobHandlerWindow *m_pJobHandlerWindow;
	};
	
	
SDSThreadJobSinkImpl::SDSThreadJobSinkImpl( void )
	{
	m_pJobHandlerWindow = new JobHandlerWindow();
	if ( m_pJobHandlerWindow )
		{
		VERIFY( m_pJobHandlerWindow->Create() );
		}
	}


SDSThreadJobSinkImpl::~SDSThreadJobSinkImpl( void )
	{
	
	// What about unhandled jobs??
	//
	m_pJobHandlerWindow->Close();
	
	delete m_pJobHandlerWindow;
	m_pJobHandlerWindow = NULL;
	}	

int	
SDSThreadJobSinkImpl::SendJob( SDSThreadJob *pJob )
	{
	
	return (int)m_pJobHandlerWindow->SendMessage( JobHandlerWindow::GetJobWindowMessage(),
											(WPARAM)pJob,
											0 );
	}

void
SDSThreadJobSinkImpl::PostJob( SDSThreadJob *pJob )
	{	
	m_pJobHandlerWindow->SendNotifyMessage( JobHandlerWindow::GetJobWindowMessage(),
										(WPARAM)pJob,
										0 );
	}


SDSThreadJobSink *CreateSDSThreadJobSink( void )
	{
	
	return new SDSThreadJobSinkImpl();
	}	
