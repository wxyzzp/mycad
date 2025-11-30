/* INTELLICAD\PRJ\LIB\AUTO\AUTOAPP.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * Application Automation Peer Class Implementation
 * This is the implementation the Automation Interface - IIcadApplication
 *
 */


#include "StdAfx.h"
#include "AutoApp.h"
#include "IcadApi.h"
#include "IcadMain.h"
#include "IcadChildWnd.h"
#include "IcadDocTemplate.h"
#include "PrintDialog.h"
#include "autorc.h"
#include "AutoDocuments.h"
#include "AutoWindows.h"
#include "AutoWindow.h"
#include "AutoAppPrefs.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoApp

HRESULT CAutoApp::Init(CIcadApp *pApp)
    {
    m_pApp = pApp;
    return NOERROR;
    }

HRESULT CAutoApp::FinalConstruct()
    {
    m_pLibrary = NULL;
    return NOERROR;
    }

HRESULT CAutoApp::FinalRelease()
    {
    return S_OK;
    }


STDMETHODIMP CAutoApp::InterfaceSupportsErrorInfo(REFIID riid)
    {
    static const IID* arr[] =
        {
        &IID_IIcadApplication
        };
    for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
        {
        if (IsEqualGUID(*arr[i],riid))  //  MDTJULY1001 - for mssdk january 2001 version
            return S_OK;
        }
    return S_FALSE;
    }

/////////////////////////////////////////////////////////////////////
// CIcadAutoApp IClassFactory implementation

STDMETHODIMP CAutoApp::CreateInstance(IUnknown* pUnkOuter,REFIID riid, void** ppv)
    {
    HRESULT hr;

    *ppv=NULL;
    hr=ResultFromScode(E_OUTOFMEMORY);

    //Verify that a controlling unknown asks for IUnknown
    if(NULL!=pUnkOuter && IID_IUnknown!=riid)
        {
        return ResultFromScode(CLASS_E_NOAGGREGATION);
        }

    hr=QueryInterface (riid, ppv);
    if(!FAILED(hr))
        {
        //      else g_cObj++;
        }
    return hr;
    }

STDMETHODIMP CAutoApp::LockServer(BOOL fLock)
    {
    //  if(fLock) g_cLock++;
    //  else g_cLock--;
    return NOERROR;
    }

STDMETHODIMP CAutoApp::get_ActiveDocument (IIcadDocument * * ppDoc)
    {
    if (ppDoc == NULL)
        return E_POINTER;

    CIcadChildWnd* pChild = (CIcadChildWnd*)(m_pApp->m_pIcadMainWnd->MDIGetActive());
    if(pChild == NULL)
        return E_FAIL;

    CIcadDoc* pDoc = (CIcadDoc*) pChild->GetActiveDocument();
    if(pDoc == NULL)
        return E_FAIL;

    return pDoc->m_pAutoDoc->QueryInterface (ppDoc);
    }

STDMETHODIMP CAutoApp::get_ActiveWindow (IIcadWindow * * ppWindow)
    {
    if (ppWindow == NULL)
        return E_POINTER;

    CIcadChildWnd* pChild = (CIcadChildWnd*)(((CMDIFrameWnd*)m_pApp->m_pMainWnd)->MDIGetActive(NULL));
    if(pChild == NULL)
        return E_FAIL;

    CComObject<CAutoWindow> *pWindow;
    CComObject<CAutoWindow>::CreateInstance(&pWindow);
    if (!pWindow)
        return E_FAIL;

    pWindow->Init(NULL, pChild, NULL);

    return (pWindow->QueryInterface (ppWindow));
    }

STDMETHODIMP CAutoApp::get_Application (IIcadApplication * * ppApp)
    {
    if (ppApp == NULL)
        return E_POINTER;

    return m_pApp->m_pAutoApp->QueryInterface (ppApp);
    }

STDMETHODIMP CAutoApp::get_Caption (BSTR * pCaption)
    {
    if (pCaption == NULL)
        return E_POINTER;

    CString str;
    if (m_pApp->m_pMainWnd == NULL)
        return E_FAIL;

    m_pApp->m_pMainWnd->GetWindowText (str);
    *pCaption = str.AllocSysString();

    return NOERROR;
    }

