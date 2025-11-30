/* 
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * Functions that convert pc chains to display objects.
 */ 

#include "gr.h"/*DNT*/
#include "pc2dispobj.h"/*DNT*/
#include "gr_view.h"/*DNT*/

#ifdef _TRANSFORMATION_AS_ARRAY_
#define _THICKNESS_ viewp->xfp[4]
#else
#define _THICKNESS_ viewp->m_thickness
#endif

/*
**  The following is a default view, to be used only if a real one
**  is unavailable (to avoid NULL ptr usage).
**
**  The screen pixel resolution is for 320x200.
**
**  gr_clean() frees the pc list (if it ever gets allocated).
*/
gr_view gr_defview;


GR_API void gr_clean(void)
{
/*
	This was freeing the allocated stuff in gr_defview.
	Now that we have a destructor, I don't think we need this.

	We'll keep this function, though, in case there's every anything
	else in the gr stuff that we need to clean up.
*/
}


void UpdateClipUCSExtents(gr_view* pGrView, const C3Point pt)
{
	if (pGrView->m_ClipUcsExtMode != -1) {
		if (pGrView->m_ClipUcsExtMode == 0) {
			pGrView->m_ClipUcsExt[0][0] = pGrView->m_ClipUcsExt[1][0] = pt.X();
			pGrView->m_ClipUcsExt[0][1] = pGrView->m_ClipUcsExt[1][1] = pt.Y();
			pGrView->m_ClipUcsExt[0][2] = pGrView->m_ClipUcsExt[1][2] = pt.Z();
			pGrView->m_ClipUcsExtMode = 1;
		}
		else {
			if (pGrView->m_ClipUcsExt[0][0] > pt.X()) 
				pGrView->m_ClipUcsExt[0][0] = pt.X();
			else if (pGrView->m_ClipUcsExt[1][0] < pt.X()) 
				pGrView->m_ClipUcsExt[1][0] = pt.X();
                
			if (pGrView->m_ClipUcsExt[0][1] > pt.Y()) 
				pGrView->m_ClipUcsExt[0][1] = pt.Y();
			else if (pGrView->m_ClipUcsExt[1][1] < pt.Y()) 
				pGrView->m_ClipUcsExt[1][1] = pt.Y();
                
			if (pGrView->m_ClipUcsExt[0][2] > pt.Z()) 
				pGrView->m_ClipUcsExt[0][2] = pt.Z();
			else if (pGrView->m_ClipUcsExt[1][2] < pt.Z()) 
				pGrView->m_ClipUcsExt[1][2] = pt.Z();
		}
	}
}


/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	An extension of gr_gensolid. Clip if necessary given polyline/polygon
 *			set by points array and create appropriate disp objs.
 * Returns:	gr_gensolid return codes.
 *
 * History
 * EBATECH(CNBR) 24-03-2003 Bugzilla#78471 Support Lineweight DIMLWD(371)
 ********************************************************************************/
