/** FILENAME:     PopUpList.h- Contains declarations of class CPopUpList.
* Copyright  (C) Copyright 1998-99  Visio Corporation.  All Rights Reserved.
*
*
*
* Abstract 
*	this class Maps the basic tiles PopUpList	
*	to windows control comboBox
*
*
*Audit Log 
*
*
*Audit Trail
*
*
*/
#if !defined _POPUPLIST_H_
#define _POPUPLIST_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ScreenObject.h"



class PopUpList : public CComboBox,public ScreenObject
{

public:
	DWORD dwStyle ; // holdes the style associated to the popuplist
	CWnd* pParentWnd  ; // holdes pointer to parent
	CRect rectpopup;	// holdes rect of listbox
	PopUpList();					// Construction
	PopUpList(int ParentAlignment); // overloaded constructor 
	virtual int ModeTile (			// Executes functions like setting focus to a control , enabling or disabling a control etc.
		int nTileState				// i:state of control
		);
   virtual int SetTile (
      CString szControlKey,
      CString szControlInfo
      );
	virtual int GetTile (			// Returns the "value" attribute of the tile
		CString & szControlInfo,	// o:Date contaned in value attribute
		int lInfoMaxLength			// o: Size of data
		);
	virtual int AddList (			// Adds a new string in the list
		CString szAddToList			// i:String to add
		);
	virtual int GetListString (		// Gets the String from the ListBox 
		CString& szListString,		// o:Parameter to hold the return string
		int* nStringLength,			// o:Size of the String
		int nWhichItem				// i:item index in the listbox whose string is returned
		);
	virtual int InitFocus();		// Set the initial focus.
	CWnd * pWnd;					

// Attributes
private: 
	CStatic n_Title; // object of type Static Control
	

// Operations
public:
// Operations derived from base class ScreenObject
public:	
	virtual int CreateOnScreenObject ( // Creates the UI objects
		CWnd* pParent				// i:pointer to the parent window
		); 


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PopUpList)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~PopUpList();

	

	// Generated message map functions
protected:
	CString SetTabs(		  // function to set tabs
		CString tabsValue);  // i : tabs string 
	//{{AFX_MSG(PopUpList)
	afx_msg void OnSelchange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



#endif 
