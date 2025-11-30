// File  : <DevDir>\source\prj\lib\acis\ACIS.cpp
// Author: Dmitry Gavrilov

#include "StdAfx.h"

/********************************************************************************
 * Author:	Bill Gates. Ok, Visual Studio AppWizard.
 * Purpose:	Entry point to the DLL.
 * Returns:	'TRUE'.
 ********************************************************************************/
BOOL APIENTRY
DllMain
(
 HANDLE	hModule, 
 DWORD	ul_reason_for_call, 
 LPVOID	lpReserved
)
{
    switch(ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH :
		case DLL_THREAD_ATTACH  :
		case DLL_THREAD_DETACH  :
		case DLL_PROCESS_DETACH :
			break;
    }
    return TRUE;
}
