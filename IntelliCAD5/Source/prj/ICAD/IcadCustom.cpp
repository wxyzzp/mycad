/* File  : <DevDir>\source\prj\icad\IcadCustom.cpp
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 */

//*********************************************************************
//*  IcadCustom.cpp                                                   *
//*  Copyright (C) 1994,1995, 1996, 1997 by Visio, Inc.               *
//*********************************************************************

// Also see files:
//	IcadCusMenu.cpp
//	IcadCusToolbar.cpp
//	IcadCusKeyboard.cpp
//	IcadCusShortcuts.cpp

#include "Icad.h"/*DNT*/
#include "IcadApi.h"/*DNT*/
#include "cmds.h"/*DNT*/
#include "IcadCustom.h"/*DNT*/
#include "Icadxlink.h"/*DNT*/
#include "IcadToolBarMain.h"/*DNT*/
#include "IcadMenuMain.h"/*DNT*/
#include "resource.hm"/*DNT*/
#include "paths.h"/*DNT*/
#include "IcadDialog.h"/*DNT*/

#define ICAD_CUSTOM_CLOSE 100
#define ICAD_CUSTOM_IMPORT 101
#define ICAD_CUSTOM_EXPORT 102
#define ICAD_CUSTOM_RESET 103

char	CUS_szText[256];
int		g_nAppendMode;

void OnMenuImportOk(HWND hDlg);


#ifdef _SUPPORT_CUSTOM_FORMS


BEGIN_MESSAGE_MAP(CIcadCusForm, CPropertyPage)
    ON_WM_PAINT()
END_MESSAGE_MAP()

void
CIcadCusForm::OnPaint()
{
	short	index;
	CString	String;

	CPropertyPage::OnPaint();

	index = m_FormList.GetCurSel();

	if(index != -1)
	{
		m_FormList.GetText(index, String);
		ShowForm(String);
	}
}

LRESULT
CIcadCusForm::WindowProc
(
 UINT	message,
 WPARAM	wParam,
 LPARAM	lParam
)
{
	int		index;
	CString	String;

	switch(message)
	{
		case WM_COMMAND :
			switch(HIWORD(wParam))
			{
				case BN_CLICKED :
					switch(LOWORD(wParam))
					{
						case CUS_FRM_NEW :
							if(!m_FormWnd)
							{
								m_FormWnd = new FRM_FormEdit();
								m_FormWnd->m_Parent = this;
								m_FormWnd->m_Formname = NULL;
								m_FormWnd->Create(NULL, "Form Designer"/*DNT*/, WS_OVERLAPPEDWINDOW, CRect(0, 0, 0, 0), this);
								m_FormWnd->ShowWindow(SW_SHOWNORMAL);
								//m_FormWnd->m_DlgWin->OnNewDlg();
								m_FormList.AddString(m_FormWnd->m_DlgWin->m_Formname);
								m_FormList.SelectString(-1, m_FormWnd->m_DlgWin->m_Formname);
							}
							break;

						case CUS_FRM_EDIT :
							index = m_FormList.GetCurSel();
							m_FormList.GetText(index, String);
							if(!m_FormWnd)
							{
								m_FormWnd = new FRM_FormEdit();
								m_FormWnd->m_Parent = this;								
								m_FormWnd->m_Formname = new char [String.GetLength() + 1];								
								strcpy(m_FormWnd->m_Formname, (LPCTSTR)String);
								m_FormWnd->Create(NULL, "Form Designer"/*DNT*/, WS_OVERLAPPEDWINDOW, CRect(0, 0, 0, 0), this);
								m_FormWnd->ShowWindow(SW_SHOWNORMAL);
							}
							else
							{
								m_FormWnd->m_DlgWin->m_Formname = String;
								m_FormWnd->m_DlgWin->ChangeDlg();
							}
							break;

						case CUS_FRM_DELETE :
							index = m_FormList.GetCurSel();
							m_FormList.GetText(index, String);
							m_FormList.DeleteString(index);
							::DeleteForm(String);
							break;
					}
					break;

				case FRM_UPDATE_CUS :
					index = m_FormList.GetCurSel();
					if(index != -1)
					{
						m_FormList.DeleteString(index);
						m_FormList.AddString(m_FormWnd->m_DlgWin->m_Formname);
						m_FormList.SelectString(-1, m_FormWnd->m_DlgWin->m_Formname);
					}
					break;

				case FRM_CLOSEING :
					m_FormWnd = NULL;

				case LBN_SELCHANGE:
					index = m_FormList.GetCurSel();
					if(index != -1)
					{
						m_FormList.GetText(index, String);
						ShowForm(String);
					}
					break;

				case LBN_DBLCLK :
					index = m_FormList.GetCurSel();
					m_FormList.GetText(index, String);
					if(!m_FormWnd)
					{
						m_FormWnd = new FRM_FormEdit();
						m_FormWnd->m_Parent = this;						
						m_FormWnd->m_Formname = new char [String.GetLength() + 1];						
						strcpy(m_FormWnd->m_Formname, (LPCTSTR)String);
						m_FormWnd->Create(NULL, "Form Designer"/*DNT*/, WS_OVERLAPPEDWINDOW, CRect(0, 0, 0, 0), this);
						m_FormWnd->ShowWindow(SW_SHOWNORMAL);
					}
					else
					{
						m_FormWnd->m_DlgWin->m_Formname = String;
						m_FormWnd->m_DlgWin->ChangeDlg();
					}
					break;
			}
			break;
	}

	return CPropertyPage::WindowProc(message, wParam, lParam);
}

void
CIcadCusForm::ShowForm
(
 CString	String
)
{
	BYTE*		bptr;
	CString		dbstr;
	COleVariant	varFieldValue1;

	if(!SDS_CMainWindow->IcadStartDao())
		return;

	SDS_CMainWindow->m_CdatarecordPtr->MoveFirst();
	while(!SDS_CMainWindow->m_CdatarecordPtr->IsBOF() && !SDS_CMainWindow->m_CdatarecordPtr->IsEOF())
	{
		VarToCStr(&dbstr, &SDS_CMainWindow->m_CdatarecordPtr->GetFieldValue("FormName"/*DNT*/));
		if(dbstr == String)
			break;
		SDS_CMainWindow->m_CdatarecordPtr->MoveNext();
	}

	VarToBinay(&bptr, &SDS_CMainWindow->m_CdatarecordPtr->GetFieldValue("FormPic"/*DNT*/));

	if(!bptr)
		return;
	m_picture.RedrawWindow();
	FRM_Showbitmap(bptr, &m_picture);
	IC_FREE(bptr);
}

