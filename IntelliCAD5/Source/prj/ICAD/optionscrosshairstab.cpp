/* F:\ICADDEV\PRJ\ICAD\OPTIONSCROSSHAIRSTAB.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

// OptionsCrosshairsTab.cpp : implementation file
//

#include "icad.h"
#include "IcadApi.h"
#include "IcadCursor.h"
#include "OptionsCrosshairsTab.h"
#include "OptionsFuncts.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const int s_minCrosshairsSize = 1;
static const int s_maxCrosshairsSize = 100;

static const int s_minAttractionDistance = 1;
static const int s_maxAttractionDistance = 10;

/////////////////////////////////////////////////////////////////////////////
// OptionsCrosshairsTab property page

IMPLEMENT_DYNCREATE(OptionsCrosshairsTab, CPropertyPage)

OptionsCrosshairsTab::OptionsCrosshairsTab() : CPropertyPage(OptionsCrosshairsTab::IDD)
{
	//{{AFX_DATA_INIT(OptionsCrosshairsTab)
	//}}AFX_DATA_INIT

	resbuf rb;

    sds_getvar("COLORX"/*DNT*/,&rb);
	iColorX=rb.resval.rint;

    sds_getvar("COLORY"/*DNT*/,&rb);
	iColorY=rb.resval.rint;

    sds_getvar("COLORZ"/*DNT*/,&rb); 
	iColorZ=rb.resval.rint;

	m_crosshairsSize = IcadCursor::AppCursor().GetCrossHairSize();
	m_alwaysCrosshair = IcadCursor::AppCursor().AlwaysUseCrosshair(true);
	IcadCursor::AppCursor().AlwaysUseCrosshair( !!m_alwaysCrosshair);

	m_EnableAttraction = IcadCursor::AppCursor().isEnableGripsAttraction();
	m_AttractionDistance = IcadCursor::AppCursor().GetAttractionDistance();

	if(m_crosshairsSize < s_minCrosshairsSize || m_crosshairsSize > s_maxCrosshairsSize)
		m_crosshairsSize = 5;
	if(m_AttractionDistance < s_minAttractionDistance || m_AttractionDistance > s_maxAttractionDistance)
		m_AttractionDistance = 3;

	bCurFlag = false;
}

OptionsCrosshairsTab::~OptionsCrosshairsTab()
{
}

void OptionsCrosshairsTab::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(OptionsCrosshairsTab)
	DDX_Text(pDX, OPTION_CROSSHAIRS_PERCENT, m_crosshairsSize);
	DDX_Check(pDX, OPTION_CROSSHAIRS_DEFAULT, m_alwaysCrosshair);
	DDX_Check(pDX, OPTION_ATTRACTION, m_EnableAttraction);
	DDX_Text(pDX, OPTION_ATTRACT_POINTS, m_AttractionDistance);
	
	// EnableWindow should be before checking m_AttractionDistance becouse exeption in DDV_MinMaxInt causes that EnableWindow is never called
	::EnableWindow(::GetDlgItem(m_hWnd, OPTION_ATTRACT_POINTS), m_EnableAttraction);
	::EnableWindow(::GetDlgItem(m_hWnd, OPTION_ATTRACT_POINTS_SPIN), m_EnableAttraction);

	DDV_MinMaxInt(pDX, m_crosshairsSize, s_minCrosshairsSize, s_maxCrosshairsSize);
	DDV_MinMaxInt(pDX, m_AttractionDistance, s_minAttractionDistance, s_maxAttractionDistance);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(OptionsCrosshairsTab, CPropertyPage)
	//{{AFX_MSG_MAP(OptionsCrosshairsTab)
	ON_BN_CLICKED(OPTION_X_AXIS, OnXAxis)
	ON_BN_CLICKED(OPTION_Y_AXIS, OnYAxis)
	ON_BN_CLICKED(OPTION_Z_AXIS, OnZAxis)
	ON_WM_DRAWITEM()
	ON_NOTIFY(UDN_DELTAPOS, OPTION_CROSSHAIRS_SPIN, OnDeltaposCrosshairsSpin)
	ON_NOTIFY(UDN_DELTAPOS, OPTION_ATTRACT_POINTS_SPIN, OnDeltaposAttractPointsSpin)
	ON_BN_CLICKED(OPTION_ATTRACTION, OnAttraction)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void OptionsCrosshairsTab::OnXAxis() 
{
    SDS_GetColorDialog(iColorX,&iColorX,3);
    
	PaintColorSwatch(this, OPTION_X_AXIS, iColorX, false);

    bCurFlag=true;	
}

void OptionsCrosshairsTab::OnYAxis() 
{
    SDS_GetColorDialog(iColorY,&iColorY,3);
    
	PaintColorSwatch(this, OPTION_Y_AXIS, iColorY, false);

    bCurFlag=true;	
}

void OptionsCrosshairsTab::OnZAxis() 
{
    SDS_GetColorDialog(iColorZ,&iColorZ,3);
    
	PaintColorSwatch(this, OPTION_Z_AXIS, iColorZ, false);

    bCurFlag=true;	
}

void OptionsCrosshairsTab::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	switch (nIDCtl)
	{
	case OPTION_X_AXIS:
		DrawColorButton(lpDrawItemStruct, this, OPTION_X_AXIS, iColorX, false);

	case OPTION_Y_AXIS:
		DrawColorButton(lpDrawItemStruct, this, OPTION_Y_AXIS, iColorY, false);

	case OPTION_Z_AXIS:
		DrawColorButton(lpDrawItemStruct, this, OPTION_Z_AXIS, iColorZ, false);

	default:
		CPropertyPage::OnDrawItem(nIDCtl, lpDrawItemStruct);
	}
}

void OptionsCrosshairsTab::OnOK() 
{
	resbuf rb;
    rb.restype=RTSHORT;

    rb.resval.rint=iColorX;
    sds_setvar("COLORX"/*DNT*/,&rb);
    rb.resval.rint=iColorY;
    sds_setvar("COLORY"/*DNT*/,&rb);
    rb.resval.rint=iColorZ;
    sds_setvar("COLORZ"/*DNT*/,&rb);

    rb.resval.rint=m_crosshairsSize;
	sds_setvar("CURSORSIZE", &rb);
	IcadCursor::AppCursor().AlwaysUseCrosshair( !!m_alwaysCrosshair);
	IcadCursor::AppCursor().EnableGripsAttraction(!!m_EnableAttraction);
	IcadCursor::AppCursor().SetAttractionDistance(m_AttractionDistance);
	
	CPropertyPage::OnOK();
}

void OptionsCrosshairsTab::OnDeltaposCrosshairsSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	
    UpdateData();
	m_crosshairsSize -= pNMUpDown->iDelta;

	if (m_crosshairsSize > s_maxCrosshairsSize)
		m_crosshairsSize = s_maxCrosshairsSize;

	if (m_crosshairsSize < s_minCrosshairsSize)
		m_crosshairsSize = s_minCrosshairsSize;

	UpdateData(FALSE);

	*pResult = 0;
}

void OptionsCrosshairsTab::OnDeltaposAttractPointsSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

    UpdateData();
	m_AttractionDistance -= pNMUpDown->iDelta;

	if (m_AttractionDistance > s_maxAttractionDistance)
		m_AttractionDistance = s_maxAttractionDistance;

	if (m_AttractionDistance < s_minAttractionDistance)
		m_AttractionDistance = s_minAttractionDistance;

	UpdateData(FALSE);
	
	*pResult = 0;
}

void OptionsCrosshairsTab::OnAttraction() 
{
    UpdateData();
}
