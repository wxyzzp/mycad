/* SDSTHREAD.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * A thread
 * 
 */ 
#include "Icad.h"/*DNT*/
#include "IcadApi.h"/*DNT*/
#include <process.h>
#include "inputeventqueue.h"/*DNT*/
#include "sdsthreadexception.h"/*DNT*/
#include "sdsthreadhandler.h"/*DNT*/
#include "sdsthreadjob.h"/*DNT*/
#include "sdsthreadjobsink.h"/*DNT*/
#include "sdsthread.h"/*DNT*/
#include "lockableobject.h"/*DNT*/
#include "ic_dynarray.h"/*DNT*/



// ****************************************************************
// CLASS SDSThreadCollection
//
class SDSThreadCollection : public BaseLockableObject
	{

	// -------------------
	// CTORS and DTORS
	//
	public:
		virtual ~SDSThreadCollection( void );

	public:
		bool Add( SDSThread *pThread );
		bool Remove( SDSThread *pThread );
		SDSThread *Find( DWORD dwThreadId );
		bool IsEmpty();

	protected:
		bool isValid( void )
			{
			return true;
			}
	private:
		ic_dynarray<SDSThread *> m_pThreadArray;

	};

// ----------------------
// DTOR
//
SDSThreadCollection::~SDSThreadCollection( void )
	{
	
	while( m_pThreadArray.Number() > 0 )
		{
		m_pThreadArray.Remove( 0 );
		}
	}

// -------------------------
// PUBLIC METHOD
//
bool
SDSThreadCollection::Add( SDSThread *pThread )
	{
	SYNCHRONIZEMETHOD();

	m_pThreadArray.Add( pThread );
	return true;
	}

// -------------------------
// PUBLIC METHOD
//
bool
SDSThreadCollection::Remove( SDSThread *pThread )
	{
	SYNCHRONIZEMETHOD();
	bool bRetval = false;


	int iCount = m_pThreadArray.Number();
	int i;
	for( i = 0; i < iCount; i++ )
		{
		SDSThread *pRetthread;
		if ( m_pThreadArray.Get(i,&pRetthread) && pRetthread == pThread )
			{
			m_pThreadArray.Remove( i );
			bRetval = true;
			break;
			}
		}

	return bRetval;
	}


// ---------------------------
// PUBLIC METHOD
//
SDSThread *
SDSThreadCollection::Find( DWORD dwThreadId )
	{
	SYNCHRONIZEMETHOD();

	SDSThread *pRetval = NULL;
	int iCount = m_pThreadArray.Number();
	int i;
	for( i = 0; i < iCount; i++ )
		{
		SDSThread *pThread;
		if (m_pThreadArray.Get(i, &pThread)==false) {
			ASSERT(false);
			return NULL;
		}
		if ( pThread->GetThreadID() == dwThreadId )
			{
			pRetval = pThread;
			break;
			}
		}

	return pRetval;
	}

// -------------------------
// PUBLIC METHOD
//
bool
SDSThreadCollection::IsEmpty( )
	{
	SYNCHRONIZEMETHOD();
	bool bRetval = false;


	int iCount = m_pThreadArray.Number();
	if (iCount == 0)
		bRetval = true;

	return bRetval;
	}


// *********************************************************************
// *********************************************************************


// *******************************************************
// Thread Procedure
//
// This is a friend of SDSThread
//

// optimization has proven to be unreliable around try/catch blocks
//
#pragma optimize( "", off )
static UINT
zThreadProcedure( SDSThread *pThread )
	{
	
	ASSERTBREAK( pThread->IsCurrent() );

	// Creating the JobSink must be done after the thread has started and on the
	// thread.  This is an implementation detail having to do with the job sink
	// containing a Window and we need the message queueu for this window to come
	// from this thread
	//
	try
		{

		pThread->m_pThreadJobSink = ::CreateSDSThreadJobSink();
		ASSERTBREAK( pThread->m_pThreadJobSink != NULL );
	
		pThread->Suspend();
#if 1//rk 04
		if (!IDS_IsIcadExiting())
		{
			ASSERTBREAK( pThread->IsValid() );
			
			pThread->m_pThreadHandler->ThreadHandlerProc();
		}
#else
		ASSERTBREAK( pThread->IsValid() );

		pThread->m_pThreadHandler->ThreadHandlerProc();
#endif
		}
	catch( SDSThreadException theException )
		{
		SDSThreadException::Reason theReason = theException.GetReason();

		if ( theReason == SDSThreadException::APPEXIT )
			{
			SDSThread *pThisThread = SDSThread::GetCurrentThread();
			if ( pThisThread != NULL )
				{

				ASSERT( CHECKSTRUCTPTR( pThisThread ) );
				pThisThread->ClearJobHandler();
				} // end of try/catch
			}
		}

	return 0;
	}	
