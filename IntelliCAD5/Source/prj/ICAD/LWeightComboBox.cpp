/* File  : <DevDir>\source\prj\icad\LWeightComboBox.cpp
 *
 * Abstract
 * implementation file
 */

#include "Icad.h"/*DNT*/
#include "IcadApi.h"/*DNT*/
#include "LWeightComboBox.h"/*DNT*/
#include "cmds.h"/*DNT*/
#include "icaddoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


///////////////////////////////////////////////////////////

CLWeightComboBox::CLWeightComboBox()
{
}

CLWeightComboBox::~CLWeightComboBox()
{
}

BEGIN_MESSAGE_MAP(CLWeightComboBox, CComboBox)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)
	ON_CONTROL_REFLECT(CBN_CLOSEUP, OnCloseUp)
	ON_WM_CREATE()
END_MESSAGE_MAP()
///////////////////////////////////////////////////////////

/********************************************************************************
 * Author:	Mohan Nayak.														*
 *																				*
 * Purpose:	Create the combo box.												*
 *																				*
 * Returns:	bool																*
 ********************************************************************************/
BOOL CLWeightComboBox::Create(const RECT& rect, CWnd* pParentWnd)
{
	CRect ctrl_rect = rect;
	ctrl_rect.bottom += 200;

	m_ToolTipRect.top = 0;
	m_ToolTipRect.left = 0;
	m_ToolTipRect.right = 200;
	m_ToolTipRect.bottom = 25;

	// Default Display Scale Size.
	m_DisplayScaleFactor = 20;

	return CComboBox::Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL |  
							CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_HASSTRINGS, ctrl_rect, pParentWnd, IDC_LWEIGHT_COMBOBOX);

}


/********************************************************************************
 * Author:	Mohan Nayak.														*
 *																				*
 * Purpose:	Set default font for combo box.										*
 *																				*
 * Returns:	int																	*
 ********************************************************************************/
int CLWeightComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;

	CFont *pFont = new CFont();
	if(pFont)
	{
		pFont->CreateStockObject(DEFAULT_GUI_FONT);
		SetFont(pFont);
	}

	delete pFont;
	pFont = NULL;

	return 0;
}


/********************************************************************************
 * Author:	Mohan Nayak.														*
 *																				*
 * Purpose:	Update the contents in Lweight combo box.							*
 *																				*
 * Returns:	void																*
 ********************************************************************************/
