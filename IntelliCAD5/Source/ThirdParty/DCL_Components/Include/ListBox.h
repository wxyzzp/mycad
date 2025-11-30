/** FILENAME:     ListBox.h- Contains declarations of class CListBox.
* Copyright  (C) Copyright 1998-99  Visio Corporation.  All Rights Reserved.
*
*
*
* Abstract 
*	this class Maps the basic tiles list	
*	to windows control listbox
*
*
*Audit Log 
*
*
*Audit Trail
*
*
*/

#if !defined _LISTBOX_H_
#define _LISTBOX_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "ScreenObject.h"


class ListBox : public CListBox,public ScreenObject
{

public:
	ListBox();							// Construction
	ListBox(int ParentAlignment);		// overloaded constructor
	virtual int ModeTile (				// Executes functions like setting focus to a control , enabling or disabling a control etc.
		int nTileState					// i:state of control
		);
	virtual int GetTile (				// Returns the "value" attribute of the tile
		CString & szControlInfo,		// o:Date contaned in value attribute
		int lInfoMaxLength				// o: Size of data
		);
	virtual int AddList (				// Adds a new string in the list
		CString szAddToList				// i:String to add
		);
	virtual int GetListString (			// Gets the String from the ListBox 
		CString& szListString,			// o:Parameter to hold the return string
		int* nStringLength,				// o:Size of the String
		int nWhichItem					// i:item index in the listbox whose string is returned
		);
	virtual int InitFocus();			// Sets the initial focus.
// Attributes
private: 
	CStatic n_Title;					// object of type Static Control

// Operations
public:
// Operations derived from base class ScreenObject
public:	
	virtual int CreateOnScreenObject (	// Creates the UI objects
		CWnd* pParent					// i:pointer to the parent window
		); 

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ListBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	int SetTile( CString szControlKey,CString szControlInfo );
	
	virtual ~ListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(ListBox)
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnSelchange();
	afx_msg void OnDblclk();
	afx_msg void OnSetfocus();
	afx_msg void OnKillfocus();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


#endif 
