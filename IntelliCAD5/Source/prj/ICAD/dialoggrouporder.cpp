#include "stdafx.h"
#include "Icad.h"
#include "DialogGroupOrder.h"
#include "drawing.h"
#include "objects.h"
#include "DbGroup.h"
#include "cmdsGroup.h"
#include "resource.hm"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern void CommandRefHelp( HWND window, DWORD context);

CString CDialogGroupOrder::s_outString;

CDialogGroupOrder::CDialogGroupOrder(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogGroupOrder::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogGroupOrder)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CDialogGroupOrder::CDialogGroupOrder(CWnd* pParent, db_drawing* pDrawing, sds_name showGroups)
	: CDialog(CDialogGroupOrder::IDD, pParent), m_pDrawing(pDrawing), m_bDidReorder(false)
{
	m_pDictGroup = (db_dictionary*)m_pDrawing->dictsearch(m_pDrawing->namedobjdict(),
		"ACAD_GROUP", 0);
	ASSERT(m_pDictGroup != NULL);

	if(showGroups == NULL)
		memset(m_showGroups, 0, sizeof(sds_name));
	else
		memcpy(m_showGroups, showGroups, sizeof(sds_name));
}

void CDialogGroupOrder::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogGroupOrder)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogGroupOrder, CDialog)
	//{{AFX_MSG_MAP(CDialogGroupOrder)
	ON_BN_CLICKED(IDC_BTN_HLIGHT, OnHighlight)
	ON_BN_CLICKED(IDC_BTN_REORDER, OnReorder)
	ON_BN_CLICKED(IDC_BTN_REVERSE, OnReverseOrder)
	ON_LBN_SELCHANGE(IDC_LBOX_GROUPS, OnSelchange)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogGroupOrder message handlers
BOOL CDialogGroupOrder::OnInitDialog()
{
	CDialog::OnInitDialog();

	CButton *pHelpButton = (CButton*)GetDlgItem(IDHELP);
	if(pHelpButton)
		pHelpButton->SetIcon(AfxGetApp()->LoadIcon(HELP_ICON));

	if(m_pDrawing == NULL)
		return FALSE;

	m_pListGroup = (CListBox*)GetDlgItem(IDC_LBOX_GROUPS);

	db_dictionaryrec* pFirstRec;
	m_pDictGroup->get_recllends(&pFirstRec, NULL);
	if(m_showGroups[0] != 0 && m_showGroups[1] != 0)
	{
		db_dictionaryrec* pCurRec;
		// hide unnecessary controls
		GetDlgItem(IDOK)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_GBOX_REORDER)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_STXT_REMOVE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STXT_PLACE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STXT_NENT)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_EDIT_OLDPOS)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_NEWPOS)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_NUM)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_BTN_REORDER)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTN_REVERSE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTN_HLIGHT)->ShowWindow(SW_HIDE);

		SetWindowText(ResourceString(IDC_GROUP_FIND, "Find Groups By Entity"));
		RECT r;
		GetClientRect(&r);

		r.left += 10;
		r.right -= 10;
		r.top += 40;
		r.bottom -= 60;
		m_pListGroup->MoveWindow(&r);
		// fill list with given groups
		long l;
		sds_name group;
		sds_sslength(m_showGroups, &l);
		for(int i = 0; i < l; ++i)
		{
			sds_ssname(m_showGroups, i, group);
			pCurRec = pFirstRec;
			while(pCurRec != NULL)
			{
				if(pCurRec->hiref->hand == ((CDbGroup*)group[0])->RetHandle())
				{
					m_pListGroup->AddString(pCurRec->name);
					break;
				}
				pCurRec = pCurRec->next;
			}
		}
		return TRUE;
	}

	int index = 0;
	CDbGroup* pGroup;
	while(pFirstRec != NULL)
	{
		pGroup = (CDbGroup*)pFirstRec->hiref->ret_hip(m_pDrawing);
		if(!pGroup->ret_deleted())
		{
			m_pListGroup->SetItemData(m_pListGroup->AddString(pFirstRec->name), index);
		}
		++index;
		pFirstRec = pFirstRec->next;
	}

	m_pCurrentGroup = NULL;
	return TRUE;
}

BOOL CDialogGroupOrder::OnHelpInfo(HELPINFO *pHel){
	OnHelp();
	return(true);
}

void CDialogGroupOrder::OnHelp() 
{
	// TODO: Add your control notification handler code here
	CommandRefHelp( m_hWnd, HLP_GROUP_REORDER);
	
}

int CDialogGroupOrder::getCurrentGroup(sds_name sdsGroupName) const
{
	if(m_pCurrentGroup == NULL)
	{
		sdsGroupName[0] = 0;
		sdsGroupName[0] = 0;
		return 0;
	}

	sdsGroupName[0] = (long)m_pCurrentGroup;
	sdsGroupName[1] = (long)m_pDrawing;
	return 1;
}

