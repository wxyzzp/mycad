/* F:\BLD\PRJ\ICAD\ICADEDITXDATA.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 *
 */

#include "Icad.h"/*DNT*/
#include "IcadApi.h"
#include "cmds.h"/*DNT*/
#include <time.h>
#include "resource.hm"/*DNT*/
#include "paths.h"
#include "IcadDialog.h"

#define XDATA_BEGIN              10
#define XDATA_APPNAME            11
#define XDATA_LAYERNAME          12
#define XDATA_DATABHAND          13
#define XDATA_3DPOINT            14
#define XDATA_REAL               15
#define XDATA_INTEGER            16
#define XDATA_LONG               17
#define XDATA_BIN_DATA           18
#define XDATA_WLD_POS            19
#define XDATA_WLD_DISP           20
#define XDATA_WLD_DIR            21
#define XDATA_WLD_DIST           22
#define XDATA_WLD_SCALE          23
#define XDATA_STRING             24
#define XDATA_BRACES             25
#define XDATA_END                900

// ======================================================================================================================================================================
// ============================================================== GLOBAL STRUCTURES =====================================================================================
// ======================================================================================================================================================================

struct Xdata {
    char          string[IC_ACADBUF];
    LPARAM        lParam;
    short         sIndex;
    struct Xdata *next;
} *Xdata_gptr;

// =============================================================================================================================================
// ================================================================ GLOBAL CLASSES =============================================================
// =============================================================================================================================================

class Xdata_Info : public IcadDialog {
    public :
        struct Xdata   *m_xdataBeg;
        short           m_cType,
                        m_indexcntr,
                        m_SelectedIndx;
        struct resbuf  *m_frb,
                        m_valuerb;
        sds_name        m_Xename;
        CTreeCtrl      *m_pXdataList;
	    CImageList     *m_pImageList;
        CToolTipCtrl    m_ToolTip;
        CPoint          m_point;
        long            m_Style;
        CWnd           *m_DelList;
        BOOL            m_editdata;

    public:
        void            Freedatalinks(void);
        Xdata_Info() : IcadDialog(XDATA_DIALOG)
			{
			m_xdataBeg = NULL;
			m_frb = NULL;
			m_pXdataList = NULL;
			m_pImageList = NULL;
			m_valuerb.resval.rstring = NULL;
			}
		virtual			~Xdata_Info();


    protected:
        afx_msg void OnHelp();
        struct Xdata*   Makexdatalink(void);
        HTREEITEM       AddOneItem(HTREEITEM hParent,LPCTSTR szText,int iImage,short index,int Type,HTREEITEM InsertAt);
        void            OnButtonInsertItem(void);
        HTREEITEM       MoveTree(HTREEITEM hCurItem,HTREEITEM hInsertAfter,HTREEITEM hInsertParent);
        void            OnContextMenuPick(UINT nId);
        struct resbuf*  SaveLinks(HTREEITEM hCurItem,struct resbuf **rb,BOOL bCloseflag);
        void            OnBeginDrag(NMHDR* pNMHDR, LRESULT* pResult);
        void            OnDelete(void);
        void            ONRename(void);
        void            OnOk(void);
        BOOL            OnSetCursor(CWnd* pWnd,UINT nHitTest,UINT message);
        BOOL		    OnInitDialog(void);
        void            AddxdataItems(void);
        void            OnDestroy(void);
        void            DrawInsertButton();
        virtual LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam );
        LRESULT         NotifyHandler(UINT message, WPARAM wParam, LPARAM lParam);

        DECLARE_MESSAGE_MAP()
};

Xdata_Info::~Xdata_Info()
	{
		Freedatalinks();
		if ( m_pImageList != NULL)
			{
			delete m_pImageList;
			m_pImageList = NULL;
			}
		if ( m_frb != NULL )
			{
            sds_relrb(m_frb);
			}
		// m_valuerb is freed as soon as it is used during the code
		// so don't touch it here
		//
	}

// ======================================================================================================================================================================
// ======================================================= XDATA_INFO CLASS FUNCTIONS =============================================================================
// ======================================================================================================================================================================

BEGIN_MESSAGE_MAP(Xdata_Info,CDialog)
	ON_NOTIFY(TVN_BEGINDRAG,XDATA_TREE,OnBeginDrag)
    ON_BN_CLICKED(XDATA_DELETE,OnDelete)
    ON_BN_CLICKED(XDATA_RENAME,ONRename)
    ON_BN_CLICKED(IDOK,OnOk)
    ON_BN_CLICKED(XDATA_INSERT,OnButtonInsertItem)
	ON_COMMAND_RANGE(XDATA_BEGIN,XDATA_END,OnContextMenuPick)
    ON_WM_SETCURSOR()
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDHELP,OnHelp)
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////
// This function is called on the initalization of the "Xdata_Info" Dialog.
//
// Returns :
//
BOOL Xdata_Info::OnInitDialog() {
    HTREEITEM hItem;
    CRect     rect(12,12,300,340);

    m_DelList=GetDlgItem(XDATA_DELETE);

    m_Style=0;
    m_editdata=FALSE;
    m_indexcntr=0;
    m_xdataBeg =NULL;
	m_SelectedIndx=0; // added initializer

    // Get the xtree list control.
    m_pXdataList=(CTreeCtrl *)GetDlgItem(XDATA_TREE);

	//*** Setup the image list for the visibility of menu items.
	m_pImageList = new CImageList();
	m_pImageList->Create(IDB_ICAD_XDATA_ALL,16,0,RGB(255,255,255));
    m_pXdataList->SetImageList(m_pImageList,TVSIL_NORMAL);
    AddxdataItems();
    hItem=m_pXdataList->GetRootItem();
    if (hItem) m_pXdataList->SelectItem(hItem); else m_Style=MF_GRAYED;
    m_ToolTip.Create(this,TTS_ALWAYSTIP);
    m_ToolTip.AddTool(m_pXdataList,ResourceString(IDC_ICADEDITXDATA_THIS_IS_SO_0, "This is some text" ));
    DrawInsertButton();

	IcadDialog::OnInitDialog();

    return(TRUE);
}

void Xdata_Info::OnHelp() {
	CommandRefHelp( m_hWnd, HLP_EDITEDATA);
}

LRESULT Xdata_Info::WindowProc(UINT message,WPARAM wParam,LPARAM lParam) {
    CString    cstring;
    CEdit     *pEdit;

    switch(message) {
        case WM_COMMAND:
            switch(HIWORD(wParam)) {
                case BN_CLICKED:
                    if (m_editdata) {
                        pEdit=m_pXdataList->GetEditControl();
                        pEdit->DestroyWindow();
                        return(1);
                    }
                    break;
            }
            break;
        case WM_NOTIFY:
            NotifyHandler(message,wParam,lParam);
            break;
    }

    return CDialog::WindowProc(message,wParam,lParam);
}

