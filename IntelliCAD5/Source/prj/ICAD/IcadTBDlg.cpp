/* File  : <DevDir>\source\prj\icad\IcadTBDLg.cpp
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 */


#include "Icad.h"/*DNT*/
#include "IcadApi.h"/*DNT*/
#include "IcadTBDlg.h"/*DNT*/
#include "IcadToolbarMain.h"/*DNT*/
#include "resource.hm"/*DNT*/
#include "paths.h"/*DNT*/
#ifdef XML_TOOLBAR
#include "xml.h"
#include "xmldoc.h"
#endif

#ifdef _DEBUG

 #ifndef USE_SMARTHEAP
  #define new DEBUG_NEW
 #endif

#endif // _DEBUG

extern int			g_NumOfButtons;
extern IcadCommands	g_IcadButtons[];
extern LPTBBUTTON	g_pAllButtons;
//Modified Cybage SBD 31/12/2001 DD/MM/YYYY [
//Reason : Fix for Bug No. 77936 from Bugzilla
extern IcadCommands	g_MnuTbItems[];
extern int			g_nMnuTbItems;
CString getBMPId(const CIcadTBbutton* pTbButtonEx,const BOOL expColBTN);
//Modified Cybage SBD 31/12/2001 DD/MM/YYYY ]


/////////////////////////////////////////////////////////////////////////////
// CIcadTBDlg dialog
/////////////////////////////////////////////////////////////////////////////

CIcadTBDlg::CIcadTBDlg
(
 CWnd*	pParent /*=NULL*/
) : CDialog(CIcadTBDlg::IDD, pParent)
{
	resbuf	rb;
    sds_getvar("EXPLEVEL"/*DNT*/, &rb);
	switch (rb.resval.rint)
	{
		case 1 : m_ExpLevel = 3; break;
		case 2 : m_ExpLevel = 7; break;
		case 3 : m_ExpLevel = 15;
	}

	m_idxCurSel = -1;
	//{{AFX_DATA_INIT(CIcadTBDlg)
	//}}AFX_DATA_INIT
}

void
CIcadTBDlg::DoDataExchange
(
 CDataExchange*	pDX
)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIcadTBDlg)
	DDX_Control(pDX, IDC_ICAD_TB_CHECK2, m_Color);
	DDX_Control(pDX, IDC_ICAD_TB_CHECK1, m_LgBut);
	DDX_Control(pDX, IDC_ICAD_TB_CHECK3, m_Tcheck);
	DDX_Control(pDX, IDC_ICAD_TB_LIST1, m_TbList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CIcadTBDlg, CDialog)
	//{{AFX_MSG_MAP(CIcadTBDlg)
	ON_BN_CLICKED(IDCUSTOMIZE, OnCustomize)
	ON_BN_CLICKED(IDDELETE, OnDelete)
	ON_BN_CLICKED(IDC_ICAD_TB_CHECK3, OnIcadTbCheck3)
	ON_BN_CLICKED(IDC_ICAD_TB_CHECK1, OnIcadTbCheck1)
	ON_BN_CLICKED(IDC_ICAD_TB_CHECK2, OnIcadTbCheck2)
	ON_LBN_SETFOCUS(IDC_ICAD_TB_LIST1, OnSetfocusForCustomize)
	ON_LBN_SELCHANGE(IDC_ICAD_TB_LIST1, OnSelChangeTbList)
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDHELP,OnHelp)
END_MESSAGE_MAP()


// CIcadTBDlg message handlers

