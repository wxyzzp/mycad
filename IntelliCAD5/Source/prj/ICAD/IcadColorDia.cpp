/* D:\DEV\PRJ\ICAD\ICADCOLORDIA.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

//*********************************************************************
//*  IcadColorDia.cpp                                                 *
//*  Copyright (C) 1994,1995, 1996, 1997 by Visio, Inc.               *
//*                                                                   *
//*********************************************************************

//This function takes 3 parameters.
//First and Second are current and picked colors respectivly
//The third is the mode.
// 0:This will display the standard dialog with BYLAYER and BYBLOCK
// 1:This will display the standard dialog with BYBLOCK removed and BYLAYER changed to black.
//   This is used for the screen background color.
// 2:This will display the standard dialog with BYLAYER,BYBLOCK and Varies.
//   This is used for the EntProperties dialog.
// 3:This will be used for cursor colors. Similar to 1.
// 4:This will display the standard dialog with BYBLOCK and BYLAYER removed

//** Includes
#include "Icad.h"/*DNT*/
#include "IcadApi.h"
#include "resource.hm"/*DNT*/
#include "paths.h"
#include "IcadDialog.h"

//** Defines
#define BYBLOCK_COLOR     0
#define FIRST_COLOR       1
#define LAST_COLOR        255
#define BYLAYER_COLOR     256
#define VARIES_COLOR      258
#define ID_COLOR_OFFSET   10000   //This is the resource ID offset

BOOL col_DblClick;
int PassedMode;
int bgCol;
int col_Current;
WNDPROC col_WndProc;
void col_SubClassItems(CDialog *Cdlg); //This is our main subclass function
LRESULT CALLBACK col_ButtonProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam); //This is our Button Proc for processing button clicks

//This is the dialog class
class col_Color2:public CDialog {
    public:
        BOOL OnInitDialog();
        col_Color2();
};

//We will subclass the dialog in the OnInit
BOOL col_Color2::OnInitDialog() {
    col_SubClassItems(this);
    CDialog::OnInitDialog();
    return(TRUE);
}

col_Color2::col_Color2():CDialog(IDD_COLOR2){
}

