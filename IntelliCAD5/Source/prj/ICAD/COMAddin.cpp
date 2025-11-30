#include "stdafx.h"

#include "Icad.h"
#include "IcadDoc.h"
#include "IcadApp.h"
#include "Preferences.h"
#include "IcadVbaWorkspace.h"
#include "AutoDoc.h"

#include "COMAddin.h"
#include "IDTExtensibility2.h"
#include "IcadDispID.h"

COMAddin::COMAddin()
{
}

COMAddin::~COMAddin()
{
}

HRESULT
COMAddin::load(CIcadApp* icad)
{
	HRESULT hr;
	IDispatch* this_app;

	hr = icad->m_pAutoApp->QueryInterface(&this_app);

	if (!SUCCEEDED(hr))
		return hr;

								// Retrieve class id for addin from registry

	PreferenceKey app_classid_key(HKEY_CURRENT_USER, "Application");
	HKEY hKey = app_classid_key;

	const int STRSIZE = 256;
	CString clsidstr('\0', STRSIZE);
	DWORD clsidstrtype;
	DWORD strsize = STRSIZE;

	LONG rval = RegQueryValueEx(hKey, 0, 0, &clsidstrtype,
								(LPBYTE) clsidstr.GetBuffer(strsize),
								&strsize);
	clsidstr.ReleaseBuffer();
	if (rval !=	ERROR_SUCCESS)
		return S_OK;			// Not an error, no addin registered

	if (clsidstr.IsEmpty())
		return S_OK;			// Not an error, no addin registered

	BSTR bstrname = clsidstr.AllocSysString();
	CLSID clsid;
	if (clsidstr[0] == '{')
	{
		hr = CLSIDFromString(bstrname, &clsid);
		if (!SUCCEEDED(hr))
			return hr;
	}
	else
	{
		hr = CLSIDFromProgID(bstrname, &clsid);
		if (!SUCCEEDED(hr))
			return hr;
	}
	::SysFreeString(bstrname);
	
								// Load addin and fire OnConnection
								// and OnStartupComplete
	hr = addin.CoCreateInstance(clsid, 0, CLSCTX_INPROC_SERVER);

	if (!SUCCEEDED(hr))
		return hr;

	IDTExtensibility2* iext = 0;
	hr = addin.QueryInterface(&iext);

	if (SUCCEEDED(hr))
	{
		hr = iext->OnConnection(this_app, ext_cm_Startup,
								addin, 0);
		if (!SUCCEEDED(hr))
		{
			iext->Release();
			iext = 0;
			return hr;
		}

		hr = iext->OnAddInsUpdate(0);
		if (!SUCCEEDED(hr))
		{
			iext->Release();
			iext = 0;
			return hr;
		}

		hr = iext->OnStartupComplete(0);
		if (!SUCCEEDED(hr))
		{
			iext->Release();
			iext = 0;
			return hr;
		}

		iext->Release();
		iext = 0;
	}
	else
	{
		addin.Release();
		return hr;
	}
	
	return S_OK;
}

HRESULT
COMAddin::destroy(void)
{
	if (!addin)
		return S_OK;			// Not an error, no addin registered

	IDTExtensibility2* iext = 0;
	HRESULT hr = addin.QueryInterface(&iext);

	if (SUCCEEDED(hr))
	{
		hr = iext->OnBeginShutdown(0);
	
		hr = iext->OnDisconnection(ext_dm_HostShutdown, 0);

		iext->Release();
		iext = 0;
	}

	addin.Release();

	return S_OK;
}

template <class SourceClass>
HRESULT
fireEvent(SourceClass* source,
		  int disp_id,
		  DISPPARAMS* parms)
{
	if (!source)
		return E_POINTER;

	int n_conn = source->m_vec.GetSize();

	for (int conn = 0; conn < n_conn; conn++)
	{
		source->Lock();
		CComPtr<IUnknown> sp = source->m_vec.GetAt(conn);
		source->Unlock();

		IDispatch* idisp = reinterpret_cast<IDispatch*>(sp.p);

		if (!idisp)
			continue;

		idisp->Invoke(disp_id,
					  IID_NULL, LOCALE_USER_DEFAULT,
					  DISPATCH_METHOD, parms,
					  0, 0, 0);
	}
	
	return S_OK;
}


HRESULT
COMAddin::fireAddinRun(IIcadWorkspace* icad_ws, const char* action)
{
	CIcadWorkspace* wsc = static_cast<CIcadWorkspace*>(icad_ws);

	if (!wsc)
		return E_POINTER;

	CComVariant act(action);
	VARIANT varArg[1];
	varArg[0] = act;

	DISPPARAMS parm;
	parm.rgvarg = varArg;
	parm.cArgs = 1;
	parm.rgdispidNamedArgs = NULL;
	parm.cNamedArgs = 0;
		
	return fireEvent(wsc, DISPID_WS_ADDINRUN, &parm);
}

HRESULT
COMAddin::fireWorkspaceExit(IIcadWorkspace* icad_ws)
{
	CIcadWorkspace* wsc = static_cast<CIcadWorkspace*>(icad_ws);

	if (!wsc)
		return E_POINTER;

	DISPPARAMS parm = {0, 0, 0, 0};
		
	return fireEvent(wsc, DISPID_WS_EXIT, &parm);
}

HRESULT
COMAddin::fireWorkspaceReinit(IIcadWorkspace* icad_ws)
{
	CIcadWorkspace* wsc = static_cast<CIcadWorkspace*>(icad_ws);

	if (!wsc)
		return E_POINTER;

	DISPPARAMS parm = {0, 0, 0, 0};
		
	return fireEvent(wsc, DISPID_WS_REINIT, &parm);
}

HRESULT
COMAddin::fireWorkspaceDocumentEvent(IIcadWorkspace* icad_ws,
									 int id, IIcadDocument* doc)
{
	CIcadWorkspace* wsc = static_cast<CIcadWorkspace*>(icad_ws);

	if (!wsc)
		return E_POINTER;

	CComVariant docp(static_cast<IDispatch*>(doc));
	VARIANT varArg[1];
	varArg[0] = docp;

	DISPPARAMS parm;
	parm.rgvarg = varArg;
	parm.cArgs = 1;
	parm.rgdispidNamedArgs = NULL;
	parm.cNamedArgs = 0;
		
	return fireEvent(wsc, id, &parm);
}
	
HRESULT
COMAddin::fireDocumentEvent(int id, CAutoDoc* doc)
{
	if (!doc)
		return E_POINTER;

	DISPPARAMS parm = {0, 0, 0, 0};
	
	return fireEvent(doc, id, &parm);
}

HRESULT
COMAddin::fireWorkspaceStartup(IIcadWorkspace* icad_ws)
{
	CIcadWorkspace* wsc = static_cast<CIcadWorkspace*>(icad_ws);

	if (!wsc)
		return E_POINTER;

	DISPPARAMS parm = {0, 0, 0, 0};
		
	return fireEvent(wsc, DISPID_WS_STARTUP, &parm);
}

HRESULT
COMAddin::fireWorkspaceBeforeExit(IIcadWorkspace* icad_ws)
{
	CIcadWorkspace* wsc = static_cast<CIcadWorkspace*>(icad_ws);

	if (!wsc)
		return E_POINTER;

	DISPPARAMS parm = {0, 0, 0, 0};
		
	return fireEvent(wsc, DISPID_WS_BEFOREEXIT, &parm);
}

