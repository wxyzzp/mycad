// ICADINPUTEVENTQUEUE.CPP
// Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
// 
// Abstract
//
//	This implements the input event queue interface for the specific case of Icad
// This class could be easily generalized, but I'll do that when I need it.
//
//  Basically this is class from any thread to put messages which will be picked up and
// processed later by any thread (probably another one, but could be the same one).  The basic API
// is this:
//		PostEvent
//		SendEvent			-- not currently implementd
//		PeekEvent		
//		WaitEvent
// 
// NOTE:  A SignalCancel method is also implemented by not tested!!
//
// The class maintains a queue of event objects.  If the reader calls WaitEvent and there are no
// events that match what its looking for it waits until one shows up.  While it is waiting, the thread
// processes windows messages!  The writer notifies the reader that a new element has been added to the
// queue by using a Win32 Event Synchronization object (this Event must not be confused with out InputEvent objects!).
// 
//
#include "Icad.h"/*DNT*/
#include "inputevent.h"/*DNT*/
#include "inputeventfilter.h"/*DNT*/
#include "inputeventqueue.h"/*DNT*/
#include "icadinputeventqueue.h"/*DNT*/  


#ifndef _IC_DYNARRAY_H
	#include "ic_dynarray.h"
#endif




// ******************************
// Concrete Class which implements the exported InputEventQueueInterface
//

class IcadInputEventQueue : public InputEventQueueInterface
	{
	// ----------------------------
	// internal constants
	//
	private:

		// This tells us about the queue
		//
		enum EventQueueState
			{
			EQS_BAD = 0,			// probably uninitialized
			EQS_NORMAL = 1,		// normal
			EQS_CANCEL = 2,		// we've been told to cancel
			EQS_SHUTDOWN = 3
			};


	// CTORS and DTORS
	//
	public:
		IcadInputEventQueue( void );
		virtual ~IcadInputEventQueue( void );



	// PUBLIC OPERATIONS
	//
	public:
		virtual bool PostEvent( InputEvent *pEvent );
		virtual bool SendEvent( InputEvent *pEvent );

		virtual InputEvent *PeekEvent( InputEventFilter *pEventFilter );
		virtual InputEvent *WaitEvent( InputEventFilter *pEventFilter );


		virtual void	SignalCancel( void );

		// This method tells the queue to stop accepting events.  This should only be called right before the
		// queue goes away
		//
		virtual void	DestroySelf( void )
			{
			delete this;
		   	}
		
		virtual void	Shutdown( void );

		virtual bool IsWaiting() { return (m_nWaitCount ? true : false); }

	// sanify check infrastructure
	protected:
		virtual bool isValid( void );


	// internal helper methods
	protected:
		// let the reader know something is there
		//
		bool	signalEvent( void );

		// empty out the queue
		//
		void	flushQueue( void );

		InputEvent *getEvent( InputEventFilter *pEventFilter );

		void processMessagesWhileWaiting( void );


	// ****************************
	// INTERNAL DATA MEMBERS
	//
	private:

		// Just checking...
		//
		EventQueueState					m_ourState;

		// How many threads are waiting???
		//
		long							m_nWaitCount;

		// Granularity of the queue is an arbitrary guess
		//
		ic_dynarray< InputEvent * >	m_theQueue;

		// This is a WIN32 Event Synchronization object, not in any way to be confused
		// with our InputEvent objects
		//
		HANDLE						m_hSynchroEvent;

	};	  // end of class IcadInputEventQueue



// -----------------------------------------
// CTOR -- class IcadInputEventQueue
//
IcadInputEventQueue::IcadInputEventQueue( void )
	{
	m_ourState = EQS_BAD;
	m_nWaitCount = 0;

	m_hSynchroEvent = ::CreateEvent( NULL,	// security attributes
											FALSE,	// this means an auto-reset object
											FALSE,	// initially in non-signaled state
											NULL	// unnamed object (we'll just keep the handle)
										  );

	if ( m_hSynchroEvent != NULL )
		{
		m_ourState = EQS_NORMAL;
		}

	}

// ------------------------------------------
// DTOR -- class IcadInputEventQueue
//
IcadInputEventQueue::~IcadInputEventQueue( void )
	{
	Shutdown();
	m_ourState = EQS_BAD;

	::CloseHandle( m_hSynchroEvent );
	m_hSynchroEvent = NULL;
	}

// -----------------------------------------
// PUBLIC METHOD -- class IcadInputEventQueue
//
//	Just put the event in the queue and get on with it
//
// NOTE:  This method should NOT block except for very short periods
//
bool
IcadInputEventQueue::PostEvent( InputEvent *pEvent )
	{
	SYNCHRONIZEMETHOD();

	if ( m_ourState != EQS_NORMAL )
		{
		return false;
		}

	m_theQueue.Add( pEvent );
	VERIFY( ::SetEvent( m_hSynchroEvent ) );

	return true;
	}

// ---------------------------------------
// PUBLIC METHOD -- class IcadInputEventQueue
//
//  Put the event in the queue and wait until it is processed
// or the whole queue is canceled
//
bool
IcadInputEventQueue::SendEvent( InputEvent *pEvent )
	{

	// IcadInputEventQueue is not designed for synchronous calls and shouldn't implement
	// SendEvent
	//
	ASSERT( false );

	return false;
	}

// ---------------------------------------
// PUBLIC METHOD -- class IcadInputEventQueue
//
// NOTE:  This method should NOT block except for very short periods
//
InputEvent *
IcadInputEventQueue::PeekEvent( InputEventFilter *pFilter )
	{
	InputEvent *pRetval = NULL;

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// Be careful, we don't want to conflict with WaitEvent and cause this to 
	// block indefinitely
	//

	pRetval = getEvent( pFilter );

	return pRetval;
	}

