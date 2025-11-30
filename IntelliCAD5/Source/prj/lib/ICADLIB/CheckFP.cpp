/* PRJ\ICADLIB\CheckFP.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Check the floating point control word and reset if necessary.
 * The application assumes floating point errors will be masked and
 * NAN values returned.	Some drivers will reset this control, though, 
 * causing exceptions in the presence of those drivers.
 *
 * This function can be called in the message loop or wherever necessary
 * to preserve the desired status.
 * 
 */ 

// THIS FILE HAS BEEN GLOBALIZED!

#include <float.h>
#include "CheckFP.h"/*DNT*/


static unsigned int FloatingPointControlWord = 0;

/*	The default FPCW is _CW_DEFAULT defined in FLOAT.H.  We use a slightly 
 *	different FPCW that has _IC_AFFINE + _PC_64 on.	 This is what VBA wants.
 */


//#ifdef DEBUG		// Would like to check errors in DEBUG, but there are too many in IntelliCAD
#if 0		
#define FPCW_INIT	( _RC_NEAR + _IC_AFFINE + _PC_64 + /*_EM_INVALID + _EM_ZERODIVIDE + _EM_OVERFLOW + _EM_UNDERFLOW */+ _EM_INEXACT + _EM_DENORMAL)
#else
#define FPCW_INIT	( _RC_NEAR + _IC_AFFINE + _PC_64 + _EM_INVALID + _EM_ZERODIVIDE + _EM_OVERFLOW + _EM_UNDERFLOW  + _EM_INEXACT + _EM_DENORMAL)
#endif

void CheckFPControlWord()
	{
	unsigned int FPCW = _control87(0, 0);

	if (!FloatingPointControlWord)
		{
		FloatingPointControlWord = _control87(FPCW_INIT, 0xFFFFFFFF);
//		DB_MSGX("FloatingPointControlWord %lX", (FloatingPointControlWord));
		}
	else if (FPCW != FloatingPointControlWord)
		{
//		DB_MSGX("FloatingPointControlWord has changed, old %lX, new %lX", (FloatingPointControlWord, FPCW));
		FloatingPointControlWord = _control87(FPCW_INIT, 0xFFFFFFFF);
//		DB_MSGX("FloatingPointControlWord has been reset %lX", (FloatingPointControlWord));
		}
	}

