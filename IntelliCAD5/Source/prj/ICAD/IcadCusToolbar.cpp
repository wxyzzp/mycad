/* File  : <DevDir>\source\prj\icad\IcadCusToolbar.cpp
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 */


#include "Icad.h"/*DNT*/
#include "IcadCusToolbar.h"/*DNT*/
#include "IcadCustom.h"/*DNT*/
#include "IcadToolbarMain.h"/*DNT*/
#include "IcadCusToolBarAdvDlg.h"/*DNT*/
#include "IcadApp.h"/*DNT*/
#include "LicensedComponents.h"/*DNT*/

#include "ItcLanguage.h" // EBATECH

#ifdef _DEBUG

 #ifndef USE_SMARTHEAP
  #define new DEBUG_NEW
 #endif

#endif // _DEBUG

extern int			g_NumOfToolbars;
extern IcadToolBars	g_IcadToolBars[];
extern int			g_nMnuTbItems;
extern IcadCommands	g_MnuTbItems[];


/////////////////////////////////////////////////////////////////////////////
// CIcadCusToolbar property page
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CIcadCusToolbar, CPropertyPage)

CIcadCusToolbar::CIcadCusToolbar() : CPropertyPage(CIcadCusToolbar::IDD)
{
	//{{AFX_DATA_INIT(CIcadCusToolbar)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pCurSelTb = NULL;
}

CIcadCusToolbar::~CIcadCusToolbar()
{
}

void
CIcadCusToolbar::DoDataExchange
(
 CDataExchange*	pDX
)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIcadCusToolbar)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CIcadCusToolbar, CPropertyPage)
	//{{AFX_MSG_MAP(CIcadCusToolbar)
	ON_LBN_SELCHANGE(CUS_TOOLBAR_LIST, OnSelChangeList)
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(CUS_TOOLBAR_BTNADV, OnButtonAdvanced)
	ON_BN_CLICKED(CUS_TOOLBAR_BTNBMP2, OnButtonFromBitmap)
	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE(ICAD_CUSTOOL_BTNID_START, ICAD_CUSTOOL_BTNID_END, OnToolBarPick)
END_MESSAGE_MAP()


// CIcadCusToolbar message handlers

BOOL
CIcadCusToolbar::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// Fill the categories listbox.
	CListBox*	pList = (CListBox*)GetDlgItem(CUS_TOOLBAR_LIST);
	CStatic*	pBtnsWnd = (CStatic*)GetDlgItem(CUS_TOOLBAR_GRPBUTTONS);
	CWnd*		pFlyChkbox = GetDlgItem(CUS_TOOLBAR_CHKFLYOUT);

	// Disable the editboxes until a toolbar button is picked.
	CEdit	*pEditToolTip = (CEdit*)GetDlgItem(CUS_TOOLBAR_EDITTOOLTIP),
			*pEditHelpStr = (CEdit*)GetDlgItem(CUS_TOOLBAR_EDITHELPSTR),
			*pEditCommand = (CEdit*)GetDlgItem(CUS_TOOLBAR_EDITCOMMAND);

	pEditToolTip->EnableWindow(FALSE);
	pEditHelpStr->EnableWindow(FALSE);
	pEditCommand->EnableWindow(FALSE);

	// Get the rect of the window to be used to display the buttons.
	CRect	rectBtnsWnd, rectFlyChkbox;
	pBtnsWnd->GetWindowRect(rectBtnsWnd);
	pFlyChkbox->GetWindowRect(rectFlyChkbox);

	rectBtnsWnd.left += 12;
	rectBtnsWnd.right -= 4;
	rectBtnsWnd.top += 18;
	rectBtnsWnd.bottom = rectFlyChkbox.top;

	ScreenToClient(rectBtnsWnd);

	int		ct, idxNew;
    CString	tmpToolBarName;

	for(ct = 0; ct < g_NumOfToolbars; ++ct)
	{
        tmpToolBarName.LoadString(ICADTOOLBARS + ct);
		if(g_IcadToolBars[ct].FlyOut)
			continue;

		idxNew = pList->AddString(LPCTSTR(tmpToolBarName));
		CToolBarCtrl*	pToolBar = CreateDisplayToolBar(rectBtnsWnd, g_IcadToolBars[ct].lToolID, ct);
		pList->SetItemDataPtr(idxNew, pToolBar);
	}

	// Select the first item in the list.
	pList->SetCurSel(0);
	// Call this message to display the buttons because SetCurSel() does not call it.
	OnSelChangeList();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

