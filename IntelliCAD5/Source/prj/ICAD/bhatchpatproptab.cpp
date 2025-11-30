/* D:\ICADDEV\PRJ\ICAD\BHATCHPATPROPTAB.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * Pattern Properties tab of bhatch/boundary dialog
 *
 */

#include "icad.h"
#include "IcadApi.h"
#include "BHatchPatPropTab.h"
#include "BHatchDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// BHatchPatPropTab dialog


BHatchPatPropTab::BHatchPatPropTab(CWnd* pParent /*=NULL*/)
	: CDialog(BHatchPatPropTab::IDD, pParent)
{
	//{{AFX_DATA_INIT(BHatchPatPropTab)
	m_patType = PREDEFINED;
	m_spacing = 1.0;
	m_scale = 1.0;
	m_crossHatch = FALSE;
	m_associative = TRUE;
	m_angle = 0.0;
	m_isoPenWidth = _T("");
	//}}AFX_DATA_INIT
}


void BHatchPatPropTab::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(BHatchPatPropTab)
	DDX_CBIndex(pDX, PATTERN_TYPE, m_patType);
	DDX_Text(pDX, PATTERN_SPACING, m_spacing);
	DDX_Text(pDX, PATTERN_SCALE, m_scale);
	DDX_Check(pDX, PATTERN_CROSS_HATCH, m_crossHatch);
	DDX_Check(pDX, PATTERN_ASSOCIATIVE, m_associative);
	DDX_Text(pDX, PATTERN_ANGLE, m_angle);
	DDV_MinMaxDouble(pDX, m_angle, 0., 360.);
	DDX_CBString(pDX, PATTERN_ISO_PEN, m_isoPenWidth);
	//}}AFX_DATA_MAP
}

void BHatchPatPropTab::EnableControls()
{
// VT. 28-3-02. Fix. Possible using uninitialize variable 'isoWidth'.
	BOOL scale, spacing, isoWidth = FALSE, xHatch;
	BOOL patternTab;

	BHatchDialog *parent = (BHatchDialog*) GetParent();

	switch(m_patType)
	{
	case PREDEFINED:
		{
			scale = TRUE;
			spacing = FALSE;
			xHatch = FALSE;
			patternTab = TRUE;

			// need to set this according to pattern name being used.
			int selection = parent->m_patternTab.m_fileCombo.GetCurSel();
			if (selection == CB_ERR)
			{
				patternTab = FALSE;
				break;
			}

			CString file;
			parent->m_patternTab.m_fileCombo.GetLBText(selection, file);

//4M Item:17->
			parent->m_patternTab.filepat=file;
//4M Item:17<-

			if(!file.CompareNoCase(SDS_ISOPATTERNFILE))
				isoWidth = TRUE;
		}
		break;

	case USER_DEFINED:
		scale = FALSE;
		spacing = TRUE;
		xHatch = TRUE;
		patternTab = FALSE;

		break;

	default:
		ASSERT(FALSE);
		return;
	}

	CWnd *wnd = GetDlgItem(PATTERN_SCALE);
	wnd->EnableWindow(scale);

	wnd = GetDlgItem(PATTERN_SPACING);
	wnd->EnableWindow(spacing);

	wnd = GetDlgItem(PATTERN_ISO_PEN);
	wnd->EnableWindow(isoWidth);

	wnd = GetDlgItem(PATTERN_CROSS_HATCH);
	wnd->EnableWindow(xHatch);

	parent->EnablePatternTab(patternTab);

}



BEGIN_MESSAGE_MAP(BHatchPatPropTab, CDialog)
	//{{AFX_MSG_MAP(BHatchPatPropTab)
	ON_CBN_SELCHANGE(PATTERN_TYPE, OnSelchangePatType)
	ON_BN_CLICKED(PATTERN_CROSS_HATCH, OnCrossHatch)
	ON_BN_CLICKED(PATTERN_ASSOCIATIVE, OnAssociative)
	ON_EN_KILLFOCUS(PATTERN_ANGLE, OnKillfocusAngle)
	ON_EN_KILLFOCUS(PATTERN_SCALE, OnKillfocusScale)
	ON_EN_KILLFOCUS(PATTERN_SPACING, OnKillfocusSpacing)
	ON_CBN_SELCHANGE(PATTERN_ISO_PEN, OnSelchangeIsoPen)
	ON_BN_CLICKED(PATTERN_COPY_HATCH, OnCopyHatch)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL BHatchPatPropTab::OnCommand(WPARAM wParam, LPARAM lParam)
{
	int wNotifyCode = HIWORD(wParam); // notification code
	int wID = LOWORD(wParam);         // item, control, or accelerator identifier
//	HWND hwndCtl = (HWND) lParam;      // handle of control

	// don't know the #defined version of this message.
	if (wNotifyCode == 0)
	{
		// when "Enter" is hit send to parent's OK
		if (wID == 1)
			return GetParent()->SendMessage(WM_COMMAND, wParam, lParam);

		// want to send escape codes to parent
		if (wID == 2)
			return GetParent()->SendMessage(WM_COMMAND, wParam, lParam);
	}

	return CDialog::OnCommand(wParam, lParam);
}


