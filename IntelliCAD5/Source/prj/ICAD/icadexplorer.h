/* ICAD\ICADEXPLORER.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * Header file for the IntelliCAD Explorer.
 *
 */

#pragma once

#include "IcadToolBar.h"
#include "ListCtrlEx.h"
#include "ExplorerPropsTab.h"
#include "IcadDialog.h"

// ============================ 		================================
// ============================ Defines ================================
// ============================ 		================================

#define IEXP_LATAB_IDX		  0 				  // Index of the layer table.
#define IEXP_LTTAB_IDX		  1 				  // Index of the linetype table.
#define IEXP_TSTAB_IDX		  2 				  // Index of the text style table.
#define IEXP_CSTAB_IDX		  3 				  // Index of the UCS table.
#define IEXP_VWTAB_IDX		  4 				  // Index of the view table.
#define IEXP_BNTAB_IDX		  5 				  // Index of the blocks table.
#define IEXP_DSTAB_IDX		  6 				  // Index of the dimstyles table.
#define IEXP_XDATA_IDX		  7

//EBATECH(CNBR) -[ used for IEXP_EntityCol[IEXP_LATAB_IDX.Order
#define IEXP_LA_NAME			0
#define IEXP_LA_COLOR			1
#define IEXP_LA_LINETYPE		2
#define IEXP_LA_ON_OFF			3
#define IEXP_LA_LOCKED			4
#define IEXP_LA_FROZEN			5
#define IEXP_LA_LINEWEIGHT		6
#define IEXP_LA_PLOT			7
#define IEXP_LA_VP_FREEZECUR	8
#define IEXP_LA_VP_FREEZENEW	9

//EBATECH(CNBR) -[ used for IEXP_TABINFO[IEXP_LATAB_IDX].ItemID
#define IEXP_PLA_NAME		0
#define IEXP_PLA_COLOR		1
#define IEXP_PLA_LINETYPE	2
#define IEXP_PLA_LINEWEIGHT	3
#define IEXP_PLA_ON_OFF		4
#define IEXP_PLA_FROZEN		5
#define IEXP_PLA_LOCKED		6
#define IEXP_PLA_PLOT		7
#define IEXP_PLA_VPNEW		8

//EBATECH(CNBR) -[ text style column index
#define IEXP_TS_NAME		0
#define IEXP_TS_HEIGHT		1
#define IEXP_TS_WIDTH		2
#define IEXP_TS_SLANT		3
#define IEXP_TS_FONT		4
#define IEXP_TS_STYLE		5
#define IEXP_TS_LANG		6
#define IEXP_TS_BIGFONT		7
#define IEXP_TS_BACKWORD	8
#define IEXP_TS_UPSIDEDOWN	9
#define IEXP_TS_VERTICAL	10
//EBATECH(CNBR) ]-

#define IEXP_TREEVIEW		  1000				  // Id for the (CTreeView) on the left side of the explorer.
#define IEXP_LISTVIEW		  1001				  // Id for all the (CDetailViews) on the right side of the explorer,"All the list view have the same id".
#define IEXP_SBAR_TV		  1002				  // Id for the top status bar.
#define IEXP_SBAR_BTV		  1003				  //
#define IEXP_SBAR_IV		  1004				  // Id for the static bar on the left side of the window.
#define IEXP_SBAR_BIV		  1005				  // Id for the static bar on the right side of the window.
#define IEXP_BLOCKSLIST 	  1006				  // Id for the (CWnd) blocks list.
#define IEXP_MAX_ITEMLEN	  64				  // The maximum text length in the first column in the (CListCtrl)

#define IEXP_CHANGEROW		  WM_USER+100		  // Case id sent to the window procedure when the user selects one of the item listings in the left window.
#define IEXP_INSERTROW		  WM_USER+101		  // Case id sent to the window procedure when the user inserts a new row.
#define IEXP_PROPERTY		  WM_USER+102		  // Case id sent to the window procedure when the user selects the property option.
#define IEXP_CHANGEITEM 	  WM_USER+103		  // Case id sent to the window procedure when the user selects one of the items in the right window.
#define IEXP_ADD_ITEM		  WM_USER+104		  // Case id sent to the window procedure when a new item is inserted in the left window.
#define IEXP_EXIT_EXP		  WM_USER+105		  //
#define IEXP_WM_NOTIFY		  WM_USER+106		  //
#define IEXP_ITEMCHANGED	  WM_USER+107		  //
#define IEXP_CHANGINGITEM	  WM_USER+108
#define IEXP_DBLCLK 		  WM_USER+109
#define EXP_LAYER_ON_M		  WM_USER+110
#define EXP_LAYER_FREEZE_M	  WM_USER+111
#define EXP_LAYER_LOCK_M	  WM_USER+112
#define EXP_LAYER_VISRETAIN_M WM_USER+113