LRESULT Xdata_Info::NotifyHandler(UINT message, WPARAM wParam, LPARAM lParam) {
    LPNMHDR         pNMhdr=(LPNMHDR)lParam;
    LPNMTVDISPINFO  pDispInfo=(LPNMTVDISPINFO)lParam;
    LPNMTVKEYDOWN   pnkd=(LPNMTVKEYDOWN)lParam;
    CEdit          *pEdit;
    HTREEITEM       hItem;
    long            lType;
    struct resbuf  *rbb;
    char           *fcp1;

    switch(pNMhdr->code) {
        case TVN_KEYDOWN:
            switch(pnkd->wVKey) {
                case VK_SPACE:
                    return(1);
            }
            break;
        case TVN_BEGINLABELEDIT:
            m_editdata=TRUE;
	        hItem=m_pXdataList->GetSelectedItem();
            pEdit=m_pXdataList->GetEditControl();
            Xdata_gptr=(struct Xdata *)m_pXdataList->GetItemData(hItem);
            if (Xdata_gptr->lParam==1002) {
                pEdit=m_pXdataList->GetEditControl();
                pEdit->DestroyWindow();
            }
            break;
        case TVN_ENDLABELEDIT:
            m_editdata=FALSE;
            pEdit=m_pXdataList->GetEditControl();
	        hItem=m_pXdataList->GetSelectedItem();
            Xdata_gptr=(struct Xdata *)m_pXdataList->GetItemData(hItem);
            switch(Xdata_gptr->lParam) {
                case XDATA_CODE_APPLICATION:
   	                if (pDispInfo->item.pszText && *(pDispInfo->item.pszText)!=0) {
                        if ((strchr(pDispInfo->item.pszText,' '))) {
                             m_pXdataList->EditLabel(hItem);
                        } else m_pXdataList->SetItemText(hItem,pDispInfo->item.pszText);
                    }
                    break;
                case XDATA_CODE_LAYER_NAME:
   	                if (pDispInfo->item.pszText && *(pDispInfo->item.pszText)!=0) {
                        if ((rbb=sds_tblsearch("LAYER"/*DNT*/,pDispInfo->item.pszText,0))==NULL)
							{
                             m_pXdataList->EditLabel(hItem);
	                        }
						else
							{
							m_pXdataList->SetItemText(hItem,pDispInfo->item.pszText);
							sds_relrb( rbb );
							}
                    } else if (Xdata_gptr->string) m_pXdataList->SetItemText(hItem,Xdata_gptr->string);
                    else m_pXdataList->EditLabel(hItem);
                    break;
                case XDATA_CODE_STRING:
   	                if (pDispInfo->item.pszText && *(pDispInfo->item.pszText)!=0) {
                        m_pXdataList->SetItemText(hItem,pDispInfo->item.pszText);
                        strcpy(Xdata_gptr->string,pDispInfo->item.pszText);
                    } else if (Xdata_gptr->string && *Xdata_gptr->string) {
                        m_pXdataList->SetItemText(hItem,Xdata_gptr->string);
                    } else m_pXdataList->EditLabel(hItem);
                    break;
                case XDATA_CODE_3DPOINT:
                case XDATA_CODE_WLD_POS:
                case XDATA_CODE_WLD_DISP:
                case XDATA_CODE_WLD_DIR:
                    if (pDispInfo->item.pszText && *(pDispInfo->item.pszText)!=0) {
                        fcp1=pDispInfo->item.pszText;
                        for(;isdigit((unsigned char) *(fcp1));++fcp1);
                        if (*fcp1!=0 && *fcp1=='.')
							for(++fcp1;isdigit((unsigned char) *(fcp1));++fcp1);
                        if (*fcp1!=',') { m_pXdataList->EditLabel(hItem); break; }
                        for(++fcp1;isdigit((unsigned char) *(fcp1));++fcp1);
                        if (*fcp1!=0 && *fcp1=='.')
							for(++fcp1;isdigit((unsigned char) *(fcp1));++fcp1);
                        if (*fcp1!=',') { m_pXdataList->EditLabel(hItem); break; }
                        for(++fcp1;isdigit((unsigned char) *(fcp1));++fcp1);
                        if (*fcp1!=0 && *fcp1=='.')
							for(++fcp1;isdigit((unsigned char) *(fcp1));++fcp1);
                        if (*fcp1!=0) { m_pXdataList->EditLabel(hItem); break; }
                        m_pXdataList->SetItemText(hItem,pDispInfo->item.pszText);
                    } else if (Xdata_gptr->string) m_pXdataList->SetItemText(hItem,Xdata_gptr->string);
                    else m_pXdataList->EditLabel(hItem);
                    break;
                case XDATA_CODE_REAL:
                    if (pDispInfo->item.pszText && *(pDispInfo->item.pszText)!=0) {
                        fcp1=pDispInfo->item.pszText;
                        if (!isdigit((unsigned char) *(fcp1)) && *fcp1!='-') { m_pXdataList->EditLabel(hItem); break; }
                        for(;isdigit((unsigned char) *(fcp1));++fcp1);
                        if (*fcp1!=0) {
                            if (*fcp1=='.') {
                                for(++fcp1;isdigit((unsigned char) *(fcp1));++fcp1);
                                if (*fcp1!=0) { m_pXdataList->EditLabel(hItem); break; }
                            } else { m_pXdataList->EditLabel(hItem); break; }
                        }
                        m_pXdataList->SetItemText(hItem,pDispInfo->item.pszText);
                    } else if (Xdata_gptr->string) m_pXdataList->SetItemText(hItem,Xdata_gptr->string);
                    else m_pXdataList->EditLabel(hItem);
                    break;
                case XDATA_CODE_INTEGER:
                    if (pDispInfo->item.pszText && *(pDispInfo->item.pszText)!=0) {
                        fcp1=pDispInfo->item.pszText;
                        if (!isdigit((unsigned char) *(fcp1)) && *fcp1!='-') { m_pXdataList->EditLabel(hItem); break; }
                        for(;isdigit((unsigned char) *(fcp1));++fcp1);
                        if (*fcp1!=0) { m_pXdataList->EditLabel(hItem); break; }
                        m_pXdataList->SetItemText(hItem,pDispInfo->item.pszText);
                    } else if (Xdata_gptr->string) m_pXdataList->SetItemText(hItem,Xdata_gptr->string);
                    else m_pXdataList->EditLabel(hItem);
                    break;
                case XDATA_CODE_LONG:
                    if (pDispInfo->item.pszText && *(pDispInfo->item.pszText)!=0) {
                        fcp1=pDispInfo->item.pszText;
                        if (!isdigit((unsigned char) *(fcp1)) && *fcp1!='-') { m_pXdataList->EditLabel(hItem); break; }
                        for(;isdigit((unsigned char) *(fcp1));++fcp1);
                        if (*fcp1!=0) { m_pXdataList->EditLabel(hItem); break; }
                        m_pXdataList->SetItemText(hItem,pDispInfo->item.pszText);
                    } else if (Xdata_gptr->string) m_pXdataList->SetItemText(hItem,Xdata_gptr->string);
                    else m_pXdataList->EditLabel(hItem);
                    break;
                case XDATA_CODE_BINDATA:
                	// EBATECH(CNBR) -[ 2002/7/3 Bugzilla#78241 Allow to create or edit xdata binary chunk.
                    if (pDispInfo->item.pszText && *(pDispInfo->item.pszText)!=0) {
						int clen = strlen(pDispInfo->item.pszText);
						if( clen % 2 ) { m_pXdataList->EditLabel(hItem); break; } // refuse odd.
						if( clen > 254 ) pDispInfo->item.pszText[254] = '\0';   // trim to 127 bytes
                        fcp1=pDispInfo->item.pszText;
                        for(;isxdigit((unsigned char) *(fcp1));++fcp1);
                        if (*fcp1!=0) { m_pXdataList->EditLabel(hItem); break; }
                        m_pXdataList->SetItemText(hItem,pDispInfo->item.pszText);
                    } else if (Xdata_gptr->string) m_pXdataList->SetItemText(hItem,Xdata_gptr->string);
                    else m_pXdataList->EditLabel(hItem);
                    // EBATECH(CNBR) ]-
                    break;
                //case XDATA_CODE_WLD_POS:  // Move to 3DPOINTS
                //case XDATA_CODE_WLD_DISP:
                //case XDATA_CODE_WLD_DIR:
                case XDATA_CODE_SCALE_FAC:
                case XDATA_CODE_DISTANCE:
                    if (pDispInfo->item.pszText && *(pDispInfo->item.pszText)!=0) {
						// EBATECH(CNBR) -[
						double dTest = atof(pDispInfo->item.pszText);
						if( Xdata_gptr->lParam == XDATA_CODE_SCALE_FAC && dTest <= 0.0 ||
						 Xdata_gptr->lParam == XDATA_CODE_DISTANCE && dTest < 0.0 )
						 { m_pXdataList->EditLabel(hItem); break; }
						 // EBATECH(CNBR) ]-
                        fcp1=pDispInfo->item.pszText;
                        for(;isdigit((int)*(fcp1));++fcp1);
                        if (*fcp1!=0) {
                            if (*fcp1=='.') {
                                for(++fcp1;isdigit((unsigned char) *(fcp1));++fcp1);
                                if (*fcp1!=0) { m_pXdataList->EditLabel(hItem); break; }
                            } else { m_pXdataList->EditLabel(hItem); break; }
                        }
                        m_pXdataList->SetItemText(hItem,pDispInfo->item.pszText);
                    } else if (Xdata_gptr->string) m_pXdataList->SetItemText(hItem,Xdata_gptr->string);
                    else m_pXdataList->EditLabel(hItem);
                    break;
                    // EBATECH(CNBR) -[ 2002/4/30 You can enter DBHANDLE
                case XDATA_CODE_DATABHANDLE:
					if (pDispInfo->item.pszText && *(pDispInfo->item.pszText)!=0) {
						sds_name p;
						if( sds_handent(pDispInfo->item.pszText, p) == RTNORM || strisame(pDispInfo->item.pszText, "FFFFFFFF"/*DNT*/)){
							m_pXdataList->SetItemText(hItem,pDispInfo->item.pszText);
						}else{ m_pXdataList->EditLabel(hItem);}
						strcpy(Xdata_gptr->string,pDispInfo->item.pszText);
					} else if (Xdata_gptr->string && *Xdata_gptr->string) {
						m_pXdataList->SetItemText(hItem,Xdata_gptr->string);
					} else { m_pXdataList->EditLabel(hItem); }
					break;
					// EBATECH(CNBR) ]-
                default:
                    break;
            }
            break;
        case TVN_SELCHANGED:
            hItem=m_pXdataList->GetSelectedItem();
            Xdata_gptr=(struct Xdata *)m_pXdataList->GetItemData(hItem);
            lType=Xdata_gptr->lParam;
            if (lType==XDATA_CODE_APPLICATION) {
                if (m_pXdataList->GetChildItem(hItem)) {
                    m_DelList->EnableWindow(TRUE);
                } else {
                    m_DelList->EnableWindow(FALSE);
                }
            } else m_DelList->EnableWindow(TRUE);
            break;
    }
    return 0L;
}

