/* CMDS_MISC.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * Supports miscellaneous commands and helpers
 *
 */
// THIS FILE HAS BEEN GLOBALIZED!
//** Includes
#include "cmds.h"/*DNT*/
#include "Icad.h" /*DNT*/
#include "IcadDoc.h"
#include "IcadApi.h"
#include "preferences.h"

#define NEW_OOPS_VERSION

// **************
// GLOBALS
//
extern bool    SDS_DontBitBlt;

// This function concatinates two initget ResourceStrings. Given:
// "One Two _three _four" and "5 6 _7 _8" it will set RetStr to:
// "One Two 5 6 _three _four _7 _8", which is still a valid initget()
// string.
void catTwoResourceStrings(char *FirstStr, char *SecondStr, char *RetStr)
{
	char localPart1[IC_ACADBUF], localPart2[IC_ACADBUF]; 
	TCHAR *globalPart1, *globalPart2;

	globalPart1 = _tcsstr(FirstStr, "~_"/*DNT*/);
	globalPart2 = _tcsstr(SecondStr,"~_"/*DNT*/);
    ASSERT(globalPart1 != NULL && globalPart2 != NULL );

	_tcsncpy(localPart1, FirstStr, IC_ACADBUF - 1);
	localPart1[globalPart1 - FirstStr - 1] = '\0'/*DNT*/;

	_tcsncpy(localPart2, SecondStr, IC_ACADBUF - 1);
	localPart2[globalPart2 - SecondStr - 1] = '\0'/*DNT*/;

    sprintf(RetStr, "%s %s %s %s"/*DNT*/, localPart1, localPart2, globalPart1, globalPart2);
	return;
}


