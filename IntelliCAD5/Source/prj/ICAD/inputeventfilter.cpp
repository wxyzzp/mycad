/* D:\ICADDEV\PRJ\ICAD\INPUTEVENTFILTER.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * $Revision: 1.1.1.1 $ $Date: 2000/01/19 13:41:22 $ 
 * 
 * Abstract
 * 
 * This implements the input event filter class
 * 
 */ 


#include "Icad.h"			   
#include <windows.h>	// --- probably not needed over time				
#include "icadlib.h" /*DNT*/
#include "inputevent.h" /*DNT*/
#include "inputeventfilter.h" /*DNT*/



// ************************************
// CLASS IMPLEMENTATION -- class InputEventFilter
//
//
//


// ------------------
// CTOR
//
InputEventFilter::InputEventFilter( int iFilterFlags )
	{
	m_iFilterFlags = iFilterFlags;
	}

// --------------------
// DTOR
//
InputEventFilter::~InputEventFilter( void )
	{
	m_iFilterFlags = 0;
	}

// ************************************

// --------------------
// PUBLIC OPERATION
//
//
bool
InputEventFilter::Match( InputEvent *pEvent )
	{

	bool bRetval = false;
	if ( ( pEvent->GetType() & m_iFilterFlags ) != 0 )
		{
		bRetval = true;
		}

	return bRetval;
	}

