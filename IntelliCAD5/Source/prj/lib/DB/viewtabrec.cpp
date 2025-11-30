/* C:\ICAD\PRJ\LIB\DB\VIEWTABREC.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "db.h"
#include "viewtabrec.h"

/************************** db_viewtabrec **************************/

/*F*/
db_viewtabrec::db_viewtabrec(void) :
    db_tablerecord(DB_VIEWTABREC,db_str_quotequote) {


    sds_point sp1;

    sp1[0]=sp1[1]=sp1[2]=0.0;

    unk=0;

    db_qgetvar(DB_QVIEWCTR   ,NULL,cent    ,DB_2DPOINT,0);
    db_qgetvar(DB_QVIEWDIR   ,NULL,dir     ,DB_3DPOINT,0);
    db_qgetvar(DB_QTARGET    ,NULL,target  ,DB_3DPOINT,0);
    db_qgetvar(DB_QVIEWSIZE  ,NULL,&ht     ,DB_DOUBLE ,0);
    db_qgetvar(DB_QSCREENSIZE,NULL,sp1    ,DB_2DPOINT,0);
        wd=ht*((sp1[0]>0.0 && sp1[1]>0.0) ? sp1[0]/sp1[1] : 1.5);
    db_qgetvar(DB_QLENSLENGTH,NULL,&lenslen,DB_DOUBLE ,0);
    db_qgetvar(DB_QFRONTZ    ,NULL,clip    ,DB_DOUBLE ,0);
    db_qgetvar(DB_QBACKZ     ,NULL,clip+1  ,DB_DOUBLE ,0);
    db_qgetvar(DB_QVIEWTWIST ,NULL,&twist  ,DB_DOUBLE ,0);
    db_qgetvar(DB_QVIEWMODE  ,NULL,&mode   ,DB_INT    ,0);
	ucstabrec = NULL;
}
db_viewtabrec::db_viewtabrec(char *pname, db_drawing *dp) :
    db_tablerecord(DB_VIEWTABREC,pname) {


    char *bufp;
    sds_point sp1;

    bufp=(dp!=NULL) ? dp->ret_headerbuf() : NULL;
    sp1[0]=sp1[1]=sp1[2]=0.0;

    unk=0;

    db_qgetvar(DB_QVIEWCTR    ,bufp,cent    ,DB_2DPOINT,0);
    db_qgetvar(DB_QVIEWDIR    ,bufp,dir     ,DB_3DPOINT,0);
    db_qgetvar(DB_QTARGET     ,bufp,target  ,DB_3DPOINT,0);
    db_qgetvar(DB_QVIEWSIZE   ,bufp,&ht     ,DB_DOUBLE ,0);
    db_qgetvar(DB_QSCREENSIZE ,NULL,sp1     ,DB_2DPOINT,0); /* Config var */
        wd=ht*((sp1[0]>0.0 && sp1[1]>0.0) ? sp1[0]/sp1[1] : 1.5);
    db_qgetvar(DB_QLENSLENGTH ,bufp,&lenslen,DB_DOUBLE ,0);
    db_qgetvar(DB_QFRONTZ     ,bufp,clip    ,DB_DOUBLE ,0);
    db_qgetvar(DB_QBACKZ      ,bufp,clip+1  ,DB_DOUBLE ,0);
    db_qgetvar(DB_QVIEWTWIST  ,bufp,&twist  ,DB_DOUBLE ,0);
    db_qgetvar(DB_QVIEWMODE   ,bufp,&mode   ,DB_INT    ,0);
	ucstabrec = NULL;

}
db_viewtabrec::db_viewtabrec(const db_viewtabrec &sour) :
    db_tablerecord(sour) {  /* Copy constructor */

    cent[0]=sour.cent[0]; cent[1]=sour.cent[1];
    DB_PTCPY(dir,sour.dir); DB_PTCPY(target,sour.target);

    ht     =sour.ht;      wd   =sour.wd;    lenslen=sour.lenslen; clip[0]=sour.clip[0];
    clip[1]=sour.clip[1]; twist=sour.twist; mode   =sour.mode;        unk=sour.unk;
	ucstabrec = NULL;

}


