/* G:\PROJECTS\AUTHENTICATE\AUTHENTICATOR.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

// Authenticator.h : Declaration of the Authenticator

#ifndef __AUTHENTICATOR_H_
#define __AUTHENTICATOR_H_

#include "Authrc.h"       // authentication symbols


#ifdef	AUTH_DLL
#define	AUTH_DECLSPEC	__declspec(dllexport)
#else
#define	AUTH_DECLSPEC	__declspec(dllimport)
#endif


#define AUTH_API	EXTERN_C AUTH_DECLSPEC

AUTH_API HINSTANCE	AuthenticationInstance();


#endif //__AUTHENTICATOR_H_


