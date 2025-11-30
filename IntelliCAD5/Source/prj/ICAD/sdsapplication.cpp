/* SDSAPPLICATION.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Class that keeps track of sds applications that are loaded.
 * 
 */ 
#include "Icad.h" /*DNT*/
#include "IcadApi.h"
#include "sdsthreadexception.h" /*DNT*/
#include "sdsthreadhandler.h" /*DNT*/
#include "sdsthreadjob.h" /*DNT*/
#include "sdsthreadjobsink.h" /*DNT*/
#include "sdsthread.h" /*DNT*/
#include "sdsapplication.h" /*DNT*/
#include "threadcontroller.h" /*DNT*/
#include "commandqueue.h" /*DNT*/
#include <process.h>


// ------------------------------
// GLOBAL
//
SDSApplication *SDS_LoadedAppsLast;


// ***********************
// CLASS DATA
//
SDSApplication *SDSApplication::pLoadedAppsAct = NULL;		
SDSApplication *SDSApplication::pEngineApplication = NULL;

// **************************************
// MODULE VARIABLE
//
static SingleThreadSection zStaticMethodSection;

// **********************
// CLASS METHOD -- class SDSApplication
//
SDSApplication*
SDSApplication::GetCurrentApplication( void )
	{
	// Careful about threads...
	//
	SingleThreadGuard theGuard( &zStaticMethodSection );

	DWORD dwThreadID = ::GetCurrentThreadId();
	SDSApplication *pIter = SDSApplication::GetEngineApplication();
	SDSApplication *pRetval = NULL;
	while( pIter != NULL )
		{
		if ( (pIter->GetMainThread()->GetThreadID() == dwThreadID) ||
			 (pIter->GetCommandThread()->GetThreadID() == dwThreadID) )
			{
			pRetval = pIter;
			break;
			}

		pIter = pIter->GetNextLink();
		}

	return pRetval;
	}

// ************************
// CLASS METHOD -- class SDSApplication
//
// The Active Application is the one associated with this thread.
// If none such, keep track of a variable pLoadedAppsAct that is probably
// the "closest".  This is a HACK required by the old architecture
//
SDSApplication *
SDSApplication::GetActiveApplication( void )
	{
	// Careful about threads...
	//
	SingleThreadGuard theGuard( &zStaticMethodSection );
	

	SDSApplication *pRetval = NULL;
	
	// we really shouldn't be on the UI Thread
	//
	if ( ::OnUIThread() )
		{
		pRetval = SDSApplication::pLoadedAppsAct;
		}
	else
		{		
		pRetval = SDSApplication::GetCurrentApplication();
		}
	
	// If pRetval is NULL, we are on a foreign thread.  Hope the old
	// setting is correct
	//
	if ( pRetval == NULL )
		{
		
		
		pRetval = SDSApplication::pLoadedAppsAct;
		
		// If the old setting is also NULL, hope the Engine thread is around
		//
		if ( pRetval == NULL )
			{
			pRetval = SDSApplication::pEngineApplication;
			
			// If this is bad, PANIC!
			ASSERT( pRetval != NULL );
			return pRetval;	
			}
		}	

	//rk
#if 1
	ASSERTBREAK( IDS_IsIcadExiting() || pRetval->IsValid() );
#else
	ASSERTBREAK( pRetval->IsValid() );
#endif
	
	SDSApplication::pLoadedAppsAct = pRetval;
	
	return pRetval;
	}	


// ******************************
// CLASS METHOD -- class SDSApplication
//
/*
bool
SDSApplication::SetEngineApplication( SDSApplication *pNewBegin )
	{
	// This setting should only happen once
	//
	ASSERT( SDSApplication::pEngineApplication == NULL );
	ASSERT( pNewBegin->IsValid() );
	
	SDSApplication::pEngineApplication = pNewBegin;
	
	return true;
	}
*/
// *********************************
// CLASS METHOD -- class SDSApplication
//
SDSApplication *
SDSApplication::GetEngineApplication( void )
	{
	
	return SDSApplication::pEngineApplication;
	}	
	
// ***************************************************
// CLASS IMPLEMENTATION -- class SDSApplication
//
//
//
//
#define SDSAPPLICATIONCHECKVAL	0x1324

