/* F:\BLD\PRJ\LIB\SDS\SDS_GEOM.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Misc Geometric Functions 
 * 
 */ 

//** Includes

// 6/98 including icad.h doesn't work for the dwgcnvt - therefore I only include it when SDS_SDSSTANDALONE
// isn't defined

#if defined(SDS_SDSSTANDALONE)
	#include <windows.h>
	#include "icadlib.h"/*DNT*/
	#include "sds.h"/*DNT*/
	#include "db.h"/*DNT*/
#else
	#include "Icad.h"/*DNT*/
	#include "cmds.h"/*DNT*/
#endif

/****************************************************************************/
/********************* Geometry Functions ***********************************/
/****************************************************************************/

// *****************************************
// This is an SDS External API
//
int 
sdsengine_inters(const sds_point ptFrom1, const sds_point ptTo1, 
			   const sds_point ptFrom2, const sds_point ptTo2, 
			   int swFinite, sds_point ptIntersection) 
	
	{
	//NOTE: THIS IS A FULL 3D INTERSECTION!
    int ret=RTNORM,fi1;
    sds_point respt,disp1,disp2,normal,from1,to1,from2,to2;
	sds_real fr1,dist1,dist2,fuzz2use;
	struct resbuf rbwcs,rbplane;

	fuzz2use=CMD_FUZZ*CMD_FUZZ;
	disp1[0]=ptTo1[0]-ptFrom1[0];
	disp1[1]=ptTo1[1]-ptFrom1[1];
	disp1[2]=ptTo1[2]-ptFrom1[2];
	disp2[0]=ptTo2[0]-ptFrom2[0];
	disp2[1]=ptTo2[1]-ptFrom2[1];
	disp2[2]=ptTo2[2]-ptFrom2[2];
	dist1=(disp1[0]*disp1[0]+disp1[1]*disp1[1]+disp1[2]*disp1[2]);
	dist2=(disp2[0]*disp2[0]+disp2[1]*disp2[1]+disp2[2]*disp2[2]);
	// Modified PK 24/04/2000 [
	// Reason: Fix for bug no. 47450  
	//first, check if they passed identical points for BOTH segments
	if(dist1<fuzz2use && dist2<fuzz2use){
		return(RTERROR); 
//		if(!icadRealEqual(ptFrom1[0],ptFrom2[0]))return(RTERROR);
//		else return(RTNORM);
	}
	// Modified PK 24/04/2000 ]
	//if one line is a point, define displacements so as to get normal to plane from cross prod
	if(dist1<fuzz2use){
		if(sds_distance(ptFrom1,ptFrom2)>sds_distance(ptFrom1,ptTo2)){
			disp1[0]=ptFrom1[0]-ptFrom2[0];
			disp1[1]=ptFrom1[1]-ptFrom2[1];
			disp1[1]=ptFrom1[2]-ptFrom2[2];
		}else{
			disp1[0]=ptFrom1[0]-ptTo2[0];
			disp1[1]=ptFrom1[1]-ptTo2[1];
			disp1[1]=ptFrom1[2]-ptTo2[2];
		}
	}else if(dist2<fuzz2use){
		if(sds_distance(ptFrom2,ptFrom1)>sds_distance(ptFrom2,ptTo1)){
			disp2[0]=ptFrom2[0]-ptFrom1[0];
			disp2[1]=ptFrom2[1]-ptFrom1[1];
			disp2[1]=ptFrom2[2]-ptFrom1[2];
		}else{
			disp1[0]=ptFrom2[0]-ptTo1[0];
			disp1[1]=ptFrom2[1]-ptTo1[1];
			disp1[1]=ptFrom2[2]-ptTo1[2];
		}
	}
	ic_crossproduct(disp1,disp2,normal);
	fr1=(normal[0]*normal[0]+normal[1]*normal[1]+normal[2]*normal[2]);

	if(fr1<fuzz2use){
		if(dist2<fuzz2use){
			//point 2 is along line 1's path
			ic_ptcpy(respt,ptFrom2);
			if(swFinite){
				if(icadDirectionEqual((double *)ptFrom1,(double *)ptTo1,(double *)ptFrom1,(double *)ptFrom2) &&
				   icadDirectionEqual((double *)ptTo1,(double *)ptFrom1,(double *)ptTo1,(double *)ptFrom2))ret=RTNORM;
			}
			goto exit;
		}else if(dist1<fuzz2use){
			//point 1 is along line 2's path
			ic_ptcpy(respt,ptFrom1);
			if(swFinite){
				if(icadDirectionEqual((double *)ptFrom2,(double *)ptTo2,(double *)ptFrom2,(double *)ptFrom1) &&
				   icadDirectionEqual((double *)ptTo2,(double *)ptFrom2,(double *)ptTo2,(double *)ptFrom1))ret=RTNORM;
				else ret=RTERROR;
			}
			goto exit;
		}else return(RTERROR);
	}
	fr1=sqrt(fr1);
	rbplane.resval.rpoint[0]=normal[0]/fr1;
	rbplane.resval.rpoint[1]=normal[1]/fr1;
	rbplane.resval.rpoint[2]=normal[2]/fr1;
	rbplane.restype=RT3DPOINT;
	rbwcs.restype=RTSHORT;
	rbwcs.resval.rint=0;
	//transform all points into plane of the lines
	sds_trans(ptFrom1,&rbwcs,&rbplane,0,from1);
	sds_trans(ptTo1,  &rbwcs,&rbplane,0,to1);
	sds_trans(ptFrom2,&rbwcs,&rbplane,0,from2);
	sds_trans(ptTo2,  &rbwcs,&rbplane,0,to2);

	//at this point, both lines must exist at the same elevation
	if(!icadRealEqual(from1[2],from2[2]))return(RTERROR);
	
	fi1=ic_linexline(from1,to1,from2,to2,respt);
	if(-1==fi1)return(RTERROR);
	if(swFinite && 3!=fi1)return(RTERROR);
	sds_trans(respt,&rbplane,&rbwcs,0,respt);

	exit:

	if(ret==RTNORM)	ic_ptcpy(ptIntersection,respt);
	return(ret);


	}

