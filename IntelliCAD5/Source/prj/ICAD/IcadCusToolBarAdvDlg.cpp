/* File  : <DevDir>\source\prj\icad\IcadCusToolBarAdvDlg.cpp
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * Advanced customization support for Toolbars
 */ 

#include "Icad.h"/*DNT*/
#include "IcadToolbarMain.h"/*DNT*/
#include "IcadCusToolBarAdvDlg.h"/*DNT*/
#include "paths.h"				/*DNT*/
#include "resource.hm"			/*DNT*/

#ifdef _DEBUG

 #ifndef USE_SMARTHEAP
  #define new DEBUG_NEW
 #endif

#endif // _DEBUG

extern int			g_nMnuTbItems;	// Global value for number of commands
extern IcadCommands	g_MnuTbItems[];	// Global command structure array.


/////////////////////////////////////////////////////////////////////////////
// CIcadCusToolBarAdvDlg dialog
/////////////////////////////////////////////////////////////////////////////

CIcadCusToolBarAdvDlg::CIcadCusToolBarAdvDlg
(
 CWnd*	pParent
) : CDialog(CIcadCusToolBarAdvDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CIcadCusToolBarAdvDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pTbMain = ((CIcadCusToolbar*)pParent)->m_pTbMain;
	m_bFlyOut = FALSE;

	if(m_pTbMain)
	{
		TBBUTTON		tbButton;
		CToolBarCtrl*	pTbCtrl = NULL;

		if(m_pTbMain->m_pToolBarCurBtnDown)
			pTbCtrl = &(m_pTbMain->m_pToolBarCurBtnDown->GetToolBarCtrl());
		else
			if(m_pTbMain->m_pFlyOutCurBtnDown)
			{
				pTbCtrl = (CToolBarCtrl*)m_pTbMain->m_pFlyOutCurBtnDown;
				m_bFlyOut = TRUE;
			}

		if(pTbCtrl)
			pTbCtrl->GetButton(m_pTbMain->m_idxCurButtonDown, &tbButton);

		// Save any changes made in the customize editboxes.
		if(tbButton.dwData)
			m_picButton = (CIcadTBbutton*)tbButton.dwData;
	}
}

void
CIcadCusToolBarAdvDlg::DoDataExchange
(
 CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIcadCusToolBarAdvDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CIcadCusToolBarAdvDlg, CDialog)
	//{{AFX_MSG_MAP(CIcadCusToolBarAdvDlg)
	ON_BN_CLICKED(CUS_TB_ADV_LG_BW_BROWSE, OnBrowseLgBw)
	ON_BN_CLICKED(CUS_TB_ADV_LG_COLOR_BROWSE, OnBrowseLgColor)
	ON_BN_CLICKED(CUS_TB_ADV_SM_BW_BROWSE, OnBrowseSmBw)
	ON_BN_CLICKED(CUS_TB_ADV_SM_COLOR_BROWSE, OnBrowseSmColor)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CIcadCusToolBarAdvDlg message handlers

