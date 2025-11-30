/* D:\DEV\PRJ\ICAD\ICADSTATUSBAR.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

/////////////////////////////////////////////////////////////////////
// Icad status bar class

#include "Icad.h"/*DNT*/
#include "IcadApi.h"
#include "IcadCmdBar.h"
#include "IcadToolBarMain.h"/*DNT*/
#include "cmds.h"/*DNT*/

#include "win32misc.h"
#include <vector>

//Bugzilla No. 78447 ; 18-03-2003 [
void SDS_SetOsnapCursor(char *snapstr,int *osnapmode,bool readvar);
extern int SDS_AtCmdLine;
//Bugzilla No. 78447 ; 18-03-2003 ]

#define ICAD_STATUSMNU_BEGIN          0
#define ICAD_STATUSMNU_EXPLORER       1

#define ICAD_STATUSMNU_PROPLAY        4
#define ICAD_STATUSMNU_PROPLTP        5
#define ICAD_STATUSMNU_PROPLW         6 // EBATECH(CNBR)
#define ICAD_STATUSMNU_PROPSTY        7 // EBATECH
#define ICAD_STATUSMNU_PROPDIM        8 // EBATECH
#define ICAD_STATUSMNU_SNAP           9
#define ICAD_STATUSMNU_SNAPON        10
#define ICAD_STATUSMNU_SNAPOFF       11
#define ICAD_STATUSMNU_OSNAP         12
#define ICAD_STATUSMNU_OSNAPON       13
#define ICAD_STATUSMNU_OSNAPOFF      14
#define	ICAD_STATUSMNU_SCIENTIFIC    15
#define	ICAD_STATUSMNU_DECIMAL       16
#define	ICAD_STATUSMNU_ENGINEERING   17
#define	ICAD_STATUSMNU_ARCHITECTURAL 18
#define	ICAD_STATUSMNU_FRACTIONAL    19


//----------------------------------------------------------
// Menu Items for LWT Pane
#define ICAD_STATUSMNU_LWT								20	
#define ICAD_STATUSMNU_LWT_ON							21	
#define ICAD_STATUSMNU_LWT_OFF							22	
//----------------------------------------------------------

//----------------------------------------------------------
// Menu Items for TILE Pane
#define ICAD_STATUSMNU_TILE_MODEL						23	
#define ICAD_STATUSMNU_TILE_PAPER						24	
//----------------------------------------------------------

//----------------------------------------------------------
// Menu Items for GRID Pane
#define ICAD_STATUSMNU_GRID								25	
#define ICAD_STATUSMNU_GRID_ON							26	
#define ICAD_STATUSMNU_GRID_OFF							27	
//----------------------------------------------------------

//----------------------------------------------------------
// Menu Items for ORTHO Pane
#define ICAD_STATUSMNU_ORTHO_ON							28	
#define ICAD_STATUSMNU_ORTHO_OFF						29	
//----------------------------------------------------------

//----------------------------------------------------------
// Menu Items for TABLET Pane
#define ICAD_STATUSMNU_TABLET_ON						30	
#define ICAD_STATUSMNU_TABLET_OFF						31	
//----------------------------------------------------------


//----------------------------------------------------------
// Menu Items for Status Bar Control.

#define ICAD_STATUSMNU_COORDINATES_SHOW_HIDE			33		// Co-ordinates					
#define ICAD_STATUSMNU_CURRENT_LAYER_SHOW_HIDE			34		// Current Layer
#define ICAD_STATUSMNU_CURRENT_COLOR_SHOW_HIDE			35		// Current Color
#define ICAD_STATUSMNU_CURRENT_LINETYPE_SHOW_HIDE		36		// Current Linetype
#define ICAD_STATUSMNU_CURRENT_LINEWEIGHT_SHOW_HIDE		37		// Current Lineweight
#define ICAD_STATUSMNU_CURRENT_TEXTSTYLE_SHOW_HIDE		38		// Current Text Style
#define ICAD_STATUSMNU_CURRENT_DIMSTYLE_SHOW_HIDE		39		// Current Dimstyle
#define ICAD_STATUSMNU_SNAP_SHOW_HIDE					40		// SNAP
#define ICAD_STATUSMNU_GRID_SHOW_HIDE					41		// GRID
#define ICAD_STATUSMNU_ORTHO_SHOW_HIDE					42		// ORTHO
#define ICAD_STATUSMNU_ESNAP_SHOW_HIDE					43		// ESNAP
#define ICAD_STATUSMNU_LWT_SHOW_HIDE					44		// LWT
#define ICAD_STATUSMNU_TILE_SHOW_HIDE					45		// TILE: MODEL / PAPER
#define ICAD_STATUSMNU_TABLET_SHOW_HIDE					46		// TABLET


//----------------------------------------------------------

#define ICAD_STATUSMNU_END								47

// EBATECH-[color select menu
#define ICAD_COLORMNU_BYLAY          50
#define ICAD_COLORMNU_BYBLK          51
#define ICAD_COLORMNU_1              52
#define ICAD_COLORMNU_2              53
#define ICAD_COLORMNU_3              54
#define ICAD_COLORMNU_4              55
#define ICAD_COLORMNU_5              56
#define ICAD_COLORMNU_6              57
#define ICAD_COLORMNU_7              58
#define ICAD_COLORMNU_ELSE           59
// ]-EBATECH

#define ICAD_LAYERMNU_START          60
#define ICAD_LAYERMNU_END           100

// EBATECH-[LineType,Style,DimStyle select menu
#define ICAD_LTYPEMNU_START         101
#define ICAD_LTYPEMNU_END           150
#define ICAD_STYLEMNU_START         151
#define ICAD_STYLEMNU_END           175
#define ICAD_DIMSTMNU_START         176
#define ICAD_DIMSTMNU_END           200
// ]-EEBATECH

// EBATECH(CNBR) -[ Lineweight
#define ICAD_LWMNU_ON               201
#define ICAD_LWMNU_OFF              202
#define ICAD_LWMNU_INCH             203
#define ICAD_LWMNU_MM               204
#define ICAD_LWMNU_START            205
#define ICAD_LWMNU_END              ICAD_LWMNU_START+27
// EBATECH(CNBR) ]-

static UINT indicators[]={
    ID_SEPARATOR,
    ID_ICAD_INDICATOR_COORDS,
    ID_ICAD_INDICATOR_CURLAY,
    ID_ICAD_INDICATOR_CURCOL,
    ID_ICAD_INDICATOR_CURLTP,
    ID_ICAD_INDICATOR_CURLW,  //EBATECH(CNBR)
    ID_ICAD_INDICATOR_CURSTY, //EBATECH
    ID_ICAD_INDICATOR_CURDIM, //EBATECH
    ID_ICAD_INDICATOR_SNAP,
    ID_ICAD_INDICATOR_GRID,
    ID_ICAD_INDICATOR_ORTHO,
    ID_ICAD_INDICATOR_OSNAP,
	ID_ICAD_INDICATOR_LWT,
	ID_ICAD_INDICATOR_TILE,
    ID_ICAD_INDICATOR_TAB
};


// Indices in the indicators Array above.
const int COORDS_INDEX	= 1;
const int CURLAY_INDEX	= 2;
const int CURCOL_INDEX	= 3;
const int CURLTP_INDEX	= 4;
const int CURLW_INDEX	= 5;  
const int CURSTY_INDEX	= 6; 
const int CURDIM_INDEX	= 7; 
const int SNAP_INDEX	= 8;
const int GRID_INDEX	= 9;
const int ORTHO_INDEX	= 10;
const int OSNAP_INDEX	= 11;
const int LWT_INDEX		= 12;
const int TILE_INDEX	= 13;
const int TAB_INDEX		= 14;


IMPLEMENT_DYNCREATE(CIcadStatusBar, CStatusBar)

BEGIN_MESSAGE_MAP(CIcadStatusBar, CStatusBar)
	//{{AFX_MSG_MAP(CIcadStatusBar)
	ON_WM_CREATE()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE(ICAD_LAYERMNU_START,ICAD_LAYERMNU_END,MenuPick)
	ON_COMMAND_RANGE(ICAD_COLORMNU_BYLAY,ICAD_COLORMNU_ELSE,MenuPick)
	ON_COMMAND_RANGE(ICAD_LTYPEMNU_START,ICAD_LTYPEMNU_END,MenuPick)
	ON_COMMAND_RANGE(ICAD_STYLEMNU_START,ICAD_STYLEMNU_END,MenuPick)
	ON_COMMAND_RANGE(ICAD_DIMSTMNU_START,ICAD_DIMSTMNU_END,MenuPick)
	ON_COMMAND_RANGE(ICAD_LWMNU_ON,ICAD_LWMNU_END,MenuPick)
	ON_COMMAND_RANGE(ICAD_STATUSMNU_BEGIN,ICAD_STATUSMNU_END,MenuPick)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////
// CIcadStatusBar construction/destruction

CIcadStatusBar::CIcadStatusBar()
{
	m_pLayerMenu=NULL;
	m_pLtypeMenu=NULL;
	m_pStyleMenu=NULL;
	m_pDimstMenu=NULL;

	m_numPanes		= sizeof(indicators) / sizeof(UINT);
	m_pPaneArray	= new SPaneInfo[m_numPanes];
	m_numVisiblePanes = m_numPanes;
	
	// Copy all the IDS from "indicators" Array to "m_pPaneArray"
	for (int iIndex = 0; iIndex < m_numPanes; iIndex++)
	{
		m_pPaneArray[iIndex].uPaneValue = indicators[iIndex];
	}

	// Assign the default Status Values. By default we want the Panes 2 to 7 hidden,
	// and the rest of them to be Visible.
	m_pPaneArray[COORDS_INDEX].bShowPane	= TRUE;
	m_pPaneArray[CURLAY_INDEX].bShowPane	= FALSE;
	m_pPaneArray[CURCOL_INDEX].bShowPane	= FALSE;
	m_pPaneArray[CURLTP_INDEX].bShowPane	= FALSE;
	m_pPaneArray[CURLW_INDEX].bShowPane		= FALSE;
	m_pPaneArray[CURSTY_INDEX].bShowPane	= FALSE;
	m_pPaneArray[CURDIM_INDEX].bShowPane	= FALSE;
	m_pPaneArray[SNAP_INDEX].bShowPane		= TRUE;
	m_pPaneArray[GRID_INDEX].bShowPane		= TRUE;
	m_pPaneArray[ORTHO_INDEX].bShowPane		= TRUE;
	m_pPaneArray[OSNAP_INDEX].bShowPane		= TRUE;
	m_pPaneArray[LWT_INDEX].bShowPane		= TRUE;
	m_pPaneArray[TILE_INDEX].bShowPane		= TRUE;
	m_pPaneArray[TAB_INDEX].bShowPane		= TRUE;

	LoadSettingsFromRegistry();

	m_pProgress=NULL;
	m_pMain=NULL;
}

CIcadStatusBar::~CIcadStatusBar()
{
	if(m_pLayerMenu!=NULL) delete m_pLayerMenu;
	if(m_pLtypeMenu!=NULL) delete m_pLtypeMenu;
	if(m_pStyleMenu!=NULL) delete m_pStyleMenu;
	if(m_pDimstMenu!=NULL) delete m_pDimstMenu;
	
	SaveSettingsToRegistry();
	
	if (m_pPaneArray != NULL) 
	{
		delete [] m_pPaneArray;
		m_pPaneArray = NULL;
	}

	CloseProgressBar();
}

afx_msg int CIcadStatusBar::OnCreate(LPCREATESTRUCT lpcs)
{
	if(CStatusBar::OnCreate(lpcs)!=0) return -1;
	
	SetVisibleIndicators();
	
	m_hFont=(HFONT)SendMessage(WM_GETFONT);

	m_pMain = (CMainWindow*)FromHandle(lpcs->hwndParent);

	// EBATECH-[Stretch min size
	SetPaneInfo(0,ID_SEPARATOR,SBPS_STRETCH|SBPS_NOBORDERS,100);
	AddToolTips();

	return 0;
}

void CIcadStatusBar::AddToolTips(void)
{
	CRect rect;
	int ct,nItemID;

	HWND hSafeWnd = NULL;
	hSafeWnd = m_ToolTipCtrl.GetSafeHwnd();
	if (hSafeWnd != NULL)
		m_ToolTipCtrl.DestroyWindow();
	
	m_ToolTipCtrl.Create(this);
	
	int nItems = m_numVisiblePanes;

	for(ct=0; ct<nItems; ct++)
	{
		GetItemRect(ct,rect);
		nItemID=GetItemID(ct);
		switch(nItemID)
		{
		case ID_ICAD_INDICATOR_CURLAY:
			m_ToolTipCtrl.AddTool(this,ResourceString(IDC_ICADSTATUSBAR_CURRENT_LA_0, "Current Layer" ),rect,nItemID);
			break;
		case ID_ICAD_INDICATOR_CURCOL:
			m_ToolTipCtrl.AddTool(this,ResourceString(IDC_ICADSTATUSBAR_CURRENT_CO_1, "Current Color" ),rect,nItemID);
			break;
		case ID_ICAD_INDICATOR_CURLTP:
			m_ToolTipCtrl.AddTool(this,ResourceString(IDC_ICADSTATUSBAR_CURRENT_LI_2, "Current Linetype" ),rect,nItemID);
			break;
		case ID_ICAD_INDICATOR_CURSTY:
			m_ToolTipCtrl.AddTool(this, ResourceString(IDC_ICADSTATUSBAR_CURRENT_STYLE, "Current Text Style" ),rect,nItemID);
			break;
		case ID_ICAD_INDICATOR_CURLW:
			m_ToolTipCtrl.AddTool(this,ResourceString(IDC_ICADSTATUSBAR_CURRENT_LW, "Current Lineweight" ),rect,nItemID);
			break;
		case ID_ICAD_INDICATOR_CURDIM:
			m_ToolTipCtrl.AddTool(this,ResourceString(IDC_ICADSTATUSBAR_CURRENT_DIMST, "Current Dim Style" ),rect,nItemID);
			break;
		case ID_ICAD_INDICATOR_SNAP:
			m_ToolTipCtrl.AddTool(this,ResourceString(IDC_ICADSTATUSBAR_TOGGLE_SNA_3, "Toggle Snap On/Off" ),rect,nItemID);
			break;
		case ID_ICAD_INDICATOR_GRID:
			m_ToolTipCtrl.AddTool(this,ResourceString(IDC_ICADSTATUSBAR_TOGGLE_GRI_4, "Toggle Grid On/Off" ),rect,nItemID);
			break;
		case ID_ICAD_INDICATOR_ORTHO:
			m_ToolTipCtrl.AddTool(this,ResourceString(IDC_ICADSTATUSBAR_TOGGLE_ORT_5, "Toggle Orthogonal On/Off" ),rect,nItemID);
			break;
		case ID_ICAD_INDICATOR_OSNAP:
			m_ToolTipCtrl.AddTool(this,ResourceString(IDC_ICADSTATUSBAR_ENTITY_SNA_6, "Entity Snap Settings" ),rect,nItemID);
			break;
		
		case ID_ICAD_INDICATOR_COORDS:
			m_ToolTipCtrl.AddTool(this,ResourceString(IDC_ICADSTATUSBAR_COORDS_TIPS, "Toggle Coordinates" ),rect,nItemID);
			break;
		
		case ID_ICAD_INDICATOR_LWT:
			m_ToolTipCtrl.AddTool(this,ResourceString(IDC_ICADSTATUSBAR_TOGGLE_LWT_1, "Toggle Lineweight On/Off" ),rect,nItemID);
			break;
		
		case ID_ICAD_INDICATOR_TILE:
			m_ToolTipCtrl.AddTool(this,ResourceString(ID_ICAD_MODEL_PAPER_SPACE, "Model or Paper Space"),rect,nItemID);
			break;
		
		case ID_ICAD_INDICATOR_TAB:
			m_ToolTipCtrl.AddTool(this,ResourceString(IDC_ICADSTATUSBAR_TOGGLE_TAB, "Toggle Tablet On/Off" ),rect,nItemID);
			break;
		}
	}
	return;
}

