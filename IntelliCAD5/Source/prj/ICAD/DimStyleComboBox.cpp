#include "Icad.h"/*DNT*/
#include "IcadApi.h"/*DNT*/
#include "DimStyleComboBox.h"/*DNT*/
#include "cmds.h"/*DNT*/
#include "icaddoc.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDimstyleComboBox

CDimStyleComboBox::CDimStyleComboBox()
{
	
}

CDimStyleComboBox::~CDimStyleComboBox()
{

}

BOOL CDimStyleComboBox::PreTranslateMessage(MSG *pMsg)
{
	m_ToolTipCtrl.Activate(TRUE); 
	m_ToolTipCtrl.RelayEvent(pMsg);
	return CComboBox::PreTranslateMessage(pMsg);
}
 

BEGIN_MESSAGE_MAP(CDimStyleComboBox, CComboBox)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnDropDown)
	ON_CONTROL_REFLECT(CBN_CLOSEUP, OnCloseUp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDimStyleComboBox message handlers

BOOL CDimStyleComboBox::Create(const RECT& rect, CWnd* pParentWnd)
{
	CRect ctrl_rect = rect;
	ctrl_rect.bottom += 200;
	m_rect.left = 0;
	m_rect.top = 0;
	m_rect.right = ctrl_rect.Width(); 
	m_rect.bottom = 122;
	return CComboBox::Create(WS_CHILD|WS_VISIBLE|WS_VSCROLL|CBS_HASSTRINGS|CBS_DROPDOWNLIST, ctrl_rect, pParentWnd, IDC_DIMSTYLE_COMBOBOX);
}


void CDimStyleComboBox::OnSelchange ()
{
	CString csTmp;
	GetLBText(GetCurSel(),csTmp);
	char *eName = NULL;

	// New Added Stuff
	long ret,fl1;
	ads_name ename;
	resbuf rb;
	if((ret=SDS_getvar(NULL,DB_QPICKFIRST,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))==RTNORM)
	{
		if (rb.resval.rint && SDS_CURDOC && RTNORM==sds_sslength(SDS_CURDOC->m_pGripSelection,&fl1) && fl1>0L)
		{
			fl1--;
			while (RTNORM==sds_ssname(SDS_CURDOC->m_pGripSelection,fl1,ename)) 
			{
				resbuf *prb=ads_entget(ename);
				ic_assoc(prb,0);
				eName=strdup(ic_rbassoc->resval.rstring);
				if(strisame(eName,"DIMENSION"/*DNT*/) || strisame(eName,"LEADER"/*DNT*/))
				{
					ic_assoc(prb,3);
					if((ic_rbassoc->resval.rstring=(char *)realloc(ic_rbassoc->resval.rstring,strlen(csTmp)+1)))
					{
						strcpy(ic_rbassoc->resval.rstring,csTmp);
						sds_entmod(prb);
						ads_redraw(ename,3);
					}
					sds_entmod(prb);
					IC_RELRB(prb);
					sds_redraw(ename,IC_REDRAW_DRAW);
					fl1--;
				}
				else
				{
					SetCurSel(-1);
					fl1--;

				}
			}

		}

	}


	// End here
	free(eName);
	sds_sslength(SDS_CURDOC->m_pGripSelection,&fl1);
	if(fl1==0)
	{
    
		rb.restype = RTSTR;
		rb.resval.rstring = new char [csTmp.GetLength() + 1];
		strcpy(rb.resval.rstring, (LPCTSTR)csTmp);
		sds_setvar("DIMSTYLE"/*DNT*/, &rb);
		IC_FREE(rb.resval.rstring);	
		struct sds_resbuf *pDimStyle = sds_tblsearch("DIMSTYLE"/*DNT*/,csTmp,0);
		cmd_setActiveDimStyle (pDimStyle);
		IC_RELRB(pDimStyle);
	}


	SDS_CMainWindow->SetFocus(); 

	
}



void CDimStyleComboBox::OnDropDown()
{
	UpdateDimStyleList();
}

void CDimStyleComboBox::OnCloseUp ()
{
	SDS_CMainWindow->SetFocus(); 
}


/////////////////////////////////////////////////////////////////////////////
// CDimStyleComboBox implementation

void CDimStyleComboBox::UpdateDimStyleList()
{

	CFont *pFont = new CFont();
	if (pFont)
	{
		pFont->CreateStockObject(DEFAULT_GUI_FONT);
		SetFont(pFont);
	}
	delete pFont;
	ResetContent();
	CString csTmp;
	SDS_StartTblRead("DIMSTYLE"/*DNT*/);
	while(SDS_TblReadHasMore()) 
	{
  		csTmp=SDS_TblReadString();
		if(csTmp.Find(CMD_XREF_TBLDIVIDER)>0) continue;
		AddString(csTmp);
	}
	resbuf rb;
	SDS_getvar(NULL,DB_QDIMSTYLE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	int position = SelectString(0,rb.resval.rstring);
	IC_FREE(rb.resval.rstring);	
	SetCurSel(position);
	SDS_CMainWindow->SetFocus(); 
}


void CDimStyleComboBox::AddToolTip ()
{
 
   m_ToolTipCtrl.Create(this);
   m_ToolTipCtrl.AddTool(this, ResourceString(IDC_TBMNU_1484,"Dimension Style"/*DNT*/), &m_rect,IDC_DIMSTYLE_COMBOBOX); 

}
