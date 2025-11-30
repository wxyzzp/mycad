/* File  : <DevDir>\source\prj\icad\IcadToolBarMain.h
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 */

#ifndef	_ICADTOOLBARMAIN_H
#define	_ICADTOOLBARMAIN_H


#include "IcadToolBar.h"/*DNT*/
#include "IcadCusToolBar.h"/*DNT*/

// The WndActions are 4000-5000
#define TB_START_ID	15000
#define TB_END_ID	17999

#define ICAD_TBID_START	18000
#define ICAD_TBID_END	18999

#define ICAD_MRUFILELIST_START 19000
#define ICAD_MRUFILELIST_END   19049

#define ICAD_CONTEXTMENU_START 19050
#define ICAD_CONTEXTMENU_END   19099

#define ICAD_TEXTSCREENMENU_START 19100
#define ICAD_TEXTSCREENMENU_END 19599

#define ICAD_ACCELID_START	19600
#define ICAD_ACCELID_END	20599

#define IDR_ICAD_TOOLBAR0   102
#define IDR_ICAD_TOOLBAR10  112

// Button size and image size for large buttons.
// These sizes are consistent with Microsoft Word.
#define ICAD_LG_BUTTONX 31
#define ICAD_LG_BUTTONY 30
#define ICAD_LG_IMGX 24
#define ICAD_LG_IMGY 22

// Button size and image size for small buttons.
#define ICAD_SM_BUTTONX 23	// Documentation says the default button width is 24 pixels,
#define ICAD_SM_BUTTONY 22	// but I found that to be untrue.  These are the correct sizes.
#define ICAD_SM_IMGX 16
#define ICAD_SM_IMGY 15

#define ICAD_SEP_WIDTH 9	// Width of spacers

// Used with QueryInsertAt()
#define ICAD_INSERTBEFORE	0
#define ICAD_INSERTAFTER	1
#define ICAD_INSERTON		2

/////////////////////////////////////////////////////////////////////
//	Visibility defines
/////////////////////////////////////////////////////////////////////

// User Levels
#define ICAD_MN_BEG		0x00000001L	// Beginner
#define ICAD_MN_INT		0x00000002L	// Intermediate
#define ICAD_MN_EXP		0x00000004L	// Expert
#define ICAD_MN_LEV		(ICAD_MN_BEG | ICAD_MN_INT | ICAD_MN_EXP)	// All levels

// MDI Window
#define ICAD_MN_OPN		0x00000008L	// At least one open
#define ICAD_MN_CLS		0x00000010L	// No MDI windows
#define ICAD_MN_MDI		(ICAD_MN_OPN | ICAD_MN_CLS)	// Either MDI state

// OLE
#define ICAD_MN_SEM		0x00000020L	// Server, embedded
#define ICAD_MN_SIP		0x00000040L	// Server, in-place
#define ICAD_MN_CLI		0x00000080L	// Client
#define ICAD_MN_OLE		(ICAD_MN_SEM | ICAD_MN_SIP | ICAD_MN_CLI)	// Any OLE state

// ALL of the above
#define ICAD_MN_ALL		0x000000ffL

#define ICAD_MN_RCO		0x00000100L	// Context (right-click) menu ONLY(applies to pop17, modify).

#define ICAD_MN_HIDE	0x00000200L	// Just plain Hide (created for tear-off menus).

#define ICAD_MN_TOM		0x00000400L	// Temporary osnap mode (pop0 shift rightclick when command is active).

#define ICAD_MN_CTRL  0x00000800L // Buttin is a control (like a combobox, edit, ...)

extern const int g_DefaultMenuVisibility;

/////////////////////////////////////////////////////////////////////
//	Visibility defines for entities
/////////////////////////////////////////////////////////////////////

#define ICAD_MN_PNT		0x00000001L	// Point
#define ICAD_MN_LIN		0x00000002L	// Line
#define ICAD_MN_RAY		0x00000004L	// Ray
#define ICAD_MN_XLN		0x00000008L	// XLine
#define ICAD_MN_ARC		0x00000010L	// Arc
#define ICAD_MN_CIR		0x00000020L	// Circle
#define ICAD_MN_ELL		0x00000040L	// Ellipse