CIcadTBDlg::OnInitDialog()
{
	VERIFY(m_TbList.SubclassDlgItem(IDC_ICAD_TB_LIST1, this));
	VERIFY(m_Tcheck.SubclassDlgItem(IDC_ICAD_TB_CHECK3, this));
	VERIFY(m_Color.SubclassDlgItem(IDC_ICAD_TB_CHECK2, this));
	VERIFY(m_LgBut.SubclassDlgItem(IDC_ICAD_TB_CHECK1, this));

	m_HasTipChng = m_pTbMain->m_bToolTips;
	m_HasLgChng = m_pTbMain->m_bLargeButtons;
	m_HasColorChng = m_pTbMain->m_bColorButtons;

	if(m_pTbMain->m_bToolTips)
		m_Tcheck.SetCheck(1);
	else
		m_Tcheck.SetCheck(0);

	if(m_pTbMain->m_bLargeButtons)
		m_LgBut.SetCheck(1);
	else
		m_LgBut.SetCheck(0);

	if(m_pTbMain->m_bColorButtons)
		m_Color.SetCheck(1);
	else
		m_Color.SetCheck(0);

	CPtrList*		pToolBarList = m_pTbMain->GetToolBarList();
	CIcadToolBar*	pToolBar;
	CString			str;
	POSITION		pos = pToolBarList->GetHeadPosition();
	int				ct, idxListItem;

	for(ct = 0; pos; ++ct)
	{
		pToolBar = (CIcadToolBar*)pToolBarList->GetNext(pos);
		if(!pToolBar)
			continue;

		pToolBar->GetWindowText(str);
		if(pToolBar->m_bEmpty || str.IsEmpty())
			continue;

		idxListItem = m_TbList.AddString(str);
		m_TbList.SetCheck(idxListItem, pToolBar->IsWindowVisible()? 1 : 0);
		m_TbList.SetItemDataPtr(idxListItem, pToolBar);
	}

	CButton*	helpWnd = (CButton*)GetDlgItem(IDHELP);	

	if(helpWnd)
		helpWnd->SetIcon(AfxGetApp()->LoadIcon(HELP_ICON));

	if(m_bExport)
	{
		CWnd*	pControl;
		pControl = GetDlgItem(IDCUSTOMIZE);
		pControl->ShowWindow(SW_HIDE);
		pControl = GetDlgItem(IDDELETE);
		pControl->ShowWindow(SW_HIDE);
	}

	return 1;
}


void
CMainWindow::IcadToolbars
(
 bool	bExport /* = false */
)
{
	CIcadTBDlg	tbDlg(this);
	tbDlg.m_bExport = bExport;
	tbDlg.m_pTbMain = (CIcadToolBarMain*)m_pToolBarMain;

	tbDlg.DoModal();
}

void
CIcadTBDlg::OnCustomize()
{
	OnOK();
	SDS_CMainWindow->IcadCustomDia(1);
}

void
CIcadTBDlg::OnDelete()
{
	if(m_idxCurSel < 0)
		return;

	CIcadToolBar*	pDelToolBar = (CIcadToolBar*)m_TbList.GetItemDataPtr(m_idxCurSel);
	if(!pDelToolBar)
		return;

	CPtrList*		pToolBarList = m_pTbMain->GetToolBarList();
	POSITION		pos = pToolBarList->GetHeadPosition();
	CIcadToolBar*	pToolBar;

	while(pos)
	{
		pToolBar = (CIcadToolBar*)pToolBarList->GetAt(pos);
		if(pToolBar == pDelToolBar)
		{
			CString strText, strName, strFormat;
			m_TbList.GetText(m_idxCurSel, strName);
			strText.Format(ResourceString(IDS_DELETE_TOOLBAR_YN, "Are you sure you want to delete %s toolbar?" ),strName);
			if (MessageBox(strText,ResourceString(IDS_DELETE_TOOLBAR, "Select Toolbars" ),MB_ICONQUESTION|MB_YESNO)==IDYES)
				{

				// Remove the toolbar from the listbox.
				m_TbList.DeleteString(m_idxCurSel);
				// Remove the toolbar from the toolbar list.
				pToolBarList->RemoveAt(pos);
				delete pToolBar;

				// Set focus to the next item.
				if(m_idxCurSel > 0)
					--m_idxCurSel;

				CString	strText;

				if(m_TbList.GetCount() <= 0)
					m_idxCurSel = -1;

				m_TbList.SetCurSel(m_idxCurSel);

				if(m_idxCurSel >= 0)
					m_TbList.GetText(m_idxCurSel, strText);

				CEdit*	pEdit = (CEdit*)GetDlgItem(ICAD_TB_EDITTBNAME);
				if(pEdit)
					pEdit->SetWindowText(strText);

				m_pTbMain->SetModifiedFlag();
				m_pTbMain->m_pParentFrame->RecalcLayout();
			}
			break;
		}

		pToolBarList->GetNext(pos);
	}	// while(
}

void
CIcadTBDlg::OnCancel()
{
	m_pTbMain->m_bToolTips = m_HasTipChng;
	m_pTbMain->m_bLargeButtons = m_HasLgChng;
	m_pTbMain->m_bColorButtons = m_HasColorChng;

	CDialog::OnCancel();
}

