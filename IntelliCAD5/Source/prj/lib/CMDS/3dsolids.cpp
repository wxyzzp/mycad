/* F:\DEV\PRJ\LIB\CMDS\3DSOLIDS.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */
// THIS FILE HAS BEEN GLOBALIZED!

#include "cmds.h"/*DNT*/
#include "Icad.h"/*DNT*/
#include "IcadApi.h"
#include "Modeler.h"
#include "gvector.h"
using icl::point;
using icl::gvector;
#include "transf.h"
using icl::transf;
using icl::coordinate;
#include "values.h"
using icl::eq;
using icl::ge;
using icl::le;
using icl::lt;
#include "CmdQueryTools.h" //wcs2ucs(), checkSuccess,checkNoFailure
#include <memory>
using std::auto_ptr;
#include <functional>
#include <algorithm>

#if defined(SDS_NEWDB)
	#define gra_displayobject gr_displayobject
	#define gra_view      gr_view
	#define dwg_entlink	  db_handitem
	#define dwg_filelink  db_drawing
	#define gra_rp2ucs    gr_rp2ucs
	#define gra_ucs2rp    gr_ucs2rp
	#define gra_rp2pix    gr_rp2pix
	#define gra_pix2rp    gr_pix2rp
	#define gra_twist4ucs gr_twist4ucs
	#define gra_initview  gr_initview
#endif

// Globals
bool cmd_acad_compatible = TRUE;
extern double SDS_CorrectElevation;
extern bool	  SDS_ElevationChanged;
extern int    SDS_GetZScale;


extern struct cmd_drag_globalpacket cmd_drag_gpak;	//structure def moved to header file...

class release_resbuf
{
public:
	release_resbuf(resbuf*& rb)
		:_rb(rb){}

	~release_resbuf()
	{destroy();}

	void destroy()
	{if (_rb) sds_relrb(_rb), _rb = 0;}

private:
	resbuf*& _rb;
};



//functions producing meshes

/*-------------------------------------------------------------------------*//**
Create pyramid mesh
@param
@return 1 - for success; 0 - for error
*//*--------------------------------------------------------------------------*/
short cmd_pyramid_create
(
int mode,
sds_point bpt1,
sds_point bpt2,
sds_point bpt3,
sds_point bpt4,
sds_point tpt1,
sds_point tpt2,
sds_point tpt3,
sds_point tpt4,
int *undraw
)
{
	//MODE: 0 - Draw pyramid w/4 distinct top and bottom pts, all parameters used
	//		1 - Draw ridge pyramid using 4 base pts and tpt1 & tpt2.  If tpt1 and tpt2
	//				are equal, we get square "egyptian" pyramid, but this is really done
	//				the same way.
	//		2 - Draw tetrahedron using bpt1,bpt2,bpt3, and tpt1.
	//		3 - Draw pyramid w/3 distinct top and bottom pts (don't use bpt4 & tpt4)

	//NOTE:  ALL pts assumed to be ucs.  Plines made in WCS!
	//		 Undraw bits are for clearing grdraw done in cmd_pyramid

	//BugZilla No. 78050; 25-03-2002
	if (cmd_istransparent() == RTERROR) return RTERROR;

	if ((*undraw)&1)	sds_grdraw(bpt1,bpt2,-1,0);
	if ((*undraw)&2)	sds_grdraw(bpt2,bpt3,-1,0);
	if ((*undraw)&4)	sds_grdraw(bpt3,bpt4,-1,0);
	if ((*undraw)&8)	sds_grdraw(bpt4,bpt1,-1,0);
	if ((*undraw)&16)sds_grdraw(bpt3,bpt1,-1,0);
	if ((*undraw)&32)sds_grdraw(tpt1,tpt2,-1,0);
	if ((*undraw)&64)sds_grdraw(tpt2,tpt3,-1,0);
	if ((*undraw)&128)sds_grdraw(bpt1,tpt1,-1,0);
	if ((*undraw)&256)sds_grdraw(bpt2,tpt2,-1,0);
	if ((*undraw)&512)sds_grdraw(bpt3,tpt3,-1,0);
	if ((*undraw)&1024)sds_grdraw(bpt4,tpt1,-1,0);
	*undraw=0;

    sds_point *ppt;
    struct resbuf *pline=NULL,rbucs,rbwcs;
    RT rc,ret;
	int vtxcnt;

	rbucs.restype=rbwcs.restype=RTSHORT;
	rbucs.resval.rint=1;
	rbwcs.resval.rint=0;


    sds_entmake(NULL);
    //build pline header
	if (mode==0)
       pline=sds_buildlist(RTDXF0,"POLYLINE"/*DNT*/,66,1,10,bpt1,70,16,71,5,72,4,0);
	else if (mode==2)
       pline=sds_buildlist(RTDXF0,"POLYLINE"/*DNT*/,66,1,10,bpt1,70,16,71,5,72,2,0);
	else
       pline=sds_buildlist(RTDXF0,"POLYLINE"/*DNT*/,66,1,10,bpt1,70,16,71,4,72,4,0);
	if (NULL==pline){
		cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
        rc=RTERROR; goto bail;
    }
    if ((ret=sds_entmake(pline))!=RTNORM) {rc=ret;goto bail;}
    IC_RELRB(pline);

    if ((pline=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,
                                10,bpt1,  //dummy point
                                70,64,
                                 0))==NULL) {
        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
        rc=RTERROR; goto bail;
    }
    ic_assoc(pline,10);
	ppt=&ic_rbassoc->resval.rpoint;
	if (mode==0){
		for (vtxcnt=0;vtxcnt<20;vtxcnt++){
			switch(vtxcnt){
				case 0:
				case 1:
				case 12:
				case 16:
				case 17: ic_ptcpy(*ppt,tpt1);break;

				case 2:
				case 3:
				case 15:
				case 18:
				case 19: ic_ptcpy(*ppt,tpt2);break;

				case 4:
				case 5:
				case 8:  ic_ptcpy(*ppt,tpt4);break;

				case 6:
				case 7:
				case 11: ic_ptcpy(*ppt,tpt3);break;

				case 9:  ic_ptcpy(*ppt,bpt4);break;

				case 10: ic_ptcpy(*ppt,bpt3);break;

				case 13: ic_ptcpy(*ppt,bpt1);break;

				case 14: ic_ptcpy(*ppt,bpt2);break;
			}
			sds_trans((*ppt),&rbucs,&rbwcs,0,(*ppt));
			if ((ret=sds_entmake(pline))!=RTNORM)goto bail;
		}
	}else if (mode==1){
		for (vtxcnt=0;vtxcnt<16;vtxcnt++){
			switch(vtxcnt){
				case 0:
				case 1:
				case 4:
				case 8:
				case 12:
				case 13: ic_ptcpy(*ppt,tpt1);break;

				case 2:
				case 3:
				case 7:
				case 11:
				case 14:
				case 15: ic_ptcpy(*ppt,tpt2);break;

				case 5:	 ic_ptcpy(*ppt,bpt4);break;

				case 6:	 ic_ptcpy(*ppt,bpt3);break;

				case 9:	 ic_ptcpy(*ppt,bpt1);break;

				case 10: ic_ptcpy(*ppt,bpt2);break;
			}
			sds_trans((*ppt),&rbucs,&rbwcs,0,(*ppt));
			if ((ret=sds_entmake(pline))!=RTNORM)goto bail;
		}
	}else if (mode==2){
		for (vtxcnt=0;vtxcnt<10;vtxcnt++){
			switch (vtxcnt){
				case 0:
				case 6:
				case 8: ic_ptcpy(*ppt,tpt1);break;

				case 1:
				case 3:
				case 9: ic_ptcpy(*ppt,bpt2);break;

				case 2:
				case 4: ic_ptcpy(*ppt,bpt3);break;

				case 5:
				case 7: ic_ptcpy(*ppt,bpt1);break;
			}
			sds_trans((*ppt),&rbucs,&rbwcs,0,(*ppt));
			if ((ret=sds_entmake(pline))!=RTNORM)goto bail;
		}
	}else if (mode==3){
		for (vtxcnt=0;vtxcnt<16;vtxcnt++){
			switch (vtxcnt){
				case 0:
				case 8:
				case 9:
				case 12: ic_ptcpy(*ppt,bpt3);break;

				case 1:
				case 13: ic_ptcpy(*ppt,bpt1);break;

				case 2:
				case 14: ic_ptcpy(*ppt,tpt1);break;

				case 3:
				case 10:
				case 11:
				case 15: ic_ptcpy(*ppt,tpt3);break;

				case 4:
				case 5:  ic_ptcpy(*ppt,bpt2);break;

				case 6:
				case 7:  ic_ptcpy(*ppt,tpt2);break;
			}
			sds_trans((*ppt),&rbucs,&rbwcs,0,(*ppt));
			if ((ret=sds_entmake(pline))!=RTNORM)goto bail;
		}
	}

    IC_RELRB(pline);

    //build the seqend
    if ((pline=sds_buildlist(RTDXF0,"SEQEND"/*DNT*/,
                                 0))==NULL) {
        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
        rc=RTERROR; goto bail;
    }
    if ((ret=sds_entmake(pline))!=RTNORM) {rc=ret;goto bail;}
    IC_RELRB(pline);

    rc=RTNORM;
bail:
 IC_RELRB(pline);
 return(rc);
}

int cmd_revolve_pl_ptarray
(
sds_point cc,
sds_point extru_axis,
sds_point *ptarray,
int vcnt,
sds_real totalangle,
int ribs,
struct resbuf *vtx2make
)
{

    //This f'n takes an array of vcnt points in *ptarray and arrays them about the axis extru_axis. The "ribs" it creates
    //will cover totalangle radians with ribs ribs.  Calc'd pts will be dumped into resbuf 10 of vtx2make and entmake will be called.
    //Center point cc (in wcs coords) may anywhere along axis of revolution.  1st set of pts is made w/o any revolution.
    //
    //NOTES:    - CALLER MUST MAKE HEADER (PLINE) ENT AND SEQEND MARKER, AND FREE vtx2make
    //          - PTS IN ptarray ARE ASSUMED TO BE IN WCS.  ALL PTS ARE PUT INTO WCS BEFORE ENTMAKE!!!
	//
	//	Returns RTNORM if successful, RTERROR otherwise
	//	If you're not making a full 360 degree revolution, ribs will be incremented for the ending
	//		set of vertices.  Otherwise, it's assumed that the pline is closed to the 1st rib

    struct resbuf rbecs,rbwcs,rbucs;
    int ret,ct1,ct2,marker,nextmark;
    sds_real angcos,angsin;
    sds_point *pp1,pt0,pt1;

	if (vcnt<2 || ribs<2)return(RTERROR);

    if (NULL==extru_axis || NULL==vtx2make || 0!=ic_assoc(vtx2make,10))return(RTERROR);
    pp1=&ic_rbassoc->resval.rpoint;

    angsin=sin(totalangle/ribs);
    angcos=cos(totalangle/ribs);
	//use real_equal, because it must be 2pi, not 0.0
	if (!icadRealEqual(totalangle,IC_TWOPI))ribs++;

    rbecs.restype=RT3DPOINT;
    ic_ptcpy(rbecs.resval.rpoint,extru_axis);
    rbwcs.restype=RTSHORT;
    rbwcs.resval.rint=0;
    rbucs.restype=RTSHORT;
    rbucs.resval.rint=1;

    if (RTNORM!=sds_trans(cc,&rbwcs,&rbecs,0,pt0))return(RTERROR);

    if (ribs*vcnt>1000){
		SDS_ProgressStart();
		marker=0;
    }else{
		marker=-1;
	}

    for (ct1=0;ct1<ribs;ct1++){
	    if (sds_usrbrk()){
			if (marker>=0)SDS_ProgressStop();
		    return(RTCAN);
	    }
        if (1==ct1){
            for (ct2=0;ct2<vcnt;ct2++){
                sds_trans(ptarray[ct2],&rbwcs,&rbecs,0,ptarray[ct2]);
            }
	    }
        if (ct1>0){
		    //*****rotate points around z axis using pt0 as center*****
		    for (ct2=0;ct2<vcnt;ct2++){
				if (marker>=0){
					nextmark=100*((ct1*vcnt)+ct2)/(ribs*vcnt);
					if (nextmark-marker>=5){
						marker=nextmark;
						SDS_ProgressPercent(nextmark);
					}
				}
			    pt1[0]=(ptarray[ct2][0]-pt0[0])*angcos-(ptarray[ct2][1]-pt0[1])*angsin+pt0[0];
			    pt1[1]=(ptarray[ct2][1]-pt0[1])*angcos+(ptarray[ct2][0]-pt0[0])*angsin+pt0[1];
			    ptarray[ct2][0]=pt1[0];
			    ptarray[ct2][1]=pt1[1];
		    }
	    }
	    for (ct2=0;ct2<vcnt;ct2++){
		    if (ct1>0)
                sds_trans(ptarray[ct2],&rbecs,&rbwcs,0,(*pp1));
            else
                ic_ptcpy(*pp1,ptarray[ct2]);
			if ((ret=sds_entmake(vtx2make))!=RTNORM){
				if (marker>=0)SDS_ProgressStop();
				return(ret);
			}
	    }
    }
	if (marker>=0)SDS_ProgressStop();
    return(RTNORM);
}

/*-------------------------------------------------------------------------*//**
Create sphere mesh
@param
@return 1 - for success; 0 - for error
*//*--------------------------------------------------------------------------*/
short cmd_sphere_create
(
sds_point pt0,
sds_real rad,
int longseg,
int latseg,
int domemode
)
{
	//Creates spherical entities.  If domemode==2, a dish, domemode==1 for a dome, and
	//	domemode==0 for an entire sphere.  pt0 is ALWAYS center of spherical object.
	//pt0 assumed to be ucs point.  all pline pts made in wcs coords

    struct resbuf *pline=NULL,setgetrb;
    RT ret;
	int i;
    sds_real angsinlat,angcoslat,fuzz_fac;
    sds_point base,pt2,pt210;
	sds_point *pp=NULL/* ,*pp1 */;

	struct resbuf rbucs,rbwcs;
	rbucs.restype=rbwcs.restype=RTSHORT;
	rbucs.resval.rint=1;
	rbwcs.resval.rint=0;
	ret=RTNORM;

    fuzz_fac=rad*CMD_FUZZ;

    sds_trans(pt0,&rbucs,&rbwcs,0,base);

    if ((ret=SDS_getvar(NULL,DB_QUCSXDIR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM) goto exit;
    ic_ptcpy(pt2,setgetrb.resval.rpoint);
    if ((ret=SDS_getvar(NULL,DB_QUCSYDIR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM) goto exit;
    pt210[0]=(pt2[1]*setgetrb.resval.rpoint[2])-(setgetrb.resval.rpoint[1]*pt2[2]);
    pt210[2]=(pt2[0]*setgetrb.resval.rpoint[1])-(setgetrb.resval.rpoint[0]*pt2[1]);
    pt210[1]=(pt2[2]*setgetrb.resval.rpoint[0])-(setgetrb.resval.rpoint[2]*pt2[0]);

	if (!domemode){
		//each longitude line from pole to pole covers pi radians
		angsinlat=sin(IC_PI/latseg);
		angcoslat=cos(IC_PI/latseg);
	}else{
		//each longitude line covers pi/2 radians
		angsinlat=sin(0.5*IC_PI/latseg);
		angcoslat=cos(0.5*IC_PI/latseg);
		//if a dish, we go the opposite way as a dome
		if (domemode==2)angsinlat=-angsinlat;
		//note: cosine of minus angle is same as for pos
	}
	//our looping for latitude will require a node at both poles, so increment latseg;
    latseg++;

	if (latseg<2 || longseg<2){
		cmd_ErrorPrompt(CMD_ERR_MESH3D,0);
		return(RTERROR);
	}
	if ((pp= new sds_point [latseg] )==NULL){
		cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
		ret=RTERROR;
		goto exit;
	}
	memset(pp,0,sizeof(sds_point)*latseg);
    //clear the entmake
    sds_entmake(NULL);
    if ((pline=sds_buildlist(RTDXF0,"POLYLINE"/*DNT*/,
                                66,1,
                                10,base,		//now in wcs coords
                                70,17,
                                71,longseg,
                                72,latseg,
                                 0))==NULL) {
        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
		ret=RTERROR;
		goto exit;
	}
    if ((ret=sds_entmake(pline))!=RTNORM)goto exit;
    IC_RELRB(pline);
    //set up the vars and bulid the inital list
    if ((pline=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,
                                10,pt0,
                                70,64,
                                 0))==NULL) {
        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
		ret=RTERROR;
		goto exit;
	}
//	ic_assoc(pline,10);
//	pp1=&ic_rbassoc->resval.rpoint;
	//*****make 1st set of longitudinal pts*****
	for (i=0;i<latseg;i++){
		if (i==0){
			ic_ptcpy(pp[0],pt0);
			if (domemode==2)
				pp[0][2]-=rad;	//start from bottom of dish
			else
				pp[0][2]+=rad;
		}else{
			//rotate pt1 around the Y axis
			pp[i][0]=(pp[i-1][0]-pt0[0])*angcoslat-(pp[i-1][2]-pt0[2])*angsinlat+pt0[0];
			if (fuzz_fac>fabs(pp[i][0]))
                pp[i][0]=0.0;
            pp[i][2]=(pp[i-1][2]-pt0[2])*angcoslat+(pp[i-1][0]-pt0[0])*angsinlat+pt0[2];
			if (fuzz_fac>fabs(pp[i][2]))
                pp[i][2]=0.0;
			pp[i][1]=pt0[1];
		}
	}
	for (i=0;i<latseg;i++){
		sds_trans(pp[i],&rbucs,&rbwcs,0,pp[i]);
	}
	if ((ret=cmd_revolve_pl_ptarray(base,pt210,pp,latseg,IC_TWOPI,longseg,pline))!=RTNORM)goto exit;
	if (pp!=NULL){IC_FREE(pp);pp=NULL;}

 	IC_RELRB(pline);
//	if (pp!=NULL){free(pp);pp=NULL;}
    //build the seqend
    if ((pline=sds_buildlist(RTDXF0,"SEQEND"/*DNT*/,
                                 0))==NULL) {
        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
        sds_entmake(NULL);
        return(RTERROR);
    }
    if ((ret=sds_entmake(pline))!=RTNORM) {
        IC_RELRB(pline);sds_entmake(NULL);
        return(ret);
    }
exit:
	if (pp!=NULL){IC_FREE(pp);pp=NULL;}
    IC_RELRB(pline);
    return(ret);
}

/*-------------------------------------------------------------------------*//**
Create torus mesh
@author Alexey Malov
@param
@return 1 - for success; 0 - for error
*//*--------------------------------------------------------------------------*/
short cmd_torus_create
(
sds_point pt0,
sds_real tordia,
sds_real tubedia,
int torseg,
int tubeseg
)
{
	//pt0 assumed ucs, pline created in wcs
    struct resbuf *pline=NULL,rbucs,rbwcs,setgetrb;
    RT ret;
	int i;
    sds_real angsintor,angcostor;
    sds_point base,pt1,pt2,pt210;
	sds_point *pp=NULL,*pp1;

	rbucs.restype=rbwcs.restype=RTSHORT;
	rbucs.resval.rint=1;
	rbwcs.resval.rint=0;

    sds_trans(pt0,&rbucs,&rbwcs,0,base);

	ret=RTNORM;
	angsintor=sin(IC_TWOPI/torseg);
	angcostor=cos(IC_TWOPI/torseg);


	if (tubeseg<2 || torseg<2){
		cmd_ErrorPrompt(CMD_ERR_MESH3D,0);
		return(RTERROR);
	}
	if ((pp= new sds_point [torseg] )==NULL){
		cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
		ret=RTERROR;
		goto exit;
	}
	memset(pp,0,sizeof(sds_point)*torseg);
    //clear the entmake
    sds_entmake(NULL);
    if ((pline=sds_buildlist(RTDXF0,"POLYLINE"/*DNT*/,
                                66,1,
                                10,base,
                                70,49,
                                71,tubeseg,
                                72,torseg,
                                 0))==NULL) {
        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
		ret=RTERROR;
		goto exit;
	}
    if ((ret=sds_entmake(pline))!=RTNORM)goto exit;
    IC_RELRB(pline);
    //set up the vars and bulid the inital list
    if ((pline=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,
                                10,pt0,
                                70,64,
                                 0))==NULL) {
        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
		ret=RTERROR;
		goto exit;
	}
	pp1=&pline->rbnext->resval.rpoint;
	//*****make 1st set of pts at tube cross-section*****
	for (i=0;i<torseg;i++){
		if (i==0){
			//generate center to use and 1st point on edge
			ic_ptcpy(pp[0],pt0);
			if (cmd_acad_compatible){
                pp[0][0]+=tordia/2.0;
            }else{
                pp[0][0]+=((tordia/2.0)+(tubedia/2.0));
			}
            ic_ptcpy(pt1,pp[0]);
			pt1[0]-=tubedia/2.0;
		}else{
			//rotate pt1 around the Y axis
			pp[i][0]=(pp[i-1][0]-pt1[0])*angcostor-(pp[i-1][2]-pt1[2])*angsintor+pt1[0];
			pp[i][2]=(pp[i-1][2]-pt1[2])*angcostor+(pp[i-1][0]-pt1[0])*angsintor+pt1[2];
			pp[i][1]=pt1[1];
		}
	}
    //convert all the pts from ucs to WCS
    for (i=0;i<torseg;i++){
        sds_trans(pp[i],&rbucs,&rbwcs,0,pp[i]);
    }
    if ((ret=SDS_getvar(NULL,DB_QUCSXDIR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM) goto exit;
    ic_ptcpy(pt2,setgetrb.resval.rpoint);
    if ((ret=SDS_getvar(NULL,DB_QUCSYDIR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM) goto exit;
    pt210[0]=(pt2[1]*setgetrb.resval.rpoint[2])-(setgetrb.resval.rpoint[1]*pt2[2]);
    pt210[2]=(pt2[0]*setgetrb.resval.rpoint[1])-(setgetrb.resval.rpoint[0]*pt2[1]);
    pt210[1]=(pt2[2]*setgetrb.resval.rpoint[0])-(setgetrb.resval.rpoint[2]*pt2[0]);

    if ((ret=cmd_revolve_pl_ptarray(base,pt210,pp,torseg,IC_TWOPI,tubeseg,pline))!=RTNORM)goto exit;

	if (pp!=NULL){IC_FREE(pp);pp=NULL;}
 	IC_RELRB(pline);
    //build the seqend
    if ((pline=sds_buildlist(RTDXF0,"SEQEND"/*DNT*/,
                                 0))==NULL) {
        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
        sds_entmake(NULL);
        return(RTERROR);
    }
    if ((ret=sds_entmake(pline))!=RTNORM) {
        IC_RELRB(pline);sds_entmake(NULL);
        return(ret);
    }
	exit:
	if (pp!=NULL){IC_FREE(pp);pp=NULL;}
    IC_RELRB(pline);
    return(ret);
}

/*-------------------------------------------------------------------------*//**
Create elliptical cone mesh
@author Alexey Malov
@param
@return 1 - for success; 0 - for error
*//*--------------------------------------------------------------------------*/
short cmd_cone_create_elp
(
sds_point cent,
sds_point v1,
sds_point v2,
sds_real ratio,
sds_point pthigh,
int nseg
)
{
    struct resbuf *pline=NULL,rbucs,rbwcs;
    RT rc,ret;
	short i;
    sds_real inc,angsin,angcos;
    sds_point pt1;
	double *pd1=NULL;

	rbucs.restype=rbwcs.restype=RTSHORT;
	rbucs.resval.rint=1;
	rbwcs.resval.rint=0;

    sds_entmake(NULL);
    //build pline header
    if ((pline=sds_buildlist(RTDXF0,"POLYLINE"/*DNT*/,
                                66,1,
                                10,cent,
                                70,48,
                                71,2,
                                72,nseg,
                                 0))==NULL) {
        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
        rc=RTERROR; goto bail;
    }
    if ((ret=sds_entmake(pline))!=RTNORM) {rc=ret;goto bail;}
    IC_RELRB(pline);

    inc=IC_TWOPI/nseg;

    //build the base circle
	cmd_pt_add(cent,v1,pt1);
    if ((pline=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,
                                10,pt1,
                                70,64,
                                 0))==NULL) {
        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
        rc=RTERROR; goto bail;
    }
	pd1=&(pline->rbnext->resval.rpoint[0]);

    for (i=0;i<nseg;i++){
		angcos=cos(inc*i);
        angsin=sin(inc*i);
        pd1[0]=v1[0]*angcos+v2[0]*angsin+cent[0];
		pd1[1]=v1[1]*angcos+v2[1]*angsin+cent[1];
		pd1[2]=v1[2]*angcos+v2[2]*angsin+cent[2];
		sds_trans(pd1,&rbucs,&rbwcs,0,pd1);
        if ((ret=sds_entmake(pline))!=RTNORM) {rc=ret;goto bail;}
    }

    //build the top circle with 0 radius
//    ic_ptcpy(pt1,pthigh);
    sds_trans(pthigh,&rbucs,&rbwcs,0,pd1);
    for (i=0;i<nseg;i++){
        if ((ret=sds_entmake(pline))!=RTNORM) {rc=ret;goto bail;}
	}
    IC_RELRB(pline);

    //build the seqend
    if ((pline=sds_buildlist(RTDXF0,"SEQEND"/*DNT*/,0))==NULL) {
	    cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
	    rc=RTERROR; goto bail;
    }
    if ((ret=sds_entmake(pline))!=RTNORM) {rc=ret;goto bail;}
    IC_RELRB(pline);

    rc=RTNORM;
bail:
 IC_RELRB(pline);
 return(rc);
}

/*-------------------------------------------------------------------------*//**
Create cone mesh
@author Alexey Malov
@param
@return 1 - for success; 0 - for error
*//*--------------------------------------------------------------------------*/
short cmd_cone_create
(
sds_point pt0,
sds_real bdia,
sds_real tdia,
sds_point pthigh,
int nseg
)
{
	sds_real high;
    high=pthigh[2];
    //pts assumed in ucs, pline generated in wcs

    struct resbuf *pline=NULL,rbucs,rbwcs;
    RT rc,ret;
	short i;
    sds_real angsin,angcos;
    sds_point pt1,vec;
	double *pd1=NULL;

	rbucs.restype=rbwcs.restype=RTSHORT;
	rbucs.resval.rint=1;
	rbwcs.resval.rint=0;

    sds_entmake(NULL);
    //build pline header
    if ((pline=sds_buildlist(RTDXF0,"POLYLINE"/*DNT*/,
                                66,1,
                                10,pt0,
                                70,48,
                                71,2,
                                72,nseg,
                                 0))==NULL) {
        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
        rc=RTERROR; goto bail;
    }
    if ((ret=sds_entmake(pline))!=RTNORM) {rc=ret;goto bail;}
    IC_RELRB(pline);

    angsin=sin(IC_TWOPI/nseg);
	angcos=cos(IC_TWOPI/nseg);

    //build the base circle
	ic_ptcpy(pt1,pt0);
    if ((pline=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,
                                10,pt1,
                                70,64,
                                 0))==NULL) {
        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
        rc=RTERROR; goto bail;
    }
	pd1=&(pline->rbnext->resval.rpoint[0]);
    //Set a start point.
	vec[0]=1.0;vec[1]=0.0;vec[2]=0.0;
	sds_trans(vec,&rbwcs,&rbucs,1,vec);
    cmd_rpt_mul(vec,bdia/2.0,vec);
    cmd_pt_add(pt1,vec,pt1);

	sds_trans(pt1,&rbucs,&rbwcs,0,pd1);
    if ((ret=sds_entmake(pline))!=RTNORM) {rc=ret;goto bail;}

    for (i=1;i<nseg;i++){
		pd1[0]=(pt1[0]-pt0[0])*angcos-(pt1[1]-pt0[1])*angsin+pt0[0];
		pd1[1]=(pt1[1]-pt0[1])*angcos+(pt1[0]-pt0[0])*angsin+pt0[1];
		pd1[2]=pt1[2];
		pt1[0]=pd1[0];//copy pt1 back
		pt1[1]=pd1[1];
		sds_trans(pd1,&rbucs,&rbwcs,0,pd1);
        if ((ret=sds_entmake(pline))!=RTNORM) {rc=ret;goto bail;}
    }
    //build the top circle
    ic_ptcpy(pt1,pthigh);
	vec[0]=1.0;vec[1]=0.0;vec[2]=0.0;
    sds_trans(vec,&rbwcs,&rbucs,1,vec);
    cmd_rpt_mul(vec,tdia/2.0,vec);
    cmd_pt_add(pt1,vec,pt1);

	sds_trans(pt1,&rbucs,&rbwcs,0,pd1);
    if ((ret=sds_entmake(pline))!=RTNORM) {rc=ret;goto bail;}

    for (i=1;i<nseg;i++){
		if (!icadIsZero(tdia)){
			pd1[0]=(pt1[0]-pt0[0])*angcos-(pt1[1]-pt0[1])*angsin+pt0[0];
			pd1[1]=(pt1[1]-pt0[1])*angcos+(pt1[0]-pt0[0])*angsin+pt0[1];
			pd1[2]=pt1[2];
			//copy pt1 for next iteration
			pt1[0]=pd1[0];
			pt1[1]=pd1[1];
			//transform for entmake
			sds_trans(pd1,&rbucs,&rbwcs,0,pd1);
		}else{
		    sds_trans(pt1,&rbucs,&rbwcs,0,pd1);
        }
        if ((ret=sds_entmake(pline))!=RTNORM) {rc=ret;goto bail;}
    }
	IC_RELRB(pline);
    //build the seqend
    if ((pline=sds_buildlist(RTDXF0,"SEQEND"/*DNT*/,0))==NULL) {
	    cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
	    rc=RTERROR; goto bail;
    }
    if ((ret=sds_entmake(pline))!=RTNORM) {rc=ret;goto bail;}
    IC_RELRB(pline);

    rc=RTNORM;
bail:
 IC_RELRB(pline);
 return(rc);
}

/*-------------------------------------------------------------------------*//**
Create wedge mesh
@author Alexey Malov
@param
@return 1 - for success; 0 - for error
*//*--------------------------------------------------------------------------*/
short cmd_wedge_create
(
sds_point pt1,
sds_point pt2,
sds_real high,
double ang
)
{
	//all pts assumed ucs, entity created in wcs

    struct resbuf *pline=NULL,rbucs,rbwcs;
    RT rc,ret=0;
	double *pd1;
	int vtxcnt;

	rbucs.restype=rbwcs.restype=RTSHORT;
	rbucs.resval.rint=1;
	rbwcs.resval.rint=0;

    sds_entmake(NULL);
    //build pline header
    if ((pline=sds_buildlist(RTDXF0,"POLYLINE"/*DNT*/,
                                66,1,
                                10,pt1,
                                70,16,
                                71,6,
                                72,3,
                                 0))==NULL) {
        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
        rc=RTERROR; goto bail;
    }
    if ((ret=sds_entmake(pline))!=RTNORM) {rc=ret;goto bail;}
    IC_RELRB(pline);

    if ((pline=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,
                                10,pt1,  //dummy point
                                70,64,
                                 0))==NULL) {
        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
        rc=RTERROR; goto bail;
    }
    pd1=&pline->rbnext->resval.rpoint[0];

	for (vtxcnt=0;vtxcnt<18;vtxcnt++){
		//as we step through the 18 vertices we want to create, pick
		//  one of the 8 corners in the appropriate order...
		switch (vtxcnt){
		case 0:
		case 1:
		case 2:
		case 11:
		case 14: pd1[0]=pt2[0];
				 pd1[1]=pt1[1];
				 pd1[2]=pt1[2];
				 break;
		case 3:
		case 17: pd1[0]=pt1[0];
				 pd1[1]=pt1[1];
				 pd1[2]=pt1[2]+high;
				 break;
		case 4:
		case 5:
		case 8:  pd1[0]=pt1[0];
				 pd1[1]=pt1[1];
				 pd1[2]=pt1[2];
				 break;
		case 6:
		case 15:
		case 16: pd1[0]=pt1[0];
				 pd1[1]=pt2[1];
				 pd1[2]=pt1[2]+high;
				 break;
		case 7:  pd1[0]=pt1[0];
				 pd1[1]=pt2[1];
				 pd1[2]=pt1[2];
				 break;
		case 9:
		case 10:
		case 12:
		case 13: pd1[0]=pt2[0];
				 pd1[1]=pt2[1];
				 pd1[2]=pt1[2];
				 break;
		}
		sds_trans(pd1,&rbucs,&rbwcs,0,pd1);
		if ((ret=sds_entmake(pline))!=RTNORM)goto bail;
	}

    IC_RELRB(pline);

    //build the seqend
    if ((pline=sds_buildlist(RTDXF0,"SEQEND"/*DNT*/,
                                 0))==NULL) {
        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
        rc=RTERROR; goto bail;
    }
    if ((ret=sds_entmake(pline))!=RTNORM) {rc=ret;goto bail;}
    rc=RTNORM;
bail:
 IC_RELRB(pline);
 return(rc);
}

/*-------------------------------------------------------------------------*//**
Create elliptical cylinder mesh
@author Alexey Malov
@param
@return 1 - for success; 0 - for error
*//*--------------------------------------------------------------------------*/
short cmd_cyl_create_elp
(
sds_point cent,
sds_point v1,
sds_point v2,
sds_real ratio,
sds_point pthigh,
int nseg
)
{
    struct resbuf *pline=NULL,rbucs,rbwcs,rbxcs;
    RT rc,ret;
	short i;
    sds_real inc,angsin,angcos;
    sds_point pt1,vpt;
	double *pd1=NULL;

	rbucs.restype=rbwcs.restype=RTSHORT;
	rbucs.resval.rint=1;
	rbwcs.resval.rint=0;

    rbxcs.restype=RT3DPOINT;
    cmd_unit_vector(pthigh,cent,vpt);
	sds_trans(vpt,&rbucs,&rbwcs,1,vpt);
    ic_ptcpy(rbxcs.resval.rpoint,vpt);

	sds_trans(cent,&rbucs,&rbxcs,0,cent);
	sds_trans(pthigh,&rbucs,&rbxcs,0,pthigh);
	sds_trans(v1,&rbucs,&rbxcs,1,v1);
	sds_trans(v2,&rbucs,&rbxcs,1,v2);

    sds_entmake(NULL);
    //build pline header
    if ((pline=sds_buildlist(RTDXF0,"POLYLINE"/*DNT*/,
                                66,1,
                                10,cent,
                                70,48,
                                71,2,
                                72,nseg,
                                 0))==NULL) {
        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
        rc=RTERROR; goto bail;
    }
    if ((ret=sds_entmake(pline))!=RTNORM) {rc=ret;goto bail;}
    IC_RELRB(pline);

    inc=IC_TWOPI/nseg;

	cmd_pt_add(cent,v1,pt1);
    if ((pline=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,
                                10,pt1,
                                70,64,
                                 0))==NULL) {
        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
        rc=RTERROR; goto bail;
    }
	pd1=&(pline->rbnext->resval.rpoint[0]);

    //build the base
    for (i=0;i<nseg;i++){
		angcos=cos(inc*i);
        angsin=sin(inc*i);
        pd1[0]=v1[0]*angcos+v2[0]*angsin+cent[0];
		pd1[1]=v1[1]*angcos+v2[1]*angsin+cent[1];
		pd1[2]=v1[2]*angcos+v2[2]*angsin+cent[2];
		sds_trans(pd1,&rbxcs,&rbwcs,0,pd1);
        if ((ret=sds_entmake(pline))!=RTNORM) {rc=ret;goto bail;}
    }

    //build the top
    for (i=0;i<nseg;i++){
		angcos=cos(inc*i);
        angsin=sin(inc*i);
        pd1[0]=v1[0]*angcos+v2[0]*angsin+pthigh[0];
		pd1[1]=v1[1]*angcos+v2[1]*angsin+pthigh[1];
		pd1[2]=v1[2]*angcos+v2[2]*angsin+pthigh[2];
		sds_trans(pd1,&rbxcs,&rbwcs,0,pd1);
        if ((ret=sds_entmake(pline))!=RTNORM) {rc=ret;goto bail;}
    }

    IC_RELRB(pline);
    //build the seqend
    if ((pline=sds_buildlist(RTDXF0,"SEQEND"/*DNT*/,0))==NULL) {
	    cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
	    rc=RTERROR; goto bail;
    }
    if ((ret=sds_entmake(pline))!=RTNORM) {rc=ret;goto bail;}
    IC_RELRB(pline);

    rc=RTNORM;
bail:
 IC_RELRB(pline);
 return(rc);
}

/*-------------------------------------------------------------------------*//**
Create cylinder mesh
@author Alexey Malov
@param
@return 1 - for success; 0 - for error
*//*--------------------------------------------------------------------------*/
short cmd_cyl_create
(
sds_point pt0,
sds_real dia,
sds_point pthigh,
int nseg
)
{
    //pts assumed in ucs, pline generated in wcs
    struct resbuf *pline=NULL,rbucs,rbwcs;
    RT rc,ret;
	int i;
    sds_real angsin,angcos;
    sds_point pt1;
	double *pd1=NULL;

    rbucs.restype=rbwcs.restype=RTSHORT;
	rbucs.resval.rint=1;
	rbwcs.resval.rint=0;

    sds_entmake(NULL);
    //build pline header
    if ((pline=sds_buildlist(RTDXF0,"POLYLINE"/*DNT*/,
                                66,1,
                                10,pt0,
                                70,48,
                                71,2,
                                72,nseg,
                                 0))==NULL) {
        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
        rc=RTERROR; goto bail;
    }
    if ((ret=sds_entmake(pline))!=RTNORM) {rc=ret;goto bail;}
    IC_RELRB(pline);

    angsin=sin(IC_TWOPI/nseg);
	angcos=cos(IC_TWOPI/nseg);

    //build the base circle
	ic_ptcpy(pt1,pt0);
    if ((pline=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,
                                10,pt1,
                                70,64,
                                 0))==NULL) {
        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
        rc=RTERROR; goto bail;
    }
	pd1=&(pline->rbnext->resval.rpoint[0]);

	pt1[1]-=dia/2.0;

    for (i=nseg;i>0;i--){
		pd1[0]=(pt1[0]-pt0[0])*angcos-(pt1[1]-pt0[1])*angsin+pt0[0];
		pd1[1]=(pt1[1]-pt0[1])*angcos+(pt1[0]-pt0[0])*angsin+pt0[1];
		pd1[2]=pt1[2];
		pt1[0]=pd1[0];//copy pt1 back
		pt1[1]=pd1[1];
		sds_trans(pd1,&rbucs,&rbwcs,0,pd1);
        if ((ret=sds_entmake(pline))!=RTNORM) {rc=ret;goto bail;}
    }
    //build the top circle
    ic_ptcpy(pt1,pthigh);
	pt1[1]-=dia/2.0;
    for (i=nseg;i>0;i--){
			pd1[0]=(pt1[0]-pt0[0])*angcos-(pt1[1]-pt0[1])*angsin+pt0[0];
			pd1[1]=(pt1[1]-pt0[1])*angcos+(pt1[0]-pt0[0])*angsin+pt0[1];
			pd1[2]=pt1[2];
			//copy pt1 for next iteration
			pt1[0]=pd1[0];
			pt1[1]=pd1[1];
			//transform for entmake
			sds_trans(pd1,&rbucs,&rbwcs,0,pd1);
        if ((ret=sds_entmake(pline))!=RTNORM) {rc=ret;goto bail;}
    }
	IC_RELRB(pline);
    //build the seqend
    if ((pline=sds_buildlist(RTDXF0,"SEQEND"/*DNT*/,0))==NULL) {
	    cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
	    rc=RTERROR; goto bail;
    }
    if ((ret=sds_entmake(pline))!=RTNORM) {rc=ret;goto bail;}
    IC_RELRB(pline);

    rc=RTNORM;
bail:
 IC_RELRB(pline);
 return(rc);
}

/*-------------------------------------------------------------------------*//**
Create box mesh
@author Alexey Malov
@param
@return 1 - for success; 0 - for error
*//*--------------------------------------------------------------------------*/
short cmd_box_create
(
sds_point pt1,
sds_point pt2,
sds_real high
)
{
    resbuf rbucs, rbwcs;
	rbucs.restype=rbwcs.restype=RTSHORT;
	rbucs.resval.rint=1;
	rbwcs.resval.rint=0;

	double *pd1;
    struct resbuf *pline=NULL;
    RT ret;
	int vtxcnt;

    sds_entmake(NULL);
    //build pline header
    if ((pline=sds_buildlist(RTDXF0,"POLYLINE"/*DNT*/,
                                66,1,
                                10,pt1,
                                70,16,
                                71,6,
                                72,3,
                                 0))==NULL) {
        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
        ret=RTERROR; goto bail;
    }
    if ((ret=sds_entmake(pline))!=RTNORM) {goto bail;}
    IC_RELRB(pline);

    if ((pline=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,
                                10,pt1,  //dummy point
                                70,64,
                                 0))==NULL) {
        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
        ret=RTERROR; goto bail;
    }
    ic_assoc(pline,10);
	pd1=&ic_rbassoc->resval.rpoint[0];
	for (vtxcnt=0;vtxcnt<18;vtxcnt++){
		//as we step through the 18 vertices we want to create, pick
		//  one of the 8 corners in the appropriate order...
		switch (vtxcnt){
		case 0:
		case 14: pd1[0]=pt2[0];
				 pd1[1]=pt1[1];
				 pd1[2]=pt1[2]+high;
				 break;
		case 1:
		case 2:
		case 11: pd1[0]=pt2[0];
				 pd1[1]=pt1[1];
				 pd1[2]=pt1[2];
				 break;
		case 3:
		case 17: pd1[0]=pt1[0];
				 pd1[1]=pt1[1];
				 pd1[2]=pt1[2]+high;
				 break;
		case 4:
		case 5:
		case 8:  pd1[0]=pt1[0];
				 pd1[1]=pt1[1];
				 pd1[2]=pt1[2];
				 break;
		case 6:
		case 15:
		case 16: pd1[0]=pt1[0];
				 pd1[1]=pt2[1];
				 pd1[2]=pt1[2]+high;
				 break;
		case 7:  pd1[0]=pt1[0];
				 pd1[1]=pt2[1];
				 pd1[2]=pt1[2];
				 break;
		case 9:
		case 12:
		case 13: pd1[0]=pt2[0];
				 pd1[1]=pt2[1];
				 pd1[2]=pt1[2]+high;
				 break;
		case 10: pd1[0]=pt2[0];
				 pd1[1]=pt2[1];
				 pd1[2]=pt1[2];
				 break;
		}
		sds_trans(pd1,&rbucs,&rbwcs,0,pd1);
		if ((ret=sds_entmake(pline))!=RTNORM)goto bail;
	}

   IC_RELRB(pline);

    //build the seqend
    if ((pline=sds_buildlist(RTDXF0,"SEQEND"/*DNT*/,
                                 0))==NULL) {
        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
        ret=RTERROR; goto bail;
    }
    if ((ret=sds_entmake(pline))!=RTNORM) {goto bail;}

    ret=RTNORM;
bail:
    IC_RELRB(pline);
    return(ret);
}