SDSApplication::SDSApplication( const char *appname )
	{
	m_ApplicationName = NULL;
	db_astrncpy(&m_ApplicationName, appname, -1);

	m_iRQcode = 0;
	m_bAppLoaded = false;

	m_pAppMainThread = NULL;
	m_pAppCommandThread = NULL;

	m_bCommandRunning = false;
    ClearKeywords();
	::memset( thekeyword, 0, IC_ACADBUF );
	m_iInputControlBits = 0;
	m_bAcceptParenOrCR = false;
	m_icmddepth = 0;
	m_ilispcmddepth = 0;
	m_CallBackFunctionPtr = NULL;
	m_pNextLink = NULL;
	m_bUnloading = false;

	m_iCheckVal = SDSAPPLICATIONCHECKVAL;

	m_pCommandQueue = new CommandQueue();

	m_bTerminatedMainThread = false;
	m_bTerminatedCommandThread = false;
	m_bCmdThreadError = false;
	}

SDSApplication::~SDSApplication( void )
	{

	ASSERT( IsValid() );

	// Remove registered custom osnap modes assotiated with this application from the
	// Osnap Manager (if any)
	// Note: Standard modes are added in constructor and removed in destructor of OsnapManager
	if( pEngineApplication && pEngineApplication == this )
	{
		COsnapManagerIcad *pOsnapManager = ((CEngineApplication*)pEngineApplication)->getOsnapManager();
		pOsnapManager->removeAppModes( this );
	}

	m_iCheckVal = -1;

	if ( m_ApplicationName != NULL )
		{
		delete [] m_ApplicationName ;
		m_ApplicationName = NULL;
		}


	delete m_pAppMainThread;	
	m_pAppMainThread = NULL;
	
	delete m_pAppCommandThread;
	m_pAppCommandThread = NULL;

	m_bCommandRunning = false;
    ClearKeywords();
	::memset( thekeyword, 0, IC_ACADBUF );
	m_iInputControlBits = 0;
	m_bAcceptParenOrCR = false;
	m_icmddepth = 0;
	m_ilispcmddepth = 0;
	m_CallBackFunctionPtr = NULL;
	m_pNextLink = NULL;
	m_bUnloading = false;

	if ( m_pCommandQueue != NULL )
		{
		delete m_pCommandQueue;
		m_pCommandQueue = NULL;
		}

	}

bool
SDSApplication::isValid( void )
	{

	if ( this == NULL )
		{
		return false;
		}

	if ( !CHECKSTRUCTPTR( this ) )
		{
		return false;
		}

	if ( m_iCheckVal != SDSAPPLICATIONCHECKVAL )
		{
		return false;
		}

	if ( !m_pCommandQueue->IsValid() )
		{
		return false;
		}

	// TODO: ADD MORE VALIDITY CHECKS HERE -- class SDSApplication

	return true;
	}


SDSApplication *
SDSApplication::GetNextLink( void )
	{
	SYNCHRONIZEMETHOD();

	SDSApplication *pRetval;
	ASSERT( IsValid() );

	pRetval = m_pNextLink;
	ASSERT( (pRetval == NULL) ||
			(pRetval->IsValid()) );

	return pRetval;
	}

SDSApplication *
SDSApplication::AddLink( SDSApplication *pNewLink )
	{
	SYNCHRONIZEMETHOD();

	ASSERT( IsValid() );
	ASSERT( pNewLink->IsValid() );
	ASSERT( (pNewLink->m_pNextLink == NULL) );
	SDSApplication *pTemp = this;
	while( pTemp->GetNextLink() )
		{
		pTemp = pTemp->GetNextLink();
		ASSERT( pTemp->IsValid() );
		}

	pTemp->m_pNextLink = pNewLink;

	return pTemp;
	}