CToolBarCtrl*
CIcadCusToolbar::CreateDisplayToolBar
(
 CRect	rectWnd,
 //long	lToolID,
 long_double	lToolID,
 int	nID
)
{
	/*D.G.*/// accessibility licensed components commands checking added

	// Create the toolbar.
	CIcadToolBarCtrl*	pToolBar = new CIcadToolBarCtrl();
	pToolBar->m_pTbMain = m_pTbMain;

	DWORD	dwStyle = WS_CHILD | CCS_NORESIZE | CCS_NODIVIDER | CCS_NOHILITE | TBSTYLE_WRAPABLE | TBSTYLE_FLAT;
	if(m_pTbMain->m_bToolTips)
		dwStyle |= TBSTYLE_TOOLTIPS;

	pToolBar->Create(dwStyle, rectWnd, this, nID);

	// Get the number of buttons for the toolbar.
	int		ct, nButtons;
	BOOL	bJustAddedSpcr = TRUE;

	for(ct = 0, nButtons = 0; ct < g_nMnuTbItems; ++ct)
	{
		if(!(lToolID & g_MnuTbItems[ct].lToolID))
			continue;

		// If the item is a separator.
    // TECNOBIT(Todeschini) 2003.13.05 - [ Prevent displaying of place holder icons on the dialog
    // if(!(*g_MnuTbItems[ct].ToolTip) && !(*g_MnuTbItems[ct].Command))
		if(!(*g_MnuTbItems[ct].ToolTip) && !(*g_MnuTbItems[ct].Command) || g_MnuTbItems[ct].lflgVisibility & ICAD_MN_CTRL)
    // ]-
		{
			// Only add a spacer if the previous button is not a spacer.
			if(!bJustAddedSpcr)
				bJustAddedSpcr = TRUE;
			continue;
		}

		// Check for the accessibility of the licensed components commands.
		if(!CIcadApp::m_LicensedComponents.IsCommandAccessible(g_MnuTbItems[ct].Command))
			continue;

		if(g_MnuTbItems[ct].lFlyOutID)
		{
			// Add a spacer before the flyout, but only if there is
			// not a spacer already there.
			if(!bJustAddedSpcr)
				bJustAddedSpcr = TRUE;

			// Add the flyout buttons.
			int	ctFly;
			for(ctFly = 0; ctFly < g_nMnuTbItems; ++ctFly)
			{
				if(!(g_MnuTbItems[ctFly].lToolID & g_MnuTbItems[ct].lFlyOutID))
					continue;
				++nButtons;
				bJustAddedSpcr = FALSE;
			}

			// Add a spacer after the flyout.
			if(!bJustAddedSpcr)
				bJustAddedSpcr = TRUE;
			continue;
		}

		++nButtons;
		bJustAddedSpcr = FALSE;
	}	// for(

	// Allocate the array of button structures.
	LPTBBUTTON	pTbButton = new TBBUTTON[nButtons];
	memset(pTbButton, 0, sizeof(TBBUTTON) * nButtons);

	// Fill the array of button structures.
	int	ctButton, ctImage;

	for(ct = 0, ctButton = 0, ctImage = 0; ct < g_nMnuTbItems; ++ct)
	{
		if(!(lToolID & g_MnuTbItems[ct].lToolID))
			continue;

		// If the item is a separator.
    // TECNOBIT(Todeschini) 2003.13.05 - [ Prevent displaying of place holder icons on the dialog
    // if(!(*g_MnuTbItems[ct].ToolTip) && !(*g_MnuTbItems[ct].Command))
		if(!(*g_MnuTbItems[ct].ToolTip) && !(*g_MnuTbItems[ct].Command) || g_MnuTbItems[ct].lflgVisibility & ICAD_MN_CTRL)
			continue;
    // ]-

		// Check for the accessibility of the licensed components commands.
		if(!CIcadApp::m_LicensedComponents.IsCommandAccessible(g_MnuTbItems[ct].Command))
			continue;

		if(g_MnuTbItems[ct].lFlyOutID)
		{
			// Add the flyout buttons.
			int	ctFly;
			for(ctFly = 0; ctFly < g_nMnuTbItems; ++ctFly)
			{
				if(!(g_MnuTbItems[ctFly].lToolID & g_MnuTbItems[ct].lFlyOutID))
					continue;

				pTbButton[ctButton].iBitmap = pToolBar->AddBitmap(1, g_MnuTbItems[ctFly].BMPIdSmColor);
				pTbButton[ctButton].idCommand = ICAD_CUSTOOL_BTNID_START + ctFly;
				pTbButton[ctButton].fsState = TBSTATE_ENABLED;
				pTbButton[ctButton].fsStyle = TBSTYLE_BUTTON;
				pTbButton[ctButton].dwData = 0;
				pTbButton[ctButton].iString = 0;
				++ctButton;
			}
			continue;
		}

		pTbButton[ctButton].iBitmap = pToolBar->AddBitmap(1, g_MnuTbItems[ct].BMPIdSmColor);
		pTbButton[ctButton].idCommand = ICAD_CUSTOOL_BTNID_START + ct;
		pTbButton[ctButton].fsState = TBSTATE_ENABLED;
		pTbButton[ctButton].fsStyle = TBSTYLE_BUTTON;
		pTbButton[ctButton].dwData = 0;
		pTbButton[ctButton].iString = 0;
		++ctButton;
	}	// for(

	pToolBar->AddButtons(nButtons, pTbButton);
	delete pTbButton;
	return pToolBar;
}

