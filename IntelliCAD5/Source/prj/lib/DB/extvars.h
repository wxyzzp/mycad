/* C:\ICAD\PRJ\LIB\DB\EXTVARS.H
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

struct db_handitemtypeinfo {
    LPTSTR name;
    char  typetype;
    int   tabtype;
};



    extern char *db_str_0;
    extern char *db_str_acad;
    extern char *db_str_byblock;
    extern char *db_str_bylayer;
    extern char *db_str_continuous;
    extern char *db_str_invalid;
    extern char *db_str_msblockname;
    extern char *db_str_psblockname;
    extern char *db_str_quotequote;
    extern char *db_str_standard;
    extern char *db_str_staractive;
    extern char *db_str_txt;
	extern char* db_str_bycolor; //Ebatech(cnbr)
	extern char* db_str_normal;  //Ebatech(cnbr)

    extern char *db_icfontnm,*db_icfontfn;
    extern char *db_tolfontnm;
    extern char *db_ourtolfontnm;

    extern db_handitemtypeinfo db_hitypeinfo[];
    extern int db_tab2tabrec[];

    extern db_sysvarlink db_oldsysvar[];
    extern int db_nvars;

	extern db_sysvarlink db_measurement_metric[];
	extern db_sysvarlink db_pstylemode_named[];