#define IEXP_COL_ORDER		  0 				  // Index of the postion of the coloumns order in the (order) field of IEXP_EntityCol structure.
#define IEXP_COLUMNWIDTH	  1 				  // Index of the columns width in the (order) field of IEXP_EntityCol structure.

#define IEXP_LATAB_FLG		  0x0001			  // Id used in the (CutItem) function for Layers type.
#define IEXP_LTTAB_FLG		  0x0002			  // Id used in the (CutItem) function for Linetypes type.
#define IEXP_STYLE_FLG		  0x0004			  // Id used in the (CutItem) function for the Text Styles type.
#define IEXP_UCS_FLG		  0x0008			  // Id used in the (CutItem) function for the UCS type.
#define IEXP_VIEW_FLG		  0x0010			  // Id used in the (CutItem) function for the View type.
#define IEXP_BLOCK_FLG		  0x0020			  // Id used in the (CutItem) function for the Blocks type.

#define IEXP_HI_BIT_SHORT	  0x8000			  // This is used to check if the high order bit is one in the (GetKeyState) function.
#define IEXP_DEL_FLG		  0x80				  // This is used to check if the delete bit is set for the drawing tables.
#define IEXP_XREF_FLG		  0x30				  //
#define IEXP_DLG_W_SPACER	  40				  //
#define IEXP_DLG_H_SPACER	  65				  //
#define IEXP_BITMAP_WIDTH	  16				  // The width of the bitmaps used in the (CListCtrl), right side of the window.
#define IEXP_BITMAP_HEIGHT	  16				  // The height of the bitmaps used in the (CListCtrl), right side of the window.
#define IEXP_ICON_BITMAP_WIDTH	48
#define IEXP_ICON_BITMAP_HEIGHT 48
#define IEXP_BITMASK		  0xFFFFFF			  // Bitmask used in the Add function of the images.
#define IEXP_VARIES_I		  2 				  // Interger used when multiple items are selected, sort of a flag.
#define	IEXP_COMMA			  ','
#define IEXP_STANDARD			"STANDARD"
#define IEXP_COLOR_WHITE		"white" 			// The rest are constant strings.
#define	IEXP_CONTINUOUS			"CONTINUOUS"
#define IEXP_BYBLOCK			"BYBLOCK"
#define IEXP_BYLAYER			"BYLAYER"
#define IEXP_WORLD				"WORLD"
#define IEXP_DEFAULT_FONT		"txt.shx"
#define IEXP_STYLEDEFLT			""
#define IEXP_LAYER				"LAYER"
#define IEXP_LTYPE				"LTYPE"
#define IEXP_STYLE				"STYLE"
#define IEXP_UCS				"UCS"
#define IEXP_VIEW				"VIEW"
#define IEXP_BLOCK				"BLOCK"
#define	IEXP_DIMSTY 			"DIMSTYLE"
#define IEXP_SCOMMA 			","
#define IEXP_BUTTON 			"BUTTON"
#define	IEXP_VER_BAR			'|'
#define	IEXP_ZERO				"0"

const int IEXP_TEXT_STYLE_TAB_BOTTOM = 145;	// need to enlarge container for text style tab
const int IEXP_TEXT_STYLE_TAB_RIGHT = 102;	// need to enlarge container for text style tab
const int IEXP_LAYER_TAB_BOTTOM = 30;
const int IEXP_LAYER_TAB_RIGHT = 0;

// =============================================================================================================================================
// ============================================================ FUNCTION DECLORATIONS ==========================================================
// =============================================================================================================================================

LRESULT			PaintProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
BOOL			IEXP_strtopointr(char *string,double point[3]);
void			IEXP_CnvLTtoString(double farray[12],short cntr,CString *string);
short			IEXP_help(void);
void			IEXP_AddNewOpenDoc(void);
void			IEXP_FreeOpenDoc(void);
BOOL			CleanupEntsInsideBlocks
	(
	int		ItemType,			//Type of attribute of the block's members need cleanup - Layer, Linetype or Style
	CString	ItemName,			//Value of the Attribute that needs cleanup
	int		ActionType,			//Type of cleanup - Change or Delete
	CString NewItemName,		//If type of Cleanup is Change, this is the new value of the attribute, else NULL
	db_drawing *drawing			//The drawing being modified
	);

