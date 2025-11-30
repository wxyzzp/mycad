/* C:\DEV\LIB\CMDS\CMDS12.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here> 
 *
 */

// THIS FILE HAS BEEN GLOBALIZED!

//#pragma optimize ("", off)	// uncomment this if things become too bad...

// =========================================================================================================================================
// ================================================================ Includes ===============================================================
// =========================================================================================================================================

#include "cmds.h"/*DNT*/
//BugZilla No. 78155; 27-05-2002
#include "IcadDoc.h"/*DNT*/

#include "dimensions.h"/*DNT*/
#include "IcadView.h"/*DNT*/
#include "cmdsDimBlock.h"/*DNT*/
#include "cmdsNotUsed.h"/*DNT*/
#include "cmdsDimVars.h"
#include "DbMText.h"/*DNT*/
#include "IcadTextScrn.h"/*DNT*/

static void addMtextFormatCodes(int type, char *text, LPCTSTR ext, bool continuation);
static void addTolerances(int toltype, sds_real unformated, char *formatedMtext);

// =========================================================================================================================================
// =========================================================== External Variables ==========================================================
// =========================================================================================================================================

extern                        SDS_AtNodeDrag;
extern char* layername;
extern cmd_dimvars plinkvar[];
//Modified Cybage SBD 30/11/2001 DD/MM/YYYY 
//Reason : Fix for Bug No. 77866 and 77867 from Bugzilla 
extern BOOL bdimUpdate;

// =========================================================================================================================================
// =========================================================== Global Variables ============================================================
// =========================================================================================================================================

sds_point    dimucs[4] = {{0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}};
sds_real     dimelev = 0.0;
BOOL         tmp_depoints = TRUE;
short        cmd_dimmode = 0;					// Flag specifying if the user is at the "Dim:" prompt (dimmode=1).
sds_point    dimzeropt = {0.0, 0.0, 0.0};		// Needed to put this variable here because it gets initialized.
char         dimlsttext[256] = "0.0000"/*DNT*/,	// Last value entered.
             cmd_string[256] = ""/*DNT*/,
			 cmd_string1[256] = ""/*DNT*/,
			 cmd_string2[256] = ""/*DNT*/;
db_drawing*  tmp_flp = NULL;

// =========================================================================================================================================
// =========================================================== Global structure's ==========================================================
// =========================================================================================================================================

//BugZilla No. 78155; 27-05-2002
//struct cmd_dimlastpt lastdimpts;
struct cmd_dimeparlink* plink = NULL;
// Added Cybage VSB  18/12/2001 DD/MM/YYYY [
// Reference: Bug 77946 from Bugzilla
// Description: IntelliCAD does not provide proper validation for Block Name during DOV command
bool isArrowBlock(char* str);
void cmd_addarrowblock(char* str);
// Added Cybage VSB  18/12/2001 DD/MM/YYYY ]
// =======================================================================================================================================
// ============================================================= Function's ==============================================================
// =======================================================================================================================================

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DIM_SETPARLINK
//
//
//
// return : TRUE on success.
//        : FALSE on fail.
//
void cmd_setparlink(db_drawing *flp) {

    resbuf	rbb;

    if (plink->Setparflag) return;

	cmd_CleanUpDimVarStrings();
	db_astrncpy(&layername,"0",2);

	if (flp->ret_FileVersion() > IC_ACAD14)
	{
		if(plink->pLayerName)
		{
			db_astrncpy(&layername, plink->pLayerName, strlen(plink->pLayerName) + 1);
			plink->pLayerName = NULL;
		}
		else
		{
			SDS_getvar(NULL, DB_QCLAYER, &rbb, flp, &SDS_CURCFG, &SDS_CURSES);
			db_astrncpy(&layername, rbb.resval.rstring, strlen(rbb.resval.rstring) + 1);
			IC_FREE(rbb.resval.rstring);
			rbb.resval.rstring = NULL;
		}
	}
	cmd_setplinkvar(flp);
}

