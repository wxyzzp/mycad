
// THIS FILE HAS BEEN GLOBALIZED!
#include "db.h"/*DNT*/
#include "entity.h"/*DNT*/
#include "entities.h"/*DNT*/
#include "icadlib.h"/*DNT*/
#include "EntConversion.h"/*DNT*/

#include "ptarray.h"
#include "curve.h"
#include "elliparc.h"
#include "nurbs.h"



short ic_divmeas(sds_name ename, sds_real dmval, char *bname,
    sds_real bscl, sds_real brot, short maxnpts, short mode,
    struct resbuf **ptlistp) {
/*
**  NOTE: F'n now goes both FORWARD and BACKWARD along entity!
**
**  This function performs operations similar to the ACAD DIVIDE and
**  MEASURE commands.  It calculates points along a LINE, ARC, CIRCLE,
**  ELLIPSE, SPLINE or non-mesh POLYLINE, dividing the entity into a 
**  given number of pieces or marking it at intervals of a given length.  
**  (SNAPANG determines the starting point of a circle.) Control vertices 
**  are ignored for plines which have been splined.
**
**  The following operations can be performed with the calculated points:
**    They can be put into a linked list of resbufs (with tangent
**      directions, if requested).
**    ACAD POINT entities can be created at the calculated points.
**    A block can be inserted at the calculated points, in which case
**      a scale must be specified and the rotation can be either
**      specified or taken from the tangent direction of the curve.
**      (Attributes are not added -- just like with ACAD's DIVIDE
**      and MEASURE.)
**
**  Pass parameters:
**
**      ename : The entity name of the LINE, ARC, CIRCLE, ELLIPSE, SPLINE,
**              or non-mesh POLYLINE to be used.
**      dmval : The distance for a Measure operation or the number of
**                segments for a Divide operation.
**      bname : The name of a block to insert at the calculated points.
**                (NULL or "" if no block-insertion is to be performed.)
**       bscl : Block-insertion scale.  Used for all 3 scale factors.
**                (Can be negative (mirror); has the effect of rotating
**                the INSERT 180 degrees.)
**       brot : Block-insertion rotation (used if not using the tangent
**                direction).  (Radians.)
**    maxnpts : Max # of points to determine.  If the number of
**                calculated points is greater than this, this function
**                returns an error code (-8) and does nothing.
**                Pass 0 to get the default (32000).  (This is just a
**                safety feature; it may be off by 1, depending on the mode.)
**       mode : Bit-mapped options (see below).
**    ptlistp : The ADDRESS of a struct resbuf pointer.  When non-NULL,
**                a llist of resbufs is allocated and *ptlistp
**                is pointed to it.  Depending on bit 5 in "mode",
**                this is either a llist of just the calculate points
**                (RT3DPOINT), or a llist of alternating point and
**                tangent direction links (RT3DPOINT/RTANG).  (Tangent
**                directions are in radians.) THE CALLER MUST EVENTUALLY
**                FREE IT (WITH sds_relrb()) IF *ptlistp IS SET TO
**                NON-NULL.
**
**  The value of "mode" is bit-coded as follows (76543210):
**    0 : 0=divide; 1=measure
**    1 : Use fractional mode?  (Measure only; dmval specifies a
**          dist as a fraction of the entity's total length.)
**    2 : Rotate the INSERTs to follow tangent direction?
**    3 : Put POINT entities at the calculated points?
**    4 : Take only the first calculated point?  (NOTE: You'll get
**          TWO points if you're dividing a closed entity; the
**          beginning point of a closed entity is always included
**          in a "divide" operation, but you probably REALLY want
**          the first dividing point.)
**    5 : Include tangent directions in *ptlistp?  This results in a
**          llist of resbufs that alternate: pt,tandir,pt,tandir,...
**          where tandir is the tangent direction at each point.
**          (See description of ptlistp above.)
**	  6 : Reverse the direction of the divide/measure operation (i.e.
**			measure from end of polyline or go cw on arc)
**
**  The POINTs and INSERTs are created using the current layer, color,
**  and linetype.  HOWEVER -- with UDS -- the color and linetype
**  come out BYLAYER because UDS is setting them in ic_cmdst().
**
**  Returns:
**       0 : OK
**      -1 : sds_entget() failed (bad ename?)
**      -2 : ic_assoc() failed
**      -3 : Insufficient memory (calloc() or sds_buildlist())
**      -4 : Not a LINE, ARC, CIRCLE, or non-mesh POLYLINE
**      -5 : sds_entmake() failed (block to insert not defined?)
**      -6 : Not enough VERTEXs found (1-vertex pline?  Seen them.)
**      -7 : Bad pass parmeters (dmval or bscl; see the tests)
**      -8 : Too many calculated points necessary (>maxnpts)
**             or the entity has 0 length
**      -9 : User did a break
*/
    char clayer[IC_ACADNMLEN],celtype[IC_ACADNMLEN];
    int closed,pass,ndivpts,nmade,dobeg,cecolor,fi1;
	short rc;
    int meas,rottan,putpt,putblk,fractmode,onept,ptnrot,revmode;
    sds_real crdlen,rad,seglen,sofar,cs,sn,ar1,ar2,ar3;
    sds_real snapang,target,dx,dy,pi,hpi,tandir;
    sds_point cent,crd,thispt,ap1;
    sds_name fename;
    struct resbuf *elist,*ptl,*prevptl,wcsrb,ucsrb,*rbp1,rb;
	db_entity* pEntity = NULL;
	CCurve* pCurve = NULL;

    struct dmseglink {
        sds_point pt;
        sds_real bulge;
        struct dmseglink *next;
    } *seg,*pseg,*tseg1,*tseg2;


                        /***** INIT STUFF *****/

    rc=0; elist=ptl=prevptl=NULL; seg=pseg=NULL;
    closed=0; dx=dy=0.0;
    cent[0]=cent[1]=cent[2]=0.0;
    hpi=0.5*(pi=IC_PI);

    clayer[0]='0'/*DNT*/; clayer[1]=0;
    cecolor=256; snapang=0.0;
    strncpy(celtype,"CONTINUOUS"/*DNT*/,sizeof(celtype)-1);

    ic_encpy(fename,ename);  /* Don't use passed ename; sds_entnext_noxref() will */
                             /* change it for the caller. */

    if (maxnpts<1) maxnpts=32000;  /*   (ACAD's max is 32K for DIVIDE.) */

    /* Set up the sds_trans() from/to resbufs: */
    wcsrb.restype=RTSHORT; wcsrb.resval.rint=0;
    ucsrb.restype=RT3DPOINT;
    ucsrb.resval.rpoint[0]=ucsrb.resval.rpoint[1]=0.0;  /* Safe init */
    ucsrb.resval.rpoint[2]=1.0;

    ic_normang(&brot,NULL);

    /* Get system var settings: */
    if (sds_getvar("CLAYER"/*DNT*/,&rb)==RTNORM) {
        strncpy(clayer,rb.resval.rstring,sizeof(clayer)-1);
        IC_FREE(rb.resval.rstring);
    }
    if (sds_getvar("CECOLOR"/*DNT*/,&rb)==RTNORM) {
        if      (strisame(rb.resval.rstring,ic_bylayer)) cecolor=256;
        else if (strisame(rb.resval.rstring,ic_byblock)) cecolor=0;
        else cecolor=atoi(rb.resval.rstring);
        IC_FREE(rb.resval.rstring);
    }

    if (sds_getvar("CELTYPE"/*DNT*/,&rb)==RTNORM) {
        strncpy(celtype,rb.resval.rstring,sizeof(celtype)-1);
        IC_FREE(rb.resval.rstring);
    }
    if (sds_getvar("SNAPANG"/*DNT*/,&rb)==RTNORM)
		snapang=rb.resval.rreal;

    /* Figure out mode: */
    meas=      (mode&0x01);
    fractmode=((mode&0x02)!=0);
    rottan=   ((mode&0x04)!=0);
    putpt=    ((mode&0x08)!=0);
    onept=    ((mode&0x10)!=0);
    ptnrot=   ((mode&0x20)!=0);
	revmode=  ((mode&0x40)!=0);
    if (bname==NULL) putblk=0;
    else {
        fi1=0; while (bname[fi1]==' '/*DNT*/) fi1++;
        putblk=(bname[fi1]!=0);
    }

    /* Make sure there's something to do: */
    if (!putblk && !putpt && ptlistp==NULL) { rc=0; goto out; }

    if (meas) {
        if (dmval<=0.0) { rc=-7; goto out; }
    } else {
        dmval+=0.001;
        if (dmval<2.0 || dmval>(sds_real)maxnpts) { rc=-7; goto out; }
        dmval=(sds_real)((short)dmval);
    }

    if (fabs(bscl)<IC_ZRO) { rc=-7; goto out; }

	pEntity = (db_entity*)fename[0];
	if (pEntity->ret_type() == DB_ELLIPSE || pEntity->ret_type() == DB_SPLINE) {
		pCurve = (pEntity->ret_type() == DB_ELLIPSE) ?
			     (CCurve*)asGeoEllipArc((db_ellipse*)pEntity) :
				 (CCurve*)asGeoSpline((db_spline*)pEntity);

		CPointArray ptArray;
		CRealArray parArray;
		if (!meas)
			rc = pCurve->GetPointsWithEqualArcLength((int)dmval, ptArray, parArray);
		else {
			rc = SUCCESS;
			C3Point cPoint;
			double rParam, rLength = dmval;
			do {
				rc = pCurve->FindPointByLength(rLength, rParam, cPoint);
				if (rc == SUCCESS) {
					ptArray.Add(cPoint);
					parArray.Add(rParam);
				}

				rLength += dmval;
			} while (rc == SUCCESS);
		}
		if (ptArray.Size()) {
			if (revmode) 
				ptArray.ReverseOrder();

			C3Point cPoint, cTangent;
			for (register i = 0; i < ptArray.Size(); i++) {
				ap1[0] = ptArray[i].X(); 
				ap1[1] = ptArray[i].Y();
				ap1[2] = ptArray[i].Z();

				if (!pCurve->Evaluate(parArray[i], cPoint, cTangent)) {
					if (!icadIsZero(cTangent.X()))
						tandir = atan(cTangent.Y() / cTangent.X());
					else {
						if (!icadIsZero(cTangent.Y()))
							tandir = 0.0;
						else if (cTangent.Y() > 0.0)
							tandir = PIOVER2;
						else
							tandir = -PIOVER2;
					}
				}

				if (putpt) {
					IC_FREEIT
					elist=sds_buildlist(RTDXF0,"POINT"/*DNT*/,
										8,clayer,
										6,celtype,
										62,cecolor,
										10,ap1,
										0);
					if (elist==NULL) { 
						rc=-3; 
						goto out; 
					}
					if (sds_entmake(elist)!=RTNORM) { 
						rc=-5; 
						goto out; 
					}
				}
				IC_FREEIT
				if (putblk) {
					elist=sds_buildlist(RTDXF0,"INSERT"/*DNT*/,
										8,clayer,
										6,celtype,
										62,cecolor,
										2,bname,
										10,ap1,
										41,bscl,
										42,bscl,
										43,bscl,
										50,(rottan) ? tandir : brot,
										210,ucsrb.resval.rpoint,
										0);
					if (elist==NULL) { 
						rc=-3; 
						goto out; 
					}
					if (sds_entmake(elist)!=RTNORM) { 
						rc=-5; 
						goto out; 
					}
				}
				IC_FREEIT
				if (ptlistp!=NULL) {
					// Add the point link: 
					if (( rbp1=sds_newrb(RT3DPOINT) )==NULL) { 
						rc=-3; 
						goto out; 
					}
					rbp1->restype=RT3DPOINT;
					ic_ptcpy(rbp1->resval.rpoint,ap1);
					if (ptl==NULL) 
						ptl=rbp1;
					else 
						prevptl->rbnext=rbp1;
					
					prevptl=rbp1;

					if (ptnrot) {
						// Add the tangent direction link: 
						if ((rbp1=sds_newrb(RTANG))==NULL) { 
							rc=-3; 
							goto out; 
						}
						rbp1->restype=RTANG;
						rbp1->resval.rreal=tandir;
						if (ptl==NULL) 
							ptl=rbp1; 
						else 
							prevptl->rbnext=rbp1;
								
						prevptl=rbp1;
					}
				}
			}
		}
		if (pCurve) {
			delete pCurve;
			pCurve = NULL;
		}
	}
	else {
					   /***** BUILD THE seg LLIST:*****/

		IC_FREEIT
		if ((elist=sds_entget(fename))==NULL) { rc=-1; goto out; }

		if (ic_assoc(elist,0)) { rc=-2; goto out; }
		if (strisame(ic_rbassoc->resval.rstring,"LINE"/*DNT*/)) {        
			if ((seg = new struct dmseglink )== NULL ) { rc=-3; goto out; }
			memset(seg,0, sizeof(struct dmseglink));		
			if ((seg->next= new struct dmseglink )==NULL) { rc=-3; goto out; }
			memset(seg->next,0, sizeof(struct dmseglink));
			if (ic_assoc(elist,10)) { rc=-2; goto out; }
			ic_ptcpy(seg->pt,ic_rbassoc->resval.rpoint);
			ic_rbassoc=NULL;  /* Avoid Metaware bug */
			if (ic_assoc(elist,11)) { rc=-2; goto out; }
			ic_ptcpy(seg->next->pt,ic_rbassoc->resval.rpoint);
			ucsrb.resval.rpoint[0]=ucsrb.resval.rpoint[1]=0.0;
			ucsrb.resval.rpoint[2]=1.0;
		} else if (strisame(ic_rbassoc->resval.rstring,"ARC"/*DNT*/)) {
			if ((seg= new struct dmseglink )==NULL )	{ rc=-3; goto out; }
			memset(seg, 0 ,sizeof(struct dmseglink));
			if ((seg->next= new struct dmseglink )==NULL) { rc=-3; goto out; }
			memset(seg->next,0 ,sizeof(struct dmseglink));
			if (ic_assoc(elist,10)) { rc=-2; goto out; }
			ic_ptcpy(ap1,ic_rbassoc->resval.rpoint);  /* Center */
			if (ic_assoc(elist,40)) { rc=-2; goto out; }
			ar1=ic_rbassoc->resval.rreal;  /* Radius */
			if (ic_assoc(elist,50)) { rc=-2; goto out; }
			ar2=ic_rbassoc->resval.rreal;  /* Start angle */
			if (ic_assoc(elist,51)) { rc=-2; goto out; }
			ar3=ic_rbassoc->resval.rreal;  /* End angle */
			if (ic_assoc(elist,210)) { rc=-2; goto out; }  /* Extrusion dir */
			ic_ptcpy(ucsrb.resval.rpoint,ic_rbassoc->resval.rpoint);
			ic_normang(&ar2,&ar3);
			seg->pt[0]=ap1[0]+ar1*cos(ar2);
			seg->pt[1]=ap1[1]+ar1*sin(ar2);
			seg->next->pt[0]=ap1[0]+ar1*cos(ar3);
			seg->next->pt[1]=ap1[1]+ar1*sin(ar3);
			seg->pt[2]=seg->next->pt[2]=ap1[2];
			seg->bulge=tan((ar3-ar2)/4.0);
		} else if (strisame(ic_rbassoc->resval.rstring,"CIRCLE"/*DNT*/)) {
			/* Make it look like a closed POLYLINE with two VERTEXs with */
			/* bulges of 1.0: */

			if ((seg= new struct dmseglink )==NULL )  { rc=-3; goto out; }
			memset(seg,0,sizeof(struct dmseglink));
			if ((seg->next= new struct dmseglink )==NULL) { rc=-3; goto out; }
			memset(seg->next,0,sizeof(struct dmseglink));
			if (ic_assoc(elist,10)) { rc=-2; goto out; }
			ic_ptcpy(ap1,ic_rbassoc->resval.rpoint);  /* Center */
			if (ic_assoc(elist,40)) { rc=-2; goto out; }
			ar1=ic_rbassoc->resval.rreal;  /* Radius */
			if (ic_assoc(elist,210)) { rc=-2; goto out; }  /* Extrusion dir */
			ic_ptcpy(ucsrb.resval.rpoint,ic_rbassoc->resval.rpoint);
			ar2=ar1*cos(snapang); ar3=ar1*sin(snapang);
			seg->pt[0]=      ap1[0]+ar2; seg->pt[1]=      ap1[1]+ar3;
			seg->next->pt[0]=ap1[0]-ar2; seg->next->pt[1]=ap1[1]-ar3;
			seg->pt[2]=seg->next->pt[2]=ap1[2];
			if (meas && (sds_getvar("ANGDIR"/*DNT*/,&rb)==RTNORM) && rb.resval.rint==1)
				seg->bulge=seg->next->bulge=-1.0;	//go clockwise
			else
				seg->bulge=seg->next->bulge=1.0;
			closed=1;
		} else if (strisame(ic_rbassoc->resval.rstring,"POLYLINE"/*DNT*/)) {
			if (ic_assoc(elist,70)) { rc=-2; goto out; }
			fi1=ic_rbassoc->resval.rint;
			if (fi1 & (IC_PLINE_POLYFACEMESH | IC_PLINE_3DMESH)) { rc=-4; goto out; }  /* No meshes */
			if (fi1 & IC_PLINE_CLOSED) closed=1;
			if (ic_assoc(elist,210)) { rc=-2; goto out; }  /* Extrusion dir */
			ic_ptcpy(ucsrb.resval.rpoint,ic_rbassoc->resval.rpoint);
			for (;;) {
				if (sds_entnext_noxref(fename,fename)!=RTNORM) break;
				IC_FREEIT
				if ((elist=sds_entget(fename))==NULL) { rc=-1; goto out; }
				if (ic_assoc(elist,0)) { rc=-2; goto out; }
				if (stricmp(ic_rbassoc->resval.rstring,"VERTEX"/*DNT*/)) break;
				if(fi1 & IC_PLINE_SPLINEFIT){
					//skip spline control pts
					ic_assoc(elist,70);
					if(ic_rbassoc->resval.rint & IC_VERTEX_SPLINEFRAME)continue;
				}
				if ((tseg1= new struct dmseglink )==NULL) { rc=-3; goto out; }
				memset(tseg1,0,sizeof(struct dmseglink));
				if (ic_assoc(elist,10)) { rc=-2; goto out; }
				ic_ptcpy(tseg1->pt,ic_rbassoc->resval.rpoint);
				if (ic_assoc(elist,42)) { rc=-2; goto out; }
				tseg1->bulge=ic_rbassoc->resval.rreal;
				if (seg==NULL) seg=tseg1; else pseg->next=tseg1;
				pseg=tseg1;
			}
		}
		else if (strisame(ic_rbassoc->resval.rstring,"LWPOLYLINE"/*DNT*/))
			{
			if (ic_assoc(elist,70)) 
				{
				rc=-2; 
				goto out; 
				}
			fi1=ic_rbassoc->resval.rint;
			if (fi1 & IC_PLINE_CLOSED)
				closed=1;
			if (ic_assoc(elist,210))  // extrusion dir
				{
				rc=-2;
				goto out;
				}
			ic_ptcpy(ucsrb.resval.rpoint,ic_rbassoc->resval.rpoint);
			resbuf *vertptr=ic_ret_assoc(elist,10); // point to first vert data
			do
				{
				if ((tseg1= new struct dmseglink )==NULL)
					{
					rc=-3;
					goto out;
					}
				memset(tseg1,0,sizeof(struct dmseglink));
				ic_ptcpy(tseg1->pt,vertptr->resval.rpoint);
				do
					vertptr=vertptr->rbnext;
				while (vertptr->restype!=42);
				tseg1->bulge=vertptr->resval.rreal;
				do
					vertptr=vertptr->rbnext;
				while (vertptr && vertptr->restype!=10);
				if (seg==NULL)
					seg=tseg1;
				else
					pseg->next=tseg1;
				pseg=tseg1;
				}
			while (vertptr);
			}
		else 
			{
			rc=-4;
			goto out;
			}

		if (seg==NULL || seg->next==NULL) { rc=-6; goto out; }
		if(revmode){
			//reverse the llist and go from end to start of entity...
			for(tseg1=tseg2=seg;tseg1!=NULL;seg->next=tseg2){
				if(seg==tseg2){
					seg=seg->next;
					tseg1=seg->next;
					tseg2->next=NULL;
					dx=seg->bulge;
					seg->bulge=-tseg2->bulge;
				}else{
					tseg2=seg;
					seg=tseg1;
					tseg1=tseg1->next;
					dy=seg->bulge;
					seg->bulge=-dx;
					dx=dy;
				}
			}
			for(tseg1=seg;tseg1->next!=NULL;tseg1=tseg1->next);
			tseg1->bulge=-dx;
			tseg1=tseg2=NULL;
			dx=dy=0.0;
		}

						/***** USE THE seg LLIST: *****/

		sofar=target=0.0; ndivpts=nmade=0;

		for (pass=0; pass<2; pass++) {
			/* 0=get total length */
			/* 1=convert dmval to a "measure" distance; calc the pts */

			if (pass) {
				/* See if need to convert dmval to an absolute "measure" dist. */
				/* Check number of divisions. */

				if (meas) { if (fractmode) dmval*=sofar; }
				else dmval=sofar/dmval;

				if (icadRealEqual(dmval,0.0) || sofar/dmval>(sds_real)maxnpts)
					{ rc=-8; goto out; }

				/* To avoid an end effect with Divide, figure out exactly */
				/* how many pts to determine: */
				if (!meas) ndivpts=(short)(sofar/dmval+0.001)-1+closed;
			}

			sofar=0.0; target=dmval;
			dobeg=(!meas && closed);  /* Do beg pt for closed Divides */
			for (tseg1=seg; tseg1!=NULL; tseg1=tseg1->next) { /* Each seg */

				/* Get the endpoint for this seg (tseg2): */
				if (tseg1->next==NULL) {  /* Last vertex */
					if (!closed) break;
					tseg2=seg;  /* Close */
				} else tseg2=tseg1->next;

				/* Get the chord vector and its length: */
				crd[0]=(tseg2->pt[0]-tseg1->pt[0]);
				crd[1]=(tseg2->pt[1]-tseg1->pt[1]);
				crd[2]=(tseg2->pt[2]-tseg1->pt[2]);
				crdlen=sqrt(crd[0]*crd[0]+crd[1]*crd[1]+crd[2]*crd[2]);

				/* Get radius, angle, and segment curve length: */
				if (icadRealEqual(tseg1->bulge,0.0)) {
					rad=1.0E+10;  /* Safety */
					seglen=crdlen;
				} else {
					rad=fabs(crdlen*(tseg1->bulge+1.0/tseg1->bulge)/4.0);
					seglen=rad*fabs(4.0*atan(tseg1->bulge));
				}

				sofar+=seglen; /* Dist covered so far (counting this segment) */

				if (!pass) continue;  /* Just measure total length */

				if (target<sofar || dobeg) {  /* At least 1 pt to do in this seg */

					if (!icadRealEqual(tseg1->bulge,0.0)) {  /* Going to need center of arc seg */

						/* Init center point at midpoint of chord: */
						cent[0]=(tseg1->pt[0]+tseg2->pt[0])/2.0;
						cent[1]=(tseg1->pt[1]+tseg2->pt[1])/2.0;
						cent[2]=(tseg1->pt[2]+tseg2->pt[2])/2.0;

						/* How far from the midpoint of the chord? */
						ar1=rad*rad-crdlen*crdlen/4.0;
						ar1=(ar1<0.0) ? 0.0 : sqrt(ar1);

						/* Set fi1 to +1 for right or -1 for left: */
						fi1=(tseg1->bulge>1.0 ||
							(tseg1->bulge<0.0 && tseg1->bulge>-1.0)) ? 1 : -1;

						/* Apply perpendicular offset (right or left): */
						if (!icadRealEqual(crdlen,0.0)) {
							cent[0]+=fi1*crd[1]*ar1/crdlen;
							cent[1]-=fi1*crd[0]*ar1/crdlen;
						}
					}

					while (target<sofar || dobeg) {  /* Calc the pt(s): */

						if (sds_usrbrk()) { rc=-9; goto out; }

						/* Get dist into this seg: */
						ar1=(dobeg) ? 0.0 : target-sofar+seglen;

						/* Set up thispt as the pt and dx and dy for */
						/* determining the tangent direction: */
						ic_ptcpy(thispt,tseg1->pt);
						dx=1.0; dy=0.0;  /* Init for tan rotation of 0.0 */
						if (icadRealEqual(tseg1->bulge,0.0)) {  /* Line seg */
							dx=crd[0]; dy=crd[1];  /* For tan rot */
							if (!icadRealEqual(crdlen,0.0)) {
								thispt[0]+=crd[0]*ar1/crdlen;
								thispt[1]+=crd[1]*ar1/crdlen;
								thispt[2]+=crd[2]*ar1/crdlen;
							}
						} else if (rad>0.0) {  /* Arc with non-0 radius */

							ar1/=rad;  /* Angle to go */

							if (tseg1->bulge<0.0) ar1=-ar1;
							sn=sin(ar1); cs=cos(ar1);

							/* Find the end pt by rotating about cent: */
							ar1=tseg1->pt[0]-cent[0];
							ar2=tseg1->pt[1]-cent[1];
							thispt[0]=cent[0]+ar1*cs-ar2*sn;
							thispt[1]=cent[1]+ar2*cs+ar1*sn;

							/* Get dx and dy for tan rot: */
							dx=thispt[0]-cent[0]; dy=thispt[1]-cent[1];
						}

						/* Determine the tangent direction: */
						if (icadRealEqual(dx,0.0)) { tandir=hpi; if (dy<0.0) tandir+=pi; }
						else { tandir=atan(dy/dx); if (dx<0.0) tandir+=pi; }

						if (!icadRealEqual(tseg1->bulge,0.0)) {
							tandir+=hpi;  /* For CCW */
							if (tseg1->bulge<0.0) tandir+=pi;  /* For CW */
						}
						ic_normang(&tandir,NULL);


						/* Use thispt: */

						if (putpt) {

							/* POINTs are true 3D (WCS), so convert the UCS pt */
							/* to WCS: */
							if (!icadRealEqual(ucsrb.resval.rpoint[0],0.0) ||
								!icadRealEqual(ucsrb.resval.rpoint[1],0.0))
									fi1=sds_trans(thispt,&ucsrb,&wcsrb,1,ap1);
							else { ic_ptcpy(ap1,thispt); fi1=RTNORM; }

							if (fi1==RTNORM) {
								IC_FREEIT
								elist=sds_buildlist(
									RTDXF0,"POINT"/*DNT*/,
										 8,clayer,
										 6,celtype,
										62,cecolor,
										10,ap1,
										 0);
								if (elist==NULL) { rc=-3; goto out; }
								if (sds_entmake(elist)!=RTNORM)
									{ rc=-5; goto out; }
							}
						}

						if (putblk) {
							IC_FREEIT
							elist=sds_buildlist(
								RTDXF0,"INSERT"/*DNT*/,
									 8,clayer,
									 6,celtype,
									62,cecolor,
									 2,bname,
									10,thispt,
									41,bscl,
									42,bscl,
									43,bscl,
									50,(rottan) ? tandir : brot,
								   210,ucsrb.resval.rpoint,
									 0);
							if (elist==NULL) { rc=-3; goto out; }
							if (sds_entmake(elist)!=RTNORM) { rc=-5; goto out; }
						}

						if (ptlistp!=NULL) {

							/* Add the point link: */
							if (( rbp1=sds_newrb(RT3DPOINT) )==NULL)
									{ rc=-3; goto out; }
							rbp1->restype=RT3DPOINT;
							ic_ptcpy(rbp1->resval.rpoint,thispt);
							if (ptl==NULL) ptl=rbp1; else prevptl->rbnext=rbp1;
							prevptl=rbp1;

							if (ptnrot) {
								/* Add the tangent direction link: */
								if ((rbp1=sds_newrb(RTANG))==NULL)
										{ rc=-3; goto out; }
								rbp1->restype=RTANG;
								rbp1->resval.rreal=tandir;
								if (ptl==NULL) ptl=rbp1; else prevptl->rbnext=rbp1;
								prevptl=rbp1;
							}

						}

						nmade++;

						if (onept && !dobeg) goto out;

						/* For Divide, avoid end effects by making no more */
						/* than the required number of pts: */
						if (!meas && nmade>=ndivpts) { rc=0; goto out; }

						if (dobeg) dobeg=0; else target+=dmval;

					}  /* end while target<sofar */

				}  /* end if target<sofar || dobeg */

			} /* End for each seg */

		} /* End for pass */
	}

out:
	IC_FREEIT

	if (pCurve) {
		delete pCurve;
		pCurve = NULL;
	}

	for (tseg1=seg; tseg1!=NULL; tseg1=tseg2) { 
		tseg2=tseg1->next; 
		delete tseg1; 
	}
	seg=pseg=NULL;

	if (rc && ptl!=NULL) { 
		sds_relrb(ptl); 
		ptl=NULL; 
	}

    if (ptlistp!=NULL) 
		*ptlistp=ptl;

    return rc;
}
