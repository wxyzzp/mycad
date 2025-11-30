/* C:\ICAD\PRJ\LIB\DB\IDBUFFER.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "db.h"
#include "objects.h"

/***************************** db_idbuffer *****************************/

db_idbuffer::db_idbuffer(void) : db_handitem(DB_IDBUFFER) {
    deleted=0; hirefll[0]=hirefll[1]=NULL;
}
db_idbuffer::db_idbuffer(const db_idbuffer &sour) :
    db_handitem(sour) {  /* Copy constructor */

    db_hireflink *tp1,*tp2;

    deleted  =sour.deleted;

    hirefll[0]=hirefll[1]=NULL;
    for (tp1=sour.hirefll[0]; tp1!=NULL; tp1=tp1->next)
        { tp2=new db_hireflink(*tp1); addhiref(tp2); }

}
db_idbuffer::~db_idbuffer(void) { db_hireflink::delll(hirefll[0]); }

// EBATECH(CNBR) -[ 2002/10/10 Bugzilla78218
int db_idbuffer::entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp)
{
/*
**  Specific entgetter.  Returns 0 (OK) or -1 (no memory).
*/
    int rc;
    struct resbuf *sublist[2];
    sds_name en;
    db_handitem *thip1;
    db_hireflink *refp1;

    rc=0; sublist[0]=sublist[1]=NULL;

    if (begpp==NULL || endpp==NULL) goto out;

    /* 330 Object Reference(s) */
	for( refp1 = hirefll[0] ; refp1 != NULL ; refp1 = refp1->next ) {
		refp1->get_data(NULL, NULL, &thip1, dp);
		en[0]=(long)thip1; en[1]=(long)dp;
		if( sublist[0] != NULL ) {
        	if ((sublist[1]->rbnext=db_newrb(330,'N',en))==NULL) {
        		rc=-1; goto out;
        	}
	        sublist[1]=sublist[1]->rbnext;
        } else {
        	if ((sublist[0] = sublist[1] = db_newrb(330,'N',en)) == NULL ) {
				rc=-1; goto out;
			}
        }
	}

out:
    if (begpp!=NULL && endpp!=NULL) {
        if (rc && sublist[0]!=NULL)
            { sds_relrb(sublist[0]); sublist[0]=sublist[1]=NULL; }
        *begpp=sublist[0]; *endpp=sublist[1];
    }
    return rc;
}
// EBATECH(CNBR) ]-

/*F*/
int db_idbuffer::entmod(struct resbuf *modll, db_drawing *dp) {
/*
**  Entity/table-record/object specific.  Uses the setters for range-correction.
**
**  Returns: See db_handitem::entmod().
*/
// modified from db_dictionary::entmod()
    
	int rc;
    struct resbuf *curlink;
    db_handitem *thiship,*thip1;
    db_hireflink *thiref;


    rc=0; thiship=(db_handitem *)this;

    db_lasterror=0;

    if (modll==NULL || dp==NULL) { rc=-1; goto out; }

    del_xdict();

    curlink=modll;

    /* Free the old llist of records (so that the caller can delete */
    /* ALL records by not supplying any in modll): */
    delhirefll();

    /*
    **  If modll has been built properly, the first few groups
    **  are things we don't allow to be modified (ename, type, handle),
    **  so let's walk over these right now:
    */
    while (curlink!=NULL &&
          (curlink->restype==-1 ||
           curlink->restype== 0 ||
           curlink->restype== 5)) curlink=curlink->rbnext;

    /* Walk; stop at the end or the EED sentinel (-3): */
    for (; !rc && curlink!=NULL && curlink->restype!=-3; curlink=curlink->rbnext) {
        /* Call the correct setter: */
        switch (curlink->restype) {
            case   330:
				/* Make the new link and add it on: */
				thip1=(db_handitem *)curlink->resval.rlname[0];
                thiref=new db_hireflink(
                    DB_SOFT_POINTER,
                    (thip1!=NULL) ? thip1->RetHandle() : NULL,
                    thip1);
                addhiref(thiref);
                break;

            default:  /* In the case of an OBJECT, this virtual function */
                      /* calls entmodhi(). */
                rc=thiship->entmod1common(&curlink,dp);
                break;
        }
    }

    if (db_lasterror) { rc=-6; goto out; }

    if (!rc && curlink!=NULL) rc=set_eed(curlink,dp);

out:

    return rc;
}



