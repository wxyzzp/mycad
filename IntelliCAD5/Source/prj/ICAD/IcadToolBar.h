/* File  : <DevDir>\source\prj\icad\IcadToolBar.h
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 * Master Structure for IntelliCAD Toolbars
 */

#ifndef	_ICADTOOLBAR_H
#define	_ICADTOOLBAR_H


#include "IcadCusToolbar.h"/*DNT*/
#include "IcadAccelerator.h"/*DNT*/
#include <shlwapi.h>

// Common control style that is not included in commctrl.h for some reason.
#define CCS_NOHILITE	0x00000010L

typedef HRESULT (CALLBACK* DLLGETVERSIONPROC)(DLLVERSIONINFO*);

//#define XML_TOOLBAR_AS_DEFAULT - not completed!
#define XML_TOOLBAR
// FORWARDS
#ifdef XML_TOOLBAR
class XMLNode;
class CBitmapLoadHook;
#endif

// This class contains all necessary information needed by our toolbar buttons.
// Its address is stored in the dwData member of the TBBUTTON structure in every button.
class CIcadTBbutton
{

public:
	int 			&m_idToolBar;		// ID of the toolbar owning this button.
	CString			m_toolTip,			// Tooltip string.
					m_helpString,		// Help string.
					m_command,			// Command string to be sent to SDS Event
					m_pushVar,			// Controls the pushed-in state of buttons based on the value of the setvar.
					m_grayVar;			// Controls whether the button is available based on the setvar.
	short			&m_BMPIdSmColor,	// ID for Small/Color bitmap.
					&m_BMPIdLgColor,	// ID for Large/Color bitmap.
					&m_BMPIdSmBW,		// ID for Small/Monochrome bitmap.
					&m_BMPIdLgBW;		// ID for Large/Monochrome bitmap.
	long			&m_lflgVisibility;	// Bit-set value for the visibility of this item.
	CIcadTBbutton	*&m_pibFlyOut,		// Pointer to the linked list of IcadButton structures
										// defining the flyout buttons for this button.
					*&m_pibCurFlyOut;	// Pointer to the current CIcadTBbutton object to
										// use for a flyout parent button.
	int				&m_idxFlyOutImage;	// Index of the current flyout image to use.  For a
										// flyout this member is set to the index of the
										// image in the owner toolbar.
	BOOL			&m_bAddSpacerBefore,// If set to TRUE a spacer precedes the button.
					&m_bChgParentBtnImg;// If set to TRUE the parent button of a flyout
										// changes its image to the image of the flyout
										// button that was picked.
										// I needed to add these to the end of the stuct to keep existing users
										// from crashing on startup.
	CString			m_smColorBmpPath,	// Optional path and file name for Small/Color bitmap.
					m_lgColorBmpPath,	// Optional path and file name for Large/Color bitmap.
					m_smBwBmpPath,		// Optional path and file name for Small/Monochrome bitmap.
					m_lgBwBmpPath;		// Optional path and file name for Large/Monochrome bitmap.

/*D.G.*/// Since we have switched to CString in CIcadTBbutton then the following struct was added
// for saving/writing data to the registry. So,
// 1. changes in these structs must be co-ordinated
//    (by the way, changing of CIcadTBbutton is very dangerous anyway because it was saved to the registry);
// 2. char* fields in ICBUTTON are just for compatibility (for the sizeof etc.) and are not used now.

private:
	struct ICBUTTON
	{
		int 			idToolBar;
		char			*toolTip,
						*helpString,
						*command,
						*pushVar,
						*grayVar;
		short			BMPIdSmColor,
						BMPIdLgColor,
						BMPIdSmBW,
						BMPIdLgBW;
		long			lflgVisibility;
		CIcadTBbutton	*pibFlyOut,
						*pibCurFlyOut;
		int				idxFlyOutImage;
		BOOL			bAddSpacerBefore,
						bChgParentBtnImg;
		char			*smColorBmpPath,
						*lgColorBmpPath,
						*smBwBmpPath,
						*lgBwBmpPath;

		ICBUTTON() : idToolBar(0), BMPIdSmColor(0), BMPIdLgColor(0), BMPIdSmBW(0), BMPIdLgBW(0),
					lflgVisibility(0L), pibFlyOut(NULL), pibCurFlyOut(NULL), idxFlyOutImage(0),
					bAddSpacerBefore(FALSE), bChgParentBtnImg(FALSE), toolTip(NULL), helpString(NULL),
					command(NULL), pushVar(NULL), grayVar(NULL),
					smColorBmpPath(NULL), lgColorBmpPath(NULL), smBwBmpPath(NULL), lgBwBmpPath(NULL)
		{}

	};	// struct ICBUTTON

	ICBUTTON	m_data4reg;

public:
	CIcadTBbutton() : m_idToolBar(m_data4reg.idToolBar),
					m_BMPIdSmColor(m_data4reg.BMPIdSmColor),
					m_BMPIdLgColor(m_data4reg.BMPIdLgColor),
					m_BMPIdSmBW(m_data4reg.BMPIdSmBW),
					m_BMPIdLgBW(m_data4reg.BMPIdLgBW),
					m_lflgVisibility(m_data4reg.lflgVisibility),
					m_pibFlyOut(m_data4reg.pibFlyOut),
					m_pibCurFlyOut(m_data4reg.pibCurFlyOut),
					m_idxFlyOutImage(m_data4reg.idxFlyOutImage),
					m_bAddSpacerBefore(m_data4reg.bAddSpacerBefore),
					m_bChgParentBtnImg(m_data4reg.bChgParentBtnImg)
	{}

