/** FILENAME:     Button.h- Contains declarations of class CButton.
* Copyright  (C) Copyright 1998-99  Visio Corporation.  All Rights Reserved.
*
*
*
* Abstract 
*	this class Maps the basic tiles (Button , RadioButton, Toggle)	
*	to windows control
*
*Audit Log 
*
*
*Audit Trail
*
*
*/

#if !defined _BUTTON_H_
#define _BUTTON_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


# include "ScreenObject.h"
#include "sds.h"


class Button : public CButton, public ScreenObject
{

public:
	Button(); // constructor

	Button(						// overloaded constructor which takes the type of control 
	int type ,					// i:type of basic tile (button , radoibutton , toggle)
	int ParentAlignment			// i:parent alignment
	);
	virtual int ModeTile ( // Executes functions like setting focus to a control , enabling or disabling a control etc.
		int nTileState	   // i:state of control
		);
	virtual int InitFocus(); // this function is usedto set the porperty initial_focus of dialog
	virtual int SetTile (		// Sets the "value" attribute of the tile
		CString szControlKey,	// i:Key of the tile
		CString szControlInfo	// i:Data to store in the value attribute
		);
// Attributes
private:
	int m_bType;	// parameter which discribes the type of tile (button,radoibutton,toggle)
	

// Operations
public:

// Operations derived from base class ScreenObject
public:	
	virtual int CreateOnScreenObject ( // Creates the UI objects
		CWnd* pParent				// i:pointer to the parent window
		); 
	 
	virtual int GetTile (		// Returns the "value" attribute of the tile
		CString & szControlInfo,// o:Date contaned in value attribute
		int lInfoMaxLength	// o: Size of data
		);
	//Modified SAU 11/07/2000 
	//Reason:REGRESSION:  is_cancel attribute is ignored and for Bug Fixed:45448
	CWnd* GetWindow();      //returns the CWnd pointer for the screen object
	
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Button)
	//}}AFX_VIRTUAL

// Implementation
public:
	int UnCheckAllSiblingRadioBtn(); // Loop and Uncheck all the radio buttons in this parent container.
	
	virtual ~Button(); // Destructor

	// Generated message map functions
protected:
	//{{AFX_MSG(Button)
	afx_msg void OnClicked(); 
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


#endif 