void
CIcadCusToolbar::OnSelChangeList()
{
	CListBox*		pList = (CListBox*)GetDlgItem(CUS_TOOLBAR_LIST);
	// Get the tool id of the new item
	int				idxCur = pList->GetCurSel();
	CToolBarCtrl*	pToolBar = (CToolBarCtrl*)pList->GetItemDataPtr(idxCur);
	if(pToolBar == m_pCurSelTb)
		return;

	//* Hide the previously selected toolbar and display the new one.
	if(m_pCurSelTb)
		m_pCurSelTb->ShowWindow(SW_HIDE);
	pToolBar->ShowWindow(SW_SHOW);
	m_pCurSelTb = pToolBar;
}

void
CIcadCusToolbar::OnToolBarPick
(
 UINT	nID
)
{
	// These messages are being handled in the OnTbButtonPick() function that gets
	// called from the OnLButtonDown function in the CIcadToolBarCtrl class.
}

void
CIcadCusToolbar::OnTbButtonPick
(
 UINT	nID
)
{
	CEdit	*pEditToolTip = (CEdit*)GetDlgItem(CUS_TOOLBAR_EDITTOOLTIP),
			*pEditHelpStr = (CEdit*)GetDlgItem(CUS_TOOLBAR_EDITHELPSTR),
			*pEditCommand = (CEdit*)GetDlgItem(CUS_TOOLBAR_EDITCOMMAND);

	// Enable the Advanced... button.
	CButton*	pAdvButton = (CButton*)GetDlgItem(CUS_TOOLBAR_BTNADV);
	if(pAdvButton->IsWindowEnabled())
		pAdvButton->EnableWindow(FALSE);

	// Disable the editboxes
	if(pEditToolTip->IsWindowEnabled())
	{
		pEditToolTip->EnableWindow(FALSE);
		pEditHelpStr->EnableWindow(FALSE);
		pEditCommand->EnableWindow(FALSE);
	}

	// The localized tooltip is stored one beyond the resource index, if the pAcadTbStr is available.
	int	resourceIndex = g_MnuTbItems[nID - ICAD_CUSTOOL_BTNID_START].resourceIndex + (g_MnuTbItems[nID - ICAD_CUSTOOL_BTNID_START].pAcadTbStr? 1 : 0);

	if(g_MnuTbItems[nID - ICAD_CUSTOOL_BTNID_START].ToolTip)
	{
		CString	localizedToolTip;
		localizedToolTip.LoadString(resourceIndex++);	// Increment resourceIndex for help string.
		pEditToolTip->SetWindowText(localizedToolTip);
	}

	if(g_MnuTbItems[nID-ICAD_CUSTOOL_BTNID_START].HelpString)
	{
		CString	localizedHelpString;
		localizedHelpString.LoadString(resourceIndex);
		pEditHelpStr->SetWindowText(localizedHelpString);
	}

	if(g_MnuTbItems[nID-ICAD_CUSTOOL_BTNID_START].Command)
		pEditCommand->SetWindowText(g_MnuTbItems[nID - ICAD_CUSTOOL_BTNID_START].Command);
}

