/* SCALE_AND_ROTATE.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * Supports the scale and rotate
 *
 */
// THIS FILE HAS BEEN GLOBALIZED!
//** Includes
#include "cmds.h"/*DNT*/
#include "Icad.h" /*DNT*/
#include "IcadApi.h"
#ifndef INTELLICAD
#define INTELLICAD // needed for the following Geo include statements
#endif
#include "Geometry.h"/*DNT*/
#include "point.h"

#include "TransformUtils.h"


// **********
// GLOBALS
//
extern struct cmd_drag_globalpacket cmd_drag_gpak;
extern int SDS_PointDisplayMode;
extern sds_point SDS_PointDisplay;

int setUCS( const sds_point ucsOrg, const sds_point ucsXDir, const sds_point ucsYDir )
{
	const char cUcsOrg[] = "UCSORG"/*DNT*/;
	const char cUcsXDir[] = "UCSXDIR"/*DNT*/;
	const char cUcsYDir[] = "UCSYDIR"/*DNT*/;
	sds_resbuf rb;
	rb.restype = RT3DPOINT;
	int ret = RTNORM;
	sds_point_set( ucsOrg, rb.resval.rpoint );
	if( (ret = sdsengine_setvar( cUcsOrg, &rb ))!=RTNORM) return 0;
	sds_point_set( ucsXDir, rb.resval.rpoint );
	if( (ret = sdsengine_setvar( cUcsXDir, &rb ))!=RTNORM) return 0;
	sds_point_set( ucsYDir, rb.resval.rpoint );
	if( (ret = sdsengine_setvar( cUcsYDir, &rb ))!=RTNORM) return 0;
	return 1;
}

int changeUCStoNewZDirAndOrig( const sds_point newZDir, const sds_point newOrig,
								 sds_point oldUcsOrg, sds_point oldUcsXDir, sds_point oldUcsYDir )
{
	const char cUcsOrg[] = "UCSORG"/*DNT*/;
	const char cUcsXDir[] = "UCSXDIR"/*DNT*/;
	const char cUcsYDir[] = "UCSYDIR"/*DNT*/;
	sds_resbuf rb;
	rb.restype = RT3DPOINT;
	int res = 0;

	sds_point	ucsOrg = {0, 0, 0 };
	double  rAngle = 0;

	sdsengine_getvar( cUcsOrg,&rb );
	sds_point_set( rb.resval.rpoint, oldUcsOrg );
	sdsengine_getvar( cUcsXDir,&rb );
	sds_point_set( rb.resval.rpoint, oldUcsXDir );
	sdsengine_getvar( cUcsYDir,&rb );
	sds_point_set( rb.resval.rpoint, oldUcsYDir );

	C3Point _oldUcsXDir( oldUcsXDir[0], oldUcsXDir[1], oldUcsXDir[2] );
	C3Point _oldUcsYDir( oldUcsYDir[0], oldUcsYDir[1], oldUcsYDir[2] );
	C3Point _oldUcsZDir = _oldUcsXDir ^ _oldUcsYDir;
	C3Point _newZDir( newZDir[0], newZDir[1], newZDir[2] );
	if( _oldUcsZDir.Unitize() != SUCCESS || _newZDir.Unitize() != SUCCESS )
		return 0;
	C3Point tmpRotAxis = _oldUcsZDir ^ _newZDir;
	if( tmpRotAxis.Unitize() == SUCCESS && Angle( _oldUcsZDir, _newZDir, rAngle, &tmpRotAxis ) == SUCCESS )
	{
		sds_matrix mtx;
		ic_rotxform( ucsOrg, (double*)&tmpRotAxis, rAngle, mtx );
		sds_point newXDir, newYDir;
		// Set new UCS
		sds_point_set( newOrig, rb.resval.rpoint );
		ic_xformpt( oldUcsXDir, newXDir, mtx );
		ic_xformpt( oldUcsYDir, newYDir, mtx );
		// Check for correct result of z axis rotation
		C3Point resNewZDir;
		ic_crossproduct(newXDir, newYDir, (double*)&resNewZDir);
		resNewZDir.Unitize();
		assert( resNewZDir == _newZDir );
		return setUCS( newOrig, newXDir, newYDir );
	}
	return 0;
}

