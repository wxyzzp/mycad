/*	
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *	
 * Abstract
 *	
 * Breaks up a point chain according to line type and returns the appropriate gr_displayobject list
 *	
 */ 

#include "gr.h"
#include "gr_view.h"
#include "ltypetabrec.h"
#include "textgen.h"
#include "pc2dispobj.h"
#include "DoBase.h"

extern gr_view gr_defview;

/*********************************************************************************************
* Author:		Denis Petrov
* Description:	Helper function. Used in gr_doltype() for generation of solid line or dots, 
*				when distance between points doesn't allow to build pattern.
*********************************************************************************************/
short gr_doLine(db_entity* pSourceEntity, 
				gr_displayobject **ppDOListBegin, gr_displayobject **ppDOListEnd, 
				gr_view *pView, gr_pclink** ppInitialPoint, gr_pclink** ppFinalPoint, 
				long& finalPointIndex, sds_real& pointsChainLength,	BOOL allDots);

/*********************************************************************************************
* Author:		Chikahiro Masami
* Description:	Helper function. Extract from doPatternElement() for generation of text or shape. 
*********************************************************************************************/
short gr_doPattern(
	gr_displayobject **ppDOListBegin, 
	gr_displayobject **ppDOListEnd,
	gr_view *pView, 
	db_drawing* pDrawing, 
	CDC* pDC,
	db_ltypetabrec* pLinetype,
	short elementIndex,
	gr_pclink& realInitialPoint,
	gr_pclink& realFinalPoint,
    long finalPointIndex, 
	sds_real scaleFactor,
	int useInitialPoint
	);
/*********************************************************************************************
* Author:		Denis Petrov
* Description:	Helper function. Used in gr_doltype() for generation of one linetype pattern 
*				element.
*********************************************************************************************/
short gr_doPatternElement(db_entity* pSourceEntity, 
						  gr_displayobject **ppDOListBegin, gr_displayobject **ppDOListEnd,
						  gr_view *pView, db_drawing* pDrawing, CDC* pDC,
						  db_ltypetabrec* pLinetype, short elementIndex,
						  gr_pclink*& pInitialPoint, gr_pclink*& pFinalPoint, 
						  long& finalPointIndex, gr_pclink& realInitialPoint,
						  sds_real& pointsChainLength, sds_real& mainDoneDistance, 
						  sds_real size, sds_real scaleFactor);