STDMETHODIMP CAutoApp::get_DefaultFilePath (BSTR * pDefFilePath)
    {
    if (pDefFilePath == NULL)
        return E_POINTER;

    struct resbuf rb;
    rb.resval.rstring = NULL;
    CMainWindow *pMain = ((CMainWindow*)m_pApp->m_pIcadMainWnd);
    if (pMain == NULL)
        return E_FAIL;

    db_getvar (NULL, DB_QSRCHPATH, &rb, pMain->m_pCurDoc->m_dbDrawing, &pMain->m_CurSession, &pMain->m_CurConfig);
    CString str = rb.resval.rstring;
    *pDefFilePath = str.AllocSysString();

    return NOERROR;
    }

STDMETHODIMP CAutoApp::put_DefaultFilePath (BSTR pDefFilePath)
    {
    struct resbuf rb;

    CString str(pDefFilePath);
    if (str.IsEmpty ())
        return E_INVALIDARG;

    CMainWindow *pMain = ((CMainWindow*)m_pApp->m_pIcadMainWnd);
    if (pMain == NULL)
        return E_FAIL;

    rb.resval.rstring = (char *)str.GetBuffer(0);
    db_setvar (NULL, DB_QSRCHPATH, &rb, pMain->m_pCurDoc->m_dbDrawing, &pMain->m_CurSession, &pMain->m_CurConfig, 0);

    return NOERROR;
    }

STDMETHODIMP CAutoApp::get_Documents (IIcadDocuments * * ppDocs)
    {
    if (ppDocs == NULL)
        return E_POINTER;

    CComObject<CAutoDocuments>* pDocuments;
    CComObject<CAutoDocuments>::CreateInstance(&pDocuments);
    if (!pDocuments)
        return E_FAIL;

    POSITION posDocTemplate = m_pApp->GetFirstDocTemplatePosition();
    CIcadMultiDocTemplate* pDocTemplate;

    pDocTemplate = (CIcadMultiDocTemplate *) m_pApp->GetNextDocTemplate(posDocTemplate);
    if (NULL == pDocTemplate)
        return E_FAIL;

    pDocuments->Init (pDocTemplate);

    return pDocuments->QueryInterface (ppDocs);
    }

STDMETHODIMP CAutoApp::get_FullName (BSTR * pFullAppName)
    {
    if (pFullAppName == NULL)
        return E_POINTER;

    TCHAR szModule[_MAX_PATH];

    GetModuleFileName (m_pApp->m_hInstance, szModule, sizeof(szModule)/sizeof(TCHAR));
    CString strServerPath (szModule);
    *pFullAppName = strServerPath.AllocSysString();

    return NOERROR;
    }

STDMETHODIMP CAutoApp::get_Height (long * pHeight)
    {
    if (pHeight == NULL)
        return E_POINTER;

    CRect rect;

    m_pApp->m_pMainWnd->GetWindowRect (rect);
    *pHeight = rect.bottom - rect.top;

    return NOERROR;
    }

STDMETHODIMP CAutoApp::put_Height (long pHeight)
    {
    if (pHeight < 0)
        return E_INVALIDARG;

    CRect rect;

    m_pApp->m_pMainWnd->GetWindowRect (rect);
    m_pApp->m_pMainWnd->SetWindowPos (NULL, rect.left, rect.top, (rect.right - rect.left), pHeight, SWP_DRAWFRAME);

    return NOERROR;
    }

STDMETHODIMP CAutoApp::get_LocaleID (long * pLocaleId)
    {
    if (pLocaleId == NULL)
        return E_POINTER;

    LANGID sysLangID = LANG_NEUTRAL;
    sysLangID = GetSystemDefaultLangID();
    *pLocaleId = PRIMARYLANGID( sysLangID);

    return NOERROR;
    }

STDMETHODIMP CAutoApp::get_Left (long * pLeft)
    {
    if (pLeft == NULL)
        return E_POINTER;

    CRect rect;

    m_pApp->m_pMainWnd->GetWindowRect (rect);
    *pLeft = rect.left;

    return NOERROR;
    }

STDMETHODIMP CAutoApp::put_Left (long pLeft)
    {
    if (pLeft < 0)
        return E_INVALIDARG;

    CRect rect;

    m_pApp->m_pMainWnd->GetWindowRect (rect);
    m_pApp->m_pMainWnd->SetWindowPos (NULL, pLeft, rect.top, (rect.right - rect.left), (rect.bottom - rect.top), SWP_DRAWFRAME);

    return NOERROR;
    }

