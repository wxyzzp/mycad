#include "StdAfx.h"
#include "COMAddinManager.h"

#include "Icad.h"
#include "IcadDoc.h"
#include "IcadApp.h"
#include "Preferences.h"
#include "IcadVbaWorkspace.h"
#include "AutoDoc.h"

#include "IcadDispID.h"

#include "Msaddndr.tlh"
using namespace AddInDesignerObjects;

#ifndef _COMAddinManagerDlg_H
#include "COMAddinManagerDlg.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////
// CCOMAddinStatus
////////////////////////////////////////////////////////////////
CCOMAddinStatus::CCOMAddinStatus() 
              : m_bLoaded(false), m_loadBehavior(0), m_bWillBeLoaded(false), m_commandLineSafe (0)
{
}

CCOMAddinStatus::CCOMAddinStatus(const CCOMAddinStatus& status)
{
	*this = status;
}

CCOMAddinStatus& CCOMAddinStatus::operator=( const CCOMAddinStatus& status )
{
	if ( this != &status )
	{
		m_description   = status.m_description;
		m_friendlyName  = status.m_friendlyName;    
		m_loadBehavior  = status.m_loadBehavior;
		m_bLoaded       = status.m_bLoaded;
		m_bWillBeLoaded = status.m_bWillBeLoaded;
		m_fullDllName   = status.m_fullDllName;
		m_commandLineSafe = status.m_commandLineSafe;
	}

    return *this;
}

////////////////////////////////////////////////////////////////
// CCOMAddin
////////////////////////////////////////////////////////////////
class CCOMAddin
{
private:
    _IDTExtensibility2 * m_pIDTExtensibility2;
	CString m_progID;
public:
	CCOMAddinStatus m_status;

public:
//	CCOMAddin () : m_pIDTExtensibility2 (NULL) {}
    CCOMAddin (LPCTSTR pProgID); 
    ~CCOMAddin();
private:
	CCOMAddin (const CCOMAddin &other);
	CCOMAddin & operator = (const CCOMAddin &other);
public:
    // creates instance and calls OnConnection method
    // connectMode = 
    //  ext_cm_Startup  - if the add-in was loaded when the host called HostAddIns.LoadHostAddIns
    //  ext_cm_AfterStartup - if the add-in was loaded using the Add-In Manager dialog box
    //  ext_cm_External     - if the add-in was loaded by another add-in
    // return 1 if success, 0 if not
    int connect( IDispatch* pIcadApp, ext_ConnectMode connectMode );
    // calls OnStartupComplete method
    // return 1 if success, 0 if not
    int startupComplete();
    // calls OnBeginShutdown method
    // return 1 if success, 0 if not
    int beginShutdown();
    // calls OnDisconnection method and destroy addin
    // disconnectMode = 
    //  ext_dm_HostShutdown - if the host called HostAddIns. NotifyBeginShutdown 
    //  ext_dm_UserClosed - if the add-in was removed using the Add-In Manager dialog box.
    // return 1 if success, 0 if not
    int disconnect(ext_DisconnectMode disconnectMode);
    // Called when changes are made to the add-ins collection, 
    // either using the Add-In Manager dialog box or programatically
    // return 1 if success, 0 if not
    int addInsUpdate();

	int setLoadBehavior(int loadBehavior);
};

CCOMAddin::CCOMAddin (LPCTSTR pProgID) :
            m_pIDTExtensibility2(NULL), m_progID (pProgID)
{
}

CCOMAddin::~CCOMAddin()
{
    disconnect(ext_dm_HostShutdown);
    if( m_pIDTExtensibility2 )
    {
        m_pIDTExtensibility2->Release();
        m_pIDTExtensibility2 = NULL;
    }
}


