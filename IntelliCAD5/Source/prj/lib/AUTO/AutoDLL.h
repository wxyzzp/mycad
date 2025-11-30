// AutoDLL.h : main header file for the AUTODLL DLL
//

#if !defined(AFX_AUTODLL_H__E286D9A3_7E50_11D2_BE7C_00C04F990354__INCLUDED_)
#define AFX_AUTODLL_H__E286D9A3_7E50_11D2_BE7C_00C04F990354__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "icadRes.h"		// main symbols

#ifdef	AUTODLL
#define	AUTO_DECLSPEC __declspec( dllexport)
#else
#define	AUTO_DECLSPEC __declspec( dllimport)
#endif

#define	AUTO_API	EXTERN_C AUTO_DECLSPEC

AUTO_API struct IIcadPlotManager *		CreatePlotManager();
AUTO_API struct IIcadPlotProperties *	CreatePlotProperties();

AUTO_API HRESULT				RegisterAutomation( BOOL always, ITypeLib **ppITypeLib);
AUTO_API HRESULT				UnregisterAutomation();


/////////////////////////////////////////////////////////////////////////////
// CAutoDLLApp
// See AutoDLL.cpp for the implementation of this class
//
class CAutoDLLApp : public CWinApp
{
public:
	CAutoDLLApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoDLLApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CAutoDLLApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTODLL_H__E286D9A3_7E50_11D2_BE7C_00C04F990354__INCLUDED_)
