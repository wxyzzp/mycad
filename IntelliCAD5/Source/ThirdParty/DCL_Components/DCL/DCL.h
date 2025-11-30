// DCL.h : main header file for the DCL DLL
//

#if !defined(AFX_DCL_H__335C3132_1D56_11D3_880B_0080C8006398__INCLUDED_)
#define AFX_DCL_H__335C3132_1D56_11D3_880B_0080C8006398__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "sds.h"

/////////////////////////////////////////////////////////////////////////////
// CDCLApp
// See DCL.cpp for the implementation of this class
//

class CDCLApp : public CWinApp
{
public:
	CDCLApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDCLApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CDCLApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DCL_H__335C3132_1D56_11D3_880B_0080C8006398__INCLUDED_)
