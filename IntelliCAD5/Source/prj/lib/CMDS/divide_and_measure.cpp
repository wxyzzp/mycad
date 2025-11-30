/* DIVIDE_AND_MEASURE.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * Supports the divide and measure commands
 *
 */
// THIS FILE HAS BEEN GLOBALIZED!
//** Includes
#include "cmds.h"/*DNT*/
#include "Icad.h" /*DNT*/
#include "IcadApi.h"
#include "IcadDoc.h"

short cmd_divide(void) {
//*** This function places evenly spaced point entities or blocks along the length or
//*** perimeter of an entity.

    if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    char fs1[IC_ACADBUF],bname[IC_PATHSIZE+IC_FILESIZE],*fcp1;
    struct resbuf *ptlist,*tmprb;
    sds_point pt1,ptarray[2];
    sds_name ename,elast,etemp;
    long fl1;
	int fint,ret=RTNORM;
    RT	rc=0;
	int	mode;

    ptlist=NULL; rc=0; *bname=0;
	tmprb=NULL;
	repick:
	do{
		if(tmprb!=NULL){sds_relrb(tmprb);tmprb=NULL;}
		if((fint=sds_pmtssget(ResourceString(IDC_DIVIDE_AND_MEASURE__NSEL_0, "\nSelect entity to divide: " ),""/*DNT*/,&tmprb,NULL,NULL,etemp,1))!=RTNORM)return(-1);
		sds_sslength(etemp,&fl1);
		if(fl1==0L)
			sds_ssfree(etemp);
	}while(fl1==0L);
	if(fl1>1){
		for(fint=1;sds_ssname(etemp,fint,ename)==RTNORM;fint++){
			sds_redraw(ename,IC_REDRAW_UNHILITE);
		}
	}
  	if(sds_ssname(etemp,0L,ename)!=RTNORM)return(-1);
	sds_ssfree(etemp);
	if(tmprb!=NULL){sds_relrb(tmprb);tmprb=NULL;}
	if((tmprb=sds_entget(ename))==NULL)return(-1);
	ic_assoc(tmprb,0);
	strncpy(fs1,ic_rbassoc->resval.rstring,sizeof(fs1)-1);
	if(tmprb!=NULL){sds_relrb(tmprb);tmprb=NULL;}
	if(sds_wcmatch(fs1,"LINE,ARC,CIRCLE,ELLIPSE,SPLINE,POLYLINE,LWPOLYLINE"/*DNT*/)!=RTNORM){
		cmd_ErrorPrompt(CMD_ERR_LINEARCCIRPLY,0);
		goto repick;
	}

    for(;;) {
        if(!*bname){
			strcpy(fs1,ResourceString(IDC_DIVIDE_AND_MEASURE__NBLO_1, "\nBlocks/<Number of segments>: " ));
			if(sds_initget(SDS_RSG_OTHER,ResourceString(IDC_DIVIDE_AND_MEASURE_INITGET_2, "Insert_blocks|Block ~_Block" ))!=RTNORM) { 
                rc=(-2); 
                goto bail; 
            }
        }else{
			strcpy(fs1,ResourceString(IDC_DIVIDE_AND_MEASURE__NNUM_3, "\nNumber of segments: " ));
           if(sds_initget(SDS_RSG_NOZERO+SDS_RSG_NONEG,NULL)!=RTNORM) {
               rc=(-2);
               goto bail;
            }
		}
        if((ret=sds_getint(fs1,&fint))==RTERROR) {
            rc=(-2); goto bail;
        } else if(ret==RTCAN) {
            rc=(-1); goto bail;
        } else if(ret==RTNORM) {
            if(fint<2 || fint>32767) {
                if(!*bname) cmd_ErrorPrompt(CMD_ERR_SEGSKWORD,0);
                else cmd_ErrorPrompt(CMD_ERR_DIVSEGS,0);
                continue;
            }
            if(!*bname) mode=8;
            break;
        } else if((ret==RTSTR || ret==RTKWORD) && !*bname) {
            if(sds_getinput(fs1)!=RTNORM) { rc=(-2); goto bail; }
            if(strisame(fs1,"BLOCK"/*DNT*/)) {
                for(;;) {
                    if((ret=sds_getstring(0,ResourceString(IDC_DIVIDE_AND_MEASURE__NNAM_5, "\nName of block to insert: " ),fs1))==RTERROR) {
                        rc=(-2); goto bail;
                    } else if(ret==RTCAN) {
                        rc=(-1); goto bail;
                    } else if(ret==RTNORM) {
						//*** ~| TODO - There may need to be more checking put in here if it needs to work just like insert.
                       if((tmprb=sds_tblsearch("BLOCK"/*DNT*/,fs1,0))==NULL) {
							strncpy(bname,fs1,sizeof(bname)-1);
							ic_setext(fs1,"DWG"/*DNT*/);
							if(RTERROR==cmd_ReadAsBlock(fs1,NULL,IC_INSMODE_INSERT,NULL)) {
								sds_printf(ResourceString(IDC_DIVIDE_AND_MEASURE__NCOU_6, "\nCould not find block %s." ),bname);
								*bname=0;

//TODO 				cmd_ErrorPrompt(CMD_ERR_FINDBLOCK,0);
// I could put this here and not tell the user the block name they just entered, or change cmd_ErrorPrompt() to accept
// another argument, or just leave it alone.
								continue;
							}else{
								ic_setext(bname,""/*DNT*/);
								if((fcp1=strrchr(bname,IC_SLASH))!=NULL){
									*fcp1=0;
									strncpy(bname,fcp1+1,sizeof(bname)-1);
								}

							}
						} else strncpy(bname,fs1,sizeof(bname)-1);
						IC_RELRB(tmprb);



                        for(;;) {
							sds_initget(SDS_RSG_OTHER,ResourceString(IDC_DIVIDE_AND_MEASURE_INITGET_7, "Yes-Align_blocks|Yes No-Do_not_align|No ~_Yes ~_No" ));
                            ret=sds_getkword(ResourceString(IDC_DIVIDE_AND_MEASURE__NALI_8, "\nAlign blocks with entity? <Y>: " ),fs1);
							if (ret==RTERROR || ret==RTCAN) {
                                rc=ret; goto bail;
							} else if (ret==RTNONE) {
								mode=4;
                            } else if (ret==RTNORM) {
                                if (strisame(fs1,"YES"/*DNT*/)) {
                                    mode=4;
                                } else if (strisame(fs1,"NO"/*DNT*/)) {
                                    mode=0;
                                } else {
									cmd_ErrorPrompt(CMD_ERR_YESNO,0);  //"Yes or No, please."
                                    continue;
                                }
                            }
							break;
                        }
                        break;
                    }
                }
                continue;
            } else {
				cmd_ErrorPrompt(CMD_ERR_SEGSKWORD,0);
            }
        } else {
            if(!*bname) cmd_ErrorPrompt(CMD_ERR_SEGSKWORD,0);
            else cmd_ErrorPrompt(CMD_ERR_DIVSEGS,0);
            continue;
        }
    }
	//find last main or subent in dwg...
	if(sds_entlast(elast)!=RTNORM){rc=-1;goto bail;}
	for(;sds_entnext_noxref(elast,etemp)==RTNORM;ic_encpy(elast,etemp));

	//find out if we want to measure from the start or the end of the ent
	db_getendpts(ename,ptarray);
	if(sds_distance(pt1,ptarray[0])>sds_distance(pt1,ptarray[1]))mode+=64;
    rc=ic_divmeas(ename,fint,bname,1,0,32767,mode,&ptlist);
	if(rc==0){
		//set selection set of objects as previous set....
		sds_ssfree(SDS_CURDOC->m_pPrevSelection);
		SDS_ssadd(NULL, NULL, SDS_CURDOC->m_pPrevSelection);
		for( ; sds_entnext_noxref(elast, elast) == RTNORM;
			   SDS_ssadd(elast, SDS_CURDOC->m_pPrevSelection, SDS_CURDOC->m_pPrevSelection))
			;
	}


    bail:

    sds_redraw(ename,IC_REDRAW_UNHILITE);
    IC_RELRB(ptlist);
    return(rc);
}

