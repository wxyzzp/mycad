/* PRJ\ICAD\IcadApp.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 *	NOTE:  Optimization for this file has been turned of for Win32 Release build because it causes the build
 *	to fail.  CIcadWorkspace::GetConnMap is reported as unresolved.  This appears to be an optimizer problem.
 *	Using pragma's to turn optimization off does not resolve the problem.
 */

//** Includes
#include "Icad.h"/*DNT*/
#include "IcadView.h"/*DNT*/

#include "IcadATL.h"/*DNT*/
#define CLSID_DEFINED
namespace ICAD_AUTOMATION
{
#include "IcadTLB.h"/*DNT*/
#include "icad_i.c"/*DNT*/
}
using namespace ICAD_AUTOMATION;
#include "IcadAPC.h"/*DNT*/

#include "IcadApp.h"/*DNT*/
#include "IcadDoc.h"/*DNT*/
#include "IcadChildWnd.h"/*DNT*/
#include "IcadCursor.h"/*DNT*/
#include "IcadOleRes.h"/*DNT*/
#include "IcadIpFrame.h"/*DNT*/
#include "IcadTipDlg.h"/*DNT*/
#include "IcadSplash.h"/*DNT*/
#include "IcadTextScrn.h"/*DNT*/
#include "IcadMenuMain.h"/*DNT*/
#include "IcadMenu.h"/*DNT*/
#include "IcadDocTemplate.h"/*DNT*/
#include "IcadPrivateGuids.h"/*DNT*/
#include "Cmds.h"/*DNT*/
#include "hiddenwindow.h"/*DNT*/
#include "Preferences.h"/*DNT*/
#include "IcadCustom.h"/*DNT*/

#include <shlobj.h>
#include "blddate.h"/*DNT*/
#include "CheckFP.h"/*DNT*/

#include "win32misc.h"/*DNT*/
#include "PrintDialog.h"/*DNT*/
#include "Authenticator.h"/*DNT*/
#include "AutoDLL.h"/*DNT*/
#include "BHatchFactory.h"/*DNT*/
// EBATECH(watanabe)-[update leader according to modifying annotation
#include "UpdateLeader.h"/*DNT*/
// ]-EBATECH(watanabe)
#include "Paths.h"/*DNT*/

#include "AutoPoint.h"/*DNT*/
#include "AutoPoints.h"/*DNT*/
#include "AutoVector.h"/*DNT*/
#include "AutoMatrix.h"/*DNT*/

#include "Modeler.h" /*DNT*/

#include "LicensedComponents.h"/*DNT*/

#ifdef USE_ICAD_SPOOLING
#include "DlgSpoolingSettings.h"
#endif

#include "plotproperties.h"
#include "winspool.h"

#include "Configure.h"

CIcadApp icadApp;

CComModule _Module;

extern PreferenceKey configKey; 			// Add by SystemMetrix(Maeda) 09.10.2001
extern PreferenceKey plotConfigKey;

BEGIN_OBJECT_MAP(ObjectMap)
//	OBJECT_ENTRY(CLSID_Application, CAutoApp)
//	OBJECT_ENTRY(CLSID_Document, CAutoDoc)
	OBJECT_ENTRY(CLSID_Point, CAutoPoint)
	OBJECT_ENTRY(CLSID_Points, CAutoPoints)
	OBJECT_ENTRY(CLSID_Vector, CAutoVector)
	OBJECT_ENTRY(CLSID_Matrix, CAutoMatrix)
END_OBJECT_MAP()

								// define authentication IIDs
#ifdef _ATL_STATIC_REGISTRY
#include <statreg.h>
#endif
#include <atlimpl.cpp>
#include <atlctl.cpp>
#ifdef BUILD_WITH_VBA
#include <apcGuids.h>
#include <apcImpl.cpp>
#else
HRESULT ApcFireEvent(IUnknown* punk, REFIID iid, DISPID dispid, DISPPARAMS* pDispParams /*= NULL*/, VARIANT* pVarResult /*= NULL*/);
#endif

#include <initguid.h>

#include "IcadVbaWorkspace.h"
#include "IcadDispID.h"

#include "COMAddin.h"

#include <delayimp.h>

#ifdef	BUILD_WITH_VBA
static const bool g_BuiltWithVba = TRUE;
#else
static const bool g_BuiltWithVba = FALSE;
#endif

#ifndef __ItcLanguageH__
#include "ItcLanguage.h"
#endif

#ifndef _COMAddinManager_H
#include "COMAddinManager.h"
#endif

bool g_Vba6Installed;

// This identifier was generated to be statistically unique for your app.
// You may change it if you prefer to choose a specific identifier.

/////////////////////////////////////////////////////////////////////
// Message handle-ing (huh-huh) and Member (huh-huh) functions for
// the CIcadApp class

BEGIN_MESSAGE_MAP(CIcadApp, CWinApp)
	//{{AFX_MSG_MAP(CIcadApp)
	//}}AFX_MSG_MAP
	ON_COMMAND(CG_IDS_TIPOFTHEDAY, ShowTipOfTheDay)
END_MESSAGE_MAP()

