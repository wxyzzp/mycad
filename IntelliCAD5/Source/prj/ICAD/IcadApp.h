/* D:\ICADDEV\PRJ\ICAD\ICADAPP.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

#ifndef	_ICADAPP_H
#define	_ICADAPP_H

#pragma once

#include "IcadATL.h"
#include "IcadAPC.h"

#include "IcadCommandLineInfo.h"
class CCOMAddinManager;

//*** This structure contains the neccessary items for registering and
//*** unregistering our class factories with OLE.
typedef struct tagICCLASSFACTORY
{
	IClassFactory* pIClassFactory; //*** Class factory interface pointer.
	DWORD dwRegCO;				   //*** Registration key for CoRevokeClassObject().
} ICCLASSFACTORY, FAR* LPICCLASSFACTORY;

//*** Defines for print dialog bit sets.
#define PRINTDLG_DISPLAY		0x00000001L
#define PRINTDLG_PREVIEW		0x00000002L
#define PRINTDLG_PRINTING		0x00000004L
#define PRINTDLG_PRINTDIRECT	0x00000008L

// ** Defines
#define SDS_ESCAPECHAR  27
#define SDS_ENTERCHAR  13

class BHatchBuilderFactory;
// EBATECH(watanabe)-[update leader according to modifying annotation
class CUpdateLeaderFactory;
// ]-EBATECH(watanabe)

#include "IcadDoc.h"
#include "AutoApp.h"
#ifdef BUILD_WITH_VBA
#include "IcadVbaAddin.h"
#endif

class CLicensedComponents;
class CLmLicenseManager;

class CIcadApp : public CApcApplication<CIcadApp,CWinApp>,
				 public CApcHostAddIns<CIcadApp>
{

	friend class CAutoApp;

public:
	CIcadApp();
	virtual ~CIcadApp();

    BOOL InitInstance();
	BOOL OnIdle(LONG lCount){ return CWinApp::OnIdle( lCount ); }
	virtual int ExitInstance();
	COleTemplateServer m_server;
	CMDIFrameWnd* m_pIcadMainWnd;
	virtual void ShowAppWindows(void);
	void InitMRUList(UINT nMaxMRU);
	static HRESULT CIcadApp:: GetAuthenticator( struct IVisioAuthenticator* *authenticator);
	static bool CIcadApp:: SerialNumber( LPTSTR number, DWORD size);
	static bool CIcadApp:: IsTestDrive() {return !!isTestDrive;}
	
	bool CheckAuthentication(void);
	bool CheckLicense(void);

	void FilePrinterSetup(void);
	BOOL IcadPrintDlg(int nFlags);
	BOOL GetPrinterDC(CDC& dc);
	virtual void FileNew(void);
	virtual CIcadDoc* GetOpenFile(LPCTSTR pStr);
	virtual void FileOpen(LPCTSTR pStr);
	CString GetMRUString(int nIndex);
	void SetMRUSize(int nSize);
	void RemoveMRUString(int nIndex);
	void RegisterControls(void);
	void UnregisterControls(void);

	HRESULT RegisterMiniBitmap(BSTR bstrCoClass, UINT nID, LONG rgbBack);
	HRESULT ApcHost_OpenProject(BSTR bstrProjectFileName, IApcProject* pReferencingProject, IApcProject** ppProject);
	HRESULT ApcHost_OnCreate();
	void CloseWorkspace(BOOL bSave);
	void FireWorkspaceEvent(int id, CIcadDoc *pDoc);
	HRESULT InitSecurity();
	void Encrypt(AxSecurityLevel& nValue);
	void Decrypt(AxSecurityLevel& nValue);
	CComPtr<IIcadWorkspace> m_spWorkspace;
	BOOL m_bReadOnlyWorkspace;
	BOOL m_bDisableWorkspace;
	CString m_regpathBase;
	CString m_regpathSecurity;
//	CString m_regpathAddins; // not used (EBATECH)
	void GetWorkspaceSecurity (BOOL *bSec);
	void SetWorkspaceSecurity (BOOL bSec);

	BOOL MessageLoop(BOOL bPushedByHost);

	BOOL RegisterClassFactories();
	void RegisterServer();
	void UnregisterServer();
	BOOL HasInPlaceActiveItem();

	bool m_isRegistered;
	UINT m_uIcadClipFormat;

	int m_nLWDisplayScaleFactor;
	
	CString GetRegistryRoot();
	int LoadLWDisplayScaleFactorFromRegistry();
	int SaveLWDisplayScaleFactorInRegistry();


	// licensed components accessibility
	static CLicensedComponents	m_LicensedComponents;
	static CString				m_sApplicationTitle;
	static CLmLicenseManager	*m_pLicenseManager;
	static HINSTANCE			m_hLicenseManagerDLL;
	static const short			m_kVersion;

	bool						startLicenseManager();
	void						releaseLicenseManager();

	//*** Command line member
	CIcadCommandLineInfo m_cmdInfo;
	//*** Cursor manager
	class IcadCursor*	m_cursor;

	CComObject<CAutoApp>* m_pAutoApp;
	
	//Automation Helpers - these are functions that Automation calls so that Automation does not directly 
	//depend on cmds, sds etc.  
	virtual HRESULT InitPrintDialog ();
	virtual HRESULT RegisterFunction (const char *LocalName, void *funccall, const char *GlobalName, int id);
	virtual HRESULT UnRegisterFunction (const char *LocalName, const char *GlobalName, int id);
	virtual BOOL IsFunctionRegistered (const char *FuncName);
	virtual BOOL IsIcadChildWindow (CWnd *pWnd);
	virtual HRESULT GetLoadedApps (struct resbuf **ppApps);
	virtual HRESULT LoadOrUnloadApp (CString AppName, BOOL bSDS, BOOL bLoad);
	virtual HRESULT RunCommand (CString Command);
	virtual HRESULT RunHelp (CString HelpFile, CString HelpContext, LONG HelpCommand);
	virtual HRESULT SDSSetVar (CIcadDoc *pDoc, const char *szSysVar, const struct sds_resbuf *prbVarInfo);
	virtual HRESULT SDSGetVar (CIcadDoc *pDoc, const char *szSysVar, struct sds_resbuf *prbVarInfo);
	// Modified CyberAge VSB 02/23/2001 [
	// Reason: Mail for Sander dated 02/08/2001
	virtual HRESULT SDSSetLispVar (CIcadDoc *pDoc, const char *szSysVar, struct sds_resbuf **prbVarInfo);
	virtual HRESULT SDSGetLispVar (CIcadDoc *pDoc, const char *szSysVar, struct sds_resbuf **prbVarInfo);	
	// Modified CyberAge VSB 02/23/2001 ]
	virtual HRESULT AutoOpenFile (char* FileName, bool bReadOnly);
	virtual HRESULT CloseAllWindows ();

	virtual bool IsVBAAccessible();
	bool	getPrinterSettingsFromPlotcfg();
	bool	putPrinterSettingsToPlotcfg();
protected:
	virtual int Run();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	static BOOL isTestDrive;

private:
	void ShowTipAtStartup(void);
	void ShowTipOfTheDay(void);
private:
	BOOL m_bATLInited;
	DWORD m_dwRegCO;
	DWORD m_dwRegActive;

	bool bSavePrinterSettingsAfterFirstCall;

private:
	BOOL InitATL();
	BHatchBuilderFactory *m_hatchFactory;
	// EBATECH(watanabe)-[update leader according to modifying annotation
	CUpdateLeaderFactory *m_leaderFactory;
	// ]-EBATECH(watanabe)

protected:
// Generated message map functions
	//{{AFX_MSG(CIcadApp)
	afx_msg void OnFileNew();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
		CCOMAddinManager & comAddinManager ();

};


/////////////////////////////////////////////////////////////////////////////
inline CIcadApp* GetApp()
{
	CIcadApp* pApp = (CIcadApp*) AfxGetApp();
	ASSERT_VALID(pApp);
//	ASSERT_KINDOF(CIcadApp, pApp);
	return pApp;
}

#endif	// _ICADAPP_H


