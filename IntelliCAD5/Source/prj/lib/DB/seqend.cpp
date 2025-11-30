/* C:\ICAD\PRJ\LIB\DB\SEQEND.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "db.h"

/**************************** db_seqend *****************************/

/*F*/
db_seqend::db_seqend(void) : db_entity(DB_SEQEND) {

    mainhip=NULL;
}
db_seqend::db_seqend(db_drawing *dp) : db_entity(DB_SEQEND,dp) 
	{

	if ( dp != NULL )
		{
		mainhip = dp->ret_complexmain();
		}
	else
		{
		mainhip = NULL;
		}
	}


db_seqend::db_seqend(const db_seqend &sour) :
    db_entity(sour) {  /* Copy constructor */

    mainhip=NULL;  /* Don't copy this one. */
}

/*F*/
int db_seqend::entgetspecific(
    struct resbuf **begpp,
    struct resbuf **endpp,
    db_drawing     *dp) {
/*
**  Specific entgetter.  Returns 0 (OK) or -1 (no memory).
*/
    int rc;
    sds_name en;
    struct resbuf *sublist[2];


    rc=0; sublist[0]=sublist[1]=NULL;

    if (begpp==NULL || endpp==NULL) goto out;

    /* -2 (mainhip) */
    en[0]=(long)mainhip; en[1]=(long)dp;
    if ((sublist[0]=sublist[1]=db_newrb(-2,'N',en))==NULL) { rc=-1; goto out; }

out:
    if (begpp!=NULL && endpp!=NULL) {
        if (rc && sublist[0]!=NULL)
            { sds_relrb(sublist[0]); sublist[0]=sublist[1]=NULL; }
        *begpp=sublist[0]; *endpp=sublist[1];
    }
    return rc;
}


