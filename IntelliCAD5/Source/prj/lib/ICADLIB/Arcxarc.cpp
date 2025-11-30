
// THIS FILE HAS BEEN GLOBALIZED!

#include "icadlib.h"/*DNT*/

short ic_arcxarc(sds_point c0, sds_real r0, sds_real start0, sds_real end0,
    sds_point c1, sds_real r1, sds_real start1, sds_real end1,
    sds_point i0, sds_point i1) {
/*
**  Determines intersections i0 and i1 of circle0 with circle1.
**
**  Returns:
**      -1 : No intersections.  (Circles too far from each other or one
**             is inside the other -- or one or both circles are undefined
**             (0 radius).)
**     >-1 : Bit-coded (bits counted from right):
**             Bit 0 : i0 is on arc0
**             Bit 1 : i0 is on arc1
**             Bit 2 : i1 is on arc0
**             Bit 3 : i1 is on arc1
*/

    short i0ona0,i0ona1,i1ona0,i1ona1,rc;
    sds_real cs,dx,dy,i0ang,i1ang,sep,ar1,ar2;

/*BUGBEGIN 	#ITC_56420 	(LINE 25)
    i0[2]=i1[2]=0.0; rc=0;
BUGEND   	#ITC_56420
*/
//#ITC_56420 FIXED
//-----------------------------------------------------------------------------
//WHY & HOW:     i0[2]=i1[2] is Z - coordinate (elevation for arc or circle) 
//	    and initialization value for it no modify on exit. In order to make 
//	    the value correctly there is need to get elevation of arc/circle #0 or #1
//-----------------------------------------------------------------------------		    
//FIX	(instead of line 25): 
    i0[2]=i1[2]=c1[2]; rc=0;
//END of FIXED 	#ITC_56420    

    if (r0<0.0) r0=-r0;
    if (r1<0.0) r1=-r1;

    dx=c1[0]-c0[0]; dy=c1[1]-c0[1]; sep=sqrt(dx*dx+dy*dy);

    if (r0<IC_ZRO || r1<IC_ZRO || r0+r1<sep-IC_ZRO || fabs(r0-r1)>sep+IC_ZRO || sep<IC_ZRO)
        return -1;

    ar1=r0+sep;

    if (fabs(cs=((ar1+r1)*(ar1-r1))/(2.0*sep*r0)-1.0)>1.0+IC_ZRO) return -1;
    cs*=r0;  /* Not a cosine any more */
    ar1=cs/sep;
    ar2=r0*r0-cs*cs; 
	ar2=(ar2<0.0) ? 0.0 : sqrt(ar2); 
	ar2/=sep;
    i0[0]=c0[0]+ar1*dx-ar2*dy; i1[0]=c0[0]+ar1*dx+ar2*dy;
    i0[1]=c0[1]+ar1*dy+ar2*dx; i1[1]=c0[1]+ar1*dy-ar2*dx;


/*
//  SEE linexarc() FOR REASON THIS IS NOT SAFE.  THE FIX IS BELOW.
//
//  ** See if i0 and i1 are on arc0: **
//  ic_normang(&start0,&end0);
//  i0ang=atan2(i0[1]-c0[1],i0[0]-c0[0]);
//  ic_normang(&start0,&i0ang);  ** Makes sure i0ang>=start0 **
//  i1ang=atan2(i1[1]-c0[1],i1[0]-c0[0]);
//  ic_normang(&start0,&i1ang);
//  i0ona0=(i0ang<=end0);
//  i1ona0=(i1ang<=end0);
//
//  ** See if i0 and i1 are on arc1: **
//  ic_normang(&start1,&end1);
//  i0ang=atan2(i0[1]-c1[1],i0[0]-c1[0]);
//  ic_normang(&start1,&i0ang);  ** Makes sure i0ang>=start1 **
//  i1ang=atan2(i1[1]-c1[1],i1[0]-c1[0]);
//  ic_normang(&start1,&i1ang);
//  i0ona1=(i0ang<=end1);
//  i1ona1=(i1ang<=end1);
*/


    /* See if i0 and i1 are on arc0: */
    ic_normang(&start0,&end0);
    ar1=i0[1]-c0[1]; ar2=i0[0]-c0[0];
    i0ang=ic_atan2(ar1,ar2);
    ic_normang(&start0,&i0ang);  /* Makes sure i0ang>=start0 */
    ar1=i1[1]-c0[1]; ar2=i1[0]-c0[0];
    i1ang=ic_atan2(ar1,ar2);
    ic_normang(&start0,&i1ang);
    i0ona0=(i0ang>=start0 && i0ang<=end0);
    i1ona0=(i1ang>=start0 && i1ang<=end0);

    /* See if i0 and i1 are on arc1: */
    ic_normang(&start1,&end1);
    ar1=i0[1]-c1[1]; ar2=i0[0]-c1[0];
    i0ang=ic_atan2(ar1,ar2);
    ic_normang(&start1,&i0ang);  /* Makes sure i0ang>=start1 */
    ar1=i1[1]-c1[1]; ar2=i1[0]-c1[0];
    i1ang=ic_atan2(ar1,ar2);
    ic_normang(&start1,&i1ang);
    i0ona1=(i0ang>=start1 && i0ang<=end1);
    i1ona1=(i1ang>=start1 && i1ang<=end1);


    rc= i0ona0;
    if (i0ona1) rc|=2;
    if (i1ona0) rc|=4;
    if (i1ona1) rc|=8;

    return rc;
}