int CIcadStatusBar::GetPaneFromPoint(CPoint point)
{
//*** This function returns the ID of the status bar pane containg the
//*** "point" point.  This function returns -1 if the point wasn't found
//*** in any of the panes.
	CRect rect;
	BOOL bFound=0;
	int ct;

	int nItems = m_numVisiblePanes;

	for(ct=0; ct<nItems; ct++) {
		GetItemRect(ct,rect);
		if(point.x>=rect.left && point.x<=rect.right) {
			bFound=1;
			break;
		}
	}
	if(!bFound) return -1;
	return GetItemID(ct);
}

BOOL CIcadStatusBar::PreTranslateMessage(MSG* pMsg)
{
	m_ToolTipCtrl.Activate(TRUE);
	m_ToolTipCtrl.RelayEvent(pMsg);
	return CStatusBar::PreTranslateMessage(pMsg);
}

afx_msg void CIcadStatusBar::OnRButtonUp(UINT nFlags, CPoint point)
{
	CPoint scrnpt(point);
	ClientToScreen(&scrnpt);

	CMainWindow* pParent = (CMainWindow*)GetParentFrame();
	if(pParent->m_bPrintPreview)
	{
		return;
	}
	int nID=GetPaneFromPoint(point);
	if(nID==ID_ICAD_INDICATOR_COORDS)
	{
		//*** Create the Coordinates popup menu
		CMenu* pMenu = new CMenu();
		pMenu->CreatePopupMenu();
		//*** Append the units options
		pMenu->AppendMenu(MF_STRING,ICAD_STATUSMNU_SCIENTIFIC   ,ResourceString(IDC_ICADSTATUSBAR_SCIENTIFIC_8, "Scientific Units" ));
		pMenu->AppendMenu(MF_STRING,ICAD_STATUSMNU_DECIMAL      ,ResourceString(IDC_ICADSTATUSBAR_DECIMAL_UN_9, "Decimal Units" ));
		pMenu->AppendMenu(MF_STRING,ICAD_STATUSMNU_ENGINEERING  ,ResourceString(IDC_ICADSTATUSBAR_ENGINEERI_10, "Engineering Units" ));
		pMenu->AppendMenu(MF_STRING,ICAD_STATUSMNU_ARCHITECTURAL,ResourceString(IDC_ICADSTATUSBAR_ARCHITECT_11, "Architectural Units" ));
		pMenu->AppendMenu(MF_STRING,ICAD_STATUSMNU_FRACTIONAL   ,ResourceString(IDC_ICADSTATUSBAR_FRACTIONA_12, "Fractional Units" ));
		//*** Put a check next to the current units setting
		struct resbuf rb;
		SDS_getvar(NULL,DB_QLUNITS,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		pMenu->CheckMenuItem(rb.resval.rint-1,MF_BYPOSITION | MF_CHECKED);
		
		//*** Display the menu
		pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON,
			scrnpt.x,scrnpt.y,
			this);
		delete pMenu;
	}
	else if(nID==ID_ICAD_INDICATOR_CURLAY)
	{
		//*** Create the Current Layer popup menu
		if(m_pLayerMenu!=NULL) delete m_pLayerMenu;
		m_pLayerMenu = new CMenu();
		m_pLayerMenu->CreatePopupMenu();

		struct resbuf *tmprb,*rbbUnSort,*rbUnSort,*pRb,*rbbSort;
		tmprb=rbbUnSort=rbUnSort=pRb=rbbSort=NULL;

        if((tmprb=sds_tblnext("LAYER"/*DNT*/,1))==NULL)
		{
			delete m_pLayerMenu;
            m_pLayerMenu=NULL;
			return;
		}
		//create a rbbUnSort list of layers
        do{
            // Modified Cybage VSB 25/02/2002 (DD/MM/YYYY)[
			// Reason: Fix for bug no 77982 from BugZilla		
			//Check if layer is FROZEN
			if ( ic_assoc(tmprb,70)==0 && ic_rbassoc->resval.rint & IC_LAYER_FROZEN ) 
			{
				IC_RELRB(tmprb);
				continue;
			}

			//Check if its XREF layer
			if ( ic_assoc(tmprb,2)==0 )
			{
				if(_tcschr(ic_rbassoc->resval.rstring,'|'/*DNT*/) != NULL)
				{
					IC_RELRB(tmprb);
					continue;
				}				
			}
			else
			{
				//layer without 2 group -> bail
				delete m_pLayerMenu;
			    IC_RELRB(tmprb);
                return;
			}
			// Modified Cybage VSB 25/02/2002 (DD/MM/YYYY)]

			if(rbbUnSort==NULL){
                rbbUnSort=rbUnSort=sds_newrb(RTSTR);
            }else{
                rbUnSort=rbUnSort->rbnext=sds_newrb(RTSTR);
            }
            rbUnSort->rbnext=NULL;
			// Modified Cybage VSB 25/02/2002 (DD/MM/YYYY)[
			// Reason: Fix for bug no 77982 from BugZilla		
            //get layer name
            //for(pRb=tmprb;(pRb!=NULL)&&(pRb->restype!=2);pRb=pRb->rbnext);
            //if(pRb==NULL)
            //{   //layer without 2 group -> bail
			//    delete m_pLayerMenu;
			//    IC_RELRB(tmprb);
            //    return;
            //}
			
            //store layers
			rbUnSort->resval.rstring= new char [strlen(ic_rbassoc->resval.rstring)+1];
            strcpy(rbUnSort->resval.rstring,ic_rbassoc->resval.rstring);
			// Modified Cybage VSB 25/02/2002 (DD/MM/YYYY)]
            //get next layer
            IC_RELRB(tmprb);
        }while((tmprb=sds_tblnext("LAYER"/*DNT*/,0))!=NULL);
        //sort layerlist
        rbbSort=cmd_listsorter(rbbUnSort);
        IC_RELRB(rbbUnSort);
        //*** Get the current layer.
		struct resbuf rb;
		SDS_getvar(NULL,DB_QCLAYER,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		CString strCLayer(rb.resval.rstring);
		IC_FREE(rb.resval.rstring); // 2003/6/1
		//*** Add the layer names to the menu.
		CString str;
		int ct;
        for(ct=0,tmprb=rbbSort;tmprb!=NULL;tmprb=tmprb->rbnext,ct++)
		{
			//*** Layer names.
    		str=tmprb->resval.rstring;
			m_pLayerMenu->AppendMenu(MF_STRING,ICAD_LAYERMNU_START+ct,str);
			if(str.Compare(strCLayer)==0)
			{
				m_pLayerMenu->CheckMenuItem(ICAD_LAYERMNU_START+ct,MF_BYCOMMAND | MF_CHECKED);
			}
		}
        IC_RELRB(rbbSort);
		
		m_pLayerMenu->AppendMenu(MF_SEPARATOR);
		m_pLayerMenu->AppendMenu(MF_STRING,ICAD_STATUSMNU_PROPLAY,ResourceString(IDC_ICADSTATUSBAR_PROPERTIE_15, "Properties..." ));
		//*** Display the menu
		m_pLayerMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON,scrnpt.x,scrnpt.y,this);
	}
	///////////////////////////////////////////////////////////////////////
	// EBATECH-[ add Color,LineType,Style,DimStyle list in menu

	// COLOR list
	else if(nID==ID_ICAD_INDICATOR_CURCOL)
	{
		//*** Create the Current Color popup menu
		CMenu* pMenu = new CMenu();
		pMenu->CreatePopupMenu();
		pMenu->AppendMenu(MF_STRING,ICAD_COLORMNU_BYLAY,ResourceString(IDC_TBMNU_513, "BYLAYER"));
		pMenu->AppendMenu(MF_STRING,ICAD_COLORMNU_BYBLK,ResourceString(IDC_TBMNU_515, "BYBLOCK"));
		pMenu->AppendMenu(MF_SEPARATOR);
		pMenu->AppendMenu(MF_STRING,ICAD_COLORMNU_1,ResourceString(IDC_TBMNU_509, "Red"));
		pMenu->AppendMenu(MF_STRING,ICAD_COLORMNU_2,ResourceString(IDC_TBMNU_507, "Yellow"));
		pMenu->AppendMenu(MF_STRING,ICAD_COLORMNU_3,ResourceString(IDC_TBMNU_505, "Green"));
		pMenu->AppendMenu(MF_STRING,ICAD_COLORMNU_4,ResourceString(IDC_TBMNU_503, "Cyan"));
		pMenu->AppendMenu(MF_STRING,ICAD_COLORMNU_5,ResourceString(IDC_TBMNU_501, "Blue"));
		pMenu->AppendMenu(MF_STRING,ICAD_COLORMNU_6,ResourceString(IDC_TBMNU_511, "Magenta"));
		pMenu->AppendMenu(MF_STRING,ICAD_COLORMNU_7,ResourceString(IDC_TBMNU_499, "White"));
		pMenu->AppendMenu(MF_STRING,ICAD_COLORMNU_ELSE,ResourceString(IDC_TBMNU_497, "Select Color..."));
		
		//*** Check current Color
		struct resbuf rb;
		SDS_getvar(NULL,DB_QCECOLOR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		int nCol = ic_colornum(rb.resval.rstring);
		IC_FREE(rb.resval.rstring);
		switch (nCol) {
			case 256:pMenu->CheckMenuItem(ICAD_COLORMNU_BYLAY,MF_CHECKED);break;
			case   0:pMenu->CheckMenuItem(ICAD_COLORMNU_BYBLK,MF_CHECKED);break;
			case   1:pMenu->CheckMenuItem(ICAD_COLORMNU_1,MF_CHECKED);break;
			case   2:pMenu->CheckMenuItem(ICAD_COLORMNU_2,MF_CHECKED);break;
			case   3:pMenu->CheckMenuItem(ICAD_COLORMNU_3,MF_CHECKED);break;
			case   4:pMenu->CheckMenuItem(ICAD_COLORMNU_4,MF_CHECKED);break;
			case   5:pMenu->CheckMenuItem(ICAD_COLORMNU_5,MF_CHECKED);break;
			case   6:pMenu->CheckMenuItem(ICAD_COLORMNU_6,MF_CHECKED);break;
			case   7:pMenu->CheckMenuItem(ICAD_COLORMNU_7,MF_CHECKED);break;
			default :pMenu->CheckMenuItem(ICAD_COLORMNU_ELSE,MF_CHECKED);
		}

		//*** Display the menu
		pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON,scrnpt.x,scrnpt.y,this);
		delete pMenu;
	}

	// LINETYPE list
	else if(nID==ID_ICAD_INDICATOR_CURLTP)
	{
		//*** Create the Current Linetype popup menu
		if(m_pLtypeMenu!=NULL) delete m_pLtypeMenu;
		m_pLtypeMenu = new CMenu();
		m_pLtypeMenu->CreatePopupMenu();

		struct resbuf *tmprb,*rbbUnSort,*rbUnSort,*pRb,*rbbSort;
		tmprb=rbbUnSort=rbUnSort=pRb=rbbSort=NULL;

        if((tmprb=sds_tblnext("LTYPE"/*DNT*/,1))==NULL)
		{
			delete m_pLtypeMenu;
            m_pLtypeMenu=NULL;
			return;
		}
		//create a rbbUnSort list of linetypes
        do{
            if(rbbUnSort==NULL){
                rbbUnSort=rbUnSort=sds_newrb(RTSTR);
            }else{
                rbUnSort=rbUnSort->rbnext=sds_newrb(RTSTR);
            }
            rbUnSort->rbnext=NULL;
            //get linetype name
            for(pRb=tmprb;(pRb!=NULL)&&(pRb->restype!=2);pRb=pRb->rbnext);
            if(pRb==NULL)
            {   //linetype without 2 group -> bail
			    delete m_pLtypeMenu;
			    IC_RELRB(tmprb);
                return;
            }
            //store linetypes
			rbUnSort->resval.rstring=new char [strlen(pRb->resval.rstring)+1];
            strcpy(rbUnSort->resval.rstring,pRb->resval.rstring);
            //get next linetype
            IC_RELRB(tmprb);
        }while((tmprb=sds_tblnext("LTYPE"/*DNT*/,0))!=NULL);
        //sort linetypelist
        rbbSort=cmd_listsorter(rbbUnSort);
        IC_RELRB(rbbUnSort);
        //*** Get the current linetype.
		struct resbuf rb;
		SDS_getvar(NULL,DB_QCELTYPE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		CString strCLtype(rb.resval.rstring);
		IC_FREE(rb.resval.rstring); // 2003/6/1
		//*** Add the linetype names to the menu.
		CString str;
		str = "BYLAYER";
		m_pLtypeMenu->AppendMenu(MF_STRING,ICAD_LTYPEMNU_START  ,str);
		if(str.Compare(strCLtype)==0)
			m_pLtypeMenu->CheckMenuItem(ICAD_LTYPEMNU_START,MF_BYCOMMAND | MF_CHECKED);

		str = "BYBLOCK";
		m_pLtypeMenu->AppendMenu(MF_STRING,ICAD_LTYPEMNU_START+1,str);
		if(str.Compare(strCLtype)==0)
			m_pLtypeMenu->CheckMenuItem(ICAD_LTYPEMNU_START+1,MF_BYCOMMAND | MF_CHECKED);

		m_pLtypeMenu->AppendMenu(MF_SEPARATOR);

		int ct;
        for(ct=2,tmprb=rbbSort;tmprb!=NULL;tmprb=tmprb->rbnext,ct++)
		{
			//*** Linetype names.
    		str=tmprb->resval.rstring;
			m_pLtypeMenu->AppendMenu(MF_STRING,ICAD_LTYPEMNU_START+ct,str);
			if(str.Compare(strCLtype)==0)
				m_pLtypeMenu->CheckMenuItem(ICAD_LTYPEMNU_START+ct,MF_BYCOMMAND | MF_CHECKED);
		}
        IC_RELRB(rbbSort);

		m_pLtypeMenu->AppendMenu(MF_SEPARATOR);
		m_pLtypeMenu->AppendMenu(MF_STRING,ICAD_STATUSMNU_PROPLTP,ResourceString(IDC_ICADSTATUSBAR_PROPERTIE_15, "Properties..." ));
		//*** Display the menu
		m_pLtypeMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON,scrnpt.x,scrnpt.y,this);
	}

	// STYLE list
	else if(nID==ID_ICAD_INDICATOR_CURSTY)
	{
		//*** Create the Current TextStyle popup menu
		if(m_pStyleMenu!=NULL) delete m_pStyleMenu;
		m_pStyleMenu = new CMenu();
		m_pStyleMenu->CreatePopupMenu();

		struct resbuf *tmprb,*rbbUnSort,*rbUnSort,*pRb,*rbbSort;
		tmprb=rbbUnSort=rbUnSort=pRb=rbbSort=NULL;

        if((tmprb=sds_tblnext("STYLE"/*DNT*/,1))==NULL)
		{
			delete m_pStyleMenu;
            m_pStyleMenu=NULL;
			return;
		}
		//create a rbbUnSort list of textstyle
        do{
            if(rbbUnSort==NULL){
                rbbUnSort=rbUnSort=sds_newrb(RTSTR);
            }else{
                rbUnSort=rbUnSort->rbnext=sds_newrb(RTSTR);
            }
            rbUnSort->rbnext=NULL;
            //get textstyle name
            for(pRb=tmprb;(pRb!=NULL)&&(pRb->restype!=2);pRb=pRb->rbnext);
            if(pRb==NULL)
            {   //textstyle without 2 group -> bail
			    delete m_pStyleMenu;
			    IC_RELRB(tmprb);
                return;
            }
            //store textstyle
			rbUnSort->resval.rstring=new char [strlen(pRb->resval.rstring)+1];
            strcpy(rbUnSort->resval.rstring,pRb->resval.rstring);
            //get next textstyle
            IC_RELRB(tmprb);
        }while((tmprb=sds_tblnext("STYLE"/*DNT*/,0))!=NULL);
        //sort textstylelist
        rbbSort=cmd_listsorter(rbbUnSort);
        IC_RELRB(rbbUnSort);
        //*** Get the current textstyle.
		struct resbuf rb;
		SDS_getvar(NULL,DB_QTEXTSTYLE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		CString strCStyle(rb.resval.rstring);
		IC_FREE(rb.resval.rstring); // 2003/6/1
		//*** Add the textstyle names to the menu.
		CString str;
		int ct;
        for(ct=0,tmprb=rbbSort;tmprb!=NULL;tmprb=tmprb->rbnext,ct++)
		{
			//*** Textstyle names.
    		str=tmprb->resval.rstring;
			m_pStyleMenu->AppendMenu(MF_STRING,ICAD_STYLEMNU_START+ct,str);
			if(str.Compare(strCStyle)==0)
				m_pStyleMenu->CheckMenuItem(ICAD_STYLEMNU_START+ct,MF_BYCOMMAND | MF_CHECKED);
		}
        IC_RELRB(rbbSort);

		m_pStyleMenu->AppendMenu(MF_SEPARATOR);
		m_pStyleMenu->AppendMenu(MF_STRING,ICAD_STATUSMNU_PROPSTY,ResourceString(IDC_ICADSTATUSBAR_PROPERTIE_15, "Properties..." ));
		//*** Display the menu
		m_pStyleMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON,scrnpt.x,scrnpt.y,this);
	}

	// DIMSTYLE list
	else if(nID==ID_ICAD_INDICATOR_CURDIM)
	{
		//*** Create the Current DimStyle popup menu
		if(m_pDimstMenu!=NULL) delete m_pDimstMenu;
		m_pDimstMenu = new CMenu();
		m_pDimstMenu->CreatePopupMenu();

		struct resbuf *tmprb,*rbbUnSort,*rbUnSort,*pRb,*rbbSort;
		tmprb=rbbUnSort=rbUnSort=pRb=rbbSort=NULL;

        if((tmprb=sds_tblnext("DIMSTYLE"/*DNT*/,1))==NULL)
		{
			delete m_pDimstMenu;
            m_pDimstMenu=NULL;
			return;
		}
		//create a rbbUnSort list of dimstyle
        do{
            if(rbbUnSort==NULL){
                rbbUnSort=rbUnSort=sds_newrb(RTSTR);
            }else{
                rbUnSort=rbUnSort->rbnext=sds_newrb(RTSTR);
            }
            rbUnSort->rbnext=NULL;
            //get dimstyle name
            for(pRb=tmprb;(pRb!=NULL)&&(pRb->restype!=2);pRb=pRb->rbnext);
            if(pRb==NULL)
            {   //dimstyle without 2 group -> bail
			    delete m_pDimstMenu;
			    IC_RELRB(tmprb);
                return;
            }
            //store dimstyle
			rbUnSort->resval.rstring=new char [strlen(pRb->resval.rstring)+1];
            strcpy(rbUnSort->resval.rstring,pRb->resval.rstring);
            //get next dimstyle
            IC_RELRB(tmprb);
        }while((tmprb=sds_tblnext("DIMSTYLE"/*DNT*/,0))!=NULL);
        //sort dimstylelist
        rbbSort=cmd_listsorter(rbbUnSort);
        IC_RELRB(rbbUnSort);
        //*** Get the current dimstyle.
		struct resbuf rb;
		SDS_getvar(NULL,DB_QDIMSTYLE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		CString strCDimst(rb.resval.rstring);
		IC_FREE(rb.resval.rstring); // 2003/6/1
		//*** Add the dimstyle names to the menu.
		CString str;
		int ct;
        for(ct=0,tmprb=rbbSort;tmprb!=NULL;tmprb=tmprb->rbnext,ct++)
		{
			//*** Dimstyle names.
    		str=tmprb->resval.rstring;
			m_pDimstMenu->AppendMenu(MF_STRING,ICAD_DIMSTMNU_START+ct,str);
			if(str.Compare(strCDimst)==0)
				m_pDimstMenu->CheckMenuItem(ICAD_DIMSTMNU_START+ct,MF_BYCOMMAND | MF_CHECKED);
		}
        IC_RELRB(rbbSort);

		m_pDimstMenu->AppendMenu(MF_SEPARATOR);
		m_pDimstMenu->AppendMenu(MF_STRING,ICAD_STATUSMNU_PROPDIM,ResourceString(IDC_ICADSTATUSBAR_PROPERTIE_15, "Properties..." ));
		//*** Display the menu
		m_pDimstMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON,scrnpt.x,scrnpt.y,this);
	}
	// ]-EBATECH
	///////////////////////////////////////////////////////////////////////
	// Lineweight add by EBATECH(CNBR)
	else if(nID==ID_ICAD_INDICATOR_CURLW)
	{
		CString cStr;
		struct resbuf rb;
		int top, i;
		//*** Create the Current Lineweight popup menu
		// EBATECH(CNBR) -[  2003/8/10 make local var due to LW don't use menu text.
		CMenu* pMenu = new CMenu();
		pMenu->CreatePopupMenu();
		pMenu->AppendMenu(MF_STRING,ICAD_LWMNU_ON,  ResourceString(IDC_ICADSTATUSBAR_ON_47, "On" ));
		pMenu->AppendMenu(MF_STRING,ICAD_LWMNU_OFF, ResourceString(IDC_ICADSTATUSBAR_OFF_48, "Off" ));
		pMenu->AppendMenu(MF_SEPARATOR);
		pMenu->AppendMenu(MF_STRING,ICAD_LWMNU_INCH,ResourceString(IDC_TBMNU_1469, "inch"));
		pMenu->AppendMenu(MF_STRING,ICAD_LWMNU_MM,  ResourceString(IDC_TBMNU_1470, "mm"));
		pMenu->AppendMenu(MF_SEPARATOR);
		SDS_getvar(NULL,DB_QLWDISPLAY,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		pMenu->CheckMenuItem((rb.resval.rint == 0 ? ICAD_LWMNU_OFF : ICAD_LWMNU_ON),MF_CHECKED);
		SDS_getvar(NULL,DB_QLWUNITS,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		pMenu->CheckMenuItem((rb.resval.rint == 0 ? ICAD_LWMNU_INCH : ICAD_LWMNU_MM),MF_CHECKED);
		top = (rb.resval.rint == 0 ? IDC_COMBO_I_LWDEFAULT : IDC_COMBO_M_LWDEFAULT);
		// The amount of Lineweight is 27
		for(i=0;i<27;i++){
			cStr.Format("LWeight %d", db_index2lweight(i-3));
			pMenu->AppendMenu(MF_STRING,ICAD_LWMNU_START+i,  ResourceString(top+i, cStr));
		}
		SDS_getvar(NULL,DB_QCELWEIGHT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		// The lowest celweight index is -3(Default)..
		pMenu->CheckMenuItem(ICAD_LWMNU_START + db_lweight2index(rb.resval.rint)+3,MF_CHECKED);
		// *** Append command bars menu

		//*** Display the menu
		pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON,scrnpt.x,scrnpt.y,this);
		delete pMenu;
		// EBATECH(CNBR) ]-
	}
	///////////////////////////////////////////////////////////////////////

	else if (nID == ID_ICAD_INDICATOR_GRID)
	{
		//*** Create the GRID popup menu
		CMenu* pMenu = new CMenu();
		pMenu->CreatePopupMenu();

		pMenu->AppendMenu(MF_STRING, ICAD_STATUSMNU_GRID_ON, ResourceString(IDC_ICADSTATUSBAR_ON_47, "On" /*DNT*/));
		pMenu->AppendMenu(MF_STRING, ICAD_STATUSMNU_GRID_OFF, ResourceString(IDC_ICADSTATUSBAR_OFF_48, "Off" /*DNT*/));

		//*** Put a check next to the current GRID setting
		struct resbuf rb;
		sds_getvar("GRIDMODE", &rb);
		if (rb.resval.rint)
			pMenu->CheckMenuItem(0, MF_BYPOSITION | MF_CHECKED);
		else
			pMenu->CheckMenuItem(1, MF_BYPOSITION | MF_CHECKED);

		pMenu->AppendMenu(MF_SEPARATOR);

		//*** Settings Menu Item...
		pMenu->AppendMenu(MF_STRING, ICAD_STATUSMNU_GRID, ResourceString(IDC_ICADSTATUSBAR_SNAP_46, "Settings..." ));

		//*** Display the menu
		pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON,scrnpt.x,scrnpt.y,this);
		delete pMenu;
	}

    else if (nID == ID_ICAD_INDICATOR_ORTHO)
	{
		//*** Create the ORTHO popup menu
		CMenu* pMenu = new CMenu();
		pMenu->CreatePopupMenu();

		pMenu->AppendMenu(MF_STRING, ICAD_STATUSMNU_ORTHO_ON, ResourceString(IDC_ICADSTATUSBAR_ON_47, "On" /*DNT*/));
		pMenu->AppendMenu(MF_STRING, ICAD_STATUSMNU_ORTHO_OFF, ResourceString(IDC_ICADSTATUSBAR_OFF_48, "Off" /*DNT*/));

		//*** Put a check next to the current ORTHO setting
		struct resbuf rb;
		sds_getvar("ORTHOMODE", &rb);
		if (rb.resval.rint)
			pMenu->CheckMenuItem(0, MF_BYPOSITION | MF_CHECKED);
		else
			pMenu->CheckMenuItem(1, MF_BYPOSITION | MF_CHECKED);

		//*** Display the menu
		pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON,scrnpt.x,scrnpt.y,this);
		delete pMenu;
	}

	else if (nID == ID_ICAD_INDICATOR_TAB)
	{
		//*** Create the TABLET popup menu
		CMenu* pMenu = new CMenu();
		pMenu->CreatePopupMenu();

		pMenu->AppendMenu(MF_STRING, ICAD_STATUSMNU_TABLET_ON, ResourceString(IDC_ICADSTATUSBAR_ON_47, "On" /*DNT*/));
		pMenu->AppendMenu(MF_STRING, ICAD_STATUSMNU_TABLET_OFF, ResourceString(IDC_ICADSTATUSBAR_OFF_48, "Off" /*DNT*/));

		//*** Put a check next to the current TABLET setting
		struct resbuf rb;
		sds_getvar("TABMODE", &rb);
		if (rb.resval.rint)
			pMenu->CheckMenuItem(0, MF_BYPOSITION | MF_CHECKED);
		else
			pMenu->CheckMenuItem(1, MF_BYPOSITION | MF_CHECKED);

		//*** Display the menu
		pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON,scrnpt.x,scrnpt.y,this);
		delete pMenu;

	}
	
	else if (nID == ID_ICAD_INDICATOR_LWT)
	{
		//*** Create the LWT (Lineweight) popup menu
		CMenu* pMenu = new CMenu();
		pMenu->CreatePopupMenu();

		pMenu->AppendMenu(MF_STRING, ICAD_STATUSMNU_LWT_ON, ResourceString(IDC_ICADSTATUSBAR_ON_47, "On" /*DNT*/));
		pMenu->AppendMenu(MF_STRING, ICAD_STATUSMNU_LWT_OFF, ResourceString(IDC_ICADSTATUSBAR_OFF_48, "Off" /*DNT*/));

		//*** Put a check next to the current LWDISPLAY setting
		struct resbuf rb;
		SDS_getvar(NULL,DB_QLWDISPLAY, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
		if (rb.resval.rint)
			pMenu->CheckMenuItem(0, MF_BYPOSITION | MF_CHECKED);
		else
			pMenu->CheckMenuItem(1, MF_BYPOSITION | MF_CHECKED);

		pMenu->AppendMenu(MF_SEPARATOR);

		//*** Settings Menu Item...
		pMenu->AppendMenu(MF_STRING, ICAD_STATUSMNU_LWT, ResourceString(IDC_ICADSTATUSBAR_SNAP_46, "Settings..." ));

		//*** Display the menu
		pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON,scrnpt.x,scrnpt.y,this);
		delete pMenu;
	}	

	else if(nID==ID_ICAD_INDICATOR_SNAP)
	{
		//*** Create the Snap popup menu
		CMenu* pMenu = new CMenu();
		pMenu->CreatePopupMenu();

		pMenu->AppendMenu(MF_STRING,ICAD_STATUSMNU_SNAPON,	ResourceString(IDC_ICADSTATUSBAR_ON_47, "On" ));
		pMenu->AppendMenu(MF_STRING,ICAD_STATUSMNU_SNAPOFF,	ResourceString(IDC_ICADSTATUSBAR_OFF_48, "Off" ));
		//*** Put a check next to the current units setting
		struct resbuf rb;
		SDS_getvar(NULL,DB_QSNAPMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		if( rb.resval.rint )
			pMenu->CheckMenuItem(0,MF_BYPOSITION | MF_CHECKED);
		else
			pMenu->CheckMenuItem(1,MF_BYPOSITION | MF_CHECKED);

		pMenu->AppendMenu(MF_SEPARATOR);

		//*** Menu Items specific to the snapping
		pMenu->AppendMenu(MF_STRING,ICAD_STATUSMNU_SNAP,ResourceString(IDC_ICADSTATUSBAR_SNAP_46, "Settings..." ));

		//*** Display the menu
		pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON,scrnpt.x,scrnpt.y,this);
		delete pMenu;
	}
	else if( nID==ID_ICAD_INDICATOR_OSNAP )
	{
		//*** Create the Snap popup menu
		CMenu* pMenu = new CMenu();
		pMenu->CreatePopupMenu();

		pMenu->AppendMenu(MF_STRING,ICAD_STATUSMNU_OSNAPON, ResourceString(IDC_ICADSTATUSBAR_ON_47, "On" ));
		pMenu->AppendMenu(MF_STRING,ICAD_STATUSMNU_OSNAPOFF, ResourceString(IDC_ICADSTATUSBAR_OFF_48, "Off" ));
		//*** Put a check next to the current units setting
		struct resbuf rb;
		SDS_getvar(NULL,DB_QOSMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
//4M Item:77->
/*
		if( rb.resval.rint & 0x4000 )
*/
		if( rb.resval.rint & IC_OSMODE_OFF )
//<-4M Item:77
            pMenu->CheckMenuItem(1,MF_BYPOSITION | MF_CHECKED);
		else
			pMenu->CheckMenuItem(0,MF_BYPOSITION | MF_CHECKED);

		pMenu->AppendMenu(MF_SEPARATOR);

		//*** Menu Items specific to the snapping
		pMenu->AppendMenu(MF_STRING,ICAD_STATUSMNU_OSNAP,ResourceString(IDC_ICADSTATUSBAR_SNAP_46, "Settings..." ));

		//*** Display the menu
		pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON,scrnpt.x,scrnpt.y,this);
		delete pMenu;
	}
	
	else if (nID == ID_ICAD_INDICATOR_TILE)
	{
		CMenu* pMenu = new CMenu();
		pMenu->CreatePopupMenu();

		//*** Menu Items specific to the TILE : MODEL / PAPER
		pMenu->AppendMenu(MF_STRING, ICAD_STATUSMNU_TILE_MODEL,	ResourceString(ID_ICAD_MODEL_SPACE_STRING, "Model Space (switches to Model Space)"));
		pMenu->AppendMenu(MF_STRING, ICAD_STATUSMNU_TILE_PAPER,	ResourceString(ID_ICAD_PAPER_SPACE_STRING, "Paper Space (switches to Paper Space)"));
		
		//*** Put a check next to the current TILE setting
		struct resbuf rb;
		SDS_getvar(NULL,DB_QTILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		int tilemode = rb.resval.rint;
		SDS_getvar(NULL,DB_QCVPORT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		int cvport = rb.resval.rint;

		if (tilemode == 0 && cvport == 1)
		{
			pMenu->CheckMenuItem(1, MF_BYPOSITION | MF_CHECKED);
		}
		else
		{
			pMenu->CheckMenuItem(0, MF_BYPOSITION | MF_CHECKED);
		}

		//*** Display the menu
		pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON,scrnpt.x,scrnpt.y,this);
		delete pMenu;
	}

	else
	{
		// Provide a context menu for Status Bar to hide / show the following menu items.
		
		// Cursor Co-ordinate Values.
		// Current Layer
		// Current Color
		// Current Linetype
		// Current Lineweight
		// Current Text Style
		// Current Dimstyle
		// SNAP
		// GRID
		// ORTHO
		// ESNAP
		// LWT
		// MODEL / PAPER
		// TABLET


		//*** Create the Snap popup menu
		CMenu* pMenu = new CMenu();
		pMenu->CreatePopupMenu();
	
		pMenu->AppendMenu(MF_STRING, ICAD_STATUSMNU_COORDINATES_SHOW_HIDE, ResourceString(IDC_ICADSTATUSBAR_COORDS_MENU, "Cursor Co-ordinate Values"));
		pMenu->AppendMenu(MF_STRING, ICAD_STATUSMNU_CURRENT_LAYER_SHOW_HIDE, ResourceString(IDC_ICADSTATUSBAR_CURRENT_LA_0, "Current Layer"));
		pMenu->AppendMenu(MF_STRING, ICAD_STATUSMNU_CURRENT_COLOR_SHOW_HIDE, ResourceString(IDC_ICADSTATUSBAR_CURRENT_CO_1, "Current Color"));
		pMenu->AppendMenu(MF_STRING, ICAD_STATUSMNU_CURRENT_LINETYPE_SHOW_HIDE,	ResourceString(IDC_ICADSTATUSBAR_CURRENT_LI_2, "Current Linetype"));
		pMenu->AppendMenu(MF_STRING, ICAD_STATUSMNU_CURRENT_LINEWEIGHT_SHOW_HIDE, ResourceString(IDC_ICADSTATUSBAR_CURRENT_LW, "Current Lineweight"));
		pMenu->AppendMenu(MF_STRING, ICAD_STATUSMNU_CURRENT_TEXTSTYLE_SHOW_HIDE, ResourceString(IDC_ICADSTATUSBAR_CURRENT_STYLE, "Current Text Style"));
		pMenu->AppendMenu(MF_STRING, ICAD_STATUSMNU_CURRENT_DIMSTYLE_SHOW_HIDE,	ResourceString(IDC_ICADSTATUSBAR_CURRENT_DIMST, "Current Dim Style"));
		pMenu->AppendMenu(MF_SEPARATOR);
		pMenu->AppendMenu(MF_STRING, ICAD_STATUSMNU_SNAP_SHOW_HIDE,	ResourceString(ID_ICAD_INDICATOR_SNAP, "SNAP"));
		pMenu->AppendMenu(MF_STRING, ICAD_STATUSMNU_GRID_SHOW_HIDE,	ResourceString(ID_ICAD_INDICATOR_GRID, "GRID"));
		pMenu->AppendMenu(MF_STRING, ICAD_STATUSMNU_ORTHO_SHOW_HIDE, ResourceString(ID_ICAD_INDICATOR_ORTHO, "ORTHO"));
		pMenu->AppendMenu(MF_STRING, ICAD_STATUSMNU_ESNAP_SHOW_HIDE, ResourceString(ID_ICAD_INDICATOR_OSNAP, "ESNAP"));
		pMenu->AppendMenu(MF_STRING, ICAD_STATUSMNU_LWT_SHOW_HIDE, ResourceString(ID_ICAD_INDICATOR_LWT, "LWT"));
		pMenu->AppendMenu(MF_STRING, ICAD_STATUSMNU_TILE_SHOW_HIDE, ResourceString(ID_ICAD_MODEL_PAPER_STRING, "Model / Paper"));
		pMenu->AppendMenu(MF_STRING, ICAD_STATUSMNU_TABLET_SHOW_HIDE, ResourceString(ID_ICAD_INDICATOR_TAB, "TABLET"));

		int iPaneIndex;
		int iMenuID;
		for (iPaneIndex = COORDS_INDEX, iMenuID = ICAD_STATUSMNU_COORDINATES_SHOW_HIDE; 
				iPaneIndex <= TAB_INDEX; 
					iPaneIndex++, iMenuID++)
		{
			if (m_pPaneArray[iPaneIndex].bShowPane)
				pMenu->CheckMenuItem(iMenuID, MF_CHECKED);
		}

		//*** Display the menu
		pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON,scrnpt.x,scrnpt.y,this);
		delete pMenu;
	}
	return;
}

