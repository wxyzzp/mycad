/* LIB\SDS\SDS_XFORM.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * Geometry Functions
 *
 */

//** Includes
#include "Icad.h"/*DNT*/
#include "IcadApi.h"
#include "cmds.h"/*DNT*/
#include <fcntl.h>
#include "DbAcisEntity.h"
#include "Modeler.h"
#include "transf.h"
#include "CmdQueryTools.h"

extern bool SDS_DontBitBlt;
extern int SDS_AtNodeDrag;
void	 sds_xformpt(sds_point from, sds_point to, sds_matrix mat);

// *****************************************
// This is an SDS External API
//
int 
sdsengine_trans(
			  const sds_point		   ptVectOrPtFrom,
			  const struct sds_resbuf *prbCoordFrom,
			  const struct sds_resbuf *prbCoordTo,
			  int					   swVectOrDisp,
			  sds_point 			   ptVectOrPtTo) 
	{

  	if(SDS_CURDWG==NULL) 
		{
		return(RTERROR);
		}

	return SDS_CURDWG->trans((sds_real *)ptVectOrPtFrom,(struct resbuf *)prbCoordFrom,
		(struct resbuf *)prbCoordTo,swVectOrDisp,ptVectOrPtTo,NULL);
	}

// *****************************************
// This is an SDS External API
//
int 
sdsengine_xformss(const sds_name nmSetName, sds_matrix mxTransform) 
	{

	ASSERT( OnEngineThread() );

	return(SDS_progxformss(nmSetName,mxTransform,0));
	}

int SDS_ProgressPos;

