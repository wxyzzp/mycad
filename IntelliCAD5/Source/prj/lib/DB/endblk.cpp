/* C:\ICAD\PRJ\LIB\DB\ENDBLK.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "db.h"

/**************************** db_endblk *****************************/

/*F*/
db_endblk::db_endblk(void) : db_entity(DB_ENDBLK) {
}
db_endblk::db_endblk(db_drawing *dp) : db_entity(DB_ENDBLK,dp) {
}
db_endblk::db_endblk(const db_endblk &sour) :
    db_entity(sour) {  /* Copy constructor */

    /* No members to copy, but weird unresolved externals occur */
    /* if we omit this function and try the default copy constructor. */
}

/*F*/
int db_endblk::entgetspecific(
    struct resbuf **begpp,
    struct resbuf **endpp,
    db_drawing     *dp) {
/*
**  Specific entgetter.  Returns 0 (OK) or -1 (no memory).
**  (This one's empty.  ENDBLK has no entity-specific groups.
**  This function keeps the virtual function from wasting its time.)
*/

    if (begpp!=NULL) *begpp=NULL;
    if (endpp!=NULL) *endpp=NULL;

    return 0;
}


