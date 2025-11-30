/* SDSTHREADEXCEPTION.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 *
 * 
 */ 
#ifndef _SDSTHREADEXCEPTION_H
#define _SDSTHREADEXCEPTION_H


// *******************************
// SDSThreadException
//
//
class SDSThreadException
	{
	
	// ctors and dtors
	public:
	
		enum Reason
			{
			BAD = 0,
			TERMINATED,
			APPEXIT,
			RESTARTCOMMANDTHREAD
			};

		SDSThreadException(){ m_Reason = BAD; }
    
		SDSThreadException( Reason theReason ) 
			{
			m_Reason = theReason;
			}

	    virtual ~SDSThreadException()
			{
			}

	// accessors
	public:
		Reason GetReason( void )
			{
			return m_Reason;
			}

		SDSThreadException& operator = ( /*const*/ SDSThreadException& exception )
		{
			m_Reason = exception.GetReason();
			return (*this);
		}
		

	// data internals
	private:
		Reason m_Reason;

	};
	
#endif // _SDSTHREADEXCEPTION_H