short
ClipAndCreateDO
(
 db_entity*				pSourceEntity,	// =>
 CPolyLine&				points,			// =>
 short					closed,			// => 0 or 1
 short					fillable,		// => 0 or 4 (DISP_OBJ_PTS_FILLABLE)
 short					dimension,		// => 2 or 3
 gr_view*				pGrView,		// =>
 gr_displayobject**		ppBegNewDO,		// <=>
 gr_displayobject**		ppEndNewDO		// <=>
)
{
#define GR_LINKDO	if(!*ppBegNewDO)					\
						*ppBegNewDO = pNewDO;			\
					else								\
						(*ppEndNewDO)->next = pNewDO;	\
					*ppEndNewDO = pNewDO;				\
					pNewDO = NULL;


	ASSERT(pSourceEntity && pGrView && ppBegNewDO && ppEndNewDO);

	int					size = points.Size(),
						vertNum;
	gr_displayobject*	pNewDO;

	if(pGrView->m_Clipper.IsNotEmpty())
	{
		// construct the transformation matrix to transform lines from UCS to WCS. The Bug #38466 fix: Stephen Hou, 8/14/2003
		//
		CAffine transf(new C3Point(pGrView->ucsorg[0], pGrView->ucsorg[1], pGrView->ucsorg[2]),
					   new C3Point(pGrView->ucsaxis[0][0], pGrView->ucsaxis[0][1], pGrView->ucsaxis[0][2]),
					   new C3Point(pGrView->ucsaxis[1][0], pGrView->ucsaxis[1][1], pGrView->ucsaxis[1][2]),
					   new C3Point(pGrView->ucsaxis[2][0], pGrView->ucsaxis[2][1], pGrView->ucsaxis[2][2]));

		// the transformation matrix to transform lines from WCS to UCS.
		//
		CAffine invTransf;
		invTransf.SetInverse(transf);

		if(size > 2 || fillable)		// clip as polyline if we have more than 2 points
		{
			PPolyLine		pPoints;
			PPolylinesList	PPolylines;
			RC				geoRc;

			if(!(pPoints = new CPolyLine(points, geoRc)) || geoRc != SUCCESS)
				return -3;

			if(closed && points[0] != points.Last() && pPoints->Add(points[0]) != SUCCESS)
				return -3;

			// transform polyline to UCS.
			//
			pPoints->Transform(transf);    // The Bug #38466 fix
			PPolylines.push_back(pPoints);

			// clip
			bool	bClipRetCode;
			if(fillable)
				bClipRetCode = pGrView->m_Clipper.ClipPolygons(PPolylines);
			else
				bClipRetCode = pGrView->m_Clipper.ClipPolylines(PPolylines);
			
			if(!bClipRetCode)
			{
				FreePointersList(PPolylines);
				return -3;
			}

			// create disp objs
			for(PPolylinesList::iterator pPPolyline = PPolylines.begin(); pPPolyline != PPolylines.end(); ++pPPolyline)
			{
				if(!(pNewDO = gr_displayobject::newDisplayObject(pSourceEntity)))
				{
					FreePointersList(PPolylines);
					return -3;
				}

				pNewDO->next = NULL;
				pNewDO->type = (dimension > 2) + fillable;
				if((**pPPolyline)[0] == (*pPPolyline)->Last())
					pNewDO->type += DISP_OBJ_PTS_CLOSED;
				pNewDO->color = pGrView->color;
				pNewDO->lweight = pGrView->lweight;
				size = pNewDO->npts = (*pPPolyline)->Size();
				if(!(pNewDO->chain = new sds_real[ size * dimension + 1 ]))	/*D.G.*/// + 1 sds_real because
				{						// we need 3 sds_reals for every vertex we considering, so allocate one for dimension == 2.
					IC_FREE(pNewDO);
					FreePointersList(PPolylines);
					return -3;
				}
				C3Point point;
				for(vertNum = 0; vertNum < size; ++vertNum)
				{
					C3Point&	geoPoint = (*(*pPPolyline))[vertNum];
					invTransf.ApplyToPoint(geoPoint, point);  // Transform from WCS to UCS: Bug #38466 fix

					// Update the values of m_ClipUcsExt[2] in pGrView - Bug #38331 fix
					UpdateClipUCSExtents(pGrView, point);

					pNewDO->chain[vertNum * dimension] = point.X();
					pNewDO->chain[vertNum * dimension + 1] = point.Y();
					pNewDO->chain[vertNum * dimension + 2] = point.Z();
					if(pGrView->projmode && gr_ucs2rp(pNewDO->chain + vertNum * dimension, pNewDO->chain + vertNum * dimension, pGrView))
					{
						gr_displayobject::freeDisplayObject(pNewDO);
						FreePointersList(PPolylines);
						return 0;
					}
				}

				GR_LINKDO
			}	// for( thru obtained polylines

			FreePointersList(PPolylines);

		}	// size > 2 || fillable
		else
		{	// size <= 2 && !fillable
			LinesList	Lines;

			// create lines list
			C3Point startPoint, endPoint;
			for(vertNum = 1; vertNum < size; ++vertNum) {
				// Transform end points from UCS to WCS - The Bug #38466 fix
				transf.ApplyToPoint(points[vertNum-1], startPoint); 
				transf.ApplyToPoint(points[vertNum], endPoint);
				Lines.push_back(CLine(startPoint, endPoint));
			}

			if(closed)
			{
				// Transform end points from UCS to WCS - The Bug #38466 fix
				transf.ApplyToPoint(points[size-1], startPoint);
				transf.ApplyToPoint(points[0], endPoint);
				Lines.push_back(CLine(startPoint, endPoint));
				++size;
			}
			if(!pGrView->m_Clipper.ClipLines(Lines))
				return -3;

			// create disp objs
			C3Point		geoPoint1, geoPoint2;
			for(LinesList::iterator pLine = Lines.begin(); pLine != Lines.end(); ++pLine)
			{
				if(!(pNewDO = gr_displayobject::newDisplayObject(pSourceEntity)))
					return -3;

				pNewDO->next = NULL;
				pNewDO->type = (dimension > 2);
				pNewDO->color = pGrView->color;
				pNewDO->lweight = pGrView->lweight;
				pNewDO->npts = 2;
				if(!(pNewDO->chain = new sds_real[ 2 * dimension + 1]))	/*D.G.*/// + 1 sds_real because
				{						// we need 3 sds_reals for every vertex we considering, so allocate one for dimension == 2.
					IC_FREE(pNewDO);
					return -3;
				}

				if(pLine->GetEnds(startPoint, endPoint) != SUCCESS)
				{
					gr_displayobject::freeDisplayObject(pNewDO);
					return -3;
				}

				// Transform result from WCS to UCS - The Bug #38466 fix
				invTransf.ApplyToPoint(startPoint, geoPoint1);
				invTransf.ApplyToPoint(endPoint, geoPoint2);

				// Update the values of m_ClipUcsExt[2] in pGrView - Bug #38331 fix
				UpdateClipUCSExtents(pGrView, geoPoint1);
				UpdateClipUCSExtents(pGrView, geoPoint2);

				pNewDO->chain[0] = geoPoint1.X();
				pNewDO->chain[1] = geoPoint1.Y();
				pNewDO->chain[2] = geoPoint1.Z();
				if(pGrView->projmode && gr_ucs2rp(pNewDO->chain, pNewDO->chain, pGrView))
				{
					gr_displayobject::freeDisplayObject(pNewDO);
					return 0;
				}

				pNewDO->chain[dimension] = geoPoint2.X();
				pNewDO->chain[dimension + 1] = geoPoint2.Y();
				pNewDO->chain[dimension + 2] = geoPoint2.Z();
				if(pGrView->projmode && gr_ucs2rp(pNewDO->chain + dimension, pNewDO->chain + dimension, pGrView))
				{
					gr_displayobject::freeDisplayObject(pNewDO);
					return 0;
				}

				GR_LINKDO

			}	// for( thru obtained lines

		}	// size <= 2 && !fillable

	}	// clipping
	else
	{	// no clipping
		
		if(!(pNewDO = gr_displayobject::newDisplayObject( pSourceEntity )))
			return -3;

		pNewDO->next = NULL;
		pNewDO->type = (dimension > 2) + 2 * closed + fillable;
		pNewDO->color = pGrView->color;
		pNewDO->lweight = pGrView->lweight;
		pNewDO->npts = size;

		if(!(pNewDO->chain = new sds_real[(size * dimension + 1) ]))	/*D.G.*/// + 1 sds_real because
		{						// we need 3 sds_reals for every vertex we considering, so allocate one for dimension == 2.
			IC_FREE(pNewDO);
			return -3;
		}

		for(vertNum = 0; vertNum < size; ++vertNum)
		{
			pNewDO->chain[vertNum * dimension] = points[vertNum].X();
			pNewDO->chain[vertNum * dimension + 1] = points[vertNum].Y();
			pNewDO->chain[vertNum * dimension + 2] = points[vertNum].Z();

			if(pGrView->projmode && gr_ucs2rp(pNewDO->chain + vertNum * dimension, pNewDO->chain + vertNum * dimension, pGrView))
			{
				gr_displayobject::freeDisplayObject(pNewDO);
				return 0;
			}
		}

		GR_LINKDO

	}	// not clipping

	return 0;

#undef GR_LINKDO

}


