/* D:\ICADDEV\PRJ\ICAD\PRINTPENMAPTAB.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *	
 * Abstract
 *	
 * Pen Map tab on print dialog
 *	
 */ 

// PrintPenMapTab.cpp : implementation file
//

#include "icad.h"
#include "IcadApi.h"
#include "PrintPenMapTab.h"
#include "PlotProperties.h"
#include "PenMapListCtrl.h"
#include "IcadPrintDia.h"
#include "PrintDialog.h"

/////////////////////////////////////////////////////////////////////////////
// PrintPenMapTab dialog


PrintPenMapTab::PrintPenMapTab(CWnd* pParent /*=NULL*/)
	: CPropertyPage(PrintPenMapTab::IDD), m_plotProperties(NULL), editingPenMap(false),
	  m_firstMapSelect(TRUE), m_editBoxSelected(NONE), m_bDataUpdated(FALSE)


{

	m_penList = new PenMapListCtrl(this);

	//{{AFX_DATA_INIT(PrintPenMapTab)
	m_lineWidth = _T("");
	m_drawingColor = _T("");
	m_outputColor = _T("");
	m_lineWidthUnits = _T("");
	//}}AFX_DATA_INIT

}

PrintPenMapTab::~PrintPenMapTab()
{
	if (m_penList)
		delete m_penList;
}

void PrintPenMapTab::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, PRN_PENMAP, *m_penList);

	//{{AFX_DATA_MAP(PrintPenMapTab)
	DDX_Text(pDX, PRN_LINE_WIDTH, m_lineWidth);
	DDX_Text(pDX, PRN_DRAWING_COLOR_EDIT, m_drawingColor);
	DDX_Text(pDX, PRN_OUTPUT_COLOR, m_outputColor);
	DDX_Text(pDX, PRN_LINE_WIDTH_UNITS, m_lineWidthUnits);
	//}}AFX_DATA_MAP
}

BOOL PrintPenMapTab::ValidWidth(CString currentWidth)
{
	double width;
	BOOL result = FALSE;

	if (ic_isnumstr(currentWidth.GetBuffer(0), &width))
		if ((width < 10) && (width >= 0))
			result = TRUE;

	if (result == FALSE)
		AfxMessageBox("Invalid width entry. Please enter a valid number.");

	return result;
}


void PrintPenMapTab::LoadPlotSettings()
{
	// Load latest pen map

	short outColor;
	double width;
	TCHAR widthStr[20];

	HRESULT result = -1;

	m_penList->DeleteAllItems();

	if (m_plotProperties)
	{
		IIcadPlotterPens	*pens;
		result = m_plotProperties->get_Pens(&pens);

		if (SUCCEEDED(result))
		{
			VARIANT	index;
			index.vt = VT_I2;

			for (index.iVal = 1; index.iVal < 256; index.iVal++)
			{
				CComPtr< IIcadPlotterPen>	plotterPen;
				result = pens->get_Item( index, &plotterPen);

				if (SUCCEEDED(result))
				{
					int	i = index.iVal;
					m_penList->InsertItem(i - 1, ColorToString(i), NULL);  

					plotterPen->get_Number(&outColor);
					m_penList->SetItemText(i - 1, 1, ColorToString(outColor));

					plotterPen->get_Weight(&width);

					m_penList->SetActualWidth(i - 1, width);
					sprintf(widthStr,"%.4f", width);
					m_penList->SetItemText(i - 1, 2, widthStr);
				}					
			}
			pens->Release();
		}
		// EBATECH(CNBR) -[ Bugzilla:78569 Read units here to compare current layout value.
		CString units;
		BSTR unitsBstr;

		result = m_plotProperties->get_Units(&unitsBstr);

		if (SUCCEEDED(result) && unitsBstr)
		{
			units = unitsBstr;
			
			if (units == "_I"/*DNT*/)
			{
				m_lineWidthUnits = ResourceString(IDC_ICADPRINT_INCHES, "Inches");
			}
			else
			{
				m_lineWidthUnits = ResourceString(IDC_ICADPRINT_MILLIMETERS, "Millimeters");
			}

			SysFreeString(unitsBstr);
		}
		// EBATECH(CNBR) ]-
	}

	// if anything was unsuccessful - set default values.
	if (!SUCCEEDED(result))
		SetDefaults();

	else // succeeded
		UpdateData(FALSE);
}

