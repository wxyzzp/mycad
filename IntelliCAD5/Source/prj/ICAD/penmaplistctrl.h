/* D:\ICADDEV\PRJ\ICAD\PENMAPLISTCTRL.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#if !defined(AFX_PENMAPLISTCTRL_H__3376EBE3_8189_11D2_ACD3_C03F0EC10000__INCLUDED_)
#define AFX_PENMAPLISTCTRL_H__3376EBE3_8189_11D2_ACD3_C03F0EC10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PenMapListCtrl.h : header file
//
#include "listctrlex.h"

class PrintPenMapTab;

/////////////////////////////////////////////////////////////////////////////
// PenMapListCtrl window

class PenMapListCtrl : public CListCtrlEx
{
// Construction
public:
	PenMapListCtrl(PrintPenMapTab *mapTab);

// Attributes
public:
	int editRow;		//Row/column currently being edited.  Used for validate data
	int	editColumn;		

// Operations
public:
	BOOL ValidateData();
	virtual BOOL PreCreateWindow(CREATESTRUCT &cs);
	virtual BOOL IsSubclassedControl( );
	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	void DrawSubItem(int nItem, int nColumn, LV_COLUMN& lvc, CRect& rcCol, CDC *pDC);
	int InsertItem(int nItem, LPCTSTR lpszItem, int nImage);
	BOOL DeleteAllItems();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PenMapListCtrl)
	//}}AFX_VIRTUAL


// Implementation
public:
	void SetEditPrecision(int precision);
	CEdit* EditInvalidSubLabel();
	CEdit* EditSubLabel(int row, int column);
	double GetActualWidth(int nItem);
	void SetActualWidth(int nItem, double nWidth);

	virtual ~PenMapListCtrl();

//*********************************
// General User defined Values to be associated with each item 
// in the PenMapListCtrl
protected:

	// inner Structure definition
	struct SListItemData
	{
		double ActualWidth;
		
		// Any other item specific values can be added here...
		//...
		//...
		//...

	};
//*********************************

	// Generated message map functions
protected:
	PrintPenMapTab *m_mapTab;

	void MapColor(int nItem, int nColumn, LV_COLUMN& lvc, CRect& rcCol,CDC *pDC);

	//{{AFX_MSG(PenMapListCtrl)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PENMAPLISTCTRL_H__3376EBE3_8189_11D2_ACD3_C03F0EC10000__INCLUDED_)


