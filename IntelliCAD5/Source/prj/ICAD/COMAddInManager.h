#ifndef _COMAddInManager_H
#define _COMAddInManager_H

#ifndef _IcadVbaWorkspace_H
#include "IcadVbaWorkspace.h"
#endif

class CAutoDoc;
class CIcadApp;
class CCOMAddin;

// keeps all information about an add-in
class CCOMAddinStatus
{
public:
    CString m_description;
    CString m_friendlyName;
    bool      m_bLoaded;     
    CString m_fullDllName;   // including disk, path, name and extension
	long m_commandLineSafe;
    long      m_loadBehavior;
    bool      m_bWillBeLoaded; 

    CCOMAddinStatus();
    CCOMAddinStatus (const CCOMAddinStatus& status);
    CCOMAddinStatus& operator=( const CCOMAddinStatus& status );
};

class CCOMAddinManager
{
public:
	CCOMAddinManager();
	~CCOMAddinManager();

	HRESULT load (CIcadApp* icad);
	HRESULT unload (void);

	void showDialog ();

								// for CCOMAddinManagerDlg
	int		getAddInsStatus(CArray<CCOMAddinStatus,CCOMAddinStatus&> &addins);
	void	correctAddInsStatus(CArray<CCOMAddinStatus,CCOMAddinStatus&> &addins);

								// ICADWorkspaceEvents
	HRESULT fireWorkspaceStartup(IIcadWorkspace* icad_ws);
	HRESULT fireWorkspaceDocumentEvent(IIcadWorkspace* icad_ws, int id,
									   IIcadDocument* doc);
	HRESULT fireWorkspaceBeforeExit(IIcadWorkspace* icad_ws);
	HRESULT fireWorkspaceExit(IIcadWorkspace* icad_ws);
	HRESULT fireAddinRun(IIcadWorkspace* icad_ws, const char* action);
	HRESULT fireWorkspaceReinit(IIcadWorkspace* icad_ws);

								// _EIcadDocument
	HRESULT fireDocumentEvent(int id, CAutoDoc* doc);
private:
	IDispatch *m_pIcadApp;
	CArray<CCOMAddin *,CCOMAddin*&> m_addins;

	void loadAddin (LPTSTR lpName,HKEY hRegKeyParent);
	int findByName(LPCTSTR name) const;
};

#endif //#define _COMAddInManager_H
