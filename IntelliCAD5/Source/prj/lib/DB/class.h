/* C:\ICAD\PRJ\LIB\DB\CLASS.H
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#ifndef _INC_CLASS
#define _INC_CLASS

class db_class {

    friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:

    db_class(void);
    db_class(
        int   pclassnum,
        char *pdxfrecname,
        char *pcppname,
        char *pappname,
        char *pdxf3,
        int   pvernum,
        char  pwasproxy,
        int   pitemclassid);
    db_class(const db_class &sour);  /* Copy constructor */
   ~db_class(void);

    static void delll(db_class *ll) {
        db_class *p;
        while (ll!=NULL) { p=ll->next; delete ll; ll=p; }
    }

    void get_data(
        int   *classnump,
        char **dxfrecnamepp,
        char **cppnamepp,
        char **appnamepp,
        char **dxf3pp,
        int   *vernump,
        int   *wasproxyp,
        int   *itemclassidp) {

        if (classnump   !=NULL) *classnump   =classnum   ;
        if (dxfrecnamepp!=NULL) *dxfrecnamepp=dxfrecname ;  /* DON'T FREE! */
        if (cppnamepp   !=NULL) *cppnamepp   =cppname    ;  /* DON'T FREE! */
        if (appnamepp   !=NULL) *appnamepp   =appname    ;  /* DON'T FREE! */
        if (dxf3pp      !=NULL) *dxf3pp      =dxf3       ;  /* DON'T FREE! */
        if (vernump     !=NULL) *vernump     =vernum     ;
        if (wasproxyp   !=NULL) *wasproxyp   =wasproxy   ;
        if (itemclassidp!=NULL) *itemclassidp=itemclassid;
    }
    void set_data(
        int   pclassnum,
        char *pdxfrecname,
        char *pcppname,
        char *pappname,
        char *pdxf3,
        int   pvernum,
        int   pwasproxy,
        int   pitemclassid) {

        classnum=pclassnum; vernum=pvernum;
        wasproxy=(char)pwasproxy; itemclassid=pitemclassid;
        db_astrncpy(&dxfrecname,pdxfrecname,-1);
        db_astrncpy(&cppname   ,pcppname   ,-1);
        db_astrncpy(&appname   ,pappname   ,-1);
        db_astrncpy(&dxf3      ,pdxf3      ,-1);
    }

  /* Public "next" for llist purposes: */
    db_class *next;

  private:

    int   classnum;    /*   ?  (MarComp has it) */
    char *dxfrecname;  /*   0 */
    char *cppname;     /*   1 */
    char *appname;     /*   2 */
    char *dxf3;        /*   3  (DXFOUT shows it; not doc'd; not in MarComp; */
                       /*        always seems to be "ISM".) */
    int   vernum;      /*  90 */
    char  wasproxy;    /* 280 */
    int   itemclassid; /* 281 Item class ID, which generates "isent". */
};

#endif