BOOL
CIcadCusForm::OnInitDialog()
{
	CString	dbstr, string;

	CPropertyPage::OnInitDialog();

	m_FormWnd = NULL;
	m_FormList.SubclassDlgItem(CUS_FORM_LIST, this);
	m_picture.SubclassDlgItem(CUS_FORM_PREVIEW, this);

	if(!SDS_CMainWindow->IcadStartDao())
		return FALSE;

	if(sds_findfile(SDS_DBFILE, CUS_szText) != SDS_RTNORM)
	{
		dbstr.Format(ResourceString(IDC_ICADCUSTOM_UNABLE_TO_LOC_3, "Unable to locate the Database %s"), CUS_szText);
		sds_alert(dbstr);
		return FALSE;
	}

	if(FILE_ATTRIBUTE_READONLY == GetFileAttributes(CUS_szText))
	{
		SDS_CMainWindow->m_readwrite = TRUE;
		string.Format(ResourceString(IDC_ICADCUSTOM_THE_DATABASE__4, "The Database %s is read only.\n You will not be able to save changes."), CUS_szText);
		MessageBox(string, ResourceString(IDC_ICADCUSTOM_INTELLICADD_F_5, "IntelliCADD Form Designer"), MB_ICONASTERISK | MB_APPLMODAL);
	}
	else
		SDS_CMainWindow->m_readwrite = FALSE;

	if(!SDS_CMainWindow->m_CdatabasePtr.IsOpen())
	{
		try
		{
			SDS_CMainWindow->m_CdatabasePtr.Open(CUS_szText, FALSE, SDS_CMainWindow->m_readwrite);
		}
		catch(CDaoException* e)
		{
			e->Delete();
			return FALSE;
		}
	}

	SDS_CMainWindow->m_CdatarecordPtr = new CDaoRecordset(&SDS_CMainWindow->m_CdatabasePtr);

	if(!SDS_CMainWindow->m_CdatarecordPtr->IsOpen())
	{
		try
		{
			SDS_CMainWindow->m_CdatarecordPtr->Open(dbOpenDynaset, ResourceString(IDC_ICADCUSTOM_SELECT___FROM_6, "Select * from Forminfo"));
		}
		catch(CDaoException* e)
		{
			AfxMessageBox(e->m_pErrorInfo->m_strDescription);
			e->Delete();
			return TRUE;
		}
	}

	while(!SDS_CMainWindow->m_CdatarecordPtr->IsBOF() && !SDS_CMainWindow->m_CdatarecordPtr->IsEOF())
	{
		VarToCStr(&dbstr, &SDS_CMainWindow->m_CdatarecordPtr->GetFieldValue("FormName"/*DNT*/));
		m_FormList.AddString((LPCTSTR)dbstr);
		SDS_CMainWindow->m_CdatarecordPtr->MoveNext();
	}

	return TRUE;
}

CIcadCusForm::CIcadCusForm(): CPropertyPage(CUS_FORM)
{
}

void
CIcadCusForm::DoDataExchange
(
 CDataExchange*	pDX
)
{
	CPropertyPage::DoDataExchange(pDX);
}


///////////////////////////////////////////////////////////////////////
// Xlink tab
//
///////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CIcadCusXlink, CPropertyPage)
END_MESSAGE_MAP()

BOOL
CIcadCusXlink::OnInitDialog()
{
	CString	dbstr;

	CPropertyPage::OnInitDialog();

	m_xlinklist.SubclassDlgItem(CUS_XLINK_LIST, this);
	m_colcombo.SubclassDlgItem(CUS_XLINK_COLUMN, this);

	if(!SDS_CMainWindow->IcadStartDao())
		return FALSE;

	if(sds_findfile(SDS_DBFILE, CUS_szText) != SDS_RTNORM)
	{
		dbstr.Format(ResourceString(IDC_ICADCUSTOM_UNABLE_TO_LOC_3, "Unable to locate the Database %s"), CUS_szText);
		sds_alert(dbstr);
		return FALSE;
	}

	if(FILE_ATTRIBUTE_READONLY & GetFileAttributes(CUS_szText))
	{
		SDS_CMainWindow->m_readwrite = TRUE;
		dbstr.Format(ResourceString(IDC_ICADCUSTOM_THE_DATABASE__4, "The Database %s is read only.\n You will not be able to save changes."), CUS_szText);
		sds_alert(dbstr);
	}
	else
		SDS_CMainWindow->m_readwrite = FALSE;

	try
	{
		SDS_CMainWindow->m_CdatabasePtr.Open(CUS_szText, FALSE, SDS_CMainWindow->m_readwrite);
	}
	catch(CDaoException* e)
	{
		e->Delete();
		return FALSE;
	}

	SDS_CMainWindow->m_CdatarecordPtr = new CDaoRecordset(&SDS_CMainWindow->m_CdatabasePtr);

	if(SDS_CMainWindow->m_CdatarecordPtr->IsOpen())
		SDS_CMainWindow->m_CdatarecordPtr->Close();

	try
	{
		SDS_CMainWindow->m_CdatarecordPtr->Open(dbOpenDynaset, ResourceString(IDC_ICADCUSTOM_SELECT___FROM_7, "Select * from xlinkinfo"));
	}
	catch(CDaoException* e)
	{
		AfxMessageBox(e->m_pErrorInfo->m_strDescription);
		e->Delete();
	}

	while(!SDS_CMainWindow->m_CdatarecordPtr->IsBOF() && !SDS_CMainWindow->m_CdatarecordPtr->IsEOF())
	{
		VarToCStr(&dbstr, &SDS_CMainWindow->m_CdatarecordPtr->GetFieldValue("Xlink"/*DNT*/));
		m_xlinklist.AddString((LPCTSTR)dbstr);
		SDS_CMainWindow->m_CdatarecordPtr->MoveNext();
	}

	m_xlinklist.SetCurSel(0);
	UpdateFields(0);

	return TRUE;
}

