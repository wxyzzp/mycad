/* D:\DEV\PRJ\ICAD\ICADXLINK.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

#include "Icad.h"/*DNT*/
#include "IcadApi.h"
#include "IcadXlink.h"/*DNT*/

#define XLINK_WIZ1_STR1 ResourceString(IDC_ICADXLINK_WELCOME_TO_THE_1, "Welcome to the XLink Wizard! This wizard helps you prepare an XLink. An XLink is used to link an entity in your drawing with a record in a database. Please select next if you wish to continue." )
char    XLINK_szText[256];

// ====================================================                           ===========================================================
// ==================================================== CXlnkWiz1 class functions ===========================================================
// ====================================================                           ===========================================================
BEGIN_MESSAGE_MAP(CXlnkWiz1,CPropertyPage)
    ON_EN_SETFOCUS(XLINK_NAME,OnESetfocus)
END_MESSAGE_MAP()

void CXlnkWiz1::OnESetfocus() {
    HBITMAP hBitmap;

    if (m_foundflg) {
        hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(XLINK_LINK1));
        SetDlgItemText(XLINK_TEXT,XLINK_WIZ1_STR1);
        m_Picture.SetBitmap(hBitmap);
        m_foundflg=FALSE;
    }
}

BOOL CXlnkWiz1::OnSetActive() {
    CPropertyPage::OnSetActive();
	m_pToSheet->SetWizardButtons(PSWIZB_NEXT);
	return (TRUE);
}

BOOL CXlnkWiz1::OnInitDialog() {
    CPropertyPage::OnInitDialog();

    m_foundflg=FALSE;

    SetDlgItemText(XLINK_TEXT,XLINK_WIZ1_STR1);
    m_Edit.SubclassDlgItem(XLINK_NAME,this);
    m_Picture.SubclassDlgItem(XLINK_PIC1,this);

	try	{
        m_pToSheet->m_CxlinkdbPtr.Open(SDS_DBFILE);
	}
	catch (CDaoException* e) {
		AfxMessageBox(e->m_pErrorInfo->m_strDescription);
		e->Delete();
    }

	m_pToSheet->m_CxlinkrcPtr = new CDaoRecordset(&m_pToSheet->m_CxlinkdbPtr);

    try	{
		m_pToSheet->m_CxlinkrcPtr->Open(dbOpenDynaset,ResourceString(IDC_ICADXLINK_SELECT___FROM__2, "SELECT * FROM xlinkinfo" ));
	}
	catch(CDaoException* e)	{
		AfxMessageBox(e->m_pErrorInfo->m_strDescription);
		e->Delete();
	}

    return TRUE;
}

void CXlnkWiz1::DoDataExchange(CDataExchange *pDX) {
    CPropertyPage::DoDataExchange(pDX);

    DDX_Text(pDX,XLINK_NAME,m_xlink);
}


LRESULT CXlnkWiz1::OnWizardNext() {
    CString String;
    HBITMAP hBitmap;

    hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(XLINK_LINK2));

    UpdateData();
    String.Format("Xlink=\'%s\'"/*DNT*/,m_xlink);
    if (m_pToSheet->m_CxlinkrcPtr->Find(AFX_DAO_FIRST,(LPCTSTR)String)) {
        m_foundflg=TRUE;
        String.Format(ResourceString(IDC_ICADXLINK_THE_XLINK_NAME_4, "The Xlink name \'%s\' is currently defined,Please enter a different XLink key name" ),m_xlink);
        SetDlgItemText(XLINK_TEXT,String);
        m_Picture.SetBitmap(hBitmap);
        return(-1);
    }

    return(0);
}

CXlnkWiz1::CXlnkWiz1():CPropertyPage(IDD_WIZARD1) { }

// ====================================================                           ===========================================================
// ==================================================== CXlnkWiz2 class functions ===========================================================
// ====================================================                           ===========================================================
BEGIN_MESSAGE_MAP(CXlnkWiz2,CPropertyPage)
    ON_BN_CLICKED(XLNK_BROWSE,OnBrowse)
END_MESSAGE_MAP()

BOOL CXlnkWiz2::OnSetActive() {
    CPropertyPage::OnSetActive();
	m_pToSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	return (TRUE);
}

