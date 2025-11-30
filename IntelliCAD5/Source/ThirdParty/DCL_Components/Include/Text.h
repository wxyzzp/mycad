/** FILENAME:     Text.h- Contains declarations of class CText.
* Copyright  (C) Copyright 1998-99  Visio Corporation.  All Rights Reserved.
*
*
*
* Abstract 
*	his class Maps the basic text text_part and errtile tile
*	to windows  static control 
*
*
*Audit Log 
*
*
*Audit Trail
*
*
*/

#if !defined _TEXT_H_
#define _TEXT_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "ScreenObject.h"

class Text : public CStatic ,public ScreenObject
{

public:
	Text(); // Construction
	Text(						// overloaded constructor
		int Type,				// i:type weather text text_part or errtile
		int ParentAlignment);  // i:parent alignment
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

// Attributes
public:

// Operations
public:
private :
	int nType;

// Operations derived from base class ScreenObject
public:	
	virtual int CreateOnScreenObject ( // Creates the UI objects
		CWnd* pParent				// i:pointer to the parent window
		); 
	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Text)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~Text();

	// Generated message map functions
protected:
	//{{AFX_MSG(Text)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


#endif
