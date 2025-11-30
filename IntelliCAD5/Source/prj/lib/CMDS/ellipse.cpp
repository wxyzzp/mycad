/* ELLIPSE.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * Supports the ellipse related commands
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
extern double SDS_CorrectElevation;
extern bool	  SDS_ElevationChanged;
extern int SDS_GetZScale;

// *****************************
// COMMAND FUNCTION
//
short cmd_ellipse(void) {
/*
**  ACAD handles the default include angle and param in an interesting
**  way.  They are initialized to 180 deg and then they are saved
**  every time an include angle or param is specified (BOTH are
**  save for EITHER type of entry).  But -- if an included is cancelled,
**  the dragging include angle/param is taken!  We can't do that
**  here, so we save the included values actually entered.
**  ACAD does not capture the include values if start/end are specified
**  in a manner not involving a prompt for included angle/param
**  (so neither do we).
*/

    if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    char pmpt[81],kwl[81],*qq;
    short pcode,pell,ssty,sip,doarc,npts,pidx;
    short ok,fi1,reso,breakcase,angdir;
	RT	rc;
    sds_real dist1,dist2,spar,epar,axisang,ar1,ar2,angbase,fr1,curelev,newelev;
    sds_point cent,zeropt,*pa,ap1,extru;
	sds_name ename;
    struct resbuf rb,*toentmake,*the10,rbucs,rbent,rbecs;
	db_handitem *elp=NULL;

    static sds_real iang=IC_PI,ipar=IC_PI;  /* The "included" ang/par */

    SDS_getvar(NULL,DB_QANGDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	angdir=rb.resval.rint;
    SDS_getvar(NULL,DB_QANGBASE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	angbase=rb.resval.rreal;

	rbent.restype=RTSHORT;
	rbucs.restype=RTSHORT;
	rbucs.resval.rint=1;
	rbent.resval.rint=0;
    rbecs.restype=RT3DPOINT;

    SDS_getvar(NULL,DB_QUCSXDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	ic_ptcpy(ap1,rb.resval.rpoint);
    SDS_getvar(NULL,DB_QUCSYDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	ic_crossproduct(ap1,rb.resval.rpoint,extru);
	ic_ptcpy(rbecs.resval.rpoint,extru);

    SDS_getvar(NULL,DB_QELEVATION,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	curelev=newelev=rb.resval.rreal;


    rc=RTNORM; toentmake=NULL; pa=NULL; npts=0; doarc=0;
    qq=""/*DNT*/;
//    numchars="+-.0123456789";  /* Possible 1st chars of a numeric entry. */

    reso=12;  /* # of chords per quadrant */

    /* Set safe defaults: */
    dist1=dist2=1.0; spar=0.0; epar=IC_TWOPI;
    cent[0]=cent[1]=cent[2]=0.0;
    zeropt[0]=1.0; zeropt[1]=zeropt[2]=0.0;

    pell=(SDS_getvar(NULL,DB_QPELLIPSE   ,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)==RTNORM) ? (rb.resval.rint!=0) : 0;
    ssty=(SDS_getvar(NULL,DB_QSNAPSTYL   ,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)==RTNORM) ? (rb.resval.rint!=0) : 0;
    sip= (SDS_getvar(NULL,DB_QSNAPISOPAIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)==RTNORM) ?  rb.resval.rint     : 0;
    if (sip<0) sip=0; else if (sip>2) sip=2;

    /*
    **  The following while and switch walk my flow chart of prompts;
    **  the pcode values are the prompt nodes on the chart:
    */
    pcode=1;
    while (pcode>0 && rc==RTNORM) {
        switch (pcode) {

            case 1:
                if (pell) {
                    if (ssty)  strcpy(kwl, ResourceString(IDC_ELLIPSE_CENTER_ISOCI_0, "Center Isocircle ~_Center ~_Isocircle" ));
                    if (!ssty) strcpy(kwl, ResourceString(IDC_ELLIPSE_CENTER_1, "Center ~_Center" ));
                } else {
                    if (ssty)  strcpy(kwl, ResourceString(IDC_ELLIPSE_ARC_CENTER_ISOCIRCLE_2, "Arc Center Isocircle ~_Arc ~_Center ~_Isocircle" ));
                    if (!ssty) strcpy(kwl, ResourceString(IDC_ELLIPSE_ARC_CENTER_3, "Arc Center ~_Arc ~_Center" ));
                }

                sprintf(pmpt,ResourceString(IDC_ELLIPSE__N_SCENTER__S_FI_3, "\n%sCenter/%s<First end of ellipse axis>: " ),
                    (pell ? qq : ResourceString(IDC_ELLIPSE_ARC__4, "Arc/" )),(ssty ? ResourceString(IDC_ELLIPSE_ISOCIRCLE__5, "Isocircle/" ) : qq));

                sds_initget(SDS_RSG_NONULL,kwl);
                rc=internalGetpoint(NULL,pmpt,zeropt);
                if (rc==RTNORM){
					pcode=3;
					if(!icadRealEqual(zeropt[2],curelev) && !ssty){
						SDS_CorrectElevation=curelev;
						SDS_ElevationChanged=TRUE;
						newelev=rb.resval.rreal=zeropt[2];
						rb.restype=RTREAL;
						sds_setvar("ELEVATION"/*DNT*/,&rb);
					}
                }else if (rc==RTKWORD && (rc=sds_getinput(kwl))==RTNORM) {
                    if(strisame(kwl,"ARC"/*DNT*/)) { 
                        doarc=1; 
                        pcode=2; 
                    } else {
                        if(strisame(kwl,"CENTER"/*DNT*/))
                           pcode = 12;
                        else
                            pcode = 14;
                    }
                }
                break;

            case 2:
                if (ssty)
                    strcpy(kwl, ResourceString(IDC_ELLIPSE_CENTER_ISOCI_0, "Center Isocircle ~_Center ~_Isocircle" ));
                else
                    strcpy(kwl, ResourceString(IDC_ELLIPSE_CENTER___CENTER_6, "Center ~_Center" ));
                sprintf(pmpt,ResourceString(IDC_ELLIPSE__NCENTER_S__FIRS_7, "\nCenter%s/<First end of ellipse axis>: " ),
                    (ssty ? ResourceString(IDC_ELLIPSE__ISOCIRCLE_8, "/Isocircle" ) : qq));
                sds_initget(SDS_RSG_NONULL,kwl);
                rc=internalGetpoint(NULL,pmpt,zeropt);
                if (rc==RTNORM){
					pcode=3;
					if(!icadRealEqual(zeropt[2],curelev) && !ssty){
						SDS_CorrectElevation=curelev;
						SDS_ElevationChanged=TRUE;
						newelev=rb.resval.rreal=zeropt[2];
						rb.restype=RTREAL;
						sds_setvar("ELEVATION"/*DNT*/,&rb);
					}
                }else if (rc==RTKWORD && (rc=sds_getinput(kwl))==RTNORM) {
                    if(strisame(kwl,"CENTER"/*DNT*/))
                           pcode = 12;
                        else
                            pcode = 14;
                }
                break;

            case 3:
                do {
                    ok=1;
                    sds_initget(SDS_RSG_NONULL,NULL);
                    rc=internalGetpoint(zeropt,ResourceString(IDC_ELLIPSE__NSECOND_END_OF__9, "\nSecond end of axis: " ),ap1);
                    if (rc==RTNORM) {
                        cent[0]=(ap1[0]+zeropt[0])/2.0;
                        cent[1]=(ap1[1]+zeropt[1])/2.0;
                        cent[2]=zeropt[2];
                        dist1=sqrt(
                            (zeropt[0]-cent[0])*(zeropt[0]-cent[0])+
                            (zeropt[1]-cent[1])*(zeropt[1]-cent[1]));
                        if ((ok=(dist1>0.0))==0) cmd_ErrorPrompt(CMD_ERR_TRYAGAIN,0);
                        else pcode=4;
                    }
                } while (!ok);
                break;

            case 4:
                do {
                    ok=1;
					ap1[0]=zeropt[0]-cent[0];
					ap1[1]=zeropt[1]-cent[1];
					ap1[2]=zeropt[2]-cent[2];
					elp=new db_ellipse(SDS_CURDWG);

                    sds_point pt1,pt2;
                    int oldGetZScale = SDS_GetZScale;

					elp->set_210(extru);
					elp->set_40(dist2);
					elp->set_41(spar);
					elp->set_42(epar);
					sds_trans(cent,&rbucs,&rbent,0,pt1);
					elp->set_10(pt1);
					sds_trans(ap1,&rbucs,&rbent,1,pt1);
					elp->set_11(pt1);
    				ename[0]=(long)elp;
					ename[1]=(long)SDS_CURDWG;
					rb.restype=RT3DPOINT;
					ic_ptcpy(rb.resval.rpoint,cent);	// Use center point of ellipse ("cent") as the new LASTPOINT
					SDS_setvar(NULL,DB_QLASTPOINT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
					sds_initget(SDS_RSG_2D|SDS_RSG_NONEG|SDS_RSG_NOZERO|SDS_RSG_NONULL,ResourceString(IDC_ELLIPSE_INITGET_ROTATIO_10, "Rotation ~_Rotation" ) );
					SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
                    // Set SDS_GetZScale value to 1 temporarily to allow SDS_AskForPoint() 
                    // to accept 3D input point to fix the bug #78086 - SWH 27/8/2004
                    SDS_GetZScale = 1;      // Set to 
					rc=SDS_dragobject(-9,rb.resval.rint,(sds_real *)ename,
						 (double *)elp->retp_40(),
						 0.0,ResourceString(IDC_ELLIPSE__NROTATION__OTH_11, "\nRotation/<Other axis>: " ),(struct resbuf **)cent,ap1,NULL);
                    SDS_GetZScale = oldGetZScale;
					dist2=ap1[0]*dist1;
                    //rc=sds_getdist(cent,"\nRotation/<Other axis>: ",
                    //    &dist2);

                    if (rc==RTNORM) {
                        if ((ok=(ap1[0]<=100.0 && ap1[0]>=0.01))==0){
                            cmd_ErrorPrompt(CMD_ERR_TRYAGAIN,0);
                        }else{
                            pcode=(doarc) ? 5 : 0;

                            if(!pell && dist2>dist1){
                                //pt1 is 11 vect in entity system.  rotate 90deg ccw and scale it up
                                //put it back in ucs coords
                                sds_trans(pt1,&rbent,&rbucs,1,pt1);
                                pt2[2]=pt1[2]*dist2/dist1;
                                pt2[0]=-pt1[1]*dist2/dist1;
                                pt2[1]=pt1[0]*dist2/dist1;
                                //recalc zeropt from new vector and center
                                zeropt[0]=cent[0]+pt2[0];
                                zeropt[1]=cent[1]+pt2[1];
                                zeropt[2]=cent[2]+pt2[2];
                                fr1=dist2;
                                dist2=dist1;
                                dist1=fr1;
                            }
                        }
                   } else if (rc==RTKWORD && (rc=sds_getinput(kwl))==RTNORM) {
                        pcode=11;
                    }
                } while (!ok);
                break;

            case 5: case 6:

                if (!doarc) { pcode=0; break; }  /* Precaution. */
                for(;;){
					//temporarily set angbase to the base angle for this ellipse
					SDS_getvar(NULL,DB_QANGBASE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
					angbase=rb.resval.rreal;
					rb.resval.rreal=atan2(zeropt[1]-cent[1],zeropt[0]-cent[0]);
					ic_normang(&rb.resval.rreal,NULL);
					SDS_setvar(NULL,DB_QANGBASE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
                    if (pcode == 5)
                        strcpy(kwl, ResourceString(IDC_ELLIPSE_PARAMETER___PAR_12, "Parameter ~_Parameter" ));
                    else
                        strcpy(kwl, ResourceString(IDC_ELLIPSE_ANGLE__ANGLE_13, "Angle ~_Angle" ));
                    sds_initget(1,kwl);
                    rc=sds_getangle(cent,(pcode==5)?ResourceString(IDC_ELLIPSE__NPARAMETER__ST_14, "\nParameter/<Start angle of arc>: " ):ResourceString(IDC_ELLIPSE__NANGLE__START__15, "\nAngle/<Start parameter>: " ),&spar);
					//set angbase back regardless of what user did
					rb.resval.rreal=angbase;
					SDS_setvar(NULL,DB_QANGBASE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
                    if(rc==RTCAN){
                        goto out;
                    }else if(rc==RTKWORD){
                        sds_getinput(kwl); 
                        strupr(kwl);
                        if(strisame(kwl,(pcode==5) ? "PARAMETER"/*DNT*/ : "ANGLE"/*DNT*/)) 
                            pcode=5+(pcode==5);
                    }else if(rc==RTNORM){
                        if(pcode==5) ic_angpar(spar,&spar,dist1,dist2,0); /* Convert to eccentric anomaly. */
                        break;
                    }else{
                        cmd_ErrorPrompt(CMD_ERR_ANG2NDPTKWORD,0);
                    }
                }
                pcode=(pcode==5) ? 8 : 7;
                break;

            case 7: case 8:

                axisang=sds_angle(cent,zeropt);
                breakcase=0;
                do {
                    ok=1;
                    if (pcode == 7)
                        strcpy(kwl, ResourceString(IDC_ELLIPSE_ANGLE_INCLUDED__16, "Angle Included_angle|Included ~_Angle ~_Included" ));
                    else
                        strcpy(kwl, ResourceString(IDC_ELLIPSE_PARAMETER_INCLU_17, "Parameter Included_angle|Included ~_Parameter ~_Included" ));

                    sds_initget(SDS_RSG_OTHER,kwl);
					SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
					if(elp==NULL) elp=new db_ellipse(SDS_CURDWG);
					sds_point pt1;
					ap1[0]=zeropt[0]-cent[0];
					ap1[1]=zeropt[1]-cent[1];
					ap1[2]=zeropt[2]-cent[2];
                    sds_trans(ap1,&rbucs,&rbent,1,pt1);
                    elp->set_11(pt1);
					elp->set_41(spar);
					sds_trans(cent,&rbucs,&rbent,0,pt1);
					elp->set_10(pt1);
					fr1=dist2/dist1;
					elp->set_40(fr1);
					elp->set_210(extru);
					//temporarily set angbase to the base angle for this ellipse,
					//	and keep value in fr1 for later
					SDS_getvar(NULL,DB_QANGBASE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
					angbase=rb.resval.rreal;
					rb.resval.rreal=atan2(zeropt[1]-cent[1],zeropt[0]-cent[0]);
					ic_normang(&rb.resval.rreal,NULL);
					fr1=rb.resval.rreal;
					SDS_setvar(NULL,DB_QANGBASE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
					if(pcode==7)
						rc=SDS_dragobject(-3,rb.resval.rint,(sds_real *)ename,
							 (double *)elp->retp_42(),
							 0.0,ResourceString(IDC_ELLIPSE__NANGLE_INCLUDE_18, "\nAngle/Included/<End parameter>: " ),(struct resbuf **)cent,ap1,NULL);
					else
						//-10 drags by angle and updates database by parameter
						rc=SDS_dragobject(-10,rb.resval.rint,(sds_real *)ename,
							 (double *)elp->retp_42(),
							 0.0,ResourceString(IDC_ELLIPSE__NPARAMETER_INC_19, "\nParameter/Included/<End angle>: " ),(struct resbuf **)cent,ap1,NULL);
					//set angbase back regardless of what user did
					rb.resval.rreal=angbase;
					SDS_setvar(NULL,DB_QANGBASE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
                    if (rc==RTNORM){  /* User PICKED a pt. */
						ap1[0]-=fr1;
						ic_normang(ap1,NULL);
						if(pcode==8)ic_angpar(ap1[0],&epar,dist1,dist2,0);
						else epar=ap1[0];
                    } else if (rc==RTNONE) {
                        ok=0;
                    } else if (rc==RTKWORD && (rc=sds_getinput(kwl))==RTNORM) {
                        /* TYPED keyword or angle value. */
                        strupr(kwl);
                        if (!(*kwl)) {
                            ok=0;
                        } else if (strisame(kwl,(pcode==7) ? "ANGLE"/*DNT*/ : "PARAMETER"/*DNT*/)) {
                            pcode=7+(pcode==7); 
                            breakcase=1;
                        } else if (strisame(kwl,"INCLUDED"/*DNT*/)  ) {
                            pcode+=2; 
                            breakcase=1;
                        }
						/*else if (strchr(numchars,*kwl)==NULL) {
                            ok=0;
                        } else if (sds_angtof(kwl,-1,&epar)==RTNORM) {
                            if (dist2>dist1) epar+=IC_PI/2.0;
                        } else {
                            epar=0.0;
                        }*/
                    }
                    if (!ok) cmd_ErrorPrompt(CMD_ERR_ANG2NDPTKWORD,0);
                } while (!ok && !breakcase);
                if (breakcase) break;


                pcode=0;

                break;

            case 9:

				if(angdir==0)
					fr1=ipar+angbase;
				else
					fr1=ipar-angbase;
				if(angdir==1)fr1-=IC_TWOPI;
                if (sds_angtos(fr1,-1,-1,kwl)!=RTNORM) 
                    *kwl=0;
                sprintf(pmpt,ResourceString(IDC_ELLIPSE__NINCLUDED_PARA_20, "\nIncluded parameter <%s>: " ),kwl);
                if ((rc=sds_getorient(cent,pmpt,&ar1))==RTNONE) { 
                    rc=RTNORM; 
                    ar1=ipar; 
                }
                ic_normang(&ar1,NULL);
                if (fabs(ar1)<1.0E-10) {
                    spar=0.0; epar=IC_TWOPI; doarc=0; iang=ipar=0.0;
                    pcode=0; break;
                }
                epar=spar+ar1;
                ipar=ar1;

                /* Set the static included angle: */
                ic_angpar(spar,&ar1,dist1,dist2,1);
                ic_angpar(epar,&ar2,dist1,dist2,1);
                ic_normang(&ar1,&ar2);
                iang=ar2-ar1;

                pcode=0;

                break;

            case 10:

				if(angdir==0)
					fr1=ipar+angbase;
				else
					fr1=ipar-angbase;
				if(angdir==1)fr1-=IC_TWOPI;
                if (sds_angtos(fr1,-1,-1,kwl)!=RTNORM) *kwl=0;
                sprintf(pmpt,ResourceString(IDC_ELLIPSE__NINCLUDED_ANGL_21, "\nIncluded angle of arc <%s>: " ),kwl);
                if ((rc=sds_getorient(cent,pmpt,&ar1))==RTNONE) { 
                    rc=RTNORM; 
                    ar1=iang; 
                }
                ic_normang(&ar1,NULL);
                if (fabs(ar1)<=1.0E-10) {
                    spar=0.0; epar=IC_TWOPI; doarc=0; iang=ipar=0.0;
                    pcode=0; break;
                }
                iang=ar1;
                ic_angpar(spar,&ar1,dist1,dist2,1);  /* Get start ANG */
                ar1+=iang;                           /* End ANG */
                ic_angpar(ar1,&epar,dist1,dist2,0);  /* Set end PAR */
                ic_normang(&spar,&epar);
                ipar=epar-spar;

                pcode=0;

                break;

            case 11:
                do {
                    ok=1;
					/*
					**Acad isn't ersetting angbase to the base angle for this ellipse.  Don't ask me why!!!
					//temporarily set angbase to the base angle for this ellipse
					SDS_getvar(NULL,DB_QANGBASE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
					angbase=rb.resval.rreal;
					rb.resval.rreal=atan2(zeropt[1]-cent[1],zeropt[0]-cent[0]);
					ic_normang(&rb.resval.rreal,NULL);
					SDS_setvar(NULL,DB_QANGBASE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
					*/
                    sds_initget(SDS_RSG_NONULL,NULL);
                    rc=sds_getangle(cent,ResourceString(IDC_ELLIPSE__NROTATION_AROU_22, "\nRotation around major axis: " ),&ar1);
					/*
					//set angbase back regardless of what user did
					rb.resval.rreal=angbase;
					SDS_setvar(NULL,DB_QANGBASE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
					*/
                    if (rc==RTNORM) {
                        dist2=fabs(cos(ar1))*dist1;
                        if ((ok=(dist2<=100.0*dist1 && dist2>=0.01*dist1))==0)
                            cmd_ErrorPrompt(CMD_ERR_TRYAGAIN,0);
                        else pcode=(doarc) ? 5 : 0;
                    }
                } while (!ok);
                break;

            case 12: case 14:
                sds_initget(SDS_RSG_NONULL,NULL);
                sprintf(pmpt,ResourceString(IDC_ELLIPSE__NCENTER_OF__S__23, "\nCenter of %s: " ),(pcode==12) ?
                    ResourceString(IDC_ELLIPSE_ELLIPSE_24, "ellipse" ) : ResourceString(IDC_ELLIPSE_CIRCLE_25, "circle" ));
                rc=internalGetpoint(NULL,pmpt,cent);
				if(rc==RTNORM && pcode==12 && !ssty){
					if(!icadRealEqual(cent[2],curelev) && !ssty){
						SDS_CorrectElevation=curelev;
						SDS_ElevationChanged=TRUE;
						newelev=rb.resval.rreal=cent[2];
						rb.restype=RTREAL;
						sds_setvar("ELEVATION"/*DNT*/,&rb);
					}
				}
                pcode=(pcode==12) ? 13 : 15;
                break;

            case 13:
                do {
                    ok=1;
                    sds_initget(SDS_RSG_NONULL,NULL);
                    rc=internalGetpoint(cent,ResourceString(IDC_ELLIPSE__NENDPOINT_OF_A_26, "\nEndpoint of axis: " ),zeropt);
                    if (rc==RTNORM) {
                        zeropt[2]=cent[2];
                        dist1=sqrt(
                            (zeropt[0]-cent[0])*(zeropt[0]-cent[0])+
                            (zeropt[1]-cent[1])*(zeropt[1]-cent[1]));
                        if ((ok=(dist1>0.0))==0) cmd_ErrorPrompt(CMD_ERR_TRYAGAIN,0);
                        else pcode=4;
                    }
                } while (!ok);
                break;

            case 15:
                sds_initget(SDS_RSG_2D|SDS_RSG_NONEG|SDS_RSG_NOZERO|SDS_RSG_NONULL,ResourceString(IDC_ELLIPSE_INITGET_DIAMETE_27, "Diameter ~_Diameter" ));
                rc=sds_getdist(cent,ResourceString(IDC_ELLIPSE__NDIAMETER__RAD_28, "\nDiameter/<Radius of circle>: " ),
                    &dist1);
                if (rc==RTKWORD && (rc=sds_getinput(kwl))==RTNORM) {
                    sds_initget(SDS_RSG_2D|SDS_RSG_NONEG|SDS_RSG_NOZERO|SDS_RSG_NONULL,NULL);
                    rc=sds_getdist(cent,ResourceString(IDC_ELLIPSE__NDIAMETER_OF_C_29, "\nDiameter of circle: " ),&dist1);
                    dist1/=2.0;
                }
                if (rc!=RTNORM) break;

			    sip= (SDS_getvar(NULL,DB_QSNAPISOPAIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)==RTNORM) ?  rb.resval.rint     : 0;
				if (sip<0) sip=0; else if (sip>2) sip=2;

                dist1*=sqrt(1.5);  /* sqrt(2)*cos(30 deg) */
                dist2=dist1/sqrt(3.0);
                axisang=((sip==0) ? 5.0 : ((sip==1) ? 3.0 : 4.0))*IC_PI/3.0;
                zeropt[0]=cent[0]+dist1*cos(axisang);
                zeropt[1]=cent[1]+dist1*sin(axisang);
                zeropt[2]=cent[2];
                pcode=(doarc) ? 5 : 0;
                break;

        }  /* End switch */

    }  /* End while pcode */

    if (rc!=RTNORM || pcode) goto out;


	if(!pell){
		//build ellipse entity
		if (toentmake!=NULL) { sds_relrb(toentmake); toentmake=NULL; }
		fr1=dist2/dist1;
		if(!doarc){
			spar=0.0;
			epar=IC_TWOPI;
		}
		//get vector for semimajor axis
		ap1[0]=zeropt[0]-cent[0];
		ap1[1]=zeropt[1]-cent[1];
		ap1[2]=zeropt[2]-cent[2];
		sds_trans(ap1,&rbucs,&rbent,1,ap1);
		sds_trans(cent,&rbucs,&rbent,0,cent);
		if ((toentmake=sds_buildlist(
			RTDXF0,"ELLIPSE"/*DNT*/,
				10,cent,
				11,ap1,
				40,fr1,
				41,spar,
				42,epar,
				210,extru,
				0
			))==NULL) { rc=RTERROR; goto out; }
		rc=sds_entmake(toentmake);
		goto out;
	}
	//Build ellipse as a pline...
// determine which kind of pline
    int plinetype;
	SDS_getvar(NULL,DB_QPLINETYPE   ,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	plinetype=rb.resval.rint;
	if (plinetype==0) // make a regular polyline
		{
	/* Build the array of points: */
		fi1=ic_ellpts(cent,zeropt,dist2,spar,epar,1,(short)(!doarc),reso,&npts,&pa);
		if (fi1 || npts<2 || pa==NULL) { rc=RTERROR; goto out; }

		/* Create POLYLINE entity: */
		FreeResbufIfNotNull(&toentmake);
		cent[0]=cent[1]=0.0;
		sds_trans(cent,&rbucs,&rbecs,0,cent);
		if ((toentmake=sds_buildlist(RTDXF0,"POLYLINE"/*DNT*/,66,1,10,cent,        /* For the elevation */
				70,!doarc,40,0.0,41,0.0,210,extru,0))==NULL)
			{
			rc=RTERROR;
			goto out; 
			}
		if ((rc=sds_entmake(toentmake))!=RTNORM)
			goto out;

		/* Alloc a VERTEX elist and get the address of the 10-link: */
		FreeResbufIfNotNull(&toentmake);
		
		//Modified Cybage SBD 03/02/2001 
		//Reason : Fix for Bug No. 6988 from Bugzilla 
		//toentmake=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,10,pa[0],40,0.0,41,0.0,0);		
		toentmake=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,10,pa[0],40,0.0,41,0.0,42,0.05,0);

		for (the10=toentmake; the10!=NULL && the10->restype!=10; the10=the10->rbnext);
		if (the10==NULL)
			{
			rc=RTERROR;
			goto out; 
			}

		/* Walk the points and make the VERTEXs by changing the 10-pt: */
		for (pidx=0; pidx<npts-!doarc; pidx++) 
			{
			sds_trans(pa[pidx],&rbucs,&rbecs,0,the10->resval.rpoint);
			//ic_ptcpy(the10->resval.rpoint,pa[pidx]);
			if ((rc=sds_entmake(toentmake))!=RTNORM)
				goto out;
			}

		/* Create the SEQEND: */
		FreeResbufIfNotNull(&toentmake);
		if ((toentmake=sds_buildlist(RTDXF0,"SEQEND"/*DNT*/,0))==NULL)
			{
			rc=RTERROR;
			goto out; 
			}
		if ((rc=sds_entmake(toentmake))!=RTNORM)
			goto out;
		}
	else	// make a lightweight polyline
		{
		/* Build the array of points: */
		fi1=ic_ellpts(cent,zeropt,dist2,spar,epar,1,(short)(!doarc),reso,&npts,&pa);
		if (fi1 || npts<2 || pa==NULL) { rc=RTERROR; goto out; }

		FreeResbufIfNotNull(&toentmake);
		cent[0]=cent[1]=0.0;
		sds_trans(cent,&rbucs,&rbecs,0,cent);
		if ((toentmake=sds_buildlist(RTDXF0,"LWPOLYLINE"/*DNT*/,38,cent[2],        /* For the elevation */
				210,extru,90,0,70,!doarc,0))==NULL)
			{
			rc=RTERROR;
			goto out; 
			}

		resbuf *lwpladdrb=toentmake;
		while (lwpladdrb->rbnext!=NULL)
			lwpladdrb=lwpladdrb->rbnext;
		/* Walk the points and make the VERTEXs by changing the 10-pt: */
		for (pidx=0; pidx<npts-!doarc; pidx++) 
			{
			sds_point temppt;
			sds_trans(pa[pidx],&rbucs,&rbecs,0,temppt);
			//ic_ptcpy(the10->resval.rpoint,pa[pidx]);

			//Modified Cybage SBD 03/02/2001 
			//Reason : Fix for Bug No. 6988 from Bugzilla 
			//LwplrbAddVertFromData(&lwpladdrb,temppt,0,0,0);
			LwplrbAddVertFromData(&lwpladdrb,temppt,0,0,0.05);

			}

		LwplrbSetNumVerts(toentmake);

		if ((rc=sds_entmake(toentmake))!=RTNORM)
			goto out;
		}

out:
   	if(elp) 
		{
		delete elp;
		elp=NULL; 
		}
	FreeResbufIfNotNull(&toentmake);
    if (pa!=NULL) 
		{
		IC_FREE(pa);
		pa=NULL;
		npts=0; 
		}
    sds_entmake(NULL);  /* In case we aborted in the middle somehow. */

	if (SDS_ElevationChanged)
		{
		rb.restype=RTREAL;
		rb.resval.rreal=curelev;
		sds_setvar("ELEVATION"/*DNT*/,&rb);
		SDS_ElevationChanged=FALSE;
		}
    return rc;
	}





