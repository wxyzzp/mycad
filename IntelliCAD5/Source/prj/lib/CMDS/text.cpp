/* TEXT.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * Supports the text commands
 *
 */
// THIS FILE HAS BEEN GLOBALIZED!
//** Includes
#include "cmds.h"/*DNT*/
#include "Icad.h" /*DNT*/
#include "IcadDoc.h"
#include "sdsapplication.h" /*DNT*/
#include "threadcontroller.h" /*DNT*/
#include "commandqueue.h" /*DNT*/
#include "styletabrec.h"
#include "IcadView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define CMD_ATTDEFMODE		0  //*** These three defines are used in cmd_textdtext();
#define CMD_TEXTMODE		1
#define CMD_DTEXTMODE		2  //***

// **********
// GLOBALS
//
extern int	  SDS_IgnoreMenuWQuote;
//BugZilla No. 78142; 11-11-2002 
extern char	  SDS_cmdcharbuf[IC_ACADBUF];

long  cmd_style_list(void);

// sds_getangle() returns angle which is got ignore setting of ANGBASE. This useful function 
// is used after sds_getangle() function and returns inputed angle in WCSA (ANGBASE is taken into account) 
static sds_real getWCSAngle(sds_real ang)
{
	sds_real result;
	resbuf rb;
	ic_normang(&ang, NULL);

	sds_real angBase = SDS_getvar(NULL, DB_QANGBASE, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES) 
						== RTNORM ? rb.resval.rreal : 0.0; // in WCSA
	
	if(SDS_getvar(NULL, DB_QANGDIR, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES) == RTNORM ? rb.resval.rint : 0)
		result = angBase - (IC_TWOPI - ang);
	else
		result = ang + angBase;

	ic_normang(&result, NULL);
	return result;
}


/* *******************
 * HELPER FUNCTION
 */

