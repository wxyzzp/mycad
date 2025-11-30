/* D:\ICADDEV\PRJ\ICAD\ICADXLINK.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * $Revision: 1.1.1.1 $ $Date: 2000/01/19 13:41:22 $ 
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#pragma once
#include "IcadDialog.h"

#define XLINK_TABLE	   "XLINKINFO"

class CXlnkWizard;

class CXlnkWiz1:public CPropertyPage {
    public:
        BOOL         m_foundflg;
        CEdit        m_Edit;
        CStatic      m_Picture;
        CString      m_xlink;
        CXlnkWizard *m_pToSheet;

    public:
        BOOL OnInitDialog();
             CXlnkWiz1();
        BOOL OnSetActive();
    protected:
        virtual LRESULT OnWizardNext();
        virtual void    DoDataExchange(CDataExchange *pDC);
        afx_msg void    OnESetfocus();

	DECLARE_MESSAGE_MAP()
};

class CXlnkWiz2:public CPropertyPage {
    public:
        CStringList  m_DbStrList;
        CListBox     m_DbList;
        CString      m_Database,
                     m_dbpath;
        CXlnkWizard *m_pToSheet;

    public:
        BOOL OnInitDialog();
             CXlnkWiz2();
        BOOL OnSetActive();

    protected:
        LRESULT         WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
        virtual LRESULT OnWizardNext();
        afx_msg void    OnBrowse();

	DECLARE_MESSAGE_MAP()
};

class CXlnkWiz3:public CPropertyPage {
    public:
        CListBox      m_TabList,
                      m_ColList,
                      m_KeyList;
        CXlnkWizard  *m_pToSheet;

    public:
        BOOL OnInitDialog();
             CXlnkWiz3();
        BOOL OnSetActive();
    protected:
        LRESULT      WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
        afx_msg void OnAone();
        afx_msg void OnAall();
        afx_msg void OnRone();
        afx_msg void OnRall();
		virtual BOOL OnWizardFinish( );

    DECLARE_MESSAGE_MAP()
};

class CXlnkWizard:public IcadPropertySheet {
    public:
        CXlnkWiz1       m_Wiz1;
        CXlnkWiz2       m_Wiz2;
        CXlnkWiz3       m_Wiz3;
        CDaoDatabase    m_CdatabasePtr;
		CDaoDatabase    m_CxlinkdbPtr;
		CDaoRecordset  *m_CxlinkrcPtr;
		CMainWindow    *m_pToMain;

    public:
             CXlnkWizard();
        BOOL OnInitDialog();
};


void        DeleteXlink(CString name);
inline void VarToCStr(CString *c, COleVariant *v);


