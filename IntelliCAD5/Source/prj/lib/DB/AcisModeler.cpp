// File  : <DevDir>\source\prj\lib\db\AcisModeler.cpp
// Author: Dmitry Gavrilov

#include "db.h"
#include "AcisModeler.h"

const char* ACIS_DLL_NAME = "acis.dll";
const char* ACIS_START_FUNC = "acisStart";
const char* ACIS_STOP_FUNC = "acisStop";

CAcisModeler	g_AcisModelerTerminator;
static HMODULE	g_hAcisDll = NULL;

/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Load ACIS-based DLL, start the modeler and initialise its components.
 * Returns:	True in success and false in failure.
 ********************************************************************************/
bool
CAcisModeler::start()
{
	if(g_hAcisDll)
		ASSERT(GetModuleHandle(ACIS_DLL_NAME) == g_hAcisDll);
	else
	{
		ASSERT(!GetModuleHandle(ACIS_DLL_NAME));
		if(!(g_hAcisDll = LoadLibrary(ACIS_DLL_NAME)))
			return false;
	}

	bool	(*startFunc)() = (bool (*)())GetProcAddress(g_hAcisDll, ACIS_START_FUNC);
	if(startFunc)
		return (*startFunc)();
	else
	{
		FreeLibrary(g_hAcisDll);
		g_hAcisDll = NULL;
		return false;
	}
}

/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Terminate ACIS modeler's components stop it and unload ACIS-based DLL.
 * Returns:	True in success and false in failure.
 ********************************************************************************/
bool
CAcisModeler::stop()
{
	if(!g_hAcisDll)
		return true;

	bool	bResult = false;

	bool	(*stopFunc)() = (bool (*)())GetProcAddress(g_hAcisDll, ACIS_STOP_FUNC);
	if(stopFunc)
		bResult = (*stopFunc)();

	FreeLibrary(g_hAcisDll);
	g_hAcisDll = NULL;

	return bResult;
}

/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Get a handle of a loaded ACIS-related DLL.
 * Returns:	The handle of the DLL.
 ********************************************************************************/
HMODULE
CAcisModeler::getAcisDllHandle()
{
	return g_hAcisDll;
}
