// DialogHyperlink.cpp : implementation file
//

#include "stdafx.h"
#include "icad.h"
#include "DialogHyperlink.h"
#include "IcadDoc.h"
#include "cmdsInet.h"
#include "resource.hm"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern void CommandRefHelp( HWND window, DWORD context);

/////////////////////////////////////////////////////////////////////////////
// CDialogHyperlink dialog


CDialogHyperlink::CDialogHyperlink(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogHyperlink::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogHyperlink)
	m_link = _T("");
	m_loc = _T("");
	m_desc = _T("");
	m_rel = 0;
	//}}AFX_DATA_INIT
	m_entities[0] = 0;
	m_entities[1] = 0;
}

CDialogHyperlink::CDialogHyperlink(CWnd* pParent, sds_name entities, bool bApplyLater /* = false */)
	: CDialog(CDialogHyperlink::IDD, pParent)
{
	m_link = _T("");
	m_loc = _T("");
	m_desc = _T("");
	m_rel = 0;
	setEntities(entities);
	m_bApplyLater = bApplyLater;
}

CDialogHyperlink::~CDialogHyperlink()
{
	if(m_entities[0] != 0 || m_entities[1] != 0)
		sds_ssfree(m_entities);
}

void CDialogHyperlink::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogHyperlink)
	DDX_Text(pDX, IDC_EDIT_HLURL, m_link);
	DDX_Text(pDX, IDC_EDIT_HLLOC, m_loc);
	DDX_Text(pDX, IDC_EDIT_HLDESC, m_desc);
	DDX_Check(pDX, IDC_CHK_HLRELATIVE, m_rel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogHyperlink, CDialog)
	//{{AFX_MSG_MAP(CDialogHyperlink)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_BN_CLICKED(IDC_BTN_HLBROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_BTN_HLREMOVE, OnRemove)
	ON_BN_CLICKED(IDC_CHK_HLRELATIVE, OnRelative)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int CDialogHyperlink::setEntities(sds_name entities)
{
	if(m_entities[0] != 0 || m_entities[1] != 0)
		sds_ssfree(m_entities);

	if(entities != NULL)
	{
		long i;
		sds_name entity;
		sds_sslength(entities, &i);
		if(i > 0)
			while(--i >= 0)
			{
				sds_ssname(entities, i, entity);
				sds_ssadd(entity, m_entities, m_entities);
			}
		
		char link[IC_ACADBUF] = "";
		char loc[IC_ACADBUF] = "";
		char desc[IC_ACADBUF] = "";
		m_rel = 0;
		if(cmd_getEntitiesHL(m_entities, link, desc, loc))
		{
			m_link = link;
			m_desc = desc;
			m_loc = loc;
			if(cmd_isRelativeHyperlink(link))
				m_rel = 1;
		}
	}
	else
		memset(m_entities, 0, sizeof(sds_name));
	return 1;
}

void CDialogHyperlink::modify()
{
	char hlbase[IC_ACADBUF];
	cmd_getHyperlinkBase(hlbase);
	if(m_rel == 1)
	{
		// try to remove base if any
		if(m_link.Find(hlbase) == 0)
			m_link.Delete(0, strlen(hlbase));
	}
	else
		if(cmd_isRelativeHyperlink(m_link) && m_link.Find(hlbase) != 0)
		{
			// if not relative then add base as in ACAD
			CString relLink(m_link);
			m_link.Format("%s/%s", hlbase, relLink);
		}

	long i;
	sds_name entity;
	sds_sslength(m_entities, &i);
	while(--i >= 0)
	{
		sds_ssname(m_entities, i, entity);
		cmd_modifyEntityHL(entity, (LPCTSTR)m_link, (LPCTSTR)m_desc, (LPCTSTR)m_loc);
	}
}

void CDialogHyperlink::remove()
{
	long i;
	sds_name entity;
	sds_sslength(m_entities, &i);
	while(--i >= 0)
	{
		sds_ssname(m_entities, i, entity);
		cmd_removeEntityHL(entity);
	}
}
/////////////////////////////////////////////////////////////////////////////
// CDialogHyperlink message handlers

BOOL CDialogHyperlink::OnInitDialog()
{
	CDialog::OnInitDialog();

	CButton *pHelpButton = (CButton*)GetDlgItem(IDHELP);
	if(pHelpButton)
		pHelpButton->SetIcon(AfxGetApp()->LoadIcon(HELP_ICON));

	char hlbase[IC_ACADBUF];
	cmd_getHyperlinkBase(hlbase);
	CString outString;
	outString.Format("%s: %s", ResourceString(IDC_HYPERLINK_BASE, "Hyperlink base"), hlbase);
	GetDlgItem(IDC_STXT_HLBASE)->SetWindowText(outString);

	UpdateData(FALSE);
	return TRUE;
}

void CDialogHyperlink::OnApply()
{
	if(m_action == eACTION_MODIFY)
		modify();
	else
		if(m_action == eACTION_REMOVE)
			remove();
		else
			ASSERT(FALSE);
}

void CDialogHyperlink::OnOK()
{
	UpdateData();
	m_action = eACTION_MODIFY;
	if(!m_bApplyLater)
		OnApply();

	CDialog::OnOK();
}

void CDialogHyperlink::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return;
}

void CDialogHyperlink::OnHelp() 
{
	CommandRefHelp( m_hWnd, HLP_PROP_HYPERLINK);
	return;
	
}

void CDialogHyperlink::OnBrowse() 
{
	UpdateData();
	resbuf path;

	// EBATECH(CNBR) 2002/4/30 Disable TYPE button option+2
	if(SDS_GetFiled(ResourceString(IDC_HYPERLINK_SELECT, "Select hyperlink"), NULL,
		ResourceString(IDC_HYPERLINK_SELECTEXT, "Any file|*.*"),
		2, &path, NULL, NULL) != RTNORM)
		return;

	m_link = path.resval.rstring;	
	IC_FREE(path.resval.rstring);
	m_rel = 0;
	UpdateData(FALSE);
}

void CDialogHyperlink::OnRemove() 
{
	m_link.Empty();
	m_desc.Empty();
	m_loc.Empty();
	m_rel = 0;

	m_action = eACTION_REMOVE;
	if(!m_bApplyLater)
	{
		OnApply();
		m_bApplyLater = true;
	}
	EndDialog(IDOK);	
}

void CDialogHyperlink::OnRelative() 
{
	// TODO: Add your control notification handler code here

}

