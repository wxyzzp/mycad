#include "icadlib.h"/*DNT*/

short ic_3pt2arc(sds_point ep1, sds_point mp, sds_point ep2, sds_point cent,
    sds_real *rad, sds_real *sa, sds_real *ea) {
/*
**  Given two arc endpts (ep1 and ep2) and a pt on the arc between them (mp),
**  this function determines the center pt (cent), radius (*rad),
**  start angle (*sa), and end angle (*ea) for the arc.
**
**  Returns:
**       0 : OK
**      -1 : No solution, not a circle (treat as a line)
**      -2 : Endpts coincident with each other but not with midpt;
**             treat as a circle (cent and *rad set)
**
*/
    short fi1;
    sds_real dx[3],dy[3],ang[3],len[2];
    sds_point rline[2][2];  /* Two radial lines through chord midpts */


    /* Set Z's to same as middle pt: */
    rline[0][0][2]=rline[0][1][2]=rline[1][0][2]=rline[1][1][2]=mp[2];
    cent[2]=mp[2];

    /* Set 1st pt of each radial line to chord midpts: */
    rline[0][0][0]=(ep1[0]+mp[0])/2.0; rline[0][0][1]=(ep1[1]+mp[1])/2.0;
    rline[1][0][0]=(ep2[0]+mp[0])/2.0; rline[1][0][1]=(ep2[1]+mp[1])/2.0;

    /* For second pts, take the chord dirs and rotate 90 degrees right */
    /* (use dy for dx and -dx for dy) and add the 1st pts: */
    rline[0][1][0]=rline[0][0][0]+(mp[1]-ep1[1]);
    rline[0][1][1]=rline[0][0][1]-(mp[0]-ep1[0]);
    rline[1][1][0]=rline[1][0][0]+(ep2[1]-mp[1]);
    rline[1][1][1]=rline[1][0][1]-(ep2[0]-mp[0]);

    if (ic_linexline(rline[0][0],rline[0][1],rline[1][0],rline[1][1],cent)<0) {
        dx[0]=mp[0]-ep1[0]; dy[0]=mp[1]-ep1[1];
        len[0]=sqrt(dx[0]*dx[0]+dy[0]*dy[0]);
        dx[0]=ep2[0]-ep1[0]; dy[0]=ep2[1]-ep1[1];
        len[1]=sqrt(dx[0]*dx[0]+dy[0]*dy[0]);
        if (icadRealEqual(len[1],0.0) && !icadRealEqual(len[0],0.0)) {  /* Endpts coincident: circle */
            cent[0]=(ep1[0]+mp[0])/2.0; cent[1]=(ep1[1]+mp[1])/2.0;
            *rad=len[0]/2.0;
            return -2;
        } else return -1;  /* Treat it as a line */
    }

    /* Get angle vectors: */
    dx[0]=ep1[0]-cent[0]; dy[0]=ep1[1]-cent[1];
    dx[1]= mp[0]-cent[0]; dy[1]= mp[1]-cent[1];
    dx[2]=ep2[0]-cent[0]; dy[2]=ep2[1]-cent[1];

    *rad=sqrt(dx[1]*dx[1]+dy[1]*dy[1]);

    for (fi1=0; fi1<3; fi1++) {
        if (icadRealEqual(dx[fi1],0.0)) ang[fi1]=(dy[fi1]<0.0) ? 1.5*IC_PI : 0.5*IC_PI;
        else {
            ang[fi1]=atan(dy[fi1]/dx[fi1]);
            if (dx[fi1]<0.0) ang[fi1]+=IC_PI;
        }
    }

    ic_normang(ang,ang+2);
    while (ang[1]<ang[0]) ang[1]+=IC_TWOPI;  /* Don't norm ang[0] twice */

    if (ang[1]<ang[2]) { *sa=ang[0]; *ea=ang[2]; }
    else               { *sa=ang[2]; *ea=ang[0]; }

    ic_normang(sa,NULL); ic_normang(ea,NULL);

    return 0;
}
