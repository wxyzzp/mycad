/*------------------------------------------------------------------------
$Workfile  gr_wordwrap.h$
$Archive$
$Revision: 1.1 $
$Date: 2001/03/17 00:19:32 $
$Author    Vitaly Spirin$

$Contents: Declarations of functions to process CJK text wrapping$
------------------------------------------------------------------------*/

#ifndef GR_WORDWRAP_H
#define GR_WORDWRAP_H

#include "gr.h"

char*
cjk_wordwrap(						// R: new string with slashes in the places where it can be wrapped
unsigned char*  	str_origin,		// I: string that has to be wrapped
db_drawing*		    pDrawing		// I: drawing (to get dwgcodepage)
);

#define ORDINARY_CHARACTER		0
#define LEADING_CHARACTER		1
#define FOLLOWING_CHARACTER		2

int
cjk_wordwrap_exceptions(		// R: ORDINARY_CHARACTER or LEADING_CHARACTER or FOLLOWING_CHARACTER
	UINT			code_page,	// I: code page for processing
	unsigned char*	mb_string	// I: multibyte code
);

#endif
