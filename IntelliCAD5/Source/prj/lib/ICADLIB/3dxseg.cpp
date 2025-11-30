/* D:\ICADDEV\PRJ\LIB\ICADLIB\3DXSEG.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "icadlib.h"/*DNT*/
#include "EntConversion.h" /*DNT*/
#include "db.h"
#include "entities.h"
#include "curve.h"
#include "line.h"
#include "elliparc.h"
#include "nurbs.h"
#include "crvpair.h"

#undef	X	
#undef	Y	
#include <windows.h>

static const double EPSILON = 0.01;

sds_real ic_dist2d (sds_point p1,sds_point p2){
	return(sqrt((p2[0]-p1[0])*(p2[0]-p1[0])+(p2[1]-p1[1])*(p2[1]-p1[1])));
}

//support f'n
int ic_direction_equal(sds_point p1,sds_point p2, sds_point p3, sds_point p4){
	//assumes all 4 pts are colinear.
	//returns 1 if vector p1-p2 is same direction as vector p3-p4, 0 otherwise
	int i;

	if(fabs(p2[0]-p1[0])>fabs(p2[1]-p1[1])){
		if(fabs(p2[0]-p1[0])<fabs(p2[2]-p1[2]))i=2;
		else i=0;
	}else if(fabs(p2[1]-p1[1])<fabs(p2[2]-p1[2]))i=2;
	else i=1;

	if(((p2[i]-p1[i])*(p4[i]-p3[i]))>0.0)return(1);
	else return(0);
}

