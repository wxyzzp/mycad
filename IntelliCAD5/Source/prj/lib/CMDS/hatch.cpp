/* F:\DEV\PRJ\LIB\CMDS\HATCH.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * HATCH command
 * 
 */ 
// THIS FILE HAS BEEN GLOBALIZED!
#include "cmds.h"/*DNT*/
#include "Icad.h"/*DNT*/
#include "db.h"/*DNT*/
#include "paths.h"
#include "IcadApi.h"
#include "cmdsHatch.h"

/*D.Gavrilov*/// The following two includes was added for getting current
// linetype in cmd_loadUserDefPattern.
#include "IcadView.h"
#include "ltypetabrec.h"

/*D.Gavrilov*/// for using in cmd_hatch_objects
#include "Geometry.h"/*DNT*/
#include "Extents.h"/*DNT*/
#include "Region.h"/*DNT*/
#include "Boolean.h"/*DNT*/
#include "BhatchDialog.h"/*DNT*/	// just for the USER contant
#include "BHatchBuilder.h"
#include "GeoConvert.h"/*DNT*/

// Globals
bool cmd_HatchedOK;

extern bool SDS_LastCmdWasHatch;
extern int svar_ChildDialog;

#define MAX_HATCH_LINES 10000

// Proto
long  cmd_hatch_objects(struct cmd_hpdefll *hplcur, sds_name sset, int mode, int hpstyle,
					sds_real baseang, sds_real hpscale,char *hpname, int hplines);
void  cmd_hatch_hatch2ucs(sds_point hatchpt,sds_point ucspt,sds_point hatchorg,sds_real angsin,sds_real angcos);
void  cmd_hatch_ucs2hatch(sds_point ucspt,sds_point hatchpt,sds_point hatchorg,sds_real angsin,sds_real angcos);
short cmd_hatch_onoff(long thisint, long numints, int hpstyle);
int   cmd_hatch_qsortcomp(const void *vp1, const void *vp2);
void  cmd_hatch_2dptminmax(sds_point ll,sds_point ur,sds_point pt);