//Added Cybage AW 17-12-01 [
//Reason : DIASTAT Variable functionality added
//Bug no. 54233 from BugZilla
HHOOK hHook = NULL;
LONG CALLBACK  HookForDIASTATvar(int code, WPARAM wParam, LPARAM lParam)
{
	if (code < 0)  // do not process message
		return CallNextHookEx(hHook, code,wParam, lParam);

	if(MSGF_DIALOGBOX == code)
	{
		CWPSTRUCT *lpStruct = (CWPSTRUCT *) lParam;
		HWND hwndButton = (HWND)lpStruct->lParam;
		struct resbuf rb;
		rb.restype = RTSHORT;

		if(lpStruct->message == WM_COMMAND &&
			HIWORD(lpStruct->wParam) == BN_CLICKED &&
			hwndButton)
		{
			char *pTemp=NULL, chCaption[10];
			GetWindowText(hwndButton, chCaption, 8);
			pTemp = _strupr(chCaption);

			// Bugzilla No. 78215; 01/07/2002 [
			if(LOWORD(lpStruct->wParam) == IDCANCEL /*|| strstr(pTemp, "CANCEL"/*DNT* /) != NULL*/)
			{
				rb.resval.rint = 0;
				SDS_setvar(NULL,DB_QDIASTAT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
			}
			else if(LOWORD(lpStruct->wParam) == IDOK /*|| strstr(pTemp, "OK"/*DNT* /) != NULL*/)
			{
				rb.resval.rint = 1;
				SDS_setvar(NULL,DB_QDIASTAT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
			}
			// Bugzilla No. 78215; 01/07/2002 ]
		}
		else if(lpStruct->wParam == SC_CLOSE && lpStruct->message == WM_SYSCOMMAND)
		{
			rb.resval.rint = 0;
			SDS_setvar(NULL,DB_QDIASTAT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
		}
	}
	return CallNextHookEx(hHook, code, wParam, lParam);
}
//Added Cybage AW 17-12-01 ]

extern IcadCursor	cursorSingleton;
									// initialize cursor singleton
CIcadApp:: CIcadApp() : m_cursor( &cursorSingleton)
	{
	CComObject<CAutoApp>::CreateInstance(&m_pAutoApp);
	m_pAutoApp->Init (this);
	m_pAutoApp->AddRef();

	//m_comAddin = new COMAddin();

	bSavePrinterSettingsAfterFirstCall = false;

	// Initialise the Lineweight Display Scale Factor to 8 as default.
	m_nLWDisplayScaleFactor = 8;

	}

CIcadApp::~CIcadApp()
	{

	//m_pAutoApp->Release();
	delete m_pAutoApp; //pointer to smart pointer
#ifdef BUILD_WITH_ACIS_SUPPORT

	// Unload ACIS DLL it has not been unloaded. This will fix 
	// memory leaks in the case when the reference count in 
	// CDbAcisEntity never go down to zero(i.e. in the case 
	// users create solids but don't want to save them) when 
	// ICAD is shutted down.   Stephen Hou 9-2002
	//
	//
	CModeler::stop();
#endif // BUILD_WITH_ACIS_SUPPORT
	}


// Support for CommonProjects
// In MFC, the only way (and arguably, the 'correct' way) to close
// the Common project during shut down is to supply a Doc Template
// for it, and respond to the CloseAllDocuments virtual function call.
//
// If this is not done, an extra OLE Lock will exist, and the app
// will disappear, but not shut down
//
class CActLibraryDocTemplate : public CSingleDocTemplate
{
public:
	CActLibraryDocTemplate() : CSingleDocTemplate(1, NULL, NULL, NULL)
	{
		// 1 passed above to avoid ASSERT, change to 0 here
		m_nIDResource = 0;
	}
	virtual void CloseAllDocuments(BOOL)
	{
		CIcadApp *pApp = GetApp();
		if( !pApp )
			return;

		if( pApp->m_spWorkspace )
		{
			// Send the VBA workspace event
			if (g_Vba6Installed)
			{
			// fire the IWorkspace::Exit(VARIANT_BOOL) event and determine if the user wants to Exit
				HRESULT hr = ApcFireEvent(GetApp()->m_spWorkspace, DIID__IcadWorkspaceEvents, DISPID_WS_EXIT, NULL, NULL);
				ASSERT(SUCCEEDED(hr));
			}
			else
			{
				HRESULT hr = GetApp()->comAddinManager ().fireWorkspaceExit(GetApp()->m_spWorkspace);
				ASSERT(SUCCEEDED(hr));
			}
			pApp->CloseWorkspace(TRUE);
		}

	}
};


void CIcadApp::CloseWorkspace(BOOL bSave)
{
#ifdef BUILD_WITH_VBA
	if(CIcadApp::m_LicensedComponents.IsAccessible(CLicensedComponents::Components::VBA))
	{
		HRESULT hr;

		if (1) //scope
		{
			CComQIPtr<IActProjectStorage, &__uuidof(IActProjectStorage)> spProjectStorage(m_spWorkspace);

			//
			// Save and Close
			//
			hr = spProjectStorage->Save();
			ASSERT(SUCCEEDED(hr));			//If you have another instance of Intellicad running, you can ignore this assert.
			//This is asserting because it cannot save the VBA CommonProjects.
			hr = spProjectStorage->Close();
			ASSERT(SUCCEEDED(hr));
		}
	}

	m_spWorkspace.Release();
	m_spWorkspace = 0;
#endif // BUILD_WITH_VBA
}

void CIcadApp::FireWorkspaceEvent(int id, CIcadDoc *pDoc)
	{
#ifdef BUILD_WITH_VBA
	if(CIcadApp::m_LicensedComponents.IsAccessible(CLicensedComponents::Components::VBA))
	{
		switch (id)
			{
			case DISPID_WS_STARTUP:
			case DISPID_WS_BEFOREEXIT:
			case DISPID_WS_EXIT:
			case DISPID_WS_ADDINRUN:
				ASSERT (0); //you should directly fire these events
				break;
			case DISPID_WS_REINIT:
				ApcFireEvent(m_spWorkspace, DIID__IcadWorkspaceEvents, id, NULL, NULL);
				break;
			case DISPID_WS_NEWDOC:
			case DISPID_WS_OPENDOC:
			case DISPID_WS_SAVEDOC:
			case DISPID_WS_CLOSEDOC:
				{
				HRESULT hr;
				// get IIcadDoc* parameter as variant
				CComPtr<IIcadDocument> spDoc;
				hr = pDoc->m_pAutoDoc->QueryInterface (&spDoc);
				ASSERT(SUCCEEDED(hr));

				if (spDoc.p)
					{
					CComVariant doc(spDoc.p);
					VARIANT varArg[1];
					varArg[0] = doc;

					DISPPARAMS parm;
					parm.rgvarg = varArg;
					parm.cArgs = 1;
					parm.rgdispidNamedArgs = NULL;
					parm.cNamedArgs = 0;

					ApcFireEvent(m_spWorkspace, DIID__IcadWorkspaceEvents, id, &parm, NULL);
					}
				}
				break;
			default:
				ASSERT(0);
			}
	}
	else
#endif //BUILD_WITH_VBA
	{
		switch (id)
		{
		case DISPID_WS_STARTUP:
		case DISPID_WS_BEFOREEXIT:
		case DISPID_WS_EXIT:
		case DISPID_WS_ADDINRUN:
			ASSERT (0);	//you should directly fire these events
			break;
		case DISPID_WS_REINIT:
			comAddinManager ().fireWorkspaceReinit(m_spWorkspace);

			break;
		case DISPID_WS_NEWDOC:
		case DISPID_WS_OPENDOC:
		case DISPID_WS_SAVEDOC:
		case DISPID_WS_CLOSEDOC:
		{
			HRESULT hr;
			// get IIcadDoc* parameter as variant
			CComPtr<IIcadDocument> spDoc;
			hr = pDoc->m_pAutoDoc->QueryInterface (&spDoc);
			ASSERT(SUCCEEDED(hr));

			if (spDoc.p)
			{
				comAddinManager ().fireWorkspaceDocumentEvent(m_spWorkspace, id, spDoc.p);
			}
		}
		break;
		default:
			ASSERT(0);
		}
	}
	}


// TODO: OpenProject notification
HRESULT CIcadApp::ApcHost_OpenProject(BSTR bstrProjectFileName, IApcProject* pReferencingProject, IApcProject** ppProject)
	{
	return NOERROR; //not implemented yet
	}

HRESULT CIcadApp::ApcHost_OnCreate()
	{
	HRESULT hr = S_OK;
	CString temp;
	// get registry key name (check /p option)
    	getRegCurrentProfileRoot(temp.GetBuffer(IC_ACADBUF),IC_ACADBUF);
	temp.ReleaseBuffer();	
	m_regpathBase = temp;
	m_regpathSecurity.LoadString (IDC_VBASECURITY_KEY); //Security
//	m_regpathAddins.LoadString (IDC_VBA_ADDINS_KEY); //Addins <-this isn't used (EBATECH)

	// Choose Windows help, instead of HTML help and notify APC
	ApcHost->APC_PUT(HelpStyle)(axHelpHTML); // or axHelpWindows (see MSAPC::AxHelpStyle)

#ifdef BUILD_WITH_VBA // EBATECH- PutLocale() is undefine if we don't set BUILD_WITH_VBA.
	ApcHost->PutLocale(USED_VBA_LOCALE);
#endif

	hr = InitSecurity();
	ASSERT(SUCCEEDED(hr));	// could indicate registry failure

	// This enables a menu item in the VBA IDE, and checks
	// for trusted signature when opening documents, which may
	// result in nag dialogs from VBA, asking if you want to
	// disable macros because of the possibility of VBA viruses.
	//
	hr = ApcHost->APC_PUT(EnableDigitalSignatures)(VARIANT_TRUE);
	ASSERT(SUCCEEDED(hr));

	hr = CApcApplication<CIcadApp>::ApcHost_OnCreate();
	return hr;
	}


HRESULT CIcadApp::InitSecurity()
	{
	// Try to get the current value from the registry.
	// If key does not exist (FAILED(hr)), default to 'Medium'
	AxSecurityLevel axLevel = axSecurityMedium;
	AxSecurityLevel axWSpace = axSecurityNone;
	CRegKey reg;

	long nSuccess = reg.Create(HKEY_CURRENT_USER, m_regpathBase);
	if (ERROR_SUCCESS == nSuccess)
		nSuccess = reg.QueryValue((unsigned long&) axLevel, m_regpathSecurity);
	if (ERROR_SUCCESS != nSuccess)
		{
		TRACE0("Warning... no security value stored in registry\n");
		nSuccess = ERROR_SUCCESS;
		//
		// This error is harmless until the user explicitly specifies
		// a value using the dialog box from Tools | Security...
		// At that point, the elimination of the security value
		// from the registry should be considered malicious.
		//

		LOADSTRINGS_3(IDC_VBA_WARN_NOSECURITY)
		//Bugzilla::78502; 09-06-03
		if(m_cmdInfo.m_nShellCommand != CCommandLineInfo::FileDDE)
		MessageBox(m_pIcadMainWnd->GetSafeHwnd(), LOADEDSTRING, ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD" ),MB_OK | MB_ICONEXCLAMATION);
		}
	else
		Decrypt(axLevel);

	// set ApcHost.SecurityLevel
	HRESULT hr;
	hr = ApcHost->APC_PUT(SecurityLevel)(axLevel);
	if (FAILED(hr))
		return hr;

	//Determine the Workspace security - i.e. should we disable common projects macros?
	if (ERROR_SUCCESS == nSuccess)
		nSuccess = reg.QueryValue((unsigned long&) axWSpace, "WorkspaceSecurity");
	if (ERROR_SUCCESS != nSuccess)
		{
		TRACE0("Warning... no workspace security value stored in registry\n");
		nSuccess = ERROR_SUCCESS;

		m_bDisableWorkspace = FALSE;
		}
	else
		{
		Decrypt(axWSpace);
		if (axWSpace == axSecurityNone)
			m_bDisableWorkspace = FALSE;
		else
			m_bDisableWorkspace = TRUE;
		}

	if (ERROR_SUCCESS != nSuccess)
		return HRESULT_FROM_WIN32(nSuccess);

	return S_OK;
	}

void CIcadApp::GetWorkspaceSecurity (BOOL *bSec)
	{
	*bSec = m_bDisableWorkspace;
	}
void CIcadApp::SetWorkspaceSecurity (BOOL bSec)
	{
	AxSecurityLevel axWSpace = axSecurityNone;

	m_bDisableWorkspace = bSec;
	if (m_bDisableWorkspace)
		axWSpace = axSecurityHigh;
	else
		axWSpace = axSecurityNone;

	Encrypt(axWSpace);

	// write the value to the registry
	CRegKey reg;
	long nSuccess = reg.Open(HKEY_CURRENT_USER, m_regpathBase);
	if (ERROR_SUCCESS != reg.SetValue(static_cast<unsigned long>(axWSpace), "WorkspaceSecurity"))
		{
		GetApp()->m_pMainWnd->MessageBox(
			ResourceString(IDC_VBA_SECURITY_NOTSAVED, "IntelliCAD is unable to store the requested security value in registry\n"),
			ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD" ),
			MB_OK | MB_ICONEXCLAMATION);
		}

	}


//
// Since the user's chosen security level is being stored in the
// registry, it needs to be encrypted.	A malicious macro virus
// could easily remove or change this value, so a secure scheme
// should be implemented for encryption of this registry setting.
//
// The scheme used should have the following features
// - a difficult algorithm, perhaps using public/private keys.
// - the creation of a value that can not be copied from one machine to another.
//
void CIcadApp::Encrypt(AxSecurityLevel& nValue)
	{
	// do nothing.
	}

void CIcadApp::Decrypt(AxSecurityLevel& nValue)
	{
	// do nothing.
	}





//	InitSysVars
//
//	Perform runtime initialization of system variables

void InitSysVars()
{
	static CString	vernum;
	static CString	icadver;
	// Added Cybage VSB 13/08/2001 [
	// Reason: Fix for Bug No: 77822 from BugZilla
	static CString	vendorname;
	// Added Cybage VSB 13/08/2001 DD/MM/YY ]

	// vernum is a nightly build value from blddate.h
	// icadver is the release version string
#ifdef	DEBUG
	vernum.Format( "%s d"/*DNT*/, MLBLDVERS);
#else
	vernum.Format( "%s"/*DNT*/, MLBLDVERS);
#endif
	icadver.Format( "%s", BLDVERS );/*AVH*/
	// Added Cybage VSB 13/08/2001 [
	// Reason: Fix for Bug No: 77822 from BugZilla
	vendorname.Format("%s",VENDORNAME);
	// Added Cybage VSB 13/08/2001 DD/MM/YY ]

	db_findsysvar( NULL, DB_Q_VERNUM, NULL)->defval = const_cast<char *>((LPCTSTR)vernum);
	db_findsysvar( NULL, DB_QICADVER, NULL)->defval = const_cast<char *>((LPCTSTR)icadver);
	// Added Cybage VSB 13/08/2001 [
	// Reason: Fix for Bug No: 77822 from BugZilla
	db_findsysvar( NULL, DB_Q_VENDORNAME, NULL)->defval = const_cast<char *>((LPCTSTR)vendorname);
	// Added Cybage VSB 13/08/2001 DD/MM/YY ]
	static CString	ctab;
	ctab.Format("%s", ResourceString(IDC_ICADAPP_CTAB_DEFAULT, "Model"/*DNT*/));
	db_findsysvar( NULL, DB_QCTAB, NULL)->defval = ctab.GetBuffer(0);
	static CString	cprofile;
	static CString	exedir;

	GetCurrentRegistryProfileName(cprofile.GetBuffer(IC_ACADBUF), IC_ACADBUF);
	db_findsysvar( NULL, DB_QCPROFILE, NULL)->defval = cprofile.GetBuffer(0);
	exedir = FileString::ModulePath();
	db_findsysvar( NULL, DB_QEXEDIR, NULL)->defval = exedir.GetBuffer(0);
}

	// initialization progress
class AppProgressMeter : public ProgressMeter
	{
	public:
		void Start()					{ SDS_ProgressStart();}
		void Stop() 					{ SDS_ProgressStop();}
		void Percent( int percentDone)	{ SDS_ProgressPercent( percentDone);}
	};

// Modified Cybage VSB 23/03/2001 [
// Reason: Fix for the ICAD.LSP bug reported Ronald Prepchuk
// email dated 21/03/2001
CEvent g_WaitEvent( FALSE, TRUE);
// Modified Cybage VSB 23/03/2001

BOOL CIcadApp::InitInstance()
{
	// Parse command line for standard shell commands, DDE, file open
	// SystemMetrix(Maeda) 09.10.2001 -[ change for function call timing
	ParseCommandLine(m_cmdInfo);
	CString strCurProfileName;
	if (m_cmdInfo.m_strOverrideProfileName.IsEmpty())
	  {
		CString curProfName;
		GetCurrentRegistryProfileName(curProfName.GetBuffer(IC_ACADBUF), IC_ACADBUF);
		SetCurrentRegistryProfileName(curProfName); // if /Pxxx option not setting, set "Default" name.
	  }
	else
	  {
		SetCurrentRegistryProfileName(m_cmdInfo.m_strOverrideProfileName);
	  }
    	GetCurrentRegistryProfileName(strCurProfileName.GetBuffer(IC_ACADBUF),IC_ACADBUF);
	strCurProfileName.ReleaseBuffer();
	
	configKey.Reload(HKEY_CURRENT_USER, "Config");
	plotConfigKey.Reload(HKEY_CURRENT_USER, "Config\\plot");
	UserPreference::readFromRegistry();
	//cursor is initialized in the CIcadApp constructor, 
	//before InitInstance, so it needs to be reinitialized
	cursorSingleton.readFromRegistry();
	// ]- SystemMetrix(Maeda) 09.10.2001
	//	save copies of instance handles accessible to DLLs
	IcadSharedGlobals::SetIcadAppInstance( AfxGetInstanceHandle() );
	IcadSharedGlobals::SetIcadResourceInstance( AfxGetInstanceHandle() );	// no resource DLL yet

	m_hatchFactory = new BHatchBuilderFactory();
	IcadSharedGlobals::SetHatchFactory(m_hatchFactory);
	// EBATECH(watanabe)-[update leader according to modifying annotation
	m_leaderFactory = new CUpdateLeaderFactory();
	IcadSharedGlobals::SetLeaderFactory(m_leaderFactory);
	// ]-EBATECH(watanabe)

	IcadSharedGlobals::SetCmdMessage(&sds_printf);

	db_SetSerialNumber( SerialNumber );


	if (CheckLicense() == false)
		return FALSE;

	LoadLWDisplayScaleFactorFromRegistry();



	// hook for tracking memory leaks
	long	alloc = -1;
	_CrtSetBreakAlloc( alloc);

	// Initialize OLE libraries
	if (!ApcOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	//Modified Cybage VSB 18/12/2001 DD/MM/YYYY [
	//Reason : Bug 77919 from Bugzilla
	//Description :ole32.dll dialog box dialog box in Win98, the commented code
	//is shifted below, just above _Module.RegisterClassObjects()
	//if (!InitATL())
	//	  return FALSE;
	//Modified Cybage VSB 18/12/2001 DD/MM/YYYY ]

	// runtime variable initialization
	InitSysVars();

	ic_setlocale();

	AfxEnableControlContainer();


	// "Recent File List" and "Settings" keys are common for all profiles
	// SaveBarState() uses m_pszRegistryKey + m_pszProfileName (= "BricsCad\\IntelliCAD" and "")
	CString strRegName;
	strRegName += ResourceString(IDS_REGISTRY_ROOT, "ITC"/*DNT*/);
	strRegName += _T("\\");
	
#ifdef BUILD_WITH_LICENSED_COMPONENTS
	strRegName += ResourceString(IDS_REGISTRY_INTELLICAD_5_PROFESSIONAL, "IntelliCAD 5.0 Professional"/*DNT*/);
#else
	strRegName += ResourceString(IDS_REGISTRY_INTELLICAD_5_STANDARD, "IntelliCAD 5.0 Standard"/*DNT*/);
#endif
	
	SetRegistryKey(strRegName); // "Software\\" key will be inserted in this function
	m_pszProfileName = strdup(_T("")); 

	//*** The LoadStdProfileSettings() function gets called in the constructor for CMainWindow.


	CIcadMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CIcadMultiDocTemplate(IDR_ICADTYPE,
		RUNTIME_CLASS(CIcadDoc),
		RUNTIME_CLASS(CIcadChildWnd),
		RUNTIME_CLASS(CIcadView));

	pDocTemplate->SetContainerInfo(IDR_ICADTYPE_CNTR_IP);
	pDocTemplate->SetServerInfo(
		IDR_ICADTYPE_SRVR_EMB, IDR_ICADTYPE_SRVR_IP,
		RUNTIME_CLASS(CIcadIpFrame));
	AddDocTemplate(pDocTemplate);


	// Parse command line for standard shell commands, DDE, file open
	//ParseCommandLine(m_cmdInfo);		// SystemMetrix(Maeda) 09.10.2001 change for function call timing

	//*** Display the splash screen.  Moved from OnCreate of CMainWindow() because of
	//*** an aparent timing conflict.
	bool showSplash = false;
	if(!m_cmdInfo.m_bRunEmbedded && !m_cmdInfo.m_bRunAutomated)
	{
		CSplashWnd::EnableSplashScreen(m_cmdInfo.m_bShowSplash);
		showSplash = true;
	}


	if(m_cmdInfo.m_nShellCommand==CCommandLineInfo::FileOpen)
	{
		HWND hwnd;
		if(NULL!=(hwnd=FindWindow("IntelliCADApplicationWindow"/*DNT*/,NULL)))
		{
			// We found another version of ourself. Lets defer to it:
			if(IsIconic(hwnd))
			{
				ShowWindow(hwnd,SW_RESTORE);
			}
			SetForegroundWindow(hwnd);

			//*** Pass the file name(s) to the window via the WM_DROPFILES message.
			DWORD dwBytes = (sizeof(DROPFILES)+(m_cmdInfo.m_strFileName.GetLength())+2);
			HANDLE hGlobal = GlobalAlloc((GHND | GMEM_SHARE),dwBytes);
			LPDROPFILES pDropFiles = (LPDROPFILES)GlobalLock(hGlobal);
			//*** Write the files names to the memory that is allocated after the
			//*** DROPFILES structure.
			pDropFiles->pFiles = sizeof(DROPFILES);
			LPSTR pszFile = ((LPSTR)pDropFiles)+sizeof(DROPFILES);
			strcpy(pszFile,m_cmdInfo.m_strFileName.GetBuffer(0));
			GlobalUnlock(hGlobal);
			OpenClipboard(NULL);
			EmptyClipboard();
			if(NULL==SetClipboardData(CF_HDROP,hGlobal))
			{
				DWORD dwErr = GetLastError();
				CString strError;
				strError.Format(ResourceString(IDC_ICADAPP_ERROR_WRITING_TO_0, "Error writing to clipboard (%d)." ),dwErr);
				sds_alert(strError);
				CloseClipboard();
				return FALSE;
			}
			CloseClipboard();
			::SendMessage(hwnd,WM_DROPFILES,(WPARAM)NULL,0L);
			return FALSE;
		}
}



	// Connect the COleTemplateServer to the document template.
	//	The COleTemplateServer creates new documents on behalf
	//	of requesting OLE containers by using information
	//	specified in the document template.
	m_server.ConnectTemplate(CLSID_Document, pDocTemplate, FALSE);
	// Register all OLE server factories as running.  This enables the
	//	OLE libraries to create objects from other applications.
	COleTemplateServer::RegisterAll();
		// Note: MDI applications register all server objects without regard
		//	to the /Embedding or /Automation on the command line.

	//Modified Cybage VSB 18/12/2001 DD/MM/YYYY [
	//Reason : Bug 77919 from Bugzilla
	//Description :ole32.dll dialog box dialog box in Win98, the commented code
	if (!InitATL())
		return FALSE;
	//Modified Cybage VSB 18/12/2001 DD/MM/YYYY ]

	if (FAILED(_Module.RegisterClassObjects(CLSCTX_LOCAL_SERVER, REGCLS_MULTIPLEUSE)))
		return FALSE;

	if(m_cmdInfo.m_nOpenCommand==CIcadCommandLineInfo::RegServer)
	{
		RegisterServer();
		return FALSE;
	}
	else if(m_cmdInfo.m_nOpenCommand==CIcadCommandLineInfo::UnRegServer)
	{
		UnregisterServer();
		return FALSE;
	}

	//*** Set m_pMainWnd (CWnd*) for the framework to use, and set
	//*** m_pIcadMainWnd (CMDIFrameWnd*)for our use.
	m_pMainWnd=m_pIcadMainWnd = new CMainWindow( showSplash);

	HiddenWindow::InitializeClass( IcadSharedGlobals::GetIcadAppInstance() );

	//*** Register our clipboard format.
	m_uIcadClipFormat = RegisterClipboardFormat(ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD" ));

/*
// I replaced this section from here to avoid running engine and other application threads before
// complete VBA initialization. Now it's just after VBA initialization code.
	//*** Init the Icad command line.
	SDS_Init(m_pIcadMainWnd->GetSafeHwnd(),0);

	// *** Override the search path (if one was passed with the /s"path;path").
	// *** Make sure this is done after SDS_Init(), so we have read the registry.
	if(!m_cmdInfo.m_strOverridePath.IsEmpty()) {
		struct resbuf rb;
		CString ts;
		SDS_getvar(NULL,DB_QSRCHPATH,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		ts=rb.resval.rstring;
		rb.resval.rstring=(char *)(LPCTSTR)m_cmdInfo.m_strOverridePath;
		SDS_setvar(NULL,DB_QSRCHPATH,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
		m_cmdInfo.m_strOverridePath=ts;
	}
*/

	//*** Update the registry.
	RegisterServer();

	//*** Register the Application object with OLE.
	if(!RegisterClassFactories()) return FALSE;

		// _________________________________________________APC Initializations


#include "Licenses.h"
	static const char* k_license = VBA_LICENSE;

	HRESULT hr;
	CComPtr<IIcadApplication> spApp;
	hr = m_pAutoApp->QueryInterface (&spApp);
	ASSERT(SUCCEEDED(hr));
	if (FAILED(hr))
		return FALSE;
	CString root;
	// get registry key name (check /p option)
    	getRegCurrentProfileRoot(root.GetBuffer(IC_ACADBUF), IC_ACADBUF); // Modify by SystemMetrix(Maeda) 09.10.2001
	root.ReleaseBuffer(); 
	root += "\\Addins";

	/*D.G.*/// Maybe we should call ApcHostAddIns.Create for add-ins and
	// then set g_Vba6Installed to FALSE if VBA isn't accessible. I'm not sure...
	if(!CIcadApp::m_LicensedComponents.IsAccessible(CLicensedComponents::Components::VBA))
	{
		g_Vba6Installed = FALSE;
		hr = E_FAIL;
	}
	else
		// Add-in support must be declared prior to ApcHost.Create
		hr = ApcHostAddIns.Create(spApp, TRUE, FALSE, CComBSTR(k_license), USED_VBA_LOCALE, CComBSTR(root));
	ASSERT(!g_BuiltWithVba || SUCCEEDED(hr)); //This assert indicates that the Addins are not correctly registered

	if (FAILED(hr))
		g_Vba6Installed = FALSE;
	else
		g_Vba6Installed = TRUE;

	// a return value of APC_E_BADOBJ (0x80040512 ) may indicate a failure to
	// register the applications typelib.  Check above, in the OLE Registrations
	// section of InitInstance
	CComBSTR Appname;
	Appname.LoadString(AFX_IDS_APP_TITLE);

//AfxMessageBox(Appname);

	//
	// Check the state of the SHIFT key, to override the default mode for macro
	// execution.  No startup events will be fired if the SHIFT key is held down
	// when the application is started.  See also the code in CIcadDoc::OnNewDocument
	// to deal with the Document.Open event
	//
	SHORT shiftState = GetAsyncKeyState(VK_SHIFT) & ~0x0001; // The low order bit is the shift state
	bool fEnableStartupMacros = (shiftState == 0);

	if (g_Vba6Installed)
		hr = ApcHost.Create(m_pMainWnd->GetSafeHwnd(), Appname, spApp, CComBSTR(k_license));
	if (FAILED(hr))
		g_Vba6Installed = FALSE;
	else
		g_Vba6Installed = TRUE;

	if (g_Vba6Installed)
	{

		//
		// Add Application mini-bitmap
		//
		HBITMAP hBitmap;
		PICTDESC pict;

		hBitmap = LoadBitmap(m_hInstance, MAKEINTRESOURCE(EXP_DRAWING));
		pict.cbSizeofstruct = sizeof(pict);
		pict.picType = PICTYPE_BITMAP;
		pict.bmp.hbitmap = hBitmap;
		pict.bmp.hpal = NULL;
		hBitmap = NULL;
		// OK to discard hBitmap(s), since the OleCreatePictureIndirect third argument is TRUE

		//TOTEST: MIDL mode bitmaps
		#ifdef APC_IMPORT_MIDL

			CComPtr<IPictureDisp> spPictAppDisp;
			MSAPC::Picture* spPictApp = static_cast<MSAPC::Picture*>(spPictAppDisp);

		#else

			CComPtr<Picture> spPictApp;

		#endif

		hr = OleCreatePictureIndirect(&pict, IID_IPictureDisp, TRUE, (void**) &spPictApp);
		ASSERT(SUCCEEDED(hr));
		if (SUCCEEDED(hr))
		{
			hr = ApcHost->APC_PUT(Picture)(spPictApp);
			ASSERT(SUCCEEDED(hr));
			hr = ApcHost->APC_PUT(BackColor)(RGB(255, 255, 255));
			ASSERT(SUCCEEDED(hr));
		}


		DWORD rgbBack = RGB(255, 255, 255);

		// Register the bitmap for the ThisDocument
		#if 1
			CComBSTR Docname;
			Docname.LoadString(IDC_VBA_DOCUMENT);
			hr = RegisterMiniBitmap(Docname, EXP_DRAWING, rgbBack);
		#else
			LPOLESTR bstrGuidD = NULL;
			ApcCallF(StringFromCLSID(CLSID_Document, &bstrGuidD));
			hr = RegisterMiniBitmap(bstrGuidD, EXP_DRAWING, rgbBack);
			CoTaskMemFree(bstrGuidD);
		#endif


		// Register the bitmap for the CommonProjects - ThisWorkspace
		#if 1
			CComBSTR Wsname;
			Wsname.LoadString(IDC_VBA_WORKSPACE);
			hr = RegisterMiniBitmap(Wsname, EXP_PROJECT, rgbBack);
		#else
			LPOLESTR bstrGuidW = NULL; //TOTEST: does this leak
			ApcCallF(StringFromCLSID(CLSID_Workspace, &bstrGuidW));
			hr = RegisterMiniBitmap(bstrGuidW, EXP_PROJECT, rgbBack);
			CoTaskMemFree(bstrGuidW);
		#endif



		// Add the DocTemplate for the CommonProjects
		// The library template needs to be shut down in sequence after the
		// other documents, since the other docs have a reference to the
		// library.  Thus, it must be added to the template manager last
		//
		AddDocTemplate(new CActLibraryDocTemplate());

		CSplashWnd::HideSplashScreen();

		//If we are running as a server, do not load CommonProjects, or any project...
		//if(!m_cmdInfo.m_bRunEmbedded && !m_cmdInfo.m_bRunAutomated)
		//{
		//Removed the above check - this gets the document in an unstable state and crashes when invoked via CreateObject
		//The document uses ApcProject in many places, assuming it is initialized fully ....

			// Create the IWorkspace object.
			m_bReadOnlyWorkspace = FALSE;	//usually the workspace is readwrite. But if you run another instance of Icad, the workspace in that instance becomes readonly.
			hr = CIcadWorkspace::_CreatorClass::CreateInstance(NULL, IID_IIcadWorkspace, (void**) &m_spWorkspace.p);
			if (SUCCEEDED(hr))
			{
				CComQIPtr<IActProjectStorage, &__uuidof(IActProjectStorage)> spProjectStorage(m_spWorkspace);
				if (spProjectStorage)
				{
					TCHAR szFull[_MAX_PATH];
					TCHAR szDrive[_MAX_DRIVE];
					TCHAR szDir[_MAX_DIR];
					::GetModuleFileName(NULL, szFull, sizeof(szFull)/sizeof(TCHAR));
					_tsplitpath(szFull, szDrive, szDir, NULL, NULL);
					_tcscpy(szFull, szDrive);
					_tcscat(szFull, szDir);
					_tcscat(szFull, "CommonProjects.vbi"/*DNT*/);

					//
					// Open existing or Create new
					//
					//Bugzilla::78502; 09-06-03 [
					AxSecurityLevel axLevel = axSecurityMedium;
					CRegKey reg;
					long nSuccess = reg.Create(HKEY_CURRENT_USER, m_regpathBase);
					if(ERROR_SUCCESS == nSuccess)
						nSuccess = reg.QueryValue((unsigned long&) axLevel, m_regpathSecurity);

					if((axLevel == axSecurityMedium) && (m_cmdInfo.m_nShellCommand == CCommandLineInfo::FileDDE))
						m_bDisableWorkspace = TRUE;
					//Bugzilla::78502; 09-06-03 ]
										
					hr = spProjectStorage->Open(ApcHost, CComBSTR(szFull), m_bDisableWorkspace);
					if (SUMMIT_E_FILE_NOT_EXIST == hr)
						hr = spProjectStorage->CreateNew(ApcHost, CComBSTR(szFull));
					if (S_FALSE == hr)	//there was a problem - the storage was opened as readonly
						m_bReadOnlyWorkspace = TRUE;
					ASSERT(SUCCEEDED(hr));
				}
				else
					hr = E_NOINTERFACE;
			}
			ASSERT(SUCCEEDED(hr));

		//}

	} //endif g_Vba6Installed
	else
	{
								// Just create the workspace object
								// obviously we can't load a VBA project
		hr = CIcadWorkspace::_CreatorClass::CreateInstance(NULL, IID_IIcadWorkspace, (void**) &m_spWorkspace.p);
		ASSERT(SUCCEEDED(hr));
	}

			//
			// Now fire the IWorkspaceEvents::Startup event
			//
	if (m_spWorkspace && fEnableStartupMacros)
	{
		HRESULT hr;

		if (g_Vba6Installed)
			hr = ApcFireEvent(m_spWorkspace, DIID__IcadWorkspaceEvents, DISPID_WS_STARTUP, NULL, NULL);
		else
			hr = comAddinManager ().fireWorkspaceStartup(m_spWorkspace);

		ASSERT(SUCCEEDED(hr));
	}
//-----------------------------------------------------------------------------------------------
	//*** Init the Icad command line.
	SDS_Init(m_pIcadMainWnd->GetSafeHwnd(),0);

	// *** Override the search path (if one was passed with the /s"path;path").
	// *** Make sure this is done after SDS_Init(), so we have read the registry.
	if(!m_cmdInfo.m_strOverridePath.IsEmpty()) {
		struct resbuf rb;
		CString ts;
		SDS_getvar(NULL,DB_QSRCHPATH,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		ts=rb.resval.rstring;
		IC_FREE(rb.resval.rstring);
		rb.resval.rstring=(char *)(LPCTSTR)m_cmdInfo.m_strOverridePath;
		SDS_setvar(NULL,DB_QSRCHPATH,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
		m_cmdInfo.m_strOverridePath=ts;
	}
//-----------------------------------------------------------------------------------------------

	// check version of menu
	PreferenceKey menuKey(HKEY_CURRENT_USER, "Menu", FALSE);
	//PreferenceKey configKey(HKEY_CURRENT_USER, "Config", FALSE);
	CString versionString;
	versionString.Format("%s"/*DNT*/, MLBLDVERS);
	LONG result;
	TCHAR buffer[MAX_PATH] = {0};
	DWORD bufferSize = MAX_PATH;
	//result = RegQueryValueEx((HKEY)configKey, "MenuVersion", 0, 0, buffer, &bufferSize);
	result = configKey.QueryValue(buffer, "MenuVersion", &bufferSize);
//	if(result != ERROR_SUCCESS || versionString.Compare(buffer) != 0)
//	{
		// maybe need to reset menus, toolbars, etc.
		if((HKEY)menuKey != NULL  && versionString.Compare(buffer) != 0)
		{
			if(m_pMainWnd->MessageBox(ResourceString(IDC_ICADCUSMENU_ARE_YOU_SUR_60, "IntelliCAD has newer versions of the menus and toolbars than the ones\nyou are currently using. In order to use the newer menus and toolbars,\nthey need to be reset.\n\nDo you want to reset the menus and toolbars?" ),NULL,MB_YESNO | MB_ICONQUESTION | MB_TOPMOST) == IDYES)
//			if(MessageBox(NULL, ResourceString(IDC_ICADCUSMENU_ARE_YOU_SUR_60, "IntelliCAD has newer versions of the menus and toolbars than the ones\nyou are currently using. In order to use the newer menus and toolbars,\nthey need to be reset.\n\nDo you want to reset the menus and toolbars?" ), ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD" ), MB_YESNO | MB_ICONQUESTION | MB_TASKMODAL) == IDYES)
			{
				((CMainWindow*)m_pMainWnd)->IcadCustomDia(-1);
				CIcadCustom* pCustomizeDlg = ((CIcadCustom*)((CMainWindow*)m_pMainWnd)->m_pIcadCustom);
				for(int i = 0; i < pCustomizeDlg->GetPageCount(); ++i)
				{
					pCustomizeDlg->SetActivePage(i);
					pCustomizeDlg->OnResetButton();
				}
				pCustomizeDlg->OnCloseButton();
				// write new version
				//result = RegSetValueEx((HKEY)configKey, "MenuVersion", 0, REG_SZ, buffer, versionString.GetLength() + 1);
				result = configKey.SetValue(versionString, "MenuVersion");
				if(result != ERROR_SUCCESS)
					ASSERT(false);
			}
		}
		else
		{
			// write new version
			//result = RegSetValueEx((HKEY)configKey, "MenuVersion", 0, REG_SZ, buffer, versionString.GetLength() + 1);
			result = configKey.SetValue(versionString, "MenuVersion");
			if(result != ERROR_SUCCESS)
				ASSERT(false);
		}
//	}
	
	// Now everything is done resume the command line.
	SDS_Run();

	// Modified Cybage VSB 23/03/2001 [
	// Reason: Fix for the ICAD.LSP bug reported Ronald Prepchuk
	// email dated 21/03/2001. This code is moved a little down
	// as a New .dwg file is not opened when CreateObject called from VBA
	/*
	// Check to see if launched as ActiveX server
	if(m_cmdInfo.m_bRunEmbedded || m_cmdInfo.m_bRunAutomated)
	{
		// Application was run with /Embedding or /Automation.	Don't show the
		//	main window in this case.
		// AG: we have to set this flag to avoid sticking at the beginning of
		// MainEngineThreadHandler::ThreadHandlerProc( void )
		// AG : to do: replace this flag at all with more appropriate syncronization
		bCriticalSectionOver = TRUE ;
		return TRUE;
	}
	else
	{
		//*** Play our startup sound only if we are not embedding or automation.
		cmd_PlaySound(0);
	}
	*/
	// Modified Cybage VSB 23/03/2001 ]

	//*** If a file name is passed, make sure it is valid.
	char szPath[IC_ACADBUF];
	if(!m_cmdInfo.m_strFileName.IsEmpty())
	{
		if(RTNORM==sds_findfile(m_cmdInfo.m_strFileName.GetBuffer(0),szPath))
		{
			m_cmdInfo.m_strFileName=szPath;
		}
		else
		{
			CString str;
			str.Format(ResourceString(IDC_ICADAPP_A_SHELL_COMMAND__2, "A shell command was not executed properly.  Please confirm\nthat all command line parameters are valid and try again (1)." ));
			MessageBox(NULL,str,ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD" ),MB_OK | MB_ICONERROR);
			((CMainWindow*)m_pMainWnd)->CloseApp();
			return FALSE;
		}
	}
	if(!ProcessShellCommand(m_cmdInfo))
	{
		CString str;
		str.Format(ResourceString(IDC_ICADAPP_THAT_ALL_COMMAND_4, "A shell command was not executed properly.  Please confirm\nthat all command line parameters are valid and try again (2)." ));
		MessageBox(NULL,str,ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD" ),MB_OK | MB_ICONERROR);
		((CMainWindow*)m_pMainWnd)->CloseApp();
		return FALSE;
	}

	// validate that product is not timebombed
	// It changed the timing to check authorization. by SystemMetrix(Maeda) 09.10.2001
	if ( !CheckAuthentication() )
	  {
		g_WaitEvent.SetEvent();
		return FALSE;
	  }

	// Modified Cybage VSB 23/03/2001 [
	// Reason: Fix for the ICAD.LSP bug reported Ronald Prepchuk
	// email dated 21/03/2001.
	// Check to see if launched as ActiveX server

	// We have to set this event to avoid sticking at the beginning of
	// MainEngineThreadHandler::ThreadHandlerProc( void )
	g_WaitEvent.SetEvent();

	if(m_cmdInfo.m_bRunEmbedded || m_cmdInfo.m_bRunAutomated)
	{
		return TRUE;
	}
	else
	{
		//*** Play our startup sound only if we are not embedding or automation.
		cmd_PlaySound(0);
	}
	// Modified Cybage VSB 23/03/2001 ]

	//*** Update the menu if a file was opened
	if(m_cmdInfo.m_nShellCommand==CCommandLineInfo::FileOpen)
	{
		CMainWindow* pMain = (CMainWindow*)m_pMainWnd;
		CIcadMenu* pIcadMenu;
		if(NULL!=pMain &&
			NULL!=pMain->m_pMenuMain &&
			NULL!=(pIcadMenu=pMain->m_pMenuMain->GetCurrentMenu()))
		{
			pIcadMenu->SetMenu(FALSE);
		}
	}

	ShowAppWindows();

	if(!m_cmdInfo.m_strAltPath.IsEmpty())
	{
		char szPath[IC_ACADBUF];
		//*** Open the file.
		if(sds_findfile(m_cmdInfo.m_strAltPath.GetBuffer(0),szPath)!=RTNORM ||
			!(((CMainWindow*)m_pMainWnd)->OpenFileUnknown(szPath)))
		{
			CString str;
			str.Format(ResourceString(IDC_ICADAPP_THAT_ALL_COMMAND_5, "A shell command was not executed properly.  Please confirm\nthat all command line parameters are valid and try again (3)." ));
			sds_alert(str);
		}
	}

	if (g_Vba6Installed)
	{
		// Add-ins must be loaded after the application is fully loaded,
		// and ready to respond to automation requests.
		hr = ApcHostAddIns.Load();
		ASSERT(SUCCEEDED(hr));
	}
	else
	{
	   hr = comAddinManager ().load(this);
		ASSERT(SUCCEEDED(hr));
	}

	// Dispatch commands specified on the command line

	//*** Self explanatory
	ShowTipAtStartup();

	// set up progress meter callback for DB
	static AppProgressMeter meter;

	SetAdviseProgressMeter( &meter );

	//Modified Cybage AW 17-12-01
	//Reason : DIASTAT Variable functionality added
	//Bug no. 54233 from BugZilla
	hHook = SetWindowsHookEx(HSHELL_WINDOWACTIVATED, HookForDIASTATvar, AfxGetInstanceHandle(),(DWORD)GetCurrentThreadId());

	return TRUE;

}


int CIcadApp::ExitInstance()
{

	if (m_bATLInited)
	{
		_Module.RevokeClassObjects();
		_Module.Term();
	}

	//revoke the Class Factory registration
	if (0L != m_dwRegCO)
		CoRevokeClassObject(m_dwRegCO);
	if (0L != m_dwRegActive)
		RevokeActiveObject(m_dwRegActive,NULL);

	if (m_hatchFactory)
		delete m_hatchFactory;
	// EBATECH(watanabe)-[update leader according to modifying annotation
	if (m_leaderFactory)
		delete m_leaderFactory;
	// ]-EBATECH(watanabe)

	//Modified Cybage AW 17-12-01
	//Reason : DIASTAT Variable functionality added
	//Bug no. 54233 from BugZilla
	UnhookWindowsHookEx(hHook);

	SaveLWDisplayScaleFactorInRegistry();

	if (g_Vba6Installed)
		{
//		ApcHostAddIns.Destroy();
		return CApcApplication<CIcadApp,CWinApp>::ExitInstance();
		}
	else
		return CWinApp::ExitInstance();

}

BOOL CIcadApp::InitATL()
{
	m_bATLInited = TRUE;
	_Module.Init(ObjectMap, AfxGetInstanceHandle());

	return TRUE;
}

HRESULT CIcadApp::RegisterMiniBitmap(BSTR bstrCoClass, UINT nID, LONG rgbBack)
{
	// Add ProjectItem mini-bitmap
	CComPtr<IApcMiniBitmaps> spMinis;
	HRESULT hr = ApcHost->APC_GET(MiniBitmaps)(&spMinis);
	ASSERT(SUCCEEDED(hr));

	if (spMinis)
	{
		#ifdef APC_IMPORT_MIDL

			CComPtr<IPictureDisp> spPictDisp;
			MSAPC::Picture* spPict = reinterpret_cast<MSAPC::Picture*>(spPictDisp);

		#else

			CComPtr<Picture> spPict;

		#endif

		HBITMAP hBitmap;
		PICTDESC pict;

		hBitmap = LoadBitmap(m_hInstance, MAKEINTRESOURCE(nID));
		pict.cbSizeofstruct = sizeof(pict);
		pict.picType = PICTYPE_BITMAP;
		pict.bmp.hbitmap = hBitmap;
		pict.bmp.hpal = NULL;
		pict.bmp.hbitmap = hBitmap;
		hBitmap = NULL;
		// OK to discard hBitmap(s), since the OleCreatePictureIndirect third argument is TRUE

		hr = OleCreatePictureIndirect(&pict, IID_IPictureDisp, TRUE, (void**) &spPict);
		ASSERT(SUCCEEDED(hr));
		if (SUCCEEDED(hr))
		{
			CComPtr<IApcMiniBitmap> spMini;
			hr = spMinis->APC_RAW(Add)(bstrCoClass, spPict, rgbBack, &spMini);
			ASSERT(SUCCEEDED(hr));
		}
	}
	return hr;
}

BOOL CIcadApp::RegisterClassFactories(void)
{
//*** Create our class factory and register it for this application
//*** using CoRegisterClassObject. We are able to service more than
//*** one object at a time so we use REGCLS_MULTIPLEUSE.
	HRESULT hr = S_OK;

	m_dwRegCO = 0L;
	m_dwRegActive = 0L;
	CComPtr<IIcadApplication> spApp;
	hr = m_pAutoApp->QueryInterface (&spApp);
	if(FAILED(hr))
		return FALSE;
	IClassFactory* pClsFact=NULL;
	hr = spApp->QueryInterface(IID_IClassFactory, (void**)&pClsFact);
	if(FAILED(hr))
		return FALSE;

	if(m_cmdInfo.m_bRunEmbedded || m_cmdInfo.m_bRunAutomated)
	{
		//*** Register the Application Object's CLSID with OLE.
		hr=CoRegisterClassObject(CLSID_Application,
			(IClassFactory*)pClsFact,CLSCTX_LOCAL_SERVER,
			REGCLS_MULTIPLEUSE,&(m_dwRegCO));
	}
	else
	{
		hr=RegisterActiveObject((IClassFactory*)pClsFact
			,CLSID_Application,ACTIVEOBJECT_STRONG,&(m_dwRegActive));
	}

	if (pClsFact)
		pClsFact->Release();

	if(FAILED(hr))
		return FALSE;
	return TRUE;
}

void CIcadApp::RegisterControls(void)
{
	WCHAR wszID[128];
	TCHAR szCLSID[128];
	TCHAR szModule[512];
	LPSTR pszID;

	//*** Only register the spelling checker if it has not already been registered.
	USES_CONVERSION;
	StringFromGUID2(CLSID_CICSpell,wszID,128);
	pszID=OLE2T(wszID);
	lstrcpy(szCLSID,TEXT("CLSID\\"/*DNT*/));
	lstrcat(szCLSID,pszID);
	HKEY hKey;
	if(RegOpenKeyEx(HKEY_CLASSES_ROOT,szCLSID,0,KEY_READ,&hKey)==ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return;
	}
	//*** Get the path of the application.
	GetModuleFileName(m_hInstance,szModule,sizeof(szModule)/sizeof(TCHAR));
	CString strServerPath(szModule);
	int idxChar = strServerPath.ReverseFind('\\');
	if(idxChar!=(-1))
	{
		strServerPath=strServerPath.Left(idxChar);
	}
	else
	{
		strServerPath.Empty();
	}
	//*** Register the spell checker.
	CString str(strServerPath);
	str+="\\VSpell.dll"/*DNT*/;
	HINSTANCE hLib = LoadLibrary(str);
	if(hLib!=NULL)
	{
		FARPROC lpEntryPoint = GetProcAddress(hLib,_T("DllRegisterServer"/*DNT*/));
		if(lpEntryPoint!=NULL)
		{
			(*lpEntryPoint)();
		}
	}
	//***
	return;
}

void CIcadApp::UnregisterControls(void)
{
	//*** Get the path of the application.
	TCHAR szModule[512];
	GetModuleFileName(m_hInstance,szModule,sizeof(szModule)/sizeof(TCHAR));
	CString strServerPath(szModule);
	int idxChar = strServerPath.ReverseFind('\\');
	if(idxChar!=(-1))
	{
		strServerPath=strServerPath.Left(idxChar);
	}
	else
	{
		strServerPath.Empty();
	}
	//*** Unregister the spell checker.
	CString str(strServerPath);
	str+="\\VSpell.dll"/*DNT*/;
	HINSTANCE hLib = LoadLibrary(str);
	if(hLib!=NULL)
	{
		FARPROC lpEntryPoint = GetProcAddress(hLib,_T("DllUnregisterServer"/*DNT*/));
		if(lpEntryPoint!=NULL)
		{
			(*lpEntryPoint)();
		}
	}
	//***
	return;
}

/*
BOOL CIcadApp::GetFileVersion(char* szFile, char* szVersion)
{
	DWORD	dwVerHnd = 0;		// An 'ignored' parameter, always '0'
	DWORD	dwVerInfoSize;		// Size of version information block
	CString sGetName;
	LPSTR	lpVersion;			// String pointer to 'version' text
	UINT	uVersionLen;
	BOOL	bRetCode;
	char	szLangStr[ 9 ];

	// Now lets dive in and pull out the version information:
	dwVerInfoSize = GetFileVersionInfoSize( szFile, &dwVerHnd );
	if (dwVerInfoSize)
	{
		LPSTR	lpstrVffInfo;
		HANDLE	hMem;
		hMem = GlobalAlloc( GMEM_MOVEABLE, dwVerInfoSize );
		lpstrVffInfo  = (LPSTR)GlobalLock( hMem );
		GetFileVersionInfo( szFile, dwVerHnd, dwVerInfoSize, lpstrVffInfo );

		// Get first language in the list for this file
		sGetName = "\\VarFileInfo\\Translation";
		bRetCode = VerQueryValue( (LPVOID)lpstrVffInfo,
			  (LPSTR)(LPCTSTR)sGetName,
			  (LPVOID*)&lpVersion,
			  (UINT *)&uVersionLen);
		sprintf( szLangStr, "%02X%02X%02X%02X", (BYTE)(lpVersion[ 1 ]), (BYTE)(lpVersion[ 0 ]), (BYTE)(lpVersion[ 3 ]), (BYTE)(lpVersion[ 2 ]) );

		// Get the FileVersion
		sGetName = (CString)"\\StringFileInfo\\" + szLangStr + ResourceString(IDC_ICADAPP___FILEVERSION_6, "\\FileVersion" );
		bRetCode = VerQueryValue( (LPVOID)lpstrVffInfo,
			  (LPSTR)(LPCTSTR)sGetName,
			  (LPVOID*)&lpVersion,
			  (UINT *)&uVersionLen);
		strcpy( szVersion, lpVersion );

		GlobalUnlock( hMem );
		GlobalFree( hMem );
		return TRUE;
	}
	else
	{
		// No version information available.
		strcpy( szVersion, ResourceString(IDC_ICADAPP_UNKNOWN_7, "UNKNOWN" ) );
		return FALSE;
	}
}
*/

void RegisterAddins()
	{
	PreferenceKey		addinKey(HKEY_CURRENT_USER, "Addins");
	HKEY hKey = addinKey;
	char* strval;
	CString root, root1;
	// get registry key name (check /p option)
	getRegCurrentProfileRoot(root1.GetBuffer(IC_ACADBUF), IC_ACADBUF);// Modify by SystemMetrix(Maeda) 09.10.2001
	root1.ReleaseBuffer();

	//root.Insert(0, "HKEY_CURRENT_USER\\");	// CString::insert() causes Purify overlap memory warning.
	root = "HKEY_CURRENT_USER\\" + root1;
	strval = root.GetBuffer(0);

	IcadSetKeyAndValue2 (HKEY_LOCAL_MACHINE,
		TEXT("SOFTWARE\\Microsoft\\Shared Tools\\Addin Designer\\IntelliCAD\\IntelliCAD 5"/*DNT*/),
		NULL, NULL, REG_SZ, (BYTE *)strval);

	int Value = 1;
	IcadSetKeyAndValue2 (HKEY_LOCAL_MACHINE,
		TEXT("SOFTWARE\\Microsoft\\Shared Tools\\Addin Designer\\IntelliCAD\\IntelliCAD 5\\LoadBehaviors"/*DNT*/),
		NULL, TEXT("Startup"/*DNT*/), REG_DWORD, (BYTE *)&Value);

	Value = 0;
	IcadSetKeyAndValue2 (HKEY_LOCAL_MACHINE,
		TEXT("SOFTWARE\\Microsoft\\Shared Tools\\Addin Designer\\IntelliCAD\\IntelliCAD 5\\LoadBehaviors"/*DNT*/),
		NULL, TEXT("Unloaded"/*DNT*/), REG_DWORD, (BYTE *)&Value);

	IcadSetKeyAndValue2 (hKey, "",
		NULL, NULL, REG_SZ, NULL);
	}

void UnregisterAddins()
	{
	IcadRegDeleteKey(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Microsoft\\Shared Tools\\Addin Designer\\Intellicad"/*DNT*/));
	}

void CIcadApp::RegisterServer(void)
{
//*** Creates registry enties for this server.
	WCHAR wszID[128];
	TCHAR szCLSID[128];
	TCHAR szModule[512];
	LPSTR pszID;
	//Bugzilla::78318; 28-04-03
	bool bStrictUser = false;

	//Create some base key strings.
	USES_CONVERSION;
	StringFromGUID2(CLSID_Application,wszID,128);
	pszID=OLE2T(wszID);
	lstrcpy(szCLSID,TEXT("CLSID\\"/*DNT*/));
	lstrcat(szCLSID,pszID);

	//Create ProgID keys
	// EBATECH(CNBR) -[ 2002/9/24 Windows NT Strict User should not continue next steps.
	if(	IcadSetKeyAndValue(TEXT("Icad.Application.1"/*DNT*/),NULL,
		TEXT("IntelliCAD Application Object"/*DNT*/)) == FALSE )
	{
		TRACE0("Windows NT Strict User can not write HKEY_CLASSES_ROOT and HKEY_LOCAL_MACHINE\n");
		//Bugzilla::78318; 28-04-03
		//	return;
		bStrictUser = true;
	}
	//IcadSetKeyAndValue(TEXT("Icad.Application.1"/*DNT*/),NULL,
	//	TEXT("IntelliCAD Application Object"/*DNT*/));
		
	IcadSetKeyAndValue(TEXT("Icad.Application.1"/*DNT*/),TEXT("CLSID"/*DNT*/),pszID);

	//Create VersionIndependentProgID keys
	IcadSetKeyAndValue(TEXT("Icad.Application"/*DNT*/),NULL,
		TEXT("IntelliCAD Application Object"/*DNT*/));
	IcadSetKeyAndValue(TEXT("Icad.Application"/*DNT*/),TEXT("CurVer"/*DNT*/),
		TEXT("Icad.Application.1"/*DNT*/));
	IcadSetKeyAndValue(TEXT("Icad.Application"/*DNT*/),TEXT("CLSID"/*DNT*/),pszID);

	//Create entries under CLSID
	IcadSetKeyAndValue(szCLSID,NULL,TEXT("IntelliCAD Application Object"/*DNT*/));
	IcadSetKeyAndValue(szCLSID,TEXT("ProgID"/*DNT*/),TEXT("Icad.Application.1"/*DNT*/));
	IcadSetKeyAndValue(szCLSID,TEXT("VersionIndependentProgID"/*DNT*/),
		TEXT("Icad.Application"/*DNT*/));
	IcadSetKeyAndValue(szCLSID,TEXT("NotInsertable"/*DNT*/),NULL);

	GetModuleFileName(m_hInstance,szModule,sizeof(szModule)/sizeof(TCHAR));

	IcadSetKeyAndValue(szCLSID, TEXT("LocalServer32"/*DNT*/), szModule);
	
	//*** Deal with CLSID_Document and Visio
	StringFromGUID2(CLSID_Document,wszID,128);
	pszID=OLE2T(wszID);
	lstrcpy(szCLSID,TEXT("CLSID\\"/*DNT*/));
	lstrcat(szCLSID,pszID);

	CIntelliCadPrefKey	setupKey(HKEY_CURRENT_USER, "Setup");
	HKEY hKey = setupKey;
	TCHAR szTemp[256];
	memset(&szTemp,0,sizeof(szTemp));

	BOOL bUpdateIcon = FALSE;
	if(RegOpenKeyEx(hKey,"Extension",0,KEY_READ,&hKey)==ERROR_SUCCESS)
	{
		DWORD dwStrSize=sizeof(szTemp);
		DWORD dwType;
		if(RegQueryValueEx(hKey,TEXT("INSTALL_EXTENSION"/*DNT*/),NULL,&dwType,
			(LPBYTE)szTemp,&dwStrSize)==ERROR_SUCCESS)
		{
			CString str(szTemp);
			if(str.Find(ResourceString(IDC_ICADAPP_B_11, "B" ))!=(-1))
			{
				//*** Delete the .dwg file extention association, MFC will
				//*** register it to us below.
				IcadRegDeleteKey(HKEY_CLASSES_ROOT,TEXT(".dwg"/*DNT*/));
				bUpdateIcon=TRUE;
			}
		}
//		RegCloseKey(hKey);
	}

	//*** MFC's registration stuff.
	if ( !bStrictUser ) //Bugzilla::78318; 28-04-03
		m_server.UpdateRegistry(OAT_INPLACE_SERVER);
	COleObjectFactory::UpdateRegistryAll();

	// Enable DDE Execute open
	EnableShellOpen();
	//*** Register the .dwg file extention.
	RegisterShellFileTypes(TRUE);

	if(bUpdateIcon)
	{
		//*** Ask the shell to refresh the icon list...
		SHChangeNotify(SHCNE_ASSOCCHANGED,SHCNF_FLUSHNOWAIT | SHCNF_IDLIST,NULL,NULL);
	}

#ifdef USEVISIODWGDISPLAY
	//*** Register the VisioDWG/Custom handler
	if(RegOpenKeyEx(HKEY_CLASSES_ROOT,TEXT("VisioDWGDisplay.VisioDWGDisplay\\CurVer"/*DNT*/),
			0,KEY_READ,&hKey)==ERROR_SUCCESS)
	{
		TCHAR szTemp[256];
		TCHAR szTempClsid[256];
		memset(&szTemp,0,sizeof(szTemp));
		memset(&szTempClsid,0,sizeof(szTempClsid));
		DWORD dwStrSize=sizeof(szTemp);
		DWORD dwType;
		DWORD dwStrSizeClsid=sizeof(szTempClsid);
		DWORD dwTypeClsid;
		if(RegQueryValueEx(hKey,NULL,NULL,&dwType,(LPBYTE)szTemp,&dwStrSize)==ERROR_SUCCESS)
		{
			RegCloseKey(hKey);
			lstrcat(szTemp,TEXT("\\CLSID"/*DNT*/));
			if(RegOpenKeyEx(HKEY_CLASSES_ROOT,szTemp,0,KEY_READ,&hKey)==ERROR_SUCCESS)
			{
				if(RegQueryValueEx(hKey,NULL,NULL,&dwTypeClsid,(LPBYTE)szTempClsid,&dwStrSizeClsid)==ERROR_SUCCESS)
				{
					RegCloseKey(hKey);
					lstrcpy(szTemp,TEXT("CLSID\\"/*DNT*/));
					lstrcat(szTemp,szTempClsid);
					lstrcat(szTemp,TEXT("\\InprocServer32"/*DNT*/));
					if(RegOpenKeyEx(HKEY_CLASSES_ROOT,szTemp,0,KEY_READ,&hKey)==ERROR_SUCCESS)
					{
						dwStrSize=sizeof(szTemp);
						if(RegQueryValueEx(hKey,NULL,NULL,&dwType,(LPBYTE)szTemp,&dwStrSize)==ERROR_SUCCESS)
						{
							RegCloseKey(hKey);
							HINSTANCE hLib = LoadLibrary(szTemp);
							if(hLib!=NULL)
							{
								//*** Add the display component registry information.
								FARPROC lpEntryPoint = GetProcAddress(hLib,_T("DllRegisterServer"/*DNT*/));
								if(lpEntryPoint!=NULL)
								{
									(*lpEntryPoint)();
								}
								FreeLibrary(hLib);
							}
						}
						else
						{
							RegCloseKey(hKey);
						}
					}
				}
				else
					RegCloseKey(hKey);
			}
		}
		else
			RegCloseKey(hKey);
	}
#endif // USEVISIODWGDISPLAY

	//*** Avoid the bogus "A shell command was not executed properly" messages with
	//*** long file names for extensions DWG, DXF, LSP, SCR, and SLD.
	char extensions[5][5] = { ".dwg"/*DNT*/,".dxf"/*DNT*/,".lsp"/*DNT*/,".scr"/*DNT*/,".sld"/*DNT*/ };
	int ct;
	for(ct=0; ct<5; ct++)
	{
		if(ERROR_SUCCESS==RegOpenKeyEx(HKEY_CLASSES_ROOT,extensions[ct],0,KEY_READ,
			&hKey))
		{
			DWORD dwStrSize=sizeof(szTemp);
			DWORD dwType;
			if(ERROR_SUCCESS==RegQueryValueEx(hKey,NULL,NULL,&dwType,
				(LPBYTE)szTemp,&dwStrSize))
			{
				RegCloseKey(hKey);
				lstrcat(szTemp,TEXT("\\shell\\open\\command"/*DNT*/));
				if(ERROR_SUCCESS==RegOpenKeyEx(HKEY_CLASSES_ROOT,szTemp,0,
					KEY_READ | KEY_WRITE,&hKey))
				{
					TCHAR szVal[256];
					DWORD dwStrSize=sizeof(szVal);
					DWORD dwType;
					if(ERROR_SUCCESS==RegQueryValueEx(hKey,NULL,NULL,&dwType,
						(LPBYTE)szVal,&dwStrSize))
					{
						CString str(szVal);
						str.MakeLower();
						int idx = str.Find("icad.exe %1"/*DNT*/);
						if(idx!=(-1))
						{
							//*** Force quotes around %1.
							CString str2(szVal);
							str2 = str2.Left(idx);
							str2+="icad.exe \"%1\""/*DNT*/;
							//*** Set the value
							IcadSetKeyAndValue(szTemp,NULL,str2.GetBuffer(0));
						}
					}
				}
			}
			RegCloseKey(hKey);
		}
	}	

	//*** Register any custom controls to the registry.
	RegisterControls();

	//*** Register the Addins entries - this is required to let Addin Designer know of our existence
	RegisterAddins ();

	//*** Register the automation DLL
	RegisterAutomation( true, NULL);

	//*** Register the ATL objects
	_Module.RegisterServer();

	return;
}

void CIcadApp::UnregisterServer(void)
{
	PreferenceKey		root(HKEY_CURRENT_USER, "");
	HKEY hKeyRoot = root;

//*** Removes registry entries for this server
	WCHAR wszID[128];
	TCHAR szCLSID[128];
	LPSTR pszID;

	//*** Figure out what to do with the CLSID if Visio is installed.
	USES_CONVERSION;
	StringFromGUID2(CLSID_Document,wszID,sizeof(wszID));
	pszID=OLE2T(wszID);
	lstrcpy(szCLSID,TEXT("CLSID\\"/*DNT*/));
	lstrcat(szCLSID,pszID);
	HKEY hKey;

	if(RegOpenKeyEx(HKEY_CLASSES_ROOT,TEXT(".dwg"/*DNT*/),0,KEY_READ,&hKey)==ERROR_SUCCESS)
	{
		TCHAR szTemp[256];
		DWORD dwStrSize=sizeof(szTemp);
		DWORD dwType;
		if(RegQueryValueEx(hKey,NULL,NULL,&dwType,(LPBYTE)szTemp,&dwStrSize)
			==ERROR_SUCCESS)
		{
			CString str(szTemp);
			if(str.Compare("Icad.Drawing"/*DNT*/)==0)
			{
				//*** Delete the .dwg file extention association
				IcadRegDeleteKey(HKEY_CLASSES_ROOT,TEXT(".dwg"/*DNT*/));
			}
		}
		RegCloseKey(hKey);
	}

	BOOL bDeleteDocCLSID = FALSE;
	if(RegOpenKeyEx(HKEY_CLASSES_ROOT,TEXT("Icad.Drawing"/*DNT*/),0,KEY_READ,&hKey)
		==ERROR_SUCCESS)
	{
		//*** Only delete the document clsid if we are sure icad has been installed.
		bDeleteDocCLSID=TRUE;
	}

	IcadRegDeleteKey(HKEY_CLASSES_ROOT,TEXT("Icad.Drawing"/*DNT*/));
	IcadRegDeleteKey(hKeyRoot, "");

	TCHAR szTempClsid[256];
	IcadRegDeleteKey(HKEY_CLASSES_ROOT,TEXT("Icad.Application"/*DNT*/));
	IcadRegDeleteKey(HKEY_CLASSES_ROOT,TEXT("Icad.Application.1"/*DNT*/));
	StringFromGUID2(CLSID_Application,wszID,sizeof(wszID));
	pszID=OLE2T(wszID);
	lstrcpy(szTempClsid,TEXT("CLSID\\"/*DNT*/));
	lstrcat(szTempClsid,pszID);
	IcadRegDeleteKey(HKEY_CLASSES_ROOT,szTempClsid);

	//*** UnRegister the VisioDWG/Custom handler
	if(RegOpenKeyEx(HKEY_CLASSES_ROOT,TEXT("VisioDWGDisplay.VisioDWGDisplay\\CurVer"/*DNT*/),
			0,KEY_READ,&hKey)==ERROR_SUCCESS)
	{
		TCHAR szTemp[256];
		memset(&szTemp,0,sizeof(szTemp));
		memset(&szTempClsid,0,sizeof(szTempClsid));
		DWORD dwStrSize=sizeof(szTemp);
		DWORD dwType;
		DWORD dwStrSizeClsid=sizeof(szTempClsid);
		DWORD dwTypeClsid;
		if(RegQueryValueEx(hKey,NULL,NULL,&dwType,(LPBYTE)szTemp,&dwStrSize)==ERROR_SUCCESS)
		{
			RegCloseKey(hKey);
			lstrcat(szTemp,TEXT("\\CLSID"/*DNT*/));
			if(RegOpenKeyEx(HKEY_CLASSES_ROOT,szTemp,0,KEY_READ,&hKey)==ERROR_SUCCESS)
			{
				if(RegQueryValueEx(hKey,NULL,NULL,&dwTypeClsid,(LPBYTE)szTempClsid,&dwStrSizeClsid)==ERROR_SUCCESS)
				{
					RegCloseKey(hKey);
					lstrcpy(szTemp,TEXT("CLSID\\"/*DNT*/));
					lstrcat(szTemp,szTempClsid);

					//*** If the CLSID being referenced here is the same as the one being
					//*** referenced by our document, then the old display component must
					//*** be installed, so don't delete the key, let the display component
					//*** update it.
					if(0!=lstrcmp(szCLSID,szTemp) && bDeleteDocCLSID)
					{
						IcadRegDeleteKey(HKEY_CLASSES_ROOT,szCLSID);
						bDeleteDocCLSID=FALSE;
					}

					lstrcat(szTemp,TEXT("\\InprocServer32"/*DNT*/));
					if(RegOpenKeyEx(HKEY_CLASSES_ROOT,szTemp,0,KEY_READ,&hKey)==ERROR_SUCCESS)
					{
						dwStrSize=sizeof(szTemp);
						if(RegQueryValueEx(hKey,NULL,NULL,&dwType,(LPBYTE)szTemp,&dwStrSize)==ERROR_SUCCESS)
						{
							RegCloseKey(hKey);
							HINSTANCE hLib = LoadLibrary(szTemp);
							if(hLib!=NULL)
							{
								//*** Update the display component registry information.
								FARPROC lpEntryPoint = GetProcAddress(hLib,_T("DllRegisterServer"/*DNT*/));
								if(lpEntryPoint!=NULL)
								{
									(*lpEntryPoint)();
									bDeleteDocCLSID=FALSE;
								}
								FreeLibrary(hLib);
							}
						}
						else
							RegCloseKey(hKey);
					}
				}
				else
					RegCloseKey(hKey);
			}
		}
		else
			RegCloseKey(hKey);
	}

	if(bDeleteDocCLSID)
	{
		IcadRegDeleteKey(HKEY_CLASSES_ROOT,szCLSID);
	}
	//*** Unregister any custom controls.
	UnregisterControls();

	//*** UnRegister the Addins entries
	UnregisterAddins ();

	//*** Unregister automation.
	UnregisterAutomation();

	//*** Unregister the ATL objects
	_Module.UnregisterServer();

	return;
}

void CIcadApp::ShowAppWindows(void)
{
	struct sds_resbuf rb;
	//*** Set the location of the main window (maximized/minimized/normal)
	SDS_getvar(NULL,DB_QWNDLMAIN,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if(rb.resval.rint==0) {
		m_pIcadMainWnd->ShowWindow(SW_SHOWNORMAL);
	} else if(rb.resval.rint==1) {
		m_pIcadMainWnd->ShowWindow(SW_SHOWMINIMIZED);
	} else if(rb.resval.rint==2) {
		m_pIcadMainWnd->ShowWindow(SW_SHOWMAXIMIZED);
	}

	//*** Set the location of the text window (off/normal/maximized)
	SDS_getvar(NULL,DB_QWNDLTEXT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if(rb.resval.rint==0) {
		((CMainWindow*)m_pIcadMainWnd)->m_pTextScrn->ShowWindow(SW_HIDE,FALSE);
	} else if(rb.resval.rint==1) {
		((CMainWindow*)m_pIcadMainWnd)->m_pTextScrn->ShowWindow(SW_SHOWNORMAL,FALSE);
	} else if(rb.resval.rint==2) {
		((CMainWindow*)m_pIcadMainWnd)->m_pTextScrn->ShowWindow(SW_SHOWMINIMIZED,FALSE);
	} else if(rb.resval.rint==3) {
		((CMainWindow*)m_pIcadMainWnd)->m_pTextScrn->ShowWindow(SW_SHOWMAXIMIZED,FALSE);
	}
}

void CIcadApp::InitMRUList(UINT nMaxMRU)
{
	LoadStdProfileSettings((nMaxMRU<=0) ? _AFX_MRU_COUNT : nMaxMRU);
}

void CIcadApp::FileNew()
{
	CWinApp::OnFileNew();
}

CIcadDoc* CIcadApp::GetOpenFile(LPCTSTR pStr)
{
	//*** Most of this code was copied from DOCMGR.CPP CDocManager::OpenDocumentFile()
	//*** in the MFC source.

	// find the highest confidence
	POSITION pos = GetFirstDocTemplatePosition();
	CDocTemplate::Confidence bestMatch = CDocTemplate::noAttempt;
	CDocTemplate* pBestTemplate = NULL;
	CDocument* pOpenDocument = NULL;

	TCHAR szPath[_MAX_PATH];
	ASSERT(lstrlen(pStr) < sizeof(szPath));
#ifndef _MAC
	TCHAR szTemp[_MAX_PATH];
	if (pStr[0] == '\"')
		++pStr;
	lstrcpyn(szTemp, pStr, _MAX_PATH);
	LPTSTR lpszLast = _tcsrchr(szTemp, '\"');
	if (lpszLast != NULL)
		*lpszLast = 0;
	AfxFullPath(szPath, szTemp);
	TCHAR szLinkName[_MAX_PATH];
	if (AfxResolveShortcut(AfxGetMainWnd(), szPath, szLinkName, _MAX_PATH))
		lstrcpy(szPath, szLinkName);

#else
	lstrcpyn(szPath, lpszFileName, _MAX_PATH);
	WIN32_FIND_DATA fileData;
	HANDLE hFind = FindFirstFile(lpszFileName, &fileData);
	if (hFind != INVALID_HANDLE_VALUE)
		VERIFY(FindClose(hFind));
	else
		fileData.dwFileType = 0;	// won't match any type
#endif

	while (pos != NULL)
	{
		CDocTemplate* pTemplate = (CDocTemplate*)GetNextDocTemplate(pos);
		ASSERT_KINDOF(CDocTemplate, pTemplate);

		CDocTemplate::Confidence match;
		ASSERT(pOpenDocument == NULL);
#ifndef _MAC
		match = pTemplate->MatchDocType(szPath, pOpenDocument);
#else
		match = pTemplate->MatchDocType(szPath, fileData.dwFileType, pOpenDocument);
#endif
		if (match > bestMatch)
		{
			bestMatch = match;
			pBestTemplate = pTemplate;
		}
		if (match == CDocTemplate::yesAlreadyOpen)
			break;		// stop here
	}

	return (CIcadDoc*)pOpenDocument;
}

void CIcadApp::FileOpen(LPCTSTR pStr)
{
	BOOL bSendOpenCallback = TRUE;

	CIcadDoc* pOpenDoc = GetOpenFile(pStr);
	if(NULL!=pOpenDoc)	//the file is already open
	{
		CString str;
		str.Format(ResourceString(IDC_ICADAPP_THE_DRAWING___18, "The drawing \"%s\" is already open.\nDo you want to revert to the last saved version?" ),pStr);
		if(IDYES==m_pMainWnd->MessageBox(str,ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD" ),
			MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2))
		{
			//*** Close the drawing that is open
			pOpenDoc->OnCloseDocument();
		}
		else
			bSendOpenCallback = FALSE;	//do not send callback if we are not reverting to the last saved version

	}
	CIcadDoc* pNewDoc = static_cast<CIcadDoc*>(CWinApp::OpenDocumentFile(pStr));
	if(NULL==pNewDoc) return;

	if (bSendOpenCallback)
		{
		SDS_CallUserCallbackFunc(SDS_CBOPENDOC,(void *)(char *)pStr,NULL,NULL);

		POSITION posView;
		CIcadView* pView;
		posView = pNewDoc->GetFirstViewPosition();
		pView = (CIcadView*)pNewDoc->GetNextView(posView);
		if (pView)
			SDS_CMainWindow->DocChangeCallback( pView );
		}

	//*** This needs to be done here instead of OnOpenDocument because MFC
	//*** overwrites it.
	CString str(pNewDoc->GetTitle());
	if(pNewDoc->m_bOpenReadOnly || pNewDoc->m_bIsReadOnly || pNewDoc->m_bAlreadyOpen ||
		pNewDoc->m_hFile==NULL)
	{
		CString strReadOnly(ResourceString(IDC_ICADAPP___READ_ONLY__21, " (Read-Only)" ));
		if(str.Find(strReadOnly)==(-1))
		{
			str+=strReadOnly;
		}
		pNewDoc->SetTitle(str);
	}
	if(str.GetLength()>30)
	{
		//*** If the string is too long add ... to the display string.
		int nStrLen = str.GetLength()+1;
		char* pszTmp = new char[nStrLen];
		memset(pszTmp,0,nStrLen);
		strcpy(pszTmp,str);
		ic_dotstr(pszTmp,50,ResourceString(IDC_ICADAPP_E_22, "e" ));
		str=pszTmp;
		delete [] pszTmp;
		pNewDoc->SetTitle(str);
	}
	//*** Update the most recent used file list in the menus
	if(NULL!=m_pIcadMainWnd && NULL!=((CMainWindow*)m_pIcadMainWnd)->m_pMenuMain)
	{
		((CMainWindow*)m_pIcadMainWnd)->m_pMenuMain->UpdateMRU(TRUE);
	}

	if (g_Vba6Installed)
	{
		// Name the VBA Project
		if (pNewDoc->ApcProject)
		{
			CString caption;
			CString title = pNewDoc->GetTitle();
			int n = title.Find('.');
			if (n==-1)
				caption = title;
			else
				caption = title.Left(n);
			VERIFY(SUCCEEDED(pNewDoc->ApcProject->APC_PUT(Name)(CComBSTR(caption))));

			//just setting the name shouldn't dirty the project
			pNewDoc->ApcProject->APC_PUT(Dirty)(VARIANT_FALSE);
		}
	} //endif g_Vba6Installed

	return;
}

void CIcadApp::ShowTipAtStartup(void)
{
	if (m_cmdInfo.m_bShowSplash)
	{
		CTipDlg dlg;
		if (dlg.m_bStartup)
			dlg.DoModal();
	}

}

void CIcadApp::ShowTipOfTheDay(void)
{
	CTipDlg dlg;
	dlg.DoModal();

}

//	Subclassed CPrintDialog to set hook proc for debugging
class IcadPrintDialog : public CPrintDialog
{
public:
	IcadPrintDialog(BOOL setupOnly);

#ifdef USE_ICAD_SPOOLING
	~IcadPrintDialog();
	CButton		*m_pSpoolingButton,*m_pResetButton;
	CDlgSpoolingSettings	m_spoolingSettingsDlg;
protected:
	BOOL OnInitDialog();
	void OnSpooling();
	void OnReset();
	DECLARE_MESSAGE_MAP()
#endif

};

// this hook is for debug purposes only now
static COMMDLGPROC	setupHook;
UINT __stdcall IcadDialogProc(HWND window, UINT message, UINT wParam, LONG lParam)
{
	static UINT	lastMessage;

	switch(message)
	{
		case WM_DELETEITEM:
		{
			LPDELETEITEMSTRUCT lpdis = (LPDELETEITEMSTRUCT)lParam;
			lastMessage = message;
			break;
		}
		case WM_DEVMODECHANGE:
			lastMessage = message;
	}

	return (*setupHook)(window, message, wParam, lParam);
}


IcadPrintDialog::IcadPrintDialog(BOOL setupOnly) : CPrintDialog(setupOnly), m_pSpoolingButton(NULL),m_pResetButton(NULL)
{
	setupHook = m_pd.lpfnSetupHook;
	m_pd.lpfnSetupHook = IcadDialogProc;
}


#ifdef USE_ICAD_SPOOLING
IcadPrintDialog::~IcadPrintDialog()
{
	delete m_pSpoolingButton;
	delete m_pResetButton;
}

BEGIN_MESSAGE_MAP(IcadPrintDialog, CPrintDialog)
	//{{AFX_MSG_MAP(IcadPrintDialog)
	ON_BN_CLICKED(IDC_SPOOLING, OnSpooling)
	ON_BN_CLICKED(IDC_RESET, OnReset)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// this function adds the "Spooling..." button to the system dialog
BOOL IcadPrintDialog::OnInitDialog()
{
	CPrintDialog::OnInitDialog();

	// add "Reset" and "Spooling..." buttons
	CRect		okRect, cancelRect;
	GetDlgItem(IDOK)->GetWindowRect(&okRect);
	ScreenToClient(&okRect);
	GetDlgItem(IDCANCEL)->GetWindowRect(&cancelRect);
	ScreenToClient(&cancelRect);
	long	lengthPlusGap = okRect.Width() + (cancelRect.left - okRect.right);

	m_pSpoolingButton = new CButton;
	m_pResetButton = new CButton;

	m_pResetButton->Create(ResourceString(IDS_RESET, "Reset" ), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
						   CRect(okRect.left - lengthPlusGap, okRect.top, okRect.right - lengthPlusGap, okRect.bottom), this, IDC_RESET);
	m_pResetButton->SetFont(GetDlgItem(IDOK)->GetFont());

	m_pSpoolingButton->Create(ResourceString(IDS_SPOOLING, "Spooling..."), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
							  CRect(okRect.left - 2 * lengthPlusGap, okRect.top, okRect.right - 2 * lengthPlusGap, okRect.bottom), this, IDC_SPOOLING);
	m_pSpoolingButton->SetFont(GetDlgItem(IDOK)->GetFont());

	return TRUE;
}

void IcadPrintDialog::OnSpooling()
{
	m_spoolingSettingsDlg.DoModal();
}

void IcadPrintDialog::OnReset()
{
	((PlotManager*)SDS_CMainWindow->m_pIcadPrintDlg->GetPlotManager())->getSystemDefaultPrinterSettings();
	EndDialog(0);	// return value of PrintDlg for 'cancel' is zero
	PostUIAction(ICAD_WNDACTION_PSETUP);
}


/////////////////////////////////////////////////////////////////////////////////////


#endif // USE_ICAD_SPOOLING

/////////////////////////////////////////////////////////////////////////////////////

void CIcadApp::FilePrinterSetup(void)
{
	InitPrintDialog();	// create PlotManager object (it gets Printer Setup settings from the registry during the creating)
	IcadPrintDialog pd(TRUE);

#ifdef USE_ICAD_SPOOLING
	if(DoPrintDialog(&pd) == IDOK)
	{
		UserPreference::s_bForcePrintToFile = !!pd.m_spoolingSettingsDlg.m_bForcePrintToFile;
		UserPreference::s_bUseOutputFolder = !!pd.m_spoolingSettingsDlg.m_bUseOutputFolder;
		UserPreference::s_outputFolder = pd.m_spoolingSettingsDlg.m_outputFolder;
		putPrinterSettingsToPlotcfg();

		bSavePrinterSettingsAfterFirstCall = true;
	}
#else
	DoPrintDialog(&pd);
	putPrinterSettingsToPlotcfg();
#endif
}


//4M Item:46->
long  cmd_ucszaxis(const sds_point pp1,const sds_point pp2);
long cmd_ucsprev(void);
//<-4M Item:46

BOOL CIcadApp::IcadPrintDlg(int nFlags)
{
	int ret=IDOK;
	double ptTmp[3];
	sds_name ename;
	struct sds_resbuf rb,*pRb;
	long ssct;

	BOOL bDisplay = (nFlags & PRINTDLG_DISPLAY);			//*** Display the dialog.
	BOOL bPreview = (nFlags & PRINTDLG_PREVIEW);			//*** Gray-out the Select Print Area and Select Origin buttons
	BOOL bPrinting = (nFlags & PRINTDLG_PRINTING);			//*** Title the dialog "Print" instead of "Page Setup"
	BOOL bPrintDirect = (nFlags & PRINTDLG_PRINTDIRECT);	//*** Gray-out the Preview button.

	if(m_pIcadMainWnd==NULL) return IDCANCEL;
	//if(((CMainWindow*)m_pIcadMainWnd)->m_pIcadPrintDlg==NULL)
	//{
	//	((CMainWindow*)m_pIcadMainWnd)->m_pIcadPrintDlg = new PrintDialog();
	//}
	InitPrintDialog();
	CPrintDialogSheet* pPrintDlg = ((CMainWindow*)m_pIcadMainWnd)->m_pIcadPrintDlg;
	if(pPrintDlg==NULL) return IDCANCEL;

	CIcadDoc* pDoc = NULL;
	CView* pView = ((CMainWindow*)m_pIcadMainWnd)->GetIcadView();

	pDoc=(CIcadDoc*)pView->GetDocument();

	db_drawing *flp=NULL;
	if(pDoc) flp=pDoc->m_dbDrawing;

	if(sds_sslength(pPrintDlg->m_scaleViewTab.m_ssPrintEnt,&ssct)==RTNORM && ssct>0L)
	{
		//*** Re-hilight the selection set
		for(long ssct=0L; sds_ssname(((CMainWindow*)m_pIcadMainWnd)->m_pIcadPrintDlg->m_scaleViewTab.m_ssPrintEnt,ssct,ename)==RTNORM; ssct++)
		{
			sds_redraw(ename,IC_REDRAW_HILITE);
		}
	}
	else
	{
		//*** Check for gripped entities
		if((ret=SDS_getvar(NULL,DB_QPICKFIRST,&rb,flp,&SDS_CURCFG,&SDS_CURSES))!=RTNORM)
		{
			return IDCANCEL;
		}
		if(rb.resval.rint && pDoc &&
			sds_sslength(pDoc->m_pGripSelection,&ssct)==RTNORM && ssct>0L)
		{
			ic_sscpy(pPrintDlg->m_scaleViewTab.m_ssPrintEnt,pDoc->m_pGripSelection);
			sds_ssfree(((CIcadDoc*)pDoc)->m_pGripSelection);
			pPrintDlg->m_scaleViewTab.m_SelMode=PRN_OPT_SELENTS;
			for(long ssct=0L; sds_ssname(((CMainWindow*)m_pIcadMainWnd)->m_pIcadPrintDlg->m_scaleViewTab.m_ssPrintEnt,ssct,ename)==RTNORM; ssct++)
			{
				sds_redraw(ename,IC_REDRAW_HILITE);
			}
		}
		else
		{
			pPrintDlg->m_scaleViewTab.m_SelMode=PRN_OPT_ALLENTS;
		}
	}

	if(!bPreview)
	{
		//*** Set a flag specifying whether we currently in paper space or not.
		struct sds_resbuf rb;
		BOOL bInPaperSpace = FALSE;
		SDS_getvar(NULL,DB_QTILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		if(rb.resval.rint==0)
		{
			SDS_getvar(NULL,DB_QCVPORT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			if(rb.resval.rint==1)
				bInPaperSpace=TRUE;
		}
		//*** Get the list of saved views.
		pPrintDlg->m_scaleViewTab.ResetViewList();
		for(pRb=sds_tblnext(ResourceString(IDC_ICADAPP_VIEW_23, "VIEW" ),1); pRb!=NULL; pRb=sds_tblnext(ResourceString(IDC_ICADAPP_VIEW_23, "VIEW" ),0))
		{
			//*** If this is a paper space view and we are not in paper space, continue.
			if(ic_assoc(pRb,70)!=0)
			{
				continue;
			}
			if(bInPaperSpace!=(BOOL)(ic_rbassoc->resval.rint&1))
			{
				continue;
			}
			pPrintDlg->m_scaleViewTab.m_ViewList->AddTail(pRb);
		}
	}

	pPrintDlg->m_Printing=bPrinting;
	pPrintDlg->m_Preview=bPreview;
	pPrintDlg->m_PrintDirect=bPrintDirect;

	ret=IDOK;

	//Need propreties to set some settings properly.
	//IIcadPlotProperties *properties = pPrintDlg->GetPlotProperties();
	//IIcadPlotManager *plotMgr = pPrintDlg->GetPlotManager();

	for(;;)
	{
		if(bDisplay) ret=pPrintDlg->DoModal();

		if(bPreview && (ret==PRN_SELECT_AREA || ret==PRN_SELECT_ORIGIN ||
			ret==IDOK || ret==PRN_PREVIEW))
		{
			//*** If this function was called from the Setup... button in print preview
			//*** we need to make sure we close down the preview to select the point(s).
			pView=((CMainWindow*)m_pIcadMainWnd)->GetIcadView();
			if(!pView->IsKindOf(RUNTIME_CLASS(CIcadView)))
			{
				((CPreviewView*)pView)->SendMessage(WM_COMMAND,AFX_ID_PREVIEW_CLOSE,0);
			}
		}
		if(ret==PRN_SELECT_AREA)
		{
			pPrintDlg->m_prevTab=CPrintDialogSheet::SCALE_VIEW ;// SCALE_VIEW

//4M Item:47->
// IDC_ZOOM Cursor is not wanted
/*
			SDS_SetCursor(IDC_ZOOM);
*/
//<-4M Item:47

			sds_point corner;

			if((ret=internalGetpoint(NULL,ResourceString(IDC_ICADAPP__NSELECT_FIRST__24, "\nSelect first corner of window: " ),ptTmp))==RTNORM)
			{
//4M Item:47->
// IDC_ZOOM Cursor is not wanted
/*
				SDS_SetCursor(IDC_ZOOM);
*/
//<-4M Item:47
				if((ret=sds_getcorner(ptTmp,ResourceString(IDC_ICADAPP__NSELECT_SECOND_25, "\nSelect second corner of window: " ),corner))==RTNORM)
				{
					pPrintDlg->setWindow(ptTmp[0], ptTmp[1], corner[0], corner[1]);
				}
			}
			// Turn the cursor off.
			((CMainWindow*)m_pIcadMainWnd)->IcadWndAction(ICAD_WNDACTION_CURSOROFF);
		}
		else if(ret==PRN_SELECT_ORIGIN)
		{
			pPrintDlg->m_prevTab=CPrintDialogSheet::ADVANCED;  // ADVANCED

			if((ret=internalGetpoint(NULL,ResourceString(IDC_ICADAPP__NSELECT_POINT__26, "\nSelect point for new origin: " ),ptTmp))==RTNORM)
			{
				pPrintDlg->setOrigin(ptTmp[0], ptTmp[1]);
				//properties->put_OriginX(ptTmp[0]);
				//properties->put_OriginY(ptTmp[1]);
			}
			// Turn the cursor off.
			((CMainWindow*)m_pIcadMainWnd)->IcadWndAction(ICAD_WNDACTION_CURSOROFF);
		}
		else if(ret==PRN_PREVIEW) //|| (bPreview && ret==IDOK))
		{
			//*** Call up the print preview.
			pView=((CMainWindow*)m_pIcadMainWnd)->GetIcadView();
			((CIcadView*)pView)->FilePrintPreview();
			if(PRN_PREVIEW==ret) ret=IDCANCEL;

			break;
		}
		else break;
	}


	if(ret==IDCANCEL) return FALSE;
	return TRUE;
}

BOOL CIcadApp::GetPrinterDC(CDC& dc)
{
	UpdatePrinterSelection(FALSE);
	BOOL bRet = CreatePrinterDC(dc);
	CheckFPControlWord();
	return bRet;
}

CString CIcadApp::GetMRUString(int nIndex)
{
	//CString str("");
	//str.Empty(); // Somehow str can have garbage in it causeing
	// a saveas in PRO (with VBA on) build to hang when CIcadMenu::UpdateVisibility()
	// calls this function When this function returns an empty string,
	// UpdateVisibility() breaks out of the for loop that calls this
	// function. Have used return CString("") instead.
	if(m_pRecentFileList==NULL || nIndex>=m_pRecentFileList->m_nSize) 
		return CString("");
	return m_pRecentFileList->m_arrNames[nIndex];
}

void CIcadApp::SetMRUSize(int nSize)
{
	int nOldSize = m_pRecentFileList->m_nSize;
	if(nSize<0 || nSize==nOldSize)
		return;

	if(nSize<nOldSize)
	{
		while(nOldSize>nSize)
		{
			RemoveMRUString(nOldSize);
			nOldSize--;
		}
	}
	else
	{
		//*** We have to setup a new, larger, string array for the MRU list.
		int idx;
		CString* arrNames = new CString[nSize];
		for(idx=0; idx<nSize; idx++)
		{
			if(idx<nOldSize)
			{
				arrNames[idx]=m_pRecentFileList->m_arrNames[idx];
				continue;
			}
			arrNames[idx].Empty();
		}
		delete[] m_pRecentFileList->m_arrNames;
		m_pRecentFileList->m_arrNames = arrNames;
	}
	m_pRecentFileList->m_nSize=nSize;
}

void CIcadApp::RemoveMRUString(int nIndex)
{
	CString str;
	str.Empty();
	if(m_pRecentFileList==NULL || nIndex>=m_pRecentFileList->m_nSize) return;
	m_pRecentFileList->Remove(nIndex);
}

BOOL CIcadApp::HasInPlaceActiveItem()
{
	POSITION posDoc,posView;
	CIcadDoc* pDoc;
	CView* pView;
	//*** Loop through the document templates.
	POSITION posDocTemplate = GetFirstDocTemplatePosition();
	CDocTemplate* pDocTemplate;
	while(NULL!=posDocTemplate)
	{
		pDocTemplate=GetNextDocTemplate(posDocTemplate);
		if(NULL==pDocTemplate)
		{
			continue;
		}
		//*** Loop through the documents.
		posDoc=pDocTemplate->GetFirstDocPosition();
		while(NULL!=posDoc)
		{
			pDoc=(CIcadDoc*)pDocTemplate->GetNextDoc(posDoc);
			if(NULL==pDoc)
			{
				continue;
			}
			//*** Loop through the views for this document.
			posView=pDoc->GetFirstViewPosition();
			while(NULL!=posView)
			{
				pView=pDoc->GetNextView(posView);

				COleClientItem* pActiveItem = pDoc->GetInPlaceActiveItem(pView);
				if(pActiveItem != NULL)
				{
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

int CIcadApp::Run()
{
	ASSERT_VALID(this);
//	if(!MessageLoop(TRUE)) return 1;
//	return 0;
	return CWinApp::Run();
}

BOOL CIcadApp::MessageLoop(BOOL bPushedByHost)
{
	return FALSE;

	/*
	// for tracking the idle time state
	BOOL bIdle=TRUE;
	LONG lIdleCount=0;

	BOOL bContinue=TRUE;
	// acquire and dispatch messages until a WM_QUIT message is received.
	for (;;)
	{
									// always reset floating point control
									// to known status
									// (sometimes changed by drivers)
		CheckFPControlWord();

		do
		{
			// pump message, but quit on WM_QUIT
			if(!PumpMessage()) return ExitInstance();

			// reset "no idle" state after pumping "normal" message
			if(IsIdleMessage(&m_msgCur))
			{
				bIdle=TRUE;
				lIdleCount=0;
			}

		} while(::PeekMessage(&m_msgCur,NULL,NULL,NULL,PM_NOREMOVE));
	}
	ASSERT(FALSE);	// not reachable
	*/
}

BOOL CIcadApp::PreTranslateMessage(MSG* pMsg)
{

	if (g_Vba6Installed)
	{
		long fConsumed;
		if (ApcHost.GetApcCompManager())
			ApcHost.GetApcCompManager()->APC_RAW(PreTranslateMessage)(pMsg, &fConsumed);
		if (fConsumed)
			return TRUE;

		if(!ApcHost.m_fHostIsActiveComponent)
		{
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			return TRUE;
		}
	} //endif g_Vba6Installed

	return CWinApp::PreTranslateMessage(pMsg);
}

#ifdef OLDVBA
BOOL CIcadApp::MessageLoop(BOOL bPushedByHost)
{
	// for tracking the idle time state
	BOOL bIdle=TRUE;
	LONG lIdleCount=0;

	BOOL bContinue=TRUE;
	// acquire and dispatch messages until a WM_QUIT message is received.
	for (;;)
	{
									// always reset floating point control
									// to known status
									// (sometimes changed by drivers)
		CheckFPControlWord();

		if(m_pIVbaCompManager)
		{
			m_pIVbaCompManager->ContinueMessageLoop(bPushedByHost,&bContinue);
		}
		if(bContinue==FALSE) return FALSE;

		// phase1: check to see if we can do idle work
		while(bIdle &&
			!::PeekMessage(&m_msgCur, NULL, NULL, NULL, PM_NOREMOVE))
		{
			// call OnIdle while in bIdle state
			if(DoVbaComponentIdle()==TRUE) continue;
			if(m_pIVbaCompManager!=NULL)
			{
				m_pIVbaCompManager->OnWaitForMessage();
			}
			if(!m_bIsComponentTracking)
			{
				if(!OnIdle(lIdleCount++))
					bIdle=FALSE; // assume "no idle" state
			}
		}

		// phase2: pump messages while available
		do
		{
			// pump message, but quit on WM_QUIT
			if(!PumpMessage()) return ExitInstance();

			// reset "no idle" state after pumping "normal" message
			if(IsIdleMessage(&m_msgCur))
			{
				bIdle=TRUE;
				lIdleCount=0;
			}

		} while(::PeekMessage(&m_msgCur,NULL,NULL,NULL,PM_NOREMOVE));
	}
	ASSERT(FALSE);	// not reachable
}

BOOL CIcadApp::PreTranslateMessage(MSG* pMsg)
{
	BOOL bConsumed=FALSE;
	if(m_pIVbaCompManager!=NULL)
	{
		m_pIVbaCompManager->PreTranslateMessage(&m_msgCur,&bConsumed);
	}
	if(bConsumed)
	{
		return TRUE;
	}

	if(!m_bIsHostActiveComponent)
	{
		::TranslateMessage(pMsg);
		::DispatchMessage(pMsg);
		return TRUE;
	}
	return CWinApp::PreTranslateMessage(pMsg);
}

BOOL CIcadApp::DoVbaComponentIdle()
{
	BOOL bContinue;

	if(!m_pIVbaCompManager) return FALSE;

	// if we have periodic idle tasks that to be done,
	// do them and return TRUE.
	m_pIVbaCompManager->DoIdle(VBAIDLEFLAG_Periodic,&bContinue);
	if(bContinue) return TRUE;

	// if any high priority idle tasks need to be done,
	// do it, and return TRUE.
	m_pIVbaCompManager->DoIdle(VBAIDLEFLAG_Priority,&bContinue);
	if(bContinue) return TRUE;

	// if any lower priority idle tasks need to be done,
	// do it, and return TRUE.
	m_pIVbaCompManager->DoIdle(VBAIDLEFLAG_NonPeriodic,&bContinue);
	if(bContinue) return TRUE;

	return FALSE;
}

#endif //OLDVBA

/////////////////////////////////////////////////////////////////////////////
// Automation Helpers

HRESULT CIcadApp::InitPrintDialog ()
{
	if(((CMainWindow*)m_pIcadMainWnd)->m_pIcadPrintDlg==NULL)
	{
		((CMainWindow*)m_pIcadMainWnd)->m_pIcadPrintDlg = new CPrintDialogSheet(ResourceString(IDC_PRINT_DLG, "Print"));
		if(((CMainWindow*)m_pIcadMainWnd)->m_pIcadPrintDlg==NULL)
			return E_FAIL;
	}
	if(!getPrinterSettingsFromPlotcfg())
		return E_FAIL;
	return S_OK;
}

/*-------------------------------------------------------------------------*//**
The function gets Printer Setup dialog settings from PlotCfg struct of PlotManager
and puts them to the application DEVMODE/DEVNAMES structs.
Code from MSDN Q166129 was used.

@author	Dmitry Gavrilov
@return 'true' on success, 'false' on failure
*//*--------------------------------------------------------------------------*/
bool CIcadApp::getPrinterSettingsFromPlotcfg()
{
	IIcadPlotManager*	pPlotMgr = ((CMainWindow*)m_pIcadMainWnd)->m_pIcadPrintDlg->GetPlotManager();

	if (!bSavePrinterSettingsAfterFirstCall)
	{

	#ifndef USE_DEFAULT_PRINTER_THROUGH_INTERFACE
		((PlotManager*)pPlotMgr)->getSystemDefaultPrinterSettings();
	#else
		pPlotMgr->GetDefaultPrinterSettings();
	#endif

	}


#ifdef USE_PLOTCFG_DIRECTLY
	CString	printerName(((PlotManager*)pPlotMgr)->m_plotConfig->m_printerName);
#else
	BSTR	bstrPrinterName;
	if(FAILED(pPlotMgr->get_PrinterName(&bstrPrinterName))
		return false;

	CString	printerName(bstrPrinterName);
	SysFreeString(bstrPrinterName);
#endif

	// open printer
    HANDLE	hPrinter;
    if(!OpenPrinter((LPTSTR)(LPCTSTR)printerName, &hPrinter, NULL))
        return false;

	// obtain PRINTER_INFO_2 structure
	DWORD	dwBytesReturned, dwBytesNeeded;
	GetPrinter(hPrinter, 2, NULL, 0, &dwBytesNeeded);
	PRINTER_INFO_2* pPI2 = (PRINTER_INFO_2*)GlobalAlloc(GPTR, dwBytesNeeded);
	if(!GetPrinter(hPrinter, 2, (LPBYTE)pPI2, dwBytesNeeded, &dwBytesReturned))
	{
		GlobalFree(pPI2);
		ClosePrinter(hPrinter);
		return false;
	}

	// Allocate a global handle for DEVMODE
	DWORD		size = DocumentProperties(NULL, hPrinter, (LPTSTR)(LPCTSTR)printerName, NULL, NULL, 0);
	if(size < sizeof(*pPI2->pDevMode) + pPI2->pDevMode->dmDriverExtra)
		size = sizeof(*pPI2->pDevMode) + pPI2->pDevMode->dmDriverExtra;
	HGLOBAL		hDevMode = GlobalAlloc(GHND, size);
	DEVMODE*	pDevMode = (DEVMODE*)GlobalLock(hDevMode);

	// copy DEVMODE data from PRINTER_INFO_2::pDevMode
	memcpy(pDevMode, pPI2->pDevMode, sizeof(*pPI2->pDevMode) + pPI2->pDevMode->dmDriverExtra);

	// copy data from PlotCfg
#ifdef USE_PLOTCFG_DIRECTLY
	if(((PlotManager*)pPlotMgr)->m_plotConfig->m_paperOrientation >= 0)
	{
		pDevMode->dmFields |= DM_ORIENTATION;
		pDevMode->dmOrientation = ((PlotManager*)pPlotMgr)->m_plotConfig->m_paperOrientation;
	}
	else
		pDevMode->dmFields &= ~DM_ORIENTATION;

	if(((PlotManager*)pPlotMgr)->m_plotConfig->m_paperSize >= 0)
	{
		pDevMode->dmFields |= DM_PAPERSIZE;
		pDevMode->dmPaperSize = ((PlotManager*)pPlotMgr)->m_plotConfig->m_paperSize;
	}
	else
	{
		pDevMode->dmFields &= ~DM_PAPERSIZE;
		pDevMode->dmPaperSize = DMPAPER_USER;
	}

	if(((PlotManager*)pPlotMgr)->m_plotConfig->m_paperLength >= 0)
	{
		pDevMode->dmFields |= DM_PAPERLENGTH;
		pDevMode->dmPaperLength = ((PlotManager*)pPlotMgr)->m_plotConfig->m_paperLength;
	}
	else
		pDevMode->dmFields &= ~DM_PAPERLENGTH;

	if(((PlotManager*)pPlotMgr)->m_plotConfig->m_paperWidth >= 0)
	{
		pDevMode->dmFields |= DM_PAPERWIDTH;
		pDevMode->dmPaperWidth = ((PlotManager*)pPlotMgr)->m_plotConfig->m_paperWidth;
	}
	else
		pDevMode->dmFields &= ~DM_PAPERWIDTH;

	if(((PlotManager*)pPlotMgr)->m_plotConfig->m_paperSource >= 0)
	{
		pDevMode->dmFields |= DM_DEFAULTSOURCE;
		pDevMode->dmDefaultSource = ((PlotManager*)pPlotMgr)->m_plotConfig->m_paperSource;
	}
	else
		pDevMode->dmFields &= ~DM_DEFAULTSOURCE;
#else
	// set flags which fields we really change - TO DO!
	pDevMode->dmFields |= DM_ORIENTATION;
	pDevMode->dmFields |= DM_PAPERSIZE;
	pDevMode->dmFields |= DM_PAPERLENGTH;
	pDevMode->dmFields |= DM_PAPERWIDTH;
	pDevMode->dmFields |= DM_DEFAULTSOURCE;

	Orientation	orientation;
	if(FAILED(result = pPlotMgr->get_PaperOrientation(&orientation))
	{
		ClosePrinter(hPrinter);
		GlobalUnlock(hDevMode);
		GlobalFree(pPI2);
		return false;
	}
	pDevMode->dmOrientation = orientation == Portrait ? DMORIENT_PORTRAIT : DMORIENT_LANDSCAPE;
	if(FAILED(result = pPlotMgr->get_PaperSize(&pDevMode->dmPaperSize))
	{
		ClosePrinter(hPrinter);
		GlobalUnlock(hDevMode);
		GlobalFree(pPI2);
		return false;
	}
	double	length, width;
	if(FAILED(result = pPlotMgr->GetPaperSize(&length, &width))
	{
		ClosePrinter(hPrinter);
		GlobalUnlock(hDevMode);
		GlobalFree(pPI2);
		return false;
	}
	pDevMode->dmPaperLength = length;
	pDevMode->dmPaperWidth = width;
	if(FAILED(result = pPlotMgr->get_PaperSource(&pDevMode->dmDefaultSource))
	{
		ClosePrinter(hPrinter);
		GlobalUnlock(hDevMode);
		GlobalFree(pPI2);
		return false;
	}
#endif // USE_PLOTCFG_DIRECTLY

	// Compute size of DEVNAMES structure from PRINTER_INFO_2's data
	DWORD	drvNameLen = lstrlen(pPI2->pDriverName) + 1,	// driver name
			ptrNameLen = lstrlen(pPI2->pPrinterName) + 1,	// printer name
			porNameLen = lstrlen(pPI2->pPortName) + 1;		// port name

	// Allocate a global handle big enough to hold DEVNAMES.
	HGLOBAL		hDevNames = GlobalAlloc(GHND, sizeof(DEVNAMES) + (drvNameLen + ptrNameLen + porNameLen) * sizeof(TCHAR));
	DEVNAMES*	pDevNames = (DEVNAMES*)GlobalLock(hDevNames);

	// Copy the DEVNAMES information from PRINTER_INFO_2
	// tcOffset = TCHAR Offset into structure
	int	tcOffset = sizeof(DEVNAMES) / sizeof(TCHAR);
	ASSERT(sizeof(DEVNAMES) == tcOffset * sizeof(TCHAR));

	pDevNames->wDriverOffset = tcOffset;
	memcpy((LPTSTR)pDevNames + tcOffset, pPI2->pDriverName, drvNameLen * sizeof(TCHAR));
	tcOffset += drvNameLen;

	pDevNames->wDeviceOffset = tcOffset;
	memcpy((LPTSTR)pDevNames + tcOffset, pPI2->pPrinterName, ptrNameLen * sizeof(TCHAR));
	tcOffset += ptrNameLen;

	pDevNames->wOutputOffset = tcOffset;
	memcpy((LPTSTR)pDevNames + tcOffset, pPI2->pPortName, porNameLen * sizeof(TCHAR));
	pDevNames->wDefault = 0;

	GlobalUnlock(hDevNames);
	GlobalFree(pPI2);

	// correct (merge) devmode values to meet our document properties
	DWORD	dwRet = DocumentProperties(NULL, hPrinter, (LPTSTR)(LPCTSTR)printerName, pDevMode, pDevMode, DM_IN_BUFFER | DM_OUT_BUFFER);

	ClosePrinter(hPrinter);
	GlobalUnlock(hDevMode);

	if(dwRet != IDOK)
	   return false;

	// select printer
	SelectPrinter(hDevNames, hDevMode);

	return true;
}

/*-------------------------------------------------------------------------*//**
The function gets Printer Setup dialog settings from the application's
DEVMODE/DEVNAMES structs and puts them to PlotCfg struct of PlotManager.

@author	Dmitry Gavrilov
@return 'true' on success, 'false' on failure
*//*--------------------------------------------------------------------------*/
bool CIcadApp::putPrinterSettingsToPlotcfg()
{
	DEVMODE*	pDevMode = (DEVMODE*)GlobalLock(m_hDevMode);
	if(pDevMode)
	{
		IIcadPlotManager*	pPlotMgr = ((CMainWindow*)m_pIcadMainWnd)->m_pIcadPrintDlg->GetPlotManager();
		if(pPlotMgr)
		{
#ifdef USE_PLOTCFG_DIRECTLY
			strcpy(((PlotManager*)pPlotMgr)->m_plotConfig->m_printerName, (char*)pDevMode->dmDeviceName);
			((PlotManager*)pPlotMgr)->m_plotConfig->m_paperOrientation	= pDevMode->dmFields & DM_ORIENTATION	? pDevMode->dmOrientation	: -1;
			((PlotManager*)pPlotMgr)->m_plotConfig->m_paperSize			= pDevMode->dmFields & DM_PAPERSIZE		? pDevMode->dmPaperSize		: -1;
			((PlotManager*)pPlotMgr)->m_plotConfig->m_paperLength		= pDevMode->dmFields & DM_PAPERLENGTH	? pDevMode->dmPaperLength	: -1;
			((PlotManager*)pPlotMgr)->m_plotConfig->m_paperWidth		= pDevMode->dmFields & DM_PAPERWIDTH	? pDevMode->dmPaperWidth	: -1;
			((PlotManager*)pPlotMgr)->m_plotConfig->m_paperSource		= pDevMode->dmFields & DM_DEFAULTSOURCE	? pDevMode->dmDefaultSource	: -1;
#else
			// TO DO
			ASSERT(!"implement IIcadPlotManager's props!");
#endif
		}
		::GlobalUnlock(m_hDevMode);
		return !!pPlotMgr;
	}
	return false;
}

#include "lisp.h"/*DNT*/
#include "CommandAtom.h"
HRESULT CIcadApp::RegisterFunction (const char *LocalName, void *funccall, const char *GlobalName, int id)
{
	struct resbuf rb;
	int err = RTNORM;
	rb.restype=LSP_RTXSUB;
	rb.resval.rstring=(char *)funccall;
	rb.rbnext = NULL;
	if(lsp_defatom(LocalName,id,&rb,0)!=0)
		err = RTERROR;

	class commandAtomObj *atom = NULL;
	atom = lsp_findatom(LocalName, 0);
	if (!atom)
		return E_FAIL;

	atom->SetFuncPtr ((int (__cdecl *)(void))funccall);
	atom->SetVbaFunction();

	return S_OK;
	}

HRESULT CIcadApp::UnRegisterFunction (const char *LocalName, const char *GlobalName, int id)
	{
	if (lsp_undefatom((char *)LocalName, 0) == 0)
		return S_OK;
	else
		return NOERROR;
	}

BOOL CIcadApp::IsFunctionRegistered (const char *FuncName)
{
	class commandAtomObj *atom = NULL;
	atom = lsp_findatom(FuncName, 0);

	if (atom)
		return TRUE;
	else
		return FALSE;
	}


BOOL CIcadApp::IsIcadChildWindow (CWnd *pWnd)
	{
		return (pWnd->IsKindOf(RUNTIME_CLASS(CIcadChildWnd)));
	}

HRESULT CIcadApp::GetLoadedApps (struct resbuf **ppApps)
	{
		*ppApps = sds_loaded();
		return NOERROR;
	}

HRESULT CIcadApp::LoadOrUnloadApp (CString AppName, BOOL bSDS, BOOL bLoad)
	{
	char	command[IC_ACADBUF];
	int n;

	if (bSDS)
		{
		if (bLoad)
			{
			sprintf(command, "(XLOAD \"%s\")"/*DNT*/, AppName);
			for(n = 0; command[n] != 0; n++)
				{
				if(command[n]=='\\'/*DNT*/)
					command[n]='/'/*DNT*/;
				//EBATECH(R.Arayashiki) -[ for MBCS String
				else if ((_MBC_LEAD ==_mbbtype((unsigned char)command[n],0)) &&
					  (_MBC_TRAIL==_mbbtype((unsigned char)command[n+1],1)))
					n ++;
				// ]- EBATECH
				}
			sds_printf( ResourceString(IDC_ICADAPPLOAD__NLOADING___14, "\nLoading %s" ), AppName);
			SDS_SendMessage(WM_MENUSELECT, 0, (LPARAM)command);
			SDS_CMainWindow->m_IgnoreLastCmd = true;
			}
		else
			{
			sprintf(command, "(XUNLOAD \"%s\")"/*DNT*/, AppName);
			for(n = 0; command[n] != 0; n++)
				{
				if(command[n]=='\\'/*DNT*/)
					command[n]='/'/*DNT*/;
				//EBATECH(R.Arayashiki) -[ for MBCS String
				else if ((_MBC_LEAD ==_mbbtype((unsigned char)command[n],0)) &&
					  (_MBC_TRAIL==_mbbtype((unsigned char)command[n+1],1)))
					n ++;
				// ]- EBATECH
				}
			sds_printf(ResourceString(IDC_ICADAPPLOAD__NUNLOADING__5, "\nUnloading %s" ),AppName);
			SDS_SendMessage(WM_MENUSELECT, 0, (LPARAM)command);
			SDS_CMainWindow->m_IgnoreLastCmd = true;
			}
		}
	else
		{
		sprintf(command,"(LOAD \"%s\")"/*DNT*/, AppName);
		for(n = 0; command[n] != 0; n++)
			{
			if(command[n]=='\\'/*DNT*/)
				command[n]='/'/*DNT*/;
			//EBATECH(R.Arayashiki) -[ for MBCS String
			else if ((_MBC_LEAD ==_mbbtype((unsigned char)command[n],0)) &&
				  (_MBC_TRAIL==_mbbtype((unsigned char)command[n+1],1)))
				n ++;
			// ]- EBATECH
			}
		sds_printf( ResourceString(IDC_ICADAPPLOAD__NLOADING___14, "\nLoading %s" ), AppName);
		SDS_SendMessage(WM_MENUSELECT, 0, (LPARAM)command);
		SDS_CMainWindow->m_IgnoreLastCmd=true;
		}

	return NOERROR;
	}


HRESULT CIcadApp::RunCommand (CString Command)
	{
	char	command[IC_ACADBUF];
	int n;

	sprintf(command,"%s"/*DNT*/, Command);

	for(n = 0; command[n] != 0; n++)
		{
		if(command[n]=='\\'/*DNT*/)
			command[n]='/'/*DNT*/;
		else if (command[n] == 13)	//vbCr, vbCrLf
			command[n] = ';';
		else if (command[n] == 10)	//vbLf
			command[n] = ';';
		//EBATECH(R.Arayashiki) -[ for MBCS String
		else if ((_MBC_LEAD ==_mbbtype((unsigned char)command[n],0)) &&
			  (_MBC_TRAIL==_mbbtype((unsigned char)command[n+1],1)))
			n ++;
			// ]- EBATECH
		}

	SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)(command));
//	SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)"^C^C^C"/*DNT*/);

	return NOERROR;
	}

HRESULT CIcadApp::RunHelp (CString HelpFile, CString HelpContext, LONG HelpCommand)
	{
	if (sds_help (HelpFile.GetBuffer(0), HelpContext.GetBuffer(0), HelpCommand) == RTNORM)
		return NOERROR;
	return E_FAIL;
	}

HRESULT CIcadApp::SDSSetVar (CIcadDoc *pDoc, const char *szSysVar, const struct sds_resbuf *prbVarInfo)
	{
	if (sdsengine_setvar( szSysVar, prbVarInfo ) == RTNORM)
		return NOERROR;
	else
		return E_FAIL;
	}

HRESULT CIcadApp::SDSGetVar (CIcadDoc *pDoc, const char *szSysVar, struct sds_resbuf *prbVarInfo)
	{
	if (sdsengine_getvar( szSysVar, prbVarInfo ) == RTNORM)
		return NOERROR;
	else
		return E_FAIL;
	}

// Modified CyberAge VSB 02/23/2001  [
// Reason: Mail for Sander dated 02/08/2001
HRESULT CIcadApp::SDSSetLispVar (CIcadDoc *pDoc, const char *szSysVar, struct sds_resbuf **prbVarInfo)
	{
	CString cmd(szSysVar);
	lsp_lispev(cmd,prbVarInfo,NULL,0);

	return NOERROR;
	}
HRESULT CIcadApp::SDSGetLispVar(CIcadDoc *pDoc, const char *szSysVar, struct sds_resbuf **prbVarInfo)
	{
	CString cmd(szSysVar);
	lsp_lispev(cmd,prbVarInfo,NULL,0);

	return NOERROR;

	}

// Modified CyberAge VSB 02/23/2001 ]
short cmd_OpenNewDrawing(char *fname);

HRESULT CIcadApp::AutoOpenFile (char* FileName, bool bReadOnly)
	{
	cmd_bOpenReadOnly = bReadOnly;	//set the global variable

	short ret = cmd_OpenNewDrawing(FileName);

	cmd_bOpenReadOnly = FALSE;

	if (ret == RTNORM)
		return NOERROR;
	else
		return E_FAIL;
	}

HRESULT CIcadApp::CloseAllWindows ()
	{
	short ret = cmd_wcloseall();

	if (ret == RTNORM)
		return NOERROR;
	else
		return E_FAIL;
	}

CString CIcadApp::GetRegistryRoot()
{
    CString SDS_RegEntry;
    VERIFY(getRegCurrentProfileRoot(SDS_RegEntry.GetBuffer(IC_ACADBUF), IC_ACADBUF));
	SDS_RegEntry.ReleaseBuffer();
	SDS_RegEntry += "\\Config";
	
	return SDS_RegEntry;
}

int CIcadApp::LoadLWDisplayScaleFactorFromRegistry()
{
    HKEY hKey;
    DWORD retType;
    unsigned long nStrLen;
    char tmpBuffer[IC_ACADBUF];

    CString SDS_RegEntry = GetRegistryRoot();

    if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_CURRENT_USER, SDS_RegEntry, 0,KEY_READ, &hKey)) 
	{
        return(RTERROR);
    }

    nStrLen = sizeof(tmpBuffer);
    if (RegQueryValueEx(hKey, "LineweightDisplayScale"/*DNT*/, NULL, &retType, (unsigned char*)tmpBuffer, &nStrLen) == ERROR_SUCCESS)
		m_nLWDisplayScaleFactor = atoi(tmpBuffer);

    RegCloseKey(hKey);

    return RTNORM;
}

int CIcadApp::SaveLWDisplayScaleFactorInRegistry()
{
    HKEY hKey;
    DWORD retval;
    char tmpBuffer[IC_ACADBUF];

    CString SDS_RegEntry = GetRegistryRoot();

    if(ERROR_SUCCESS != RegCreateKeyEx(HKEY_CURRENT_USER,
									   SDS_RegEntry,
									   0,
									   ""/*DNT*/,
									   REG_OPTION_NON_VOLATILE,
									   KEY_ALL_ACCESS,
									   NULL,
									   &hKey,
									   &retval)
		) 
	{
        return(RTERROR);
    }

	sprintf(tmpBuffer, "%d", (int)m_nLWDisplayScaleFactor);
    RegSetValueEx(hKey, "LineweightDisplayScale"/*DNT*/, 0, REG_SZ, (const unsigned char*)tmpBuffer, strlen(tmpBuffer) + 1);

    RegCloseKey(hKey);

    return RTNORM;
}







CCOMAddinManager & CIcadApp::comAddinManager ()
{
	static CCOMAddinManager theComAddinManager;
	return theComAddinManager;
}
// **********************************************************************
// DelayLoad support
//
//
#include "delayload.h" // from icadlib


FARPROC WINAPI MyDliNotifyHook( unsigned dliNotify, PDelayLoadInfo pdli )
	{
	FARPROC Result = NULL;

	if ( dliNotify == dliNotePreLoadLibrary )
		{
		// Passing NULL as the calling module assumes the current module (ICAD.EXE)
		//
		HMODULE hModule = FindDelayLoadModule( NULL, pdli->szDll );

		if ( hModule == NULL )
			{
			hModule = LoadLibrary( pdli->szDll );
			}


		Result = (FARPROC)hModule;
		}

	return Result;
	}

extern "C" PfnDliHook __pfnDliNotifyHook = MyDliNotifyHook;



