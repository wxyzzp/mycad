// imagemenu.h : main header file for the IMAGEMENU DLL
//

#if !defined(AFX_IMAGEMENU_H__3ED590AA_B478_11D1_BD5D_0020AFEA3A98__INCLUDED_)
#define AFX_IMAGEMENU_H__3ED590AA_B478_11D1_BD5D_0020AFEA3A98__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

// Declarations of defined commands
int imagemenu(struct sds_resbuf *rb);

//  Declarations of other local support functions
int    main       (int, char **);
int    invokefun  (void);
int    funcload   (void);


/////////////////////////////////////////////////////////////////////////////
// CImagemenuApp
// See imagemenu.cpp for the implementation of this class
//

class CImagemenuApp : public CWinApp
{
public:
	CImagemenuApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImagemenuApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CImagemenuApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMAGEMENU_H__3ED590AA_B478_11D1_BD5D_0020AFEA3A98__INCLUDED_)
