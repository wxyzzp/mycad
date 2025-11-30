/* F:\BLD\PRJ\LIB\ICADLIB\RESVAL.C
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 *
 */

#include "icadlib.h"/*DNT*/

short ic_resval(short type) {
/*
**  Given a type (RTtype or DXF code), this function returns
**  a value telling which resval member applies in a resbuf.
**  With most RTtypes, it's obvious -- but not so with DXF codes.
**  But, even with RTtypes, it's handy; it'll tell you that
**  RTREAL, RTANG, and RTORINT use rreal; RTPOINT and RT3DPOINT
**  use rpoint; and RTENAME and RTPICKS use rlname.
**
**  Return val  resbuf.resval.? involved
**  ----------  ---------------------------------------------
**
**  RTNONE      None; the type IS the value (like RTLB, etc.)
**  RTREAL      rreal
**  RT3DPOINT   rpoint
**  RTSHORT     rint
**  RTSTR       rstring
**  RTENAME     rlname
**  RTLONG      rlong
**  RTBINARY    rbinary (binary chunk)
**  RTERROR     Not valid (unknown)
**
*/
    short rc;


    /* Don't do "else if"s; too many will max out the compiler's */
    /* ability to handle ifs within ifs. */

    rc=RTERROR;
    if (type>-6 && type<2000) {  /* DXF; cover all ranges */


		if (type < 0) {
		    if (type<  -4) { rc=RTENAME;   goto out; }  /* A guess */
			if (type<  -3) { rc=RTSTR;     goto out; }
			if (type<  -2) { rc=RTNONE;    goto out; }
		    rc=RTENAME;   goto out;
		}
		if (type<  10) { rc=RTSTR;     goto out; }
        if (type<  20) { rc=RT3DPOINT; goto out; }
        if (type<  60) { rc=RTREAL;    goto out; }
        if (type<  80) { rc=RTSHORT;   goto out; }
        if (type<  90) { rc=RTERROR;   goto out; } /* 80-89 reserved */
        if (type< 100) { rc=RTLONG;    goto out; }
        if (type< 110) { rc=RTSTR;     goto out; }
        // EBATECH(CNBR) -[ 2002/6/19 Bugzilla#78208 VIEW,VIEWPORT,VPORT,LAYOUT use 110-112 as 3DP
        if (type< 120) { rc=RT3DPOINT; goto out; }
        // EBATECH(CNBR) ]-
        if (type< 140) { rc=RTREAL;    goto out; }
        if (type< 160) { rc=RTREAL;    goto out; }
        if (type< 180) { rc=RTSHORT;   goto out; }
        if (type< 210) { rc=RTERROR;   goto out; } /* 180-209 reserved */
        if (type< 220) { rc=RT3DPOINT; goto out; }
        if (type< 240) { rc=RTREAL;    goto out; }
        if (type< 270) { rc=RTERROR;   goto out; } /* 240-269 reserved */
        if (type< 290) { rc=RTSHORT;   goto out; }
        // EBATECH(CNBR) -[ 23-1-2002 activate 290-299 LAYER use 290 as plotable flag
        if (type< 300) { rc=RTSHORT;   goto out; }
        //if (type< 300) { rc=RTERROR;   goto out; }
        // EBATECH(CNBR) -]
        if (type< 310) { rc=RTSTR;     goto out; }
        if (type< 320) { rc=RTBINARY;    goto out; }
        if (type< 330) { rc=RTENAME;     goto out; }
        if (type< 370) { rc=RTENAME;   goto out; }
        if (type< 390) { rc=RTSHORT;   goto out; }
        // EBATECH(CNBR) -[ 16-9-2002 activate 390-399 LAYER and ENTITY use 390 as plotstyle pointer
        if (type< 400) { rc=RTENAME;     goto out; }
        //if (type< 400) { rc=RTSTR;     goto out; }
        // EBATECH(CNBR) ]-
        if (type< 410) { rc=RTSHORT;   goto out; }
        if (type< 420) { rc=RTSTR;     goto out; }
        if (type< 999) { rc=RTERROR;   goto out; } /* 420-998 reserved */
        if (type<1004) { rc=RTSTR;     goto out; }
        if (type==1004) { rc=RTBINARY;  goto out; }
        if (type==1005) { rc=RTSTR;     goto out; }
        // EBATECH(CNBR) -[ 23-1-2002 reserved 1006-1009
        if (type<1010) { rc=RTERROR;   goto out; }
        // EBATECH(CNBR) ]-
        if (type<1014) { rc=RT3DPOINT; goto out; }
        if (type<1020) { rc=RTERROR;   goto out; }
        if (type<1024) { rc=RTREAL;    goto out; }
        if (type<1030) { rc=RTERROR;   goto out; }
        if (type<1034) { rc=RTREAL;    goto out; }
        if (type<1043) { rc=RTREAL;    goto out; }
        // EBATECH(CNBR) -[ 23-1-2002 reserved 1043-1069
        if (type<1070) { rc=RTERROR;   goto out; }
        // EBATECH(CNBR) ]-
        if (type==1070) { rc=RTSHORT;   goto out; }
        if (type==1071) { rc=RTLONG;    goto out; }
    } else {  /* an RTtype */
        if (type==RTREAL || type==RTANG || type==RTORINT) { rc=RTREAL;    goto out; }
        if (type==RTPOINT || type==RT3DPOINT)             { rc=RT3DPOINT; goto out; }
        if (type==RTSHORT)                                { rc=RTSHORT;   goto out; }
        if (type==RTSTR)                                  { rc=RTSTR;     goto out; }
        if (type==RTENAME || type==RTPICKS)               { rc=RTENAME;   goto out; }
        if (type==RTLONG)                                 { rc=RTLONG;    goto out; }
        if ( type==RTNONE || type==RTVOID || type==RTLB || type==RTLE ||
            type==RTDOTE || type==RTT || type==RTNIL || type==RTDXF0)
                                                          { rc=RTNONE;    goto out; }
    }

out:  /* Could do returns instead of all the "goto out"s, */
      /* but each return probably generates all the stack-popping */
      /* code. */

    return rc;
}


