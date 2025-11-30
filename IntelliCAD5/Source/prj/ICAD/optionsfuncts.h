/* 
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * General functions that are used for options dialog
 * 
 */ 

#ifndef _OPTIONSFUNCTS_H
#define _OPTIONSFUNCTS_H

//Draws a color button with a color swatch
//Doesn't handle disabled buttons since this feature is not needed currently.
// If checkBackgrd is true it will paint the color specified no matter what the background is.
// If checkBackgrd is false the XORed color will be used when the background and color are the same.
void DrawColorButton (LPDRAWITEMSTRUCT lpDrawItemStruct, CWnd *window, int id, int color, bool checkBackgrd);

// Paints a color swatch on the left side of a color button
// If checkBackgrd is true it will paint the color specified no matter what the background is.
// If checkBackgrd is false the XORed color will be used when the background and color are the same.
void PaintColorSwatch (CWnd *window, int id, int color, bool checkBackgrd);

#endif

