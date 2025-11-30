/* File  : <DevDir>\source\prj\icad\IcadCusToolbar1.cpp
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 */ 

#include "Icad.h"/*DNT*/
#include "IcadCusToolbar.h"/*DNT*/
#include "IcadCustom.h"/*DNT*/
#include "IcadToolbarMain.h"/*DNT*/

#ifdef _DEBUG

 #ifndef USE_SMARTHEAP
  #define new DEBUG_NEW
 #endif

#endif

extern int			g_NumOfToolbars;
extern IcadToolBars	g_IcadToolBars[];
extern IcadCommands	g_MnuTbItems[];

/////////////////////////////////////////////////////////////////////////////
// CIcadToolBarCtrl
/////////////////////////////////////////////////////////////////////////////

CIcadToolBarCtrl::CIcadToolBarCtrl()
{
	m_prevCursor = NULL;
}

CIcadToolBarCtrl::~CIcadToolBarCtrl()
{
}

BEGIN_MESSAGE_MAP(CIcadToolBarCtrl, CToolBarCtrl)
	//{{AFX_MSG_MAP(CIcadToolBarCtrl)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipText)
	ON_NOTIFY_EX(TTN_NEEDTEXTW, 0, OnToolTipTextW)
END_MESSAGE_MAP()


// CIcadToolBarCtrl message handlers

void
CIcadToolBarCtrl::OnLButtonDown
(
 UINT	nFlags,
 CPoint	point
)
{
	// Get the button located at the point the left button was pressed at.
	int	idxButton;
	if( (idxButton = m_pTbMain->GetButtonFromPt(this, point, &m_CurDragButton)) != -1 && 
		!(m_CurDragButton.fsStyle & TBSTYLE_SEP) )
	{
		m_pTbMain->OnCustButtonDown(NULL, NULL, this, idxButton);
		PressButton(m_CurDragButton.idCommand);

		CIcadCusToolbar*	pCusToolbarDlg = (CIcadCusToolbar*)GetParent();

		pCusToolbarDlg->OnTbButtonPick(m_CurDragButton.idCommand);
		m_prevCursor = ::SetCursor(AfxGetApp()->LoadCursor(IDC_CUSTTOOLBARADD));

		// Set the flyout variable.
		CButton*	pFlyOutButton = (CButton*)pCusToolbarDlg->GetDlgItem(CUS_TOOLBAR_CHKFLYOUT);
		if(pFlyOutButton->GetCheck())
			m_bInsAsFlyOut = TRUE;
		else
			m_bInsAsFlyOut = FALSE;
		SetCapture();
	}
}

void
CIcadToolBarCtrl::OnMouseMove
(
 UINT	nFlags,
 CPoint	point
)
{
	if(!(nFlags & MK_LBUTTON))
	{
		CToolBarCtrl::OnMouseMove(nFlags, point);
		return;
	}

	// Get the window that the mouse is being dragged over.
	CPoint	ptScrn(point);
	ClientToScreen(&ptScrn);

	CWnd*	pWnd = WindowFromPoint(ptScrn);
	if(!pWnd || !pWnd->IsKindOf(RUNTIME_CLASS(CIcadToolBar)))
	{
		m_pTbMain->DrawInsertMark(NULL, NULL, ptScrn, m_bInsAsFlyOut);
		return;
	}

	// Check to see if the mouse is being dragged over a flyout button.
	CIcadToolBar*	pToolBar = (CIcadToolBar*)pWnd;
	CPoint			ptToolBar(ptScrn);
	pToolBar->ScreenToClient(&ptToolBar);

	// If the mouse is being dragged over a flyout button display the flyout.
	m_pTbMain->DrawInsertMark(pToolBar, NULL, ptToolBar, m_bInsAsFlyOut);
	pToolBar->CreateFlyOut(ptToolBar);
	return;
}

