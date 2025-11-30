/* LINE.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * Supports the line related commands
 *
 */
// THIS FILE HAS BEEN GLOBALIZED!
//** Includes
#include "cmds.h"/*DNT*/
#include "Icad.h" /*DNT*/
#include "IcadView.h"
#include "gvector.h"
#include "transf.h"
#include <vector>
#include "ntype.h"
#include "values.h"
#include "CmdQueryTools.h"
#include "GeoData.h"
#include "gvector.h"
using namespace icl;


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// **********
// GLOBALS
//
extern signed char SDS_OSNAP_TANPER_IGNORE;

bool CMD_COMMAND_LINE;


// *********************
// HELPER FUNCTIONS
//

// WCSA - world coordinate system angle: start at 3 o'clock and has counter-clockwise direction. (ANGBASE is kept in WCSA)
// sds_getangle() returns angle which is got ignore setting of ANGBASE. This useful function 
// is used after sds_getangle() function and returns inputed angle in WCSA (ANGBASE is taken into account) 
static sds_real getWCSAngle(sds_real ang)
{
	sds_real result;
	resbuf rb;
	ic_normang(&ang, NULL);

	sds_real angBase = SDS_getvar(NULL, DB_QANGBASE, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES) 
						== RTNORM ? rb.resval.rreal : 0.0; // in WCA
	
	if(SDS_getvar(NULL, DB_QANGDIR, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES) == RTNORM ? rb.resval.rint : 0)
		result = angBase - (IC_TWOPI - ang);
	else
		result = ang + angBase;

	ic_normang(&result, NULL);
	return result;
}

// WCSA - world coordinate system angle: start at 3 o'clock and has counter-clockwise direction. (ANGBASE is kept in WCSA)
// this function remove ANGBASE value from WCS angle 
// returned value is like returned value sds_getangle() 
static sds_real removeAngBaseFromWCSAngle(sds_real wcsAngle)
{
	sds_real result;
    resbuf rb;
	ic_normang(&wcsAngle, NULL);

	sds_real angBase = SDS_getvar(NULL, DB_QANGBASE, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES) 
							== RTNORM ? rb.resval.rreal : 0.0; // in WCA
	if(SDS_getvar(NULL, DB_QANGDIR, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES) == RTNORM ? rb.resval.rint : 0)
		result = wcsAngle - angBase + IC_TWOPI;
	else
		result = wcsAngle - angBase;
	ic_normang(&result, NULL);
	return result;
}

// *********************
// HELPER FUNCTION
//
static void
cmd_line_tan_tan_angle(sds_real fr4,int fi1,sds_point p7,sds_point p5)
{
	//subroutine which is repeated above for calculating which angle to use for tangency
	//decided not to put this inline
	if(fr4>0.0){
		if(fi1){
			p7[0]=p5[2]+p5[0];
			p7[1]=p5[2]+p5[1];
		}else{
			p7[0]=p5[2]+p5[1];
			p7[1]=p5[2]+p5[0];
		}
	}else{
		if(fi1){
			p7[0]=p5[2]-p5[0];
			p7[1]=p5[2]-p5[1];
		}else{
			p7[0]=p5[2]-p5[1];
			p7[1]=p5[2]-p5[0];
		}
	}
}


// *******************
// HELPER FUNCTION
//


