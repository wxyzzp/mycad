
#pragma warning (disable : 4786)

#include "gr.h"
#include "DoText.h"
#include "DoFont.h"
#include "gr_view.h"
#include "pc2dispobj.h"
#include "textgen.h"
#include "truetypeutils.h"
#include "gr_parsetext.h"
#include "dwgcodepage.h"

extern gr_view gr_defview;

CDoText CDoText::s_theDoText;

CDoText::CDoText(): m_pTextStyle(NULL)
{
}

int CDoText::getBoundingBox(CD3& minPoint, CD3& maxPoint)
{
	int result = 1;
	CTextFragmentStyle textStyle;
	CTextFragmentStyle* pTextInfo = &textStyle;
	sds_point box[2];
	/*D.G.*/// The Old TTF text output method:
	// If GDI can output TrueType text, we don't vectorize the text. Instead, we create a text box
	// for the normal display objects and also keep the actual text fragment for outputting.
	// The New method: see my comments in the truetypeutils.cpp file.
	bool haveGDITrueTypeText, haveTrueTypeFont;
	haveTrueTypeFont = false;
	haveGDITrueTypeText = TrueTypeTextForGDI(m_pEntity, m_pViewData, &haveTrueTypeFont);

	// this test was included as an interim solution since we decided not to handle the %% and other codes
	// in text and mtext; it can be removed once IntelliCAD supports the various codes
	if(m_pTextStyle->dontDrawGDITrueType)
		haveGDITrueTypeText = false;

	// Check if the text is in a block insert and insertion rotation is not zero.
	//if(m_pViewData->recurlevel > 1 && m_pViewData->absrot)	// TO DO:
	//	haveGDITrueTypeText = false;				// Remove it implementing correct transformation.
	if(haveGDITrueTypeText)
	{
		if(::getBoundingBox(m_pEntity, m_pTextStyle, m_pDrawing, m_pViewData, m_pDC, box) == 0)
			result = 0;
	}
	else
	{
		// If we got this far, we're not dealing with TrueType text
		// that can be output by GDI.  This means we have to vectorize
		// the text.
		
		// we may have a TrueType font that couldn't be displayed with GDI;
		// if so, substitute txt.shx for it
		if(haveTrueTypeFont)
		{
			char szFontName[IC_ACADBUF];
			strcpy(szFontName, "txt.shx"/*DNT*/);
			db_fontlink*	tmpFontLink = NULL; // DO NOT FREE THIS, "NEVER".
			if(db_setfontusingmap(szFontName, NULL, &tmpFontLink, IC_ALLOW_ICAD_FNT, m_pDrawing) >= 0)
				m_pTextStyle->font = tmpFontLink;
		}
		if(processShapes(m_pTextStyle, 1, NULL, NULL, box[0], box[1]) != 0)
			result = 0;
	}

	GR_PTCPY(minPoint, box[0]);
	GR_PTCPY(maxPoint, box[1]);
	minPoint[2] = maxPoint[2] = 0.0;
	
	/* Adjust for backward and/or upside down for the caller.	*/
	/* (For our use in here, we don't need to because we use	*/
	/* the TCS axes, which we adjusted earlier.):				*/
	sds_real ar1;
	if(pTextInfo->gen & IC_TEXT_GEN_BACKWARD)		/* Backward; swap and negate X's.	*/
	{
		ar1 = minPoint[0];
		minPoint[0] = - maxPoint[0];
		maxPoint[0] = - ar1;
	}
	if(pTextInfo->gen & IC_TEXT_GEN_UPSIDEDOWN)		/* Upside down; swap and negate Y's.*/
	{
		ar1 = minPoint[1];
		minPoint[1] = - maxPoint[1];
		maxPoint[1] = - ar1;
	}
	return result;
}

int CDoText::getDisplayObjects(gr_displayobject*& pListBegin, gr_displayobject*& pListEnd)
{
	db_handitem* pTextStyle = NULL;
	m_pEntity->get_7(&pTextStyle);
	if(pTextStyle == NULL && 
		(m_pEntity->ret_type() == DB_TEXT || 
		m_pEntity->ret_type() == DB_ATTRIB ||
		m_pEntity->ret_type() == DB_ATTDEF ||
		m_pEntity->ret_type() == DB_MTEXT))
		return 0;

	int result = 1;
    int savltsclwblk;
    sds_real tcsaxis[2][2], TanObl;
	sds_point box[2];

    /* Trick gr_gensolid() into doing the gr_ncs2ucs() calls	*/
    /* so that we can work with a font vector system pc chain	*/
    /* (and later a NCS pc chain) in this function:				*/
    savltsclwblk = m_pViewData->ltsclwblk;
	m_pViewData->ltsclwblk = 1;

	/*D.G.*/// The Old TTF text output method:
	// If GDI can output TrueType text, we don't vectorize the text. Instead, we create a text box
	// for the normal display objects and also keep the actual text fragment for outputting.
	// The New method: see my comments in the truetypeutils.cpp file.
	bool haveGDITrueTypeText, haveTrueTypeFont;
	haveTrueTypeFont = false;
	haveGDITrueTypeText = TrueTypeTextForGDI(m_pEntity, m_pViewData, &haveTrueTypeFont);

	// this test was included as an interim solution since we decided not to handle the %% and other codes
	// in text and mtext; it can be removed once IntelliCAD supports the various codes
	if(m_pTextStyle->dontDrawGDITrueType)
		haveGDITrueTypeText = false;

	// Check if the text is in a block insert and insertion rotation is not zero.
	//if(m_pViewData->recurlevel > 1 && m_pViewData->absrot)	// TO DO:
	//	haveGDITrueTypeText = false;				// Remove it implementing correct transformation.

	if(haveGDITrueTypeText)
	{
		if(m_pViewData->qtextmode)
		{
			CD3 p1, p2;
			if(getBoundingBox(p1, p2) == 0)
				result = 0;
			memcpy(box[0], (double*)p1, 3 * sizeof(double));
			memcpy(box[1], (double*)p2, 3 * sizeof(double));
		}
		else
		{
			if(::getDisplayObjects(m_pEntity, m_pTextStyle, m_pDrawing,	m_pViewData, m_pDC, pListBegin, pListEnd) == 0)
				result = 0;
		}
	}
	else
	{
		// If we got this far, we're not dealing with TrueType text
		// that can be output by GDI.  This means we have to vectorize
		// the text.
		
		// we may have a TrueType font that couldn't be displayed with GDI;
		// if so, substitute txt.shx for it
		if(haveTrueTypeFont)
		{
			char szFontName[IC_ACADBUF];
			strcpy(szFontName, "txt.shx"/*DNT*/);
			db_fontlink*	tmpFontLink = NULL; // DO NOT FREE THIS, "NEVER".
			if(db_setfontusingmap(szFontName, NULL, &tmpFontLink, IC_ALLOW_ICAD_FNT, m_pDrawing) >= 0)
				m_pTextStyle->font = tmpFontLink;
		}

		if(m_pViewData->qtextmode)
		{
			if(processShapes(m_pTextStyle, 1, NULL, NULL, box[0], box[1]) == 0)
				result = 0;
		}
		else
		{
			if(processShapes(m_pTextStyle, 0, &pListBegin, &pListEnd, NULL, NULL) != 0)
				result = 0;
		}
	}

	if(m_pViewData->qtextmode && result)
	{
		sds_point ap1;
		TanObl = m_pTextStyle->obl;
		ic_normang(&TanObl, NULL);
		if(fabs(TanObl - IC_PI * 0.5) >= IC_ZRO && fabs(TanObl - IC_PI * 1.5) >= IC_ZRO)
			TanObl = tan(TanObl);
		else
			TanObl = tan(TanObl - IC_ZRO);

		// Define the TCS axes according to the ECS:
		tcsaxis[0][0] = cos(m_pTextStyle->rot);
		tcsaxis[0][1] = sin(m_pTextStyle->rot);
		tcsaxis[1][0] = - tcsaxis[0][1];
		tcsaxis[1][1] = tcsaxis[0][0];
		
		if(m_pTextStyle->gen & IC_TEXT_GEN_BACKWARD)		// Backward; flip the X-axis.
		{
			tcsaxis[0][0] = - tcsaxis[0][0];
			tcsaxis[0][1] = - tcsaxis[0][1];
		}
		if(m_pTextStyle->gen & IC_TEXT_GEN_UPSIDEDOWN)		// Upside down; flip the Y-axis.
		{
			tcsaxis[1][0] = - tcsaxis[1][0];
			tcsaxis[1][1] = - tcsaxis[1][1];
		}
		// Use box[] and m_pViewData->pc to gen the box.
		if(m_pViewData->npc < 5 && m_pViewData->allocatePointsChain(5))
			return 0;

		m_pViewData->upc = 5;
		m_pViewData->lupc = m_pViewData->pc;

		for(int i = 0; i < 4; i++, m_pViewData->lupc = m_pViewData->lupc->next)
		{
			m_pViewData->lupc->d2go = -1.0;
			m_pViewData->lupc->swid = m_pViewData->lupc->ewid = 0.0;
			m_pViewData->lupc->startang = m_pViewData->lupc->endang = IC_ANGLE_DONTCARE;

			switch(i)
			{
				case 0: GR_PTCPY(ap1, box[0]);
						break;
				case 1: ap1[0] = box[0][0];
						ap1[1] = box[1][1];
						break;
				case 2: GR_PTCPY(ap1, box[1]);
						break;
				case 3: ap1[0] = box[1][0];
						ap1[1] = box[0][1];
			}

			// transform from TCS to NCS and set the pc pt
			// TO DO: make correct transformation for all cases (rotated in 3D etc.)
			ap1[0] += ap1[1] * TanObl;

			m_pViewData->lupc->pt[0] = ap1[0] * tcsaxis[0][0] +	ap1[1] * tcsaxis[1][0] + m_pTextStyle->llpt[0];
			m_pViewData->lupc->pt[1] = ap1[0] * tcsaxis[0][1] + ap1[1] * tcsaxis[1][1] + m_pTextStyle->llpt[1];
			m_pViewData->lupc->pt[2] = m_pTextStyle->llpt[2];
		}
		m_pViewData->lupc->d2go = -1.0;
		m_pViewData->lupc->swid = m_pViewData->lupc->ewid = 0.0;
		m_pViewData->lupc->startang = m_pViewData->lupc->endang = IC_ANGLE_DONTCARE;
		GR_PTCPY(m_pViewData->lupc->pt, m_pViewData->pc->pt);

		setPointsData(m_pViewData->pc, m_pViewData->lupc, m_pViewData->pc->next, m_pViewData->upc - 2);
		setFlags(1, 0);
		setDotDir(NULL);
		if(CDoBase::getDisplayObjects(pListBegin, pListEnd) != 0) 
			result = 0;
	}
    if(!result)
	{
		gr_freedisplayobjectll(pListBegin);
		pListBegin = pListEnd = NULL;
	}
    m_pViewData->ltsclwblk = savltsclwblk;

	return result;
}

