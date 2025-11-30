// IcadLWListBox.cpp : implementation file
//

#include "stdafx.h"
#include "IcadLWListBox.h"

#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIcadLWListBox

CIcadLWListBox::CIcadLWListBox()
{
	m_nDisplayScaleFactor = 2;
	m_bDisplayValuesInMM = true;
}

CIcadLWListBox::~CIcadLWListBox()
{
}


BEGIN_MESSAGE_MAP(CIcadLWListBox, CListBox)
	//{{AFX_MSG_MAP(CIcadLWListBox)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIcadLWListBox message handlers

void CIcadLWListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
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

		if (nIndex < 3)
		{
			switch (nIndex)
			{
				case 0:
					strDisplay = "BYLAYER";
					break;

				case 1:
					strDisplay = "BYBLOCK";
					break;

				case 2:
					strDisplay = "Default";					
					break;
			}
		}
		else
		{
			if (m_bDisplayValuesInMM)
				strDisplay.Format("%.2f mm", (nWidth * 1.0) / 100.0);
			else
				strDisplay.Format("%.3f\"", (nWidth * 1.0) / 2540.0);
		}

		if (nWidth > 0)
		{
			// calculate actual thickness to be shown. (range = 1 to 42 pixels max)
			double temp1 = ((double)(m_nDisplayScaleFactor * nWidth) / 100.0) + 0.5;
			
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
		solidRect.left	= 3;
		solidRect.right = 42;
		if (nThickness == 1)
		{
			solidRect.top		= 7 + rect.top;
			solidRect.bottom	= 8 + rect.top;
		}
		else
		{
			solidRect.top		= 8 - ((nThickness + 1) / 2) + rect.top;
			solidRect.bottom	= 8 + ((nThickness + 1) / 2) + rect.top;
		}
		
		dc.FillRect(solidRect, pBrush);
		delete pBrush;

		dc.TextOut(46, 1 + rect.top, strDisplay);
	}


	dc.Detach();
}


void CIcadLWListBox::AddLineWeight(int nLineWeight)
{
	int nIndex = AddString(_T (""));
    if ((nIndex != LB_ERR) && (nIndex != LB_ERRSPACE))
		SetItemData(nIndex, (DWORD)nLineWeight);

}

void CIcadLWListBox::SetCurrentDefLWeight(int nDefLineWeight)
{
	SetItemData(2, (DWORD)nDefLineWeight);
}
