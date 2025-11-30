/* E:\INTELLICAD\PRJ\ICAD\VBACOMMPROJ.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Helper objects to handle the CommonProjects object.  
 * Original source is from the Summit APC Sample Library
 * Classes defined in this file:
 * - CActProjectStorageMember - aggregated implementation class for Project Storage behavior.
 * - CActProjectStorage - contains a member variable named "ActProjectStorage" of type CActProjectStorageMember.
 * - IActProjectStorageImpl - used to add this functionality to a pure COM class.
 * 
 */ 

#ifndef __VBACOMMPROJ_H__
#define __VBACOMMPROJ_H__

//
//
// Use of this class (in an ATL class):
// 1. Derive your own ATL class from CActProjectStorage and IActProjectStorageImpl.
// 2. Implement the Project Item primary and source interfaces in the ATL class.
// 3. Also derive this ATL class from CApcProject and CApcProjectItem [optional].
// 4. Implement the ActProjectStorage_OnRegister, _OnDefine and _OnClose overrides for Project Item maintenance.
// 5. Redirect the ApcProject_Save handler to ActProjectStorage.Save
// 6. Create an instance of the ATL class.
// 7. QI for IActProjectStorage (provided by the IActProjectStorageImpl base).
// 8. Call IActProjectStorage->Open or IActProjectStorage->CreateNew to initialize.
// 9. Call IActProjectStorage->Save to save where appropriate
// 10. Call IActProjectStorage->Close to close prior to shutdown

// TODO: remove dependence on MFC::CString, replace with STD::string

#include <stdlib.h>
#include <io.h>
#include <string.h>

#include "IcadApp.h"/*DNT*/
#include "LicensedComponents.h"

#define SUMMIT_MAKE_HRESULT(__err__) MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, __err__ + 5000)

#define SUMMIT_E_FILE_NOT_EXIST	SUMMIT_MAKE_HRESULT(100)
#define SUMMIT_E_OPEN_FAILURE	SUMMIT_MAKE_HRESULT(101)
#define SUMMIT_E_CREATE_FAILURE	SUMMIT_MAKE_HRESULT(102)

namespace SUMMIT 
{
struct __declspec(uuid("461EAB70-E395-11d2-8C75-00104B2B716B")) IActProjectStorage : IUnknown
	{
	STDMETHOD(Open)(IApc* pApcHost, BSTR bstrFileName, BOOL bDisableMacros) = 0;
	STDMETHOD(Save)() = 0;
	STDMETHOD(CreateNew)(IApc* pApcHost, BSTR bstrFileName) = 0;
	STDMETHOD(ReferenceToSelf)(IApcProject* pReferencingProject) = 0;
	STDMETHOD(Close)() = 0;
	STDMETHOD(put_ProjectName)(BSTR bstrName) = 0;
	STDMETHOD(get_ProjectName)(BSTR* pbstrName) = 0;
	};


template <class _ActProjectStorageOwner>
class CActProjectStorageMember
	{
	_ActProjectStorageOwner* m_pActProjectStorageOwner;
	public:
		CActProjectStorageMember(_ActProjectStorageOwner* pActProjectStorageOwner) :
		  m_pActProjectStorageOwner(pActProjectStorageOwner)
			  {
			  _ASSERTE(pActProjectStorageOwner);
			  }
		  ~CActProjectStorageMember()
			  {
			  }
		  
		  //
		  // IActProjectStorage
		  //
		  STDMETHOD(Open)(IApc* pApcHost, BSTR bstrFileName, BOOL bDisableMacros)
			  {
			  HRESULT hr;
			  
			  hr = m_pActProjectStorageOwner->ActProjectStorage_OnOpen(pApcHost, bstrFileName, bDisableMacros);
			  
			  if (FAILED(hr))
				  m_bstrApcFileName = "";
			  
			  return hr;
			  }
		  
