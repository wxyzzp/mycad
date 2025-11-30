#include "stdafx.h"
#include "icad.h"
#include "DialogGroup.h"
#include "IcadApi.h"
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

CDialogGroup::CDialogGroup(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogGroup::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogGroup)
	m_displayUnnamed = FALSE;
	//}}AFX_DATA_INIT
}

CDialogGroup::CDialogGroup(CWnd* pParent, db_drawing* pDrawing)
	: CDialog(CDialogGroup::IDD, pParent), m_pDrawing(pDrawing)
{
	m_pDictGroup = (db_dictionary*)m_pDrawing->dictsearch(m_pDrawing->namedobjdict(),
		"ACAD_GROUP", 0);

	ASSERT(m_pDictGroup != NULL);
}

void CDialogGroup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogGroup)
	DDX_Check(pDX, GROUP_DISP_UNNAMED_CHK, m_displayUnnamed);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogGroup, CDialog)
	//{{AFX_MSG_MAP(CDialogGroup)
	ON_BN_CLICKED(GROUP_NEW_CREATE_BTN, OnCreateNew)
	ON_BN_CLICKED(GROUP_HILITE_BTN, OnHighlight)
	ON_BN_CLICKED(GROUP_BY_ENT_BTN, OnFindByEntity)
	ON_NOTIFY(NM_CLICK, GROUP_LIST, OnListClick)
	ON_BN_CLICKED(GROUP_MOD_ADD_BTN, OnAdd)
	ON_BN_CLICKED(GROUP_MOD_RMV_BTN, OnRemove)
	ON_BN_CLICKED(GROUP_MOD_UNGROUP_BTN, OnUngroup)
	ON_BN_CLICKED(GROUP_MOD_REORDER_BTN, OnReorder)
	ON_BN_CLICKED(GROUP_DISP_UNNAMED_CHK, OnDisplayUnnamed)
	ON_BN_CLICKED(GROUP_MOD_RENAME_BTN, OnRename)
	ON_BN_CLICKED(GROUP_MOD_DESC_BTN, OnChangeDescription)
	ON_BN_CLICKED(GROUP_MOD_SEL_CHK, OnChangeSelected)
	ON_BN_CLICKED(GROUP_NEW_UNNAMED_CHK, OnCreateUnnamed)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_EN_CHANGE(GROUP_NEW_NAME_EDIT, OnChangeNewName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogGroup message handlers
CDialogGroup::OnInitDialog()
{
	CDialog::OnInitDialog();

	CButton *pHelpButton = (CButton*)GetDlgItem(IDHELP);
	if(pHelpButton)
		pHelpButton->SetIcon(AfxGetApp()->LoadIcon(HELP_ICON));

	if(m_pDrawing == NULL || m_pDictGroup == NULL)
		return FALSE;

	((CButton*)GetDlgItem(GROUP_NEW_SEL_CHK))->SetCheck(1);
	GetDlgItem(GROUP_NEW_CREATE_BTN)->EnableWindow(FALSE);

	m_pListGroup = (CListCtrl*)GetDlgItem(GROUP_LIST);

	LVCOLUMN columnParams;
	columnParams.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	columnParams.fmt = LVCFMT_LEFT ;

	columnParams.pszText = "Name";
	columnParams.iSubItem = -1;
	columnParams.cx = 260;
	m_pListGroup->InsertColumn(0, &columnParams);

	columnParams.pszText = "Description";
	columnParams.iSubItem = 1;
	columnParams.cx = 430;
	m_pListGroup->InsertColumn(1, &columnParams);

	columnParams.pszText = "Selectable";
	columnParams.iSubItem = 2;
	columnParams.cx = 40;
	m_pListGroup->InsertColumn(2, &columnParams);

	fillListGroup();
	m_pCurrentGroup = NULL;

	return TRUE;
}

BOOL CDialogGroup::OnHelpInfo(HELPINFO *pHel){
	OnHelp();
	return(true);
}

void CDialogGroup::OnHelp() 
{
	// TODO: Add your control notification handler code here
	CommandRefHelp( m_hWnd, HLP_GROUP_MAIN);
}

int CDialogGroup::fillListGroup()
{
	db_dictionaryrec* pCurRec;
	CDbGroup* pGroup;
	m_pDictGroup->get_recllends(&pCurRec, NULL);
	int index = 0;
	int id = 0;
	LVITEM listItem;
	listItem.mask = LVIF_PARAM | LVIF_TEXT;
	listItem.iItem = 0;
	listItem.iSubItem = 0;
	while(pCurRec != NULL)
	{
		pGroup = (CDbGroup*)pCurRec->hiref->ret_hip(m_pDrawing);
		if(!pGroup->ret_deleted() && 
			(!pGroup->isUnnamed() || (pGroup->isUnnamed() && m_displayUnnamed)))
		{
			++listItem.iItem;
			listItem.pszText = pCurRec->name;
			listItem.lParam = id;
			index = m_pListGroup->InsertItem(&listItem);
			
			m_pListGroup->SetItem(index, 1, LVIF_TEXT, 
				pGroup->description(), 0, 0, 0, 0);
			m_pListGroup->SetItem(index, 2, LVIF_TEXT, 
				pGroup->isSelectable() ? ResourceString(IDC_GROUP_OPTIONYES, "Yes") : ResourceString(IDC_GROUP_OPTIONNO, "No"), 0, 0, 0, 0);
		}
		++id;
		pCurRec = pCurRec->next;
	}
	return 1;
}

int CDialogGroup::getCurrentGroup(sds_name sdsGroupName) const
{
	if(m_pCurrentGroup == NULL)
	{
		memset(sdsGroupName, 0, sizeof(sds_name));
		return 0;
	}

	sdsGroupName[0] = (long)m_pCurrentGroup;
	sdsGroupName[1] = (long)m_pDrawing;
	return 1;
}

void CDialogGroup::OnDisplayUnnamed() 
{
	UpdateData();
	m_pListGroup->DeleteAllItems();
	fillListGroup();
}

void CDialogGroup::OnCreateNew() 
{
	char groupName[IC_ACADBUF];
	char groupDesc[IC_ACADBUF];
	((CEdit*)GetDlgItem(GROUP_NEW_NAME_EDIT))->GetLine(0, groupName, IC_ACADBUF);
	((CEdit*)GetDlgItem(GROUP_NEW_DESC_EDIT))->GetLine(0, groupDesc, IC_ACADBUF);

	if(((CButton*)GetDlgItem(GROUP_NEW_UNNAMED_CHK))->GetCheck() == 0)
	{
		ic_trim(groupName,"be");
		if(cmd_findGroup(groupName))
		{
			MessageBox(ResourceString(IDC_GROUP_ALREADYEXIST, "Group with such name already exist."));
			return;
		}
		if(!ic_isvalidname(groupName))
		{
			MessageBox(ResourceString(IDC_GROUP_NOTVALIDNAME, "This is not valid name for group."));
			return;
		}
	}

	sds_name sdsGroupName;
	if(!cmd_createGroup(groupName, groupDesc, 
		((CButton*)GetDlgItem(GROUP_NEW_SEL_CHK))->GetCheck(),
		((CButton*)GetDlgItem(GROUP_NEW_UNNAMED_CHK))->GetCheck(),
		sdsGroupName))
	{
		MessageBox(ResourceString(IDC_GROUP_CANTCREATE, "Can't create group."));
		return;
	}

	m_pCurrentGroup = (CDbGroup*)sdsGroupName[0];
	EndDialog(eACTION_ADD);
}

void CDialogGroup::OnCreateUnnamed() 
{
	if(((CButton*)GetDlgItem(GROUP_NEW_UNNAMED_CHK))->GetCheck())
		((CEdit*)GetDlgItem(GROUP_NEW_NAME_EDIT))->EnableWindow(FALSE);
	else
		((CEdit*)GetDlgItem(GROUP_NEW_NAME_EDIT))->EnableWindow();

	TCHAR groupName[IC_ACADBUF];
	((CEdit*)GetDlgItem(GROUP_NEW_NAME_EDIT))->GetLine(0, groupName, IC_ACADBUF);
	if(((CButton*)GetDlgItem(GROUP_NEW_UNNAMED_CHK))->GetCheck() || _tcslen(groupName) > 0)
		GetDlgItem(GROUP_NEW_CREATE_BTN)->EnableWindow(TRUE);
	else
		GetDlgItem(GROUP_NEW_CREATE_BTN)->EnableWindow(FALSE);
}

void CDialogGroup::OnUngroup() 
{
	if(m_pCurrentGroup == NULL)
	{
		MessageBox(ResourceString(IDC_GROUP_NOTSELECTED, "No group selected for this action."));
		return;
	}

	sds_name group;
	group[0] = (long)m_pCurrentGroup;
	group[1] = (long)m_pDrawing;
	sds_entdel(group);

	m_pCurrentGroup = NULL;
	m_pListGroup->DeleteAllItems();
	fillListGroup();
	/*
	sds_name entities;
	group[0] = (long)m_pCurrentGroup;
	group[1] = (long)m_pDrawing;
	cmd_getSSFromGroup(entities, group);
	cmd_removeSSFromGroup(entities, group);
	*/
}

void CDialogGroup::OnRename() 
{
	if(m_pCurrentGroup == NULL)
	{
		MessageBox(ResourceString(IDC_GROUP_NOTSELECTED, "No group selected for this action."));
		return;
	}

	TCHAR newName[IC_ACADBUF];
	((CEdit*)GetDlgItem(GROUP_MOD_NAME_EDIT))->GetLine(0, newName, IC_ACADBUF);
	ic_trim(newName,"be");
	if(cmd_findGroup(newName))
	{
		MessageBox(ResourceString(IDC_GROUP_ALREADYEXIST, "Group with such name already exist."));
		return;
	}
	if(!ic_isvalidname(newName))
	{
		MessageBox(ResourceString(IDC_GROUP_NOTVALIDNAME, "This is not valid name for group."));
		return;
	}

	sds_name group;
	group[0] = (long)m_pCurrentGroup;
	group[1] = (long)m_pDrawing;
	if(!cmd_modifyGroup(newName, NULL, -1, group))
		ASSERT(FALSE);

	m_pListGroup->DeleteAllItems();
	fillListGroup();
}

void CDialogGroup::OnChangeDescription() 
{
	if(m_pCurrentGroup == NULL)
	{
		MessageBox(ResourceString(IDC_GROUP_NOTSELECTED, "No group selected for this action."));
		return;
	}

	TCHAR newDesc[IC_ACADBUF];
	((CEdit*)GetDlgItem(GROUP_MOD_DESC_EDIT))->GetLine(0, newDesc, IC_ACADBUF);

	sds_name group;
	group[0] = (long)m_pCurrentGroup;
	group[1] = (long)m_pDrawing;

	if(!cmd_modifyGroup(NULL, newDesc, -1, group))
		ASSERT(FALSE);

	m_pListGroup->DeleteAllItems();
	fillListGroup();
}

void CDialogGroup::OnChangeSelected() 
{
	if(m_pCurrentGroup == NULL)
	{
		MessageBox(ResourceString(IDC_GROUP_NOTSELECTED, "No group selected for this action."));
		return;
	}

	sds_name group;
	group[0] = (long)m_pCurrentGroup;
	group[1] = (long)m_pDrawing;

	if(!cmd_modifyGroup(NULL, NULL, ((CButton*)GetDlgItem(GROUP_MOD_SEL_CHK))->GetCheck(), group))
		ASSERT(FALSE);

	m_pListGroup->DeleteAllItems();
	fillListGroup();
}

void CDialogGroup::OnReorder() 
{
	EndDialog(eACTION_REORDER);	
}

void CDialogGroup::OnHighlight() 
{
	if(m_pCurrentGroup == NULL)
	{
		MessageBox(ResourceString(IDC_GROUP_NOTSELECTED, "No group selected for this action."));
		return;
	}
	EndDialog(eACTION_HIGHLIGHT);
}

void CDialogGroup::OnFindByEntity() 
{
	EndDialog(eACTION_FINDBYENTITY);	
}

void CDialogGroup::OnAdd() 
{
	if(m_pCurrentGroup == NULL)
	{
		MessageBox(ResourceString(IDC_GROUP_NOTSELECTED, "No group selected for this action."));
		return;
	}
	EndDialog(eACTION_ADD);
}

void CDialogGroup::OnRemove() 
{
	if(m_pCurrentGroup == NULL)
	{
		MessageBox(ResourceString(IDC_GROUP_NOTSELECTED, "No group selected for this action."));
		return;
	}
	EndDialog(eACTION_REMOVE);	
}

void CDialogGroup::OnListClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HD_NOTIFY *phdn = (HD_NOTIFY *) pNMHDR;
	// TODO: Add your control notification handler code here
	//if(phdn == NULL)
	//	return;

	// set new current group
	LVITEM listItem;
	listItem.mask = LVIF_PARAM;

	POSITION selectedItemPos = m_pListGroup->GetFirstSelectedItemPosition();
	if(selectedItemPos != NULL)
		listItem.iItem = m_pListGroup->GetNextSelectedItem(selectedItemPos);
	else
		return;

	m_pListGroup->GetItem(&listItem);

	int index = 0;;
	db_dictionaryrec* pCurRec;
	m_pDictGroup->get_recllends(&pCurRec, NULL);
	while(pCurRec != NULL)
	{
		if(listItem.lParam == index)
			break;
		++index;
		pCurRec = pCurRec->next;
	}

	if(pCurRec != NULL)
	{
		m_pCurrentGroup = (CDbGroup*)pCurRec->hiref->ret_hip(m_pDrawing);
		
		// display new current group parameters
		((CEdit*)GetDlgItem(GROUP_MOD_NAME_EDIT))->SetSel(0, -1);
		((CEdit*)GetDlgItem(GROUP_MOD_NAME_EDIT))->ReplaceSel(pCurRec->name);
		
		((CEdit*)GetDlgItem(GROUP_MOD_DESC_EDIT))->SetSel(0, -1);
		((CEdit*)GetDlgItem(GROUP_MOD_DESC_EDIT))->ReplaceSel(m_pCurrentGroup->description());
		
		((CButton*)GetDlgItem(GROUP_MOD_SEL_CHK))->SetCheck(m_pCurrentGroup->isSelectable() ? 1 : 0);
	}
	else
		m_pCurrentGroup = NULL;
	
	*pResult = 0;
}


void CDialogGroup::OnChangeNewName() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	TCHAR groupName[IC_ACADBUF];
	((CEdit*)GetDlgItem(GROUP_NEW_NAME_EDIT))->GetLine(0, groupName, IC_ACADBUF);
	if(((CButton*)GetDlgItem(GROUP_NEW_UNNAMED_CHK))->GetCheck() || (_tcslen(groupName) > 0 && ic_isvalidname(groupName)))
		GetDlgItem(GROUP_NEW_CREATE_BTN)->EnableWindow(TRUE);
	else
		GetDlgItem(GROUP_NEW_CREATE_BTN)->EnableWindow(FALSE);
}