void CDialogGroupOrder::OnHighlight() 
{
	if(m_pCurrentGroup == NULL)
	{
		MessageBox(ResourceString(IDC_GROUP_NOTSELECTED, "No group selected for this action."));
		return;
	}
	EndDialog(eACTION_HIGHLIGHT);	
}

void CDialogGroupOrder::OnReorder() 
{
	if(m_pCurrentGroup == NULL)
	{
		MessageBox(ResourceString(IDC_GROUP_NOTSELECTED, "No group selected for this action."));
		return;
	}

	sds_name group;
	group[0] = (long)m_pCurrentGroup;
	group[1] = (long)m_pDrawing;

	TCHAR ns[8];
	int n = m_pCurrentGroup->nEntities();
	int fromPos, toPos, count;
	((CEdit*)GetDlgItem(IDC_EDIT_OLDPOS))->GetLine(0, ns, 8);
	fromPos = _ttoi(ns);
	if(fromPos < 0 || fromPos >= n)
	{
		MessageBox(ResourceString(IDC_GROUP_NOTVALIDNUMBER, "Not valid number for this parameter."));
		((CEdit*)GetDlgItem(IDC_EDIT_OLDPOS))->SetSel(0, -1);
		((CEdit*)GetDlgItem(IDC_EDIT_OLDPOS))->ReplaceSel("");
		return;
	}
	((CEdit*)GetDlgItem(IDC_EDIT_NEWPOS))->GetLine(0, ns, 8);
	toPos = _ttoi(ns);
	if(toPos < 0 || toPos >= n)
	{
		MessageBox(ResourceString(IDC_GROUP_NOTVALIDNUMBER, "Not valid number for this parameter."));
		((CEdit*)GetDlgItem(IDC_EDIT_NEWPOS))->SetSel(0, -1);
		((CEdit*)GetDlgItem(IDC_EDIT_NEWPOS))->ReplaceSel("");
		return;
	}
	((CEdit*)GetDlgItem(IDC_EDIT_NUM))->GetLine(0, ns, 8);
	count = _ttoi(ns);
	if(count < 1 || count > n)
	{
		MessageBox(ResourceString(IDC_GROUP_NOTVALIDNUMBER, "Not valid number for this parameter."));
		((CEdit*)GetDlgItem(IDC_EDIT_NUM))->SetSel(0, -1);
		((CEdit*)GetDlgItem(IDC_EDIT_NUM))->ReplaceSel("");
		return;
	}

	if(cmd_reorderGroupEntities(group, fromPos, toPos, count))
		m_bDidReorder = true;
	else
		MessageBox(ResourceString(IDC_GROUP_CANTREORDER, "Can't reorder entities in group."));
}

void CDialogGroupOrder::OnReverseOrder() 
{
	if(m_pCurrentGroup == NULL)
	{
		MessageBox(ResourceString(IDC_GROUP_NOTSELECTED, "No group selected for this action."));
		return;
	}

	sds_name group;
	group[0] = (long)m_pCurrentGroup;
	group[1] = (long)m_pDrawing;
	if(cmd_reverseGroupOrder(group))
		m_bDidReorder = true;
}

void CDialogGroupOrder::OnSelchange() 
{
	int index = m_pListGroup->GetItemData(m_pListGroup->GetCurSel());
	int i = 0;
	db_dictionaryrec* pCurRec;
	m_pDictGroup->get_recllends(&pCurRec, NULL);
	while(pCurRec != NULL)
	{
		if(i == index)
			break;
		++i;
		pCurRec = pCurRec->next;
	}
	if(pCurRec != NULL)
		m_pCurrentGroup = (CDbGroup*)pCurRec->hiref->ret_hip(m_pDrawing);
	else
	{
		m_pCurrentGroup = NULL;
		return;
	}

	int n = m_pCurrentGroup->nEntities();
	s_outString.Format("%s (0-%d):", ResourceString(IDC_GROUP_REMOVEFROM, "Remove from position"), n - 1);
	GetDlgItem(IDC_STXT_REMOVE)->SetWindowText((LPCTSTR)s_outString);
	s_outString.Format("%s (0-%d):", ResourceString(IDC_GROUP_PLACETO, "Place to Position"), n - 1);
	GetDlgItem(IDC_STXT_PLACE)->SetWindowText((LPCTSTR)s_outString);
	s_outString.Format("%s (1-%d):", ResourceString(IDC_GROUP_NUMBER, "Number of entities"), n);
	GetDlgItem(IDC_STXT_NENT)->SetWindowText((LPCTSTR)s_outString);
}