	BOOL	readFromReg(HKEY hKey, int idxButton);
	void	saveToReg(HKEY hKey, int idxButton);
#ifdef XML_TOOLBAR
	BOOL	readFromXML(XMLNode& node, CBitmapLoadHook& bmpLoader);
	void	saveToXML(XMLNode& node, bool bSaveFlyOut);
#endif
};


/////////////////////////////////////////////////////////////////////////////

class CIcadPopUp : public CMiniFrameWnd
{
	DECLARE_DYNCREATE(CIcadPopUp)

// Attributes
public:
	CIcadPopUp()
	{}
	virtual ~CIcadPopUp()
	{}

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIcadPopUp)
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CIcadPopUp)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


class CIcadToolBarMain;

class CIcadFlyOut : public CToolBarCtrl
{
	DECLARE_DYNCREATE(CIcadFlyOut)

// Construction
public:
	CIcadFlyOut();

// Attributes
public:
	CMiniFrameWnd*		m_pPopup;
	int					m_idxParentButton;
	CWnd*				m_pTbOwner;
	CIcadToolBarMain*	m_pTbMain;
	BOOL				m_bImagesReversed;
	CToolTipCtrl*		m_pToolTipCtrl;
	HBITMAP				m_hbmImageWell;
	CString				m_strMnuFileName;	// Filename of the mnu file this flyout was create from.
	CIcadTBbutton*		m_pLastButtonPick;

private:
	int				m_idPrevButton;
	CWnd*			m_pTbLostCapture;
	TBBUTTON		m_CurDragButton;
    HCURSOR			m_prevCursor,
					m_hcurToolBar,
					m_hcurToolBarDel,
					m_hcurFlyOutH,
					m_hcurFlyOutHR,
					m_hcurFlyOutV,
					m_hcurFlyOutVR;
	int				m_idxCurDragButton;
	BOOL			m_bWin95;
	CIcadFlyOut*	m_pNestedFlyOut;

// Operations
public:
	void LoadFlyOutButtons(CIcadTBbutton* pibParent, BOOL bReverseImages);
	void UpdateSize();
	void OnButtonOrderChanged();
	void ReleaseCapture();
	void DestroyFlyOut();
	void AddToolTips();
	int  SetBitmap(HBITMAP hbmImageWell);
	BOOL IsNested();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIcadFlyOut)
	public:
	virtual BOOL Create(const RECT& rect, CIcadTBbutton* pibParent, BOOL bReverseImages);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CIcadFlyOut();

	// Generated message map functions
protected:
	//{{AFX_MSG(CIcadFlyOut)
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd* pWnd);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	BOOL OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);
	BOOL OnToolTipTextW(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()
};


class CIcadToolBar : public CToolBar
{
	DECLARE_DYNCREATE(CIcadToolBar)

public:
	CIcadToolBar();
	~CIcadToolBar();

// Member variables
public:
	CIcadFlyOut*	m_pFlyOut;
	BOOL			m_bEmpty,
					m_bJustGotDblClk,
					m_bPressedOnLBtnDn;
	CIcadToolBarMain*	m_pTbMain;
	//long		m_lDefID;		// Save the ToolID from the default structure for the Customize-Reset option.
	long_double		m_lDefID;		// Save the ToolID from the default structure for the Customize-Reset option.
	UINT			m_nID;			// Save the ID that the toolbar was created with to avoid problems with LoadBarState().
	// For customize
    HCURSOR			m_prevCursor;
	int				m_idxCurDragButton;
	CPoint			m_ptStartDrag;
	BOOL			m_bflgButtonDown,
					m_bShiftCust,
					m_bInNcPaint,
					m_bIsStyleFlat;

	DLLVERSIONINFO	m_dllVersion;	// Version number of comctl32.dll

	BOOL SetDllVersion();

	CString			m_strMnuFileName,	// Filename of the mnu file this toolbar was create from.
					m_strGroupName;		// Groupname specified in the mnu that this toolbar was create from.

// Member functions
public:
	int		LoadButtonsFromReg(LPCTSTR pstrTbKey, HKEY hKeyTb);
#ifdef XML_TOOLBAR
	int		LoadButtonsFromXML(XMLNode& node);
	void	SaveToXML(XMLNode& node);
	int		ReadFromXML(XMLNode& node);
#endif
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	BOOL	CreateFlyOut(CPoint point);
	void	EnableDocking(DWORD dwDockStyle);
	BOOL	ChangeBitmap(int idCommand, int iBitmap);
	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);
	virtual CSize CalcDynamicLayout(int nLength, DWORD nMode);
	CSize	CalcSize(TBBUTTON* pData, int nCount);
	void	_GetButton(int nIndex, TBBUTTON* pButton);
	void	_SetButton(int nIndex, TBBUTTON* pButton);
	CSize	CalcLayout(DWORD nMode, int nLength = -1);
	void	CalcInsideRect(CRect& rect, BOOL bHorz);

	void	UpdateLayout(int nRows = 0);
	BOOL	SetRows(int nRows);
	int		GetButtonRows();

	HBITMAP GetImageWell()
	{
		return m_hbmImageWell;
	}

	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:
	// Generated message map functions
	//{{AFX_MSG(CIcadToolBar)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnNcPaint();
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif	// _ICADTOOLBAR_H