//This is the dialog class
class col_Color:public IcadDialog {
    public:
        BOOL OnInitDialog();
        col_Color();
        col_Color2 CColor2;
    protected:
		//{{AFX_MSG(col_Color)
		afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
		afx_msg void OnHelp();
		virtual void DoDataExchange(CDataExchange *pDX);
		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
		afx_msg void OnclickedOk();
		//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(col_Color,CDialog)
	//{{AFX_MSG_MAP(col_Color)
    ON_WM_CREATE()
    ON_BN_CLICKED(IDHELP,OnHelp)
	ON_WM_HELPINFO()
	ON_WM_ACTIVATE()
	ON_BN_CLICKED(IDOK, OnclickedOk)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL col_Color::OnHelpInfo(HELPINFO *pHel){
	OnHelp();
	return(true);
}

void col_Color::OnHelp() {
	CommandRefHelp( m_hWnd, HLP_SETCOLOR);
}

void col_Color::DoDataExchange(CDataExchange *pDX) 
	{
    DDX_Text(pDX,COL_NUMBER,col_Current);
    if(PassedMode==2)
		{
        DDV_MinMaxInt(pDX,col_Current,0,258);
        if(col_Current==257)
			{
            col_Current=256;
			}
		}
	else if (PassedMode==4)
		{
        DDV_MinMaxInt(pDX,col_Current,1,255);
		}
	else
		{
        DDV_MinMaxInt(pDX,col_Current,0,256);
		}
    }

//We will subclass the dialog in the OnInit
BOOL col_Color::OnInitDialog() {
    char szTmp[10];
    HWND hItemWnd;
    struct resbuf rb;
    col_SubClassItems(this);
    IcadDialog::OnInitDialog();

	if(PassedMode==0) {
		::EnableWindow(::GetDlgItem(m_hWnd,COL_258),FALSE);
		::ShowWindow(::GetDlgItem(m_hWnd,COL_258),SW_HIDE);
	}else if(PassedMode==1||PassedMode==3) {
		// Disable ByBlock button
		::EnableWindow(::GetDlgItem(m_hWnd,COL_0),FALSE);
		::ShowWindow(::GetDlgItem(m_hWnd,COL_0),SW_HIDE);
		::EnableWindow(::GetDlgItem(m_hWnd,COL_258),FALSE);
		::ShowWindow(::GetDlgItem(m_hWnd,COL_258),SW_HIDE);

		// Change the text on the group box for ByLayer button (which gets turned to black)
		::SetWindowText(::GetDlgItem(m_hWnd,IDC_LOGICAL_GROUP), ResourceString(IDC_COLOR_BLACK2, "Black" ));

		// Enlarge ByLayer/Black button to fill the group box
		RECT rc;
		CWnd  *pWnd = NULL;
		pWnd = GetDlgItem(COL_256);
		if (pWnd) {
			pWnd->GetWindowRect(&rc);
			ScreenToClient(&rc);
			rc.right += (rc.right-rc.left) * 1.35;
			pWnd->MoveWindow(&rc, true);
		}
	}
	//Modified SAU 31/05/2000 [
	//Reason:If you give acad_colordlg a nil second argument
	//		 it is supposed to disable the BYBLOCK and BYLAYER buttons.
	else if(PassedMode==6) {
		::ShowWindow(::GetDlgItem(m_hWnd,IDC_LOGICAL_GROUP),SW_SHOW);
		::EnableWindow(::GetDlgItem(m_hWnd,IDC_LOGICAL_GROUP),FALSE);
		::ShowWindow(::GetDlgItem(m_hWnd,COL_0),SW_SHOW);
		::EnableWindow(::GetDlgItem(m_hWnd,COL_0),FALSE);
		::ShowWindow(::GetDlgItem(m_hWnd,COL_256),SW_SHOW);
		::EnableWindow(::GetDlgItem(m_hWnd,COL_256 ),FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd,COL_258),FALSE);
		::ShowWindow(::GetDlgItem(m_hWnd,COL_258),SW_HIDE);
		//Modified SAU 31/05/2000  ]
	}else if(PassedMode==2) {
		::EnableWindow(::GetDlgItem(m_hWnd,COL_258),FALSE);
		::ShowWindow(::GetDlgItem(m_hWnd,COL_258),SW_HIDE);
	}else if(PassedMode==4) {
		::EnableWindow(::GetDlgItem(m_hWnd,COL_258),FALSE);
		::ShowWindow(::GetDlgItem(m_hWnd,COL_258),SW_HIDE);
		::EnableWindow(::GetDlgItem(m_hWnd,IDC_LOGICAL_GROUP),FALSE);
		::ShowWindow(::GetDlgItem(m_hWnd,IDC_LOGICAL_GROUP),SW_HIDE);
		::EnableWindow(::GetDlgItem(m_hWnd,COL_0),FALSE);
		::ShowWindow(::GetDlgItem(m_hWnd,COL_0),SW_HIDE);
		::EnableWindow(::GetDlgItem(m_hWnd,COL_256),FALSE);
		::ShowWindow(::GetDlgItem(m_hWnd,COL_256),SW_HIDE);
	}
	hItemWnd=::GetDlgItem(m_hWnd,COL_NUMBER);
	if(PassedMode==1) {
        sds_getvar("BKGCOLOR"/*DNT*/,&rb);
        bgCol=rb.resval.rint;
        itoa(bgCol,szTmp,10);
    }else{
        itoa(col_Current,szTmp,10);
    }
    ::SetWindowText(hItemWnd,szTmp);

    return(TRUE);
}

int col_Color::OnCreate(LPCREATESTRUCT lpCreateStruct) {
    int ret=CDialog::OnCreate(lpCreateStruct);

    if (!CColor2) CColor2.Create(IDD_COLOR2,this);
    CColor2.EnableWindow(TRUE);
    CColor2.ShowWindow(SW_SHOWNORMAL);
    return(ret);
}

col_Color::col_Color():IcadDialog(IDD_COLOR){
    col_DblClick=FALSE;
}