BOOL
CIcadCusToolBarAdvDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	//EBATECH{
	// Bug No 77925(+) from BugZilla
	// Desc :set the icon on the ?/Help button
	CButton *helpWnd = (CButton *) GetDlgItem(IDHELP);
	helpWnd->SetIcon(AfxGetApp()->LoadIcon(HELP_ICON));
	//}
	
	CButton*	pCheckBox;

	if(m_picButton)
	{
		pCheckBox = (CButton*)GetDlgItem(CUS_TOOLBAR_ADV_CHKBEGINNER);
		if(m_picButton->m_lflgVisibility & ICAD_MN_BEG)
			pCheckBox->SetCheck(1);
		else
			pCheckBox->SetCheck(0);

		pCheckBox = (CButton*)GetDlgItem(CUS_TOOLBAR_ADV_CHKINTERMEDIATE);
		if(m_picButton->m_lflgVisibility & ICAD_MN_INT)
			pCheckBox->SetCheck(1);
		else
			pCheckBox->SetCheck(0);

		pCheckBox = (CButton*)GetDlgItem(CUS_TOOLBAR_ADV_CHKADVANCED);
		if(m_picButton->m_lflgVisibility & ICAD_MN_EXP)
			pCheckBox->SetCheck(1);
		else
			pCheckBox->SetCheck(0);

		pCheckBox = (CButton*)GetDlgItem(CUS_TOOLBAR_ADV_CHKMDIONE);
		if(m_picButton->m_lflgVisibility & ICAD_MN_OPN)
			pCheckBox->SetCheck(1);
		else
			pCheckBox->SetCheck(0);

		pCheckBox = (CButton*)GetDlgItem(CUS_TOOLBAR_ADV_CHKMDINONE);
		if(m_picButton->m_lflgVisibility & ICAD_MN_CLS)
			pCheckBox->SetCheck(1);
		else
			pCheckBox->SetCheck(0);

		pCheckBox = (CButton*)GetDlgItem(CUS_TOOLBAR_ADV_CHKEMBEDDED);
		if(m_picButton->m_lflgVisibility & ICAD_MN_SEM)
			pCheckBox->SetCheck(1);
		else
			pCheckBox->SetCheck(0);

		pCheckBox = (CButton*)GetDlgItem(CUS_TOOLBAR_ADV_CHKINPLACE);
		if(m_picButton->m_lflgVisibility & ICAD_MN_SIP)
			pCheckBox->SetCheck(1);
		else
			pCheckBox->SetCheck(0);

		pCheckBox = (CButton*)GetDlgItem(CUS_TOOLBAR_ADV_CHKCONTAINER);
		if(m_picButton->m_lflgVisibility & ICAD_MN_CLI)
			pCheckBox->SetCheck(1);
		else
			pCheckBox->SetCheck(0);
	}

	InitBitmapCombo(CUS_TB_ADV_SM_COLOR);
	InitBitmapCombo(CUS_TB_ADV_LG_COLOR);
	InitBitmapCombo(CUS_TB_ADV_SM_BW);
	InitBitmapCombo(CUS_TB_ADV_LG_BW);

	CEdit*	pEdit;
	if(!m_picButton->m_pushVar.IsEmpty())
	{
		pEdit = (CEdit*)GetDlgItem(CUS_TOOLBAR_ADV_EDITCHECKVAR);
		pEdit->SetWindowText(m_picButton->m_pushVar);
	}

	if(!m_picButton->m_grayVar.IsEmpty())
	{
		pEdit = (CEdit*)GetDlgItem(CUS_TOOLBAR_ADV_EDITGRAYVAR);
		pEdit->SetWindowText(m_picButton->m_grayVar);
	}

	pCheckBox = (CButton*)GetDlgItem(CUS_TOOLBAR_ADV_CHKRECENTFLYOUT);
	CStatic*	pText = (CStatic*)GetDlgItem(CUS_TOOLBAR_ADV_STATICFLYOUTSONLY);
	if(pCheckBox && pText)
	{
		if(m_bFlyOut)
		{
			TBBUTTON	tbButton;
			tbButton.dwData = 0;
			pCheckBox->EnableWindow();
			pText->EnableWindow();
			/*DG - 4.10.2002*/// We should find the button on the parent toolbar, which is the parent (1st) button
			// in this flyout. This parent button should have m_bChgParentBtnImg flag set correctly, but child
			// buttons in the flyout can have different flags
			// (e.g., they are set to TRUE or FALSE by default regardless of parent flyout button flag in some places;
			// search for 'picButton->m_bChgParentBtnImg = TRUE' (or FALSE); - TO DO).
 //			if(m_picButton->m_bChgParentBtnImg)
			((CToolBar*)m_pTbMain->m_pFlyOutCurBtnDown->m_pTbOwner)->GetToolBarCtrl().GetButton(m_pTbMain->m_pFlyOutCurBtnDown->m_idxParentButton, &tbButton);
			if(((CIcadTBbutton*)tbButton.dwData)->m_bChgParentBtnImg)
				pCheckBox->SetCheck(1);
			else
				pCheckBox->SetCheck(0);
		}
		else
		{
			pCheckBox->EnableWindow(FALSE);
			pText->EnableWindow(FALSE);
			pCheckBox->SetCheck(0);
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	  // EXCEPTION: OCX Property Pages should return FALSE
}

void
CIcadCusToolBarAdvDlg::InitBitmapCombo
(
 int	nID
)
{
	CString	strBmp;
	CEdit*	pEdit = (CEdit*)GetDlgItem(nID);
	int		ct, nCurID, nCurMnuTbID;

	switch(nID)
	{
		case CUS_TB_ADV_SM_COLOR :
			strBmp = m_picButton->m_smColorBmpPath;
			nCurID = m_picButton->m_BMPIdSmColor;
			break;

		case CUS_TB_ADV_LG_COLOR :
			strBmp = m_picButton->m_lgColorBmpPath;
			nCurID = m_picButton->m_BMPIdLgColor;
			break;

		case CUS_TB_ADV_SM_BW :
			strBmp = m_picButton->m_smBwBmpPath;
			nCurID = m_picButton->m_BMPIdSmBW;
			break;

		case CUS_TB_ADV_LG_BW :
			strBmp = m_picButton->m_lgBwBmpPath;
			nCurID = m_picButton->m_BMPIdLgBW;
			break;

		default:
			return;
	}

	if(!strBmp.IsEmpty())
		pEdit->SetWindowText(strBmp);

	// Fill the listbox
	for(ct = 0; ct < g_nMnuTbItems; ++ct)
	{
		switch(nID)
		{
			case CUS_TB_ADV_SM_COLOR :
				nCurMnuTbID = g_MnuTbItems[ct].BMPIdSmColor;
				break;

			case CUS_TB_ADV_LG_COLOR :
				nCurMnuTbID = g_MnuTbItems[ct].BMPIdLgColor;
				break;

			case CUS_TB_ADV_SM_BW :
				nCurMnuTbID = g_MnuTbItems[ct].BMPIdSmBW;
				break;

			case CUS_TB_ADV_LG_BW :
				nCurMnuTbID = g_MnuTbItems[ct].BMPIdLgBW;
				break;
		}

		// Exclude items without a BMPId or ToolTip string.
		if(!g_MnuTbItems[ct].lToolID || !nCurMnuTbID || !g_MnuTbItems[ct].ToolTip || !(*g_MnuTbItems[ct].ToolTip))
			continue;

		// Set the editbox string.
		if(strBmp.IsEmpty() && nCurID == nCurMnuTbID)
			pEdit->SetWindowText(g_MnuTbItems[ct].ToolTip);

		// Add the listbox string.
		((CComboBox*)pEdit)->AddString(g_MnuTbItems[ct].ToolTip);
	}
}

void
CIcadCusToolBarAdvDlg::OnOK() 
{

	CButton*	pCheckBox;

	if(m_picButton)
	{
		m_picButton->m_lflgVisibility = 0L;
		pCheckBox = (CButton*)GetDlgItem(CUS_TOOLBAR_ADV_CHKBEGINNER);
		if(pCheckBox->GetCheck())
			m_picButton->m_lflgVisibility |= ICAD_MN_BEG;

		pCheckBox = (CButton*)GetDlgItem(CUS_TOOLBAR_ADV_CHKINTERMEDIATE);
		if(pCheckBox->GetCheck())
			m_picButton->m_lflgVisibility |= ICAD_MN_INT;

		pCheckBox = (CButton*)GetDlgItem(CUS_TOOLBAR_ADV_CHKADVANCED);
		if(pCheckBox->GetCheck())
			m_picButton->m_lflgVisibility |= ICAD_MN_EXP;

		pCheckBox = (CButton*)GetDlgItem(CUS_TOOLBAR_ADV_CHKMDIONE);
		if(pCheckBox->GetCheck())
			m_picButton->m_lflgVisibility |= ICAD_MN_OPN;

		pCheckBox = (CButton*)GetDlgItem(CUS_TOOLBAR_ADV_CHKMDINONE);
		if(pCheckBox->GetCheck())
			m_picButton->m_lflgVisibility |= ICAD_MN_CLS;

		pCheckBox = (CButton*)GetDlgItem(CUS_TOOLBAR_ADV_CHKEMBEDDED);
		if(pCheckBox->GetCheck())
			m_picButton->m_lflgVisibility |= ICAD_MN_SEM;

		pCheckBox = (CButton*)GetDlgItem(CUS_TOOLBAR_ADV_CHKINPLACE);
		if(pCheckBox->GetCheck())
			m_picButton->m_lflgVisibility |= ICAD_MN_SIP;

		pCheckBox = (CButton*)GetDlgItem(CUS_TOOLBAR_ADV_CHKCONTAINER);
		if(pCheckBox->GetCheck())
			m_picButton->m_lflgVisibility |= ICAD_MN_CLI;
	}

	BOOL	bUpdateBmp = FALSE;
	if( UpdateBmpFromCombo(CUS_TB_ADV_SM_COLOR) || UpdateBmpFromCombo(CUS_TB_ADV_LG_COLOR) ||
		UpdateBmpFromCombo(CUS_TB_ADV_SM_BW) || UpdateBmpFromCombo(CUS_TB_ADV_LG_BW) )
		bUpdateBmp = TRUE;

	CString	str;
	CEdit*	pEdit = (CEdit*)GetDlgItem(CUS_TOOLBAR_ADV_EDITCHECKVAR);
	pEdit->GetWindowText(str);
	str.TrimLeft();
	str.TrimRight();

	if(!str.IsEmpty())
		m_picButton->m_pushVar = str;

	pEdit = (CEdit*)GetDlgItem(CUS_TOOLBAR_ADV_EDITGRAYVAR);
	pEdit->GetWindowText(str);
	str.TrimLeft();
	str.TrimRight();

	if(!str.IsEmpty())
		m_picButton->m_grayVar = str;

	// Set the m_bChgParentBtnImg variable
	pCheckBox = (CButton*)GetDlgItem(CUS_TOOLBAR_ADV_CHKRECENTFLYOUT);
	if(m_bFlyOut && pCheckBox)
	{
		BOOL	bIsChecked = TRUE;

		if(!pCheckBox->GetCheck())
			bIsChecked = FALSE;

		TBBUTTON	tbButton;
		tbButton.dwData = 0;

		if(m_pTbMain->m_pFlyOutCurBtnDown)
		{
			/*DG - 4.10.2002*/// We should find the button on the parent toolbar, not on the flyout, so use CIcadFlyOut::m_pTbOwner.
//			if(m_pTbMain->m_pFlyOutCurBtnDown->GetButton(m_pTbMain->m_pFlyOutCurBtnDown->m_idxParentButton, &tbButton))
			if(((CToolBar*)m_pTbMain->m_pFlyOutCurBtnDown->m_pTbOwner)->GetToolBarCtrl().GetButton(m_pTbMain->m_pFlyOutCurBtnDown->m_idxParentButton, &tbButton))
			{
				// Make sure the parent button and all flyouts have the m_bChgParentBtnImg
				// variable set correctly.
				CIcadTBbutton*	pibCur = (CIcadTBbutton*)tbButton.dwData;
				while(pibCur)
				{
					pibCur->m_bChgParentBtnImg = bIsChecked;
					pibCur = pibCur->m_pibFlyOut;
				}
			}
		}
	}

	if(bUpdateBmp)
		UpdateTbButton();

	CDialog::OnOK();
}

BOOL
CIcadCusToolBarAdvDlg::UpdateBmpFromCombo
(
 int	nComboID
)
{
	CString	str, strBmp;
	int		ct, nBMPId;
	BOOL	bUpdateBmp = FALSE;
	CEdit*	pEdit = (CEdit*)GetDlgItem(nComboID);

	pEdit->GetWindowText(str);
	str.TrimLeft();
	str.TrimRight();

	switch(nComboID)
	{
		case CUS_TB_ADV_SM_COLOR :
			strBmp = m_picButton->m_smColorBmpPath;
			nBMPId = m_picButton->m_BMPIdSmColor;
			break;

		case CUS_TB_ADV_LG_COLOR :
			strBmp = m_picButton->m_lgColorBmpPath;
			nBMPId = m_picButton->m_BMPIdLgColor;
			break;

		case CUS_TB_ADV_SM_BW :
			strBmp = m_picButton->m_smBwBmpPath;
			nBMPId = m_picButton->m_BMPIdSmBW;
			break;

		case CUS_TB_ADV_LG_BW :
			strBmp = m_picButton->m_lgBwBmpPath;
			nBMPId = m_picButton->m_BMPIdLgBW;
			break;

		default:
			return FALSE;
	}

	if(!str.IsEmpty())
	{
		if(strBmp.IsEmpty() || str.CompareNoCase(strBmp))
		{
			if(!strBmp.IsEmpty())
				bUpdateBmp = TRUE;

			// If the string from the editbox is not a bitmap then we need
			// to find it in the command array.
			// EBATECH-[ accept ".BMP"
			//if(str.Find(".bmp") > -1)
			char drive[_MAX_DRIVE];
			char dir[_MAX_DIR];
			char fname[_MAX_FNAME];
			char ext[_MAX_EXT];
			_splitpath(str.GetBuffer(MAX_PATH), drive, dir, fname, ext);
			CString str2 = ext;
			str2.MakeUpper();
			if(str2.Find(".BMP"/*DNT*/) > -1) //]-EBATECH
			{
				strBmp = str;
				bUpdateBmp = TRUE;
			}
			else
			{
				int	nNewBMPId;

				for(ct = 0; ct < g_nMnuTbItems; ++ct)
				{
					// Exclude items without a BMPId or ToolTip string.
					if(!g_MnuTbItems[ct].lToolID || !g_MnuTbItems[ct].ToolTip || !(*g_MnuTbItems[ct].ToolTip))
						continue;

					if(!str.CompareNoCase(g_MnuTbItems[ct].ToolTip))
					{
						switch(nComboID)
						{
							case CUS_TB_ADV_SM_COLOR :
								nNewBMPId = g_MnuTbItems[ct].BMPIdSmColor;
								break;

							case CUS_TB_ADV_LG_COLOR :
								nNewBMPId = g_MnuTbItems[ct].BMPIdLgColor;
								break;

							case CUS_TB_ADV_SM_BW :
								nNewBMPId = g_MnuTbItems[ct].BMPIdSmBW;
								break;

							case CUS_TB_ADV_LG_BW :
								nNewBMPId = g_MnuTbItems[ct].BMPIdLgBW;
								break;
						}

						if(nBMPId != nNewBMPId)
						{
							nBMPId = nNewBMPId;
							bUpdateBmp= TRUE;
						}

						break;
					}
				}	// for(
			}
		}
	}
	else
		if(!strBmp.IsEmpty() && IDB_ICAD_GENERIC != nBMPId)
		{
			// If there is no string in the editbox, use the default bitmap.
			strBmp.Empty();
			nBMPId = IDB_ICAD_GENERIC;
			bUpdateBmp = TRUE;
		}

	if(bUpdateBmp)
	{
		switch(nComboID)
		{
			case CUS_TB_ADV_SM_COLOR :
				m_picButton->m_smColorBmpPath = strBmp;
				m_picButton->m_BMPIdSmColor = nBMPId;
				break;

			case CUS_TB_ADV_LG_COLOR :
				m_picButton->m_lgColorBmpPath = strBmp;
				m_picButton->m_BMPIdLgColor = nBMPId;
				break;

			case CUS_TB_ADV_SM_BW :
				m_picButton->m_smBwBmpPath = strBmp;
				m_picButton->m_BMPIdSmBW = nBMPId;
				break;

			case CUS_TB_ADV_LG_BW :
				m_picButton->m_lgBwBmpPath = strBmp;
				m_picButton->m_BMPIdLgBW = nBMPId;
		}
	}

	return bUpdateBmp;
}

void
CIcadCusToolBarAdvDlg::UpdateTbButton()
{
	CToolBarCtrl*	pTbCtrl = NULL;

	if(m_pTbMain->m_pToolBarCurBtnDown)
		pTbCtrl = &(m_pTbMain->m_pToolBarCurBtnDown->GetToolBarCtrl());
	else
		if(m_pTbMain->m_pFlyOutCurBtnDown)
			pTbCtrl = (CToolBarCtrl*)m_pTbMain->m_pFlyOutCurBtnDown;

	int			idxCurButtonDown = m_pTbMain->m_idxCurButtonDown;
	TBBUTTON	tbButton;

	pTbCtrl->GetButton(idxCurButtonDown, &tbButton);
	pTbCtrl->DeleteButton(idxCurButtonDown);

	CBitmap*	pBmp = NULL;

	if(m_pTbMain->m_pToolBarCurBtnDown)
		tbButton.iBitmap = m_pTbMain->AddIcadBitmap(m_picButton, 0, m_pTbMain->m_pToolBarCurBtnDown);
	else
		if(m_pTbMain->m_pFlyOutCurBtnDown)
		{
			CBitmap*	pBmp = NULL;
			m_pTbMain->AddIcadBitmap(m_picButton, 0, NULL, &pBmp);
			if(pBmp)
			{
				int	nBmpID = 0;
				m_pTbMain->AddToolBarBitmap(NULL, m_pTbMain->m_pFlyOutCurBtnDown, pBmp, &nBmpID);
				tbButton.iBitmap = nBmpID;
				delete pBmp;
			}

			// Add the flyout version of this bitmap to the parent button.
			CIcadTBbutton*	pibNew = (CIcadTBbutton*)tbButton.dwData;
			pibNew->m_idxFlyOutImage = m_pTbMain->AddIcadBitmap(pibNew, 1, (CIcadToolBar*)m_pTbMain->m_pFlyOutCurBtnDown->m_pTbOwner);
		}

	pTbCtrl->InsertButton(idxCurButtonDown, &tbButton);
}

void
CIcadCusToolBarAdvDlg::OnBrowseLgBw() 
{
	BrowseBitmaps(CUS_TB_ADV_LG_BW);
	return;
}

void
CIcadCusToolBarAdvDlg::OnBrowseLgColor() 
{
	BrowseBitmaps(CUS_TB_ADV_LG_COLOR);
	return;
}

void
CIcadCusToolBarAdvDlg::OnBrowseSmBw() 
{
	BrowseBitmaps(CUS_TB_ADV_SM_BW);
	return;
}

void
CIcadCusToolBarAdvDlg::OnBrowseSmColor() 
{
	BrowseBitmaps(CUS_TB_ADV_SM_COLOR);
	return;
}

const char*	pSELECT_BITMAP_STR_1 = "Select Bitmap";
const char*	pSELECT_BITMAP_STR_2 = "Windows Bitmap Image|bmp";

BOOL
CIcadCusToolBarAdvDlg::BrowseBitmaps
(
 int	nEditBoxID
)
{
	CEdit*	pEdit = (CEdit*)GetDlgItem(nEditBoxID);
	if(!pEdit)
		return FALSE;

	// If there is an existing pathname in the editbox, use it as the default pathname.
	CString	strBmpPath;
	pEdit->GetWindowText(strBmpPath);

	// Get the new pathname
	// EBATECH(CNBR) 2002/4/30 Disable TYPE button option+2
	resbuf	rb;
	if(sds_getfiled(pSELECT_BITMAP_STR_1, strBmpPath, pSELECT_BITMAP_STR_2, 4+2, &rb) != RTNORM)
		return FALSE;

	if(RTSTR == rb.restype)
	{
		strBmpPath = rb.resval.rstring;
		IC_FREE(rb.resval.rstring);
	}

	// Set the new pathname
	pEdit->SetWindowText(strBmpPath);

	return TRUE;
}

// EBATECH{
// Bug No 77925(+) from BugZilla
void CIcadCusToolBarAdvDlg::OnHelp()
	{

	CommandRefHelp(m_hWnd, HLP_TCOPTIONS);
	}
// }
