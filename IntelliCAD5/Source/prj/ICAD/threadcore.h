/* THREADCORE.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Internal data for threading stuff
 * 
 */ 
#ifndef _THREADCORE_H
#define _THREADCORE_H

#include "IcadApi.h"

class IcadThreadController;

class IcadThreadCore
	{

	friend IcadThreadController;

	// CTORS and DTORS
	//
	public:
		IcadThreadCore();
		~IcadThreadCore();

	// internal data
	//
	private:
		bool			m_bAtWaitLoop;
		bool			m_bNoThreadSwap;
		bool		   m_bDontThreadSwapCmd;
		bool           m_bInsideCmdFunction;
		DWORD		   m_dwWaitThreadID;
		DWORD		   m_dwMainThreadID;

		//
		SDSApplication *m_pLoadedAppsBeg;
		SDSApplication *m_pLoadedAppsCur;
		SDSApplication *m_pLoadedAppsAct;
		SDSApplication *m_pLoadedAppsLast;

	};



#endif // _THREADCORE_H