#pragma optimize( "", off )

// ************************************************
// ************************************************
// CLASS DATA
//
SDSThreadCollection *SDSThread::pAllThreads = NULL;

// *********************************************************************
// CLASS METHOD (i.e. static)
//
SDSThread *
SDSThread::GetCurrentThread( void )
	{

	DWORD dwThreadID = ::GetCurrentThreadId();

	return SDSThread::pAllThreads->Find( dwThreadID );
	}


// *********************************************************************
// CLASS METHOD (i.e. static)
//
SDSThread *
SDSThread::GetThread( DWORD dwThreadID )
	{

	return SDSThread::pAllThreads->Find( dwThreadID );
	}

// *********************************************************************
// *********************************************************************
// END OF STATIC METHODS
// *********************************************************************
// *********************************************************************

// ************************************************
// CTOR
//
SDSThread::SDSThread( SDSThreadHandler *pThreadHandler )
	{

	if ( SDSThread::pAllThreads == NULL )
		{
		SDSThread::pAllThreads = new SDSThreadCollection;
		}

	ASSERT( pThreadHandler->IsValid() );
	
	m_pThread = NULL;
	m_dwThreadID = 0;
	m_pThreadHandler = pThreadHandler;
	m_pThreadHandler->setThread( this );

	// The thread job sink must be created on this thread, after it has started.
	// See zThreadProcedure
	//
	m_pThreadJobSink = NULL;
	m_hResumeEvent = NULL;

	SDSThread::pAllThreads->Add( this );
	}

SDSThread::~SDSThread( void )
	{
	
	SDSThread::pAllThreads->Remove( this );

	if (SDSThread::pAllThreads->IsEmpty())
		{
		delete SDSThread::pAllThreads;
		SDSThread::pAllThreads = NULL;
		}

	delete m_pThread;
	
	if ( m_pThreadHandler != NULL )
		{
		delete m_pThreadHandler;
		m_pThreadHandler = NULL;
		}

	if ( m_pThreadJobSink != NULL )
		{
		delete m_pThreadJobSink;
		}
	m_pThreadJobSink = NULL;

	::CloseHandle( m_hResumeEvent );
	m_hResumeEvent = NULL;
	}	
	
// *********************
// Basic Accessor method
//
DWORD
SDSThread::GetThreadID( void ) const
	{
	return m_dwThreadID;
	}

// *************************************************
//
bool 
SDSThread::Launch( void )
	{
		m_hResumeEvent = ::CreateEvent( NULL,	// security attributes
											FALSE,	// this means an auto-reset object
											FALSE,	// initially in non-signaled state
											NULL	// unnamed object (we'll just keep the handle)
										  );

		ASSERT( m_hResumeEvent != NULL );
		if ( m_hResumeEvent )
			{
		
		    m_pThread = AfxBeginThread( (UINT(*)(LPVOID))zThreadProcedure,
										this,				// pass self to zThreadProcedure
										THREAD_PRIORITY_NORMAL,
										0,
										CREATE_SUSPENDED );

			ASSERT( m_pThread != NULL );
			
			// Prevent auto deleting
			m_pThread->m_bAutoDelete = FALSE;

			m_dwThreadID = m_pThread->m_nThreadID;
			m_pThread->ResumeThread();
			}
										
		return ( m_pThread != NULL );
	}



void
SDSThread::Resume( void )
	{
	// We shouldn't be continuing ourselves
	//
	ASSERTBREAK( !this->IsCurrent() );
	
	if ( !this->IsCurrent() )
		{
		this->signalResume();
		}
	}	
	