void
CIcadCusToolbar::OnDestroy()
{
	CPropertyPage::OnDestroy();

	// Make sure the last button that was pressed gets unpressed and the
	// variables in CIcadToolBarMain get set to NULL.
	m_pTbMain->OnCustButtonDown(NULL, NULL, NULL, -1);

	// Delete the CToolBarCtrl for each item in the listbox.
	CListBox*	pList = (CListBox*)GetDlgItem(CUS_TOOLBAR_LIST);
	int			nItems = pList->GetCount(), ct;

	for(ct = 0; ct < nItems; ++ct)
	{
		CToolBarCtrl*	pToolBar = (CToolBarCtrl*)pList->GetItemDataPtr(ct);
		delete pToolBar;
	}
}

void
CIcadCusToolbar::OnMouseMove
(
 UINT	nFlags,
 CPoint	point
)
{
	CWnd*	pWnd = WindowFromPoint(point);

	CPropertyPage::OnMouseMove(nFlags, point);
}

BOOL
CIcadCusToolbar::OnSetActive()
{
	m_pTbMain->m_pParentFrame->EndModalState();
	m_pTbMain->SetModifiedFlag();

	return CPropertyPage::OnSetActive();
}

BOOL
CIcadCusToolbar::OnKillActive()
{
	// Make sure the last button that was pressed gets unpressed and the
	// variables in CIcadToolBarMain get set to NULL.
	m_pTbMain->OnCustButtonDown(NULL, NULL, NULL, -1);

	// Clear and disable the editboxes.
	ResetEditboxes();
	m_pTbMain->m_pParentFrame->BeginModalState();
	SDS_CMainWindow->m_pIcadCustom->EnableWindow();

	// Remove this when we have something to export. ~| TODO
	((CIcadCustom*)SDS_CMainWindow->m_pIcadCustom)->m_ExportBtn.EnableWindow();

	return CPropertyPage::OnKillActive();
}

void
CIcadCusToolbar::ResetEditboxes()
{
	CEdit	*pEditToolTip = (CEdit*)GetDlgItem(CUS_TOOLBAR_EDITTOOLTIP),
			*pEditHelpStr = (CEdit*)GetDlgItem(CUS_TOOLBAR_EDITHELPSTR),
			*pEditCommand = (CEdit*)GetDlgItem(CUS_TOOLBAR_EDITCOMMAND);

	pEditToolTip->SetWindowText(""/*DNT*/);
	pEditHelpStr->SetWindowText(""/*DNT*/);
	pEditCommand->SetWindowText(""/*DNT*/);

	pEditToolTip->EnableWindow(FALSE);
	pEditHelpStr->EnableWindow(FALSE);
	pEditCommand->EnableWindow(FALSE);

	// Enable the Advanced... button.
	CButton*	pAdvButton = (CButton*)GetDlgItem(CUS_TOOLBAR_BTNADV);
	pAdvButton->EnableWindow(FALSE);
}

