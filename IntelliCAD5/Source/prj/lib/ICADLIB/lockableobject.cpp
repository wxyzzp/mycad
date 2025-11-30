/* LockableObject.cpp
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Base class for synchronizable, lockable object
 * 
 */ 

#include <windows.h>
#include <assert.h>
#include "singlethreadsection.h" /*DNT*/
#include "LockableObject.h"		/*DNT*/

// ********************************
// CLASS BaseBaseLockableObject
//
// Derive classes from this one that you want
// to use synchronously.  This allows external
// synchronization, or the class can be written to 
// internally synchronize using its own Lock/Unlock
// calls.
//

// **************************************
//
// CTOR
//
BaseLockableObject::BaseLockableObject( void )
	{

	this->m_iLockCount = 0;

	}

// ****************************************
// DTOR
//
// 
BaseLockableObject::~BaseLockableObject( void )
	{
	this->m_iLockCount = -1;
	}


// *************************************
// Public Operation
//
void
BaseLockableObject::Lock( void )
	{

	this->m_TryLockLock.Enter();
	this->m_iLockCount++;
	this->m_TryLockLock.Leave();


	this->m_ObjectLock.Enter();
	}

// *************************************
// Public Operation
//
void
BaseLockableObject::Unlock( void )
	{
	this->m_ObjectLock.Leave();
	this->m_iLockCount--;
	assert( this->m_iLockCount >= 0 );

	}

// **************************************
// Public Operation
//
//
bool
BaseLockableObject::TryLock( void )
	{
	bool bRetval = false;

	this->m_TryLockLock.Enter();
	if ( this->m_iLockCount == 0 )
		{
		this->m_ObjectLock.Enter();
		bRetval = true;
		}
	this->m_TryLockLock.Leave();

	return bRetval;
	}

// ************************************
// internal method
//
bool
BaseLockableObject::isValid( void )
	{

	if ( !this->m_ObjectLock.IsValid() )
		return false;

	if ( !this->m_TryLockLock.IsValid() )
		return false;

	if ( this->m_iLockCount < 0 )
		return false;

	return true;
	}