		  HRESULT OnOpen(IApc* pApcHost, BSTR bstrFileName, BOOL bDisableMacros)
			  {
			  HRESULT hr = S_OK;
			  
			  m_bstrApcFileName = bstrFileName;
			  CString szFileName = bstrFileName;
			  
			  // Q: does file exist?
			  if (0 != _taccess(szFileName,0))
				  {
				  // N: Failure
				  return SUMMIT_E_FILE_NOT_EXIST;
				  }
			  else
				  {
				  // Y: file exists, try to open it
				  hr = OpenFromFile(pApcHost, bstrFileName, bDisableMacros);
				  if (FAILED(hr))
					  return SUMMIT_E_OPEN_FAILURE;
				  }
			  
			  return hr;
			  }
		  
		  STDMETHOD(Save)()
			  {
			  HRESULT hr;
			  
			  ApcCallHR(m_pActProjectStorageOwner->ActProjectStorage_OnSave(m_spApcProjectStg));
			  
			  return S_OK;
			  }
		  
		  STDMETHOD(CreateNew)(IApc* pApcHost, BSTR bstrFileName)
			  {
			  m_bstrApcFileName = bstrFileName;
			  
			  //
			  // Open Docfile
			  //
			  HRESULT hr;
			  hr = StgCreateDocfile(bstrFileName, 
				  STGM_TRANSACTED | STGM_READWRITE | STGM_SHARE_DENY_WRITE, 
				  0, &m_spApcProjectStg);
			  if (FAILED(hr))
				  {
				  m_bstrApcFileName = "";
				  return hr;
				  }
			  
			  hr = m_pActProjectStorageOwner->ActProjectStorage_OnCreateNew(pApcHost, bstrFileName, m_spApcProjectStg);
			  
			  if (FAILED(hr))
				  {
				  m_spApcProjectStg.Release(); 
				  CString szFileName = bstrFileName;
				  DeleteFile(szFileName); // clean up StgCreateDocfile
				  m_bstrApcFileName = "";
				  }
			  
			  return hr;
			  }
		  
		  HRESULT OnCreateNew(IApcProject* pApcProject)
			  {
			  HRESULT hr;
			  //
			  // Specify name
			  //
			  if (pApcProject)
				  ApcCallHR(pApcProject->APC_PUT(Name)(m_bstrApcProjectName));
			  
			  //
			  // Customization entry point
			  //
			  ApcCallHR(m_pActProjectStorageOwner->ActProjectStorage_OnDefine());
			  
			  //
			  // Save file after creation
			  //
			  ApcCallHR(Save());
			  
			  return S_OK;
			  }
		  
		  STDMETHOD(ReferenceToSelf)(IApcProject* pReferencingProject)
			  {
#ifdef BUILD_WITH_VBA
				if(CIcadApp::m_LicensedComponents.IsAccessible(CLicensedComponents::Components::VBA))
				{
				  CComPtr<VBIDE::_VBProject> spProj;
				  
	#ifdef APC_IMPORT_MIDL
				  HRESULT hr = pReferencingProject->get_VBProject((VBIDE::VBProject**) &spProj);
	#else
				  HRESULT hr = pReferencingProject->get_VBProject(&spProj);
	#endif
				  
				  if (SUCCEEDED(hr)) 
					  {
					  CComPtr<VBIDE::_References> sp_Refs;
					  CComPtr<VBIDE::Reference> spRef;
					  
	#ifdef APC_IMPORT_MIDL
					  hr = spProj->get_References((VBIDE::References**) &sp_Refs);
	#else
					  hr = spProj->get_References(&sp_Refs);
	#endif
					  
					  if (SUCCEEDED(hr)) 
						  hr = sp_Refs->APC_RAW(AddFromFile)(m_bstrApcFileName, &spRef);
					  }
				  return hr;
				}
#endif //BUILD_WITH_VBA
			  return NOERROR;
			  }
		  
		  STDMETHOD(Close)()
			  {
			  HRESULT hr;
			  
			  ApcCallHR(m_pActProjectStorageOwner->ActProjectStorage_OnClose());
			  
			  return S_OK;
			  }
		  