class IEXP_CListCtrl;
// EBATECH(CNBR) -[ Add langName
//long GetFontStyleNumber(IEXP_CListCtrl listControl[], short listIndex, int styleIndex,
//	const char *styleName);
long GetFontStyleNumber(IEXP_CListCtrl listControl[], short listIndex, int styleIndex,
	const char *styleName, const char *langName);
// EBATECH(CNBR) ]-

// =============================================================================================================================================
// ============================================================== GLOBAL STRUCTURES ============================================================
// =============================================================================================================================================

// This structure is used to store all the information for each block in the blocks listing, icon mode window.
struct IEXP_BlksList {
	short	 textlen,
			 index;
	BOOL	 current;
	POINT	 pt;
	CBitmap *cBitmap;
	long	 dataptr;
};

// =============================================================================================================================================
// ================================================================ GLOBAL CLASSES =============================================================
// =============================================================================================================================================

class IEXP_CdetailView;
class IEXP_FormEdit;


class CTreeCtrlEx : public CTreeCtrl
{
// Construction
public:
	CTreeCtrlEx() { };

public:
	virtual ~CTreeCtrlEx() { };

	// Generated message map functions
protected:
	//{{AFX_MSG(CTreeCtrlEx)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};



class IEXP_CItemList : public CFrameWnd {
	DECLARE_DYNCREATE(IEXP_CItemList)

	public:
		CTreeCtrlEx 	  m_TreeCtl;	  // tree view control
		CImageList		  m_ImageList;	  // image list associated with tree
		CStatic 		  m_staticTvbar,  // Static bar.
						  m_staticTvbtm;  // Static bar.
		IEXP_CdetailView *m_CdetailView;
		IEXP_Explorer	 *m_Explorer;

   		BOOL	  AddTreeViewItems();
		BOOL	  AddOneTreeViewItem(LPCTSTR name,short index);
		HTREEITEM AddOneItem(HTREEITEM,LPCTSTR,HTREEITEM,int,int,short,UINT);
		void	  UpdateEntityCOl(void);

	protected:
		afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg void OnItmChanged(LPNMHDR pnmhdr, LRESULT *pResult);
		afx_msg void OnSetFocus(CWnd* pOldWnd);
		afx_msg void OnChar( UINT nChar, UINT nRepCnt, UINT nFlags );

		LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);


	DECLARE_MESSAGE_MAP()
};

class IEXP_CdetailView;

class IEXP_CListCtrl : public CListCtrlEx
{
public:
	IEXP_CdetailView *m_CdetailView;
	int GetActiveItem();
	BOOL ValidateData (int nRow, int nCol);

	//Overrides
	void DrawSubItem(int nItem, int nColumn, LV_COLUMN& lvc, CRect& rcCol, CDC *pDC);
	void initData(IEXP_CdetailView* view, int Mindex);
	BOOL DeleteAllItems();
	IEXP_CListCtrl(): m_bInit(false){}
protected:
	bool m_bInit;
	afx_msg void OnLButtonDblClk(UINT nFlags,CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags,CPoint point);
	afx_msg void OnChar( UINT nChar, UINT nRepCnt, UINT nFlags );

	DECLARE_MESSAGE_MAP()
};

class IEXP_EditDlg;

class IEXP_CdetailView : public CFrameWnd {
	DECLARE_DYNCREATE(IEXP_CdetailView);

private:
	char*				m_pBlockName;	/*DG - 6.5.2002*/// the name of the last pasted block

public:
		IEXP_CListCtrl	m_ListCtl[8];
		CStatic 		m_staticIvbar,
						m_staticIvbtm;
		CImageList		m_pimagelist,
						m_pimagelistI;
		IEXP_EditDlg   *m_EditDlg;
		IEXP_CItemList *m_CItemList;
		IEXP_Explorer  *m_Explorer;
		BOOL			m_DlgState,
						m_DlgTack,
						m_selflag;
		long			m_dataptr;
		short			m_ItemIndex,
						m_Layer_zero;
		POINT			m_lpoint;
		BOOL			m_regenstate;
		BOOL			m_BlockState,
						m_BlkSpecialflg,
						m_Newflag,
						m_MessageType,
						m_BlockInsCreate,
						m_Purge,
						m_Windowsideflag;
		CString			m_fullName;

		static bool		m_ShowXrefLayers; // It controls visibility of layers in xref files in explorer window. 

	public:

