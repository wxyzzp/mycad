/* C:\ICAD\PRJ\LIB\DB\APPID.H
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#ifndef _INC_APPID
#define _INC_APPID

class DB_CLASS db_appidtabrec : public db_tablerecord {

    friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:

    db_appidtabrec(void);
    db_appidtabrec(char *pname, db_drawing *dp);
    db_appidtabrec(const db_appidtabrec &sour);  /* Copy constructor */

    bool get_71(int *p) { *p=xrefidx; return true; }

    bool set_71(int p) { xrefidx=p; return true; }

  private:

    int xrefidx;  /* 71?  Following MarComp's info */
};

#endif