int
cmd_ent_to_2pt(sds_point pickpt,sds_name ent1,sds_point ep1,sds_point ep2,int *type,sds_resbuf *rbent)
{
	//given ent1 as a line,arc,circ,ray,xline,solid,circle,or arc, converts entity's data
	//	into 2-point form (p1-p2, or cc, {rr,sa,ea}.  Sets type1 to be:
	//		0=line
	//		1=arc
	//		2=circle
	//	for plines & solids, f'n returns data for segment CLOSEST to UCS point pickpt
	//NOTE: For rays, p2 is in direction of ray and a distance of approx 10* scrrensize away.
	//		Same for xlines, except both pts are 10*screensize away from definition pt
	//		EP1 & EP2 are returned in native coords and rbent will be resbuf for transforming
	//			entity data into other coord systems (ECS or WCS).

	sds_real fr1,fr2;
	sds_resbuf *rbp1,*rbtemp,rb;
	sds_point p1,p2,p3,p4;
	sds_name etemp;
	long fl1,fl2;
	int fi1,ret=RTNORM;
	struct gr_view *view=SDS_CURGRVW;
	int exactvert;

	if(view==NULL || rbent==NULL)return(RTERROR);
	rbp1=NULL;


	rbent->resval.rpoint[0]=rbent->resval.rpoint[1]=0.0;
	rbent->resval.rpoint[2]=1.0;
	rbent->restype=RT3DPOINT;

	if(rbp1!=NULL){sds_relrb(rbp1);rbp1=NULL;}
	if(NULL==(rbp1=sds_entget(ent1))){ret=RTERROR;goto exit;}
	ic_assoc(rbp1,0);
	if(RTNORM==sds_wcmatch(ic_rbassoc->resval.rstring,"LINE,XLINE,RAY"/*DNT*/)){
		rbent->restype=RTSHORT;
		rbent->resval.rint=0;
		for(rbtemp=rbp1;rbtemp!=NULL;rbtemp=rbtemp->rbnext){
			if(rbtemp->restype==10)ic_ptcpy(ep1,rbtemp->resval.rpoint);
			else if(rbtemp->restype==11)ic_ptcpy(p1,rbtemp->resval.rpoint);
		}
		if(!strsame(ic_rbassoc->resval.rstring,db_hitype2str(DB_LINE))){
			if(SDS_getvar(NULL,DB_QSCREENSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM){ret=RTERROR;goto exit;}
			ep2[0]=ep1[0]+10.0*p1[0]*rb.resval.rreal;
			ep2[1]=ep1[1]+10.0*p1[1]*rb.resval.rreal;
			ep2[2]=ep1[2]+10.0*p1[2]*rb.resval.rreal;
		}else{
			ic_ptcpy(ep2,p1); // had erroneously been ic_encpy
		}
		if(strsame(ic_rbassoc->resval.rstring,db_hitype2str(DB_XLINE))){
			ep2[0]-=10.0*p1[0]*rb.resval.rreal;
			ep2[1]-=10.0*p1[1]*rb.resval.rreal;
			ep2[2]-=10.0*p1[2]*rb.resval.rreal;
		}
		*type=0;
	}else if(strsame(ic_rbassoc->resval.rstring,"ARC"/*DNT*/)||
		strsame(ic_rbassoc->resval.rstring,"CIRCLE"/*DNT*/)){
		if(strsame(ic_rbassoc->resval.rstring,"ARC"/*DNT*/))*type=1;
		else *type=2;
		for(rbtemp=rbp1;rbtemp!=NULL;rbtemp=rbtemp->rbnext){
			if(rbtemp->restype==10)ic_ptcpy(ep1,rbtemp->resval.rpoint);
			else if(rbtemp->restype==40)ep2[0]=rbtemp->resval.rreal;
			else if(rbtemp->restype==50)ep2[1]=rbtemp->resval.rreal;
			else if(rbtemp->restype==51)ep2[2]=rbtemp->resval.rreal;
			else if(rbtemp->restype==210)ic_ptcpy(rbent->resval.rpoint,rbtemp->resval.rpoint);
		}
	}else if(strsame(ic_rbassoc->resval.rstring,"SOLID"/*DNT*/)){
		sds_point p5,p6,p7,p8,p9,screenpt;

		for(rbtemp=rbp1;rbtemp!=NULL;rbtemp=rbtemp->rbnext){
			if(rbtemp->restype==10)ic_ptcpy(p1,rbtemp->resval.rpoint);
			else if(rbtemp->restype==11)ic_ptcpy(p2,rbtemp->resval.rpoint);
			else if(rbtemp->restype==12)ic_ptcpy(p3,rbtemp->resval.rpoint);
			else if(rbtemp->restype==13)ic_ptcpy(p4,rbtemp->resval.rpoint);
			else if(rbtemp->restype==210)ic_ptcpy(rbent->resval.rpoint,rbtemp->resval.rpoint);
		}
		*type=0;
		gr_ucs2rp(p1,p5,view);
		gr_ucs2rp(p2,p6,view);
		gr_ucs2rp(p3,p7,view);
		gr_ucs2rp(p4,p8,view);
		//now, find out which one is the line to use for snap
		gr_ucs2rp(pickpt,screenpt,view);
		ic_ptlndist(screenpt,p5,p6,&fr1,p9);
		ic_ptlndist(screenpt,p5,p7,&fr2,p9);
		if(fr2<fr1){
			fr1=fr2;
			fi1=5;//bits 0 and 2
		}else{
			fi1=3;//bits 0 and 1
		}
		ic_ptlndist(screenpt,p8,p6,&fr2,p9);
		if(fr2<fr1){
			fr1=fr2;
			fi1=10;//bits 1 and 3
		}
		if(!icadRealEqual(p7[0],p8[0]) || !icadRealEqual(p7[1],p8[1])){
			ic_ptlndist(screenpt,p8,p7,&fr2,p9);
			if(fr2<fr1)fi1=12;//bits 2 and 3
		}
		//p1 for snap line will either be p1 or p4
		if(fi1&8)ic_ptcpy(ep1,p4);
		else ic_ptcpy(ep1,p1);
		//p2 for snap line will either be p2 or p3
		if(fi1&4)ic_ptcpy(ep2,p3);
		else ic_ptcpy(ep2,p2);
	}else if(strsame(ic_rbassoc->resval.rstring,"POLYLINE"/*DNT*/)){
		for(rbtemp=rbp1;rbtemp!=NULL;rbtemp=rbtemp->rbnext){
			if(rbtemp->restype==70)fi1=rbtemp->resval.rint;
			else if(rbtemp->restype==210)ic_ptcpy(rbent->resval.rpoint,rbtemp->resval.rpoint);
		}
		if(fi1&(IC_PLINE_3DPLINE | IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH)){
			rbent->restype=RTSHORT;
			rbent->resval.rint=0;
		}
		fl1=cmd_pl_vtx_no(ent1,pickpt,1,&exactvert);
		ic_encpy(etemp,ent1);
		for(fl2=0;fl2<fl1;fl2++){
			if(RTNORM!=(ret=sds_entnext_noxref(etemp,etemp)))goto exit;
		}
		if(rbp1!=NULL){sds_relrb(rbp1);rbp1=NULL;}
		if(NULL==(rbp1=sds_entget(etemp))){ret=RTERROR;goto exit;}
		for(rbtemp=rbp1;rbtemp!=NULL;rbtemp=rbtemp->rbnext){
			if(10==rbtemp->restype)ic_ptcpy(p3,rbtemp->resval.rpoint);
			else if(42==rbtemp->restype)fr1=rbtemp->resval.rreal;
		}
		if(icadRealEqual(fr1,0.0))*type=0;
		else *type=1;
		cmd_pline_entnext(etemp,etemp,fi1);
		if(rbp1!=NULL){sds_relrb(rbp1);rbp1=NULL;}
		if(NULL==(rbp1=sds_entget(etemp))){ret=RTERROR;goto exit;}
		ic_assoc(rbp1,10);
		if(0!=(*type)){
			ic_ptcpy(p4,ic_rbassoc->resval.rpoint);
			ic_bulge2arc(p3,p4,fr1,ep1,&ep2[0],&ep2[1],&ep2[2]);
		}else{
			ic_ptcpy(ep2,ic_rbassoc->resval.rpoint);
			ic_ptcpy(ep1,p3);
		}
	}else if(strsame(ic_rbassoc->resval.rstring,"LWPOLYLINE"/*DNT*/)){
		db_lwpolyline* lwp = reinterpret_cast<db_lwpolyline*>(ent1[0]);
		
		geodata::pline lwpline;
		lwpline.extract(*lwp);

		if (lwpline.m_flags & geodata::pline::eNClosed)
		{
			point p = lwpline.m_pts.front();
			lwpline.m_pts.push_back(p);
			double b = lwpline.m_bulges.front();
			lwpline.m_bulges.push_back(b);
		}

		// determine closest lwpolyline segment to pick point
		double mindist;
		int idist = -1;
		sds_point tmp;
		for (int i = 0, n = lwpline.m_pts.size(); i < n-1; i++)
		{
			double dist;
			point& pt0 = lwpline.m_pts[i];
			point& pt1 = lwpline.m_pts[i+1];
			// determine type of segment
			if (fabs(lwpline.m_bulges[i]) < resnorm())
				// line segment
				ic_segdist(pickpt, pt0, pt1, 0, &dist, tmp);
			else
				// arc segment
				ic_ptsegdist(pickpt, pt0, pt1, lwpline.m_bulges[i], &dist);

			if ((idist < 0) ||
				(dist < mindist))
			{
				mindist = dist;
				idist = i;
			}
		}
		if (idist >=0)
		{
			point& p1 = lwpline.m_pts[idist];
			point& p2 = lwpline.m_pts[idist+1];
			if (eq(lwpline.m_bulges[idist], 0., resnorm()))
			{
				*type = 0;
				ic_ptcpy(ep1, p1);
				ic_ptcpy(ep2, p2);
			}
			else
			{
				*type = 1;
				ic_bulge2arc(p1,p2,lwpline.m_bulges[idist],ep1,&ep2[0],&ep2[1],&ep2[2]);
			}
		}
	}else ret=RTERROR;

	exit:
	if(rbp1!=NULL)sds_relrb(rbp1);
	return(ret);
}




// *******************
// HELPER FUNCTION
//

static int
cmd_line_tan_per(sds_point linept1,sds_point linept2,const unsigned char tan_per1,const unsigned char tan_per2)
{
	//linept1 and linept2 are points used for constructing a line. based upon
	//	bit 0 being PER snap and bit 1 being TAN snap, adjust these points for proper
	//	construction of line.  PER has higher priority.
	//NOTEs:	linept1 and linept2 are UCS point

	//Returns:  RTNORM if successful geometric sol'n found
	//			RTNONE if no solution found
	//			RTERROR otherwise


	sds_resbuf rb,rbent1,rbent2,rbucs;
	sds_real fr1,fr2,fr3,fr4,fr5;
	sds_point p1,p2,p3,p4,p5,p6,p7,p210,screenpt1,screenpt2,lp1,lp2;
	sds_name ss1,ss2,ent1,ent2;
	struct gr_view *view=SDS_CURGRVW;
	int ret=RTNORM,fi1,type1,type2,done=0,reverse_ents=0;
	long fl1,fl2;

	ic_ptcpy(lp1,linept1);
	ic_ptcpy(lp2,linept2);

	if(view==NULL) return(RTERROR);
	rbucs.restype=RTSHORT;
	rbucs.resval.rint=1;

	ss1[0]=ss1[1]=ss2[0]=ss2[1]=0L;

	if(SDS_getvar(NULL,DB_QUCSXDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM){ret=RTERROR;goto exit;}
	ic_ptcpy(p1,rb.resval.rpoint);
	if(SDS_getvar(NULL,DB_QUCSYDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM){ret=RTERROR;goto exit;}
	ic_ptcpy(p2,rb.resval.rpoint);
	p210[0]=(p1[1]*p2[2])-(p2[1]*p1[2]);
	p210[1]=(p1[2]*p2[0])-(p2[2]*p1[0]);
	p210[2]=(p1[0]*p2[1])-(p2[0]*p1[1]);


	if(SDS_getvar(NULL,DB_QVIEWSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM){ret=RTERROR;goto exit;}
	fr1=rb.resval.rreal;
	if(SDS_getvar(NULL,DB_QSCREENSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM){ret=RTERROR;goto exit;}
	fr1/=rb.resval.rpoint[1];
	if(SDS_getvar(NULL,DB_QAPERTURE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM){ret=RTERROR;goto exit;}
	fr1*=rb.resval.rint;
	for(fi1=0;fi1<2;fi1++){
		if(fi1==0){
			gr_ucs2rp(lp1,p1,view);
			ic_ptcpy(screenpt1,p1);
		}else{
			gr_ucs2rp(lp2,p1,view);
			ic_ptcpy(screenpt2,p1);
		}
		p3[0]=p1[0]-fr1;
		p3[1]=p1[1]-fr1;
		p4[0]=p1[0]+fr1;
		p4[1]=p1[1]+fr1;
		p3[2]=p4[2]=0.0;
		gr_rp2ucs(p3,p1,view);
		gr_rp2ucs(p4,p2,view);
		if(0==fi1){
			if((ret=sds_pmtssget(NULL,"C"/*DNT*/,p1,p2,NULL,ss1,0))!=RTNORM){ret=RTNONE;goto exit;}
			sds_sslength(ss1,&fl1);
		}else{
			if((ret=sds_pmtssget(NULL,"C"/*DNT*/,p1,p2,NULL,ss2,0))!=RTNORM){ret=RTNONE;goto exit;}
			sds_sslength(ss2,&fl1);
		}
		if(fl1==0L) {
            ret=RTNONE;
            goto exit;
        }
	}



	for(fl1=0L;RTNORM==sds_ssname(ss1,fl1,ent1);fl1++){
		if(RTNORM!=cmd_ent_to_2pt(lp1,ent1,p1,p2,&type1,&rbent1))continue;
		//I'm not checking if entity's extru parallel to current ucs.  They just need to
		//	be parallel to EACH OTHER, which I'm checking below
		/*if(rbent1.restype==RT3DPOINT && !icadPointEqual(p210,rbent1.resval.rpoint)){
			if(p210[0]==-rbent1.resval.rpoint[0] &&
			   p210[1]==-rbent1.resval.rpoint[1] &&
			   p210[2]==-rbent1.resval.rpoint[2]){
				if(type1==1){
					p3[0]=p2[1];
					p2[1]=p2[2];
					p2[2]=p3[0];
				}
			}else continue;//get next ent in set1
		}*/
		for(fl2=0L;RTNORM==sds_ssname(ss2,fl2,ent2);fl2++){
			if(RTNORM!=cmd_ent_to_2pt(lp2,ent2,p3,p4,&type2,&rbent2))continue;
			/*if(rbent2.restype==RT3DPOINT && !icadPointEqual(p210,rbent2.resval.rpoint)){
				if(p210[0]==-rbent2.resval.rpoint[0] &&
				   p210[1]==-rbent2.resval.rpoint[1] &&
				   p210[2]==-rbent2.resval.rpoint[2]){
					if(type2==2){
						p3[0]=p2[1];
						p2[1]=p2[2];
						p2[2]=p3[0];
					}
				}else continue;//get next ent in set1
			}*/
			//if neither one is a line, check to make sure extrusions are parallel & arcs not concentric
			if(type1!=0 && type2!=0 && rbent1.restype==RT3DPOINT && rbent2.restype==RT3DPOINT){
				if(!icadPointEqual(rbent1.resval.rpoint,rbent2.resval.rpoint)){
					if(!icadRealEqual(rbent1.resval.rpoint[0],-rbent2.resval.rpoint[0]) ||
					   !icadRealEqual(rbent1.resval.rpoint[1],-rbent2.resval.rpoint[1]) ||
					   !icadRealEqual(rbent1.resval.rpoint[2],-rbent2.resval.rpoint[2]))continue;
					if(!icadRealEqual(p1[2],-p3[2]))continue;
					//switch start and end angles on one ent so we're in the same ecs
					fr1=p4[1];
					p4[1]=p4[2];
					p4[2]=fr1;
				}else if(!icadRealEqual(p1[2],p3[2]))continue;
				//
				if(icadPointEqual(p1,p3))continue;
			}
			//at this point, we have the two ents we want to work with, so check for osnaps
			if((tan_per1&1) && (tan_per2&1)){
				//both snaps perpendicular....
				//first, you can't be perpendicular between 2 lines
				if(type1==0 && type2==0) continue;
				if(type1==0){
					//type2 must be an arc or circle
					sds_trans(p1,&rbent1,&rbent2,0,p5);
					sds_trans(p2,&rbent1,&rbent2,0,p6);
					if(!icadRealEqual(p1[2],p3[2]) || !icadRealEqual(p2[2],p3[2]))continue;
					if(-1==ic_ptlndist(p3,p5,p6,&fr1,p7))continue;
					fr1=sds_angle(p3,p7);
					if(type2==1){	//an arc
						ic_normang(&p4[1],&p4[2]);
						ic_normang(&p4[1],&fr1);
						if(fr1>p4[2]){
							fr1-=IC_PI;
							ic_normang(&p4[1],&fr1);
							if(fr1>p4[2])continue;
						}
					}
					sds_trans(p7,&rbent1,&rbucs,0,linept1);
					sds_polar(p3,fr1,p4[0],p7);
					sds_trans(p7,&rbent2,&rbucs,0,linept2);
					done=1;
					goto exit;
				}else if(type2==0){
					//type2==0 & type1!=0
					sds_trans(p3,&rbent2,&rbent1,0,p5);
					sds_trans(p4,&rbent2,&rbent1,0,p6);
					if(!icadRealEqual(p3[2],p1[2]) || !icadRealEqual(p4[2],p1[2]))continue;
					if(-1==ic_ptlndist(p1,p5,p6,&fr1,p7))continue;
					fr1=sds_angle(p1,p7);
					if(type1==1){	//not a circle
						ic_normang(&p2[1],&p2[2]);
						ic_normang(&p2[1],&fr1);
						if(fr1>p2[2]){
							fr1-=IC_PI;
							ic_normang(&p2[1],&fr1);
							if(fr1>p2[2])continue;
						}
					}
					sds_trans(p7,&rbent2,&rbucs,0,linept2);
					sds_polar(p1,fr1,p2[0],p7);
					sds_trans(p7,&rbent1,&rbucs,0,linept1);
					done=1;
					goto exit;
				}else{
					fr1=fr2=sds_angle(p1,p3);//z's are ignored, so ok if they're opposites
					if(type1==1){
						ic_normang(&p2[1],&p2[2]);
						ic_normang(&p2[1],&fr1);
						if(fr1>p2[2]){
							fr1-=IC_PI;
							if(fr1>p2[2]) continue;
						}
					}
					fr2-=IC_PI;
					if(type2==1){
						ic_normang(&p4[1],&p4[2]);
						ic_normang(&p4[1],&fr2);
						if(fr2>p4[2]){
							fr2-=IC_PI;
							if(fr2>p4[2]) continue;
						}
					}
					sds_polar(p1,fr1,p2[0],p7);
					sds_trans(p7,&rbent1,&rbucs,0,linept1);
					sds_polar(p3,fr2,p4[0],p7);
					sds_trans(p7,&rbent1,&rbucs,0,linept2);
					//use rbent1 because we're doing math in that ecs if they're antiparallel
					done=1;
					goto exit;
				}
			}
			if((tan_per2&2) && (tan_per1&1)){
				reverse_ents=1;
				//reverse all the data for the entities
				//types
				fi1=type1;
				type1=type2;
				type2=fi1;
				//point data
				ic_ptcpy(p5,p1);
				ic_ptcpy(p1,p3);
				ic_ptcpy(p3,p5);
				ic_ptcpy(p5,p2);
				ic_ptcpy(p2,p4);
				ic_ptcpy(p4,p5);
				//the selected pts
				ic_ptcpy(p7,lp1);
				ic_ptcpy(lp1,lp2);
				ic_ptcpy(lp2,p7);
				//resbufs
				if(rbent1.restype!=RTSHORT || rbent2.restype!=RTSHORT){
					//only reverse if they're not both WCS ents
					rb.restype=rbent1.restype;
					if(rb.restype==RT3DPOINT)
						ic_ptcpy(rb.resval.rpoint,rbent1.resval.rpoint);
					else
						rb.resval.rint=rbent1.resval.rint;
					rbent1.restype=rbent2.restype;
					if(rbent1.restype==RT3DPOINT)
						ic_ptcpy(rbent1.resval.rpoint,rbent2.resval.rpoint);
					else
						rbent1.resval.rint=rbent2.resval.rint;
					rbent2.restype=rb.restype;
					if(rbent2.restype==RT3DPOINT)
						ic_ptcpy(rbent2.resval.rpoint,rb.resval.rpoint);
					else
						rbent2.resval.rint=rb.resval.rint;
				}
				goto reverse_here;
			}
			if((tan_per1&2) && (tan_per2&1)){
				reverse_here:
				//tangent to ent 1 and per to ent 2
				if(type1==0)continue;	//can't be tangent to linear entity
				//first, get tangency angles on arc toward entity2, call them fr3 & fr4
				if(type2>0){
					//tangent to arc1 and perpendicular to arc2
					sds_trans(lp1,&rbucs,&rbent1,0,p7);
					fr1=sds_angle(p1,p7);
					fr2=sds_angle(p1,p3);
					fr3=cmd_dist2d(p1,p3);
                    //check for valid ang for asin
                    if(!icadRealEqual(fr3,0.0)) {fr3=p2[0]/fr3;}
                    if(fabs(fr3)>=1.0) {fr3/=fabs(fr3);}
                    p7[0]=asin(fr3);
					p7[0]=0.5*IC_PI-p7[0];
					fr3=fr2+p7[0];
					fr4=fr2-p7[0];
				}else{
					//per is to a line
					ic_ptlndist(p1,p3,p4,&fr1,p5);
					fr1=sds_angle(p1,p5);
					fr3=fr1+0.5*IC_PI;
					fr4=fr1-0.5*IC_PI;
				}
				//let fi1 bits show which tangent pts to check, and p5 be closer to the
				//	pt the user picked than p6 if both are valid
				if(type1==2){//a circle
					ic_normang(&fr4,&fr1);
					p7[0]=fr1-fr4;
					ic_normang(&fr1,&fr3);
					if(icadAngleEqual(p7[0],fr3-fr1)){
						//both tangent pts equally far from normal thru center
						sds_polar(p1,fr4,p2[0],p5);//p5 is primary point to try
						sds_polar(p1,fr3,p2[0],p6);//p6 is secondary point to try
						if(sds_distance(p5,lp2)>sds_distance(p6,lp2)){
							//reverse the points
							ic_ptcpy(p7,p5);
							ic_ptcpy(p5,p6);
							ic_ptcpy(p6,p7);
						}
					}else if(p7[0]<fr3-fr1){
						sds_polar(p1,fr4,p2[0],p5);//p5 is primary point to try
						sds_polar(p1,fr3,p2[0],p6);//p6 is secondary point to try
					}else{
						sds_polar(p1,fr3,p2[0],p5);
						sds_polar(p1,fr4,p2[0],p6);
					}
					fi1=3;//set first 2 bits in fi1
				}else if(type1==1){//an arc
					fi1=0;
					ic_normang(&p2[1],&p2[2]);
					//ic_normang(&p2[1],&fr1);
					ic_normang(&p2[1],&fr3);
					ic_normang(&p2[1],&fr4);
					if(fr3<p2[2])fi1|=1;
					if(fr4<p2[2])fi1|=2;
					if(fi1==1){
						sds_polar(p1,fr3,p2[0],p5);
					}else if(fi1==2){
						sds_polar(p1,fr4,p2[0],p6);
					}else{
						p7[0]=(fr3+fr4)/2.0;
						ic_normang(&fr1,&p7[0]);
						if(p7[0]-fr1<0.5*(p2[2]-p2[1])){
							sds_polar(p1,fr4,p2[0],p5);
							sds_polar(p1,fr3,p2[0],p6);
						}else{
							sds_polar(p1,fr3,p2[0],p5);
							sds_polar(p1,fr4,p2[0],p6);
						}
					}
				}
				if((fi1&1) && type2>0){
					//check if perpendicular is on 2nd entity
					fr1=sds_angle(p3,p5);
					if(type2==1){
						ic_normang(&p4[1],&p4[2]);
						ic_normang(&p4[1],&fr1);
						if(fr1>p4[2])fi1&=2;
					}
				}
				if((fi1&2) && type2>0){
					fr2=sds_angle(p3,p6);
					if(type2==1){
						ic_normang(&p4[1],&p4[2]);
						ic_normang(&p4[1],&fr1);
						if(fr1>p4[2])fi1&=1;
					}
				}
				if(fi1&1){
					sds_trans(p5,&rbent1,&rbucs,0,linept1);
					if(type2>0){
						sds_polar(p3,fr1,p4[0],p7);
						sds_trans(p7,&rbent1,&rbucs,0,linept2);
					}else{
						ic_ptlndist(p5,p3,p4,&fr3,p7);
						sds_trans(p7,&rbent2,&rbucs,0,linept2);
					}
					done=1;
				}else if(fi1&2){
					sds_trans(p6,&rbent1,&rbucs,0,linept1);
					if(type2>0){
						sds_polar(p3,fr2,p4[0],p7);
						sds_trans(p7,&rbent1,&rbucs,0,linept2);
					}else{
						ic_ptlndist(p6,p3,p4,&fr3,p7);
						sds_trans(p7,&rbent2,&rbucs,0,linept2);
					}
					done=1;
				}
				if(reverse_ents){
					ic_ptcpy(p7,linept1);
					ic_ptcpy(linept1,linept2);
					ic_ptcpy(linept2,p7);
					reverse_ents=0;
				}
				if(done)goto exit;
			}
			if((tan_per1&2) && (tan_per2&2)){
				//tangent/tangent
				if(type1==0 || 0==type2) continue;
				//assume radius of 1st circle >= that of 2nd
				if(p4[0]>p2[0]){
					reverse_ents=1;
					ic_ptcpy(p7,p1);
					ic_ptcpy(p1,p3);
					ic_ptcpy(p3,p7);
					ic_ptcpy(p7,p2);
					ic_ptcpy(p2,p4);
					ic_ptcpy(p4,p7);
					ic_ptcpy(p7,rbent1.resval.rpoint);
					ic_ptcpy(rbent1.resval.rpoint,rbent2.resval.rpoint);
					ic_ptcpy(rbent2.resval.rpoint,p7);
					ic_ptcpy(p7,lp1);
					ic_ptcpy(lp1,lp2);
					ic_ptcpy(lp2,p7);
					fi1=type1;
					type1=type2;
					type2=fi1;
				}
				//there are 2 different types of tangents - one where the tangent crosses
				//	the line connecting the arc centers, and one where it doesn't.  These
				//	are VERY similar, and we'll have to try both if the type suggested by
				//	the pick pts is not possible.
				p5[0]=p3[0]-p1[0];
				p5[1]=p3[1]-p1[1];
				p5[2]=p3[2]-p1[2];//always 0.0
				sds_trans(lp1,&rbucs,&rbent1,0,p6);
				sds_trans(lp2,&rbucs,&rbent1,0,p7);
				//cross p1-p3 vector with p1-p6 and p1-p7.  if same sign z, we want non-crossover
				fr4=(p5[0]*(p6[1]-p1[1]))-((p6[0]-p1[0])*p5[1]);
				fr5=(p5[0]*(p7[1]-p1[1]))-((p7[0]-p1[0])*p5[1]);
				if(fr4*fr5>0.0)
					fi1=0;//don't try crossover first
				else
					fi1=1;//try crossover tangent first
				//get distance between center points
				fr1=sds_angle(p1,p6);//angle from p1 to pickpt1
				fr2=sds_angle(p2,p7);//angle from p3 to pickpt2
				fr3=sqrt(p5[0]*p5[0]+p5[1]*p5[1]+p5[2]*p5[2]);
				//check for valid ang for asin
                p5[0]=(p2[0]+p4[0])/fr3;
                if(fabs(p5[0])>1.0) {p5[0]/=fabs(p5[0]);}
                p5[0]=IC_PI/2.0-asin(p5[0]);//angle from perpendicular if crossover
				//check for valid ang for asin
                p5[1]=(p2[0]-p4[0])/fr3;
                if(fabs(p5[1])>1.0) {p5[1]/=fabs(p5[1]);}
				p5[1]=IC_PI/2.0-asin(p5[1]);//angle from perp w/0 crossover
				p5[2]=sds_angle(p1,p3);
				//setup p7[0] as angle to use for tangent to primary point selected by user on 2nd ent
				//setup p7[1] as angle to use for tangent to alternate tangent solution pt
				cmd_line_tan_tan_angle(fr4,fi1,p7,p5);
				if(type1==1){
					ic_normang(&p2[1],&p2[2]);
					ic_normang(&p2[1],&p7[0]);
					if(p7[0]>p2[2]){
						//tangency pt not within subtended arc, so see if we can try the other tangent from
						//the same primary side and switch the target side
						if(fi1) fi1=0;
						else fi1=1;
						cmd_line_tan_tan_angle(fr4,fi1,p7,p5);
						ic_normang(&p2[1],&p7[0]);
						if(p7[0]>p2[2]){
							//at this point, both attempts at the side the user selected are not valid,
							//so try the other side of the arc for tangency.  Switch sign on fr4, but don't switch
							//the crossover sign....
							fr4=-fr4;
							cmd_line_tan_tan_angle(fr4,fi1,p7,p5);
							ic_normang(&p2[1],&p7[0]);
							if(p7[0]>p2[2]){
								if(fi1) fi1=0;
								else fi1=1;
								cmd_line_tan_tan_angle(fr4,fi1,p7,p5);
								ic_normang(&p2[1],&p7[0]);
								if(p7[0]>p2[2])continue;
							}
						}
					}
				}
                p5[2]+=IC_PI;
				ic_normang(&p5[2],NULL);
				//check for valid ang for asin
                p5[1]=(p2[0]-p4[0])/fr3;
                if(fabs(p5[1])>1.0) {p5[1]/=fabs(p5[1]);}
				p5[1]=IC_PI/2.0+asin(p5[1]);//angle from perp w/0 crossover
				//locate target point
				if(fr5>0.0){
					if(fi1)p6[0]=p5[2]-p5[0];
					else p6[0]=p5[2]-p5[1];
				}else{
					if(fi1)p6[0]=p5[2]+p5[0];
					else p6[0]=p5[2]+p5[1];
				}
				if(type2==1){
					ic_normang(&p4[1],&p4[2]);
					ic_normang(&p4[1],&p6[0]);
					if(p6[0]>p4[2]){//target not within subtended arc...
						p7[0]=p7[1];//switch to alternate angle on first ent
						if(fr5>0.0){
							if(fi1)p6[0]=p5[2]+p5[1];
							else p6[0]=p5[2]+p5[0];
						}else{
							if(fi1)p6[0]=p5[2]-p5[1];
							else p6[0]=p5[2]-p5[0];
						}
						ic_normang(&p4[1],&p6[0]);
						if(p6[0]>p4[2])continue;
					}
				}
				sds_polar(p1,p7[0],p2[0],p5);
				sds_trans(p5,&rbent1,&rbucs,0,linept1);
				sds_polar(p3,p6[0],p4[0],p5);
				sds_trans(p5,&rbent1,&rbucs,0,linept2);
				if(reverse_ents){
					reverse_ents=0;
					ic_ptcpy(p7,linept1);
					ic_ptcpy(linept1,linept2);
					ic_ptcpy(linept2,p7);
				}
				done=1;
				goto exit;
			}

		}//for sset2
	}//for sset1



	exit:
	if(ss1[0]!=0L)sds_ssfree(ss1);
	if(ss2[0]!=0L)sds_ssfree(ss2);
	if(done)
		return(RTNORM);
	else{
		if(ret!=RTNORM)
			return(ret);
		else	//if ret==RTNORM but we didn't find a geometric solution, return RTNONE
			return(RTNONE);
	}

}

// ******************
// COMMAND FUNCTION
//
short cmd_line(void) 
{
	using namespace icl;
//*** This function draws straight lines of any length.
//*** ENTER as reply to "From point:" - Starts at end of previous Line
//***                                   or Arc.
//*** C as reply to "To point:" - Closes polygon.
//*** U as reply to "To point:" - Undoes segment.
//***
//*** RETURNS: -1 - User cancel.
//***          -2 - RTERROR returned from an ads function.
//***

    if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	struct AutoSetOnOff {
		AutoSetOnOff(){
			CMD_COMMAND_LINE = true;}
		~AutoSetOnOff(){
			CMD_COMMAND_LINE = false;}
	} setOnOff;


    char fs1[IC_ACADBUF];
    struct resbuf *newline=NULL,*elist=NULL,rb,rblast,rbucs,rbwcs;
    sds_point pt1,pt2,firstpt,pt3,pt4,pt210;
    sds_real fr1,bulge=0.0;
    sds_name ename;
    RT ret;
	int numents=0,segflg=0;
	signed char tan_per1;
	sds_point temparray[3]={0,0,0};  // kludge for calls to dragobject

	rbucs.restype=rbwcs.restype=RTSHORT;
	rbucs.resval.rint=1;
	rbwcs.resval.rint=0;

	SDS_getvar(NULL,DB_QUCSXDIR,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	SDS_getvar(NULL,DB_QUCSYDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	//cross the two to get extrusion direction for new line
	ic_crossproduct(rblast.resval.rpoint,rb.resval.rpoint,pt210);

    for(;;) {
		for (;;) {
			SDS_OSNAP_TANPER_IGNORE = -1;
            //*** Pass RSG_OTHER to sds_initget in case user enters C or U.
			SDS_getvar(NULL,DB_QISLASTCMDPT,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
//            if(rblast.resval.rint){
//				if(sds_initget(SDS_RSG_OTHER,"Follow")!=RTNORM) return(-2);
//				ret=internalGetpoint(NULL,"\nENTER to use last point/Follow/<Start of line>: ",pt1);
//			}else{
//				if(sds_initget(SDS_RSG_OTHER,NULL)!=RTNORM) return(-2);
//				ret=internalGetpoint(NULL,"\nStart of line: ",pt1);
//			}
            if(rblast.resval.rint){
				if(sds_initget(0,ResourceString(IDC_LINE_INITGET_FOLLOW_0, "Follow ~_Follow" ))!=RTNORM) return(-2);
				ret=internalGetpoint(NULL,ResourceString(IDC_LINE__NENTER_TO_USE_LAST_1, "\nENTER to use last point/Follow/<Start of line>: " ),pt1);
			}else{
				if(sds_initget(SDS_RSG_NONULL,NULL)!=RTNORM) return(-2);
				ret=internalGetpoint(NULL,ResourceString(IDC_LINE__NSTART_OF_LINE___2, "\nStart of line: " ),pt1);
			}

			if(ret==RTERROR){
                return(-2);
            } else if(ret==RTCAN) {
                return(-1);
            } else if(ret==RTNORM) {
                ic_ptcpy(firstpt,pt1);
				//if user didn't ask for tan or perp, make sure we don't leave the IGNORE flag set
                break;
            } else if(ret==RTNONE || ret==RTKWORD) {
                //*** If ENTER was pressed we need to check for a LASTPOINT.
                SDS_getvar(NULL,DB_QISLASTCMDPT,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
                if(!rblast.resval.rint) {
					cmd_ErrorPrompt(CMD_ERR_NOCONTINUE,0);
                    continue;
                }
                //*** Set pt1=lastpt variable.
                SDS_getvar(NULL,DB_QLASTCMDPT,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				ic_ptcpy(pt1,rblast.resval.rpoint);
                ic_ptcpy(firstpt,pt1);
				if(ret==RTNONE)break;
				//RTKWORD = FOLLOW mode, and we know LASTPT and LASTANGLE are valid
				SDS_getvar(NULL,DB_QLASTCMDANG,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
                if((ret=SDS_dragobject(2,1,firstpt,pt2,rblast.resval.rreal,ResourceString(IDC_LINE__NLENGTH_OF_LINE___3, "\nLength of line: " ),&elist,pt4,NULL))==RTERROR){
				//NOTE: pt4 is a dummy pt - we don't care where the dragged to
					return(-2);
				} else if(ret==RTCAN) {
					return(-1);
				}
				ic_assoc(elist,11);
				ic_ptcpy(pt4,ic_rbassoc->resval.rpoint);
				sds_relrb(elist);elist=NULL;
				sds_trans(pt4,&rbwcs,&rbucs,0,pt2);
				sds_trans(pt1,&rbucs,&rbwcs,0,pt3);
				if((newline=sds_buildlist(RTDXF0,db_hitype2str(DB_LINE),10,pt3,11,pt4,210,pt210,0))==NULL)return(-2);

				//newline->rbnext=elist;
				ret=sds_entmake(newline);
				sds_relrb(newline);newline=elist=NULL;
				if(ret!=RTNORM){
					return(-2);
				}else{
					ic_ptcpy(rblast.resval.rpoint,pt4);//set as is using wcs pt
					rblast.restype=RT3DPOINT;
					SDS_setvar(NULL,DB_QLASTCMDPT,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,1);
					ic_ptcpy(pt1,pt2);
                    numents++;
					break;
				}


                sds_entlast(ename);
                if((elist=sds_entget(ename))!=NULL) {
                    if(ic_assoc(elist,0)!=0) { IC_FREEIT return(-2); }
                    if(strisame("ARC"/*DNT*/,ic_rbassoc->resval.rstring)) segflg=1;
                    else if(strisame("POLYLINE"/*DNT*/,ic_rbassoc->resval.rstring)) {
                        if(ic_assoc(elist,66)==0 && ic_rbassoc->resval.rint==1) {
                            while(sds_entnext_noxref(ename,ename)==RTNORM) {
                                IC_FREEIT
                                if((elist=sds_entget(ename))==NULL) continue;
                                if(ic_assoc(elist,42)==0) bulge=ic_rbassoc->resval.rreal;
                            }
                            if(!icadRealEqual(bulge,0.0)) segflg=1;
                        }
                    }
                    IC_FREEIT
                    if(segflg) {
						//CONTINUATION OF LINE FROM LAST ARC OR CURVED PLINE SEG
						SDS_getvar(NULL,DB_QLASTCMDANG,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
                        if((ret=SDS_dragobject(2,1,firstpt,pt2,rblast.resval.rreal,ResourceString(IDC_LINE__NLENGTH_OF_LINE___3, "\nLength of line: " ),&elist,pt1,NULL))==RTERROR){
							return(-2);
						} else if(ret==RTCAN) {
							return(-1);
						}
						ic_assoc(elist,11);
						ic_ptcpy(pt4,ic_rbassoc->resval.rpoint);
						sds_trans(pt4,&rbwcs,&rbucs,0,pt2);
						if((newline=sds_buildlist(RTDXF0,db_hitype2str(DB_LINE),0))==NULL)return(-2);
						newline->rbnext=elist;
						ret=sds_entmake(newline);
						sds_relrb(newline);newline=elist=NULL;
						if(ret!=RTNORM){
							return(-2);
						}else{
				        	ic_ptcpy(rblast.resval.rpoint,pt2);
    	    				rblast.restype=RT3DPOINT;
							SDS_setvar(NULL,DB_QLASTCMDPT,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
							numents++;
							continue;
						}
                    }
                }
                break;
            } else {
                cmd_ErrorPrompt(CMD_ERR_PT,0);
                continue;
            }
        }
		sds_trans(pt1,&rbucs,&rbwcs,0,pt3);

		//if user wanted tangency or per on 1st point in line sequence...
		/*D.G.*/// ... then we ignore it by zeroing tan/per vars and commenting out the following old code
#if 0
		tan_per1 = SDS_OSNAP_TANPER_IGNORE = 0;
		
#else
		if(SDS_OSNAP_TANPER_IGNORE > 0){
			//get ignored snap settings for 1st point...
            struct resbuf rb;
            SDS_getvar(NULL,DB_QOSMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

            //if (((rb.resval.rint&128) != 0) || ((rb.resval.rint&256) != 0)) {
			//There is no need to check OSMODE above.  If OSMODE was set or if the user typed in TAN or PER, the SDS_OSNAP_TANPERIGNORE is correctly set

                if (SDS_OSNAP_TANPER_IGNORE == 3)
                    tan_per1 = 2;
                else if (SDS_OSNAP_TANPER_IGNORE == 1)
                    tan_per1 = (rb.resval.rint&128) == 128 ? 1 : 0;
                else
                    tan_per1 = SDS_OSNAP_TANPER_IGNORE;

            //} else
            //    tan_per1 = 0;
			//reset to trap for second point
			//SDS_OSNAP_TANPER_IGNORE ^= SDS_OSNAP_TANPER_IGNORE;
			SDS_OSNAP_TANPER_IGNORE = -1;
		}else{
			tan_per1=SDS_OSNAP_TANPER_IGNORE=0;
		}
#endif

        //*** Get the rest of the points.
	    db_handitem* elp = NULL;
        for(;;) {
			//convert pt1 from ucs to wcs
			//*** Pass RSG_OTHER to sds_initget in case user enters C or U.
			//*** Build a parlink for dragging.
			if(elp)			/*D.G.*/// It's not a joke: we may have non-zero elp
				delete elp;	// if we go here from some "continue" statements.
			elp=new db_line(SDS_CURDWG);
			elp->set_210(pt210);
			elp->set_10(pt3);
			elp->set_11(pt3);
			ename[0]=(long)elp;
			ename[1]=(long)SDS_CURDWG;

			SDS_getvar(NULL,DB_QTHICKNESS,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			elp->set_39(rb.resval.rreal);

			memset(pt2,0,3*sizeof(sds_real));

			SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

			if(0!=tan_per1){
				//if tangent or perpendicular is pending selection of pt,
				//	no extra stuff allowed
				if(sds_initget(0,NULL)!=RTNORM) return(-2);
				ret=SDS_dragobject(-6,rb.resval.rint,(sds_real *)ename,
			                    (double *)ename[0],0.0,ResourceString(IDC_LINE__NEND_POINT___4, "\nEnd point: " ),
				                                (struct resbuf **)pt3,pt2,NULL);
			}else if(numents==0){	//can't follow.  if using follow, entity was created above so numents=1
				if(sds_initget(SDS_RSG_OTHER,ResourceString(IDC_LINE_INITGET_ANGLE_LENGT_5, "Angle Length ~_Angle ~_Length" ) )!=RTNORM) return(-2);
				ret=SDS_dragobject(-6,rb.resval.rint,(sds_real *)ename,
			     (double *)ename[0],0.0,ResourceString(IDC_LINE__NANGLE_LENGTH__END_6, "\nAngle/Length/<End point>: " ),
				  (struct resbuf **)pt3,pt2,NULL);
			}else if(numents==1){
               if(sds_initget(SDS_RSG_OTHER+SDS_RSG_NOCANCEL,ResourceString(IDC_LINE_INITGET_ANGLE_LENGT_7, "Angle Length ~ Follow ~ Undo ` ~_Angle ~_Length ~_ ~_Follow ~_ ~_Undo" ))!=RTNORM)
                  return(-2);
				ret=SDS_dragobject(-6,rb.resval.rint,(sds_real *)ename,
			     (double *)ename[0],0.0,ResourceString(IDC_LINE__NANGLE_LENGTH_FOLL_8, "\nAngle/Length/Follow/Undo/<End point>: " ),
				  (struct resbuf **)pt3,pt2,NULL);
			}else{
				if(sds_initget(SDS_RSG_OTHER+SDS_RSG_NOCANCEL,ResourceString(IDC_LINE_INITGET_ANGLE_LENGT_9, "Angle Length ~ Follow ~ Close Undo ` ~_Angle ~_Length ~_ ~_Follow ~_ ~_Close ~_Undo" ))!=RTNORM) return(-2);
				ret=SDS_dragobject(-6,rb.resval.rint,(sds_real *)ename,
			     (double *)ename[0],0.0,ResourceString(IDC_LINE__NANGLE_LENGTH_FOL_10, "\nAngle/Length/Follow/Close/Undo/<End point>: " ),
				  (struct resbuf **)pt3,pt2,NULL);
			}
           	if(elp && ret!=RTNORM) { delete elp; elp=NULL; }


			if(ret==RTERROR || ret==RTCAN) {
                return(ret);
            } else if(ret==RTNORM) {
                //*** Create entity.
				if(tan_per1>0 && SDS_OSNAP_TANPER_IGNORE<=0){
					SDS_OSNAP_TANPER_IGNORE=0;
					if     (1==(tan_per1&3))sds_osnap(pt1,"PER"/*DNT*/,pt1);
					else if(2==(tan_per1&3))sds_osnap(pt1,"TAN"/*DNT*/,pt1);
					else if(3==(tan_per1&3))sds_osnap(pt1,"TAN,PER"/*DNT*/,pt1);
				}else if(tan_per1>0 && SDS_OSNAP_TANPER_IGNORE>0){
					ret=cmd_line_tan_per(pt1,pt2,tan_per1,SDS_OSNAP_TANPER_IGNORE);
					if(ret!=RTNORM && ret!=RTNONE) return(ret);
					if(ret==RTNONE){
						cmd_ErrorPrompt(CMD_ERR_BADGEOMETRY,0);
						ret=RTNORM;
						SDS_OSNAP_TANPER_IGNORE = -1;
						continue;
					}
					ic_ptcpy(firstpt,pt1);
				}
				if(tan_per1>0){
					sds_trans(pt1,&rbucs,&rbwcs,0,pt3);
				}
				sds_trans(pt2,&rbucs,&rbwcs,0,pt4);

				double thick;
				elp->get_39(&thick);
				if(icadRealEqual(thick,0.0)) {
					newline=sds_buildlist(RTDXF0,db_hitype2str(DB_LINE),10,pt3,11,pt4,210,pt210,0);
				} else {
					newline=sds_buildlist(RTDXF0,db_hitype2str(DB_LINE),10,pt3,11,pt4,39,thick,210,pt210,0);
				}
	           	if(elp) { delete elp; elp=NULL; }
                if(sds_entmake(newline)!=RTNORM) {
                    cmd_ErrorPrompt(CMD_ERR_CREATEENT,0);
                } else numents++;
                IC_RELRB(newline);
                //*** Set lastpt variable.
                ic_ptcpy(rblast.resval.rpoint,pt2);
                rblast.restype=RT3DPOINT;
				SDS_setvar(NULL,DB_QLASTCMDPT,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
				// add EBATECH(FUTA) 2001/11/9 -[
				SDS_setvar(NULL,DB_QLASTPOINT,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
				// ]-
                rblast.restype=RTSHORT;
                rblast.resval.rint=1;
				SDS_setvar(NULL,DB_QISLASTCMDPT,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
                //*** Set lastangle variable.
                rblast.restype=RTREAL;
                rblast.resval.rreal=sds_angle(pt1,pt2);
				SDS_setvar(NULL,DB_QLASTCMDANG,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
				SDS_setvar(NULL,DB_QLASTANGLE,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
            } else if(ret==RTNONE) {
                return(0);
            } else if(ret==RTKWORD) {
                if(sds_getinput(fs1)!=RTNORM)
                    return(-2);
				if(strisame(fs1,"DONE"/*DNT*/))
                   return(0);
                else if(strisame(fs1,"CLOSE"/*DNT*/)) {
                    if(numents<2) {
						cmd_ErrorPrompt(CMD_ERR_CANTCLOSE,0);
                        continue;
                    }
                    ic_ptcpy(pt2,firstpt);
                    //*** Create entity.
					rb.restype=rblast.restype=RTSHORT;
					rb.resval.rint=0;
					rblast.resval.rint=1;
					sds_trans(pt2,&rblast,&rb,0,pt4);

                    newline=sds_buildlist(RTDXF0,db_hitype2str(DB_LINE),10,pt3,11,pt4,210,pt210,0);
                    if(sds_entmake(newline)!=RTNORM) {
                        cmd_ErrorPrompt(CMD_ERR_CREATEENT,0);
                    }
                    IC_RELRB(newline);
                    //*** Set lastpt global variable.
                    ic_ptcpy(rblast.resval.rpoint,pt2);
                    rblast.restype=RT3DPOINT;
					SDS_setvar(NULL,DB_QLASTCMDPT,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
					// add EBATECH(FUTA) 2001/11/9 -[
					SDS_setvar(NULL,DB_QLASTPOINT,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
					// ]-
					rblast.restype=RTSHORT;
					rblast.resval.rint=1;
					SDS_setvar(NULL,DB_QISLASTCMDPT,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
                    //*** Set lastangle global variable.
                    rblast.restype=RTREAL;
                    rblast.resval.rreal=sds_angle(pt1,pt2);
					SDS_setvar(NULL,DB_QLASTCMDANG,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
					SDS_setvar(NULL,DB_QLASTANGLE,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
                    return(0);
                } else if(strisame(fs1,"UNDO"/*DNT*/)) {
                    if(numents<1) {
						cmd_ErrorPrompt(CMD_ERR_NOMOREUNDO,0);
                        break;
                    }
                    sds_entlast(ename);
                    if(sds_entdel(ename)!=RTNORM) { IC_FREEIT return(-2); }
					if(elist!=NULL){sds_relrb(elist);elist=NULL;}
					if(1==numents){
						ic_ptcpy(pt1,firstpt);
						sds_trans(pt1,&rbucs,&rbwcs,0,pt3);
						numents=0;
						ic_ptcpy(rblast.resval.rpoint,pt1);
						rblast.restype=RT3DPOINT;
						SDS_setvar(NULL,DB_QLASTCMDPT,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
						SDS_setvar(NULL,DB_QLASTPOINT,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
						continue;
					}
                    sds_entlast(ename);
                    if((elist=sds_entget(ename))==NULL)return(-2);
					if(ic_assoc(elist,10)!=0) { IC_FREEIT return(-2); }
					ic_ptcpy(pt1,ic_rbassoc->resval.rpoint);
					sds_trans(pt1,&rbucs,&rbwcs,0,pt3);
					if(ic_assoc(elist,11)!=0) { IC_FREEIT return(-2); }
					ic_ptcpy(pt2,ic_rbassoc->resval.rpoint);
					sds_trans(pt2,&rbucs,&rbwcs,0,pt4);
					ic_ptcpy(rblast.resval.rpoint,pt2);
					rblast.restype=RT3DPOINT;
					SDS_setvar(NULL,DB_QLASTCMDPT,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
					SDS_setvar(NULL,DB_QLASTPOINT,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
					//*** Set lastangle global variable.
					rblast.restype=RTREAL;
					rblast.resval.rreal=sds_angle(pt1,pt2);
					SDS_setvar(NULL,DB_QLASTCMDANG,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
					SDS_setvar(NULL,DB_QLASTANGLE,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
					if(elist!=NULL){sds_relrb(elist);elist=NULL;}
                    --numents;
					//don't continue - we need to reset pts at bottom of loop
				}
				else if(strisame(fs1, "ANGLE"/*DNT*/) || strisame(fs1, "FOLLOW"/*DNT*/)) 
				{
					if(strisame(fs1, "ANGLE"/*DNT*/)) 
					{
						resbuf rb;
						sds_real wcsAngle = SDS_getvar(NULL,DB_QLASTANGLE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES) == RTNORM 
									? rb.resval.rreal : 0.0;
						char angleBuf[IC_ACADBUF], strBuf[IC_ACADBUF];
						if(sds_angtos(wcsAngle,-1,-1,angleBuf)!=RTNORM) 
							strcpy(angleBuf, "0");
						sprintf(strBuf,ResourceString(IDC_LINE__NANGLE_OF_LINE___15, "\nAngle of line <%s>: "), angleBuf);
						if((ret=sds_getangle(pt1, strBuf, &fr1))==RTERROR)//returned angle does not take ANGBASE 
						{ 
    							return(-2);
						} else if(ret==RTCAN) {
        							return(-1);
						}
						else if(ret == RTNONE) {
							fr1 = removeAngBaseFromWCSAngle(wcsAngle);
						}
					}else{
						SDS_getvar(NULL,DB_QLASTCMDANG,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
                        fr1=rblast.resval.rreal;
					}
					rb.restype=rblast.restype=RTSHORT;
					if((ret=SDS_dragobject(2,1,pt1,pt2,fr1,ResourceString(IDC_LINE__NLENGTH_OF_LINE___3, "\nLength of line: " ),&elist,pt1,NULL))==RTERROR){
						return(-2);
				    } else if(ret==RTCAN) {
						return(-1);
					}
					//dragobject returns wcs pts, so convert back to maintain ucs too
					ic_assoc(elist,11);
					ic_ptcpy(pt4,ic_rbassoc->resval.rpoint);
					sds_relrb(elist);elist=NULL;
					rb.restype=rblast.restype=RTSHORT;
					rb.resval.rint=0;
					rblast.resval.rint=1;
					sds_trans(pt4,&rbwcs,&rbucs,0,pt2);
					//sds_trans(pt1,&rbucs,&rbwcs,0,pt3);
					if((newline=sds_buildlist(RTDXF0,db_hitype2str(DB_LINE),10,pt3,11,pt4,210,pt210,0))==NULL)return(-2);
					//newline->rbnext=elist;
					ret=sds_entmake(newline);
					sds_relrb(newline);newline=elist=NULL;
					if(ret!=RTNORM)return(-2);
                    //*** Set LASTCMDPT,ISLASTCMDPT,LASTCMDANG variables.
					rblast.restype=RTREAL;
	                rblast.resval.rreal = getWCSAngle(fr1);
					SDS_setvar(NULL,DB_QLASTCMDANG,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
					SDS_setvar(NULL,DB_QLASTANGLE,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
                    rblast.restype=RT3DPOINT;
                    ic_ptcpy(rblast.resval.rpoint,pt2);
					SDS_setvar(NULL,DB_QLASTCMDPT,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
					// add EBATECH(FUTA) 2001/11/9 -[
					SDS_setvar(NULL,DB_QLASTPOINT,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
					// ]-
                    rblast.restype=RTSHORT;
                    rblast.resval.rint=1;
					SDS_setvar(NULL,DB_QISLASTCMDPT,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
					numents++;
                } else if(strisame(fs1, "LENGTH"/*DNT*/)) {
                    if(sds_initget(0,NULL)!=RTNORM) {
						return(-2);
                    }
					if((ret=sds_getdist(pt1,ResourceString(IDC_LINE__NLENGTH_OF_LINE___3, "\nLength of line: " ),&fr1))==RTERROR){
						return(-2);
				    } else if(ret==RTCAN) {
						return(-1);
					}
                    sds_point tmp;
                    // Save off pt1 in case user enters an angle instead of dragging one.
                    // Call to SDS_dragobject() changes it in the process of filling elist.
                    tmp[0] = pt1[0]; tmp[1] = pt1[1]; tmp[2] = pt1[2];
					resbuf _rb;
					sds_real wcsAngle = SDS_getvar(NULL,DB_QLASTANGLE,&_rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES) == RTNORM 
								? _rb.resval.rreal : 0.0;
					char angleBuf[IC_ACADBUF], strBuf[IC_ACADBUF];
					if(sds_angtos(wcsAngle,-1,-1,angleBuf)!=RTNORM) 
						strcpy(angleBuf, "0");
					sprintf(strBuf,ResourceString(IDC_LINE__NANGLE_OF_LINE___15, "\nAngle of line <%s>: "), angleBuf);
				
					sds_point retPt;
					if((ret=SDS_dragobject(27,1,pt1,pt2,fr1,strBuf,&elist,retPt/*pt1*/,NULL))==RTERROR)
					{
						return(-2);
				    } else if(ret==SDS_RTKWORD) {
                        // User entered an angle instead of allowing the crosshair position dictate
                        // the angle. So first we'll get what the user input.
                        sds_real angle;
                        if(sds_getinput(fs1)!=RTNORM)
                            return(-1);
                        // Convert it to a real angle.
                        if(RTNORM!=sds_angtof(fs1,-1,&angle)) 
						    return(-1);
                        // Resolve the to point.
                        sds_polar(tmp,angle,fr1,pt2); 
                        // Simplest to reset the elist 11 point and allow following code to 
                        // process as usual.
                        ic_ptcpy(elist->rbnext->resval.rpoint,pt2);
                    } else if(ret==RTCAN) {
						return(-1);
					}
					else if(ret == RTNONE) 
					{	// enter was pressed
						sds_point wcsPt;
						sds_polar(pt1/*tmp*/,wcsAngle,fr1,pt2);  // pt1 in ucs
	                        		sds_trans(pt2,&rbucs,&rbwcs,0,wcsPt);	// convert to wcs
						ic_ptcpy(elist->rbnext->resval.rpoint,wcsPt);
					}
					//dragobject returns wcs pts, so convert back to maintain ucs too
					ic_assoc(elist,11);
					ic_ptcpy(pt4,ic_rbassoc->resval.rpoint);
					sds_relrb(elist);elist=NULL;
					rb.restype=rblast.restype=RTSHORT;
					rb.resval.rint=0;
					rblast.resval.rint=1;
					sds_trans(pt4,&rbwcs,&rbucs,0,pt2);
					//sds_trans(pt1,&rblast,&rb,0,pt3);
					if((newline=sds_buildlist(RTDXF0,db_hitype2str(DB_LINE),10,pt3,11,pt4,210,pt210,0))==NULL)return(-2);
					//newline->rbnext=elist;
					ret=sds_entmake(newline);
					sds_relrb(newline);newline=elist=NULL;
					if(ret!=RTNORM)return(-2);
                    //*** Set lastpt & lastangle global variables.
					rblast.restype=RTREAL;
                    rblast.resval.rreal=sds_angle(pt1,pt2);
					SDS_setvar(NULL,DB_QLASTCMDANG,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
					SDS_setvar(NULL,DB_QLASTANGLE,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
                    rblast.restype=RT3DPOINT;
                    ic_ptcpy(rblast.resval.rpoint,pt2);
					SDS_setvar(NULL,DB_QLASTCMDPT,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
					// add EBATECH(FUTA) 2001/11/9 -[
					SDS_setvar(NULL,DB_QLASTPOINT,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
					// ]-
                    rblast.restype=RTSHORT;
                    rblast.resval.rint=1;
					SDS_setvar(NULL,DB_QISLASTCMDPT,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
					numents++;
                } else {
					cmd_ErrorPrompt(CMD_ERR_PTKWORD,0);
                    continue;
                }
            } else return(ret);

            //*** Set pt1=pt2 for next internalGetpoint.
            ic_ptcpy(pt1,pt2);
			ic_ptcpy(pt3,pt4);
			tan_per1=SDS_OSNAP_TANPER_IGNORE=0;

        }
    }
}