STDMETHODIMP CAutoApp::get_Library (IIcadLibrary * * ppLibrary)
    {
    if (ppLibrary == NULL)
        return E_POINTER;

    if (!m_pLibrary)
        {
        CComObject<CAutoLibrary>* pLibrary;
        CComObject<CAutoLibrary>::CreateInstance(&pLibrary);
        if (!pLibrary)
            return E_FAIL;

        pLibrary->QueryInterface (&m_pLibrary);
        }

    if (!m_pLibrary)
        return E_FAIL;
    return m_pLibrary->QueryInterface (ppLibrary);
    }

STDMETHODIMP CAutoApp::get_Name (BSTR * pstrAppName)
    {
    if (pstrAppName == NULL)
        return E_POINTER;

    /* Another way of doing it
    CString strResult (m_pApp->m_pszAppName);
    *pFullAppName = strResult.AllocSysString();
    */

    CComBSTR Appname;
    Appname.LoadString(IDS_INTELLICAD);
    *pstrAppName = Appname;
    return NOERROR;
    }

STDMETHODIMP CAutoApp::get_Parent (IIcadApplication * * ppParent)
    {
    if (ppParent == NULL)
        return E_POINTER;

    return get_Application(ppParent);
    }

STDMETHODIMP CAutoApp::get_Path (BSTR * pstrPath)
    {
    if (pstrPath == NULL)
        return E_POINTER;

    //*** Get the path of the application.
    TCHAR szModule[_MAX_PATH];

    GetModuleFileName (m_pApp->m_hInstance, szModule, sizeof(szModule)/sizeof(TCHAR));
    CString strServerPath (szModule);
    int idxChar = strServerPath.ReverseFind('\\');
    if(idxChar!=(-1))
        {
        strServerPath=strServerPath.Left(idxChar);
        }
    else
        {
        strServerPath.Empty();
        }

    *pstrPath=strServerPath.AllocSysString();

    return NOERROR;
    }

STDMETHODIMP CAutoApp::get_PlotManager (IIcadPlotManager * * ppPlotManager)
    {
    if (ppPlotManager == NULL)
        return E_POINTER;

    if (m_pApp->m_pIcadMainWnd == NULL)
        return E_FAIL;

    m_pApp->InitPrintDialog();

    IIcadPlotManager * pPlotManager;
    pPlotManager = ((CMainWindow*)m_pApp->m_pIcadMainWnd)->m_pIcadPrintDlg->GetPlotManager();
    pPlotManager->AddRef();

    *ppPlotManager = pPlotManager;

    return NOERROR;

    }

STDMETHODIMP CAutoApp::get_Preferences (IIcadApplicationPreferences * * ppPreferences)
    {
    if (ppPreferences == NULL)
        return E_POINTER;

    CComObject<CAutoAppPreferences> *pPrefs;
    CComObject<CAutoAppPreferences>::CreateInstance(&pPrefs);
    if (!pPrefs)
        return E_FAIL;

    return pPrefs->QueryInterface (ppPreferences);
    }

STDMETHODIMP CAutoApp::get_StatusBar (BSTR * pstrStatusText)
    {
    if (pstrStatusText == NULL)
        return E_POINTER;

    CWnd* bar = ((CFrameWnd*)m_pApp->m_pMainWnd)->GetMessageBar ();
    CString str;
    bar->GetWindowText (str);

    *pstrStatusText = str.AllocSysString();

    return NOERROR;
    }

STDMETHODIMP CAutoApp::put_StatusBar (BSTR pstrStatusText)
    {
    CWnd* bar = ((CFrameWnd*)m_pApp->m_pMainWnd)->GetMessageBar ();
    bar->SetWindowText ((CString)pstrStatusText);

    return NOERROR;
    }

STDMETHODIMP CAutoApp::get_Top (long * pTop)
    {
    if (pTop == NULL)
        return E_POINTER;

    CRect rect;
    m_pApp->m_pMainWnd->GetWindowRect (rect);

    *pTop = rect.top;

    return NOERROR;
    }

STDMETHODIMP CAutoApp::put_Top (long pTop)
    {
    if (pTop < 0)
        return E_INVALIDARG;

    CRect rect;
    m_pApp->m_pMainWnd->GetWindowRect (rect);
    m_pApp->m_pMainWnd->SetWindowPos (NULL, rect.left, pTop, (rect.right - rect.left), (rect.bottom - rect.top), SWP_DRAWFRAME);

    return NOERROR;
    }

