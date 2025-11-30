#ifndef __DoTextH__
#define __DoTextH__

#include "DoBase.h"

class db_entity;
class db_drawing;
class CTextFragmentStyle;
class CDC;
class CDoFont;
class TTF_table;
class CTextFragmentStyle;

int getBoundingBox(db_entity* pTextEntity, CTextFragmentStyle* pTextInfo, 
				   db_drawing* pDrw, gr_view* pGrView,CDC* pDC, sds_point* pBox);

int getDisplayObjects(db_entity* pTextEntity, CTextFragmentStyle* pTextInfo, 
					  db_drawing* pDrw, gr_view* pGrView, CDC* pDC,	gr_displayobject*& pListBegin, gr_displayobject*& pListEnd);

int TTFCreateDisplayObjects(db_entity* pTextEntity, db_drawing*	pDrw, CTextFragmentStyle* pTextInfo, unsigned int FontModifiers, 
							wchar_t w_Text[], gr_view* pGrView, CDC* pDC, CDoFont* pFont, TTF_table* pTable, sds_point* TotalBoundingBox, 
							gr_displayobject** pBegDispObj, gr_displayobject** pEndDispObj);

class CDoText: public CDoBase
{
public:
    /* Subshape data: */
    struct subshapelink
	{
		char	extended; /* Flag: Extended subshape spec (extended bigfont). */
		short	code;     /* Code of chr to draw as a subshape. */
		char	basex;    /* Basepoint X (extended only). */
		char	basey;    /* Basepoint Y (extended only). */
		char	wd;       /* Width       (extended only). */
		char	ht;       /* Height      (extended only). */
    } subsh;
    /*
    **  The Current Chr Stack (for subshape processing).
    **
    **   forcependown : Used to force a pendown command under certain
    **                    circumstances (beginning of a chr or subshape,
    **                    after a pop command).
    **      whichfont : The font we're using (pTextInfo->font or pTextInfo->bigfont).
    **    thisfontchr : The chr we're working on (whichfont->chr or
    **                    &deffontchr).
    **           didx : Index into thisfontchr->def[].
    **          above : Height of a chr in font vector units
    **                    (whichfont->above or defabove).
    **        fact[2] : The factors to convert dx and dy from font vector
    **                    units to TCS units.
    */
    struct currchrlink
	{
		char			forcependown;
		db_fontlink*	whichfont;
		db_fontchrlink*	thisfontchr;
		short			didx;
		sds_real		above;
		sds_real		fact[2];

		currchrlink*	next;
    } *ccs, *tccp1;
    /*
    **  Llist of underscore and overscore toggling for this TEXT entity:
    */
    struct scorelink
	{
		char		over;	/* Score being toggled: 0=Under; 1=Over.			*/
		sds_real	tcsx;	/* TCS X-coord (on baseline (without obliquing))	*/
							/*   when toggled.  (Avoid Font Vector System,		*/
							/*   which can change with each chr.)				*/

		scorelink*	next;
    } *scorell, *pscore, *tsp1;

	CDoText();

	virtual int getDisplayObjects(gr_displayobject*& pListBegin, gr_displayobject*& pListEnd);
	virtual int getBoundingBox(CD3& minPoint, CD3& maxPoint);
	int processShapes(CTextFragmentStyle* pTextInfo, short boxonly, gr_displayobject** ppBegDispObj, gr_displayobject** ppEndDispObj, sds_point MinPoint, sds_point MaxPoint);

	static CDoText& get() { return s_theDoText; }

	// temporary function and member variable
	void setTextStyle(CTextFragmentStyle* pTextStyle) { m_pTextStyle = pTextStyle; }
private:
	CTextFragmentStyle* m_pTextStyle;
	static CDoText s_theDoText;
};

#endif
