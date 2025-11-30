#include "stdafx.h"
#include "Icad.h"
#include "DialogGroupHLight.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString CDialogGroupHLight::s_outString;

CDialogGroupHLight::CDialogGroupHLight(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogGroupHLight::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogGroupHLight)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CDialogGroupHLight::CDialogGroupHLight(CWnd* pParent, sds_name entities)
	: CDialog(CDialogGroupHLight::IDD, pParent)
{
	memcpy(m_entities, entities, sizeof(sds_name));

	sds_sslength(m_entities, &m_count);
	ASSERT(m_count != 0);

	m_index = 0;
	sds_ssname(m_entities, m_index, m_currentEntity);
}

void CDialogGroupHLight::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogGroupHLight)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogGroupHLight, CDialog)
	//{{AFX_MSG_MAP(CDialogGroupHLight)
	ON_BN_CLICKED(GROUP_STEP_NEXT, OnNext)
	ON_BN_CLICKED(GROUP_STEP_PREVIOUS, OnPrevious)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogGroupHLight message handlers
BOOL CDialogGroupHLight::OnInitDialog()
{
	CDialog::OnInitDialog();

	CButton *pHelpButton = (CButton*)GetDlgItem(IDHELP);
	if(pHelpButton)
		pHelpButton->SetIcon(AfxGetApp()->LoadIcon(HELP_ICON));
	
	m_pOutWin = (CStatic*)GetDlgItem(IDC_STXT_ENT);

	s_outString.Format("%s: %d", ResourceString(IDC_GROUP_ENTITY, "Entity"), m_index);
	m_pOutWin->SetWindowText((LPCTSTR)s_outString);

	sds_redraw(m_currentEntity, IC_REDRAW_HILITE);
	return TRUE;
}

void CDialogGroupHLight::OnHelp() 
{
	// TODO: Add your control notification handler code here
	
}

void CDialogGroupHLight::OnNext() 
{
	if(m_index + 1 < m_count)
		++m_index;
	changeCurrentEntity();
}

void CDialogGroupHLight::OnPrevious() 
{
	if(m_index - 1 >= 0)
		--m_index;
	changeCurrentEntity();
}

void CDialogGroupHLight::changeCurrentEntity()
{
	sds_redraw(m_currentEntity, IC_REDRAW_UNHILITE);
	sds_ssname(m_entities, m_index, m_currentEntity);
	sds_redraw(m_currentEntity, IC_REDRAW_HILITE);

	s_outString.Format("%s: %d", ResourceString(IDC_GROUP_ENTITY, "Entity"), m_index);
	m_pOutWin->SetWindowText((LPCTSTR)s_outString);
}