short cmd_scale(void) {								// Prompting is frozen, 4/30/96. 

    if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	char fs1[IC_ACADBUF],dragstr[IC_ACADBUF],inputstr[IC_ACADBUF];
    sds_name ss1,ename;
    sds_point pt;
    sds_real len;
    sds_matrix mtx;
    RT rc=0,ret;
    long ssct;
    struct cmd_drag_globalpacket *gpak;

    gpak=&cmd_drag_gpak;
    gpak->reference=0; gpak->refval=1; gpak->rband=1;
    //*** Get selection set of entities to scale.
    if(sds_pmtssget(ResourceString(IDC_SCALE_AND_ROTATE__NSELEC_0, "\nSelect entities to scale: " ),NULL,NULL,NULL,NULL,ss1,0)!=RTNORM) return(-1);

    //*** Get base point.
    for(;;) {
        if(sds_initget(SDS_RSG_NODOCCHG,NULL)!=RTNORM) {
           rc=(-2);
           goto bail;
        }
        if((ret=internalGetpoint(NULL,ResourceString(IDC_SCALE_AND_ROTATE__NBASE__1, "\nBase point: " ),gpak->pt1))==RTERROR) {
            rc=(-2); goto bail;
        } else if(ret==RTCAN) {
            rc=(-1); goto bail;
        } else if(ret==RTNORM) {
            break;
        } else {
            cmd_ErrorPrompt(CMD_ERR_PT,0);
            continue;
        }
    }

    //*** Get second point.
    strcpy(inputstr,ResourceString(IDC_SCALE_AND_ROTATE_BASE__R_2, "Base ~Reference Scale ~_Base ~_Reference ~_Scale" ));
    strcpy(dragstr,ResourceString(IDC_SCALE_AND_ROTATE__NBASE__3, "\nBase scale/<Scale factor>: " ));
    for(;;) {
        if(gpak->reference) {
            *inputstr=0;
            strcpy(dragstr,ResourceString(IDC_SCALE_AND_ROTATE__NNEW_S_4, "\nNew scale: " ));
        }
		SDS_PointDisplayMode=5;
		ic_ptcpy(SDS_PointDisplay,gpak->pt1);
        if((ret=cmd_drag(CMD_SCALE_DRAG,ss1,dragstr,inputstr,pt,fs1))==RTERROR) {
			SDS_PointDisplayMode=0;
            rc=(-2); goto bail;
        } else if(ret==RTCAN) {
			SDS_PointDisplayMode=0;
            rc=(-1); goto bail;
        } else if(ret==RTNORM) {
			SDS_PointDisplayMode=0;
			//Modified Cybage AW 03-12-01 [
			//Reason : Global variables "bNotAPoint" and "strAngleScale" are not used anymore.
			/*
			//Modified Cybage AW 02-07-01 [
			//Reason : The dragmode OFF or ON affect the commands like rotate/Scale
			extern BOOL bNotAPoint;
			extern char strAngleScale[IC_ACADBUF];
			if(bNotAPoint)
			{
				strcpy(fs1, strAngleScale);
				if(ic_isnumstr(fs1,&len)) {
                if(len<=0.0) {
					cmd_ErrorPrompt(CMD_ERR_POSNOZERO,0);
                    len=0.0; 
					continue; }
				else break;
				}
			}
			//Modified Cybage AW 02-07-01 ]
			else
			*/
			//Modified Cybage AW 03-12-01 ]
			len=sds_distance(gpak->pt1,pt);	
            break;
        } else if(ret==RTSTR || ret==RTKWORD) {
			SDS_PointDisplayMode=0;
            if((strisame("REFERENCE"/*DNT*/,fs1)) || (strisame("BASE"/*DNT*/,fs1))) {
                for(;;) {
                    gpak->refval=1;
                    if(sds_initget(SDS_RSG_NODOCCHG | SDS_RSG_OTHER,NULL)!=RTNORM) { 
                        rc=(-2); 
                        goto bail; 
                    }
                    if((ret=sds_getdist(NULL,ResourceString(IDC_SCALE_AND_ROTATE__NBASE__7, "\nBase scale <1>: " ),&gpak->refval))==RTERROR) {
                        rc=(-2); goto bail;
                    } else if(ret==RTCAN) {
                        rc=(-1); goto bail;
                    } else if(ret==RTNONE || ret==RTNORM) {
                        break;
                    } else {
						cmd_ErrorPrompt(CMD_ERR_DIST2PTS,0);
                        continue;
                    }
                    break;
                }
                gpak->reference=1;
                continue;
			/*DG - 2.7.2003*/// Allow more flexible input, particularly fractions.
            //} else if(ic_isnumstr(fs1,&len)) {
            } else if(sds_distof(fs1, -1, &len) == RTNORM) {
                if(len<=0.0) {
					cmd_ErrorPrompt(CMD_ERR_POSNOZERO,0);
                    len=0.0; continue;
                } else break;
            } else {
                if(!gpak->reference) cmd_ErrorPrompt(CMD_ERR_DIST2NDPTKWORD,0);
                else cmd_ErrorPrompt(CMD_ERR_DIST2NDPT,0);
                continue;
            }
        } else {
			SDS_PointDisplayMode=0;
            if(!gpak->reference) cmd_ErrorPrompt(CMD_ERR_DIST2NDPTKWORD,0);
            else cmd_ErrorPrompt(CMD_ERR_DIST2NDPT,0);
            continue;
        }
    }

	SDS_PointDisplayMode=0;
    //*** Setup matrix for sds_xformss.
    len/=gpak->refval;
    mtx[0][0]=len; mtx[0][1]=0.0; mtx[0][2]=0.0; mtx[0][3]=(-(len-1)*gpak->pt1[0]);
    mtx[1][0]=0.0; mtx[1][1]=len; mtx[1][2]=0.0; mtx[1][3]=(-(len-1)*gpak->pt1[1]);
    mtx[2][0]=0.0; mtx[2][1]=0.0; mtx[2][2]=len; mtx[2][3]=(-(len-1)*gpak->pt1[2]);
    mtx[3][0]=0.0; mtx[3][1]=0.0; mtx[3][2]=0.0; mtx[3][3]=1.0;
    //***
    if(SDS_progxformss(ss1,mtx,1)!=RTNORM) { rc=(-2); goto bail; }

	// break association appropriately if moving one entity without the reactor
	// or update the reactors if have both.
	cmd_checkReactors (ss1);

bail:
	// redraw entities
	for(ssct = 0L; sds_ssname(ss1, ssct, ename) == RTNORM; ssct++)
		sds_redraw(ename, IC_REDRAW_UNHILITE);
	sds_ssfree(ss1);
    return(rc);
}