STDMETHODIMP CAutoApp::get_VBE (IDispatch * * ppVBE)
    {
    if (ppVBE == NULL)
        return E_POINTER;

#ifdef BUILD_WITH_VBA
    if(!m_pApp->IsVBAAccessible())
        return E_FAIL;

    CComPtr<VBIDE::VBE> spVBE;
    HRESULT hr = m_pApp->ApcHost->APC_GET(VBE)(&spVBE.p);
    if (SUCCEEDED(hr))
        return spVBE->QueryInterface(IID_IDispatch, (void**) ppVBE);
    else
#endif
        return E_FAIL;
    }

STDMETHODIMP CAutoApp::get_Version (BSTR * pstrVersion)
    {
    if (pstrVersion == NULL)
        return E_POINTER;

    struct resbuf rb;
    rb.resval.rstring = NULL;
    //db_getvar (NULL, DB_QICADVER, &rb, ((CMainWindow*)m_pApp->m_pIcadMainWnd)->m_pCurDoc->m_dbDrawing, NULL, NULL);
    db_getvar (NULL, DB_Q_VERNUM, &rb, ((CMainWindow*)m_pApp->m_pIcadMainWnd)->m_pCurDoc->m_dbDrawing,
        &((CMainWindow*)m_pApp->m_pIcadMainWnd)->m_CurSession,
        &((CMainWindow*)m_pApp->m_pIcadMainWnd)->m_CurConfig);
    CString str = rb.resval.rstring;
    *pstrVersion = str.AllocSysString();

    return NOERROR;
    }

STDMETHODIMP CAutoApp::get_Visible (VARIANT_BOOL * pVisible)
    {
    if (pVisible == NULL)
        return E_POINTER;

    (*pVisible) = m_bVisible;

    return NOERROR;
    }

STDMETHODIMP CAutoApp::put_Visible (VARIANT_BOOL pVisible)
    {
    if (VARIANT_TRUE == pVisible)
        m_pApp->ShowAppWindows();
    else
        m_pApp->HideApplication();

    m_bVisible=pVisible;

    return NOERROR;
    }

STDMETHODIMP CAutoApp::get_Width (long * pWidth)
    {
    if (pWidth == NULL)
        return E_POINTER;

    CRect rect;
    m_pApp->m_pMainWnd->GetWindowRect (rect);

    *pWidth = rect.right - rect.left;

    return NOERROR;
    }

STDMETHODIMP CAutoApp::put_Width (long pWidth)
    {
    if (pWidth < 0)
        return E_INVALIDARG;

    CRect rect;
    m_pApp->m_pMainWnd->GetWindowRect (rect);
    m_pApp->m_pMainWnd->SetWindowPos (NULL, rect.left, rect.top, pWidth, (rect.bottom - rect.top), SWP_DRAWFRAME);

    return NOERROR;
    }

STDMETHODIMP CAutoApp::get_Windows (IIcadWindows * * ppWindows)
    {
    if (ppWindows == NULL)
        return E_POINTER;

    CComObject<CAutoWindows>* pWindows;
    CComObject<CAutoWindows>::CreateInstance(&pWindows);
    if (!pWindows)
        return E_FAIL;

    pWindows->Init (NULL);

    return pWindows->QueryInterface (ppWindows);
    }

STDMETHODIMP CAutoApp::get_Workspace (IIcadWorkspace * * ppWorkspace)
    {
    if (ppWorkspace == NULL)
        return E_POINTER;

    if (m_pApp->m_spWorkspace)
        return m_pApp->m_spWorkspace->QueryInterface (ppWorkspace);
    else
        return E_FAIL;
    }

STDMETHODIMP CAutoApp::DefineFunction (BSTR FunctionName, VARIANT FunctionPointer)
    {
    static int id = 0;
    CString fname (FunctionName);
    CComVariant vFPtr (FunctionPointer);
    long (*fnptr)(void);
    fnptr = (long (__cdecl *)(void))V_I4 (&vFPtr);

    id++;
    m_pApp->RegisterFunction (fname, fnptr, fname, id);

    return NOERROR;
    }

STDMETHODIMP CAutoApp::UndefineFunction (BSTR FunctionName)
    {
    CString fname (FunctionName);
    m_pApp->UnRegisterFunction (fname, fname, 0);

    return NOERROR;
    }

