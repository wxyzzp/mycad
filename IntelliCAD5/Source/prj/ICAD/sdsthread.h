/* SDSTHREAD.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Abstracts a thread
 * 
 */ 
#ifndef _SDSTHREAD_H
#define _SDSTHREAD_H

class SDSThreadCollection;
class SDSThreadHandler;
class SDSThreadJobSink;
class SDSThreadJob;

// from MFC
//
class CWinThread;

class SDSThread
	{
	
	// This is not a hack.  This static procedure is really "part" of
	// the SDSThread class.  It is needed because I don't know how
	// to point a Thread Entry Proc right at a member function
	//
	friend static UINT zThreadProcedure( SDSThread *pThread );
	
	
	// CTORS and DTORS
	//
	public:
		SDSThread( SDSThreadHandler *pThreadHandler );
		virtual ~SDSThread( void );

		
		bool Launch( void );
	
		bool IsSame( const SDSThread *theOther );
		bool IsSame( DWORD dwThreadID );
		bool IsCurrent( void );
		
		void Resume( void );
		void Suspend( void );
		void Terminate( void );

		
		// ClearJobHandler gets rid of the job handler so we know not to send more
		// jobs.  Do this when the thread is shutting down
		//
		void ClearJobHandler( void );
		int SendJob( SDSThreadJob *pJob );
		void PostJob( SDSThreadJob *pJob );

		DWORD GetThreadID( void ) const;


	// ***********************
	// CLASS METHODS
	//
	public:
		static SDSThread *GetCurrentThread( void );
		static SDSThread *GetThread( DWORD dwThreadID );

	// -----------------------------		
	// helper methods
	//
	protected:
		bool	signalResume( void );
	
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

		
	protected:
		// Use MFC thread so we can use MFC in add-ons etc.
		//
		CWinThread *m_pThread;
		DWORD m_dwThreadID;	// This is redundant from m_pThread, but m_pThread has a different lifespan
		
		SDSThreadHandler *m_pThreadHandler;
		SDSThreadJobSink *m_pThreadJobSink;
	
		// This is a WIN32 Event Synchronization object, not in any way to be confused
		// with our InputEvent objects
		//
		HANDLE						m_hResumeEvent;

	// ********************************
	// CLASS DATA
	//
	private:
		static SDSThreadCollection *pAllThreads;
	
	};


int SDS_ThreadSwap( SDSThread *pStartThread, SDSThread *pStopThread);
	
#endif // _SDSTHREAD_H