BOOL Xdata_Info::OnSetCursor(CWnd* pWnd,UINT nHitTest,UINT message) {
    TV_HITTESTINFO  HitTestInfo;
    MSG             msgData;
    SYSTEMTIME      SystemTime;
    int             Type;
    CString         cString;
    time_t          osBinaryTime;

    GetSystemTime(&SystemTime);
    CTime t(SystemTime.wYear,SystemTime.wMonth,SystemTime.wDay,SystemTime.wHour,SystemTime.wMinute,SystemTime.wSecond);
    osBinaryTime = t.GetTime();
    switch(message) {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
            GetCursorPos(&m_point);
            m_pXdataList->ScreenToClient(&m_point);
            HitTestInfo.pt.x=m_point.x; HitTestInfo.pt.y=m_point.y;
            msgData.hwnd   =this->m_hWnd;
            msgData.message=message;
            msgData.wParam =0;
            msgData.lParam =0;
            msgData.time   =osBinaryTime;
            msgData.pt.x   =m_point.x;
            msgData.pt.y   =m_point.y;
            m_ToolTip.RelayEvent(&msgData);
            break;
        case WM_MOUSEMOVE:
            GetCursorPos(&m_point);
            m_pXdataList->ScreenToClient(&m_point);
            HitTestInfo.pt.x=m_point.x; HitTestInfo.pt.y=m_point.y;
            m_pXdataList->HitTest(&HitTestInfo);
            switch(HitTestInfo.flags) {
                case TVHT_ONITEMICON:
                case TVHT_ONITEM:
                case TVHT_ONITEMLABEL:
                    Xdata_gptr=(struct Xdata *)m_pXdataList->GetItemData(HitTestInfo.hItem);
                    Type=Xdata_gptr->lParam;
                    switch(Type) {
                        case XDATA_CODE_CONTROL_STR:
                            cString=ResourceString(IDC_ICADEDITXDATA_BRACES____2, "Braces {}" );                break;
                        case XDATA_CODE_STRING:
                            cString=ResourceString(IDC_ICADEDITXDATA_STRING_3, "String" );                   break;
                        case XDATA_CODE_LAYER_NAME:
                            cString=ResourceString(IDC_ICADEDITXDATA_LAYER_NAME_4, "Layer name" );               break;
                        case XDATA_CODE_DATABHANDLE:
                            cString=ResourceString(IDC_ICADEDITXDATA_DATABASE_H_5, "Database handle" );          break;
                        case XDATA_CODE_3DPOINT:
                            cString=ResourceString(IDC_ICADEDITXDATA_3D_POINT_6, "3D point" );                 break;
                        case XDATA_CODE_REAL:
                            cString=ResourceString(IDC_ICADEDITXDATA_REAL_TYPE_7, "Real type" );                break;
                        case XDATA_CODE_INTEGER:
                            cString=ResourceString(IDC_ICADEDITXDATA_INTEGER_TY_8, "Integer type" );             break;
                        case XDATA_CODE_LONG:
                            cString=ResourceString(IDC_ICADEDITXDATA_LONG_TYPE_9, "Long type" );                break;
                        case XDATA_CODE_BINDATA:
                            cString=ResourceString(IDC_ICADEDITXDATA_BINARY_DA_10, "Binary data" );              break;
                        case XDATA_CODE_WLD_POS:
                            cString=ResourceString(IDC_ICADEDITXDATA_WORLD_SPA_11, "World space position" );     break;
                        case XDATA_CODE_WLD_DISP:
                            cString=ResourceString(IDC_ICADEDITXDATA_WORLD_SPA_12, "World space displacement" ); break;
                        case XDATA_CODE_WLD_DIR:
                            cString=ResourceString(IDC_ICADEDITXDATA_WORLD_DIR_13, "World direction" );          break;
                        case XDATA_CODE_DISTANCE:
                            cString=ResourceString(IDC_ICADEDITXDATA_DISTANCE_14, "Distance" );                 break;
                        case XDATA_CODE_SCALE_FAC:
                            cString=ResourceString(IDC_ICADEDITXDATA_SCALE_FAC_15, "Scale factor" );             break;
                        case XDATA_CODE_APPLICATION:
                            cString=ResourceString(IDC_ICADEDITXDATA_APPLICATI_16, "Application" );              break;
                    }
                    m_ToolTip.UpdateTipText(cString,m_pXdataList);

                    msgData.hwnd   =pWnd->m_hWnd;
                    msgData.message=message;
                    msgData.wParam =0;
                    msgData.lParam =0;
                    msgData.time   =osBinaryTime;
                    msgData.pt.x   =m_point.x;
                    msgData.pt.y   =m_point.y;
                    m_ToolTip.RelayEvent(&msgData);
                    break;
                default:
                    msgData.hwnd   =this->m_hWnd;
                    msgData.message=message;
                    msgData.wParam =0;
                    msgData.lParam =0;
                    msgData.time   =osBinaryTime;
                    msgData.pt.x   =m_point.x;
                    msgData.pt.y   =m_point.y;
                    m_ToolTip.RelayEvent(&msgData);
                    break;
            }
            break;
    }
    return(TRUE);
}

void Xdata_Info::DrawInsertButton() {
//*** This function draws the down arrow bitmap into the Insert... button.

	CButton* pInsButton = (CButton*)GetDlgItem(XDATA_INSERT);
	//*** Create off-screen device context.
	CDC* pButtonDC = pInsButton->GetDC();
	CDC* pMemDC = new CDC();
	pMemDC->CreateCompatibleDC(pButtonDC);
	//*** Select the bitmap into the device context.
	CBitmap* pbm = new CBitmap();
	pbm->LoadMappedBitmap(IDB_ICAD_CUSMENU_POPIND);
	CBitmap* pbmOld = pMemDC->SelectObject(pbm);

	BITMAP bms;
	pbm->GetBitmap(&bms);
	CRect rect(0,0,bms.bmWidth,bms.bmHeight);
	CBrush cb(GetSysColor(COLOR_BTNFACE));
	pMemDC->FillRect(rect,&cb);
	CPoint points[3];
	points[0].x=66;	points[0].y=10; points[1].x=72; points[1].y=10; points[2].x=69; points[2].y=13;

	HPEN hOldPen = (HPEN)pMemDC->SelectObject(CreatePen(PS_SOLID,1,GetSysColor(COLOR_WINDOWTEXT)));
	HBRUSH hOldBrush = (HBRUSH)pMemDC->SelectObject(CreateSolidBrush(GetSysColor(COLOR_WINDOWTEXT)));

	pMemDC->Polygon(points,3);

	//*** Create the font for the button.
	CFont* pFont = new CFont();
	//*** Set the font based on whether we are running under Windows95 or NT 3.5.
	OSVERSIONINFO osVer;
	osVer.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
	GetVersionEx(&osVer);
	if(osVer.dwPlatformId==VER_PLATFORM_WIN32_WINDOWS ||
		(osVer.dwPlatformId==VER_PLATFORM_WIN32_NT && osVer.dwMajorVersion>=4))
	{
		//*** Windows 95 or NT 4.0
		pFont->CreateStockObject(DEFAULT_GUI_FONT);
	}
	else
	{
		pFont->CreateStockObject(DEVICE_DEFAULT_FONT);
	}
	CFont* pOldFont = pMemDC->SelectObject(pFont);
	pMemDC->SetBkColor(GetSysColor(COLOR_BTNFACE));
	pMemDC->SetTextColor(GetSysColor(COLOR_WINDOWTEXT));

	//*** Make sure the text gets centered correctly.
	CRect rectBmp;
	pInsButton->GetClientRect(rectBmp);
	CString str(ResourceString(IDC_ICADEDITXDATA_INSERT____17, "Insert..." ));
	TEXTMETRIC tm;
    pMemDC->GetTextMetrics(&tm);
	pMemDC->SetTextAlign(TA_CENTER | TA_TOP);
	int nYPos = (((rectBmp.bottom-6)/2)-(tm.tmHeight/2))-1;
	pMemDC->TextOut(((rectBmp.right-18)/2),nYPos,str);
	//*** Clean up.

	DeleteObject( pMemDC->SelectObject(hOldPen));
	DeleteObject( pMemDC->SelectObject(hOldBrush));

	pMemDC->SelectObject(pOldFont);
	delete pFont;
	pMemDC->SelectObject(pbmOld);
	delete pMemDC;
	pInsButton->ReleaseDC(pButtonDC);
	//*** Draw the bitmap on the button.
	pInsButton->SetBitmap((HBITMAP)pbm->Detach());
	delete pbm;
}

