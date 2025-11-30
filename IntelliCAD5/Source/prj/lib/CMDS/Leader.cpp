// =========================================================================================================================================
// ================================================================ Includes ===============================================================
// =========================================================================================================================================

#include "cmds.h"/*DNT*/
#include "dimensions.h"
#include "cmdsDimVars.h"
#include "IcadView.h"
#include "cmdsDimBlock.h"

// =========================================================================================================================================
// =========================================================== External Variables ==========================================================
// =========================================================================================================================================

// EBATECH(watanabe)-[get mtext info only
extern struct SDS_mTextData   SDS_mTextDataPtr;
//]-EBATECH(watanabe)
// EBATECH(watanabe)-[add
double*                       ptTolerance;
// ]-EBATECH(watanabe)
extern char     dimlsttext[256];
extern char     *layername;
extern struct cmd_dimvars plinkvar[];
extern struct cmd_dimeparlink* plink;

// ============================================================================================================================================================
// ========================================================= LEADER DIMENSION FUNCTIONS =====================================================================
// ============================================================================================================================================================
//
//
//
//
// ============================================================================================================================================================

// EBATECH(watanabe)-[add
// transform point from ucs to wcs
BOOL
ucs2wcs(
  sds_point 	xdir,
  const int     disp = 1
)
{
  struct resbuf rbucs;
  rbucs.restype = RTSHORT;
  rbucs.resval.rint = 1;

  struct resbuf rbwcs;
  rbwcs.restype = RTSHORT;
  rbwcs.resval.rint = 0;

  sds_trans(xdir, &rbucs, &rbwcs, disp, xdir);

  return TRUE;
}
// ]-EBATECH(watanabe)

// EBATECH(watanabe)-[add
void cmd_ReleaseLeaderPts(CPtrArray& pts, sds_name& ss)
{
  // Release pointers for point
  int iCount = pts.GetSize();
  for (int i = 0; i < iCount; i++)
    {
      delete pts[i];
    }
  pts.RemoveAll();

  // delete line entity
  long iLen;
  sds_name name;
  int ret;
  ret = sds_sslength(ss, &iLen);
  if (ret != RTNORM) return;
  for (i = 0; i < iLen; i++)
    {
      ret = sds_ssname(ss, i, name);
      if (ret != RTNORM) return;

      ret = sds_entdel(name);
      if (ret != RTNORM) return;
    }
  ret = sds_ssfree(ss);
  if (ret != RTNORM) return;
}

// Add point to array
void cmd_AddPointToArray(CPtrArray& pts, sds_point pt1)
{
  double*   pt = new sds_point;
  sds_point_set(pt1, pt);
  pts.Add(pt);
}

// Move the last leader point witdh DIMASZ
void cmd_MoveLastLeaderPoint(CPtrArray& pts, const int hookflg)
{
  int i = pts.GetSize() - 1;

  double d = plinkvar[DIMASZ  ].dval.rval *
             plinkvar[DIMSCALE].dval.rval;
  switch (hookflg)
    {
      // right
      case 1:
         ((double*)pts[i])[X] += d;
         break;

      // left
      case 2:
         ((double*)pts[i])[X] -= d;
         break;
    }
}

// Move annotation with DIMGAP
void cmd_MovePointWithDimgap(CPtrArray& pts, const int hookflg, sds_point& pt, const bool bMoveY = true)
{
  double d = plinkvar[DIMGAP  ].dval.rval *
             plinkvar[DIMSCALE].dval.rval;

  // DIMGAP can have minus.
  d = fabs(d);

  // X
  if (abs(hookflg) == 1)
    {
      pt[X] += d;
    }
  else
    {
      pt[X] -= d;
    }

  // Y
  if (bMoveY)
    {
      if (plinkvar[DIMTAD].dval.ival != 0)
        {
          pt[Y] += d;
        }
    }
}

// Get the attachement of MTEXT
int cmd_GetLeaderAttachment(CPtrArray& pts, const int hookflg)
{
  int ret = 7;
  int dimtad = plinkvar[DIMTAD].dval.ival;

  if (abs(hookflg) == 1)
    {
      ret = (dimtad != 0) ?
              7:  // left bottom
              4;  // left center
    }
  else
    {
      ret = (dimtad != 0) ?
              9:  // right bottom
              6;  // right center
    }

  return ret;
}
// ]-EBATECH(watanabe)

short cmd_DrawLeader(int iDoTrueLeaderEnt) {
    if (cmd_iswindowopen() == RTERROR) return RTERROR;

	char           fs1[IC_ACADBUF] = ""/*DNT*/,
    // EBATECH(watanabe)-[comment
    //               fs2[IC_ACADBUF] = ""/*DNT*/,
    // ]-EBATECH(watanabe)
                   dimtxt[IC_ACADBUF] = ""/*DNT*/;
    // EBATECH(watanabe)-[changed
    //struct resbuf *ptllbeg=NULL,
    //              *ptllcur=NULL,
    //              *ptllprev=NULL,
    //              *elist=NULL,
    struct resbuf *elist=NULL,
    // ]-EBATECH(watanabe)
                   setgetrb;

    // EBATECH-[ get current system valiable
    struct resbuf  rbb;
    // ]-EBATECH

    // EBATECH(watanabe)-[add
    CPtrArray      pts;
    int            i;
    sds_point      p10, p11;
    long           iLen;
    sds_name       name,
                   ss;
    int            iAnno = 0;
    bool           bCopy = false;
    // ]-EBATECH(watanabe)

    //struct resbuf  rbucs,
    //               rbwcs,
    //               rbecs;
    sds_name       ename = {0L, 0L};
    sds_point      pt1 = {0.0, 0.0, 0.0},
                   tmppt = {0.0, 0.0, 0.0},
                   ipt[2] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};
    sds_real       fr1 = 0.0,
				   //Bugzilla No 78476 28-04-2003
				   zValue = 0.0,
    // EBATECH(watanabe)-[changed
    //               txthgt = 0.0,
    //               cosofdeg = 0.0,
    //               sinofdeg = 0.0;
                   txthgt = 0.0;
    // ]-EBATECH(watanabe)
    int            ret = 0,
                   color = 0,
    // EBATECH(watanabe)-[comment
    //               oldaso = 0,
    // ]-EBATECH(watanabe)
                   fret = 0;
    short          contflg  =0,
                   hasarrow =1,
                   splineseg=0,
                   hookflg  =0,
                   txthjust =0;
    // EBATECH(watanabe)-[comment
    //BOOL           BlkFlag = TRUE;
    // ]-EBATECH(watanabe)

	//Modified Cybage SBD 09/03/2001 DD/MM/YYYY 
	//Reason : Fix for Bug No. 45931 from Bugzilla 
	BOOL		   AddDimTextFlag=FALSE;

