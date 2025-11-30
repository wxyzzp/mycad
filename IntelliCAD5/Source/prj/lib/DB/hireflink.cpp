/* C:\ICAD\PRJ\LIB\DB\HIREFLINK.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "db.h"

/*************************** db_hireflink ****************************/

db_hireflink::db_hireflink(void) {
    memset(this,0,sizeof(*this));
}
db_hireflink::db_hireflink(int ptype, db_objhandle phand, db_handitem *phip) {
    memset(this,0,sizeof(*this));
    set_data(ptype,phand,phip);
}
db_hireflink::db_hireflink(const db_hireflink &sour) {  /* Copy constructor */
	memset(this,0,sizeof(*this));
    type=sour.type; hip=sour.hip;
    hand.Nullify(); hand=sour.hand;
}
db_hireflink::~db_hireflink(void) {
}


/*
**  This function is to be used whenever we need to GET
**  hip AFTER THE DRAWING HAS BEEN OPENED.  When we open a drawing,
**  we read handles only, because the database isn't complete.
**  So, later, when we're ready to use the hips, we have to make
**  sure they've been set from their handles.  Unless you're SURE
**  that hip has been set, use this function instead of using hip directly
**  to avoid getting a NULL when you shouldn't get one.
*/
/*F*/
inline db_handitem *db_hireflink::ret_hip(db_drawing *dp) 
{
	if (hip == NULL && dp != NULL)
		hip = dp->handent(hand);

	return hip;

// old code	return (hip==NULL && dp!=NULL) ? (hip=dp->handent(hand)) : hip;
}