//***This is the main window function
int CMainWindow::IcadColorDia(int DefColor,int *CurColor, short Mode) {
    int ret;
    struct resbuf rb;

    PassedMode=Mode;  //Put the mode in a global to this file

    if(PassedMode==1) {
      sds_getvar("BKGCOLOR"/*DNT*/,&rb);
      col_Current=rb.resval.rint;
    }else{
        col_Current=DefColor;
    } 
	col_Color CColor;
    ret=CColor.DoModal();
    if(ret==IDOK || col_DblClick){
        col_DblClick=FALSE;
        *CurColor=col_Current;
        ret=IDOK;
    }else{
        *CurColor=DefColor;
    }

    return(ret);
}

//This is the subclass function that will intercept the button proc
void col_SubClassItems(CDialog *Cdlg) {
    _TCHAR fs1[81];
    CWnd   *hCurItem;

    if((hCurItem=Cdlg->GetWindow(GW_CHILD))==NULL) return;
    do {
        GetClassName(hCurItem->m_hWnd,fs1,81);
        if(_tcsicmp(fs1,"BUTTON"/*DNT*/)==0) {
            col_WndProc=(WNDPROC)SetWindowLong(hCurItem->m_hWnd,GWL_WNDPROC,(LONG)col_ButtonProc);
        }
    }while((hCurItem=hCurItem->GetNextWindow(GW_HWNDNEXT))!=NULL);
    return;
}
// ButtonProc()
LRESULT CALLBACK col_ButtonProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam) {
    HDC hDC;
    CBrush brush;
    RECT rect;
    int Id,ColorID;
    LRESULT ret;
    HWND hItemWnd,hWndDad;
    char szTmp[10];

    ret=CallWindowProc((WNDPROC)(long (__stdcall *)(void))col_WndProc,hWnd,uMsg,wParam,lParam);
    Id=GetWindowLong(hWnd,GWL_ID);
    if(Id==IDCANCEL||Id==IDHELP) return(ret);
    ColorID=Id-ID_COLOR_OFFSET;
        switch(uMsg) {
            case WM_LBUTTONDBLCLK:
                if((ColorID>=BYBLOCK_COLOR && ColorID<=BYLAYER_COLOR) || (ColorID==VARIES_COLOR && PassedMode==2)) {
                    col_Current=ColorID;
                    hWndDad=::GetParent(hWnd);
                    if(ColorID>=10 && ColorID<=249) hWndDad=::GetParent(hWndDad);
                    col_DblClick=TRUE;
                    ::SendMessage(hWndDad,WM_CLOSE,0,0);
                }
                break;
		//Modified SAU 31/05/2000	[
		//Reason:(Bug fixed)acad_colordlg doesn't update color swatch when you tab off the color field
			case WM_SETFOCUS:
				if(Id==COL_CURRENT)
				{
					hWndDad=::GetParent(hWnd);
					hItemWnd=GetDlgItem(hWndDad,COL_NUMBER);
					GetWindowText(hItemWnd,szTmp,10);
					ColorID=atoi(szTmp);
					
					if((ColorID>=BYBLOCK_COLOR && ColorID<=BYLAYER_COLOR) || (ColorID==VARIES_COLOR)) {
						col_Current=ColorID;
						if(col_Current==0) {
							if(PassedMode==6) {
								hDC=::GetDC(hWndDad);
								::GetClientRect(hWndDad,&rect);
								//Modified EBATECH(CNBR) 30/08/2001 Text control is wrong color. [
								//::SetBkColor(hDC,RGB(192,192,192));
								::SetBkColor(hDC,GetSysColor(COLOR_BTNFACE));
								//EBATECH(CNBR)
								TextOut(hDC,rect.left+50,rect.bottom-30,ResourceString(IDS_ICADCOLORDIA_INVALIDCOLOR,"Invalid Color"),13); 
								::ReleaseDC(hWndDad,hDC);
							}
							else if(PassedMode!=1) {
								hWndDad=::GetParent(hWnd);
								hItemWnd=GetDlgItem(hWndDad,COL_CURRENT);
								SetWindowText(hItemWnd,ResourceString(IDC_ICADCOLORDIA_BYBLOCK_3, "BYBLOCK" ));
							}
						}else if(col_Current==256) {
							if(PassedMode==6) {
								hDC=::GetDC(hWndDad);
								::GetClientRect(hWndDad,&rect);
								//Modified EBATECH(CNBR) 30/08/2001 Text control is wrong color. [
								//::SetBkColor(hDC,RGB(192,192,192));
								::SetBkColor(hDC,GetSysColor(COLOR_BTNFACE));
								//EBATECH(CNBR)
								TextOut(hDC,rect.left+50,rect.bottom-30,ResourceString(IDS_ICADCOLORDIA_INVALIDCOLOR,"Invalid Color"),13); 
								::ReleaseDC(hWndDad,hDC);
							}
							else if(PassedMode!=1&&PassedMode!=3) {
								hWndDad=::GetParent(hWnd);
								hItemWnd=GetDlgItem(hWndDad,COL_CURRENT);
								SetWindowText(hItemWnd,ResourceString(IDC_ICADCOLORDIA_BYLAYER_4, "BYLAYER" ));
							}else{
								hWndDad=::GetParent(hWnd);
								if(ColorID>=10 && ColorID<=249) hWndDad=::GetParent(hWndDad);
								hItemWnd=GetDlgItem(hWndDad,COL_CURRENT);
								hDC=::GetDC(hItemWnd);
								HPALETTE savpal=::SelectPalette(hDC,(HPALETTE)SDS_CMainWindow->m_pPalette->GetSafeHandle(),TRUE);
								brush.CreateSolidBrush(RGB(0,0,0));
								::GetClientRect(hItemWnd,&rect);
								::FillRect(hDC,&rect,(HBRUSH)brush);
								brush.DeleteObject();
								//								::SelectPalette(hDC,savpal,TRUE);
								::ReleaseDC(hItemWnd,hDC);
							}
						}else if(col_Current==257) {
							return(ret);
						}else if(col_Current==258) {
							if(PassedMode==2) {
								hWndDad=::GetParent(hWnd);
								hItemWnd=GetDlgItem(hWndDad,COL_CURRENT);
								SetWindowText(hItemWnd,ResourceString(IDC_ICADCOLORDIA_VARIES_5, "VARIES" ));
							}
						}else{
							hWndDad=::GetParent(hWnd);
							if(ColorID>=10 && ColorID<=249) hWndDad=::GetParent(hWndDad);
							hItemWnd=GetDlgItem(hWndDad,COL_CURRENT);
							hDC=::GetDC(hItemWnd);
							HPALETTE savpal=::SelectPalette(hDC,(HPALETTE)SDS_CMainWindow->m_pPalette->GetSafeHandle(),TRUE);
							if(PassedMode==1) {
								brush.CreateSolidBrush((bgCol==ColorID)?SDS_BrushColorFromACADColor(256):SDS_BrushColorFromACADColor(ColorID));
							}else{
								brush.CreateSolidBrush(SDS_BrushColorFromACADColor(ColorID));
							}
							::GetClientRect(hItemWnd,&rect);
							::FillRect(hDC,&rect,(HBRUSH)brush);
							brush.DeleteObject();
							//							::SelectPalette(hDC,savpal,TRUE);
							::ReleaseDC(hItemWnd,hDC);
						}
					}
					if(ColorID>=10 && ColorID<=249){
						::SetFocus(::GetDlgItem(::GetParent(::GetParent(hWnd)),IDOK));
					}else{
						::SetFocus(::GetDlgItem(::GetParent(hWnd),IDOK));
					}
				}	
					break;
			//Modified SAU 31/05/2000                      ]
            case WM_LBUTTONDOWN:
                if((ColorID>=BYBLOCK_COLOR && ColorID<=BYLAYER_COLOR) || (ColorID==VARIES_COLOR)) {
                    col_Current=ColorID;
                    if(col_Current==0) {
                        if(PassedMode!=1) {
                            hWndDad=::GetParent(hWnd);
                            hItemWnd=GetDlgItem(hWndDad,COL_CURRENT);
                            SetWindowText(hItemWnd,ResourceString(IDC_ICADCOLORDIA_BYBLOCK_3, "BYBLOCK" ));
                            hItemWnd=GetDlgItem(hWndDad,COL_NUMBER);
                            itoa(0,szTmp,10);
                            SetWindowText(hItemWnd,szTmp);
                        }
                    }else if(col_Current==256) {
                        if(PassedMode!=1&&PassedMode!=3) {
                            hWndDad=::GetParent(hWnd);
                            hItemWnd=GetDlgItem(hWndDad,COL_CURRENT);
                            SetWindowText(hItemWnd,ResourceString(IDC_ICADCOLORDIA_BYLAYER_4, "BYLAYER" ));
                            hItemWnd=GetDlgItem(hWndDad,COL_NUMBER);
                            itoa(256,szTmp,10);
                            SetWindowText(hItemWnd,szTmp);
                        }else{
                            hWndDad=::GetParent(hWnd);
                            if(ColorID>=10 && ColorID<=249) hWndDad=::GetParent(hWndDad);
                            hItemWnd=GetDlgItem(hWndDad,COL_CURRENT);
                            hDC=::GetDC(hItemWnd);
						    HPALETTE savpal=::SelectPalette(hDC,(HPALETTE)SDS_CMainWindow->m_pPalette->GetSafeHandle(),TRUE);
                            brush.CreateSolidBrush(RGB(0,0,0));
                            ::GetClientRect(hItemWnd,&rect);
                            ::FillRect(hDC,&rect,(HBRUSH)brush);
                            brush.DeleteObject();
//						    ::SelectPalette(hDC,savpal,TRUE);
                            ::ReleaseDC(hItemWnd,hDC);
                            hItemWnd=GetDlgItem(hWndDad,COL_NUMBER);
                            itoa(ColorID,szTmp,10);
                            SetWindowText(hItemWnd,szTmp);
                        }
                    }else if(col_Current==257) {
                        return(ret);
                    }else if(col_Current==258) {
                        if(PassedMode==2) {
                            hWndDad=::GetParent(hWnd);
                            hItemWnd=GetDlgItem(hWndDad,COL_CURRENT);
                            SetWindowText(hItemWnd,ResourceString(IDC_ICADCOLORDIA_VARIES_5, "VARIES" ));
                            hItemWnd=GetDlgItem(hWndDad,COL_NUMBER);
                            SetWindowText(hItemWnd,ResourceString(DNT_ICADCOLORDIA_____6, "###" ));
                        }
                    }else{
                        hWndDad=::GetParent(hWnd);
                        if(ColorID>=10 && ColorID<=249) hWndDad=::GetParent(hWndDad);
                        hItemWnd=GetDlgItem(hWndDad,COL_CURRENT);
                        hDC=::GetDC(hItemWnd);
						HPALETTE savpal=::SelectPalette(hDC,(HPALETTE)SDS_CMainWindow->m_pPalette->GetSafeHandle(),TRUE);
                        if(PassedMode==1) {
                            brush.CreateSolidBrush((bgCol==ColorID)?SDS_BrushColorFromACADColor(256):SDS_BrushColorFromACADColor(ColorID));
                        }else{
                            brush.CreateSolidBrush(SDS_BrushColorFromACADColor(ColorID));
                        }
                        ::GetClientRect(hItemWnd,&rect);
                        ::FillRect(hDC,&rect,(HBRUSH)brush);
                        brush.DeleteObject();
//						::SelectPalette(hDC,savpal,TRUE);
                        ::ReleaseDC(hItemWnd,hDC);
                        hItemWnd=GetDlgItem(hWndDad,COL_NUMBER);
                        itoa(ColorID,szTmp,10);
                        SetWindowText(hItemWnd,szTmp);
                    }
                }
                if(ColorID>=10 && ColorID<=249){
                    ::SetFocus(::GetDlgItem(::GetParent(::GetParent(hWnd)),IDOK));
                }else{
                    ::SetFocus(::GetDlgItem(::GetParent(hWnd),IDOK));
                }
            case WM_PAINT:
            case BM_SETSTATE:
                //This will paint the color buttons
                if(ColorID==0) {  //BYBLOCK button
                    SetWindowText(hWnd,ResourceString(IDC_ICADCOLORDIA_BYBLOCK_3, "BYBLOCK" ));
                }else if(ColorID>=1 && ColorID<=255) {
                    hDC=::GetDC(hWnd);
					HPALETTE savpal=::SelectPalette(hDC,(HPALETTE)SDS_CMainWindow->m_pPalette->GetSafeHandle(),TRUE);
                    if(PassedMode==1) {
                        brush.CreateSolidBrush((bgCol==ColorID)?SDS_BrushColorFromACADColor(256):SDS_BrushColorFromACADColor(ColorID));
                    }else{
                        brush.CreateSolidBrush(SDS_BrushColorFromACADColor(ColorID));
                    }
                    ::GetClientRect(hWnd,&rect);
                    ::FillRect(hDC,&rect,(HBRUSH)brush);
                    brush.DeleteObject();
//					::SelectPalette(hDC,savpal,TRUE);

                    ::ReleaseDC(hWnd,hDC);
                }else if(ColorID==256) {  //BYLAYER button
                    if(PassedMode==1||PassedMode==3) {
                        hDC=::GetDC(hWnd);
					    HPALETTE savpal=::SelectPalette(hDC,(HPALETTE)SDS_CMainWindow->m_pPalette->GetSafeHandle(),TRUE);
                        brush.CreateSolidBrush(RGB(0,0,0));
                        ::GetClientRect(hWnd,&rect);
                        ::FillRect(hDC,&rect,(HBRUSH)brush);
                        brush.DeleteObject();
//					    ::SelectPalette(hDC,savpal,TRUE);
                        ::ReleaseDC(hWnd,hDC);
                    }else{
                        SetWindowText(hWnd,ResourceString(IDC_ICADCOLORDIA_BYLAYER_4, "BYLAYER" ));
                    }
                }else if(ColorID==257) { //This is for the Current Color box
//Modified SAU 31/05/2000	[
//Reason:(Bug fixed)acad_colordlg doesn't update color swatch when you tab off the color field
					        hWndDad=::GetParent(hWnd);
        					hDC=::GetDC(hWndDad);
							GetClientRect(hWndDad,&rect);
							//Modified EBATECH(CNBR) 30/08/2001 Text control is wrong color. [
							//::SetBkColor(hDC,RGB(192,192,192));
							::SetBkColor(hDC,GetSysColor(COLOR_BTNFACE));
							//EBATECH(CNBR)
							TextOut(hDC,rect.left+50,rect.bottom-30,"                        ",24);
							::ReleaseDC(hWndDad,hDC);
                    if(col_Current==0) {
                       	if(PassedMode==6) {
							hWndDad=::GetParent(hWnd);
        					hDC=::GetDC(hWndDad);
							GetClientRect(hWndDad,&rect);
							//Modified EBATECH(CNBR) 30/08/2001 Text control is wrong color. [
							//::SetBkColor(hDC,RGB(192,192,192));
							::SetBkColor(hDC,GetSysColor(COLOR_BTNFACE));
							//EBATECH(CNBR)
							TextOut(hDC,rect.left+50,rect.bottom-30,ResourceString(IDS_ICADCOLORDIA_INVALIDCOLOR,"Invalid Color"),13); 
							::ReleaseDC(hWndDad,hDC);
						}
					else if(PassedMode==1||PassedMode==3) {
                            hDC=::GetDC(hWnd);
					        HPALETTE savpal=::SelectPalette(hDC,(HPALETTE)SDS_CMainWindow->m_pPalette->GetSafeHandle(),TRUE);
                            brush.CreateSolidBrush(SDS_BrushColorFromACADColor(256));
                            ::GetClientRect(hWnd,&rect);
                            ::FillRect(hDC,&rect,(HBRUSH)brush);
                            brush.DeleteObject();
//					        ::SelectPalette(hDC,savpal,TRUE);
                            ::ReleaseDC(hWnd,hDC);
                        }else{
                            SetWindowText(hWnd,ResourceString(IDC_ICADCOLORDIA_BYBLOCK_3, "BYBLOCK" ));
                        }
                    }else if(col_Current==256) {
						if(PassedMode==6) {
							hWndDad=::GetParent(hWnd);
        					hDC=::GetDC(hWndDad);
							GetClientRect(hWndDad,&rect);
							//Modified EBATECH(CNBR) 30/08/2001 Text control is wrong color. [
							//::SetBkColor(hDC,RGB(192,192,192));
							::SetBkColor(hDC,GetSysColor(COLOR_BTNFACE));
							//EBATECH(CNBR)
							TextOut(hDC,rect.left+50,rect.bottom-30,ResourceString(IDS_ICADCOLORDIA_INVALIDCOLOR,"Invalid Color"),13); 
							::ReleaseDC(hWndDad,hDC);
						}
//Modified SAU 31/05/2000	]
					else if(PassedMode==1||PassedMode==3) {
                            hDC=::GetDC(hWnd);
					        HPALETTE savpal=::SelectPalette(hDC,(HPALETTE)SDS_CMainWindow->m_pPalette->GetSafeHandle(),TRUE);
                            brush.CreateSolidBrush(RGB(0,0,0));
                            ::GetClientRect(hWnd,&rect);
                            ::FillRect(hDC,&rect,(HBRUSH)brush);
                            brush.DeleteObject();
//					        ::SelectPalette(hDC,savpal,TRUE);
                            ::ReleaseDC(hWnd,hDC);
                        }else{
                            SetWindowText(hWnd,ResourceString(IDC_ICADCOLORDIA_BYLAYER_4, "BYLAYER" ));
                        }
                    }else if(col_Current==258) {
                        if(PassedMode==2) {
                            SetWindowText(hWnd,ResourceString(IDC_ICADCOLORDIA_VARIES_5, "VARIES" ));
                        }
                    }else{
                        hDC=::GetDC(hWnd);
						HPALETTE savpal=::SelectPalette(hDC,(HPALETTE)SDS_CMainWindow->m_pPalette->GetSafeHandle(),TRUE);
                        if(PassedMode==1||PassedMode==3) {
                            brush.CreateSolidBrush((bgCol==col_Current)?SDS_BrushColorFromACADColor(256):SDS_BrushColorFromACADColor(col_Current));
                        }else{
                            brush.CreateSolidBrush(SDS_BrushColorFromACADColor(col_Current));
                        }
                        ::GetClientRect(hWnd,&rect);
                        ::FillRect(hDC,&rect,(HBRUSH)brush);
                        brush.DeleteObject();
//						::SelectPalette(hDC,savpal,TRUE);
                        ::ReleaseDC(hWnd,hDC);
                    }
                }else if(ColorID==258) { //VARIES button
                    if(PassedMode==2) {
                       SetWindowText(hWnd,ResourceString(IDC_ICADCOLORDIA_VARIES_5, "VARIES" ));
                    }
                }
                break;

        }
    return(ret);
}