void PrintPenMapTab::SavePlotSettings()
{
	CString listData;
	double width = 0;

	HRESULT result = -1;
	UpdateData(TRUE);

	if (m_plotProperties)
	{
		IIcadPlotterPens	*pens;
		result = m_plotProperties->get_Pens(&pens);

		if (SUCCEEDED(result))
		{
			VARIANT	index;
			index.vt = VT_I2;

			for (index.iVal = 1; index.iVal < 256; index.iVal++)
			{
				CComPtr< IIcadPlotterPen>	plotterPen;
				result = pens->get_Item( index, &plotterPen);

				if (SUCCEEDED(result))
				{
					int i = index.iVal;

					listData = m_penList->GetItemText(i - 1, 1);
					plotterPen->put_Number(StringToColor(listData));

					width = m_penList->GetActualWidth(i - 1);
					plotterPen->put_Weight(width);
				}
			}

			pens->Release();
		}
	}

}


// offset = how far away from the edge of the control to paint
void PrintPenMapTab::PaintItem(int id, int color, int offset)
{
	CRect rect;
	CBrush brush;
	HWND hItemWnd=::GetDlgItem(m_hWnd, id);
	HDC hDC=::GetDC(hItemWnd);
	HPALETTE oldpal=::SelectPalette(hDC,(HPALETTE)SDS_CMainWindow->m_pPalette->GetSafeHandle(),TRUE);

	// if invalid color set to default
	if ((color <= 0) || (color > 256))
	{
		color = GetSysColor(COLOR_3DFACE);
		brush.CreateSolidBrush(color);
	}
	else
	{
		if(color == 256)
			brush.CreateSolidBrush(RGB(0,0,0));
		else
			brush.CreateSolidBrush(SDS_BrushColorFromACADColor(color));
	}

	::GetClientRect(hItemWnd,&rect);
	rect.left += offset;
	rect.top += offset;
	rect.bottom -= offset;
	rect.right -= offset;
	::FillRect(hDC,&rect,(HBRUSH)brush);
	brush.DeleteObject();
	::ReleaseDC(hItemWnd,hDC);
}

// returns color
int PrintPenMapTab::SetColorFromString(CString colorStr)
{
	int color;

	if (colorStr.IsEmpty() || (colorStr == ResourceString(IDC_ICADPRINT_VARIES, "varies")))
		return -1;

	color = StringToColor(colorStr);

	if ((color < 1) || (color > 256))
		return -1;

	return color;
}

void PrintPenMapTab::LineWidthEdited()
{
	m_lineWidth = m_penList->GetItemText(m_penList->editRow, m_penList->editColumn);
	m_penList->SetActualWidth(m_penList->editRow, atof(m_lineWidth));

	UpdateData(FALSE);

	editingPenMap = false;
}

short PrintPenMapTab::StringToColor(CString colorStr)
{
	if (colorStr == ResourceString(IDC_COLOR_BLACK, "black"))
		return 256;

	return ic_colornum(colorStr);
}

CString PrintPenMapTab::ColorToString(short colorNum)
{
	if (colorNum == 256)
		return ResourceString(IDC_COLOR_BLACK, "black");

	return ic_colorstr(colorNum, NULL);
}


void PrintPenMapTab::SetUnits(int units)
{
	double conversionFactor;

//	if (units == m_currentUnits)
//		return;

	if (units == PRN_OPT_INCHES)
	{
		// Ebatech(CNBR) -[ Bugzilla:78569
		// If units is same as pen map units, nothing to do.
		if( m_lineWidthUnits == ResourceString(IDC_ICADPRINT_INCHES, "Inches"))
			return;
		// Ebatech(CNBR) ]-
		//mm to inches
		conversionFactor = 1 / 25.4;	

		//change label
		m_lineWidthUnits = ResourceString(IDC_ICADPRINT_INCHES, "Inches");
	}

	else //units == PRN_OPT_MM
	{
		// Ebatech(CNBR) -[ Bugzilla:78569
		// If units is same as pen map units, nothing to do.
		if( m_lineWidthUnits == ResourceString(IDC_ICADPRINT_MILLIMETERS, "Millimeters"))
			return;
		// Ebatech(CNBR) ]-
		//inches to mm
		conversionFactor = 25.4;

		//change label
		m_lineWidthUnits = ResourceString(IDC_ICADPRINT_MILLIMETERS, "Millimeters");
	}

	double width;
	CString widthStr;
	for (int i = 1; i < 256; i++)
	{
		width = m_penList->GetActualWidth(i - 1);
		width = width * conversionFactor;
		m_penList->SetActualWidth(i - 1, width);

		widthStr.Format("%.4f", width);

		m_penList->SetItemText(i - 1, 2, widthStr);
	}

	if (!m_lineWidth.IsEmpty() && (m_lineWidth != ResourceString(IDC_ICADPRINT_VARIES, "varies")))
	{
		width = atof(m_lineWidth) * conversionFactor;
		m_lineWidth.Format("%f", width);
	}

	UpdateData(FALSE);
}