int CDoText::processShapes(gr_textgenparslink* pTextInfo, short boxonly, gr_displayobject** ppBegDispObj, gr_displayobject** ppEndDispObj, sds_point minPoint, sds_point maxPoint)
{
/*
**  An extension of gr_getdispobjs() and gr_textbox().
**
**  Uses m_pViewData's "gr_conv" items, but only ->pc and related members and
**  ->m_pViewData. (We're careful to use m_pViewData only when boxonly==0 -- that is,
**  when gr_getdispobjs() has been called to actually generate text.
**  The members set by gr_getdispobjs() should be avoided during calls
**  where boxonly!=0 because gr_getdispobjs() has not been called.)
**
**  Allocates a display objects llist for the TEXT currently being created
**  via gr_gensolid(). *ppBegDispObj is pointed to the first link;
**  *ppEndDispObj is pointed to the last link.
**
**  TCS refers to the Text Coordinate System.  Text always starts from
**  (0,0) and is at rotation 0 and generates in the positive X-direction.
**  Scaling and obliquing are applied in the TCS.  (Textbox coordinates
**  are in the TCS.)
**
**  The text parameters are passed (instead of coming from m_pViewData->ment)
**  to make it easier to use this function for ATTDEF, ATTDEF, and
**  gr_textbox().
**
**  pTextInfo->absxsclp is the absolute value of the cumulative X-scale factor
**  (for nested INSERTs).  gr_getdispobjs() passes m_pViewData->absxsclp,
**  but gr_textbox() (and any other functions that can't count on
**  gr_getdispobjs() setting it) pass 1.0.
**
**  If boxonly!=0, minPoint and maxPoint are set -- but the display objects
**  are not generated.  HOWEVER -- the pc chain IS generated.
**  The box is determined by examining the finished pc chains,
**  just before the calls to gr_gensolid() -- not from curpt as we go.
**  The curpt method didn't work: a pendown followed by a penup
**  with no drawing generated no line work, but tricked the textbox
**  adjuster into taking the pendown point.
**
**  minPoint and maxPoint are for the "textbox" corners; they are set to the
**  lower left and upper right corners of the enclosing text box
**  in the TCS (if they are non-NULL).  AutoLISP textbox returns 3D points
**  (and ADS uses sds_points), but it seems the Z's are not used
**  and are always set to 0.0 -- so, we're following suit.  (The alternative
**  would be to use the thickness (scaled properly).)
**
**  Once we have a pt in the TCS it can be transformed to the ECS
**  via the TCS axes defined in the ECS.  The TCS axes are defined
**  in ECS coordinates such that they have the proper rotation and
**  account for the "upside down" and "backward" flags.
**
**  For arcs, we trick gr_arc2pc() into adding arc segments
**  (in font vector units, scaled only by the vector factor vfact) to
**  the end of the pc chain.  Then we walk them and scale them (using
**  fact[0] and fact[1]) which applies the width factor, and use the
**  results to adjust the textbox. Then we convert them to the NCS.
**
**  SUBSHAPES:
**
**    I can think of 2 ways to do this: recursion or tricking the def
**    byte loop into working on another chr's def bytes for a while.  I
**    don't like either, but I think the latter may be simpler.  For
**    recursion, I'd have to break out the "each chr" part of this
**    function into its own function, pass a lot of this function's
**    locals/params to it, figure out which variables need to survive
**    each call and which don't, chain together the display objects
**    generated by each recursion, etc.  Perhaps that wouldn't have been
**    as big a deal if I had written this function with all that in mind
**    to begin with.
**
**    As it is, I think it's probably easier to go with a "stack" method
**    that makes it look like we're splicing in some extra def bytes
**    when they're needed.  First, start with the original chr's
**    font/chr data in the only link of a llist stack.  When a subshape
**    command comes along, create a new top link for that subshape and
**    work on it until it's done -- then chuck that link and continue
**    working on the one beneath it.  The key is to preserve all of the
**    font/chr-specific data on the stack; we can see those variables in
**    the code between the start of the each-chr loop and the start of
**    the each-def-byte loop (because that's where we're setting up for
**    a specific chr's def bytes).
**
**    This seems to work in simple cases, but we still have a problem
**    with NESTED subshapes.  Try this:
**
**      In a copy of isocp.shp, comment out the 'B' and replace
**      it with the following (a 'B' which subshapes 'C'):
**
**        *042,28,B
**        2,8,(6,2),1,0A0,10,(10,100),0A8,2,0A0,1,10,(8,100),7,043,0A8,
**        8,(0,-36),2,8,(26,-2),0
**
**      In a test bigfont (with 7E in the escape chr range), add this
**      chr (which is isocp's 'A' with a subshape-B call in it):
**
**        *07eff,30,A
**        2,8,(6,2),1,8,(12,36),7,042,8,(12,-36),3,2,2,8,(-7,20),1,
**        8,(-34,0),2,8,(53,-24),4,2,0
**
**        (The header was
**          *BIGFONT 46,1,126,126
**          *0,4,SPECIALS
**          21,7,2,0
**        )
**
**      Now make a TEXT with value |7E|FF| ("\\U+007E\\U+00FF").
**      It's an 'A' that subshapes a 'B' that subhapes 'C', but some of
**      our line work doesn't match ACAD's.
**
**  Minimal error-checking is done here; that's up to gr_getdispobjs().
**
**  Interesting ACAD bug:  While testing our 9-command processing, I
**  found a case where OURS works and ACAD's doesn't. I doctored '&'
**  and '0' in complex.shp by adding a 14 (do next command only if
**  vertical) in front of a 9 (multiple vectors) in each chr.  ACAD's
**  generator skips all the 9-vectors -- PLUS THE COMMAND THAT FOLLOWS
**  IT.  ACAD has the same problem with the other repeater: the 13-command.
**
**  I also noticed that complex.shp's '0' is missing a vector
**  in its definition.
**
**  Returns: gr_getdispobjs() codes.
*/
// ----------------------------------------------------------
//  EbaTech(Japan). modifies to display extended subshapes.
//  EXTENDED SUBSHAPES:
// ----------------------------------------------------------
    char*			valp;
    unsigned char	vlen, vdir;
    short			skip, vertonly, gotdxdy, arcmode, rc;
    short			pendown, repmode, done, psidx, pssz, cw, fi1;
    short			circ, genpc, cmdcode, scoretog;
    long			pcidx, fl1;
    // EBATECH(CNBR) -[ Extended Subshapes:
    //sds_real		ps[4][2], tcsaxis[2][2], vfact, dx, dy, TanObl;
    sds_real		ps[4][2], tcsaxis[2][2], vfactx, vfacty, savex, savey, dx, dy, TanObl;
	struct db_fontlink *wf;      /* whichfont */
	struct db_fontchrlink *tfc;  /* thisfontchr */
    // ]- EBATECH(CNBR)
    sds_real		rad, sa, ea, iang, bulge, defabove, oldabove, wabsxsclp, ar1;
	sds_point		box[2];
    sds_point		curpt, endpt, ap1;
    gr_displayobject *begdo, *enddo, *tdop1, *tdop2;
    db_fontchrlink	deffontchr;
    gr_pclink*		tpcp1;
	bool			bGotBox;

	// boxonly should always be 1 if minPoint and maxPoint aren't NULL
	// or always 0 if *ppBegDispObj and *ppEndDispObj aren't NULL; and
	// so far, we never get both display objects and points
	// at the same time
	ASSERT((minPoint && maxPoint && boxonly) || (ppBegDispObj && ppEndDispObj && !boxonly));

	ASSERT((minPoint && maxPoint && !ppBegDispObj && !ppEndDispObj) ||
		   (!minPoint && !maxPoint && ppBegDispObj && ppEndDispObj));

	//
	// **************************** DATA FOR NON-TTF TEXT *******************************
	//
    /* Default chr definition (for deffontchr): */
    char	defdef[] = {2, 14, 3, 2, 14, 8, -3, -12, 14, 4, 2, 0x54, 1, 0x12, 0x10, 0x1E, 0x1C, 0x1A, 0x1C,
						2, 0x1C, 1, 0x1C, 2, 0x30, 14, 3, 2, 14, 8, -7, -6, 14, 4, 2, 0};

    /* Subshape data: */
    subshapelink subsh;
    /*
    **  The Current Chr Stack (for subshape processing).
    */
    currchrlink *ccs, *tccp1;
    /*
    **  Llist of underscore and overscore toggling for this TEXT entity:
    */
	scorelink *scorell, *pscore, *tsp1;
	/***** Things to do before the first goto out: *****/

    ccs = NULL;
	scorell = pscore = NULL;

    rc = 0;
	bGotBox = false;
	begdo = enddo = NULL;

    if(minPoint)
		minPoint[0] = minPoint[1] = minPoint[2] = 0.0;
    if(maxPoint)
		maxPoint[0] = maxPoint[1] = maxPoint[2] = 0.0;

    if(!m_pViewData || icadRealEqual(m_pViewData->viewsize,0.0))
		m_pViewData = &gr_defview;

	box[0][2] = box[1][2] = 0.0; // EBATECH(CNBR) Avoid to copy uninitialized value.
    box[1][0] = box[1][1] = - (box[0][0] = box[0][1] = 1.0 / IC_ZRO);

    if(!pTextInfo->val || !*pTextInfo->val)
		goto out;

    if(pTextInfo->val[0] == '%' && pTextInfo->val[1] == '%' && !pTextInfo->val[2])
		goto out;

	//
	// ***************** INITIALIZATION BOTH FOR TTF & NON-TTF CASES ********************
	//
    /* Keep obliquing angle away from 90 and 270 deg (where tangent */
    /* is undefined): */
	TanObl = pTextInfo->obl;
	ic_normang(&TanObl, NULL);
	if(fabs(TanObl - IC_PI * 0.5) >= IC_ZRO && fabs(TanObl - IC_PI * 1.5) >= IC_ZRO)
		TanObl = tan(TanObl);
	else
		TanObl = tan(TanObl - IC_ZRO);

    // Define the TCS axes according to the ECS:
    tcsaxis[0][0] = cos(pTextInfo->rot);
	tcsaxis[0][1] = sin(pTextInfo->rot);
    tcsaxis[1][0] = - tcsaxis[0][1];
	tcsaxis[1][1] = tcsaxis[0][0];

    if(pTextInfo->gen & IC_TEXT_GEN_BACKWARD)		// Backward; flip the X-axis.
	{
		tcsaxis[0][0] = - tcsaxis[0][0];
		tcsaxis[0][1] = - tcsaxis[0][1];
	}
    if(pTextInfo->gen & IC_TEXT_GEN_UPSIDEDOWN)		// Upside down; flip the Y-axis.
	{
		tcsaxis[1][0] = - tcsaxis[1][0];
		tcsaxis[1][1] = - tcsaxis[1][1];
	}

	//
	// ************************ INITIALIZATION FOR NON-TTF CASE ****************************
	//

    // Start the pc list with 100 links. Should be plenty for any one pen-down chain (but we'll check later):
    if(m_pViewData->npc < 100 && m_pViewData->allocatePointsChain(100))
	{
		rc = -3;
		goto out;
	}

    wabsxsclp = (pTextInfo->absxsclp > 0.0) ? pTextInfo->absxsclp : 1.0;

    defabove = 6.0;  /* A default for the # of font vectors per chr height. */

    pssz = sizeof(ps) / sizeof(ps[0]);  /* # of elements in the position stack */
    psidx = -1;		/* Idx of most recently filled member of the position stack */
					/* (-1 means none). */

	/* Set up default for unknown chrs ('?' from icad.fnt): */
	deffontchr.code = 0;
    deffontchr.defsz = sizeof(defdef) / sizeof(defdef[0]);
    deffontchr.def = defdef;

    /*
    **  vfact is the multiplier that is adjustable via codes 3 and 4; it
    **  is set to 1.0 at the beginning of each TEXT item:
    */
    // EBATECH(CNBR) -[ for Extended Shape
    //vfact = 1.0;
    vfactx = vfacty = savex = savey = 1.0;
    // ]- EBATECH(CNBR)

    /* Init the current point (font vector system) and the textbox points */
    /* (TCS): */
    curpt[0] = curpt[1] = curpt[2] = 0.0;
    //box[1][0] = box[1][1] = - (box[0][0] = box[0][1] = 1.0 / IC_ZRO);
    oldabove = 1.0;		/* Safe default; see below. */

	//
	// ******************* MAIN LOOP FOR NON-TTF CASE ****************************
	//
	for (valp=pTextInfo->val; *valp; valp++) {  /* Each chr */
		/* Start the Current Chr Stack: */
		while (ccs!=NULL) { tccp1=ccs->next; delete ccs; ccs=tccp1; }
		if ((ccs= new struct currchrlink )==NULL) { rc=-3; goto out; }
		memset(ccs,0,sizeof(struct currchrlink));
		ccs->forcependown=1;

		/* Point ccs->thisfontchr to the db_fontchrlink to use */
		/* (the default, if necessary), and set ccs->whichfont: */
		scoretog=gr_getfontchr(m_pDrawing, &valp,pTextInfo->font,pTextInfo->bigfont,&ccs->thisfontchr,
			&ccs->whichfont);

		if (ccs->thisfontchr==NULL) ccs->thisfontchr=&deffontchr;

		ccs->above=(ccs->whichfont!=NULL && ccs->whichfont->above) ?
			fabs((sds_real)ccs->whichfont->above) : defabove;

		/*
		**  CORRECTING curpt:
		**
		**  Here's a solution to a very subtle problem with curpt.
		**  curpt[] is maintained in the font vector system (and must
		**  stay that way, since maintaining it in the TCS would apply
		**  obliquing, which would screw up our arcs/circles generation).
		**  If a single text item has characters from two fonts (normal
		**  and bigfont) with different vector scales ("above"), then
		**  the relationship between the font vector system and the TCS
		**  changes.  curpt[] is the only value that has to make the
		**  transition and remain valid.  When we switch from one font
		**  to the other, curpt[] is incorrect for the new system.  The
		**  code below applies a simple correction factor so that curpt
		**  looks correct in the new font vector system: we divide by
		**  the old "above" and multiply by the new one.
		**
		**  For the very first character, oldabove is safe (non-0)
		**  but bogus (1.0).  That's okay -- because curpt[] is (0,0,0)
		**  at that time.
		*/
		ar1=ccs->above/oldabove;
		if (ar1!=1.0) {
			curpt[0]*=ar1;
			curpt[1]*=ar1;
			curpt[2]*=ar1;  /* In case */
			oldabove=ccs->above;
		}

		/*
		**  ccs->fact[] will be used as the dx and dy multipliers
		**  to convert from font vector units to TCS units:
		*/
		ccs->fact[0]=(ccs->fact[1]=pTextInfo->ht/ccs->above)*pTextInfo->wfact;

		/* Capture over/underscore toggle (now that we have ccs->fact[0] */
		/* so that we can convert curpt[0] from font vector system */
		/* to the TCS): */
		if (scoretog==-2 || scoretog==-3) {
			if ((tsp1= new struct scorelink )==NULL)
					{ rc=-3; goto out; }
			memset(tsp1,0,sizeof(struct scorelink));
			tsp1->over=(scoretog==-2);
			tsp1->tcsx=curpt[0]*ccs->fact[0];
			if (scorell==NULL) scorell=tsp1; else pscore->next=tsp1;
			pscore=tsp1;

			continue;
		}

		/*
		**  Walk the chr def.  For each pen-down chain, build m_pViewData->pc
		**  and send it to gr_gensolid().  (We build the pc chain in
		**  font vector units with the aid of curpt.  Then, when it's
		**  time to process it, we convert it to the TCS, adjust the
		**  textbox, convert it to the NCS, and pass it on to
		**  gr_gensolid().)
		*/
		pendown=1; skip=done=cmdcode=0;
		dx=dy=0.0;  /* Used for vectors AND arcs; indicates the endpt */
					/* in both cases (font vector units -- BEFORE APPLYING */
					/* vfact). */
		repmode=0;  /* 0, 9, or 13 (the repeating commands) */
		m_pViewData->upc=0; m_pViewData->lupc=NULL;  /* Init the pc chain. */
		for (ccs->didx=0; !done; ccs->didx++) {  /* Each def byte */

			/*
			**  Flag a "done" condition -- but don't bail.  Let code
			**  at the bottom of this loop generate the last pc chain.
			**
			**  Also, note that this may mean we're done with a
			**  subshape -- not with the primary chr.  In that case, the
			**  code at the bottom of the loop will fix ccs and set
			**  "done" to zero again to continue after the subshape.
			*/
			if (ccs->didx>=ccs->thisfontchr->defsz) done=1;

			vertonly=gotdxdy=circ=genpc=0; arcmode=-1;

			if (!done) {

				/* If "skip" is set, skip the ACTION, but do the STEPPING. */

				if (repmode) {

					if (++ccs->didx<ccs->thisfontchr->defsz) {
						if (!ccs->thisfontchr->def[ccs->didx-1] &&
							!ccs->thisfontchr->def[ccs->didx]) repmode=0;

						/* The logic from cases 8 and 12 is repeated here: */

						if (repmode==9) {
							if (!skip) {
								dx=(sds_real)ccs->thisfontchr->def[ccs->didx-1];
								dy=(sds_real)ccs->thisfontchr->def[ccs->didx];
								gotdxdy=1;
							}
						} else if (repmode==13) {
							if (++ccs->didx<ccs->thisfontchr->defsz && !skip) {
								arcmode=1;  /* Set up for bulge format (so CW arcs */
											/* and circles can be done by */
											/* gr_arc2pc() for connectivity with */
											/* a chain that's already been started). */

								/* GET THE PARAMETERS IN FONT VECTOR UNITS: */

								/* dx and dy to endpt: */
								dx=(sds_real)ccs->thisfontchr->def[ccs->didx-2];
								dy=(sds_real)ccs->thisfontchr->def[ccs->didx-1];

								/* Bulge */
								if ((fi1=ccs->thisfontchr->def[ccs->didx])>127) fi1=127;
								else if (fi1<-127) fi1=-127;
								bulge=((sds_real)fi1)/127.0;
							}
						}
					}

				} else {

					cmdcode=(ccs->forcependown) ?
						1 : ccs->thisfontchr->def[ccs->didx];

					switch (cmdcode) {

						case  0:  /* End */
							if (skip) break;
							ccs->didx=ccs->thisfontchr->defsz;  /* Trigger a "done" */
							break;
						case  1:  /* Pen down */
							/* If we're doing a forced pendown, decrement */
							/* ccs->didx so that we don't eat a def byte: */
							if (ccs->forcependown) {
								ccs->didx--;
								ccs->forcependown=0;
							}

							if (skip) break;

							pendown=1; dx=dy=0.0; gotdxdy=1;

							break;
						case  2:  /* Pen up */
							if (skip || !pendown) break;
							pendown=0;
							genpc=1;
							break;
						case  3:  /* Divide vector lengths by next byte */
							if (++ccs->didx>=ccs->thisfontchr->defsz) break;
							if (skip) break;
							if (!ccs->thisfontchr->def[ccs->didx]) break;
							// EBATECH(CNBR) ]- for extended subshapes
							//vfact/=((unsigned char)ccs->thisfontchr->def[ccs->didx]);
							vfactx/=((unsigned char)ccs->thisfontchr->def[ccs->didx]);
							vfacty/=((unsigned char)ccs->thisfontchr->def[ccs->didx]);
							// ]- EBATECH(CNBR)
							break;
						case  4:  /* Multiply vector lengths by next byte */
							if (++ccs->didx>=ccs->thisfontchr->defsz) break;
							if (skip) break;
							if (!ccs->thisfontchr->def[ccs->didx]) break;
							// EBATECH(CNBR) ]- for extended subshapes
							//vfact*=((unsigned char)ccs->thisfontchr->def[ccs->didx]);
							vfactx*=((unsigned char)ccs->thisfontchr->def[ccs->didx]);
							vfacty*=((unsigned char)ccs->thisfontchr->def[ccs->didx]);
							// ]- EBATECH(CNBR)
							break;
						case  5:  /* Push position */
							if (skip || psidx>pssz-2) break;
							psidx++;
							ps[psidx][0]=curpt[0];
							ps[psidx][1]=curpt[1];
							break;
						case  6:  /* Pop position */
							/*
							**  Okay.  The code's getting a little bizarre
							**  as I keep patching problems.
							**
							**  Pop needs to lift the pen, move, and
							**  then restore the pen to its original
							**  status.  We can do all of this by
							**  popping the position, setting
							**  ccs->forcependown if the pen is
							**  currently down, and doing the guts of
							**  the penup command (case 2),.  (See the
							**  processing code below the end of this
							**  "else" and the forcependown code above.)
							*/

							if (skip || psidx<0) break;

							/* Pop curpt: */
							curpt[0]=ps[psidx][0];
							curpt[1]=ps[psidx][1];
							dx=dy=0.0; psidx--; gotdxdy=1;

							/* Set css->forcependown if it's currently down: */
							ccs->forcependown=(pendown!=0);

							/* Do a penup command: */
							pendown=0; genpc=1;

							break;
						case  7:  /* Subshape */

							subsh.extended=0;
							if (ccs->whichfont!=NULL &&
								ccs->whichfont->format == db_fontlink::UNIFONT) { /* UNIFONT */

								if ((ccs->didx+=2)>=ccs->thisfontchr->defsz) break;
								subsh.code=(unsigned char)
									ccs->thisfontchr->def[ccs->didx-1];
								subsh.code=(subsh.code<<8)|
									((short)((unsigned char)
									ccs->thisfontchr->def[ccs->didx]));
							} else {  /* SHAPE or BIGFONT */
								if (++ccs->didx>=ccs->thisfontchr->defsz) break;
								subsh.code=(unsigned char)
									ccs->thisfontchr->def[ccs->didx];
								if (!subsh.code) {
									subsh.extended=1;
									if ((ccs->didx+=6)>=ccs->thisfontchr->defsz) break;
									subsh.code=(unsigned char)
										ccs->thisfontchr->def[ccs->didx-5];
									// EBATECH(CNBR) ]- Miss it
									//subsh.code=(subsh.code<<8)&
									//	((short)((unsigned char)
									//	ccs->thisfontchr->def[ccs->didx-4]));
									subsh.code=(subsh.code<<8)|
										((short)((unsigned char)
										ccs->thisfontchr->def[ccs->didx-4]));
									// EBATECH(CNBR) ]-	
									subsh.basex=ccs->thisfontchr->def[ccs->didx-3];
									subsh.basey=ccs->thisfontchr->def[ccs->didx-2];
									subsh.wd   =ccs->thisfontchr->def[ccs->didx-1];
									subsh.ht   =ccs->thisfontchr->def[ccs->didx];
								}
							}

							/* Now that the stepping has been done: */
							if (skip || !subsh.code) break;

							if (subsh.extended) {
							  // EBATECH(CNBR) ]- resolved Extended bigfonts.
							  wf=NULL; tfc=NULL;
							  for (fi1=256; fi1<pTextInfo->bigfont->nchrs &&
								pTextInfo->bigfont->chr[fi1].code!=
								subsh.code; fi1++);
							  if (fi1>-1 && fi1<pTextInfo->bigfont->nchrs &&
								pTextInfo->bigfont->chr[fi1].def!=NULL) {
								tfc=pTextInfo->bigfont->chr+fi1;
								wf=pTextInfo->bigfont;
							  }
							  /* If we found it */
							  if (tfc!=NULL && wf!=NULL) {
								/* Push Current Point */
								if (psidx>pssz-2) break;
								psidx++;
								ps[psidx][0]=curpt[0];
								ps[psidx][1]=curpt[1];
								/* Set dx/dy and flag */
								dx=(sds_real)subsh.basex;
								dy=(sds_real)subsh.basey;
								gotdxdy=1;
							  }
							  // EBATECH(CNBR) ]-

							  break;

							} else {
							  // EBATECH(CNBR) Move wf and tfc to local
							  //struct db_fontlink *wf;      /* whichfont */
							  //struct db_fontchrlink *tfc;  /* thisfontchr */

							  /*
							  **  Look for subsh.code and set wf and tfc.
							  **  (A bigfont can subshape a bigfont or
							  **  normal font chr; a normal font can only
							  **  subshape a normal font chr.)
							  */
							  wf=NULL; tfc=NULL;
							  if (ccs->whichfont!=NULL &&
								  ccs->whichfont->format == db_fontlink::BIGFONT &&
								  pTextInfo->bigfont!=NULL) {

								if (subsh.code>-1 && subsh.code<256)
								  fi1=subsh.code;
								else for (fi1=256; fi1<pTextInfo->bigfont->nchrs &&
								  pTextInfo->bigfont->chr[fi1].code!=
								  subsh.code; fi1++);
								if (fi1>-1 && fi1<pTextInfo->bigfont->nchrs &&
								  pTextInfo->bigfont->chr[fi1].def!=NULL) {

								  tfc=pTextInfo->bigfont->chr+fi1;
								  wf=pTextInfo->bigfont;
								}
							  }
							  if (tfc==NULL && pTextInfo->font!=NULL) {
								if (subsh.code>-1 && subsh.code<256)
								  fi1=subsh.code;
								else for (fi1=256; fi1<pTextInfo->font->nchrs &&
								  pTextInfo->font->chr[fi1].code!=subsh.code; fi1++);
								if (fi1>-1 && fi1<pTextInfo->font->nchrs &&
								  pTextInfo->font->chr[fi1].def!=NULL) {

								  tfc=pTextInfo->font->chr+fi1;
								  wf=pTextInfo->font;
								}
							  }

							  /* If we found it, create the new link on top */
							  /* of the current chr stack: */
							  if (tfc!=NULL && wf!=NULL) {

								if ((tccp1= new struct currchrlink )==NULL)
									{ rc=-3; goto out; }

								memset(tccp1,0,sizeof(struct currchrlink));

								tccp1->forcependown=1;

								/* Set vals for the subshape: */
								tccp1->whichfont=wf;
								tccp1->thisfontchr=tfc;
								tccp1->didx=-1;  /* Loop will inc to 0 */
								tccp1->above=(tccp1->whichfont!=NULL &&
								  tccp1->whichfont->above) ?
									fabs((sds_real)tccp1->whichfont->above) :
									defabove;
								tccp1->fact[0]=(tccp1->fact[1]=pTextInfo->ht/tccp1->above)*
								  pTextInfo->wfact;

								/* Push it onto the top: */
								tccp1->next=ccs; ccs=tccp1;

								/* Correct curpt for the new "above" value. */
								/* (See "CORRECTING curpt" note earlier.): */
								ar1=ccs->above/ccs->next->above;
								if (ar1!=1.0) {
								  curpt[0]*=ar1;
								  curpt[1]*=ar1;
								  curpt[2]*=ar1;  /* In case */
								}

								/* New ccs link in place; let 'er rip. */

							  }
							}  /* End else not an extended subshape. */

							break;
						case  8:  /* dx,dy in next 2 bytes */
							if ((ccs->didx+=2)>=ccs->thisfontchr->defsz) break;
							if (skip) break;
							dx=(sds_real)ccs->thisfontchr->def[ccs->didx-1];
							dy=(sds_real)ccs->thisfontchr->def[ccs->didx];
							gotdxdy=1;
							break;
						case  9: case 13:  /* Repeat until (0,0) */
							repmode=ccs->thisfontchr->def[ccs->didx];
							break;
						case 10:  /* Octant arc (next 2 bytes) */
							if ((ccs->didx+=2)>=ccs->thisfontchr->defsz) break;
							if (skip) break;

							arcmode=1;  /* Set up for bulge format (so CW arcs */
										/* and circles can be done by */
										/* gr_arc2pc() for connectivity with */
										/* a chain that's already been started). */

							cw=((ccs->thisfontchr->def[ccs->didx]&'\x80')!=0);

							/* GET THE PARAMETERS IN FONT VECTOR UNITS: */


							/* Get the radius: */
							rad=(sds_real)
								((unsigned char)ccs->thisfontchr->def[ccs->didx-1]);
							if (rad<1.0) rad=1.0;

							/* Start angle: */
							sa=0.25*IC_PI*(((unsigned char)
								(ccs->thisfontchr->def[ccs->didx]&'\x70'))>>4);

							/* Included angle: */
							if ((fi1=ccs->thisfontchr->def[ccs->didx]&'\x07')==0) {
								/* Circle; set up an arc that almost closes */
								/* (so we can continue to use bulge format */
								/* as discussed above). */

								/* Get the unit tangent direction vector: */
								if (cw) { ap1[0]=-sin(sa); ap1[1]= cos(sa); }
								else    { ap1[0]= sin(sa); ap1[1]=-cos(sa); }

								/* Put the 2nd pt 0.01 font vector units away */
								/* from the 1st pt in that direction: */
								ar1=0.01;
								dx=ar1*ap1[0]; dy=ar1*ap1[1];
								/*
								**  The approx angle subtended is ar1/rad.
								**  The tangent of one fourth of this is the
								**  fabs(bulge) of the short arc.  The
								**  inverse of that is the fabs(bulge) of
								**  the long arc:
								*/
								bulge=1.0/tan(0.25*ar1/rad);

								circ=1;
							} else {
								iang=0.25*IC_PI*fi1;

								if (cw) ea=sa-iang; else ea=sa+iang;

								/* dx and dy to endpt: */
								dx=rad*(cos(ea)-cos(sa));
								dy=rad*(sin(ea)-sin(sa));

								/* abs(bulge): */
								bulge=tan(0.25*iang);
							}
							if (cw) bulge=-bulge;  /* Neg bulge for CW arcs. */

							break;
						case 11:  /* Fractional arc (next 5 bytes) */

							/* See documentation in learned.doc for this one; */
							/* ACAD's documentation is incorrect and */
							/* insufficient. */

						  {
							sds_real soct,eoctrel,soff,eoff;

							if ((ccs->didx+=5)>=ccs->thisfontchr->defsz) break;
							if (skip) break;

							arcmode=1;  /* Set up for bulge format (so CW arcs */
										/* and circles can be done by */
										/* gr_arc2pc() for connectivity with */
										/* a chain that's already been started). */

							cw=((ccs->thisfontchr->def[ccs->didx]&'\x80')!=0);

							/* GET THE PARAMETERS IN FONT VECTOR UNITS: */
							/* (Note we work in degrees for rounding.) */

							/* Radius: */
							rad=256.0*((sds_real)((unsigned char)
								ccs->thisfontchr->def[ccs->didx-2]))+
								(sds_real)((unsigned char)
								ccs->thisfontchr->def[ccs->didx-1]);
							if (rad<1.0) rad=1.0;

							/* Starting octant (deg): */
							soct=45.0*(((unsigned char)
								(ccs->thisfontchr->def[ccs->didx]&'\x70'))>>4);

							/* Ending octant rel to starting octant (deg): */
							if ((fi1=ccs->thisfontchr->def[ccs->didx]&'\x07')<1) fi1=8;
							eoctrel=45.0*(fi1-1);

							/* Starting offset (deg): */
							ar1=((sds_real)((unsigned char)
								ccs->thisfontchr->def[ccs->didx-4]))*45.0/256.0;
							if (modf(ar1,&soff)>=0.5) soff+=1.0;

							/* Ending offset (deg): */
							ar1=((sds_real)((unsigned char)
								ccs->thisfontchr->def[ccs->didx-3]))*45.0/256.0;
							if (modf(ar1,&eoff)>=0.5) eoff+=1.0;
							if (eoff<0.9) eoff=45.0;

							/* Starting angle: */
							sa=soct+((cw) ? -soff : soff);

							/* Ending angle: */
							ar1=eoctrel+eoff;
							ea=soct+((cw) ? -ar1 : ar1);

							/* Convert to radians finally: */
							ar1=IC_PI/180.0;
							sa*=ar1; ea*=ar1;

							/* Get the included angle: */
							ic_normang(&sa,&ea);
							iang=ea-sa; if (cw) iang=IC_TWOPI-iang;

							ar1=fabs(iang);
							if (ar1<IC_ZRO || fabs(ar1-IC_TWOPI)<IC_ZRO) {
								/* Circle; set up an arc that almost closes */
								/* (so we can continue to use bulge format */
								/* as discussed above). */

								/* Get the unit tangent direction vector: */
								if (cw) { ap1[0]=-sin(sa); ap1[1]= cos(sa); }
								else    { ap1[0]= sin(sa); ap1[1]=-cos(sa); }

								/* Put the 2nd pt 0.01 font vector units away */
								/* from the 1st pt in that direction: */
								ar1=0.01;
								dx=ar1*ap1[0]; dy=ar1*ap1[1];
								/*
								**  The approx angle subtended is ar1/rad.
								**  The tangent of one fourth of this is the
								**  fabs(bulge) of the short arc.  The
								**  inverse of that is the fabs(bulge) of
								**  the long arc:
								*/
								bulge=1.0/tan(0.25*ar1/rad);

								circ=1;
							} else {
								/* dx and dy to endpt: */
								dx=rad*(cos(ea)-cos(sa));
								dy=rad*(sin(ea)-sin(sa));

								/* abs(bulge): */
								bulge=tan(0.25*iang);
							}
							if (cw) bulge=-bulge;  /* Neg bulge for CW arcs. */

							break;

						  }

						case 12:  /* Arc by bulge (next 3 bytes) */
							if ((ccs->didx+=3)>=ccs->thisfontchr->defsz) break;
							if (skip) break;

							arcmode=1;  /* Set up for bulge format (so CW arcs */
										/* and circles can be done by */
										/* gr_arc2pc() for connectivity with */
										/* a chain that's already been started). */

							/* GET THE PARAMETERS IN FONT VECTOR UNITS: */

							/* dx and dy to endpt: */
							dx=(sds_real)ccs->thisfontchr->def[ccs->didx-2];
							dy=(sds_real)ccs->thisfontchr->def[ccs->didx-1];

							/* Bulge */
							if ((fi1=ccs->thisfontchr->def[ccs->didx])>127) fi1=127;
							else if (fi1<-127) fi1=-127;
							bulge=((sds_real)fi1)/127.0;

							break;
						case 14:  /* Process next command only for vertical text */
							vertonly=1;
							break;
						case 15:  /* Not used */
							break;
						default:  /* Vector/direction */
							if (skip) break;
							vlen=(unsigned char)ccs->thisfontchr->def[ccs->didx];
							vdir=vlen&'\x0F'; if (!(vlen>>=4)) break;
							switch (vdir) {
								case '\x00': dx= 1.0; dy= 0.0; break;
								case '\x01': dx= 1.0; dy= 0.5; break;
								case '\x02': dx= 1.0; dy= 1.0; break;
								case '\x03': dx= 0.5; dy= 1.0; break;
								case '\x04': dx= 0.0; dy= 1.0; break;
								case '\x05': dx=-0.5; dy= 1.0; break;
								case '\x06': dx=-1.0; dy= 1.0; break;
								case '\x07': dx=-1.0; dy= 0.5; break;
								case '\x08': dx=-1.0; dy= 0.0; break;
								case '\x09': dx=-1.0; dy=-0.5; break;
								case '\x0A': dx=-1.0; dy=-1.0; break;
								case '\x0B': dx=-0.5; dy=-1.0; break;
								case '\x0C': dx= 0.0; dy=-1.0; break;
								case '\x0D': dx= 0.5; dy=-1.0; break;
								case '\x0E': dx= 1.0; dy=-1.0; break;
								case '\x0F': dx= 1.0; dy=-0.5; break;
							}
							dx*=vlen; dy*=vlen; gotdxdy=1;

							break;
					}

				}  /* End else !repmode */


				if (gotdxdy || arcmode>-1) {  /* Process vector or arc cmd */
					// EBATECH(CNBR) ]- for extended subshapes
					//endpt[0]=curpt[0]+dx*vfact;
					//endpt[1]=curpt[1]+dy*vfact;
					endpt[0]=curpt[0]+dx*vfactx;
					endpt[1]=curpt[1]+dy*vfacty;
					// ]- EBATECH(CNBR)

//VS: to consider interspace before and after character{
					// Apply height, width factor, and obliquing 
					// to get to the TSC: 
					ap1[1]=endpt[1]*ccs->fact[1];
					ap1[0]=endpt[0]*ccs->fact[0]+ap1[1]*TanObl;

					// Adjust the textbox: 
					if (box[0][0]>ap1[0]) box[0][0]=ap1[0];
					if (box[1][0]<ap1[0]) box[1][0]=ap1[0];
					if (box[0][1]>ap1[1]) box[0][1]=ap1[1];
					if (box[1][1]<ap1[1]) box[1][1]=ap1[1];
					bGotBox = true;
//VS}
					if (pendown) {

						if (gotdxdy) {  /* Vector */

							/*
							**  In font isocp, several European chrs are
							**  defined by subshaping English chrs and then
							**  adding marks above them.  When the subshape
							**  command is the first command, TWO pendowns
							**  occur (the automatics) and a dot was being
							**  produced.  The "if" below guards against
							**  successive pendown commands where no
							**  intervening drawing is occurring:
							*/
							if (cmdcode!=1 || m_pViewData->lupc==NULL ||
								endpt[0]!=m_pViewData->lupc->pt[0] ||
								endpt[1]!=m_pViewData->lupc->pt[1]) {

								if (m_pViewData->upc>=m_pViewData->npc) {  /* Add 100 links: */
									fl1=m_pViewData->upc;  /* Save; allocatePointsChain() will set it. */
									if (m_pViewData->allocatePointsChain(m_pViewData->npc+100)) 
									{ 
										rc = -3; 
										goto out; 
									}
									m_pViewData->upc=fl1;
								}

								m_pViewData->upc++;
								m_pViewData->lupc=(m_pViewData->lupc==NULL) ?
									m_pViewData->pc : m_pViewData->lupc->next;
								m_pViewData->lupc->d2go=-1.0;
								m_pViewData->lupc->swid=m_pViewData->lupc->ewid=0.0;
								m_pViewData->lupc->startang=m_pViewData->lupc->endang=IC_ANGLE_DONTCARE;

								/* Keep the pc chain in the font vector system */
								/* for now: */
								m_pViewData->lupc->pt[0]=endpt[0];
								m_pViewData->lupc->pt[1]=endpt[1];
								m_pViewData->lupc->pt[2]=0.0;
							}

						} else {  /* arcmode>-1 */

							/*
							**  We're going to call gr_arc2pc(), so
							**  make sure that m_pViewData->lupc is
							**  non-NULL.  (The pt is irrelevant here
							**  because gr_arc2pc() will replace it.)
							*/
							if (m_pViewData->lupc==NULL) {
								m_pViewData->lupc=m_pViewData->pc; m_pViewData->upc=1;
								m_pViewData->lupc->pt[0]=m_pViewData->lupc->pt[1]=
									m_pViewData->lupc->pt[2]=0.0;
								m_pViewData->lupc->d2go=-1.0;
								m_pViewData->lupc->swid=m_pViewData->lupc->ewid=0.0;
								m_pViewData->lupc->startang=m_pViewData->lupc->endang=IC_ANGLE_DONTCARE;
							}

							/* Call gr_arc2pc(); it should write over */
							/* m_pViewData->lupc: */
							if ((rc=gr_arc2pc(arcmode,curpt[0],curpt[1],
								endpt[0],endpt[1],bulge,pTextInfo->llpt[2],1,0.0,0.0,
								1,  /* ltsclwblk */
								/* pixsz: How big is a pixel in font vectors?: */
								(boxonly) ?
									ccs->above/100.0 :
									fabs(m_pViewData->GetPixelHeight())/ccs->fact[1]/wabsxsclp,
								/* Force a good resolution for boxonly: */
								(boxonly) ? 100 : m_pViewData->curvdispqual,
								m_pViewData))!=0)
										goto out;

							/* If it's a circle, add 1 more seg to close */
							/* the circle (since our bulge-specified */
							/* "circles" don't quite close): */
							if (circ) {
								if (m_pViewData->upc>=m_pViewData->npc) {  /* Add 100 links: */
									fl1=m_pViewData->upc;  /* Save; allocatePointsChain() will set it. */
									if (m_pViewData->allocatePointsChain(m_pViewData->npc+100)) 
									{ 
										rc = -3; 
										goto out; 
									}
									m_pViewData->upc=fl1;
								}
								m_pViewData->upc++;
								m_pViewData->lupc=(m_pViewData->lupc==NULL) ?
									m_pViewData->pc : m_pViewData->lupc->next;
								m_pViewData->lupc->d2go=-1.0;
								m_pViewData->lupc->swid=m_pViewData->lupc->ewid=0.0;
								m_pViewData->lupc->startang=m_pViewData->lupc->endang=IC_ANGLE_DONTCARE;

								GR_PTCPY(m_pViewData->lupc->pt,curpt);
							}

						}  /* End else arcmode>-1 */

					}  /* End if pendown */

					curpt[0]=endpt[0]; curpt[1]=endpt[1];

				}  /* End if (gotdxdy || arcmode>-1) */

			}  /* End if (!done) */

			/* pc chain finished if genpc is set, or we're done with */
			/* the primary chr (not just done with a subshape): */
			if (genpc || (done && ccs->next==NULL)) {
				if (m_pViewData->upc>1) {
					/*
					** This pc chain is finished and has more
					** than 1 pt.  Walk it and do the following:
					** transform from font vector system to TCS;
					** adjust the textbox; transform from TCS to
					** NCS; call gr_gensolid():
					*/
					for (pcidx=0L,tpcp1=m_pViewData->pc; pcidx<
						m_pViewData->upc && tpcp1!=NULL; pcidx++,
						tpcp1=tpcp1->next) {

						// Apply height, width factor, and obliquing 
						// to get to the TSC: 
						ap1[1]=tpcp1->pt[1]*ccs->fact[1];
						ap1[0]=tpcp1->pt[0]*ccs->fact[0]+ap1[1]*TanObl;

						// Adjust the textbox: 
/* VS: it's calculated above
						if (box[0][0]>ap1[0]) box[0][0]=ap1[0];
						if (box[1][0]<ap1[0]) box[1][0]=ap1[0];
						if (box[0][1]>ap1[1]) box[0][1]=ap1[1];
						if (box[1][1]<ap1[1]) box[1][1]=ap1[1];
						bGotBox = true;
*/
						/* Transform to NCS and put the pt back */
						/* into the pc chain: */
						tpcp1->pt[0]=ap1[0]*tcsaxis[0][0]+
									 ap1[1]*tcsaxis[1][0]+pTextInfo->llpt[0];
						tpcp1->pt[1]=ap1[0]*tcsaxis[0][1]+
									 ap1[1]*tcsaxis[1][1]+pTextInfo->llpt[1];
						tpcp1->pt[2]=pTextInfo->llpt[2];
					}

					/* Gen the disp objs (if needed): */
					if (!boxonly)
					{
						setPointsData(m_pViewData->pc, m_pViewData->lupc, m_pViewData->pc->next, m_pViewData->upc-2);
						setFlags(0, 0);
						setDotDir(NULL);
						if((rc = CDoBase::getDisplayObjects(tdop1, tdop2)) != 0) 
							goto out;

						if (tdop1!=NULL && tdop2!=NULL) {
							if (begdo==NULL) begdo=tdop1; else enddo->next=tdop1;
							enddo=tdop2;
						}
					}
				}

				m_pViewData->upc=0; m_pViewData->lupc=NULL;  /* Start over */

			}  /* End if (genpc || done) */

			/* Did we just finish a subshape? */
			if (done && ccs->next!=NULL) {
				/* Correct curpt for the "above" value we're restoring. */
				/* (See "CORRECTING curpt" note earlier.): */
				ar1=ccs->next->above/ccs->above;
				if (ar1!=1.0) {
				  curpt[0]*=ar1;
				  curpt[1]*=ar1;
				  curpt[2]*=ar1;  /* In case */
				}

				/* Discard the top link that we just finished: */
				tccp1=ccs->next; delete ccs; ccs=tccp1;

				done=0; /* Continue with the parent. */
				// EBATECH(CNBR) ]- for extended subshape
				if (subsh.extended) {
					if (psidx<0) break;
					/* Pop curpt: */
					curpt[0]=ps[psidx][0];
					curpt[1]=ps[psidx][1];
					psidx--;
					vfactx /= savex;
					vfacty /= savey;
				}
				// EBATECH(CNBR) ]-
			}

			if (!repmode) skip=(vertonly && !pTextInfo->vert);

			// EBATECH(CNBR) ]- extended subshape Start
			if (cmdcode==7 && subsh.extended && tfc!=NULL && wf!=NULL ) {
				/* create subshape's current char link*/
				if ((tccp1= new struct currchrlink )==NULL)
					{ rc=-3; goto out; }
				memset(tccp1,0,sizeof(struct currchrlink));
				tccp1->forcependown=1;
				/* Set vals for the subshape: */
				tccp1->whichfont=wf;
				tccp1->thisfontchr=tfc;
				tccp1->didx=-1;  /* Loop will inc to 0 */
				tccp1->above=(tccp1->whichfont!=NULL &&
				  tccp1->whichfont->above) ?
					fabs((sds_real)tccp1->whichfont->above) :
					defabove;
				tccp1->fact[1]=ccs->fact[1];
				tccp1->fact[0]=ccs->fact[0];
				/* Push it onto the top: */
				tccp1->next=ccs; ccs=tccp1;
                /* Then, apply subsape's scale factor */
				savex = (sds_real)subsh.wd / (sds_real)ccs->whichfont->width;
				savey = (sds_real)subsh.ht / (sds_real)ccs->above;
				vfactx *= savex;
				vfacty *= savey;
			}
			// EBATECH(CNBR) ]-
		}  /* End for each chr def byte */

	}  /* End for each chr */


	/* Do the over and under scoring (process scorell) */

	if (scorell!=NULL) {
		short oscore,uscore,nullpass,got1;
		sds_real begox,begux,begx,endx,scoreht;

		oscore=uscore=0; begox=begux=0.0; begx=endx=scoreht=0.0;
		nullpass=0;  /* 0 to terminated unterminated underscore; */
					 /* 1 to terminated unterminated  overscore. */
		for (;;) {
			got1=0;

			if ((scorell==NULL && nullpass==0) ||
				(scorell!=NULL && !scorell->over)) {

				if (uscore) {
					uscore=0; scoreht=-0.2*pTextInfo->ht;
					ar1=scoreht*TanObl;
					begx=begux+ar1;
					endx=((scorell==NULL) ?
						curpt[0]*ccs->fact[0] : scorell->tcsx)+ar1;
					got1=1;
				} else if (scorell!=NULL) {
					uscore=1; begux=scorell->tcsx;
				}

			}

			if ((scorell==NULL && nullpass==1) ||
				(scorell!=NULL && scorell->over)) {

				if (oscore) {
					oscore=0; scoreht=1.2*pTextInfo->ht;
					ar1=scoreht*TanObl;
					begx=begox+ar1;
					endx=((scorell==NULL) ?
						curpt[0]*ccs->fact[0] : scorell->tcsx)+ar1;
					got1=1;
				} else if (scorell!=NULL) {
					oscore=1; begox=scorell->tcsx;
				}

			}

			if (got1 && fabs(begx-endx)>IC_ZRO*pTextInfo->ht) {
				/* Adjust the textbox: */
				if (box[0][0]>begx)    box[0][0]=begx;
				if (box[1][0]<begx)    box[1][0]=begx;
				if (box[0][0]>endx)    box[0][0]=endx;
				if (box[1][0]<endx)    box[1][0]=endx;
				if (box[0][1]>scoreht) box[0][1]=scoreht;
				if (box[1][1]<scoreht) box[1][1]=scoreht;
				bGotBox = true;

				/* Gen the disp objs (if needed): */
				if (!boxonly) {
					/* Set up the pc chain with NCS coords: */
					if (m_pViewData->allocatePointsChain(2)) 
					{ 
						rc = -3; 
						goto out; 
					}
					m_pViewData->lupc=m_pViewData->pc->next;
					m_pViewData->pc->d2go=m_pViewData->lupc->d2go=-1.0;
					m_pViewData->pc->swid=m_pViewData->pc->ewid=
						m_pViewData->lupc->swid=m_pViewData->lupc->ewid=0.0;
					m_pViewData->pc->startang=m_pViewData->pc->endang=
						m_pViewData->lupc->startang=m_pViewData->lupc->endang=IC_ANGLE_DONTCARE;
					ar1=scoreht*tcsaxis[1][0]+pTextInfo->llpt[0];
					m_pViewData->pc->pt[0]  =begx*tcsaxis[0][0]+ar1;
					m_pViewData->lupc->pt[0]=endx*tcsaxis[0][0]+ar1;
					ar1=scoreht*tcsaxis[1][1]+pTextInfo->llpt[1];
					m_pViewData->pc->pt[1]  =begx*tcsaxis[0][1]+ar1;
					m_pViewData->lupc->pt[1]=endx*tcsaxis[0][1]+ar1;

					/* Gen: */
					setPointsData(m_pViewData->pc, m_pViewData->lupc, NULL, 0);
					setFlags(0, 0);
					setDotDir(NULL);
					if((rc = CDoBase::getDisplayObjects(tdop1, tdop2)) != 0) 
						goto out;

					if (tdop1!=NULL && tdop2!=NULL) {
						if (begdo==NULL) begdo=tdop1; else enddo->next=tdop1;
						enddo=tdop2;
					}
				}
			}

			if (scorell==NULL) {
				 if (nullpass>0) break;
				 nullpass++;
			} else {
				tsp1=scorell->next; delete scorell; scorell=tsp1;
			}

		}  /* End of scorell-processing loop */

	}  /* End if scorell!=NULL */

out:

    deffontchr.def = NULL;	/* Keep destructor from freeing. */

	if(ppBegDispObj && ppEndDispObj && !boxonly)
	{
		*ppBegDispObj = begdo;
		*ppEndDispObj = enddo;
		begdo = enddo = NULL;
	}
	if(bGotBox && minPoint && maxPoint)
	{
		GR_PTCPY(minPoint, box[0]);
		GR_PTCPY(maxPoint, box[1]);
		minPoint[2] = maxPoint[2] = 0.0;
	}
    while(ccs)
	{
		tccp1 = ccs->next;
		delete ccs;
		ccs = tccp1;
	}
    while(scorell)
	{
		tsp1 = scorell->next;
		delete scorell;
		scorell = tsp1;
	}
    return rc;
}

