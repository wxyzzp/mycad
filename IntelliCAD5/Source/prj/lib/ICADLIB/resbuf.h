/* PRJ\LIB\ICADLIB\RESBUF.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * $Revision: 1.1.1.1 $ $Date: 2000/01/19 13:41:38 $ 
 * 
 * Abstract
 * 
 * Utility functions for dealing with resbufs.
 * 
 */ 

#if !defined(_RESBUF_H)
#define _RESBUF_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "sds.h"

void appendRbLists(resbuf **list1, resbuf **list2);
resbuf	*DataList(resbuf **rbb, int type, const void *value);
resbuf	*BuildDataList(resbuf **rbb, int type, const sds_point value);
resbuf	*BuildDataList(resbuf **rbb, int type, const sds_real value);
resbuf	*BuildDataList(resbuf **rbb, int type, const int value);
resbuf	*BuildDataList(resbuf **rbb, int type, const long value);
resbuf	*BuildDataList(resbuf **rbb, int type, const char *value);




#endif // _RESBUF_H