//===============================================================================================
// Commands producing meshes

/*-------------------------------------------------------------------------*//**
Implementation of command 3D
@return RTNORM - for success; RTERROR - for error
*//*--------------------------------------------------------------------------*/
short cmd_3d(void)
{
    if (cmd_iswindowopen() == RTERROR) return RTERROR;
	if (cmd_isinpspace() == RTERROR) return RTERROR;
	//BugZilla No. 78050; 25-03-2002
	if (cmd_istransparent() == RTERROR) return RTERROR;

    char fs1[IC_ACADBUF];
    RT rc,ret=0;

    if (sds_initget(1,ResourceString(IDC_3DSOLIDS_INITGET_WEDGE_103, "Wedge Pyramid Box ~ Cone Cylinder|cyLinder Sphere ~ Dish|DIsh Dome|DOme Torus ~ Mesh ~_Wedge ~_Pyramid ~_Box ~_ ~_Cone ~_cyLinder ~_Sphere ~_ ~_DIsh ~_DOme ~_Torus ~_ ~_Mesh" ) )!=RTNORM) {
        rc=ret;
        goto bail;
    }
    if ((ret=sds_getkword(ResourceString(IDC_3DSOLIDS__NWEDGE_PYRAM_104, "\nWedge/Pyramid/Box/Cone/cyLinder/Sphere/DIsh/DOme/Torus/Mesh: " ) ,fs1))==RTERROR) {
        rc=ret;
        goto bail;
    }else if (ret==RTCAN){
        rc=ret;
        goto bail;
    }

    if (strisame(fs1,"BOX"/*DNT*/ )){
        if ((ret=cmd_box_mesh())!=RTNORM){
            rc=ret;
            goto bail;
        }
    }else if (strisame(fs1,"CONE"/*DNT*/ )){
        if ((ret=cmd_cone_mesh())!=RTNORM){
            rc=ret;
            goto bail;
        }
    }else if (strisame(fs1,"CYLINDER"/*DNT*/ )){
        if ((ret=cmd_cylinder_mesh())!=RTNORM){
            rc=ret;
            goto bail;
        }
    }else if (strisame(fs1,"DISH"/*DNT*/ )){
        if ((ret=cmd_dish_mesh())!=RTNORM){
            rc=ret;
            goto bail;
        }
    }else if (strisame(fs1,"DOME"/*DNT*/ )){
        if ((ret=cmd_dome_mesh())!=RTNORM){
            rc=ret;
            goto bail;
        }
    }else if (strisame(fs1,"MESH"/*DNT*/ )){
        if ((ret=cmd_mesh())!=RTNORM){
            rc=ret;
            goto bail;
        }
    }else if (strisame(fs1,"PYRAMID"/*DNT*/ )){
        if ((ret=cmd_pyramid_mesh())!=RTNORM){
            rc=ret;
            goto bail;
        }
    }else if (strisame(fs1,"SPHERE"/*DNT*/ )){
        if ((ret=cmd_sphere_mesh())!=RTNORM){
            rc=ret;
            goto bail;
        }
    }else if (strisame(fs1,"TORUS"/*DNT*/ )){
        if ((ret=cmd_torus_mesh())!=RTNORM){
            rc=ret;
            goto bail;
        }
    }else if (strisame(fs1,"WEDGE"/*DNT*/ )){
        if ((ret=cmd_wedge3d())!=RTNORM){
            rc=ret;
            goto bail;
        }
    }

    rc=RTNORM;
bail:

    return(rc);
}

/*-------------------------------------------------------------------------*//**
Implementation of AI_PYRAMID
@return RTNORM - for success; others - for error
*//*--------------------------------------------------------------------------*/
short cmd_pyramid_mesh()
{
	//all grdraws done within f'n manually and then cleared by cmd_pyramid_create
	//	prior to entmake.  For details on which bit is between which two points,
	//	see bail at end of function.

    char fs1[IC_ACADBUF];
    RT rc,ret=0;
	int diddraw=0;
	struct resbuf rb;
    sds_point bpt1,bpt2,bpt3,bpt4,tpt1,tpt2,tpt3,tpt4,cpt;

    if (sds_initget(1,NULL)!=RTNORM) {
        rc=ret;
        goto bail;
    }
    if ((ret=sds_getpoint(NULL,ResourceString(IDC_3DSOLIDS__NFIRST_POINT_F_0, "\nFirst point for base of pyramid: " ) ,bpt1))!=RTNORM) {rc=ret; goto bail;}
    if (sds_initget(1,NULL)!=RTNORM) {
        rc=ret;
        goto bail;
    }
    if ((ret=sds_getpoint(bpt1,ResourceString(IDC_3DSOLIDS__NSECOND_POINT__1, "\nSecond point: " ) ,bpt2))!=RTNORM) {rc=ret; goto bail;}
    sds_grdraw(bpt1,bpt2,-1,0);
	diddraw|=1;
    if (sds_initget(1,NULL)!=RTNORM) {
        rc=ret;
        goto bail;
    }
    if ((ret=sds_getpoint(bpt2,ResourceString(IDC_3DSOLIDS__NTHIRD_POINT___2, "\nThird point: " ) ,bpt3))!=RTNORM) {rc=ret; goto bail;}
	sds_grdraw(bpt2,bpt3,-1,0);
	diddraw|=2;
    if (sds_initget(0,ResourceString(IDC_3DSOLIDS_INITGET_TETRAHE_3, "Tetrahedron ~_Tetrahedron" ) )!=RTNORM) {rc=ret;goto bail;}
    SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
   if ((ret=SDS_dragobject(35,rb.resval.rint,bpt1,bpt3,NULL,ResourceString(IDC_3DSOLIDS__NTETRAHEDRON___4, "\nTetrahedron/<Last point for base>: " ) ,NULL,bpt4,NULL))==RTERROR){
    //if ((ret=sds_getpoint(bpt3,"\nTetrahedron/<Last point for base>: ",bpt4))==RTERROR) {
        rc=ret; goto bail;
    }else if (ret==RTCAN){
        rc=ret; goto bail;
    }else if (ret==RTKWORD || ret==RTNONE){
		sds_grdraw(bpt1,bpt3,-1,0);
		diddraw|=16;
        if (sds_initget(1,ResourceString(IDC_3DSOLIDS_INITGET_TOP_SUR_5, "Top_surface|Top ~_Top" ) )!=RTNORM) {
            rc=ret;
            goto bail;
        }
        if ((ret=sds_getpoint(NULL,ResourceString(IDC_3DSOLIDS__NTOP_SURFACE___6, "\nTop surface/<Apex of tetrahedron>: " ) ,tpt1))==RTERROR) {
            rc=ret;
            goto bail;
        }else if (ret==RTCAN){
            rc=ret;
            goto bail;
        }else if (ret==RTKWORD){
            if (sds_initget(1,NULL)!=RTNORM) {
                rc=ret;
                goto bail;
            }
            if ((ret=sds_getpoint(bpt1,ResourceString(IDC_3DSOLIDS__NFIRST_POINT_O_7, "\nFirst point on top surface of tetrahedron: " ) ,tpt1))!=RTNORM) {rc=ret; goto bail;}
			sds_grdraw(bpt1,tpt1,-1,0);
			diddraw|=128;
            if (sds_initget(1,NULL)!=RTNORM) {
                rc=ret;
                goto bail;
            }
           if ((ret=SDS_dragobject(35,rb.resval.rint,tpt1,bpt2,NULL,ResourceString(IDC_3DSOLIDS__NSECOND_POINT__1, "\nSecond point: " ) ,NULL,tpt2,NULL))!=RTNORM) {rc=ret; goto bail;}
            //if ((ret=sds_getpoint(tpt1,"\nSecond point: ",tpt2))!=RTNORM) {rc=ret; goto bail;}
			sds_grdraw(bpt2,tpt2,-1,0);
			diddraw|=256;
			sds_grdraw(tpt1,tpt2,-1,0);
			diddraw|=32;
            if (sds_initget(1,NULL)!=RTNORM) {
                rc=ret;
                goto bail;
            }
           if ((ret=SDS_dragobject(35,rb.resval.rint,tpt1,tpt2,NULL,ResourceString(IDC_3DSOLIDS__NLAST_POINT_OF_8, "\nLast point of top surface: " ) ,NULL,tpt3,NULL))!=RTNORM) {rc=ret; goto bail;}
            //if ((ret=sds_getpoint(tpt2,"\nLast point of top surface: ",tpt3))!=RTNORM) {rc=ret; goto bail;}
            if ((ret=cmd_pyramid_create(3,bpt1,bpt2,bpt3,bpt4,tpt1,tpt2,tpt3,tpt4,&diddraw))!=RTNORM) {rc=ret; goto bail;}
        }else if (ret==RTNORM){
            ic_ptcpy(tpt2,tpt1);
            ic_ptcpy(tpt3,tpt1);
            if ((ret=cmd_pyramid_create(2,bpt1,bpt2,bpt3,bpt4,tpt1,tpt2,tpt3,tpt4,&diddraw))!=RTNORM) {rc=ret; goto bail;}
        }
    }else if (ret==RTNORM){
		sds_grdraw(bpt3,bpt4,-1,0);
		sds_grdraw(bpt4,bpt1,-1,0);
		diddraw|=(4+8);
        cpt[0]=(bpt1[0]+bpt2[0]+bpt3[0]+bpt4[0])/4;
        cpt[1]=(bpt1[1]+bpt2[1]+bpt3[1]+bpt4[1])/4;
        cpt[2]=(bpt1[2]+bpt2[2]+bpt3[2]+bpt4[2])/4;
        if (sds_initget(1,ResourceString(IDC_3DSOLIDS_INITGET_RIDGE_T_9, "Ridge Top_surface|Top ~_Ridge ~_Top" ) )!=RTNORM) {
            rc=ret;
            goto bail;
        }
        if ((ret=sds_getpoint(cpt,ResourceString(IDC_3DSOLIDS__NRIDGE_TOP_SU_10, "\nRidge/Top surface/<Apex of pyramid>: " ) ,tpt1))==RTERROR) {
            rc=ret; goto bail;
        }else if (ret==RTCAN){
            rc=ret; goto bail;
        }else if (ret==RTKWORD){
            sds_getinput(fs1);
            if (strisame(fs1,"RIDGE"/*DNT*/ )){
                if (sds_initget(1,NULL)!=RTNORM) {
                    rc=ret;
                    goto bail;
                }
                //if ((ret=sds_getpoint(NULL,"\nStart of ridge: ",tpt1))!=RTNORM) {rc=ret; goto bail;}
               if ((ret=SDS_dragobject(35,rb.resval.rint,bpt1,bpt4,NULL,ResourceString(IDC_3DSOLIDS__NSTART_OF_RID_12, "\nStart of ridge: " ) ,NULL,tpt1,NULL))!=RTNORM) {rc=ret; goto bail;}
				sds_grdraw(bpt1,tpt1,-1,0);
				diddraw|=128;
				sds_grdraw(bpt4,tpt1,-1,0);
				diddraw|=1024;
                if (sds_initget(1,NULL)!=RTNORM) {
                    rc=ret;
                    goto bail;
                }
                if ((ret=sds_getpoint(tpt1,ResourceString(IDC_3DSOLIDS__NEND_OF_RIDGE_13, "\nEnd of ridge: " ) ,tpt2))!=RTNORM) {rc=ret; goto bail;}
                ic_ptcpy(tpt4,tpt1);
                ic_ptcpy(tpt3,tpt2);
                if ((ret=cmd_pyramid_create(1,bpt1,bpt2,bpt3,bpt4,tpt1,tpt2,tpt3,tpt4,&diddraw))!=RTNORM) {rc=ret; goto bail;}
            }else if (strisame(fs1,"TOP"/*DNT*/ )){
                if (sds_initget(1,NULL)!=RTNORM) {
                    rc=ret;
                    goto bail;
                }
                if ((ret=sds_getpoint(bpt1,ResourceString(IDC_3DSOLIDS__NFIRST_POINT__15, "\nFirst point on top surface of pyramid: " ) ,tpt1))!=RTNORM) {rc=ret; goto bail;}
				sds_grdraw(bpt1,tpt1,-1,0);
				diddraw|=128;
                if (sds_initget(1,NULL)!=RTNORM) {
                    rc=ret;
                    goto bail;
                }
               if ((ret=SDS_dragobject(35,rb.resval.rint,tpt1,bpt2,NULL,ResourceString(IDC_3DSOLIDS__NSECOND_POINT__1, "\nSecond point: " ) ,NULL,tpt2,NULL))!=RTNORM) {rc=ret; goto bail;}
                //if ((ret=sds_getpoint(tpt1,"\nSecond point: ",tpt2))!=RTNORM) {rc=ret; goto bail;}
				sds_grdraw(tpt1,tpt2,-1,0);
				diddraw|=32;
				sds_grdraw(bpt2,tpt2,-1,0);
				diddraw|=256;
                if (sds_initget(1,NULL)!=RTNORM) {
                    rc=ret;
                    goto bail;
                }
               if ((ret=SDS_dragobject(35,rb.resval.rint,tpt2,bpt3,NULL,ResourceString(IDC_3DSOLIDS__NTHIRD_POINT___2, "\nThird point: " ) ,NULL,tpt3,NULL))!=RTNORM) {rc=ret; goto bail;}
                //if ((ret=sds_getpoint(tpt2,"\nThird point: ",tpt3))!=RTNORM) {rc=ret; goto bail;}
				sds_grdraw(tpt2,tpt3,-1,0);
				diddraw|=64;
				sds_grdraw(bpt3,tpt3,-1,0);
				diddraw|=512;
                if (sds_initget(1,NULL)!=RTNORM) {
                    rc=ret;
                    goto bail;
                }
               if ((ret=SDS_dragobject(35,rb.resval.rint,tpt1,tpt3,NULL,ResourceString(IDC_3DSOLIDS__NLAST_POINT_OF_8, "\nLast point of top surface: " ) ,NULL,tpt4,NULL))!=RTNORM){rc=ret; goto bail;}
                //if ((ret=sds_getpoint(tpt3,"\nLast point of top surface: ",tpt4))!=RTNORM) {rc=ret; goto bail;}
                if ((ret=cmd_pyramid_create(0,bpt1,bpt2,bpt3,bpt4,tpt1,tpt2,tpt3,tpt4,&diddraw))!=RTNORM) {rc=ret; goto bail;}
            }
        }else if (ret==RTNORM){
			//don't need last 3 top pts or last base pt
			ic_ptcpy(tpt2,tpt1);
            if ((ret=cmd_pyramid_create(1,bpt1,bpt2,bpt3,bpt4,tpt1,tpt2,tpt3,tpt4,&diddraw))!=RTNORM) {rc=ret; goto bail;}
        }
    }

    rc=RTNORM;
bail:
	if (diddraw&1)	sds_grdraw(bpt1,bpt2,-1,0);
	if (diddraw&2)	sds_grdraw(bpt2,bpt3,-1,0);
	if (diddraw&4)	sds_grdraw(bpt3,bpt4,-1,0);
	if (diddraw&8)	sds_grdraw(bpt4,bpt1,-1,0);
	if (diddraw&16)	sds_grdraw(bpt3,bpt1,-1,0);
	if (diddraw&32)	sds_grdraw(tpt1,tpt2,-1,0);
	if (diddraw&64)	sds_grdraw(tpt2,tpt3,-1,0);
	if (diddraw&128)	sds_grdraw(bpt1,tpt1,-1,0);
	if (diddraw&256)	sds_grdraw(bpt2,tpt2,-1,0);
	if (diddraw&512)	sds_grdraw(bpt3,tpt3,-1,0);
	if (diddraw&1024)sds_grdraw(bpt4,tpt1,-1,0);
    return(rc);
}