int getCurrColorIndex()
{
	char curcolor[IC_ACADBUF], curlay[IC_ACADNMLEN];
	struct resbuf rb;
	SDS_getvar(NULL,DB_QCECOLOR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	strcpy(curcolor,rb.resval.rstring);
	IC_FREE(rb.resval.rstring);
	int nCurCol;
	if (sds_wcmatch(curcolor, "BYBLOCK") == RTNORM) 			
		nCurCol = 7;
	else if (sds_wcmatch(curcolor, "BYLAYER") == RTNORM) {	
		SDS_getvar(NULL,DB_QCLAYER,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		strncpy(curlay, rb.resval.rstring,sizeof(curlay)-1);
		IC_FREE(rb.resval.rstring);
		resbuf	*pRrb = sds_tblsearch("LAYER",curlay,0), *pCurRb = pRrb;
		while (pCurRb && pCurRb->restype != 62)
			pCurRb = pCurRb->rbnext;

		if(pCurRb)
			nCurCol = pCurRb->resval.rint;
		else
			nCurCol = 7;

		sds_relrb(pRrb);
	}
    else
        nCurCol = atoi(curcolor);

	return nCurCol;
}

sds_real cmd_dist2d(sds_point pt1, sds_point pt2) {
//*** This function calculates the 2D distance between pt1 and pt2.
    sds_real deltaX,deltaY;

    deltaX=pt1[0]-pt2[0];
    deltaY=pt1[1]-pt2[1];
    return(sqrt(deltaX*deltaX + deltaY*deltaY));
}


short cmd_erase(void) {

    if (cmd_iswindowopen() == RTERROR) return RTERROR;
	
	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    sds_name ss1;
    RT	ret;

    if((ret=sds_pmtssget(ResourceString(IDC_CMDS_MISC__NSELECT_ENTIT_0, "\nSelect entities to delete: " ),NULL,NULL,NULL,NULL,ss1,1))!=RTNORM) return(ret);
    ret=cmd_erase_and_oops(ss1,0);
    sds_ssfree(ss1);
	//free the previous selection set even though we didn't update it.
	//Acad frees it, as some may now be deleted ents
	if(ret==RTNORM)sds_ssfree(SDS_CURDOC->m_pPrevSelection);
    return(ret);
}

#ifndef NEW_OOPS_VERSION
short cmd_oops(void) {
    if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    return(cmd_erase_and_oops(NULL,1));
}
#else
short cmd_oops(void) {
    if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	if(SDS_CURDOC && SDS_CURDOC->m_dbDrawing)
	{
		if(!SDS_CURDOC->m_cmdHelper.isOopsAvailable())
		{
			cmd_ErrorPrompt(CMD_ERR_NOUNERASE,0);
			return RTCAN;
		}
		SDS_CURDOC->m_cmdHelper.oopsProcess(SDS_CURDOC);
		return RTNORM;
	}
	return RTERROR;
}
#endif

short cmd_erase_and_oops(sds_name argss, short oopsmode) {
//*** oopsmode:  0 - Erases entities from drawing.
//***            1 - Restores erased entities.
//***
//*** This function also gets called from cmd_block() and cmd_wblock().
//***
//*** Returns:  RTNORM - success.
//***            RTCAN - nothing to undo or funtion canceled.
//***          RTERROR - bad sds_entdel value.
//***
	if(SDS_CURDOC==NULL) return(RTERROR);

    sds_name ename;
    long ssct,rc=0L;
	int	curprogress=0,fi1;
	long ssLen;

    sds_name ss1;

	// Remove proxy entities from the selection set.
	long removedCount = 0;
	bool userNotified = false;
	for(ssct=0L;sds_ssname(argss,ssct,ename)==RTNORM;ssct++) 
	{
		if (IC_TYPE_FROM_ENAME(ename) == DB_ACAD_PROXY_ENTITY) 
		{
			if (userNotified == false)
			{
				userNotified = true;
				sds_printf(ResourceString(CMD_ERR_DWGPROXY, "Proxy entities can not be edited." ));
			}
			removedCount++;
			sds_redraw(ename,IC_REDRAW_UNHILITE);
			sds_ssdel(ename, argss);
		}
	}
	if (removedCount)
	{
		char fs1[81];
		sprintf(fs1, "%s%d", ResourceString(IDC_ICADSELECTDIA__ENTITIES__8, " Entities removed="), removedCount);
		sds_printf(fs1);
	}

    if(oopsmode) {
        if(SDS_sscpy(ss1,SDS_CURDOC->m_pOopsSelection)<=0L) {
			cmd_ErrorPrompt(CMD_ERR_NOUNERASE,0);
            rc=(-1); goto bail;
        }
        sds_ssfree(SDS_CURDOC->m_pOopsSelection);
    } else SDS_sscpy(ss1,argss);

	SDS_ProgressStart();
	SDS_DontBitBlt=1;
	sds_sslength(ss1,&ssLen);

	if(oopsmode)
	{
		for(ssct=0L;sds_ssname(ss1,ssct,ename)==RTNORM;ssct++) {
			if(sds_usrbrk()) { rc=(-1L); goto bail; }
			if(sds_entdel(ename)!=RTNORM) { rc=(-__LINE__); goto bail; }
			fi1=(int)(((double)ssct/ssLen)*100);
			if(curprogress+5<=fi1) {
				curprogress=fi1;
				SDS_ProgressPercent(curprogress);
			}
		}
	}
	else
	{
		int ret = sds_delss(ss1, true);
		if(ret != RTNORM)
		{
			rc = (ret == RTCAN ? -1 : (-__LINE__));
		}
	}
	// EBATECH(FUTA)-[ 2001-01-17
	if(oopsmode){
		sds_ssfree(SDS_CURDOC->m_pPrevSelection);
		SDS_ssadd(NULL,NULL,SDS_CURDOC->m_pPrevSelection);
		SDS_sscpy(SDS_CURDOC->m_pPrevSelection,ss1);
	}// ]-EBATECH(FUTA)

    bail:
	SDS_BitBlt2Scr(1);
	SDS_ProgressStop();
	SDS_DontBitBlt=0;


    if(!oopsmode && rc==0) {
        sds_ssfree(SDS_CURDOC->m_pOopsSelection);
        SDS_sscpy(SDS_CURDOC->m_pOopsSelection,ss1);
    }
    sds_ssfree(ss1);

    if(rc==0L) return(RTNORM);
    if(rc==(-1L)) return(RTCAN);
    if(rc<(-1L)) CMD_INTERNAL_MSG(rc);
    return(RTERROR);
}

short cmd_handles(void) {
//*** This function handles handles.  We do not support turning handles off (destroying them).

    if (cmd_iswindowopen() == RTERROR) return RTERROR;

	char fs1[IC_ACADBUF];
    struct resbuf setgetrb;
    RT	ret=RTERROR;		// If nothing resets it, something is wrong

    for(;;) {
		SDS_getvar(NULL,DB_QHANDLES,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);	// Get current value of HANDLES
		if (setgetrb.resval.rint==0) {
			if (sds_initget(SDS_RSG_OTHER,ResourceString(IDC_CMDS_MISC_INITGET_CREATE_1, "Create_Handles|Create ~On ~OFf ~Destroy ~_Create ~_On ~_OFf ~_Destroy" ))!=RTNORM) 
                return(RTERROR);
			ret=sds_getkword(ResourceString(IDC_CMDS_MISC__NHANDLES_ARE__2, "\nHandles are not yet being used in this drawing:  Create/<Do not create handles>: " ),fs1);
		} else {
			if (sds_initget(SDS_RSG_OTHER,ResourceString(IDC_CMDS_MISC_INITGET__CREAT_3, "~Create ~On ~OFf ~Destroy ~_Create ~_On ~_OFf ~_Destroy" ))!=RTNORM) 
                return(RTERROR);
			ret=sds_getkword(ResourceString(IDC_CMDS_MISC__NHANDLES_ARE__4, "\nHandles are currently being used in this drawing.  <ENTER to continue> " ),fs1);
		}

		if ((ret==RTERROR) || (ret==RTCAN)) {
            return(ret);
		} else if (ret==RTNONE) {
			return (RTNORM);
		}
        if (strisame("CREATE"/*DNT*/,fs1) || strisame("ON"/*DNT*/,fs1)) {
            setgetrb.restype=RTSHORT;
            setgetrb.resval.rint=1;
			sds_setvar("HANDLES"/*DNT*/,&setgetrb);
		} else if (strisame("DESTROY"/*DNT*/,fs1) || strisame("OFF"/*DNT*/,fs1)) {
			cmd_ErrorPrompt(CMD_ERR_NODESTROY,0);
        } else {
			cmd_ErrorPrompt(CMD_ERR_KWORD,0);
            continue;
        }
        break;
    }
	setgetrb.rbnext=NULL;
    return(RTNORM);
}


long cmd_ss2elistll(struct cmd_elistll **beg, struct cmd_elistll **cur, sds_name ss1, short assocval) {
//*** This function creates a linked list of entity lists from a selection set
//*** of entities.  If "assocval" is set to CMD_NULLASSOCVAL then the grpptr
//*** element of the cmd_elistll structure will be set to NULL, otherwise it
//*** gets set to the link specified by the group code number in "assocval".
//*** This function was created to be called from cmd_change(), and is also
//*** called from cmd_trim_or_extend.
//***
//*** RETURNS:  0 - Success.
//***          -1 - User cancel
//***          <0 - Line number (negative) an error occured at.
//***
    struct cmd_elistll *curptr=NULL;
    sds_name ename;
    long ssct;
    long rc=0;

    if((*beg)!=NULL) {
        if(cur!=NULL) cmd_free_elistll(&(*beg),&(*cur));
        else cmd_free_elistll(&(*beg),NULL);
    }
    if(cur!=NULL) curptr=(*cur);
    for(ssct=0L;sds_ssname(ss1,ssct,ename)==RTNORM;ssct++) {
        //*** Check for a Cancel from the user.
        if(sds_usrbrk()) { rc=(-1L); goto bail; }
        //*** Call recursive function to allocate the list for the entity and its sub-entities
        if((rc=cmd_alloc_elistll(&(*beg),&curptr,ename,assocval))!=0L) goto bail;
    }

    bail:

    if(rc<0) {
        if(cur!=NULL) cmd_free_elistll(&(*beg),&(*cur));
        else cmd_free_elistll(&(*beg),NULL);
    }
    if(cur!=NULL) (*cur)=curptr;
    return(rc);
}

long cmd_alloc_elistll(struct cmd_elistll **beg, struct cmd_elistll **cur, sds_name ename, short assocval) {
	//NOTE:f'n does NOT set ->prev for each linked list entity!
    struct cmd_elistll *tmp,*curptr=NULL;
    struct resbuf app;
    sds_name tename;
    long rc=0L;

    if(cur!=NULL) curptr=(*cur);
    ic_encpy(tename,ename);
    //*** Allocate a new link in property list.    
	if((tmp= new struct cmd_elistll )==NULL) {
        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
        rc=(-__LINE__); goto bail;
    }
	memset(tmp,0,sizeof(struct cmd_elistll));
    if((*beg)==NULL) {
        curptr=(*beg)=tmp;
    } else {
        curptr=curptr->next=tmp;
    }
    //*** Set the elements of the list.
    ic_encpy(curptr->ename,tename);
    app.restype=RTSTR; app.resval.rstring="*"/*DNT*/; app.rbnext=NULL;
    if((curptr->entlst=sds_entgetx(tename,&app))==NULL) {
        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
        rc=(-__LINE__); goto bail;
    }
    if(assocval!=CMD_NULLASSOCVAL) {
        if(ic_assoc(curptr->entlst,assocval)==0) {
			curptr->grpptr=ic_rbassoc;
		}else{
			curptr->grpptr=NULL;
		}
    }
    if(ic_assoc(curptr->entlst,0)!=0) { rc=(-__LINE__); goto bail; }
    if(strsame("INSERT"/*DNT*/,ic_rbassoc->resval.rstring) ||
       strsame("POLYLINE"/*DNT*/,ic_rbassoc->resval.rstring)) {
        if(ic_assoc(curptr->entlst,66)==0 && ic_rbassoc->resval.rint==1) {
            while(sds_entnext_noxref(tename,tename)==RTNORM) {
                if((rc=cmd_alloc_elistll(&(*beg),&curptr,tename,assocval))<0) goto bail;
                if(rc==1L) { rc=0L; goto bail; }
            }
        }
    } else if(strsame("SEQEND"/*DNT*/,ic_rbassoc->resval.rstring)) {
        rc=1L; goto bail;
    }

    bail:

    if(cur!=NULL) (*cur)=curptr;
    return(rc);
}

void cmd_free_elistll(struct cmd_elistll **beg, struct cmd_elistll **cur) {
//*** This function frees the cmd_elistll linked list.
//***
// a function that requires TWO parameters to free a linked list?  Weird, eh?
// In the OLDVERSION section is the goofy original version of this function.  Note that cur's
// only function seems to be for *cur to be set to NULL at the end.  Although curptr is set to
// (*cur) at the top, it is immediately overwritten, before being used, in the first step of the loop body.
//
// Since existing code may depend on the side effect of *cur being set to NULL, I emulate that in the new version

#ifdef OLDVERSION
    struct cmd_elistll *tmp, *curptr=NULL;

    if(beg==NULL) return;
    if(cur!=NULL) curptr=(*cur);
    for(tmp=(*beg); tmp!=NULL; tmp=curptr) {
        curptr=tmp->next;
        IC_RELRB(tmp->entlst);
        IC_FREE(tmp);
    }
    (*beg)=NULL;
    if(cur!=NULL) (*cur)=NULL;
#endif

// NEW VERSION
    struct cmd_elistll *tmp, *curptr=NULL;

    if(beg==NULL) return;
// offending line removed here
    for(tmp=(*beg); tmp!=NULL; tmp=curptr) {
        curptr=tmp->next;
        IC_RELRB(tmp->entlst);
        IC_FREE(tmp);
    }
    (*beg)=NULL;
    if(cur!=NULL) (*cur)=NULL;
}


void cmd_elev_correct(sds_real zval, sds_point pt, int mode,sds_point extru2use){
	//f'n corrects along VIEWDIR for change in elevation between pt and zval.
	//if mode=0, pt is wcs pt
	//if mode=1, pt is ucs pt
	//if mode=2, pt is ecs point w/extru equal to current ucs z axis
	//if mode=3, pt is ecs point and use extru passed in extru2use

	struct resbuf rbucs,rbent,rbtemp;

	if(pt[2]==zval)return;
	if(mode==2){
		rbent.restype=RT3DPOINT;
		SDS_getvar(NULL,DB_QUCSXDIR,&rbtemp,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		SDS_getvar(NULL,DB_QUCSYDIR,&rbucs,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		ic_crossproduct(rbtemp.resval.rpoint,rbucs.resval.rpoint,rbent.resval.rpoint);
	}else if(mode==0){
		rbent.restype=RTSHORT;
		rbent.resval.rint=0;
	}else if(mode==3){
		if(extru2use==NULL)mode=1;
		else{
			rbent.restype=RT3DPOINT;
			ic_ptcpy(rbent.resval.rpoint,extru2use);
		}
	}
	SDS_getvar(NULL,DB_QVIEWDIR,&rbtemp,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

	if(mode!=1){
		rbucs.restype=RTSHORT;
		rbucs.resval.rint=1;
		sds_trans(rbtemp.resval.rpoint,&rbucs,&rbent,1,rbtemp.resval.rpoint);
	}
	if(icadRealEqual(rbtemp.resval.rpoint[2],0.0))return;

	pt[0]+=rbtemp.resval.rpoint[0]*(zval-pt[2])/rbtemp.resval.rpoint[2];
	pt[1]+=rbtemp.resval.rpoint[1]*(zval-pt[2])/rbtemp.resval.rpoint[2];
	pt[2]=zval;
	return;
}

//===========================================================================
void cmd_unit_vector(sds_point pt1,sds_point pt2,sds_point vpt) {
    //takes two points and makes a vector then unitizes them
    cmd_pt_sub(pt2,pt1,vpt);
    cmd_rpt_div(vpt,sqrt(pow(vpt[0],2)+pow(vpt[1],2)+pow(vpt[2],2)),vpt);
}
//===========================================================================

short cmd_pt_add(sds_point pt1,sds_point pt2,sds_point pt3) {
	pt3[0]=pt1[0]+pt2[0];
	pt3[1]=pt1[1]+pt2[1];
	pt3[2]=pt1[2]+pt2[2];
	return(RTNORM);
}

short cmd_pt_sub(sds_point pt1,sds_point pt2,sds_point pt3) {
	pt3[0]=pt1[0]-pt2[0];
	pt3[1]=pt1[1]-pt2[1];
	pt3[2]=pt1[2]-pt2[2];
	return(RTNORM);
}
	

short cmd_rpt_mul(sds_point pt1,sds_real r,sds_point pt2) {
	pt2[0]=pt1[0]*r;
	pt2[1]=pt1[1]*r;
	pt2[2]=pt1[2]*r;
	return(RTNORM);
}

short cmd_rpt_div(sds_point pt1,sds_real r,sds_point pt2) {
	if (icadRealEqual(r,0.0)) return(RTERROR);

	pt2[0]=pt1[0]/r;
	pt2[1]=pt1[1]/r;
	pt2[2]=pt1[2]/r;
	return(RTNORM);
}
