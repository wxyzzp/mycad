/** FILENAME:     InactiveCtrl.h- Contains declarations of class CInactiveCtrl.
* Copyright  (C) Copyright 1998-99 Visio Corporation.  All Rights Reserved.
*
*
*
* Abstract 
*	this class Maps the basic tiles Row,Column,Boxed_row,Boxed_column,radio_row	
*	radoi_column,Boxed_radoi_row and Boxed_radoi_column to windows control 
*
*
*Audit Log 
*
*
*Audit Trail
*
*
*/
#if !defined _DCLINACTIVECTRL_H_
#define _DCLINACTIVECTRL_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ScreenObject.h"


class InactiveCtrl : public CButton ,public ScreenObject
{

public:
	InactiveCtrl(); // Construction
	InactiveCtrl(		// Overloaded Constructor
		int RowColumn ,	// i:value which determines weather row or column (true if row else false)
		int IsBoxed=0 ,	// i:value which determines weather boxed
		int IsRadio =0,	// i:value which determines weather grouped
		int IsConcatenate=0,// i:value which determines weather two text can be concatenated
		int IsParagraph=0 // i:value which determines weather the tile is Paragraph
		);

	virtual int ModeTile ( // Executes functions like setting focus to a control , enabling or disabling a control etc.
		int nTileState	   // i:state of control
		);
	virtual int GetTile (		// Returns the "value" attribute of the tile
		CString & szControlInfo,// o:Date contaned in value attribute
		int lInfoMaxLength	// o: Size of data
		);
// Attributes 
private:
	int m_IsParagraph;   // flag to indicate weather the tile is paragraph tile
	int m_IsConcatenate; // flag to indicate weather the tile is  Concatenate tile
	int m_IsBoxed;		 // flag to indicate weather the tile is  Boxed tile
	int m_RadioFlag;     // flag to indicate weather the tile is  Radoi tile
	int m_RowColumn;	 // flag to indicate weather the tile is  row or column tile
// Operations
public:
	int set_IsParagraph(  // method to set m_IsParagraph member
		int paragraph   // i:value of member
		);
	int get_IsParagraph();	// method to get m_IsParagraph member
	int set_IsConcatenate(  // method to set m_IsConcatenate member
		int Concatenate   // i:value of member
		);
	int get_IsConcatenate();	// method to get m_IsConcatenate member
	int set_IsBoxed(  // method to set m_IsBoxed member
		int Boxed   // i:value of member
		);
	int get_IsBoxed();	// method to get m_IsBoxed member
	int set_RadioFlag(  // method to set m_RadioFlag member
		int RadioFlag   // i:value of member
		);
	int get_RadioFlag();	// method to get m_RadioFlag member
	int set_RowColumn(  // method to set m_RowColumn member
		int RowColumn   // i:value of member
		);
	int get_RowColumn();	// method to get m_RowColumn member
	CRect PositionControl();


// Operations derived from base class ScreenObject
public:	
	virtual int CreateOnScreenObject ( // Creates the UI objects
		CWnd* pParent				// i:pointer to the parent window
		); 
	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(InactiveCtrl)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	
	virtual ~InactiveCtrl(); // Destructor

	// Generated message map functions
protected:
	//{{AFX_MSG(InactiveCtrl)
	afx_msg UINT OnGetDlgCode();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


#endif 