short gr_doltype(
	db_entity				*pSourceEntity,
	struct gr_displayobject **begpp,
	struct gr_displayobject **endpp,
	gr_view 		  *pView,
	db_drawing* pDrawing /* = NULL */,
	CDC* pDC /* = NULL */) 
{

/*
**	Reads point chain pView->pc and its associated members
**	and breaks it up according to linetype.  Each solid fragment
**	is passed on to gr_gensolid() to generate display objects.
**	The pieces allocated are gathered into one llist of all allocated
**	display objects for pView->pc, which is returned to the caller via
**	*begpp and *endpp (which are pointed to the beginning and ending
**	links).
**
**	To begin and end a linetype generation interval with a dash (or
**	a dot plus some space), ACAD seems to apply the following
**	formula:
**
**		L : Length of the chain for generation (whole entity or
**			  one seg of a 2D POLYLINE)
**		P : Linetype pattern length (adjusted by scaling factors)
**		F : Length of the first dash in the linetype def
**		S : The length of the 1st and last dash or dot+space
**
**		S=(modf(L/P,&dummy)*P+F)/2.0;
**
**	This came from the empirical observation that the chain gets
**	broken up such that it contains an integral number of whole
**	patterns MINUS ONE FIRST-DASH, and that whatever is left over
**	is divided evenly between the beginning and end.
**
**	Returns: gr_getdispobjs() error codes (0 or -3 (no memory)).
*/
	short rc = 0;
	short didx,closed,capped,eachseg,alldot,fitted;
	long patidx,p1idx,nintern;
	int menttype,fint1;
	sds_real pcdist,enddash,fpat,ar1,ar2,ar3;
	sds_real donedist,newdonedist,p0p1dist,p0ewid;
	sds_point ucdir;
	struct gr_displayobject *begdo = NULL, *enddo = NULL, *tdop1 = NULL, *tdop2 = NULL;

	sds_real linetypeScaleFactor;
	pView->ment->get_48(&linetypeScaleFactor); 
	linetypeScaleFactor *= pView->ltscale;
	if(linetypeScaleFactor < 0.0) 
		linetypeScaleFactor = -linetypeScaleFactor;
		if(icadRealEqual(linetypeScaleFactor,0.0))
			linetypeScaleFactor = 1.0;

	db_ltypetabrec* pLinetype;
	int i;
	sds_real textStyleParam = 0.0;
	sds_real patternLength = 0.0;
	long nPatterns = 0;
	sds_real elementsLens[DB_MAXLTDASHES];
	struct gr_pclink *ilink,*p0,*p1;
	struct gr_pclink begpcl, endpcl;
	memset(&begpcl,0,sizeof(begpcl));
	memset(&endpcl,0,sizeof(endpcl));
	gr_displayobject* pLTTextDO1 = NULL;
	gr_displayobject* pLTTextDO2 = NULL;
	gr_pclink* pPCFirstLink = NULL;
	gr_pclink* pPCLastLink = NULL;
	gr_pclink* pPCLastLinkUsed = NULL;
	long nPoints = 0;
	long nPointsUsed = 0;

	begdo = enddo = NULL;
	if(pView == NULL || icadRealEqual(pView->viewsize,0.0)) 
		pView = &gr_defview;
	if(pView->upc < 1) 
		goto out;

#ifdef _DOCLASSES_
	CDoBase::get().setEntity(pSourceEntity);
	CDoBase::get().setViewData(pView);
#endif

	closed = capped = eachseg = 0;
	if(pView->upc == 1) 
	{  /* Just a dot (no direction). */
#ifdef _DOCLASSES_
		CDoBase::get().setPointsData(pView->pc, pView->pc, NULL, 0);
		CDoBase::get().setFlags(0, 0);
		CDoBase::get().setDotDir(NULL);
		if((rc = CDoBase::get().getDisplayObjects(tdop1, tdop2)) != 0) 
#else
		if((rc = gr_gensolid(pSourceEntity, pView->pc, pView->pc, NULL, 0, 0, 0, NULL, &tdop1, &tdop2, pView)) != 0) 
#endif
			goto out;
		if(tdop1 && tdop2) 
		{ 
			begdo = tdop1; 
			enddo = tdop2; 
		}
		goto out;
	}
	if(pView->ment) 
	{
		menttype = pView->ment->ret_type();
		if(menttype==DB_LWPOLYLINE)
			menttype=DB_POLYLINE;		// for our purposes
		if(menttype==DB_CIRCLE) 
		{
			closed = capped = 1;
		} 
		else
		{
			if(menttype==DB_ELLIPSE) 
			{
				pView->ment->get_41(&ar1); ic_normang(&ar1,NULL);
				pView->ment->get_42(&ar2); ic_normang(&ar2,NULL);
				if((fabs(ar1) < IC_ZRO || fabs(ar1 - IC_TWOPI) < IC_ZRO) &&
					(fabs(ar2) < IC_ZRO || fabs(ar2 - IC_TWOPI) < IC_ZRO) &&
						pView->upc>2) closed = 1;
			}
			else 
				if(menttype==DB_POLYLINE) 
				{
					pView->ment->get_70(&fint1);
					if(fint1 & IC_PLINE_CLOSED)
						closed = 1;
					if(!(fint1 & IC_PLINE_CONTINUELT))
						eachseg = 1;
				} 
				else 
					if(menttype==DB_SPLINE)
					{
						pView->ment->get_70(&fint1);
						if(fint1 & IC_SPLINE_PERIODIC) 
							closed = 1;/* I think this is right: check the */
									   /* "periodic" bit */
					}
		}
	}
	fitted = (closed && !eachseg);/* CIRCLE, ELLIPSE, Periodic SPLINE, closed */
								  /* 2D POLYLINE with continuous pattern */
								  /* generation. */

	pLinetype = (db_ltypetabrec*)pView->lthip;
	if(pLinetype == NULL)
		goto docont;

	alldot = 0; 
	if(pLinetype->nElements() > DB_MAXLTDASHES) // 12-> 2003/11/20 Use define (CNBR)
	{
		ASSERT(false);
		goto out;
	}

	if(pLinetype->nElements() > 1) 
	{
		/* Set up elementsLens[] as the working array of dash lengths -- scaled */
		/* by LTSCALE and the entity's own ltype scale: */
		alldot = 1;  /* Assume all dots until we find a dash. */
		for(i = 0; i < pLinetype->nElements(); ++i) 
		{
			elementsLens[i] = linetypeScaleFactor * pLinetype->element(i).len;
			patternLength += fabs(elementsLens[i]);
			if(elementsLens[i] > 0.0) 
				alldot = 0;  /* Found a dash */
		}
		assert(elementsLens[0] >= 0.0);
	}
							/* CONTINUOUS */
	if (patternLength <= 0.0 || patternLength / fabs(pView->GetPixelHeight()) < 3.0) 
	{
	  docont:

		if((nintern = pView->upc - 2L) < 0L) 
			nintern=0L;  /* # of internal pts */

		ilink = (nintern < 1L) ? NULL : pView->pc->next;  /* 1st internal pt */
#ifdef _DOCLASSES_
		CDoBase::get().setPointsData(pView->pc, pView->pc, ilink, nintern);
		CDoBase::get().setFlags(closed, capped);
		CDoBase::get().setDotDir(NULL);
		if((rc = CDoBase::get().getDisplayObjects(tdop1, tdop2)) != 0) 
#else
		if((rc = gr_gensolid(pSourceEntity, pView->pc, pView->lupc, ilink, nintern,
			closed, capped, NULL, &tdop1, &tdop2, pView)) != 0)
#endif			
			goto out;
		if(tdop1 && tdop2) 
		{ 
			begdo=tdop1; 
			enddo=tdop2; 
		}
		goto out;
	}
						  /* NON-CONTINUOUS */
	if(fitted) 
	{
		/* Closed, continuous gen: fit an exact # of patterns around it. */
		if((ar1 = pView->pc->d2go / patternLength) < 1.0) 
			goto docont;
		ar2 = modf(ar1, &ar3);
		/* Never just 1 pattern for fitted entities: */
		if((nPatterns = ((long)ar3) + (ar2 >= 0.5)) < 2L) 
			nPatterns=2L;
		fpat = 0.0; /* Fraction of a pattern left over.  Because we're */
				   /* scaling the dashes to ensure an exact fit, it's zero */
				   /* here; fpat can be non-zero in other cases. */

		/* Scale elementsLens[] and patternLength to exactly fit: */
		ar1 /= nPatterns;
		for(i = 0; i < pLinetype->nElements(); ++i) 
			elementsLens[i] *= ar1;
		patternLength *= ar1;
	}


	/*
	**	There are several important variables for breaking up
	**	pView->pc into linetype pieces in the loop below:
	**
	**		  p0 and p1 : The endpoints of the chord of pView->pc that is
	**						currently being worked on.	(These aren't
	**						sds_points.  They are ptrs to gr_pclinks
	**						(which contain the points, widths, etc.)).
	**			  p1idx : The 0-based index of p1 in the pView->pc chain
	**						(for determining when we're done).
	**		   p0p1dist : The length of the current p0p1 chord.
	**			 p0ewid : The ORIGINAL ewid that p0 had.  (This function
	**						changes ewid for p0 whenever a dash wraps
	**						around p0 but doesn't extend to p1.)
	**			  ucdir : The unit chord-direction vector from p0->pt
	**						to p1->pt.	(Used for finding endpcl.pt by
	**						stepping back from p1->pt toward p0->pt
	**						by a specified amount.	Also used for telling
	**						gr_getwidepc() the direction the pc is going
	**						at the time a dot is generated.)
	**			 pcdist : The distance in the chain from the beginning
	**						(pView->pc) through p1.  This is generally
	**						kept ahead of donedist as we work.
	**		   donedist : The distance along the chain that has been
	**						processed so far (the distance we've applied
	**						the pattern to).
	**		 begpcl and
	**			 endpcl : Struct gr_pclinks (that may contain .pts that
	**						are mid-chord) that are passed to gr_gensolid()
	**						as the endpoints of a solid section.
	**						Their swid and ewid members are set based on
	**						interpolation between p0 and p1.
	**			  ilink : Passed to gr_gensolid().	The first internal link
	**						in pView->pc to use (the point after
	**						begpcl.pt).  Can be NULL.
	**			nintern : Passed to gr_gensolid().	The number of internal
	**						pView->pc links to use before using
	**						endpcl.pt.	Can be 0.
	**
	**	Note that for "fitted" entities, we avoid the 1st/last dash
	**	code and just do nPatterns full patterns.
	*/


	/* INIT STUFF FOR THE LOOP: */

	p0 = pView->pc; 
	p1 = p0->next;
	p1idx = 1; 
	p0ewid = p0->ewid;
	pcdist = p0p1dist = GR_DIST(p0->pt,p1->pt);
	donedist = 0.0;

	/* Let's define the p0/p1 stepping stuff since it's used so often */
	/* and undef it later): */
#define GR_STEPP0P1 p0=p1; p1=p0->next; p1idx++; p0ewid=p0->ewid

	for (;;) {	/* Each generation zone */

		/* Make sure we always start a new generation zone here */
		/* at the top of this loop. */
		nintern = 0L; 
		ilink=NULL;
		memcpy(&begpcl, p0, sizeof(begpcl));

		// 2004/1/8 EBATECH(CNBR) *FIX* polyline plinegen=1 lost last short segment.
        //if( p0->d2go < 0.0 )
        //    goto out;

		if(!fitted && p0->d2go / patternLength < 1.0) 
		{
			// less than 1 pattern in this generation zone,
			// make it solid or 2 dots if the pattern only has dots
			rc = gr_doLine(pSourceEntity, &begdo, &enddo, pView, &p0, &p1, p1idx, pcdist, alldot);
			if(rc != 0)
				goto out;
		}
		else
		{
			if (!fitted) 
			{  /* (Already set fpat and nPatterns for fitted earlier.) */
				/* Determine # of whole patterns and the fraction left over: */
				fpat = modf(p0->d2go / patternLength, &ar1); 
				nPatterns = (long)ar1;
			}
			
			/*
			**	TODO
			**
			**	Here's a super-flako way to avoid (for now) a big problem
			**	we have: an entity that just has too many linetype patterns
			**	in it.	When you're zoomed out to view the whole entity,
			**	no problem: it's solid because one pattern is less than
			**	3 pixels long (see docont).  But, if we're zoomed way in
			**	and see 10 patterns across the screen, the whole entity
			**	has thousands (off-screen).  That's a problem; we'll need to
			**	clip for too many linetype patterns (just as we'll have to
			**	clip arcs and circles for too many segments).
			**
			**	For now, if this generation zone needs more than 1000
			**	patterns, it's continous (at ALL zooms):
			*/
			if (nPatterns > 1000) 
				goto docont;
			
			/* Figure out how long the first/last dash should be. */
			/* (See comments at top of function about the formula): */
			enddash = 0.5 * (fpat * patternLength + elementsLens[0]);
			
			/* Step over 0-length chords and get the unit chord direction: */
			ucdir[0] = p1->pt[0] - p0->pt[0];
			ucdir[1] = p1->pt[1] - p0->pt[1];
			ucdir[2] = p1->pt[2] - p0->pt[2];
			p0p1dist = GR_LEN(ucdir);
			while(icadRealEqual(p0p1dist,0.0))
			{
				if(p1idx >= pView->upc-1) 
					goto out;  /* Done */
				GR_STEPP0P1;
				ucdir[0] = p1->pt[0] - p0->pt[0];
				ucdir[1] = p1->pt[1] - p0->pt[1];
				ucdir[2] = p1->pt[2] - p0->pt[2];
				pcdist += (p0p1dist = GR_LEN(ucdir));
			}
			GR_UNITIZE(ucdir, p0p1dist);
			/* -------------------------------- */
			/* Do 1st dash or dot in this zone: */
			/* -------------------------------- */
			if (!fitted) {
				nintern=0L; ilink=NULL;  /* begpcl was set at the top of the loop. */
				newdonedist=donedist+enddash;
				/* Walk until we're at or beyond the place this dash reaches: */
				while (newdonedist>pcdist) {
					if (p1idx>=pView->upc-1) goto out;	/* Done? */
					GR_STEPP0P1;
					ucdir[0]=p1->pt[0]-p0->pt[0];
					ucdir[1]=p1->pt[1]-p0->pt[1];
					ucdir[2]=p1->pt[2]-p0->pt[2];
					pcdist+=(p0p1dist=GR_LEN(ucdir));
					if(!icadRealEqual(p0p1dist,0.0)) // ???
					{
						GR_UNITIZE(ucdir,p0p1dist);
						if (ilink==NULL) ilink=p0;
						nintern++;
					}
				}
				ar1=pcdist-newdonedist;  /* Dist from end of dash to p1->pt */
				/* Get endpcl.pt by stepping back from p1->pt toward p0->pt: */
				endpcl.pt[0]=p1->pt[0]-ar1*ucdir[0];
				endpcl.pt[1]=p1->pt[1]-ar1*ucdir[1];
				endpcl.pt[2]=p1->pt[2]-ar1*ucdir[2];
				
				/*
				**	TAPER THE WIDTHS.  TRICKY!
				**	Set the endpcl's widths and the ewid of the vertex just
				**	before it (which may be begpcl or p0). Interpolate
				**	between p0's original widths to get endpcl.swid (and the
				**	prev vertex's ewid). Set endpcl.ewid to p0ewid (the
				**	original p0->ewid). (Note that the endpcl widths are
				**	being set here so that, when it becomes begpcl, it will
				**	have the correct widths to reach p1.  Of course the ewid
				**	may be overridden by THAT pass.)
				*/
				endpcl.swid=ar1/p0p1dist*(p0->swid-p0ewid)+p0ewid;
				endpcl.ewid=p0ewid;
				if (ilink==NULL) begpcl.ewid=endpcl.swid;
				else				p0->ewid=endpcl.swid;
				// MR -- add similar interpolation for the angles
				if (p0->startang==IC_ANGLE_DONTCARE || p0->endang==IC_ANGLE_DONTCARE) {
					endpcl.startang=p0->startang;
					endpcl.endang=p0->endang;
				}
				else {
					endpcl.startang=ar1/p0p1dist*(p0->startang-p0->endang)+p0->endang;
					endpcl.endang=p0->endang;
					if (ilink==NULL) begpcl.endang=endpcl.startang;
					else				p0->endang=endpcl.startang;
				}
				
				/* Make it: */
				if (icadRealEqual(elementsLens[0],0.0))
				{	// dot 
#ifdef _DOCLASSES_
					CDoBase::get().setPointsData(&begpcl, &begpcl, NULL, 0);
					CDoBase::get().setFlags(0, 0);
					CDoBase::get().setDotDir(ucdir);
					if((rc = CDoBase::get().getDisplayObjects(tdop1, tdop2)) != 0) 
#else
					if((rc = gr_gensolid( pSourceEntity, &begpcl,&begpcl,NULL,0,0,0,ucdir,&tdop1,&tdop2,
						pView)) != 0) 
#endif
						goto out;
				} 
				else 
				{	// dash 
#ifdef _DOCLASSES_
					CDoBase::get().setPointsData(&begpcl, &endpcl, ilink, nintern);
					CDoBase::get().setFlags(0, 0);
					CDoBase::get().setDotDir(NULL);
					if((rc = CDoBase::get().getDisplayObjects(tdop1, tdop2)) != 0) 
#else
					if((rc=gr_gensolid( pSourceEntity, &begpcl,&endpcl,ilink,nintern,0,0,NULL,&tdop1,
						&tdop2,pView))!=0) 
#endif
						goto out;
				}
				if(tdop1 != NULL && tdop2 != NULL) 
				{
					if(begdo == NULL) 
						begdo = tdop1;
					else 
						enddo->next = tdop1;
					enddo = tdop2;
					tdop1 = NULL;
					tdop2 = NULL;
				}
				// EBATECH(CNBR) -[ Draw first text or shape
				if (icadRealEqual(elementsLens[0],0.0) && icadRealEqual(fpat, 0.0))
				{
					gr_doPattern( &tdop1, &tdop2, pView, pDrawing, pDC,
						pLinetype, 0, begpcl, *begpcl.next, p1idx, linetypeScaleFactor, 1);
				}
				else
				{
					gr_doPattern( &tdop1, &tdop2, pView, pDrawing, pDC,
						pLinetype, 0, begpcl, endpcl, p1idx, linetypeScaleFactor, 0);
				}
				if(tdop1 != NULL && tdop2 != NULL) 
				{
					if(begdo == NULL) 
						begdo = tdop1;
					else 
						enddo->next = tdop1;
					enddo = tdop2;
					tdop1 = NULL;
					tdop2 = NULL;
				}
				memcpy(&begpcl, &endpcl, sizeof(begpcl)); 
				donedist = newdonedist;
				// EBATECH(CNBR) ]-
			}  /* End if !fitted (do 1st dash). */
			
			
			/* -------------------------------------------------------------- */
			/* Do nPatterns patterns (but skip the very 1st dash if !fitted): */
			/* -------------------------------------------------------------- */
			for(patidx = 0L; patidx < nPatterns; patidx++) 
			{  /* Each pattern rep */
				for(didx = (!patidx && !fitted); didx < pLinetype->nElements(); didx++) 
				{ /* Each dash spec */
					rc = gr_doPatternElement(pSourceEntity, &tdop1, &tdop2, pView, pDrawing, pDC,
						pLinetype, didx, p0, p1, p1idx, begpcl, pcdist, donedist, elementsLens[didx], linetypeScaleFactor);
					if (tdop1 != NULL && tdop2 != NULL) 
					{
						if (begdo == NULL) 
							begdo = tdop1;
						else 
							enddo->next = tdop1;
						enddo = tdop2;
						tdop1 = NULL;
						tdop2 = NULL;
					}
				}  /* End for each dash */
			}  /* End for each pattern rep */
			if(fitted) 
				goto out;
			/* --------------------------------------- */
			/* Do the last dash (or dot) in this zone: */
			/* --------------------------------------- */
			nintern=0L; ilink=NULL;
			/* Walk until p1 reaches the 1st pt of the next generation zone: */
			while (p1->d2go<0.0 && p1idx<pView->upc-1-1) {	// extra -1 by MR here
				GR_STEPP0P1;
				ucdir[0]=p1->pt[0]-p0->pt[0];
				ucdir[1]=p1->pt[1]-p0->pt[1];
				ucdir[2]=p1->pt[2]-p0->pt[2];
				pcdist+=(p0p1dist=GR_LEN(ucdir));
				while (icadRealEqual(p0p1dist,0.0) && p1idx<pView->upc-1-1) { // MR -- points atop each other?
					GR_STEPP0P1;
					ucdir[0]=p1->pt[0]-p0->pt[0];
					ucdir[1]=p1->pt[1]-p0->pt[1];
					ucdir[2]=p1->pt[2]-p0->pt[2];
					pcdist+=(p0p1dist=GR_LEN(ucdir));
				}
				if (p1idx>=pView->upc-1) break;
				GR_UNITIZE(ucdir,p0p1dist);
				if (ilink==NULL) ilink=p0;
				nintern++;
			}
			/* Make it: */
			if (icadRealEqual(elementsLens[0],0.0)) 
			{	// dot
#ifdef _DOCLASSES_
				CDoBase::get().setPointsData(p1, p1, NULL, 0);
				CDoBase::get().setFlags(0, 0);
				CDoBase::get().setDotDir(ucdir);
				if((rc = CDoBase::get().getDisplayObjects(tdop1, tdop2)) != 0) 
#else
				if((rc = gr_gensolid(pSourceEntity, p1,p1,NULL,0,0,0,ucdir,&tdop1,&tdop2,pView))!=0)
#endif
					goto out;
			} else 
			{  
				// dash
#ifdef _DOCLASSES_
				CDoBase::get().setPointsData(&begpcl, p1, ilink, nintern);
				CDoBase::get().setFlags(0, 0);
				CDoBase::get().setDotDir(ucdir);
				if((rc = CDoBase::get().getDisplayObjects(tdop1, tdop2)) != 0) 
#else
				if((rc = gr_gensolid(pSourceEntity, &begpcl,p1,ilink,nintern,0,0,NULL,&tdop1,&tdop2,
					pView))!=0) 
#endif
					goto out;
			}
			if (tdop1 != NULL && tdop2 != NULL) 
			{
				if (begdo == NULL) 
					begdo = tdop1;
				else 
					enddo->next = tdop1;
				enddo = tdop2;
			}
			// EBATECH(CNBR) -[ Draw last text or shape
			if( !closed && didx >= pLinetype->nElements() && icadRealEqual(fpat,0.0) && icadRealEqual(elementsLens[0],0.0))
			{
				tdop1 = 0;
				tdop2 = 0;
				gr_doPattern( &tdop1, &tdop2, pView, pDrawing, pDC,
					pLinetype, 0, *p0,	*p1, p1idx, linetypeScaleFactor, 0);
				if(tdop1 != NULL && tdop2 != NULL) 
				{
					if(begdo == NULL) 
						begdo = tdop1;
					else 
						enddo->next = tdop1;
					enddo = tdop2;
				}
			}
			// EBATECH(CNBR) ]-
		}
		donedist = pcdist;
		if (p1idx>=pView->upc-1) 
			goto out;  /* Done */
		
		/* Step p0 to 1st pt of the next generation zone: */
		GR_STEPP0P1;
		pcdist+=(p0p1dist=GR_DIST(p0->pt,p1->pt));
	}  /* End for each generation zone */

#undef GR_STEPP0P1
out:
	if(rc) 
	{ 
		gr_freedisplayobjectll(begdo); 
		begdo = enddo = NULL; 
	}
	*begpp = begdo; 
	*endpp = enddo;
	return rc;
}