//
// Will need to work on this stuff latter it's not critical. ALP'O
//
void Xdata_Info::OnBeginDrag(NMHDR* pNMHDR, LRESULT* pResult) {
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
    CImageList* m_pDragImage;
    IMAGEINFO   iImageInfo;

	m_pDragImage = m_pXdataList->CreateDragImage(pNMTreeView->itemNew.hItem);
    if (m_pDragImage!=NULL) {
		m_pDragImage->GetImageInfo(0,&iImageInfo);
		CPoint pt(0,(iImageInfo.rcImage.bottom));
        m_pDragImage->BeginDrag(0,pt);
		pt=pNMTreeView->ptDrag;
		ClientToScreen(&pt);
		m_pXdataList->ScreenToClient(&pt);
//		m_pXdataList->SelectItem(m_hCurDragTarget);
  		m_pDragImage->DragEnter((CWnd*)m_pXdataList,pt);
    }

    delete m_pDragImage;
}

HTREEITEM Xdata_Info::MoveTree(HTREEITEM hCurItem,HTREEITEM hInsertAfter,HTREEITEM hInsertParent) {
    HTREEITEM  hChildInsert,
               hNextInsert,
               hNewInsert;
    int        iType,
               iImage,
               iValue;
    CString    csString;

    // Get all the selected node information.
    csString=m_pXdataList->GetItemText(hCurItem);
    Xdata_gptr=(struct Xdata *)m_pXdataList->GetItemData(hCurItem);
    iType=Xdata_gptr->lParam;
    m_pXdataList->GetItemImage(hCurItem,iImage,iValue);

    // Insert the copy of the node in the new position.
    hNewInsert=AddOneItem(hInsertParent,(LPCTSTR)csString,iImage,0,iType,hInsertAfter);
    // Check if selected node has any children.
    if (m_pXdataList->ItemHasChildren(hCurItem)) {
        m_pXdataList->SetItem(hCurItem,TVIF_CHILDREN,NULL,0,0,0,0,0);
        hChildInsert=m_pXdataList->GetChildItem(hCurItem);
        MoveTree(hChildInsert,TVI_FIRST,hNewInsert);
    }
    if (hNextInsert=m_pXdataList->GetNextItem(hCurItem,TVGN_NEXT)) {
        MoveTree(hNextInsert,hNewInsert,hInsertParent);
    }

    return(hNewInsert);
}

void Xdata_Info::OnDelete(void) {
    HTREEITEM  hCurItem,
               hTopItem,
               hInsertAfter,
               hMoveItem;
    int        Type;
    CString    csString;

    // Get the selected item
    if (NULL==(hCurItem=m_pXdataList->GetSelectedItem())) return;
    // Get the type of item.
    Xdata_gptr=(struct Xdata *)m_pXdataList->GetItemData(hCurItem);
    Type=Xdata_gptr->lParam;
    switch(Type) {
        case XDATA_CODE_CONTROL_STR:
            if (m_pXdataList->GetChildItem(hCurItem)) {
                if (NULL==(hTopItem=m_pXdataList->GetNextItem(hCurItem,TVGN_PREVIOUS))) {
                    hTopItem=m_pXdataList->GetParentItem(hCurItem);
                    hInsertAfter=TVI_FIRST;
                } else {
                    hInsertAfter=hTopItem;
                    hTopItem=m_pXdataList->GetParentItem(hInsertAfter);
                }
                hMoveItem=m_pXdataList->GetChildItem(hCurItem);
                MoveTree(hMoveItem,hInsertAfter,hTopItem);
            }
            m_pXdataList->DeleteItem(hCurItem);
            break;
        case XDATA_CODE_APPLICATION:
            hCurItem=m_pXdataList->GetNextItem(hCurItem,TVGN_CHILD);
            while (hCurItem) {
                Xdata_gptr=(struct Xdata *)m_pXdataList->GetItemData(hCurItem);
                Type=Xdata_gptr->lParam;
                if (Type==XDATA_CODE_APPLICATION) break;
                hTopItem=m_pXdataList->GetNextItem(hCurItem,TVGN_NEXT);
                m_pXdataList->DeleteItem(hCurItem);
                hCurItem=hTopItem;
            }
            m_DelList->EnableWindow(FALSE);
            break;
        default:
            m_pXdataList->DeleteItem(hCurItem);
            break;
    }
}

#define __TEMPOLARY_CODE	// EBATECH(CNBR) -[ Temporary comment out. 2003/4/30

