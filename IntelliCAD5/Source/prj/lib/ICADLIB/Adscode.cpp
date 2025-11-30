
// THIS FILE HAS BEEN GLOBALIZED!

#include "icadlib.h"/*DNT*/

void ic_adscode(short code, char *str) {
/*
**  Translates the numeric ADS codes into their #define names.
**  Based on adscodes.h.
*/

    if (str==NULL) return;

/* 
	The Following Strings are used by lib\lisp\debug.cpp only,
	No translations required.
*/
    if (code==RQSAVE   ) { strcpy(str,"RQSAVE"/*DNT*/);    return; }
    if (code==RQEND    ) { strcpy(str,"RQEND"/*DNT*/);     return; }
    if (code==RQQUIT   ) { strcpy(str,"RQQUIT"/*DNT*/);    return; }
    if (code==RQCFG    ) { strcpy(str,"RQCFG"/*DNT*/);     return; }
    if (code==RQXLOAD  ) { strcpy(str,"RQXLOAD"/*DNT*/);   return; }
    if (code==RQXUNLD  ) { strcpy(str,"RQXUNLD"/*DNT*/);   return; }
    if (code==RQSUBR   ) { strcpy(str,"RQSUBR"/*DNT*/);    return; }
    if (code==RSRSLT   ) { strcpy(str,"RSRSLT"/*DNT*/);    return; }
    if (code==RSERR    ) { strcpy(str,"RSERR"/*DNT*/);     return; }
    if (code==RTNONE   ) { strcpy(str,"RTNONE"/*DNT*/);    return; }
    if (code==RTREAL   ) { strcpy(str,"RTREAL"/*DNT*/);    return; }
    if (code==RTPOINT  ) { strcpy(str,"RTPOINT"/*DNT*/);   return; }
    if (code==RTSHORT  ) { strcpy(str,"RTSHORT"/*DNT*/);   return; }
    if (code==RTANG    ) { strcpy(str,"RTANG"/*DNT*/);     return; }
    if (code==RTSTR    ) { strcpy(str,"RTSTR"/*DNT*/);     return; }
    if (code==RTENAME  ) { strcpy(str,"RTENAME"/*DNT*/);   return; }
    if (code==RTPICKS  ) { strcpy(str,"RTPICKS"/*DNT*/);   return; }
    if (code==RTORINT  ) { strcpy(str,"RTORINT"/*DNT*/);   return; }
    if (code==RT3DPOINT) { strcpy(str,"RT3DPOINT"/*DNT*/); return; }
    if (code==RTLONG   ) { strcpy(str,"RTLONG"/*DNT*/);    return; }
    if (code==RTVOID   ) { strcpy(str,"RTVOID"/*DNT*/);    return; }
    if (code==RTLB     ) { strcpy(str,"RTLB"/*DNT*/);      return; }
    if (code==RTLE     ) { strcpy(str,"RTLE"/*DNT*/);      return; }
    if (code==RTDOTE   ) { strcpy(str,"RTDOTE"/*DNT*/);    return; }
    if (code==RTNIL    ) { strcpy(str,"RTNIL"/*DNT*/);     return; }
    if (code==RTDXF0   ) { strcpy(str,"RTDXF0"/*DNT*/);    return; }
    if (code==RTT      ) { strcpy(str,"RTT"/*DNT*/);       return; }
    if (code==RTNORM   ) { strcpy(str,"RTNORM"/*DNT*/);    return; }
    if (code==RTERROR  ) { strcpy(str,"RTERROR"/*DNT*/);   return; }
    if (code==RTCAN    ) { strcpy(str,"RTCAN"/*DNT*/);     return; }
    if (code==RTREJ    ) { strcpy(str,"RTREJ"/*DNT*/);     return; }
    if (code==RTFAIL   ) { strcpy(str,"RTFAIL"/*DNT*/);    return; }
    if (code==RTKWORD  ) { strcpy(str,"RTKWORD"/*DNT*/);   return; }

    strcpy(str,"UNKNOWN"/*DNT*/);
}
