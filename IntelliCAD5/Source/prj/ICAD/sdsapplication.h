/* SDSAPPLICATION.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Class that keeps track of sds applications that are loaded.
 * 
 */ 
#ifndef _SDSAPPLICATION_H
#define _SDSAPPLICATION_H

#ifndef _LOCKABLEOBJECT_H
	#include "lockableobject.h"
#endif

#include "osnapManagerImp.h"

class CommandQueue;
class SDSThread;

class SDSApplication : public BaseLockableObject
	{
	// ********************
	// CLASS METHODS
	//
	public:
		static SDSApplication *GetCurrentApplication( void );
		static SDSApplication *GetActiveApplication( void );
		
//		static bool SetEngineApplication( SDSApplication *pNewBegin );
		static SDSApplication *GetEngineApplication( void );
		

		// ---------------------------------
		// Used during shutdown
		//
		static bool UnloadAllExternalApps( void );
		
	// ********************
	// CLASS DATA
	//
	protected:
		static SDSApplication *pLoadedAppsAct;		
		static SDSApplication *pEngineApplication;

		
		
	// CTORS and DTORS
	//
	public:
		SDSApplication( const char *appname );
		virtual ~SDSApplication( void );

	public:
		const char *GetAppName( void )
			{
			return m_ApplicationName;
			}
		bool AppIsUnloading( void )
			{
			return m_bUnloading;
			}
		void SetAppIsUnloading( void )
			{
			m_bUnloading = true;
			}

		int CallCallBack( int flag,void *arg1,void *arg2,void *arg3 );
		bool SetCallBack( int (*cbfnptr)(int flag,void *arg1,void *arg2,void *arg3) );


		SDSApplication *GetNextLink( void );
		SDSApplication *AddLink( SDSApplication *pNewLink );
		static bool RemoveLink( SDSApplication *pOldLink );
        
        int     IsKeywordsEmpty();
        void    ClearKeywords();
        LPCTSTR GetKeywords();
        int     SetKeywords(char *kwords);

		int		GetLispCommandDepth();
		void	IncrementLispCommandDepth();
		void	DecrementLispCommandDepth();
		void	ClearLispCommandDepth();

		int		GetCommandDepth( void );
		void	IncrementCommandDepth( void );
		void	DecrementCommandDepth( void );

		bool	AcceptParenOrCR( void );
		void	SetAcceptParenOrCR( bool bValue );

		int		GetInputControlBits( void );
		void	SetInputControlBits( int bits );

		bool	IsCommandRunning( void )
					{
					return this->m_bCommandRunning;
					}
		void	SetCommandRunning( bool bValue )
					{
					this->m_bCommandRunning = bValue;
					}
		bool	IsAppLoaded( void )
					{
					return this->m_bAppLoaded;
					}
		void	MarkAppAsLoaded( void )
					{
					this->m_bAppLoaded = true;
					}
		void	SetRQCode( int code )
					{
					this->m_iRQcode = code;
					}
		int		GetRQCode( void )
					{
					return this->m_iRQcode;
					}

		CommandQueue *GetCommandQueue( void )
			{
			return this->m_pCommandQueue;
			}

	
		SDSThread *GetMainThread();
		SDSThread *GetCommandThread();

		bool TerminateMainThread( void );
		bool TerminateCommandThread( void );
		

		bool StartMainEngineThread();
		bool StartLispCommandThread();
		bool StartXAppMainThread( LPTHREAD_START_ROUTINE pEntryFunc );
		bool StartXAppCommandThread();

		void setCmdThreadError( bool bErr );
		bool getCmdThreadError();
	  

	// ----------------------------
	// validity
	//
	protected:
		bool isValid( void );


	// *****************
	// DATA
	//
	public:

		
	    char    thekeyword[IC_ACADBUF];

	// private internal data
	//
	private:
		CommandQueue	*m_pCommandQueue;


	    int     m_iRQcode;

	    bool    m_bAppLoaded;
	    bool	m_bCommandRunning;

		// These are the bits set by sds_initget()
	    int     m_iInputControlBits;


	    bool     m_bAcceptParenOrCR;
		int     m_icmddepth;
		int     m_ilispcmddepth;

	    CString  keywords;
		int		m_iCheckVal;   // Set to magic to keep track of memory overwrites, bad pointers etc.

		char   *m_ApplicationName;
		bool	m_bUnloading;

		// Pointer to callback function for this application
		//
	    int   (*m_CallBackFunctionPtr)(int flag,void *arg1,void *arg2,void *arg3);

	
		SDSThread *		m_pAppMainThread;
		bool			m_bTerminatedMainThread;

		SDSThread *		m_pAppCommandThread;
		bool			m_bTerminatedCommandThread;

		// Linked list pointer
		//
	    SDSApplication *m_pNextLink;

		// This flag is set if the error in the command thread happened
		bool			m_bCmdThreadError;


	};


class CEngineApplication : public SDSApplication
{
public:
	CEngineApplication( const char* appname );
	virtual ~CEngineApplication();

private:
	COsnapManagerImp m_osnapManager;	// the only OSNAP manager

public:
	COsnapManagerIcad* getOsnapManager(){ return static_cast<COsnapManagerIcad*>(&m_osnapManager); }

};



void           SDS_PushLastThread(SDSApplication *Last);
SDSApplication *SDS_GetLastThread(void);


int sds_xunload_application( SDSApplication *pThisApp );

#endif // _SDSAPPLICATION_H

