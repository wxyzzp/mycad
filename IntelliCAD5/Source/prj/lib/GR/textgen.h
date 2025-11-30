#ifndef __textgenH__
#define __textgenH__

#include "sds.h"

#define STYLENUMBER_BOLD		0x02000000L
#define STYLENUMBER_ITALIC		0x01000000L
#define STYLENUMBER_CHAR_MASK	0x0000FF00L
#define STYLENUMBER_PITCH_MASK	0x000000FFL

class db_handitem;
class db_entity;
class db_drawing;
struct db_fontlink;
struct gr_view;
struct gr_displayobject;
class CDC;
/*
**	This is for anything that calls gr_textgen().  We used to pass
**	all the parameters separately.	Can't use a specific entity's
**	class, because the text-like entities differ slightly.
**	Also, TOLERANCE introduced the concept of a font disassociated
**	from a text style.	gr_getftextfrags() uses this in a llist for a
**	parameters stack, and struct gr_ftextfraglink also contains one.
**
**	Note that color is not present; gr_textgen() gets it from
**	the gr_view (viewp->color).
*/
class CTextFragmentStyle;
typedef CTextFragmentStyle gr_textgenparslink;
class CTextFragmentStyle
{
public:
	enum EModifiers
	{
		eUNKNOWN = 0,
		eOVERLINED = 0x0001,
		eUNDERLINED = 0x0002,
		eBOLD = 0x0010,
		eITALIC = 0x0020,
	};
public:
	CTextFragmentStyle() :	m_modifiers(0), m_color(0), m_alignment(0), m_rotation(0.0), m_spaceFactor(0.0),
							m_height(0.0), m_obliqueAngle(0.0), m_widthFactor(0.0), m_pFont(NULL), val(NULL),
							gen(0), vert(0), dontDrawGDITrueType(false), bigfont(NULL), absxsclp(1.0), m_bReorderFlag(false)
							, m_lastspace(0)
	{
		memset(m_insertionPoint, 0, sizeof(m_insertionPoint));
	}
	// members for compatibility
	char		*val;
	int 		 gen;
	int 		 vert;	   /* Vertical (STYLE flags (70), bit 2 (0x04) or Mtext 72 == 3) */

	bool dontDrawGDITrueType;	// needed at times to pass flag between functions
	sds_real	 absxsclp; /* 1.0 -- or a value set by gr_getdispobjs(), */
						   /* where scaling for nested INSERTs is tracked. */

	// base members
	union
	{
		sds_point llpt;
		sds_point m_insertionPoint;
	};
	union
	{
		sds_real rot;
		sds_real m_rotation;
	};

	int m_modifiers;
	int m_color;
	int m_alignment;
	sds_real m_spaceFactor;
	long m_stylenumber;		// EBATECH(CNBR)
	int m_lastspace;		// EBATECH(CNBR) 0:zero 1:any

	union
	{
		sds_real m_height;
		sds_real ht;
	};
	union
	{
		sds_real m_obliqueAngle;
		sds_real obl;
	};
	union
	{
		sds_real m_widthFactor;
		sds_real wfact;
	};
	union
	{
		db_fontlink* m_pFont;
		db_fontlink *font;	   /* Don't free */
	};
	union
	{
		db_fontlink *bigfont;  /* Don't free */
		db_fontlink *m_pBigfont;  // EBATEC(CNBR) Add synonim
	};

	bool	m_bReorderFlag;		// 'true' if the next fragment (after this one) of mtext is moved (reordered) to somewhere else
};

int gr_getboundingBox(
	db_handitem			*texthip,
	gr_textgenparslink	 par,
	sds_point			 bottomLeft,
	sds_point			 upperRight,
	db_drawing			*p_dp,
	gr_view		*viewp,
	CDC					*dc);

short gr_textgen(
  /* Given: */
	db_entity				 *pSourceEntity,
	gr_textgenparslink		 *par,
	short					  boxonly,
	db_drawing				 *dp,
	gr_view 		  *viewp,
	CDC						 *dc,
  /* Set by this function: */
	gr_displayobject **begpp,
	gr_displayobject **endpp,
	sds_point				  p1,
	sds_point				  p2);

short gr_getfontchr(db_drawing* pDrawing,
    char                  **valpp,
    struct db_fontlink     *font,
    struct db_fontlink     *bigfont,
    struct db_fontchrlink **fontchrpp,
    struct db_fontlink    **whichfontpp);

#endif
