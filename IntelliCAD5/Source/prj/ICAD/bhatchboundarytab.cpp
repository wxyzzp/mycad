/* D:\ICADDEV\PRJ\ICAD\BHATCHBOUNDARYTAB.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Boundary tab of bhatch/boundary dialog
 * 
 */ 

#include "icad.h"
#include "BHatchBoundaryTab.h"
#include "BHatchDialog.h"
#include "cmds.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// BHatchBoundaryTab dialog

BHatchBoundaryTab::BHatchBoundaryTab(BOOL isHatch, CWnd* pParent /*=NULL*/)
	: CDialog(BHatchBoundaryTab::IDD, pParent), m_isHatch(isHatch), m_boundarySet(), m_pickPointsCalled(FALSE),m_selectObjectsCalled(FALSE),m_callFromBhatch(FALSE)
{
	//{{AFX_DATA_INIT(BHatchBoundaryTab)
	m_boundarySetRadio = ALL_OBJECTS;
	m_retainBoundCheck = !m_isHatch;
	m_boundaryTypeCombo = _T("Polyline");
	//}}AFX_DATA_INIT
	m_islandRadio = BHatchBoundaryTab::NESTED_ISLAND;

}


void BHatchBoundaryTab::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Radio(pDX, BOUND_ISLAND_NESTED, m_islandRadio);

	//{{AFX_DATA_MAP(BHatchBoundaryTab)
	DDX_Radio(pDX, BOUNDARY_SET_ALL, m_boundarySetRadio);
	DDX_Check(pDX, BOUNDARY_RETAIN, m_retainBoundCheck);
	DDX_CBString(pDX, BOUNDARY_TYPE, m_boundaryTypeCombo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(BHatchBoundaryTab, CDialog)
	//{{AFX_MSG_MAP(BHatchBoundaryTab)
	ON_BN_CLICKED(BOUND_ISLAND_IGNORE, OnIslandIgnore)
	ON_BN_CLICKED(BOUND_ISLAND_NESTED, OnIslandNested)
	ON_BN_CLICKED(BOUND_ISLAND_OUTER, OnIslandOuter)
	ON_BN_CLICKED(BOUNDARY_PICK_POINTS, OnPickPoints)
	ON_BN_CLICKED(BOUNDARY_SET_SELECT, OnBoundarySetSelect)
	ON_BN_CLICKED(BOUNDARY_SELECT_OBJECTS, OnChooseObjects)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// BHatchBoundaryTab message handlers

BOOL BHatchBoundaryTab::OnInitDialog() 
{
	if (!m_isHatch)
	{
		CWnd *retainBound = GetDlgItem(BOUNDARY_RETAIN);
		retainBound->EnableWindow(FALSE);
	}

	// Once pick points has been called the boundary and island detection option set cannot be redefined.
	if (m_pickPointsCalled)
	{	
		CWnd *item = GetDlgItem(BOUNDARY_SET_SELECT);
		item->EnableWindow(FALSE);

		item = GetDlgItem(BOUNDARY_SET_ALL);
		item->EnableWindow(FALSE);

		item = GetDlgItem(BOUNDARY_SET_CURRENT);
		item->EnableWindow(FALSE);

		item = GetDlgItem(BOUND_ISLAND_IGNORE);
		item->EnableWindow(FALSE);

		item = GetDlgItem(BOUND_ISLAND_OUTER);
		item->EnableWindow(FALSE);
		
		item = GetDlgItem(BOUND_ISLAND_NESTED);
		item->EnableWindow(FALSE);

	}
	
	if(!m_callFromBhatch)
	{
		CWnd *item = GetDlgItem(BOUNDARY_SELECT_OBJECTS);
		item->EnableWindow(FALSE);
	}
	

	if ((!m_pickPointsCalled) && (!m_boundarySet.IsEmpty()))
	{
		CWnd *boundCheck = GetDlgItem(BOUNDARY_SET_CURRENT);
		boundCheck->EnableWindow(TRUE);
	}
	
	if (m_boundarySet.IsEmpty())  // user may have cancelled a boundary set selection so ensure set to all
		m_boundarySetRadio = ALL_OBJECTS;

	
	// make sure correct bitmap is showing
	switch (m_islandRadio)
	{
		case IGNORE_ISLAND:
			UpdateIslandBmp(HATCH_ISL_IGNORE);
			break;
			
		case OUTER_ISLAND:
			UpdateIslandBmp(HATCH_ISL_OUTER);
			break;
			
		case NESTED_ISLAND:
			UpdateIslandBmp(HATCH_ISL_NESTED);
			break;
	}

	CDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void BHatchBoundaryTab::UpdateIslandBmp (int newBmp)
{
	CStatic *islandBmp = (CStatic *) GetDlgItem(ISLAND_BITMAP);

	if (islandBmp) 
	{
		HBITMAP bmp = (HBITMAP)LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(newBmp),
			IMAGE_BITMAP,0,0,LR_LOADMAP3DCOLORS);

		islandBmp->SetBitmap(bmp);
	}

	/*D.G.*/// Update the hatch style too.
	BHatchDialog* Parent = (BHatchDialog*)GetParent();
	if(Parent->m_hatch)
		Parent->m_hatch->set_75((int)m_islandRadio);
}


void BHatchBoundaryTab::OnIslandIgnore() 
{
	UpdateIslandBmp(HATCH_ISL_IGNORE);
}


void BHatchBoundaryTab::OnIslandNested() 
{
	UpdateIslandBmp(HATCH_ISL_NESTED);
}


void BHatchBoundaryTab::OnIslandOuter() 
{
	UpdateIslandBmp(HATCH_ISL_OUTER);
}


void BHatchBoundaryTab::OnPickPoints() 
{
	//Boundary set can only be set before first pick points is called.
	if ((!m_pickPointsCalled) && (m_boundarySetRadio == ALL_OBJECTS))
	{
		cmd_getAllEntities(m_boundarySet);
	}

	m_pickPointsCalled = TRUE;

	CDialog *parent = (CDialog*) GetParent();
	parent->EndDialog(BOUNDARY_PICK_POINTS);
}

void BHatchBoundaryTab::OnBoundarySetSelect() 
{
	CDialog *parent = (CDialog*) GetParent();
	parent->EndDialog(BOUNDARY_SET_SELECT);
}



void BHatchBoundaryTab::OnChooseObjects() 
{
	m_selectObjectsCalled = TRUE;
	CDialog *parent = (CDialog*) GetParent();
	parent->EndDialog(BOUNDARY_SELECT_OBJECTS);
}


BOOL BHatchBoundaryTab::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	int wNotifyCode = HIWORD(wParam); // notification code 
	int wID = LOWORD(wParam);         // item, control, or accelerator identifier 
//	HWND hwndCtl = (HWND) lParam;      // handle of control 	

	// don't know the #defined version of this message.
	if (wNotifyCode == 0)
	{
		// when "Enter" is hit if parent's OK is enabled then send enter message to OK
		// otherwise send enter message to OnPickPoints
		if (wID == 1)
		{
			if (m_pickPointsCalled)
				return GetParent()->SendMessage(WM_COMMAND, wParam, lParam);

			else
				OnPickPoints();
		}

		// want to send escape codes to parent
		else if (wID == 2)
			return GetParent()->SendMessage(WM_COMMAND, wParam, lParam);

	}
	
	return CDialog::OnCommand(wParam, lParam);
}


