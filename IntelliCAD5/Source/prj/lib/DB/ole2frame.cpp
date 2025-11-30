/* C:\ICAD\PRJ\LIB\DB\OLE2FRAME.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "db.h"


/**************************** db_ole2frame *****************************/

/*F*/
db_ole2frame::db_ole2frame(void) : db_entity(DB_OLE2FRAME) {
    tilemodedesc=datasz=0; unk1=unk2=0; ver=oletype=2;
    memset(unk3,0,sizeof(unk3)); desc=data=NULL;
    memset(corner,0,sizeof(corner));
    memset(extent,0,sizeof(extent));
}
db_ole2frame::db_ole2frame(db_drawing *dp) : db_entity(DB_OLE2FRAME,dp) {
    tilemodedesc=datasz=0; unk1=unk2=0; ver=oletype=2;
    memset(unk3,0,sizeof(unk3)); desc=data=NULL;
    memset(corner,0,sizeof(corner));
    memset(extent,0,sizeof(extent));
}
db_ole2frame::db_ole2frame(const db_ole2frame &sour) :
    db_entity(sour) {  /* Copy constructor */

    ver         =sour.ver;          oletype=sour.oletype;
    tilemodedesc=sour.tilemodedesc; datasz =sour.datasz;
    unk1        =sour.unk1;         unk2   =sour.unk2;
    memcpy(unk3,sour.unk3,sizeof(unk3));
    memcpy(corner,sour.corner,sizeof(corner));
    desc=NULL; db_astrncpy(&desc,sour.desc,-1);
    data=NULL;
    if (datasz>0 && sour.data!=NULL) {
        data=new char[datasz];
        memcpy(data,sour.data,datasz);
    }
    DB_PTCPY(extent[0],sour.extent[0]); DB_PTCPY(extent[1],sour.extent[1]);
}
db_ole2frame::~db_ole2frame(void) { delete [] desc; delete [] data; }



