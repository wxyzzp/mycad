/* File  : <DevDir>\source\prj\icad\LayerComboBox.cpp
 *
 * Abstract
 * implementation file
 */

#include "Icad.h"/*DNT*/
#include "IcadApi.h"/*DNT*/
#include "LayerComboBox.h"/*DNT*/
#include "cmds.h"/*DNT*/
#include "Resbuf.h"				/*DNT*/
#include "icaddoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CLayerComboBox

CLayerComboBox::CLayerComboBox()
{
  m_bSubclassedListBox = false;
  m_nCurSelItem = -1;
}

CLayerComboBox::~CLayerComboBox()
{
}


BEGIN_MESSAGE_MAP(CLayerComboBox, CComboBox)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)
	ON_CONTROL_REFLECT(CBN_DROPDOWN, OnDropDown)
	ON_CONTROL_REFLECT(CBN_CLOSEUP, OnCloseUp)
	ON_WM_CREATE()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLayerComboBox message handlers

BOOL CLayerComboBox::Create(const RECT& rect, CWnd* pParentWnd)
{
  CRect ctrl_rect = rect;
  ctrl_rect.bottom += 200;

  m_ToolTipRect.top = 0;
  m_ToolTipRect.left = 0;
  m_ToolTipRect.right = 200;
  m_ToolTipRect.bottom = 25;

  return CComboBox::Create(WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_HASSTRINGS | WS_VSCROLL, ctrl_rect, pParentWnd, IDC_LAYER_COMBOBOX);
}

int CLayerComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;

  return 0;
}

void CLayerComboBox::OnSelchange() 
{
  // Set current layer
	char curlay[IC_ACADNMLEN];
	GetLBText(GetCurSel(), curlay);
	resbuf rb;
	long ret,fl1;
	ads_name ename;
	if((ret=SDS_getvar(NULL,DB_QPICKFIRST,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))==RTNORM)
	{
		if (rb.resval.rint && SDS_CURDOC && RTNORM==sds_sslength(SDS_CURDOC->m_pGripSelection,&fl1) && fl1>0L)
		{
			fl1--;
			while (RTNORM==sds_ssname(SDS_CURDOC->m_pGripSelection,fl1,ename)) 
			{
				resbuf *prb=ads_entget(ename);
				ic_assoc(prb,8);
				if((ic_rbassoc->resval.rstring=(char *)realloc(ic_rbassoc->resval.rstring,strlen(curlay)+1)))
				{
					strcpy(ic_rbassoc->resval.rstring,curlay);
					sds_entmod(prb);
					sds_redraw(ename,IC_REDRAW_DRAW);
				}
				IC_RELRB(prb);
				fl1--;
			}
		}
	}
	struct resbuf *rbb = SDS_tblgetter("LAYER", curlay, TRUE, SDS_CURDWG);
	if(ic_assoc(rbb, 70))
	{
	    SDS_CMainWindow->SetFocus();
		return;
	}
	if((ic_rbassoc->resval.rint & IC_LAYER_FROZEN))
	{
		sds_relrb(rbb);
		//AfxMessageBox(ResourceString(IDC_ICADEXPLORER_CANNOT_SET_19, "Cannot set a frozen layer current." ));
		SetCurSel(m_nCurSelItem);
		SDS_CMainWindow->SetFocus();
		return;
	}

	sds_relrb(rbb);
    
	//  struct resbuf rb;
	sds_sslength(SDS_CURDOC->m_pGripSelection,&fl1);
	if(fl1==0)
	{
		rb.restype=SDS_RTSTR;
		rb.resval.rstring = curlay;
		sds_setvar("CLAYER"/*DNT*/,&rb);	
		// Added the fixes of Kartik *****************[
		if (SDS_CMainWindow->m_wndColor.GetSafeHwnd())
			SDS_CMainWindow->m_wndColor.SetCurSel(SDS_CMainWindow->m_wndColor.GetCurSel()); // Doonga : Etheract: 2004-08-10 :

		if (SDS_CMainWindow->m_wndLType.GetSafeHwnd())
			SDS_CMainWindow->m_wndLType.SetCurSel(SDS_CMainWindow->m_wndLType.GetCurSel()); // Doonga : Etheract: 2004-08-10 :
		// Kartik's Fixes end here ]*****************
	}
	SDS_CMainWindow->SetFocus();
}

