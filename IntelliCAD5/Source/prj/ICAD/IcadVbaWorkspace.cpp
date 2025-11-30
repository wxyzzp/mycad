/* INTELLICAD\PRJ\LIB\AUTO\VBAWORKSPACE.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * The CommonProjects object that contains document independent code.
 * Original source is from the Summit APC Sample Library
 * 
 */ 
#include "stdafx.h"
#include <afxole.h>
#include "IcadDoc.h"
#include "IcadApp.h"
#include "IcadVbaWorkspace.h"
#include "res\icadrc2.h"
#include "LicensedComponents.h"

/////////////////////////////////////////////////////////////////////////////
// CIcadWorkspace

HRESULT CIcadWorkspace::FinalConstruct()
	{
	CComBSTR CommProj;
	CommProj.LoadString(IDC_VBA_COMMONPROJECTS);
	ActProjectStorage.put_ProjectName(CommProj);
	return S_OK;
	}

HRESULT CIcadWorkspace::FinalRelease()
	{
	if(CIcadApp::m_LicensedComponents.IsAccessible(CLicensedComponents::Components::VBA))
		{
		ASSERT(ApcProjectItem == NULL);
		ASSERT(ApcProject == NULL);
		}
	//ApcProjectItem.Detach();
	//ApcProject.Close();
	return S_OK;
	}

//
// IWorkspace
//
STDMETHODIMP CIcadWorkspace::get_Application(IIcadApplication** ppVal)
	{
	return GetApp()->m_pAutoApp->QueryInterface (ppVal);
	}

STDMETHODIMP CIcadWorkspace::get_Parent(IIcadApplication** ppVal)
	{
	return GetApp()->m_pAutoApp->QueryInterface (ppVal);
	}

STDMETHODIMP CIcadWorkspace::get_ActiveDocument(IIcadDocument** ppVal)
	{
	CComPtr<IIcadApplication> spApp;
	HRESULT hr = GetApp()->m_pAutoApp->QueryInterface (&spApp);
	ASSERT(SUCCEEDED(hr));
	if (spApp)
		{
		hr = spApp->get_ActiveDocument(ppVal);
		}
	return hr;
	}

STDMETHODIMP CIcadWorkspace::put_Name(BSTR bstr)
	{
	if (!ApcProjectItem)
		return E_FAIL;
	
	return ApcProjectItem->put_Name(bstr);
	}

STDMETHODIMP CIcadWorkspace::get_Name(BSTR* pbstr)
	{
	if (!ApcProjectItem)
		return E_FAIL;
	
	return ApcProjectItem->get_Name(pbstr);
	}

//
// ApcProject Overrides
//
HRESULT CIcadWorkspace::ApcProject_Save() // IApcProjectEvents::Save handler
	{
	return ActProjectStorage.Save();
	}

//
// ActProjectStorage Overrides
//
HRESULT CIcadWorkspace::ActProjectStorage_OnClose()
	{
	ApcProjectItem.Detach();
	return _CActProjectStorage::ActProjectStorage_OnClose();
	}

HRESULT CIcadWorkspace::ActProjectStorage_OnDefine()
	{
	// Define "ThisWorkspace"
	CComBSTR WSpace;
	WSpace.LoadString(IDC_VBA_THISWORKSPACE);
	HRESULT hr = ApcProjectItem.Define(ApcProject, this, axTypeHostProjectItem, WSpace);
	if (FAILED(hr))
		{
		TRACE0(_T("Workspace ApcProjectItem.Define failed"));
		return hr;
		}
	
	ApcProjectItem->APC_PUT(AllowViewObject)(VARIANT_FALSE);
	
	return S_OK;
	}

HRESULT CIcadWorkspace::ActProjectStorage_OnRegister()
	{
	//
	// Register "ThisWorkspace", always as ID 1
	//
	HRESULT hr = ApcProjectItem.Register(ApcProject, this, (DWORD) 1);
	if (FAILED(hr))
		{
		TRACE0(_T("Workspace ApcProjectItem.Register failed"));
		return hr;
		}
	return S_OK;
	}

HRESULT CIcadWorkspace::ActProjectStorage_OnSave(IStorage* pStg)
	{
	//
	// Register "ThisWorkspace", always as ID 1
	//
	ASSERT (1 == ApcProjectItem.ID());
	
	return CActProjectStorage<CIcadWorkspace>::ActProjectStorage_OnSave(pStg);
	
	//
	// Calling a base class member when not using the namespace can be tricky,
	//    Either syntax shown below will work.  The type of _CActProjectStorage is 
	//    typedef'd in advLibrary.h.  
	// If using the SUMMIT namespace, simply call directly as shown above
	//
	// return _CActProjectStorage::ActProjectStorage_OnSave(pStg);
	// return ((CActProjectStorage<CIcadWorkspace>*) this)->ActProjectStorage_OnSave(pStg);
	//
	}



