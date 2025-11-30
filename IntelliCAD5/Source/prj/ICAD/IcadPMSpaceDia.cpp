/* <DevDir>\source\prj\icad\icadpmspacedia.cpp
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 */


#include "Icad.h"
#include "IcadApi.h"
#include "resource.hm"
#include "paths.h"
#include "IcadDialog.h"
#include "vxtabrec.h"
#include "cmds.h"
#include "cmdsLayout.h"

class pms_CPMSpace : public IcadDialog
{

	friend void CMainWindow::IcadPMSpaceDia();

	int		m_workspaceChoice;
	CString	m_oldLayout, m_newLayout;

public:
	pms_CPMSpace();
	BOOL OnInitDialog();

protected:
	CLayoutManager m_layoutManager;
	void CheckLayoutCombo();
	afx_msg void OnHelp();
	afx_msg void OnVport();
	afx_msg void OnWorkspace0();
	afx_msg void OnWorkspace1();
	afx_msg void OnWorkspace2();
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()

	void initLayoutsComboBox();
};


BEGIN_MESSAGE_MAP(pms_CPMSpace, CDialog)
	ON_BN_CLICKED(WS_VPORT, OnVport)
	ON_BN_CLICKED(WS_WORKSPACE0, OnWorkspace0)
	ON_BN_CLICKED(WS_WORKSPACE1, OnWorkspace1)
	ON_BN_CLICKED(WS_WORKSPACE2, OnWorkspace2)
	ON_BN_CLICKED(IDHELP, OnHelp)
END_MESSAGE_MAP()

void
pms_CPMSpace::OnVport()
{
	EndDialog(WS_VPORT);
}

void
pms_CPMSpace::OnHelp()
{
	CommandRefHelp(m_hWnd, HLP_PMSPACE);
}

BOOL
pms_CPMSpace::OnInitDialog()
{
	if(m_workspaceChoice == 1)
		GetDlgItem(IDC_WS_COMBO_LAYS)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_WS_COMBO_LAYS)->EnableWindow(TRUE);

	initLayoutsComboBox();

	return IcadDialog::OnInitDialog();
}

void
pms_CPMSpace::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, WS_WORKSPACE0, m_workspaceChoice);
	DDX_CBString(pDX, IDC_WS_COMBO_LAYS, m_newLayout);
}

pms_CPMSpace::pms_CPMSpace(): IcadDialog(IDD_WORKSPACES), m_layoutManager(SDS_CURDWG)
{
	resbuf	rb;

	SDS_getvar(NULL, DB_QTILEMODE, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
	if(rb.resval.rint)
		// model space
		m_workspaceChoice = 1;
	else
	{
		// paper space
		SDS_getvar(NULL, DB_QCVPORT, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
		if(rb.resval.rint == 1)
			m_workspaceChoice = 2;
		else
			m_workspaceChoice = 0;
	}
}

// EBATECH(CNBR) 2002/8/26 Layout combobox makes disable when there is no layout.
void pms_CPMSpace::CheckLayoutCombo()
{
	if( static_cast<CComboBox*>(GetDlgItem(IDC_WS_COMBO_LAYS))->GetCount() > 0 )
		GetDlgItem(IDC_WS_COMBO_LAYS)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_WS_COMBO_LAYS)->EnableWindow(FALSE);
}

void
pms_CPMSpace::OnWorkspace0()
{
	GetDlgItem(IDC_WS_COMBO_LAYS)->EnableWindow(TRUE);
}

void
pms_CPMSpace::OnWorkspace1()
{
	GetDlgItem(IDC_WS_COMBO_LAYS)->EnableWindow(FALSE);
}

void
pms_CPMSpace::OnWorkspace2()
{
	GetDlgItem(IDC_WS_COMBO_LAYS)->EnableWindow(TRUE);
}

void
pms_CPMSpace::initLayoutsComboBox()
{
	const char* pName;
	m_layoutManager.resetIterator();
	while(m_layoutManager.next(pName, NULL, NULL, false))
		static_cast<CComboBox*>(GetDlgItem(IDC_WS_COMBO_LAYS))->AddString(pName);
	m_newLayout = m_layoutManager.findActiveLayout(false);
}

//////////////////////////////////////////////////////////////////////////////////////////
//
//
void
CMainWindow::IcadPMSpaceDia()
{
	resbuf	rb;
	pms_CPMSpace p_CPMSpace;
	int result = p_CPMSpace.DoModal();

	switch(result)
	{
	case IDOK :
		switch(p_CPMSpace.m_workspaceChoice)
		{
		case 0:
			rb.restype = RTSTR;
			rb.resval.rstring = (char*)(LPCTSTR)p_CPMSpace.m_newLayout;
			sds_setvar("CTAB", &rb);
			if(SDS_CURDWG->ret_ntabrecs(DB_VXTAB, 0) == 1)
			{
				sds_alert(ResourceString(IDC_ICADPMSPACEDIA_NO_MODEL__2, "No model space viewports defined,\nstarting the mspace command."));
				SDS_SendMessage(WM_MENUSELECT, 0, (LPARAM)ResourceString(IDC_ICADPMSPACEDIA__C_CMVIEW_3, "^C^C_MVIEW"));
			}
			else
			{
				SDS_SendMessage(WM_MENUSELECT, 0, (LPARAM)ResourceString(IDC_ICADPMSPACEDIA__C_CMSPAC_4, "^C^C_MSPACE"));
			}
			break;

		case 1 :
			rb.restype = RTSHORT;
			rb.resval.rint = 1;
			sds_setvar("TILEMODE"/*DNT*/, &rb);
			break;

		case 2 :
			rb.restype = RTSTR;
			rb.resval.rstring = (char*)(LPCTSTR)p_CPMSpace.m_newLayout;
			sds_setvar("CTAB", &rb);

			SDS_SendMessage(WM_MENUSELECT, 0, (LPARAM)ResourceString(IDC_ICADPMSPACEDIA__C_CPSPAC_5, "^C^C_PSPACE" ));
		}
		break;

	case WS_VPORT :
		SDS_CMainWindow->m_IgnoreLastCmd = true;
		SDS_getvar(NULL, DB_QTILEMODE, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
		if(rb.resval.rint)
			SDS_SendMessage(WM_MENUSELECT, 0, (LPARAM)ResourceString(IDC_ICADPMSPACEDIA__C_CVIEWP_6, "^C^C_VIEWPORTS"));
		else
			SDS_SendMessage(WM_MENUSELECT, 0, (LPARAM)ResourceString(IDC_ICADPMSPACEDIA__C_CMVIEW_3, "^C^C_MVIEW"));

	} // switch(p_CPMSpace.DoModal())
}