/******************************************************************************
*+ gr_distance
*
*  A version to get the distance between two points in GR.  Similar functions
*  elsewhere in the code are not accessible from GR.
*
*/
double gr_distance(const sds_point fromPt, const sds_point toPt)
{
	assert(fromPt != NULL && toPt != NULL);
	CD3 d(toPt[0] - fromPt[0], toPt[1] - fromPt[1], toPt[2] - fromPt[2]);
	return d.norm();
}

int getBoundingBox(db_entity* pTextEntity, CTextFragmentStyle* pTextInfo,
 db_drawing*			pDrw,		
 gr_view*				pGrView,	
 CDC*					pDC,
 sds_point*				pBox)
{
	int result = 1;
	CFont* pOldFont = NULL;
	CDoFont* pNewFont = NULL;
	// create and select a logical font
	if(!(pNewFont = CDoFont ::Factory(pTextEntity, pDrw, pDC)))
		return 0;

	if(!(pOldFont = pDC->SelectObject(pNewFont)))
	{
		delete pNewFont;
		return 0;
	}
	TTF_table* pFontTable;  // to get glyph index
	TTF_table fontTable;  // to get glyph index
	if(fontTable.InitCMAP(pDC->GetSafeHdc()) != TRUE)
		pFontTable = NULL;
	else
		pFontTable = &fontTable;

	pBox[0][0] = pBox[0][1] = pBox[0][2] = pBox[1][0] = pBox[1][1] = 0.0;
	pBox[1][2] = - 1.0;	// we calculate bounding pBox of whole text in TTFCreateDisplayObjects if pBox[1][2] < pBox[0][2]
	TCHAR* pCurChar;
	// Bugzilla No. 78410; 06-01-2003 [
	//wchar_t w_text[MAX_TEXT_LENGTH], wchar_code[2];    // for converting from ANSI to Unicode
	wchar_t wchar_code[2];								 // for converting from ANSI to Unicode
	static wchar_t text[MAX_TEXT_LENGTH];
	wchar_t* pText;
	bool bNeedToDelete = false;
	if(_tcslen(pTextInfo->val) >=  MAX_TEXT_LENGTH)
	{
		pText = new wchar_t[ _tcslen(pTextInfo->val) + 1 ];
		bNeedToDelete = true;
	}
	else
		pText = text;
	// Bugzilla No. 78410; 06-01-2003 ]
	long code_from_parser;
	UINT dwg_code_page = get_dwgcodepage(pDrw);
	// Bugzilla No. 78410; 06-01-2003 
	wcscpy(pText, L"");
	//wcscpy(w_text, L"");
	wchar_code[0] = 0;
	wchar_code[1] = 0;
	for(pCurChar = pTextInfo->val; _tcslen(pCurChar) != 0;)
	{
		code_from_parser = gr_parsetext(&pCurChar, dwg_code_page, pTextInfo->font->format, pFontTable);
		if(code_from_parser == -1L)
		{
			result = 0;
			break;
		}
		if(code_from_parser > 0) // to exclude control sequences
		{
			wchar_code[0] = code_from_parser;
			// Bugzilla No. 78410; 06-01-2003 
			wcscat( pText, wchar_code );
			//wcscat( w_text, wchar_code );
		}
	}
	
	// Bugzilla No. 78410; 06-01-2003 
	if(result && !TTFCreateDisplayObjects(pTextEntity, pDrw, pTextInfo, 0, pText, pGrView, pDC, pNewFont, pFontTable, pBox, NULL, NULL))
	//if(result && !TTFCreateDisplayObjects(pTextEntity, pDrw, pTextInfo, 0, w_text, pGrView, pDC, pNewFont, pFontTable, pBox, NULL, NULL))
		result = 0;

	pDC->SelectObject(pOldFont);
	pNewFont->DeleteObject();
	delete pNewFont;
	// Bugzilla No. 78410; 06-01-2003 [
	if(bNeedToDelete)
	{
		delete[] pText;
		pText = NULL;
	}
	// Bugzilla No. 78410; 06-01-2003 ]
	
	return result;
}