short gr_doLine(db_entity* pSourceEntity, 
				gr_displayobject **ppDOListBegin, gr_displayobject **ppDOListEnd, 
				gr_view *pView, gr_pclink** ppInitialPoint, gr_pclink** ppFinalPoint, 
				long& finalPointIndex, sds_real& pointsChainLength,	BOOL allDots)
{
	short result = 0;
	long nInternalPoints = 0;
	gr_pclink* pInternalPointLink = NULL;
	gr_pclink* pPrevPointLink = NULL;
	gr_displayobject* pDO1 = NULL;
	gr_displayobject* pDO2 = NULL;
	sds_real distance = 0.0;
	sds_point direction;
	/* Walk until p1 is at the beginning of a new gen zone or */
	/* the end of the chain: */
	while(((*ppFinalPoint)->d2go < 0.0) && (finalPointIndex < pView->upc - 1)) 
	{
		pPrevPointLink = *ppFinalPoint;
		*ppFinalPoint = pPrevPointLink->next;
		++finalPointIndex;
		distance = GR_DIST(pPrevPointLink->pt, (*ppFinalPoint)->pt);
		pointsChainLength += distance;
		++nInternalPoints;
	}
	if(nInternalPoints)
		pInternalPointLink = (*ppInitialPoint)->next;
	
	// generate display objects from ppInitialPoint through ppFinalPoint
	if(allDots) 
	{  
		// pattern has dots only, put begin, end dots
		direction[0] = (*ppFinalPoint)->pt[0] - (*ppInitialPoint)->pt[0];
		direction[1] = (*ppFinalPoint)->pt[1] - (*ppInitialPoint)->pt[1];
		direction[2] = (*ppFinalPoint)->pt[2] - (*ppInitialPoint)->pt[2];
		distance = GR_LEN(direction);
		pointsChainLength += distance;
		GR_UNITIZE(direction, distance);
		
#ifdef _DOCLASSES_
		CDoBase::get().setPointsData(*ppInitialPoint, *ppInitialPoint, NULL, 0);
		CDoBase::get().setFlags(0, 0);
		CDoBase::get().setDotDir(direction);
		result = CDoBase::get().getDisplayObjects(pDO1, pDO2); 
#else
		result = gr_gensolid(pSourceEntity, *ppInitialPoint, *ppInitialPoint, NULL, 0, 0, 0, direction, &pDO1, &pDO2, pView);
#endif
		if(result)
			return result;
		if (pDO1 && pDO2) 
		{
			if(*ppDOListBegin == NULL) 
				*ppDOListBegin = pDO1; 
			else 
				(*ppDOListEnd)->next = pDO1;
			*ppDOListEnd = pDO2;
			pDO1 = NULL;
			pDO2 = NULL;
		}
#ifdef _DOCLASSES_
		CDoBase::get().setPointsData(*ppFinalPoint, *ppFinalPoint, NULL, 0);
		CDoBase::get().setFlags(0, 0);
		CDoBase::get().setDotDir(direction);
		result = CDoBase::get().getDisplayObjects(pDO1, pDO2); 
#else
		result = gr_gensolid(pSourceEntity, *ppFinalPoint, *ppFinalPoint, NULL, 0, 0, 0, direction, &pDO1, &pDO2, pView);
#endif
		if(result)
			return result;
	} 
	else 
	{
#ifdef _DOCLASSES_
		CDoBase::get().setPointsData(*ppInitialPoint, *ppFinalPoint, pInternalPointLink, nInternalPoints);
		CDoBase::get().setFlags(0, 0);
		CDoBase::get().setDotDir(NULL);
		result = CDoBase::get().getDisplayObjects(pDO1, pDO2); 
#else
		result = gr_gensolid(pSourceEntity, *ppInitialPoint, *ppFinalPoint, 
			pInternalPointLink, nInternalPoints, 0, 0, NULL, &pDO1, &pDO2, pView); 
#endif
		if(result)
			return result;
	}
	if (pDO1 && pDO2) 
	{
		if(*ppDOListBegin == NULL) 
			*ppDOListBegin = pDO1; 
		else 
			(*ppDOListEnd)->next = pDO1;
		*ppDOListEnd = pDO2;
		pDO1 = NULL;
		pDO2 = NULL;
	}
	*ppInitialPoint = pPrevPointLink;
	return result;
}

