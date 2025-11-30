/* D:\ICADDEV\PRJ\ICAD\PRINTSCALEVIEWTAB.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *	
 * Abstract
 *	
 * scale view tab on print dialog
 *	
 */ 

#if !defined(AFX_PRINTSCOPETAB_H__CE54B893_6D27_11D2_BA8F_0060081C2430__INCLUDED_)
#define AFX_PRINTSCOPETAB_H__CE54B893_6D27_11D2_BA8F_0060081C2430__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PrintScaleViewTab.h : header file
//

namespace ICAD_AUTOMATION
{
struct IIcadPlotProperties;
struct IIcadPlotManager;
}
using namespace ICAD_AUTOMATION;

class CPrintDialogSheet;

/////////////////////////////////////////////////////////////////////////////
// PrintScaleViewTab dialog

class PrintScaleViewTab : public CPropertyPage
{
// Construction
public:
	PrintScaleViewTab(CWnd* pParent = NULL);   // standard constructor
	~PrintScaleViewTab();

	bool initSettings();
	bool wasPicked() const { return m_bWasPicked; }
	//un-hightlights and frees the m_ssPrintEnt
	void RemovePrintEnts ();

	void ResetViewList();

	BOOL GetPrintWindow(CIcadView* pView, sds_point ptLwrLeft, sds_point ptUprRight);
	void SetPaperSize();

	void LoadPlotSettings();
	void SavePlotSettings();
	void LoadRegSettings(bool bFromLayout = true);
	void SaveRegSettings(bool bToLayout = true);

	int getScaleType( double printedUnits, double drawingUnits );

	void SetPlotProperties(IIcadPlotProperties *plotProp) {m_plotProperties = plotProp;}
	void SetPlotManager(IIcadPlotManager *plotMgr) {m_plotMgr = plotMgr;}
	void CopyList ();
	



// Dialog Data
	//{{AFX_DATA(PrintScaleViewTab)
	enum { IDD = PRINT_SCALE_VIEW };
	int		m_View;
	int		m_SelMode;
	int		m_Scale;
	int		m_ClipMode;
	BOOL	m_FitMode;
	CString	m_winFromX;
	CString	m_winFromY;
	CString	m_winToX;
	CString	m_winToY;
	CString	m_DrawingUnits;
	CString	m_PrintedUnits;
	CString	m_ViewName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PrintScaleViewTab)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

public:
	void SetDefaults();
	void SetUnits();
	void setWindow(double x1, double y1, double x2, double y2);

	sds_name m_ssPrintEnt;
	double m_drawUnitScale, m_printUnitScale;
	bool m_bApplyWasPressed ;
	bool m_bPreviewWasPressed;
	struct resbuf* pLayout ;
	CPtrList *m_ViewList;

// Implementation
protected:
	double m_winX1, m_winY1, m_winX2, m_winY2;
	bool m_bWasPicked;

	IIcadPlotProperties *m_plotProperties;
	IIcadPlotManager *m_plotMgr;

	sds_resbuf* GetCurViewRb();
	BOOL UpdateFitSize(CDC* pDC = NULL);
	// Generated message map functions
	//{{AFX_MSG(PrintScaleViewTab)
	afx_msg void OnViewRadio();
	afx_msg void OnSelchangeViewlist();
	afx_msg void OnUnitsChanged();
	afx_msg void OnFitToPage();
	afx_msg void OnSelectArea();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRINTSCOPETAB_H__CE54B893_6D27_11D2_BA8F_0060081C2430__INCLUDED_)


