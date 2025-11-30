/** FILENAME:     EditBox.h- Contains declarations of class CEditBox.
* Copyright  (C) Copyright 1998-99  Visio Corporation.  All Rights Reserved.
*
*
*
* Abstract 
*	this class Maps the basic tiles edit
*	to windows controls
*
*
*Audit Log 
*
*
*Audit Trail
*
*
*/

#if !defined _EDITBOX_H_
#define _EDITBOX_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "ScreenObject.h"


class EditBox : public CEdit ,public ScreenObject
{

public:
	EditBox(); // Construction
	EditBox(int Parent_layout); //  overloaded constructor
	virtual int ModeTile ( // Executes functions like setting focus to a control , enabling or disabling a control etc.
		int nTileState	   // i:state of control
		);
	virtual int GetTile (		// Returns the "value" attribute of the tile
		CString & szControlInfo,// o:Date contaned in value attribute
		int lInfoMaxLength	// o: Size of data
		);
	virtual int SetTile (		// Sets the "value" attribute of the tile
		CString szControlKey,	// i:Key of the tile
		CString szControlInfo	// i:Data to store in the value attribute
		);
	virtual int InitFocus();	// this function is usedto set the porperty initial_focus of dialog
// Attributes
private: 
	CStatic n_Title; // object of type Static Control 

protected:
// Operations specific to this class.
	void ProcessCALLBACKFunction (int reason);

// Operations
public:

// Operations derived from base class ScreenObject	
public:	
	virtual int CreateOnScreenObject ( // Creates the UI objects
		CWnd* pParent				// i:pointer to the parent window
		); 


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(EditBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~EditBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(EditBox)
	afx_msg void OnKillfocus();
	afx_msg void OnSetfocus();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


#endif