// ***********************
// Static Method
//
bool
SDSApplication::RemoveLink( SDSApplication *pOldLink )
	{
	// Careful about threads...
	//
	SingleThreadSection theSection;
	SingleThreadGuard theGuard( &theSection );
   
	if ( SDSApplication::GetEngineApplication() == pOldLink )
		{
		SDSApplication::pEngineApplication = pOldLink->GetNextLink();
		return true;
		}

	SDSApplication *pTemp,*pPrev = NULL;

	pTemp = SDSApplication::GetEngineApplication();

	while( (pTemp != NULL) && 
			(pOldLink != pTemp) )
		{
		pPrev = pTemp;
		pTemp = pTemp->GetNextLink();
		}

	if ( pTemp == pOldLink )
		{
		ASSERT( pPrev != NULL );
		ASSERT( pPrev->m_pNextLink == pOldLink );
		pPrev->m_pNextLink = pOldLink->m_pNextLink;
		return true;
		}

	// didn't find it, must have already been removed
	//
	return false;

	}


// *********************************************************************
// PUBLIC METHOD -- class SDSApplication
//
// CallCallBack
//
//


// ---------------------------
// Helper class -- used to dispatch the callback to another thread
//
class CallCallBackJob : public SDSThreadJob
	{
		// ------------------------------
		// CTOR and DTOR
		//
		public:
			CallCallBackJob( int (*cbfnptr)(int flag,void *arg1,void *arg2,void *arg3),
							 int flag,
							void *arg1,
							void *arg2,
							void *arg3 )
				{
				this->m_cbfnptr = cbfnptr;
				this->m_flag = flag;
				this->m_arg1 = arg1;
				this->m_arg2 = arg2;
				this->m_arg3 = arg3;
				}
		
			virtual int Execute( void )
				{
				ASSERT( CHECKFUNCPTR( m_cbfnptr ) );

				int iRetval = m_cbfnptr( m_flag, m_arg1, m_arg2, m_arg3 );

				return iRetval;
				}
		protected:
			virtual bool isValid( void )
				{
				return ( m_cbfnptr != NULL );
				}

		// --------------------
		// internal data
		//
		private:
				int (*m_cbfnptr)(int flag,void *arg1,void *arg2,void *arg3);
				int m_flag;
				void *m_arg1;
				void *m_arg2;
				void *m_arg3;
	};

int 
SDSApplication::CallCallBack( int flag,void *arg1,void *arg2,void *arg3 )
	{
	int retval = RTNORM;

	ASSERT( IsValid() );

	if ( IsValid() )
		{
		if ( m_CallBackFunctionPtr != NULL )
			{
			ASSERT( CHECKFUNCPTR( m_CallBackFunctionPtr ) );


			// If we're active, go ahead and call the callback
			// otherwise let that thread do it through it's job sink
			//
			if ( this->GetMainThread()->IsCurrent() )
				{
				retval = m_CallBackFunctionPtr( flag, arg1, arg2, arg3 );
				}
			else
				{
										 
				CallCallBackJob *pJob = new CallCallBackJob( m_CallBackFunctionPtr, flag, arg1, arg2, arg3 );
				retval = this->GetMainThread()->SendJob( pJob );

				// This line of code has the side-effect of setting the last active application to
				// the current one, which is all to the good, though theoretically not necessary
				//
				SDSApplication::GetActiveApplication();

				}
			}
		}

	return retval;
	}
	
// *********************************************************************
// PUBLIC METHOD -- class SDSApplication
//
// SetCallBack
//
//
bool 
SDSApplication::SetCallBack( int (*cbfnptr)(int flag,void *arg1,void *arg2,void *arg3) )
	{
	ASSERT( IsValid() );
	ASSERT( CHECKFUNCPTR( cbfnptr ) );

	m_CallBackFunctionPtr = cbfnptr;

	return true;
	}

int  
SDSApplication::IsKeywordsEmpty()
{
    return ((int)keywords.IsEmpty());
}
void 
SDSApplication::ClearKeywords()
{
    keywords.Empty();
}

LPCTSTR 
SDSApplication::GetKeywords()
{
    return (LPCTSTR(keywords));
}

int  
SDSApplication::SetKeywords(char *kwords)
{
    keywords = kwords;
    return RTNORM;
}

int 
SDSApplication::GetLispCommandDepth( void )
	{
	SYNCHRONIZEMETHOD();

	ASSERT( m_ilispcmddepth >= 0 );
	return m_ilispcmddepth;
	}