void
CIcadTBDlg::OnHelp()
{
	CommandRefHelp(m_hWnd, HLP_TBCONFIG);
}

void
CIcadTBDlg::OnOK()
{
	if(m_bExport)
	{
		OnExport();
		CDialog::OnOK();
		return;
	}

	UpdateTbName();
	CPtrList*		pToolBarList = m_pTbMain->GetToolBarList();
	CIcadToolBar*	pToolBar;
	POSITION		pos = pToolBarList->GetHeadPosition();
	CString			strTbName;
	int				idxItem;

	while(pos)
	{
		pToolBar = (CIcadToolBar*)pToolBarList->GetNext(pos);
		if(!pToolBar)
			continue;

		pToolBar->GetWindowText(strTbName);
		if(pToolBar->m_bEmpty || strTbName.IsEmpty())
			continue;

		if((idxItem = m_TbList.FindString(-1, strTbName)) == LB_ERR)
			continue;

		if(m_TbList.GetCheck(idxItem) && !pToolBar->IsWindowVisible())
			SDS_CMainWindow->ShowControlBar(pToolBar, TRUE, TRUE);
		else
			if(!m_TbList.GetCheck(idxItem) && pToolBar->IsWindowVisible())
				SDS_CMainWindow->ShowControlBar(pToolBar, FALSE, TRUE);
	}

	resbuf	rb;
	rb.restype = SDS_RTSHORT;
	if(m_HasTipChng != m_pTbMain->m_bToolTips)
	{
		rb.resval.rint = m_pTbMain->m_bToolTips? 1 : 0;
		SDS_setvar(NULL, DB_QTOOLTIPS, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES, 0);
		m_pTbMain->OnToolTipsChanged();
	}

	BOOL	bUpdate = FALSE;
	if(m_HasLgChng != m_pTbMain->m_bLargeButtons)
	{
		rb.resval.rint = m_pTbMain->m_bLargeButtons? 1 : 0;
		SDS_setvar(NULL, DB_QTBSIZE, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES, 0);
		bUpdate = TRUE;
	}

	if(m_HasColorChng != m_pTbMain->m_bColorButtons)
	{
		rb.resval.rint = m_pTbMain->m_bColorButtons? 1 : 0;
		SDS_setvar(NULL, DB_QTBCOLOR, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES, 0);
		bUpdate = TRUE;
	}

	if(bUpdate)
		m_pTbMain->UpdateBitmaps();

	m_pTbMain->SetModifiedFlag();	// Just to be sure.

	CDialog::OnOK();
}

void
CIcadTBDlg::OnIcadTbCheck3()
{
	if(m_pTbMain->m_bToolTips)
		m_pTbMain->m_bToolTips = FALSE;
	else
		m_pTbMain->m_bToolTips = TRUE;
}

void
CIcadTBDlg::OnIcadTbCheck1()
{
	if(m_pTbMain->m_bLargeButtons)
		m_pTbMain->m_bLargeButtons = FALSE;
	else
		m_pTbMain->m_bLargeButtons = TRUE;
}

void
CIcadTBDlg::OnIcadTbCheck2()
{
	if(m_pTbMain->m_bColorButtons)
		m_pTbMain->m_bColorButtons = FALSE;
	else
		m_pTbMain->m_bColorButtons = TRUE;

}

void
CIcadTBDlg::OnSetfocusForCustomize()
{
	GetDlgItem(IDCUSTOMIZE)->EnableWindow(TRUE);
}

void
CIcadTBDlg::OnSelChangeTbList()
{
	int	idxCur = m_TbList.GetCurSel();

	if(m_idxCurSel >= 0)
	{
		UpdateTbName();
		m_TbList.SetCurSel(idxCur);
	}

	m_idxCurSel = idxCur;

	CString	strText;
	m_TbList.GetText(m_idxCurSel, strText);

	if(strText.IsEmpty())
		return;

	// Enable the delete button.
	CButton*	pButton = (CButton*)GetDlgItem(IDDELETE);
	pButton->EnableWindow();

	CEdit*	pEdit = (CEdit*)GetDlgItem(ICAD_TB_EDITTBNAME);
	pEdit->SetWindowText(strText);
}

