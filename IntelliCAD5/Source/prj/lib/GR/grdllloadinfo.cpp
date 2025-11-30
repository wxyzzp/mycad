/* D:\ICADDEV\PRJ\LIB\GR\GRDLLLOADINFO.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * $Revision: 1.2 $ $Date: 2001/03/16 21:48:17 $ 
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

// GRDLLLoadInfo.cpp : Implementation of GRDLLLoadInfo
#define STRICT

#include "IcadATL.h"

#include "grmodule.h"

#include "GRDLLLoadInfo.h"

/////////////////////////////////////////////////////////////////////////////
// GRDLLLoadInfo


STDMETHODIMP GRDLLLoadInfo::GetModule(long *pModuleHandle)
{
	extern HINSTANCE	ModuleInstance;

	*pModuleHandle = (long)ModuleInstance;
	return S_OK;
}


