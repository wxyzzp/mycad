/* LAYER.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * Supports the layer commands
 *
 */
// THIS FILE HAS BEEN GLOBALIZED!

//** Includes
#include "cmds.h"/*DNT*/
#include "Icad.h" /*DNT*/
#include "IcadApi.h"

// Protos
short cmd_layer_check(char *layerlst);
long cmd_layer_alter(char *layerlst, short group, short value, char *valstr,int *needregen);
long cmd_layer_list(void);

short cmd_layer(void) {

	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	char fs1[IC_ACADBUF],fs2[IC_ACADBUF],fs3[IC_ACADBUF],curlay[IC_ACADNMLEN],
		ltype[IC_ACADNMLEN],*fcp1;
	struct resbuf rb,setgetrb,*tmprb=NULL,*tmprb2=NULL;
	int ret,needregen=0;
	short newcol;
	long rc=0L;

	//*** Get the curent layer name.
	if(SDS_getvar(NULL,DB_QCLAYER,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-__LINE__); goto bail; }
	strncpy(curlay,setgetrb.resval.rstring,sizeof(curlay)-1);
	IC_FREE(setgetrb.resval.rstring);
	for(;;) {
		LOAD_COMMAND_OPTIONS_2(IDC_LAYER_INITGET_LIST_EXIST_0);
		if(sds_initget(0, LOADEDSTRING)!=RTNORM) {
			rc=(-__LINE__);
			goto bail;
		}
		if((ret=sds_getkword(ResourceString(IDC_LAYER__NLAYER_____TO_LIS_1, "\nLayer:  ? to list/New/Make/Set/Color/Ltype/LWeight/ON/OFF/Plot/Freeze/Thaw/LOck/Unlock: " ),fs1))==RTERROR) {
			rc=(-__LINE__);
			goto bail;
		} else if(ret==RTCAN) {
			rc=(-1L);
			goto bail;
		} else if(ret==RTNONE) {
			break;
		}
		strupr(fs1);
		if(strisame("?"/*DNT*/,fs1)) {
			if((rc=cmd_layer_list())!=0) goto bail;
			sds_printf("\n"/*DNT*/);
			continue;
		} else if(strisame("MAKE"/*DNT*/,fs1) || strisame("SET"/*DNT*/,fs1) || strisame("NEW"/*DNT*/,fs1) ) {


			if(strisame("NEW"/*DNT*/,fs1)) {
				sprintf(fs3,ResourceString(IDC_LAYER__NNAMES_FOR_NEW_LA_2, "\nNames for new layers: " ));
			} else if(strisame("SET"/*DNT*/,fs1)) {
				sprintf(fs3,ResourceString(IDC_LAYER__NLAYER_TO_SET_AS__3, "\nLayer to set as current <%s>: " ),curlay);
			} else if (strisame("MAKE"/*DNT*/,fs1)) {
				sprintf(fs3,ResourceString(IDC_LAYER__NNEW_LAYER_TO_MAK_4, "\nNew layer to make current: " ));
			}


			if((ret=sds_getstring(1,fs3,fs2))==RTERROR) {
				rc=(-__LINE__); goto bail;
			} else if(ret==RTCAN) {
				continue;
			}

			ic_trim(fs2,"be");


			if(!*fs2 && (strisame("NEW"/*DNT*/,fs1)))
				continue;

			strupr(fs2);

			// Make layer/New layers
			if(strisame("MAKE"/*DNT*/,fs1) ||
			   strisame("NEW"/*DNT*/,fs1)) {

				//alloc a new rb llist for layer EXCEPT for layer name...
				if((tmprb=sds_buildlist(RTDXF0,"LAYER"/*DNT*/,70,0,62,7,6,"CONTINUOUS"/*DNT*/,0))==NULL) {
					cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
					rc=(-1L); goto bail;
				}
				ic_assoc(tmprb,6);
				ic_rbassoc->rbnext=&setgetrb;
				setgetrb.restype=2;
				setgetrb.rbnext=NULL;
				fcp1=fs2;
				while(*fs2 && fcp1!=NULL){
					if((fcp1=strchr(fs2,','/*DNT*/))!=NULL)
						*fcp1=0;
					if(!ic_isvalidname(fs2)){
						cmd_ErrorPrompt(CMD_ERR_LAYERNAME,0);
						//sds_printf("\n-- Invalid layer name %s. --",fs2);
				   }else if((tmprb2=sds_tblsearch("LAYER"/*DNT*/,fs2,0))!=NULL){
						if(strisame("MAKE"/*DNT*/,fs1)) {
							//make sure layer is on
							ic_assoc(tmprb2,62);
							if(ic_rbassoc->resval.rint<0){
								ic_rbassoc->resval.rint=-ic_rbassoc->resval.rint;
								SDS_tblmesser(tmprb2,IC_TBLMESSER_MODIFY,SDS_CURDWG);
							}
							strncpy(curlay,fs2,sizeof(curlay)-1);
						}
						sds_relrb(tmprb2);tmprb2=NULL;
						SDS_getvar(NULL,DB_QCMDECHO,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
						if(strisame("NEW"/*DNT*/,fs1) &&
																							CMD_CMDECHO_PRINTF(rb.resval.rint))
						   sds_printf(ResourceString(IDC_LAYER__NLAYER__S_ALREADY_5, "\nLayer %s already exists." ) ,fs2);
					}else{
						setgetrb.resval.rstring=fs2;
						ret=SDS_tblmesser(tmprb,IC_TBLMESSER_MAKE,SDS_CURDWG);
						if(strisame("MAKE"/*DNT*/,fs1))
						   strncpy(curlay,fs2,sizeof(curlay)-1);
					}
					if(fcp1!=NULL)
						strncpy(fs2,fcp1+1,sizeof(fs2)-1);
				}
				ic_assoc(tmprb,6);
				ic_rbassoc->rbnext=NULL;
				IC_RELRB(tmprb);
				if(strisame("MAKE"/*DNT*/,fs1)){
					setgetrb.restype=RTSTR;
					setgetrb.resval.rstring=curlay;
					if((ret=sds_setvar("CLAYER"/*DNT*/,&setgetrb))!=RTNORM){
						rc=(-__LINE__);
						goto bail;
					}
				}
			} else {   // *fs1=='S', so set current layer
				if(*fs2){
				   if((tmprb=sds_tblsearch("LAYER"/*DNT*/,fs2,0))==NULL) {
					   sds_printf(ResourceString(IDC_LAYER__NCANNOT_FIND_LAYE_6, "\nCannot find layer %s." ) ,fs2);
						SDS_getvar(NULL,DB_QCLAYER,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
						strncpy(curlay,setgetrb.resval.rstring,sizeof(curlay)-1);
						continue;
					}else{
						ic_assoc(tmprb,70);
						if(ic_rbassoc->resval.rint & IC_ENTRY_XREF_DEPENDENT){
							cmd_ErrorPrompt(CMD_ERR_XRCURLAY,0); continue;
						}
						if(ic_rbassoc->resval.rint & IC_LAYER_FROZEN){
							cmd_ErrorPrompt(CMD_ERR_NOFROZEN,0); continue;
						}
						ic_assoc(tmprb,62);
						if(ic_rbassoc->resval.rint<0){
							//set the layer on
							ic_rbassoc->resval.rint=-ic_rbassoc->resval.rint;
							SDS_tblmesser(tmprb,IC_TBLMESSER_MODIFY,SDS_CURDWG);
							if(needregen==0) SDS_SetUndo(IC_UNDO_REGEN_ON_UNDO,NULL,NULL,NULL,SDS_CURDWG);
							needregen=1;	// Only need to regen if a layer was turned from off to on.
						}
						strncpy(curlay,fs2,sizeof(curlay)-1);
						setgetrb.restype=RTSTR;
						setgetrb.resval.rstring=curlay;
						setgetrb.rbnext=NULL;
						if(sds_setvar("CLAYER"/*DNT*/,&setgetrb)!=RTNORM) {
							rc=(-__LINE__);
							goto bail;
						}
						sds_relrb(tmprb);tmprb=NULL;
					}
					IC_RELRB(tmprb);
				}
			}
			continue;
		} else if(strisame("ON"/*DNT*/,fs1)) {
			if((ret=sds_getstring(1,ResourceString(IDC_LAYER__NLAYER_S_TO_TURN__7, "\nLayer(s to turn On: " ),fs2))==RTERROR) {
				rc=(-__LINE__);
				goto bail;
			} else if(ret==RTCAN) {
				continue;
			}
			ic_trim(fs2,"be");
			if(needregen==0) SDS_SetUndo(IC_UNDO_REGEN_ON_UNDO,NULL,NULL,NULL,SDS_CURDWG);
			needregen=1;
			// Modified CyberAge PP 08/06/2000 [
			// Fix for bug no. 57437
			// if(cmd_layer_alter(fs2,62,0,"ON"/*DNT*/,&needregen)!=0) goto bail;
			cmd_layer_alter(fs2,62,0,"ON"/*DNT*/,&needregen);
			continue;
			// Modified CyberAge PP 08/06/2000 ]
		} else if(strisame("OFF"/*DNT*/,fs1)) {
			if((ret=sds_getstring(1,ResourceString(IDC_LAYER__NLAYER_S_TO_TURN__8, "\nLayer(s to turn Off: " ),fs2))==RTERROR) {
				rc=(-__LINE__); goto bail;
			} else if(ret==RTCAN) {
				continue;
			}
			ic_trim(fs2,"be");
			if(needregen==0) SDS_SetUndo(IC_UNDO_REGEN_ON_UNDO,NULL,NULL,NULL,SDS_CURDWG);
			needregen=1;
			// Modified CyberAge PP 08/06/2000 [
			// Fix for bug no. 57437
			// if(cmd_layer_alter(fs2,62,0,"OFF"/*DNT*/,&needregen)!=0) goto bail;
			cmd_layer_alter(fs2,62,0,"OFF"/*DNT*/,&needregen);
			// Modified CyberAge PP 08/06/2000 ]
			continue;
		} else if(strisame("COLOR"/*DNT*/,fs1)) {
			for(;;) {
				if((ret=sds_getstring(0,ResourceString(IDC_LAYER__NCOLOR___9, "\nColor: " ) ,fs2))==RTERROR) {
					rc=(-__LINE__);
					goto bail;
				} else if(ret==RTCAN) {
					rc=(-1L); break;
				}
				strupr(fs2);
				// Check for valid color
				if(((newcol=ic_colornum(fs2))==7 && atoi(fs2)!=7 && !strnisame(fs2,ResourceString(IDC_LAYER_WHITE_10, "WHITE" ) ,strlen(fs2))) ||
				   (newcol==0 && atoi(fs2)!=0 &&
				   !strnisame(fs2,"BYBLOCK"/*DNT*/,strlen(fs2)) && !strnisame(fs2,"BYBLOCK"/*DNT*/,strlen(fs2))) ) {
					// Modified Cybage AJK 29/10/2001 [
				   // Reason: Fix for Bug No: 77865 from BugZilla
					//cmd_ErrorPrompt(CMD_ERR_COLOR0255,0);
					cmd_ErrorPrompt(CMD_ERR_COLOR1255,0);
					// Modified Cybage AJK 29/10/2001 DD/MM/YYYY ]
					continue;
				}
				// Modified Cybage AJK 22/10/2001 [
				 // Reason: Fix for Bug No: 77865 from BugZilla
				//if(newcol==256 ) {
				//cmd_ErrorPrompt(CMD_ERR_COLOR0255,0);
				if(newcol==256 || newcol==0) {
					cmd_ErrorPrompt(CMD_ERR_COLOR1255,0);
					continue;
				// Modified Cybage AJK 22/10/2001 DD/MM/YYYY ]
				 }
				break;
			}
			if(rc!=0) continue;
			sprintf(fs1,ResourceString(IDC_LAYER__NLAYER_S__TO_BE__11, "\nLayer(s) to be %s <%s>: " ),strupr(ic_colorstr(newcol,0)),curlay);
			if((ret=sds_getstring(1,fs1,fs2))==RTERROR) {
				rc=(-__LINE__); goto bail;
			} else if(ret==RTCAN) {
				continue;
			}
			ic_trim(fs2,"be");
			if(!*fs2) strcpy(fs2,curlay);
			if(needregen==0) SDS_SetUndo(IC_UNDO_REGEN_ON_UNDO,NULL,NULL,NULL,SDS_CURDWG);
			needregen=1;
			// Modified CyberAge PP 08/06/2000 [
			// Fix for bug no. 57437
			//if(cmd_layer_alter(fs2,62,newcol,NULL,&needregen)!=0) goto bail;
			cmd_layer_alter(fs2,62,newcol,NULL,&needregen);
			// Modified CyberAge PP 08/06/2000 ]
			continue;
		} else if(strisame("LTYPE"/*DNT*/,fs1)) {
			//*** Get the curent linetype.
			if(SDS_getvar(NULL,DB_QCELTYPE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) {
				rc=(-__LINE__);
				goto bail;
			}
			if(strisame(setgetrb.resval.rstring,"BYLAYER"/*DNT*/)) {
				strncpy(ltype,"CONTINUOUS"/*DNT*/,sizeof(ltype)-1);
			} else {
				strncpy(ltype,setgetrb.resval.rstring,sizeof(ltype)-1);
			}
			IC_FREE(setgetrb.resval.rstring);
			for(;;) {
				sprintf(fs1,ResourceString(IDC_LAYER__NLINETYPE_____TO_12, "\nLinetype:  ? to list/<%s>: " ) ,ltype);
				if((ret=sds_getstring(1,fs1,fs2))==RTERROR) {
					rc=(-__LINE__);
					goto bail;
				} else if(ret==RTCAN) {
					rc=(-1L);
					break;
				}
				
				ic_trim(fs2,"be");

				if(!*fs2)
					break;
				strupr(fs2);
				// EBATECH(CNBR) -[ 2002/9/14 -LAYER Ltype ?=List option can not work.
				if(strsame(fs2,"?"/*DNT*/)) {
					cmd_ltypelister();
					continue;
				}
				// EBATECH(CNBR) ]-
				if(!strsame(fs2,"BYBLOCK"/*DNT*/) && !strsame(fs2,"BYLAYER"/*DNT*/) || !strsame(fs2,"BYBLOCK"/*DNT*/) && !strsame(fs2,"BYLAYER"/*DNT*/)) {
					if(tmprb!=NULL) {
						rc=(-__LINE__);
						goto bail;
					}
				   if((tmprb=sds_tblsearch("LTYPE"/*DNT*/,fs2,0))==NULL) {
						//EBATECH(CNBR)-[ 2002/9/20 Default linetype file is dependent on $MEASUREMENT.
						SDS_getvar(NULL,DB_QMEASUREMENT,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
						if( cmd_ltload( fs2, ( setgetrb.resval.rint == DB_ENGLISH_MEASUREINIT ?
							SDS_LTYPEFILE : SDS_ISOLTYPEFILE), 0) != RTNORM )
						//if( cmd_ltload(fs2, "ICAD.LIN"/*DNT*/, 0) != RTNORM &&
						//	cmd_ltload(fs2, "ICADISO.LIN"/*DNT*/, 0) != RTNORM )	/*D.G.*/// icadiso added.
						//EBATECH(CNBR)]-
						{
							sds_printf(ResourceString(IDC_LAYER__NLINETYPE__S_NOT_13, "\nLinetype %s not found.  You can use LINETYPE to load it." ) ,fs2);
							continue;
						}
					}
					IC_RELRB(tmprb);
				}
				// if(strcmp("BYLAYER",fs2)==0) {
				//	  cmd_ErrorPrompt(CMD_ERR_LTYPENAME,0);
					// TODO -  clear this up one way or the other:
					// If this means that the way the name is written is wrong, use CMD_ERR_LTYPENAME
					// If it means that ICAD can't find a linetype by this name, use CMD_ERR_FINDLTYPE
				//	  continue;
				//}
				strncpy(ltype,fs2,sizeof(ltype)-1);
				break;
			}
			if(rc!=0) continue;
			sprintf(fs1,ResourceString(IDC_LAYER__NLAYER_S__FOR_LI_14, "\nLayer(s) for linetype %s <%s>: " ),ltype,curlay);
			if((ret=sds_getstring(1,fs1,fs2))==RTERROR) {
				rc=(-__LINE__);
				goto bail;
			} else if(ret==RTCAN) {
				continue;
			}

			ic_trim(fs2,"be");

			if(!*fs2)
				strcpy(fs2,curlay);
			if(needregen==0)
				SDS_SetUndo(IC_UNDO_REGEN_ON_UNDO,NULL,NULL,NULL,SDS_CURDWG);
			needregen=1;
			// Modified CyberAge PP 08/06/2000 [
			// Fix for bug no. 57437
			// if(cmd_layer_alter(fs2,6,0,ltype,&needregen)!=0) goto bail;
			cmd_layer_alter(fs2,6,0,ltype,&needregen);
			// Modified CyberAge PP 08/06/2000 ]
			continue;
		// EBATECH(CNBR) -[ 24-03-2003 Bugzilla#78471 Support Lineweight
		} else if(strisame("LWEIGHT"/*DNT*/,fs1)) {
			int lweight;
			for(;;) {
				if(sds_initget(0, ResourceString(IDC_LAYER__LWEIGHT_INITGET__37, "List_Lweight|? ~ Default|Default ~_? ~_ ~_Default" ))!=RTNORM) {
					rc=(-__LINE__);
					goto bail;
				}
				if ((ret=sds_getint(ResourceString(IDC_LAYER__LWEIGHT_PROMPT___38, "\nLineweight: ? to list/0-211/<Default>: ") ,&lweight))==RTERROR) {
					rc=(-__LINE__);
					goto bail;
				} else if(ret==RTCAN) {
					rc=(-1L);
					break;
				} else if(ret==RTKWORD) {
					sds_getinput(fs2);
					if(!*fs2)
						break;
					strupr(fs2);
					if(strsame(fs2,"?"/*DNT*/)) {
						cmd_lweightlister(DB_DFWMASK);
						continue;
					}
					else if( strsame(fs2,"DEFAULT"/*DNT*/)){
						lweight= DB_DFWEIGHT;
					}
					else {
						cmd_ErrorPrompt(CMD_ERR_LWEIGHT,0);
						continue;
					}
				} else if(ret==RTNONE) {
					lweight= DB_DFWEIGHT;
				}
				// Is lweight valid?
				if( db_is_validweight( lweight, DB_DFWMASK) == false ) {
					cmd_ErrorPrompt(CMD_ERR_LWEIGHT,0);
					continue;
				}
				break;
			}
			if(rc!=0) continue;
			sprintf(fs1,ResourceString(IDC_LAYER__LWEIGHT_LAYERS___39, "\nLayer(s) for Lineweight %d <%s>: "),lweight,curlay);
			if((ret=sds_getstring(1,fs1,fs2))==RTERROR) {
				rc=(-__LINE__);
				goto bail;
			} else if(ret==RTCAN) {
				continue;
			}
			
			ic_trim(fs2,"be");

			if(!*fs2)
				strcpy(fs2,curlay);
			if(needregen==0)
				SDS_SetUndo(IC_UNDO_REGEN_ON_UNDO,NULL,NULL,NULL,SDS_CURDWG);
			needregen=1;
			cmd_layer_alter(fs2,370,lweight,NULL,&needregen);
			continue;
		// EBATECH(CNBR) ]-
		// EBATECH(CNBR) -[ Change Plotting Settings
		} else if(strisame("PLOT"/*DNT*/,fs1)) {
			int plottable=1;
			if(sds_initget(0, ResourceString(IDC_LAYER__PLOT_INITGET_____46, "Yes-Plot|Yes No-Don't_plot|No ~_Yes ~_No"))!=RTNORM) {
				rc=(-__LINE__);
				goto bail;
			}
			if ((ret=sds_getkword(ResourceString(IDC_LAYER__PLOT_PLOMPT______47, "\nPlot settings: <Yes to plot>/No to don't plot: ") , fs2))==RTERROR) {
				rc=(-__LINE__);
				goto bail;
			} else if(ret==RTCAN) {
				continue;
			} else if(ret==RTNORM) {
				strupr(fs2);
				if(strisame("NO"/*DNT*/,fs2)) {
					plottable=0;
				}
			}

			if((ret=sds_getstring(1,ResourceString(IDC_LAYER__PLOT_LAYERS______48,"\nLayer(s) to change plot settings: "),fs2))==RTERROR) {
				rc=(-__LINE__);
				goto bail;
			} else if(ret==RTCAN) {
				continue;
			}
			ic_trim(fs2,"be");
			cmd_layer_alter(fs2,290,plottable,NULL,&needregen);
			continue ;
		// EBATECH(CNBR) ]-
		} else if(strisame("FREEZE"/*DNT*/,fs1)) {
			if((ret=sds_getstring(1,ResourceString(IDC_LAYER__NLAYER_S__TO_FRE_15, "\nLayer(s) to Freeze: " ),fs2))==RTERROR) {
				rc=(-__LINE__);
				goto bail;
			} else if(ret==RTCAN) {
				continue;
			}
			ic_trim(fs2,"be");
			if(needregen==0)
				SDS_SetUndo(IC_UNDO_REGEN_ON_UNDO,NULL,NULL,NULL,SDS_CURDWG);
			needregen=1;
			// Modified CyberAge PP 08/06/2000 [
			// Fix for bug no. 57437
			//if(cmd_layer_alter(fs2,70,1,NULL,&needregen)!=0) goto bail;
			cmd_layer_alter(fs2,70,1,NULL,&needregen);
			continue ;
			// Modified CyberAge PP 08/06/2000 ]
		} else if(strisame("THAW"/*DNT*/,fs1)) {
			if((ret=sds_getstring(1,ResourceString(IDC_LAYER__NLAYER_S__TO_THA_16, "\nLayer(s) to Thaw: " ),fs2))==RTERROR) {
				rc=(-__LINE__);
				goto bail;
			} else if(ret==RTCAN) {
				continue;
			}
			ic_trim(fs2,"be");
			if(needregen==0)
				SDS_SetUndo(IC_UNDO_REGEN_ON_UNDO,NULL,NULL,NULL,SDS_CURDWG);
			needregen=1;
			// Modified CyberAge PP 08/06/2000 [
			// Fix for bug no. 57437
			// if(cmd_layer_alter(fs2,70,-1,NULL,&needregen)!=0) goto bail;
			cmd_layer_alter(fs2,70,-1,NULL,&needregen);
			continue ;
			// Modified CyberAge PP 08/06/2000 ]
		} else if(strisame("LOCK"/*DNT*/,fs1)) {
			if((ret=sds_getstring(1,ResourceString(IDC_LAYER__NLAYER_S__TO_LOC_17, "\nLayer(s) to Lock: " ),fs2))==RTERROR) {
				rc=(-__LINE__);
				goto bail;
			} else if(ret==RTCAN) {
				continue;
			}
			ic_trim(fs2,"be");
			// Modified CyberAge PP 08/06/2000 [
			// Fix for bug no. 57437
			// if(cmd_layer_alter(fs2,70,4,NULL,&needregen)!=0) goto bail;
			cmd_layer_alter(fs2,70,4,NULL,&needregen);
			continue;
			// Modified CyberAge PP 08/06/2000 ]
		} else if(strisame("UNLOCK"/*DNT*/,fs1)) {
			if((ret=sds_getstring(1,ResourceString(IDC_LAYER__NLAYER_S__TO_UNL_18, "\nLayer(s) to Unlock: " ),fs2))==RTERROR) {
				rc=(-__LINE__);
				goto bail;
			} else if(ret==RTCAN) {
				continue;
			}
			ic_trim(fs2,"be");
			// Modified CyberAge PP 08/06/2000 [
			// Fix for bug no. 57437
			//if(cmd_layer_alter(fs2,70,-4,NULL,&needregen)!=0) goto bail;
			ic_trim(fs2,"be");
			cmd_layer_alter(fs2,70,-4,NULL,&needregen);
			continue;
			// Modified CyberAge PP 08/06/2000 ]
		}
	}

	//*** Set the curent layer name.

	bail:

	if(needregen) {
		cmd_regenall();
		SDS_SetUndo(IC_UNDO_REGEN_ON_REDO,NULL,NULL,NULL,SDS_CURDWG);
	}

	if(rc==0L) return(RTNORM);
	if(rc==(-1L)) return(RTCAN);
	if(rc<(-1L)) CMD_INTERNAL_MSG(rc);
	return(RTERROR);
}

short cmd_layer_check(char *layerlst){
	char fs2[IC_ACADBUF],*fcp1=NULL;
	struct resbuf *tlist=NULL;
	short gotit,ret,foundone=0;

	//check for null
	if(layerlst==NULL) { ret=RTERROR; goto bail; }
	//setup first layer in list
   	strncpy(fs2,layerlst,sizeof(fs2)-1);
	if((fcp1=strchr(fs2,','/*DNT*/))!=NULL){
		*fcp1=0;
		fcp1++;
	}

	while(*fs2){
		//loop through the layer table
		for(tlist=sds_tblnext("LAYER"/*DNT*/,1),gotit=0; tlist!=NULL; tlist=sds_tblnext("LAYER"/*DNT*/,0)) {
			if(ic_assoc(tlist,2)!=0) {ret=RTERROR; goto bail; }
			if(sds_wcmatch(strupr(ic_rbassoc->resval.rstring),fs2)==RTNORM){
				if(ic_assoc(tlist,70)!=0) {ret=RTERROR; goto bail; }
//				if(0==(ic_rbassoc->resval.rint & IC_ENTRY_XREF_DEPENDENT)){
					foundone=gotit=1;
//				}
			}
			IC_RELRB(tlist);
		}
		if(!gotit) {
			sds_printf(ResourceString(IDC_LAYER__NNO_LAYERS_FOUND_19, "\nNo layers found matching pattern %s. " ),fs2);
		}
 		if(fcp1==NULL){
			*fs2=0;
		}else{
			strncpy(fs2,fcp1,sizeof(fs2)-1);
			if((fcp1=strchr(fs2,','/*DNT*/))!=NULL){
				*fcp1=0;
				fcp1++;
			}
		}
	}

	ret=RTNORM;
	if (!foundone) ret=RTERROR;
bail:
	return(ret);

}


long cmd_layer_alter(char *layerlst, short group, short value, char *valstr,int *needregen) {

	//*** This function alters the properties of the layers specified in "laylst".
	//***
	//*** Property	 |	group	|		 value		  | 	 ltype
	//*** -----------|----------|---------------------|-----------------
	//***  On		 |	 62 	|		  0 		  | 	  ON
	//***  Off		 |	 62 	|		  0 		  | 	  OFF
	//***  Color	 |	 62 	|	new color number  | 	  NULL
	//***  Linetype  |	 6		|		  0 		  |   new linetype
	//***  Freeze	 |	 70 	|		  1 		  | 	  NULL
	//***  Thaw 	 |	 70 	|		 -1 		  | 	  NULL
	//***  Lock 	 |	 70 	|		  4 		  | 	  NULL
	//***  Unlock	 |	 70 	|		 -4 		  | 	  NULL
	//***  LWeight	 |	 370 	|	new lweight num.  | 	  NULL
	//***  Plot/ON	 |	 290 	|		  1 		  | 	  NULL
	//***  Plot/OFF	 |	 290 	|		  0 		  | 	  NULL
	//***
	//NOTE:  If you try to turn of the current layer and the EXPERT var is
	//	==0, we must warn the user
	//EBATECH(CNBR) Add LWeight, PlotON and OFF Property for A2K Compatibility

	char fs1[IC_ACADBUF],fs2[IC_ACADBUF],laylst[IC_ACADBUF],
			curlay[IC_ACADNMLEN],tempstr[75],tempstr2[25],*fcp1;
	struct resbuf setgetrb,*tlist=NULL,*tmprb;
	int ret,expert,fi1;
	bool inuse,frozen;
	long rc=0L;

	if(layerlst==NULL) { rc=(-__LINE__); goto bail; }
	strncpy(laylst,layerlst,sizeof(laylst)-1);
	strupr(laylst);
	if(SDS_getvar(NULL,DB_QCLAYER,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-__LINE__); goto bail; }
	strncpy(curlay,setgetrb.resval.rstring,sizeof(curlay)-1);
	IC_FREE(setgetrb.resval.rstring);
	if(SDS_getvar(NULL,DB_QEXPERT,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-__LINE__); goto bail; }
	expert=setgetrb.resval.rint;

	if(cmd_layer_check(laylst)!=RTNORM) { rc=(-__LINE__); goto bail; }

	for(tlist=sds_tblnext("LAYER"/*DNT*/,1); tlist!=NULL; tlist=sds_tblnext("LAYER"/*DNT*/,0)) {
		if(ic_assoc(tlist,group)!=0){
			IC_RELRB(tlist);
			continue;
		}
		tmprb=ic_rbassoc;
		//for each layer in the table having the resbuf we want...
		if(ic_assoc(tlist,70)!=0) {
			rc=(-__LINE__);
			goto bail;
		}
		if((ic_rbassoc->resval.rint & IC_ENTRY_XREF_DEPENDENT) && (value==4 || value==-4)) // can't lock xdep layer
			continue;//xref dependent layer
		inuse=(ic_rbassoc->resval.rint & IC_REFERENCED) ? 1 : 0;
		frozen=(ic_rbassoc->resval.rint & (IC_LAYER_FROZEN | IC_LAYER_VPFROZEN)) ? 1 : 0;
		if(ic_assoc(tlist,2)!=0) {
			rc=(-__LINE__);
			goto bail;
		}
		strncpy(fs1,ic_rbassoc->resval.rstring,sizeof(fs1)-1);
		strupr(fs1);
		strncpy(fs2,laylst,sizeof(fs2)-1);
		if((fcp1=strchr(fs2,','/*DNT*/))!=NULL){
			*fcp1=0;
			fcp1++;
		}

		//see if any of the tokens in laylst match it
		while(*fs2){
			if(RTNORM!=sds_wcmatch(fs1,fs2))goto nexttok;
			switch(group) {
				case 70:
					if(value==1) {
						if(strsame(fs1,curlay)) {
						   sds_printf(ResourceString(IDC_LAYER__NLAYER__S_IS_THE_20, "\nLayer %s is the CURRENT layer. It cannot be frozen. " ),curlay);
							goto nexttok;
						}
						fi1=tmprb->resval.rint|((short)IC_LAYER_FROZEN);
					} else if(value==(-1)) {
						fi1=tmprb->resval.rint&(~((short)IC_LAYER_FROZEN));
					} else if(value==(4)) {
						fi1=tmprb->resval.rint|((short)IC_LAYER_LOCKED);
					} else if(value==(-4)) {
						fi1=tmprb->resval.rint&(~((short)IC_LAYER_LOCKED));
					}
					if(fi1!=tmprb->resval.rint) {
						tmprb->resval.rint=fi1;
						if(inuse) {
							if((*needregen)==0) SDS_SetUndo(IC_UNDO_REGEN_ON_UNDO,NULL,NULL,NULL,SDS_CURDWG);
							*needregen=1;
						}
					}
					break;
				case 62:
					if(valstr==NULL) {						// Change color
						if(tmprb->resval.rint!=value && inuse) {
							if((*needregen)==0) SDS_SetUndo(IC_UNDO_REGEN_ON_UNDO,NULL,NULL,NULL,SDS_CURDWG);
							*needregen=1;
						}
						tmprb->resval.rint=value;
					} else {
						if(strisame("ON"/*DNT*/,valstr)) { // Turn layer ON
							if(tmprb->resval.rint<0) {
								if(inuse) {
									if((*needregen)==0) SDS_SetUndo(IC_UNDO_REGEN_ON_UNDO,NULL,NULL,NULL,SDS_CURDWG);
									*needregen=1;
								}
								tmprb->resval.rint=(-tmprb->resval.rint);
							}
							if(frozen){
								sds_printf(ResourceString(IDC_LAYER__NTHE_LAYER__S_IS_21, "\nThe layer %s is frozen and will not display until thawed. " ),fs1);
							}
						} else if(strisame("OFF"/*DNT*/,valstr)) { // Turn layer OFF
							if(strisame(fs1,curlay) && expert==0) {
							   sprintf(tempstr,ResourceString(IDC_LAYER__N_S_IS_THE_CURRE_22, "\n%s is the CURRENT layer.  Turn it off anyway? <N>: " ),curlay);
								sds_initget(0,ResourceString(IDC_LAYER_INITGET_YES_TURN__23, "Yes-Turn_current_layer_off|Yes No-Leave_current_layer_on|No ~_Yes ~_No" ));
								ret=sds_getkword(tempstr,tempstr2);
								if(ret==RTCAN || ret==RTERROR) {
									rc=ret; goto bail;
								} else if(ret==RTNONE) {
									goto nexttok;
								} else if(ret==RTNORM) {
									if(strisame(tempstr2,"NO"/*DNT*/))
										goto nexttok;
								}
							}
							if(tmprb->resval.rint>=0) {
								if(inuse) {
									if((*needregen)==0) SDS_SetUndo(IC_UNDO_REGEN_ON_UNDO,NULL,NULL,NULL,SDS_CURDWG);
									*needregen=1;
								}
								tmprb->resval.rint=(-tmprb->resval.rint);
							}
						} else {
							rc=(-__LINE__); goto bail;
						}
					}
					break;
				case 6: //*** Change linetype
					if(!strsame(tmprb->resval.rstring,valstr) && inuse) {
						if((*needregen)==0) SDS_SetUndo(IC_UNDO_REGEN_ON_UNDO,NULL,NULL,NULL,SDS_CURDWG);
						*needregen=1;
					}
					if((tmprb->resval.rstring=ic_realloc_char(tmprb->resval.rstring,strlen(valstr)+1))==NULL) {
						cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
						rc=(-2); goto bail;
					}
					strcpy(tmprb->resval.rstring,valstr);
					break;
				// EBATECH(CNBR) -[ 24-03-2003 Bugzilla#78471 Support Lineweight
				case 370: //*** Change Lineweight
					if(tmprb->resval.rint!=value && inuse) {
						if((*needregen)==0) SDS_SetUndo(IC_UNDO_REGEN_ON_UNDO,NULL,NULL,NULL,SDS_CURDWG);
						*needregen=1;
					}
					tmprb->resval.rint=value;
					break;
				case 290: //*** Change Plot On/Off
					tmprb->resval.rint=value;
					break;
				// EBATECH(CNBR) ]-
			}
			SDS_tblmesser(tlist,IC_TBLMESSER_MODIFY,SDS_CURDWG);
nexttok:
			if(fcp1==NULL)
				*fs2=0;
			else{
				strncpy(fs2,fcp1,sizeof(fs2)-1);
				if((fcp1=strchr(fs2,','/*DNT*/))!=NULL){
					*fcp1=0;
					fcp1++;
				}
			}
		}
		if(tlist!=NULL){sds_relrb(tlist);tlist=NULL;}
	}

	bail:

	IC_RELRB(tlist);
	return(rc);
}

long cmd_layer_list(void) {
//*** This function lists defined layer information.
//***
//*** NOTE: This function does not list the layers in alphabetical order
//***		like AutoCAD does. ~|
//***
	char fs1[IC_ACADBUF],laystr[IC_ACADBUF],*fcp1,*fcp2,fc1,lstate[13],curlay[IC_ACADNMLEN];
	char scrollines[IC_ACADBUF];
	struct resbuf *tmprb=NULL,*beglaylst=NULL,*curlaylst=NULL,*tmplaylst,setgetrb;
	int ret,listcur=0,scroll=0;
	short numlines=0,cecolor,fi1;
	long rc=0L;

	if((ret=sds_getstring(0,ResourceString(IDC_LAYER__NLAYER_S__TO_LIS_24, "\nLayer(s) to list <*>: " ),laystr))==RTERROR) {
		rc=(-__LINE__); goto bail;
	} else if(ret==RTCAN) {
		rc=(-1L); goto bail;
	}
	if(SDS_getvar(NULL,DB_QCLAYER,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM){rc=(-__LINE__); goto bail;}
	strncpy(curlay,setgetrb.resval.rstring,sizeof(curlay)-1);
	IC_FREE(setgetrb.resval.rstring);
	setgetrb.resval.rstring=NULL;

	strupr(laystr); sds_textscr();
	sds_printf(ResourceString(IDC_LAYER__N__LAYER_NAME____25, "\n  Layer name                       State       Linetype    Color " ));
	sds_printf(ResourceString(IDC_LAYER__N________________26, "\n------------------------------- ------------ ------------ -------" ));
	//*** ~| TODO - Some day I need to redo the way this is being done with a call to sds_wcmatch().
	if(!*laystr || (laystr[0]=='*'/*DNT*/ && laystr[1]==0)) {
		if((beglaylst=curlaylst=sds_buildlist(RTSTR,"*"/*DNT*/,0))==NULL) {
			cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
			rc=(-__LINE__); goto bail;
		}
	} else {
		for(fcp1=fcp2=laystr; *fcp2!=0; fcp1=fcp2+1) {
			for(fcp2=fcp1; *fcp2!=',' && *fcp2!=0; fcp2++);
			fc1=*fcp2;
			*fcp2=0;
			if((tmplaylst=sds_buildlist(RTSTR,fcp1,0))==NULL) {
				cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
				rc=(-__LINE__); goto bail;
			}
			if(beglaylst==NULL) {
				beglaylst=curlaylst=tmplaylst;
			} else {
				curlaylst=curlaylst->rbnext=tmplaylst;
			}
			*fcp2=fc1;
		}
	}
	if((tmprb=sds_tblnext("LAYER"/*DNT*/,1))==NULL) {
	   rc=(-__LINE__);
	   goto bail;
	}
	while(tmprb!=NULL) {
		if(sds_usrbrk()) {
		   rc=(-1L);
		   goto bail;
		}
		//*** Layer names.
		if(ic_assoc(tmprb,2)==0) {
			for(tmplaylst=beglaylst; tmplaylst!=NULL; tmplaylst=tmplaylst->rbnext) {
				if(sds_wcmatch(ic_rbassoc->resval.rstring,tmplaylst->resval.rstring)==RTNORM) {
					strncpy(fs1,ic_rbassoc->resval.rstring,sizeof(fs1)-1);
					//*** Get properties of the layer.
					if(ic_assoc(tmprb,62)!=0) { rc=(-__LINE__); goto bail; }
					cecolor=ic_rbassoc->resval.rint;
					if(ic_assoc(tmprb,70)!=0) { rc=(-__LINE__); goto bail; }
					fi1=ic_rbassoc->resval.rint;
					if(cecolor<0)
						strcpy(lstate,ResourceString(IDC_LAYER_OFF_27, "Off" ));
					else
						strcpy(lstate,ResourceString(IDC_LAYER_ON__28, "On " ));
					if(fi1&IC_LAYER_FROZEN)
					   strcat(lstate,ResourceString(IDC_LAYER__FRZ_29, " Frz" ));
					else
					   strcat(lstate,ResourceString(IDC_LAYER__THW_30, " Thw" ));
					if(fi1&IC_LAYER_LOCKED)
						strcat(lstate,ResourceString(IDC_LAYER__LCK_31, " Lck" ));
					else
						strcat(lstate,ResourceString(IDC_LAYER__UNL_32, " Unl" ));

					// TODO:TODO: Add 290:Plotable 370:Lweight here !!

					if(strisame(curlay,fs1)){
					   strcat(lstate,"*"/*DNT*/);
						listcur=1;	//current layer among those listed..
					}
					if(ic_assoc(tmprb,6)!=0) {
					   rc=(-__LINE__);
					   goto bail;
					}
					/*DG 13.5.2003*/// Do support long names (up to IC_ACADNMLEN = 256 chars), but don't align the output to such long values;
					// use the length of 'Layer name' column in IDC_LAYER__N________________26 string.
					// For names longer than this value print the name on the 1st line and layer attributes on the next one.
					const size_t shortLayerNameLen = 31;
					size_t	lnameLen;
					lnameLen = _tcsclen(fs1);
					if(lnameLen < shortLayerNameLen)
						sds_printf(ResourceString(IDC_LAYER__N__31_31S___12_1_33, "\n%-*.*s %-12.12s %-12.12s %-7.7s" ),
							   shortLayerNameLen, shortLayerNameLen, fs1, lstate, ic_rbassoc->resval.rstring, strupr(ic_colorstr(cecolor, NULL)));
					else
					{
						sds_printf(ResourceString(IDC_LAYER__N__31_31S___12_1_33, "\n%-*.*s %-12.12s %-12.12s %-7.7s" ),
							   lnameLen, lnameLen, fs1, ""/*DNT*/, ""/*DNT*/, ""/*DNT*/);
						sds_printf(ResourceString(IDC_LAYER__N__31_31S___12_1_33, "\n%-*.*s %-12.12s %-12.12s %-7.7s" ),
							   shortLayerNameLen, shortLayerNameLen, ""/*DNT*/, lstate, ic_rbassoc->resval.rstring, strupr(ic_colorstr(cecolor, NULL)));
					}
					numlines++;
					if(numlines>=20 && !scroll) {
						if(listcur){
							listcur=0;
							sds_printf(ResourceString(IDC_LAYER__N___IN_STATE_FIE_34, "\n * in State field indicates CURRENT layer." ));
						}
						sds_initget(0,ResourceString(IDC_LAYER_INITGET_SCROLL____35, "Scroll ~_Scroll" ));
						if((ret=sds_getkword(ResourceString(IDC_LAYER__NSCROLL__ENTER_T_36, "\nScroll/<ENTER to view next screen>:" ),scrollines))==RTERROR) {
							rc=(-__LINE__);
							goto bail;
						}else if(ret==RTCAN) {
							rc=(-1L);
							goto bail;
						}else if(ret==RTNONE) {
							numlines=0;
						}else if(ret==RTNORM) {
							scroll=1;
						}
					}
				}
			}
		}
		IC_RELRB(tmprb);
		//*** Do NOT check for NULL here.  NULL is checked in the while().
		tmprb=sds_tblnext("LAYER"/*DNT*/,0);
	}
	if(listcur){
		listcur=0;
		sds_printf(ResourceString(IDC_LAYER__N___IN_STATE_FIE_34, "\n * in State field indicates CURRENT layer." ));
	}
	bail:

	IC_RELRB(tmprb);
	IC_RELRB(beglaylst);
	return(rc);
}

// EBATECH(CNBR) -[ 24-03-2003 Bugzilla#78471 Support Lineweight
short cmd_lweightlister(int flag) {

	sds_textscr();
	sds_printf(ResourceString(IDC_LAYER__LWEIGHT_LIST_TTL_40, "\nAvailable Lineweights (=1/100 mm): "));

	if( flag == DB_DFWMASK )
		sds_printf(ResourceString(IDC_LAYER__LWEIGHT_LIST_ITM_45, "\nDefault"));
	else
		sds_printf(ResourceString(IDC_LAYER__LWEIGHT_LIST_ITM_44, "\nByLayer ByBlock  Default"));

	sds_printf(ResourceString(IDC_LAYER__LWEIGHT_LIST_ITM_41, "\n  0    5    9   13   15   18   20   25"));
	sds_printf(ResourceString(IDC_LAYER__LWEIGHT_LIST_ITM_42, "\n 30   35   40   50   53   60   70   80"));
	sds_printf(ResourceString(IDC_LAYER__LWEIGHT_LIST_ITM_43, "\n 90  100  106  120  140  158  200  211"));

	return(RTNORM);
}
// EBATECH(CNBR) ]-
