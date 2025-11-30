#include "icadlib.h"/*DNT*/

void ic_trim(char *ss, char *bem) {
/* Removes spaces from the beginning and/or end and/or middle of strings */
    char fc1,*fcp1,*fcp2,*fcp3,*bemptr;

    for (fcp1=ss; *fcp1==' '/*DNT*/; fcp1++);
    if (!(*fcp1)) { *ss=0; return; }  /* Null or ALL blanks */

    /* Now there's at least one non-space */

    for (bemptr=bem; *bemptr; bemptr++) {
        if (*ss==' ' && (*bemptr=='b'/*DNT*/ || *bemptr=='B'/*DNT*/)) {
            for (fcp2=ss; *fcp2==' '/*DNT*/; fcp2++);
            for (fcp1=ss; *fcp2; fcp1++,fcp2++) *fcp1=*fcp2;
            *fcp1=0;
        } else if (*bemptr=='e'/*DNT*/ || *bemptr=='E'/*DNT*/) {
            for (fcp1=ss; *fcp1; fcp1++);
            while (*(--fcp1)==' '/*DNT*/); *(fcp1+1)=0;
        } else if (*bemptr=='m'/*DNT*/ || *bemptr=='M'/*DNT*/) {
            /* Just compact between 1st & last non-space: */
            for (fcp1=ss; *fcp1==' '/*DNT*/; fcp1++); /* 1st non-space */
            for (fcp3=fcp1; *fcp3; fcp3++);
            while (*(--fcp3)==' '/*DNT*/); /* fcp3 now at last non-space */
            fc1=*(fcp3+1); *(fcp3+1)=0; /* terminate string temporarily */
            for (fcp2=fcp3=fcp1; *fcp3; fcp3++) { /* compact */
                if (*fcp3!=' '/*DNT*/) { *fcp2=*fcp3; fcp2++; }
            }
            *fcp3=fc1; /* put the blasted char back */
            for (; *fcp3; fcp2++,fcp3++)  *fcp2=*fcp3; /* Just copy the rest */
            *fcp2=0;
        }
    }
}