void
CIcadToolBarCtrl::OnLButtonUp
(
 UINT	nFlags,
 CPoint	point
)
{
	// Return if a button was not being dragged.
    if(!m_prevCursor)
	{
		ReleaseCapture();
		return;
	}

	// Set the cursor back to what is was before OnLButtonDown.
	::SetCursor(m_prevCursor);
	m_prevCursor = NULL;

	// Get the window that the left button was released on.
	CPoint			ptScrn(point);
	ClientToScreen(&ptScrn);
	CWnd*			pWnd = WindowFromPoint(ptScrn);
	CIcadToolBar*	pToolBar;
	CToolBarCtrl*	pTbCtrl;
	int				idxNewButton;

	if(!pWnd || !pWnd->IsKindOf(RUNTIME_CLASS(CIcadToolBar)))
	{
		if(pWnd == this || pWnd->IsKindOf(RUNTIME_CLASS(CIcadCusToolbar)))
		{
			ReleaseCapture();
			return;
		}

		CPtrList*	pToolBarList = m_pTbMain->GetToolBarList();
		// Get the count of standard toolbars (not including flyouts).
		CString		strText;
		int			ct, nStdToolBars = 0;

		for(ct = 0; ct < g_NumOfToolbars; ++ct)
		{
			if(!g_IcadToolBars[ct].FlyOut)
				continue;
			++nStdToolBars;
		}

		// Get the name of the toolbar.
		strText.Format(ResourceString(IDC_ICADCUSTOOLBAR1_TOOLBAR__0, "ToolBar%d"), pToolBarList->GetCount()-nStdToolBars + 1);
		// Create a new toolbar.
		CIcadToolBar*	pTbTemp = NULL;
		UINT			idxToolBar = 0;
		POSITION		pos = pToolBarList->GetHeadPosition();

		while(pos)
		{
			pTbTemp = (CIcadToolBar*)pToolBarList->GetAt(pos);
			if(!pTbTemp)
				continue;

			idxToolBar = max(idxToolBar, pTbTemp->m_nID - IDR_ICAD_TOOLBAR0);
			pToolBarList->GetNext(pos);
		}

		if(idxToolBar > 0)
			++idxToolBar;

		pToolBar = m_pTbMain->CreateNewToolBar(idxToolBar, 0, strText);
		if(!pToolBar)
		{
			ReleaseCapture();
			return;
		}

		pToolBarList->AddTail(pToolBar);
		pTbCtrl = &(pToolBar->GetToolBarCtrl());
		// Add the button to the toolbar.
		idxNewButton = m_pTbMain->AddDraggedButton(this, pToolBar, NULL, ptScrn);
		// Set the sizes for the buttons for this toolbar.
		if(m_pTbMain->m_bLargeButtons)
		{
			pTbCtrl->SetBitmapSize(CSize(ICAD_LG_IMGX, ICAD_LG_IMGY));
			pTbCtrl->SetButtonSize(CSize(ICAD_LG_BUTTONX, ICAD_LG_BUTTONY));
		}
		else
		{
			pTbCtrl->SetBitmapSize(CSize(ICAD_SM_IMGX, ICAD_SM_IMGY));
			pTbCtrl->SetButtonSize(CSize(ICAD_SM_BUTTONX, ICAD_SM_BUTTONY));
		}

		// Enable docking.
		pToolBar->EnableDocking(CBRS_ALIGN_ANY);
		// Set the position of the new toolbar.
		m_pTbMain->m_pParentFrame->ShowControlBar(pToolBar, TRUE, TRUE);
		m_pTbMain->m_pParentFrame->FloatControlBar(pToolBar, ptScrn, CBRS_ALIGN_TOP);
	}
	else
	{
		// The button was released on a toolbar.
		pToolBar = (CIcadToolBar*)pWnd;
		pTbCtrl = (&(pToolBar->GetToolBarCtrl()));
		if(m_bInsAsFlyOut)
		{
			// Create a flyout out of the two buttons.
			CPoint			ptToolBar(ptScrn);
			pTbCtrl->ScreenToClient(&ptToolBar);
			TBBUTTON		tbButton;
			CIcadTBbutton*	pibOldParent;
			int				idxNewFlyOutBtn = m_pTbMain->GetButtonFromPt(pTbCtrl, ptToolBar, &tbButton);

			pibOldParent = (CIcadTBbutton*)tbButton.dwData;
			if(idxNewFlyOutBtn == -1 || !pibOldParent)
			{
				ReleaseCapture();
				return;
			}

			// Initialize the new button.
			CIcadTBbutton*	picButton = new CIcadTBbutton;
			picButton->m_idToolBar = 0;	// TODO ~|
			picButton->m_bAddSpacerBefore = FALSE;
			picButton->m_bChgParentBtnImg = TRUE;
			// Use the m_CurDragButton member to get the index into the g_MnuTbItems
			// array for the rest of the new button's information.
			int	idxItems = m_CurDragButton.idCommand - ICAD_CUSTOOL_BTNID_START;

			// The localized tooltip is stored one beyond the resource index, if the pAcadTbStr is available.
			int	resourceIndex = g_MnuTbItems[idxItems].resourceIndex + (g_MnuTbItems[idxItems].pAcadTbStr? 1 : 0);

			picButton->m_toolTip.LoadString(resourceIndex++);	// Increment resource index for help string.
			picButton->m_helpString.LoadString(resourceIndex);
			picButton->m_command = g_MnuTbItems[idxItems].Command;

			picButton->m_BMPIdSmColor = g_MnuTbItems[idxItems].BMPIdSmColor;
			picButton->m_BMPIdLgColor = g_MnuTbItems[idxItems].BMPIdLgColor;
			picButton->m_BMPIdSmBW = g_MnuTbItems[idxItems].BMPIdSmBW;
			picButton->m_BMPIdLgBW = g_MnuTbItems[idxItems].BMPIdLgBW;

			picButton->m_idxFlyOutImage = m_pTbMain->AddIcadBitmap(picButton, 1, pToolBar);

			if(pibOldParent->m_pibFlyOut)
			{
				// The parent button is a flyout with all its buttons hidden because
				// of the user level.
				picButton->m_lflgVisibility = pibOldParent->m_pibCurFlyOut->m_lflgVisibility;
				tbButton.iBitmap = pibOldParent->m_pibCurFlyOut->m_idxFlyOutImage;
				// Add the new flyout onto the end of the existing list of flyouts.
				CIcadTBbutton*	pibCur = pibOldParent->m_pibFlyOut;
				while(pibCur->m_pibFlyOut)
					pibCur = pibCur->m_pibFlyOut;

				pibCur->m_pibFlyOut = picButton;
			}
			else
			{
				picButton->m_lflgVisibility = pibOldParent->m_lflgVisibility;
				// Create a new IcadButton structure for the parent button.
				CIcadTBbutton*	pibNewParent = new CIcadTBbutton;
				pibNewParent->m_bAddSpacerBefore = pibOldParent->m_bAddSpacerBefore;
				pibNewParent->m_bChgParentBtnImg = TRUE;
				pibNewParent->m_idxFlyOutImage = -1;
				pibNewParent->m_pibFlyOut = pibNewParent->m_pibCurFlyOut = pibOldParent;

				tbButton.iBitmap = pibOldParent->m_idxFlyOutImage = m_pTbMain->AddIcadBitmap(pibOldParent, 1, pToolBar);
				tbButton.dwData = (DWORD)pibNewParent;
				// Add the new button onto the flyout list.
				pibOldParent->m_pibFlyOut = picButton;
				pibOldParent->m_pibCurFlyOut = NULL;
			}

			// Delete the old parent button so we can insert the new one containing
			// the flyout information.
			pTbCtrl->DeleteButton(idxNewFlyOutBtn);
			pTbCtrl->InsertButton(idxNewFlyOutBtn, &tbButton);

			// Display the flyout with the new button pressed down.
			pToolBar->CreateFlyOut(ptToolBar);
			if(pToolBar->m_pFlyOut->m_bImagesReversed)
				idxNewFlyOutBtn = m_pTbMain->GetFirstVisibleButtonIndex(pToolBar->m_pFlyOut);
			else
				idxNewFlyOutBtn = m_pTbMain->GetLastVisibleButtonIndex(pToolBar->m_pFlyOut);

			pToolBar->m_pFlyOut->GetButton(idxNewFlyOutBtn, &tbButton);
			pToolBar->m_pFlyOut->PressButton(tbButton.idCommand);

			m_pTbMain->OnCustButtonDown(NULL, pToolBar->m_pFlyOut, NULL, idxNewFlyOutBtn);

			// Enable the Advanced... button.
			CButton*	pAdvButton = (CButton*)m_pTbMain->m_pCusTbDlg->GetDlgItem(CUS_TOOLBAR_BTNADV);
			pAdvButton->EnableWindow();

			// Enable the customize editboxes.
			CEdit	*pEditToolTip = (CEdit*)m_pTbMain->m_pCusTbDlg->GetDlgItem(CUS_TOOLBAR_EDITTOOLTIP),
					*pEditHelpStr = (CEdit*)m_pTbMain->m_pCusTbDlg->GetDlgItem(CUS_TOOLBAR_EDITHELPSTR),
					*pEditCommand = (CEdit*)m_pTbMain->m_pCusTbDlg->GetDlgItem(CUS_TOOLBAR_EDITCOMMAND);

			pEditToolTip->EnableWindow();
			pEditHelpStr->EnableWindow();
			pEditCommand->EnableWindow();

			ReleaseCapture();
			return;
		}

		// Add the button to the toolbar.
		idxNewButton = m_pTbMain->AddDraggedButton(this, pToolBar, NULL, ptScrn);
	}

	// Set the ID of the current pressed button in the toolbar with the new button.
	m_pTbMain->m_idxCurButtonDown = idxNewButton;
	m_pTbMain->m_pToolBarCurBtnDown = pToolBar;
	m_pTbMain->m_pFlyOutCurBtnDown = NULL;

	// Enable the Advanced... button.
	CButton*	pAdvButton = (CButton*)m_pTbMain->m_pCusTbDlg->GetDlgItem(CUS_TOOLBAR_BTNADV);
	pAdvButton->EnableWindow();

	// Enable the customize editboxes.
	CEdit	*pEditToolTip = (CEdit*)m_pTbMain->m_pCusTbDlg->GetDlgItem(CUS_TOOLBAR_EDITTOOLTIP),
			*pEditHelpStr = (CEdit*)m_pTbMain->m_pCusTbDlg->GetDlgItem(CUS_TOOLBAR_EDITHELPSTR),
			*pEditCommand = (CEdit*)m_pTbMain->m_pCusTbDlg->GetDlgItem(CUS_TOOLBAR_EDITCOMMAND);

	pEditToolTip->EnableWindow();
	pEditHelpStr->EnableWindow();
	pEditCommand->EnableWindow();

	// Redraw the toolbars.
	(pToolBar->GetParentFrame())->RecalcLayout();
	ReleaseCapture();
}

