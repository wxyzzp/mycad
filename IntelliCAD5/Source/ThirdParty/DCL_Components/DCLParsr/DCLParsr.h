// DCLParsr.h : main header file for the DCLPARSR application
//

#if !defined(AFX_DCLPARSR_H__DBA36005_05C2_11D3_87D2_0080C8006398__INCLUDED_)
#define AFX_DCLPARSR_H__DBA36005_05C2_11D3_87D2_0080C8006398__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDCLParsrApp:
// See DCLParsr.cpp for the implementation of this class
//

class CDCLParsrApp : public CWinApp
{
public:
	CDCLParsrApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDCLParsrApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDCLParsrApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DCLPARSR_H__DBA36005_05C2_11D3_87D2_0080C8006398__INCLUDED_)