BEGIN_MESSAGE_MAP(PrintPenMapTab, CPropertyPage)
	//{{AFX_MSG_MAP(PrintPenMapTab)
	ON_NOTIFY(NM_CLICK, PRN_PENMAP, OnClickPenmap)
	ON_NOTIFY(LVN_ENDLABELEDIT, PRINT_PENMAP, OnEndLabelEditPenmap)
	ON_BN_CLICKED(PRN_COLOR_BUTTON, OnColorButton)
	ON_EN_KILLFOCUS(PRN_OUTPUT_COLOR, OnKillfocusOutputColor)
	ON_NOTIFY(LVN_KEYDOWN, PRN_PENMAP, OnKeydownPenmap)
	ON_WM_PAINT()
	ON_EN_KILLFOCUS(PRN_LINE_WIDTH, OnKillfocusLineWidth)
	ON_WM_DRAWITEM()
	ON_EN_SETFOCUS(PRN_OUTPUT_COLOR, OnSetfocusOutputColor)
	ON_EN_SETFOCUS(PRN_LINE_WIDTH, OnSetfocusLineWidth)
	ON_NOTIFY(LVN_ITEMCHANGED, PRN_PENMAP, OnItemchangedPenmap)
	ON_NOTIFY(LVN_ITEMCHANGING, PRN_PENMAP, OnItemchangingPenmap)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PrintPenMapTab message handlers

BOOL PrintPenMapTab::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// Reinit these for each instance
	m_firstMapSelect = TRUE;
	m_lineWidth = _T("");
	m_drawingColor = _T("");
	m_outputColor = _T("");

	//Add pen map columns
	m_penList->InsertColumn(0, ResourceString(IDC_ICADPRINT_DRAWCOLOR, "Screen Color"), LVCFMT_LEFT, 105);
	m_penList->InsertColumn(1, ResourceString(IDC_ICADPRINT_OUTPUTCOLOR, "Output Color"), LVCFMT_LEFT, 105);
	m_penList->InsertColumn(2, ResourceString(IDC_ICADPRINT_LINEWIDTH, "Line Width"), LVCFMT_LEFT, 102);

	initSettings();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

bool PrintPenMapTab::initSettings() 
{
	LoadPlotSettings();

	// Ebatech(CNBR) -[ Bugzilla:78569 match pen width units to current layout value.
	CPrintDialogSheet *parent = (CPrintDialogSheet *) GetParent();
	SetUnits(parent->GetUnits());
	//if (parent->GetUnits() == PRN_OPT_INCHES)
	//	m_lineWidthUnits = ResourceString(IDC_ICADPRINT_INCHES, "Inches");
	//else // millimeters
	//	m_lineWidthUnits = ResourceString(IDC_ICADPRINT_MILLIMETERS, "Millimeters");
	// Ebatech(CNBR) ]-

/*	PrintDialog *parent = (PrintDialog*) GetParent();

	BSTR unitsBstr;
	int realWidthUnits = PRN_OPT_INCHES;
	HRESULT result = m_plotProperties->get_Units(&unitsBstr);
	if(SUCCEEDED(result) && unitsBstr)
	{
		realWidthUnits = (CString(unitsBstr) == "_I") ? PRN_OPT_INCHES : PRN_OPT_MM;
		SysFreeString(unitsBstr);
	}
	else
		realWidthUnits = parent->GetUnits();
	
	if(parent->GetUnits() != realWidthUnits)
	{
		SetUnits(parent->GetUnits()); 
	}
	else
	{
		if (parent->GetUnits() == PRN_OPT_INCHES)
			m_lineWidthUnits = ResourceString(IDC_ICADPRINT_INCHES, "Inches");
		else // millimeters
			m_lineWidthUnits = ResourceString(IDC_ICADPRINT_MILLIMETERS, "Millimeters");
		UpdateData(FALSE);
	}
*/
	return true;
}