		  STDMETHOD(put_ProjectName)(BSTR bstrName)
			  {
			  m_bstrApcProjectName = bstrName;
			  return S_OK;
			  }
		  
		  STDMETHOD(get_ProjectName)(BSTR* pbstrName)
			  {
			  *pbstrName = m_bstrApcProjectName.Copy();
			  return S_OK;
			  }
		  
	protected:
		HRESULT OpenFromFile(IApc* pApcHost, BSTR bstrFileName, BOOL bDisableMacros)
			{
			//
			// Open Storage
			//
			HRESULT hr;
			BOOL bWarnReadOnly = FALSE;
			hr = StgOpenStorage(bstrFileName, NULL, 
				STGM_TRANSACTED | STGM_READWRITE | STGM_SHARE_DENY_WRITE, 
				0, 0, &m_spApcProjectStg);
			
			// If we cannot open with READWRITE permission, try READONLY.
			if (FAILED(hr)) 
				{
				bWarnReadOnly = TRUE;
				hr = StgOpenStorage(bstrFileName, NULL,
					//STGM_TRANSACTED | STGM_READ | STGM_SHARE_EXCLUSIVE,
					STGM_TRANSACTED | STGM_READ | STGM_SHARE_DENY_NONE,
					0, 0, &m_spApcProjectStg);
				}
			
			if (FAILED(hr))
				return hr;
			
			hr = OpenProjectFromStg(pApcHost, bDisableMacros);
#ifndef BUILD_WITH_VBA
			long APC_E_VBAMACROENABLE_DONTOPEN = -2147220090;
#endif
			if (hr == APC_E_VBAMACROENABLE_DONTOPEN)
				{
				//the user hit cancel on the dialog.  If this is common projects, try to open it with disable macros
				hr = OpenProjectFromStg(pApcHost, true);
				}

			if (FAILED(hr))
				return hr;
			
			// Success!
			return (bWarnReadOnly ? S_FALSE : S_OK);
			}
		
		
		HRESULT OpenProjectFromStg(IApc* pApcHost, BOOL bDisableMacros)
			{
			HRESULT hr;
			
			ApcCallHR(m_pActProjectStorageOwner->ActProjectStorage_OnRegister());
			ApcCallHR(m_pActProjectStorageOwner->ActProjectStorage_OnOpenStorage(pApcHost, m_spApcProjectStg, bDisableMacros));
			
			return S_OK;
			}
		
		CComBSTR m_bstrApcProjectName;
		CComBSTR m_bstrApcFileName;
		CComPtr<IStorage> m_spApcProjectStg;
	};
	
	template <class T, class _ActProjectStorageMember = CActProjectStorageMember<T> >
		class CActProjectStorage
		{
	public:
		typedef CActProjectStorage<T, _ActProjectStorageMember> _CActProjectStorage;
		// 
		// Members
		// 
		_ActProjectStorageMember ActProjectStorage;
		
		// 
		// Lifetime
		// 
		CActProjectStorage() : ActProjectStorage(static_cast<T*>(this)) 
			{
			}
		
		//
		// Overrides
		//
		HRESULT ActProjectStorage_OnDefine()
			{
			return S_OK;
			}
		
		HRESULT ActProjectStorage_OnRegister()
			{
			return S_OK;
			}
		
		HRESULT ActProjectStorage_OnOpen(IApc* pApcHost, BSTR bstrFileName, BOOL bDisableMacros)
			{
			return ActProjectStorage.OnOpen(pApcHost, bstrFileName, bDisableMacros);
			}
		
		HRESULT ActProjectStorage_OnSave(IStorage* pStg)
			{
			HRESULT hr;
			
			T* pT = static_cast<T*>(this);
			
			hr = pT->ApcProject.Save(pStg, TRUE);
			if (FAILED(hr))
				return hr;
			
			hr = pStg->Commit(STGC_DEFAULT);
			if (FAILED(hr))
				{
				pT->ApcProject.SaveCompleted(NULL);
				return hr;
				}
			
			hr = pT->ApcProject.SaveCompleted(NULL);
			_ASSERTE(SUCCEEDED(hr)); // if SaveCompleted failed, something's really wrong
			if (FAILED(hr))
				return hr;
			
			return S_OK;
			}
		
