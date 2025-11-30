/* SOLID_AND_TRACE.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * Supports the solid and trace commands
 *
 */
// THIS FILE HAS BEEN GLOBALIZED!
//** Includes
#include "cmds.h"/*DNT*/
#include "Icad.h" /*DNT*/
#include "IcadApi.h"


// **********
// GLOBALS
//
extern struct cmd_drag_globalpacket cmd_drag_gpak;	//structure def moved to header file...
extern double SDS_CorrectElevation;
extern bool	  SDS_ElevationChanged;
extern sds_point SDS_PointDisplay;


// *************************
// HELPER FUNCTION
//
static int
cmd_tracejoint(sds_point p0, sds_point p1, sds_point p2, sds_real width,
    sds_point j0, sds_point j1) {
/*
**  Determines the points where two consecutive TRACE entities meet,
**  given the three points defining the two TRACEs, and the width.
**
**  Given:
**       p0 : Beginning of 1st TRACE
**       p1 : End of 1st TRACE and beginning of 2nd TRACE
**       p2 : End of 2nd TRACE
**    width : TRACE width
**
**  Sets:
**       j0 : 3rd pt of 1st TRACE
**       j1 : 4th pt of 1st TRACE
**
**  Assumes ACAD point order: the 1st and 3rd defining points lie
**  to the left of the TRACE direction vector; the 2nd and 4th lie
**  to the right.
**
**  Note that 135 deg (0.75*pi) is the division between the two
**  joining scenarios.
**
**  j0 and j1 may refer to the same points in memory as p0, p1, and p2.
**
**  Returns:
**     +1 : OK; 2nd TRACE starts with j1 and j0 (large deflection scenario)
**      0 : OK; 2nd TRACE starts with j0 and j1 (small deflection scenario)
**     -1 : Calling error (something NULL)
*/
    short rc,fi1;
    sds_real vdir[2][2],jdir[2],ar1;


    rc=0;

    if (p0==NULL || p1==NULL || p2==NULL || j0==NULL || j1==NULL)
        { rc=-1; goto out; }

    j0[2]=j1[2]=p0[2];

    /* Get the direction vectors: */
    vdir[0][0]=p1[0]-p0[0]; vdir[0][1]=p1[1]-p0[1];
    vdir[1][0]=p2[0]-p1[0]; vdir[1][1]=p2[1]-p1[1];

    /* Get unit direction vectors (use (1,0) if pts coincident): */
    for (fi1=0; fi1<2; fi1++) {
        if (icadRealEqual((ar1=sqrt(vdir[fi1][0]*vdir[fi1][0]+vdir[fi1][1]*vdir[fi1][1])),0.0))
             { vdir[fi1][0]=1.0; vdir[fi1][1]=0.0; }
        else { vdir[fi1][0]/=ar1; vdir[fi1][1]/=ar1; }
    }

    /* Set unit junction vector to the direction of the sum of the */
    /* unit direction vectors (if the 2nd TRACE exactly reverses the first, */
    /* use a direction 90 deg from the 1st vector): */
    jdir[0]=vdir[0][0]+vdir[1][0]; jdir[1]=vdir[0][1]+vdir[1][1];
    if (icadRealEqual((ar1=sqrt(jdir[0]*jdir[0]+jdir[1]*jdir[1])),0.0))
         { jdir[0]=-vdir[0][1]; jdir[1]=vdir[0][0]; }
    else { jdir[0]/=ar1; jdir[1]/=ar1; }

    /* If the angle between the 2 direction vectors is <= 135 deg, */
    /* rotate the junction vector 90 deg.  Otherwise, leave it, */
    /* but set the return value to +1: */
    if (vdir[0][0]*vdir[1][0]+vdir[0][1]*vdir[1][1]>=cos(0.75*IC_PI))
        { ar1=jdir[0]; jdir[0]=-jdir[1]; jdir[1]=ar1; }
    else rc=1;

    /* Use cross product to get the sine of the angle between the 1st */
    /* direction vector and the junction vector, and to make sure */
    /* the junction vector points to the left of the 1st direction vector: */
    if ((ar1=vdir[0][0]*jdir[1]-vdir[0][1]*jdir[0])<0.0)
        { jdir[0]=-jdir[0]; jdir[1]=-jdir[1]; ar1=-ar1; }

    /* ar1 is now the sine of the angle between the 1st direction vector */
    /* and the junction vector.  Set it to the offset distance from p1. */
    /* Note the div-by-0 precaution (which should never occur): */
    if (width<0.0) width=-width;
    ar1=0.5*width/((icadRealEqual(ar1,0.0)) ? 1.0 : ar1);

    /* Now ar1 is the offset distance from p1.  Set j0 and j1: */
    j0[0]=p1[0]+ar1*jdir[0]; j0[1]=p1[1]+ar1*jdir[1];
    j1[0]=p1[0]-ar1*jdir[0]; j1[1]=p1[1]-ar1*jdir[1];

out:
    return rc;
}