void Xdata_Info::OnContextMenuPick(UINT nId) {
     HTREEITEM      hCurItem,
                    hItem;
     BOOL           bEditflag=TRUE;
     int            Type;
     short          fi1;
     struct resbuf *rbb;
#ifndef __TEMPOLARY_CODE
     struct Xdata  *temp;
#endif

     // Get the selected item.
     hCurItem=m_pXdataList->GetSelectedItem();
     switch(nId) {
        case XDATA_APPNAME:
            // Get the root node.
            hItem=m_pXdataList->GetRootItem();
            // Set the new selected node to be the root node.
            hCurItem=hItem;
            // Add an application node and set the selected node to the application.
            hCurItem=AddOneItem(NULL,ResourceString(IDC_ICADEDITXDATA_APPLICATI_16, "Application" ),14,0,XDATA_CODE_APPLICATION,TVI_LAST);
            if (!hItem) hItem=hCurItem;
            m_Style=0;
            break;
        default:
            if (!hCurItem) break;
            Xdata_gptr=(struct Xdata *)m_pXdataList->GetItemData(hCurItem);
            Type=Xdata_gptr->lParam;
            if (Type==XDATA_CODE_CONTROL_STR || Type==XDATA_CODE_APPLICATION) {
                hItem=hCurItem;
                hCurItem=TVI_FIRST;
                if (!m_pXdataList->ItemHasChildren(hItem)) {
                    m_pXdataList->SetItem(hItem,TVIF_CHILDREN,NULL,0,0,0,0,0);
                }
            } else {
                hItem=m_pXdataList->GetParentItem(hCurItem);
            }
            m_DelList->EnableWindow(TRUE);
            if (nId>=100) {
                // Modified on july 011/97 ~ALPO~.
                fi1=0;
                rbb=SDS_tblgetter("LAYER"/*DNT*/,NULL,TRUE,SDS_CURDWG);
                while(rbb) {
                    if (fi1==(short)nId-100) break;
                    sds_relrb(rbb);
                    rbb=SDS_tblgetter("LAYER"/*DNT*/,NULL,FALSE,SDS_CURDWG);
                    ++fi1;
                };
                if (ic_assoc(rbb,2)) return;
                hCurItem=AddOneItem(hItem,ic_rbassoc->resval.rstring,7,0,XDATA_CODE_LAYER_NAME,hCurItem);
				sds_relrb( rbb );

            } else {
                switch(nId) {
                    case XDATA_BRACES:
                        hCurItem=hItem=AddOneItem(hItem,ResourceString(IDC_ICADEDITXDATA_BRACES____2, "Braces {}" ),11,0,XDATA_CODE_CONTROL_STR,hCurItem);
                        bEditflag=FALSE;
                        break;
                    case XDATA_STRING:
                        hCurItem=AddOneItem(hItem,ResourceString(IDC_ICADEDITXDATA_STRING_3, "String" ),1,0,XDATA_CODE_STRING,hCurItem);
                        break;
                    case XDATA_LAYERNAME:
                        hCurItem=AddOneItem(hItem,ResourceString(IDC_ICADEDITXDATA_LAYER_18, "Layer" ),7,0,XDATA_CODE_LAYER_NAME,hCurItem);
                        break;
                    case XDATA_DATABHAND:
#ifdef __TEMPOLARY_CODE
                        hCurItem=AddOneItem(hItem,"FFFFFFFF",13,0,XDATA_CODE_DATABHANDLE,hCurItem);
#else
                        hCurItem=(hCurItem!=TVI_FIRST)?hCurItem:hItem;
                        temp=(struct Xdata *)m_pXdataList->GetItemData(hCurItem);
                        m_SelectedIndx=temp->sIndex;
                        EndDialog(XDATA_DATABHAND);
#endif
                        break;
                    case XDATA_3DPOINT:
                        hCurItem=AddOneItem(hItem,ResourceString(IDC_ICADEDITXDATA_0_0_0_0_0_19, "0.0,0.0,0.0" ),5,0,XDATA_CODE_3DPOINT,hCurItem);//3D point
                        break;
                    case XDATA_REAL:
                        hCurItem=AddOneItem(hItem,ResourceString(IDC_ICADEDITXDATA_0_0_20, "0.0" ),9,0,XDATA_CODE_REAL,hCurItem);//Real
                        break;
                    case XDATA_INTEGER:
                        hCurItem=AddOneItem(hItem,ResourceString(IDC_ICADEDITXDATA_0_21, "0" ),6,0,XDATA_CODE_INTEGER,hCurItem);//Integer
                        break;
                    case XDATA_LONG:
                        hCurItem=AddOneItem(hItem,ResourceString(IDC_ICADEDITXDATA_0_21, "0" ),8,0,XDATA_CODE_LONG,hCurItem);//Long
                        break;
                    case XDATA_BIN_DATA:
                        hCurItem=AddOneItem(hItem,ResourceString(IDC_ICADEDITXDATA_BINARY_DA_10, "Binary data" ),10,0,XDATA_CODE_BINDATA,hCurItem);
                        break;
                    case XDATA_WLD_POS:
#ifdef __TEMPOLARY_CODE
                        hCurItem=AddOneItem(hItem,ResourceString(IDC_ICADEDITXDATA_0_0_0_0_0_19, "0.0,0.0,0.0" ),4,0,XDATA_CODE_WLD_POS,hCurItem);
#else
                        hCurItem=(hCurItem!=TVI_FIRST)?hCurItem:hItem;
                        temp=(struct Xdata *)m_pXdataList->GetItemData(hCurItem);
                        m_SelectedIndx=temp->sIndex;
                        EndDialog(XDATA_WLD_POS);
#endif
                        break;
                    case XDATA_WLD_DISP:
#ifdef __TEMPOLARY_CODE
                        hCurItem=AddOneItem(hItem,ResourceString(IDC_ICADEDITXDATA_0_0_0_0_0_19, "0.0,0.0,0.0" ),3,0,XDATA_CODE_WLD_DISP,hCurItem);
#else
                        hCurItem=(hCurItem!=TVI_FIRST)?hCurItem:hItem;
                        temp=(struct Xdata *)m_pXdataList->GetItemData(hCurItem);
                        m_SelectedIndx=temp->sIndex;
                        EndDialog(XDATA_WLD_DISP);
#endif
                        break;
                    case XDATA_WLD_DIR:
#ifdef __TEMPOLARY_CODE
                        hCurItem=AddOneItem(hItem,ResourceString(IDC_ICADEDITXDATA_0_0_0_0_0_19, "0.0,0.0,0.0" ),2,0,XDATA_CODE_WLD_DIR,hCurItem);
#else
                        hCurItem=(hCurItem!=TVI_FIRST)?hCurItem:hItem;
                        temp=(struct Xdata *)m_pXdataList->GetItemData(hCurItem);
                        m_SelectedIndx=temp->sIndex;
                        EndDialog(XDATA_WLD_DIR);
#endif
                        break;
                    case XDATA_WLD_DIST:
#ifdef __TEMPOLARY_CODE
                        hCurItem=AddOneItem(hItem,ResourceString(IDC_ICADEDITXDATA_0_0_20, "0.0" ),12,0,XDATA_CODE_DISTANCE,hCurItem);
#else
                        hCurItem=(hCurItem!=TVI_FIRST)?hCurItem:hItem;
                        temp=(struct Xdata *)m_pXdataList->GetItemData(hCurItem);
                        m_SelectedIndx=temp->sIndex;
                        EndDialog(XDATA_WLD_DIST);
#endif
                        break;
                    case XDATA_WLD_SCALE:
                        hCurItem=AddOneItem(hItem,ResourceString(IDC_ICADEDITXDATA_SCALE_FAC_15, "Scale factor" ),0,0,XDATA_CODE_SCALE_FAC,hCurItem);
                        break;
                }
            }
     }

     m_pXdataList->SelectItem(hCurItem);

     if (bEditflag) {
         m_pXdataList->EditLabel(hCurItem);
     }
}

void Xdata_Info::OnButtonInsertItem(void) {
	CMenu* pMenu    = new CMenu();
	CMenu* pSubMenu = new CMenu();
    short fi1=0;
	pMenu->CreatePopupMenu();
    pSubMenu->CreatePopupMenu();
    CString csTmp;

	SDS_StartTblRead("LAYER"/*DNT*/);
	while(SDS_TblReadHasMore()) {
    	csTmp=SDS_TblReadString();
        if(csTmp.Find('|')>0) continue;
		pSubMenu->AppendMenu(MF_STRING|MF_UNCHECKED,fi1+100,csTmp);
		fi1++;
	}

	pMenu->AppendMenu(MF_STRING|MF_UNCHECKED,XDATA_APPNAME            ,ResourceString(IDC_ICADEDITXDATA_APPLICATI_22, "Application name" ));
	pMenu->AppendMenu(MF_STRING|MF_UNCHECKED|m_Style,XDATA_BRACES     ,ResourceString(IDC_ICADEDITXDATA_BRACES____23, "Braces \'{\'\'}\'" ));
	pMenu->AppendMenu(MF_SEPARATOR);
	pMenu->AppendMenu(MF_POPUP|m_Style,(UINT)pSubMenu->m_hMenu        ,ResourceString(IDC_ICADEDITXDATA_LAYER_NAME_4, "Layer name" ));
	pMenu->AppendMenu(MF_STRING|MF_UNCHECKED|m_Style,XDATA_STRING     ,ResourceString(IDC_ICADEDITXDATA_STRING_3, "String" ));
	pMenu->AppendMenu(MF_STRING|MF_UNCHECKED|m_Style,XDATA_DATABHAND  ,ResourceString(IDC_ICADEDITXDATA_DATABASE_H_5, "Database handle" ));
	pMenu->AppendMenu(MF_STRING|MF_UNCHECKED|m_Style,XDATA_3DPOINT    ,ResourceString(IDC_ICADEDITXDATA_3D_POINT_6, "3D point" ));
	pMenu->AppendMenu(MF_STRING|MF_UNCHECKED|m_Style,XDATA_REAL       ,ResourceString(IDC_ICADEDITXDATA_REAL_24, "Real" ));
	pMenu->AppendMenu(MF_STRING|MF_UNCHECKED|m_Style,XDATA_INTEGER    ,ResourceString(IDC_ICADEDITXDATA_INTEGER_25, "Integer" ));
	pMenu->AppendMenu(MF_STRING|MF_UNCHECKED|m_Style,XDATA_LONG       ,ResourceString(IDC_ICADEDITXDATA_LONG_26, "Long" ));
	pMenu->AppendMenu(MF_STRING|MF_UNCHECKED|m_Style,XDATA_BIN_DATA   ,ResourceString(IDC_ICADEDITXDATA_BINARY_DA_10, "Binary data" )); // EBATECH(CNBR) 2002/7/3 Bugzilla#78241 Allow to create or edit xdata binary chunk.
	pMenu->AppendMenu(MF_STRING|MF_UNCHECKED|m_Style,XDATA_WLD_POS    ,ResourceString(IDC_ICADEDITXDATA_WORLD_SPA_11, "World space position" ));
	pMenu->AppendMenu(MF_STRING|MF_UNCHECKED|m_Style,XDATA_WLD_DISP   ,ResourceString(IDC_ICADEDITXDATA_WORLD_SPA_12, "World space displacement" ));
	pMenu->AppendMenu(MF_STRING|MF_UNCHECKED|m_Style,XDATA_WLD_DIR    ,ResourceString(IDC_ICADEDITXDATA_WORLD_DIR_13, "World direction" ));
	pMenu->AppendMenu(MF_STRING|MF_UNCHECKED|m_Style,XDATA_WLD_DIST   ,ResourceString(IDC_ICADEDITXDATA_DISTANCE_14, "Distance" ));
	pMenu->AppendMenu(MF_STRING|MF_UNCHECKED|m_Style,XDATA_WLD_SCALE  ,ResourceString(IDC_ICADEDITXDATA_SCALE_FAC_15, "Scale factor" ));

	CButton* pButton = (CButton*)GetDlgItem(XDATA_INSERT);
	CRect rectButton;
	pButton->GetWindowRect(rectButton);
	// Display the menu
 	pMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON,rectButton.left,rectButton.bottom,this);
	// Delete the menu
	delete pMenu;
	delete pSubMenu;
}

