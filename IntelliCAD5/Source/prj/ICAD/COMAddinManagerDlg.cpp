#include "stdafx.h"
#include "COMAddinManagerDlg.h"
#include "res/icadrc2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CCOMAddinManagerDlg::CCOMAddinManagerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCOMAddinManagerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCOMAddinManagerDlg)
	m_description = _T("");
	//}}AFX_DATA_INIT
}

CCOMAddinManagerDlg::~CCOMAddinManagerDlg()
{
}

void CCOMAddinManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCOMAddinManagerDlg)
	DDX_Control(pDX, IDC_ADDINS_LIST, m_addinsListCtrl);
	DDX_Control(pDX, IDC_AM_CHECK3, m_checkBoxBtnCommand);
	DDX_Control(pDX, IDC_AM_CHECK2, m_checkBoxBtnStartup);
	DDX_Control(pDX, IDC_AM_CHECK1, m_checkBoxBtnLoad);
	DDX_Text(pDX, IDC_AM_DESCRIPTION, m_description);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCOMAddinManagerDlg, CDialog)
	//{{AFX_MSG_MAP(CCOMAddinManagerDlg)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_ADDINS_LIST, OnItemChanged)
	ON_BN_CLICKED(IDC_AM_CHECK1, OnCheckLoadBox)
	ON_BN_CLICKED(IDC_AM_CHECK2, OnCheckStartupBox)
	ON_BN_CLICKED(IDC_AM_CHECK3, OnCheckCommandBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCOMAddinManagerDlg message handlers

BOOL CCOMAddinManagerDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	RECT rect;
	m_addinsListCtrl.GetWindowRect(&rect);
	int width = rect.right - rect.left;
	AddColumn(ResourceString(IDC_ADDIN_MGR_ADD_INS, "Available Add-Ins"),1,-1,width*0.63); // 230);
	AddColumn(ResourceString(IDC_ADDIN_MGR_LOAD_BEHAVIOR, "Load Behavior"),2,-1,width*0.35);     // 128);

	int count = GetApp ()->comAddinManager ().getAddInsStatus(m_addins);

	for (int i=0; i<count; i++)
	{
		m_addins[i].m_bWillBeLoaded = m_addins[i].m_bLoaded; // init m_bWillBeLoaded 

		AddItem(i,0,m_addins[i].m_friendlyName);
		outLoadBehaviorColumn(i, m_addins[i]);
	}
	if ( count )
	{
		m_addinsListCtrl.SetItemState(0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		updateDialog(0);
	}
	else
	{
		m_checkBoxBtnLoad.EnableWindow(false);
		m_checkBoxBtnStartup.EnableWindow(false);
		//m_checkBoxBtnCommand.EnableWindow(false);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCOMAddinManagerDlg::OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	updateDialog(pNMListView->iItem);

	POSITION pos = m_addinsListCtrl.GetFirstSelectedItemPosition();
	if ( pos )
	{
		m_checkBoxBtnLoad.EnableWindow();
		m_checkBoxBtnStartup.EnableWindow();
		//m_checkBoxBtnCommand.EnableWindow();
	}
	else
	{
		m_checkBoxBtnLoad.SetCheck(0);
		m_checkBoxBtnStartup.SetCheck(0);
		//m_checkBoxBtnCommand.SetCheck(0);
		
		m_checkBoxBtnLoad.EnableWindow(false);
		m_checkBoxBtnStartup.EnableWindow(false);
		//m_checkBoxBtnCommand.EnableWindow(false);
	}
	*pResult = 0;
}

void CCOMAddinManagerDlg::OnCheckLoadBox() 
{
	POSITION pos = m_addinsListCtrl.GetFirstSelectedItemPosition();
	if ( pos )
	{
		int index = m_addinsListCtrl.GetNextSelectedItem(pos);
		m_addins[index].m_bWillBeLoaded = !m_addins[index].m_bWillBeLoaded;
		updateDialog(index);
	}
}

void CCOMAddinManagerDlg::OnCheckStartupBox() 
{
	POSITION pos = m_addinsListCtrl.GetFirstSelectedItemPosition();
	if ( pos )
	{
		int index = m_addinsListCtrl.GetNextSelectedItem(pos);
		m_addins[index].m_loadBehavior = !m_addins[index].m_loadBehavior;
		updateDialog(index);
	}
}

void CCOMAddinManagerDlg::OnCheckCommandBox() 
{
	;
}

void CCOMAddinManagerDlg::updateDialog(int index)
{
	if ( index >=0 && index < m_addins.GetSize() )
	{
		CCOMAddinStatus& aStatus = m_addins[index];

		outLoadBehaviorColumn(index, aStatus);

		m_checkBoxBtnLoad.SetCheck(aStatus.m_bWillBeLoaded);
		m_checkBoxBtnStartup.SetCheck(aStatus.m_loadBehavior);
		//m_checkBoxBtnCommand.SetCheck(0);

		m_description = aStatus.m_description;
	}
	UpdateData(false);
}

void CCOMAddinManagerDlg::outLoadBehaviorColumn(int index, const CCOMAddinStatus& aStatus)
{
	CString strLoaded(_T("Loaded")), 
			strUnloaded(_T("Unloaded")), 
			strStartup(_T("Startup"));

	CString str1, str2;
	if ( aStatus.m_loadBehavior )
		str1 = strStartup;

	if ( aStatus.m_bWillBeLoaded )
		str2 = strLoaded;
	else
		if ( aStatus.m_bLoaded )
			str2 = strUnloaded;

	if ( str1.IsEmpty() )
		str1 = str2;
	else
		if ( !str2.IsEmpty() )
			str1 += _T(" / ") + str2;

	AddItem(index,1, str1);
}

BOOL CCOMAddinManagerDlg::AddColumn(LPCTSTR strItem,int nItem,int nSubItem, int nCx, int nMask,int nFmt)
{
	LV_COLUMN lvc;
	lvc.mask = nMask;
	lvc.fmt = nFmt;
	lvc.pszText = (LPTSTR) strItem;
	if( nCx != -1 ) 
		lvc.cx = nCx;
	else
		lvc.cx = m_addinsListCtrl.GetStringWidth(lvc.pszText) + 15;
	if(nMask & LVCF_SUBITEM){
		if(nSubItem != -1)
			lvc.iSubItem = nSubItem;
		else
			lvc.iSubItem = nItem;
	}
	return m_addinsListCtrl.InsertColumn(nItem,&lvc);
}

BOOL CCOMAddinManagerDlg::AddItem(int nItem,int nSubItem,LPCTSTR strItem,int nImageIndex)
{
	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = nItem;
	lvItem.iSubItem = nSubItem;
	lvItem.pszText = (LPTSTR) strItem;
	if(nImageIndex != -1){
		lvItem.mask |= LVIF_IMAGE;
		lvItem.iImage |= nImageIndex;
	}
	if(nSubItem == 0)
		return m_addinsListCtrl.InsertItem(&lvItem);
	return m_addinsListCtrl.SetItem(&lvItem);
}

void CCOMAddinManagerDlg::OnOK() 
{
	CDialog::OnOK(); // close dialog before

	GetApp ()->comAddinManager ().correctAddInsStatus(m_addins);
}