// creates instance and calls OnConnection method
int CCOMAddin::connect( IDispatch* pIcadApp, ext_ConnectMode connectMode )
{
    assert( pIcadApp );

    CLSID addinClsid;
    BSTR bstrProgId = m_progID.AllocSysString();
    
    HRESULT hr = ::CLSIDFromProgID(bstrProgId, &addinClsid);
    ::SysFreeString(bstrProgId);
    if ( FAILED(hr) )
        return 0;

    try 
    {
        _IDTExtensibility2Ptr extPtr;
        hr = extPtr.CreateInstance(addinClsid);
        if ( FAILED(hr) )
            return 0;
        m_pIDTExtensibility2 = extPtr;
        m_pIDTExtensibility2->AddRef();
    }
    catch(_com_error& )
    {   return 0;
    }

    // calls OnConnection if m_loadBehavior == 1 or if addin loads from dialog
    if ( m_status.m_loadBehavior || connectMode == ext_cm_AfterStartup )
    {
        SAFEARRAY* psa = ::SafeArrayCreateVector( VT_VARIANT, 1, 1);
        try {
            m_pIDTExtensibility2->OnConnection(pIcadApp, connectMode, m_pIDTExtensibility2, &psa);
        }
        catch (_com_error& ) 
        {   ; // do nothing
        }
        ::SafeArrayDestroy(psa);
		m_status.m_bLoaded = true;
    }
    else
    {
        m_pIDTExtensibility2->Release();    // add-in will be loaded from dialog
        m_pIDTExtensibility2 = NULL;        //  But information about add-in will be added to the list
		m_status.m_bLoaded = false;
    }
    return 1;
}

// calls OnStartupComplete method
int CCOMAddin::startupComplete()
{
    int ok = 0;
    if ( m_pIDTExtensibility2 )
    {
        SAFEARRAY* psa = ::SafeArrayCreateVector( VT_VARIANT, 1, 1);
        try {   
            m_pIDTExtensibility2->OnStartupComplete(&psa);
            ok = 1;
        }
        catch (_com_error& ) 
        {   ; // do nothing
        }
        ::SafeArrayDestroy(psa);
    }
    return ok;
}

int CCOMAddin::beginShutdown()
{
    int ok = 0;
    if ( m_pIDTExtensibility2 )
    {
        SAFEARRAY* psa = ::SafeArrayCreateVector( VT_VARIANT, 1, 1);
        try {   
            m_pIDTExtensibility2->OnBeginShutdown(&psa);
            ok = 1;
        }
        catch (_com_error& ) 
        {   ; // do nothing
        }
        ::SafeArrayDestroy(psa);
    }
    return ok;
}

// calls OnDisconnection method and destroy addin
int CCOMAddin::disconnect(ext_DisconnectMode disconnectMode)
{
    if ( m_pIDTExtensibility2 )
    {
        SAFEARRAY* psa = ::SafeArrayCreateVector( VT_VARIANT, 1, 1);
        try {   
            m_pIDTExtensibility2->OnDisconnection(disconnectMode, &psa);
        }
        catch (_com_error& ) 
        {   ; // do nothing
        }
        ::SafeArrayDestroy(psa);
        m_pIDTExtensibility2->Release();
        m_pIDTExtensibility2 = NULL;
		m_status.m_bLoaded = false;
        return 1;
    }
    return 0;
}

int CCOMAddin::addInsUpdate()
{
    if ( m_pIDTExtensibility2 )
    {
        try {   
            m_pIDTExtensibility2->OnAddInsUpdate(NULL);
        }
        catch (_com_error& )
        {   ; // do nothing
        }
        return 1;
    }
    return 0;
}

int CCOMAddin::setLoadBehavior(int loadBehavior)
{
	PreferenceKey addinsKey (HKEY_CURRENT_USER,"Addins",false);
	CRegKey regAddin;
	if ( ERROR_SUCCESS == regAddin.Open (addinsKey,m_progID) )
	{
		DWORD   value = (loadBehavior > 0 ? 1 : 0);
        if ( regAddin.SetValue (value,_T("LoadBehavior")) == ERROR_SUCCESS )
		{
			m_status.m_loadBehavior = value;
			return 1;
		}
	}
    return 0;
}

