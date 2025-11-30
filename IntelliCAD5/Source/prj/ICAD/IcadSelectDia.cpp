/* F:\BLD\PRJ\ICAD\ICADSELECTDIA.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 *
 */

//** Includes
#include "Icad.h"/*DNT*/
#include "IcadApi.h"
#include "cmds.h"/*DNT*/
#include "paths.h"
#include "IcadHelp.h"


int sel_GetAll(sds_name ss,struct resbuf *entmask,int AndOrRemove,int InOutXing,long *lSelEnts);
int sel_GetCircle(sds_name ss,struct resbuf *entmask,int AndOrRemove,int InOutXing,long *lSelEnts);
int sel_GetFence(sds_name ss,struct resbuf *entmask,int AndOrRemove,int InOutXing,long *lSelEnts);
int sel_GetPoint(sds_name ss,struct resbuf *entmask,int AndOrRemove,int InOutXing,long *lSelEnts);
int sel_GetPolygon(sds_name ss,struct resbuf *entmask,int AndOrRemove,int InOutXing,long *lSelEnts);
int sel_GetSingle(sds_name ss,struct resbuf *entmask,int AndOrRemove,int InOutXing,long *lSelEnts);
int sel_GetWindow(sds_name ss,struct resbuf *entmask,int AndOrRemove,int InOutXing,long *lSelEnts);

//**************************************************************************************
//    Class for the location dialog
//**************************************************************************************
class sel_Loc : public CDialog {
    public:
        int iIOXing,
            iPoly,
            iBorder;
        BOOL bLocPick;
        sel_Loc();
        BOOL OnInitDialog();
    protected:
        afx_msg void OnOK();
        afx_msg void OnCancel();
        afx_msg void OnPick();
    	afx_msg void OnSelBorder(UINT nID);
        virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(sel_Loc, CDialog)
    ON_COMMAND(SEL_LOC_PICK,OnPick)
    ON_COMMAND_RANGE(SEL_ALL,SEL_WINDOW,OnSelBorder)
END_MESSAGE_MAP()

void sel_Loc::OnOK(){
    ::SendMessage(::GetParent(m_hWnd),WM_CLOSE,0,0);
}

void sel_Loc::OnCancel(){
    ::SendMessage(::GetParent(m_hWnd),WM_CLOSE,0,0);
}

void sel_Loc::DoDataExchange(CDataExchange *pDX) {
	DDX_Radio(pDX,SEL_ALL,iBorder);
	DDX_Radio(pDX,SEL_INSIDE,iIOXing);
	DDX_Radio(pDX,SEL_LINEGON,iPoly);
}

void sel_Loc::OnSelBorder(UINT nID) {
    if(nID==SEL_POLYLINE) {
        ::EnableWindow(::GetDlgItem(m_hWnd,SEL_LINEGON),TRUE);
        ::EnableWindow(::GetDlgItem(m_hWnd,SEL_LINEFENCE),TRUE);
        ::EnableWindow(::GetDlgItem(m_hWnd,SEL_BUFFERZONE),TRUE);
    }else{
        ::EnableWindow(::GetDlgItem(m_hWnd,SEL_LINEGON),FALSE);
        ::EnableWindow(::GetDlgItem(m_hWnd,SEL_LINEFENCE),FALSE);
        ::EnableWindow(::GetDlgItem(m_hWnd,SEL_BUFFERZONE),FALSE);
    }
    if((nID==SEL_CIRCLE)||(nID==SEL_POLYGON)||(nID==SEL_WINDOW)) {
        ::EnableWindow(::GetDlgItem(m_hWnd,SEL_INSIDE),TRUE);
        ::EnableWindow(::GetDlgItem(m_hWnd,SEL_OUTSIDE),TRUE);
        ::EnableWindow(::GetDlgItem(m_hWnd,SEL_CROSSING),TRUE);
    }else{
        ::EnableWindow(::GetDlgItem(m_hWnd,SEL_INSIDE),FALSE);
        ::EnableWindow(::GetDlgItem(m_hWnd,SEL_OUTSIDE),FALSE);
        ::EnableWindow(::GetDlgItem(m_hWnd,SEL_CROSSING),FALSE);
    }
}

BOOL sel_Loc::OnInitDialog() {
    BOOL ret=CDialog::OnInitDialog();
    sel_Loc::OnSelBorder(0);
    return(ret);
}

void sel_Loc::OnPick() {
    HWND hWnd;
    bLocPick=1;
    hWnd=::GetParent(m_hWnd);
    ::SendMessage(hWnd,WM_CLOSE,0,0);
}

sel_Loc::sel_Loc():CDialog(SEL_LOCATION) {
    bLocPick=0;
    iBorder=0;
    iIOXing=0;
    iPoly=0;
}

//**************************************************************************************
//    Class for the properties tab
//**************************************************************************************
class sel_Prop : public CDialog {
    public:
        int iSelProp;
        BOOL bPropPick;
        sel_Prop();
    protected:
        afx_msg void OnOK();
        afx_msg void OnCancel();
        afx_msg void OnPick();
        virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(sel_Prop, CDialog)
    ON_COMMAND(SEL_PROP_PICK,OnPick)
END_MESSAGE_MAP()

void sel_Prop::OnOK(){
    ::SendMessage(::GetParent(m_hWnd),WM_CLOSE,0,0);
}

void sel_Prop::OnCancel(){
    ::SendMessage(::GetParent(m_hWnd),WM_CLOSE,0,0);
}

void sel_Prop::DoDataExchange(CDataExchange *pDX) {
	DDX_Radio(pDX,SEL_AREA,iSelProp);
}

void sel_Prop::OnPick() {
    HWND hWnd;
    bPropPick=1;
    hWnd=::GetParent(m_hWnd);
    ::SendMessage(hWnd,WM_CLOSE,0,0);
}

sel_Prop::sel_Prop():CDialog(SEL_PROPERTIES) {
    bPropPick=0;
    iSelProp=0;
}


//**************************************************************************************
//    Class for the sql dialog
//**************************************************************************************
class sel_Sql : public CDialog {
    public:
        sel_Sql();
    protected:

};

sel_Sql::sel_Sql():CDialog(SEL_SQL) {
}

//**************************************************************************************
//    Class for the sql dialog
//**************************************************************************************
class sel_Att : public CDialog {
    public:
        sel_Att();
    protected:

};

sel_Att::sel_Att():CDialog(SEL_ATTRIBUTE) {
}

//**************************************************************************************
//    Class for the sql dialog
//**************************************************************************************
class sel_Eed : public CDialog {
    public:
        sel_Eed();
    protected:

};

sel_Eed::sel_Eed():CDialog(SEL_EED) {
}

//**************************************************************************************
//    Class for the main dialog
//**************************************************************************************
class sel_SelDia : public CDialog {
    public:
        int         m_CurSel,
                    iAndOr;
        long        lSelEnts,
                    lAddEnts,
                    lTotalEnts;
        CStringArray csArray;
        sel_Loc     m_SelLoc;
        sel_Prop    m_SelProp;
        sel_Sql     m_SelSql;
        sel_Att     m_SelAtt;
        sel_Eed     m_SelEed;
        CTabCtrl    m_TabCtrl;
        CListBox    m_ListBox;