void PrintPenMapTab::SetPropertiesInfo()
{
	int drawingColorNum = 0;
	int outputColorNum = 0;

	int itemNum = -1;
	int row;
	CString newStr;
	bool checkOutputColor = true;
	bool checkLineWidth = true;

	UINT numSelected = m_penList->GetSelectedCount();

	// set initial values
	row = m_penList->GetNextItem(itemNum, LVIS_SELECTED);
	if (row < 0)
		return;

	if (numSelected > 1)
	{
		m_drawingColor = ResourceString(IDC_ICADPRINT_VARIES, "varies"); // more than one item in the first column means it always varies.
		drawingColorNum = -1;
	}
	else
		m_drawingColor = m_penList->GetItemText(row, 0);


	m_outputColor = m_penList->GetItemText(row, 1);
	
	m_lineWidth.Format("%f", m_penList->GetActualWidth(row));

	itemNum = row;

	//Set all selected values
	while ((row = m_penList->GetNextItem(itemNum, LVIS_SELECTED)) >= 0)
	{
		// check output color
		newStr = m_penList->GetItemText(row, 1);
		if (checkOutputColor && (newStr != m_outputColor))
		{
			m_outputColor = ResourceString(IDC_ICADPRINT_VARIES, "varies");
			checkOutputColor = false;
		}

		// check line width
		newStr = m_penList->GetItemText(row, 2);			
		if (checkLineWidth && (newStr != m_lineWidth))
		{
			m_lineWidth = ResourceString(IDC_ICADPRINT_VARIES, "varies");
			checkLineWidth = false;
		}

	itemNum = row;
	}
	
	drawingColorNum = StringToColor(m_drawingColor);
	outputColorNum = SetColorFromString(m_outputColor);

	if ((drawingColorNum > 0) && (drawingColorNum <= 256))
		m_drawingColor = ColorToString(drawingColorNum);
	else
		m_drawingColor = ResourceString(IDC_ICADPRINT_VARIES, "varies");
	
	if ((outputColorNum > 0) && (outputColorNum <= 256))
		m_outputColor = ColorToString(outputColorNum);
	else 
		m_outputColor = ResourceString(IDC_ICADPRINT_VARIES, "varies");

	UpdateData(FALSE);

	PaintItem(PRN_DRAWING_COLOR_SWATCH, drawingColorNum, 1);
	PaintItem(PRN_COLOR_BUTTON, outputColorNum, 2);
}


void PrintPenMapTab::OnClickPenmap(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// enable edit boxes on right when something is selected.
	if (m_firstMapSelect)
	{
		CWnd *item = GetDlgItem(PRN_OUTPUT_COLOR);
		item->EnableWindow(TRUE);

		item = GetDlgItem(PRN_LINE_WIDTH);
		item->EnableWindow(TRUE);

		item = GetDlgItem(PRN_COLOR_BUTTON);
		item->EnableWindow(TRUE);

		m_firstMapSelect = FALSE;
	}

	UINT numSelected = m_penList->GetSelectedCount();

	if (numSelected == 1)
	{
		// only one selected so handle processing of other columns
		DWORD dwPos = ::GetMessagePos();
		CPoint point ((int) LOWORD (dwPos), (int) HIWORD (dwPos));
		m_penList->ScreenToClient(&point);

		int row, column;
		row = m_penList->HitTestEx(point, &column);

		// line width may have been edited last so ensure it is valid
		if (editingPenMap)
		{
			if (!m_penList->ValidateData())
			{
				return;
			}

			editingPenMap = false;
		}

		if (column == 1)
		{
			int outputColorNum = 0;
			
			if(m_outputColor == -1) 
				SDS_GetColorDialog(258, &outputColorNum, 1);
			else
				SDS_GetColorDialog(StringToColor(m_outputColor), &outputColorNum, 3);

			if ((outputColorNum <= 0) || (outputColorNum > 256))
				outputColorNum = 256; //black

			m_outputColor = ColorToString(outputColorNum);
			m_penList->SetItemText(row, 1, m_outputColor);
		}

		else if (column == 2)
		{
			m_penList->EditSubLabel(row, column);
			editingPenMap = true;
		}
	
		if ((column == 1) || (column == 2))
			m_penList->Invalidate();

		SetPropertiesInfo();
	}

	*pResult = 0;
}


