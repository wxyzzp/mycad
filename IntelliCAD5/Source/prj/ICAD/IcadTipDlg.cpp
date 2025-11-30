/* F:\BLD\PRJ\ICAD\ICADTIPDLG.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "Icad.h"/*DNT*/
#include "IcadTipDlg.h"/*DNT*/
#include <winreg.h>
#include "win32misc.h"

#ifdef _DEBUG

	#if !defined(USE_SMARTHEAP)
	#define new DEBUG_NEW
	#endif

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTipDlg dialog

#define MAX_BUFLEN 1000

static const TCHAR szSection[] = _T("Tip"/*DNT*/);
static const TCHAR szIntFilePos[] = _T("FilePos"/*DNT*/);
static const TCHAR szTimeStamp[] = _T("TimeStamp"/*DNT*/);
static const TCHAR szIntStartup[] = _T("StartUp"/*DNT*/);

CTipDlg::CTipDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_TIP, pParent)
{
	char fs1[IC_ACADBUF];
	//{{AFX_DATA_INIT(CTipDlg)
	m_bStartup = TRUE;
	//}}AFX_DATA_INIT

	// Now try to open the tips file
	m_pStream=NULL;
	if(sds_findfile("icad.tip"/*DNT*/,fs1)!=RTNORM || (m_pStream=fopen(fs1, ResourceString(IDC_ICADTIPDLG_R_5, "r" ))) == NULL) 
	{
		LOADSTRINGS_3(CG_IDS_FILE_ABSENT)
		m_strTip = LOADEDSTRING;
		return;
	} 

	// We need to find out what the startup and file position parameters are
	// If startup does not exist, we assume that the Tips on startup is checked TRUE.
	CWinApp* pApp = AfxGetApp();

	CString prefixProfileName = ResourceString(IDS_REGISTRY_PROFILES, "Profiles");
	CString profName;
	GetCurrentRegistryProfileName(profName.GetBuffer(IC_ACADBUF), IC_ACADBUF);
	profName.ReleaseBuffer();
	prefixProfileName += _T("\\") + profName;

    m_pSaved_ProfileName = pApp->m_pszProfileName;
	pApp->m_pszProfileName = strdup(prefixProfileName); 

	m_bStartup = !pApp->GetProfileInt(szSection, szIntStartup, 0);
	UINT iFilePos = pApp->GetProfileInt(szSection, szIntFilePos, 0);

	// If the timestamp in the INI file is different from the timestamp of
	// the tips file, then we know that the tips file has been modified
	// Reset the file position to 0 and write the latest timestamp to the
	// ini file
	struct _stat buf;
	_fstat(_fileno(m_pStream), &buf);
	CString strCurrentTime = ctime(&buf.st_ctime);
	strCurrentTime.TrimRight();
	CString strStoredTime = pApp->GetProfileString(szSection, szTimeStamp, NULL);
	if (strCurrentTime != strStoredTime) 
	{
		iFilePos = 0;
		pApp->WriteProfileString(szSection, szTimeStamp, strCurrentTime);
	}

	if (fseek(m_pStream, iFilePos, SEEK_SET) != 0) 
	{
		LOADSTRINGS_3(CG_IDP_FILE_CORRUPT)
		AfxMessageBox(LOADEDSTRING);
	}
	else 
	{
		GetNextTipString(m_strTip);
	}
}

CTipDlg::~CTipDlg()
{
	// This destructor is executed whether the user had pressed the escape key
	// or clicked on the close button. If the user had pressed the escape key,
	// it is still required to update the filepos in the ini file with the 
	// latest position so that we don't repeat the tips! 
    
	// But make sure the tips file existed in the first place....
	if (m_pStream != NULL) 
	{
		CWinApp* pApp = AfxGetApp();
		pApp->WriteProfileInt(szSection, szIntFilePos, ftell(m_pStream));
		fclose(m_pStream);
		//!!!! This string comes from strdup -- has to be free'ed not delete'ed
		free((void*)pApp->m_pszProfileName);
		pApp->m_pszProfileName = m_pSaved_ProfileName;
	}

}
        
void CTipDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTipDlg)
	DDX_Check(pDX, IDC_STARTUP, m_bStartup);	// This reads the box OnOK, but not OnNextTip
	DDX_Text(pDX, IDC_TIPSTRING, m_strTip);			// Note:  In OnOK, UpdateData is TRUE, in NextTip it's FALSE
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTipDlg, CDialog)
	//{{AFX_MSG_MAP(CTipDlg)
	ON_BN_CLICKED(IDC_NEXTTIP, OnNextTip)
	ON_WM_CTLCOLOR()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTipDlg message handlers

void CTipDlg::OnNextTip()
{
	UpdateData(TRUE);
	GetNextTipString(m_strTip);
	UpdateData(FALSE);							// If this is TRUE, tip doesn't increment.
}

void CTipDlg::GetNextTipString(CString& strNext)
{
	LPTSTR lpsz = strNext.GetBuffer(MAX_BUFLEN);

	// This routine identifies the next string that needs to be
	// read from the tips file
	BOOL bStop = FALSE;
	while (!bStop) 
	{
		if (_fgetts(lpsz, MAX_BUFLEN, m_pStream) == NULL) 
		{
			// We have either reached EOF or enocuntered some problem
			// In both cases reset the pointer to the beginning of the file
			// This behavior is same as VC++ Tips file
			if (fseek(m_pStream, 0, SEEK_SET) != 0)
			{
				LOADSTRINGS_3(CG_IDP_FILE_CORRUPT)
				AfxMessageBox(LOADEDSTRING);
			}
		} 
		else 
		{
			if (*lpsz != ' ' && *lpsz != '\t' && 
				*lpsz != '\n' && *lpsz != ';') 
			{
				// There should be no space at the beginning of the tip
				// This behavior is same as VC++ Tips file
				// Comment lines are ignored and they start with a semicolon
				bStop = TRUE;
			}
		}
	}
	int sl=strlen(lpsz); 
	// EBATECH(CNBR) -[ MBCS String
	//for(int fi1=0; fi1<sl; ++fi1)
	//	if(lpsz[fi1]=='\\') lpsz[fi1]='\n';
	for(int fi1=0; fi1<sl; ++fi1){
		if(lpsz[fi1]=='\\'){
			lpsz[fi1]='\n';
		} else if((_MBC_LEAD ==_mbbtype((unsigned char)lpsz[fi1],0)) &&
		 (_MBC_TRAIL==_mbbtype((unsigned char)lpsz[fi1+1],1))){
			++fi1;
		}
	}
	// EBATECH(CNBR) ]-
	strNext.ReleaseBuffer();
}

HBRUSH CTipDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if (pWnd->GetDlgCtrlID() == IDC_TIPSTRING)
		return (HBRUSH)GetStockObject(WHITE_BRUSH);

	return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
}

void CTipDlg::OnOK()
{
	CDialog::OnOK();
	
    // Update the startup information stored in the INI file
	
	CWinApp* pApp = AfxGetApp();	
	pApp->WriteProfileInt(szSection, szIntStartup, !m_bStartup);
}

BOOL CTipDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// If Tips file does not exist then disable NextTip
	if (m_pStream == NULL)
		GetDlgItem(IDC_NEXTTIP)->EnableWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CTipDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	// Get paint area for the big static control
	CWnd* pStatic = GetDlgItem(IDC_BULB);
	CRect rect;
	pStatic->GetWindowRect(&rect);
	ScreenToClient(&rect);

	// Paint the background white.
	CBrush brush;
	brush.CreateStockObject(WHITE_BRUSH);
	dc.FillRect(rect, &brush);

	// Load bitmap and get dimensions of the bitmap
	CBitmap bmp;
	bmp.LoadBitmap(IDB_LIGHTBULB);
	BITMAP bmpInfo;
	bmp.GetBitmap(&bmpInfo);

	// Draw bitmap in top corner and validate only top portion of window
	CDC dcTmp;
	dcTmp.CreateCompatibleDC(&dc);
	dcTmp.SelectObject(&bmp);
	rect.bottom = bmpInfo.bmHeight + rect.top;
	dc.BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), 
		&dcTmp, 0, 0, SRCCOPY);

	// Draw out "Did you know..." message next to the bitmap
	CString strMessage;
	strMessage.LoadString(CG_IDS_DIDYOUKNOW);
	rect.left += bmpInfo.bmWidth;
	dc.DrawText(strMessage, rect, DT_VCENTER | DT_SINGLELINE);

	// Do not call CDialog::OnPaint() for painting messages
}