void
SDSApplication::IncrementLispCommandDepth( void )
	{
	SYNCHRONIZEMETHOD();

	ASSERT( m_ilispcmddepth >= 0 );

	m_ilispcmddepth++;
	}

void
SDSApplication::DecrementLispCommandDepth( void )
	{
	SYNCHRONIZEMETHOD();

	ASSERT( m_ilispcmddepth > 0 );

	m_ilispcmddepth--;
	}

void
SDSApplication::ClearLispCommandDepth( void )
	{
	m_ilispcmddepth = 0;
	}


int 
SDSApplication::GetCommandDepth( void )
	{
	SYNCHRONIZEMETHOD();

	ASSERT( m_icmddepth >= 0 );
	return m_icmddepth;
	}

void
SDSApplication::IncrementCommandDepth( void )
	{
	SYNCHRONIZEMETHOD();

	ASSERT( m_icmddepth >= 0 );

	m_icmddepth++;
	}

void
SDSApplication::DecrementCommandDepth( void )
	{
	SYNCHRONIZEMETHOD();

	ASSERT( m_icmddepth > 0 );

	m_icmddepth--;
	}

bool
SDSApplication::AcceptParenOrCR( void )
	{

	return m_bAcceptParenOrCR;
	}

void
SDSApplication::SetAcceptParenOrCR( bool bValue )
	{

	m_bAcceptParenOrCR = bValue;
	}

int
SDSApplication::GetInputControlBits( void )
	{
		
	return m_iInputControlBits;
	}

void
SDSApplication::SetInputControlBits( int bits )
	{

	m_iInputControlBits = bits;
	}

SDSThread *
SDSApplication::GetMainThread( void )
	{
	return m_pAppMainThread;
	}

SDSThread *
SDSApplication::GetCommandThread( void )
	{
	return m_pAppCommandThread;
	}

bool 
SDSApplication::TerminateMainThread( void )
	{
	this->Lock();

	if ( !m_bTerminatedMainThread &&
		 (m_pAppMainThread != NULL) )
		{
		m_bTerminatedMainThread = true;

		this->Unlock();
		m_pAppMainThread->Terminate();
		return true;
		}

	this->Unlock();
	return true;
	}

bool
SDSApplication::TerminateCommandThread( void )
	{
	this->Lock();

	if ( !m_bTerminatedCommandThread &&
		 (m_pAppCommandThread != NULL) )
		{
		m_bTerminatedCommandThread = true;

		this->Unlock();
		m_pAppCommandThread->Terminate();
		return true;
		}


	this->Unlock();
	return true;
	}

// ***************************************************************************
// ***************************************************************************
// ***************************************************************************
// ***************************************************************************
// ***************************************************************************
// ***************************************************************************
void SDS_cmdline(HWND hWnd) 
	{
    CString fname=ResourceString(IDC_ICADAPI_COMMAND_LINE_6, "Command Line" );

	SDSApplication *pBegin = new CEngineApplication( fname );

	ASSERT( pBegin->IsValid() );
	if ( pBegin == NULL )
		{
		return;
		}


	pBegin->StartMainEngineThread();
	
	
	}


int SDS_BroadcastRQToSDSApps(int RQcode) 
	{
	int ret=RTNORM;

	if(!SDSApplication::GetEngineApplication()->GetMainThread()->IsCurrent()) 
		{
		BroadcastRQJob *pJob = new BroadcastRQJob( RQcode );
		SDSThread *pMainThread = SDSApplication::GetEngineApplication()->GetMainThread();
		ASSERT( pMainThread != NULL );
		if ( pMainThread != NULL )
			{
			pMainThread->PostJob( pJob );
			}

		return ret;
		}

	SDSApplication *ptempApp;
	SDSApplication *pNextApp = NULL;
    for(ptempApp=SDSApplication::GetEngineApplication()->GetNextLink(); ptempApp!=NULL; ptempApp=pNextApp) 
		{
		ASSERT( CHECKSTRUCTPTR( ptempApp ) );
		ASSERT( ptempApp->IsValid() );

		SDS_LoadedAppsLast=SDSApplication::GetEngineApplication();
		ptempApp->SetRQCode( RQcode );

		// Don't use ptempApp after the thread swap, which may have destroyed it
		//
		pNextApp = ptempApp->GetNextLink();

		// ****************************************
		// THREADSWAP !!!!!!!!!!!!!!!!!
			{
			SDSApplication *pStartApp = ptempApp;
			SDSApplication *pStopApp = SDSApplication::GetEngineApplication();
			if(FALSE==SDS_ThreadSwap( pStartApp->GetMainThread(), pStopApp->GetMainThread())) 
				{
				ret=RTERROR;
				}
			}

		// Safety check
		//
		if ( (RQcode != SDS_RQQUIT ) &&
			 (CHECKSTRUCTPTR( ptempApp ))	)
			{
			ptempApp->SetRQCode( 0 );
			}
		}
	return(ret);
	}

