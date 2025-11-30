#include "icadlib.h"/*DNT*/

void ic_setext(char *pn, char *ext) {
/*
**  Sets pathname pn's extension to ext (whether or not ext includes the
//  dot as the first character -- ".abc" and "abc" are both OK).
**
**  Note that ext can be "" to delete an extension.
**
**  THE CALLER MUST MAKE SURE pn POINTS TO A STRING ALLOCATED BIG ENOUGH.
*/
    char *fcp1,*fcp2;

    fcp1=strrchr(pn,'.'/*DNT*/); fcp2=strrchr(pn,IC_SLASH);
    if (fcp1==NULL || (fcp2!=NULL && fcp2>fcp1))
        { fcp1=pn; while (*fcp1) fcp1++; }
    if (*ext) {
        if (*ext!='.'/*DNT*/) *fcp1++='.'/*DNT*/;
        while (*ext) *fcp1++=*ext++;
    }
    *fcp1=0;
}


int
ic_setpath( char *filename, char *pathname )
	{
	int iRetval = 0;
   char drive[_MAX_DRIVE];   char dir[_MAX_DIR];
   char fname[_MAX_FNAME];   char ext[_MAX_EXT];

   if ( ( filename != NULL ) &&
	    ( pathname != NULL ) )
	   {
	   _splitpath( pathname, drive, dir, NULL, NULL );

	   _splitpath( filename, NULL, NULL, fname, ext );
	   if ( strlen( dir ) > 0 )
		   {
		   iRetval = 1;
		   _makepath( filename, drive, dir, fname, ext );
		   }
	   }
	return iRetval;
	}
