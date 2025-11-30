// IcTestApp.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ItsApp.h"
#include "ItsMainFrm.h"
#include "ItsDoc.h"
#include "ItsTestTree.h"
#include "ItsResultView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CItsApp::CItsApp()
{}
CItsApp theApp;

BOOL CItsApp::InitInstance()
{
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CItsDoc),
		RUNTIME_CLASS(CItsMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CItsResultView));
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);
	m_pMainWnd->UpdateWindow();

	return TRUE;
}