/**************************************************************************/
/************************* Thread Functions *******************************/
/**************************************************************************/


struct SDS_LastStack 
	{
	SDSApplication *Last;
	struct SDS_LastStack *next;
	} *SDS_LastStackBeg;

void SDS_PushLastThread(SDSApplication *Last) 
	{
	struct SDS_LastStack *tmp=new struct SDS_LastStack;
	tmp->Last=Last;
	tmp->next=SDS_LastStackBeg;
	SDS_LastStackBeg=tmp;
	}

SDSApplication *SDS_GetLastThread(void) 
	{
	if(SDS_LastStackBeg==NULL)
		{
		return(NULL);
		}

	SDSApplication *Last=SDS_LastStackBeg->Last;
	struct SDS_LastStack *tmp=SDS_LastStackBeg;
	SDS_LastStackBeg=SDS_LastStackBeg->next;
	delete tmp;
	return(Last);
	}




// *************************************************
// MainEngineThread
//
//
// Entry point for main command line thread
//
// SDS_LoadAppsBeg->m_pAppMainThread goes through here
//

class MainEngineThreadHandler : public SDSThreadHandler
	{
		// CTOR and DTOR
		//
		public:
			MainEngineThreadHandler( void )
				{
				}
			virtual ~MainEngineThreadHandler( void )
				{
				}
				
		 //-------------------
		 // INTERFACE
		 //
		 public:
		 	virtual bool ThreadHandlerProc( void );
		 	
		 // validity
		 //
		 protected:
		 	virtual bool isValid( void )
			 	{
			 	return true;
			 	}
		 		
	};
	
// Modified Cybage VSB 23/03/2001 [
// Reason: Fix for the ICAD.LSP bug reported Ronald Prepchuk
// email dated 21/03/2001. 
extern CEvent g_WaitEvent;
// Modified Cybage VSB 23/03/2001 ]

bool
MainEngineThreadHandler::ThreadHandlerProc( void )
	{
	// Initialize OLE for the command thread.  sds_regfunc functions may be called on
	// this thread and need ole init to be called for them
	//
	VERIFY( SUCCEEDED( ::OleInitialize( NULL ) ) );

	// Modified Cybage VSB 23/03/2001 [
	// Reason: Fix for the ICAD.LSP bug reported Ronald Prepchuk
	// email dated 21/03/2001. 
	::WaitForSingleObject(g_WaitEvent.m_hObject,INFINITE);
	// Modified Cybage VSB 23/03/2001 ]	

	::SDS_cmdthread( );

	// don't believe we ever get back here
	//
	ASSERT( false );

	return false;
	}

bool
SDSApplication::StartMainEngineThread( )	
	{
	ASSERT( m_pAppMainThread == NULL );
	
	m_pAppMainThread = new SDSThread( new MainEngineThreadHandler() );
	
	return m_pAppMainThread->Launch();
	}

	

// *************************************************
// LispCommandThread
//
// Entry point for the LISP command line thread
//
// SDS_LoadAppsBeg->m_pAppCommandThread goes through here
//

