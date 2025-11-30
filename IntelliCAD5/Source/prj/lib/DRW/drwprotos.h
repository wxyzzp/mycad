/* C:\ICAD\PRJ\LIB\DRW\DRWPROTOS.H
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

class drw_readwrite;

//**** Prototypes
void drw_progress(short perc);
short drw_criticalerror(short err);
void drw_findfile(char *shapefile,char *newshapefile,void *userdata);
