#include "icadlib.h"/*DNT*/

void ic_lpad(char *ss, short len) { /* pad beginning with spaces to length len */
    int fi1,fi2;

    fi1=strlen(ss);
    if ((fi2=len-fi1)<1) return;
    do ss[fi1+fi2]=ss[fi1]; while (--fi1>=0);
    for (fi1=0; fi1<fi2; fi1++) ss[fi1]=' '/*DNT*/;
}

