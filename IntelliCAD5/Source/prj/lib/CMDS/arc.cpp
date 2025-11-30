/* ARC.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * Supports the  arc commands
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


// *******************
// COMMAND FUNCTION
//

short cmd_arc(void) {
//*** Draws an arc of any size.
//*** ENTER as reply to "Start point:" - Sets start point and direction as
//***                                    end of previous Line or Arc.
//*** A - Included angle.
//*** C - Center point.
//*** D - Starting direction.
//*** E - Endpoint.
//*** L - Length of chord.
//*** R - Radius.
//***
//*** RETURNS: -1 - User cancel.
//***          -2 - RTERROR returned from an ads function.
//***		   -3 - Unable to create arc w/requested geometry (collinear pts)
//***
//*** Outstanding concerns with this command:
//***   1) Dragmode
//***   2) Continuing an arc (or line) from an existing arc or line (LASTANGLE).
//***   3) Transparent commands.  Especially - redraw and turning grid mode
//***      on and off.
//***   4) Tangents
//***

    if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    char fs1[IC_ACADBUF];
    sds_point pt1,pt2,pt3,stpt;
    sds_real curelev,fr1,fr2,fr3,newelev;
    struct resbuf setgetrb,*newarc,*rslt,rb,rblast,rbucs,rbwcs,rbecs;
    RT ret;
	short centerflg=0,endflg=0,gotstpt=0,rc=0;

    rslt=NULL;
	rbucs.restype=rbwcs.restype=RTSHORT;
	rbucs.resval.rint=1;
	rbwcs.resval.rint=0;
	rbecs.restype=RT3DPOINT;
	SDS_getvar(NULL,DB_QUCSXDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	ic_ptcpy(pt1,rb.resval.rpoint);
	SDS_getvar(NULL,DB_QUCSYDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	ic_ptcpy(pt2,rb.resval.rpoint);
	ic_crossproduct(pt1,pt2,rbecs.resval.rpoint);
	//*** Get current elevation to check for valid point values.
	if(SDS_getvar(NULL,DB_QELEVATION,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) return(-2);
	curelev=newelev=setgetrb.resval.rreal;

    //*** First point.
    for(;;) {
        //*** Pass RSG_OTHER to sds_initget in case user enters C.
		SDS_getvar(NULL,DB_QISLASTCMDPT,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
        if(rblast.resval.rint){
           if(sds_initget(SDS_RSG_OTHER,ResourceString(IDC_ARC_INITGET_CENTER_CE_FO_0, "Center|Ce Follow ~_Ce ~_Follow" ))!=RTNORM) {
               rc=-2;
               goto exit;
           }
			ret=internalGetpoint(NULL,ResourceString(IDC_ARC__NENTER_TO_USE_LAST__1, "\nENTER to use last point/Center/Follow/<Start of arc>: " ),pt1);
		}else{
           if(sds_initget(SDS_RSG_OTHER,ResourceString(IDC_ARC_INITGET_CENTER_CE_2, "Center|Ce ~_Ce" ))!=RTNORM)  {
               rc=-2;
               goto exit;
           }
			ret=internalGetpoint(NULL,ResourceString(IDC_ARC__NCENTER__START_OF_A_3, "\nCenter/<Start of arc>: " ),pt1);
		}

		if(ret==RTERROR){
             rc=-2;goto exit;
        } else if(ret==RTCAN) {
            return(-1);
        } else if(ret==RTNORM) {
            ic_ptcpy(stpt,pt1);  gotstpt=1;
			if(!icadRealEqual(pt1[2],curelev)){
				SDS_CorrectElevation=curelev;
				SDS_ElevationChanged=TRUE;
				newelev=setgetrb.resval.rreal=pt1[2];
				setgetrb.restype=RTREAL;
				sds_setvar("ELEVATION"/*DNT*/,&setgetrb);
			}
            break;
        } else if(ret==RTNONE || ret==RTKWORD) {
			sds_getinput(fs1);
			if(ret==RTNONE || strisame(fs1,"FOLLOW"/*DNT*/)) {
				//*** If ENTER was pressed we need to check for a LASTPOINT.
		        SDS_getvar(NULL,DB_QISLASTCMDPT,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				if(!rblast.resval.rint) {
					cmd_ErrorPrompt(CMD_ERR_NOCONTINUE,0);
					continue;
				}
				//*** Get pt1=LASTCMDPT variable.
				SDS_getvar(NULL,DB_QLASTCMDPT,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				ic_ptcpy(pt1,rblast.resval.rpoint);
				ic_ptcpy(stpt,pt1);  gotstpt=1;
				if(!icadRealEqual(stpt[2],curelev)){
					SDS_CorrectElevation=curelev;
					SDS_ElevationChanged=TRUE;
					newelev=setgetrb.resval.rreal=stpt[2];
					setgetrb.restype=RTREAL;
					sds_setvar("ELEVATION"/*DNT*/,&setgetrb);
				}
				//*** Get second point and return.
				for(;;) {
					//*** Get LASTANGLE and point values.
					if(SDS_getvar(NULL,DB_QLASTANGLE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) return(-2);

					//*** ~| If the last point is an end point of an arc, I need to
					//***    take different steps.  The function I call requires a last
					//***    angle parameter which I have to figure out how we want
					//***    to set.  AutoCAD's LASTANGLE system variable is read-only,
					//***    therefore I cannot set it after I draw a line or arc
					//***    (which is what the cmd_lastangle global variable is for).
					//***    The following call to SDS_dragobject works ONLY on a line.

                   if(sds_initget(0,NULL)!=RTNORM)
                       return(-2);
					SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
					SDS_getvar(NULL,DB_QLASTCMDANG,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
                    if((ret=SDS_dragobject(15,rb.resval.rint,pt1,pt2,rblast.resval.rreal,ResourceString(IDC_ARC__NEND_OF_ARC___5, "\nEnd of arc: " ),&rslt,pt3,NULL))==RTERROR) {
						rc=-2;goto exit;
					} else if(ret==RTCAN) {
						 rc=-1;goto exit;
					} else if(ret==RTNORM) {
						if(!icadRealEqual(pt3[2],pt1[2]) && !icadRealEqual(pt3[2],curelev)) {
							cmd_ErrorPrompt(CMD_ERR_2DPT,0);
							continue;
						}
						if(rslt==NULL)  {rc=-3;goto exit;}
						//*** Create entity.
                       newarc=sds_buildlist(RTDXF0,"ARC"/*DNT*/,0); newarc->rbnext=rslt;
						if(sds_entmake(newarc)!=RTNORM) {
							cmd_ErrorPrompt(CMD_ERR_CREATEENT,0);
							IC_RELRB(newarc);rc=-2;goto exit;
						}
						ic_assoc(newarc,10);
						ic_ptcpy(pt1,ic_rbassoc->resval.rpoint);
						ic_assoc(newarc,51);
						rblast.restype=RTREAL;
                        rblast.resval.rreal=ic_rbassoc->resval.rreal+(IC_PI/2.0);
						SDS_setvar(NULL,DB_QLASTCMDANG,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
						SDS_setvar(NULL,DB_QLASTANGLE,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
                        ic_assoc(newarc,40);
						sds_polar(pt1,rblast.resval.rreal-(IC_PI/2.0),ic_rbassoc->resval.rreal,pt2);
						if(gotstpt && sds_distance(pt2,stpt)<sds_distance(pt2,pt3)) {
							ic_assoc(newarc,50);
    						rblast.restype=RTREAL;
							rblast.resval.rreal=ic_rbassoc->resval.rreal-(IC_PI/2.0);
							SDS_setvar(NULL,DB_QLASTCMDANG,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
							SDS_setvar(NULL,DB_QLASTANGLE,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
							ic_assoc(newarc,40);
							sds_polar(pt1,rblast.resval.rreal+(IC_PI/2.0),ic_rbassoc->resval.rreal,pt2);
						}
						IC_RELRB(newarc);
						//*** Set LASTCMDPT,ISLASTCMDPT variable.
	                    rblast.restype=RT3DPOINT;
                        ic_ptcpy(rblast.resval.rpoint,pt2);
						SDS_setvar(NULL,DB_QLASTCMDPT,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
						rblast.restype=RTSHORT;
                        rblast.resval.rint=1;
						SDS_setvar(NULL,DB_QISLASTCMDPT,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);

                         goto exit;
					} else {
						cmd_ErrorPrompt(CMD_ERR_2DPT,0);
						continue;
					}
				}
			} else if(strisame(fs1,"CE"/*DNT*/)) {
                for(;;) {
                    if(sds_initget(0,NULL)!=RTNORM)
                       return(-2);
                    if((ret=internalGetpoint(NULL,ResourceString(IDC_ARC__NCENTER_OF_ARC___7, "\nCenter of arc: " ),pt1))==RTERROR) {
                         rc=-2;goto exit;
                    } else if(ret==RTCAN) {
                         rc=-1;goto exit;
                    } else if(ret==RTNORM) {
                        centerflg=1;
						if(!icadRealEqual(pt1[2],curelev)){
							SDS_CorrectElevation=curelev;
							SDS_ElevationChanged=TRUE;
							newelev=setgetrb.resval.rreal=pt1[2];
							setgetrb.restype=RTREAL;
							sds_setvar("ELEVATION"/*DNT*/,&setgetrb);
						}
                        break;
                    } else {
                        cmd_ErrorPrompt(CMD_ERR_PT,0);
                        continue;
                    }
                }
            } else {
                cmd_ErrorPrompt(CMD_ERR_PTKWORD,0);
                continue;
            }
            break;
        } else{
			rc=ret;goto exit;
		}
    }

    //*** Second point.
    if(centerflg) {
        //*** If pt1 is a center point.
        for(;;) {
            if(sds_initget(0,NULL)!=RTNORM) return(-2);
            if((ret=internalGetpoint(NULL,ResourceString(IDC_ARC__NSTART___8, "\nStart: " ),pt2))==RTERROR) {
                 rc=-2;goto exit;
            } else if(ret==RTCAN) {
                 rc=-1;goto exit;
            } else if(ret==RTNORM) {
                if(!icadRealEqual(pt2[2],pt1[2]) && !icadRealEqual(pt2[2],curelev)) {
                    cmd_ErrorPrompt(CMD_ERR_2DPT,0);
                    continue;
                }
                if(icadRealEqual(pt1[0],pt2[0]) && icadRealEqual(pt1[1],pt2[1])) {
                    cmd_ErrorPrompt(CMD_ERR_BADGEOMETRY,0);
                    continue;
                }
                pt2[2]=pt1[2];
                ic_ptcpy(stpt,pt2);  gotstpt=1;
                break;
            } else {
                cmd_ErrorPrompt(CMD_ERR_2DPT,0);
                continue;
            }
        }
    } else {
        //*** If pt1 is a starting point.
        for(;;) {
            //*** Pass RSG_OTHER to sds_initget in case user enters kword.
			//NOTE: Kword for End is EN.  This is for compatibility w/Acad and to avoid end osnap
            if(sds_initget(SDS_RSG_OTHER,ResourceString(IDC_ARC_INITGET_ANGLE_CENTER_9, "Angle Center|Ce Direction End|En Radius ~_Angle ~_Ce ~_Direction ~_En ~_Radius" ))!=RTNORM)
               return(-2);
            if((ret=internalGetpoint(pt1,ResourceString(IDC_ARC__NANGLE_CENTER_DIRE_10, "\nAngle/Center/Direction/End/Radius/<Second point>: " ),pt2))==RTERROR) {
                 rc=-2;goto exit;
            } else if(ret==RTCAN) {
                 rc=-1;goto exit;
            } else if(ret==RTNORM) {
                if(!icadRealEqual(pt2[2],pt1[2]) && !icadRealEqual(pt2[2],curelev)) {
                    cmd_ErrorPrompt(CMD_ERR_2DPTKWORD,0);
                    continue;
                }
                pt2[2]=pt1[2];
                break;
            } else if(ret==RTKWORD) {
                if(sds_getinput(fs1)!=RTNORM) return(-2);
                if(strisame(fs1,"CE"/*DNT*/)) {
                    for(;;) {
                        if(sds_initget(0,NULL)!=RTNORM)
                           return(-2);
                        if((ret=internalGetpoint(NULL,ResourceString(IDC_ARC__NCENTER_OF_ARC___7, "\nCenter of arc: " ),pt2))==RTERROR) {
                             rc=-2;goto exit;
                        } else if(ret==RTCAN) {
                             rc=-1;goto exit;
                        } else if(ret==RTNORM) {
                            if(!icadRealEqual(pt2[2],pt1[2]) && icadRealEqual(pt2[2],curelev)) {
                                cmd_ErrorPrompt(CMD_ERR_2DPT,0);
                                continue;
                            }
                            pt2[2]=pt1[2];
                            centerflg=2;
                            break;
                        } else {
                            cmd_ErrorPrompt(CMD_ERR_2DPT,0);
                            continue;
                        }
                    }
                } else if(strisame(fs1,"EN"/*DNT*/)) {
                    for(;;) {
                        if(sds_initget(0,NULL)!=RTNORM)
                           return(-2);
						//Modified Cybage AJK 14/01/2002
						//Reason: Fix for Bug no 77935 from Bugzilla
                        /*if((ret=internalGetpoint(NULL,ResourceString(IDC_ARC__NEND_OF_ARC___5, "\nEnd of arc: " ),pt2))==RTERROR) {
                            rc=-2;goto exit;*/
						if((ret=internalGetpoint(pt1,ResourceString(IDC_ARC__NEND_OF_ARC___5, "\nEnd of arc: " ),pt2))==RTERROR) {
                            rc=-2;goto exit;
						//Modified Cybage AJK 14/01/2002 DD/MM/YYYY ]
                        } else if(ret==RTCAN) {
                            rc=-1;goto exit;
                        } else if(ret==RTNORM) {
                            if(!icadRealEqual(pt2[2],pt1[2]) && !icadRealEqual(pt2[2],curelev)) {
                                cmd_ErrorPrompt(CMD_ERR_2DPT,0);
                                continue;
                            }
                            pt2[2]=pt1[2];
                            endflg=1;
                            break;
                        } else {
                            cmd_ErrorPrompt(CMD_ERR_2DPT,0);
                            continue;
                        }
                    }
                } else if(strisame(fs1,"RADIUS"/*DNT*/)) {
					for(;;){
						//if(sds_initget(SDS_RSG_NONULL,NULL)!=RTNORM)return(-2);
						if((ret=sds_getdist(NULL,ResourceString(IDC_ARC__NRADIUS_OF_ARC___13, "\nRadius of arc: " ),&fr1))==RTERROR) {
                            rc=-2;goto exit;
                        } else if(ret==RTCAN) {
                            rc=-1;goto exit;
                        } else if(ret==RTNORM) {
                            break;
						}
					}
					SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
					sds_initget(0,ResourceString(IDC_ARC_INITGET__INCLUDED_I_14, "~Included Included_angle|Angle ~_Included ~_Angle" ));
					ret=SDS_dragobject(23,rb.resval.rint,stpt,pt2,fr1,ResourceString(IDC_ARC__NINCLUDED_ANGLE__E_15, "\nIncluded Angle/<End of arc>: " ),&rslt,pt3,fs1);
					if(ret==RTKWORD || ret==RTSTR){
						sds_initget(SDS_RSG_NOZERO,NULL);
                        if((ret=sds_getorient(stpt,ResourceString(IDC_ARC__NINCLUDED_ANGLE_FO_16, "\nIncluded angle for arc: " ),&fr2))==RTERROR) {
							IC_RELRB(rslt);
							rc=-2;goto exit;
						} else if(ret==RTCAN) {
							IC_RELRB(rslt);
							rc=-1;goto exit;
						} else if(ret==RTNORM){
							// Bugzilla No. 78019; 03/06/2002 [
							if(icadRealEqual(fr2,IC_TWOPI))
							{
								cmd_ErrorPrompt(CMD_ERR_INVALID,0);
								rc=-1;
								goto exit;
							}
							// Bugzilla No. 78019; 03/06/2002 ]
							pt2[0]=fr2;
							for(;;){
								IC_RELRB(rslt);
						        sds_initget(SDS_RSG_NOZERO,NULL);
								ret=SDS_dragobject(24,rb.resval.rint,stpt,pt2,fr1,ResourceString(IDC_ARC__NDIRECTION_OF_CHOR_17, "\nDirection of chord: " ),&rslt,pt3,NULL);
								if(ret==RTNORM && !icadRealEqual(pt3[2],stpt[2]) && !icadRealEqual(pt3[2],curelev)) {
									cmd_ErrorPrompt(CMD_ERR_2DPTKWORD,0);
									continue;
								}else break;
							}
						}
					}
					if(ret==RTERROR){
						IC_RELRB(rslt);
                        rc=-2;goto exit;
                    } else if(ret==RTCAN) {
						IC_RELRB(rslt);
                        rc=-1;goto exit;
                    } else if(ret==RTNORM) {
						if(!icadRealEqual(pt3[2],pt1[2]) && !icadRealEqual(pt3[2],curelev)){
							cmd_ErrorPrompt(CMD_ERR_2DPTKWORD,0);
							continue;
						}
						goto create;
					}
                } else if(strisame(fs1,"ANGLE"/*DNT*/)) {
					for(;;){
						sds_initget(SDS_RSG_NOZERO,NULL);
						if((ret=sds_getorient(NULL,ResourceString(IDC_ARC__NINCLUDED_ANGLE_FO_16, "\nIncluded angle for arc: " ),&fr1))==RTERROR) {
                            rc=-2;goto exit;
                        } else if(ret==RTCAN) {
                            rc=-1;goto exit;
                        } else if(ret==RTNORM) {
							// Bugzilla No. 78019; 03/06/2002 [
							if(icadRealEqual(fr1,IC_TWOPI))
							{
								cmd_ErrorPrompt(CMD_ERR_INVALID,0);
								rc=-1;
								goto exit;
							}
							// Bugzilla No. 78019; 03/06/2002 ]
                            break;
						}
					}
					SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
					sds_initget(0,ResourceString(IDC_ARC_INITGET_CENTER_CE_2, "Center|Ce ~_Ce" ));
					ret=SDS_dragobject(25,rb.resval.rint,stpt,pt1,fr1,ResourceString(IDC_ARC__NCENTER__ENDPOINT__19, "\nCenter/<Endpoint of arc>: " ),&rslt,pt3,fs1);
					if(ret==RTKWORD || ret==RTSTR){
						ret=SDS_dragobject(26,rb.resval.rint,stpt,pt1,fr1,ResourceString(IDC_ARC__NCENTER_OF_ARC___7, "\nCenter of arc: " ),&rslt,pt3,NULL);
					}
					if(ret==RTERROR){
                        rc=-2;goto exit;
                    } else if(ret==RTCAN) {
                        rc=-1;goto exit;
                    } else if(ret==RTNORM) {
						if(!icadRealEqual(pt3[2],stpt[2]) && !icadRealEqual(pt3[2],curelev)) {
							cmd_ErrorPrompt(CMD_ERR_2DPTKWORD,0);
							continue;
						}
						goto create;
					}
                } else if(strisame(fs1,"DIRECTION"/*DNT*/)) {
					sds_initget(NULL,ResourceString(IDC_ARC_FOLLOW_4, "Follow ~_Follow" ));
					for(;;){
                       if(sds_initget(0,ResourceString(IDC_ARC_FOLLOW_4, "Follow ~_Follow" ))!=RTNORM)
                           return(-2);
						if((ret=sds_getangle(stpt,ResourceString(IDC_ARC__NFOLLOW__INITIAL_D_21, "\nFollow/<Initial direction for arc>: " ),&fr1))==RTERROR) {
                            rc=-2;goto exit;
                        } else if(ret==RTCAN) {
                            rc=-1;goto exit;
                        } else if(ret==RTNORM) {
                            break;
						} else if(ret==RTNONE || ret==RTKWORD) {
							SDS_getvar(NULL,DB_QLASTCMDANG,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
                            fr1=rblast.resval.rreal;
							break;
						}
					}
					SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
					ret=SDS_dragobject(15,rb.resval.rint,stpt,pt1,fr1,ResourceString(IDC_ARC__NEND_POINT___22, "\nEnd point: " ),&rslt,pt3,NULL);
					if(ret==RTERROR){
                        rc=-2;goto exit;
                    } else if(ret==RTCAN) {
                        rc=-1;goto exit;
                    } else if(ret==RTNORM) {
						if(!icadRealEqual(pt3[2],stpt[2]) && !icadRealEqual(pt3[2],curelev)) {
							cmd_ErrorPrompt(CMD_ERR_2DPTKWORD,0);
							continue;
						}
						goto create;
					}
                } else {
                    cmd_ErrorPrompt(CMD_ERR_2DPTKWORD,0);
                    continue;
                }
                break;
            } else {
                cmd_ErrorPrompt(CMD_ERR_2DPTKWORD,0);
                continue;
            }
        }
    }

    //*** Third point.
    if(centerflg) {
        //*** If pt1 or pt2 is a center point.
        for(;;) {
            //*** Pass RSG_OTHER to sds_initget in case user enters A or L.
            if(sds_initget(0,ResourceString(IDC_ARC_INITGET_ANGLE_LENGT_23, "Angle Length_of_chord|Length ~_Angle ~_Length" ))!=RTNORM)
               return(-2);
            SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
            if(centerflg==1) {
                ret=SDS_dragobject(7,rb.resval.rint,pt1,pt2,0,ResourceString(IDC_ARC__NANGLE_LENGTH_OF_C_24, "\nAngle/Length of chord/<End point>: " ),&rslt,pt3,fs1);
            } else {
                ret=SDS_dragobject(9,rb.resval.rint,pt1,pt2,0,ResourceString(IDC_ARC__NANGLE_LENGTH_OF_C_24, "\nAngle/Length of chord/<End point>: " ),&rslt,pt3,fs1);
            }
            if(ret==RTERROR) {
                rc=-2;goto exit;
            } else if(ret==RTCAN) {
                rc=-1;goto exit;
            } else if(ret==RTNORM) {
                if(!icadRealEqual(pt3[2],pt1[2]) && !icadRealEqual(pt3[2],curelev)) {
                    cmd_ErrorPrompt(CMD_ERR_2DPTKWORD,0);
                    continue;
                }
                break;
            } else if(ret==RTKWORD) {
				sds_relrb(rslt); rslt=NULL;
                if(strisame(fs1,"ANGLE"/*DNT*/)) {
                    for(;;) {
                        if(sds_initget(SDS_RSG_NOZERO,NULL)!=RTNORM)
                           return(-2);
                        SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
                        if(centerflg==1) {
                            ret=SDS_dragobject(8,rb.resval.rint,pt2,pt1,0,ResourceString(IDC_ARC__NINCLUDED_ANGLE___25, "\nIncluded angle: " ),&rslt,pt3,NULL);
                        } else {
                            ret=SDS_dragobject(8,rb.resval.rint,pt1,pt2,0,ResourceString(IDC_ARC__NINCLUDED_ANGLE___25, "\nIncluded angle: " ),&rslt,pt3,NULL);
                        }
                        if(ret==RTERROR) {
                            rc=-2;goto exit;
                        } else if(ret==RTCAN) {
                            rc=-1;goto exit;
                        } else if(ret==RTNORM) {
                            break;
                        } else {
                            cmd_ErrorPrompt(CMD_ERR_2DPT,0);
                            continue;
                        }
                    }
                } else if(strisame(fs1,"LENGTH"/*DNT*/)) {
                    for(;;) {
                        SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
                        if(sds_initget(SDS_RSG_NOZERO,NULL)!=RTNORM)
                           return(-2);
                        if(centerflg==1) {
                            ret=SDS_dragobject(6,rb.resval.rint,pt1,pt2,0,ResourceString(IDC_ARC__NLENGTH_OF_CHORD___27, "\nLength of chord: " ),&rslt,pt3,NULL);
                        } else {
                            ret=SDS_dragobject(10,rb.resval.rint,pt1,pt2,0,ResourceString(IDC_ARC__NLENGTH_OF_CHORD___27, "\nLength of chord: " ),&rslt,pt3,NULL);
                        }
                        if(ret==RTERROR) {			// We are currently allowing entry / selection of a chord that
                            rc=-2;goto exit;		// is longer than what is possible, mathematically.  We default
                        } else if(ret==RTCAN) {		// to a max-chord-length arc (180 degrees).  Makes it easy to
                           rc=-2;goto exit;			// use graphically (just try it in AutoCAD - ick!).
                        } else if(ret==RTNORM) {	// AutoCAD dumps you out of the command if you specify a chord
                            break;					// that is too long, with a helpful and polite "*Invalid*".
                        } else {					// Is there a reason we should match this behavior (but more nicely)?
                            cmd_ErrorPrompt(CMD_ERR_2DPT,0); 
                        }
                    }
                } else {
                    cmd_ErrorPrompt(CMD_ERR_2DPTKWORD,0);
                    continue;
                }
            } else {
                cmd_ErrorPrompt(CMD_ERR_2DPTKWORD,0);
                continue;
            }
            break;
        }
    } else if(endflg) {
        //*** If pt2 is an ending point.
        for(;;) {
            //*** Pass RSG_OTHER to sds_initget in case user enters A, D or R.
            if(sds_initget(SDS_RSG_OTHER,ResourceString(IDC_ARC_INITGET_ANGLE_DIREC_28, "Angle Direction Radius ~_Angle ~_Direction ~_Radius" ))!=RTNORM)
               return(-2);
            SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
            if((ret=SDS_dragobject(14,rb.resval.rint,pt1,pt2,0,ResourceString(IDC_ARC__NANGLE_DIRECTION_R_29, "\nAngle/Direction/Radius/<Center point>: " ),&rslt,pt3,fs1))==RTERROR) {
                rc=-2;goto exit;
            } else if(ret==RTCAN) {
                rc=-1;goto exit;
            } else if(ret==RTNORM) {
                if(!icadRealEqual(pt3[2],pt1[2]) && !icadRealEqual(pt3[2],curelev)) {
					cmd_ErrorPrompt(CMD_ERR_2DPTKWORD,0);
                    continue;
                }
                break;
            } else if(ret==RTKWORD || ret==RTSTR) {
				sds_relrb(rslt); rslt=NULL;
                if(strisame(fs1,"ANGLE"/*DNT*/)) {
                    for(;;) {
                        if(sds_initget(SDS_RSG_NOZERO,NULL)!=RTNORM)
                           return(-2);
                        SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
                        if((ret=SDS_dragobject(11,rb.resval.rint,pt1,pt2,0,ResourceString(IDC_ARC__NINCLUDED_ANGLE___25, "\nIncluded angle: " ),&rslt,pt3,NULL))==RTERROR) {
                            rc=-2;goto exit;
                        } else if(ret==RTCAN) {
                            rc=-1;goto exit;
                        } else if(ret==RTNORM) {
                            ic_ptcpy(pt3,pt2);
                            break;
                        } else {
                            cmd_ErrorPrompt(CMD_ERR_2DPT,0);
                            continue;
                        }
                    }
                } else if(strisame(fs1,"DIRECTION"/*DNT*/)) {
                    for(;;) {
                        if(sds_initget(0,NULL)!=RTNORM)
                           return(-2);

						//since this is Direction from the start point, reset the QLASTPOINT sys var to the start point
						SDS_getvar(NULL,DB_QLASTPOINT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
						ic_ptcpy(rb.resval.rpoint,pt1);
						SDS_setvar(NULL,DB_QLASTPOINT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);

                        SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
                        if((ret=SDS_dragobject(12,rb.resval.rint,pt1,pt2,0,ResourceString(IDC_ARC__NDIRECTION_FROM_ST_30, "\nDirection from start: " ),&rslt,pt3,NULL))==RTERROR) {
                            rc=-2;goto exit;
                        } else if(ret==RTCAN) {
                            rc=-1;goto exit;
                        } else if(ret==RTNORM) {
                            ic_ptcpy(pt3,pt2);
                            break;
                        } else {
                            cmd_ErrorPrompt(CMD_ERR_2DPT,0);
                            continue;
                        }
                    }
                } else if(strisame(fs1,"RADIUS"/*DNT*/)) {
                    for(;;) {
                        if(sds_initget(0,NULL)!=RTNORM)
                           return(-2);
                        SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
                        if((ret=SDS_dragobject(13,rb.resval.rint,pt1,pt2,0,ResourceString(IDC_ARC__NRADIUS___31, "\nRadius: " ),&rslt,pt3,NULL))==RTERROR) {
                            rc=-2;goto exit;
                        } else if(ret==RTCAN) {
                            rc=-1;goto exit;
                        } else if(ret==RTNORM) {
                            ic_ptcpy(pt3,pt2);
                            break;
                        } else {
                            cmd_ErrorPrompt(CMD_ERR_2DPT,0);
                            continue;
                        }
                    }
                } else {
                    cmd_ErrorPrompt(CMD_ERR_2DPTKWORD,0);
                    continue;
                }
                break;
            } else {
                cmd_ErrorPrompt(CMD_ERR_2DPTKWORD,0);
                continue;
            }
        }
    } else {
        for(;;) {
            if(sds_initget(0,NULL)!=RTNORM)
               return(-2);
            SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
            if((ret=SDS_dragobject(0,rb.resval.rint,pt1,pt2,0,ResourceString(IDC_ARC__NEND_POINT___22, "\nEnd point: " ),&rslt,pt3,NULL))==RTERROR) {
                rc=-2;goto exit;
            } else if(ret==RTCAN) {
                rc=-1;goto exit;
            } else if(ret==RTNORM) {
                if(!icadRealEqual(pt3[2],pt1[2]) && !icadRealEqual(pt3[2],curelev)) {
                    cmd_ErrorPrompt(CMD_ERR_2DPT,0);
                    continue;
                }
                if(rslt==NULL){
					//cmd_ErrorPrompt(CMD_ERR_CREATEENT,0);
					cmd_ErrorPrompt(CMD_ERR_ONSAMELINE,0);
					rc=-3;
					goto exit;
				}
                break;
            } else {
                cmd_ErrorPrompt(CMD_ERR_2DPT,0);
                continue;
            }
        }
    }

    //*** Create entity.
create:
    newarc=sds_buildlist(RTDXF0,"ARC"/*DNT*/,0); newarc->rbnext=rslt;
	// Bugzilla No. 78019; 03/06/2002 [
	sds_real ang1,ang2;
	ang1=ang2=0.0;
	if(ic_assoc(newarc,50)==0)
		ang1=ic_rbassoc->resval.rreal;
	if(ic_assoc(newarc,51)==0)
		ang2=ic_rbassoc->resval.rreal;
	if(icadRealEqual(ang1,ang2) || ((icadAngleEqual(ang1,0.0)) && (icadAngleEqual(ang2,IC_TWOPI))))
	{
		cmd_ErrorPrompt(CMD_ERR_INVALID,0);
		rc=-1;
		IC_RELRB(newarc);
		goto exit;
	}
	// Bugzilla No. 78019; 03/06/2002 ]
    if(sds_entmake(newarc)!=RTNORM) {
        cmd_ErrorPrompt(CMD_ERR_CREATEENT,0);
        IC_RELRB(newarc); return(-2);
    }
	for(rslt=newarc;rslt!=NULL;rslt=rslt->rbnext){
		if(rslt->restype==10)ic_ptcpy(pt1,rslt->resval.rpoint);
		else if(rslt->restype==50)fr1=rslt->resval.rreal;
		else if(rslt->restype==51)fr2=rslt->resval.rreal;
		else if(rslt->restype==40)fr3=rslt->resval.rreal;
	}
    sds_polar(pt1,fr2,fr3,pt2);
    sds_polar(pt1,fr1,fr3,pt3);
	sds_trans(stpt,&rbucs,&rbecs,0,stpt);
    if(gotstpt && sds_distance(pt2,stpt)<sds_distance(pt3,stpt)) {
        //use ending point instead of starting point
        ic_ptcpy(pt2,pt3);
        pt3[0]=cos(fr1-(IC_PI/2.0));
		pt3[1]=sin(fr1-(IC_PI/2.0));
		pt3[2]=0.0;
	}else{
		pt3[0]=cos(fr2+(IC_PI/2.0));
		pt3[1]=sin(fr2+(IC_PI/2.0));
		pt3[2]=0.0;
	}
    //*** Set LASTANGLE variable.
	sds_trans(pt3,&rbecs,&rbucs,1,pt3);
    rblast.restype=RTREAL;
	rblast.resval.rreal=atan2(pt3[1],pt3[0]);
	SDS_setvar(NULL,DB_QLASTCMDANG,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
	SDS_setvar(NULL,DB_QLASTANGLE,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);

    //*** Set LASTCMDPT,ISLASTCMDPT variable.
    rblast.restype=RT3DPOINT;
	sds_trans(pt2,&rbecs,&rbucs,0,rblast.resval.rpoint);
	SDS_setvar(NULL,DB_QLASTCMDPT,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
	rblast.restype=RTSHORT;
    rblast.resval.rint=1;
	SDS_setvar(NULL,DB_QISLASTCMDPT,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
    IC_RELRB(newarc);
exit:
	if(SDS_ElevationChanged){
		rblast.restype=RTREAL;
		rblast.resval.rreal=curelev;
		sds_setvar("ELEVATION"/*DNT*/,&rblast);
		SDS_ElevationChanged=FALSE;
	}
    return(rc);
}