afx_msg void CIcadStatusBar::OnLButtonUp(UINT nFlags, CPoint point)
{
	MSG msg;

	//*** If a WM_LBUTTONDBLCLK is in the message que, return to process the double-click message
	::PeekMessage(&msg,GetSafeHwnd(),WM_LBUTTONDBLCLK,WM_LBUTTONDBLCLK,PM_NOREMOVE);
	if(msg.message==WM_LBUTTONDBLCLK) return;

	CMainWindow* pParent=(CMainWindow*)GetParentFrame();
	if(pParent->m_bPrintPreview)
	{
		return;
	}

	int nID=GetPaneFromPoint(point);
	if(nID==ID_ICAD_INDICATOR_COORDS)
	{
		return;
	}
	else if(nID==ID_ICAD_INDICATOR_CURLAY)
	{
		return;
	}
	else if(nID==ID_ICAD_INDICATOR_CURLTP)
	{
		return;
	}
	else if(nID==ID_ICAD_INDICATOR_CURCOL)
	{
		return;
	}
	// EBATECH-[
	else if(nID==ID_ICAD_INDICATOR_CURSTY)
	{
		return;
	}
	else if(nID==ID_ICAD_INDICATOR_CURDIM)
	{
		return;
	}
	// ]-EBATECH
	// EBATECH(CNBR) -[ for Lineweight
	else if(nID==ID_ICAD_INDICATOR_CURLW)
	{
		return;
	}
	// ]-EBATECH
	else return;
}