short
gr_gensolid
(
 db_entity*		pSourceEntity,
 gr_pclink*		begpclp,
 gr_pclink*		endpclp,
 gr_pclink*		ilink,
 long			nintern,
 short			closed,
 short			capped,
 sds_point		dotdir,
 gr_displayobject**	begpp,
 gr_displayobject**	endpp,
 gr_view*		viewp
)
{
/*
**  This is an extension of gr_getdispobjs().
**
**  Converts a chain of points to a llist of display objects.  Handles
**  thickness for each viewp->projmode.
**
**  Note that the chain may be in the NCS or the UCS at this point,
**  depending on the value of our system variable LTSCLWBLK.  If the
**  points have not been converted to UCS, they are here (via
**  gr_ncs2ucs()), and then they are projected onto the real plane if
**  viewp->projmode!=0.
**
**  The chain of points to convert (the pc chain) is as follows:
**
**      begpclp
**      ilink
**      ilink->next... (for nintern links)
**      endpclp
**
**  begpclp and endpclp are always expected to be non-NULL;
**  ilink can be NULL and nintern can be 0 when there is only a dot
**  or one chord.
**
**  begpclp and endpclp may lie in between vertices in the viewp->pc
**  llist; gr_doltype() determines them when breaking up viewp->pc
**  for linetype.  FOR WHOLE CLOSED ENTITIES, THIS CHAIN IS ALWAYS
**  EXPLICITLY CLOSED (begpclp AND endpclp HAVE COINCIDENT POINTS).
**
**  "closed" flags that the chain is for a WHOLE entity and is
**  considered closed (begpclp->pt and endpclp->pt are coincident).
**
**  "capped" flags that the chain is for a WHOLE entity and is
**  to be capped in UCS (3D) mode.
**
**  Parameter dotdir is needed by gr_getwidepc() to generate widths
**  for dots in ltypes that have dots.  It's the pc chain direction
**  vector at the time the dot is encountered.  It's NULL in other
**  cases (dashes).  See gr_getwidepc().  All functions except gr_doltype()
**  should be calling this one with dotdir!=NULL.
**
**  Don't confuse a closed entity with a closed gr_displayobject llist.
**
**  *begpp is pointed to the first display object in the llist.
**  *endpp is pointed to the last  display object in the llist (for
**  speed in chaining).
**
**  Caller's responsibility to free, of course.
**
**  Returns: gr_getdispobjs() error codes (0 or -3 (no memory)).
**
** History
** EBATECH(CNBR) 24-03-2003 Bugzilla#78471 Support Lineweight DIMLWD(371)
*/

/*D.G.*/// Rewritten for using ClipAndCreateDO.

    char				fillable;
    short				wsidx, coordidx, rc;
	int					endidx, ndims, fi1, fi2, fi3, fint1;
    sds_point			ap1, ap2, ap3, ap4;
    gr_displayobject	*begdo, *enddo;
    gr_pclink			*thispclp, *nextpclp;
    gr_wideseglink*		wsap;
	
	static RC			geoRC;
	static CPolyLine	pointsArray2(2, geoRC), pointsArray4(4, geoRC), pointsArray10(10, geoRC), pointsArray;

    rc = 0;
	begdo = enddo = NULL;
	wsap = NULL;

    if(!viewp || icadRealEqual(viewp->viewsize,0.0))
		viewp = &gr_defview;

    /*
    **  The variable ndims should be used strictly for determining the
    **  type of display object (2D (RP) with an XYXY... chain,
    **  or 3D (UCS) with an XYZXYZ... chain) -- NOT THE CONSTRUCTION
    **  LOGIC (which should use viewp->projmode).
    */
    ndims = 2 + (!viewp->projmode || viewp->noucs2rp);

    endidx = nintern + 1;	// For loops, the 0-based index of the last point (for deciding when to use endpclp, etc.)

    // If we're not doing thickness and it's a fillable entity (SOLID, TRACE, 2D POLYLINE with width),
    // set fillable to the bit-2 mask (4) for the disp obj "type" member:
    fillable = 0;
    if(!viewp->dothick && viewp->ment)
	{
        fint1 = viewp->ment->ret_type();
        if( fint1 == DB_SOLID || fint1 == DB_TRACE ||
			(fint1 == DB_POLYLINE && viewp->dowid) || (fint1 == DB_LWPOLYLINE && viewp->dowid) )
			fillable = DISP_OBJ_PTS_FILLABLE;
    }
	if(!viewp->projmode && viewp->dothick && viewp->ment)
	{
		fint1 = viewp->ment->ret_type();
		if(fint1 == DB_SOLID || fint1 == DB_TRACE || 
			(fint1 == DB_POLYLINE && viewp->dowid) || 
			(fint1 == DB_LWPOLYLINE && viewp->dowid))
		{
			begdo = enddo = gr_displayobject::newDisplayObject(pSourceEntity);
			begdo->type = DISP_OBJ_PTS_3D | DISP_OBJ_SPECIAL;
			begdo->color = 0;
			begdo->lweight = 0;
			begdo->npts = 1;
			begdo->chain = new sds_real[4];
			memcpy(begdo->chain, viewp->m_thickness, 3 * sizeof(sds_real));
		}
	}

    if(viewp->dowid)
	{	// Has non-0 width somewhere in it. Get the UCS wide seg array.
        if(gr_getwidepc(begpclp, endpclp, ilink, nintern, closed, dotdir, &wsap, viewp) || !wsap)
		{
			rc = -3;
			goto out;
		}
    }


    if(viewp->dothick)
	{
        if(viewp->dowid)
		{	// Has non-0 width somewhere in it.
			if(viewp->projmode)
			{	// Planar (2D)

                /*
                **  Since we're not filling things that show thickness,
                **  try to build it efficiently.
                **
                **           6------5
                **          /|     /|
                **         / |    / |
                **        /  2---/--1       --.
                **       /  /el /  /er       /|
                **      7------4  /         /
                **      | /    | /         /
                **      |/     |/         /
                **      3------0      wsap[] chain
                **      sl     sr     direction
                **
                **
                **  sr, sl, er, and el are the members of each
                **  wsap[] element.
                **
                **  4-7 are 0-3 offset by the thickness.
                **
                **  If a particular segment has no width (01 coincident
                **  with 32), just make 01540.
                **
                **  Otherwise, for the first segment, build the whole solid
                **  (0123045674 15 26 37).  For the very last segment:
                **  draw 03740 if 03 doesn't coincide with the previous
                **  12; draw 12651 if 12 doesn't coincide with the FIRST
                **  segment's 03; draw 01 23 45 67 no matter what.
                **  For internal segments, if 03 coincides with 12 of
                **  the previous segment, just draw 0123 4567 15 26;
                **  otherwise, draw the full solid.
                */

                for(wsidx = 0; wsidx <= nintern; ++wsidx)
				{
                    if( wsap[wsidx].sr[0] == wsap[wsidx].sl[0] && wsap[wsidx].sr[1] == wsap[wsidx].sl[1] &&
                        wsap[wsidx].sr[2] == wsap[wsidx].sl[2] && wsap[wsidx].er[0] == wsap[wsidx].el[0] &&
                        wsap[wsidx].er[1] == wsap[wsidx].el[1] && wsap[wsidx].er[2] == wsap[wsidx].el[2] )
					{
                        // This seg has no width; make the 01540 disp obj:
						pointsArray4.Set(0, C3Point(wsap[wsidx].sr[0], wsap[wsidx].sr[1], wsap[wsidx].sr[2]));
						pointsArray4.Set(1, C3Point(wsap[wsidx].er[0], wsap[wsidx].er[1], wsap[wsidx].er[2]));

						pointsArray4.Set(2, C3Point(wsap[wsidx].er[0] + _THICKNESS_[0],
													wsap[wsidx].er[1] + _THICKNESS_[1],
													wsap[wsidx].er[2] + _THICKNESS_[2]));
						pointsArray4.Set(3, C3Point(wsap[wsidx].sr[0] + _THICKNESS_[0],
													wsap[wsidx].sr[1] + _THICKNESS_[1],
													wsap[wsidx].sr[2] + _THICKNESS_[2]));

						if(rc = ClipAndCreateDO(pSourceEntity, pointsArray4, 1, 0, ndims, viewp, &begdo, &enddo))
							goto out;

                        continue;
                    }

                    // This seg has width if execution gets to here.

                    if(wsidx < 1)
					{	// First seg

					wholewidesolid:

						// Make the 0123045674 disp obj:

						// The base level (01230):
						pointsArray10.Set(0, C3Point(wsap[wsidx].sr[0], wsap[wsidx].sr[1], wsap[wsidx].sr[2]));
						pointsArray10.Set(1, C3Point(wsap[wsidx].er[0], wsap[wsidx].er[1], wsap[wsidx].er[2]));
						pointsArray10.Set(2, C3Point(wsap[wsidx].el[0], wsap[wsidx].el[1], wsap[wsidx].el[2]));
						pointsArray10.Set(3, C3Point(wsap[wsidx].sl[0], wsap[wsidx].sl[1], wsap[wsidx].sl[2]));
						pointsArray10.Set(4, C3Point(wsap[wsidx].sr[0], wsap[wsidx].sr[1], wsap[wsidx].sr[2]));

						// The thickness level (45674):
						pointsArray10.Set(5, C3Point(wsap[wsidx].sr[0] + _THICKNESS_[0],
													 wsap[wsidx].sr[1] + _THICKNESS_[1],
													 wsap[wsidx].sr[2] + _THICKNESS_[2]));
						pointsArray10.Set(6, C3Point(wsap[wsidx].er[0] + _THICKNESS_[0],
													 wsap[wsidx].er[1] + _THICKNESS_[1],
													 wsap[wsidx].er[2] + _THICKNESS_[2]));
						pointsArray10.Set(7, C3Point(wsap[wsidx].el[0] + _THICKNESS_[0],
													 wsap[wsidx].el[1] + _THICKNESS_[1],
													 wsap[wsidx].el[2] + _THICKNESS_[2]));
						pointsArray10.Set(8, C3Point(wsap[wsidx].sl[0] + _THICKNESS_[0],
													 wsap[wsidx].sl[1] + _THICKNESS_[1],
													 wsap[wsidx].sl[2] + _THICKNESS_[2]));
						pointsArray10.Set(9, C3Point(wsap[wsidx].sr[0] + _THICKNESS_[0],
													 wsap[wsidx].sr[1] + _THICKNESS_[1],
													 wsap[wsidx].sr[2] + _THICKNESS_[2]));

						if(rc = ClipAndCreateDO(pSourceEntity, pointsArray10, 0, 0, ndims, viewp, &begdo, &enddo))
							goto out;

						// Make the 15, 26, and 37 disp objs:

						for(fi1 = 1; fi1 < 4; ++fi1)
						{
							pointsArray2.Set(0, pointsArray10[fi1]);
							pointsArray2.Set(1, pointsArray10[fi1 + 5]);

							if(rc = ClipAndCreateDO(pSourceEntity, pointsArray2, 0, 0, ndims, viewp, &begdo, &enddo))
								goto out;
						}
                    }
					else
						if(wsidx >= nintern)
						{	// Last seg

							// Check junction with prev seg to see if we need 03740:

							if( wsap[wsidx].sr[0] != wsap[wsidx-1].er[0] ||	wsap[wsidx].sr[1] != wsap[wsidx-1].er[1] ||
								wsap[wsidx].sr[2] != wsap[wsidx-1].er[2] ||	wsap[wsidx].sl[0] != wsap[wsidx-1].el[0] ||
								wsap[wsidx].sl[1] != wsap[wsidx-1].el[1] ||	wsap[wsidx].sl[2] != wsap[wsidx-1].el[2] )
							{
								pointsArray4.Set(0, C3Point(wsap[wsidx].sr[0], wsap[wsidx].sr[1], wsap[wsidx].sr[2]));
								pointsArray4.Set(1, C3Point(wsap[wsidx].sl[0], wsap[wsidx].sl[1], wsap[wsidx].sl[2]));

								pointsArray4.Set(2, C3Point(wsap[wsidx].sl[0] + _THICKNESS_[0],
															wsap[wsidx].sl[1] + _THICKNESS_[1],
															wsap[wsidx].sl[2] + _THICKNESS_[2]));
								pointsArray4.Set(3, C3Point(wsap[wsidx].sr[0] + _THICKNESS_[0],
															wsap[wsidx].sr[1] + _THICKNESS_[1],
															wsap[wsidx].sr[2] + _THICKNESS_[2]));

								if(rc = ClipAndCreateDO(pSourceEntity, pointsArray4, 1, 0, ndims, viewp, &begdo, &enddo))
									goto out;
							}

							// Check junction with FIRST seg to see if we need 12561:

							if( wsap[wsidx].er[0] != wsap[0].sr[0] || wsap[wsidx].er[1] != wsap[0].sr[1] ||
								wsap[wsidx].er[2] != wsap[0].sr[2] || wsap[wsidx].el[0] != wsap[0].sl[0] ||
								wsap[wsidx].el[1] != wsap[0].sl[1] || wsap[wsidx].el[2] != wsap[0].sl[2] )
							{
								pointsArray4.Set(0, C3Point(wsap[wsidx].er[0], wsap[wsidx].er[1], wsap[wsidx].er[2]));
								pointsArray4.Set(1, C3Point(wsap[wsidx].el[0], wsap[wsidx].el[1], wsap[wsidx].el[2]));

								pointsArray4.Set(2, C3Point(wsap[wsidx].el[0] + _THICKNESS_[0],
															wsap[wsidx].el[1] + _THICKNESS_[1],
															wsap[wsidx].el[2] + _THICKNESS_[2]));
								pointsArray4.Set(3, C3Point(wsap[wsidx].er[0] + _THICKNESS_[0],
															wsap[wsidx].er[1] + _THICKNESS_[1],
															wsap[wsidx].er[2] + _THICKNESS_[2]));

								if(rc = ClipAndCreateDO(pSourceEntity, pointsArray4, 1, 0, ndims, viewp, &begdo, &enddo))
									goto out;
							}

							// Add the 01, 23, 45, and 67 disp objs:

							for(fi1 = 0; fi1 < 4; ++fi1)
							{
								// 0 : 01; 1 : 23; 2 : 45; 3 : 67
								if(fi1 % 2)
								{	// 23 and 67 -- left pts
									if(fi1 > 1)
									{	// 67
										pointsArray2.Set(0, C3Point(wsap[wsidx].sl[0] + _THICKNESS_[0],
																   wsap[wsidx].sl[1] + _THICKNESS_[1],
																   wsap[wsidx].sl[2] + _THICKNESS_[2]));
										pointsArray2.Set(1, C3Point(wsap[wsidx].el[0] + _THICKNESS_[0],
																   wsap[wsidx].el[1] + _THICKNESS_[1],
																   wsap[wsidx].el[2] + _THICKNESS_[2]));
									}
									else
									{	// 23
										pointsArray2.Set(0, C3Point(wsap[wsidx].sl[0], wsap[wsidx].sl[1], wsap[wsidx].sl[2]));
										pointsArray2.Set(1, C3Point(wsap[wsidx].el[0], wsap[wsidx].el[1], wsap[wsidx].el[2]));
									}
								}
								else
								{	// 01 and 45 -- right pts
									if(fi1 > 1)
									{	// 45
										pointsArray2.Set(0, C3Point(wsap[wsidx].sr[0] + _THICKNESS_[0],
																   wsap[wsidx].sr[1] + _THICKNESS_[1],
																   wsap[wsidx].sr[2] + _THICKNESS_[2]));
										pointsArray2.Set(1, C3Point(wsap[wsidx].er[0] + _THICKNESS_[0],
																   wsap[wsidx].er[1] + _THICKNESS_[1],
																   wsap[wsidx].er[2] + _THICKNESS_[2]));
									}
									else
									{	// 01
										pointsArray2.Set(0, C3Point(wsap[wsidx].sr[0], wsap[wsidx].sr[1], wsap[wsidx].sr[2]));
										pointsArray2.Set(1, C3Point(wsap[wsidx].er[0], wsap[wsidx].er[1], wsap[wsidx].er[2]));
									}
								}

								if(rc = ClipAndCreateDO(pSourceEntity, pointsArray2, 0, 0, ndims, viewp, &begdo, &enddo))
									goto out;
							}
						}
						else
						{	// Internal seg

							// Check junction with prev seg to see if we need to do the whole thing:
							if( wsap[wsidx].sr[0] != wsap[wsidx-1].er[0] || wsap[wsidx].sr[1] != wsap[wsidx-1].er[1] ||
								wsap[wsidx].sr[2] != wsap[wsidx-1].er[2] || wsap[wsidx].sl[0] != wsap[wsidx-1].el[0] ||
								wsap[wsidx].sl[1] != wsap[wsidx-1].el[1] || wsap[wsidx].sl[2] != wsap[wsidx-1].el[2] )
									goto wholewidesolid;

							// Make the 0123 and 4567 disp objs:

							pointsArray4.Set(0, C3Point(wsap[wsidx].sr[0], wsap[wsidx].sr[1], wsap[wsidx].sr[2]));
							pointsArray4.Set(1, C3Point(wsap[wsidx].er[0], wsap[wsidx].er[1], wsap[wsidx].er[2]));
							pointsArray4.Set(2, C3Point(wsap[wsidx].el[0], wsap[wsidx].el[1], wsap[wsidx].el[2]));
							pointsArray4.Set(3, C3Point(wsap[wsidx].sl[0], wsap[wsidx].sl[1], wsap[wsidx].sl[2]));

							if(rc = ClipAndCreateDO(pSourceEntity, pointsArray4, 0, 0, ndims, viewp, &begdo, &enddo))
								goto out;

							pointsArray4.Set(0, C3Point(wsap[wsidx].sr[0] + _THICKNESS_[0],
														wsap[wsidx].sr[1] + _THICKNESS_[1],
														wsap[wsidx].sr[2] + _THICKNESS_[2]));
							pointsArray4.Set(1, C3Point(wsap[wsidx].er[0] + _THICKNESS_[0],
														wsap[wsidx].er[1] + _THICKNESS_[1],
														wsap[wsidx].er[2] + _THICKNESS_[2]));
							pointsArray4.Set(2, C3Point(wsap[wsidx].el[0] + _THICKNESS_[0],
														wsap[wsidx].el[1] + _THICKNESS_[1],
														wsap[wsidx].el[2] + _THICKNESS_[2]));
							pointsArray4.Set(3, C3Point(wsap[wsidx].sl[0] + _THICKNESS_[0],
														wsap[wsidx].sl[1] + _THICKNESS_[1],
														wsap[wsidx].sl[2] + _THICKNESS_[2]));

							if(rc = ClipAndCreateDO(pSourceEntity, pointsArray4, 0, 0, ndims, viewp, &begdo, &enddo))
								goto out;

							// Make the 15 and 26 disp objs:

							pointsArray2.Set(0, C3Point(wsap[wsidx].er[0], wsap[wsidx].er[1], wsap[wsidx].er[2]));
							pointsArray2.Set(1, C3Point(wsap[wsidx].er[0] + _THICKNESS_[0],
														wsap[wsidx].er[1] + _THICKNESS_[1],
														wsap[wsidx].er[2] + _THICKNESS_[2]));

							if(rc = ClipAndCreateDO(pSourceEntity, pointsArray2, 0, 0, ndims, viewp, &begdo, &enddo))
								goto out;

							pointsArray2.Set(0, C3Point(wsap[wsidx].el[0], wsap[wsidx].el[1], wsap[wsidx].el[2]));
							pointsArray2.Set(1, C3Point(wsap[wsidx].el[0] + _THICKNESS_[0],
														wsap[wsidx].el[1] + _THICKNESS_[1],
														wsap[wsidx].el[2] + _THICKNESS_[2]));

							if(rc = ClipAndCreateDO(pSourceEntity, pointsArray2, 0, 0, ndims, viewp, &begdo, &enddo))
								goto out;
						}  // End else internal seg
                }  // End for each wsap[] element

            }	// Planar (2D)
			else
			{	// UCS (3D)

                /*
                **           6------5
                **          /|     /|
                **         / |    / |
                **        /  2---/--1       --.
                **       /  /el /  /er       /|
                **      7------4  /         /
                **      | /    | /         /
                **      |/     |/         /
                **      3------0      wsap[] chain
                **      sl     sr     direction
                **
                **  Similar strategy to the above -- except we're
                **  building faces this time.
                **
                **  In the planar case, we tried to make each display
                **  objects carry as many lines as possible; it would
                **  have been inefficient to make 1 disp obj per line.
                **  But here, we have to make 1 face per disp object,
                **  so we can flag which faces to build for each case
                **  (using ff[]), and then build all of the flagged
                **  faces at the end.
                **
                **  Here are the ff[] face associations:
                **
                **      ff[0] : bottom (01230)
                **      ff[1] : top    (45674)
                **      ff[2] : right  (01540)
                **      ff[3] : left   (23762)
                **      ff[4] : near   (03740)
                **      ff[5] : far    (12651)
                **
                **  If a particular segment has no width (01 coincident
                **  with 32), just make the right side face.
                **
                **  Otherwise:
                **
                **  For all segments, build the "tube" walls (ff[0-3]).
                **
                **  If the last seg does not join smoothly with the first,
                **  cap the beginning and ending segments (ff[4-5]).
                **
                **  We don't need the internal "near" and "far" faces.
                */

                short	ff[6], ffidx, endcaps;

                // Does the wide seg chain NOT close smoothly?
                endcaps = wsap[nintern].er[0] != wsap[0].sr[0] || wsap[nintern].er[1] != wsap[0].sr[1] ||
						  wsap[nintern].er[2] != wsap[0].sr[2] || wsap[nintern].el[0] != wsap[0].sl[0] ||
						  wsap[nintern].el[1] != wsap[0].sl[1] || wsap[nintern].el[2] != wsap[0].sl[2];

                for(wsidx = 0; wsidx <= nintern; ++wsidx)
				{
                    ff[0] = ff[1] = ff[3] = ff[4] = ff[5] = 0;
                    ff[2] = 1;  // Always have at least the right face.

                    if( wsap[wsidx].sr[0] != wsap[wsidx].sl[0] || wsap[wsidx].sr[1] != wsap[wsidx].sl[1] ||
                        wsap[wsidx].sr[2] != wsap[wsidx].sl[2] || wsap[wsidx].er[0] != wsap[wsidx].el[0] ||
                        wsap[wsidx].er[1] != wsap[wsidx].el[1] || wsap[wsidx].er[2] != wsap[wsidx].el[2] )
					{
                        ff[0] = ff[1] = ff[3] = 1;
                        if(wsidx < 1 && endcaps)
							ff[4] = 1;
                        if(wsidx >= nintern && endcaps)
							ff[5] = 1;
                    }

                    // Create the appropriate face disp objs:
                    for(ffidx = 0; ffidx < 6; ++ffidx)
					{
                        if(!ff[ffidx])
							continue;

                        switch(ffidx)
						{
                        case 0: // Bottom
                                GR_PTCPY(ap1, wsap[wsidx].sr);
                                GR_PTCPY(ap2, wsap[wsidx].er);
                                GR_PTCPY(ap3, wsap[wsidx].el);
                                GR_PTCPY(ap4, wsap[wsidx].sl);
                                break;
                        case 1: // Top
                                for(coordidx = 0; coordidx < 3; ++coordidx)
								{
                                    ap1[coordidx] = wsap[wsidx].sr[coordidx] + _THICKNESS_[coordidx];
                                    ap2[coordidx] = wsap[wsidx].er[coordidx] + _THICKNESS_[coordidx];
                                    ap3[coordidx] = wsap[wsidx].el[coordidx] + _THICKNESS_[coordidx];
                                    ap4[coordidx] = wsap[wsidx].sl[coordidx] + _THICKNESS_[coordidx];
                                }
                                break;
                        case 2: // Right
                                GR_PTCPY(ap1, wsap[wsidx].sr);
                                GR_PTCPY(ap2, wsap[wsidx].er);
                                for(coordidx = 0; coordidx < 3; ++coordidx)
								{
                                    ap3[coordidx] = wsap[wsidx].er[coordidx] + _THICKNESS_[coordidx];
                                    ap4[coordidx] = wsap[wsidx].sr[coordidx] + _THICKNESS_[coordidx];
                                }
                                break;
                        case 3: // Left
                                GR_PTCPY(ap1, wsap[wsidx].sl);
                                GR_PTCPY(ap2, wsap[wsidx].el);
                                for(coordidx = 0; coordidx < 3; ++coordidx)
								{
                                    ap3[coordidx] = wsap[wsidx].el[coordidx] + _THICKNESS_[coordidx];
                                    ap4[coordidx] = wsap[wsidx].sl[coordidx] + _THICKNESS_[coordidx];
                                }
                                break;
                        case 4: // Near
                                GR_PTCPY(ap1, wsap[wsidx].sr);
                                GR_PTCPY(ap2, wsap[wsidx].sl);
                                for(coordidx = 0; coordidx < 3; ++coordidx)
								{
                                    ap3[coordidx] = wsap[wsidx].sl[coordidx] + _THICKNESS_[coordidx];
                                    ap4[coordidx] = wsap[wsidx].sr[coordidx] + _THICKNESS_[coordidx];
                                }
                                break;
                        case 5: // Far
                                GR_PTCPY(ap1, wsap[wsidx].er);
                                GR_PTCPY(ap2, wsap[wsidx].el);
                                for(coordidx = 0; coordidx < 3; ++coordidx)
								{
                                    ap3[coordidx] = wsap[wsidx].el[coordidx] + _THICKNESS_[coordidx];
                                    ap4[coordidx] = wsap[wsidx].er[coordidx] + _THICKNESS_[coordidx];
                                }
                        } // switch

						pointsArray4.Set(0, C3Point(ap1[0], ap1[1], ap1[2]));
						pointsArray4.Set(1, C3Point(ap2[0], ap2[1], ap2[2]));
						pointsArray4.Set(2, C3Point(ap3[0], ap3[1], ap3[2]));
						pointsArray4.Set(3, C3Point(ap4[0], ap4[1], ap4[2]));

						if(rc = ClipAndCreateDO(pSourceEntity, pointsArray4, 1, 0, ndims, viewp, &begdo, &enddo))
							goto out;

                    }	// End for each ffidx
                }	// End for each wsap[] element
            }  // End else UCS (3D)

        }
		else
		{	// No width

            if(viewp->projmode)
			{	// Planar (a "band" plus internal joints)

                int		lastbandptidx;

                // First, make the "band" without the joints:

				lastbandptidx = 2 * (endidx + 1) - 1;
				pointsArray.setSize(lastbandptidx + 1);

                for(fi1 = 0; fi1 <= endidx; ++fi1)
				{
                    // CAREFUL: Always check for begpclp and endpclp first:
                    if(!fi1)
						thispclp = begpclp;
                    else
						if(fi1 == endidx)
							thispclp = endpclp;
						else
							if(fi1 == 1)
								thispclp = ilink;
							else
								thispclp = thispclp->next;

                    if(viewp->ltsclwblk)
                        gr_ncs2ucs(thispclp->pt, ap1, 0, viewp);
                    else
                        GR_PTCPY(ap1, thispclp->pt);

					pointsArray.Set(fi1, C3Point(ap1[0], ap1[1], ap1[2]));
					pointsArray.Set(lastbandptidx - fi1, C3Point(ap1[0] + _THICKNESS_[0],
																 ap1[1] + _THICKNESS_[1],
																 ap1[2] + _THICKNESS_[2]));
                }

                // Make the "band" open if the viewp->pc chain is closed so that we don't have a duplicate joint at the end:
				if(rc = ClipAndCreateDO(pSourceEntity, pointsArray, !closed, fillable, ndims, viewp, &begdo, &enddo))
					goto out;

                // Now add the internal joints (by using the band just made):

                for(fi1 = 1; fi1 < endidx; ++fi1)
				{
					pointsArray2.Set(0, pointsArray[fi1]);
					pointsArray2.Set(1, pointsArray[lastbandptidx-fi1]);

					if(rc = ClipAndCreateDO(pSourceEntity, pointsArray2, 0, 0, ndims, viewp, &begdo, &enddo))
						goto out;
				}
            }	// 2D
			else
			{	// UCS (3D) (a face for each segment (and maybe caps))

				CPolyLine	pointsArray_tmp1, pointsArray_tmp2;
                if(capped)
				{
					pointsArray_tmp1.MakeRoom(0, endidx);
					pointsArray_tmp2.MakeRoom(0, endidx);
				}

                // Do the "siding":
                for(fi1 = 0, fi2 = 1, fi3 = endidx - 1;  fi1 <= fi3;  ++fi1, ++fi2)
				{
                    // CAREFUL: Always check for begpclp and endpclp first:
                    if(!fi1)
						thispclp = begpclp;
                    else
						if(fi1 == 1)
							thispclp = ilink;
						else
							thispclp = thispclp->next;

                    if(fi2 == endidx)
						nextpclp = endpclp;
					else
						if(fi2 == 1)
							nextpclp = ilink;
						else
							nextpclp = nextpclp->next;

                    if(viewp->ltsclwblk)
					{
                        gr_ncs2ucs(thispclp->pt, ap1, 0, viewp);
                        gr_ncs2ucs(nextpclp->pt, ap2, 0, viewp);
                    }
					else
					{
                        GR_PTCPY(ap1, thispclp->pt);
                        GR_PTCPY(ap2, nextpclp->pt);
                    }

					pointsArray4.Set(0, C3Point(ap1[0], ap1[1], ap1[2]));
					pointsArray4.Set(1, C3Point(ap2[0], ap2[1], ap2[2]));
					pointsArray4.Set(2, C3Point(ap2[0] + _THICKNESS_[0],
												ap2[1] + _THICKNESS_[1],
												ap2[2] + _THICKNESS_[2]));
					pointsArray4.Set(3, C3Point(ap1[0] + _THICKNESS_[0],
												ap1[1] + _THICKNESS_[1],
												ap1[2] + _THICKNESS_[2]));

					if(rc = ClipAndCreateDO(pSourceEntity, pointsArray4, 1, fillable, ndims, viewp, &begdo, &enddo))
						goto out;

					if(capped)
					{
						pointsArray_tmp1.Set(fi1, pointsArray4[0]);
						pointsArray_tmp2.Set(fi1, pointsArray4[3]);
					}
                }

                // Do the caps (by using the siding faces just made):

                if(capped)
				{
					if(rc = ClipAndCreateDO(pSourceEntity, pointsArray_tmp1, 1, fillable, ndims, viewp, &begdo, &enddo))
						goto out;

					if(rc = ClipAndCreateDO(pSourceEntity, pointsArray_tmp2, 1, fillable, ndims, viewp, &begdo, &enddo))
						goto out;
				}
            }  // End else UCS (3D)
        }  // End else no width
    }
	else
	{	// No thickness
        if(viewp->dowid)
		{	// Has non-0 width somewhere in it.

            // Make a face for each seg (unless there's no width):
            for(wsidx = 0; wsidx <= nintern; ++wsidx)
			{
                if( wsap[wsidx].sr[0] == wsap[wsidx].sl[0] && wsap[wsidx].sr[1] == wsap[wsidx].sl[1] &&
                    wsap[wsidx].sr[2] == wsap[wsidx].sl[2] && wsap[wsidx].er[0] == wsap[wsidx].el[0] &&
                    wsap[wsidx].er[1] == wsap[wsidx].el[1] && wsap[wsidx].er[2] == wsap[wsidx].el[2] )
				{	// No width

					pointsArray2.Set(0, C3Point(wsap[wsidx].sr[0], wsap[wsidx].sr[1], wsap[wsidx].sr[2]));
					pointsArray2.Set(1, C3Point(wsap[wsidx].er[0], wsap[wsidx].er[1], wsap[wsidx].er[2]));

					if(rc = ClipAndCreateDO(pSourceEntity, pointsArray2, 0, 0, ndims, viewp, &begdo, &enddo))
						goto out;
                }
				else
				{	// Non-0 width in this seg; make a face.

					pointsArray4.Set(0, C3Point(wsap[wsidx].sr[0], wsap[wsidx].sr[1], wsap[wsidx].sr[2]));
					pointsArray4.Set(1, C3Point(wsap[wsidx].er[0], wsap[wsidx].er[1], wsap[wsidx].er[2]));
					pointsArray4.Set(2, C3Point(wsap[wsidx].el[0], wsap[wsidx].el[1], wsap[wsidx].el[2]));
					pointsArray4.Set(3, C3Point(wsap[wsidx].sl[0], wsap[wsidx].sl[1], wsap[wsidx].sl[2]));

					if(rc = ClipAndCreateDO(pSourceEntity, pointsArray4, 1, fillable, ndims, viewp, &begdo, &enddo))
						goto out;
                }
            }  // End for each wide seg element.

        }	// End with width
		else
		{	// No width (just a chain).

			int		ArraySize = endidx + 1 - closed;
			pointsArray.setSize(ArraySize);

			for(fi1 = 0; fi1 < ArraySize; ++fi1)
			{
				// CAREFUL: Always check for begpclp and endpclp first:
				if(!fi1)
					thispclp = begpclp;
				else
					if(fi1 == endidx)
						thispclp = endpclp;
					else
						if(fi1 == 1)
							thispclp = ilink;
						else
							thispclp = thispclp->next;

				if(viewp->ltsclwblk)
					gr_ncs2ucs(thispclp->pt, ap1, 0, viewp);
				else
					GR_PTCPY(ap1, thispclp->pt);
				pointsArray.Set(fi1, C3Point(ap1[0], ap1[1], ap1[2]));
			}

			rc = ClipAndCreateDO(pSourceEntity, pointsArray, closed, fillable, ndims, viewp, &begdo, &enddo);
        }	// no width
    }	// no thickness

out:

    if(rc)
	{
		gr_freedisplayobjectll(begdo);
		begdo = enddo = NULL;
	}
    
	if(wsap)
	{
		IC_FREE(wsap);
		wsap = NULL;
	}

    *begpp = begdo;
	*endpp = enddo;

    return rc;
}

#undef _THICKNESS_