BOOL CXlnkWiz2::OnInitDialog() {
    CString dbstr,
            sbstr;
    short   index;

    CPropertyPage::OnInitDialog();

    m_DbList.SubclassDlgItem(XLNK_DATABLIST,this);

    if (m_pToSheet->m_CxlinkrcPtr->IsOpen()) m_pToSheet->m_CxlinkrcPtr->Close();
    try {
        m_pToSheet->m_CxlinkrcPtr->Open(dbOpenDynaset,ResourceString(IDC_ICADXLINK_SELECT_DISTINC_5, "SELECT DISTINCT Database FROM xlinkinfo" ));
    }
	catch(CDaoException* e)	{
		AfxMessageBox(e->m_pErrorInfo->m_strDescription);
        e->Delete();
    }

    while (!m_pToSheet->m_CxlinkrcPtr->IsBOF() && !m_pToSheet->m_CxlinkrcPtr->IsEOF()) {
        VarToCStr(&dbstr,&m_pToSheet->m_CxlinkrcPtr->GetFieldValue("Database"/*DNT*/));
        index=dbstr.Find('.');
        sbstr=dbstr.Left(index);
        m_DbList.AddString((LPCTSTR)sbstr);
        m_DbStrList.AddTail(sbstr);
        m_pToSheet->m_CxlinkrcPtr->MoveNext();
    }

    if (m_pToSheet->m_CxlinkrcPtr->IsOpen()) m_pToSheet->m_CxlinkrcPtr->Close();
    try {
        m_pToSheet->m_CxlinkrcPtr->Open(dbOpenDynaset,ResourceString(IDC_ICADXLINK_SELECT___FROM__2, "SELECT * FROM xlinkinfo" ));
    }
	catch(CDaoException* e)	{
		AfxMessageBox(e->m_pErrorInfo->m_strDescription);
        e->Delete();
    }

    return(TRUE);
}

LRESULT CXlnkWiz2::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) {
	switch(message) {
        case WM_COMMAND:
            if ((HIWORD(wParam)==LBN_SELCHANGE) && (LOWORD(wParam)==XLNK_DATABLIST)) {
                {
                    short   index;
                    CString DBname,
                            DBpath,
                            String;

                    index=m_DbList.GetCurSel();
                    m_DbList.GetText(index,DBname);

                    String.Format("Database=\'%s.mdb\'"/*DNT*/,DBname);
                    DBname+=".mdb"/*DNT*/;
                    if (m_pToSheet->m_CxlinkrcPtr->Find(AFX_DAO_FIRST,(LPCTSTR)String)) {
                        VarToCStr(&DBpath,&m_pToSheet->m_CxlinkrcPtr->GetFieldValue("Path"/*DNT*/));
                        String  =DBpath; String+=DBname;
                        m_dbpath=DBpath; m_Database=DBname;
                        SetDlgItemText(XLNK_SEL_DB,String);
                    }
                }
            }
            break;
	}
	return CPropertyPage::WindowProc(message, wParam, lParam);
}

void CXlnkWiz2::OnBrowse() {
    CString       String;
    short         index;
    struct resbuf rb;
	// EBATECH(CNBR) 2002/4/30 Disable TYPE button option+2
    if (RTNORM!=sds_getfiled(ResourceString(IDC_ICADXLINK_OPEN_10, "Open" ),""/*DNT*/,"Database|mdb"/*DNT*/,4+2,&rb)) return;
    if(rb.restype==RTSTR) { String=rb.resval.rstring; IC_FREE(rb.resval.rstring); }
    SetDlgItemText(XLNK_SEL_DB,String);

    index=String.ReverseFind('\\'/*DNT*/);
    m_dbpath  =String.Left(index+1);
    m_Database=(LPCTSTR)String+(index+1);
}

LRESULT CXlnkWiz2::OnWizardNext() {
    CString String;

    String.Format("%s%s"/*DNT*/,m_pToSheet->m_Wiz2.m_dbpath,m_pToSheet->m_Wiz2.m_Database);
    if (m_pToSheet->m_CdatabasePtr.IsOpen()) m_pToSheet->m_CdatabasePtr.Close();
	try	{
        m_pToSheet->m_CdatabasePtr.Open(String);
	}
	catch (CDaoException* e) {
		AfxMessageBox(e->m_pErrorInfo->m_strDescription);
		e->Delete();
        return -1;
    }

    return 0;
}