afx_msg void CIcadStatusBar::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	resbuf rb;
    LPCTSTR tmp;
	int nID=GetPaneFromPoint(point);
	CMainWindow* pParent=(CMainWindow*)GetParentFrame();
	if(pParent->m_bPrintPreview)
	{
		return;
	}

	if(nID==0)
	{
		pParent->ShowCmdBars();
	}
	else if(nID==ID_ICAD_INDICATOR_COORDS)
	{
		return;
	}
	else if(nID==ID_ICAD_INDICATOR_CURLW)
	{
		return;
	}
	// EBATECH(CNBR) ]-
	else if(nID==ID_ICAD_INDICATOR_CURLAY)
	{
        tmp="'._EXPLAYERS"/*DNT*/;
		SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)tmp);
    }
	else if (nID==ID_ICAD_INDICATOR_CURLTP)
	{
        tmp="'._EXPLTYPES"/*DNT*/;
		SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)tmp);
	}
	else if(nID==ID_ICAD_INDICATOR_CURCOL)
	{
        tmp="'._SETCOLOR"/*DNT*/;
		SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)tmp);
	}
	// EBATECH-[
	else if (nID==ID_ICAD_INDICATOR_CURSTY)
	{
        tmp="'._FONT"/*DNT*/;
		SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)tmp);
	}
	else if (nID==ID_ICAD_INDICATOR_CURDIM)
	{
        tmp="'._SETDIM"/*DNT*/;
		SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)tmp);
	}
	// ]-EBATECH
	else if(nID==ID_ICAD_INDICATOR_SNAP)
	{
        tmp="'._SNAP;_T";
		//*** Toggle SNAPMODE on/off.
		SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)tmp);
	}
	else if(nID==ID_ICAD_INDICATOR_GRID)
	{
        tmp="'._GRID;_T"/*DNT*/;
		//*** Toggle GRIDMODE on/off.
		SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)tmp);
	}
	else if(nID==ID_ICAD_INDICATOR_ORTHO)
	{
        tmp="'._ORTHOGONAL;_T"/*DNT*/;
		//*** Toggle ORTHOMODE on/off.
		SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)tmp);
	}
	else if(nID==ID_ICAD_INDICATOR_OSNAP)
	{
/*        tmp="'._SETESNAP";
		//*** Call the osnap command
		SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)tmp);
*/
		//struct resbuf rb;
		sds_getvar("OSMODE",&rb);
//4M Item:77->
/*
		rb.resval.rint ^= 0x4000;
*/
		// Check if ESNAP is ON, and no snapping options are provided. 
		// If this is the case we need to show the Drafting Settings Dialog.
		if (rb.resval.rint == 0)
		{

			tmp = "'._SETESNAP";
			//*** Call the osnap command
			SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)tmp);

		}
		else
		{
			rb.resval.rint ^= IC_OSMODE_OFF;
			sds_setvar("OSMODE",&rb);
		}

