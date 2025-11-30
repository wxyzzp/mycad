/* C:\ICAD\PRJ\LIB\DB\RTEXT.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "db.h"

/**************************** db_rtext *****************************/


/*F*/
db_rtext::db_rtext(void) : db_entity_with_extrusion(DB_RTEXT) 
{
	pt0[0]=pt0[1]=pt0[2]=0.0;
	rotang=0.0;
	height=0.2;
	textstyleobjhandle=NULL;
	typeflag=0;
	contents[0]=0;
}

db_rtext::db_rtext(db_drawing *dp) : db_entity_with_extrusion(DB_RTEXT,dp) 
{
	pt0[0]=pt0[1]=pt0[2]=0.0;
	rotang=0.0;
	height=0.2;
	textstyleobjhandle=NULL;
	typeflag=0;
	contents[0]=0;
}

db_rtext::db_rtext(const db_rtext &sour) :
    db_entity_with_extrusion(sour) 
{  /* Copy constructor */

	ic_ptcpy(pt0,sour.pt0);
    rotang=sour.rotang;
	height=sour.height;
	textstyleobjhandle=sour.textstyleobjhandle;
	typeflag=sour.typeflag;
	strcpy(contents,sour.contents);
	}

db_rtext::~db_rtext(void) 
{
}
