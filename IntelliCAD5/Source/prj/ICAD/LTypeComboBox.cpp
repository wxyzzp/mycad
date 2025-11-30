/* File  : <DevDir>\source\prj\icad\LTypeComboBox.cpp
 *
 * Abstract
 * implementation file
 */

#include "Icad.h"/*DNT*/
#include "IcadApi.h"/*DNT*/
#include "LTypeComboBox.h"/*DNT*/
#include "cmds.h"/*DNT*/
#include "icaddoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLTypeComboBox

CLTypeComboBox::CLTypeComboBox()
{
}

CLTypeComboBox::~CLTypeComboBox()
{
}


BEGIN_MESSAGE_MAP(CLTypeComboBox, CComboBox)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)
	ON_CONTROL_REFLECT(CBN_DROPDOWN, OnDropDown)
	ON_CONTROL_REFLECT(CBN_CLOSEUP, OnCloseUp)
	ON_WM_CREATE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLTypeComboBox message handlers

BOOL CLTypeComboBox::Create(const RECT& rect, CWnd* pParentWnd)
{
  CRect ctrl_rect = rect;
  ctrl_rect.bottom += 200;

  m_ToolTipRect.top = 0;
  m_ToolTipRect.left = 0;
  m_ToolTipRect.right = 200;
  m_ToolTipRect.bottom = 25;

  return CComboBox::Create(WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_HASSTRINGS | WS_VSCROLL, ctrl_rect, pParentWnd, IDC_LTYPE_COMBOBOX);
}

int CLTypeComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;
		
	return 0;
}


