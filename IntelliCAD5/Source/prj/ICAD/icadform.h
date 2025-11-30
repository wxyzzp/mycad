#pragma once

// ====================================================          ===========================================================
// ==================================================== #defines ===========================================================
// ====================================================          ===========================================================

#define FRM_TEST_DIALOG     WM_USER+101
#define FRM_EDIT_DIALOG     WM_USER+102
#define FRM_ADD_ITEM        WM_USER+103
#define FRM_CLOSEING        WM_USER+104
#define FRM_TEMP            WM_USER+105
#define FRM_UPDATE_CUS      WM_USER+106
#define FRM_PROPDLG         WM_USER+107
#define FRM_NEW_FORM        "form"
#define FRM_CROSS_ID        0
#define FRM_SIZENESW_ID     1
#define FRM_SIZENWSE_ID     2
#define FRM_SIZEWE_ID       3
#define FRM_SIZENS_ID       4
#define FRM_ARROW_ID        5
#define FRM_TOOLBAR_STATE   "formtoolbar"
#define FRM_W_SPACER        40
#define FRM_H_SPACER        65
#define FRM_H_DLGOFF        5
#define FRM_V_DLGOFF        5
#define FRM_VK_a            97
#define FRM_VK_A		    65
#define FRM_VK_C            67
#define FRM_VK_c            99
#define FRM_VK_X            88
#define FRM_VK_x            120
#define FRM_VK_V            86
#define FRM_VK_v            118
#define FRM_X_OFFDIFF       15
#define FRM_Y_OFFDIFF       15
#define FRM_LB_CTRL         MK_LBUTTON|MK_CONTROL

// ====================================================            ===========================================================
// ==================================================== Structures ===========================================================
// ====================================================            ===========================================================

struct FRM_frmList {
    HDC                 hdcCompat;
    RECT                rect;
    long                fkey;
    long                fieldlen;
    char               *frmname,
                       *tfname;
    BYTE               *bptr;
    short               ItemCntr,
                        pointsize;
    struct FRM_frmInfo *ptr;
    struct FRM_frmList *next;
};

struct FRM_frmInfo {
    BOOL   SelectFlg;
    RECT   rect,
           offrect,
           Loffset;
    long   Wid;
    POINT  LPoint;
    struct FRM_frmInfo *next;
};

struct FRM_cpypoint {
	int x,
		y;
};

// ====================================================           ===========================================================
// ==================================================== Variables ===========================================================
// ====================================================           ===========================================================

extern WNDPROC ICAD_OrigWndProcT;
extern WNDPROC ICAD_OrigWndProcH;

static UINT BASED_CODE FRM_FormMBId[]={FRM_FORM_NEW,FRM_FORM_SAVE,ID_SEPARATOR,FRM_FORM_CUT,FRM_FORM_COPY,FRM_FORM_PASTE,ID_SEPARATOR,
                                       FRM_FORM_TEST,ID_SEPARATOR,FRM_FORM_LALIGN,FRM_FORM_RALIGN,FRM_FORM_TALIGN,FRM_FORM_BALIGN,
                                       ID_SEPARATOR,FRM_FORM_VCENT,FRM_FORM_HCENT,ID_SEPARATOR,FRM_FORM_HSACROS,FRM_FORM_VSACROS,
                                       ID_SEPARATOR,FRM_FORM_SWIDTH,FRM_FORM_SHEIGHT,FRM_FORM_SSIZE};
static UINT BASED_CODE FRM_FormFBId[]={FRM_FORM_ARROW,FRM_FORM_TEXT,FRM_FORM_BUTTON,FRM_FORM_CHECK,FRM_FORM_RADIO,FRM_FORM_EDIT,FRM_FORM_LISTBOX};

static UINT indicators[] ={ID_SEPARATOR,FRM_TLPOS_INDICATOR,FRM_WHLEN_INDICATOR};