void 
SDSThread::Suspend( void )
	{
	// We should only be telling ourselves to wait
	//
	ASSERTBREAK( this->IsCurrent() );


	// If we're not the current thread, who are we.  In anycase, we might go to sleep
	// forever, so just say NO!
	//
	if ( this->IsCurrent() )
		{
		::ProcessMsgWaitForObject( this->m_hResumeEvent );
		}
	}



int 
SDSThread::SendJob( SDSThreadJob *pJob )
  	{
  	if ( m_pThreadJobSink != NULL )
  		{
  		return m_pThreadJobSink->SendJob( pJob );
  		}
  	else
  		{
  		// Still need to delete the job
  		//
  		delete pJob;
  		return 0;
  		}
  	}

void 
SDSThread::PostJob( SDSThreadJob *pJob )
	{
	if ( m_pThreadJobSink != NULL )
		{
		m_pThreadJobSink->PostJob( pJob );
		}
	else
		{
		// Still need to delete the job
		//
		delete pJob;
		}
	}

// ************************************************************************
// ClearJobHandler gets rid of the job handler so we know not to send more
// jobs.  Do this when the thread is shutting down
//
void 
SDSThread::ClearJobHandler( void )
	{
	// Get rid of the thread job sink immediately, so we can know to stop
	// sending things to it
	//
	SDSThreadJobSink *pSink = m_pThreadJobSink;
	m_pThreadJobSink = NULL;

	if ( pSink != NULL )
		{
		delete pSink;
		}
	}
// ******************************
//	
class TerminateJob : public SDSThreadJob	
	{
		// ------------------------------
		// CTOR and DTOR
		//
		public:
			TerminateJob( DWORD dwThreadId )
				{
				this->m_dwThreadId = dwThreadId;
				}
		
			virtual int Execute( void )
				{
				// If Job came from another thread, let's reply before killing ourselves
				//
				::ReplyMessage( 1 );
				delete this;
				AfxEndThread( 0 );
				return 1;
				}
				
		protected:
			virtual bool isValid( void )
				{
				return ( m_dwThreadId != 0 );
				}

		// --------------------
		// internal data
		//
		private:
			DWORD m_dwThreadId;
	};
	
void
SDSThread::Terminate( void )
	{
	if ( this->IsCurrent() )
		{
		throw SDSThreadException( SDSThreadException::TERMINATED );
		}
	else
		{
		// AG: handle to wait for temination
		HANDLE hThread = m_pThread->m_hThread;

		TerminateJob *pJob = new TerminateJob( GetThreadID() );
		SendJob( pJob );

		// AG: when SDSThread gets terminated this way it should call ::ReplyMessage( 1 )
		// to avoid sticking here. But it can lead to a problem if actual termination of
		// this thread (by calling AfxEndThread( 0 ) inside a thread) happens with some delay
		// after replying message(it's quite possible in debugging mode or thread preemting).
		// In this case race conditions are possible and can lead to a total crash.
		// For external sds-apps this could result in calling FreeLibrary before
		// proper termination of its threads. Because all these events get DllMain called with
		// appropriate fdReason flag and some of CRT and MFC implementations use this point
		// for initialization/deinitialization purposes, this behaviour is not eligible.
		// So we have to wait for a proper termination of the thread using WIN32 technique.
		
		::WaitForSingleObject( hThread, INFINITE);

		// now it's safe to return
		return;
		}
	}
	
	

bool
SDSThread::IsSame( const SDSThread *pOther )
	{
	
	return ( this->GetThreadID() == (pOther->GetThreadID()) );
	}	

bool
SDSThread::IsSame( DWORD dwThreadID )
	{

	return ( this->GetThreadID() == dwThreadID );
	}	

bool
SDSThread::IsCurrent( void )
	{	
	return IsSame( ::GetCurrentThreadId() );
	}

bool
SDSThread::signalResume( void )
	{
	VERIFY( ::SetEvent( m_hResumeEvent ) );
	
	return true;
	}
	
bool
SDSThread::isValid( void )
	{
	if ( !CHECKSTRUCTPTR( this ) )
		{
		return false;
		}

	if ( this->m_pThread == NULL )
		{
		return false;
		}
	   
	return true;
	}

// ****************************************************
// SDS_ExitThread
//
// Purpose
// Don't call Win32's ExitThread directly.
//
VOID SDS_ExitThread( DWORD dwExitCode // exit code for this thread
					)
	{
		_endthreadex( dwExitCode );
	}





