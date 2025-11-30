/* SAFECOUNTER.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * 
 */ 
#ifndef _SAFECOUNTER_H
#define _SAFECOUNTER_H


// **********************************
// This class allows us to increment and decrement
// a counter around a function call while being
// exception safe
//
class SafeRecursionCounter
	{

	// CTORS and DTORS
	//
	public:
		SafeRecursionCounter( LONG *pCounter )
			{
			m_pCounter = pCounter;
			::InterlockedIncrement( m_pCounter );
			}

		~SafeRecursionCounter( void )
			{
			::InterlockedDecrement( m_pCounter );
			}


	// ------------------
	// internal data
	//
	private:
		
		LONG *m_pCounter;

	};
		
		
		
	
	
	



	
#endif // _SAFECOUNTER_H

