/** FILENAME:     DCL.h- Contains declarations of class CDCL.
* Copyright  (C) Copyright 1998-99 Visio Corporation.  All Rights Reserved.
*
*
*
* Abstract 
*
*
*
*Audit Log 
*
*
*Audit Trail
*
*
*/


#ifndef _DCL_H
#define _DCL_H


#if _MSC_VER > 1000
#pragma once
#endif 

#define DCL_TITLE _T("TreeViewer ")

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols


class CDCLApp : public CWinApp
{
public:
	CDCLApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDCLApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CDCLApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif 