LRESULT
CIcadCusXlink::WindowProc
(
 UINT	message,
 WPARAM	wParam,
 LPARAM	lParam
)
{
	int		index;
	CString	String;

	switch(message)
	{
		case WM_COMMAND :
			switch(HIWORD(wParam))
			{
				case BN_CLICKED :
					switch(LOWORD(wParam))
					{
						case CUS_XLINK_NEW :
							SDS_CMainWindow->DisplayWizard();
							break;

						case CUS_XLINK_DELETE :
							index=m_xlinklist.GetCurSel();
							m_xlinklist.GetText(index, String);
							DeleteXlink(String);
							break;
					}
					break;

				case LBN_SELCHANGE :
					index = m_xlinklist.GetCurSel();
					UpdateFields(index);
			}
	}

	return CPropertyPage::WindowProc(message, wParam, lParam);
}

void
CIcadCusXlink::UpdateFields
(
 short	index
)
{
	CString	Itemtext, Path, Database, Accessvalue;
	int		sindex;
	char	*sptr, *eptr;

	m_xlinklist.GetText(index, Accessvalue);
	Itemtext.Format("Xlink='%s'"/*DNT*/, Accessvalue);

	if(!SDS_CMainWindow->m_CdatarecordPtr->Find(AFX_DAO_FIRST, Itemtext))
		return;

	VarToCStr(&Itemtext, &SDS_CMainWindow->m_CdatarecordPtr->GetFieldValue("XLink"/*DNT*/));
	VarToCStr(&Path, &SDS_CMainWindow->m_CdatarecordPtr->GetFieldValue("Path"/*DNT*/));
	VarToCStr(&Database, &SDS_CMainWindow->m_CdatarecordPtr->GetFieldValue("Database"/*DNT*/));
	VarToCStr(&Accessvalue, &SDS_CMainWindow->m_CdatarecordPtr->GetFieldValue("AccessValue"/*DNT*/));

	SetDlgItemText(CUS_XLINK_NAME, Itemtext);
	SetDlgItemText(CUS_XLINK_PATH, Path);
	SetDlgItemText(CUS_XLINK_DATAB, Database);

	sindex = Accessvalue.Find('.');
	Accessvalue.SetAt(sindex, 0);
	SetDlgItemText(CUS_XLINK_TABLE, Accessvalue);
	++sindex;
	m_colcombo.ResetContent();

	sptr = (char*)((LPCTSTR)(Accessvalue) + sindex);

	do
	{
		if(!(eptr = strchr(sptr, '.'/*DNT*/)))
			break;

		*eptr = 0;
		m_colcombo.AddString(sptr);
		sptr = eptr + 1;
	} while(1);

	m_colcombo.AddString(sptr);
}

CIcadCusXlink::CIcadCusXlink() : CPropertyPage(CUS_XLINK)
{
}

void
CIcadCusXlink::DoDataExchange
(
 CDataExchange*	pDX
)
{
	CPropertyPage::DoDataExchange(pDX);
}


#endif	// _SUPPORT_CUSTOM_FORMS


///////////////////////////////////////////////////////////////////////
// Property sheet
//
///////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CIcadCustom, IcadPropertySheet)
	//{{AFX_MSG_MAP(CIcadCustom)
    ON_BN_CLICKED(IDHELP, OnHelp)
	ON_COMMAND(ICAD_CUSTOM_IMPORT, OnImportButton)
	ON_COMMAND(ICAD_CUSTOM_EXPORT, OnExportButton)
	ON_COMMAND(ICAD_CUSTOM_RESET, OnResetButton)
	ON_WM_CLOSE()
	ON_WM_HELPINFO()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ICAD_CUSTOM_CLOSE, OnCloseButton)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void
CIcadCustom::OnContextMenu
(
 CWnd*	pWnd,
 CPoint	point
)
{
	// Return from here so we don't get that stupid "What's this?" context menu.
	return;
}

CIcadCustom::CIcadCustom() : IcadPropertySheet(ResourceString(IDC_ICADCUSTOM_CUSTOMIZE_14, "Customize"))
{

    AddPage(&m_CusMenu);
    AddPage(&m_CusToolbar);
    AddPage(&m_CusKeyboard);

    #if _SUPPORT_CUSTOM_FORMS
        AddPage(&m_CusForm);
	    AddPage(&m_CusXlink);
    #endif

    AddPage(&m_Shortcuts);
}

//This is the function call from the main
void
CMainWindow::IcadCustomDia
(
 short	nActiveTab
)
{
// nActiveTab is an index to the page that the caller wishes to display
// as active.  Default=0

    BeginModalState();

	if(m_pIcadCustom)
		delete m_pIcadCustom;

	m_pIcadCustom = new CIcadCustom();
	((CIcadCustom*)m_pIcadCustom)->m_pMain = this;
	((CIcadCustom*)m_pIcadCustom)->m_nActiveTab = nActiveTab < 0 ? 0 : nActiveTab;

	CIcadToolBarMain*	pTbMain = (CIcadToolBarMain*)m_pToolBarMain;

	pTbMain->m_pCusTbDlg = &(((CIcadCustom*)m_pIcadCustom)->m_CusToolbar);
	pTbMain->m_pCusTbDlg->m_pTbMain = pTbMain;
	pTbMain->m_bCustomize = TRUE;
	m_bCustomize = TRUE;

	CIcadCustom*	pCust = (CIcadCustom*)m_pIcadCustom;

	DWORD	dwStyle;
	if(nActiveTab < 0)
	{
		pCust->m_bQueryUser = false;
		dwStyle = DS_MODALFRAME | DS_3DLOOK | DS_SETFONT | WS_SYSMENU | WS_POPUP | WS_CAPTION;
	}
	else
	{
		pCust->m_bQueryUser = true;
		dwStyle = DS_MODALFRAME | DS_3DLOOK | DS_SETFONT | WS_SYSMENU | WS_POPUP | WS_VISIBLE | WS_CAPTION;
	}

	pCust->Create(this, dwStyle);
}