int NumOfLineType()
{
	resbuf *rb=sds_tblnext("LTYPE",1);
	int i=0;
	for(resbuf *rbt=rb;rbt;rbt=rbt->rbnext) 
		i++;
	return i;
}
#include "icadexplorer.h"
int LType(char *sType)
{
	IEXP_LTDlg	   ltdlg;
	IEXP_LTList    ltlst;
	resbuf rb;
	short offindex,retvalue;
	sds_real	   len;
	char *ptr1=NULL,*ptr2=NULL;
	char IEXP_szText[200];
	BOOL NameExist;

	int IEXP_LinetCnt=NumOfLineType();

	int	measurement;
	SDS_getvar(NULL,DB_QMEASUREMENT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	measurement = rb.resval.rint;
	// Display new linetype dialog for description, and definition.
	int rindex=0, index=0;
	do {
		retvalue=ltdlg.DoModal();
		if (retvalue==EXP_BROWSE)
		{
			char fullFileName[IC_ACADBUF];
			fullFileName[0]=0;
			//EBATECH(CNBR)-[ 2002/9/20 Default linetype file is dependent on $MEASUREMENT.
			if ((sds_findfile( measurement == DB_ENGLISH_MEASUREINIT ?
				SDS_LTYPEFILE : SDS_ISOLTYPEFILE, &fullFileName[0]))!=RTNORM)
			{
				strcpy(&fullFileName[0], measurement == DB_ENGLISH_MEASUREINIT ?
					SDS_LTYPEFILE : SDS_ISOLTYPEFILE);
				rindex = 1;	//set this so that if neighter icad.lin nor icadiso.lin was not found, user can select linetype file
			}
			//EBATECH(CNBR)]-
			do
			{
				if (rindex)
				{
					// EBATECH(CNBR) 2002/4/30 Disable TYPE button option+2
					if ((sds_getfiled(ResourceString(IDC_ICADEXPLORER_SELECT_LIN_73, "Select Linetype File" ),&fullFileName[0],ResourceString(IDC_ICADEXPLORER_LINETYPE_F_74, "Linetype File|lin" ),2,&rb))!=RTNORM)
					{
						rb.resval.rstring=NULL;
						return FALSE;
					}
					else
					{
						ltlst.m_Filename=rb.resval.rstring;
						IC_FREE(rb.resval.rstring);
					}
				}
				else
				{
					ltlst.m_Filename=&fullFileName[0];
				}
				if ((index=ltlst.DoModal())==IDCANCEL)
				{
					ltlst.m_ret=0;
					break ;
				}
				if (index==EXP_BROWSE2)
				{
					rindex=1;
					continue;
				}
				offindex=IEXP_LinetCnt;
				rindex=0;
				break;
			} while(TRUE);

			if (ltlst.m_ret==0)
				continue;

		} else break;
	} while(!ltlst.m_ret);

	if (retvalue==IDOK && ltdlg.m_Desc && ltdlg.m_Defstr) 
	{
		// parse the definition string.
		for(len=0,index=1,ptr1=(char *)(LPCTSTR)ltdlg.m_Defstr;ptr2=strchr(ptr1,IEXP_COMMA);ptr1=ptr2+1,++index) {
			*ptr2=0; len+=fabs(atof(ptr1)); *ptr2=IEXP_COMMA;
		}
		if(*ptr1)	//D.Gavrilov. If the last symbol in ltdlg.m_Defstr is a ',' then we must decrement index.
			len+=fabs(atof(ptr1));
		else
			index--;
		strcpy(IEXP_szText,ResourceString(IDC_IEXP_NEW_LINETYPE, "NewLinetype")); rindex=0;
		do {
			NameExist=FALSE;
			resbuf *rbb,*rbt;
			sprintf(IEXP_szText,"%s%d"/*DNT*/,ResourceString(IDC_IEXP_NEW_LINETYPE, "NewLinetype"),(++rindex));
			if ((rbb=sds_buildlist(RTDXF0,IEXP_LTYPE,2,IEXP_szText,70,0,3,ltdlg.m_Desc,73,index,40,len,0))==NULL) 
				return FALSE;
			for(rbt=rbb;rbt->rbnext;rbt=rbt->rbnext);
			for(index=1,ptr1=(char *)(LPCTSTR)ltdlg.m_Defstr;ptr2=strchr(ptr1,IEXP_COMMA);ptr1=ptr2+1,++index,rbt=rbt->rbnext) {
				*ptr2=0; rbt->rbnext=sds_newrb(49); rbt->rbnext->resval.rreal=atof(ptr1);
				*ptr2 = IEXP_COMMA;	//D.Gavrilov. We must do it to save a string as it was (for next do-loops)
			}
			if(*ptr1)	//D.Gavrilov. We should add the last 49 ONLY in this case.
			{
				rbt->rbnext = sds_newrb(49);
				rbt->rbnext->resval.rreal = atof(ptr1);
			}
			if (SDS_tblmesser(rbb,IC_TBLMESSER_MAKE,SDS_CURDWG)!=RTNORM) NameExist=TRUE;
			sds_relrb(rbb);
		} while(NameExist);
		// Set linetype names in the (m_linetypes) variable.
		ltlst.m_linetypes.Empty();
		ltlst.m_linetypes=IEXP_szText;
		strcpy(sType,IEXP_szText);
	} 
	else if (retvalue==IDCANCEL) 
		return FALSE;
	else if(retvalue==EXP_BROWSE)
		strcpy(sType,ltlst.m_linetypes);
	return TRUE;
}
void CLTypeComboBox::OnSelchange() 
{
	// Set current layer
	char curlay[IC_ACADNMLEN]="";
	struct resbuf rb;
	int nSel=GetCurSel();
	if(nSel==GetCount()-1)
	{
		if(!LType(curlay))
		{
			sds_getvar("CELTYPE"/*DNT*/,&rb);	
			nSel=FindString(0,rb.resval.rstring);
			if(nSel!=-1)
				SetCurSel(nSel);
			return;
		}
	}
	else
		GetLBText(nSel, curlay);
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
				ic_assoc(prb,6);
				if((ic_rbassoc->resval.rstring=(char *)realloc(ic_rbassoc->resval.rstring,strlen(curlay)+1)))
				{
					strcpy(ic_rbassoc->resval.rstring,curlay);
					sds_entmod(prb);
					ads_redraw(ename,3);
				}
				sds_entmod(prb);
				IC_RELRB(prb);
				sds_redraw(ename,IC_REDRAW_DRAW);
				fl1--;
			}
		}
	}
	sds_sslength(SDS_CURDOC->m_pGripSelection,&fl1);
	if(fl1==0)
	{
		rb.restype=SDS_RTSTR;
		rb.resval.rstring = curlay;
		sds_setvar("CELTYPE"/*DNT*/,&rb);	
	}
	SDS_CMainWindow->SetFocus();
}

void CLTypeComboBox::OnDropDown()
{
	UpdateLTypeList();
}
void CLTypeComboBox::OnCloseUp()
{
	SDS_CMainWindow->SetFocus();
}

/////////////////////////////////////////////////////////////////////////////
// CLTypeComboBox implementation