void Xdata_Info::AddxdataItems(void) {
    struct resbuf  *rb,
                   *rbprv;
    HTREEITEM       hCurItem,
                    hItem;
    char           *string;
    short           Index=1;
    BOOL            CntFlag=TRUE;
    struct resbuf   appname;

    appname.restype=RTSTR; appname.resval.rstring=const_cast<char*>(ResourceString(DNT_ICADEDITXDATA___27, "*") ); appname.rbnext=NULL;

    string=(char *)new char[IC_ACADBUF];

    if (!m_frb) {
        if (NULL==(rb=sds_entgetx(m_Xename,&appname))) return;
        m_frb=rb;
    } else rb=m_frb;
    for(rbprv=rb;rb && rb->restype!=-3;rbprv=rb,rb=rb->rbnext);
    if (!rb) rbprv->rbnext=sds_newrb(-3);
    do {
        // Get the 1001 group this would be the application name.
        for(;rb;rb=rb->rbnext) {
            if (rb->restype==XDATA_CODE_APPLICATION) {
                hCurItem=AddOneItem(NULL,rb->resval.rstring,14,0,XDATA_CODE_APPLICATION,TVI_LAST);
                break;
            }
        }
        if (!rb) break;
        if (m_SelectedIndx==Index) {
            switch(m_valuerb.restype) {
                case XDATA_CODE_WLD_POS:
                    sprintf(string,ResourceString(IDC_ICADEDITXDATA__F__F__F_28, "%f,%f,%f" ),m_valuerb.resval.rpoint[0],m_valuerb.resval.rpoint[1],m_valuerb.resval.rpoint[2]);
                    hItem=AddOneItem(hCurItem,string,4,0,XDATA_CODE_WLD_POS,TVI_LAST);
                    break;
                case XDATA_CODE_WLD_DISP:
                    sprintf(string,ResourceString(IDC_ICADEDITXDATA__F__F__F_28, "%f,%f,%f" ),m_valuerb.resval.rpoint[0],m_valuerb.resval.rpoint[1],m_valuerb.resval.rpoint[2]);
                    hItem=AddOneItem(hCurItem,string,3,0,XDATA_CODE_WLD_DISP,TVI_LAST);
                    break;
                case XDATA_CODE_WLD_DIR:
                    sprintf(string,ResourceString(IDC_ICADEDITXDATA__F__F__F_28, "%f,%f,%f" ),m_valuerb.resval.rpoint[0],m_valuerb.resval.rpoint[1],m_valuerb.resval.rpoint[2]);
                    hItem=AddOneItem(hCurItem,string,2,0,XDATA_CODE_WLD_DIR,TVI_LAST);
                    break;
                case XDATA_CODE_DISTANCE:
                    _gcvt(m_valuerb.resval.rreal,7,string);
                    hItem=AddOneItem(hCurItem,string,12,0,XDATA_CODE_DISTANCE,TVI_LAST);
                    break;
                case XDATA_CODE_DATABHANDLE:
                    if (m_valuerb.resval.rstring)
						{
                        hItem=AddOneItem(hCurItem,m_valuerb.resval.rstring,13,0,XDATA_CODE_DATABHANDLE,TVI_LAST);
						delete [] m_valuerb.resval.rstring;
						m_valuerb.resval.rstring = NULL;
			            }
					else
						{
						delete [] string;
						return;
						}
                    break;
            }
            m_pXdataList->SelectItem(hItem);
        }
        ++Index;
        for(rb=rb->rbnext;rb;rb=rb->rbnext) {
            switch(rb->restype) {
                case XDATA_CODE_CONTROL_STR:
                    if (*rb->resval.rstring=='{') {
                        hCurItem=AddOneItem(hCurItem,ResourceString(IDC_ICADEDITXDATA_BRACES____2, "Braces {}" ),11,0,XDATA_CODE_CONTROL_STR,TVI_LAST);
                    } else {
                        hCurItem=m_pXdataList->GetParentItem(hCurItem);
                    }
                    break;
                case XDATA_CODE_STRING:
                    AddOneItem(hCurItem,rb->resval.rstring,1,0,XDATA_CODE_STRING,TVI_LAST);
                    break;
                case XDATA_CODE_LAYER_NAME:
                    AddOneItem(hCurItem,rb->resval.rstring,7,0,XDATA_CODE_LAYER_NAME,TVI_LAST);
                    break;
                case XDATA_CODE_DATABHANDLE:
                    AddOneItem(hCurItem,rb->resval.rstring,13,0,XDATA_CODE_DATABHANDLE,TVI_LAST);
                    break;
                case XDATA_CODE_3DPOINT:
                    sprintf(string,ResourceString(IDC_ICADEDITXDATA__F__F__F_28, "%f,%f,%f" ),rb->resval.rpoint[0],rb->resval.rpoint[1],rb->resval.rpoint[2]);
                    AddOneItem(hCurItem,string,5,0,XDATA_CODE_3DPOINT,TVI_LAST);
                    break;
                case XDATA_CODE_REAL:
                    _gcvt(rb->resval.rreal,7,string);
                    AddOneItem(hCurItem,string,9,0,XDATA_CODE_REAL,TVI_LAST);
                    break;
                case XDATA_CODE_INTEGER:
                    itoa(rb->resval.rint,string,10);
                    AddOneItem(hCurItem,string,6,0,XDATA_CODE_INTEGER,TVI_LAST);
                    break;
                case XDATA_CODE_LONG:
                    ltoa(rb->resval.rlong,string,10);
                    AddOneItem(hCurItem,string,8,0,XDATA_CODE_LONG,TVI_LAST);
                    break;
                case XDATA_CODE_BINDATA:
					char tempstr[300];
					db_BinaryHexToAscii((unsigned char *)rb->resval.rbinary.buf,rb->resval.rbinary.clen,tempstr, 300);
                    AddOneItem(hCurItem,tempstr,1,0,XDATA_CODE_BINDATA,TVI_LAST);
                    break;
                case XDATA_CODE_WLD_POS:
                    sprintf(string,ResourceString(IDC_ICADEDITXDATA__F__F__F_28, "%f,%f,%f" ),rb->resval.rpoint[0],rb->resval.rpoint[1],rb->resval.rpoint[2]);
                    AddOneItem(hCurItem,string,4,0,XDATA_CODE_WLD_POS,TVI_LAST);
                    break;
                case XDATA_CODE_WLD_DISP:
                    sprintf(string,ResourceString(IDC_ICADEDITXDATA__F__F__F_28, "%f,%f,%f" ),rb->resval.rpoint[0],rb->resval.rpoint[1],rb->resval.rpoint[2]);
                    AddOneItem(hCurItem,string,3,0,XDATA_CODE_WLD_DISP,TVI_LAST);
                    break;
                case XDATA_CODE_WLD_DIR:
                    sprintf(string,ResourceString(IDC_ICADEDITXDATA__F__F__F_28, "%f,%f,%f" ),rb->resval.rpoint[0],rb->resval.rpoint[1],rb->resval.rpoint[2]);
                    AddOneItem(hCurItem,string,2,0,XDATA_CODE_WLD_DIR,TVI_LAST);
                    break;
                case XDATA_CODE_DISTANCE:
                    _gcvt(rb->resval.rreal,7,string);
                    AddOneItem(hCurItem,string,12,0,XDATA_CODE_DISTANCE,TVI_LAST);
                    break;
                case XDATA_CODE_SCALE_FAC:
                    _gcvt(rb->resval.rreal,7,string);
                    AddOneItem(hCurItem,string,0,0,XDATA_CODE_SCALE_FAC,TVI_LAST);
                    break;
                default:
                    CntFlag=FALSE;
                    break;
            }
            if (CntFlag) {
                if (m_SelectedIndx==Index) {
                    switch(m_valuerb.restype) {
                        case XDATA_CODE_WLD_POS:
                            sprintf(string,ResourceString(IDC_ICADEDITXDATA__F__F__F_28, "%f,%f,%f" ),m_valuerb.resval.rpoint[0],m_valuerb.resval.rpoint[1],m_valuerb.resval.rpoint[2]);
                            hItem=AddOneItem(hCurItem,string,4,0,XDATA_CODE_WLD_POS,TVI_LAST);
                            break;
                        case XDATA_CODE_WLD_DISP:
                            sprintf(string,ResourceString(IDC_ICADEDITXDATA__F__F__F_28, "%f,%f,%f" ),m_valuerb.resval.rpoint[0],m_valuerb.resval.rpoint[1],m_valuerb.resval.rpoint[2]);
                            hItem=AddOneItem(hCurItem,string,3,0,XDATA_CODE_WLD_DISP,TVI_LAST);
                            break;
                        case XDATA_CODE_WLD_DIR:
                            sprintf(string,ResourceString(IDC_ICADEDITXDATA__F__F__F_28, "%f,%f,%f" ),m_valuerb.resval.rpoint[0],m_valuerb.resval.rpoint[1],m_valuerb.resval.rpoint[2]);
                            hItem=AddOneItem(hCurItem,string,2,0,XDATA_CODE_WLD_DIR,TVI_LAST);
                            break;
                        case XDATA_CODE_DISTANCE:
                            _gcvt(m_valuerb.resval.rreal,7,string);
                            hItem=AddOneItem(hCurItem,string,12,0,XDATA_CODE_DISTANCE,TVI_LAST);
                            break;
                        case XDATA_CODE_DATABHANDLE:
                            if (m_valuerb.resval.rstring)
								{
                                hItem=AddOneItem(hCurItem,m_valuerb.resval.rstring,13,0,XDATA_CODE_DATABHANDLE,TVI_LAST);
								delete [] m_valuerb.resval.rstring;
								m_valuerb.resval.rstring = NULL;
	                            }
							else
								{
								delete [] string;
								return;
								}
                            break;
                    }
                    m_pXdataList->SelectItem(hItem);
                    CntFlag=FALSE;
                }
                ++Index;
            }
            if (rb->restype==XDATA_CODE_APPLICATION) break;
        }
    } while(rb);
    delete [] string;
}