void
CIcadTBDlg::UpdateTbName()
{
	if(m_idxCurSel == -1)
		return;

	CEdit*			pEdit = (CEdit*)GetDlgItem(ICAD_TB_EDITTBNAME);
	CIcadToolBar*	pToolBar = (CIcadToolBar*)m_TbList.GetItemDataPtr(m_idxCurSel);
	CString			strText, strTbName;

	pEdit->GetWindowText(strText);
	pToolBar->GetWindowText(strTbName);

	if(!strTbName.Compare(strText))
		return;

	// Update the toolbar.
	pToolBar->SetWindowText(strText);
	CWnd*	pParent = pToolBar->GetParentFrame();
	if(pParent->IsKindOf(RUNTIME_CLASS(CMiniDockFrameWnd)))
	{
		pParent->SetWindowText(strText);
		((CMiniDockFrameWnd*)pParent)->m_wndDockBar.SetWindowText(strText);
	}

	// Update the listbox.
	int	nCheck = m_TbList.GetCheck(m_idxCurSel);
	m_TbList.DeleteString(m_idxCurSel);
	m_TbList.InsertString(m_idxCurSel, strText);
	m_TbList.SetItemDataPtr(m_idxCurSel, pToolBar);
	m_TbList.SetCheck(m_idxCurSel, nCheck);
	m_pTbMain->SetModifiedFlag();
}

CTbListBox::CTbListBox()
{
	CCheckListBox();
}

CTbListBox::~CTbListBox()
{
}

