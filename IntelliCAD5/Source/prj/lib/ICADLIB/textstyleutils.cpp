/* PRJ\LIB\ICADLIB\TEXTSTYLEUTILS.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * $Revision: 1.2 $ $Date: 2001/03/16 21:48:19 $ 
 * 
 * Abstract
 * 
 * Utilities to support the handling of text styles.
 * 
 */ 


#include "icadlib.h"			/*DNT*/
#include "TextStyleUtils.h"		/*DNT*/
#include "styletabrec.h"		/*DNT*/

bool TextHasTrueTypeFont(db_handitem *text)
	{

	ASSERT(text);

	// get the text's style
	db_handitem *textStyle = NULL;
	text->get_7(&textStyle);
	ASSERT(textStyle);

	return ((db_styletabrec *)textStyle)->hasTrueTypeFont();
	}







#ifdef USE_THIS
/******************************************************************************
*+ CreateGenericTextStyle - creates a text style with default values
*
*/

bool CreateGenericTextStyle(const char *styleName, const db_drawing *drawing)
	{
	resbuf *rb;

	// build the entity's value list
	if ((rb = sds_buildlist(RTDXF0, "STYLE"/*DNT*/,
							2, styleName,
							70, 0,
							40, 0.0,
							41, 1.0,
							50, 0.0,
							71, 0,
							3, "txt.shx"/*DNT*/,
							0)) == NULL)
		return false;

	if (RTNORM != SDS_tblmesser(rb, IC_TBLMESSER_MAKE, drawing))
		{
		IC_RELRB(rb);
		return false;
		}

	IC_RELRB(rb);
	return true;
	}
#endif // USE_THIS