struct resbuf* Xdata_Info::SaveLinks(HTREEITEM hCurItem,struct resbuf **rb,BOOL bCloseflag) {
    HTREEITEM      hNextItem;
    int            iType;
    CString        csString;
    char          *fcp1;
    short          fi1;
    struct resbuf  appname,
                  *rbl,
                  *rbb,
                  *rbt,
                  *relist;

    if (bCloseflag) {
        (*rb)=sds_newrb(XDATA_CODE_CONTROL_STR);
        //Bugzilla No. 78104 ; 15-04-2002
		(*rb)->resval.rstring=new char [2];
        strcpy((*rb)->resval.rstring,ResourceString(DNT_ICADEDITXDATA___29, "}" ));
        rbb=(*rb);
    } else {
        // Get all the selected node information.
        csString=m_pXdataList->GetItemText(hCurItem);
        Xdata_gptr=(struct Xdata *)m_pXdataList->GetItemData(hCurItem);
        iType=Xdata_gptr->lParam;

        (*rb)=sds_newrb(iType);

        switch(iType) {
            case XDATA_CODE_CONTROL_STR:
                //Bugzilla No. 78104 ; 15-04-2002
				(*rb)->resval.rstring= new char [2];
                strcpy((*rb)->resval.rstring,ResourceString(DNT_ICADEDITXDATA___30, "{" ));
                break;
            case XDATA_CODE_APPLICATION:
                //Bugzilla No. 78104 ; 15-04-2002
				(*rb)->resval.rstring= new char [csString.GetLength()+1];
                csString.MakeUpper();
                strcpy((*rb)->resval.rstring,(char *)((LPCTSTR)csString));

                if (!strisame(Xdata_gptr->string,(*rb)->resval.rstring)) {
                    appname.restype=RTSTR; appname.resval.rstring=const_cast<char*>(ResourceString(DNT_ICADEDITXDATA___27, "*") ); appname.rbnext=NULL;
                    if (NULL!=(relist=sds_entgetx(m_Xename,&appname))) {
                        for(rbb=relist;rbb;rbb=rbb->rbnext) {
                            if (rbb->restype==XDATA_CODE_APPLICATION) { if (strisame(rbb->resval.rstring,Xdata_gptr->string)) break;  }
                        }
                        if (rbb) {
                            for(rbl=rbt=rbb->rbnext;rbt && rbt->restype!=XDATA_CODE_APPLICATION;rbl=rbt,rbt=rbt->rbnext);
                            if (rbt) rbl->rbnext=NULL;
                            sds_relrb(rbb->rbnext);
//                            rbb->rbnext=(rbt)?rbl:NULL;
							rbb->rbnext=rbt;   // in ANY case, NULL or not.  rbl is already released, so the above line is wrong
                            fi1=sds_entmod(relist);
						}
						sds_relrb(relist);
                    }
                }
                break;
            case XDATA_CODE_DATABHANDLE:
            case XDATA_CODE_STRING:
            case XDATA_CODE_LAYER_NAME:
				(*rb)->resval.rstring= new char [csString.GetLength()+1];
                strcpy((*rb)->resval.rstring,(char *)((LPCTSTR)csString));
                break;
            case XDATA_CODE_DISTANCE:
            case XDATA_CODE_SCALE_FAC:
            case XDATA_CODE_REAL:
                (*rb)->resval.rreal=atof((LPCTSTR)csString);
                break;
            case XDATA_CODE_INTEGER:
                (*rb)->resval.rint=atoi((LPCTSTR)csString);
                break;
            case XDATA_CODE_LONG:
                (*rb)->resval.rlong=atol((LPCTSTR)csString);
                break;
            case XDATA_CODE_3DPOINT:
            case XDATA_CODE_WLD_POS:
            case XDATA_CODE_WLD_DISP:
            case XDATA_CODE_WLD_DIR:
                fi1=0;
                fcp1=strtok((char *)((LPCTSTR)csString),ResourceString(IDC_ICADEDITXDATA___31, "," ));
                while(fcp1) {
                    (*rb)->resval.rpoint[fi1]=atof(fcp1);
                    fcp1=strtok(NULL,ResourceString(IDC_ICADEDITXDATA___31, "," ));
                    ++fi1;
                }
                break;
            case XDATA_CODE_BINDATA:
				short validChars;
				char* tempbin = new char [(*rb)->resval.rbinary.clen];
				db_AsciiHexToBinary((char *)((LPCTSTR)csString),tempbin,(*rb)->resval.rbinary.clen, &validChars);
				(*rb)->resval.rbinary.buf=new char [(*rb)->resval.rbinary.clen];
				memcpy((*rb)->resval.rbinary.buf,tempbin,(*rb)->resval.rbinary.clen);
				delete [] tempbin;
                break;
        }
        rbb=(*rb);
        // Check if selected node has any children.
        if (m_pXdataList->ItemHasChildren(hCurItem)) {
            if (hNextItem=m_pXdataList->GetChildItem(hCurItem)) {
                rbb=SaveLinks(hNextItem,&rbb->rbnext,FALSE);
            }
        }
        // Add a link for the closeing brace.
        if (iType==XDATA_CODE_CONTROL_STR) {
            rbb=SaveLinks(NULL,&rbb->rbnext,TRUE);
        }
        // Check if there are other sibling nodes.
        if (hNextItem=m_pXdataList->GetNextItem(hCurItem,TVGN_NEXT)) {
            rbb=SaveLinks(hNextItem,&rbb->rbnext,FALSE);
        }
    }
    return(rbb);
}

