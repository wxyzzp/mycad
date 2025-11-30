/* THREADCONTROLLER.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Data for the threading that goes on in IntelliCAD.
 * This was just globals lying around, mostly in icadapi.cpp
 * This should change ALOT!
 * 
 */ 
#ifndef _THREADCONTROLLER_H
#define _THREADCONTROLLER_H

#include "IcadApi.h"

class IcadThreadCore;


class IcadThreadController
	{

	// CTORS and DTORS
	//
	public:
		IcadThreadController( void );
		~IcadThreadController( void );


	// Public Operations
	//
	public:
		bool GetAtWaitLoop( void );
		void SetAtWaitLoop( bool bValue );

		bool GetNoThreadSwap( void );
		void SetNoThreadSwap( bool bValue );

		bool GetDontThreadSwapCmd( void );
		void SetDontThreadSwapCmd( bool bValue );

		bool GetInsideCmdFunction( void );
		void SetInsideCmdFunction( bool bValue );

		DWORD GetWaitThreadID( void );
		void SetWaitThreadID( DWORD dwThreadId );

		DWORD GetMainThreadID( void );
		void SetMainThreadID( DWORD dwThreadId );

		SDSApplication *GetLoadedAppsBeg( void );
		void SetLoadedAppsBeg( SDSApplication *pApp );

		SDSApplication *GetLoadedAppsCur( void );
		void SetLoadedAppsCur( SDSApplication *pApp );

		SDSApplication *GetLoadedAppsAct( void );
		void SetLoadedAppsAct( SDSApplication *pApp );

		SDSApplication *GetLoadedAppsLast( void );
		void SetLoadedAppsLast( SDSApplication *pApp );

	private:

		IcadThreadCore *m_pCoreData;
	};



#endif // _THREADCONTROLLER_H
