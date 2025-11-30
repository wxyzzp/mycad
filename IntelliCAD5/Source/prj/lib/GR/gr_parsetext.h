/*------------------------------------------------------------------------
$Workfile  gr_parsetext.h$
$Archive$
$Revision: 1.2.8.1 $
$Date: 2003/03/09 00:06:37 $
$Author    Vitaly Spirin$

$Contents: Header for parser that is used in TEXT and MTEXT objects$
$Log: gr_parsetext.h,v $
Revision 1.2.8.1  2003/03/09 00:06:37  master
Build 17 Part 1

Revision 1.2.10.1  2002/12/23 10:59:44  cyberage
ACIS Scripts

Revision 1.2  2001/07/05 15:26:15  alexanderv
impovements and fixes in TT text generation

Revision 1.2  2001/06/29 10:17:57  denisp
impovements and fixes in TT text generation

Revision 1.1  2000/12/27 13:43:15  vitalys
Fixed bugs concerning and lowlights localization.

------------------------------------------------------------------------*/

#include "fontlink.h"

#ifndef _GRPARSETEXT_H
#define _GRPARSETEXT_H

#define MAX_TEXT_LENGTH 200
class TTF_table;
/*-------------------------------------------------------------------------*//** 
 Parser that is used in TEXT and MTEXT objects.

@author Vitaly Spirin
@param  output
    _TCHAR**                cpp,    // IO: String of text that should be parsed
@param  input
    _TCHAR**                cpp,    // IO: String of text that should be parsed
    db_fontlink::FontType   type    // I:  Font type. May be:
                                    // SHAPE_1_0, SHAPE_1_1, UNIFONT, BIGFONT, TRUETYPE
@return                             // R:  Unicode of character of control code
*//*--------------------------------------------------------------------------*/ 

long 
gr_parsetext(                       // R:  Unicode of character of control code
    _TCHAR**                cpp,    // IO: String of text that should be parsed
    UINT                    dwg_code_page,  // for converting from multibyte to Unicode
    db_fontlink::FontType   type,    // I:  Font type. May be: SHAPE_1_0, SHAPE_1_1, UNIFONT, BIGFONT, TRUETYPE
	TTF_table* pFontTable = NULL
);


#endif
