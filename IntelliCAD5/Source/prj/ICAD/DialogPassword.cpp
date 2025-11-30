// DialogPassword.cpp : implementation file
//

#include "stdafx.h"
#include "icad.h"
#include "DialogPassword.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPasswordDlg dialog

CPasswordDlg::CPasswordDlg(char* caption, CWnd* pParent /*=NULL*/)
	: CDialog(CPasswordDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPasswordDlg)
	m_sPassword = _T("");
	m_sFileName = _T("");
	m_sMessage = _T("");
	m_sCaution = _T("");
	//}}AFX_DATA_INIT

	m_Caption = caption;
}

void CPasswordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPasswordDlg)
	DDX_Text(pDX, IDS_PASSWORD_EDIT, m_sPassword);
	DDX_Text(pDX, IDS_PASSWORD_FILENAME, m_sFileName);
	DDX_Text(pDX, IDS_PASSWORD_MESSAGE, m_sMessage);
	DDX_Text(pDX, IDS_PASSWORD_CAUTION, m_sCaution);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPasswordDlg, CDialog)
	//{{AFX_MSG_MAP(CPasswordDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPasswordDlg message handlers

BOOL CPasswordDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set caption from resource string.
	SetWindowText(m_Caption);
	CStatic* tmp = (CStatic*)GetDlgItem(IDS_PASSWORD_FILENAME);
	tmp->SetWindowText(m_sFileName);
	tmp = (CStatic*)GetDlgItem(IDS_PASSWORD_MESSAGE);
	tmp->SetWindowText(m_sMessage);
	tmp = (CStatic*)GetDlgItem(IDS_PASSWORD_CAUTION);
	tmp->SetWindowText(m_sCaution);

	return TRUE;  
}
