/* D:\ICADDEV\PRJ\ICAD\RESBUFS.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * $Revision: 1.1.1.1.10.1 $ $Date: 2003/10/18 06:01:33 $ 
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "Icad.h"
#include "threadcontroller.h" /* DNT */
#include "inputevent.h" /*DNT*/
#include "inputeventfilter.h" /*DNT*/
#include "inputeventqueue.h" /*DNT*/

struct resbuf *SDS_ResBufCopy(struct resbuf *source) {

  	struct resbuf *srbb=NULL, *srbc=NULL, *srbt;
	struct resbuf *trb;
    int fi1;

	for(trb=(struct resbuf *)source; trb!=NULL; trb=trb->rbnext) {
		srbt=sds_newrb(trb->restype);
        fi1=ic_resval(trb->restype);

        if(fi1==RTSTR) {
			srbt->resval.rstring= new char [ strlen(trb->resval.rstring)+1];
            strcpy(srbt->resval.rstring,trb->resval.rstring);
        } else if(fi1==RTBINARY) {
			srbt->resval.rbinary.buf= new char [ trb->resval.rbinary.clen];
            memcpy(srbt->resval.rbinary.buf,trb->resval.rbinary.buf,trb->resval.rbinary.clen);
			srbt->resval.rbinary.clen=trb->resval.rbinary.clen;
		} else {
            srbt->resval.rpoint[0]=trb->resval.rpoint[0];
            srbt->resval.rpoint[1]=trb->resval.rpoint[1];
            srbt->resval.rpoint[2]=trb->resval.rpoint[2];
        }

		if(srbb==NULL) {
			srbb=srbc=srbt;
		} else {
			srbc=srbc->rbnext=srbt;
		}
	}
    return(srbb);
}