//<-4M Item:77
        



	}
	else if (nID == ID_ICAD_INDICATOR_LWT)
	{
		sds_getvar("LWDISPLAY"/*DNT*/, &rb);
		
		rb.resval.rint = !rb.resval.rint;
		
		sds_setvar("LWDISPLAY"/*DNT*/, &rb);
	}

	else if (nID == ID_ICAD_INDICATOR_TILE)
	{
		// Check if TILEMODE is 1. If so set it to 0.  
		// i.e we switch from the MODEL Tab to the last active LAYOUT Tab.
		SDS_getvar(NULL,DB_QTILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		if(rb.resval.rint)
		{
			rb.resval.rint = 0;
			sds_setvar("TILEMODE"/*DNT*/,&rb);
		}
		else
		{
			// A Layout Tab is active. Check the value of CVPORT to determine 
			// whether the space is MODEL or PAPER. 
			// For Paper Space, CVPORT is 1.
			// For Model Space, CVPORT > 1.

			SDS_getvar(NULL,DB_QCVPORT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			if (rb.resval.rint == 1)
			{
				// If the Space is PAPER, check the number of MVIEW ports.
				int numMVports = cmd_GetNumberOfMViewPorts();
				if (numMVports > 1)
				{
					// Issue the "mspace" command to switch to the active viewport.
					SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)"^C^C._MSPACE"/*DNT*/);
				}
				else
				{
					// If there are no MVIEW ports, switch to MODEL Tab
					rb.restype = SDS_RTSHORT;
					rb.resval.rint = 1;
					sds_setvar("TILEMODE"/*DNT*/,&rb);
				}
			}
			else
			{
				// If the Space is MODEL, Put in paper space by issuing the "pspace" command. 
				SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)"^C^C._PSPACE"/*DNT*/);
			}
		}
	}

	else if(nID==ID_ICAD_INDICATOR_TAB)
	{
        tmp="'._TABLET;_T"/*DNT*/;
		//*** Toggle TABMODE on/off.
		SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)tmp);
	}
	else
	{
		return;
	}
}