class LispCommandThreadHandler : public SDSThreadHandler
	{
		// CTOR and DTOR
		//
		public:
			LispCommandThreadHandler( void )
				{
				}
			virtual ~LispCommandThreadHandler( void )
				{
				}
				
		 //-------------------
		 // INTERFACE
		 //
		 public:
		 	virtual bool ThreadHandlerProc( void );
		 	
		 // validity
		 //
		 protected:
		 	virtual bool isValid( void )
			 	{
			 	return true;
			 	}
		 		
	};
	
// optimization has proven to break try/catch blocks
//
#pragma optimize( "", off )
bool
LispCommandThreadHandler::ThreadHandlerProc( void )
	{
	
	bool bFirstTime = true;
	
		// basically we'll only get out of here by throwing an exception
		//		
	while( true )
		{
		try
			{


			// If we get back here again, we need to do a thread swap
			//
			if ( !bFirstTime )
				{
				// Restart the main thread
				//
				// ****************************************
				// THREADSWAP !!!!!!!!!!!!!!!!!
				SDSApplication *pStartApp = SDSApplication::GetActiveApplication();
				SDSApplication *pStopApp = SDSApplication::GetActiveApplication();

				pStartApp->setCmdThreadError( true ); // we set this flag to interrupt 
													  // execution of LISP code in Engine thread
				SDS_ThreadSwap(pStartApp->GetMainThread(),pStopApp->GetCommandThread());
				}
			bFirstTime = false;

			SDS_cmdthread( );
			}
		catch( SDSThreadException theException )
			{
			IDS_GetThreadController()->SetNoThreadSwap( false );
			if ( SDSApplication::GetCurrentApplication() != NULL )
				{
			    SDSApplication::GetCurrentApplication()->SetCommandRunning( false );
				SDSApplication::GetCurrentApplication()->ClearLispCommandDepth();
				}

			if ( theException.GetReason() == SDSThreadException::RESTARTCOMMANDTHREAD )
				{

				// From here we'll re-enter the while loop, the try block and do a thread swap

				}	
			else
				{
				throw;		// <<<<<<< ------ EXIT POINT
				}	
			}
		}	
			
		

	// Don't ever get back here
	//
	ASSERT( false );

	return false;
	}
#pragma optimize( "", on )

bool
SDSApplication::StartLispCommandThread( )	
	{
	
	m_pAppCommandThread = new SDSThread( new LispCommandThreadHandler() );
	
	return m_pAppCommandThread->Launch();
	}

// *************************************************
// XAppCommandThread
//
// Entry point for any SDS App's CmdThread
//
// That is, m_pAppCommandThread will run through here
//
// Parameter pointer should have been created with new, and will
// be deleted
//

class XAppCommandThreadHandler : public SDSThreadHandler
	{
		// CTOR and DTOR
		//
		public:
			XAppCommandThreadHandler( void )
				{
				}
			virtual ~XAppCommandThreadHandler( void )
				{
				}
				
		 //-------------------
		 // INTERFACE
		 //
		 public:
		 	virtual bool ThreadHandlerProc( void );
		 	
		 // validity
		 //
		 protected:
		 	virtual bool isValid( void )
			 	{
			 	return true;
			 	}
		 		
	};
	
// optimization has proven to break try/catch blocks
//
#pragma optimize( "", off )
bool
XAppCommandThreadHandler::ThreadHandlerProc( void )
	{

		
	bool bFirstTime = true;
	
		// basically we'll only get out of here by throwing an exception
		//		
	while( true )
		{
		try
			{


			// If we get back here again, we need to do a thread swap
			//
			if ( !bFirstTime )
				{
				// Restart the main thread
				//
				// ****************************************
				// THREADSWAP !!!!!!!!!!!!!!!!!
				SDSApplication *pStartApp = SDSApplication::GetActiveApplication();
				SDSApplication *pStopApp = SDSApplication::GetActiveApplication();

				pStartApp->setCmdThreadError( true ); // we set this flag to interrupt 
													  // execution of LISP code in Engine thread
				SDS_ThreadSwap(pStartApp->GetMainThread(),pStopApp->GetCommandThread());
				}
			bFirstTime = false;

			SDS_cmdthread( );
			}
		catch( SDSThreadException theException )
			{
			if ( theException.GetReason() == SDSThreadException::RESTARTCOMMANDTHREAD )
				{
		        SDSApplication::GetCurrentApplication()->SetCommandRunning( false );
				SDSApplication::GetCurrentApplication()->ClearLispCommandDepth();

				// From here we'll re-enter the while loop, the try block and do a thread swap

				}	
			else
				{
				throw;		//  ------ EXIT POINT
				}	
			}
		}	
			
		

	// Don't ever get back here
	//
	ASSERT( false );

	return false;
 	}
