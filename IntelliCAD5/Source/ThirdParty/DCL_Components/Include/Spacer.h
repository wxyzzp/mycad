/** FILENAME:     Spacer.h- Contains declarations of class CSpacer.
* Copyright  (C) Copyright 1998-99 Visio Corporation.  All Rights Reserved.
*
*
*
* Abstract 
*	this class Maps the basic (spacer , spacer0	)
*	to windows control
*
*
*Audit Log 
*
*
*Audit Trail
*
*
*/

#if !defined _SPACER_H_
#define _SPACER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ScreenObject.h"


class Spacer : public CStatic ,public ScreenObject
{

public:
	Spacer(); // Construction
	Spacer(int nType, int ParentLayout); // Construction
	virtual int GetTile (		// Returns the "value" attribute of the tile
		CString & szControlInfo,// o:Date contaned in value attribute
		int lInfoMaxLength	// o: Size of data
		);
// Attributes
public:
private:
	int m_nType;
// Operations
public:
// Operations derived from base class ScreenObject
public:	
	virtual int CreateOnScreenObject ( // Creates the UI objects
		CWnd* pParent				// i:pointer to the parent window
		); 

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Spacer)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~Spacer();

	// Generated message map functions
protected:
	//{{AFX_MSG(Spacer)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif 