		virtual void OnDraw(CDC* pDC) { }
		afx_msg void ChangeWindow(WPARAM OldIndex,LPARAM NewIndex);
		afx_msg void Onhelp(void);
 		static	int  CALLBACK ListViewCompareProc(LPARAM, LPARAM, LPARAM);
		afx_msg void SaveWblock();
		afx_msg void Insertblock(short mode);
		afx_msg void LayerAtt(UINT Id);
		afx_msg void BlkListType(UINT Id);
		afx_msg void PropertyDlg();
		afx_msg void RemoveItem();
		afx_msg void CurrentItem();
		afx_msg void RenameItem();
		afx_msg void CopyCutItem(BOOL type);
		afx_msg void PasteItem();
		afx_msg void InsertRow(UINT Id);
		afx_msg void UpdateBlkCurrent();
		afx_msg void OnSetFocus(CWnd* pOldWnd);
		afx_msg void OnClose();
		afx_msg void SelectAll();
				BOOL LayerIns(short rindex,short index,short mindex,char *string,LV_ITEM lvI,struct resbuf *rb);
				BOOL LinetIns(short *rindex,short index,short mindex,char *string,LV_ITEM lvI,BOOL addbl,struct resbuf *rb);
				BOOL StyleIns(short rindex,short index,short mindex,char *string,LV_ITEM lvI,struct resbuf *rb);
				BOOL UcsIns(short rindex,short index,short mindex,char *string,LV_ITEM lvI,BOOL addbl,struct resbuf *rb);
				BOOL BlockIns(short rindex,short index,short mindex,char *string,LV_ITEM lvI,struct resbuf *rb);
				BOOL ViewIns(short rindex,short index,short mindex,char *string,LV_ITEM lvI,struct resbuf *rb);
				BOOL DimStyleIns(short rindex,short index,short mindex,char *string,LV_ITEM lvI,struct resbuf *rb);
				BOOL RowIns(char *string,short index,long lparam,short mindex,LV_ITEM lvI,const char *name);
				BOOL ItemListIns(LV_ITEM lvI);
				BOOL UpdateDataList(void);
				LRESULT	NotifyHandler(UINT message, WPARAM wParam, LPARAM lParam);
				// Modified CyberAge AP 09/04/2000[
				BOOL UpdateDataList1(short SetFlag);
				void InsertDataList(int type, short SetFlag);
				// Modified CyberAge AP 09/04/2000]
				//Added Cybage AJK 10/12/2001 [
				//Reason : Fix for bug no 77922
				void ShowXrefLayers();
				void RemoveXrefLayers();
				//Added Cybage AJK 10/12/2001 DD/MM/YYYY ]
				//Bugzilla No. 78037 ; 29-04-2002
				void IEXP_UpdateXrefName(char *origname,char * newname);
				//Bugzilla No. 78162 ; 20-05-2002 
				void IEXP_UpdateLineTypeName(struct resbuf * rbb);

	protected:
		afx_msg int 	OnCreate(LPCREATESTRUCT lpCreateStruct);
		afx_msg void	OnSize(UINT nType, int cx, int cy);
		afx_msg void	OnChar( UINT nChar, UINT nRepCnt, UINT nFlags );
				LRESULT NotifyBlkHandler(UINT message, WPARAM wParam, LPARAM lParam);
				LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

class IEXP_Newdlg : public CDialog {
	public :
		BOOL	 m_Dlgflag;
		BOOL	 m_state;
		long	 m_top,
				 m_left;
		CString  m_String,
				 m_type;
		CWnd	*m_ListWnd;

		IEXP_Newdlg() : CDialog(EXP_DEL_NEWLIST) { }
	protected:
		afx_msg void OnDblClk();
		BOOL		 OnInitDialog();
		afx_msg void OnActivate(UINT nState,CWnd* pWndOther,BOOL bMinimized);

	DECLARE_MESSAGE_MAP()
};

class IEXP_MessItem : public CDialog {
	public :
		CString 	m_txtmes,
					m_Continue,
					m_BtnTwo,
					m_txtcnt,
					m_txtchng,
					m_txtcancel,
					m_title;

		IEXP_Newdlg m_Newdlg;
		short		m_typeflag;

		void		OnMove();

	public:
		IEXP_MessItem() : CDialog(EXP_DEL_DIALOG) { }

	protected:
		void		 OnContinue();
		void		 OnCancel();
		virtual BOOL OnInitDialog();
		virtual void DoDataExchange(CDataExchange *pDC);
		afx_msg void OnActivate(UINT nState,CWnd* pWndOther,BOOL bMinimized);
		afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);

	DECLARE_MESSAGE_MAP()
};

class IEXP_LTList : public CDialog {
	public:
		BOOL	   m_ret;
		CListCtrl *Item;
		CString    m_Filename,
				   m_linetypes;

		IEXP_LTList() : CDialog(EXP_LINEDLG) { }

	public:
		static int CALLBACK LtListProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