#define ICAD_MN_SHP		0x00000080L	// Shape
#define ICAD_MN_TRC		0x00000100L	// Trace
#define ICAD_MN_SLD		0x00000200L	// Solid

#define ICAD_MN_3DF		0x00000400L	// 3D Face
#define ICAD_MN_3DS		0x00000800L	// 3D Solid

#define ICAD_MN_2DP		0x00001000L	// 2D Polyline
#define ICAD_MN_3DP		0x00002000L	// 3D Polyline
#define ICAD_MN_PFM		0x00004000L	// Polyface Mesh
#define ICAD_MN_3DM		0x00008000L	// 3D Mesh
#define ICAD_MN_PLN		(ICAD_MN_2DP | ICAD_MN_3DP | ICAD_MN_PFM | ICAD_MN_3DM)	// All types of polylines

#define ICAD_MN_TXT		0x00010000L	// Text

#define ICAD_MN_INS		0x00020000L	// Insert

#define ICAD_MN_ATD		0x00040000L	// AttDef

#define ICAD_MN_DIM		0x00080000L	// Dimension
#define ICAD_MN_LDR		0x00100000L	// Leader
#define ICAD_MN_TOL		0x00200000L	// Tolerance

#define ICAD_MN_SPL		0x00400000L	// Spline
#define ICAD_MN_MTX		0x00800000L	// Mtext
#define ICAD_MN_MLN		0x01000000L	// Mline
#define ICAD_MN_GRP		0x02000000L	// Group
#define ICAD_MN_IMG		0x04000000L	// Image
#define ICAD_MN_VPT		0x08000000L	// Viewport

#define ICAD_MN_HAT		0x10000000L	// Hatch

#define ICAD_MN_RGN		0x20000000L	// Region
#define ICAD_MN_BDY		0x40000000L	// Body
#define ICAD_MN_ACS		(ICAD_MN_3DS | ICAD_MN_RGN | ICAD_MN_BDY)	// All ACIS entities

#define ICAD_MN_ENT		0x1fffffffL	// All of the above entities

#define ICAD_MN_MUL		0x80000000L	// Exclude from multiple entity selection (break).


// This structure is used in the creation of the toolbars.
typedef struct tagICTOOLBARCREATESTRUCT
{
	CFrameWnd*	pParentFrame;
	BOOL		bToolTips,
				bLargeButtons,
				bColorButtons;
	long		lflgVisibility;
} ICTOOLBARCREATESTRUCT, FAR* LPICTOOLBARCREATESTRUCT;

// FORWARDS
class XMLString;

/////////////////////////////////////////////////////////////////////////////
// CIcadToolBarMain command target
/////////////////////////////////////////////////////////////////////////////

