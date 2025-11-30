
#ifndef _DWGCODEPAGE_H_
#define _DWGCODEPAGE_H_

#include "db.h"
#include "drawing.h"


DB_CLASS
int
get_dwgcodepage(            // R: code page number
    db_drawing*     pDrw    // I: drawing to get dwgcodepage
);

#endif
