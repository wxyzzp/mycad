
// THIS FILE HAS BEEN GLOBALIZED!

#include "icadlib.h"/*DNT*/

void ic_dotstr(char *ss, short len, LPCTSTR be) {
    /* begin or end str with "..." if str is too long.  (Using string
         for "be" to be consistent with ic_trim().) */

    int fi1;
	char *source;

    if ((fi1=strlen(ss))<=len || len<4) return;
    if (toupper(be[0])=='B'/*DNT*/) {
        ss[0]=ss[1]=ss[2]='.'/*DNT*/;
		if (fi1>len) {
			source=ss+3+fi1-len;
			memmove(ss+3,source,strlen(source)+1);
		}
    } else {
        ss[len-3]=ss[len-2]=ss[len-1]='.'/*DNT*/; ss[len]=0;
    }
}
