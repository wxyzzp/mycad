// =========================================================================================================================================
// ================================================================ Includes ===============================================================
// =========================================================================================================================================

#include "cmds.h"/*DNT*/
#include "dimensions.h"
#include "cmdsDimVars.h"
#include "IcadTextScrn.h"/*DNT*/
#include "IcadView.h"

// SystemMetrix(Hiro)-[Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED(add)
static char strClosedFilled[] = "";
// ]-SystemMetrix(Hiro) Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED(add)

// =========================================================================================================================================
// =========================================================== Global Variables ============================================================
// =========================================================================================================================================

//extern BOOL     useStoredValues;
extern short    cmd_dimmode;
extern char     *layername;
extern struct cmd_dimvars plinkvar[];
extern struct cmd_dimeparlink* plink;
//Modified Cybage SBD 13/11/2001 DD/MM/YYYY 
//Reason : Fix for Bug No. 77866 and 77867 from Bugzilla 
BOOL bdimUpdate;

// ============================================================================================================================================================
// ========================================================= DIMENSION STYLE FUNCTIONS ========================================================================
// ============================================================================================================================================================
//
//
//
//
// ============================================================================================================================================================

// ============================================================================================================================================================
// ========================================================= DIMENSION STYLE FUNCTIONS ========================================================================
// ============================================================================================================================================================
//
//
//
//
// ============================================================================================================================================================

int cmd_setActiveDimStyle (struct resbuf *elist)
	{
	int		fi1 = 0,
			fi2 = 0,
			ret = RTNORM;
	resbuf	*rb = NULL,
			setgetrb;

    for (fi2=0,rb=elist;rb!=NULL;rb=rb->rbnext)
		{
        if (rb->restype<=0) continue;
        for (fi1=0;fi1<DIM_LASTDIMVAR && plinkvar[fi1].DXF!=rb->restype;++fi1);
        if (fi1>DIM_LASTDIMVAR) continue;
        switch(plinkvar[fi1].type)
			{
            case RTSTR:
                setgetrb.restype=RTSTR;
                // If the variable is 340 then it is DIMTXSTY this is stored as a an ename not a string, get the name using the handitem.
                if (rb->restype==340)
					{
                    db_handitem   *handitem;
                    char          *string;
                    handitem=(db_handitem *)((long)rb->resval.rlname[0]);
                    handitem->get_2(&string);                    
					setgetrb.resval.rstring= new char [strlen(string)+1];
                    strcpy(setgetrb.resval.rstring,string);

					}
                // SystemMetrix(Hiro)-[Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED(add)
                else if (rb->restype == 341)
                  {
                    char* string;
                    db_handitem* handitem = (db_handitem *)((long)rb->resval.rlname[0]);
                    if (handitem)
                      {
                        handitem->get_2(&string);
                      }
                    else
                      {
                        // ClosedFilled
                        string = strClosedFilled;
                      }
                    setgetrb.resval.rstring = new char [strlen(string) + 1];
                    strcpy(setgetrb.resval.rstring, string);
                  }
                // ]-SystemMetrix(Hiro) Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED(add)
				else
					{
                    setgetrb.resval.rstring= new char [strlen(rb->resval.rstring)+1];
                    strcpy(setgetrb.resval.rstring,rb->resval.rstring);
					}
                fi2=1;
                break;
            case RTREAL:
                setgetrb.restype=RTREAL;
                setgetrb.resval.rreal=rb->resval.rreal;
                break;
            case RTSHORT:
                setgetrb.restype=RTSHORT;
                setgetrb.resval.rint=rb->resval.rint;
                break;
            default:
                continue;
			}
        if (sds_setvar(plinkvar[fi1].Var,&setgetrb)!=RTNORM) { ret = RTERROR; goto exit;}
        if (fi2) { IC_FREE(setgetrb.resval.rstring); fi2=0; }
		}
exit:
	return ret;
	}

