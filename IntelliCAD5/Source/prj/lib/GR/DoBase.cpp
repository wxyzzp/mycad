#include "Gr.h"
#include "DoBase.h"
#include "pc2dispobj.h"

extern gr_view gr_defview;

CDoBase CDoBase::s_theDoBase;

RC CDoBase::s_geoRC;
CPolyLine CDoBase::s_pointsArray2(2, s_geoRC);
CPolyLine CDoBase::s_pointsArray4(4, s_geoRC);
CPolyLine CDoBase::s_pointsArray10(10, s_geoRC);
CPolyLine CDoBase::s_pointsArray;

CDoBase::CDoBase(): m_pEntity(NULL), m_pViewData(NULL), m_pDrawing(NULL), m_pDC(NULL),
m_pListBegin(NULL), m_pListEnd(NULL)
{
}

void CDoBase::setPointsData(gr_pclink* pInitialPoint, gr_pclink* pLastPoint, gr_pclink* pInternalPoint, long nInternalPoints)
{
	m_pInitialPoint = pInitialPoint;
	m_pLastPoint = pLastPoint;
	m_pInternalPoint = pInternalPoint;
	m_nInternalPoints = nInternalPoints;
}

void CDoBase::setFlags(short closed, short capped)
{
	m_closed = closed;
	m_capped = capped;
}

void CDoBase::setDotDir(sds_point dotDir)
{
	if(dotDir != NULL)
		m_dotDir = dotDir;
	else
		m_dotDir.nullify();
}

void CDoBase::setPointsData(gr_displayobject* pListBegin, gr_displayobject* pListEnd)
{
	m_pListBegin = pListBegin;
	m_pListEnd = pListEnd;
}

int CDoBase::getBoundingBox(CD3& minPoint, CD3& maxPoint)
{
	return getPlanarBoundingBox(m_pViewData, m_pListBegin, minPoint, maxPoint);
}

