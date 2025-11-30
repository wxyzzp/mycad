/** FILENAME:     CtrlListAttributes.h- Contains declarations of class CCtrlListAttributes.
* Copyright  (C) Copyright 1998-99 Visio Corporation.  All Rights Reserved.
*
*
*
* Abstract : Class for the dialog to show the attribute list when a user double clicks
*             on a particular node in the TreeView
*
*
*Audit Log 
*
*
*Audit Trail
*
*
*/



#ifndef _CTRLLISTATTRIBUTES_H
#define _CTRLLISTATTRIBUTES_H

class CCtrlListAttributes : public CDialog
{
public:
	int InitList(									// To initialise the ListCtrl
				CListCtrl* pListCtrl				// i : a pointer to the ListCtrl
				 );
	int InsertColumns(								// To insert columns in the ListCtrl
				CListCtrl* pListCtrl				// i : a pointer to the ListCtrl
				 );
	CCtrlListAttributes(CWnd* pParent = NULL);		// standard constructor

// Dialog Data
	//{{AFX_DATA(CCtrlListAttributes)
	enum { IDD = IDD_DIALOG3 };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCtrlListAttributes)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCtrlListAttributes)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif 