#pragma optimize( "", on )



bool
SDSApplication::StartXAppCommandThread( )	
	{
	ASSERT( m_pAppCommandThread == NULL );
	
	m_pAppCommandThread = new SDSThread( new XAppCommandThreadHandler() );
	
	return m_pAppCommandThread->Launch();
	}


// *************************************************
// XAppMainThread
//
// Call the entry point in an SDS App's DLL
//
// This is where m_pAppMainThread runs through
//
// Parameter pointer should have been created with new, and will
// be deleted
//


class XAppMainThreadHandler : public SDSThreadHandler
	{
		// CTOR and DTOR
		//
		public:
			XAppMainThreadHandler( SDSApplication *pApp, LPTHREAD_START_ROUTINE pEntryFunc )
				{
				m_pApplication = pApp;
				m_pEntryFunc = pEntryFunc;
				}
			virtual ~XAppMainThreadHandler( void )
				{
				}
				
		 //-------------------
		 // INTERFACE
		 //
		 public:
		 	virtual bool ThreadHandlerProc( void );
		 	
		// validity
		//
		protected:
		 	virtual bool isValid( void )
			 	{
			 	return true;
			 	}
			
		// -------------------
		// internal data
		//
		private: 	
			SDSApplication *m_pApplication;
			LPTHREAD_START_ROUTINE m_pEntryFunc;
		 		
	};

// optimization has proven to break try/catch blocks
//
#pragma optimize( "", off )	
bool
XAppMainThreadHandler::ThreadHandlerProc( void )
	{
	ASSERT( CHECKFUNCPTR( m_pEntryFunc ) );
	if ( CHECKFUNCPTR( m_pEntryFunc ) )
		{
		// *******************************
		// sds_exit() will throw SDSThreadException to break back up to the top
		//
		try
			{
			m_pEntryFunc( NULL );
			}
		catch( SDSThreadException theException )
			{
			// Some chance that by the time we return from EntryFunc that
			// pThisApp has already been freed
			//
			ASSERT( CHECKSTRUCTPTR( m_pApplication ) );

			m_pApplication->GetMainThread()->ClearJobHandler();
			if ( !m_pApplication->AppIsUnloading() )
				{
				// sds_xunload will definitely clean out pThisApp
				//
				::sds_xunload_application( m_pApplication );
				}
			// Resume the thread that is making the xunload call
			//
			else if ( theException.GetReason() == SDSThreadException::APPEXIT )
				{
				if ( SDS_LoadedAppsLast != NULL )
					{
					if ( SDS_LoadedAppsLast->GetMainThread()->IsValid() )
						{
						SDS_LoadedAppsLast->GetMainThread()->Resume();
						}
					}
				}

			throw; // <<<< ---------- EXIT UP TO NEXT LEVEL
			} // end of try/catch
		}



	return true;
	}


#pragma optimize( "", on )	





bool
SDSApplication::StartXAppMainThread( LPTHREAD_START_ROUTINE pEntryFunc )	
	{


	ASSERT( m_pAppMainThread == NULL );

	m_pAppMainThread = new SDSThread( new XAppMainThreadHandler( this, pEntryFunc ) );
	
	return m_pAppMainThread->Launch();
	}





// ---------------------------
// Helper class -- Terminate all external applications
//
//
class UnloadAllXAppsJob : public SDSThreadJob
	{
		// ------------------------------
		// CTOR and DTOR
		//
		public:
			UnloadAllXAppsJob( void )
				{
				}
		
			virtual int Execute( void )
				{

				// The whole purpose is to make sure we are on the main engine thread, so ASSERT that fact
				//
				ASSERT( SDSApplication::GetEngineApplication()->GetMainThread()->IsCurrent() );

			    SDSApplication *cur;

			    for(cur=SDSApplication::GetEngineApplication()->GetNextLink(); cur!=NULL; ) 
					{
	
					::sds_xunload_application( cur );
				  
					cur = SDSApplication::GetEngineApplication()->GetNextLink();
				    }
				return 1;
				}
		protected:
			virtual bool isValid( void )
				{
				return true;
				}

	};