BOOL
CIcadCustom::OnInitDialog()
{
	m_CusMenu.m_bOKtoWarn = FALSE;

    CPropertySheet::OnInitDialog();

	RECT	rc;
	GetWindowRect(&rc);

	// Increase the height of the CPropertySheet by 30 to add room for the buttons.
	rc.bottom += 30;
	MoveWindow(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
	ScreenToClient(&rc);

	// Create the font for the buttons.
	CFont*	pFont = new CFont();
	// Set the font based on whether we are running under Windows95 or NT 3.5.
	OSVERSIONINFO	osVer;
	osVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osVer);

	BOOL	bWin95 = TRUE;

	if( osVer.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS ||
		(osVer.dwPlatformId == VER_PLATFORM_WIN32_NT && osVer.dwMajorVersion >= 4) )
		// Windows 95 or NT 4.0
		pFont->CreateStockObject(DEFAULT_GUI_FONT);
	else
	{
		pFont->CreateStockObject(DEVICE_DEFAULT_FONT);
		bWin95 = FALSE;
	}

	// Create the Close button
	m_CloseBtn.Create(ResourceString(IDC_ICADCUSTOM_CLOSE_15, "Close"), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON,
					  CRect(rc.right - 300, rc.bottom - 32, rc.right - 235, rc.bottom - 8),
					  this, ICAD_CUSTOM_CLOSE);

	m_CloseBtn.SetFont(pFont);

	// Create the Import button
	m_ImportBtn.Create(ResourceString(IDC_ICADCUSTOM__IMPORT____16, "&Import..."), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					   CRect(rc.right - 225, rc.bottom - 32, rc.right - 160, rc.bottom - 8),
					   this, ICAD_CUSTOM_IMPORT);
	m_ImportBtn.SetFont(pFont);

	// Create the Export button
	m_ExportBtn.Create(ResourceString(IDC_ICADCUSTOM__EXPORT____17, "&Export..."), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					   CRect(rc.right - 150, rc.bottom - 32, rc.right - 85, rc.bottom - 8),
					   this, ICAD_CUSTOM_EXPORT);
	m_ExportBtn.SetFont(pFont);

	// Create the Reset button
	m_ResetBtn.Create(ResourceString(IDC_ICADCUSTOM__RESET_18, "&Reset"), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					  CRect(rc.right - 75, rc.bottom - 32, rc.right - 10, rc.bottom - 8),
					  this, ICAD_CUSTOM_RESET);
	m_ResetBtn.SetFont(pFont);

	// Create the Help button
	AddHelpButton();
	m_help.SetFont(pFont);

	// Set the flag that gets checked in PreTranslateMessage()
	CMainWindow*	pMain = (CMainWindow*)m_pMain;

	// Gray-out the main menu.
	if(pMain && pMain->m_pMenuMain)
	{
		CIcadMenu*	pMenu = pMain->m_pMenuMain->GetCurrentMenu();
		if(pMenu)
			pMenu->EnableAllItems(FALSE);
	}

	// Set the active tab
	SetActivePage(m_nActiveTab);
	m_CusMenu.m_bOKtoWarn = TRUE;

	if(!m_nActiveTab)
		m_CusMenu.WarnUser();

	delete pFont;
	return TRUE;
}

BOOL
CIcadCustom::PreTranslateMessage
(
 MSG*	pMsg
)
{
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		OnCloseButton();
		return TRUE;
	}

	return CPropertySheet::PreTranslateMessage(pMsg);
}

afx_msg void
CIcadCustom::OnClose()
{
	OnCloseButton();
}

afx_msg void
CIcadCustom::OnCloseButton()
{
	CMainWindow*	pMain = (CMainWindow*)m_pMain;
	pMain->EndModalState();
	BeginWaitCursor();
	m_CusMenu.OnExit();

	// Toolbar registry gets saved in OnExit().
    m_CusToolbar.OnExit();

    m_CusKeyboard.OnExit();
	EndWaitCursor();

    if(!m_Shortcuts.OnExit())
		return;

	DestroyWindow();

	pMain->m_bCustomize = FALSE;
	((CIcadToolBarMain*)pMain->m_pToolBarMain)->m_bCustomize = FALSE;
	((CIcadToolBarMain*)pMain->m_pToolBarMain)->m_pCusTbDlg = NULL;

	// Load the mnl file for this menu.
	if(!m_CusMenu.m_strLoadLisp.IsEmpty())
		cmd_loadlispmnl((char*)m_CusMenu.m_strLoadLisp.GetBuffer(0));

	// Save everything to the registry.
	BeginWaitCursor();
	pMain->SaveAcceleratorsToReg();
	pMain->SaveAliasesToReg();

	if(pMain->m_pMenuMain)
	{
		CIcadMenu*	pMenu = pMain->m_pMenuMain->GetDefaultMenu();
		if(pMenu)
			pMenu->SaveToReg();
	}

	EndWaitCursor();
}

