/* D:\ICADDEV\PRJ\LIB\ICADLIB\STRNCPY.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * $Revision: 1.1.1.1 $ $Date: 2000/01/19 13:41:38 $ 
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "icadlib.h"/*DNT*/

void ic_strncpy( LPTSTR string1, LPCTSTR string2, size_t i1)  {
    size_t fi1;
    for(fi1=0; fi1<i1 && string2[fi1]; ++fi1) {
       string1[fi1]=string2[fi1];
    }
    string1[fi1]='\0'/*DNT*/;
}

LPTSTR ic_strupr( LPTSTR ss)
{
	_tcsupr( ss);

	return ss;
}


