// IcadCusToolbar.h : header file
//
#ifndef _ICADCUSTOOLBAR_H_
#define _ICADCUSTOOLBAR_H_

//*** Start and ending button ID values.
#define ICAD_CUSTOOL_BTNID_START 100
#define ICAD_CUSTOOL_BTNID_END   1100
#define long_double	__int64
/////////////////////////////////////////////////////////////////////////////
// CIcadCusToolbar dialog
class CIcadToolBarMain;

class CIcadCusToolbar : public CPropertyPage
{
	DECLARE_DYNCREATE(CIcadCusToolbar)

//*** Member variables
public:
	CToolBarCtrl* m_pCurSelTb;
	CIcadToolBarMain* m_pTbMain;

//*** Helper functions
public:
	//CToolBarCtrl* CreateDisplayToolBar(CRect rectWnd, long lToolID, int nID);
	CToolBarCtrl* CreateDisplayToolBar(CRect rectWnd, long_double lToolID, int nID);
	void OnToolBarPick(UINT nID);
	void OnTbButtonPick(UINT nID);
	void ResetEditboxes();
	void ResetButtons();
	void OnExit();

// Construction
public:
	CIcadCusToolbar();
	~CIcadCusToolbar();

// Dialog Data
	//{{AFX_DATA(CIcadCusToolbar)
	enum { IDD = CUS_TOOLBAR };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CIcadCusToolbar)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CIcadCusToolbar)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelChangeList();
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnButtonAdvanced();
	afx_msg void OnButtonFromBitmap();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
// CIcadToolBarCtrl window

class CIcadToolBarCtrl : public CToolBarCtrl
{
// Construction
public:
	CIcadToolBarCtrl();
	CIcadToolBarMain* m_pTbMain;

// Attributes
public:
    HCURSOR m_prevCursor;
	TBBUTTON m_CurDragButton;
	BOOL m_bInsAsFlyOut;
	CWnd* m_pMain;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIcadToolBarCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CIcadToolBarCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CIcadToolBarCtrl)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	afx_msg BOOL OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnToolTipTextW(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif
