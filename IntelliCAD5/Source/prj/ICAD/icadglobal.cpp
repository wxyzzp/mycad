/* IcadGlobal.cpp
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 *
 * Core support for IntelliCAD "globals".
 *
 * ICAD was/is highly dependent on globals.  This file is a place to remove them to.
 *
 */

#include "Icad.h" /* DNT */
#include "sdsapplication.h" /* DNT */
#include "threadcontroller.h" /* DNT */
#include "commandqueue.h" /*DNT*/	 
#include "inputeventqueue.h" /*DNT*/
#include "icadinputeventqueue.h" /*DNT*/

/* 
 * 
 * Core storage for "global" variables used by the IntelliCAD engine
 *
 * These used to be global variables scattered throughout the code.  Over time this will
 * evolve into a class with lots of accessors and multiple-instance/threading support.
 * 
 */ 
class IcadEngineVariables
{
public:
	IcadEngineVariables();
	~IcadEngineVariables();

	IcadThreadController *GetThreadController();
	CommandQueue *GetMenuQueue();
	CommandQueue *GetActiveCommandQueue();
	CommandQueue *GetScriptQueue();
	CommandQueue *GetCancelScriptQueue();
	InputEventQueueInterface *GetInputEventQueue()
	{
		return m_pInputEventQueue;
	}
	
private:
	IcadThreadController *m_pThreadController;
	CommandQueue		 *m_pMenuQueue;
	CommandQueue		 *m_pScriptQueue;
	CommandQueue		 *m_pCancelScriptQueue;
	InputEventQueueInterface *m_pInputEventQueue;
	
};

// ****************************************
// CLASS IMPLEMENTATION -- class IcadEngineVariables
//
//
IcadEngineVariables::IcadEngineVariables()
{
	this->m_pThreadController = new IcadThreadController();
	this->m_pMenuQueue = new CommandQueue();
	this->m_pScriptQueue = new CommandQueue();
	this->m_pCancelScriptQueue = new CommandQueue();
	this->m_pInputEventQueue = ::CreateIcadInputEventQueue();
}

IcadEngineVariables::~IcadEngineVariables()
{
	if(m_pThreadController != NULL)
	{
		delete m_pThreadController;
		m_pThreadController = NULL;
	}
	if(this->m_pMenuQueue != NULL)
	{
		delete this->m_pMenuQueue;
		this->m_pMenuQueue = NULL;
	}
	if(this->m_pScriptQueue != NULL)
	{
		delete this->m_pScriptQueue;
		this->m_pScriptQueue = NULL;
	}
	if(this->m_pCancelScriptQueue != NULL)
	{
		delete this->m_pCancelScriptQueue;
		this->m_pCancelScriptQueue = NULL;
	}
	if(this->m_pInputEventQueue != NULL)
	{
		this->m_pInputEventQueue->DestroySelf();
		this->m_pCancelScriptQueue = NULL;
	}
}

IcadThreadController *
IcadEngineVariables::GetThreadController( void )
{
	return this->m_pThreadController;
}

CommandQueue *
IcadEngineVariables::GetMenuQueue( void )
{
	return this->m_pMenuQueue;
}

CommandQueue *
IcadEngineVariables::GetScriptQueue( void )
{
	return this->m_pScriptQueue;
}

CommandQueue *
IcadEngineVariables::GetCancelScriptQueue( void )
{
	return this->m_pCancelScriptQueue;
}

// !!!NOTE:  This method currently does not operate on the engine variables,
// it gets its value from another global.  Clearly this is temporary
CommandQueue *
IcadEngineVariables::GetActiveCommandQueue( void )
{
	ASSERT( SDSApplication::GetActiveApplication()->IsValid() );
	return SDSApplication::GetActiveApplication()->GetCommandQueue();
}

// *************************
// Actual data
//
static IcadEngineVariables zEngineVariables;
static bool	zIcadExiting = false;

void
IDS_SignalIcadExiting()
{
	zIcadExiting = true;
}

bool
IDS_IsIcadExiting()
{
	return zIcadExiting;
}

IcadThreadController*
IDS_GetThreadController()
{
	return zEngineVariables.GetThreadController();
}

CommandQueue*
GetActiveCommandQueue(void)
{
#if 1//rk
	return IDS_IsIcadExiting() ? NULL : zEngineVariables.GetActiveCommandQueue();
#else
	return zEngineVariables.GetActiveCommandQueue();
#endif
}

CommandQueue*
GetScriptQueue()
{
	return zEngineVariables.GetScriptQueue();
}

CommandQueue*
GetCancelScriptQueue()
{
	return zEngineVariables.GetCancelScriptQueue();
}

CommandQueue*
GetMenuQueue()
{
	return zEngineVariables.GetMenuQueue();
}

InputEventQueueInterface*
IDS_GetInputEventQueue()
{
	return zEngineVariables.GetInputEventQueue();
}
