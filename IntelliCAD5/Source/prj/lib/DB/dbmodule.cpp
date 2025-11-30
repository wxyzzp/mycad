/* D:\ICADDEV\PRJ\LIB\DB\DBMODULE.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * $Revision: 1.1.1.1 $ $Date: 2000/01/19 13:41:33 $ 
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

// dbmodule.cpp : Implementation of DLL Exports.


// Note: Proxy/Stub Information
//      To build a separate proxy/stub DLL, 
//      run nmake -f dbmoduleps.mk in the project directory.

#define STRICT



#include "IcadATL.h"

#include <initguid.h>
#include "dbmodule.h"

#include "dbmodule_i.c"

#include "dbdllloadinfo.h"





HINSTANCE	ModuleInstance;


CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
	OBJECT_ENTRY(CLSID_DBDLLLoadInfo, DBDLLLoadInfo)
END_OBJECT_MAP()

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point

extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
		ModuleInstance = hInstance;

        _Module.Init(ObjectMap, hInstance, &CLSID_DBDLLLoadInfo );
        DisableThreadLibraryCalls(hInstance);
    }
    else if (dwReason == DLL_PROCESS_DETACH)
        _Module.Term();
    return TRUE;    // ok
}

/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE

STDAPI DllCanUnloadNow(void)
{
    return (_Module.GetLockCount()==0) ? S_OK : S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _Module.GetClassObject(rclsid, riid, ppv);
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
    // registers object, typelib and all interfaces in typelib
    return _Module.RegisterServer(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
    return _Module.UnregisterServer(TRUE);
}


// **********************************************************************
// DelayLoad support
//
//
#include <delayimp.h>
#include "delayload.h" // from icadlib 


FARPROC WINAPI MyDliNotifyHook( unsigned dliNotify, PDelayLoadInfo pdli )
	{
	FARPROC Result = NULL;

	if ( dliNotify == dliNotePreLoadLibrary )
		{	
		HMODULE hModule = FindDelayLoadModule( GetModuleHandle( "DB.DLL" ), pdli->szDll );

		if ( hModule == NULL )
			{		
			hModule = LoadLibrary( pdli->szDll );
			}


		Result = (FARPROC)hModule;
		}

	return Result;
	}

extern "C" PfnDliHook __pfnDliNotifyHook = MyDliNotifyHook;