void CIcadStatusBar::UpdateCurLay(void)
{
	if (!m_pPaneArray[CURLAY_INDEX].bShowPane)
		return;
	
	HGDIOBJ hOldFont=NULL;
	struct resbuf rb;

	SDS_getvar(NULL,DB_QCLAYER,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

	CClientDC dcScreen(NULL);
	CString strTmp(rb.resval.rstring);
	if(m_hFont!=NULL) hOldFont=dcScreen.SelectObject(m_hFont);
	CSize sizeTmp(dcScreen.GetOutputTextExtent(strTmp));

    SetPaneInfo(CommandToIndex(ID_ICAD_INDICATOR_CURLAY),
        ID_ICAD_INDICATOR_CURLAY,
        0,
        sizeTmp.cx);  //*** Width of the CURLAY pane

	SetPaneText(CommandToIndex(ID_ICAD_INDICATOR_CURLAY),strTmp);

	//*** Resize the tooltips.
	ResizeToolTips();

	if(hOldFont!=NULL) dcScreen.SelectObject(hOldFont);
	IC_FREE(rb.resval.rstring);

	// EBATECH-[ Update color pain
	UpdateCurCol();
	// ]-EBATECH

	return;
}

///////////////////////////////////////////////////////////////////////////////
void CIcadStatusBar::UpdateCurCol(void)
{
	if (!m_pPaneArray[CURCOL_INDEX].bShowPane)
		return;

	HGDIOBJ hOldFont;
	struct resbuf rb;

	SDS_getvar(NULL,DB_QCECOLOR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

	CClientDC dcScreen(NULL);
	if(m_hFont!=NULL) hOldFont=dcScreen.SelectObject(m_hFont);
	CString strTmp(rb.resval.rstring);
	int nCurCol;
//	BOOL bByFlag=0;
	CSize sizeTmp;
	UINT nStyle=0;

	if(strTmp.CompareNoCase("BYBLOCK"/*DNT*/)==0){
		nCurCol = 0;
	}
	else if (strTmp.CompareNoCase("BYLAYER"/*DNT*/)==0){
		nCurCol = 256;
	}
	else{
		nCurCol=ic_colornum(strTmp.GetBuffer(0));
	}
		sizeTmp=dcScreen.GetOutputTextExtent("COL");
		nStyle=SBPS_DISABLED;

    SetPaneInfo(CommandToIndex(ID_ICAD_INDICATOR_CURCOL),
        ID_ICAD_INDICATOR_CURCOL,
        nStyle,
        sizeTmp.cx);  //*** Width of the CURCOL pane

	GetStatusBarCtrl().SetText((const char*)nCurCol,CommandToIndex(ID_ICAD_INDICATOR_CURCOL),SBT_OWNERDRAW);

	//*** Resize the tooltips.
	ResizeToolTips();

	IC_FREE(rb.resval.rstring);
	if(hOldFont!=NULL) dcScreen.SelectObject(hOldFont);
	return;
}
///////////////////////////////////////////////////////////////////////////////
void CIcadStatusBar::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	int nIndex = lpDrawItemStruct->itemID;
	UINT nID = GetItemID(nIndex);

	if(0==nIndex)
	{
		if(NULL!=m_pProgress) m_pProgress->SetPos((int)lpDrawItemStruct->itemData);
		return;
	}

	CString strTmp = "";

	if(nID==ID_ICAD_INDICATOR_CURCOL)
	{
		int nCol = (int)lpDrawItemStruct->itemData;
		if ((0 < nCol) && (nCol < 256)){
			// FillRect
			HPALETTE oldpal=::SelectPalette(lpDrawItemStruct->hDC,(HPALETTE)SDS_CMainWindow->m_pPalette->GetSafeHandle(),TRUE);
			HBRUSH hBr = ::CreateSolidBrush(SDS_BrushColorFromACADColor(nCol));
			::FillRect(lpDrawItemStruct->hDC,&lpDrawItemStruct->rcItem,hBr);
			//Resource leak: delete the brush
			VERIFY(::DeleteObject(hBr));
		}
		else{
			strTmp = ((nCol == 256) ? "LAY" : "BLK");
			/*// get screen background color
			struct resbuf rb;
			SDS_getvar(NULL,DB_QBKGCOLOR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			nBkCol = rb.resval.rint;*/
			int nBkCol = 0;

			// ByLayer
			if (nCol == 256){
				//get current layer color
				
				// It is not necessary that if COLOR_PANE is shown, LAYER_PANE is also
				// shown. It may be hidden. 
				// In this case "GetPaneText" gives access violation for ID_ICAD_INDICATOR_CURLAY.
				// Therefore get the current Layer_Name Text as follows:
				//!!! CString strLayname;
				//!!! GetPaneText(CommandToIndex(ID_ICAD_INDICATOR_CURLAY),strLayname);
				struct resbuf rbstr;
				SDS_getvar(NULL,DB_QCLAYER,&rbstr,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
								
				/*DG 2.10.2001*/// Some things done for avoiding memory leaks.
				//!!! resbuf	*pRrb = sds_tblsearch("LAYER",strLayname,0),
				resbuf	*pRrb = sds_tblsearch("LAYER", rbstr.resval.rstring, 0),
						*pCurRb = pRrb;

				while(pCurRb && pCurRb->restype != 62)
					pCurRb = pCurRb->rbnext;
				if(pCurRb)
					nCol = pCurRb->resval.rint;
				else
					nCol = 7;

				sds_relrb(pRrb);
			}
			// ByBlock
			else{
				nCol = 7;
			}
			// DrawText
			if(m_hFont==NULL) return;
			HGDIOBJ hOldFont = ::SelectObject(lpDrawItemStruct->hDC,m_hFont);
			//COLORREF crOldBkColor   = ::SetBkColor(lpDrawItemStruct->hDC,SDS_RGBFromACADColor(nBkCol));
			COLORREF crOldBkColor   = ::SetBkColor(lpDrawItemStruct->hDC,::GetSysColor(COLOR_BTNFACE));
			COLORREF crOldTextColor = ::SetTextColor(lpDrawItemStruct->hDC,SDS_RGBFromACADColor(nCol));

			::DrawText(lpDrawItemStruct->hDC,strTmp,-1,&lpDrawItemStruct->rcItem,
				DT_SINGLELINE | DT_VCENTER | DT_CENTER);

			//*** Set the old values back into the device context.
			::SelectObject(lpDrawItemStruct->hDC,hOldFont);
			::SetBkColor(lpDrawItemStruct->hDC,crOldBkColor);
			::SetTextColor(lpDrawItemStruct->hDC,crOldTextColor);
		}
	}
	else{
		strTmp = GetPaneText(nIndex);

		if(m_hFont==NULL) return;
		HGDIOBJ hOldFont = ::SelectObject(lpDrawItemStruct->hDC,m_hFont);
		COLORREF crOldBkColor = ::SetBkColor(lpDrawItemStruct->hDC,::GetSysColor(COLOR_BTNFACE));
		COLORREF crOldTextColor;
		if((BOOL)lpDrawItemStruct->itemData)
		{
			crOldTextColor=::SetTextColor(lpDrawItemStruct->hDC,::GetSysColor(COLOR_BTNTEXT));
		}
		else
		{
			crOldTextColor=::SetTextColor(lpDrawItemStruct->hDC,::GetSysColor(COLOR_GRAYTEXT));
		}
		::DrawText(lpDrawItemStruct->hDC,strTmp,-1,&lpDrawItemStruct->rcItem,
			DT_SINGLELINE | DT_VCENTER | DT_CENTER);

		//*** Set the old values back into the device context.
		::SelectObject(lpDrawItemStruct->hDC,hOldFont);
		::SetBkColor(lpDrawItemStruct->hDC,crOldBkColor);
		::SetTextColor(lpDrawItemStruct->hDC,crOldTextColor);
	}
}
///////////////////////////////////////////////////////////////////////////////
void CIcadStatusBar::UpdateCurLType(void)
{
	if (!m_pPaneArray[CURLTP_INDEX].bShowPane)
		return;
	
	HGDIOBJ hOldFont=NULL;
	struct resbuf rb;

	SDS_getvar(NULL,DB_QCELTYPE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

	CClientDC dcScreen(NULL);
	CString strTmp(rb.resval.rstring);
	if(m_hFont!=NULL) hOldFont=dcScreen.SelectObject(m_hFont);
	CSize sizeTmp(dcScreen.GetOutputTextExtent(strTmp));

    SetPaneInfo(CommandToIndex(ID_ICAD_INDICATOR_CURLTP),
        ID_ICAD_INDICATOR_CURLTP,
        0,
        sizeTmp.cx);  //*** Width of the CURLTP pane

	SetPaneText(CommandToIndex(ID_ICAD_INDICATOR_CURLTP),strTmp);

	//*** Resize the tooltips.
	ResizeToolTips();

	if(hOldFont!=NULL) dcScreen.SelectObject(hOldFont);
	IC_FREE(rb.resval.rstring);
	return;
}

// EBATECH-[ TEXTSTYLE
void CIcadStatusBar::UpdateCurStyle(void)
{
	if (!m_pPaneArray[CURSTY_INDEX].bShowPane)
		return;
	
	HGDIOBJ hOldFont=NULL;
	struct resbuf rb;

	SDS_getvar(NULL,DB_QTEXTSTYLE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

	CClientDC dcScreen(NULL);
	CString strTmp(rb.resval.rstring);
	if(m_hFont!=NULL) hOldFont=dcScreen.SelectObject(m_hFont);
	CSize sizeTmp(dcScreen.GetOutputTextExtent(strTmp));

    SetPaneInfo(CommandToIndex(ID_ICAD_INDICATOR_CURSTY),
        ID_ICAD_INDICATOR_CURSTY,
        0,
        sizeTmp.cx);  //*** Width of the CURSTY pane

	SetPaneText(CommandToIndex(ID_ICAD_INDICATOR_CURSTY),strTmp);

	//*** Resize the tooltips.
	ResizeToolTips();

	if(hOldFont!=NULL) dcScreen.SelectObject(hOldFont);
	IC_FREE(rb.resval.rstring);
	return;
}// ]-EBATECH

// EBATECH-[ DIMSTYLE
void CIcadStatusBar::UpdateCurDimSt(void)
{
	if (!m_pPaneArray[CURDIM_INDEX].bShowPane)
		return;
	
	HGDIOBJ hOldFont=NULL;
	struct resbuf rb;

	// EBATECH(CNBR) 2002/7/30 Skip next step until CurDwg is not ready.
	if( SDS_CURDWG == NULL)
		return;
	// EBATECH(CNBR)
	SDS_getvar(NULL,DB_QDIMSTYLE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	CClientDC dcScreen(NULL);
	CString strTmp(rb.resval.rstring);
	if(m_hFont!=NULL) hOldFont=dcScreen.SelectObject(m_hFont);
	CSize sizeTmp(dcScreen.GetOutputTextExtent(strTmp));

    SetPaneInfo(CommandToIndex(ID_ICAD_INDICATOR_CURDIM),
        ID_ICAD_INDICATOR_CURDIM,
        0,
        sizeTmp.cx);  //*** Width of the CURDIM pane

	SetPaneText(CommandToIndex(ID_ICAD_INDICATOR_CURDIM),strTmp);

	//*** Resize the tooltips.
	ResizeToolTips();

	if(hOldFont!=NULL) dcScreen.SelectObject(hOldFont);
	IC_FREE(rb.resval.rstring);
	return;
}// ]-EBATECH

// EBATECH-[ LINEWEIGHT
void CIcadStatusBar::UpdateCurLw(void)
{
	if (!m_pPaneArray[CURLW_INDEX].bShowPane)
		return;
	
	HGDIOBJ hOldFont=NULL;
	struct resbuf rb;
	CString strTmp;
	int top;
	// EBATECH(CNBR) 2002/7/30 Skip next step until CurDwg is not ready.
	if( SDS_CURDWG == NULL)
		return;
	// EBATECH(CNBR)
	SDS_getvar(NULL,DB_QLWUNITS,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	top = (rb.resval.rint == 0 ? IDC_COMBO_I_LWDEFAULT : IDC_COMBO_M_LWDEFAULT);
	SDS_getvar(NULL,DB_QCELWEIGHT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	strTmp.Format("LWeight %d", rb.resval.rint);
	strTmp = ResourceString(top+db_lweight2index(rb.resval.rint)+3,strTmp);
	CClientDC dcScreen(NULL);
	if(m_hFont!=NULL) hOldFont=dcScreen.SelectObject(m_hFont);
	CSize sizeTmp(dcScreen.GetOutputTextExtent(strTmp));

    SetPaneInfo(CommandToIndex(ID_ICAD_INDICATOR_CURLW),
        ID_ICAD_INDICATOR_CURLW,
        0,
        sizeTmp.cx);  //*** Width of the CURDIM pane

	SetPaneText(CommandToIndex(ID_ICAD_INDICATOR_CURLW),strTmp);

	//*** Resize the tooltips.
	ResizeToolTips();

	if(hOldFont!=NULL) dcScreen.SelectObject(hOldFont);
	return;
}// ]-EBATECH

void CIcadStatusBar::ResizeToolTips()
{
	CRect rect;

	if (m_pPaneArray[COORDS_INDEX].bShowPane)
	{
		GetItemRect(CommandToIndex(ID_ICAD_INDICATOR_COORDS),rect);
		m_ToolTipCtrl.SetToolRect(this,ID_ICAD_INDICATOR_COORDS,rect);
	}
	
	if (m_pPaneArray[CURLAY_INDEX].bShowPane)
	{
		GetItemRect(CommandToIndex(ID_ICAD_INDICATOR_CURLAY),rect);
		m_ToolTipCtrl.SetToolRect(this,ID_ICAD_INDICATOR_CURLAY,rect);
	}
		
	if (m_pPaneArray[CURCOL_INDEX].bShowPane)
	{
		GetItemRect(CommandToIndex(ID_ICAD_INDICATOR_CURCOL),rect);
		m_ToolTipCtrl.SetToolRect(this,ID_ICAD_INDICATOR_CURCOL,rect);
	}
		
	if (m_pPaneArray[CURLTP_INDEX].bShowPane)
	{
		GetItemRect(CommandToIndex(ID_ICAD_INDICATOR_CURLTP),rect);
		m_ToolTipCtrl.SetToolRect(this,ID_ICAD_INDICATOR_CURLTP,rect);
	}

	if (m_pPaneArray[CURLW_INDEX].bShowPane)
	{
		// EBATECH(CNBR) -[
		GetItemRect(CommandToIndex(ID_ICAD_INDICATOR_CURLW),rect);
		m_ToolTipCtrl.SetToolRect(this,ID_ICAD_INDICATOR_CURLW,rect);
		// EBATECH(CNBR) ]-
	}

	if (m_pPaneArray[CURSTY_INDEX].bShowPane)
	{
		// EBATECH-[
		GetItemRect(CommandToIndex(ID_ICAD_INDICATOR_CURSTY),rect);
		m_ToolTipCtrl.SetToolRect(this,ID_ICAD_INDICATOR_CURSTY,rect);
		// ]-EBATECH
	}

	if (m_pPaneArray[CURDIM_INDEX].bShowPane)
	{
		// EBATECH-[
		GetItemRect(CommandToIndex(ID_ICAD_INDICATOR_CURDIM),rect);
		m_ToolTipCtrl.SetToolRect(this,ID_ICAD_INDICATOR_CURDIM,rect);
		// ]-EBATECH
	}

	if (m_pPaneArray[SNAP_INDEX].bShowPane)
	{
		GetItemRect(CommandToIndex(ID_ICAD_INDICATOR_SNAP),rect);
		m_ToolTipCtrl.SetToolRect(this,ID_ICAD_INDICATOR_SNAP,rect);
	}	
		
	if (m_pPaneArray[GRID_INDEX].bShowPane)
	{
		GetItemRect(CommandToIndex(ID_ICAD_INDICATOR_GRID),rect);
		m_ToolTipCtrl.SetToolRect(this,ID_ICAD_INDICATOR_GRID,rect);
	}	
		
	if (m_pPaneArray[ORTHO_INDEX].bShowPane)
	{
		GetItemRect(CommandToIndex(ID_ICAD_INDICATOR_ORTHO),rect);
		m_ToolTipCtrl.SetToolRect(this,ID_ICAD_INDICATOR_ORTHO,rect);
	}
		
	if (m_pPaneArray[OSNAP_INDEX].bShowPane)
	{
		GetItemRect(CommandToIndex(ID_ICAD_INDICATOR_OSNAP),rect);
		m_ToolTipCtrl.SetToolRect(this,ID_ICAD_INDICATOR_OSNAP,rect);
	}	
		
	if (m_pPaneArray[LWT_INDEX].bShowPane)
	{
		GetItemRect(CommandToIndex(ID_ICAD_INDICATOR_LWT), rect);
		m_ToolTipCtrl.SetToolRect(this, ID_ICAD_INDICATOR_LWT, rect);
	}
		
	if (m_pPaneArray[TILE_INDEX].bShowPane)
	{
		GetItemRect(CommandToIndex(ID_ICAD_INDICATOR_TILE), rect);
		m_ToolTipCtrl.SetToolRect(this, ID_ICAD_INDICATOR_TILE, rect);
	}
		
	if (m_pPaneArray[TAB_INDEX].bShowPane)
	{
		GetItemRect(CommandToIndex(ID_ICAD_INDICATOR_TAB),rect);
		m_ToolTipCtrl.SetToolRect(this,ID_ICAD_INDICATOR_TAB,rect);
	}

}

void CIcadStatusBar::MenuPick(UINT nId)
{
	struct resbuf rb;
    LPCSTR tmp;
	CMainWindow* pParent=(CMainWindow*)GetParentFrame();
	if(nId>=ICAD_LAYERMNU_START && nId<=ICAD_LAYERMNU_END)
	{
		if(m_pLayerMenu==NULL) return;
		char curlay[IC_ACADNMLEN];
		m_pLayerMenu->GetMenuString(nId,curlay,sizeof(curlay)-1,MF_BYCOMMAND);
		rb.restype=SDS_RTSTR;
		rb.resval.rstring = curlay;
		sds_setvar("CLAYER"/*DNT*/,&rb);
	}
	if(nId>=ICAD_LTYPEMNU_START && nId<=ICAD_LTYPEMNU_END)
	{
		if(m_pLtypeMenu==NULL) return;
		char curlty[IC_ACADNMLEN];
		m_pLtypeMenu->GetMenuString(nId,curlty,sizeof(curlty)-1,MF_BYCOMMAND);
		rb.restype=SDS_RTSTR;
		rb.resval.rstring = curlty;
		sds_setvar("CELTYPE"/*DNT*/,&rb);
	}
	if(nId>=ICAD_STYLEMNU_START && nId<=ICAD_STYLEMNU_END)
	{
		if(m_pStyleMenu==NULL) return;
		char cursty[IC_ACADNMLEN];
		m_pStyleMenu->GetMenuString(nId,cursty,sizeof(cursty)-1,MF_BYCOMMAND);
		rb.restype=SDS_RTSTR;
		rb.resval.rstring = cursty;
		sds_setvar("TEXTSTYLE"/*DNT*/,&rb);
	}
	if(nId>=ICAD_DIMSTMNU_START && nId<=ICAD_DIMSTMNU_END)
	{
		if(m_pDimstMenu==NULL) return;
		char curdim[IC_ACADNMLEN];
		m_pDimstMenu->GetMenuString(nId,curdim,sizeof(curdim)-1,MF_BYCOMMAND);
		rb.restype=SDS_RTSTR;
		rb.resval.rstring = curdim;
		sds_setvar("DIMSTYLE"/*DNT*/,&rb);
		//
		struct sds_resbuf *pDimStyle = sds_tblsearch("DIMSTYLE",curdim,0);
		cmd_setActiveDimStyle (pDimStyle);
		IC_RELRB(pDimStyle);
	}
	if(nId>=ICAD_LWMNU_START && nId<=ICAD_LWMNU_END)
	{
		rb.restype=SDS_RTSHORT;
		rb.resval.rint = db_index2lweight(nId - ICAD_LWMNU_START - 3);
		sds_setvar("CELWEIGHT"/*DNT*/,&rb);
	}
	
	if (nId >= ICAD_STATUSMNU_COORDINATES_SHOW_HIDE && 
		nId <= ICAD_STATUSMNU_TABLET_SHOW_HIDE)
	{
		ShowHideVariablePanes(nId);
		return;
	}

	//Bugzilla No. 78447 ; 18-03-2003
	char snapstr[IC_SNAPSTR];
	
	switch(nId) {
		case ICAD_STATUSMNU_EXPLORER:
            tmp="^C^C._EXPLAYERS"/*DNT*/;
			SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)tmp);
			break;
	
		case ICAD_STATUSMNU_PROPLAY:
            tmp="^C^C._EXPLAYERS"/*DNT*/;
			SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)tmp);
			break;
		case ICAD_STATUSMNU_PROPLTP:
            tmp="^C^C._EXPLTYPES"/*DNT*/;
			SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)tmp);
			break;
		
		case ICAD_STATUSMNU_OSNAP:
			// AG: don't break executed command
//			tmp="^C^C._SETESNAP";
			tmp="'._SETESNAP";
			SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)tmp);
			break;
		case ICAD_STATUSMNU_SNAPON:
			rb.restype=SDS_RTSHORT;
			rb.resval.rint=1;
			sds_setvar("SNAPMODE"/*DNT*/,&rb);
			break;
		case ICAD_STATUSMNU_SNAPOFF:
			rb.restype=SDS_RTSHORT;
			rb.resval.rint=0;
			sds_setvar("SNAPMODE"/*DNT*/,&rb);
			break;
		case ICAD_STATUSMNU_OSNAPON:
			sds_getvar("OSMODE"/*DNT*/,&rb);
//4M Item:77->
/*
			rb.resval.rint &= (~0x4000);
*/
			rb.resval.rint &= (~IC_OSMODE_OFF);
//<-4M Item:77
            sds_setvar("OSMODE"/*DNT*/,&rb);
			//Bugzilla No. 78447 ; 18-03-2003 [
			if(!SDS_AtCmdLine)
				SDS_SetOsnapCursor(snapstr,(int *)&rb.resval.rint, FALSE );
			//Bugzilla No. 78447 ; 18-03-2003 ]
			break;
		case ICAD_STATUSMNU_OSNAPOFF:
			sds_getvar("OSMODE",&rb);
//4M Item:77->
/*
			rb.resval.rint |= 0x4000;
*/
			rb.resval.rint |= IC_OSMODE_OFF;
//<-4M Item:77
            sds_setvar("OSMODE",&rb);
			//Bugzilla No. 78447 ; 18-03-2003 [
			if(!SDS_AtCmdLine)
				SDS_SetCursor(IDC_ICAD_CROSS);				
			//Bugzilla No. 78447 ; 18-03-2003 ]
			break;
		case ICAD_STATUSMNU_SCIENTIFIC :
			rb.restype=SDS_RTSHORT;
			rb.resval.rint=1;
			sds_setvar("LUNITS"/*DNT*/,&rb);
			break;
		case ICAD_STATUSMNU_DECIMAL :
			rb.restype=SDS_RTSHORT;
			rb.resval.rint=2;
			sds_setvar("LUNITS"/*DNT*/,&rb);
			break;
		case ICAD_STATUSMNU_ENGINEERING :
			rb.restype=SDS_RTSHORT;
			rb.resval.rint=3;
			sds_setvar("LUNITS"/*DNT*/,&rb);
			break;
		case ICAD_STATUSMNU_ARCHITECTURAL :
			rb.restype=SDS_RTSHORT;
			rb.resval.rint=4;
			sds_setvar("LUNITS"/*DNT*/,&rb);
			break;
		case ICAD_STATUSMNU_FRACTIONAL :
			rb.restype=SDS_RTSHORT;
			rb.resval.rint=5;
			sds_setvar("LUNITS"/*DNT*/,&rb);
			break;
		
		case ICAD_STATUSMNU_SNAP:	
		case ICAD_STATUSMNU_GRID:
			cmd_ddrmodes();
			break;

		case ICAD_STATUSMNU_GRID_ON:
			rb.restype = SDS_RTSHORT;
			rb.resval.rint = 1;
			sds_setvar("GRIDMODE"/*DNT*/, &rb);
			break;

		case ICAD_STATUSMNU_GRID_OFF:
			rb.restype = SDS_RTSHORT;
			rb.resval.rint = 0;
			sds_setvar("GRIDMODE"/*DNT*/, &rb);
			break;

		case ICAD_STATUSMNU_ORTHO_ON:
			rb.restype = SDS_RTSHORT;
			rb.resval.rint = 1;
			sds_setvar("ORTHOMODE"/*DNT*/, &rb);
			break;

		case ICAD_STATUSMNU_ORTHO_OFF:
			rb.restype = SDS_RTSHORT;
			rb.resval.rint = 0;
			sds_setvar("ORTHOMODE"/*DNT*/, &rb);
			break;

		case ICAD_STATUSMNU_TABLET_ON:
			rb.restype = SDS_RTSHORT;
			rb.resval.rint = 1;
			sds_setvar("TABMODE"/*DNT*/, &rb);
			break;

		case ICAD_STATUSMNU_TABLET_OFF:
			rb.restype = SDS_RTSHORT;
			rb.resval.rint = 0;
			sds_setvar("TABMODE"/*DNT*/, &rb);
			break;
			
		case ICAD_STATUSMNU_LWT:
			cmd_lweight();
			break;

		case ICAD_STATUSMNU_TILE_MODEL:
			SDS_getvar(NULL,DB_QTILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			
			// Only take an action if Layout Tab other than MODEL is active.
			if(rb.resval.rint == 0)  
			{
				// Get the number of CVPORT. If it is 1 we are in paper space.
				// If its greater than 1, we are already in Model Space, so do nothing.
				SDS_getvar(NULL,DB_QCVPORT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				if (rb.resval.rint == 1)
				{
					// If the Space is PAPER, check the number of MVIEW ports.
					int numMVports = cmd_GetNumberOfMViewPorts();
					if (numMVports > 1)
					{
						// Issue the "mspace" command to switch to the active viewport.
						SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)"^C^C._MSPACE"/*DNT*/);
					}
					else
					{
						// If there are no MVIEW ports, switch to MODEL Tab
						rb.restype = SDS_RTSHORT;
						rb.resval.rint = 1;
						sds_setvar("TILEMODE"/*DNT*/,&rb);
					}
				}
			}
			break;

		case ICAD_STATUSMNU_TILE_PAPER:
			// Check if TILEMODE is 1. If so set it to 0.  
			// i.e we switch from the MODEL Tab to the last active LAYOUT Tab.
			SDS_getvar(NULL,DB_QTILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			if(rb.resval.rint)
			{
				rb.resval.rint = 0;
				sds_setvar("TILEMODE"/*DNT*/,&rb);
			}
			else
			{
				// A Layout Tab is active. Check the value of CVPORT to determine 
				// whether the space is MODEL or PAPER. 
				// For Paper Space, CVPORT is 1, and we do nothing.
				// For Model Space, CVPORT > 1.

				SDS_getvar(NULL,DB_QCVPORT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				if (rb.resval.rint > 1)
				{
					// If the Space is MODEL, Put in paper space by issuing the "pspace" command. 
					SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)"^C^C._PSPACE"/*DNT*/);
				}
			}			
			break;

		case ICAD_STATUSMNU_LWT_ON:
			rb.restype = SDS_RTSHORT;
			rb.resval.rint = 1;
			sds_setvar("LWDISPLAY"/*DNT*/, &rb);
			break;
		case ICAD_STATUSMNU_LWT_OFF:
			rb.restype = SDS_RTSHORT;
			rb.resval.rint = 0;
			sds_setvar("LWDISPLAY"/*DNT*/, &rb);
			break;

		// Color
		case ICAD_COLORMNU_BYLAY:
			SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)"'._COLOR BYLAYER"/*DNT*/);
			break;
		case ICAD_COLORMNU_BYBLK:
			SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)"'._COLOR BYBLOCK"/*DNT*/);
			break;
		case ICAD_COLORMNU_1:
			SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)"'._COLOR RED"/*DNT*/);
			break;
		case ICAD_COLORMNU_2:
			SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)"'._COLOR YELLOW"/*DNT*/);
			break;
		case ICAD_COLORMNU_3:
			SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)"'._COLOR GREEN"/*DNT*/);
			break;
		case ICAD_COLORMNU_4:
			SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)"'._COLOR CYAN"/*DNT*/);
			break;
		case ICAD_COLORMNU_5:
			SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)"'._COLOR BLUE"/*DNT*/);
			break;
		case ICAD_COLORMNU_6:
			SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)"'._COLOR MAGENTA"/*DNT*/);
			break;
		case ICAD_COLORMNU_7:
			SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)"'._COLOR WHITE"/*DNT*/);
			break;
		case ICAD_COLORMNU_ELSE:
			SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)"'._SETCOLOR"/*DNT*/);
			break;
		// TextStyle
		case ICAD_STATUSMNU_PROPSTY:
            tmp="^C^C._FONT"/*DNT*/;
			SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)tmp);
			break;
		// Dimension Style
		case ICAD_STATUSMNU_PROPDIM:
            tmp="^C^C._SETDIM"/*DNT*/;
			SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)tmp);
			break;
		// ]-EBATECH
		// EBATECH(CNBR) -[ Lineweight
		case ICAD_LWMNU_ON:
			rb.restype=SDS_RTSHORT;
			rb.resval.rint=1;
			sds_setvar("LWDISPLAY"/*DNT*/,&rb);
			break;
		case ICAD_LWMNU_OFF:
			rb.restype=SDS_RTSHORT;
			rb.resval.rint=0;
			sds_setvar("LWDISPLAY"/*DNT*/,&rb);
			break;
		case ICAD_LWMNU_INCH:
			rb.restype=SDS_RTSHORT;
			rb.resval.rint=0;
			sds_setvar("LWUNITS"/*DNT*/,&rb);
			break;
		case ICAD_LWMNU_MM:
			rb.restype=SDS_RTSHORT;
			rb.resval.rint=1;
			sds_setvar("LWUNITS"/*DNT*/,&rb);
			break;
		// ]-EBATECH
	}
	// Free Menu
	if(m_pLayerMenu)
	{
		delete m_pLayerMenu;
		m_pLayerMenu=NULL;
	}
	if(m_pLtypeMenu)
	{
		delete m_pLtypeMenu;
		m_pLtypeMenu=NULL;
	}
	if(m_pStyleMenu)
	{
		delete m_pStyleMenu;
		m_pStyleMenu=NULL;
	}
	if(m_pDimstMenu)
	{
		delete m_pDimstMenu;
		m_pDimstMenu=NULL;
	}

}

