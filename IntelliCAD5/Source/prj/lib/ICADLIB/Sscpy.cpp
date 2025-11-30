#include "icadlib.h"/*DNT*/

long ic_sscpy(sds_name ss2, sds_name ss1) {
//*** Copies entities in from existing selection set(ss1) to a new 
//*** selection set(ss2).
//*** 
//*** Returns: Number of items in new selection set upon success;
//***          -1 if an error was returned from sds_ssadd.
//***
    sds_name ename;
    long ssct;

    for(sds_ssadd(NULL,NULL,ss2),ssct=0;sds_ssname(ss1,ssct,ename)==RTNORM;ssct++) {
        if(sds_ssadd(ename,ss2,ss2)!=RTNORM) return(-1);
    }
    return(ssct);
}
