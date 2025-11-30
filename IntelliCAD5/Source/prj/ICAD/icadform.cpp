/* D:\DEV\PRJ\ICAD\ICADFORM.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

#include "icad.h"/*DNT*/
#include "cmds.h"/*DNT*/
#include "IcadCusButton.h"/*DNT*/
#include "icadform.h"/*DNT*/
#include "dcl.h"/*DNT*/

#if defined(DEBUG)

struct FRM_TabInfo {
    char      *CName,
              *Name,            // Name of property dialog (TABS).
			  *Tab[4];          // Tab titles (TABS).
    int        TabCnt,          // Number of tabs for dialog (TABS).
			   TabTlen[4];      // Hold the length of the tabs (TABS).
    short      TabID[4];        // The Id of the tabs (TABS).
    RECT       Tabrect[4];      // Holds the rect size of the tabs (TABS).
                                // true, then a true value is set if this field has more than one value or false if all values are the same.
} FRM_tabinfo[]={{"#32770", "Dialog"     ,{"  General  "},1,{0},{FRM_DLG_GENERAL}},
                 {"EDIT",   "Edit"       ,{"  General  ","  Styles  ","  Events  "},3,{0},{FRM_CHLD_GENERAL,FRM_EDT_STYLE,FRM_EDT_EVENT}},
                 {"BUTTON", "Push Button",{"  General  ","  Styles  ","  Events  "},3,{0},{FRM_CHLD_GENERAL,FRM_BTN_STYLE,FRM_BTN_EVENT}},
                 {"STATIC", "Text"       ,{"  General  ","  Styles  "},2,{0},{FRM_CHLD_GENERAL,FRM_TXT_STYLE}},
				 {"LISTBOX","List Box"   ,{"  Styles  "},1,{0},{FRM_LB_STYLE}}
};

struct FRM_Event {
    int  ID;
} FRM_eventlist[]    ={FRM_CHLD_ENTER,FRM_CHLD_EXIT,FRM_CHLD_GFOCUS,FRM_CHLD_LFOCUS,FRM_CHLD_CLICK,
                       FRM_CHLD_DBLCLICK,FRM_CHLD_MDOWN,FRM_CHLD_MMOVE,FRM_CHLD_MUP,FRM_CHLD_KDOWN},
  FRM_edt_eventlist[]={FRM_EDT_BUPDATE,FRM_EDT_AUPDATE,FRM_EDT_CHANGE,FRM_EDT_ENTER,FRM_EDT_EXIT,
					   FRM_EDT_GFOCUS,FRM_EDT_LFOCUS,FRM_EDT_CLICK	,FRM_EDT_DBLCLICK,FRM_EDT_MDOWN,
					   FRM_EDT_MMOVE,FRM_EDT_MUP,FRM_EDT_KDOWN,FRM_EDT_KUP};

struct FRM_idlist {
    long style,
         ctrlid;
} FRM_idedt_style[]={{ES_WANTRETURN,FRM_EDT_WANTRET},{WS_BORDER,FRM_EDT_BORDER},{ES_MULTILINE,FRM_EDT_MULTILINE},{ES_UPPERCASE,FRM_EDT_UPPERCAS},
                     {ES_LOWERCASE,FRM_EDT_LOWERCAS},{ES_READONLY,FRM_EDT_READONLY},{WS_DLGFRAME,FRM_EDT_MODALFRM},{ES_PASSWORD,FRM_EDT_PASSWORD},
                     {ES_AUTOHSCROLL,FRM_EDT_AUTOHSCR},{ES_AUTOVSCROLL,FRM_EDT_AUTOVSCR}},
  FRM_idbtn_style[]={{BS_DEFPUSHBUTTON,FRM_BTN_DEFBUT}},
  FRM_idtxt_style[]={{SS_SIMPLE,FRM_TXT_SIMPLE},{WS_BORDER,FRM_TXT_BORDER}};


CWnd         *FRM_Parent;
FRM_frmInfo **FRM_Listptr;
long          FRM_ByteCount;
BOOL          FRM_FirstItem;
HWND          FRM_PhWnd;
char          FRM_szText[256];
short         FRM_typeindex,
              FRM_Sorttype,
              FRM_eventlist_cnt,
        	  FRM_idedt_style_cnt=sizeof(FRM_idedt_style)/sizeof(FRM_idedt_style[0]),
			  FRM_idtxt_style_cnt=sizeof(FRM_idtxt_style)/sizeof(FRM_idtxt_style[0]),
			  FRM_idbtn_style_cnt=sizeof(FRM_idbtn_style)/sizeof(FRM_idbtn_style[0]),
    		  FRM_type_cnt=sizeof(FRM_tabinfo)/sizeof(FRM_tabinfo[0]);

// ============================================================================================================================================
// ==================================================== IEXP_SUBDLG CLASS FUNCTIONS ===========================================================
// ============================================================================================================================================

BEGIN_MESSAGE_MAP(FRM_SubDlg,CDialog)
    ON_WM_DESTROY()
END_MESSAGE_MAP()

BOOL FRM_NewTableQuery( LPCTSTR query) {
    if (SDS_CMainWindow->m_CdatarecordPtr->IsOpen()) SDS_CMainWindow->m_CdatarecordPtr->Close();
    try	{ SDS_CMainWindow->m_CdatarecordPtr->Open(dbOpenDynaset,query); }
	catch(CDaoException* e)	{
        AfxMessageBox(e->m_pErrorInfo->m_strDescription); e->Delete();
        return(FALSE);
    }
    return(TRUE);
}

void FRM_SubDlg::OnDestroy() {
    int         fi1,
                fi2;
    COleVariant olefkey;
    COleVariant oleId;
    WPARAM      wParam;

    fi2=sizeof(FRM_eventlist)/sizeof(FRM_eventlist[0]);

    switch(FRM_tabinfo[FRM_typeindex].TabID[m_oldindex]) {
        case FRM_DLG_GENERAL:
            GetDlgItemText(FRM_DLG_NAME,FRM_szText,255);            
			m_PropDlg->m_DlgWin->m_FrmCur->frmname= new char [strlen(FRM_szText)+1];			
            strcpy(m_PropDlg->m_DlgWin->m_FrmCur->frmname,FRM_szText);
            m_PropDlg->m_DlgWin->m_Formname=FRM_szText;
            {
                COleVariant  formname(m_PropDlg->m_DlgWin->m_FrmCur->frmname,VT_BSTRT);
                sprintf(FRM_szText,ResourceString(IDC_ICADFORM_SELECT___FROM__13, "Select * from Forminfo where Fkey=%d" ),m_PropDlg->m_DlgWin->m_FrmCur->fkey);
                if (!FRM_NewTableQuery(FRM_szText)) goto exit;
                if (!SDS_CMainWindow->m_CdatarecordPtr->IsBOF()) {
                    SDS_CMainWindow->m_CdatarecordPtr->Edit();
                    SDS_CMainWindow->m_CdatarecordPtr->SetFieldValue(ResourceString(IDC_ICADFORM_FORMNAME_14, "FormName" ),formname);
                    SDS_CMainWindow->m_CdatarecordPtr->Update();
                }
                wParam=MAKEWPARAM(0,FRM_UPDATE_CUS);
                m_PropDlg->m_DlgWin->m_formedit->m_Parent->SendMessage(WM_COMMAND,wParam,0);
            }
            break;
		case FRM_EDT_EVENT:
        case FRM_BTN_EVENT:
            olefkey=m_PropDlg->m_DlgWin->m_FrmCur->fkey;
            oleId=m_PropDlg->m_DlgWin->m_ListCur->Wid;
            sprintf(FRM_szText,ResourceString(IDC_ICADFORM_SELECT___FROM__15, "Select * from Itempropvals where Fkey=%d and Id=%d" ),m_PropDlg->m_DlgWin->m_FrmCur->fkey,m_PropDlg->m_DlgWin->m_ListCur->Wid);
                if (!FRM_NewTableQuery(FRM_szText)) goto exit;
            if (SDS_CMainWindow->m_CdatarecordPtr->IsBOF()) {
                SDS_CMainWindow->m_CdatarecordPtr->AddNew();
    	        SDS_CMainWindow->m_CdatarecordPtr->SetFieldValue(ResourceString(IDC_ICADFORM_FKEY_16, "Fkey" ),olefkey);
	            SDS_CMainWindow->m_CdatarecordPtr->SetFieldValue(ResourceString(IDC_ICADFORM_ID_17, "Id" ),oleId);
            } else  SDS_CMainWindow->m_CdatarecordPtr->Edit();
            for(fi1=0;fi1<(fi2-1);++fi1) {
				switch(FRM_tabinfo[FRM_typeindex].TabID[m_oldindex]) {
			        case FRM_BTN_EVENT:
		                GetDlgItemText(FRM_eventlist[fi1].ID,FRM_szText,255);
						break;
					case FRM_EDT_EVENT:
		                GetDlgItemText(FRM_edt_eventlist[fi1].ID,FRM_szText,255);
						break;
				}
                if (!*FRM_szText) continue;
                COleVariant  olecomand(FRM_szText,VT_BSTRT);
	            SDS_CMainWindow->m_CdatarecordPtr->SetFieldValue((fi1+3),olecomand);
            }
            SDS_CMainWindow->m_CdatarecordPtr->Update();
            break;
        default: break;
    }
    m_oldindex=m_DlgIndex;

    exit:

    CDialog::OnDestroy();
}

///////////////////////////////////////////////////////////////////////
// This function is called right before the subdialog of the edit dialog
// displays, the sub dialogs contains all the child items such as edit
// boxes,buttons,etc.
//
// TRUE  : on success.
// FALSE : on error.
//
BOOL FRM_SubDlg::OnInitDialog() {
    int          fi1,
                 fi2;
    COleVariant  olefkey;
    COleVariant  oleId;
    CString      cmdstr;
    long         style;
	CWnd        *pWnd,
                *bWnd;

    m_oldindex=m_DlgIndex;
    fi2=sizeof(FRM_eventlist)/sizeof(FRM_eventlist[0]);

    switch(FRM_tabinfo[FRM_typeindex].TabID[m_DlgIndex]) {
        case FRM_DLG_GENERAL:
            m_PropDlg->m_DlgWin->m_mdialog->GetWindowText(FRM_szText,255);
            if (m_PropDlg->m_DlgWin->m_nChar) { FRM_szText[0]=m_PropDlg->m_DlgWin->m_nChar; FRM_szText[1]=0; }
            SetDlgItemText(FRM_CAP,FRM_szText);
            SetDlgItemText(FRM_DLG_NAME,m_PropDlg->m_DlgWin->m_FrmCur->frmname);
            SetDlgItemText(FRM_DLG_TYPEFACE,m_PropDlg->m_DlgWin->m_FrmCur->tfname);
            sprintf(FRM_szText,"%d"/*DNT*/,m_PropDlg->m_DlgWin->m_FrmCur->pointsize);
            SetDlgItemText(FRM_DLG_POINTS,FRM_szText);
            break;
        case FRM_CHLD_GENERAL:
            m_PropDlg->m_DlgWin->m_mdialog->GetDlgItemText(m_PropDlg->m_DlgWin->m_ListCur->Wid,FRM_szText,255);
            if (m_PropDlg->m_DlgWin->m_nChar) { FRM_szText[0]=m_PropDlg->m_DlgWin->m_nChar; FRM_szText[1]=0; }
            SetDlgItemText(FRM_CAP,FRM_szText);
            if (m_PropDlg->m_DlgWin->m_nChar) { pWnd=GetDlgItem(FRM_CAP); pWnd->SetFocus(); }
            break;
        case FRM_BTN_EVENT:
		case FRM_EDT_EVENT:
            olefkey=m_PropDlg->m_DlgWin->m_FrmCur->fkey;
            oleId=m_PropDlg->m_DlgWin->m_ListCur->Wid;
            sprintf(FRM_szText,ResourceString(IDC_ICADFORM_SELECT___FROM__15, "Select * from Itempropvals where Fkey=%d and Id=%d" ),m_PropDlg->m_DlgWin->m_FrmCur->fkey,m_PropDlg->m_DlgWin->m_ListCur->Wid);
            if (!FRM_NewTableQuery(FRM_szText)) goto exit;
            if (SDS_CMainWindow->m_CdatarecordPtr->IsBOF() || SDS_CMainWindow->m_CdatarecordPtr->IsEOF()) { goto exit; }
            for(fi1=0;fi1<(fi2-1);++fi1) {
                VarToCStr(&cmdstr,&SDS_CMainWindow->m_CdatarecordPtr->GetFieldValue((fi1+3)));
                if (cmdstr.IsEmpty()) continue;
				switch(FRM_tabinfo[FRM_typeindex].TabID[m_DlgIndex]) {
			        case FRM_BTN_EVENT:
		                SetDlgItemText(FRM_eventlist[fi1].ID,(LPCTSTR)cmdstr);
						break;
					case FRM_EDT_EVENT:
		                SetDlgItemText(FRM_edt_eventlist[fi1].ID,(LPCTSTR)cmdstr);
						break;
				}
            }
            break;
        case FRM_EDT_STYLE:
			pWnd=m_PropDlg->m_DlgWin->m_mdialog->GetDlgItem(m_PropDlg->m_DlgWin->m_ListCur->Wid);
			style=GetWindowLong(pWnd->m_hWnd,GWL_STYLE);
            for(fi1=0;fi1<FRM_idedt_style_cnt;++fi1) {
                if (style&FRM_idedt_style[fi1].style) { bWnd=GetDlgItem(FRM_idedt_style[fi1].ctrlid); ((CButton *)bWnd)->SetCheck(1); }
            }
			FillCombo_EDT_STYLE();
			break;
		case FRM_LB_STYLE:
			FillCombo_LB_STYLE();
			break;
		case FRM_TXT_STYLE:
			pWnd=m_PropDlg->m_DlgWin->m_mdialog->GetDlgItem(m_PropDlg->m_DlgWin->m_ListCur->Wid);
			style=GetWindowLong(pWnd->m_hWnd,GWL_STYLE);
            for(fi1=0;fi1<FRM_idtxt_style_cnt;++fi1) {
                if (style&FRM_idtxt_style[fi1].style) { bWnd=GetDlgItem(FRM_idtxt_style[fi1].ctrlid); ((CButton *)bWnd)->SetCheck(1); }
            }
			FillCombo_TXT_STYLE();
            break;
		case FRM_BTN_STYLE:
			pWnd=m_PropDlg->m_DlgWin->m_mdialog->GetDlgItem(m_PropDlg->m_DlgWin->m_ListCur->Wid);
			style=GetWindowLong(pWnd->m_hWnd,GWL_STYLE);
            for(fi1=0;fi1<FRM_idbtn_style_cnt;++fi1) {
                if (style&FRM_idbtn_style[fi1].style) { bWnd=GetDlgItem(FRM_idbtn_style[fi1].ctrlid); ((CButton *)bWnd)->SetCheck(1); }
            }
			break;
        default:
            break;
    }

    exit:
        return CDialog::OnInitDialog();
}

void FRM_SubDlg::FillCombo_EDT_STYLE()
{
	CComboBox *pCombo = NULL;

	if (NULL == (pCombo = (CComboBox *) GetDlgItem(FRM_EDT_ALIGN)))
		return;

	pCombo->ResetContent();
	CString	comboOption;
	comboOption.LoadString(IDC_COMBO_FRM_EDT_ALIGN_1);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_FRM_EDT_ALIGN_2);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_FRM_EDT_ALIGN_3);
	pCombo->AddString(comboOption);
}

void FRM_SubDlg::FillCombo_LB_STYLE()
{
	CComboBox *pCombo = NULL;

	if (NULL == (pCombo = (CComboBox *) GetDlgItem(IDC_COMBO1)))
		return;

	pCombo->ResetContent();
	CString	comboOption;
	comboOption.LoadString(IDC_COMBO_IDC_COMBO1_1);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_IDC_COMBO1_2);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_IDC_COMBO1_3);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_IDC_COMBO1_4);
	pCombo->AddString(comboOption);
}

void FRM_SubDlg::FillCombo_TXT_STYLE()
{
	CComboBox *pCombo = NULL;

	if (NULL == (pCombo = (CComboBox *) GetDlgItem(IDC_COMBO1)))
		return;

	pCombo->ResetContent();
	CString	comboOption;
	comboOption.LoadString(IDC_COMBO_ALIGN_TEXT_1);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_ALIGN_TEXT_2);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_ALIGN_TEXT_3);
	pCombo->AddString(comboOption);
}

