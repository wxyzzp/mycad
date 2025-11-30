/* C:\ICADDEV\PRJ\ICAD\ICADATTEXTDIA.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

//*********************************************************************
//*  IcadAttExtDia.cpp                                                *
//*  Copyright (C) 1994,1995, 1996, 1997 by Visio, Inc.               *
//*                                                                   *
//*********************************************************************

//** Includes
#include "Icad.h"/*DNT*/
#include "IcadApi.h"
#include "IcadDoc.h"/*DNT*/
#include "cmds.h"/*DNT*/
#include "resource.hm"/*DNT*/
#include "paths.h"
#include "IcadDialog.h"

#define dwg_entlink	db_handitem

class ext_CAttExt:public IcadDialog {
    public:
        ext_CAttExt();
        BOOL OnInitDialog();
        long lNumOfAtts;
        int iExtType;
        CString csTemplate,csOutFile;
    protected:
        afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
        afx_msg void OnHelp();
        afx_msg void OnSelect();
        afx_msg void OnDxfClick();
        afx_msg void OnDfClick();
        afx_msg void OnBrowse1();
        afx_msg void OnBrowse2();
        virtual void OnOK();
        virtual void DoDataExchange(CDataExchange *pDX);

    DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(ext_CAttExt,CDialog)
    ON_BN_CLICKED(EXT_SELECT,OnSelect)
    ON_BN_CLICKED(EXT_DXF,OnDxfClick)
    ON_BN_CLICKED(EXT_CDF,OnDfClick)
    ON_BN_CLICKED(EXT_SDF,OnDfClick)
    ON_BN_CLICKED(EXT_BROWSE1,OnBrowse1)
    ON_BN_CLICKED(EXT_BROWSE2,OnBrowse2)
	ON_WM_HELPINFO()
    ON_BN_CLICKED(IDHELP,OnHelp)
END_MESSAGE_MAP()

BOOL ext_CAttExt::OnHelpInfo(HELPINFO *pHel){
	OnHelp();
	return(true);
}

void ext_CAttExt::OnHelp() {
	CommandRefHelp( m_hWnd, HLP_ATTEXT);
}

void ext_CAttExt::OnOK() {
    CDialog::OnOK();
}

void ext_CAttExt::OnSelect() {
    EndDialog(EXT_SELECT);
}

void ext_CAttExt::OnDxfClick() {
    ::EnableWindow(::GetDlgItem(m_hWnd,EXT_TEMPLATE),false);
    ::EnableWindow(::GetDlgItem(m_hWnd,EXT_BROWSE1),false);
}

void ext_CAttExt::OnDfClick() {
    ::EnableWindow(::GetDlgItem(m_hWnd,EXT_TEMPLATE),true);
    ::EnableWindow(::GetDlgItem(m_hWnd,EXT_BROWSE1),true);
}

void ext_CAttExt::OnBrowse1() {
    struct resbuf rb;
    char title[40],*def;
    int flags=4+2;	// EBATECH(CNBR) 2002/4/30 Disable TYPE button option+2

    UpdateData(true);
    strcpy(title,ResourceString(IDC_ICADATTEXTDIA_ATTRIBUTE__1, "Attribute Extract Template File" ));
	def= new char [csTemplate.GetLength()+1];
    strcpy(def,(LPCTSTR)csTemplate);

    if(sds_getfiled(title,def,ResourceString(IDC_ICADATTEXTDIA_TEXT_FILE__2, "Text File for Template|txt" ),flags,&rb)==RTNORM){
        csTemplate=rb.resval.rstring;
        UpdateData(FALSE);  //initialize data

        if(rb.resval.rstring!=NULL) IC_FREE(rb.resval.rstring);
		if(def!=NULL) IC_FREE(def);
    }
}

void ext_CAttExt::OnBrowse2() {
    struct resbuf rb;
    char title[40],*def,ext[IC_ACADBUF];
    int flags=5+2;	// EBATECH(CNBR) 2002/4/30 Disable TYPE button option+2

    UpdateData(true);
    if(iExtType==0) {
        strcpy(ext,"Drawing Exchange Format|dxx");
    }else {
        strcpy(ext,"CDF/SDF|txt");
	}
    strcpy(title,"Attribute Extract Output File");
	def= new char [csOutFile.GetLength()+1];
    strcpy(def,(LPCTSTR)csOutFile);

    if(sds_getfiled(title,def,ext,flags,&rb)==RTNORM){
        csOutFile=rb.resval.rstring;
        UpdateData(false);

        if(rb.resval.rstring!=NULL) IC_FREE(rb.resval.rstring);
		if(def!=NULL) IC_FREE(def);		        
    }
}


BOOL ext_CAttExt::OnInitDialog() {
    IcadDialog::OnInitDialog();
    if(iExtType==0){
        OnDxfClick();
    }else{
        OnDfClick();
    }
    if(lNumOfAtts==0L){
        ::EnableWindow(::GetDlgItem(m_hWnd,EXT_TEMPLATE),false);
        ::EnableWindow(::GetDlgItem(m_hWnd,EXT_BROWSE1),false);
        ::EnableWindow(::GetDlgItem(m_hWnd,EXT_DXF),false);
        ::EnableWindow(::GetDlgItem(m_hWnd,EXT_CDF),false);
        ::EnableWindow(::GetDlgItem(m_hWnd,EXT_SDF),false);
        ::EnableWindow(::GetDlgItem(m_hWnd,EXT_BROWSE2),false);
        ::EnableWindow(::GetDlgItem(m_hWnd,EXT_OUTPUT),false);
    }
    return(true);
}

void ext_CAttExt::DoDataExchange(CDataExchange *pDX) {
    DDX_Radio(pDX,EXT_DXF,iExtType);
    DDX_Text(pDX,EXT_TEMPLATE,csTemplate);
    DDX_Text(pDX,EXT_OUTPUT,csOutFile);
    DDX_Text(pDX,EXT_NUMBER,lNumOfAtts);
}

ext_CAttExt::ext_CAttExt():IcadDialog(IDD_ATTEXT) {
    iExtType=1;
}

void CMainWindow::IcadAttExtDia(void) {
    struct resbuf *entmask=NULL,setgetrb,rb,*pRbb=NULL;
    char cQuote,cDelim,outfilter[5];
    struct cmd_xtemplate *pXbeg=NULL,*pXtmp;
    sds_name ss={0L,0L},ename;
    FILE *outputfile=NULL;
    long i;
    int ret;
    dwg_entlink **elp=NULL;
	char *outputline=NULL;
	int reterror;

    ext_CAttExt m_CAttExt;
    m_CAttExt.lNumOfAtts=0L;
	if ((outputline=new char[8192])==NULL) goto bail;

	do{
        ret=m_CAttExt.DoModal();
        if(ret==IDOK){
            if(m_CAttExt.csOutFile.GetLength()==0) continue;
            if(m_CAttExt.iExtType==0){   //DXF
                //make a link list of entities		        
				elp= new dwg_entlink * [m_CAttExt.lNumOfAtts];
				memset( elp,0,m_CAttExt.lNumOfAtts * sizeof(dwg_entlink*) );
		        for(i=0L;sds_ssname(ss,i,ename)==RTNORM;i++){
			        elp[i]=(dwg_entlink*)ename[0];
		        }
                //get current prec
				SDS_getvar(NULL,DB_QLUPREC,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				SDS_getvar(NULL,DB_QMAKEBAK,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
                //dxf out
				ret=SDS_CMainWindow->m_pFileIO.drw_savefile(SDS_CURDWG,m_CAttExt.lNumOfAtts,elp,m_CAttExt.csOutFile.GetBuffer(IC_ACADBUF),IC_DXF,IC_ACAD2004,0,setgetrb.resval.rint,1,1,rb.resval.rint,TRUE);
                m_CAttExt.csOutFile.ReleaseBuffer();
                break;
            }else if(m_CAttExt.iExtType>0){  //CDF //SDF
                if(m_CAttExt.csTemplate.GetLength()==0) continue;
                if(m_CAttExt.iExtType==1){
                    strcpy(outfilter,ResourceString(IDC_ICADATTEXTDIA_CDF_7, "CDF" ));
                }else{
                    strcpy(outfilter,ResourceString(IDC_ICADATTEXTDIA_SDF_8, "SDF" ));
                }
                cQuote='\''; //set the default
                cDelim=',';  //set the default
                if((ret=cmd_templinklist((LPCTSTR)m_CAttExt.csTemplate,&cQuote,&cDelim,&pXbeg,&reterror))!=RTNORM) {
					if (reterror!=0) {
						CString cstr;
						cstr.LoadString(reterror);
						MessageBox((LPCTSTR)cstr,NULL,MB_OK);
					}
					goto bail;
				}
                //open output file in read/append mode
                if((outputfile = fopen((LPCTSTR)m_CAttExt.csOutFile,"w"/*DNT*/)) == NULL ) {
                    MessageBox(ResourceString(IDC_ICADATTEXTDIA_THE_FILE__10, "The file output file could not be opened." ),NULL,MB_OK);
                    continue;
                }

				if ((ret=dotheextract(ss,pXbeg,outputline,outputfile,cQuote,cDelim,outfilter))!=RTNORM) goto bail;

                break;
            }
        }else if(ret==EXT_SELECT){
            for(i=0;i<m_CAttExt.lNumOfAtts;i++) {
                sds_ssname(ss,i,ename);
                sds_redraw(ename,IC_REDRAW_UNHILITE);
            }
            sds_ssfree(ss);
            entmask=sds_buildlist(RTDXF0,"INSERT"/*DNT*/,0);
            sds_pmtssget(NULL,NULL,NULL,NULL,entmask,ss,0);
            sds_sslength(ss,&m_CAttExt.lNumOfAtts);
            IC_RELRB(entmask);
        }else if(ret==IDCANCEL){
            break;
        }
    }while(1);
bail:
    //free up space
    while (pXbeg!=NULL) {
        pXtmp=pXbeg->next;
        if(pXbeg->szFieldName) IC_FREE(pXbeg->szFieldName);
		IC_FREE(pXbeg);		
        pXbeg=pXtmp;
    }
	if (outputline!=NULL) delete [] outputline;
    if (outputfile!=NULL) {
        fclose(outputfile);
        outputfile=NULL;
    }
    IC_RELRB(pRbb);
    IC_RELRB(entmask);
    for (i=0;i<m_CAttExt.lNumOfAtts;i++) {
        sds_ssname(ss,i,ename);
        sds_redraw(ename,IC_REDRAW_UNHILITE);
    }
    sds_ssfree(ss);
}