void PrintPenMapTab::OnEndLabelEditPenmap(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

	if (pDispInfo->item.iSubItem == 2)
	{
		CString currentItem = m_penList->GetItemText(pDispInfo->item.iItem, pDispInfo->item.iSubItem);

		if (ValidWidth(pDispInfo->item.pszText))
		{
			m_penList->SetItemText(pDispInfo->item.iItem, pDispInfo->item.iSubItem, pDispInfo->item.pszText);
		}
	}

	*pResult = 0;
}

void PrintPenMapTab::OnColorButton() 
{
	int outputColorNum;
	
	outputColorNum = SetColorFromString(m_outputColor);

	if(outputColorNum == -1) 
		SDS_GetColorDialog(258, &outputColorNum, 1);
	else
		SDS_GetColorDialog(outputColorNum, &outputColorNum,3);

	if ((outputColorNum <= 0) || (outputColorNum > 256))
		outputColorNum = 256; //black

	m_outputColor = ColorToString(outputColorNum);

	//Set all selected values
	int itemNum = -1;
	int row;
	while ((row = m_penList->GetNextItem(itemNum, LVIS_SELECTED)) >= 0)
	{
		m_penList->SetItemText(row, 1, m_outputColor);
		itemNum = row;
	}
	
	UpdateData(FALSE);

	PaintItem(PRN_COLOR_BUTTON, outputColorNum, 2);
}

