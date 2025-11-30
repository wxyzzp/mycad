/*------------------------------------------------------------------------
$Workfile$
$Archive$
$Revision: 1.1 $
$Date: 2001/03/17 00:19:32 $
$Author: alexanderv $

$Contents: class to get glyph index of a character$
$Log: glyph.h,v $
Revision 1.1  2001/03/17 00:19:32  alexanderv
merge 14-9-2000 14-3-2001

Revision 1.2  2001/03/02 16:28:34  vitalys
kinsoku rule for CJP text wrapping

Revision 1.1  2001/01/24 13:13:19  vitalys
enable showing ideographs on win 9.x

------------------------------------------------------------------------*/

//	thanks to Eric D Crahen [crahen@cse.Buffalo.EDU] for idea.

#ifndef _GLYPH_H_
#define _GLYPH_H_

#include <windows.h>

class TTF_table {

	unsigned long SWAPLONG(unsigned long);
	unsigned short SWAPSHORT(unsigned short);

public:

	TTF_table ();
	~TTF_table();

	BOOL InitCMAP(HDC hdc, bool bVertical = false); //this function must be called after
                                                    //True Type font being attached to HDC

	int FindGlyph(int);
  
private:

  char   *cmap;                 // Copy of the cmap structure for this font.
  char   *gsub;                 // GSUB structre for this font, used for viertical glyph replacemnt
  unsigned short   count;                // Number of entries in the cmap subtable.
  unsigned short  *start;                // Start table for ranges                                
  unsigned short  *end;                  // End table for ranges
  unsigned short  *delta;                // Delta table for displacements
  unsigned short  *offset;               // Offest table for entriesinto the glyph table
  unsigned short  *glyph;                // Glyph table.
  unsigned short   vcount;               // Number of vertical replacement glyphs
  unsigned short  *from;                 // Glphys to be replaced.
  unsigned short  *to;                   // replacement glyphs.

};

#endif


