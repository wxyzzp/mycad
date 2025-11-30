/** FILENAME:     ErrorMessage.h- Contains declarations of class CErrorMessage.
* Copyright  (C) Copyright 1998-99  Visio Corporation.  All Rights Reserved.
*
*
*
* Abstract 
*	this class is used to have a generic error message dialog box
*
*
*Audit Log 
*
*
*Audit Trail
*
*
*/


#if !defined _ERRORMESSAGE_H_
#define _ERRORMESSAGE_H_

#if _MSC_VER > 1000
#pragma once
#endif 


class ErrorMessage : public CDialog
{

public:
	int Message(					// function to display error error dialog
		CString& wszErrorMessage);	// the error message
	int Message(					// function to display error error dialog
		int messageID);				// the error message ID of string table

	ErrorMessage(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(ErrorMessage)
	enum { IDD = IDD_ERROR_DIALOG };
	CString	m_message;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ErrorMessage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ErrorMessage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif 