afx_msg void
CIcadCustom::OnImportButton()
{
	CPropertyPage*	pActivePage = GetActivePage();

	if(pActivePage == &m_CusMenu)			// Menu
	{
		resbuf	rb;
		CString	str;

		g_nAppendMode = 0;

		// EBATECH(CNBR) 2002/4/30 Disable TYPE button option+2
		if(SDS_GetFiled(ResourceString(IDC_ICADCUSTOM_SELECT_MENU__20, "Select Menu File"),
						""/*DNT*/,
						ResourceString(IDC_ICADCUSTOM_STANDARD_MEN_22, "Standard Menu File|mnu,Add-on Menu File|mns,IntelliCAD Menu File|icm"),
						4+2,
						&rb,
						MAKEINTRESOURCE(CUS_MENU_IMPORT),
						OnMenuImportOk) != RTNORM)
			return;

		if(rb.restype == RTSTR)
		{
			str = rb.resval.rstring;
			IC_FREE(rb.resval.rstring);
		}

		// EBATECH{2001.9.8 accept upper-case char.
		char drive[_MAX_DRIVE];
		char dir[_MAX_DIR];
		char fname[_MAX_FNAME];
		char ext[_MAX_EXT];
		_splitpath(str.GetBuffer(MAX_PATH), drive, dir, fname, ext);
		CString str2 = ext;
		str2.MakeUpper();
		// }EBATECH

		//if(str.Find(".icm"/*DNT*/) != -1)
		if(str2.Find(".ICM"/*DNT*/) != -1) // EBATECH{2001.9.8}
		// EBATECH
		{
			// Read the menu file (our format).
			if(!m_CusMenu.m_pCurMenu)
				return;

			if(!m_CusMenu.m_pCurMenu->LoadFromICM(str))
			{
				CString	strText;
				strText.Format(ResourceString(IDC_ICADCUSTOM_THE_FILE___24, "The file \"%s\" is not a valid IntelliCAD Menu file" ), str);
				m_CusMenu.MessageBox(strText, NULL, MB_ICONEXCLAMATION);
				return;
			}

			if(g_nAppendMode != 1)
				((CTreeCtrl*)m_CusMenu.GetDlgItem(CUS_MENU_TREE))->DeleteAllItems();

			int	ct;
			if(m_CusMenu.m_pCurMenu)
			{
				for(ct = 0; ct < m_CusMenu.m_pCurMenu->m_nMenuItems; ++ct)
					m_CusMenu.AddMenuItemsToTree(&ct, NULL);

				m_CusMenu.m_pCurMenu->m_bMenuModified = TRUE;
			}

			m_CusMenu.m_strLoadLisp.Empty();
		}
		//else if(str.Find(".mnu"/*DNT*/) != -1 || str.Find(".mns"/*DNT*/) != -1)
		else if(str2.Find(".MNU"/*DNT*/) != -1 || str2.Find(".MNS"/*DNT*/) != -1) // EBATECH{2001.9.8}
		{
			if(!m_CusMenu.m_pCurMenu)
				return;

			LPICMNUINFO	pMnuInfo = m_CusMenu.m_pCurMenu->MnuToMnuInfo(str, g_nAppendMode);
			if(!pMnuInfo)
				return;

			if(g_nAppendMode != 1)
				((CTreeCtrl*)m_CusMenu.GetDlgItem(CUS_MENU_TREE))->DeleteAllItems();

			LPICMNUINFO	pmiCur = pMnuInfo;
			m_CusMenu.m_pCurMenu->MnuInfoToMenuArray(pmiCur, 0);
			m_CusMenu.m_pCurMenu->FreeMnuInfo(pMnuInfo);

			int	ct;
			for(ct = 0; ct < m_CusMenu.m_pCurMenu->m_nMenuItems; ++ct)
				m_CusMenu.AddMenuItemsToTree(&ct, NULL);

			m_CusMenu.m_pCurMenu->m_bMenuModified = TRUE;
			m_CusMenu.m_strLoadLisp = str;
		}
		else
		{
			CString	strText;
			strText.Format(ResourceString(IDC_ICADCUSTOM_THE_FILE___24, "The file \"%s\" is not a valid IntelliCAD Menu file"), str);
			m_CusMenu.MessageBox(strText, NULL, MB_ICONEXCLAMATION);
			return;
		}
	}
	else if(pActivePage == &m_CusToolbar)	// Toolbars
	{
		if(m_CusToolbar.m_pTbMain)
			// Always do this when focus is taken away from drag and drop.
			m_CusToolbar.m_pTbMain->OnCustButtonDown(NULL, NULL, NULL, -1);

		resbuf	rb;
		CString	str;

		g_nAppendMode = 1;

		// EBATECH(CNBR) 2002/4/30 Disable TYPE button option+2
		if(SDS_GetFiled(ResourceString(IDC_ICADCUSTOM_SELECT_TOOLB_28, "Select Toolbar File"),
						""/*DNT*/,
#ifdef XML_TOOLBAR
						ResourceString(IDC_ICADCUSTOM_STANDARD_MEN_29_TMP_WXML,
						    "Standard Menu File|mnu,Add-on Menu File|mns,XML ToolBar File|xml"/*DNT*/),
#else
						ResourceString(IDC_ICADCUSTOM_STANDARD_MEN_29,
						    "Standard Menu File|mnu,Add-on Menu File|mns"/*DNT*/),
#endif
						4+2,
						&rb,
						MAKEINTRESOURCE(CUS_TOOLBAR_IMPORT),
						OnMenuImportOk) != RTNORM)
			return;

		if(rb.restype == RTSTR)
		{
			str = rb.resval.rstring;
			IC_FREE(rb.resval.rstring);
			str.MakeLower();
		}

		// EBATECH{2001.9.8 accept upper-case char.
		char drive[_MAX_DRIVE];
		char dir[_MAX_DIR];
		char fname[_MAX_FNAME];
		char ext[_MAX_EXT];
		_splitpath(str.GetBuffer(MAX_PATH), drive, dir, fname, ext);
		CString str2 = ext;
		str2.MakeUpper();
		// }EBATECH

		//if(str.Find(".ict"/*DNT*/) != -1)
		if(str2.Find(".ICT"/*DNT*/) != -1) // EBATECH{2001.9.8}
		{
			// Read Icad toolbar file here.
		}
		//else if(str.Find(".mnu"/*DNT*/) != -1 || str.Find(".mns"/*DNT*/) != -1)
		else if(str2.Find(".MNU"/*DNT*/) != -1 || str2.Find(".MNS"/*DNT*/) != -1) // EBATECH{2001.9.8}
		{
			((CIcadToolBarMain *)((CMainWindow*)m_pMain)->m_pToolBarMain)->CreateFromMnu(str, TRUE, (BOOL)g_nAppendMode);
		}
#ifdef XML_TOOLBAR
		else if(str2.Find(".XML"/*DNT*/) != -1) // VT 4-6-02
		{
			((CIcadToolBarMain *)((CMainWindow*)m_pMain)->m_pToolBarMain)->ReadFromXML(str);
		}
#endif
		else
		{
			CString	strText;
			strText.Format(ResourceString(IDC_ICADCUSTOM__IS_NOT_A_VA_31, "The file \"%s\" is not a valid IntelliCAD Toolbar file"), str);
			m_CusMenu.MessageBox(strText, NULL, MB_ICONEXCLAMATION);
			return;
		}
	}
	else if(pActivePage == &m_CusKeyboard)	// Keyboard
	{
		resbuf	rb;
		CString	str;

		// EBATECH(CNBR) 2002/4/30 Disable TYPE button option+2
		if(sds_getfiled(ResourceString(IDC_ICADCUSTOM_SELECT_KEYBO_32, "Select Keyboard File"),
						""/*DNT*/,
						ResourceString(IDC_ICADCUSTOM_INTELLICAD_K_33, "IntelliCAD Keyboard File|ick"),
						4+2,
						&rb) != RTNORM)
			return;

		if(rb.restype == RTSTR)
		{
			str = rb.resval.rstring;
			IC_FREE(rb.resval.rstring);
		}

		// EBATECH{2001.9.8 accept upper-case char.
		char drive[_MAX_DRIVE];
		char dir[_MAX_DIR];
		char fname[_MAX_FNAME];
		char ext[_MAX_EXT];
		_splitpath(str.GetBuffer(MAX_PATH), drive, dir, fname, ext);
		CString str2 = ext;
		str2.MakeUpper();
		// }EBATECH

		//if(str.Find(".ick"/*DNT*/) != -1)
		if(str2.Find(".ICK"/*DNT*/) != -1) // EBATECH{2001.9.8}
		{
			// Read the alias file (our format).
			if(!m_CusKeyboard.ReadAcceleratorFile(str))
			{
				CString	strText;
				strText.Format(ResourceString(IDC_ICADCUSTOM__IS_NOT_A_VA_35, "The file \"%s\" is not a valid IntelliCAD Keyboard file"), str);
				m_CusKeyboard.MessageBox(strText, NULL, MB_ICONEXCLAMATION);
				return;
			}
		}
		else
		{
			CString	strText;
			strText.Format(ResourceString(IDC_ICADCUSTOM__IS_NOT_A_VA_35, "The file \"%s\" is not a valid IntelliCAD Keyboard file"), str);
			m_CusKeyboard.MessageBox(strText, NULL, MB_ICONEXCLAMATION);
			return;
		}

		m_CusKeyboard.ResetAccelList();
		((CMainWindow*)m_pMain)->m_bKeyboardModified = TRUE;
	}

#ifdef _SUPPORT_CUSTOM_FORMS

	else if(pActivePage == &m_CusForm)		// Forms
	{
		resbuf	rb;
		char	fs1[80];

		// EBATECH(CNBR) 2002/4/30 Disable TYPE button option+2
		if(sds_getfiled(ResourceString(IDC_ICADCUSTOM_SELECT_FILE_36, "Select File"),
						""/*DNT*/,
						ResourceString(IDC_ICADCUSTOM_RESOURCE_FIL_37, "Resource File|rc,Dialog File|dcl"),
						4+2,
						&rb) != RTNORM)
			return;

		if(rb.restype == RTSTR)
		{
			strcpy(fs1, rb.resval.rstring);
			IC_FREE(rb.resval.rstring);
		}

        if(strstr(fs1, ".dcl"/*DNT*/))
            FRM_dcltoform(fs1, this->m_hWnd);
	}
	else if(pActivePage == &m_CusXlink)		// XLink
	{
	}

#endif	// _SUPPORT_CUSTOM_FORMS

	else if(pActivePage == &m_Shortcuts)	// Aliases
	{
		resbuf	rb;
		CString	str;

		g_nAppendMode = 1;

		// EBATECH(CNBR) 2002/4/30 Disable TYPE button option+2
		if(SDS_GetFiled(ResourceString(IDC_ICADCUSTOM_SELECT_ALIAS_39, "Select Alias File"),
						""/*DNT*/,
						ResourceString(IDC_ICADCUSTOM_INTELLICAD_A_40, "IntelliCAD Alias File|ica,AutoCAD Alias File|pgp"),
						4+2,
						&rb,
						MAKEINTRESOURCE(CUS_SHORTCUTS_IMPORT),
						OnMenuImportOk) != RTNORM)
			return;

		if(rb.restype == RTSTR)
		{
			str = rb.resval.rstring;
			IC_FREE(rb.resval.rstring);
		}

		// EBATECH{2001.9.8 accept upper-case char.
		char drive[_MAX_DRIVE];
		char dir[_MAX_DIR];
		char fname[_MAX_FNAME];
		char ext[_MAX_EXT];
		_splitpath(str.GetBuffer(MAX_PATH), drive, dir, fname, ext);
		CString str2 = ext;
		str2.MakeUpper();
		// }EBATECH

		//if(str.Find(".pgp"/*DNT*/) != -1)
		if(str2.Find(".PGP"/*DNT*/) != -1) // EBATECH{2001.9.8}
		{
			if(!g_nAppendMode)
				SDS_FreeAlias();
			SDS_ReadAlias(str.GetBuffer(0));
		}
		//else if(str.Find(".ica"/*DNT*/) != -1)
		else if(str2.Find(".ICA"/*DNT*/) != -1) // EBATECH{2001.9.8}
		{
			// Read the alias file (our format).
			if(!m_Shortcuts.ReadAliasFile(str, g_nAppendMode))
			{
				CString	strText;
				strText.Format(ResourceString(IDC_ICADCUSTOM__IS_NOT_A_VA_43, "The file \"%s\" is not a valid IntelliCAD Alias file"), str);
				m_Shortcuts.MessageBox(strText, NULL, MB_ICONEXCLAMATION);
				return;
			}
		}
		else
		{
			CString	strText;
			strText.Format(ResourceString(IDC_ICADCUSTOM__IS_NOT_A_VA_43, "The file \"%s\" is not a valid IntelliCAD Alias file"), str);
			m_Shortcuts.MessageBox(strText, NULL, MB_ICONEXCLAMATION);
			return;
		}
		m_Shortcuts.ResetAliasList();
		((CMainWindow*)m_pMain)->m_bAliasModified = TRUE;
	}
}

