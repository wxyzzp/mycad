/* SDSTHREADJOB.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * A thread job is some kind of code and it's associated data you want a particular thread to execute.
 * 
 */ 
#ifndef _SDSTHREADJOB_H
#define _SDSTHREADJOB_H

#include "sdsapplication.h"
#include "sdsthread.h"

// *************************************
// SDSThreadJob
//
// This is an ABSTRACT class.  Derive a class from this and implement
// Execute
//

class SDSThreadJob
	{
	
	// ---------------------
	// DTOR
	//
	public:
		virtual ~SDSThreadJob( void )
			{
			return;
			}
	
	
	// ---------------------
	// INTERFACE
	//
	public:
		virtual int Execute( void ) = 0;
	
	
	// ------------------
	// validity operations
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
	protected:
		virtual bool isValid( void ) = 0;		
		
	};	

//////////////////////////////////////////////////////////////////////

#if defined(__cplusplus)
	extern "C"
	{
#endif

int SDS_BroadcastRQToSDSApps(int RQcode) ;

#if defined(__cplusplus)
	}
#endif
		
// ---------------------------
// Helper class -- used to dispatch the rq broadcast to the main engine thread
//
//


class BroadcastRQJob : public SDSThreadJob
	{
		// ------------------------------
		// CTOR and DTOR
		//
		public:
			BroadcastRQJob( int iRQcode )
				{
				m_iRQcode = iRQcode;
				}
		
			virtual int Execute( void )
				{

				// The whole purpose is to make sure we are on the main engine thread, so ASSERT that fact
				//
				ASSERT( SDSApplication::GetEngineApplication()->GetMainThread()->IsCurrent() );

				return SDS_BroadcastRQToSDSApps( m_iRQcode );
				}
		protected:
			virtual bool isValid( void )
				{
				return true;
				}

		// --------------------
		// internal data
		//
		private:
			int	m_iRQcode;
	};
	
#endif // _SDSTHREADJOB_H