////////////////////////////////////////////////////////////////
// CCOMAddinManager
////////////////////////////////////////////////////////////////
CCOMAddinManager::CCOMAddinManager() : m_pIcadApp (NULL)
{
}

CCOMAddinManager::~CCOMAddinManager()
{
}

HRESULT
CCOMAddinManager::load(CIcadApp* icad)
{
	HRESULT hr;

	hr = icad->m_pAutoApp->QueryInterface(&m_pIcadApp);

	if ( !SUCCEEDED (hr) )
		return hr;

	// Retrieve class id for addins from registry
	PreferenceKey addinsKey (HKEY_CURRENT_USER,"Addins",false);
	HKEY hKey = addinsKey;
	DWORD dwIndex = 0;
	DWORD cbName = MAX_PATH;
	TCHAR lpName[MAX_PATH + 1];
	PFILETIME pIgnore = NULL;

	//Bugzilla::78318; 28-04-03
	if ( hKey == NULL )
		return E_HANDLE;

	long res = RegEnumKeyEx (hKey,dwIndex++,(LPTSTR)lpName,&cbName,NULL,NULL,NULL,pIgnore);
	while ( res != ERROR_NO_MORE_ITEMS ) 
	{
		loadAddin (lpName,hKey);
		cbName = MAX_PATH;	/*DG - 7.4.2003*/// cbName is <=> param, so reset it every time.
		res = RegEnumKeyEx (hKey,dwIndex++,lpName,&cbName,NULL,NULL,NULL,pIgnore);
	}

	return S_OK;
}

void CCOMAddinManager::loadAddin (LPTSTR lpName,HKEY hRegKeyParent)
{
	CCOMAddin * pAddin = new CCOMAddin (lpName);

	CRegKey regAddin;
	if ( ERROR_SUCCESS == regAddin.Open (hRegKeyParent,lpName) )
	{
		DWORD bufSize = MAX_PATH;
		TCHAR buf[MAX_PATH] = _T("");
		if ( regAddin.QueryValue (buf,_T("FriendlyName"),&bufSize) == ERROR_SUCCESS )
			pAddin->m_status.m_friendlyName = buf;
		else 
			pAddin->m_status.m_friendlyName.Empty();

		bufSize = MAX_PATH;
		if ( regAddin.QueryValue( buf, _T("Description"), &bufSize) == ERROR_SUCCESS )
			pAddin->m_status.m_description = buf;
		else 
			pAddin->m_status.m_description.Empty();

		DWORD value;
		if ( regAddin.QueryValue( value, _T("LoadBehavior") ) == ERROR_SUCCESS )
			pAddin->m_status.m_loadBehavior = long(value);
		else 
			pAddin->m_status.m_loadBehavior = 0;

		if ( regAddin.QueryValue( value, _T("CommandLineSafe") ) == ERROR_SUCCESS )
			pAddin->m_status.m_commandLineSafe = long(value);
		else 
			pAddin->m_status.m_commandLineSafe = 0;

		pAddin->connect (m_pIcadApp,ext_cm_Startup);
		m_addins.Add (pAddin);
	}
}