/*-------------------------------------------------------------------------*//**
Implementation of AI_DOME
@return RTNORM - for success; others - for error
*//*--------------------------------------------------------------------------*/
short cmd_dome_mesh()
{

    if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002
	if (cmd_istransparent() == RTERROR) return RTERROR;

    RT rc,ret=0;
	sds_real rad;
    int longseg,latseg;
	sds_point pt0;

	if (sds_initget(RSG_NONULL,NULL)!=RTNORM) {
        rc=ret;
        goto bail;
    }
    if ((ret=sds_getpoint(NULL,ResourceString(IDC_3DSOLIDS__NCENTER_OF_DO_16, "\nCenter of dome: " ) ,pt0))==RTERROR) {
	    rc=ret; goto bail;
	}else if (ret==RTCAN){
	    rc=ret; goto bail;
	}else if (ret==RTNONE){
	    rc=ret; goto bail;
    }

    if (sds_initget(RSG_NONULL|RSG_NOZERO,ResourceString(IDC_3DSOLIDS_INITGET_DIAMET_17, "Diameter ~_Diameter" ) )!=RTNORM) {
        rc=ret;
        goto bail;
    }  // RSG_NONEG has no affect on getdist.
   if ((ret=sds_getdist(pt0,ResourceString(IDC_3DSOLIDS__NDIAMETER__RA_18, "\nDiameter/<Radius of dome>: " ) ,&rad))==RTERROR) {
	    rc=ret; goto bail;
	}else if (ret==RTCAN){
	    rc=ret; goto bail;
	}else if (ret==RTNONE){
	    rc=ret; goto bail;
	}else if (ret==RTKWORD){
        if (sds_initget(RSG_NONULL|RSG_NOZERO,NULL)!=RTNORM) {
            rc=ret;
            goto bail;
        }		// RSG_NONEG has no affect on getdist.
        if ((ret=sds_getdist(pt0,ResourceString(IDC_3DSOLIDS__NDIAMETER_OF__19, "\nDiameter of dome: " ) ,&rad))!=RTNORM) {rc=ret;goto bail;}
        rad/=2.0;
    }

	// Modified Cybage PP 2/03/2001 // Reason: Fix for Bug # 46896
    if (cmd_acad_compatible) {
		for (;;){
			if (sds_initget(RSG_NOZERO|RSG_NONEG,NULL)!=RTNORM) {
                rc=ret;
                goto bail;
            }
           if ((ret=sds_getint(ResourceString(IDC_3DSOLIDS__NNUMBER_OF_DI_20, "\nNumber of divisions around dome <16>: " ) ,&longseg))==RTERROR) {
				rc=ret;goto bail;
			}else if (ret==RTCAN) {
				rc=ret;goto bail;
			}else if (ret==RTNONE){
				longseg=16;
				break;
			}else if (ret==RTNORM){
				if (1==longseg){
					cmd_ErrorPrompt(CMD_ERR_SPHEREDIVS,0);  // needs more than one division
					continue;
				}else{
					break;
				}
			}
		}

		for (;;){
			if (sds_initget(RSG_NOZERO|RSG_NONEG,NULL)!=RTNORM) {
                rc=ret;
                goto bail;
            }
           if ((ret=sds_getint(ResourceString(IDC_3DSOLIDS__NNUMBER_OF_TO_21, "\nNumber of top-to-bottom divisions <8>: " ) ,&latseg))==RTERROR) {
				rc=ret;goto bail;
			}else if (ret==RTCAN) {
				rc=ret;goto bail;
			}else if (ret==RTNONE){
				latseg=8;
				break;
			}else if (ret==RTNORM){
				if (1==latseg){
					cmd_ErrorPrompt(CMD_ERR_SPHEREDIVS,0);  // needs more than one division
					continue;
				}else{
					break;
				}
			}
		}
    }
    else{
		longseg=16;
		latseg=8;
	}
    if ((ret=cmd_sphere_create(pt0,rad,longseg,latseg,1))!=RTNORM) {rc=ret;goto bail;}

    rc=RTNORM;
bail:
    return(rc);
}

/*-------------------------------------------------------------------------*//**
Implementation of AI_DISH
@return RTNORM - for success; others - for error
*//*--------------------------------------------------------------------------*/
short cmd_dish_mesh()
{

    if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002
	if (cmd_istransparent() == RTERROR) return RTERROR;

    RT rc,ret=0;
	sds_real rad;
    int longseg,latseg;
	sds_point pt0;

	if (sds_initget(RSG_NONULL,NULL)!=RTNORM) {
        rc=ret;
        goto bail;
    }
    if ((ret=sds_getpoint(NULL,ResourceString(IDC_3DSOLIDS__NCENTER_OF_DI_22, "\nCenter of dish: " ) ,pt0))==RTERROR) {
	    rc=ret; goto bail;
	}else if (ret==RTCAN){
	    rc=ret; goto bail;
	}else if (ret==RTNONE){
	    rc=ret; goto bail;
    }

   if (sds_initget(RSG_NONULL|RSG_NOZERO,ResourceString(IDC_3DSOLIDS_INITGET_DIAMET_17, "Diameter ~_Diameter" ) )!=RTNORM) {
       rc=ret;
       goto bail;
   }   // RSG_NONEG has no affect on getdist.
   if ((ret=sds_getdist(pt0,ResourceString(IDC_3DSOLIDS__NDIAMETER__RA_23, "\nDiameter/<Radius of dish>: " ) ,&rad))==RTERROR) {
		rc=ret; goto bail;
	}else if (ret==RTCAN){
		rc=ret; goto bail;
	}else if (ret==RTNONE){
		rc=ret; goto bail;
	}else if (ret==RTKWORD){
		if (sds_initget(RSG_NONULL|RSG_NOZERO,NULL)!=RTNORM) {
            rc=ret;
            goto bail;
        }		// RSG_NONEG has no affect on getdist.
       if ((ret=sds_getdist(pt0,ResourceString(IDC_3DSOLIDS__NDIAMETER_OF__24, "\nDiameter of dish: " ) ,&rad))!=RTNORM) {rc=ret;goto bail;}
		rad/=2.0;
	}

	if (cmd_acad_compatible) {
		for (;;){
			if (sds_initget(RSG_NOZERO|RSG_NONEG,NULL)!=RTNORM) {
                rc=ret;
                goto bail;
            }
           if ((ret=sds_getint(ResourceString(IDC_3DSOLIDS__NNUMBER_OF_DI_25, "\nNumber of divisions around dish <16>: " ) ,&longseg))==RTERROR) {
				rc=ret;goto bail;
			}else if (ret==RTCAN) {
				rc=ret;goto bail;
			}else if (ret==RTNONE){
				longseg=16;
				break;
			}else if (ret==RTNORM){
				if (1==longseg){
					cmd_ErrorPrompt(CMD_ERR_SPHEREDIVS,0);  // needs more than one division
					continue;
				}else{
					break;
				}
			}
		}

		for (;;){
			if (sds_initget(RSG_NOZERO|RSG_NONEG,NULL)!=RTNORM) {
                rc=ret;
                goto bail;
            }
           if ((ret=sds_getint(ResourceString(IDC_3DSOLIDS__NNUMBER_OF_TO_21, "\nNumber of top-to-bottom divisions <8>: " ) ,&latseg))==RTERROR) {
				rc=ret;goto bail;
			}else if (ret==RTCAN) {
				rc=ret;goto bail;
			}else if (ret==RTNONE){
				latseg=8;
				break;
			}else if (ret==RTNORM){
				if (1==latseg){
					cmd_ErrorPrompt(CMD_ERR_SPHEREDIVS,0);  // needs more than one division
					continue;
				}else{
					break;
				}
			}
		}
    }
    else{
		longseg=16;
		latseg=8;
	}

    if ((ret=cmd_sphere_create(pt0,rad,longseg,latseg,2))!=RTNORM) {rc=ret;goto bail;}

    rc=RTNORM;
bail:
    return(rc);
}

/*-------------------------------------------------------------------------*//**
Implementation of AI_SPHERE
@return RTNORM - for success; others - for error
*//*--------------------------------------------------------------------------*/
short cmd_sphere_mesh()
{

    if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002
	if (cmd_istransparent() == RTERROR) return RTERROR;

    RT rc,ret=0;
	sds_real rad;
    int longseg,latseg;
	sds_point pt0;

	if (sds_initget(RSG_NONULL,NULL)!=RTNORM) {
        rc=ret;
        goto bail;
    }
    if ((ret=sds_getpoint(NULL,ResourceString(IDC_3DSOLIDS__NCENTER_OF_SP_29, "\nCenter of sphere: " ) ,pt0))==RTERROR) {
	    rc=ret; goto bail;
	}else if (ret==RTCAN){
	    rc=ret; goto bail;
	}else if (ret==RTNONE){
	    rc=ret; goto bail;
    }

    if (sds_initget(RSG_NONULL|RSG_NOZERO,ResourceString(IDC_3DSOLIDS_INITGET_DIAMET_17, "Diameter ~_Diameter" ) )!=RTNORM) {
        rc=ret;
        goto bail;
    }  // RSG_NONEG has no affect on getdist.
   if ((ret=sds_getdist(pt0,ResourceString(IDC_3DSOLIDS__NDIAMETER__RA_30, "\nDiameter/<Radius of sphere>: " ) ,&rad))==RTERROR) {
	    rc=ret; goto bail;
	}else if (ret==RTCAN){
	    rc=ret; goto bail;
	}else if (ret==RTNONE){
	    rc=ret; goto bail;
	}else if (ret==RTKWORD){
        if (sds_initget(RSG_NONULL|RSG_NOZERO,NULL)!=RTNORM) {
            rc=ret;
            goto bail;
        }		// RSG_NONEG has no affect on getdist.
        if ((ret=sds_getdist(pt0,ResourceString(IDC_3DSOLIDS__NDIAMETER_OF__31, "\nDiameter of sphere: " ) ,&rad))!=RTNORM) {rc=ret;goto bail;}
        rad/=2.0;
    }

	if (cmd_acad_compatible) {
		for (;;){
			if (sds_initget(RSG_NOZERO|RSG_NONEG,NULL)!=RTNORM) {
                rc=ret;
                goto bail;
            }
           if ((ret=sds_getint(ResourceString(IDC_3DSOLIDS__NNUMBER_OF_LO_32, "\nNumber of longitudinal sections <16>: " ) ,&longseg))==RTERROR) {
				rc=ret;goto bail;
			}else if (ret==RTCAN) {
				rc=ret;goto bail;
			}else if (ret==RTNONE){
				longseg=16;
				break;
			}else if (ret==RTNORM){
				if (1==longseg){
					cmd_ErrorPrompt(CMD_ERR_SPHEREDIVS,0);  // needs more than one division
					continue;
				}else{
					break;
				}
			}
		}

		for (;;){
			if (sds_initget(RSG_NOZERO|RSG_NONEG,NULL)!=RTNORM) {
                rc=ret;
                goto bail;
            }
           if ((ret=sds_getint(ResourceString(IDC_3DSOLIDS__NNUMBER_OF_LA_33, "\nNumber of latitudinal sections <16>: " ) ,&latseg))==RTERROR) {
				rc=ret;goto bail;
			}else if (ret==RTCAN) {
				rc=ret;goto bail;
			}else if (ret==RTNONE){
				latseg=16;
				break;
			}else if (ret==RTNORM){
				if (1==latseg){
					cmd_ErrorPrompt(CMD_ERR_SPHEREDIVS,0);  // needs more than one division
					continue;
				}else{
					break;
				}
			}
		}

	}else{
		longseg=16;
		latseg=16;
	}

	if ((ret=cmd_sphere_create(pt0,rad,longseg,latseg,0))!=RTNORM) {rc=ret;goto bail;}

    rc=RTNORM;
bail:
    return(rc);
}

/*-------------------------------------------------------------------------*//**
Implementation of AI_TORUS
@return RTNORM - for success; others - for error
*//*--------------------------------------------------------------------------*/
short cmd_torus_mesh()
{

    if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002
	if (cmd_istransparent() == RTERROR) return RTERROR;

    RT rc,ret=0;
    int tubseg = 16,torseg = 16;
    sds_point pt0;
    sds_real tordia,tubdia;


	if (sds_initget(0,NULL)!=RTNORM) {
        rc=ret;
        goto bail;
    }
    if ((ret=sds_getpoint(NULL,ResourceString(IDC_3DSOLIDS__NCENTER_OF_WH_35, "\nCenter of whole torus: <0,0,0>" ) ,pt0))==RTERROR) {
	    rc=ret; goto bail;
	}else if (ret==RTCAN){
	    rc=ret; goto bail;
	}else if (ret==RTNONE){
	    pt0[0]=pt0[1]=pt0[2]=0.0;
    }

    if (sds_initget(7,ResourceString(IDC_3DSOLIDS_INITGET_DIAMET_17, "Diameter ~_Diameter" ) )!=RTNORM) {
        rc=ret;
        goto bail;
    }
   if ((ret=sds_getdist(pt0,ResourceString(IDC_3DSOLIDS__NDIAMETER__RA_36, "\nDiameter/<Radius of whole torus>: " ),&tordia))==RTERROR) {
	    rc=ret; goto bail;
	}else if (ret==RTCAN){
	    rc=ret; goto bail;
	}else if (ret==RTNONE){
	    rc=ret; goto bail;
	}else if (ret==RTKWORD){
        if (sds_initget(7,NULL)!=RTNORM) {
            rc=ret;
            goto bail;
        }
        if ((ret=sds_getdist(pt0,ResourceString(IDC_3DSOLIDS__NDIAMETER_OF__37, "\nDiameter of whole torus: " ) ,&tordia))!=RTNORM) {rc=ret;goto bail;}
    }else if (ret==RTNORM){
        tordia*=2.0;
    }


    if (sds_initget(5,ResourceString(IDC_3DSOLIDS_INITGET_DIAMET_17, "Diameter ~_Diameter" ) )!=RTNORM) {
        rc=ret;
        goto bail;
    }
    if ((ret=sds_getdist((cmd_acad_compatible ? pt0 : NULL),ResourceString(IDC_3DSOLIDS__NDIAMETER__RA_38, "\nDiameter/<Radius of body of torus>: " ) ,&tubdia))==RTERROR) {
        rc=ret; goto bail;
    }else if (ret==RTCAN){
        rc=ret; goto bail;
    }else if (ret==RTNONE){
        rc=ret; goto bail;
    }else if (ret==RTKWORD){
        if (sds_initget(5,NULL)!=RTNORM) {
            rc=ret;
            goto bail;
        }
        if ((ret=sds_getdist((cmd_acad_compatible ? pt0 : NULL),ResourceString(IDC_3DSOLIDS__NDIAMETER_OF__39, "\nDiameter of body of torus: " ) ,&tubdia))!=RTNORM) {rc=ret;goto bail;}
    }else if (ret==RTNORM){
        tubdia*=2.0;
    }

    if (cmd_acad_compatible) {
        if (2.0*tubdia>tordia) {
            tubdia=tordia/2.0;
        }
    }

    if (cmd_acad_compatible) {
		if (sds_initget(6,NULL)!=RTNORM) {
            rc=ret;
            goto bail;
        }
       if ((ret=sds_getint(ResourceString(IDC_3DSOLIDS__NDIVISIONS_AR_40, "\nDivisions around body of torus <16>: " ) ,&tubseg))==RTERROR) {
			rc=ret;goto bail;
		}else if (ret==RTCAN) {
			rc=ret;goto bail;
		}else if (ret==RTNONE){
			tubseg=16;
		}
		if (sds_initget(6,NULL)!=RTNORM) {
            rc=ret;
            goto bail;
        }
       if ((ret=sds_getint(ResourceString(IDC_3DSOLIDS__NDIVISIONS_AR_41, "\nDivisions around whole torus <16>: " ) ,&torseg))==RTERROR) {
			rc=ret;goto bail;
		}else if (ret==RTCAN) {
			rc=ret;goto bail;
		}else if (ret==RTNONE){
			torseg=16;
		}
	}

    if ((ret=cmd_torus_create(pt0,tordia,tubdia,torseg,tubseg))!=RTNORM) {rc=ret;goto bail;}

    rc=RTNORM;
bail:
    return(rc);
}

