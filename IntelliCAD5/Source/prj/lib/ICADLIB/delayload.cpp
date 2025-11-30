/* DELAYLOAD.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Delay Load support routines
 * 
 */ 
#include <windows.h>
#include <stdlib.h>
#include <tchar.h>
#include "IcadATL.h"

#include "icadlib.h"

#include "dllloadinfo.h"
#include "delayload.h"


// *******************************************************
// FindAndLoadDBModule
//
// Use COM and registry techniques to get the latest installed
// DB.DLL
//
HMODULE
FindAndLoadDBModule( void )
	{

	HMODULE hRetval = NULL;

	CLSID dbCLSID;
	if (SUCCEEDED( CLSIDFromProgID( L"Visio.DB.DBDLLLoadInfo", &dbCLSID ) ) )
		{
		CComPtr<IVisioDLLLoadInfo> pDllLoadInfo;

		HRESULT hr = CoCreateInstance( dbCLSID, 
										NULL, 
										CLSCTX_SERVER,
										__uuidof( IVisioDLLLoadInfo ),
										reinterpret_cast<void **>(&pDllLoadInfo) );
		if ( SUCCEEDED( hr ) )
			{
			pDllLoadInfo->GetModuleHandle( (long *)&hRetval );
			}	

		}

	return hRetval;
	}



// *******************************************************
// FindAndLoadGRModule
//
// Use COM and registry techniques to get the latest installed
// GR.DLL
//
HMODULE
FindAndLoadGRModule( void )
	{

	HMODULE hRetval = NULL;

	CLSID dbCLSID;
	if (SUCCEEDED( CLSIDFromProgID( L"Visio.GR.GRDLLLoadInfo", &dbCLSID ) ) )
		{
		CComPtr<IVisioDLLLoadInfo> pDllLoadInfo;

		HRESULT hr = CoCreateInstance( dbCLSID, 
										NULL, 
										CLSCTX_SERVER,
										__uuidof( IVisioDLLLoadInfo ),
										reinterpret_cast<void **>(&pDllLoadInfo) );
		if ( SUCCEEDED( hr ) )
			{
			pDllLoadInfo->GetModuleHandle( (long *)&hRetval );
			}	

		}

	return hRetval;
	}



// *******************************************************
// FindDelayLoadModule
//
// This function is used to support dynamic loading of some of our DLLs.
// We use this to support sharing of DLLs between IntelliCAD and other projects.
// The basic algorithm is this:
//
// 1)  Look in the same directory as the module trying to load the DLL
// 2)  Look in the registry to see where the DLL is installed
// 3)  Use the regular LoadLibrary to find the DLL in the path
//
// Step 1. is done to make things work in the case where some DLLs are not registered.  We then
// guess that maybe the one in the same directory (not the "current" directory) probably goes with the
// one we already loaded.
//
HMODULE
FindDelayLoadModule( HMODULE hLoadingLibrary,  // input:  The module that is doing the loading
					LPCTSTR szModulePath )	   // input:  The name of the module to be loaded
	{
	HMODULE hRetval = NULL;

	TCHAR fdrive[ _MAX_DRIVE ];
	TCHAR fdir[ MAX_PATH ];
	TCHAR fname[ MAX_PATH ];
	TCHAR fext[ _MAX_EXT ];
	TCHAR szModuleName[ MAX_PATH ];

	// Get the module name, without path into szModuleName
	//
	_tsplitpath( szModulePath, NULL, NULL, fname, fext );
	_tmakepath( szModuleName, NULL, NULL, fname, fext );

	// First we'll look in the same directory as the loading library, for that 
	// we need to get the directory name
	//
	TCHAR szCurrentModuleDirectory[ MAX_PATH ];
	GetModuleFileName( hLoadingLibrary, szCurrentModuleDirectory, MAX_PATH );

	// Now get rid of the name, and just keep the path
	//
	_tsplitpath( szCurrentModuleDirectory, fdrive, fdir, NULL, NULL );


	// Put together the drive and dir from the current module with the name and extension
	// from the module we're looking for 
	//
	TCHAR szTemp[ MAX_PATH ];
	_tmakepath( szTemp, fdrive, fdir, fname, fext );

	hRetval = LoadLibrary( szTemp );
	if ( hRetval == NULL )
		{
		// Well, that didn't work.  Try looking in the registry next
		//
		if strisame( fname, "DB" )
			{
			hRetval = FindAndLoadDBModule();
			}
		else if strisame( fname, "GR" )
			{
			hRetval = FindAndLoadGRModule();
			}
		}

	// If nothing else worked, just try to load from the path
	//
	if ( hRetval == NULL )
		{
		hRetval = LoadLibrary( szModulePath );
		}
	

	return hRetval;
	}




