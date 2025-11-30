/* THREADCORE.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Internal data for threading stuff
 * 
 */ 

#include "Icad.h" /*DNT*/
#include "threadcore.h" /*DNT*/

// *******************************************
// CLASS IMPLEMENTATION -- class IcadThreadCore
//
//


// *******************
// PUBLIC CTOR -- class IcadThreadCore
//
//
IcadThreadCore::IcadThreadCore( void )
	{
	m_bNoThreadSwap = false;
	m_bAtWaitLoop = false;
	m_bDontThreadSwapCmd = false;
	m_bInsideCmdFunction = false;
	m_dwWaitThreadID = 0L;
	m_dwMainThreadID = 0L;

	m_pLoadedAppsBeg = NULL;
	m_pLoadedAppsCur = NULL;
	m_pLoadedAppsAct = NULL;
	m_pLoadedAppsLast = NULL;

	}


// ********************
// PUBLIC DTOR -- class IcadThreadCore
//
//
IcadThreadCore::~IcadThreadCore( void )
	{
	m_bNoThreadSwap = false;
	m_bAtWaitLoop = false;
	m_bDontThreadSwapCmd = false;
	m_bInsideCmdFunction = false;
	m_dwWaitThreadID = 0L;
	m_dwMainThreadID = 0L;

	m_pLoadedAppsBeg = NULL;
	m_pLoadedAppsCur = NULL;
	m_pLoadedAppsAct = NULL;
	m_pLoadedAppsLast = NULL;

	}