class CIcadToolBarMain : public CCmdTarget
{
	DECLARE_DYNCREATE(CIcadToolBarMain)

// Attributes
public:
	CFrameWnd*			m_pParentFrame;
	BOOL				m_bToolTips,
						m_bLargeButtons,
						m_bColorButtons;
	long				m_lflgVisibility;
	BOOL				m_bCustomize;
	int					m_idxCurButtonDown;			// Index of the customize button with the pressed state set
	CIcadToolBar*		m_pToolBarCurBtnDown;		// A pointer to the toolbar with the current button with the pressed state set.
	CIcadFlyOut*		m_pFlyOutCurBtnDown;		// A pointer to the flyout with the current button with the pressed state set.
	CIcadToolBarCtrl*	m_pToolBarCtrlCurBtnDown;	// A pointer to the custom toolbar with the current button with the pressed state set.
	CIcadCusToolbar*	m_pCusTbDlg;
	CPtrList*			m_pCreateBmpList;

private:
	CPtrList*		m_pToolBarList;
	CIcadTBbutton*	m_pibCurToolTip;	// Pointer to the IcadButton structure containing the current tooltip and helpstring.
	CWnd*			m_pHelpStrOutputWnd;
	BOOL			m_bTbModified;
	CRect			m_rectInsertMark;
	CToolBarCtrl*	m_pTbCtrlInsertMark;

// Operations
public:
	CPtrList*  GetToolBarList();
	void  SetToolBarList(CPtrList* pToolBarList);
	CWnd* GetHelpStringOutputWnd();
	void  SetHelpStringOutputWnd(CWnd* pWnd);
	CIcadTBbutton*  GetCurToolTipPtr();
	void  SetCurToolTipPtr(CIcadTBbutton* pibCurToolTip);
	BOOL  IsModified();
	void  SetModifiedFlag(BOOL bModified = TRUE);
	BOOL  CreateFromRegistry(LPICTOOLBARCREATESTRUCT lpCreateToolBar);
	BOOL  CreateDefault(LPICTOOLBARCREATESTRUCT lpCreateToolBar);
	//BOOL  CreateDefaultButtons(CIcadToolBar* pToolBar, long lToolID);
	BOOL  CreateDefaultButtons(CIcadToolBar* pToolBar, long_double lToolID);
	BOOL  CreateFromMnu(LPCTSTR lpszFile, BOOL bDisplayMessages = TRUE, BOOL bAppend = TRUE);
	void  SerializeDockedState(CArchive& ar, CMapPtrToPtr* pExcludedBars = NULL);
	void  SerializeDockedState(XMLString& str, bool bStoring, CMapPtrToPtr* pExcludedBars = NULL);

#ifdef XML_TOOLBAR
	BOOL  ReadFromXML(LPCTSTR lpszFile);
#ifdef XML_TOOLBAR_AS_DEFAULT
	BOOL  ReadFromXML(LPICTOOLBARCREATESTRUCT lpCreateToolBar);
#endif
#endif
	int   GetVisibleButtonCount(CToolBarCtrl* pToolBarCtrl);
	int   GetFirstVisibleButtonIndex(CToolBarCtrl* pToolBarCtrl);
	int   GetLastVisibleButtonIndex(CToolBarCtrl* pToolBarCtrl);
	int   GetPrevVisibleButtonIndex(CToolBarCtrl* pToolBarCtrl, int idxButton);
	int   GetNextVisibleButtonIndex(CToolBarCtrl* pToolBarCtrl, int idxButton);
	int   AddIcadBitmap(CIcadTBbutton* pibButton, BOOL bFlyOut, CIcadToolBar* pToolBar = NULL,
						CBitmap** ppBmp = NULL, LPCTSTR pszAltBmpPath = NULL, HINSTANCE hResDLL = NULL);
	BOOL  AddToolBarBitmap(CIcadToolBar* pToolBar, CIcadFlyOut* pFlyOut, CBitmap* pBmp = NULL, int* nBmpID = 0);
	void  SetButtonVisibility(long lflgVisibility, BOOL bShow);
	void  UpdateBitmaps();
	void  OnToolTipsChanged();
	void  OnButtonStateChanged();
	void  OnVisibilityChanged();
	void  SaveToRegistry(BOOL bInPlace = FALSE);
#ifdef XML_TOOLBAR
#ifdef XML_TOOLBAR_AS_DEFAULT
	void  SaveToXML(CString& fName = CString(), CMapPtrToPtr* pExcludedBars = NULL);
#else
	void  SaveToXML(CString& fName, CMapPtrToPtr* pExcludedBars = NULL);
#endif
#endif
	BOOL  OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);
	BOOL  OnToolTipTextW(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);
	int   AddDraggedButton(CIcadToolBarCtrl* pFromTbCtrl, CIcadToolBar* pToolBar, CIcadFlyOut* pFlyOut, CPoint ptScrn);
	int   MoveDraggedButton(CIcadToolBar* pToolBar, CIcadFlyOut* pFlyOut, CPoint point, int idxDragButton);
	int   QueryInsertAt(CIcadToolBar* pToolBar, CIcadFlyOut* pFlyOut, CPoint point, BOOL* pbVert, int* pnInsertAt);
	BOOL  DrawInsertMark(CIcadToolBar* pToolBar, CIcadFlyOut* pFlyOut, CPoint point, BOOL bInsAsFlyOut = FALSE);
	void  DeleteExcessSpacers(CToolBarCtrl* pToolBarCtrl, int* idxDragButton, int* idxInsertButton);
	int   GetButtonFromPt(CToolBarCtrl* pToolBarCtrl, CPoint point, LPTBBUTTON lpButton);
	int   GetClosestButtonFromPt(CToolBarCtrl* pToolBarCtrl, CPoint point);
	void  OnCustButtonDown(CIcadToolBar* pToolBar, CIcadFlyOut* pFlyOut, CIcadToolBarCtrl* pToolBarCtrl, int idxButton);
	void  DisplayCustData(CIcadTBbutton* picButton);
	void  FreeIcadButtonData(CIcadTBbutton* picButton);
	int   GetNewTbID();
	BOOL  CreateToolBarFromList(LPCTSTR lpszFile, LPCTSTR lpszGroupName, CPtrList *ptrlist, LPCTSTR title,
								int xpos, int ypos, int placement, int nrows, BOOL show);
	BOOL  ParseMenuInBuffer(LPCTSTR lpszFile,  LPCTSTR lpszGroupName, char *pBuffer, char *helpstrs, char **sortlist,
							int sortcount, char *pSubmenu, CPtrList *pButtonList, CIcadTBbutton* foButtons);
	//CIcadToolBar*  CreateNewToolBar(int idxToolBar, long lDefID, LPCTSTR pszTitle);
	CIcadToolBar*  CreateNewToolBar(int idxToolBar, long_double lDefID, LPCTSTR pszTitle);
	BOOL  CopyButtonInfo(CIcadTBbutton **ppibDest, CIcadTBbutton* pibSource);
	int   AddToBmpList(CBitmap* pBitmap);
	void  DeleteBmpList();
	CBitmap*  CreateToolBarBitmap();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIcadToolBarMain)
	//}}AFX_VIRTUAL

