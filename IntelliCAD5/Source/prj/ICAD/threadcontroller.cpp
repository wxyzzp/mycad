/* THREADCONTROLLER.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Internal data for threading stuff
 * 
 */ 

#include "Icad.h" /* DNT */
#include "threadcore.h" /* DNT */
#include "threadcontroller.h" /* DNT */

// *******************************************
// CLASS IMPLEMENTATION -- class IcadThreadController
//
//


// *******************
// PUBLIC CTOR -- class IcadThreadController
//
//
IcadThreadController::IcadThreadController( void )
	{

	m_pCoreData = new IcadThreadCore();

	}


// ********************
// PUBLIC DTOR -- class IcadThreadController
//
//
IcadThreadController::~IcadThreadController( void )
	{

	if ( m_pCoreData != NULL )
		{
		delete m_pCoreData;
		m_pCoreData = NULL;
		}

	}

// ************************************
// ACCESSORS
//
//


bool 
IcadThreadController::GetAtWaitLoop( void )
	{

	return this->m_pCoreData->m_bAtWaitLoop;
	}

void 
IcadThreadController::SetAtWaitLoop( bool bValue )
	{
	this->m_pCoreData->m_bAtWaitLoop = bValue;
	}

bool 
IcadThreadController::GetInsideCmdFunction( void )
	{

	return this->m_pCoreData->m_bInsideCmdFunction;
	}

void 
IcadThreadController::SetInsideCmdFunction( bool bValue )
	{
	this->m_pCoreData->m_bInsideCmdFunction = bValue;
	}

DWORD 
IcadThreadController::GetWaitThreadID( void )
	{
	return this->m_pCoreData->m_dwWaitThreadID;
	}

void 
IcadThreadController::SetWaitThreadID( DWORD dwThreadId )
	{

	this->m_pCoreData->m_dwWaitThreadID = dwThreadId;
	}

DWORD 
IcadThreadController::GetMainThreadID( void )
	{

	return this->m_pCoreData->m_dwMainThreadID;
	}

void 
IcadThreadController::SetMainThreadID( DWORD dwThreadId )
	{
	this->m_pCoreData->m_dwMainThreadID = dwThreadId;
	}

SDSApplication *
IcadThreadController::GetLoadedAppsBeg( void )
	{
	return this->m_pCoreData->m_pLoadedAppsBeg;
	}

void 
IcadThreadController::SetLoadedAppsBeg( SDSApplication *pApp )
	{
	this->m_pCoreData->m_pLoadedAppsBeg = pApp;
	}


SDSApplication *
IcadThreadController::GetLoadedAppsCur( void )
	{
	return this->m_pCoreData->m_pLoadedAppsCur;
	}

void 
IcadThreadController::SetLoadedAppsCur( SDSApplication *pApp )
	{
	this->m_pCoreData->m_pLoadedAppsCur = pApp;
	}

SDSApplication *
IcadThreadController::GetLoadedAppsLast( void )
	{
	return this->m_pCoreData->m_pLoadedAppsLast;
	}

void 
IcadThreadController::SetLoadedAppsLast( SDSApplication *pApp )
	{
	this->m_pCoreData->m_pLoadedAppsLast = pApp; 
	}

SDSApplication *
IcadThreadController::GetLoadedAppsAct( void )
	{
	return this->m_pCoreData->m_pLoadedAppsAct;
	}

void 
IcadThreadController::SetLoadedAppsAct( SDSApplication *pApp )
	{
	this->m_pCoreData->m_pLoadedAppsAct = pApp;
	}


bool 
IcadThreadController::GetNoThreadSwap( void )
	{

	return this->m_pCoreData->m_bNoThreadSwap;
	}

void 
IcadThreadController::SetNoThreadSwap( bool bValue )
	{
	this->m_pCoreData->m_bNoThreadSwap = bValue;
	}



bool 
IcadThreadController::GetDontThreadSwapCmd( void )
	{

	return this->m_pCoreData->m_bDontThreadSwapCmd;
	}

void 
IcadThreadController::SetDontThreadSwapCmd( bool bValue )
	{
	this->m_pCoreData->m_bDontThreadSwapCmd = bValue;
	}