/*-------------------------------------------------------------------------*//**
Implementation of AI_CONE
@return RTNORM - for success; others - for error
*//*--------------------------------------------------------------------------*/
short cmd_cone_mesh()
{
    if (cmd_iswindowopen() == RTERROR) return RTERROR;

    char prompt[IC_ACADBUF],fs1[IC_ACADBUF];
	RT rc,ret=0;
    struct resbuf setgetrb,*pDragrb=NULL,rbent,rbucs;
	dwg_entlink *elp=NULL;
    sds_point pt0,pt1,pthigh,cent,ptmp,extru,v1,v2;
    sds_real bdia,tdia,dist1=1.0,dist2=1.0,spar=0.0,epar=IC_TWOPI,ratio;
    int nseg;
    sds_name ename;

	rbent.restype=RT3DPOINT;
	rbucs.restype=RTSHORT;
	rbucs.resval.rint=1;
    nseg=16;

	if (!cmd_acad_compatible) {
again:
       sprintf(prompt,ResourceString(IDC_3DSOLIDS__NELLIPTICAL_D_43, "\nElliptical/Divisions/<Center of base of cone> <0,0,0>: " ) );
        if (sds_initget(0,ResourceString(IDC_3DSOLIDS_INITGET_ELLIPT_44, "Elliptical Divisions ~_Elliptical ~_Divisions" ) )!=RTNORM) {rc=ret;goto bail;}
        if ((ret=sds_getpoint(NULL,prompt,pt0))==RTERROR) {
            rc=ret;goto bail;
        }else if (ret==RTCAN){
            rc=ret;goto bail;
        }else if (ret==RTNONE){
            pt0[0]=0.0;pt0[1]=0.0;pt0[2]=0.0;
        }else if (ret==RTKWORD){
            sds_getinput(fs1);
            if (strisame("DIVISIONS"/*DNT*/ ,fs1)){
                while(1){
                    sprintf(prompt,ResourceString(IDC_3DSOLIDS__NNUMBER_OF_DI_46, "\nNumber of Divisions <%d>: " ) ,nseg);
                    if (sds_initget(6,NULL)!=RTNORM) {
                        rc=ret;
                        goto bail;
                    }
                    if ((ret=sds_getint(prompt,&nseg))==RTERROR || ret==RTCAN) {
                        rc=ret;
                        goto bail;
                    }
                    if (nseg>=2||nseg<=32767) goto again;
                    cmd_ErrorPrompt(CMD_ERR_SURFTABS,0);
                }
            }else{
                if (sds_initget(1,ResourceString(IDC_3DSOLIDS_INITGET_CENTER_47, "Center ~_Center" ) )!=RTNORM) {
                    rc=ret;
                    goto bail;
                }
                if ((ret=sds_getpoint(NULL,ResourceString(IDC_3DSOLIDS__NCENTER__FIRS_48, "\nCenter/<First end of ellipse axis>: " ) ,pt0))==RTERROR) {
                    rc=ret;
                    goto bail;
                }else if (ret==RTCAN){
                    rc=ret;
                    goto bail;
                }else if (ret==RTKWORD){
                    if (sds_initget(1,NULL)!=RTNORM) {
                        rc=ret;
                        goto bail;
                    }
                    if ((ret=sds_getpoint(NULL,ResourceString(IDC_3DSOLIDS__NCENTER_OF_EL_49, "\nCenter of ellipse: " ) ,cent))!=RTNORM) {
                        rc=ret;
                        goto bail;
                    }
                    if (sds_initget(1,NULL)!=RTNORM) {
                        rc=ret;
                        goto bail;
                    }
                    if ((ret=sds_getpoint(cent,ResourceString(IDC_3DSOLIDS__NENDPOINT_OF__50, "\nEndpoint of axis: " ) ,pt0))!=RTNORM) {
                        rc=ret;
                        goto bail;
                    }
                    pt0[2]=cent[2];
                    dist1=sqrt((pt0[0]-cent[0])*(pt0[0]-cent[0])+(pt0[1]-cent[1])*(pt0[1]-cent[1]));

                }else if (ret==RTNORM){
                    if (sds_initget(1,NULL)!=RTNORM) {
                        rc=ret;
                        goto bail;
                    }
                    if ((ret=sds_getpoint(pt0,ResourceString(IDC_3DSOLIDS__NSECOND_END_O_51, "\nSecond end of ellipse axis: " ) ,pt1))!=RTNORM) {rc=ret;goto bail;}
                        cent[0]=(pt1[0]+pt0[0])/2.0;
                        cent[1]=(pt1[1]+pt0[1])/2.0;
                        cent[2]=pt0[2];
                        dist1=sqrt((pt0[0]-cent[0])*(pt0[0]-cent[0])+(pt0[1]-cent[1])*(pt0[1]-cent[1]));
                }
            }
            SDS_getvar(NULL,DB_QUCSXDIR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	        ic_ptcpy(ptmp,setgetrb.resval.rpoint);
            SDS_getvar(NULL,DB_QUCSYDIR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	        rbent.resval.rpoint[0]=(ptmp[1]*setgetrb.resval.rpoint[2])-(setgetrb.resval.rpoint[1]*ptmp[2]);
	        rbent.resval.rpoint[1]=(ptmp[2]*setgetrb.resval.rpoint[0])-(setgetrb.resval.rpoint[2]*ptmp[0]);
	        rbent.resval.rpoint[2]=(ptmp[0]*setgetrb.resval.rpoint[1])-(setgetrb.resval.rpoint[0]*ptmp[1]);
	        ic_ptcpy(extru,rbent.resval.rpoint);

			pt1[0]=pt0[0]-cent[0];
			pt1[1]=pt0[1]-cent[1];
			pt1[2]=pt0[2]-cent[2];
			elp=new db_ellipse(SDS_CURDWG);
			elp->set_210(extru);
			elp->set_40(dist2);
			elp->set_41(spar);
			elp->set_42(epar);
			sds_trans(cent,&rbucs,&rbent,0,ptmp);
			elp->set_10(ptmp);
			sds_trans(pt1,&rbucs,&rbent,1,ptmp);
			elp->set_11(ptmp);
    		ename[0]=(long)elp;
			ename[1]=(long)SDS_CURDWG;
			sds_initget(71,NULL);

            int oldGetZScale = SDS_GetZScale;
            SDS_GetZScale = 1;
			SDS_getvar(NULL,DB_QDRAGMODE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
            rc=SDS_dragobject(-9,setgetrb.resval.rint,(sds_real *)ename,(double *)elp->retp_40(),0.0,ResourceString(IDC_3DSOLIDS__OTHER_AXIS____52, "<Other axis>: " ) ,(struct resbuf **)cent,pt1,NULL);
            SDS_GetZScale = oldGetZScale;
           dist2=pt1[0]*dist1;
       		ratio=dist2/dist1;

            if (elp) { delete elp; elp=NULL; }

          	ic_ptcpy(pthigh,cent);
            if (sds_initget(1,ResourceString(IDC_3DSOLIDS_INITGET_APEX___53, "Apex ~_Apex" ) )!=RTNORM) {
                rc=ret;
                goto bail;
            }
            if ((ret=sds_getdist(NULL,ResourceString(IDC_3DSOLIDS__NAPEX__HEIGHT_54, "\nApex/<Height of cone>: " ) ,&pthigh[2]))==RTERROR) {
                rc=ret;
                goto bail;
            }else if (ret==RTCAN){
                rc=ret;
                goto bail;
            }else if (ret==RTKWORD){
                if (sds_initget(1,NULL)!=RTNORM) {
                    rc=ret;
                    goto bail;
                }
                if ((ret=sds_getpoint(cent,ResourceString(IDC_3DSOLIDS_APEX_OF_CONE___55, "Apex of cone: " ) ,pthigh))!=RTNORM) {
                    rc=ret;
                    goto bail;
                }
            }else if (ret==RTNORM){
                pthigh[2]+=cent[2];
            }
	        cmd_pt_sub(pt0,cent,v1);
            ic_crossproduct(extru,v1,v2);
            cmd_rpt_mul(v2,ratio,v2);
            if ((ret=cmd_cone_create_elp(cent,v1,v2,ratio,pthigh,nseg))!=RTNORM) {rc=ret;goto bail;}

            return(RTNORM);
        }
    }else{
       sprintf(prompt,ResourceString(IDC_3DSOLIDS__NCENTER_OF_BA_56, "\nCenter of base of cone: " ) );
        if (sds_initget(1,NULL)!=RTNORM) {
            rc=ret;
            goto bail;
        }
        if ((ret=sds_getpoint(NULL,prompt,pt0))!=RTNORM) {
            rc=ret;
            goto bail;
        }
    }

    if (sds_initget(7,ResourceString(IDC_3DSOLIDS_INITGET_DIAMET_17, "Diameter ~_Diameter" ) )!=RTNORM) {
        rc=ret;
        goto bail;
    }
	SDS_getvar(NULL,DB_QDRAGMODE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
   if ((ret=SDS_dragobject(1,setgetrb.resval.rint,pt0,NULL,0.0,ResourceString(IDC_3DSOLIDS__NDIAMETER__RA_57, "\nDiameter/<Radius of base>: " ) ,&pDragrb,pt1,NULL))==RTERROR) {
        rc=ret;
        goto bail;
    }else if (ret==RTCAN){
        rc=ret;
        goto bail;
    }else if (ret==RTKWORD){
        IC_RELRB(pDragrb);
        sds_getinput(fs1);
        if (strisame("DIAMETER"/*DNT*/,fs1)) {
            if (sds_initget(7,NULL)!=RTNORM) {
                rc=ret;
                goto bail;
            }
           if ((ret=SDS_dragobject(1,setgetrb.resval.rint,pt0,NULL,0.0,ResourceString(IDC_3DSOLIDS__NDIAMETER_OF__59, "\nDiameter of base: " ) ,&pDragrb,pt1,NULL))!=RTNORM) {rc=ret;goto bail;}
		    ic_assoc(pDragrb,40);
            bdia=ic_rbassoc->resval.rreal;
        }
    }else if (ret==RTNORM){
		ic_assoc(pDragrb,40);
        bdia=2.0*ic_rbassoc->resval.rreal;
    }
    IC_RELRB(pDragrb);

	if (cmd_acad_compatible) {
       if (sds_initget(4,ResourceString(IDC_3DSOLIDS_INITGET_DIAMET_17, "Diameter ~_Diameter" ) )!=RTNORM) {
           rc=ret;
           goto bail;
       }
       if ((ret=SDS_dragobject(1,setgetrb.resval.rint,pt0,NULL,0.0,ResourceString(IDC_3DSOLIDS__NDIAMETER__RA_60, "\nDiameter/<Radius of top surface> <0>: " ) ,&pDragrb,pt1,NULL))==RTERROR) {
			 rc=ret;
             goto bail;
		}else if (ret==RTCAN){
			rc=ret;
            goto bail;
		}else if (ret==RTKWORD){
            IC_RELRB(pDragrb);
			sds_getinput(fs1);
            if (strisame("DIAMETER"/*DNT*/ ,fs1)) {
				if (sds_initget(4,NULL)!=RTNORM) {
                    rc=ret;
                    goto bail;
                }
               if ((ret=SDS_dragobject(1,setgetrb.resval.rint,pt0,NULL,0.0,ResourceString(IDC_3DSOLIDS__NDIAMETER__0__61, "\nDiameter <0>: " ) ,&pDragrb,pt1,NULL))==RTERROR) {
                    rc=ret;
                    goto bail;
                }else if (ret==RTCAN){
			        rc=ret;
                    goto bail;
                }else if (ret==RTNONE||pDragrb==NULL){
			        tdia=0.0;
                }else if (ret==RTNORM){
		            ic_assoc(pDragrb,40);
                    tdia=ic_rbassoc->resval.rreal;
			    }
            }
		}else if (ret==RTNONE||pDragrb==NULL){
			tdia=0.0;
		}else if (ret==RTNORM){
		    ic_assoc(pDragrb,40);
            tdia=2.0*ic_rbassoc->resval.rreal;
		}
	}else tdia=0.0;
    IC_RELRB(pDragrb);

	ic_ptcpy(pthigh,pt0);
	if (!cmd_acad_compatible) {
        if (sds_initget(1,ResourceString(IDC_3DSOLIDS_INITGET_APEX___53, "Apex ~_Apex" ) )!=RTNORM) {
            rc=ret;
            goto bail;
        }
        if ((ret=sds_getdist(NULL,ResourceString(IDC_3DSOLIDS__NAPEX__HEIGHT_54, "\nApex/<Height of cone>: " ) ,&pthigh[2]))==RTERROR) {
            rc=ret;
            goto bail;
        }else if (ret==RTCAN){
            rc=ret;
            goto bail;
        }else if (ret==RTKWORD){
            if (sds_initget(1,NULL)!=RTNORM) {
                rc=ret;
                goto bail;
            }
            if ((ret=sds_getpoint(pt0,ResourceString(IDC_3DSOLIDS_APEX_OF_CONE___55, "Apex of cone: " ) ,pthigh))!=RTNORM) {
                rc=ret;
                goto bail;
            }
        }else if (ret==RTNORM){
            pthigh[2]+=pt0[2];
        }
    }else{
        if (sds_initget(1,NULL)!=RTNORM) {
            rc=ret;
            goto bail;
        }
        if ((ret=sds_getdist(pt0,ResourceString(IDC_3DSOLIDS__NHEIGHT_OF_CO_62, "\nHeight of cone: " ) ,&pthigh[2]))!=RTNORM) {
            rc=ret;
            goto bail;
        }
        pthigh[2]+=pt0[2];

		do{
            if (sds_initget(6,NULL)!=RTNORM) {
                rc=ret;
                goto bail;
            }
           if ((ret=sds_getint(ResourceString(IDC_3DSOLIDS__NNUMBER_OF_DI_63, "\nNumber of divisions around cone <16>: " ) ,&nseg))==RTERROR) {
			    rc=ret;
                goto bail;
		    }else if (ret==RTCAN){
			    rc=ret;
                goto bail;
		    }else if (ret==RTNONE){
			    nseg=16;
		    }
			if (nseg<2 || nseg>32767) cmd_ErrorPrompt(CMD_ERR_SURFTABS,0);
		}while(nseg<2 || nseg>32767);
	}

    if ((ret=cmd_cone_create(pt0,bdia,tdia,pthigh,nseg))!=RTNORM) {rc=ret;goto bail;}

    rc=RTNORM;
bail:
    if (elp) { delete elp; elp=NULL; }
    IC_RELRB(pDragrb);
	return(rc);
}

/*-------------------------------------------------------------------------*//**
Implementation of AI_CYLINDER
@return RTNORM - for success; others - for error
*//*--------------------------------------------------------------------------*/
short cmd_cylinder_mesh()
{
    char prompt[IC_ACADBUF],fs1[IC_ACADBUF];
	RT rc,ret=0;
    sds_point pt0,pt1,pthigh,cent,ptmp,extru,v1,v2;
    sds_real bdia,dist1=1.0,dist2=1.0,spar=0.0,epar=IC_TWOPI,ratio;
	dwg_entlink *elp=NULL;
    sds_name ename;
    int nseg;
	struct resbuf setgetrb,*pDragrb=NULL,rbucs,rbwcs;

//	rbent.restype=RT3DPOINT;
	rbucs.restype=RTSHORT;
	rbucs.resval.rint=1;
	rbwcs.restype=RTSHORT;
	rbwcs.resval.rint=0;
    nseg=16;

    if (!cmd_acad_compatible) {
again:
       sprintf(prompt,ResourceString(IDC_3DSOLIDS__NELLIPTICAL_D_64, "\nElliptical/Divisions/<Center of base of cylinder> <0,0,0>: " ) );
        if (sds_initget(0,ResourceString(IDC_3DSOLIDS_INITGET_ELLIPT_44, "Elliptical Divisions ~_Elliptical ~_Divisions" ) )!=RTNORM) {
            rc=ret;
            goto bail;
        }
        if ((ret=sds_getpoint(NULL,prompt,pt0))==RTERROR) {
            rc=ret;
            goto bail;
        }else if (ret==RTCAN){
            rc=ret;
            goto bail;
        }else if (ret==RTNONE){
            pt0[0]=0.0;pt0[1]=0.0;pt0[2]=0.0;
        }else if (ret==RTKWORD){
            sds_getinput(fs1);
            if (strisame("DIVISIONS"/*DNT*/ ,fs1)){
                while(1){
                    sprintf(prompt,ResourceString(IDC_3DSOLIDS__NNUMBER_OF_DI_46, "\nNumber of Divisions <%d>: " ) ,nseg);
                    if (sds_initget(6,NULL)!=RTNORM) {
                        rc=ret;
                        goto bail;
                    }
                    if ((ret=sds_getint(prompt,&nseg))==RTERROR || ret==RTCAN) {
                        rc=ret;
                        goto bail;
                    }
                    if (nseg>=2||nseg<=32767)
                        goto again;
                    cmd_ErrorPrompt(CMD_ERR_SURFTABS,0);
                }
            }else{
                if (sds_initget(1,ResourceString(IDC_3DSOLIDS_INITGET_CENTER_47, "Center ~_Center" ) )!=RTNORM) {
                    rc=ret;
                    goto bail;
                }
                if ((ret=sds_getpoint(NULL,ResourceString(IDC_3DSOLIDS__NCENTER__FIRS_48, "\nCenter/<First end of ellipse axis>: " ) ,pt0))==RTERROR) {
                    rc=ret;
                    goto bail;
                }else if (ret==RTCAN){
                    rc=ret;
                    goto bail;
                }else if (ret==RTKWORD){
                    if (sds_initget(1,NULL)!=RTNORM) {
                        rc=ret;
                        goto bail;
                    }
                    if ((ret=sds_getpoint(NULL,ResourceString(IDC_3DSOLIDS__NCENTER_OF_EL_49, "\nCenter of ellipse: " ) ,cent))!=RTNORM) {
                        rc=ret;
                        goto bail;
                    }
                    if (sds_initget(1,NULL)!=RTNORM) {
                        rc=ret;
                        goto bail;
                    }
                    if ((ret=sds_getpoint(cent,ResourceString(IDC_3DSOLIDS__NENDPOINT_OF__50, "\nEndpoint of axis: " ) ,pt0))!=RTNORM) {
                        rc=ret;
                        goto bail;
                    }
					//Acad doesn't treat this as a planar entity, and the other endpt of the axis is
					//	NOT assumed to be at the same elev as the first.  We need to do a true 3D dist,
					//	and then correct pt0's elevation so the entity will be in current XY plane.
                    //pt0[2]=cent[2];
                    //dist1=sqrt((pt0[0]-cent[0])*(pt0[0]-cent[0])+(pt0[1]-cent[1])*(pt0[1]-cent[1]));
					//replacement code here:
					dist1=sds_distance(pt0,cent);
					sds_polar(cent,sds_angle(cent,pt0),dist1,pt0);

                }else if (ret==RTNORM){
                    if (sds_initget(1,NULL)!=RTNORM) {
                        rc=ret;
                        goto bail;
                    }
                    if ((ret=sds_getpoint(pt0,ResourceString(IDC_3DSOLIDS__NSECOND_END_O_65, "\nSecond end of ellipse axis>: " ) ,pt1))!=RTNORM) {
                        rc=ret;
                        goto bail;
                    }
                    cent[0]=(pt1[0]+pt0[0])/2.0;
                    cent[1]=(pt1[1]+pt0[1])/2.0;
                    cent[2]=pt0[2];
                    dist1=sqrt((pt0[0]-cent[0])*(pt0[0]-cent[0])+(pt0[1]-cent[1])*(pt0[1]-cent[1]));
                }
            }
            SDS_getvar(NULL,DB_QUCSXDIR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	        ic_ptcpy(ptmp,setgetrb.resval.rpoint);
            SDS_getvar(NULL,DB_QUCSYDIR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	        ic_crossproduct(ptmp,setgetrb.resval.rpoint,extru);
//            rbent.resval.rpoint[0]=(ptmp[1]*setgetrb.resval.rpoint[2])-(setgetrb.resval.rpoint[1]*ptmp[2]);
//	        rbent.resval.rpoint[1]=(ptmp[2]*setgetrb.resval.rpoint[0])-(setgetrb.resval.rpoint[2]*ptmp[0]);
//	        rbent.resval.rpoint[2]=(ptmp[0]*setgetrb.resval.rpoint[1])-(setgetrb.resval.rpoint[0]*ptmp[1]);
//	        ic_ptcpy(extru,rbent.resval.rpoint);
            do{
			    pt1[0]=pt0[0]-cent[0];
			    pt1[1]=pt0[1]-cent[1];
			    pt1[2]=pt0[2]-cent[2];
			    elp=new db_ellipse(SDS_CURDWG);
			    elp->set_210(extru);
			    elp->set_40(dist2);
			    elp->set_41(spar);
			    elp->set_42(epar);
			    sds_trans(cent,&rbucs,&rbwcs,0,ptmp);
			    elp->set_10(ptmp);
			    sds_trans(pt1,&rbucs,&rbwcs,1,ptmp);
			    elp->set_11(ptmp);
    		    ename[0]=(long)elp;
			    ename[1]=(long)SDS_CURDWG;
			    sds_initget(71,NULL);
			    SDS_getvar(NULL,DB_QDRAGMODE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
                rc=SDS_dragobject(-9,setgetrb.resval.rint,(sds_real *)ename,(double *)elp->retp_40(),0.0,ResourceString(IDC_3DSOLIDS__OTHER_AXIS____52, "<Other axis>: " ) ,(struct resbuf **)cent,pt1,NULL);
			    dist2=pt1[0]*dist1;
       		    ratio=pt1[0]; //dist2/dist1;
                if (pt1[0]<=100.0 && pt1[0]>=0.01){
                    break;
                }else{
                    cmd_ErrorPrompt(CMD_ERR_TRYAGAIN,0);
                }
            }while(1);

            if (elp) { delete elp; elp=NULL; }

          	ic_ptcpy(pthigh,cent);
            if (sds_initget(1,ResourceString(IDC_3DSOLIDS_INITGET_CENTER_47, "Center ~_Center" ) )!=RTNORM) {
                rc=ret;
                goto bail;
            }
            if ((ret=sds_getdist(NULL,ResourceString(IDC_3DSOLIDS__NCENTER_OF_SE_66, "\nCenter of second end/<Height of cylinder>: " ) ,&pthigh[2]))==RTERROR) {
                rc=ret;
                goto bail;
            }else if (ret==RTCAN){
                rc=ret;
                goto bail;
            }else if (ret==RTKWORD){
                if (sds_initget(1,NULL)!=RTNORM) {
                    rc=ret;
                    goto bail;
                }
                if ((ret=sds_getpoint(pt0,ResourceString(IDC_3DSOLIDS_CENTER_OF_SECO_67, "Center of second end: " ) ,pthigh))!=RTNORM) {
                    rc=ret;
                    goto bail;
                }
            }else if (ret==RTNORM){
                pthigh[2]+=cent[2];
            }
	        cmd_pt_sub(pt0,cent,v1);
            sds_trans(v1,&rbucs,&rbwcs,1,pt1);
            ic_crossproduct(extru,pt1,v2);
            cmd_rpt_mul(v2,ratio,v2);

            if ((ret=cmd_cyl_create_elp(cent,v1,v2,ratio,pthigh,nseg))!=RTNORM) {rc=ret;goto bail;}

            return(RTNORM);
        }
    }else{
       sprintf(prompt,ResourceString(IDC_3DSOLIDS__NCENTER_OF_BA_68, "\nCenter of base of cylinder: " ) );
        if (sds_initget(1,NULL)!=RTNORM) {
            rc=ret;
            goto bail;
        }
        if ((ret=sds_getpoint(NULL,prompt,pt0))!=RTNORM) {rc=ret;goto bail;}
    }

	SDS_getvar(NULL,DB_QDRAGMODE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	for (;;) {
       if (sds_initget(RSG_NONULL | RSG_NOZERO | RSG_NONEG | SDS_RSG_NODOCCHG,ResourceString(IDC_3DSOLIDS_INITGET_DIAMET_17, "Diameter ~_Diameter" ) )!=RTNORM) {
           rc=ret;
           goto bail;
       }
       if ((ret=SDS_dragobject(1,setgetrb.resval.rint,pt0,NULL,0.0,ResourceString(IDC_3DSOLIDS__NDIAMETER__RA_69, "\nDiameter/<Radius of cylinder>: " ) ,&pDragrb,pt1,NULL))==RTERROR) {
			rc=ret;
            goto bail;
		}else if (ret==RTCAN){
			rc=ret;
            goto bail;
		}else if (ret==RTNORM){
			ic_assoc(pDragrb,40);
			bdia=2.0*ic_rbassoc->resval.rreal;
			break;
		}else if (ret==RTKWORD){		// SDS_dragobject returns RTKWORD for any alpha input, regardless of the initget.
			sds_getinput(fs1);
            if (strisame("DIAMETER"/*DNT*/ ,fs1)) {
				if (sds_initget(7,NULL)!=RTNORM) {
                    rc=ret;
                    goto bail;
                }
               if ((ret=SDS_dragobject(20,setgetrb.resval.rint,pt0,NULL,0.0,ResourceString(IDC_3DSOLIDS__NDIAMETER_OF__70, "\nDiameter of cylinder: " ) ,&pDragrb,pt1,NULL))!=RTNORM) {
                   rc=ret;
                   goto bail;
               }
				ic_assoc(pDragrb,40);
				bdia=(2*(ic_rbassoc->resval.rreal));	// Kludge - how should I do this right?
				break;
			} else {
				cmd_ErrorPrompt(CMD_ERR_PTKWORD,0);    // "Please pick a point or choose an option from the list."
			}
		}
	}
    if (pDragrb!=NULL){sds_relrb(pDragrb);pDragrb=NULL;}

	ic_ptcpy(pthigh,pt0);
	if (!cmd_acad_compatible) {
        if (sds_initget(1,ResourceString(IDC_3DSOLIDS_INITGET_CENTER_47, "Center ~_Center" ) )!=RTNORM) {
            rc=ret;
            goto bail;
        }
        if ((ret=sds_getdist(NULL,ResourceString(IDC_3DSOLIDS__NCENTER_OF_SE_66, "\nCenter of second end/<Height of cylinder>: " ) ,&pthigh[2]))==RTERROR) {
            rc=ret;
            goto bail;
        }else if (ret==RTCAN){
            rc=ret;
            goto bail;
        }else if (ret==RTKWORD){
            if (sds_initget(1,NULL)!=RTNORM) {
                rc=ret;
                goto bail;
            }
            if ((ret=sds_getpoint(pt0,ResourceString(IDC_3DSOLIDS_CENTER_OF_SECO_67, "Center of second end: " ) ,pthigh))!=RTNORM) {
                rc=ret;
                goto bail;
            }
        }else if (ret==RTNORM){
            pthigh[2]+=pt0[2];
        }
    }else{
        if (sds_initget(3,NULL)!=RTNORM) {
            rc=ret;
            goto bail;
        }
        if ((ret=sds_getdist(NULL,ResourceString(IDC_3DSOLIDS__NHEIGHT_OF_CY_71, "\nHeight of cylinder: " ),&pthigh[2]))!=RTNORM) {
            rc=ret;
            goto bail;
        }
        pthigh[2]+=pt0[2];
        do{
		    if (sds_initget(6,NULL)!=RTNORM) {
                rc=ret;
                goto bail;
            }
           if ((ret=sds_getint(ResourceString(IDC_3DSOLIDS__NNUMBER_OF_DI_72, "\nNumber of divisions around cylinder <16>: " ) ,&nseg))==RTERROR) {
			    rc=ret;
                goto bail;
		    }else if (ret==RTCAN){
			    rc=ret;
                goto bail;
		    }else if (ret==RTNONE){
			    nseg=16;
		    }
			if (nseg<2 || nseg>32767)cmd_ErrorPrompt(CMD_ERR_SURFTABS,0);
		}while(nseg<2 || nseg>32767);
	}

    if ((ret=cmd_cyl_create(pt0,bdia,pthigh,nseg))!=RTNORM) {rc=ret;goto bail;}

    rc=RTNORM;
bail:
    if (pDragrb!=NULL){sds_relrb(pDragrb);pDragrb=NULL;}
	return(rc);
}

/*-------------------------------------------------------------------------*//**
Implementation of AI_WEDGE
@return RTNORM - for success; others - for error
*//*--------------------------------------------------------------------------*/
short cmd_wedge_mesh()
{

    if (cmd_iswindowopen() == RTERROR) return RTERROR;

	char prompt[IC_ACADBUF],fs1[IC_ACADBUF];
	char szTmp0[IC_ACADBUF],szTmp1[IC_ACADBUF],szTmp2[IC_ACADBUF];
    sds_point pt0,pt1,pt2,lastpt;
	sds_real len,wth,high;
	RT ret,rc;
	struct resbuf setgetrb,rbucs,rbwcs;

	rbucs.restype=rbwcs.restype=RTSHORT;
	rbucs.resval.rint=1;
	rbwcs.resval.rint=0;

	//get last point
	if ((ret=SDS_getvar(NULL,DB_QLASTPOINT,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM) {rc=ret;goto bail;}
	ic_ptcpy(lastpt,setgetrb.resval.rpoint);

	sds_rtos(lastpt[0],-1,-1,szTmp0);
	sds_rtos(lastpt[1],-1,-1,szTmp1);
	sds_rtos(lastpt[2],-1,-1,szTmp2);
    sprintf(prompt,ResourceString(IDC_3DSOLIDS__NCENTER__CORN_82, "\nCenter/<Corner of wedge> <%s,%s,%s>: " ) ,szTmp0,szTmp1,szTmp2);
    if (sds_initget(0,ResourceString(IDC_3DSOLIDS_INITGET_CENTER_47, "Center ~_Center" ) )!=RTNORM) {
        rc=ret;
        goto bail;
    }
	if ((ret=sds_getpoint(NULL,prompt,pt1))==RTERROR) {
	    rc=ret; goto bail;
	}else if (ret==RTCAN){
	    rc=ret; goto bail;
	}else if (ret==RTKWORD){
        sprintf(prompt,ResourceString(IDC_3DSOLIDS__NCENTER_OF_WE_83, "\nCenter of wedge <%s,%s,%s>: " ) ,szTmp0,szTmp1,szTmp2);
        if (sds_initget(0,NULL)!=RTNORM) {
            rc=ret;
            goto bail;
        }
        if ((ret=sds_getpoint(NULL,prompt,pt0))==RTERROR) {
            rc=ret;
            goto bail;
        }else if (ret==RTCAN){
            rc=ret;
            goto bail;
        }else if (ret==RTNONE){
            ic_ptcpy(pt0,lastpt);
        }
        if (sds_initget(1,ResourceString(IDC_3DSOLIDS_INITGET_CUBE_L_84, "Cube Length_of_side|Length ~_Cube ~_Length" ) )!=RTNORM) {
            rc=ret;
            goto bail;
        }
		SDS_getvar(NULL,DB_QDRAGMODE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
       if ((ret=SDS_dragobject(21,setgetrb.resval.rint,pt0,NULL,0.0,ResourceString(IDC_3DSOLIDS__NCUBE_LENGTH__85, "\nCube/Length/<Corner of wedge>: " ) ,NULL,pt1,NULL))==RTERROR) {
        //if ((ret=sds_getcorner(pt0,"\nCube/Length/<Corner of wedge>: ",pt1))==RTERROR)
            rc=ret;
            goto bail;
        }else if (ret==RTCAN){
            rc=ret;
            goto bail;
        }else if (ret==RTNONE){
            ic_ptcpy(pt1,lastpt);
        }else if (ret==RTKWORD){
            sds_getinput(fs1);
            if (strisame("CUBE"/*DNT*/ ,fs1)) {
                if (sds_initget(3,NULL)!=RTNORM) {rc=ret;goto bail;}
                if ((ret=sds_getdist(NULL,ResourceString(IDC_3DSOLIDS__NLENGTH_OF_SI_87, "\nLength of sides of cubic wedge: " ) ,&len))!=RTNORM) {
                    rc=ret;
                    goto bail;
                }
                pt1[0]=pt0[0]-len/2.0;
                pt1[1]=pt0[1]-len/2.0;
                pt1[2]=pt0[2]-len/2.0;
                pt2[0]=pt0[0]+len/2.0;
                pt2[1]=pt0[1]+len/2.0;
                pt2[2]=pt0[2]-len/2.0;
                wth=high=len;
                if ((ret=cmd_wedge_create(pt1,pt2,len,0.0))!=RTNORM) {rc=ret;goto bail;}
            }else if (strisame("LENGTH"/*DNT*/ ,fs1)) {
                if (sds_initget(3,NULL)!=RTNORM) {
                    rc=ret;
                    goto bail;
                }
                if ((ret=sds_getdist(NULL,ResourceString(IDC_3DSOLIDS__NLENGTH_OF_SI_75, "\nLength of side: " ) ,&len))!=RTNORM) {
                    rc=ret;
                    goto bail;
                }
                if (sds_initget(3,NULL)!=RTNORM) {
                    rc=ret;
                    goto bail;
                }
                if ((ret=sds_getdist(NULL,ResourceString(IDC_3DSOLIDS__NWIDTH_ACROSS_89, "\nWidth across wedge: " ) ,&wth))!=RTNORM) {
                    rc=ret;
                    goto bail;
                }
                if (sds_initget(3,NULL)!=RTNORM) {
                    rc=ret;
                    goto bail;
                }
                if ((ret=sds_getdist(NULL,ResourceString(IDC_3DSOLIDS__NHEIGHT_OF_WE_77, "\nHeight of wedge: " ) ,&high))!=RTNORM) {
                    rc=ret;
                    goto bail;
                }
                pt1[0]=pt0[0]-len/2.0;
                pt1[1]=pt0[1]-wth/2.0;
                pt1[2]=pt0[2]-high/2.0;
                pt2[0]=pt0[0]+len/2.0;
                pt2[1]=pt0[1]+wth/2.0;
                pt2[2]=pt0[2]-high/2.0;
                if ((ret=cmd_wedge_create(pt1,pt2,high,0.0))!=RTNORM) {rc=ret;goto bail;}
            }
        }else if (ret==RTNORM){
            ic_ptcpy(lastpt,pt1);
            if (!icadRealEqual(pt0[2],pt1[2])){  //if Z's are not equal use dif for height
                high=(pt1[2]-pt0[2])*2.0;
            }else{
                if (sds_initget(3,NULL)!=RTNORM) {
                    rc=ret;
                    goto bail;
                }
                if ((ret=sds_getdist(NULL,ResourceString(IDC_3DSOLIDS__NHEIGHT_OF_WE_77, "\nHeight of wedge: " ) ,&high))!=RTNORM) {
                    rc=ret;
                    goto bail;
                }
            }
            pt2[0]=pt0[0]-(pt1[0]-pt0[0]);
            pt2[1]=pt0[1]-(pt1[1]-pt0[1]);
            pt1[2]=pt2[2]=pt0[2]-(high/2.0);
            if ((ret=cmd_wedge_create(pt1,pt2,high,0.0))!=RTNORM) {
                rc=ret;
                goto bail;
            }
        }
    }else if ((ret==RTNORM)||(ret==RTNONE)){
        if (ret==RTNONE){
            ic_ptcpy(pt1,lastpt);
        }
        ic_ptcpy(lastpt,pt1);
        if (sds_initget(1,ResourceString(IDC_3DSOLIDS_INITGET_CUBE_L_84, "Cube Length_of_side|Length ~_Cube ~_Length" ) )!=RTNORM) {
            rc=ret;
            goto bail;
        }

		SDS_getvar(NULL,DB_QDRAGMODE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
       if ((ret=SDS_dragobject(21,setgetrb.resval.rint,pt1,NULL,0.0,ResourceString(IDC_3DSOLIDS__NCUBE_LENGTH__90, "\nCube/Length/<Opposite corner>: " ) ,NULL,pt2,NULL))==RTERROR) {
        //if ((ret=sds_getcorner(pt1,"\nCube/Length/<Opposite corner>: ",pt2))==RTERROR)
            rc=ret;
            goto bail;
        }else if (ret==RTCAN){
            rc=ret;
            goto bail;
        }else if (ret==RTKWORD){
            sds_getinput(fs1);
            if (strisame("CUBE"/*DNT*/ ,fs1)) {
                if (sds_initget(3,NULL)!=RTNORM) {
                    rc=ret;
                    goto bail;
                }
                if ((ret=sds_getdist(NULL,ResourceString(IDC_3DSOLIDS__NLENGTH_OF_SI_87, "\nLength of sides of cubic wedge: " ) ,&len))!=RTNORM) {
                    rc=ret;
                    goto bail;
                }
                pt2[0]=pt1[0]+len;
                pt2[1]=pt1[1]+len;
                pt2[2]=pt1[2];
                if ((ret=cmd_wedge_create(pt1,pt2,len,0.0))!=RTNORM) {rc=ret;goto bail;}
            }else if (strisame("LENGTH"/*DNT*/ ,fs1)) {
                if (sds_initget(3,NULL)!=RTNORM) {
                    rc=ret;
                    goto bail;
                }
                if ((ret=sds_getdist(NULL,ResourceString(IDC_3DSOLIDS__NLENGTH_OF_SI_75, "\nLength of side: " ) ,&len))!=RTNORM) {
                    rc=ret;
                    goto bail;
                }
                if (sds_initget(3,NULL)!=RTNORM) {rc=ret;goto bail;}
                if ((ret=sds_getdist(NULL,ResourceString(IDC_3DSOLIDS__NWIDTH_OF_WED_76, "\nWidth of wedge: " ) ,&wth))!=RTNORM) {
                    rc=ret;
                    goto bail;
                }
                if (sds_initget(3,NULL)!=RTNORM) {rc=ret;goto bail;}
                if ((ret=sds_getdist(NULL,ResourceString(IDC_3DSOLIDS__NHEIGHT_OF_WE_77, "\nHeight of wedge: " ) ,&high))!=RTNORM) {
                    rc=ret;
                    goto bail;
                }
                pt2[0]=pt1[0]+len;
                pt2[1]=pt1[1]+wth;
                pt2[2]=pt1[2];
                if ((ret=cmd_wedge_create(pt1,pt2,high,0.0))!=RTNORM) {
                    rc=ret;
                    goto bail;
                }
            }
        }else if (ret==RTNORM){
            if (!icadRealEqual(pt1[2],pt2[2])){  //if Z's are not equal use dif for height
                high=pt2[2]-pt1[2];
                pt2[2]=pt1[2];
            }else{
				ic_ptcpy(pt0,pt1);
				pt0[0]=pt2[0];
				sds_grdraw(pt1,pt0,-1,0);
				sds_grdraw(pt2,pt0,-1,0);
				pt0[0]=pt1[0];
				pt0[1]=pt2[1];
				sds_grdraw(pt1,pt0,-1,0);
				sds_grdraw(pt2,pt0,-1,0);
                if (sds_initget(3,NULL)!=RTNORM) {
                    rc=ret;
                    goto bail;
                }
                if ((ret=sds_getdist(pt1,ResourceString(IDC_3DSOLIDS__NHEIGHT_OF_WE_77, "\nHeight of wedge: " ) ,&high))!=RTNORM) {
                    rc=ret;
                    goto bail;
                }
				sds_grdraw(pt1,pt0,-1,0);
				sds_grdraw(pt2,pt0,-1,0);
				pt0[0]=pt2[0];
				pt0[1]=pt1[1];
				sds_grdraw(pt1,pt0,-1,0);
				sds_grdraw(pt2,pt0,-1,0);
            }
            if ((ret=cmd_wedge_create(pt1,pt2,high,0.0))!=RTNORM) {
                rc=ret;
                goto bail;
            }
        }
    }

	//get last point
	setgetrb.restype=RT3DPOINT;
	ic_ptcpy(setgetrb.resval.rpoint,lastpt);
	SDS_setvar(NULL,DB_QLASTPOINT,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);

    rc=RTNORM;
bail:
    return(rc);
}

/*-------------------------------------------------------------------------*//**
Implementation of AI_BOX
@return RTNORM - for success; others - for error
*//*--------------------------------------------------------------------------*/
short cmd_box_mesh()
{
	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	char prompt[IC_ACADBUF];
	sds_point pt1,pt2,lastpt;
	sds_real len,wth,high,ang;
	RT ret,rc;
	struct resbuf setgetrb,rbucs,rbwcs;
	struct cmd_drag_globalpacket *gpak;
	char szTmp0[IC_ACADBUF],szTmp1[IC_ACADBUF],szTmp2[IC_ACADBUF];
	sds_name elast,ss;
	sds_matrix mtx;

	rbucs.restype=rbwcs.restype=RTSHORT;
	rbucs.resval.rint=1;
	rbwcs.resval.rint=0;

	//get last point
	if ((ret=SDS_getvar(NULL,DB_QLASTPOINT,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM) {rc=ret;goto bail;}
	ic_ptcpy(lastpt,setgetrb.resval.rpoint);

	sds_rtos((fabs(lastpt[0])<CMD_FUZZ)?0.0:lastpt[0],-1,-1,szTmp0);
	sds_rtos((fabs(lastpt[1])<CMD_FUZZ)?0.0:lastpt[1],-1,-1,szTmp1);
	sds_rtos((fabs(lastpt[2])<CMD_FUZZ)?0.0:lastpt[2],-1,-1,szTmp2);

	{	// Order for 3D-Box is very different from just Box.  This handles the 3D way:
		// Note - there are a lot of ways we could improve on this (accepting defaults on NULL response, etc.), but
		// it's only supported for compatibility - not worth a lot of trouble to improve it.

       sprintf(prompt,ResourceString(IDC_3DSOLIDS__NCORNER_OF_BO_92, "\nCorner of box <%s,%s,%s>: " ) ,szTmp0,szTmp1,szTmp2);   // Get Corner (accept ENTER for last point)
		if (sds_initget(0,NULL)!=RTNORM) {
            rc=ret;
            goto bail;
        }
		if ((ret=sds_getpoint(NULL,prompt,pt1))==RTERROR) {
			rc=ret;
            goto bail;
		}else if (ret==RTCAN){
			rc=ret;
            goto bail;
		}else if (ret==RTNONE){
			ic_ptcpy(pt1,lastpt);
		}else if (ret==RTNORM){
			ic_ptcpy(lastpt,pt1);
		}

		if (sds_initget(3,NULL)!=RTNORM) {
            rc=ret;
            goto bail;
        }						// Get Length, in one pick
       if ((ret=sds_getdist(pt1,ResourceString(IDC_3DSOLIDS__NLENGTH_OF_SI_93, "\nLength of side of box: " ) ,&len))!=RTNORM) {
           rc=ret;
           goto bail;
       }
		pt2[0]=pt1[0]+len;

       if (sds_initget(3,ResourceString(IDC_3DSOLIDS_INITGET_CUBE___94, "Cube ~_Cube" ) )!=RTNORM) {
           rc=ret;
           goto bail;
       }                  // Get Width, in one pick (or "C" for Cube)
       if ((ret=sds_getdist(pt1,ResourceString(IDC_3DSOLIDS__NCUBE__WIDTH__95, "\nCube/<Width of box>: " ) ,&wth))==RTERROR) {
			rc=ret;
            goto bail;
		}else if (ret==RTCAN){
			rc=ret;
            goto bail;
		}else if (ret==RTKWORD){		// Cube - We already have the length of a side, just get rot angle and draw it.
			pt2[1]=pt1[1]+len;
			pt2[2]=pt1[2];			// make bottom flat with respect to the x,y plane
			if ((ret=cmd_box_create(pt1,pt2,len))!=RTNORM) {rc=ret; goto bail;}   // len = height 'cause it's a cube.
		}else if (ret==RTNORM){
			pt2[1]=pt1[1]+wth;
		    if (sds_initget(3,NULL)!=RTNORM) {
                rc=ret;
                goto bail;
            }						// Get Height, in one pick
           if ((ret=sds_getdist(pt1,ResourceString(IDC_3DSOLIDS__NHEIGHT_OF_BO_96, "\nHeight of box: " ) ,&high))!=RTNORM) {
               rc=ret;
               goto bail;
           }
		    pt2[2]=pt1[2];		// make bottom flat with respect to the x,y plane
		    if ((ret=cmd_box_create(pt1,pt2,high))!=RTNORM) {
                rc=ret;
                goto bail;
            }
        }

		gpak=&cmd_drag_gpak;

		//get the rotation angle for the rectang
		ic_ptcpy(gpak->pt1,pt1); //pt1 is wcs
		ic_ptcpy(gpak->lastpt,pt1);
		gpak->reference=0;
		gpak->refval=0.0;
		gpak->rband=0;
		sds_entlast(elast);
		sds_ssadd(elast,NULL,ss);

        *szTmp2=0;
        strcpy(szTmp0,ResourceString(IDC_3DSOLIDS__NROTATION_ANG_97, "\nRotation angle for box: " ) );
        while(1) {
            if ((ret=cmd_drag(CMD_ROTATE_DRAG,ss,szTmp0,szTmp2,pt2,szTmp1))==RTERROR){
                goto bail;
            }else if (ret==RTCAN) {
                goto bail;
            }else if (ret==RTNORM) {
                 ang=sds_angle(pt1,pt2);
                break;
            }else if (ret==RTSTR){
                sds_angtof(szTmp1,-1,&ang);
                break;
            }else{
                cmd_ErrorPrompt(CMD_ERR_ANG2NDPT,0);
                continue;
            }
        }

        //*** Setup matrix for sds_xformss.
        mtx[0][0]=cos(ang); mtx[0][1]=(-sin(ang)); mtx[0][2]=0.0; mtx[0][3]=((pt1[1]*sin(ang))+(pt1[0]*(1-cos(ang))));
        mtx[1][0]=sin(ang); mtx[1][1]=cos(ang);    mtx[1][2]=0.0; mtx[1][3]=(-(pt1[0]*sin(ang))+(pt1[1]*(1-cos(ang))));
        mtx[2][0]=0.0;      mtx[2][1]=0.0;         mtx[2][2]=1.0; mtx[2][3]=0.0;
        mtx[3][0]=0.0;      mtx[3][1]=0.0;         mtx[3][2]=0.0; mtx[3][3]=1.0;
        //***
        if ((ret=SDS_progxformss(ss,mtx,0))!=RTNORM)
            goto bail;
  		SDS_BitBlt2Scr(0);

    }

	//get last point
	setgetrb.restype=RT3DPOINT;
	ic_ptcpy(setgetrb.resval.rpoint,lastpt);
	SDS_setvar(NULL,DB_QLASTPOINT,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);

    rc=RTNORM;
bail:
    sds_ssfree(ss);
    return(rc);
}

short cmd_wedge3d()
{
    if (cmd_iswindowopen() == RTERROR) return RTERROR;

	char prompt[IC_ACADBUF];
	char szTmp0[IC_ACADBUF],szTmp1[IC_ACADBUF],szTmp2[IC_ACADBUF];
    sds_point pt1,pt2,lastpt;
	sds_real len,wth,high,ang;
	RT ret,rc;
	struct resbuf setgetrb,rbucs,rbwcs;
	struct cmd_drag_globalpacket *gpak;
    sds_name elast,ss;
    sds_matrix mtx;


	rbucs.restype=rbwcs.restype=RTSHORT;
	rbucs.resval.rint=1;
	rbwcs.resval.rint=0;

	//get last point
	if ((ret=SDS_getvar(NULL,DB_QLASTPOINT,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM) {rc=ret;goto bail;}
	ic_ptcpy(lastpt,setgetrb.resval.rpoint);

	sds_rtos((fabs(lastpt[0])<CMD_FUZZ)?0.0:lastpt[0],-1,-1,szTmp0);
	sds_rtos((fabs(lastpt[1])<CMD_FUZZ)?0.0:lastpt[1],-1,-1,szTmp1);
	sds_rtos((fabs(lastpt[2])<CMD_FUZZ)?0.0:lastpt[2],-1,-1,szTmp2);
    sprintf(prompt,ResourceString(IDC_3DSOLIDS_CORNER_OF_WEDG_74, "Corner of wedge <%s,%s,%s>: " ) ,szTmp0,szTmp1,szTmp2);
    if (sds_initget(0,NULL)!=RTNORM) {
        rc=ret;
        goto bail;
    }
	if ((ret=sds_getpoint(NULL,prompt,pt1))==RTERROR) {
	    rc=ret;
        goto bail;
	}else if (ret==RTCAN){
	    rc=ret;
        goto bail;
    }else if ((ret==RTNORM)||(ret==RTNONE)){
        if (ret==RTNONE){
            ic_ptcpy(pt1,lastpt);
        }
        ic_ptcpy(lastpt,pt1);
        //TODO should have dragobject with ortho for these.
        if (sds_initget(3,NULL)!=RTNORM) {
            rc=ret;
            goto bail;
        }
        if ((ret=sds_getdist(pt1,ResourceString(IDC_3DSOLIDS__NLENGTH_OF_SI_75, "\nLength of side: " ) ,&len))!=RTNORM) {
            rc=ret;
            goto bail;
        }
        if (sds_initget(3,NULL)!=RTNORM) {rc=ret;goto bail;}
        if ((ret=sds_getdist(pt1,ResourceString(IDC_3DSOLIDS__NWIDTH_OF_WED_76, "\nWidth of wedge: " ) ,&wth))!=RTNORM) {
            rc=ret;
            goto bail;
        }
        if (sds_initget(3,NULL)!=RTNORM) {
            rc=ret;
            goto bail;
        }
        if ((ret=sds_getdist(pt1,ResourceString(IDC_3DSOLIDS__NHEIGHT_OF_WE_77, "\nHeight of wedge: " ) ,&high))!=RTNORM) {
            rc=ret;
            goto bail;
        }
        pt2[0]=pt1[0]+len;
        pt2[1]=pt1[1]+wth;
        pt2[2]=pt1[2];

        if ((ret=cmd_wedge_create(pt1,pt2,high,0.0))!=RTNORM) {
            rc=ret;
            goto bail;
        }


		gpak=&cmd_drag_gpak;

		//get the rotation angle for the rectang
		ic_ptcpy(gpak->pt1,pt1); //pt1 is wcs
		ic_ptcpy(gpak->lastpt,pt1);
		gpak->reference=0;
		gpak->refval=0.0;
		gpak->rband=0;
		sds_entlast(elast);
		sds_ssadd(elast,NULL,ss);

        strcpy(szTmp2,ResourceString(IDC_3DSOLIDS_REFERENCE_78, "~Reference ~_Reference" ) );
        strcpy(szTmp0,ResourceString(IDC_3DSOLIDS__NROTATION_ANG_79, "\nRotation angle for wedge: " ) );
        while(1) {
            if (gpak->reference) {
                strcpy(szTmp0,ResourceString(IDC_3DSOLIDS__NNEW_ANGLE___80, "\nNew angle: " ) );
            }
            if ((ret=cmd_drag(CMD_ROTATE_DRAG,ss,szTmp0,szTmp2,pt2,szTmp1))==RTERROR){
                goto bail;
            }else if (ret==RTCAN) {
                goto bail;
            }else if (ret==RTNORM) {
		        sds_trans(pt1,&rbwcs,&rbucs,0,pt1); //convert pt1 into ucs for angle calc
                ang=sds_angle(pt1,pt2);
                break;
            }else if (ret==RTSTR) {
                if (strsame(ResourceString(IDC_3DSOLIDS_REFERENCE_78, "Reference" ) ,szTmp1)) {
                    while(1) {
                        sds_initget(RSG_OTHER,NULL);
                        if ((ret=sds_getangle(NULL,ResourceString(IDC_3DSOLIDS__NBASE_ANGLE___81, "\nBase angle <0>: " ) ,&gpak->refval))==RTERROR) {
                            goto bail;
                        }else if (ret==RTCAN){
                            goto bail;
                        }else if (ret==RTNONE){
                            gpak->refval=0;
                            break;
                        }else if (ret==RTNORM){
                            break;
                        }else{
					        cmd_ErrorPrompt(CMD_ERR_ANG2PTS,0);
                            continue;
                        }
                        break;
                    }
                    gpak->reference=1;
                    continue;
                }else if (ic_isnumstr(szTmp1,&ang)){
                    ang=(ang*IC_PI/180);
                    break;
                }else{
                    if (!gpak->reference) cmd_ErrorPrompt(CMD_ERR_ANG2NDPTKWORD,0);
                    else cmd_ErrorPrompt(CMD_ERR_ANG2NDPT,0);
                    continue;
                }
            }else{
                if (!gpak->reference) cmd_ErrorPrompt(CMD_ERR_ANG2NDPTKWORD,0);
                else cmd_ErrorPrompt(CMD_ERR_ANG2NDPT,0);
                continue;
            }
        }

        //*** Setup matrix for sds_xformss.
        if (gpak->reference) ang-=gpak->refval;
        mtx[0][0]=cos(ang); mtx[0][1]=(-sin(ang)); mtx[0][2]=0.0; mtx[0][3]=((pt1[1]*sin(ang))+(pt1[0]*(1-cos(ang))));
        mtx[1][0]=sin(ang); mtx[1][1]=cos(ang);    mtx[1][2]=0.0; mtx[1][3]=(-(pt1[0]*sin(ang))+(pt1[1]*(1-cos(ang))));
        mtx[2][0]=0.0;      mtx[2][1]=0.0;         mtx[2][2]=1.0; mtx[2][3]=0.0;
        mtx[3][0]=0.0;      mtx[3][1]=0.0;         mtx[3][2]=0.0; mtx[3][3]=1.0;
        //***
        if ((ret=SDS_progxformss(ss,mtx,0))!=RTNORM) goto bail;
		SDS_BitBlt2Scr(0);
    }

   //get last point
	setgetrb.restype=RT3DPOINT;
	ic_ptcpy(setgetrb.resval.rpoint,lastpt);
	SDS_setvar(NULL,DB_QLASTPOINT,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);

    rc=RTNORM;
bail:
	sds_ssfree(ss);
    return(rc);
}


/*------------------------------------------------------------------*\
functions producing solids (exception: see box-related funcs)
\*------------------------------------------------------------------*/


/*-----------------------------------------------------------------*\
Test if two lines are intersected on XY plane
@author Stephen Hou
@return true if lines are intersected and false otherwise
\*------------------------------------------------------------------*/
bool linesXYIntersected(sds_point start1, sds_point end1,
					    sds_point start2, sds_point end2)
{
	double a = (end1[1] - start1[1]) * (end2[0] - start2[0]) -
		       (end1[0] - start1[0]) * (end2[1] - start2[1]);

	if (icadIsZero(a))
		return false;

	double b = end1[0] - start1[0];
	if (icadIsZero(b))
		return false;

	double t2 = ((end2[0] - start1[0]) * (start2[1] - start1[1]) -
		         (end1[1] - start1[1]) * (start2[0] - start1[0])) / a;

	if (t2 < 0.0 || t2 > 1.0)
		return false;

	double t1 = ((end2[0] - start2[0]) * t2 + (start2[0] - start1[0])) / b;
	if (t1 < 0. || t1 > 1.)
		return false;

	return true;
}


bool arePointsCoplanar(sds_point pt1, sds_point pt2, sds_point pt3, sds_point pt)
{
	gvector a(pt2[0] - pt1[0], pt2[1] - pt1[1], pt2[2] - pt1[2]);
	gvector b(pt3[0] - pt1[0], pt3[1] - pt1[1], pt3[2] - pt1[2]);
	gvector c(pt[0] - pt1[0], pt[1] - pt1[1], pt[2] - pt1[2]);

	return icl::perpendicular(a * b, c);
}


/*-------------------------------------------------------------------------*//**
Create solid pyramid
@author Alexey Malov
@return RTNORM - for success; RTERROR - for error
*//*--------------------------------------------------------------------------*/
short cmd_pyramid_solid_create
(
int mode,
const point& bpt1,
const point& bpt2,
const point& bpt3,
const point& bpt4,
const point& tpt1,
const point& tpt2,
const point& tpt3,
const point& tpt4,
int *undraw
)
{
	//MODE: 0 - Draw pyramid w/4 distinct top and bottom pts, all parameters used
	//		1 - Draw ridge pyramid using 4 base pts and tpt1 & tpt2.  If tpt1 and tpt2
	//				are equal, we get square "egyptian" pyramid, but this is really done
	//				the same way.
	//		2 - Draw tetrahedron using bpt1,bpt2,bpt3, and tpt1.
	//		3 - Draw pyramid w/3 distinct top and bottom pts (don't use bpt4 & tpt4)

	//NOTE:  ALL pts assumed to be ucs.  Plines made in WCS!
	//		 Undraw bits are for clearing grdraw done in cmd_pyramid
	if ((*undraw)&1)	sds_grdraw(bpt1,bpt2,-1,0);
	if ((*undraw)&2)	sds_grdraw(bpt2,bpt3,-1,0);
	if ((*undraw)&4)	sds_grdraw(bpt3,bpt4,-1,0);
	if ((*undraw)&8)	sds_grdraw(bpt4,bpt1,-1,0);
	if ((*undraw)&16)sds_grdraw(bpt3,bpt1,-1,0);
	if ((*undraw)&32)sds_grdraw(tpt1,tpt2,-1,0);
	if ((*undraw)&64)sds_grdraw(tpt2,tpt3,-1,0);
	if ((*undraw)&128)sds_grdraw(bpt1,tpt1,-1,0);
	if ((*undraw)&256)sds_grdraw(bpt2,tpt2,-1,0);
	if ((*undraw)&512)sds_grdraw(bpt3,tpt3,-1,0);
	if ((*undraw)&1024)sds_grdraw(bpt4,tpt1,-1,0);
	*undraw=0;

    transf mtx;
    if (!ucs2wcs(mtx))
        return RTERROR;

	CDbAcisEntityData obj;
    if (!CModeler::get()->createPyramid(bpt1, bpt2, bpt3, bpt4,
                                        tpt1, tpt2, tpt3, tpt4, mode, mtx, getCurrColorIndex(), obj))
        return RTERROR;

	sds_name ename;
	if (create(obj, ename))
		return RTNORM;

	return RTERROR;
}

enum EDomeMode
{
	eSphere = 0,
	eDome = 1,
	eDish = 2
};

/*-------------------------------------------------------------------------*//**
Create solid sphere, dome or dish
@author Alexey Malov
@return RTNORM - for success; RTERROR - for error
*//*--------------------------------------------------------------------------*/
short cmd_sphere_solid_create
(
const point& center,
sds_real rad,
int domemode
)
{
    transf mtx;
    if (!ucs2wcs(mtx))
        return RTERROR;

	CDbAcisEntityData obj;
	int rc = 0;
    switch (domemode)
    {
    case eDish:
        rc = CModeler::get()->createDish(center, rad, mtx, getCurrColorIndex(), obj);
        break;
    case eDome:
        rc = CModeler::get()->createDome(center, rad, mtx, getCurrColorIndex(), obj);
        break;
    case eSphere:
        rc = CModeler::get()->createSphere(center, rad, mtx, getCurrColorIndex(), obj);
        break;
	default:
		assert(false);
		return RTERROR;
    }
	if (rc)
	{
		sds_name ename;
		if (create(obj, ename))
			return RTNORM;
	}
	return RTERROR;
}

/*-------------------------------------------------------------------------*//**
Create solid torus
@author Alexey Malov
@return RTNORM - for success; RTERROR - for error
*//*--------------------------------------------------------------------------*/
short cmd_torus_solid_create
(
const point& center,
sds_real tordia,
sds_real tubedia
)
{
    transf mtx;
    if (!ucs2wcs(mtx))
        return RTERROR;

	CDbAcisEntityData obj;
    if (!CModeler::get()->createTorus(center, 0.5*tordia, 0.5*tubedia, mtx, getCurrColorIndex(), obj))
        return RTERROR;

	sds_name ename;
	if (create(obj, ename))
		return RTNORM;
	return RTERROR;
}

/*-------------------------------------------------------------------------*//**
Create solid elliptical cone
@author Alexey Malov
@return RTNORM - for success; RTERROR - for error
*//*--------------------------------------------------------------------------*/
short cmd_cone_solid_create_elp
(
const point& cent,
const gvector& v1,
sds_real ratio,
const point& pthigh
)
{
    transf mtx;
    if (!ucs2wcs(mtx))
        return RTERROR;

	CDbAcisEntityData obj;
    if (!CModeler::get()->createCone(cent, v1, ratio, pthigh, mtx, getCurrColorIndex(), obj))
        return RTERROR;

	sds_name ename;
	if (create(obj, ename))
		return RTNORM;
	return RTERROR;
}

/*-------------------------------------------------------------------------*//**
Create solid cone
@author Alexey Malov
@return RTNORM - for success; RTERROR - for error
*//*--------------------------------------------------------------------------*/
short cmd_cone_solid_create
(
const point& center,
sds_real bdia,
sds_real tdia,
const point& pthigh
)
{
    transf mtx;
    if (!ucs2wcs(mtx))
        return RTERROR;

	CDbAcisEntityData obj;
    if (!CModeler::get()->createCone(center, 0.5*bdia, pthigh, 0.5*tdia, mtx, getCurrColorIndex(), obj))
        return RTERROR;

	sds_name ename;
	if (create(obj, ename))
		return RTNORM;
	return RTERROR;
}

/*-------------------------------------------------------------------------*//**
Create solid elliptical cylinder
@author Alexey Malov
@return RTNORM - for success; RTERROR - for error
*//*--------------------------------------------------------------------------*/
short cmd_cyl_solid_create_elp
(
const point& center,
const gvector& axis,
sds_real ratio,
const point& pthigh
)
{
    transf mtx;
    if (!ucs2wcs(mtx))
        return RTERROR;

	CDbAcisEntityData obj;
    if (!CModeler::get()->createCylinder(center, axis, ratio, pthigh, mtx, getCurrColorIndex(), obj))
        return RTERROR;

	sds_name ename;
	if (create(obj, ename))
		return RTNORM;

	return RTERROR;
}

/*-------------------------------------------------------------------------*//**
Create solid cylinder
@author Alexey Malov
@return RTNORM - for success; RTERROR - for error
*//*--------------------------------------------------------------------------*/
short cmd_cyl_solid_create
(
const point& center,
sds_real dia,
const point& pthigh
)
{
    //pts assumed in ucs, pline generated in wcs
    transf mtx;
    if (!ucs2wcs(mtx))
        return RTERROR;

	CDbAcisEntityData obj;
    if (!CModeler::get()->createCylinder(center, 0.5*dia, pthigh, mtx, getCurrColorIndex(), obj))
        return RTERROR;

	sds_name ename;
	if (create(obj, ename))
		return RTNORM;
	return RTERROR;
}

/*-------------------------------------------------------------------------*//**
Create solid wedge
@author Alexey Malov
@return RTNORM - for success; RTERROR - for error
*//*--------------------------------------------------------------------------*/
short cmd_wedge_solid_create
(
const point& corner1,
const point& corner2,
sds_real height
)
{
	//all pts assumed ucs, entity created in wcs
    transf mtx;
    if (!ucs2wcs(mtx))
        return RTERROR;

	CDbAcisEntityData obj;
    if (!CModeler::get()->createWedge(corner1, corner2, height, mtx, getCurrColorIndex(), obj))
        return RTERROR;

	sds_name ename;
	return create(obj, ename)? RTNORM: RTERROR;
}


/*-------------------------------------------------------------------------*//**
Create solid box
@author Alexey Malov
@return RTNORM - for success; RTERROR - for error
*//*--------------------------------------------------------------------------*/
short cmd_box_solid_create
(
const point& corner1,
const point& corner2
)
{
    transf mtx;
    if (!ucs2wcs(mtx))
        return RTERROR;

	CDbAcisEntityData obj;

    if (!CModeler::get()->createBlock(corner1, corner2, mtx, getCurrColorIndex(), obj))
        return RTERROR;

	sds_name ename;
	if (create(obj, ename))
		return RTNORM;
	return RTERROR;
}

/*-------------------------------------------------------------------------*//**
Create solid or mesh box regarding on ACIS availableness
@author Dmitry Gavrilov
@return RTNORM - for success; RTERROR - for error
*//*--------------------------------------------------------------------------*/
short cmd_box_solid_or_mesh_create
(
 const point& corner1,
 const point& corner2
)
{
	if(CModeler::checkFullModeler())
		return cmd_box_solid_create(corner1, corner2);
	else
	{
		sds_point	pt1 = {corner1.x(), corner1.y(), corner1.z()},
					pt2 = {corner2.x(), corner2.y(), corner1.z()};
		return cmd_box_create(pt1, pt2, fabs(corner2.z() - corner1.z()));
	}
}

/*-------------------------------------------------------------------------*//**
Commands producing solids
*//*--------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*//**
Implementation of command PYRAMID
@author Alexey Malov
@return RTNORM - for success; RTERROR - for error
*//*--------------------------------------------------------------------------*/
short cmd_pyramid()
{
	int rc;
    if (cmd_iswindowopen() == RTERROR)
		return RTERROR;
    if (!CModeler::checkFullModeler()) {
        cmd_acad_compatible = FALSE;
		rc = cmd_pyramid_mesh();
        cmd_acad_compatible = TRUE;
        return rc;
    }

	//all grdraws done within f'n manually and then cleared by cmd_pyramid_create
	//	prior to entmake.  For details on which bit is between which two points,
	//	see bail at end of function.

	int diddraw=0;
	point bpt1, bpt2, bpt3, bpt4, tpt1, tpt2, tpt3, tpt4;
	do try
	{
		int ret;

		//query first base point
		checkSuccess(sds_initget(1,NULL), RTNORM, 0);
		checkSuccess(sds_getpoint(NULL,
			ResourceString(IDC_3DSOLIDS__NFIRST_POINT_F_0, "\nFirst point for base of pyramid: "),
			bpt1),
			RTNORM, 0);

		//query second base point
		for (;;) {
			checkSuccess(sds_initget(1,NULL), RTNORM, 0);
			checkSuccess(sds_getpoint(bpt1,
				ResourceString(IDC_3DSOLIDS__NSECOND_POINT__1, "\nSecond point: "),
				bpt2),
				RTNORM, 0);

			bpt2[2] = bpt1[2];
			if (icadPointEqual(bpt1, bpt2)) {
				cmd_ErrorPrompt(IDC_CMDS4__TIDENTICAL_POIN_143, 0);
				continue;
			}
			else
				break;
		}

		sds_grdraw(bpt1,bpt2,-1,0);
		diddraw|=1;

		//query third base point
		for (;;) {
			checkSuccess(sds_initget(1,NULL), RTNORM, 0);
			checkSuccess(sds_getpoint(bpt2,
				ResourceString(IDC_3DSOLIDS__NTHIRD_POINT___2, "\nThird point: "),
				bpt3),
				RTNORM, 0);

			bpt3[2] = bpt1[2];
			if (icadPointEqual(bpt1, bpt3) || icadPointEqual(bpt2, bpt3)) {
				cmd_ErrorPrompt(IDC_CMDS4__TIDENTICAL_POIN_143, 0);
				continue;
			}
			else
				break;
		}
		sds_grdraw(bpt2,bpt3,-1,0);
		diddraw|=2;

		//do we build tetrahedron?
		bool done = false;
		for (;;) {
			checkSuccess(sds_initget(0,
				ResourceString(IDC_3DSOLIDS_INITGET_TETRAHE_3, "Tetrahedron ~_Tetrahedron")),
				RTNORM, 0);

			resbuf rb;
			SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			checkNoFailure(ret=SDS_dragobject(35,rb.resval.rint,bpt1,bpt3,NULL,
				ResourceString(IDC_3DSOLIDS__NTETRAHEDRON___4, "\nTetrahedron/<Last point for base>: "),
				NULL,bpt4,NULL),
				RTERROR, RTCAN, 0);

			if (ret==RTKWORD || ret==RTNONE)
			{
				//tetrahedron
				point apex;
				sds_grdraw(bpt1,bpt3,-1,0);
				diddraw|=16;
				for (;;) {
					checkSuccess(sds_initget(1,
						ResourceString(IDC_3DSOLIDS_INITGET_TOP_SUR_5, "Top_surface|Top ~_Top")),
						RTNORM, 0);

					checkNoFailure(ret=sds_getpoint(NULL,
						ResourceString(IDC_3DSOLIDS__NTOP_SURFACE___6, "\nTop surface/<Apex of tetrahedron>: "),
						apex),
						RTERROR, RTCAN);

					if (ret==RTKWORD)
					{
						//top surface

						//query first point of top surface
						double displayment = 0.0;
						for (;;) {
							checkSuccess(sds_initget(1,NULL), RTNORM, 0);
							checkSuccess(ret=sds_getpoint(bpt1,
								ResourceString(IDC_3DSOLIDS__NFIRST_POINT_O_7, "\nFirst point on top surface of tetrahedron: "),
								tpt1),
								RTNORM, 0);

							displayment = tpt1[2] - bpt1[2];
							if (icadIsZero(displayment)) {
								cmd_ErrorPrompt(CMD_ERR_POINT_ON_PYRAMID_BASE, 0);
								continue;
							}
							break;
						}
						sds_grdraw(bpt1,tpt1,-1,0);
						diddraw|=128;

						//query second point of top surface
						for (;;) {
							checkSuccess(sds_initget(1,NULL), RTNORM, 0);
							checkSuccess(ret=SDS_dragobject(35,rb.resval.rint,tpt1,bpt2,NULL,
								ResourceString(IDC_3DSOLIDS__NSECOND_POINT__1, "\nSecond point: "),
								NULL,tpt2,NULL),
								RTNORM, 0);

							double dis = tpt2[2] - bpt1[2];
							if (icadIsZero(dis)) {
								cmd_ErrorPrompt(CMD_ERR_POINT_ON_PYRAMID_BASE, 0);
								continue;
							}
							if (dis * displayment < 0.0) {
								cmd_ErrorPrompt(CMD_ERR_SELFINTERSECTED, 0);
								continue;
							}
							if (icadPointEqual(tpt1, tpt2)) {
								cmd_ErrorPrompt(IDC_CMDS4__TIDENTICAL_POIN_143, 0);
								continue;
							}
							if (!arePointsCoplanar(bpt1, bpt2, tpt1, tpt2)) {
								cmd_ErrorPrompt(CMD_ERR_POINT_NOT_ON_FACE_PLANE, 0);
								continue;
							}
							break;
						}
						sds_grdraw(bpt2,tpt2,-1,0);
						diddraw|=256;
						sds_grdraw(tpt1,tpt2,-1,0);
						diddraw|=32;

						//query last point of top surface
						for (;;) {
							checkSuccess(sds_initget(1,NULL), RTNORM, 0);
							checkSuccess(ret=SDS_dragobject(35,rb.resval.rint,tpt1,tpt2,NULL,
								ResourceString(IDC_3DSOLIDS__NLAST_POINT_OF_8, "\nLast point of top surface: "),
								NULL,tpt3,NULL),
								RTNORM, 0);

							double dis = tpt3[2] - bpt1[2];
							if (icadIsZero(dis)) {
								cmd_ErrorPrompt(CMD_ERR_POINT_ON_PYRAMID_BASE, 0);
								continue;
							}
							if (dis * displayment < 0.0) {
								cmd_ErrorPrompt(CMD_ERR_SELFINTERSECTED, 0);
								continue;
							}
							if (icadPointEqual(tpt1, tpt3) || icadPointEqual(tpt2, tpt3)) {
								cmd_ErrorPrompt(IDC_CMDS4__TIDENTICAL_POIN_143, 0);
								continue;
							}
							if (!arePointsCoplanar(bpt2, bpt3, tpt2, tpt3) ||
								!arePointsCoplanar(bpt3, bpt1, tpt3, tpt1)) {
								cmd_ErrorPrompt(CMD_ERR_POINT_NOT_ON_FACE_PLANE, 0);
								continue;
							}
							break;
						}

						point dummy;
						checkSuccess(cmd_pyramid_solid_create(3,bpt1,bpt2,bpt3,bpt4,tpt1,tpt2,tpt3,dummy,&diddraw), RTNORM, 0);
						done = true;
					}
					else if (ret==RTNORM)
					{
						if (icadRealEqual(bpt1[2], apex[2])) {
							cmd_ErrorPrompt(CMD_ERR_POINT_ON_PYRAMID_BASE, 0);
							continue;
						}
						//apex of tetrahedron was picked
						point dummy;
						checkSuccess(cmd_pyramid_solid_create(2,bpt1,bpt2,bpt3,bpt4,apex,apex,apex,dummy,&diddraw), RTNORM, 0);
						done = true;
					}
					if (done)
						break;
				}
				if (done)
					break;
			}
			else if (ret==RTNORM)
			{
				bpt4[2] = bpt1[2];
				// test if two lines are intersected. If they are not, the geomery is concave
				// so we will ask users to select again.
				//
				if (!linesXYIntersected(bpt1, bpt3, bpt2, bpt4)) {
					cmd_ErrorPrompt(CMD_ERR_CONCAVE_PYRAMID_BASE, 0);
					continue;
				}

				//last point of pyramid base was picked
				sds_grdraw(bpt3,bpt4,-1,0);
				sds_grdraw(bpt4,bpt1,-1,0);
				diddraw|=(4+8);

				const point zero(0., 0., 0.);
				point base_center = .25*((bpt1 - zero) +
										 (bpt2 - zero) +
										 (bpt3 - zero) +
										 (bpt4 - zero));
				point apex;
				for (;;) {
					checkSuccess(sds_initget(1,
							ResourceString(IDC_3DSOLIDS_INITGET_RIDGE_T_9, "Ridge Top_surface|Top ~_Ridge ~_Top" )),
							RTNORM, 0);
					checkNoFailure(ret=sds_getpoint(base_center,
							ResourceString(IDC_3DSOLIDS__NRIDGE_TOP_SU_10, "\nRidge/Top surface/<Apex of pyramid>: "),
							apex),
							RTERROR, RTCAN);

					if (ret==RTKWORD) {
						char fs1[IC_ACADBUF];
						sds_getinput(fs1);
						if (strisame(fs1,"RIDGE"/*DNT*/)) {
							//query start of the ridge
							double displayment;
							point ridge_start;
							for (;;) {
								checkSuccess(sds_initget(1,NULL), RTNORM, 0);
								checkSuccess(SDS_dragobject(35,rb.resval.rint,bpt1,bpt4,NULL,
										ResourceString(IDC_3DSOLIDS__NSTART_OF_RID_12, "\nStart of ridge: "),
										NULL,ridge_start,NULL),
										RTNORM, 0);

								displayment = ridge_start[2] - bpt1[2];
								if (icadIsZero(displayment)) {
									cmd_ErrorPrompt(CMD_ERR_POINT_ON_PYRAMID_BASE, 0);
									continue;
								}
								break;
							}
							sds_grdraw(bpt1,ridge_start,-1,0);
							tpt1 = tpt4 = ridge_start; // need for erasing lines when the command is cancelled
							diddraw|=128;
							sds_grdraw(bpt4,ridge_start,-1,0);
							diddraw|=1024;

							//query end of the ridge
							point ridge_end;
							for (;;) {
								checkSuccess(sds_initget(1,NULL), RTNORM);
								checkSuccess(sds_getpoint(ridge_start,
										ResourceString(IDC_3DSOLIDS__NEND_OF_RIDGE_13, "\nEnd of ridge: "),
										ridge_end),
										RTNORM, 0);

								double dis = ridge_end[2] - bpt1[2];
								if (icadIsZero(dis)) {
									cmd_ErrorPrompt(CMD_ERR_POINT_ON_PYRAMID_BASE, 0);
									continue;
								}
								if (displayment * dis < 0.0) {
									cmd_ErrorPrompt(CMD_ERR_SELFINTERSECTED, 0);
									continue;
								}

								if (!arePointsCoplanar(bpt1, bpt2, ridge_start, ridge_end) ||
									!arePointsCoplanar(bpt3, bpt4, ridge_end, ridge_start)) {
									cmd_ErrorPrompt(CMD_ERR_POINT_NOT_ON_FACE_PLANE, 0);
									continue;
								}
								break;
							}

							checkSuccess(cmd_pyramid_solid_create(1,bpt1,bpt2,bpt3,bpt4,
									   ridge_start,ridge_end,ridge_end,ridge_start,&diddraw), RTNORM, 0);
							done = true;
						}
						else if (strisame(fs1,"TOP"/*DNT*/))
						{
							//query first top point
							double displayment;
							for (;;) {
								checkSuccess(sds_initget(1,NULL), RTNORM, 0);
								checkSuccess(sds_getpoint(bpt1,
										ResourceString(IDC_3DSOLIDS__NFIRST_POINT__15, "\nFirst point on top surface of pyramid: "),
										tpt1),
										RTNORM, 0);

								double displayment = tpt1[2] - bpt1[2];
								if (icadIsZero(displayment)) {
									cmd_ErrorPrompt(CMD_ERR_POINT_ON_PYRAMID_BASE, 0);
									continue;
								}
							}
							sds_grdraw(bpt1,tpt1,-1,0);
							diddraw|=128;

							//query second top point
							for (;;) {
								checkSuccess(sds_initget(1,NULL), RTNORM, 0);
								checkSuccess(SDS_dragobject(35,rb.resval.rint,tpt1,bpt2,NULL,
										ResourceString(IDC_3DSOLIDS__NSECOND_POINT__1, "\nSecond point: "),
										NULL,tpt2,NULL),
										RTNORM, 0);

								double dis = tpt2[2] - bpt1[2];
								if (icadIsZero(dis)) {
									cmd_ErrorPrompt(CMD_ERR_POINT_ON_PYRAMID_BASE, 0);
									continue;
								}
								if (dis * displayment < 0.0) {
									cmd_ErrorPrompt(CMD_ERR_SELFINTERSECTED, 0);
									continue;
								}
								if (icadPointEqual(tpt1, tpt2)) {
									cmd_ErrorPrompt(IDC_CMDS4__TIDENTICAL_POIN_143, 0);
									continue;
								}
								if (!arePointsCoplanar(bpt1, bpt2, tpt1, tpt2)) {
									cmd_ErrorPrompt(CMD_ERR_POINT_NOT_ON_FACE_PLANE, 0);
									continue;
								}
								break;
							}

							sds_grdraw(tpt1,tpt2,-1,0);
							diddraw|=32;
							sds_grdraw(bpt2,tpt2,-1,0);
							diddraw|=256;


							//query third top point
							for (;;) {
								checkSuccess(sds_initget(1,NULL), RTNORM, 0);
								checkSuccess(SDS_dragobject(35,rb.resval.rint,tpt2,bpt3,NULL,
										ResourceString(IDC_3DSOLIDS__NTHIRD_POINT___2, "\nThird point: "),
										NULL,tpt3,NULL),
										RTNORM, 0);

								double dis = tpt3[2] - bpt1[2];
								if (icadIsZero(dis)) {
									cmd_ErrorPrompt(CMD_ERR_POINT_ON_PYRAMID_BASE, 0);
									continue;
								}
								if (dis * displayment < 0.0) {
									cmd_ErrorPrompt(CMD_ERR_SELFINTERSECTED, 0);
									continue;
								}
								if (icadPointEqual(tpt1, tpt3) || icadPointEqual(tpt2, tpt3)) {
									cmd_ErrorPrompt(IDC_CMDS4__TIDENTICAL_POIN_143, 0);
									continue;
								}
								if (!arePointsCoplanar(bpt2, bpt3, tpt2, tpt3)) {
									cmd_ErrorPrompt(CMD_ERR_POINT_NOT_ON_FACE_PLANE, 0);
									continue;
								}
							}

							sds_grdraw(tpt2,tpt3,-1,0);
							diddraw|=64;
							sds_grdraw(bpt3,tpt3,-1,0);
							diddraw|=512;

							//query last point
							for (;;) {
								checkSuccess(sds_initget(1,NULL), RTNORM, 0);
								checkSuccess(SDS_dragobject(35,rb.resval.rint,tpt1,tpt3,NULL,
										ResourceString(IDC_3DSOLIDS__NLAST_POINT_OF_8, "\nLast point of top surface: "),
										NULL,tpt4,NULL),
										RTNORM, 0);

								double dis = tpt4[2] - bpt1[2];
								if (icadIsZero(dis)) {
									cmd_ErrorPrompt(CMD_ERR_POINT_ON_PYRAMID_BASE, 0);
									continue;
								}
								if (dis * displayment < 0.0) {
									cmd_ErrorPrompt(CMD_ERR_SELFINTERSECTED, 0);
									continue;
								}
								if (icadPointEqual(tpt1, tpt4) || icadPointEqual(tpt2, tpt4) || icadPointEqual(tpt3, tpt4)) {
									cmd_ErrorPrompt(IDC_CMDS4__TIDENTICAL_POIN_143, 0);
									continue;
								}
								if (!arePointsCoplanar(bpt3, bpt4, tpt3, tpt4) ||
									!arePointsCoplanar(bpt4, bpt1, tpt4, tpt1) ||
									!arePointsCoplanar(tpt1, tpt2, tpt3, tpt4)) {
									cmd_ErrorPrompt(CMD_ERR_POINT_NOT_ON_FACE_PLANE, 0);
									continue;
								}
							}

							checkSuccess(cmd_pyramid_solid_create(0,bpt1,bpt2,bpt3,bpt4,
									tpt1,tpt2,tpt3,tpt4,&diddraw), RTNORM, 0);
							done = true;
						}
						if (done)
							break;
					}
					else if (ret==RTNORM)
					{
						double high = apex[2] - bpt1[2];
						if (icadIsZero(high)) {
							if (icadPointEqual(apex, base_center)) {
								cmd_ErrorPrompt(CMD_ERR_POINT_ON_PYRAMID_BASE, 0);
								continue;
							}
							else {
								double xDiff = apex[0] - base_center[0];
								double yDiff = apex[1] - base_center[1];
								apex[2] = sqrt(xDiff * xDiff + yDiff * yDiff);
							}
						}

						//don't need last 3 top pts or last base pt
						point dummy;
						checkSuccess(cmd_pyramid_solid_create(1,bpt1,bpt2,bpt3,bpt4,apex,apex,dummy,dummy,&diddraw), RTNORM, 0);
						done = true;
					}
					if (done)
						break;
				}
			}
			if (done)
				break;
		}
		rc = RTNORM;
	}
	catch (int ret)
	{
		rc = ret;
	}
	while (false);

	if (rc == RTCAN) {
		if (diddraw&1)	sds_grdraw(bpt1,bpt2,-1,0);
		if (diddraw&2)	sds_grdraw(bpt2,bpt3,-1,0);
		if (diddraw&4)	sds_grdraw(bpt3,bpt4,-1,0);
		if (diddraw&8)	sds_grdraw(bpt4,bpt1,-1,0);
		if (diddraw&16)	sds_grdraw(bpt3,bpt1,-1,0);
		if (diddraw&32)	sds_grdraw(tpt1,tpt2,-1,0);
		if (diddraw&64)	sds_grdraw(tpt2,tpt3,-1,0);
		if (diddraw&128)	sds_grdraw(bpt1,tpt1,-1,0);
		if (diddraw&256)	sds_grdraw(bpt2,tpt2,-1,0);
		if (diddraw&512)	sds_grdraw(bpt3,tpt3,-1,0);
		if (diddraw&1024)sds_grdraw(bpt4,tpt1,-1,0);
	}
	return rc;
}

/*-------------------------------------------------------------------------*//**
Implementation of command DOME
@author Alexey Malov
@return RTNORM - for success; RTERROR - for error
*//*--------------------------------------------------------------------------*/
short cmd_dome()
{
    if (cmd_iswindowopen() == RTERROR)
		return RTERROR;

    if (!CModeler::checkFullModeler()) 
		return cmd_dome_mesh();

	try
	{
		//query point
		point center;
		checkSuccess(sds_initget(RSG_NONULL,NULL), RTNORM, 0);
		checkNoFailure(sds_getpoint(NULL,
			ResourceString(IDC_3DSOLIDS__NCENTER_OF_DO_16, "\nCenter of dome: " ),center),
			RTERROR, RTCAN, RTNONE, 0);

		//query diameter or radius
		double rad = 0.;
		for (;;) {
			checkSuccess(sds_initget(RSG_NONULL|RSG_NOZERO,
				ResourceString(IDC_3DSOLIDS_INITGET_DIAMET_17, "Diameter ~_Diameter" )),
				RTNORM, 0);  // RSG_NONEG has no affect on getdist.

			int ret;
			checkNoFailure(ret=sds_getdist(center,
				ResourceString(IDC_3DSOLIDS__NDIAMETER__RA_18, "\nDiameter/<Radius of dome>: "), &rad),
				RTERROR, RTCAN, RTNONE, 0);

			if (ret==RTKWORD)
			{
				//query diameter
				for (;;) {
					checkSuccess(sds_initget(RSG_NONULL|RSG_NOZERO,NULL), RTNORM, 0);
					// RSG_NONEG has no affect on getdist.
					checkSuccess(sds_getdist(center,
						ResourceString(IDC_3DSOLIDS__NDIAMETER_OF__19, "\nDiameter of dome: "), &rad),
						RTNORM, 0);

					// verify input and prompt users to input again if it is not valid.
					//
					if (rad <= 0.0) {
						cmd_ErrorPrompt(IDC_ERRORPROMPT_POSITIVE_VALUE_303, 0);
						continue;
					}
					else
						break;
				}

				rad *= .5;
				break;
			}
			else {
				// verify input and prompt users to input again if it is not valid.
				//
				if (rad <= 0.0) {
					cmd_ErrorPrompt(IDC_ERRORPROMPT_POSITIVE_VALUE_303, 0);
					continue;
				}
				else
					break;
			}
		}
		return checkSuccess(cmd_sphere_solid_create(center, rad, eDome), RTNORM, 0);
	}
	catch (int ret)
	{
		return ret;
	}
}

/*-------------------------------------------------------------------------*//**
Implementation of command DISH
@author Alexey Malov
@return RTNORM - for success; RTERROR - for error
*//*--------------------------------------------------------------------------*/
short cmd_dish()
{
    if (cmd_iswindowopen() == RTERROR)
		return RTERROR;

    if (!CModeler::checkFullModeler()) 
		return cmd_dish_mesh();

	try
	{
		//query center
		point center;
		checkSuccess(sds_initget(RSG_NONULL,NULL), RTNORM, 0);
		checkNoFailure(sds_getpoint(NULL,
			ResourceString(IDC_3DSOLIDS__NCENTER_OF_DI_22, "\nCenter of dish: "), center),
			RTERROR, RTCAN, RTNONE, 0);

		//query radius or diameter
		double rad = 0.;
		for (;;) {
			checkSuccess(sds_initget(RSG_NONULL|RSG_NOZERO,
						ResourceString(IDC_3DSOLIDS_INITGET_DIAMET_17, "Diameter ~_Diameter")),
						RTNORM, 0);
			// RSG_NONEG has no affect on getdist.
			int ret;
			checkNoFailure(ret=sds_getdist(center,
				ResourceString(IDC_3DSOLIDS__NDIAMETER__RA_23, "\nDiameter/<Radius of dish>: "), &rad),
				RTERROR, RTCAN, RTNONE, 0);

			if (ret==RTKWORD)
			{
				for (;;) {
					//query diameter
					checkSuccess(sds_initget(RSG_NONULL|RSG_NOZERO,NULL), RTNORM, 0);
					// RSG_NONEG has no affect on getdist.
					checkSuccess(sds_getdist(center,
						ResourceString(IDC_3DSOLIDS__NDIAMETER_OF__24, "\nDiameter of dish: "),
						&rad),
						RTNORM, 0);

					// verify input and prompt users to input again if it is not valid.
					//
					if (rad <= 0.0) {
						cmd_ErrorPrompt(IDC_ERRORPROMPT_POSITIVE_VALUE_303, 0);
						continue;
					}
					else
						break;
				}
				rad *= .5;
				break;
			}
			else {
				// verify input and prompt users to input again if it is not valid.
				//
				if (rad <= 0.0) {
					cmd_ErrorPrompt(IDC_ERRORPROMPT_POSITIVE_VALUE_303, 0);
					continue;
				}
				else
					break;
			}
		}
		return checkSuccess(cmd_sphere_solid_create(center, rad, eDish), RTNORM);
	}
	catch (int ret)
	{
		return ret;
	}
}

/*-------------------------------------------------------------------------*//**
Implementation of command SPHERE
@author Alexey Malov
@return RTNORM - for success; RTERROR - for error
*//*--------------------------------------------------------------------------*/
short cmd_sphere()
{
    if (cmd_iswindowopen() == RTERROR)
		return RTERROR;

    int rc;
    if (!CModeler::checkFullModeler()) {
        cmd_acad_compatible = FALSE;
		rc = cmd_sphere_mesh();
        cmd_acad_compatible = TRUE;
        return rc;
    }
	try
	{
		//query center
		point center;
		checkSuccess(sds_initget(RSG_NONULL,NULL), RTNORM, 0);
		checkNoFailure(sds_getpoint(NULL,
			ResourceString(IDC_3DSOLIDS__NCENTER_OF_SP_29, "\nCenter of sphere: "), center),
			RTERROR, RTCAN, RTNONE, 0);

		//query diameter or radius
		sds_real rad;
		for (;;) {
			checkSuccess(sds_initget(RSG_NONULL|RSG_NOZERO,
				ResourceString(IDC_3DSOLIDS_INITGET_DIAMET_17, "Diameter ~_Diameter")),
				RTNORM, 0);
			// RSG_NONEG has no affect on getdist.
			int ret;
			checkNoFailure(ret=sds_getdist(center,
				ResourceString(IDC_3DSOLIDS__NDIAMETER__RA_30, "\nDiameter/<Radius of sphere>: "), &rad),
				RTERROR, RTCAN, RTNONE, 0);

			if (ret==RTKWORD)
			{
				//query diameter
				for (;;) {
					checkSuccess(sds_initget(RSG_NONULL|RSG_NOZERO,NULL), RTNORM, 0);
					// RSG_NONEG has no affect on getdist.
					checkSuccess(sds_getdist(center,
						ResourceString(IDC_3DSOLIDS__NDIAMETER_OF__31, "\nDiameter of sphere: "), &rad),
						RTNORM, 0);

					// verify input and prompt users to input again if it is not valid.
					//
					if (rad <= 0.0) {
						cmd_ErrorPrompt(IDC_ERRORPROMPT_POSITIVE_VALUE_303, 0);
						continue;
					}
					else
						break;
				}
				rad *= .5;
				break;
			}
			else {
				// verify input and prompt users to input again if it is not valid.
				//
				if (rad <= 0.0) {
					cmd_ErrorPrompt(IDC_ERRORPROMPT_POSITIVE_VALUE_303, 0);
					continue;
				}
				else
					break;
			}
		}
		return checkSuccess(cmd_sphere_solid_create(center, rad, eSphere), RTNORM, 0);
	}
	catch (int ret)
	{
		return ret;
	}
}

/*-------------------------------------------------------------------------*//**
Implementation of command TORUS
@author Alexey Malov
@return RTNORM - for success; RTERROR - for error
*//*--------------------------------------------------------------------------*/
short cmd_torus()
{
    if (cmd_iswindowopen() == RTERROR)
		return RTERROR;

	//BugZilla No. 78050; 25-03-2002
	if (cmd_istransparent() == RTERROR) return RTERROR;

    int rc;
    if (!CModeler::checkFullModeler()) {
        cmd_acad_compatible = FALSE;
		rc = cmd_torus_mesh();
        cmd_acad_compatible = TRUE;
        return rc;
    }
	try
	{
		//query center
		point center(0., 0., 0.);
		checkSuccess(sds_initget(0,NULL), RTNORM, 0);
		checkNoFailure(sds_getpoint(NULL,
			ResourceString(IDC_3DSOLIDS__NCENTER_OF_WH_35, "\nCenter of whole torus: <0,0,0>"), center),
			RTERROR, RTCAN, 0);

		//query diameter or radius
		int ret;
		sds_real tordia = 0.;
		for (;;) {
			checkSuccess(sds_initget(7,
				ResourceString(IDC_3DSOLIDS_INITGET_DIAMET_17, "Diameter ~_Diameter")),
				RTNORM, 0);
			checkNoFailure(ret=sds_getdist(center,
				ResourceString(IDC_3DSOLIDS__NDIAMETER__RA_36, "\nDiameter/<Radius of whole torus>: "), &tordia),
				RTERROR, RTCAN, RTNONE);
			tordia *= 2.;

			if (ret==RTKWORD)
			{
				//query diameter
				for (;;) {
					checkSuccess(sds_initget(7,NULL), RTNORM, 0);
					checkSuccess(ret=sds_getdist(center,
						ResourceString(IDC_3DSOLIDS__NDIAMETER_OF__37, "\nDiameter of whole torus: "), &tordia),
						RTNORM, 0);

					// verify input and prompt users to input again if it is not valid.
					//
					if (tordia <= 0.0) {
						cmd_ErrorPrompt(IDC_ERRORPROMPT_POSITIVE_VALUE_303, 0);
						continue;
					}
					else
						break;
				}
				break;
			}
			else {
				// verify input and prompt users to input again if it is not valid.
				//
				if (tordia <= 0.0) {
					cmd_ErrorPrompt(IDC_ERRORPROMPT_POSITIVE_VALUE_303, 0);
					continue;
				}
				else
					break;
			}
		}

		sds_real tubdia;
		for (;;) {
			checkSuccess(sds_initget(5,
				ResourceString(IDC_3DSOLIDS_INITGET_DIAMET_17, "Diameter ~_Diameter")),
				RTNORM, 0);
			checkNoFailure(ret=sds_getdist(NULL,
				ResourceString(IDC_3DSOLIDS__NDIAMETER__RA_38, "\nDiameter/<Radius of body of torus>: "),
				&tubdia),
				RTERROR, RTCAN, RTNONE);
			tubdia *= 2.;

			if (ret==RTKWORD)
			{
				for (;;) {
					checkSuccess(sds_initget(5,NULL), RTNORM, 0);
					checkSuccess(sds_getdist(NULL,
						ResourceString(IDC_3DSOLIDS__NDIAMETER_OF__39, "\nDiameter of body of torus: "),
						&tubdia),
						RTNORM, 0);

					// verify input and prompt users to input again if it is not valid.
					//
					if (tubdia <= 0.0) {
						cmd_ErrorPrompt(IDC_ERRORPROMPT_POSITIVE_VALUE_303, 0);
						continue;
					}
					else
						break;
				}
				break;
			}
			else {
				// verify input and prompt users to input again if it is not valid.
				//
				if (tubdia <= 0.0) {
					cmd_ErrorPrompt(IDC_ERRORPROMPT_POSITIVE_VALUE_303, 0);
					continue;
				}
				else
					break;
			}
		}
		return checkSuccess(cmd_torus_solid_create(center, tordia, tubdia), RTNORM, 0);
	}
	catch (int ret)
	{
		return ret;
	}
}

/*-------------------------------------------------------------------------*//**
Helper scenario for creation of elliptical cone; part of command CONE
@author Alexey Malov
@return RTNORM - for success; RTERROR - for error
*//*--------------------------------------------------------------------------*/
short cmd_cone_solid_elp()
{
	resbuf rb;
	SDS_getvar(NULL,DB_QELEVATION,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	sds_real curelev = rb.resval.rreal;
	sds_real newelev = curelev;
	try
	{
		int ret;

		point center;
		point axis_end_pt;//pt0
		sds_real axis_length;//length of axis
		for (;;) {
			//query first end of ellipse axis, or choose center option
			checkSuccess(sds_initget(1,
				ResourceString(IDC_3DSOLIDS_INITGET_CENTER_47, "Center ~_Center")),
				RTNORM, 0);
			checkNoFailure(ret=sds_getpoint(NULL,
				ResourceString(IDC_3DSOLIDS__NCENTER__FIRS_48, "\nCenter/<First end of ellipse axis>: "),
				axis_end_pt),
				RTERROR, RTCAN, 0);

			gvector axisDir, axis;
			if (ret==RTKWORD)
			{
				//option center was chosen
				//query center
				checkSuccess(sds_initget(1,NULL), RTNORM, 0);
				checkSuccess(sds_getpoint(NULL,
					ResourceString(IDC_3DSOLIDS__NCENTER_OF_EL_49, "\nCenter of ellipse: "),
					center), RTNORM, 0);

				// query end point of axis. Prompt users to input its value again if
				// it is the same as the center.
				//
				for (;;) {
					checkSuccess(sds_initget(1,NULL), RTNORM, 0);
					checkSuccess(sds_getpoint(center,
						ResourceString(IDC_3DSOLIDS__NENDPOINT_OF__50, "\nEndpoint of axis: "),
						axis_end_pt), RTNORM, 0);

					if (center == axis_end_pt) {
						cmd_ErrorPrompt(CMD_ERR_ZERO_LENGTH_AXIS,0);
						continue;
					}

					axis = axis_end_pt - center;
					axisDir = axis.normalized();
					if (icadRealEqual(fabs(axisDir.z()), 1.0)) { // parallel to the Z axis of UCS
						cmd_ErrorPrompt(CMD_ERR_PERPENDICULAR_AXIS,0);
						continue;
					}
					else
						break;
				}

				axis_end_pt.z() = center.z();
				axis_length = axis.norm();

				// project the end point of the axis to the UCS plane if it is not on the plane
				//
				if (!icadIsZero(axisDir.z()))
					sds_polar(center, sds_angle(center, axis_end_pt), axis_length, axis_end_pt);

				break;
			}
			else if (ret==RTNORM)
			{
				//first end was picked
				//query second end
				point axis_end_pt2;
				for (;;) {
					checkSuccess(sds_initget(1,NULL), RTNORM, 0);
					checkSuccess(sds_getpoint(axis_end_pt,
						ResourceString(IDC_3DSOLIDS__NSECOND_END_O_51, "\nSecond end of ellipse axis: "),
						axis_end_pt2), RTNORM, 0);

					if (axis_end_pt == axis_end_pt2) {
						cmd_ErrorPrompt(CMD_ERR_ZERO_LENGTH_AXIS,0);
						continue;
					}

					axisDir = (axis_end_pt2 - axis_end_pt).normalized();

					if (icadRealEqual(fabs(axisDir.z()), 1.0)) { // parallel to the Z axis of UCS
						cmd_ErrorPrompt(CMD_ERR_PERPENDICULAR_AXIS,0);
						continue;
					}
					else
						break;
				}
				center.set(.5*(axis_end_pt.x() + axis_end_pt2.x()),
						   .5*(axis_end_pt.y() + axis_end_pt2.y()),
						   axis_end_pt.z());

				axis_length = (axis_end_pt - center).norm();

				// project the end point of the axis to the UCS plane if it is not on the plane
				//
				if (!icadIsZero(axisDir.z()))
					sds_polar(center, sds_angle(center, axis_end_pt), axis_length, axis_end_pt);

				break;
			}
		}

		if (!icadRealEqual(center[2], curelev)){
			SDS_CorrectElevation = curelev;
			SDS_ElevationChanged = TRUE;
			newelev = rb.resval.rreal = center[2];
			rb.restype=RTREAL;
			sds_setvar("ELEVATION"/*DNT*/,&rb);
		}

		gvector axis = axis_end_pt - center;//pt1
		sds_real ratio;
		{
			auto_ptr<db_ellipse> elp(new db_ellipse(SDS_CURDWG));
			{
				gvector extrusion;
				{
					resbuf rb;
					SDS_getvar(NULL,DB_QUCSXDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
					gvector xaxis = rb.resval.rpoint;
					SDS_getvar(NULL,DB_QUCSYDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
					gvector yaxis = rb.resval.rpoint;

					extrusion = xaxis * yaxis;
				}

				elp->set_210(extrusion);
				elp->set_40(1.);
				elp->set_41(0.);
				elp->set_42(IC_TWOPI);

				transf u2e = ucs2wcs() * coordinate(point(0.,0.,0.), extrusion);
				point center_ecs = center * u2e;
				gvector axis_ecs = axis * u2e;
				elp->set_10(center_ecs);
				elp->set_11(axis_ecs);
#if 0
				sds_trans(cent,&rbucs,&rbent,0,ptmp);
				elp->set_10(ptmp);
				sds_trans(pt1,&rbucs,&rbent,1,ptmp);
				elp->set_11(ptmp);
#endif
			}

            int oldGetZScale = SDS_GetZScale;
			sds_name ename;
			ename[0]=(long)elp.get();
			ename[1]=(long)SDS_CURDWG;

			sds_point pnt;
			checkSuccess(sds_initget(71,NULL), RTNORM, 0);
			SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
            // Set SDS_GetZScale value to 1 temporarily to allow SDS_AskForPoint() 
            // to accept 3D input point to fix the bug #78086 - SWH 27/8/2004
            SDS_GetZScale = 1;      
            SDS_dragobject(-9,rb.resval.rint,(sds_real *)ename,(double *)elp->retp_40(),0.0,
				ResourceString(IDC_3DSOLIDS__OTHER_AXIS____52, "<Other axis>: "),
				(struct resbuf **)(center.operator double *()), pnt, NULL);
            SDS_GetZScale = oldGetZScale;

			ratio = pnt[0];	// :) it's the natural icad code
		}

		if (SDS_ElevationChanged) {
			rb.restype = RTREAL;
			rb.resval.rreal = curelev;
			sds_setvar("ELEVATION"/*DNT*/,&rb);
			SDS_ElevationChanged = FALSE;
		}
		point pthigh = center;
		for (;;) {
			//query cone height, or choose apex option
			checkSuccess(sds_initget(1,
				ResourceString(IDC_3DSOLIDS_INITGET_APEX___53, "Apex ~_Apex")),
				RTNORM, 0);
			checkNoFailure(ret=sds_getdist(NULL,
				ResourceString(IDC_3DSOLIDS__NAPEX__HEIGHT_54, "\nApex/<Height of cone>: "),
				&pthigh.z()),
				RTERROR, RTCAN, 0);

			if (ret==RTKWORD)
			{
				//query apex
				for (;;) {
					checkSuccess(sds_initget(1,NULL), RTNORM, 0);
					checkSuccess(sds_getpoint(center,
						ResourceString(IDC_3DSOLIDS_APEX_OF_CONE___55, "Apex of cone: "),
						pthigh), RTNORM, 0);
					// error out and prompt users to input the value again if the centers are
					// coplanar.
					//
					if (icadRealEqual(pthigh.z(), center.z())) {
						cmd_ErrorPrompt(CMD_ERR_CENTER_COPLANAR, 0);
						continue;
					}
					else
						break;
				}
				break;
			}
			else if (ret==RTNORM)
			{
				// error out and prompt users to input height again if the input value is zero.
				//
				if (icadIsZero(pthigh.z())) {
					cmd_ErrorPrompt(CMD_ERR_NONZERO, 0);
					continue;
				}
				else {
					pthigh.z() += center.z();
					break;
				}
			}
		}

		return checkSuccess(cmd_cone_solid_create_elp(center, axis, ratio, pthigh), RTNORM, 0);
	}
	catch (int rc)
	{
		if (SDS_ElevationChanged) {
			rb.restype = RTREAL;
			rb.resval.rreal = curelev;
			sds_setvar("ELEVATION"/*DNT*/,&rb);
			SDS_ElevationChanged = FALSE;
		}
		return rc;
	}
}

/*-------------------------------------------------------------------------*//**
Implementation of command CONE
@author Alexey Malov
@return RTNORM - for success; RTERROR - for error
*//*--------------------------------------------------------------------------*/
short cmd_cone()
{
    if (cmd_iswindowopen() == RTERROR)
		return RTERROR;

	//BugZilla No. 78050; 25-03-2002
	if (cmd_istransparent() == RTERROR) return RTERROR;

    int rc;
    if (!CModeler::checkFullModeler()) {
        cmd_acad_compatible = FALSE;
		rc = cmd_cone_mesh();
        cmd_acad_compatible = TRUE;
        return rc;
    }

	SDS_ElevationChanged = false;

	resbuf rb;
	SDS_getvar(NULL,DB_QELEVATION,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	sds_real curelev = rb.resval.rreal;
	sds_real newelev = curelev;
	try
	{
		int ret;
		char prompt[IC_ACADBUF];
		sprintf(prompt,
			ResourceString(IDC_3DSOLIDS_CONE_ELL_CENTER, "\nElliptical/<Center of base of cone> <0,0,0>: "));

		//query center or choose option elliptical
		point center;
		{
			checkSuccess(sds_initget(0,
				ResourceString(IDC_3DSOLIDS_CYL_CONE_ELLIPT, "Elliptical ~_Elliptical")),
				RTNORM, 0);
			checkNoFailure(ret=sds_getpoint(NULL,prompt,center),
				RTERROR, RTCAN, 0);

			if (ret==RTNONE)
			{
				center.set(0., 0., 0.);
			}
			else if (ret==RTKWORD)
			{
				return cmd_cone_solid_elp();
			}
		}

		if (!icadRealEqual(center[2], curelev)){
			SDS_CorrectElevation = curelev;
			SDS_ElevationChanged = TRUE;
			newelev = rb.resval.rreal = center[2];
			rb.restype=RTREAL;
			sds_setvar("ELEVATION"/*DNT*/,&rb);
		}

		//create circle cone
		sds_real bdia = 0., tdia = 0.;
		for (;;) {
			resbuf* pDragrb = 0;
			release_resbuf destroyer(pDragrb);

			//query diameter
			checkSuccess(sds_initget(7,
				ResourceString(IDC_3DSOLIDS_INITGET_DIAMET_17, "Diameter ~_Diameter")),
				RTNORM, 0);
			SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			sds_point dummy;
			checkNoFailure(ret=SDS_dragobject(1,rb.resval.rint,center,NULL,0.,
				ResourceString(IDC_3DSOLIDS__NDIAMETER__RA_57, "\nDiameter/<Radius of base>: "),
				&pDragrb, dummy, NULL),
				RTERROR, RTCAN, 0);

			if (ret==RTKWORD)
			{
				//option 'diameter' was choosen
				destroyer.destroy();
				char fs1[IC_ACADBUF];
				sds_getinput(fs1);
				if (strisame("DIAMETER"/*DNT*/,fs1))
				{
					for (;;) {
						//query diameter
						checkSuccess(sds_initget(7,NULL), RTNORM, 0);
						checkSuccess(SDS_dragobject(1,rb.resval.rint,center,NULL,0.,
							ResourceString(IDC_3DSOLIDS__NDIAMETER_OF__59, "\nDiameter of base: "),
							&pDragrb, dummy, NULL),
							RTNORM, 0);

						ic_assoc(pDragrb,40);
						bdia=ic_rbassoc->resval.rreal;

						// error out and prompt to input again if the dimension is zero.
						//
						if (icadIsZero(bdia)) {
							cmd_ErrorPrompt(IDC_ERRORPROMPT_POSITIVE_VALUE_303, 0);
							continue;
						}
						else
							break;
					}
				}
				break;
			}
			else if (ret==RTNORM)
			{
				ic_assoc(pDragrb,40);
				bdia=2.0*ic_rbassoc->resval.rreal;

				// error out and prompt to input again if the input dimension is zero
				//
				if (icadIsZero(bdia))
					cmd_ErrorPrompt(IDC_ERRORPROMPT_POSITIVE_VALUE_303, 0);
				else
					break;
			}
		}

		if (SDS_ElevationChanged) {
			rb.restype = RTREAL;
			rb.resval.rreal = curelev;
			sds_setvar("ELEVATION"/*DNT*/,&rb);
			SDS_ElevationChanged = FALSE;
		}

		point pthigh = center;
		for (;;) {
			checkSuccess(sds_initget(1,
				ResourceString(IDC_3DSOLIDS_INITGET_APEX___53, "Apex ~_Apex")),
				RTNORM, 0);
			checkNoFailure(ret=sds_getdist(NULL,
				ResourceString(IDC_3DSOLIDS__NAPEX__HEIGHT_54, "\nApex/<Height of cone>: "),
				&pthigh[2]),
				RTERROR, RTCAN, 0);

			if (ret==RTKWORD)
			{
				for (;;) {
					checkSuccess(sds_initget(1,NULL), RTNORM, 0);
					checkSuccess(sds_getpoint(center,
						ResourceString(IDC_3DSOLIDS_APEX_OF_CONE___55, "Apex of cone: "),
						pthigh),
						RTNORM, 0);

					// error out and prompt users to input the value again if the centers are
					// coplanar.
					//
					if (icadRealEqual(pthigh.z(), center.z())) {
						cmd_ErrorPrompt(CMD_ERR_CENTER_COPLANAR, 0);
						continue;
					}
					else
						break;
				}
				break;
			}
			else if (ret==RTNORM)
			{
				// error out and prompt users to input height again if the input value is zero.
				//
				if (icadIsZero(bdia))
					cmd_ErrorPrompt(CMD_ERR_NONZERO, 0);
				else {
					pthigh.z() += center.z();
					break;
				}
			}
		}
		return checkSuccess(cmd_cone_solid_create(center, bdia, tdia, pthigh), RTNORM, 0);
	}
	catch (int rc)
	{
		if (SDS_ElevationChanged) {
			rb.restype = RTREAL;
			rb.resval.rreal = curelev;
			sds_setvar("ELEVATION"/*DNT*/,&rb);
			SDS_ElevationChanged = FALSE;
		}
		return rc;
	}
}

/*-------------------------------------------------------------------------*//**
Implementation of command CYLINDER
@author Alexey Malov
@return RTNORM - for success; RTERROR - for error
*//*--------------------------------------------------------------------------*/
short cmd_cylinder()
{
    if (cmd_iswindowopen() == RTERROR)
		return RTERROR;

	//BugZilla No. 78050; 25-03-2002
	if (cmd_istransparent() == RTERROR) return RTERROR;

    int rc;
    if (!CModeler::checkFullModeler()) {
        cmd_acad_compatible = FALSE;
		rc = cmd_cylinder_mesh();
        cmd_acad_compatible = TRUE;
        return rc;
    }

	SDS_ElevationChanged = false;

	resbuf rb;
	SDS_getvar(NULL,DB_QELEVATION,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	sds_real curelev = rb.resval.rreal;
	sds_real newelev = curelev;
	try
	{
		char prompt[IC_ACADBUF],fs1[IC_ACADBUF];

		sprintf(prompt,
				ResourceString(IDC_3DSOLIDS_CYLINDER_ELL_CENTER, "\nElliptical/<Center of base of cylinder> <0,0,0>: "));

		//query center of cylinder base or choose option
		point center;
		checkSuccess(sds_initget(0,
			ResourceString(IDC_3DSOLIDS_CYL_CONE_ELLIPT, "Elliptical ~_Elliptical" )),
			RTNORM, 0);
		int ret;
        checkNoFailure(ret=sds_getpoint(NULL, prompt, center),
			RTERROR, RTCAN, 0);

		if (ret==RTNONE)
		{
			center.set(0., 0., 0.);
        }
		else if (ret==RTKWORD)
		{
			//elliptical cylinder

			//query first end of ellipse axis or choose option
			point axis_end1;//pt0
            checkSuccess(sds_initget(1,
				ResourceString(IDC_3DSOLIDS_INITGET_CENTER_47, "Center ~_Center")),
				RTNORM, 0);
            checkNoFailure(ret=sds_getpoint(NULL,
				ResourceString(IDC_3DSOLIDS__NCENTER__FIRS_48, "\nCenter/<First end of ellipse axis>: "),
				axis_end1),
				RTERROR, RTCAN, 0);

			sds_real dist1;
			gvector axis1, axisDir;
			if (ret==RTKWORD)
			{
				//option center is selected
				//query center
                checkSuccess(sds_initget(1,NULL), RTNORM, 0);
                checkSuccess(sds_getpoint(NULL,
					ResourceString(IDC_3DSOLIDS__NCENTER_OF_EL_49, "\nCenter of ellipse: " ),
					center),
					RTNORM, 0);

				for (;;) {
					//query end point of axis
					checkSuccess(sds_initget(1,NULL), RTNORM, 0);
					checkSuccess(sds_getpoint(center,
						ResourceString(IDC_3DSOLIDS__NENDPOINT_OF__50, "\nEndpoint of axis: "),
						axis_end1),
						RTNORM, 0);

					if (center == axis_end1) { // duplicate point. Ask to input again
						cmd_ErrorPrompt(CMD_ERR_ZERO_LENGTH_AXIS,0);
						continue;
					}

					axis1 = axis_end1 - center;
					axisDir = axis1.normalized();
					if (icadRealEqual(fabs(axisDir.z()), 1.0)) { // parallel to the Z axis of UCS
						cmd_ErrorPrompt(CMD_ERR_PERPENDICULAR_AXIS,0);
						continue;
					}
					else
						break;

				}
				dist1 = axis1.norm();

				// project the end point of the axis to the UCS plane if it is not on the plane
				//
				if (!icadIsZero(axisDir.z()))
					sds_polar(center, sds_angle(center,axis_end1), dist1, axis_end1);
            }
			else if (ret==RTNORM)
			{
				//first end of ellipse axis was picked
				//query second end of ellipse axis
				point axis_end2;//pt1
				for (;;) {
					checkSuccess(sds_initget(1,NULL), RTNORM, 0);
					checkSuccess(sds_getpoint(axis_end1,
						ResourceString(IDC_3DSOLIDS__NSECOND_END_O_65, "\nSecond end of ellipse axis>: " ),
						axis_end2),
						RTNORM, 0);

					if (axis_end2 == axis_end1) { // duplicate axis point
						cmd_ErrorPrompt(CMD_ERR_ZERO_LENGTH_AXIS,0);
						continue;
					}

					axis1 = axis_end2 - axis_end1;
					axisDir = axis1.normalized();

					if (icadRealEqual(fabs(axisDir.z()), 1.0)) { // parallel to the Z axis of UCS
						cmd_ErrorPrompt(CMD_ERR_PERPENDICULAR_AXIS,0);
						continue;
					}
					else
						break;
				}

				//calculate center
				center.set((axis_end1.x() + axis_end2.x())*.5,
						   (axis_end1.y() + axis_end2.y())*.5,
						   (axis_end1.z() + axis_end2.z())*.5);

				axis1 *= 0.5;
				dist1 = axis1.norm();
				// project the end point of the axis to the UCS plane if it is not on the plane
				//
				if (!icadIsZero(axisDir.z()))
					sds_polar(center, sds_angle(center,axis_end1), dist1, axis_end1);
            }

			//query ratio
			sds_real ratio;
			{
				gvector extrusion;
				{
					resbuf rb;
					SDS_getvar(NULL,DB_QUCSXDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
					gvector xaxis = rb.resval.rpoint;
					SDS_getvar(NULL,DB_QUCSYDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
					gvector yaxis = rb.resval.rpoint;

					extrusion = xaxis * yaxis;
				}

				auto_ptr<db_ellipse> elp(new db_ellipse(SDS_CURDWG));
				{
					elp->set_210(extrusion);
					elp->set_41(0.);//start parameter
					elp->set_42(IC_TWOPI);//end parameter

					transf u2w = ucs2wcs();

					gvector axis_wcs = axis1 * u2w;
					elp->set_11(axis_wcs);

					point center_wcs = center * u2w;
					elp->set_10(center_wcs);

					sds_real dist2 = 1.;

					elp->set_40(1.);//dist2);
				}
				sds_name ename = {(long)elp.get(), (long)SDS_CURDWG};

				if (!icadRealEqual(center[2], curelev)){
					SDS_CorrectElevation = curelev;
					SDS_ElevationChanged = TRUE;
					newelev = rb.resval.rreal = center[2];
					rb.restype=RTREAL;
					sds_setvar("ELEVATION"/*DNT*/,&rb);
				}

				do
				{
					checkSuccess(sds_initget(71,NULL), RTNORM, 0);
					resbuf rb;
					SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

					point ratiop = axis1;
//					sds_point ratiop;
					SDS_dragobject(-9,rb.resval.rint,(sds_real *)ename,(double*)elp->retp_40(),0.,
						ResourceString(IDC_3DSOLIDS__OTHER_AXIS____52, "<Other axis>: " ) ,(resbuf**)(center.operator double*()), ratiop, NULL);
#if 0
					dist2 = axis1.x()*dist1;
       				ratio = axis1.x(); //dist2/dist1;
#endif
					ratio = ratiop[0];
					if (le(ratio, 100.) &&
						ge(ratio, .01))
						break;

					cmd_ErrorPrompt(CMD_ERR_TRYAGAIN,0);
				}
				while(1);

				if (SDS_ElevationChanged) {
					rb.restype = RTREAL;
					rb.resval.rreal = curelev;
					sds_setvar("ELEVATION"/*DNT*/,&rb);
					SDS_ElevationChanged = FALSE;
				}
			}

			// query height or coose option
			//
			for (;;) {
				point pthigh = center;
				checkSuccess(sds_initget(1,
					ResourceString(IDC_3DSOLIDS_INITGET_CENTER_47, "Center ~_Center")),
					RTNORM, 0);
				checkNoFailure(ret=sds_getdist(NULL,
					ResourceString(IDC_3DSOLIDS__NCENTER_OF_SE_66, "\nCenter of second end/<Height of cylinder>: "),
					&pthigh.z()),
					RTERROR, RTCAN, 0);

				if (ret!=RTKWORD) {
					// error out and ask to input again if the height is zero
					//
					if (icadIsZero(pthigh.z())) {
						cmd_ErrorPrompt(CMD_ERR_NONZERO,0);
						continue;
					}
					else
						pthigh.z() += center.z();
				}
				else {
					for (;;) {
						//query center of second end (end of what?)
						checkSuccess(sds_initget(1,NULL), RTNORM, 0);
						checkSuccess(ret=sds_getpoint(center,
							ResourceString(IDC_3DSOLIDS_CENTER_OF_SECO_67, "Center of second end: " ),
							pthigh),
							RTNORM, 0);

						// error out and prompt to input again if the centers are co-planar
						//
						if (icadRealEqual(center.z(), pthigh.z())) {
							cmd_ErrorPrompt(CMD_ERR_CENTER_COPLANAR,0);
							continue;
						}
						else
							break;
					}
				}
				return checkSuccess(cmd_cyl_solid_create_elp(center, axis1, ratio, pthigh), RTNORM, 0);
			}
		}

		//circle cylinder
		if (!icadRealEqual(center[2], curelev)){
			SDS_CorrectElevation = curelev;
			SDS_ElevationChanged = TRUE;
			newelev = rb.resval.rreal = center[2];
			rb.restype=RTREAL;
			sds_setvar("ELEVATION"/*DNT*/,&rb);
		}
		//here we have center point
		SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

		//query diameter
		sds_real bdia = 0.;
		for (;;)
		{
			checkSuccess(sds_initget(RSG_NONULL|RSG_NOZERO|RSG_NONEG|SDS_RSG_NODOCCHG,
			   ResourceString(IDC_3DSOLIDS_INITGET_DIAMET_17, "Diameter ~_Diameter")),
			   RTNORM, 0);

			resbuf* pDragrb = 0;
			release_resbuf destroyer(pDragrb);

			point pt;
			checkNoFailure(ret=SDS_dragobject(1,rb.resval.rint,center,NULL,0.0,
				ResourceString(IDC_3DSOLIDS__NDIAMETER__RA_69, "\nDiameter/<Radius of cylinder>: " ),
				&pDragrb,pt,NULL),
				RTERROR, RTCAN, 0);

			if (ret==RTNORM)
			{
				ic_assoc(pDragrb,40);
				bdia = 2.*ic_rbassoc->resval.rreal;
				if (icadIsZero(bdia)) {
					cmd_ErrorPrompt(IDC_ERRORPROMPT_POSITIVE_VALUE_303, 0);
					continue;
				}
				else
					break;
			}
			else if (ret==RTKWORD)
			{
				// SDS_dragobject returns RTKWORD for any alpha input, regardless of the initget.
				sds_getinput(fs1);
				if (strisame("DIAMETER"/*DNT*/ ,fs1))
				{
					checkSuccess(sds_initget(7,NULL), RTNORM, 0);
					checkSuccess(SDS_dragobject(20,rb.resval.rint,center,NULL,0.0,
						ResourceString(IDC_3DSOLIDS__NDIAMETER_OF__70, "\nDiameter of cylinder: " ),
						&pDragrb, pt, NULL),
						RTNORM, 0);

					ic_assoc(pDragrb,40);
					bdia=(2*(ic_rbassoc->resval.rreal));	// Kludge - how should I do this right?
					if (icadIsZero(bdia)) {
						cmd_ErrorPrompt(IDC_ERRORPROMPT_POSITIVE_VALUE_303, 0);
						continue;
					}
					else
						break;
				}
				else
				{
					cmd_ErrorPrompt(CMD_ERR_PTKWORD,0);    // "Please pick a point or choose an option from the list."
				}
			}
		}

		if (SDS_ElevationChanged) {
			rb.restype = RTREAL;
			rb.resval.rreal = curelev;
			sds_setvar("ELEVATION"/*DNT*/,&rb);
			SDS_ElevationChanged = FALSE;
		}
		point pthigh = center;
		for (;;) {
			//query height of cylinder or choose option
			checkSuccess(sds_initget(1,
				ResourceString(IDC_3DSOLIDS_INITGET_CENTER_47, "Center ~_Center")),
				RTNORM, 0);

			checkNoFailure(ret=sds_getdist(NULL,
				ResourceString(IDC_3DSOLIDS__NCENTER_OF_SE_66, "\nCenter of second end/<Height of cylinder>: "),
				&pthigh[2]),
				RTERROR, RTCAN, 0);

			if (ret!=RTKWORD) {
				if (icadIsZero(pthigh.z())) {
					cmd_ErrorPrompt(CMD_ERR_NONZERO, 0);
					continue;
				}

				pthigh.z() += center.z();
			}
			else {
				for (;;) {
					//query center of second end of cylinder
					checkSuccess(sds_initget(1,NULL), RTNORM, 0);
					checkSuccess(sds_getpoint(center,
						ResourceString(IDC_3DSOLIDS_CENTER_OF_SECO_67, "Center of second end: "),
						pthigh),
						RTNORM, 0);

					// error out and prompt users to input the value again if the centers are
					// coplanar.
					//
					if (icadRealEqual(pthigh.z(), center.z()))
						cmd_ErrorPrompt(CMD_ERR_CENTER_COPLANAR, 0);
					else
						break;
				}
			}
			return checkSuccess(cmd_cyl_solid_create(center, bdia, pthigh), RTNORM, 0);
		}
	}
	catch (int rc)
	{
		if (SDS_ElevationChanged) {
			rb.restype = RTREAL;
			rb.resval.rreal = curelev;
			sds_setvar("ELEVATION"/*DNT*/,&rb);
			SDS_ElevationChanged = FALSE;
		}
		return rc;
	}
}


/*-------------------------------------------------------------------------*//**
Implementation of command WEDGE
@author Alexey Malov
@return RTNORM - for success; RTERROR - for error
*//*--------------------------------------------------------------------------*/
short cmd_wedge()
{
    if (cmd_iswindowopen() == RTERROR)
		return RTERROR;

	//BugZilla No. 78050; 25-03-2002
	if (cmd_istransparent() == RTERROR) return RTERROR;

    int rc;
    if (!CModeler::checkFullModeler()) {
        cmd_acad_compatible = FALSE;
		rc = cmd_wedge_mesh();
        cmd_acad_compatible = TRUE;
        return rc;
    }

	point lastpt, corner1, corner2;
	resbuf rb;
	try
	{
		int ret;
		//get last point
		checkSuccess(SDS_getvar(NULL,DB_QLASTPOINT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES), RTNORM, 0);
		lastpt = rb.resval.rpoint;

		char szLastPt[IC_ACADBUF];
		{
			char szx[IC_ACADBUF], szy[IC_ACADBUF], szz[IC_ACADBUF];
			sds_rtos(lastpt.x(), -1, -1, szx);
			sds_rtos(lastpt.y(), -1, -1, szy);
			sds_rtos(lastpt.z(), -1, -1, szz);

			sprintf(szLastPt, "<%s,%s,%s>", szx, szy, szz);
		}
		char prompt[IC_ACADBUF];
		sprintf(prompt,
			ResourceString(IDC_3DSOLIDS_WEDGE_CENTER_CORNER, "\nCenter/<Corner of wedge> %s: "), szLastPt);

		//query corner of wedge or center
		corner1 = lastpt;//pt1
		checkSuccess(sds_initget(0,
			ResourceString(IDC_3DSOLIDS_INITGET_CENTER_47, "Center ~_Center")),
			RTNORM, 0);
		checkNoFailure(ret=sds_getpoint(NULL, prompt, corner1),
			RTERROR, RTCAN, 0);

		if (ret==RTKWORD)
		{
			//center of wedge
			sprintf(prompt,
				ResourceString(IDC_3DSOLIDS_WEDGE_CENTER_PROMPT, "\nCenter of wedge %s: " ), szLastPt);
			//query center
			point center = lastpt;//pt0
			checkSuccess(sds_initget(0,NULL), RTNORM, 0);
			checkNoFailure(ret=sds_getpoint(NULL,prompt,center),
				RTERROR, RTCAN, 0);

			//query cube?, length or corner
			corner1 = lastpt;//pt1
			checkSuccess(sds_initget(1,
				ResourceString(IDC_3DSOLIDS_INITGET_CUBE_L_84, "Cube Length_of_side|Length ~_Cube ~_Length")),
				RTNORM, 0);

			SDS_getvar(NULL, DB_QDRAGMODE, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
			checkNoFailure(ret=SDS_dragobject(21, rb.resval.rint, center, NULL, 0.,
				ResourceString(IDC_3DSOLIDS__NCUBE_LENGTH__85, "\nCube/Length/<Corner of wedge>: "),
				NULL, corner1, NULL),
				RTERROR, RTCAN);

			if (ret==RTKWORD)
			{
				char fs1[IC_ACADBUF];
				sds_getinput(fs1);
				if (strisame("CUBE"/*DNT*/ ,fs1))
				{
					//is it a cube?
					//query length
					sds_real length = 0.;
					checkSuccess(sds_initget(3,NULL), RTNORM, 0);
					checkSuccess(sds_getdist(NULL,
						ResourceString(IDC_3DSOLIDS__NLENGTH_OF_SI_87, "\nLength of sides of cubic wedge: "),
						&length),
						RTNORM, 0);

					sds_real half_length = .5*length;
					gvector half_diag(half_length, half_length, half_length);
					corner1 = center - half_diag;
					half_diag.z() *= -1.;
					point corner2 = center + half_diag;

					checkSuccess(cmd_wedge_solid_create(corner1, corner2, length), RTNORM, 0);
				}
				else if (strisame("LENGTH"/*DNT*/,fs1))
				{
					//length option is choosen
					//query length
					sds_real length = 0.;
					checkSuccess(sds_initget(3,NULL), RTNORM, 0);
					checkSuccess(sds_getdist(NULL,
						ResourceString(IDC_3DSOLIDS__NLENGTH_OF_SI_75, "\nLength of side: " ),
						&length),
						RTNORM, 0);

					//query width
					sds_real width = 0.;
					checkSuccess(sds_initget(3,NULL), RTNORM, 0);
					checkSuccess(sds_getdist(NULL,
						ResourceString(IDC_3DSOLIDS__NWIDTH_ACROSS_89, "\nWidth across wedge: "),
						&width),
						RTNORM, 0);

					//query height
					sds_real height = 0.;
					checkSuccess(sds_initget(3,NULL), RTNORM, 0);
					checkSuccess(sds_getdist(NULL,
						ResourceString(IDC_3DSOLIDS__NHEIGHT_OF_WE_77, "\nHeight of wedge: "),
						&height),
						RTNORM, 0);

					gvector half_diag(.5*length, .5*width, .5*height);
					corner1 = center - half_diag;
					half_diag.z() *= -1.;
					corner2 = center + half_diag;

					checkSuccess(cmd_wedge_solid_create(corner1, corner2, height), RTNORM, 0);
				}
			}
			else if (ret==RTNORM)
			{
				//corner was picked (we have center and corner)
				//get height
				sds_real height = 2.*(corner1.z() - center.z());
				lastpt = corner1;
				if (eq(height, 0.))
				{
					//query height
					checkSuccess(sds_initget(3,NULL), RTNORM, 0);
					checkSuccess(sds_getdist(NULL,
						ResourceString(IDC_3DSOLIDS__NHEIGHT_OF_WE_77, "\nHeight of wedge: "),
						&height),
						RTNORM, 0);
				}

				corner2 = center + (center - corner1);
				corner1.z() = corner2.z() = center.z() - 0.5 * height;

				//create wedge
				checkSuccess(cmd_wedge_solid_create(corner1, corner2, height), RTNORM, 0);
			}
		}
		else if ((ret==RTNORM)||(ret==RTNONE))
		{
			//first corner was picked
			lastpt = corner1;

			//query second corner or select cube, length options
			corner2;
			checkSuccess(sds_initget(1,
				ResourceString(IDC_3DSOLIDS_INITGET_CUBE_L_84, "Cube Length_of_side|Length ~_Cube ~_Length")),
				RTNORM, 0);

			SDS_getvar(NULL, DB_QDRAGMODE, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
			checkNoFailure(ret=SDS_dragobject(21, rb.resval.rint, corner1, NULL, 0.0,
				ResourceString(IDC_3DSOLIDS__NCUBE_LENGTH__90, "\nCube/Length/<Opposite corner>: "),
				NULL, corner2, NULL),
				RTERROR, RTCAN, 0);

			if (ret==RTKWORD)
			{
				char fs1[IC_ACADBUF];
				sds_getinput(fs1);
				if (strisame("CUBE"/*DNT*/, fs1))
				{
					//option cube is selected
					//query cube length
					sds_real length = 0.;
					checkSuccess(sds_initget(3,NULL), RTNORM, 0);
					checkSuccess(sds_getdist(NULL,
						ResourceString(IDC_3DSOLIDS__NLENGTH_OF_SI_87, "\nLength of sides of cubic wedge: "),
						&length),
						RTNORM, 0);

					corner2 = corner1 + gvector(length, length, 0.);

					checkSuccess(cmd_wedge_solid_create(corner1, corner2, length), RTNORM, 0);
				}
				else if (strisame("LENGTH"/*DNT*/, fs1))
				{
					//option length is selected
					//query length
					sds_real length = 0.;
					checkSuccess(sds_initget(3,NULL), RTNORM, 0);
					checkSuccess(sds_getdist(NULL,
						ResourceString(IDC_3DSOLIDS__NLENGTH_OF_SI_75, "\nLength of side: "),
						&length),
						RTNORM, 0);

					//query width
					sds_real width = 0.;
					checkSuccess(sds_initget(3,NULL), RTNORM, 0);
					checkSuccess(sds_getdist(NULL,
						ResourceString(IDC_3DSOLIDS__NWIDTH_OF_WED_76, "\nWidth of wedge: "),
						&width),
						RTNORM, 0);

					//query height
					sds_real height = 0.;
					checkSuccess(sds_initget(3,NULL), RTNORM, 0);
					checkSuccess(sds_getdist(NULL,
						ResourceString(IDC_3DSOLIDS__NHEIGHT_OF_WE_77, "\nHeight of wedge: "),
						&height),
						RTNORM, 0);

					corner2 = corner1 + gvector(length, width, 0.);

					//create wedge
					checkSuccess(cmd_wedge_solid_create(corner1, corner2, height), RTNORM, 0);
				}
			}
			else if (ret==RTNORM)
			{
				//second corner was picked
				sds_real height = corner2.z() - corner1.z();
				corner2.z() = corner1.z();
				if (eq(height, 0.))
				{
					//query height
					//draw base
					point corner = corner1;
					corner.x() = corner2.x();
					sds_grdraw(corner1, corner, -1, 0);
					sds_grdraw(corner2, corner, -1, 0);

					corner.x() = corner1.x();
					corner.y() = corner2.y();
					sds_grdraw(corner1, corner, -1, 0);
					sds_grdraw(corner2, corner, -1, 0);

					//query
					checkSuccess(sds_initget(3,NULL), RTNORM, 0);
					checkSuccess(sds_getdist(corner1,
						ResourceString(IDC_3DSOLIDS__NHEIGHT_OF_WE_77, "\nHeight of wedge: "),
						&height),
						RTNORM, 0);

					//undraw
					sds_grdraw(corner1, corner, -1, 0);
					sds_grdraw(corner2, corner, -1, 0);
					corner.x() = corner2.x();
					corner.y() = corner1.y();
					sds_grdraw(corner1, corner, -1, 0);
					sds_grdraw(corner2, corner, -1, 0);
				}

				//create wedge
				checkSuccess(cmd_wedge_solid_create(corner1, corner2, height), RTNORM, 0);
			}
		}
	}
	catch (int rc)
	{
		if (rc == RTCAN) {
			//undraw
			point corner;
			corner.x() = corner1.x();
			corner.y() = corner2.y();
			sds_grdraw(corner1, corner, -1, 0);
			sds_grdraw(corner2, corner, -1, 0);
			corner.x() = corner2.x();
			corner.y() = corner1.y();
			sds_grdraw(corner1, corner, -1, 0);
			sds_grdraw(corner2, corner, -1, 0);
		}
		return rc;
	}

	//set last point
	rb.restype = RT3DPOINT;
	ic_ptcpy(rb.resval.rpoint, lastpt);
	SDS_setvar(NULL,DB_QLASTPOINT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);

	return RTNORM;
}

/*-------------------------------------------------------------------------*//**
Implementation of command BOX
@author Alexey Malov
@return RTNORM - for success; RTERROR - for error
*//*--------------------------------------------------------------------------*/
short cmd_box()
{
	/*DG - 21.5.2002*/// I moved checking ACIS availableness to from this function to cmd_box_solid_or_mesh_create
	// so cmd_box_solid_create calls are replaced with cmd_box_solid_or_mesh_create.

    if (cmd_iswindowopen() == RTERROR)
		return RTERROR;

	//BugZilla No. 78050; 25-03-2002
	if (cmd_istransparent() == RTERROR) return RTERROR;

	point lastpt;

	/*DG - 21.5.2002*/// I moved this "try" upward for including the following checkSuccess of SDS_getvar.
	try
	{
		//get last point
		{
			resbuf getrb;
			checkSuccess(SDS_getvar(NULL,DB_QLASTPOINT,&getrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES),
				RTNORM, 0);

			lastpt = getrb.resval.rpoint;
		}

		do
		{
			char szLastPt[IC_ACADBUF];
			{
				char szLastPtX[IC_ACADBUF],szLastPtY[IC_ACADBUF],szLastPtZ[IC_ACADBUF];
				sds_rtos((eq(lastpt.x(), 0., CMD_FUZZ))? 0.0: lastpt.x(), -1, -1, szLastPtX);
				sds_rtos((eq(lastpt.y(), 0., CMD_FUZZ))? 0.0: lastpt.y(), -1, -1, szLastPtY);
				sds_rtos((eq(lastpt.z(), 0., CMD_FUZZ))? 0.0: lastpt.z(), -1, -1, szLastPtZ);
				sprintf(szLastPt, "<%s,%s,%s>", szLastPtX, szLastPtY, szLastPtZ);
			}

			int ret;
			char prompt[IC_ACADBUF], fs1[IC_ACADBUF];
			sprintf(prompt,
				ResourceString(IDC_3DSOLIDS__NCENTER__CORN_98, "\nCenter/<Corner of box> %s: "),
				szLastPt);

			checkSuccess(sds_initget(0,ResourceString(IDC_3DSOLIDS_INITGET_CENTER_47, "Center ~_Center")),
				RTNORM, 0);

			point corner1;//pt1
			checkNoFailure(ret=sds_getpoint(NULL,prompt,corner1),
				RTERROR, RTCAN, 0);

			if (ret==RTKWORD)
			{
				//Center point
				sprintf(prompt,
					ResourceString(IDC_3DSOLIDS__NCENTER___S___99, "\nCenter %s: "),
					szLastPt);
				checkSuccess(sds_initget(0,NULL),
					RTNORM, 0);

				point center;//pt0
				checkNoFailure(ret=sds_getpoint(NULL, prompt, center),
					RTERROR, RTCAN, 0);

				if (ret==RTNONE)
				{
					//use default value
					center = lastpt;
				}
#if 0
				resbuf getrb;
				checkSuccess(SDS_getvar(NULL,DB_QDRAGMODE,&getrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES),
					RTNORM, 0);
#endif
				//TODO - we need a dragobject mode to draw rectang of fixed length or width
				checkSuccess(sds_initget(1,ResourceString(IDC_3DSOLIDS_INITGET_CUBE_L_84, "Cube Length_of_side|Length ~_Cube ~_Length")),
					RTNORM, 0);

				point corner2;//pt1
				checkNoFailure(ret=sds_getpoint(center, ResourceString(IDC_3DSOLIDS__NCUBE_LENGTH_100, "\nCube/Length of box/<Corner of box>: " ), corner2),
					RTERROR, RTCAN, 0);

				if (ret==RTKWORD)
				{
					//Cube or Length
					checkSuccess(sds_getinput(fs1), RTNORM, 0);
					if (strisame("CUBE"/*DNT*/, fs1))
					{
						//query cube's edge length
						double len;
						checkSuccess(sds_initget(3,NULL), RTNORM, 0);
						checkSuccess(sds_getdist(NULL,ResourceString(IDC_3DSOLIDS__NLENGTH_OF_S_101, "\nLength of sides of cube: " ) ,&len),
							RTNORM, 0);

						//calculate corners
						double half_len = 0.5*len;
						gvector half_diag(half_len, half_len, half_len);
						point corner1 = center + half_diag;
						point corner2 = center - half_diag;
						//create solid
						checkSuccess(cmd_box_solid_or_mesh_create(corner1, corner2), RTNORM, 0);
						break;
					}
					else if (strisame("LENGTH"/*DNT*/ ,fs1))
					{
						//query length
						double len;
						checkSuccess(sds_initget(3,NULL), RTNORM, 0);
						checkSuccess(sds_getdist(NULL,ResourceString(IDC_3DSOLIDS__NLENGTH_OF_SI_93, "\nLength of side of box: " ) ,&len),
							RTNORM, 0);

						//query width
						double wth;
						checkSuccess(sds_initget(3,NULL), RTNORM, 0);
						checkSuccess(sds_getdist(NULL,ResourceString(IDC_3DSOLIDS__NWIDTH_OF_BO_102, "\nWidth of box: " ) ,&wth),
							RTNORM, 0);

						//query height
						double high;
						checkSuccess(sds_initget(3,NULL), RTNORM, 0);
						checkSuccess(sds_getdist(NULL,ResourceString(IDC_3DSOLIDS__NHEIGHT_OF_BO_96, "\nHeight of box: " ) ,&high),
							RTNORM, 0);

						//calculate corners
						gvector half_diag(.5*len, .5*wth, .5*high);
						point corner1 = center + half_diag;
						point corner2 = center - half_diag;
						//create solid
						checkSuccess(cmd_box_solid_or_mesh_create(corner1, corner2), RTNORM, 0);
						break;
					}
				}
				else if (ret==RTNORM)
				{
					//Corner was picked (now we have center and corner2)
					lastpt = corner2; //pt1;
					gvector half_diag = corner2 - center;
					point corner1 = center - half_diag;

					if (eq(corner1.z(), corner2.z()))
					{
						//query height
						double high;
						checkSuccess(sds_initget(3,NULL), RTNORM, 0);
						checkSuccess(sds_getdist(NULL,ResourceString(IDC_3DSOLIDS__NHEIGHT_OF_BO_96, "\nHeight of box: " ) ,&high),
							RTNORM, 0);

						high *= 0.5;
						corner1.z() -= high;
						corner2.z() += high;
					}

					checkSuccess(cmd_box_solid_or_mesh_create(corner1, corner2), RTNORM);
					break;
				}
			}
			else if ((ret==RTNORM)||(ret==RTNONE))
			{
				//First corner was picked
				if (ret==RTNONE)
					corner1 = lastpt;
				else
					lastpt = corner1;

				//query Cube or Length
				checkSuccess(sds_initget(1,ResourceString(IDC_3DSOLIDS_INITGET_CUBE_L_84, "Cube Length_of_side|Length ~_Cube ~_Length" )),
					RTNORM, 0);

				point corner2;//pt2
				resbuf getrb;
				SDS_getvar(NULL,DB_QDRAGMODE,&getrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				checkNoFailure(ret=SDS_dragobject(21,getrb.resval.rint,corner1,NULL,0.0,ResourceString(IDC_3DSOLIDS__NCUBE_LENGTH__90, "\nCube/Length/<Opposite corner>: " ) ,NULL,corner2,NULL),
					RTERROR, RTCAN, 0);

				if (ret==RTKWORD)
				{
					//get key-word
					sds_getinput(fs1);
					if (strisame("CUBE"/*DNT*/ ,fs1))
					{
						//query cube length
						double len;
						checkSuccess(sds_initget(3,NULL), RTNORM, 0);
						checkSuccess(sds_getdist(NULL,ResourceString(IDC_3DSOLIDS__NLENGTH_OF_S_101, "\nLength of sides of cube: " ) ,&len),
							RTNORM, 0);
						//calculate second corner
						point corner2 = corner1 + gvector(len, len, len);
						//create solid
						checkSuccess(cmd_box_solid_or_mesh_create(corner1,corner2), RTNORM, 0);
						break;
					}
					else if (strisame("LENGTH"/*DNT*/ ,fs1))
					{
						//query length
						double len;
						checkSuccess(sds_initget(3,NULL), RTNORM, 0);
						checkSuccess(sds_getdist(NULL,ResourceString(IDC_3DSOLIDS__NLENGTH_OF_SI_93, "\nLength of side of box: " ) ,&len),
							RTNORM, 0);

						//query width
						double wth;
						checkSuccess(sds_initget(3,NULL), RTNORM, 0);
						checkSuccess(sds_getdist(NULL,ResourceString(IDC_3DSOLIDS__NWIDTH_OF_BO_102, "\nWidth of box: " ) ,&wth),
							RTNORM, 0);

						//query height
						double high;
						checkSuccess(sds_initget(3,NULL), RTNORM, 0);
						checkSuccess(sds_getdist(NULL,ResourceString(IDC_3DSOLIDS__NHEIGHT_OF_BO_96, "\nHeight of box: " ) ,&high),
							RTNORM, 0);

						//calculate second corner
						point corner2 = corner1 + gvector(len, wth, high);
						//create solid
						checkSuccess(cmd_box_solid_or_mesh_create(corner1, corner2), RTNORM, 0);
						break;
					}
				}
				else if (ret==RTNORM)
				{
					//second crner was picked (we have 2 corners)
					lastpt = corner2;
					if (eq(corner1.z(), corner2.z()))
					{
						//query height
						checkSuccess(sds_initget(3,NULL), RTNORM, 0);

						//draw base rectangle
						point corner = corner1;
						corner.x() = corner2.x();
						sds_grdraw(corner1, corner, -1, 0);
						sds_grdraw(corner2, corner, -1, 0);
						corner.x() = corner1.x();
						corner.y() = corner2.y();
						sds_grdraw(corner1, corner, -1, 0);
						sds_grdraw(corner2, corner, -1, 0);

						double high;
						ret=sds_getdist(NULL,ResourceString(IDC_3DSOLIDS__NHEIGHT_OF_BO_96, "\nHeight of box: " ) ,&high); // AutoCAD makes user pick both points.

						//clean drawn base rectangle
						sds_grdraw(corner1, corner, -1, 0);
						sds_grdraw(corner2, corner, -1, 0);
						corner.x() = corner2.x();
						corner.y() = corner1.y();
						sds_grdraw(corner1, corner, -1, 0);
						sds_grdraw(corner2, corner, -1, 0);

						if (ret!=RTNORM)
							throw ret;

						corner2.z() += high;
					}

					//create solid
					checkSuccess(cmd_box_solid_or_mesh_create(corner1, corner2), RTNORM, 0);
					break;
				}
			}
		}
		while (false);
	}
	catch (int rc)
	{
		return rc;
	}

	//get last point
	resbuf setrb;
	setrb.restype=RT3DPOINT;
	ic_ptcpy(setrb.resval.rpoint, lastpt);
	SDS_setvar(NULL,DB_QLASTPOINT,&setrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);

    return RTNORM;
}
#if 0
class CCmdQueryMesh: public CCmdQueryEntity
{
public:
	CCmdQueryMesh()
	{
		init("Select polyface mesh: \n");
	}
	virtual bool isAppropriate() const
	{
		int flags;
		getEntity()->get_70(&flags);
		if (flags & 64)
			return true;
		//this is notnpolyface mesh
		return true;
	}
};

class CCmdReassemble: public CCmdCommand
{
	CCmdQueryMesh m_getMeshes;

	int getPolyfaceMeshData
	(
	vector<point>& points,
	vector<int>& faces
	);
	int getPolygonMeshData
	(
	vector<point>& points,
	vector<int>& faces
	);

	enum {eNone = 0, ePolyfaceMesh, ePolygonMesh};
	int getMeshType();

public:
	CCmdReassemble();
	virtual int run();
};

CCmdReassemble::CCmdReassemble()
{
	m_pStart = &m_getMeshes;
}

int CCmdReassemble::run()
{
	int rc = RTERROR;
	if (CCmdCommand::run())
	{
		vector<point> points;
		vector<int> faces;

		//traverse the mesh
		int meshType = getMeshType();
		switch (meshType)
		{
		case ePolyfaceMesh:
			if (!getPolyfaceMeshData(points, faces))
				return RTERROR;
			break;
		case ePolygonMesh:
			if (!getPolygonMeshData(points, faces))
				return RTERROR;
			break;
		default:
			return RTERROR;
		}

		CDbAcisEntityData obj;
		if (!CModeler::get()->reassembleFromMesh(obj, points, faces))
			return RTERROR;

		if (!create(obj))
			return RTERROR;
	}

	return m_rc;
}

//create solid from poliface, polygon meshes
short cmd_convert()
{
	if (!CModeler::get()->canModify())
		return RTERROR;

	CCmdReassemble cmd;
	return cmd.run();
}

int CCmdReassemble::getMeshType()
{
	sds_name ename;
	m_getMeshes.getEntityName(ename);

	db_handitem* item = reinterpret_cast<db_handitem*>(ename[0]);
	int type = item->ret_type();

	if (item->ret_type() != DB_POLYLINE)
		return eNone;

	int flag;
	item->get_70(&flag);

	if (flag & 16)
		return ePolygonMesh;
	else if (flag & 64)
		return ePolyfaceMesh;
	return eNone;
}

int CCmdReassemble::getPolyfaceMeshData
(
vector<point>& points,
vector<int>& faces
)
{
	sds_name ename;
	m_getMeshes.getEntityName(ename);

	while (true)
	{
		db_handitem* item = reinterpret_cast<db_handitem*>(ename[0]);

		int type = item->ret_type();

		if (type == DB_VERTEX)
		{
			int flag;
			item->get_70(&flag);

			int ivertex[4];
			item->get_71(&ivertex[0]);
			item->get_72(&ivertex[1]);
			item->get_73(&ivertex[2]);
			item->get_74(&ivertex[3]);

			enum
			{
				e3dPolygonMesh = 1<<6,
				ePolyfaceMeshVertex = 1<<7
			};
			if ((flag & (ePolyfaceMeshVertex|e3dPolygonMesh)) &&
				(ivertex[0] == 0))
			{
				point p;
				item->get_10(p);
				points.push_back(p);
			}
			else if (ivertex[0] != 0)
			{
				faces.push_back(ivertex[0]);
				faces.push_back(ivertex[1]);
				faces.push_back(ivertex[2]);
				faces.push_back(ivertex[3]);
			}
		}
		else if (type == DB_SEQEND)
			break;
		else if (type != DB_POLYLINE)
			return 0;

		if (RTNORM != sds_entnext_noxref(ename, ename))
			return 0;
	}
	return 1;
}

template <typename _FI, class _Ty>
void
iota(_FI _F, _FI _L, _Ty _V)
{
	while (_F != _L)
		*_F++ = _V++;
}

using namespace std;
class nindex_sorter: public binary_function<int, int, bool>
{
	const point* _f;
	const point* _l;
	vector<int> _ind[3];
	vector<int> _rind;
	int _i;

	typedef void(nindex_sorter::*sort_t)();
	sort_t _sort[3];

	bool _sort_with_tolerance;

	void sortZ();

	typedef std::vector<int>::iterator iint_t;

public:
	nindex_sorter(const point* f, const point* l)
		:_f(f), _l(l),_rind(_l-_f),_i(0),_sort_with_tolerance(false)
	{iota(_rind.begin(), _rind.end(), 0);
	 _ind[0].resize(_rind.size());
	 iota(_ind[0].begin(), _ind[0].end(), 0);
	 _sort[0] = _sort[1] = sort; _sort[2] = sortZ;}

	bool operator () (int i1, int i2) const
	{if (_sort_with_tolerance)
		return lt((*(_f+i1))[_i], (*(_f+i2))[_i]);
	else
		return (*(_f+i1))[_i] < (*(_f+i2))[_i];}

	void sort();

	const vector<int>& rind() const
	{return _rind;}
};

void nindex_sorter::sort()
{
	assert((_i >= 0) && (_i < 3));

	iint_t s = _ind[_i].begin(), e = _ind[_i].end();
	_sort_with_tolerance = false;
	std::sort(s, e, *this);
	_sort_with_tolerance = true;

	for (; s != e-1; ++s)
	{
		iint_t l = find_if(s+1, e, bind1st(*this, *s));
		if (l == (s+1))
			continue;

		_i++;
		_ind[_i].resize(0);
		copy(s, l, back_insert_iterator<vector<int> >(_ind[_i]));

		(this->*_sort[_i])();

		_i--;
	}
}

void nindex_sorter::sortZ()
{
	assert((_i >= 0) && (_i < 3));

	iint_t s = _ind[_i].begin(), e = _ind[_i].end();
	_sort_with_tolerance = false;
	std::sort(s, e, *this);
	_sort_with_tolerance = true;

	for (; s != e-1; ++s)
	{
		iint_t l = find_if(s+1, e, bind1st(*this, *s));
		if (l == (s+1))
			continue;

		for (iint_t i = s+1; i != l; ++i)
			_rind[*i] = _rind[*s];
	}
}

static
bool checkAndCorrectFace(int iv[4])
{
	int eq = 0;
	if (iv[0] == iv[2] || iv[1] == iv[3])
		return false;

	int* new_end = unique(iv, iv+4);
	int n = new_end - iv;
	assert(n > 0 && n < 5);
	if (*iv == *(new_end-1))
		n--;

	if (n < 3)
		return false;

	if (n == 3)
		iv[3] = -1;

	return true;
}

//array auto-destroyer
template <typename _Ty>
struct _destroy_array
{
	_Ty* m_p;
	_destroy_array(_Ty* p)
		:m_p(p){}
	~_destroy_array()
	{delete [] m_p;}
};

int CCmdReassemble::getPolygonMeshData
(
vector<point>& points,
vector<int>& faces
)
{
	sds_name ename;
	m_getMeshes.getEntityName(ename);

	//read polyline header
	db_handitem* dbitem = reinterpret_cast<db_handitem*>(ename[0]);
	if (dbitem->ret_type() != DB_POLYLINE)
		return 0;
	db_polyline* pline = static_cast<db_polyline*>(dbitem);

	int flag = pline->ret_70();
	if (!(flag & 16))
		//it's not polygon mesh
		return 0;

	//read mesh settings
	int nclosed = flag & 32;
	int mclosed = flag & 1;
	int mdim = pline->ret_71();
	int ndim = pline->ret_72();

	//allocate memory for vertices
	int npoint = mdim*ndim;
	point* mesh = new point[npoint];
	if (!mesh)
		return 0;

	_destroy_array<point> _destr_mesh(mesh);

	int iv = 0;
	do
	{
		if (RTNORM != sds_entnext_noxref(ename, ename))
			return 0;

		dbitem = reinterpret_cast<db_handitem*>(ename[0]);
		int type = dbitem->ret_type();

		if (type == DB_VERTEX)
		{
			int flag;
			dbitem->get_70(&flag);
			if (!(flag & 64))
				return 0;

			if (iv >= npoint)
				//number of vertices is greater then was considered
				return 0;
			mesh[iv++] = dbitem->retp_10();
		}
		else if (type == DB_SEQEND)
			break;
		else
			return 0;
	}
	while (true);

	//if mesh is closed, supplement with additional
	//analyse vertices, build faces
	int nfaces = (ndim-1)*(mdim-1) + (mclosed? ndim-1:0) + (nclosed? mdim-1:0);
	int nface = nclosed? ndim: ndim-1;
	int mface = mclosed? mdim: mdim-1;

	//allocate memory for faces
	int* af = new int[nfaces*4];
	if (!af)
		return 0;
	_destroy_array<int> _destr_faces(af);

	//init vertex indices
	for (int iface = 0; iface < nfaces; iface++)
	{
		//calculate face configuration:
		//[0]-->[1]
		//^      |
		//|		 v
		//[3]<--[2]
		int im = iface/nface;//row index
		int in = iface%nface;//collumn index
		af[4*iface+0] = im*ndim + in;
		af[4*iface+1] = im*ndim + (in+1)%ndim;
		af[4*iface+2] = ((im+1)%mdim)*ndim + (in+1)%ndim;
		af[4*iface+3] = ((im+1)%mdim)*ndim + in;
	}

	//exclude equal points from pool
	nindex_sorter sorter(mesh, mesh+npoint);
	sorter.sort();

	vector<int> newindices(npoint);
	fill(newindices.begin(), newindices.end(), -1);
	points.clear();
	const vector<int> rind = sorter.rind();

	for (iface = 0; iface < nfaces; iface++)
	{
		int* ov = af+(4*iface);
		int nv[4] = {rind[ov[0]], rind[ov[1]], rind[ov[2]], rind[ov[3]]};
		if (!checkAndCorrectFace(nv))
			continue;

		for (int i = 0; i < 4; i++)
		{
			const int ni = nv[i];
			if (ni < 0)
			{
				//the end of face
				assert(i == 3);
				faces.push_back(0);
			}
			else
			{
				if (newindices[ni] < 0)
				{
					newindices[ni] = points.size();
					points.push_back(mesh[ni]);
				}
				faces.push_back(newindices[ni]+1);
			}
		}
	}

	return 1;
}

#endif
