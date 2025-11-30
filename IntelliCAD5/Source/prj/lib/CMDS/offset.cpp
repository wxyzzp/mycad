/* F:\DEV\PRJ\LIB\CMDS\OFFSET.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */
// THIS FILE HAS BEEN GLOBALIZED!
#include <vector>
#include "cmds.h"/*DNT*/
#include "Icad.h"/*DNT*/
#include "splines.h"/*DNT*/
#include "IcadView.h"/*DNT*/

// the following are for cmd_offset_pline, which uses Geo.
#include "Geometry.h"/*DNT*/
#include "Point.h"/*DNT*/
#include "GeoConvert.h"/*DNT*/
#include "Path.h"/*DNT*/
#include "Curve.h"/*DNT*/
#include "EllipArc.h"/*DNT*/
#include "Line.h"/*DNT*/
#include "CrvPair.h"/*DNT*/


enum OffsetDistanceMethod{Uninitialized_t, // no value set
                          UseDistance_t, // use the input distance for offset distance
				          UsePickPoint_t};  // use the pick point for offset distance

OffsetDistanceMethod DistanceMethod;


// Protos
int cmd_offset_func(sds_name ename, double offset, int bothmode, int plmode, sds_point dirIndicator, bool bInteractive, sds_name ssetout=NULL);
int cmd_offset_pline(db_handitem * inputPLINE, // I: polyline to be offset
					 double rDistance, // I: the offset distance
					 sds_point sds_pick, // I: the pick point
					 int mode, // 0 to use rDistance and to use sds_pick to pick the side
					           // 1 to ignore rDistance and to use sds_pick as a through point.
							   // 2 will not be used by this routine and will return an error
					 std::vector<db_handitem*> &outputPLINES);

//Modified Cybage SBD 05/11/2001 DD/MM/YYYY
//Reason : Fix for Bug No. 77864 from Bugzilla
bool areCoincident(PCurve  pC1,PCurve  pC2);

int cmd_offset_linepts(sds_point p1,sds_point p2, sds_real offset, sds_point p3, sds_point p4){
	//offsets p1-p2 by distance specified in offset.  if offset <0, offset is to left
	//if offset >0, offset is to right
    sds_real fr1,fr2,fr3;

    fr3=sds_distance(p1,p2);
	if (icadRealEqual(fr3,0.0)){
		return(-1);
	}else{
		fr1=(p2[0]-p1[0])/fr3;	//unit dx1 for line
		fr2=(p2[1]-p1[1])/fr3;	//unit dy1 for line
		p3[0]=p1[0]+(fr2 * offset);
		p3[1]=p1[1]-(fr1 * offset);
		p4[0]=p2[0]+(fr2 * offset);
		p4[1]=p2[1]-(fr1 * offset);
		p3[2]=p4[2]=p1[2];
	}
	return(0);
}

int cmd_offset_arcpts(sds_point p1,sds_point p2,sds_real bulge,sds_real offset,sds_point p3,sds_point p4)
	{
	//offsets pline segment.  if offset < 0, offset is to left.  This means for
	//ccw bulge it's an inner arc, for cw bulge it's an outer one.
	//Reverse applies if offset > 0
	//Returns -1 if pts coincident, 0 if unable to offset, 1 if successful
    sds_real rr,sa,ea;
    sds_point p0;

	if (icadPointEqual(p1,p2))
		return-1;
	ic_bulge2arc(p1,p2,bulge,p0,&rr,&sa,&ea);

	if (bulge<0.0)
		{
		if (offset>0.0 && offset>rr)
			return(0);
        sds_polar(p1,ea,-offset,p3);
        sds_polar(p2,sa,-offset,p4);
		}
	else
		{
		if (offset<0.0 && (-offset)>rr)
			return(0);
        sds_polar(p1,sa,offset,p3);
        sds_polar(p2,ea,offset,p4);
		}
	if (icadPointEqual(p3,p4))
		return(0);
	return(1);
	}


