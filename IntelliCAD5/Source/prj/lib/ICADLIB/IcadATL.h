/* D:\ICADDEV\PRJ\LIB\ICADLIB\ICADATL.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * This collection of includes allows IntelliCAD code to use ATL services.
 * 
 */ 
#ifndef __ICADATL_H__
#define __ICADATL_H__

#define _WIN32_WINNT 0x0400
#define _ATL_APARTMENT_THREADED

#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;

#include <atlcom.h>

#endif //__ICADATL_H__

