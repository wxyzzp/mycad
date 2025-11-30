/* D:\DEV\PRJ\ICAD\ICADRUNFORM.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

#include "icad.h"/*DNT*/
#include "IcadApi.h"
#include "cmds.h"/*DNT*/

class RFRM_formdlg : public CDialog {
	DECLARE_DYNCREATE(RFRM_formdlg)

    public:
        long           m_FId;
        CString        m_dbstr;

    public:
        void RFRM_SendAction( LPCTSTR colname,short id);

    protected:
        LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};

char  RFRM_szText[256];

inline void VarToCStr(CString *c, COleVariant *v);
inline void VarToBinay(BYTE **c, COleVariant *v);

void CMainWindow::IcadRunform(char *formname) {
    long          *longptr;
    BYTE          *bptr;
    COleVariant    oleint;
    CString        dbstr;
    long           recnumber;

    m_formdlg = new RFRM_formdlg();

    if(!SDS_CMainWindow->IcadStartDao()) return;

    if ((sds_findfile(SDS_DBFILE,RFRM_szText))!=SDS_RTNORM) {
        dbstr.Format(ResourceString(IDC_ICADRUNFORM_UNABLE_TO_LO_0, "Unable to locate the Database %s" ),RFRM_szText);
        sds_alert(dbstr);
        return;
	}

    if (!SDS_CMainWindow->m_CdatabasePtr.IsOpen()) {
	    try	{ SDS_CMainWindow->m_CdatabasePtr.Open(RFRM_szText,FALSE,TRUE); }
	    catch (CDaoException* e) {
            e->Delete();
		    return;
        }
    }

    SDS_CMainWindow->m_CdatarecordPtr = new CDaoRecordset(&SDS_CMainWindow->m_CdatabasePtr);

    sprintf(RFRM_szText,"Select * from Forminfo where FormName='%s'"/*DNT*/,formname);

    if (!SDS_CMainWindow->m_CdatarecordPtr->IsOpen()) {
        try	{ SDS_CMainWindow->m_CdatarecordPtr->Open(dbOpenDynaset,RFRM_szText); }
	    catch(CDaoException* e)	{ AfxMessageBox(e->m_pErrorInfo->m_strDescription); e->Delete(); return; }
    }

    if ((recnumber=SDS_CMainWindow->m_CdatarecordPtr->GetAbsolutePosition())==-1) {
        sprintf(RFRM_szText,ResourceString(IDC_ICADRUNFORM_FORM___2, "Form \"%s\" was not found" ),formname);
        AfxMessageBox(RFRM_szText);
        return;
    }
    VarToBinay(&bptr,&SDS_CMainWindow->m_CdatarecordPtr->GetFieldValue("FormDefinition"/*DNT*/));
    oleint=SDS_CMainWindow->m_CdatarecordPtr->GetFieldValue("Fkey"/*DNT*/);
    m_formdlg->m_FId=oleint.lVal;

    longptr=(long *)(bptr);
    *longptr=WS_POPUP|WS_VISIBLE|WS_CLIPSIBLINGS|WS_BORDER|WS_DLGFRAME|0x000000C4;

    m_formdlg->CreateIndirect((HGLOBAL)bptr,this);
    m_formdlg->ShowWindow(SW_SHOWNORMAL);
	m_formdlg->UpdateWindow();

	SDS_CMainWindow->m_CdatabasePtr.Close();
}

// =====================================================                               ============================================================
// ===================================================== RFRM_formdlg class functions ============================================================
// =====================================================                               ============================================================

IMPLEMENT_DYNCREATE(RFRM_formdlg,CDialog)

LRESULT RFRM_formdlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) {
	switch(message) {
        case WM_COMMAND:
            if (LOWORD(wParam)==IDCANCEL) break;
            switch(HIWORD(wParam)) {
                case WM_MOUSEMOVE:
                    RFRM_SendAction(ResourceString(IDC_ICADRUNFORM_ONKD_6, "OnKD" ),LOWORD(wParam));
                    break;
                case WM_KEYDOWN:
                    RFRM_SendAction(ResourceString(IDC_ICADRUNFORM_ONKD_6, "OnKD" ),LOWORD(wParam));
                    break;
                case WM_LBUTTONUP:
                    RFRM_SendAction(ResourceString(IDC_ICADRUNFORM_ONMUP_7, "OnMUp" ),LOWORD(wParam));
                    break;
                case WM_LBUTTONDOWN:
                    RFRM_SendAction(ResourceString(IDC_ICADRUNFORM_ONMD_8, "OnMD" ),LOWORD(wParam));
                    break;
                case WM_KILLFOCUS:
//                case EN_KILLFOCUS:
                case BN_KILLFOCUS:
                    RFRM_SendAction(ResourceString(IDC_ICADRUNFORM_ONLCLICK_9, "OnLClick" ),LOWORD(wParam));
                    break;
//                case EN_SETFOCUS:
                case BN_SETFOCUS:
                    RFRM_SendAction(ResourceString(IDC_ICADRUNFORM_ONGCLICK_10, "OnGClick" ),LOWORD(wParam));
                    break;
                case BN_DBLCLK:
                    RFRM_SendAction(ResourceString(IDC_ICADRUNFORM_ONDCLICK_11, "OnDClick" ),LOWORD(wParam));
                    break;
                case BN_CLICKED:
                    RFRM_SendAction(ResourceString(IDC_ICADRUNFORM_ONCLICK_12, "OnClick" ),LOWORD(wParam));
                    break;
                case EN_UPDATE:
                    RFRM_SendAction(ResourceString(IDC_ICADRUNFORM_ONBUPD_13, "OnBUpd" ),LOWORD(wParam));
                    break;
                case EN_CHANGE:
                    RFRM_SendAction(ResourceString(IDC_ICADRUNFORM_ONCHNG_14, "OnChng" ),LOWORD(wParam));
                    break;
            }
            break;

    }
	return CDialog::WindowProc(message, wParam, lParam);
}

void RFRM_formdlg::RFRM_SendAction( LPCTSTR colname,short id) {
    sprintf(RFRM_szText,ResourceString(IDC_ICADRUNFORM_SELECT__S_F_15, "Select %s from Itempropvals where Fkey=%d and Id=%d" ),colname,m_FId,id);
    if (SDS_CMainWindow->m_CdatarecordPtr->IsOpen()) SDS_CMainWindow->m_CdatarecordPtr->Close();
    SDS_CMainWindow->m_CdatarecordPtr->Open(dbOpenDynaset,RFRM_szText);
    if (SDS_CMainWindow->m_CdatarecordPtr->IsBOF()) return;
    VarToCStr(&m_dbstr,&SDS_CMainWindow->m_CdatarecordPtr->GetFieldValue(colname));
    if (!m_dbstr.IsEmpty()) SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)((LPCTSTR)m_dbstr));
}

//**************************************************************************************************************
//**************************************** what to do **********************************************************
//**************************************************************************************************************
inline void VarToBinay(BYTE **c, COleVariant *v) {
  if(v->vt==8209) {    
	*c = new BYTE [v->parray->rgsabound->cElements];
    memcpy(*c,(BYTE *)v->parray->pvData,v->parray->rgsabound->cElements);
  } else
    *c = NULL;
}

inline void VarToCStr(CString *c, COleVariant *v) {
  if(v->vt==VT_BSTR)
    *c = (LPCTSTR)v->bstrVal;
  else
    *c = _T(""/*DNT*/);
}


