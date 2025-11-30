/* D:\ICADDEV\PRJ\ICAD\ICADNEWWIZ.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * $Revision: 1.1.1.1.18.1 $ $Date: 2004/08/16 16:29:36 $ 
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#pragma once

#include "IcadDialog.h"

class CIcadNewWizard;

//This is for the first wizard page
class CIcadNewWiz1:public CPropertyPage {
    public:
        int iNewDwg;
        CIcadNewWizard *m_pToSheet;
        CIcadNewWiz1();
        BOOL OnSetActive();
    protected:
        virtual LRESULT OnWizardNext();
        virtual void DoDataExchange(CDataExchange *pDX);

    DECLARE_MESSAGE_MAP()
};

//This is for the second wizard page
class CIcadNewWiz2:public CPropertyPage {
    public:
		CMainWindow* m_pMain;
        CString m_csDwgFile, m_csTemplatePath;
        CIcadNewWizard *m_pToSheet;
        BOOL OnSetActive();
        void OnBrowse();
        CIcadNewWiz2();
    protected:
        virtual LRESULT OnWizardBack();
        virtual BOOL OnWizardFinish();
        virtual void DoDataExchange(CDataExchange *pDX);

    DECLARE_MESSAGE_MAP()
};

//This is for the third wizard page
class CIcadNewWiz3:public CPropertyPage {
    public:
        CIcadNewWizard *m_pToSheet;
        CIcadNewWiz3();
        BOOL OnSetActive();
    protected:
        virtual LRESULT OnWizardBack();
        virtual LRESULT OnWizardNext();
        virtual void DoDataExchange(CDataExchange *pDX);

    DECLARE_MESSAGE_MAP()
};

//This is for the fourth wizard page
class CIcadNewWiz4:public CPropertyPage {
    public:
        CIcadNewWizard *m_pToSheet;
        CIcadNewWiz4();
        BOOL OnSetActive();
    protected:
        virtual LRESULT OnWizardBack();
        virtual LRESULT OnWizardNext();
        virtual void DoDataExchange(CDataExchange *pDX);

    DECLARE_MESSAGE_MAP()
};

//This is for the fourth wizard page
class CIcadNewWiz5:public CPropertyPage {
    public:
        CIcadNewWizard *m_pToSheet;
        CIcadNewWiz5();
        BOOL OnSetActive();
    protected:
        virtual LRESULT OnWizardBack();
        virtual LRESULT OnWizardNext();
        virtual void DoDataExchange(CDataExchange *pDX);

    DECLARE_MESSAGE_MAP()
};

//This is for the fifth wizard page
class CIcadNewWiz6:public CPropertyPage {
    public:
        int iColor,iBlip,iSnap,iGrid,iUcs,DoOnce;
        CIcadNewWizard *m_pToSheet;
        CIcadNewWiz6();
        void OnColor();
        BOOL OnSetActive();
        BOOL OnInitDialog();
        CComboBox m_CLinetype;
    protected:
        void PaintIt();
        afx_msg void OnPaint();
        virtual LRESULT OnWizardBack();
        virtual BOOL OnWizardFinish();
        virtual void DoDataExchange(CDataExchange *pDX);

    DECLARE_MESSAGE_MAP()
};

//This is for the main wizard sheet
class CIcadNewWizard:public CPropertySheet {
    public:
        CIcadNewWiz1 m_NewWiz1;
        CIcadNewWiz2 m_NewWiz2;
        CIcadNewWiz3 m_NewWiz3;
        CIcadNewWiz4 m_NewWiz4;
        CIcadNewWiz5 m_NewWiz5;
        CIcadNewWiz6 m_NewWiz6;
        CIcadNewWizard();
        BOOL OnInitDialog();
    protected:
        afx_msg void OnHelp();


    DECLARE_MESSAGE_MAP()

};


