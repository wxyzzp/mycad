/* C:\ICAD\PRJ\LIB\DB\GROUP.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "db.h"
#include "objects.h"

/***************************** db_group *****************************/

db_group::db_group(void) : db_handitem(DB_GROUP) {
    deleted=0;
    unnamed=selectable=0; desc=NULL; hirefll[0]=hirefll[1]=NULL;
}
db_group::db_group(const db_group &sour) :
    db_handitem(sour) {  /* Copy constructor */

    db_hireflink *tp1,*tp2;

    deleted   =sour.deleted;
    unnamed   =sour.unnamed;
    selectable=sour.selectable;
    desc=NULL; db_astrncpy(&desc,sour.desc,-1);

    hirefll[0]=hirefll[1]=NULL;
    for (tp1=sour.hirefll[0]; tp1!=NULL; tp1=tp1->next)
        { tp2=new db_hireflink(*tp1); addhiref(tp2); }
}
db_group::~db_group(void) { delete [] desc; db_hireflink::delll(hirefll[0]); }