BOOL
CIcadToolBarCtrl::OnToolTipText
(
 UINT		nID,
 NMHDR*		pNMHDR,
 LRESULT*	pResult
)
{
	TOOLTIPTEXT*	pTTT = (TOOLTIPTEXT*)pNMHDR;
	char*			pToolTip = g_MnuTbItems[pTTT->hdr.idFrom - ICAD_CUSTOOL_BTNID_START].ToolTip;

	if(pToolTip)
	{
		int		resourceIndex = g_MnuTbItems[pTTT->hdr.idFrom - ICAD_CUSTOOL_BTNID_START].resourceIndex +
								(g_MnuTbItems[pTTT->hdr.idFrom - ICAD_CUSTOOL_BTNID_START].pAcadTbStr? 1 : 0);
		CString	localizedToolTip;
		localizedToolTip.LoadString(resourceIndex);
		lstrcpyn(pTTT->szText, localizedToolTip, sizeof(pTTT->szText) - 1);
	}

	return TRUE;
}

BOOL
CIcadToolBarCtrl::OnToolTipTextW
(
 UINT		nID,
 NMHDR*		pNMHDR,
 LRESULT*	pResult
)
{
	// This function is for Windows NT.
	TOOLTIPTEXTW*	pTTT = (TOOLTIPTEXTW*)pNMHDR;
	char*			pToolTip = g_MnuTbItems[pTTT->hdr.idFrom - ICAD_CUSTOOL_BTNID_START].ToolTip;

	if(pToolTip)
	{
		int		resourceIndex = g_MnuTbItems[pTTT->hdr.idFrom - ICAD_CUSTOOL_BTNID_START].resourceIndex +
								(g_MnuTbItems[pTTT->hdr.idFrom - ICAD_CUSTOOL_BTNID_START].pAcadTbStr? 1 : 0);
		CString	localizedToolTip;
		localizedToolTip.LoadString(resourceIndex);
		mbstowcs(pTTT->szText, localizedToolTip, sizeof(pTTT->szText) / sizeof(wchar_t));
	}

	return TRUE;
}
