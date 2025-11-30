/* D:\ICADDEV\PRJ\LIB\ICADLIB\LISTDXF.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * $Revision: 1.1.1.1 $ $Date: 2000/01/19 13:41:38 $ 
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "cmds.h"
#include "icadlib.h"/*DNT*/

void ic_listdxf(struct resbuf *elist) {
    char fs1[41],fc1;
    int dxfmod100,lc;
    int lnbrk,fu1,fu2;
    struct resbuf *tb;

    lnbrk=60;

    for (tb=elist,lc=0; tb!=NULL; tb=tb->rbnext,lc++) {
        if (lc>20) {
            if (sds_getstring(0,ResourceString(IDC_LISTDXF__NPRESS_ENTER_TO_0, "\nPress ENTER to continue..." ),fs1)!=RTNORM)
                goto out;
            lc=0;
        }

        dxfmod100=tb->restype%100;
        sds_printf("\n%10d"/*DNT*/,tb->restype);
        if (tb->restype<0) {
            if (tb->restype==-1 || tb->restype==-2) sds_printf("  (0x%lX,0x%lX)"/*DNT*/,
                tb->resval.rlname[0],tb->resval.rlname[1]);
            continue;
        } else if (dxfmod100<10) {
            if (tb->restype==1004) sds_printf(ResourceString(IDC_LISTDXF___BINARY_CHUNK_3, "  Binary chunk" ));
            else {
                if ((fu1=strlen(tb->resval.rstring))<=lnbrk) {
                    sds_printf("  |%s|"/*DNT*/,tb->resval.rstring);
                } else {  /* Too long for 1 sds_printf() */
                    for (fu2=lnbrk; fu2<fu1; fu2+=lnbrk) {
                        fc1=tb->resval.rstring[fu2];
                        tb->resval.rstring[fu2]=0;
                        if (fu2>lnbrk) sds_printf("\n          "/*DNT*/);
                        sds_printf("  |%s|"/*DNT*/,
                            tb->resval.rstring+fu2-lnbrk);
                        tb->resval.rstring[fu2]=fc1;
                    }
                    sds_printf("\n            |%s|"/*DNT*/,
                        tb->resval.rstring+fu2-lnbrk);
                    lc+=fu1/lnbrk;
                }
            }
        } else if (dxfmod100<20)
            sds_printf("  (%0.15G,%0.15G,%0.15G)"/*DNT*/,tb->resval.rpoint[0],
                tb->resval.rpoint[1],tb->resval.rpoint[2]);
        else if (dxfmod100<38)
            sds_printf(ResourceString(IDC_LISTDXF___OTHER_COORDS__8, "  Other coords?" ));
        else if (dxfmod100<60)
		{
			if (tb->restype==340) {
				
			}
			else sds_printf("  %0.15G"/*DNT*/,tb->resval.rreal);
		}
        else if (dxfmod100<80) {
            if (tb->restype==1071)  sds_printf("  %ld (long)"/*DNT*/,tb->resval.rlong);
            else sds_printf("  %d"/*DNT*/,tb->resval.rint);
        } else sds_printf(ResourceString(IDC_LISTDXF___UNKNOWN_12, "  Unknown" ));
    }

out:
    IC_FREEIT
;
}