// Implementation
public:
	CIcadToolBarMain();
	virtual ~CIcadToolBarMain();

protected:
	// Generated message map functions
	//{{AFX_MSG(CIcadToolBarMain)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

};


/////////////////////////////////////////////////////////////////////////////

struct IcadMenus
{
#if 0
	char*	menuName;	// Name of Menu
#else
	int		menuNameID;	// ID of string with menu name's
#endif
	//long	lMenuID;	// Bit-set identifier (associates menu items to
	long_double	lMenuID;	// Bit-set identifier (associates menu items to
						// a pulldown menu - bit-set so that one item
						// can be associated to more than one pulldown)
	BOOL	IsSubMenu;	// Is this a submenu
	char*	globalName; // language independent tearoffname, usually "POPNN" with NN a number
};


struct IcadToolBars
{
	char*		  ToolBarName;		// Name of toolbar
	//long	      lToolID;			// Bit-set identifier (associates buttons to
	long_double	      lToolID;			// Bit-set identifier (associates buttons to
									// toolbar - bit-set so that one button can be
									// associated to more than one toolbar)
	short	      TotalButtons;		// Number of buttons associated with this toolbar
	CIcadToolBar* IcadToolBar;		// Pointer to the CToolBar class.
	BOOL		  FlyOut;			// Is this a Flyout
	BOOL		  OtherIcon;		// Should we switch the Icon when FlyOut
};


struct IcadCommands
{
	//long	lToolID,		// Identifies which toolbar to associate to this item.
	long_double	lToolID,		// Identifies which toolbar to associate to this item.
			lMnuID,			// Identifies which menu to associate to this item.
			lFlyOutID,		// Identifies which flyout to associate to this item.
			lSubMenuID;		// Identifies which submenu to associate to this item.
	short	Position;		// Menu Position
	char	*pAcadTbStr,	// AutoCAD toolbar conversion string.
			*pLocalAcadTbStr,
			*Command,		// Store Command to be sent to SDS Event
			*strChekVar,	// Controls the state of menus and buttons based on the value of the setvar
			*strGreyVar;	// Controls whether the button or menu item is available based on the setvar
	short	BMPIdSmColor,	// Contains the Identifier of Sm Color Bitmap
			BMPIdLgColor,	// Contains the Identifier of Lg Color Bitmap
			BMPIdSmBW,		// Contains the Identifier of Sm Monochrome Bitmap
			BMPIdLgBW;		// Contains the Identifier of Lg Monochrome Bitmap
	char	*ToolTip,		// Stores the Tooltip String
			*LocalToolTip,
			*HelpString,	// Stores the Help string
			*LocalHelpString;
	long    lflgVisibility,	// Bit-set value for the visibility of this item.
			lflgEntVis;		// Bit-set value for the entity visibility of this item.
    int     resourceIndex;
};


#endif //_ICADTOOLBARMAIN_H