// ====================================================         ===========================================================
// ==================================================== Classes ===========================================================
// ====================================================         ===========================================================

class FRM_PropDlg;
class FRM_FormEdit;
class FRM_MainDlg;

class FRM_ViewEdit : public CFrameWnd {
	DECLARE_DYNCREATE(FRM_ViewEdit);

    public:
        FRM_FormEdit  *m_formedit;
        FRM_PropDlg   *m_propdlg;
        FRM_MainDlg   *m_mdialog;
        FRM_frmList   *m_FrmBeg,
                      *m_FrmCur;
        FRM_frmInfo   *m_ListEnd,
                      *m_ListCur,
                      *m_OldCur,
                      **m_SelList;
        short          m_idcCursor,
                       m_selcntr;
        HCURSOR        m_prevCursor;
        CPoint         m_GPOff;
        short          m_boxstate,
                       m_pickplace,
                       m_NumofTypes;
        int            m_Controltype,
                       m_TitleHeight;
        long           m_IDCntr;
        CString        m_Formname;
        CPoint         m_lpoint;
        BOOL           m_CtrlFlag,
                       m_Selection,
                       m_MoveFlag,
                       m_SingleGrp;
		FRM_cpypoint  *m_lcpoint;
        UINT           m_nChar;

    public:
        void  FRM_OnAlign(UINT type);
        void  FRM_AddSelHead();
        void  FRM_FreeSelList();
        void  FRM_FreeItem();
        void  FRM_FreeAll();
        void  FRM_AddDlgHead();
        void  FRM_FreeDlgLinks();
        void  FRM_AddHead(CWnd *hItem);
        BOOL  FRM_DrawRect(CPoint point,CPoint lpoint,short state,RECT offset,RECT loffset);
        BOOL  FRM_SelectItem(RECT rect,short style);
        void  FRM_FormSave(struct FRM_frmList *form,BOOL type);
        void  OnTestDlg();
        void  OnEditDlg(LPCTSTR String);
        int   Memtotal(int size,CWnd *Item);
        void  OnNewDlg();
        void  ChangeDlg();
        int   GetWindowType();
        void *MakeTemplate(CWnd *ptr,long *memsize);
        void  PropertyDlg();
        void  OnNewFont();
        void  OnDeleteItem();
        void  FRM_MoveItem(struct FRM_frmInfo *Item,BOOL type);
        void *MakePicture(CWnd *ptr);
	    void  OnMoveItems(UINT type);
		void  OnSelectAll();
        long  FRM_Makebitmap(HBITMAP hBMP,HDC hDC,BYTE **hp);
        void  SaveDlgChanges();
        void  OnCopy(BOOL Ctype);
        void  OnPaste();

    protected:
      afx_msg void OnCaptureChanged(CWnd* pWnd);
      afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
      afx_msg void OnEnable(BOOL bEnable);
      afx_msg void OnLButtonUp( UINT nFlags, CPoint point);
      afx_msg void OnLButtonDown( UINT nFlags, CPoint point);
      afx_msg void OnRButtonDown( UINT nFlags, CPoint point);
      afx_msg void OnPaint();
      afx_msg void OnMouseMove( UINT nFlags, CPoint point);
      afx_msg void OnClose();
  	  afx_msg void OnContextMenu(CWnd* pWnd, CPoint pos);
      virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs,CCreateContext* pContext);

    DECLARE_MESSAGE_MAP()
};

class FRM_SubDlg : public CDialog {
    public:
        FRM_PropDlg *m_PropDlg;
        short        m_oldindex,
                     m_DlgIndex;
        BOOL         m_Inuse;

    public:
   	    BOOL OnInitDialog();
        void ChangItemStyle(long style);

    protected:
                void DoDataExchange(CDataExchange *pDX);
        afx_msg void OnDestroy();
        LRESULT      WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
				void FillCombo_EDT_STYLE();
				void FillCombo_LB_STYLE();
				void FillCombo_TXT_STYLE();