// *************************************
// Unload all the external applications
//
bool
SDSApplication::UnloadAllExternalApps( void )
	{

	ASSERT( SDSApplication::GetEngineApplication() != NULL );
	ASSERT( SDSApplication::GetEngineApplication()->GetMainThread() != NULL );

	SDSThread *pMainThread = SDSApplication::GetEngineApplication()->GetMainThread();

	UnloadAllXAppsJob	*pJob = new UnloadAllXAppsJob;

	int retval = pMainThread->SendJob( pJob );


	// Should be no apps loaded after this
	//
	ASSERT( SDSApplication::GetEngineApplication()->GetNextLink() == NULL );

	return true;
	}

// CmdThread error functions. These functions are used in sdsengine_cmd() and
// ThreadHandlerProc() to allow the CommandThread to signal to the MainThread
// of ActiveApplicationif error took place while executing command
void SDSApplication::setCmdThreadError( bool bErr )
{
	m_bCmdThreadError = bErr;
}

bool SDSApplication::getCmdThreadError()
{
	return m_bCmdThreadError;
}

// *************************************************
// SDS_CreateThread
//
// Purpose
// Mirrors the CreateThread function, but allows us to control exactly
// how it is done.  In particular, the C runtime wants us to call _beginthread
// instead of CreateThread, so now we do
//
HANDLE SDS_CreateThread( LPSECURITY_ATTRIBUTES lpThreadAttributes,
							DWORD dwStackSize, // initial thread stack size, in bytes
							LPTHREAD_START_ROUTINE lpStartAddress,  // pointer to thread function
							LPVOID lpParameter,  // argument for new thread
							DWORD dwCreationFlags, // creation flags
							LPDWORD lpThreadId  // pointer to returned thread identifier
							)
	{
		return (HANDLE)_beginthreadex(lpThreadAttributes,
										dwStackSize,
										(unsigned int (__stdcall *)(void *))lpStartAddress,
										lpParameter,
										dwCreationFlags,
										(unsigned int *)lpThreadId );

	}
	


extern int		       SDS_AtThreadSwapCount;



int SDS_ThreadSwap( SDSThread *pStartThread, SDSThread *pStopThread) 
	{
	ASSERT( !pStopThread->IsSame( pStartThread ) );
	ASSERT( !pStartThread->IsCurrent() );
	ASSERTBREAK( pStopThread->IsCurrent() );


	if ( pStartThread->IsCurrent() )
		{
		return TRUE;
		}

	static SingleThreadSection theCriticalSection;
	static int nSwap;


	nSwap++;
	SDS_AtThreadSwapCount++;
	int    bitsets=SDSApplication::GetActiveApplication()->GetInputControlBits();

	// ****************************
	// The danger with doing ResumeThread();SuspendThread(); is that the other thread
	// will resume, and re-enter here before the current thread is Suspend'd.  The following
	// combination of Sleep and critical sections minimizes but does not eliminate the problem.  
	// The real solution is to not do SuspendThread and ResumeThread...that's coming
	//
	//-------------------------------
	theCriticalSection.Enter();
	Sleep( 0 );
	pStartThread->Resume();
	theCriticalSection.Leave();

	pStopThread->Suspend();
	Sleep( 0 );
	//-------------------------------

	SDSApplication::GetActiveApplication()->SetInputControlBits( bitsets );
	SDS_AtThreadSwapCount--;
	nSwap--;

    return(TRUE);
	}


CEngineApplication::CEngineApplication( const char* appname ) : SDSApplication( appname )
{
	SDSApplication::pEngineApplication = this;
	
	// Create standard OSNAP modes and add them to the array
	m_osnapManager.initializeStandardModes( this );	
}

CEngineApplication::~CEngineApplication()
{
	//clean up
	m_osnapManager.removeAppModes( this );
}
