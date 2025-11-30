#include "icadlib.h"/*DNT*/


short ic_ptlndist(sds_point pp, sds_point p0, sds_point p1, sds_real *dist,
    sds_point ii) {
/*
**  Sets *dist to the distance of pp from the line p0p1 and sets ii to the
**  projection of pp on p0p1.
**
**  Unneeded return pass parameters may be NULL (dist and ii).
**
**  This 3D version is a complete re-write of the old 2D one.
**  (The old version follows this function (commented out), in case we ever
**  need it.)
**
**  Returns:
**       1 : ii is on line segment
**       0 : ii is not on the line segment
**      -1 : line has coincident endpts; ii set to p0; *dist set to separation
**             of pp and p0
**      -2 : Calling error: NULL pp, p0, and/or p1.
*/
    short rc;
    int fi1;
    sds_real wdist,p0ppdist,ar1;
    sds_point p0p1u,p0pp,wii,ap1;


    rc=-2;

    if (pp==NULL || p0==NULL || p1==NULL) goto out;

    p0p1u[0]=p1[0]-p0[0]; p0p1u[1]=p1[1]-p0[1]; p0p1u[2]=p1[2]-p0[2];
     p0pp[0]=pp[0]-p0[0];  p0pp[1]=pp[1]-p0[1];  p0pp[2]=pp[2]-p0[2];

    p0ppdist=sqrt(p0pp[0]*p0pp[0]+p0pp[1]*p0pp[1]+p0pp[2]*p0pp[2]);

    if (icadRealEqual((ar1=sqrt(p0p1u[0]*p0p1u[0]+p0p1u[1]*p0p1u[1]+p0p1u[2]*p0p1u[2])),0.0)) {
        /* Line undefined; coincident points.  Return p0 in ii and the */
        /* p0pp distance: */

        wdist=p0ppdist;
        wii[0]=p0[0]; wii[1]=p0[1]; wii[2]=p0[2];
        rc=-1; goto out;
    }
    p0p1u[0]/=ar1; p0p1u[1]/=ar1; p0p1u[2]/=ar1;

    /* Get directed dist of p0pp on p0p1's direction: */
    ar1=p0pp[0]*p0p1u[0]+p0pp[1]*p0p1u[1]+p0pp[2]*p0p1u[2];

    /* Move that distance along p0p1u from p0 to get ii: */
    wii[0]=p0[0]+ar1*p0p1u[0];
    wii[1]=p0[1]+ar1*p0p1u[1];
    wii[2]=p0[2]+ar1*p0p1u[2];

    /* Calc the iipp dist: */
    ap1[0]=pp[0]-wii[0]; ap1[1]=pp[1]-wii[1]; ap1[2]=pp[2]-wii[2];
    wdist=sqrt(ap1[0]*ap1[0]+ap1[1]*ap1[1]+ap1[2]*ap1[2]);

    /* Find the p0p1u component with the greatest abs val: */
    fi1=(fabs(p0p1u[1])>fabs(p0p1u[0]));
    if (fabs(p0p1u[2])>fabs(p0p1u[fi1])) fi1=2;

    /* See if ii lies on p0p1 using the old "opposite delta signs" trick: */
    rc=((wii[fi1]-p0[fi1])*(wii[fi1]-p1[fi1])<=0.0);

out:
    if (ii!=NULL) { ii[0]=wii[0]; ii[1]=wii[1]; ii[2]=wii[2]; }
    if (dist!=NULL) *dist=wdist;
    return rc;
}



//          THE OLD (2D) ONE:
//
//  short ic_ptlndist(sds_point pp, sds_point p0, sds_point p1, sds_real *dist,
//      sds_point ii) {
//  /*
//  **  Sets *dist to the distance of pp from the line p0p1 and sets ii to the
//  **  projection of pp on p0p1.
//  **
//  **  Returns:
//  **      -1 : line has coincident endpts; ii set to p0; *dist set to separation
//  **             of pp and p0
//  **       0 : ii is not on the line segment
//  **       1 : ii is on line segment
//  **
//  */
//      sds_real dx,dy,cc,dd,sep,ar1;
//
//      ii[2]=0.0;
//
//      dx=p1[0]-p0[0]; dy=p1[1]-p0[1];
//      if ((sep=sqrt(dx*dx+dy*dy))<IC_ZRO) {
//          ii[0]=p0[0]; ii[1]=p0[1]; dx=pp[0]-p0[0]; dy=pp[1]-p0[1];
//          *dist=sqrt(dx*dx+dy*dy); return -1;
//      }
//      cc=p0[0]*dy-p0[1]*dx;
//      *dist=fabs(dd=(-dy*pp[0]+dx*pp[1]+cc)/sep);
//      ii[0]=pp[0]+dy*(ar1=dd/sep); ii[1]=pp[1]-dx*ar1;
//
//      return (fabs(dx)>fabs(dy)) ?
//          ((ii[0]-p0[0])*(ii[0]-p1[0])<=0.0) :
//          ((ii[1]-p0[1])*(ii[1]-p1[1])<=0.0);
//  }
