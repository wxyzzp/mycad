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

extern sds_point    dimzeropt; // Needed to put this variable here because it gets initialized.
extern struct cmd_dimvars plinkvar[];
extern struct cmd_dimeparlink* plink;
//Modified Cybage SBD 13/11/2001 DD/MM/YYYY 
//Reason : Fix for Bug No. 77866 and 77867 from Bugzilla 
extern BOOL bdimUpdate;

// ============================================================================================================================================================
// ========================================================= DIAMETER RADIUS FUNCTIONS ========================================================================
// ============================================================================================================================================================

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// mode: 0 - called from cmd_dimcenter
//       1 - called from dimdiameter
//       2 - called from dimradius
//       3 - called from DIM - diameter
//       4 - called from DIM - radius
//
short cmd_DimArcOrCircleFunc(short mode,db_drawing *flp) 
	{
    struct resbuf *elist=NULL,
                  *newelist=NULL,
                   setgetrb,
                   rbucs,
                   rbwcs,
                   rbecs;
    sds_name       ename = {0L, 0L},
                   ename2 = {0L, 0L};
    char           dimtxt[IC_ACADBUF] = ""/*DNT*/,
                   fs1[IC_ACADBUF] = ""/*DNT*/,
                   fs2[IC_ACADBUF] = ""/*DNT*/;
    CString  	   pmt;
    sds_point      pt1 = {0.0, 0.0, 0.0},
                   pt2 = {0.0, 0.0, 0.0},
                   pt3 = {0.0, 0.0, 0.0},
                   endpt1 = {0.0, 0.0, 0.0},
                   endpt2 = {0.0, 0.0, 0.0},
                   vertexpt = {0.0, 0.0, 0.0};
    sds_real       dimcen = 0.0,
                   txtang = 0.0,
                   r42 = 0.0,
                   fr1 = 0.0;
    db_handitem   *elp = NULL;
    int            ret = 0,
                   plflag = 0,
                   dimtype = 0;
    struct gr_view *view=SDS_CURGRVW;
    long           fl1 = 0L;
	int				exactvert = 0,
					retval = 0,
					vert1no = 0,
					vert2no = 0;

    cmd_initialrbcs(&rbecs,&rbwcs,&rbucs,flp);
    ic_ptcpy(plink->pt210,rbecs.resval.rpoint);

	struct sds_resbuf rb;
	BOOL bInPaperSpace = FALSE;
	//Modified Cybage SBD 13/11/2001 DD/MM/YYYY 
	//Reason : Fix for Bug No. 77866 and 77867 from Bugzilla 
	bdimUpdate=FALSE;
	SDS_getvar(NULL,DB_QTILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if(rb.resval.rint==0)
	{
		SDS_getvar(NULL,DB_QCVPORT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		if(rb.resval.rint==1)
			bInPaperSpace=TRUE;
	}

    plink->txtangflag=FALSE;
    //useStoredValues=FALSE;
    plink->GenInitflg=TRUE;
    *dimtxt=0;
    switch(mode) 
		{
        case 3:
        case 1: dimtype=DIM_DIAMETER;
			break;
        case 4:
        case 2: dimtype=DIM_RADIUS;
			break;
		}
    dimtype|=DIM_BIT_6;
    for (;;) 
		{
        if ((ret=sds_entsel(ResourceString(IDC_DIMENSIONS__NSELECT_ARC_76, "\nSelect arc or circle: " ),ename,pt1))==RTCAN)
			goto exit;
        else if (ret==RTERROR) 
			{
            if (SDS_getvar(NULL,DB_QERRNO,&setgetrb,flp,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
				goto exit;
            if (setgetrb.resval.rint==OL_ENTSELPICK) 
				{
				cmd_ErrorPrompt(CMD_ERR_NOENT,0); 
				continue;
				} 
			else 
				goto exit;
            break;
			}
		else if(ret==RTNORM) 
			{
            sds_matrix mat;
            bool bInBlock = FALSE;

            IC_RELRB(elist);		// Changed to IC_RELRB().
            if ((elist=sds_entget(ename))==NULL) 
				{
				cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
				goto exit; 
				}
            if (ic_assoc(elist,0)!=0) 
				goto exit;
            if (strsame(db_hitype2str(DB_INSERT),ic_rbassoc->resval.rstring)) 
                {
                IC_RELRB(elist);
                bInBlock = TRUE;
                //sds_resbuf* refstkres;	/*D.G.*/
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
            if (strsame("ARC"/*DNT*/,ic_rbassoc->resval.rstring)) 
				{
                sds_resbuf rbtemp;
                rbtemp.restype=RT3DPOINT;
                if (ic_assoc(elist,210)!=0) 
					goto exit;
                ic_ptcpy(rbtemp.resval.rpoint,ic_rbassoc->resval.rpoint);
                for (newelist=elist;newelist!=NULL;newelist=newelist->rbnext) 
					{
                    switch(newelist->restype) 
						{
                        case 10:  ic_ptcpy(vertexpt,newelist->resval.rpoint); break;
                        case 40:  plink->distance=newelist->resval.rreal; break;
                        case 50:  plink->sangle=newelist->resval.rreal; break;
                        case 51:  plink->eangle=newelist->resval.rreal; break;
                        case 210: ic_ptcpy(rbecs.resval.rpoint,newelist->resval.rpoint);break;
						}
					}
                // Calculate the endpoints of the arc
                endpt1[0]=(vertexpt[0]+(plink->distance*(cos(plink->sangle))));
                endpt1[1]=(vertexpt[1]+(plink->distance*(sin(plink->sangle))));
                endpt1[2]=vertexpt[2];
                endpt2[0]=(vertexpt[0]+(plink->distance*(cos(plink->eangle))));
                endpt2[1]=(vertexpt[1]+(plink->distance*(sin(plink->eangle))));
                endpt2[2]=vertexpt[2];
                if(bInBlock)
                    xformpt(vertexpt,vertexpt,mat);

                sds_trans(vertexpt,&rbtemp,&rbucs,0,vertexpt);  // Transform the point from ECS to UCS.
				}
			else if (strsame("POLYLINE"/*DNT*/,ic_rbassoc->resval.rstring)) 
				{
                sds_resbuf rbtemp;
                // This code is called if a polyline was picked, so we need to find the segment closest to the point picked and do the dimenstion on only that segment.
				ic_assoc(elist,70);                                                                     // Get the polyline flag.
				if (ic_rbassoc->resval.rint & (IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH)) 
					{
					cmd_ErrorPrompt(CMD_ERR_ENTTYPE,0); 
					continue;
					}
				plflag=ic_rbassoc->resval.rint;
                if (plflag & IC_PLINE_3DPLINE)
					{
                    rbtemp.restype=RTSHORT;
                    rbtemp.resval.rint=0;
					}
				else 
					{
                    rbtemp.restype=RT3DPOINT;
                    ic_assoc(elist,210);
                    ic_ptcpy(rbtemp.resval.rpoint,ic_rbassoc->resval.rpoint);
					}
                fr1=cmd_pl_vtx_no(ename,pt1,1,&exactvert);                                              // Get the index of the vertex closest to the point picked.
                for (fl1=0;fl1<fr1;++fl1)                                                               // Step to the next entity of the vertex with the specified index.
                    {
					if (sds_entnext_noxref(ename,ename2)!=RTNORM)										// Get the next entity.
						return(-1);
                    memcpy((void *)ename,(void *)ename2,sizeof(ename2));
					}
                // Get the elist for the first entity.
				IC_RELRB(elist);	
                if ((elist=sds_entget(ename))==NULL) { cmd_ErrorPrompt(CMD_ERR_MOREMEM,0); goto exit; }
                ic_assoc(elist,10);
				ic_ptcpy(pt3,ic_rbassoc->resval.rpoint);                       // Copy the vertex of the first entity.
                ic_assoc(elist,42);
				r42=ic_rbassoc->resval.rreal;
                if (!r42) 
					{
					cmd_ErrorPrompt(CMD_ERR_ARCCIRCLE,0);
					continue; 
					}
                cmd_pline_entnext(ename,ename2,plflag);                                                 // find the next vertex. This works on closed and open polylines.
                // Get the elist for the second entity.
				IC_RELRB(elist);
                if ((elist=sds_entget(ename2))==NULL) { cmd_ErrorPrompt(CMD_ERR_MOREMEM,0); goto exit; }
                ic_assoc(elist,10); 
				ic_ptcpy(pt2,ic_rbassoc->resval.rpoint);                            // Copy the vertex of the seconed entity.
                ic_bulge2arc(pt3,pt2,r42,vertexpt,&plink->distance,&plink->sangle,&plink->eangle);
                if(bInBlock)
                    {
                    xformpt(pt2,pt2,mat);
                    xformpt(pt3,pt3,mat);
                    xformpt(vertexpt,vertexpt,mat);
                    }
                sds_trans(pt2,&rbtemp,&rbucs,0,pt2);                                                     // Transform the point from ECS to UCS.
                sds_trans(pt3,&rbtemp,&rbucs,0,pt3);                                                     // Transform the point from ECS to UCS.
                sds_trans(vertexpt,&rbtemp,&rbucs,0,vertexpt);                                                     // Transform the point from ECS to UCS.
                //*** Calculate the endpoints of the arc
                endpt1[0]=(vertexpt[0]+(plink->distance*(cos(plink->sangle))));
                endpt1[1]=(vertexpt[1]+(plink->distance*(sin(plink->sangle))));
                endpt1[2]=vertexpt[2];
                endpt2[0]=(vertexpt[0]+(plink->distance*(cos(plink->eangle))));
                endpt2[1]=(vertexpt[1]+(plink->distance*(sin(plink->eangle))));
                endpt2[2]=vertexpt[2];
				}
			else if (strsame("LWPOLYLINE"/*DNT*/,ic_rbassoc->resval.rstring)) 
				{
                sds_resbuf rbtemp;
				vert1no=0;
                // This code is called if a polyline was picked, so we need to find the segment closest to the point picked and do the dimenstion on only that segment.
				ic_assoc(elist,70);                                                                     // Get the polyline flag.
				plflag=ic_rbassoc->resval.rint;
	            rbtemp.restype=RT3DPOINT;
				ic_assoc(elist,210);
                ic_ptcpy(rbtemp.resval.rpoint,ic_rbassoc->resval.rpoint);
                vert1no=cmd_lwpl_vtx_no(ename,pt1,1,&exactvert);                                              // Get the index of the vertex closest to the point picked.
                // Get the first entity.
				db_lwpolyline *lwpl=(db_lwpolyline *)ename[0];
				lwpl->get_10(pt3,vert1no);
				lwpl->get_42(&r42,vert1no);
                if (!r42) 
					{
					cmd_ErrorPrompt(CMD_ERR_ARCCIRCLE,0);
					continue; 
					}
                vert2no=cmd_lwpline_entnext(vert1no,lwpl);                                                 // find the next vertex. This works on closed and open polylines.
                // Get the elist for the second entity.
                lwpl->get_10(pt2,vert2no);
                ic_bulge2arc(pt3,pt2,r42,vertexpt,&plink->distance,&plink->sangle,&plink->eangle);
                if(bInBlock)
                    {
                    xformpt(pt2,pt2,mat);
                    xformpt(pt3,pt3,mat);
                    xformpt(vertexpt,vertexpt,mat);
                    }
                sds_trans(pt2,&rbtemp,&rbucs,0,pt2);                                                     // Transform the point from ECS to UCS.
                sds_trans(pt3,&rbtemp,&rbucs,0,pt3);                                                     // Transform the point from ECS to UCS.
                sds_trans(vertexpt,&rbtemp,&rbucs,0,vertexpt);                                                     // Transform the point from ECS to UCS.
                //*** Calculate the endpoints of the arc
                endpt1[0]=(vertexpt[0]+(plink->distance*(cos(plink->sangle))));
                endpt1[1]=(vertexpt[1]+(plink->distance*(sin(plink->sangle))));
                endpt1[2]=vertexpt[2];
                endpt2[0]=(vertexpt[0]+(plink->distance*(cos(plink->eangle))));
                endpt2[1]=(vertexpt[1]+(plink->distance*(sin(plink->eangle))));
                endpt2[2]=vertexpt[2];
				}
			else if(strsame("CIRCLE"/*DNT*/,ic_rbassoc->resval.rstring)) 
				{
                sds_resbuf rbtemp;
                rbtemp.restype=RT3DPOINT;
                if (ic_assoc(elist,210)!=0) 
					goto exit;
                ic_ptcpy(rbtemp.resval.rpoint,ic_rbassoc->resval.rpoint);
                if (ic_assoc(elist,10)!=0) 
					goto exit;
                ic_ptcpy(vertexpt,ic_rbassoc->resval.rpoint);
                if (ic_assoc(elist,40)!=0)
					goto exit;

                plink->distance=ic_rbassoc->resval.rreal;
                plink->sangle=0.0;
                plink->eangle=(2.0*IC_PI);
                if(bInBlock)
                    xformpt(vertexpt,vertexpt,mat);
 
                sds_trans(vertexpt,&rbtemp,&rbucs,0,vertexpt); // Transform the point from ECS to UCS.
				}
			else 
				{
				cmd_ErrorPrompt(CMD_ERR_ARCCIRCLE,0);
                continue;
				}
            dimcen=(mode==1 || mode==3)?(2.0*plink->distance):plink->distance;
            if(bInPaperSpace)
                dimcen*=fabs(plinkvar[DIMLFAC].dval.rval);
            else
                dimcen*=(plinkvar[DIMLFAC].dval.rval < 0) ? 1 : plinkvar[DIMLFAC].dval.rval;
            switch(mode) 
				{
                case 0:
                    break;
                case 1:
                case 2:
					//EBATECH(CNBR) Bugzilla#78443 Use DIMZIN from current SYSVAR buf.
					cmd_rtos_dim(dimcen,plinkvar[DIMUNIT].dval.ival,plinkvar[DIMDEC].dval.ival,dimtxt, plinkvar[DIMZIN].dval.ival,plinkvar[DIMDSEP].dval.ival);
					//cmd_rtos_dim(dimcen,plinkvar[DIMUNIT].dval.ival,plinkvar[DIMDEC].dval.ival,dimtxt, DIMTYPEMAIN);
                    sprintf(fs2,ResourceString(IDC_DIMENSIONS__NDIMENSION__77, "\nDimension text = %s: " ),dimtxt);
                    sds_printf(fs2);
                    *dimtxt=0;
                    break;
                case 3:
                case 4:
					//EBATECH(CNBR) Bugzilla#78443 Use DIMZIN from current SYSVAR buf.
					cmd_rtos_dim(dimcen,plinkvar[DIMUNIT].dval.ival,plinkvar[DIMDEC].dval.ival,dimtxt, plinkvar[DIMZIN].dval.ival,plinkvar[DIMDSEP].dval.ival);
					//cmd_rtos_dim(dimcen,plinkvar[DIMUNIT].dval.ival,plinkvar[DIMDEC].dval.ival,dimtxt, DIMTYPEMAIN);
                    sprintf(fs2,ResourceString(IDC_DIMENSIONS__NDIMENSION__52, "\nDimension text <%s>: " ),dimtxt);
                    if ((ret=sds_getstring(1,fs2,fs1))!=RTNORM) goto exit;
                    if (*fs1) strncpy(dimtxt,fs1,sizeof(dimtxt)-1); else *dimtxt=0;
                    break;
				}
            break;
			}
		}
    if (mode==0) 
		{
        plinkvar[DIMASO].dval.ival=FALSE;
        cmd_CenterMark(vertexpt,flp,0);
        plinkvar[DIMASO].dval.ival=TRUE;
        goto ok;
		}
    plink->angle=sds_angle(vertexpt,pt1);
    ic_normang(&plink->sangle,&plink->eangle);
	// EBATECH(CNBR) -[ Bugzilla#78455 13-03-2003 DIMTMOVE=2
	plink->DefValidflg = ( plinkvar[DIMTMOVE].dval.ival == 2 ) ? TRUE : FALSE;
	// EBATECH(CNBR) ]-
    for (;;) 
		{
        switch(mode) 
			{
            case 3:
            case 4:
                if (sds_initget(SDS_RSG_NOVIEWCHG,ResourceString(IDC_DIMENSIONS_INITGET_ANGL_49, "Angle Text ~_Angle ~_Text" ))!=RTNORM)
					goto exit;
                pmt=ResourceString(IDC_DIMENSIONS__NANGLE_TEXT_50, "\nAngle/Text/<Location of dimension line>: ");
                break;
            case 1:
            case 2:
                if (sds_initget(SDS_RSG_NOVIEWCHG,ResourceString(IDC_DIMENSIONS_INITGET_MTEX_78, "Angle Text ~_Angle ~_Text" ))!=RTNORM)
					goto exit;
                pmt=ResourceString(IDC_DIMENSIONS__NMTEXT_ANGL_79, "\nAngle/Text/<Location of dimension line>: " );
                break;
			}

		elp=new db_dimension(flp);
        // The 13 point is being used for the center of the circle or arc this is only when the
        // dimension is first created. This point is only temporary it will be set to 0,0 in the dimension.
        if (dimtxt && *dimtxt)
			elp->set_1(dimtxt);
        elp->set_13(vertexpt);                                                                  // vertexpt is a UCS point.
        elp->set_70(dimtype);
        elp->set_53(txtang);
        elp->set_40(0.0);

        db_ucshelper(view->ucsorg,view->ucsaxis,view->elevation,pt1,pt2,&fr1);
        elp->set_210(plink->pt210);
        elp->set_51(fr1);
        elp->set_12(pt2);

		ename[0]=(long)elp;
		ename[1]=(long)flp;
		SDS_getvar(NULL,DB_QDRAGMODE,&setgetrb,flp,&SDS_CURCFG,&SDS_CURSES);

		// EBATECH(CNBR) -[ Bugzilla#78455 13-03-2003 DIMTMOVE=2
		if( plinkvar[DIMTMOVE].dval.ival == 2 )
		{
			ret=SDS_dragobject(-1,setgetrb.resval.rint,(sds_real *)ename,(double *)elp->retp_11(),0.0,pmt,NULL,pt3,NULL);
			elp->set_11(pt3);
			ic_ptcpy(plink->textmidpt,pt3);
		}
		else
		{
			// The 14 point is being used for the draging of the dimension, this is only when the dimension is first created.
			// this point is only temporary it will be set to 0,0 in the dimension.
			ret=SDS_dragobject(-1,setgetrb.resval.rint,(sds_real *)ename,(double *)elp->retp_14(),0.0,pmt,NULL,pt2,NULL);
			elp->set_14(pt2);
			ic_ptcpy(plink->pt14,pt2);
		}
		// EBATECH(CNBR) ]-

        cmd_makedimension(elp,flp,-1);
        if (plinkvar[DIMASO].dval.ival) 
			{
			delete elp; 
			elp=NULL; 
			}

		if (ret==RTERROR || ret==RTCAN)
			goto exit;
        if (ret==RTNONE) 
			{
			cmd_ErrorPrompt(CMD_ERR_TRYAGAIN,0);
			goto exit;
			}
        else if (ret==RTKWORD) 
			{
            if (sds_getinput(fs1)!=RTNORM) 
				goto exit;
            strupr(fs1);
            if (strsame("TEXT"/*DNT*/ ,fs1)) 
				{
                //*** ~| TODO AutoCAD calls the MTEXT command here.  Eventually we need to do the same.
				//EBATECH(CNBR) Bugzilla#78443 Use DIMZIN from current SYSVAR buf.
				cmd_rtos_dim(plink->dimvalue,plinkvar[DIMUNIT].dval.ival,plinkvar[DIMDEC].dval.ival,dimtxt, plinkvar[DIMZIN].dval.ival,plinkvar[DIMDSEP].dval.ival);
				//cmd_rtos_dim(plink->dimvalue,plinkvar[DIMUNIT].dval.ival,plinkvar[DIMDEC].dval.ival,dimtxt, DIMTYPEMAIN);
                sprintf(fs2,ResourceString(IDC_DIMENSIONS__NDIMENSION__52, "\nDimension text <%s>: " ),dimtxt);
                if((ret=sds_getstring(1,fs2,fs1))==RTERROR) goto exit;
                if (*fs1) 
					{
                    strcpy(plink->text,fs1);
                    strcpy(dimtxt,plink->text);
					}
                continue;
				}
			else if (strsame("ANGLE"/*DNT*/ ,fs1)) 
				{
                if ((ret=sds_getangle(NULL,ResourceString(IDC_DIMENSIONS__NANGLE_OF_T_54, "\nAngle of text: " ),&txtang))==RTERROR)
					goto exit;
                plink->txtangflag=TRUE;
                continue;
				}
			else 
				{
                cmd_ErrorPrompt(CMD_ERR_PT,0);
                continue;
				}
			}
        break;
		}

    plink->GenInitflg=FALSE;
    // Do the transformation from (UCS) to (NATIVE) format this is becouse when a dimension is created via entmake the points need to be in
    // native format for compatability when someone does an entmake from the command line.
    sds_trans(plink->defpt,&rbucs,&rbwcs,0,plink->defpt);
    sds_trans(plink->textmidpt,&rbucs,&rbecs,0,plink->textmidpt);
    sds_trans(plink->pt15,&rbucs,&rbwcs,0,plink->pt15);
    // Create Dimension as a block.
    if (plinkvar[DIMASO].dval.ival) 
		{
        db_ucshelper(view->ucsorg,view->ucsaxis,view->elevation,pt1,pt2,&fr1);
        if ((newelist=sds_buildlist(RTDXF0,"DIMENSION"/*DNT*/,
                                    10,plink->defpt,
                                    11,plink->textmidpt,
                                    12,pt2,
                                    70,plink->flags,
                                    1 ,plink->text,
                                    13,dimzeropt,
                                    14,dimzeropt,
                                    15,plink->pt15,
                                    16,dimzeropt,
                                    40,plink->leadlen,
                                    50,plink->dlnang,
                                    51,fr1,
                                    53,plink->textrot,
                                    210,rbecs.resval.rpoint,0))==NULL) { cmd_ErrorPrompt(CMD_ERR_MOREMEM,0); goto exit; }
        cmd_buildEEdList(newelist);
        ret=sds_entmake(newelist);
		}
	else 
		{
        elp->set_1(plink->text);
        cmd_makedimension(elp,flp,0);
		}

ok:
	retval=RTNORM;
	goto out;
exit:
	retval=RTERROR;
out:
	plink->eangle=0.0;
	plink->sangle=0.0;
	plink->GenInitflg=FALSE;
	plink->DefValidflg=FALSE;	// EBATECH(CNBR) Bugzilla#78455 13-03-2003 DIMTMOVE=2
	IC_RELRB(elist);	
	IC_RELRB(newelist);	
	return retval;
	}