void
CIcadTBDlg::OnExport()
{
	resbuf	rb;
	CString	fileName;

	// get file name
	// EBATECH(CNBR) 2002/4/30 Disable TYPE button option+2
	if(sds_getfiled(ResourceString(IDC_ICADCUSTOM_SELECT_TOOLB_28, "Select Toolbar File"),
			""/*DNT*/,
#ifdef XML_TOOLBAR
			ResourceString(IDC_ICADCUSTOM_STANDARD_MEN_29_TMP_WXML,
			"Standard Menu File|mnu,Add-on Menu File|mns,XML ToolBar File|xml"/*DNT*/),
#else
			ResourceString(IDC_ICADCUSTOM_STANDARD_MEN_29,
			"Standard Menu File|mnu,Add-on Menu File|mns"/*DNT*/),
#endif
			5+2, &rb) != RTNORM)
		return;

	CPtrList*	pToolBarList = m_pTbMain->GetToolBarList();
	POSITION	pos = pToolBarList->GetHeadPosition();
	CString		tbName;
	int			tbIndex;
	
	if(rb.restype == RTSTR)
	{
		fileName = rb.resval.rstring;
		IC_FREE(rb.resval.rstring);
#ifdef XML_TOOLBAR
		// VT. Add saving of toolbars to XML. 4-6-02
		if(fileName.Right(4).CompareNoCase(".xml") == 0)
		{
			CMapPtrToPtr includedBars;
			int			ct;

			for(ct = 0; pos; ++ct)
			{
				CIcadToolBar*	pToolBar = (CIcadToolBar*)pToolBarList->GetNext(pos);
				
				if(!pToolBar)
					continue;
				
				pToolBar->GetWindowText(tbName);
				if(!((tbIndex = m_TbList.FindString(-1, tbName)) == LB_ERR || !m_TbList.GetCheck(tbIndex)))
					continue;

				includedBars[pToolBar] = pToolBar;
			}	// for(

			m_pTbMain->SaveToXML(fileName, &includedBars);

			return;
		}
#endif
	}
	
	// create export file
	CStdioFile	exportFile(fileName, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone | CFile::typeText);
	CString		line;
	
	// export selected toolbars
	UpdateTbName();
	
	CIcadToolBar*	pToolBar;
	CString			tbNameCorrected;
	TBBUTTON		tbButton;
	CIcadTBbutton*	pTbButtonEx;
	
	//Modified Cybage SBD 31/12/2001 DD/MM/YYYY [
	//Reason : Fix for Bug No. 77936 from Bugzilla
	//To enable loading of toolbars in ACAD
	line.Format("***MENUGROUP=ICAD\n");
	exportFile.WriteString(line);
	//Modified Cybage SBD 31/12/2001 DD/MM/YYYY ]
	
	// export ***TOOLBARS section
	line.Format("***TOOLBARS\n");
	exportFile.WriteString(line);
	
	while(pos)
	{
		pToolBar = (CIcadToolBar*)pToolBarList->GetNext(pos);
		if(!pToolBar)
			continue;
		
		pToolBar->GetWindowText(tbName);
		
		if(pToolBar->m_bEmpty || tbName.IsEmpty())
			continue;
		
		if((tbIndex = m_TbList.FindString(-1, tbName)) == LB_ERR)
			continue;
		
		if(m_TbList.GetCheck(tbIndex))
		{
			// export one selected toolbar
			// export general toolbar definition into ***TOOLBARS section
			tbNameCorrected = tbName;
			tbNameCorrected.Replace(' ', '_');
			tbNameCorrected.Replace(',', '_');
			tbNameCorrected.Replace('.', '_');
			line.Format("**%s\n", tbNameCorrected);
			line.MakeUpper();
			exportFile.WriteString(line);
			RECT	tbRect;
			pToolBar->GetWindowRect(&tbRect);
			// TAG1 [Toolbar ("tbarname", orient, visible, xval, yval, rows)]
			//Modified Cybage SBD 31/12/2001 DD/MM/YYYY [
			//Reason : Fix for Bug No. 77936 from Bugzilla
			//To enable loading of toolbars in ACAD
			//line.Format("TBAR_%s [_Toolbar (\"%s\", %s, %s, %d, %d, %d)]\n",
			line.Format("TBAR_%s [_Toolbar(\"%s\", %s, %s, %d, %d, %d)]\n",
				tbNameCorrected,
				tbName,
				pToolBar->IsFloating()? "_Floating" : "_Top",
				pToolBar->IsWindowVisible()? "_Show" : "_Hide",
				tbRect.left, tbRect.top, pToolBar->GetToolBarCtrl().GetRows());
			exportFile.WriteString(line);
			
			// export toolbar buttons definitions
			for(int i = 0; i < pToolBar->GetToolBarCtrl().GetButtonCount(); ++i)
			{
				pToolBar->GetToolBarCtrl().GetButton(i, &tbButton);
				if(tbButton.fsStyle & TBSTYLE_SEP)
					// write separator
					line.Format("[--]\n");
				else
				{
					pTbButtonEx = (CIcadTBbutton*)tbButton.dwData;
					if(!pTbButtonEx->m_pibFlyOut)
					{
						// write non-flyout buttons
						tbNameCorrected = pTbButtonEx->m_toolTip;
						tbNameCorrected.Replace(' ', '_');
						tbNameCorrected.Replace(',', '_');
						tbNameCorrected.Replace('.', '_');

						// TAG2 [Button ("btnname", id_small, id_large)]macro
						//Modified Cybage SBD 31/12/2001 DD/MM/YYYY [
						//Reason : Fix for Bug No. 77936 from Bugzilla
						//line.Format("ID_%s [_Button (\"%s\", %s, %s)]%s\n",
						//			tbNameCorrected,
						//			pTbButtonEx->m_toolTip,
						//			m_pTbMain->m_bColorButtons? pTbButtonEx->m_smColorBmpPath.GetBuffer(0) : pTbButtonEx->m_smBwBmpPath.GetBuffer(0),
						//			m_pTbMain->m_bColorButtons? pTbButtonEx->m_lgColorBmpPath.GetBuffer(0) : pTbButtonEx->m_lgBwBmpPath.GetBuffer(0),
						//			pTbButtonEx->m_command);
						line.Format("ID_%s [_Button(\"%s\", %s)]%s\n",
									tbNameCorrected,
									pTbButtonEx->m_toolTip,
									getBMPId(pTbButtonEx,m_pTbMain->m_bColorButtons),								
									pTbButtonEx->m_command);
					}
					else
					{
						// write flyout buttons
						pTbButtonEx = pTbButtonEx->m_pibCurFlyOut;
						tbNameCorrected = pTbButtonEx->m_toolTip;
						tbNameCorrected.Replace(' ', '_');
						tbNameCorrected.Replace(',', '_');
						tbNameCorrected.Replace('.', '_');

						tbName.Format("TBAR_%s", tbNameCorrected);
						tbName.MakeUpper();
						// TAG3 [Flyout ("flyname", id_small, id_large, icon, alias)]macro
						//Modified Cybage SBD 31/12/2001 DD/MM/YYYY [
						//Reason : Fix for Bug No. 77936 from Bugzilla
						//line.Format("ID_%s [_Flyout (\"%s\", %s, %s, %s, %s)]%s\n",
						//			tbNameCorrected,
						//			pTbButtonEx->m_toolTip,
						//			m_pTbMain->m_bColorButtons? pTbButtonEx->m_smColorBmpPath : pTbButtonEx->m_smBwBmpPath,
						//			m_pTbMain->m_bColorButtons? pTbButtonEx->m_lgColorBmpPath : pTbButtonEx->m_lgBwBmpPath,
						//			pTbButtonEx == pTbButtonEx->m_pibCurFlyOut? "_OwnIcon" : "_OtherIcon",
						//			tbName, pTbButtonEx->m_command);
						line.Format("ID_%s [_Flyout(\"%s\", %s, %s, %s)]%s\n",
									tbNameCorrected,
									pTbButtonEx->m_toolTip,
									getBMPId(pTbButtonEx,m_pTbMain->m_bColorButtons),
									pTbButtonEx == pTbButtonEx->m_pibCurFlyOut? "_OwnIcon" : "_OtherIcon",
									tbName, pTbButtonEx->m_command);

						pTbButtonEx = pTbButtonEx->m_pibFlyOut;
					}
				}

				exportFile.WriteString(line);
			}	// for(
		}
	}	// while(pos)

	line.Format("\n");
	exportFile.WriteString(line);

	// export flyout toolbars
	pos = pToolBarList->GetHeadPosition();

	while(pos)
	{
		pToolBar = (CIcadToolBar*)pToolBarList->GetNext(pos);
		if(!pToolBar)
			continue;

		pToolBar->GetWindowText(tbName);

		if(pToolBar->m_bEmpty || tbName.IsEmpty())
			continue;

		if((tbIndex = m_TbList.FindString(-1, tbName)) == LB_ERR)
			continue;

		if(m_TbList.GetCheck(tbIndex))
		{
			for(int i = 0; i < pToolBar->GetToolBarCtrl().GetButtonCount(); ++i)
			{
				pToolBar->GetToolBarCtrl().GetButton(i, &tbButton);
				if(!(tbButton.fsStyle & TBSTYLE_SEP))
				{
					pTbButtonEx = (CIcadTBbutton*)tbButton.dwData;
					if(pTbButtonEx->m_pibCurFlyOut)
					{
						// export general toolbar definition into ***TOOLBARS section
						tbNameCorrected = pTbButtonEx->m_pibCurFlyOut->m_toolTip;
						tbNameCorrected.Replace(' ', '_');
						tbNameCorrected.Replace(',', '_');
						tbNameCorrected.Replace('.', '_');

						line.Format("**TBAR_%s\n", tbNameCorrected);
						line.MakeUpper();

						exportFile.WriteString(line);
						RECT	tbRect;
						pToolBar->GetWindowRect(&tbRect);

						// TAG1 [Toolbar ("tbarname", orient, visible, xval, yval, rows)]
						//Modified Cybage SBD 31/12/2001 DD/MM/YYYY [
						//Reason : Fix for Bug No. 77936 from Bugzilla
						//To enable loading of toolbars in ACAD
						//line.Format("TBAR_%s [_Toolbar (\"%s\", %s, %s, %d, %d, %d)]\n",
						line.Format("TBAR_%s [_Toolbar(\"%s\", %s, %s, %d, %d, %d)]\n",
									tbNameCorrected,
									pTbButtonEx->m_pibCurFlyOut->m_toolTip,
									"_Floating", "_Hide",
									tbRect.left, tbRect.top, pToolBar->GetToolBarCtrl().GetRows());
						exportFile.WriteString(line);

						// write flyout buttons
						pTbButtonEx = pTbButtonEx->m_pibFlyOut;
						while(pTbButtonEx)
						{
							// export toolbar buttons definitions
							tbNameCorrected = pTbButtonEx->m_toolTip;
							tbNameCorrected.Replace(' ', '_');
							tbNameCorrected.Replace(',', '_');
							tbNameCorrected.Replace('.', '_');

							// TAG2 [Button ("btnname", id_small, id_large)]macro
							//Modified Cybage SBD 31/12/2001 DD/MM/YYYY [
							//Reason : Fix for Bug No. 77936 from Bugzilla
							//line.Format("ID_%s [_Button (\"%s\", %s, %s)]%s\n",
							//			tbNameCorrected,
							//			pTbButtonEx->m_toolTip,
							//			m_pTbMain->m_bColorButtons? pTbButtonEx->m_smColorBmpPath : pTbButtonEx->m_smBwBmpPath,
							//			m_pTbMain->m_bColorButtons? pTbButtonEx->m_lgColorBmpPath : pTbButtonEx->m_lgBwBmpPath,
							//			pTbButtonEx->m_command);
							line.Format("ID_%s [_Button(\"%s\", %s)]%s\n",
										tbNameCorrected,
										pTbButtonEx->m_toolTip,
										getBMPId(pTbButtonEx,m_pTbMain->m_bColorButtons),
										pTbButtonEx->m_command);
							exportFile.WriteString(line);

							pTbButtonEx = pTbButtonEx->m_pibFlyOut;
						}	// while(
					}
				}
			}	// for(
		}
	}	// while(pos)

	line.Format("\n");
	exportFile.WriteString(line);

	// export ***HELPSTRINGS section
	line.Format("***HELPSTRINGS\n");
	exportFile.WriteString(line);
	pos = pToolBarList->GetHeadPosition();
	while(pos)
	{
		pToolBar = (CIcadToolBar*)pToolBarList->GetNext(pos);
		if(!pToolBar)
			continue;

		pToolBar->GetWindowText(tbName);

		if(pToolBar->m_bEmpty || tbName.IsEmpty())
			continue;

		if((tbIndex = m_TbList.FindString(-1, tbName)) == LB_ERR)
			continue;

		if(m_TbList.GetCheck(tbIndex))
		{
			// export one selected toolbar
			// export general toolbar definition into ***TOOLBARS section
			tbNameCorrected = tbName;
			tbNameCorrected.Replace(' ', '_');
			tbNameCorrected.Replace(',', '_');
			tbNameCorrected.Replace('.', '_');

			line.Format("TBAR_%s [%s]\n", tbNameCorrected, tbName);
			exportFile.WriteString(line);

			// export toolbar buttons definitions
			for(int i = 0; i < pToolBar->GetToolBarCtrl().GetButtonCount(); ++i)
			{
				pToolBar->GetToolBarCtrl().GetButton(i, &tbButton);
				if(!(tbButton.fsStyle & TBSTYLE_SEP))
				{
					pTbButtonEx = (CIcadTBbutton*)tbButton.dwData;
					if(!pTbButtonEx->m_pibFlyOut)
					{
						// write non-flyout buttons
						tbNameCorrected = pTbButtonEx->m_toolTip;
						tbNameCorrected.Replace(' ', '_');
						tbNameCorrected.Replace(',', '_');
						tbNameCorrected.Replace('.', '_');

						line.Format("ID_%s [%s]\n", tbNameCorrected, pTbButtonEx->m_helpString);
						exportFile.WriteString(line);
					}
					else
					{
						// write flyout buttons
						tbNameCorrected = pTbButtonEx->m_pibCurFlyOut->m_toolTip;
						tbNameCorrected.Replace(' ', '_');
						tbNameCorrected.Replace(',', '_');
						tbNameCorrected.Replace('.', '_');
						
						line.Format("TBAR_%s [%s]\n", tbNameCorrected, pTbButtonEx->m_pibCurFlyOut->m_helpString);
						exportFile.WriteString(line);

						pTbButtonEx = pTbButtonEx->m_pibFlyOut;
						while(pTbButtonEx)
						{
							tbNameCorrected = pTbButtonEx->m_toolTip;
							tbNameCorrected.Replace(' ', '_');
							tbNameCorrected.Replace(',', '_');
							tbNameCorrected.Replace('.', '_');

							line.Format("ID_%s [%s]\n", tbNameCorrected, pTbButtonEx->m_helpString);
							exportFile.WriteString(line);
							pTbButtonEx = pTbButtonEx->m_pibFlyOut;
						}
					}
				}
			}	// for(
		}
	}	// while(pos)

	exportFile.Flush();
	exportFile.Close();
}

