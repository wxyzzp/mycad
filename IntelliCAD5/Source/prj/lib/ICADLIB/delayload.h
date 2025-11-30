/* DELAYLOAD.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Used to support the VC++ 6.0 delayload option.  
 * 
 */ 

#ifndef _DELAYLOAD_H
#define _DELAYLOAD_H

HMODULE
FindDelayLoadModule( HMODULE hLoadingLibrary,  // input:  The module that is doing the loading
					LPCTSTR szModulePath 	   // input:  The name of the module to be loaded
					);


#endif // _DELAYLOAD_H