short cmd_solid(void) {
//*** This function creates solid-filled polygons.
//***
//*** RETURNS:  0 - Success.
//***          -1 - User cancel.
//***          -2 - RTERROR returned from an ads function.
//***

    if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	char fs1[IC_ACADBUF];
	struct resbuf *newent,setgetrb,*rslt,rbucs,rbecs,*rb10,*rb11,*rb12,*rb13;
    sds_point pt[4],ptemp,extru;
    sds_real curelev,newelev;
    RT ret,rc,mode=0;
	sds_name ename;
	db_handitem *elp=NULL;

	//NOTE: mode&1 is for Rectangle, mode&2 for Square, mode&4 for Triangle

    rc=0; newent=rslt=NULL; setgetrb.rbnext=NULL;

	rbucs.restype=RTSHORT;
	rbecs.restype=RT3DPOINT;
	rbucs.resval.rint=1;
    if(SDS_getvar(NULL,DB_QUCSXDIR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-2); goto bail; }
	ic_ptcpy(pt[0],setgetrb.resval.rpoint);
    if(SDS_getvar(NULL,DB_QUCSYDIR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-2); goto bail; }
	rbecs.resval.rpoint[0]=(pt[0][1]*setgetrb.resval.rpoint[2])-(setgetrb.resval.rpoint[1]*pt[0][2]);
	rbecs.resval.rpoint[1]=(pt[0][2]*setgetrb.resval.rpoint[0])-(setgetrb.resval.rpoint[2]*pt[0][0]);
	rbecs.resval.rpoint[2]=(pt[0][0]*setgetrb.resval.rpoint[1])-(setgetrb.resval.rpoint[0]*pt[0][1]);
	ic_ptcpy(extru,rbecs.resval.rpoint);


    if(SDS_getvar(NULL,DB_QELEVATION,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-2); goto bail; }
    curelev=newelev=pt[0][2]=setgetrb.resval.rreal;
    //*** First point.
    for(;;) {
		if(mode==0){
			if(sds_initget(0,ResourceString(IDC_SOLID_AND_TRACE_INITGET__0, "Rectangle Square Triangle ~_Rectangle ~_Square ~_Triangle" ))!=RTNORM) { 
                rc=(-2); 
                goto bail; 
            }
			ret=internalGetpoint(NULL,ResourceString(IDC_SOLID_AND_TRACE__NRECTAN_1, "\nRectangle/Square/Triangle/<First point of plane>: " ),pt[0]);
		}else{
			if(sds_initget(0,NULL)!=RTNORM) { 
                rc=(-2); 
                goto bail; 
            }
			ret=internalGetpoint(NULL,ResourceString(IDC_SOLID_AND_TRACE__NFIRST__2, "\nFirst point of plane: " ),pt[0]);
		}
		if(ret==RTERROR){
            rc=(-2); goto bail;
        } else if(ret==RTCAN) {
            rc=(-1); goto bail;
        } else if(ret==RTNONE) {
            goto bail;
		} else if(ret==RTKWORD) {
			sds_getinput(fs1);
			if(strisame(fs1,"RECTANGLE"/*DNT*/))
                mode=1;
			else if(strisame(fs1,"SQUARE"/*DNT*/))
                mode=2;
			else if(strisame(fs1,"TRIANGLE"/*DNT*/))
                mode=4;
			else goto bail;
        } else if(ret==RTNORM) {
            //*** Set the Z coordinates for the rest of the points.
            if(!icadRealEqual(pt[0][2],curelev)) {
				SDS_CorrectElevation=curelev;
				SDS_ElevationChanged=TRUE;
				newelev=setgetrb.resval.rreal=pt[0][2];
				setgetrb.restype=RTREAL;
				sds_setvar("ELEVATION"/*DNT*/,&setgetrb);
			}
            break;
        }
    }

	if(mode){
		SDS_getvar(NULL,DB_QDRAGMODE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
        if((newent=sds_buildlist(RTDXF0,"SOLID"/*DNT*/,10,pt[0],11,pt[1],12,pt[2],13,pt[3],210,extru,0))==NULL) {
            cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
            rc=(-2); goto bail;
        }
		if(rslt!=NULL)sds_relrb(rslt);
		for(rslt=newent;rslt!=NULL;rslt=rslt->rbnext){
			if(rslt->restype==10)rb10=rslt;
			else if(rslt->restype==11)rb11=rslt;
			else if(rslt->restype==12)rb12=rslt;
			else if(rslt->restype==13)rb13=rslt;
		}
	}
	if(mode&4){//triangle mode
		for(;;){
			IC_RELRB(rslt);
			if((ret=SDS_dragobject(19,setgetrb.resval.rint,pt[0],pt[0],(sds_real)3.0,ResourceString(IDC_SOLID_AND_TRACE__NSECOND_6, "\nSecond corner of triangle: " ),&rslt,pt[1],NULL))==RTERROR) {
				rc=(-2); goto bail;
			} else if(ret==RTCAN) {
				rc=(-1); goto bail;
			} else if(ret==RTNONE) {
				rc=0; goto bail;
			} else if(ret==RTNORM) {
				//1st resbuf is the one we want.  2nd and 3rd are already defined
				//by pt[0] and pt[1]
				if(!icadRealEqual(pt[1][2],newelev)){
					cmd_ErrorPrompt(CMD_ERR_2DPT,0);
					continue;
				}
				if(rslt==NULL){rc=-2;goto bail;}
				/*ic_assoc(newent,10);
				ic_ptcpy(ic_rbassoc->resval.rpoint,pt[0]);
				ic_assoc(newent,11);
				ic_ptcpy(ic_rbassoc->resval.rpoint,pt[1]);
				ic_assoc(newent,12);
				ic_ptcpy(ic_rbassoc->resval.rpoint,rslt->resval.rpoint);
				ic_assoc(newent,13);
				ic_ptcpy(ic_rbassoc->resval.rpoint,rslt->resval.rpoint);*/
				sds_trans(rslt->resval.rpoint,&rbucs,&rbecs,0,rb10->resval.rpoint);
				sds_trans(rslt->rbnext->resval.rpoint,&rbucs,&rbecs,0,rb11->resval.rpoint);
				sds_trans(rslt->rbnext->rbnext->resval.rpoint,&rbucs,&rbecs,0,rb12->resval.rpoint);
				ic_ptcpy(rb13->resval.rpoint,rb12->resval.rpoint);
				if(sds_entmake(newent)!=RTNORM) { rc=(-2); goto bail; }
				ic_ptcpy(pt[0],pt[1]);
			}
		}
	}else if(mode&2){//square mode
		for(;;){
			IC_RELRB(rslt);
			if((ret=SDS_dragobject(19,setgetrb.resval.rint,pt[0],pt[0],(sds_real)4.0,ResourceString(IDC_SOLID_AND_TRACE__NSECOND_7, "\nSecond corner of square: " ),&rslt,pt[1],NULL))==RTERROR) {
				rc=(-2); goto bail;
			} else if(ret==RTCAN) {
				rc=(-1); goto bail;
			} else if(ret==RTNONE){
				rc=0; goto bail;
			} else if(ret==RTNORM) {
				if(!icadRealEqual(pt[1][2],newelev)){
					cmd_ErrorPrompt(CMD_ERR_2DPT,0);
					continue;
				}
				if(rslt==NULL || rslt->rbnext==NULL){rc=-2;goto bail;}
				/*ic_assoc(newent,10);
				ic_ptcpy(ic_rbassoc->resval.rpoint,pt[0]);
				ic_assoc(newent,11);
				ic_ptcpy(ic_rbassoc->resval.rpoint,pt[1]);
				ic_assoc(newent,12);
				ic_ptcpy(ic_rbassoc->resval.rpoint,rslt->rbnext->resval.rpoint);
				ic_assoc(newent,13);
				ic_ptcpy(ic_rbassoc->resval.rpoint,rslt->resval.rpoint);*/
				sds_trans(rslt->resval.rpoint,&rbucs,&rbecs,0,rb10->resval.rpoint);
				sds_trans(rslt->rbnext->resval.rpoint,&rbucs,&rbecs,0,rb11->resval.rpoint);
				sds_trans(rslt->rbnext->rbnext->rbnext->resval.rpoint,&rbucs,&rbecs,0,rb12->resval.rpoint);
				sds_trans(rslt->rbnext->rbnext->resval.rpoint,&rbucs,&rbecs,0,rb13->resval.rpoint);

				if(sds_entmake(newent)!=RTNORM) { rc=(-2); goto bail; }
				ic_ptcpy(pt[0],pt[1]);

			}
		}
	}else if(mode&1){	//Rectangle mode
		struct cmd_drag_globalpacket *gpak;
		char defang[50],rectpmt[IC_ACADBUF];
		gpak=&cmd_drag_gpak;
		sds_point pt0;
		sds_name tempset;
		sds_real ang,sinang,cosang;

		ang=0.0;
		for(;;){
			IC_RELRB(rslt);
			if((ret=SDS_dragobject(21,setgetrb.resval.rint,pt[0],pt[0],NULL,ResourceString(IDC_SOLID_AND_TRACE__NOTHER__8, "\nOther corner of rectangle: " ),NULL,pt[3],NULL))!=RTNORM) {
				rc=(-2); goto bail;
			} else if(ret==RTCAN) {
				rc=(-1); goto bail;
			} else if(ret==RTNONE) {
				rc=0; goto bail;
			} else if(ret==RTNORM) {
				if(!icadRealEqual(pt[3][2],newelev)){
					cmd_ErrorPrompt(CMD_ERR_2DPT,0);
					continue;
				}
				/*ic_assoc(newent,10);
				ic_ptcpy(ic_rbassoc->resval.rpoint,pt[0]);
				ic_assoc(newent,13);
				ic_ptcpy(ic_rbassoc->resval.rpoint,pt[3]);
				ic_assoc(newent,12);
				ic_ptcpy(ic_rbassoc->resval.rpoint,pt[0]);
				ic_rbassoc->resval.rpoint[1]=pt[3][1];
				ic_assoc(newent,11);
				ic_ptcpy(ic_rbassoc->resval.rpoint,pt[0]);
				ic_rbassoc->resval.rpoint[0]=pt[3][0];*/
				sds_trans(pt[0],&rbucs,&rbecs,0,rb10->resval.rpoint);
				sds_trans(pt[3],&rbucs,&rbecs,0,rb13->resval.rpoint);
				ic_ptcpy(pt[2],pt[0]);
				pt[2][1]=pt[3][1];
				sds_trans(pt[2],&rbucs,&rbecs,0,rb12->resval.rpoint);
				ic_ptcpy(pt[1],pt[0]);
				pt[1][0]=pt[3][0];
				sds_trans(pt[1],&rbucs,&rbecs,0,rb11->resval.rpoint);

				if(sds_entmake(newent)!=RTNORM) { rc=(-2); goto bail; }

				//get the rotation angle for the solid
				ic_ptcpy(gpak->pt1,pt[0]);
				ic_ptcpy(gpak->lastpt,pt[0]);
				gpak->reference=0;
				gpak->refval=0.0;
				gpak->rband=0;
				needangle:
				sds_entlast(ename);
				IC_RELRB(newent);
				if((newent=sds_entget(ename))==NULL){rc=-2;goto bail;}
				for(rslt=newent;rslt!=NULL;rslt=rslt->rbnext){
					if(rslt->restype==10)rb10=rslt;
					else if(rslt->restype==11)rb11=rslt;
					else if(rslt->restype==12)rb12=rslt;
					else if(rslt->restype==13)rb13=rslt;
				}
				sds_ssadd(ename,NULL,tempset);
				sds_rtos(ang,-1,-1,defang);
				sprintf(rectpmt,ResourceString(IDC_SOLID_AND_TRACE__NROTATI_9, "\nRotation angle for plane <%s>: " ),defang);
				ret=cmd_drag(CMD_ROTATE_DRAG,tempset,rectpmt,NULL,pt0,fs1);
				ic_ptcpy(SDS_PointDisplay,pt0);
				sds_ssfree(tempset);
				if(ret==RTERROR){
					rc=-2;goto bail;
				}else if(ret!=RTNORM  && ret!=RTSTR && ret!=RTNONE){
					rc=-1;goto bail;
				}else{
					if(ret==RTSTR){
						if(sds_angtof(fs1,-1,&cosang)!=RTNORM){
							cmd_ErrorPrompt(CMD_ERR_ANG2NDPT,0);
							goto needangle;
						}
						ang=cosang;
					}else if(ret==RTNORM){
						ang=sds_angle(pt[0],pt0);
					}
					//else if(ret==RTNONE) leave ang unchanged
					sinang=sin(ang);
					cosang=cos(ang);
					//ic_assoc(newent,11);
					ptemp[0]=pt[0][0]+(pt[3][0]-pt[0][0])*cosang;
					ptemp[1]=pt[0][1]+(pt[3][0]-pt[0][0])*sinang;
					pt[1][0]=ptemp[0];
					pt[1][1]=ptemp[1];
					sds_trans(pt[1],&rbucs,&rbecs,0,rb11->resval.rpoint);
					//get starting point for next solid
					//ic_assoc(newent,12);
					ptemp[0]=pt[0][0]-(pt[3][1]-pt[0][1])*sinang;
					ptemp[1]=pt[0][1]+(pt[3][1]-pt[0][1])*cosang;
					pt[2][0]=ptemp[0];
					pt[2][1]=ptemp[1];
					sds_trans(pt[2],&rbucs,&rbecs,0,rb12->resval.rpoint);
					//ic_assoc(newent,13);
					ptemp[0]=pt[0][0]+(pt[3][0]-pt[0][0])*cosang-(pt[3][1]-pt[0][1])*sinang;
					ptemp[1]=pt[0][1]+(pt[3][0]-pt[0][0])*sinang+(pt[3][1]-pt[0][1])*cosang;
					pt[3][0]=ptemp[0];
					pt[3][1]=ptemp[1];
					sds_trans(pt[3],&rbucs,&rbecs,0,rb13->resval.rpoint);
					if((ret=sds_entmod(newent))!=RTNORM) { rc=(-2); goto bail; }
					//ic_assoc(newent,11);
					//ic_ptcpy(pt[0],ic_rbassoc->resval.rpoint);
					ic_ptcpy(pt[0],pt[1]);
				}
	    		SDS_BitBlt2Scr(0);
            }
		}
	}

    //*** Second point.
    for(;;) {
        pt[1][2]=newelev;
        if(sds_initget(SDS_RSG_NONULL,NULL)!=RTNORM) { rc=(-2); goto bail; }
        if((ret=internalGetpoint(pt[0],ResourceString(IDC_SOLID_AND_TRACE__NSECON_10, "\nSecond point: " ),pt[1]))==RTERROR) {
            rc=(-2); goto bail;
        } else if(ret==RTCAN) {
            rc=(-1); goto bail;
        } else if(ret==RTNORM) {
            //*** Check for valid Z coordinate.
            if(!icadRealEqual(pt[1][2],newelev)) {
                cmd_ErrorPrompt(CMD_ERR_2DPT,0);
                continue;
            }
            break;
        }
    }


	for(;;) {
		//*** Build a parlink for dragging.
		//from this point on, pt[0] thru pt[3] will be in UCS, while
		//	par->pt[0] thru par->pt[3] will be in ECS
		if(elp) delete elp; elp=NULL;
		elp=new db_solid(SDS_CURDWG);
		elp->set_210(extru);
		elp->set_10(pt[0]);
		elp->set_11(pt[1]);
		elp->set_12(pt[1]);
		elp->set_13(pt[1]);

		sds_trans(pt[0],&rbucs,&rbecs,0,elp->retp_10());
		sds_trans(pt[1],&rbucs,&rbecs,0,elp->retp_11());

		ename[0]=(long)elp;
		ename[1]=(long)SDS_CURDWG;

		SDS_getvar(NULL,DB_QTHICKNESS,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		elp->set_39(setgetrb.resval.rreal);

		SDS_getvar(NULL,DB_QDRAGMODE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

        //*** Third point.
        for(;;) {
            pt[2][2]=newelev;
            if(sds_initget(0,NULL)!=RTNORM) {
               rc=(-2);
               goto bail;
            }
			ret=SDS_dragobject(-1,setgetrb.resval.rint,(sds_real *)ename,(double *)elp->retp_12(),0.0,ResourceString(IDC_SOLID_AND_TRACE__NTHIRD_11, "\nThird point of plane: " ),NULL,pt[2],NULL);
            if(ret==RTERROR || ret==RTCAN) {
                rc=(ret); goto bail;
            } else if(ret==RTNONE) {
                goto bail;
            } else if(ret==RTKWORD) {
                continue;
            } else if(ret==RTNORM) {
                //*** Check for valid Z coordinate.
                if(!icadRealEqual(pt[2][2],newelev)) {
                    cmd_ErrorPrompt(CMD_ERR_2DPT,0);
                    continue;
                }
        		sds_trans(pt[2],&rbucs,&rbecs,0,elp->retp_12());
                break;
            }
        }
        //*** Fourth point.
        for(;;) {
            if(sds_initget(0,NULL)!=RTNORM) {
               rc=(-2);
               goto bail;
            }
			ret=SDS_dragobject(-1,setgetrb.resval.rint,(sds_real *)ename,(double *)elp->retp_13(),0.0,ResourceString(IDC_SOLID_AND_TRACE__NFOURT_12, "\nFourth point: " ),NULL,pt[3],NULL);
            if(ret==RTERROR || ret==RTCAN) {
                rc=(ret); goto bail;
            } else if(ret==RTNONE) {
                ic_ptcpy(pt[3],pt[2]);
                ic_ptcpy(elp->retp_13(),elp->retp_12());
                break;
            } else if(ret==RTKWORD) {
                continue;
            } else if(ret==RTNORM) {
                //*** Check for valid Z coordinate.
                if(!icadRealEqual(pt[3][2],newelev)) {
                    cmd_ErrorPrompt(CMD_ERR_2DPT,0);
                    continue;
                }
        		sds_trans(pt[3],&rbucs,&rbecs,0,elp->retp_13());
                break;
            }
        }

        //*** Make entity.
        if((newent=sds_buildlist(RTDXF0,"SOLID"/*DNT*/,10,elp->retp_10(),11,elp->retp_11(),12,elp->retp_12(),13,elp->retp_13(),210,extru,0))==NULL) {
            cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
            rc=(-2); goto bail;
        }
        if(sds_entmake(newent)!=RTNORM) { rc=(-2); goto bail; }
        IC_RELRB(newent);
		sds_trans(elp->retp_12(),&rbecs,&rbucs,0,pt[0]);
		sds_trans(elp->retp_13(),&rbecs,&rbucs,0,pt[1]);
    }

    bail:
	if(SDS_ElevationChanged){
		setgetrb.restype=RTREAL;
		setgetrb.resval.rreal=curelev;
		sds_setvar("ELEVATION"/*DNT*/,&setgetrb);
		SDS_ElevationChanged=FALSE;
	}

 	if(elp) delete elp; elp=NULL;
    IC_RELRB(newent);
	IC_RELRB(rslt);
    return(rc);
}

short cmd_trace(void) {
//*** This function creates solid lines.
//***
//*** RETURNS:  RTNORM - Success.
//***            RTCAN - User cancel.
//***          RTERROR - RTERROR returned from an ads function.
//***

    if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	char fs1[IC_ACADBUF],fs2[IC_ACADBUF];
    struct resbuf setgetrb,*newent,rbucs,rbecs;
    sds_real tracewid,curelev,ang1,newelev;
    sds_point pt[3],creatept[4],extru;
    RT ret,swapflg=0;
    long rc=0L;
    short initflg;

    newent=NULL; setgetrb.rbnext=NULL; initflg=1;

	rbucs.rbnext=rbecs.rbnext=NULL;
    rbucs.restype=RTSHORT;
	rbecs.restype=RT3DPOINT;
	rbucs.resval.rint=1;

    if(SDS_getvar(NULL,DB_QUCSXDIR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-2); goto bail; }
	ic_ptcpy(pt[0],setgetrb.resval.rpoint);
    if(SDS_getvar(NULL,DB_QUCSYDIR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-2); goto bail; }
	rbecs.resval.rpoint[0]=(pt[0][1]*setgetrb.resval.rpoint[2])-(setgetrb.resval.rpoint[1]*pt[0][2]);
	rbecs.resval.rpoint[1]=(pt[0][2]*setgetrb.resval.rpoint[0])-(setgetrb.resval.rpoint[2]*pt[0][0]);
	rbecs.resval.rpoint[2]=(pt[0][0]*setgetrb.resval.rpoint[1])-(setgetrb.resval.rpoint[0]*pt[0][1]);
	ic_ptcpy(extru,rbecs.resval.rpoint);

    // *** Get the current curelevation.
    if(SDS_getvar(NULL,DB_QELEVATION,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-__LINE__); goto bail; }
    curelev=newelev=setgetrb.resval.rreal;

	//*** Get default trace width.
    if(SDS_getvar(NULL,DB_QTRACEWID,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-__LINE__); goto bail; }
    tracewid=setgetrb.resval.rreal;
    if(sds_rtos(tracewid,-1,-1,fs2)!=RTNORM) { rc=(-__LINE__); goto bail; }
    sprintf(fs1,ResourceString(IDC_SOLID_AND_TRACE__NWIDTH_13, "\nWidth of trace <%s>: " ),fs2);
    //*** Get trace width.
    for(;;) {
        if(sds_initget(SDS_RSG_NONEG+SDS_RSG_NOZERO,NULL)!=RTNORM) {
           rc=(-__LINE__);
           goto bail;
        }
        if((ret=sds_getdist(NULL,fs1,&tracewid))==RTERROR) {
            rc=(-__LINE__); goto bail;
        } else if(ret==RTCAN) {
            rc=(-1L); goto bail;
        } else if(ret==RTNONE) {
			tracewid=setgetrb.resval.rreal;
            break;
        } else if(ret==RTNORM)
			break;
    }

    // *** First point.
    for(;;) {
        if(sds_initget(0,NULL)!=RTNORM) {
           rc=(-__LINE__);
           goto bail;
        }
        if((ret=internalGetpoint(NULL,ResourceString(IDC_SOLID_AND_TRACE__NSTART_14, "\nStart of trace: " ),pt[0]))==RTERROR) {
            rc=(-__LINE__); goto bail;
        } else if(ret==RTCAN) {
            rc=(-1L); goto bail;
        } else if(ret==RTNONE) {
            goto bail;
        } else if(ret==RTNORM) {
			if(!icadRealEqual(pt[0][2],curelev)){
				SDS_CorrectElevation=curelev;
				SDS_ElevationChanged=TRUE;
				newelev=setgetrb.resval.rreal=pt[0][2];
				setgetrb.restype=RTREAL;
				sds_setvar("ELEVATION"/*DNT*/,&setgetrb);
			}
            break;
        }
    }
    // Second point.
    for(;;) {
        if(sds_initget(0,NULL)!=RTNORM) {
           rc=(-__LINE__);
           goto bail;
        }
        if((ret=internalGetpoint(pt[0],ResourceString(IDC_SOLID_AND_TRACE__NNEXT__15, "\nNext point: " ),pt[1]))==RTERROR) {
            rc=(-__LINE__); goto bail;
        } else if(ret==RTCAN) {
            rc=(-1L); goto bail;
        } else if(ret==RTNORM ||ret==RTNONE) {
            // Check for valid Z coordinate.
            if(ret==RTNONE || (!icadRealEqual(pt[1][2],newelev) && !icadRealEqual(pt[1][2],curelev))) {
                cmd_ErrorPrompt(CMD_ERR_2DPT,0);
                continue;
            }
            pt[1][2]=newelev;
            // Calculate the angle between pt[0] and pt[1].
            ang1=sds_angle(pt[0],pt[1]);
            break;
        }
    }
    // Rest of the points.
    for(;;) {
        if(sds_initget(0,NULL)!=RTNORM) {
           rc=(-__LINE__);
           goto bail;
        }
		sds_grdraw(pt[0],pt[1],-1,0);
        ret=internalGetpoint(pt[1],ResourceString(IDC_SOLID_AND_TRACE__NNEXT__15, "\nNext point: " ),pt[2]);
		sds_grdraw(pt[0],pt[1],-1,0);
		if(ret==RTERROR){
            rc=(-__LINE__); goto bail;
        } else if(ret==RTCAN) {
            rc=(-1L); goto bail;
        } else if(ret==RTNONE) {
            // Calculate the angle between pt[0] and pt[1].
            ang1=sds_angle(pt[0],pt[1]);

            // Calculate the points for the entity.
            if(initflg) {
                sds_polar(pt[0],ang1+(IC_PI/2.0),tracewid/2.0,creatept[0]);
                sds_polar(pt[0],ang1-(IC_PI/2.0),tracewid/2.0,creatept[1]);
            }
            // Calculate the rest of the points.
            sds_polar(pt[1],ang1+(IC_PI/2.0),tracewid/2.0,creatept[2]);
            sds_polar(pt[1],ang1-(IC_PI/2.0),tracewid/2.0,creatept[3]);
        } else if(ret==RTNORM) {
            //*** Check for valid Z coordinate.
            if(!icadRealEqual(pt[2][2],newelev) && !icadRealEqual(pt[2][2],curelev)) {
                cmd_ErrorPrompt(CMD_ERR_2DPT,0);
                continue;
            }
            pt[2][2]=newelev;
            //*** Calculate the points for the entity.
            if(initflg) {
                sds_polar(pt[0],ang1+(IC_PI/2.0),tracewid/2.0,creatept[0]);
                sds_polar(pt[0],ang1-(IC_PI/2.0),tracewid/2.0,creatept[1]);
            }
            if((swapflg=(int)cmd_tracejoint(pt[0],pt[1],pt[2],tracewid,creatept[2],creatept[3]))==(-1)) {
                rc=(-__LINE__); goto bail;
            }
            ret=RTNORM;
        }
        //*** Create entity.
		if(initflg){
			sds_trans(creatept[0],&rbucs,&rbecs,0,creatept[0]);
			sds_trans(creatept[1],&rbucs,&rbecs,0,creatept[1]);
		}
		sds_trans(creatept[2],&rbucs,&rbecs,0,creatept[2]);
		sds_trans(creatept[3],&rbucs,&rbecs,0,creatept[3]);

        if((newent=sds_buildlist(RTDXF0,"TRACE"/*DNT*/,10,creatept[0],11,creatept[1],12,creatept[2],13,creatept[3],210,extru,0))==NULL) {
            cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
            rc=(-__LINE__); goto bail;
        }
        if(sds_entmake(newent)!=RTNORM) { rc=(-__LINE__); goto bail; }
        IC_RELRB(newent);
        if(ret==RTNONE) break;
        //*** Re-set variables.
        ic_ptcpy(pt[0],pt[1]); ic_ptcpy(pt[1],pt[2]);
        if(swapflg) {
            ic_ptcpy(creatept[1],creatept[2]); ic_ptcpy(creatept[0],creatept[3]);
        } else {
            ic_ptcpy(creatept[0],creatept[2]); ic_ptcpy(creatept[1],creatept[3]);
        }
        initflg=0;
    }

    bail:

    IC_RELRB(newent);
	if(SDS_ElevationChanged){
		setgetrb.restype=RTREAL;
		setgetrb.resval.rreal=curelev;
		sds_setvar("ELEVATION"/*DNT*/,&setgetrb);
		SDS_ElevationChanged=FALSE;
	}
    if(rc==0L) {
        //*** Set TRACEWID system variable.
        setgetrb.restype=RTREAL;
        setgetrb.resval.rreal=tracewid;
        if(sds_setvar("TRACEWID"/*DNT*/,&setgetrb)!=RTNORM) { rc=(-__LINE__); goto bail; }
        return(RTNORM);
    }
    if(rc==(-1L)) return(RTCAN);
    if(rc<(-1L)) CMD_INTERNAL_MSG(rc);
    return(RTERROR);
}