void
OnMenuImportOk
(
 HWND	hDlg
)
{
	HWND	hAppend;
	if( !(hAppend = GetDlgItem(hDlg, CUS_MENU_IMPORT_APPEND)) &&
		!(hAppend = GetDlgItem(hDlg, CUS_TOOLBAR_IMPORT_APPEND)) &&
		!(hAppend = GetDlgItem(hDlg, CUS_SHORTCUTS_IMPORT_APPEND)) )
		return;

	g_nAppendMode = (int)SendMessage(hAppend, BM_GETCHECK, 0, 0L);
}

afx_msg void
CIcadCustom::OnExportButton()
{
	CPropertyPage*	pActivePage = GetActivePage();

	if(pActivePage == &m_CusMenu)			// Menu
	{
		resbuf	rb;
		CString	str;

		// EBATECH(CNBR) 2002/4/30 Disable TYPE button option+2
		if(sds_getfiled(ResourceString(IDC_ICADCUSTOM_SELECT_MENU__20, "Select Menu File"),
						""/*DNT*/,
						ResourceString(IDC_ICADCUSTOM_INTELLICAD_M_44, "IntelliCAD Menu File|icm"),
						5+2,
						&rb) != RTNORM)
			return;

		if(rb.restype == RTSTR)
		{
			str = rb.resval.rstring;
			IC_FREE(rb.resval.rstring);
		}

		// EBATECH{2001.9.8 accept upper-case char.
		char drive[_MAX_DRIVE];
		char dir[_MAX_DIR];
		char fname[_MAX_FNAME];
		char ext[_MAX_EXT];
		_splitpath(str.GetBuffer(MAX_PATH), drive, dir, fname, ext);
		CString str2 = ext;
		str2.MakeUpper();
		// }EBATECH

		// Save any changes to the command and help strings in the dialog.
		CTreeCtrl*	pTreeCtrl = (CTreeCtrl*)m_CusMenu.GetDlgItem(CUS_MENU_TREE);
		if(pTreeCtrl)
		{
			HTREEITEM	hItem = pTreeCtrl->GetSelectedItem();
			m_CusMenu.SaveCommandToItem(hItem);
		}

		//if(str.Find(ResourceString(DNT_ICADCUSTOM__ICM_23, ".icm")) != -1)
		if(str2.Find(".ICM"/*DNT*/) != -1) // EBATECH{2001.9.8}
		{
			// Write the file (our format).
			// Delete the old menu array.
			if(m_CusMenu.m_pCurMenu)
				m_CusMenu.m_pCurMenu->DeleteMenuArray();

			// Create a new menu array.
			m_CusMenu.CreateCustomMenuArray();
			// Save the MenuArray items to disk.
			m_CusMenu.m_pCurMenu->SaveToICM(str);
		}
		else
		{
			CString	strText;
			strText.Format(ResourceString(IDC_ICADCUSTOM_THE_FILE___24, "The file \"%s\" is not a valid IntelliCAD Menu file"), str);
			m_CusMenu.MessageBox(strText, NULL, MB_ICONEXCLAMATION);
			return;
		}
	}
	else if(pActivePage == &m_CusToolbar)	// Toolbars
	{
		if(m_CusToolbar.m_pTbMain)
			// Always do this when focus is taken away from drag and drop.
			m_CusToolbar.m_pTbMain->OnCustButtonDown(NULL, NULL, NULL, -1);

		SDS_CMainWindow->IcadToolbars(true);
	}
	else if(pActivePage == &m_CusKeyboard)	// Keyboard
	{
		resbuf	rb;
		CString	str;
		// EBATECH(CNBR) 2002/4/30 Disable TYPE button option+2
		if(sds_getfiled(ResourceString(IDC_ICADCUSTOM_SELECT_KEYBO_32, "Select Keyboard File"),
						""/*DNT*/,
						ResourceString(IDC_ICADCUSTOM_INTELLICAD_K_33, "IntelliCAD Keyboard File|ick"),
						5+2,
						&rb) != RTNORM)
			return;

		if(rb.restype == RTSTR)
		{
			str = rb.resval.rstring;
			IC_FREE(rb.resval.rstring);
		}

		// EBATECH{2001.9.8 accept upper-case char.
		char drive[_MAX_DRIVE];
		char dir[_MAX_DIR];
		char fname[_MAX_FNAME];
		char ext[_MAX_EXT];
		_splitpath(str.GetBuffer(MAX_PATH), drive, dir, fname, ext);
		CString str2 = ext;
		str2.MakeUpper();
		// }EBATECH

		m_CusKeyboard.SaveCommandString();
		//if(str.Find(".ick"/*DNT*/))
		if(str2.Find(".ICK"/*DNT*/) != -1) // EBATECH{14-1-2002}
			// Write the file (our format).
			m_CusKeyboard.SaveAcceleratorsToFile(str);
		else
		{
			CString	strText;
			strText.Format(ResourceString(IDC_ICADCUSTOM__IS_NOT_A_VA_35, "The file \"%s\" is not a valid IntelliCAD Keyboard file"), str);
			m_CusKeyboard.MessageBox(strText, NULL, MB_ICONEXCLAMATION);
			return;
		}
	}

#ifdef _SUPPORT_CUSTOM_FORMS
	else if(pActivePage == &m_CusForm)		// Forms
	{
	}
	else if(pActivePage == &m_CusXlink)		// XLink
	{
	}
#endif

	else if(pActivePage == &m_Shortcuts)	// Aliases
	{
		resbuf	rb;
		CString	str;

		// EBATECH(CNBR) 2002/4/30 Disable TYPE button option+2
		if(sds_getfiled(ResourceString(IDC_ICADCUSTOM_SELECT_ALIAS_39, "Select Alias File"),
						""/*DNT*/,
						ResourceString(IDC_ICADCUSTOM_INTELLICAD_A_40, "IntelliCAD Alias File|ica,AutoCAD Alias File|pgp"),
						5+2,
						&rb) != RTNORM)
			return;

		if(rb.restype == RTSTR)
		{
			str = rb.resval.rstring;
			IC_FREE(rb.resval.rstring);
		}

		// EBATECH{2001.9.8 accept upper-case char.
		char drive[_MAX_DRIVE];
		char dir[_MAX_DIR];
		char fname[_MAX_FNAME];
		char ext[_MAX_EXT];
		_splitpath(str.GetBuffer(MAX_PATH), drive, dir, fname, ext);
		CString str2 = ext;
		str2.MakeUpper();
		// }EBATECH

		if(!m_Shortcuts.SaveAliasString())
			return;

		//if(str.Find(".pgp"/*DNT*/) != -1)
		if(str2.Find(".PGP"/*DNT*/) != -1) // EBATECH{2001.9.8}
			SDS_WriteAlias(str.GetBuffer(0));
		//else if(str.Find(".ica"/*DNT*/))
		else if(str2.Find(".ICA"/*DNT*/) != -1) // EBATECH{14-1-2002}
		{
			// Write the file (our format).
			m_Shortcuts.SaveAliasesToFile(str);
		}
		else
		{
			CString	strText;
			strText.Format(ResourceString(IDC_ICADCUSTOM__IS_NOT_A_VA_43, "The file \"%s\" is not a valid IntelliCAD Alias file"), str);
			m_Shortcuts.MessageBox(strText, NULL, MB_ICONEXCLAMATION);
			return;
		}
	}
}