// ---------------------------------------
// PUBLIC METHOD -- class IcadInputEventQueue
//
// NOTE:  This method could block indefinitely
// WARNING WARNING WARNING --- This method may block on
// m_hSynchroEvent, so be careful about using the BaseLockableObject lock also
// or we could end up in deadlock
//
InputEvent *
IcadInputEventQueue::WaitEvent( InputEventFilter *pFilter )
	{

	ASSERT( IsValid() );

	InputEvent *pRetval = getEvent( pFilter );

	// Now this is where it gets complicated
	//
	while ( ( m_ourState == EQS_NORMAL ) &&
		    ( pRetval == NULL ) )
		{

		// start waiting !!!!!!!!!!!!
		::InterlockedIncrement( &m_nWaitCount );



		while ( (m_ourState == EQS_NORMAL) &&
			    (pRetval == NULL) )
			{
			processMessagesWhileWaiting();

			pRetval = getEvent( pFilter );
			}

		// end waiting !!!!!!!!!!!!!
		::InterlockedDecrement( &m_nWaitCount );

		}

	return pRetval;
	}

// ------------------------------------------
// PUBLIC METHOD -- class IcadInputEventQueue
//
//
void
IcadInputEventQueue::SignalCancel( void )
	{
	SYNCHRONIZEMETHOD();

	m_ourState = EQS_CANCEL;
	flushQueue();


	// arbitrary sanity check
	//
	ASSERT( m_nWaitCount < 1000 );
	ASSERT( m_nWaitCount >= 0 );

	int iCount = m_nWaitCount;

	for( int i = 0; i < iCount; i++ )
		{
		::SetEvent( m_hSynchroEvent );
		}

	// Okay, everybody was told we cancel'd and got out
	// Now reset the state
	//
	m_ourState = EQS_NORMAL;
	}

// *******************************************
// PUBLIC METHOD -- class IcadInputEventQueue
//
//
void
IcadInputEventQueue::Shutdown( void )
	{
	SYNCHRONIZEMETHOD();

	SignalCancel();

	m_ourState = EQS_SHUTDOWN;

	}

// ****************************************************
// INTERNAL METHODS 
//

// -------------------------
// INTERNAL METHOD -- class IcadInputEventQueue
//
// NOTE:  This method should NOT block except for very short periods
//
InputEvent *
IcadInputEventQueue::getEvent( InputEventFilter *pFilter )
	{

	// Actual queue contents manipulation must be synchronized
	//
	SYNCHRONIZEMETHOD();

	InputEvent *pRetval = NULL;
	if ( (m_theQueue.Number() > 0) &&
		 (m_ourState == EQS_NORMAL) )
		{

		// Look for the first event that matches the filter
		//
		int i;
		for( i = 0; i < (int)m_theQueue.Number(); i++ )
			{
			InputEvent *pCurrent;
			if (m_theQueue.Get( i, &pCurrent )==false) {
				ASSERT(false);
				return NULL;
			}
			// NULL filter means match all
			//
			if ( (pFilter == NULL) ||
				 (pFilter->Match( pCurrent )) )
				{
				pRetval = pCurrent;
				ASSERT( pRetval->IsValid() );
				m_theQueue.Remove( i );
				break;
				}
			}
		}

	return pRetval;
	}

// -------------------------------
// INTERNAL METHOD -- class IcadInputEventQueue
//
//
void
IcadInputEventQueue::flushQueue( void )
	{

	SYNCHRONIZEMETHOD();

	int iNumber;
	while( (iNumber = m_theQueue.Number()) > 0 )
		{
		InputEvent *pItem;
		if (m_theQueue.Get( iNumber - 1 , &pItem)==false) {
			ASSERT(false);
			return;
		}
		m_theQueue.Remove( iNumber-1 );
		delete pItem;
		}
	}

// -----------------------------
// INTERNAL METHOD -- class IcadInputEventQueue
//
//
void 
ProcessMsgWaitForObject( HANDLE hSynchroObject ) 
	{

	// See docs for MsgWaitForMultipleObjects to understand this and other magic
	//
	static const DWORD GOTWINDOWSMESSAGE = (WAIT_OBJECT_0 + 1);

	DWORD dwWaitResult;

	do
		{
		MSG msg;

		// process any and all windows messages
		//
		while( ::PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
			{
			if ( !AfxGetApp()->PreTranslateMessage(&msg) )
				{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
				}
			}


		dwWaitResult = ::MsgWaitForMultipleObjects( 1, 				// Only one object to wait on
												&hSynchroObject,	// this is the one
													FALSE,			// wait for only one (redundant, since there is only one)
													INFINITE,		// wait until the sun dies
													QS_ALLINPUT	// TODO -- is this right????
													);
		}
	while( dwWaitResult == GOTWINDOWSMESSAGE );

	}
 
void
IcadInputEventQueue::processMessagesWhileWaiting( void )
	{

	::ProcessMsgWaitForObject( this->m_hSynchroEvent );
	}


// *************************
// sanity check
//
bool
IcadInputEventQueue::isValid( void )
	{
	if (!BaseLockableObject::isValid())
		{
		return false;
		}

	if ( m_hSynchroEvent == NULL )
		{
		return false;
		}

	if ( (m_ourState != EQS_NORMAL) &&
		 (m_ourState != EQS_CANCEL) &&
		 (m_ourState != EQS_SHUTDOWN) )
		{
		return false;
		}

	// TODO add more
	//
	return true;
	}




// ************************************************************
// ************************************************************
// ************************************************************
// ************************************************************
// ************************************************************


InputEventQueueInterface *
CreateIcadInputEventQueue( void )
	{

	return new IcadInputEventQueue;
	}

