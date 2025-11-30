// INPUTEVENTQUEUE.H
// Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
// 
// Abstract
//
//	This implements the input event queue class
// 
#ifndef _INPUTEVENTQUEUE_H
#define _INPUTEVENTQUEUE_H

#ifndef _LOCKABLEOBJECT_H
	#include "lockableobject.h"
#endif


void ProcessMsgWaitForObject(HANDLE hSynchroObject);


// Interface of InputEventQueue uses these classes in name
//
class InputEvent;
class InputEventFilter;

// *********************************************
// This is an interface class -- i.e. abstract
//
class InputEventQueueInterface	: public BaseLockableObject
	{

	// PUBLIC OPERATIONS
	//
	public:

		//	Just put the event in the queue and forget about it
		// Destination is responsible for freeing the event
		// Returns false if the queue is exiting...
		//
		virtual bool PostEvent( InputEvent *pEvent ) = 0;

		//	Put the event in the queue and wait until it is
		// processed or the queue is canceled
		// Destination is responsible for freeing the event
		// Returns false if the queue is exiting or cancel'd
		//
		virtual bool SendEvent( InputEvent *pEvent ) = 0;

		// Get an event if one is waiting, else return NULL
		virtual InputEvent *PeekEvent( InputEventFilter *pEventFilter = NULL ) = 0;


		// Get an event if one is waiting, or wait until one shows up
		// returns NULL if the queue is cancel'd or destroyed
		//
		virtual InputEvent *WaitEvent( InputEventFilter *pEventFilter = NULL ) = 0;


		// This method tells waiting threads to get out of WaitEvent
		// and cancels processing
		//
		virtual void SignalCancel( void ) = 0;	

		// basically the destructor
		//
		virtual void DestroySelf( void ) = 0;

		// This method tells the queue to stop accepting events.  This should only be called right before the
		// queue goes away
		//
		virtual void Shutdown( void ) = 0;

		// AG: returns true if somebody is waiting for event in this queue
		virtual bool IsWaiting() = 0;
	};





#endif // _INPUTEVENTQUEUE_H