short cmd_offset(void)
	{
    if (cmd_iswindowopen() == RTERROR)
		return RTERROR;

	//BugZilla No. 78050; 25-03-2002
	if (cmd_istransparent() == RTERROR) return RTERROR;

	// set the global DistanceMethod
	DistanceMethod = Uninitialized_t;

    sds_point p0;
    sds_name ename,sstemp;
	struct resbuf *elist, rb;
	RT ret=0;
	int fi1,plmode,bothmode=0;
	long fl1;
	char fs1[IC_ACADBUF],fs2[IC_ACADBUF],etype[35];
    sds_real offset,fr1;

	rb.rbnext=NULL;
	elist=NULL;
	SDS_getvar(NULL,DB_QOFFSETDIST,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	offset=rb.resval.rreal;
	if (offset<0.0)
		{
        sds_initget(SDS_RSG_NODOCCHG | RSG_NOZERO | RSG_NONEG,ResourceString(IDC_OFFSET_INITGET_THROUGH_P_0, "Through_point|Through ~_Through" ));
		sprintf(fs1,ResourceString(IDC_OFFSET__NPARALLEL__ENTER_1, "\nParallel: ENTER for Through point/<Distance>: " ),fs2);
		}
	else
		{
        sds_rtos(rb.resval.rreal,-1,-1,fs2);
        sds_initget(SDS_RSG_NODOCCHG | RSG_NOZERO | RSG_NONEG,ResourceString(IDC_OFFSET_INITGET_THROUGH_P_0, "Through_point|Through ~_Through" ));
		sprintf(fs1,ResourceString(IDC_OFFSET__NPARALLEL___THRO_2, "\nParallel:  Through point/<Distance> <%s>: " ),fs2);
		}

    if ((fi1=sds_getdist(NULL,fs1,&fr1))==RTKWORD)
		{
		offset=rb.resval.rreal=-1.0;
        if (sds_setvar("OFFSETDIST"/*DNT*/,&rb)!=RTNORM)
			return(-1);
		}
	else if (fi1==RTNORM)
		{
		if (fr1<0.0)
			fr1=-1.0;
		offset=rb.resval.rreal=fr1;
        if (sds_setvar("OFFSETDIST"/*DNT*/,&rb)!=RTNORM)
			return(-1);
		}
	else if (fi1==RTNONE)
		{
		DistanceMethod = UseDistance_t;
		offset=rb.resval.rreal;
		}
	else
		goto exit;

	if (icadRealEqual(offset,0.0))	//fcad bug - default dist is 0.0
		{
		cmd_ErrorPrompt(CMD_ERR_NONZERO,0);
		return(-1);
		}
	for (;;)
		{
		rb.restype=RTSTR;
		rb.rbnext=NULL;
		rb.resval.rstring=(char *)(LPCTSTR)"*"/*DNT*/;
		bothmode=0;
        FreeResbufIfNotNull(&elist);
		fi1=sds_pmtssget( ResourceString(IDC_OFFSET__NSELECT_ENTITY___3, "\nSelect entity: " ),""/*DNT*/,&elist,NULL,NULL,sstemp,1);
		if (fi1==RTERROR)
			{
			if (elist!=NULL) 	//they tried, but didn't get anything
				continue;
			else
				{
				ret=-1;
				goto exit;
				}
			}
		else if (fi1==RTNORM)
			{
			sds_sslength(sstemp,&fl1);
			// Bugzilla No. 78245; 22/07/2002 [
			/*if (fl1==0L)
				{
				sds_ssfree(sstemp);
				continue;
				}
			else if (fl1>1L)
				{
				for (fl1=1;sds_ssname(sstemp,fl1,ename)==RTNORM;fl1++)
					sds_redraw(ename,IC_REDRAW_UNHILITE);
				sds_ssfree(sstemp);
				cmd_ErrorPrompt(CMD_ERR_SINGLEOBJECT,0);
				continue;
				}*/
			if (fl1 != 1L)
			{
				for (fl1=0;sds_ssname(sstemp,fl1,ename)==RTNORM;fl1++)
					sds_redraw(ename,IC_REDRAW_UNHILITE);
				sds_ssfree(sstemp);
				cmd_ErrorPrompt(CMD_ERR_SINGLEOBJECT,0);
				continue;
				}
			// Bugzilla No. 78245; 22/07/2002 ]
  			if (sds_ssname(sstemp,0L,ename)!=RTNORM) {ret=-1;goto exit;}
			sds_ssfree(sstemp);
			if (elist!=NULL)
				{
				sds_relrb(elist);
				elist=NULL;
				}
			}
		else if (fi1=RTCAN)
			{
			ret=-1;
			goto exit;
			}
        if ((elist=sds_entgetx(ename,&rb))==NULL)
			{
			ret=(-__LINE__);
			goto exit;
			}
		ic_assoc(elist,0);
		strncpy(etype,ic_rbassoc->resval.rstring,sizeof(etype)-1);

		if (!strsame(etype,"POLYLINE"/*DNT*/) && !strsame(etype,"LWPOLYLINE"/*DNT*/))  // not a polyline/lwpolyline
			{
            // We need to save off the lastpoint variable and restore it after we
            // get the side to offset to. internalGetpoint() resets it and for this
            // prompt, we are only using it to determine which side to offset to.
            // BugFix # 6388.
            SDS_getvar(NULL,DB_QLASTPOINT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

            if (RTNORM!=sds_wcmatch(etype,"LINE,ARC,CIRCLE,RAY,XLINE,ELLIPSE,SPLINE"/*DNT*/))
				{
				cmd_ErrorPrompt(CMD_ERR_ENTTYPE,0);
				continue;
				}
			if (offset<0.0)
				{
				if (internalGetpoint(NULL,ResourceString(IDC_OFFSET__NTHROUGH_POINT___4, "\nThrough point: " ),p0)!=RTNORM)
					{
					sds_redraw(ename,IC_REDRAW_UNHILITE);
					goto exit;
					}
				}
			else
				{
				sds_initget(SDS_RSG_NODOCCHG,ResourceString(IDC_OFFSET_INITGET_BOTH_SIDE_5, "Both_sides|Both ~_Both" ));
				if ((fi1=internalGetpoint(NULL,ResourceString(IDC_OFFSET__NBOTH_SIDES__SID_6, "\nBoth sides/<Side for parallel copy>: " ),p0))!=RTNORM && fi1!=RTKWORD)
					{
					sds_redraw(ename,IC_REDRAW_UNHILITE);
					goto exit;
					}
				if (fi1==RTKWORD)
					bothmode=1;
				}
            // Restore last point.
            SDS_setvar(NULL,DB_QLASTPOINT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
			}
		else		// is a polyline/lwpolyline -- MR -- I think this code will work for both types
			{
			ic_assoc(elist,70);
			if (ic_rbassoc->resval.rint&(IC_PLINE_3DPLINE | IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH))
				{
				sds_redraw(ename,IC_REDRAW_UNHILITE);
                cmd_ErrorPrompt(CMD_ERR_ENTTYPE,0);
                continue;
				}
			plmode=1;
			if (offset<0.0)
				{
//Removed Wrap option bug 1-74926 				sds_initget(SDS_RSG_NODOCCHG,ResourceString(IDC_OFFSET_INITGET_WRAP_7, "Wrap ~_Wrap" ));
//Removed Wrap option bug 1-74926				if ((fi1=internalGetpoint(NULL,ResourceString(IDC_OFFSET__NWRAP__THROUGH_P_8, "\nWrap/<Through point>: " ),p0))!=RTNORM && fi1!=RTKWORD)
				if ((fi1=internalGetpoint(NULL,ResourceString(IDC_OFFSET__NTHROUGH_POINT___4, "\nThrough point: " ),p0))!=RTNORM && fi1!=RTKWORD)
					{
					sds_redraw(ename,IC_REDRAW_UNHILITE);
					goto exit;
					}
//Removed Wrap option bug 1-74926				if (fi1==RTKWORD)
//Removed Wrap option bug 1-74926					plmode=0;
				}
			else
				{
//Removed Wrap option bug 1-74926				sds_initget(SDS_RSG_NODOCCHG,ResourceString(IDC_OFFSET_INITGET_WRAP_BOTH_9, "Wrap Both_sides|Both ~_Wrap ~_Both" ));
//Removed Wrap option bug 1-74926				if ((fi1=internalGetpoint(NULL,ResourceString(IDC_OFFSET__NWRAP_BOTH_SIDE_10, "\nWrap/Both sides/<Side for parallel copy>: " ),p0))!=RTNORM && fi1!=RTKWORD)
				sds_initget(SDS_RSG_NODOCCHG,ResourceString(IDC_OFFSET_INITGET_BOTH_SIDE_5, "Both_sides|Both ~_Both"));
				if ((fi1=internalGetpoint(NULL,ResourceString(IDC_OFFSET__NBOTH_SIDES__SID_6, "\nBoth sides/<Side for parallel copy>: " ),p0))!=RTNORM && fi1!=RTKWORD)
					{
					sds_redraw(ename,IC_REDRAW_UNHILITE);
					goto exit;
					}
				if (fi1==RTKWORD)
					{
                    // At this point p0 is garbage. Need to init it. I'll set it to the
                    // first valid vertex. 1-75346 FIX
                    if (strsame(etype,"LWPOLYLINE"/*DNT*/))
                        ((db_lwpolyline*)ename[0])->get_10(p0, 0);
                    else
                        {
                        sds_name vertName;
                        sds_entnext_noxref(ename,vertName);
                        ((db_vertex*)vertName[0])->get_10(p0);
                        }
//Removed Wrap option bug 1-74926					sds_getinput(fs1);
//Removed Wrap option bug 1-74926					if (strisame(fs1,"WRAP"/*DNT*/))
//Removed Wrap option bug 1-74926						plmode=0;
//Removed Wrap option bug 1-74926					else
						bothmode=1;
					}
				}
			if (plmode==0)
				{
				fi1=RTNORM;
				if (offset<0.0)
					{
                    sds_initget(SDS_RSG_NODOCCHG,NULL);
                    if ((fi1=sds_getpoint(NULL,ResourceString(IDC_OFFSET__NTHROUGH_POINT___4, "\nThrough point: " ),p0))!=RTNORM)
						{
						sds_redraw(ename,IC_REDRAW_UNHILITE);
						goto exit;
						}
					}
				else
					{
                    sds_initget(SDS_RSG_NODOCCHG,ResourceString(IDC_OFFSET_INITGET_BOTH_SIDE_5, "Both_sides|Both ~_Both" ));
                    if ((fi1=sds_getpoint(NULL,ResourceString(IDC_OFFSET__NBOTH_SIDES__SID_6, "\nBoth sides/<Side for parallel copy>: " ),p0))!=RTNORM && fi1!=RTKWORD)
						{
						sds_redraw(ename,IC_REDRAW_UNHILITE);
						goto exit;
						}
					if (fi1==RTKWORD)
						bothmode=1;
					}
				}
			if (fi1!=RTNORM && fi1!=RTKWORD)
				goto exit;
			}

		sds_redraw(ename,IC_REDRAW_UNHILITE);

		if (offset < 0)
			DistanceMethod = UsePickPoint_t;
		else
			DistanceMethod = UseDistance_t;

		if (cmd_offset_func (ename, offset, bothmode, plmode, p0, true, NULL) != RTNORM)
			goto exit;
		}

	exit:
	FreeResbufIfNotNull(&elist);
	if (ret==0)
		{

		return(RTNORM);
		}
	if (ret==-1)
		return(RTCAN);
	if (ret<-1)
		CMD_INTERNAL_MSG(ret);
	return(ret);
}

// This function constructs the offset of inputPLINE and adds it to the drawing
int new_offset_pline(db_handitem * inputPLINE, // I: the polyline to be offset
					sds_point sds_pick, // I: the pick point
					double rDistance, // I: the offset distance
					int mode) // I: 0 to use rDistance; 1 to ignore rDistance and use sds_pick
	{
	int rc;

	db_handitem * outputPLINE = NULL;
	sds_resbuf * rb;
	db_handitem * hip;

	std::vector<db_handitem*> newPlines;
	rc = cmd_offset_pline(inputPLINE, rDistance, sds_pick, mode, newPlines);

	if (rc != RTNORM)
		return rc;

	for (register i = 0; i < newPlines.size(); i++) {
		outputPLINE = newPlines[i];
		if (outputPLINE->ret_type()==DB_LWPOLYLINE)
			{
			rb = outputPLINE->entgetx(NULL, SDS_CURDWG);

			// we are done with outputPLINE so we can delete it.
			delete outputPLINE;

			if (!rb)
				return RTERROR;

			rc = sds_entmake(rb);
			sds_relrb(rb);
		}
		else if (outputPLINE->ret_type()==DB_POLYLINE) {
			db_handitem * lasthip;
			hip = outputPLINE;
			bool reached_end = false;
			while (!reached_end) {   // begin while (!reached_end)
				rb = hip->entgetx(NULL, SDS_CURDWG);

				if (!rb) {
					delete hip;
					return RTERROR;
				}

				rc = sds_entmake(rb);
				sds_relrb(rb);

				if (rc != RTNORM)
					return rc;

				if (hip->ret_type() == DB_SEQEND) {
					delete hip;
					reached_end = true;
					break;
				}

				lasthip = hip;
				hip = hip->next;
				delete lasthip;

				if (hip == NULL) {
					reached_end = true;
					break;
				}
			}   //   end while (!reached_end)
		}
		else
			return RTERROR;
	}
	return RTNORM;
}


int cmd_offset_func(sds_name ename, double offset, int bothmode, int plmode, sds_point p0, bool bInteractive, sds_name ssetout)
	{
	char etype[35];
	struct resbuf *elist, rb, *rbtemp,rbucs,rbent;
	rbucs.restype=RTSHORT;
	rbucs.resval.rint=1; rb.rbnext=NULL;
	elist=rbtemp=NULL;

    sds_point p1,p2,p3,p4;
	RT ret=0;
    sds_real fr1,fr2,fr3;

	if ((elist=sds_entgetx(ename,&rb))==NULL)
		return RTERROR;

	ic_assoc(elist,0);
	strncpy(etype,ic_rbassoc->resval.rstring,sizeof(etype)-1);

	//initialize the output selection set
	if (ssetout)
		sds_ssadd (NULL, NULL, ssetout);

	if (strsame(etype, "POLYLINE") || strsame(etype, "LWPOLYLINE"))
		{
		int distancemode;

		ASSERT(DistanceMethod != Uninitialized_t);

		if (DistanceMethod == UsePickPoint_t)
			distancemode = 1;
		else if (DistanceMethod == UseDistance_t)
			distancemode = 0;

		if (bothmode)
			{
			ic_assoc(elist,70);
			if (0==ic_assoc(elist,210))
				{
				rbent.restype=RT3DPOINT;
				ic_ptcpy(rbent.resval.rpoint,ic_rbassoc->resval.rpoint);
				sds_trans(p0,&rbucs,&rbent,0,p1);
				ic_assoc(elist,10);
				p4[2]=ic_rbassoc->resval.rpoint[2];
				if (!icadRealEqual(p1[2],p4[2]))
					cmd_elev_correct(p4[2],p1,3,rbent.resval.rpoint);
				}
			else
				{
				rbent.restype=RTSHORT;
				rbent.resval.rint=0;
				sds_trans(p0,&rbucs,&rbent,0,p1);
				}
			ret = new_offset_pline((db_handitem *)ename[0],p1,offset,distancemode);
			// error processing for degenerate input or output
			if (ret==CMD_ERR_OFFZEROLEN)
				{
				if (bInteractive)
					cmd_ErrorPrompt(CMD_ERR_OFFZEROLEN,0);
				ret = RTERROR;
				goto exit;
				}
			else if (ret==CMD_ERR_OFFSHARPCURVE)
				{
				if (bInteractive)
					cmd_ErrorPrompt(CMD_ERR_OFFSHARPCURVE,0);
				ret = RTERROR;
				goto exit;
				}
			else if (ret==CMD_ERR_OFFSMALLRAD)
				{
				if (bInteractive)
					cmd_ErrorPrompt(CMD_ERR_OFFSMALLRAD,0);
				ret = RTERROR;
				goto exit;
				}
			else if (ret==CMD_ERR_OFFSET_FAILURE) 
				{
				if (bInteractive)
					cmd_ErrorPrompt(CMD_ERR_OFFSET_FAILURE,0);
				ret = RTERROR;
				goto exit;
				}
			if (ssetout)
				{
				sds_name tmpename;
				sds_entlast(tmpename);
				sds_ssadd (tmpename, ssetout, ssetout);
				} 
			ret = new_offset_pline((db_handitem *)ename[0],p1,-offset,distancemode);
			// error processing for degenerate input or output
			if (ret==CMD_ERR_OFFZEROLEN)
				{
				if (bInteractive)
					cmd_ErrorPrompt(CMD_ERR_OFFZEROLEN,0);
				ret = RTERROR;
				goto exit;
				}
			else if (ret==CMD_ERR_OFFSHARPCURVE)
				{
				if (bInteractive)
					cmd_ErrorPrompt(CMD_ERR_OFFSHARPCURVE,0);
				ret = RTERROR;
				goto exit;
				}
			else if (ret==CMD_ERR_OFFSMALLRAD)
				{
				if (bInteractive)
					cmd_ErrorPrompt(CMD_ERR_OFFSMALLRAD,0);
				ret = RTERROR;
				goto exit;
				}
			else if (ret==CMD_ERR_OFFSET_FAILURE) 
				{
				if (bInteractive)
					cmd_ErrorPrompt(CMD_ERR_OFFSET_FAILURE,0);
				ret = RTERROR;
				goto exit;
				}
			if (ssetout)
				{
				sds_name tmpename;
				sds_entlast(tmpename);
				sds_ssadd (tmpename, ssetout, ssetout);
				}
			}
		else
			{
			//through point...
			ic_assoc(elist,70);
			if (0==ic_assoc(elist,210))
				{
				rbent.restype=RT3DPOINT;
				ic_ptcpy(rbent.resval.rpoint,ic_rbassoc->resval.rpoint);
				sds_trans(p0,&rbucs,&rbent,0,p1);
				ic_assoc(elist,10);
				p4[2]=ic_rbassoc->resval.rpoint[2];
				if (!icadRealEqual(p1[2],p4[2]))
					cmd_elev_correct(p4[2],p1,3,rbent.resval.rpoint);
				}
			else
				{
				rbent.restype=RTSHORT;
				rbent.resval.rint=0;
				sds_trans(p0,&rbucs,&rbent,0,p1);
				}
			ret = new_offset_pline((db_handitem *)ename[0],p1,offset,distancemode);
			// error processing for degenerate input or output
			if (ret==CMD_ERR_OFFZEROLEN)
				{
				if (bInteractive)
					cmd_ErrorPrompt(CMD_ERR_OFFZEROLEN,0);
				ret = RTERROR;
				goto exit;
				}
			else if (ret==CMD_ERR_OFFSHARPCURVE)
				{
				if (bInteractive)
					cmd_ErrorPrompt(CMD_ERR_OFFSHARPCURVE,0);
				ret = RTERROR;
				goto exit;
				}
			else if (ret==CMD_ERR_OFFSMALLRAD)
				{
				if (bInteractive)
					cmd_ErrorPrompt(CMD_ERR_OFFSMALLRAD,0);
				ret = RTERROR;
				goto exit;
				}
			else if (ret==CMD_ERR_OFFSET_FAILURE) 
				{
				if (bInteractive)
					cmd_ErrorPrompt(CMD_ERR_OFFSET_FAILURE,0);
				ret = RTERROR;
				goto exit;
				}
			if (ssetout)
				{
				sds_name tmpename;
				sds_entlast(tmpename);
				sds_ssadd (tmpename, ssetout, ssetout);
				}
			}
		goto exit;
		}

	if (sds_wcmatch(etype,"ARC,CIRCLE"/*DNT*/)==RTNORM)
		{
		for (rbtemp=elist; rbtemp!=NULL; rbtemp=rbtemp->rbnext)
			{
			if (10==rbtemp->restype)
				ic_ptcpy(p2,rbtemp->resval.rpoint);
			else if (40==rbtemp->restype)
				fr1=rbtemp->resval.rreal;
			else if (210==rbtemp->restype)
				{
				ic_ptcpy(rbent.resval.rpoint,rbtemp->resval.rpoint);
				rbent.restype=RT3DPOINT;
				}
			}
		sds_trans(p0,&rbucs,&rbent,0,p3);
		if (!icadRealEqual(p3[2],p2[2]))
			{
			SDS_getvar(NULL,DB_QVIEWDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			sds_trans(rb.resval.rpoint,&rbucs,&rbent,1,rb.resval.rpoint);
			if (fabs(rb.resval.rpoint[2])<CMD_FUZZ)
				{
				if (bInteractive)
					cmd_ErrorPrompt(CMD_ERR_SELECTION,0);
				goto exit;
				}
			fr2=(p3[2]-p2[2])/rb.resval.rpoint[2];
			p3[0]+=fr2*rb.resval.rpoint[0];
			p3[1]+=fr2*rb.resval.rpoint[1];
			p3[2]=p2[2];
			}
		if (offset>0.0)
			{
			fr2=cmd_dist2d(p2,p3);
			if (bothmode ||(cmd_dist2d(p2,p3)<fr1))
				fr1-=offset;
			else
				fr1+=offset;
			}
		else
			fr1=cmd_dist2d(p3,p2);
		if (fr1>0.0)
			{
			ic_assoc(elist,40);
			ic_rbassoc->resval.rreal=fr1;
			if (sds_entmake(elist)!=RTNORM) {ret=-1;goto exit;}
			if (ssetout)
				{
				sds_name tmpename;
				sds_entlast(tmpename);
				sds_ssadd (tmpename, ssetout, ssetout);
				}
			}
		else
			{
			if (bInteractive)
				cmd_ErrorPrompt(CMD_ERR_OFFSMALLRAD,0);
			goto exit;
			}
		if (bothmode)
			{
			ic_rbassoc->resval.rreal+=2.0*offset;
			if (sds_entmake(elist)!=RTNORM) {ret=-1;goto exit;}
			if (ssetout)
				{
				sds_name tmpename;
				sds_entlast(tmpename);
				sds_ssadd (tmpename, ssetout, ssetout);
				}
			}
		goto exit;
		}

	if (strsame(etype,"ELLIPSE"/*DNT*/) || strsame(etype,"SPLINE"/*DNT*/))
		{
		// Set the mode sentinel
		int mode = 0;
		if (offset < 0)
			mode = 1;
		else if (bothmode)
			mode = 2;

		// Transform the pick point to wcs
		rb.restype=RTSHORT;
		rb.resval.rint=0;		// wcs
		rbucs.restype=RTSHORT;
		rbucs.resval.rint=1;	// ucs
		sds_trans(p0, &rbucs, &rb, 0, p4);

		// Make the offset
		if (strsame(etype,"ELLIPSE"/*DNT*/))
			ret = gr_offset_ellipse(elist, fabs(offset), p4, mode, &rbtemp);
		else
			ret = gr_offset_spline(elist, fabs(offset), p4, mode, &rbtemp);
		if (ret)
			{
			if (ret == CMD_ERR_OFFSHARPCURVE)
				{
				// The offset distance is too big, but that's not fatal
				if (bInteractive)
					cmd_ErrorPrompt(CMD_ERR_OFFSHARPCURVE,0);
				ret = 0;
				}
			else
				goto exit;
			}
		else
			{
			// Create the entity
			if (sds_entmake(rbtemp)!=RTNORM)     {ret=-1; goto exit;}
			if (ssetout)
				{
				sds_name tmpename;
				sds_entlast(tmpename);
				sds_ssadd (tmpename, ssetout, ssetout);
				}
			if (rbtemp)
				{
				sds_relrb(rbtemp);
				rbtemp=NULL;
				}
			}

		if (bothmode)
			{
			// Do the same on the other side
			if (strsame(etype,"ELLIPSE"/*DNT*/))
				ret = gr_offset_ellipse(elist, -fabs(offset), p4, mode, &rbtemp);
			else
				ret = gr_offset_spline(elist, -fabs(offset), p4, mode, &rbtemp);
			if (ret)
				{
				if (ret == CMD_ERR_OFFSHARPCURVE)
					{
					// The offset distance is too big, but that's not fatal
					if (bInteractive)
						cmd_ErrorPrompt(CMD_ERR_OFFSHARPCURVE,0);
					ret = 0;
					goto exit;
					}
				else
					goto exit;
				}
			// Create the entity
			if (sds_entmake(rbtemp)!=RTNORM)
				{
				ret=-1;
				goto exit;
				}
			if (ssetout)
				{
				sds_name tmpename;
				sds_entlast(tmpename);
				sds_ssadd (tmpename, ssetout, ssetout);
				}
			if (rbtemp)
				{
				sds_relrb(rbtemp);
				rbtemp=NULL;
				}
			}
		goto exit;
		}

	if (strsame(etype,db_hitype2str(DB_LINE)) ||
		strsame(etype,"RAY"/*DNT*/) ||
		strsame(etype,"XLINE"/*DNT*/))
		{
		rbent.restype=RTSHORT;
		rbent.resval.rint=0;
		ic_assoc(elist,10);
		ic_ptcpy(p1,ic_rbassoc->resval.rpoint);
		ic_assoc(elist,11);
		ic_ptcpy(p2,ic_rbassoc->resval.rpoint);
		sds_trans(p1,&rbent,&rbucs,0,p1);
		sds_trans(p2,&rbent,&rbucs,0,p2);
		if (strsame(etype,db_hitype2str(DB_LINE)))
			{
			fr3=cmd_dist2d(p1,p2);
			if (icadRealEqual(fr3,0.0))
				{
				if (bInteractive)
					cmd_ErrorPrompt(CMD_ERR_OFFZEROLEN,0);
				goto exit;
				}
			p2[0]=(p2[0]-p1[0])/fr3;	//unit dx1 for existing line
			p2[1]=(p2[1]-p1[1])/fr3;	//unit dy1 for existing line
			p2[2]=0.0;
			}
		cmd_elev_correct(p1[2],p0,1,NULL);//move p0 along viewdir to same elev as p1
		//cross p1-p0 with p1-p2 to get normal and distance,
		//	and ignoring z's of vectors
		p3[0]=0.0;	//(p0[1]-p1[1])*p2[2]-p2[1]*(p0[2]-p1[2]);
		p3[1]=0.0;	//(p0[2]-p1[2])*p2[0]-p2[2]*(p0[0]-p1[0]);
		p3[2]=(p0[0]-p1[0])*p2[1]-p2[0]*(p0[1]-p1[1]);
		//fr3=sqrt(p3[0]*p3[0]+p3[1]*p3[1]+p3[2]*p3[2]);
		fr3=fabs(p3[2]);
		//NOTE fr3 is also dist from line to p4
		if (icadRealEqual(fr3,0.0))
			{
			if (bInteractive)
				cmd_ErrorPrompt(CMD_ERR_DIFFPTS,0);
			goto exit;
			}
		//now cross our p1-p2 vector with the p3 vector to get
		//	vector toward selected point.  Most of the math drops
		//	out by omission of the z components
		//p3[0]/=fr3;
		//p3[1]/=fr3;
		//p3[2]/=fr3;
		p1[0]=(p3[2]>0.0)?p2[1]:-p2[1];	//p2[1]*p3[2]-p3[1]*p2[2];
		p1[1]=(p3[2]>0.0)?-p2[0]:p2[0];	//p2[2]*p3[0]-p3[2]*p2[0];
		p1[2]=0.0;						//p2[0]*p3[1]-p3[0]*p2[1];
		if (offset>0.0)
			{
			p1[0]*=offset;
			p1[1]*=offset;
			//p1[2]*=offset;
			}
		else
			{
			p1[0]*=fr3;
			p1[1]*=fr3;
			//p1[2]*=fr3;
			}
		sds_trans(p1,&rbucs,&rbent,1,p1);
		ic_assoc(elist,10);
		ic_rbassoc->resval.rpoint[0]+=p1[0];
		ic_rbassoc->resval.rpoint[1]+=p1[1];
		ic_rbassoc->resval.rpoint[2]+=p1[2];
		if (strsame(etype,db_hitype2str(DB_LINE)))
			{
			ic_assoc(elist,11);
			ic_rbassoc->resval.rpoint[0]+=p1[0];
			ic_rbassoc->resval.rpoint[1]+=p1[1];
			ic_rbassoc->resval.rpoint[2]+=p1[2];
			}
		if (sds_entmake(elist)!=RTNORM) {ret=-1;goto exit;}
		if (ssetout)
			{
			sds_name tmpename;
			sds_entlast(tmpename);
			sds_ssadd (tmpename, ssetout, ssetout);
			}
		if (bothmode)
			{
			ic_assoc(elist,10);
			ic_rbassoc->resval.rpoint[0]-=2.0*p1[0];
			ic_rbassoc->resval.rpoint[1]-=2.0*p1[1];
			ic_rbassoc->resval.rpoint[2]-=2.0*p1[2];
			if (strsame(etype,db_hitype2str(DB_LINE)))
				{
				ic_assoc(elist,11);
				ic_rbassoc->resval.rpoint[0]-=2.0*p1[0];
				ic_rbassoc->resval.rpoint[1]-=2.0*p1[1];
				ic_rbassoc->resval.rpoint[2]-=2.0*p1[2];
				}
			if (sds_entmake(elist)!=RTNORM) {ret=-1;goto exit;}
			if (ssetout)
				{
				sds_name tmpename;
				sds_entlast(tmpename);
				sds_ssadd (tmpename, ssetout, ssetout);
				}
			}
		}

	exit:
	FreeResbufIfNotNull(&elist);
	FreeResbufIfNotNull(&rbtemp);
	if (ret==0)
		return(RTNORM);
	if (ret==-1)
		return(RTCAN);
	return(ret);
	}



// enumerated type that indicates the relative configuration of two neighboring curves in a path
enum IntType{StartStart_t, // start of curve i coincides with start of curve i+1
             StartEnd_t, // start of curve i coincides with end of curve i+1
			 EndStart_t, // end of curve i coincides with start of curve i+1
			 EndEnd_t, // end of curve i coincides with end of curve i+1
			 NoInt_t}; // none of the above configurations apply


// Determine the relative configuration of two curves that may be joined end to start,
// start to end, or not at all
IntType get_IntType(PCurve pC1, // I:  the first curve
					PCurve pC2) // I:  the second curve
	{
	IntType typ;

	C3Point S1,E1,S2,E2;

	pC1->GetStartPoint(S1);
	pC1->GetEndPoint(E1);

	pC2->GetStartPoint(S2);
	pC2->GetEndPoint(E2);

 	if (PointDistance(S1, S2) < FUZZ_DIST)
		typ = StartStart_t;
	else if (PointDistance(E1, S2) < FUZZ_DIST)
		typ = EndStart_t;
	else if (PointDistance(S1, E2) < FUZZ_DIST)
		typ = StartEnd_t;
	else if (PointDistance(E1, E2) < FUZZ_DIST)
		typ = EndEnd_t;
	else
		typ = NoInt_t;

	return typ;
	}

// align the curves in a path so that they join up end to start
RC AlignCurves(PPath & pPath)
	{
	RC rc = SUCCESS; // innocent until proven guilty
	int numAligned = 0;

	int numCurves = pPath->Size();

	int i;


	IntType typ;
	for (i = 0; i < numCurves - 1; i++)
		{
		// we use the intersection type of the input curves to determine
		// the processing of the output curves
		typ = get_IntType(pPath->Curve(i), pPath->Curve(i+1));
		if (typ == NoInt_t) return FAILURE;

		if ((typ == StartStart_t) || (typ == StartEnd_t))
			{
			rc = pPath->Curve(i)->Reverse();
			++numAligned;
			if (rc != SUCCESS)
				return rc;
			}

		}

	// finally if the input path is closed we have one more pair to trim or extend
	if (pPath->Closed())
		{
		int iLast = numCurves - 1;
		int iFirst = 0;

		typ = get_IntType(pPath->Curve(iLast), pPath->Curve(iFirst));
		if (typ == NoInt_t) return FAILURE;

		if ((typ == StartStart_t) || (typ == StartEnd_t))
			{
			rc = pPath->Curve(iLast)->Reverse();
			++numAligned;
			if (rc != SUCCESS)
				return rc;
			}
		// having done this, lets check to make sure that the intType is ok for the last to first.
		typ = get_IntType(pPath->Curve(iLast), pPath->Curve(iFirst));
		if (typ != EndStart_t)
		  rc = FAILURE;

		}



	return rc;
	}

// special-purpose function to adjust a start parameter of an elliptical
// object by factors of TWOPI until it is <= the end parameter
void AdjustStart(PCurve pC, double & start)
	{
	if (pC->Type() == TYPE_Elliptical)
		{
		while (start > pC->End())
			start -= TWOPI;
		}
	}

// reset the start parameter of a curve, adjusting it by factors of TWOPI if necessary.
void AdjustAndResetStart(PCurve pC, double & start)
	{
	RC rc;
	AdjustStart(pC, start);
	rc = pC->ResetStart(start);
	ASSERT(rc == SUCCESS);
	}

// special-purpose function to adjust an end parameter of an elliptical
// object by factors of TWOPI until it is >= the start parameter
void AdjustEnd(PCurve pC, double & end)
	{
	if (pC->Type() == TYPE_Elliptical)
		{
		while (end < pC->Start())
			end += TWOPI;
		}
	}
// reset the end parameter of a curve, adjusting it by factors of TWOPI if necessary.
void AdjustAndResetEnd(PCurve pC, double & end)
	{
	RC rc;
	AdjustEnd(pC, end);
	rc = pC->ResetEnd(end);
	ASSERT(rc == SUCCESS);
	}

// Helper function for trim_or_extend.  determines whether we need to call the
// Bump() function instead of trimming the line

bool NeedToBump(bool bLast, // I - true if this curve is in last trim_or_extend pair
				PCurve pC,  // I - the curve to be analyzed
				double t)   // I - the value that would be used for AdjustAndResetStart
				{
				bool doNeedToBump = false;

				if (!bLast && (pC->Type() == TYPE_Line) )
					{
					if (fabs(t - pC->End()) < FUZZ_DIST)
					// This is a case where we shouldn't trim, so
						doNeedToBump = true;
					}

				return doNeedToBump;
				}

// If the curve is a line from A to B, change it into a line from B to B + (B-A)
// otherwise do nothing.
void Bump(PCurve pC)
	{
	if (pC->Type() == TYPE_Line)
		{
		C3Point A;
		pC->GetStartPoint(A);
		C3Point B;
		pC->GetEndPoint(B);

		pC->SnapEnd(LEFT, B);
		pC->SnapEnd(RIGHT, B + (B-A));

		}
	}




// trim or extend two curves to their real or extended intersections
RC trim_or_extend(IntType typ, // I - intersection type (but must be EndStart_t)
				  bool bLast,  // I - true iff this is the last pair being processed
				  PCurve  pC1, // IO - the first curve to be trimmed or extended
				  PCurve  pC2, // IO - the second curve to be trimmed or extended
				  bool &bRemoveC1,	// O - the first curve is invalid and shoule be removed from the path
				  bool &bRemoveC2)  // O - the second curve is invalid and shoule be removed from the path
	{
	ASSERT(typ==EndStart_t);
	RC rc;

	PCurve pLocal1 = NULL;
	PCurve pLocal2 = NULL;
	bRemoveC1 = bRemoveC2 = false;

	if (pC1->Type() == TYPE_Line)
		{
		pLocal1 = new CUnboundedLine((CLine *)pC1);
		pLocal1->SetDomain(-INFINITY, INFINITY);
		}
	else if (pC1->Type() == TYPE_Elliptical)
		{
		pLocal1 = new CEllipArc(((CEllipArc *)pC1)->Center(),
			                    ((CEllipArc *)pC1)->MajorAxis(),
								((CEllipArc *)pC1)->MinorAxis(),
								0.0,
								TWOPI);
		}


	if (pC2->Type() == TYPE_Line)
		{
		pLocal2 = new CUnboundedLine((CLine *)pC2);
		pLocal2->SetDomain(-INFINITY, INFINITY);
		}
	else if (pC2->Type() == TYPE_Elliptical)
		{
		pLocal2 = new CEllipArc(((CEllipArc *)pC2)->Center(),
			                    ((CEllipArc *)pC2)->MajorAxis(),
								((CEllipArc *)pC2)->MinorAxis(),
								0.0,
								TWOPI);
		}


	double tol = 0.0001; // tolerance for intersection

	CRealArray onLocal1, onLocal2;
	double s1,t1,s2,t2;
	double v1,v2;

    rc = Intersect(pLocal1, pLocal2, onLocal1, onLocal2, tol);

	int temp = 1;
	int size1 = onLocal1.Size();
	int size2 = onLocal2.Size();

#ifdef _DEBUG
	// for debugging purposes
	if ((size1 == 0) || (size2 == 0))
		{
		double radius1;
		C3Point Center1;
		if (pLocal1->Type() == TYPE_Elliptical)
			{
			( (CEllipArc *) pLocal1)->IsCircular(radius1);
			Center1 = ( (CEllipArc *) pLocal1)->Center();
			}
		}
#endif // _DEBUG

    delete pLocal1;
    delete pLocal2;

	if (rc == SUCCESS)
		{
		double a1=pC1->Start();
		double b1=pC1->End();
		double a2=pC2->Start();
		double b2=pC2->End();

		if ((onLocal1.Size() == 1) && (onLocal2.Size() == 1)) // line-line intersections or line-arc tangent
			{  // begin case of one intersection point on each curve
			t1 = onLocal1[0];
			t2 = onLocal2[0];

			switch (typ)
				{  // begin switch (typ)
				case EndStart_t: // this should be the only case that occurs

					// the end parameter is greater than start parameter when t1 > 0.0. That
					// means the offset path is not twiced. Otherwise, C1 curve should be 
					// removed.
					//
					if (t1 > 0.0) 
						AdjustAndResetEnd(pC1, t1);
					else 
						bRemoveC1 = true;

					if (!bLast ||				// it is not the last parameter
						bLast && t2 < 1.0) { 	// the start parameter of the last line segment should be smaller than end parameter 1.0
						if (NeedToBump(bLast, pC2, t2))
							Bump(pC2);
						else
							AdjustAndResetStart(pC2,t2);
					}
					else
						bRemoveC2 = true;

					break;
				default:
					return FAILURE;
					break;
				}  //   end switch (typ)
			}  //   end case of one intersection point on each curve
		else if ((onLocal1.Size() == 2) && (onLocal2.Size() == 2)) // line-arc, arc-arc intersection
			{  // begin case of two intersection points on each curve
			s1 = onLocal1[0];
			s2 = onLocal2[0];
			t1 = onLocal1[1];
			t2 = onLocal2[1];

			switch (typ)
				{  // begin switch (typ)
				case EndStart_t: // this should be the only case that occurs
					{
					bool pt1IsInBound = (s1 > (a1 - FUZZ_DIST) && s1 < (b1 + FUZZ_DIST) &&
										 s2 > (a2 - FUZZ_DIST) && s2 < (b2 + FUZZ_DIST)) ? true : false;
					bool pt2IsInBound = (t1 > (a1 - FUZZ_DIST) && t1 < (b1 + FUZZ_DIST) &&
										 t2 > (a2 - FUZZ_DIST) && t2 < (b2 + FUZZ_DIST)) ? true : false;
					if (pt1IsInBound && !pt2IsInBound) { // the first poit is in the bound of the curves
						v1 = s1;
						v2 = s2;
					}
					else if (!pt1IsInBound && pt2IsInBound) { // the second point is in the bound of the curves
						v1 = t1;
						v2 = t2;
					}
					else { // both points are either in the bound or out of the bound of the curves

						// compare which intersection point is more closed to the end point of
						// the first curve
						//
						if (fabs(s1-b1) < fabs(t1-b1)) {
							v1 = s1;
							v2 = s2;
						}
						else {
							v1 = t1;
							v2 = t2;
						}
					}

					// the end parameter is greater than start parameter when t1 > 0.0. That
					// means the offset path is not twiced. Otherwise, C1 curve should be 
					// removed.
					//
					if (v1 > 0.0) 
						AdjustAndResetEnd(pC1, v1);
					else
						bRemoveC1 = true;

					if (!bLast ||				// it is not the last segment
						bLast && v2 < TWOPI) {	// the start parameter of last arc segment should be smaller than end parameter TWOPI
						if (NeedToBump(bLast, pC2, v2))
							Bump(pC2);
						else
							AdjustAndResetStart(pC2, v2);
					}
					else
						bRemoveC2 = true;

					break;
					}
				default:
					return FAILURE;
					break;
				}  //   end switch (typ)
			}  //   end case of two intersection points on each curve
/*
		// theoretically this case should never exist if curve is either line
		// or circular arc unless the Intersect() function gives duplicate of
		// solutions.
		//
		else if ( (onLocal1.Size() > 0) && (onLocal1.Size() == onLocal2.Size()) )
			{ // this logic could actually be used above also

#ifdef _DEBUG
			double t;
			int num = onLocal1.Size();
			for (register j = 0; j < num; j++)
				t = onLocal1[j];

			num = onLocal2.Size();
			for (j = 0; j < num; j++)
				t = onLocal2[j];
#endif // _DEBUG

			int i, iBest;
			double minDist = fabs(onLocal1[0] - b1);
			double dist;
			iBest = 0;

			for (i = 1; i < onLocal1.Size(); i++)
				{
				dist = fabs(onLocal1[i] - b1);
				if (dist < minDist)
					{
					minDist = dist;
					iBest = i;
					}
				}

			v1 = onLocal1[iBest];
			v2 = onLocal2[iBest];

			AdjustAndResetEnd(pC1, v1);

			if (NeedToBump(bLast, pC2, v2))
				Bump(pC2);
			else
				AdjustAndResetStart(pC2, v2);
			}
*/
		else
			rc = FAILURE;
		}

	return rc;
	}

// This function removes all short curves from a path
// If the post processed pPath has no curves left, we will return a failure, otherwise
// we will return success.
void RemoveDegenerateCurves(PPath & pPath, // IO - path to be processed
                            bool & degeneratePath)// O - true if the resulting path is degenerate
		{
		int numCurves = pPath->Size();

		int i;

		PCurve pCurve;

		double curveLength;

		RC rc;


		// start at the top and work our way down
		for (i = numCurves-1; i >= 0; i--)
			{
			pCurve = pPath->Curve(i);
			rc = pCurve->Length(curveLength);

			if (rc == SUCCESS)
				{
				if (curveLength < FUZZ_DIST)
					pPath->Remove(i);
				}
			}

		// now check whether there is anything left in the path
		if (pPath->Size() < 1)
			degeneratePath = true;
		else
			degeneratePath = false;

		}

// Test if an arc is inside of an offset zone - SWH, 9/12/2003
//
bool IsArcInsideOffsetZone(CEllipArc* pArc, 
						   CCurve* pOriginalCurve, 
						   CCurve* pOffsetCurve,
						   double offsetDistance)
{
	if (!pOriginalCurve || !pOffsetCurve)
		return false;

	double r = pArc->Radius();
	if (offsetDistance < 2.0 * r)
		return false;

	double dis1, dis2;
	if (pOriginalCurve->Type() == TYPE_Line) {
		// we need to test if the circle is between the unbounded lines
		//
		CUnboundedLine originalLine(static_cast<CLine*>(pOriginalCurve));
		CUnboundedLine offsetLine(static_cast<CLine*>(pOffsetCurve));

		C3Point ptNearest;
		double at;
		originalLine.PullBack(pArc->Center(), ptNearest, at, dis1);
		offsetLine.PullBack(pArc->Center(), ptNearest, at, dis2);
	}
	else { // is arc
		CEllipArc* pArc1 = static_cast<CEllipArc*>(pOriginalCurve);
		CEllipArc* pArc2 = static_cast<CEllipArc*>(pOffsetCurve);
		double r1 = pArc1->Radius();
		double r2 = pArc2->Radius();

		double dis = PointDistance(pArc->Center(), pArc2->Center());

		if (r1 > r2) {
			dis1 = dis - r2;		// distance to inner circle
			dis2 = r1 - dis;		// distance to outer circle
		}
		else {
			dis1 = dis - r1;		// distance to inner circle
			dis2 = r2 - dis;		// distance to outer circle
		}
	}
	if (icadIsZero(fabs(dis1 + dis2 - offsetDistance)) &&		// located between curves
		dis1 > r && dis2 > r)									// do not touch either curve
		return true;
	else
		return false;
}


// remove circular offset segments that are completely within the offset zone of the previous 
// or next curve segments. Stephen W. Hou, 9/12/2003
//
void RemoveUnqualifiedOffsetArcs(PPath & pOriginalPath,		// IO - The original path
								 double offsetDistance,		// I - The size of offset zone
							     PPath & pOffsetPath)		// IO - The offset path
{
	ASSERT(pOriginalPath->Size() == pOffsetPath->Size());
	int numCurves = pOriginalPath->Size();
	if (numCurves < 2)
		return;

	// remove all degengented offset curve segments from the offset list. Also remove 
	// coresponding curves from the original path so the number of curves on both
	// the paths are the same.
	//
	double curveLength;
	CCurve *pOffsetCurve;
	for (register i = numCurves - 1; i >= 0; i--) {
		pOffsetCurve = pOffsetPath->Curve(i);
		if (pOffsetCurve->Length(curveLength) == SUCCESS && 
			curveLength <= FUZZ_DIST) {
			pOffsetPath->Remove(i);
			pOriginalPath->Remove(i);
		}
	}
	numCurves = pOriginalPath->Size();
	if (numCurves > 1) {

		// find the indices of circular segments that are inside the offset zone of 
		// previous or next segment
		//
		std::vector<int> removedSegmentList;

		CEllipArc* pOffsetArc;
		CCurve *pPrevCurve, *pNextCurve, *pPrevOffset, *pNextOffset;
		for (i = 0; i < numCurves; i++) {
			pOffsetCurve = pOffsetPath->Curve(i);
			if (pOffsetCurve->Type() == TYPE_Elliptical) {
				pOffsetArc = static_cast<CEllipArc*>(pOffsetCurve);
				if (i == 0) {
					if (pOriginalPath->Closed()) {
						pPrevCurve = pOriginalPath->Curve(numCurves - 1);
						pPrevOffset = pOffsetPath->Curve(numCurves - 1);
					}
					else 
						pPrevCurve = pPrevOffset = NULL;

					pNextCurve = pOriginalPath->Curve(i + 1);
					pNextOffset = pOffsetPath->Curve(i + 1);
				}
				else if (i == numCurves - 1) {
					if (pOriginalPath->Closed()) {
						pNextCurve = pOriginalPath->Curve(0);
						pNextOffset = pOffsetPath->Curve(0);
					}
					else 
						pNextCurve = pNextOffset = NULL;

					pPrevCurve = pOriginalPath->Curve(i - 1);
					pPrevOffset = pOffsetPath->Curve(i - 1);
				}
				else {
					pPrevCurve = pOriginalPath->Curve(i - 1);
					pPrevOffset = pOffsetPath->Curve(i - 1);
					pNextCurve = pOriginalPath->Curve(i + 1);
					pNextOffset = pOffsetPath->Curve(i + 1);
				}

				if (IsArcInsideOffsetZone(pOffsetArc, pPrevCurve, pPrevOffset, offsetDistance) ||
					IsArcInsideOffsetZone(pOffsetArc, pNextCurve, pNextOffset, offsetDistance))
					removedSegmentList.push_back(i);
			}
		}
		// remove all unqualified curve segments from both the paths
		//
		if (removedSegmentList.size()) {
			int index;
			int size = removedSegmentList.size();
			for (i = size - 1; i >= 0; i--) {
				index = removedSegmentList[i];
				pOriginalPath->Remove(index);
				pOffsetPath->Remove(index);
			}
		}
	}
}


// construct an arc as the offset of a cusp formed by line-arc or arc-arc. Stepen W. Hou
//
PCurve ConstructCuspOffset(PCurve pThisCurve, PCurve pNextCurve, double offsetDistance)
{
	PCurve pOffset = NULL;
	if (pThisCurve->Type() == TYPE_Line && pNextCurve->Type() == TYPE_Elliptical ||
		pThisCurve->Type() == TYPE_Elliptical && pNextCurve->Type() == TYPE_Line ||
		pThisCurve->Type() == TYPE_Elliptical && pNextCurve->Type() == TYPE_Elliptical) {
		C3Point endTangent, startTangent, startPoint, endPoint;
		pThisCurve->Evaluate(pThisCurve->End(), endPoint, endTangent);
		pNextCurve->Evaluate(pNextCurve->Start(), startPoint, startTangent);
		if (endTangent * startTangent < 0.0) {
			RC rc;
			CEllipArc* pArc = new CEllipArc(endPoint, endTangent, startPoint, fabs(offsetDistance), rc);
			if (rc == SUCCESS) {
				C3Point sPoint, ePoint;
				pArc->GetStartPoint(sPoint);
				pArc->GetEndPoint(ePoint);
				if ((pArc->End() - pArc->Start()) <= PI  && //  origina curves do not intersect at more than common end point
					PointDistance(endPoint, sPoint) < FUZZ_DIST &&  // eliminate tiny segment
					PointDistance(startPoint, ePoint) < FUZZ_DIST)  // eliminate tiny segment
					pOffset = pArc;
				else
					delete pArc;
			}
			else
				delete pArc;
		}
	}
	return pOffset;
}




// find a candidate path - Stepen W. Hou
//
PPath FindCandidatePath(std::list<PCurve> &candidateSegments)
{
	ASSERT(candidateSegments.size());

	PPath pCandidatePath = new CPath;
	pCandidatePath->SetClosed(FALSE);
	if (candidateSegments.size() == 1) {
		pCandidatePath->Add(*candidateSegments.begin());
		candidateSegments.clear();
	}
	else {
		C3Point startPoint, endPoint, firstPoint;
		std::list<PCurve>::iterator theIterator = candidateSegments.begin();
		PCurve pThisSegment = *theIterator;
		pThisSegment->GetStartPoint(firstPoint);
		for (;;) {
			pThisSegment->GetEndPoint(endPoint);
			pCandidatePath->Add(pThisSegment);
			theIterator = candidateSegments.erase(theIterator);
			if (PointDistance(endPoint, firstPoint) < FUZZ_DIST) { // the end of a closed path is found
				pCandidatePath->SetClosed(TRUE);
				break;
			}
			else {
				if (theIterator != candidateSegments.end()) {
					PCurve pNextSegment = *theIterator;
					pNextSegment->GetStartPoint(startPoint);
					if (PointDistance(endPoint, startPoint) < FUZZ_DIST)
						pThisSegment = pNextSegment;
					else {
						// search the array until a candidate segment starting at the end of this 
						// segment is found.
						for (;;) {
							if (++theIterator != candidateSegments.end()) {
								pNextSegment = *theIterator;
								pNextSegment->GetStartPoint(startPoint);
								if (PointDistance(endPoint, startPoint) < FUZZ_DIST) { // the next segment found
									pThisSegment = pNextSegment;
									break;
								}
							}
							else // the end of the candidate segment array is reached
								return pCandidatePath;
						}
					}
				}
				else
					break; // the end of the candidate segment array is reached
			}
		}
	}
	return pCandidatePath;
}


// find all valid offset paths. - Stepen W. Hou
//
void FindValidOffsetPaths(std::list<PCurve> &candidateSegments,
						     BOOL isOriginalPathClosed,
					         const C3Point &startPoint,
						     const C3Point &endPoint,
						     std::vector<PPath> &offsetPaths)
{
	while (candidateSegments.size()) {
		PPath pCandidatePath = FindCandidatePath(candidateSegments);

		if (pCandidatePath->Size() > 0) {
			if (pCandidatePath->Closed())
				offsetPaths.push_back(pCandidatePath);
			else { // an open path
				if (!isOriginalPathClosed) { // the original path is an open curve
					C3Point start, end;
					pCandidatePath->EndPoints(start, end);
					if (PointDistance(start, startPoint) < FUZZ_DIST ||
						PointDistance(end, endPoint) < FUZZ_DIST) // first or last part
						offsetPaths.push_back(pCandidatePath);
					else	// an invalid open inner path
						delete pCandidatePath;
				}
				else // the original path is closed
					delete pCandidatePath;      // the path is not a valid offset curve
			}
		}
		else
			delete pCandidatePath;
	}
}


// Test if a part of the segment in the offset zone - Stephen W. Hou
//
bool IsSegmentInOffsetZone(PPath pPath,
						   double disOffset,
						   PCurve pSegmentGeometry,
						   double segmentStart,
						   double segmentEnd,
						   bool isFirstSegment)
{
	// use a point in the neightberhood at one of the segment ends to do test.
	//
	C3Point point;
	pSegmentGeometry->Evaluate((segmentStart + segmentEnd) / 2.0, point);
	double offsetDistance = fabs(disOffset);

	int curveIndex;
	C3Point ptNearest;
	double dist, at;
	PCurve pAtCurve;
	pPath->NearestPoint(point, &ptNearest,&dist, &pAtCurve, &at, &curveIndex);
	if (dist < offsetDistance - FUZZ_GEN)  // note: don't change it to a tighter tolerance
		return true;
	else {
		// Testing the middle point of segment is not sufficient. It is possible the part 
		// of the segment is inside the offset zone so we need to test the end points of 
		// the segment. If one of them is inside the zone, the segment is partially lying 
		// in the zone.

		pSegmentGeometry->Evaluate(segmentStart, point);
		pPath->NearestPoint(point, &ptNearest,&dist, &pAtCurve, &at, &curveIndex);	
		if (dist < offsetDistance - FUZZ_GEN)  // note: don't change it to a tighter tolerance
			return true;
		else {
			pSegmentGeometry->Evaluate(segmentEnd, point);
			pPath->NearestPoint(point, &ptNearest,&dist, &pAtCurve, &at, &curveIndex);	
			if (dist < offsetDistance - FUZZ_GEN)  // note: don't change it to a tighter tolerance
				return true;
			else
				return false;
		}
	}
}


// This function constructs the offset of a polyline that is represented as a Geo Path.
// An error will be returned if pPathIn contains any curve segments that are not
// straight lines or circular arcs.
RC offset_pline_ppath(PPath & pPathIn,  // I: A Geo path that represents a polyline
					  double rDistance, // I: The offset distance
					  C3Point ptPick,   // I: The pick point
					  int mode,         // I: 0 to use rDistance and to use ptPick
					                    //    to pick the side.
										//    1 to ignore rDistance and to use ptPick
										//    as a through point.
					  std::vector<PPath> &offsetPaths) // I: Geo paths that are the offset of pPathIn
	{
	int i;
	RC rc = SUCCESS;
	if (pPathIn == NULL)
		return RTERROR;

	// Preprocess the path to remove degenerate segments
	bool degeneratePath;
	RemoveDegenerateCurves(pPathIn, degeneratePath);

	// If there's nothing left, return appropriate error code.
	if (degeneratePath)
		return CMD_ERR_OFFZEROLEN;


	rc = AlignCurves(pPathIn);
	if (rc != SUCCESS)
		return RTERROR;

	// Make sure that each entity in the path is either a straight line or a circular
	// arc.

	int numCurves = pPathIn->Size();

	if (numCurves <= 0)
		return RTERROR;

	PCurve pCurve;
	CURVETYPE type;
	bool allLinesOrArcs = true;
	for (i = 0; i < numCurves; i++) {   // begin i-loop
		pCurve = pPathIn->Curve(i);
		type = pCurve->Type();
		if ( (type != TYPE_Line) && (type != TYPE_Elliptical) ) {
			allLinesOrArcs = false;
			break;
		}

		if (type == TYPE_Elliptical) {
			CEllipArc * pArc = (CEllipArc *) pCurve;
			double radius;
			if ( !(pArc->IsCircular(radius)) )
				return RTERROR;
		}
	}   //   end i-loop

	if (!allLinesOrArcs)
		return RTERROR;

	// for each curve in pPathIn, offset it and add it to offSetPaths
	// we need to be careful that we offset them consistently.

	C3Point pUp(0,0,1);
	PCurve pOffset = NULL;
	double sign;

	C3Point P,T,N,V, Pi;
	GetUcsZ(N);

	double r, d, ri, di;

	d = 10000.0; // just initialize to a high value
	int iNearest = -1;
	PCurve pCurr, pBest = NULL;

	// recalculate numCurves in case we have reduced the number of curves in the path

	numCurves = pPathIn->Size();

	for (i = 0; i < numCurves; i++) {
		pCurr = pPathIn->Curve(i);
		rc = pCurr->NearestPoint(ptPick, P, ri, di);

		if (rc == SUCCESS && di > FUZZ_DIST) {
			if (d > di) {
				d = di;
				iNearest = i;
				pBest = pCurr;
			}
		}
	}

	if ((iNearest < 0) || (pBest == NULL) )
		return RTERROR;

	rc = pBest->NearestPoint(ptPick, P, r, d);
	if (rc == SUCCESS) {
		if (mode == 1) // thru-point offset
			rDistance = d;

		rc = pBest->Evaluate(r, P, T);
		if (rc == SUCCESS) {
			V = ptPick - P;
			if (Determinant(N, T, V) <= 0.)
				// it is an offset to the right
				sign = -1.0;
			else
				sign = 1.0;
		}
		else
			return RTERROR;
	}
	else
		return RTERROR;


	double sDistance = sign*rDistance;
	C3Point oldMajorAxis;
	C3Point newMajorAxis;

	PPath pPathOut = new CPath;
	std::vector<int> removeList;
	for (i = 0; i < numCurves; i++)
		{   // begin i-loop

 		pCurve = pPathIn->Curve(i);
		if (pCurve)
			{

			// save major axis for some post-processing checking later
			if (pCurve->Type() == TYPE_Elliptical)
				oldMajorAxis = ((CEllipArc *)pCurve)->MajorAxis();

			rc = pCurve->Offset(sDistance, pOffset, &pUp);
			if (
				(rc == SUCCESS) &&
				(pOffset != NULL) &&
				( (pOffset->Type() == TYPE_Line) || (pOffset->Type() == TYPE_Elliptical) )
				)
				{
				if (pOffset->Type() == TYPE_Elliptical) {
					newMajorAxis = ((CEllipArc *)pOffset)->MajorAxis();
					// make sure that oldMajorAxis and newMajorAxis point in the same direction
					double dot = oldMajorAxis*newMajorAxis;
					if (dot <= 0.0) {
						// skip this segment if the offset radius is too big for this arc. 
						removeList.push_back(i);
						continue;
					}
				}

				pPathOut->Add(pOffset);
				}
			else
				{
				return RTERROR;
				}
			}
		else
			{
			return RTERROR;
			}


		}   //   end i-loop

	if (removeList.size()) {
		int num = removeList.size();
		numCurves -= num;
		for (i = num - 1; i >= 0; i--)
			pPathIn->Remove(i);
	}

	// If we have gotten this far, we have successfully offset each curve in the
	// input path, now we need to make adjustments.

	// Let's remove degenerated curves and offset circular arcs that are lying inside the 
	// offset zone of their previous or next segment. When an offset circue is lying
	// inside the offst zone of its neighbors, it does not intersect witht the offset 
	// curves of its neighbor and will be a part of the offset path  - SWH, 9/12/2003
	//
	RemoveUnqualifiedOffsetArcs(pPathIn, rDistance, pPathOut);

	if (!pPathOut->Size())
		return CMD_ERR_OFFZEROLEN;

	// reset the number of curves
	PPath pOffsetPath = new CPath;
	numCurves = pPathOut->Size();

	std::list<int> removedCurveList;

	bool bRemoveThisCurve, bRemoveNextCurve;
	IntType typ = EndStart_t; // all curves have been reparameterized to join in this manner
	bool bLast; // true if this is the last pair of curves to be processed, false otherwise
	for (i = 0; i < numCurves - 1; i++)
	{
		if (pPathIn->Closed() || i < numCurves - 2)
			bLast = false;
		else
			bLast = true;

		PCurve pThisCurve = pPathOut->Curve(i);
		PCurve pNextCurve = pPathOut->Curve(i+1);
		pOffsetPath->Add(pThisCurve);

		if(!areCoincident(pThisCurve,pNextCurve))
		{
			rc = trim_or_extend(typ, bLast, pThisCurve, pNextCurve, bRemoveThisCurve, bRemoveNextCurve);

			if (rc != SUCCESS) {
				// we need to do the following extra step to be consistent with AutoCAD.
				//
				PCurve pVertexOffset = ConstructCuspOffset(pThisCurve, pNextCurve, sDistance);
				if (pVertexOffset) {
					pOffsetPath->Add(pVertexOffset);
					rc = SUCCESS;
				}
				else {
					pOffsetPath->RemoveAll();
					delete pOffsetPath;
					delete pPathOut;
					return CMD_ERR_OFFSHARPCURVE;
				}
			}
			else {
				if (bRemoveThisCurve)
					removedCurveList.push_back(i);
			}
		}
	}
	PCurve pLastCurve = pPathOut->Curve(numCurves - 1);
	pOffsetPath->Add(pLastCurve);
	pPathOut->RemoveAll();
	delete pPathOut;

	// finally if the input path is closed we have one more pair to trim or extend
	if (pPathIn->Closed())
	{
		PCurve pFirstCurve = pOffsetPath->Curve(0);
		bLast = true;

		//Modified Cybage SBD 05/11/2001 DD/MM/YYYY
		//Reason : Fix for Bug No. 77864 from Bugzilla
		if (!areCoincident(pLastCurve, pFirstCurve))
		{
			bool bRemoveLastCurve, bRemoveFirstCurve;
			rc = trim_or_extend(typ, bLast, pLastCurve, pFirstCurve, bRemoveLastCurve, bRemoveFirstCurve);
			if (rc != SUCCESS) {
				// we need to do the following extra step to be consistent with AutoCAD.
				//
				PCurve pVertexOffset = ConstructCuspOffset(pLastCurve, pFirstCurve, sDistance);
				if (pVertexOffset) {
					pOffsetPath->Add(pVertexOffset);
					rc = SUCCESS;
				}
				else {
					delete pOffsetPath;
					return CMD_ERR_OFFSHARPCURVE;
				}
			}
			else {
				if (bRemoveLastCurve)
					removedCurveList.push_back(numCurves - 1);

				if (bRemoveFirstCurve)
					removedCurveList.push_front(0);
			}
		}
		pOffsetPath->SetClosed(pPathIn->Closed());
	}

	// now we need to remove unqualified curves and their coresponsing original curves from 
	// pOffsetPath and pPathIn respectively.
	//
	int size = removedCurveList.size();
	if (size) {
		removedCurveList.reverse();
		int index;
		for (std::list<int>::iterator i = removedCurveList.begin(); i != removedCurveList.end(); ++i) {
			index = *i;
			pOffsetPath->Remove(*i);
			pPathIn->Remove(*i);
		}
		numCurves -= size;
	}

	if (pOffsetPath->Size()) {

		pOffsetPath->WrapUp();
		pPathOut = pOffsetPath;
		numCurves = pPathOut->Size();

		// Once again, let's remove degenerate curves from the path.

		RemoveDegenerateCurves(pPathOut, degeneratePath);

		if (degeneratePath) {
			delete pPathOut;
			rc = CMD_ERR_OFFZEROLEN;
		}
		else { 

			// now we need to test if the offset curve is self-intersected. If so
			// we need to split it at all intersection points and do segment classification
			// again the original pline to remove segments. The remainder will be used
			// to form offset curve(s).  - Stepen W. Hou, 9/1/2003
			//

			// construct a point array to record all possible end points of the curves
			//
			CRealArray* pCurvePoints;
			CCurve *pThisCurve = NULL, *pOtherCurve = NULL;
			std::vector<CRealArray*> ptArray;
			for (register i = 0; i < numCurves; i++) {
				pCurvePoints = new CRealArray;
				pThisCurve = pPathOut->Curve(i);
				pCurvePoints->Add(pThisCurve->Start());
				pCurvePoints->Add(pThisCurve->End());
				ptArray.push_back(pCurvePoints);
			}

			// test curve to find self-intersection and add the intersection points to
			// the point array.
			//
			bool isSelfIntersected = false;
			double tol = 0.0001; // tolerance for intersection
			CRealArray onThisCurve, onOtherCurve;
			for (i = 0; i < numCurves - 1; i++) {
				pThisCurve = pPathOut->Curve(i);
				for (register j = i + 1; j < numCurves; j++) {
					pOtherCurve = pPathOut->Curve(j);
					rc = Intersect(pThisCurve, pOtherCurve, onThisCurve, onOtherCurve, tol);
					if (onThisCurve.Size() > 0) {
						if (j == i + 1) {
							for (register k = 0; k < onThisCurve.Size(); k++) {
								ptArray[i]->AddSort(onThisCurve[k]);  // Note: the value is added to array iff it doesn't exist in the array
								ptArray[j]->AddSort(onOtherCurve[k]);
								if (!icadRealEqual(onThisCurve[k], pThisCurve->End()) ||
									!icadRealEqual(onOtherCurve[k], pOtherCurve->Start()))
									isSelfIntersected = true;
							}
						}
						else if (i == 0 && j == numCurves - 1 && pPathOut->Closed()) {
							for (register k = 0; k < onThisCurve.Size(); k++) {
								ptArray[i]->AddSort(onThisCurve[k]);
								ptArray[j]->AddSort(onOtherCurve[k]);
								if (!icadRealEqual(onThisCurve[k], pThisCurve->Start()) ||
									!icadRealEqual(onOtherCurve[k], pOtherCurve->End()))
									isSelfIntersected = true;
							}
						}
						else {
							isSelfIntersected = true;
							for (register k = 0; k < onThisCurve.Size(); k++) {
								ptArray[i]->AddSort(onThisCurve[k]);
								ptArray[j]->AddSort(onOtherCurve[k]);
							}
						}
					}
				}
			}
			if (isSelfIntersected) {
				// perform segment classification aginst the original curve. If a segment
				// is a candidate, the shortest distance from a neighberhood point of the
				// its end point to the original curve should be greater than the offset
				// distance.
				//
				std::list<PCurve> candidateSegments;
				for (i = 0; i < numCurves; i++) {    // for each curve
					pThisCurve = pPathOut->Curve(i);
					pCurvePoints = ptArray[i];

					// a curve may have more than one intersection points and it is possible
					// the segments on both side of an intersection point need to be kept.
					// In that case, instead of constructing the segment on each side as an
					// individual candidate segment, we will construct them as a single
					// candidate segment because they are corresponding to the same geometry
					// of the origianl curve.
					//
					C3Point nbhPoint, ptOnPath;
					double start, end;
					int n = pCurvePoints->Size() - 1;
					bool candidateSegmentStartPointFound = false;
					for (register j = 0; j < n; j++) {  // for each curve segment

						// use a point in the neightberhood at the start of the segment to do test.
						//
						if (IsSegmentInOffsetZone(pPathIn,				// original path
												  rDistance,			// offset distance
												  pThisCurve,			// segment geoemtry
												  (*pCurvePoints)[j],	// segment start
												  (*pCurvePoints)[j + 1],// segment end
												  (i == 0))) { 
							// the segment is inside the offset zone
							if (candidateSegmentStartPointFound) {  // this segment should not be kept
								// construct curve segment and append it to candidate
								// segment array.
								//
								PCurve pCurve = pThisCurve->Clone();
								pCurve->ResetStart(start);
								pCurve->ResetEnd(end);
								candidateSegments.push_back(pCurve);
								candidateSegmentStartPointFound = false;
							}
						}
						else { // a candidate segment
							if (!candidateSegmentStartPointFound) {
								start = (*pCurvePoints)[j];
								candidateSegmentStartPointFound = true;
							}

							// a possible end point of a candidate segment
							//
							end = (*pCurvePoints)[j + 1];
						}
					}
					// after reaching the end of the segment list of the curve, if the start point of an
					// candidate segment has been found, we need to construct the segment and put it into
					// candidate segment list.
					//
					if (candidateSegmentStartPointFound) {
						PCurve pCurve = pThisCurve->Clone();
						pCurve->ResetStart(start);
						pCurve->ResetEnd(end);
						candidateSegments.push_back(pCurve);
					}
				}
				if (candidateSegments.size() > 0) {
					// construct all valid offset paths
					//
					C3Point startPoint, endPoint;
					if (!pPathIn->Closed()) {
						pPathOut->Curve(0)->GetStartPoint(startPoint);
						pPathOut->Curve(numCurves - 1)->GetEndPoint(endPoint);
					}
					FindValidOffsetPaths(candidateSegments, pPathIn->Closed(),
											startPoint, endPoint, offsetPaths);
					delete pPathOut;
				}
				else {
					delete pPathOut;
					rc = CMD_ERR_OFFSET_FAILURE;
				}
			}
			else 
				offsetPaths.push_back(pPathOut);

			for (i = 0; i < ptArray.size(); i++)
				delete ptArray[i];
			ptArray.clear();
		}
	}
	else {
		delete pOffsetPath;
		rc = CMD_ERR_OFFSET_FAILURE;
	}

	return rc;
}

// copy the salient properties of one polyline onto another
// (if either the input or output entity is not a lw or hw polyline, no action is taken)

void copyProperties(db_handitem * p,  // I:  input db_polyline or db_lwpolyline
					db_handitem *& q) // O: output db_polyline or db_lwpolyline
	{

	if ((!p) || (!q))return;

	// check that they are either both lwpolylines or both hwpolylines
	bool both_lwplines;
	bool both_hwplines;

	if ((p->ret_type() == DB_LWPOLYLINE) &&
		(q->ret_type() == DB_LWPOLYLINE))
		both_lwplines = true;
	else
		both_lwplines = false;

	if ((p->ret_type() == DB_POLYLINE) &&
		(q->ret_type() == DB_POLYLINE))
		both_hwplines = true;
	else
		both_hwplines = false;

	if ((both_lwplines == false) && (both_hwplines == false))
		return;



	// first copyProperties at the db_entity level
	q->set_ltypehip(p->ret_ltypehip());
	q->set_layerhip(p->ret_layerhip());
	q->set_48(((db_entity *)p)->ret_ltscale());

	int color;
	p->get_62(&color);
	q->set_62(color);

	q->set_60(p->ret_invisible());

	q->set_67(p->ret_pspace());

	((db_entity *)q)->set_deleted(((db_entity *)p)->ret_deleted());

	// next copyProperties at the entity with extrusion level
	sds_point extruvec;
	p->get_210(extruvec);
	q->set_210(extruvec);


    // now copyProperties at the polyline level; lightweight or heavyweight as appropriate
	sds_real thickness, elev;
	int npverts, nqverts;

	sds_real constwid;
	sds_real wid;



	int flags;

	int i;



	if (both_lwplines)
		{
		p->get_38(&elev);	/*D.Gavrilov*/// We really should set it too.
		q->set_38(elev);	// (so, we'll satisfied TO-DO comments in geoconvert.cpp)

		p->get_39(&thickness);
		q->set_39(thickness);

		p->get_90(&npverts);
		q->get_90(&nqverts);

		p->get_43(&constwid);
		q->set_43(constwid);

		if (npverts==nqverts)
			{
			for (i = 0; i < npverts; i++)
				{
				p->get_40(&wid, i);
				q->set_40(wid, i);

				p->get_41(&wid, i);
				q->set_41(wid, i);
				}
			}

		p->get_70(&flags);
		q->set_70(flags);


		}
	else if (both_hwplines)
		{
		p->get_39(&thickness);
		q->set_39(thickness);

		((db_polyline *)p)->count_ordinary_vertices(&npverts);
		((db_polyline *)q)->count_ordinary_vertices(&nqverts);

		if (npverts==nqverts)
			{
			db_handitem * pvertex = p->next;
			db_handitem * qvertex = q->next;
			bool isSplineFrameVertex;
			for (i = 0; i <= npverts; i++) // deliberately made it <= so we hit seqend.
				{

				if ((pvertex==NULL) || (qvertex==NULL))
					break;

				if ((qvertex->ret_type() == DB_SEQEND) && (qvertex->ret_type() == DB_SEQEND))
					{
                   // set properties at the entity level (seems kind of heavy-handed)
					qvertex->set_ltypehip(pvertex->ret_ltypehip());
					qvertex->set_layerhip(pvertex->ret_layerhip());
					qvertex->set_48(((db_entity *)pvertex)->ret_ltscale());


					pvertex->get_62(&color);
					qvertex->set_62(color);

					qvertex->set_60(pvertex->ret_invisible());

					qvertex->set_67(pvertex->ret_pspace());

					((db_entity *)qvertex)->set_deleted(((db_entity *)pvertex)->ret_deleted());
					break;
					}


				// To Do:  skip over any spline frame vertices
				pvertex->get_70(&flags);
				if (flags & IC_VERTEX_SPLINEFRAME)
					isSplineFrameVertex = true;
				else
					isSplineFrameVertex = false;
				while (isSplineFrameVertex)
					{
					pvertex = pvertex->next;
					if ((pvertex == NULL) || (pvertex->ret_type() == DB_SEQEND))
						break;
					pvertex->get_70(&flags);

					if (flags & IC_VERTEX_SPLINEFRAME)
						isSplineFrameVertex = true;
					else
						isSplineFrameVertex = false;
					}


				if ((pvertex != NULL) && (pvertex->ret_type() != DB_SEQEND) &&
					(qvertex != NULL) && (qvertex->ret_type() != DB_SEQEND))
					{

                    // set properties at the entity level (seems kind of heavy-handed)
					qvertex->set_ltypehip(pvertex->ret_ltypehip());
					qvertex->set_layerhip(pvertex->ret_layerhip());
					qvertex->set_48(((db_entity *)pvertex)->ret_ltscale());


					pvertex->get_62(&color);
					qvertex->set_62(color);

					qvertex->set_60(pvertex->ret_invisible());

					qvertex->set_67(pvertex->ret_pspace());

					((db_entity *)qvertex)->set_deleted(((db_entity *)pvertex)->ret_deleted());

                    // set properties at the vertex level
					pvertex->get_40(&wid);
					qvertex->set_40(wid);

					pvertex->get_41(&wid);
					qvertex->set_41(wid);

					pvertex->get_70(&flags);
					pvertex->set_70(flags);

					pvertex = pvertex->next;
					qvertex = qvertex->next;
					}

				}
			}
		p->get_70(&flags);
		q->set_70(flags);


		}
	}


// Create the offset to a db_handitem that is a lightweight or heavyweight polyline.
// Represent this offset as a lightweight or heavyweight polyline depending upon plinetype.

int cmd_offset_pline(db_handitem * inputPLINE, // I: polyline to be offset
					 double rDistance, // I: the offset distance
					 sds_point sds_pick, // I: the pick point
					 int mode, // 0 to use rDistance and to use sds_pick to pick the side
					           // 1 to ignore rDistance and to use sds_pick as a through point.
							   // 2 will not be used by this routine and will return an error
					 std::vector<db_handitem*> &outputPLINES)
{
	if (( mode < 0) || (mode > 1))
		return RTERROR;

	// make sure that the db_handitem is indeed a polyline
	if ( (inputPLINE->ret_type() != DB_LWPOLYLINE) &&
		 (inputPLINE->ret_type() != DB_POLYLINE) )
		return RTERROR;


	CDWGToGeo * dwgtogeo = new CDWGToGeo();
	C3Point ptNormal;
	GetUcsZ(ptNormal);
	CGeoToDWG * geotodwg = new CGeoToDWG(SDS_CURDWG, ptNormal);

	stdPPathList pPathList;
	RC rc;

    // Convert the Intellicad polyline to a Geo Path
	rc = dwgtogeo->ExtractEntity(inputPLINE, pPathList);
	delete dwgtogeo;

	if (rc != SUCCESS) {
		if (NULL != geotodwg)
			delete geotodwg;

		return RTERROR;
	}

	if (pPathList.size() != 1) {
		if (NULL != geotodwg)
			delete geotodwg;

		return RTERROR;
	}

	PPath pPathIn = *(pPathList.begin());

	C3Point ptPick(sds_pick[0], sds_pick[1], sds_pick[2]);

	PPath pPathOut = NULL;

	std::vector<PPath> paths;
	rc = offset_pline_ppath(pPathIn,
		                    rDistance,
							ptPick,
							mode,
							paths);

	if (pPathIn)  {
		pPathIn->Purge();
		delete pPathIn;
	}

	if (rc != SUCCESS) {
		delete geotodwg;
		return rc;
	}

	for (register i = 0; i < paths.size(); i++) {
		pPathOut = paths[i];

		// Convert the Geo path back to an IntelliCAD polyline.

		// Subtle point here:  We need to make the closed flag of pPathOut
		// agree with the closed flag of the input polyline, regardless of
		// whether it is really closed in the geometric sense.

		int flags;
		inputPLINE->get_70(&flags);
		pPathOut->SetClosed(flags & IC_PLINE_CLOSED);

		HanditemCollection* pPolyCollection = new HanditemCollection;
		if (inputPLINE->ret_type() == DB_POLYLINE)
			rc = geotodwg->PolylineFromPath(pPathOut, pPolyCollection);
		else if (inputPLINE->ret_type() == DB_LWPOLYLINE)
			rc = geotodwg->LWPolylineFromPath(pPathOut, pPolyCollection);
		else {
			delete geotodwg;
			delete pPolyCollection;

			for (register j = i; j < paths.size(); j++) {
				paths[j]->Purge();
				delete paths[j];
			}
			goto Cleanup;
		}

		if (pPathOut) {
			pPathOut->Purge();
			delete pPathOut;
		}

		db_handitem* outputPLINE = NULL;
		if (rc == SUCCESS) {
			outputPLINE = pPolyCollection->GetNext(NULL);
			pPolyCollection->SetListDestroyHI(outputPLINE, false);
			delete pPolyCollection;
		}
		else {
			for (register j = i + 1; j < paths.size(); j++) {
				paths[j]->Purge();
				delete paths[j];
			}
			delete pPolyCollection;
			goto Cleanup;
		}
		if (outputPLINE) {
			copyProperties(inputPLINE, outputPLINE);

			outputPLINES.push_back(outputPLINE);
		}
	}
	delete geotodwg;

	return RTNORM;

Cleanup:

	delete geotodwg;

	for (register k = 0; k < paths.size(); k++)
		delete paths[k];

	paths.clear();

	return RTERROR;
}


//Modified Cybage SBD 05/11/2001 DD/MM/YYYY [
//Reason : Fix for Bug No. 77864 from Bugzilla
bool areCoincident(PCurve  pC1,PCurve  pC2)
{
	if( (pC1==NULL) || (pC2==NULL))
		return false;

	if( (pC1->Type() == TYPE_Line) && (pC2->Type() == TYPE_Line ) )
	{
		if( ( (CLine *)pC1!=NULL ) && ( (CLine *)pC2!=NULL ) )
		{
			C3Point S1,E1,S2,E2;
			((CLine *)pC1)->GetEnds(S1,E1);
			((CLine *)pC2)->GetEnds(S2,E2);

			if( Collinear(S1,E1,S2,FUZZ_DIST) && Collinear(S1,E1,E2,FUZZ_DIST) )
				return true;
		}
	}
	else if (pC1->Type() == TYPE_Elliptical && pC2->Type() == TYPE_Elliptical) {
		CEllipArc* pArc1 = (CEllipArc*)pC1;
		CEllipArc* pArc2 = (CEllipArc*)pC2;

		double r1, r2;
		if (pArc1 && pArc2 && pArc1->IsCircular(r1) && pArc2->IsCircular(r2)) 
			return (pArc1->Center() == pArc2->Center() && FEQUAL(r1, r2, FUZZ_GEN));
	}
	return false;
}
//Modified Cybage SBD 05/11/2001 DD/MM/YYYY ]
