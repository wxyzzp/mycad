/* F:\RC98C\PRJ\API\SDS\SDSDIALOG.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 *	Classes to facilitate creation of modal dialogs from SDS applications
 *	written as MFC Extension DLLs.
 * 
 *	$Revision: 1.1.1.1 $ $Date: 2000/01/19 13:41:15 $
 *
 */ 

#ifndef _SDSDIALOG_H
#define _SDSDIALOG_H

#ifndef __AFXDLGS_H__
#error	#include "SdsDialog.h" must follow #include <afxdlgs.h>
#endif

class SdsDialog : public CDialog
	{
public:
	SdsDialog( LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL )
		: CDialog( lpszTemplateName, pParentWnd) {}
	SdsDialog( UINT nIDTemplate, CWnd* pParentWnd = NULL )
		: CDialog( nIDTemplate, pParentWnd) {}

	static UINT	GetDoModalMsg();
	static int SendDoModalMsg( CDialog *);
	static int SendDoModalMsg( CPropertySheet *);

	int DoModal();

protected:
	static UINT m_doModalMsg;
	};


class SdsPropertyPage : public CPropertyPage
	{
public:
	SdsPropertyPage( UINT nIDTemplate, UINT nIDCaption = 0  )
		: CPropertyPage( nIDTemplate, nIDCaption) {}
	SdsPropertyPage( LPCTSTR lpszTemplateName, UINT nIDCaption  = 0 )
		: CPropertyPage( lpszTemplateName, nIDCaption) {}

	int DoModal();
	};


class SdsPropertySheet : public CPropertySheet
	{
public:
	SdsPropertySheet( UINT nIDCaption, CWnd *pParentWnd = NULL, UINT iSelectPage = 0  )
		: CPropertySheet( nIDCaption, pParentWnd, iSelectPage) {}
	SdsPropertySheet( LPCTSTR pszCaption, CWnd *pParentWnd = NULL, UINT iSelectPage = 0 )
		: CPropertySheet( pszCaption, pParentWnd, iSelectPage) {}

	int DoModal();
	};


#endif
