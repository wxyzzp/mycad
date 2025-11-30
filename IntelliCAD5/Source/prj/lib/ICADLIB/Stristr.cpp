#include "icadlib.h"/*DNT*/

LPCTSTR ic_stristr( LPCTSTR ss1, LPCTSTR ss2) {
    LPCTSTR fcp1;
    size_t len1,len2,lastchk,fi1,fi2;

    if ((len1=strlen(ss1))<(len2=strlen(ss2))) return NULL;
    if (!len2) return ss1+len1;

    for (fi1=0,lastchk=len1-len2; fi1<=lastchk; fi1++) {
        if (toupper(ss1[fi1])!=toupper(*ss2)) continue;
        for (fcp1=ss1+fi1,fi2=0; fi2<len2 && toupper(fcp1[fi2])==
            toupper(ss2[fi2]); fi2++);
        if (fi2>=len2) return fcp1;
    }

    return NULL;
}

