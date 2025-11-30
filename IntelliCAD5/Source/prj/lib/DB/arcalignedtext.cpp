/* C:\ICAD\PRJ\LIB\DB\ARCALIGNEDTEXT.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "db.h"

/**************************** db_arcalignedtext *****************************/


/*F*/
db_arcalignedtext::db_arcalignedtext(void) : db_entity_with_extrusion(DB_ARCALIGNEDTEXT) {
	text[0]=fontname[0]=bigfontname[0]=0;
	textstyleobjhandle=NULL;
	pt0[0]=pt0[1]=pt0[2]=0.0;
	radius=0.0;
	widthfactor=1.0;
	height=0.2;
	charspacing=1.0;
	offsetfromarc=0.0;
	rightoffset=0.0;
	leftoffset=0.0;
	startangle=0.0;
	endangle=0.0;
	charorder=0;
	direction=0;
	alignment=0;
	side=0;
	bold=0;
	italic=0;
	underline=0;
	charset=0;
	pitch=0;
	fonttype=0;
	color=0;
	wizardflag=0;
	grblob=NULL;
	arcobjhandle=NULL;
}

db_arcalignedtext::db_arcalignedtext(db_drawing *dp) : db_entity_with_extrusion(DB_ARCALIGNEDTEXT,dp) {
	text[0]=fontname[0]=bigfontname[0]=0;
	textstyleobjhandle=NULL;
	pt0[0]=pt0[1]=pt0[2]=0.0;
	radius=0.0;
	widthfactor=1.0;
	height=0.2;
	charspacing=1.0;
	offsetfromarc=0.0;
	rightoffset=0.0;
	leftoffset=0.0;
	startangle=0.0;
	endangle=0.0;
	charorder=0;
	direction=0;
	alignment=0;
	side=0;
	bold=0;
	italic=0;
	underline=0;
	charset=0;
	pitch=0;
	fonttype=0;
	color=0;
	wizardflag=0;
	grblob=NULL;
	grblobsz=0;
	arcobjhandle=NULL;
}

db_arcalignedtext::db_arcalignedtext(const db_arcalignedtext &sour) :
    db_entity_with_extrusion(sour) {  /* Copy constructor */

	strcpy(text,sour.text);
    strcpy(fontname,sour.fontname);
	strcpy(bigfontname,sour.bigfontname);
	textstyleobjhandle=sour.textstyleobjhandle;
	ic_ptcpy(pt0,sour.pt0);
    radius=sour.radius;
	widthfactor=sour.widthfactor;
	height=sour.height;
	charspacing=sour.charspacing;
	offsetfromarc=sour.offsetfromarc;
	rightoffset=sour.rightoffset;
	leftoffset=sour.leftoffset;
	startangle=sour.startangle;
	endangle=sour.endangle;
	charorder=sour.charorder;
	direction=sour.direction;
	alignment=sour.alignment;
	side=sour.side;
	bold=sour.bold;
	italic=sour.italic;
	underline=sour.underline;
	charset=sour.charset;
	pitch=sour.pitch;
	fonttype=sour.fonttype;
	color=sour.color;
	wizardflag=sour.wizardflag;
	grblobsz=sour.grblobsz;
	grblob=NULL;
    if (grblobsz>0 && sour.grblob!=NULL) {
        grblob=new char[grblobsz];
        memcpy(grblob,sour.grblob,grblobsz);
    }
	}

db_arcalignedtext::~db_arcalignedtext(void) {

    delete [] grblob;
}
