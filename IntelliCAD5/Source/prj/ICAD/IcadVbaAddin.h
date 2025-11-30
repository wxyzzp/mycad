/* INTELLICAD\PRJ\LIB\AUTO\APCADDIN.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * The APC Host Addin object - original source from Summit advAddin.h
 * 
 */ 



#ifndef __APCADDIN_H__
#define __APCADDIN_H__

template<class Owner>
class CApcHostAddInsMember : public CApcComPtr<IApcHostAddIns>
	{
	public:
		CApcHostAddInsMember(Owner* pOwner) : m_pOwner(pOwner)
			{
			ASSERT(m_pOwner);
			}
		
		~CApcHostAddInsMember()
			{
			_ASSERTE(!GetApcHostAddIns()); // forgot to call Destroy?
			if (GetApcHostAddIns())
				m_pDispHost.Release(); // still fully attached
			else
				OnDestroy(); // not fully attached, so quietly shut down
			}
		
		IApcHostAddIns* GetApcHostAddIns() { return *this; }
		
		//ApcHostAddIns.Create
		HRESULT Create(
			IDispatch* pDispHost,
			BOOL fRemoveOnFailure = TRUE, 
			BOOL fDisableOnStartup = FALSE,
			BSTR bstrLicKey = NULL, 
			long nLcid = 0,
			BSTR bstrRegPath = NULL)
			{
			ApcEntryHR(CApcHostAddInsMember::Create);
			
			_ASSERTE(pDispHost);
			
			hr = _Create(pDispHost, fRemoveOnFailure, fDisableOnStartup, bstrLicKey, nLcid, bstrRegPath);
			if (FAILED(hr))
				Destroy();
			
			ApcReturnHR(hr);
			}
		
		HRESULT Load()
			{
			ApcEntryHR(CApcHostAddInsMember::Load);
			
			ApcCallHR(GetApcHostAddIns()->APC_RAW(LoadHostAddIns)());
			ApcCallHR(GetApcHostAddIns()->APC_RAW(NotifyStartupComplete)());
			
			ApcReturnHR(S_OK);
			}
		
		// ApcHostAddIns.OnCreate
		HRESULT OnCreate(
			IDispatch* pDispHost,
			BOOL fRemoveOnFailure, 
			BOOL fDisableOnStartup,
			BSTR bstrLicKey,
			long nLcid,
			BSTR bstrRegPath)
			
			{
			ApcEntryHR(CApcHostAddIns::Attach);
			
			m_pDispHost = pDispHost;
			ApcCallHR(GetApcHostAddIns()->APC_PUT(HostObject)(m_pDispHost));
			ApcCallHR(GetApcHostAddIns()->APC_PUT(RemoveOnFailure)(ApcInVARIANT_BOOL(fRemoveOnFailure)));
			ApcCallHR(GetApcHostAddIns()->APC_PUT(DisableOnStartup)(ApcInVARIANT_BOOL(fDisableOnStartup)));
			
			if (bstrLicKey)
				ApcCallHR(GetApcHostAddIns()->APC_PUT(LicenseKey)(bstrLicKey));
			if (nLcid)
				ApcCallHR(GetApcHostAddIns()->APC_PUT(Locale)(nLcid));
			if (bstrRegPath)
				ApcCallHR(GetApcHostAddIns()->APC_PUT(RegistryPath)(bstrRegPath));
			
			// success!
			ApcReturnHR(NOERROR);
			}
		
		// ApcHost.Destroy
		void Destroy()
			{
			ApcEntryV(CApcHostAddIns::Destroy);
			
			ApcNotifyV(("ApcHostAddIns_OnDestroy"), m_pOwner->ApcHostAddIns_OnDestroy());
			}
		
		// ApcHost.OnDestroy
		void OnDestroy()
			{
			ApcEntryV(CApcHostAddIns::OnDestroy);
			
			if (GetApcHostAddIns())
				{
				HRESULT hr = GetApcHostAddIns()->APC_RAW(NotifyBeginShutdown)();
				ASSERT(SUCCEEDED(hr)); // did you forget to double click on the advAddin.reg file?
				if (FAILED(hr))
					AfxMessageBox("This assert indicates that you didn't double-click on the advAddin.reg file");
				
				}
			
			if (m_pDispHost)
				m_pDispHost.Release();
			
			if (GetApcHostAddIns())
				_ApcComPtr::OnDetach();
			}
		
	private:
		HRESULT _Create(
			IDispatch* pDispHost,
			BOOL fRemoveOnFailure, 
			BOOL fDisableOnStartup,
			BSTR bstrLicKey, 
			long nLcid,
			BSTR bstrRegPath)
			{
			ApcEntryHR(CApcHostAddInsMember::_Create);
			
			CComPtr<IApcHostAddIns> spApcHostAddIns;
			ApcCallHR(::CoCreateInstance( APC_CLSID(HostAddIns), 
				NULL, CLSCTX_INPROC_SERVER, APC_IID(IApcHostAddIns), 
				(void**)&spApcHostAddIns));
			
			_ApcComPtr::OnAttach(spApcHostAddIns);
			
			ApcCallHR(ApcNotifyHR(("ApcHostAddIns_OnCreate"), m_pOwner->ApcHostAddIns_OnCreate(
				pDispHost, fRemoveOnFailure, fDisableOnStartup, bstrLicKey, nLcid, bstrRegPath)));
			
			ApcReturnHR(S_OK);
			}
		
	protected:
		Owner* m_pOwner;
		CComPtr<IDispatch> m_pDispHost;
	};

template <class T, class _AddInsMember = CApcHostAddInsMember<T> >
class CApcHostAddIns
	{
	public:
		_AddInsMember ApcHostAddIns;
		
		CApcHostAddIns() : ApcHostAddIns(static_cast<T*>(this))
			{
			}
		
		// ApcHost_OnCreate - called by ApcHost.Create
		HRESULT ApcHostAddIns_OnCreate(
			IDispatch* pDispHost,
			BOOL fRemoveOnFailure, 
			BOOL fDisableOnStartup,
			BSTR bstrLicKey, 
			long nLcid,
			BSTR bstrRegPath)
			{
			ApcEntryHR(CApcHostAddIns::ApcHostAddIns_OnCreate);
			
			ApcCallHR(ApcHostAddIns.OnCreate(pDispHost, fRemoveOnFailure, fDisableOnStartup, bstrLicKey, nLcid, bstrRegPath));
			
			ApcReturnHR(NOERROR);
			}
		
		// ApcHost_OnDestroy - called by ApcHost.Destroy
		void ApcHostAddIns_OnDestroy()
			{
			ApcHostAddIns.OnDestroy();
			}
/*		
		//{{AFX_MSG(CApcHostAddIns)
		afx_msg void OnVbaAddIns()
			{
			if (ApcHostAddIns)
				{
				HRESULT hr = ApcHostAddIns->ShowAddInsDialog(long(AfxGetMainWnd()->m_hWnd));
				ASSERT(SUCCEEDED(hr));
				}
			}
		//}}AFX_MSG
*/		
	};

#endif //__APCADDIN_H__