BOOL
CIcadCustom::OnHelpInfo
(
 HELPINFO*	pHel
)
{
	OnHelp();
	return(CWnd::OnHelpInfo(pHel));
}

void
CIcadCustom::OnHelp()
{
	CommandRefHelp(m_hWnd, HLP_CUSTOMIZE);
	return;

/*	CPropertyPage* pActivePage = GetActivePage();
	if(pActivePage==(&m_CusMenu)) //*** Menu
    {
	::WinHelp(m_hWnd,fs1,HELP_CONTENTS,0);
    }
	else if(pActivePage==(&m_CusToolbar)) //*** Toolbars
    {
	::WinHelp(m_hWnd,fs1,HELP_CONTENTS,0);
    }
	else if(pActivePage==(&m_CusKeyboard))	//*** Keyboard
    {
	::WinHelp(m_hWnd,fs1,HELP_CONTENTS,0);
    }
	else if(pActivePage==(&m_Shortcuts)) //*** Aliases
    {
	::WinHelp(m_hWnd,fs1,HELP_CONTENTS,0);
    }
*/

}

afx_msg void
CIcadCustom::OnResetButton()
{
	CPropertyPage*	pActivePage = GetActivePage();

	if(pActivePage == &m_CusMenu)			// Menu
	{
		if(m_bQueryUser && m_CusMenu.MessageBox(ResourceString(IDC_ICADCUSMENU_ARE_YOU_SUR_59, "Are you sure you want to reset the menu?  This action replaces\nthe current menu with the default menu that is provided\nby IntelliCAD."), NULL, MB_YESNO | MB_ICONQUESTION) == IDNO)
			return;

		m_CusMenu.OnReset();
	}
	else if(pActivePage == &m_CusToolbar)	// Toolbars
	{
		if(m_CusToolbar.m_pTbMain)
			// Always do this when focus is taken away from drag and drop.
			m_CusToolbar.m_pTbMain->OnCustButtonDown(NULL, NULL, NULL, -1);

		/*D.Gavrilov*/// make this MessageBox as modal
		if( m_bQueryUser &&
			((CMainWindow*)m_pMain)->MessageBox(ResourceString(IDC_ICADCUSTOM_ARE_YOU_SURE_46, "Are you sure you want to reset the toolbars?  This action replaces the\ncurrent toolbar buttons with the default buttons that are provided by\nIntelliCAD."), NULL, MB_YESNO | MB_ICONQUESTION) == IDNO)
			return;

		m_CusToolbar.ResetButtons();
	}
	else if(pActivePage == &m_CusKeyboard)	// Keyboard
	{
		if(m_bQueryUser && m_CusKeyboard.MessageBox(ResourceString(IDC_ICADCUSTOM_ARE_YOU_SURE_49, "Are you sure you want to reset the keyboard assignments?  This action\nreplaces the list of accelerators with the default list that is provided\nby IntelliCAD."), NULL, MB_YESNO | MB_ICONQUESTION) == IDNO)
			return;

		m_CusKeyboard.SaveCommandString();
		((CMainWindow*)m_pMain)->LoadDefaultAccelList();
		m_CusKeyboard.ResetAccelList();
		((CMainWindow*)m_pMain)->m_bKeyboardModified = TRUE;
	}

#ifdef _SUPPORT_CUSTOM_FORMS
	else if(pActivePage == &m_CusForm)		// Forms
	{
	}
	else if(pActivePage == &m_CusXlink)		// XLink
	{
	}
#endif

	else if(pActivePage == &m_Shortcuts)	// Aliases
	{
		if(m_bQueryUser && m_Shortcuts.MessageBox(ResourceString(IDC_ICADCUSTOM_ARE_YOU_SURE_52, "Are you sure you want to reset the alias assignments?  This action\nreplaces the list of aliases with the default list that is provided\nby IntelliCAD."), NULL, MB_YESNO | MB_ICONQUESTION) == IDNO)
			return;

		SDS_FreeAlias();
		SDS_DefaultAlias();
		m_Shortcuts.ResetAliasList();
		((CMainWindow*)m_pMain)->m_bAliasModified = TRUE;
	}
}