        sel_SelDia();
        BOOL OnInitDialog();
        void sel_Rem();
    protected:
        afx_msg void OnHelp();
      	afx_msg void OnDestroy();
        afx_msg void OnTabChange( NMHDR * pNotifyStruct, LRESULT * result );
    	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(sel_SelDia, CDialog)
    ON_NOTIFY(TCN_SELCHANGE, SEL_TAB, OnTabChange)
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDHELP,OnHelp)
END_MESSAGE_MAP()


void sel_SelDia::OnHelp() {
    char fs1[IC_ACADBUF];

    if( !SearchHelpFile( SDS_COMMANDHELP, fs1) ) {
		MessageBox(ResourceString(IDC_ICADMLEDITDIA_UNABLE_TO__0, "Unable to find the requested help file." ),NULL,MB_OK|MB_ICONEXCLAMATION);
        return;
    }
	CMainWindow::IcadHelp( fs1,ICADHELP_CONTENTS,0);
}

void sel_SelDia::DoDataExchange(CDataExchange *pDX) {
	DDX_Control(pDX,SEL_TAB,m_TabCtrl);
    DDX_Radio(pDX,SEL_AND,iAndOr);
    DDX_Control(pDX,SEL_LIST,m_ListBox);
}

void sel_SelDia::OnDestroy() {
    CDialog::OnDestroy();
    UpdateData(TRUE);
    if(m_SelLoc)   m_SelLoc.UpdateData(TRUE);
    if(m_SelProp)  m_SelProp.UpdateData(TRUE);
}

void sel_SelDia::sel_Rem() {
    if(lTotalEnts) {
        ::EnableWindow(::GetDlgItem(m_hWnd,SEL_AND),TRUE);
        ::EnableWindow(::GetDlgItem(m_hWnd,SEL_REMOVE),TRUE);
    }else{
        ::EnableWindow(::GetDlgItem(m_hWnd,SEL_AND),FALSE);
        ::EnableWindow(::GetDlgItem(m_hWnd,SEL_REMOVE),FALSE);
        iAndOr=1;
    }
}

//This is for a tab change
void sel_SelDia::OnTabChange(NMHDR *pNotifyStruct,LRESULT *result) {

    //Disable
    switch(m_CurSel) {
        case 0:
            m_SelLoc.EnableWindow(FALSE);
            m_SelLoc.ShowWindow(SW_HIDE);
            break;
        case 1:
            m_SelProp.EnableWindow(FALSE);
            m_SelProp.ShowWindow(SW_HIDE);
            break;
/**Next Version
        case 2:
            m_SelSql.EnableWindow(FALSE);
            m_SelSql.ShowWindow(SW_HIDE);
            break;
        case 3:
            m_SelAtt.EnableWindow(FALSE);
            m_SelAtt.ShowWindow(SW_HIDE);
            break;
        case 4:
            m_SelEed.EnableWindow(FALSE);
            m_SelEed.ShowWindow(SW_HIDE);
            break;
*/
    }

    //Enable
    m_CurSel=m_TabCtrl.GetCurSel(); //Get the current selection
    switch(m_CurSel) {
        case 0:
            if(!m_SelLoc) m_SelLoc.Create(SEL_LOCATION,this);
            m_SelLoc.EnableWindow(TRUE);
            m_SelLoc.ShowWindow(SW_SHOWNORMAL);
            break;
        case 1:
            if(!m_SelProp) m_SelProp.Create(SEL_PROPERTIES,this);
            m_SelProp.EnableWindow(TRUE);
            m_SelProp.ShowWindow(SW_SHOWNORMAL);
            break;
/**Next Version
        case 2:
            if(!m_SelSql) m_SelSql.Create(SEL_SQL,this);
            m_SelSql.EnableWindow(TRUE);
            m_SelSql.ShowWindow(SW_SHOWNORMAL);
            break;
        case 3:
            if(!m_SelAtt) m_SelAtt.Create(SEL_ATTRIBUTE,this);
            m_SelAtt.EnableWindow(TRUE);
            m_SelAtt.ShowWindow(SW_SHOWNORMAL);
            break;
        case 4:
            if(!m_SelEed) m_SelEed.Create(SEL_EED,this);
            m_SelEed.EnableWindow(TRUE);
            m_SelEed.ShowWindow(SW_SHOWNORMAL);
            break;
*/
    }
}


BOOL sel_SelDia::OnInitDialog() {
    TC_ITEM TabCtrlItem;
    BOOL ret;

    sel_Rem();
    ret=CDialog::OnInitDialog();

    TabCtrlItem.mask=TCIF_TEXT;
	EditString	itemText;
	itemText = ResourceString(IDC_ICADSELECTDIA_LOCATION_1, "Location" );
    TabCtrlItem.pszText=itemText;
    m_TabCtrl.InsertItem(0,&TabCtrlItem);
	itemText = ResourceString(IDC_ICADSELECTDIA_PROPERTIES_2, "Properties" );
    TabCtrlItem.pszText=itemText;
    m_TabCtrl.InsertItem(1,&TabCtrlItem);
/**Next Version
        TabCtrlItem.pszText=ResourceString(IDC_ICADSELECTDIA_LINK_SQL_3, "Link/SQL" );
        m_TabCtrl.InsertItem(2,&TabCtrlItem);
        TabCtrlItem.pszText=ResourceString(IDC_ICADSELECTDIA_ATTRIBUTE_4, "Attribute" );
        m_TabCtrl.InsertItem(3,&TabCtrlItem);
        TabCtrlItem.pszText=ResourceString(IDC_ICADSELECTDIA_EED_5, "EED" );
        m_TabCtrl.InsertItem(4,&TabCtrlItem);
*/
    switch(m_CurSel) {
        case 0:
            if(!m_SelLoc) m_SelLoc.Create(SEL_LOCATION,this);
            m_SelLoc.EnableWindow(TRUE);
            m_SelLoc.ShowWindow(SW_SHOWNORMAL);
            m_TabCtrl.SetCurSel(0);
            break;
        case 1:
            if(!m_SelProp) m_SelProp.Create(SEL_PROPERTIES,this);
            m_SelProp.EnableWindow(TRUE);
            m_SelProp.ShowWindow(SW_SHOWNORMAL);
            m_TabCtrl.SetCurSel(1);
            break;
/**Next Version
        case 2:
            if(!m_SelSql) m_SelSql.Create(SEL_SQL,this);
            m_SelSql.EnableWindow(TRUE);
            m_SelSql.ShowWindow(SW_SHOWNORMAL);
            m_TabCtrl.SetCurSel(2);
            break;
        case 3:
           if(!m_SelAtt) m_SelAtt.Create(SEL_ATTRIBUTE,this);
            m_SelAtt.EnableWindow(TRUE);
            m_SelAtt.ShowWindow(SW_SHOWNORMAL);
            m_TabCtrl.SetCurSel(3);
            break;
        case 4:
            if(!m_SelEed) m_SelEed.Create(SEL_EED,this);
            m_SelEed.EnableWindow(TRUE);
            m_SelEed.ShowWindow(SW_SHOWNORMAL);
            m_TabCtrl.SetCurSel(4);
            break;
*/
    }
    for(long l=0;l<=csArray.GetUpperBound();l++) {
        m_ListBox.InsertString(-1,csArray.GetAt(l));
    }
    m_ListBox.SetCaretIndex(l,FALSE);
    return(ret);
}


sel_SelDia::sel_SelDia():CDialog(SEL_SELECT) {
    m_CurSel=0;
    iAndOr=1;
    lTotalEnts=0;
}

//***This is the main window function
void CMainWindow::IcadSelDia(sds_name ss) {
    int ret;
    long ltmp;
    CString csList;
    char svAdd[IC_ACADBUF],svSel[IC_ACADBUF],svTot[IC_ACADBUF];
    struct resbuf *entmask=NULL;

    sel_SelDia m_Select;
    sds_ssadd(NULL,NULL,ss);
    do {
        sds_sslength(ss,&ltmp);
        ret=m_Select.DoModal();
        m_Select.lSelEnts=m_Select.lAddEnts=0;
        if((ret==IDCANCEL) && (m_Select.m_SelLoc.bLocPick==1)) {
            m_Select.m_SelLoc.bLocPick=0;
            switch(m_Select.m_SelLoc.iBorder) {
                case 0: //ALL
                    sel_GetAll(ss,entmask,m_Select.iAndOr,m_Select.m_SelLoc.iIOXing,&m_Select.lSelEnts);
                    break;
                case 1: //CIRCLE
                    sel_GetCircle(ss,entmask,m_Select.iAndOr,m_Select.m_SelLoc.iIOXing,&m_Select.lSelEnts);
                    break;
                case 2: //FENCE
                    sel_GetFence(ss,entmask,m_Select.iAndOr,m_Select.m_SelLoc.iIOXing,&m_Select.lSelEnts);
                    break;
                case 3: //POINT
                    sel_GetPoint(ss,entmask,m_Select.iAndOr,m_Select.m_SelLoc.iIOXing,&m_Select.lSelEnts);
                    break;
                case 4: //POLYGON
                    sel_GetPolygon(ss,entmask,m_Select.iAndOr,m_Select.m_SelLoc.iIOXing,&m_Select.lSelEnts);
                    break;
                case 5: //POLYLINE
                    break;
                case 6: //SINGLE
                    sel_GetSingle(ss,entmask,m_Select.iAndOr,m_Select.m_SelLoc.iIOXing,&m_Select.lSelEnts);
                    break;
                case 7: //WINDOW
                    sel_GetWindow(ss,entmask,m_Select.iAndOr,m_Select.m_SelLoc.iIOXing,&m_Select.lSelEnts);
                    break;
            }//switch
            sds_sslength(ss,&m_Select.lTotalEnts);
            m_Select.lAddEnts=m_Select.lTotalEnts-ltmp;
            csList=ResourceString(IDC_ICADSELECTDIA_ENTITIES_S_6, "Entities Selected=" );
            ltoa(m_Select.lSelEnts,svSel,10);
            csList+=svSel;
            if(m_Select.lAddEnts>0) {
                csList+=ResourceString(IDC_ICADSELECTDIA__ENTITIES__7, " Entities added=" );
                ltoa(m_Select.lAddEnts,svAdd,10);
                csList+=svAdd;
            }else if(m_Select.lAddEnts<0) {
                csList+=ResourceString(IDC_ICADSELECTDIA__ENTITIES__8, " Entities removed=" );
                ltoa(-m_Select.lAddEnts,svAdd,10);
                csList+=svAdd;
            }else{
                csList+=ResourceString(IDC_ICADSELECTDIA__NO_ENTITI_9, " No entities added or removed" );
            }
            csList+=ResourceString(IDC_ICADSELECTDIA__TOTAL_EN_10, " Total entities=" );
            ltoa(m_Select.lTotalEnts,svTot,10);
            csList+=svTot;
            m_Select.csArray.Add(csList);

        }else if((ret==IDCANCEL) && (m_Select.m_SelProp.bPropPick==1)) {
            m_Select.m_SelProp.bPropPick=0;
            switch(m_Select.m_SelProp.iSelProp) {
                case 0:
                    break;
                case 1:
                    break;
                case 2:
                    break;
                case 3:
                    break;
                case 4:
                    break;
                case 5:
                    break;
                case 6:
                    break;
                case 7:
                    break;
            }
        }else{
            break;
        }
    }while(1);

}


int sel_GetAll(sds_name ss,struct resbuf *entmask,int AndOrRemove,int InOutXing,long *lSelEnts) {
    sds_name ename={NULL,NULL},tmp_ss={NULL,NULL};
    int ret;


    SDS_PickObjects(ResourceString(IDC_ICADSELECTDIA__A_11, "_A" ),NULL,NULL,entmask,tmp_ss,0,SDS_CURDWG);
    sds_sslength(tmp_ss,lSelEnts);
    switch(AndOrRemove) {
        case 0: //AND
            break;
        case 1: //OR
            while(RTNORM==sds_ssname(tmp_ss,0,ename)) {
                if((ret=sds_ssadd(ename,ss,ss))!=RTNORM) {goto bail;}
                sds_ssdel(ename,tmp_ss);
                SDS_RedrawEntity(ename,IC_REDRAW_HILITE,NULL,NULL,1);
            }
            break;
        case 2: //REMOVE
            while(RTNORM==sds_ssname(tmp_ss,0,ename)) {
                sds_ssdel(ename,ss);
                sds_ssdel(ename,tmp_ss);
                SDS_RedrawEntity(ename,IC_REDRAW_UNHILITE,NULL,NULL,1);
            }
            break;
    }

    ret=RTNORM;
    bail:
    if(tmp_ss) sds_ssfree(tmp_ss);
    IC_RELRB(entmask);
    return(ret);
}

int sel_GetCircle(sds_name ss,struct resbuf *entmask,int AndOrRemove,int InOutXing,long *lSelEnts){
    sds_point pt1,pt2;
    int ret;
    double rad;
    long lcount;
    struct resbuf rb,*rbb=NULL;
    sds_name ename={NULL,NULL},tmp_ss={NULL,NULL};

    if((ret=internalGetpoint(NULL,ResourceString(IDC_ICADSELECTDIA__NCENTER__12, "\nCenter of circle: " ),pt1))!=RTNORM) goto bail;
	SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
    SDS_SetCursor(IDC_ICAD_WINDOW);
    if((ret=SDS_dragobject(1,rb.resval.rint,pt1,NULL,0L,ResourceString(IDC_ICADSELECTDIA__NRADIUS__13, "\nRadius of circle: " ),&rbb,pt2,NULL))!=RTNORM) goto bail;
	ic_assoc(rbb,40);
	rad=ic_rbassoc->resval.rreal;
	sds_relrb(rbb);
	rbb=sds_buildlist(RT3DPOINT,pt1,RTREAL,rad,0);
    SDS_SetCursor(IDC_ICAD_CROSS);
    switch(InOutXing) {
        case 0://inside
            SDS_PickObjects(ResourceString(IDC_ICADSELECTDIA__WC_14, "_WC" ),rbb,NULL,entmask,tmp_ss,0,SDS_CURDWG);
            break;
        case 1://outside
            SDS_PickObjects(ResourceString(IDC_ICADSELECTDIA__OC_15, "_OC" ),rbb,NULL,entmask,tmp_ss,0,SDS_CURDWG);
            break;
        case 2://xing
            SDS_PickObjects(ResourceString(IDC_ICADSELECTDIA__CC_16, "_CC" ),rbb,NULL,entmask,tmp_ss,0,SDS_CURDWG);
           break;
    }
    if(rbb) {sds_relrb(rbb); rbb=NULL;}
    sds_sslength(tmp_ss,lSelEnts);

    switch(AndOrRemove) {
        case 0: //AND
            for(lcount=0;RTNORM==sds_ssname(ss,lcount,ename);lcount++) {
                if(sds_ssmemb(ename,tmp_ss)==RTERROR) {
                    if((ret=sds_ssdel(ename,ss))!=RTNORM) {goto bail;}
                    SDS_RedrawEntity(ename,IC_REDRAW_UNHILITE,NULL,NULL,1);
                    lcount--;
                }
            }
            if(tmp_ss) {
                sds_ssfree(tmp_ss);
                tmp_ss[0]=tmp_ss[1]=NULL;
            }
            break;
        case 1: //OR
            while(RTNORM==sds_ssname(tmp_ss,0,ename)) {
                if((ret=sds_ssadd(ename,ss,ss))!=RTNORM) {goto bail;}
                sds_ssdel(ename,tmp_ss);
                SDS_RedrawEntity(ename,IC_REDRAW_HILITE,NULL,NULL,1);
            }
            break;
        case 2: //REMOVE
            while(RTNORM==sds_ssname(tmp_ss,0,ename)) {
                sds_ssdel(ename,ss);
                sds_ssdel(ename,tmp_ss);
                SDS_RedrawEntity(ename,IC_REDRAW_UNHILITE,NULL,NULL,1);
            }
            break;
    }

    ret=RTNORM;
    bail:
    if(tmp_ss) sds_ssfree(tmp_ss);
    if(rbb) {sds_relrb(rbb); rbb=NULL;}
    IC_RELRB(entmask);
    return(ret);
}

int sel_GetFence(sds_name ss,struct resbuf *entmask,int AndOrRemove,int InOutXing,long *lSelEnts){
    sds_point pt1,pt2;
    int ret;
    long lcount;
    struct resbuf *rbb=NULL,*rbc=NULL;
    sds_name ename={NULL,NULL},tmp_ss={NULL,NULL};

    if((ret=internalGetpoint(NULL,ResourceString(IDC_ICADSELECTDIA__NFIRST_P_17, "\nFirst point of fence: " ),pt1))!=RTNORM) goto bail;
    rbb=rbc=sds_buildlist(RTPOINT,pt1,0);
    SDS_SetCursor(IDC_ICAD_CROSSING);
    while(RTNORM==internalGetpoint(pt1,ResourceString(IDC_ICADSELECTDIA__NNEXT_PO_18, "\nNext point: " ),pt2)) {
        rbc=rbc->rbnext=sds_buildlist(RTPOINT,pt2,0);
        sds_grdraw(pt1,pt2,-1,0);
        ic_ptcpy(pt1,pt2);
	    SDS_SetCursor(IDC_ICAD_CROSSING);
    }
    SDS_SetCursor(IDC_ICAD_CROSS);
    SDS_PickObjects(ResourceString(IDC_ICADSELECTDIA__F_19, "_F" ),rbb,NULL,entmask,tmp_ss,0,SDS_CURDWG);
    ic_ptcpy(pt1,rbb->resval.rpoint);
    for(rbc=rbb->rbnext; rbc!=NULL; rbc=rbc->rbnext) {
        sds_grdraw(pt1,rbc->resval.rpoint,-1,0);
        ic_ptcpy(pt1,rbc->resval.rpoint);
    }
    if(rbb) {sds_relrb(rbb); rbb=NULL;}
    sds_sslength(tmp_ss,lSelEnts);

    switch(AndOrRemove) {
        case 0: //AND
            for(lcount=0;RTNORM==sds_ssname(ss,lcount,ename);lcount++) {
                if(sds_ssmemb(ename,tmp_ss)==RTERROR) {
                    if((ret=sds_ssdel(ename,ss))!=RTNORM) {goto bail;}
                    SDS_RedrawEntity(ename,IC_REDRAW_UNHILITE,NULL,NULL,1);
                    lcount--;
                }
            }
            if(tmp_ss) {
                sds_ssfree(tmp_ss);
                tmp_ss[0]=tmp_ss[1]=NULL;
            }
            break;
        case 1: //OR
            while(RTNORM==sds_ssname(tmp_ss,0,ename)) {
                if((ret=sds_ssadd(ename,ss,ss))!=RTNORM) {goto bail;}
                sds_ssdel(ename,tmp_ss);
                SDS_RedrawEntity(ename,IC_REDRAW_HILITE,NULL,NULL,1);
            }
            break;
        case 2: //REMOVE
            while(RTNORM==sds_ssname(tmp_ss,0,ename)) {
                sds_ssdel(ename,ss);
                sds_ssdel(ename,tmp_ss);
                SDS_RedrawEntity(ename,IC_REDRAW_UNHILITE,NULL,NULL,1);
            }
            break;
    }

    ret=RTNORM;
bail:
    if(tmp_ss) sds_ssfree(tmp_ss);
    if(rbb) {sds_relrb(rbb); rbb=NULL;}
    IC_RELRB(entmask);
    return(ret);


}

int sel_GetPoint(sds_name ss,struct resbuf *entmask,int AndOrRemove,int InOutXing,long *lSelEnts){
    sds_name ename={NULL,NULL},tmp_ss={NULL,NULL};
    sds_point pt1;
    long lcount;
    int ret;

    if((ret=internalGetpoint(NULL,ResourceString(IDC_ICADSELECTDIA__NPOINT_T_20, "\nPoint to locate entity around: " ),pt1))!=RTNORM) goto bail;
    SDS_PickObjects(ResourceString(IDC_ICADSELECTDIA__PO_21, "_PO" ),pt1,NULL,entmask,tmp_ss,0,SDS_CURDWG);

    sds_sslength(tmp_ss,lSelEnts);
    switch(AndOrRemove) {
        case 0: //AND
            for(lcount=0;RTNORM==sds_ssname(ss,lcount,ename);lcount++) {
                if(sds_ssmemb(ename,tmp_ss)==RTERROR) {
                    if((ret=sds_ssdel(ename,ss))!=RTNORM) {goto bail;}
                    SDS_RedrawEntity(ename,IC_REDRAW_UNHILITE,NULL,NULL,1);
                    lcount--;
                }
            }
            if(tmp_ss) {
                sds_ssfree(tmp_ss);
                tmp_ss[0]=tmp_ss[1]=NULL;
            }
            break;
        case 1: //OR
            while(RTNORM==sds_ssname(tmp_ss,0,ename)) {
                if((ret=sds_ssadd(ename,ss,ss))!=RTNORM) {goto bail;}
                sds_ssdel(ename,tmp_ss);
                SDS_RedrawEntity(ename,IC_REDRAW_HILITE,NULL,NULL,1);
            }
            break;
        case 2: //REMOVE
            while(RTNORM==sds_ssname(tmp_ss,0,ename)) {
                sds_ssdel(ename,ss);
                sds_ssdel(ename,tmp_ss);
                SDS_RedrawEntity(ename,IC_REDRAW_UNHILITE,NULL,NULL,1);
            }
            break;
    }

    ret=RTNORM;
    bail:
    if(tmp_ss) sds_ssfree(tmp_ss);
    IC_RELRB(entmask);
    return(ret);

}

int sel_GetPolygon(sds_name ss,struct resbuf *entmask,int AndOrRemove,int InOutXing,long *lSelEnts){
    sds_point pt,pt1,pt2;
    int ret,ptct;
    long lcount;
    struct resbuf rb,*rbb=NULL,*rbc=NULL;
    sds_name ename={NULL,NULL},tmp_ss={NULL,NULL};

    switch(InOutXing) {
        case 0://inside
            if((ret=internalGetpoint(NULL,ResourceString(IDC_ICADSELECTDIA__NFIRST_V_22, "\nFirst vertex of inclusive polygon: " ),pt))!=RTNORM) goto bail;
            SDS_SetCursor(IDC_ICAD_WINDOW);
            break;
        case 1://outside
            if((ret=internalGetpoint(NULL,ResourceString(IDC_ICADSELECTDIA__NFIRST_V_23, "\nFirst vertex of outside polygon: " ),pt))!=RTNORM) goto bail;
            SDS_SetCursor(IDC_ICAD_OUTSIDE);
            break;
        case 2://xing
            if((ret=internalGetpoint(NULL,ResourceString(IDC_ICADSELECTDIA__NFIRST_V_24, "\nFirst vertex of crossing polygon: " ),pt))!=RTNORM) goto bail;
            SDS_SetCursor(IDC_ICAD_CROSSING);
    }
    rbb=rbc=sds_buildlist(RTPOINT,pt,0);
	ic_ptcpy(pt1,pt);
	SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	ptct=1;
    while(RTNORM==
		(ptct>1 ? SDS_dragobject(35,rb.resval.rint,pt1,pt,0.0,ResourceString(IDC_ICADSELECTDIA__NNEXT_VE_25, "\nNext vertex: " ),NULL,pt2,NULL)
		: internalGetpoint(pt1,ResourceString(IDC_ICADSELECTDIA__NNEXT_VE_25, "\nNext vertex: " ),pt2))) {
		++ptct;
        rbc=rbc->rbnext=sds_buildlist(RTPOINT,pt2,0);
        sds_grdraw(pt1,pt2,-1,0);
        ic_ptcpy(pt1,pt2);
	    SDS_SetCursor(IDC_ICAD_WINDOW);
    }
    SDS_SetCursor(IDC_ICAD_CROSS);
    switch(InOutXing) {
        case 0://inside
            SDS_PickObjects(ResourceString(IDC_ICADSELECTDIA__WP_26, "_WP" ),rbb,NULL,entmask,tmp_ss,0,SDS_CURDWG);
            ic_ptcpy(pt1,rbb->resval.rpoint);
            for(rbc=rbb->rbnext; rbc!=NULL; rbc=rbc->rbnext) {
                sds_grdraw(pt1,rbc->resval.rpoint,-1,0);
                ic_ptcpy(pt1,rbc->resval.rpoint);
            }
            break;
        case 1://outside
            SDS_PickObjects(ResourceString(IDC_ICADSELECTDIA__OP_27, "_OP" ),rbb,NULL,entmask,tmp_ss,0,SDS_CURDWG);
            ic_ptcpy(pt1,rbb->resval.rpoint);
            for(rbc=rbb->rbnext; rbc!=NULL; rbc=rbc->rbnext) {
                sds_grdraw(pt1,rbc->resval.rpoint,-1,0);
                ic_ptcpy(pt1,rbc->resval.rpoint);
            }
            break;
        case 2://xing
            SDS_PickObjects(ResourceString(IDC_ICADSELECTDIA__CP_28, "_CP" ),rbb,NULL,entmask,tmp_ss,0,SDS_CURDWG);
            ic_ptcpy(pt1,rbb->resval.rpoint);
            for(rbc=rbb->rbnext; rbc!=NULL; rbc=rbc->rbnext) {
                sds_grdraw(pt1,rbc->resval.rpoint,-1,0);
                ic_ptcpy(pt1,rbc->resval.rpoint);
            }
           break;
    }
    if(rbb) {sds_relrb(rbb); rbb=NULL;}
    sds_sslength(tmp_ss,lSelEnts);

    switch(AndOrRemove) {
        case 0: //AND
            for(lcount=0;RTNORM==sds_ssname(ss,lcount,ename);lcount++) {
                if(sds_ssmemb(ename,tmp_ss)==RTERROR) {
                    if((ret=sds_ssdel(ename,ss))!=RTNORM) {goto bail;}
                    SDS_RedrawEntity(ename,IC_REDRAW_UNHILITE,NULL,NULL,1);
                    lcount--;
                }
            }
            if(tmp_ss) {
                sds_ssfree(tmp_ss);
                tmp_ss[0]=tmp_ss[1]=NULL;
            }
            break;
        case 1: //OR
            while(RTNORM==sds_ssname(tmp_ss,0,ename)) {
                if((ret=sds_ssadd(ename,ss,ss))!=RTNORM) {goto bail;}
                sds_ssdel(ename,tmp_ss);
                SDS_RedrawEntity(ename,IC_REDRAW_HILITE,NULL,NULL,1);
            }
            break;
        case 2: //REMOVE
            while(RTNORM==sds_ssname(tmp_ss,0,ename)) {
                sds_ssdel(ename,ss);
                sds_ssdel(ename,tmp_ss);
                SDS_RedrawEntity(ename,IC_REDRAW_UNHILITE,NULL,NULL,1);
            }
            break;
    }

    ret=RTNORM;
    bail:
    if(tmp_ss) sds_ssfree(tmp_ss);
    if(rbb) {sds_relrb(rbb); rbb=NULL;}
    IC_RELRB(entmask);
    return(ret);
}

int sel_GetSingle(sds_name ss,struct resbuf *entmask,int AndOrRemove,int InOutXing,long *lSelEnts) {
    sds_point pt1;
    int ret;
    sds_name ename={NULL,NULL},tmp_ss={NULL,NULL};

    sds_entsel(ResourceString(IDC_ICADSELECTDIA__NSELECT__29, "\nSelect single entity: " ),ename,pt1);
    switch(AndOrRemove) {
        case 0: //AND
            if(sds_ssmemb(ename,ss)==RTERROR) {
                while(sds_ssname(ss,0,ename)==RTNORM) {
                    if((ret=sds_ssdel(ename,ss))!=RTNORM) {goto bail;}
                    (*lSelEnts)=1;
                    SDS_RedrawEntity(ename,IC_REDRAW_UNHILITE,NULL,NULL,1);
                }
            }
            break;
        case 1: //OR
            if((ret=sds_ssadd(ename,ss,ss))!=RTNORM) {goto bail;}
            (*lSelEnts)=1;
            SDS_RedrawEntity(ename,IC_REDRAW_HILITE,NULL,NULL,1);
            break;
        case 2: //REMOVE
            if(sds_ssmemb(ename,ss)==RTNORM) {
                if((ret=sds_ssdel(ename,ss))!=RTNORM) {goto bail;}
                (*lSelEnts)=1;
                SDS_RedrawEntity(ename,IC_REDRAW_UNHILITE,NULL,NULL,1);
            }
            break;
    }

    ret=RTNORM;
    bail:
    if(tmp_ss) sds_ssfree(tmp_ss);
    IC_RELRB(entmask);
    return(ret);
}

int sel_GetWindow(sds_name ss,struct resbuf *entmask,int AndOrRemove,int InOutXing,long *lSelEnts) {
    sds_point pt1,pt2;
    int ret;
    long lcount;
    sds_name ename={NULL,NULL},tmp_ss={NULL,NULL};

    switch(InOutXing) {
        case 0://inside
            if((ret=internalGetpoint(NULL,ResourceString(IDC_ICADSELECTDIA__NFIRST_C_30, "\nFirst corner of inclusive window: " ),pt1))!=RTNORM) goto bail;
            SDS_SetCursor(IDC_ICAD_WINDOW);
			if((ret=sds_getcorner(pt1,ResourceString(IDC_ICADSELECTDIA__NOPPOSIT_31, "\nOpposite corner: " ),pt2))!=RTNORM) goto bail;
            SDS_SetCursor(IDC_ICAD_CROSS);
			SDS_PickObjects(ResourceString(IDC_ICADSELECTDIA__W_32, "_W" ),pt1,pt2,entmask,tmp_ss,0,SDS_CURDWG);
            break;
        case 1://outside
            if((ret=internalGetpoint(NULL,ResourceString(IDC_ICADSELECTDIA__NFIRST_C_33, "\nFirst corner of exclusive window: " ),pt1))!=RTNORM) goto bail;
            SDS_SetCursor(IDC_ICAD_OUTSIDE);
			if((ret=sds_getcorner(pt1,ResourceString(IDC_ICADSELECTDIA__NOPPOSIT_31, "\nOpposite corner: " ),pt2))!=RTNORM) goto bail;
            SDS_SetCursor(IDC_ICAD_CROSS);
            SDS_PickObjects(ResourceString(IDC_ICADSELECTDIA__O_34, "_O" ),pt1,pt2,entmask,tmp_ss,0,SDS_CURDWG);
            break;
        case 2://xing
            if((ret=internalGetpoint(NULL,ResourceString(IDC_ICADSELECTDIA__NFIRST_C_35, "\nFirst corner of crossing window: " ),pt1))!=RTNORM) goto bail;
            SDS_SetCursor(IDC_ICAD_CROSSING);
			if((ret=sds_getcorner(pt1,ResourceString(IDC_ICADSELECTDIA__NOPPOSIT_31, "\nOpposite corner: " ),pt2))!=RTNORM) goto bail;
            SDS_SetCursor(IDC_ICAD_CROSS);
            SDS_PickObjects(ResourceString(IDC_ICADSELECTDIA__C_36, "_C" ),pt1,pt2,entmask,tmp_ss,0,SDS_CURDWG);
           break;
    }
    sds_sslength(tmp_ss,lSelEnts);

    switch(AndOrRemove) {
        case 0: //AND
            for(lcount=0;RTNORM==sds_ssname(ss,lcount,ename);lcount++) {
                if(sds_ssmemb(ename,tmp_ss)==RTERROR) {
                    if((ret=sds_ssdel(ename,ss))!=RTNORM) {goto bail;}
                    SDS_RedrawEntity(ename,IC_REDRAW_UNHILITE,NULL,NULL,1);
                    lcount--;
                }
            }
            if(tmp_ss) {
                sds_ssfree(tmp_ss);
                tmp_ss[0]=tmp_ss[1]=NULL;
            }
            break;
        case 1: //OR
            while(RTNORM==sds_ssname(tmp_ss,0,ename)) {
                if((ret=sds_ssadd(ename,ss,ss))!=RTNORM) {goto bail;}
                sds_ssdel(ename,tmp_ss);
                SDS_RedrawEntity(ename,IC_REDRAW_HILITE,NULL,NULL,1);
            }
            break;
        case 2: //REMOVE
            while(RTNORM==sds_ssname(tmp_ss,0,ename)) {
                sds_ssdel(ename,ss);
                sds_ssdel(ename,tmp_ss);
                SDS_RedrawEntity(ename,IC_REDRAW_UNHILITE,NULL,NULL,1);
            }
            break;
    }

    ret=RTNORM;
    bail:
    if(tmp_ss) sds_ssfree(tmp_ss);
    IC_RELRB(entmask);
    return(ret);
}


