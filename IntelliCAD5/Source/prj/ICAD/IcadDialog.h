/* 
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#ifndef _ICADDIALOG_H
#define _ICADDIALOG_H

class IcadDialog : public CDialog
	{

public:
	IcadDialog(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL) : CDialog(lpszTemplateName, pParentWnd)
		{}

	IcadDialog(UINT nIDTemplate, CWnd* pParentWnd = NULL) : CDialog(nIDTemplate, pParentWnd)
		{}

	IcadDialog() : CDialog() {}


protected:
	virtual BOOL OnInitDialog();
	};


class IcadPropertySheet : public CPropertySheet
	{

public:
	IcadPropertySheet() {}
	IcadPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0)
			: CPropertySheet( nIDCaption, pParentWnd, iSelectPage)
		{}
	IcadPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0)
			: CPropertySheet( pszCaption, pParentWnd, iSelectPage)
		{}

// Operations
public:
	void AddHelpButton();

// Attributes
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(OptionsPropSheet)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

protected:
	CButton	m_help;

	};



#endif	//	_ICADDIALOG_H_

