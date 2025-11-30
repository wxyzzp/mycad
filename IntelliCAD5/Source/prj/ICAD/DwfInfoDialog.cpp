// DwfInfoDialog.cpp : implementation file
//

#include "stdafx.h"
#include "icad.h"
#include "DwfInfoDialog.h"
#include "resource.hm"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int DIST_FROM_EDGE_WIDTH = 28;
extern void CommandRefHelp( HWND window, DWORD context);

/////////////////////////////////////////////////////////////////////////////
// CDwfInfoDialog dialog

CDwfInfoDialog::CDwfInfoDialog(char* caption, 
							   char* versionCaption,
							   char* formatCaption,
							   char* ver1,
							   char* ver2,
							   char* ver3,
							   char* format1,
							   char* format2,
							   char* format3,
							   char* layoutCaption,
							   char* layoutCurrent,
						       char* layoutAll,
							   CWnd* pParent /*=NULL*/)
	: CDialog(CDwfInfoDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDwfInfoDialog)
	m_sCaption = _T("");
	m_sVersionGroupCaption = _T("");
	m_sFormatGroupCaption = _T("");
	m_sVersion1 = _T("");
	m_sVersion2 = _T("");
	m_sVersion3 = _T("");
	m_sFormat1 = _T("");
	m_sFormat2 = _T("");
	m_sFormat3 = _T("");
	m_sLayoutCaption = _T("");
	m_sLayoutCurrent = _T("");
	m_sLayoutAll = _T("");

	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32

	m_sCaption = caption;
	m_sVersionGroupCaption = versionCaption;
	m_sFormatGroupCaption = formatCaption;
	m_sVersion1 = ver1;
	m_sVersion2 = ver2;
	m_sVersion3 = ver3;
	m_sFormat1 = format1;
	m_sFormat2 = format2;
	m_sFormat3 = format3;
	m_sLayoutCaption =layoutCaption;
	m_sLayoutCurrent = layoutCurrent;
	m_sLayoutAll = layoutAll;

}

void CDwfInfoDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDwfInfoDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDwfInfoDialog, CDialog)
	//{{AFX_MSG_MAP(CDwfInfoDialog)
	ON_BN_CLICKED(IDC_DWF_VERSION_RADIO1, OnDwfVersionRadio1)
	ON_BN_CLICKED(IDC_DWF_VERSION_RADIO2, OnDwfVersionRadio2)
	ON_BN_CLICKED(IDC_DWF_VERSION_RADIO3, OnDwfVersionRadio3)

	ON_BN_CLICKED(IDC_DWF_LAYOUT_RADIO1, OnDwfLayoutRadio1)
	ON_BN_CLICKED(IDC_DWF_LAYOUT_RADIO2, OnDwfLayoutRadio2)
	ON_WM_HELPINFO()
    ON_BN_CLICKED(IDHELP,OnHelp)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDwfInfoDialog message handlers

BOOL CDwfInfoDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	SetWindowText(m_sCaption);

	// Setup help button
	CButton *helpWnd = (CButton*)GetDlgItem(IDHELP);	
	if (helpWnd)
		helpWnd->SetIcon(AfxGetApp()->LoadIcon(HELP_ICON));

	SetDlgItemText(IDC_DWF_VERSION_GROUP, m_sVersionGroupCaption);
	SetDlgItemText(IDC_DWF_FORMAT_GROUP, m_sFormatGroupCaption);
	SetDlgItemText(IDC_DWF_VERSION_RADIO1, m_sVersion1);
	SetDlgItemText(IDC_DWF_VERSION_RADIO2, m_sVersion2);
	SetDlgItemText(IDC_DWF_VERSION_RADIO3, m_sVersion3);
	SetDlgItemText(IDC_DWF_FORMAT_RADIO1, m_sFormat1);
	SetDlgItemText(IDC_DWF_FORMAT_RADIO2, m_sFormat2);
	SetDlgItemText(IDC_DWF_FORMAT_RADIO3, m_sFormat3);

	SetDlgItemText(IDC_DWF_LAYOUT_GROUP, m_sLayoutCaption);
	SetDlgItemText(IDC_DWF_LAYOUT_RADIO1, m_sLayoutCurrent);
	SetDlgItemText(IDC_DWF_LAYOUT_RADIO2, m_sLayoutAll);

	((CButton*)GetDlgItem(IDC_DWF_LAYOUT_RADIO1))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_DWF_VERSION_RADIO3))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_DWF_FORMAT_RADIO1))->SetCheck(1);

