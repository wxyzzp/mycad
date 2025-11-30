/* C:\DEV\LIB\CMDS\DDEDIT.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * DDEDIT command
 * 
 */ 
// THIS FILE HAS BEEN GLOBALIZED!
//** Includes
#include "cmds.h"/*DNT*/
#include "Icad.h"/*DNT*/
#include "IcadDoc.h"
#include "db.h"/*DNT*/
#include "IcadApi.h"

//** Global Vars


//** Prototypes
short cmd_get_blk_tag_val(char **pBlock,char **pTag,char **pValue);
short cmd_get_str(char *pTxt);
short cmd_edit_global_att(bool bMode);
short cmd_get_new_str(char *szFnd,char *szIns);
short cmd_edit_one_att();

struct cmd_ddedit_globalpacket {
    char text[IC_ACADBUF];
    char tag[IC_ACADBUF];
    char prompt[IC_ACADBUF];
    char defval[IC_ACADBUF];
} cmd_ddedit_gpak;

// Functions

short cmd_ddedit(void) { 

	if (cmd_iswindowopen() == RTERROR) return RTERROR;			

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	RT ret=RTNORM;
    extern sds_name SDS_EditPropsSS;

	if(RTNORM==(ret=sds_pmtssget(ResourceString(IDC_DDEDIT__NSELECT_ENTITY_T_0, "\nSelect entity to modify: " ),""/*DNT*/,NULL,NULL,NULL,SDS_EditPropsSS,1))) 
	{
		if (RemoveHatchFromSS(SDS_EditPropsSS) == RTNIL) {
			ret = RTNIL;
		}

		ExecuteUIAction( ICAD_WNDACTION_MODIFY );
	}
    return(ret); 
}


//***********************************************************************************************************
//***********************************************************************************************************
//***********************************************************************************************************
//ATTEDIT
//***********************************************************************************************************
//***********************************************************************************************************
//***********************************************************************************************************

short cmd_get_blk_tag_val(char **pBlock,char **pTag,char **pValue) {
    RT	ret;
    char fs1[IC_ACADBUF];
    
    char tmpPrompt[100];
    sprintf(tmpPrompt,ResourceString(IDC_DDEDIT__NEDIT_ATTRIBUTES_1, "\nEdit attributes of which blocks? <*>: " ));
    if((ret=sds_getstring(0,tmpPrompt,fs1))==RTERROR) {
        goto bail;
    } else if(ret==RTCAN) {
        goto bail;
    }
    if(strlen(fs1)==0) {fs1[0]='*'/*DNT*/;fs1[1]=0;}
    strupr(fs1);
    *pBlock= new char [strlen(fs1)+1];
    strcpy(*pBlock,fs1);

    sprintf(tmpPrompt,ResourceString(IDC_DDEDIT__NEDIT_ATTRIBUTES_2, "\nEdit attributes with these names <*>: " ));
    if((ret=sds_getstring(0,tmpPrompt,fs1))==RTERROR) {
        goto bail;
    } else if(ret==RTCAN) {
        goto bail;
    }
    if(strlen(fs1)==0) {fs1[0]='*'/*DNT*/;fs1[1]=0;}
    strupr(fs1);
    *pTag= new char [strlen(fs1)+1];
    strcpy(*pTag,fs1);

    sprintf(tmpPrompt,ResourceString(IDC_DDEDIT__NEDIT_ATTRIBUTES_3, "\nEdit attributes with this text <*>: " ));
    if((ret=sds_getstring(0,tmpPrompt,fs1))==RTERROR) {        
        goto bail;
    } else if(ret==RTCAN) {
        goto bail;
    }
    if(strlen(fs1)==0) {fs1[0]='*'/*DNT*/;fs1[1]=0;}
    *pValue= new char [strlen(fs1)+1];
    strcpy(*pValue,fs1);

    ret=RTNORM;
bail:
    SDS_SetLastPrompt(tmpPrompt);
    return(ret);

}

