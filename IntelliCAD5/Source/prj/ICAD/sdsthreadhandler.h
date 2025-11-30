/* SDSTHREADHANDLER.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Abstracts the procedure that a thread runs
 * Used by SDSThread class
 * This is an ABSTRACT class
 * 
 */ 
#ifndef _SDSTHREADHANDLER_H
#define _SDSTHREADHANDLER_H


class SDSThread;

class SDSThreadHandler
	{
	
		friend SDSThread;
		
		// DTOR
		//
		public:
			virtual ~SDSThreadHandler( void )
				{
				return;
				};
		
	
		// ------------------------------------
		// PUBLIC INTERFACE 
		//
		public:
		
			// This is the method that will be run by the thread when it
			// starts up.  It will return when the process is finished.
			// It may throw exceptions!
			// In general it should return false iff it can't even start the
			// process.
			//
			virtual bool ThreadHandlerProc( void ) = 0;	
		
		
		// --------------------------------------
		// INTERFACE for its child classes
		//
		protected:
			SDSThread *GetThread( void )
				{
				return m_pParentThread;
				}
		
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		// special interface for SDSThread to use
		//
		private:
		
			// Used by SDSThread during startup
			void setThread( SDSThread *pParent )
				{
				m_pParentThread = pParent;
				}
			SDSThread *m_pParentThread;
			
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
			virtual bool isValid( void ) = 0;				
			
	};



	
#endif // _SDSTHREADHANDLER_H

