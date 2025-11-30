/* 3DFACE.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * Supports the 3dface
 *
 */
// THIS FILE HAS BEEN GLOBALIZED!
//** Includes
#include "cmds.h"/*DNT*/
#include "Icad.h" /*DNT*/
#include "IcadApi.h"



short cmd_3dface(void) {
//*** This function creates three-dimensional faces.
//***
//*** RETURNS:  0 - Success.
//***          -1 - User cancel.
//***          -2 - RTERROR returned from an ads function.
//***

    if (cmd_iswindowopen() == RTERROR) return RTERROR;
	
	//BugZilla No. 78050; 25-03-2002
	if (cmd_istransparent() == RTERROR) return RTERROR;

	char fs1[IC_ACADBUF];
    struct resbuf *newent;
    sds_point pt[4];
	sds_name ename;
    int ret,rc,iflg,iflg2,rflg;
	short onedrawn=0;				// 0 if no faces drawn yet, 1 if there is one (or more) so "Done" shows up in prompt box.
	db_handitem *elp=NULL;
	struct resbuf rb,rbucs,rbwcs;

    rc=iflg=rflg=0; newent=NULL;
	rbucs.restype=rbwcs.restype=RTSHORT;
	rbucs.resval.rint=1;rbwcs.resval.rint=0;

    //*** First point.
    for(;;) {
        if(iflg<1) {
				_tcscpy(fs1,ResourceString(IDC_3DFACE__NINVISIBLE_EDGE__0, "\nInvisible edge/<First point of 3D face>: " ));
	        if(sds_initget(0,ResourceString(IDC_3DFACE_INITGET_INVISIBLE_1, "Invisible_edge|Invisible ~_Invisible" ))!=RTNORM) { 
                rc=(-2); 
                goto bail; 
            }
		} else strcpy(fs1,ResourceString(IDC_3DFACE__NFIRST_POINT_OF__2, "\nFirst point of 3D face: " ));
        if((ret=internalGetpoint(NULL,fs1,pt[0]))==RTERROR) {
            rc=(-2); goto bail;
        } else if(ret==RTCAN) {
            rc=(-1); goto bail;
        } else if(ret==RTNONE) {
            goto bail;
        } else if(ret==RTKWORD) {
            if(iflg<1) iflg+=1;
        } else if(ret==RTNORM) {
            break;
        }
    }

    //*** Second point.
    for(;;) {
        if(iflg<2) {
			strcpy(fs1,ResourceString(IDC_3DFACE__NINVISIBLE_EDGE__3, "\nInvisible edge/<Second point>: " ));
	        if(sds_initget(SDS_RSG_NONULL,ResourceString(IDC_3DFACE_INITGET_INVISIBLE_1, "Invisible_edge|Invisible ~_Invisible" ))!=RTNORM) { 
                rc=(-2); 
                goto bail; 
            }
		} else strcpy(fs1,ResourceString(IDC_3DFACE__NSECOND_POINT___4, "\nSecond point: " ));
		ret=internalGetpoint(pt[0],fs1,pt[1]);
        if(ret==RTERROR || ret==RTCAN) {
            rc=(ret); goto bail;
        } else if(ret==RTKWORD) {
            if(iflg<2) iflg+=2;
            continue;
        } else if(ret==RTNORM) {
            break;
        }
    }


    for(;;) {
		//*** Build a parlink for dragging.

		if(elp) delete elp; elp=NULL;
		elp=new db_3dface(SDS_CURDWG);
		elp->set_10(pt[0]);
		elp->set_11(pt[1]);
		elp->set_12(pt[1]);
		elp->set_13(pt[0]);

		sds_trans(pt[0],&rbucs,&rbwcs,0,elp->retp_10());
		sds_trans(pt[1],&rbucs,&rbwcs,0,elp->retp_11());
		ic_ptcpy(elp->retp_13(),elp->retp_10());

		ename[0]=(long)elp;
		ename[1]=(long)SDS_CURDWG;

		SDS_getvar(NULL,DB_QTHICKNESS,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		elp->set_39(rb.resval.rreal);

		SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

        //*** Third point.
        for(;;) {
            if(iflg<4) {
				strcpy(fs1,ResourceString(IDC_3DFACE__NINVISIBLE_EDGE__5, "\nInvisible edge/<Third point of 3D face>: " ));
				if (onedrawn==0) {
					if(sds_initget(0,ResourceString(IDC_3DFACE_INITGET_INVISIBLE_1, "Invisible_edge|Invisible ~_Invisible" ))!=RTNORM) { 
                        rc=(-2); 
                        goto bail; 
                    }
				} else{
             	    if(sds_initget(SDS_RSG_NOCANCEL,ResourceString(IDC_3DFACE_INITGET_INVISIBLE_6, "Invisible_edge|Invisible ~ ` ~_Invisible ~_" ))!=RTNORM) { 
                        rc=(-2); 
                        goto bail; 
                    }
				}
		    } else {
				strcpy(fs1,ResourceString(IDC_3DFACE__NTHIRD_POINT_OF__7, "\nThird point of 3D face: " ));
			}
			//if both pts are equal, we're "double dragging" across the face,so it doesn't show
			if(!icadPointEqual(pt[0],pt[1])){
				elp->set_12(elp->retp_11());
				ret=SDS_dragobject(-1,rb.resval.rint,(sds_real *)ename,(double *)elp->retp_12(),0.0,fs1,NULL,elp->retp_12(),NULL);
				if(RTNORM==ret)sds_trans(elp->retp_12(),&rbwcs,&rbucs,0,pt[2]);
			}else {
				ret=internalGetpoint(pt[0],fs1,pt[2]);
				if(RTNORM==ret)sds_trans(pt[2],&rbucs,&rbwcs,0,elp->retp_12());
			}
            if(ret==RTERROR || ret==RTCAN) {
                rc=(ret); goto bail;
            } else if(ret==RTNONE) {
                goto bail;
            } else if(ret==RTKWORD) {
                if(iflg<4) iflg+=4;
                continue;
            } else if(ret==RTNORM) {
                break;
            }
        }
        //*** Fourth point.
        for(;;) {
            if(iflg<8) {
				sprintf(fs1,ResourceString(IDC_3DFACE__NINVISIBLE_EDGE__8, "\nInvisible edge/<Last point>: " ));
				if(sds_initget(0,ResourceString(IDC_3DFACE_INITGET_INVISIBLE_6, "Invisible_edge|Invisible ~ ` ~_Invisible ~_" ))!=RTNORM) { 
                    rc=(-2); 
                    goto bail; 
                }
			} else strcpy(fs1,ResourceString(IDC_3DFACE__NLAST_POINT___9, "\nLast point: " ));

			ret=SDS_dragobject(-1,rb.resval.rint,(sds_real *)ename,(double *)elp->retp_13(),0.0,fs1,NULL,pt[2],NULL);
			elp->set_13(pt[2]);
            if(ret==RTERROR || ret==RTCAN) {
                rc=(ret); goto bail;
            } else if(ret==RTNONE) {
				elp->set_13(elp->retp_12());
                iflg2=0;
                break;
            } else if(ret==RTKWORD) {
                if(iflg<8) iflg+=8;
                continue;
            } else if(ret==RTNORM) {
				sds_trans(elp->retp_13(),&rbucs,&rbwcs,0,elp->retp_13());
                //*** Check to see if side 3 is invisible.
                if(iflg & 4) iflg2=1;
                else iflg2=0;
                break;
            }
        }
        //*** Make entity.
        if((newent=sds_buildlist(RTDXF0,"3DFACE"/*DNT*/,10,elp->retp_10(),11,elp->retp_11(),12,elp->retp_12(),13,elp->retp_13(),70,iflg,0))==NULL) {
            cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
            rc=(-2); goto bail;
        }
        if(sds_entmake(newent)!=RTNORM) { rc=(-2); goto bail; }
        IC_RELRB(newent);
		sds_trans(elp->retp_12(),&rbwcs,&rbucs,0,pt[0]);
		sds_trans(elp->retp_13(),&rbwcs,&rbucs,0,pt[1]);
		elp->set_70(1);
        rflg=1; iflg=iflg2;
		onedrawn=1;
    }

    bail:

	if(elp) delete elp; elp=NULL;
    IC_RELRB(newent);
    return(RT(rc));
}

