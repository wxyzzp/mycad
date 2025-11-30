#include "icadlib.h"/*DNT*/

void ic_makebak(char *pn) {
    char fs1[IC_PATHSIZE+IC_FILESIZE];

    strcpy(fs1,pn); ic_setext(fs1,IC_BAKEXT);
    unlink(fs1); rename(pn,fs1); 
}
