/* File  : <DevDir>\source\prj\icad\ColorComboBox.cpp
 *
 * Abstract
 * implementation file
 */

#include "Icad.h"/*DNT*/
#include "IcadApi.h"/*DNT*/
#include "ColorComboBox.h"/*DNT*/
#include "cmds.h"/*DNT*/
#include "icaddoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

 
/////////////////////////////////////////////////////////////////////////////
// CColorComboBox

CColorComboBox::CColorComboBox()
{
  m_bInitialized = false;
}

CColorComboBox::~CColorComboBox()
{
}


BEGIN_MESSAGE_MAP(CColorComboBox, CComboBox)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)
	ON_CONTROL_REFLECT(CBN_CLOSEUP, OnCloseUp)
	ON_WM_CREATE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorComboBox message handlers
void CColorComboBox::OnCloseUp()
{
	SDS_CMainWindow->SetFocus();
}
BOOL CColorComboBox::Create(const RECT& rect, CWnd* pParentWnd)
{
  CRect ctrl_rect = rect;
  ctrl_rect.bottom += 200;

  m_ToolTipRect.top = 0;
  m_ToolTipRect.left = 0;
  m_ToolTipRect.right = 200;
  m_ToolTipRect.bottom = 25;

  return CComboBox::Create(WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_HASSTRINGS | WS_VSCROLL , ctrl_rect, pParentWnd, IDC_COLOR_COMBOBOX);
}

int CColorComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;
		
	Initialize(); // Initialize Contents
	SetCurSel( 0 ); // Select First Item By Default

  return 0;
}
short ddcolor(int &nCol) 
{
	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	struct resbuf rb;
	RT	ret;
	int	col;

	SDS_getvar(NULL,DB_QCECOLOR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	col=ic_colornum(rb.resval.rstring);
	free(rb.resval.rstring);
    ret=SDS_GetColorDialog(col,&nCol,0);
	return ret;
}

void CColorComboBox::OnSelchange() 
{
  // Set current layer
	char curlay[IC_ACADNMLEN];
	int nSel=GetCurSel(),nCol=-1,iAddedItem;
	if(nSel==GetCount()-1)
	{
		CString str;
		if(ddcolor(nCol)!=RTNORM)
		{
			resbuf rb;
			sds_getvar("CECOLOR"/*DNT*/,&rb);
			nSel=FindString(0,rb.resval.rstring);
			if(nSel!=-1)
				SetCurSel(nSel);
			else
			{
				str.Format("Color %s",rb.resval.rstring);
				nSel=FindString(0,str);
				if(nSel!=-1)
					SetCurSel(nSel);
			}
			return;
		}
		str.Format("Color %d",nCol);
		int nCheck = FindString(0,str);
		if(nCheck == -1)
		{
			if((iAddedItem = InsertString(nSel,str)) == CB_ERRSPACE) 
				return;
			SetItemData(iAddedItem, SDS_BrushColorFromACADColor(nCol));
			SetCurSel(iAddedItem);
		}
		else
			SetCurSel(nCheck);
		nSel=iAddedItem;
	}
	GetLBText(nSel, curlay);
	resbuf rb;
	// Modified By Mohan 
	CString szColorString(curlay);
	if(szColorString.Find("Color ")!= -1) // To Find string "Color "
		szColorString.Replace("Color ", "");
	//End Modifications
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
				ic_assoc(prb,62);
				if(nCol!=-1)
					ic_rbassoc->resval.rint=nCol;
				else
					ic_rbassoc->resval.rint=ic_colornum(szColorString);
				sds_entmod(prb);
				sds_redraw(ename,IC_REDRAW_DRAW);
				IC_RELRB(prb);
				fl1--;
			}
		}
	}
	sds_sslength(SDS_CURDOC->m_pGripSelection,&fl1);
	if(fl1==0)
	{
		rb.restype=SDS_RTSTR;
		if(nCol!=-1)
		{
			char resstr[20];
			sprintf(resstr,"%d"/*DNT*/,nCol);
			rb.resval.rstring=resstr;
			sds_setvar("CECOLOR"/*DNT*/,&rb);	
		}
		else
		{
			// Added the fixes of Kartik *****************[
			// Bug:		If you select a custom color in the Color Combo box it
			//			does not actually select the color in the dropdown but
			//			defaults to byblock instead. 
			CString colorval;
			colorval=curlay;
			if(colorval.Find("Color ")!= -1) // Did we find string "Color "?
			{
				colorval.Replace("Color ", "");
				nCol=atoi(colorval);
			}
			else // Nope, it was kosher so call ic_colornum
			{
				nCol=ic_colornum(curlay); // This is where it oughtta be
			}
			// Kartik Fixes end here ]*****************

			char resstr[20];
			sprintf(resstr,"%d"/*DNT*/,nCol);
			rb.resval.rstring=resstr;
			sds_setvar("CECOLOR"/*DNT*/,&rb);	
		}
	}