void cmd_DeleteDimStructAndStrings(void) {	// SDS_EXIT calls this to clean up the plink structure on exiting. 

	if(plink)		  { delete plink;			plink      =NULL;  }
	cmd_CleanUpDimVarStrings();

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DIM_GENSTRING
//
//
//
// return : TRUE on success.
//        : FALSE on fail.
//
void cmd_genstring(struct cmd_dimeparlink *plink,char *text,sds_real minwidth,LPCTSTR ext,short mode,db_drawing *flp) {
    sds_real       ff1 = 0.0;
    char          *sptr1 = NULL,
                  *sptr2 = NULL,
                   string[IC_ACADBUF] = ""/*DNT*/;
    
    *text=0;

	struct sds_resbuf rb;
	BOOL bInPaperSpace = FALSE;
	SDS_getvar(NULL,DB_QTILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if(rb.resval.rint==0)
	{
		SDS_getvar(NULL,DB_QCVPORT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		if(rb.resval.rint==1)
			bInPaperSpace=TRUE;
	}

    // Now look to see if there is something that is supposed to be post fixed to the dimension.
	char *prefix = NULL, *postfix = NULL;
	char tmpDimPost[IC_ACADBUF];
	if (plinkvar[DIMPOST].dval.cval && *plinkvar[DIMPOST].dval.cval) {
		_tcscpy(tmpDimPost, plinkvar[DIMPOST].dval.cval);

		if (_tcsstr(tmpDimPost, "<"/*DNT*/) && _tcsstr(tmpDimPost, ">"/*DNT*/)) {
			if (*tmpDimPost == '<'/*DNT*/) {
				prefix = _tcstok(tmpDimPost, ">"/*DNT*/);
				postfix	= _tcstok(NULL, ">"/*DNT*/);
				prefix = NULL;
			} else {
				prefix	= _tcstok(tmpDimPost, "<"/*DNT*/);
				postfix	= _tcstok(NULL,">"/*DNT*/);
			}
		} else
			postfix = tmpDimPost;
	} 

	// Now look to see if there is something that is supposed to be post/pre fixed to the 
	// alternate dimensions.
	char *preAfix = NULL, *postAfix = NULL;
	char tmpDimAPost[IC_ACADBUF];
	if (plinkvar[DIMAPOST].dval.cval && *plinkvar[DIMAPOST].dval.cval) {
		_tcscpy(tmpDimAPost, plinkvar[DIMAPOST].dval.cval);

		if (_tcsstr(tmpDimAPost, "["/*DNT*/) && _tcsstr(tmpDimAPost, "]"/*DNT*/)) {
			if (*tmpDimAPost == '['/*DNT*/) {
				preAfix = _tcstok(tmpDimAPost, "]"/*DNT*/);
				postAfix= _tcstok(NULL, "]"/*DNT*/);
				preAfix = NULL;
			} else {
				preAfix	= _tcstok(tmpDimAPost, "["/*DNT*/);
				postAfix= _tcstok(NULL,"]"/*DNT*/);
			}
		} else
			postAfix = tmpDimAPost;
	}

    // Get the text that will be displayed in the dimension.
    if (plink->text && *plink->text && !strstr(plink->text,"<>"/*DNT*/)) {
        strcpy(text,plink->text);
    } else {
        if (mode) {
            ff1=minwidth;
            // Multiply the with with the scale factor (DIMLFAC).
            if(bInPaperSpace)
                minwidth*=fabs(plinkvar[DIMLFAC].dval.rval);
            else
                minwidth*=(plinkvar[DIMLFAC].dval.rval < 0) ? 1 : plinkvar[DIMLFAC].dval.rval;
            // If the value needs to be rounded (DIMRND) is the rounding value.
            if (!icadRealEqual(plinkvar[DIMRND].dval.rval, 0.0)) {
                // SystemMetrix(Hiro)-[FIX: When DIMRND is over 1.0, the dimension value is not rounded correctly.
                //sds_real intpart, rangeint;
                //sds_real rangefract = modf(modf(minwidth, &intpart) / plinkvar[DIMRND].dval.rval, &rangeint);
                //if (rangefract >= 0.5) {
                //    // Round it up.
                //    minwidth = intpart + ((rangeint + 1) * plinkvar[DIMRND].dval.rval);
                //} else {
                //    //Round it down.
                //    minwidth = intpart + ((rangeint) * plinkvar[DIMRND].dval.rval);
                //}
                double dDimrnd = plinkvar[DIMRND].dval.rval;
                double a = fabs(minwidth / dDimrnd);
                double b1 = floor(a)       * dDimrnd;
                double b2 = floor(a + 1.0) * dDimrnd;
                double c1 = fabs(minwidth - b1);
                double c2 = fabs(minwidth - b2);
                if (c1 > c2)
                  {
                    minwidth = b2;
                  }
                else
                  {
                    minwidth = b1;
                  }
                // ]-SystemMetrix(Hiro) FIX: When DIMRND is over 1.0, the dimension value is not rounded correctly.
            }
			//EBATECH(CNBR) Bugzilla#78443 Use DIMZIN from current SYSVAR buf.
			cmd_rtos_dim(minwidth,plinkvar[DIMUNIT].dval.ival,plinkvar[DIMDEC].dval.ival,text, plinkvar[DIMZIN].dval.ival, plinkvar[DIMDSEP].dval.ival);
 		    //cmd_rtos_dim(minwidth,plinkvar[DIMUNIT].dval.ival,plinkvar[DIMDEC].dval.ival,text, DIMTYPEMAIN);
			// If prefix is not NULL, then the 'dimpost' sysvar has been set and already
			// split into the usable parts. prefix<>postfix. prepend prefix to text here 
			// before adding mtext format codes.
			if (prefix != NULL) {
                addMtextFormatCodes(plinkvar[DIMUNIT].dval.ival, text, NULL, TRUE);
                addTolerances(DIMUNIT, minwidth, text);
                sprintf(string, "\\A1;%s%s"/*DNT*/, prefix, text);
                _tcscpy(text, string);
            } else if (prefix == NULL && plinkvar[DIMLIM].dval.ival) {
                addMtextFormatCodes(plinkvar[DIMUNIT].dval.ival, text, ext, TRUE);
                addTolerances(DIMUNIT, minwidth, text);
				if(*ext=='D'/*DNT*/) 
					sprintf(string, "\\A1;\\U+2205%s"/*DNT*/, text);
				else if(*ext=='R'/*DNT*/)
					sprintf(string, "\\A1;R%s"/*DNT*/, text);
				else
					sprintf(string, "\\A1;%s"/*DNT*/, text);
                _tcscpy(text, string);
			} else {
                addMtextFormatCodes(plinkvar[DIMUNIT].dval.ival, text, ext, FALSE);
                addTolerances(DIMUNIT, minwidth, text);
            }
			if (postfix != NULL) {
                if (plinkvar[DIMTOL].dval.ival || plinkvar[DIMLIM].dval.ival)
				    sprintf(string, "%s%s", text, postfix);
                else
                    sprintf(string, "%s%s", text, postfix);
				_tcscpy(text, string);
			}

			// If the alternate units flag is on.
			if (plinkvar[DIMALT].dval.ival) {
				// This multiplies the unit by the alternate unit factor. DIMALTF
				ff1*=plinkvar[DIMALTF].dval.rval;
				// EBATECH(CNBR) -[ 2003-3-31 Support DIMALTRND
				// If the value needs to be rounded (DIMALTRND) is the rounding value.
				if (!icadRealEqual(plinkvar[DIMALTRND].dval.rval, 0.0)) {
					double dDimaltrnd = plinkvar[DIMALTRND].dval.rval;
					double a = fabs(ff1 / dDimaltrnd);
					double b1 = floor(a)       * dDimaltrnd;
					double b2 = floor(a + 1.0) * dDimaltrnd;
					double c1 = fabs(ff1 - b1);
					double c2 = fabs(ff1 - b2);
					if (c1 > c2)
					{
						ff1 = b2;
					}
					else
					{
						ff1 = b1;
					}
				}
				// EBATECH(CNBR) ]-
				// Creates the string of the real with the correct decimal places defined by (DIMALTD) and unit type defined by DIMALTU
				//EBATECH(CNBR) Bugzilla#78443 Use DIMALTZ from current SYSVAR buf.
				cmd_rtos_dim(ff1,plinkvar[DIMALTU].dval.ival,plinkvar[DIMALTD].dval.ival,string, plinkvar[DIMALTZ].dval.ival, plinkvar[DIMDSEP].dval.ival);
				//cmd_rtos_dim(ff1,plinkvar[DIMALTU].dval.ival,plinkvar[DIMALTD].dval.ival,string, DIMTYPEALT);
				addMtextFormatCodes(plinkvar[DIMALTU].dval.ival, string, ext, TRUE);
				addTolerances(DIMALT, minwidth, string);

				char altformat[IC_ACADBUF];
				bool tolsAreSame = plinkvar[DIMTP].dval.rval == plinkvar[DIMTM].dval.rval;
				//Bugzilla No. 77887 ; 03-06-2002
				sds_real dimtfac = plink->dimtfac;
				sds_real dimfontheight = plinkvar[DIMTXT].dval.rval;
				if ( (plinkvar[DIMTOL].dval.ival && !tolsAreSame) || plinkvar[DIMLIM].dval.ival)
					dimtfac = dimtfac * 2.4;
				if ( plink->dimfontheight != 0 )
					dimfontheight = plink->dimfontheight;

                //if ((plinkvar[DIMTOL].dval.ival || plinkvar[DIMLIM].dval.ival)  && !tolsAreSame) {
				if ((plinkvar[DIMTOL].dval.ival || plinkvar[DIMLIM].dval.ival) && (dimtfac > dimfontheight )  ) {
					//Bugzilla No. 77887 ; 03-06-2002
					if (preAfix == NULL)
						//sprintf(altformat, string[strlen(string) - 1] == ';' || plinkvar[DIMTFAC].dval.rval != 1.0 ? " {\\o\\l\\H%f;[}%s"/*DNT*/ : ";{\\o\\l\\H%f;[}%s"/*DNT*/, plinkvar[DIMTXT].dval.rval * plinkvar[DIMSCALE].dval.rval * 2.4,string);
						 sprintf(altformat," {\\o\\l\\H%f;[}%s"/*DNT*/, dimtfac,string);
					else
						//sprintf(altformat, string[strlen(string) - 1] == ';' || plinkvar[DIMTFAC].dval.rval != 1.0 ? " {\\o\\l\\H%f;[}%s%s"/*DNT*/ : ";{\\o\\l\\H%f;[}%s%s"/*DNT*/, plinkvar[DIMTXT].dval.rval * plinkvar[DIMSCALE].dval.rval * 2.4, preAfix,string);						
						sprintf(altformat, " {\\o\\l\\H%f;[}%s%s"/*DNT*/, dimtfac, preAfix,string);
                } else {
					if (preAfix == NULL)
						sprintf(altformat, " [%s"/*DNT*/, string);
					else
						sprintf(altformat, " [%s%s"/*DNT*/, preAfix, string);
				}
                strcat(text,altformat);

                //if ((plinkvar[DIMTOL].dval.ival || plinkvar[DIMLIM].dval.ival) && !tolsAreSame) {
				if ((plinkvar[DIMTOL].dval.ival || plinkvar[DIMLIM].dval.ival) && (dimtfac > dimfontheight )  ) {
					//Bugzilla No. 77887 ; 03-06-2002
					if (postAfix == NULL)
						//sprintf(altformat, text[strlen(text) - 1] == ';'  || plinkvar[DIMTFAC].dval.rval != 1.0 ? "{\\o\\l\\H%f;]}"/*DNT*/ : ";{\\o\\l\\H%f;]}"/*DNT*/, plinkvar[DIMTXT].dval.rval * plinkvar[DIMSCALE].dval.rval * 2.4);
						sprintf(altformat, "{\\o\\l\\H%f;]}"/*DNT*/ , dimtfac);
					else
						//sprintf(altformat, text[strlen(text) - 1] == ';'  || plinkvar[DIMTFAC].dval.rval != 1.0 ? "%s{\\o\\l\\H%f;]}"/*DNT*/ : ";%s{\\o\\l\\H%f;]}"/*DNT*/,postAfix, plinkvar[DIMTXT].dval.rval * plinkvar[DIMSCALE].dval.rval * 2.4);
						sprintf(altformat, "%s{\\o\\l\\H%f;]}"/*DNT*/ ,postAfix, dimtfac);

                } else {
					if (postAfix == NULL)
						sprintf(altformat, "]", postAfix);
					else
						sprintf(altformat, "%s]", postAfix);
				}
                strcat(text,altformat);
            }
        } else {
			 // Bugzila#78443 EBATECH(CNBR) DIMAZIN, 2002/3/31 DIMDSEP
#ifdef OLD_ANGBASE
			int ignoremode = 2;
#else
            // We should not take into account ANGBASE value
			int ignoremode = 3;
#endif
            sds_angtos_dim(ignoremode,minwidth,plinkvar[DIMAUNIT].dval.ival,(plinkvar[DIMADEC].dval.ival==-1)?plinkvar[DIMDEC].dval.ival:plinkvar[DIMADEC].dval.ival,text,plinkvar[DIMAZIN].dval.ival,plinkvar[DIMDSEP].dval.ival);
            //sds_angtos_dim(2,minwidth,plinkvar[DIMAUNIT].dval.ival,(plinkvar[DIMADEC].dval.ival==-1)?plinkvar[DIMDEC].dval.ival:plinkvar[DIMADEC].dval.ival,text);

            // EBATECH(watanabe)-[add degree mark
            if (plinkvar[DIMAUNIT].dval.ival == 0) {
                strcat(text, "%%d");
            }
            // ]-EBATECH(watanabe)

            addMtextFormatCodes(plinkvar[DIMUNIT].dval.ival, text, ext, TRUE);
            addTolerances(DIMANG, minwidth, text);
            if (plinkvar[DIMPOST].dval.cval && *plinkvar[DIMPOST].dval.cval) {
                if ((sptr1=strchr(plinkvar[DIMPOST].dval.cval,'<'/*DNT*/))!=NULL) {
                    if ((sptr2=strchr(plinkvar[DIMPOST].dval.cval,'>'/*DNT*/))==NULL) return;
                    *sptr1=0;
                    sprintf(string,"%s%s%s"/*DNT*/,plinkvar[DIMPOST].dval.cval,text,(sptr2+1));
                    *sptr1='<'/*DNT*/;
                    strcpy(text,string);
                } else { 
                    sprintf(string,"%s%s"/*DNT*/,text,plinkvar[DIMPOST].dval.cval); 
                    strcpy(text,string); 
                }
            }
            sprintf(string,"\\A1;%s"/*DNT*/,text);
            strcpy(text,string);
        }
        // IF the user entered the (<>) symbol.
        if (plink->text && *plink->text && strstr(plink->text,"<>"/*DNT*/)) {
            sptr1=strchr(plink->text,'<'/*DNT*/); *sptr1=0; sptr2=sptr1+2;
            sprintf(string,"%s%s%s"/*DNT*/,plink->text,text,sptr2);
            strcpy(text,string);
            *sptr1='<'/*DNT*/;
        }
    }
    return;
}

//////////////////////////////////////////////////////////////////////////////
// This function draws the box, using the cord. in xpt.
//      _xpt[0]                  _xpt[1]
//     /________________________/
//     |                        |
//     |________________________|
//      \_xpt[2]                \_xpt[3]
//
// Parameter defs.
// -----------------------------------------------------------
// dimclrd      The color assigned to the dimension lines.
// xpt          arraw of points containing the corners of the box.
// flp          ??????????
// dragmode     Specifies if dimension is being dragged or not.
//
BOOL cmd_drawbox(short dimclrd,short dimlwd, sds_point *xpt,db_drawing *flp,short dragmode) {
    if(RTNORM!=cmd_dimaddline(layername,dimclrd,dimlwd,xpt[0],xpt[1],flp,dragmode)) return FALSE;
    if(RTNORM!=cmd_dimaddline(layername,dimclrd,dimlwd,xpt[0],xpt[2],flp,dragmode)) return FALSE;
    if(RTNORM!=cmd_dimaddline(layername,dimclrd,dimlwd,xpt[1],xpt[3],flp,dragmode)) return FALSE;
    if(RTNORM!=cmd_dimaddline(layername,dimclrd,dimlwd,xpt[2],xpt[3],flp,dragmode)) return FALSE;
    return TRUE;
}

void  cmd_ucstodim(sds_point oragin,sds_point ucspoint,sds_point dimpoint,sds_real sinofang,sds_real cosofang) {
    dimpoint[0]=((ucspoint[0]-oragin[0])*cosofang)+((ucspoint[1]-oragin[1])*sinofang);
    dimpoint[1]=((oragin[0]-ucspoint[0])*sinofang)+((ucspoint[1]-oragin[1])*cosofang);
}

void  cmd_dimtoucs(sds_point oragin,sds_point ucspoint,sds_point dimpoint,sds_real sinofang,sds_real cosofang) {
    dimpoint[0]=(ucspoint[0]*cosofang)-(ucspoint[1]*sinofang)+oragin[0];
    dimpoint[1]=(ucspoint[0]*sinofang)+(ucspoint[1]*cosofang)+oragin[1];
    dimpoint[2]=oragin[2];
}

// CHECKS for existance of internal Arrowhead blocks and creates if need
BOOL checkBlock(char *blockName,db_drawing *flp) {
    sds_point		ptt[4] = {{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0}},
					ptl[5] = {{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0}};
	sds_point		blkPt = {0.0, 0.0, 0.0};
 	int				res = TRUE;
    short			fi1 = 0;
    sds_real		sinofang = 0.0, cosofang = 0.0;
    short			clr = DB_BBCOLOR;
    short			lweight = DB_BBWEIGHT;
    resbuf*			rb = NULL;

	const sds_real		unity = 1.0;

    if ( !blockName || !(*blockName) || !IsInternalArrow( blockName) ) return(FALSE);

    // Firs we check if block exists
	rb=SDS_tblgetter("BLOCK"/*DNT*/,NULL,TRUE,flp);
    while(rb) {
        if (ic_assoc(rb,2)!=0) { sds_relrb(rb); return(FALSE); }
        if (strisame(ic_rbassoc->resval.rstring,blockName)) break;
        sds_relrb(rb);
        rb=SDS_tblgetter("BLOCK"/*DNT*/,NULL,FALSE,flp);
    };
	if( !rb )
	{
		// just create corresponded block and add it into database

		/*D.G.*/// As far as I understand, we shouldn't use DIMSCALE while creating arrowheads blocks definitions
		// because we scale these blocks while inserting them in cmd_dimaddinsert. So, I've commented out scaling here.
//		sds_real	Scale = plinkvar[DIMSCALE].dval.rval;
		
		//clr=(plinkvar[DIMCLRD].dval.ival)?plinkvar[DIMCLRD].dval.ival:0; //EBATECH(CNBR) Block always create BYBLOCK Color.
			
		if( RTNORM != dwg_addblock( layername, clr, blockName,0, blkPt, NULL, flp ) )
			return (FALSE);

		if (strisame(DIM_OBLIQUE,blockName))
		{                     // Oblique.
			ptl[0][0]+=-((unity/**Scale*/)/2);
			ptl[1][0]-=-((unity/**Scale*/)/2);
			ptl[0][1]+=-((unity/**Scale*/)/2);
			ptl[1][1]-=-((unity/**Scale*/)/2);

			if(RTNORM!=cmd_dimaddline(layername,clr,lweight,ptl[0],ptl[1],flp,0 /*dragmode*/,DIM_ADD_TO_BLOCK)) return(FALSE);
			ptl[3][0]+=-(unity/**Scale*/);
			if(RTNORM!=cmd_dimaddline(layername,clr,lweight,ptt[2],ptt[3],flp,0 /*dragmode*/,DIM_ADD_TO_BLOCK)) return(FALSE);
			// add end of block
		}
		else if(strisame(DIM_CLOSED,blockName))
		{                      // Closed arrow.
			ptl[1][0]+=-(unity/**Scale*/);
			for (fi1=1;fi1<3;++fi1) ic_ptcpy(ptl[fi1+1],ptl[fi1]);
			ptl[1][1]-=((unity/**Scale*/)*DIM_ONE_SIXTH);
			ptl[2][1]+=((unity/**Scale*/)*DIM_ONE_SIXTH);
			if(RTNORM!=cmd_dimaddline(layername,clr,lweight,ptl[0],ptl[1],flp,0,DIM_ADD_TO_BLOCK)) return(FALSE);
			if(RTNORM!=cmd_dimaddline(layername,clr,lweight,ptl[1],ptl[2],flp,0,DIM_ADD_TO_BLOCK)) return(FALSE);
			if(RTNORM!=cmd_dimaddline(layername,clr,lweight,ptl[2],ptl[0],flp,0,DIM_ADD_TO_BLOCK)) return(FALSE);
			if(RTNORM!=cmd_dimaddline(layername,clr,lweight,ptl[0],ptl[3],flp,0,DIM_ADD_TO_BLOCK)) return(FALSE);
		}
		else if (strisame(DIM_DOT,blockName))
		{                         // Dot
			sds_polar(ptl[0],0.0,((unity/**Scale*/)/4),ptl[1]);
			sds_polar(ptl[0],IC_PI,((unity/**Scale*/)/4),ptl[0]);
			sds_polar(ptl[1],IC_PI,(((unity/**Scale*/)/4)*3),ptl[2]);
			sds_polar(ptl[2],IC_PI,((unity/**Scale*/)/2),ptl[3]);
			db_handitem *pelp=NULL;
			if(RTNORM!=cmd_dimaddPolyline(layername,clr,lweight,1,ptl[0],1,((unity/**Scale*/)/2),((unity/**Scale*/)/2),flp,0,&pelp)) return(FALSE);
			if(RTNORM!=cmd_dimaddVertex(layername,clr,lweight,ptl[0],((unity/**Scale*/)/2),((unity/**Scale*/)/2),1,flp,0)) return(FALSE);
			if(RTNORM!=cmd_dimaddVertex(layername,clr,lweight,ptl[1],((unity/**Scale*/)/2),((unity/**Scale*/)/2),1,flp,0)) return(FALSE);
			if(RTNORM!=cmd_dimaddSeqend(layername,clr,lweight,flp,0)) return(FALSE);
			if(RTNORM!=cmd_dimaddline(layername,clr,lweight,ptl[2],ptl[3],flp,0,DIM_ADD_TO_BLOCK)) return(FALSE);
		}
		else if (strisame(DIM_CLOSED_BLANK,blockName))
		{                // Closed Blank.
			ptl[1][0]+=-(unity/**Scale*/);
			ic_ptcpy(ptl[2],ptl[1]);
			ptl[1][1]-=((unity/**Scale*/)*DIM_ONE_SIXTH);
			ptl[2][1]+=((unity/**Scale*/)*DIM_ONE_SIXTH);
			if(RTNORM!=cmd_dimaddline(layername,clr,lweight,ptl[0],ptl[1],flp,0,DIM_ADD_TO_BLOCK)) return(FALSE);
			if(RTNORM!=cmd_dimaddline(layername,clr,lweight,ptl[1],ptl[2],flp,0,DIM_ADD_TO_BLOCK)) return(FALSE);
			if(RTNORM!=cmd_dimaddline(layername,clr,lweight,ptl[2],ptl[0],flp,0,DIM_ADD_TO_BLOCK)) return(FALSE);
		}
		else if (strisame(DIM_ARCH_TICK,blockName))
		{                   // Architectural Tick.
			sinofang=sin(IC_PI/4); cosofang=cos(IC_PI/4);
	        ptt[0][0]-=-((unity/**Scale*/)/2);
	        ptt[2][0]+=-((unity/**Scale*/)/2);
	        ic_ptcpy(ptt[1],ptt[0]); ic_ptcpy(ptt[3],ptt[2]);
	        ptt[0][1]+=-((unity/**Scale*/)/16);
	        ptt[1][1]-=-((unity/**Scale*/)/16);
	        ptt[2][1]+=-((unity/**Scale*/)/16);
	        ptt[3][1]-=-((unity/**Scale*/)/16);
			for (fi1=0;fi1<4;++fi1) cmd_dimtoucs(dimzeropt,ptt[fi1],ptl[fi1],sinofang,cosofang);
	        if (RTNORM!=cmd_dimaddsolid(layername,clr,lweight,ptl[0],ptl[1],ptl[2],ptl[3],flp,0,DIM_ADD_TO_BLOCK,plink->pt210)) return(FALSE);
			// Make the line from the extension line the the end of the dimension line.
	        ic_ptcpy(ptl[2],ptl[4]);
		    ic_ptcpy(ptl[3],ptl[4]);
			ptl[3][0]+=-(unity/**Scale*/);
			if(RTNORM!=cmd_dimaddline(layername,clr,lweight,ptl[2],ptl[3],flp,0,DIM_ADD_TO_BLOCK)) return(FALSE);
		}
		else if (strisame(DIM_OPEN,blockName))
		{                        // Open.
			ptl[1][0]+=-(unity/**Scale*/);
			for (fi1=1;fi1<3;++fi1) ic_ptcpy(ptl[fi1+1],ptl[fi1]);
			ptl[1][1]-=((unity/**Scale*/)*DIM_ONE_SIXTH);
			ptl[2][1]+=((unity/**Scale*/)*DIM_ONE_SIXTH);
			if(RTNORM!=cmd_dimaddline(layername,clr,lweight,ptl[0],ptl[1],flp,0,DIM_ADD_TO_BLOCK)) return(FALSE);
			if(RTNORM!=cmd_dimaddline(layername,clr,lweight,ptl[2],ptl[0],flp,0,DIM_ADD_TO_BLOCK)) return(FALSE);
			if(RTNORM!=cmd_dimaddline(layername,clr,lweight,ptl[0],ptl[3],flp,0,DIM_ADD_TO_BLOCK)) return(FALSE);
		}
		else if (strisame(DIM_ORIGIN_IND,blockName))
		{                  // Origin Indiaction.
			ptl[1][0]+=-(unity/**Scale*/);
			if(RTNORM!=cmd_dimaddCircle(layername,clr,lweight,ptl[0],((unity/**Scale*/)/2),flp,0)) return(FALSE);
			if(RTNORM!=cmd_dimaddline(layername,clr,lweight,ptl[0],ptl[1],flp,0,DIM_ADD_TO_BLOCK)) return(FALSE);
		}
		else if (strisame(DIM_RIGHT_ANGLE,blockName))
		{                 // Right-Angle.
			sinofang=sin(-(IC_PI/4)); cosofang=cos(-(IC_PI/4));
			ptt[1][0]+=-(unity/**Scale*/);
			cmd_dimtoucs(dimzeropt,ptt[1],ptl[1],sinofang,cosofang);
			sinofang=sin(IC_PI/4); cosofang=cos(IC_PI/4);
			ptt[2][0]+=-(unity/**Scale*/);
			cmd_dimtoucs(dimzeropt,ptt[2],ptl[2],sinofang,cosofang);
			ptl[3][0]+=-((unity/**Scale*/)/**2*/);
			if(RTNORM!=cmd_dimaddline(layername,clr,lweight,ptl[0],ptl[1],flp,0,DIM_ADD_TO_BLOCK)) return(FALSE);
			if(RTNORM!=cmd_dimaddline(layername,clr,lweight,ptl[0],ptl[2],flp,0,DIM_ADD_TO_BLOCK)) return(FALSE);
			if(RTNORM!=cmd_dimaddline(layername,clr,lweight,ptl[0],ptl[3],flp,0,DIM_ADD_TO_BLOCK)) return(FALSE);
		}
		else if (strisame(DIM_OPEN_30,blockName))
		{                     // Open 30.
			sinofang=sin(-(IC_PI/12)); cosofang=cos(-(IC_PI/12));
		    ptt[1][0]+=-(unity/**Scale*/);
			cmd_dimtoucs(dimzeropt,ptt[1],ptl[1],sinofang,cosofang);
			sinofang=sin(IC_PI/12); cosofang=cos(IC_PI/12);
			ptt[2][0]+=-(unity/**Scale*/);
			cmd_dimtoucs(dimzeropt,ptt[2],ptl[2],sinofang,cosofang);
			ptl[3][0]+=-(unity/**Scale*/);
			if(RTNORM!=cmd_dimaddline(layername,clr,lweight,ptl[0],ptl[1],flp,0,DIM_ADD_TO_BLOCK)) return(FALSE);
			if(RTNORM!=cmd_dimaddline(layername,clr,lweight,ptl[0],ptl[2],flp,0,DIM_ADD_TO_BLOCK)) return(FALSE);
			if(RTNORM!=cmd_dimaddline(layername,clr,lweight,ptl[0],ptl[3],flp,0,DIM_ADD_TO_BLOCK)) return(FALSE);
		}
		else if (strisame(DIM_DOT_BLANKED,blockName))
		{                 // Dot Blanked.
			ptl[1][0]+=-((unity/**Scale*/)/2);
			ptl[2][0]+=-(unity/**Scale*/);
			if(RTNORM!=cmd_dimaddCircle(layername,clr,lweight,ptl[0],((unity/**Scale*/)/2),flp,0)) return(FALSE);
			if(RTNORM!=cmd_dimaddline(layername,clr,lweight,ptl[1],ptl[2],flp,0,DIM_ADD_TO_BLOCK)) return(FALSE);
		}
		else if (strisame(DIM_DOT_SMALL,blockName))
		{                   // Dot Small.
			sds_polar(ptl[0],0.0,((unity/**Scale*/)/16),ptl[1]);
			sds_polar(ptl[0],IC_PI,((unity/**Scale*/)/16),ptl[0]);
			sds_polar(ptl[2],(IC_PI),(unity/**Scale*/),ptl[3]);
			db_handitem *pelp=NULL;
			if(RTNORM!=cmd_dimaddPolyline(layername,plinkvar[DIMCLRD].dval.ival,lweight,1,ptl[0],1,((unity/**Scale*/)/2),((unity/**Scale*/)/2),flp,0,&pelp)) return(FALSE);
			if(RTNORM!=cmd_dimaddVertex(layername,clr,lweight,ptl[0],((unity/**Scale*/)/2),((unity/**Scale*/)/2),1,flp,0)) return(FALSE);
			if(RTNORM!=cmd_dimaddVertex(layername,clr,lweight,ptl[1],((unity/**Scale*/)/2),((unity/**Scale*/)/2),1,flp,0)) return(FALSE);
			if(RTNORM!=cmd_dimaddSeqend(layername,clr,lweight,flp,0)) return(FALSE);
			if(RTNORM!=cmd_dimaddline(layername,clr,lweight,ptl[2],ptl[3],flp,0,DIM_ADD_TO_BLOCK)) return(FALSE);
		}
		else if (strisame(DIM_BOX_FILLED,blockName))
		{                   // Box Filled.
			ptl[0][0]+=-((unity/**Scale*/)/2);
			ptl[0][1]-=-((unity/**Scale*/)/2);
			ptl[1][0]-=-((unity/**Scale*/)/2);
			ptl[1][1]-=-((unity/**Scale*/)/2);
			ptl[2][0]-=-((unity/**Scale*/)/2);
			ptl[2][1]+=-((unity/**Scale*/)/2);
			ptl[3][0]+=-((unity/**Scale*/)/2);
			ptl[3][1]+=-((unity/**Scale*/)/2);
			if (RTNORM!=cmd_dimaddsolid(layername,clr,lweight,ptl[0],ptl[1],ptl[3],ptl[2],flp,0,DIM_ADD_TO_BLOCK,plink->pt210)) return(FALSE);
			ic_ptcpy(ptl[0],blkPt);
			ptl[0][0]+=-((unity/**Scale*/)/2);
			ic_ptcpy(ptl[1],ptl[0]);
			ptl[1][0]+=-((unity/**Scale*/)/2);
			if(RTNORM!=cmd_dimaddline(layername,clr,lweight,ptl[0],ptl[1],flp,0,DIM_ADD_TO_BLOCK)) return(FALSE);
		}
		else if (strisame(DIM_BOX,blockName))
		{                         // Box.
			ptl[0][0]+=-((unity/**Scale*/)/2);
			ptl[0][1]-=-((unity/**Scale*/)/2);
			ptl[1][0]-=-((unity/**Scale*/)/2);
			ptl[1][1]-=-((unity/**Scale*/)/2);
			ptl[2][0]-=-((unity/**Scale*/)/2);
			ptl[2][1]+=-((unity/**Scale*/)/2);
			ptl[3][0]+=-((unity/**Scale*/)/2);
			ptl[3][1]+=-((unity/**Scale*/)/2);
			if(RTNORM!=cmd_dimaddline(layername,clr,lweight,ptl[0],ptl[1],flp,0,DIM_ADD_TO_BLOCK)) return(FALSE);
			if(RTNORM!=cmd_dimaddline(layername,clr,lweight,ptl[1],ptl[2],flp,0,DIM_ADD_TO_BLOCK)) return(FALSE);
			if(RTNORM!=cmd_dimaddline(layername,clr,lweight,ptl[2],ptl[3],flp,0,DIM_ADD_TO_BLOCK)) return(FALSE);
			if(RTNORM!=cmd_dimaddline(layername,clr,lweight,ptl[3],ptl[0],flp,0,DIM_ADD_TO_BLOCK)) return(FALSE);
			ic_ptcpy(ptl[0],blkPt);
			ptl[0][0]+=-((unity/**Scale*/)/2);
			ic_ptcpy(ptl[1],ptl[0]);
			ptl[1][0]+=-((unity/**Scale*/)/2);
			if(RTNORM!=cmd_dimaddline(layername,clr,lweight,ptl[0],ptl[1],flp,0,DIM_ADD_TO_BLOCK)) return(FALSE);
		}
		else if (strisame(DIM_DATUMFILLED,blockName))
		{                 // Datum Triangle Filled.
			ptl[0][1]-=-((unity/**Scale*/)/2);
			ptl[1][1]+=-((unity/**Scale*/)/2);
			ptl[2][0]+=-((unity/**Scale*/)/*/2*/);
			if (RTNORM!=cmd_dimaddsolid(layername,clr,lweight,ptl[0],ptl[1],ptl[2],ptl[2],flp,0,DIM_ADD_TO_BLOCK,plink->pt210)) return(FALSE);
    
		}
		else if (strisame(DIM_DATUMBLANK,blockName))
		{                  // Datum Triangle.
			ptl[0][1]-=-((unity/**Scale*/)/2);
			ptl[1][1]+=-((unity/**Scale*/)/2);
			ptl[2][0]+=-((unity/**Scale*/)/*/2*/);
			if(RTNORM!=cmd_dimaddline(layername,clr,lweight,ptl[0],ptl[1],flp,0,DIM_ADD_TO_BLOCK)) return(FALSE);
			if(RTNORM!=cmd_dimaddline(layername,clr,lweight,ptl[1],ptl[2],flp,0,DIM_ADD_TO_BLOCK)) return(FALSE);
			if(RTNORM!=cmd_dimaddline(layername,clr,lweight,ptl[2],ptl[0],flp,0,DIM_ADD_TO_BLOCK)) return(FALSE);
		}
		else if (strisame(DIM_INTEGRAL,blockName))
		{                    // Integral.
			sinofang=sin(IC_PI/3); cosofang=cos(IC_PI/3);
			ptt[1][0]+=-((unity/**Scale*/)/4);
			cmd_dimtoucs(dimzeropt,ptt[1],ptl[1],sinofang,cosofang);
			sinofang=sin(IC_PI/6); cosofang=cos(IC_PI/6);
			cmd_ucstodim(dimzeropt,ptl[1],ptt[1],sinofang,cosofang);
			ptt[1][0]+=-((unity/**Scale*/)/4);
			cmd_dimtoucs(dimzeropt,ptt[1],ptl[2],sinofang,cosofang);
			if(RTNORM!=cmd_dimaddline(layername,clr,lweight,ptl[0],ptl[1],flp,0,DIM_ADD_TO_BLOCK)) return(FALSE);
			if(RTNORM!=cmd_dimaddline(layername,clr,lweight,ptl[1],ptl[2],flp,0,DIM_ADD_TO_BLOCK)) return(FALSE);
			ptl[1][0] = -ptl[1][0]; ptl[1][1] = -ptl[1][1];
			ptl[2][0] = -ptl[2][0]; ptl[2][1] = -ptl[2][1];
			if(RTNORM!=cmd_dimaddline(layername,clr,lweight,ptl[0],ptl[1],flp,0,DIM_ADD_TO_BLOCK)) return(FALSE);
			if(RTNORM!=cmd_dimaddline(layername,clr,lweight,ptl[1],ptl[2],flp,0,DIM_ADD_TO_BLOCK)) return(FALSE);
			// Make the line from the extension line the the end of the dimension line.
			ptl[3][0]+=-(unity/**Scale*/);
			if(RTNORM!=cmd_dimaddline(layername,clr,lweight,ptl[0],ptl[3],flp,0,DIM_ADD_TO_BLOCK)) return(FALSE);
		}

		if( dwg_addendblk( flp ) == RTERROR )
			res = FALSE;
	}
	if( rb )
		sds_relrb( rb );
	return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DIM_MAKEITEMHEAD
//
//
//
// return : TRUE on success.
//        : FALSE on fail.
//
BOOL cmd_MakeItemHead(sds_point pt[4],char *ItemName,db_drawing *flp,short dragmode,sds_real sinofdeg,sds_real cosofdeg,sds_real rotAng, BOOL dirflag,BOOL sideflag,sds_point ptdef,int color) {
    sds_point	ptt[4]={{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0}},
                ptl[5];
    short		fi1 = 0;
    char*		fcp1 = NULL;
    short		clr = 0;
    resbuf*		rb = NULL;
	short		celweight;
    if (!ItemName) return(FALSE);

    clr=(plinkvar[DIMCLRD].dval.ival)?plinkvar[DIMCLRD].dval.ival:color;
    celweight = cmd_getCelweight();

    ic_ptcpy(ptl[0],pt[(sideflag)?0:1]);                            // pt[0] = right or left side pt[1] = opposite side of pt[0].
    for (fi1=0;fi1<4;++fi1) ic_ptcpy(ptl[fi1+1],ptl[fi1]);           // Make all points the same.

//    if (ItemName && (fcp1=strchr(ItemName,'_'/*DNT*/))==NULL) fcp1=ItemName; else { if (ItemName) fcp1=(fcp1+1); }
	fcp1 = ItemName;

    rb=SDS_tblgetter("BLOCK"/*DNT*/,NULL,TRUE,flp);
    while(rb) {
        if (ic_assoc(rb,2)!=0) { sds_relrb(rb); return(FALSE); }
        if (strisame(ic_rbassoc->resval.rstring,fcp1)) break;
        sds_relrb(rb);
        rb=SDS_tblgetter("BLOCK"/*DNT*/,NULL,FALSE,flp);
    };
    if (rb) {
        cmd_dimtoucs(ptdef,ptl[0],ptt[0],sinofdeg,cosofdeg);

		if (RTNORM!=cmd_dimaddinsert(layername,clr, plinkvar[DIMLWD].dval.ival, ptt[0], plinkvar[DIMASZ].dval.rval, rotAng, fcp1,flp,dragmode)) return(FALSE);
        sds_relrb(rb);
        return(TRUE);
    }

    if (plinkvar[DIMTSZ].dval.rval) {                                                 // Oblique caused by DIMTSZ= some length.
		//Bugzilla No. 60918 ; 24-06-2002 [
        ptl[0][1]+=(dirflag)?-((plinkvar[DIMTSZ].dval.rval*plinkvar[DIMSCALE].dval.rval)):((plinkvar[DIMTSZ].dval.rval*plinkvar[DIMSCALE].dval.rval));
        ptl[1][1]-=(dirflag)?-((plinkvar[DIMTSZ].dval.rval*plinkvar[DIMSCALE].dval.rval)):((plinkvar[DIMTSZ].dval.rval*plinkvar[DIMSCALE].dval.rval));
        ptl[0][0]+=(dirflag)?-((plinkvar[DIMTSZ].dval.rval*plinkvar[DIMSCALE].dval.rval)):((plinkvar[DIMTSZ].dval.rval*plinkvar[DIMSCALE].dval.rval));
        ptl[1][0]-=(dirflag)?-((plinkvar[DIMTSZ].dval.rval*plinkvar[DIMSCALE].dval.rval)):((plinkvar[DIMTSZ].dval.rval*plinkvar[DIMSCALE].dval.rval));
		//Bugzilla No. 60918 ; 24-06-2002 ]
        for (fi1=0;fi1<2;++fi1) cmd_dimtoucs(ptdef,ptl[fi1],ptt[fi1],sinofdeg,cosofdeg);
        if(RTNORM!=cmd_dimaddline(layername,clr,plinkvar[DIMLWD].dval.ival,ptt[0],ptt[1],flp,dragmode)) return(FALSE);
    } else if (strisame(DIM_NONE,fcp1)) {
        // Do Nothing.
    } else if (!(*fcp1)/* || (strisame(DIM_ARCH_TICK,fcp1) && plinkvar[DIMTSZ].dval.rval == 0.0)*/) {                                             // Closed Filled.
        ptl[1][0]+=(dirflag)?-(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval):(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval);
        ic_ptcpy(ptl[2],ptl[1]);
        ptl[1][1]-=((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)*DIM_ONE_SIXTH);
        ptl[2][1]+=((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)*DIM_ONE_SIXTH);
        for (fi1=0;fi1<3;++fi1) {
            cmd_dimtoucs(ptdef,ptl[fi1],ptt[fi1],sinofdeg,cosofdeg);
        }
        if (RTNORM!=cmd_dimaddsolid(layername,clr,celweight,ptt[0],ptt[1],ptt[2],ptt[2],flp,dragmode,
			plinkvar[DIMASO].dval.ival ? DIM_ADD_ASSOC : 0,plink->pt210)) return(FALSE);
	}

// Obsolete code /creating internal arrowheads on the fly/
/*    } else if (strisame(DIM_CLOSED,fcp1)) {                      // Closed arrow.
        ptl[1][0]+=(dirflag)?-(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval):(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval);
        for (fi1=1;fi1<3;++fi1) ic_ptcpy(ptl[fi1+1],ptl[fi1]);
        ptl[1][1]-=((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)*DIM_ONE_SIXTH);
        ptl[2][1]+=((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)*DIM_ONE_SIXTH);
        for (fi1=0;fi1<4;++fi1) cmd_dimtoucs(ptdef,ptl[fi1],ptt[fi1],sinofdeg,cosofdeg);
        if(RTNORM!=cmd_dimaddline(layername,clr,ptt[0],ptt[1],flp,dragmode)) return(FALSE);
        if(RTNORM!=cmd_dimaddline(layername,clr,ptt[1],ptt[2],flp,dragmode)) return(FALSE);
        if(RTNORM!=cmd_dimaddline(layername,clr,ptt[2],ptt[0],flp,dragmode)) return(FALSE);
        if(RTNORM!=cmd_dimaddline(layername,clr,ptt[0],ptt[3],flp,dragmode)) return(FALSE);
    } else if (strisame(DIM_DOT,fcp1)) {                         // Dot
        sds_polar(ptl[0],0.0,((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/4),ptl[1]);
        sds_polar(ptl[0],IC_PI,((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/4),ptl[0]);
        sds_polar(ptl[((dirflag)?1:0)],((dirflag)?IC_PI:0.0),(((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/4)*3),ptl[2]);
        sds_polar(ptl[2],((dirflag)?IC_PI:0.0),((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2),ptl[3]);
        for (fi1=0;fi1<4;++fi1) {
            cmd_dimtoucs(ptdef,ptl[fi1],ptt[fi1],sinofdeg,cosofdeg);
        }
        db_handitem *pelp=NULL;
        if(RTNORM!=cmd_dimaddPolyline(layername,clr,1,ptl[0],1,((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2),((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2),flp,dragmode,&pelp)) return(FALSE);
        if(RTNORM!=cmd_dimaddVertex(layername,clr,ptt[0],((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2),((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2),1,flp,dragmode)) return(FALSE);
        if(RTNORM!=cmd_dimaddVertex(layername,clr,ptt[1],((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2),((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2),1,flp,dragmode)) return(FALSE);
        if(RTNORM!=cmd_dimaddSeqend(layername,clr,flp,dragmode)) return(FALSE);
        if(RTNORM!=cmd_dimaddline(layername,clr,ptt[2],ptt[3],flp,dragmode)) return(FALSE);
        sds_name ename;
		ename[0]=(long)pelp;
		ename[1]=(long)flp;
		SDS_UpdateEntDisp(ename,0);
		sds_redraw(ename,IC_REDRAW_XOR);
        delete pelp;
    } else if (strisame(DIM_CLOSED_BLANK,fcp1)) {                // Closed Blank.
        ptl[1][0]+=(dirflag)?-(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval):(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval);
        ic_ptcpy(ptl[2],ptl[1]);
        ptl[1][1]-=((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)*DIM_ONE_SIXTH);
        ptl[2][1]+=((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)*DIM_ONE_SIXTH);
        for (fi1=0;fi1<3;++fi1) cmd_dimtoucs(ptdef,ptl[fi1],ptt[fi1],sinofdeg,cosofdeg);
        if(RTNORM!=cmd_dimaddline(layername,clr,ptt[0],ptt[1],flp,dragmode)) return(FALSE);
        if(RTNORM!=cmd_dimaddline(layername,clr,ptt[1],ptt[2],flp,dragmode)) return(FALSE);
        if(RTNORM!=cmd_dimaddline(layername,clr,ptt[2],ptt[0],flp,dragmode)) return(FALSE);
    } else if (strisame(DIM_OBLIQUE,fcp1)) {                     // Oblique.
        ptl[0][0]+=(dirflag)?-((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2):((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2);
        ptl[1][0]-=(dirflag)?-((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2):((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2);
        ptl[0][1]+=(dirflag)?-((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2):((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2);
        ptl[1][1]-=(dirflag)?-((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2):((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2);
        for (fi1=0;fi1<2;++fi1) cmd_dimtoucs(ptdef,ptl[fi1],ptt[fi1],sinofdeg,cosofdeg);
        if(RTNORM!=cmd_dimaddline(layername,clr,ptt[0],ptt[1],flp,dragmode)) return(FALSE);
        ptl[3][0]+=(dirflag)?-(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval):(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval);
        for (fi1=2;fi1<4;++fi1) cmd_dimtoucs(ptdef,ptl[fi1],ptt[fi1],sinofdeg,cosofdeg);
        if(RTNORM!=cmd_dimaddline(layername,clr,ptt[2],ptt[3],flp,dragmode)) return(FALSE);
    } else if (strisame(DIM_ARCH_TICK,fcp1)) {                   // Architectural Tick.
        sinofang=sin(IC_PI/4); cosofang=cos(IC_PI/4);
        for (fi1=0;fi1<4;++fi1) cmd_ucstodim(dimzeropt,ptl[fi1],ptt[fi1],sinofang,cosofang);
        ptt[0][0]-=(dirflag)?-((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2):((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2);
        ptt[2][0]+=(dirflag)?-((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2):((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2);
        ic_ptcpy(ptt[1],ptt[0]); ic_ptcpy(ptt[3],ptt[2]);
        ptt[0][1]+=(dirflag)?-((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/16):((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/16);
        ptt[1][1]-=(dirflag)?-((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/16):((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/16);
        ptt[2][1]+=(dirflag)?-((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/16):((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/16);
        ptt[3][1]-=(dirflag)?-((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/16):((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/16);
        for (fi1=0;fi1<4;++fi1) cmd_dimtoucs(dimzeropt,ptt[fi1],ptl[fi1],sinofang,cosofang);
        for (fi1=0;fi1<4;++fi1) cmd_dimtoucs(ptdef,ptl[fi1],ptt[fi1],sinofdeg,cosofdeg);
        if (RTNORM!=cmd_dimaddsolid(layername,clr,ptt[0],ptt[1],ptt[2],ptt[3],flp,dragmode)) return(FALSE);
        // Make the line from the extension line the the end of the dimension line.
        ic_ptcpy(ptl[2],ptl[4]);
        ic_ptcpy(ptl[3],ptl[4]);
        ptl[3][0]+=(dirflag)?-(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval):(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval);
        for (fi1=2;fi1<4;++fi1) cmd_dimtoucs(ptdef,ptl[fi1],ptt[fi1],sinofdeg,cosofdeg);
        if(RTNORM!=cmd_dimaddline(layername,clr,ptt[2],ptt[3],flp,dragmode)) return(FALSE);
    } else if (strisame(DIM_OPEN,fcp1)) {                        // Open.
        ptl[1][0]+=(dirflag)?-(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval):(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval);
        for (fi1=1;fi1<3;++fi1) ic_ptcpy(ptl[fi1+1],ptl[fi1]);
        ptl[1][1]-=((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)*DIM_ONE_SIXTH);
        ptl[2][1]+=((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)*DIM_ONE_SIXTH);
        for (fi1=0;fi1<4;++fi1) cmd_dimtoucs(ptdef,ptl[fi1],ptt[fi1],sinofdeg,cosofdeg);
        if(RTNORM!=cmd_dimaddline(layername,clr,ptt[0],ptt[1],flp,dragmode)) return(FALSE);
        if(RTNORM!=cmd_dimaddline(layername,clr,ptt[2],ptt[0],flp,dragmode)) return(FALSE);
        if(RTNORM!=cmd_dimaddline(layername,clr,ptt[0],ptt[3],flp,dragmode)) return(FALSE);
    } else if (strisame(DIM_ORIGIN_IND,fcp1)) {                  // Origin Indiaction.
        ptl[1][0]+=(dirflag)?-(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval):(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval);
        for (fi1=0;fi1<2;++fi1) cmd_dimtoucs(ptdef,ptl[fi1],ptt[fi1],sinofdeg,cosofdeg);
        if(RTNORM!=cmd_dimaddCircle(layername,clr,ptt[0],((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2),flp,dragmode)) return(FALSE);
        if(RTNORM!=cmd_dimaddline(layername,clr,ptt[0],ptt[1],flp,dragmode)) return(FALSE);
    } else if (strisame(DIM_RIGHT_ANGLE,fcp1)) {                 // Right-Angle.
        sinofang=sin(-(IC_PI/4)); cosofang=cos(-(IC_PI/4));
        cmd_ucstodim(dimzeropt,ptl[0],ptt[1],sinofang,cosofang);
        ptt[1][0]+=(dirflag)?-(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval):(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval);
        cmd_dimtoucs(dimzeropt,ptt[1],ptl[1],sinofang,cosofang);
        sinofang=sin(IC_PI/4); cosofang=cos(IC_PI/4);
        cmd_ucstodim(dimzeropt,ptl[0],ptt[2],sinofang,cosofang);
        ptt[2][0]+=(dirflag)?-(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval):(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval);
        cmd_dimtoucs(dimzeropt,ptt[2],ptl[2],sinofang,cosofang);
*///        ptl[3][0]+=(dirflag)?-((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/**2*/):((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/**2*/);
/*        for (fi1=0;fi1<4;++fi1) cmd_dimtoucs(ptdef,ptl[fi1],ptt[fi1],sinofdeg,cosofdeg);
        if(RTNORM!=cmd_dimaddline(layername,clr,ptt[0],ptt[1],flp,dragmode)) return(FALSE);
        if(RTNORM!=cmd_dimaddline(layername,clr,ptt[0],ptt[2],flp,dragmode)) return(FALSE);
        if(RTNORM!=cmd_dimaddline(layername,clr,ptt[0],ptt[3],flp,dragmode)) return(FALSE);
    } else if (strisame(DIM_OPEN_30,fcp1)) {                     // Open 30.
        sinofang=sin(-(IC_PI/12)); cosofang=cos(-(IC_PI/12));
        cmd_ucstodim(dimzeropt,ptl[0],ptt[1],sinofang,cosofang);
        ptt[1][0]+=(dirflag)?-(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval):(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval);
        cmd_dimtoucs(dimzeropt,ptt[1],ptl[1],sinofang,cosofang);
        sinofang=sin(IC_PI/12); cosofang=cos(IC_PI/12);
        cmd_ucstodim(dimzeropt,ptl[0],ptt[2],sinofang,cosofang);
        ptt[2][0]+=(dirflag)?-(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval):(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval);
        cmd_dimtoucs(dimzeropt,ptt[2],ptl[2],sinofang,cosofang);
        ptl[3][0]+=(dirflag)?-(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval):(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval);
        for (fi1=0;fi1<4;++fi1) cmd_dimtoucs(ptdef,ptl[fi1],ptt[fi1],sinofdeg,cosofdeg);
        if(RTNORM!=cmd_dimaddline(layername,clr,ptt[0],ptt[1],flp,dragmode)) return(FALSE);
        if(RTNORM!=cmd_dimaddline(layername,clr,ptt[0],ptt[2],flp,dragmode)) return(FALSE);
        if(RTNORM!=cmd_dimaddline(layername,clr,ptt[0],ptt[3],flp,dragmode)) return(FALSE);
    } else if (strisame(DIM_DOT_BLANKED,fcp1)) {                 // Dot Blanked.
        ptl[1][0]+=(dirflag)?-((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2):((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2);
        ptl[2][0]+=(dirflag)?-(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval):(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval);
        for (fi1=0;fi1<3;++fi1) cmd_dimtoucs(ptdef,ptl[fi1],ptt[fi1],sinofdeg,cosofdeg);
        if(RTNORM!=cmd_dimaddCircle(layername,clr,ptt[0],((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2),flp,dragmode)) return(FALSE);
        if(RTNORM!=cmd_dimaddline(layername,clr,ptt[1],ptt[2],flp,dragmode)) return(FALSE);
    } else if (strisame(DIM_DOT_SMALL,fcp1)) {                   // Dot Small.
        sds_polar(ptl[0],0.0,((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/16),ptl[1]);
        sds_polar(ptl[0],IC_PI,((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/16),ptl[0]);
        sds_polar(ptl[2],((dirflag)?IC_PI:0.0),(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval),ptl[3]);
        for (fi1=0;fi1<4;++fi1) {
            cmd_dimtoucs(ptdef,ptl[fi1],ptt[fi1],sinofdeg,cosofdeg);
        }
        db_handitem *pelp=NULL;
        if(RTNORM!=cmd_dimaddPolyline(layername,plinkvar[DIMCLRD].dval.ival,1,ptl[0],1,((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2),((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2),flp,dragmode,&pelp)) return(FALSE);
        if(RTNORM!=cmd_dimaddVertex(layername,clr,ptt[0],((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2),((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2),1,flp,dragmode)) return(FALSE);
        if(RTNORM!=cmd_dimaddVertex(layername,clr,ptt[1],((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2),((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2),1,flp,dragmode)) return(FALSE);
        if(RTNORM!=cmd_dimaddSeqend(layername,clr,flp,dragmode)) return(FALSE);
        if(RTNORM!=cmd_dimaddline(layername,clr,ptt[2],ptt[3],flp,dragmode)) return(FALSE);
        sds_name ename;
		ename[0]=(long)pelp;
		ename[1]=(long)flp;
		SDS_UpdateEntDisp(ename,0);
		sds_redraw(ename,IC_REDRAW_XOR);
        delete pelp;
    } else if (strisame(DIM_BOX_FILLED,fcp1)) {                   // Box Filled.
        ic_ptcpy(pt1,ptl[0]);
        ptl[0][0]+=(dirflag)?-((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2):((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2);
        ptl[0][1]-=(dirflag)?-((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2):((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2);
        ptl[1][0]-=(dirflag)?-((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2):((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2);
        ptl[1][1]-=(dirflag)?-((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2):((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2);
        ptl[2][0]-=(dirflag)?-((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2):((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2);
        ptl[2][1]+=(dirflag)?-((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2):((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2);
        ptl[3][0]+=(dirflag)?-((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2):((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2);
        ptl[3][1]+=(dirflag)?-((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2):((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2);
        for (fi1=0;fi1<4;++fi1) cmd_dimtoucs(ptdef,ptl[fi1],ptt[fi1],sinofdeg,cosofdeg);
        if (RTNORM!=cmd_dimaddsolid(layername,clr,ptt[0],ptt[1],ptt[3],ptt[2],flp,dragmode)) return(FALSE);
        ic_ptcpy(ptl[0],pt1);
        ptl[0][0]+=(dirflag)?-((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2):((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2);
        ic_ptcpy(ptl[1],ptl[0]);
        ptl[1][0]+=(dirflag)?-((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2):((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2);
        for (fi1=0;fi1<2;++fi1) cmd_dimtoucs(ptdef,ptl[fi1],ptt[fi1],sinofdeg,cosofdeg);
        if(RTNORM!=cmd_dimaddline(layername,clr,ptt[0],ptt[1],flp,dragmode)) return(FALSE);
    } else if (strisame(DIM_BOX,fcp1)) {                         // Box.
        ic_ptcpy(pt1,ptl[0]);
        ptl[0][0]+=(dirflag)?-((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2):((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2);
        ptl[0][1]-=(dirflag)?-((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2):((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2);
        ptl[1][0]-=(dirflag)?-((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2):((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2);
        ptl[1][1]-=(dirflag)?-((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2):((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2);
        ptl[2][0]-=(dirflag)?-((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2):((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2);
        ptl[2][1]+=(dirflag)?-((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2):((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2);
        ptl[3][0]+=(dirflag)?-((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2):((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2);
        ptl[3][1]+=(dirflag)?-((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2):((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2);
        for (fi1=0;fi1<4;++fi1) cmd_dimtoucs(ptdef,ptl[fi1],ptt[fi1],sinofdeg,cosofdeg);
        if(RTNORM!=cmd_dimaddline(layername,clr,ptt[0],ptt[1],flp,dragmode)) return(FALSE);
        if(RTNORM!=cmd_dimaddline(layername,clr,ptt[1],ptt[2],flp,dragmode)) return(FALSE);
        if(RTNORM!=cmd_dimaddline(layername,clr,ptt[2],ptt[3],flp,dragmode)) return(FALSE);
        if(RTNORM!=cmd_dimaddline(layername,clr,ptt[3],ptt[0],flp,dragmode)) return(FALSE);
        ic_ptcpy(ptl[0],pt1);
        ptl[0][0]+=(dirflag)?-((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2):((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2);
        ic_ptcpy(ptl[1],ptl[0]);
        ptl[1][0]+=(dirflag)?-((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2):((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2);
        for (fi1=0;fi1<2;++fi1) cmd_dimtoucs(ptdef,ptl[fi1],ptt[fi1],sinofdeg,cosofdeg);
        if(RTNORM!=cmd_dimaddline(layername,clr,ptt[0],ptt[1],flp,dragmode)) return(FALSE);
    } else if (strisame(DIM_DATUMFILLED,fcp1)) {                 // Datum Triangle Filled.
        ptl[0][1]-=(dirflag)?-((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2):((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2);
        ptl[1][1]+=(dirflag)?-((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2):((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2);
*///        ptl[2][0]+=(dirflag)?-((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/*/2*/):((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/*/2*/);
/*        for (fi1=0;fi1<3;++fi1) cmd_dimtoucs(ptdef,ptl[fi1],ptt[fi1],sinofdeg,cosofdeg);
        if (RTNORM!=cmd_dimaddsolid(layername,clr,ptt[0],ptt[1],ptt[2],ptt[2],flp,dragmode)) return(FALSE);
    } else if (strisame(DIM_DATUMBLANK,fcp1)) {                  // Datum Triangle.
        ptl[0][1]-=(dirflag)?-((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2):((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2);
        ptl[1][1]+=(dirflag)?-((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2):((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/2);
*///        ptl[2][0]+=(dirflag)?-((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/*/2*/):((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/*/2*/);
/*        for (fi1=0;fi1<3;++fi1) cmd_dimtoucs(ptdef,ptl[fi1],ptt[fi1],sinofdeg,cosofdeg);
        if(RTNORM!=cmd_dimaddline(layername,clr,ptt[0],ptt[1],flp,dragmode)) return(FALSE);
        if(RTNORM!=cmd_dimaddline(layername,clr,ptt[1],ptt[2],flp,dragmode)) return(FALSE);
        if(RTNORM!=cmd_dimaddline(layername,clr,ptt[2],ptt[0],flp,dragmode)) return(FALSE);
    } else if (strisame(DIM_INTEGRAL,fcp1)) {                    // Integral.
        sinofang=sin(IC_PI/3); cosofang=cos(IC_PI/3);
        cmd_ucstodim(dimzeropt,ptl[0],ptt[1],sinofang,cosofang);
        ptt[1][0]+=(dirflag)?-((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/4):((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/4);
        cmd_dimtoucs(dimzeropt,ptt[1],ptl[1],sinofang,cosofang);
        sinofang=sin(IC_PI/6); cosofang=cos(IC_PI/6);
        cmd_ucstodim(dimzeropt,ptl[1],ptt[1],sinofang,cosofang);
        ptt[1][0]+=(dirflag)?-((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/4):((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/4);
        cmd_dimtoucs(dimzeropt,ptt[1],ptl[2],sinofang,cosofang);
        sinofang=sin((IC_PI)+(IC_PI/3)); cosofang=cos((IC_PI)+(IC_PI/3));
        cmd_ucstodim(dimzeropt,ptl[0],ptt[1],sinofang,cosofang);
        ptt[1][0]+=(dirflag)?-((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/4):((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/4);
        cmd_dimtoucs(dimzeropt,ptt[1],ptl[3],sinofang,cosofang);
        sinofang=sin((IC_PI)+(IC_PI/6)); cosofang=cos((IC_PI)+(IC_PI/6));
        cmd_ucstodim(dimzeropt,ptl[3],ptt[1],sinofang,cosofang);
        ptt[1][0]+=(dirflag)?-((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/4):((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)/4);
        cmd_dimtoucs(dimzeropt,ptt[1],pt1,sinofang,cosofang);
        for (fi1=0;fi1<4;++fi1) cmd_dimtoucs(ptdef,ptl[fi1],ptt[fi1],sinofdeg,cosofdeg);
        if(RTNORM!=cmd_dimaddline(layername,clr,ptt[0],ptt[1],flp,dragmode)) return(FALSE);
        if(RTNORM!=cmd_dimaddline(layername,clr,ptt[1],ptt[2],flp,dragmode)) return(FALSE);
        if(RTNORM!=cmd_dimaddline(layername,clr,ptt[0],ptt[3],flp,dragmode)) return(FALSE);
        cmd_dimtoucs(ptdef,pt1,ptt[0],sinofdeg,cosofdeg);
        if(RTNORM!=cmd_dimaddline(layername,clr,ptt[3],ptt[0],flp,dragmode)) return(FALSE);
        // Make the line from the extension line the the end of the dimension line.
        ic_ptcpy(ptl[2],ptl[4]);
        ic_ptcpy(ptl[3],ptl[4]);
        ptl[3][0]+=(dirflag)?-(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval):(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval);
        for (fi1=2;fi1<4;++fi1) cmd_dimtoucs(ptdef,ptl[fi1],ptt[fi1],sinofdeg,cosofdeg);
        if(RTNORM!=cmd_dimaddline(layername,clr,ptt[2],ptt[3],flp,dragmode)) return(FALSE);
    }
*/
    return(TRUE);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DIM_INITIALIZE
//
// If the plink struct has not been allocated then this function allocates and zeroes out the structure.
//
// return : void
//
void cmd_initialize(bool bReset /* = false */) 
{
    if (!plink)
	{
        plink=(struct cmd_dimeparlink *)new (struct cmd_dimeparlink);
        memset(plink,(int)0,(sizeof(struct cmd_dimeparlink))-4);
        plink->BeginDrag=TRUE;
		plink->dimtfac = 1.0; 
    }
	if(bReset)
	{
        memset(plink,(int)0,(sizeof(struct cmd_dimeparlink))-4);
		//BugZilla No. 78217; 12-08-2002
		plink->BeginDrag=TRUE;
		plink->dimtfac = 1.0; 
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DIMADDINSERT
//
//
//
// return : RTERROR if fail.
//        : RTNORM  ok.
//
int cmd_dimaddinsert(char *szLayer,short sColor,short sLweight, sds_point pt10, sds_real rScale, sds_real gc_50, char *Iname,db_drawing *pFlp,short dragmode) {
    struct resbuf *pEed=NULL;
    struct resbuf *pRb=NULL,
                  *pRbtmp1=NULL,
                   rbucs,
                   rbwcs,
                   rbecs;
    int            ret = 0;
    sds_point      pt1 = {0.0, 0.0, 0.0},
				   pt3 = {0.0, 0.0, 0.0},
                   pt210 = {0.0, 0.0, 1.0};

    setColor(&sColor);

    sds_real xScale, yScale, zScale;
    xScale = yScale = zScale =rScale*plinkvar[DIMSCALE].dval.rval;

    pt210[0]=pt210[1]=0.0;pt210[2]=1.0;
    cmd_initialrbcs(&rbecs,&rbwcs,&rbucs,pFlp,plink->pt210);
    
    pt3[0]=cos(gc_50); pt3[1]=sin(gc_50); pt3[2]=0.0;

    if (dragmode==-1) return RTNORM;
    if (dragmode)
	{
        db_handitem *elp;
        sds_name     ename;
        elp=new CDbMText(pFlp);
        elp->set_6(DIM_BYBLOCK);
        elp->set_8(szLayer);
        elp->set_370(sLweight);
        elp->set_41(xScale);
        elp->set_42(yScale);
        elp->set_43(zScale);
        elp->set_62(sColor);
        elp->set_10(pt10);
        elp->set_2(Iname);

		ename[0]=(long)elp;
		ename[1]=(long)pFlp;
		SDS_UpdateEntDisp(ename,0);
		sds_redraw(ename,IC_REDRAW_XOR);
		delete elp;
    }
	else 
	{
        sds_trans(pt10,&rbucs,&rbecs,0,pt1); // pt1[2]=0.0;

		// Calculate the insert angle.
        sds_trans(pt3,&rbucs,&rbecs,1,pt3);
        if(fabs(pt3[0])>CMD_FUZZ || fabs(pt3[1])>CMD_FUZZ) gc_50=atan2(pt3[1],pt3[0]);
		

		if ((pRb=sds_buildlist(RTDXF0,"INSERT"/*DNT*/,8,szLayer,62,sColor,370,sLweight,10,pt1,41,xScale,42,yScale,43,zScale,50, gc_50, 2,Iname,6,DIM_BYBLOCK,210,pt210,0))==NULL) return(RTERROR);
        // if there is eed then append it to the end of the list
        if (pEed){
            for (pRbtmp1=pRb;pRbtmp1->rbnext!=NULL;pRbtmp1=pRbtmp1->rbnext);
            pRbtmp1->rbnext=pEed;
        }
        // entmake the object into the drawing
        if ((ret=SDS_entmake(pRb,pFlp))!=RTNORM) { IC_RELRB(pRb); return(RTERROR); }
        IC_RELRB(pRb);
	}
    return RTNORM;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DIMADDPOINT
//
// If (dragmode)==1 then the dimension is being dragged and the point is not part of a block.
// If (dragmode)==0 then the dimension is being created and the point is part of a block definition.
//
// return : RTERROR if fail.
//        : RTNORM  ok.
//
int cmd_dimaddpoint(char *szLayer,short sColor,short sLweight, sds_point pt10,db_drawing *pFlp,short dragmode) {
    struct resbuf  *pEed=NULL;
    struct resbuf  *pRb=NULL,
                   *pRbtmp1=NULL,
                    rbucs,
                    rbecs,
                    rbwcs;
    int             ret = 0;
    sds_point       pt1 = {0.0, 0.0, 0.0},
                    pt2 = {0.0, 0.0, 0.0},
                    pt210 = {0.0, 0.0, 1.0};
    sds_real        fr1 = 0.0;
    struct gr_view *view=SDS_CURGRVW;

//    setColor(&sColor);

    if (dragmode==-1) return RTNORM;
    db_ucshelper(dimucs[0],dimucs+1,dimelev,pt1,pt2,&fr1);
    cmd_initialrbcs(&rbecs,&rbwcs,&rbucs,pFlp,plink->pt210);
    pt210[0]=pt210[1]=0.0;pt210[2]=1.0;
	if (dragmode) {
		db_handitem    *elp;
        sds_name        ename;
        // If dragmode is 1 then transform the point from (UCS) to (WCS This is the Native format of the entity). This is becouse
        // the point is not going to be part of a block definition when it is being dragged.
        sds_trans(pt10,&rbucs,&rbwcs,0,pt1);

        elp=new db_point(pFlp);
        elp->set_6(DIM_BYBLOCK);
        elp->set_8(szLayer,pFlp);
        elp->set_62(sColor);
        elp->set_370(sLweight);
        elp->set_10(pt1);
        elp->set_50(fr1);
        elp->set_210(view->ucsaxis[2]);
		elp->set_39(0.0);
	    ename[0]=(long)elp;
	    ename[1]=(long)pFlp;
	    SDS_UpdateEntDisp(ename,0);
	    sds_redraw(ename,IC_REDRAW_XOR);
        delete elp;
	} else {
        if (plinkvar[DIMASO].dval.ival) {
            sds_trans(pt10,&rbucs,&rbecs,0,pt1); // pt1[2]=0.0;
			if ((pRb=sds_buildlist(RTDXF0,"POINT"/*DNT*/,8,szLayer,62,sColor,370,sLweight,10,pt1,50,fr1,210,pt210,6,DIM_BYBLOCK,39,0.0,0))==NULL) return(RTERROR);
        } else {
			if (sColor==0) sColor=256;
            sds_trans(pt10,&rbucs,&rbwcs,0,pt1);
			if ((pRb=sds_buildlist(RTDXF0,"POINT"/*DNT*/,8,szLayer,62,sColor,370,sLweight,10,pt1,6,DIM_BYBLOCK,39,0.0,0))==NULL) return(RTERROR);
        }
        // if there is eed then append it to the end of the list
        if (pEed){
            for (pRbtmp1=pRb;pRbtmp1->rbnext!=NULL;pRbtmp1=pRbtmp1->rbnext);
            pRbtmp1->rbnext=pEed;
        }
        // entmake the object into the drawing
        if ((ret=SDS_entmake(pRb,pFlp))!=RTNORM){ IC_RELRB(pRb); return(RTERROR); }
        IC_RELRB(pRb);
	}
    return RTNORM;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DIMADDLINE
//
//
// If (dragmode)==1 then the dimension is being dragged and the line is not part of a block.
// If (dragmode)==0 then the dimension is being created and the line is part of a block definition.
//
// return : RTERROR if fail.
//        : RTNORM  ok.
//
int cmd_dimaddline(char *szLayer, short sColor, short sLweight, sds_point pt10, sds_point pt11, db_drawing *pFlp, short dragmode, short flags ) {
    struct resbuf *pEed=NULL;
    struct resbuf *pRb=NULL,
                  *pRbtmp1=NULL,
                   rbucs,
                   rbwcs,
                   rbecs;
    int            ret = 0;
    sds_point      pt1 = {0.0, 0.0, 0.0},
                   pt2 = {0.0, 0.0, 0.0},
                   pt210 = {0.0, 0.0, 1.0};
	char	      *linetype=DIM_BYBLOCK;

	ASSERT((flags & DIM_ADD_TO_BLOCK) || (flags == 0));
    if (dragmode==-1) return RTNORM;
//    setColor(&sColor);

    pt210[0]=pt210[1]=0.0;pt210[2]=1.0;
    cmd_initialrbcs(&rbecs,&rbwcs,&rbucs,pFlp,plink->pt210);
	if (dragmode) {
		db_handitem *elp;
        sds_name     ename;

        // If dragmode is 1 then transform the point from (UCS) to (WCS This is the Native format of the entity). This is becouse
        // the line is not going to be part of a block definition when it is being dragged.
        sds_trans(pt10,&rbucs,&rbwcs,0,pt1);
        sds_trans(pt11,&rbucs,&rbwcs,0,pt2);

        elp=new db_line(pFlp);
        elp->set_6(DIM_BYBLOCK);
        elp->set_8(szLayer);
        elp->set_62(sColor);
        elp->set_370(sLweight);
        elp->set_10(pt1);
        elp->set_11(pt2);
		elp->set_39(0.0);
	    ename[0]=(long)elp;
	    ename[1]=(long)pFlp;
	    SDS_UpdateEntDisp(ename,0);
	    sds_redraw(ename,IC_REDRAW_XOR);
        delete elp;
	} else {
		if(flags & DIM_ADD_TO_BLOCK)
		{
			ic_ptcpy(pt1, pt10);
			ic_ptcpy(pt2, pt11);
		}
		else
	        if (plinkvar[DIMASO].dval.ival) {
		        sds_trans(pt10,&rbucs,&rbecs,0,pt1); // pt1[2]=0.0;
			    sds_trans(pt11,&rbucs,&rbecs,0,pt2); // pt2[2]=0.0;
			} else {
				sds_trans(pt10,&rbucs,&rbwcs,0,pt1);
				sds_trans(pt11,&rbucs,&rbwcs,0,pt2);
				linetype=DIM_BYLAYER;
			}
        
		if ((pRb=sds_buildlist(RTDXF0,db_hitype2str(DB_LINE),8,szLayer,62,sColor,370,sLweight,10,pt1,11,pt2,210,pt210,6,linetype,39,0.0,0))==NULL) return(RTERROR);
        
        // if there is eed then append it to the end of the list
        if (pEed){
            for (pRbtmp1=pRb;pRbtmp1->rbnext!=NULL;pRbtmp1=pRbtmp1->rbnext);
            pRbtmp1->rbnext=pEed;
        }
        // entmake the object into the drawing
        if ((ret=SDS_entmake(pRb,pFlp))!=RTNORM) { IC_RELRB(pRb); return(RTERROR); }
        IC_RELRB(pRb);
	}
    return RTNORM;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DIMOVERRIDE
//
//
//
//
//
//
// Modified Cybage VSB  04/03/2002 DD/MM/YYYY [
// Reference: Bug 77973 from Bugzilla
typedef struct tagOverrideNode 
{	
	int		  DXF;
	short     type;		
	union {
	sds_real  rval;		
	int       ival;			
	char      *cval;
	}dval;
	struct tagOverrideNode *Next;
	
	tagOverrideNode( struct cmd_dimvars * pDimVar)
	{		
		DXF		= pDimVar->DXF;
		type	= pDimVar->type;
		Next	= NULL;
		dval.cval	= NULL;
		switch(pDimVar->type) {
		case RTREAL:
			dval.rval	=	pDimVar->dval.rval;
			break;
		case RTSHORT:
			dval.ival	=	pDimVar->dval.ival;
			break;
		case RTSTR:                    
			dval.cval	=	(char *)new char[strlen(pDimVar->dval.cval)+1];
			strcpy(dval.cval,pDimVar->dval.cval);
			break;
		}
		
	}
	~tagOverrideNode()
	{
		if ( type == RTSTR && dval.cval )
		{
			//Bugzilla No. 78104 ; 15-04-2002 
			delete[] dval.cval;
		}
	}

}OVERRIDE_NODE;

void AddToDimOverrideList(OVERRIDE_NODE **pDimOverrideList,struct cmd_dimvars * pDimVariable)
{	
	//check if the dimvariable is already present in DimOverridelist
	bool bfound = false;
	for( OVERRIDE_NODE *pOverrideNode = *pDimOverrideList; pOverrideNode != NULL; pOverrideNode = pOverrideNode->Next)
	{
		if ( pOverrideNode->DXF == pDimVariable->DXF )
		{
			bfound = true;
			break;
		}
	}	
	
	if ( bfound == false)
	{
		OVERRIDE_NODE *pOriginaList = *pDimOverrideList;			
		*pDimOverrideList = new OVERRIDE_NODE(pDimVariable);
		(*pDimOverrideList)->Next = pOriginaList;
		
	}
	else
	{
		if ( pOverrideNode == NULL )
			return;

		switch(pOverrideNode->type) 
		{
		case RTREAL:
			pOverrideNode->dval.rval=pDimVariable->dval.rval;
			break;
		case RTSHORT:
			pOverrideNode->dval.ival=pDimVariable->dval.ival;
			break;
		case RTSTR:                    
			if ( pOverrideNode->dval.cval )
			{
				//Bugzilla No. 78104 ; 15-04-2002 
				delete[] pOverrideNode->dval.cval;
				pOverrideNode->dval.cval = NULL;
			}
			pOverrideNode->dval.cval=(char *)new char[strlen(pDimVariable->dval.cval)+1];
			strcpy(pOverrideNode->dval.cval,pDimVariable->dval.cval);
			break;
		}		
	}
}

void RemoveDimOverrideList(OVERRIDE_NODE **pDimOverrideList )
{	
	OVERRIDE_NODE *pOverrideNodeDelete = NULL;
	for( OVERRIDE_NODE *pOverrideNode = *pDimOverrideList; pOverrideNode != NULL; )
	{
		pOverrideNodeDelete = pOverrideNode;
		pOverrideNode = pOverrideNode->Next;		
		delete pOverrideNodeDelete;
	}	
}

struct resbuf * GetNewexlist(OVERRIDE_NODE * pDimOverrideList,struct resbuf * exAcadlist)
{
	struct resbuf  *newexlist = NULL;
	
	if ( exAcadlist != NULL )
	{
		for( OVERRIDE_NODE *pOverrideNode = pDimOverrideList; pOverrideNode != NULL; pOverrideNode = pOverrideNode->Next)
		{
			struct resbuf* temp			= NULL;			
			struct resbuf* pPreviousrb  = NULL;			
			
			for(temp = exAcadlist; temp ; temp = temp->rbnext)
			{
				if( temp->restype == AD_XD_STRING && strisame(temp->resval.rstring, DIM_DSTYLE ))
					break;				
				if( temp->restype == AD_XD_APPOBJHANDLE && !strisame(temp->resval.rstring, DIM_ACADAPP))	
				{
					temp = NULL;
					break;
				}
			}
			if ( temp == NULL )
			{
				struct resbuf *rb = exAcadlist->rbnext;
				struct resbuf *rbb = NULL;

				rbb = exAcadlist->rbnext=sds_newrb(AD_XD_STRING); 				
				//Bugzilla No. 78104 ; 15-04-2002 
				rbb->resval.rstring= new char [strlen(DIM_DSTYLE)+1]; 
				strcpy(rbb->resval.rstring,DIM_DSTYLE);
				
				rbb->rbnext=sds_newrb(AD_XD_CONTROL);
				rbb=rbb->rbnext; 
				//Bugzilla No. 78104 ; 15-04-2002 
				rbb->resval.rstring= new char [2]; 
				strcpy(rbb->resval.rstring,"{"/*DNT*/ );
				
				// Add the closing of the list.
				rbb->rbnext=sds_newrb(AD_XD_CONTROL); 
				rbb=rbb->rbnext; 
				//Bugzilla No. 78104 ; 15-04-2002 
				rbb->resval.rstring=new char [2]; 
				strcpy(rbb->resval.rstring,"}"/*DNT*/ );			
				
				rbb->rbnext = rb;
				break;
				
			}
			for(temp = temp->rbnext; temp && temp->restype != AD_XD_CONTROL; temp = temp->rbnext);
			if(strcmp(temp->resval.rstring, "{"/*DNT*/) == 0)
			{
				pPreviousrb = temp;
				for(temp = temp->rbnext; temp && temp->restype != AD_XD_CONTROL; pPreviousrb = temp,temp = temp->rbnext)
				{
					if ( temp->restype == AD_XD_INT &&  temp->resval.rint == pOverrideNode->DXF )							
					{	
						struct resbuf * rbDel = NULL;
						rbDel = temp; 
						temp= temp->rbnext;
						pPreviousrb->rbnext = temp->rbnext;
						temp->rbnext = NULL;
						sds_relrb(rbDel);
						break;
					}					
					else // Bugzilla No: 77973 ; 25-03-2002 
					{
							temp = temp->rbnext;
					}
				}	
			}			
			
		}
	}
	else	
	{
		// Make a new ACAD xdata list
		resbuf  *rbb = NULL;
		//exlist=rbb=sds_newrb(-3);
		exAcadlist=rbb=sds_newrb(AD_XD_APPOBJHANDLE); 		
		//Bugzilla No. 78104 ; 15-04-2002 
		rbb->resval.rstring= new char [strlen(DIM_ACADAPP)+1];
		strcpy(rbb->resval.rstring,DIM_ACADAPP);
		
		rbb->rbnext=sds_newrb(AD_XD_STRING); 
		rbb=rbb->rbnext; 
		//Bugzilla No. 78104 ; 15-04-2002 
		rbb->resval.rstring= new char [strlen(DIM_DSTYLE)+1]; 
		strcpy(rbb->resval.rstring,DIM_DSTYLE);
		
		rbb->rbnext=sds_newrb(AD_XD_CONTROL);
		rbb=rbb->rbnext; 
		//Bugzilla No. 78104 ; 15-04-2002 
		rbb->resval.rstring= new char [2]; 
		strcpy(rbb->resval.rstring,"{"/*DNT*/ );
		
		// Add the closing of the list.
		rbb->rbnext=sds_newrb(AD_XD_CONTROL); 
		rbb=rbb->rbnext; 
		//Bugzilla No. 78104 ; 15-04-2002 
		rbb->resval.rstring= new char [2]; 
		strcpy(rbb->resval.rstring,"}"/*DNT*/ );			
		
	}
	
	newexlist = exAcadlist;
	
	struct resbuf* rb = NULL;
	for(rb  = newexlist; rb; rb = rb->rbnext)
	{
		if( rb->restype == AD_XD_STRING && strisame(rb->resval.rstring, DIM_DSTYLE))
			break;				
		if( rb->restype == AD_XD_APPOBJHANDLE  && !strisame(rb->resval.rstring, DIM_ACADAPP))	
		{
			rb = NULL;
			break;
		}
	}
	if(rb && strcmp(rb->resval.rstring, DIM_DSTYLE ) == 0)
	{
		for( rb = rb->rbnext; rb && rb->restype != AD_XD_CONTROL; rb = rb->rbnext);	
		if(strcmp(rb->resval.rstring, "{"/*DNT*/) == 0)	
		{
			struct resbuf* exlist_remaining_part = NULL;
			exlist_remaining_part = rb->rbnext;
			
			for( OVERRIDE_NODE *pOverrideNode = pDimOverrideList; pOverrideNode != NULL; pOverrideNode = pOverrideNode->Next)
			{
				
				switch(pOverrideNode->type) 
				{
				case RTSTR:
					{
						char szTableName[6] = "";
						
						if(pOverrideNode->DXF == 341 )
							strcpy(szTableName,"BLOCK");
						else if(pOverrideNode->DXF == 340 )
							strcpy(szTableName,"STYLE");

						rb->rbnext = sds_newrb(AD_XD_INT);
						rb = rb->rbnext;
						rb->resval.rint = pOverrideNode->DXF;						
						
						if(pOverrideNode->DXF == 341 || pOverrideNode->DXF == 340 )
						{
							rb->rbnext = sds_newrb(1005);
							rb = rb->rbnext;
							if ( strsame(pOverrideNode->dval.cval,"") )
							{
								//Bugzilla No. 78104 ; 15-04-2002 
								rb->resval.rstring = new char [2];
								strcpy(rb->resval.rstring, "");
							}
							else
							{
								struct resbuf *tmprb = sds_tblnext(szTableName, 1);
								while (tmprb)
								{
									if (ic_assoc(tmprb, 2) == 0)
									{
										if (stricmp(ic_rbassoc->resval.rstring, pOverrideNode->dval.cval) == 0)
										{
											if (ic_assoc(tmprb, 5) == 0)
											{
												//Bugzilla No. 78104 ; 15-04-2002 
												rb->resval.rstring = new char [strlen(ic_rbassoc->resval.rstring)+1];
												strcpy(rb->resval.rstring, ic_rbassoc->resval.rstring);	
												IC_RELRB(tmprb);											
												break;
											}
										}
									}								
									IC_RELRB(tmprb);
									tmprb = sds_tblnext(szTableName, 0);
								}
							}
						}
						else
						{
							rb->rbnext = sds_newrb(AD_XD_STRING);
							rb = rb->rbnext;
							//Bugzilla No. 78104 ; 15-04-2002 
							rb->resval.rstring = new char [strlen(pOverrideNode->dval.cval)+1];
							strcpy(rb->resval.rstring, pOverrideNode->dval.cval);
						}
					}						
					break;
					
				case RTREAL:
					{
						rb->rbnext = sds_newrb(AD_XD_INT);
						rb = rb->rbnext;
						rb->resval.rint = pOverrideNode->DXF;
						
						rb->rbnext = sds_newrb(AD_XD_REAL);
						rb = rb->rbnext; 
						rb->resval.rreal = pOverrideNode->dval.rval;
						break;
					}
				case RTSHORT:				
					{
						rb->rbnext = sds_newrb(AD_XD_INT);
						rb = rb->rbnext;
						rb->resval.rint = pOverrideNode->DXF;
						
						rb->rbnext = sds_newrb(AD_XD_INT);
						rb = rb->rbnext; 
						rb->resval.rint = pOverrideNode->dval.ival;
						break;
					}
				}
			}
			
			rb->rbnext  = exlist_remaining_part;
		}
	}
	return newexlist;
}

// Modified Cybage VSB  04/03/2002 DD/MM/YYYY ]

short cmd_doverride(db_drawing *flp) {			//  TODO - Make it recognize "VARNAME" or "DIMVARNAME"
    if (cmd_iswindowopen() == RTERROR) return RTERROR;
    char           fs1[IC_ACADBUF] = ""/*DNT*/,
                   fs2[IC_ACADBUF] = ""/*DNT*/,
                   varname[IC_ACADBUF] = ""/*DNT*/;
    struct resbuf  setgetrb,
                  *rb=NULL,
                  *elist=NULL;
    sds_name       ename = {0L, 0L},
                   ss1 = {0L, 0L};
    long           ssct = 0L,
                   rc = 0L;
    sds_real       fr1 = 0.0;
    int            ret = 0,
                   fi1 = 0,
                   fi2 = 0;
    short          initflg = 1;


	// Modified Cybage VSB  04/03/2002 DD/MM/YYYY [
	// Reference: Bug 77973 from Bugzilla
	OVERRIDE_NODE *pDimOverrideList = NULL;
	struct resbuf *newexlist		= NULL;
	struct resbuf *exlist			= NULL;
	struct db_sysvarlink *svp1		= NULL;
	// Modified Cybage VSB  04/03/2002 DD/MM/YYYY ]
    if (sds_ssadd(NULL,NULL,ss1)!=RTNORM) goto exit;
    for (;;) {
        if (initflg) {		// TODO - Should be an initget with "Clear"
            if (sds_initget(SDS_RSG_OTHER,ResourceString(IDC_DIMENSIONS_INITGET_CLEAR_0, "Clear ~_Clear" ))!=RTNORM) goto exit;
            if ((ret=sds_getkword(ResourceString(IDC_DIMENSIONS__NCLEAR_OVERR_1, "\nClear overrides/<Dimension variable to override>: " ),fs1))==RTERROR) goto exit;
        } else {
            if (sds_initget(SDS_RSG_OTHER,""/*DNT*/)!=RTNORM) goto exit;
            if ((ret=sds_getkword(ResourceString(IDC_DIMENSIONS__NDIMENSION_V_2, "\nDimension variable to override: " ),fs1))==RTERROR) goto exit;
        }
        strupr(fs1);
        if (ret==RTERROR || ret==RTCAN) goto exit;
        if (!*fs1) { if (initflg) goto exit; break; }
        if (initflg && strnisame("CLEAR"/*DNT*/,fs1,strlen(fs1))) {
			// Added Cybage VSB  25/02/2002 DD/MM/YYYY [
			// Reference: Bug 77949 from Bugzilla
            //cmd_setparlink(flp);
			break;
			// Added Cybage VSB  25/02/2002 DD/MM/YYYY ]
        } else {
            if (!strstr(fs1,"DIM"/*DNT*/ )) {
                sprintf(varname,"DIM%s"/*DNT*/ ,fs1);
            } else strcpy(varname,fs1);
            if (sds_getvar(varname,&setgetrb)!=RTNORM) {
                sds_printf(ResourceString(IDC_DIMENSIONS__NVARIABLE___6, "\nVariable \"%s\" is not recognized." ),varname);       
                continue;
            }

			// Modified Cybage VSB  18/12/2001 DD/MM/YYYY [
			// Reference: Bug 77945 from Bugzilla
			// Description: IntelliCAD shows wrong prompt and does not display current values set during Dimoverride command    

			// Search for the entered variable in the plinkvar list.
            for (fi2=0;fi2<DIM_LASTDIMVAR;++fi2) { if (strsame(plinkvar[fi2].Var,varname)) break; }
            if (fi2==DIM_LASTDIMVAR) continue;

            initflg=0;
			// Modified Cybage VSB  04/03/2002 DD/MM/YYYY [
			// Reference: Bug 77973 from Bugzilla
			if ( (svp1 = db_findsysvar(varname,-1,SDS_CURDWG)) == NULL )
			{
				cmd_ErrorPrompt(CMD_ERR_UNRECOGVAR,0);
				continue;
			}
AskAgain :
            switch ( setgetrb.restype )
			{
				case RTSTR :					
					if ( strisame(varname,"DIMBLK"/*DNT*/ ) || strisame(varname,"DIMBLK1"/*DNT*/) ||
						 strisame(varname,"DIMBLK2"/*DNT*/) || strisame(varname,"DIMLDRBLK"/*DNT*/) )
					{
						sprintf(fs2,ResourceString(IDC_DIMENSIONS__S_IS_NOW__S__8, "\n%s is now <\"%s\">.  New value: " ),varname,plinkvar[fi2].dval.cval!=NULL && strlen(plinkvar[fi2].dval.cval)==0?ResourceString(IDC_COMBO_LDR_ARROWS_4, "Arrow, filled"):plinkvar[fi2].dval.cval);
					}
					else
						sprintf(fs2,ResourceString(IDC_DIMENSIONS__S_IS_NOW__S__8, "\n%s is now <\"%s\">.  New value: " ),varname,plinkvar[fi2].dval.cval==NULL?"":plinkvar[fi2].dval.cval);
					ret=sds_getstring((svp1->spl & IC_SYSVARFLAG_FILEPATH) ? 1 : 0,fs2,fs1);
					if ( ret==RTERROR && !*fs1 )
						goto AskAgain;
					if ( ret==RTNORM && strlen(fs1) ==0 && plinkvar[fi2].dval.cval != NULL )
						strcpy(fs1,plinkvar[fi2].dval.cval);
					break;
				case RTSHORT :										
					// Modified Cybage VSB  11/03/2002 DD/MM/YYYY [
					// Reference: Bug 78024 from Bugzilla
					// Description: DIMCLRD,DIMCLRE,DIMCLRT does not accepts color string like red, blue    
					if ( strisame(varname,"DIMCLRD"/*DNT*/ ) || strisame(varname,"DIMCLRE"/*DNT*/ ) ||
						strisame(varname,"DIMCLRT"/*DNT*/ ) ) 
					{						
						char colorstr[IC_ACADBUF];
						if ( plinkvar[fi2].dval.ival == 0 || plinkvar[fi2].dval.ival == 256 )						
							sprintf(fs2,ResourceString(IDC_DIMENSIONS__S_IS_NOW__S__8, "\n%s is now <\"%s\">.  New value: " ),varname,strupr(ic_colorstr(plinkvar[fi2].dval.ival,0)));
						else
							if ( plinkvar[fi2].dval.ival >= 1 && plinkvar[fi2].dval.ival <= 7 )
							{
								sprintf(colorstr,"%d (%s)",plinkvar[fi2].dval.ival,strlwr(ic_colorstr(plinkvar[fi2].dval.ival,0)));
								sprintf(fs2,ResourceString(IDC_DIMENSIONS__S_IS_NOW__S__8, "\n%s is now <\"%s\">.  New value: " ),varname,colorstr);
							}
							else								
								sprintf(fs2,ResourceString(IDC_DIMENSIONS__S_IS_NOW__D__9, "\n%s is now <%d>.  New value: " ),varname,plinkvar[fi2].dval.ival);
							
							ret=sds_getstring(0,fs2,fs1);							
							if ( ret == RTNORM )
							{
								if ( strlen(fs1) == 0 )
									fi1 = plinkvar[fi2].dval.ival;
								else
								{
									int iColor= ic_colornum(fs1);
									if ( iColor == 0 && atoi(fs1)==0 && !strnisame(fs1,"0"/*DNT*/,strlen(fs1)) && !strnisame(fs1,"BYBLOCK"/*DNT*/,strlen(fs1)) ||
										iColor == 7 && atoi(fs1)!=7 && !strnisame(fs1,ResourceString(IDC_LAYER_WHITE_10, "WHITE" ) ,strlen(fs1))
										)
									{
										cmd_ErrorPrompt(CMD_ERR_COLOR0256,0);
										goto AskAgain;
									}
									else
										plinkvar[fi2].dval.ival = fi1 = iColor;
								}
							}
					}
					else
					{
						if (strsame(svp1->range,"0_1"/*DNT*/)) 
						{
							if ( plinkvar[fi2].dval.ival == 1 )
								sprintf(fs2,ResourceString(IDC_DIMENSIONS__S_IS_NOW__S__8, "\n%s is now <\"%s\">.  New value: " ),varname,"On");
							else
								sprintf(fs2,ResourceString(IDC_DIMENSIONS__S_IS_NOW__S__8, "\n%s is now <\"%s\">.  New value: " ),varname,"Off");
						}
						else
							sprintf(fs2,ResourceString(IDC_DIMENSIONS__S_IS_NOW__D__9, "\n%s is now <%d>.  New value: " ),varname,plinkvar[fi2].dval.ival);
						if (strsame(svp1->range,"0_1"/*DNT*/)) {
							sds_initget(0, ResourceString(IDC_SETVAR_0_1_INIT1, "On|ON Off|OFf ~_ON ~_OFF"));
						} else {
							sds_initget(0,NULL);
						}
						ret=sds_getint(fs2,&fi1);					
						if (ret == RTKWORD) 
						{				       
							sds_getinput(fs2);
							if (strisame(fs2,"ON" )) fi1=1;
							if (strisame(fs2,"OFf" )) fi1=0;				        
						}
					}					
					if ( ret == RTNONE )
						fi1 = plinkvar[fi2].dval.ival;
					break;
					// Modified Cybage VSB  11/03/2002 DD/MM/YYYY ]
				case RTREAL :
					if (sds_rtos(plinkvar[fi2].dval.rval,-1,-1,fs1)!=RTNORM)
						goto exit;
                sprintf(fs2,ResourceString(IDC_DIMENSIONS__S_IS_NOW__S__8, "\n%s is now <\"%s\">.  New value: " ),varname,fs1);
					ret=sds_getreal(fs2,&fr1);		
					if ( ret == RTNONE )
						fr1 = plinkvar[fi2].dval.rval;
					break;
            }
			// Modified Cybage VSB  04/03/2002 DD/MM/YYYY ]

            if (ret==RTERROR || ret==RTCAN) goto exit;

			// Modified Cybage VSB  18/12/2001 DD/MM/YYYY ]
            
            // Added Cybage VSB  18/12/2001 DD/MM/YYYY [
			// Reference: Bug 77946 from Bugzilla
			// Description: IntelliCAD does not provide proper validation for Block Name during DOV command

			if ((strisame(varname,"DIMBLK"/*DNT*/ ) || strisame(varname,"DIMBLK1"/*DNT*/) ||
				strisame(varname,"DIMBLK2"/*DNT*/) || strisame(varname,"DIMLDRBLK"/*DNT*/))
				&& fs1[0] != 0) 
			{
			    // Added Cybage VSB  04/03/2002 DD/MM/YYYY 
				// Reference: Bug 77973 from Bugzilla
				if (fs1[0]=='.' && fs1[1]==0) 
					fs1[0]=0;
				
				if (isArrowBlock(fs1))
				{
					if (fs1[0] != '_') // add under score to block name
					{
						char temp[IC_ACADBUF];
						strncpy(temp, fs1, sizeof(temp));
						sprintf(fs1, "_%s", temp);
					}
					struct resbuf rbb;
					if (sds_getvar("CLAYER" ,&rbb) == RTNORM) {
						db_astrncpy(&layername, rbb.resval.rstring, strlen(rbb.resval.rstring) + 1);
					} else {
						db_astrncpy(&layername,"0",3);
					}
					checkBlock(fs1, SDS_CURDWG);
				} 
				
				if (fs1[0]) 
				{
					struct resbuf *tmp=NULL;
					if ((tmp=sds_tblsearch("BLOCK"/*DNT*/,fs1,0))==NULL){
						cmd_ErrorPrompt(CMD_ERR_FINDBLOCK,0);
						goto AskAgain;
					}
					IC_RELRB(tmp);
				}
			}
			else
			{
				// Added Cybage VSB  09/09/2002 DD/MM/YYYY 
				// Reference: Bug 78277 from Bugzilla
				if ( (strisame(varname,"DIMPOST"/*DNT*/) || strisame(varname,"DIMAPOST"/*DNT*/) ) && fs1[0]=='.' && fs1[1]==0  )				
					fs1[0]=0;

				struct resbuf setrb;
				setrb.restype = setgetrb.restype;
				switch(setgetrb.restype) {
                case RTREAL:
                    setrb.resval.rreal =fr1;
                    break;
                case RTSHORT:
                    setrb.resval.rint=fi1;
                    break;
                case RTSTR:                    
                    //Bugzilla No. 78104 ; 15-04-2002 
					setrb.resval.rstring= new char [strlen(fs1)+1];
                    strcpy(setrb.resval.rstring,fs1);
                    break;
				}
				if ( sds_setvar(plinkvar[fi2].Var,&setrb) != RTNORM )
				{
					if (setrb.restype==RTSTR) IC_FREE(setrb.resval.rstring);
					cmd_ErrorPrompt(CMD_ERR_SYSVARVAL,0);
					goto AskAgain;
				}
				else
					sds_setvar(plinkvar[fi2].Var,&setgetrb);
				
				if (setrb.restype==RTSTR) IC_FREE(setrb.resval.rstring);
			}
			// Added Cybage VSB  18/12/2001 DD/MM/YYYY ]

            plinkvar[fi2].dval.oride=TRUE;
            // Update the plinkvar list with the new value.
            switch(setgetrb.restype) {
                case RTREAL:
                    plinkvar[fi2].dval.rval=fr1;
                    break;
                case RTSHORT:
                    plinkvar[fi2].dval.ival=fi1;
                    break;
                case RTSTR:
                    //Bugzilla No. 78104 ; 15-04-2002 
					if (plinkvar[fi2].dval.cval)  { delete[] plinkvar[fi2].dval.cval; }
                    plinkvar[fi2].dval.cval=(char *)new char[strlen(fs1)+1];
                    strcpy(plinkvar[fi2].dval.cval,fs1);
                    break;
            }
            if (setgetrb.restype==RTSTR) IC_FREE(setgetrb.resval.rstring);
			// Modified Cybage VSB  04/03/2002 DD/MM/YYYY 
			// Reference: Bug 77973 from Bugzilla
			// add overriden variable in Dimension Override List
			AddToDimOverrideList(&pDimOverrideList,&plinkvar[fi2]);
        }
    }
    //*** Get selection set of entities.
	if (initflg && strsame("CLEAR"/*DNT*/ ,fs1)) {
		if ((ret=sds_pmtssget(ResourceString(IDC_DIMENSIONS__NSELECT_DIM_10, "\nSelect dimensions to clear overrides: " ),NULL,NULL,NULL,NULL,ss1,0))==RTERROR) goto exit;
    } else if((ret=sds_pmtssget(ResourceString(IDC_DIMENSIONS__NSELECT_DIM_11, "\nSelect dimensions to use new value(s): " ),NULL,NULL,NULL,NULL,ss1,0))==RTERROR) goto exit;
    else if (ret==RTCAN) goto exit;

    // Make sure we get all the applictions in the dimension.
    setgetrb.restype=RTSTR; setgetrb.resval.rstring="*"/*DNT*/; setgetrb.rbnext=NULL;

    for (ssct=0L;sds_ssname(ss1,ssct,ename)==RTNORM; ssct++) {
        if ((elist=sds_entgetx(ename,&setgetrb))==NULL) { cmd_ErrorPrompt(CMD_ERR_MOREMEM,0); goto exit; }
        if (ic_assoc(elist,0)!=0) goto exit;
		// Added Cybage VSB  25/02/2002 DD/MM/YYYY [
		// Reference: Bug 77949 from Bugzilla
		// Bugzilla 78003 Support LEADER object. EBATECH(CNBR)
		//if( (!strsame("DIMENSION"/*DNT*/,ic_rbassoc->resval.rstring)) && (!strsame("TOLERANCE"/*DNT*/,ic_rbassoc->resval.rstring)))
		if( (!strsame("DIMENSION"/*DNT*/,ic_rbassoc->resval.rstring)) && (!strsame("TOLERANCE"/*DNT*/,ic_rbassoc->resval.rstring)) && (!strsame("LEADER"/*DNT*/,ic_rbassoc->resval.rstring)))
		{
			sds_redraw(ename,IC_REDRAW_UNHILITE);
			continue;
		}
        
		//If clear the dimension overrides if any 
		if ( strisame("CLEAR"/*DNT*/ ,fs1) )
		{
			for(rb = elist ; rb && rb->restype!=-3;rb=rb->rbnext);
			if ( rb ==NULL ) { sds_redraw(ename,IC_REDRAW_UNHILITE); continue;}
			
			// Search for ACAD application name.
			for( ;rb!=NULL ;rb=rb->rbnext)
			{
				if ( rb->restype == AD_XD_APPOBJHANDLE && strisame(rb->resval.rstring, DIM_ACADAPP) ) 
					break;
			}
			if (rb==NULL) { sds_redraw(ename,IC_REDRAW_UNHILITE); continue; }

			// If application exists, Search for DSTYLE  name.
			struct resbuf *rbprv = rb;
			for( rb=rb->rbnext;rb!=NULL;rbprv = rb,rb=rb->rbnext)
			{
				 if ( rb->restype == AD_XD_STRING && strisame(rb->resval.rstring,DIM_DSTYLE) )
					 break;
				 //If we come across another application name
				 if ( rb->restype == AD_XD_APPOBJHANDLE )
				 {
					 rb = NULL;
					 break;
				 }
			}
			if (rb==NULL) { sds_redraw(ename,IC_REDRAW_UNHILITE); continue; }
			
			// Move till the end of dimension override list.
			for( rb=rb->rbnext;rb!=NULL ;rb=rb->rbnext)
			{
				if ( rb->restype == AD_XD_CONTROL && strisame(rb->resval.rstring,"}"/*DNT*/) )
					break;
				//If we come across another application name
				if ( rb->restype == AD_XD_APPOBJHANDLE )
				 {
					 rb = NULL;
					 break;
				 }
			}
			if (rb==NULL) { sds_redraw(ename,IC_REDRAW_UNHILITE); continue; }
			
			struct resbuf *rbDelete = NULL;
			rbDelete		= rbprv->rbnext;
			rbprv->rbnext	= rb->rbnext;
			
			rb->rbnext= NULL;
			// Free the memory.
			sds_relrb(rbDelete);							
			
			int SavedSDS_AtNodeDrag;
			SavedSDS_AtNodeDrag = SDS_AtNodeDrag;			
			SDS_AtNodeDrag = 1;		
			sds_entmod(elist);			
			SDS_AtNodeDrag = SavedSDS_AtNodeDrag;			
			continue;
		}
		// Added Cybage VSB  25/02/2002 DD/MM/YYYY ]
		// Modified Cybage VSB  04/03/2002 DD/MM/YYYY [
		// Reference: Bug 77973 from Bugzilla
		/*
		for (rb=elist;rb;rb=rb->rbnext) {
            switch(rb->restype) {
                case  8: layername=(char *)calloc((strlen(rb->resval.rstring)+1),sizeof(char *));
                         strcpy(layername,rb->resval.rstring);
                         break;
                case 10: ic_ptcpy(plink->defpt,rb->resval.rpoint); break;
                case 11: ic_ptcpy(plink->textmidpt,rb->resval.rpoint); break;
                case 12: ic_ptcpy(plink->blkipt,rb->resval.rpoint); break;
                case 13: ic_ptcpy(plink->pt13,rb->resval.rpoint); break;
                case 14: ic_ptcpy(plink->pt14,rb->resval.rpoint); break;
                case 15: ic_ptcpy(plink->pt15,rb->resval.rpoint); break;
                case 40: plink->leadlen=rb->resval.rreal; break;
                case 50: plink->dlnang=rb->resval.rreal; break;
                case 51: plink->hdir=rb->resval.rreal; break;
                case 52: plink->elnang=rb->resval.rreal; break;
                case 53: plink->textrot=rb->resval.rreal; break;
                case 70: plink->flags=rb->resval.rint; break;
                case  1: strcpy(plink->text,rb->resval.rstring); break;
            }
        }
        cmd_geteedvalues((db_handitem *)ename[0],(db_drawing *)ename[1]);
        sds_entdel(ename);
        plink->Setparflag=TRUE;
		IC_RELRB(elist);	
        if ((elist=sds_buildlist(RTDXF0,"DIMENSION"/*DNT* /,
                                        1 ,plink->text,
                                        10,plink->defpt,
                                        11,plink->textmidpt,
                                        12,dimzeropt,
                                        13,plink->pt13,
                                        14,plink->pt14,
                                        15,plink->pt15,
                                        16,plink->pt16,
                                        50,plink->dlnang,
                                        51,plink->hdir,
                                        52,0.0,
                                        53,plink->textrot,
                                        70,plink->flags,0))==NULL) { cmd_ErrorPrompt(CMD_ERR_MOREMEM,0); goto exit; }

        cmd_buildEEdList(elist);
        if ((sds_entmake(elist))!=RTNORM) goto exit;
        plink->Setparflag=FALSE;
		IC_RELRB(elist);			// 	Should this be here or at the top of the for?
		*/
		
		//Make exlist to point to xdata list from elist
		for(rb = elist ; rb && rb->restype!=-3;rb=rb->rbnext);
		if ( rb != NULL ) 		
			exlist = rb;
		else
			exlist = NULL;

		struct resbuf *exAcadlist = NULL;
		if ( exlist != NULL )
		{
			// Get the 1001 group this would be the application name.
			struct resbuf *rb = NULL;
			for( rb = exlist;rb;rb=rb->rbnext) {
				if (rb->restype==AD_XD_APPOBJHANDLE)
				{
					if ( strisame(rb->resval.rstring,DIM_ACADAPP))
					{
						exAcadlist = rb;
						break;
					}
				}
			}
		}

		
		
		newexlist = GetNewexlist(pDimOverrideList,exAcadlist);

		if ( exlist == NULL )
		{
			//Add the new xdata list to elist
			struct resbuf * plastrb = NULL;
			for (plastrb=elist; plastrb && plastrb->rbnext;plastrb=plastrb->rbnext);			
			if ( plastrb && plastrb->rbnext == NULL )
			{
				plastrb->rbnext = sds_newrb(-3);
				plastrb->rbnext->rbnext =newexlist;
			}
		}
		else
		{
			// we had xdata but not of ACAD
			if ( exAcadlist == NULL )
			{
				struct resbuf * temp = NULL;
				struct resbuf * rb = NULL;
				for(rb = elist ; rb->restype!=-3;rb=rb->rbnext);
				temp = rb->rbnext;
				rb->rbnext = newexlist;
				for(rb = newexlist; rb->rbnext != NULL ;rb=rb->rbnext);
				rb->rbnext = temp;
			}
			// we had xdata of ACAD and was updated properly in GetNewexlist()
		}
		
		sds_redraw(ename, IC_REDRAW_UNDRAW);
		
		//plink->Setparflag=TRUE;
		//if ((sds_entmod(elist))!=RTNORM) goto exit;		
		//plink->Setparflag=FALSE;

		int SavedSDS_AtNodeDrag;
		SavedSDS_AtNodeDrag = SDS_AtNodeDrag;
		SDS_AtNodeDrag = 1;				
		if ((sds_entmod(elist))!=RTNORM) goto exit;
		sds_redraw(ename, IC_REDRAW_UNDRAW);
		if ((sds_entmod(elist))!=RTNORM) goto exit;
		SDS_AtNodeDrag = SavedSDS_AtNodeDrag;    		
				
		IC_RELRB(elist);    

		// Modified Cybage VSB  04/03/2002 DD/MM/YYYY ]

    }

    exit:

	IC_RELRB(elist);
	// Added Cybage VSB  04/03/2002 DD/MM/YYYY 
	// Reference: Bug 77973 from Bugzilla
	RemoveDimOverrideList(&pDimOverrideList);
    if (sds_ssfree(ss1)!=RTNORM)
    if (rc==0L) return(RTNORM);
    if (rc==(-1L)) return(RTCAN);
    if (rc<(-1L))  CMD_INTERNAL_MSG(rc);
    return(RTERROR);
}

void cmd_savelast(struct cmd_dimeparlink *ptr,struct gr_view *view) {
	short fi1 = 0;

	if (view) {
		ic_ptcpy(lastdimpts.ucsorg,view->ucsorg);
		for (fi1=0;fi1<3;++fi1) ic_ptcpy(lastdimpts.ucsaxis[fi1],view->ucsaxis[fi1]);
		lastdimpts.elev=view->elevation;
	}
    ic_ptcpy(lastdimpts.pt10,ptr->defpt);
    ic_ptcpy(lastdimpts.pt13,ptr->pt13);
    ic_ptcpy(lastdimpts.pt14,ptr->pt14);
    ic_ptcpy(lastdimpts.pt15,ptr->pt15);
    ic_ptcpy(lastdimpts.pt16,ptr->pt16);
    lastdimpts.startpt[0]=ptr->pt13[0];
    lastdimpts.startpt[1]=ptr->defpt[1];
    ic_ptcpy(lastdimpts.pt210,ptr->pt210);
    lastdimpts.r50=ptr->dlnang;
    lastdimpts.i70=ptr->flags;
}

// ============================================================================================================================================================
// ========================================================= DIMEDIT FUNCTIONS =====================================================================
// ============================================================================================================================================================
//
//
//
//
// ============================================================================================================================================================

// This function gets called from several different places.
// The "mode" flag tells this function where it was called from.
// mode: 0 - called from DIMEDIT
//             1 - called from DIM - HOMETEXT
//             2 - called from DIM - NEWTEXT
//             3 - called from DIM - OBLIQUE
//             4 - called from DIM - TROTATE
//
// RETURNS: RTNORM  - Success.
//          RTCAN   - User cancel.
//          RTERROR - Misc. error occured.
//
short cmd_DimEditFunc(short mode) {
    char           fs1[IC_ACADBUF] = ""/*DNT*/,
                   fs2[IC_ACADBUF] = ""/*DNT*/,
                   dimtxt[IC_ACADBUF] = ""/*DNT*/;
    struct resbuf *filter=NULL;
    sds_name       ss1 = {0L, 0L},
                   ename = {0L, 0L};
    sds_real       fr1 = 0.0;
    int            ret = RTNORM;
	long ssct;
	int type = 0;
	struct resbuf* elist = NULL;
	struct resbuf* pCurPair = NULL;
	resbuf allApps;
    allApps.restype=RTSTR; 
	allApps.resval.rstring = "*"/*DNT*/;
	allApps.rbnext = NULL;

//	cmd_initialrbcs(&rbecs,&rbwcs,&rbucs,SDS_CURDWG);
    if (!mode) {
        *fs1=0; cmd_dimmode=1;
        if (sds_initget(SDS_RSG_NOVIEWCHG,ResourceString(IDC_DIMENSIONS_INITGET__HO_145, "~Home ~New Edit_text|Edit ~ Oblique_lines|Oblique Rotate_text|Rotate ~ Restore_text|REstore ~_Home ~_New ~_Edit ~_ ~_Oblique ~_Rotate ~_ ~_REstore" ))!=RTNORM) goto exit;
		// Modified Cybage VSB  03/12/2001 DD/MM/YYYY
		// Description: Wrong prompt displayed on "dimedit" command, "ROtate" instead of "Rotate". "RO" on command prompt did'nt work, instead "R" on command promt works. AutoCAD also displays "Rotate" and not "ROtate" on dimedit command.
        //if ((ret=sds_getkword(ResourceString(IDC_DIMENSIONS__NEDIT_DIME_146, "\nEdit dimension:  Edit text/Oblique lines/ROtate text/<REstore text>: " ),fs1))==RTERROR) 
		if ((ret=sds_getkword(ResourceString(IDC_DIMENSIONS__NEDIT_DIME_146, "\nEdit dimension:  Edit text/Oblique lines/Rotate text/<REstore text>: " ),fs1))==RTERROR) 
			goto exit;
		// BugZilla No. 78079; 20-05-2002 [
		if(ret == RTNONE)
			strcpy(fs1,"RESTORE"/*DNT*/);
		// BugZilla No. 78079; 20-05-2002 ]
    } else {
        strupr(fs1);
        switch(mode) {
            case 1: strncpy(fs1,"HOME"/*DNT*/ ,sizeof(fs1)-1); break;
            case 2: strncpy(fs1,"NEW"/*DNT*/ ,sizeof(fs1)-1); break;
            case 3: strncpy(fs1,"OBLIQUE"/*DNT*/ ,sizeof(fs1)-1); break;
            case 4: strncpy(fs1,"ROTATE"/*DNT*/ ,sizeof(fs1)-1); break;
        }
    }
    strupr(fs1);
    // Build filter list for ssget.
    if ((filter=sds_buildlist(RTDXF0,"DIMENSION"/*DNT*/ ,0))==NULL) 
        goto exit;
    if (strsame("HOME"/*DNT*/ ,fs1) || strsame("RESTORE"/*DNT*/ ,fs1) || ret==RTNONE) {
        //*** Get selection set of entities.
        if ((ret=sds_pmtssget(ResourceString(IDC_DIMENSIONS__NSELECT_DI_150, "\nSelect dimensions to restore text to default position: " ),NULL,NULL,NULL,filter,ss1,0))==RTERROR) goto exit;
		plink->HomeText = TRUE;	// EBATECH(CNBR) Bugzilla#78455 13-03-2003 DIMEDIT /HOME
    } else if ((strsame("NEW"/*DNT*/ ,fs1)) || (strsame("EDIT"/*DNT*/ ,fs1))) {
        sprintf(fs2,ResourceString(IDC_DIMENSIONS__NNEW_DIMEN_152, "\nNew dimension text <%s>: " ),dimtxt);
        if ((ret=sds_getstring(1,fs2,dimtxt))==RTERROR) goto exit;
        // Get selection set of entities.
        if ((ret=sds_pmtssget(ResourceString(IDC_DIMENSIONS__NSELECT_DI_153, "\nSelect dimensions to replace with new text: " ),NULL,NULL,NULL,filter,ss1,0))==RTERROR) goto exit;
    } else if(strsame("OBLIQUE"/*DNT*/ ,fs1)) {
        // Get selection set of entities.
        if ((ret=sds_pmtssget(ResourceString(IDC_DIMENSIONS__NSELECT_LI_154, "\nSelect linear dimensions to make oblique: " ),NULL,NULL,NULL,filter,ss1,0))==RTERROR) goto exit;
        fr1=0;
        if ((ret=sds_getangle(NULL,ResourceString(IDC_DIMENSIONS__NENTER_OBL_155, "\nEnter obliquing angle: " ),&fr1))==RTERROR) goto exit;
    } else if (strsame("ROTATE"/*DNT*/ ,fs1)) {
        fr1=0;
        if ((ret=sds_getangle(NULL,ResourceString(IDC_DIMENSIONS__NANGLE_OF__115, "\nAngle of dimension text: " ),&fr1))==RTERROR) goto exit;
        ic_normang(&fr1,NULL);
        //*** Get selection set of entities.
        if ((ret=sds_pmtssget(ResourceString(IDC_DIMENSIONS__NSELECT_EN_156, "\nSelect entities to rotate text: " ),NULL,NULL,NULL,filter,ss1,0))==RTERROR) goto exit;
    }

    for(ssct = 0L; sds_ssname(ss1, ssct, ename) == RTNORM; ssct++) 
	{
        if((elist = sds_entgetx(ename, &allApps)) == NULL) 
			goto exit;
		pCurPair = ic_ret_assoc(elist, 70);
        if(pCurPair == NULL)
			continue;

        type = pCurPair->resval.rint;
		type &= ~DIM_TEXT_MOVED;
		//if(type & DIM_TEXT_MOVED)	type -=DIM_TEXT_MOVED;
		type &= ~DIM_BIT_6;
        //if(type & DIM_BIT_6)	type -=DIM_BIT_6;

        if(strsame("HOME"/*DNT*/, fs1) || strsame("RESTORE"/*DNT*/, fs1) || ret == RTNONE) 
		{
			//Bugzilla No. 78182 ; 03-06-2002 [ 
            //pCurPair->resval.rint ^= DIM_TEXT_MOVED;
			pCurPair->resval.rint &= ~DIM_TEXT_MOVED;
			//Bugzilla No. 78182 ; 03-06-2002 ] 
			pCurPair = ic_ret_assoc(elist, 53);
			if(pCurPair != NULL)
				pCurPair->resval.rreal = 0.0;
            SDS_AtNodeDrag = 0;
        } 
		else 
		{
			if((strsame("NEW"/*DNT*/, fs1)) || (strsame("EDIT"/*DNT*/, fs1)))
			{
				pCurPair = ic_ret_assoc(elist, 1);
				ASSERT(pCurPair != NULL);
				pCurPair->resval.rstring = ic_realloc_char(pCurPair->resval.rstring, strlen(dimtxt) + 1);
				strcpy(pCurPair->resval.rstring, dimtxt);
			}
			else 
				if(strsame("OBLIQUE"/*DNT*/, fs1)) 
				{
					if (type != DIM_ROT_HOR_VER && type != DIM_ALIGNED) 
						continue;
					pCurPair = ic_ret_assoc(elist, 52);
					if(pCurPair == NULL)
					{
						// group 52 is optional so create it
						for(pCurPair = elist; pCurPair->rbnext != NULL || pCurPair->rbnext->restype != -3; pCurPair = pCurPair->rbnext);
						struct resbuf* pGroup52 = sds_newrb(RTREAL);
						pGroup52->resval.rreal = fr1;
						pGroup52->rbnext = pCurPair->rbnext;
						pCurPair->rbnext = pGroup52;
					}
					else
						pCurPair->resval.rreal = fr1;
				}
				else 
					if(strsame("ROTATE"/*DNT*/, fs1)) 
					{
						pCurPair = ic_ret_assoc(elist, 53);
						if(pCurPair == NULL)
						{
							// group 53 is optional so create it
							for(pCurPair = elist; pCurPair->rbnext != NULL || pCurPair->rbnext->restype != -3; pCurPair = pCurPair->rbnext);
							struct resbuf* pGroup53 = sds_newrb(RTREAL);
							pGroup53->resval.rreal = fr1;
							pGroup53->rbnext = pCurPair->rbnext;
							pCurPair->rbnext = pGroup53;
						}
						else
							pCurPair->resval.rreal = fr1;
						SDS_AtNodeDrag = 0;
					}
		}

		//Modified Cybage SBD 30/11/2001 DD/MM/YYYY 
		//Reason : Fix for Bug No. 77866 and 77867 from Bugzilla 
		bdimUpdate = TRUE;
		if((ret = sds_entmod(elist)) != RTNORM)
			goto exit;
		//Modified Cybage SBD 30/11/2001 DD/MM/YYYY 
		//Reason : Fix for Bug No. 77866 and 77867 from Bugzilla 
		bdimUpdate = FALSE;
        memcpy(lastdimpts.ename, ename, sizeof(sds_name));
		IC_RELRB(elist);
    }

    exit:
	plink->HomeText = FALSE;	// EBATECH(CNBR) Bugzilla#78455 13-03-2003 DIMEDIT /HOME
	sds_ssfree(ss1);	// Was inside an "if(ssct>0L)" 
	IC_RELRB(elist);	// Was IC_FREEIT
    IC_RELRB(filter);
	//Modified Cybage SBD 30/11/2001 DD/MM/YYYY 
	//Reason : Fix for Bug No. 77866 and 77867 from Bugzilla 
	bdimUpdate = FALSE;
    return(RTERROR);
}

static void addMtextFormatCodes(int type, char *text, LPCTSTR ext, bool continuation)
{
    if (text == NULL)
        return;

    TCHAR	tmpStr[IC_ACADBUF] = ""/*DNT*/,
			*tmpStrPtr = NULL,
			extStr[20] = ""/*DNT*/,
			nonStacked[IC_ACADBUF] = ""/*DNT*/,
			*upperStack = NULL,
			*lowerStack = NULL,
			//Bugzilla No. 77887 ; 03-06-2002
			//stackedFractions1[] = "\\A1;%s%s{\\H1.000000x;\\S%s/%s;}\""/*DNT*/,
			stackedFractions1[IC_ACADBUF] = "\\A1;%s%s{\\H1.000000x;\\S%s/%s;}\""/*DNT*/,
			stackedInchesOnly[] = "\\A1;%s{\\H1.000000x;\\S%s;}\""/*DNT*/,
			nonStackedFormat1[] = "\\A1;%s%s"/*DNT*/;

    bool	isaFraction = FALSE,
			isaTolerance = FALSE;

    TCHAR *stackedFormat = continuation ? stackedFractions1+4 : stackedFractions1;
    TCHAR *nonstakFormat = continuation ? nonStackedFormat1+4 : nonStackedFormat1;

    _tcscpy(tmpStr, text);
    tmpStrPtr = tmpStr;

    isaFraction  = _tcsstr(tmpStr, "/"/*DNT*/) != NULL;
    isaTolerance = _tcsstr(tmpStr, "^"/*DNT*/) != NULL;

    extStr[0] = '\0'/*DNT*/;
    if (ext != NULL  && *ext != 0 && isdigit((unsigned char) *text)/*First char is NOT a postfix*/) {
        if (*ext=='D'/*DNT*/) 
            strcpy(extStr, "\\U+2205"/*DNT*/);     // AutoCAD unicode char.
        else 
            strcpy(extStr, ext);
    } 

    switch(type) {

    case -4:
    case -5:
    case -6:
    case -7:
        if (_tcsstr(tmpStr, "/")) {
            upperStack = _tcstok(tmpStr, "/"/*DNT*/);
            lowerStack = _tcstok(NULL, " \n"/*DNT*/);
            if (type == -7) {
                // Need to add inch mark for this value.
                sprintf(text, "%s\\/%s\"", upperStack, lowerStack);
            } else
                sprintf(text, "%s\\/%s", upperStack, lowerStack);
        } else if(type == -7)
            sprintf(text, "%s\"", tmpStr);
            
        break;
        break;
    case 1:             // dimunit 1 or dimaltu 1.
    case 2:
    case 3:
    case 6:
    case 7:
        sprintf(text, nonstakFormat, extStr, tmpStr);
        break;
    case 4:             // dimunit 4 || 5 or dimaltu 4 || 5.
    case 5:
        if (isaFraction || isaTolerance) {
            if (isaFraction) {
				//Bugzilla No. 77887 ; 03-06-2002
				if (!icadRealEqual(plinkvar[DIMTFAC].dval.rval,1.0))
				{
					sprintf(stackedFractions1,"%s%f%s","\\A1;%s%s{\\H",plink->dimtfac,";\\S%s/%s;}\"");
				}
                if (_tcsstr(tmpStr, " "/*DNT*/) != NULL) {
                    tmpStrPtr = _tcsstr(tmpStr, " "/*DNT*/) + 1;
                    _tcscpy(nonStacked,tmpStr);
                    *(_tcsstr(nonStacked, " "/*DNT*/)) = '\0'/*DNT*/;                    
                } else {
                    // If we get something like 0'-01/4", we need to separate into nonStacked and Stacked
                    // parts. Note that the first 4 characters of the string MUST be '0'-0'
                    if (_tcsstr(tmpStr, "0'-0"/*DNT*/) == tmpStr) {
                        _tcscpy(nonStacked,"0'-0"/*DNT*/);
                        tmpStrPtr = tmpStr + strlen("0'-0"/*DNT*/);
                    } else if (_tcsstr(tmpStr, "-0"/*DNT*/) != NULL) {
                        _tcscpy(nonStacked,tmpStr);
                        *(_tcsstr(nonStacked, "-0"/*DNT*/)+2) = '\0'/*DNT*/; 
                        tmpStrPtr = _tcsstr(tmpStr, "-0"/*DNT*/) + 2;
                    } else if (_tcsstr(tmpStr, "-"/*DNT*/) != NULL) {
                        tmpStrPtr = _tcsstr(tmpStr, "-"/*DNT*/) + 1;
                        _tcscpy(nonStacked,tmpStr);
                        *(_tcsstr(nonStacked, "-"/*DNT*/)+1 ) = '\0'/*DNT*/; 
                    } else
                        _tcscpy(nonStacked,tmpStr);
                }


                if (strsame(nonStacked, text)) {
                    if (_tcsstr(text, "/"/*DNT*/) != NULL) {
                        if (nonStacked[strlen(nonStacked) - 1] == '"'/*DNT*/)
                            nonStacked[strlen(nonStacked) - 1] = '\0'/*DNT*/;
                        sprintf(text, continuation ? stackedInchesOnly+ 4 : stackedInchesOnly , extStr, nonStacked);
                        if (type == 5 && text[strlen(text) - 1] == '"'/*DNT*/)
                            text[strlen(text) - 1] = '\0'/*DNT*/;
                    } else                    
                        sprintf(text, nonstakFormat, extStr, nonStacked);
                } else { 
                    upperStack = lowerStack = NULL;
                    upperStack   = _tcstok(tmpStrPtr, "/"/*DNT*/);
                    lowerStack   = _tcstok(NULL, "\"\n"/*DNT*/);
                    sprintf(text, stackedFormat, extStr, nonStacked, upperStack, lowerStack);
                    //Bugzilla No. 78177 ; 03-06-2002
					if (plinkvar[DIMUNIT].dval.ival == 5 || plinkvar[DIMALTU].dval.ival == 5)
                        text[_tcslen(text) -1] = '\0'/*DNT*/;
                }
            } else {
                        // Tolerance handled here.
            }
        } else {
            // Nothing in this string to stack.
            sprintf(text, nonstakFormat, extStr, tmpStr);
        }
        break;
    case 8: {
			// Windows desktop decimal format.
			TCHAR numericSep[2];
			TCHAR sep = '.'/*DNT*/;
			int size      = GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, numericSep, 2);
			sprintf(text, nonstakFormat, extStr, tmpStr);

			if (numericSep[0] != sep && size != 0) {
				char *walk = text;
				while(*(walk++) != 0) {
					if (*walk == sep)
						*walk = numericSep[0];
				}
			}
		}
        break;
    default:
        break;

    }

    return;
}

static void addTolerances(int toltype, sds_real unformated, char *formatedMtext)
{
	if(plinkvar[DIMTOL].dval.ival != 1 && plinkvar[DIMLIM].dval.ival != 1)
		return;

    int			dec = 0,
    			zin = 0,
				unit = 0,
				ignoremode = 0;
	sds_real	upperTol = 0.0,
				lowerTol = 0.0;
    char		uTol[IC_ACADBUF] = ""/*DNT*/,
				lTol[IC_ACADBUF] = ""/*DNT*/,
				string[IC_ACADBUF] = ""/*DNT*/;
    bool		doDimLim = plinkvar[DIMLIM].dval.ival == 1;
	int			dsep = plinkvar[DIMDSEP].dval.ival; // EBATECH(CNBR) 2003/3/31 Support DIMDSEP

    switch(toltype) {
        case DIMUNIT:
			//EBATECH(CNBR) -[ Bugzilla#78443 Use DIMTZIN from current SYSVAR buf.
                unit		 = plinkvar[DIMUNIT].dval.ival;
			    dec			 = plinkvar[DIMTDEC].dval.ival;
			    zin			 = plinkvar[DIMTZIN].dval.ival;
			    upperTol	 = plinkvar[DIMTP].dval.rval;
			    lowerTol	 = plinkvar[DIMTM].dval.rval;

            if (doDimLim) {
                upperTol = unformated + upperTol;
                lowerTol = unformated - lowerTol;
            }

            cmd_rtos_dim(upperTol,unit,dec,uTol, zin, dsep);
            //cmd_rtos_dim(upperTol,unit,dec,uTol, DIMTYPETOL);
            addMtextFormatCodes((unit * -1), uTol, NULL, TRUE);	
            
            if (upperTol != lowerTol || doDimLim) {
 	            cmd_rtos_dim(lowerTol,unit,dec,lTol, zin, dsep);
 	            //cmd_rtos_dim(lowerTol,unit,dec,lTol, DIMTYPETOL);
                addMtextFormatCodes((unit * -1), lTol, NULL, TRUE);  
            }
            break;
			//EBATECH(CNBR)]-
        case DIMALT:
			//EBATECH(CNBR) -[ Bugzilla#78443 Use DIMALTTZ from current SYSVAR buf.
                unit		 = plinkvar[DIMALTU].dval.ival;
			    dec			 = plinkvar[DIMALTTD].dval.ival;
			    zin			 = plinkvar[DIMALTTZ].dval.ival;
			    unformated	*= plinkvar[DIMALTF].dval.rval;
			    upperTol	 = plinkvar[DIMTP].dval.rval * plinkvar[DIMALTF].dval.rval;
			    lowerTol	 = plinkvar[DIMTM].dval.rval * plinkvar[DIMALTF].dval.rval;
            
            if (doDimLim) {
                upperTol = unformated + upperTol;
                lowerTol = unformated - lowerTol;
            }

            cmd_rtos_dim(upperTol,unit,dec,uTol, zin, dsep);
            //cmd_rtos_dim(upperTol,unit,dec,uTol, DIMTYPEALTTOL);
            addMtextFormatCodes((unit * -1), uTol, NULL, TRUE);	
            
            if (upperTol != lowerTol || doDimLim) {
 	            cmd_rtos_dim(lowerTol,unit,dec,lTol, zin, dsep);
 	            //cmd_rtos_dim(lowerTol,unit,dec,lTol, DIMTYPEALTTOL);
                addMtextFormatCodes((unit * -1), lTol, NULL, TRUE);  
            }
			break;
			//EBATECH(CNBR)]-
        case DIMANG:
			//EBATECH(CNBR) -[ Bugzilla#78443 Use DIMAZIN from current SYSVAR buf.
            unit		 = plinkvar[DIMAUNIT].dval.ival;
			dec			 = plinkvar[DIMADEC].dval.ival == -1 ? plinkvar[DIMDEC].dval.ival : plinkvar[DIMADEC].dval.ival;
			zin			 = plinkvar[DIMAZIN].dval.ival;
			upperTol	 = plinkvar[DIMTP].dval.rval;
			lowerTol	 = plinkvar[DIMTM].dval.rval;

            if (unit != 3) upperTol = (upperTol*IC_PI/180.0);
            if (unit == 2) upperTol /= 10.0/9.0;

            if (doDimLim)
                upperTol += unformated;

#ifdef OLD_ANGBASE
			ignoremode = 2;
#else
            // We should not take into account ANGBASE value
			ignoremode = 3;
#endif
            sds_angtos_dim(ignoremode,upperTol,unit,dec,uTol,zin, dsep);
            addMtextFormatCodes((unit * -1), uTol, NULL, TRUE);	
            
            if (upperTol != lowerTol || doDimLim) {
                if (unit != 3) lowerTol = (lowerTol*IC_PI/180.0);
                if (unit == 2) lowerTol /= 10.0/9.0;
                if (doDimLim)
                    lowerTol = unformated - lowerTol;
                bool lessThanZero = FALSE;
                if (unit == 3 && lowerTol < 0) {
                    // This is for acad compatibliliy. If the less than tol is less
                    // than 0 radians, acad shows it as a negative number. Now we do too.
                    lowerTol *= -1;
                    lessThanZero = TRUE;
                }
                sds_angtos_dim(ignoremode,lowerTol,unit,dec,lTol,zin, dsep);

                if (lessThanZero) {
                    // Patch up the tolerance.
                    CString tmp = "-";
                    tmp += lTol;
                    _tcscpy(lTol, tmp);
                }

                addMtextFormatCodes((unit * -1), lTol, NULL, TRUE);  
            }
            break;
			//EBATECH(CNBR)]-
        default:
            ASSERT(0);
            break;
    }

// May turn this on later. MHB
//    bool dimtfact = FALSE;
//    sds_real dimtfactor = 1.0;

//    if (plinkvar[DIMTFAC].dval.rval != 1.0) {
//        dimtfact = TRUE;
//        dimtfactor = plinkvar[DIMTXT].dval.rval * plinkvar[DIMSCALE].dval.rval * plinkvar[DIMTFAC].dval.rval;
//    }

    if (upperTol == lowerTol && !doDimLim) {
        // Prepend the +/- symbol.
        if (unit == 7  && uTol[strlen(uTol) -1] != '"') {
			//Bugzilla No. 77887 ; 03-06-2002 
            if (!icadRealEqual(plinkvar[DIMTFAC].dval.rval,1.0))
                sprintf(string, "{\\H%f;%c%s\"}"/*DNT*/ , plink->dimtfac, 177, uTol);
            else

                // EBATECH(watanabe)-[localize
                //sprintf(string, "%c%s\""/*DNT*/ , 177, uTol);
                sprintf(string, "%%%%p%s\""/*DNT*/ , uTol);
                // ]-EBATECH(watanabe)
        } else {
            //Bugzilla No. 77887 ; 03-06-2002
			if (!icadRealEqual(plinkvar[DIMTFAC].dval.rval,1.0))
                sprintf(string, "{\\H%f;%c%s}"/*DNT*/, plink->dimtfac, 177, uTol);
            else

                // EBATECH(watanabe)-[localize
                //sprintf(string, "%c%s"/*DNT*/, 177, uTol);
                sprintf(string, "%%%%p%s"/*DNT*/, uTol);
                // ]-EBATECH(watanabe)
        }
    } else {
        // Add stacking format characters.
        if (doDimLim) {
            //Bugzilla No. 77887 ; 03-06-2002
			if (!icadRealEqual(plinkvar[DIMTFAC].dval.rval,1.0))
                sprintf(string, "{\\H%f;\\S%s^%s;}"/*DNT*/, plink->dimtfac, uTol, lTol);
            else
                sprintf(string, "\\S%s^%s;"/*DNT*/, uTol, lTol);	
        } else {

            bool hasUSign = *uTol == '+'/*DNT*/ || *uTol == '-'/*DNT*/;
            bool hasLSign = *lTol == '+'/*DNT*/ || *lTol == '-'/*DNT*/;

            char upperSign = (upperTol < 0.0) ? '-'/*DNT*/ : '+'/*DNT*/;    // Negative in negative out.
            char lowerSign = (lowerTol < 0.0) ? '+'/*DNT*/ : '-'/*DNT*/;    // Negative in positive out.
            //Bugzilla No. 77887 ; 03-06-2002
			if (!icadRealEqual(plinkvar[DIMTFAC].dval.rval,1.0))
			    sprintf(string, "{\\H%f;\\S%c%s^%c%s;}"/*DNT*/, plink->dimtfac, upperSign , hasUSign?uTol+1:uTol, lowerSign, hasLSign?lTol+1:lTol);			    
            else            
            sprintf(string, "\\S%c%s^%c%s;"/*DNT*/, upperSign , hasUSign?uTol+1:uTol, lowerSign, hasLSign?lTol+1:lTol);	
        }
	}

	if(doDimLim)
        // If we are, then we are replacing the sent in 'formatedMtext'.
		strcpy(formatedMtext, string);
	else
		strcat(formatedMtext, string);
}

// EBATECH(watanabe)-[This function is called from other files
//static xformpt(sds_point from, sds_point to, sds_matrix mat) {
void xformpt(sds_point from, sds_point to, sds_matrix mat) {
// ]-EBATECH(watanabe)
/*
**  Applies an ADS-type matrix to a point.
**  'from and 'to' can be the same point in memory.
*/
    sds_point ap1;

    ap1[0]=mat[0][0]*from[0]+mat[0][1]*from[1]+mat[0][2]*from[2]+mat[0][3];
    ap1[1]=mat[1][0]*from[0]+mat[1][1]*from[1]+mat[1][2]*from[2]+mat[1][3];
    ap1[2]=mat[2][0]*from[0]+mat[2][1]*from[1]+mat[2][2]*from[2]+mat[2][3];
    to[0]=ap1[0]; to[1]=ap1[1]; to[2]=ap1[2];
}

// EBATECH(watanabe)-[add arrow block
void cmd_addarrowblock(char* str)
{
    cmd_initialize();
    cmd_setparlink(SDS_CURDWG);

    struct resbuf rbb;
    if (sds_getvar("CLAYER" ,&rbb) == RTNORM) {
        db_astrncpy(&layername, rbb.resval.rstring, strlen(rbb.resval.rstring) + 1);
    } else {
        db_astrncpy(&layername,"0",3);
    }
    checkBlock(str, SDS_CURDWG);
    cmd_CleanUpDimVarStrings();
}
// ]-EBATECH(watanabe)

#pragma optimize ("", on)
