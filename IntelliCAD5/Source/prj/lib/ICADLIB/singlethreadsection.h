/* SINGLETHREADSECTION.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Basic class for critical section
 * 
 */ 

#ifndef _SINGLETHREADSECTION_H
#define _SINGLETHREADSECTION_H


class SingleThreadSection
	{


	// ctors and dtors
	//
	public:
		SingleThreadSection( void );
		~SingleThreadSection( void );

	// operations
	//
	public:

		// Is this a properly formed instance?
		//
		bool IsValid();

		// Enter the section.  Blocks until it can enter
		//
		void Enter( void );

		// vacate and let others in
		//
		void Leave( void );

	private:

		// hide the internals
		//
		CRITICAL_SECTION CritSect;

	};


// Class to use ctor and dtor semantics for critical section
//
class SingleThreadGuard
	{

	// ctor and dtor
	//
	public:
		SingleThreadGuard( SingleThreadSection *pSection )
			{
			this->m_pSection = pSection;
			this->m_pSection->Enter();
			}

		~SingleThreadGuard( void )
			{
			this->m_pSection->Leave();
			this->m_pSection = NULL;
			}


	private:
		SingleThreadSection *m_pSection;
	};

#endif // _SINGLETHREADSECTION_H