short cmd_hatch(void) {

	cmd_HatchedOK=FALSE;

    if (cmd_iswindowopen() == RTERROR) return RTERROR;
	
	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    char fs1[IC_ACADBUF],fs2[IC_ACADBUF],hpname[IC_ACADNMLEN],orighpname[IC_ACADNMLEN],*fcp1,fc1[IC_ACADNMLEN];
	bool chgedsvars=FALSE;
    struct resbuf setgetrb,*rbptemp;
    sds_name ss1={0,0};
	sds_name ename={0,0};
    sds_real hpscale,hpang,hpspace,origscale,origang,origspace,fr1;
    int ret,patlen=1;
    short stdpattern=0,hpdouble=0,origdouble,direct=0,origretpline,fi1,
		hpstyle,drawmode=1,donemode=0;
	//Note: drawmode=2 for preview, 1 for block, 0 for indiv entities
    long rc=0L,ssct;
	static short cmd_hatch_retpline=0;
	struct cmd_hpdefll *hplcur,*hpltmp;
	db_handitem *hip;

	rbptemp=NULL;
	hplcur=hpltmp=NULL;
    setgetrb.resval.rstring=NULL;
    setgetrb.rbnext=NULL;

	//EBATECH(CNBR)-[ 2003/10/05 Default linetype file is dependent on $MEASUREMENT.
	char chHPFileName[IC_ACADBUF];
	if(SDS_getvar(NULL,DB_QMEASUREMENT,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-__LINE__); goto bail; }
	strcpy( chHPFileName, ( setgetrb.resval.rint == DB_ENGLISH_MEASUREINIT ?
		SDS_PATTERNFILE : SDS_ISOPATTERNFILE));
	//EBATECH(CNBR)]-

    //*** Get the default hatch pattern name
    if(SDS_getvar(NULL,DB_QHPNAME,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-__LINE__); goto bail; }
	if(strisame(setgetrb.resval.rstring, USER))
		strcpy(hpname, "&");
	else
		strncpy(hpname,setgetrb.resval.rstring,sizeof(hpname)-1);
    strncpy(orighpname,setgetrb.resval.rstring,sizeof(hpname)-1);
    IC_FREE(setgetrb.resval.rstring);
	if(SDS_getvar(NULL,DB_QHPSTYLE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
		hpstyle=0;	//normal hatching style
	else
		hpstyle=setgetrb.resval.rint;
    if(SDS_getvar(NULL,DB_QHPSCALE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-__LINE__); goto bail; }
    hpscale=origscale=setgetrb.resval.rreal;
    if(SDS_getvar(NULL,DB_QHPANG,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-__LINE__); goto bail; }
    hpang=origang=setgetrb.resval.rreal;
    if(SDS_getvar(NULL,DB_QHPSPACE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-__LINE__); goto bail; }
    hpspace=origspace=setgetrb.resval.rreal;
    if(SDS_getvar(NULL,DB_QHPDOUBLE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-__LINE__); goto bail; }
    hpdouble=origdouble=setgetrb.resval.rint;
	//if(SDS_getvar(NULL,DB_QHPSTYLE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-__LINE__); goto bail; }
	//hpstyle=setgetrb.resval.rint;
	
	if(!*hpname){
		stdpattern=1;
		strncpy(hpname,"&"/*DNT*/,sizeof(hpname)-1);
	}

	if(SDS_LastCmdWasHatch) {
	    if(!stdpattern) {
			//*************LOAD USER-SET PATTERN****************
			if((patlen=cmd_hatch_loadpat(chHPFileName/*SDS_PATTERNFILE*/, hpname,&hplcur))<0){
			    rc=(-1L); goto bail;
			}
		} else hpscale=1.0;
		goto Askforents;
	} else {
		stdpattern=0;
		hpdouble=0;
	}

    for(;;) {
        if(*hpname) {
			if(strnisame(hpname,"&"/*DNT*/,1)){	
				if(drawmode!=2){	// Cheap fix, I know, but it looks a lot better to the user. 
					sds_initget(RSG_OTHER,ResourceString(IDC_HATCH_INITGET_LIST_PATTE_0, "List_patterns|? SEttings ~ Hatch_with_lines|& Preview_pattern|Preview Specify_style|STyle ~ LINES|User ~Solid ~LINE ~_? ~_SEttings ~_ ~_& ~_Preview ~_STyle ~_ ~_User ~_Solid ~_LINE" ));
					strncpy(fs2,ResourceString(IDC_HATCH__NHATCH_____TO_LIS_1, "\nHatch:  ? to list patterns/SEttings/& for lines/Preview/STyle/<Pattern name> <LINES>: " ),sizeof(fs2)-1);
				}else{
					sds_initget(RSG_OTHER,ResourceString(IDC_HATCH_INITGET_LIST_PATTE_2, "List_patterns|? SEttings ~ Hatch_with_lines|& ~ LINES|User ~Solid ~_? ~_SEttings ~_ ~_& ~_ ~_User ~_Solid" ));
					strncpy(fs2,ResourceString(IDC_HATCH__NPATTERN_TO_PREVI_3, "\nPattern to preview:  ? to list/SEttings/& for lines/<LINES> : " ),sizeof(fs2)-1);
				}
			}else{
				if(drawmode!=2){

                   sprintf(fs2, ResourceString(IDC_HATCH_INITGET_LIST_PATTE_55,"List_patterns|? SEttings ~ Hatch_with_lines|& Preview_pattern|Preview Specify_style|STyle ~ %s ~User ~Solid ~LINE ~_? ~_SEttings ~_ ~_& ~_Preview ~_STyle ~_ ~%s ~_User ~_Solid ~_LINE"), hpname, hpname);  // 1-57915
					sds_initget(RSG_OTHER,fs2);
					//sprintf(fs2,"\nPattern (? or name/U,style) <%s>: ",hpname);
					sprintf(fs2,ResourceString(IDC_HATCH__NHATCH_____TO_LIS_4, "\nHatch:  ? to list patterns/SEttings/& for lines/Preview/STyle/<Pattern name> <%s>: " ),hpname);
				}else{

                   sprintf(fs2, ResourceString(IDC_HATCH_INITGET_LIST_PATTE_56, "List_patterns|? SEttings ~ Hatch_with_lines|& ~ %s ~User ~Solid ~LINE ~_? ~_SEttings ~_ ~_& ~_ ~%s ~_User ~_Solid ~_LINE"), hpname, hpname);    // 1-57915
					sds_initget(RSG_OTHER,fs2);
					sprintf(fs2,ResourceString(IDC_HATCH__NPATTERN_TO_PREVI_5, "\nPattern to preview:  ? to list/SEttings/& for lines/<%s>: " ),hpname);
				}
			}
        } else {
			if(drawmode!=2){
				sds_initget(RSG_OTHER,ResourceString(IDC_HATCH_INITGET_LIST_PATTE_6, "List_patterns|? SEttings ~ Hatch_with_lines|& Preview_pattern|Preview Specify_style|STyle ~User ~Solid ~LINE ~_? ~_SEttings ~_ ~_& ~_Preview ~_STyle ~_User ~_Solid ~_LINE" ));
				strncpy(fs2,ResourceString(IDC_HATCH__NHATCH_____TO_LIS_7, "\nHatch:  ? to list patterns/SEttings/& for lines/Preview/STyle/<Pattern name>: " ),sizeof(fs2)-1);
			}else{
				sds_initget(RSG_OTHER,ResourceString(IDC_HATCH_INITGET_LIST_PATTE_8, "List_patterns|? SEttings ~ Hatch_with_lines|& ~User ~Solid ~LINE ~_? ~_SEttings ~_ ~_& ~_User ~_Solid ~_LINE" ));
				strncpy(fs2,ResourceString(IDC_HATCH__NPATTERN_TO_PREVI_9, "\nPattern to preview:  ? to list/SEttings/& for lines/<Pattern name>: " ),sizeof(fs2)-1);
			}
            //strncpy(fs2,"\nPattern (? or name/U,style): ",sizeof(fs2)-1);
        }
		if((ret=sds_getkword(fs2,fs1))==RTERROR) {
            rc=(-__LINE__); goto bail;
        } else if(ret==RTCAN) {
            rc=(-1L); goto bail;
        }else if(ret==RTNORM){
			strupr(fs1);
		}

        if(!*fs1) {										// If user hits ENTER to accept the default...
            if(!*hpname) { rc=(-1L); goto bail; }
			if(strisame(hpname,"&"/*DNT*/) || strisame(hpname,"_&"/*DNT*/)){	// If "&" (for lines) was the default, do same as if they entered "&".
				stdpattern=1;
				hpscale=1.0;
//				*hpname=0;
			}else{
				strncpy(fs1,hpname,sizeof(fs1)-1);			// Otherwise use the existing hpname as if the user entered it.
			}
		}else if(strisame(fs1,"SETTINGS"/*DNT*/)) {
			svar_ChildDialog=EC_HATCH;
			ExecuteUIAction( ICAD_WNDACTION_SETVAR );

			//after return from dialog, we have to re-gather all the vars....
			if(SDS_getvar(NULL,DB_QHPNAME,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-__LINE__); goto bail; }
			strncpy(hpname,setgetrb.resval.rstring,sizeof(hpname)-1);
			strncpy(orighpname,setgetrb.resval.rstring,sizeof(hpname)-1);
			IC_FREE(setgetrb.resval.rstring);
			if(SDS_getvar(NULL,DB_QHPSTYLE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
				hpstyle=0;	//normal hatching style
			else
				hpstyle=setgetrb.resval.rint;
			if(SDS_getvar(NULL,DB_QHPSCALE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-__LINE__); goto bail; }
			hpscale=origscale=setgetrb.resval.rreal;
			if(SDS_getvar(NULL,DB_QHPANG,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-__LINE__); goto bail; }
			hpang=origang=setgetrb.resval.rreal;
			if(SDS_getvar(NULL,DB_QHPSPACE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-__LINE__); goto bail; }
			hpspace=origspace=setgetrb.resval.rreal;
			if(SDS_getvar(NULL,DB_QHPDOUBLE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-__LINE__); goto bail; }
			hpdouble=origdouble=setgetrb.resval.rint;
			if(SDS_getvar(NULL,DB_QHPSTYLE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-__LINE__); goto bail; }
			hpstyle=setgetrb.resval.rint;
            
			if(!*hpname){
				stdpattern=1;
				strncpy(hpname,"&"/*DNT*/,sizeof(hpname)-1);
			}else stdpattern=0;

			continue;
#ifdef LOCALISE_ISSUE
// Seems that the _? and _& are checking to see if there is anything following them.
// Count not taking into consideration the '_' character.
// Ammended. GH.
#endif
        } else if((strisame(fs1,"?"/*DNT*/) || strisame(fs1,"_?"/*DNT*/))) {
            rc=cmd_hatch_list(); goto bail;
        } else if(((!strncmp(fs1,"&"/*DNT*/,1)) || (!strncmp(fs1,"U"/*DNT*/,1))) && (fs1[1]==0 || fs1[1]==',')) {		// 1-57936, 1-57941
            stdpattern=1;
			hpscale=1.0;
        } else if(((!strncmp(fs1,"USER"/*DNT*/,4)) || (!strncmp(fs1,"LINE"/*DNT*/,4))) && (fs1[4]==0 || fs1[4]==',')) {	// 1-57936, 1-57941
            stdpattern=1;
			hpscale=1.0;
        } else if(((!strncmp(fs1,"LINES"/*DNT*/,5)) || (!strncmp(fs1, USER, 5))) && (fs1[5]==0 || fs1[5]==',')) {	// 1-57927, 1-57941
            stdpattern=1;
			hpscale=1.0;
		}else if(strisame(fs1,"PREVIEW"/*DNT*/)) {
			drawmode=2;
			continue;
		}else if(strisame(fs1,"STYLE"/*DNT*/)){
			sds_initget(NULL,ResourceString(IDC_HATCH_INITGET__NORMAL_A_17, "~Normal All Outer Standard ~_Normal ~_All ~_Outer ~_Standard" ));
			if(hpstyle==0)
				ret=sds_getkword(ResourceString(IDC_HATCH__NHATCH_STYLE_TO__18, "\nHatch style to use:  All/Outer/<Standard>: " ),fs1);
			else if(hpstyle==1)
				ret=sds_getkword(ResourceString(IDC_HATCH__NHATCH_STYLE_TO__19, "\nHatch style to use:  All/Standard/<Outer>: " ),fs1);
			else
				ret=sds_getkword(ResourceString(IDC_HATCH__NHATCH_STYLE_TO__20, "\nHatch style to use:  Standard/Outer/<All>: " ),fs1);
			if(ret==RTERROR){
				rc=(-__LINE__); goto bail;
			} else if(ret==RTCAN) {
				rc=(-1L); goto bail;
			}else if(ret==RTNONE){
				continue;
			}else if(ret==RTNORM){
				setgetrb.restype=RTSHORT;
				if(strisame(fs1,"NORMAL"/*DNT*/) || strisame(fs1,"STANDARD"/*DNT*/))
					setgetrb.resval.rint=0;
				else if(strisame(fs1,"OUTER"/*DNT*/))
					setgetrb.resval.rint=1;
				else
					setgetrb.resval.rint=2;
				sds_setvar("HPSTYLE"/*DNT*/,&setgetrb);
				hpstyle=setgetrb.resval.rint;
				continue;
			}
        } else {
			if(fs1[0]=='*'/*DNT*/){
				if(drawmode!=2)
					drawmode=0;	//draw hatch pattern as individual lines
				else
					cmd_ErrorPrompt(CMD_ERR_HATCHEXPPREV,0);
				fcp1=fs1+1;
				strncpy(hpname,fcp1,sizeof(hpname)-1);
				if( (strisame(fs1, "&"/*DNT*/) && (fs1[1]==0 || fs1[1]==','/*DNT*/)) ||
					(strisame(fs1, "USER"/*DNT*/) && (fs1[4]==0 || fs1[4]==','/*DNT*/)) ||
					(strisame(fs1, USER) && (fs1[5]==0 || fs1[5]==','/*DNT*/)) ) {
					stdpattern=1;
					hpscale=1.0;
					*hpname=0;
				}
			}else{
				strncpy(hpname,fs1,sizeof(hpname)-1);
				if((fcp1=strchr(hpname,','/*DNT*/))!=NULL)*fcp1=0;
				if(!strisame(hpname, "SOLID"/*DNT*/))		// Don't verify for SOLID: it always exists.
					if(cmd_hatch_valid(hpname, chHPFileName) == RTERROR)
					{ *hpname = 0; continue; }
            }
        }
		if((fcp1=strchr(fs1,','/*DNT*/))!=NULL){
            *fcp1=0; fcp1++;
			toupper(*fcp1);
			// If the first letter doesn't match, complain.
			int len = strlen(fcp1);
            if((!strnisame(fcp1,"NORMAL"/*DNT*/, len) && 
			      !strnisame(fcp1,"STANDARD"/*DNT*/,len)&& 
                     !strnisame(fcp1,"OUTER"/*DNT*/,len) && 
				  !strnisame(fcp1,"IGNORE"/*DNT*/,len)) 
				||
				 (!strnisame(fcp1,ResourceString(IDC_HATCH_NORMAL_24, "NORMAL"), len) &&	
			      !strnisame(fcp1,ResourceString(IDC_HATCH_STANDARD_25, "STANDARD"),len)&&	
                     !strnisame(fcp1,ResourceString(IDC_HATCH_OUTER_26, "OUTER"),len) &&		
				  !strnisame(fcp1,ResourceString(IDC_HATCH_IGNORE_27, "IGNORE"),len)) 							  
				  ) 
			{
                sds_printf(ResourceString(IDC_HATCH__NPLEASE_USE_ONE__28, "\nPlease use one of the following hatch styles:\n    S = Standard - Fills alternating areas\n    O = Outer    - Fills outermost areas only\n    I = Internal - Fills all areas within outermost boundary" ));
                cmd_ErrorPrompt(CMD_ERR_TRYAGAIN,0);
                rc=(-1L); goto bail;
            }else if(strnisame(fcp1,"STANDARD"/*DNT*/,len) || strnisame(fcp1,ResourceString(IDC_HATCH_STANDARD_25, "STANDARD"),len))	
                hpstyle=0;
			else if (strnisame(fcp1,"NORMAL"/*DNT*/,len)   || strnisame(fcp1,ResourceString(IDC_HATCH_NORMAL_24, "NORMAL"),len))		
                hpstyle=0;
			else if (strnisame(fcp1,"OUTER"/*DNT*/,len)    || strnisame(fcp1,ResourceString(IDC_HATCH_OUTER_26, "OUTER"),len))		
                hpstyle=1;
			else if (strnisame(fcp1,"IGNORE"/*DNT*/,len)   || strnisame(fcp1,ResourceString(IDC_HATCH_IGNORE_27, "IGNORE"),len))		
                hpstyle=2;
		}
        break;
    }

	if(strisame(fs1, "SOLID"/*DNT*/))
		goto Askforents;
    
    if(!stdpattern) {
		//*************LOAD USER-SET PATTERN****************
		if((patlen=cmd_hatch_loadpat(chHPFileName/*SDS_PATTERNFILE*/, hpname,&hplcur))<0){
            rc=(-1L); goto bail;
        }
        //*** Get the hatch pattern scale
        hpscale=origscale;
        if(sds_rtos(hpscale,-1,-1,fs1)!=RTNORM) { rc=(-__LINE__); goto bail; }
        sprintf(fs2,ResourceString(IDC_HATCH__NPROCEED_SCALE_F_29, "\nProceed/Scale for pattern <%s>: " ),fs1);

        if(sds_initget(RSG_NONEG+RSG_NOZERO,ResourceString(IDC_HATCH_INITGET_PROCEED_30, "Proceed ~_Proceed" ))!=RTNORM) { 
            rc=(-__LINE__); 
            goto bail; 
        }
        if((ret=sds_getdist(NULL,fs2,&fr1))==RTERROR) {
            rc=(-__LINE__); goto bail;
        } else if(ret==RTCAN) {
            rc=(-1L); goto bail;
        } else if(ret==RTKWORD){
			donemode=1;
		} else if(ret==RTNORM){
			hpscale=fr1;
		}
    }
    //*** Get the angle for the hatch pattern
    hpang=origang;
	if(0==donemode){
		if(sds_angtos(hpang,-1,-1,fs1)!=RTNORM) { rc=(-__LINE__); goto bail; }
		if(stdpattern){
			sprintf(fs2,ResourceString(IDC_HATCH__NPROCEED_ANGLE_F_31, "\nProceed/Angle for pattern <%s>: " ),fs1);
			if(sds_initget(0,ResourceString(IDC_HATCH_INITGET_PROCEED_30, "Proceed ~_Proceed" ))!=RTNORM) { 
                rc=(-__LINE__); 
                goto bail; 
            }
		}else{
			sprintf(fs2,ResourceString(IDC_HATCH__NANGLE_FOR_PATTE_32, "\nAngle for pattern <%s>: " ),fs1);
			if(sds_initget(0,NULL)!=RTNORM) { 
                rc=(-__LINE__); 
                goto bail; 
            }
		}
		if((ret=sds_getangle(NULL,fs2,&hpang))==RTERROR) {
			rc=(-__LINE__); goto bail;
		} else if(ret==RTCAN) {
			rc=(-1L); goto bail;
		}else if(ret==RTKWORD){
			donemode=1;
		}
	}
    if(stdpattern) {
		patlen=1;
		if(donemode==1){
			hpspace=origspace;
			hpdouble=origdouble;
		}else{
			//*** Spacing between lines
			hpspace=origspace;
			if(sds_rtos(hpspace,-1,-1,fs1)!=RTNORM) { rc=(-__LINE__); goto bail; }
			sprintf(fs2,ResourceString(IDC_HATCH__NSPACE_BETWEEN_S_33, "\nSpace between standard pattern lines <%s>: " ),fs1);
			sds_initget(RSG_NONEG+RSG_NOZERO,NULL);
			if((ret=sds_getdist(NULL,fs2,&fr1))==RTERROR) {
				rc=(-__LINE__); goto bail;
			} else if(ret==RTCAN) {
				rc=(-1L); goto bail;
			} else if(ret==RTNORM){
				hpspace=fr1;
			}
			//*** Double hatching
			if(origdouble!=0) 
                _tcscpy(fc1, ResourceString(IDC_HATCH_YES_34, "Yes" ));
			else 
                _tcscpy(fc1, ResourceString(IDC_HATCH_NO_35, "No" ));

			sprintf(fs2,ResourceString(IDC_HATCH__NCROSS_HATCH_ARE_36, "\nCross-hatch area? <%s> " ),fc1);
			if(sds_initget(0,ResourceString(IDC_HATCH_INITGET_YES_CROSS_37, "Yes-Cross-hatch|Yes No-One_direction|No ~_Yes ~_No" ))!=RTNORM) { 
                rc=(-__LINE__); 
                goto bail; 
            }
			if((ret=sds_getkword(fs2,fs1))==RTERROR) {
				rc=(-__LINE__); 
                goto bail;
			} else if(ret==RTCAN) {
				rc=(-1L); 
                goto bail;
			}
			if(*fs1==0){
				if(strisame(fc1, "YES"/*DNT*/))
                    hpdouble=1;
			}else 
                if(strisame(fs1, "YES"/*DNT*/)) 
                    hpdouble=1;
		}
    }
    //*** Get hatch boundaries.
	//NOTE: can't filter w/ents.  If user attempts to pick but gets nothing matching filter,
	//	f'n returns RTNONE.  We need to catch this differently from return at initial prompt
	/*
   if((rbptemp=sds_buildlist(RTDXF0,"LINE,ARC,CIRCLE,TRACE,3DFACE,SOLID,TEXT,POLYLINE",0))==NULL){
		rc=(-__LINE__); goto bail; 
	}	*/

	Askforents: ;

	ret=sds_pmtssget(ResourceString(IDC_HATCH__NENTER_FOR_DIREC_39, "\nENTER for direct hatch option/<Select hatch boundary entities>: " ),NULL,NULL,NULL,rbptemp,ss1,0);
    if(ret==RTERROR) {
        direct=1;
    } else if(ret==RTCAN) {
        rc=(-1L); goto bail;
    } 
	if(RTNORM==ret){
		cmd_HatchedOK=TRUE;
        for(ssct=0L; sds_ssname(ss1,ssct,ename)==RTNORM; ssct++) {
			hip=(db_handitem *)ename[0];
			fi1=hip->ret_type();
			switch (fi1){
				case DB_LINE:
				case DB_ARC:
				case DB_CIRCLE:
				case DB_TRACE:
				case DB_3DFACE:
				case DB_SOLID:
				case DB_TEXT:
				case DB_POLYLINE:
				case DB_LWPOLYLINE:
				case DB_XLINE:
				case DB_RAY:
				case DB_ELLIPSE:
					break;
				default:
					sds_ssdel(ename,ss1);
					ssct--;
			}
        }
		if(0L==ssct){
			ret=RTNORM;
			sds_redraw(ename,IC_REDRAW_DRAW);
			cmd_ErrorPrompt(CMD_ERR_HATCHBDRY,0);
			goto bail;
		}
	}
    if(direct) {
        //*** Define hatch boundary 
        if((origretpline=cmd_hatch_retpline)!=0) 
            _tcscpy(fc1, ResourceString(IDC_HATCH_YES_34, "Yes" ));
        else 
            _tcscpy(fc1, ResourceString(IDC_HATCH_NO_35, "No" ));
        sprintf(fs2,ResourceString(IDC_HATCH__NRETAIN_BOUNDARY_40, "\nRetain boundary as a polyline? <%s> " ),fc1);
        if(sds_initget(0,ResourceString(IDC_HATCH_INITGET_YES_RETAI_41, "Yes-Retain_boundary|Yes No-Discard_boundary|No ~_Yes ~_No" ))!=RTNORM) { rc=(-__LINE__); goto bail; }
        if((ret=sds_getkword(fs2,fs1))==RTERROR) {
            rc=(-__LINE__); goto bail;
        } else if(ret==RTCAN) {
            rc=(-1L); goto bail;
        } else if(ret==RTNORM){
			if(strisame(fs1, "YES"/*DNT*/))		// 1-57925
                cmd_hatch_retpline=1;
			else 
                cmd_hatch_retpline=0;
		}
		//*** Create boundary
        for(fi1=0; ;fi1++) {
			if(fi1==0)sds_printf(ResourceString(IDC_HATCH__NSTARTING_POINT__42, "\nStarting point: " ));
            else sds_printf(ResourceString(IDC_HATCH__NENTER_TO_APPLY__43, "\nENTER to apply hatch/<Starting point>: " ));		
            if((ret=cmd_PolylineFunc(2))==RTERROR) {
                rc=(-__LINE__); goto bail;
            } else if(ret==RTCAN) {
                rc=(-1L); goto bail;
            } else if(ret==RTNONE){
				if(fi1) break;
				else{
					fi1--;
					continue;
				}
			}
            if(sds_entlast(ename)!=RTNORM) { rc=(-__LINE__); goto bail; }
            if(SDS_ssadd(ename, ss1, ss1)!=RTNORM) { rc=(-__LINE__); goto bail; }
        }
	}
	if(stdpattern){
		strcpy(hpname,""/*DNT*/);
		if (!cmd_loadUserDefPattern(&hplcur, hpdouble, hpspace))
		{
			rc=(-__LINE__);
			goto bail;
		}
    }

	chgedsvars=TRUE;
    //*** Set the hatch pattern name
	if(strsame(hpname, ""/*DNT*/)) 
		strcpy(hpname, USER);
    setgetrb.restype=RTSTR;
    setgetrb.resval.rstring=hpname;
    if(sds_setvar("HPNAME"/*DNT*/,&setgetrb)!=RTNORM) { rc=(-__LINE__); goto bail; }
    if(!stdpattern) {
        //*** Set the hatch pattern scale
        setgetrb.restype=RTREAL;
        setgetrb.resval.rreal=hpscale;
        if(sds_setvar("HPSCALE"/*DNT*/,&setgetrb)!=RTNORM) { rc=(-__LINE__); goto bail; }
    } else {
        //*** Set the user-defined pattern spacing
        setgetrb.restype=RTREAL;
        setgetrb.resval.rreal=hpspace;
        if(sds_setvar("HPSPACE"/*DNT*/,&setgetrb)!=RTNORM) { rc=(-__LINE__); goto bail; }
        //*** Set the double-hatch flag
        setgetrb.restype=RTSHORT;
        setgetrb.resval.rint=hpdouble;
        if(sds_setvar("HPDOUBLE"/*DNT*/,&setgetrb)!=RTNORM) { rc=(-__LINE__); goto bail; }
    }
    //*** Set the angle of the hatch pattern
    setgetrb.restype=RTREAL;
    setgetrb.resval.rreal=hpang;
    if(sds_setvar("HPANG"/*DNT*/,&setgetrb)!=RTNORM) { rc=(-__LINE__); goto bail; }
    //*** Create the hatch
	if(strsame(hpname,""/*DNT*/))strcpy(hpname,"&"/*DNT*/);  // "&" is OK in cmd_hatch_objects, but makes no sense used again in cmd_hatch().
	
	rc=cmd_hatch_objects(hplcur,ss1,drawmode,hpstyle,hpang,hpscale,hpname,patlen);
	if (rc<0)
		goto bail;

	// We are giving users the option of keeping or discarding a hatch they have just previewed.
	// AutoCAD has no Preview mode, so prompting compatibility is not an issue here.
	if (2==drawmode) {								// At this point user will be seeing a preview of their hatch.

		if (sds_initget(0,ResourceString(IDC_HATCH_INITGET_YES_KEEP__44, "Yes-Keep_hatch|Yes No-Discard_hatch|No ~_Yes ~_No" ))!=RTNORM) { 
            rc=(-__LINE__); 
            goto bail; 
        }
        if ((ret=sds_getkword( ResourceString(IDC_HATCH_DO_YOU_WANT_TO_KE_45, "Do you want to keep this hatch?  Yes/<No>: " ),fs1))==RTERROR) {
            rc=(-__LINE__); goto bail;
        } else if (ret==RTCAN) {
            rc=(-1L); goto bail;
		} else if (ret==RTNONE){							// Discard is the default selection...
				sds_redraw(NULL,IC_REDRAW_DONTCARE);		//	  so redraw to make the preview go away.
        } else if (ret==RTNORM){
			if (strisame(fs1, "YES"/*DNT*/)) {		// 1-57926 // If user wants to keep the hatch they are previewing...
				drawmode=3;
				rc=cmd_hatch_objects(hplcur,ss1,drawmode,hpstyle,hpang,hpscale,hpname,patlen);
			} else {										// Otherwise ("No"), user wants to discard the hatch...
				sds_redraw(NULL,IC_REDRAW_DONTCARE);		//	  so redraw to make the preview go away.
			}
		}
	}

    bail:

    if(rc<0L && chgedsvars) {
        //*** Set the hatch pattern variables back to their original values
        setgetrb.restype=RTSTR;
        setgetrb.resval.rstring= new char [strlen(orighpname)+1];
        strcpy(setgetrb.resval.rstring,orighpname);
        if(sds_setvar("HPNAME"/*DNT*/,&setgetrb)!=RTNORM) { rc=(-__LINE__); }
        IC_FREE(setgetrb.resval.rstring);
        if(!stdpattern) {
            setgetrb.restype=RTREAL;
            setgetrb.resval.rreal=origscale;
            if(sds_setvar("HPSCALE"/*DNT*/,&setgetrb)!=RTNORM) { rc=(-__LINE__); }
        } else {
            setgetrb.restype=RTREAL;
            setgetrb.resval.rreal=origspace;
            if(sds_setvar("HPSPACE"/*DNT*/,&setgetrb)!=RTNORM) { rc=(-__LINE__); }
            setgetrb.restype=RTSHORT;
            setgetrb.resval.rint=origdouble;
            if(sds_setvar("HPDOUBLE"/*DNT*/,&setgetrb)!=RTNORM) { rc=(-__LINE__); }
        }
        setgetrb.restype=RTREAL;
        setgetrb.resval.rreal=origang;
        if(sds_setvar("HPANG"/*DNT*/,&setgetrb)!=RTNORM) { rc=(-__LINE__); }
    }
    if(direct && !cmd_hatch_retpline) {
        //*** Delete the polyline boundaries
        for(ssct=0L; sds_ssname(ss1,ssct,ename)==RTNORM; ssct++) {
            if(sds_entdel(ename)!=RTNORM) { rc=(-__LINE__); }
        }
    }
	cmd_hatch_freehpdefll(&hplcur);
	if(!direct || cmd_hatch_retpline)
		for(ssct=0L; sds_ssname(ss1,ssct,ename)==RTNORM; ssct++, sds_redraw(ename,IC_REDRAW_DRAW));
    if(sds_ssfree(ss1)!=RTNORM) rc=(-__LINE__);
    if(rc==0L) return(RTNORM); 
    if(rc==(-1L)) return(RTCAN); 
    if(rc<(-1L)) CMD_INTERNAL_MSG(rc);
    return(RTERROR);
}

long cmd_hatch_list(void) {
    char fs1[IC_ACADBUF],patstr[IC_ACADBUF],patnm[31],patdesc[81],*fcp1,scrollines[IC_ACADBUF];
    int ret,numlines=0,scroll=0;
    short ct;
    long rc=0L;
    FILE *fptr=NULL;

    if((ret=sds_getstring(0, ResourceString(IDC_HATCH__NPATTERN_S__TO_L_46, "\nPattern(s) to list <*>: " ),patstr))==RTERROR) {
        rc=(-__LINE__); goto bail;
    } else if(ret==RTCAN) {
        rc=(-1L); goto bail;
    }

	//***Set pattern file name
	//EBATECH(CNBR)-[ 2003/10/05 Default linetype file is dependent on $MEASUREMENT.
    char chHPFileName[IC_ACADBUF];
    resbuf SetGetRB;
	SDS_getvar(NULL,DB_QMEASUREMENT,&SetGetRB,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	strcpy( chHPFileName, ( SetGetRB.resval.rint == DB_ENGLISH_MEASUREINIT ?
		SDS_PATTERNFILE : SDS_ISOPATTERNFILE));

    if(!SearchHatchFile(chHPFileName,fs1)) {
        sds_printf(ResourceString(IDC_HATCH__NCOULD_NOT_FIND__47, "\nCould not find file \"%s\"." ),chHPFileName);
        rc=(-1L); goto bail;
    }
    if((fptr=fopen(fs1, "rt"/*DNT*/ ))==NULL) {
        sds_printf(ResourceString(IDC_HATCH__NCOULD_NOT_OPEN__49, "\nCould not open file \"%s\"." ),chHPFileName);
        rc=(-1L); goto bail;
    }
	//EBATECH(CNBR)]-
    if(!*patstr) 
        strcpy(patstr,"*"/*DNT*/);
    sds_textscr();
    sds_printf(ResourceString(IDC_HATCH__N__PATTERN_TYPE__50, "\n  Pattern Type                    Description\n" ));
    for(ct=0; ic_nfgets(fs1,sizeof(fs1)-1,fptr)!=0; ct++) {
        if(sds_usrbrk()) { rc=(-1L); goto bail; }
        if(*fs1!='*'/*DNT*/) continue;
        if((fcp1=strchr(fs1,','/*DNT*/))==NULL) continue;
        *fcp1=0;
        strncpy(patdesc,fcp1+1,sizeof(patnm)-1);
        strncpy(patnm,fs1+1,sizeof(patdesc)-1);
		strupr(patnm);
		strupr(patstr);
        if(sds_wcmatch(patnm,patstr)==RTNORM) {
            sds_printf("\n%-31s %s"/*DNT*/,patnm,patdesc); 
            numlines++;		
            if(numlines>=20 && !scroll) {
                sds_initget(0,ResourceString(IDC_HATCH_INITGET_SCROLL_51, "Scroll ~_Scroll" ));
                if((ret=sds_getkword(ResourceString(IDC_HATCH__NSCROLL__ENTER_T_52, "\nScroll/<ENTER to view next screen>:" ),scrollines))==RTERROR) {
                    rc=(-__LINE__); goto bail;    
                }else if(ret==RTCAN) {
                    rc=(-1L); goto bail;
                }else if(ret==RTNONE) {
                    numlines=0;
                }else if(ret==RTNORM) {
                    scroll=1;
                }
            }
        }
    }
    if(!ct) cmd_ErrorPrompt(CMD_ERR_NOPATTERNS,0);

    bail:

    if(fptr!=NULL) fclose(fptr);
    return(rc);
}

void cmd_hatch_freehpdefll(struct cmd_hpdefll **def2free){
	struct cmd_hplinedef *pdefstart,*pdef1,*pdef2;
	struct cmd_hpdefll *ldef1,*ldef2;

	if((*def2free)==NULL) return;

	for(ldef1=ldef2=(*def2free);ldef1!=NULL;){
		//free the llist of pen definitions within the line definition 
		if(ldef1->linedef!=NULL){
			//this is a CLOSED loop, where the last element's next is the first element
			for(pdefstart=pdef1=pdef2=ldef1->linedef;pdef2!=NULL;){
				pdef2=pdef2->next;
				delete pdef1;
				if(pdef2==pdefstart)break;
				pdef1=pdef2;
			}
			pdefstart=pdef1=pdef2=NULL;
		}
		//free the line definition itself
		ldef2=ldef1->next;
		delete ldef1;
		ldef1=ldef2;
	}
	ldef1=ldef2=(*def2free)=NULL;
	return;
}

int cmd_hatch_valid(char *hpname,char *patfile){
    //check for vaild pattern
	int fi1,fi2,ret=RTERROR;
	char fs1[IC_ACADBUF],fs2[IC_ACADBUF],fname[IC_ACADBUF],patname[IC_ACADNMLEN],*fcp1;
    FILE *fptr=NULL;

	fname[0] = 0;
	fs2[0] = 0;
	fs1[0] = 0;
	patname[0] = 0;

	fi2=fi1=0; //fi1 tracks which file we're looking in, fi2 tracks if we found pattern
	strncpy(patname,hpname,sizeof(patname)-1);
	if(sds_findfile(patfile/*SDS_PATTERNFILE*/,fname)==RTNORM){
		fi1=1;
	}
	do{
		if(fi1==0){
			if(NULL==(fcp1=strrchr(SDS_PATTERNFILE,'.'))){ret=RTERROR;goto exit;}
			sprintf(fs1,"%s%s"/*DNT*/,patname,fcp1);
			if(sds_findfile(fs1,fname)!=RTNORM){
				cmd_ErrorPrompt(CMD_ERR_FINDPATTERN,0);
				ret=RTERROR;goto exit;
			}
		}
		if((fptr=fopen(fname, "rt"/*DNT*/ ))==NULL) {
			sds_printf(ResourceString(IDC_HATCH__NCOULD_NOT_OPEN__49, "\nCould not open file \"%s\"." ),fname);		
			ret=RTERROR; goto exit;
		}
		sprintf(fs2, "*%s"/*DNT*/,patname);	
		while(NULL!=ic_nfgets(fs1,IC_ACADBUF-1,fptr)){
			if(fs1[0]!='*'/*DNT*/)continue;
			if(NULL!=(fcp1=strchr(fs1,','/*DNT*/)))
				*fcp1=0;
			if(strisame(fs1,fs2)){
				fi2=1;
				break;	 //break out if we acquire pattern definition
			}
		}
		//if we're here, we haven't found pattern yet.  Look in alternate file
		//if we're not there already
		if(fi2==1)break;	//break if we found the pattern
		if(fi1==1){
			fi1=0;
		}
		
	}while(1);
	

    ret=RTNORM;
exit:	
	if(fptr!=NULL){ fclose(fptr); }
	
    return(ret);
}


int cmd_hatch_loadpat(const char *hpfile, const char *hpname,struct cmd_hpdefll **hplptr){
	//Loads the hatch pattern specified by hpname into the cmd_hpdefll structure.
	//Calling f'n must free list by calling cmd_hatch_freehpdefll.
	//Searches first in file hpfile for definition.  If not found, searches for
	//file (*HPNAME).PAT.  We don't limit to 6 up/down pen instructions per line like
	//ACAD does (we just make a looped linked list of pen instructions).

	//Function returns # of lines in pattern if pattern successfully loaded, RTERROR otherwise.
	
	int ret=0;
	int fi1,fi2,lc;
	char fs1[IC_ACADBUF],fs2[IC_ACADBUF],fname[IC_ACADBUF],
			patname[IC_ACADNMLEN],*fcp1;
    FILE *fptr=NULL;
	struct cmd_hpdefll *hplcur=NULL,*hpltemp=NULL,*hplbeg=NULL;
	struct cmd_hplinedef *hppcur=NULL,*hpptemp=NULL,*hppbeg=NULL;
	sds_real fr1;//double
	


	fi2=fi1=0; //fi1 tracks which file we're looking in, fi2 tracks if we found pattern
	strncpy(patname,hpname,sizeof(patname)-1);

	if (SearchHatchFile(hpfile, fname)) {
		fi1=1;
	}
	do{
		if(fi1==0){
			if(NULL==(fcp1=strrchr(SDS_PATTERNFILE,'.'))){ret=(RTERROR);lc=(__LINE__);goto exit;}
			sprintf(fs1, "%s%s"/*DNT*/,patname,fcp1);
			if(!SearchHatchFile(fs1,fname)){
				cmd_ErrorPrompt(CMD_ERR_FINDPATTERN,0);
				ret=RTERROR;goto exit;
			}
		}
		if((fptr=fopen(fname, "rt"/*DNT*/))==NULL) {
			sds_printf(ResourceString(IDC_HATCH__NCOULD_NOT_OPEN__49, "\nCould not open file \"%s\"." ),fname);		
			ret=RTERROR; goto exit;
		}
		sprintf(fs2,"*%s"/*DNT*/,patname);	
		while(NULL!=ic_nfgets(fs1,IC_ACADBUF-1,fptr)){
			if(fs1[0]!='*')continue;
			if(NULL!=(fcp1=strchr(fs1,',')))
				*fcp1=0;
			if(strisame(fs1,fs2)){
				fi2=1;
				break;	 //break out if we acquire pattern definition
			}
		}
		//if we're here, we haven't found pattern yet.  Look in alternate file
		//if we're not there already
		if(fi2==1)break;	//break if we found the pattern
		if(fi1==1){
			fi1=0;
			fclose(fptr);
		}else{
			break;
		}
	}while(1);
	if(fi2==0){
		cmd_ErrorPrompt(CMD_ERR_FINDPATTERN,0);
		goto exit;
	}
	//now populate hplcur with pattern definition....
	while(NULL!=ic_nfgets(fs1,IC_ACADBUF-1,fptr)){
		if(fs1[0]==0 || fs1[0]==';'/*DNT*/)continue;
		if(fs1[0]=='*'/*DNT*/){		//end of pattern definition
			if(hplcur==NULL){   //if nothing loaded into pattern,exit
					// We can use: cmd_ErrorPrompt(CMD_ERR_FINDPATTERN,0);
					// unless this means that a pattern of the given name *was* found,
					// but there's nothing in it.
				sds_printf(ResourceString(IDC_HATCH__NPATTERN_DEFINIT_53, "\nPattern definition not found." ));
				ret=RTERROR;
				goto exit;
			}
			break;
		}
		ret++;
		//now, we have the line definition in fs1 - populate structure....
		if((fcp1=strtok(fs1,","/*DNT*/))==NULL){ret=RTERROR;goto exit;}//read the angle
		if((hpltemp= new struct cmd_hpdefll )==NULL){
			cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
			ret=RTERROR;
			lc=(__LINE__);
			goto exit;
		}
		memset(hpltemp,0,sizeof(struct cmd_hpdefll));
		if(hplbeg==NULL){
			hplbeg=hplcur=hpltemp;
		}else{
			hplcur->next=hpltemp;
			hplcur=hpltemp;
		}
		hpltemp->angle=atof(fcp1)*IC_PI/180.0;
		if((fcp1=strtok(NULL,","/*DNT*/))==NULL){ret=RTERROR;goto exit;}//read x of startpt
		hpltemp->startpt[0]=atof(fcp1);
		if((fcp1=strtok(NULL,","/*DNT*/))==NULL){ret=RTERROR;goto exit;}//read y of startpt
		hpltemp->startpt[1]=atof(fcp1);
		if((fcp1=strtok(NULL,","/*DNT*/))==NULL){ret=RTERROR;goto exit;}//read x offset
		hpltemp->xoffset=atof(fcp1);
		if((fcp1=strtok(NULL,","/*DNT*/))==NULL){ret=RTERROR;goto exit;}//read y offset
		hpltemp->yoffset=atof(fcp1);
		//All arguments after this point are optional.  If no pen instructions given,
		//	a solid line is drawn
		if((hppcur= new cmd_hplinedef )==NULL){
			cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
			ret=(-1L);
			lc=(__LINE__);
			goto exit;
		}
		hpltemp->linedef=hppcur;
		hpptemp=hppbeg=hppcur;
		if((fcp1=strtok(NULL,","/*DNT*/))==NULL){
			hppcur->pendown=-1;		//NOTE pendown==-1 for continuous line
			hppcur->drawlength=0.0;
			hpltemp->linelen=-1.0;//set overall line length to -1, a solid line
		}else{
			//hpltemp->linelen==0.0 after calloc
			do{
				fr1=atof(fcp1);
				if(fr1<0.0){
					hpptemp->pendown=0;
					hpptemp->drawlength=-fr1;
					hpltemp->linelen-=fr1; //keep track of overall pattern length
				}else{	  //NOTE: length of 0.0 (dot) gets pendown value of 1
					hpptemp->pendown=1;
					hpptemp->drawlength=fr1;
					hpltemp->linelen+=fr1; //keep track of overall pattern length
				}
				if((fcp1=strtok(NULL,","/*DNT*/))==NULL) break;
				if((hpptemp= new cmd_hplinedef )==NULL){
					cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
					ret=(-1L);
					lc=(__LINE__);
					goto exit;
				}
				hppcur->next=hpptemp;
				hppcur=hpptemp;
			}while(1);
		}
		hppcur->next=hppbeg;	//close the loop of pen instructions
	}
	exit:
	if(fptr!=NULL) fclose(fptr);
	if(ret<0)cmd_hatch_freehpdefll(&hplbeg);
	else		   (*hplptr)=hplbeg;
	if(ret==(-1L)){
		CMD_INTERNAL_MSG(lc);
		ret=RTERROR;
	}
	return(ret);
}

// Note the space can be calculated in the load as it was in the original hatch command by setting the
// space parameter.  This can be handled outside the routine in the same way scale is handled by leaving
// space to a default 1;
/*D.Gavrilov*/// I've changed this function for real support of "User-defined" style.
bool cmd_loadUserDefPattern(cmd_hpdefll **hplcur, int hpdouble, double hpspace)
{
	cmd_hpdefll *hpltmp = NULL;
	struct gr_view * viewp = SDS_CURGRVW;
	int NumOfDashes, i;
	sds_real Dashes[DB_MAXLTDASHES];
	struct cmd_hplinedef * pDash;

	//manually define the hatch pattern to use
	if((*hplcur = new struct cmd_hpdefll ) == NULL)
	{
		cmd_ErrorPrompt(CMD_ERR_MOREMEM, 0);
		return false;
	}
	memset(*hplcur,0,sizeof(struct cmd_hpdefll));
	//NOTE: angle will be taken care of by overall pattern angle in drawing subroutine
	(*hplcur)->yoffset = hpspace;
	(*hplcur)->linelen = -1.0;  // it may be changed below
	(*hplcur)->next = NULL;
	if(((*hplcur)->linedef = new cmd_hplinedef ) == NULL)
	{
		cmd_ErrorPrompt(CMD_ERR_MOREMEM, 0);
		cmd_hatch_freehpdefll(hplcur);
		return false;
	}

	if(viewp->lthip != NULL)	// copy dashes from linetype to linedef
	{
		viewp->lthip->get_73(&NumOfDashes);
		viewp->lthip->get_49(Dashes, DB_MAXLTDASHES);
		(*hplcur)->linelen = ((db_ltypetabrec *)(viewp->lthip))->ret_patternlength();
		for(pDash = (*hplcur)->linedef, i = 0; i < NumOfDashes; i++)
		{
			if(i > 0)
			{
				if((pDash->next = new cmd_hplinedef ) == NULL)
				{
					cmd_ErrorPrompt(CMD_ERR_MOREMEM, 0);
					cmd_hatch_freehpdefll(hplcur);
					return false;
				}
				pDash = pDash->next;
			}
			pDash->pendown = Dashes[i] >= 0 ? 1 : 0;
			pDash->drawlength = fabs(Dashes[i]);
		}
		pDash->next = (*hplcur)->linedef;	// make a ring
	}
	else
	{
		(*hplcur)->linedef->pendown = -1;
		(*hplcur)->linedef->drawlength = 0.0;
		(*hplcur)->linedef->next = (*hplcur)->linedef; //point next back at itself(only one item)
	}

	if(hpdouble){
		//add 2nd line to pattern
		if((hpltmp = new struct cmd_hpdefll ) == NULL)
		{
			cmd_ErrorPrompt(CMD_ERR_MOREMEM, 0);
			cmd_hatch_freehpdefll(hplcur);
			return false;
		}
		memset(hpltmp,0,sizeof(struct cmd_hpdefll));
		hpltmp->angle = IC_PI/2.0;	//hpang will be added in drawing subroutine
		hpltmp->linelen = -1.0;  // it may be changed below
		hpltmp->yoffset = hpspace;
		hpltmp->next = NULL;
		(*hplcur)->next = hpltmp;  // insert into ll
		if((hpltmp->linedef = new cmd_hplinedef )==NULL)
		{
			cmd_ErrorPrompt(CMD_ERR_MOREMEM, 0);
			cmd_hatch_freehpdefll(hplcur);
			return false;
		}

		if(viewp->lthip != NULL)
		{
			hpltmp->linelen = ((db_ltypetabrec *)(viewp->lthip))->ret_patternlength();
			for(pDash = hpltmp->linedef, i = 0; i < NumOfDashes; i++)
			{
				if(i > 0)
				{
					if((pDash->next = new cmd_hplinedef ) == NULL)
					{
						cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
						cmd_hatch_freehpdefll(hplcur);
						return false;
					}
					pDash = pDash->next;
				}
				pDash->pendown = Dashes[i] >= 0 ? 1 : 0;
				pDash->drawlength = fabs(Dashes[i]);
			}
			pDash->next = hpltmp->linedef;  // make a ring
		}
		else
		{
			hpltmp->linedef->pendown = -1;
			hpltmp->linedef->drawlength = 0.0;
			hpltmp->linedef->next = hpltmp->linedef; //point next back at itself(only one item)
		}
		
	}

	return true;
}

long cmd_hatch_objects(struct cmd_hpdefll *hplstart, sds_name sset, int mode, int hpstyle,
					sds_real baseang, sds_real hpscale, char *hpname, int hplines){
	//does hatching of sset with hatch pattern loaded into hplcur.
	//MODE= 0:	Draw hatch as individual lines (current lay, ltype etc)
	//		1:  Draw hatch as anonymous block inserted at 0,0,0 (current layer)
	//		2:  Draw hatch using sds_grdraw (for Preview mode - so it vanishes on redraw
	//		3:	Same as mode 1, except hpdefll has already been scaled.  This happens if
	//				user wants to insert a previewed hatch permanently 

	//hpstyle=0 for std, 1 for outer only, 2 for ignore interior lines
	//baseang=angle user wants for pattern
	//hpscale=scale factor for pattern
	//hpname= name of pattern loaded into hpdefll.  if the std pattern, "&" is passed, and
	//	f'n will determine if it's cross hatched for proper eed value
	//hplines is number of lines in pattern def.  used only for progress bar
	
	/*D.Gavrilov*/// Create a real hatch object (not insert!) for SOLID.
	// I've take some code from BHATCH processing, but it doesn't work for all cases of boundaries
	// (see comments below and CR1583) because BHATCH uses "Pick Points" by user.
	// TO DO: Fix it and then use this way for all hatches (note, ACAD creates hatches, not inserts!).

	if(strisame(hpname, "SOLID"/*DNT*/))
	{
		// from CMainWindow::IcadBoundaryHatchDia
		RC  rc = 0L;
		C3Point  ptNormal;
		GetUcsZ(ptNormal);
		//Bugzilla No. 78414 ; 01-30-2003 
#ifdef VARYING_HATCH_TOLERANCE
		CBHatchBuilder  hatchBuilder(ptNormal, rc);
#else
		CBHatchBuilder  hatchBuilder(HATCH_TOLERANCE, ptNormal, rc);
#endif
		
		if(!SUCCEEDED(rc))
		{
			ASSERT(false);
			return rc;
		}
		db_hatch*  hatch = new db_hatch(SDS_CURDWG);
		hatchBuilder.set_hatch(hatch, true);
		// from BHatchDialog::OnInitDialog
		// from BHatchDialog::InitializeHatch
		hatch->set_75(hpstyle);
		hatch->set_52(baseang);
		hatch->set_71(0);	// non-associative
		hatch->set_2(hpname);
		hatch->set_76(strisame(hpname,"&"/*DNT*/) ? 0 : 1);	// predefined or user defined
		hatch->set_41(hpscale);
		if(hplstart)
			hatch->set_77(hplstart->next == NULL ? 0 : 1);	// cross (double) or not
		if(strsame("SOLID"/*DNT*/, hpname))
			hatch->set_70(1);
		// from BHatchDialog::RecalcPattern
		cmd_SetHatchFromDef(hplines, hplstart, hatch);
		// from cmd_getEntitySet
		sds_name  entity;
		HanditemCollection  collection;
		long  length;
		sds_sslength(sset, &length);
		for(int i = 0; i < length; i++)
		{
			rc = sds_ssname(sset, i, entity);
			if(rc == RTNORM)
				collection.AddItem(SDS_CURDWG, (db_handitem*)(entity[0]));
			else
				return -1L;
		}
		// from CMainWindow::PickPoints
		hatchBuilder.AcceptHanditemColl(SDS_CURDWG, collection, ptNormal);
		hatchBuilder.SetIslandDetection(hpstyle == 0 ? Nested_Islands :
										hpstyle == 2 ? Ignore_Islands : Outer_Only);
		// from cmd_boundaryPointSelection
		// from CBoundaryBuilder::ClickPoint
		CandidateList*  pCandidates = hatchBuilder.m_pCandidates;
		rc = hatchBuilder.m_pFragment->Operate();		// get intersections & split paths
		if(rc == SUCCESS)
			rc = hatchBuilder.m_pFragment->WrapUp();	// create closed boundaries
		if(rc == SUCCESS)
			rc = pCandidates->GetIslands();				// fill m_Islands array for each candidate
		if(rc == SUCCESS)
		{
			int  Size = pCandidates->Size();

			if(Size)
			{
				// Here are two ways of implementation of finding hatch finalists among the candidates.

				// THE FIRST: let they be those which have one or more the most or the least extent
				// (the most MaxX, the most MaxY, the least MinX and the least MinY).
				// This way is more quick then the 2nd but it has two disadvantages:
				// 1. it works wrong for some non-overlapping boundaries (to fix it all candidates should be grouped;
				// in non-intersecting groups and this way should be used for each group).
				// 2. it's results depend from an order of candidates in the list in case if some candidates have
				// one or more the same extents.
				// THE SECOND: use Candidate::GetContainer function to see if the candidate is not one of holes
				// in other candidates in the list.
				// This way is more slow then the 1st one but it has less code here. It works correctly for
				// non-overlapping boundaries but it's wrong for touching boundaries in some cases.

				// We use the 2nd way now (the 1st one is commented out).

				// NOTE, we do some additional things for finding finalists for both ways after them.

/*				i = 0;
				int			MinX_item(i), MaxX_item(i), MinY_item(i), MaxY_item(i);
				CExtents	ItemExtents = pCandidates->Item(i)->Extents();
				double	MinX(ItemExtents.MinX()),
						MaxX(ItemExtents.MaxX()),
						MinY(ItemExtents.MinY()),
						MaxY(ItemExtents.MaxY());
				double  tmp_ext;
				for(i = 1; i < Size; i++)
				{
					ItemExtents = pCandidates->Item(i)->Extents();

					if( ((tmp_ext = ItemExtents.MinX()) < MinX) ||
						(tmp_ext == MinX && !pCandidates->Item(MinX_item)->Extents().Contain(ItemExtents)) )
					{
						MinX = tmp_ext; MinX_item = i;
					}
					if( ((tmp_ext = ItemExtents.MaxX()) > MaxX) ||
						(tmp_ext == MaxX && !pCandidates->Item(MaxX_item)->Extents().Contain(ItemExtents)) )
					{
						MaxX = tmp_ext; MaxX_item = i;
					}
					if( ((tmp_ext = ItemExtents.MinY()) < MinY) ||
						(tmp_ext == MinY && !pCandidates->Item(MinY_item)->Extents().Contain(ItemExtents)) )
					{
						MinY = tmp_ext; MinY_item = i;
					}
					if( ((tmp_ext = ItemExtents.MaxY()) > MaxY) ||
						(tmp_ext == MaxY && !pCandidates->Item(MaxY_item)->Extents().Contain(ItemExtents)) )
					{
						MaxY = tmp_ext; MaxY_item = i;
					}
				}
				// don't refer to an item twice or more
				if(MinX_item == MaxX_item)
					MinX_item = -1;
				if(MinY_item == MaxY_item)
					MinY_item = -1;
				if(MaxX_item == MaxY_item)
					MaxX_item = -1;
*/
				// from CandidateList::ClickPoint for these items
				for(i = 0; i < Size; i++)
//					if(i == MinX_item || i == MaxX_item || i == MinY_item || i == MaxY_item)	// the FIRST way
					if( pCandidates->Item(i)->GetContainer(pCandidates, false) == -1 &&			// the SECOND way
						pCandidates->Item(i)->IsOpen(pCandidates) )
					{
						pCandidates->Item(i)->SetFinalist(true, pCandidates->m_RecursionDepth);
						pCandidates->Item(i)->SetHatchFinalist(true, MAX_ISLAND_DEPTH);
					}

				// Add some other candidates to the finalists set - holes (not islands!) in non-finalists.
				for(i = 0; i < Size; i++)
				{
					Candidate*	AnItem = pCandidates->Item(i);
					int			ItemContainerNum = AnItem->GetContainer(pCandidates, false);
					if(!AnItem->IsFinalist() && !AnItem->GetParent() && ItemContainerNum != -1 && !pCandidates->Item(ItemContainerNum)->IsFinalist())
					{
						AnItem->SetFinalist(true, pCandidates->m_RecursionDepth);
						AnItem->SetHatchFinalist(true, MAX_ISLAND_DEPTH);
					}
				}
			} // if(Size)
		}	// if(rc == SUCCESS)
		// from CMainWindow::IcadBoundaryHatchDia
		cmd_boundaryAccept(hatchBuilder);
		return rc;
	}

	char bname[IC_ACADNMLEN],eedname[IC_ACADBUF],did_draw;
	long ret=0L,fl1,fl2;
	sds_resbuf *rbp1,*rbp2,*rbplmake,*rbpstart,*rbpend,rb,**pa,rbucs,rbwcs,rbecs;
	sds_point ll,ur,ll2,ur2,snaporg,ucsorg,p1,p2,p3,p4,p5,p6,p7,p8, extentsLL, extentsUR;
	sds_real elev,angsin,angcos,bottom,top,fr1,fr2,fr3,fr4,xoffset,yoffset;
	struct cmd_hpdefll *hplcur;
	struct cmd_hplinedef *hppcur;
	int xhatch,curprog=0;

	if(hplstart==NULL)
		return(0L);

	rbp1=rbp2=rbplmake=rbpstart=rbpend=NULL;
	pa=NULL;

	// max num of lines calc variables
	double xlines, ylines;
	double xdist;
	double ydist;
	cmd_hpdefll *currenthp = hplstart;
	double numLines;
	double dashesPerLine;

	//get extents for selection set passed...
	if((ret=ic_ss2dexts(sset,extentsLL,extentsUR))!=0)goto exit;

	//See if pattern is too dense.  LOOSELY estimated to 10,000 line segments
	xlines = 0;
	ylines = 0;

	xdist = extentsUR[0] - extentsLL[0];
	ydist = extentsUR[1] - extentsLL[1];

	if (xdist < 0)
		xdist *= -1;

	if (ydist < 0)
		ydist *= -1;


	currenthp = hplstart;

	while (currenthp)
	{
		// if line is not continuous
		if (currenthp->linelen != -1) 
		{
			//At this point we don't know how many lines are in each def so just estimate based on hplines and linelength
			
			// x value
			if (currenthp->xoffset)
			{
				numLines = xdist / (currenthp->xoffset * hpscale);
				dashesPerLine = ydist / (currenthp->linelen * hpscale);
				xlines += numLines * dashesPerLine;
			}

			// y value
			if (currenthp->yoffset)
			{
				numLines = ydist / (currenthp->yoffset * hpscale);
				dashesPerLine = xdist / (currenthp->linelen * hpscale);
				ylines += numLines * dashesPerLine;
			}
		}
		else //continuous
		{
			if (currenthp->xoffset)
				xlines += xdist / (currenthp->xoffset * hpscale);

			if (currenthp->yoffset)
				ylines += ydist / (currenthp->yoffset * hpscale);
		}

		
		currenthp = currenthp->next;
	}

	if (max(xlines, ylines) > MAX_HATCH_LINES)
	{
		cmd_ErrorPrompt(CMD_ERR_R12HATCHTOOMANYLINES, 0);
		ret = SDS_RTERROR;
		goto exit;
	}
	
	rbucs.restype=rbwcs.restype=RTSHORT;
	rbucs.resval.rint=1;
	rbwcs.resval.rint=0;
	rbucs.rbnext=rbwcs.rbnext=NULL;
	SDS_getvar(NULL,DB_QUCSXDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	ic_ptcpy(p1,rb.resval.rpoint);
	SDS_getvar(NULL,DB_QUCSYDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	ic_crossproduct(p1,rb.resval.rpoint,rbecs.resval.rpoint);
	rbecs.restype=RT3DPOINT;

 	if(mode!=2)sds_entmake(NULL);

	if(mode&1){
		p5[0]=p5[1]=0.0;
		p5[2]=1.0;
		p1[0]=p1[1]=p1[2]=0.0;
       if((rbplmake=sds_buildlist(RTDXF0,"BLOCK"/*DNT*/,2,"*X"/*DNT*/,70,1,10,p1,0))==NULL)return(-1L);
		if(sds_entmake(rbplmake)!=RTNORM){ret=(-1L);goto exit;}
		sds_relrb(rbplmake);rbplmake=NULL;
	}


	if(mode!=2){
		if(mode&1){//mode==1 or 3
			//anonymous block, make lines w/color "BYBLOCK"
			if((rbplmake=sds_buildlist(RTDXF0,db_hitype2str(DB_LINE),10,p1,11,p2,62,0,210,rbecs.resval.rpoint,0))==NULL)return(-1L);
		}else{//mode==0
			//indiv lines w/default color
			if((rbplmake=sds_buildlist(RTDXF0,db_hitype2str(DB_LINE),10,p1,11,p2,210,rbecs.resval.rpoint,0))==NULL)return(-1L);
		}
		for(rbp2=rbplmake;rbp2!=NULL;rbp2=rbp2->rbnext){
			if(rbp2->restype==10)rbpstart=rbp2;
			if(rbp2->restype==11)rbpend=rbp2;
		}
	}
	SDS_getvar(NULL,DB_QELEVATION,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	elev=rb.resval.rreal;
	SDS_getvar(NULL,DB_QSNAPBASE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	ic_ptcpy(snaporg,rb.resval.rpoint);
	SDS_getvar(NULL,DB_QUCSORG,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	ic_ptcpy(ucsorg,rb.resval.rpoint);

	//check all 8 corners of the extentsLL-extentsUR cube!!!
	sds_trans(extentsLL,&rbwcs,&rbucs,0,p7);
	sds_trans(extentsUR,&rbwcs,&rbucs,0,p8);
	ll[0]=min(p7[0],p8[0]);
	ll[1]=min(p7[1],p8[1]);
	ur[0]=max(p7[0],p8[0]);
	ur[1]=max(p7[1],p8[1]);
	ic_ptcpy(p3,extentsLL);
	p3[0]=extentsUR[0];
	sds_trans(p3,&rbwcs,&rbucs,0,p8);
	cmd_hatch_2dptminmax(ll,ur,p8);
	ic_ptcpy(p3,extentsLL);
	p3[1]=extentsUR[1];
	sds_trans(p3,&rbwcs,&rbucs,0,p8);
	cmd_hatch_2dptminmax(ll,ur,p8);
	ic_ptcpy(p3,extentsLL);
	p3[2]=extentsUR[2];
	sds_trans(p3,&rbwcs,&rbucs,0,p8);
	cmd_hatch_2dptminmax(ll,ur,p8);
	ic_ptcpy(p3,extentsUR);
	p3[0]=extentsLL[0];
	sds_trans(p3,&rbwcs,&rbucs,0,p8);
	cmd_hatch_2dptminmax(ll,ur,p8);
	ic_ptcpy(p3,extentsUR);
	p3[1]=extentsLL[1];
	sds_trans(p3,&rbwcs,&rbucs,0,p8);
	cmd_hatch_2dptminmax(ll,ur,p8);
	ic_ptcpy(p3,extentsUR);
	p3[2]=extentsLL[2];
	sds_trans(p3,&rbwcs,&rbucs,0,p8);
	cmd_hatch_2dptminmax(ll,ur,p8);

	//for each line in the pattern, we'll set up a new coordinate system with
	//origin at SNAPORG and x-axis along the angle for drawing the line.
	did_draw=0;
	for(hplcur=hplstart;hplcur!=NULL;hplcur=hplcur->next){
		if(hplines>5){
			curprog++;
			if(hplcur==hplstart)SDS_ProgressStart();
			else{
				SDS_ProgressPercent((int)(((double)curprog/hplines)*100));
			}
		}
		
		if(mode!=3){
			hplcur->yoffset*=hpscale;
			hplcur->xoffset*=hpscale;
			hplcur->startpt[0]*=hpscale;
			hplcur->startpt[1]*=hpscale;
			hplcur->linelen*=hpscale;
		}
		//NOTE:  Pen up/down distances have NOT yet been adjusted for hatch scale!
		if(fabs(hplcur->yoffset)<CMD_FUZZ || fabs(hplcur->linelen)<CMD_FUZZ){
			//prevent inf loops by checking to see if pen instructions are bogus
			continue;
		}
		angsin=sin(hplcur->angle+baseang);
		angcos=cos(hplcur->angle+baseang);
		fr1=sin(hplcur->angle);
		fr2=cos(hplcur->angle);
		xoffset=(hplcur->startpt[0]*fr2)+(hplcur->startpt[1]*fr1);
		yoffset=(hplcur->startpt[1]*fr2)-(hplcur->startpt[0]*fr1);
		//Now, define a coordinate system based upon this hatch line's base pt & angle
		//get new limits ll2 & ur2
		p1[0]=(ll[0]-snaporg[0])*angcos+(ll[1]-snaporg[1])*angsin;
		p1[1]=(snaporg[0]-ll[0])*angsin+(ll[1]-snaporg[1])*angcos;
		p2[0]=(ll[0]-snaporg[0])*angcos+(ur[1]-snaporg[1])*angsin;
		p2[1]=(snaporg[0]-ll[0])*angsin+(ur[1]-snaporg[1])*angcos;
		p3[0]=(ur[0]-snaporg[0])*angcos+(ur[1]-snaporg[1])*angsin;
		p3[1]=(snaporg[0]-ur[0])*angsin+(ur[1]-snaporg[1])*angcos;
		p4[0]=(ur[0]-snaporg[0])*angcos+(ll[1]-snaporg[1])*angsin;
		p4[1]=(snaporg[0]-ur[0])*angsin+(ll[1]-snaporg[1])*angcos;
		ll2[2]=ur2[2]=snaporg[2];
		ll2[0]=min(p1[0],p2[0]);
		ll2[0]=min(ll2[0],p3[0]);
		ll2[0]=min(ll2[0],p4[0]);
		ll2[1]=min(p1[1],p2[1]);
		ll2[1]=min(ll2[1],p3[1]);
		ll2[1]=min(ll2[1],p4[1]);
		ur2[0]=max(p1[0],p2[0]);
		ur2[0]=max(ur2[0],p3[0]);
		ur2[0]=max(ur2[0],p4[0]);
		ur2[1]=max(p1[1],p2[1]);
		ur2[1]=max(ur2[1],p3[1]);
		ur2[1]=max(ur2[1],p4[1]);
		//now we have LowerLeft & UpperRight for sset in new UCS for this pattern line

		modf((ll2[1]-yoffset)/hplcur->yoffset,&fr1);
		modf((ur2[1]-yoffset)/hplcur->yoffset,&fr2);
		bottom=min(fr1,fr2);
		top=max(fr1,fr2);
		p1[2]=p2[2]=snaporg[2];
		p1[0]=ll2[0];
		p2[0]=ur2[0];
		for(fr1=bottom-1;fr1<=top+1;fr1+=1.0){	
			//fr1 will count ROW number (pos or neg w/0.0=SNAPORG)
			if(sds_usrbrk()){ret=(-1L);goto exit;}
			p1[1]=p2[1]=(fr1*hplcur->yoffset)+yoffset;
			cmd_hatch_hatch2ucs(p1,p3,snaporg,angsin,angcos);
			cmd_hatch_hatch2ucs(p2,p4,snaporg,angsin,angcos);
			ic_entxss(p3,p4,sset,(CMD_FUZZ*hplcur->yoffset),&rbp1,&fl1,8+16);//entxss takes ucs pts
			//note: fl1 is 1-based (# of intersections found)
			if(fl1>(1L)){	//if we found a region to hatch...
				//points are returned in no particular order.  We
				//have to sort 'em.  Allocate sorting array
				if((pa= new resbuf * [fl1] )==NULL){ret=(-1L);goto exit;}
				//populate sorting array, and convert points back into hatch coordinates
				for(fl2=0L,rbp2=rbp1;fl2<fl1 && rbp2!=NULL;fl2++, rbp2=rbp2->rbnext){
					pa[fl2]=rbp2;
					ic_ptcpy(p8,rbp2->resval.rpoint);
					p8[2]=elev;
					cmd_hatch_ucs2hatch(p8,rbp2->resval.rpoint,snaporg,angsin,angcos);
				}
				//sort the resbufs w/points in HATCH COORDINATES!
				qsort(pa,fl1,sizeof(struct resbuf *),cmd_hatch_qsortcomp);
				fl1--;	//go from 1-based to 0-based
				//we didn't ask to eliminate duplicates from list, but we DO need to eliminate
				//duplicates at start/end of crossing list
				if(fl1>=2){
					if(icadPointEqual(pa[0]->resval.rpoint,pa[1]->resval.rpoint)){
						//drop every number down one.  can't just start at 2nd element, 
						//because on/off checker won't work right 
						for(fl2=0;fl2<fl1;fl2++){
							ic_ptcpy(pa[fl2]->resval.rpoint,pa[fl2+1]->resval.rpoint);
						}
						fl1--;//set new ceiling for intersections
					}
					if(icadPointEqual(pa[fl1]->resval.rpoint,pa[fl1-1]->resval.rpoint))fl1--;//set new ceiling
				}
				for(fl2=0;fl2<fl1;fl2++){
					if(hpstyle==2){
						ic_ptcpy(p3,pa[0]->resval.rpoint);
						ic_ptcpy(p4,pa[fl1]->resval.rpoint);
					}else{
						if(!cmd_hatch_onoff(fl2+1,fl1+1,hpstyle))	continue;
						ic_ptcpy(p3,pa[fl2]->resval.rpoint);
						ic_ptcpy(p4,pa[fl2+1]->resval.rpoint);
					}
					//hatch block's lines are defined in ucs, which is treated as wcs for block def purposes!!
					if(hplcur->linedef->pendown==-1){	//draw solid line betw pts
						cmd_hatch_hatch2ucs(p3,p7,snaporg,angsin,angcos);
						cmd_hatch_hatch2ucs(p4,p8,snaporg,angsin,angcos);
						if(mode!=2){
							//pts in llist are in HCS
							sds_trans(p7,&rbucs,&rbwcs,0,rbpstart->resval.rpoint);
							sds_trans(p8,&rbucs,&rbwcs,0,rbpend->resval.rpoint);
							sds_entmake(rbplmake);
						}else{
							sds_grdraw(p7,p8,-1,0);//grdraw uses ucs pts
						}
						did_draw=1;
					}else{
						//we need to draw the pattern between p3 & p4 (hatch coord system points)
						//make copies of p3 & p4 (HCS) & assure they're in ascending X order
						if(p3[0]<=p4[0]){
							ic_ptcpy(p5,p3);
							ic_ptcpy(p6,p4);
						}else{
							ic_ptcpy(p6,p3);
							ic_ptcpy(p5,p4);
							ic_ptcpy(p3,p5);
							ic_ptcpy(p4,p6);
						}
						//NOTE:  Pen up/down distances in pattern lines have NOT yet been
						//		adjusted for hatch scale!  All params in hplcur OK.
						
						//calc how many patterns from origin, & go left one more
						modf((p3[0]-xoffset)/hplcur->linelen,&fr2);
						if(fr2<=0.0)fr2-=1.0;
						//fr3 will track starting X value for drawing
						fr3=(fr2*hplcur->linelen)+xoffset;
						if(!icadRealEqual(hplcur->xoffset,0.0)){
							modf((fr1*hplcur->xoffset/hplcur->linelen),&fr4);
							fr3-=fr4*hplcur->linelen;
							fr3+=fr1*hplcur->xoffset;
						} 
 						hppcur=hplcur->linedef;
						while(fr3<p4[0]){
							fr2=hppcur->drawlength*hpscale;
							if(hppcur->pendown==1 && (fr3+fr2)>p3[0]){  
								if(sds_usrbrk()){ret=-1L;goto exit;}
								//if we're in range & are in draw mode....
								if(fr3<p3[0]) 
									p5[0]=p3[0];
								else
									p5[0]=fr3;
								if(fr3+fr2>p4[0])
									p6[0]=p4[0];
								else
									p6[0]=fr3+fr2;
								cmd_hatch_hatch2ucs(p5,p7,snaporg,angsin,angcos);
								cmd_hatch_hatch2ucs(p6,p8,snaporg,angsin,angcos);
								did_draw=1;
								if(mode!=2){
									sds_trans(p7,&rbucs,&rbwcs,0,rbpstart->resval.rpoint);
									sds_trans(p8,&rbucs,&rbwcs,0,rbpend->resval.rpoint);
									sds_entmake(rbplmake);
								}else{						// If we are just displaying the hatch in Preview mode...
									sds_grdraw(p7,p8,-1,0);
								}
							}
							fr3+=fr2;
							hppcur=hppcur->next;	//walk to next item in continuous loop
						}
						
					}
					if(hpstyle==2)break;
				}
				
				if(rbp1!=NULL){
					sds_relrb(rbp1);
					rbp1=rbp2=NULL;
				}
				if(pa!=NULL){
					IC_FREE(pa);
					pa=NULL;
				}
			}else{
				if(rbp1!=NULL){
					sds_relrb(rbp1);
					rbp1=rbp2=NULL;
				}
			}
		}
	}
	if(0==did_draw){
		sds_entmake(NULL);
		sds_printf(ResourceString(IDC_HATCH__NPATTERN_SCALE_T_54, "\nPattern scale too large for selected boundary." ));
		ret=-1L;
		goto exit;
	}
	if(mode&1){		//end the block def and make the insert
		if(strisame(hpname,"&"/*DNT*/)){
			strncpy(eedname,"U"/*DNT*/,sizeof(eedname)-1);
			if(hplstart->next==NULL)xhatch=0;
			else xhatch=1;
		}else{
			strncpy(eedname,hpname,sizeof(eedname)-1);
			xhatch=-1;
		}
		if(hpstyle==2)
            strcat(eedname,",I"/*DNT*/);
		else if(hpstyle==1)
            strcat(eedname,",O"/*DNT*/);
		
		if(rbplmake!=NULL){sds_relrb(rbplmake);rbplmake=NULL;}
       if((rbplmake=sds_buildlist(RTDXF0,"ENDBLK"/*DNT*/,0))==NULL){ret=(-1L);goto exit;}
		if(sds_entmake(rbplmake)!=RTKWORD){ret=(-1L);goto exit;}
		if(sds_getinput(bname)!=RTNORM){ret=(-1L);goto exit;}
		sds_relrb(rbplmake);rbplmake=NULL;
		p1[0]=p1[1]=p1[2]=p2[0]=p2[1]=0.0;
		p2[2]=1.0;
		//p1[2]=snaporg[2];
		if(xhatch==-1){
           if((rbplmake=sds_buildlist(RTDXF0,"INSERT"/*DNT*/,
									   2,bname,
									   10,p1,
									   210,p2,
									   -3,
									   1001,"ACAD"/*DNT*/,
									   1000,"HATCH"/*DNT*/,
									   1002,"{"/*DNT*/,
									   1070,19,
									   1000,eedname,
									   1040,hpscale,
									   1040,baseang,
									   1002,"}"/*DNT*/,
									   0))==NULL){ret=(-1L);goto exit;}
		}else{
           if((rbplmake=sds_buildlist(RTDXF0,"INSERT"/*DNT*/,
									   2,bname,
									   10,p1,
									   210,p2,
									   -3,
									   1001,"ACAD"/*DNT*/,
									   1000,"HATCH"/*DNT*/,
									   1002,"{"/*DNT*/,
									   1070,19,
									   1000,eedname,
									   1040,hplstart->yoffset,//std patter's scale is always 1, w/
									   // distance between lines specified in pattern definition
									   1040,baseang,
									   1070,xhatch,
									   1002,"}"/*DNT*/,
									   0))==NULL){ret=(-1L);goto exit;}
		}
		if(sds_entmake(rbplmake)!=RTNORM){ret=(-1L);goto exit;}
	}


	exit:
	if(curprog>0)SDS_ProgressStop();
	if(rbp1!=NULL){sds_relrb(rbp1);}
	if(rbplmake!=NULL){sds_relrb(rbplmake);}
	rbp1=rbp2=rbplmake=rbpstart=rbpend=NULL;
	if(pa!=NULL){IC_FREE(pa);pa=NULL;}
	return(ret);
}

short cmd_hatch_onoff(long thisint, long numints, int hpstyle){
	//returns 1 if after this intersection we should be pen down
	//		  0 if after this inters we should be pen up
	//hpstyle = current value of hpstyle(std, outer only, all)
	//numints = number of intersections (1-based) of current hatch line w/sset of
	//				entities to be hatched (always 1 more than number of areas)
	int fi1;

	numints--;  //go from # of intersections to # of regions
	if(hpstyle==2)return(1);
	//if first or last region, always return 1
	if(thisint==1 || thisint==numints)return(1);
	//we're not in 1st or last region, so if hpstyle is outer, return 0
	if(hpstyle==1) return(0);
	//hpstyle==0, standard hatch format...
	fi1=thisint%2;
	if((numints%2)==0 && thisint>(numints/2.0))
		//if even # of regions and we're in upper half of region numbers
		return(!fi1);
	else
		return(RT(fi1));
}


void cmd_hatch_ucs2hatch(sds_point ucspt,sds_point hatchpt,sds_point hatchorg,sds_real angsin,sds_real angcos){
	hatchpt[2]=ucspt[2];
	hatchpt[0]=(ucspt[0]-hatchorg[0])*angcos+(ucspt[1]-hatchorg[1])*angsin;
	hatchpt[1]=(hatchorg[0]-ucspt[0])*angsin+(ucspt[1]-hatchorg[1])*angcos;
}

void cmd_hatch_hatch2ucs(sds_point hatchpt,sds_point ucspt,sds_point hatchorg,sds_real angsin,sds_real angcos){
	ucspt[2]=hatchpt[2];
	ucspt[0]=hatchpt[0]*angcos-hatchpt[1]*angsin+hatchorg[0];
	ucspt[1]=hatchpt[0]*angsin+hatchpt[1]*angcos+hatchorg[1];
}
int cmd_hatch_qsortcomp(const void *vp1, const void *vp2){
	//NOTE: can't use a "hard equals" in case of floating pt error...
	if(icadRealEqual((*((struct resbuf **)vp1))->resval.rpoint[0],(*((struct resbuf **)vp2))->resval.rpoint[0])){
		//if the x's are equal, sort on the y's
		if((*((struct resbuf **)vp1))->resval.rpoint[1]<
		   (*((struct resbuf **)vp2))->resval.rpoint[1]) return(-1);
		else if((*((struct resbuf **)vp1))->resval.rpoint[1]>
				(*((struct resbuf **)vp2))->resval.rpoint[1]) return(1);
		else return(0);
	}else{
		//sort on the x's
		if((*((struct resbuf **)vp1))->resval.rpoint[0]<
		   (*((struct resbuf **)vp2))->resval.rpoint[0]) return(-1);
		//else if((*((struct resbuf **)vp1))->resval.rpoint[0]>
		//		(*((struct resbuf **)vp2))->resval.rpoint[0]) return(1);
		else return(1);
	}
}
void cmd_hatch_2dptminmax(sds_point ll,sds_point ur,sds_point pt){
	if(pt[0]<ll[0])ll[0]=pt[0];
	if(pt[1]<ll[1])ll[1]=pt[1];
	if(pt[0]>ur[0])ur[0]=pt[0];
	if(pt[1]>ur[1])ur[1]=pt[1];
}

int cmd_hatch_blkmidpt(sds_name ename, sds_point result){
	//used principally to calculate a point within a hatch block at which the 
	//	hatch's grip should be placed.  We'll use the average of all the pen-up
	//	pen-down lines in the block. (full 3D point is returned, including Z)  
	//NOTE: Just to be safe, for non-hatch blocks we'll filter out all non-line
	//		entities in the block
	// Result is returned in UCS COORDINATES!!!
	//
	//RETURNS:  RTNORM  if successful in calculating result
	//			RTERROR otherwise
	sds_name ent;
	char blkname[IC_ACADBUF];
	struct resbuf *rbp1,*rbptemp,rbucs,rbecs;
	int ret=RTNORM,fi1;
	long npoints;
	sds_point total,inspt,bscale,base;
	sds_real *pd1,rot,angsin,angcos;
	db_handitem *hip;
	//db_drawing *flp;

	rbucs.restype=RTSHORT;
	rbucs.resval.rint=1;
	rbecs.restype=RT3DPOINT;
	rbecs.resval.rpoint[0]=rbecs.resval.rpoint[1]=0.0;
	rbecs.resval.rpoint[2]=1.0;

	rbp1=NULL;
	if(NULL==(rbp1=sds_entget(ename))){ret=RTERROR;goto exit;}
	for(rbptemp=rbp1;rbptemp!=NULL;rbptemp=rbptemp->rbnext){
		if(rbptemp->restype==0 && !strsame(rbptemp->resval.rstring,"INSERT"/*DNT*/)){ret=RTERROR;goto exit;}
		else if(rbptemp->restype==2)strncpy(blkname,rbptemp->resval.rstring,sizeof(blkname)-1);
		else if(rbptemp->restype==10)ic_ptcpy(inspt,rbptemp->resval.rpoint);
		else if(rbptemp->restype==41)bscale[0]=rbptemp->resval.rreal;
		else if(rbptemp->restype==42)bscale[1]=rbptemp->resval.rreal;
		else if(rbptemp->restype==43)bscale[2]=rbptemp->resval.rreal;
		else if(rbptemp->restype==50)rot=rbptemp->resval.rreal;
		else if(rbptemp->restype==210)ic_ptcpy(rbecs.resval.rpoint,rbptemp->resval.rpoint);
	}
	sds_relrb(rbp1);rbp1=NULL;
   if(NULL==(rbp1=sds_tblsearch("BLOCK"/*DNT*/,blkname,0)))return(RTERROR);
	for(rbptemp=rbp1;rbptemp!=NULL;rbptemp=rbptemp->rbnext){
		if(rbptemp->restype==-2)ic_encpy(ent,rbptemp->resval.rlname);
		else if(rbptemp->restype==10)ic_ptcpy(base,rbptemp->resval.rpoint);
	}
	hip=(db_handitem *)ent[0];
	//flp=(db_drawing *)ent[1];
	total[0]=total[1]=total[2]=0.0;
	npoints=0L;
	for( ;hip!=NULL;hip=hip->next){
		if(DB_ENDBLK==(fi1=hip->ret_type()))break;
		if(fi1!=DB_LINE || hip->ret_deleted())continue;
		pd1=hip->retp_10();
		total[0]+=*pd1;
		total[1]+=*(pd1+1);
		total[2]+=*(pd1+2);
		pd1=hip->retp_11();
		total[0]+=*pd1;
		total[1]+=*(pd1+1);
		total[2]+=*(pd1+2);
		npoints+=2;
	}
	if(0L==npoints){ret=RTERROR;goto exit;}
	total[0]/=npoints;
	total[1]/=npoints;
	total[2]/=npoints;
	total[0]-=base[0];
	total[1]-=base[1];
	total[2]-=base[2];
	//now scale then rotate then translate
	total[0]*=bscale[0];
	total[1]*=bscale[1];
	total[2]*=bscale[2];
	angsin=sin(rot);angcos=cos(rot);
	result[0]=(total[0])*angcos-(total[1])*angsin;
	result[1]=(total[1])*angcos+(total[0])*angsin;
	result[2]=total[2];

	result[0]+=inspt[0];
	result[1]+=inspt[1];
	result[2]+=inspt[2];
	sds_trans(result,&rbecs,&rbucs,0,result);

	exit:
	if(rbp1!=NULL){sds_relrb(rbp1);rbp1=NULL;}
	return(ret);
}
