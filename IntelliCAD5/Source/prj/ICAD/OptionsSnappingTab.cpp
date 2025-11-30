/* F:\ICADDEV\PRJ\ICAD\OPTIONSSnappingTAB.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

// OptionsSnappingTab.cpp : implementation file
//

#include "stdafx.h"
#include "icad.h"
#include "OptionsSnappingTab.h"
#include "IcadAPI.h"
#include "Preferences.h"
#include "OptionsFuncts.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// OptionsSnappingTab property page

IMPLEMENT_DYNCREATE(OptionsSnappingTab, CPropertyPage)

OptionsSnappingTab::OptionsSnappingTab() : CPropertyPage(OptionsSnappingTab::IDD)
{
	//{{AFX_DATA_INIT(OptionsSnappingTab)
	//}}AFX_DATA_INIT
	m_bEnableFlyOver = UserPreference::SnapFlyOverEnablePreference;
	m_bDisplayMarker = UserPreference::SnapMarkerPreference;
	m_bDisplayTooltips = UserPreference::SnapTooltipsPreference;
	m_bDisplayApertureBox = UserPreference::SnapApertureBoxPreference;
	m_bDrawMarkerInAllViews = UserPreference::SnapMarkerDrawInAllViews;
	m_bShowOsnapCursorDecor = UserPreference::SnapShowOsnapCursorDecorationPreference;
	m_iMarkerSize = UserPreference::SnapMarkerSizePreference;
	if( m_iMarkerSize < 1 || m_iMarkerSize > 20 )
	{
		m_iMarkerSize = 8;
		UserPreference::SnapMarkerSizePreference = m_iMarkerSize;
	}
	m_iMarkerThickness = UserPreference::SnapMarkerThicknessPreference;
	if( m_iMarkerThickness < 1 || m_iMarkerThickness > 5 )
	{
		m_iMarkerThickness = 2;
		UserPreference::SnapMarkerThicknessPreference = m_iMarkerThickness;
	}
	if( m_iMarkerThickness > m_iMarkerSize )
	{
		m_iMarkerThickness = m_iMarkerSize;
		UserPreference::SnapMarkerThicknessPreference = m_iMarkerThickness;
	}
	
	m_iMarkerColor = UserPreference::SnapMarkerColorPreference;
	if( m_iMarkerColor < 0 || m_iMarkerColor > 256 )
	{
		m_iMarkerColor = 256;
		UserPreference::SnapMarkerColorPreference = m_iMarkerColor;
	}

	// Set values to the Picture window
	m_cDlgPicture.m_iColor = m_iMarkerColor;
	m_cDlgPicture.m_iSize = m_iMarkerSize;
	m_cDlgPicture.m_iThickness = m_iMarkerThickness;
}

OptionsSnappingTab::~OptionsSnappingTab()
{
}

void OptionsSnappingTab::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(OptionsSnappingTab)
	DDX_Control(pDX, OPTIONS_SNAPPING_PIC, m_cDlgPicture);
	DDX_Check(pDX, OPTIONS_SNAPPING_ENABLE, m_bEnableFlyOver);
	DDX_Check(pDX, OPTIONS_SNAPPING_MARKER, m_bDisplayMarker);
	DDX_Check(pDX, OPTIONS_SNAPPING_TOOLTIPS, m_bDisplayTooltips);
	DDX_Check(pDX, OPTIONS_SNAPPING_APERTURE, m_bDisplayApertureBox);
	DDX_Check(pDX, OPTIONS_SNAPPING_DRAWINALL, m_bDrawMarkerInAllViews);
	DDX_Check(pDX, OPTIONS_SNAPPING_SHOWCURSORDECOR, m_bShowOsnapCursorDecor);
	DDX_Text(pDX, OPTIONS_SNAPPING_SIZE, m_iMarkerSize);
	DDV_MinMaxInt(pDX, m_iMarkerSize, 1, 20);
	DDX_Text(pDX, OPTIONS_SNAPPING_THICKNESS, m_iMarkerThickness);
	DDV_MinMaxInt(pDX, m_iMarkerThickness, 1, 5);
	//}}AFX_DATA_MAP
}



BEGIN_MESSAGE_MAP(OptionsSnappingTab, CPropertyPage)
	//{{AFX_MSG_MAP(OptionsSnappingTab)
	ON_BN_CLICKED(OPTIONS_SNAPPING_MARKERCOLOR, OnSnapMarkercolor)
	ON_WM_DRAWITEM()
	ON_NOTIFY(UDN_DELTAPOS, OPTIONS_SNAPPING_SIZE_UPDN, OnDeltaposSnapSizeUpdn)
	ON_NOTIFY(UDN_DELTAPOS, OPTIONS_SNAPPING_THICKNESS_UPDN, OnDeltaposSnapThicknessUpdn)
	ON_BN_CLICKED(OPTIONS_SNAPPING_ENABLE, OnEnableCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void OptionsSnappingTab::OnSnapMarkercolor() 
{
    SDS_GetColorDialog(m_iMarkerColor,&m_iMarkerColor,3);
	PaintColorSwatch(this, OPTIONS_SNAPPING_MARKERCOLOR, m_iMarkerColor, false);
	
	m_cDlgPicture.m_iColor = m_iMarkerColor;
	m_cDlgPicture.Draw();
}

void OptionsSnappingTab::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	if(nIDCtl == OPTIONS_SNAPPING_MARKERCOLOR)
		DrawColorButton(lpDrawItemStruct, this, OPTIONS_SNAPPING_MARKERCOLOR, m_iMarkerColor, false);

	CPropertyPage::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void OptionsSnappingTab::OnOK() 
{
	UpdateData(TRUE);
	UserPreference::SnapFlyOverEnablePreference = (m_bEnableFlyOver ? true : false); 
	UserPreference::SnapMarkerPreference = (m_bDisplayMarker ? true : false);
	UserPreference::SnapMarkerSizePreference = m_iMarkerSize;
	UserPreference::SnapMarkerThicknessPreference = m_iMarkerThickness;
	UserPreference::SnapMarkerColorPreference = m_iMarkerColor;
	UserPreference::SnapTooltipsPreference = (m_bDisplayTooltips ? true : false);
	UserPreference::SnapApertureBoxPreference = (m_bDisplayApertureBox ? true : false);	
	UserPreference::SnapMarkerDrawInAllViews = (m_bDrawMarkerInAllViews ? true : false);
	UserPreference::SnapShowOsnapCursorDecorationPreference = (m_bShowOsnapCursorDecor ? true : false);
	CPropertyPage::OnOK();
}

void OptionsSnappingTab::OnDeltaposSnapSizeUpdn(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
    UpdateData();
	m_iMarkerSize -= pNMUpDown->iDelta;

	if (m_iMarkerSize > 20)
		m_iMarkerSize = 20;
	
	if (m_iMarkerSize < 1)
		m_iMarkerSize = 1;

	// should not be lower than thickness
	if (m_iMarkerSize < m_iMarkerThickness)
		m_iMarkerThickness = m_iMarkerSize;

	UpdateData(FALSE);

	m_cDlgPicture.m_iSize = m_iMarkerSize;
	m_cDlgPicture.m_iThickness = m_iMarkerThickness;
	m_cDlgPicture.Draw();
	
	*pResult = 0;
}

void OptionsSnappingTab::OnDeltaposSnapThicknessUpdn(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
    UpdateData();
	m_iMarkerThickness -= pNMUpDown->iDelta;

	if (m_iMarkerThickness > 5)
		m_iMarkerThickness = 5;
	
	// should not be greater than size
	if (m_iMarkerThickness > m_iMarkerSize )
		m_iMarkerThickness = m_iMarkerSize;

	if (m_iMarkerThickness < 1)
		m_iMarkerThickness = 1;

	UpdateData(FALSE);

	m_cDlgPicture.m_iThickness = m_iMarkerThickness;
	m_cDlgPicture.m_iSize = m_iMarkerSize;
	m_cDlgPicture.Draw();
	
	*pResult = 0;
}

void OptionsSnappingTab::OnEnableCheck()
{
		UpdateData(TRUE);

		// enabling dlg-items
		enableControls( m_bEnableFlyOver );
}

void OptionsSnappingTab::enableControls( BOOL bEnable )
{
		::EnableWindow(::GetDlgItem(m_hWnd,OPTIONS_SNAPPING_DRAWINALL),bEnable);
		::EnableWindow(::GetDlgItem(m_hWnd,OPTIONS_SNAPPING_APERTURE),bEnable);
		::EnableWindow(::GetDlgItem(m_hWnd,OPTIONS_SNAPPING_TOOLTIPS),bEnable);
		::EnableWindow(::GetDlgItem(m_hWnd,OPTIONS_SNAPPING_MARKER),bEnable);
		::EnableWindow(::GetDlgItem(m_hWnd,OPTIONS_SNAPPING_SIZE),bEnable);
		::EnableWindow(::GetDlgItem(m_hWnd,OPTIONS_SNAPPING_SIZE_UPDN),bEnable);
		::EnableWindow(::GetDlgItem(m_hWnd,OPTIONS_SNAPPING_THICKNESS),bEnable);
		::EnableWindow(::GetDlgItem(m_hWnd,OPTIONS_SNAPPING_THICKNESS_UPDN),bEnable);
		::EnableWindow(::GetDlgItem(m_hWnd,OPTIONS_SNAPPING_MARKERCOLOR),bEnable);
}

BOOL OptionsSnappingTab::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	enableControls( m_bEnableFlyOver );
	return TRUE;
}
