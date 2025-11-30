// File: DlgSpoolingSettings.cpp
//

#include "stdafx.h"
#include "icadres.h"
#include "DlgSpoolingSettings.h"
#include "Preferences.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


/*DG - 2.12.2002*/// Comment out the following defining if you don't want to use icad's spooling features
// (e.g. print to PRN file directly w/o interaction with user, store output printing files to a particular folder).
// NOTE: Keep in sync USE_ICAD_SPOOLING definings in dlgspoolingsettings.cpp, icadview.h and preferences.h files!
#define USE_ICAD_SPOOLING

#ifdef USE_ICAD_SPOOLING

CDlgSpoolingSettings::CDlgSpoolingSettings(CWnd* pParent /*=NULL*/)	: CDialog(CDlgSpoolingSettings::IDD, pParent)
{
	m_bForcePrintToFile = UserPreference::s_bForcePrintToFile;
	m_bUseOutputFolder = UserPreference::s_bUseOutputFolder;
	m_outputFolder = UserPreference::s_outputFolder;
}


void CDlgSpoolingSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK1, m_bForcePrintToFile);
	DDX_Check(pDX, IDC_CHECK2, m_bUseOutputFolder);
	DDX_Text(pDX, IDC_EDIT1, m_outputFolder);

	CWnd*	pDlgItem;
	if(pDlgItem = GetDlgItem(IDC_EDIT1))
		pDlgItem->EnableWindow(m_bUseOutputFolder);
	if(pDlgItem = GetDlgItem(IDC_SPOOLING))
		pDlgItem->EnableWindow(m_bUseOutputFolder);
	if(pDlgItem = GetDlgItem(IDC_BUTTON1))
		pDlgItem->EnableWindow(m_bUseOutputFolder);
}


BEGIN_MESSAGE_MAP(CDlgSpoolingSettings, CDialog)
	//{{AFX_MSG_MAP(CDlgSpoolingSettings)
	ON_BN_CLICKED(IDC_BUTTON1, OnBrowse)
	ON_BN_CLICKED(IDC_CHECK1, OnForcePrintToFile)
	ON_BN_CLICKED(IDC_CHECK2, OnUsePath)
	ON_EN_CHANGE(IDC_EDIT1, OnChangePath)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CDlgSpoolingSettings::OnBrowse() 
{
	LPMALLOC		pMalloc;
	SHGetMalloc(&pMalloc);

	char			buffer[MAX_PATH];
	LPITEMIDLIST	pidlRoot;
	BROWSEINFO		bi;

	bi.hwndOwner = this->m_hWnd;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = buffer;
	bi.lpszTitle = ""/*DNT*/;
	bi.ulFlags = 0;
	bi.lpfn = NULL;

	if(!(pidlRoot = SHBrowseForFolder(&bi)))
		return;

	if(SHGetPathFromIDList(pidlRoot, buffer))
	{
		m_outputFolder = buffer;
		UpdateData(FALSE);
	}
		
	pMalloc->Free(pidlRoot);
}

void CDlgSpoolingSettings::OnForcePrintToFile() 
{
	UpdateData();
}

void CDlgSpoolingSettings::OnUsePath() 
{
	UpdateData();
}

void CDlgSpoolingSettings::OnChangePath() 
{
	UpdateData();
}

#endif // USE_ICAD_SPOOLING