LRESULT FRM_SubDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) {
	short fi1;
	short lwparam;

    switch(message) {
        case WM_COMMAND:
            switch(HIWORD(wParam)) {
                case BN_CLICKED:
					lwparam=LOWORD(wParam);
					switch(lwparam) {
						case FRM_DLG_FONT:
							m_PropDlg->m_DlgWin->OnNewFont();
							if (m_PropDlg->m_DlgWin->m_FrmCur) {
								if (m_PropDlg->m_DlgWin->m_FrmCur->tfname) {
									SetDlgItemText(FRM_DLG_TYPEFACE,m_PropDlg->m_DlgWin->m_FrmCur->tfname);
									sprintf(FRM_szText,"%d"/*DNT*/,m_PropDlg->m_DlgWin->m_FrmCur->pointsize);
									SetDlgItemText(FRM_DLG_POINTS,FRM_szText);
								}
							}
							break;
						default:
							for(fi1=0;fi1<FRM_idedt_style_cnt && FRM_idedt_style[fi1].ctrlid!=lwparam;++fi1);
							if (fi1<FRM_idedt_style_cnt) { ChangItemStyle(FRM_idedt_style[fi1].style); break; }

							for(fi1=0;fi1<FRM_idtxt_style_cnt && FRM_idtxt_style[fi1].ctrlid!=lwparam;++fi1);
							if (fi1<FRM_idtxt_style_cnt) { ChangItemStyle(FRM_idtxt_style[fi1].style); break; }

							for(fi1=0;fi1<FRM_idbtn_style_cnt && FRM_idbtn_style[fi1].ctrlid!=lwparam;++fi1);
							if (fi1<FRM_idbtn_style_cnt) { ChangItemStyle(FRM_idbtn_style[fi1].style); break; }
							break;
                    }
                    break;
                case EN_CHANGE:
                    if (LOWORD(wParam)==FRM_CAP) {
                        GetDlgItemText(FRM_CAP,FRM_szText,255);
                        switch(FRM_typeindex) {
                            case 0:
                                m_PropDlg->m_DlgWin->m_mdialog->SetWindowText(FRM_szText);
                                break;
                            default:
                                m_PropDlg->m_DlgWin->m_mdialog->SetDlgItemText(m_PropDlg->m_DlgWin->m_ListCur->Wid,FRM_szText);
                                break;
                        }
                    }
                    break;
            }
            break;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

void FRM_SubDlg::ChangItemStyle(long nstyle) {
	CWnd *pWnd;
  	long  style;

	pWnd=m_PropDlg->m_DlgWin->m_mdialog->GetDlgItem(m_PropDlg->m_DlgWin->m_ListCur->Wid);
	style=GetWindowLong(pWnd->m_hWnd,GWL_STYLE);
	style^=nstyle;
	SetWindowLong(pWnd->m_hWnd,GWL_STYLE,style);
}

void FRM_SubDlg::DoDataExchange(CDataExchange *pDX) {
    CDialog::DoDataExchange(pDX);
}

// =================================================================================================================================================
// ==================================================== FRM_PROPDLG CLASS FUNCTIONS ===============================================================
// =================================================================================================================================================

BEGIN_MESSAGE_MAP(FRM_PropDlg,CDialog)
//    ON_WM_ACTIVATE()
    ON_WM_CLOSE()
    ON_WM_PAINT()
    ON_WM_CREATE()
//    ON_BN_CLICKED(EXP_PROPTACK,OnTack)
END_MESSAGE_MAP()

int FRM_PropDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	CPaintDC dc(this);
    RECT     rect,
             crect;
	CString  String;
	SIZE	 size;
	short    index,
             entcnt;

    m_SubDlg.m_PropDlg =this;
    m_SubDlg.m_DlgIndex =0;
    m_SubDlg.m_Inuse    =FALSE;
    m_DlgIndex          =0;

    for(entcnt=0;entcnt<FRM_type_cnt;++entcnt) {
        GetClientRect(&rect);
        rect.bottom=rect.top+24; rect.top+=4; rect.left+=51; rect.right=rect.left;
        for(index=0;index<FRM_tabinfo[entcnt].TabCnt;++index) {
            String=FRM_tabinfo[entcnt].Tab[index];
		    GetTextExtentPoint32(dc.m_ps.hdc,FRM_tabinfo[entcnt].Tab[index],String.GetLength(),&size);
			FRM_tabinfo[entcnt].TabTlen[index]=size.cx;
	        rect.right+=size.cx;
            FRM_tabinfo[entcnt].Tabrect[index].top   =rect.top;
            FRM_tabinfo[entcnt].Tabrect[index].bottom=rect.bottom;
            FRM_tabinfo[entcnt].Tabrect[index].left  =rect.left;
            FRM_tabinfo[entcnt].Tabrect[index].right =rect.right;
		    rect.left   =rect.right+2;
        }
    }
    FRM_typeindex=m_DlgWin->GetWindowType();
    m_SubDlg.Create(FRM_tabinfo[FRM_typeindex].TabID[m_DlgIndex],this);
    m_SubDlg.GetClientRect(&rect);
    rect.bottom+=35; rect.top+=35; rect.left+=11;
    GetClientRect(&crect);
    crect.bottom=(rect.bottom+37);
    crect.right =(rect.right+17);
    MoveWindow(&crect);
    m_SubDlg.MoveWindow(&rect);
    m_SubDlg.ShowWindow(SW_SHOWNORMAL);

    return(CDialog::OnCreate(lpCreateStruct));
}

void FRM_PropDlg::OnPaint() {
	CPaintDC dc(this);
    RECT     rect;
	CString  String;
	short    index;
    short    offset;

    dc.SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
	dc.SetBkMode(TRANSPARENT);

    GetClientRect(&rect);
    rect.left   +=5;
    rect.top    +=25;
    rect.right  -=5;
    rect.bottom -=5;
  	ICAD_BorderUp(rect,dc.m_ps.hdc);

	for(index=0;index<FRM_tabinfo[FRM_typeindex].TabCnt;++index) {
        if (index!=m_DlgIndex) {
    		ICAD_EraseBkGround(FRM_tabinfo[FRM_typeindex].Tabrect[index],dc.m_ps.hdc);
    	    ICAD_TAB(FRM_tabinfo[FRM_typeindex].Tabrect[index],dc.m_ps.hdc);
            String=FRM_tabinfo[FRM_typeindex].Tab[index];
		    offset=((FRM_tabinfo[FRM_typeindex].Tabrect[index].right-
					 FRM_tabinfo[FRM_typeindex].Tabrect[index].left)-FRM_tabinfo[FRM_typeindex].TabTlen[index])/2;
    		dc.TextOut((FRM_tabinfo[FRM_typeindex].Tabrect[index].left+offset),
                       (FRM_tabinfo[FRM_typeindex].Tabrect[index].top+4),String);
        }
	}

    rect.top    =FRM_tabinfo[FRM_typeindex].Tabrect[m_DlgIndex].top-2;
    rect.bottom =FRM_tabinfo[FRM_typeindex].Tabrect[m_DlgIndex].bottom+2;
    rect.left   =FRM_tabinfo[FRM_typeindex].Tabrect[m_DlgIndex].left;
	rect.right  =FRM_tabinfo[FRM_typeindex].Tabrect[m_DlgIndex].right+2;

    String=FRM_tabinfo[FRM_typeindex].Tab[m_DlgIndex];
    ICAD_EraseBkGround(rect,dc.m_ps.hdc);
    ICAD_TAB(rect,dc.m_ps.hdc);
    offset=((rect.right-rect.left)-FRM_tabinfo[FRM_typeindex].TabTlen[m_DlgIndex])/2;
    dc.TextOut((rect.left+offset),(rect.top+4),String);
}

void FRM_PropDlg::OnClose() {
    m_SubDlg.DestroyWindow();
    CDialog::OnClose();
}

LRESULT FRM_PropDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) {
	RECT  rect,
          crect;
    short index;

	switch(message) {
        case WM_LBUTTONDOWN:
        	// EBATECH(CNBR) 2001-06-24 unsigned short -> long
            rect.right =(long)(short)LOWORD(lParam);
            rect.bottom=(long)(short)HIWORD(lParam);
            for(index=0;index<FRM_tabinfo[FRM_typeindex].TabCnt;++index) {
                if (rect.right<FRM_tabinfo[FRM_typeindex].Tabrect[index].right && rect.right>FRM_tabinfo[FRM_typeindex].Tabrect[index].left &&
                    rect.bottom>FRM_tabinfo[FRM_typeindex].Tabrect[index].top && rect.bottom<FRM_tabinfo[FRM_typeindex].Tabrect[index].bottom) {
                    m_SubDlg.m_DlgIndex=m_DlgIndex=index;
                    m_SubDlg.DestroyWindow();
                    m_SubDlg.Create(FRM_tabinfo[FRM_typeindex].TabID[index],this);

                    m_SubDlg.GetClientRect(&rect);
                    rect.bottom+=35; rect.top+=35; rect.left+=11;
            		GetWindowRect(&crect);
                    crect.bottom=crect.top+(rect.bottom+37);
                    crect.right =crect.left+(rect.right+17);

                    MoveWindow(&crect);
                    RedrawWindow();
                    m_SubDlg.MoveWindow(&rect);
                    m_SubDlg.ShowWindow(SW_SHOWNORMAL);
                    break;
                }
            }
            break;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

BOOL FRM_PropDlg::OnInitDialog() {
    long nID;
    CWnd *hCurItem;

    for(hCurItem=this->GetWindow(GW_CHILD);hCurItem;hCurItem=hCurItem->GetNextWindow(GW_HWNDNEXT)) {
        nID=GetWindowLong(hCurItem->m_hWnd,GWL_ID);
		switch(nID) {
			case EXP_PROPTACK:
				ICAD_OrigWndProcT=(WNDPROC)SetWindowLong(hCurItem->m_hWnd,GWL_WNDPROC,(LONG)ICAD_Tbutton);
				break;
			case EXP_PROPHELP:
				ICAD_OrigWndProcH=(WNDPROC)SetWindowLong(hCurItem->m_hWnd,GWL_WNDPROC,(LONG)ICAD_Hbutton);
				break;
        }
    }
    return TRUE;
}

// ===========================================================================================================================================
// ====================================================== FRM_MAINDLG CLASS FUNCTIONS ======================================================
// ===========================================================================================================================================

BEGIN_MESSAGE_MAP(FRM_MainDlg,CDialog)
    ON_WM_PAINT()
END_MESSAGE_MAP()

void FRM_MainDlg::OnPaint() {
//    short    fi1;

    CDialog::OnPaint();

//    if (!m_DlgWin->m_FrmCur || !m_DlgWin->m_ListCur || !m_DlgWin->m_selcntr) return;

//    for(fi1=0;fi1<m_DlgWin->m_selcntr;++fi1) m_DlgWin->FRM_SelectItem(m_DlgWin->m_SelList[fi1]->rect,0);
//    m_DlgWin->FRM_SelectItem(m_DlgWin->m_ListCur->rect,1);
}

// ===========================================================================================================================================
// ====================================================== FRM_VIEWEDIT CLASS FUNCTIONS ======================================================
// ===========================================================================================================================================

IMPLEMENT_DYNCREATE(FRM_ViewEdit,CFrameWnd)

BEGIN_MESSAGE_MAP(FRM_ViewEdit,CFrameWnd)
    ON_COMMAND(FRM_PROPERTIES,PropertyDlg)
    ON_WM_CONTEXTMENU()
    ON_WM_ENABLE()
    ON_WM_SETCURSOR()
    ON_WM_CAPTURECHANGED()
    ON_WM_LBUTTONUP()
    ON_WM_LBUTTONDOWN()
    ON_WM_RBUTTONDOWN()
    ON_WM_MOUSEMOVE()
    ON_WM_CLOSE()
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////
// This function opens up a dialog that is realted to the TYPE of ITEM
// selected.
//
// Nothing is returned if it faild.
//
void FRM_ViewEdit::PropertyDlg() {
    RECT           rect;
    long           xOff,
                   yOff;

//  if (m_DlgState) return;
//	m_DlgState=TRUE;
    m_propdlg = new FRM_PropDlg();
	m_propdlg->m_DlgWin=this;
    if (!m_propdlg->Create(EXP_PROPDLG,this)) return;
    m_propdlg->SetWindowText(FRM_tabinfo[FRM_typeindex].Name);
    m_propdlg->GetWindowRect(&rect);

    xOff=m_lpoint.x-rect.left;
    yOff=m_lpoint.y-rect.top;
    rect.left   +=xOff;
    rect.right  +=xOff;
    rect.top    +=yOff;
    rect.bottom +=yOff;

    m_propdlg->MoveWindow(&rect);
    m_propdlg->ShowWindow(SW_SHOW);
    m_propdlg->UpdateWindow();
}

///////////////////////////////////////////////////////////////////////
// This function is called when a right click on the splitter window
// has been done, this brings up a menu to select from.
//
// Nothing is returned
//
void FRM_ViewEdit::OnContextMenu(CWnd* pWnd, CPoint pos) {
    CMenu menubar;

    m_lpoint=pos;

	if (menubar.LoadMenu(FORM_MNU)) {
		CMenu* pPopup = menubar.GetSubMenu(0);
    	pPopup->TrackPopupMenu(TPM_RIGHTBUTTON,pos.x,pos.y,this);
    }
	CFrameWnd::OnContextMenu(pWnd,pos);
}

void FRM_ViewEdit::OnNewFont() {
    int           cy;
	LPCHOOSEFONT  FontSpecs;
	CDC          *cdc;

    if (!m_FrmCur) return;

	CFont* pFont = new CFont();
    cdc=m_mdialog->GetDC();

	FontSpecs= new CHOOSEFONT;
	memset(FontSpecs,0,sizeof(CHOOSEFONT));	
	FontSpecs->lpLogFont= new LOGFONT;
	memset(FontSpecs->lpLogFont,0,sizeof(LOGFONT));
	FontSpecs->lStructSize=sizeof(CHOOSEFONT);
	FontSpecs->hwndOwner=this->m_hWnd;
	FontSpecs->Flags=CF_SCREENFONTS|CF_NOSCRIPTSEL|CF_TTONLY|CF_USESTYLE|CF_INITTOLOGFONTSTRUCT;
    strcpy(FontSpecs->lpLogFont->lfFaceName,m_FrmCur->tfname);

    cy=cdc->GetDeviceCaps(LOGPIXELSY);
    FontSpecs->lpLogFont->lfHeight=(m_FrmCur->pointsize*cy)/72;

	if (ChooseFont(FontSpecs)) {
        if (m_FrmCur->tfname) IC_FREE(m_FrmCur->tfname);        
		m_FrmCur->tfname= new char [strlen(FontSpecs->lpLogFont->lfFaceName)+1];		
        strcpy(m_FrmCur->tfname,FontSpecs->lpLogFont->lfFaceName);
        m_FrmCur->pointsize=FontSpecs->iPointSize/10;
        pFont->CreateFontIndirect(FontSpecs->lpLogFont);
        m_mdialog->SetFont(pFont);
    }
    m_mdialog->ReleaseDC(cdc);
    delete (FontSpecs->lpLogFont);
    delete FontSpecs;
	delete pFont;

}

void FRM_ViewEdit::SaveDlgChanges() {
    if (m_mdialog->m_hWnd) MakePicture(m_mdialog);
    if (m_FrmCur) {
        if (m_FrmCur->bptr) IC_FREE(m_FrmCur->bptr);
        m_FrmCur->bptr=(BYTE *)MakeTemplate(this,&m_FrmCur->fieldlen);
    }
    for(m_FrmCur=m_FrmBeg;m_FrmCur;m_FrmCur=m_FrmCur->next) {
        sprintf(FRM_szText,ResourceString(IDC_ICADFORM_SAVE_CHANGES_T_18, "Save changes to %s" ),m_FrmCur->frmname);
        if ((MessageBox(FRM_szText,ResourceString(IDC_ICADFORM_FORM_DESIGNER_19, "Form Designer" ),MB_ICONQUESTION|MB_YESNO|MB_APPLMODAL))==IDYES) {
            FRM_FormSave(m_FrmCur,FALSE);
        }
    }
    if (m_mdialog->m_hWnd) m_mdialog->DestroyWindow();
    delete m_mdialog;

    FRM_FreeDlgLinks();
    FRM_FreeSelList();
	delete m_lcpoint;
}

void FRM_ViewEdit::OnClose() {
    CFrameWnd::OnClose();
    if (m_formedit->m_Formname) IC_FREE(m_formedit->m_Formname);

	::EnableWindow(IcadSharedGlobals::GetMainHWND(),TRUE);
}

BOOL FRM_ViewEdit::OnCreateClient(LPCREATESTRUCT lpcs,CCreateContext* pContext) {
    CRect          rectScrn;
    long          *longptr;
    RECT           rect;
    CWnd          *hCurItem,
                  *hChildItem;
    CString        dbstr;
    CFont         *fontptr;
    COleVariant    oleint;
    LOGFONT        logfont;
    short          fi1;
	CDC           *cdc;
    int            cy;
    div_t          div_result;

    m_SingleGrp  =FALSE;
    m_CtrlFlag   =FALSE;
    m_MoveFlag   =FALSE;
    m_IDCntr     =100;
    m_pickplace  =0;
	m_ListCur    =NULL;
    m_FrmBeg     =NULL;
    m_FrmCur     =NULL;
	m_lcpoint	 =NULL;
    m_selcntr    =0;
    m_idcCursor  =5;
    m_boxstate   =0;
    m_Controltype=FRM_FORM_ARROW;
    m_nChar      =0;
    m_lpoint.x   =0;
    m_lpoint.y   =0;

    if(!SDS_CMainWindow->IcadStartDao()) return(FALSE);

    if ((sds_findfile(SDS_DBFILE,FRM_szText))!=SDS_RTNORM) {
        dbstr.Format(ResourceString(IDC_ICADFORM_UNABLE_TO_LOCA_20, "Unable to locate the Database %s" ),FRM_szText);
        sds_alert(dbstr);
		return FALSE;
	}
    if (FILE_ATTRIBUTE_READONLY&(GetFileAttributes(FRM_szText))) {
        SDS_CMainWindow->m_readwrite=TRUE;
        dbstr.Format(ResourceString(IDC_ICADFORM_THE_DATABASE___21, "The Database %s is read only.\n You will not be able to save changes." ),FRM_szText);
        sds_alert(dbstr);
    } else SDS_CMainWindow->m_readwrite=FALSE;

    if (!SDS_CMainWindow->m_CdatarecordPtr->IsOpen()) {
	    try	{ SDS_CMainWindow->m_CdatabasePtr.Open(FRM_szText,FALSE,SDS_CMainWindow->m_readwrite); }
	    catch (CDaoException* e) {
            e->Delete();
		    return(FALSE);
        }
    }
    SDS_CMainWindow->m_CdatarecordPtr = new CDaoRecordset(&SDS_CMainWindow->m_CdatabasePtr);

	::EnableWindow(IcadSharedGlobals::GetMainHWND(),FALSE);

    m_formedit=(FRM_FormEdit *)FRM_Parent;
    if (m_formedit->m_Formname) {
        m_Formname=m_formedit->m_Formname;

        m_mdialog = new FRM_MainDlg();
        m_mdialog->m_DlgWin=this;

        if (!m_Formname.IsEmpty()) {
            if (!FRM_NewTableQuery(ResourceString(IDC_ICADFORM_SELECT___FROM__22, "Select * from Forminfo" ))) return FALSE;
            FRM_AddDlgHead();

            dbstr.Format(ResourceString(IDC_ICADFORM_FORMNAME___S__23, "FormName='%s'" ),m_Formname);
            SDS_CMainWindow->m_CdatarecordPtr->Find(AFX_DAO_FIRST,dbstr);

            m_FrmCur->frmname= new char [m_Formname.GetLength()+1];
			strcpy(m_FrmCur->frmname,(LPCTSTR)m_Formname);
            VarToBinay(&m_FrmCur->bptr,&SDS_CMainWindow->m_CdatarecordPtr->GetFieldValue(ResourceString(IDC_ICADFORM_FORMDEFINITION_24, "FormDefinition" )));
            oleint=SDS_CMainWindow->m_CdatarecordPtr->GetFieldValue(ResourceString(IDC_ICADFORM_FORMSIZE_25, "FormSize" ));
            m_FrmCur->fieldlen=oleint.lVal;
            oleint=SDS_CMainWindow->m_CdatarecordPtr->GetFieldValue(ResourceString(IDC_ICADFORM_FKEY_16, "Fkey" ));
            m_FrmCur->fkey=oleint.lVal;

            longptr=(long *)m_FrmCur->bptr;
            *longptr=WS_DISABLED|WS_CHILD|WS_CLIPSIBLINGS|WS_BORDER|WS_DLGFRAME|0x000000C4;

            if (!m_mdialog->CreateIndirect(m_FrmCur->bptr,this)) return 0;
            m_mdialog->ShowWindow(SW_SHOWNORMAL);

            m_mdialog->GetClientRect(&rect);
            rect.left+=6; rect.top+=6; rect.right+=6; rect.bottom+=6;
            m_mdialog->MoveWindow(&rect,FALSE);
            m_mdialog->EnableWindow(FALSE);

            if ((hCurItem=this->GetWindow(GW_CHILD))==NULL) return 0;

            FRM_AddHead(hCurItem);

            FRM_PhWnd=hCurItem->m_hWnd;
            if ((hChildItem=hCurItem->GetWindow(GW_CHILD))==NULL) return 0;
            do {
                ++m_IDCntr;
                FRM_AddHead(hChildItem);
            } while((hChildItem=hChildItem->GetNextWindow(GW_HWNDNEXT))!=NULL);

            // Get the font name and point size of the dialog.
            fontptr=m_mdialog->GetFont();
            fontptr->GetLogFont(&logfont);
            cdc=m_mdialog->GetDC();
            // convert the point size to logical pixels using the following code (iPointSize is assumed to be in whole units, not tenths).
            cy=cdc->GetDeviceCaps(LOGPIXELSY);

            div_result = div(abs(logfont.lfHeight*72),cy);
            m_FrmCur->pointsize=div_result.quot+((div_result.rem>=50)?1:0);

            m_mdialog->ReleaseDC(cdc);            
			m_FrmCur->tfname= new char [strlen(logfont.lfFaceName)+1];			
            strcpy(m_FrmCur->tfname,logfont.lfFaceName);

            m_mdialog->GetWindowRect(&rectScrn);
            m_mdialog->ClientToScreen(&rect);
            m_TitleHeight=rect.top-rectScrn.top;
        }
    } else {
        if (!FRM_NewTableQuery(ResourceString(IDC_ICADFORM_SELECT___FROM__22, "Select * from Forminfo" ))) return FALSE;
        for(fi1=0;!SDS_CMainWindow->m_CdatarecordPtr->IsEOF();++fi1) SDS_CMainWindow->m_CdatarecordPtr->MoveNext();
        sprintf(FRM_szText,"%s%d"/*DNT*/,FRM_NEW_FORM,(fi1+1));        
		m_formedit->m_Formname= new char [strlen(FRM_szText)+1];		
        strcpy(m_formedit->m_Formname,FRM_szText);
        m_Formname=m_formedit->m_Formname;

        m_mdialog = new FRM_MainDlg();
        m_mdialog->m_DlgWin=this;

        // Call on new dialog.
        OnNewDlg();

        fontptr=m_mdialog->GetFont();
        fontptr->GetLogFont(&logfont);

        // If lfHeight is greater than zero, it is transformed into device units and matched against the cell height
        // of the available fonts. If it is zero, a reasonable default size is used. If it is less than zero, it is
        // transformed into device units and the absolute value is matched against the character height of the available fonts.
        if (!logfont.lfHeight) logfont.lfHeight=16; else if (logfont.lfHeight<0) logfont.lfHeight=abs(logfont.lfHeight);

        m_FrmCur->tfname= new char [strlen(logfont.lfFaceName)+1];
        strcpy(m_FrmCur->tfname,logfont.lfFaceName);
        m_FrmCur->pointsize=(short)logfont.lfHeight;
    }
    return TRUE;
}

void FRM_ViewEdit::OnTestDlg() {
    long      *longptr;
    CDialog   *formdlg;
    formdlg = new CDialog();

    if (!m_FrmCur) return;

    if (m_FrmCur->bptr) IC_FREE(m_FrmCur->bptr);
    m_FrmCur->bptr=(BYTE *)MakeTemplate(this,&m_FrmCur->fieldlen);

    longptr=(long *)(m_FrmCur->bptr);
    *longptr=WS_POPUP|WS_VISIBLE|WS_CLIPSIBLINGS|WS_BORDER|WS_DLGFRAME|0x000000C4;

    formdlg->InitModalIndirect((HGLOBAL)m_FrmCur->bptr,this);
    formdlg->DoModal();

    delete formdlg;
}

long FRM_ViewEdit::FRM_Makebitmap(HBITMAP hBMP,HDC hDC,BYTE **hp) {
    BITMAP            bmp;
    PBITMAPINFO       pbmi;
    WORD              cClrBits;

    BITMAPFILEHEADER  hdr;
    PBITMAPINFOHEADER pbih;
    LPBYTE            lpBits;
    DWORD             size;

    if (!GetObject(hBMP,sizeof(BITMAP),(LPSTR)&bmp)) return NULL;

    cClrBits=(WORD)(bmp.bmPlanes*bmp.bmBitsPixel);
    if (cClrBits==1)       cClrBits=1;
    else if (cClrBits<=4)  cClrBits=4;
    else if (cClrBits<=8)  cClrBits=8;
    else if (cClrBits<=16) cClrBits=16;
    else if (cClrBits<=24) cClrBits=24;
    else cClrBits=32;

    if (cClrBits!=24) {
        pbmi=(PBITMAPINFO) new BYTE [sizeof(BITMAPINFOHEADER)+(sizeof(RGBQUAD)*(1<<cClrBits))];
		memset(pbmi,0,sizeof(BITMAPINFOHEADER)+(sizeof(RGBQUAD)*(1<<cClrBits));
    } else {        
		pbmi=(PBITMAPINFO) new BYTE [sizeof(BITMAPINFOHEADER)];
		memset(pbmi,0,sizeof(BITMAPINFOHEADER));
    }

    pbmi->bmiHeader.biSize         =sizeof(BITMAPINFOHEADER);
    pbmi->bmiHeader.biWidth        =bmp.bmWidth;
    pbmi->bmiHeader.biHeight       =bmp.bmHeight;
    pbmi->bmiHeader.biPlanes       =1;//bmp.bmPlanes;
    pbmi->bmiHeader.biBitCount     =bmp.bmBitsPixel;
    pbmi->bmiHeader.biXPelsPerMeter=0;
    pbmi->bmiHeader.biYPelsPerMeter=0;
    if (cClrBits<24) pbmi->bmiHeader.biClrUsed=2^cClrBits;

    pbmi->bmiHeader.biCompression =BI_RGB;
    pbmi->bmiHeader.biSizeImage   =(pbmi->bmiHeader.biWidth+7)/8*pbmi->bmiHeader.biHeight*cClrBits;
    pbmi->bmiHeader.biClrImportant=0;

    pbih=(PBITMAPINFOHEADER)pbmi;

    if (!(lpBits=(LPBYTE)GlobalAlloc(GMEM_FIXED,pbih->biSizeImage))) return NULL;
    if (!GetDIBits(hDC,hBMP,0,(WORD)pbih->biHeight,lpBits,(PBITMAPINFO)pbmi,DIB_RGB_COLORS)) return NULL;

    hdr.bfType     =0x4d42;
    hdr.bfSize     =(DWORD)(sizeof(BITMAPFILEHEADER)+pbih->biSize+pbih->biClrUsed*sizeof(RGBQUAD)+pbih->biSizeImage);
    hdr.bfOffBits  =(DWORD)sizeof(BITMAPFILEHEADER)+pbih->biSize+pbih->biClrUsed*sizeof(RGBQUAD);
    hdr.bfReserved1=0;
    hdr.bfReserved2=0;

    size=hdr.bfOffBits+pbih->biSizeImage;
    (*hp)=(BYTE *)GlobalAlloc(GMEM_FIXED,size);

    memcpy((*hp),&hdr,sizeof(BITMAPFILEHEADER));
    memcpy((*hp)+sizeof(BITMAPFILEHEADER),pbih,(sizeof(BITMAPINFOHEADER)+pbih->biClrUsed*sizeof(RGBQUAD)));
    memcpy((*hp)+sizeof(BITMAPFILEHEADER)+(sizeof(BITMAPINFOHEADER)+pbih->biClrUsed*sizeof(RGBQUAD)),lpBits,pbih->biSizeImage);

    delete pbmi;
    GlobalFree(lpBits);
    return (size);
}

void FRM_Showbitmap(BYTE *pbytes,CWnd *pWin) {
    BITMAPFILEHEADER fHdr;          // BitmapFile header
    BITMAPINFOHEADER bmiHdr;        // BitmapInfo header
    BITMAPINFO      *lpbits;
    char            *lpDIBBits;
    HDC              hdcMem,
                     hdc;
    HBITMAP          hbmMem,
                     hBitmap,
                     hbmMono;
	CDC             *cdc;
	CRect            rect;

    memcpy(&fHdr,pbytes,sizeof(BITMAPFILEHEADER));

    if (fHdr.bfType!=0x4d42) return;

    memcpy(&bmiHdr,pbytes+sizeof(BITMAPFILEHEADER),sizeof(BITMAPINFOHEADER));

    lpbits=(BITMAPINFO *)(pbytes+sizeof(BITMAPFILEHEADER));

    if ((lpDIBBits= new char [ bmiHdr.biSizeImage])==0) return;

	memset(lpDIBBits,0,bmiHdr.biSizeImage);

    pbytes=pbytes+(sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+(lpbits->bmiHeader.biClrUsed*sizeof(RGBQUAD)));
    memcpy(lpDIBBits,pbytes,bmiHdr.biSizeImage);

    cdc=pWin->GetDC();
	pWin->GetClientRect(&rect);
	hdc=cdc->m_hDC;
    hdcMem=CreateCompatibleDC(hdc);
    hBitmap=CreateDIBitmap(hdc,&bmiHdr,0L,NULL,NULL,0);

	SelectObject(hdcMem,hBitmap);

	// Create a temporary bitmap.
	hbmMono=CreateBitmap(1,1,1,1,NULL);
    hbmMem=(HBITMAP)SelectObject(hdcMem,hbmMono);

	SetDIBits(hdcMem,hbmMem,0,bmiHdr.biHeight,lpDIBBits,lpbits,DIB_RGB_COLORS);
	SelectObject(hdcMem,hbmMem);
//    rect.left+=5; rect.top+=5;
    BitBlt(hdc,rect.left,rect.top,bmiHdr.biWidth,bmiHdr.biHeight,hdcMem,0,0,SRCCOPY);
//    StretchBlt(hdc,rect.left,rect.top,rect.right,rect.bottom,hdcMem,0,0,bmiHdr.biWidth,bmiHdr.biHeight,SRCCOPY);

	DeleteDC(hdcMem);
    DeleteObject(hbmMono);
    DeleteObject(hbmMem);
    pWin->ReleaseDC(cdc);

    if(lpDIBBits!=NULL) delete lpDIBBits;
}

void FRM_ViewEdit::FRM_FormSave(struct FRM_frmList *form,BOOL type) {
    HBITMAP     hBitmap,
                hpBitmap;
    HDC         hdc;
	long	    li1,
                longptr,
                len;
    CString     namestring;
    CByteArray  pArray,
                picArray;
    COleVariant olefkey;
    RECT        rect;
	BYTE       *picbyte;
    CStatic    *picture;
    float       p1,
                p0;

    longptr=(long)*(form->bptr);
    longptr=WS_POPUP|WS_VISIBLE|WS_CLIPSIBLINGS|WS_BORDER|WS_DLGFRAME|0x000000C4;
    *(form->bptr)=(BYTE)longptr;

    pArray.SetSize(form->fieldlen);
    for(li1=0;li1<form->fieldlen;++li1) pArray[li1]=((BYTE)form->bptr[li1]);
    COleVariant  datavalue=pArray;
    COleVariant  datalen(m_FrmCur->fieldlen,VT_I4);

    picture=(CStatic *)m_formedit->m_Parent->GetDlgItem(CUS_FORM_PREVIEW);
    picture->GetClientRect(&rect);
    hdc=CreateCompatibleDC(m_FrmCur->hdcCompat);
//    rect.bottom-=10; rect.right-=10;
    p1=(float)m_FrmCur->rect.right/(float)m_FrmCur->rect.bottom;
    p0=(float)rect.right/(float)rect.bottom;
    if (p1>=p0) {
        rect.bottom=(long)((float)rect.right/p1);
    } else if (p0>=p1) {
        rect.right=(long)((float)rect.bottom*p1);
    }
  	hpBitmap=CreateBitmap(rect.right,rect.bottom,1,8,NULL);
    SelectObject(hdc,hpBitmap);
    len=StretchBlt(hdc,rect.left,rect.top,rect.right,rect.bottom,m_FrmCur->hdcCompat,0,0,m_FrmCur->rect.right,m_FrmCur->rect.bottom,SRCCOPY);
    if ((hpBitmap=CreateCompatibleBitmap(hdc,0,0))!=NULL) {
         hBitmap=(HBITMAP)SelectObject(hdc,hpBitmap);
         len=FRM_Makebitmap(hBitmap,hdc,&picbyte);
         DeleteObject(hBitmap);
         DeleteObject(hpBitmap);
         DeleteDC(hdc);
         picArray.SetSize(len);
         for(li1=0;li1<len;++li1) picArray[li1]=((BYTE)picbyte[li1]);
    }

    COleVariant  picvalue=picArray;
    GlobalFree(picbyte);
    if (!FRM_NewTableQuery(ResourceString(IDC_ICADFORM_SELECT___FROM__22, "Select * from Forminfo" ))) return;
    if (type) {
        COleVariant  formname(form->frmname,VT_BSTRT);

        SDS_CMainWindow->m_CdatarecordPtr->AddNew();
	    SDS_CMainWindow->m_CdatarecordPtr->SetFieldValue(ResourceString(IDC_ICADFORM_FORMNAME_14, "FormName" ),formname);
	    SDS_CMainWindow->m_CdatarecordPtr->SetFieldValue(ResourceString(IDC_ICADFORM_FORMSIZE_25, "FormSize" ),datalen);
	    SDS_CMainWindow->m_CdatarecordPtr->SetFieldValue(ResourceString(IDC_ICADFORM_FORMDEFINITION_24, "FormDefinition" ),datavalue);
	    SDS_CMainWindow->m_CdatarecordPtr->Update();
        SDS_CMainWindow->m_CdatarecordPtr->MoveLast();
        olefkey=SDS_CMainWindow->m_CdatarecordPtr->GetFieldValue(ResourceString(IDC_ICADFORM_FKEY_16, "Fkey" ));
        m_FrmCur->fkey=olefkey.lVal;
    } else {
        namestring.Format(ResourceString(IDC_ICADFORM_FKEY__D_26, "Fkey=%d" ),form->fkey);
        COleVariant  formname(form->frmname,VT_BSTRT);
        if (SDS_CMainWindow->m_CdatarecordPtr->Find(AFX_DAO_FIRST,namestring)) {
	        SDS_CMainWindow->m_CdatarecordPtr->Edit();
	        SDS_CMainWindow->m_CdatarecordPtr->SetFieldValue(ResourceString(IDC_ICADFORM_FORMNAME_14, "FormName" ),formname);
	        SDS_CMainWindow->m_CdatarecordPtr->SetFieldValue(ResourceString(IDC_ICADFORM_FORMSIZE_25, "FormSize" ),datalen);
	        SDS_CMainWindow->m_CdatarecordPtr->SetFieldValue(ResourceString(IDC_ICADFORM_FORMDEFINITION_24, "FormDefinition" ),datavalue);
	        SDS_CMainWindow->m_CdatarecordPtr->SetFieldValue(ResourceString(IDC_ICADFORM_FORMPIC_27, "FormPic" ),picvalue);
            SDS_CMainWindow->m_CdatarecordPtr->Update();
        }
    }

    pArray.RemoveAll();
    picArray.RemoveAll();
}

int FRM_ViewEdit::Memtotal(int size,CWnd *Item) {
	long totalmem;
	totalmem=(size);											 // This is the size of the structure.
    totalmem+=(2*(GetClassName(Item->m_hWnd,FRM_szText,255)+1)); // This is for the class name.
	totalmem+=(2*(Item->GetWindowTextLength()+1));				 // This is for the window title.
    return(totalmem);
}

void FRM_ViewEdit::ChangeDlg() {
    RECT         rect;
    CWnd        *hCurItem,
                *hChildItem;
    long        *longptr;
    CString      dbstr;
    COleVariant  oleint;

    if (!m_Formname.IsEmpty()) {
        if (m_FrmCur) {
            if (m_FrmCur->bptr) IC_FREE(m_FrmCur->bptr);
            m_FrmCur->bptr=(BYTE *)MakeTemplate(this,&m_FrmCur->fieldlen);
        }
        if (m_mdialog->m_hWnd) m_mdialog->DestroyWindow();

        FRM_AddDlgHead();

        dbstr.Format(ResourceString(IDC_ICADFORM_FORMNAME___S__23, "FormName='%s'" ),m_Formname);
        SDS_CMainWindow->m_CdatarecordPtr->Find(AFX_DAO_FIRST,dbstr);

        m_FrmCur->frmname= new char [m_Formname.GetLength()+1];		
        strcpy(m_FrmCur->frmname,(LPCTSTR)m_Formname);
        VarToBinay(&m_FrmCur->bptr,&SDS_CMainWindow->m_CdatarecordPtr->GetFieldValue(ResourceString(IDC_ICADFORM_FORMDEFINITION_24, "FormDefinition" )));
        oleint=SDS_CMainWindow->m_CdatarecordPtr->GetFieldValue(ResourceString(IDC_ICADFORM_FORMSIZE_25, "FormSize" ));
        m_FrmCur->fieldlen=oleint.lVal;
        oleint=SDS_CMainWindow->m_CdatarecordPtr->GetFieldValue(ResourceString(IDC_ICADFORM_FKEY_16, "Fkey" ));
        m_FrmCur->fkey=oleint.lVal;

        longptr=(long *)m_FrmCur->bptr;
        *longptr=WS_DISABLED|WS_CHILD|WS_CLIPSIBLINGS|WS_BORDER|WS_DLGFRAME|0x000000C4;

        m_mdialog->CreateIndirect(m_FrmCur->bptr,this);
        m_mdialog->ShowWindow(SW_SHOWNORMAL);

        m_mdialog->GetClientRect(&rect);
        rect.left+=6; rect.top+=6; rect.right+=6; rect.bottom+=6;
        m_mdialog->MoveWindow(&rect,FALSE);
        m_mdialog->EnableWindow(FALSE);

        if ((hCurItem=this->GetWindow(GW_CHILD))==NULL) return;

        FRM_AddHead(hCurItem);

        FRM_PhWnd=hCurItem->m_hWnd;
        if ((hChildItem=hCurItem->GetWindow(GW_CHILD))==NULL) return;
        do {
            FRM_AddHead(hChildItem);
        } while((hChildItem=hChildItem->GetNextWindow(GW_HWNDNEXT))!=NULL);
    }
}

void FRM_ViewEdit::OnNewDlg() {
    RECT       rect;
    CWnd      *hCurItem,
              *hChildItem;
    CString    String;

    if (m_FrmCur) {
        if (m_FrmCur->bptr) IC_FREE(m_FrmCur->bptr);
        m_FrmCur->bptr=(BYTE *)MakeTemplate(this,&m_FrmCur->fieldlen);
    }
    if (m_mdialog->m_hWnd) m_mdialog->DestroyWindow();
    m_mdialog->Create(FRM_MDIALOG,this);

    m_IDCntr     =100;
    m_pickplace  =0;
	m_ListCur    =NULL;
    m_idcCursor  =5;
    m_boxstate   =0;
    m_Controltype=FRM_FORM_ARROW;

    m_mdialog->GetClientRect(&rect);
    rect.left+=6; rect.top+=6; rect.right+=6; rect.bottom+=6;
    m_mdialog->MoveWindow(&rect,FALSE);
    m_mdialog->EnableWindow(FALSE);

    if ((hCurItem=this->GetWindow(GW_CHILD))==NULL) return;

    FRM_AddDlgHead();
    FRM_AddHead(hCurItem);

    FRM_PhWnd=hCurItem->m_hWnd;
    if ((hChildItem=hCurItem->GetWindow(GW_CHILD))==NULL) return;
    do {
        ++m_IDCntr;
        FRM_AddHead(hChildItem);
    } while((hChildItem=hChildItem->GetNextWindow(GW_HWNDNEXT))!=NULL);

    m_FrmCur->frmname= new char [ m_Formname.GetLength()+1];
    strcpy(m_FrmCur->frmname,(LPCTSTR)m_Formname);

    m_FrmCur->bptr=(BYTE *)MakeTemplate(this,&m_FrmCur->fieldlen);
    FRM_FormSave(m_FrmCur,TRUE);
    m_Formname=m_FrmCur->frmname;
}

void *FRM_ViewEdit::MakePicture(CWnd *ptr) {
    HBITMAP  hBitmap;
	CDC     *cdc;

    if (!m_FrmCur) return(NULL);

    if (m_FrmCur->hdcCompat) DeleteDC(m_FrmCur->hdcCompat);

	ptr->GetClientRect(&m_FrmCur->rect);
	cdc=ptr->GetDC();

    m_FrmCur->hdcCompat=CreateCompatibleDC(cdc->m_hDC);
    if((hBitmap=CreateCompatibleBitmap(cdc->m_hDC,m_FrmCur->rect.right,m_FrmCur->rect.bottom))!=NULL) {
        SelectObject(m_FrmCur->hdcCompat,hBitmap);
        BitBlt(m_FrmCur->hdcCompat,0,0,m_FrmCur->rect.right,m_FrmCur->rect.bottom,cdc->m_hDC,m_FrmCur->rect.left,m_FrmCur->rect.top,SRCCOPY);

    }
    ptr->ReleaseDC(cdc);
    return(NULL);
}

void *FRM_ViewEdit::MakeTemplate(CWnd *ptr,long *memsize) {
    void      *CntlResult;
    char       string[256];
    CWnd      *hCurItem,
              *hPItem;
	long	   TotalMem;
    long       BaseValue,
               unitx,
               unity;
    CByteArray pArray;
	RECT       rect;
	long	   Style;
    CString    String;
    long       ItemID;
	void      *hGlbl;
	long       bytecount;

	TotalMem=Memtotal(sizeof(FRM_TEMPLATE),m_mdialog);
	++TotalMem;																			   // add two bytes for no menu.
	if (m_FrmCur->tfname && *m_FrmCur->tfname) TotalMem+=(2*(strlen(m_FrmCur->tfname)+1)); // This is for the fonts.
	TotalMem+=sizeof(short);

    if ((hCurItem=m_mdialog->GetWindow(GW_CHILD))==NULL) return NULL;
    do {
		TotalMem+=Memtotal(sizeof(FRM_ITEMTEMPLATE),hCurItem);
		TotalMem+=2; // This for the control data member.
		TotalMem+=2; // for the Dword alignment.
    } while((hCurItem=hCurItem->GetNextWindow(GW_HWNDNEXT))!=NULL);

    hPItem=m_mdialog;

	BaseValue=GetDialogBaseUnits();
	// EBATECH(CNBR) 2001-06-24 unsigned short -> long
	unitx=(long)(short)LOWORD(BaseValue);
	unity=(long)(short)HIWORD(BaseValue);

    hPItem->GetWindowRect(&rect);
    ptr->ScreenToClient(&rect);

    rect.right =rect.right-rect.left;
    rect.bottom=rect.bottom-rect.top;

    rect.left  =(rect.left   *4)/unitx;
    rect.top   =(rect.top    *8)/unity;
    rect.bottom=(rect.bottom *8)/unity;
    rect.right =(rect.right  *4)/unitx;

    Style=WS_CLIPSIBLINGS|WS_BORDER|WS_DLGFRAME|0x000000C4;
    hPItem->GetWindowText(String);

    strcpy(FRM_szText,(LPCSTR)String);

    hGlbl=(void *)dlg_CreateDlgTemplate(Style,rect,_T(ResourceString(IDC_ICADFORM__28, "" )),_T(ResourceString(IDC_ICADFORM__32770_0, "#32770" )),FRM_szText,m_FrmCur->pointsize,m_FrmCur->tfname,TotalMem,&bytecount);

    if ((hCurItem=hPItem->GetWindow(GW_CHILD))==NULL) return NULL;
    do {
//		TotalMem+=Memtotal(sizeof(FRM_ITEMTEMPLATE),hCurItem);
        hCurItem->GetWindowRect(&rect);
        hPItem->ScreenToClient(&rect);
        rect.right =rect.right-rect.left;
        rect.bottom=rect.bottom-rect.top;
        rect.left =(rect.left *4)/unitx;
        rect.top  =(rect.top *8)/unity;
        rect.bottom=(rect.bottom *8)/unity;
        rect.right=(rect.right *4)/unitx;

	    Style =GetWindowLong(hCurItem->m_hWnd,GWL_STYLE);
        ItemID=GetWindowLong(hCurItem->m_hWnd,GWL_ID);
        GetClassName(hCurItem->m_hWnd,FRM_szText,255);
        hCurItem->GetWindowText(String);

        strcpy(string,(LPCSTR)String);

        CntlResult=dlg_AddDlgControl(hGlbl,rect,(short)ItemID,Style,FRM_szText,ResourceString(IDC_ICADFORM__28, "" ),string,0,0,0,&bytecount);

        // Return false if we have an error.
        if (LOWORD(CntlResult)==FALSE) return NULL;

  		hGlbl=(void *) CntlResult;
    } while((hCurItem=hCurItem->GetNextWindow(GW_HWNDNEXT))!=NULL);
    *memsize=TotalMem;
    return(hGlbl);
}

void FRM_ViewEdit::OnEditDlg(LPCTSTR String) {
    RECT     rect;
    CWnd    *hCurItem,
            *hChildItem;
    CRect    Crect;
    CString  dbstr;
    BYTE    *bptr;

    COleVariant  varFieldValue1;

    while (!SDS_CMainWindow->m_CdatarecordPtr->IsBOF() && !SDS_CMainWindow->m_CdatarecordPtr->IsEOF()) {
        VarToCStr(&dbstr,&SDS_CMainWindow->m_CdatarecordPtr->GetFieldValue(ResourceString(IDC_ICADFORM_FORMNAME_14, "FormName" )));
        if (dbstr==String) break;
        SDS_CMainWindow->m_CdatarecordPtr->MoveNext();
    }
    VarToBinay(&bptr,&SDS_CMainWindow->m_CdatarecordPtr->GetFieldValue(ResourceString(IDC_ICADFORM_FORMDEFINITION_24, "FormDefinition" )));

    if (m_mdialog->m_hWnd) m_mdialog->DestroyWindow();
    m_mdialog->CreateIndirect(bptr,this);

    m_IDCntr     =100;
    m_pickplace  =0;
	m_ListCur    =NULL;
    m_idcCursor  =5;
    m_boxstate   =0;
    m_Controltype=FRM_FORM_ARROW;

    m_mdialog->GetClientRect(&rect);
    rect.left+=6; rect.top+=6; rect.right+=6; rect.bottom+=6;
    m_mdialog->MoveWindow(&rect,FALSE);
    m_mdialog->EnableWindow(FALSE);

    if ((hCurItem=this->GetWindow(GW_CHILD))==NULL) return;

    FRM_FreeAll();
    FRM_AddHead(hCurItem);

    FRM_PhWnd=hCurItem->m_hWnd;
    if ((hChildItem=hCurItem->GetWindow(GW_CHILD))==NULL) return;
    do {
        ++m_IDCntr;
        FRM_AddHead(hChildItem);
    } while((hChildItem=hChildItem->GetNextWindow(GW_HWNDNEXT))!=NULL);
}

void FRM_ViewEdit::OnSelectAll() {
	short  fi1;
    RECT   rect;
	CDC   *cdc;

	if (m_FrmCur->hdcCompat) {
		m_mdialog->GetWindowRect(&rect);
		cdc=m_mdialog->GetDC();
		BitBlt(cdc->m_hDC,0,0,rect.right,rect.bottom,m_FrmCur->hdcCompat,0,0,SRCCOPY);
        ReleaseDC(cdc);
	}
	FRM_FreeSelList();
    for(m_ListCur=m_FrmCur->ptr,fi1=0;fi1<(m_FrmCur->ItemCntr-1);m_ListCur=m_ListCur->next,++fi1) {
		FRM_AddSelHead();
		FRM_SelectItem(m_SelList[fi1]->rect,0);
	}
}

void FRM_ViewEdit::OnDeleteItem() {
    short  fi1;
    CWnd  *pWnd;

    if (!m_FrmCur || !m_ListCur || !m_selcntr) return;

    for(fi1=0;fi1<m_selcntr;++fi1) {
        pWnd=m_mdialog->GetDlgItem(m_SelList[fi1]->Wid);
        pWnd->DestroyWindow();
        m_ListCur=m_SelList[fi1];
        FRM_FreeItem();
    }
    FRM_FreeSelList();
    m_mdialog->RedrawWindow();
    MakePicture(m_mdialog);
}

void FRM_ViewEdit::OnMoveItems(UINT type) {
	short fi1;

    if (!m_FrmCur || !m_ListCur || !m_selcntr) return;

    for(fi1=0;fi1<m_selcntr;++fi1) {
		switch(type) {
			case VK_LEFT:
				--m_SelList[fi1]->rect.left;
				--m_SelList[fi1]->rect.right;
				break;
			case VK_UP:
				--m_SelList[fi1]->rect.top;
				--m_SelList[fi1]->rect.bottom;
				break;
			case VK_RIGHT:
				++m_SelList[fi1]->rect.left;
				++m_SelList[fi1]->rect.right;
				break;
			case VK_DOWN:
				++m_SelList[fi1]->rect.top;
				++m_SelList[fi1]->rect.bottom;
				break;
		}
        if (m_boxstate) FRM_MoveItem(m_SelList[fi1],(m_SelList[fi1]==m_ListEnd)?FALSE:TRUE);
	}
	m_mdialog->RedrawWindow();
	MakePicture(m_mdialog);
    for(fi1=0;fi1<m_selcntr;++fi1) if (m_SelList[fi1]!=m_ListCur) FRM_SelectItem(m_SelList[fi1]->rect,0);
    FRM_SelectItem(m_ListCur->rect,1);
}

///////////////////////////////////////////////////////////////////////
// This function copies whatever items that are selected to the clip
// board. The layout of the items copied to the clipboard are as follows
//
// The header of this record is ([A][B]) then is repeated from ([C] - to - [H]),
// this record structure is also a reference for the OnPaste() function also.
// [A][B][C][C][D][D][E][E][F][F][G][G][G....][H][H][H.....][C][C][D][D][E][E]..
//
// A = This is a short containing a flag for (cut) or (copy)
// B = This is a short containing the number of items copied to the clipboard.
// C = This is a long containing the type of item such as a (button,edit box,text,etc...)
// D = This is a long containing the style of the item.
// E = This is a long containing the width of the item.
// F = This is a long containing the height of the item.
// G = This is a character string containing a variable number of chars,
//     this string is valid until a zero is read. the first char is zero
//     for no string.
// H = This is a comma delimited string that contains the row that is associated
//     with the item. this string is valied until a zero is read, the first char
//     is zero for no string.
//
// Nothing is returned.
//
void FRM_ViewEdit::OnCopy(BOOL Ctype) {
    void    *memptr;
    long     memsize;
    CString  string,
             cmdstr,
             txtstr;
    HGLOBAL  hGlobal;
    CWnd    *pwnd;
    long     style,
             type,
             memidx=0;
    short    fi1,
             fi2,
             fi3;

    if (!m_FrmCur || !m_ListCur || !m_selcntr) return;

	if (m_lcpoint) delete m_lcpoint;

    for(memsize=0,fi1=0;fi1<m_selcntr;++fi1) memsize+=(sizeof(long)*4);

	m_lcpoint= new struct FRM_cpypoint [fi1];
	memset(m_lcpoint,0,sizeof(struct FRM_cpypoint)*fi1);
	memsize+=sizeof(long);
    if (!(hGlobal=GlobalAlloc(GHND,memsize))) return;
    if (!(memptr=GlobalLock(hGlobal))) goto exit;
    *((short *)((char *)memptr+memidx))=(short)Ctype; memidx+=sizeof(short);
	*((short *)((char *)memptr+memidx))=(short)fi1;   memidx+=sizeof(short);
    for(fi1=0;fi1<m_selcntr;++fi1) {

        //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
        // Create an sql statment for this item and get the values from the row
        // if one is there.
        sprintf(FRM_szText,ResourceString(IDC_ICADFORM_SELECT___FROM__15, "Select * from Itempropvals where Fkey=%d and Id=%d" ),m_FrmCur->fkey,m_SelList[fi1]->Wid);
        if (SDS_CMainWindow->m_CdatarecordPtr->IsOpen()) SDS_CMainWindow->m_CdatarecordPtr->Close();
        try	{ SDS_CMainWindow->m_CdatarecordPtr->Open(dbOpenDynaset,FRM_szText); }
	    catch(CDaoException* e)	{ e->Delete(); continue; }
        if (!SDS_CMainWindow->m_CdatarecordPtr->IsBOF()) {
            string.Empty();
            fi3=SDS_CMainWindow->m_CdatarecordPtr->GetFieldCount();
            for(fi2=0;fi2<(fi3-3);++fi2) {
                VarToCStr(&cmdstr,&SDS_CMainWindow->m_CdatarecordPtr->GetFieldValue((fi2+3)));
                string+=cmdstr; string+=ResourceString(IDC_ICADFORM___29, "," );
            }
        }

        m_lcpoint[fi1].x=m_SelList[fi1]->rect.left;
        m_lcpoint[fi1].y=m_SelList[fi1]->rect.top;
        m_mdialog->GetDlgItemText(m_SelList[fi1]->Wid,txtstr);

        memsize+=(txtstr.GetLength()+string.GetLength()+2);
        GlobalUnlock(hGlobal);
        if (!(hGlobal=GlobalReAlloc(hGlobal,memsize,GMEM_ZEROINIT))) goto exit;
        if (!(memptr=GlobalLock(hGlobal))) goto exit;

        if (!(pwnd=m_mdialog->GetDlgItem(m_SelList[fi1]->Wid))) return;
        GetClassName(pwnd->m_hWnd,FRM_szText,255);
        if (strcmp(FRM_szText,ResourceString(IDC_ICADFORM_BUTTON_7, "BUTTON" )))       type=FRM_FORM_BUTTON;
        else if (strcmp(FRM_szText,ResourceString(IDC_ICADFORM_EDIT_3, "EDIT" )))    type=FRM_FORM_EDIT;
        else if (strcmp(FRM_szText,ResourceString(IDC_ICADFORM_LISTBOX_11, "LISTBOX" ))) type=FRM_FORM_LISTBOX;
        else if (strcmp(FRM_szText,ResourceString(IDC_ICADFORM_STATIC_9, "STATIC" )))  type=FRM_FORM_TEXT;
        style=GetWindowLong(pwnd->m_hWnd,GWL_STYLE);
        memcpy((void *)((char *)memptr+memidx),(void *)&type,sizeof(long));  memidx+=sizeof(long);
        memcpy((void *)((char *)memptr+memidx),(void *)&style,sizeof(long)); memidx+=sizeof(long);
        *((long *)((char *)memptr+memidx))=(m_SelList[fi1]->rect.right-m_SelList[fi1]->rect.left); memidx+=sizeof(long);
        *((long *)((char *)memptr+memidx))=(m_SelList[fi1]->rect.bottom-m_SelList[fi1]->rect.top); memidx+=sizeof(long);
        if (txtstr.GetLength())  memcpy(((char *)memptr+memidx),(LPCTSTR)txtstr,txtstr.GetLength()); memidx+=(txtstr.GetLength()+1);
        if (string.GetLength())  memcpy(((char *)memptr+memidx),(LPCTSTR)string,string.GetLength()); memidx+=(string.GetLength()+1);
    }
    GlobalUnlock(hGlobal);
    if (!OpenClipboard()) goto exit;
    EmptyClipboard();
    SetClipboardData(CF_TEXT,hGlobal);
    CloseClipboard();
	if (Ctype) OnDeleteItem();
    return;

    exit:
        GlobalFree(hGlobal);
}

void FRM_ViewEdit::OnPaste() {
    CString     string,
                cmdstr;
//    const char *sptr1,
//               *sptr2;
    HANDLE      clipdata;
    long        style,
                type,
                Ctype;
    RECT        rect;
	short       fi1,
			    fi2,
                //fi3,
                fi4;
                //idx,
//                lstidx;
    COleVariant oleValue1,
                oleValue2;

    if (!OpenClipboard()) return;
    if (!(clipdata=GetClipboardData(CF_TEXT))) goto exit;
	if (m_selcntr) FRM_FreeSelList();
    Ctype=(*((short *)clipdata));
    clipdata=(((char *)clipdata)+sizeof(short));
	fi1=(short)(*((short *)clipdata));
    clipdata=(((char *)clipdata)+sizeof(short));
	for(fi2=0;fi2<fi1;++fi2) {
		FRM_AddHead(NULL);
		if (!Ctype) m_lcpoint[fi2].x+=FRM_X_OFFDIFF; m_lcpoint[fi2].y+=FRM_Y_OFFDIFF;
		m_ListCur  =m_FrmCur->ptr;
		FRM_AddSelHead();
		m_pickplace=0;
		m_GPOff.x=0; m_GPOff.y=0;
		memset((void *)&m_ListCur->Loffset,0,sizeof(m_ListCur->Loffset));
		memset((void *)&m_ListCur->offrect,0,sizeof(m_ListCur->offrect));
		m_ListCur->rect.left  =m_lcpoint[fi2].x;
		m_ListCur->rect.top   =m_lcpoint[fi2].y;
		m_ListCur->rect.right =m_lcpoint[fi2].x+(long)*(((char *)clipdata)+(sizeof(long)*2));
		m_ListCur->rect.bottom=m_lcpoint[fi2].y+(long)*(((char *)clipdata)+(sizeof(long)*3));

		memcpy(&rect,&m_ListCur->rect,sizeof(RECT));
		ClientToScreen(&rect);
		m_mdialog->ScreenToClient(&rect);

		m_ListCur->LPoint.x=m_lcpoint[fi2].x;
		m_ListCur->LPoint.y=m_lcpoint[fi2].y;
		memcpy(&type,clipdata,sizeof(long));
	    clipdata=(((char *)clipdata)+sizeof(long));
		memcpy(&style,clipdata,sizeof(long));
	    clipdata=(((char *)clipdata)+(sizeof(long)*3));
		string=((char *)clipdata);
	    clipdata=(((char *)clipdata)+string.GetLength()+1);
        cmdstr=((char *)clipdata);

		switch(type) {
			case FRM_FORM_TEXT:
				{
					CStatic *hItem;
					hItem = new CStatic();
					hItem->Create((LPCTSTR)string,style,rect,(CWnd *)m_mdialog,m_IDCntr);
					m_ListCur->Wid=m_IDCntr++;
				}
				break;
			case FRM_FORM_CHECK:
			case FRM_FORM_RADIO:
			case FRM_FORM_BUTTON:
				{
					CButton *hItem;
					hItem = new CButton();
					hItem->Create((LPCTSTR)string,style,rect,(CWnd *)m_mdialog,m_IDCntr);
					m_ListCur->Wid=m_IDCntr++;
				}
				break;
			case FRM_FORM_EDIT:
				{
					CEdit *hItem;
					hItem = new CEdit();
					hItem->Create(style,rect,(CWnd *)m_mdialog,m_IDCntr);
					m_ListCur->Wid=m_IDCntr++;
				}
				break;
			case FRM_FORM_LISTBOX:
				{
					CListBox *hItem;
					hItem = new CListBox();
					hItem->Create(style,rect,(CWnd *)m_mdialog,m_IDCntr);
					m_ListCur->Wid=m_IDCntr++;
				}
			default:
				break;
		}

        //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
        // Create an SQL statment for this item and get the values from the row
        // if one is there.
        sprintf(FRM_szText,"Select * from Itempropvals where Fkey=%d and Id=%d"/*DNT*/,m_FrmCur->fkey,m_SelList[fi2]->Wid);
        if (SDS_CMainWindow->m_CdatarecordPtr->IsOpen()) SDS_CMainWindow->m_CdatarecordPtr->Close();
        try	{ SDS_CMainWindow->m_CdatarecordPtr->Open(dbOpenDynaset,FRM_szText); }
	    catch(CDaoException* e)	{ e->Delete(); continue; }
        if (SDS_CMainWindow->m_CdatarecordPtr->IsBOF()) {
            oleValue1=m_FrmCur->fkey;
            oleValue2=m_SelList[fi2]->Wid;
            SDS_CMainWindow->m_CdatarecordPtr->AddNew();
    	    SDS_CMainWindow->m_CdatarecordPtr->SetFieldValue("Fkey"/*DNT*/,oleValue1);
	        SDS_CMainWindow->m_CdatarecordPtr->SetFieldValue("Id"/*DNT*/,oleValue2);
            fi4=SDS_CMainWindow->m_CdatarecordPtr->GetFieldCount();
/*
            sptr1=sptr2=(LPCTSTR)cmdstr;
            for(fi3=0;fi3<(fi4-3);++fi3) {
                sptr2=strchr(sptr1,',');
                if (sptr2=sptr1) { sptr1=sptr1+1; continue; }
                *sptr2=(char)0;
                sptr1=sptr1+1;
                if (sptr2=strchr(sptr1,',')) sptr2=0;
                oleValue1=sptr1;
    	        SDS_CMainWindow->m_CdatarecordPtr->SetFieldValue((fi3+3),oleValue1);
            }
*/
            SDS_CMainWindow->m_CdatarecordPtr->Update();
        }

	    clipdata=(((char *)clipdata)+cmdstr.GetLength()+1);
	}
    m_mdialog->RedrawWindow();
    MakePicture(m_mdialog);
    for(fi2=0;fi2<fi1;++fi2) FRM_SelectItem(m_SelList[fi2]->rect,0);
    exit:
        CloseClipboard();
}

BOOL FRM_ViewEdit::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) {
    switch(m_idcCursor) {
        case FRM_CROSS_ID:
            m_prevCursor=(::SetCursor(AfxGetApp()->LoadCursor(FRM_FORM_MOVE)));
            break;
        case FRM_SIZENESW_ID:
            m_prevCursor=(::SetCursor(AfxGetApp()->LoadCursor(FRM_FORM_DIAG2)));
            break;
        case FRM_SIZENWSE_ID:
            m_prevCursor=(::SetCursor(AfxGetApp()->LoadCursor(FRM_FORM_DIAG1)));
            break;
        case FRM_SIZEWE_ID:
            m_prevCursor=(::SetCursor(AfxGetApp()->LoadCursor(FRM_FORM_HORZR)));
            break;
        case FRM_SIZENS_ID:
            m_prevCursor=(::SetCursor(AfxGetApp()->LoadCursor(FRM_FORM_VERTR)));
            break;
        case FRM_ARROW_ID:
            m_prevCursor=(::SetCursor(AfxGetApp()->LoadCursor(FRM_FORM_SELECT)));
            break;
    }
    return TRUE;
}

void FRM_ViewEdit::OnCaptureChanged(CWnd* pWnd) {
    ::SetCursor(m_prevCursor);
}

void FRM_ViewEdit::OnEnable(BOOL bEnable) {
    if (bEnable) {
        if (m_mdialog && m_mdialog->m_hWnd) m_mdialog->ShowWindow(SW_SHOWNORMAL);
    }
}

void FRM_ViewEdit::FRM_OnAlign(UINT type) {
    short  fi1;
    long   lfttop,
           rtbottom,
           thtwth=0,
           diff;
    RECT   rect;

    if (!m_FrmCur || !m_ListCur || !m_selcntr || m_selcntr<=1) return;

    switch(type) {
        case FRM_FORM_VCENT:
            FRM_Sorttype=1;
            FRM_Listptr=m_SelList;
            sortlist(m_selcntr);
            lfttop  =m_SelList[0]->rect.top;
            rtbottom=m_SelList[m_selcntr-1]->rect.bottom;
            m_mdialog->GetClientRect(&rect);
            thtwth=((rect.bottom+FRM_V_DLGOFF)-(rtbottom-lfttop))/2;
            thtwth-=(lfttop);
            break;
        case FRM_FORM_HCENT:
            FRM_Sorttype=0;
            FRM_Listptr=m_SelList;
            sortlist(m_selcntr);
            lfttop  =m_SelList[0]->rect.left;
            rtbottom=m_SelList[m_selcntr-1]->rect.right;
            m_mdialog->GetClientRect(&rect);
            thtwth=((rect.right+ FRM_H_DLGOFF)-(rtbottom-lfttop))/2;
            thtwth-=(lfttop);
            break;
        case FRM_FORM_HSACROS:
            FRM_Sorttype=0;
            FRM_Listptr=m_SelList;
            sortlist(m_selcntr);
            lfttop  =m_SelList[0]->rect.left;
            rtbottom=m_SelList[m_selcntr-1]->rect.right;
            for(fi1=0;fi1<m_selcntr;++fi1) thtwth+=m_SelList[fi1]->rect.right-m_SelList[fi1]->rect.left;
            thtwth=((rtbottom-lfttop)-thtwth)/(m_selcntr-1);
            break;
        case FRM_FORM_VSACROS:
            FRM_Sorttype=1;
            FRM_Listptr=m_SelList;
            sortlist(m_selcntr);
            lfttop  =m_SelList[0]->rect.top;
            rtbottom=m_SelList[m_selcntr-1]->rect.bottom;
            for(fi1=0;fi1<m_selcntr;++fi1) thtwth+=m_SelList[fi1]->rect.bottom-m_SelList[fi1]->rect.top;
            thtwth=((rtbottom-lfttop)-thtwth)/(m_selcntr-1);
            break;
    }

    for(fi1=0;fi1<m_selcntr;++fi1) {
        switch(type) {
            case FRM_FORM_LALIGN:
                diff=m_SelList[fi1]->rect.right-m_SelList[fi1]->rect.left;
                m_SelList[fi1]->rect.left =m_ListCur->rect.left;
                m_SelList[fi1]->rect.right=m_ListCur->rect.left+diff;
                break;
            case FRM_FORM_RALIGN:
                diff=m_SelList[fi1]->rect.right-m_SelList[fi1]->rect.left;
                m_SelList[fi1]->rect.right=m_ListCur->rect.right;
                m_SelList[fi1]->rect.left =m_ListCur->rect.right-diff;
                break;
            case FRM_FORM_TALIGN:
                diff=m_SelList[fi1]->rect.bottom-m_SelList[fi1]->rect.top;
                m_SelList[fi1]->rect.top   =m_ListCur->rect.top;
                m_SelList[fi1]->rect.bottom=m_ListCur->rect.top+diff;
                break;
            case FRM_FORM_BALIGN:
                diff=m_SelList[fi1]->rect.bottom-m_SelList[fi1]->rect.top;
                m_SelList[fi1]->rect.bottom=m_ListCur->rect.bottom;
                m_SelList[fi1]->rect.top   =m_ListCur->rect.bottom-diff;
                break;
            case FRM_FORM_SWIDTH:
                diff=m_ListCur->rect.right-m_ListCur->rect.left;
                m_SelList[fi1]->rect.right=m_SelList[fi1]->rect.left+diff;
                break;
            case FRM_FORM_SHEIGHT:
                diff=m_ListCur->rect.bottom-m_ListCur->rect.top;
                m_SelList[fi1]->rect.bottom=m_SelList[fi1]->rect.top+diff;
                break;
            case FRM_FORM_SSIZE:
                diff=m_ListCur->rect.right-m_ListCur->rect.left;
                m_SelList[fi1]->rect.right=m_SelList[fi1]->rect.left+diff;
                diff=m_ListCur->rect.bottom-m_ListCur->rect.top;
                m_SelList[fi1]->rect.bottom=m_SelList[fi1]->rect.top+diff;
                break;
            case FRM_FORM_VCENT:
                m_SelList[fi1]->rect.top   +=thtwth;
                m_SelList[fi1]->rect.bottom+=thtwth;
                break;
            case FRM_FORM_HCENT:
                m_SelList[fi1]->rect.left +=thtwth;
                m_SelList[fi1]->rect.right+=thtwth;
                break;
            case FRM_FORM_HSACROS:
                if (!fi1 || (fi1+1==m_selcntr)) { rtbottom=m_SelList[fi1]->rect.right; continue; }
                diff=m_SelList[fi1]->rect.right-m_SelList[fi1]->rect.left;
                m_SelList[fi1]->rect.left =rtbottom+thtwth;
                m_SelList[fi1]->rect.right=m_SelList[fi1]->rect.left+diff;
                rtbottom=m_SelList[fi1]->rect.right;
                break;
            case FRM_FORM_VSACROS:
                if (!fi1 || (fi1+1==m_selcntr)) { rtbottom=m_SelList[fi1]->rect.bottom; continue; }
                diff=m_SelList[fi1]->rect.bottom-m_SelList[fi1]->rect.top;
                m_SelList[fi1]->rect.top   =rtbottom+thtwth;
                m_SelList[fi1]->rect.bottom=m_SelList[fi1]->rect.top+diff;
                rtbottom=m_SelList[fi1]->rect.bottom;
                break;
        }
        FRM_MoveItem(m_SelList[fi1],TRUE);
    }
    m_mdialog->RedrawWindow();
    MakePicture(m_mdialog);
    for(fi1=0;fi1<m_selcntr;++fi1) if (m_SelList[fi1]!=m_ListCur) FRM_SelectItem(m_SelList[fi1]->rect,0);
    FRM_SelectItem(m_ListCur->rect,1);
}

void FRM_ViewEdit::FRM_MoveItem(struct FRM_frmInfo *Item,BOOL type) {
    CWnd  *ItemPtr;
    RECT   cnvrect;

	memcpy(&cnvrect,&Item->rect,sizeof(RECT));

    ClientToScreen(&cnvrect);
    Item->LPoint.x=Item->rect.left; Item->LPoint.y=Item->rect.top;
    Item->offrect.top=0; Item->offrect.left=0;
    Item->offrect.bottom=Item->rect.bottom-Item->rect.top;
    Item->offrect.right =Item->rect.right-Item->rect.left;

    if (type) {
        m_mdialog->ScreenToClient(&cnvrect);
        ItemPtr=m_mdialog->GetDlgItem(Item->Wid);
    } else {
        ScreenToClient(&cnvrect);
        ItemPtr=m_mdialog;
    }
    ItemPtr->MoveWindow(cnvrect.left,cnvrect.top,cnvrect.right-cnvrect.left,cnvrect.bottom-cnvrect.top,TRUE);
}

void FRM_ViewEdit::OnRButtonDown( UINT nFlags, CPoint point ) {
    short fi1;

    RedrawWindow();
    for(m_ListCur=m_FrmCur->ptr,fi1=0;fi1<m_FrmCur->ItemCntr;m_ListCur=m_ListCur->next,++fi1) {
        if ((point.x>m_ListCur->rect.left && point.x<m_ListCur->rect.right && point.y>m_ListCur->rect.top && point.y<m_ListCur->rect.bottom) || (!m_ListCur->next)) {
            if (!m_Selection) FRM_FreeSelList();
            FRM_AddSelHead();
            FRM_SelectItem(m_ListCur->rect,0);
            m_ListCur->Loffset.left  =m_ListCur->offrect.left  =point.x-m_ListCur->rect.left;
            m_ListCur->Loffset.right =m_ListCur->offrect.right =m_ListCur->rect.right-point.x;
            m_ListCur->Loffset.top   =m_ListCur->offrect.top   =point.y-m_ListCur->rect.top;
            m_ListCur->Loffset.bottom=m_ListCur->offrect.bottom=m_ListCur->rect.bottom-point.y;
			m_idcCursor=FRM_ARROW_ID;
            break;
        }
    }
    CFrameWnd::OnRButtonDown(nFlags, point);
}

void FRM_ViewEdit::OnLButtonUp(UINT nFlags,CPoint point) {
    short  fi1;
    long   WStyle;
    RECT   rect;
	CDC   *cdc;

    CFrameWnd::OnLButtonUp(nFlags, point);

    if (m_selcntr) {
        for(fi1=0;fi1<m_selcntr;++fi1) {
            if (m_boxstate) {
                if (m_pickplace) {
                    point.x-=m_GPOff.x;
                    point.y-=m_GPOff.y;
                }
			    switch(m_pickplace) {
				    case 0:
				    case 1:
				    case 2:
				    case 3:
				    case 4:
                        if (m_SelList[fi1]->offrect.top<0)  {
                            m_SelList[fi1]->offrect.bottom=-(m_SelList[fi1]->offrect.top);
                            m_SelList[fi1]->offrect.top=0;
                        }
                        if (m_SelList[fi1]->offrect.left<0) {
                            m_SelList[fi1]->offrect.right =-(m_SelList[fi1]->offrect.left);
                            m_SelList[fi1]->offrect.left=0;
                        }
					    m_SelList[fi1]->rect.top   =m_SelList[fi1]->LPoint.y-m_SelList[fi1]->offrect.top;
					    m_SelList[fi1]->rect.bottom=m_SelList[fi1]->LPoint.y+m_SelList[fi1]->offrect.bottom;
					    m_SelList[fi1]->rect.left  =m_SelList[fi1]->LPoint.x-m_SelList[fi1]->offrect.left;
					    m_SelList[fi1]->rect.right =m_SelList[fi1]->LPoint.x+m_SelList[fi1]->offrect.right;
					    break;
				    case 5:
				    case 6:
					    m_ListCur->rect.left  =point.x-m_ListCur->offrect.left;
					    m_ListCur->rect.right =point.x+m_ListCur->offrect.right;
					    m_ListCur->rect.top   =m_ListCur->rect.top;
					    m_ListCur->rect.bottom=m_ListCur->rect.bottom;
					    break;
				    case 7:
				    case 8:
					    m_ListCur->rect.left  =m_ListCur->rect.left;
					    m_ListCur->rect.right =m_ListCur->rect.right;
					    m_ListCur->rect.top   =point.y-m_ListCur->offrect.top;
					    m_ListCur->rect.bottom=point.y+m_ListCur->offrect.bottom;
					    break;
			    }
            }
            switch(m_Controltype) {
                case FRM_FORM_TEXT:
                    {
                        CStatic *hItem;
                        hItem = new CStatic();
                        hItem->Create(ResourceString(IDC_ICADFORM_STATIC_30, "Static" ),WS_CHILD|WS_VISIBLE|WS_TABSTOP,m_ListCur->rect,(CWnd *)m_mdialog,m_IDCntr);
                        m_ListCur->Wid=m_IDCntr++;
                    }
                    break;
                case FRM_FORM_CHECK:
                case FRM_FORM_RADIO:
                case FRM_FORM_BUTTON:
                    {
                        CString String;
                        switch(m_Controltype) {
                            case FRM_FORM_CHECK: WStyle=BS_AUTOCHECKBOX;    String=ResourceString(IDC_ICADFORM_CHECK_BOX_31, "Check Box" );    break;
                            case FRM_FORM_RADIO: WStyle=BS_AUTORADIOBUTTON; String=ResourceString(IDC_ICADFORM_RADIO_BUTTON_32, "Radio button" ); break;
                            default: WStyle=0;    String=ResourceString(DNT_ICADFORM_BUTTON_33, "Button" ); break;
                        }
                        WStyle|=(WS_CHILD|WS_VISIBLE|WS_TABSTOP);
                        CButton *hItem;
                        hItem = new CButton();
                        hItem->Create((LPCTSTR)String,WStyle,m_ListCur->rect,(CWnd *)m_mdialog,m_IDCntr);
                        m_ListCur->Wid=m_IDCntr++;
                    }
                    break;
                case FRM_FORM_EDIT:
                    {
                        CEdit *hItem;
                        hItem = new CEdit();
                        hItem->Create(WS_CHILD|WS_VISIBLE|WS_TABSTOP|WS_BORDER,m_ListCur->rect,(CWnd *)m_mdialog,m_IDCntr);
                        m_ListCur->Wid=m_IDCntr++;
                    }
                    break;
				case FRM_FORM_LISTBOX:
					{
						CListBox *hItem;
						hItem = new CListBox();
						hItem->Create(WS_CHILD|WS_VISIBLE|WS_TABSTOP|WS_BORDER,m_ListCur->rect,(CWnd *)m_mdialog,m_IDCntr);
                        m_ListCur->Wid=m_IDCntr++;
					}
                default:
                    break;
            }
            if (m_Controltype!=FRM_FORM_ARROW) {
                CToolBarCtrl &TCtrl=m_formedit->m_FToolBar.GetToolBarCtrl();
            	UINT  nId;
                short fi1,
                      fi2;

		        for(fi2=0,fi1=(sizeof(FRM_FormFBId)/sizeof(UINT));fi2<fi1;++fi2) {
				    nId=m_formedit->m_FToolBar.GetItemID(fi2);
				    if (TCtrl.IsButtonChecked(nId)) { TCtrl.CheckButton(nId,FALSE); break; }
			    }
                TCtrl.CheckButton(FRM_FORM_ARROW,TRUE);
                m_Controltype=FRM_FORM_ARROW;
            }
            if (m_boxstate && m_MoveFlag) {
                FRM_MoveItem(m_SelList[fi1],(m_SelList[fi1]==m_ListEnd)?FALSE:TRUE);
                if (m_SelList[fi1]==m_ListEnd) RedrawWindow();
            }
        }
        if (m_MoveFlag) {
            m_mdialog->RedrawWindow();
            MakePicture(m_mdialog);
        } else if (m_SingleGrp) {
            FRM_FreeSelList();
            FRM_AddSelHead();
            if (m_FrmCur->hdcCompat) {
                m_mdialog->GetWindowRect(&rect);
                cdc=m_mdialog->GetDC();
                BitBlt(cdc->m_hDC,0,0,rect.right,rect.bottom,m_FrmCur->hdcCompat,0,0,SRCCOPY);
                ReleaseDC(cdc);
            }
        }
        m_SingleGrp=FALSE;
        for(fi1=0;fi1<m_selcntr;++fi1) if (m_SelList[fi1]!=m_ListCur) FRM_SelectItem(m_SelList[fi1]->rect,0);
        FRM_SelectItem(m_ListCur->rect,1);
    }
}

void FRM_ViewEdit::OnLButtonDown(UINT nFlags,CPoint point) {
    short        fi1;
    RECT         rect;
	CDC         *cdc;

    CFrameWnd::OnLButtonDown(nFlags,point);

    switch(m_Controltype) {
        case FRM_FORM_ARROW:
             if (m_ListCur && (m_idcCursor!=FRM_ARROW_ID)) {
                if (point.x>=m_ListCur->rect.right && point.x<=m_ListCur->rect.right+FRM_H_DLGOFF && point.y<=m_ListCur->rect.top && point.y>=m_ListCur->rect.top-FRM_V_DLGOFF) {
			        m_pickplace=1; // top right corner.
                    m_GPOff.x=point.x-m_ListCur->rect.right;
                    m_GPOff.y=point.y-m_ListCur->rect.top;
                    m_ListCur->offrect.left  =m_ListCur->rect.right-m_ListCur->rect.left;;
                    m_ListCur->offrect.right =0;
                    m_ListCur->offrect.top   =0;
                    m_ListCur->offrect.bottom=m_ListCur->rect.bottom-m_ListCur->rect.top;
                } else if (point.x<=m_ListCur->rect.left && point.x>=m_ListCur->rect.left-FRM_H_DLGOFF && point.y>=m_ListCur->rect.bottom && point.y<=m_ListCur->rect.bottom+FRM_V_DLGOFF) {
			        m_pickplace=2; // bottom left corner..
                    m_GPOff.x=point.x-m_ListCur->rect.left;
                    m_GPOff.y=point.y-m_ListCur->rect.bottom;
                    m_ListCur->offrect.left  =0;
                    m_ListCur->offrect.right =m_ListCur->rect.right-m_ListCur->rect.left;;
                    m_ListCur->offrect.top   =m_ListCur->rect.bottom-m_ListCur->rect.top;;
                    m_ListCur->offrect.bottom=0;
                } else if (point.x<=m_ListCur->rect.left && point.x>=m_ListCur->rect.left-FRM_H_DLGOFF && point.y<=m_ListCur->rect.top && point.y>=m_ListCur->rect.top-FRM_V_DLGOFF) {
			        m_pickplace=3; // top left corner.
                    m_GPOff.x=point.x-m_ListCur->rect.left;
                    m_GPOff.y=point.y-m_ListCur->rect.top;
                    m_ListCur->offrect.left  =0;
                    m_ListCur->offrect.right =m_ListCur->rect.right-m_ListCur->rect.left;
                    m_ListCur->offrect.top   =0;
                    m_ListCur->offrect.bottom=m_ListCur->rect.bottom-m_ListCur->rect.top;
                } else if (point.x>=m_ListCur->rect.right && point.x<=m_ListCur->rect.right+FRM_H_DLGOFF && point.y>=m_ListCur->rect.bottom && point.y<=m_ListCur->rect.bottom+FRM_V_DLGOFF) {
			        m_pickplace=4; // bottom right corner.
                    m_GPOff.x=point.x-m_ListCur->rect.right;
                    m_GPOff.y=point.y-m_ListCur->rect.bottom;
                    m_ListCur->offrect.left  =m_ListCur->rect.right-m_ListCur->rect.left;
                    m_ListCur->offrect.right =0;
                    m_ListCur->offrect.top   =m_ListCur->rect.bottom-m_ListCur->rect.top;
                    m_ListCur->offrect.bottom=0;
                } else if (point.y>=(m_ListCur->rect.top+((m_ListCur->rect.bottom-m_ListCur->rect.top)/2)-2) && point.y<=(m_ListCur->rect.top+((m_ListCur->rect.bottom-m_ListCur->rect.top)/2)+3)) {
                    if (point.x>=m_ListCur->rect.left-FRM_H_DLGOFF && point.x<=m_ListCur->rect.left) {
    			        m_pickplace=5; // left middle.
                        m_GPOff.x=point.x-m_ListCur->rect.left;
                        m_GPOff.y=point.y-m_ListCur->rect.bottom;
                        m_ListCur->offrect.left  =0;
                        m_ListCur->offrect.right =m_ListCur->rect.right-m_ListCur->rect.left;
                        m_ListCur->offrect.top   =m_ListCur->rect.bottom-m_ListCur->rect.top;
                        m_ListCur->offrect.bottom=0;
			        } else if (point.x<=m_ListCur->rect.right+FRM_H_DLGOFF && point.x>=m_ListCur->rect.right) {
    			        m_pickplace=6; // right middle.
                        m_GPOff.x=point.x-m_ListCur->rect.right;
                        m_GPOff.y=point.y-m_ListCur->rect.bottom;
                        m_ListCur->offrect.left  =m_ListCur->rect.right-m_ListCur->rect.left;
                        m_ListCur->offrect.right =0;
                        m_ListCur->offrect.top   =m_ListCur->rect.bottom-m_ListCur->rect.top;
                        m_ListCur->offrect.bottom=0;
                    } else m_pickplace=0;
                } else if (point.x>=(m_ListCur->rect.left+((m_ListCur->rect.right-m_ListCur->rect.left)/2)-2) && point.x<=(m_ListCur->rect.left+((m_ListCur->rect.right-m_ListCur->rect.left)/2)+3)) {
                    if (point.y<=m_ListCur->rect.top && point.y>=m_ListCur->rect.top-FRM_V_DLGOFF) {
    			        m_pickplace=7; // top middle.
                        m_GPOff.x=point.x-m_ListCur->rect.right;
                        m_GPOff.y=point.y-m_ListCur->rect.top;
                        m_ListCur->offrect.left  =m_ListCur->rect.right-m_ListCur->rect.left;
                        m_ListCur->offrect.right =0;
                        m_ListCur->offrect.top   =0;
                        m_ListCur->offrect.bottom=m_ListCur->rect.bottom-m_ListCur->rect.top;
			        } else if (point.y<=m_ListCur->rect.bottom+FRM_V_DLGOFF && point.y>=m_ListCur->rect.bottom) {
    			        m_pickplace=8; // bottom middle.
                        m_GPOff.x=point.x-m_ListCur->rect.right;
                        m_GPOff.y=point.y-m_ListCur->rect.bottom;
                        m_ListCur->offrect.left  =m_ListCur->rect.right-m_ListCur->rect.left;
                        m_ListCur->offrect.right =0;
                        m_ListCur->offrect.top   =m_ListCur->rect.bottom-m_ListCur->rect.top;
                        m_ListCur->offrect.bottom=0;
                    } else m_pickplace=0;
                } else m_pickplace=0;
				if (m_pickplace) {
                    m_ListCur->LPoint.x=point.x-m_GPOff.x;
                    m_ListCur->LPoint.y=point.y-m_GPOff.y;
					memcpy(&m_ListCur->Loffset,&m_ListCur->offrect,sizeof(RECT));
                    return;
				}
            }
            m_OldCur=m_ListCur;
            for(m_ListCur=m_FrmCur->ptr,fi1=0;fi1<m_FrmCur->ItemCntr;m_ListCur=m_ListCur->next,++fi1) {
                if ((point.x>m_ListCur->rect.left && point.x<m_ListCur->rect.right && point.y>m_ListCur->rect.top && point.y<m_ListCur->rect.bottom) || (fi1==(m_FrmCur->ItemCntr-1))) {
                    m_ListCur->Loffset.left  =m_ListCur->offrect.left  =point.x-m_ListCur->rect.left;
                    m_ListCur->Loffset.right =m_ListCur->offrect.right =m_ListCur->rect.right-point.x;
                    m_ListCur->Loffset.top   =m_ListCur->offrect.top   =point.y-m_ListCur->rect.top;
                    m_ListCur->Loffset.bottom=m_ListCur->offrect.bottom=m_ListCur->rect.bottom-point.y;
                    m_ListCur->LPoint=point;

                    // If cliked on the dialog then set the cursor to an arrow.
                    if (m_ListCur!=m_ListEnd) m_idcCursor=FRM_CROSS_ID; else { m_idcCursor=FRM_ARROW_ID; m_pickplace=0; }
                    // Check if cliked on a button already cliked.
                    for(fi1=0;fi1<m_selcntr && m_SelList[fi1]!=m_ListCur;++fi1);
                    // If the ctrl key is not being held down.
                    if (!m_CtrlFlag) {
                        // if its a new button then select it.
                        if (fi1==m_selcntr) {
                            // if not part of a multiple select then do a redraw.
                            if (m_FrmCur->hdcCompat) {
                                m_mdialog->GetWindowRect(&rect);
                                cdc=m_mdialog->GetDC();
                                BitBlt(cdc->m_hDC,0,0,rect.right,rect.bottom,m_FrmCur->hdcCompat,0,0,SRCCOPY);
                                ReleaseDC(cdc);
                            }
                            MakePicture(m_mdialog);
                        } else m_SingleGrp=TRUE;
                    } else if (m_ListCur==m_ListEnd) { m_ListCur=m_OldCur; return; }
                    if (m_CtrlFlag) for(fi1=0;fi1<m_selcntr;++fi1) if (m_SelList[fi1]!=m_ListCur) FRM_SelectItem(m_SelList[fi1]->rect,0);
                    FRM_SelectItem(m_ListCur->rect,1);
                    m_formedit->m_StatusBar.UpdateBar(m_ListCur->rect);
                    m_MoveFlag=FALSE;
                    break;
                }
            }
            // if there is a selected item, and its a new one, and the ctrl key is not being held down then free the list.
            if (m_selcntr && (fi1==m_selcntr) && !m_CtrlFlag) FRM_FreeSelList();
            // if there is a seleted item, and its a new one, and the
            if (fi1==m_selcntr || !m_selcntr) FRM_AddSelHead();
            break;
        case FRM_FORM_TEXT:
        case FRM_FORM_CHECK:
        case FRM_FORM_RADIO:
        case FRM_FORM_BUTTON:
        case FRM_FORM_EDIT:
		case FRM_FORM_LISTBOX:
            FRM_AddHead(NULL);
            m_ListCur=m_FrmCur->ptr;
	        m_pickplace=4;
            m_GPOff.x=0; m_GPOff.y=0;
            memset((void *)&m_ListCur->Loffset,0,sizeof(m_ListCur->Loffset));
            memset((void *)&m_ListCur->offrect,0,sizeof(m_ListCur->offrect));
            m_ListCur->rect.left=point.x;
            m_ListCur->rect.top =point.y;
            if (m_selcntr) FRM_FreeSelList();
            FRM_AddSelHead();
            m_ListCur->LPoint.x=point.x;
            m_ListCur->LPoint.y=point.y;
            break;
    }

}

void FRM_ViewEdit::OnMouseMove(UINT nFlags,CPoint point) {
    short  fi1;
    CPoint pOff,
           Cpoint;

    CFrameWnd::OnMouseMove(nFlags,point);

	switch(nFlags) {
        case MK_CONTROL:
		case 0:
			if (m_selcntr) {
                if (m_selcntr>1) {
                    for(fi1=0;fi1<m_selcntr;++fi1) {
				        if (point.x>m_SelList[fi1]->rect.left && point.x<m_SelList[fi1]->rect.right && point.y>m_SelList[fi1]->rect.top && point.y<m_SelList[fi1]->rect.bottom) {
					        m_idcCursor=FRM_CROSS_ID; break;
				        }
                    }
                    if (fi1==m_selcntr) m_idcCursor=FRM_ARROW_ID;
                } else {
				    if (point.x>m_SelList[(m_selcntr-1)]->rect.left && point.x<m_SelList[(m_selcntr-1)]->rect.right && point.y>m_SelList[(m_selcntr-1)]->rect.top && point.y<m_SelList[(m_selcntr-1)]->rect.bottom) {
                        if (m_SelList[(m_selcntr-1)]==m_ListEnd) { m_idcCursor=FRM_ARROW_ID; break; }
					    m_idcCursor=FRM_CROSS_ID;
                    } else if (point.x>=m_SelList[(m_selcntr-1)]->rect.right && point.x<=m_SelList[(m_selcntr-1)]->rect.right+FRM_H_DLGOFF && point.y<=m_SelList[(m_selcntr-1)]->rect.top && point.y>=m_SelList[(m_selcntr-1)]->rect.top-FRM_V_DLGOFF) {
                        // top right corner.
                        if (m_SelList[(m_selcntr-1)]==m_ListEnd) { m_idcCursor=FRM_ARROW_ID; break; }
                        m_idcCursor=FRM_SIZENESW_ID;
                    } else if (point.x<=m_SelList[(m_selcntr-1)]->rect.left && point.x>=m_SelList[(m_selcntr-1)]->rect.left-FRM_H_DLGOFF && point.y>=m_SelList[(m_selcntr-1)]->rect.bottom && point.y<=m_SelList[(m_selcntr-1)]->rect.bottom+FRM_V_DLGOFF) {
                        // Bottom left corner.
                        if (m_SelList[(m_selcntr-1)]==m_ListEnd) { m_idcCursor=FRM_ARROW_ID; break; }
                        m_idcCursor=FRM_SIZENESW_ID;
                    } else if (point.x<=m_SelList[(m_selcntr-1)]->rect.left && point.x>=m_SelList[(m_selcntr-1)]->rect.left-FRM_H_DLGOFF && point.y<=m_SelList[(m_selcntr-1)]->rect.top && point.y>=m_SelList[(m_selcntr-1)]->rect.top-FRM_V_DLGOFF) {
                        // Top left corner.
                        if (m_SelList[(m_selcntr-1)]==m_ListEnd) { m_idcCursor=FRM_ARROW_ID; break; }
                        m_idcCursor=FRM_SIZENWSE_ID;
                    } else if (point.x>=m_SelList[(m_selcntr-1)]->rect.right && point.x<=m_SelList[(m_selcntr-1)]->rect.right+FRM_H_DLGOFF && point.y>=m_SelList[(m_selcntr-1)]->rect.bottom && point.y<=m_SelList[(m_selcntr-1)]->rect.bottom+FRM_V_DLGOFF) {
                        m_idcCursor=FRM_SIZENWSE_ID;
                    } else if (point.y>=(m_SelList[(m_selcntr-1)]->rect.top+((m_SelList[(m_selcntr-1)]->rect.bottom-m_SelList[(m_selcntr-1)]->rect.top)/2)-2) && point.y<=(m_SelList[(m_selcntr-1)]->rect.top+((m_SelList[(m_selcntr-1)]->rect.bottom-m_SelList[(m_selcntr-1)]->rect.top)/2)+3)) {
                        if (point.x>=m_SelList[(m_selcntr-1)]->rect.left-FRM_H_DLGOFF && point.x<=m_SelList[(m_selcntr-1)]->rect.left) {
                            // left middle.
                            if (m_SelList[(m_selcntr-1)]==m_ListEnd) { m_idcCursor=FRM_ARROW_ID; break; }
                            m_idcCursor=FRM_SIZEWE_ID;
                        } else if (point.x<=m_SelList[(m_selcntr-1)]->rect.right+FRM_H_DLGOFF && point.x>=m_SelList[(m_selcntr-1)]->rect.right) {
                            m_idcCursor=FRM_SIZEWE_ID;
                        } else {
                            m_idcCursor=FRM_ARROW_ID;
                        }
                    } else if (point.x>=(m_SelList[(m_selcntr-1)]->rect.left+((m_SelList[(m_selcntr-1)]->rect.right-m_SelList[(m_selcntr-1)]->rect.left)/2)-2) && point.x<=(m_SelList[(m_selcntr-1)]->rect.left+((m_SelList[(m_selcntr-1)]->rect.right-m_SelList[(m_selcntr-1)]->rect.left)/2)+3)) {
                        if (point.y<=m_SelList[(m_selcntr-1)]->rect.top && point.y>=m_SelList[(m_selcntr-1)]->rect.top-FRM_V_DLGOFF) {
                            // top middle.
                            if (m_SelList[(m_selcntr-1)]==m_ListEnd) { m_idcCursor=FRM_ARROW_ID; break; }
                            m_idcCursor=FRM_SIZENS_ID;
                        } else if (point.y<=m_SelList[(m_selcntr-1)]->rect.bottom+FRM_V_DLGOFF && point.y>=m_SelList[(m_selcntr-1)]->rect.bottom) {
                            m_idcCursor=FRM_SIZENS_ID;
                        } else {
                            m_idcCursor=FRM_ARROW_ID;
                        }
				    } else {
                        m_idcCursor=FRM_ARROW_ID;
                    }
                }
			}
			break;
        case FRM_LB_CTRL:
		case MK_LBUTTON:
            if (m_selcntr) {
				switch(m_pickplace) {
					case 0:
                        if (m_selcntr>1) break;
						if (m_SelList[(m_selcntr-1)]==m_ListEnd) return;
						break;
					case 1: // top right corner.
                        point.x-=m_GPOff.x;
                        point.y-=m_GPOff.y;
						m_SelList[0]->offrect.left  +=(point.x-m_SelList[0]->LPoint.x);
						m_SelList[0]->offrect.bottom+=(m_SelList[0]->LPoint.y-point.y);
						break;
					case 2: // bottom left corner.
                        point.x-=m_GPOff.x;
                        point.y-=m_GPOff.y;
						m_SelList[0]->offrect.right+=(m_SelList[0]->LPoint.x-point.x);
						m_SelList[0]->offrect.top  +=(point.y-m_SelList[0]->LPoint.y);
						break;
					case 3: // top left corner.
                        point.x-=m_GPOff.x;
                        point.y-=m_GPOff.y;
						m_SelList[0]->offrect.right +=(m_SelList[0]->LPoint.x-point.x);
						m_SelList[0]->offrect.bottom+=(m_SelList[0]->LPoint.y-point.y);
						break;
					case 4: // bottom right corner.
                        point.x-=m_GPOff.x;
                        point.y-=m_GPOff.y;
						m_SelList[0]->offrect.left+=(point.x-m_SelList[0]->LPoint.x);
						m_SelList[0]->offrect.top +=(point.y-m_SelList[0]->LPoint.y);
						break;
					case 5: // left middle.
                        point.x-=m_GPOff.x;
                        point.y =m_SelList[0]->LPoint.y;
						m_SelList[0]->offrect.right+=(m_SelList[0]->LPoint.x-point.x);
						break;
                    case 6: // right middle.
                        point.x-=m_GPOff.x;
                        point.y =m_SelList[0]->LPoint.y;
						m_SelList[0]->offrect.left+=(point.x-m_SelList[0]->LPoint.x);
                        break;
					case 7: // top middle.
                        point.x =m_SelList[0]->LPoint.x;
                        point.y-=m_GPOff.y;
						m_SelList[0]->offrect.bottom+=(m_SelList[0]->LPoint.y-point.y);
						break;
                    case 8: // bottom middle.
                        point.x =m_SelList[0]->LPoint.x;
                        point.y-=m_GPOff.y;
						m_SelList[0]->offrect.top+=(point.y-m_SelList[0]->LPoint.y);
                        break;
				}
                m_MoveFlag=TRUE;
//              need to calculate the total square area of the items.
                if (!m_pickplace) {
                    pOff.x=point.x-m_ListCur->LPoint.x;
                    pOff.y=point.y-m_ListCur->LPoint.y;
                } else {
                    pOff.x=0; pOff.y=0;
                }

                for(fi1=0;fi1<m_selcntr;++fi1) {
                    if (!m_pickplace) {
                        Cpoint.x=m_SelList[fi1]->LPoint.x+pOff.x;
                        Cpoint.y=m_SelList[fi1]->LPoint.y+pOff.y;
                    } else Cpoint=point;

                    FRM_DrawRect(Cpoint,m_SelList[fi1]->LPoint,m_boxstate,m_SelList[fi1]->offrect,m_SelList[fi1]->Loffset);
					memcpy(&m_SelList[fi1]->Loffset,&m_SelList[fi1]->offrect,sizeof(RECT));
                    m_SelList[fi1]->LPoint=Cpoint;
                }
                m_boxstate=1;
            }
            break;
    }
}

BOOL FRM_ViewEdit::FRM_DrawRect(CPoint point,CPoint lpoint,short state,RECT offset,RECT loffset) {
    CDC *pDC;

   	// Set the device context ready for painting.
    pDC=GetDC();
    pDC->SetROP2(R2_NOT);

    if (state) {
	    MoveToEx(pDC->m_hDC,lpoint.x+loffset.right,lpoint.y-loffset.top,NULL);
	    LineTo(pDC->m_hDC,lpoint.x-loffset.left,lpoint.y-loffset.top);
	    LineTo(pDC->m_hDC,lpoint.x-loffset.left,lpoint.y+loffset.bottom);
	    LineTo(pDC->m_hDC,lpoint.x+loffset.right,lpoint.y+loffset.bottom);
	    LineTo(pDC->m_hDC,lpoint.x+loffset.right,lpoint.y-loffset.top);
    }
    if (state!=2) {
	    MoveToEx(pDC->m_hDC,point.x+offset.right,point.y-offset.top,NULL);
	    LineTo(pDC->m_hDC,point.x-offset.left,point.y-offset.top);
	    LineTo(pDC->m_hDC,point.x-offset.left,point.y+offset.bottom);
	    LineTo(pDC->m_hDC,point.x+offset.right,point.y+offset.bottom);
	    LineTo(pDC->m_hDC,point.x+offset.right,point.y-offset.top);
    }

    ReleaseDC(pDC);
    return(TRUE);
}

BOOL FRM_ViewEdit::FRM_SelectItem(RECT rect,short type) {
    RECT    square;         // Holds the edit boxe's coords.
	CBrush *OldbrBkground;
    CDC    *pDC;
    CPen   *Oldpen;

    CPen   *pen        = new CPen();
    CBrush *brBkground = new CBrush();

   	// Set the device context ready for painting.
    pDC=GetDC();

    pen->CreatePen(PS_DASH,1,::GetSysColor(COLOR_HIGHLIGHT));
    Oldpen=(CPen *)pDC->SelectObject(pen);

    // Draw box around the item.
	MoveToEx(pDC->m_hDC,rect.left-3,rect.top,NULL);
	LineTo(pDC->m_hDC,rect.left-3,rect.bottom+2);
	LineTo(pDC->m_hDC,rect.right+2,rect.bottom+2);
	LineTo(pDC->m_hDC,rect.right+2,rect.top-3);
	LineTo(pDC->m_hDC,rect.left-1,rect.top-3);
    pDC->SelectObject(Oldpen);
    pen->DeleteObject();

    switch(type) {
        case 0:
            brBkground->CreateSolidBrush(::GetSysColor(COLOR_WINDOW));
            pen->CreatePen(PS_SOLID,1,::GetSysColor(COLOR_HIGHLIGHT));
            break;
        case 1:
            brBkground->CreateSolidBrush(::GetSysColor(COLOR_HIGHLIGHT));
            pen->CreatePen(PS_SOLID,1,::GetSysColor(COLOR_HIGHLIGHT));
            break;
    }
    pDC->SelectObject(pen);
    OldbrBkground=(CBrush *)pDC->SelectObject(brBkground);

    // Draw top left corner box of item.
	square.left  =rect.left-FRM_H_DLGOFF;
	square.right =rect.left;
	square.top   =rect.top-FRM_V_DLGOFF;
	square.bottom=rect.top;
    pDC->Rectangle(&square);

    // Draw left middle box.
  	square.top   =rect.top+((rect.bottom-rect.top)/2)-2;
	square.bottom=square.top+FRM_V_DLGOFF;
    pDC->Rectangle(&square);

    // Draw left bottom box.
  	square.top   =rect.bottom;
	square.bottom=rect.bottom+FRM_V_DLGOFF;
    pDC->Rectangle(&square);

    // Draw middle bottom box.
  	square.left =rect.left+((rect.right-rect.left)/2)-2;
	square.right=square.left+FRM_H_DLGOFF;
    pDC->Rectangle(&square);

    // Draw bottom right box.
	square.left =rect.right;
	square.right=rect.right+FRM_H_DLGOFF;
    pDC->Rectangle(&square);

    // Draw right middle box.
  	square.top   =rect.top+((rect.bottom-rect.top)/2)-2;
	square.bottom=square.top+FRM_V_DLGOFF;
    pDC->Rectangle(&square);

    // Draw top right box.
	square.top   =rect.top-FRM_V_DLGOFF;
	square.bottom=rect.top;
    pDC->Rectangle(&square);

    // Draw middle top box.
  	square.left =rect.left+((rect.right-rect.left)/2)-2;
	square.right=square.left+FRM_H_DLGOFF;
    pDC->Rectangle(&square);

    pDC->SelectObject(OldbrBkground);
    brBkground->DeleteObject();
    pDC->SelectObject(Oldpen);
    pen->DeleteObject();
    delete brBkground;
    delete pen;
    ReleaseDC(pDC);

    return(TRUE);
}

void FRM_ViewEdit::FRM_AddDlgHead() {
    struct FRM_frmList *frmlist;

    frmlist= new struct FRM_frmList ;
	memset(frmlist,0,sizeof(struct FRM_frmList));
    if (!m_FrmBeg) {
        m_FrmBeg=m_FrmCur=frmlist;
    } else {
        for(m_FrmCur=m_FrmBeg;m_FrmCur->next;m_FrmCur=m_FrmCur->next);
        m_FrmCur->next=frmlist;
        m_FrmCur=frmlist;
    }
}

void FRM_ViewEdit::FRM_FreeDlgLinks() {
    struct FRM_frmList *frmlist;

    if (!m_FrmBeg) return;
    for(m_FrmCur=frmlist=m_FrmBeg;frmlist;frmlist=m_FrmCur) {
        FRM_FreeAll();
        m_FrmCur=frmlist->next;
        if (frmlist->hdcCompat) DeleteDC(frmlist->hdcCompat);
        if (frmlist->frmname)   IC_FREE(frmlist->frmname);
        if (frmlist->bptr)      IC_FREE(frmlist->bptr);
        if (frmlist->tfname)    IC_FREE(frmlist->tfname);
        IC_FREE(frmlist);
    }
    m_FrmBeg=NULL;
}

void FRM_ViewEdit::FRM_AddHead(CWnd *hItem) {
    struct FRM_frmInfo *wininfo;

    if (!m_FrmCur) return;
    
	wininfo= new struct FRM_frmInfo ;
	memset(wininfo,0,sizeof(struct FRM_frmInfo));
    if (hItem!=NULL) {
        wininfo->Wid=GetWindowLong(hItem->m_hWnd,GWL_ID);
        hItem->GetWindowRect(&wininfo->rect);
        ScreenToClient(&wininfo->rect);
    }
    if (m_FrmCur->ItemCntr) {
        wininfo->next=m_FrmCur->ptr;
    } else m_ListEnd=wininfo;
    m_FrmCur->ptr=wininfo;
    ++m_FrmCur->ItemCntr;
}

void FRM_ViewEdit::FRM_FreeAll() {
    struct FRM_frmInfo *wininfo;

    if (!m_FrmCur) return;

    for(wininfo=m_FrmCur->ptr;wininfo;wininfo=m_ListCur) {
        m_ListCur=wininfo->next;
        delete wininfo;
    }
    m_FrmCur->ptr=NULL;
}

void FRM_ViewEdit::FRM_FreeItem() {
    struct FRM_frmInfo *wininfo;

    if (!m_FrmCur || !m_ListCur) return;

    for(wininfo=m_FrmCur->ptr;wininfo && wininfo->next!=m_ListCur;wininfo=wininfo->next);
    if (wininfo) {
        wininfo->next=m_ListCur->next;
        delete m_ListCur;
        m_ListCur=wininfo;
    } else if (m_FrmCur->ptr==m_ListCur) {
        if (m_ListCur->next) m_FrmCur->ptr=m_ListCur->next; else m_FrmCur->ptr=NULL;
        delete m_ListCur;
        m_ListCur=m_FrmCur->ptr;
    }
    --m_FrmCur->ItemCntr;
}

void FRM_ViewEdit::FRM_AddSelHead() {
    if (!m_FrmCur || !m_ListCur) return;

    if (m_selcntr) {
		FRM_frmInfo *temp;
		temp = new struct FRM_frmInfo *[m_selcntr + 1]
		short i;
		for (i = 0; i < m_selcntr; i++)
			temp[i] = m_SelList[i];
        delete [] m_SelList;
		m_SelList = temp;
    } else {
        m_SelList= new struct FRM_frmInfo *[1];
    }
    m_SelList[m_selcntr]=m_ListCur;
    ++m_selcntr;
}

void FRM_ViewEdit::FRM_FreeSelList() {
    if (m_selcntr) {
        delete []  m_SelList;
        m_selcntr=0;
    }
}

int FRM_ViewEdit::GetWindowType() {
    short fi1;
    long  nID;
    CWnd *hCurItem;

    if (!m_FrmCur)  return 0;
    if (!m_ListCur) return 0;
     nID=GetWindowLong(m_mdialog->m_hWnd,GWL_ID);
    if (nID==m_ListCur->Wid) return 0;
    hCurItem=m_mdialog->GetDlgItem(m_ListCur->Wid);
    GetClassName(hCurItem->m_hWnd,FRM_szText,255);
    for(fi1=1;fi1<FRM_type_cnt;++fi1) {
        if (strisame(FRM_tabinfo[fi1].CName,FRM_szText)) break;
    }
    return(fi1);
}

void sortlist(short cntr) {
   qsort((void *)FRM_Listptr,(size_t)cntr,sizeof(struct FRM_frmInfo *),FRM_ItemLocSort);
}

int FRM_ItemLocSort(const void *arg1,const void *arg2) {
    int retvalue;

    switch(FRM_Sorttype) {
        case 0:
            retvalue=(*((struct FRM_frmInfo **)arg1))->rect.left-(*((struct FRM_frmInfo **)arg2))->rect.left;
            break;
        case 1:
            retvalue=(*((struct FRM_frmInfo **)arg1))->rect.top-(*((struct FRM_frmInfo **)arg2))->rect.top;
            break;
    }

    return retvalue;
}

// ===========================================================================================================================================
// ====================================================== FRM_STATUSBAR CLASS FUNCTIONS ======================================================
// ===========================================================================================================================================

IMPLEMENT_DYNCREATE(FRM_StatusBar,CStatusBar)

BEGIN_MESSAGE_MAP(FRM_StatusBar,CStatusBar)
END_MESSAGE_MAP()

void FRM_StatusBar::UpdateBar(RECT rect) {
    m_rectinfo.left  =rect.left;
    m_rectinfo.top   =rect.top;
    m_rectinfo.right =(rect.right-rect.left);
    m_rectinfo.bottom=(rect.bottom-rect.top);

    m_recttl.left+=16;
    InvalidateRect(m_recttl);
    m_rectwh.left+=16;
    InvalidateRect(m_rectwh);
}

void FRM_StatusBar::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) {
    HDC      hCmpDC;
    HBITMAP  hBitmap;
    int      len;
    CDC      dc;

    CRect rect(&lpDrawItemStruct->rcItem);

    switch(lpDrawItemStruct->itemID) {
        case 1:
            m_recttl=lpDrawItemStruct->rcItem;
            dc.Attach(lpDrawItemStruct->hDC);
            hCmpDC=CreateCompatibleDC(lpDrawItemStruct->hDC);
            hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(FRM_TL_CORNER));
            SelectObject(hCmpDC,hBitmap);
            BitBlt(lpDrawItemStruct->hDC,rect.left,rect.top,16,16,hCmpDC,0,0,SRCCOPY);
            DeleteDC(hCmpDC);
            len=sprintf(FRM_szText,ResourceString(IDC_ICADFORM__D__D_34, "%d,%d" ),m_rectinfo.left,m_rectinfo.top);
            dc.SetBkColor(::GetSysColor(COLOR_BTNFACE));
            dc.TextOut(rect.left+20,rect.top,FRM_szText,len);
            dc.Detach();
            return;
            break;
        case 2:
            m_rectwh=lpDrawItemStruct->rcItem;
            dc.Attach(lpDrawItemStruct->hDC);
            hCmpDC=CreateCompatibleDC(lpDrawItemStruct->hDC);
            hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(FRM_WIDTH_HEIGHT));
            SelectObject(hCmpDC,hBitmap);
            BitBlt(lpDrawItemStruct->hDC,rect.left,rect.top,16,16,hCmpDC,0,0,SRCCOPY);
            DeleteDC(hCmpDC);
            len=sprintf(FRM_szText,ResourceString(IDC_ICADFORM__D__D_34, "%d,%d" ),m_rectinfo.right,m_rectinfo.bottom);
            dc.SetBkColor(::GetSysColor(COLOR_BTNFACE));
            dc.TextOut(rect.left+20,rect.top,FRM_szText,len);
            dc.Detach();
            return;
            break;
    }
    CStatusBar::DrawItem(lpDrawItemStruct);
}

