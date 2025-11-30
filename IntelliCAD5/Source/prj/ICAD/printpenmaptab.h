/* D:\ICADDEV\PRJ\ICAD\PRINTPENMAPTAB.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * pen map tab on print dialog
 * 
 */ 

#if !defined(AFX_PRINTPENMAPTAB_H__CE54B894_6D27_11D2_BA8F_0060081C2430__INCLUDED_)
#define AFX_PRINTPENMAPTAB_H__CE54B894_6D27_11D2_BA8F_0060081C2430__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PrintPenMapTab.h : header file
//

class PenMapListCtrl;
namespace ICAD_AUTOMATION
{
struct IIcadPlotProperties;
}
using namespace ICAD_AUTOMATION;

/////////////////////////////////////////////////////////////////////////////
// PrintPenMapTab dialog

class PrintPenMapTab : public CPropertyPage
{
// Construction
public:
	PrintPenMapTab(CWnd* pParent = NULL);   // standard constructor
	~PrintPenMapTab();

	void LoadPlotSettings();
	void SavePlotSettings();

	bool initSettings();
	void SetPlotProperties(IIcadPlotProperties *plotProp) {m_plotProperties = plotProp;}
	BOOL ValidWidth(CString currentWidth);
	void SetDefaults();
	short StringToColor(CString colorStr);
	CString ColorToString(short colorNum);
	void LineWidthEdited();
	void SetUnits(int units);
	void SetPropertiesInfo();


// Dialog Data
	//{{AFX_DATA(PrintPenMapTab)
	enum { IDD = PRINT_PENMAP };
	CString	m_lineWidth;
	CString	m_drawingColor;
	CString	m_outputColor;
	CString	m_lineWidthUnits;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PrintPenMapTab)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
	PenMapListCtrl	*m_penList;
	bool editingPenMap;
	int m_currentUnits;

	void PaintItem(int id, int color, int offset);
	int SetColorFromString(CString colorStr);

	IIcadPlotProperties *m_plotProperties;

	// Generated message map functions
	//{{AFX_MSG(PrintPenMapTab)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickPenmap(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndLabelEditPenmap(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColorButton();
	afx_msg void OnKillfocusOutputColor();
	afx_msg void OnKeydownPenmap(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPaint();
	afx_msg void OnKillfocusLineWidth();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnSetfocusOutputColor();
	afx_msg void OnSetfocusLineWidth();
	afx_msg void OnItemchangedPenmap(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangingPenmap(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
	enum EditBoxType {OUTPUT_COLOR, LINE_WIDTH, NONE};

	EditBoxType m_editBoxSelected;
	BOOL m_firstMapSelect;
	//BugZilla No. 78341; 21-10-2002 
	BOOL m_bDataUpdated;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRINTPENMAPTAB_H__CE54B894_6D27_11D2_BA8F_0060081C2430__INCLUDED_)