STDMETHODIMP CAutoApp::Help (BSTR HelpFile, BSTR HelpContext, long HelpCommand)
    {
    CString helpfile (HelpFile);
    CString helpcontext (HelpContext);

    if (HelpCommand == 0)
        HelpCommand = HELP_CONTENTS;

    if ((HelpCommand != HELP_CONTENTS) && (HelpCommand != HELP_HELPONHELP) && (HelpCommand != HELP_PARTIALKEY))
        return E_INVALIDARG;

    return m_pApp->RunHelp (helpfile, helpcontext, HelpCommand);
    }

STDMETHODIMP CAutoApp::Quit ()
    {
    SendMessage( m_pApp->m_pIcadMainWnd->GetSafeHwnd(), WM_COMMAND, ICAD_WNDACTION_EXIT, 0 );

    return NOERROR;
    }

STDMETHODIMP CAutoApp::ListSDS (VARIANT * ListOfSDSApps)
    {
    if (ListOfSDSApps == NULL)
        return E_POINTER;

    struct resbuf *rb, *pApps;
    m_pApp->GetLoadedApps (&pApps);

    long nCount = 0;
    for (rb = pApps; rb != NULL; rb = rb->rbnext)
        nCount++;

    VARIANT var;
    SAFEARRAY FAR* Apps;
    SAFEARRAYBOUND Bound[1];
    Bound[0].lLbound = 0;
    Bound[0].cElements = nCount;
    Apps = SafeArrayCreate(VT_VARIANT, 1, Bound);

    for (rb = pApps, nCount = 0; rb != NULL; rb = rb->rbnext, nCount++)
        {
        CString str(rb->resval.rstring);
        var.vt = VT_BSTR;
        var.bstrVal = str.AllocSysString();
        SafeArrayPutElement(Apps, &nCount, &var);
        }

    (*ListOfSDSApps).vt = VT_ARRAY | VT_VARIANT;
    (*ListOfSDSApps).parray = Apps;

    if ( pApps != NULL )
        sds_relrb( pApps );

    return NOERROR;
    }

STDMETHODIMP CAutoApp::LoadSDS (BSTR ApplicationName)
    {
    CString str(ApplicationName);
    if (str.IsEmpty())
        return E_INVALIDARG;

    return m_pApp->LoadOrUnloadApp (str, TRUE, TRUE);
    }

STDMETHODIMP CAutoApp::UnloadSDS (BSTR ApplicationName)
    {
    CString str(ApplicationName);
    if (str.IsEmpty())
        return E_INVALIDARG;

    return m_pApp->LoadOrUnloadApp (str, TRUE, FALSE);
    }

STDMETHODIMP CAutoApp::LoadLISP (BSTR ApplicationName)
    {
    CString str(ApplicationName);
    if (str.IsEmpty())
        return E_INVALIDARG;

    return m_pApp->LoadOrUnloadApp (str, FALSE, TRUE);
    }

STDMETHODIMP CAutoApp::RunCommand (BSTR Command)
    {
    CString str(Command);
    if (str.IsEmpty())
        return E_INVALIDARG;

    return m_pApp->RunCommand (str);
    }

STDMETHODIMP CAutoApp::RunScript (BSTR ScriptName)
    {
    CString str(ScriptName);
    if (str.IsEmpty())
        return E_INVALIDARG;

    str = "script " + str;

    return m_pApp->RunCommand (str);
    }

STDMETHODIMP CAutoApp::GetInterfaceObject (BSTR ProgID, IDispatch * * ppObject)
    {
    if (ppObject == NULL)
        return E_POINTER;

    CString progid (ProgID);
    CLSID clsid;

    if (SUCCEEDED( CLSIDFromProgID( (LPCOLESTR)progid.GetBuffer(0), &clsid ) ) )
        return CoCreateInstance( clsid, NULL, CLSCTX_SERVER, IID_IDispatch, (void **)ppObject );

    return E_FAIL;
    }

STDMETHODIMP CAutoApp::IsFunctionLoaded (BSTR FunctionName, VARIANT_BOOL *Loaded)
    {
    if (Loaded == NULL)
        return E_POINTER;

    CString fname(FunctionName);
    if (fname.IsEmpty())
        return E_INVALIDARG;

    if (m_pApp->IsFunctionRegistered (fname))
        *Loaded = VARIANT_TRUE;
    else
        *Loaded = VARIANT_FALSE;

    return NOERROR;
    }


