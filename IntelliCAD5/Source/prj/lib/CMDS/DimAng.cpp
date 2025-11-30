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

// =========================================================================================================================================
// =========================================================== External Variables ==========================================================
// =========================================================================================================================================

//extern BOOL         useStoredValues;
extern sds_point    dimzeropt; // Needed to put this variable here because it gets initialized.
extern struct cmd_dimvars plinkvar[];
//BugZilla No. 78155; 27-05-2002
//extern struct cmd_dimlastpt lastdimpts;
extern struct cmd_dimeparlink* plink;

// ============================================================================================================================================================
// ========================================================= ANGULAR DIMENSION FUNCTIONS =====================================================================
// ============================================================================================================================================================

int cmd_compare1(const void *arg1,const void *arg2) {
    return((*(sds_real *)arg1)>=(*(sds_real *)arg2));
}

sds_real cmd_CalculateAngle(sds_point center,sds_point firstver,sds_point secondver) {
    sds_real fr1 = 0.0,
             fr2 = 0.0,
             fr3 = 0.0,
             angle = 0.0;

    // Get the vector lengths of the angles made.
    fr1=sqrt(((center[0]-firstver[0])*(center[0]-firstver[0]))+((center[1]-firstver[1])*(center[1]-firstver[1])));
    fr2=sqrt(((center[0]-secondver[0])*(center[0]-secondver[0]))+((center[1]-secondver[1])*(center[1]-secondver[1])));
    // Calculate the angle that the vectors will make, this will always be the smaller angle.
    fr3=((((firstver[0]-center[0])*(secondver[0]-center[0]))+((firstver[1]-center[1])*(secondver[1]-center[1])))/(fr1*fr2));
    if (fr3>1.0)  fr3=1.0;
    if (fr3<-1.0) fr3=-1.0;
    angle=acos(fr3);
    return(angle);
}

