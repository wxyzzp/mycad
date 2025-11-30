/* PRJ\LIB\DB\TEXTSTYLEUTILS.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * $Revision: 1.1 $ $Date: 2001/05/15 14:54:53 $ 
 * 
 * Abstract
 * 
 * Utilities to support the handling of text styles.
 * 
 */ 

#if !defined(_TEXTSTYLEUTILS_H)
#define _TEXTSTYLEUTILS_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "db.h"

bool TextHasTrueTypeFont(db_handitem *text);

#endif  // _TEXTSTYLEUTILS_H