void CLWeightComboBox::UpdateLWeightList()
{
    ResetContent();

	struct resbuf rb;
	CString strTmp;
	int top;

	if( SDS_CURDWG == NULL)
		return;

	// Check for unit - mm or In 
	SDS_getvar(NULL,DB_QLWUNITS,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	top = (rb.resval.rint == 0 ? IDC_COMBO_I_LWDEFAULT : IDC_COMBO_M_LWDEFAULT);

	SItemData* pItemData = NULL;
	
	// Fill ComboBox with all the available options.
	int itemCount;
	for (itemCount = 0; itemCount < 3; itemCount++)
	{
		pItemData = new SItemData;
		pItemData->width = db_index2lweight(itemCount - 3);
		if (pItemData->width == -3)
		{
			SDS_getvar(NULL,DB_QLWDEFAULT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			pItemData->width = rb.resval.rint;
		}

		strTmp.Format("LWeight %d", db_index2lweight(itemCount - 3));
		::lstrcpy((LPTSTR)pItemData->strDisplay, ResourceString(top + itemCount, strTmp));
	
		AddLineWeight(pItemData);
	}

	for(itemCount = 3; itemCount < 27; itemCount++)
	{
		pItemData = new SItemData;
		pItemData->width = db_index2lweight(itemCount - 3);

		strTmp.Format("LWeight %d", db_index2lweight(itemCount - 3));
		::lstrcpy((LPTSTR)pItemData->strDisplay, ResourceString(top + itemCount, strTmp));
	
		AddLineWeight(pItemData);
	}
	
	// Get current lweight value and show the corresponding combo item.
	SDS_getvar(NULL,DB_QCELWEIGHT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	SetCurSel(FindLWeight(rb.resval.rint));

	RedrawWindow();
}



/********************************************************************************
 * Author:	Mohan Nayak.														*
 *																				*
 * Purpose:	Change the focus to main window.									*
 *																				*
 * Returns:	void																*
 ********************************************************************************/
void CLWeightComboBox::OnCloseUp()
{
	SDS_CMainWindow->SetFocus();
}


/********************************************************************************
 * Author:	Mohan Nayak.														*
 *																				*
 * Purpose:	Change the selected entity's lineweight and set the current line	*
 *			weight.																*
 *																				*
 * Returns:	void																*
 ********************************************************************************/
void CLWeightComboBox::OnSelchange()
{
	struct resbuf rb;
	int nSel = GetCurSel();
	
	long ret, fl1;
	sds_name ename;

	// Change the selected entity's lweight.
	if ((ret = SDS_getvar(NULL, DB_QPICKFIRST, &rb,SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES)) == RTNORM)
	{
		if (rb.resval.rint && SDS_CURDOC && RTNORM==sds_sslength(SDS_CURDOC->m_pGripSelection,&fl1) && fl1>0L)
		{
			fl1--;
			while (RTNORM==sds_ssname(SDS_CURDOC->m_pGripSelection, fl1, ename)) 
			{
				resbuf *prb=sds_entget(ename);
				ic_assoc(prb, 370);

				ic_rbassoc->resval.rint = db_index2lweight(nSel - 3);
				sds_entmod(prb);
				sds_redraw(ename, IC_REDRAW_HILITE);

				IC_RELRB(prb);
				sds_redraw(ename, IC_REDRAW_DRAW);        
				fl1--;
			}
		}
	}
	
	
	sds_sslength(SDS_CURDOC->m_pGripSelection, &fl1); 

	// in case no entity is selected, set the current lweight.
	if(fl1==0) 
	{ 
		rb.restype=SDS_RTSHORT;
		rb.resval.rint = db_index2lweight (nSel-3);
		sds_setvar("CELWEIGHT"/*DNT*/,&rb);	
	}

	SDS_CMainWindow->SetFocus();
}


/********************************************************************************
 * Author:	Mohan Nayak.														*
 *																				*
 * Purpose:	Add ToolTip.														*
 *																				*
 * Returns:	void																*
 ********************************************************************************/
void CLWeightComboBox::AddToolTip()
{
	m_LWeightToolTop.Create(this);
	m_LWeightToolTop.AddTool(this,ResourceString(IDC_TBMNU_1477,"Lineweight control"/*DNT*/),&m_ToolTipRect,IDB_ICAD_LWEIGHT_CTRL);
}


BOOL CLWeightComboBox::PreTranslateMessage(MSG* pMsg) 
{
	m_LWeightToolTop.Activate(TRUE);
	m_LWeightToolTop.RelayEvent(pMsg);
	return CComboBox::PreTranslateMessage(pMsg);
}

void CLWeightComboBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);

	CRect rect = lpDrawItemStruct->rcItem;
    int nIndex = lpDrawItemStruct->itemID;

    CBrush* pBrush = new CBrush;
    pBrush->CreateSolidBrush(
				::GetSysColor((lpDrawItemStruct->itemState & ODS_SELECTED) ? 
													COLOR_HIGHLIGHT : COLOR_WINDOW
							  )
							);

    dc.FillRect(rect, pBrush);
    delete pBrush;

    if (lpDrawItemStruct->itemState & ODS_FOCUS)
        dc.DrawFocusRect(rect);

	if (nIndex != (UINT)-1)
	{
		//int nWidth = (int)GetItemDataPtr(nIndex);
		SItemData* pItemData = (SItemData*)(GetItemDataPtr(nIndex));
		
		CString strDisplay(pItemData->strDisplay);
		int nThickness = 1;

		if (pItemData->width > 0)
		{
			// calculate actual thickness to be shown. (range = 1 to 42 pixels max)
			double temp1 = ((double)(m_DisplayScaleFactor * pItemData->width) / 100.0) + 0.5;
			
			// Bring it in the scale of range 1 to 14) 
			double temp2 = (14 * temp1) / 42.0;
			
			if (temp2 < 1.0)
				nThickness = 1;
			else
				nThickness = (int)floor(temp2);
		}


		pBrush = new CBrush;
		if (lpDrawItemStruct->itemState & ODS_SELECTED)
		{
			pBrush->CreateSolidBrush(RGB(255, 255, 255));
			dc.SetTextColor(RGB(255, 255, 255));
		}
		else
		{
			pBrush->CreateSolidBrush(RGB(0, 0, 0));
			dc.SetTextColor(RGB(0, 0, 0));
		}

		dc.SetBkMode(TRANSPARENT);

		CRect solidRect;
		solidRect.left	= 6;
		solidRect.right = 88;
		if (nThickness == 1)
		{
			solidRect.top		= 7 + (rect.top + 2);
			solidRect.bottom	= 8 + (rect.top + 2);
		}
		else
		{
			solidRect.top		= 8 - ((nThickness + 1) / 2) + (rect.top + 2);
			solidRect.bottom	= 8 + ((nThickness + 1) / 2) + (rect.top + 2);
		}
		
		dc.FillRect(solidRect, pBrush);
		delete pBrush;

		dc.TextOut(97, rect.top + 2, strDisplay);
	}


	dc.Detach();
}

void CLWeightComboBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{

}

void CLWeightComboBox::AddLineWeight(void* pItemData)
{
	int nIndex = AddString (_T (""));
    if ((nIndex != CB_ERR) && (nIndex != CB_ERRSPACE))
		SetItemDataPtr(nIndex, pItemData);
}


/********************************************************************************
 * Author:	Mohan Nayak.														*
 *																				*
 * Purpose:	To return the index of the Item in Combo-Box that has the Line		*
 *			weight specified by nLineweight.									*
 *																				*
 * Parameters: nLineweight is in hundreths of mm, or -1, -2, -3, as returned	*
			   by CELWEIGHT.													*
 *																				*
 * Returns:	int																*
 ********************************************************************************/
int CLWeightComboBox::FindLWeight (int nLineweight)
{
	SItemData* pItemData = NULL;
	
	if (nLineweight < 0)
	{
		switch(nLineweight)
		{
			case -1: return 2;
			case -2: return 1;
			case -3: return 0;
		}
	}
	else
	{
		for (int iIndex = 3; iIndex < 27; iIndex++)
		{
			pItemData = (SItemData*)(GetItemDataPtr(iIndex));
			if (pItemData->width == nLineweight)
				return iIndex;
		}
	}

	return -1;
}


void CLWeightComboBox::DeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct)
{
	ASSERT(lpDeleteItemStruct->CtlType == ODT_COMBOBOX);
	
	LPVOID lpItemData = (LPVOID)(lpDeleteItemStruct->itemData);

	if (lpItemData != NULL)
	{
		SItemData* pItemData = (SItemData*)lpItemData;
		delete pItemData;
		pItemData = NULL;
	}
}