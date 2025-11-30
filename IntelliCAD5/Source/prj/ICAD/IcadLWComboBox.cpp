// IcadLWComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "IcadLWComboBox.h"

#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIcadNewLWComboBox

CIcadNewLWComboBox::CIcadNewLWComboBox()
{
	// Temporary Initialisation
	m_nDisplayScaleFactor = 2;
	m_bDisplayValuesInMM = true;
}

CIcadNewLWComboBox::~CIcadNewLWComboBox()
{
}


BEGIN_MESSAGE_MAP(CIcadNewLWComboBox, CComboBox)
	//{{AFX_MSG_MAP(CIcadNewLWComboBox)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIcadNewLWComboBox message handlers

void CIcadNewLWComboBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your code to draw the specified item

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
		int nWidth = (int)GetItemData(nIndex);
		
		CString strDisplay;
		int nThickness = 1;

		if (nWidth < 0)
		{
			
			// Not in case of Dialog, but should be present in case of toolbar
			/*!!!
			switch (nWidth)
			{
				case -1:
					strDisplay = "ByLayer";
					break;

				case -2:
					strDisplay = "ByBlock";
					break;

				case -3:
					strDisplay = "Default";					
					break;
			}
			//!!!*/
		}
		else
		{
			if (m_bDisplayValuesInMM)
				strDisplay.Format("%.2f mm", (nWidth * 1.0) / 100.0);
			else
				strDisplay.Format("%.3f\"", (nWidth * 1.0) / 2540.0);

			if (nWidth > 0)
			{
				// calculate actual thickness to be shown. (range = 1 to 42 pixels max)
				double temp1 = ((double)(m_nDisplayScaleFactor * nWidth) / 100.0) + 0.5;
				
				// Bring it in the scale of range 1 to 12) 
				double temp2 = (12 * temp1) / 42.0;
				
				if (temp2 < 1.0)
					nThickness = 1;
				else
					nThickness = (int)floor(temp2);
			}
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
		solidRect.right = 15;
		if (nThickness == 1)
		{
			solidRect.top		= 6 + rect.top;
			solidRect.bottom	= 7 + rect.top;
		}
		else
		{
			solidRect.top		= 7 - ((nThickness + 1) / 2) + rect.top;
			solidRect.bottom	= 7 + ((nThickness + 1) / 2) + rect.top;
		}
		
		dc.FillRect(solidRect, pBrush);
		delete pBrush;

		dc.TextOut(20, 2 + rect.top, strDisplay);
	}


	dc.Detach();


}

void CIcadNewLWComboBox::AddLineWeight(int nLineWeight)
{
	int nIndex = AddString (_T (""));
    if ((nIndex != CB_ERR) && (nIndex != CB_ERRSPACE))
		SetItemData(nIndex, (DWORD)nLineWeight);

}

int CIcadNewLWComboBox::GetCurrentDefaultLWeight()
{
	return ((int)GetItemData(GetCurSel()));
}