/* C:\ICAD\PRJ\LIB\DB\IMAGEDEF.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "db.h"
#include "objects.h"
#include "IRasDef.h"
class CDC;
#include "CVisioRaster.h"

/***************************** db_imagedef **************************/

db_imagedef::db_imagedef(void) : db_handitem(DB_IMAGEDEF) {
    deleted=0; looked4image=0;
    filepn=NULL; imagesz[0]=imagesz[1]=dxf11[0]=dxf11[1]=0.0;
    dxf90=0; dxf280=dxf281=0; imagedisp=NULL;
}
db_imagedef::db_imagedef(const db_imagedef &sour) :
    db_handitem(sour) {  /* Copy constructor */

    deleted     =sour.deleted;

    looked4image=sour.looked4image;

    filepn=NULL; db_astrncpy(&filepn,sour.filepn,-1);
    imagesz[0]  =sour.imagesz[0];
    imagesz[1]  =sour.imagesz[1];
    dxf11[0]    =sour.dxf11[0];
    dxf11[1]    =sour.dxf11[1];
    dxf90       =sour.dxf90;
    dxf280      =sour.dxf280;
    dxf281      =sour.dxf281;

    imagedisp   =NULL;  /* Don't copy this one. */
}

//extern void _cdecl deletetheraster(void *imagedisp);
static HINSTANCE Ras_hDLL;
static void (*Ras_deleteRaster)(CVisioRaster *)=NULL;

db_imagedef::~db_imagedef(void) {
	delete [] filepn;
	if (!Ras_deleteRaster) {
		Ras_hDLL= LoadLibrary("HiisRas.dll");
		Ras_deleteRaster = (void(*)(CVisioRaster *))GetProcAddress(Ras_hDLL, "hiisDeleteRaster");
	}
	if (Ras_deleteRaster) (*Ras_deleteRaster)((CVisioRaster *)imagedisp);
 }


