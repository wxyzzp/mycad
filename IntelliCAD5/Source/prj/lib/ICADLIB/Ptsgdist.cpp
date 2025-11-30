#include "icadlib.h"/*DNT*/


short ic_ptsegdist(sds_point pp, sds_point p0, sds_point p1, sds_real bulge,
    sds_real *dist) {
/*
**  Determines the distance of point pp from the POLYLINE segment specified
**  by p0, p1, and bulge.  For a line segment, the perpendicular distance
**  is used -- unless the projection of pp on the line does not line
**  on the segment.  For an arc, the radial distance of pp from the arc
**  is used -- unless the line from the center through pp defines an
**  angle not within the arc's defined angle.  In both cases that fail,
**  the distance to the nearest endpoint is used.
**
**  *dist is set to the distance.
**
**  Returns:
**       0 : OK; pt projects onto arc or line seg.
**      +1 : OK; pt does not project onto arc or line seg.
*/
    short fi1,rc;
    sds_real rr,sa,ea,dx0,dx1,dy0,dy1,pang,ar1,ar2;
    sds_point cc,ap1;

    rc=0;

    if (icadRealEqual(bulge,0.0)) {  /* Line */
        doline:

        if (ic_ptlndist(pp,p0,p1,&ar1,ap1)==1) { *dist=ar1; goto out; }

        /* IF WE GET TO HERE, FALL THROUGH TO THE ENDPT CHECK */

    } else {  /* Arc */
        if ((fi1=ic_bulge2arc(p0,p1,bulge,cc,&rr,&sa,&ea))!=-1) {
            if (fi1==1) goto doline;  /* Shouldn't happen, but... */
            dx0=pp[0]-cc[0]; dy0=pp[1]-cc[1];
            ar1=sqrt(dx0*dx0+dy0*dy0);  /* Dist from center */
            if (icadRealEqual(ar1,0.0)) { *dist=rr; goto out; }
            pang=ic_atan2(dy0,dx0);
            ic_normang(&sa,&ea); while (pang<sa) pang+=IC_TWOPI;
            if (pang<=ea) {  /* pp is within the subtended angle */
                *dist=fabs(ar1-rr); goto out;
            }
        }

        /* IF WE GET TO HERE, FALL THROUGH TO THE ENDPT CHECK */

    }

    /* If we get to here, pp must have been outside the arc's */
    /* angle range, or the projection on the line must have been */
    /* off the segment, or p0 and p1 are coincident.  Therefore, */
    /* use the endpoints: */

    dx0=pp[0]-p0[0]; dy0=pp[1]-p0[1]; ar1=sqrt(dx0*dx0+dy0*dy0);
    dx1=pp[0]-p1[0]; dy1=pp[1]-p1[1]; ar2=sqrt(dx1*dx1+dy1*dy1);
    *dist=(ar1<ar2) ? ar1 : ar2;

    rc=1;

out:
    return rc;
}