short gr_doPattern(
	gr_displayobject **ppDOListBegin, 
	gr_displayobject **ppDOListEnd,
	gr_view *pView, 
	db_drawing* pDrawing, 
	CDC* pDC,
	db_ltypetabrec* pLinetype,
	short elementIndex,
	gr_pclink& realInitialPoint,
	gr_pclink& realFinalPoint,
    long    finalPointIndex,
	sds_real scaleFactor,
	int useInitialPoint )
{
	short result = 0;
	gr_displayobject* pLBegin = NULL;
	gr_displayobject* pLEnd = NULL;
	db_text textEntity;
	CTextFragmentStyle textInfo;
	sds_real textStyleParam;
	sds_real textInsertPointOffset[2];
	gr_pclink* pPCFirstLink;
	gr_pclink* pPCLastLink;
	long nPoints;
	gr_pclink* pPCLastLinkUsed;
	long nPointsUsed;

	if(pLinetype->element(elementIndex).type > 0 && pLinetype->element(elementIndex).stylehip != NULL)
	{
		// element with text or shape
		char shapeCode[2];
		if(pLinetype->element(elementIndex).type & 0x04)
		{
			shapeCode[0] = pLinetype->element(elementIndex).shapecd;
			shapeCode[1] = 0;
			textInfo.val = shapeCode;
		}
		else if(pLinetype->element(elementIndex).type & 0x02)
		{
			textEntity.set_1(pLinetype->element(elementIndex).text);
			textInfo.val = pLinetype->element(elementIndex).text;
		}
		else
			assert(false);
		textEntity.set_7(pLinetype->element(elementIndex).stylehip);
		textInfo.m_pFont = pLinetype->element(elementIndex).stylehip->ret_font();
		textInfo.m_pBigfont = pLinetype->element(elementIndex).stylehip->ret_bigfont();
		pLinetype->element(elementIndex).stylehip->get_40(&textStyleParam);
		textInfo.m_height = textStyleParam ? textStyleParam : 1.0;
		pLinetype->element(elementIndex).stylehip->get_41(&textStyleParam);
		textInfo.m_widthFactor = textStyleParam ? textStyleParam : 1.0;
		textInfo.m_height  *= (pLinetype->element(elementIndex).scale * scaleFactor);

		textInfo.m_rotation = atan2(realFinalPoint.pt[1] - realInitialPoint.pt[1], realFinalPoint.pt[0] - realInitialPoint.pt[0]);
		textStyleParam = GR_DIST(realFinalPoint.pt, realInitialPoint.pt);
		if( icadRealEqual(textStyleParam, 0.0 ) && finalPointIndex < pView->npc - 1 )
        {
		    textInfo.m_rotation = atan2(realFinalPoint.next->pt[1] - realInitialPoint.pt[1], realFinalPoint.next->pt[0] - realInitialPoint.pt[0]);
		}
		textInsertPointOffset[0] = cos(textInfo.m_rotation) * (pLinetype->element(elementIndex).offset[0]) - 
			sin(textInfo.m_rotation) * pLinetype->element(elementIndex).offset[1];
		textInsertPointOffset[1] = sin(textInfo.m_rotation) * (pLinetype->element(elementIndex).offset[0]) + 
			cos(textInfo.m_rotation) * pLinetype->element(elementIndex).offset[1];
		textInfo.m_rotation += pLinetype->element(elementIndex).rot;

		if( useInitialPoint )
		{
			textInfo.m_insertionPoint[0] = realInitialPoint.pt[0] + scaleFactor * textInsertPointOffset[0];
			textInfo.m_insertionPoint[1] = realInitialPoint.pt[1] + scaleFactor * textInsertPointOffset[1];
			textInfo.m_insertionPoint[2] = realInitialPoint.pt[2];
		}
		else
		{
			textInfo.m_insertionPoint[0] = realFinalPoint.pt[0] + scaleFactor * textInsertPointOffset[0];
			textInfo.m_insertionPoint[1] = realFinalPoint.pt[1] + scaleFactor * textInsertPointOffset[1];
			textInfo.m_insertionPoint[2] = realFinalPoint.pt[2];
		}
		// save transformation to prevent points of text to be transformed one more time
		// TODO: build transformation matrix so what line and text will lie in one plane (in case 3D line)
		CMatrix4 viewTransformation;
		if(!pView->ltsclwblk)
		{
			viewTransformation = pView->m_transformation;
			pView->m_transformation.makeIdentity();
		}
		// save points chain
		pPCFirstLink = pView->pc;
		pPCLastLink = pView->lapc;
		nPoints = pView->npc;
		pPCLastLinkUsed = pView->lupc;
		nPointsUsed = pView->upc;
		pView->pc = NULL;
		pView->lapc = NULL;
		pView->npc = 0;
		pView->lupc = NULL;
		pView->upc = 0;
		result = gr_textgen(&textEntity, &textInfo, 0, pDrawing, pView, pDC, &pLBegin, &pLEnd, NULL, NULL);
		if(pLBegin != NULL && pLEnd != NULL)
		{
			if(*ppDOListBegin == NULL)
				*ppDOListBegin = pLBegin;
			else
				(*ppDOListEnd)->next = pLBegin;
			*ppDOListEnd = pLEnd;
		}
		// restore transformation
		if(!pView->ltsclwblk)
			pView->m_transformation = viewTransformation;
		// restore points chain
		pView->freePointsChain();
		pView->pc = pPCFirstLink;
		pView->lapc = pPCLastLink;
		pView->npc = nPoints;
		pView->lupc = pPCLastLinkUsed;
		pView->upc = nPointsUsed;
	}
	return result;
}

