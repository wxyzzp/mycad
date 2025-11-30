/* SDSTHREADJOBSINK.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * A thread job is a bit of code you want a particular thread to execute.  A thread job sink is an object that handles thread jobs.   So a thread job sink
 * is attached to a thread, and you make that thread execute a particular job by sending or posting the thread job to the thread job sink.
 * 
 */ 
#ifndef _SDSTHREADJOBSINK_H
#define _SDSTHREADJOBSINK_H


// forward declared class names
class SDSThreadJob;

// ************************************
// SDSThreadJobSink
//
// This is an abstract base class (an interface)
// The actual implementation class is found in SDSThreadJobSink.cpp
//
//
class SDSThreadJobSink
	{
	
	// --------------------------------
	// CTORS and DTORS
	//
	public:
		virtual ~SDSThreadJobSink( void )
			{
			return;
			}
	

	//--------------------------------
	// INTERFACE
	//
	public:
		// Synchronous
		// return value is the return value of the
		// SDSThreadJob::Execute() method
		//
		virtual int 	SendJob( SDSThreadJob *pJob ) = 0;
		
		// Asynchronous
		//
		virtual void 	PostJob( SDSThreadJob *pJob ) = 0;
		
	};
		

SDSThreadJobSink *CreateSDSThreadJobSink( void );	
	
#endif // _SDSTHREADJOBSINK_H