void CLayerComboBox::OnDropDown()
{
	m_nCurSelItem = GetCurSel();
	UpdateLayerList();
}
void CLayerComboBox::OnCloseUp()
{
	SDS_CMainWindow->SetFocus();
}

/////////////////////////////////////////////////////////////////////////////
// CLayerComboBox implementation

void CLayerComboBox::UpdateLayerList()
{
  struct resbuf *tmprb, *rbbUnSort, *rbUnSort, *rbbSort;
  tmprb = rbbUnSort = rbUnSort = rbbSort = NULL;

  if((tmprb = sds_tblnext("LAYER"/*DNT*/,1)) == NULL)
  {
    return;
  }

  ResetContent();

  // create a rbbUnSort list of layers
  do
  {
    // Check if its XREF layer
    if(ic_assoc(tmprb, 2) == 0)
    {
      if(strchr(ic_rbassoc->resval.rstring, '|'/*DNT*/) != NULL)
      {
        IC_RELRB(tmprb);
        continue;
      }				
    }
    else
    {
      IC_RELRB(tmprb);
      return;
    }

    if(rbbUnSort == NULL)
    {
      rbbUnSort = rbUnSort = sds_newrb(RTSTR);
    }
    else
    {
      rbUnSort = rbUnSort->rbnext = sds_newrb(RTSTR);
    }
    rbUnSort->rbnext = NULL;
    // store layers
    rbUnSort->resval.rstring = new char [strlen(ic_rbassoc->resval.rstring) + 1];
    strcpy(rbUnSort->resval.rstring, ic_rbassoc->resval.rstring);
    IC_RELRB(tmprb);
  } while((tmprb = sds_tblnext("LAYER"/*DNT*/, 0)) != NULL);
  // sort layerlist
  rbbSort = cmd_listsorter(rbbUnSort);
  IC_RELRB(rbbUnSort);
  // Add the layer names to the combobox.
  CString str;
  for(tmprb = rbbSort; tmprb != NULL; tmprb = tmprb->rbnext)
  {
    // Layer names.
    str = tmprb->resval.rstring;
    AddString(str);
  }
  IC_RELRB(rbbSort);
  // Get the current layer.
  struct resbuf rb;
  SDS_getvar(NULL, DB_QCLAYER, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
  CString strCLayer(rb.resval.rstring);
  IC_FREE(rb.resval.rstring);
  // Select active layer
  SetCurSel(FindString(-1, strCLayer));
}

void CLayerComboBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
  CString sLayer;
	if(lpDrawItemStruct->itemID != -1)
    GetLBText(lpDrawItemStruct->itemID, sLayer);	// Get selected text
  m_wndListBox.DrawItem(lpDrawItemStruct->rcItem, lpDrawItemStruct->itemState, lpDrawItemStruct->itemID, lpDrawItemStruct->hDC, sLayer);
}

void CLayerComboBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
}

HBRUSH CLayerComboBox::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
  if(!m_bSubclassedListBox && nCtlColor == CTLCOLOR_LISTBOX && pWnd != NULL && m_wndListBox.GetSafeHwnd() == NULL)
  {
    m_wndListBox.SubclassWindow(pWnd->GetSafeHwnd());
    m_wndListBox.SetOwner(this);
    m_bSubclassedListBox = true;
  }
	return CComboBox::OnCtlColor(pDC, pWnd, nCtlColor);
}

/////////////////////////////////////////////////////////////////////////////
// CLayerComboListBox

CLayerComboListBox::CLayerComboListBox()
{
  m_nPanelWidth = (IMG_WIDTH + IMG_OFFSET) * 4;
}

CLayerComboListBox::~CLayerComboListBox()
{
}

BEGIN_MESSAGE_MAP(CLayerComboListBox, CListBox)
  ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLayerComboBox message handlers