//Added Cybage SBD 31/12/2001 DD/MM/YYYY [
//Reason : Fix for Bug No. 77936 from Bugzilla
//returns Bitmap ID's in the form "ICON_16_LINE, ICON_16_LINE" or "746, 234" or
//"small.bmp, large.bmp" or a combination of above

CString getBMPId(const CIcadTBbutton* pTbButtonEx,const BOOL expColBTN)
{
	CString temp;
	CString SmallBMPID;
	CString LargeBMPID;
	BOOL bFoundSm,bFoundLg;
	bFoundSm=FALSE;
	bFoundLg=FALSE;
	temp="";
	SmallBMPID="";
	LargeBMPID="";

	if(expColBTN)
	{
		if(!pTbButtonEx->m_smColorBmpPath.IsEmpty())
		{
			SmallBMPID=pTbButtonEx->m_smColorBmpPath;
			bFoundSm=TRUE;
		}

		if(!pTbButtonEx->m_lgColorBmpPath.IsEmpty())
		{
			LargeBMPID=pTbButtonEx->m_lgColorBmpPath;
			bFoundLg=TRUE;
		}

	}
	else
	{
	
		if(!pTbButtonEx->m_smBwBmpPath.IsEmpty())
		{
			SmallBMPID=pTbButtonEx->m_smBwBmpPath;
			bFoundSm=TRUE;
		}

		if(!pTbButtonEx->m_lgBwBmpPath.IsEmpty())
		{
			LargeBMPID=pTbButtonEx->m_lgBwBmpPath;
			bFoundLg=TRUE;
		}

	}

	for(int ct = 0; ct < g_nMnuTbItems; ++ct)
	{
		if(bFoundLg && bFoundSm)
			break;
		
		if(expColBTN)
		{
			if(!bFoundSm && g_MnuTbItems[ct].BMPIdSmColor ==  pTbButtonEx->m_BMPIdSmColor)
			{
				bFoundSm=TRUE;
				if(NULL == g_MnuTbItems[ct].pAcadTbStr)
					SmallBMPID.Format("%d",pTbButtonEx->m_BMPIdSmColor);
				else
					SmallBMPID.Format("ICON_16_%s",g_MnuTbItems[ct].pAcadTbStr) ;
			}
			
			if(!bFoundLg && g_MnuTbItems[ct].BMPIdLgColor ==  pTbButtonEx->m_BMPIdLgColor)
			{
				bFoundLg=TRUE;
				if(NULL == g_MnuTbItems[ct].pAcadTbStr)
					LargeBMPID.Format("%d",pTbButtonEx->m_BMPIdLgColor);
				else
					LargeBMPID.Format("ICON_16_%s",g_MnuTbItems[ct].pAcadTbStr) ;
			}
		}
		else
		{
			if(!bFoundSm && g_MnuTbItems[ct].BMPIdSmBW	  ==  pTbButtonEx->m_BMPIdSmBW)
			{
				bFoundSm=TRUE;
				if(NULL == g_MnuTbItems[ct].pAcadTbStr)
					SmallBMPID.Format("%d",pTbButtonEx->m_BMPIdSmBW);
				else
					SmallBMPID.Format("ICON_16_%s",g_MnuTbItems[ct].pAcadTbStr) ;
			}

			if(!bFoundLg && g_MnuTbItems[ct].BMPIdLgBW ==  pTbButtonEx->m_BMPIdLgBW)
			{
				bFoundLg=TRUE;
				if(NULL == g_MnuTbItems[ct].pAcadTbStr)
					LargeBMPID.Format("%d",pTbButtonEx->m_BMPIdLgBW);
				else
					LargeBMPID.Format("ICON_16_%s",g_MnuTbItems[ct].pAcadTbStr) ;
			}
		}
	}

	if(expColBTN)
	{
		if(!bFoundSm)
			SmallBMPID.Format("%d",pTbButtonEx->m_BMPIdSmColor);
		
		if(!bFoundLg)
			LargeBMPID.Format("%d",pTbButtonEx->m_BMPIdLgColor);
	}
	else
	{
		if(!bFoundSm)
			SmallBMPID.Format("%d",pTbButtonEx->m_BMPIdSmBW);

		if(!bFoundLg)
			LargeBMPID.Format("%d",pTbButtonEx->m_BMPIdLgBW);
	}

	temp.Format("%s, %s",SmallBMPID,LargeBMPID);
	return temp;
}

//Added Cybage SBD 31/12/2001 DD/MM/YYYY ]