void Xdata_Info::ONRename(void) {
    HTREEITEM       hItem;

    hItem=m_pXdataList->GetSelectedItem();
    m_pXdataList->EditLabel(hItem);

}

///////////////////////////////////////////////////////////////////////
// This function is called when the close button is hit.
//
// returns : Nothing
//
void Xdata_Info::OnOk(void) {
    struct resbuf  *rb,
                   *rbt,
                   *rblist;
    HTREEITEM       hParentItem;
    CString         csString;

    // Search for the -3 restype.
    for(rb=m_frb;rb;rb=rb->rbnext) { if (rb->restype==-3) break; }
    if (!rb) return;
    // free the old link list.
    if (rb->rbnext) {
        sds_relrb(rb->rbnext);
        rb->rbnext=NULL;
    }
    if (NULL!=(hParentItem=m_pXdataList->GetRootItem())) {
        SaveLinks(hParentItem,&rblist,FALSE);
        for(rbt=rblist;rbt;rbt=rbt->rbnext) {
            if (rbt->restype==1001) sds_regapp(rbt->resval.rstring);
        }
        rb->rbnext=rblist;
        int ret=sds_entmod(m_frb);
        sds_relrb(m_frb);
        m_frb=NULL;
    }
    EndDialog(0);
}

///////////////////////////////////////////////////////////////////////
// This function is called when an item to the (CTreeList) control
// is added.
//
// hParent   : Handle of the inserted item’s parent.
// szText    :
// hInsAfter : Handle of the item after which the new item is to be inserted.
// iImage    : Index of the item’s image in the tree view control’s image list.
// SiImage   : Index of the item’s selected image in the tree view control’s image list.
// index     :
//
// HTREEITEM : Handle of the new item if successful; otherwise NULL.
//
HTREEITEM Xdata_Info::AddOneItem(HTREEITEM hParent,LPCTSTR szText,int iImage,short index,int Type,HTREEITEM InsertAt) {
	HTREEITEM        hItem;
	TV_ITEM          tvI;
	TV_INSERTSTRUCT  tvIns;
    struct Xdata    *temp;

    if (NULL==(temp=Makexdatalink())) return(NULL);
    temp->lParam=(int)Type;
    temp->sIndex=++m_indexcntr;
    strcpy(temp->string,szText);

	tvI.mask	       = TVIF_TEXT|TVIF_PARAM|TVIF_IMAGE|TVIF_SELECTEDIMAGE;
	tvI.pszText	       = (char *)szText;
	tvI.cchTextMax     = lstrlen(szText);
	tvI.iImage		   = iImage;
	tvI.iSelectedImage = iImage;
    tvI.lParam         = (LPARAM)temp;

	tvIns.item         = tvI;
	tvIns.hInsertAfter = InsertAt;
	tvIns.hParent      = hParent;
	hItem              = m_pXdataList->InsertItem(&tvIns);

	return (hItem);
}


struct Xdata *Xdata_Info::Makexdatalink(void) {
    struct Xdata *temp,
                 *xcur;

    if (NULL==(temp=(struct Xdata *)new (struct Xdata))) return(NULL);
    memset(temp,0,sizeof(struct Xdata));
    for(xcur=m_xdataBeg;xcur && xcur->next;xcur=xcur->next);
    if (!xcur) {
        m_xdataBeg=temp;
    } else {
        xcur->next=temp;
    }

    return(temp);
}

void Xdata_Info::Freedatalinks(void) {
    struct Xdata *temp,
                 *xcur;

    for(temp=m_xdataBeg;temp;temp=xcur) {
        xcur=temp->next;
        delete temp;
    }
    m_xdataBeg=NULL;
}

void Xdata_Info::OnDestroy(void) {
    struct resbuf  *rb,
                   *rbt,
                   *rblist;
    HTREEITEM       hParentItem;
    CString         csString;

    // Search for the -3 restype.
    for(rb=m_frb;rb;rb=rb->rbnext) { if (rb->restype==-3) break; }
    if (!rb) return;
    // free the old link list.
    if (rb->rbnext) sds_relrb(rb->rbnext);
    if (NULL!=(hParentItem=m_pXdataList->GetRootItem())) {
        SaveLinks(hParentItem,&rblist,FALSE);
        for(rbt=rblist;rbt;rbt=rbt->rbnext) {
            if (rbt->restype==1001) sds_regapp(rbt->resval.rstring);
        }
        rb->rbnext=rblist;
    }
	if ( m_pImageList != NULL)
		{
		delete m_pImageList;
		m_pImageList = NULL;
		}

    return;
}

// ======================================================================================================================================================================
// ======================================================= CMAINWINDOW CLASS FUNCTION =============================================================================
// ======================================================================================================================================================================

void CMainWindow::IcadXdataEdit(sds_name nmEntity) {
    int                iType;
    sds_point          pt1 = { 0.0, 0.0, 0.0};
#ifndef __TEMPOLARY_CODE
    sds_name           ename;
    struct sds_resbuf *elist,
		*rb;
#endif

    m_XdataEdit=(Xdata_Info *) new Xdata_Info();
    m_XdataEdit->m_frb=NULL;

    ic_encpy(m_XdataEdit->m_Xename,nmEntity);

    for(;;) {
        iType=m_XdataEdit->DoModal();
        m_XdataEdit->Freedatalinks();
        switch(iType) {
#ifndef __TEMPOLARY_CODE
            case XDATA_WLD_POS:
                m_XdataEdit->m_valuerb.restype=XDATA_CODE_WLD_POS;
                internalGetpoint(pt1,ResourceString(IDC_ICADEDITXDATA__N3D_WORL_32, "\n3D World space position: " ),m_XdataEdit->m_valuerb.resval.rpoint);
                break;
            case XDATA_WLD_DISP:
                m_XdataEdit->m_valuerb.restype=XDATA_CODE_WLD_DISP;
                internalGetpoint(pt1,ResourceString(IDC_ICADEDITXDATA__N3D_WORL_33, "\n3D World space displacement: " ),m_XdataEdit->m_valuerb.resval.rpoint);
                break;
            case XDATA_WLD_DIR:
                m_XdataEdit->m_valuerb.restype=XDATA_CODE_WLD_DIR;
                internalGetpoint(pt1,ResourceString(IDC_ICADEDITXDATA__3D_WORLD_34, "\3D World space direction: " ),m_XdataEdit->m_valuerb.resval.rpoint);
                break;
            case XDATA_WLD_DIST:
                m_XdataEdit->m_valuerb.restype=XDATA_CODE_DISTANCE;
                sds_getdist(NULL,ResourceString(IDC_ICADEDITXDATA__NDISTANC_35, "\nDistance" ),&m_XdataEdit->m_valuerb.resval.rreal);
                break;
            case XDATA_DATABHAND:
                m_XdataEdit->m_valuerb.restype=XDATA_CODE_DATABHANDLE;
                if (sds_entsel(ResourceString(IDC_ICADEDITXDATA__NSELECT__36, "\nSelect entity: " ),ename,pt1)==RTNORM) {
                    elist=sds_entget(ename);
                    for(rb=elist;rb && rb->restype!=5;rb=rb->rbnext);
                    if (!rb) break;
                    m_XdataEdit->m_valuerb.resval.rstring=(char *)new char[strlen(rb->resval.rstring)+1];
                    strcpy(m_XdataEdit->m_valuerb.resval.rstring,rb->resval.rstring);
					IC_RELRB(elist);
                } else m_XdataEdit->m_valuerb.resval.rstring=NULL;
                break;
#endif
            case 0:
            case 2:
				delete m_XdataEdit;
				m_XdataEdit = NULL;
                return;
        }
    }

	delete m_XdataEdit;
	m_XdataEdit = NULL;
    return;
}


