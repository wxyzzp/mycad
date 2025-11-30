/* ICADDEV\PRJ\LIB\AUTO\AUTODLL.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

// AutoDLL.cpp : Defines the initialization routines for the DLL.
//

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

#include "StdAfx.h"
#define CLSID_DEFINED
namespace ICAD_AUTOMATION
{
#include "icad_i.c"
}
using namespace ICAD_AUTOMATION;
#include "icadres.h"


CComModule _Module;

//we have removed PlotProperties from the ObjectMap so that it will not get auto-registered.  When this functionality
//is fully implemented, we can add this back in.  When we do, we will also have to add the following into AutoDLL.rc.  
//
// REGISTRY
//
///IDR_PLOTPROPERTIES      REGISTRY DISCARDABLE    "PlotProperties.rgs"
// AM 1/4/99

#include "AutoPoint.h"

BEGIN_OBJECT_MAP(ObjectMap)
	//OBJECT_ENTRY(CLSID_Point, CAutoPoint)
	//OBJECT_ENTRY(CLSID_PlotProperties, PlotProperties)	//currently we will not add PlotProperties to the object map
END_OBJECT_MAP()

class AutoDLLApp : public CWinApp
	{
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	};

AutoDLLApp theApp;

BOOL AutoDLLApp::InitInstance()
	{
	_Module.Init(ObjectMap, m_hInstance);
	return CWinApp::InitInstance();
	}

int AutoDLLApp::ExitInstance()
	{
	_Module.Term();
	return CWinApp::ExitInstance();
	}


/////////////////////////////////////////////////////////////////////////////
// These registration hooks are always called so that the automation
// DLL can determine if registration is necessary. 
// This allows incremental upgrades to the automation server.

HRESULT
RegisterAutomation( BOOL always, ITypeLib** ppITypeLib)
	{
	HRESULT hr = S_OK;
	if ( always )
		{
		//First register the main typelib
		TCHAR szModule[_MAX_PATH+10];
		GetModuleFileName(_Module.m_hInstTypeLib, szModule, _MAX_PATH);

		//TODO: If we add multiple typelibs, we need to strcat the index, and load each one.
		//if (lpszIndex != NULL)
		//	lstrcat(szModule, OLE2CT(lpszIndex));
		
		USES_CONVERSION;
		LPOLESTR lpszModule = T2OLE(szModule);

		if (ppITypeLib)
			{
			hr = LoadTypeLibEx(lpszModule,REGKIND_DEFAULT,ppITypeLib);
			}
		else
			{
			ITypeLib* pTypeLib=NULL;
			hr = LoadTypeLibEx(lpszModule,REGKIND_NONE,&pTypeLib);
			if (pTypeLib != NULL)
				pTypeLib->Release();
			}

		//Next load the stuff from the RGS files
		if (SUCCEEDED(hr))
			hr = DllRegisterServer();
		}
	
	return hr;
	}

HRESULT
UnregisterAutomation()
	{
	//*** Unregister the type library info.
	UnRegisterTypeLib(ICAD_AUTOMATION::LIBID_IntelliCAD,1,0,(LCID)0x0000,SYS_WIN32);
	return DllUnregisterServer();
	}

/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE

STDAPI DllCanUnloadNow(void)
	{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return (AfxDllCanUnloadNow()==S_OK && _Module.GetLockCount()==0) ? S_OK : S_FALSE;
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
	LCID	locale = LOCALE_NEUTRAL;
	HRESULT	result = S_OK;
	
#if _WIN32_WINNT >= 0x0400
//	no longer defined
//	result = UnRegisterTypeLib( LIBID_PLOTSUPPORTLib, 1, 0, locale, SYS_WIN32);
#endif
	
	if ( SUCCEEDED( result) )
		result = _Module.UnregisterServer();
	
	return S_OK;
	}


