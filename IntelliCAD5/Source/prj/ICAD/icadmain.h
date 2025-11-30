/* E:\INTELLICAD\PRJ\ICAD\ICADMAIN.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * CMainWindow class definition
 *
 */

#pragma once

//** Includes
//#include "IcadCmdBar.h"
//#include "IcadTextScrn.h"
#include "IcadStatusBar.h"
//#include "IcadChildWnd.h"
#include "db.h"
#include "drw.h"
#include "LayerComboBox.h" 
#include "ColorComboBox.h" 
#include "LTypeComboBox.h" 
#include "LWeightComboBox.h"/*DNT*/
#include "DimStyleComboBox.h"/*DNT*/


/////////////////////////////////////////////////////////////////////////////

class prn_Print;
interface IVSpell;
class CPrintDialogSheet;
class CCmdBar;
class CIcadMenu;
class CIcadChildWnd;
class CIcadDoc;
class CIcadView;
class CIcadMenuMain;
class COlePasteSpecialDialog;
class CIcadTablet;
class SystemPaths;
class CPromptMenu;
class CTextScreen;
class CBoundaryBuilder;
class C3Point;
class BHatchDialog;

class CMainWindow : public CMDIFrameWnd
{

public:
	//*** Constructor/Destructor
    CMainWindow( bool showSplash);
    virtual ~CMainWindow();

	db_drawing*	CurrentDrawing() const;

	//*** Main application variables.
	DWORD           m_nMainThreadID;
	int		        m_fHasFocus;
	BOOL	        m_bInOpenNew;
	int             m_iAutoSaveEvent;
	drw_readwrite   m_pFileIO;

	BOOL m_bSentChar;

	//*** Database work space variable, pointer variable
    //*** Table pointer variable, and read/write flag.
	CDaoWorkspace   m_CdbWorkSpace;
    CDaoDatabase    m_CdatabasePtr;
    CDaoRecordset  *m_CdatarecordPtr;
    BOOL            m_readwrite;
    BOOL            m_bDaoRunning;
    //*** Status Bar members variables
    CIcadStatusBar  m_StatusBar;
	int             m_nMaxCoordsLen;
    char            m_strCoords[128];
    //*** Command line/Text screen member variables
    CCmdBar*        m_pCmdBar;
    CTextScreen*    m_pTextScrn;
	CStringList*    m_pHistList;
	CString*        m_pCmdStr;
	CString         m_strOpenFname;
	CPromptMenu*    m_pPromptMenu;
	bool  			m_bPrintHoldFill;
	//*** View member variables
	bool            m_bUsingTempView;
    UINT            m_ViewCursorType;
	CIcadView*      m_wndViewFocus;
	CIcadView*      m_wndViewClose;
	CIcadView*      m_pCurView;
	CIcadDoc*       m_pCurDoc;
	CIcadChildWnd*  m_pCurFrame;
	int             m_nOpenViews;
	int             m_nIsClosing;
	db_charbuflink m_CurSession;
    db_charbuflink m_CurConfig;
	db_handitem   *m_pViewTabEntryHip;
	int             m_nLastMDIState;
	CRect          *m_rectCreateView;
	CRect           m_rectMDIWin;
	CPalette       *m_pPalette;
	BOOL			m_bMapToRGBColor;
	BOOL            m_bIsAppActive;
	short           m_bIsRecovering;	// BIT flag 0x1 - lets open know we're recovering,
										//			0x2 - audit info saved to file.
	//*** Macro recording file.
	CFile           m_fMacroRecFile;
	//*** Log file.
	CFile           m_fLogFile;
	//*** For Dragging.
	struct SDS_dragvars *m_pDragVarsCur;
  sds_point       m_pKeyDragPt;
	short           m_fIsCtrlDown;
	short           m_fIsAltDown;
	short           m_fIsShiftDown;
    //*** XdataEdit variables.
    class Xdata_Info *m_XdataEdit;
	//*** This is used for explorer to sendmessage to calling func
    HWND  m_ExpReturnHwnd;
    //*** This will be used for ddinsert and ?????
    bool  m_IgnoreLastCmd;
    //*** Explorer variables.
    class IEXP_Explorer *m_pExplorer;
    BOOL  m_bExplorerState,
          m_bExpInsertState;
	//*** Form Builder variables.
    class RFRM_formdlg  *m_formdlg;
    //*** Tool Bar & Menu Member Variables
	CCmdTarget*     m_pToolBarMain;
	CPropertySheet* m_pIcadCustom;
	CPtrList*       m_pAccelList;
	CIcadMenuMain*  m_pMenuMain;
	BOOL			m_bCustomize;