CXlnkWiz2::CXlnkWiz2():CPropertyPage(IDD_WIZARD2) { }

// ====================================================                           ===========================================================
// ==================================================== CXlnkWiz3 class functions ===========================================================
// ====================================================                           ===========================================================
BEGIN_MESSAGE_MAP(CXlnkWiz3,CPropertyPage)
    ON_BN_CLICKED(XLNK_ADD_ONE,OnAone)
    ON_BN_CLICKED(XLNK_ADD_ALL,OnAall)
    ON_BN_CLICKED(XLNK_REM_ONE,OnRone)
    ON_BN_CLICKED(XLNK_REM_ALL,OnRall)
END_MESSAGE_MAP()

void CXlnkWiz3::OnAone() {
    short         index;
    CString       Tname,
                  Cname,
                  String;

    index=m_TabList.GetCurSel();
    m_TabList.GetText(index,Tname);

    index=m_ColList.GetCurSel();
    m_ColList.GetText(index,Cname);

    String.Format("%s.%s"/*DNT*/,Tname,Cname);

    m_KeyList.AddString(String);
}

void CXlnkWiz3::OnAall() {
    short         index,
                  fi1;
    CString       Tname,
                  Cname,
                  String;
	CDaoFieldInfo fieldInfo;

    CDaoTableDef Ctabdef(&m_pToSheet->m_CdatabasePtr);
    index=m_TabList.GetCurSel();
    m_TabList.GetText(index,Tname);
	Ctabdef.Open(Tname);
    index=Ctabdef.GetFieldCount();
    for(fi1=0;fi1<index;++fi1) {
        m_ColList.GetText(fi1,Cname);
        String.Format("%s.%s"/*DNT*/,Tname,Cname);
        m_KeyList.AddString(String);
    }
}

void CXlnkWiz3::OnRone() {
    short index;

    index=m_TabList.GetCurSel();
    m_TabList.DeleteString(index);
}

void CXlnkWiz3::OnRall() {
    m_KeyList.ResetContent();
}

LRESULT CXlnkWiz3::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) {
	switch(message) {
        case WM_COMMAND:
            if ((HIWORD(wParam)==LBN_SELCHANGE) && (LOWORD(wParam)==XLNK_TABLST)) {
                {
                    short         index,
                                  fi1;
                    CString       Tname;
				    CDaoFieldInfo fieldInfo;

                    CDaoTableDef Ctabdef(&m_pToSheet->m_CdatabasePtr);
                    index=m_TabList.GetCurSel();
                    m_TabList.GetText(index,Tname);
				    Ctabdef.Open(Tname);
                    index=Ctabdef.GetFieldCount();
                    m_ColList.ResetContent();
                    m_KeyList.ResetContent();
                    for(fi1=0;fi1<index;++fi1) {
                		Ctabdef.GetFieldInfo(fi1,fieldInfo);
                        m_ColList.AddString(fieldInfo.m_strName);
                    }
                }
            }
            break;
	}
	return CPropertyPage::WindowProc(message, wParam, lParam);
}

BOOL CXlnkWiz3::OnSetActive() {
    short            index,
                     fi1;
    long             attrib;
    CDaoTableDefInfo tabinfo;

    CDaoTableDef Ctabdef(&m_pToSheet->m_CdatabasePtr);

    CPropertyPage::OnSetActive();
    m_pToSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);

    m_TabList.ResetContent();
    index=m_pToSheet->m_CdatabasePtr.GetTableDefCount();
    for(fi1=0;fi1<index;++fi1) {
        m_pToSheet->m_CdatabasePtr.GetTableDefInfo(fi1,tabinfo);
        Ctabdef.Open(tabinfo.m_strName);
        attrib=Ctabdef.GetAttributes();
        if (attrib^dbRelationDontEnforce) {
            m_TabList.AddString(tabinfo.m_strName);
        }
        Ctabdef.Close();
    }

    return (TRUE);
}

BOOL CXlnkWiz3::OnInitDialog() {
    CPropertyPage::OnInitDialog();

    m_TabList.SubclassDlgItem(XLNK_TABLST,this);
    m_ColList.SubclassDlgItem(XLNK_COLLIST,this);
    m_KeyList.SubclassDlgItem(XLNK_KEY_LIST,this);

    return TRUE;
}

