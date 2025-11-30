/* C:\ICAD\PRJ\LIB\DB\DIMSTYLETABREC.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

/************************** db_dimstyletabrec **************************/

#include "db.h"
#include "dimstyletabrec.h"

/*F*/
db_dimstyletabrec::db_dimstyletabrec(void) :
    db_tablerecord(DB_DIMSTYLETABREC,db_str_quotequote) {


    post =NULL; db_qgetvar(DB_QDIMPOST ,NULL,&post ,DB_ASTRING,0);
    apost=NULL; db_qgetvar(DB_QDIMAPOST,NULL,&apost,DB_ASTRING,0);
    blk  =NULL; db_qgetvar(DB_QDIMBLK  ,NULL,&blk  ,DB_ASTRING,0);
    blk1 =NULL; db_qgetvar(DB_QDIMBLK1 ,NULL,&blk1 ,DB_ASTRING,0);
    blk2 =NULL; db_qgetvar(DB_QDIMBLK2 ,NULL,&blk2 ,DB_ASTRING,0);

    // SystemMetrix(Hiro)-[Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED
    //// EBATECH(watanabe)-[dimldrblk
    //ldrblk=NULL;db_qgetvar(DB_QDIMLDRBLK,NULL,&ldrblk,DB_ASTRING,0);
    ldrblk = NULL;
    //// ]-EBATECH(watanabe)
    // ]-SystemMetrix(Hiro) Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED

    db_qgetvar(DB_QDIMSCALE,NULL,&scale,DB_DOUBLE,0);
    db_qgetvar(DB_QDIMASZ  ,NULL,&asz  ,DB_DOUBLE,0);
    db_qgetvar(DB_QDIMEXO  ,NULL,&exo  ,DB_DOUBLE,0);
    db_qgetvar(DB_QDIMDLI  ,NULL,&dli  ,DB_DOUBLE,0);
    db_qgetvar(DB_QDIMEXE  ,NULL,&exe  ,DB_DOUBLE,0);
    db_qgetvar(DB_QDIMRND  ,NULL,&rnd  ,DB_DOUBLE,0);
    db_qgetvar(DB_QDIMDLE  ,NULL,&dle  ,DB_DOUBLE,0);
    db_qgetvar(DB_QDIMTP   ,NULL,&tp   ,DB_DOUBLE,0);
    db_qgetvar(DB_QDIMTM   ,NULL,&tm   ,DB_DOUBLE,0);

    db_qgetvar(DB_QDIMTOL,NULL,&tol,DB_CHAR,0);
    db_qgetvar(DB_QDIMLIM,NULL,&lim,DB_CHAR,0);
    db_qgetvar(DB_QDIMTIH,NULL,&tih,DB_CHAR,0);
    db_qgetvar(DB_QDIMTOH,NULL,&toh,DB_CHAR,0);
    db_qgetvar(DB_QDIMSE1,NULL,&se1,DB_CHAR,0);
    db_qgetvar(DB_QDIMSE2,NULL,&se2,DB_CHAR,0);
    db_qgetvar(DB_QDIMTAD,NULL,&tad,DB_CHAR,0);
    db_qgetvar(DB_QDIMZIN,NULL,&zin,DB_CHAR,0);
    db_qgetvar(DB_QDIMAZIN,NULL,&azin,DB_CHAR,0); // EBATECH(CNBR) Bugzilla#78443 2003/2/7

    db_qgetvar(DB_QDIMTXT ,NULL,&txt ,DB_DOUBLE,0);
    db_qgetvar(DB_QDIMCEN ,NULL,&cen ,DB_DOUBLE,0);
    db_qgetvar(DB_QDIMTSZ ,NULL,&tsz ,DB_DOUBLE,0);
    db_qgetvar(DB_QDIMALTF,NULL,&altf,DB_DOUBLE,0);
    db_qgetvar(DB_QDIMLFAC,NULL,&lfac,DB_DOUBLE,0);
    db_qgetvar(DB_QDIMTVP ,NULL,&tvp ,DB_DOUBLE,0);
    db_qgetvar(DB_QDIMTFAC,NULL,&tfac,DB_DOUBLE,0);
    db_qgetvar(DB_QDIMGAP ,NULL,&gap ,DB_DOUBLE,0);

    db_qgetvar(DB_QDIMALT ,NULL,&alt ,DB_CHAR,0);
    db_qgetvar(DB_QDIMALTD,NULL,&altd,DB_CHAR,0);
    db_qgetvar(DB_QDIMTOFL,NULL,&tofl,DB_CHAR,0);
    db_qgetvar(DB_QDIMSAH ,NULL,&sah ,DB_CHAR,0);
    db_qgetvar(DB_QDIMTIX ,NULL,&tix ,DB_CHAR,0);
    db_qgetvar(DB_QDIMSOXD,NULL,&soxd,DB_CHAR,0);

    db_qgetvar(DB_QDIMCLRD,NULL,&clrd,DB_INT,0);
    db_qgetvar(DB_QDIMCLRE,NULL,&clre,DB_INT,0);
    db_qgetvar(DB_QDIMCLRT,NULL,&clrt,DB_INT,0);
    db_qgetvar(DB_QDIMADEC,NULL,&adec,DB_CHAR,0); // EBATECH(CNBR) Bugzilla#78443 2003/2/7

    db_qgetvar(DB_QDIMUNIT ,NULL,&unit ,DB_CHAR,0);
    db_qgetvar(DB_QDIMDEC  ,NULL,&dec  ,DB_CHAR,0);
    db_qgetvar(DB_QDIMTDEC ,NULL,&tdec ,DB_CHAR,0);
    db_qgetvar(DB_QDIMALTU ,NULL,&altu ,DB_CHAR,0);
    db_qgetvar(DB_QDIMALTTD,NULL,&alttd,DB_CHAR,0);
    db_qgetvar(DB_QDIMAUNIT,NULL,&aunit,DB_CHAR,0);
    db_qgetvar(DB_QDIMJUST ,NULL,&just ,DB_CHAR,0);
    db_qgetvar(DB_QDIMSD1  ,NULL,&sd1  ,DB_CHAR,0);
    db_qgetvar(DB_QDIMSD2  ,NULL,&sd2  ,DB_CHAR,0);
    db_qgetvar(DB_QDIMTOLJ ,NULL,&tolj ,DB_CHAR,0);
    db_qgetvar(DB_QDIMTZIN ,NULL,&tzin ,DB_CHAR,0);
    db_qgetvar(DB_QDIMALTZ ,NULL,&altz ,DB_CHAR,0);
    db_qgetvar(DB_QDIMALTTZ,NULL,&alttz,DB_CHAR,0);
    db_qgetvar(DB_QDIMFIT  ,NULL,&fit  ,DB_CHAR,0);
    db_qgetvar(DB_QDIMUPT  ,NULL,&upt  ,DB_CHAR,0);

    // EBATECH(watanabe)-[dimatfit
    db_qgetvar(DB_QDIMATFIT,NULL,&atfit,DB_CHAR,0);
    // ]-EBATECH(watanabe)

    // EBATECH(watanabe)-[dimfrac
    db_qgetvar(DB_QDIMFRAC ,NULL,&frac ,DB_CHAR,0);
    // ]-EBATECH(watanabe)

    // EBATECH(watanabe)-[dimlunit
    db_qgetvar(DB_QDIMLUNIT,NULL,&lunit,DB_CHAR,0);
    // ]-EBATECH(watanabe)

    // EBATECH(watanabe)-[dimtmove
    db_qgetvar(DB_QDIMTMOVE,NULL,&tmove,DB_CHAR,0);
    // ]-EBATECH(watanabe)

    txsty=NULL;

	// EBATECH(CNBR) 24-03-2003 Bugzilla#78471 Support Lineweight
	db_qgetvar(DB_QDIMLWD,NULL,&lwd,DB_INT,0);
	db_qgetvar(DB_QDIMLWE,NULL,&lwe,DB_INT,0);
    db_qgetvar(DB_QDIMALTRND  ,NULL,&altrnd  ,DB_DOUBLE,0);
    {
		char tmp[2];
		db_qgetvar(DB_QDIMDSEP ,NULL,tmp,DB_STRING,2);
		dsep = tmp[0];
	}
	// EBATECH(CNBR) ]-
}
db_dimstyletabrec::db_dimstyletabrec(char *pname, db_drawing *dp) :
    db_tablerecord(DB_DIMSTYLETABREC,pname) {


    char *bufp;


    bufp=(dp!=NULL) ? dp->ret_headerbuf() : NULL;

    post =NULL; db_qgetvar(DB_QDIMPOST ,bufp,&post ,DB_ASTRING,0);
    apost=NULL; db_qgetvar(DB_QDIMAPOST,bufp,&apost,DB_ASTRING,0);
    blk  =NULL; db_qgetvar(DB_QDIMBLK  ,bufp,&blk  ,DB_ASTRING,0);
    blk1 =NULL; db_qgetvar(DB_QDIMBLK1 ,bufp,&blk1 ,DB_ASTRING,0);
    blk2 =NULL; db_qgetvar(DB_QDIMBLK2 ,bufp,&blk2 ,DB_ASTRING,0);

    // SystemMetrix(Hiro)-[Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED
    //// EBATECH(watanabe)-[dimldrblk
    //ldrblk=NULL;db_qgetvar(DB_QDIMLDRBLK,bufp,&ldrblk,DB_ASTRING,0);
    ldrblk=(dp!=NULL) ? dp->ret_currenttabrec(DB_QDIMLDRBLK) : NULL;
    //// ]-EBATECH(watanabe)
    // ]-SystemMetrix(Hiro) Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED

    db_qgetvar(DB_QDIMSCALE,bufp,&scale,DB_DOUBLE,0);
    db_qgetvar(DB_QDIMASZ  ,bufp,&asz  ,DB_DOUBLE,0);
    db_qgetvar(DB_QDIMEXO  ,bufp,&exo  ,DB_DOUBLE,0);
    db_qgetvar(DB_QDIMDLI  ,bufp,&dli  ,DB_DOUBLE,0);
    db_qgetvar(DB_QDIMEXE  ,bufp,&exe  ,DB_DOUBLE,0);
    db_qgetvar(DB_QDIMRND  ,bufp,&rnd  ,DB_DOUBLE,0);
    db_qgetvar(DB_QDIMDLE  ,bufp,&dle  ,DB_DOUBLE,0);
    db_qgetvar(DB_QDIMTP   ,bufp,&tp   ,DB_DOUBLE,0);
    db_qgetvar(DB_QDIMTM   ,bufp,&tm   ,DB_DOUBLE,0);

    db_qgetvar(DB_QDIMTOL,bufp,&tol,DB_CHAR,0);
    db_qgetvar(DB_QDIMLIM,bufp,&lim,DB_CHAR,0);
    db_qgetvar(DB_QDIMTIH,bufp,&tih,DB_CHAR,0);
    db_qgetvar(DB_QDIMTOH,bufp,&toh,DB_CHAR,0);
    db_qgetvar(DB_QDIMSE1,bufp,&se1,DB_CHAR,0);
    db_qgetvar(DB_QDIMSE2,bufp,&se2,DB_CHAR,0);
    db_qgetvar(DB_QDIMTAD,bufp,&tad,DB_CHAR,0);
    db_qgetvar(DB_QDIMZIN,bufp,&zin,DB_CHAR,0);
    db_qgetvar(DB_QDIMAZIN,bufp,&azin,DB_CHAR,0); // EBATECH(CNBR) Bugzilla#78443 2003/2/7

    db_qgetvar(DB_QDIMTXT ,bufp,&txt ,DB_DOUBLE,0);
    db_qgetvar(DB_QDIMCEN ,bufp,&cen ,DB_DOUBLE,0);
    db_qgetvar(DB_QDIMTSZ ,bufp,&tsz ,DB_DOUBLE,0);
    db_qgetvar(DB_QDIMALTF,bufp,&altf,DB_DOUBLE,0);
    db_qgetvar(DB_QDIMLFAC,bufp,&lfac,DB_DOUBLE,0);
    db_qgetvar(DB_QDIMTVP ,bufp,&tvp ,DB_DOUBLE,0);
    db_qgetvar(DB_QDIMTFAC,bufp,&tfac,DB_DOUBLE,0);
    db_qgetvar(DB_QDIMGAP ,bufp,&gap ,DB_DOUBLE,0);

    db_qgetvar(DB_QDIMALT ,bufp,&alt ,DB_CHAR,0);
    db_qgetvar(DB_QDIMALTD,bufp,&altd,DB_CHAR,0);
    db_qgetvar(DB_QDIMTOFL,bufp,&tofl,DB_CHAR,0);
    db_qgetvar(DB_QDIMSAH ,bufp,&sah ,DB_CHAR,0);
    db_qgetvar(DB_QDIMTIX ,bufp,&tix ,DB_CHAR,0);
    db_qgetvar(DB_QDIMSOXD,bufp,&soxd,DB_CHAR,0);

    db_qgetvar(DB_QDIMCLRD,bufp,&clrd,DB_INT,0);
    db_qgetvar(DB_QDIMCLRE,bufp,&clre,DB_INT,0);
    db_qgetvar(DB_QDIMCLRT,bufp,&clrt,DB_INT,0);
    db_qgetvar(DB_QDIMADEC,bufp,&adec,DB_CHAR,0); // EBATECH(CNBR) Bugzilla#78443 2003/2/7

    db_qgetvar(DB_QDIMUNIT ,bufp,&unit ,DB_CHAR,0);
    db_qgetvar(DB_QDIMDEC  ,bufp,&dec  ,DB_CHAR,0);
    db_qgetvar(DB_QDIMTDEC ,bufp,&tdec ,DB_CHAR,0);
    db_qgetvar(DB_QDIMALTU ,bufp,&altu ,DB_CHAR,0);
    db_qgetvar(DB_QDIMALTTD,bufp,&alttd,DB_CHAR,0);
    db_qgetvar(DB_QDIMAUNIT,bufp,&aunit,DB_CHAR,0);
    db_qgetvar(DB_QDIMJUST ,bufp,&just ,DB_CHAR,0);
    db_qgetvar(DB_QDIMSD1  ,bufp,&sd1  ,DB_CHAR,0);
    db_qgetvar(DB_QDIMSD2  ,bufp,&sd2  ,DB_CHAR,0);
    db_qgetvar(DB_QDIMTOLJ ,bufp,&tolj ,DB_CHAR,0);
    db_qgetvar(DB_QDIMTZIN ,bufp,&tzin ,DB_CHAR,0);
    db_qgetvar(DB_QDIMALTZ ,bufp,&altz ,DB_CHAR,0);
    db_qgetvar(DB_QDIMALTTZ,bufp,&alttz,DB_CHAR,0);
    db_qgetvar(DB_QDIMFIT  ,bufp,&fit  ,DB_CHAR,0);
    db_qgetvar(DB_QDIMUPT  ,bufp,&upt  ,DB_CHAR,0);

    // EBATECH(watanabe)-[dimatfit
    db_qgetvar(DB_QDIMATFIT,bufp,&atfit,DB_CHAR,0);
    // ]-EBATECH(watanabe)

    // EBATECH(watanabe)-[dimfrac
    db_qgetvar(DB_QDIMFRAC ,bufp,&frac ,DB_CHAR,0);
    // ]-EBATECH(watanabe)

    // EBATECH(watanabe)-[dimlunit
    db_qgetvar(DB_QDIMLUNIT,bufp,&lunit,DB_CHAR,0);
    // ]-EBATECH(watanabe)

    // EBATECH(watanabe)-[dimtmove
    db_qgetvar(DB_QDIMTMOVE,bufp,&tmove,DB_CHAR,0);
    // ]-EBATECH(watanabe)

    txsty=(dp!=NULL) ? dp->ret_currenttabrec(DB_QDIMTXSTY) : NULL;

	// EBATECH(CNBR) 24-03-2003 Bugzilla#78471 Support Lineweight
	db_qgetvar(DB_QDIMLWD,    bufp,&lwd,DB_INT,0);
	db_qgetvar(DB_QDIMLWE,    bufp,&lwe,DB_INT,0);
    db_qgetvar(DB_QDIMALTRND, bufp,&altrnd ,DB_DOUBLE,0);
    {
		char tmp[2];
		db_qgetvar(DB_QDIMDSEP ,bufp,tmp,DB_STRING,2);
		dsep = tmp[0];
	}
	// EBATECH(CNBR) ]-
}
db_dimstyletabrec::db_dimstyletabrec(const db_dimstyletabrec &sour) :
    db_tablerecord(sour) {  /* Copy constructor */

    post =NULL; if (sour.post !=NULL) db_astrncpy(&post ,sour.post ,-1);
    apost=NULL; if (sour.apost!=NULL) db_astrncpy(&apost,sour.apost,-1);
    blk  =NULL; if (sour.blk  !=NULL) db_astrncpy(&blk  ,sour.blk  ,-1);
    blk1 =NULL; if (sour.blk1 !=NULL) db_astrncpy(&blk1 ,sour.blk1 ,-1);
    blk2 =NULL; if (sour.blk2 !=NULL) db_astrncpy(&blk2 ,sour.blk2 ,-1);

    // SystemMetrix(Hiro)-[Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED
    //// EBATECH(watanabe)-[dimldrblk
    //ldrblk=NULL;if (sour.ldrblk!=NULL) db_astrncpy(&ldrblk,sour.ldrblk,-1);
    ldrblk = sour.ldrblk;
    //// ]-EBATECH(watanabe)
    // ]-SystemMetrix(Hiro) Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED

    scale=(icadRealEqual(sour.scale,0.0)) ? 1.0 : sour.scale;
                      se1 =sour.se1;  altd =sour.altd;  aunit=sour.aunit;
    asz  =sour.asz;   se2 =sour.se2;  tofl =sour.tofl;  just =sour.just;
    exo  =sour.exo;   tad =sour.tad;  sah  =sour.sah;   sd1  =sour.sd1;
    dli  =sour.dli;   zin =sour.zin;  tix  =sour.tix;   sd2  =sour.sd2;
    exe  =sour.exe;   txt =sour.txt;  soxd =sour.soxd;  tolj =sour.tolj;
    rnd  =sour.rnd;   cen =sour.cen;  clrd =sour.clrd;  tzin =sour.tzin;
    dle  =sour.dle;   tsz =sour.tsz;  clre =sour.clre;  altz =sour.altz;
    tp   =sour.tp;    altf=sour.altf; clrt =sour.clrt;  alttz=sour.alttz;
    tm   =sour.tm;    lfac=sour.lfac; unit =sour.unit;  fit  =sour.fit;
    tol  =sour.tol;   tvp =sour.tvp;  dec  =sour.dec;   upt  =sour.upt;
    lim  =sour.lim;   tfac=sour.tfac; tdec =sour.tdec;  txsty=sour.txsty;
    tih  =sour.tih;   gap =sour.gap;  altu =sour.altu;
    toh  =sour.toh;   alt =sour.alt;  alttd=sour.alttd;

    azin =sour.azin;  adec=sour.adec; // Bugzilla#78443 2003/2/7 EBATECH(CNBR)
	lwd = sour.lwd;   lwe = sour.lwe; // Bugzilla#78443 24-03-2003 Bugzilla#78471 Support Lineweight
	dsep = sour.dsep; altrnd = sour.altrnd;

    // EBATECH(watanabe)-[dimatfit
    atfit=sour.atfit;
    // ]-EBATECH(watanabe)

    // EBATECH(watanabe)-[dimfrac
    frac =sour.frac;
    // ]-EBATECH(watanabe)

    // EBATECH(watanabe)-[dimlunit
    lunit=sour.lunit;
    // ]-EBATECH(watanabe)

    // EBATECH(watanabe)-[dimtmove
    tmove=sour.tmove;
    // ]-EBATECH(watanabe)
}
db_dimstyletabrec::~db_dimstyletabrec(void) {
    delete [] post; delete [] apost;
    delete [] blk;  delete [] blk1;  delete [] blk2;

    // SystemMetrix(Hiro)-[Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED
    //// EBATECH(watanabe)-[dimldrblk
    //delete [] ldrblk;
    //// ]-EBATECH(watanabe)
    // ]-SystemMetrix(Hiro) Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED
}
