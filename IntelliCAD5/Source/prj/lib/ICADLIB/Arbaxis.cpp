
// THIS FILE HAS BEEN GLOBALIZED!

#include "icadlib.h"/*DNT*/

#define EXTRU_FUZZ 1.0E-7

short ic_arbaxis(sds_point extrudir, sds_point ecsx, sds_point ecsy, sds_point ecsz) {
/*
**  Implementation of the ACAD Arbitrary Axis Algorithm for finding
**  ECS axes given an extrusion direction.  Call this function with ecsz set
**  to the extrusion direction.  This function sets ecsz to unit length
**  and sets ecsx and ecsy to the unit ECS X- and Y-axis directions.
**
**  Returns:
**       0 : OK
**      -1 : ecsz is the 0-vector; can't use that
*/
    sds_real cap,ar1;
	
	cap=0.015625;  /* 1/64; square polar cap. */

	memcpy(ecsz,extrudir,3*sizeof(double));

    ar1=sqrt(ecsz[0]*ecsz[0]+ecsz[1]*ecsz[1]+ecsz[2]*ecsz[2]);
    if (icadRealEqual(ar1,0.0)) return -1;
    ecsz[0]/=ar1; ecsz[1]/=ar1; ecsz[2]/=ar1;


// check for closeness to 0,0,1 and set to world if within fuzz
	if (fabs(ecsz[0])<EXTRU_FUZZ && fabs(ecsz[1])<EXTRU_FUZZ && fabs(ecsz[2]-1.0)<EXTRU_FUZZ) {
		ecsx[0]=ecsy[1]=ecsz[2]=1.0;    // set to 1,0,0,  0,1,0,  and 0,0,1
		ecsx[1]=ecsx[2]=ecsy[0]=ecsy[2]=ecsz[0]=ecsz[1]=0.0;
		return 0;
	}

    if (fabs(ecsz[0])<cap && fabs(ecsz[1])<cap) {  /* ECSX = WCSY X ECSZ */
        ecsx[0]=ecsz[2]; ecsx[1]=0.0; ecsx[2]=-ecsz[0];
    } else {  /* ECSX = WCSZ X ECSZ */
        ecsx[0]=-ecsz[1]; ecsx[1]=ecsz[0]; ecsx[2]=0.0;
    }

    ar1=sqrt(ecsx[0]*ecsx[0]+ecsx[1]*ecsx[1]+ecsx[2]*ecsx[2]);
    if (icadRealEqual(ar1,0.0)) return -1;  /* No way */
    ecsx[0]/=ar1; ecsx[1]/=ar1; ecsx[2]/=ar1;

//	ecsy[0]=ecsz[1]*ecsx[2]-ecsz[2]*ecsx[1];
//	ecsy[1]=ecsz[2]*ecsx[0]-ecsz[0]*ecsx[2];
//	ecsy[2]=ecsz[0]*ecsx[1]-ecsz[1]*ecsx[0];
	ic_crossproduct(ecsz,ecsx,ecsy);

    return 0;
}