int getDisplayObjects(db_entity* pTextEntity, CTextFragmentStyle* pTextInfo,
 db_drawing*			pDrw,		
 gr_view*				pGrView,	
 CDC*					pDC,		
 gr_displayobject*&		pListBegin,
 gr_displayobject*&		pListEnd)
{
	//
	// ****************************** TTF TEXT CASE **********************************
	//
	// we have to process the text one character at a time
	// so that we can deal with % codes:
	//	%%nnn	- Draws character number nnn. Note, it can be one or two digits (e.g. %%9following or %%65following).
	//	%%o		- Toggles overscore mode on/off.
	//	%%u		- Toggles underscore mode on/off.
	//	%%d		- Draws degree symbol (°)
	//	%%p		- Draws plus/minus tolerance symbol (±)
	//	%%c		- Draws circle diameter dimensioning symbol (Ø).
	//	%%%		- Draws a single percent sign.
	// Parser for %%u added by Denis Petrov
	// So this parser oriented only on this sequence in string and
	// in this case text may have only two fragments and in future
	// this may cause problems.
	int result = 1;
	CFont* pOldFont = NULL;
	CDoFont* pNewFont = NULL;
	// create and select a logical font
	if(!(pNewFont = CDoFont ::Factory(pTextEntity, pDrw, pDC)))
		return 0;
#undef _SEARCH_FOR_FONT_
	CDoFont* pFontToUse = pNewFont;

	if(!(pOldFont = pDC->SelectObject(pNewFont)))
	{
		delete pNewFont;
		return 0;
	}
	TTF_table* pFontTable;  // to get glyph index
	TTF_table fontTable;  // to get glyph index
	if(fontTable.InitCMAP(pDC->GetSafeHdc()) != TRUE)
		pFontTable = NULL;
	else
		pFontTable = &fontTable;

	sds_point box[2];
	box[0][0] = box[0][1] = box[0][2] = box[1][0] = box[1][1] = 0.0;
	box[1][2] = - 1.0;	// we calculate bounding box of whole text in TTFCreateDisplayObjects if box[1][2] < box[0][2]
	
	// get bounding box of the whole pTextInfo->val and generate box if necessary
	if(getBoundingBox(pTextEntity, pTextInfo, pDrw, pGrView, pDC, box) == 0)
		result = 0;

	TCHAR*		pCurChar;
	wchar_t wchar_code[2];								 // for converting from ANSI to Unicode
	static wchar_t text[MAX_TEXT_LENGTH];
	wchar_t* pText;
	bool bNeedToDelete = false;
	if(_tcslen(pTextInfo->val) >=  MAX_TEXT_LENGTH)
	{
		pText = new wchar_t[ _tcslen(pTextInfo->val) + 1 ];
		bNeedToDelete = true;
	}
	else
		pText = text;
	
	long        code_from_parser;
	UINT		dwg_code_page;
	
	wcscpy(pText, L"");
	wchar_code[0] = 0;
	wchar_code[1] = 0;
	
	dwg_code_page = get_dwgcodepage(pDrw);
	unsigned int		FontModifiers;
	gr_displayobject	*pDOBegin, *pDOEnd;
	
	FontModifiers = 0;
	
	for(pCurChar = pTextInfo->val, wcscpy(pText, L""); _tcslen(pCurChar) != 0;)
	{
		TCHAR*	temp_pCurChar;
		temp_pCurChar = pCurChar;
		code_from_parser = gr_parsetext(&pCurChar, dwg_code_page, pTextInfo->font->format, pFontTable);
		if(wchar_code[0] == -1L || !result)
		{
			result = 0;
			break;
		}
		wchar_code[0] = code_from_parser;
		if (temp_pCurChar != pCurChar)
		{
			if (code_from_parser == -2L || code_from_parser == -3L)
			{
				if (wcslen(pText) == 0)
				{
					switch (code_from_parser)
					{
					case    -2L :
						// overscore toggle
						if(FontModifiers & CTextFragmentStyle::EModifiers::eOVERLINED)
							FontModifiers &= ~CTextFragmentStyle::EModifiers::eOVERLINED;
						else
							FontModifiers |= CTextFragmentStyle::EModifiers::eOVERLINED;
						break;
					case    -3L :
						// underscore toggle
						if(FontModifiers & CTextFragmentStyle::EModifiers::eUNDERLINED)
							FontModifiers &= ~CTextFragmentStyle::EModifiers::eUNDERLINED;
						else
							FontModifiers |= CTextFragmentStyle::EModifiers::eUNDERLINED;
						break;
					}
					continue;
				}
				else
				{
					//first we must show already parsed text and then 
					//change font modifiers. Undo parsing of "%%" simbols
					pCurChar = temp_pCurChar;
				}
			}
			else
			{
#ifndef _SEARCH_FOR_FONT_
				// special glyph
				wcscat( pText, wchar_code );
				if( _tcslen(pCurChar) != 0)
					continue;
#else
				// special glyph
				// DP: check whether glyph with given code exist
				if(pFontTable == NULL || pFontTable->FindGlyph(wchar_code[0]) != 0)
				{
					// DP: very well, continue
					wcscat(pText, wchar_code);
					if(_tcslen(pCurChar) != 0)
						continue;
				}
				else
				{
					// DP: try to do something
					if(wcslen(pText) == 0)
					{
						// DP: try to set for this code another font and then generate primitives for him
						pFontToUse = NULL;
						wcscat(pText, wchar_code);
						if(pNewFont->backup() != NULL)
						{
							// DP: system proposed to use this font, check him
							pFontToUse = pNewFont->backup();
							pDC->SelectObject(pFontToUse);
							if(fontTable.InitCMAP(pDC->GetSafeHdc()) != TRUE)
								pFontTable = NULL;
							else
								pFontTable = &fontTable;
							if(pFontTable && pFontTable->FindGlyph(wchar_code[0]) == 0)
								pFontToUse = NULL;
						}
						if(pFontToUse == NULL)
						{
							// DP: begin brute force find
							if(pNewFont->createBackup(wchar_code[0], pDC))
								pFontToUse = pNewFont->backup();
							else
								pFontToUse = pNewFont;
							pDC->SelectObject(pFontToUse);
							if(fontTable.InitCMAP(pDC->GetSafeHdc()) != TRUE)
								pFontTable = NULL;
							else
								pFontTable = &fontTable;
						}
					}
					else
						// DP: generate previous fragment and set this code to be processed again
						pCurChar = temp_pCurChar;
				}
#endif
			}
		}
		
#ifndef _SEARCH_FOR_FONT_		
		if(!TTFCreateDisplayObjects(pTextEntity, pDrw, pTextInfo, FontModifiers, pText, pGrView, pDC, pNewFont, pFontTable, box, &pDOBegin, &pDOEnd))
		{
			result = 0;
			break;
		}
#else
		if(!TTFCreateDisplayObjects(pTextEntity, pDrw, pTextInfo, FontModifiers, pText, pGrView, pDC, pFontToUse, pFontTable, box, &pDOBegin, &pDOEnd))
		{
			result = 0;
			break;
		}
		// DP: restore original font
		if(pFontToUse != pNewFont)
		{
			pFontToUse = pNewFont;
			pDC->SelectObject(pFontToUse);
			if(fontTable.InitCMAP(pDC->GetSafeHdc()) != TRUE)
				pFontTable = NULL;
			else
				pFontTable = &fontTable;
		}
#endif
		wcscpy(pText, L"");
		
		if(pListBegin)
			pListEnd->next = pDOBegin;
		else
			pListBegin = pDOBegin;
		if(pDOEnd != NULL)
			pListEnd = pDOEnd;
		
	} // for pCurChar
	pDC->SelectObject(pOldFont);
	pNewFont->DeleteObject();
	delete pNewFont;
	if(bNeedToDelete)
	{
		delete[] pText;
		pText = NULL;
	}
	return result;
}

