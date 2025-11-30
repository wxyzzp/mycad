#include "icadlib.h"/*DNT*/

char *ic_nfgets(char *s, short n, FILE *stream) {
    /* An "fgets" WITHOUT \n that also checks for lines that end with 127 */

    int fi1,fi2;
    char *fcp1;

    if(s==NULL || stream==NULL) return(NULL);
    if(feof(stream)) return(NULL);

    fi1=1; fcp1=s;
    while ((fi2=fgetc(stream))!='\n'/*DNT*/ && fi2!=127 && fi1<n && !feof(stream)) {
        if(fi2!='\r'/*DNT*/) {
            *fcp1=(char)fi2; fcp1++; fi1++;
        }
    }
    *fcp1=0; return (feof(stream) && fi1<2) ? NULL : s;
}