void
CIcadCusToolbar::ResetButtons()
{
	// Get a pointer to the toolbar list.
	if(!m_pTbMain)
		return;

	CPtrList*	pTbList = m_pTbMain->GetToolBarList();
	if(!pTbList)
		return;

	int		ct;
	UINT	idxToolBar = 0;
	CIcadToolBar*	pToolBar = NULL;

	// First delete all the non-default toolbars
	POSITION	pos = pTbList->GetHeadPosition();
	while(pos)
	{
		pToolBar = (CIcadToolBar*)pTbList->GetAt(pos);
		if(!pToolBar)
			continue;

		if(!pToolBar->m_lDefID)
		{
			// If the default toolbar ID is 0, delete the toolbar.
			POSITION	posDel = pos;

			delete pToolBar;
			pTbList->GetNext(pos);
			pTbList->RemoveAt(posDel);
			continue;
		}

		idxToolBar = max(idxToolBar, pToolBar->m_nID - IDR_ICAD_TOOLBAR0);
		pTbList->GetNext(pos);
	}

	if(idxToolBar > 0)
		++idxToolBar;

	pToolBar = NULL;
	BOOL		bAddedNewTb = FALSE;
	POSITION	posCurrent = NULL;

	// Reset and add the default toolbars.
	for(ct = 0; ct < g_NumOfToolbars; ++ct)
	{
		if(g_IcadToolBars[ct].FlyOut)
			continue;

		// Loop through the list of toolbars to find the existing toolbar with the
		// matching ID.
		pos = pTbList->GetHeadPosition();
        CString	tmpToolBarName;

		while(pos)
		{
			pToolBar = (CIcadToolBar*)pTbList->GetAt(pos);
			if(pToolBar && pToolBar->m_lDefID == g_IcadToolBars[ct].lToolID)
			{
				posCurrent = pos;
				break;
			}

			pToolBar = NULL;
			pTbList->GetNext(pos);
		}

		if(!pToolBar)
		{
            tmpToolBarName.LoadString(ICADTOOLBARS + ct);
			// Create a new toolbar because it has been deleted.
			pToolBar = m_pTbMain->CreateNewToolBar(idxToolBar, g_IcadToolBars[ct].lToolID, LPCTSTR(tmpToolBarName));
			if(!pToolBar)
				continue;

			posCurrent = pTbList->AddTail(pToolBar);
			bAddedNewTb = TRUE;
		}

		// Reset the buttons for this toolbar.
		CToolBarCtrl&	TbCtrl = pToolBar->GetToolBarCtrl();
		// Delete the old buttons.
		int				nButtons = TbCtrl.GetButtonCount(), ct;
		TBBUTTON		tbButton;
		CIcadTBbutton*	picButton;

		for(ct = 0; ct < nButtons; ++ct)
		{
			if(TbCtrl.GetButton(0, &tbButton))
			{
				picButton = (CIcadTBbutton*)tbButton.dwData;
				m_pTbMain->FreeIcadButtonData(picButton);
				TbCtrl.DeleteButton(0);
			}
		}

		// Delete the last flyout
		delete pToolBar->m_pFlyOut;
		pToolBar->m_pFlyOut = NULL;

		// Create the new buttons from the header file struct thingie.
		if(!m_pTbMain->CreateDefaultButtons(pToolBar, pToolBar->m_lDefID))
		{
			// This toolbar doesn't have any buttons that will ever be visible,
			// so delete it.
			delete pToolBar;
			pTbList->RemoveAt(posCurrent);
			bAddedNewTb = FALSE;
			continue;
		}

		if(bAddedNewTb)
		{
#ifndef BUILD_EBATECH_JAPAN
			// ITC original toolbar layout.
			BOOL	bShow = TRUE;
			// Now that the buttons are created enable docking.
			pToolBar->EnableDocking(CBRS_ALIGN_ANY);

			if(idxToolBar > 2 && idxToolBar != 9)
			{
				m_pTbMain->m_pParentFrame->DockControlBar(pToolBar, AFX_IDW_DOCKBAR_TOP);
				bShow = FALSE;
			}
			else
				if(idxToolBar == 9)
					m_pTbMain->m_pParentFrame->DockControlBar(pToolBar, AFX_IDW_DOCKBAR_RIGHT);
				else
					m_pTbMain->m_pParentFrame->DockControlBar(pToolBar, AFX_IDW_DOCKBAR_TOP);

			m_pTbMain->m_pParentFrame->ShowControlBar(pToolBar, bShow, FALSE);
#else
			// This is EBATECH-JAPAN original toolbar layout.
			// require 'IcadMenuTBDef.h'
			BOOL bShow = TRUE;
			CRect rect;
			CControlBar* pToolBarPre;
			pToolBar->EnableDocking(CBRS_ALIGN_ANY);
			switch (idxToolBar){
			case 0:
				m_pTbMain->m_pParentFrame->DockControlBar(pToolBar,AFX_IDW_DOCKBAR_TOP);
				break;
			case 1:
				m_pTbMain->m_pParentFrame->DockControlBar(pToolBar,AFX_IDW_DOCKBAR_RIGHT);
				break;
			case 2:
				m_pTbMain->m_pParentFrame->DockControlBar(pToolBar,AFX_IDW_DOCKBAR_LEFT);
				break;
			case 3:
				m_pTbMain->m_pParentFrame->DockControlBar(pToolBar,AFX_IDW_DOCKBAR_LEFT);
				break;
			case 4:
				m_pTbMain->m_pParentFrame->DockControlBar(pToolBar,AFX_IDW_DOCKBAR_BOTTOM);
				pToolBarPre = pToolBar;
				break;
			case 5:
				m_pTbMain->m_pParentFrame->RecalcLayout();
				pToolBarPre->GetWindowRect(&rect);
				rect.OffsetRect(1,1);
				m_pTbMain->m_pParentFrame->DockControlBar(pToolBar,AFX_IDW_DOCKBAR_BOTTOM,&rect);
				break;
			default:
				m_pTbMain->m_pParentFrame->DockControlBar(pToolBar,AFX_IDW_DOCKBAR_TOP);
				bShow = FALSE;
				break;
			}
			m_pTbMain->m_pParentFrame->ShowControlBar(pToolBar,bShow,FALSE);
#endif  // BUILD_EBATECH_JAPAN

			bAddedNewTb = FALSE;
		}

		++idxToolBar;
	}	// for(

	// Update the visibilty of the buttons since when they are created they are not updated.
	m_pTbMain->OnVisibilityChanged();
}

