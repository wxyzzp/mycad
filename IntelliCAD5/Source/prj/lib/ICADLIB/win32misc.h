/* File  : <DevDir>\source\prj\lib\icadlib\win32misc.h
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 */

#ifndef _WIN32MISC_H
#define _WIN32MISC_H

#include "icadlib.h"


void ic_setlocale();


// get intellicad registry root "Software\\CompanyName\\IntelliCAD" 
// returns success/error(buffer is small)
bool getRegistryICADRoot(char* buf, int lenbuf); 

// get "Software\\CompanyName\\IntelliCAD\\Profiles" 
// returns success/error(buffer is small)
bool getRegistryProfilesRoot(char* buf, int lenbuf);

// get current profile name from registry  
// returns success/error(buffer is small)
bool GetCurrentRegistryProfileName(char* buf, int lenbuf);

// set current profile name to registry root
// returns success/error(buffer is small)
bool SetCurrentRegistryProfileName(const char *ptrName);

// get current profile name from registry root (full path name) "Software\\CompanyName\\IntelliCAD\\Profiles\\MyProfile" 
// returns success/error(buffer is small)
bool getRegCurrentProfileRoot(char* buf, int lenbuf);


#endif // _WIN32MISC_H