static short
cmd_textdtext(short mode) {
//*** mode: CMD_TEXTMODE - For TEXT - This function creates a single line
//***					   of text.
//***		CMD_DTEXTMODE - For DTEXT - This function displays text on
//***						screen as it is entered.
//***		CMD_ATTDEFMODE - For ATTDEF - This function displays text on
//***						 screen as it is entered.
//***
//*** NOTE: This function gets called from cmd_text() and cmd_dtext()
//***		They both do the same thing right now, the dtext functionality
//***		still needs to be added.  Also, the FIT and ALIGN justifications need
//***		work.  And, if text was the last command, AutoCAD doesn't prompt
//***		the user for height and rotation if ENTER is pressed at the "Start
//***		point" prompt - ours doesn't do this (yet). ~|
//***		The angle kept in ->lastang is REALATIVE TO CURRENT UCS.  It has to be
//***		converted to an ecs angle before doing the entmake, because that's where
//***		it is measured
//***
//*** RETURNS:	RTNORM - Success.
//***			 RTCAN - User cancel.
//***		   RTERROR - RTERROR returned from an ads function.
//***
	double TEXTINC=1.666666; //this is the equivlent of a local define, we did't need a global.
							 //this should be read out of the font file.
	
	BOOL bSaveTextAngle=TRUE;
	struct SDS_textdtext_globalpacket *gpak;
	char fs1[IC_ACADBUF],fs2[IC_ACADBUF],atttag[IC_ACADBUF],attpmt[IC_ACADBUF],
		 attdflt[IC_ACADBUF],stylstr[IC_ACADNMLEN],oldqtext;
	struct resbuf *newelist=NULL,*tmprb=NULL;
	struct resbuf setgetrb,rbucs,rbecs;
	db_handitem *elp=NULL;
	sds_name ename;
	sds_point pt1,pt2,pt3,pt4,pt5,pt6,pt7;
	sds_real oblqang,widthfact,lastangle,adjlastangle,baseang4ecs,lastheight;
	int ret,realign,aflags,hgtflg=1,rotflg=1,horizalgn=0,vertalgn=0,justify=0,tgflags,tsflags,algnfit=0;
	short vert;
	long rc=0L;
	struct gr_view *view;
	db_handitem *telp1=NULL;

	//set up resbufs needed for point transformations in creating entities
	if(SDS_getvar(NULL,DB_QUCSXDIR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) return(-2);
	ic_ptcpy(pt1,setgetrb.resval.rpoint);
	if(SDS_getvar(NULL,DB_QUCSYDIR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) return(-2);
	ic_ptcpy(pt2,setgetrb.resval.rpoint);
	rbucs.rbnext=rbecs.rbnext=NULL;
	rbucs.restype=RTSHORT;
	rbecs.restype=RT3DPOINT;
	rbucs.resval.rint=1;
	ic_crossproduct(pt1,pt2,rbecs.resval.rpoint);

	sds_entmake(NULL);

	pt1[0]=1.0;
	pt1[1]=pt1[2]=0.0;
	sds_trans(pt1,&rbucs,&rbecs,1,pt1);
	baseang4ecs=atan2(pt1[1],pt1[0]);

	if(SDS_getvar(NULL,DB_QQTEXTMODE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) return(-2);
	oldqtext=(char)setgetrb.resval.rint;
	setgetrb.resval.rint=0;
	//it's not enough to set the var off - you have to repopulate the viewing structure so the
	//	new text is created w/the new setting.
	sds_setvar("QTEXTMODE"/*DNT*/,&setgetrb);
	view=SDS_CURGRVW;
	view->qtextmode=0;

	CIcadDoc	*currentDoc = SDS_CURDOC;
	if(currentDoc==NULL)
		return(RTERROR);

	if(mode==CMD_ATTDEFMODE)
		gpak=currentDoc->m_pGpakatt;
	else
		gpak=currentDoc->m_pGpaktxt;

	realign=0;

	*stylstr=0;
	if(mode==CMD_ATTDEFMODE) {
		//*** Get default attribute mode.
		if(SDS_getvar(NULL,DB_QAFLAGS,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-__LINE__); goto bail; }
		aflags=setgetrb.resval.rint;
		//*** Get attribute modes.
		for(;;) {		//	PROceed is used below instead of PRoceed to eliminate any chance that someone (a program?)
						//	would enter PR expecting Preset and get PRoceed instead.
			LOAD_COMMAND_OPTIONS_2(IDC_TEXT_INITGET__CONSTANT_T_0)
			if(sds_initget(SDS_RSG_NOCANCEL, LOADEDSTRING) != RTNORM) {
				rc=(-__LINE__);
				goto bail;
			}
			sprintf(fs2,ResourceString(IDC_TEXT__NTOGGLE_ATTRIBUTE__1, "\nToggle attribute modes:  Fixed: %c/Hidden: %c/Defined: %c/Validate: %c /<PROceed>: " ),
#ifdef LOCALISE_ISSUE
// Y N should be Yes and no & resourced. Won't work for Language versions.
#endif
				(aflags & IC_ATTDEF_CONST) ? 'Y':'N',(aflags & IC_ATTDEF_INVIS) ? 'Y':'N',(aflags & IC_ATTDEF_PRESET) ? 'Y':'N',(aflags & IC_ATTDEF_VERIFY) ? 'Y':'N');
			if((ret=sds_getkword(fs2,fs1))==RTERROR) {
				rc=(-__LINE__); goto bail;
			} else if(ret==RTNONE) {
				break;
			} else if(ret==RTCAN) {
				rc=(-1L); goto bail;
			}
			if(!*fs1) break;
			if		  (strisame("PROCEED"/*DNT*/,fs1)) break;
			else if (strisame("CONSTANT"/*DNT*/,fs1)   || strisame("FIXED"/*DNT*/,fs1))
					  aflags^=IC_ATTDEF_CONST;
			else if (strisame("INVISIBLE"/*DNT*/,fs1)  || strisame("HIDDEN"/*DNT*/,fs1))
					  aflags^=IC_ATTDEF_INVIS;
			else if (strisame("PRESET"/*DNT*/,fs1)	   || strisame("DEFINED"/*DNT*/,fs1))
					  aflags^=IC_ATTDEF_PRESET;
			else if (strisame("VERIFY"/*DNT*/,fs1)	   || strisame("VALIDATE"/*DNT*/,fs1))
					  aflags^=IC_ATTDEF_VERIFY;
			else {
				cmd_ErrorPrompt(CMD_ERR_KWORD,0);
				continue;
			}
			//*** Set AFLAGS system variable.
			setgetrb.restype=RTSHORT;
			setgetrb.resval.rint=aflags;
			if(sds_setvar("AFLAGS"/*DNT*/,&setgetrb)!=RTNORM) { rc=(-__LINE__); goto bail; }
		}
		//*** Get the name of the attribute (formerly "tag").
		for(;;) {
			if((ret=sds_getstring(0,ResourceString(IDC_TEXT__NNAME_OF_ATTRIBUT_11, "\nName of attribute: " ),atttag))==RTERROR) {
				rc=(-__LINE__); goto bail;
			} else if(ret==RTCAN) {
				rc=(-1L); goto bail;
			}
			if(!*atttag) {
				cmd_ErrorPrompt(CMD_ERR_NULLTAG,0);
				continue;
			}
			strupr(atttag);
			break;
		}
		SDS_IgnoreMenuWQuote=1;
		if(aflags & IC_ATTDEF_CONST) { //*** If Constant
			//*** There is no attribute prompt for a constant.
			*attpmt=0;
			//*** Get text for constant attribute.
			if((ret=sds_getstring(1,ResourceString(IDC_TEXT__NTEXT___12, "\nText: " ),attdflt))==RTERROR) {
				rc=(-__LINE__); goto bail;
			} else if(ret==RTCAN) {
				rc=(-1L); goto bail;
			}
		} else {
			//*** Get the attribute prompt.
			if((ret=sds_getstring(1,ResourceString(IDC_TEXT__NPROMPT___13, "\nPrompt: " ),attpmt))==RTERROR) {
				rc=(-__LINE__); goto bail;
			} else if(ret==RTCAN) {
				rc=(-1L); goto bail;
			}
			if(!*attpmt) strncpy(attpmt,atttag,sizeof(attpmt)-1);
			//*** Get the default text for the attribute.
			if((ret=sds_getstring(1,ResourceString(IDC_TEXT__NDEFAULT_TEXT___14, "\nDefault text: " ),attdflt))==RTERROR) {
				rc=(-__LINE__); goto bail;
			} else if(ret==RTCAN) {
				rc=(-1L); goto bail;
			}
		}
		SDS_IgnoreMenuWQuote=0;
	}
	for(;;) {

		//*** Get the text style information.
		if(!*stylstr) {
			if(SDS_getvar(NULL,DB_QTEXTSTYLE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-__LINE__); goto bail; }
			strncpy(stylstr,setgetrb.resval.rstring,sizeof(stylstr)-1);
			IC_FREE(setgetrb.resval.rstring);
		}
	   if((tmprb=sds_tblsearch("STYLE"/*DNT*/,stylstr,0))==NULL) { rc=(-__LINE__); goto bail; }
		if(ic_assoc(tmprb,40)!=0) { IC_RELRB(tmprb); rc=(-__LINE__); goto bail; }
		if(!icadRealEqual(ic_rbassoc->resval.rreal,0.0)) {
			//*** If the text style has a fixed height, don't prompt for a height.
			gpak->txtheight=ic_rbassoc->resval.rreal;
			hgtflg=0;
		}else{
			if(SDS_getvar(NULL,DB_QTEXTSIZE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-__LINE__); goto bail; }
			gpak->txtheight=setgetrb.resval.rreal;
		}
		if(ic_assoc(tmprb,41)!=0) { IC_RELRB(tmprb); rc=(-__LINE__); goto bail; }
		widthfact=ic_rbassoc->resval.rreal; //*** Oblique angle.
		if(ic_assoc(tmprb,50)!=0) { IC_RELRB(tmprb); rc=(-__LINE__); goto bail; }
		oblqang=ic_rbassoc->resval.rreal; //*** Oblique angle.
		if(ic_assoc(tmprb,71)!=0) { IC_RELRB(tmprb); rc=(-__LINE__); goto bail; }
		tgflags=ic_rbassoc->resval.rint;  //*** Text-generation flags.
		if(ic_assoc(tmprb,70)!=0) { IC_RELRB(tmprb); rc=(-__LINE__); goto bail; }
		tsflags=ic_rbassoc->resval.rint;  //*** Text style table flags.
		vert=(tsflags&4); // Check to see if the text is vertical.
		if(vert){
			lastangle=IC_PI*1.5;
		}else{
			if(SDS_getvar(NULL,DB_QTEXTANGLE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-__LINE__); goto bail; }
			lastangle=gpak->lastang=setgetrb.resval.rreal;
		}
		IC_RELRB(tmprb);
		if(gpak->isnextpt){
			telp1=(db_handitem *)gpak->lastent[0];
			if(telp1 && !telp1->ret_deleted()) sds_redraw(gpak->lastent,IC_REDRAW_HILITE);//highlight last text
		}
		if(vert) {
			if(sds_initget(0,ResourceString(IDC_TEXT_INITGET_USE_DEFINE_15, "Use_defined_Style|Style ~ Align Center Middle Right ~_Style ~_ ~_Align ~_Center ~_Middle ~_Right" ))!=RTNORM) {
				rc=(-__LINE__);
				goto bail;
			}
			ret=internalGetpoint(NULL,ResourceString(IDC_TEXT__NTEXT__STYLE_ALIG_16, "\nText: Style/Align/Center/Middle/Right/<Start point>: " ),pt1);
		} else {
			if(justify) {
				LOAD_COMMAND_OPTIONS_3(IDC_TEXT_INITGET_USE_DEFINE_17)
				if(sds_initget(0, LOADEDSTRING) != RTNORM) {
					rc=(-__LINE__);
					goto bail;
				}
				ret=internalGetpoint(NULL,ResourceString(IDC_TEXT__NTEXT__STYLE_ALIG_18, "\nText: Style/Align/Fit/Center/Middle/Right/TL/TC/TR/ML/MC/MR/BL/BC/BR/<Start point>: " ),pt1);
			} else {
				LOAD_COMMAND_OPTIONS_3(IDC_TEXT_INITGET_USE_DEFINE_19)
				if(sds_initget(0, LOADEDSTRING) != RTNORM) {
					rc=(-__LINE__);
					goto bail;
				}
				ret=internalGetpoint(NULL,ResourceString(IDC_TEXT__NTEXT__STYLE_ALIG_20, "\nText: Style/Align/Fit/Center/Middle/Right/Justify/<Start point>: " ),pt1);
			}
		}
		if(gpak->isnextpt){
			//unhighlight last text
			if (telp1 && !telp1->ret_deleted()) sds_redraw(gpak->lastent,IC_REDRAW_UNHILITE);
		}
		if(ret==RTERROR) {
			rc=(-__LINE__); goto bail;
		} else if(ret==RTCAN) {
			rc=(-1L); goto bail;
		} else if(ret==RTNONE) {
			if(!gpak->isnextpt){
				cmd_ErrorPrompt(CMD_ERR_PTKWORD,0);
				continue;
			}
			ic_ptcpy(pt1,gpak->nextpt);
			horizalgn=(int)gpak->halign;
			vertalgn=(int)gpak->valign;
			if(5==horizalgn || 3==horizalgn){
				ic_ptcpy(pt2,gpak->nextfitpt);
				algnfit=1;
				if(3==horizalgn){
					sds_polar(pt1,(gpak->lastang-(IC_PI/2.0)),(!vert)?(gpak->txtheight*TEXTINC):(-gpak->txtheight*TEXTINC),pt5);
					sds_polar(pt2,(gpak->lastang-(IC_PI/2.0)),(!vert)?(gpak->txtheight*TEXTINC):(-gpak->txtheight*TEXTINC),pt6);
					sds_trans(pt5,&rbucs,&rbecs,0,pt5);
					sds_trans(pt6,&rbucs,&rbecs,0,pt6);
					realign=1;
				}
			}
			hgtflg=0; rotflg=0;
			break;
		} else if(ret==RTNORM) {
			break;
		} else if(ret==RTKWORD) {
			if(sds_getinput(fs1)!=RTNORM) { rc=(-__LINE__); goto bail; }
			if(strisame("JUSTIFY"/*DNT*/,fs1)) {
				justify=1; continue;
			} else if(strisame("STYLE"/*DNT*/,fs1)) {
				if(SDS_getvar(NULL,DB_QTEXTSTYLE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-__LINE__); goto bail; }
				sprintf(fs2,ResourceString(IDC_TEXT__N__TO_LIST_AVAILA_23, "\n Text style to use (or '?') <%s>: " ),setgetrb.resval.rstring);
				// EBATECH(CNBR) 2001-06-11 -[ Force string free
				strncpy(stylstr,setgetrb.resval.rstring,sizeof(stylstr)-1);
				IC_FREE(setgetrb.resval.rstring);
				//	EBATECH(CNBR) 2001-06-11 ]-
				for(;;) {
					sds_initget(RSG_OTHER,NULL);
					if((ret=sds_getstring(1,fs2,fs1))==RTERROR) {
						rc=(-__LINE__); goto bail;
					} else if(ret==RTCAN) {
						rc=(-1L); goto bail;
					}
					ic_trim(fs1,"be");
					strupr(fs1);
					// EBATECH(CNBR) 2001-06-11 -[ Force string free
					//if(!*fs1) {
					//	  strncpy(stylstr,setgetrb.resval.rstring,sizeof(stylstr)-1);
					//} else

					if(!*fs1){
						break;
					} else
					if(strsame("?"/*DNT*/,fs1)) {
						if((rc=cmd_style_list())!=RTNORM) goto bail;
					} else {
						if((tmprb=sds_tblsearch("STYLE"/*DNT*/,fs1,0))==NULL) {
							cmd_ErrorPrompt(CMD_ERR_FINDTEXT,0);
							//*stylstr=0; continue;
							continue;
						}
						IC_RELRB(tmprb);
						strncpy(stylstr,fs1,sizeof(stylstr)-1);
					}
					break;
					// EBATECH(CNBR) ]-
				}
				//*** Set the current text style
				setgetrb.restype=RTSTR;
				if((setgetrb.resval.rstring= new char [ strlen(stylstr)+1])==NULL) {
					cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
					rc=RTCAN; goto bail;
				}
				strcpy(setgetrb.resval.rstring,stylstr);
				if(sds_setvar("TEXTSTYLE"/*DNT*/,&setgetrb)!=RTNORM) { rc=(-__LINE__); goto bail; }
				IC_FREE(setgetrb.resval.rstring);
			} else if(strisame("ALIGN"/*DNT*/,fs1)) {
				while(1)
				{
					bSaveTextAngle=FALSE;
					if((ret=internalGetpoint(NULL,ResourceString(IDC_TEXT__NTEXT_START_POINT_25, "\nText start point: " ),pt1))==RTERROR) {
						rc=(-__LINE__); goto bail;
					} else if(ret==RTNONE) {
						cmd_ErrorPrompt(CMD_ERR_INVALIDPOINT,0);
						continue;
					} else if(ret==RTCAN) {
						rc=(-1L); goto bail;
					} else break;
				}

				while(1)
				{
					if((ret=internalGetpoint(pt1,ResourceString(IDC_TEXT__NTEXT_END_POINT___26, "\nText end point: " ),pt2))==RTERROR) {
						rc=(-__LINE__); goto bail;
					} else if(ret==RTNONE) {
						cmd_ErrorPrompt(CMD_ERR_INVALIDPOINT,0);
						continue;
					} else if(ret==RTCAN) {
						rc=(-1L); goto bail;
					}

					double nTemp=pt2[2];
					pt2[2]=pt1[2];
					if(icadPointEqual(pt1,pt2)) cmd_ErrorPrompt(CMD_ERR_NONZERODIST,0);
					else 
					{
						pt2[2]=nTemp;
						break; 
					}
				}

				horizalgn=3; vertalgn=0;
				hgtflg=0;
				rotflg=0;
				algnfit=1;
				lastangle=sds_angle(pt1,pt2);
				ic_ptcpy(pt5,pt1);
				ic_ptcpy(pt6,pt2);
				sds_trans(pt5,&rbucs,&rbecs,0,pt5);
				sds_trans(pt6,&rbucs,&rbecs,0,pt6);
				break;
			} else if (strisame("FIT"/*DNT*/,fs1)) {
				while(1)
				{
					bSaveTextAngle=FALSE;
					if((ret=internalGetpoint(NULL,ResourceString(IDC_TEXT__NTEXT_START_POINT_25, "\nText start point: " ),pt1))==RTERROR) {
						rc=(-__LINE__); goto bail;
					} else if(ret==RTNONE) {
						cmd_ErrorPrompt(CMD_ERR_INVALIDPOINT,0);
						continue;
					} else if(ret==RTCAN) {
						rc=(-1L); goto bail;
					} else break;
				}

				while(1)
				{
					if((ret=internalGetpoint(pt1,ResourceString(IDC_TEXT__NTEXT_END_POINT___26, "\nText end point: " ),pt2))==RTERROR) {
						rc=(-__LINE__); goto bail;
					} else if(ret==RTNONE) {
						cmd_ErrorPrompt(CMD_ERR_INVALIDPOINT,0);
						continue;
					} else if(ret==RTCAN) {
						rc=(-1L); goto bail;
					}
					double nTemp=pt2[2];
					pt2[2]=pt1[2];
					if(icadPointEqual(pt1,pt2)) cmd_ErrorPrompt(CMD_ERR_NONZERODIST,0);
					else 
					{
						pt2[2]=nTemp;
						break; 
					}
				}

				horizalgn=5; vertalgn=0;
				rotflg=0;
				algnfit=1;
				lastangle=sds_angle(pt1,pt2);
				break;
			} else if(strisame("CENTER"/*DNT*/,fs1)) {
				while(1)
				{
					if((ret=internalGetpoint(NULL,ResourceString(IDC_TEXT__NCENTER_POINT_OF__29, "\nCenter point of text: " ),pt1))==RTERROR) {
					rc=(-__LINE__); goto bail;
					} else if(ret==RTNONE) {
						cmd_ErrorPrompt(CMD_ERR_INVALIDPOINT,0);
						continue;
					} else if(ret==RTCAN) {
						rc=(-1L); goto bail;
					} else break;
				}

				horizalgn=1; vertalgn=0;
				break;
			} else if(strisame("MIDDLE"/*DNT*/,fs1)) {
				while(1)
				{
					if((ret=internalGetpoint(NULL,ResourceString(IDC_TEXT__NMIDDLE_POINT_OF__31, "\nMiddle point of text: " ),pt1))==RTERROR) {
					rc=(-__LINE__); goto bail;
					} else if(ret==RTNONE) {
						cmd_ErrorPrompt(CMD_ERR_INVALIDPOINT,0);
						continue;
					} else if(ret==RTCAN) {
						rc=(-1L); goto bail;
					} else break;
				}

				horizalgn=4; vertalgn=0;
				break;
			} else if(strisame("RIGHT"/*DNT*/,fs1)) {
				while(1)
				{
					if((ret=internalGetpoint(NULL,ResourceString(IDC_TEXT__NRIGHT_POINT_OF_T_33, "\nRight point of text: " ),pt1))==RTERROR) {
					rc=(-__LINE__); goto bail;
					} else if(ret==RTNONE) {
						cmd_ErrorPrompt(CMD_ERR_INVALIDPOINT,0);
						continue;
					} else if(ret==RTCAN) {
						rc=(-1L); goto bail;
					} else break;
				}

				horizalgn=2; vertalgn=0;
				break;
			} else if(strisame("TL"/*DNT*/,fs1)) {
				while(1)
				{
					if((ret=internalGetpoint(NULL,ResourceString(IDC_TEXT__NTOP_LEFT_POINT_O_35, "\nTop-left point of text: " ),pt1))==RTERROR) {
					rc=(-__LINE__); goto bail;
					} else if(ret==RTNONE) {
						cmd_ErrorPrompt(CMD_ERR_INVALIDPOINT,0);
						continue;
					} else if(ret==RTCAN) {
						rc=(-1L); goto bail;
					} else break;
				}

				horizalgn=0; vertalgn=3;
				break;
			} else if(strisame("TC"/*DNT*/,fs1)) {
				while(1)
				{
					if((ret=internalGetpoint(NULL,ResourceString(IDC_TEXT__NTOP_CENTER_POINT_37, "\nTop-center point of text: " ),pt1))==RTERROR) {
					rc=(-__LINE__); goto bail;
					} else if(ret==RTNONE) {
						cmd_ErrorPrompt(CMD_ERR_INVALIDPOINT,0);
						continue;
					} else if(ret==RTCAN) {
						rc=(-1L); goto bail;
					} else break;
				}

				horizalgn=1; vertalgn=3;
				break;
			} else if(strisame("TR"/*DNT*/,fs1)) {
				while(1)
				{
					if((ret=internalGetpoint(NULL,ResourceString(IDC_TEXT__NTOP_RIGHT_POINT__39, "\nTop-right point of text: " ),pt1))==RTERROR) {
					rc=(-__LINE__); goto bail;
					} else if(ret==RTNONE) {
						cmd_ErrorPrompt(CMD_ERR_INVALIDPOINT,0);
						continue;
					} else if(ret==RTCAN) {
						rc=(-1L); goto bail;
					} else break;
				}

				horizalgn=2; vertalgn=3;
				break;
			} else if(strisame("ML"/*DNT*/,fs1)) {
				while(1)
				{
					if((ret=internalGetpoint(NULL,ResourceString(IDC_TEXT__NMIDDLE_LEFT_POIN_41, "\nMiddle-left point of text: " ),pt1))==RTERROR) {
					rc=(-__LINE__); goto bail;
					} else if(ret==RTNONE) {
						cmd_ErrorPrompt(CMD_ERR_INVALIDPOINT,0);
						continue;
					} else if(ret==RTCAN) {
						rc=(-1L); goto bail;
					} else break;
				}

				horizalgn=0; vertalgn=2;
				break;
			} else if(strisame("MC"/*DNT*/,fs1)) {
				while(1)
				{
					if((ret=internalGetpoint(NULL,ResourceString(IDC_TEXT__NMIDDLE_CENTER_PO_43, "\nMiddle-center point of text: " ),pt1))==RTERROR) {
					rc=(-__LINE__); goto bail;
					} else if(ret==RTNONE) {
						cmd_ErrorPrompt(CMD_ERR_INVALIDPOINT,0);
						continue;
					} else if(ret==RTCAN) {
						rc=(-1L); goto bail;
					} else break;
				}

				horizalgn=1; vertalgn=2;
				break;
			} else if(strisame("MR"/*DNT*/,fs1)) {
				while(1)
				{
					if((ret=internalGetpoint(NULL,ResourceString(IDC_TEXT__NMIDDLE_RIGHT_POI_45, "\nMiddle-right point of text: " ),pt1))==RTERROR) {
					rc=(-__LINE__); goto bail;
					} else if(ret==RTNONE) {
						cmd_ErrorPrompt(CMD_ERR_INVALIDPOINT,0);
						continue;
					} else if(ret==RTCAN) {
						rc=(-1L); goto bail;
					} else break;
				}

				horizalgn=2; vertalgn=2;
				break;
			} else if(strisame("BL"/*DNT*/,fs1)) {
				while(1)
				{
					if((ret=internalGetpoint(NULL,ResourceString(IDC_TEXT__NBOTTOM_LEFT_POIN_47, "\nBottom-left point of text: " ),pt1))==RTERROR) {
					rc=(-__LINE__); goto bail;
					} else if(ret==RTNONE) {
						cmd_ErrorPrompt(CMD_ERR_INVALIDPOINT,0);
						continue;
					} else if(ret==RTCAN) {
						rc=(-1L); goto bail;
					} else break;
				}

				horizalgn=0; vertalgn=1;
				break;
			} else if(strisame("BC"/*DNT*/,fs1)) {
				while(1)
				{
					if((ret=internalGetpoint(NULL,ResourceString(IDC_TEXT__NBOTTOM_CENTER_PO_49, "\nBottom-center point of text: " ),pt1))==RTERROR) {
						rc=(-__LINE__); goto bail;
					} else if(ret==RTNONE) {
						cmd_ErrorPrompt(CMD_ERR_INVALIDPOINT,0);
						continue;
					} else if(ret==RTCAN) {
						rc=(-1L); goto bail;
					} else break;
				}

				horizalgn=1; vertalgn=1;
				break;
			} else if(strisame("BR"/*DNT*/,fs1)) {
				while(1)
				{
					if((ret=internalGetpoint(NULL,ResourceString(IDC_TEXT__NBOTTOM_RIGHT_POI_51, "\nBottom-right point of text: " ),pt1))==RTERROR) {
					rc=(-__LINE__); goto bail;
					} else if(ret==RTNONE) {
						cmd_ErrorPrompt(CMD_ERR_INVALIDPOINT,0);
						continue;
					} else if(ret==RTCAN) {
						rc=(-1L); goto bail;
					} else break;
				}

				horizalgn=2; vertalgn=1;
				break;
			}
		} else {
			cmd_ErrorPrompt(CMD_ERR_PTKWORD,0);
			continue;
		}
	}

	//*** Height.
	for(;hgtflg;) {
		//*** Get default text height.
//		  if(gpak->txtheight==0.0) {
//			  if(SDS_getvar(NULL,DB_QTEXTSIZE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-__LINE__); goto bail; }
//			  gpak->txtheight=setgetrb.resval.rreal;
//		  }
		if(sds_rtos(gpak->txtheight,-1,-1,fs2)!=RTNORM) { rc=(-__LINE__); goto bail; }
		sprintf(fs1,ResourceString(IDC_TEXT__NHEIGHT_OF_TEXT___52, "\nHeight of text <%s>: " ),fs2);
		if(sds_initget(RSG_NOZERO,NULL)!=RTNORM)
		   return(-2);
		if((ret=sds_getdist(pt1,fs1,&gpak->txtheight))==RTERROR) {
			return(-2);
		} else if(ret==RTCAN) {
			return(-1);
		} else if(ret==RTNORM || ret==RTNONE) {
			//Add by IntelliKorea for updating textsize. 2001/6/2
			struct resbuf rb;
			rb.restype=RTREAL;
			rb.resval.rreal = gpak->txtheight;
			sds_setvar("TEXTSIZE",&rb);
			//End of add. IntelliKorea 2001/6/1
			break;
		} else {
			cmd_ErrorPrompt(CMD_ERR_DIST2NDPT,0);
			continue;
		}
	}
	setgetrb.restype=RTREAL;
	setgetrb.resval.rreal=gpak->txtheight;
	sds_setvar("TEXTSIZE"/*DNT*/,&setgetrb);
//progesoft
	double angbase; 
	struct resbuf rb;
	SDS_getvar(NULL,DB_QANGBASE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	angbase = rb.resval.rreal;
	SDS_getvar(NULL,DB_QANGDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
//end progesoft
	//*** Rotation angle.
	for(;rotflg;) {
		//progesoft
		if(sds_angtos(lastangle,-1,-1,fs2)!=RTNORM) { rc=(-__LINE__); goto bail; }
		//if(sds_angtos(lastangle+angbase,-1,-1,fs2)!=RTNORM) { rc=(-__LINE__); goto bail; } // ACAD doesn't add BaseAngle in the prompt!
		//end progesoft
		//sprintf(fs1,"\nRotation angle of text <%s>: ",(mode!=CMD_TEXTMODE)?fs2:"0");
		sprintf(fs1,ResourceString(IDC_TEXT__NROTATION_ANGLE_O_53, "\nRotation angle of text <%s>: " ),fs2);
		if(sds_initget(0,NULL)!=RTNORM) {
		   rc=(-__LINE__);
		   goto bail;
		}
		if((ret=sds_getangle(pt1,fs1,&lastangle))==RTERROR) {
			rc=(-__LINE__); goto bail;
		} else if(ret==RTCAN) {
			rc=(-1L); goto bail;
		} else if(ret==RTNORM){
			lastangle = getWCSAngle(lastangle);
			break;
		} else if(ret==RTNONE) {
			//if(mode==CMD_TEXTMODE) lastangle=0.0;
			break;
		} else {
			cmd_ErrorPrompt(CMD_ERR_ANG2NDPT,0);
			continue;
		}
	}
	
	if(!vert && bSaveTextAngle)
	{
		setgetrb.restype=RTREAL;
		gpak->lastang=setgetrb.resval.rreal=lastangle;;
		sds_setvar("TEXTANGLE"/*DNT*/,&setgetrb);   // in 'WCA'
	}

	if(mode==CMD_ATTDEFMODE) {
		//*** Create entity.
		//pt3 will be insertion pt of text in WCS
		sds_trans(pt1,&rbucs,&rbecs,0,pt3);
		sds_trans(pt2,&rbucs,&rbecs,0,pt4);

		if(3==horizalgn || 5==horizalgn){	//for aligned or fitted text, we need 2 distinct pts
			newelist=sds_buildlist(RTDXF0,"ATTDEF"/*DNT*/,10,pt3,11,pt4,40,gpak->txtheight,41,widthfact,1,attdflt,3,attpmt,
			  2,atttag,70,aflags,50,lastangle+baseang4ecs,51,oblqang,7,stylstr,71,tgflags,72,horizalgn,74,vertalgn,
			  210,rbecs.resval.rpoint,0);
		}else if(horizalgn || vertalgn) {	//for other alignments, pass pt as alignment pt
			newelist=sds_buildlist(RTDXF0,"ATTDEF"/*DNT*/,10,pt3,11,pt3,40,gpak->txtheight,41,widthfact,1,attdflt,3,attpmt,
			  2,atttag,70,aflags,50,lastangle+baseang4ecs,51,oblqang,7,stylstr,71,tgflags,72,horizalgn,74,vertalgn,
			  210,rbecs.resval.rpoint,0);
		}else{								//else just pass point
			newelist=sds_buildlist(RTDXF0,"ATTDEF"/*DNT*/,10,pt3,40,gpak->txtheight,41,widthfact,1,attdflt,3,attpmt,
			  2,atttag,70,aflags,50,lastangle+baseang4ecs,51,oblqang,7,stylstr,71,tgflags,72,horizalgn,74,vertalgn,
			  210,rbecs.resval.rpoint,0);
		}

		if(newelist==NULL){
			cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
			rc=(-__LINE__); goto bail;
		}
		if(sds_entmake(newelist)!=RTNORM) {
			cmd_ErrorPrompt(CMD_ERR_CREATEATTDEF,0);
			rc=(-__LINE__); goto bail;
		}
		//*** Increment insertion point for next att entity
		if(3==horizalgn){
			//NOTE:  if there was aligned text, we don't know what the calculated height
			//	was.  Get the height of the last object in the drawing and move the text
			//	by an appropriate amount
			if(NULL==(tmprb=sds_entget(gpak->lastent))){
				setgetrb.resval.rreal=gpak->txtheight;
			}else{
				ic_assoc(tmprb,40);
				setgetrb.resval.rreal=ic_rbassoc->resval.rreal;
			}
			pt3[0]=pt3[1]=pt3[2]=0.0;
			sds_polar(pt3,(lastangle-(IC_PI/2.0)),(!vert)?(setgetrb.resval.rreal*TEXTINC):(-setgetrb.resval.rreal*TEXTINC),pt4);
			pt1[0]+=pt4[0];
			pt1[1]+=pt4[1];
			pt1[2]+=pt4[2];
			pt2[0]+=pt4[0];
			pt2[1]+=pt4[1];
			pt2[2]+=pt4[2];
			if(realign){
				//move the last entity to the correct location
				sds_trans(pt4,&rbucs,&rbecs,1,pt3);
				ic_assoc(tmprb,10);
				ic_rbassoc->resval.rpoint[0]+=pt3[0];
				ic_rbassoc->resval.rpoint[1]+=pt3[1];
				ic_rbassoc->resval.rpoint[2]+=pt3[2];
				ic_assoc(tmprb,11);
				ic_rbassoc->resval.rpoint[0]+=pt3[0];
				ic_rbassoc->resval.rpoint[1]+=pt3[1];
				ic_rbassoc->resval.rpoint[2]+=pt3[2];
				if(sds_entmod(tmprb)!=RTNORM){rc=RTERROR;goto bail;}
			}
			if(tmprb!=NULL){sds_relrb(tmprb);tmprb=NULL;}

			ic_ptcpy(gpak->nextpt,pt1);
			ic_ptcpy(gpak->nextfitpt,pt2);
		}else{
			//BugZilla No. 75557; 21-10-2002 
			if ( tgflags & 4 )
				sds_polar(pt1,(lastangle-(IC_PI/2.0)),(!vert && !(tgflags & 4))?(gpak->txtheight*TEXTINC):(-gpak->txtheight*TEXTINC),gpak->nextpt);
			else
			sds_polar(pt1,(lastangle-(IC_PI/2.0)),(!vert)?(gpak->txtheight*TEXTINC):(-gpak->txtheight*TEXTINC),gpak->nextpt);
			ic_ptcpy(pt1,gpak->nextpt);
			if(5==horizalgn){
				sds_polar(pt2,(lastangle-(IC_PI/2.0)),(!vert)?(gpak->txtheight*TEXTINC):(-gpak->txtheight*TEXTINC),gpak->nextfitpt);
				ic_ptcpy(pt2,gpak->nextfitpt);
			}
		}
		gpak->isnextpt=1;
		gpak->halign=(char)horizalgn;
		gpak->valign=(char)vertalgn;
		sds_entlast(ename);
		ic_encpy(gpak->lastent,ename);
	} else {
		if(mode==CMD_DTEXTMODE)
			{
			IDS_GetThreadController()->SetNoThreadSwap( true );
			}
// MR -- moved this outside the loop, added new var
		adjlastangle = lastangle;

		for(;;realign++) { //*** This loop is for dtext only
			//pt3 will be insertion pt of text in WCS
			sds_trans(pt1,&rbucs,&rbecs,0,pt3);
			sds_trans(pt2,&rbucs,&rbecs,0,pt4);
			//*** Text string.
			for(;;) {
				*fs1=0;

				if( GetActiveCommandQueue()->IsEmpty() &&
					GetMenuQueue()->IsEmpty() &&
					mode==CMD_DTEXTMODE) {

					//*** Build a parlink for dragging in DTEXT MODE ONLY.
					if(elp) delete elp; elp=NULL;
					if(SDS_getvar(NULL,DB_QTEXTSIZE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-__LINE__); goto bail; }
					if(3==horizalgn){
						//aligned text uses dummy pts pt5 and pt6
						//because we don't know height of text until user is done typing.
						//pt5 & pt6 are temp, and actual pts will be calc'd
						if(icadRealEqual(lastheight,0.0)){
							//on first pass, we don't know what height to use for our aligned text, because
							//	its going to be adjusted.
							if(SDS_getvar(NULL,DB_QTEXTSIZE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-__LINE__); goto bail; }
							lastheight=setgetrb.resval.rreal;
							if(0.0==lastheight)lastheight=1.0;
						}
						elp=new db_text(SDS_CURDWG);
						elp->set_210(rbecs.resval.rpoint);
						elp->set_10(pt5);
						elp->set_40(lastheight);
						elp->set_50(vert ? adjlastangle+baseang4ecs+(IC_PI/2.0) : adjlastangle+baseang4ecs);
						elp->set_41(widthfact);
						elp->set_51(oblqang);
						elp->set_7( stylstr);
						elp->set_71(tgflags);
						elp->set_72(horizalgn);
						elp->set_73(vertalgn);
						elp->set_11(pt6);
					}else{
						if(icadRealEqual(gpak->txtheight,0.0)){	//first pass thru on a fit or aligned dtext
							if(SDS_getvar(NULL,DB_QTEXTSIZE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-__LINE__); goto bail; }
						}else{
							setgetrb.resval.rreal=gpak->txtheight;
						}
						elp=new db_text(SDS_CURDWG);
						elp->set_210(rbecs.resval.rpoint);
						elp->set_10(pt3);
						elp->set_40(setgetrb.resval.rreal);
						elp->set_50(vert ? adjlastangle+baseang4ecs+(IC_PI/2.0) : adjlastangle+baseang4ecs);
						elp->set_41(widthfact);
						elp->set_51(oblqang);
						elp->set_7(stylstr);
						elp->set_71(tgflags);
						elp->set_72(horizalgn);
						elp->set_73(vertalgn);
						elp->set_11((5==horizalgn) ? pt4 : pt3);
					}

					ename[0]=(long)elp;
					ename[1]=(long)SDS_CURDWG;

					//Store Ortho mode
					struct resbuf rbOrthoStored, rb;
					SDS_getvar(NULL,DB_QORTHOMODE,&rbOrthoStored,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
					
					//Set Ortho off temporarily
					rb.resval.rint = 0;
					sds_setvar("ORTHOMODE"/*DNT*/,&rb);

					SDS_IgnoreMenuWQuote=1;
					SDS_getvar(NULL,DB_QDRAGMODE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
					ret=SDS_dragobject(-5,setgetrb.resval.rint,(sds_real *)ename,
							 (double *)elp->retp_1(),0.0,ResourceString(IDC_TEXT__NTEXT___12, "\nText: " ),NULL,pt7,NULL);
					SDS_IgnoreMenuWQuote=0;

					//Reset Ortho to original
					sds_setvar("ORTHOMODE"/*DNT*/,&rbOrthoStored);

					if(ret==RTKWORD) sds_getinput(fs1);
					if(ret==RTNORM)sds_getinput(fs1);
				} else {
					SDS_IgnoreMenuWQuote=1;
					ret=sds_getstring(1,ResourceString(IDC_TEXT__NTEXT___12, "\nText: " ),fs1);
					SDS_IgnoreMenuWQuote=0;
				}

				if(ret==RTERROR) {
					rc=(-__LINE__); goto bail;
				} else if(ret==RTCAN) {
					rc=(-1L); goto bail;
				} else if(ret==RTKWORD || ret==RTSTR) {
					break;
                } else if(ret==RTNONE
//4M Item:145->
// dtext should continue at a new insertion point
/*
                   || *fs1==0
*/
//<-4M Item:145
                   ){
                    rc=0L;
					goto bail;
				}else if(ret==RTNORM){
					if(0==*fs1){
						//user entered a point for starting a new text entity...
//4M Item:145->
// dtext should continue at a new insertion point

						strcpy(fs1,SDS_cmdcharbuf);
						*SDS_cmdcharbuf = 0;

                        sds_point pttemp;
                        ic_ptcpy(pttemp,pt7);
                        sds_polar(pttemp,(adjlastangle+(IC_PI/2.0)),(!vert)?(gpak->txtheight*TEXTINC):(-gpak->txtheight*TEXTINC),pt7);
//<-4M Item:145
                        pt2[0]+=pt7[0]-pt1[0];
						pt2[1]+=pt7[1]-pt1[1];
						pt2[2]+=pt7[2]-pt1[2];
						ic_ptcpy(pt1,pt7);
						//note: *fs1!=0, if text (==0 if dtext)
					}
					break;
				} else {
					rc=(-1L); goto bail;
				}
			}
			if(*fs1){
				//*** Create entity.
				if(algnfit) {
					if((newelist=sds_buildlist(RTDXF0,"TEXT"/*DNT*/,10,pt3,11,pt4,39,0.0,
						40,(icadRealEqual(gpak->txtheight,0.0)) ? 1.0 : gpak->txtheight,1,fs1,51,oblqang,
						  41,widthfact,7,stylstr,71,tgflags,72,horizalgn,73,vertalgn,210,rbecs.resval.rpoint,0))==NULL) {
						cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
						rc=(-__LINE__); goto bail;
					}
				} else {
					if((newelist=sds_buildlist(RTDXF0,"TEXT"/*DNT*/,10,pt3,40,(icadRealEqual(gpak->txtheight,0.0)) ? 1.0 : gpak->txtheight,39,0.0,
						41,widthfact,1,fs1,50,vert ? adjlastangle+baseang4ecs+(IC_PI/2.0) : adjlastangle+baseang4ecs,
						  51,oblqang,7,stylstr,71,tgflags,210,rbecs.resval.rpoint,0))==NULL) {
						cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
						rc=(-__LINE__); goto bail;
					}
					//*** If there is a horizontal or vertical alignment, add them and the 11 point to the elist.
					if(horizalgn || vertalgn) {
						for(tmprb=newelist; tmprb!=NULL && tmprb->rbnext!=NULL; tmprb=tmprb->rbnext);
						if((tmprb->rbnext=sds_buildlist(72,horizalgn,73,vertalgn,11,pt3,0))==NULL) {
							cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
							rc=(-__LINE__); goto bail;
						}
					}
				}
				if(sds_entmake(newelist)!=RTNORM) {
					cmd_ErrorPrompt(CMD_ERR_CREATETEXT,0);
					rc=(-__LINE__); goto bail;
				}
				sds_entlast(ename);
			}
			//don't copy ename to gpak->lastent yet, we still need both if text is aligned
			if(3==horizalgn){
				if(NULL==(tmprb=sds_entget(ename))){
					lastheight=gpak->txtheight;
				}else{
					ic_assoc(tmprb,40);
					lastheight=gpak->txtheight=ic_rbassoc->resval.rreal;
				}
				if(realign>0){
					//NOTE:  if there was aligned text, we don't know what the calculated height
					//	was.  Get the height of the last object in the drawing and move the text
					//	by an appropriate amount
					pt3[0]=pt3[1]=pt3[2]=0.0;
					sds_polar(pt3,(adjlastangle-(IC_PI/2.0)),(!vert)?(lastheight*TEXTINC):(-lastheight*TEXTINC),pt4);
					pt1[0]+=pt4[0];
					pt1[1]+=pt4[1];
					pt1[2]+=pt4[2];
					pt2[0]+=pt4[0];
					pt2[1]+=pt4[1];
					pt2[2]+=pt4[2];
					//if we're continuing the dext command, or user hit enter
					//to continue from previous aligned dtext, then
					//move the last entity to the correct location
					sds_trans(pt4,&rbucs,&rbecs,1,pt3);
					ic_assoc(tmprb,10);
					ic_rbassoc->resval.rpoint[0]+=pt3[0];
					ic_rbassoc->resval.rpoint[1]+=pt3[1];
					ic_rbassoc->resval.rpoint[2]+=pt3[2];
					ic_assoc(tmprb,11);
					ic_rbassoc->resval.rpoint[0]+=pt3[0];
					ic_rbassoc->resval.rpoint[1]+=pt3[1];
					ic_rbassoc->resval.rpoint[2]+=pt3[2];
					if(sds_entmod(tmprb)!=RTNORM){rc=RTERROR;goto bail;}
					//redraw the previous entity after we move the new one
					sds_redraw(gpak->lastent,IC_REDRAW_DRAW);
				}
				if(tmprb!=NULL){sds_relrb(tmprb);tmprb=NULL;}

				if(mode==CMD_DTEXTMODE){	//get 2 new fake pts to use in dragging parlink
					sds_polar(pt1,(adjlastangle-(IC_PI/2.0)),(!vert)?(lastheight*TEXTINC):(-lastheight*TEXTINC),pt5);
					sds_polar(pt2,(adjlastangle-(IC_PI/2.0)),(!vert)?(lastheight*TEXTINC):(-lastheight*TEXTINC),pt6);
					sds_trans(pt5,&rbucs,&rbecs,0,pt5);
					sds_trans(pt6,&rbucs,&rbecs,0,pt6);
				}
				ic_ptcpy(gpak->nextpt,pt1);
				ic_ptcpy(gpak->nextfitpt,pt2);
			}else{
				//*** Increment insertion point for next text entity

				// Check if Upside Down ( mirror in Y ) flag is set in Text Generation
				if ( tgflags & 4 )
					sds_polar(pt1,(-adjlastangle-(IC_PI/2.0)),(!vert && !(tgflags & 4))?(gpak->txtheight*TEXTINC):(-gpak->txtheight*TEXTINC),gpak->nextpt);
				else
				sds_polar(pt1,(adjlastangle-(IC_PI/2.0)),(!vert)?(gpak->txtheight*TEXTINC):(-gpak->txtheight*TEXTINC),gpak->nextpt);
				pt2[0]+=(gpak->nextpt[0]-pt1[0]);
				pt2[1]+=(gpak->nextpt[1]-pt1[1]);
				pt2[2]+=(gpak->nextpt[2]-pt1[2]);
				ic_ptcpy(pt1,gpak->nextpt);
				if(5==horizalgn)
					ic_ptcpy(gpak->nextfitpt,pt2);
				else
					gpak->nextfitpt[0]=gpak->nextfitpt[1]=gpak->nextfitpt[2]=0.0;
			}
			if(*fs1)ic_encpy(gpak->lastent,ename);//if we made an ent, put it in gpak
			gpak->isnextpt=1;
			gpak->halign=(char)horizalgn;
			gpak->valign=(char)vertalgn;
			//*** DTEXT command continues looping
			if(mode!=CMD_DTEXTMODE) break;
			//now reset pt1 & pt2 for next loop
		}
	}

	bail:
	
	//reset qtextmode after genteration of graphic ents.
	//	regen will repaint them to boxes if needed
	setgetrb.restype=RTSHORT;
	setgetrb.resval.rint=(int)oldqtext;
	sds_setvar("QTEXTMODE"/*DNT*/,&setgetrb);
	//once var is reset, re-init the structure so other
	//	textual cmds (insert of blk w/attribs) will work right
	view->qtextmode=oldqtext;

	SDS_IgnoreMenuWQuote=0;

	if(elp) delete elp; elp=NULL;
	if(mode==CMD_DTEXTMODE)
		{
		IDS_GetThreadController()->SetNoThreadSwap( false );
		}
	IC_RELRB(newelist);
	if(rc==0L) return(RTNORM);
	if(rc==(-1L)) return(RTCAN);
	if(rc<(-1L)) CMD_INTERNAL_MSG(rc);
	return(RTERROR);
}


short cmd_dtext(void) {
	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	return(cmd_textdtext(CMD_DTEXTMODE));
}

short cmd_text(void) {
	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	return(cmd_textdtext(CMD_TEXTMODE));
}

short cmd_attdef(void) {
	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	return(cmd_textdtext(CMD_ATTDEFMODE));
}





