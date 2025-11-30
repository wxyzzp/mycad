/* C:\ICAD\PRJ\LIB\DB\VPORTTABREC.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

/************************** db_vporttabrec **************************/

#include "db.h"
#include "vporttabrec.h"

/*F*/
db_vporttabrec::db_vporttabrec(void) :
    db_tablerecord(DB_VPORTTABREC,db_str_quotequote) {


    sds_point sp1;

    sp1[0]=sp1[1]=sp1[2]=0.0;

    corner[0][0]=corner[0][1]=0.0;
    corner[1][0]=corner[1][1]=1.0;
    db_qgetvar(DB_QVIEWCTR    ,NULL,cent        ,DB_2DPOINT,0);
    db_qgetvar(DB_QSNAPBASE   ,NULL,snapbase    ,DB_2DPOINT,0);
    db_qgetvar(DB_QSNAPUNIT   ,NULL,snapspace   ,DB_2DPOINT,0);
    db_qgetvar(DB_QGRIDUNIT   ,NULL,gridspace   ,DB_2DPOINT,0);
    db_qgetvar(DB_QVIEWDIR    ,NULL,dir         ,DB_3DPOINT,0);
    db_qgetvar(DB_QTARGET     ,NULL,target      ,DB_3DPOINT,0);
    db_qgetvar(DB_QVIEWSIZE   ,NULL,&ht         ,DB_DOUBLE ,0);
    db_qgetvar(DB_QSCREENSIZE ,NULL,sp1         ,DB_2DPOINT,0);
        asp=(sp1[0]>0.0 && sp1[1]>0.0) ? sp1[0]/sp1[1] : 1.5;
    db_qgetvar(DB_QLENSLENGTH ,NULL,&lenslen    ,DB_DOUBLE ,0);
    db_qgetvar(DB_QFRONTZ     ,NULL,clip        ,DB_DOUBLE ,0);
    db_qgetvar(DB_QBACKZ      ,NULL,clip+1      ,DB_DOUBLE ,0);
    db_qgetvar(DB_QSNAPANG    ,NULL,&snaprot    ,DB_DOUBLE ,0);
    db_qgetvar(DB_QVIEWTWIST  ,NULL,&twist      ,DB_DOUBLE ,0);
    stat=0;
    id=2;
    db_qgetvar(DB_QVIEWMODE   ,NULL,&mode       ,DB_INT    ,0);
    db_qgetvar(DB_QZOOMPERCENT,NULL,&czoom      ,DB_INT    ,0);
    db_qgetvar(DB_QFASTZOOM   ,NULL,&fzoom      ,DB_CHAR   ,0);
    db_qgetvar(DB_QUCSICON    ,NULL,&ucsicon    ,DB_CHAR   ,0);
    db_qgetvar(DB_QSNAPMODE   ,NULL,&snap       ,DB_CHAR   ,0);
    db_qgetvar(DB_QGRIDMODE   ,NULL,&grid       ,DB_CHAR   ,0);
    db_qgetvar(DB_QSNAPSTYL   ,NULL,&snapstyl   ,DB_CHAR   ,0);
    db_qgetvar(DB_QSNAPISOPAIR,NULL,&snapisopair,DB_CHAR   ,0);
	
	original_vport = NULL;
}
db_vporttabrec::db_vporttabrec(char *pname, db_drawing *dp) :
    db_tablerecord(DB_VPORTTABREC,pname) {


    char *bufp;
    sds_point sp1;

    bufp=(dp!=NULL) ? dp->ret_headerbuf() : NULL;
    sp1[0]=sp1[1]=sp1[2]=0.0;

    corner[0][0]=corner[0][1]=0.0;
    corner[1][0]=corner[1][1]=1.0;
    db_qgetvar(DB_QVIEWCTR    ,bufp,cent        ,DB_2DPOINT,0);
    db_qgetvar(DB_QSNAPBASE   ,bufp,snapbase    ,DB_2DPOINT,0);
    db_qgetvar(DB_QSNAPUNIT   ,bufp,snapspace   ,DB_2DPOINT,0);
    db_qgetvar(DB_QGRIDUNIT   ,bufp,gridspace   ,DB_2DPOINT,0);
    db_qgetvar(DB_QVIEWDIR    ,bufp,dir         ,DB_3DPOINT,0);
    db_qgetvar(DB_QTARGET     ,bufp,target      ,DB_3DPOINT,0);
    db_qgetvar(DB_QVIEWSIZE   ,bufp,&ht         ,DB_DOUBLE ,0);
    db_qgetvar(DB_QSCREENSIZE ,NULL,sp1         ,DB_2DPOINT,0); /* Config var */
        asp=(sp1[0]>0.0 && sp1[1]>0.0) ? sp1[0]/sp1[1] : 1.5;
    db_qgetvar(DB_QLENSLENGTH ,bufp,&lenslen    ,DB_DOUBLE ,0);
    db_qgetvar(DB_QFRONTZ     ,bufp,clip        ,DB_DOUBLE ,0);
    db_qgetvar(DB_QBACKZ      ,bufp,clip+1      ,DB_DOUBLE ,0);
    db_qgetvar(DB_QSNAPANG    ,bufp,&snaprot    ,DB_DOUBLE ,0);
    db_qgetvar(DB_QVIEWTWIST  ,bufp,&twist      ,DB_DOUBLE ,0);
    stat=0;
    id=2;
    db_qgetvar(DB_QVIEWMODE   ,bufp,&mode       ,DB_INT    ,0);
    db_qgetvar(DB_QZOOMPERCENT,bufp,&czoom      ,DB_INT    ,0);
    db_qgetvar(DB_QFASTZOOM   ,bufp,&fzoom      ,DB_CHAR   ,0);
    db_qgetvar(DB_QUCSICON    ,bufp,&ucsicon    ,DB_CHAR   ,0);
    db_qgetvar(DB_QSNAPMODE   ,bufp,&snap       ,DB_CHAR   ,0);
    db_qgetvar(DB_QGRIDMODE   ,bufp,&grid       ,DB_CHAR   ,0);
    db_qgetvar(DB_QSNAPSTYL   ,bufp,&snapstyl   ,DB_CHAR   ,0);
    db_qgetvar(DB_QSNAPISOPAIR,bufp,&snapisopair,DB_CHAR   ,0);

	original_vport = NULL;
}
db_vporttabrec::db_vporttabrec(const db_vporttabrec &sour) :
    db_tablerecord(sour) {  /* Copy constructor */

    DB_PTCPY(dir,sour.dir); DB_PTCPY(target,sour.target);

    corner[0][0]=sour.corner[0][0];   lenslen    =sour.lenslen;
    corner[0][1]=sour.corner[0][1];   clip[2]    =sour.clip[2];
    corner[1][0]=sour.corner[1][0];   snaprot    =sour.snaprot;
    corner[1][1]=sour.corner[1][1];   twist      =sour.twist;
    cent[0]     =sour.cent[0];        stat       =sour.stat;
    cent[1]     =sour.cent[1];        id         =sour.id;
    snapbase[0] =sour.snapbase[0];    mode       =sour.mode;
    snapbase[1] =sour.snapbase[1];    czoom      =sour.czoom;
    snapspace[0]=sour.snapspace[0];   fzoom      =sour.fzoom;
    snapspace[1]=sour.snapspace[1];   ucsicon    =sour.ucsicon;
    gridspace[0]=sour.gridspace[0];   snap       =sour.snap;
    gridspace[1]=sour.gridspace[1];   grid       =sour.grid;
    ht          =sour.ht;             snapstyl   =sour.snapstyl;
    asp         =sour.asp;            snapisopair=sour.snapisopair;
	original_vport = NULL;
}

bool db_vporttabrec::get_original_viewport (db_handitem **vp)
	{
	*vp = original_vport;
	return true;
	}

bool db_vporttabrec::set_original_viewport (db_handitem *vp)
	{
	original_vport = vp;
	return true;
	}



