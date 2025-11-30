//=====================================================================
//
//                     IntelliCAD Raster Support DLL
//   Copyright (c) 1997 by Hitachi Software Engineering America, Ltd.
//                          All Rights Reserved
//
//                  Hitachi Software Company Confidential
//
//  This code and all associated files are the proprietary property of
//  Hitachi Software.  No portion or portions may be used without the
//  express written permission of Hitachi Software.
//
//---------------------------------------------------------------------
//
//  File:  Exports.h
//  Desc:  Functions exported from the DLL
//
//  Programmer:  Geoffrey P. Vandegrift
//
//---------------------------------------------------------------------
//
//  
//  6     5/15/98 11:40a Gvandegrift
//  Added hiisDeleteRaster export.
//  
//  5     1/14/98 11:41a Gvandegrift
//  Added hiisGetHiisInstance export to allow access to a global dll info
//  class.
//  
//  4     1/07/98 4:18p Gvandegrift
//  Changed return value of hiisNewRaster to CVisioRaster * (which it
//  should have always been).
//  
//  3     11/21/97 2:48p Gvandegrift
//  Added an include.
//  
//  2     9/25/97 11:18a Gvandegrift
//  Added hiisGetRasterWriteFileFilter export.
//  
//  1     9/18/97 12:05p Gvandegrift
//  Initial revision.
//  
//=====================================================================

#ifndef __Exports_h__
#define __Exports_h__

#include "CVisioRaster.h"

// Prototypes for exported functions
char * hiisGetRasterReadFileFilter(const char *szDLLPath);
char * hiisGetRasterWriteFileFilter(const char *szDLLPath);
char * hiisGetPositionReadFileFilter(void);
char * hiisGetLastError(void);
void * hiisGetHiisInstance(void);

CVisioRaster * hiisNewRaster(void);
void hiisDeleteRaster(CVisioRaster *);

// The following functions are not exported
void setLastErrorRes(long resID);
void setLastErrorStr(char * szString);
void setLastErrorWin();

#endif // __Exports_h__
