// =========================================================================================================================================
// ================================================================ Includes ===============================================================
// =========================================================================================================================================

#include "cmds.h"/*DNT*/
//BugZilla No. 78155; 27-05-2002
#include "IcadDoc.h"/*DNT*/

#include "dimensions.h"
#include "cmdsDimVars.h"
#include "IcadView.h"
#include "cmdsDimBlock.h"
#include "cmdsNotUsed.h"

// EBATECH(watanabe)-[text rotation
extern BOOL bTextFreeAng;
// ]-EBATECH(watanabe)

// =========================================================================================================================================
// =========================================================== External Variables ==========================================================
// =========================================================================================================================================

//extern BOOL         useStoredValues;
extern sds_point    dimzeropt; // Needed to put this variable here because it gets initialized.
extern char         cmd_string[256];
extern struct cmd_dimvars plinkvar[];
//BugZilla No. 78155; 27-05-2002
//extern struct cmd_dimlastpt lastdimpts;
extern struct cmd_dimeparlink* plink;

// ============================================================================================================================================================
// ========================================================= LINEAR FUNCTIONS =================================================================================
// ============================================================================================================================================================
//
// This area has only the functions that create the linear dimensions,(DIMLINEAR,DIMALIGNED,DIM - HOR,DIM - VER,DIM - ALIGNED,
// DIM - ROTATED).
//
// ============================================================================================================================================================

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This function gets called from several different places.
// The "mode" flag tells this function where it was called from.
// mode: 0 - called from DIMLINEAR
//       1 - called from DIM - HORIZONTAL
//       2 - called from DIM - VERTICAL
//       3 - called from DIM - ROTATED
//       4 - called from DIMALIGNED
//       5 - called from DIM - ALIGNED
//
// RETURNS: RTNORM  - Success.
//          RTCAN   - User cancel.
//          RTERROR - Misc. error occured.
//
short cmd_DimLinearFunc(short mode,db_drawing *flp) {
    char            dimtxt[IC_ACADBUF] = ""/*DNT*/,
                    fs1[IC_ACADBUF] = ""/*DNT*/,
                    fs2[IC_ACADBUF] = ""/*DNT*/;
    CString			pmt;
    sds_real        dimang = 0.0,
                    txtang = 0.0,
                    fr1 = 0.0,
                    radius = 0.0,
                    sangle = 0.0,
                    eangle = 0.0,
                    angle51 = 0.0;
    RT              ret = 0;
	int             dimtype = 0,
                    plflag = -1;
    sds_point       pt1 = {0.0, 0.0, 0.0},
                    pt2 = {0.0, 0.0, 0.0},
                    pt3 = {0.0, 0.0, 0.0},
					pt4 = {0.0, 0.0, 0.0};
    sds_name        ename = {0L, 0L},
                    ename2 = {0L, 0L},
                    ename3 = {0L, 0L};
    struct resbuf   setgetrb,
                    rb,
                   *rbb=NULL,
                   *newelist=NULL,
                   *elist=NULL,
                    rbucs,
                    rbwcs,
                    rbecs;
    long            fl1 = 0L,
                    fl2 = 0L;
	db_handitem    *elp=NULL;
    struct gr_view *view=SDS_CURGRVW;
	int				exactvert = 0;

    // EBATECH(watanabe)-[text rotation
    bTextFreeAng = TRUE;
    // ]-EBATECH(watanabe)

    cmd_initialrbcs(&rbecs,&rbwcs,&rbucs,flp);
    ic_ptcpy(plink->pt210,rbecs.resval.rpoint);

    *dimtxt            =0;
//    useStoredValues=FALSE;
    plink->txtangflag=FALSE;
	plink->HomeText = TRUE;	// EBATECH(CNBR) Bugzilla#78455 13-03-2003 Support DIMTMOVE
    plink->dimmode   =mode;
    switch(mode) {
        case 4:
        case 5: dimtype=DIM_ALIGNED; break;
    }
    dimtype|=DIM_BIT_6;
    switch(mode) {
        case 1: dimang=0.0; break;
        case 2: dimang=(IC_PI/2); break;
        case 3:
            if (sds_initget(SDS_RSG_NOVIEWCHG,NULL)!=RTNORM) goto exit;
            if ((ret=sds_getangle(NULL,ResourceString(IDC_DIMENSIONS__NANGLE_OF_D_43, "\nAngle of dimension line <0>: " ),&dimang))==RTERROR || ret==RTCAN) goto exit;
            break;
    }
    // Getting a point in (UCS).
    if ((ret=sds_getpoint(NULL,ResourceString(IDC_DIMENSIONS__NENTER_TO_S_44, "\nENTER to select entity/<Origin of first extension line>: " ),pt1))==RTERROR) goto exit;
    if (ret==RTCAN) goto exit;
    if (ret==RTNONE) {
        for (;;) {
            sds_matrix mat;
            bool bInBlock = FALSE;

            // Getting a point in (UCS).
            if ((ret=sds_entsel(ResourceString(IDC_DIMENSIONS__NSELECT_ENT_45, "\nSelect entity to dimension: " ),ename,pt1))==RTERROR) {
                if (SDS_getvar(NULL,DB_QERRNO,&setgetrb,flp,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) goto exit;
                if (setgetrb.resval.rint==OL_ENTSELPICK) continue; else if (setgetrb.resval.rint==OL_ENTSELNULL) goto exit;
            } else if (ret==RTCAN || ret==RTNONE) goto exit;
			IC_RELRB(elist);  
            if ((elist=sds_entget(ename))==NULL) { cmd_ErrorPrompt(CMD_ERR_MOREMEM,0); goto exit; }
            if (ic_assoc(elist,0)!=0) goto exit;
            if (strsame(db_hitype2str(DB_INSERT),ic_rbassoc->resval.rstring)) 
                {
                IC_RELRB(elist);
                bInBlock = TRUE;
                //sds_resbuf* refstkres; // EBATECH(CNBR) 13-03-2003 Never use refstkres
                if ( (ret = sds_nentselp(NULL,ename,pt1,1,mat,NULL/*&refstkres*/)) == RTERROR )
                    {
                    if (SDS_getvar(NULL,DB_QERRNO,&setgetrb,flp,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) 
                        goto exit;
                    if (setgetrb.resval.rint==OL_ENTSELPICK) 
                        continue; 
                    else if (setgetrb.resval.rint==OL_ENTSELNULL) 
                        goto exit;
                    }
                else if (ret==RTCAN || ret==RTNONE) 
                    goto exit;

                //IC_RELRB(refstkres);
                if ((elist=sds_entget(ename))==NULL) 
                    { 
                    cmd_ErrorPrompt(CMD_ERR_MOREMEM,0); 
                    goto exit; 
                    }
                if (ic_assoc(elist,0)!=0) 
                    goto exit;
                }
            if (strsame(db_hitype2str(DB_LINE),ic_rbassoc->resval.rstring)) {
    			sds_osnap(pt1,"END"/*DNT*/,pt1);
                if (ic_assoc(elist,10)!=0) goto exit;
                if (!icadPointEqual(pt1,ic_rbassoc->resval.rpoint)) {
                    ic_ptcpy(pt1,ic_rbassoc->resval.rpoint); // Getting a point in (UCS).
                    if(bInBlock)
                        xformpt(pt1,pt1,mat);
                    sds_trans(pt1,&rbwcs,&rbucs,0,pt1);                                                      // Transforming from WCS to UCS
                }
                if (ic_assoc(elist,11)!=0) goto exit;
                if (!icadPointEqual(pt1,ic_rbassoc->resval.rpoint)) {
                    ic_ptcpy(pt2,ic_rbassoc->resval.rpoint);                                                 // Getting a point in (UCS).
                    if(bInBlock)                        
                        xformpt(pt2,pt2,mat);
                    sds_trans(pt2,&rbwcs,&rbucs,0,pt2);                                                      // Transforming from WCS to UCS
                }
            } else if (strsame("ARC"/*DNT*/,ic_rbassoc->resval.rstring)) {
                sds_resbuf rbtemp;
                rbtemp.restype=RT3DPOINT;
                if (ic_assoc(elist,210)!=0) goto exit;
                ic_ptcpy(rbtemp.resval.rpoint,ic_rbassoc->resval.rpoint);
                for (rbb=elist;rbb;rbb=rbb->rbnext) {
                    switch(rbb->restype) {
                        case  10: ic_ptcpy(pt3,rbb->resval.rpoint); break;
                        case  40: radius=rbb->resval.rreal; break;
                        case  50: sangle=rbb->resval.rreal; break;
                        case  51: eangle=rbb->resval.rreal; break;
                    }
                }
                // Calculate the endpoints of the arc
                pt1[0]=(pt3[0]+(radius*(cos(sangle))));
                pt1[1]=(pt3[1]+(radius*(sin(sangle))));
                pt1[2]=pt3[2];
                pt2[0]=(pt3[0]+(radius*(cos(eangle))));
                pt2[1]=(pt3[1]+(radius*(sin(eangle))));
                pt2[2]=pt3[2];
                if(bInBlock)   
                    {
                    xformpt(pt1,pt1,mat);
                    xformpt(pt2,pt2,mat);
                    }
                sds_trans(pt1,&rbtemp,&rbucs,0,pt1);                                                     // Transform the point from ECS to UCS.
                sds_trans(pt2,&rbtemp,&rbucs,0,pt2);                                                     // Transform the point from ECS to UCS.
            } else if (strsame("CIRCLE"/*DNT*/,ic_rbassoc->resval.rstring)) {
                sds_resbuf rbtemp;
                rbtemp.restype=RT3DPOINT;
                if (ic_assoc(elist,210)!=0) goto exit;
                ic_ptcpy(rbtemp.resval.rpoint,ic_rbassoc->resval.rpoint);
                for (rbb=elist;rbb;rbb=rbb->rbnext) {
                    switch(rbb->restype) {
                        case  10: ic_ptcpy(pt3,rbb->resval.rpoint); break;
                        case  40: radius=rbb->resval.rreal; break;
                    }
                }
                switch(mode) {
                    case 1:
            		    ic_ptcpy(pt1,pt3);
					    pt1[0]+=radius;
					    ic_ptcpy(pt2,pt3);
					    pt2[0]-=radius;
                        break;
                    case 2:
					    ic_ptcpy(pt1,pt3);
					    pt1[1]+=radius;
					    ic_ptcpy(pt2,pt3);
					    pt2[1]-=radius;
                        break;
                    case 0:
                    case 3:
                        sds_polar(pt3,0,radius,pt1);
                        sds_polar(pt3,IC_PI,radius,pt2);
                        break;
                    case 4:
                    case 5:
                        fr1=sds_angle(pt1,pt3);
					    sds_polar(pt3,fr1,radius,pt2);
					    fr1+=IC_PI;
					    sds_polar(pt3,fr1,radius,pt1);
                        break;
                }
                if(bInBlock)   
                    {
                    xformpt(pt1,pt1,mat);
                    xformpt(pt2,pt2,mat);
                    }
                sds_trans(pt1,&rbtemp,&rbucs,0,pt1);                                                     // Transform the point from ECS to UCS.
                sds_trans(pt2,&rbtemp,&rbucs,0,pt2);                                                     // Transform the point from ECS to UCS.
			} else if (strsame("POLYLINE"/*DNT*/,ic_rbassoc->resval.rstring)){
                sds_resbuf rbtemp;
                // This code is called if a polyline was picked, so we need to find the segment closest to the point picked and do the dimenstion on only that segment.
				ic_assoc(elist,70);                                                                      // Get the polyline flag.
                if (ic_rbassoc->resval.rint&(IC_PLINE_POLYFACEMESH | IC_PLINE_3DMESH)) { cmd_ErrorPrompt(CMD_ERR_ENTTYPE,0); continue; }   // Continue if the polyline is a 16=3D polygon mesh or 64=polyface mesh.
				plflag=ic_rbassoc->resval.rint;
                if (plflag & IC_PLINE_3DPLINE){
                    rbtemp.restype=RTSHORT;
                    rbtemp.resval.rint=0;
                } else {
                    rbtemp.restype=RT3DPOINT;
                    ic_assoc(elist,210);
                    ic_ptcpy(rbtemp.resval.rpoint,ic_rbassoc->resval.rpoint);
                }
                fl2=cmd_pl_vtx_no(ename,pt1,1,&exactvert);                                                          // Get the index of the vertex closest to the point picked.
				ic_encpy(ename2,ename);
                for (fl1=0;fl1<fl2;++fl1) { if (sds_entnext_noxref(ename2,ename2)!=RTNORM) goto exit; }          // Step to the next entity of the vertex with the specified index.
                // Get the elist for the first entity.
				IC_RELRB(elist); 
                if ((elist=sds_entget(ename2))==NULL) { cmd_ErrorPrompt(CMD_ERR_MOREMEM,0); goto exit; }
                ic_assoc(elist,10); ic_ptcpy(pt1,ic_rbassoc->resval.rpoint); // Copy the vertex of the first entity.
                if(bInBlock)   
                    xformpt(pt1,pt1,mat);
                sds_trans(pt1,&rbtemp,&rbucs,0,pt1);                                                     // Transform the point from ECS to UCS.
                cmd_pline_entnext(ename2,ename3,plflag);                                                 // find the next vertex. This works on closed and open polylines.
                // Get the elist for the second entity.
				IC_RELRB(elist);   //  was:  if (elist!=NULL){ sds_relrb(elist); elist=NULL; }
                if ((elist=sds_entget(ename3))==NULL) { cmd_ErrorPrompt(CMD_ERR_MOREMEM,0); goto exit; }
                ic_assoc(elist,10); ic_ptcpy(pt2,ic_rbassoc->resval.rpoint);                             // Copy the vertex of the seconed entity.
                if(bInBlock)   
                    xformpt(pt2,pt2,mat);
                sds_trans(pt2,&rbtemp,&rbucs,0,pt2);                                                     // Transform the point from ECS to UCS.
			} else if (strsame("LWPOLYLINE"/*DNT*/,ic_rbassoc->resval.rstring)){
				db_lwpolyline *lwpl=(db_lwpolyline *)ename[0];
                sds_resbuf rbtemp;
                // This code is called if a polyline was picked, so we need to find the segment closest to the point picked and do the dimenstion on only that segment.
				ic_assoc(elist,70);                                                                      // Get the polyline flag.
				plflag=ic_rbassoc->resval.rint;
                rbtemp.restype=RT3DPOINT;
                ic_assoc(elist,210);
                ic_ptcpy(rbtemp.resval.rpoint,ic_rbassoc->resval.rpoint);
                fl2=cmd_lwpl_vtx_no(ename,pt1,1,&exactvert);                                                          // Get the index of the vertex closest to the point picked.
                // Get the elist for the first entity.
				lwpl->get_10(pt1,fl2);
                if(bInBlock)   
                    xformpt(pt1,pt1,mat);
                sds_trans(pt1,&rbtemp,&rbucs,0,pt1);                                                     // Transform the point from ECS to UCS.
				fl2++;
                if (fl2>=lwpl->ret_90())
					fl2=0;
				lwpl->get_10(pt2,fl2);
                // Get the elist for the second entity.
                if(bInBlock)   
                    xformpt(pt2,pt2,mat);
                sds_trans(pt2,&rbtemp,&rbucs,0,pt2);                                                     // Transform the point from ECS to UCS.
            } else {
				cmd_ErrorPrompt(CMD_ERR_LINEARCCIRPLY,0);
                continue;
            }
            switch(mode) {
                case 4:
                case 5:
                    dimang=sds_angle(pt1,pt2);
                    break;
                case 0:
                    dimang=0.0;
                    break;
            }
            break;
        }
    } else if ((ret=sds_getpoint(pt1,ResourceString(IDC_DIMENSIONS__NORIGIN_OF__46, "\nOrigin of second extension line: " ),pt2))==RTNORM) {              // Getting a point in (UCS).
        switch(mode) {
            case 0:
                dimang=0.0;
                break;
            case 4:
                dimang=sds_angle(pt1,pt2);
                break;
            case 5:
                dimang=sds_angle(pt1,pt2);
                while(dimang>=IC_PI) dimang-=IC_PI;
                break;
        }
    } else goto exit;
    cmd_make_51_group(&angle51,flp);
    for (;;) {
        switch(mode) {
            case 0:
                if (sds_initget(SDS_RSG_NOVIEWCHG,ResourceString(IDC_DIMENSIONS_INITGET_ANGL_47, "Angle Text Horizontal Vertical Rotated ~_Angle ~_Text ~_Horizontal ~_Vertical ~_Rotated" ))!=RTNORM) goto exit;
				pmt=ResourceString(IDC_DIMENSIONS__NANGLE_TEXT_48, "\nAngle/Text/Orientation of dimension line:  Horizontal/Vertical/Rotated: " );
                break;
            case 4:
//              if (sds_initget(SDS_RSG_NOVIEWCHG,"Angle Mtext Text")!=RTNORM) goto exit;
//				pmt="\nAngle/Mtext/Text/<Location of dimension line>: ";
//                if (sds_initget(SDS_RSG_NOVIEWCHG,ResourceString(IDC_DIMENSIONS_INITGET_ANGL_49, "Angle Text ~_Angle ~_Text" ))!=RTNORM) goto exit;
//				pmt=ResourceString(IDC_DIMENSIONS__NANGLE_TEXT_50, "\nAngle/Text/<Location of dimension line>: " );
//                break;
            case 1:
            case 2:
            case 3:
            case 5:
                if (sds_initget(SDS_RSG_NOVIEWCHG,ResourceString(IDC_DIMENSIONS_INITGET_ANGL_49, "Angle Text ~_Angle ~_Text" ))!=RTNORM) goto exit;
				pmt=ResourceString(IDC_DIMENSIONS__NANGLE_TEXT_50, "\nAngle/Text/<Location of dimension line>: " );
                break;
        }
        elp=new db_dimension(flp);
        elp->set_13(pt1);                                                                                // point 13 is in (UCS).
        elp->set_14(pt2);                                                                                // point 14 is in (UCS).
        elp->set_50(dimang);
        elp->set_53(txtang);
        elp->set_1(dimtxt);
        elp->set_11(dimzeropt);
        elp->set_10(dimzeropt);
        elp->set_51(angle51);
        elp->set_70(dimtype);

        db_ucshelper(view->ucsorg,view->ucsaxis,view->elevation,pt3,pt4,&fr1);
		if(fr1 && mode != 4 && mode !=5)
		{
			dimang += (2 * IC_PI - fr1);
	        elp->set_50(dimang);
		}
        elp->set_210(plink->pt210);
        elp->set_51(fr1);
        elp->set_12(pt3);

		ename2[0]=(long)elp;
		ename2[1]=(long)flp;
		SDS_getvar(NULL,DB_QDRAGMODE,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
        ret=SDS_dragobject(-1,rb.resval.rint,(sds_real *)ename2,(double *)elp->retp_10(),0.0,pmt,NULL,pt3,NULL);

        elp->set_10(pt3);
        ic_ptcpy(plink->defpt,pt3);
        ic_ptcpy(plink->pt13,pt1);
        ic_ptcpy(plink->pt14,pt2);

        cmd_makedimension(elp,flp,-1);
        if (plinkvar[DIMASO].dval.ival) { if(elp) { delete elp; elp=NULL; } }
        if (ret==RTERROR || ret==RTCAN) goto exit;
        if (ret==RTNONE) { cmd_ErrorPrompt(CMD_ERR_TRYAGAIN,0); goto exit; }
        else if (ret==RTKWORD) {
            if (sds_getinput(fs1)!=RTNORM) goto exit;
            strupr(fs1);
            if (strsame("TEXT"/*DNT*/,fs1)) {
                //
                //*** ~| TODO AutoCAD calls the MTEXT command here.  Eventually we need to do the same.
                //
                // If there has not been a text value entered then use the value in DIMVALUE.
				//EBATECH(CNBR) Bugzilla#78443 Use DIMZIN from current SYSVAR buf.
				cmd_rtos_dim(plink->dimvalue,plinkvar[DIMUNIT].dval.ival,plinkvar[DIMDEC].dval.ival,dimtxt,plinkvar[DIMZIN].dval.ival,plinkvar[DIMDSEP].dval.ival);
				//cmd_rtos_dim(plink->dimvalue,plinkvar[DIMUNIT].dval.ival,plinkvar[DIMDEC].dval.ival,dimtxt, DIMTYPEMAIN);
                sprintf(fs2,ResourceString(IDC_DIMENSIONS__NDIMENSION__52, "\nDimension text <%s>: " ),dimtxt);
                if ((ret=sds_getstring(1,fs2,fs1))==RTERROR || ret==RTCAN) goto exit;
                if (*fs1) {
                    strcpy(plink->text,fs1);
                    strcpy(dimtxt,plink->text);
                }
                // If there has not been text entered then don't save the value as text.
                if (!plink->text && !(*plink->text)) *dimtxt=0;
                 continue;
            } else if (strsame("ANGLE"/*DNT*/,fs1)) {
                if ((ret=sds_getangle(NULL,ResourceString(IDC_DIMENSIONS__NANGLE_OF_T_54, "\nAngle of text: " ),&txtang))==RTERROR || ret==RTCAN) goto exit;
                plink->txtangflag=TRUE;
                // EBATECH(watanabe)-[
                bTextFreeAng = FALSE;
                if (icadAngleEqual(txtang,0.0))
                  {
                    bTextFreeAng = TRUE;
                  }
                // ]-EBATECH(watanabe)
                 continue;
            } else if (strsame("HORIZONTAL"/*DNT*/,fs1)) {
                plink->dimmode=mode=1; dimang=0.0;
                continue;
            } else if (strsame("VERTICAL"/*DNT*/,fs1)) {
                plink->dimmode=mode=2; dimang=(IC_PI/2);
                continue;
            } else if (strsame("ROTATED"/*DNT*/,fs1)) {
                if (sds_initget(SDS_RSG_NOVIEWCHG,NULL)!=RTNORM) goto exit;
                if ((ret=sds_getangle(NULL,ResourceString(IDC_DIMENSIONS__NANGLE_OF_D_43, "\nAngle of dimension line <0>: " ),&dimang))==RTERROR || ret==RTCAN) goto exit;
                continue;
            } else if (strsame("MTEXT"/*DNT*/,fs1)) {
                // Need to do some Mtext.
				continue;
            } else {
                cmd_ErrorPrompt(CMD_ERR_PT,0);
                continue;
            }
        }
        break;
    }
    if (!(*dimtxt) && mode!=0 && mode!=4) {
        // If there has not been a text value entered then use the value in DIMVALUE.
		//EBATECH(CNBR) Bugzilla#78443 Use DIMZIN from current SYSVAR buf.
		cmd_rtos_dim(plink->dimvalue,plinkvar[DIMUNIT].dval.ival,plinkvar[DIMDEC].dval.ival,dimtxt, plinkvar[DIMZIN].dval.ival,plinkvar[DIMDSEP].dval.ival);
		//cmd_rtos_dim(plink->dimvalue,plinkvar[DIMUNIT].dval.ival,plinkvar[DIMDEC].dval.ival,dimtxt, DIMTYPEMAIN);
		sprintf(fs2,ResourceString(IDC_DIMENSIONS__NDIMENSION__52, "\nDimension text <%s>: " ),dimtxt);
        if ((ret=sds_getstring(1,fs2,fs1))==RTERROR || ret==RTCAN) goto exit;
        if (*fs1) { strcpy(plink->text,fs1); cmd_genstring(plink,dimtxt,plink->dimvalue,""/*DNT*/,TRUE,flp); }
	}
    // Copy data to the last points picked structure.
	cmd_savelast(plink,view);
    // Do the transformation from (UCS) to (NATIVE) format this is becouse when a dimension is created via entmake the points need to be in
    // native format for compatibility when someone does an entmake from the command line.
    sds_trans(plink->defpt,&rbucs,&rbwcs,0,plink->defpt);
    sds_trans(plink->textmidpt,&rbucs,&rbecs,0,plink->textmidpt);
    sds_trans(plink->pt13,&rbucs,&rbwcs,0,plink->pt13);
    sds_trans(plink->pt14,&rbucs,&rbwcs,0,plink->pt14);
    // Create the dimension as a block.
    if (plinkvar[DIMASO].dval.ival) {
        db_ucshelper(view->ucsorg,view->ucsaxis,view->elevation,pt1,pt2,&fr1);
        if ((newelist=sds_buildlist(RTDXF0,"DIMENSION"/*DNT*/,
                                        1 ,plink->text,
                                        10,plink->defpt,
                                        11,plink->textmidpt,
                                        12,pt2,
                                        13,plink->pt13,
                                        14,plink->pt14,
                                        40,0.0,
                                        50,plink->dlnang,
                                        51,fr1,
                                        52,0.0,
                                        53,plink->textrot,
                                        70,plink->flags,
                                        210,rbecs.resval.rpoint,0))==NULL) { cmd_ErrorPrompt(CMD_ERR_MOREMEM,0); goto exit; }
        cmd_buildEEdList(newelist);
        if ((ret=sds_entmake(newelist))!=RTNORM)         goto exit;
        if ((ret=sds_entlast(lastdimpts.ename))!=RTNORM) goto exit;
    } else {
        elp->set_1(plink->text);
        elp->set_13(plink->pt13);                                                                                // point 13 is in (UCS).
        elp->set_14(plink->pt14);                                                                                // point 14 is in (UCS).
        elp->set_11(plink->textmidpt);
        elp->set_10(plink->defpt);
        cmd_makedimension(elp,flp,0);
        if (elp) {delete elp; elp=NULL;}  
    }

	IC_RELRB(elist);			//  (fixed leak w/selecting entities in dimlinear)
    IC_RELRB(newelist);
	plink->HomeText = FALSE;	// EBATECH(CNBR) Bugzilla#78455 13-03-2003 Support DIMTMOVE
	return(RTNORM);
exit:
	if (elp) {delete elp; elp=NULL;}
	IC_RELRB(elist);		// Changed to IC_RELRB(). 
	IC_RELRB(newelist);
	plink->HomeText = FALSE;	// EBATECH(CNBR) Bugzilla#78455 13-03-2003 Support DIMTMOVE
	return(ret);
}