// ===========================================================================================================================================
// ====================================================== FRM_FORMEDIT CLASS FUNCTIONS =======================================================
// ===========================================================================================================================================

IMPLEMENT_DYNCREATE(FRM_FormEdit,CFrameWnd)

BEGIN_MESSAGE_MAP(FRM_FormEdit,CFrameWnd)
    ON_COMMAND_RANGE(FRM_FORM_ARROW,FRM_FORM_LISTBOX,FormControls)
	ON_WM_SIZE()
    ON_WM_DESTROY()
    ON_WM_CREATE()
    ON_WM_KEYDOWN()
    ON_WM_KEYUP()
    ON_WM_CLOSE()
END_MESSAGE_MAP()

BOOL FRM_FormEdit::PreTranslateMessage( MSG* pMsg ) {
    if (pMsg->message==WM_KEYDOWN) {
        switch(pMsg->wParam) {
		    case VK_LEFT:
		    case VK_UP:
		    case VK_RIGHT:
		    case VK_DOWN:
			    m_DlgWin->OnMoveItems(pMsg->wParam);
			    break;
            case VK_DELETE:
                m_DlgWin->OnDeleteItem();
                break;
        }
    }
    return(CFrameWnd::PreTranslateMessage(pMsg));
}

void FRM_FormEdit::OnKeyDown(UINT nChar,UINT nRepCnt,UINT nFlags) {
    CFrameWnd::OnKeyDown(nChar,nRepCnt,nFlags);
    switch(nChar) {
        case FRM_VK_c:
        case FRM_VK_C:
            if (m_DlgWin->m_CtrlFlag) m_DlgWin->OnCopy(FALSE); else m_DlgWin->m_nChar=nChar; m_DlgWin->PropertyDlg();
            break;
        case FRM_VK_x:
        case FRM_VK_X:
            if (m_DlgWin->m_CtrlFlag) m_DlgWin->OnCopy(TRUE); else m_DlgWin->m_nChar=nChar; m_DlgWin->PropertyDlg();
            break;
        case FRM_VK_v:
        case FRM_VK_V:
            if (m_DlgWin->m_CtrlFlag) m_DlgWin->OnPaste(); else m_DlgWin->m_nChar=nChar; m_DlgWin->PropertyDlg();
            break;
        case FRM_VK_a:
		case FRM_VK_A:
			if (m_DlgWin->m_CtrlFlag) m_DlgWin->OnSelectAll(); else m_DlgWin->m_nChar=nChar; m_DlgWin->PropertyDlg();
			break;
        case VK_SHIFT:
        case VK_CONTROL:
            m_DlgWin->m_CtrlFlag=TRUE;
            break;
        default:
            if (isalpha((unsigned char) nChar)) {
                m_DlgWin->m_nChar=nChar;
                m_DlgWin->PropertyDlg();
            }
            break;
    }
}

