/* F:\ICADDEV\PRJ\ICAD\ICADDIALOG.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "icad.h"
#include "icaddialog.h"

const int DIST_FROM_EDGE_WIDTH = 14;
const int DIST_FROM_EDGE_HEIGHT =14;
const int DIST_BETWEEN_BUTTONS = 6;

BOOL IcadDialog::OnInitDialog()
	{
	BOOL bResult = CDialog::OnInitDialog();

	/* Not repositioning buttons because it causes too many problems with current dialogs	
	CRect dialogRect;
	GetWindowRect(dialogRect);
	ScreenToClient(dialogRect);

	CRect cancelRect, newCancelRect;
	CWnd *cancelWnd = GetDlgItem(IDCANCEL);

	if (cancelWnd)
		{
		cancelWnd->GetWindowRect(cancelRect);
		cancelWnd->ScreenToClient(cancelRect);

		newCancelRect.bottom = dialogRect.bottom - DIST_FROM_EDGE_HEIGHT;
		newCancelRect.top = newCancelRect.bottom - (cancelRect.bottom - cancelRect.top);
		newCancelRect.right = dialogRect.right - DIST_FROM_EDGE_WIDTH;
		newCancelRect.left = newCancelRect.right - (cancelRect.right - cancelRect.left);

		cancelWnd->MoveWindow(newCancelRect);
		}

	CRect okRect, newOkRect;
	CWnd *okWnd = GetDlgItem(IDOK);

	if (okWnd)
		{
		okWnd->GetWindowRect(okRect);
		okWnd->ScreenToClient(okRect);

		int width = okRect.right - okRect.left;

		newOkRect.bottom = dialogRect.bottom - DIST_FROM_EDGE_HEIGHT;
		newOkRect.top = newOkRect.bottom - (okRect.bottom - okRect.top);
		newOkRect.right = dialogRect.right - DIST_FROM_EDGE_WIDTH - width - DIST_BETWEEN_BUTTONS;
		newOkRect.left = newOkRect.right - width;

		okWnd->MoveWindow(newOkRect);
		}

	CRect applyRect, newApplyRect;
	CWnd *applyWnd = GetDlgItem(ID_APPLY_NOW);

	if (applyWnd)
		{
		applyWnd->GetWindowRect(applyRect);
		applyWnd->ScreenToClient(applyRect);

		int width = applyRect.right - applyRect.left;

		newApplyRect.bottom = dialogRect.bottom - DIST_FROM_EDGE_HEIGHT;
		newApplyRect.top = newApplyRect.bottom - (applyRect.bottom - applyRect.top);
		newApplyRect.right = dialogRect.right - DIST_FROM_EDGE_WIDTH - 2 * (width - DIST_BETWEEN_BUTTONS);
		newApplyRect.left = newApplyRect.right - width;

		applyWnd->MoveWindow(newApplyRect);
		}
*/
//	CRect helpRect, newHelpRect;
	CButton *helpWnd = (CButton*)GetDlgItem(IDHELP);	

	if (helpWnd)
		{
//		helpWnd->GetWindowRect(helpRect);
//		helpWnd->ScreenToClient(helpRect);

		// help button should be square so use height
//		int size = helpRect.bottom - helpRect.top;

//		newHelpRect.bottom = dialogRect.bottom - DIST_FROM_EDGE_HEIGHT;
//		newHelpRect.top = newHelpRect.bottom - size;
//		newHelpRect.left = dialogRect.left + DIST_FROM_EDGE_WIDTH;
//		newHelpRect.right = newHelpRect.left + size;

//		helpWnd->MoveWindow(newHelpRect);
		helpWnd->SetIcon(AfxGetApp()->LoadIcon(HELP_ICON));
		}
	
	return bResult;
	}

BOOL IcadPropertySheet:: OnInitDialog() 
	{
	BOOL bResult = CPropertySheet::OnInitDialog();
	
	CWnd *applyWnd = GetDlgItem(ID_APPLY_NOW);
	CWnd *okWnd = GetDlgItem(IDOK);
	CWnd *cancelWnd = GetDlgItem(IDCANCEL);
	
	//Save position info so we can move other buttons into position
	CRect applyRect, cancelRect, okRect;
	applyWnd->GetWindowRect(applyRect);
	cancelWnd->GetWindowRect(cancelRect);
	okWnd->GetWindowRect(okRect);

	//Convert position from screen to client coord
	ScreenToClient(applyRect);
	ScreenToClient(cancelRect);
	ScreenToClient(okRect);

	//Move OK and cancel buttons;
	cancelWnd->MoveWindow(applyRect, TRUE);
	okWnd->MoveWindow(cancelRect, TRUE);
	applyWnd->MoveWindow(okRect, TRUE);
	
	return bResult;
	}

void IcadPropertySheet::AddHelpButton()
{
	CRect helpRect, tabRect, cancelRect;

	CWnd *cancelWnd = GetDlgItem(IDCANCEL);
	cancelWnd->GetWindowRect(cancelRect);
	ScreenToClient(cancelRect);

	GetTabControl()->GetWindowRect(tabRect);
	ScreenToClient(tabRect);

	helpRect.top = cancelRect.top;
	helpRect.bottom = cancelRect.bottom;
	helpRect.left = tabRect.left;
	helpRect.right = tabRect.left + cancelRect.bottom - cancelRect.top;

	m_help.Create("", WS_CHILD | WS_VISIBLE | BS_ICON, helpRect, this, IDHELP);
	m_help.SetIcon(AfxGetApp()->LoadIcon(HELP_ICON));
}