int SDS_progxformss(const sds_name nmSetName, sds_matrix mxTransform,int useprogress) 
	{

	ASSERT( OnEngineThread() );

	if( nmSetName==NULL || 
		mxTransform==NULL ) 
		{
		return(RTERROR);
		}

	if(RTERROR==SDS_CallUserCallbackFunc(SDS_CBXFORMSS,(void *)nmSetName,(void *)mxTransform,NULL)) return(RTERROR);

	int nError,savprog=0;
	long lSetCount,ssLen;
	sds_name nmEntity;
	struct resbuf tilemode;

	SDS_getvar(NULL,DB_QTILEMODE,&tilemode,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

	nError=0;
	// This is for dimensions.
	SDS_AtNodeDrag=1;

	// Remove proxy entities from the selection set.
	long removedCount = 0;
	bool userNotified = false;
	for(lSetCount=0L;sds_ssname(nmSetName,lSetCount,nmEntity)==RTNORM;lSetCount++) 
	{
		if (IC_TYPE_FROM_ENAME(nmEntity) == DB_ACAD_PROXY_ENTITY) 
		{
			if (userNotified == false)
			{
				userNotified = true;
				sds_printf(ResourceString(CMD_ERR_DWGPROXY, "Proxy entities can not be edited." ));
			}
			removedCount++;
			sds_redraw(nmEntity,IC_REDRAW_UNHILITE);
			sds_ssdel(nmEntity, nmSetName);
		}
	}
	if (removedCount)
	{
		char fs1[81];
		sprintf(fs1, "%s%d", ResourceString(IDC_ICADSELECTDIA__ENTITIES__8, " Entities removed="), removedCount);
		sds_printf(fs1);
	}

	if(useprogress) 
		{
		sds_sslength(nmSetName,&ssLen);
		ExecuteUIAction( ICAD_WNDACTION_STARTPROG,0L);
		}

	if(useprogress && tilemode.resval.rint)
		{
		SDS_DontBitBlt=1;
		}

	for (lSetCount=0L; sds_ssname(nmSetName,lSetCount,nmEntity)==RTNORM; lSetCount++) 
		{
		if (sds_usrbrk()) 
			{
			SDS_DontBitBlt=0;
			SDS_BitBlt2Scr(1);
			return RTCAN;
			}

		if (SDS_xforment(nmEntity,NULL,mxTransform)) 
			{
			nError=1;
			}

		if(useprogress) 
			{
			SDS_ProgressPos=(int)(((double)lSetCount/ssLen)*100);
			if(savprog+5<=SDS_ProgressPos) 
				{
				savprog=SDS_ProgressPos;
				ExecuteUIAction( ICAD_WNDACTION_UPDATEPROG,0L);
				}
			}
		}

	if(useprogress && tilemode.resval.rint) 
		{
		SDS_DontBitBlt=0;
		SDS_BitBlt2Scr(1);
		}

	if(useprogress) 
		{
		ExecuteUIAction( ICAD_WNDACTION_ENDPROG,0L);
		}

	// This is for dimensions.
	SDS_AtNodeDrag=0;

	return ((nError) ? RTERROR : RTNORM);
	}

typedef double Matrix3x3[3][3];

// The following are support utilities for finding the inverse of a 3D transformation
/*********************************************************************/
// get the indices involved in the cofactor of a 3 x 3 matrix
static void get_indices(int n, int & n0, int & n1)
	{
   switch (n)
		{
		case 0:
			n0 = 1;
			n1 = 2;
			break;
		case 1:
			n0 = 0;
			n1 = 2;
			break;
		case 2:
			n0 = 0;
			n1 = 1;
			break;
		}
	}
/*********************************************************************/
static double cofactor(const Matrix3X3 m, int i, int j)
	{
	double c;
	ASSERT((0 <= i) && (i <= 2));
	ASSERT((0 <= j) && (j <= 2));
	double fac;
	if ( (i + j) % 2 == 0)
		fac = 1;
	else
		fac = -1;

	int i0, i1;
	int j0, j1;

	get_indices(i, i0, i1);
	get_indices(j, j0, j1);

	c = fac*(m[i0][j0]*m[i1][j1] - m[i1][j0]*m[i0][j1]);

	return c;
	}
/*********************************************************************/

double det(const Matrix3X3 m)
	{
    return (m[0][0]*cofactor(m,0,0) + 
		    m[0][1]*cofactor(m,0,1) + 
			m[0][2]*cofactor(m,0,2));
	}

// determine the sign of the determinant of the upper left 3 x 3 submatrix of an sds_matrix
bool IsDetPositive(sds_matrix wmat)
	{
	// get the upper 3 x 3 corner of wmat
	Matrix3x3 m;
	bool posdet;
	for (int i = 0; i < 3; i++)
		{
		for (int j = 0; j < 3; j++)
			{
			m[i][j] = wmat[i][j];
			}
		}

	if (det(m) > 0)
		posdet = true;
	else
		posdet = false;

	return posdet;
	}


void SDS_xformarc(sds_point oldextru, sds_point newextru, sds_matrix wmat, resbuf *ecs, resbuf *ucs, resbuf *center,
				 resbuf *radius, resbuf *start, resbuf *end) {

	sds_real iang,newsa,newea,newrad;
	sds_point newcent;
	sds_point ap0, ap1;
	sds_real ar[3];

	bool posdet = IsDetPositive(wmat);

	/* newcent */
	ic_ptcpy(ecs->resval.rpoint,oldextru);
	sds_trans(center->resval.rpoint,ecs,ucs,0,ap0);
	sds_xformpt(ap0,ap0,wmat);
	ic_ptcpy(ecs->resval.rpoint,newextru);
	sds_trans(ap0,ucs,ecs,0,newcent);

	/* iang (stays constant) */
	ar[0]=start->resval.rreal;
	ar[1]=end->resval.rreal;
	ic_normang(ar,ar+1);
	iang=ar[1]-ar[0];

	/* newsa and newrad */
	if (posdet==true)
		{
		ap0[0]=center->resval.rpoint[0]+
			radius->resval.rreal*cos(start->resval.rreal);
		ap0[1]=center->resval.rpoint[1]+
			radius->resval.rreal*sin(start->resval.rreal);
		ap0[2]=center->resval.rpoint[2];
		}
	else
		{
		ap0[0]=center->resval.rpoint[0]+
			radius->resval.rreal*cos(end->resval.rreal);
		ap0[1]=center->resval.rpoint[1]+
			radius->resval.rreal*sin(end->resval.rreal);
		ap0[2]=center->resval.rpoint[2];
		}
	ic_ptcpy(ecs->resval.rpoint,oldextru);
	sds_trans(ap0,ecs,ucs,0,ap1);
	sds_xformpt(ap1,ap1,wmat);
	ic_ptcpy(ecs->resval.rpoint,newextru);
	sds_trans(ap1,ucs,ecs,0,ap0);
	newsa=sds_angle(newcent,ap0);
	newrad=sqrt((ap0[0]-newcent[0])*(ap0[0]-newcent[0])+
				(ap0[1]-newcent[1])*(ap0[1]-newcent[1]));

	/* newea */
	newea=newsa+iang; ic_normang(&newea,NULL);

	/* Set the new vals: */
	ic_ptcpy(center->resval.rpoint,newcent);
	radius->resval.rreal=newrad;
	start->resval.rreal=newsa;
	end->resval.rreal=newea;





	
}


// ********************************************
// This is a hack to try and make sure that rotate, move and scale
// still work properly.  SDS_xforment used to do this matrix reformatting,
// now only this entry-point does.
//
// However, this reformatted matrix won't do proper 3D transfomations
//
// HACK HACK HACK
//
//
int SDS_simplexforment( sds_name ename, struct resbuf *argelist, sds_matrix genmat )
	{

	sds_real xscale,cs,sn;	
	sds_matrix wmat;
	
	/* Get X-scale from genmat: */
	xscale=sqrt(genmat[0][0]*genmat[0][0]+genmat[1][0]*genmat[1][0]);
	if (xscale<IC_ZRO)	
		{	
		return -2;	
		}

	/* Get rotation cosine and sine from genmat: */
	if ((cs=genmat[0][0]/xscale)<-1.0)	
		cs=-1.0;
	else if (cs>1.0)	
		cs=1.0;
		
		
	if ((sn=genmat[1][0]/xscale)<-1.0)	
		sn=-1.0;
	else if (sn>1.0)	
		sn=1.0;

	/* Make sure the working matrix is correct for simple */
	/* scale/Z-rotate/translate: */
	memset(wmat,0,sizeof(wmat));
	wmat[0][0]=wmat[1][1]=xscale*cs;
	wmat[0][1]=-(wmat[1][0]=xscale*sn);
	wmat[2][2]=xscale;
	wmat[3][3]=1.0;
	wmat[0][3]=genmat[0][3];
	wmat[1][3]=genmat[1][3];
	wmat[2][3]=genmat[2][3];
	
	return SDS_xforment( ename, argelist, wmat );
	}
  
// ***************
// HACK ALERT -- See SDS_simplexforment above, which reformats the input
// matrix to be compatible with the old version of SDS_xforment
//
// See commens on SDS_simplexforment
//	
int SDS_xforment(sds_name ename, struct resbuf *argelist, sds_matrix wmat) {
/*
**	This function transforms the entity passed from sds_xformss.
**
**	Adapted from the mirroring routine.
**
**	Returns:
**		 0 : OK
**		-1 : Calling error (something NULL)
**		-2 : Invalid scale specified by genmat.
**
**		(Unexpected errors may 'break' out of the loop and just not
**		transform that entity.)
*/

//	if(ename) {
//		db_handitem *hip=(db_handitem *)ename[0];
//		if(hip->ret_type()==DB_3DSOLID || hip->ret_type()==DB_REGION || hip->ret_type()==DB_BODY) {
////			sds_redraw(ename,IC_REDRAW_UNDRAW);
////			int ret;
////			SDS_DEMANDLOAD(SDS_ACISFILE,"ACIS_XformObject"/*DNT*/,(db_handitem *elp, sds_matrix genmat),(hip,genmat),ret)
////			SDS_UpdateEntDisp(ename,3);
////			sds_redraw(ename,IC_REDRAW_DRAW);
////			return(ret);
//			return RTNORM;
//		}
//	}
	int tolDimtxt = 0; //eed 1040 is not scaled expect tolerance dimstyle override DIMTXT(140) and DIMGAP(147).
	int complexent,ok2mod,rc,fi1,fi2;
	sds_real ar[3],xscale,cs,sn;
	sds_point oldextru,newextru,oldplextru,newplextru,ap[10];
	struct resbuf *elist,allapps,*rbp[10],*trbp1,wcs,ucs,ecs;
	struct resbuf *typelink,*extrulink,*thicklink,*flaglink,rb4extru;
	static sds_real sds_pl_2dxfelev;//needed so we maintain elev correctly on 2d plines
	static sds_point parent_extru;	//if argelist is a 2d vertex, we'll need to keep parent pline extru

	rc=0; elist=NULL;
	allapps.rbnext=NULL; allapps.restype=RTSTR; allapps.resval.rstring=const_cast<char*>("*"/*DNT*/);
	oldplextru[0]=oldplextru[1]=0.0; oldplextru[2]=1.0;
	ic_ptcpy(newplextru,oldplextru);

	if ((ename==NULL && argelist==NULL) || wmat==NULL)	
		{	
		rc=-1;	
		goto out;	
		}

	/* Set up resbufs for sds_trans(): */
	wcs.rbnext=ucs.rbnext=ecs.rbnext=NULL;
	wcs.restype=RTSHORT; wcs.resval.rint=0;
	ucs.restype=RTSHORT; ucs.resval.rint=1;
	ecs.restype=RT3DPOINT;


	xscale=sqrt( wmat[0][0]*wmat[0][0] +
				 wmat[1][0]*wmat[1][0] +	
				 wmat[2][0]*wmat[2][0] );


		/* Get rotation cosine and sine from genmat: */
	if ((cs=wmat[0][0]/xscale)<-1.0)	
		cs=-1.0;
	else if (cs>1.0)	
		cs=1.0;
		
		
	if ((sn=wmat[1][0]/xscale)<-1.0)	
		sn=-1.0;
	else if (sn>1.0)	
		sn=1.0;

	complexent=0;
	do {  /* At least once; loop for a complex entity if no argelist. */
		ok2mod=1;

		if(argelist) elist=argelist;
		else {
			if (elist!=NULL) { sds_relrb(elist); elist=NULL; }
			if ((elist=sds_entgetx(ename,&allapps))==NULL) break;
		}

		/* Get type, extrusion, and thickness data: */
		typelink=extrulink=thicklink=flaglink=NULL;
		oldextru[0]=oldextru[1]=newextru[0]=newextru[1]=0.0;
		oldextru[2]=newextru[2]=1.0;
		for (trbp1=elist; trbp1!=NULL; trbp1=trbp1->rbnext) {
			if ( trbp1->restype == 0 )	
				typelink=trbp1;
			else if (trbp1->restype== 39)
				thicklink=trbp1;
			else if (trbp1->restype== 70)
				flaglink=trbp1;
			else if (trbp1->restype==210) {
				extrulink=trbp1;
				ic_ptcpy(oldextru,extrulink->resval.rpoint);
				/* Make sure it's unit length: */
				ar[0]=sqrt(oldextru[0]*oldextru[0]+
						   oldextru[1]*oldextru[1]+
						   oldextru[2]*oldextru[2]);
				if (!icadRealEqual(ar[0],0.0)) {
					if (!icadRealEqual(ar[0],1.0)) {
						oldextru[0]/=ar[0];
						oldextru[1]/=ar[0];
						oldextru[2]/=ar[0];
					}
				} else { oldextru[0]=oldextru[1]=0.0; oldextru[2]=1.0; }
			}

								// HACK: strip out any layer name
								// since it will reset layers in POLYFACE faces
								// Layer never changes in xforment anyway
/*			  if ( trbp1->rbnext && trbp1->rbnext->restype == 8)
				{
				struct resbuf *layerResbuf = trbp1->rbnext;
				trbp1->rbnext = layerResbuf->rbnext;
				layerResbuf->rbnext = NULL;
				sds_relrb( layerResbuf);
				}  */
		}
		if (typelink==NULL || typelink->resval.rstring==NULL) break;

		if (thicklink!=NULL) thicklink->resval.rreal*=xscale;
		//if this is a 2d pline AND we got the data from a llist so we're not
		//	going to loop here, then keep the extrusion of the parent pline
		//	for the subsequent call with the pline's vertices
		if (argelist!=NULL && flaglink!=NULL && extrulink!=NULL &&
			!(flaglink->resval.rint & (IC_PLINE_3DPLINE | IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH)) &&
			strsame(typelink->resval.rstring,"POLYLINE"/*DNT*/))
			ic_ptcpy(parent_extru,extrulink->resval.rpoint);
		//if this is a 2d vertex which requires grabbing the extrusion from
		//	the parent pline, get it
		if (argelist!=NULL && flaglink !=NULL &&
			!(flaglink->resval.rint & (IC_VERTEX_3DPLINE | IC_VERTEX_3DMESHVERT | IC_VERTEX_FACELIST)) &&
			strsame(typelink->resval.rstring,"VERTEX"/*DNT*/)){
			//get the extrusion of the parent
			rb4extru.restype=210;
			ic_ptcpy(oldextru,parent_extru);
			ic_ptcpy(oldplextru,parent_extru);
			extrulink=&rb4extru;//point it at this resbuf & it will use this as vertex extru
		}
		if (argelist!=NULL && flaglink!=NULL && extrulink!=NULL &&
			strsame(typelink->resval.rstring,"LWPOLYLINE"/*DNT*/))
			{
			ic_ptcpy(parent_extru,extrulink->resval.rpoint); // just do all of these
			ic_ptcpy(oldextru,parent_extru);
			ic_ptcpy(oldplextru,parent_extru);
			}
		//if this is a 2d vertex which requires grabbing the extrusion from
		//	the parent pline, get it
		if (extrulink!=NULL) {
			/* Xform (0,0,0) and the oldextru to get newextru: */
			ap[0][0]=ap[0][1]=ap[0][2]=0.0;
			sds_trans(ap[0]   ,&wcs,&ucs,0,ap[2]);
			sds_trans(oldextru,&wcs,&ucs,0,ap[3]);
			sds_xformpt(ap[2],ap[2],wmat);
			sds_xformpt(ap[3],ap[3],wmat);
			sds_trans(ap[2],&ucs,&wcs,0,ap[0]);
			sds_trans(ap[3],&ucs,&wcs,0,ap[1]);
			newextru[0]=ap[1][0]-ap[0][0];
			newextru[1]=ap[1][1]-ap[0][1];
			newextru[2]=ap[1][2]-ap[0][2];
			/* Make sure it's unit length: */
			ar[0]=sqrt(newextru[0]*newextru[0]+
					   newextru[1]*newextru[1]+
					   newextru[2]*newextru[2]);
			if (!icadRealEqual(ar[0],0.0)) {
				if (!icadRealEqual(ar[0],1.0)) {
					newextru[0]/=ar[0];
					newextru[1]/=ar[0];
					newextru[2]/=ar[0];
				}
			} else { newextru[0]=newextru[1]=0.0; newextru[2]=1.0; }

			/* No reason we can't set the new extrusion right now: */
			ic_ptcpy(extrulink->resval.rpoint,newextru);
			//if we're getting info from parent entity, set newplextru
			if (argelist!=NULL && flaglink !=NULL &&
				!(flaglink->resval.rint & (IC_VERTEX_3DPLINE | IC_VERTEX_3DMESHVERT | IC_VERTEX_FACELIST)) &&
				strsame(typelink->resval.rstring,"VERTEX"/*DNT*/))
					ic_ptcpy(newplextru,newextru);
		}

		/* Save the old and new extrusions for the VERTEXs */
		/* (only 2D VERTEXs will use them): */
		if (!db_compareNames("POLYLINE"/*DNT*/,typelink->resval.rstring) ||
			!db_compareNames("LWPOLYLINE"/*DNT*/,typelink->resval.rstring)) {
			ic_ptcpy(oldplextru,oldextru);
			ic_ptcpy(newplextru,newextru);
		}


		/* Do each case: */

		memset(rbp,0,sizeof(rbp));

		if (!db_compareNames("LINE"/*DNT*/,typelink->resval.rstring)) {

			for (trbp1=elist; trbp1!=NULL; trbp1=trbp1->rbnext) {
				switch (trbp1->restype) {
					case  10: rbp[0]=trbp1; break;	/* 1st pt */
					case  11: rbp[1]=trbp1; break;	/* 2nd pt */
				}
			}
			for (fi1=0; fi1<2 && rbp[fi1]!=NULL; fi1++);
			if (fi1<2) break;  /* Expected group not found. */

			/* Both pts are in the WCS: */
			sds_trans(rbp[0]->resval.rpoint,&wcs,&ucs,0,ap[0]);
			sds_xformpt(ap[0],ap[0],wmat);
			sds_trans(ap[0],&ucs,&wcs,0,rbp[0]->resval.rpoint);
			sds_trans(rbp[1]->resval.rpoint,&wcs,&ucs,0,ap[0]);
			sds_xformpt(ap[0],ap[0],wmat);
			sds_trans(ap[0],&ucs,&wcs,0,rbp[1]->resval.rpoint);

		} else if (!db_compareNames("POINT"/*DNT*/,typelink->resval.rstring)) {

			for (trbp1=elist; trbp1!=NULL; trbp1=trbp1->rbnext) {
				switch (trbp1->restype) {
					case  10: rbp[0]=trbp1; break;	/* Point */
					case  50: rbp[1]=trbp1; break;	/* Rotation */
				}
			}
			for (fi1=0; fi1<2 && rbp[fi1]!=NULL; fi1++);
			if (fi1<2) break;  /* Expected group not found. */

			/* Point is in the WCS: */
			sds_trans(rbp[0]->resval.rpoint,&wcs,&ucs,0,ap[0]);
			sds_xformpt(ap[0],ap[0],wmat);
			sds_trans(ap[0],&ucs,&wcs,0,rbp[0]->resval.rpoint);

			/*
			**	Rotation.
			**
			**	Interesting.  ACAD does not adjust the 50-group value,
			**	so, the xformed copy doesn't seem to be oriented correctly.
			**	Okay.  We won't, either.  (But, if we ever want to,
			**	rbp[1] points to the 50-link.  (Maybe it's because it's
			**	not really a rotation.	It's "angle of X-axis for the
			**	UCS in effect when the POINT was drawn".)
			*/

		} else if (!db_compareNames("CIRCLE"/*DNT*/,typelink->resval.rstring)) {
			//let ap[8][0] be new rad
			//let ap[7] be new center

			for (trbp1=elist; trbp1!=NULL; trbp1=trbp1->rbnext) {
				switch (trbp1->restype) {
					case 10: rbp[0]=trbp1; break;  /* Center */
					case 40: rbp[1]=trbp1; break;  /* Radius */
				}
			}
			for (fi1=0; fi1<2 && rbp[fi1]!=NULL; fi1++);
			if (fi1<2) break;  /* Expected group not found. */

			/* center - ap[7] */
			ic_ptcpy(ecs.resval.rpoint,oldextru);
			sds_trans(rbp[0]->resval.rpoint,&ecs,&ucs,0,ap[0]);
			sds_xformpt(ap[0],ap[0],wmat);
			ic_ptcpy(ecs.resval.rpoint,newextru);
			sds_trans(ap[0],&ucs,&ecs,0,ap[7]);

			/* radius - ap[8][0] */
			ap[0][0]=rbp[0]->resval.rpoint[0]+rbp[1]->resval.rreal;
			ap[0][1]=rbp[0]->resval.rpoint[1];
			ap[0][2]=rbp[0]->resval.rpoint[2];
			ic_ptcpy(ecs.resval.rpoint,oldextru);
			sds_trans(ap[0],&ecs,&ucs,0,ap[1]);
			sds_xformpt(ap[1],ap[1],wmat);
			ic_ptcpy(ecs.resval.rpoint,newextru);
			sds_trans(ap[1],&ucs,&ecs,0,ap[0]);
			ap[8][0]=sqrt((ap[0][0]-ap[7][0])*(ap[0][0]-ap[7][0])+
						(ap[0][1]-ap[7][1])*(ap[0][1]-ap[7][1]));

			ic_ptcpy(rbp[0]->resval.rpoint,ap[7]);
			rbp[1]->resval.rreal=ap[8][0];
		} else if (!db_compareNames("VIEWPORT"/*DNT*/,typelink->resval.rstring)) {
			//if(!icadAngleEqual(sn,0.0)){
			//	//we can use angle f'n because we've lost precision and sine will never
			//	//	be +/-2pi.
			//	ok2mod=0;
			//	break;
			//}
			for (trbp1=elist; trbp1!=NULL; trbp1=trbp1->rbnext) {
				switch (trbp1->restype) {
					case 10: rbp[0]=trbp1; break;  /* Center */
					case 40: rbp[1]=trbp1; break;  /* Width  */
					case 41: rbp[2]=trbp1; break;  /* Height */
					case -3: break;//eed always last
				}

			}
			for (fi1=0; fi1<3 && rbp[fi1]!=NULL; fi1++);
			if (fi1<2) break;  /* Expected group not found. */

			ar[0]=rbp[2]->resval.rreal;//save exist ht
			ic_ptcpy(ap[1],rbp[0]->resval.rpoint);//save exist ctr

			/* new center (WCS)*/
			sds_trans(rbp[0]->resval.rpoint,&wcs,&ucs,0,ap[0]);
			sds_xformpt(ap[0],ap[0],wmat);
			sds_trans(ap[0],&ucs,&wcs,0,rbp[0]->resval.rpoint);

			/* newwid & newht */
			rbp[1]->resval.rreal*=xscale;
			rbp[2]->resval.rreal*=xscale;

			//if doing anything OTHER than scale, we need to stop here.  Scaling
			//	affects the viewport entity's eed.	Rotation & translation don't

			//TODO: get better handle on when we're translating
			//if(icadAngleEqual(sn,0.0) && icadRealEqual(wmat[0][3],0.0) &&
			//	 icadRealEqual(wmat[1][3],0.0) && icadRealEqual(wmat[2][3],0.0)){}
			//	//if rotating or moving object, don't adjust eed
			//
			if(icadAngleEqual(sn,0.0) && !icadRealEqual(xscale,1.0)){
				//scrictly a scale...

				// DP: I don't know why this code need here and what and when really stores in eed,
				// DP: but we had cases when VIEWPORT didn't have these eed in elist.
				// DP: So in these cases VIEWPORT scaling in paperspace didn't work because of break operator.
				// DP: I did some modifications to avoid that. 
				// DP: Modified code looks like to work well.
				//now update eed
				// Begin of old code.
				//for(trbp1=elist; trbp1!=NULL && trbp1->restype!=1040; trbp1=trbp1->rbnext);//1st 1040 gp
				//if(NULL==trbp1)break;
				//for(trbp1=trbp1->rbnext;trbp1!=NULL && trbp1->restype!=1040;trbp1=trbp1->rbnext);//go to 2nd 1040 gp
				//if(NULL==trbp1)break;
				//rbp[6]=trbp1;//size
				//for(trbp1=trbp1->rbnext;trbp1!=NULL && trbp1->restype!=1040;trbp1=trbp1->rbnext);//go to 3rd 1040 gp
				//if(NULL==trbp1)break;
				//rbp[7]=trbp1;//x ordinate
				//for(trbp1=trbp1->rbnext;trbp1!=NULL && trbp1->restype!=1040;trbp1=trbp1->rbnext);//go to 4th 1040 gp
				//if(NULL==trbp1)break;
				//rbp[8]=trbp1;//y ordinate
				//ar[2]=rbp[6]->resval.rreal/ar[0];//orig size/orig ht (ps/ms ratio)
				//rbp[6]->resval.rreal*=xscale;
				//rbp[7]->resval.rreal+=ar[2]*(rbp[0]->resval.rpoint[0]-ap[1][0]);
				//rbp[8]->resval.rreal+=ar[2]*(rbp[0]->resval.rpoint[1]-ap[1][1]);
				// End of old code.
				// Begin of modified code.
				for(trbp1=elist; trbp1!=NULL && trbp1->restype!=1040; trbp1=trbp1->rbnext);//1st 1040 gp
				if(trbp1)
				{
					for(trbp1=trbp1->rbnext;trbp1!=NULL && trbp1->restype!=1040;trbp1=trbp1->rbnext);//go to 2nd 1040 gp
					if(trbp1)
					{
						rbp[6]=trbp1;//size
						for(trbp1=trbp1->rbnext;trbp1!=NULL && trbp1->restype!=1040;trbp1=trbp1->rbnext);//go to 3rd 1040 gp
						if(trbp1)
						{
							rbp[7]=trbp1;//x ordinate
							for(trbp1=trbp1->rbnext;trbp1!=NULL && trbp1->restype!=1040;trbp1=trbp1->rbnext);//go to 4th 1040 gp
							if(trbp1)
							{
								rbp[8]=trbp1;//y ordinate
								ar[2]=rbp[6]->resval.rreal/ar[0];//orig size/orig ht (ps/ms ratio)
								rbp[6]->resval.rreal*=xscale;
								rbp[7]->resval.rreal+=ar[2]*(rbp[0]->resval.rpoint[0]-ap[1][0]);
								rbp[8]->resval.rreal+=ar[2]*(rbp[0]->resval.rpoint[1]-ap[1][1]);
							}
						}
					}
				}
				// End of modified code.
			}
	   } else if (!db_compareNames("SHAPE"/*DNT*/,typelink->resval.rstring)) {
			//newinspt=ap[7];

			for (trbp1=elist; trbp1!=NULL; trbp1=trbp1->rbnext) {
				switch (trbp1->restype) {
					case 10: rbp[0]=trbp1; break;  /* Ins pt */
					case 40: rbp[1]=trbp1; break;  /* Size */
					case 50: rbp[2]=trbp1; break;  /* Rotation */
				}
			}
			for (fi1=0; fi1<3 && rbp[fi1]!=NULL; fi1++);
			if (fi1<3) break;  /* Expected group not found. */

			/* Rotation (do it 1st before we change the other params) */
			/*
			**	Determine new rotation by xforming the ins pt
			**	and a pt 1 unit away in the rotation direction:
			*/
			ap[0][0]=rbp[0]->resval.rpoint[0]+
				cos(rbp[2]->resval.rreal);
			ap[0][1]=rbp[0]->resval.rpoint[1]+
				sin(rbp[2]->resval.rreal);
			ap[0][2]=rbp[0]->resval.rpoint[2];
			ic_ptcpy(ecs.resval.rpoint,oldextru);
			sds_trans(rbp[0]->resval.rpoint,&ecs,&ucs,0,ap[1]);
			sds_trans(ap[0],&ecs,&ucs,0,ap[2]);
			sds_xformpt(ap[1],ap[1],wmat);
			sds_xformpt(ap[2],ap[2],wmat);
			ic_ptcpy(ecs.resval.rpoint,newextru);
			sds_trans(ap[1],&ucs,&ecs,0,ap[7]);
			sds_trans(ap[2],&ucs,&ecs,0,ap[0]);
			rbp[2]->resval.rreal=sds_angle(ap[7],ap[0]);

			/* Ins pt */
			ic_ptcpy(rbp[0]->resval.rpoint,ap[7]);

			/* Size: */
			rbp[1]->resval.rreal*=xscale;

		} else if (!db_compareNames("TEXT"/*DNT*/  ,typelink->resval.rstring) ||
				   !db_compareNames("ATTDEF"/*DNT*/,typelink->resval.rstring) ||
				   !db_compareNames("ATTRIB"/*DNT*/,typelink->resval.rstring)) {

			int att,defjust;
			sds_real newrot,newht;
			//new10=ap[7],new11=ap[8];

			att=(toupper(*typelink->resval.rstring)=='A');	/* ATTDEF or ATTRIB */

			for (trbp1=elist; trbp1!=NULL; trbp1=trbp1->rbnext) {
				switch (trbp1->restype) {
					case 10: rbp[0]=trbp1; break;  /* Ins pt */
					case 11: rbp[1]=trbp1; break;  /* Align pt */
					case 50: rbp[2]=trbp1; break;  /* Rotation */
					case 71: rbp[3]=trbp1; break;  /* Generation */
					case 72: rbp[4]=trbp1; break;  /* Halign */
					case 73: case 74:			   /* Valign */
						if (trbp1->restype==73+att) rbp[5]=trbp1;
						break;
					case 40: rbp[6]=trbp1; break;  /* Text size */
				}
			}
			for (fi1=0; fi1<7 && rbp[fi1]!=NULL; fi1++);
			if (fi1<7) break;  /* Expected group not found. */

			defjust=(!rbp[4]->resval.rint && !rbp[5]->resval.rint);

			ic_normang(&rbp[2]->resval.rreal,NULL);  /* Make sure */

			/*
			**	Determine new rotation and text size by xforming the ins pt
			**	and a pt 1 unit away in the rotation direction:
			*/
			ap[0][0]=rbp[0]->resval.rpoint[0]+
				cos(rbp[2]->resval.rreal);
			ap[0][1]=rbp[0]->resval.rpoint[1]+
				sin(rbp[2]->resval.rreal);
			ap[0][2]=rbp[0]->resval.rpoint[2];
			ic_ptcpy(ecs.resval.rpoint,oldextru);
			sds_trans(rbp[0]->resval.rpoint,&ecs,&ucs,0,ap[1]);
			sds_trans(ap[0],&ecs,&ucs,0,ap[2]);
			sds_xformpt(ap[1],ap[1],wmat);
			sds_xformpt(ap[2],ap[2],wmat);
			ic_ptcpy(ecs.resval.rpoint,newextru);
			sds_trans(ap[1],&ucs,&ecs,0,ap[7]);
			sds_trans(ap[2],&ucs,&ecs,0,ap[0]);
			newrot=sds_angle(ap[7],ap[0]);
			ic_normang(&newrot,NULL);  /* Precaution */
			newht=rbp[6]->resval.rreal*
				sqrt((ap[0][0]-ap[7][0])*(ap[0][0]-ap[7][0])+
					 (ap[0][1]-ap[7][1])*(ap[0][1]-ap[7][1]));

			/* Align pt */
			if (defjust) {	/* For lower left, 11-pt must be (0,0,elev). */
				ap[8][0]=ap[8][1]=0.0;
				ap[8][2]=ap[7][2];
			} else {
				ic_ptcpy(ecs.resval.rpoint,oldextru);
				sds_trans(rbp[1]->resval.rpoint,&ecs,&ucs,0,ap[0]);
				sds_xformpt(ap[0],ap[0],wmat);
				ic_ptcpy(ecs.resval.rpoint,newextru);
				sds_trans(ap[0],&ucs,&ecs,0,ap[8]);
			}

			ic_ptcpy(rbp[0]->resval.rpoint,ap[7]);
			ic_ptcpy(rbp[1]->resval.rpoint,ap[8]);
			rbp[2]->resval.rreal=newrot;
			rbp[6]->resval.rreal=newht;

		} else if (!db_compareNames("ARC"/*DNT*/,typelink->resval.rstring)) {
			for (trbp1=elist; trbp1!=NULL; trbp1=trbp1->rbnext) {
				switch (trbp1->restype) {
					case 10: rbp[0]=trbp1; break;  /* Center */
					case 40: rbp[1]=trbp1; break;  /* Radius */
					case 50: rbp[2]=trbp1; break;  /* Start angle */
					case 51: rbp[3]=trbp1; break;  /* End angle */
				}
			}
			for (fi1=0; fi1<4 && rbp[fi1]!=NULL; fi1++);
			if (fi1<4) break;  /* Expected group not found. */

			SDS_xformarc(oldextru, newextru, wmat, &ecs, &ucs, rbp[0], rbp[1], rbp[2], rbp[3]);

		} else if (!db_compareNames("ELLIPSE"/*DNT*/,typelink->resval.rstring)) {
			//newcent=ap[7],newstart=ap[8];

			for (trbp1=elist; trbp1!=NULL; trbp1=trbp1->rbnext) {
				switch (trbp1->restype) {
					case 10: rbp[0]=trbp1; break;  /* Center */
					case 11: rbp[1]=trbp1; break;  /* Vector */
					case 40: rbp[2]=trbp1; break;  /* Ratio */
					case 41: rbp[3]=trbp1; break;  /* Start angle */
					case 42: rbp[4]=trbp1; break;  /* End angle */
				}
			}
			for (fi1=0; fi1<5 && rbp[fi1]!=NULL; fi1++);
			if (fi1<5) break;  /* Expected group not found. */

			/* new center - ap[7] */
			sds_trans(rbp[0]->resval.rpoint,&wcs,&ucs,0,ap[0]);
			sds_xformpt(ap[0],ap[0],wmat);
			sds_trans(ap[0],&ucs,&wcs,0,ap[7]);
			//new axis vector				;
			ap[1][0]=rbp[0]->resval.rpoint[0]+rbp[1]->resval.rpoint[0];
			ap[1][1]=rbp[0]->resval.rpoint[1]+rbp[1]->resval.rpoint[1];
			ap[1][2]=rbp[0]->resval.rpoint[2]+rbp[1]->resval.rpoint[2];
			sds_trans(ap[1],&wcs,&ucs,0,ap[2]);
			sds_xformpt(ap[2],ap[2],wmat);
			sds_trans(ap[2],&ucs,&wcs,0,ap[8]);
			rbp[1]->resval.rpoint[0]=ap[8][0]-ap[7][0];
			rbp[1]->resval.rpoint[1]=ap[8][1]-ap[7][1];
			rbp[1]->resval.rpoint[2]=ap[8][2]-ap[7][2];
			ic_ptcpy(rbp[0]->resval.rpoint,ap[7]);

			// Check if we have an orientation-reversing transformation
			// If so, modify the start and end angles accordingly

			// Warning:  This is only valid if the transformation is one that
			// is orthogonal and scales uniformly.
			bool posdet = IsDetPositive(wmat);
			if (posdet == false)
				{
				double oldStartAngle = rbp[3]->resval.rreal;
				double oldEndAngle = rbp[4]->resval.rreal;
				rbp[3]->resval.rreal = -oldEndAngle;
				rbp[4]->resval.rreal = -oldStartAngle;
				}


		} else if (!db_compareNames("TRACE"/*DNT*/ ,typelink->resval.rstring) ||
			!db_compareNames("SOLID"/*DNT*/,typelink->resval.rstring) ||
			!db_compareNames("3DFACE"/*DNT*/,typelink->resval.rstring)) {

			int face;

			face=(*typelink->resval.rstring=='3');

			for (trbp1=elist; trbp1!=NULL; trbp1=trbp1->rbnext) {
				switch (trbp1->restype) {
					case 10: rbp[0]=trbp1; break;
					case 11: rbp[1]=trbp1; break;
					case 12: rbp[2]=trbp1; break;
					case 13: rbp[3]=trbp1; break;
				}
			}
			for (fi1=0; fi1<4 && rbp[fi1]!=NULL; fi1++);
			if (fi1<4) break;  /* Expected group not found. */

			for (fi1=0; fi1<4; fi1++) {
				if (face) {
					sds_trans(rbp[fi1]->resval.rpoint,&wcs,&ucs,0,ap[0]);
				} else {
					ic_ptcpy(ecs.resval.rpoint,oldextru);
					sds_trans(rbp[fi1]->resval.rpoint,&ecs,&ucs,0,ap[0]);
				}
				sds_xformpt(ap[0],ap[0],wmat);
				if (face) {
					sds_trans(ap[0],&ucs,&wcs,0,rbp[fi1]->resval.rpoint);
				} else {
					ic_ptcpy(ecs.resval.rpoint,newextru);
					sds_trans(ap[0],&ucs,&ecs,0,rbp[fi1]->resval.rpoint);
				}
			}

		} else if (!db_compareNames("DIMENSION"/*DNT*/,typelink->resval.rstring)) {

			/*
			**	° Translation only for now.  Scaling and rotating
			**	need to alter the anonymous block.
			*/
			//ok=1;
			//for (fi1=0; fi1<3 && ok; fi1++)  /* 1st 3 rows */
			//	  for (fi2=0; fi2<3 && ok; fi2++)  /* 1st 3 cols */
			//		  if (wmat[fi1][fi2]!=((fi1==fi2) ? 1.0 : 0.0)) ok=0;
			//if (!ok) break;

			for (trbp1=elist; trbp1!=NULL; trbp1=trbp1->rbnext) {
				switch (trbp1->restype) {
					case 70: rbp[0]=trbp1; break;  /* DIM type */
					case 10: rbp[1]=trbp1; break;  /* WCS */
					case 13: rbp[2]=trbp1; break;  /* WCS */
					case 14: rbp[3]=trbp1; break;  /* WCS */
					case 15: rbp[4]=trbp1; break;  /* WCS */
					case 11: rbp[5]=trbp1; break;  /* ECS */
					case 16: rbp[7]=trbp1; break;  /* ECS */
					case 50: rbp[8]=trbp1; break;  /* WCS (angle)*/
// Don't transform the base point for the dim. Removed
//					case 12: rbp[6]=trbp1; break;  /* ECS */
				}
			}
			if (rbp[0]==NULL) break;  /* Expected group not found. */

			/* Check the dim type and NULL the rbp[] elements that */
			/* don't apply for that type: */
			switch (rbp[0]->resval.rint&0x0007) {
				case 0: case 1:				   rbp[4]=rbp[7]=NULL; break;
				case 6:					rbp[4]=rbp[7]=rbp[8]=NULL; break;
				case 3: case 4: 		rbp[2]=rbp[3]=rbp[8]=NULL; break;
				case 5: 					   rbp[7]=rbp[8]=NULL; break;
			}

			/* Do the WCS points: */
			for (fi1=1; fi1<5; fi1++) {
				if (rbp[fi1]==NULL) continue;
				sds_trans(rbp[fi1]->resval.rpoint,&wcs,&ucs,0,ap[0]);
				sds_xformpt(ap[0],ap[0],wmat);
				sds_trans(ap[0],&ucs,&wcs,0,rbp[fi1]->resval.rpoint);
			}

			/*
			**	Do the ECS points.	(Make sure they all have the same
			**	elevation (use the 12's) -- ACAD seems to take the 16's
			**	elevation even when it's a DIM type that doesn't use the
			**	16):
			*/
			for (fi1=5; fi1<8; fi1++) {
				if (rbp[fi1]==NULL) continue;
				ic_ptcpy(ecs.resval.rpoint,oldextru);
				sds_trans(rbp[fi1]->resval.rpoint,&ecs,&ucs,0,ap[0]);
				sds_xformpt(ap[0],ap[0],wmat);
				ic_ptcpy(ecs.resval.rpoint,newextru);
				sds_trans(ap[0],&ucs,&ecs,0,rbp[fi1]->resval.rpoint);
			}
			if (rbp[6]!=NULL) {
				if (rbp[5]!=NULL) rbp[5]->resval.rpoint[2]=
					rbp[6]->resval.rpoint[2];
				if (rbp[7]!=NULL) rbp[7]->resval.rpoint[2]=
					rbp[6]->resval.rpoint[2];
			}

			if (rbp[8]!=NULL){
				ic_ptcpy(ecs.resval.rpoint,oldextru);
				ap[0][0]=cos(rbp[8]->resval.rreal);
				ap[0][1]=sin(rbp[8]->resval.rreal);
				ap[0][2]=0.0;
				sds_trans(ap[0],&ecs,&ucs,1,ap[1]);
				ap[0][2]=atan2(ap[1][1],ap[1][0]);
				ap[0][2]+=atan2(sn,cs);
				ap[1][0]=cos(ap[0][2]);
				ap[1][1]=sin(ap[0][2]);
				ap[1][2]=0.0;
				ic_ptcpy(ecs.resval.rpoint,newextru);
				sds_trans(ap[1],&ucs,&ecs,1,ap[0]);
				rbp[8]->resval.rreal=atan2(ap[0][1],ap[0][0]);
				ic_normang(&rbp[8]->resval.rreal,NULL);
			}

		} else if (!db_compareNames("POLYLINE"/*DNT*/,typelink->resval.rstring)) {

			for (trbp1=elist; trbp1!=NULL; trbp1=trbp1->rbnext) {
				switch (trbp1->restype) {
					case 10: rbp[0]=trbp1; break;  /* (0,0,elev) */
					case 70: rbp[1]=trbp1; break;  /* Flags */
					case 40: rbp[2]=trbp1; break;  /* Default starting width */
					case 41: rbp[3]=trbp1; break;  /* Default ending   width */
				}
			}
			for (fi1=0; fi1<4 && rbp[fi1]!=NULL; fi1++);
			if (fi1<4) break;  /* Expected group not found. */

			if (!(rbp[1]->resval.rint&0x58)) {	/* 2D */
				/* The (0,0,elev) pt should transform to the proper */
				/* elevation, but we should zero X and Y: */
				//NOTE!!! Keep Z from 2D pline to plant onto 2D vertices!!
				sds_pl_2dxfelev=rbp[0]->resval.rpoint[2];
				ic_ptcpy(ecs.resval.rpoint,oldextru);
				sds_trans(rbp[0]->resval.rpoint,&ecs,&ucs,0,ap[0]);
				sds_xformpt(ap[0],ap[0],wmat);
				ic_ptcpy(ecs.resval.rpoint,newextru);
				sds_trans(ap[0],&ucs,&ecs,0,rbp[0]->resval.rpoint);
				rbp[0]->resval.rpoint[0]=rbp[0]->resval.rpoint[1]=0.0;

				/* The default widths */
				rbp[2]->resval.rreal*=xscale;
				rbp[3]->resval.rreal*=xscale;
			}

			complexent=1;

		} else if (!db_compareNames("VERTEX"/*DNT*/,typelink->resval.rstring)) {

			for (trbp1=elist; trbp1!=NULL; trbp1=trbp1->rbnext) {
				switch (trbp1->restype) {
					case 10: rbp[0]=trbp1; break;  /* Point */
					case 42: rbp[1]=trbp1; break;  /* Bulge */
					case 70: rbp[2]=trbp1; break;  /* Flags */
					case 50: rbp[3]=trbp1; break;  /* Tangent dir */
					case 40: rbp[4]=trbp1; break;  /* Starting width */
					case 41: rbp[5]=trbp1; break;  /* Ending width */
				}
			}
			for (fi1=0; fi1<6 && rbp[fi1]!=NULL; fi1++);
			if (fi1<6) break;  /* Expected group not found. */

			if (rbp[2]->resval.rint&0xE0) {  /* Some type of 3D */

				if (!(rbp[2]->resval.rint&0x80) ||
					 (rbp[2]->resval.rint&0x40)) {
					/* A 3D (WCS) point (not a PFACE face def) */

					sds_trans(rbp[0]->resval.rpoint,&wcs,&ucs,0,ap[0]);
					sds_xformpt(ap[0],ap[0],wmat);
					sds_trans(ap[0],&ucs,&wcs,0,rbp[0]->resval.rpoint);
				}

			} else {  /* 2D */
				sds_point newinspt;

				/* Tangent direction (do it 1st before we change the pt) */
				/*
				**	Determine new direction by xforming the pt
				**	and a pt 1 unit away in the tangent direction:
				*/

				fi1=((rbp[2]->resval.rint&2)!=0);  /* Is the 50 valid? */

				ic_ptcpy(ecs.resval.rpoint,oldplextru);
				rbp[0]->resval.rpoint[2]=sds_pl_2dxfelev;//set elev from parent entity
				sds_trans(rbp[0]->resval.rpoint,&ecs,&ucs,0,ap[1]);
				sds_xformpt(ap[1],ap[1],wmat);
				if (fi1) {
					ap[0][0]=rbp[0]->resval.rpoint[0]+
						cos(rbp[3]->resval.rreal);
					ap[0][1]=rbp[0]->resval.rpoint[1]+
						sin(rbp[3]->resval.rreal);
					ap[0][2]=rbp[0]->resval.rpoint[2];
					sds_trans(ap[0],&ecs,&ucs,0,ap[2]);
					sds_xformpt(ap[2],ap[2],wmat);
				}
				ic_ptcpy(ecs.resval.rpoint,newplextru);
				sds_trans(ap[1],&ucs,&ecs,0,newinspt);
				if (fi1) {
					sds_trans(ap[2],&ucs,&ecs,0,ap[0]);
					rbp[3]->resval.rreal=sds_angle(newinspt,ap[0]);
				}

				/* Take the xformed pt: */
				ic_ptcpy(rbp[0]->resval.rpoint,newinspt);

				/* Scale the widths: */
				rbp[4]->resval.rreal*=xscale;
				rbp[5]->resval.rreal*=xscale;
			}

		} else if (!db_compareNames("LWPOLYLINE"/*DNT*/,typelink->resval.rstring))
			{

			for (trbp1=elist; trbp1!=NULL; trbp1=trbp1->rbnext) {
				switch (trbp1->restype) {
					case 43: rbp[1]=trbp1; break;  /* constant width */
					}
			}

		// compute new elevation
			sds_point elevpt={0,0,0};
			resbuf *elevrb=ic_ret_assoc(elist,38);
			if (elevrb)
				elevpt[2]=elevrb->resval.rreal;
			/* The (0,0,elev) pt should transform to the proper */
			/* elevation, but we should zero X and Y: */
			ic_ptcpy(ecs.resval.rpoint,oldextru);
			sds_trans(elevpt,&ecs,&ucs,0,ap[0]);
			sds_xformpt(ap[0],ap[0],wmat);
			ic_ptcpy(ecs.resval.rpoint,newextru);
			sds_trans(ap[0],&ucs,&ecs,0,elevpt);
			elevrb->resval.rreal=elevpt[2];

			/* The constant width */
			if (rbp[1])
				rbp[1]->resval.rreal*=xscale;
			
			resbuf *rbpoint, *rbstwid, *rbendwid, *rbextru;
			rbpoint=ic_ret_assoc(elist,10);
			rbextru=ic_ret_assoc(elist,210);

			while (rbpoint && rbpoint->restype==10)
				{
				rbstwid=rbpoint->rbnext;
				rbendwid=rbstwid->rbnext;

				sds_point newinspt;
				ic_ptcpy(ecs.resval.rpoint,oldextru);

				sds_trans(rbpoint->resval.rpoint,&ecs,&ucs,0,ap[1]);
				sds_xformpt(ap[1],ap[1],wmat);
				ic_ptcpy(ecs.resval.rpoint,newextru);
				sds_trans(ap[1],&ucs,&ecs,0,newinspt);

				/* Take the xformed pt: */
				ic_ptcpy(rbpoint->resval.rpoint,newinspt);

				/* Scale the widths: */
				rbstwid->resval.rreal *=xscale;
				rbendwid->resval.rreal*=xscale;
				rbpoint=rbpoint->rbnext->rbnext->rbnext->rbnext;
				}

			}
		else if (!db_compareNames("SEQEND"/*DNT*/,typelink->resval.rstring)) {

			complexent=0;

		} else if (!db_compareNames("INSERT"/*DNT*/,typelink->resval.rstring)) {
			//newinspt=ap[7];

			for (trbp1=elist; trbp1!=NULL; trbp1=trbp1->rbnext) {
				switch (trbp1->restype) {
					case 10: rbp[0]=trbp1; break;  /* Ins pt */
					case 50: rbp[1]=trbp1; break;  /* Rotation */
					case 41: rbp[2]=trbp1; break;  /* X-scale */
					case 42: rbp[3]=trbp1; break;  /* Y-scale */
					case 43: rbp[4]=trbp1; break;  /* Z-scale */
					case 66: rbp[5]=trbp1; break;  /* Atts follow */
												   /*	MAY NOT BE PRESENT */
				}
			}
			/* 1 less because the last one may not be present: */
			for (fi1=0; fi1<5 && rbp[fi1]!=NULL; fi1++);
			if (fi1<5) break;  /* Expected group not found. */

			/* Rotation (do it 1st before we change the other params) */
			/*
			**	Determine new rotation by xforming the ins pt
			**	and a pt 1 unit away in the rotation direction:
			*/
			ap[0][0]=rbp[0]->resval.rpoint[0]+
				cos(rbp[1]->resval.rreal);
			ap[0][1]=rbp[0]->resval.rpoint[1]+
				sin(rbp[1]->resval.rreal);
			ap[0][2]=rbp[0]->resval.rpoint[2];
			ic_ptcpy(ecs.resval.rpoint,oldextru);
			sds_trans(rbp[0]->resval.rpoint,&ecs,&ucs,0,ap[1]);
			sds_trans(ap[0],&ecs,&ucs,0,ap[2]);
			sds_xformpt(ap[1],ap[1],wmat);
			sds_xformpt(ap[2],ap[2],wmat);
			ic_ptcpy(ecs.resval.rpoint,newextru);
			sds_trans(ap[1],&ucs,&ecs,0,ap[7]);
			sds_trans(ap[2],&ucs,&ecs,0,ap[0]);

			// special handling for mirroring transformations
			double fac;
			if (IsDetPositive(wmat))
				{
				fac = 1;
				rbp[1]->resval.rreal=sds_angle(ap[7],ap[0]);
				}
			else
				{
				fac = -1;
				rbp[1]->resval.rreal=sds_angle(ap[0],ap[7]);
				}

			/* Ins pt */
			ic_ptcpy(rbp[0]->resval.rpoint,ap[7]);

			/* Scale the scales: */
			rbp[2]->resval.rreal*=xscale*fac;
			rbp[3]->resval.rreal*=xscale;
			rbp[4]->resval.rreal*=xscale;

			/* Check for "ATTRIBs follow" (complex entity): */
			complexent=(rbp[5]!=NULL && rbp[5]->resval.rint);

		} else if (!db_compareNames("MTEXT"/*DNT*/,typelink->resval.rstring)){

			for (trbp1=elist; trbp1!=NULL; trbp1=trbp1->rbnext) {
				switch (trbp1->restype) {
					case  10: rbp[0]=trbp1; break;	/* 1st pt */
					case  11: rbp[1]=trbp1; break;	/* x axis vect */
					case  40: rbp[2]=trbp1; break;	/* text ht */
					case  43: rbp[3]=trbp1; break;	/* total text height within rect(Optional) */
					case  41: rbp[4]=trbp1; break;	/* rect width */
					case  42: rbp[5]=trbp1; break;	/* total text width within rect(Optional) */
					case  50: rbp[6]=trbp1; break;	/* rotation ang (Optional)*/
				}
			}
			for (fi1=fi2=0; fi1<6; fi1++){if(rbp[fi1])fi2++;}
			if (fi2<3) break;  /* Expected group not found. */

			/* 1st pt (WCS): */
			//keep ap[0] as CURRENT insertion pt in WCS
			ic_ptcpy(ap[0],rbp[0]->resval.rpoint);
			sds_trans(ap[0],&wcs,&ucs,0,ap[1]);
			sds_xformpt(ap[1],ap[1],wmat);
			sds_trans(ap[1],&ucs,&wcs,0,rbp[0]->resval.rpoint);

			//now do all scale-related items
			sds_trans(rbp[1]->resval.rpoint,&wcs,&ucs,0,ap[8]);
			ap[9][0]=atan2(ap[8][1],ap[8][0]);	//ap[9][0] is angle of mtext in ucs
			ap[9][1]=sin(ap[9][0]);				//ap[9][1] is sine of text angle in ucs
			ap[9][2]=cos(ap[9][0]);				//ap[9][2] is cosine of text angle in ucs
			//heights
			for(fi1=2;fi1<4;fi1++){
				if(NULL==rbp[fi1])continue;
				ap[1][0]=ap[0][0]-rbp[fi1]->resval.rreal*ap[9][1];
				ap[1][1]=ap[0][1]+rbp[fi1]->resval.rreal*ap[9][2];
				ap[1][2]=ap[0][2];
				sds_trans(ap[1],&wcs,&ucs,0,ap[2]);
				sds_xformpt(ap[2],ap[2],wmat);
				sds_trans(ap[2],&ucs,&wcs,0,ap[2]);
				rbp[fi1]->resval.rreal=sds_distance(ap[2],rbp[0]->resval.rpoint);
			}
			//widths
			for(fi1=4;fi1<6;fi1++){
				if(NULL==rbp[fi1])continue;
				ap[1][0]=ap[0][0]+rbp[fi1]->resval.rreal*ap[9][2];
				ap[1][1]=ap[0][1]+rbp[fi1]->resval.rreal*ap[9][1];
				ap[1][2]=ap[0][2];
				sds_trans(ap[1],&wcs,&ucs,0,ap[2]);
				sds_xformpt(ap[2],ap[2],wmat);
				sds_trans(ap[2],&ucs,&wcs,0,ap[2]);
				rbp[fi1]->resval.rreal=sds_distance(ap[2],rbp[0]->resval.rpoint);
			}
			//rotation angle
			if(rbp[6]!=NULL){
				//take the 50 out of the entmake.  It's not a std angle calc (it's
				//	dependent upon UCS).  It's sufficient to use the 11.
				for (trbp1=elist; trbp1!=NULL; trbp1=trbp1->rbnext) {
					if(trbp1->rbnext==rbp[6]){
						trbp1->rbnext=rbp[6]->rbnext;
						rbp[6]->rbnext=NULL;
						sds_relrb(rbp[6]);
						rbp[6]=trbp1=NULL;
						break;
					}
				}
			}

			/* X axis vect (WCS): */
			ap[0][0]=ap[0][1]=ap[0][2]=0.0;
			sds_trans(ap[0] 			   ,&wcs,&ucs,0,ap[2]);
			sds_trans(rbp[1]->resval.rpoint,&wcs,&ucs,0,ap[3]);
			sds_xformpt(ap[2],ap[2],wmat);
			sds_xformpt(ap[3],ap[3],wmat);
			sds_trans(ap[2],&ucs,&wcs,0,ap[0]);
			sds_trans(ap[3],&ucs,&wcs,0,ap[1]);
			rbp[1]->resval.rpoint[0]=ap[1][0]-ap[0][0];
			rbp[1]->resval.rpoint[1]=ap[1][1]-ap[0][1];
			rbp[1]->resval.rpoint[2]=ap[1][2]-ap[0][2];
			/* Make sure it's unit length: */
			ar[0]=sqrt(rbp[1]->resval.rpoint[0]*rbp[1]->resval.rpoint[0]+
					   rbp[1]->resval.rpoint[1]*rbp[1]->resval.rpoint[1]+
					   rbp[1]->resval.rpoint[2]*rbp[1]->resval.rpoint[2]);
			if (!icadRealEqual(ar[0],0.0)) {
				if (!icadRealEqual(ar[0],1.0)) {
					rbp[1]->resval.rpoint[0]/=ar[0];
					rbp[1]->resval.rpoint[1]/=ar[0];
					rbp[1]->resval.rpoint[2]/=ar[0];
				}
			} else {  /* Use (1,0,0) as default. */
				rbp[1]->resval.rpoint[0]=1.0;
				rbp[1]->resval.rpoint[1]=rbp[1]->resval.rpoint[2]=0.0;
			}
		} else if (!db_compareNames("IMAGE"/*DNT*/,typelink->resval.rstring)){
			for (trbp1=elist; trbp1!=NULL; trbp1=trbp1->rbnext) {
				switch (trbp1->restype) {
					case  10: rbp[0]=trbp1; break;	/* Ins pt */
					case  11: rbp[1]=trbp1; break;	/* Vector U*/
					case  12: rbp[2]=trbp1; break;	/* Vector V*/
				}
			}
			for (fi1=0; fi1<3 && rbp[fi1]!=NULL; fi1++);
			if (fi1<3) break;  /* Expected group not found. */

			/* Pt (WCS): */
			sds_trans(rbp[0]->resval.rpoint,&wcs,&ucs,0,ap[0]);
			sds_xformpt(ap[0],ap[0],wmat);
			sds_trans(ap[0],&ucs,&wcs,0,rbp[0]->resval.rpoint);

			/* Vector (WCS): */
			ap[0][0]=ap[0][1]=ap[0][2]=0.0;
			sds_trans(ap[0] 			   ,&wcs,&ucs,0,ap[2]);
			sds_trans(rbp[1]->resval.rpoint,&wcs,&ucs,0,ap[3]);
			sds_trans(rbp[2]->resval.rpoint,&wcs,&ucs,0,ap[4]);
			sds_xformpt(ap[2],ap[2],wmat);
			sds_xformpt(ap[3],ap[3],wmat);
			sds_xformpt(ap[4],ap[4],wmat);
			sds_trans(ap[2],&ucs,&wcs,0,ap[0]);
			sds_trans(ap[3],&ucs,&wcs,0,ap[1]);
			sds_trans(ap[4],&ucs,&wcs,0,ap[2]);
			rbp[1]->resval.rpoint[0]=ap[1][0]-ap[0][0];
			rbp[1]->resval.rpoint[1]=ap[1][1]-ap[0][1];
			rbp[1]->resval.rpoint[2]=ap[1][2]-ap[0][2];
			rbp[2]->resval.rpoint[0]=ap[2][0]-ap[0][0];
			rbp[2]->resval.rpoint[1]=ap[2][1]-ap[0][1];
			rbp[2]->resval.rpoint[2]=ap[2][2]-ap[0][2];

		} else if (!db_compareNames("XLINE"/*DNT*/,typelink->resval.rstring) ||
				   !db_compareNames("RAY"/*DNT*/ ,typelink->resval.rstring)) {

			for (trbp1=elist; trbp1!=NULL; trbp1=trbp1->rbnext) {
				switch (trbp1->restype) {
					case  10: rbp[0]=trbp1; break;	/* 1st pt */
					case  11: rbp[1]=trbp1; break;	/* 2nd pt */
				}
			}
			for (fi1=0; fi1<2 && rbp[fi1]!=NULL; fi1++);
			if (fi1<2) break;  /* Expected group not found. */

			/* Pt (WCS): */
			sds_trans(rbp[0]->resval.rpoint,&wcs,&ucs,0,ap[0]);
			sds_xformpt(ap[0],ap[0],wmat);
			sds_trans(ap[0],&ucs,&wcs,0,rbp[0]->resval.rpoint);

			/* Vector (WCS): */
			ap[0][0]=ap[0][1]=ap[0][2]=0.0;
			sds_trans(ap[0] 			   ,&wcs,&ucs,0,ap[2]);
			sds_trans(rbp[1]->resval.rpoint,&wcs,&ucs,0,ap[3]);
			sds_xformpt(ap[2],ap[2],wmat);
			sds_xformpt(ap[3],ap[3],wmat);
			sds_trans(ap[2],&ucs,&wcs,0,ap[0]);
			sds_trans(ap[3],&ucs,&wcs,0,ap[1]);
			rbp[1]->resval.rpoint[0]=ap[1][0]-ap[0][0];
			rbp[1]->resval.rpoint[1]=ap[1][1]-ap[0][1];
			rbp[1]->resval.rpoint[2]=ap[1][2]-ap[0][2];
			/* Make sure it's unit length: */
			ar[0]=sqrt(rbp[1]->resval.rpoint[0]*rbp[1]->resval.rpoint[0]+
					   rbp[1]->resval.rpoint[1]*rbp[1]->resval.rpoint[1]+
					   rbp[1]->resval.rpoint[2]*rbp[1]->resval.rpoint[2]);
			if (!icadRealEqual(ar[0],0.0)) {
				if (!icadRealEqual(ar[0],1.0)) {
					rbp[1]->resval.rpoint[0]/=ar[0];
					rbp[1]->resval.rpoint[1]/=ar[0];
					rbp[1]->resval.rpoint[2]/=ar[0];
				}
			} else {  /* Use (1,0,0) as default. */
				rbp[1]->resval.rpoint[0]=1.0;
				rbp[1]->resval.rpoint[1]=rbp[1]->resval.rpoint[2]=0.0;
			}

		}
		//<alm: implemented full transformation for TOLERANCE. 
		//		Added operating with EED, because real text height are stored in dxf1040 in EED>
		else if (!db_compareNames("TOLERANCE"/*DNT*/,typelink->resval.rstring))
		{
			tolDimtxt = 1;
			for (trbp1=elist; trbp1!=NULL; trbp1=trbp1->rbnext)
			{
				switch (trbp1->restype)
				{
				case  10: rbp[0]=trbp1; break;	/* 1st pt */
				case  11: rbp[1]=trbp1; break;	/* 2nd pt */
				}
			}
			for (fi1=0; fi1<2 && rbp[fi1]!=NULL; fi1++);
			if (fi1<2) break;  /* Expected group not found. */

			/* Pt (WCS): */
			sds_trans(rbp[0]->resval.rpoint,&wcs,&ucs,0,ap[0]);
			sds_xformpt(ap[0],ap[0],wmat);
			sds_trans(ap[0],&ucs,&wcs,0,rbp[0]->resval.rpoint);

			/* Vector (WCS): */
			ap[0][0]=ap[0][1]=ap[0][2]=0.0;
			sds_trans(ap[0] 			   ,&wcs,&ucs,0,ap[2]);
			sds_trans(rbp[1]->resval.rpoint,&wcs,&ucs,0,ap[3]);
			sds_xformpt(ap[2],ap[2],wmat);
			sds_xformpt(ap[3],ap[3],wmat);
			sds_trans(ap[2],&ucs,&wcs,0,ap[0]);
			sds_trans(ap[3],&ucs,&wcs,0,ap[1]);
			rbp[1]->resval.rpoint[0]=ap[1][0]-ap[0][0];
			rbp[1]->resval.rpoint[1]=ap[1][1]-ap[0][1];
			rbp[1]->resval.rpoint[2]=ap[1][2]-ap[0][2];
			/* Make sure it's unit length: */
			ar[0]=sqrt(rbp[1]->resval.rpoint[0]*rbp[1]->resval.rpoint[0]+
					   rbp[1]->resval.rpoint[1]*rbp[1]->resval.rpoint[1]+
					   rbp[1]->resval.rpoint[2]*rbp[1]->resval.rpoint[2]);
			if (!icadRealEqual(ar[0],0.0))
			{
				if (!icadRealEqual(ar[0],1.0))
				{
					rbp[1]->resval.rpoint[0]/=ar[0];
					rbp[1]->resval.rpoint[1]/=ar[0];
					rbp[1]->resval.rpoint[2]/=ar[0];
				}
			}
			else
			{  /* Use (1,0,0) as default. */
				rbp[1]->resval.rpoint[0]=1.0;
				rbp[1]->resval.rpoint[1]=rbp[1]->resval.rpoint[2]=0.0;
			}

			// Update EED info

			// Set the ACAD app name (and never relrb it):
			resbuf appacadrb;
			appacadrb.rbnext=NULL;
			appacadrb.restype=RTSTR;
			appacadrb.resval.rstring="ACAD"/*DNT*/;

			db_tolerance* pTolerance = 0;
			db_drawing* pDrawing = 0;

			if (ename)
			{
				pTolerance = reinterpret_cast<db_tolerance*>(ename[0]);
				pDrawing = reinterpret_cast<db_drawing*>(ename[1]);
			}
			else
			{
				if (!ic_assoc(elist, -1))
				{
					pTolerance = reinterpret_cast<db_tolerance*>(
						ic_rbassoc->resval.rlname[0]);
					pDrawing = reinterpret_cast<db_drawing*>(
						ic_rbassoc->resval.rlname[1]);
				}
			}

			if (pTolerance && pDrawing)
			{
				// Get EED
				resbuf* pEED = pTolerance->get_eed(&appacadrb,NULL);
				{
					// Go to DIMSTYLE
					resbuf* rb;
					for (rb = pEED; rb; rb = rb->rbnext)
					{
						if (rb->restype == 1000 &&
							!stricmp(rb->resval.rstring,"DSTYLE"/*DNT*/))
							break;
					}

					resbuf* rbOpenBrace = 0;

					bool bDimScaleExist, bTextHeightExist, bGapExist;
					bDimScaleExist = bTextHeightExist = bGapExist = false;

					// Parse DSTYLE section until the }
					for (; rb; rb = rb->rbnext)
			   		{
						if (rb->restype == 1002 &&
							*rb->resval.rstring == '}')
							break;

						if (rb->restype == 1002 &&
							*rb->resval.rstring == '{')
							rbOpenBrace = rb;

						if (rb->restype != 1070 || rb->rbnext==NULL) 
							continue;

						int itype = rb->resval.rint;
						resbuf* rbnext = rb->rbnext;
						switch (itype)
							{
						case 40:
							if (rbnext->restype != 1040) break;
							bDimScaleExist = true;
							break;
						case 140:
							if (rbnext->restype != 1040) break;
							bTextHeightExist = true;
							break;
						case 147:
							if (rbnext->restype != 1040) break;
							bGapExist = true;
							break;
						}
					}

					// Check existence of groups
					if (pEED &&
						(!bGapExist || !bTextHeightExist || !bDimScaleExist))
					{
						// Get default values from DIMSTYLE
						db_handitem* pDimStyle = 0;
						pTolerance->get_3(&pDimStyle);

						// If NULL, try for STANDARD:
						if (pDimStyle == NULL)
						{
							pDimStyle = pDrawing->findtabrec(DB_DIMSTYLETAB,
															 "STANDARD"/*DNT*/,1);
						}
						assert(pDimStyle && "There is not STANDARD dimension style");


						resbuf* rbTolScale = 0;
						resbuf* rbHead = 0;
						if (!bGapExist)
						{
							sds_real dGap;
							pDimStyle->get_147(&dGap);
							rbHead = sds_buildlist(1070, 147,
												   1040, dGap, RTNONE);
							rbTolScale = rbHead;
						}
						if (!bTextHeightExist)
						{
							sds_real dTextHeight;
							pDimStyle->get_140(&dTextHeight);
							rbHead = sds_buildlist(1070, 140,
												   1040, dTextHeight, RTNONE);
							rbHead->rbnext->rbnext = rbTolScale;
							rbTolScale = rbHead;
						}
						if (!bDimScaleExist)
						{
							sds_real dDimScale;
							pDimStyle->get_40(&dDimScale);
							rbHead = sds_buildlist(1070, 40,
												   1040, dDimScale, RTNONE);
							rbHead->rbnext->rbnext = rbTolScale;
							rbTolScale = rbHead;
						}

						// pEED is complete DSTYLE buffer.
						assert(rbOpenBrace);
						assert(rbTolScale);
						resbuf* rbTmp = rbOpenBrace->rbnext;
						rbOpenBrace->rbnext = rbTolScale;

						resbuf* rbprev = 0;
						for (rb = rbTolScale; rb; rb = rb->rbnext)
						{
							rbprev = rb;
						}
						assert(!rbprev->rbnext);
						rbprev->rbnext = rbTmp;
						// Substitute existing pEED ACAD.
                        rbTmp = 0;
						rbHead = 0;
                        rbprev = 0;
                        for( rb = elist ; rb ; rb = rb->rbnext )
                        {
                            if( rb->rbnext && rb->rbnext->restype == 1001 && 
                                !stricmp( rb->rbnext->resval.rstring, "ACAD"/*DNT*/))
                            {
                                rbHead = rb;
                            }
                            else if( rbHead && rb->rbnext && rb->rbnext->restype == 1001 )
                            {
                                rbTmp = rb;
                                break;
                            }
                        }
                        assert(rbHead);
                        rbprev = rbHead->rbnext;        // Old 1001 ACAD --
                        rbHead->rbnext = pEED->rbnext;  // New 1001 ACAD -- 1002 "}"
                        for( rb = rbHead ; rb->rbnext ; rb = rb->rbnext ); // find last
                        if( rbTmp )                     // has old next appid
                        {
                            rb->rbnext = rbTmp->rbnext;  // joint new last and old next---
                            rbTmp->rbnext = 0;          // break ACAD and old next---
                        }
                        sds_relrb(rbprev);              // Old ACAD is deleted.
                        pEED->rbnext = 0;               // New -3 is deleted. 
                        sds_relrb(pEED);
					}
					else if (!pEED)
					{
						// Get default values from DIMSTYLE
						db_handitem* pDimStyle = 0;
						pTolerance->get_3(&pDimStyle);

						// If NULL, try for STANDARD:
						if (pDimStyle == NULL)
						{
							pDimStyle = pDrawing->findtabrec(DB_DIMSTYLETAB,
															 "STANDARD"/*DNT*/,1);
						}
						assert(pDimStyle && "There is not STANDARD dimension style");

						double dGap, dDimScale, dTextHeight;
						pDimStyle->get_40(&dDimScale);
						pDimStyle->get_140(&dTextHeight);
						pDimStyle->get_147(&dGap);

						pEED = sds_buildlist(-3,
											 1001, "ACAD",
											 1000, "DSTYLE",
											 1002, "{",
											 1070, 140,
											 1040, dTextHeight,
											 1070, 147,
											 1040, dGap,
											 1002, "}",
											 RTNONE);

						resbuf* rbprev = 0;
						for (rb = elist; rb; rb = rb->rbnext)
						{
							rbprev = rb;
						}
						assert(!rbprev->rbnext);
						rbprev->rbnext = pEED;
					}
				}
			}

		}else if (!db_compareNames("LEADER"/*DNT*/,typelink->resval.rstring)) {

			for (trbp1=elist; trbp1!=NULL; trbp1=trbp1->rbnext) {
				switch (trbp1->restype) {
					case 10:
						{
						/* Pt (WCS): */
						sds_trans(trbp1->resval.rpoint,&wcs,&ucs,0,ap[0]);
						sds_xformpt(ap[0],ap[0],wmat);
						sds_trans(ap[0],&ucs,&wcs,0,trbp1->resval.rpoint);
						}
						break;
					case  211: rbp[0]=trbp1; break;  /* 1st pt */
					case  212: rbp[1]=trbp1; break;  /* 2nd pt */
					case  213: rbp[2]=trbp1; break;  /* 2nd pt */
				}
			}


			/* Vector (WCS): */
			ap[0][0]=ap[0][1]=ap[0][2]=0.0;
			sds_trans(ap[0],&wcs,&ucs,0,ap[2]);
			sds_xformpt(ap[2],ap[2],wmat);
			sds_trans(ap[2],&ucs,&wcs,0,ap[0]);
			for(fi1=0;fi1<3;fi1++){
				if(rbp[fi1]!=NULL){
					sds_trans(rbp[fi1]->resval.rpoint,&wcs,&ucs,0,ap[3]);
					sds_xformpt(ap[3],ap[3],wmat);
					sds_trans(ap[3],&ucs,&wcs,0,ap[1]);
					rbp[fi1]->resval.rpoint[0]=ap[1][0]-ap[0][0];
					rbp[fi1]->resval.rpoint[1]=ap[1][1]-ap[0][1];
					rbp[fi1]->resval.rpoint[2]=ap[1][2]-ap[0][2];
				}
			}

		} else if (!db_compareNames("SPLINE"/*DNT*/,typelink->resval.rstring)){
			//make one pass for singular entries
			for (trbp1=elist; trbp1!=NULL; trbp1=trbp1->rbnext) {
				switch (trbp1->restype) {
					case  10:
					case  11: /* Pt (WCS): */
							sds_trans(trbp1->resval.rpoint,&wcs,&ucs,0,ap[0]);
							sds_xformpt(ap[0],ap[0],wmat);
							sds_trans(ap[0],&ucs,&wcs,0,trbp1->resval.rpoint);
							break;
					case 12:
					case 13:
							/* Vector (WCS): */
							ap[0][0]=ap[0][1]=ap[0][2]=0.0;
							//if vector is 0,0,0 then don't do anything
							if(icadPointEqual(ap[0],trbp1->resval.rpoint))break;
							sds_trans(ap[0] 			   ,&wcs,&ucs,0,ap[2]);
							sds_trans(trbp1->resval.rpoint,&wcs,&ucs,0,ap[3]);
							sds_xformpt(ap[2],ap[2],wmat);
							sds_xformpt(ap[3],ap[3],wmat);
							sds_trans(ap[2],&ucs,&wcs,0,ap[0]);
							sds_trans(ap[3],&ucs,&wcs,0,ap[1]);
							trbp1->resval.rpoint[0]=ap[1][0]-ap[0][0];
							trbp1->resval.rpoint[1]=ap[1][1]-ap[0][1];
							trbp1->resval.rpoint[2]=ap[1][2]-ap[0][2];
							/* Make sure it's unit length: */
							ar[0]=sqrt(trbp1->resval.rpoint[0]*trbp1->resval.rpoint[0]+
									   trbp1->resval.rpoint[1]*trbp1->resval.rpoint[1]+
									   trbp1->resval.rpoint[2]*trbp1->resval.rpoint[2]);
							if (!icadRealEqual(ar[0],0.0)) {
								if (!icadRealEqual(ar[0],1.0)) {
									trbp1->resval.rpoint[0]/=ar[0];
									trbp1->resval.rpoint[1]/=ar[0];
									trbp1->resval.rpoint[2]/=ar[0];
								}
							} else {  /* Use (1,0,0) as default. */
								trbp1->resval.rpoint[0]=1.0;
								trbp1->resval.rpoint[1]=trbp1->resval.rpoint[2]=0.0;
							}
							break;
				}
			}

		} else if (!db_compareNames("HATCH"/*DNT*/,typelink->resval.rstring)){
			bool first = true;
			resbuf *insertpt, *patternbase = NULL, *patternoffset = NULL;
			resbuf *patternlineangle = NULL;

			for (trbp1=elist; trbp1!=NULL; trbp1=trbp1->rbnext) {
				switch (trbp1->restype) {
				   case  10: // Pt (OCS):
						if (first) {
							insertpt = trbp1;
							first = false;
						}

						ic_ptcpy(ecs.resval.rpoint,oldextru);
						sds_trans(trbp1->resval.rpoint,&ecs,&ucs,0,ap[0]);
						sds_xformpt(ap[0],ap[0],wmat);
						ic_ptcpy(ecs.resval.rpoint,newextru);
						sds_trans(ap[0],&ucs,&ecs,0,trbp1->resval.rpoint);
						break;

					case 91: //Num Boundary paths
						{
							long numPaths = trbp1->resval.rlong;
							for (long i = 0; trbp1 && (i < numPaths); i++)
							{
								// first item must be a flag so we can tell if it's a polyline
								if ((!trbp1->rbnext) || (trbp1->rbnext->restype != 92)) {
									ASSERT(false);
									break;
								}
								trbp1 = trbp1->rbnext;	/*D.G.*/// Placed this line after the previous checking.

								int hatchflag = trbp1->resval.rint;

								if (hatchflag & IC_HATCH_PATHFLAG_POLYLINE) {		//polyline edge

									// 3 items in polyline that are not multiple entries
									// multi item entries are set under 93
									for (int j = 0; j < 3; j++)
									{
										trbp1 = trbp1->rbnext;

										if (trbp1 == NULL)
											break;

										if (trbp1->restype == 93)
										{
											//check each vertex and bulge
											long numvertices = trbp1->resval.rlong;
											for (long k = 0; k < numvertices && trbp1; k++)
											{
												trbp1 = trbp1->rbnext;
												// next value must be vertex loc - 10
												if ((trbp1 == NULL) || (trbp1->restype != 10))
													break;

												ic_ptcpy(ecs.resval.rpoint,oldextru);
												sds_trans(trbp1->resval.rpoint,&ecs,&ucs,0,ap[0]);
												sds_xformpt(ap[0],ap[0],wmat);
												ic_ptcpy(ecs.resval.rpoint,newextru);
												sds_trans(ap[0],&ucs,&ecs,0,trbp1->resval.rpoint);

												// if bulge is specified it should be next value otherwise set to 0;
												if (trbp1->rbnext != NULL && trbp1->rbnext->restype == 42)
													//bulge doesn't appear to be needed so just skip it
													trbp1 = trbp1->rbnext;
											}
										}

									}// for polyline items

								} // if polyline

								else {				//non polyline edge
									// next item must be number of edges in path
									if ((!trbp1->rbnext) || (trbp1->rbnext->restype != 93)) {
										ASSERT(false);
										break;
									}
									trbp1 = trbp1->rbnext;	/*D.G.*/// Placed this line after the previous checking.

									long numedgetypes = trbp1->resval.rlong;
									for (long j = 0; trbp1 && (j < numedgetypes); j++)
									{
										// next item must be edgetype
										if ((!trbp1->rbnext) || (trbp1->rbnext->restype != 72)) {
											ASSERT(false);
											break;
										}
										trbp1 = trbp1->rbnext;	/*D.G.*/// Placed this line after the previous checking.

										switch (trbp1->resval.rint)
										{
										case IC_HATCH_PATHTYPE_LINE: //line
											{
												// 2 values in line
												for (int k = 0; trbp1 && (k < 2); k++) {
													trbp1 = trbp1->rbnext;
													if (trbp1 == NULL)
														break;

													switch (trbp1->restype) {
													case 10:
														ic_ptcpy(ecs.resval.rpoint,oldextru);
														sds_trans(trbp1->resval.rpoint,&ecs,&ucs,0,ap[0]);
														sds_xformpt(ap[0],ap[0],wmat);
														ic_ptcpy(ecs.resval.rpoint,newextru);
														sds_trans(ap[0],&ucs,&ecs,0,trbp1->resval.rpoint);
														break;

													case 11:
														ic_ptcpy(ecs.resval.rpoint,oldextru);
														sds_trans(trbp1->resval.rpoint,&ecs,&ucs,0,ap[0]);
														sds_xformpt(ap[0],ap[0],wmat);
														ic_ptcpy(ecs.resval.rpoint,newextru);
														sds_trans(ap[0],&ucs,&ecs,0,trbp1->resval.rpoint);
														break;

													default:
														ASSERT(false);
													}
												}
											}
											break;

										case IC_HATCH_PATHTYPE_CIRCARC: //circular arc
											{
												// 5 values in circular arc
												for (int k = 0; trbp1 && (k < 5); k++)
												{
													trbp1 = trbp1->rbnext;
													if (trbp1 == NULL)
														break;

													switch (trbp1->restype) {
													case 10:
														rbp[0]=trbp1;  // Center
														break;

													case 40:
														rbp[1]=trbp1;  // Radius
														break;

													case 50:
														rbp[2]=trbp1;  // Start angle
														break;

													case 51:
														rbp[3]=trbp1;  // End angle
														break;

													case 73:
														break; // do nothing

													default:
														ASSERT(false);
													}
												}

												SDS_xformarc(oldextru, newextru, wmat, &ecs, &ucs, rbp[0], rbp[1], rbp[2],rbp[3]);
											}
											break;

										case IC_HATCH_PATHTYPE_ELLIPARC: // elliptical arc
											{
												// 6 values in ellipitical arc
												for (int k = 0; trbp1 && (k < 6); k++)
												{
													trbp1 = trbp1->rbnext;
													if (trbp1 == NULL)
														break;

													switch (trbp1->restype) {
													case 10:			//center
														rbp[0]=trbp1;
														break;

													case 11:			//vector
														rbp[1]=trbp1;
														break;

													case 40:			//ratio
														rbp[2]=trbp1;
														break;

													case 50:
														rbp[3]=trbp1;	//start angle
														break;

													case 51:
														rbp[4]=trbp1;	//end angle
														break;

													case 73:			//ccw flag - do nothing
														break;

													default:
														ASSERT(false);
													}
												}

												// new center - ap[7]
												ic_ptcpy(ecs.resval.rpoint,oldextru);
												sds_trans(rbp[0]->resval.rpoint,&wcs,&ucs,0,ap[0]);
												sds_xformpt(ap[0],ap[0],wmat);
												ic_ptcpy(ecs.resval.rpoint,newextru);
												sds_trans(ap[0],&ucs,&wcs,0,ap[7]);

												//new axis vector				;
												ap[1][0]=rbp[0]->resval.rpoint[0]+rbp[1]->resval.rpoint[0];
												ap[1][1]=rbp[0]->resval.rpoint[1]+rbp[1]->resval.rpoint[1];
												ap[1][2]=rbp[0]->resval.rpoint[2]+rbp[1]->resval.rpoint[2];
												ic_ptcpy(ecs.resval.rpoint,oldextru);
												sds_trans(ap[1],&wcs,&ucs,0,ap[2]);
												sds_xformpt(ap[2],ap[2],wmat);
												ic_ptcpy(ecs.resval.rpoint,newextru);
												sds_trans(ap[2],&ucs,&wcs,0,ap[8]);
												rbp[1]->resval.rpoint[0]=ap[8][0]-ap[7][0];
												rbp[1]->resval.rpoint[1]=ap[8][1]-ap[7][1];
												rbp[1]->resval.rpoint[2]=ap[8][2]-ap[7][2];
												ic_ptcpy(rbp[0]->resval.rpoint,ap[7]);
											}
											break;

										case IC_HATCH_PATHTYPE_SPLINE: // spline
											{
												long numctrlpts = 0;
												long numknots = 0;
												int rational = 0;

												// 5 fixed values in spline arc
												for (int k = 0; trbp1 && (k < 5); k++)
												{
													trbp1 = trbp1->rbnext;
													if (trbp1 == NULL)
														break;

													switch (trbp1->restype) {
														case 73:
															rational = trbp1->resval.rint;
															break;

														case 95:
															numknots = trbp1->resval.rlong;
															break;

														case 96:
															numctrlpts = trbp1->resval.rlong;
															break;

														// don't care about other values.
													}
												}

												//bypass knot values
												for (k = 0; trbp1 && (k < numknots); k++)
												{
													if ((!trbp1->rbnext) || (trbp1->rbnext->restype != 40)) {
														ASSERT(false);
														break;
													}
													trbp1 = trbp1->rbnext;	/*D.G.*/// Placed this line after the previous checking.
												}

												//set control points
												for (k = 0; trbp1 && (k < numctrlpts); k++) {
													if ((!trbp1->rbnext) || (trbp1->rbnext->restype != 10)) {
														ASSERT(false);
														break;
													}
													trbp1 = trbp1->rbnext;	/*D.G.*/// Placed this line after the previous checking.

													ic_ptcpy(ecs.resval.rpoint,oldextru);
													sds_trans(trbp1->resval.rpoint,&wcs,&ucs,0,ap[0]);
													sds_xformpt(ap[0],ap[0],wmat);
													ic_ptcpy(ecs.resval.rpoint,newextru);
													sds_trans(ap[0],&ucs,&wcs,0,trbp1->resval.rpoint);
												}

												//bypass weight values only if rational
												if (rational)
												{
													for (k = 0; trbp1 && k < numctrlpts; k++) {
														if ((!trbp1->rbnext) || (trbp1->rbnext->restype != 42)) {
															ASSERT(false);
															break;
														}
														trbp1 = trbp1->rbnext;	/*D.G.*/// Placed this line after the previous checking.
													}
												}
											}
											break;

										default: // undefined
											ASSERT(false);
										}

									}// for each edge
								}// else not polyline

								// bypass boundary objects
								if ((!trbp1->rbnext) || (trbp1->rbnext->restype != 97)) {
									ASSERT(false);
									break;
								}
								trbp1 = trbp1->rbnext;	/*D.G.*/// Placed this line after the previous checking.

								int numobjs = trbp1->resval.rlong;

								for (int j = 0; j < numobjs; j++)
								{
									if ((!trbp1->rbnext) || (trbp1->rbnext->restype != 330)) {
										ASSERT(false);
										break;
									}
									trbp1 = trbp1->rbnext;	/*D.G.*/// Placed this line after the previous checking.
								}
							} //for boundary path
						} //case 91
						break;

					case 52: {
							double patternAng = trbp1->resval.rreal;

							// Rotation (do it 1st before we change the other params)
							//
							//	Determine new rotation by xforming the ins pt
							//	and a pt 1 unit away in the rotation direction:
							//
							ap[0][0] = insertpt->resval.rpoint[0] + cos(patternAng);
							ap[0][1] = insertpt->resval.rpoint[1] + sin(patternAng);
							ap[0][2] = insertpt->resval.rpoint[2];
							ic_ptcpy(ecs.resval.rpoint,oldextru);
							sds_trans(insertpt->resval.rpoint,&ecs,&ucs,0,ap[1]);
							sds_trans(ap[0],&ecs,&ucs,0,ap[2]);
							sds_xformpt(ap[1],ap[1],wmat);
							sds_xformpt(ap[2],ap[2],wmat);
							ic_ptcpy(ecs.resval.rpoint,newextru);
							sds_trans(ap[1],&ucs,&ecs,0,ap[7]);
							sds_trans(ap[2],&ucs,&ecs,0,ap[0]);
							trbp1->resval.rreal=sds_angle(ap[7],ap[0]);
						}
						break;

					case 41:
					case 49:
						trbp1->resval.rreal *= xscale;
						break;

					case 53:
						patternlineangle = trbp1;
						break;

					case 43:
						patternbase = trbp1;
						break;

					case 44:	//calcs rotation and base
						{
							if (!patternbase)
								break;

							sds_point base, unitaway;

							base[0] = patternbase->resval.rreal;
							base[1] = trbp1->resval.rreal;
							base[2] = 0;

							// Rotation - Determine new rotation by xforming the ins pt
							// and a pt 1 unit away in the rotation direction:
							unitaway[0] = base[0] + cos(patternlineangle->resval.rreal);
							unitaway[1] = base[1] + sin(patternlineangle->resval.rreal);
							unitaway[2] = 0;

							ic_ptcpy(ecs.resval.rpoint,oldextru);
							sds_trans(base,&ecs,&ucs,0,ap[1]);
							sds_trans(unitaway,&ecs,&ucs,0,ap[2]);
							sds_xformpt(ap[1],ap[1],wmat);
							sds_xformpt(ap[2],ap[2],wmat);
							ic_ptcpy(ecs.resval.rpoint,newextru);
							sds_trans(ap[1],&ucs,&ecs,0,base);
							sds_trans(ap[2],&ucs,&ecs,0,unitaway);

							patternbase->resval.rreal = base[0];
							trbp1->resval.rreal = base[1];

							patternlineangle->resval.rreal=sds_angle(base,unitaway);
						}
						break;

					case 45:
						patternoffset = trbp1;
						break;

					case 46:
						if (!patternoffset)
							break;

						//since this is a vector - apply the transform to it, then apply transform to 0,0,0
						//then subtract the 0,0,0 result from the vector transform to get the correct offset.
						ap[1][0] = patternoffset->resval.rreal;
						ap[1][1] = trbp1->resval.rreal;
						ap[1][2] = oldextru[2];
						ic_ptcpy(ecs.resval.rpoint,oldextru);
						sds_trans(ap[1],&ecs,&ucs,0,ap[0]);
						sds_xformpt(ap[0],ap[0],wmat);
						ic_ptcpy(ecs.resval.rpoint,newextru);
						sds_trans(ap[0],&ucs,&ecs,0,ap[2]);

						// transform 0,0,0
						ap[3][0] = 0;
						ap[3][1] = 0;
						ap[3][2] = 0;
						ic_ptcpy(ecs.resval.rpoint,oldextru);
						sds_trans(ap[3],&ecs,&ucs,0,ap[4]);
						sds_xformpt(ap[4],ap[4],wmat);
						ic_ptcpy(ecs.resval.rpoint,newextru);
						sds_trans(ap[4],&ucs,&ecs,0,ap[3]);

						patternoffset->resval.rreal = ap[2][0] - ap[3][0];
						trbp1->resval.rreal = ap[2][1] - ap[3][1];
						break;

				}//switch
			} //for each group code
		} else if (!db_compareNames("MLINE"/*DNT*/,typelink->resval.rstring)){ // MR 6/25/98 -- exactly the same as spline
			//make one pass for singular entries
			for (trbp1=elist; trbp1!=NULL; trbp1=trbp1->rbnext) {
				switch (trbp1->restype) {
					case  10:
					case  11: /* Pt (WCS): */
							sds_trans(trbp1->resval.rpoint,&wcs,&ucs,0,ap[0]);
							sds_xformpt(ap[0],ap[0],wmat);
							sds_trans(ap[0],&ucs,&wcs,0,trbp1->resval.rpoint);
							break;
					case 12:
					case 13:
							/* Vector (WCS): */
							ap[0][0]=ap[0][1]=ap[0][2]=0.0;
							//if vector is 0,0,0 then don't do anything
							if(icadPointEqual(ap[0],trbp1->resval.rpoint))break;
							sds_trans(ap[0] 			   ,&wcs,&ucs,0,ap[2]);
							sds_trans(trbp1->resval.rpoint,&wcs,&ucs,0,ap[3]);
							sds_xformpt(ap[2],ap[2],wmat);
							sds_xformpt(ap[3],ap[3],wmat);
							sds_trans(ap[2],&ucs,&wcs,0,ap[0]);
							sds_trans(ap[3],&ucs,&wcs,0,ap[1]);
							trbp1->resval.rpoint[0]=ap[1][0]-ap[0][0];
							trbp1->resval.rpoint[1]=ap[1][1]-ap[0][1];
							trbp1->resval.rpoint[2]=ap[1][2]-ap[0][2];
							/* Make sure it's unit length: */
							ar[0]=sqrt(trbp1->resval.rpoint[0]*trbp1->resval.rpoint[0]+
									   trbp1->resval.rpoint[1]*trbp1->resval.rpoint[1]+
									   trbp1->resval.rpoint[2]*trbp1->resval.rpoint[2]);
							if (!icadRealEqual(ar[0],0.0)) {
								if (!icadRealEqual(ar[0],1.0)) {
									trbp1->resval.rpoint[0]/=ar[0];
									trbp1->resval.rpoint[1]/=ar[0];
									trbp1->resval.rpoint[2]/=ar[0];
								}
							} else {  /* Use (1,0,0) as default. */
								trbp1->resval.rpoint[0]=1.0;
								trbp1->resval.rpoint[1]=trbp1->resval.rpoint[2]=0.0;
							}
							break;
				}
			}


		}
		else
			//<ALM>
			if (sds_wcmatch(typelink->resval.rstring, "BODY,3DSOLID,REGION") == RTNORM)
			{
				icl::transf tr = wmat;
				wcs2ucs2wcs(tr);

				resbuf*	pSatRb = ic_ret_assoc(elist, 1);
				if (pSatRb)
				{
					CDbAcisEntityData oldObj(pSatRb->resval.rstring, typelink->resval.rstring, CDbAcisEntityData::eDbType);
					CDbAcisEntityData newObj;
					if (CModeler::get()->transform(oldObj, newObj, tr))
					{
						char*& sat = pSatRb->resval.rstring;
						IC_FREE(sat);
						sat = new char [newObj.getSat().size()+1];
						strcpy(sat, newObj.getSat().c_str());
					}
				}
			}
			//</ALM>
			else
				ok2mod=0;

		/* Fix the EED points that xform: */
		for (trbp1=elist; trbp1!=NULL && trbp1->restype!=-3; trbp1=
			trbp1->rbnext);
		for (; trbp1!=NULL; trbp1=trbp1->rbnext) {
			switch (trbp1->restype) {
				case 1011:	/* A WCS point */

					sds_trans(trbp1->resval.rpoint,&wcs,&ucs,0,ap[0]);
					sds_xformpt(ap[0],ap[0],wmat);
					sds_trans(ap[0],&ucs,&wcs,0,trbp1->resval.rpoint);
					break;

				case 1012: case 1013:  /* WCS vectors */

					/* Xform (0,0,0) and the old vector to get the new: */
					ap[0][0]=ap[0][1]=ap[0][2]=0.0;
					sds_trans(ap[0],&wcs,&ucs,0,ap[2]);
					sds_trans(trbp1->resval.rpoint,&wcs,&ucs,0,ap[3]);
					sds_xformpt(ap[2],ap[2],wmat);
					sds_xformpt(ap[3],ap[3],wmat);
					sds_trans(ap[2],&ucs,&wcs,0,ap[0]);
					sds_trans(ap[3],&ucs,&wcs,0,ap[1]);
					trbp1->resval.rpoint[0]=ap[1][0]-ap[0][0];
					trbp1->resval.rpoint[1]=ap[1][1]-ap[0][1];
					trbp1->resval.rpoint[2]=ap[1][2]-ap[0][2];

					/* The 1013 isn't supposed to scale; but the above */
					/* process DID scale it, so remove it: */
					if (trbp1->restype==1013) {
						trbp1->resval.rpoint[0]/=xscale;
						trbp1->resval.rpoint[1]/=xscale;
						trbp1->resval.rpoint[2]/=xscale;
					}

					break;
				case 1041: case 1042:
					trbp1->resval.rreal*=xscale;
					break;
				// EBATECH(CNBR) -[ TOLERANCE's DIMTXT and DIMGAP is scaled value.
				case 1001:
					if( tolDimtxt & 1 )	// This is TOLERANCE?
					{
						if( db_compareNames("ACAD"/*DNT*/, trbp1->resval.rstring ) == 0 )
							tolDimtxt |= 2;	// DIMSTYLE Overrides.
						else
							tolDimtxt &= ~2 ;
					}
					break;
				case 1070:
					if(( tolDimtxt & 3 ) == 3 )
					{
						if( trbp1->resval.rint == 140 ||  trbp1->resval.rint == 147 )
							tolDimtxt |= 4;	 // DIMTXT or DIMGAP
						else
							tolDimtxt &= ~4;
					}
					break;
				case 1040:
					if( tolDimtxt == 7 )
					{
						trbp1->resval.rreal*=xscale;
						tolDimtxt &= ~4;
					}
					break;
				// EBATECH(CNBR) ]-
			}
		}


		if(argelist) break;
		else {
			if (ok2mod) sds_entmod(elist);	/* Keep on rolling; shouldn't fail. */
			if (complexent && sds_entnext_noxref(ename,ename)!=RTNORM) break;
		}

	} while (complexent);


out:
	if(!argelist)
		if (elist!=NULL) { sds_relrb(elist); elist=NULL; }
	return rc;
}

void sds_xformpt(sds_point from, sds_point to, sds_matrix mat) {
/*
**	Applies an ADS-type matrix to a point.
**	'from and 'to' can be the same point in memory.
*/
	sds_point ap1;

	ap1[0]=mat[0][0]*from[0]+mat[0][1]*from[1]+mat[0][2]*from[2]+mat[0][3];
	ap1[1]=mat[1][0]*from[0]+mat[1][1]*from[1]+mat[1][2]*from[2]+mat[1][3];
	ap1[2]=mat[2][0]*from[0]+mat[2][1]*from[1]+mat[2][2]*from[2]+mat[2][3];
	to[0]=ap1[0]; to[1]=ap1[1]; to[2]=ap1[2];
}