void BHatchPatPropTab::OnSelchangePatType()
{
	BHatchDialog *dialog = (BHatchDialog*) GetParent();

	dialog->m_bCopied = false;

	dialog->UndrawHatch();

	// don't update until old hatch is undrawn
	UpdateData(TRUE);

	// Since scale/spacing are two different fields that share same data member, need to make sure
	// hatch is set to use proper value.  Note do not want to call OnKillfocus methods as we don't want
	// the hatch redrawn more than once.
	if (m_patType == USER_DEFINED)
		dialog->m_hatch->set_41(m_spacing);
	else
		dialog->m_hatch->set_41(m_scale);

	// enable/disable proper fields
	EnableControls();

	dialog->RecalcPattern();
	dialog->RedrawHatch();
}

BOOL BHatchPatPropTab::OnInitDialog()
{
	FillCombo();
	
	//Bugzilla No. 78414 ; 01-30-2003 [
	//Setting the tolerance value into edit box.
	CWnd *wnd = GetDlgItem(IDC_HATCH_TOLERANCE);
	if ( wnd != NULL )
	{		
		TCHAR szbuffer[10];
#ifdef VARYING_HATCH_TOLERANCE
		struct resbuf rb;
		SDS_getvar(NULL,DB_QVIEWSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);		
		sprintf(szbuffer,"%1.6f",(rb.resval.rreal * HATCH_TOLERANCE_COEFFICIENT ));
#else
		_tcscpy(szbuffer,"0.00001");	//HATCH_TOLERANCE defined in BhatchBuilder.h
#endif //VARYING_HATCH_TOLERANCE
		wnd->SetWindowText(szbuffer); 
	}
	//Bugzilla No. 78414 ; 01-30-2003 ]


	CDialog::OnInitDialog();

	// Enable/Disable proper fields
	// Now called on BHatchDialog::InitializeHatch instead
	//EnableControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void BHatchPatPropTab::FillCombo()
{
	CComboBox *pCombo = NULL;

	if (NULL == (pCombo = (CComboBox *) GetDlgItem(PATTERN_TYPE)))
		return;

	pCombo->ResetContent();
	CString	comboOption;
	comboOption.LoadString(IDC_COMBO_PATTERN_TYPE_1);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_PATTERN_TYPE_2);
	pCombo->AddString(comboOption);
}

void BHatchPatPropTab::OnCrossHatch()
{
	BHatchDialog *parent = (BHatchDialog *) GetParent();

	UpdateData(TRUE);

	if (m_crossHatch)
		parent->m_hatch->set_77(1);

	else
		parent->m_hatch->set_77(0);

	parent->RecalcPattern();
	parent->RedrawHatch();
}

void BHatchPatPropTab::OnAssociative()
{
	UpdateData(TRUE);

	BHatchDialog *parent = (BHatchDialog *) GetParent();

	if (m_associative)
		parent->m_hatch->set_71(1);

	else
		parent->m_hatch->set_71(0);
}


void BHatchPatPropTab::OnKillfocusAngle()
{
	UpdateData(TRUE);

	BHatchDialog *parent = (BHatchDialog *) GetParent();

	parent->UndrawHatch();

	//convert angle to radians
	double angle = m_angle * IC_PI/180.0;

	parent->m_hatch->set_52(angle);

	parent->RecalcPattern();

	parent->RedrawHatch();
}

void BHatchPatPropTab::OnSelchangeIsoPen()
{
	UpdateData(TRUE);

	if (!m_isoPenWidth.IsEmpty())
	{
		int loc = m_isoPenWidth.Find(" mm");
		CString width;

		if (loc > 0)
			width = m_isoPenWidth.Left(loc);
		else
			width = m_isoPenWidth;


		m_scale = atof(width.GetBuffer(0));
		width.ReleaseBuffer();

		UpdateData(FALSE);
		OnKillfocusScale();
	}
}

void BHatchPatPropTab::OnKillfocusScale()
{
	UpdateData(TRUE);

	BHatchDialog *parent = (BHatchDialog *) GetParent();

	parent->UndrawHatch();

	parent->m_hatch->set_41(m_scale);

	parent->RecalcPattern();

	parent->RedrawHatch();
}

void BHatchPatPropTab::OnKillfocusSpacing()
{
	UpdateData(TRUE);

	BHatchDialog *parent = (BHatchDialog *) GetParent();

	parent->UndrawHatch();

	parent->m_hatch->set_41(m_spacing);
	parent->RecalcPattern();

	parent->RedrawHatch();
}


void BHatchPatPropTab::OnCopyHatch()
{
	BHatchDialog *parent = (BHatchDialog*) GetParent();
	parent->EndDialog(PATTERN_COPY_HATCH);
}


/////////////////////////////////////////////////////////////////////////////
// BHatchPatPropTab message handlers


