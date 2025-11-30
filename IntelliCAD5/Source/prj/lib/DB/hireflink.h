/* C:\ICAD\PRJ\LIB\DB\HIREFLINK.H
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

/*
**  For llists of reactors/owners.
*/
#ifndef _INC_HIREFLINK
#define _INC_HIREFLINK

class DB_CLASS db_hireflink {

  public:

    db_hireflink(void);
    db_hireflink(int ptype, db_objhandle phand, db_handitem *phip);
    db_hireflink(const db_hireflink &sour);  /* Copy constructor */
   ~db_hireflink(void);

    /*
    **  Unless you're SURE hip has been set, use this instead of
    **  directly using hip as an rvalue.  See comments at definition.
    **  (Defined later because it uses a db_drawing function that
    **  hasn't been defined yet.)
    */
    db_handitem *ret_hip(db_drawing *dp);

	db_handitem **retp_hip(void) { return &hip; }
    
	void get_data(int *typep, db_objhandle *handpp, db_handitem **hipp, db_drawing *dp) {
        if (typep!=NULL) *typep=type;
        if (handpp!=NULL) *handpp=hand;  /* DON'T FREE! */
        if (hipp!=NULL) *hipp=ret_hip(dp);
    }
    void set_data(int ptype, db_objhandle phand, db_handitem *phip) {
        type=ptype; hip=phip; hand=phand;
    }

    static void delll(db_hireflink *ll) {
        db_hireflink *p;
        while (ll!=NULL) { p=ll->next; delete ll; ll=p; }
    }

   /* Data */

    int           type;  /* A DB_[SOFT|HARD]_[POINTER|OWNER] #define. */
                         /*   (0 means undefined.) */
    db_objhandle  hand;  /* We'll probably get the handle first, during a read. */
                         /*   (Allocated for this link.) */

    db_hireflink *next;

private:

	db_handitem  *hip;   /* Eventually, we'll want a db_handitem ptr. */
                         /*   (NOT allocated for this link; just a ref ptr.) */
                         /*   DON'T USE hip DIRECTLY AS AN RVALUE. */
                         /*   SEE COMMENTS AT DEFINITION OF ret_hip(). */
};
#endif