void
CIcadCusToolbar::OnButtonAdvanced()
{
	if(!m_pTbMain || (!m_pTbMain->m_pToolBarCurBtnDown && !m_pTbMain->m_pFlyOutCurBtnDown))
	{
		sds_alert(ResourceString(IDC_ICADCUSTOOLBAR_A_BUTTON__1, "A button has not yet been selected."));
		return;
	}

	CIcadCusToolBarAdvDlg	pAdvDlg(this);
	pAdvDlg.DoModal();
}

void
CIcadCusToolbar::OnButtonFromBitmap()
{
	resbuf	rb;
	CString	str;

	// EBATECH(CNBR) 2002/4/30 Disable TYPE button option+2
    if(sds_getfiled(ResourceString(IDC_ICADCUSTOOLBAR_SELECT_BI_2, "Select Bitmap File"),
					ResourceString(IDC_ICADCUSTOOLBAR__0, ""),
					ResourceString(IDC_ICADCUSTOOLBAR_WINDOWS_B_3, "Windows Bitmap Image|bmp"),
					4+2, &rb) != RTNORM)
					return;

	if(rb.restype == RTSTR)
	{
		str = rb.resval.rstring;
		IC_FREE(rb.resval.rstring);
	}

	sds_alert(ResourceString(IDC_ICADCUSTOOLBAR_SORRY__TH_4, "Sorry, this function is not yet completed."));
}

void
CIcadCusToolbar::OnExit()
{
	m_pTbMain->OnCustButtonDown(NULL, NULL, NULL, -1);
	if(m_pTbMain)
	{
		// Save the toolbars to the registry, then redraw.
		m_pTbMain->SaveToRegistry();
		m_pTbMain->OnVisibilityChanged();
	}
}