//	sds_setvar("QCECOLOR"/*DNT*/,&rb);	
	SDS_CMainWindow->SetFocus();
}

/////////////////////////////////////////////////////////////////////////////
// CColorComboBox implementation


void CColorComboBox::Initialize(void)
{
	int iAddedItem = -1;

	if(m_bInitialized)
		return;


 	if((iAddedItem = AddString(ResourceString(IDC_TBMNU_513, "BYLAYER"))) == CB_ERRSPACE) return;
	if((iAddedItem = AddString(ResourceString(IDC_TBMNU_515, "BYBLOCK"))) == CB_ERRSPACE) return;
  SetItemData(iAddedItem, SDS_BrushColorFromACADColor(7));
	if((iAddedItem = AddString(ResourceString(IDC_TBMNU_509, "Red"))) == CB_ERRSPACE) return;
  SetItemData(iAddedItem, (COLORREF)SDS_BrushColorFromACADColor(1) );
	if((iAddedItem = AddString(ResourceString(IDC_TBMNU_507, "Yellow"))) == CB_ERRSPACE) return;
  SetItemData(iAddedItem, SDS_BrushColorFromACADColor(2));
	if((iAddedItem = AddString(ResourceString(IDC_TBMNU_505, "Green"))) == CB_ERRSPACE) return;
  SetItemData(iAddedItem, SDS_BrushColorFromACADColor(3));
	if((iAddedItem = AddString(ResourceString(IDC_TBMNU_503, "Cyan"))) == CB_ERRSPACE) return;
  SetItemData(iAddedItem, SDS_BrushColorFromACADColor(4));
	if((iAddedItem = AddString(ResourceString(IDC_TBMNU_501, "Blue"))) == CB_ERRSPACE) return;
  SetItemData(iAddedItem, SDS_BrushColorFromACADColor(5));
	if((iAddedItem = AddString(ResourceString(IDC_TBMNU_511, "Magenta"))) == CB_ERRSPACE) return;
  SetItemData(iAddedItem, SDS_BrushColorFromACADColor(6));
	if((iAddedItem = AddString(ResourceString(IDC_TBMNU_499, "White"))) == CB_ERRSPACE) return;
  SetItemData(iAddedItem, SDS_BrushColorFromACADColor(7));
	if((iAddedItem = AddString(ResourceString(IDC_TBMNU_497, "Select Color..."))) == CB_ERRSPACE) return;
  SetItemData(iAddedItem, RGB( 0x00, 0x80, 0x80));

	m_bInitialized = true; // Set Initialized Flag
}


