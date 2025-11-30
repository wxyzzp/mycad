/* C:\DEV\LIB\ICADLIB\SEGDIST.C
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "icadlib.h"/*DNT*/


short ic_segdist(sds_point pp, sds_point p0, sds_point p1, short type,
    sds_real *dist, sds_point nearpt) {
/*
**
**  DON'T CONFUSE THIS FUNCTION WITH ic_ptsegdist(); THAT FUNCTION
**  WORKS WITH POLYLINE SEGMENTS AND DOES SOMETHING SIMILAR.
**
**  Determines the distance of point pp from the line or arc segment
**  specified by p0 and p1, and sets nearpt to the nearest point on the
**  segment.
**
**  Variable "type" determines whether the segment is a line (0),
**  an arc (1), or a circle (2).
**
**  FOR A LINE (type==0):
**
**      p0 and p1 are the endpoints of the segment.
**
**      The projection of pp on the line is taken -- unless it's
**      not on the line segment (in which case the nearest endpoint is
**      taken).
**
**  FOR AN ARC (type==1) (defined CCW):
**
**      p0 is the center.
**      p1 is interpreted as follows:
**          p1[0] : radius
**          p1[1] : start angle (radians)
**          p1[2] : end   angle (radians)
**
**      The radial projection of pp on the arc segment is taken --
**      unless the line from the center through pp defines an angle not
**      within the arc's defined angle (in which case the nearest
**      endpoint is used).
**
**      *dist is set to the distance (if not NULL).
**      nearpt is set to the nearest point on the line segment (if not NULL).
**
**	FOR A CIRCLE (type==2) 
**
**		Same as an arc, except that the point always falls within the
**		subtended arc.  *dist is set to the distance and nearpt to the
**		nearest point on the line segment (if not NULL).		
**
**  Returns:
**       0 : OK (Can always return something meaningful.)
*/
    sds_real dx0,dx1,dy0,dy1,dz0,dz1,sa,ea,pang,ar1,ar2;
    sds_point spt,ept,ap1;


    if (type==0) {  /* Line seg */

        /* If projection is on the segment; take the dist: */
        if (ic_ptlndist(pp,p0,p1,&ar1,ap1)==1) {
            if (dist!=NULL) *dist=ar1;
            if (nearpt!=NULL) ic_ptcpy(nearpt,ap1);
        } else {  /* Use the endpoints: */
            dx0=pp[0]-p0[0]; dy0=pp[1]-p0[1]; dz0=pp[2]-p0[2]; ar1=sqrt(dx0*dx0+dy0*dy0+dz0*dz0);
            dx1=pp[0]-p1[0]; dy1=pp[1]-p1[1]; dz1=pp[2]-p1[2]; ar2=sqrt(dx1*dx1+dy1*dy1+dz1*dz1);
            if (ar1<ar2) {
                if (dist!=NULL) *dist=ar1;
                if (nearpt!=NULL) ic_ptcpy(nearpt,p0);
            } else {
                if (dist!=NULL) *dist=ar2;
                if (nearpt!=NULL) ic_ptcpy(nearpt,p1);
            }
        }

    } else {  /* Arc seg or Circle */
		//near point for arc is 2d!!!
		//set z to arc's z at bottom of f'n

        sa=p1[1]; ea=p1[2];  /* Don't let ic_normang() change p1 */

        dx0=pp[0]-p0[0]; dy0=pp[1]-p0[1];
        ar1=sqrt(dx0*dx0+dy0*dy0);  /* Dist from center */

		if(type==1){	/*Arc seg*/
			spt[0]=p0[0]+p1[0]*cos(sa); spt[1]=p0[1]+p1[0]*sin(sa);
			ept[0]=p0[0]+p1[0]*cos(ea); ept[1]=p0[1]+p1[0]*sin(ea);
		}else{			/*Circle*/
			//just set start and end to 3o'clock position
			ic_ptcpy(spt,p0);
			spt[0]+=p1[0];
			ic_ptcpy(ept,p0);	//added to remove compiler warning - AM
		}
		

        if (icadRealEqual(ar1,0.0)) {  /* pp is at the center */
            if (dist!=NULL) *dist=p1[0];  /* The radius */
            if (nearpt!=NULL) ic_ptcpy(nearpt,spt);  /* Use start pt */
        } else {
            pang=ic_atan2(dy0,dx0); ic_normang(&sa,&ea);
            ic_normang(&pang,NULL); while (pang<sa) pang+=IC_TWOPI;

            if ((pang<ea)||(type==2)) {  /* pp within the subtended angle */
                if (dist!=NULL) *dist=fabs(p1[0]-ar1);
                if (nearpt!=NULL) {
                    nearpt[0]=p0[0]+p1[0]*dx0/ar1;
                    nearpt[1]=p0[1]+p1[0]*dy0/ar1;
                }
            } else { /* pp is outside the arc's angle range; use endpoints: */
                dx0=pp[0]-spt[0]; dy0=pp[1]-spt[1]; ar1=sqrt(dx0*dx0+dy0*dy0);
                dx1=pp[0]-ept[0]; dy1=pp[1]-ept[1]; ar2=sqrt(dx1*dx1+dy1*dy1);
                if (ar1<ar2) {
                    if (dist!=NULL) *dist=ar1;
                    if (nearpt!=NULL) ic_ptcpy(nearpt,spt);
                } else {
                    if (dist!=NULL) *dist=ar2;
                    if (nearpt!=NULL) ic_ptcpy(nearpt,ept);
                }
            }
        }

    }  /* End else arc seg */

    if (nearpt!=NULL && 0!=type) nearpt[2]=p0[2];  /* Preserve 2D line elevation */

    return 0;
}


