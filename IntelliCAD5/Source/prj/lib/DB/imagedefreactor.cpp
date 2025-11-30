/* C:\ICAD\PRJ\LIB\DB\IMAGEDEFREACTOR.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "db.h"
#include "objects.h"

/***************************** db_imagedef_reactor *****************************/

db_imagedef_reactor::db_imagedef_reactor(void) :
    db_handitem(DB_IMAGEDEF_REACTOR) {

    deleted=0;
    classver=2;  /* R14 */
    hirefll[0]=hirefll[1]=NULL;
}
db_imagedef_reactor::db_imagedef_reactor(const db_imagedef_reactor &sour) :
    db_handitem(sour) {  /* Copy constructor */

    db_hireflink *tp1,*tp2;

    deleted =sour.deleted;
    classver=sour.classver;

    hirefll[0]=hirefll[1]=NULL;
    for (tp1=sour.hirefll[0]; tp1!=NULL; tp1=tp1->next)
        { tp2=new db_hireflink(*tp1); addhiref(tp2); }

}
db_imagedef_reactor::~db_imagedef_reactor(void) {
    db_hireflink::delll(hirefll[0]);
}

/*F*/
int db_imagedef_reactor::entgetspecific(
    struct resbuf **begpp,
    struct resbuf **endpp,
    db_drawing     *dp) {
/*
**  Specific entgetter.  Returns 0 (OK) or -1 (no memory).
*/
    int rc;
    long fl1;
    sds_name en;
    struct resbuf *sublist[2];
    db_handitem *thip1;
    db_hireflink *thirp1;


    rc=0; sublist[0]=sublist[1]=NULL;

    if (begpp==NULL || endpp==NULL) goto out;

    fl1=classver;
    if ((sublist[0]=sublist[1]=db_newrb(90,'L',&fl1))==NULL)
        { rc=-1; goto out; }

    /* 330  ACAD docs indicate there can only be one, but our database */
    /* is set up for a llist of them, so I'm coding it for a llist: */
    for (thirp1=hirefll[0]; thirp1!=NULL; thirp1=thirp1->next) {
        en[0]=en[1]=0;
        if ((thip1=thirp1->ret_hip(dp))!=NULL)
            { en[0]=(long)thip1; en[1]=(long)dp; }
        if ((sublist[1]->rbnext=db_newrb(330,'N',en))==NULL) { rc=-1; goto out; }
        sublist[1]=sublist[1]->rbnext;
    }

out:
    if (begpp!=NULL && endpp!=NULL) {
        if (rc && sublist[0]!=NULL)
            { sds_relrb(sublist[0]); sublist[0]=sublist[1]=NULL; }
        *begpp=sublist[0]; *endpp=sublist[1];
    }
    return rc;
}

/*F*/
int db_imagedef_reactor::entmod(struct resbuf *modll, db_drawing *dp) {
/*
**  Entity/table-record/object  specific.  Uses the setters for range-correction.
**
**  Returns: See db_handitem::entmod().
*/
    int first330,rc;
    struct resbuf *curlink;
    db_handitem *thiship,*thip1;
    db_hireflink *thirp1;


    rc=0; first330=1; thiship=(db_handitem *)this;

    db_lasterror=0;

    if (modll==NULL || dp==NULL) { rc=-1; goto out; }

    del_xdict();

    curlink=modll;

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
            case 90: thiship->set_90(curlink->resval.rlong); break;

            case 330:

                /* ACAD docs indicate there can only be one, but our database */
                /* is set up for a llist of them, so I'm coding it for a llist: */

                if (first330) {
                    /* Free the old llist: */
                    db_hireflink::delll(hirefll[0]); hirefll[0]=hirefll[1]=NULL;
                    first330=0;
                }
                thip1=(db_handitem *)curlink->resval.rlname[0];
                thirp1=new db_hireflink(
                    DB_SOFT_POINTER,
                    (thip1!=NULL) ? thip1->RetHandle() : NULL,
                    thip1);
                addhiref(thirp1);

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