short cmd_get_new_str(char *szFnd,char *szIns){
    RT ret;

    if((ret=sds_getstring(0,ResourceString(IDC_DDEDIT__NTEXT_TO_CHANGE__4, "\nText to change: " ),szFnd))==RTERROR) {
        goto bail;
    } else if(ret==RTCAN) {
        goto bail;
    }
    if((ret=sds_getstring(0,ResourceString(IDC_DDEDIT__NNEW_TEXT___5, "\nNew text: " ),szIns))==RTERROR) {
        goto bail;
    } else if(ret==RTCAN) {
        goto bail;
    }

    ret=RTNORM;
bail:
    return(ret);
}

//Bugzilla::78536; 07-07-03; [
short cmd_get_str(char **pTxt,char *szFnd,char *szIns){
    CString csOld;    

    csOld=*pTxt; //save the pointer

	if ( csOld.Replace(szFnd,szIns) == 0 )
		return(RTNORM);
        
	if ( strlen(*pTxt) < csOld.GetLength())
		*pTxt=ic_realloc_char((char *)*pTxt,csOld.GetLength()+1);
    
	strcpy(*pTxt,(LPCSTR)csOld);

    return(RTNORM);
}

//Bugzilla::78536; 07-07-03;]

short cmd_edit_global_att(bool bMode){
    char szFnd[IC_ACADBUF],szIns[IC_ACADBUF];
    struct resbuf *entmask=NULL,*pRb=NULL,*pRbb=NULL,rb210;
    char *pBlockFilter=NULL,*pTagFilter=NULL,*pValueFilter=NULL;
    sds_name ss,ent;
    sds_point pt210,pttmp;
    RT ret;
    long len,i,count;

    //get block,tag,value spec                     
    if((ret=cmd_get_blk_tag_val(&pBlockFilter,&pTagFilter,&pValueFilter))!=RTNORM) goto bail;
 
    //build the entmask
	SDS_getvar(NULL,DB_QUCSXDIR,&rb210,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	pttmp[0]=rb210.resval.rpoint[0];pttmp[1]=rb210.resval.rpoint[1];pttmp[2]=rb210.resval.rpoint[2];
    SDS_getvar(NULL,DB_QUCSYDIR,&rb210,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	//find the current UCS dir for the 210 group
	pt210[0]=(pttmp[1]*rb210.resval.rpoint[2])-(pttmp[2]*rb210.resval.rpoint[1]);
	pt210[1]=(pttmp[2]*rb210.resval.rpoint[0])-(pttmp[0]*rb210.resval.rpoint[2]);
	pt210[2]=(pttmp[0]*rb210.resval.rpoint[1])-(pttmp[1]*rb210.resval.rpoint[0]);
    
    if((entmask=sds_buildlist(RTDXF0,"INSERT"/*DNT*/,2,pBlockFilter,66,1,210,pt210,0))==NULL) {
        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);                     
        ret=RTERROR; goto bail;                                           
    }                                                                

    if(bMode){
        //get all blocks that match blockname and the user selected
        if((ret=sds_pmtssget(ResourceString(IDC_DDEDIT__NSELECT_ATTRIBUT_6, "\nSelect attributes: " ),NULL,NULL,NULL,entmask,ss,0))!=RTNORM) {
			if(ret==RTERROR) cmd_ErrorPrompt(CMD_ERR_NOATTRIBS,0);	//don't give error on cancel
        goto bail;       
            goto bail;
        }
    }else{
        //get all blocks that match blockname
        if((ret=sds_pmtssget(NULL,"A"/*DNT*/,NULL,NULL,entmask,ss,0))!=RTNORM) {
        cmd_ErrorPrompt(CMD_ERR_NOATTRIBS,0);        
            goto bail;
        }
    }
    IC_RELRB(entmask);
    
    sds_sslength(ss,&len);
    //count attribs
    for(count=i=0;i<len;i++){
        if((ret=sds_ssname(ss,i,ent))!=RTNORM) goto bail;
        while(sds_entnext_noxref(ent,ent)==RTNORM){
            if((pRbb=pRb=sds_entget(ent))==NULL) {ret=RTERROR; goto bail;}    
            for(pRb=pRbb;pRb->restype!=0;pRb=pRb->rbnext);
            if(strsame(pRb->resval.rstring,"INSERT"/*DNT*/)) continue;
            if(strsame(pRb->resval.rstring,"SEQEND"/*DNT*/)) break;
            for(pRb=pRbb;pRb->restype!=2;pRb=pRb->rbnext);
            if(sds_wcmatch(pRb->resval.rstring,pTagFilter)==RTNORM) {
                for(pRb=pRbb;pRb->restype!=1;pRb=pRb->rbnext);
                if(sds_wcmatch(pRb->resval.rstring,pValueFilter)==RTNORM) {
                    count++;
                }
            }
        }
    }
    if(!count){ 
        cmd_ErrorPrompt(CMD_ERR_NOATTRIBS,0);        
        goto bail;
    }else{
        sds_printf(ResourceString(IDC_DDEDIT__N_I_ATTRIBUTES_W_7, "\n%i attributes were selected." ),count);
    }
    
    //get str to replace
    if((ret=cmd_get_new_str(szFnd,szIns))!=RTNORM) goto bail;
    
    //loop through all of the blocks
    for(i=0;i<len;i++){
        if((ret=sds_ssname(ss,i,ent))!=RTNORM) goto bail;
        //loop though all the attribs
        while(sds_entnext_noxref(ent,ent)==RTNORM){
            if((pRbb=pRb=sds_entget(ent))==NULL) {ret=RTERROR; goto bail;}    
            for(pRb=pRbb;pRb->restype!=0;pRb=pRb->rbnext);
            if(strsame(pRb->resval.rstring,"INSERT"/*DNT*/)) continue;
            if(strsame(pRb->resval.rstring,"SEQEND"/*DNT*/)) break;
            for(pRb=pRbb;pRb->restype!=2;pRb=pRb->rbnext);
            if(sds_wcmatch(pRb->resval.rstring,pTagFilter)==RTNORM) {
                for(pRb=pRbb;pRb->restype!=1;pRb=pRb->rbnext);
                if(sds_wcmatch(pRb->resval.rstring,pValueFilter)==RTNORM) {
                    if((ret=cmd_get_str(&pRb->resval.rstring,szFnd,szIns))!=RTNORM) goto bail;
                    if((ret=sds_entmod(pRbb))!=RTNORM) goto bail;
                    IC_RELRB(pRbb);
                }
            }
        }
        if((ret=sds_entupd(ent))!=RTNORM) goto bail;
    }
    ret=RTNORM;
bail:
    delete pBlockFilter;delete pTagFilter;delete pValueFilter;
    sds_ssfree(ss);
    IC_RELRB(pRbb);
    IC_RELRB(entmask);
    return(ret);
}

short cmd_edit_one_att(){
    struct resbuf *entmask=NULL,*pRb=NULL,*pRbb=NULL,rb210,*tmprb=NULL;
    char fs1[IC_ACADBUF],fs2[IC_ACADBUF];
    char *pBlockFilter=NULL,*pTagFilter=NULL,*pValueFilter=NULL;
    sds_real fr1;
    sds_name ss={NULL,NULL},ent={NULL,NULL};
    struct cmd_elistll *pBeg=NULL,*pCur=NULL;                                 
    sds_point pt210,pttmp,ptX;
    RT ret;
    long len,i,count;
    int fi1;
    bool bGripSelect=false;
     
    //build the entmask
	//find the current UCS dir for the 210 group
	SDS_getvar(NULL,DB_QUCSXDIR,&rb210,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	pttmp[0]=rb210.resval.rpoint[0];pttmp[1]=rb210.resval.rpoint[1];pttmp[2]=rb210.resval.rpoint[2];
    SDS_getvar(NULL,DB_QUCSYDIR,&rb210,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
    ic_crossproduct(pttmp,rb210.resval.rpoint,pt210);

    if((sds_sslength(SDS_CURDOC->m_pGripSelection,&i)==RTNORM)&& i>0L) {
        bGripSelect=true;
    }

    //get block,tag,value spec                     
    if((ret=cmd_get_blk_tag_val(&pBlockFilter,&pTagFilter,&pValueFilter))!=RTNORM) goto bail;
    
    if((entmask=sds_buildlist(RTDXF0,"INSERT"/*DNT*/,2,pBlockFilter,66,1,210,pt210,0))==NULL) {
        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);                     
        ret=RTERROR; goto bail;                                           
    }                                                                

    //get all blocks that match blockname and the user selected
    if((ret=sds_pmtssget(ResourceString(IDC_DDEDIT__NSELECT_ATTRIBUT_6, "\nSelect attributes: " ),NULL,NULL,NULL,entmask,ss,0))!=RTNORM) {
        if(ret==RTERROR)cmd_ErrorPrompt(CMD_ERR_NOATTRIBS,0); //don't give error on cancel
        goto bail;
    }
    IC_RELRB(entmask);

    sds_sslength(ss,&len);
    //*check for proper entites
    if(bGripSelect==true){
        for(i=0L;i<len;i++){
            if((ret=sds_ssname(ss,i,ent))!=RTNORM) goto bail;
            if((pRbb=sds_entget(ent))==NULL) {ret=RTERROR; goto bail;}    
                for(pRb=pRbb;(pRb!=NULL)&&(pRb->restype!=0);pRb=pRb->rbnext);
                if(pRb==NULL) {IC_RELRB(pRbb);sds_ssdel(ent,ss);continue;}
                if(strsame(pRb->resval.rstring,"INSERT"/*DNT*/)){
                    for(pRb=pRbb;(pRb!=NULL)&&(pRb->restype!=2);pRb=pRb->rbnext);
                    if(pRb==NULL) {IC_RELRB(pRbb);sds_ssdel(ent,ss);continue;}
                    if(sds_wcmatch(pRb->resval.rstring,pBlockFilter)==RTNORM){
                        for(pRb=pRbb;(pRb!=NULL)&&(pRb->restype!=66);pRb=pRb->rbnext);
                        if(pRb==NULL) {IC_RELRB(pRbb);sds_ssdel(ent,ss);continue;}
                        if(pRb->resval.rint==1){
                            continue;
                        }else{
                            IC_RELRB(pRbb);sds_ssdel(ent,ss);
                        }    
                    }else{
                        IC_RELRB(pRbb);sds_ssdel(ent,ss);
                    }
                }else{
                    IC_RELRB(pRbb);sds_ssdel(ent,ss);
                }
        
        }                    
    }
    IC_RELRB(pRbb);        
    sds_sslength(ss,&len);
            
    //count attribs
    for(count=i=0L;i<len;i++){
        if((ret=sds_ssname(ss,i,ent))!=RTNORM) goto bail;
        sds_redraw(ent,IC_REDRAW_DRAW);           //TODO Take out after we get the right selection implemented
        while(sds_entnext_noxref(ent,ent)==RTNORM){
            if((pRbb=pRb=sds_entget(ent))==NULL) {ret=RTERROR; goto bail;}    
            for(pRb=pRbb;pRb->restype!=0;pRb=pRb->rbnext);
            if(strsame(pRb->resval.rstring,"INSERT"/*DNT*/)) continue;
            if(strsame(pRb->resval.rstring,"SEQEND"/*DNT*/)) break;
            for(pRb=pRbb;pRb->restype!=2;pRb=pRb->rbnext);
            if(sds_wcmatch(pRb->resval.rstring,pTagFilter)==RTNORM) {
                for(pRb=pRbb;pRb->restype!=1;pRb=pRb->rbnext);
                if(sds_wcmatch(pRb->resval.rstring,pValueFilter)==RTNORM) {
                    count++;
                }
            }
        }
    }
    IC_RELRB(pRbb);
    
    if(!count){ 
        cmd_ErrorPrompt(CMD_ERR_NOATTRIBS,0);        
        goto bail;
    }else{
        sds_printf(ResourceString(IDC_DDEDIT__N_I_ATTRIBUTES_W_7, "\n%i attributes were selected." ),count);
    }
    
    cmd_ss2elistll(&pBeg,&pCur,ss,CMD_NULLASSOCVAL); 
    pCur=pBeg;
    //set pCur->prev pointers
    pCur->prev=NULL;
    while(pCur->next!=NULL){
        (pCur->next)->prev=pCur;
        pCur=pCur->next;
    }
    pCur=pBeg;
        
    //go to first att
    for(pRb=pCur->entlst;pRb->restype!=0;pRb=pRb->rbnext);
    if(strsame(pRb->resval.rstring,"INSERT"/*DNT*/)) pCur=pCur->next;
    for(pRb=pCur->entlst;pRb->restype!=-1;pRb=pRb->rbnext);
    ic_encpy(ent,pRb->resval.rlname);
    sds_redraw(ent,IC_REDRAW_HILITE_ONE);
    for(pRb=pCur->entlst;pRb->restype!=10;pRb=pRb->rbnext);
    cmd_pedit_drawx(pRb->resval.rpoint,2,pt210);
    ic_ptcpy(ptX,pRb->resval.rpoint);

    while(1) {
        if((ret=sds_initget(0,ResourceString(IDC_DDEDIT_INITGET_POSITION__8, "Position Angle ~ Text ~Value ~ Style Color Height Layer ~ Next Previous|pRev Quit ~_Position ~_Angle ~_ ~_Text ~_Value ~_ ~_Style ~_Color ~_Height ~_Layer ~_ ~_Next ~_pRev ~_Quit" )))!=RTNORM) { 
            goto bail; 
        }
        if((ret=sds_getkword(ResourceString(IDC_DDEDIT__NPOSITION_ANGLE__9, "\nPosition/Angle/Text/Style/Color/Height/Layer/Next/pRev/Quit/<Next>:  " ),fs1))==RTERROR) {
            goto bail;
        } else if(ret==RTCAN) {
            sds_redraw(ent,IC_REDRAW_DRAW);
            cmd_pedit_drawx(ptX,2,pt210);
            goto bail;
        }
        strupr(fs1);
        if(ret==RTNONE || strisame("NEXT"/*DNT*/,fs1)) {
            if(pCur->next==NULL) goto bail; //end of the llist   
            while(1){ //check for att, loop until one is found
                pCur=pCur->next;
                for(pRb=pCur->entlst;pRb->restype!=0;pRb=pRb->rbnext);
                if(strsame(pRb->resval.rstring,"INSERT"/*DNT*/)||strsame(pRb->resval.rstring,"SEQEND"/*DNT*/)){
                    if(pCur->next==NULL) {
                        sds_redraw(ent,IC_REDRAW_DRAW);
                        cmd_pedit_drawx(ptX,2,pt210);
                        goto exit;
                    }
                }else{
                    break;
                }
            }
            sds_redraw(ent,IC_REDRAW_DRAW);
            for(pRb=pCur->entlst;pRb->restype!=-1;pRb=pRb->rbnext);
            ic_encpy(ent,pRb->resval.rlname);
            sds_redraw(ent,IC_REDRAW_HILITE_ONE);
            cmd_pedit_drawx(ptX,2,pt210);
            for(pRb=pCur->entlst;pRb->restype!=10;pRb=pRb->rbnext);
            ic_ptcpy(ptX,pRb->resval.rpoint);
            cmd_pedit_drawx(ptX,2,pt210);
            continue;
        } else if(strisame("PREV"/*DNT*/,fs1)) {
            if(pCur->prev==NULL) goto bail; //end of the llist   
            while(1){ //check for att, loop until one is found
                pCur=pCur->prev;
                for(pRb=pCur->entlst;pRb->restype!=0;pRb=pRb->rbnext);
                if(strsame(pRb->resval.rstring,"INSERT"/*DNT*/)||strsame(pRb->resval.rstring,"SEQEND"/*DNT*/)){
                    if(pCur->prev==NULL) {
                        sds_redraw(ent,IC_REDRAW_DRAW);
                        cmd_pedit_drawx(ptX,2,pt210);
                        goto exit;
                    }
                }else{
                    break;
                }
            }
            sds_redraw(ent,IC_REDRAW_DRAW);
            for(pRb=pCur->entlst;pRb->restype!=-1;pRb=pRb->rbnext);
            ic_encpy(ent,pRb->resval.rlname);
            sds_redraw(ent,IC_REDRAW_HILITE_ONE);
            cmd_pedit_drawx(ptX,2,pt210);
            for(pRb=pCur->entlst;pRb->restype!=10;pRb=pRb->rbnext);
            ic_ptcpy(ptX,pRb->resval.rpoint);
            cmd_pedit_drawx(ptX,2,pt210);
            continue;
        }else if(strisame("QUIT"/*DNT*/,fs1)){
            sds_redraw(ent,IC_REDRAW_DRAW);
            cmd_pedit_drawx(ptX,2,pt210);
            goto exit;
        } else if((strisame("VALUE"/*DNT*/,fs1)) || strisame("TEXT"/*DNT*/,fs1)) {
            if((ret=sds_initget(0,ResourceString(IDC_DDEDIT_INITGET_CHANGE_R_15, "Change Replace ~_Change ~_Replace" )))!=RTNORM) { goto bail; }
            if((ret=sds_getkword(ResourceString(IDC_DDEDIT__NCHANGE__REPLAC_16, "\nChange/<Replace>  " ),fs1))==RTERROR) {
                goto bail;
            } else if(ret==RTCAN) {
                goto bail;
            } 
            strupr(fs1);
            for(pRb=pCur->entlst;pRb->restype!=1;pRb=pRb->rbnext);
            //if(ret==RTNONE || strisame("PREV"/*DNT*/,fs1)) {
            if(ret==RTNONE || strisame("REPLACE"/*DNT*/,fs1)) { //  EBATECH(FUTA) 2001-8-11  fix to don't move REPLACE-option
                sprintf(fs2,ResourceString(IDC_DDEDIT__NNEW_ATTRIBUTE__17, "\nNew attribute text <%s>: " ),pRb->resval.rreal);
                if((ret=sds_getstring(1,fs2,fs1))==RTERROR) {
                    goto bail;
                } else if(ret==RTCAN) {
                    goto bail;
                }
                if(*fs1){
                    pRb->resval.rstring=ic_realloc_char((char *)pRb->resval.rstring,strlen(fs1)+1);
                    strcpy(pRb->resval.rstring,fs1);
                }
            } else {
                if((ret=sds_getstring(1,ResourceString(IDC_DDEDIT__NTEXT_TO_CHANGE__4, "\nText to change: " ),fs1))==RTERROR) {
                    goto bail;
                } else if(ret==RTCAN) {
                    goto bail;
                }
                if((ret=sds_getstring(1,ResourceString(IDC_DDEDIT__NNEW_TEXT___5, "\nNew text: " ),fs2))==RTERROR) {
                    goto bail;
                } else if(ret==RTCAN) {
                    goto bail;
                }
                if((ret=cmd_get_str(&pRb->resval.rstring,fs1,fs2))!=RTNORM) goto bail;
            }
        } else if(strisame("POSITION"/*DNT*/,fs1)) {
            for(pRb=pCur->entlst;pRb->restype!=10;pRb=pRb->rbnext);
            if((ret=sds_getpoint(pRb->resval.rpoint,ResourceString(IDC_DDEDIT__NENTER_TEXT_INS_19, "\nEnter text insertion point: " ),pttmp))==RTERROR) {
                goto bail;
            } else if(ret==RTCAN){
                goto bail;
            }else if(ret==RTNORM){
                ic_ptcpy(pRb->resval.rpoint,pttmp);
                cmd_pedit_drawx(ptX,2,pt210);
                ic_ptcpy(ptX,pRb->resval.rpoint);
                cmd_pedit_drawx(ptX,2,pt210);
            }
        } else if(strisame("HEIGHT"/*DNT*/,fs1)) {
            for(pRb=pCur->entlst;pRb->restype!=10;pRb=pRb->rbnext);
            ic_ptcpy(pttmp,pRb->resval.rpoint);
            for(pRb=pCur->entlst;pRb->restype!=40;pRb=pRb->rbnext);
            sprintf(fs2,ResourceString(IDC_DDEDIT__NNEW_TEXT_HEIGH_21, "\nNew text height <%f>: " ),pRb->resval.rreal);
            if((ret=sds_getdist(pttmp,fs2,&fr1))==RTERROR) {
                goto bail;
            }else if(ret==RTCAN) {
                goto bail;
            }else if(ret==RTNORM) {
                pRb->resval.rreal=fr1;
            }
        } else if(strisame("ANGLE"/*DNT*/,fs1)) {
            for(pRb=pCur->entlst;pRb->restype!=10;pRb=pRb->rbnext);
            ic_ptcpy(pttmp,pRb->resval.rpoint);
            for(pRb=pCur->entlst;pRb->restype!=50;pRb=pRb->rbnext);
            sds_angtos(pRb->resval.rreal,-1,-1,fs1);
            sprintf(fs2,ResourceString(IDC_DDEDIT__NNEW_ROTATION_A_23, "\nNew rotation angle <%s>: " ),fs1);
            if((ret=sds_getangle(pttmp,fs2,&fr1))==RTERROR) {
                goto bail;
            } else if(ret==RTCAN) {
                goto bail;
            }else if(ret==RTNORM) {
                pRb->resval.rreal=fr1;
            }
        } else if(strisame("STYLE"/*DNT*/,fs1)) {
            for(pRb=pCur->entlst;pRb->restype!=7;pRb=pRb->rbnext);
			sds_initget(SDS_RSG_NODOCCHG|RSG_OTHER,NULL);            
			sprintf(fs2,ResourceString(IDC_DDEDIT__NNEW_TEXT_STYLE_25, "\nNew text style or ENTER for no change <%s>: " ),pRb->resval.rstring);
            if((ret=sds_getstring(1,fs2,fs1))==RTERROR) {
                goto bail;
            } else if(ret==RTCAN) {
                goto bail;
            } else if(ret==RTNONE) {
                continue;				
			}
            ic_trim(fs1,"be");
            if((tmprb=sds_tblsearch("STYLE"/*DNT*/,fs1,0))!=NULL){
                pRb->resval.rstring=ic_realloc_char((char *)pRb->resval.rstring,strlen(fs1)+1);
                strcpy(pRb->resval.rstring,fs1);
                IC_RELRB(tmprb);
            }else{
                sds_printf(ResourceString(IDC_DDEDIT__NSTYLE___26, "\nStyle \"%s\" not defined:" ),fs1);
            }
        } else if(strisame("LAYER"/*DNT*/,fs1)) {
            for(pRb=pCur->entlst;pRb->restype!=8;pRb=pRb->rbnext);
			sds_initget(SDS_RSG_NODOCCHG|RSG_OTHER,NULL);
			sprintf(fs2,ResourceString(IDC_DDEDIT__NNEW_LAYER_OR_E_29, "\nNew layer or ENTER for no change <%s>: " ),pRb->resval.rstring);
            if((ret=sds_getkword(fs2,fs1))==RTERROR) {
                goto bail;
            } else if(ret==RTCAN) {
                goto bail;
            } else if(ret==RTNONE) {
                continue;						
			}
            if((tmprb=sds_tblsearch("LAYER"/*DNT*/,fs1,0))!=NULL){
                pRb->resval.rstring=ic_realloc_char((char *)pRb->resval.rstring,strlen(fs1)+1);
                strcpy(pRb->resval.rstring,fs1);
                IC_RELRB(tmprb);
            }else{
                sds_printf(ResourceString(IDC_DDEDIT__NLAYER___30, "\nLayer \"%s\" is not defined:" ),fs1);
            }
        } else if(strisame("COLOR"/*DNT*/,fs1)) {
            while(1){  
                for(pRb=pCur->entlst;pRb->restype!=62;pRb=pRb->rbnext);
                ic_colorstr(pRb->resval.rint,fs1);
				sds_initget(SDS_RSG_NODOCCHG|RSG_OTHER,ResourceString(IDC_DDEDIT_INITGET_BYLAYER__33, "BYLAYER|BYLayer BYBLOCK|BYBlock ~ Red Yellow Green Cyan BLue Magenta White ~_BYLayer ~_BYBlock ~_ ~_Red ~_Yellow ~_Green ~_Cyan ~_BLue ~_Magenta ~_White" ));
                sprintf(fs2,ResourceString(IDC_DDEDIT__NNEW_COLOR___S__34, "\nNew color <%s>: " ),fs1);
                if((ret=sds_getkword(fs2,fs1))==RTERROR) {
                    goto bail;
                } else if(ret==RTCAN) {
                    goto bail;
				} else if(ret==RTNONE) {
					continue;						
				}
                if(*fs1) {
                    fi1=ic_colornum(fs1);
                    if(fi1>=0 && fi1<=256){
                        pRb->resval.rint=fi1;
                        break;
                    }else{
                        cmd_ErrorPrompt(CMD_ERR_COLOR0256,0);        
                    }
                }else{
                    break;
                }
            }
        }
        sds_entmod(pCur->entlst);
        for(pRb=pCur->entlst;pRb->restype!=-1;pRb=pRb->rbnext);
        sds_entupd(pRb->resval.rlname);
        sds_redraw(ent,IC_REDRAW_HILITE_ONE);
    }

exit:
    ret=RTNORM;
bail:
    sds_ssfree(ss);
    IC_RELRB(pRbb);
    IC_RELRB(entmask);
    cmd_free_elistll(&pBeg,NULL);
    return(ret);
}

short cmd_attedit(void) {

    if (cmd_iswindowopen() == RTERROR) return RTERROR;			

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    char fs1[IC_ACADBUF];
    RT ret;
    
    if((ret=sds_initget(0,ResourceString(IDC_DDEDIT_INITGET_YES_EDIT_35, "Yes-Edit_one_at_a_time|Yes No-Edit_multiple|No ~_Yes ~_No" )))!=RTNORM) { 
        goto bail; 
    }
    char tmpPrompt[100];
    sprintf(tmpPrompt,ResourceString(IDC_DDEDIT__NEDIT_ATTRIBUTE_36, "\nEdit attributes one at a time? <Y> " ));
    if((ret=sds_getkword(tmpPrompt,fs1))==RTERROR) {
        SDS_SetLastPrompt(tmpPrompt);
        goto bail;
    } else if(ret==RTCAN) {
        goto bail;
    } else if(ret==RTNONE) {
    } else if(ret==RTNORM) {
        strupr(fs1);
        //Edit attributes global
        if(strisame(fs1, "NO"/*DNT*/)) {
            sds_printf(ResourceString(IDC_DDEDIT__NGLOBAL_EDIT_OF_38, "\nGlobal edit of attribute values." ));	
            if((ret=sds_initget(0,ResourceString(IDC_DDEDIT_INITGET_YES_VISI_39, "Yes-Visible_only|Yes No-Edit_all|No ~_Yes ~_No" )))!=RTNORM) { goto bail; }
            if((ret=sds_getkword(ResourceString(IDC_DDEDIT__NEDIT_ONLY_ATTR_40, "\nEdit only attributes visible on screen? <Y> " ),fs1))==RTERROR) {
                goto bail;
            } else if(ret==RTCAN) {
                goto bail;
            } else if(ret==RTNONE||ret==RTNORM) {
                strupr(fs1);
                //Edit attributes visible
                if(strisame(fs1, "NO"/*DNT*/)) {
                    if((ret=cmd_edit_global_att(false))!=RTNORM) goto bail;
                    return(RTNORM);
                }else{
                    if((ret=cmd_edit_global_att(true))!=RTNORM) goto bail;
                    return(RTNORM);
                }
            }
        }
    }

    if((ret=cmd_edit_one_att())!=RTNORM) goto bail;

bail:

    return(ret);
}






bool cmd_access(char *fname){
    HANDLE fhand=NULL;

    //check if it already exists
    if((fhand=::CreateFile(fname,GENERIC_WRITE,0,NULL,CREATE_NEW,0,NULL))==INVALID_HANDLE_VALUE){
        //if it does exist, try to open for writing.
        if((fhand=::CreateFile(fname,GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL))==INVALID_HANDLE_VALUE){
      	    //DWORD err=GetLastError(); //used for debuging
            return(0);
        }
    }else{ //if we created the file then delete it.
        ::CloseHandle(fhand);fhand=NULL;
        unlink(fname);    
        return(1);
    }
    ::CloseHandle(fhand);fhand=NULL;
    return(1);
}