short gr_doPatternElement(db_entity* pSourceEntity, 
						  gr_displayobject **ppDOListBegin, gr_displayobject **ppDOListEnd,
						  gr_view *pView, db_drawing* pDrawing, CDC* pDC,
						  db_ltypetabrec* pLinetype, short elementIndex,
						  gr_pclink*& pInitialPoint, gr_pclink*& pFinalPoint, 
						  long& finalPointIndex, gr_pclink& realInitialPoint,
						  sds_real& pointsChainLength, sds_real& mainDoneDistance, 
						  sds_real size, sds_real scaleFactor)
{
	short result = 0;
	long nInternalPoints = 0;
	gr_pclink* pInternalPointLink = NULL;
	gr_pclink* pPrevPointLink = pInitialPoint;
	gr_pclink realFinalPoint;
	sds_point direction;
	sds_real distance;
	sds_real diversity;
	sds_real angleTan = 0.0;
	sds_real absdash;

	direction[0] = pFinalPoint->pt[0] - pInitialPoint->pt[0];
	direction[1] = pFinalPoint->pt[1] - pInitialPoint->pt[1];
	direction[2] = pFinalPoint->pt[2] - pInitialPoint->pt[2];
	distance = GR_LEN(direction);
	if(!icadRealEqual(distance,0.0))
		GR_UNITIZE(direction, distance);


	if ((absdash = size) < 0.0) 
		absdash = -absdash;
	if (icadRealEqual(absdash,0.0)) 
	{	
		// dot
#ifdef _DOCLASSES_
		CDoBase::get().setPointsData(&realInitialPoint, &realInitialPoint, NULL, 0);
		CDoBase::get().setFlags(0, 0);
		CDoBase::get().setDotDir(direction);
		result = CDoBase::get().getDisplayObjects(*ppDOListBegin, *ppDOListEnd); 
#else
		result = gr_gensolid(pSourceEntity, &realInitialPoint, &realInitialPoint, NULL, 0, 0, 0,direction, 
			ppDOListBegin, ppDOListEnd, pView);
#endif

		// perform rotation angle for Dot 
		gr_doPattern( ppDOListBegin, ppDOListEnd, pView, pDrawing, pDC,
			pLinetype, elementIndex, realInitialPoint, *pFinalPoint, finalPointIndex, scaleFactor, 1);
	} 
	else
	{
		// dash or space, maybe with text or shape
		while(mainDoneDistance + absdash > pointsChainLength) 
		{
            // Fuzzy compare 
            if(icadRealEqual(mainDoneDistance + absdash, pointsChainLength ))
                break;
			if(finalPointIndex >= pView->upc-1) 
				return result;

			pPrevPointLink = pFinalPoint;
			pFinalPoint = pPrevPointLink->next;
			++finalPointIndex;

			direction[0] = pFinalPoint->pt[0] - pPrevPointLink->pt[0];
			direction[1] = pFinalPoint->pt[1] - pPrevPointLink->pt[1];
			direction[2] = pFinalPoint->pt[2] - pPrevPointLink->pt[2];
			distance = GR_LEN(direction);
			pointsChainLength += distance;
			if(!icadRealEqual(distance,0.0)) // ???
			{
				GR_UNITIZE(direction, distance);
				++nInternalPoints;
			}
		}
		if(nInternalPoints) 
			pInternalPointLink = pInitialPoint->next;
		diversity = pointsChainLength - mainDoneDistance - absdash;
		// get realFinalPoint.pt by stepping back from pFinalPoint->pt toward pInitialPoint->pt: */
		realFinalPoint.pt[0] = pFinalPoint->pt[0] - diversity * direction[0];
		realFinalPoint.pt[1] = pFinalPoint->pt[1] - diversity * direction[1];
		realFinalPoint.pt[2] = pFinalPoint->pt[2] - diversity * direction[2];
		
		// dash or space
		/*
		**	TAPER THE WIDTHS.  TRICKY!
		**	(See full note earlier.)
		*/
		realFinalPoint.swid = diversity / distance * (pInitialPoint->swid - pPrevPointLink->ewid) + pPrevPointLink->ewid;
		realFinalPoint.ewid = pPrevPointLink->ewid;
		realFinalPoint.startang = realFinalPoint.endang = IC_ANGLE_DONTCARE;
		if(pInternalPointLink == NULL) 
			realInitialPoint.ewid = realFinalPoint.swid;
		else				
			pInitialPoint->ewid = realFinalPoint.swid;
		if(pInitialPoint->startang == IC_ANGLE_DONTCARE || pInitialPoint->endang == IC_ANGLE_DONTCARE) 
		{
			realFinalPoint.startang = pInitialPoint->startang;
			realFinalPoint.endang = pInitialPoint->endang;
		}
		else 
		{
			realFinalPoint.startang = diversity / distance * (pInitialPoint->startang - pInitialPoint->endang) + pInitialPoint->endang;
			realFinalPoint.endang = pInitialPoint->endang;
			if(pInternalPointLink == NULL) 
				realInitialPoint.endang = realFinalPoint.startang;
			else				
				pInitialPoint->endang = realFinalPoint.startang;
		}
		if (size > 0.0) 
		{  
			// dash
#ifdef _DOCLASSES_
			CDoBase::get().setPointsData(&realInitialPoint, &realInitialPoint, pInternalPointLink, nInternalPoints);
			CDoBase::get().setFlags(0, 0);
			CDoBase::get().setDotDir(direction);
			result = CDoBase::get().getDisplayObjects(*ppDOListBegin, *ppDOListEnd); 
#else
			result = gr_gensolid(pSourceEntity, &realInitialPoint, &realFinalPoint, 
				pInternalPointLink, nInternalPoints, 0, 0, NULL, ppDOListBegin, ppDOListEnd, pView);
#endif
			if(result)
				return result;
		}
		gr_doPattern( ppDOListBegin, ppDOListEnd, pView, pDrawing, pDC,
			pLinetype, elementIndex, *pPrevPointLink, realFinalPoint, finalPointIndex, scaleFactor, 0);
		memcpy(&realInitialPoint, &realFinalPoint, sizeof(realInitialPoint));
		mainDoneDistance += absdash;
		pInitialPoint = pPrevPointLink;
	}
	return result;
}
