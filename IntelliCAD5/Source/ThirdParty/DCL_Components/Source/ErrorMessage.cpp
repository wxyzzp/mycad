/** FILENAME:     ErrorMessage.cpp- Contains implementation of class CErrorMessage.
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



#include "stdafx.h"
#include "resource.h"
#include "ErrorMessage.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/************************************************************************************
/* + ErrorMessage::ErrorMessage(CWnd* pParent - 
*
*	MFC generated constructor
*
*
*
*/

ErrorMessage::ErrorMessage(CWnd* pParent /*=NULL*/)
	: CDialog(ErrorMessage::IDD, pParent)
	{
	//{{AFX_DATA_INIT(ErrorMessage)
	m_message = _T("");
	//}}AFX_DATA_INIT
	}
/************************************************************************************
/* + void ErrorMessage::DoDataExchange(CDataExchange* pD - 
*
*	standard MFC data exchange function
*
*
*
*/

void ErrorMessage::DoDataExchange(CDataExchange* pDX)
	{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ErrorMessage)
	DDX_Text(pDX, IDC_MESSAGE, m_message);
	//}}AFX_DATA_MAP
	}


BEGIN_MESSAGE_MAP(ErrorMessage, CDialog)
	//{{AFX_MSG_MAP(ErrorMessage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/************************************************************************************
/* + int ErrorMessage::Message(int messageI - 
*
*	function to display error error dialog
*
*
*
*/

int ErrorMessage::Message(
						  int messageID)		// the error message ID of string table
	{

	CString wszResourceString;
	int error;

	error = wszResourceString.LoadString(messageID); ;	// get message string from string table
	if(error == 0)
		{
		wszResourceString  = "Load string failed";;
		}
	
    ::MessageBox(NULL,wszResourceString,_T("IntelliCAD"),MB_ICONSTOP|MB_SETFOREGROUND|MB_TOPMOST );

	return true;

	}


/************************************************************************************
/* + int ErrorMessage::Message(CString &wszErrorMessage) - 
*
*     Displays the string passed in a message box.
*  
*
*
*/
int ErrorMessage::Message(                                // Displays a message box. 
						  CString &wszErrorMessage        // i: Message to be displayed.
						  )
	{

    m_message = wszErrorMessage;
	::MessageBox(NULL,m_message,_T("IntelliCAD"),MB_ICONSTOP|MB_SETFOREGROUND|MB_TOPMOST );

	return true;
	}


