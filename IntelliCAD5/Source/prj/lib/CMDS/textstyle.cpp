/* TEXTSTYLE.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * Supports the style
 *
 */
// THIS FILE HAS BEEN GLOBALIZED!
//** Includes
#include "cmds.h"/*DNT*/
#include "Icad.h" /*DNT*/
#include "sdsapplication.h" /*DNT*/
#include "commandqueue.h" /*DNT*/
#include "IcadApi.h"
#include "styletabrec.h"/*DNT*/
#include "TTFInfoGetter.h"/*DNT*/


// **********
// GLOBALS
//
extern char  *cmd_FontSavePath;


long cmd_style_list(void) {
//*** This function lists text style information.
	char fs1[IC_ACADBUF],stylstr[IC_ACADBUF],*fcp1,*fcp2,fc1;
	struct resbuf *tmprb=NULL,setgetrb;
	int ret,scrollmax,scrollcur=0;
	short tmpflg=0;
	long rc=0L;
	db_styletabrec *pRec;	// EBATECH(CNBR)

	sds_initget(SDS_RSG_OTHER,NULL);
	if((ret=sds_getkword(ResourceString(IDC_TEXTSTYLE__NTEXT_STYLE_S_0, "\nText style(s) to list <*>: " ),stylstr))==RTERROR) {
		rc=(-__LINE__); goto bail;
	} else if(ret==RTCAN) {
		rc=(-1L); goto bail;
	}


//	  if((ret=sds_getstring(0,"\nText style(s) to list <*>: ",stylstr))==RTERROR) {		// TODO - Test above, then delete this.
//		  rc=(-__LINE__); goto bail;
//	  } else if(ret==RTCAN) {
//		  rc=(-1L); goto bail;
//	  }

	sds_textscr();
	SDS_getvar(NULL,DB_QSCRLHIST,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	scrollmax=min(SDS_CMainWindow->m_pTextScrn->m_pPromptWnd->m_pPromptWndList->m_nLinesPerPage,setgetrb.resval.rint)-10;

	sds_printf(ResourceString(IDC_TEXTSTYLE__NTEXT_STYLES__1, "\nText styles:\n" ));
	if(RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
	if(!*stylstr || (stylstr[0]=='*' && stylstr[1]==0)) {
		if((tmprb=sds_tblnext("STYLE"/*DNT*/,1))==NULL) { rc=(-__LINE__); goto bail; }
		tmpflg=1;
	} else {
		for(fcp1=fcp2=stylstr; *fcp2!=0; fcp1=fcp2+1) {
			for(fcp2=fcp1; *fcp2!=',' && *fcp2!=0; fcp2++);
			fc1=*fcp2;
			*fcp2=0;
			tmprb=sds_tblsearch("STYLE"/*DNT*/,fcp1,0);
			*fcp2=fc1;
			if(tmprb!=NULL) break;
		}
	}
	if(tmprb==NULL) {
		cmd_ErrorPrompt(CMD_ERR_FINDTEXT,0);
		rc=(-1L); goto bail;
	}
	do {
		// EBATECH(CNBR) -[ 2002/4/16 Remove shapefont from style list
		if(ic_assoc(tmprb,70)!=0) { rc=(-__LINE__); goto bail; }
		if((ic_rbassoc->resval.rint & IC_SHAPEFILE_SHAPES) == 0 ) {
		// EBATECH(CNBR) ]-
			if(ic_assoc(tmprb,2)!=0) { rc=(-__LINE__); goto bail; }
			if (!strsame(ic_rbassoc->resval.rstring,"SHAPE|REF"/*DNT*/) && ic_rbassoc->resval.rstring[0]!=0) {
				sds_printf(ResourceString(IDC_TEXTSTYLE__NSTYLE_NAME___2, "\nStyle name: %s " ),ic_rbassoc->resval.rstring);
				if(RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
				// EBATECH(CNBR) -[ Print font family when True Type font is used.
				if( pRec = (db_styletabrec *)(SDS_CURDWG->findtabrec( DB_STYLETAB, ic_rbassoc->resval.rstring, 0 ))){
					if( pRec->hasTrueTypeFont()){
						char localName[IC_ACADBUF];
						if( pRec->getFontFamily(localName)){
							sds_printf(ResourceString(IDC_TEXTSTYLE__NFONT_FILE____3, "\nFont file: %s" ),localName);
							if(RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
						}else{
							pRec = NULL; // pRec act as flag.
						}
					}else{
						pRec = NULL;
					}
				}
				if(!pRec){
					if(ic_assoc(tmprb,3)!=0) { rc=(-__LINE__); goto bail; }
					sds_printf(ResourceString(IDC_TEXTSTYLE__NFONT_FILE____3, "\nFont file: %s" ),ic_rbassoc->resval.rstring);
					if(RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
					if(0==ic_assoc(tmprb,4)){
						sds_printf(ResourceString(IDC_TEXTSTYLE__NBIG_FONT_FIL_4, "\nBig-font file: %s" ),ic_rbassoc->resval.rstring);
						if(RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
					}
				}//EBATECH(CNBR) ]-
				if(ic_assoc(tmprb,40)!=0) { rc=(-__LINE__); goto bail; }
				if(sds_rtos(ic_rbassoc->resval.rreal,-1,-1,fs1)!=RTNORM) { rc=(-__LINE__); goto bail; }
				sds_printf(ResourceString(IDC_TEXTSTYLE__N____HEIGHT___5, "\n    Height: %s  " ),fs1);
				if(RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
				if(ic_assoc(tmprb,41)!=0) { rc=(-__LINE__); goto bail; }
				sds_printf(ResourceString(IDC_TEXTSTYLE_WIDTH_FACTOR___6, "Width factor: %.4f  " ),ic_rbassoc->resval.rreal);
				if(ic_assoc(tmprb,50)!=0) { rc=(-__LINE__); goto bail; }
				if(sds_rtos((ic_rbassoc->resval.rreal*180.0/IC_PI),2,0,fs1)!=RTNORM) { rc=(-__LINE__); goto bail; }
				sds_printf(ResourceString(IDC_TEXTSTYLE_OBLIQUING_ANGL_7, "Obliquing angle: %s" ),fs1);
				if(ic_assoc(tmprb,71)!=0) { rc=(-__LINE__); goto bail; }
				*fs1=0;
				if(ic_rbassoc->resval.rint & IC_SHAPEFILE_GEN_BACKWARD) strcat(fs1,ResourceString(IDC_TEXTSTYLE_BACKWARDS__8, "Backwards " ));
				if(ic_rbassoc->resval.rint & IC_SHAPEFILE_GEN_UPSIDEDOWN) strcat(fs1,ResourceString(IDC_TEXTSTYLE_UPSIDE_DOWN__9, "Upside-down " ));
				if(ic_assoc(tmprb,70)!=0) { rc=(-__LINE__); goto bail; }
				if(ic_rbassoc->resval.rint & IC_SHAPEFILE_VERTICAL) strcat(fs1,ResourceString(IDC_TEXTSTYLE_VERTICAL__10, "Vertical " ));
				if(!*fs1) strcpy(fs1,ResourceString(IDC_TEXTSTYLE_NORMAL_11, "Normal" ));
				sds_printf(ResourceString(IDC_TEXTSTYLE__N____GENERAT_12, "\n    Generation: %s\n\n" ),fs1);
				if(RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
			}
		}	// EBATECH(CNBR) 2002/4/16 Remove Shapefont

		IC_RELRB(tmprb);
		if(tmpflg) {
			tmprb=sds_tblnext("STYLE"/*DNT*/,0);
		} else {
			for(fcp1=fcp2+1; *fcp2!=0; fcp1=fcp2+1) {
				for(fcp2=fcp1; *fcp2!=',' && *fcp2!=0; fcp2++);
				fc1=*fcp2;
				*fcp2=0;
				tmprb=sds_tblsearch("STYLE"/*DNT*/,fcp1,0);
				*fcp2=fc1;
				if(tmprb!=NULL) break;
			}
		}
	} while(tmprb!=NULL);

	if(SDS_getvar(NULL,DB_QTEXTSTYLE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-__LINE__); goto bail; }
	sds_printf(ResourceString(IDC_TEXTSTYLE__NCURRENT_TEX_13, "\nCurrent text style: %s" ),setgetrb.resval.rstring);
	IC_FREE(setgetrb.resval.rstring);

bail:

	IC_RELRB(tmprb);
	return(rc);
}

/******************************************************************************
*+	cmd_style
*
*	This function displays the Text Style dialog to allow the user to create
*	and modify text styles.
*
*/

short cmd_style(void)
	{
	if (RTERROR == cmd_iswindowopen())
		return RTERROR;

	ExecuteUIAction(ICAD_WNDACTION_TEXT_STYLE);

	/*DG - 9.1.2002*/// We shouldn't regen in all cases (eg, when cancelling changes in the dialog)
	// So, move it to CTextStyleDlg...
	//cmd_regenall(); // added by Vitaly Spirin

	return(RTNORM);


#ifdef USE_THIS_IN_DIALOG_VERSION_OF_COMMAND
	// See if we have a bigfont.
	if(!typeit)
		{
		if((fcp1=strchr(fontfile,','))!=NULL)
			{
			*fcp1=0;
			strncpy(fs1,fcp1+1,sizeof(fs1)-1);
			if((fcp1=strchr(fs1,'.'/*DNT*/))==NULL)
				ic_setext(fs1,"shx"/*DNT*/);

			if(RTNORM!=sds_findfile(fs1,bigfont))
				{
				//also look in font save path
				if(fcp1=strrchr(fs1,IC_SLASH))
					strncpy(bigfont,fcp1+1,sizeof(bigfont)-1);
				else
					strncpy(bigfont,fs1,sizeof(bigfont)-1);

				strncpy(fs1,cmd_FontSavePath,sizeof(fs1)-1);
				strcat(fs1,bigfont);
				if(RTNORM!=sds_findfile(fs1,bigfont))
					{
					cmd_ErrorPrompt(CMD_ERR_FINDFILE2,0);
					goto getfiles;
					}
				}
			}
		else
			*bigfont=0;
		}

#endif	//USE_THIS_IN_DIALOG_VERSION_OF_COMMAND
	}

/******************************************************************************
*+	cmd_dashStyle
*
*	This function creates named text styles at the command line.
*
*	RETURNS:	RTNORM	- Success.
*				RTCAN	- User cancel.
*				RTERROR	- RTERROR returned from an SDS function.
*
*/

short cmd_dashStyle(void)
	{
	if (cmd_iswindowopen() == RTERROR)
		return RTERROR;

	char fs1[IC_ACADBUF],fs2[IC_ACADBUF],fs3[IC_ACADBUF],stylstr[IC_ACADNMLEN],curstyl[IC_ACADNMLEN],
		fontfile[IC_ACADBUF],bigfont[IC_ACADBUF],*fcp1;
	char fnt3[IC_ACADBUF],fnt4[IC_ACADBUF];//2000-08-26 EBATECH(FUTA)
	struct resbuf setgetrb,*tmprb=NULL;
	sds_real height,width,oblang,fr1;
	int ret;
	int tflags,vertflg=0,newflg=0;
	long rc=0L;
	bool skipvertprompt=FALSE,fontwasfromapp=FALSE;
	db_fontlink *fl=NULL; //DO NOT FREE THIS, "NEVER".
	// EBATECH(CNBR) -[ for TrueType
	char* fcp2=0;
	long stylenumber=0;
	char localName[IC_ACADBUF];
	char globalName[IC_ACADBUF];
	db_styletabrec *pRec = NULL;
	localName[0] = globalName[0] = '\0';
	// ]-


	if(SDS_getvar(NULL,DB_QTEXTSTYLE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
		{
		rc=(-__LINE__);
		goto bail;
		}

	strncpy(curstyl,setgetrb.resval.rstring,sizeof(curstyl)-1);
	IC_FREE(setgetrb.resval.rstring);
	sprintf(fs2,ResourceString(IDC_TEXTSTYLE__N__TO_LIST_E_14, "\n? to list existing styles/<Text style to create or modify> <%s>: " ),
		curstyl);

	for(;;)
		{
		//ret=sds_getstring(0,fs2,fs1);
		ret=sds_getstring(1,fs2,fs1);
		if (ret==RTERROR || ret==RTCAN)
			{
			rc=ret;
			goto bail;
			}

		ic_trim(fs1,"be");
		strupr(fs1);
		if(!*fs1)
			{
			strncpy(stylstr,curstyl,sizeof(stylstr)-1);
			}
		else if(strsame("?"/*DNT*/,fs1) || strsame("_?"/*DNT*/,fs1))
			{
			rc=cmd_style_list();
			goto bail;
			}
		else
			{
			strncpy(stylstr,fs1,sizeof(stylstr)-1);
			}

		if(!ic_isvalidname(stylstr))
			{
			cmd_ErrorPrompt(CMD_ERR_ANYNAME,0);
			continue;
			}

		if((tmprb=sds_tblsearch("STYLE"/*DNT*/,stylstr,0))==NULL)
			{
			newflg=1;
			}

		break;
		}

	// font file name
	if(!newflg)
		{
		if(ic_assoc(tmprb,3)!=0)
			{
			rc=(-__LINE__);
			goto bail;
			}

		strncpy(fs1,ic_rbassoc->resval.rstring,sizeof(fs1)-1);
		if(0==ic_assoc(tmprb,4) && 0!=*(ic_rbassoc->resval.rstring))
			{
			strcat(fs1,","/*DNT*/);
			strncat(fs1,ic_rbassoc->resval.rstring,IC_ACADNMLEN);
			// EBATECH(CNBR) -[ It's old fasion.
			//if(stristr(strrchr(fs1,'\\') ? strrchr(fs1,'\\')+1 : fs1,".ttf"/*DNT*/))
			//	skipvertprompt=TRUE;
			//}
			// EBATECH(CNBR) ]-
			}
		}

	*fs2=0;
	for(;;)
		{
		*bigfont=0;
		// EBATECH(FUTA) -[ Initialize additional buffer
		*fnt3=0;
		*fnt4=0;
		// EBATECH(FUTA) ]-
		if(SDS_getvar(NULL,DB_QFILEDIA,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
			{
			rc=(-__LINE__);
			goto bail;
			}

		if(!*fs2)
			{
			if(!newflg)
				sprintf(fs2,ResourceString(IDC_TEXTSTYLE__NFONT___S____17, "\nFont <%s>: " ),fs1);
			else sprintf(fs2,ResourceString(IDC_TEXTSTYLE__NFONT___18, "\nFont: " ));
			}

		if( GetActiveCommandQueue()->IsNotEmpty() ||
			GetMenuQueue()->IsNotEmpty() ||
			lsp_cmdhasmore)
			{
			fontwasfromapp=TRUE;
			}

		if((ret=sds_getstring(1,fs2,fs3))==RTERROR)
			{
			rc=(-__LINE__);
			goto bail;
			}
		else if(ret==RTCAN)
			{
			rc=RTCAN;
			goto bail;
			}
		// EBATECH(FUTA) -[ 2000-09-07 Copy previous value when hit Enter only.
		bool flg1=TRUE;
		if (!*fs3){
			strncpy(fs3,fs1,sizeof(fs1)-1);
			flg1=FALSE;
		}
		*fs2=0;
		// EBATECH(CNBR) -[ It's old fasion.
		//if(stristr(strrchr(fs3,'\\') ? strrchr(fs3,'\\')+1 : fs3,".ttf"/*DNT*/)){
		//	skipvertprompt=TRUE;
		//}
		// EBATECH(CNBR) ]-
		// EBATECH(FUTA) -[
		//if(!*fs3)
		//	{
		//	if(!newflg)
		//		strncpy(fontfile,fs1,sizeof(fontfile)-1);
		//	else
		//		continue;
		//	}
		//else
		//	{
		// EBATECH(FUTA) ]-
			if(NULL!=(fcp1=strchr(fs3,',')))
				{
				*fcp1=0;
				fcp1++;
				strncpy(fs2,fcp1,sizeof(fs2)-1);//NOTE: FCP1 still points into str
				}

			//fs3 could be empty if only bigfont entered
			if(*fs3){
				if(NULL==(strrchr(strrchr(fs3,IC_SLASH)==NULL ? fs3 : strrchr(fs3,IC_SLASH)+1,'.')))
					ic_setext(fs3,"shx"/*DNT*/);
				strncpy(fnt3,fs3,sizeof(fnt3)-1);//2000-08-26 EBATECH(FUTA)
				// EBATECH(CNBR) -[ 2002/5/11 Script & TrueType
				if(fontwasfromapp){
					strncpy(fontfile,fs3,sizeof(fontfile)-1);
				}
				//}else{
				// EBATECH(CNBR) ]-
					// EBATECH(FUTA) -[ Abort command when the font cannnot found.
					if(sds_findfile(fs3,fontfile)!=RTNORM && flg1 ){
						//sds_printf(ResourceString(IDC_TEXTSTYLE___UNABLE_TO_O_20, "\n\"%s\": Unable to open file" ),fs3);
						//sprintf(fs2,ResourceString(IDC_TEXTSTYLE__NENTER_ANOTH_21, "\nEnter another font name for style %s (or ENTER for none): " ),stylstr);
						//*fontfile=0;
						//continue;
						// EBATECH(CNBR) -[ Check True Type font
						if((fcp2 = strchr( fs3, '.')) != NULL ){
							*fcp2 = '\0';
							if(CTTFInfoGetter::get().getFontFileName(fs3, fnt3)){
								if( CTTFInfoGetter::get().getGlobalStyleByLocalName( fs3, stylenumber, globalName )){
									strcpy(fontfile,fnt3);
									strcpy(localName, fs3);
								}
							}else{
								if( CTTFInfoGetter::get().getGlobalStyleByLocalName( fs3, stylenumber, globalName )){
									strcpy(localName, fs3);
									CTTFInfoGetter::makeFontFileName(globalName,fnt3);
									strcpy(fontfile,fnt3);
									skipvertprompt=TRUE;
								}
							}
						}
						if(!localName[0]){
							// 
							if(fontwasfromapp==0){
						// ]-
							sds_printf(ResourceString(IDC_TEXTSTYLE_INVALID_FONT, "\nInvalid font" ));
							goto bail;
						// 2000-09-07 EBATECH(FUTA) ]-
							}
						}// EBATECH(CNBR) ]-
					}
				//} // EBATECH(CNBR) 2002/5/11 Script & TrueType
			}

			if(NULL!=fcp1 && !localName[0] )
				{
				if(NULL==(strrchr(strrchr(fs2,IC_SLASH)==NULL ? fs2 : strrchr(fs2,IC_SLASH)+1,'.')))
					ic_setext(fs2,"shx"/*DNT*/);
				strncpy(fnt4,fs2,sizeof(fnt4)-1);//2000-08-26 EBATECH(FUTA)
				if(RTNORM!=sds_findfile(fs2,bigfont))
					{
					strncpy(fs3,fontfile,sizeof(bigfont)-1);

					//also look in font save path
					if(NULL!=(fcp1=strrchr(fs3,IC_SLASH)))
						*(fcp1+1)=0;

					strcat(fs3,fs2);
					if(fontwasfromapp)
						{
						strncpy(bigfont,fs3,sizeof(bigfont)-1);
						}
					else
						{
						// EBATECH(FUTA) 2000-09-07 -[ abort command if font error detect.
						if(RTNORM!=sds_findfile(fs3,bigfont))
							{
							//sds_printf(ResourceString(IDC_TEXTSTYLE___UNABLE_TO_O_20, "\n\"%s\": Unable to open file" ),fs2);
							//sprintf(fs2,ResourceString(IDC_TEXTSTYLE__NENTER_ANOTH_21, "\nEnter another font name for style %s (or ENTER for none): " ),stylstr);
							//*fontfile=*bigfont=0;
							//continue;
							sds_printf(ResourceString(IDC_TEXTSTYLE_INVALID_FONT, "\nInvalid font" ));
							goto bail;
							// 2000-09-07 EBATECH(FUTA) ]-
							}
						}
					}
				}
			//} EBATECH(FUTA) 2000-08-26 Remove Font Selection Loop

		break;// always run once.
		}// end for

	if(strrchr(fontfile,IC_SLASH))
		{
		if(NULL!=(fcp1=strchr(fontfile,',')))
			db_astrncpy(&cmd_FontSavePath,fontfile,fcp1-fontfile);
		else
			db_astrncpy(&cmd_FontSavePath,fontfile,strlen(fontfile)+1);

		*(strrchr(cmd_FontSavePath,'\\')+1)=0;
		}

	// See if we have a bigfont.
	// EBATECH(CNBR) -[
	if(0==fontfile[0]){						//if only new bigfont entered...
		if( ic_assoc(tmprb,3) == 0 ){		//copy old font name
			strncpy(fontfile,ic_rbassoc->resval.rstring,sizeof(fontfile)-1);
			strcpy(fnt3,fontfile);
		}else{	// when no font name, set default. It's AutoCAD2000 way.
			strcpy(fontfile,"txt.shx"/*DNT*/);
			strcpy(fnt3,fontfile);
		}
	}	// EBATECH(CNBR) ]-
	// EBATECH(CNBR) -[ 2001/8/15 print selected fontname if succeed.
	sds_printf(ResourceString(IDC_TEXTSTYLE_VAILID_FONTS, "\nfont: %s bigfont: %s are selected." ), fnt3, fnt4 );
	// EBATECH(CNBR) ]-

	// Height
	for(;;)
		{
		// Get default text height.
		if(!newflg)
			{
			if(ic_assoc(tmprb,40)!=0)
				{
				rc=(-__LINE__);
				goto bail;
				}

			height=ic_rbassoc->resval.rreal;
			}
		else
			height=0.0;

		if(sds_rtos(height,-1,-1,fs2)!=RTNORM)
			{
			rc=(-__LINE__);
			goto bail;
			}

		sprintf(fs1,ResourceString(IDC_TEXTSTYLE__NTEXT_HEIGHT_22, "\nText height <%s>: " ),fs2);
		if(sds_initget(0,NULL)!=RTNORM)
			{
			rc=(-__LINE__);
			goto bail;
			}

		if((ret=sds_getdist(NULL,fs1,&height))==RTERROR)
			{
			rc=(-__LINE__);
			goto bail;
			}
		else if(ret==RTCAN)
			{
			rc=RTCAN;
			goto bail;
			}
		else if(ret==RTNORM || ret==RTNONE)
			{
			break;
			}
		else
			{
			cmd_ErrorPrompt(CMD_ERR_DIST2NDPT,0);
			continue;
			}
		}

	// Width factor
	for(;;)
		{
		// Get default width.
		if(!newflg)
			{
			if(ic_assoc(tmprb,41)!=0)
				{
				rc=(-__LINE__);
				goto bail;
				}

			width=ic_rbassoc->resval.rreal;
			}
		else
			width=1.0;

		sprintf(fs1,ResourceString(IDC_TEXTSTYLE__NWIDTH_FACTO_23, "\nWidth factor <%.4f>: " ),width);
		if(sds_initget(SDS_RSG_NOZERO+SDS_RSG_NONEG,NULL)!=RTNORM)
			{
			rc=(-__LINE__);
			goto bail;
			}

		if((ret=sds_getreal(fs1,&width))==RTERROR)
			{
			rc=(-__LINE__);
			goto bail;
			}
		else if(ret==RTCAN)
			{
			rc=RTCAN;
			goto bail;
			}
		else if(ret==RTNORM || ret==RTNONE)
			{
			break;
			}
		else
			{
			cmd_ErrorPrompt(CMD_ERR_NUMERIC,0);
			continue;
			}
		}

	// Obliquing angle.
	for(;;)
		{
		if(!newflg)
			{
			if(ic_assoc(tmprb,50)!=0)
				{
				rc=(-__LINE__);
				goto bail;
				}

			oblang=ic_rbassoc->resval.rreal;
			}
		else
			oblang=0.0;

		if(sds_rtos((oblang*180.0/IC_PI),-1,-1,fs2)!=RTNORM)
			{
			rc=(-__LINE__);
			goto bail;
			}

		sprintf(fs1,ResourceString(IDC_TEXTSTYLE__NOBLIQUING_A_24, "\nObliquing angle <%s>: " ),fs2);
		if(sds_initget(0,NULL)!=RTNORM)
			{
			rc=(-__LINE__);
			goto bail;
			}

		if((ret=SDS_getangleneg(NULL,fs1,&oblang))==RTERROR)
			{
			rc=(-__LINE__);
			goto bail;
			}
		else if(ret==RTCAN)
			{
			rc=RTCAN;
			goto bail;
			}
		else if(ret==RTNORM || ret==RTNONE)
			{
			if((oblang*180.0/IC_PI)>85.0 || (oblang*180.0/IC_PI)<(-85.0))
				{
				cmd_ErrorPrompt(CMD_ERR_OBLIQUELG,0);
				continue;
				}

			break;
			}
		else
			{
			cmd_ErrorPrompt(CMD_ERR_ANG2NDPT,0);
			continue;
			}
		}

	// Backwards text?
	if(!newflg)
		{
		if(ic_assoc(tmprb,71)!=0)
			{
			rc=(-__LINE__);
			goto bail;
			}

		tflags=ic_rbassoc->resval.rint;
		}
	else
		tflags=0;

	for (;;)
		{
		if (tflags & IC_SHAPEFILE_GEN_BACKWARD)
			sprintf(fs2,ResourceString(IDC_TEXTSTYLE__NDRAW_TEXT_B_25, "\nDraw text backwards? <%s> "), ResourceString(IDC_TEXTSTYLE_YES_27, "Yes"));
		else
			sprintf(fs2,ResourceString(IDC_TEXTSTYLE__NDRAW_TEXT_B_25, "\nDraw text backwards? <%s> "),ResourceString(IDC_TEXTSTYLE_NO_28, "No")	);

		sds_initget(RSG_OTHER,ResourceString(IDC_TEXTSTYLE_INITGET_YES_B_26, "Yes-Backwards_text|Yes No-Normal_text|No ~_Yes ~_No" ));
		ret=sds_getkword(fs2,fs1);
		if (ret==RTERROR || ret==RTCAN)
			{
			rc=ret;
			goto bail;
			}
		else if (ret==RTNONE)
			{
			rc=RTNORM;
			break;	// Leave tflags as is.	User accepted the current state as default.
			}
		else if (ret==RTNORM)
			{
			if (strisame(fs1,"YES"/*DNT*/))
				{
				tflags |= IC_SHAPEFILE_GEN_BACKWARD;
				break;
				}
			else if (strisame(fs1,"NO"/*DNT*/))
				{
				tflags&=(~IC_SHAPEFILE_GEN_BACKWARD);
				break;
				}
			else
				{
				cmd_ErrorPrompt(CMD_ERR_YESNO,0);  //"Yes or No, please."
				continue;
				}
			}
		}

	// Upside-down text?
	for (;;)
		{
		sprintf(fs2,ResourceString(IDC_TEXTSTYLE__NDRAW_TEXT_U_29, "\nDraw text upside-down? <%s> " ),(tflags & IC_SHAPEFILE_GEN_UPSIDEDOWN) ?
			ResourceString(IDC_TEXTSTYLE_YES_27, "Yes"):
			ResourceString(IDC_TEXTSTYLE_NO_28, "No"));

		sds_initget(RSG_OTHER,ResourceString(IDC_TEXTSTYLE_INITGET_YES_U_30, "Yes-Upside-down_text|Yes No-Right_side_up|No ~_Yes ~_No" ));
		ret=sds_getkword(fs2,fs1);
		if (ret==RTERROR || ret==RTCAN)
			{
			rc=ret;
			goto bail;
			}
		else if (ret==RTNONE)
			{
			rc=RTNORM;
			break;	// Leave tflags as is.	User accepted the current state as default.
			}
		else if (ret==RTNORM)
			{
			if (strisame(fs1,"YES"/*DNT*/))
				{
				tflags |= IC_SHAPEFILE_GEN_UPSIDEDOWN;
				break;
				}
			else if (strisame(fs1,"NO"/*DNT*/))
				{
				tflags&=(~IC_SHAPEFILE_GEN_UPSIDEDOWN);
				break;
				}
			else
				{
				cmd_ErrorPrompt(CMD_ERR_YESNO,0);  //"Yes or No, please."
				continue;
				}
			}
		}

	// Does this font support vertical text
	if(!skipvertprompt)  //if we are skipping already then no need to check
		{
		if(db_setfontusingmap(fontfile,NULL,&fl,IC_ALLOW_ICAD_FNT,SDS_CURDWG)==0)
			{
			if(fl->modes!=2)
				skipvertprompt=TRUE;
			}
		}

	// Vertical text?
	for (;;)
		{
		if(!newflg)
			{
			if(ic_assoc(tmprb,70)!=0)
				{
				rc=(-__LINE__);
				goto bail;
				}

			vertflg=ic_rbassoc->resval.rint;
			}
		else
			vertflg=0;

		// 2003-5-10 Bugzilla#78571 Clear vertical flag when user change existing text style to use TrueType font. EBATECH(CNBR)
		if(skipvertprompt)
			{
			vertflg &= (~IC_SHAPEFILE_VERTICAL);
			break;
			}
		// EBATECH(CNBR) ]-

#ifdef LOCALISE_ISSUE
// the Y N combination will not be of use to Languages..
// suggest changing to "Yes" and "No"
#endif
		sprintf(fs2,ResourceString(IDC_TEXTSTYLE__NDRAW_TEXT_V_31, "\nDraw text vertically? <%s> " ),
			(vertflg & IC_SHAPEFILE_VERTICAL) ?
			ResourceString(IDC_TEXTSTYLE_YES_27,"Yes"):
			ResourceString(IDC_TEXTSTYLE_NO_28,	"No"));

		sds_initget(RSG_OTHER,ResourceString(IDC_TEXTSTYLE_INITGET_YES_V_32, "Yes-Vertical_text|Yes No-Horizontal_text|No ~_Yes ~_No" ));
		ret=sds_getkword(fs2,fs1);
		if (ret==RTERROR || ret==RTCAN)
			{
			rc=ret;
			goto bail;
			}
		else if (ret==RTNONE)
			{
			rc=RTNORM;
			break;	// Leave vertflg as is.  User accepted the current state as default.
			}
		else if (ret==RTNORM)
			{
			if (strisame(fs1,"YES"/*DNT*/))
				{
				vertflg |= IC_SHAPEFILE_VERTICAL;
				break;
				}
			else if (strisame(fs1,"NO"/*DNT*/))
				{
				vertflg&=(~IC_SHAPEFILE_VERTICAL);
				break;
				}
			else
				{
				cmd_ErrorPrompt(CMD_ERR_YESNO,0);  //"Yes or No, please."
				continue;
				}
			}
		}

	// Set stylstr, fontfile, height, width, oblang, tflags, and vertflg into the table ~|
	if(!newflg)
		{
		if(ic_assoc(tmprb,42)!=0)
			{
			rc=(-__LINE__);
			goto bail;
			}

		fr1=ic_rbassoc->resval.rreal;
		//if(ic_assoc(tmprb,4)==0) strncpy(bigfont,ic_rbassoc->resval.rstring,sizeof(bigfont)-1);
		//else *bigfont=0;
		}
	else
		{
		fr1=0;
		}

	IC_RELRB(tmprb);
	//2000-09-07 EBATECH(FUTA) -[
	if((tmprb=sds_buildlist(RTDXF0,"STYLE"/*DNT*/,2,stylstr,70,vertflg,40,height,41,width,50,oblang,
		71,tflags,42,fr1,3,fnt3,4,fnt4,0))==NULL)
	//if((tmprb=sds_buildlist(RTDXF0,"STYLE"/*DNT*/,2,stylstr,70,vertflg,40,height,41,width,50,oblang,
	//	71,tflags,42,fr1,3,fontfile,4,bigfont,0))==NULL)
	//2000-09-07 EBATECH(FUTA) ]-
		{
		cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
		rc=RTCAN;
		goto bail;
		}

	if(newflg)
		{
		if(SDS_tblmesser(tmprb,IC_TBLMESSER_MAKE,SDS_CURDWG)!=RTNORM)
			{
			rc=(-__LINE__);
			goto bail;
			}
		}
	else
		{
		if(SDS_tblmesser(tmprb,IC_TBLMESSER_MODIFY,SDS_CURDWG)!=RTNORM)
			{
			rc=(-__LINE__);
			goto bail;
			}
		}

	//2001-06-11 EBATECH(CNBR) -[ Add or Remove TrueType font
	pRec = (db_styletabrec *)(SDS_CURDWG->findtabrec( DB_STYLETAB, stylstr, 0 ));
	if( !pRec ){
		rc=(-__LINE__);
		goto bail;
	}
	if( localName[0] ){
		pRec->setFontFamily(localName,SDS_CURDWG);
		pRec->setFontStyleNumber(localName, stylenumber, SDS_CURDWG);
	}else{
		pRec->ensureNoTrueTypeEED(SDS_CURDWG);
	}
	//2000-09-07 EBATECH(FUTA) ]-

	// Set the current text style
	setgetrb.restype=RTSTR;
	if((setgetrb.resval.rstring= new char [strlen(stylstr)+1])==NULL)
		{
		cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
		rc=RTCAN;
		goto bail;
		}

	strcpy(setgetrb.resval.rstring,stylstr);
	if(sds_setvar("TEXTSTYLE"/*DNT*/,&setgetrb)!=RTNORM)
		{
		rc=(-__LINE__);
		goto bail;
		}

	IC_FREE(setgetrb.resval.rstring);

bail:
	cmd_regenall(); // added by Vitaly Spirin
	IC_RELRB(tmprb);
	if (rc==0L || rc==RTNORM)
		{
		return(RTNORM);
		}
	else if (rc==(-1L) || rc==RTCAN)
		{
		return(RTCAN);
		}
	else if (rc==RTERROR)
		{
		return(RTERROR);
		}
	else if (rc<(-1L))
		{
		CMD_INTERNAL_MSG(rc);
		return(RTERROR);
		}
	else
		return(RTERROR);
	}		// End of cmd_dashStyle.



