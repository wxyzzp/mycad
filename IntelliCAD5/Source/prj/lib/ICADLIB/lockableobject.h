/* LOCKABLEOBJECT.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Base class for synchronizable, lockable object
 * 
 */ 

#ifndef _LOCKABLEOBJECT_H
#define _LOCKABLEOBJECT_H


class SingleThreadSection;

// ********************************
// CLASS BaseLockableObject
//
// Derive classes from this one that you want
// to use synchronously.  This allows external
// synchronization, or the class can be written to 
// internally synchronize using its own Lock/Unlock
// calls.
//
class BaseLockableObject
	{


	// ctors and dtors
	//
	public:
		BaseLockableObject( void );
		virtual ~BaseLockableObject( void );

	// operations
	//
	public:

		// Is this a properly formed instance?
		//
		bool IsValid()
			{
			bool bRetval = false;
			if ( this != NULL )
				{
				bRetval = this->isValid();
				}
			return bRetval;
			}

		// ********************
		// Lock will block until it succeeds!
		void Lock( void );
		void Unlock( void );

		// This is the only method that needs much explanation
		// If you call this method, it will try to lock the object.
		//  If the object is already locked, it will return immediately
		// with a false value.  
		// If the object is not locked, it will lock it and return 
		// immediately with a true value
		//  THIS METHOD NEVER BLOCKS
		//
		// The implementation is a little tricky, but thread-safe.  
		// By the time TryLock returns it may be possible to lock
		// the object, but it will never say it got the lock when it couldn't
		//
		// This method should be used with care, since the caller could ignore
		// the return value and go into non-safe code.
		//
		bool TryLock( void );


	// helper method
	//
	protected:
		virtual bool isValid();

	// internal data
	private:
		// Keep track of how many objects have this locked or are waiting
		//
		int						m_iLockCount;

		// This is the actual lock
		//
		SingleThreadSection		m_ObjectLock;

		// This is an extra one need to implement TryLock
		//
		SingleThreadSection		m_TryLockLock;

	};

// ************************************
// Use this class to automatically lock
// an object in a scope.  CTOR and DTOR
// handle all the work so you don't have to worry
// about abnormal exit from the scope
//
class LockBracket
	{

	// ******************
	// CTOR AND DTOR
	//
	public:
		LockBracket( BaseLockableObject *pObject )
			{
			pObject->Lock();
			this->m_pObject = pObject;
			}

		~LockBracket( void )
			{
			this->m_pObject->Unlock();
			}
			

	private:
		BaseLockableObject *m_pObject;
	};

// ****************************
// Helper macro for internal synchronization.
// For example
//
// class Foo : public BaseLockableObject
//	{
//		....
//		int SomeMethod( void )
//		{
//			SYNCHRONIZEMETHOD()
//			....
//		}
//

#define SYNCHRONIZEMETHOD()			LockBracket theLockBracket( this );

#endif // _LOCKABLEOBJECT_H



