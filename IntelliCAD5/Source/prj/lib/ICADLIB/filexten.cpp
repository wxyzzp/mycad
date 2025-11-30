
// THIS FILE HAS BEEN GLOBALIZED!

/***************************************************************************
 ***************************************************************************/

#include "icadlib.h"/*DNT*/
#include <tchar.h>

short ic_chkext(char *pathname, char *extension) {
/*
**  Checks to see if pathname has extension "extension".
**
**  Returns:
**      0 : pathname has the extension.
**     -1 : pathname does NOT have the extension (or calling error).
**
*/
    char fs1[11],fs2[11],*cptr1,*cptr2;
	short ret;

    if(pathname==NULL || extension==NULL) return(-1);
    ic_getext(pathname,fs1); strncpy(fs2,extension,sizeof(fs2)-1);
    ic_trim(fs2,"bme"/*DNT*/);
// the original version
//    return -(stricmp(fs1+(*fs1=='.'/*DNT*/),fs2+(*fs2=='.'/*DNT*/))!=0);
	cptr1=fs1;
	cptr2=fs2;
	if (*cptr1=='.'/*DNT*/) cptr1++;
	if (*cptr2=='.'/*DNT*/) cptr2++;
	ret=strisame(cptr1,cptr2);
	if (ret) return(0);
	return(-1);
}	

void ic_getext(char *pathname, char *result) {
/*
**  Checks pathname for an extension and sets "result" to the extension.
**  (Make sure "result" can hold at least 10 chars, for safety.)
**
//  NOTE: The dot is ALWAYS included ("." for no extension).  The caller
**  can use result+1 if it doesn't want the dot.
*/
    char *fcp1,*fcp2;

    if(result==NULL) return;
    *result='.'/*DNT*/; result[1]=0;
    if(pathname==NULL) return;
    fcp1=strrchr(pathname,'.'/*DNT*/); fcp2=strrchr(pathname,IC_SLASH);
    if(fcp1!=NULL && (fcp2==NULL || fcp1>fcp2))
        { strncpy(result,fcp1,10); ic_trim(result,"bme"/*DNT*/); }
}