HRESULT
CCOMAddinManager::unload(void)
{
	for ( int i=0, n=m_addins.GetSize (); i<n; i++ )
        m_addins[i]->beginShutdown (); 
    
    for (i=0; i<n; i++)
	{
		delete m_addins[i];
		m_addins[i] = NULL;
	}
    
	m_addins.RemoveAll ();

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
CCOMAddinManager::fireAddinRun(IIcadWorkspace* icad_ws, const char* action)
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
CCOMAddinManager::fireWorkspaceExit(IIcadWorkspace* icad_ws)
{
	CIcadWorkspace* wsc = static_cast<CIcadWorkspace*>(icad_ws);

	if (!wsc)
		return E_POINTER;

	DISPPARAMS parm = {0, 0, 0, 0};
		
	return fireEvent(wsc, DISPID_WS_EXIT, &parm);
}

HRESULT
CCOMAddinManager::fireWorkspaceReinit(IIcadWorkspace* icad_ws)
{
	CIcadWorkspace* wsc = static_cast<CIcadWorkspace*>(icad_ws);

	if (!wsc)
		return E_POINTER;

	DISPPARAMS parm = {0, 0, 0, 0};
		
	return fireEvent(wsc, DISPID_WS_REINIT, &parm);
}

HRESULT
CCOMAddinManager::fireWorkspaceDocumentEvent(IIcadWorkspace* icad_ws,
									 int id, IIcadDocument* doc)
{
	CIcadWorkspace* wsc = static_cast<CIcadWorkspace*>(icad_ws);

	if (!wsc)
		return E_POINTER;

	CComVariant docp(doc);
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
CCOMAddinManager::fireDocumentEvent(int id, CAutoDoc* doc)
{
	if (!doc)
		return E_POINTER;

	DISPPARAMS parm = {0, 0, 0, 0};
	
	return fireEvent(doc, id, &parm);
}

HRESULT
CCOMAddinManager::fireWorkspaceStartup(IIcadWorkspace* icad_ws)
{
	CIcadWorkspace* wsc = static_cast<CIcadWorkspace*>(icad_ws);

	if (!wsc)
		return E_POINTER;

	DISPPARAMS parm = {0, 0, 0, 0};
		
	return fireEvent(wsc, DISPID_WS_STARTUP, &parm);
}

HRESULT
CCOMAddinManager::fireWorkspaceBeforeExit(IIcadWorkspace* icad_ws)
{
	CIcadWorkspace* wsc = static_cast<CIcadWorkspace*>(icad_ws);

	if (!wsc)
		return E_POINTER;

	DISPPARAMS parm = {0, 0, 0, 0};
		
	return fireEvent(wsc, DISPID_WS_BEFOREEXIT, &parm);
}

int	CCOMAddinManager::getAddInsStatus(CArray<CCOMAddinStatus,CCOMAddinStatus&> &addins)
{
	addins.RemoveAll ();
    CCOMAddinStatus status;

    for (int i=0, n=m_addins.GetSize (); i<n; i++)
        addins.Add(m_addins[i]->m_status);
    return addins.GetSize();
}

void CCOMAddinManager::correctAddInsStatus(CArray<CCOMAddinStatus,CCOMAddinStatus&> &addins)
{
	int numAddins = m_addins.GetSize ();
	for ( int i=0, n=addins.GetSize(); i<n && i<numAddins; i++ )
    {
		int addinIndex = findByName (addins[i].m_friendlyName);
		if ( -1 != addinIndex ) 
		{
			if ( m_addins[addinIndex]->m_status.m_loadBehavior != addins[i].m_loadBehavior )
				m_addins[addinIndex]->setLoadBehavior(addins[i].m_loadBehavior);

			if ( m_addins[addinIndex]->m_status.m_bLoaded && !addins[i].m_bWillBeLoaded )
				m_addins[addinIndex]->disconnect(ext_dm_UserClosed);
    
			if ( !m_addins[addinIndex]->m_status.m_bLoaded && addins[i].m_bWillBeLoaded )
			{
				m_addins[addinIndex]->connect(m_pIcadApp, ext_cm_AfterStartup);
				m_addins[addinIndex]->addInsUpdate();
			}
		}
    }
}

int CCOMAddinManager::findByName(LPCTSTR name) const
{
    for (int i=0,n=m_addins.GetSize (); i<n; i++ )
        if ( m_addins[i]->m_status.m_friendlyName.Compare (name) == 0 )
			return i;
    return -1;
}

void CCOMAddinManager::showDialog ()
{
	CCOMAddinManagerDlg dlg;
	dlg.DoModal();
}
