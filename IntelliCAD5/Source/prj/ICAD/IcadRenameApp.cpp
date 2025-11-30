/* C:\DEV\PRJ\ICAD\ICADRENAMEAPP.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */


//** Includes
#include "Icad.h"/*DNT*/
#include "IcadApi.h"
#include "cmds.h"/*DNT*/
#include "resource.hm"/*DNT*/
#include "paths.h"
#include "IcadDialog.h"

class CReNameApp:public IcadDialog {
    public:
        CString csOldName,csNewName;
        int iOverwrite;
        CReNameApp();
        BOOL OnInitDialog();
    protected:
        virtual void OnOK();
        afx_msg void OnHelp();
        virtual void DoDataExchange(CDataExchange *pDX);

    DECLARE_MESSAGE_MAP()
};


BEGIN_MESSAGE_MAP(CReNameApp,CDialog)
    ON_BN_CLICKED(IDHELP,OnHelp)
END_MESSAGE_MAP()

void CReNameApp::OnOK(){
    char           string[IC_ACADBUF];
    struct resbuf *rb,
                  *rbb;
    UpdateData(true);
    rb=NULL;

    if(csOldName.IsEmpty()){
        ::MessageBox(m_hWnd,ResourceString(IDC_ICADRENAMEAPP_YOU_MUST_S_0, "You must supply an Old Application name." ),NULL,MB_OK|MB_ICONSTOP);
        ::SetFocus(::GetDlgItem(m_hWnd,RE_APP_OLDNAME));
        return;
    }
    rb=SDS_tblgetter("APPID"/*DNT*/,NULL,TRUE,SDS_CURDWG);
    while (rb) {
        for(rbb=rb;rbb;rbb=rbb->rbnext) {
            if (rbb->restype==2) { if (strisame((char *)((LPCTSTR)csOldName),rbb->resval.rstring)) break; }
        }
        if (rbb) break;
        IC_RELRB(rb);
        rb=SDS_tblgetter("APPID"/*DNT*/,NULL,FALSE,SDS_CURDWG);
    }
    if (!rbb) {
        sprintf(string,ResourceString(IDC_ICADRENAMEAPP_NO_APPLICA_2, "No Application name %s has been registered." ),(char *)((LPCTSTR)csOldName));
        ::MessageBox(m_hWnd,string,NULL,MB_OK|MB_ICONSTOP);
        ::SetFocus(::GetDlgItem(m_hWnd,RE_APP_OLDNAME));
        return;
    }
    if(csNewName.IsEmpty()){
        ::MessageBox(m_hWnd,ResourceString(IDC_ICADRENAMEAPP_YOU_MUST_S_3, "You must supply a New Application name." ),NULL,MB_OK|MB_ICONSTOP);
        ::SetFocus(::GetDlgItem(m_hWnd,RE_APP_NEWNAME));
        return;
    }
    for(rbb=rb;rbb;rbb=rbb->rbnext) {
        if (rbb->restype==2) { if (strisame((char *)((LPCTSTR)csNewName),rbb->resval.rstring)) break; }
    }
    IC_RELRB(rb);
    CDialog::OnOK();
}

CReNameApp::OnInitDialog() {

    return(IcadDialog::OnInitDialog());
}

void CReNameApp::DoDataExchange(CDataExchange *pDX) {
    DDX_Text(pDX,RE_APP_OLDNAME,csOldName);
    DDV_MaxChars(pDX,csOldName,100);
    DDX_Text(pDX,RE_APP_NEWNAME,csNewName);
    DDV_MaxChars(pDX,csNewName,100);
    DDX_Check(pDX,RE_APP_OVERWRITE,iOverwrite);
}

void CReNameApp::OnHelp() {
	CommandRefHelp( m_hWnd, HLP_REASSOCAPP);
}

CReNameApp::CReNameApp():IcadDialog(IDD_RENAME_APP) {
}

//***This is the main window
void CMainWindow::IcadRenameApp(void) {
    int ret;
    sds_name ss;
    char szNew[IC_ACADBUF],szOld[IC_ACADBUF];


    CReNameApp CReApp;
    ret=CReApp.DoModal();
	if(ret==IDOK) {
        if(sds_pmtssget(ResourceString(IDC_ICADRENAMEAPP__NSELECT_T_5, "\nSelect the entities to re-associate Entity Data with the new application" ),NULL,NULL,NULL,NULL,ss,0)!=RTNORM) {
            return;
        }

        strcpy(szOld,CReApp.csOldName);
        strcpy(szNew,CReApp.csNewName);

        cmd_renameed(ss,szOld,szNew,CReApp.iOverwrite);
	    if(ss) sds_ssfree(ss);
    }
}