short cmd_dimangular(BOOL mode,db_drawing *flp) {
    char            fs1[IC_ACADBUF] = ""/*DNT*/,
                    fs2[IC_ACADBUF] = ""/*DNT*/,
                   *sptr1 = NULL,
                    dimtxt[IC_ACADBUF];
    long            ret = 0L,
                    fl1 = 0L,
                    dimtype = 0L;
    struct resbuf   setgetrb,
                   *elist=NULL,
                    rbucs,
                    rbwcs,
                    rbecs;
    sds_point       pt1 = {0.0, 0.0, 0.0},
                    pt2 = {0.0, 0.0, 0.0},
                    pt3 = {0.0, 0.0, 0.0},
                    vertexpt = {0.0, 0.0, 0.0};
    sds_name        ename = {0L, 0L},
                    ename2 = {0L, 0L};
	db_handitem    *elp=NULL;
    sds_real        stang = 0.0,
                    endang = 0.0,
                    txtang = 0.0,
                    rad = 0.0,
                    fr1 = 0.0;
    int             plflag = 0;
    BOOL            loopflag = FALSE;
    struct gr_view *view=SDS_CURGRVW;
                    sds_resbuf rbtemp;
	int				exactvert = 0;

    //BugZilla No. 78149; 13-05-2002
	//cmd_initialize();
    //cmd_setparlink(flp);

    cmd_initialrbcs(&rbecs,&rbwcs,&rbucs,flp);
    ic_ptcpy(plink->pt210,rbecs.resval.rpoint);
//    useStoredValues=FALSE;
    loopflag=TRUE; *dimtxt=0; dimtype=DIM_BIT_6;
	plink->HomeText = TRUE;
    for (;;) {
        sds_matrix mat;
        bool bInBlock = FALSE;

        if (loopflag) {
            if ((ret=sds_entsel(ResourceString(IDC_DIMENSIONS__NPRESS_ENT_107, "\nPress ENTER to specify angle/<Select line, arc, or circle>: " ),ename,pt1))==RTCAN) goto exit;
        } else {
            if ((ret=sds_entsel(ResourceString(IDC_DIMENSIONS__NOTHER_LIN_108, "\nOther line for angular dimension: " ),ename,pt2))==RTCAN) goto exit;
        }
        if (ret==RTERROR) {
            if (SDS_getvar(NULL,DB_QERRNO,&setgetrb,flp,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) goto exit;
            if (setgetrb.resval.rint==OL_ENTSELPICK) {
                cmd_ErrorPrompt(CMD_ERR_NOENT,0);
                continue;
            } else if(setgetrb.resval.rint==OL_ENTSELNULL) {
                if ((ret=sds_getpoint(NULL,ResourceString(IDC_DIMENSIONS__NVERTEX_OF_109, "\nVertex of angle: " ),vertexpt))!=RTNORM) goto exit;
                if ((ret=sds_getpoint(vertexpt,ResourceString(IDC_DIMENSIONS__NFIRST_SID_110, "\nFirst side of angle: " ),pt1))!=RTNORM) goto exit;
                if ((ret=sds_getpoint(vertexpt,ResourceString(IDC_DIMENSIONS__NOTHER_SID_111, "\nOther side of angle: " ),pt2))!=RTNORM) goto exit;
                // Set the type of the dimension to angular 3 point.
                dimtype|=DIM_ANGULAR_3P;
            } else goto exit;
            break;
        } else if (ret==RTNORM) {
            IC_RELRB(elist);  
            if ((elist=sds_entget(ename))==NULL) { cmd_ErrorPrompt(CMD_ERR_MOREMEM,0); goto exit; }
            if (ic_assoc(elist,0)!=0) goto exit;
            if (strsame(db_hitype2str(DB_INSERT),ic_rbassoc->resval.rstring)) 
                {
                IC_RELRB(elist);
                bInBlock = TRUE;
                //sds_resbuf* refstkres; // Never use refstkres. EBATECH(CNBR) 03-13-2003

                if ( (ret = sds_nentselp(NULL,ename,loopflag?pt1:pt2,1,mat,NULL/*&refstkres*/)) == RTERROR )
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
                if (loopflag) {
			        sds_osnap(pt1,"NEA" ,pt1);
                    if (ic_assoc(elist,10)!=0) goto exit;
                    ic_ptcpy(plink->ptt[0],ic_rbassoc->resval.rpoint);
                    if(ic_assoc(elist,11)!=0)goto exit;
                    ic_ptcpy(plink->ptt[1],ic_rbassoc->resval.rpoint);
                    if(bInBlock)
                        {
                        xformpt(plink->ptt[0],plink->ptt[0],mat);
                        xformpt(plink->ptt[1],plink->ptt[1],mat);
                        }

                    sds_trans(plink->ptt[0],&rbwcs,&rbucs,0,plink->ptt[0]);     // Transforming from WCS to UCS
                    sds_trans(plink->ptt[1],&rbwcs,&rbucs,0,plink->ptt[1]);     // Transforming from WCS to UCS
                    loopflag=FALSE; continue;
                } else {
                    // Set the type of the dimension to angular.
					sds_osnap(pt2,"NEA" ,pt2);
                    dimtype|=DIM_ANGULAR;
                    if (ic_assoc(elist,0)!=0) goto exit;
                    if (!strsame(db_hitype2str(DB_LINE),ic_rbassoc->resval.rstring)) { cmd_ErrorPrompt(CMD_ERR_LINE,0); continue; }
                    if (ic_assoc(elist,10)!=0) goto exit;
                    ic_ptcpy(plink->ptt[2],ic_rbassoc->resval.rpoint);
                    if(ic_assoc(elist,11)!=0) goto exit;
                    ic_ptcpy(plink->ptt[3],ic_rbassoc->resval.rpoint);
                    if(bInBlock)
                        {
                        xformpt(plink->ptt[2],plink->ptt[2],mat);
                        xformpt(plink->ptt[3],plink->ptt[3],mat);
                        }

                    sds_trans(plink->ptt[2],&rbwcs,&rbucs,0,plink->ptt[2]);     // Transforming from WCS to UCS
                    sds_trans(plink->ptt[3],&rbwcs,&rbucs,0,plink->ptt[3]);     // Transforming from WCS to UCS

                    if (ic_linexline(plink->ptt[0],plink->ptt[1],plink->ptt[2],plink->ptt[3],vertexpt)<0) { cmd_ErrorPrompt(CMD_ERR_PARALLEL,0); continue; }
                }
            } else if(strsame("POLYLINE"/*DNT*/ ,ic_rbassoc->resval.rstring)) {
                if (loopflag) {
				    ic_assoc(elist,70);                                                                     // Get the polyline flag.
				    if (ic_rbassoc->resval.rint&(IC_PLINE_POLYFACEMESH+IC_PLINE_3DMESH)) { cmd_ErrorPrompt(CMD_ERR_ENTTYPE,0); continue; }	// Continue if the polyline is a 3D polygon mesh or polyface mesh.
				    plflag=ic_rbassoc->resval.rint;
                    if (plflag&IC_PLINE_3DPLINE){	// 3D polyline uses WCS.
                        rbtemp.restype=RTSHORT;
                        rbtemp.resval.rint=0;
                    } else {	// 2D polyline uses ECS.
                        rbtemp.restype=RT3DPOINT;
                        ic_assoc(elist,210);
                        ic_ptcpy(rbtemp.resval.rpoint,ic_rbassoc->resval.rpoint);
                    }
                    if (bInBlock)
                        {
                        sds_matrix inv;
                        bool x = TRUE;
                        ic_invertmatrix(mat, x, inv);
                        xformpt(pt1,pt1,inv);
                        }
                    ret=cmd_pl_vtx_no(ename,pt1,1,&exactvert);                  // Get the index of the vertex closest to the point picked.
                    for (fl1=0;fl1<ret;++fl1) {                                  // Step to the next entity of the vertex with the specified index.
                        if (sds_entnext_noxref(ename,ename2)!=RTNORM) return(-1);      // Get the next entity.
                        memcpy((void *)ename,(void *)ename2,sizeof(ename2));
                    }
                    // Get the elist for the first entity.
					IC_RELRB(elist);  
                    if ((elist=sds_entget(ename))==NULL) { cmd_ErrorPrompt(CMD_ERR_MOREMEM,0); goto exit; }
                    ic_assoc(elist,10);
                    ic_ptcpy(plink->ptt[0],ic_rbassoc->resval.rpoint);          // Copy the vertex of the first entity.
                    if(bInBlock)
                        xformpt(plink->ptt[0],plink->ptt[0],mat);

                    sds_trans(plink->ptt[0],&rbtemp,&rbucs,0,plink->ptt[0]);    // Transform the point from ECS to UCS.
                    cmd_pline_entnext(ename,ename2,plflag);                     // find the next vertex. This works on closed and open polylines.
                    // Get the elist for the seconed entity.
					IC_RELRB(elist);  
                    if ((elist=sds_entget(ename2))==NULL) { cmd_ErrorPrompt(CMD_ERR_MOREMEM,0); goto exit; }
                    ic_assoc(elist,10);
                    ic_ptcpy(plink->ptt[1],ic_rbassoc->resval.rpoint);          // Copy the vertex of the seconed entity.
                    if(bInBlock)
                        xformpt(plink->ptt[1],plink->ptt[1],mat);

                    sds_trans(plink->ptt[1],&rbtemp,&rbucs,0,plink->ptt[1]);    // Transform the point from ECS to UCS.
                    loopflag=FALSE; continue;
                } else {
                    // Set the type of the dimension to angular.
                    dimtype|=DIM_ANGULAR;
				sds_osnap(pt2,"NEA" ,pt2);
					IC_RELRB(elist);  //    Was IC_FREEIT
                    if ((elist=sds_entget(ename))==NULL) { cmd_ErrorPrompt(CMD_ERR_MOREMEM,0); goto exit; }
                    // This code is called if a polyline was picked, so we need to find the segment closest to the point picked and do the dimenstion on only that segment.
				    ic_assoc(elist,70);                                                                     // Get the polyline flag.
				    if (ic_rbassoc->resval.rint&(IC_PLINE_POLYFACEMESH+IC_PLINE_3DMESH)) { cmd_ErrorPrompt(CMD_ERR_ENTTYPE,0); continue; }  // Continue if the polyline is a 3D polygon mesh or polyface mesh.
				    plflag=ic_rbassoc->resval.rint;
				    // EBATECH(CNBR) -[ 13-03-2003 When pt1 is 2D-Polyline and pt2 is 3D-Polyline, each rbtemp are different.
                    if (plflag&IC_PLINE_3DPLINE){	// 3D polyline uses WCS.
                        rbtemp.restype=RTSHORT;
                        rbtemp.resval.rint=0;
                    } else {	// 2D polyline uses ECS.
                        rbtemp.restype=RT3DPOINT;
                        ic_assoc(elist,210);
                        ic_ptcpy(rbtemp.resval.rpoint,ic_rbassoc->resval.rpoint);
                    }
                    // EBATECH(CNBR) ]-
                    if (bInBlock)
                        {
                        sds_matrix inv;
                        bool x = TRUE;
                        ic_invertmatrix(mat, x, inv);
                        xformpt(pt2,pt2,inv);
                        }
                    ret=cmd_pl_vtx_no(ename,pt2,1,&exactvert);                             // Get the index of the vertex closest to the point picked.
                    for (fl1=0;fl1<ret;++fl1) {                                  // Step to the next entity of the vertex with the specified index.
                        if (sds_entnext_noxref(ename,ename2)!=RTNORM) return(-1);      // Get the next entity.
                        memcpy((void *)ename,(void *)ename2,sizeof(ename2));
                    }
                    // Get the elist for the first entity.
					IC_RELRB(elist); 
                    if ((elist=sds_entget(ename))==NULL) { cmd_ErrorPrompt(CMD_ERR_MOREMEM,0); goto exit; }
                    ic_assoc(elist,10); ic_ptcpy(plink->ptt[2],ic_rbassoc->resval.rpoint);      // Copy the vertex of the first entity.
                    if(bInBlock)
                        xformpt(plink->ptt[2],plink->ptt[2],mat);

                    sds_trans(plink->ptt[2],&rbtemp,&rbucs,0,plink->ptt[2]);                    // Transform the point from ECS to UCS.
                    cmd_pline_entnext(ename,ename2,plflag);                                     // find the next vertex. This works on closed and open polylines.
                    // Get the elist for the seconed entity.
					IC_RELRB(elist); 
                    if ((elist=sds_entget(ename2))==NULL) { cmd_ErrorPrompt(CMD_ERR_MOREMEM,0); goto exit; }
                    ic_assoc(elist,10); ic_ptcpy(plink->ptt[3],ic_rbassoc->resval.rpoint);      // Copy the vertex of the seconed entity.
                    if(bInBlock)
                        xformpt(plink->ptt[3],plink->ptt[3],mat);

                    sds_trans(plink->ptt[3],&rbtemp,&rbucs,0,plink->ptt[3]);                    // Transform the point from ECS to UCS.
                    // Convert points to UCS before doing the ic_linexline.
                    if (ic_linexline(plink->ptt[0],plink->ptt[1],plink->ptt[2],plink->ptt[3],vertexpt)<0) { cmd_ErrorPrompt(CMD_ERR_PARALLEL,0); continue; }
                }
            } else if (strsame("LWPOLYLINE"/*DNT*/,ic_rbassoc->resval.rstring)){
                sds_resbuf rbtemp;
			    db_lwpolyline *lwpl=(db_lwpolyline *)ename[0];
                if (loopflag) 
                    {
                    if (bInBlock)
                        {
                        sds_matrix inv;
                        bool x = TRUE;
                        ic_invertmatrix(mat, x , inv);
                        xformpt(pt1,pt1,inv);

//                    ic_invertmatrix(sds_matrix mtx,    // I: the sds_matrix to be inverted
//					 bool & invertible, // O: true if and only if mtx is invertible
//					 sds_matrix inv)
                        }
                    sds_osnap(pt1,"NEA" ,pt1);
                    // This code is called if a polyline was picked, so we need to find the segment closest to the point picked and do the dimenstion on only that segment.
				    ic_assoc(elist,70);                                                                      // Get the polyline flag.
				    plflag=ic_rbassoc->resval.rint;
                    rbtemp.restype=RT3DPOINT;
                    ic_assoc(elist,210);
                    ic_ptcpy(rbtemp.resval.rpoint,ic_rbassoc->resval.rpoint);
                    fl1=cmd_lwpl_vtx_no(ename,pt1,1,&exactvert);                                                          // Get the index of the vertex closest to the point picked.
                    // Get the elist for the first entity.
				    lwpl->get_10(plink->ptt[0],fl1++);
                    if (fl1 >= lwpl->ret_90()) fl1 = 0;
				    lwpl->get_10(plink->ptt[1],fl1);
                    if(bInBlock)   
                        {
                        xformpt(plink->ptt[0],plink->ptt[0],mat);
                        xformpt(plink->ptt[1],plink->ptt[1],mat);
                        }
                    sds_trans(plink->ptt[0],&rbtemp,&rbucs,0,plink->ptt[0]); 
                    sds_trans(plink->ptt[1],&rbtemp,&rbucs,0,plink->ptt[1]); 
                    loopflag=FALSE; continue;
                    }
                else
                    {
                    // Set the type of the dimension to angular.
                    dimtype|=DIM_ANGULAR;
                    if (bInBlock)
                        {
                        sds_matrix inv;
                        bool x = TRUE;
                        ic_invertmatrix(mat, x, inv);
                        xformpt(pt2,pt2,inv);
                        }
				    sds_osnap(pt2,"NEA" ,pt2);
                    // This code is called if a polyline was picked, so we need to find the segment closest to the point picked and do the dimenstion on only that segment.
				    ic_assoc(elist,70);	// Get the polyline flag.
				    plflag=ic_rbassoc->resval.rint;
                    rbtemp.restype=RT3DPOINT;
                    ic_assoc(elist,210);
                    ic_ptcpy(rbtemp.resval.rpoint,ic_rbassoc->resval.rpoint);
                    fl1=cmd_lwpl_vtx_no(ename,pt2,1,&exactvert);	// Get the index of the vertex closest to the point picked.
                    // Get the elist for the first entity.
				    lwpl->get_10(plink->ptt[2],fl1++);
                    if (fl1 >= lwpl->ret_90()) fl1 = 0;
				    lwpl->get_10(plink->ptt[3],fl1);
                    if(bInBlock)   
                        {
                        xformpt(plink->ptt[2],plink->ptt[2],mat);
                        xformpt(plink->ptt[3],plink->ptt[3],mat);
                        }
                    sds_trans(plink->ptt[2],&rbtemp,&rbucs,0,plink->ptt[2]); 
                    sds_trans(plink->ptt[3],&rbtemp,&rbucs,0,plink->ptt[3]); 

                    if (ic_linexline(plink->ptt[0],plink->ptt[1],plink->ptt[2],plink->ptt[3],vertexpt)<0) { cmd_ErrorPrompt(CMD_ERR_PARALLEL,0); continue; }
                    }
            } else if(strsame("ARC"/*DNT*/,ic_rbassoc->resval.rstring)) {
                sds_resbuf rbtemp;
                rbtemp.restype=RT3DPOINT;
                if (ic_assoc(elist,210)!=0) goto exit;
                ic_ptcpy(rbtemp.resval.rpoint,ic_rbassoc->resval.rpoint);
                if (ic_assoc(elist,10)!=0) goto exit;
                ic_ptcpy(vertexpt,ic_rbassoc->resval.rpoint);
                if (ic_assoc(elist,40)!=0) goto exit;
                rad=ic_rbassoc->resval.rreal;
                if (ic_assoc(elist,50)!=0) goto exit;
                stang=ic_rbassoc->resval.rreal;
                if (ic_assoc(elist,51)!=0) goto exit;
                endang=ic_rbassoc->resval.rreal;
                //*** Calculate the endpoints of the arc
                pt1[0]=(vertexpt[0]+(rad*(cos(stang))));
                pt1[1]=(vertexpt[1]+(rad*(sin(stang))));
                pt1[2]=vertexpt[2];
                pt2[0]=(vertexpt[0]+(rad*(cos(endang))));
                pt2[1]=(vertexpt[1]+(rad*(sin(endang))));
                pt2[2]=vertexpt[2];
				ic_normang(&stang,&endang);
                // Set the type of the dimension to angular 3 point.
                dimtype|=DIM_ANGULAR_3P;
                if(bInBlock)
                    {
                    xformpt(vertexpt,vertexpt,mat);
                    xformpt(pt1,pt1,mat);
                    xformpt(pt2,pt2,mat);
                    }

                sds_trans(vertexpt,&rbtemp,&rbucs,0,vertexpt);                      // Transform the point from ECS to UCS.
                sds_trans(pt1,&rbtemp,&rbucs,0,pt1);                                // Transform the point from ECS to UCS.
                sds_trans(pt2,&rbtemp,&rbucs,0,pt2);                                // Transform the point from ECS to UCS.
            } else if(strsame("CIRCLE"/*DNT*/,ic_rbassoc->resval.rstring)) {
                sds_resbuf rbtemp;
                rbtemp.restype=RT3DPOINT;
                if (ic_assoc(elist,210)!=0) goto exit;
                ic_ptcpy(rbtemp.resval.rpoint,ic_rbassoc->resval.rpoint);
                if (ic_assoc(elist,10)!=0) goto exit;
                ic_ptcpy(vertexpt,ic_rbassoc->resval.rpoint);
                if ((sds_getpoint(vertexpt,ResourceString(IDC_DIMENSIONS__NOTHER_SID_111, "\nOther side of angle: " ),pt2))!=RTNORM) goto exit;
				stang=sds_angle(vertexpt,pt1);
				endang=sds_angle(vertexpt,pt2);
				ic_normang(&stang,&endang);
                // Set the type of the dimension to angular 3 point.
                dimtype|=DIM_ANGULAR_3P;
                if(bInBlock)
                    {
                    xformpt(vertexpt,vertexpt,mat);
                    xformpt(pt1,pt1,mat);
                    xformpt(pt2,pt2,mat);
                    }
                sds_trans(pt1,&rbtemp,&rbucs,0,pt1);                                // Transform the point from ECS to UCS.
                sds_trans(pt2,&rbtemp,&rbucs,0,pt2);                                // Transform the point from ECS to UCS.
                sds_trans(vertexpt,&rbtemp,&rbucs,0,vertexpt);                      // Transform the point from ECS to UCS.
            } else {
				cmd_ErrorPrompt(CMD_ERR_SELECTION,0); continue;
            }
            IC_RELRB(elist);  //  Was IC_FREEIT;
			break;
        }
    }

    elp=new db_dimension(flp);
    elp->set_11(dimzeropt);
    elp->set_70(dimtype);
    if (dimtype&DIM_ANGULAR) {
        elp->set_13(plink->ptt[0]);
        elp->set_14(plink->ptt[1]);
        elp->set_15(plink->ptt[2]);
        elp->set_10(plink->ptt[3]);
    } else {
        elp->set_13(pt1);
        elp->set_14(pt2);
        elp->set_15(vertexpt);
    }
    db_ucshelper(view->ucsorg,view->ucsaxis,view->elevation,pt1,pt2,&fr1);
    elp->set_210(plink->pt210);
    elp->set_51(fr1);
    elp->set_12(pt2);

    for (plink->DefValidflg=FALSE;;) {
        elp->set_1(dimtxt);
        elp->set_53(txtang);

		ename[0]=(long)elp;
		ename[1]=(long)flp;
		SDS_getvar(NULL,DB_QDRAGMODE,&setgetrb,flp,&SDS_CURCFG,&SDS_CURSES);

        if (sds_initget(SDS_RSG_NOVIEWCHG,ResourceString(IDC_DIMENSIONS_INITGET_ANGL_49, "Angle Text ~_Angle ~_Text" ))!=RTNORM) goto exit;

        if (dimtype&DIM_ANGULAR) {
            if (plink->DefValidflg) {
				//BugZilla No. 78102; 10-06-2002 [
				sds_point  prvTextMidPoint = {0.0, 0.0, 0.0};
				ic_ptcpy(prvTextMidPoint,plink->textmidpt);
                ret=SDS_dragobject(-1,setgetrb.resval.rint,(sds_real *)ename,(double *)elp->retp_11(),0.0,ResourceString(IDC_DIMENSIONS__NANGLE_TEX_112, "\nAngle/Text/<Location of dimension text>: " ),NULL,pt3,NULL);
				if(ret == RTNONE)
					ic_ptcpy(plink->textmidpt,prvTextMidPoint);
				else 
                ic_ptcpy(plink->textmidpt,pt3);
                elp->set_11(plink->textmidpt);
				//BugZilla No. 78102; 10-06-2002 ]
            } else {
                ret=SDS_dragobject(-1,setgetrb.resval.rint,(sds_real *)ename,(double *)elp->retp_16(),0.0,ResourceString(IDC_DIMENSIONS__NANGLE_TEXT_67, "\nAngle/Text/<Location of dimension arc>: " ),NULL,pt3,NULL);
                ic_ptcpy(plink->pt16,pt3);
                elp->set_16(plink->pt16);
            }
        } else {
            if (plink->DefValidflg) {
				//BugZilla No. 78102; 10-06-2002 [
				sds_point  prvTextMidPoint = {0.0, 0.0, 0.0};
				ic_ptcpy(prvTextMidPoint,plink->textmidpt);
                ret=SDS_dragobject(-1,setgetrb.resval.rint,(sds_real *)ename,(double *)elp->retp_11(),0.0,ResourceString(IDC_DIMENSIONS__NANGLE_TEX_112, "\nAngle/Text/<Location of dimension text>: " ),NULL,pt3,NULL);
				if(ret == RTNONE)
					ic_ptcpy(plink->textmidpt,prvTextMidPoint);
				else 
                ic_ptcpy(plink->textmidpt,pt3);
                elp->set_11(plink->textmidpt);
				//BugZilla No. 78102; 10-06-2002 ]                                
            } else {
                ret=SDS_dragobject(-1,setgetrb.resval.rint,(sds_real *)ename,(double *)elp->retp_10(),0.0,ResourceString(IDC_DIMENSIONS__NANGLE_TEXT_67, "\nAngle/Text/<Location of dimension arc>: " ),NULL,pt3,NULL);
                ic_ptcpy(plink->defpt,pt3);
                elp->set_10(plink->defpt);
            }
        }
        cmd_makedimension(elp,flp,-1);

        switch(ret) {
			// Bugzilla No: 78175; 27-05-2002 
			case RTNONE: 
				if(plink->DefValidflg) 
					break;
            case RTERROR:
            case RTCAN:   goto exit;
//            case RTNONE:  if (txtflag) { ic_ptcpy(tmppt[0],dimzeropt); break; } else goto exit;
            case RTNORM:
                // call this when the last point is picked.
                if (!plink->DefValidflg) {
//                    if (sds_getinput(fs1)!=RTNORM) goto exit;
//                    if (strsame("Text",fs1)) {
					// Bugzilla No : 78054 ; 08-04-2002 [
					if(!(*dimtxt))
					{
                        // If there has not been a text value entered then use the value in DIMVALUE.
                        if (plink->text && *plink->text) 
							strcpy(dimtxt,plink->text); 
						else 
							cmd_genstring(plink,dimtxt,plink->dimvalue,""/*DNT*/,FALSE,flp);

                        sptr1=strchr(dimtxt,';'/*DNT*/);
                        strcpy(dimtxt,sptr1 != NULL ? sptr1+1 : dimtxt);
                        sprintf(fs2,ResourceString(IDC_DIMENSIONS__NDIMENSION__52, "\nDimension text <%s>: " ),dimtxt);
                        if (mode) break;
                        if ((ret=sds_getstring(1,fs2,fs1))==RTERROR) 
							goto exit; 
						else 
							if (ret==RTCAN) goto exit;
                        //Bugzilla No. 78105; 01-07-2002 [
						//if (*fs1) {
							if ((sptr1 = _tcsstr(fs1, "%%d"/*DNT*/)) != NULL) {
								*sptr1 = 0;
								sprintf(plink->text, "%s%c"/*DNT*/, fs1, (char)176);
							} else
	                            strcpy(plink->text, fs1);

                            strcpy(dimtxt,plink->text);
                        //}
						//Bugzilla No. 78105; 01-07-2002 ]
					}
					plink->DefValidflg=TRUE;
                    continue;
					// Bugzilla No : 78054 ; 08-04-2002 ]
//                    }
                } else break;
            case RTKWORD:
                if (sds_getinput(fs1)!=RTNORM) 
                    goto exit;
                strupr(fs1);
                if (strisame("TEXT"/*DNT*/ ,fs1)) {
                    // ~| TODO AutoCAD calls the MTEXT command here.  Eventually we need to do the same.
                    if (plink->text && *plink->text) 
                        strcpy(dimtxt,plink->text); 
                    else 
                        cmd_genstring(plink,dimtxt,plink->dimvalue,""/*DNT*/,FALSE,flp);

                    sptr1=strchr(dimtxt,';'/*DNT*/);
                    strcpy(dimtxt,sptr1 != NULL ? sptr1+1 : dimtxt);
                    sprintf(fs2,ResourceString(IDC_DIMENSIONS__NDIMENSION__52, "\nDimension text <%s>: " ),dimtxt);
                    if ((ret=sds_getstring(1,fs2,fs1))==RTERROR) 
                        goto exit; 
                    else if (ret==RTCAN) 
                        goto exit;
                        if (*fs1) {
							if ((sptr1 = _tcsstr(fs1, "%%d"/*DNT*/)) != NULL) {
								*sptr1 = 0;
								sprintf(plink->text, "%s%c"/*DNT*/, fs1, (char)176);
							} else
	                            strcpy(plink->text, fs1);

                            strcpy(dimtxt,plink->text);
                        }
                    if (!plink->text && !(*plink->text)) 
                        *dimtxt=0;
                    continue;

                } else if (strsame("ANGLE"/*DNT*/ ,fs1)) {
                    if ((ret=sds_getangle(NULL,ResourceString(IDC_DIMENSIONS__NANGLE_OF__115, "\nAngle of dimension text: " ),&txtang))==RTERROR)
                        goto exit; 
                    else if (ret==RTCAN) 
                        goto exit;
                    continue;
                } else {
                    cmd_ErrorPrompt(CMD_ERR_PT,0);
                    continue;
                }
                break;
        }
        break;
    }
    if (plinkvar[DIMASO].dval.ival) { delete elp; elp=NULL; }

    // Do the transformaion from (UCS) to (NATIVE) format this is becouse when a dimension is created via entmake the points need to be in
    // native format for compatability when someone does an entmake from the command line.
	cmd_savelast(plink,view);
    // Do the transformaion from (UCS) to (NATIVE) format this is becouse when a dimension is created via entmake the points need to be in
    // native format for compatability when someone does an entmake from the command line.
    sds_trans(plink->textmidpt,&rbucs,&rbecs,0,plink->textmidpt);
    sds_trans(plink->defpt,&rbucs,&rbwcs,0,plink->defpt);
    sds_trans(plink->pt13,&rbucs,&rbwcs,0,plink->pt13);
    sds_trans(plink->pt14,&rbucs,&rbwcs,0,plink->pt14);
    sds_trans(plink->pt15,&rbucs,&rbwcs,0,plink->pt15);
    sds_trans(plink->pt16,&rbucs,&rbecs,0,plink->pt16);
    // Create dimension as a block.
    if (plinkvar[DIMASO].dval.ival) {
        db_ucshelper(view->ucsorg,view->ucsaxis,view->elevation,pt1,pt2,&fr1);
        if (dimtype&DIM_ANGULAR) {
            ic_ptcpy(lastdimpts.startpt,plink->defpt);
            ic_ptcpy(lastdimpts.startptother,plink->pt15);
            ic_ptcpy(lastdimpts.vertex,vertexpt);
            if ((elist=sds_buildlist(RTDXF0,"DIMENSION"/*DNT*/,
                                     11,plink->textmidpt,
                                     12,pt2,
                                     70,plink->flags,
                                     1 ,plink->text,
                                     13,plink->pt13,
                                     14,plink->pt14,
                                     15,plink->pt15,
                                     10,plink->defpt,
                                     16,plink->pt16,
                                     51,fr1,
                                     53,plink->textrot,
                                     210,view->ucsaxis[2],
                                     0))==NULL) { cmd_ErrorPrompt(CMD_ERR_MOREMEM,0); goto exit; }
         } else {
            ic_ptcpy(lastdimpts.startpt,plink->pt14);
            ic_ptcpy(lastdimpts.vertex,plink->pt15);
            if ((elist=sds_buildlist(RTDXF0,"DIMENSION"/*DNT*/,
                                     10,plink->defpt,
                                     11,plink->textmidpt,
                                     12,pt2,
                                     70,plink->flags,
                                     1 ,plink->text,
                                     13,plink->pt13,
                                     14,plink->pt14,
                                     15,plink->pt15,
                                     51,fr1,
                                     53,plink->textrot,
                                     210,view->ucsaxis[2],
                                     0))==NULL) { cmd_ErrorPrompt(CMD_ERR_MOREMEM,0); goto exit; }
        }
        cmd_buildEEdList(elist);
        if ((ret=sds_entmake(elist))!=RTNORM)         goto exit;
        if ((ret=sds_entlast(lastdimpts.ename))!=RTNORM) goto exit;
    } else cmd_makedimension(elp,flp,0);

    IC_RELRB(elist);
	//BugZilla No. 78149; 13-05-2002
	//cmd_CleanUpDimVarStrings();			// Clean up first, then return ret.
	plink->HomeText = FALSE;
    return(RTNORM);
    exit:
		// Bugzilla No: 78175; 27-05-2002 
		if(elp) 
			delete elp; 
        IC_RELRB(elist);
		//BugZilla No. 78149; 13-05-2002
		//cmd_CleanUpDimVarStrings();		// Clean up first, then return ret.
		plink->HomeText = FALSE;
        return(RTERROR);
}
