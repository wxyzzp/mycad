/* SINGLETHREADSECTION.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Basic class for critical section
 * 
 */ 
#include <windows.h>
#include "icadlib.h" /*DNT*/



// ***********************
// CTOR
//
SingleThreadSection::SingleThreadSection( void )
	{

	memset(&CritSect,0,sizeof(CRITICAL_SECTION));
	::InitializeCriticalSection( &CritSect );
	}

// ************************
// DTOR
//
SingleThreadSection::~SingleThreadSection( void )
	{

	// Should this leave the critical section???
	//
	::DeleteCriticalSection( &CritSect );
	}


// ********************
// public method
//
//
bool
SingleThreadSection::IsValid( void )
	{

	if ( this == NULL )
		{
		return false;
		}

	if ( !CHECKSTRUCTPTR( this ) )
		{
		return false;
		}

	return true;
	}


// *************************
// PUBLIC METHOD
//
// Enter
//
// NOTE: Blocks until it can enter if somebody
// is already in there
//
void
SingleThreadSection::Enter( void )
	{

	::EnterCriticalSection( (LPCRITICAL_SECTION)&CritSect );

	}

// ***************************
// PUBLIC METHOD
//
// Leave
//
//
void
SingleThreadSection::Leave( void )
	{

	::LeaveCriticalSection( (LPCRITICAL_SECTION)&CritSect );
	}