int CDoBase::clipAndCreateDO(CPolyLine& points, short closed, short fillable, short dimension, 
					  gr_displayobject** ppBegNewDO, gr_displayobject**	ppEndNewDO)
{

#define GR_LINKDO	if(!*ppBegNewDO)					\
						*ppBegNewDO = pNewDO;			\
					else								\
						(*ppEndNewDO)->next = pNewDO;	\
					*ppEndNewDO = pNewDO;				\
					pNewDO = NULL;

	ASSERT(m_pEntity && m_pViewData && ppBegNewDO && ppEndNewDO);

	int					size = points.Size(),
						vertNum;
	gr_displayobject*	pNewDO;

	if(m_pViewData->m_Clipper.IsNotEmpty())
	{
		if(size > 2 || fillable)		// clip as polyline if we have more than 2 points
		{
			PPolyLine		pPoints;
			PPolylinesList	PPolylines;
			RC				geoRc;

			if(!(pPoints = new CPolyLine(points, geoRc)) || geoRc != SUCCESS)
				return -3;

			if(closed && points[0] != points.Last() && pPoints->Add(points[0]) != SUCCESS)
				return -3;

			PPolylines.push_back(pPoints);

			// clip
			bool	bClipRetCode;
			if(fillable)
				bClipRetCode = m_pViewData->m_Clipper.ClipPolygons(PPolylines);
			else
				bClipRetCode = m_pViewData->m_Clipper.ClipPolylines(PPolylines);
			
			if(!bClipRetCode)
			{
				FreePointersList(PPolylines);
				return -3;
			}

			// create disp objs
			for(PPolylinesList::iterator pPPolyline = PPolylines.begin(); pPPolyline != PPolylines.end(); ++pPPolyline)
			{
				if(!(pNewDO = gr_displayobject::newDisplayObject(m_pEntity)))
				{
					FreePointersList(PPolylines);
					return -3;
				}

				pNewDO->next = NULL;
				pNewDO->type = (dimension > 2) + fillable;
				if((**pPPolyline)[0] == (*pPPolyline)->Last())
					pNewDO->type += DISP_OBJ_PTS_CLOSED;
				pNewDO->color = m_pViewData->color;
				pNewDO->lweight = m_pViewData->lweight;	// EBATECH(CNBR) 24-03-2003 Bugzilla#78471 Support Lineweight
				size = pNewDO->npts = (*pPPolyline)->Size();
				if(!(pNewDO->chain = new sds_real [size * dimension + 1]))	/*D.G.*/// + 1 sds_real because
				{						// we need 3 sds_reals for every vertex we considering, so allocate one for dimension == 2.
					IC_FREE(pNewDO);
					FreePointersList(PPolylines);
					return -3;
				}

				for(vertNum = 0; vertNum < size; ++vertNum)
				{
					C3Point&	geoPoint = (*(*pPPolyline))[vertNum];
					pNewDO->chain[vertNum * dimension] = geoPoint.X();
					pNewDO->chain[vertNum * dimension + 1] = geoPoint.Y();
					pNewDO->chain[vertNum * dimension + 2] = geoPoint.Z();
					if(m_pViewData->projmode && gr_ucs2rp(pNewDO->chain + vertNum * dimension, pNewDO->chain + vertNum * dimension, m_pViewData))
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
			for(vertNum = 1; vertNum < size; ++vertNum)
				Lines.push_back(CLine(points[vertNum-1], points[vertNum]));

			if(closed)
			{
				Lines.push_back(CLine(points[size-1], points[0]));
				++size;
			}

			// clip lines
			if(!m_pViewData->m_Clipper.ClipLines(Lines))
				return -3;

			// create disp objs
			C3Point		geoPoint1, geoPoint2;
			for(LinesList::iterator pLine = Lines.begin(); pLine != Lines.end(); ++pLine)
			{
				if(!(pNewDO = gr_displayobject::newDisplayObject(m_pEntity)))
					return -3;

				pNewDO->next = NULL;
				pNewDO->type = (dimension > 2);
				pNewDO->color = m_pViewData->color;
				pNewDO->lweight = m_pViewData->lweight;	// EBATECH(CNBR) 24-03-2003 Bugzilla#78471 Support Lineweight
				pNewDO->npts = 2;
				if(!(pNewDO->chain = new sds_real[2 * dimension + 1 ]))	/*D.G.*/// + 1 sds_real because
				{						// we need 3 sds_reals for every vertex we considering, so allocate one for dimension == 2.
					IC_FREE(pNewDO);
					return -3;
				}

				if(pLine->GetEnds(geoPoint1, geoPoint2) != SUCCESS)
				{
					gr_displayobject::freeDisplayObject(pNewDO);
					return -3;
				}

				pNewDO->chain[0] = geoPoint1.X();
				pNewDO->chain[1] = geoPoint1.Y();
				pNewDO->chain[2] = geoPoint1.Z();
				if(m_pViewData->projmode && gr_ucs2rp(pNewDO->chain, pNewDO->chain, m_pViewData))
				{
					gr_displayobject::freeDisplayObject(pNewDO);
					return 0;
				}

				pNewDO->chain[dimension] = geoPoint2.X();
				pNewDO->chain[dimension + 1] = geoPoint2.Y();
				pNewDO->chain[dimension + 2] = geoPoint2.Z();
				if(m_pViewData->projmode && gr_ucs2rp(pNewDO->chain + dimension, pNewDO->chain + dimension, m_pViewData))
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
		
		if(!(pNewDO = gr_displayobject::newDisplayObject( m_pEntity )))
			return -3;

		pNewDO->next = NULL;
		pNewDO->type = (dimension > 2) + 2 * closed + fillable;
		pNewDO->color = m_pViewData->color;
		pNewDO->lweight = m_pViewData->lweight;	// EBATECH(CNBR) 24-03-2003 Bugzilla#78471 Support Lineweight
		pNewDO->npts = size;

		if(!(pNewDO->chain = new sds_real[size * dimension + 1]))	/*D.G.*/// + 1 sds_real because
		{						// we need 3 sds_reals for every vertex we considering, so allocate one for dimension == 2.
			IC_FREE(pNewDO);
			return -3;
		}

		for(vertNum = 0; vertNum < size; ++vertNum)
		{
			pNewDO->chain[vertNum * dimension] = points[vertNum].X();
			pNewDO->chain[vertNum * dimension + 1] = points[vertNum].Y();
			pNewDO->chain[vertNum * dimension + 2] = points[vertNum].Z();

			if(m_pViewData->projmode && gr_ucs2rp(pNewDO->chain + vertNum * dimension, pNewDO->chain + vertNum * dimension, m_pViewData))
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

int CDoBase::formDOs()
{
/*
**  This is an extension of gr_getdispobjs().
**
**  Converts a chain of points to a llist of display objects.  Handles
**  thickness for each m_pViewData->projmode.
**
**  Note that the chain may be in the NCS or the UCS at this point,
**  depending on the value of our system variable LTSCLWBLK.  If the
**  points have not been converted to UCS, they are here (via
**  gr_ncs2ucs()), and then they are projected onto the real plane if
**  m_pViewData->projmode!=0.
**
**  The chain of points to convert (the pc chain) is as follows:
**
**      m_pInitialPoint
**      m_pInternalPoint
**      m_pInternalPoint->next... (for m_nInternalPoints links)
**      m_pLastPoint
**
**  m_pInitialPoint and m_pLastPoint are always expected to be non-NULL;
**  m_pInternalPoint can be NULL and m_nInternalPoints can be 0 when there is only a dot
**  or one chord.
**
**  m_pInitialPoint and m_pLastPoint may lie in between vertices in the m_pViewData->pc
**  llist; gr_doltype() determines them when breaking up m_pViewData->pc
**  for linetype.  FOR WHOLE CLOSED ENTITIES, THIS CHAIN IS ALWAYS
**  EXPLICITLY CLOSED (m_pInitialPoint AND m_pLastPoint HAVE COINCIDENT POINTS).
**
**  "m_closed" flags that the chain is for a WHOLE entity and is
**  considered m_closed (m_pInitialPoint->pt and m_pLastPoint->pt are coincident).
**
**  "m_capped" flags that the chain is for a WHOLE entity and is
**  to be m_capped in UCS (3D) mode.
**
**  Parameter m_dotDir is needed by gr_getwidepc() to generate widths
**  for dots in ltypes that have dots.  It's the pc chain direction
**  vector at the time the dot is encountered.  It's NULL in other
**  cases (dashes).  See gr_getwidepc().  All functions except gr_doltype()
**  should be calling this one with m_dotDir!=NULL.
**
**  Don't confuse a m_closed entity with a m_closed gr_displayobject llist.
**
**  *m_pListBegin is pointed to the first display object in the llist.
**  *m_pListEnd is pointed to the last  display object in the llist (for
**  speed in chaining).
**
**  Caller's responsibility to free, of course.
**
**  Returns: gr_getdispobjs() error codes (0 or -3 (no memory)).
*/

/*D.G.*/// Rewritten for using ClipAndCreateDO.
    char				fillable;
    short				wsidx, coordidx, rc;
	int					endidx, ndims, fi1, fi2, fi3, fint1;
    sds_point			ap1, ap2, ap3, ap4;
    gr_displayobject	*begdo, *enddo;
    gr_pclink			*thispclp, *nextpclp;
    gr_wideseglink*		wsap;
	
    rc = 0;
	begdo = enddo = NULL;
	wsap = NULL;

    if(!m_pViewData || isEqualZeroWithin(m_pViewData->viewsize))
		m_pViewData = &gr_defview;

    /*
    **  The variable ndims should be used strictly for determining the
    **  type of display object (2D (RP) with an XYXY... chain,
    **  or 3D (UCS) with an XYZXYZ... chain) -- NOT THE CONSTRUCTION
    **  LOGIC (which should use m_pViewData->projmode).
    */
    ndims = 2 + (!m_pViewData->projmode || m_pViewData->noucs2rp);

    endidx = m_nInternalPoints + 1;	// For loops, the 0-based index of the last point (for deciding when to use m_pLastPoint, etc.)

    // If we're not doing thickness and it's a fillable entity (SOLID, TRACE, 2D POLYLINE with width),
    // set fillable to the bit-2 mask (4) for the disp obj "type" member:
    fillable = 0;
    if(!m_pViewData->dothick && m_pViewData->ment)
	{
        fint1 = m_pViewData->ment->ret_type();
        if( fint1 == DB_SOLID || fint1 == DB_TRACE ||
			(fint1 == DB_POLYLINE && m_pViewData->dowid) || (fint1 == DB_LWPOLYLINE && m_pViewData->dowid) )
			fillable = DISP_OBJ_PTS_FILLABLE;
    }

    if(m_pViewData->dowid)
	{	// Has non-0 width somewhere in it. Get the UCS wide seg array.
        if(gr_getwidepc(m_pInitialPoint, m_pLastPoint, m_pInternalPoint, m_nInternalPoints, m_closed, m_dotDir, &wsap, m_pViewData) || !wsap)
		{
			rc = -3;
			goto out;
		}
    }


    if(m_pViewData->dothick)
	{
        if(m_pViewData->dowid)
		{	// Has non-0 width somewhere in it.
			if(m_pViewData->projmode)
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

                for(wsidx = 0; wsidx <= m_nInternalPoints; ++wsidx)
				{
                    if( wsap[wsidx].sr[0] == wsap[wsidx].sl[0] && wsap[wsidx].sr[1] == wsap[wsidx].sl[1] &&
                        wsap[wsidx].sr[2] == wsap[wsidx].sl[2] && wsap[wsidx].er[0] == wsap[wsidx].el[0] &&
                        wsap[wsidx].er[1] == wsap[wsidx].el[1] && wsap[wsidx].er[2] == wsap[wsidx].el[2] )
					{
                        // This seg has no width; make the 01540 disp obj:
						s_pointsArray4.Set(0, C3Point(wsap[wsidx].sr[0], wsap[wsidx].sr[1], wsap[wsidx].sr[2]));
						s_pointsArray4.Set(1, C3Point(wsap[wsidx].er[0], wsap[wsidx].er[1], wsap[wsidx].er[2]));

						s_pointsArray4.Set(2, C3Point(wsap[wsidx].er[0] + m_pViewData->m_thickness[0],
													wsap[wsidx].er[1] + m_pViewData->m_thickness[1],
													wsap[wsidx].er[2] + m_pViewData->m_thickness[2]));
						s_pointsArray4.Set(3, C3Point(wsap[wsidx].sr[0] + m_pViewData->m_thickness[0],
													wsap[wsidx].sr[1] + m_pViewData->m_thickness[1],
													wsap[wsidx].sr[2] + m_pViewData->m_thickness[2]));

						if(rc = clipAndCreateDO(s_pointsArray4, 1, 0, ndims, &begdo, &enddo))
							goto out;

                        continue;
                    }

                    // This seg has width if execution gets to here.

                    if(wsidx < 1)
					{	// First seg

					wholewidesolid:

						// Make the 0123045674 disp obj:

						// The base level (01230):
						s_pointsArray10.Set(0, C3Point(wsap[wsidx].sr[0], wsap[wsidx].sr[1], wsap[wsidx].sr[2]));
						s_pointsArray10.Set(1, C3Point(wsap[wsidx].er[0], wsap[wsidx].er[1], wsap[wsidx].er[2]));
						s_pointsArray10.Set(2, C3Point(wsap[wsidx].el[0], wsap[wsidx].el[1], wsap[wsidx].el[2]));
						s_pointsArray10.Set(3, C3Point(wsap[wsidx].sl[0], wsap[wsidx].sl[1], wsap[wsidx].sl[2]));
						s_pointsArray10.Set(4, C3Point(wsap[wsidx].sr[0], wsap[wsidx].sr[1], wsap[wsidx].sr[2]));

						// The thickness level (45674):
						s_pointsArray10.Set(5, C3Point(wsap[wsidx].sr[0] + m_pViewData->m_thickness[0],
													 wsap[wsidx].sr[1] + m_pViewData->m_thickness[1],
													 wsap[wsidx].sr[2] + m_pViewData->m_thickness[2]));
						s_pointsArray10.Set(6, C3Point(wsap[wsidx].er[0] + m_pViewData->m_thickness[0],
													 wsap[wsidx].er[1] + m_pViewData->m_thickness[1],
													 wsap[wsidx].er[2] + m_pViewData->m_thickness[2]));
						s_pointsArray10.Set(7, C3Point(wsap[wsidx].el[0] + m_pViewData->m_thickness[0],
													 wsap[wsidx].el[1] + m_pViewData->m_thickness[1],
													 wsap[wsidx].el[2] + m_pViewData->m_thickness[2]));
						s_pointsArray10.Set(8, C3Point(wsap[wsidx].sl[0] + m_pViewData->m_thickness[0],
													 wsap[wsidx].sl[1] + m_pViewData->m_thickness[1],
													 wsap[wsidx].sl[2] + m_pViewData->m_thickness[2]));
						s_pointsArray10.Set(9, C3Point(wsap[wsidx].sr[0] + m_pViewData->m_thickness[0],
													 wsap[wsidx].sr[1] + m_pViewData->m_thickness[1],
													 wsap[wsidx].sr[2] + m_pViewData->m_thickness[2]));

						if(rc = clipAndCreateDO(s_pointsArray10, 0, 0, ndims, &begdo, &enddo))
							goto out;

						// Make the 15, 26, and 37 disp objs:

						for(fi1 = 1; fi1 < 4; ++fi1)
						{
							s_pointsArray2.Set(0, s_pointsArray10[fi1]);
							s_pointsArray2.Set(1, s_pointsArray10[fi1 + 5]);

							if(rc = clipAndCreateDO(s_pointsArray2, 0, 0, ndims, &begdo, &enddo))
								goto out;
						}
                    }
					else
						if(wsidx >= m_nInternalPoints)
						{	// Last seg

							// Check junction with prev seg to see if we need 03740:

							if( wsap[wsidx].sr[0] != wsap[wsidx-1].er[0] ||	wsap[wsidx].sr[1] != wsap[wsidx-1].er[1] ||
								wsap[wsidx].sr[2] != wsap[wsidx-1].er[2] ||	wsap[wsidx].sl[0] != wsap[wsidx-1].el[0] ||
								wsap[wsidx].sl[1] != wsap[wsidx-1].el[1] ||	wsap[wsidx].sl[2] != wsap[wsidx-1].el[2] )
							{
								s_pointsArray4.Set(0, C3Point(wsap[wsidx].sr[0], wsap[wsidx].sr[1], wsap[wsidx].sr[2]));
								s_pointsArray4.Set(1, C3Point(wsap[wsidx].sl[0], wsap[wsidx].sl[1], wsap[wsidx].sl[2]));

								s_pointsArray4.Set(2, C3Point(wsap[wsidx].sl[0] + m_pViewData->m_thickness[0],
															wsap[wsidx].sl[1] + m_pViewData->m_thickness[1],
															wsap[wsidx].sl[2] + m_pViewData->m_thickness[2]));
								s_pointsArray4.Set(3, C3Point(wsap[wsidx].sr[0] + m_pViewData->m_thickness[0],
															wsap[wsidx].sr[1] + m_pViewData->m_thickness[1],
															wsap[wsidx].sr[2] + m_pViewData->m_thickness[2]));

								if(rc = clipAndCreateDO(s_pointsArray4, 1, 0, ndims, &begdo, &enddo))
									goto out;
							}

							// Check junction with FIRST seg to see if we need 12561:

							if( wsap[wsidx].er[0] != wsap[0].sr[0] || wsap[wsidx].er[1] != wsap[0].sr[1] ||
								wsap[wsidx].er[2] != wsap[0].sr[2] || wsap[wsidx].el[0] != wsap[0].sl[0] ||
								wsap[wsidx].el[1] != wsap[0].sl[1] || wsap[wsidx].el[2] != wsap[0].sl[2] )
							{
								s_pointsArray4.Set(0, C3Point(wsap[wsidx].er[0], wsap[wsidx].er[1], wsap[wsidx].er[2]));
								s_pointsArray4.Set(1, C3Point(wsap[wsidx].el[0], wsap[wsidx].el[1], wsap[wsidx].el[2]));

								s_pointsArray4.Set(2, C3Point(wsap[wsidx].el[0] + m_pViewData->m_thickness[0],
															wsap[wsidx].el[1] + m_pViewData->m_thickness[1],
															wsap[wsidx].el[2] + m_pViewData->m_thickness[2]));
								s_pointsArray4.Set(3, C3Point(wsap[wsidx].er[0] + m_pViewData->m_thickness[0],
															wsap[wsidx].er[1] + m_pViewData->m_thickness[1],
															wsap[wsidx].er[2] + m_pViewData->m_thickness[2]));

								if(rc = clipAndCreateDO(s_pointsArray4, 1, 0, ndims, &begdo, &enddo))
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
										s_pointsArray2.Set(0, C3Point(wsap[wsidx].sl[0] + m_pViewData->m_thickness[0],
																   wsap[wsidx].sl[1] + m_pViewData->m_thickness[1],
																   wsap[wsidx].sl[2] + m_pViewData->m_thickness[2]));
										s_pointsArray2.Set(1, C3Point(wsap[wsidx].el[0] + m_pViewData->m_thickness[0],
																   wsap[wsidx].el[1] + m_pViewData->m_thickness[1],
																   wsap[wsidx].el[2] + m_pViewData->m_thickness[2]));
									}
									else
									{	// 23
										s_pointsArray2.Set(0, C3Point(wsap[wsidx].sl[0], wsap[wsidx].sl[1], wsap[wsidx].sl[2]));
										s_pointsArray2.Set(1, C3Point(wsap[wsidx].el[0], wsap[wsidx].el[1], wsap[wsidx].el[2]));
									}
								}
								else
								{	// 01 and 45 -- right pts
									if(fi1 > 1)
									{	// 45
										s_pointsArray2.Set(0, C3Point(wsap[wsidx].sr[0] + m_pViewData->m_thickness[0],
																   wsap[wsidx].sr[1] + m_pViewData->m_thickness[1],
																   wsap[wsidx].sr[2] + m_pViewData->m_thickness[2]));
										s_pointsArray2.Set(1, C3Point(wsap[wsidx].er[0] + m_pViewData->m_thickness[0],
																   wsap[wsidx].er[1] + m_pViewData->m_thickness[1],
																   wsap[wsidx].er[2] + m_pViewData->m_thickness[2]));
									}
									else
									{	// 01
										s_pointsArray2.Set(0, C3Point(wsap[wsidx].sr[0], wsap[wsidx].sr[1], wsap[wsidx].sr[2]));
										s_pointsArray2.Set(1, C3Point(wsap[wsidx].er[0], wsap[wsidx].er[1], wsap[wsidx].er[2]));
									}
								}

								if(rc = clipAndCreateDO(s_pointsArray2, 0, 0, ndims, &begdo, &enddo))
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

							s_pointsArray4.Set(0, C3Point(wsap[wsidx].sr[0], wsap[wsidx].sr[1], wsap[wsidx].sr[2]));
							s_pointsArray4.Set(1, C3Point(wsap[wsidx].er[0], wsap[wsidx].er[1], wsap[wsidx].er[2]));
							s_pointsArray4.Set(2, C3Point(wsap[wsidx].el[0], wsap[wsidx].el[1], wsap[wsidx].el[2]));
							s_pointsArray4.Set(3, C3Point(wsap[wsidx].sl[0], wsap[wsidx].sl[1], wsap[wsidx].sl[2]));

							if(rc = clipAndCreateDO(s_pointsArray4, 0, 0, ndims, &begdo, &enddo))
								goto out;

							s_pointsArray4.Set(0, C3Point(wsap[wsidx].sr[0] + m_pViewData->m_thickness[0],
														wsap[wsidx].sr[1] + m_pViewData->m_thickness[1],
														wsap[wsidx].sr[2] + m_pViewData->m_thickness[2]));
							s_pointsArray4.Set(1, C3Point(wsap[wsidx].er[0] + m_pViewData->m_thickness[0],
														wsap[wsidx].er[1] + m_pViewData->m_thickness[1],
														wsap[wsidx].er[2] + m_pViewData->m_thickness[2]));
							s_pointsArray4.Set(2, C3Point(wsap[wsidx].el[0] + m_pViewData->m_thickness[0],
														wsap[wsidx].el[1] + m_pViewData->m_thickness[1],
														wsap[wsidx].el[2] + m_pViewData->m_thickness[2]));
							s_pointsArray4.Set(3, C3Point(wsap[wsidx].sl[0] + m_pViewData->m_thickness[0],
														wsap[wsidx].sl[1] + m_pViewData->m_thickness[1],
														wsap[wsidx].sl[2] + m_pViewData->m_thickness[2]));

							if(rc = clipAndCreateDO(s_pointsArray4, 0, 0, ndims, &begdo, &enddo))
								goto out;

							// Make the 15 and 26 disp objs:

							s_pointsArray2.Set(0, C3Point(wsap[wsidx].er[0], wsap[wsidx].er[1], wsap[wsidx].er[2]));
							s_pointsArray2.Set(1, C3Point(wsap[wsidx].er[0] + m_pViewData->m_thickness[0],
														wsap[wsidx].er[1] + m_pViewData->m_thickness[1],
														wsap[wsidx].er[2] + m_pViewData->m_thickness[2]));

							if(rc = clipAndCreateDO(s_pointsArray2, 0, 0, ndims, &begdo, &enddo))
								goto out;

							s_pointsArray2.Set(0, C3Point(wsap[wsidx].el[0], wsap[wsidx].el[1], wsap[wsidx].el[2]));
							s_pointsArray2.Set(1, C3Point(wsap[wsidx].el[0] + m_pViewData->m_thickness[0],
														wsap[wsidx].el[1] + m_pViewData->m_thickness[1],
														wsap[wsidx].el[2] + m_pViewData->m_thickness[2]));

							if(rc = clipAndCreateDO(s_pointsArray2, 0, 0, ndims, &begdo, &enddo))
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
                endcaps = wsap[m_nInternalPoints].er[0] != wsap[0].sr[0] || wsap[m_nInternalPoints].er[1] != wsap[0].sr[1] ||
						  wsap[m_nInternalPoints].er[2] != wsap[0].sr[2] || wsap[m_nInternalPoints].el[0] != wsap[0].sl[0] ||
						  wsap[m_nInternalPoints].el[1] != wsap[0].sl[1] || wsap[m_nInternalPoints].el[2] != wsap[0].sl[2];

                for(wsidx = 0; wsidx <= m_nInternalPoints; ++wsidx)
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
                        if(wsidx >= m_nInternalPoints && endcaps)
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
                                    ap1[coordidx] = wsap[wsidx].sr[coordidx] + m_pViewData->m_thickness[coordidx];
                                    ap2[coordidx] = wsap[wsidx].er[coordidx] + m_pViewData->m_thickness[coordidx];
                                    ap3[coordidx] = wsap[wsidx].el[coordidx] + m_pViewData->m_thickness[coordidx];
                                    ap4[coordidx] = wsap[wsidx].sl[coordidx] + m_pViewData->m_thickness[coordidx];
                                }
                                break;
                        case 2: // Right
                                GR_PTCPY(ap1, wsap[wsidx].sr);
                                GR_PTCPY(ap2, wsap[wsidx].er);
                                for(coordidx = 0; coordidx < 3; ++coordidx)
								{
                                    ap3[coordidx] = wsap[wsidx].er[coordidx] + m_pViewData->m_thickness[coordidx];
                                    ap4[coordidx] = wsap[wsidx].sr[coordidx] + m_pViewData->m_thickness[coordidx];
                                }
                                break;
                        case 3: // Left
                                GR_PTCPY(ap1, wsap[wsidx].sl);
                                GR_PTCPY(ap2, wsap[wsidx].el);
                                for(coordidx = 0; coordidx < 3; ++coordidx)
								{
                                    ap3[coordidx] = wsap[wsidx].el[coordidx] + m_pViewData->m_thickness[coordidx];
                                    ap4[coordidx] = wsap[wsidx].sl[coordidx] + m_pViewData->m_thickness[coordidx];
                                }
                                break;
                        case 4: // Near
                                GR_PTCPY(ap1, wsap[wsidx].sr);
                                GR_PTCPY(ap2, wsap[wsidx].sl);
                                for(coordidx = 0; coordidx < 3; ++coordidx)
								{
                                    ap3[coordidx] = wsap[wsidx].sl[coordidx] + m_pViewData->m_thickness[coordidx];
                                    ap4[coordidx] = wsap[wsidx].sr[coordidx] + m_pViewData->m_thickness[coordidx];
                                }
                                break;
                        case 5: // Far
                                GR_PTCPY(ap1, wsap[wsidx].er);
                                GR_PTCPY(ap2, wsap[wsidx].el);
                                for(coordidx = 0; coordidx < 3; ++coordidx)
								{
                                    ap3[coordidx] = wsap[wsidx].el[coordidx] + m_pViewData->m_thickness[coordidx];
                                    ap4[coordidx] = wsap[wsidx].er[coordidx] + m_pViewData->m_thickness[coordidx];
                                }
                        } // switch

						s_pointsArray4.Set(0, C3Point(ap1[0], ap1[1], ap1[2]));
						s_pointsArray4.Set(1, C3Point(ap2[0], ap2[1], ap2[2]));
						s_pointsArray4.Set(2, C3Point(ap3[0], ap3[1], ap3[2]));
						s_pointsArray4.Set(3, C3Point(ap4[0], ap4[1], ap4[2]));

						if(rc = clipAndCreateDO(s_pointsArray4, 1, 0, ndims, &begdo, &enddo))
							goto out;

                    }	// End for each ffidx
                }	// End for each wsap[] element
            }  // End else UCS (3D)

        }
		else
		{	// No width

            if(m_pViewData->projmode)
			{	// Planar (a "band" plus internal joints)

                int		lastbandptidx;

                // First, make the "band" without the joints:

				lastbandptidx = 2 * (endidx + 1) - 1;
				s_pointsArray.setSize(lastbandptidx + 1);

                for(fi1 = 0; fi1 <= endidx; ++fi1)
				{
                    // CAREFUL: Always check for m_pInitialPoint and m_pLastPoint first:
                    if(!fi1)
						thispclp = m_pInitialPoint;
                    else
						if(fi1 == endidx)
							thispclp = m_pLastPoint;
						else
							if(fi1 == 1)
								thispclp = m_pInternalPoint;
							else
								thispclp = thispclp->next;

                    if(m_pViewData->ltsclwblk)
                        gr_ncs2ucs(thispclp->pt, ap1, 0, m_pViewData);
                    else
                        GR_PTCPY(ap1, thispclp->pt);

					s_pointsArray.Set(fi1, C3Point(ap1[0], ap1[1], ap1[2]));
					s_pointsArray.Set(lastbandptidx - fi1, C3Point(ap1[0] + m_pViewData->m_thickness[0],
																 ap1[1] + m_pViewData->m_thickness[1],
																 ap1[2] + m_pViewData->m_thickness[2]));
                }

                // Make the "band" open if the m_pViewData->pc chain is m_closed so that we don't have a duplicate joint at the end:
				if(rc = clipAndCreateDO(s_pointsArray, !m_closed, fillable, ndims, &begdo, &enddo))
					goto out;

                // Now add the internal joints (by using the band just made):

                for(fi1 = 1; fi1 < endidx; ++fi1)
				{
					s_pointsArray2.Set(0, s_pointsArray[fi1]);
					s_pointsArray2.Set(1, s_pointsArray[lastbandptidx-fi1]);

					if(rc = clipAndCreateDO(s_pointsArray2, 0, 0, ndims, &begdo, &enddo))
						goto out;
				}
            }	// 2D
			else
			{	// UCS (3D) (a face for each segment (and maybe caps))

				CPolyLine	pointsArray_tmp1, pointsArray_tmp2;
                if(m_capped)
				{
					pointsArray_tmp1.MakeRoom(0, endidx);
					pointsArray_tmp2.MakeRoom(0, endidx);
				}

                // Do the "siding":
                for(fi1 = 0, fi2 = 1, fi3 = endidx - 1;  fi1 <= fi3;  ++fi1, ++fi2)
				{
                    // CAREFUL: Always check for m_pInitialPoint and m_pLastPoint first:
                    if(!fi1)
						thispclp = m_pInitialPoint;
                    else
						if(fi1 == 1)
							thispclp = m_pInternalPoint;
						else
							thispclp = thispclp->next;

                    if(fi2 == endidx)
						nextpclp = m_pLastPoint;
					else
						if(fi2 == 1)
							nextpclp = m_pInternalPoint;
						else
							nextpclp = nextpclp->next;

                    if(m_pViewData->ltsclwblk)
					{
                        gr_ncs2ucs(thispclp->pt, ap1, 0, m_pViewData);
                        gr_ncs2ucs(nextpclp->pt, ap2, 0, m_pViewData);
                    }
					else
					{
                        GR_PTCPY(ap1, thispclp->pt);
                        GR_PTCPY(ap2, nextpclp->pt);
                    }

					s_pointsArray4.Set(0, C3Point(ap1[0], ap1[1], ap1[2]));
					s_pointsArray4.Set(1, C3Point(ap2[0], ap2[1], ap2[2]));
					s_pointsArray4.Set(2, C3Point(ap2[0] + m_pViewData->m_thickness[0],
												ap2[1] + m_pViewData->m_thickness[1],
												ap2[2] + m_pViewData->m_thickness[2]));
					s_pointsArray4.Set(3, C3Point(ap1[0] + m_pViewData->m_thickness[0],
												ap1[1] + m_pViewData->m_thickness[1],
												ap1[2] + m_pViewData->m_thickness[2]));

					if(rc = clipAndCreateDO(s_pointsArray4, 1, fillable, ndims, &begdo, &enddo))
						goto out;

					if(m_capped)
					{
						pointsArray_tmp1.Set(fi1, s_pointsArray4[0]);
						pointsArray_tmp2.Set(fi1, s_pointsArray4[3]);
					}
                }

                // Do the caps (by using the siding faces just made):

                if(m_capped)
				{
					if(rc = clipAndCreateDO(pointsArray_tmp1, 1, fillable, ndims, &begdo, &enddo))
						goto out;

					if(rc = clipAndCreateDO(pointsArray_tmp2, 1, fillable, ndims, &begdo, &enddo))
						goto out;
				}
            }  // End else UCS (3D)
        }  // End else no width
    }
	else
	{	// No thickness
        if(m_pViewData->dowid)
		{	// Has non-0 width somewhere in it.

            // Make a face for each seg (unless there's no width):
            for(wsidx = 0; wsidx <= m_nInternalPoints; ++wsidx)
			{
                if( wsap[wsidx].sr[0] == wsap[wsidx].sl[0] && wsap[wsidx].sr[1] == wsap[wsidx].sl[1] &&
                    wsap[wsidx].sr[2] == wsap[wsidx].sl[2] && wsap[wsidx].er[0] == wsap[wsidx].el[0] &&
                    wsap[wsidx].er[1] == wsap[wsidx].el[1] && wsap[wsidx].er[2] == wsap[wsidx].el[2] )
				{	// No width

					s_pointsArray2.Set(0, C3Point(wsap[wsidx].sr[0], wsap[wsidx].sr[1], wsap[wsidx].sr[2]));
					s_pointsArray2.Set(1, C3Point(wsap[wsidx].er[0], wsap[wsidx].er[1], wsap[wsidx].er[2]));

					if(rc = clipAndCreateDO(s_pointsArray2, 0, 0, ndims, &begdo, &enddo))
						goto out;
                }
				else
				{	// Non-0 width in this seg; make a face.

					s_pointsArray4.Set(0, C3Point(wsap[wsidx].sr[0], wsap[wsidx].sr[1], wsap[wsidx].sr[2]));
					s_pointsArray4.Set(1, C3Point(wsap[wsidx].er[0], wsap[wsidx].er[1], wsap[wsidx].er[2]));
					s_pointsArray4.Set(2, C3Point(wsap[wsidx].el[0], wsap[wsidx].el[1], wsap[wsidx].el[2]));
					s_pointsArray4.Set(3, C3Point(wsap[wsidx].sl[0], wsap[wsidx].sl[1], wsap[wsidx].sl[2]));

					if(rc = clipAndCreateDO(s_pointsArray4, 1, fillable, ndims, &begdo, &enddo))
						goto out;
                }
            }  // End for each wide seg element.

        }	// End with width
		else
		{	// No width (just a chain).

			int		ArraySize = endidx + 1 - m_closed;
			s_pointsArray.setSize(ArraySize);

			for(fi1 = 0; fi1 < ArraySize; ++fi1)
			{
				// CAREFUL: Always check for m_pInitialPoint and m_pLastPoint first:
				if(!fi1)
					thispclp = m_pInitialPoint;
				else
					if(fi1 == endidx)
						thispclp = m_pLastPoint;
					else
						if(fi1 == 1)
							thispclp = m_pInternalPoint;
						else
							thispclp = thispclp->next;

				if(m_pViewData->ltsclwblk)
					gr_ncs2ucs(thispclp->pt, ap1, 0, m_pViewData);
				else
					GR_PTCPY(ap1, thispclp->pt);
				s_pointsArray.Set(fi1, C3Point(ap1[0], ap1[1], ap1[2]));
			}

			rc = clipAndCreateDO(s_pointsArray, m_closed, fillable, ndims, &begdo, &enddo);
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

    m_pListBegin = begdo;
	m_pListEnd = enddo;

    return !rc;
}

int CDoBase::getDisplayObjects(gr_displayobject*& pListBegin, gr_displayobject*& pListEnd)
{
	if(formDOs() == 0)
		return -1;

	pListBegin = m_pListBegin;
	pListEnd = m_pListEnd;
	m_pListBegin = NULL;
	m_pListEnd = NULL;
	return 0;
}

int CDoBase::getBoundingBox(gr_displayobject* pListBegin, CD3& minPoint, CD3& maxPoint)
{
	gr_displayobject* pCur;
	for(pCur = pListBegin; pCur != NULL; pCur = pCur->next)
		 if(!(pCur->npts < 1 || pCur->chain == NULL || pCur->type & DISP_OBJ_SPECIAL))
			 break;

	if(pCur == NULL)
		return 0;

	if(pCur->type & DISP_OBJ_PTS_3D)
	{
		minPoint[0] = maxPoint[0] = pCur->chain[0];
		minPoint[1] = maxPoint[1] = pCur->chain[1];
		minPoint[2] = maxPoint[2] = pCur->chain[2];
	}
	else
	{
		minPoint[0] = maxPoint[0] = pCur->chain[0];
		minPoint[1] = maxPoint[1] = pCur->chain[1];
		minPoint[2] = maxPoint[2] = 0.0;
	}
	int i, n;
	for(; pCur != NULL; pCur = pCur->next)
	{
		if(pCur->npts < 1 || pCur->chain == NULL || pCur->type & DISP_OBJ_SPECIAL)
			continue;

		if(pCur->type & DISP_OBJ_PTS_3D)
		{
			n = 3 * pCur->npts;
			i = 0;
			while(i < n)
			{
				if(minPoint[0] > pCur->chain[i])
					minPoint[0] = pCur->chain[i];
				else 
					if(maxPoint[0] < pCur->chain[i])
						maxPoint[0] = pCur->chain[i];
				++i;
				if(minPoint[1] > pCur->chain[i])
					minPoint[1] = pCur->chain[i];
				else
					if(maxPoint[1] < pCur->chain[i])
						maxPoint[1] = pCur->chain[i];
				++i;
				if(minPoint[2] > pCur->chain[i])
					minPoint[2] = pCur->chain[i];
				else
					if(maxPoint[2] < pCur->chain[i])
						maxPoint[2] = pCur->chain[i];
				++i;
			}
		}
		else
		{
			n = 2 * pCur->npts;
			i = 0;
			while(i < n)
			{
				if(minPoint[0] > pCur->chain[i])
					minPoint[0] = pCur->chain[i];
				else 
					if(maxPoint[0] < pCur->chain[i])
						maxPoint[0] = pCur->chain[i];
				++i;
				if(minPoint[1] > pCur->chain[i])
					minPoint[1] = pCur->chain[i];
				else
					if(maxPoint[1] < pCur->chain[i])
						maxPoint[1] = pCur->chain[i];
				++i;
			}
		}
	}
	return 1;
}

int CDoBase::getPlanarBoundingBox(gr_view* pViewData, gr_displayobject* pListBegin, CD3& minPoint, CD3& maxPoint)
{
	gr_displayobject* pCur;
	for(pCur = pListBegin; pCur != NULL; pCur = pCur->next)
		 if(!(pCur->npts < 1 || pCur->chain == NULL || pCur->type & DISP_OBJ_SPECIAL))
			 break;

	if(pCur == NULL)
		return 0;

	CD3 projected;
	if(pCur->type & DISP_OBJ_PTS_3D)
	{
		assert(pViewData != NULL);
		gr_ucs2rp(&(pCur->chain[0]), projected, pViewData);
		minPoint[0] = maxPoint[0] = projected[0];
		minPoint[1] = maxPoint[1] = projected[1];
		minPoint[2] = maxPoint[2] = 0.0;
	}
	else
	{
		minPoint[0] = maxPoint[0] = pCur->chain[0];
		minPoint[1] = maxPoint[1] = pCur->chain[1];
		minPoint[2] = maxPoint[2] = 0.0;
	}
	int i, n;
	for(; pCur != NULL; pCur = pCur->next)
	{
		if(pCur->npts < 1 || pCur->chain == NULL || pCur->type & DISP_OBJ_SPECIAL)
			continue;

		if(pCur->type & DISP_OBJ_PTS_3D)
		{
			n = 3 * pCur->npts;
			i = 0;
			while(i < n)
			{
				gr_ucs2rp(&(pCur->chain[i]), projected, pViewData);
				if(minPoint[0] > projected[0])
					minPoint[0] = projected[0];
				else 
					if(maxPoint[0] < projected[0])
						maxPoint[0] = projected[0];
				if(minPoint[1] > projected[1])
					minPoint[1] = projected[1];
				else
					if(maxPoint[1] <projected[1])
						maxPoint[1] = projected[1];
				i += 3;
			}
		}
		else
		{
			n = 2 * pCur->npts;
			i = 0;
			while(i < n)
			{
				if(minPoint[0] > pCur->chain[i])
					minPoint[0] = pCur->chain[i];
				else 
					if(maxPoint[0] < pCur->chain[i])
						maxPoint[0] = pCur->chain[i];
				++i;
				if(minPoint[1] > pCur->chain[i])
					minPoint[1] = pCur->chain[i];
				else
					if(maxPoint[1] < pCur->chain[i])
						maxPoint[1] = pCur->chain[i];
				++i;
			}
		}
	}
	return 1;
}

int CDoBase::getPlanarBoundingBox(gr_view* pViewData, db_entity* pEntity, CD3& minPoint, CD3& maxPoint)
{
	assert(pViewData != NULL);
	assert(pEntity != NULL);

	pEntity->get_extent(minPoint, maxPoint);
	if(minPoint.isNullWithin() && maxPoint.isNullWithin())
		getPlanarBoundingBox(pViewData, (gr_displayobject*)pEntity->ret_disp(), minPoint, maxPoint);

	CD3 peaks[8];
	peaks[0] = minPoint;
	peaks[1] = maxPoint;
	peaks[2][0] = minPoint[0]; peaks[2][1] = minPoint[1]; peaks[2][2] = maxPoint[2];
	peaks[3][0] = minPoint[0]; peaks[3][1] = maxPoint[1]; peaks[3][2] = minPoint[2];
	peaks[4][0] = minPoint[0]; peaks[4][1] = maxPoint[1]; peaks[4][2] = maxPoint[2];
	peaks[5][0] = maxPoint[0]; peaks[5][1] = minPoint[1]; peaks[5][2] = minPoint[2];
	peaks[6][0] = maxPoint[0]; peaks[6][1] = minPoint[1]; peaks[6][2] = maxPoint[2];
	peaks[7][0] = maxPoint[0]; peaks[7][1] = maxPoint[1]; peaks[7][2] = minPoint[2];
	CD3 projectedPoint;

	gr_ucs2rp(peaks[0], projectedPoint, pViewData);
	minPoint = projectedPoint;
	maxPoint = projectedPoint;
    for(int i = 1; i < 8; ++i)
    {
		gr_ucs2rp(peaks[i], projectedPoint, pViewData);

        if(minPoint[0] > projectedPoint[0])
            minPoint[0] = projectedPoint[0];
		else
			if(maxPoint[0] < projectedPoint[0])
				maxPoint[0] = projectedPoint[0];
        if(minPoint[1] > projectedPoint[1])
            minPoint[1] = projectedPoint[1];
		else
	        if(maxPoint[1] < projectedPoint[1])
		        maxPoint[1] = projectedPoint[1];
        if(minPoint[2] > projectedPoint[2])
            minPoint[2] = projectedPoint[2];
		else
			if(maxPoint[2] < projectedPoint[2])
				maxPoint[2] = projectedPoint[2];
    }
	return 1;
}

