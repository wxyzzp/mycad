
// THIS FILE HAS BEEN GLOBALIZED!

#include "icadlib.h"/*DNT*/


short ic_bulge2arc(sds_point p0, sds_point p1, sds_real bulge,
    sds_point cc, sds_real *rr, sds_real *sa, sds_real *ea) {
/*
**  Given an arc defined by two pts and bulge, determines the CCW arc's
**  center, radius, starting angle and ending angle.
**
**  Returns:
**       0 : OK
**      -1 : Points coincident
**       1 : It's a line
**		-2 : Non planar arc
*/
    short fi1;
    sds_real dx,dy,sep,ss, ara[2];

    if (icadRealEqual(bulge,0.0)) return 1;   /* Line */

	/* Points must be nearly planar */
	if(fabs(p0[2])<=IC_ZRO){
		if(fabs(p1[2])>IC_ZRO)return -2;
	}else if(fabs((p0[2]/p1[2])-1.0) > IC_ZRO)return -2;

    dx=p1[0]-p0[0]; dy=p1[1]-p0[1];
    if (icadRealEqual((sep=sqrt(dx*dx+dy*dy)),0.0)) return -1;   /* Coincident */

    *rr=fabs(sep*(bulge+1.0/bulge)/4.0);  /* Radius */

    if ((ss=(*rr)*(*rr)-sep*sep/4.0)<0.0) ss=0.0;  /* Should never */
    ss=sqrt(ss);

    /* Find center: */
    ara[0]=ss/sep;
    if (bulge<-1.0 || (bulge>0.0 && bulge<1.0)) {  /* Step left of midpt */
        cc[0]=(p0[0]+p1[0])/2.0-ara[0]*dy; cc[1]=(p0[1]+p1[1])/2.0+ara[0]*dx;
    } else {  /* Step left of midpt */
        cc[0]=(p0[0]+p1[0])/2.0+ara[0]*dy; cc[1]=(p0[1]+p1[1])/2.0-ara[0]*dx;
    }
	cc[2]=p0[2]+p1[2];
	cc[2]*=0.5;

    /* Find starting and ending angles: */
    dx=p0[0]-cc[0]; dy=p0[1]-cc[1]; ara[0]=ic_atan2(dy,dx);  /* Avoid METAWARE bug */
    dx=p1[0]-cc[0]; dy=p1[1]-cc[1]; ara[1]=ic_atan2(dy,dx);

    /* If bulge>=0.0, take starting angle from p0: */
    *sa=ara[fi1=(bulge<0.0)]; *ea=ara[!fi1];

    /* Make both 0.0<=ang<IC_TWOPI : */
    if (*sa<0.0) *sa+=IC_TWOPI;
    if (*ea<0.0) *ea+=IC_TWOPI;

    return 0;
}