//	((CButton*)GetDlgItem(IDC_DWF_FORMAT_RADIO3))->EnableWindow(FALSE);	
//	((CButton*)GetDlgItem(IDC_DWF_LAYOUT_RADIO2))->EnableWindow(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDwfInfoDialog::OnOK() 
{
	// Version
	if (((CButton*)GetDlgItem(IDC_DWF_VERSION_RADIO1))->GetCheck())
		m_iVersionFormat = 0x1;
	else if (((CButton*)GetDlgItem(IDC_DWF_VERSION_RADIO2))->GetCheck()) 
		m_iVersionFormat = 0x2;
	else
		m_iVersionFormat = 0x4;

	// Format
	if (((CButton*)GetDlgItem(IDC_DWF_LAYOUT_RADIO2))->GetCheck())
	{
		m_iVersionFormat |= 0x40;		// Set export ALL layouts flag.
		m_iVersionFormat |= 0x20;		// Only ASCII can do this.
	}
	else
	{
		if (((CButton*)GetDlgItem(IDC_DWF_FORMAT_RADIO1))->GetCheck())
			m_iVersionFormat |= 0x8;
		else if (((CButton*)GetDlgItem(IDC_DWF_FORMAT_RADIO2))->GetCheck()) 
			m_iVersionFormat |= 0x10;
		else
			m_iVersionFormat |= 0x20;
	}


	CDialog::OnOK();
}

void CDwfInfoDialog::OnDwfVersionRadio1() 
{
	((CButton*)GetDlgItem(IDC_DWF_LAYOUT_RADIO1))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_DWF_LAYOUT_RADIO2))->SetCheck(0);

	((CButton*)GetDlgItem(IDC_DWF_LAYOUT_RADIO1))->EnableWindow(FALSE);	
	((CButton*)GetDlgItem(IDC_DWF_LAYOUT_RADIO2))->EnableWindow(FALSE);	

	return;
}

void CDwfInfoDialog::OnDwfVersionRadio2() 
{
	((CButton*)GetDlgItem(IDC_DWF_FORMAT_RADIO1))->EnableWindow(TRUE);	
	((CButton*)GetDlgItem(IDC_DWF_FORMAT_RADIO2))->EnableWindow(TRUE);	
	((CButton*)GetDlgItem(IDC_DWF_FORMAT_RADIO3))->EnableWindow(TRUE);	

	((CButton*)GetDlgItem(IDC_DWF_LAYOUT_RADIO1))->EnableWindow(TRUE);	
	((CButton*)GetDlgItem(IDC_DWF_LAYOUT_RADIO2))->EnableWindow(FALSE);	

	((CButton*)GetDlgItem(IDC_DWF_LAYOUT_RADIO1))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_DWF_LAYOUT_RADIO2))->SetCheck(0);
	return;
}

void CDwfInfoDialog::OnDwfVersionRadio3() 
{
	((CButton*)GetDlgItem(IDC_DWF_FORMAT_RADIO1))->EnableWindow(TRUE);	
	((CButton*)GetDlgItem(IDC_DWF_FORMAT_RADIO2))->EnableWindow(TRUE);	
	((CButton*)GetDlgItem(IDC_DWF_FORMAT_RADIO3))->EnableWindow(TRUE);	

	((CButton*)GetDlgItem(IDC_DWF_LAYOUT_RADIO1))->EnableWindow(TRUE);	
	((CButton*)GetDlgItem(IDC_DWF_LAYOUT_RADIO2))->EnableWindow(TRUE);	

	if (((CButton*)GetDlgItem(IDC_DWF_LAYOUT_RADIO2))->GetCheck())
	{
		((CButton*)GetDlgItem(IDC_DWF_FORMAT_RADIO3))->SetCheck(1);
		((CButton*)GetDlgItem(IDC_DWF_FORMAT_RADIO1))->EnableWindow(FALSE);	
		((CButton*)GetDlgItem(IDC_DWF_FORMAT_RADIO2))->EnableWindow(FALSE);	
	}
	return;
}

void CDwfInfoDialog::OnDwfLayoutRadio1() 
{
	((CButton*)GetDlgItem(IDC_DWF_FORMAT_RADIO1))->EnableWindow(TRUE);	
	((CButton*)GetDlgItem(IDC_DWF_FORMAT_RADIO2))->EnableWindow(TRUE);	
	((CButton*)GetDlgItem(IDC_DWF_FORMAT_RADIO3))->EnableWindow(TRUE);	

	return;
}

void CDwfInfoDialog::OnDwfLayoutRadio2() 
{
	((CButton*)GetDlgItem(IDC_DWF_FORMAT_RADIO1))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_DWF_FORMAT_RADIO2))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_DWF_FORMAT_RADIO3))->SetCheck(1);
	
	((CButton*)GetDlgItem(IDC_DWF_FORMAT_RADIO1))->EnableWindow(FALSE);	
	((CButton*)GetDlgItem(IDC_DWF_FORMAT_RADIO2))->EnableWindow(FALSE);	
	return;
}

void CDwfInfoDialog::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	CommandRefHelp( m_hWnd, HLP_DWFOUT);
	return;
}
void CDwfInfoDialog::OnHelp() {
	CommandRefHelp( m_hWnd, HLP_DWFOUT);
	return;
}



