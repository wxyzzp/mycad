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

extern sds_point    dimzeropt; // Needed to put this variable here because it gets initialized.
extern char         cmd_string[256];
extern struct cmd_dimvars plinkvar[];
//BugZilla No. 78155; 27-05-2002
//extern struct cmd_dimlastpt lastdimpts;
extern struct cmd_dimeparlink* plink;

//BugZilla No. 78155; 27-05-2002 [
bool isValidDim(sds_name& ename)
{

	if(sds_name_nil(lastdimpts.ename))
		return false;

	if(((db_dimension*)ename[0])->ret_deleted())
		return false;

	return true;
}
//BugZilla No. 78155; 27-05-2002 ]

// ============================================================================================================================================================
// ========================================================= LINEAR FUNCTIONS =================================================================================
// ============================================================================================================================================================
//
// This area has only the functions that create the linear dimensions,(DIMLINEAR,DIMALIGNED,DIM - HOR,DIM - VER,DIM - ALIGNED,
// DIM - ROTATED).
//
// ============================================================================================================================================================

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// mode: 0 - called from cmd_dimContinue from comand prompt "dimcontinue"
//       1 - called from cmd_dimbaseline
//       2 - called from DIM - continue
//       3 - called from DIM - baseline
//
short cmd_DimContinueFunc(short mode,db_drawing *flp) {
    sds_name        ename = {0L, 0L};
    struct resbuf  *rbb=NULL,
                   *elist=NULL,
                   *rb,
                    setgetrb,
                    rbwcs,
                    rbucs,
                    rbecs;
    sds_point       pt1 = {0.0, 0.0, 0.0},
                    pt2 = {0.0, 0.0, 0.0},
                    pt3 = {0.0, 0.0, 0.0},
                    ecspt = {0.0, 0.0, 0.0},
                    wcspt = {0.0, 0.0, 0.0},
                    ptu[5] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}},
                    ptt[4] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}},
                    ptd[4] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};
    long            ret=0L;
    sds_real        angle = 0.0,
                    angle1 = 0.0,
                    angle2 = 0.0,
                    angle3 = 0.0,
                    eangle = 0.0,
                    sinofang = 0.0,
                    cosofang = 0.0,
                    sinofelang = 0.0,
                    cosofelang = 0.0,
                    distance = 0.0;
    BOOL            sideflg = FALSE,
                    Selfirst = FALSE;
    short           type = 0,
                    dimseval = 0;
    sds_real        fr1 = 0.0;
    int             fi1 = 0;
    struct gr_view *view=SDS_CURGRVW;

	plink->HomeText = TRUE;	// EBATECH(CNBR) Bugzilla#78455 13-03-2003 Support DIMTMOVE

    do {
        type=lastdimpts.i70; type^=DIM_BIT_6;
        do {
			//BugZilla No. 78155; 27-05-2002
            //if (!sds_name_nil(lastdimpts.ename)) {
			if (isValidDim(lastdimpts.ename)) {
				//BugZilla No. 78151; 13-05-2002
			    //if (mode==1) {
				if (mode==1 || mode==3){
                    ret=sds_getpoint(((type==DIM_ANGULAR || type==DIM_ANGULAR_3P)?lastdimpts.vertex:NULL),ResourceString(IDC_DIMENSIONS__NBASELINE___59, "\nBaseline:  ENTER to select starting dimension/<Origin of next extension line>: " ),pt1);
                } else {
                    ret=sds_getpoint(((type==DIM_ANGULAR || type==DIM_ANGULAR_3P)?lastdimpts.vertex:NULL),ResourceString(IDC_DIMENSIONS__NCONTINUE___60, "\nContinue:  ENTER to select starting dimension/<Origin of next extension line>: " ),pt1);
			    }
			    if (ret==RTERROR || ret==RTCAN) goto exit;
            } else ret=RTNONE;
            if (ret==RTNONE) {
                Selfirst=TRUE;
                do {
                    if (mode==1 || mode==3) {
                        ret=sds_entsel(ResourceString(IDC_DIMENSIONS__NSELECT_DIM_61, "\nSelect dimension for baseline: " ),ename,lastdimpts.startpt);
                    } else {
                        ret=sds_entsel(ResourceString(IDC_DIMENSIONS__NSELECT_DIM_62, "\nSelect dimension to continue: " ),ename,lastdimpts.startpt);
                    }
				    if (ret==RTERROR) {
                       if (SDS_getvar(NULL,DB_QERRNO,&setgetrb,flp,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) goto exit;
                       if (setgetrb.resval.rint==OL_ENTSELPICK) { cmd_ErrorPrompt(CMD_ERR_NOENT,0); continue; } else goto exit;
                    } else if (ret==RTNORM) {
                        IC_RELRB(elist);		// Changed to IC_RELRB().
                        if ((elist=sds_entget(ename))==NULL) goto exit;
                        if (ic_assoc(elist,70)!=0) { cmd_ErrorPrompt(CMD_ERR_LINORDANG,0); continue; }
                        type=ic_rbassoc->resval.rint; type^=DIM_BIT_6;
                        switch(type) {
                            case DIM_DIAMETER:
                            case DIM_RADIUS:
                                cmd_ErrorPrompt(CMD_ERR_LINORDANG,0);
                                continue;
                            default:
                                if (ic_assoc(elist,210)!=0) goto exit;
                                ic_ptcpy(plink->pt210,ic_rbassoc->resval.rpoint);
                                cmd_initialrbcs(&rbecs,&rbwcs,&rbucs,flp,plink->pt210);
                                for (rb=elist;rb;rb=rb->rbnext) {
                                    switch(rb->restype) {
                                        case 10:  sds_trans(rb->resval.rpoint,&rbwcs,&rbucs,0,lastdimpts.pt10); break;
                                        case 13:  sds_trans(rb->resval.rpoint,&rbwcs,&rbucs,0,lastdimpts.pt13); break;
                                        case 14:  sds_trans(rb->resval.rpoint,&rbwcs,&rbucs,0,lastdimpts.pt14); break;
                                        case 15:  sds_trans(rb->resval.rpoint,&rbwcs,&rbucs,0,lastdimpts.pt15); break;
                                        case 16:  sds_trans(rb->resval.rpoint,&rbecs,&rbucs,0,lastdimpts.pt16); break;
                                        case 50:  lastdimpts.r50=rb->resval.rreal; break;
                                        case 52:  lastdimpts.r52=rb->resval.rreal; break;
                                        case 70:  lastdimpts.i70=rb->resval.rint; break;
                                        case 210: ic_ptcpy(lastdimpts.pt210,plink->pt210); break;
                                    }
                                }

                                // EBATECH(watanabe)-[FIX : not horizontal
                                //lastdimpts.startpt[1]=lastdimpts.pt10[1];
                                // ]-EBATECH(watanabe)

								ic_encpy(lastdimpts.ename, ename);	/*D.G.*/// Prevent infinite loop.
								IC_RELRB(elist);
                                break;
                        }
                        break;
                    } else goto exit;
                } while(TRUE);
            } else if (ret==RTNORM) break;
        } while(TRUE);

        ic_ptcpy(ptu[0],lastdimpts.pt10);
        ic_ptcpy(ptu[1],lastdimpts.pt13);
        ic_ptcpy(ptu[2],lastdimpts.pt14);
        ic_ptcpy(ptu[3],lastdimpts.pt15);
        ic_ptcpy(ptu[4],lastdimpts.pt16);
        angle=lastdimpts.r50;
        type=lastdimpts.i70;
        eangle=lastdimpts.r52;

		//BugZilla No. 64188; 13-05-2002
		type &= ~DIM_BIT_6;
		type &= ~DIM_TEXT_MOVED;
		//BugZilla No. 78153; 03-06-2002
		//type &= ~DIM_XTYPEORD_DEF;

		if(type == DIM_ALIGNED)		/*D.G.*/// Make Rot/Hor/Vert dimension continuing from this aligned one.
		{
			type = DIM_ROT_HOR_VER;
			angle = sds_angle(lastdimpts.pt13, lastdimpts.pt14);
		}

        //BugZilla No. 64188; 13-05-2002
		//type^=DIM_BIT_6;
        switch(type) {
            case DIM_ANGULAR:
                // Get the vertex of the angular dimension.
                ic_linexline(ptu[1],ptu[2],ptu[3],ptu[0],pt2);
                if (mode==1 || mode==3)  {
                    // lastdimpts.startpt will be the starting point of the continue or baseline dimension.
                    // so we use that to find out which side of the dimension to start from.
					
					//BugZilla No. 78151; 13-05-2002 [
                    //angle1=cmd_CalculateAngle(ptu[1],ptu[2],lastdimpts.startpt);
                    //angle2=cmd_CalculateAngle(ptu[0],ptu[3],lastdimpts.startpt);
                    //if (angle2<angle3) ic_ptcpy(pt3,ptu[3]); else ic_ptcpy(pt3,ptu[2]);
					ic_ptcpy(pt3,ptu[1]);
					//BugZilla No. 78151; 13-05-2002 ]

                } else {
                    // Get the closeset angle of the two angles made by the dimension to the angle formed by the angle made by the center and
                    // the pick point or the dimension. This will give the angle made between the dimension pick and the angle made by pt13 and pt15.
                    angle2=cmd_CalculateAngle(pt2,ptu[2],pt1);
                    // This will give the angle made between the dimension pick and the angle made by pt14 and defpt.
                    angle3=cmd_CalculateAngle(pt2,ptu[0],pt1);
                    // If angle2 is less then copy pt13 to pt3, otherwise copy pt14 to pt3.
                    if (angle2<angle3) ic_ptcpy(pt3,ptu[2]); else ic_ptcpy(pt3,ptu[0]);
                }

                // Get the angle of the vector made by the center ponit and the selected dimension side pt13 or pt14.
                angle2=sds_angle(pt2,pt3);
                // Get the angle of the vector from the center to the pick point.
                angle=sds_angle(pt2,pt1);
                // Make sure that angle2 is greater than angle.
                ic_normang(&angle,&angle2);
                // Get the angle made by angle2-180 degrees.
                angle3=angle2-IC_PI;
                // Get the distance from the center point and pt10.
                fr1=sds_distance(pt2,ptu[4]);
                // Increase the distance out for baseline type dimensions.
                if (mode==1 || mode==3) fr1+=(plinkvar[DIMDLI].dval.rval*plinkvar[DIMSCALE].dval.rval);
                if (angle>angle3) {
                    angle1=angle2-angle;
                    angle1=angle2-(angle1/2);
                    sds_polar(pt2,angle1,fr1,ptu[0]);
                    plink->GenInitflg=TRUE;
                } else {
                    angle1=DIM_2_PI-(angle2-angle);
                    angle1=angle2+(angle1/2);
                    sds_polar(pt2,angle1,fr1,ptu[0]);
                    plink->GenInitflg=TRUE;
                }
                if (mode==1 || mode==3) ic_ptcpy(lastdimpts.startpt,ptu[2]); else ic_ptcpy(lastdimpts.startpt,pt1);
                break;
            case DIM_ANGULAR_3P:
                ic_ptcpy(pt2,lastdimpts.pt15);
                // lastdimpts.startpt will be the starting point of the continue or baseline dimension.
                // so we use that to find out which side of the dimension to start from.
				
				//BugZilla No. 78151; 13-05-2002 [
                //angle1=cmd_CalculateAngle(ptu[3],ptu[1],lastdimpts.startpt);
                //angle2=cmd_CalculateAngle(ptu[3],ptu[2],lastdimpts.startpt);
                //if (angle1<angle2) ic_ptcpy(pt3,ptu[1]); else ic_ptcpy(pt3,ptu[2]);

				if(mode==1 || mode==3) 
				{
					ic_ptcpy(pt3,ptu[1]);
				}
				else
				{
					angle1=cmd_CalculateAngle(ptu[3],ptu[1],pt1);
					angle2=cmd_CalculateAngle(ptu[3],ptu[2],pt1);
                if (angle1<angle2) ic_ptcpy(pt3,ptu[1]); else ic_ptcpy(pt3,ptu[2]);
				}
				//BugZilla No. 78151; 13-05-2002 ]
                
                // Get the angle of the vector made by the center ponit and the selected dimension side pt13 or pt14.
                angle2=sds_angle(ptu[3],pt3);
                // Get the angle of the vector from the center to the pick point.
                angle=sds_angle(ptu[3],pt1);
                // Make sure that angle2 is greater than angle.
                ic_normang(&angle,&angle2);
                // Get the angle made by angle2-180 degrees.
                angle3=angle2-IC_PI;
                // Get the distance from the center point and pt10.
                fr1=sds_distance(ptu[3],ptu[0]);
                // Increase the distance out for baseline type dimensions.
                if (mode==1 || mode==3) fr1+=(plinkvar[DIMDLI].dval.rval*plinkvar[DIMSCALE].dval.rval);
                if (angle>angle3) {
                    angle1=angle2-angle;
                    angle1=angle2-(angle1/2);
                    sds_polar(ptu[3],angle1,fr1,ptu[0]);
                    plink->GenInitflg=TRUE;
                } else {
                    angle1=DIM_2_PI-(angle2-angle);
                    angle1=angle2+(angle1/2);
                    sds_polar(ptu[3],angle1,fr1,ptu[0]);
                    plink->GenInitflg=TRUE;
                }
                if (mode==1 || mode==3) ic_ptcpy(lastdimpts.startpt,ptu[1]); else ic_ptcpy(lastdimpts.startpt,pt1);
                break;
			//BugZilla No. 78153; 03-06-2002
            case DIM_x_ORDINATE:
			case DIM_Y_ORDINATE:
                //if (type==70) {
				{
					int type = lastdimpts.i70 & DIM_XTYPEORD_DEF ? 0:1;
					sds_real dimdli = plinkvar[DIMDLI].dval.rval*plinkvar[DIMSCALE].dval.rval;

					ptu[2][type]=pt1[type];

					if(pt1[type]>=lastdimpts.pt13[type] && pt1[type] <= (lastdimpts.pt14[type] + dimdli))
					{
						ptu[2][type] = lastdimpts.pt14[type]+dimdli;
                }

					if(pt1[type]<=lastdimpts.pt13[type] && pt1[type] >= (lastdimpts.pt14[type] - dimdli))
					{
						ptu[2][type] = lastdimpts.pt14[type]-dimdli;
					}

					distance=fabs(pt1[type]);
					plink->DefValidflg = TRUE;

				}                
//                sideflg=(fabs(ptu[1][1]-ptu[2][1])>fabs(ptu[1][0]-ptu[2][0]))?1:0;
//                ptu[2][(sideflg)?0:1]=pt1[(sideflg)?0:1];
                break;
            default:
                // This is the possable types of linear dims that are possable at this point.
                //                    .                 _____________Center of dimension line.
                //     |             /        | pt13   | defpt
                //     |            /         |  |     |  |
                //     |     /_____/          |  |  /__|__/
                //     |    /     /           |  | /     /
                //     |   /                  |  |/     /
                //     |  .                   |  .     /
                // ----.----------        ----.-------/--  ----(x) axis.
                //     |                      |      /
                //     |                      |     ._____ pt14
                //     |                      |
                //     |                      |
                //     |                      |
                //
                // angle is the (50) group of the dimension.
                sinofang=sin(angle); cosofang=cos(angle);
                // So rotate the dimesion so that it is easy to work on.
                // ptu[1]= pt13. ptu[2]=pt14. pt1=the first pick point, ptu[0]=defpt
                cmd_ucstodim(ptu[1],ptu[2],ptd[1],sinofang,cosofang);
                cmd_ucstodim(ptu[1],pt1   ,ptd[2],sinofang,cosofang);
                cmd_ucstodim(ptu[1],ptu[0],ptd[3],sinofang,cosofang);
                cmd_ucstodim(ptu[1],lastdimpts.startpt,ptd[0],sinofang,cosofang);

                // EBATECH(watanabe)-[FIX : not horizontal
                ptd[0][1] = ptd[3][1];
                // ]-EBATECH(watanabe)

                // Since pt2 is the last point picked. If a DIMBASELINE is being done then make sure it is from the 13 point of the dimension.
                // If a DIMCONTINUE is being done then make sure it is from the 14 point of the dimension.
                if (!Selfirst) {
                    // 0 = The 13 point will be used. 1 = The 14 point will be used.
                    if (mode==2 || mode==0) sideflg=1; else sideflg=0;
                } else {
                    // eangle is the (52) group of the dimension.
                    sinofelang=sin(eangle); cosofelang=cos(eangle);
                    // Rotate (DEFPT,PT14,pt2) eangle.
                    cmd_ucstodim(dimzeropt,ptd[1],ptt[0],sinofelang,cosofelang);
                    cmd_ucstodim(dimzeropt,ptd[3],ptt[1],sinofelang,cosofelang);
                    cmd_ucstodim(dimzeropt,ptd[0],ptt[2],sinofelang,cosofelang);
                    // If ptt[2][0] is less then the middle of the dimension then sideflg=FALSE.
					//BugZilla No. 77770; 08-07-2002  [                  
					if ( fabs(ptt[2][0]) < fabs((ptt[1][0]/2)) && !icadRealEqual(fabs(ptt[2][0]),fabs(ptt[1][0]/2)) )
						sideflg=FALSE;
					else
						sideflg=TRUE;									
					Selfirst = FALSE;
					//BugZilla No. 77770; 08-07-2002  ]                  
                }
                if (mode==1 || mode==3) {

                    // EBATECH(watanabe)-[FIX : not horizontal
                    //ptd[0][1]+=((ptd[2][1]>=ptd[3][1])?-(plinkvar[DIMDLI].dval.rval*plinkvar[DIMSCALE].dval.rval):(plinkvar[DIMDLI].dval.rval*plinkvar[DIMSCALE].dval.rval));
                    double d = plinkvar[DIMDLI].dval.rval;
                    d *= plinkvar[DIMSCALE].dval.rval;
                    if (ptd[2][1] >= ptd[3][1])
                      {
                        ptd[0][1] -= d;
                      }
                    else
                      {
                        ptd[0][1] += d;
                      }
                    // ]-EBATECH(watanabe)

                    cmd_dimtoucs(ptu[1],ptd[0],ptu[0],sinofang,cosofang);
                }
                // Set the baselines 14 point to the (X) of the 13 point or the 14 point.
                ptd[0][0]=(sideflg)?ptd[1][0]:0.0;
                ptd[0][1]=(mode==0 || mode==2)?ptd[1][1]:0.0;
                cmd_dimtoucs(ptu[1],ptd[0],ptd[2],sinofang,cosofang);
                distance=fabs(pt1[(angle)?1:0]-ptd[2][(angle)?1:0]);
                break;
        }
		//BugZilla No. 78151; 13-05-2002
        //if (mode==2 || mode==3) {
		if((type != DIM_ANGULAR) && (type != DIM_ANGULAR_3P) &&  (mode==2 || mode==3)){
			char dimtxt[IC_ACADBUF];
			//EBATECH(CNBR) Bugzilla#78443 Use DIMZIN from current SYSVAR buf.
			cmd_rtos_dim(distance,plinkvar[DIMUNIT].dval.ival,plinkvar[DIMDEC].dval.ival,dimtxt, plinkvar[DIMZIN].dval.ival, plinkvar[DIMDSEP].dval.ival);
			//cmd_rtos_dim(distance,plinkvar[DIMUNIT].dval.ival,plinkvar[DIMDEC].dval.ival,dimtxt, DIMTYPEMAIN);
            sprintf(cmd_string,ResourceString(IDC_DIMENSIONS__NDIMENSION__52, "\nDimension text <%s>: " ),dimtxt);
            ret=sds_getstring(1,cmd_string,cmd_string);
            if (ret==RTERROR || ret==RTCAN) goto exit;
			//Bugzilla No. 78105; 01-07-2002 [
			//Add by IntelliKorea for get the text value in dim->base command. 2001/6/6
            //if (*cmd_string) {
            //    strcpy(plink->text,cmd_string);
            //    strcpy(dimtxt,plink->text);
            //}
			//End of add. IntelliKorea 2001/6/6
			// Bugzilla No. 78106; 15/04/2002
			//else strcpy(plink->text,dimtxt);
			strcpy(dimtxt,cmd_string);
			strcpy(plink->text,dimtxt);
			//Bugzilla No. 78105; 01-07-2002 ]
        }

        ic_ptcpy(plink->pt210,lastdimpts.pt210);
        cmd_initialrbcs(&rbecs,&rbwcs,&rbucs,flp,plink->pt210);

        db_ucshelper(lastdimpts.ucsorg,lastdimpts.ucsaxis,lastdimpts.elev,ecspt,wcspt,&fr1);
        plink->Setparflag=TRUE;
        if (mode==0 || mode==2) {
            fi1=plinkvar[DIMFIT].dval.ival;
            fi1=plinkvar[DIMFIT].dval.ival=4;
        }
        switch(type) {
            case DIM_ANGULAR:
            case DIM_ANGULAR_3P:
                // Copy data to the last points picked structure.
                ic_ptcpy(lastdimpts.pt10,ptu[0]);
                ic_ptcpy(lastdimpts.pt13,pt3);
                ic_ptcpy(lastdimpts.pt14,pt1);
                ic_ptcpy(lastdimpts.pt15,pt2);
                ic_ptcpy(lastdimpts.pt16,dimzeropt);
                ic_ptcpy(lastdimpts.startpt,((mode==1 || mode==3)?pt3:pt1));
                lastdimpts.i70=(DIM_ANGULAR_3P|DIM_BIT_6);
                // Do the transformaion from (UCS) to (NATIVE) format this is becouse when a dimension is created via entmake the points need to be in
                // native format for compatability when someone does an entmake from the command line.
                ic_ptcpy(ptu[1],ptu[0]);
                sds_trans(pt2,&rbucs,&rbwcs,0,pt2);
                sds_trans(ptu[0],&rbucs,&rbwcs,0,ptu[0]);
                sds_trans(ptu[0],&rbucs,&rbecs,0,ptu[1]);
                sds_trans(pt3,&rbucs,&rbwcs,0,pt3);
                sds_trans(pt1,&rbucs,&rbwcs,0,pt1);
                // Create dimension as a block.
                if ((elist=sds_buildlist(RTDXF0,"DIMENSION"/*DNT*/,
                                         12 ,wcspt,
                                         51 ,fr1,
                                         210,plink->pt210,
                                         15 ,pt2,
                                         70 ,(DIM_ANGULAR_3P|DIM_BIT_6),
										 //BugZilla No. 78151; 13-05-2002
                                         //1  ,plink->text,             // This needs to be changed.
                                         10 ,ptu[0],
                                         11 ,ptu[1],
                                         13 ,pt3,
                                         14 ,pt1,
                                          0))==NULL) goto exit;
                break;
            case DIM_x_ORDINATE:
			//BugZilla No. 78153; 03-06-2002
			case DIM_Y_ORDINATE:
                ic_ptcpy(lastdimpts.pt13,pt1);
                ic_ptcpy(lastdimpts.pt14,ptu[2]);
                ic_ptcpy(lastdimpts.pt15,dimzeropt);
				//BugZilla No. 78153; 03-06-2002
				sds_trans(pt1,&rbucs,&rbwcs,0,pt1);
                sds_trans(ptu[2],&rbucs,&rbwcs,0,ptu[2]);
                
                if ((elist=sds_buildlist(RTDXF0,"DIMENSION"/*DNT*/,
											 //BugZilla No. 78153; 03-06-2002
											 1,plink->text,
                                             10,dimzeropt,
                                             11,dimzeropt,
                                             12,dimzeropt,
                                             70,(type|DIM_BIT_6),
                                             13,pt1,
                                             14,ptu[2],
                                             210,plink->pt210,
											 //BugZilla No. 78153; 03-06-2002
                                             //50,0.0,0))==NULL) goto exit;
											 51,fr1,0))==NULL) goto exit;
                break;
            default:
                // Copy data to the last points picked structure.
                ic_ptcpy(lastdimpts.pt10,ptu[0]);
                ic_ptcpy(lastdimpts.pt13,ptd[2]);
                ic_ptcpy(lastdimpts.pt14,pt1);
                ic_ptcpy(lastdimpts.pt15,dimzeropt);
                ic_ptcpy(lastdimpts.pt16,dimzeropt);
                ic_ptcpy(lastdimpts.startpt,ptu[0]);
                lastdimpts.r50=angle;
                lastdimpts.i70=(type|DIM_BIT_6);
                // Do the transformaion from (UCS) to (NATIVE) format this is becouse when a dimension is created via entmake the points need to be in
                // native format for compatability when someone does an entmake from the command line.
                sds_trans(ptu[0],&rbucs,&rbwcs,0,ptu[0]);
                sds_trans(ptd[2],&rbucs,&rbwcs,0,ptd[2]);
                sds_trans(pt1,&rbucs,&rbwcs,0,pt1);
                // Create dimension as a block.
                if ((elist=sds_buildlist(RTDXF0,"DIMENSION"/*DNT*/,
											//Add by SMR for dim->base text 2001/6/6
                                             1 ,plink->text,
											 //End of add. SMR 2001/6/6
                                             10,ptu[0],
                                             11,dimzeropt,
                                             12,wcspt,
                                             70,(type|DIM_BIT_6),
                                             13,ptd[2],
                                             14,pt1,
                                             50,angle,
                                             51,fr1,
											 53,plink->textrot,//Add by SMR 2001/2/20
                                             210,plink->pt210,0))==NULL) goto exit;
                     break;
        }
        if (mode!=1) {
            dimseval=(plinkvar[DIMSE2].dval.ival);
            setgetrb.resval.rint=1; sds_setvar("DIMSE2"/*DNT*/,&setgetrb);
        }
        cmd_buildEEdList(elist);
        if (sds_entmake(elist)!=RTNORM) goto exit;

        // EBATECH(watanabe)-[recover angle
        lastdimpts.r50 = angle;
        // ]-EBATECH(watanabe)

        if (mode!=1) setgetrb.resval.rint=dimseval; sds_setvar("DIMSE2"/*DNT*/,&setgetrb);
        if (sds_entlast(lastdimpts.ename)!=RTNORM) goto exit;
        plink->GenInitflg=FALSE;
        plink->Setparflag=FALSE;
		//BugZilla No. 78153; 03-06-2002
		plink->DefValidflg=FALSE;
        if (mode==0 || mode==2) {
            fi1=plinkvar[DIMFIT].dval.ival=fi1;
        }
        IC_RELRB(elist);		// Was IC_FREEIT 
    } while(mode==0 || mode==1);

	IC_RELRB(elist);
    return(RTNORM);

    exit:
        plink->Setparflag=FALSE;
        plink->GenInitflg=FALSE;
		//BugZilla No. 78153; 03-06-2002
		plink->DefValidflg=FALSE;
		plink->HomeText = FALSE;	// EBATECH(CNBR) Bugzilla#78455 13-03-2003 Support DIMTMOVE
        IC_RELRB(elist);		// Was IC_FREEIT
        return(RTERROR);
}
