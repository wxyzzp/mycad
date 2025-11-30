/* C:\DEV\PRJ\ICAD\ICADRASTER.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Function declarations for raster support.
 * 
 */ 

int Ras_OpenDialog(CString strPathName);
int Ras_PaintRaster(CIcadView *pView,db_handitem *elp,struct gr_view *gView,int drawmode,int color);
int Ras_InitRaster(void);
void Ras_Unloadpixelmapbuffer(db_handitem *elp);
void Ras_Reloadpixelmapbuffer(db_handitem *elp, const char* imageName, const char *szDLLPath);

int Ras_GetDisplayImageFramesFlag();
int Ras_GetRasterQuality();

