/* D:\ICADDEV\PRJ\LIB\SDS\SDS_ALERT.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

//** Includes
#include "db.h"
#include "../../ICAD/res/icadrc2.h"

//** Functions

int sds_alert(const char *szAlertMsg) 
{
	/*D.G.*/// Dependency from MFC removed.

	// Call the function IcadSharedGlobals::GetMainHWND() to pass the Main Window Application handle
	// to the Message Box, thus keeping this part of the code MFC-independent.
	MessageBox(IcadSharedGlobals::GetMainHWND(), szAlertMsg, ResourceString(DNT_ICADAPP_INTELLICAD_1, "IntelliCAD" ), MB_OK | MB_SETFOREGROUND);

	return(RTNORM);
}