int ic_3dxseg(sds_point s0p0, sds_point s0p1, short type0, sds_point extru0, sds_name ent0,
				 sds_point s1p0, sds_point s1p1, short type1, sds_point extru1, sds_name ent1,
				 sds_real fuzz, sds_point* & pXPoints) {

/*	This is a modified version of ic_segxseg
**
**  Given two segments (line,arc,circle,ellipse,spline) defined by s0p0, s0p1, s1p0, and s1p1,
**  this function determines whether they intersect and where (i0 and i1).
**  All points for STRAIGHT (TYPE 0) segments MUST BE IN WCS.
**		Note: This is native for lines & 3dpolys, but not for 2d polys
**  All pts for other segments must be in ECS, which is native for them.  
**  The ECS is defined by the normal extru0 or extru1 and the parent entity
**  for the segment is defined as ent0 or ent1. Intersection pts are returned in WCS!!!
**
**  The variable names of the 1st 4 sds_points give the segment and point
**  numbers.  For example, s0p1 is "Segment 0, Point 1".
**
**  type0 and type1 indicate the segment types: 0=line; 1=arc; 2=circle, 3=ray,4=xline,5=ellipse,6=spline.
**
**  For line segs, the two s?p? points are the endpoints.
**
**  For circles or CCW arcs, the 1st sds_point (s?p0) is the center and the 2nd
**  sds_point (s?p1) is as follows:
**    s?p1[0] : radius
**    s?p1[1] : start angle (radians)	(N/A for circles)
**    s?p1[2] : end   angle (radians)	(N/A for circles)

**
**  If the geometry functions (ic_linexline(), ic_linexarc(), and
**  ic_arcxarc()) find no solution (parallel lines, zero length segs,
**  tangent segs, etc.) the seg endpts are checked for coincidence.
**
**  If an intersection is found but is not technically on both segs,
**  the proximity of the intersection to the two segs is checked and
**  the point is taken if it's close enough to both segs.
**
**  Pass parameter "fuzz" is used for the proximity checks.
**  If an ename is NULL, it's assumed to be a WCS entity
**
**  Returns the number of valid intersections:
**      0 : No intersections (neither i0 nor i1 is valid).
**      1 : i0 is valid.
**      2 : i0 and i1 are valid.  (For 2 lines, this only occurs with
**            coincident line segs; the 2 endpts are taken).
*/
   
	// hack to fix bug 1-62981
	// since arc/arc processing seems to work and circle/arc processing
	// doesn't, we are going to change the circles into arcs.

	if (pXPoints != NULL) {
		delete [] pXPoints;
		pXPoints = NULL;
	}

	if (type0 == 2)
		{
		s0p1[1] = 0.0;
		s0p1[2] = IC_TWOPI;
		type0 = 1;
		}

	if (type1 == 2)
		{
		s1p1[1] = 0.0;
		s1p1[2] = IC_TWOPI;
		type1 = 1;
		}

    int take0,take1,fi1,fi2,fi3,fi4, numPoints = 0;
	short arctyp,linetyp;
    sds_real ar1;
    sds_real *lnp0,*lnp1,*arcp0,*arcp1,fr1,fr2,fr3;
    sds_point ap[4]={{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0}};
	sds_point pt1,pt2, i0, i1;
	struct resbuf rbwcs,rbent;
	rbwcs.rbnext=rbent.rbnext=NULL;
	rbwcs.restype=RTSHORT;rbwcs.resval.rint=0;

	if(type0==1 || type0==2){//normalize the extrusion vector
		fr1=extru0[0]*extru0[0]+extru0[1]*extru0[1]+extru0[2]*extru0[2];
		if((fr1>=1+(fuzz+2.0)*fuzz)&&(fr1<=1+(fuzz-2.0)*fuzz)){
			extru0[0]/=fr1;
			extru0[1]/=fr1;
			extru0[2]/=fr1;
		}
	}
	if(type1==1 || type1==2){//normalize the extrusion vector
		fr1=extru1[0]*extru1[0]+extru1[1]*extru1[1]+extru1[2]*extru1[2];
		if((fr1>=1+(fuzz+2.0)*fuzz)&&(fr1<=1+(fuzz-2.0)*fuzz)){
			extru1[0]/=fr1;
			extru1[1]/=fr1;
			extru1[2]/=fr1;
		}
	}

    take0=take1=0;
	if (type0 == 5 || type0 == 6 || type1 == 5 || type1 == 6) { // one of segments is ellipse or spline
		RC rc = SUCCESS;
		CCurve* pCurve1 = NULL, *pCurve2 = NULL;
		if (type0 == 5 || type0 == 6) {
			if (type0 == 5) 
				pCurve1 = asGeoEllipArc((db_ellipse*)ent0[0]); 
			else
				pCurve1 = asGeoSpline((db_spline*)ent0[0]);

			if (type1 == 0 || type1 == 3 || type1 == 4) {	// line, ray, xline
				if (type1 == 0)
					pCurve2 = new CLine(C3Point(s1p0[0], s1p0[1], s1p0[2]),
					                    C3Point(s1p1[0], s1p1[1], s1p1[2]));
				else if (type1 == 3) 
					pCurve2 = new CRay(C3Point(s1p0[0], s1p0[1], s1p0[2]),
					                    C3Point(s1p1[0] - s1p0[0], s1p1[1] - s1p0[1], s1p1[2] - s1p0[2]));
				else
					pCurve2 = new CUnboundedLine(C3Point(s1p0[0], s1p0[1], s1p0[2]),
												 C3Point(s1p1[0] - s1p0[0], s1p1[1] - s1p0[1], s1p1[2] - s1p0[2]));
			}
			else if (type1 == 1) { // arc or circle
				if (ent1 != NULL) {
					rbent.restype=RTENAME;
					ic_encpy(rbent.resval.rlname, ent1);
				} 
				else {
					rbent.restype = RTSHORT;
					rbent.resval.rint=0;
				}
				sds_point center;
				sds_trans(s1p0, &rbent, &rbwcs,0,center);
                 
				double sAngle = s1p1[1];
				double eAngle = s1p1[2];
				if (eAngle < sAngle)
					eAngle += TWOPI;

				pCurve2 = new CEllipArc(s1p1[0], C3Point(center[0], center[1], center[2]),
					                    s1p1[1], eAngle - sAngle, rc);
			}
			else if (type1 == 5) // elliparc 
				pCurve2 = asGeoEllipArc((db_ellipse*)ent1[0]);
			else	// spline
				pCurve2 = asGeoSpline((db_spline*)ent1[0]);
		}
		else {
			if (type1 == 5) 
				pCurve2 = asGeoEllipArc((db_ellipse*)ent1[0]);
			else
		        pCurve2 = asGeoSpline((db_spline*)ent1[0]);

			if (type0 == 0 || type0 == 3 || type0 == 4) {	// line, ray, xline
				if (type0 == 0)
					pCurve1 = new CLine(C3Point(s0p0[0], s0p0[1], s0p0[2]),
					                    C3Point(s0p1[0], s0p1[1], s0p1[2]));
				else if (type0 == 3) 
					pCurve1 = new CRay(C3Point(s0p0[0], s0p0[1], s0p0[2]),
					                    C3Point(s0p1[0] - s0p0[0], s0p1[1] - s0p0[1], s0p1[2] - s0p0[2]));
				else
					pCurve1 = new CUnboundedLine(C3Point(s0p0[0], s0p0[1], s0p0[2]),
												 C3Point(s0p1[0] - s0p0[0], s0p1[1] - s0p0[1], s0p1[2] - s0p0[2]));
			}
			else if (type0 == 1) { // arc or circle
				if (ent0 != NULL) {
					rbent.restype=RTENAME;
					ic_encpy(rbent.resval.rlname, ent0);
				} 
				else {
					rbent.restype = RTSHORT;
					rbent.resval.rint=0;
				}
				sds_point center;
				sds_trans(s0p0, &rbent, &rbwcs,0,center);

				double sAngle = s0p1[1];
				double eAngle = s0p1[2];
				if (eAngle < sAngle)
					eAngle += TWOPI;
				pCurve1 = new CEllipArc(s0p1[0], C3Point(center[0], center[1], center[2]),
					                        s0p1[1], eAngle - sAngle, rc);
			}
			else if (type0 == 5) // elliparc 
				pCurve1 = asGeoEllipArc((db_ellipse*)ent0[0]);
			else	// spline
				pCurve1 = asGeoSpline((db_spline*)ent0[0]);

		};
		if (pCurve1 && pCurve2) {
			CRealArray cOnThis, cOnOther;
			rc = Intersect(pCurve1, pCurve2, cOnThis, cOnOther, EPSILON);
			if (rc == SUCCESS && cOnThis.Size()) {
				C3Point pt, tan;
				double vv;
				numPoints = cOnThis.Size();
				pXPoints = new sds_point[numPoints];
				for (register i = 0; i < numPoints; i++) {
					pCurve1->Evaluate(cOnThis[i], pt, tan, vv);
					pXPoints[i][0] = pt.X(); pXPoints[i][1] = pt.Y(); pXPoints[i][2] = pt.Z();
				}
			}
		}
		delete pCurve1;
		delete pCurve2;

		return numPoints;
	}
    else if ((type0==0 || (type0==3 ||type0==4)) && (type1==0 || (type1==3 || type1==4))) {  /* line(ray/xline)-line(ray/xline) */
		if(type0+type1==0){			//two lines
			fi1=sds_inters(s0p0,s0p1,s1p0,s1p1,47,i0);
		}else if(type0+type1==8){	//two xlines
 			fi1=sds_inters(s0p0,s0p1,s1p0,s1p1,0,i0);
		}else{						//mixed bag of stuff
 			fi1=sds_inters(s0p0,s0p1,s1p0,s1p1,0,i0);
			if(fi1==RTNORM){
				//check if i0 is actually on the segment
				fi2=fi3=0;
				if(type0==4)fi2=1;
				if(type1==4)fi3=1;
				if(type0==3 && ic_direction_equal(s0p0,s0p1,s0p0,i0))fi2=1;
				if(type1==3 && ic_direction_equal(s1p0,s1p1,s1p0,i0))fi3=1;
				if(type0==0){
					//check if int is between endpts.
					//get dimension of maximum gradient(change)
					if(fabs(s0p1[0]-s0p0[0])>fabs(s0p1[1]-s0p0[1])){
						if(fabs(s0p1[0]-s0p0[0])>fabs(s0p1[2]-s0p0[2]))fi4=0;
						else fi4=2;
					}else if(fabs(s0p1[1]-s0p0[1])>fabs(s0p1[2]-s0p0[2]))fi4=1;
					else fi4=2;
					if(i0[fi4]>min(s0p0[fi4],s0p1[fi4]) && i0[fi4]<max(s0p0[fi4],s0p1[fi4]))fi2=1;
				}
				if(type1==0){
					//check if int is between endpts.
					//get dimension of maximum gradient(change)
					if(fabs(s1p1[0]-s1p0[0])>fabs(s1p1[1]-s1p0[1])){
						if(fabs(s1p1[0]-s1p0[0])>fabs(s1p1[2]-s1p0[2]))fi4=0;
						else fi4=2;
					}else if(fabs(s1p1[1]-s1p0[1])>fabs(s1p1[2]-s1p0[2]))fi4=1;
					else fi4=2;
					if(i0[fi4]>min(s1p0[fi4],s1p1[fi4]) && i0[fi4]<max(s1p0[fi4],s1p1[fi4]))fi3=1;
				}
				if(fi2+fi3==2) {
					numPoints = 1;
					pXPoints = new sds_point[numPoints];
					ic_ptcpy(pXPoints[0], i0); 
					return numPoints;
				}
			}
		}

		if (fi1!=RTNORM) {  /* No solution */
            /* Check endpt proximity (parallel or zero-length line segs */
            /* joined at endpts): */

            fi2=0;

            if ((fabs(s0p0[0]-s1p0[0])<=fuzz && fabs(s0p0[1]-s1p0[1])<=fuzz && fabs(s0p0[2]-s1p0[2])<=fuzz) ||
                (fabs(s0p0[0]-s1p1[0])<=fuzz && fabs(s0p0[1]-s1p1[1])<=fuzz && fabs(s0p0[2]-s1p1[2])<=fuzz))
                    { ic_ptcpy(i0,s0p0); fi2++; }

            if ((fabs(s0p1[0]-s1p0[0])<=fuzz && fabs(s0p1[1]-s1p0[1])<=fuzz && fabs(s0p1[2]-s1p0[2])<=fuzz) ||
                (fabs(s0p1[0]-s1p1[0])<=fuzz && fabs(s0p1[1]-s1p1[1])<=fuzz && fabs(s0p1[2]-s1p1[2])<=fuzz)) {

                if (fi2) 
					ic_ptcpy(i1,s0p1); 
				else 
					ic_ptcpy(i0,s0p1);
                fi2++;
            }

			if (fi2) {
				numPoints = fi2;
				pXPoints = new sds_point[numPoints];
				ic_ptcpy(pXPoints[0], i0); 
				if (numPoints > 1)
					ic_ptcpy(pXPoints[1], i1); 
			}
            return fi2;
        }
		else {
			numPoints = 1;
			pXPoints = new sds_point[numPoints];
			ic_ptcpy(pXPoints[0], i0); 
			return numPoints;
		}
    } 
	else if ((type0==1 || type0==2) && (type1==1 || type1==2)) {  /* arc/cir and arc/cir */
		fi2=0;//init return value

		fr1=extru0[0]*extru1[1]-extru1[0]*extru0[1];
		fr2=extru0[0]*extru1[2]-extru1[0]*extru0[2];
		fr3=extru0[1]*extru1[2]-extru1[1]*extru0[2];

		if(fabs(fr1)>fuzz || fabs(fr2)>fuzz || fabs(fr3)>fuzz){
			//if the 2 extrusion directions are NOT parallel
			//find abs max of fr1,fr2,&fr3
			if(fabs(fr1)>fabs(fr2)){
				if(fabs(fr1)>fabs(fr3))fi1=0;
				else fi1=2;
			}else{
				if(fabs(fr2)>fabs(fr3))fi1=1;
				else fi1=2;
			}
			//get a point where planes intersect & call it pt1
			pt1[0]=pt1[1]=pt1[2]=0.0;
			if(fi1==0){
				pt1[1]=(extru1[2]*s0p0[2]-extru0[2]*s1p0[2])/fr3;
				pt1[2]=(s1p0[2]*extru0[1]-s0p0[2]*extru1[1])/fr3;
			}else if(fi1==1){
				pt1[0]=(extru1[2]*s0p0[2]-extru0[2]*s1p0[2])/fr2;
				pt1[2]=(s1p0[2]*extru0[0]-s0p0[2]*extru1[0])/fr2;
			}else{
				pt1[0]=(extru1[1]*s0p0[2]-extru0[1]*s1p0[2])/fr1;
				pt1[1]=(s1p0[2]*extru0[0]-s0p0[2]*extru1[0])/fr1;
			}
			//cross the 2 normals to get a second point on the line where planes intersect...
			pt2[0]=pt1[0]+fr3;
			pt2[1]=pt1[1]+fr2;
			pt2[2]=pt1[2]+fr1;
			//we now have 2 pts (pt1 & pt2) on line of intersection of 2 planes in WCS.
			if(ent0!=NULL){
				rbent.restype=RTENAME;
				ic_encpy(rbent.resval.rlname,ent0);
			}else{
				rbent.restype=RTSHORT;
				rbent.resval.rint=0;
			}
			sds_trans(pt1,&rbwcs,&rbent,0,ap[0]);
			sds_trans(pt2,&rbwcs,&rbent,0,ap[1]);
			fi1=ic_linexarc(ap[0],ap[1],s0p0,s0p1[0],s0p1[1],s0p0[2],ap[2],ap[3]);
			if((fi1&3)==3 || ((fi1&3)==1 && type0==2)){
				ap[2][2]=ap[0][2]+(ap[1][2]-ap[0][2])*ic_dist2d(ap[0],ap[2])/ic_dist2d(ap[0],ap[1]);
				if(fabs(ap[2][2]-s0p0[2])<=fuzz){
					//if ap[2] is on 1st arc, check if it's on the second
					sds_trans(ap[2],&rbent,&rbwcs,0,pt1);
					if(ent1!=NULL){
						rbent.restype=RTENAME;
						ic_encpy(rbent.resval.rlname,ent1);
					}else{
						rbent.restype=RTSHORT;
						rbent.resval.rint=0;
					}
					sds_trans(pt1,&rbwcs,&rbent,0,ap[2]);
					//now check that ap[2] is on ent1
					if(fabs(ap[2][2]-s1p0[2])<=fuzz){
						ic_segdist(ap[2],s1p0,s1p1,type1,&fr1,NULL);
						if(fr1<=fuzz){
							fi2=1;
							sds_trans(pt1,&rbent,&rbwcs,0,i0);
							//ic_ptcpy(i0,pt1);
						}
					}
				}
			}
			if((fi1&12)==12 || ((fi1&12)==4 && type0==2)){
				ap[3][2]=ap[0][2]+(ap[1][2]-ap[0][2])*ic_dist2d(ap[0],ap[3])/ic_dist2d(ap[0],ap[1]);
				if(fabs(ap[3][2]-s0p0[2])<=fuzz){
					//if ap[3] is on 1st arc, check if it's on the second
					sds_trans(ap[3],&rbent,&rbwcs,0,pt2);
					if(ent1!=NULL){
						rbent.restype=RTENAME;
						ic_encpy(rbent.resval.rlname,ent1);
					}else{
						rbent.restype=RTSHORT;
						rbent.resval.rint=0;
					}
					sds_trans(pt2,&rbwcs,&rbent,0,ap[3]);
					//now check that ap[3] is on ent1
					if(fabs(ap[3][2]-s1p0[2])<=fuzz){
						ic_segdist(ap[3],s1p0,s1p1,type1,&fr1,NULL);
						if(fr1<=fuzz){
							if(fi2==0){
								fi2=1;
								sds_trans(pt2,&rbent,&rbwcs,0,i0);
								//ic_ptcpy(i0,pt2);
							}else{
								fi2=2;
								sds_trans(pt2,&rbent,&rbwcs,0,i1);
								//ic_ptcpy(i1,pt2);
							}
						}
					}
				}
			}
			if (fi2) {
				numPoints = fi2;
				pXPoints = new sds_point[numPoints];
				ic_ptcpy(pXPoints[0], i0); 
				if (numPoints > 1)
					ic_ptcpy(pXPoints[1], i1); 
			}
			return(fi2);
		}else{//parallel extrusion directions
			if(fabs(s0p0[2]-s1p0[2])>fuzz)return(0);//if elevations differ, no int's possible

			fi1=ic_arcxarc(s0p0,s0p1[0],s0p1[1],s0p1[2],
						   s1p0,s1p1[0],s1p1[1],s1p1[2],
						   i0,i1);
		}

        if (fi1<0) {  /* No solution; check for endpt coincidence: */

			if(type0==1){
				/* Get endpts of seg 0: */
				ap[0][0]=s0p0[0]+s0p1[0]*cos(s0p1[1]);
				ap[0][1]=s0p0[1]+s0p1[0]*sin(s0p1[1]);
				ap[1][0]=s0p0[0]+s0p1[0]*cos(s0p1[2]);
				ap[1][1]=s0p0[1]+s0p1[0]*sin(s0p1[2]);
			}

			if(type1==1){
				/* Get endpts of seg 1: */
				ap[2][0]=s1p0[0]+s1p1[0]*cos(s1p1[1]);
				ap[2][1]=s1p0[1]+s1p1[0]*sin(s1p1[1]);
				ap[3][0]=s1p0[0]+s1p1[0]*cos(s1p1[2]);
				ap[3][1]=s1p0[1]+s1p1[0]*sin(s1p1[2]);
			}

            fi2=0;
			if((type0==1)&&(type1==1)){		/*Two Arcs*/
				if ((fabs(ap[0][0]-ap[2][0])<=fuzz &&
					 fabs(ap[0][1]-ap[2][1])<=fuzz) ||
					(fabs(ap[0][0]-ap[3][0])<=fuzz &&
					 fabs(ap[0][1]-ap[3][1])<=fuzz))
						{ ic_ptcpy(i0,ap[0]); fi2++; }

				if ((fabs(ap[1][0]-ap[2][0])<=fuzz &&
					 fabs(ap[1][1]-ap[2][1])<=fuzz) ||
					(fabs(ap[1][0]-ap[3][0])<=fuzz &&
					 fabs(ap[1][1]-ap[3][1])<=fuzz)) {

					 if (fi2) ic_ptcpy(i1,ap[1]); else ic_ptcpy(i0,ap[1]);
					 fi2++;
				}
			} else if((type0==2)&&(type1==2)){	/*Two Circles*/
				if((s0p1[0]<=fuzz)||(s1p1[0]<=fuzz)||	//either rr<fuzz
					(((fabs(s0p0[0]-s1p0[0]))<=fuzz)&&	//coincident cc's
					 ((fabs(s0p0[1]-s1p0[1]))<=fuzz)))return(0);

				ar1=((s0p0[0]-s1p0[0])*(s0p0[0]-s1p0[0]))+	//dx squared +
					((s0p0[1]-s1p0[1])*(s0p0[1]-s1p0[1]));	//dy squared

				if(fabs(ar1-((s0p1[0]+s1p1[0])*(s0p1[0]+s1p1[0])))<fuzz){ //(r1+r2)squared

					i0[0]=s0p0[0]+((s0p1[0]/(s0p1[0]+s1p1[0]))*
							(s1p0[0]-s0p0[0])/(s1p0[1]-s0p0[1]));
					i1[1]=s0p0[1]+((s0p1[0]/(s0p1[0]+s1p1[0]))*
							(s1p0[1]-s0p0[1])/(s1p0[0]-s0p0[0]));
					fi2=1;
				}
			}else if((type0==1)&&(type1==2)){	/*Arc & Circle*/
				//check distance of endpoints of arc from circle center
				//note: easier to check square of distances
				//check arc0 endpoint0
				ar1=((ap[0][0]-s1p0[0])*(ap[0][0]-s1p0[0])+	//dx squared					
				   (ap[0][1]-s1p0[1])*(ap[0][1]-s1p0[1]));	//dy squared
				if(fabs(ar1-s1p1[0]*s1p1[0])<=(fuzz*fuzz)){
					ic_ptcpy(i0,ap[0]);
					fi2++;
				}
				//check arc0 endpoint1
				ar1=((ap[1][0]-s1p0[0])*(ap[1][0]-s1p0[0])+	//dx squared					
				   (ap[1][1]-s1p0[1])*(ap[1][1]-s1p0[1]));	//dy squared
				if(fabs(ar1-s1p1[0]*s1p1[0])<=(fuzz*fuzz)){
					if (fi2) ic_ptcpy(i1,ap[1]); else ic_ptcpy(i0,ap[1]);
					fi2++;
				}
			}else{								/*Circle & Arc*/
				//check arc1 endpoint0
				ar1=(ap[2][0]-s0p0[0])*(ap[2][0]-s0p0[0])+ //dx squared
					(ap[2][1]-s0p0[1])*(ap[2][1]-s0p0[1]);	 //dy squared
				if(fabs(ar1-s0p1[0]*s0p1[0])<=(fuzz*fuzz)){
					ic_ptcpy(i0,ap[2]);
					fi2++;
				}
				//check arc1 endpoint1
				ar1=(ap[3][0]-s0p0[0])*(ap[3][0]-s0p0[0])+ //dx squared
					(ap[3][1]-s0p0[1])*(ap[3][1]-s0p0[1]);	 //dy squared
				if(fabs(ar1-s0p1[0]*s0p1[0])<=(fuzz*fuzz)){
					if (fi2) ic_ptcpy(i1,ap[3]); else ic_ptcpy(i0,ap[3]);
					fi2++;
				}
			}
			if (fi2) {
				numPoints = fi2;
				pXPoints = new sds_point[numPoints];
				ic_ptcpy(pXPoints[0], i0); 
				if (numPoints > 1)
					ic_ptcpy(pXPoints[1], i1); 
			}
            return fi2;

        }

		// The following hack was my attempt to fix bug 1-62981.
		// It does fix the test case submitted for this bug, but I have
		// now found any unfavorable side effects.
		// so I have commented out the following two lines, thus letting
		// the code revert to its former state.

		// if((type0==2) && (type1==1)) type0 = 1; // hack to fix bug 1-62981
		// if((type1==2) && (type0==1)) type1 = 1; // hack to fix bug 1-62981

        /* Found solution(s) on the circles */
		if((type0==1)&&(type1==1)){
			take0=((fi1&3)==3);		/* i0 is on both */
	        take1=((fi1&12)==12);	/* i1 is on both */
		}else if((type0==2)&&(type1==2)){
			//both are circles and there are crossings because
			//arcxarc did not return -1
			take0=take1=1;
		}else if(type0==1){	//only 1st entity is arc, 2nd is circle
			take0=(fi1&1);
			take1=(fi1&4);
		}else{				//only 2nd entity is arc, 1st is circle
			take0=(fi1&2);
			take1=(fi1&8);
		}

        if (fuzz>0.0) {  /* Check proximity to the segs */
            if (!take0) {
                ic_segdist(i0,s0p0,s0p1,type0,&ar1,NULL);
                if (ar1<=fuzz) {
                    ic_segdist(i0,s1p0,s1p1,type1,&ar1,NULL);
                    if (ar1<=fuzz) take0=1;
                }
            }
            if (!take1) {
                ic_segdist(i1,s0p0,s0p1,type0,&ar1,NULL);
                if (ar1<=fuzz) {
                    ic_segdist(i1,s1p0,s1p1,type1,&ar1,NULL);
                    if (ar1<=fuzz) take1=1;
                }
            }
        }

        if ((fi1=take0+take1)==0 || fi1==2) {
			if (fi1) {
				numPoints = fi1;
				pXPoints = new sds_point[numPoints];
				ic_ptcpy(pXPoints[0], i0); 
				if (numPoints > 1)
					ic_ptcpy(pXPoints[1], i1); 
			}
			return fi1;
		}
        if (take1) ic_ptcpy(i0,i1);
        /* Only i1 should be taken; put it in i0. */

		numPoints = 1;
		pXPoints = new sds_point[numPoints];
		ic_ptcpy(pXPoints[0], i0); 
        return 1;

    } else {  /* line-arc or arc-line */

		fi3=fi4=0;		
        /* Figure out which is which: */
        if (type0==0 || type0>2){	//type0 is line
			lnp0=s0p0; lnp1=s0p1; arcp0=s1p0; arcp1=s1p1;arctyp=type1;linetyp=type0;
			if(ent1!=NULL){
				rbent.restype=RTENAME;
				ic_encpy(rbent.resval.rlname,ent1);
			}else{
				rbent.restype=RTSHORT;
				rbent.resval.rint=0;
			}
		}else{ 
			lnp0=s1p0; lnp1=s1p1; arcp0=s0p0; arcp1=s0p1;arctyp=type0;linetyp=type1;
			if(ent0!=NULL){
				rbent.restype=RTENAME;
				ic_encpy(rbent.resval.rlname,ent0);
			}else{
				rbent.restype=RTSHORT;
				rbent.resval.rint=0;
			}
		}
		sds_trans(lnp0,&rbwcs,&rbent,0,pt1);
		sds_trans(lnp1,&rbwcs,&rbent,0,pt2);

        fi1=ic_linexarc(pt1,pt2,arcp0,arcp1[0],arcp1[1],arcp1[2],i0,i1);
		//we're in ECS, so arc is all at one elevation.  Check to see
		// if the int's are at the correct elevation....
		if(fi1>-1){
			if(linetyp>2){
				if(linetyp==4)fi1|=5;
				if(linetyp==3 && ic_direction_equal(pt1,pt2,pt1,i0))fi1|=1;
				if(linetyp==3 && ic_direction_equal(pt1,pt2,pt1,i1))fi1|=4;
			}
			if((arctyp==2 && (fi1&1)) || (arctyp==1 && (fi1&3)==3)){
				i0[2]=pt1[2]+(pt2[2]-pt1[2])*ic_dist2d(pt1,i0)/ic_dist2d(pt1,pt2);
				if(fabs(i0[2]-arcp0[2])>fuzz){
					if(fi1&2)fi1-=3;
					else fi1-=1;
				}else fi3=1; //z for i0 is ok
			}
			if((arctyp==2 && (fi1&4)) || (arctyp==1 && (fi1&12)==12)){
				i1[2]=pt1[2]+(pt2[2]-pt1[2])*ic_dist2d(pt1,i1)/ic_dist2d(pt1,pt2);
				if(fabs(i1[2]-arcp0[2])>fuzz){
					if(fi1&8)fi1-=12;
					else fi1-=4;
				}else fi4=1; //z for i1 is ok
			}
		}

        if (fi1<0) {  /* No solution; check for endpt coincidence: */

			fi1=0;

			if(arctyp==1){
				/* Get endpts of the arc: */
				ap[0][0]=arcp0[0]+arcp1[0]*cos(arcp1[1]);
				ap[0][1]=arcp0[1]+arcp1[0]*sin(arcp1[1]);
				ap[1][0]=arcp0[0]+arcp1[0]*cos(arcp1[2]);
				ap[1][1]=arcp0[1]+arcp1[0]*sin(arcp1[2]);

				if ((fabs(pt1[0]-ap[0][0])<=fuzz &&
					 fabs(pt1[1]-ap[0][1])<=fuzz && 
					 fabs(pt1[2]-ap[0][2])<=fuzz)||
					(fabs(pt1[0]-ap[1][0])<=fuzz &&
					 fabs(pt1[1]-ap[1][1])<=fuzz &&
					 fabs(pt1[2]-ap[1][2])<=fuzz))
						{ ic_ptcpy(i0,lnp0); fi1++; }

				if ((fabs(pt2[0]-ap[0][0])<=fuzz &&
					 fabs(pt2[1]-ap[0][1])<=fuzz &&
					 fabs(pt2[2]-ap[0][2])<=fuzz) ||
					(fabs(pt2[0]-ap[1][0])<=fuzz &&
					 fabs(pt2[1]-ap[1][1])<=fuzz &&
					 fabs(pt2[2]-ap[1][2])<=fuzz)) {

					 if (fi1){
						 ic_ptcpy(i1,lnp1); fi1+=4;
					 }else{
						 ic_ptcpy(i0,lnp1);fi1++;
					 }
				}
			}else{	/*arctyp==2 (a circle) */
				//check if either line endpoint is dist arcp1[0] from center
				//check square of distance
				ar1=((pt1[0]-arcp0[0])*(pt1[0]-arcp0[0]))+
					((pt1[1]-arcp0[1])*(pt1[1]-arcp0[1]));
				if((fabs(arcp1[0]*arcp1[0]-ar1)<=(fuzz*fuzz))&&(fabs(pt1[2]-arcp0[2])<=fuzz))
						{ ic_ptcpy(i0,lnp0); fi1++; }

				ar1=((pt2[0]-arcp0[0])*(pt2[0]-arcp0[0]))+
					((pt2[1]-arcp0[1])*(pt2[1]-arcp0[1]));
				if((fabs(arcp1[0]*arcp1[0]-ar1)<=(fuzz*fuzz))&&(fabs(pt2[2]-arcp0[2])<=fuzz)){
					 if (fi1){
						 ic_ptcpy(i1,lnp1); fi1+=4;
					 }else{
						 ic_ptcpy(i0,lnp1);fi1++;
					 }
				}
			}

            if(fi1==0)return fi1;

        }

        /* Found solution(s) on the line and circle */
		if(arctyp==1){	//arc
			take0=((fi1&3)==3);  /* i0 is on both */
			take1=((fi1&12)==12);  /* i1 is on both */
		}else{	//circle
			take0=((fi1&1)==1);		 /* i0 is on line */
			take1=((fi1&4)==4);		 /* i1 is on line */
		}

        if (fuzz>0.0) {  /* Check proximity to the segs */
            if (!take0) {
                ic_segdist(i0,lnp0,lnp1,0,&ar1,NULL);
                if (ar1<=fuzz) {
                    ic_segdist(i0,arcp0,arcp1,arctyp,&ar1,NULL);
                    if (ar1<=fuzz && fi3) take0=1;
                }
            }
            if (!take1) {
                ic_segdist(i1,lnp0,lnp1,0,&ar1,NULL);
                if (ar1<=fuzz) {
                    ic_segdist(i1,arcp0,arcp1,arctyp,&ar1,NULL);
                    if (ar1<=fuzz && fi4) take1=1;
                }
            }
        }
		if(take0)sds_trans(i0,&rbent,&rbwcs,0,i0);
		if(take1)sds_trans(i1,&rbent,&rbwcs,0,i1);

        if ((fi1=take0+take1)==0 || fi1==2) {
			if (fi1) {
				numPoints = fi1;
				pXPoints = new sds_point[numPoints];
				ic_ptcpy(pXPoints[0], i0); 
				if (numPoints > 1)
					ic_ptcpy(pXPoints[1], i1); 
			}
			return fi1;
		}

        if (take1) ic_ptcpy(i0,i1);
        /* Only i1 should be taken; put it in i0. */

		numPoints = 1;
		pXPoints = new sds_point[numPoints];
		ic_ptcpy(pXPoints[0], i0); 
        return 1;

    }  /* end else line-arc or arc-line */


    return 0;  /* For safety */
}