void CColorComboBox::DrawItem(LPDRAWITEMSTRUCT pDIStruct)
{
	static CString	sColor;

	CDC	dc;
	CRect	rItemRect( pDIStruct -> rcItem );
	CRect	rBlockRect( rItemRect );
	CRect	rTextRect( rBlockRect );
	CBrush brFrameBrush;
	int	nIconWidth = 0;
	int	nItem = pDIStruct -> itemID;
	//int	nAction = pDIStruct -> itemAction;
	int	nState = pDIStruct -> itemState;
	COLORREF crColor = 0;
	COLORREF crNormal = GetSysColor( COLOR_WINDOW );
	COLORREF crSelected = GetSysColor( COLOR_HIGHLIGHT );
	COLORREF crText = GetSysColor( COLOR_WINDOWTEXT );

	if(!dc.Attach(pDIStruct -> hDC ))	// Attach CDC Object
		return;

	nIconWidth = (rBlockRect.Width() / 4);     // Get 1/4 Of Item Area
	brFrameBrush.CreateStockObject(BLACK_BRUSH); // Create Black Brush

	if(nState & ODS_SELECTED && !(nState & ODS_COMBOBOXEDIT))	// Selected Attributes
	{
		dc.SetTextColor((0x00FFFFFF & ~(crText))); // Inverts Text Color
		dc.SetBkColor(crSelected);
		dc.FillSolidRect(&rBlockRect, crSelected);
	}
	else // Standard Attributes
	{
		dc.SetTextColor(crText);
		dc.SetBkColor(crNormal);
		dc.FillSolidRect(&rBlockRect, crNormal);
	}

	if(nState & ODS_FOCUS && !(nState & ODS_COMBOBOXEDIT)) // Item has the focus
		dc.DrawFocusRect(&rItemRect);

	//	Calculate Text Area
	rTextRect.left += (nIconWidth + 2);
	rTextRect.top += 2;

	//	Calculate Color Block Area
	rBlockRect.DeflateRect(CSize(2, 2));
	rBlockRect.right = nIconWidth;

	//	Draw Color Text And Block
	if(nItem != -1)
	{
		GetLBText(nItem, sColor);	// Get selected color text
		if(nState & ODS_DISABLED)	// If Disabled
		{
			crColor = GetSysColor(COLOR_INACTIVECAPTIONTEXT);
			dc.SetTextColor(crColor);
		}
		else
		{
			if(sColor == ResourceString(IDC_TBMNU_513, "BYLAYER")) // Retrieve current layer color
			{
				struct resbuf rb;
		    
				CString strLayname;
				int nSel = -1;
				if (SDS_CMainWindow->m_wndLayers.GetSafeHwnd())
					nSel = SDS_CMainWindow->m_wndLayers.GetCurSel();

				if(nSel>=0)
					SDS_CMainWindow->m_wndLayers.GetLBText(nSel,strLayname);
				else
				{
					SDS_getvar(NULL, DB_QCLAYER, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
					strLayname=(rb.resval.rstring);
				}
				resbuf	*pRrb = sds_tblsearch("LAYER", strLayname, 0), *pCurRb = pRrb;
				while(pCurRb && pCurRb->restype != 62)
				pCurRb = pCurRb->rbnext;
				if(pCurRb)
				crColor = (COLORREF)SDS_BrushColorFromACADColor(pCurRb->resval.rint);
				else
				crColor = (COLORREF)SDS_BrushColorFromACADColor(7);
				sds_relrb(pRrb);
			}
			else
				crColor = GetItemData(nItem);	// get color value
		}

		CGdiObject* pCurObj = dc.SelectStockObject(DEFAULT_GUI_FONT);

		dc.SetBkMode(TRANSPARENT);
		dc.TextOut(rTextRect.left, rTextRect.top, sColor);
		dc.FillSolidRect(&rBlockRect, crColor);
		dc.FrameRect(&rBlockRect, &brFrameBrush);
		dc.SelectObject(pCurObj);
	}
	dc.Detach();
}

void CColorComboBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
}

/********************************************************************************
 * Author:	Sachin Dange.
 *																				*
 * Purpose:	
			1. To update the "Color Control" so as to work in sync with status 
			   bar "Color Control".
			2. To update the list after the entity selection is over.

 *																				*
 * Returns:	void																*
 ********************************************************************************/

void CColorComboBox::UpdateColorList()
{
	struct resbuf rb;
	int iAddedItem;
	SDS_getvar(NULL,DB_QCECOLOR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	CString strTmp(rb.resval.rstring);
	int nCurCol;
	
	if(strTmp.CompareNoCase("BYBLOCK"/*DNT*/)==0)
	{
		nCurCol = 1;
		SetCurSel(nCurCol);
	}
	else
	{
		if (strTmp.CompareNoCase("BYLAYER"/*DNT*/)==0)
		{
			//nCurCol = 256;
			nCurCol = 0;
			SetCurSel(nCurCol);
		}
		else
		{
			nCurCol=ic_colornum(strTmp.GetBuffer(0));
			CString str=ic_colorstr(nCurCol,NULL);
			int nSel = FindString(-1,str);
			if(nSel!=-1)
				SetCurSel(nSel);
			else
			{
				str.Format("Color %s",rb.resval.rstring);
				if((nSel = FindString(-1,"Select Color...")) != CB_ERR)
				{	
					int nCheck = FindString(0,str);
					if(nCheck == -1)
					{
						if((iAddedItem = InsertString(nSel,str)) == CB_ERRSPACE) 
							return;
						SetItemData(iAddedItem, SDS_BrushColorFromACADColor(nCurCol));
						SetCurSel(iAddedItem);
					}
					else
						SetCurSel(nCheck);
				}
			}
		}
	}

	IC_FREE(rb.resval.rstring);

}

/********************************************************************************
 * Author:	Mohan Nayak.														*
 *																				*
 * Purpose:	Add ToolTip.														*
 *																				*
 * Returns:	void																*
 ********************************************************************************/
void CColorComboBox::AddToolTip()
{
	m_ColorToolTip.Create(this);
	m_ColorToolTip.AddTool(this,ResourceString(IDC_TBMNU_1473,"Color control"/*DNT*/),&m_ToolTipRect,IDB_ICAD_COLOR_CTRL);
}

BOOL CColorComboBox::PreTranslateMessage(MSG* pMsg) 
{
	m_ColorToolTip.Activate(TRUE);
	m_ColorToolTip.RelayEvent(pMsg);
	return CComboBox::PreTranslateMessage(pMsg);
}