BOOL CXlnkWiz3::OnWizardFinish() {
	short		 index,
				 fi1,
                 fi2;
	CString		 Strkey,
				 String;

	m_pToSheet->m_CxlinkrcPtr->AddNew();

    COleVariant  varFieldValue1  (m_pToSheet->m_Wiz1.m_xlink,VT_BSTRT);
    COleVariant  varFieldValue2  (m_pToSheet->m_Wiz2.m_dbpath,VT_BSTRT);
    COleVariant  varFieldValue3  (m_pToSheet->m_Wiz2.m_Database,VT_BSTRT);
	index=m_KeyList.GetCount();
    if (index) {
        fi1=0;
        m_KeyList.GetText(fi1,Strkey);
        String=Strkey;
        for(++fi1;fi1<index;++fi1) {
            m_KeyList.GetText(fi1,Strkey);
            fi2=Strkey.Find('.');
		    String+=(LPCTSTR)Strkey+fi2;
        }
    }
    COleVariant  varFieldValue4  (String,VT_BSTRT );

	m_pToSheet->m_CxlinkrcPtr->SetFieldValue(0,varFieldValue1);
	m_pToSheet->m_CxlinkrcPtr->SetFieldValue(1,varFieldValue2);
	m_pToSheet->m_CxlinkrcPtr->SetFieldValue(2,varFieldValue3);
	m_pToSheet->m_CxlinkrcPtr->SetFieldValue(3,varFieldValue4);
	m_pToSheet->m_CxlinkrcPtr->Update();

	return TRUE;
}

CXlnkWiz3::CXlnkWiz3():CPropertyPage(IDD_WIZARD3) { }

// ====================================================                           ===========================================================
// ==================================================== CXlnkWizard class functions ===========================================================
// ====================================================                           ===========================================================
BOOL CXlnkWizard::OnInitDialog() {
    m_Wiz1.m_pToSheet=m_Wiz2.m_pToSheet=m_Wiz3.m_pToSheet=this;
    return(IcadPropertySheet::OnInitDialog());
}

CXlnkWizard::CXlnkWizard():IcadPropertySheet() {
    AddPage(&m_Wiz1);
    AddPage(&m_Wiz2);
    AddPage(&m_Wiz3);
}

// ====================================================                           ===========================================================
// ==================================================== CMainWindow class functions ===========================================================
// ====================================================                           ===========================================================
void CMainWindow::DisplayWizard(void) {
	CXlnkWizard Wizard;

	Wizard.m_pToMain=this;

    if (Wizard.m_pToMain->m_CdbWorkSpace.IsOpen()) {
        Wizard.SetWizardMode();
        Wizard.DoModal();
        if (Wizard.m_CxlinkrcPtr->IsOpen())  Wizard.m_CxlinkrcPtr->Close();
        if (Wizard.m_CxlinkdbPtr.IsOpen())  Wizard.m_CxlinkdbPtr.Close();
        if (Wizard.m_CdatabasePtr.IsOpen()) Wizard.m_CdatabasePtr.Close();
    }
}


void DeleteXlink(CString name) {
    CDaoDatabase   CdatabasePtr;
    CDaoRecordset *CxlinkPtr;
    CString        String;

    if ((sds_findfile(SDS_DBFILE,XLINK_szText))!=SDS_RTNORM) return;
	try	{ CdatabasePtr.Open(SDS_DBFILE		); }
	catch (CDaoException* e) { e->Delete(); }
    CxlinkPtr = new CDaoRecordset(&CdatabasePtr);
    try	{ CxlinkPtr->Open(dbOpenDynaset,ResourceString(IDC_ICADXLINK_SELECT___FROM_15, "Select * from xlinkinfo" )); }
	catch(CDaoException* e)	{ AfxMessageBox(e->m_pErrorInfo->m_strDescription); e->Delete(); }

    String.Format("Xlink='%s'"/*DNT*/,(LPCTSTR)name);
    if (CxlinkPtr->Find(AFX_DAO_FIRST,String)) {
        CxlinkPtr->Delete();
        delete CxlinkPtr;
    }
    CdatabasePtr.Close();
}

inline void VarToCStr(CString *c, COleVariant *v) {
  if(v->vt==VT_BSTR)
    *c = (LPCTSTR)v->bstrVal;
  else
    *c = _T(""/*DNT*/);
}