void CLayerComboListBox::OnLButtonDown(UINT nFlags, CPoint point)
{
  if(point.x < m_nPanelWidth)
  {
    bool bAbort = false;
  	struct resbuf rbCurLayer;
    SDS_getvar(NULL, DB_QCLAYER, &rbCurLayer, SDS_CURDWG, NULL, NULL);

    CString sLayer;
    GetText(GetCurSel(), sLayer);
	  struct resbuf *rbb = NULL, *rbt = NULL, *rbn = NULL;
    rbn = SDS_tblgetter("LAYER", sLayer, TRUE, SDS_CURDWG);
    if(!(rbt = BuildDataList(&rbb, RTDXF0, "LAYER")) || !(BuildDataList(&rbt, 2, sLayer))) return;
    if(point.x > 0 && point.x < IMG_WIDTH) // Of/Off icon
    {
			ic_assoc(rbn, 62);
			short value=-(ic_rbassoc->resval.rint);
			if(!BuildDataList(&rbt, 62, value)) return;      
    }
    else if(point.x > IMG_WIDTH + IMG_OFFSET && point.x < (IMG_WIDTH + IMG_OFFSET) * 2) // Freeze / Thaw icon
    {
      if(!strcmp(sLayer, rbCurLayer.resval.rstring))
      {
        bAbort = true;
      }
      else
      {
			  ic_assoc(rbn, 70);
			  short value=(!(ic_rbassoc->resval.rint & 0x01) ? (ic_rbassoc->resval.rint | 0x01) : (ic_rbassoc->resval.rint ^ 0x01));
			  if(!BuildDataList(&rbt, 70, value)) return;
      }
    }
    else if(point.x > (IMG_WIDTH + IMG_OFFSET) * 2 && point.x < (IMG_WIDTH + IMG_OFFSET) * 3) // Lock / Unlock icon
    {
			ic_assoc(rbn, 70);
			short value=(!(ic_rbassoc->resval.rint & 0x04) ? (ic_rbassoc->resval.rint | 0x04) : (ic_rbassoc->resval.rint ^ 0x04));
			if(!BuildDataList(&rbt, 70, value)) return;      
    }
    // Free resbuffers
		if(rbb) 
    { 
      if(!bAbort) SDS_tblmesser(rbb, IC_TBLMESSER_MODIFY, SDS_CURDWG); 
      sds_relrb(rbb); 
      rbb = NULL; 
    }
		if(rbn) 
    { 
      sds_relrb(rbn); 
      rbn = NULL; 
    }
    // Update visualizations
    CRect rect;
    GetItemRect(GetCurSel(), &rect);
    InvalidateRect(&rect, TRUE);
    cmd_regenall();

    return;
  }

  CListBox::OnLButtonDown(nFlags, point);
}

void CLayerComboListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
  CString sLayer;
  if(lpDrawItemStruct->itemID != -1)
    GetText(lpDrawItemStruct->itemID, sLayer);
  DrawItem(lpDrawItemStruct->rcItem, lpDrawItemStruct->itemState, lpDrawItemStruct->itemID, lpDrawItemStruct->hDC, sLayer);
}

void CLayerComboListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
}