void CLTypeComboBox::UpdateLTypeList()
{
  struct resbuf *prbLT;
  
  if((prbLT = sds_tblnext("LTYPE"/*DNT*/,1)) == NULL)
  {
    return;
  }

  ResetContent();

  if(AddString(ResourceString(IDC_TBMNU_513, "BYLAYER")) == CB_ERRSPACE) return;
  if(AddString(ResourceString(IDC_TBMNU_515, "BYBLOCK")) == CB_ERRSPACE) return;
  do
  {
    ic_assoc(prbLT, 2);    
    if(AddString(ic_rbassoc->resval.rstring) == CB_ERRSPACE) return;
    IC_RELRB(prbLT);
  } while((prbLT = sds_tblnext("LTYPE"/*DNT*/,0)) != NULL);

  if(AddString(ResourceString(IDC_TBMNU_482,"Explore Linetypes...")) == CB_ERRSPACE) return; // GB:"其它线型..."
  // Get the current linetype
  struct resbuf rb;
  SDS_getvar(NULL, DB_QCELTYPE, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
  CString strCLType(rb.resval.rstring);
  IC_FREE(rb.resval.rstring);
  // Select active linetype
  SetCurSel(FindString(-1, strCLType));
}

void CLTypeComboBox::DrawItem(LPDRAWITEMSTRUCT pDIStruct)
{
	CDC	dc;
	CRect	rItemRect(pDIStruct->rcItem);
	CRect	rBlockRect(pDIStruct->rcItem);
	CRect	rTextRect(pDIStruct->rcItem);
  CRect rSelRect(pDIStruct->rcItem);
	COLORREF crNormal = GetSysColor(COLOR_WINDOW);
	COLORREF crSelected = GetSysColor(COLOR_HIGHLIGHT);
	COLORREF crText = GetSysColor(COLOR_WINDOWTEXT);
	int	nState = pDIStruct->itemState;
	int	nItem = pDIStruct->itemID;

	if(!dc.Attach(pDIStruct -> hDC))	// Attach CDC Object
		return;

	int nSepWidth = (rItemRect.Width() / 2);

  //	Calculate Selection Area
  rSelRect.left += nSepWidth;

  //	Calculate Text Area
	rTextRect.left += rSelRect.left + 2;
	rTextRect.top += 2;

	//	Calculate Color Block Area
	rBlockRect.DeflateRect(CSize(2, 2));
	rBlockRect.right = nSepWidth;

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
		dc.DrawFocusRect(&rItemRect);


	//	Draw Color Text And Block
	if(nItem != -1)
	{
    CString sLType;
		GetLBText(nItem, sLType);	// Get selected text
    
    CGdiObject* pCurObj = dc.SelectStockObject(DEFAULT_GUI_FONT);

    dc.SetBkMode(TRANSPARENT);
		dc.TextOut(rTextRect.left, rTextRect.top, sLType);

		// Calculate the length of the line to display.  To do this, we find the pattern length and muliply
		// it by the linetype scale.  Then multiply by 10, so we can show 10 reps of this pattern.
    char szLT[IC_ACADBUF];
    struct resbuf *rb,*rbt,rbb;
		double size = 0.0;
		sds_point p1,p2;
		p1[0] = p1[1] = p1[2] = 0.0;
		p2[0] = p2[1] = p2[2] = 0.0;

 		strncpy (szLT, sLType, IC_ACADBUF);
		rb=SDS_tblgetter("LTYPE", szLT, TRUE, SDS_CURDWG);
		if(rb)
		{
			for(rbt = rb; rbt; rbt = rbt->rbnext)
			{
				if(rbt->restype == 49)
					size += fabs(rbt->resval.rreal);
			}
			sds_relrb(rb);
		}
		if (icadRealEqual(size,0.0)) size = 1.0;
		SDS_getvar(NULL, DB_QLTSCALE, &rbb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
		if(icadRealEqual(rbb.resval.rreal, 0.0))
			rbb.resval.rreal = 1.0;
		p2[0] = size * 10.0 * rbb.resval.rreal;

    //Create a line with the given linestyle
		db_handitem *hip = new db_line(SDS_CURDWG);
		hip->set_10(p1);
		hip->set_11(p2);
		hip->set_6(szLT, SDS_CURDWG);	//set linetype
		hip->set_62(248);							//set color to black

		// Draw the Filled rectangle
		CBitmap *Bitmap = SDS_DrawBitmap(SDS_CURDWG, NULL, NULL, hip, rBlockRect.Width(), rBlockRect.Height(), FALSE);
		delete hip;

		CDC CmpDC;
		if(CmpDC.CreateCompatibleDC(&dc))
		{
			CBitmap *OldBitmap = CmpDC.SelectObject(Bitmap);
			dc.BitBlt(rBlockRect.left, rBlockRect.top, rBlockRect.Width(), rBlockRect.Height(), &CmpDC, 0, 0, SRCCOPY);
			delete CmpDC.SelectObject(OldBitmap);
		}

    dc.SelectObject(pCurObj);
	}
	dc.Detach();
}

void CLTypeComboBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
}

/********************************************************************************
 * Author:	Mohan Nayak.														*
 *																				*
 * Purpose:	Add ToolTip.														*
 *																				*
 * Returns:	void																*
 ********************************************************************************/
void CLTypeComboBox::AddToolTip()
{
	m_LTypeToolTip.Create(this);
	m_LTypeToolTip.AddTool(this,ResourceString(IDC_TBMNU_1475,"LineType control"/*DNT*/),&m_ToolTipRect,IDB_ICAD_LTYPE_CTRL);
}


BOOL CLTypeComboBox::PreTranslateMessage(MSG* pMsg) 
{
	m_LTypeToolTip.Activate(TRUE);
	m_LTypeToolTip.RelayEvent(pMsg);
	return CComboBox::PreTranslateMessage(pMsg);
}