void PrintPenMapTab::OnKillfocusOutputColor() 
{
	//BugZilla No. 78341; 21-10-2002 
	CString outputColor;
	CWnd *item = GetDlgItem(PRN_OUTPUT_COLOR);
	item->GetWindowText(outputColor);		

	// if user clicks away just return
	if (outputColor.IsEmpty() )
		return;

	int outputColorNum = SetColorFromString(outputColor);
	if (outputColorNum < 1)
	{
		MessageBox(ResourceString(IDC_ICADPRINT_INVALIDCOLOR, "Please enter an integer between 1 and 255."),
				   ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD" ), MB_OK | MB_ICONEXCLAMATION);

		m_outputColor = "";
		UpdateData(FALSE);
		item->SetFocus();
	
		return;
	}

	UpdateData(TRUE );

	m_outputColor = ColorToString(outputColorNum);

	//Set all selected values
	int itemNum = -1;
	int row;

	while ((row = m_penList->GetNextItem(itemNum, LVIS_SELECTED)) >= 0)
	{
		m_penList->SetItemText(row, 1, m_outputColor);
		itemNum = row;
	}
	
	PaintItem(PRN_COLOR_BUTTON, outputColorNum, 2);

	UpdateData(FALSE);
}

void PrintPenMapTab::OnKillfocusLineWidth() 
{
	
	//BugZilla No. 78341; 21-10-2002 
	CString lineWidth;
	CWnd *item = GetDlgItem(PRN_LINE_WIDTH);
	item->GetWindowText(lineWidth);

	// if user clicks away just return
	//if (lineWidth.IsEmpty() || lineWidth == "varies")
	if (lineWidth.IsEmpty() )
		return;

	double lineWidthNum = atof(lineWidth);

	if ((!lineWidthNum) && (lineWidth[0] != '0'))
	{
		MessageBox(ResourceString(IDC_ICADPRINT_INVALIDWIDTH, "Please enter a numeric value."),
				   ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD" ), MB_OK | MB_ICONEXCLAMATION);

		m_lineWidth = "";
		UpdateData(FALSE);
		item->SetFocus();

		return;
	}
	
	UpdateData(TRUE);

	//Set all selected values
	int itemNum = -1;
	int row;
	
	// set precision for m_lineWidth
	TCHAR widthStr[20];

	sprintf(widthStr,"%.4f", lineWidthNum);
	m_lineWidth.Format("%f", lineWidthNum);

	while ((row = m_penList->GetNextItem(itemNum, LVIS_SELECTED)) >= 0)
	{
		m_penList->SetActualWidth(row, lineWidthNum);
		m_penList->SetItemText(row, 2, widthStr);
		itemNum = row;
	}

	UpdateData(FALSE);	// to reset precision
}


void PrintPenMapTab::OnKeydownPenmap(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	 
	switch(pLVKeyDow->wVKey)
	{
	case 'a':
	case 'A': 
		{
			if (GetKeyState(VK_CONTROL))
			{
				int count = m_penList->GetItemCount();

				for(int i = 0; i < count; i++)
					m_penList->SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
			}
		}	  
		break;

	case VK_ESCAPE:
		GetParent()->SendMessage(WM_CHAR, pLVKeyDow->wVKey, 0);
		break;
	}

	*pResult = 0;
}

void PrintPenMapTab::OnPaint() 
{
	CPropertyPage::OnPaint();
	
	PaintItem(PRN_DRAWING_COLOR_SWATCH, SetColorFromString(m_drawingColor), 1);
	m_penList->Invalidate();
}


BOOL PrintPenMapTab::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	int wNotifyCode = HIWORD(wParam); // notification code 
	int wID = LOWORD(wParam);		  // item, control, or accelerator identifier 
	HWND hwndCtl = (HWND) lParam;	   // handle of control 	

	// don't know the #defined version of this message.
	if (wNotifyCode == 0)
	{
		// want to send escape codes to parent
		if (wID == 2)
			return GetParent()->SendMessage(WM_COMMAND, wParam, lParam);

		// if user clicks enter in a edit box, ignore it.
		if (wID == 1)
		{
			switch (m_editBoxSelected)
			{
			case OUTPUT_COLOR:
				OnKillfocusOutputColor();
				break;

			case LINE_WIDTH:
				OnKillfocusLineWidth();
				break;
			}

			return TRUE;
		}
	}

	return CPropertyPage::OnCommand(wParam, lParam);
}


void PrintPenMapTab::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	switch(nIDCtl)
	{
	case PRN_COLOR_BUTTON:
		{
			ASSERT(lpDrawItemStruct != NULL);		
			
			CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);	
			CRect rect, rect2;
			CBrush brush;
			brush.CreateSolidBrush(GetSysColor(COLOR_3DFACE));

			// get the rectangle portion
			rect.CopyRect(&lpDrawItemStruct->rcItem);
			rect2.CopyRect(rect);
			
			rect2.left += 1;
			rect2.top += 1;
			rect2.bottom -= 1;
			rect2.right -= 1;

			
			// draw the up/down/focused/disabled state	
			UINT state = lpDrawItemStruct->itemState;	 
			
			if (state & ODS_SELECTED) 
			{
				// draw the down edges		
				pDC->RoundRect(rect.left, rect.top, rect.Width(), rect.Height(), 2, 2);
				pDC->SelectObject(brush);
				
				pDC->Draw3dRect (rect, GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_3DHILIGHT));
				pDC->Draw3dRect (rect2, GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_3DHILIGHT));
			}	
			else
			{	
				pDC->RoundRect(rect.left, rect.top, rect.Width(), rect.Height(), 2, 2);
				pDC->SelectObject(brush);

				// draw the up edges		
				pDC->Draw3dRect (rect, GetSysColor(COLOR_3DHILIGHT), GetSysColor(COLOR_3DSHADOW));	  
				pDC->Draw3dRect (rect2, GetSysColor(COLOR_3DHILIGHT), GetSysColor(COLOR_3DSHADOW));    
			}
			brush.DeleteObject();

			PaintItem(PRN_COLOR_BUTTON, SetColorFromString(m_outputColor), 2);
		}
		break;

	case PRN_PENMAP:
		m_penList->DrawItem(lpDrawItemStruct);
		break;

	default:
		CPropertyPage::OnDrawItem(nIDCtl, lpDrawItemStruct);
	}
}

void PrintPenMapTab::SetDefaults()
{
	m_penList->DeleteAllItems();

	TCHAR widthStr[20];
	double width = (double)0.01;
	
	sprintf(widthStr,"%.4f", width);

	for (int i = 1; i < 256; i++)
	{
		m_penList->InsertItem(i - 1, ColorToString(i), NULL);
		m_penList->SetItemText(i - 1, 1, ColorToString(i));

		m_penList->SetActualWidth(i - 1, width);
		m_penList->SetItemText(i - 1, 2, widthStr);
	}

	// Set controls at right to disabled defaults
	m_firstMapSelect = TRUE;
	m_lineWidth = _T("");
	m_drawingColor = _T("");
	m_outputColor = _T("");

	CWnd *item = GetDlgItem(PRN_OUTPUT_COLOR);
	item->EnableWindow(FALSE);

	item = GetDlgItem(PRN_LINE_WIDTH);
	item->EnableWindow(FALSE);

	item = GetDlgItem(PRN_COLOR_BUTTON);
	item->EnableWindow(FALSE);

	UpdateData(FALSE);
}