void
CIcadCustom::DoDataExchange
(
 CDataExchange*	pDX
)
{
    CPropertySheet::DoDataExchange(pDX);
}


///////////////////////////////////////////////////////////////////////
// This function is called when an item to the (CTreeList) control
// is added.
//
// hParent   : Handle of the inserted item’s parent.
// szText    : The test that will be displayed for the items link.
// hInsAfter : Handle of the item after which the new item is to be inserted.
// iImage    : Index of the item’s image in the tree view control’s image list.
// SiImage   : Index of the item’s selected image in the tree view control’s image list.
// index     : the number associated with the item.
//
// HTREEITEM : Handle of the new item if successful; otherwise NULL.
//
HTREEITEM
AddOneItem
(
 HTREEITEM	hParent,
 LPCTSTR	szText,
 HTREEITEM	hInsAfter,
 int		iImage,
 int		SiImage,
 short		index,
 UINT		mask,
 CTreeCtrl*	TCtrl
)
{
	HTREEITEM		hItem;
	TV_ITEM			tvI;
	TV_INSERTSTRUCT	tvIns;

	tvI.mask	       = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM | mask;
	tvI.pszText	       = (char*)szText;
	tvI.cchTextMax     = lstrlen(szText);
	tvI.iImage		   = iImage;
	tvI.iSelectedImage = SiImage;
    tvI.lParam         = index;
	tvIns.item         = tvI;
	tvIns.hInsertAfter = hInsAfter;
	tvIns.hParent      = hParent;
	hItem              = TCtrl->InsertItem(&tvIns);

	return hItem;
}

inline void
VarToBinay
(
 BYTE**			c,
 COleVariant*	v
)
{
	if(v->vt == 8209)
	{
		*c = new BYTE [v->parray->rgsabound->cElements];		
		memcpy(*c, (BYTE *)v->parray->pvData, v->parray->rgsabound->cElements);
	}
	else
		*c = NULL;
}

inline void
VarToCStr
(
 CString*		c,
 COleVariant*	v
)
{
	if(v->vt == VT_BSTR)
		*c = (LPCTSTR)v->bstrVal;
	else
		*c = _T(""/*DNT*/);
}