	protected:
		afx_msg void OnOK();
		afx_msg void OnBrowse2();
		afx_msg void OnCancel();
		virtual BOOL OnInitDialog();
		LRESULT	ListNotifyHandler(UINT message, WPARAM wParam, LPARAM lParam);
		LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
		void CleanupDialog();

	DECLARE_MESSAGE_MAP()
};

class IEXP_LTDlg : public IcadDialog {
	public :
		CString m_Desc,
				m_Defstr,
				m_Definition;
		BOOL	m_defstate;

		IEXP_LTDlg() : IcadDialog(EXP_LTNEW) { }
	protected:
		virtual BOOL OnInitDialog();
		virtual void DoDataExchange(CDataExchange *pDC);
		afx_msg void OnDefUpd();
		afx_msg void OnBrowse();
		afx_msg void OnHelp();


	DECLARE_MESSAGE_MAP()
};

class IEXP_YYTOANC : public CDialog {
	public:
		CString m_Blockname;
		BOOL	m_bNested;
		IEXP_YYTOANC() : CDialog(IEXP_YYTOANOCAN) { }

	protected:
		void		 OnYes();
		void		 OnYesToAll();
		void		 OnNo();
		void		 OnCanel();
		virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
};

class IEXP_EditDlg : public CDialog {
	public:
		CExplorerPropsTab m_SubDlg;

		long			  m_dataptr;
		short			  m_DlgIndex;
		IEXP_CdetailView *m_CdetailView;

		BOOL OnInitDialog();

	protected:
		LRESULT 	 WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
		afx_msg void ShutdownDlg();
		afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
		afx_msg void OnTackClick();
		afx_msg void OnHelpClick();
		afx_msg void OnTackDblClick();
		afx_msg void OnChangeItem();
		afx_msg void OnPaint();
		afx_msg void OnActivate(UINT nState,CWnd* pWndOther,BOOL bMinimized);
		afx_msg void OnClose();

	DECLARE_MESSAGE_MAP()
};

class IEXP_Explorer : public CFrameWnd {

	DECLARE_DYNCREATE(IEXP_Explorer)

	public:
		CMainWindow 	   *m_MainWindow;
		CStatusBar			m_StatusBar;
		CSplitterWnd		m_splitWnd;
		CIcadToolBar		m_MToolBar,
							m_LToolBar,
							m_BlkToolBar;
		IEXP_CItemList	   *m_CItemList;
		IEXP_CdetailView   *m_CdetailView;
		short				m_initialwin;
		BOOL				m_editmode;
		CImageList		   *m_pDragImage;
		CRect				m_rectScrn;
		// duplicates REGENMODE system variable for current drawing
		int m_regenMode;

		virtual BOOL DestroyWindow();
		virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs,CCreateContext* pContext);
				void LayerEnableM(BOOL state);
				void EnableTbB(CIcadToolBar *Toolbar,UINT id,BOOL type);
				void UpdLayerState(LPARAM Dataptr,short type);
				void UpdBlockState(int type);
				void BlockEnableM(BOOL state);
				void OnHelp();
				void ShowExpolorer(void);
				BOOL ActivateCurrentView(db_drawing *dp);

	protected:
		afx_msg void OnActivate(UINT nState,CWnd* pWndOther,BOOL bMinimized);
		afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
		afx_msg void OnToolbarState();
		afx_msg void OnBlkListType(UINT Id);
		afx_msg void OnLayerAtt(UINT Id);
		afx_msg void OnCutItem();
		afx_msg void OnCopyItem();
		afx_msg void OnPasteItem();
		afx_msg void OnNewItem();
		afx_msg void OnWorld();
		afx_msg void OnCurrent();
		afx_msg void OnRemoveItem();
		afx_msg void OnRenameItem();
		afx_msg void PropertyDlg();
		afx_msg void OnInsertRow(UINT Id);
		afx_msg void OnSetFocus(CWnd* pOldWnd);
		afx_msg void OnSbar();
		afx_msg void OnContextMenu(CWnd* pWnd, CPoint pos);
		afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
		afx_msg void OnClose();
		afx_msg BOOL OnSetCursor(CWnd* pWnd,UINT nHitTest,UINT message);
		afx_msg void OnLButtonUp( UINT nFlags, CPoint point );
		afx_msg void OnNcLButtonDown( UINT nHitTest, CPoint point );
		afx_msg void OnRedraw();
		afx_msg void OnRegen();
		afx_msg void OnSelectAll();
		afx_msg void OnPurge();
		afx_msg void OnHelpInfo(HELPINFO* lpHelpInfo);
		afx_msg void OnRegenSwitch();

	DECLARE_MESSAGE_MAP()
};