void FRM_FormEdit::OnKeyUp(UINT nChar,UINT nRepCnt,UINT nFlags) {
    CFrameWnd::OnKeyUp(nChar,nRepCnt,nFlags);

    switch(nChar) {
        case VK_SHIFT:
        case VK_CONTROL:
            m_DlgWin->m_CtrlFlag=FALSE;
            break;
    }
}

int FRM_FormEdit::OnCreate(LPCREATESTRUCT lpCreateStruct) {
    CRect         rect;
    struct resbuf rbb;
    short         fi2,
                  fi1;
    long          style;

	if (CFrameWnd::OnCreate(lpCreateStruct)==-1) return -1;

    if (!m_StatusBar.Create(this)) return FALSE;
    if (!m_StatusBar.SetIndicators(indicators,sizeof(indicators)/sizeof(UINT)))	return FALSE;
    m_StatusBar.SetPaneInfo(m_StatusBar.CommandToIndex(FRM_TLPOS_INDICATOR),FRM_TLPOS_INDICATOR,0,70);
    m_StatusBar.SetPaneInfo(m_StatusBar.CommandToIndex(FRM_WHLEN_INDICATOR),FRM_WHLEN_INDICATOR,0,70);

    CStatusBarCtrl &SCtrl=m_StatusBar.GetStatusBarCtrl();
    SCtrl.SetText(0,m_StatusBar.CommandToIndex(FRM_TLPOS_INDICATOR),SBT_OWNERDRAW);
    SCtrl.SetText(0,m_StatusBar.CommandToIndex(FRM_WHLEN_INDICATOR),SBT_OWNERDRAW);

    EnableDocking(CBRS_ALIGN_ANY);

   	if (!m_MToolBar.Create(this,WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC,100)) return FALSE;
    if (!m_MToolBar.LoadBitmap(FORM_TBAR_MAIN)) return FALSE;
    if (!m_MToolBar.SetButtons(FRM_FormMBId,sizeof(FRM_FormMBId)/sizeof(UINT))) return FALSE;

   	if (!m_FToolBar.Create(this,WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC,101)) return FALSE;
    if (!m_FToolBar.LoadBitmap(FORM_TOOLBAR)) return FALSE;
    if (!m_FToolBar.SetButtons(FRM_FormFBId,sizeof(FRM_FormFBId)/sizeof(UINT))) return FALSE;
    for(fi2=0,fi1=(sizeof(FRM_FormFBId)/sizeof(UINT));fi2<fi1;++fi2) {
        style=m_FToolBar.GetButtonStyle(fi2);
		if (fi2) style|=TBBS_CHECKBOX; else style|=TBBS_CHECKGROUP;
        m_FToolBar.SetButtonStyle(fi2,style);
    }

    m_MToolBar.SetWindowText(_T(ResourceString(IDC_ICADFORM_DIALOG_1, "Dialog" )));
    m_MToolBar.EnableDocking(CBRS_ALIGN_ANY);
    m_FToolBar.SetWindowText(_T(ResourceString(IDC_ICADFORM_CONTROLS_35, "Controls" )));
    m_FToolBar.EnableDocking(CBRS_ALIGN_ANY);

	//*** If the registry does not exist, dispay default toolbars and return FALSE.
	CWinApp* pApp = AfxGetApp();
	if(pApp->GetProfileInt(ResourceString(IDC_ICADFORM_FORMTOOLBAR_SU_36, "formtoolbar-Summary" ),ResourceString(IDC_ICADFORM_BARS_37, "Bars" ),0)==0 &&
		pApp->GetProfileInt(ResourceString(IDC_ICADFORM_FORMTOOLBAR_SU_36, "formtoolbar-Summary" ),ResourceString(IDC_ICADFORM_SCREENCX_38, "ScreenCX" ),0)==0 && pApp->GetProfileInt(ResourceString(IDC_ICADFORM_FORMTOOLBAR_SU_36, "formtoolbar-Summary" ),ResourceString(IDC_ICADFORM_SCREENCY_39, "ScreenCY" ),0)==0) {
    	DockControlBar(&m_MToolBar,AFX_IDW_DOCKBAR_TOP);
    	DockControlBar(&m_FToolBar,AFX_IDW_DOCKBAR_TOP);
    } else LoadBarState(FRM_TOOLBAR_STATE);

    CToolBarCtrl &TCtrl=m_FToolBar.GetToolBarCtrl();
    TCtrl.CheckButton(m_FToolBar.GetItemID(0));

    sds_getvar("WNDPFORM"/*DNT*/,&rbb);
    if (rbb.resval.rpoint[0] || rbb.resval.rpoint[1]) {
        rect.left=(long)rbb.resval.rpoint[0];
        rect.top =(long)rbb.resval.rpoint[1];
    } else { rect.top=20; rect.bottom=400; rect.left=20; rect.right=400; }

    sds_getvar("WNDSFORM"/*DNT*/,&rbb);
    if (rbb.resval.rpoint[0] || rbb.resval.rpoint[1]) { rect.right =(long)rbb.resval.rpoint[0]; rect.bottom=(long)rbb.resval.rpoint[1]; }

    MoveWindow(&rect);
    return 0;
}