void CLayerComboListBox::DrawItem(RECT rcItem, int nState, int nItem, HDC hDC, LPCTSTR lpszText)
{
	CDC	dc;
	CRect	rItemRect(rcItem);
	CRect	rPanelRect(rcItem);
	CRect	rTextRect(rcItem);
  CRect rSelRect(rcItem);
	COLORREF crColor = 0;
	COLORREF crNormal = GetSysColor(COLOR_WINDOW);
	COLORREF crSelected = GetSysColor(COLOR_HIGHLIGHT);
	COLORREF crText = GetSysColor(COLOR_WINDOWTEXT);
  COLORREF crDisabled = GetSysColor(COLOR_GRAYTEXT);
  CBrush brFrameBrush;

	if(!dc.Attach(hDC))	// Attach CDC Object
		return;

  //	Calculate Selection Area
  rSelRect.left += m_nPanelWidth;

  //	Calculate Text Area
	rTextRect.left = rSelRect.left + 2;
	rTextRect.top += 2;

	//	Calculate Image Block Area
	rPanelRect.DeflateRect(CSize(2, 2));
	rPanelRect.right = m_nPanelWidth;

  if(nState & ODS_SELECTED && !(nState & ODS_COMBOBOXEDIT))	// Selected Attributes
	{
		dc.SetTextColor((0x00FFFFFF & ~(crText))); // Inverts Text Color
		dc.SetBkColor(crSelected);
    dc.FillSolidRect(&rSelRect, crSelected);
	}
	else // Standard Attributes
	{
	  dc.SetTextColor(crText);
	  dc.SetBkColor(crNormal);
	  dc.FillSolidRect(&rItemRect, crNormal);
  }

	if(nState & ODS_FOCUS && !(nState & ODS_COMBOBOXEDIT)) // Item has the focus
		dc.DrawFocusRect(&rSelRect);

	//	Draw Color Text And Block
	if(nItem != -1)
	{
    // Draw layer status
		resbuf *pRrb = sds_tblsearch("LAYER", lpszText, 0);

    ic_assoc(pRrb, 62);     // Retrieve layer color
    crColor = (COLORREF)SDS_BrushColorFromACADColor(abs(ic_rbassoc->resval.rint));
    DrawBitmap(&dc, ic_rbassoc->resval.rint > 0 ? IDB_LAYER_CMBOX_ON : IDB_LAYER_CMBOX_OFF, rPanelRect.left, rPanelRect.top);
   
    ic_assoc(pRrb, 70); 
    int nFlag = ic_rbassoc->resval.rint;
    DrawBitmap(&dc, nFlag & 1 ? IDB_LAYER_CMBOX_FRZ : IDB_LAYER_CMBOX_TWN, rPanelRect.left + IMG_WIDTH + IMG_OFFSET, rPanelRect.top);
    DrawBitmap(&dc, nFlag & 4 ? IDB_LAYER_CMBOX_LCK : IDB_LAYER_CMBOX_ULCK, rPanelRect.left + (IMG_WIDTH + IMG_OFFSET) * 2, rPanelRect.top);

    // Draw color rect
    CRect rColorRect(CPoint(rPanelRect.left + (IMG_WIDTH + IMG_OFFSET) * 3, rPanelRect.top + 2), CSize(10, 10));
		dc.FillSolidRect(&rColorRect, crColor);
    brFrameBrush.CreateStockObject(BLACK_BRUSH);
		dc.FrameRect(&rColorRect, &brFrameBrush);

    // Draw layer name
    CGdiObject* pCurObj = dc.SelectStockObject(DEFAULT_GUI_FONT);
    dc.SetBkMode(TRANSPARENT);
    if(nFlag & 1) dc.SetTextColor(crDisabled);
		dc.TextOut(rTextRect.left, rTextRect.top, lpszText, strlen(lpszText));
    dc.SelectObject(pCurObj);
    sds_relrb(pRrb);
	}
	dc.Detach();
}

void CLayerComboListBox::DrawBitmap(CDC *pDC, UINT nResourceID, int nLeft, int nTop)
{
  CBitmap bmp;
  if(bmp.LoadBitmap(nResourceID))
  {
    // Create an in-memory DC compatible with the display DC we're using to paint
    CDC dcMemory;
    dcMemory.CreateCompatibleDC(pDC);

    // Select the bitmap into the in-memory DC
    CBitmap* pOldBitmap = dcMemory.SelectObject(&bmp);

    // Copy the bits from the in-memory DC 
    pDC->BitBlt(nLeft, nTop, IMG_WIDTH, IMG_HEIGHT, &dcMemory, 0, 0, SRCCOPY);
    dcMemory.SelectObject(pOldBitmap);
  }
}



/********************************************************************************
 * Author:	Mohan Nayak.														*
 *																				*
 * Purpose:	Add ToolTip.														*
 *																				*
 * Returns:	void																*
 ********************************************************************************/
void CLayerComboBox::AddToolTip()
{
	m_LayerToolTip.Create(this);
	m_LayerToolTip.AddTool(this,ResourceString(IDC_TBMNU_1471,"Layer control"/*DNT*/),&m_ToolTipRect,IDB_ICAD_LAYER_CTRL);
}

BOOL CLayerComboBox::PreTranslateMessage(MSG* pMsg) 
{
	m_LayerToolTip.Activate(TRUE);
	m_LayerToolTip.RelayEvent(pMsg);
	return CComboBox::PreTranslateMessage(pMsg);
}