/* D:\ICADDEV\PRJ\LIB\DB\DBDLLLOADINFO.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * $Revision: 1.1.1.1 $ $Date: 2000/01/19 13:41:33 $ 
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

// DBDLLLoadInfo.cpp : Implementation of DBDLLLoadInfo
#define STRICT

#include "IcadATL.h"

#include "dbmodule.h"

#include "DBDLLLoadInfo.h"

/////////////////////////////////////////////////////////////////////////////
// DBDLLLoadInfo


STDMETHODIMP DBDLLLoadInfo::GetModule(long *pModuleHandle)
{
	extern HINSTANCE	ModuleInstance;

	*pModuleHandle = (long)ModuleInstance;

	return S_OK;
}