void FRM_FormEdit::FormControls(UINT Id) {
	short fi1,
		  fi2;
	UINT  nId;

    CToolBarCtrl &TCtrl=m_FToolBar.GetToolBarCtrl();

    switch(Id) {
		case FRM_FORM_CUT:    m_DlgWin->OnCopy(TRUE);  break;
        case FRM_FORM_PASTE:  m_DlgWin->OnPaste();     break;
        case FRM_FORM_COPY:   m_DlgWin->OnCopy(FALSE); break;
        case FRM_FORM_NEW:    m_DlgWin->OnNewDlg();    break;
        case FRM_FORM_TEST:   m_DlgWin->OnTestDlg();   break;
        case FRM_FORM_LALIGN:
        case FRM_FORM_RALIGN:
        case FRM_FORM_TALIGN:
        case FRM_FORM_BALIGN:
        case FRM_FORM_SWIDTH:
        case FRM_FORM_SHEIGHT:
        case FRM_FORM_SSIZE:
        case FRM_FORM_HSACROS:
        case FRM_FORM_VSACROS:
        case FRM_FORM_VCENT:
        case FRM_FORM_HCENT:  m_DlgWin->FRM_OnAlign(Id); break;
        default:
		    if (!TCtrl.IsButtonChecked(Id)) return;
		    for(fi2=0,fi1=(sizeof(FRM_FormFBId)/sizeof(UINT));fi2<fi1;++fi2) {
				nId=m_FToolBar.GetItemID(fi2);
				if (nId==Id) continue;
				if (TCtrl.IsButtonChecked(nId)) { TCtrl.CheckButton(nId,FALSE); break; }
			}
            m_DlgWin->m_Controltype=Id;
            break;
    }
}

