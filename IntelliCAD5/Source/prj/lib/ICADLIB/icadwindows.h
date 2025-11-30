/* D:\ICADDEV\PRJ\LIB\ICADLIB\ICADWINDOWS.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Wrap Windows definitions with resolution to IntelliCAD conflicts.
 * 
 */ 

#undef	X
#undef	Y

#include <windows.h>

#define	X	SDS_X
#define	Y	SDS_Y