//    cmd_initialrbcs(NULL,&rbwcs,&rbucs,SDS_CURDWG);
    // Create (cmd_dimeparlink) structure, then update the structure.
    cmd_initialize();
    cmd_setparlink(SDS_CURDWG);
//    cmd_initialrbcs(&rbecs,&rbwcs,&rbucs,SDS_CURDWG);

    // EBATECH-[
    // color=256;
	////Bugzilla No 78140 20-05-2002  [
    /*if (sds_getvar("DIMCLRD",&rbb) == RTNORM){
        color=rbb.resval.rint;
        if (color == 0) color = 256; // BYBLOCK -> BYLAYER
    }
    else{
        color=256;
    }*/
	////Bugzilla No 78140 20-05-2002  ]
    // Current Layer

    if (sds_getvar("CLAYER" ,&rbb) == RTNORM){
        db_astrncpy(&layername, rbb.resval.rstring, strlen(rbb.resval.rstring)+1);
        IC_FREE(rbb.resval.rstring);
        rbb.resval.rstring = NULL;
    }
    else{
        db_astrncpy(&layername,"0",3);
    }
    // ]-EBATECH

    // EBATECH(watanabe)-[comment
    //if (iDoTrueLeaderEnt) cmd_dimmode=1;
    // ]-EBATECH(watanabe)
    strncpy(dimtxt,dimlsttext,sizeof(dimtxt)-1);

    // EBATECH(watanabe)-[comment
    //// Make sure that DIMASO is set to false so that when the
    //// (cmd_dimaddline,cmd_MakeItemHead) functions are called it uses the right transformations.
    //oldaso=plinkvar[DIMASO].dval.ival;
    //plinkvar[DIMASO].dval.ival=FALSE;
    // ]-EBATECH(watanabe)

    // First point
    if ((ret=sds_getpoint(NULL,ResourceString(IDC_DIMENSIONS__NSTART_OF__116, "\nStart of leader: " ),pt1))!=RTNORM) goto exit;

	//Bugzilla No 78476 28-04-2003
	zValue = pt1[2];

    // EBATECH(watanabe)-[add
    cmd_AddPointToArray(pts, pt1);
    sds_ssadd(NULL, NULL, ss);
    // ]-EBATECH(watanabe)

    // EBATECH(watanabe)-[comment
    //// Create the link list with the first point.
    //if ((ptllprev=ptllcur=ptllbeg=sds_buildlist(10,pt1,0))==NULL) { cmd_ErrorPrompt(CMD_ERR_MOREMEM,0); goto exit; }
    // ]-EBATECH(watanabe)

    // Second point
    for (;;) {
        contflg=0;
        do {
            if ((fret=sds_getpoint(pt1,ResourceString(IDC_DIMENSIONS__NNEXT_POIN_117, "\nNext point: " ),pt1))==RTERROR) goto exit;
            // EBATECH(watanabe)-[changed
            //else if (fret==RTCAN) break;
            else if (fret==RTCAN) goto exit;
            // ]-EBATECH(watanabe)
			//Bugzilla No 78476 28-04-2003
			pt1[2] = zValue;

            // EBATECH(watanabe)-[comment
            //else if (fret==RTNONE) {
            //    // Make sure that there is more then one point.
            //    if (ptllcur==ptllbeg) {
            //        cmd_ErrorPrompt(CMD_ERR_TRYAGAIN,0);
            //        if (cmd_dimmode) goto exit;
            //        continue;
            //    }
            //    if (cmd_dimmode) break;
            //}
            //ptllprev=ptllcur;
            //if ((ptllcur=ptllcur->rbnext=sds_buildlist(10,pt1,0))==NULL) { cmd_ErrorPrompt(CMD_ERR_MOREMEM,0); goto exit; }
            //color=((plinkvar[DIMCLRD].dval.ival)?plinkvar[DIMCLRD].dval.ival:color);
            //cmd_dimaddline(layername,color,ptllprev->resval.rpoint,ptllcur->resval.rpoint,SDS_CURDWG,0);
            //if (BlkFlag) {
            //    fr1=sds_angle(ptllprev->resval.rpoint,ptllcur->resval.rpoint);
            //    sinofdeg=sin(fr1); cosofdeg=cos(fr1);
            //    
            //    //Modified Cybage AW 16-02-01 
            //    //Reason : to apply leader style
            //    char *chBlkName = new char[512];    
            //    sds_name enameLdr;
            //    enameLdr[1] = (long)SDS_CURDWG;
            //    ((db_drawing*)enameLdr[1])->get_dimldrblk(chBlkName);
            //    if(chBlkName && *chBlkName) 
            //        strcpy(cmd_string, chBlkName);
            //    else
            //        strcpy(cmd_string,DIM_CLOSED_FILLED);

            //    /*
            //    if ((plinkvar[DIMSAH].dval.ival) && *plinkvar[DIMBLK1].dval.cval)
            //        strcpy(cmd_string,plinkvar[DIMBLK1].dval.cval);
            //    else if (plinkvar[DIMBLK].dval.cval && *plinkvar[DIMBLK].dval.cval)
            //        strcpy(cmd_string,plinkvar[DIMBLK].dval.cval);
            //    else
            //        strcpy(cmd_string,DIM_CLOSED_FILLED);
            //    */
            //    //Modified Cybage AW 16-02-01 ]
            //    // Checks if the block exists and create if need
            //    if( *cmd_string )
            //        checkBlock( cmd_string, SDS_CURDWG );
            //    fr1-=IC_PI;
            //    cmd_MakeItemHead(ipt,cmd_string,SDS_CURDWG,0,sinofdeg,cosofdeg,fr1, FALSE,TRUE,ptllprev->resval.rpoint,color);
            //    BlkFlag=FALSE;
            //}
            //if (!cmd_dimmode) break;
            // ]-EBATECH(watanabe)

            // EBATECH(watanabe)-[add
            cmd_AddPointToArray(pts, pt1);

            // create LINE entity
            i = pts.GetSize() - 1;
            sds_point_set((double*)pts[i - 1], p10);
            sds_point_set((double*)pts[i],     p11);
            cmd_dimaddline(layername, color, plinkvar[DIMDLE].dval.ival, p10, p11, SDS_CURDWG, 0);

            // add list for erase
            ret = sds_entlast(name);
            if (ret != RTNORM) goto exit;

            ret = sds_ssadd(name, ss, ss);
            if (ret != RTNORM) goto exit;
            break;
            // ]-EBATECH(watanabe)
        } while(TRUE);
        // This is the R13 way of getting the second point
        // EBATECH(watanabe)-[changed
        //while(!cmd_dimmode) {
        while(1) {
        // ]-EBATECH(watanabe)
            if (sds_initget(SDS_RSG_NOVIEWCHG,ResourceString(IDC_DIMENSIONS_INITGET_ANN_118, "Annotation Format ~ Undo ~_Annotation ~_Format ~_ ~_Undo" ))!=RTNORM) goto exit;
            // EBATECH(watanabe)-[changed
            //if ((ret=sds_getpoint(ptllcur->resval.rpoint,ResourceString(IDC_DIMENSIONS__NTO_POINT__119, "\nTo point:  Format/Undo/<Annotation>: " ),pt1))==RTERROR) goto exit;
            i = pts.GetSize() - 1;
            sds_point_set((double*)pts[i], p10);
            ret = sds_getpoint(p10, ResourceString(IDC_DIMENSIONS__NTO_POINT__119, "\nTo point:  Format/Undo/<Annotation>: " ), pt1);
            if (ret == RTERROR) {
                goto exit;
            }
            // ]-EBATECH(watanabe)
            else if (ret==RTCAN) goto exit;
            if (ret==RTKWORD || ret==RTNONE) {
                *fs1=0;
                if (ret==RTKWORD) {
                    if (sds_getinput(fs1)!=RTNORM) 
                        goto exit;
                }
                strupr(fs1);
                if (ret==RTNONE || (ret==RTKWORD && strsame("ANNOTATION"/*DNT*/ ,fs1))) {
                    break;
                } else if(strsame("FORMAT"/*DNT*/ ,fs1)) {
                    for (;;) {
                        if (sds_initget(SDS_RSG_NOVIEWCHG,ResourceString(IDC_DIMENSIONS_INITGET_ARR_122, "Arrow None SPline STraight ~ Exit ~_Arrow ~_None ~_SPline ~_STraight ~_ ~_Exit" ))!=RTNORM) goto exit;
                        if ((ret=sds_getkword(ResourceString(IDC_DIMENSIONS__NARROW_NON_123, "\nArrow/None/SPline/STraight/<Exit>: " ),fs1))==RTERROR) goto exit;
                        else if(ret==RTCAN) {
                            goto exit;
                        }
                        strupr(fs1);
                        if (ret==RTNONE || (ret==RTNORM && strsame("EXIT"/*DNT*/ ,fs1))) 
                            break;
                        if (strsame("SPLINE"/*DNT*/ ,fs1)) {
                            // EBATECH(watanabe)-[comment
                            //// ~| TODO - spline leader line, needs to be dealt with in cmd_CreateLeader()
                            //sds_printf(ResourceString(IDC_DIMENSIONS__NTHIS_OPTI_125, "\nThis option will have no affect on the polyline because it is not finished." ));
                            // ]-EBATECH(watanabe)
                            splineseg=1;
                        } else if(strsame("STRAIGHT"/*DNT*/ ,fs1)) {
                           splineseg=0;
                        } else if(strsame("ARROW"/*DNT*/ ,fs1)) {
                            hasarrow=1;
                        } else if(strsame("NONE"/*DNT*/ ,fs1)) {
                            hasarrow=0;
                        }
                    }
                    continue;
                } else if(strsame("UNDO"/*DNT*/ ,fs1)) {
                    // EBATECH(watanabe)-[changed
                    //// Erase the last grdraw line
                    //sds_grdraw(ptllprev->resval.rpoint,ptllcur->resval.rpoint,-1,0);
                    //// Free the last point
                    //ptllprev->rbnext=NULL;
                    //IC_RELRB(ptllcur);
                    //// Reset the previous point and the current point pointers
                    //for (ptllprev=ptllcur=ptllbeg; ptllcur->rbnext!=NULL; ptllcur=ptllcur->rbnext) {
                    //    ptllprev=ptllcur;
                    //}
                    //if (ptllcur!=ptllbeg) continue;
                    // Delete previous LINE entity
                    //Bugzilla No 78108 15-04-2002 [
					long setlen;
					sds_sslength(ss,&setlen);
					if (sds_ssname(ss, setlen-1, name) != RTNORM) goto exit;
					//if (sds_ssname(ss, 0, name) != RTNORM) goto exit;
					//Bugzilla No 78108 15-04-2002  ]
                    if (sds_entdel(name) != RTNORM) goto exit;
                    if (sds_ssdel(name, ss) != RTNORM) goto exit;

                    // Cancel the previous point
                    iLen = pts.GetSize() - 1;
                    delete pts[iLen];
                    pts.RemoveAt(iLen);

                    // if over third point
                    iLen--;
                    if (iLen > 0)
                      {
                        // back
                        sds_point_set((double*)pts[iLen], p10);
                        continue;
                      }

                    // second point
                    sds_point_set((double*)pts[0], pt1);
                    // ]-EBATECH(watanabe)
                    contflg=1;
                    break;
                }
            }

            // EBATECH(watanabe)-[changed
            //ptllprev=ptllcur;
            //if ((ptllcur=ptllcur->rbnext=sds_buildlist(10,pt1,0))==NULL) {
            //    cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
            //    goto exit;
            //}
            //if ((elist=sds_buildlist(RTDXF0,db_hitype2str(DB_LINE),10,ptllprev->resval.rpoint,11,ptllcur->resval.rpoint,62,((plinkvar[DIMCLRD].dval.ival)?plinkvar[DIMCLRD].dval.ival:color),0))==NULL) {
            //    cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
            //    goto exit;
            //}
            //if ((sds_entmake(elist))!=RTNORM) goto exit;
			//Bugzilla No 78476 28-04-2003
			pt1[2] = zValue;
            cmd_AddPointToArray(pts, pt1);

            // create LINE entity
            i = pts.GetSize() - 1;
            sds_point_set((double*)pts[i - 1], p10);
            sds_point_set((double*)pts[i],     p11);
            cmd_dimaddline(layername, color, plinkvar[DIMLWD].dval.ival, p10, p11, SDS_CURDWG, 0);

            // Add list for erase
            if (sds_entlast(name) != RTNORM) goto exit;
            if (sds_ssadd(name, ss, ss) != RTNORM) goto exit;
            // ]-EBATECH(watanabe)
        }
        if (!contflg) break;
    }

    // EBATECH(watanabe)-[changed
    //// Get the neccessary system variables
    //if (SDS_getvar(NULL,DB_QDIMTXT,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) goto exit;
    //txthgt=(setgetrb.resval.rreal*plinkvar[DIMSCALE].dval.rval);
    txthgt = plinkvar[DIMTXT].dval.rval * plinkvar[DIMSCALE].dval.rval;
    // ]-EBATECH(watanabe)

    // Add the hook
    // EBATECH(watanabe)-[changed
    //fr1=sds_angle(ptllprev->resval.rpoint,ptllcur->resval.rpoint);
    i = pts.GetSize() - 1;
    sds_point_set((double*)pts[i - 1], p10);
    sds_point_set((double*)pts[i],     p11);
    fr1 = sds_angle(p10, p11);
    // ]-EBATECH(watanabe)
    if ((fr1>DIM_15_DEG && fr1<=DIM_90_DEG) || (fr1<DIM_345_DEG && fr1>=DIM_270_DEG)) {
        // EBATECH(watanabe)-[comment
        //sds_polar(ptllcur->resval.rpoint,0.0,(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval),tmppt);
        //ptllprev=ptllcur;
        //if ((ptllcur=ptllcur->rbnext=sds_buildlist(10,tmppt,0))==NULL) {
        //    cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
        //    goto exit;
        //}
        // ]-EBATECH(watanabe)
        hookflg=1; // Right hook, left justify text
    } else if ((fr1<DIM_165_DEG && fr1>DIM_90_DEG) || (fr1>DIM_195_DEG && fr1<DIM_270_DEG)) {
        // EBATECH(watanabe)-[comment
        //sds_polar(ptllcur->resval.rpoint,IC_PI,(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval),tmppt);
        //ptllprev=ptllcur;
        //if ((ptllcur=ptllcur->rbnext=sds_buildlist(10,tmppt,0))==NULL) {
        //    cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
        //    goto exit;
        //}
        // ]-EBATECH(watanabe)
        hookflg=2; // Left hook, right justify text
    } else if(fr1<=DIM_90_DEG || fr1>=DIM_270_DEG) {
        hookflg=(-1); // No hook, left justify text
    } else if(fr1>DIM_90_DEG && fr1<DIM_270_DEG) {
        hookflg=(-2); // No hook, right justify text
    } else goto exit;

    // EBATECH(watanabe)-[add
    // text style
    char dimtxsty[IC_ACADNMLEN];
    strncpy(dimtxsty, plinkvar[DIMTXSTY].dval.cval, sizeof(dimtxsty) - 1);
    // ]-EBATECH(watanabe)

    // EBATECH(watanabe)-[changed
	////Modified Cybage SBD 09/03/2001 DD/MM/YYYY
	////Reason : Fix for Bug No. 45931 from Bugzilla 
	////if (fret==RTNONE || iDoTrueLeaderEnt||() {
    //if (fret==RTNONE || iDoTrueLeaderEnt||(cmd_dimmode==0)) {
    if (fret==RTNORM) {
    // ]-EBATECH(watanabe)

        // EBATECH(watanabe)-[comment
        //// Get the dimension text, If dimmode is true then we are in the dimension mode, if not then leader is
        //// being done in none dimension mode.
        //if (cmd_dimmode) {
		//	 //EBATECH(CNBR) Bugzilla#78443 Use DIMZIN from current SYSVAR buf.
        //    cmd_rtos_dim(plink->dimvalue,plinkvar[DIMUNIT].dval.ival,plinkvar[DIMDEC].dval.ival,dimtxt, plinkvar[DIMZIN].dval.ival,plinkvar[DIMDSEP].dval.ival);
        //    sprintf(fs2,ResourceString(IDC_DIMENSIONS__NDIMENSION__52, "\nDimension text <%s>: " ),dimtxt);
        //    if ((ret=sds_getstring(1,fs2,fs1))==RTERROR) goto exit; else { if (ret==RTCAN) goto exit; }
        //    if (*fs1){
        //        strncpy(dimtxt,fs1,sizeof(dimtxt)-1);
        //        strncpy(dimlsttext,dimtxt,sizeof(dimlsttext)-1);
        //    }
        //} else {
        // ]-EBATECH(watanabe)
            for (;;) {
				//Modified Cybage SBD 09/03/2001 DD/MM/YYYY 
				//Reason : Fix for Bug No. 45931 from Bugzilla 
				//if ((ret=sds_getstring(1,ResourceString(IDC_DIMENSIONS__NENTER_FOR_130, "\nENTER for options/<Annotation>: " ),fs1))==RTERROR) goto exit;
				if ((ret=sds_getstring(1,ResourceString(IDC_DIMENSIONS_FLAnn_Text_157, "\nFirst line of annotation text/<options>: " ),fs1))==RTERROR) goto exit;
                else if (ret==RTCAN) goto exit;
                if (!*fs1) {
                    contflg=0;
                    for (;;) {
                        if (sds_initget(SDS_RSG_NOVIEWCHG,ResourceString(IDC_DIMENSIONS_INITGET_BLO_131, "Block Copy None Tolerance Mtext ~_Block ~_Copy ~_None ~_Tolerance ~_Mtext" ))!=RTNORM) goto exit;
                        if ((ret=sds_getkword(ResourceString(IDC_DIMENSIONS__NDIMENSION_132, "\nDimension text options:  Block/Copy/None/Tolerance/<Mtext>: " ),fs1))==RTERROR) goto exit;
                        else if (ret==RTCAN) 
                            goto exit;
                        strupr(fs1);
                        if (ret==RTNONE || (ret==RTNORM && strsame("MTEXT"/*DNT*/ ,fs1))) {
                            // EBATECH(watanabe)-[changed
                            //// ~| TODO AutoCAD calls the MTEXT command here.  Eventually we need to do the same.
							////EBATECH(CNBR) Bugzilla#78443 Use DIMZIN from current SYSVAR buf.
                            //cmd_rtos_dim(plink->dimvalue,plinkvar[DIMUNIT].dval.ival,plinkvar[DIMDEC].dval.ival,dimtxt,plinkvar[DIMZIN].dval.ival,plinkvar[DIMDSEP].dval.ival);
                            //sprintf(fs2,ResourceString(IDC_DIMENSIONS__NDIMENSION__52, "\nDimension text <%s>: " ),dimtxt);
                            ////Modified Cybage SBD 09/03/2001 DD/MM/YYYY [
                            ////Reason : Fix for Bug No. 45931 from Bugzilla 
                            ////if ((ret=sds_getstring(1,fs2,fs1))==RTERROR) 
                            ////    goto exit; 
                            ////else if (ret==RTCAN) 
                            ////    continue;
                            //
                            //AddDimTextFlag=TRUE; //this text has to be added to the leader
                            //char temp[IC_ACADBUF]=""; //to store user input
                            //int count=0; //to store no of strings entered
                            //strcpy(fs1,"");
                            //for(;;){
                            //    strcpy(temp,"");
                            //    if ((ret=sds_getstring(1,ResourceString(IDC_DIMENSIONS__NMTEXT___139, "\nMtext: " ),temp))==RTERROR) goto exit;
                            //    else if (ret==RTCAN) goto exit; 
                            //    if(strsame("",temp)) break;
                            //    if(count>0){
                            //        strcat(fs1,"\\P"/*DNT*/);
                            //    }
                            //    strcat(fs1,temp);
                            //    count++;
                            //}
                            ////Modified Cybage SBD 09/03/2001 DD/MM/YYYY ] 
                            //if (*fs1) 
                            //    strncpy(dimtxt,fs1,sizeof(dimtxt)-1);

                            // EBATECH(watanabe)-[get mtext info only
                            // Style
                            SDS_mTextDataPtr.text = "";

                            // Style
                            strcpy(SDS_mTextDataPtr.szTextStyle, dimtxsty);

                            // Height
                            SDS_mTextDataPtr.rTextHeight = txthgt;

                            // Width
                            SDS_mTextDataPtr.rBoxWidth   = 0;

                            // Angle
                            SDS_mTextDataPtr.rRotAngle   = 0.0;

                            // Align
                            SDS_mTextDataPtr.nAlignment  = 4;

                            // Delection
                            SDS_mTextDataPtr.nDirection  = 1;

                            LaunchMTEXTEditor(SDS_mTextDataPtr.text, FALSE);
                            CString str = SDS_mTextDataPtr.text;
                            long len = str.GetLength();
                            if (len == 0) {
                                contflg = 1;
                            } else {
                                char* p = str.GetBuffer(len);
                                strncpy(dimtxt, p, sizeof(dimtxt) - 1);
                                break;
                            }
                            // ]-EBATECH(watanabe)
                        }
                        if (strsame("TOLERANCE"/*DNT*/ ,fs1)) {
                            // EBATECH(watanabe)-[changed
                            //// ~| TODO - This function calls a couple of dialogs
                            //sds_printf(ResourceString(IDC_DIMENSIONS__NTHIS_ISN__134, "\nThis isn't finished." ));
                            //continue;
                            iAnno = 1;
                            break;
                            // ]-EBATECH(watanabe)
                        } else if(strsame("COPY"/*DNT*/ ,fs1)) {
                            for (;;) {
                                if ((ret=sds_entsel(ResourceString(IDC_DIMENSIONS__NSELECT_AN_136, "\nSelect an entity: " ),ename,pt1))==RTERROR) {
                                    if (SDS_getvar(NULL,DB_QERRNO,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) goto exit;
                                    if (setgetrb.resval.rint==OL_ENTSELPICK) {
									    cmd_ErrorPrompt(CMD_ERR_NOENT,0);
                                        continue;
                                    } else if (setgetrb.resval.rint==OL_ENTSELNULL) {
                                        continue;
                                    } else goto exit;
                                }
								//Bugzilla No 78120 22-04-2002 [
								else if (ret==RTCAN) 
								{
									contflg=1;
									break;
								}
								//Bugzilla No 78120 22-04-2002 ]
                                IC_RELRB(elist);  //  Was IC_FREEIT
                                if ((elist=sds_entget(ename))==NULL) {
                                    cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
                                    goto exit;
                                }
                                if (ic_assoc(elist,0)!=0) goto exit;
                                // EBATECH(watanabe)-[changed
                                //if (!strsame("TEXT"/*DNT*/,ic_rbassoc->resval.rstring) &&
                                //    !strsame("MTEXT"/*DNT*/,ic_rbassoc->resval.rstring) &&
                                //    !strsame("ATTDEF"/*DNT*/,ic_rbassoc->resval.rstring)) {
                                //    sds_printf(ResourceString(IDC_DIMENSIONS__NTHAT_ENTI_137, "\nThat entity can not be an annotation." ));
                                //    contflg=1; break;
                                //}
                                //if (ic_assoc(elist,1)!=0) goto exit;
                                //strncpy(dimtxt,ic_rbassoc->resval.rstring,sizeof(dimtxt)-1);
                                ////Modified Cybage SBD 09/03/2001 DD/MM/YYYY 
                                ////Reason : Fix for Bug No. 45931 from Bugzilla 
                                //AddDimTextFlag=TRUE; //this text has to be added to the leader
                                char* pStr = ic_rbassoc->resval.rstring;
								//Bugzilla No 78120 22-04-2002 [
								if ((strsame("TEXT"/*DNT*/,   pStr) == 1) ||
                                    (strsame("MTEXT"/*DNT*/,  pStr) == 1) ||
                                    (strsame("ATTDEF"/*DNT*/, pStr) == 1) ||
                                    (strsame("INSERT"/*DNT*/, pStr) == 1) ||
                                    (strsame("TOLERANCE"/*DNT*/, pStr) == 1))
                                  {
                                    bCopy = true;
									contflg=0;
                                    break;
                                  }
								sds_printf(ResourceString(IDC_DIMENSIONS__NTHAT_ENTI_137, "\nPlease select an mtext,text,block reference or tolerance object."));
								//Bugzilla No 78120 22-04-2002 ]
                                contflg=1;
                                // ]-EBATECH(watanabe)
                                //Bugzilla No 78120 22-04-2002
								//break; 
                            }
                        } else if (strsame("BLOCK"/*DNT*/ ,fs1)) {
                            // EBATECH(watanabe)-[changed
                            //if ((ret=cmd_insert())==RTERROR) goto exit; else { if (ret==RTCAN) goto exit; }
                            //// Remove the hook.
                            //ptllprev->rbnext=NULL;
                            //IC_RELRB(ptllcur);
                            //// Reset the previous point and the current point pointers.
                            //for (ptllprev=ptllcur=ptllbeg; ptllcur->rbnext!=NULL; ptllcur=ptllcur->rbnext) ptllprev=ptllcur;
                            iAnno = 2;
                            // ]-EBATECH(watanabe)
                            break;
                        } else if (strsame("NONE"/*DNT*/ ,fs1)) {
                            // EBATECH(watanabe)-[changed
                            //// Remove the hook if they don't want text
                            //ptllprev->rbnext=NULL;
                            //IC_RELRB(ptllcur);
                            //// Reset the previous point and the current point pointers
                            //for (ptllprev=ptllcur=ptllbeg; ptllcur->rbnext!=NULL; ptllcur=ptllcur->rbnext) ptllprev=ptllcur;
                            iAnno = 3;
                            // ]-EBATECH(watanabe)
                            break;
                        }
                        if (!contflg) break;
                    }
                    break;
                } else {
                    strncpy(dimtxt,fs1,sizeof(dimtxt)-1);
					//Modified Cybage SBD 09/03/2001 DD/MM/YYYY [
					//Reason : Fix for Bug No. 45931 from Bugzilla 
					//strncpy(dimtxt,fs1,sizeof(dimtxt)-1);
					//for (;;) {
					//	  if ((ret=sds_getstring(1,ResourceString(IDC_DIMENSIONS__NMTEXT___139, "\nMtext: " ),fs1))==RTERROR) goto exit; else { if (ret==RTCAN) goto exit; }
					//	  if (!*fs1) 
					//		  break;
					
					//}
					AddDimTextFlag=TRUE; //this text has to be added to the leader
					char temp[IC_ACADBUF]=""; //to store user input
					for(;;){						strcpy(temp,"");
						if ((ret=sds_getstring(1,ResourceString(IDC_DIMENSIONS_NLAnn_Text_158, "\nNext line of annotation text: " ),temp))==RTERROR) goto exit;
						else if (ret==RTCAN) goto exit; 
						if(strsame("",temp)) break;
						strcat(fs1,"\\P"/*DNT*/);
						strcat(fs1,temp);
                    }
					strncpy(dimtxt,fs1,sizeof(dimtxt)-1);
					//Modified Cybage SBD 09/03/2001 DD/MM/YYYY ]
                    break;
                }
            }
        // EBATECH(watanabe)-[changed
        //}
        //IC_RELRB(elist);  // Was IC_FREEIT
        //if (hookflg>0) cmd_dimaddline(layername,color,ptllprev->resval.rpoint,ptllcur->resval.rpoint,SDS_CURDWG,0);
        IC_RELRB(elist);  // Was IC_FREEIT
        // ]-EBATECH(watanabe)
   }

    // EBATECH(watanabe)-[comment
    //// Create the dimension text
    //switch(hookflg) {
    //    case  1: // Right hook, left justify text No hook, left justify text
    //    case -1: sds_polar(ptllcur->resval.rpoint,0.0,fabs(plinkvar[DIMGAP].dval.rval*plinkvar[DIMSCALE].dval.rval),tmppt); txthjust=4; break;
    //    case  2: // Left hook, right justify text No hook, right justify text
    //    case -2: sds_polar(ptllcur->resval.rpoint,IC_PI,fabs(plinkvar[DIMGAP].dval.rval*plinkvar[DIMSCALE].dval.rval),tmppt); txthjust=6; break;
    //}
    // ]-EBATECH(watanabe)

    // EBATECH(watanabe)-[add
    sds_name_nil(name);
    // ]-EBATECH(watanabe)

    // EBATECH(watanabe)-[changed
    ////Modified Cybage SBD 09/03/2001 DD/MM/YYYY  
    ////Reason : Fix for Bug No. 45931 from Bugzilla 
    ////if (fret==RTNONE) {
    //if (fret==RTNONE||(AddDimTextFlag==TRUE)) {
    //    // note: text always centered vertically, so pass point as 11 pt, not 10 pt
    //    cmd_dimaddmtext(layername,color,tmppt,txthgt,dimtxt,0.0,0.0,plinkvar[DIMTXSTY].dval.cval,txthjust,plink->pt210,SDS_CURDWG,0);
    if (fret==RTNORM) {
        sds_name prev;
        if (sds_entlast(prev) != RTNORM) goto exit;

        if (bCopy)
          {
            setgetrb.restype = RTSTR;
            setgetrb.resval.rstring = "*";
            elist = sds_entgetx(ename, &setgetrb);
            if (elist == NULL) goto exit;
            if (ic_assoc(elist, 0) != 0) goto exit;

            char* pStr = ic_rbassoc->resval.rstring;
            if (strsame("MTEXT"/*DNT*/, pStr))
              {
                // Copy MTEXT
                iAnno = 4;

                cmd_MoveLastLeaderPoint(pts, hookflg);
                sds_point_set((double*)pts[i], tmppt);

                cmd_MovePointWithDimgap(pts, hookflg, tmppt);

                if (ic_assoc(elist, 10) != 0) goto exit;
                sds_point_set(tmppt, ic_rbassoc->resval.rpoint);

                txthjust = cmd_GetLeaderAttachment(pts, hookflg);

                if (ic_assoc(elist, 71) != 0) goto exit;
                ic_rbassoc->resval.rint = txthjust;

                if (ic_assoc(elist, 50) != 0) goto exit;
                ic_rbassoc->resval.rreal = 0.0;
              }
            else if (strsame("TOLERANCE"/*DNT*/, pStr))
              {
                // Copy TOLERANCE
                iAnno = 5;

                cmd_MoveLastLeaderPoint(pts, hookflg);
                sds_point_set((double*)pts[i], tmppt);

                if (ic_assoc(elist, 10) != 0) goto exit;
                sds_point_set(tmppt, ic_rbassoc->resval.rpoint);
              }
            else if (strsame("INSERT"/*DNT*/, pStr))
              {
                // Copy INSERT (block reference)
                iAnno = 6;

                sds_point_set((double*)pts[i], tmppt);

                if (ic_assoc(elist, 10) != 0) goto exit;
                sds_point_set(tmppt, ic_rbassoc->resval.rpoint);
              }
            else if ((strsame("TEXT"/*DNT*/,   pStr)) ||
                     (strsame("ATTDEF"/*DNT*/, pStr)))
              {
                // Create MTEXT (not copy)
                iAnno = 0;

                if (ic_assoc(elist, 1) != 0) goto exit;
                strncpy(dimtxt, ic_rbassoc->resval.rstring, sizeof(dimtxt) - 1);

                if (ic_assoc(elist, 40) != 0) goto exit;
                txthgt = ic_rbassoc->resval.rreal;

                if (ic_assoc(elist, 7) != 0) goto exit;
                strncpy(dimtxsty, ic_rbassoc->resval.rstring, sizeof(dimtxsty) - 1);

                IC_RELRB(elist);
              }
            else
              {
                goto exit;
              }
          }

        sds_name mtext;
        switch (iAnno)
          {
            // MTEXT
            case 0:
              {
                //Bugzilla No 78096 15-04-2002 
				if(!splineseg)
                cmd_MoveLastLeaderPoint(pts, hookflg);
                sds_point_set((double*)pts[i], tmppt);

                cmd_MovePointWithDimgap(pts, hookflg, tmppt);

                txthjust = cmd_GetLeaderAttachment(pts, hookflg);

                // EBATECH(watanabe)-[color of mtext is according to DIMCLRT, not DIMCLRD
                int color = 256;
                struct resbuf rb;
                if (sds_getvar("DIMCLRT",&rb) == RTNORM)
                  {
                    color = rb.resval.rint;
                    if (color == 0)
                      {
						struct resbuf rb;
						SDS_getvar(NULL,DB_QCECOLOR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

						// Check for BYLAYER or BYBLOCK else assign the color as it is.
						if (!stricmp(rb.resval.rstring, DIM_BYLAYER)) //DB_BLCOLOR
							color = DB_BLCOLOR;   // color = 256;
						else if (!stricmp(rb.resval.rstring, DIM_BYBLOCK)) //DB_BBCOLOR
							color = DB_BBCOLOR;	  // color = 0;
						else
							color = atoi(rb.resval.rstring);

						IC_FREE(rb.resval.rstring);
                      }
                  }
                // ]-EBATECH(watanabe)

                // Create MTEXT
                // EBATECH(watanabe)-[FIX:ucs
                //cmd_dimaddmtext(
                //            layername, color, tmppt, txthgt, dimtxt,
                //            0.0, 0.0, dimtxsty,
                //            txthjust, plink->pt210, SDS_CURDWG, 0);

                // cross the two to get extrusion direction for new ent
                struct resbuf rbx, rby;
                SDS_getvar(NULL, DB_QUCSXDIR, &rbx,
                            SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
                SDS_getvar(NULL, DB_QUCSYDIR, &rby,
                            SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
                sds_point pt210;
                ic_crossproduct(rbx.resval.rpoint, rby.resval.rpoint, pt210);

                ucs2wcs(tmppt, 0);

                struct resbuf* rbMtext = sds_buildlist(
                            RTDXF0, "MTEXT"/*DNT*/,
                            8,      layername,
                            62,     color,
                            10,     tmppt,
                            40,     txthgt,
                            1,      dimtxt,
                            50,     0.0,
                            41,     0.0,
                            7,      dimtxsty,
                            71,     txthjust,
                            //6,      DIM_BYBLOCK,
						    6,      DIM_BYLAYER,  // default value for annotation
                            11,     rbx.resval.rpoint,
                            210,    pt210,
                            0);
                if (rbMtext == NULL)
                  {
                    goto exit;
                  }

                int irt = sds_entmake(rbMtext);
                IC_RELRB(rbMtext);
                if (irt != RTNORM)
                  {
                    goto exit;
                  }
                // ]-EBATECH(watanabe) FIX:ucs

                if (sds_entlast(mtext) != RTNORM) goto exit;
                if (ads_name_equal(prev, mtext)) goto exit;
                break;
              }

            // TOLERACE
            case 1:
                //Bugzilla No 78096 15-04-2002
				if(!splineseg)
                cmd_MoveLastLeaderPoint(pts, hookflg);
                sds_point_set((double*)pts[i], tmppt);

                // EBATECH(watanabe)-[FIX:ucs
                ucs2wcs(tmppt, 0);
                // ]-EBATECH(watanabe) FIX:ucs

                // Do TOLERANCE command
                ptTolerance = tmppt;
                ExecuteUIAction(ICAD_WNDACTION_TOLERANCE_EX);

                if (sds_entlast(mtext) != RTNORM) goto exit;
                if (ads_name_equal(prev, mtext)) goto exit;
                break;

            // INSERT (block references)
            case 2:
                // Do INSERT Command
                if (cmd_insert() != RTNORM) goto exit;
                if (sds_entlast(mtext) != RTNORM) goto exit;
                if (ads_name_equal(prev, mtext)) goto exit;
                break;

            // None
            case 3:
                ads_name_clear(mtext);
                break;

            // Copy
            default:
                if (sds_entmake(elist) != RTNORM) goto exit;
                IC_RELRB(elist);
                if (sds_entlast(mtext) != RTNORM) goto exit;
                if (ads_name_equal(prev, mtext)) goto exit;
                break;
          }

        // Direction for hook line
        int iHookDir = (abs(hookflg) == 1) ?
                            0:  // right
                            1;  // left

        // Annotation type
        int iAnnoType = iAnno;
        if (iAnnoType > 3)
          {
            iAnnoType = iAnno - 4;
          }

        elist = sds_buildlist(RTDXF0, db_hitype2str(DB_LEADER), RTNONE);
        struct resbuf* rbPrev = elist;
        struct resbuf* rbCur;
        for (int i = 0; i < pts.GetSize(); i++) {
            sds_point_set((double*)pts[i], p10);

            // EBATECH(watanabe)-[transformed vertex by ucs
            ucs2wcs(p10, 0);
            //]-EBATECH(watanabe)

            rbCur = sds_buildlist(10, p10, RTNONE);
            rbPrev->rbnext = rbCur;
            rbPrev = rbCur;
        }

        // get x direction
        // EBATECH(watanabe)-[x direction
        sds_point xdir;
        xdir[0] = 1;
        xdir[1] = 0;
        xdir[2] = 0;
        ucs2wcs(xdir);
        // ]-EBATECH(watanabe)

		//Bugzilla No. 78044 ; 25-03-2002 [
		sds_point offsettoblockinspt;
		offsettoblockinspt[0] = 0.0;
		offsettoblockinspt[1] = 0.0;
		offsettoblockinspt[2] = 0.0;

		if(iAnno==2){ //insert
			
			//get leader lastpt
			int i = pts.GetSize();
			sds_point_set((double*)pts[i-1], p10);
			ucs2wcs(p10, 0);

			//get inspt insert 
			struct sds_resbuf* inslist=NULL;
			inslist = sds_entget(mtext);
			if(inslist != NULL){
				ic_assoc(inslist,10);
			
				if(ic_rbassoc != NULL){

					offsettoblockinspt[0] = p10[0] - ic_rbassoc->resval.rpoint[0];
					offsettoblockinspt[1] = p10[1] - ic_rbassoc->resval.rpoint[1];
					offsettoblockinspt[2] = p10[2] - ic_rbassoc->resval.rpoint[2];
				}
				
				sds_relrb(inslist);
			}

		}
		//Bugzilla No. 78044 ; 25-03-2002 ]

        rbCur = sds_buildlist(
                                71, hasarrow,
                                72, splineseg,
                                73, iAnnoType,
                                74, iHookDir,
                                41, 0.0,
                                340, mtext,
                                //62, color, ////Bugzilla No 78140 20-05-2002  
                                // EBATECH(watanabe)-[x direction
                                211, xdir,
								// ]-EBATECH(watanabe)
								//Bugzilla No. 78044 ; 25-03-2002 
								212,offsettoblockinspt,
                                RTNONE);
        rbPrev->rbnext = rbCur;
        rbPrev = rbCur;

        cmd_buildEEdList(elist);

        if (sds_entmake(elist) != RTNORM) goto exit;

        if (sds_entlast(name) != RTNORM) goto exit;

        // EBATECH(watanabe)-[add reactor to annotation
        if (iAnnoType != 3) {
            IC_RELRB(elist);
            elist = sds_entget(mtext);
            if (elist != NULL) {
                rbPrev = elist;
                while (rbPrev->rbnext) rbPrev = rbPrev->rbnext;
                rbCur  = sds_buildlist(
                            102,    "{ACAD_REACTORS",
                            330,    name,
                            102,    "}",
                            0);
                rbPrev->rbnext = rbCur;
                rbPrev = rbCur;
                if (sds_entmod(elist) != RTNORM) goto exit;
            }
        }
        // ]-EBATECH(watanabe)

    // ]-EBATECH(watanabe)
    }

    // EBATECH(watanabe)-[comment
    //cmd_dimmode=0;
    //plinkvar[DIMASO].dval.ival=oldaso;
    // ]-EBATECH(watanabe)
    IC_RELRB(elist);					//  Was IC_FREEIT;
    // EBATECH(watanabe)-[comment
    //IC_RELRB(ptllbeg);

    // EBATECH(watanabe)-[add
    cmd_ReleaseLeaderPts(pts, ss);
    sds_redraw(name, IC_REDRAW_DRAW);
    // ]-EBATECH(watanabe)

	cmd_CleanUpDimVarStrings();			// Clean up first, then return ret.
    return(RTNORM);

    exit:
        // EBATECH(watanabe)-[comment
        //cmd_dimmode=0;
        //plinkvar[DIMASO].dval.ival=oldaso;
        // ]-EBATECH(watanabe)
        IC_RELRB(elist);				//  Was IC_FREEIT;
        // EBATECH(watanabe)-[comment
        //IC_RELRB(ptllbeg);
        // ]-EBATECH(watanabe)

        // EBATECH(watanabe)-[add
        cmd_ReleaseLeaderPts(pts, ss);
        // ]-EBATECH(watanabe)
		cmd_CleanUpDimVarStrings();		// Clean up first, then return ret.
        return(RTERROR);
}

// EBATECH(watanabe)-[here not used
#ifdef REMOVED
// This function takes the dxf code for a LEADER entity and creates the leader out of a solid
// and line segments.
short cmd_CreateLeader(const struct resbuf *elist) {
    struct resbuf *ptllbeg=NULL,
                  *newelist=NULL,
                  *tmprb=NULL,
                   setgetrb;
    sds_point      pt[4] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}},
                   tmppt = {0.0, 0.0, 0.0},
                   tmppt2 = {0.0, 0.0, 0.0};
    sds_real       fr1 = 0.0,
                   dimasz = 0.0,
                   dimgap = 0.0,
                   dimscale = 0.0;
    short          numdxf10s = 0,
                   hasarrow = 1;
    long           rc = 0L;

    if (ic_assoc((struct resbuf *)elist,0)!=0 || !strisame("LEADER"/*DNT*/,ic_rbassoc->resval.rstring)) return(RTERROR);
    if (ic_assoc((struct resbuf *)elist,76)!=0 || ic_rbassoc->resval.rint<2) return(RTERROR);
    numdxf10s=ic_rbassoc->resval.rint;
    if (ic_assoc((struct resbuf *)elist,10)!=0 || ic_rbassoc->rbnext==NULL || ic_rbassoc->rbnext->restype!=10) return(RTERROR);
    ptllbeg=ic_rbassoc;
    if (SDS_getvar(NULL,DB_QDIMASZ,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) goto exit;
    dimasz=setgetrb.resval.rreal;
    if (SDS_getvar(NULL,DB_QDIMGAP,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) goto exit;
    dimgap=setgetrb.resval.rreal;
    if (SDS_getvar(NULL,DB_QDIMSCALE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) goto exit;
    dimscale=icadRealEqual(setgetrb.resval.rreal,0.0) ? 1.0 : setgetrb.resval.rreal;
    fr1=sds_angle(ptllbeg->resval.rpoint,ptllbeg->rbnext->resval.rpoint);
    if (ic_assoc((struct resbuf *)elist,71)==0 && ic_rbassoc->resval.rstring==0) hasarrow=0;
    if (hasarrow) {
        // First point of solid
        ic_ptcpy(pt[0],ptllbeg->resval.rpoint);
        // Second point of solid
        sds_polar(pt[0],fr1,dimasz,tmppt);
        sds_polar(tmppt,(fr1-(IC_PI/2.0)),(dimasz/6.0),pt[1]);
        // Third and fourth point of solid
        sds_polar(tmppt,(fr1+(IC_PI/2.0)),(dimasz/6.0),pt[2]);
        ic_ptcpy(pt[3],pt[2]);
        // Create the arrowhead
        if ((newelist=sds_buildlist(RTDXF0,"SOLID"/*DNT*/,10,pt[0],11,pt[1],12,pt[2],13,pt[3],0))==NULL) {
            sds_printf(ResourceString(IDC_DIMENSIONS__NERROR__INS_93, "\nERROR: Insufficient memory (#%d)." ),__LINE__);        
            goto exit;
        }
        if (sds_entmake(newelist)!=RTNORM) goto exit;
    }
    // Create the line segments
    ic_ptcpy(tmppt,ptllbeg->resval.rpoint);
    for (tmprb=ptllbeg; tmprb!=NULL && tmprb->restype==10; ic_ptcpy(tmppt,tmppt2),tmprb=tmprb->rbnext) {
        ic_ptcpy(tmppt2,tmprb->resval.rpoint);
        if (icadRealEqual(tmppt[0],tmppt2[0]) && icadRealEqual(tmppt[1],tmppt2[1])) continue;
        if ((newelist=sds_buildlist(RTDXF0,db_hitype2str(DB_LINE),10,tmppt,11,tmppt2,0))==NULL) {
            sds_printf(ResourceString(IDC_DIMENSIONS__NERROR__INS_93, "\nERROR: Insufficient memory (#%d)." ),__LINE__);        
            goto exit;
        }
        if (sds_entmake(newelist)!=RTNORM) goto exit;
    }

    IC_RELRB(newelist);
    return(RTNORM);
    exit:
        IC_RELRB(newelist);
        return(RTERROR);
}
#endif // REMOVED
// ]-EBATECH(watanabe)