short cmd_measure(void) {
//*** This function places point entities or blocks at measured intervals on an
//*** entity.
//*** This function is done except:
//***      The measuring entities are placed starting at the starting point of the
//***      entity, instead of the end nearest to the point returned by sds_entsel().
//*** Both of this change will need to be made to the ic_divmeas() function.
//***

    if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    char fs1[IC_ACADBUF],bname[IC_PATHSIZE+IC_FILESIZE],*fcp1;
    struct resbuf *ptlist,*tmprb,*rbp1;
    sds_point pt1,ptarray[2];
    sds_name ename,elast,etemp;
    sds_real fr1;
    RT	rc=0,ret=RTNORM;
	long fl1;
    int mode=0,closed;

    ptlist=NULL; rc=0; *bname=0;

	tmprb=rbp1=NULL;
	repick:
	do{
		if(tmprb!=NULL){sds_relrb(tmprb);tmprb=NULL;}
		if(sds_pmtssget(ResourceString(IDC_DIVIDE_AND_MEASURE__NSE_11, "\nSelect entity to measure: " ),""/*DNT*/,&tmprb,NULL,NULL,etemp,1)!=RTNORM)return(-1);
		sds_sslength(etemp,&fl1);
		if(fl1==0L)sds_ssfree(etemp);
	}while(fl1==0L);
	for(fl1=1;sds_ssname(etemp,fl1,ename)==RTNORM;fl1++){
		sds_redraw(ename,IC_REDRAW_UNHILITE);
	}
  	if(sds_ssname(etemp,0L,ename)!=RTNORM)return(-1);
	sds_ssfree(etemp);
	if(rbp1!=NULL){sds_relrb(rbp1);rbp1=NULL;}
	if((rbp1=sds_entget(ename))==NULL)return(-1);
	ic_assoc(rbp1,0);
	strncpy(fs1,ic_rbassoc->resval.rstring,sizeof(fs1)-1);
	if(sds_wcmatch(fs1,"LINE,ARC,CIRCLE,ELLIPSE,SPLINE,POLYLINE,LWPOLYLINE"/*DNT*/)!=RTNORM){
		cmd_ErrorPrompt(CMD_ERR_LINEARCCIRPLY,0);
		goto repick;
	}
	if(strsame(fs1,"POLYLINE"/*DNT*/) || strsame(fs1,"LWPOLYLINE"/*DNT*/)){
		ic_assoc(rbp1,70);
		closed=ic_rbassoc->resval.rint & IC_PLINE_CLOSED;
	}
	else if(strsame(fs1,"CIRCLE"/*DNT*/))closed=1;
	else closed=0;
	if(rbp1!=NULL){sds_relrb(rbp1);rbp1=NULL;}
	if(tmprb!=NULL){
		if(tmprb->restype==RTSTR && (strisame(tmprb->resval.rstring,"S"/*DNT*/))) {
			ic_ptcpy(pt1,tmprb->rbnext->resval.rpoint);
		}else{
		    SDS_SetCursor(SDS_GetPickCursor());
			if(strsame(fs1,"POLYLINE"/*DNT*/) && closed!=0){
				sds_initget(0,ResourceString(IDC_DIVIDE_AND_MEASURE_INITGET_13, "Measure_from_start|Start Measure_from_end|End ~_Start ~_End" ));
                ret=sds_getkword(ResourceString(IDC_DIVIDE_AND_MEASURE__NME_14, "\nMeasure polyline from End/<Start>: " ),fs1);
				if(ret==RTNORM){
					if (strisame(fs1,"END"/*DNT*/))
						mode+=64;
				}else if(ret!=RTNONE) return(-1);
			}
			if(!closed){
				if(internalGetpoint(NULL,ResourceString(IDC_DIVIDE_AND_MEASURE__NSE_16, "\nSelect end from which to begin measuring: " ),pt1)!=RTNORM){
					SDS_SetCursor(IDC_ICAD_CROSS);
					if(tmprb!=NULL){sds_relrb(tmprb);tmprb=NULL;}
					return(-1);
				}
			}
		    SDS_SetCursor(IDC_ICAD_CROSS);
			sds_redraw(ename,IC_REDRAW_UNHILITE);
		}
		if(tmprb!=NULL){sds_relrb(tmprb);tmprb=NULL;}
	}

    for(;;) {
        if(!*bname){
			strcpy(fs1,ResourceString(IDC_DIVIDE_AND_MEASURE__NBL_17, "\nBlock/<Segment length>: " ));
			if(sds_initget(SDS_RSG_NOZERO+SDS_RSG_NONEG+SDS_RSG_OTHER,ResourceString(IDC_DIVIDE_AND_MEASURE_INITGET_2, "Insert_blocks|Block ~_Block" ))!=RTNORM) { 
                rc=(-2); 
                goto bail; 
            }
        }else{
           if(sds_initget(SDS_RSG_NOZERO+SDS_RSG_NONEG,NULL)!=RTNORM) {
               rc=(-2);
               goto bail;
            }
			strcpy(fs1,ResourceString(IDC_DIVIDE_AND_MEASURE__NSE_18, "\nSegment length: " ));
		}
        if((ret=sds_getdist(NULL,fs1,&fr1))==RTERROR) {
            rc=(-2); goto bail;
        } else if(ret==RTCAN) {
            rc=(-1); goto bail;
        } else if(ret==RTNORM) {
            if(!*bname) mode=9;
            break;
        } else if((ret==RTSTR || ret==RTKWORD) && !*bname) {
            if(sds_getinput(fs1)!=RTNORM) { rc=(-2); goto bail; }
            if(strisame(fs1,"BLOCK"/*DNT*/)) {
                for(;;) {
                    if((ret=sds_getstring(0,ResourceString(IDC_DIVIDE_AND_MEASURE__N___19, "\n? to list blocks in drawing/<Block to insert>: " ),fs1))==RTERROR) {
                        rc=(-2); goto bail;
                    } else if(ret==RTCAN) {
                        rc=(-1); goto bail;
                    } else if(ret==RTNORM) {
						if(strisame("?"/*DNT*/,fs1) || strisame("_?"/*DNT*/,fs1)) {
							if(0!=cmd_block_list(0)){
								goto bail;
							}else{
								continue;
							}
						}
						//*** ~| TODO - There may need to be more checking put in here if it needs to work just like insert.
						//*** ~| TODO -	We need to give user a scrolling list of blocks in a dialog
                       if((tmprb=sds_tblsearch("BLOCK"/*DNT*/,fs1,0))==NULL) {
							strncpy(bname,fs1,sizeof(bname)-1);
							ic_setext(fs1,"DWG"/*DNT*/);
							if(RTERROR==cmd_ReadAsBlock(fs1,NULL,IC_INSMODE_INSERT,NULL)) {
								sds_printf(ResourceString(IDC_DIVIDE_AND_MEASURE__NCOU_6, "\nCould not find block %s." ),bname);
								*bname=0;
								continue;
							}else{
								ic_setext(bname,""/*DNT*/);
								if((fcp1=strrchr(bname,IC_SLASH))!=NULL){
									*fcp1=0;
									strncpy(bname,fcp1+1,sizeof(bname)-1);
								}
							}
						} else strncpy(bname,fs1,sizeof(bname)-1);
						IC_RELRB(tmprb);


                        for(;;) {
							sds_initget(SDS_RSG_OTHER,ResourceString(IDC_DIVIDE_AND_MEASURE_INITGET_7, "Yes-Align_blocks|Yes No-Do_not_align|No ~_Yes ~_No" ));
                            ret=sds_getkword(ResourceString(IDC_DIVIDE_AND_MEASURE__NALI_8, "\nAlign blocks with entity? <Y>: " ),fs1);
							if (ret==RTERROR || ret==RTCAN) {
                                rc=ret; goto bail;
							} else if (ret==RTNONE) {
								mode=5;
                            } else if (ret==RTNORM) {
                                if (strisame(fs1,"YES"/*DNT*/)) {
                                    mode=5;
                                } else if (strisame(fs1,"NO"/*DNT*/)) {
                                    mode=1;
                                } else {
									cmd_ErrorPrompt(CMD_ERR_YESNO,0);  //"Yes or No, please."
                                    continue;
                                }
                            }
                            break;
                        }
                        break;
                    }
                }
                continue;
            } else {
                cmd_ErrorPrompt(CMD_ERR_DIST2PTSKWORD,0);
            }
        } else {
            if(!*bname) cmd_ErrorPrompt(CMD_ERR_DIST2PTSKWORD,0);
            else cmd_ErrorPrompt(CMD_ERR_DIST2PTS,0);
            continue;
        }
    }


	//find last main or subent in dwg...
	if(sds_entlast(elast)!=RTNORM){rc=-1;goto bail;}
	for(;sds_entnext_noxref(elast,etemp)==RTNORM;ic_encpy(elast,etemp));

	//find out if we want to measure from the start or the end of the ent
	if(!closed && db_getendpts(ename,ptarray)==0){
		if(sds_distance(pt1,ptarray[0])>sds_distance(pt1,ptarray[1]))mode+=64;
	}

    rc=ic_divmeas(ename,fr1,bname,1,0,0,mode,&ptlist);
	if(rc==0){
		//set selection set of objects as previous set....
		sds_ssfree(SDS_CURDOC->m_pPrevSelection);
		SDS_ssadd(NULL, NULL, SDS_CURDOC->m_pPrevSelection);
		for( ; sds_entnext_noxref(elast, elast) == RTNORM;
			   SDS_ssadd(elast, SDS_CURDOC->m_pPrevSelection, SDS_CURDOC->m_pPrevSelection));
	}

    bail:

    sds_redraw(ename,IC_REDRAW_UNHILITE);
    IC_RELRB(ptlist);
    return(rc);
}