/*----------------------------------------------------------------------------*/
int cmd_dimStyleSet()
{

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	int status = RTNORM;

	// Get the value of the dimstyle system variable.
    struct sds_resbuf rbb;
    SDS_getvar(NULL,DB_QDIMSTYLE,&rbb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES); 
    struct sds_resbuf *pDimStyle = sds_tblsearch("DIMSTYLE"/*DNT*/,rbb.resval.rstring,0);

	// Set the active dimstyle. This means that all dimension system variables (dimscale, etc.) 
	// will get the value that is in the dimstyle from the system variable.
    status = cmd_setActiveDimStyle (pDimStyle);

	// Clean up.
    IC_RELRB(pDimStyle);
    IC_FREE(rbb.resval.rstring);

	return status;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Mode 1 = Save
//	Mode 2 = Restore
//	Mode 3 = Status
//	Mode 4 = Variables
//	Mode 5 = Apply
//
short cmd_DimStyleFunc(short mode,db_drawing *flp) {
    char           fs1[IC_ACADBUF] = ""/*DNT*/,
                   dimstyl[IC_ACADNMLEN] = ""/*DNT*/;
    struct resbuf  setgetrb,
                  *elist=NULL,
                  *dslist=NULL,
                  *eedlist=NULL,
                  *rbt=NULL,
                  *rb=NULL;
				   //Bugzilla No. 60681; 15/07/2002 
                   //rbecs,
                   //rbwcs,
                   //rbucs;
    short          fi1 = 0,
                   fi2 = 0;
    int            scrollmax = 0,
                   scrollcur = 0,
                   dispvars = 0,
                   expert = 0,
                   color = 0,
                   type = 0;
    sds_name       ename = {0L, 0L},
                   ss1 = {0L, 0L};
    long           ssct = 0L;
    sds_point      pt1 = {0.0, 0.0, 0.0},
                   pt210 = {0.0, 0.0, 1.0};
	db_handitem*	theblk = NULL;

//    cmd_initialrbcs(&rbecs,&rbwcs,&rbucs,SDS_CURDWG);

    //useStoredValues=FALSE;
    cmd_setparlink(flp);

    if (SDS_getvar(NULL,DB_QEXPERT,&setgetrb,flp,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) goto exit;
	expert=setgetrb.resval.rint;

    if (!mode) {
        if (SDS_getvar(NULL,DB_QDIMSTYLE,&setgetrb,flp,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) goto exit;
        strncpy(dimstyl,setgetrb.resval.rstring,sizeof(dimstyl)-1);
        IC_FREE(setgetrb.resval.rstring);
        sds_printf(ResourceString(IDC_DIMENSIONS__NCURRENT_DI_80, "\nCurrent dimension style: %s" ),dimstyl);
		IC_RELRB(elist);	
        elist=sds_tblsearch("DIMSTYLE"/*DNT*/,dimstyl,0);
        for (fi2=0,rb=elist;rb;rb=rb->rbnext) {
            for (fi1=0;fi1<DIM_LASTDIMVAR;++fi1) { if (plinkvar[fi1].DXF==rb->restype) break; }
            if (fi1<DIM_LASTDIMVAR) {
                switch(plinkvar[fi1].type) {
                    case RTSTR:
                        if (rb->restype==340) {
                            db_handitem   *handitem;
                            char          *string;
                            handitem=(db_handitem *)((long)rb->resval.rlname[0]);
                            handitem->get_2(&string);
                            if ((!plinkvar[fi1].Var && plinkvar[fi1].dval.cval) ||
                                (plinkvar[fi1].Var && !plinkvar[fi1].dval.cval) || (stricmp(string,plinkvar[fi1].dval.cval))) {
                                sprintf(fs1,"\"%s\""/*DNT*/,plinkvar[fi1].dval.cval);
                                if (fi2) fi2=2;
                            } else continue;
                        // SystemMetrix(Hiro)-[Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED(add)
                        } else if (rb->restype == 341)
                          {
                            char* string;
                            db_handitem* handitem = (db_handitem*)((long)rb->resval.rlname[0]);
                            if (handitem)
                              {
                                handitem->get_2(&string);
                              }
                            else
                              {
                                // ClosedFilled
                                string = strClosedFilled;
                              }

                            if ((!plinkvar[fi1].Var &&  plinkvar[fi1].dval.cval) ||
                                ( plinkvar[fi1].Var && !plinkvar[fi1].dval.cval) ||
                                (stricmp(string,plinkvar[fi1].dval.cval)))
                              {
                                sprintf(fs1,"\"%s\""/*DNT*/, plinkvar[fi1].dval.cval);
                                if (fi2)
                                  {
                                    fi2 = 2;
                                  }
                             }
                           else
                             {
                               continue;
                             }
                        // ]-SystemMetrix(Hiro) Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED(add)
                        } else if ((!plinkvar[fi1].Var && plinkvar[fi1].dval.cval) ||
                                   (plinkvar[fi1].Var && !plinkvar[fi1].dval.cval) || (stricmp(rb->resval.rstring,plinkvar[fi1].dval.cval))) {
                            sprintf(fs1,"\"%s\""/*DNT*/,plinkvar[fi1].dval.cval);
                            if (fi2) fi2=2;

                        } else continue;
                        break;
                    case RTREAL:
                        if (rb->resval.rreal!=plinkvar[fi1].dval.rval) {
                            sprintf(fs1,"%-12g"/*DNT*/,plinkvar[fi1].dval.rval);
                            if (fi2) fi2=2;
                        } else continue;
                        break;
                    case RTSHORT:
                        if (rb->resval.rint!=plinkvar[fi1].dval.ival) {
                            sprintf(fs1,"%-12i"/*DNT*/,plinkvar[fi1].dval.ival);
                            if (fi2) fi2=2;
                        } else continue;
                        break;
                }
            } else continue;
            if (fi2==2) {
                sds_printf(ResourceString(IDC_DIMENSIONS__NALTERED_DI_82, "\nAltered dimension style variables: " ));
                fi2=0;
            }
            sds_printf("\n%-10.10s=%s"/*DNT*/,plinkvar[fi1].Var,fs1);
        }
        *fs1=0; cmd_dimmode=1;
        if (sds_initget(SDS_RSG_NOVIEWCHG,ResourceString(IDC_DIMENSIONS_INITGET______83, "? ~ Apply Save STatus Variables Restore ~_? ~_ ~_Apply ~_Save ~_STatus ~_Variables ~_Restore" ))!=RTNORM) goto exit;
        if ((fi1=sds_getkword(ResourceString(IDC_DIMENSIONS__NDIMENSION__84, "\nDimension styles:  ? to list styles/Apply/Save/STatus/Variables/<Restore>: " ),fs1))==RTERROR) goto exit;
        if (fi1==RTNONE) 
            strcpy(fs1,"RESTORE"/*DNT*/ );
    } else {
        switch(mode) {
            case 1: strcpy(fs1,"SAVE"/*DNT*/);      break;
            case 2: strcpy(fs1,"RESTORE"/*DNT*/);   break;
            case 3: strcpy(fs1,"STATUS"/*DNT*/);    break;
            case 4: strcpy(fs1,"VARIABLES"/*DNT*/); break;
            case 5: strcpy(fs1,"APPLY"/*DNT*/);     break;
        }
    }
    strupr(fs1);
    if (strsame("?"/*DNT*/ ,fs1)) {
        if ((cmd_dimstyle_list())!=0) goto exit;
    } else if (strsame("STATUS"/*DNT*/ ,fs1)) {
		sds_textscr();
		if (SDS_getvar(NULL,DB_QSCRLHIST,&setgetrb,flp,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) goto exit;
		scrollmax=min(SDS_CMainWindow->m_pTextScrn->m_pPromptWnd->m_pPromptWndList->m_nLinesPerPage,setgetrb.resval.rint);
		scrollmax-=3;
        scrollcur=0;
        CString description;
        for (fi1=0;fi1<DIM_LASTDIMVAR;++fi1) {
            description.LoadString(plinkvar[fi1].DescIdx);
            switch(plinkvar[fi1].type) {
                case RTSTR:
                    *fs1=0;
					sprintf(fs1,"\"%s\""/*DNT*/,plinkvar[fi1].dval.cval);
                    sds_printf("\n%-10.10s = %-16.16s %-40.40s"/*DNT*/,plinkvar[fi1].Var,fs1,description);
                    if (cmd_entlister_scrollchk(&scrollcur,scrollmax)!=RTNORM) goto exit;
                    break;
                case RTREAL:
					sds_rtos(plinkvar[fi1].dval.rval,-1,-1,fs1);
					sds_printf("\n%-10.10s = %-16.16s %-40.40s"/*DNT*/,plinkvar[fi1].Var,fs1,description);
                    if (cmd_entlister_scrollchk(&scrollcur,scrollmax)!=RTNORM) goto exit;
                    break;
                case RTSHORT:
					sds_printf("\n%-10.10s = %-16i %-40.40s"/*DNT*/,plinkvar[fi1].Var,plinkvar[fi1].dval.ival,description);
                    if (cmd_entlister_scrollchk(&scrollcur,scrollmax)!=RTNORM) goto exit;
                    break;
            }
		}
    } else if (strsame("APPLY"/*DNT*/ ,fs1)) {
        // Make sure we get all the applictions in the dimension.
        setgetrb.restype=RTSTR; setgetrb.resval.rstring="*"/*DNT*/; setgetrb.rbnext=NULL;

        if ((sds_pmtssget(ResourceString(IDC_DIMENSIONS__NSELECT_DIM_88, "\nSelect dimensions to apply the current style: " ),NULL,NULL,NULL,NULL,ss1,0))==RTERROR) goto exit;
        for (ssct=0L;sds_ssname(ss1,ssct,ename)==RTNORM; ssct++) {
            IC_RELRB(elist);		// Changed to IC_RELRB().
            if ((elist=sds_entgetx(ename,&setgetrb))==NULL) { cmd_ErrorPrompt(CMD_ERR_MOREMEM,0); goto exit; }
            if (ic_assoc(elist,0)!=0) goto exit;
			if( (!strsame("DIMENSION"/*DNT*/,ic_rbassoc->resval.rstring)) && (!strsame("TOLERANCE"/*DNT*/,ic_rbassoc->resval.rstring)))
			{
				sds_redraw(ename,IC_REDRAW_UNHILITE);
				continue;
			}
            for (rb=elist;rb && rb->rbnext && rb->rbnext->restype!=-3;rb=rb->rbnext);
            if (rb->rbnext) eedlist=rb->rbnext; 

			// Bugzilla No. 60681; 15/07/2002 [
            /*
            for (rb=elist;rb;rb=rb->rbnext) {
                switch(rb->restype) {
                    //case  8:  layername=(char *)calloc((strlen(rb->resval.rstring)+1),sizeof(char *));
                    case  8:  layername=(char *)new char[strlen(rb->resval.rstring)+1];						
                              strcpy(layername,rb->resval.rstring);
                              break;
                    case  10: sds_trans(rb->resval.rpoint,&rbwcs,&rbucs,0,plink->defpt); break;
                    case  11: sds_trans(rb->resval.rpoint,&rbecs,&rbucs,0,plink->textmidpt); break;
                    case  12: ic_ptcpy(plink->blkipt,rb->resval.rpoint); break;
                    case  13: sds_trans(rb->resval.rpoint,&rbwcs,&rbucs,0,plink->pt13); break;
                    case  14: sds_trans(rb->resval.rpoint,&rbwcs,&rbucs,0,plink->pt14); break;
                    case  15: sds_trans(rb->resval.rpoint,&rbwcs,&rbucs,0,plink->pt15); break;
                    case  16: sds_trans(rb->resval.rpoint,&rbecs,&rbucs,0,plink->pt16); break;
                    case  40: plink->leadlen=rb->resval.rreal; break;
                    case  50: plink->dlnang=rb->resval.rreal; break;
                    case  51: plink->hdir=rb->resval.rreal; break;
                    case  52: plink->elnang=rb->resval.rreal; break;
                    case  53: plink->textrot=rb->resval.rreal; break;
                    case  70: plink->flags=rb->resval.rint; break;
                    case   1: strcpy(plink->text,rb->resval.rstring); break;
                    case 210: ic_ptcpy(pt210,rb->resval.rpoint); break;
                    case  62: color=rb->resval.rint; break;
                }
            }
			*/
			// Bugzilla No. 60681; 15/07/2002 ]

            // Get the current layer and save to fs1.
            if (SDS_getvar(NULL,DB_QCLAYER,&setgetrb,flp,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) goto exit;
            strcpy(fs1,setgetrb.resval.rstring);
            // Make the current layer whatever the dimension is on.
            setgetrb.resval.rstring=ic_realloc_char(setgetrb.resval.rstring,strlen(layername) + 1);
            strcpy(setgetrb.resval.rstring,layername);
            SDS_setvar(NULL,DB_QCLAYER,&setgetrb,flp,NULL,NULL,0);
            // Set (type) to the type of dimension.
            type=plink->flags;
            //if (type&DIM_TEXT_MOVED) type -= DIM_TEXT_MOVED;
            type &= ~DIM_TEXT_MOVED;
            //if (type&DIM_BIT_6)      type -= DIM_BIT_6;
            type &= ~DIM_BIT_6;
			// Bugzilla No. 60681; 15/04/2002 [
			/*sds_entdel(ename);
			IC_RELRB(elist);
            if ((elist=sds_buildlist(RTDXF0,"DIMENSION"/*DNT*//*,
                                            1 ,plink->text,
                                            10,plink->defpt,
                                            11,plink->textmidpt,
                                            12,plink->blkipt,
                                            13,plink->pt13,
                                            14,plink->pt14,
                                            15,plink->pt15,
                                            16,plink->pt16,
                                            50,plink->dlnang,
                                            51,plink->hdir,
                                            52,plink->elnang,
                                            53,plink->textrot,
                                            62,color,
                                            70,plink->flags,
                                            210,pt210,0))==NULL) { cmd_ErrorPrompt(CMD_ERR_MOREMEM,0); goto exit; }*/
			struct resbuf temprb;
			SDS_getvar(NULL,DB_QDIMSTYLE,&temprb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES); 
			strcpy(fs1,temprb.resval.rstring);	
			ic_assoc(elist,3);					// Locate the style in elist
			if(!ic_rbassoc) goto exit;			// If style not found simply exit 
			long len = strlen(fs1);
			// Reallocate the style string if there is a difference in length between 
			// the current style and the style in elist 
			if(len != strlen(ic_rbassoc->resval.rstring)) 
				ic_rbassoc->resval.rstring=ic_realloc_char(ic_rbassoc->resval.rstring,len+1);
			strcpy(ic_rbassoc->resval.rstring,fs1); 
			
			for (rb=elist;rb && rb->rbnext && rb->rbnext->restype!=-3;rb=rb->rbnext);
            if (rb && rb->rbnext)               // Release existing xlist if any 
			{
				sds_relrb(rb->rbnext);
				rb->rbnext=NULL;
			}
			
			cmd_buildEEdList(elist);            
			/* 
			Not need now! Previously this was necessary to give a call to setedd of sds_entmod()

			for (rb=elist;rb && rb->rbnext && rb->rbnext->restype!=-3;rb=rb->rbnext);
			// If the entity does not have xlist,add a node for APPOBJHANDLE   

			if(rb && !rb->rbnext)               
			{
				struct sds_resbuf * rbb;
				rbb=rb->rbnext =sds_newrb(-3);
                rbb->rbnext=sds_newrb(AD_XD_APPOBJHANDLE); 
				rbb=rbb->rbnext; 
				// resbufs are 'free'd.
				//rbb->resval.rstring=(char *) new char[strlen(DIM_ACADAPP)+1]; 
				rbb->resval.rstring=(char*)malloc(strlen(DIM_ACADAPP) + 1);
				strcpy(rbb->resval.rstring,DIM_ACADAPP);
			}*/
									
			bdimUpdate=TRUE;
			if ((sds_entmod(elist))!=RTNORM) goto exit;
			
			//if ((sds_entmake(elist))!=RTNORM) goto exit;
			// Bugzilla No. 60681; 15/04/2002 ]
			
			//Modified Cybage SBD 13/11/2001 DD/MM/YYYY 
			//Reason : Fix for Bug No. 77866 and 77867 from Bugzilla 
			bdimUpdate=FALSE;
            // Reset the to current layer.
            setgetrb.resval.rstring=ic_realloc_char(setgetrb.resval.rstring,strlen(fs1) + 1);
            strcpy(setgetrb.resval.rstring,fs1);
            SDS_setvar(NULL,DB_QCLAYER,&setgetrb,flp,NULL,NULL,0);
	
        }
		sds_ssfree(ss1);
    } else if (strsame("SAVE"/*DNT*/ ,fs1)) {
        fi2=IC_TBLMESSER_MAKE;
        for (;;) {
// EBATECH(CNBR) Bugzilla#78446 accept space(s)
            if ((sds_getstring(1/*0*/, ResourceString(IDC_DIMENSIONS__NSAVE_STYLE_90, "\nSave style:  ? to list styles/Name for new dimension style: " ),fs1))==RTERROR) goto exit;
            if (!*fs1) goto exit;
// MR 5/15/98 -- uppercase it
            ic_trim(fs1,"be");
            strupr(fs1);
			if (fs1[0]=='?' && fs1[1]==0) { 
                if ((cmd_dimstyle_list())!=0) 
                    goto exit; 
                continue; 
            }
            strncpy(dimstyl,fs1,sizeof(dimstyl)-1);
			if(!ic_isvalidname(dimstyl)){
				cmd_ErrorPrompt(CMD_ERR_ANYNAME,0);
				continue;
			}

			IC_RELRB(elist);		
			//BugZilla:78448; [
			// If expert == 5 just save the style, and don't ask if we want to 'Redefine it?'.
			if ((elist=sds_tblsearch("DIMSTYLE"/*DNT*/,dimstyl,0))!=NULL) {
				if (expert == 5)
				{
					fi2=IC_TBLMESSER_MODIFY;
				}
				else
				{
					if (sds_initget(SDS_RSG_NOVIEWCHG,ResourceString(IDC_DIMENSIONS_INITGET_YES__91, "Yes No ~_Yes ~_No" ))!=RTNORM) 
						goto exit;
					if ((type=sds_getkword(ResourceString(IDC_DIMENSIONS__NTHAT_NAME__92, "\nThat name is already in use.  Redefine it? <N> " ),fs1))==RTERROR || type==RTCAN) 
						goto exit;
					else if(type==RTNONE) 
						continue;
					else {
						strupr(fs1);
						if (strsame(fs1, "NO"/*DNT*/)) 
							continue;
						fi2=IC_TBLMESSER_MODIFY;
					}
				}
            }
			//BugZilla:78448; ]

            IC_RELRB(dslist);  
            if ((dslist=rb=sds_buildlist(RTDXF0,"DIMSTYLE"/*DNT*/,2,dimstyl,70,0,0))==NULL) {
                sds_printf(ResourceString(IDC_DIMENSIONS__NERROR__INS_93, "\nERROR: Insufficient memory (#%d)." ),__LINE__);
                goto exit;
            }
            while(rb->rbnext!=NULL) rb=rb->rbnext;
            for (fi1=0;fi1<DIM_LASTDIMVAR;++fi1) {
                switch(plinkvar[fi1].DXF) {
                    case 2:
                    case 340:
                    case 89:
                        continue;
                }
                switch(plinkvar[fi1].type) {
                        continue;
                    case RTSTR:
                        *fs1=0;
                        // SystemMetrix(Hiro)-[Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED(add)
                        //rb=rb->rbnext=sds_buildlist(plinkvar[fi1].DXF,plinkvar[fi1].dval.cval,0);
                        if (plinkvar[fi1].DXF == 341)
                          {
                            sds_name name;
                            sds_name_clear(name);

                            if (strlen(plinkvar[fi1].dval.cval) > 0)
                              {
                                struct resbuf* pLdrblk = sds_tblsearch("BLOCK"/*DNT*/, plinkvar[fi1].dval.cval, 0);
                                if (pLdrblk)
                                  {
                                    sds_name_set(pLdrblk->resval.rlname, name);
                                    IC_RELRB(pLdrblk);
                                  }
                              }
                            else
                              {
                                // ClosedFilled
                              }
                            rb = rb->rbnext = sds_buildlist(plinkvar[fi1].DXF, name, 0);
                          }
                        else
                          {
                            rb = rb->rbnext = sds_buildlist(plinkvar[fi1].DXF, plinkvar[fi1].dval.cval, 0);
                          }
                        // ]-SystemMetrix(Hiro) Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED(add)
                        break;
                    case RTREAL:
                        rb=rb->rbnext=sds_buildlist(plinkvar[fi1].DXF,plinkvar[fi1].dval.rval,0);
                        break;
                    case RTSHORT:
                        rb=rb->rbnext=sds_buildlist(plinkvar[fi1].DXF,plinkvar[fi1].dval.ival,0);
                        break;
                }
                if (rb==NULL) {
                    sds_printf(ResourceString(IDC_DIMENSIONS__NERROR__INS_93, "\nERROR: Insufficient memory (#%d)." ),__LINE__); goto exit;
                }
            }
            // Before we look up the arrowhead block in the block table,
            // make sure it is not an internal arrowhead type that gets
            // created on the fly.
			checkBlock( plinkvar[DIMBLK].dval.cval, flp ); // Create internal block on the fly
			if (plinkvar[DIMBLK].dval.cval[0] )
				{
				theblk=(db_blocktabrec *)flp->findtabrec(DB_BLOCKTAB,plinkvar[DIMBLK].dval.cval,1);
				if (theblk==NULL)
					{
					sds_printf(ResourceString(CMD_ERR_DIMBLKDOESNOTEXIST, "\nDIMBLK does not exist -- cannot save." ));
					goto exit;
					}
				}
			checkBlock( plinkvar[DIMBLK1].dval.cval, flp ); // Create internal block on the fly
			if (plinkvar[DIMBLK1].dval.cval[0] )
				{
				theblk=(db_blocktabrec *)flp->findtabrec(DB_BLOCKTAB,plinkvar[DIMBLK1].dval.cval,1);
				if (theblk==NULL)
					{
					sds_printf(ResourceString(CMD_ERR_DIMBLK1DOESNOTEXIST, "\nDIMBLK1 does not exist -- cannot save." ));
					goto exit;
					}
				}
			checkBlock( plinkvar[DIMBLK2].dval.cval, flp ); // Create internal block on the fly
			if (plinkvar[DIMBLK2].dval.cval[0] )
				{
				theblk=(db_blocktabrec *)flp->findtabrec(DB_BLOCKTAB,plinkvar[DIMBLK2].dval.cval,1);
				if (theblk==NULL)
					{
					sds_printf(ResourceString(CMD_ERR_DIMBLK2DOESNOTEXIST, "\nDIMBLK2 does not exist -- cannot save." ));
					goto exit;
					}
				}
			// EBATECH(watanabe)-[dimldrblk
			checkBlock( plinkvar[DIMLDRBLK].dval.cval, flp ); // Create internal block on the fly
			if (plinkvar[DIMLDRBLK].dval.cval[0] )
				{
				theblk=(db_blocktabrec *)flp->findtabrec(DB_BLOCKTAB,plinkvar[DIMLDRBLK].dval.cval,1);
				if (theblk==NULL)
					{
					sds_printf(ResourceString(CMD_ERR_DIMLDRBLKDOESNOTEXIST, "\nDIMLDRBLK does not exist -- cannot save." ));
					goto exit;
					}
				}
			// ]-EBATECH(watanabe)			
			if (SDS_tblmesser(dslist,fi2,flp)!=RTNORM){
				sds_printf(ResourceString(IDC_DIMENSIONS__NUNABLE_TO__94, "\nUnable to save dimension style." ));
				goto exit;
            } else {
				if (fi2==IC_TBLMESSER_MODIFY)
					sds_printf(ResourceString(IDC_DIMENSIONS__NDIMENSION__95, "\nDimension style %s updated." ),dimstyl);
				else 
					sds_printf(ResourceString(IDC_DIMENSIONS__NDIMENSION__96, "\nDimension style set to %s." ),dimstyl);
				setgetrb.restype=RTSTR;
				setgetrb.resval.rstring=dimstyl;
				sds_setvar("DIMSTYLE"/*DNT*/,&setgetrb);
				break;
			}
        }
    } else if (strsame("RESTORE"/*DNT*/ ,fs1) || strsame("VARIABLES"/*DNT*/ ,fs1)) {
        if (strsame("VARIABLES"/*DNT*/ ,fs1))	dispvars=1;
        for (;;) {
            dslist=NULL;
// EBATECH(CNBR) Bugzilla#78446 accept space(s)
            int result = sds_getstring(1/*0*/,ResourceString(IDC_DIMENSIONS__N__TO_LIST__97, "\n? to list styles/ENTER to select dimension/<Dimension style>: " ),fs1);
            if (result==RTERROR || result==RTCAN) 
                goto exit;
            ic_trim(fs1,"be");
            strupr(fs1);
            strcpy(dimstyl,fs1);
            if (!*fs1) {
                if ((type=sds_entsel(ResourceString(IDC_DIMENSIONS__NSELECT_DIM_98, "\nSelect dimension to use as current style: " ),ename,pt1))==RTERROR) {
                    if (SDS_getvar(NULL,DB_QERRNO,&setgetrb,flp,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) goto exit;
                    if (setgetrb.resval.rint==OL_ENTSELPICK) continue;
                    else goto exit;
                } else if (type==RTCAN) 
                    goto exit;
                setgetrb.restype=RTSTR; setgetrb.resval.rstring=DIM_ACADAPP; setgetrb.rbnext=NULL;
                if ((dslist=sds_entgetx(ename,&setgetrb))==NULL) { 
                    cmd_ErrorPrompt(CMD_ERR_MOREMEM,0); 
                    goto exit; 
                }
                if (ic_assoc(dslist,0)!=0) 
                    goto exit;
                if (!strsame("DIMENSION"/*DNT*/,ic_rbassoc->resval.rstring)) { 
                    cmd_ErrorPrompt(CMD_ERR_DIMENSION,0); 
                    continue; 
                }
                if (ic_assoc(dslist,3)!=0) {
                    if (SDS_getvar(NULL,DB_QDIMSTYLE,&setgetrb,flp,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) goto exit;
                    strncpy(fs1,setgetrb.resval.rstring,sizeof(fs1)-1);
                } else strcpy(fs1,ic_rbassoc->resval.rstring);
            } else if (strisame(fs1, "?"/*DNT*/) || strisame(fs1, "_?"/*DNT*/)) {
                if ((cmd_dimstyle_list())!=0) goto exit;
                continue;
            } else if (fs1[0]=='~'/*DNT*/ && fs1[1]!=0) {
				// Bugzilla No. 78088; 29/04/2002 [
				if(!ic_isvalidname(fs1+1)){
					cmd_ErrorPrompt(CMD_ERR_ANYNAME,0);
					continue;
				}
				// Bugzilla No. 78088; 29/04/2002 ]
				IC_RELRB(elist);		//   
                if ((elist=sds_tblsearch("DIMSTYLE"/*DNT*/,(fs1+1),0))==NULL) {
                    sds_printf(ResourceString(IDC_DIMENSIONS__NDIMENSION__99, "\nDimension style %s unknown." ),fs1);
                    continue;
                }
                sds_printf("\n%-10.10s %-16.16s %-16.16s"/*DNT*/,""/*DNT*/,(fs1+1),"Current Settings" );
                for (fi2=0,rb=elist;rb;rb=rb->rbnext) {
                    for (fi1=0;fi1<DIM_LASTDIMVAR;++fi1) { 
                        if (plinkvar[fi1].DXF==rb->restype) 
                            break; 
                    }
                    if (fi1<DIM_LASTDIMVAR) {
                        switch(plinkvar[fi1].type) {
                            case RTSTR:
								// Bugzilla No. 78135; 29/04/2002 [
								{
									char * name=NULL;
									if(plinkvar[fi1].DXF==340 || plinkvar[fi1].DXF==341)
									{
										db_handitem* handitem = (db_handitem*)((long)rb->resval.rlname[0]);
										if (handitem)
										{
											handitem->get_2(&name);
										}
										else if(plinkvar[fi1].DXF==341)
										{
										// ClosedFilled
										name=strClosedFilled;
										}
									}
									else
										name = rb->resval.rstring;

									// Bugzilla No. 78135; 29/04/2002 ]
									
                                if ((!plinkvar[fi1].Var && plinkvar[fi1].dval.cval) ||
										// Bugzilla No. 78135; 29/04/2002 
										//(plinkvar[fi1].Var && !plinkvar[fi1].dval.cval) || (stricmp(rb->resval.rstring,plinkvar[fi1].dval.cval))) {
										//sds_printf("\n%-10.10s %-16.16s %-16.16s"/*DNT*/,plinkvar[fi1].Var,rb->resval.rstring,plinkvar[fi1].dval.cval);
										(plinkvar[fi1].Var && !plinkvar[fi1].dval.cval) || (stricmp(name,plinkvar[fi1].dval.cval))) {
										sds_printf("\n%-10.10s %-16.16s %-16.16s"/*DNT*/,plinkvar[fi1].Var,name,plinkvar[fi1].dval.cval);
                                    fi2=1;
                                }
                                break;
								}
                            case RTREAL:
                                if (rb->resval.rreal!=plinkvar[fi1].dval.rval) {
            					    sds_printf("\n%-10.10s %-16.16g %-16.16g"/*DNT*/,plinkvar[fi1].Var,rb->resval.rreal,plinkvar[fi1].dval.rval);
                                    fi2=1;
                                }
                                break;
                            case RTSHORT:
                                if (rb->resval.rint!=plinkvar[fi1].dval.ival) {
            					    sds_printf("\n%-10.10s %-16i %-16i"/*DNT*/,plinkvar[fi1].Var,rb->resval.rint,plinkvar[fi1].dval.ival);
                                    fi2=1;
                                }
                                break;
                        }
                    }
                }
                if (!fi2) 
					sds_printf(ResourceString(IDC_DIMENSIONS__N_T_T_NO_D_100, "\n\t\t No Differences" ));
                continue;
            }
			// Bugzilla No. 78088; 29/04/2002 [
			if(!ic_isvalidname(fs1)){
				cmd_ErrorPrompt(CMD_ERR_ANYNAME,0);
				continue;
			}
			// Bugzilla No. 78088; 29/04/2002 ]
			IC_RELRB(elist);
			// Bugzilla No. 78135; 29/04/2002  
            //if ((elist=sds_tblsearch("DIMSTYLE"/*DNT*/,fs1,0))==NULL && dispvars==0) {
			if ((elist=sds_tblsearch("DIMSTYLE"/*DNT*/,fs1,0))==NULL ) {
                sds_printf(ResourceString(IDC_DIMENSIONS__NDIMENSION__99, "\nDimension style %s unknown." ),fs1);
                continue;
            }
            if (dslist) {
                sds_printf(ResourceString(IDC_DIMENSIONS__NDIMENSION_101, "\nDimension style: %s " ),fs1);
                for (rb=dslist;rb && rb->restype!=-3;rb=rb->rbnext);
                if (rb) {
                    for (rb=rb->rbnext;rb;rb=rb->rbnext) {
                        if (rb->restype!=AD_XD_STRING) continue;
                        if (strsame(rb->resval.rstring,DIM_DSTYLE)) break;
                    }
                }
                if (rb) {
                    rb=rb->rbnext;
                    if (rb && rb->restype==AD_XD_CONTROL) rb=rb->rbnext;
                    for (fi2=1;rb && rb->rbnext;rb=rb->rbnext) {
                        if (rb->restype==AD_XD_CONTROL) break;
                        for (fi1=0;fi1<DIM_LASTDIMVAR && plinkvar[fi1].DXF!=rb->resval.rint;++fi1); if (rb->rbnext) rb=rb->rbnext;
                        for (fi2=0,rbt=elist;rbt && rbt->restype!=plinkvar[fi1].DXF;rbt=rbt->rbnext);
                        switch(plinkvar[fi1].type) {
                            case RTSTR:
                                sprintf(fs1,"\"%s\""/*DNT*/,rb->resval.rstring);
                                //Bugzilla No. 78104 ; 15-04-2002 
								rbt->resval.rstring= new char [strlen(rb->resval.rstring)+1];
                                strcpy(rbt->resval.rstring,rb->resval.rstring);
                                if (fi2) fi2=2;
                                break;
                            case RTREAL:
                                sprintf(fs1,"%-12g"/*DNT*/,rb->resval.rreal);
                                rbt->resval.rreal=rb->resval.rreal;
                                if (fi2) fi2=2;
                                break;
                            case RTSHORT:
                                sprintf(fs1,"%-12i"/*DNT*/,rb->resval.rint);
                                rbt->resval.rint=rb->resval.rint;
                                if (fi2) fi2=2;
                                break;
                        }
                        if (fi2==2) {
                            sds_printf(ResourceString(IDC_DIMENSIONS__NALTERED_DI_82, "\nAltered dimension style variables: " ));
                            fi2=0;
                        }
                        sds_printf("\n%-10.10s=%s"/*DNT*/,plinkvar[fi1].Var,fs1);
                    }
                }
            }
            break;
        }
        // If the user entered (RESTORE).
        if (!dispvars) {
			cmd_setActiveDimStyle (elist);
            // Restore the dimension style name.
			setgetrb.restype=RTSTR;
			setgetrb.resval.rstring=fs1;
			sds_setvar("DIMSTYLE"/*DNT*/,&setgetrb);
        } else {
            sds_printf(ResourceString(IDC_DIMENSIONS__N_NSTATUS__102, "\n\nStatus of %s:" ),dimstyl);
            sds_textscr();
		    if (SDS_getvar(NULL,DB_QSCRLHIST,&setgetrb,flp,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) goto exit;
		    scrollmax=min(SDS_CMainWindow->m_pTextScrn->m_pPromptWnd->m_pPromptWndList->m_nLinesPerPage,setgetrb.resval.rint);
		    scrollmax-=3;
		    scrollcur=0;
            int switchtype;
            bool cleanBuf = FALSE;
            struct sds_resbuf *tmpRb, rbb;

            for (fi1=0;fi1<DIM_LASTDIMVAR;++fi1) {
                sds_printf("\n%-10.10s = "/*DNT*/,plinkvar[fi1].Var);

                if (ic_assoc(elist,plinkvar[fi1].DXF)!=0) 
                    {
                    memset((void*)&rbb, 0, sizeof(struct sds_resbuf));
                    SDS_getvar(plinkvar[fi1].Var/*DNT*/,0,&rbb,flp,NULL,NULL);
                    switchtype = rbb.restype;
                    tmpRb = &rbb;
                    cleanBuf = TRUE;
                    } else {
                        switchtype = plinkvar[fi1].type;
                        tmpRb = ic_rbassoc;
                    }

                switch(switchtype) {
                    case RTSTR:
						// Bugzilla No. 78135; 29/04/2002 
                        //if (ic_rbassoc->restype==340) {
						if(tmpRb->restype==340){ 
                            db_handitem   *handitem;
                            char          *string;
                            handitem=(db_handitem *)((long)tmpRb->resval.rlname[0]);
                            handitem->get_2(&string);
                            sds_printf("\"%s\""/*DNT*/,string);
                        // SystemMetrix(Hiro)-[Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED(add)
						// Bugzilla No. 78135; 29/04/2002 
                        //} else if (ic_rbassoc->restype == 341)
						} else if (tmpRb->restype == 341)
                          {
                            char* string;
                            db_handitem* handitem = (db_handitem*)((long)tmpRb->resval.rlname[0]);
                            if (handitem)
                              {
                                handitem->get_2(&string);
                              }
                            else
                              {
                                // ClosedFilled
                                string = strClosedFilled;
                              }
                            sds_printf("\"%s\""/*DNT*/, string);
                        // ]-SystemMetrix(Hiro) Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED(add)
                        } else {
                            sds_printf("\"%s\""/*DNT*/,tmpRb->resval.rstring);
                        }
                        break;
                    case RTREAL:
                        if (sds_rtos(tmpRb->resval.rreal,-1,-1,fs1)!=RTNORM) goto exit;
                        sds_printf("%-12s"/*DNT*/,fs1);
                        break;
                    case RTSHORT:
                        sds_printf("%-12d"/*DNT*/,tmpRb->resval.rint);
                        break;
                    default:
                        continue;
                }
                if (cleanBuf) 
                    {
                    cleanBuf = FALSE;
                    if (tmpRb->restype == RTSTR && tmpRb->resval.rstring != NULL) 
                        IC_FREE(tmpRb->resval.rstring);
                    }

                if (cmd_entlister_scrollchk(&scrollcur,scrollmax)!=RTNORM) goto exit;
			}
        }
    }

    cmd_dimmode=0;
	IC_RELRB(elist);		//  (elist, rb, and rbt are the same)  (eedlist is = rb, too)
	IC_RELRB(dslist);		//  (rb is also same as this, sometimes)
    return(RTNORM);

    exit:
        cmd_dimmode=0;
		IC_RELRB(elist);		//  (elist, rb, and rbt are the same)  (eedlist is = rb, too)
		IC_RELRB(dslist);		//  (rb is also same as this, sometimes)
		//Modified Cybage SBD 13/11/2001 DD/MM/YYYY 
		//Reason : Fix for Bug No. 77866 and 77867 from Bugzilla 
		bdimUpdate=FALSE;
        return(RTERROR);
}

long cmd_dimstyle_list(void) {
    char           fs1[IC_ACADBUF] = ""/*DNT*/,
                   scrollines[IC_ACADBUF] = ""/*DNT*/;
    struct resbuf *tmprb=NULL;
    int            ret = 0,
                   numlines = 0,
                   scroll = 0;
    long           rc = 0L;
    short          fi1 = 0;

// EBATECH(CNBR) Bugzilla#78446 accept space(s)
    if ((ret=sds_getstring(1/*0*/,ResourceString(IDC_DIMENSIONS__NDIMENSION_103, "\nDimension style(s) to list <*>: " ),fs1))==RTERROR)
        goto exit; 
    else if (ret==RTCAN) 
        goto exit;
    strupr(fs1);
    sds_printf(ResourceString(IDC_DIMENSIONS__NNAMED_DIM_104, "\nNamed dimension styles:\n" ));
    if (!*fs1) 
		strcpy(fs1,"*"/*DNT*/);
    sds_textscr();
    // No need to check what this returns - If it returns NULL there are no dimension styles defined in the drawing.
    tmprb=sds_tblnext("DIMSTYLE"/*DNT*/,1);
    for (fi1=0; tmprb!=NULL; fi1++) {
        if (sds_usrbrk()) goto exit;
        // Dimension names.
        if (ic_assoc(tmprb,2)!=0) goto exit;
        if (sds_wcmatch(ic_rbassoc->resval.rstring,fs1)==RTNORM) {
            sds_printf("\n  %s"/*DNT*/,ic_rbassoc->resval.rstring); numlines++;
            if (numlines>=20 && !scroll) {
                sds_initget(SDS_RSG_NOVIEWCHG,ResourceString(IDC_DIMENSIONS_INITGET_SCR_105, "Scroll ~_Scroll" ));
                if ((ret=sds_getkword(ResourceString(IDC_DIMENSIONS__NSCROLL__E_106, "\nScroll/<ENTER to view next screen>:" ),scrollines))==RTERROR) {
                    goto exit;
                } else if (ret==RTCAN) {
                    goto exit;
                } else if (ret==RTNONE) {
                    numlines=0;
                } else if (ret==RTNORM) {
                    scroll=1;
                }
            }
        }
        IC_RELRB(tmprb);
        //*** Do NOT check for NULL here.  NULL is checked in the while().
        tmprb=sds_tblnext("DIMSTYLE"/*DNT*/,0);
    }
    if (!fi1) cmd_ErrorPrompt(CMD_ERR_NODIMSTYLES,0);

    exit:

    return(rc);
}
