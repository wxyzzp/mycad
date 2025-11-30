#include "icadlib.h"/*DNT*/

short ic_2pta2arc(sds_point p0, sds_point p1, sds_real tandir,
    sds_point cent, sds_real *radp, sds_real *sangp, sds_real *eangp) {
/*
**  2-points-and-initial-tangent-direction to arc.
**
**  Given two pts p0 and p1 and initial tangent direction tandir from p0,
**  this function determines the arc center (cent), radius (*radp),
**  start angle (*sangp), and end angle (*eangp).
**
**  All angles are in radians.
**
**  Returns:
**       0 : OK
**      -1 : Coincident points
**      -2 : It's a line connecting p0 and p1
**      -3 : It's an "infinite radius circle"
*/
    short cw;
    sds_real cdx,cdy,adx,ady,halfchord,cs,sn,dd,midchord[2],ar1;


    cw=0;  /* Clockwise flag */

    cdx=p1[0]-p0[0]; cdy=p1[1]-p0[1];  /* Chord dx and dy */
    if (icadRealEqual((halfchord=sqrt(cdx*cdx+cdy*cdy)),0.0)) return -1;  /* WHOLE chord */
    cdx/=halfchord; cdy/=halfchord;  /* Unit vector chord dir */
    halfchord*=0.5;
    midchord[0]=0.5*(p0[0]+p1[0]); midchord[1]=0.5*(p0[1]+p1[1]);

    adx=cos(tandir); ady=sin(tandir);  /* Unit vect in tandir */

    /* cos and sin of ang between tangent vector and chord vector: */
    cs=adx*cdx+ady*cdy;
    if (cs>1.0) cs=1.0; else if (cs<-1.0) cs=-1.0;
    sn=sqrt(1.0-cs*cs);

    if (icadRealEqual(sn,0.0)) return (short)((cs>0.0) ? -2 : -3);

    /* Dist to go from midchord (left or right): */
    dd=halfchord*cs/sn;  /* May be neg; that's okay (counting on it). */

    /* Center */
    /* Check cross product to see which way to step from midchord to */
    /* get cent, and which way to draw the arc: */
    ar1=(adx*cdy-ady*cdx>0.0) ? 1.0 : -1.0;
    /* Go left if 1.0, right if -1.0: */
    cent[0]=midchord[0]-ar1*cdy*dd; cent[1]=midchord[1]+ar1*cdx*dd;
    cent[2]=p0[2];  /* Preserve Z for center. */
    if (ar1<0.0) cw=1;  /* Clockwise from p0 to p1 */

    /* Reusing cdx and cdy for other purposes beyond here. */

    cdx=p0[0]-cent[0]; cdy=p0[1]-cent[1];  /* Cent to p0 */

    /* Radius */
    *radp=sqrt(cdx*cdx+cdy*cdy);

    /* Ang from cent to p0: */
    *sangp=ic_atan2(cdy,cdx);
    if (*sangp<0.0) *sangp+=IC_TWOPI;

    /* Ang from cent to p0: */
    *eangp=ic_atan2(p1[1]-cent[1],p1[0]-cent[0]);
    if (*eangp<0.0) *eangp+=IC_TWOPI;

    /* Swap start and end angs if the arc goes CW from p0 to p1: */
    if (cw) { ar1=*sangp; *sangp=*eangp; *eangp=ar1; }

    return 0;
}
