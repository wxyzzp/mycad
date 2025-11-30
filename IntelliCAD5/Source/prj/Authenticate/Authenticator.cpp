/* G:\PROJECTS\AUTHENTICATE\AUTHENTICATOR.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Perform basic functions of component and serial number validation.
 * 
 */ 

// Authenticator.cpp : Implementation of Authenticator
#include "stdafx.h"
#include "IcadDef.h"
#include "Authenticator.h"


HINSTANCE
AuthenticationInstance()
	{
	return _Module.GetModuleInstance();
	}