/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Main function in TTF text output.
 *			Calculate bounding box of TTF text string and/or create display objects for it.
 * Returns:	True in success and false in failure.
 ********************************************************************************/
int
TTFCreateDisplayObjects
(
 db_entity*				pTextEntity,		// => text, mtext etc.
 db_drawing*			pDrw,               // =>
 CTextFragmentStyle*	pTextInfo,			// =>
 unsigned int			FontModifiers,		// => just for underlined & overlined text now (TO DO: move to CTextFragmentStyle)
 wchar_t				w_Text[],    		// => (TO DO: use pTextInfo->val - remove parser from gr_textgen)
 gr_view*				pGrView,			// =>
 CDC*					pDC,				// =>
 CDoFont*				pFont,
 TTF_table*				pFontTable,
 sds_point*				pTotalBoundingBox,	// <=> if pTotalBoundingBox[1][2] < pTotalBoundingBox[0][2] then calculate total BB; else use it for justifying
 gr_displayobject**		ppBegDispObj,		// <= if NULL then not created and not returned
 gr_displayobject**		ppEndDispObj		// <= if NULL then not created and not returned
)
{
	bool				bResult = true;
	gr_displayobject	*pTmpBegDispObj = NULL, *pTmpEndDispObj = NULL;
	int					Dimension,
						SymbolNum,
						HJustification, VJustification;
	sds_point			Scales,
						InsertPoint,
						CurrentBoundingBox[2];
	POINT				NextInsertPoint = {0, 0};
	double				CosAng = cos(pTextInfo->rot),
						SinAng = sin(pTextInfo->rot),
						TanObl = 0.0,
						BackwardSign = (pTextInfo->gen & IC_TEXT_GEN_BACKWARD)? -1.0 : 1.0,
						UpsidedownSign = (pTextInfo->gen & IC_TEXT_GEN_UPSIDEDOWN)? -1.0 : 1.0,
						TotalHeight, TotalWidth,
						CurrentHeight, CurrentWidth;
	LPOUTLINETEXTMETRIC	pMetrics = NULL;
	bool				bNeedAdjustTotalBB = false;
	GCP_RESULTSA		reorderingResults;
	UINT				charsNum = wcslen(w_Text);
	wchar_t*			pWCString = NULL;
	
	static sds_point	PreviousBoundingBox[2];

	// make sure we have the necessary arguments
	ASSERT(pTextEntity && pTextInfo && pGrView && charsNum);
	if(!pTextEntity ||	!pTextInfo || !pGrView || !charsNum)
		return 0;

	memset(&reorderingResults, 0, sizeof(GCP_RESULTS));

	// get metrics for this font
	if(!(pMetrics = ttfGetOutlineTextMetrics(pDC)))
	{
		bResult = false;
		goto exit;
	}

	// we create a font of fixed height (see CDoFont ::CreateTrueTypeFont), so we need some scaling
	Scales[1] = Scales[2] = pTextInfo->ht / pMetrics->otmAscent;
	Scales[0] = Scales[2] * pTextInfo->wfact;


	if(pTotalBoundingBox[1][2] < pTotalBoundingBox[0][2])		// we should calculate total BB
	{
		ttfGetBoundingBox(w_Text, !pTextInfo->vert, Scales, pMetrics, pDC, pTotalBoundingBox);
		PreviousBoundingBox[0][0] = PreviousBoundingBox[0][1] = PreviousBoundingBox[0][2] =
		PreviousBoundingBox[1][0] = PreviousBoundingBox[1][1] = PreviousBoundingBox[1][2] = 0.0;
		bNeedAdjustTotalBB = true;
	}

	// some preparing computations
	TotalHeight = pTotalBoundingBox[1][1] - pTotalBoundingBox[0][1];
	TotalWidth = pTotalBoundingBox[1][0] - pTotalBoundingBox[0][0];

	TanObl = pTextInfo->obl;
	ic_normang(&TanObl, NULL);
	if(fabs(TanObl - IC_PI * 0.5) >= IC_ZRO && fabs(TanObl - IC_PI * 1.5) >= IC_ZRO)
		TanObl = tan(TanObl);
	else
		TanObl = tan(TanObl - IC_ZRO);

	// allow different justifications: calculate insertion (starting) point and adjust total BB if necessary
	// Maybe TO DO: Make appropriate calculations for vertical text.
	if(pTextEntity->ret_type() == DB_TEXT)
	{
		pTextEntity->get_72(&HJustification);
		pTextEntity->get_73(&VJustification);
		if(!HJustification && !VJustification)
		{
			GR_PTCPY(InsertPoint, pTextInfo->llpt);
			if(bNeedAdjustTotalBB && !pTextInfo->vert)
			{
				pTotalBoundingBox[0][1] += pMetrics->otmDescent * Scales[1];
				pTotalBoundingBox[1][1] += pMetrics->otmDescent * Scales[1];
			}
			if(pTextInfo->vert)
				NextInsertPoint.y -= pMetrics->otmAscent * UpsidedownSign;
		}
		else
		{
			pTextEntity->get_11(InsertPoint);

			switch(HJustification)
			{
			case IC_TEXT_JUST_CENTER  :
				//progesoft 
				//if is printing don't need scale factor
				if (pGrView->isprinting)
				//Modified Cybage MM 24-12-2001 DD-MM-YYYY[
				//Reason : Fix for Bug# 77944 from BugZilla
					//InsertPoint[0]-=TotalWidth * 0.5 * BackwardSign;
				{
					InsertPoint[0]-= TotalWidth * 0.5 * CosAng;
					InsertPoint[1]-= TotalWidth * 0.5 * SinAng;
				}
				//Modified Cybage MM 24-12-2001 DD-MM-YYYY]
				else		
					NextInsertPoint.x -= TotalWidth * 0.5 * BackwardSign / Scales[0];
				
				if(bNeedAdjustTotalBB)
				{
					pTotalBoundingBox[0][0] -= TotalWidth * 0.5;
					pTotalBoundingBox[1][0] -= TotalWidth * 0.5;
				}
				break;

			case IC_TEXT_JUST_RIGHT   :
				//progesoft 
				// don't need scale factor
				if (pGrView->isprinting)
				//Modified Cybage MM 24-12-2001 DD-MM-YYYY[
				//Reason : Fix for Bug# 77944 from BugZilla
					//InsertPoint[0]-=TotalWidth * BackwardSign;
				{
					InsertPoint[0]-= TotalWidth * CosAng;
					InsertPoint[1]-= TotalWidth * SinAng;
				}
				//Modified Cybage MM 24-12-2001 DD-MM-YYYY]
				else
					NextInsertPoint.x -= TotalWidth * BackwardSign / Scales[0];
				
				if(bNeedAdjustTotalBB)
				{
					pTotalBoundingBox[0][0] -= TotalWidth;
					pTotalBoundingBox[1][0] -= TotalWidth;
				}
				break;

			case IC_TEXT_JUST_MIDDLE  :

				ASSERT(!VJustification);
				VJustification = 0;
				//progesoft 
				// if is printing don't need scale factor
				if (pGrView->isprinting)
				//Modified Cybage MM 24-12-2001 DD-MM-YYYY[
				//Reason : Fix for Bug# 77944 from BugZilla
					//InsertPoint[0]-=TotalWidth * 0.5 * BackwardSign;
				{
						InsertPoint[0]-= (TotalWidth * CosAng - (pMetrics->otmAscent + pMetrics->otmDescent) * Scales[1] * SinAng)* 0.5 ;
						InsertPoint[1]-= (TotalWidth * SinAng + (pMetrics->otmAscent + pMetrics->otmDescent) * Scales[1] * CosAng)* 0.5 ;
				}
				else
					//NextInsertPoint.x -= TotalWidth * 0.5 * BackwardSign / Scales[0];
				//NextInsertPoint.y -= (pMetrics->otmAscent + pMetrics->otmDescent) * 0.5 * UpsidedownSign;
				{
					NextInsertPoint.x -= TotalWidth * 0.5 * BackwardSign / Scales[0];
					NextInsertPoint.y -= (pMetrics->otmAscent + pMetrics->otmDescent) * 0.5 * UpsidedownSign;
				}
				//Modified Cybage MM 24-12-2001 DD-MM-YYYY]				
				if(bNeedAdjustTotalBB)
				{
					pTotalBoundingBox[0][0] -= TotalWidth * 0.5;
					pTotalBoundingBox[1][0] -= TotalWidth * 0.5;
				}
				break;

			case IC_TEXT_JUST_FIT     :
			case IC_TEXT_JUST_ALIGNED :

				ASSERT(!VJustification);
				VJustification = 0;

				ttfGetBoundingBox(w_Text, !pTextInfo->vert, Scales, pMetrics, pDC, CurrentBoundingBox);

				double	OldWidth = (CurrentBoundingBox[1][0] - CurrentBoundingBox[0][0]),
						NewWidth = gr_distance(InsertPoint, pTextInfo->llpt),
						ScaleFactor = NewWidth / (OldWidth < IC_ZRO ? 1.0 : OldWidth);

				TotalWidth = NewWidth;

				Scales[2] *= ScaleFactor;
				if(pTextInfo->vert)		// ??? this case works like as for non-TTF text but that working is not beautiful
				{
					Scales[1] *= ScaleFactor;
					if(HJustification == IC_TEXT_JUST_ALIGNED)
						Scales[0] *= ScaleFactor;
				}
				else
				{
					Scales[0] *= ScaleFactor;
					if(HJustification == IC_TEXT_JUST_ALIGNED)
						Scales[1] *= ScaleFactor;
				}

				GR_PTCPY(InsertPoint, pTextInfo->llpt);
				pTotalBoundingBox[1][0] = pTotalBoundingBox[0][0] + TotalWidth;
				if(pTextInfo->vert)
					NextInsertPoint.y -= pMetrics->otmAscent * UpsidedownSign;

			}	// switch on horizontal justifications


			switch(VJustification)
			{
			case IC_TEXT_VALIGN_BOTTOM	 :
				//progesoft
				if (!(pGrView->isprinting))
					NextInsertPoint.y -= pMetrics->otmDescent * UpsidedownSign ;
				else
				//Modified Cybage MM 24-12-2001 DD-MM-YYYY[
				//Reason : Fix for Bug# 77944 from BugZilla
				{
					//InsertPoint[1] -= pMetrics->otmDescent*Scales[1];
					InsertPoint[0] += pMetrics->otmDescent*Scales[1]*SinAng;
					InsertPoint[1] -= pMetrics->otmDescent*Scales[1]*CosAng;
				}
				//Modified Cybage MM 24-12-2001 DD-MM-YYYY]
				break;

			case IC_TEXT_VALIGN_MIDDLE	 :
				//progesoft
				if (!(pGrView->isprinting))
					NextInsertPoint.y -= pMetrics->otmAscent * 0.5 * UpsidedownSign;
				else
				//Modified Cybage MM 24-12-2001 DD-MM-YYYY[
				//Reason : Fix for Bug# 77944 from BugZilla
				{
					//InsertPoint[1] -= pMetrics->otmAscent*Scales[1] *0.5;
					InsertPoint[0] += pMetrics->otmAscent*Scales[1] *0.5*SinAng;
					InsertPoint[1] -= pMetrics->otmAscent*Scales[1] *0.5*CosAng;
				}
				//Modified Cybage MM 24-12-2001 DD-MM-YYYY]
				break;

			case IC_TEXT_VALIGN_TOP		 :
				//progesoft
				if (!(pGrView->isprinting))
					NextInsertPoint.y -= pMetrics->otmAscent * UpsidedownSign;
				else
				//Modified Cybage MM 24-12-2001 DD-MM-YYYY[
				//Reason : Fix for Bug# 77944 from BugZilla
				{
					//InsertPoint[1] -= pMetrics->otmAscent*Scales[1];
					InsertPoint[0] += pMetrics->otmAscent*Scales[1]*SinAng ;
					InsertPoint[1] -= pMetrics->otmAscent*Scales[1]*CosAng ;
				}
				//Modified Cybage MM 24-12-2001 DD-MM-YYYY]
				break;
				
			}	// switch on vertical justifications
 		
			if(bNeedAdjustTotalBB && (VJustification == IC_TEXT_VALIGN_BASELINE	||
									  VJustification == IC_TEXT_VALIGN_MIDDLE	||
									  VJustification == IC_TEXT_VALIGN_TOP))
			{
			
					pTotalBoundingBox[0][1] += pMetrics->otmDescent * Scales[1];
					pTotalBoundingBox[1][1] += pMetrics->otmDescent * Scales[1];
				
			}
		}	// have some non-default justification(s)
	}
	else
	{
		// TO DO... maybe...
		GR_PTCPY(InsertPoint, pTextInfo->llpt);
	}

	if(!ppEndDispObj || !ppBegDispObj)
		goto exit;

	ttfGetBoundingBox(w_Text, !pTextInfo->vert, Scales, pMetrics, pDC, CurrentBoundingBox);

	CurrentHeight = CurrentBoundingBox[1][1] - CurrentBoundingBox[0][1];
	CurrentWidth = CurrentBoundingBox[1][0] - CurrentBoundingBox[0][0];

	// shift to the current fragment
    // EBATECH(CNBR) -[ 2004/4/10 Bugzilla 78742 TEXT "abc%%udef" with angle.
	/*
	if(pTextInfo->vert)
		InsertPoint[1] += PreviousBoundingBox[1][1];
	else
		InsertPoint[0] += PreviousBoundingBox[1][0];
	*/
	if(pTextInfo->vert)
    {
	    InsertPoint[0] += PreviousBoundingBox[1][0] * CosAng;
	    InsertPoint[1] += PreviousBoundingBox[1][1] * SinAng;
    }
    else
    {
	    InsertPoint[0] += PreviousBoundingBox[1][0] * CosAng;
	    InsertPoint[1] += PreviousBoundingBox[1][0] * SinAng;
    }
    // EBATECH(CNBR) ]-

	// prepare shift for following fragments
	PreviousBoundingBox[1][0] += CurrentWidth;
	PreviousBoundingBox[1][1] += CurrentHeight;

	Scales[0] *=  BackwardSign;
	Scales[1] *=  UpsidedownSign;

	Dimension = 2 + (!pGrView->projmode || pGrView->noucs2rp);	// get the number of values per point in the point chain - i.e. 2 or 3

	*ppBegDispObj = *ppEndDispObj = NULL;

	//Vitaly Spirin. we need to consider difference between MM_TEXT and MM_ANISOTROPIC{
	SIZE	win_size, view_size;
	::GetWindowExtEx(pDC->GetSafeHdc(), &win_size);
    ::GetViewportExtEx(pDC->GetSafeHdc(), &view_size);
	Scales[0] *= ((double)win_size.cx) / view_size.cx;
    Scales[1] *= ((double)win_size.cy) / view_size.cy;
	//VS}
	/*if (pGrView->isprinting)
		NextInsertPoint.y*=Scales[1]*10;*/

	/*D.G.*/// Reordering Of Characters In The Hebrew & Arabic.
	// We store characters adequate indices in the reorderingResults.lpOrder array.

	int		reorderCharset;	// = 0 if we don't need any reordering
							// = HEBREW_CHARSET if we need a Hebrew reordering
							// = ARABIC_CHARSET if we need an Arabic reordering

	// do we need a reordering?
	reorderCharset = 0;

	// TO DO: test to be sure that we cover all Unicode values for the Hebrew & Arabic.
	for(SymbolNum = charsNum; SymbolNum--; )
	{
		if(0x0590 <= w_Text[SymbolNum] && w_Text[SymbolNum] <= 0x05FF) 		// Hebrew Unicode subrange
		{
			reorderCharset = HEBREW_CHARSET;
			break;
		}
		if(0x0600 <= w_Text[SymbolNum] && w_Text[SymbolNum] <= 0x06FF)		// Arabic Unicode subrange
		{
			reorderCharset = ARABIC_CHARSET;
			break;
		}
	}

	reorderingResults.lpOrder = new UINT [charsNum];	// used only for checking
	memset(reorderingResults.lpOrder,0,sizeof(UINT));
																		// if GetCharacterPlacementA couldn't reorder a string
	reorderingResults.lpOutString = (char *)new UINT [charsNum + 1];
	memset(reorderingResults.lpOutString ,0 ,sizeof(UINT)*(charsNum + 1));
	
	pWCString = new wchar_t[charsNum + 1];
	memset(pWCString,0,(charsNum + 1)* sizeof(wchar_t));

	UINT	tmpCodePage;	// used only if reordering needed

	if(reorderCharset)
	{	// fill the reordering array according to an appropriate order

		CFont	*pOldFont = NULL,
				tmpFont;
		LOGFONT	logFont;

		tmpCodePage = reorderCharset == HEBREW_CHARSET ? 1255 : 1256;

		pFont->GetLogFont(&logFont);

		if(logFont.lfCharSet != HEBREW_CHARSET && logFont.lfCharSet != ARABIC_CHARSET)
		{	// try to create&select a temporary font with Hebrew(or Arabic) charset.

			logFont.lfItalic = 0;
			logFont.lfWeight = 0;

			logFont.lfCharSet = reorderCharset;

			if(!tmpFont.CreateFontIndirect(&logFont))
			{
				bResult = false;
				goto exit;
			}
			if(!(pOldFont = pDC->SelectObject(&tmpFont)))
			{
				tmpFont.DeleteObject();
				bResult = false;
				goto exit;
			}

			if(GetTextCharset(pDC->GetSafeHdc()) != reorderCharset)
			{	// what TO DO ?..
				pDC->SelectObject(pOldFont);
				tmpFont.DeleteObject();
				reorderCharset = 0;
			}
		}	// current font doesn't require reordering

		if(reorderCharset)
		{
			char*	pStr = (char*)new wchar_t[charsNum+1];
			memset(pStr,0,sizeof(wchar_t)*(charsNum+1));

			WideCharToMultiByte(tmpCodePage,
								0,
								w_Text,
								charsNum,
								pStr,
								charsNum + 1,
								NULL,
								NULL);

			reorderingResults.lStructSize = sizeof(GCP_RESULTS);
			reorderingResults.nGlyphs = charsNum;

			// fill the indices array
			if(!GetCharacterPlacementA(pDC->GetSafeHdc(), pStr, charsNum, 0, &reorderingResults, GCP_REORDER))
			{
				bResult = false;
				IC_FREE(pStr);
				goto exit;
			}

			if(reorderingResults.lpOrder[0] == reorderingResults.lpOrder[charsNum-1])
				reorderCharset = 0;		// TO DO: couldn't reorder by the GetCharacterPlacementA.

			IC_FREE(pStr);

			pDC->SelectObject(pOldFont);
			tmpFont.DeleteObject();
		}
	}	// reordering needed

	if(reorderCharset)		// note: not under 'else' because of possible failure of the previous
		MultiByteToWideChar(tmpCodePage,
							0,
							reorderingResults.lpOutString,
							-1,
							pWCString,
							charsNum + 1);
	else
		wcscpy(pWCString, w_Text);

	if(pTextInfo->m_bReorderFlag)
	{
		// move all ending spaces to begin: special for mtext
		wchar_t	*pTmpStr = new wchar_t[charsNum + 1],
				*pSpaces;

		for(pSpaces = pWCString + charsNum - 1; *pSpaces == L' '; --pSpaces)
			;
		wcscpy(pTmpStr, ++pSpaces);
		*pSpaces = L'\0';
		wcscat(pTmpStr, pWCString);
		wcscpy(pWCString, pTmpStr);
		delete [] pTmpStr;
	}


	// ********************** MAIN LOOP thru string characters **********************
	for(SymbolNum = 0; SymbolNum < charsNum; ++SymbolNum)
	{

		LPTTPOLYGONHEADER	pPolygonHeaderBuffer = NULL;
		DWORD				BufferSize;
		GlyphContours		GlyphContoursList;
		short				XShiftToNext;
		gr_displayobject	*pNewDispObj, *pDOIterator;
		int					PointNum;

		GlyphContours::iterator		pContour;
		Contour::iterator			pSegment;

		// get outlines data in native format buffer
		if((BufferSize = ttfGetGlyphOutlinesData(pWCString[SymbolNum], pDC, pFontTable, &pPolygonHeaderBuffer, &XShiftToNext)) == GDI_ERROR)
		{
			bResult = false;
			goto out;
		}

		// extract outlines data
		if(BufferSize)
			if(!ttfDecodeOutlines(pPolygonHeaderBuffer, BufferSize, GlyphContoursList))
			{
				bResult = false;
				goto out;
			}

		// add underscore/overscore data if necessary
		if(pTextInfo->vert || !SymbolNum)
		{
			if(FontModifiers & CTextFragmentStyle::EModifiers::eUNDERLINED)
			{
				ContourSegment*	pContourSegment = new ContourSegment(2);
				Contour*		pContour = new Contour;

				// create a contour with 4 line segments
				pContourSegment->SegmentPoints[0].x = 0.0;
				pContourSegment->SegmentPoints[0].y = pMetrics->otmsUnderscorePosition;

				pContourSegment->SegmentPoints[1].x = 0.0;
				pContourSegment->SegmentPoints[1].y = pMetrics->otmsUnderscorePosition + pMetrics->otmsUnderscoreSize * 0.5;	// underlines seems too bold

				pContour->push_back(pContourSegment);
				
				pContourSegment = new ContourSegment(1);

				pContourSegment->SegmentPoints[0].x = CurrentWidth / fabs(Scales[0]);
				pContourSegment->SegmentPoints[0].y = pMetrics->otmsUnderscorePosition + pMetrics->otmsUnderscoreSize * 0.5;	// underlines seems too bold

				pContour->push_back(pContourSegment);

				pContourSegment = new ContourSegment(1);

				pContourSegment->SegmentPoints[0].x = CurrentWidth / fabs(Scales[0]);
				pContourSegment->SegmentPoints[0].y = pMetrics->otmsUnderscorePosition;

				pContour->push_back(pContourSegment);

				pContourSegment = new ContourSegment(1);
				
				pContourSegment->SegmentPoints[0].x = 0.0;
				pContourSegment->SegmentPoints[0].y = pMetrics->otmsUnderscorePosition;

				pContour->push_back(pContourSegment);

				GlyphContoursList.push_back(pContour);
			}

			if(FontModifiers & CTextFragmentStyle::EModifiers::eOVERLINED)
			{
				ContourSegment*	pContourSegment = new ContourSegment(2);
				Contour*		pContour = new Contour;

				// create a contour with 4 line segments
				pContourSegment->SegmentPoints[0].x = 0.0;
				pContourSegment->SegmentPoints[0].y = pMetrics->otmTextMetrics.tmHeight;

				pContourSegment->SegmentPoints[1].x = 0.0;
				pContourSegment->SegmentPoints[1].y = pMetrics->otmTextMetrics.tmHeight - pMetrics->otmsUnderscoreSize * 0.5;	// underlines seems too bold

				pContour->push_back(pContourSegment);

				pContourSegment = new ContourSegment(1);

				pContourSegment->SegmentPoints[0].x = CurrentWidth / fabs(Scales[0]);
				pContourSegment->SegmentPoints[0].y = pMetrics->otmTextMetrics.tmHeight - pMetrics->otmsUnderscoreSize * 0.5;	// underlines seems too bold

				pContour->push_back(pContourSegment);

				pContourSegment = new ContourSegment(1);

				pContourSegment->SegmentPoints[0].x = CurrentWidth / fabs(Scales[0]);
				pContourSegment->SegmentPoints[0].y = pMetrics->otmTextMetrics.tmHeight;

				pContour->push_back(pContourSegment);

				pContourSegment = new ContourSegment(1);

				pContourSegment->SegmentPoints[0].x = 0.0;
				pContourSegment->SegmentPoints[0].y = pMetrics->otmTextMetrics.tmHeight;

				pContour->push_back(pContourSegment);

				GlyphContoursList.push_back(pContour);
			}
		}

		// transform outlines data; note, z coords of points are assumed to be zero till the moment
		for(pContour = GlyphContoursList.begin(); pContour != GlyphContoursList.end(); ++pContour)
			for(pSegment = (*pContour)->begin(); pSegment != (*pContour)->end(); ++pSegment)
				for(PointNum = 0; PointNum < (*pSegment)->PointsCount; ++PointNum)
					ttfCStoUCS(NextInsertPoint, TanObl, Scales, CosAng, SinAng, InsertPoint, pGrView, (*pSegment)->SegmentPoints[PointNum]);

		// convert outlines data to display objects
		for(pContour = GlyphContoursList.begin(); pContour != GlyphContoursList.end(); ++pContour)
		{
			if(pGrView->m_Clipper.IsNotEmpty())
				pNewDispObj = ttfCreateLinearDO(pTextEntity, (**pContour), Dimension, pGrView);	// note, NULL result is allowable
			else
			{
				if(!(pNewDispObj = ttfCreateBezierDO(pTextEntity, (**pContour), Dimension, pGrView)))
				{
					bResult = false;
					goto out;
				}
			}

			if(pNewDispObj)
			{
				// link display list
				for(pDOIterator = pNewDispObj; pDOIterator->next; pDOIterator = pDOIterator->next)
					;

				if(pTmpBegDispObj)
					pTmpEndDispObj->next = pNewDispObj;
				else
					pTmpBegDispObj = pNewDispObj;

				pTmpEndDispObj = pDOIterator;
			}
		}	// for thru glyph contours

		/*DG - 14.4.2003*/// GDI fuctions for stroking/filling paths are very slow when the whole text is given to them,
		// so let them work in per-symbol mode.
		if(pTmpEndDispObj)
			pTmpEndDispObj->type |= DISP_OBJ_ENDPATH;

		out:

		delete pPolygonHeaderBuffer;
		pPolygonHeaderBuffer = NULL;
		for(pContour = GlyphContoursList.begin(); pContour != GlyphContoursList.end(); ++pContour)
		{
			for(pSegment = (*pContour)->begin(); pSegment != (*pContour)->end(); ++pSegment)
				delete *pSegment;
			(*pContour)->clear();
			delete *pContour;
		}

		if(!bResult)
			break;

		if(pTextInfo->vert)
			NextInsertPoint.y -= pMetrics->otmTextMetrics.tmHeight;
		else
			NextInsertPoint.x += XShiftToNext;

	}
	// ********************** the END of the MAIN LOOP thru string characters **********************

	exit:
	delete [] reorderingResults.lpOrder;
	delete [] reorderingResults.lpOutString;
	delete [] pWCString;

	delete pMetrics;

	if(bResult && ppBegDispObj && ppEndDispObj)
	{
		*ppBegDispObj = pTmpBegDispObj;
		*ppEndDispObj = pTmpEndDispObj;
	}
	else
		gr_freedisplayobjectll(pTmpBegDispObj);

	return (int)bResult;
}

#pragma warning (default : 4786)
