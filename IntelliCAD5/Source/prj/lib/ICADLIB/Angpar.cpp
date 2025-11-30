
// THIS FILE HAS BEEN GLOBALIZED!

#include <icadlib.h>

void ic_angpar(sds_real sour, sds_real *destp, sds_real dist1, sds_real dist2,
    short mode) {
/*
**  Converts between ellipse angle and eccentric anomaly (ACAD's
**  "parameter", based on mode:
**
**      0 : Angle to parameter
**     !0 : Parameter to angle
**
**  Angle is specified by the vector from the center of the ellipse
**  through the point of interest, measured CCW from the semi-primary
**  axis (not necessarily the semi-major axis).
**
**  The "dist" parameters (dist1 and dist2) are the semi-primary and
**  semi-secondary axis distances.  (Angles are measured CCW from the
**  semi-primary axis vector -- which can be either the semi-major or
**  semi-minor axis.)
**
**  sour is the source value.
**
**  destp is a ptr to an sds_real for the result (and it can point
**  to sour).
*/
    sds_real cs,sn,ar1;


    if (destp==NULL) return;

    ic_normang(&sour,NULL);
    sn=sin(sour); cs=cos(sour);
    ar1=(mode) ?
        dist1*cs/sqrt(dist1*dist1*cs*cs+dist2*dist2*sn*sn) :
        dist2*cs/sqrt(dist2*dist2*cs*cs+dist1*dist1*sn*sn);
    if (ar1>1.0) ar1=1.0; else if (ar1<-1.0) ar1=-1.0;
    ar1=acos(ar1);
    *destp=(sour>IC_PI) ? IC_TWOPI-ar1 : ar1;
}