BOOL PrintPenMapTab::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	LV_DISPINFO *dispinfo = (LV_DISPINFO*) lParam;

	if (dispinfo->hdr.code == LVN_ENDLABELEDIT)
	{		
		CPropertyPage::OnNotify(wParam, lParam, pResult);

		if (!m_penList->ValidateData())
			m_penList->EditInvalidSubLabel();

		else
		{
			m_penList->SetEditPrecision(4);
			LineWidthEdited();
		}	
		return TRUE;
	}

	else
		return CPropertyPage::OnNotify(wParam, lParam, pResult);
}

void PrintPenMapTab::OnSetfocusOutputColor() 
{
	//BugZilla No. 78341; 21-10-2002 
	m_bDataUpdated = FALSE;
	m_editBoxSelected = OUTPUT_COLOR;	
}

void PrintPenMapTab::OnSetfocusLineWidth() 
{
	//BugZilla No. 78341; 21-10-2002 
	m_bDataUpdated = FALSE;
	m_editBoxSelected = LINE_WIDTH;
}

void PrintPenMapTab::OnItemchangedPenmap(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	SetPropertiesInfo();
	//BugZilla No. 78341; 21-10-2002 
	m_bDataUpdated = TRUE;
		
	*pResult = 0;
}

//BugZilla No. 78341; 21-10-2002 [
void PrintPenMapTab::OnItemchangingPenmap(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if ( m_bDataUpdated == TRUE )
	{
		*pResult = 0;
		return;
	}

	int itemNum = -1;
	int row;

	if ( m_editBoxSelected == OUTPUT_COLOR )
	{	
		
		CString outputColor;
		CWnd *item = GetDlgItem(PRN_OUTPUT_COLOR);
		item->GetWindowText(outputColor);

		if (outputColor.IsEmpty() || outputColor == "varies")
		{
			*pResult = 0;
			return;
		}
		
		int outputColorNum = SetColorFromString(outputColor);
		
		if (outputColorNum < 1)
		{
			//UnSelecting ListCtrl items
			while ((row = m_penList->GetNextItem(itemNum, LVIS_SELECTED)) >= 0)
			{
				m_penList->SetItemState(row, 0, LVIS_SELECTED);
				itemNum = row;
			}
			
			*pResult = -1;						
			return;
		}

		outputColor = ColorToString(outputColorNum);
		
		while ((row = m_penList->GetNextItem(itemNum, LVIS_SELECTED)) >= 0)
		{
			m_penList->SetItemText(row, 1, outputColor);
			itemNum = row;
		}
		
		PaintItem(PRN_COLOR_BUTTON, outputColorNum, 2);
	}
	
	if ( m_editBoxSelected == LINE_WIDTH )
	{
		
		CString lineWidth;
		CWnd *item = GetDlgItem(PRN_LINE_WIDTH);
		item->GetWindowText(lineWidth);

		if (lineWidth.IsEmpty() || lineWidth == "varies")
		{
	*pResult = 0;
			return;
}

		double lineWidthNum = atof(lineWidth);
		
		if ((!lineWidthNum) && (lineWidth[0] != '0'))
		{
			//UnSelecting ListCtrl items			

			while ((row = m_penList->GetNextItem(itemNum, LVIS_SELECTED)) >= 0)
			{
				m_penList->SetItemState(row, 0, LVIS_SELECTED);
				itemNum = row;
			}

			*pResult = -1;			
			return;
		}
				
		// set precision for m_lineWidth
		TCHAR widthStr[20];
		sprintf(widthStr,"%.4f", lineWidthNum);
		lineWidth = widthStr;
		
		while ((row = m_penList->GetNextItem(itemNum, LVIS_SELECTED)) >= 0)
		{
			m_penList->SetActualWidth(row, lineWidthNum);
			m_penList->SetItemText(row, 2, lineWidth);
			itemNum = row;
		}
	}
	
	*pResult = 0;
}
//BugZilla No. 78341; 21-10-2002 ]