short cmd_rotate3d(void) {

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    static sds_point lastvec={0.0,0.0,0.0},lastpt1;
    sds_real ang=0.0,rang=0.0,rbulge,rtmp1,rtmp2,rtmp3;
	RT	ret;
	sds_name ss1,ename,ename1;
	char szKword[IC_ACADBUF];
	sds_point pt1,pt2,ptemp,vec;
    struct resbuf *pRbb=NULL,*pRb=NULL,rb,ucs,ecs,wcs;
    long lVer=0L,i;
	int exactvert;

    ucs.restype=RTSHORT;
    ucs.resval.rint=1;
    wcs.restype=RTSHORT;
    wcs.resval.rint=0;
    ecs.restype=RT3DPOINT;
    ecs.resval.rpoint[0]=ecs.resval.rpoint[1]=0.0;
    ecs.resval.rpoint[2]=1.0;

    //*** Get selection set of entities to rotate.
    if((ret=sds_pmtssget(ResourceString(IDC_SCALE_AND_ROTATE__NSELEC_8, "\nSelect entities to rotate: " ),NULL,NULL,NULL,NULL,ss1,0))!=RTNORM) goto bail;

	if (RemoveHatchFromSS(ss1) == RTNIL)
		goto bail;

	for(;;){
        if((ret=sds_initget(SDS_RSG_NODOCCHG,ResourceString(IDC_SCALE_AND_ROTATE_INITGET__9, "~Object Entity Last View ~ Xaxis Yaxis Zaxis ~_Object ~_Entity ~_Last ~_View ~_ ~_Xaxis ~_Yaxis ~_Zaxis" )))!=RTNORM) 
            goto bail;
	    if((ret=internalGetpoint(NULL,ResourceString(IDC_SCALE_AND_ROTATE__NSELE_10, "\nSelect axis by:  Entity/Last/View/Xaxis/Yaxis/Zaxis/<2 Points>: " ),pt1))==RTERROR) {
		    goto bail;
	    }else if(ret==RTCAN){
		    goto bail;
	    }else if(ret==RTNORM){
           if((ret=sds_initget(SDS_RSG_NODOCCHG|RSG_NONULL,NULL))!=RTNORM)
               goto bail;
		    if((ret=internalGetpoint(pt1,ResourceString(IDC_SCALE_AND_ROTATE__NSPEC_11, "\nSpecify the second rotation axis point: " ),pt2))!=RTNORM) goto bail;
            vec[0]=pt2[0]-pt1[0];
            vec[1]=pt2[1]-pt1[1];
            vec[2]=pt2[2]-pt1[2];
        }else if(ret==RTNONE){
           if((ret=sds_initget(SDS_RSG_NODOCCHG|RSG_NONULL,NULL))!=RTNORM)
               goto bail;
		    if((ret=internalGetpoint(NULL,ResourceString(IDC_SCALE_AND_ROTATE__NSPEC_12, "\nSpecify the first rotation axis point: " ),pt1))!=RTNORM) goto bail;
           if((ret=sds_initget(SDS_RSG_NODOCCHG|RSG_NONULL,NULL))!=RTNORM)
               goto bail;
		    if((ret=internalGetpoint(pt1,ResourceString(IDC_SCALE_AND_ROTATE__NSPEC_11, "\nSpecify the second rotation axis point: " ),pt2))!=RTNORM) goto bail;
            vec[0]=pt2[0]-pt1[0];
            vec[1]=pt2[1]-pt1[1];
            vec[2]=pt2[2]-pt1[2];
	    }else if(ret==RTKWORD){
		    sds_getinput(szKword);
            if(strisame(szKword, "OBJECT"/*DNT*/) || strisame(szKword, "ENTITY"/*DNT*/)) {
                while(1){
			        tryagain:
                    if((ret=sds_entsel(ResourceString(IDC_SCALE_AND_ROTATE__NSELE_15, "\nSelect an arc, circle, ellipse, line, 2D or 3D-polyline segment: " ),ename,ptemp))==RTCAN) goto bail;
                    if(ret==RTERROR) continue;

                    pRbb=sds_entget(ename);
                    for(pRb=pRbb;(pRb!=NULL)&&(pRb->restype!=0);pRb=pRb->rbnext);
                    if(pRb==NULL) break;
                    if(strsame(pRb->resval.rstring,"ARC"/*DNT*/)){
                        for(pRb=pRbb;(pRb!=NULL)&&(pRb->restype!=210);pRb=pRb->rbnext);
                        ic_ptcpy(ecs.resval.rpoint,pRb->resval.rpoint);
                        //210 to ucs
                        if((ret=sds_trans(pRb->resval.rpoint,&wcs,&ucs,1,vec))!=RTNORM) goto bail;
                        //10 to ucs
                        for(pRb=pRbb;(pRb!=NULL)&&(pRb->restype!=10);pRb=pRb->rbnext);
                        if((ret=sds_trans(pRb->resval.rpoint,&ecs,&ucs,0,pt1))!=RTNORM) goto bail;
                        break;
                    }else if(strsame(pRb->resval.rstring,"CIRCLE"/*DNT*/)){
                        for(pRb=pRbb;(pRb!=NULL)&&(pRb->restype!=210);pRb=pRb->rbnext);
                        ic_ptcpy(ecs.resval.rpoint,pRb->resval.rpoint);
                        //210 to ucs
                        if((ret=sds_trans(pRb->resval.rpoint,&wcs,&ucs,1,vec))!=RTNORM) goto bail;
                        //10 to ucs
                        for(pRb=pRbb;(pRb!=NULL)&&(pRb->restype!=10);pRb=pRb->rbnext);
                        if((ret=sds_trans(pRb->resval.rpoint,&ecs,&ucs,0,pt1))!=RTNORM) goto bail;
                        break;
                    }else if(strsame(pRb->resval.rstring,"ELLIPSE"/*DNT*/)){
                        //10 to ucs
                        for(pRb=pRbb;(pRb!=NULL)&&(pRb->restype!=10);pRb=pRb->rbnext);
                        if((ret=sds_trans(pRb->resval.rpoint,&wcs,&ucs,0,pt1))!=RTNORM) goto bail;
                        //210 to ucs
                        for(pRb=pRbb;(pRb!=NULL)&&(pRb->restype!=210);pRb=pRb->rbnext);
                        if((ret=sds_trans(pRb->resval.rpoint,&wcs,&ucs,1,vec))!=RTNORM) goto bail;
                        break;
                    }else if(strsame(pRb->resval.rstring,db_hitype2str(DB_LINE))){
                        //10 to ucs
                        for(pRb=pRbb;(pRb!=NULL)&&(pRb->restype!=10);pRb=pRb->rbnext);
                        if((ret=sds_trans(pRb->resval.rpoint,&wcs,&ucs,0,pt1))!=RTNORM) goto bail;
                        //11 to ucs
                        for(pRb=pRbb;(pRb!=NULL)&&(pRb->restype!=11);pRb=pRb->rbnext);
                        if((ret=sds_trans(pRb->resval.rpoint,&wcs,&ucs,0,pt2))!=RTNORM) goto bail;
                        vec[0]=pt2[0]-pt1[0];
                        vec[1]=pt2[1]-pt1[1];
                        vec[2]=pt2[2]-pt1[2];
                        break;
                    }else if(strsame(pRb->resval.rstring,"POLYLINE"/*DNT*/)){
                        for(pRb=pRbb;(pRb!=NULL)&&(pRb->restype!=70);pRb=pRb->rbnext);
                        int pflag;
                        pflag=pRb->resval.rint;
                       //set up the ecs
                        if(pflag&(IC_PLINE_3DPLINE | IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH)) { //if 3D pline the wcs
                            ecs.restype=RTSHORT;
                            ecs.resval.rint=0;
                        }else{
                            //if 2D pline then ecs
                            //we will use ecs for both wcs and ecs
                            //210 to ucs
                            for(pRb=pRbb;(pRb!=NULL)&&(pRb->restype!=210);pRb=pRb->rbnext);
                            if((ret=sds_trans(pRb->resval.rpoint,&wcs,&ucs,1,vec))!=RTNORM) goto bail;
                            ic_ptcpy(ecs.resval.rpoint,pRb->resval.rpoint);
                        }
                        IC_RELRB(pRbb);

                        //get vertex
                        ename1[0]=ename[0];ename1[1]=ename[1];
                        lVer=cmd_pl_vtx_no(ename1,ptemp,0,&exactvert);
                        if(lVer<1) {
			                cmd_ErrorPrompt(CMD_ERR_ENTTYPE,0);
                            IC_RELRB(pRbb);
                            continue;
                        }
                        for(i=0L;i<lVer;i++) {
                            if(sds_entnext_noxref(ename1,ename1)!=RTNORM){
                                cmd_ErrorPrompt(CMD_ERR_ENTTYPE,0);
                                IC_RELRB(pRbb);
                                goto tryagain;
                            }
                        }
                        //get bulge
                        for(pRb=pRbb=sds_entget(ename1);(pRb!=NULL)&&(pRb->restype!=42);pRb=pRb->rbnext);
                        rbulge=pRb->resval.rreal;
                        //10 to ucs
                        for(pRb=pRbb;(pRb!=NULL)&&(pRb->restype!=10);pRb=pRb->rbnext);
                        ic_ptcpy(pt1,pRb->resval.rpoint);
                        IC_RELRB(pRbb);
                        //get next segment
                        if(cmd_pline_entnext(ename1,ename1,pflag)!=0){
                            cmd_ErrorPrompt(CMD_ERR_ENTTYPE,0);
                            continue;
                        }
                        //get the end of first segment
                        for(pRb=pRbb=sds_entget(ename1);(pRb!=NULL)&&(pRb->restype!=10);pRb=pRb->rbnext);
                        ic_ptcpy(pt2,pRb->resval.rpoint);
                        //check for line or arc
                        if(!icadRealEqual(rbulge,0.0)){ //if it has bulge
                            ic_bulge2arc(pt1,pt2,rbulge,ptemp,&rtmp1,&rtmp2,&rtmp3);
                            sds_trans(ptemp,&ecs,&ucs,0,pt1);
                        }else{
                            sds_trans(pt1,&ecs,&ucs,0,pt1);
                            sds_trans(pt2,&ecs,&ucs,0,pt2);
                            vec[0]=pt2[0]-pt1[0];
                            vec[1]=pt2[1]-pt1[1];
                            vec[2]=pt2[2]-pt1[2];
                        }
                        break;
                    }
                    cmd_ErrorPrompt(CMD_ERR_ENTTYPE,0);
                }
            }else if(strisame(szKword, "LAST"/*DNT*/)) {
                if(icadRealEqual(lastvec[0],0.0)&&icadRealEqual(lastvec[1],0.0)&&
				    icadRealEqual(lastvec[2],0.0)){
			        cmd_ErrorPrompt(CMD_ERR_NOLASTAXIS,0);
                    continue;
                }
                ic_ptcpy(pt1,lastpt1);
                ic_ptcpy(vec,lastvec);

            }else if(strisame(szKword, "VIEW"/*DNT*/)) {
			    if(ret=(SDS_getvar(NULL,DB_QVIEWDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM) goto bail;

                if((ret=sds_initget(SDS_RSG_NODOCCHG,NULL))!=RTNORM)
                   goto bail;
		        if((ret=internalGetpoint(NULL,ResourceString(IDC_SCALE_AND_ROTATE__NSPEC_18, "\nSpecify point on view direction axis <0,0,0>: " ),pt1))==RTERROR){
                    goto bail;
                }else if(ret==RTCAN){
                    goto bail;
                }else if(ret==RTNONE){
                    pt1[0]=0.0;pt1[1]=0.0;pt1[2]=0.0;
                }
		        vec[0]=rb.resval.rpoint[0];
		        vec[1]=rb.resval.rpoint[1];
		        vec[2]=rb.resval.rpoint[2];
            }else if(strisame(szKword, "XAXIS"/*DNT*/)) {
                if((ret=sds_initget(SDS_RSG_NODOCCHG,NULL))!=RTNORM)
                   goto bail;
		        if((ret=internalGetpoint(NULL,ResourceString(IDC_SCALE_AND_ROTATE__NSPEC_20, "\nSpecify point on the X axis <0,0,0>: " ),pt1))==RTERROR){
                    goto bail;
                }else if(ret==RTCAN){
                    goto bail;
                }else if(ret==RTNONE){
                    pt1[0]=0.0;pt1[1]=0.0;pt1[2]=0.0;
                }
		        vec[0]=1.0;
                vec[1]=0.0;
                vec[2]=0.0;
            }else if(strisame(szKword, "YAXIS"/*DNT*/)) {
                if((ret=sds_initget(SDS_RSG_NODOCCHG,NULL))!=RTNORM)
                   goto bail;
		        if((ret=internalGetpoint(NULL,ResourceString(IDC_SCALE_AND_ROTATE__NSPEC_22, "\nSpecify point on the Y axis <0,0,0>: " ),pt1))==RTERROR){
                    goto bail;
                }else if(ret==RTCAN){
                    goto bail;
                }else if(ret==RTNONE){
                    pt1[0]=0.0;pt1[1]=0.0;pt1[2]=0.0;
                }
		        vec[0]=0.0;
                vec[1]=1.0;
                vec[2]=0.0;
            }else if(strisame(szKword, "ZAXIS"/*DNT*/)) {
                if((ret=sds_initget(SDS_RSG_NODOCCHG,NULL))!=RTNORM)
                   goto bail;
		        if((ret=internalGetpoint(NULL,ResourceString(IDC_SCALE_AND_ROTATE__NSPEC_24, "\nSpecify point on the Z axis <0,0,0>: " ),pt1))==RTERROR){
                    goto bail;
                }else if(ret==RTCAN){
                    goto bail;
                }else if(ret==RTNONE){
                    pt1[0]=0.0;pt1[1]=0.0;pt1[2]=0.0;
                }
		        vec[0]=0.0;
                vec[1]=0.0;
                vec[2]=1.0;
		    }
	    }
        break;
    }

	if((ret=sds_initget(SDS_RSG_NODOCCHG|SDS_RSG_NONULL,ResourceString(IDC_SCALE_AND_ROTATE_REFEREN_5, "Reference ~_Reference" )))!=RTNORM) 
        goto bail;
	if((ret=sds_getorient(NULL,ResourceString(IDC_SCALE_AND_ROTATE__NREFE_25, "\nReference/<Rotation Angle>: " ),&ang))==RTERROR) {
		goto bail;
	}else if(ret==RTCAN){
		goto bail;
	}else if(ret==RTKWORD){
       if((ret=sds_initget(SDS_RSG_NODOCCHG,NULL))!=RTNORM)
           goto bail;
        if((ret=sds_getorient(NULL,ResourceString(IDC_SCALE_AND_ROTATE__NREFE_26, "\nReference Angle <0>: " ),&rang))==RTERROR) {
		    goto bail;
	    }else if(ret==RTCAN){
		    goto bail;
	    }else if(ret==RTNONE){
	        rang=0.0;
        }
       if((ret=sds_initget(SDS_RSG_NODOCCHG|RSG_NONULL,NULL))!=RTNORM)
           goto bail;
        if((ret=sds_getorient(NULL,ResourceString(IDC_SCALE_AND_ROTATE__NNEW__27, "\nNew angle :" ),&ang))!=RTNORM) goto bail;
        ang=ang-rang;
    }

    ic_ptcpy(lastvec,vec);
    ic_ptcpy(lastpt1,pt1);
    sds_matrix mxTransform;
	ic_rotxform( pt1, vec, ang, mxTransform );

	sds_matrix	tmpMtx;

#ifdef _TEST_OLD_3DROTATE
	sds_initget(SDS_RSG_NODOCCHG, "Yes No");
	char answer[256];
	ret = sds_getkword( "Test old variant of the command (Yes/No)? <Yes>:", answer );

    if( ret != RTNORM && ret != RTNONE )
        goto bail;
	if( ret == RTNONE || strlen( answer ) == 0 || strisame( "Yes", answer ) )
	{
		ads_printf( "\nUsing old variant..." );
		ret = SDS_CallUserCallbackFunc(SDS_CBXFORMSS,(void *)ss1,(void *)mxTransform,NULL);
		if ( ret EQ RTERROR )
			goto bail;

		cmd_array_func(2,ss1,pt1,vec,ang);
		cmd_checkReactors(ss1);
		goto bail;
	}
	
	ads_printf( "\nUsing new variant..." );

	rb.restype=RT3DPOINT;
	sds_matrix	uw, wu;
	ic_idmat( uw );
	ic_idmat( wu );

	uc2wcMatrix( wu );
	wc2ucMatrix( uw );
	multMtx2Mtx( mxTransform, uw, tmpMtx );
	multMtx2Mtx( wu, tmpMtx, tmpMtx );
	sds_printf( "\nTransformation matrix in WCS:" );
	for( i = 0; i < 4; i++ )
	{
		sds_printf( "\nm[%d][0] = %f, m[%d][1] = %f, m[%d][2] = %f, m[%d][3] = %f",
		i, tmpMtx[i][0], i, tmpMtx[i][1], i, tmpMtx[i][2], i, tmpMtx[i][3] );
	}
#endif

	sds_point oldUcsOrg;
	sds_point oldUcsXDir;
	sds_point oldUcsYDir;
	sds_point newZDir, newOrig;
	uc2wc( vec, newZDir, 1 );
	uc2wc( pt1, newOrig, 0 );
	if( changeUCStoNewZDirAndOrig( newZDir, newOrig, oldUcsOrg, oldUcsXDir, oldUcsYDir ) )
	{
		sds_point ucsOrg;
		ucsOrg[0] = ucsOrg[1] = ucsOrg[2] = 0;
		ic_rotxform( ucsOrg, ang, tmpMtx );
		ret = SDS_progxformss(ss1,tmpMtx,1);
		setUCS( oldUcsOrg, oldUcsXDir, oldUcsYDir );
		if( ret !=RTNORM )
			goto bail;
	}
	else
	{
		ret = SDS_progxformss( ss1, mxTransform, 1 );
	}
	cmd_checkReactors(ss1);

bail:
	// redraw entities
	for(long ssct = 0L; sds_ssname(ss1, ssct, ename) == RTNORM; ssct++)
		sds_redraw(ename, IC_REDRAW_UNHILITE);
	sds_ssfree(ss1);
    IC_RELRB(pRbb);
    return(ret);
}

short cmd_rotate(void) {				// Prompting is frozen, 4/30/96 

    if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	char fs1[IC_ACADBUF],dragstr[IC_ACADBUF],inputstr[IC_ACADBUF];
    sds_name ss1,ename;
    sds_point pt2;
    sds_real theta;
    sds_matrix mtx;
    long ssct;
    struct resbuf rb;
    struct cmd_drag_globalpacket *gpak;
    short rc=0,ret;

    gpak=&cmd_drag_gpak;
    gpak->reference=0; gpak->rband=1;
    //*** Get selection set of entities to rotate.
    if(sds_pmtssget(ResourceString(IDC_SCALE_AND_ROTATE__NSELEC_8, "\nSelect entities to rotate: " ),NULL,NULL,NULL,NULL,ss1,0)!=RTNORM) return(-1);

	int angdir = SDS_getvar(NULL, DB_QANGDIR, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES) == RTNORM ? rb.resval.rint : 0;

    //*** Get base point.
    for(;;) {
        if(sds_initget(0,NULL)!=RTNORM) {
           rc=(-2);
           goto bail;
        }
        if((ret=internalGetpoint(NULL,ResourceString(IDC_SCALE_AND_ROTATE__NROTA_28, "\nRotation point: " ),gpak->pt1))==RTERROR) {
            rc=(-2); goto bail;
        } else if(ret==RTCAN) {
            rc=(-1); goto bail;
        } else if(ret==RTNORM) {
            break;
        } else {
            cmd_ErrorPrompt(CMD_ERR_PT,0);
            continue;
        }
    }

    //*** Get second point.
    strcpy(inputstr, ResourceString(IDC_SCALE_AND_ROTATE_REF_BASE_32,"~Reference Base_angle|Base ~_Reference ~_Base"));
    strcpy(dragstr,ResourceString(IDC_SCALE_AND_ROTATE__NBASE_29, "\nBase angle/<Rotation angle>: " ));
    for(;;) {
        if(gpak->reference) {
            *inputstr=0;
            strcpy(dragstr,ResourceString(IDC_SCALE_AND_ROTATE__NNEW__30, "\nNew angle: " ));
        }
		SDS_PointDisplayMode=5;
		ic_ptcpy(SDS_PointDisplay,gpak->pt1);
        ret=cmd_drag(CMD_ROTATE_DRAG,ss1,dragstr,inputstr,pt2,fs1);
		SDS_PointDisplayMode=0;
        if(ret==RTERROR) {
            rc=(-2); goto bail;
        } else if(ret==RTCAN) {
            rc=(-1); goto bail;
        } else if(ret==RTNORM) {
			//Modified Cybage AW 03-12-01 [
			//Reason : Global variables "bNotAPoint" and "strAngleScale" are not used anymore.
			/*
			//Modified Cybage AW 02-07-01 [
			//Reason : The dragmode OFF or ON affect the commands like rotate/Scale
			extern BOOL bNotAPoint;
			extern char strAngleScale[IC_ACADBUF];
			if(bNotAPoint)
			{
				strcpy(fs1, strAngleScale);
				if(SDS_angtof_convert(0,fs1,0,&theta)==RTNORM)
					break;
				else
				{
					cmd_ErrorPrompt(CMD_ERR_ANG2NDPT,0);
					continue;
				}
			}
			//Modified Cybage AW 02-07-01 ]
			else	
			*/
			//Modified Cybage AW 03-12-01 ]
	        theta=sds_angle(gpak->pt1,pt2);
            break;
        } else if(ret==RTSTR) {
			
			//BugZilla No. 78265; 09-09-2002 [	
			struct resbuf rbtemp;
			int nUnitType = 0;
			if(SDS_getvar(NULL,DB_QAUNITS,&rbtemp,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)==RTNORM) 
				nUnitType=rbtemp.resval.rint;
			//BugZilla No. 78265; 09-09-2002 ]	

            if(strisame("REFERENCE"/*DNT*/,fs1) || strisame("BASE"/*DNT*/,fs1)) {
                for(;;) {
                    if(sds_initget(RSG_OTHER,NULL)!=RTNORM) {
                       rc=(-2);
                       goto bail;
                    }
                    if((ret=sds_getangle(NULL,ResourceString(IDC_SCALE_AND_ROTATE__NBASE_31, "\nBase angle <0>: " ),&gpak->refval))==RTERROR) {
                        rc=(-2); goto bail;
                    } else if(ret==RTCAN) {
                        rc=(-1); goto bail;
                    } else if(ret==RTNONE) {
                        gpak->refval=0;
                        break;
                    } else if(ret==RTNORM) {
                        break;
                    } else {
						cmd_ErrorPrompt(CMD_ERR_ANG2PTS,0);
                        continue;
                    }
                    break;
                }
                gpak->reference=1;
                continue;
// OLD WAY
//            } else if(ic_isnumstr(fs1,&theta)) {
//              theta=(theta*IC_PI/180);
//                break;
			}
			else if (SDS_angtof_convert(1,fs1,nUnitType,&theta)==RTNORM) { // returned angle is not takes into account ANGBASE value
				break;
			}
			else {
                if(!gpak->reference) cmd_ErrorPrompt(CMD_ERR_ANG2NDPTKWORD,0);
                else cmd_ErrorPrompt(CMD_ERR_ANG2NDPT,0);
                continue;
            }
        } else {
            if(!gpak->reference) cmd_ErrorPrompt(CMD_ERR_ANG2NDPTKWORD,0);
            else cmd_ErrorPrompt(CMD_ERR_ANG2NDPT,0);
            continue;
        }
    }
    //*** Setup matrix for sds_xformss.
    if(gpak->reference) // gpak->refval - is not takes into account ANGBASE value. 
		theta += angdir ? IC_TWOPI - gpak->refval : -gpak->refval;

	if(angdir)	   // ANGDIR should be taken into account
		theta = -(IC_TWOPI - theta);

    mtx[0][0]=cos(theta); mtx[0][1]=(-sin(theta)); mtx[0][2]=0.0; mtx[0][3]=((gpak->pt1[1]*sin(theta))+(gpak->pt1[0]*(1-cos(theta))));
    mtx[1][0]=sin(theta); mtx[1][1]=cos(theta);    mtx[1][2]=0.0; mtx[1][3]=(-(gpak->pt1[0]*sin(theta))+(gpak->pt1[1]*(1-cos(theta))));
    mtx[2][0]=0.0;        mtx[2][1]=0.0;           mtx[2][2]=1.0; mtx[2][3]=0.0;
    mtx[3][0]=0.0;        mtx[3][1]=0.0;           mtx[3][2]=0.0; mtx[3][3]=1.0;
    //***
    if(SDS_progxformss(ss1,mtx,1)!=RTNORM) { rc=(-2); goto bail; }

	// break association appropriately if moving one entity without the reactor
	// or update the reactors if have both.
	cmd_checkReactors (ss1);

bail:
	// redraw entities
	for(ssct = 0L; sds_ssname(ss1, ssct, ename) == RTNORM; ssct++)
		sds_redraw(ename, IC_REDRAW_UNHILITE);
    sds_ssfree(ss1);
    return(rc);
}


