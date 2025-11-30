
// THIS FILE HAS BEEN GLOBALIZED!

#include "icadlib.h"/*DNT*/

void ic_arcextent(sds_point cc, sds_real rr, sds_real sa, sds_real ea,
    sds_point ll, sds_point ur) {
/*
**  Given an arc with center cc of radius rr with starting and ending
**  angles sa and ea (CCW), determines the lower left (ll) and upper right
**  (ur) corners of the arc's extents box.
*/
    short bigx,bigy;
    sds_real ar1;
    sds_point apa[2];

    ar1=0.0;

    if (rr<0.0) rr=-rr;

    ic_normang(&sa,&ea);
    apa[0][0]=cc[0]+rr*cos(sa); apa[0][1]=cc[1]+rr*sin(sa);
    apa[1][0]=cc[0]+rr*cos(ea); apa[1][1]=cc[1]+rr*sin(ea);
    bigx=(apa[1][0]>apa[0][0]); bigy=(apa[1][1]>apa[0][1]);

    if (ea>IC_TWOPI) ur[0]=cc[0]+rr; else ur[0]=apa[bigx][0];

    if ((ar1=IC_PI/2.0)<sa) ar1+=IC_TWOPI;
    if (ea>ar1) ur[1]=cc[1]+rr; else ur[1]=apa[bigy][1];

    if ((ar1=IC_PI)<sa) ar1+=IC_TWOPI;
    if (ea>ar1) ll[0]=cc[0]-rr; else ll[0]=apa[!bigx][0];

    if ((ar1=1.5*IC_PI)<sa) ar1+=IC_TWOPI;
    if (ea>ar1) ll[1]=cc[1]-rr; else ll[1]=apa[!bigy][1];
}