  // TECNOBIT(Todeschini) 2003.09.05 -[ Toolbar comboboxes
  CLayerComboBox m_wndLayers; 
  CColorComboBox m_wndColor; 
  CLTypeComboBox m_wndLType; 
  CLWeightComboBox m_wndLWeight; 
  CDimStyleComboBox m_wndDimstyle;

  // ]-

	//*** DDE Member Variables
//	CIcadServer     m_Server;
	//*** GPS Member Variables
//	sds_point       m_StartPoint;
//	BOOL            m_ConvStarted;
//	DWORD           m_DDEidInst;
	//*** Handle to the accelerator table
	HACCEL m_hAccel;
	//*** Flags to indicate whether or not to update the customize items in the registry.
//	BOOL m_bMenuModified; //*** This moved to CIcadMenu
//	BOOL m_bTbModified; //*** This moved to CIcadToolBarMain
	BOOL m_bKeyboardModified;
	BOOL m_bAliasModified;
	//*** Variables to be used across threads via the WndAction function.
	CString m_strWndAction;
//	CString m_strPrintfWndAction;
	BOOL m_bWndAction;
	void* m_pvWndAction;

    CPrintDialogSheet *m_pIcadPrintDlg; //*** Print Dialog
	BOOL m_bPrintPreview;

	BOOL m_bAlreadyDestroyed;

	CStringList* m_pOpenOleList;

	//*** Digitizer variables
	CIcadTablet *m_pTablet;

	//*** System Paths from options dialog
	SystemPaths *m_paths;

	//*** Member functions
	bool IsValidDrawing(db_drawing *pDrw) const;
    void IcadXdataEdit(sds_name nmEntity);
	int  IcadStartDao(void);
    void IcadRunform(char *formname);
    void IcadExplorer(short index);
    void IcadAppLoader(void);
	void IcadSelDia(sds_name ss);
    void IcadAtteDia(sds_name ename);
    void IcadConfigDia(void);
    void IcadDimDia(bool bDimensionProps = false);		/*D.G.*/// bDimensionProps added.
    void IcadXrefDia(void);
    void IcadCustomDia(short nActiveTab=0);
    void IcadViewCtl(void);
    void IcadInsertDia(char *name=NULL);
    void IcadAttDefDia(void);
    void IcadNameDia(int mode);
    void IcadRenameDia(void);
    void IcadVpointDia(void);
    void IcadRenameApp(void);
    void IcadAttExtDia(void);
    void IcadUcspDia(void);
    void IcadSetvarsDia(void);
    void IcadEntProps(bool isDiaNeed = true);				/*D.Gavrilov*/
	// isDiaNeed was added for disabling of EntityProperties dialog in the
	// case of ICAD_WNDACTION_DIMVARS window action. Then the 1st parameter (ss) was removed.
    void IcadAbout();
	int  IcadColorDia(int DefColor,int *CurColor,short mode);
	void SetMovePos(CWnd* pWnd, int nMode);
	void SetSize(UINT nType, CWnd* pWnd, int nMode);
    BOOL CloseApp(void);
    void AddToList(CString& str);
    int PrintOnCommand(char *str);
	CString NeedText(UINT nID, NMHDR* pNotifyStruct, LRESULT* lResult);
	CIcadView* GetIcadView(void);
	void DoPrint();
	void DoPrintDirect();
	void IcadMLEditDia();
    //EBATECH(watanabe)-[changed
    //void IcadToleranceDia(void);
    void IcadToleranceDia(double* ptTolerance = NULL);
    //]-EBATECH(watanabe)
    // EBATECH(watanabe)-[get mtext info only
    //void IcadMTextDia(struct SDS_mTextData  *mTextdataptr);
    void IcadMTextDia(struct SDS_mTextData*, const bool bEntMake = true);
    //]-EBATECH(watanabe)
	void IcadGroupDia();
	void IcadPMSpaceDia();
	BOOL OpenFileUnknown(LPSTR pszFileName, BOOL bOleAware = FALSE);

	//*** hatch and bpoly functions
	void IcadBoundaryPolyDia();
	void IcadBoundaryHatchDia(sds_name ss);

	//*** text style functions
	void IcadTextStyleDia(void);


	CIcadMenu *GetCurrentMenu( void );

	//*** Accelerator functions
	BOOL LoadAccelListFromReg(void);
	void LoadDefaultAccelList(void);
	void LoadAcceleratorTable(void);
	void SaveAcceleratorsToReg(void);
	void DeleteAccelList(void);
	afx_msg void OnAccelKey(UINT nId);

	//*** Wizard functions.
	void DisplayWizard();
    void NewDwgWizard();

	//**** Spell Check function
	void SpellCheck(sds_name ssSpellCheck);
	IVSpell* m_piVSpell;

	//**** Help functions
	static bool IcadHelp( LPCTSTR pszFile, UINT uCommand, DWORD dwData );