BOOL FRM_FormEdit::OnCreateClient(LPCREATESTRUCT lpcs,CCreateContext* pContext) {
    CRect rectScrn,
          rect;

    FRM_Parent=this;
    if (!m_splitWnd.CreateStatic(this,1,2)) return FALSE;
    if (!m_splitWnd.CreateView(0,0,RUNTIME_CLASS(CWnd),CSize(0,0),pContext)) return FALSE;
    if (!m_splitWnd.CreateView(0,1,RUNTIME_CLASS(FRM_ViewEdit),CSize(100,0),pContext)) return FALSE;
    m_DlgWin = (FRM_ViewEdit *)m_splitWnd.GetPane(0,1);

    return TRUE;
}

void FRM_FormEdit::OnClose() {
    if (m_DlgWin->m_FrmCur && !m_DlgWin->m_FrmCur->hdcCompat) m_DlgWin->MakePicture(m_DlgWin->m_mdialog);
    m_DlgWin->SaveDlgChanges();
    SaveBarState(FRM_TOOLBAR_STATE);
    
	::SetWindowPos(IcadSharedGlobals::GetMainHWND(),HWND_TOP,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
    
    CFrameWnd::OnClose();
}

void FRM_FormEdit::OnDestroy() {
    struct resbuf rbb;
    RECT          rect;
    long          wparam;

    GetWindowRect(&rect);

    rbb.restype         =RTPOINT;
    rbb.resval.rpoint[0]=rect.left;
    rbb.resval.rpoint[1]=rect.top;
    sds_setvar("WNDPFORM"/*DNT*/,&rbb);

    rbb.resval.rpoint[0]=rect.right;
    rbb.resval.rpoint[1]=rect.bottom;
    sds_setvar("WNDSFORM"/*DNT*/,&rbb);

    CFrameWnd::OnDestroy();

    m_DlgWin->DestroyWindow();

    wparam=MAKEWPARAM(0,FRM_CLOSEING);
    m_Parent->SendMessage(WM_COMMAND,wparam,0);
}

void FRM_FormEdit::OnSize(UINT nType, int cx, int cy) {
	CFrameWnd::OnSize(nType,cx,cy);
}

// ======================================================================================================================================
// ====================================================== NON CLASS FUNCTIONS ===========================================================
// ======================================================================================================================================

BOOL FRM_dcltoform(char *fs1,HWND hWnd) {
	struct dlg_DlgActionTile	  *CurDlg;
	struct dlg_DlgActionTileFile *CurFile,
                                  *FileBeg,
                                  *FileCur;
    struct dlg_hDlgList          *hDlgBeg,
                                  *hDlgCur;
    int                            dclid;
    void                          *hGlbl=NULL;
    CByteArray                     pArray;
	long	                       li1,
                                   bytecount;

    // This part is from (LoadDcl)
    hDlgBeg=(dlg_hDlgList *)dlg_RetPtr(1);
	if (dlg_ReadDLGFile(hDlgBeg->hDlg,fs1,FALSE)!=TRUE) return FALSE;
    FileCur=(dlg_DlgActionTileFile *)dlg_RetPtr(2);
	dclid=FileCur->FileID;

    // This part is from (New dcl).
	// Find the DCL file in the DCL linklist.
    FileBeg=(dlg_DlgActionTileFile *)dlg_RetPtr(3);
	for(CurFile=FileBeg;CurFile!=NULL;CurFile=CurFile->Next) {
		if (CurFile->FileID==dclid) break;
	}
	if (!CurFile) return FALSE;

	for(CurDlg=CurFile->DlgBeg;CurDlg!=NULL;CurDlg=CurDlg->Next) {
        hDlgCur=(dlg_hDlgList *)dlg_RetPtr(0);
    	dlg_TreeDim(CurDlg,(HWND)hDlgCur->hDlg,FALSE);
	    // Build the dialog box template in memory.
        CurDlg->DlgItemPtr->lStyle=WS_DISABLED|WS_CHILD|WS_CLIPSIBLINGS|WS_BORDER|WS_DLGFRAME|0x000000C4;
	    if (dlg_BuildDynamicDlgBox(CurDlg->DlgItemPtr,&hGlbl,NULL,&CurDlg->DlgSize,&bytecount,FALSE)==FALSE) return FALSE;
        if (!FRM_NewTableQuery(ResourceString(IDC_ICADFORM_SELECT___FROM__22, "Select * from Forminfo" ))) return FALSE;
        pArray.SetSize(CurDlg->DlgSize);
        for(li1=0;li1<CurDlg->DlgSize;++li1) pArray[li1]=((BYTE)((BYTE *)hGlbl)[li1]);

        COleVariant  formname(CurDlg->DlgItemPtr->IDName,VT_BSTRT);
        COleVariant  datalen(CurDlg->DlgSize,VT_I4);
        COleVariant  datavalue=pArray;

        SDS_CMainWindow->m_CdatarecordPtr->AddNew();
	    SDS_CMainWindow->m_CdatarecordPtr->SetFieldValue(ResourceString(IDC_ICADFORM_FORMNAME_14, "FormName" ),formname);
	    SDS_CMainWindow->m_CdatarecordPtr->SetFieldValue(ResourceString(IDC_ICADFORM_FORMSIZE_25, "FormSize" ),datalen);
	    SDS_CMainWindow->m_CdatarecordPtr->SetFieldValue(ResourceString(IDC_ICADFORM_FORMDEFINITION_24, "FormDefinition" ),datavalue);
	    SDS_CMainWindow->m_CdatarecordPtr->Update();
        SDS_CMainWindow->m_CdatarecordPtr->MoveLast();
    }
    dlg_unload_dialog(dclid);
    sprintf(FRM_szText,ResourceString(IDC_ICADFORM_DONE_IMPORTING_42, "Done importing from file %s" ),fs1);
    MessageBox(hWnd,FRM_szText,ResourceString(IDC_ICADFORM_FORM_DESIGNER_19, "Form Designer" ),MB_ICONEXCLAMATION|MB_APPLMODAL);
    return TRUE;
}

void DeleteForm(CString name) {
    COleVariant    oleint;
    CDaoRecordset *propvalsptr;
    CString        String;
    FRM_NewTableQuery("Select * from Forminfo"/*DNT*/);

    String.Format(ResourceString(IDC_ICADFORM_FORMNAME___S__23, "FormName='%s'" ),(LPCTSTR)name);
    if (SDS_CMainWindow->m_CdatarecordPtr->Find(AFX_DAO_FIRST,String)) {
        oleint=SDS_CMainWindow->m_CdatarecordPtr->GetFieldValue("Fkey"/*DNT*/);
        sprintf(FRM_szText,"Select * from Itempropvals where Fkey=%d"/*DNT*/,oleint.lVal);
        propvalsptr = new CDaoRecordset(&SDS_CMainWindow->m_CdatabasePtr);
        propvalsptr->Open(dbOpenDynaset,FRM_szText);
        while(!propvalsptr->IsBOF()) {
            propvalsptr->Delete();
            propvalsptr->MoveNext();
        }
        delete propvalsptr;
        SDS_CMainWindow->m_CdatarecordPtr->Delete();
    }
}

//=======================================================================================================================================================
//=========================================================== GLOBAL INLINE FUNCTIONS ===================================================================
//=======================================================================================================================================================

inline void VarToBinay(BYTE **c, COleVariant *v) {
  if(v->vt==8209) {    
	*c = new BYTE [v->parray->rgsabound->cElements];	
    memcpy(*c,(BYTE *)v->parray->pvData,v->parray->rgsabound->cElements);
  } else
    *c = NULL;
}

inline void VarToCStr(CString *c, COleVariant *v) {
  if(v->vt==VT_BSTR)
    *c = (LPCTSTR)v->bstrVal;
  else
    *c = _T(""/*DNT*/);
}

#endif

