/* COMMANDATOM.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Implements the commandAtomObj class
 * 
 */ 

// THIS FILE HAS BEEN GLOBALIZED!

#include "lisp.h"/*DNT*/
#include "commandatom.h" /*DNT*/
#include "sdsthreadjob.h" /*DNT*/
#include "sdsthreadjobsink.h" /*DNT*/
#include "sdsthread.h" /*DNT*/
#include "sdsapplication.h" /*DNT*/


// ---------------------------
// Helper class -- used to dispatch the callback to another thread
//
class CallExternalFunctionJob : public SDSThreadJob
	{
		// ------------------------------
		// CTOR and DTOR
		//
		public:
			CallExternalFunctionJob( commandAtomObj *pCommand )
				{
				m_pCommand = pCommand;
				}
		
			virtual int Execute( void )
				{
				ASSERT( m_pCommand->ThreadID == ::GetCurrentThreadId() );

				m_pCommand->CallFunction();

				return 1;
				}
		protected:
			virtual bool isValid( void )
				{
				return (m_pCommand != NULL);
				}

		// --------------------
		// internal data
		//
		private:
			commandAtomObj *m_pCommand;
	};


bool
commandAtomObj::CallFunction( void )
	{

	ASSERT( m_funcptr != NULL );
	SDSThread *pthisThread = SDSThread::GetThread( this->ThreadID );

	// If pthisThread is NULL, we have no choice but to call directly.
	// If pthisThread is current, then it is desired and correct to call
	// directly
	//
	if ( (pthisThread == NULL) ||
		 (pthisThread->IsCurrent()) )
		{
		m_funcptr();
		}
	else
		{
		// else case means that the function should be called on that other thread
		//
		CallExternalFunctionJob *pJob = new CallExternalFunctionJob( this );

		pthisThread->SendJob( pJob );


		// This line of code has the side-effect of setting the last active application to
		// the current one, which is all to the good, though theoretically not necessary
		//
		SDSApplication::GetActiveApplication();

		}

	return true;
	}


