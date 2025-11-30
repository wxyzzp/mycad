// COMMANDQUEUE.H
// Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
// 
// Abstract
//
//	This implements the command queue class
// 
#ifndef _COMMANDQUEUE_H
#define _COMMANDQUEUE_H

#ifndef _LOCKABLEOBJECT_H
	#include "lockableobject.h"
#endif



class CommandQueue	: public BaseLockableObject
	{

	// CTORS and DTORS
	public:
		CommandQueue( void );
		virtual ~CommandQueue( void );

	// PUBLIC OPERATIONS
	//
	public:
		// These three are used ALOT, so inline
		//
		inline bool IsEmpty( void )
			{
#if 1//rk 1
			return ( !this || (m_pStartPtr == NULL) );
#else
			return ( this->m_pStartPtr == NULL );
#endif
			}
		inline bool IsNotEmpty( void )
			{
#if 1//rk 1
			return ( this && (m_pStartPtr != NULL) );
#else
			return ( this->m_pStartPtr != NULL );
#endif
			}
		inline resbuf *PeekItem( void )
			{
#if 1//rk 1
			return this ? m_pStartPtr : NULL;
#else
			return this->m_pStartPtr;
#endif
			}
		void AddItem( struct resbuf *pItem );
		void AddItemToFront( struct resbuf *pItem );
		struct resbuf *PopItem( void );
		struct resbuf *AdvanceNext( void );
		void Flush( void );
		void FreeNext( void );
		void Concat( CommandQueue *pOther );
		void Copy( CommandQueue *pOther );


		const char *PeekString( void );
		bool IsCommandPause( void );

	protected:
		virtual bool isValid( void );

	// CONSTANTS
	private:
		static const int iClassCheckVal;

	private:  
		int m_iCheckVal;

		struct resbuf *m_pStartPtr;
		struct resbuf *m_pEndPtr;

	};





#endif // _COMMANDQUEUE_H