		HRESULT ActProjectStorage_OnCreateNew(IApc* pApcHost, BSTR bstrFileName, IStorage* pStg)
			{	
			HRESULT hr;
			T* pT = static_cast<T*>(this);
			
			//
			// Create Project
			//
			hr = pT->ApcProject.Create(pApcHost, axProjectNormal);
			if (FAILED(hr))
				return hr;
			
			//
			// Pass IStorage to Project
			//
			hr = pT->ApcProject.InitNew(pStg);
			if (FAILED(hr))
				{
				pT->ApcProject.Close(); // clean up VaoProject.Create
				return hr;
				}
			
			
			hr = ActProjectStorage.OnCreateNew(pT->ApcProject);
			
			if (FAILED(hr))
				{
				pT->ApcProject.Close(); // clean up ApcProject.Create
				return hr;
				}
			
			return S_OK;
			}
		
		HRESULT ActProjectStorage_OnClose()
			{
			T* pT = static_cast<T*>(this);
			pT->ApcProject.Close();
			return S_OK;
			}
		
		HRESULT ActProjectStorage_OnOpenStorage(IApc* pApcHost, IStorage* pStg, BOOL bDisableMacros)
			{
			HRESULT hr;
			T* pT = static_cast<T*>(this);
			
			//
			// Open, Load, FinishLoading
			//
			AxProjectFlag flag = axProjectNormal;
			if (bDisableMacros)
				flag = axProjectDisableMacros;
			hr = pT->ApcProject.Open(pApcHost, flag);
			if (FAILED(hr))
				return hr;
			
			hr = pT->ApcProject.Load(pStg);
			if (FAILED(hr))
				{
				pT->ApcProject.Close(); // clean up VaoProject.Open
				return hr;
				}
			
			hr = pT->ApcProject.FinishLoading();
			if (FAILED(hr))
				{
				pT->ApcProject.Close(); // clean up VaoProject.Open & VaoProject.Load
				return hr;
				}
			
			return S_OK;
			}
	};
	
	template <class T>
		class ATL_NO_VTABLE IActProjectStorageImpl : public IActProjectStorage
		{
	public:
		IActProjectStorageImpl()
			{
			}
		~IActProjectStorageImpl()
			{
			}
		
		//
		// IActProjectStorage
		//
		STDMETHOD(Open)(IApc* pApcHost, BSTR bstrFileName, BOOL bDisableMacros)
			{
			T* pT = static_cast<T*>(this);
			return pT->ActProjectStorage.Open(pApcHost, bstrFileName, bDisableMacros);
			}
		STDMETHOD(Save)()
			{
			T* pT = static_cast<T*>(this);
			return pT->ActProjectStorage.Save();
			}
		STDMETHOD(CreateNew)(IApc* pApcHost, BSTR bstrFileName)
			{
			T* pT = static_cast<T*>(this);
			return pT->ActProjectStorage.CreateNew(pApcHost, bstrFileName);
			}
		STDMETHOD(ReferenceToSelf)(IApcProject* pReferencingProject)
			{
			T* pT = static_cast<T*>(this);
			return pT->ActProjectStorage.ReferenceToSelf(pReferencingProject);
			}
		STDMETHOD(Close)()
			{
			T* pT = static_cast<T*>(this);
			return pT->ActProjectStorage.Close();
			}
		STDMETHOD(put_ProjectName)(BSTR bstrName)
			{
			T* pT = static_cast<T*>(this);
			return pT->ActProjectStorage.put_ProjectName(bstrName);
			}
		STDMETHOD(get_ProjectName)(BSTR* pbstrName)
			{
			T* pT = static_cast<T*>(this);
			return pT->ActProjectStorage.get_ProjectName(pbstrName);
			}
		};

} //namespace SUMMIT

#endif