void col_Color::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	SDS_CMainWindow->SetPalette(SDS_CMainWindow->m_pPalette);
	CDialog::OnActivate(nState,pWndOther,bMinimized);
}


//Modified SAU 31/05/2000	[
//Reason:(Bug fixed) acad_colordlg allows BYBLOCK/BYLAYER to be selected when it shouldn't
void col_Color::OnclickedOk() 
{
	// TODO: Add your control notification handler code here
	HDC hDC;
	RECT tempRect;
	if(PassedMode==6)
	{
		char szTmp[10];
		::GetWindowText(::GetDlgItem(m_hWnd,COL_NUMBER),szTmp,10);
		if((atoi(szTmp)==0) || (atoi(szTmp)==256)){
			hDC=::GetDC(m_hWnd);
			::GetClientRect(m_hWnd,&tempRect);
			//Modified EBATECH(CNBR) 30/08/2001 Text control is wrong color. [
			//::SetBkColor(hDC,RGB(192,192,192));
			::SetBkColor(hDC,GetSysColor(COLOR_BTNFACE));
			//EBATECH(CNBR)
			TextOut(hDC,tempRect.left+50,tempRect.bottom-30,ResourceString(IDS_ICADCOLORDIA_INVALIDCOLOR,"Invalid Color"),13); 
			::ReleaseDC(m_hWnd,hDC);
		}	 
		else CDialog::OnOK();
	}
	else CDialog::OnOK();
	
}
//Modified SAU 31/05/2000  ]