void CIcadStatusBar::UpdateVariablePanes(LPCTSTR pszVariable)
{
//*** If pszVariable is NULL, all variable panes are updated.
	CString strVar;
	int nIndex;
	UINT nStyle;
	struct resbuf rb;
	BOOL bEnable;

	if(NULL!=pszVariable) strVar=pszVariable;

	if(m_pPaneArray[SNAP_INDEX].bShowPane &&
			(NULL==pszVariable || 0==strVar.CompareNoCase("SNAPMODE"/*DNT*/))
		)
	{
		nIndex=CommandToIndex(ID_ICAD_INDICATOR_SNAP);
		nStyle=GetPaneStyle(nIndex);
		SDS_getvar(NULL,DB_QSNAPMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		if(rb.resval.rint) bEnable=TRUE;
		else bEnable=FALSE;
		GetStatusBarCtrl().SetText((LPCTSTR)bEnable,nIndex,SBT_OWNERDRAW);
		if(NULL!=pszVariable) return;
	}

	if (m_pPaneArray[GRID_INDEX].bShowPane &&
			(NULL==pszVariable || 0==strVar.CompareNoCase("GRIDMODE"/*DNT*/))
		)
	{
		nIndex=CommandToIndex(ID_ICAD_INDICATOR_GRID);
		nStyle=GetPaneStyle(nIndex);
		SDS_getvar(NULL,DB_QGRIDMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		if(rb.resval.rint) bEnable=TRUE;
		else bEnable=FALSE;
		GetStatusBarCtrl().SetText((LPCTSTR)bEnable,nIndex,SBT_OWNERDRAW);
		if(NULL!=pszVariable) return;
	}

	if (m_pPaneArray[ORTHO_INDEX].bShowPane	&&
			(NULL==pszVariable || 0==strVar.CompareNoCase("ORTHOMODE"/*DNT*/))
		)
	{
		nIndex=CommandToIndex(ID_ICAD_INDICATOR_ORTHO);
		nStyle=GetPaneStyle(nIndex);
		SDS_getvar(NULL,DB_QORTHOMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		if(rb.resval.rint) bEnable=TRUE;
		else bEnable=FALSE;
		GetStatusBarCtrl().SetText((LPCTSTR)bEnable,nIndex,SBT_OWNERDRAW);
		if(NULL!=pszVariable) return;
	}

	if (m_pPaneArray[OSNAP_INDEX].bShowPane &&
			(NULL==pszVariable || 0==strVar.CompareNoCase("OSMODE"/*DNT*/))
		)
	{
		nIndex=CommandToIndex(ID_ICAD_INDICATOR_OSNAP);
		nStyle=GetPaneStyle(nIndex);
		SDS_getvar(NULL,DB_QOSMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
//4M Item:77->
/*
		if(rb.resval.rint && !(rb.resval.rint & 0x4000))
*/
		if(rb.resval.rint && !(rb.resval.rint & IC_OSMODE_OFF))
//<-4M Item:77
            bEnable=TRUE;
		else
			bEnable=FALSE;
		GetStatusBarCtrl().SetText((LPCTSTR)bEnable,nIndex,SBT_OWNERDRAW);
		if(NULL!=pszVariable) return;
	}
	
	if (m_pPaneArray[LWT_INDEX].bShowPane && 
			(NULL == pszVariable || 0 == strVar.CompareNoCase("LWDISPLAY"/*DNT*/))
		)
	{
		nIndex = CommandToIndex(ID_ICAD_INDICATOR_LWT);
		nStyle = GetPaneStyle(nIndex);
		
		SDS_getvar(NULL, DB_QLWDISPLAY, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
		bEnable = (rb.resval.rint) ? TRUE : FALSE;

		GetStatusBarCtrl().SetText((LPCTSTR)bEnable, nIndex, SBT_OWNERDRAW);
		
		if (NULL != pszVariable) return;
	}

	if (m_pPaneArray[TILE_INDEX].bShowPane && 
			(NULL == pszVariable || 
				0 == strVar.CompareNoCase("TILEMODE"/*DNT*/) ||
				0 == strVar.CompareNoCase("CVPORT"/*DNT*/) ||
				0 == strVar.CompareNoCase("CTAB"/*DNT*/)
			)
		)
	{
		nIndex = CommandToIndex(ID_ICAD_INDICATOR_TILE);
		nStyle = GetPaneStyle(nIndex);
		//** Get Tilemode and Layout name
		CString strLayout;
		SDS_getvar(NULL,DB_QTILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		if (rb.resval.rint)
		{
			strLayout = ResourceString(ID_ICAD_INDICATOR_TILE, "MODEL"/*DNT*/);
		}
		else
		{
			SDS_getvar(NULL,DB_QCVPORT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			if (rb.resval.rint == 1)
				strLayout = ResourceString(ID_ICAD_PAPER, "PAPER"/*DNT*/);
			else
				strLayout = ResourceString(ID_ICAD_INDICATOR_TILE, "MODEL"/*DNT*/);
		}
		
		HGDIOBJ hOldFont = NULL;
		CClientDC dcScreen(NULL);
		if (m_hFont != NULL) 
			hOldFont = dcScreen.SelectObject(m_hFont);

		CSize sizeTmp(dcScreen.GetOutputTextExtent(strLayout));
		SetPaneInfo(nIndex, ID_ICAD_INDICATOR_TILE, 0, sizeTmp.cx);
		SetPaneText(nIndex, strLayout);
		
		if(hOldFont != NULL) 
			dcScreen.SelectObject(hOldFont);

		if(NULL!=pszVariable) return;
	}


	if (m_pPaneArray[TAB_INDEX].bShowPane &&
			(NULL==pszVariable || 0==strVar.CompareNoCase("TABMODE"/*DNT*/))
		)
	{
		nIndex=CommandToIndex(ID_ICAD_INDICATOR_TAB);
		nStyle=GetPaneStyle(nIndex);
		SDS_getvar(NULL,DB_QTABMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		if(rb.resval.rint) bEnable=TRUE;
		else bEnable=FALSE;
		GetStatusBarCtrl().SetText((LPCTSTR)bEnable,nIndex,SBT_OWNERDRAW);
		if(NULL!=pszVariable) return;
	}

}

BOOL CIcadStatusBar::TextOut(LPCTSTR pszText)
{
	//*** This function should be called to display text on the status bar.
	if(NULL!=m_pProgress) return FALSE;
	if(NULL==pszText || 0==(*pszText)) SetWindowText(""/*DNT*/);
	else
	{
		// First, Compare new and old string. EBATECH(CNBR) 2004/4/10 -[
		CString str;
        GetWindowText(str);
        if( !str.Compare(pszText))
            return TRUE;
		// Then, Set pszText to Status Bar. ]- EBATECH(CNBR)
        str = pszText;
		int idxChar,nStrLen;
		nStrLen = str.GetLength();
		for(idxChar=0; idxChar<nStrLen; idxChar++)
		{
			// EBATECH(Futagami) for MBCS String aware-[
			if ((_MBC_LEAD ==_mbbtype((unsigned char)str[idxChar],0)) &&
				(_MBC_TRAIL==_mbbtype((unsigned char)str[idxChar+1],1)) ){
				idxChar++;
				continue;
			}
			// ]-
			if(str[idxChar]!=(char)0x95) continue;
			str.SetAt(idxChar,'-');
		}
		SetWindowText(str);
	}
	return TRUE;
}

BOOL CIcadStatusBar::InitProgressBar()
{
	CloseProgressBar();

	struct resbuf rb;
	SDS_getvar(NULL,DB_QPROGBAR,&rb,NULL,&SDS_CURCFG,&SDS_CURSES);
	if(rb.resval.rint==0) return FALSE;

	//*** Change the first statusbar pane to an ownerdraw.
	UINT nStyle = GetPaneStyle(0);
	nStyle|=SBPS_OWNERDRAW;
	SetPaneStyle(0,nStyle);
	CRect rect;
	GetItemRect(0,rect);
	m_pProgress = new CProgressCtrl();
	if(NULL==m_pProgress) return FALSE;
	if(NULL!=m_pMain && NULL!=m_pMain->m_pToolBarMain)
	{
		((CIcadToolBarMain*)m_pMain->m_pToolBarMain)->SetHelpStringOutputWnd(NULL);
	}
	rect.right=(((rect.right-rect.left)/2)+rect.left);
	m_pProgress->Create(WS_VISIBLE,rect,this,1);
	GetStatusBarCtrl().SetText((LPCTSTR)0,0,SBT_OWNERDRAW | SBT_NOBORDERS);
	return TRUE;
}

BOOL CIcadStatusBar::UpdateProgressBar(int nPercentage)
{
	static int nSavePercentage;
	if(NULL==m_pProgress) return FALSE;
	if(nSavePercentage==nPercentage) return(TRUE);
	GetStatusBarCtrl().SetText((LPCTSTR)nPercentage,0,SBT_OWNERDRAW | SBT_NOBORDERS);
	nSavePercentage=nPercentage;
	UpdateWindow();
	return TRUE;
}

BOOL CIcadStatusBar::CloseProgressBar()
{
	if(NULL==m_pProgress) return FALSE;
	delete m_pProgress;
	m_pProgress=NULL;
	UINT nStyle = GetPaneStyle(0);
	nStyle&=(~SBPS_OWNERDRAW);
	SetPaneStyle(0,nStyle);
	if(NULL!=m_pMain && NULL!=m_pMain->m_pToolBarMain)
	{
		((CIcadToolBarMain*)m_pMain->m_pToolBarMain)->SetHelpStringOutputWnd(this);
	}
	return TRUE;
}

void CIcadStatusBar::SetVisibleIndicators()
{
	using namespace std;
	vector<UINT> visiblePanes; 
	
	// Always assign the First default Seperator Pane.
	visiblePanes.push_back(m_pPaneArray[0].uPaneValue);

	// Get only those panes which have "bShowPane" as TRUE, in the vector
	int iIndex;
	for (iIndex = 1; iIndex < m_numPanes; iIndex++)
	{
		if (m_pPaneArray[iIndex].bShowPane)
			visiblePanes.push_back(m_pPaneArray[iIndex].uPaneValue);
	}

	UINT* pArray = NULL;
	m_numVisiblePanes = visiblePanes.size();
	pArray = new UINT [m_numVisiblePanes];
	
	// Copy them in UINT Array
	for (iIndex = 0; iIndex < m_numVisiblePanes; iIndex++)
		pArray[iIndex] = visiblePanes[iIndex];

	// Now set the Indicators from this new array.
	SetIndicators(pArray, m_numVisiblePanes);

	// Free the memory for this temporary array.
	delete [] pArray;
	pArray = NULL;
}



void CIcadStatusBar::ShowHideVariablePanes(UINT nMenuID)
{

	switch (nMenuID)
	{
		case ICAD_STATUSMNU_COORDINATES_SHOW_HIDE:
			m_pPaneArray[COORDS_INDEX].bShowPane	= !m_pPaneArray[COORDS_INDEX].bShowPane;
			break;
	
		case ICAD_STATUSMNU_CURRENT_LAYER_SHOW_HIDE:
			m_pPaneArray[CURLAY_INDEX].bShowPane	= !m_pPaneArray[CURLAY_INDEX].bShowPane;
			break;

		case ICAD_STATUSMNU_CURRENT_COLOR_SHOW_HIDE:
			m_pPaneArray[CURCOL_INDEX].bShowPane	= !m_pPaneArray[CURCOL_INDEX].bShowPane;
			break;

		case ICAD_STATUSMNU_CURRENT_LINETYPE_SHOW_HIDE:
			m_pPaneArray[CURLTP_INDEX].bShowPane	= !m_pPaneArray[CURLTP_INDEX].bShowPane;
			break;
		
		case ICAD_STATUSMNU_CURRENT_LINEWEIGHT_SHOW_HIDE:
			m_pPaneArray[CURLW_INDEX].bShowPane		= !m_pPaneArray[CURLW_INDEX].bShowPane;
			break;

		case ICAD_STATUSMNU_CURRENT_TEXTSTYLE_SHOW_HIDE:
			m_pPaneArray[CURSTY_INDEX].bShowPane	= !m_pPaneArray[CURSTY_INDEX].bShowPane;
			break;

		case ICAD_STATUSMNU_CURRENT_DIMSTYLE_SHOW_HIDE:
			m_pPaneArray[CURDIM_INDEX].bShowPane	= !m_pPaneArray[CURDIM_INDEX].bShowPane;
			break;

		case ICAD_STATUSMNU_SNAP_SHOW_HIDE:
			m_pPaneArray[SNAP_INDEX].bShowPane		= !m_pPaneArray[SNAP_INDEX].bShowPane;
			break;

		case ICAD_STATUSMNU_GRID_SHOW_HIDE:
			m_pPaneArray[GRID_INDEX].bShowPane		= !m_pPaneArray[GRID_INDEX].bShowPane;
			break;

		case ICAD_STATUSMNU_ORTHO_SHOW_HIDE:
			m_pPaneArray[ORTHO_INDEX].bShowPane		= !m_pPaneArray[ORTHO_INDEX].bShowPane;
			break;

		case ICAD_STATUSMNU_ESNAP_SHOW_HIDE:
			m_pPaneArray[OSNAP_INDEX].bShowPane		= !m_pPaneArray[OSNAP_INDEX].bShowPane;
			break;

		case ICAD_STATUSMNU_LWT_SHOW_HIDE:
			m_pPaneArray[LWT_INDEX].bShowPane		= !m_pPaneArray[LWT_INDEX].bShowPane;
			break;

		case ICAD_STATUSMNU_TILE_SHOW_HIDE:
			m_pPaneArray[TILE_INDEX].bShowPane		= !m_pPaneArray[TILE_INDEX].bShowPane;
			break;

		case ICAD_STATUSMNU_TABLET_SHOW_HIDE:
			m_pPaneArray[TAB_INDEX].bShowPane		= !m_pPaneArray[TAB_INDEX].bShowPane;
			break;
	}


	SetVisibleIndicators();
			
	SetPaneInfo(0,ID_SEPARATOR,SBPS_STRETCH|SBPS_NOBORDERS,100);

	ExecuteUIAction(ICAD_WNDACTION_UDCOORDS);

	AddToolTips();
		
	UpdateCurLay();
	UpdateCurCol();
	UpdateCurLType();
	UpdateCurStyle();
	UpdateCurDimSt();
	UpdateCurLw();

	UpdateVariablePanes(NULL);

}


CString CIcadStatusBar::GetRegistryRoot()
{
    CString SDS_RegEntry;
    VERIFY(getRegCurrentProfileRoot(SDS_RegEntry.GetBuffer(IC_ACADBUF), IC_ACADBUF));
	SDS_RegEntry.ReleaseBuffer();
	SDS_RegEntry += "\\StatusBar";
	
	return SDS_RegEntry;
}


int CIcadStatusBar::LoadSettingsFromRegistry()
{
    HKEY hKey;
    DWORD retType;
    unsigned long nStrLen;
    char tmpBuffer[IC_ACADBUF];

    CString SDS_RegEntry = GetRegistryRoot();

    if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_CURRENT_USER, SDS_RegEntry, 0,KEY_READ, &hKey)) 
	{
        return(RTERROR);
    }

    nStrLen = sizeof(tmpBuffer);
    if (RegQueryValueEx(hKey, "Co-ordinates"/*DNT*/, NULL, &retType, (unsigned char*)tmpBuffer, &nStrLen) == ERROR_SUCCESS)
		m_pPaneArray[COORDS_INDEX].bShowPane	= atoi(tmpBuffer);

    nStrLen = sizeof(tmpBuffer);
    if (RegQueryValueEx(hKey, "CurrentLayer"/*DNT*/, NULL, &retType, (unsigned char*)tmpBuffer, &nStrLen) == ERROR_SUCCESS)
		m_pPaneArray[CURLAY_INDEX].bShowPane	= atoi(tmpBuffer);

    nStrLen = sizeof(tmpBuffer);
    if (RegQueryValueEx(hKey, "CurrentColor"/*DNT*/, NULL, &retType, (unsigned char*)tmpBuffer, &nStrLen) == ERROR_SUCCESS)
		m_pPaneArray[CURCOL_INDEX].bShowPane	= atoi(tmpBuffer);

    nStrLen = sizeof(tmpBuffer);
    if (RegQueryValueEx(hKey, "CurrentLinetype"/*DNT*/, NULL, &retType, (unsigned char*)tmpBuffer, &nStrLen) == ERROR_SUCCESS)
		m_pPaneArray[CURLTP_INDEX].bShowPane	= atoi(tmpBuffer);

    nStrLen = sizeof(tmpBuffer);
    if (RegQueryValueEx(hKey, "CurrentLineweight"/*DNT*/, NULL, &retType, (unsigned char*)tmpBuffer, &nStrLen) == ERROR_SUCCESS)
		m_pPaneArray[CURLW_INDEX].bShowPane		= atoi(tmpBuffer);

    nStrLen = sizeof(tmpBuffer);
    if (RegQueryValueEx(hKey, "CurrentTextstyle"/*DNT*/, NULL, &retType, (unsigned char*)tmpBuffer, &nStrLen) == ERROR_SUCCESS)
		m_pPaneArray[CURSTY_INDEX].bShowPane	= atoi(tmpBuffer);

    nStrLen = sizeof(tmpBuffer);
    if (RegQueryValueEx(hKey, "CurrentDimStyle"/*DNT*/, NULL, &retType, (unsigned char*)tmpBuffer, &nStrLen) == ERROR_SUCCESS)
		m_pPaneArray[CURDIM_INDEX].bShowPane	= atoi(tmpBuffer);

    nStrLen = sizeof(tmpBuffer);
    if (RegQueryValueEx(hKey, "SNAP"/*DNT*/, NULL, &retType, (unsigned char*)tmpBuffer, &nStrLen) == ERROR_SUCCESS)
		m_pPaneArray[SNAP_INDEX].bShowPane		= atoi(tmpBuffer);

    nStrLen = sizeof(tmpBuffer);
    if (RegQueryValueEx(hKey, "GRID"/*DNT*/, NULL, &retType, (unsigned char*)tmpBuffer, &nStrLen) == ERROR_SUCCESS)
		m_pPaneArray[GRID_INDEX].bShowPane		= atoi(tmpBuffer);
    
	nStrLen = sizeof(tmpBuffer);
    if (RegQueryValueEx(hKey, "ORTHO"/*DNT*/, NULL, &retType, (unsigned char*)tmpBuffer, &nStrLen) == ERROR_SUCCESS)
		m_pPaneArray[ORTHO_INDEX].bShowPane		= atoi(tmpBuffer);
    
	nStrLen = sizeof(tmpBuffer);
    if (RegQueryValueEx(hKey, "ESNAP"/*DNT*/, NULL, &retType, (unsigned char*)tmpBuffer, &nStrLen) == ERROR_SUCCESS)
		m_pPaneArray[OSNAP_INDEX].bShowPane		= atoi(tmpBuffer);
    
	nStrLen = sizeof(tmpBuffer);
    if (RegQueryValueEx(hKey, "LWT"/*DNT*/, NULL, &retType, (unsigned char*)tmpBuffer, &nStrLen) == ERROR_SUCCESS)
		m_pPaneArray[LWT_INDEX].bShowPane		= atoi(tmpBuffer);
    
	nStrLen = sizeof(tmpBuffer);
    if (RegQueryValueEx(hKey, "MODEL-PAPER"/*DNT*/, NULL, &retType, (unsigned char*)tmpBuffer, &nStrLen) == ERROR_SUCCESS)
		m_pPaneArray[TILE_INDEX].bShowPane		= atoi(tmpBuffer);
    
	nStrLen = sizeof(tmpBuffer);
    if (RegQueryValueEx(hKey, "TABLET"/*DNT*/, NULL, &retType, (unsigned char*)tmpBuffer, &nStrLen) == ERROR_SUCCESS)
		m_pPaneArray[TAB_INDEX].bShowPane		= atoi(tmpBuffer);


    RegCloseKey(hKey);

    return RTNORM;
}

int CIcadStatusBar::SaveSettingsToRegistry()
{
    HKEY hKey;
    DWORD retval;
    char tmpBuffer[IC_ACADBUF];

    CString SDS_RegEntry = GetRegistryRoot();

    if(ERROR_SUCCESS != RegCreateKeyEx(HKEY_CURRENT_USER,
									   SDS_RegEntry,
									   0,
									   ""/*DNT*/,
									   REG_OPTION_NON_VOLATILE,
									   KEY_ALL_ACCESS,
									   NULL,
									   &hKey,
									   &retval)
		) 
	{
        return(RTERROR);
    }

	sprintf(tmpBuffer, "%d", (int)m_pPaneArray[COORDS_INDEX].bShowPane);
    RegSetValueEx(hKey, "Co-ordinates"/*DNT*/, 0, REG_SZ, (const unsigned char*)tmpBuffer, strlen(tmpBuffer) + 1);

	sprintf(tmpBuffer, "%d", (int)m_pPaneArray[CURLAY_INDEX].bShowPane);
    RegSetValueEx(hKey, "CurrentLayer"/*DNT*/, 0, REG_SZ, (const unsigned char*)tmpBuffer, strlen(tmpBuffer) + 1);

	sprintf(tmpBuffer, "%d", (int)m_pPaneArray[CURCOL_INDEX].bShowPane);
    RegSetValueEx(hKey, "CurrentColor"/*DNT*/, 0, REG_SZ, (const unsigned char*)tmpBuffer, strlen(tmpBuffer) + 1);
	
	sprintf(tmpBuffer, "%d", (int)m_pPaneArray[CURLTP_INDEX].bShowPane);
    RegSetValueEx(hKey, "CurrentLinetype"/*DNT*/, 0, REG_SZ, (const unsigned char*)tmpBuffer, strlen(tmpBuffer) + 1);
	
	sprintf(tmpBuffer, "%d", (int)m_pPaneArray[CURLW_INDEX].bShowPane);
    RegSetValueEx(hKey, "CurrentLineweight"/*DNT*/, 0, REG_SZ, (const unsigned char*)tmpBuffer, strlen(tmpBuffer) + 1);
	
	sprintf(tmpBuffer, "%d", (int)m_pPaneArray[CURSTY_INDEX].bShowPane);
    RegSetValueEx(hKey, "CurrentTextstyle"/*DNT*/, 0, REG_SZ, (const unsigned char*)tmpBuffer, strlen(tmpBuffer) + 1);
	
	sprintf(tmpBuffer, "%d", (int)m_pPaneArray[CURDIM_INDEX].bShowPane);
    RegSetValueEx(hKey, "CurrentDimStyle"/*DNT*/, 0, REG_SZ, (const unsigned char*)tmpBuffer, strlen(tmpBuffer) + 1);
	
	sprintf(tmpBuffer, "%d", (int)m_pPaneArray[SNAP_INDEX].bShowPane);
    RegSetValueEx(hKey, "SNAP"/*DNT*/, 0, REG_SZ, (const unsigned char*)tmpBuffer, strlen(tmpBuffer) + 1);
	
	sprintf(tmpBuffer, "%d", (int)m_pPaneArray[GRID_INDEX].bShowPane);
    RegSetValueEx(hKey, "GRID"/*DNT*/, 0, REG_SZ, (const unsigned char*)tmpBuffer, strlen(tmpBuffer) + 1);
	
	sprintf(tmpBuffer, "%d", (int)m_pPaneArray[ORTHO_INDEX].bShowPane);
    RegSetValueEx(hKey, "ORTHO"/*DNT*/, 0, REG_SZ, (const unsigned char*)tmpBuffer, strlen(tmpBuffer) + 1);
	
	sprintf(tmpBuffer, "%d", (int)m_pPaneArray[OSNAP_INDEX].bShowPane);
    RegSetValueEx(hKey, "ESNAP"/*DNT*/, 0, REG_SZ, (const unsigned char*)tmpBuffer, strlen(tmpBuffer) + 1);
	
	sprintf(tmpBuffer, "%d", (int)m_pPaneArray[LWT_INDEX].bShowPane);
    RegSetValueEx(hKey, "LWT"/*DNT*/, 0, REG_SZ, (const unsigned char*)tmpBuffer, strlen(tmpBuffer) + 1);

	sprintf(tmpBuffer, "%d", (int)m_pPaneArray[TILE_INDEX].bShowPane);
    RegSetValueEx(hKey, "MODEL-PAPER"/*DNT*/, 0, REG_SZ, (const unsigned char*)tmpBuffer, strlen(tmpBuffer) + 1);
	
	sprintf(tmpBuffer, "%d", (int)m_pPaneArray[TAB_INDEX].bShowPane);
    RegSetValueEx(hKey, "TABLET"/*DNT*/, 0, REG_SZ, (const unsigned char*)tmpBuffer, strlen(tmpBuffer) + 1);


    RegCloseKey(hKey);

    return RTNORM;

}


void CIcadStatusBar::SetCoordsPaneWidth(int nWidth)
{
	if (m_pPaneArray[COORDS_INDEX].bShowPane)
	{
		SetPaneInfo(CommandToIndex(ID_ICAD_INDICATOR_COORDS),
					ID_ICAD_INDICATOR_COORDS, 0, nWidth);
	}

}


void CIcadStatusBar::SetCoordsText(LPCTSTR pszText)
{
	if (m_pPaneArray[COORDS_INDEX].bShowPane)
		SetPaneText(CommandToIndex(ID_ICAD_INDICATOR_COORDS), pszText);
}