    DECLARE_MESSAGE_MAP()
};

class FRM_PropDlg : public CDialog {
    public:
        FRM_ViewEdit *m_DlgWin;
        FRM_SubDlg    m_SubDlg;
        short         m_DlgIndex,
                      m_NumofTypes;

	    BOOL OnInitDialog();

    protected:
        afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
        afx_msg void OnTack();
        afx_msg void OnChangeItem();
        afx_msg void OnPaint();
        afx_msg void OnActivate(UINT nState,CWnd* pWndOther,BOOL bMinimized);
        afx_msg void OnClose();
        LRESULT      WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

    DECLARE_MESSAGE_MAP()
};

class FRM_MainDlg : public CDialog {
    public:        FRM_ViewEdit *m_DlgWin;

    protected:
        afx_msg void OnPaint();

    DECLARE_MESSAGE_MAP()
};

class FRM_StatusBar : public CStatusBar {
    DECLARE_DYNCREATE(FRM_StatusBar)

    public:
        CRect m_recttl,
              m_rectwh;
        RECT  m_rectinfo;

    public:
        void UpdateBar(RECT rect);

    protected:
        virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	    DECLARE_MESSAGE_MAP()
};

class FRM_FormEdit : public CFrameWnd {
	DECLARE_DYNCREATE(FRM_FormEdit);

    public:
        FRM_ViewEdit *m_DlgWin;
        FRM_StatusBar m_StatusBar;
        CSplitterWnd  m_splitWnd;
    	CToolBar      m_FToolBar,
                      m_MToolBar;
        char         *m_Formname;
        CWnd         *m_Parent;

    protected:
              void OnSize(UINT nType, int cx, int cy);
      afx_msg void OnClose( );
      afx_msg void FormControls(UINT Id);
      afx_msg void OnDestroy();
      afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
      afx_msg void OnKeyDown(UINT nChar,UINT nRepCnt,UINT nFlags);
      afx_msg void OnKeyUp(UINT nChar,UINT nRepCnt,UINT nFlags);
      virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs,CCreateContext* pContext);
      virtual BOOL PreTranslateMessage( MSG* pMsg );

    DECLARE_MESSAGE_MAP()
};

// ====================================================						  ===========================================================
// ==================================================== Function Declorations ===========================================================
// ====================================================                       ===========================================================

void         DeleteForm(CString name);
inline void  VarToCStr(CString *c, COleVariant *v);
inline void  VarToBinay(BYTE **c, COleVariant *v);
int          FRM_ItemLocSort(const void *arg1,const void *arg2);
void         sortlist(short cntr);
void         FRM_Showbitmap(BYTE *pbytes,CWnd *pWin);
BOOL         FRM_dcltoform(char *fs1,HWND hWnd);

// ====================================================						  ===========================================================
// ==================================================== Function Declorations ===========================================================
// ====================================================                       ===========================================================

#pragma pack(8)

typedef struct {
    long  dt_Style;
    long  dt_ExStyle;                   // Dialog box Extended style.
    short dt_ItemCount;                 // Number of controls.
    short dt_X;                         // Top left x.
    short dt_Y;                         // Top left y.
    short dt_CX;                        // Dialog width.
    short dt_CY;                        // Dialog height.
} FRM_TEMPLATE, *FRM_LPDLGTEMPLATE;   // Holds information that applies to the
                                        // entire dialog box.
typedef struct {
    long  dtil_Style;
    long  dtil_ExStyle;                 // Control extended style.
    short dtil_X;                       // Top left x corner of control.
    short dtil_Y;                       // Top left y corner of control.
    short dtil_CX;                      // Width of control.
    short dtil_CY;                      // Height of control.
    short dtil_ID;                      // ID of control.
} FRM_ITEMTEMPLATE, *FRM_LPDLGITEMTEMPLATE;
                                       // This structure is for every control that
                                       // will appear in the dialog-box.
#pragma pack()