    //*** Message Map Member Functions
	afx_msg void IcadMenuPick(UINT nId);
	afx_msg void IcadWndAction(UINT nId);
	afx_msg void IcadWndAction(UINT nId, LPARAM);
	afx_msg void ShowCmdBars(void);
	afx_msg void ShowStatusBar(void);
	afx_msg void UpdateCmdBars(CCmdUI* pCmdUI);
	afx_msg void UpdateStatusBar(CCmdUI* pCmdUI);

	//*** Context menu items
	void OnMRUPick(UINT nId);
	void ContextMenu(CPoint point);
	void OnContextMenuPick(UINT nId);
	void TextScreenMenu(CPoint point);
	void OnTextScreenMenuPick(UINT nId);

	void PasteFromClipboard();
	BOOL PasteClipboardType(UINT cfType, COlePasteSpecialDialog* pPasteDlg = NULL);
	void PasteClipboardText();

    //*** Message Map (Tool Bar Functions)
	afx_msg void QueryDelete(UINT nID, NMHDR* pNotifyStruct, LRESULT* result);
	afx_msg void QueryInsert(UINT nID, NMHDR* pNotifyStruct, LRESULT* result);
	afx_msg void BeginDrag(UINT nID, NMHDR* pNotifyStruct, LRESULT* result);
	afx_msg void EndDrag(UINT nID, NMHDR* pNotifyStruct, LRESULT* result);
	afx_msg void BeginAdjust(UINT nID, NMHDR* pNotifyStruct, LRESULT* result);
	afx_msg void ToolBarChange(UINT nID, NMHDR* pNotifyStruct, LRESULT* result);
	afx_msg BOOL OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnToolTipTextW(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnSetMessageString(WPARAM wParam, LPARAM lParam);
    afx_msg void OnDropFiles(HDROP hDropInfo);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

    //*** Tool Bar & Menu Functions
	afx_msg void IcadMenuExec(UINT nId); // Handles All Command Processing
	BOOL LoadToolbars();	// Load Icad System Toolbars
	void OnToolBarSizeChanged(); //Update Toolbars
	void OnToolTipsChanged();
	void UpdateTbButtonStates();
	void IcadTBUserLevel();
	BOOL LoadBMPFile(HWND,char *);
	BOOL FileRead(FILE* fptr,LPSTR lpBuffer,UINT nBytes);
	void IcadToolbars(bool bExport = false);
	BOOL CreateToolBarsFromReg();
	void SaveToolBars();
	void RegWriteIcadButton(CONST BYTE* lpData, int idxButton, HKEY hKey);
	void EnableDocking(DWORD dwDockStyle);
	void DockControlBar(CControlBar* pBar, UINT nDockBarID = 0,
		LPCRECT lpRect = NULL);
	void DockControlBar(CControlBar* pBar, CDockBar* pDockBar,
		LPCRECT lpRect = NULL);

	//*** Read and save the alias registry info.
	void LoadAliases();
	void SaveAliasesToReg();

    //*** DDE Functions
	void StartDDEServer(void);

	//*** Virtual function override
	virtual HMENU GetWindowMenuPopup(HMENU hMenuBar);
	CPalette* SetPalette(CPalette* pPalette);

	void DocChangeCallback( CIcadView *pNewView );

private:
    // Tool Bar & Menu Functions

	CBitmap *m_BitMap;
	static HINSTANCE m_hInstHTMLHelp; // handle to HTMLHelp OCX (HHCTRL.OCX)

protected:
    BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL DestroyWindow();

	//*** hatch and bpoly functions
	int PickPoints(BHatchDialog &hatchDia, CBoundaryBuilder &builder, bool &firstPickPoints, C3Point &ptNormal);
	long SetSelect(BHatchDialog &hatchDia);


	//{{AFX_MSG(CMainWindow)
    afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMove(int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint pos);
	afx_msg void OnClose(void);
	afx_msg void OnSetFocus( CWnd* pNewWnd );
	afx_msg void OnKillFocus( CWnd* pNewWnd );
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnEnable( BOOL bEnable );
	afx_msg void OnActivateApp(BOOL bActive, HTASK hTask);
	afx_msg LRESULT OnDoModal(WPARAM,LPARAM);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	//}}AFX_MSG

	afx_msg LRESULT OnPrintf(WPARAM,LPARAM);
	afx_msg LRESULT OnColorDialog(WPARAM,LPARAM);
	afx_msg LRESULT OnDisplayChange(WPARAM,LPARAM);
	afx_msg BOOL OnQueryNewPalette();
	afx_msg void OnPaletteChanged(CWnd* pFocusWnd);

	afx_msg LRESULT OnGetPassWord(WPARAM, LPARAM );
	afx_msg LRESULT OnGetDwfInfo(WPARAM, LPARAM );

    DECLARE_MESSAGE_MAP()
};


