// \source\prj\lib\gr\clipper.cpp

#pragma warning (disable : 4786)

#include "gr.h"
#include "objects.h"
#include "crvpair.h"
#include <float.h>	// for DBL_MAX
#include <map>
#ifdef _DEBUG
#include <fstream.h>
#endif // _DEBUG


//
// ======================== CFilter class METHODS IMPLEMENTATION ========================
//

void
CFilter::Clear()
{
	m_ClippingPolyline.RemoveAll();
	m_ClipPlanes = 0;
}


/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Get data from given spatial_filter object and fill our structures;
 *			transform filter's data using given matrix.
 * Returns:	True in success and false in failure or if there are no data.
 ********************************************************************************/
bool
CFilter::FillData
(
 db_handitem*	pSourceSpatialFilter,	// =>
 CMatrix4*		pInsertTransformation	// =>
)
{
	if(pSourceSpatialFilter)
		ASSERT(pSourceSpatialFilter->ret_type() == DB_SPATIAL_FILTER);
	if(!pSourceSpatialFilter || pSourceSpatialFilter->ret_type() != DB_SPATIAL_FILTER)
		return false;

	int		Enabled;
	((db_spatial_filter*)pSourceSpatialFilter)->get_71(&Enabled);
	if(!Enabled)
		return false;

	Clear();

	sds_point	pTmpMatrix[4];
	sds_matrix	pMatrix;
	int			i;

	pMatrix[3][0] = pMatrix[3][1] = pMatrix[3][2] = 0.0;
	pMatrix[3][3] = 1.0;

	// clipping contour points
	int			OriginalNumberOfPoints = ((db_spatial_filter*)pSourceSpatialFilter)->ret_npts(),
				NumberOfPoints;
	sds_point*	pPointsArray;

	if(OriginalNumberOfPoints == 2)
		NumberOfPoints = 5;				// we will create rectangle adding two points
	else
		NumberOfPoints = OriginalNumberOfPoints + 1;

	if(!(pPointsArray = new sds_point[NumberOfPoints]))
		return false;

	((db_spatial_filter*)pSourceSpatialFilter)->get_ptarray(pPointsArray, OriginalNumberOfPoints);

	if(OriginalNumberOfPoints == 2)
	{
		GR_PTCPY(pPointsArray[2], pPointsArray[1]);
		pPointsArray[1][0] = pPointsArray[0][0];
		GR_PTCPY(pPointsArray[3], pPointsArray[0]);
		pPointsArray[3][0] = pPointsArray[2][0];
	}

	GR_PTCPY(pPointsArray[NumberOfPoints-1], pPointsArray[0]);	// close the polyline

	m_ContourExtents.Reset();
	for(i = 0; i < NumberOfPoints; ++i)
	{
		if(m_ClippingPolyline.Add(C3Point(pPointsArray[i][0], pPointsArray[i][1], pPointsArray[i][2])) != SUCCESS)
		{
			delete [] pPointsArray;
			return false;
		}
		m_ContourExtents.Update(m_ClippingPolyline.Last());
	}

	delete [] pPointsArray;

	if(!m_ClippingPolyline.Size())
		return false;

	// clipping planes
	if(((db_spatial_filter*)pSourceSpatialFilter)->ret_frontclipon())
	{
		m_ClipPlanes |= CLIPPER_FRONT_PLANE;
		m_FrontPlaneDistance = ((db_spatial_filter*)pSourceSpatialFilter)->ret_frontclip();
	}
	else
		m_FrontPlaneDistance = DBL_MAX;

	if(((db_spatial_filter*)pSourceSpatialFilter)->ret_backclipon())
	{
		m_ClipPlanes |= CLIPPER_BACK_PLANE;
		m_BackPlaneDistance = ((db_spatial_filter*)pSourceSpatialFilter)->ret_backclip();
	}
	else
		m_BackPlaneDistance = - DBL_MAX;

	// transformation matrix
	CMatrix4	TransformMatrix;
	((db_spatial_filter*)pSourceSpatialFilter)->get_inverseblockmat(pTmpMatrix);
	for(i = 4; i--; )	// transpose the matrix
	{
		pMatrix[0][i] = pTmpMatrix[i][0];
		pMatrix[1][i] = pTmpMatrix[i][1];
		pMatrix[2][i] = pTmpMatrix[i][2];
	}

	TransformMatrix = (double*)pMatrix;

	((db_spatial_filter*)pSourceSpatialFilter)->get_clipboundmat(pTmpMatrix);
	for(i = 4; i--; )	// transpose the matrix
	{
		pMatrix[0][i] = pTmpMatrix[i][0];
		pMatrix[1][i] = pTmpMatrix[i][1];
		pMatrix[2][i] = pTmpMatrix[i][2];
	}

	m_ClipperTransformation = (double*)pMatrix;

	m_ClipperTransformation = m_ClipperTransformation * TransformMatrix.invert();

	if(pInsertTransformation)
		m_ClipperTransformation = m_ClipperTransformation * pInsertTransformation->invert();

	m_bIdentityTransformation = m_ClipperTransformation.isEqualWithin(CMatrix4::identity(), IC_ZRO);

	return true;
}


/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Clip given line by filter's clipping planes.
 * Returns: CLIPPER_RC_xxx return codes.
 ********************************************************************************/
short
CFilter::ClipLineByPlanes
(
 CLine&		LineToClip	// <=>
)const
{
	C3Point		Point1, Point2;
	double		Param;

	if(!m_ClipPlanes)
		return CLIPPER_RC_DRAWALL;

	if(LineToClip.GetEnds(Point1, Point2) != SUCCESS)
		return CLIPPER_RC_ERROR;

	if( (Point1.Z() < m_BackPlaneDistance  && Point2.Z() < m_BackPlaneDistance) ||
		(Point1.Z() > m_FrontPlaneDistance && Point2.Z() > m_FrontPlaneDistance) )
		return CLIPPER_RC_DONTDRAW;

	if( Point1.Z() >= m_BackPlaneDistance && Point1.Z() <= m_FrontPlaneDistance &&
		Point2.Z() >= m_BackPlaneDistance && Point2.Z() <= m_FrontPlaneDistance )
		return CLIPPER_RC_DRAWALL;
	else
	{
		ASSERT(Point1.Z() != Point2.Z());

		if(m_ClipPlanes & CLIPPER_FRONT_PLANE)
		{
			Param = (m_FrontPlaneDistance - Point1.Z()) / (Point2.Z() - Point1.Z());
			if(Param > 0.0 && Param < 1.0)		// line intersects front plane
			{
				if(Point1.Z() < m_FrontPlaneDistance)
					LineToClip.SetEndPoint(C3Point(Point1.X() + Param * (Point2.X() - Point1.X()),
												   Point1.Y() + Param * (Point2.Y() - Point1.Y()),
												   m_FrontPlaneDistance));
				else
					LineToClip.SetStartPoint(C3Point(Point1.X() + Param * (Point2.X() - Point1.X()),
													 Point1.Y() + Param * (Point2.Y() - Point1.Y()),
													 m_FrontPlaneDistance));
			}
		}

		if(LineToClip.GetEnds(Point1, Point2) != SUCCESS)
			return CLIPPER_RC_ERROR;

		ASSERT(Point1.Z() != Point2.Z());

		if(m_ClipPlanes & CLIPPER_BACK_PLANE)
		{
			Param = (m_BackPlaneDistance - Point1.Z()) / (Point2.Z() - Point1.Z());
			if(Param > 0.0 && Param < 1.0)		// line intersects back plane
			{
				if(Point1.Z() > m_BackPlaneDistance)
					LineToClip.SetEndPoint(C3Point(Point1.X() + Param * (Point2.X() - Point1.X()),
												   Point1.Y() + Param * (Point2.Y() - Point1.Y()),
												   m_BackPlaneDistance));
				else
					LineToClip.SetStartPoint(C3Point(Point1.X() + Param * (Point2.X() - Point1.X()),
													 Point1.Y() + Param * (Point2.Y() - Point1.Y()),
													 m_BackPlaneDistance));
			}
		}

		return CLIPPER_RC_CLIP;
	}
}


//
// ======================== CClipper class METHODS IMPLEMENTATION ========================
//

/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Free allocated memory.
 * Returns:	None.
 ********************************************************************************/
void
CClipper::Clear()
{
	FreePointersList(m_PFiltersList);
	m_PFiltersList.clear();
	m_bNotEmpty = false;
}


/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Add a new clipping filter.
 * Returns:	True if the filter was succesfully added.
 ********************************************************************************/
bool
CClipper::AddFilter
(
 db_handitem*	pSourceSpatialFilter,	// =>
 CMatrix4*		pInsertTransformation	// =>
)
{
	if(pSourceSpatialFilter)
		ASSERT(pSourceSpatialFilter->ret_type() == DB_SPATIAL_FILTER);
	if(!pSourceSpatialFilter || pSourceSpatialFilter->ret_type() != DB_SPATIAL_FILTER)
		return false;

	PFilter		pNewFilter = new CFilter;
	if(!pNewFilter)
		return false;

	if(!pNewFilter->FillData(pSourceSpatialFilter, pInsertTransformation))
	{
		delete pNewFilter;
		return false;
	}

	m_PFiltersList.push_back(pNewFilter);
	m_bNotEmpty = !m_PFiltersList.empty();

	return true;
}


/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Remove the last filter.
 * Returns:	None.
 ********************************************************************************/
void
CClipper::RemoveLastFilter()
{
	ASSERT(m_bNotEmpty);
	if(!m_bNotEmpty)
		return;

	delete m_PFiltersList.back();
	m_PFiltersList.pop_back();

	m_bNotEmpty = !m_PFiltersList.empty();
}


/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Clip given lines keeping their order.
 * Returns:	True in success and false in failure.
 ********************************************************************************/
bool
CClipper::ClipLines
(
 LinesList&		Lines	// <=>
)const
{
	LinesList			TmpLinesList;
	LinesList::iterator pLine, pEndLine, pCopyLine;

	for(PFiltersList::const_iterator pPFilter = m_PFiltersList.begin(); pPFilter != m_PFiltersList.end(); ++pPFilter)
	{
		if(Lines.empty())
			return true;

		pEndLine = Lines.end();
		--pEndLine;

		for(pLine = Lines.begin(); ; pLine = Lines.erase(pLine))
		{
			switch(ClipLineByFilter(*pLine, *pPFilter, TmpLinesList))
			{
			case CLIPPER_RC_ERROR	 :  return false;

			case CLIPPER_RC_DRAWALL	 :  Lines.push_back(*pLine);
										break;

			case CLIPPER_RC_CLIP	 :  for(pCopyLine = TmpLinesList.begin(); pCopyLine != TmpLinesList.end(); ++pCopyLine)
											Lines.push_back(*pCopyLine);

			case CLIPPER_RC_DONTDRAW :  break;

			default					 :  ASSERT(0);
										return false;
			}

			TmpLinesList.clear();

			if(pLine == pEndLine)
				break;
		}

		Lines.erase(pLine);
	}

	return true;
}


/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Clip given polylines keeping their order.
 * Returns:	True in success and false in failure.
 ********************************************************************************/
bool
CClipper::ClipPolylines
(
 PPolylinesList&	PPolylines	// <=>
)const
{
	int					VertNum;
	CLine				LineToClip;
	LinesList			TmpLinesList;
	LinesList			LinesOut;

	LinesList::iterator			pLine;
	PPolylinesList::iterator	pPPolyline;

	for(PFiltersList::const_iterator pPFilter = m_PFiltersList.begin(); pPFilter != m_PFiltersList.end(); ++pPFilter)
	{
		if(PPolylines.empty())
			return true;

		// clip given polylines filling LinesOut lines list

		for(pPPolyline = PPolylines.begin(); pPPolyline != PPolylines.end(); ++pPPolyline)
		{
			for(VertNum = 1; VertNum < (*pPPolyline)->Size(); ++VertNum)
			{
				LineToClip.SetStartPoint((*(*pPPolyline))[VertNum-1]);
				LineToClip.SetEndPoint((*(*pPPolyline))[VertNum]);

				switch(ClipLineByFilter(LineToClip, *pPFilter, TmpLinesList))
				{
				case CLIPPER_RC_ERROR	 :  return false;

				case CLIPPER_RC_DRAWALL	 :  LinesOut.push_back(LineToClip);
											break;

				case CLIPPER_RC_CLIP	 :  for(pLine = TmpLinesList.begin(); pLine != TmpLinesList.end(); ++pLine)
												LinesOut.push_back(*pLine);

				case CLIPPER_RC_DONTDRAW :  break;

				default					 :  ASSERT(0);
											return false;
				}

				TmpLinesList.clear();

			} // switch

		} // for( thru polylines

		FreePointersList(PPolylines);
		PPolylines.clear();

		if(LinesOut.empty())
			return true;

		// fill resulting polylines list using LinesOut lines list
		C3Point		StartPoint, EndPoint, PreviousEndPoint;
		PPolyLine	pPolylineOut;

		pLine = LinesOut.begin();

		if(pLine->GetEnds(StartPoint, EndPoint) != SUCCESS)
			return false;

		PreviousEndPoint = EndPoint;
		++pLine;

		if(!(pPolylineOut = new CPolyLine))
			return false;

		if( pPolylineOut->Add(StartPoint) != SUCCESS ||
			pPolylineOut->Add(EndPoint) != SUCCESS )
		{
			delete pPolylineOut;
			return false;
		}

		for( ; pLine != LinesOut.end(); ++pLine)
		{
			if(pLine->GetEnds(StartPoint, EndPoint) != SUCCESS)
			{
				delete pPolylineOut;
				return false;
			}

			if(PreviousEndPoint == StartPoint)
			{
				if(pPolylineOut->Add(EndPoint) != SUCCESS)
				{
					delete pPolylineOut;
					return false;
				}
			}
			else
			{
				ASSERT(VertNum);
				PPolylines.push_back(pPolylineOut);

				if(!(pPolylineOut = new CPolyLine))
					return false;

				if( pPolylineOut->Add(StartPoint) != SUCCESS ||
					pPolylineOut->Add(EndPoint) != SUCCESS )
				{
					delete pPolylineOut;
					return false;
				}
			}

			PreviousEndPoint = EndPoint;
		}

		PPolylines.push_back(pPolylineOut);

		LinesOut.clear();

	} // for( thru filters

	return true;
}


/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Clip given polygons.
 * Returns:	True in success and false in failure.
 ********************************************************************************/
bool
CClipper::ClipPolygons
(
 PPolylinesList&	PPolygons	// <=>
)const
{
	if(PPolygons.empty())
		return true;

	PPolylinesList				TmpPPolygonsList;
	PPolylinesList::iterator	pPPolygon, pCopyPPolygon, pEndPPolygon;
	short						RetCode;

	// be sure each polygon from the given list has no self-intersecting edges (otherwise split polygons)
	pEndPPolygon = PPolygons.end();
	--pEndPPolygon;
	for(pPPolygon = PPolygons.begin(); ; )
	{
		if((*pPPolygon)->Size() == 5)
		{
			CRealArray	Pars1, Pars2;
			C3Point		IntersectionPoint;
			PPolyLine	pNewPolygon = NULL;

			// 01/23 intersection
			Pars1.RemoveAll();
			Pars2.RemoveAll();
			if(Intersect(&CLine((**pPPolygon)[0], (**pPPolygon)[1]), &CLine((**pPPolygon)[2], (**pPPolygon)[3]), Pars1, Pars2, IC_ZRO) != SUCCESS)
				return false;
			ASSERT(Pars1.Size() == Pars2.Size());
			if(Pars1.Size() == 1)
			{
				(*pPPolygon)->Evaluate(Pars1[0], IntersectionPoint);

				if(!(pNewPolygon = new CPolyLine))
					return false;
				if( pNewPolygon->Add((**pPPolygon)[0]) != SUCCESS ||
					pNewPolygon->Add(IntersectionPoint) != SUCCESS ||
					pNewPolygon->Add((**pPPolygon)[3]) != SUCCESS ||
					pNewPolygon->Add((**pPPolygon)[0]) != SUCCESS )
					return false;
				PPolygons.push_back(pNewPolygon);

				if(!(pNewPolygon = new CPolyLine))
					return false;
				if( pNewPolygon->Add(IntersectionPoint) != SUCCESS ||
					pNewPolygon->Add((**pPPolygon)[2]) != SUCCESS ||
					pNewPolygon->Add((**pPPolygon)[1]) != SUCCESS ||
					pNewPolygon->Add(IntersectionPoint) != SUCCESS )
					return false;
				PPolygons.push_back(pNewPolygon);

				if(pPPolygon == pEndPPolygon)
				{
					PPolygons.erase(pPPolygon);
					break;
				}

				pPPolygon = PPolygons.erase(pPPolygon);
				continue;
			}

			// 03/12 intersection
			Pars1.RemoveAll();
			Pars2.RemoveAll();
			if(Intersect(&CLine((**pPPolygon)[0], (**pPPolygon)[3]), &CLine((**pPPolygon)[1], (**pPPolygon)[2]), Pars1, Pars2, IC_ZRO) != SUCCESS)
				return false;
			ASSERT(Pars1.Size() == Pars2.Size());
			if(Pars1.Size() == 1)
			{
				(*pPPolygon)->Evaluate(1.0 + Pars1[0], IntersectionPoint);

				if(!(pNewPolygon = new CPolyLine))
					return false;
				if( pNewPolygon->Add((**pPPolygon)[0]) != SUCCESS ||
					pNewPolygon->Add(IntersectionPoint) != SUCCESS ||
					pNewPolygon->Add((**pPPolygon)[1]) != SUCCESS ||
					pNewPolygon->Add((**pPPolygon)[0]) != SUCCESS )
					return false;
				PPolygons.push_back(pNewPolygon);

				if(!(pNewPolygon = new CPolyLine))
					return false;
				if( pNewPolygon->Add(IntersectionPoint) != SUCCESS ||
					pNewPolygon->Add((**pPPolygon)[2]) != SUCCESS ||
					pNewPolygon->Add((**pPPolygon)[3]) != SUCCESS ||
					pNewPolygon->Add(IntersectionPoint) != SUCCESS )
					return false;
				PPolygons.push_back(pNewPolygon);

				if(pPPolygon == pEndPPolygon)
				{
					PPolygons.erase(pPPolygon);
					break;
				}

				pPPolygon = PPolygons.erase(pPPolygon);
				continue;
			}
		}

		if(pPPolygon == pEndPPolygon)
			break;
		++pPPolygon;
	}

	// clip polygons
	for(PFiltersList::const_iterator pPFilter = m_PFiltersList.begin(); pPFilter != m_PFiltersList.end(); ++pPFilter)
	{
		if(PPolygons.empty())
			return true;

		pEndPPolygon = PPolygons.end();
		--pEndPPolygon;

		for(pPPolygon = PPolygons.begin(); ; pPPolygon = PPolygons.erase(pPPolygon))
		{
			switch(RetCode = ClipPolygonByFilter(**pPPolygon, *pPFilter, TmpPPolygonsList))
			{
			case CLIPPER_RC_ERROR	 :  FreePointersList(TmpPPolygonsList);
										return false;

			case CLIPPER_RC_DRAWALL	 :  PPolygons.push_back(*pPPolygon);
										break;

			case CLIPPER_RC_CLIP	 :  for(pCopyPPolygon = TmpPPolygonsList.begin(); pCopyPPolygon != TmpPPolygonsList.end(); ++pCopyPPolygon)
											PPolygons.push_back(*pCopyPPolygon);

			case CLIPPER_RC_DONTDRAW :  delete *pPPolygon;
										break;

			default					 :  ASSERT(0);
										FreePointersList(TmpPPolygonsList);
										return false;
			}

			if(RetCode != CLIPPER_RC_CLIP)
				FreePointersList(TmpPPolygonsList);

			TmpPPolygonsList.clear();

			if(pPPolygon == pEndPPolygon)
				break;
		}

		PPolygons.erase(pPPolygon);
	}

	return true;
}


/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Clip given line by given filter.
 *			Caller should use the resulting lines list only in CLIPPER_RC_CLIP
 *			return code case.
 * Returns:	CLIPPER_RC_xxx return codes.
 ********************************************************************************/
short
CClipper::ClipLineByFilter
(
 const CLine&	LineIn,		// =>
 const PFilter	pFilter,	// =>
 LinesList&		LinesOut	// <=
)const
{
	short		ReturnCode;
	CLine		LineToClip(LineIn);
	C3Point		GeoPoint1, GeoPoint2;
	CMatrix4	GrTransformation(pFilter->m_ClipperTransformation);
	CAffine		GeoTransformation;

	// I. transform given line to the filter's UCS
	if(!pFilter->m_bIdentityTransformation)
	{
		GeoTransformation.SetFromScratch(GrTransformation(0,0), GrTransformation(0,1), GrTransformation(0,2),
										 GrTransformation(1,0), GrTransformation(1,1), GrTransformation(1,2),
										 GrTransformation(2,0), GrTransformation(2,1), GrTransformation(2,2),
										 GrTransformation(0,3), GrTransformation(1,3), GrTransformation(2,3));

		if(LineToClip.Transform(GeoTransformation) != SUCCESS)
			return CLIPPER_RC_ERROR;
	}

	// II. clip the line by filter's clipping planes
	if((ReturnCode = pFilter->ClipLineByPlanes(LineToClip)) == CLIPPER_RC_ERROR)
		return CLIPPER_RC_ERROR;

	if(ReturnCode == CLIPPER_RC_DONTDRAW)
		return CLIPPER_RC_DONTDRAW;

	// III. clip the line by filter's contour
	// 1. check if the whole line lies outside of the clipping boundary comparing 2D extents
	CExtents		LineExtents;
	if(LineToClip.GetEnds(GeoPoint1, GeoPoint2) != SUCCESS)
		return CLIPPER_RC_ERROR;
	LineExtents.Update(GeoPoint1);
	LineExtents.Update(GeoPoint2);

	if(!pFilter->m_ContourExtents.Overlap(LineExtents))
		return CLIPPER_RC_DONTDRAW;

	// 2. intersect the line with contour
	CRealArray	LinePars, TmpLinePars, ContourPars;
	CLine		PolylineEdge;
	for(int i = 1; i < pFilter->m_ClippingPolyline.Size(); ++i)
	{
		PolylineEdge.SetStartPoint(pFilter->m_ClippingPolyline[i-1]);
		PolylineEdge.SetEndPoint(pFilter->m_ClippingPolyline[i]);
		if(Intersect(&LineToClip, &PolylineEdge, TmpLinePars, ContourPars, IC_ZRO) != SUCCESS)
			return CLIPPER_RC_ERROR;

		for(int j = TmpLinePars.Size(), NotUsedVar; j--; )
			if(LinePars.AddSort(TmpLinePars[j], 0, NotUsedVar) != SUCCESS)
				return CLIPPER_RC_ERROR;

		TmpLinePars.RemoveAll();
		ContourPars.RemoveAll();
	}

	if(!LinePars.Size())
	{
		// 3. check if the whole line lies outside or inside of the clipping boundary
		if(pFilter->m_ClippingPolyline.IsPointInside(GeoPoint1))
		{
			if(ReturnCode == CLIPPER_RC_DRAWALL)
				return CLIPPER_RC_DRAWALL;
			else
				LinesOut.push_back(LineToClip);
		}
		else
			return CLIPPER_RC_DONTDRAW;
	}
	else
	{
		// 4. add start and end pars if necessary
		if(fabs(LinePars[0]) > IC_ZRO  &&  LinePars.Insert(0.0, 0) != SUCCESS)
			return CLIPPER_RC_ERROR;
		if(fabs(LinePars.Last() - 1.0) > IC_ZRO  &&  LinePars.Add(1.0) != SUCCESS)
			return CLIPPER_RC_ERROR;

		ASSERT(LinePars.Size() > 1);

		// 5. find line's segments which lie inside of the contour
		if(LinePars.Size() == 2)
		{
			// one or both line's ends lie on the clipping boundary
			LineToClip.Evaluate(0.5, GeoPoint1);
			if(pFilter->m_ClippingPolyline.IsPointInside(GeoPoint1))
			{
				if(ReturnCode == CLIPPER_RC_DRAWALL)
					return CLIPPER_RC_DRAWALL;
				else
					LinesOut.push_back(LineToClip);
			}
			else
				return CLIPPER_RC_DONTDRAW;
		}
		else
		{
			// we have some intersection(s)
			bool	bExistInsideContour;
			bExistInsideContour = false;
			for(int SegmentNum = 1; SegmentNum < LinePars.Size(); ++SegmentNum)
			{
				LineToClip.Evaluate((LinePars[SegmentNum-1] + LinePars[SegmentNum]) / 2.0, GeoPoint1);
				if(pFilter->m_ClippingPolyline.IsPointInside(GeoPoint1))
				{
					LineToClip.Evaluate(LinePars[SegmentNum-1], GeoPoint1);
					LineToClip.Evaluate(LinePars[SegmentNum], GeoPoint2);
					LinesOut.push_back(CLine(GeoPoint1, GeoPoint2));
					bExistInsideContour = true;
				}
			}

			if(!bExistInsideContour)
				return CLIPPER_RC_DONTDRAW;
			else
				ReturnCode = CLIPPER_RC_CLIP;
		}
	}

	// IV. transform obtained line(s) (if any) back from the filter's UCS
	if(!pFilter->m_bIdentityTransformation && !LinesOut.empty())
	{
		GrTransformation = GrTransformation.invert();

		GeoTransformation.SetFromScratch(GrTransformation(0,0), GrTransformation(0,1), GrTransformation(0,2),
										 GrTransformation(1,0), GrTransformation(1,1), GrTransformation(1,2),
										 GrTransformation(2,0), GrTransformation(2,1), GrTransformation(2,2),
										 GrTransformation(0,3), GrTransformation(1,3), GrTransformation(2,3));

		for(LinesList::iterator pLine = LinesOut.begin(); pLine != LinesOut.end(); ++pLine)
			if(pLine->Transform(GeoTransformation) != SUCCESS)
				return CLIPPER_RC_ERROR;
	}

	return ReturnCode;
}


// There are two algorithms for the following method:
// 1. The Simple algorithm.
//  It just uses the CClipper::ClipLineByFilter method for clipping polygon's edges and then creates
//  resulting polygon with obtained (clipped) edges.
//  It is wrong for cases 1) when filter's vertices lie inside given polygon,
//						  2) when we should obtain more than one polygon,
//	 					  3) with clipping planes.
//  ... but it's more safe (and quick) for now.
// 2. The New algorithm.
//  Here are its steps for clipping by filter boundary:
//  - get all intersections between given polygon and filter boundary filling two special structures
//    (std::map) with GEO PolyLine parameters in intersection points (including vertices) as map keys
//    - one map for polygon's points and the second one - for boundary's points;
//    note, we should project boundary's point (i.e. calculate its Z coordinate which is always zero)
//    to the polygon's plane, so, I use 'bOnPolygon' field in the PointInfo structure for points with
//    correct Z coordinates;
//    two maps are linked by 'pIntersector' fields which are iterators pointing either to elements in
//    an opposite map if the point is a true intersection or to a following element of 'this' map if
//    the point is just a vertex and there are no intersections in it;
//  - combine resulting polygons from obtained segments getting those which lie inside opposite polygon,
//    i.e. get those polygon's segments which lie inside the filter boundary and get those boundary's
//    segments which lie inside the polygon.

//#define SIMPLE_ALGORITHM

#ifndef SIMPLE_ALGORITHM

// declarations & typedefs just for the new algorithm of the following method
struct PointInfo;
typedef std::map<double, PointInfo*>	PolygonPointsMAP;
typedef PolygonPointsMAP::iterator		MI;

void ProjectPointOntoPolygonPlane(C3Point& Point, const PPolyLine pPolygon);

struct PointInfo
{
	PointInfo::PointInfo() : bOnPolygon(true), bPassed(false)
	{}

	C3Point		Point;          // The coordinates of intersection
	bool		bOnPolygon;     // TRUE: The point is on the plane of polygon
	bool		bPassed;        // TRUE: The point has been tested to form new polygon
	MI			pIntersector;   // The interator to intersected polygon
};

template <class SomeKey, class PointerTo> inline void
FreePointersMap(std::map<SomeKey, PointerTo>& Map);

#endif // SIMPLE_ALGORITHM

/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Clip given polygon by given filter.
 *			Caller should use the resulting polygons list only in CLIPPER_RC_CLIP
 *			return code case.
 * Returns:	CLIPPER_RC_xxx return codes.
 ********************************************************************************/
short
CClipper::ClipPolygonByFilter
(
 const CPolyLine&	PolygonIn,		// =>
 const PFilter		pFilter,		// =>
 PPolylinesList&	PPolygonsOut	// <=
)const
{

#ifdef SIMPLE_ALGORITHM

// ------------------------ the SIMPLE ALGORITHM

	LinesList			LinesOut;
	LinesList::iterator	pLine;
	int					VertNum;
	bool				bDrawAll = true;

	// clip given polygon filling LinesOut lines list
	CLine		LineToClip;
	LinesList	TmpLinesList;

	for(VertNum = 1; VertNum < PolygonIn.Size(); ++VertNum)
	{
		LineToClip.SetStartPoint(PolygonIn[VertNum-1]);
		LineToClip.SetEndPoint(PolygonIn[VertNum]);

		switch(ClipLineByFilter(LineToClip, pFilter, TmpLinesList))
		{
		case CLIPPER_RC_ERROR	 :  return CLIPPER_RC_ERROR;

		case CLIPPER_RC_DRAWALL	 :  LinesOut.push_back(LineToClip);
									break;

		case CLIPPER_RC_CLIP	 :  for(pLine = TmpLinesList.begin(); pLine != TmpLinesList.end(); ++pLine)
										LinesOut.push_back(*pLine);
									bDrawAll = false;

		case CLIPPER_RC_DONTDRAW :  break;

		default					 :  ASSERT(0);
									return CLIPPER_RC_ERROR;
		}

		TmpLinesList.clear();
	}

	if(LinesOut.empty())
		return CLIPPER_RC_DONTDRAW;

	if(bDrawAll)
		return CLIPPER_RC_DRAWALL;

	// fill resulting polygons list using LinesOut lines list
	C3Point		StartPoint, EndPoint, PreviousEndPoint;
	PPolyLine	pPolygonOut;

	pLine = LinesOut.begin();

	if(pLine->GetEnds(StartPoint, EndPoint) != SUCCESS)
		return CLIPPER_RC_ERROR;

	PreviousEndPoint = EndPoint;
	++pLine;

	if(!(pPolygonOut = new CPolyLine))
		return CLIPPER_RC_ERROR;

	pPolygonOut->Add(StartPoint);
	pPolygonOut->Add(EndPoint);

	for( ; pLine != LinesOut.end(); ++pLine)
	{
		if(pLine->GetEnds(StartPoint, EndPoint) != SUCCESS)
			return CLIPPER_RC_ERROR;

		if(PreviousEndPoint != StartPoint)
			pPolygonOut->Add(StartPoint);
		pPolygonOut->Add(EndPoint);

		PreviousEndPoint = EndPoint;
	}

	pPolygonOut->Add((*pPolygonOut)[0]);

	PPolygonsOut.push_back(pPolygonOut);

	return CLIPPER_RC_CLIP;

// ------------------------ the end of the SIMPLE ALGORITHM

#else

// ------------------------ the NEW ALGORITHM

	short		ReturnCode;
	CMatrix4	GrTransformation(pFilter->m_ClipperTransformation);
	CAffine		GeoTransformation;
	RC			rc;
	CPolyLine	*pPolygonOut = NULL,
				PolygonToClip(PolygonIn, rc);
	CPolyLine&	FilterPolyline = pFilter->m_ClippingPolyline;
	CExtents	PolygonExtents;
	int			PolygonVertNum, FilterVertNum,
				ParNum, IntersectionsCount;
	CLine		PolygonEdge, FilterEdge;
	CRealArray	PolygonEdgePars, FilterEdgePars;

	PointInfo*			pPointToInsert;
	PolygonPointsMAP	OnPolygonPoints,
						OnFilterPoints;
	MI					pPolygonEdgePoint;
	std::pair<MI, bool>	InsertionResultPair;

	if(rc != SUCCESS)
		return CLIPPER_RC_ERROR;

	// I. transform given polygon to the filter's UCS
	if(!pFilter->m_bIdentityTransformation)
	{
		GeoTransformation.SetFromScratch(GrTransformation(0,0), GrTransformation(0,1), GrTransformation(0,2),
										 GrTransformation(1,0), GrTransformation(1,1), GrTransformation(1,2),
										 GrTransformation(2,0), GrTransformation(2,1), GrTransformation(2,2),
										 GrTransformation(0,3), GrTransformation(1,3), GrTransformation(2,3));

		if(GeoTransformation.ApplyToPointArray(PolygonToClip) != SUCCESS)
			return CLIPPER_RC_ERROR;
	}

	// II. clip the polygon by filter's clipping planes
	// assume the polygon is convex
	ReturnCode = CLIPPER_RC_DRAWALL;
	if(pFilter->m_ClipPlanes)
	{
		C3Point		Point1, Point2;
		CPolyLine	TmpPolygon;

		for(PolygonVertNum = PolygonToClip.Size() - 1; PolygonVertNum--; )
		{
			PolygonEdge.SetStartPoint(PolygonToClip[PolygonVertNum]);
			PolygonEdge.SetEndPoint(PolygonToClip[PolygonVertNum+1]);

			switch(pFilter->ClipLineByPlanes(PolygonEdge))
			{
			case CLIPPER_RC_DRAWALL	 :	if( TmpPolygon.Add(PolygonToClip[PolygonVertNum+1]) != SUCCESS ||
											TmpPolygon.Add(PolygonToClip[PolygonVertNum]) != SUCCESS )
											return CLIPPER_RC_ERROR;
										break;

			case CLIPPER_RC_CLIP	 :	if( PolygonEdge.GetEnds(Point1, Point2) != SUCCESS ||
											TmpPolygon.Add(Point2) != SUCCESS ||
											TmpPolygon.Add(Point1) != SUCCESS )
											return CLIPPER_RC_ERROR;

			case CLIPPER_RC_DONTDRAW :	ReturnCode = CLIPPER_RC_CLIP;
										break;

			case CLIPPER_RC_ERROR	 :
			default					 :	ASSERT(0);
										return CLIPPER_RC_ERROR;
			}
		}

		if(!TmpPolygon.Size())
			return CLIPPER_RC_DONTDRAW;

		if(ReturnCode == CLIPPER_RC_CLIP)
		{
			ASSERT(TmpPolygon.Size() > 1);

			PolygonToClip.RemoveAll();
			if(PolygonToClip.Add(TmpPolygon[TmpPolygon.Size()-1]) != SUCCESS)
				return CLIPPER_RC_ERROR;

			for(PolygonVertNum = TmpPolygon.Size() - 1; PolygonVertNum--; )
				if(TmpPolygon[PolygonVertNum] != TmpPolygon[PolygonVertNum+1])
					if(PolygonToClip.Add(TmpPolygon[PolygonVertNum]) != SUCCESS)
						return CLIPPER_RC_ERROR;

			if(PolygonToClip[0] != PolygonToClip[PolygonToClip.Size()-1])
				if(PolygonToClip.Add(PolygonToClip[0]) != SUCCESS)
					return CLIPPER_RC_ERROR;
		}
	}

	// III. clip obtained polygon by filter's contour
	// 1. check if the whole polygon lies outside of clipping boundary comparing 2D extents
	for(PolygonVertNum = PolygonToClip.Size(); PolygonVertNum--; )
		PolygonExtents.Update(PolygonToClip[PolygonVertNum]);

	if(!pFilter->m_ContourExtents.Overlap(PolygonExtents))
		return CLIPPER_RC_DONTDRAW;
    else {
        // If two extents only overlap at their boundaries, the polygon also should not
        // be drawn. SWH - Sept 9, 2004
        if (icadRealEqual(PolygonExtents.MaxX(), pFilter->m_ContourExtents.MinX()) ||
            icadRealEqual(PolygonExtents.MinX(), pFilter->m_ContourExtents.MaxX()) ||
            icadRealEqual(PolygonExtents.MaxY(), pFilter->m_ContourExtents.MinY()) ||
            icadRealEqual(PolygonExtents.MinY(), pFilter->m_ContourExtents.MaxY()))
            return CLIPPER_RC_DONTDRAW;
    }

	// 2. make sure both polygons are of the same orientation
	if(FilterPolyline.IsClockwiseProjection() != PolygonToClip.IsClockwiseProjection())
		if(PolygonToClip.Reverse() != SUCCESS)
			return CLIPPER_RC_ERROR;

	// 3. find all intersections
	for(PolygonVertNum = 0; PolygonVertNum < PolygonToClip.Size() - 1; ++PolygonVertNum)	// Don't revert the loop!
	{
		PolygonEdge.SetStartPoint(PolygonToClip[PolygonVertNum]);
		PolygonEdge.SetEndPoint(PolygonToClip[PolygonVertNum+1]);

		for(FilterVertNum = 0; FilterVertNum < FilterPolyline.Size() - 1; ++FilterVertNum)	// Don't revert the loop!
		{
			FilterEdge.SetStartPoint(FilterPolyline[FilterVertNum]);
			FilterEdge.SetEndPoint(FilterPolyline[FilterVertNum+1]);

			// intersect two edges
			if(Intersect(&PolygonEdge, &FilterEdge, PolygonEdgePars, FilterEdgePars, IC_ZRO) != SUCCESS)
			{
				ReturnCode = CLIPPER_RC_ERROR;
				goto out;
			}

			IntersectionsCount = PolygonEdgePars.Size();

			ASSERT(IntersectionsCount == FilterEdgePars.Size() && IntersectionsCount < 3);

			// insert intersection(s) points structs to appropriate maps binding these inserts
			for(ParNum = IntersectionsCount; ParNum--; )
			{
				// insert point struct into polygon's map
				if(!(pPointToInsert = new PointInfo))
				{
					ReturnCode = CLIPPER_RC_ERROR;
					goto out;
				}

                double param = PolygonEdgePars[ParNum];
				if(PolygonEdge.Evaluate(param, pPointToInsert->Point) != SUCCESS)
				{
					ReturnCode = CLIPPER_RC_ERROR;
					goto out;
				}

				InsertionResultPair = OnPolygonPoints.insert(std::make_pair(param + (double)PolygonVertNum, pPointToInsert));

                if (!InsertionResultPair.second) {
                    // The point has already been inserted. Igonre the second one!
					delete pPointToInsert;
					continue;
				}

				pPolygonEdgePoint = InsertionResultPair.first;  // The iterator pointing to the param-pPointToInsert pair.

				// insert point struct into filter's map
				if (!(pPointToInsert = new PointInfo))
				{
					ReturnCode = CLIPPER_RC_ERROR;
					goto out;
				}

                param = FilterEdgePars[ParNum];
				pPointToInsert->pIntersector = pPolygonEdgePoint;
				pPointToInsert->Point = pPolygonEdgePoint->second->Point;
				InsertionResultPair = OnFilterPoints.insert(std::make_pair(param, pPointToInsert));

				if (!InsertionResultPair.second) // interesting case: one point on the filter is an intersection with plygon in two or more places
				{
					delete pPointToInsert;
					delete pPolygonEdgePoint->second;    // delete pPointToInsert for the point on PolygonToClip
					OnPolygonPoints.erase(pPolygonEdgePoint);
					continue;
				}

				// set pIntersector field of inserted into polygon's map point struct
				pPolygonEdgePoint->second->pIntersector = InsertionResultPair.first;
			}

			PolygonEdgePars.RemoveAll();
			FilterEdgePars.RemoveAll();
		}
	}

	ASSERT(OnFilterPoints.empty() == OnPolygonPoints.empty());

	if(OnPolygonPoints.size() < 1)
	{
		// 4. check if the whole polygon lies outside or inside of the clipping boundary
		// or the whole clipping boundary lies inside of the polygon
		if(FilterPolyline.IsPointInside(PolygonToClip[0]))
		{
			if(ReturnCode == CLIPPER_RC_DRAWALL)
				goto out;
			else
			{
				if(!(pPolygonOut = new CPolyLine(PolygonToClip, rc)) || rc != SUCCESS)
					return CLIPPER_RC_ERROR;

				PPolygonsOut.push_back(pPolygonOut);
				pPolygonOut = NULL;
			}
		}
		else
			if(PolygonToClip.IsPointInside(FilterPolyline[0]))
			{
				if(!(pPolygonOut = new CPolyLine(FilterPolyline, rc)) || rc != SUCCESS)
					return CLIPPER_RC_ERROR;

				if(rc != SUCCESS)
					return CLIPPER_RC_ERROR;

				// set Z coordinates of filter's points to that they lie on the polygon's plane.
				for(FilterVertNum = pPolygonOut->Size(); FilterVertNum--; )
					ProjectPointOntoPolygonPlane((*pPolygonOut)[FilterVertNum], &PolygonToClip);

				PPolygonsOut.push_back(pPolygonOut);
				pPolygonOut = NULL;
				ReturnCode = CLIPPER_RC_CLIP;
			}
			else
				return CLIPPER_RC_DONTDRAW;
	}
	else
	{	// have intersections
		ReturnCode = CLIPPER_RC_CLIP;

		// 5. add vertices if necessary
		bool	bBindLastToFirst;
		// work with (clipped) polygon
		bBindLastToFirst = false;
		for(PolygonVertNum = PolygonToClip.Size(); PolygonVertNum--; )
		{
			InsertionResultPair = OnPolygonPoints.insert(std::make_pair((double)PolygonVertNum, (PointInfo*)NULL));
			if(InsertionResultPair.second)
			{
				if(!(pPointToInsert = new PointInfo))
				{
					ReturnCode = CLIPPER_RC_ERROR;
					goto out;
				}
				pPointToInsert->Point = PolygonToClip[PolygonVertNum];
				if(PolygonVertNum == PolygonToClip.Size() - 1)
					bBindLastToFirst = true;
				else
				{
					pPolygonEdgePoint = InsertionResultPair.first;
					++pPolygonEdgePoint;
					pPointToInsert->pIntersector = pPolygonEdgePoint;
				}

				InsertionResultPair.first->second = pPointToInsert;
			}
		}
		if(bBindLastToFirst)
		{
			pPolygonEdgePoint = OnPolygonPoints.end();
			(--pPolygonEdgePoint)->second->pIntersector = OnPolygonPoints.begin();
		}

		// work with filter's polygon (clipping polygon)
		bBindLastToFirst = false;
		for(FilterVertNum = FilterPolyline.Size(); FilterVertNum--; )
		{
			InsertionResultPair = OnFilterPoints.insert(std::make_pair((double)FilterVertNum, (PointInfo*)NULL));
			if(InsertionResultPair.second)
			{
				if(!(pPointToInsert = new PointInfo))
				{
					ReturnCode = CLIPPER_RC_ERROR;
					goto out;
				}
				pPointToInsert->Point = FilterPolyline[FilterVertNum];
				if(FilterVertNum == FilterPolyline.Size() - 1)
					bBindLastToFirst = true;
				else
				{
					pPolygonEdgePoint = InsertionResultPair.first;
					++pPolygonEdgePoint;
					pPointToInsert->pIntersector = pPolygonEdgePoint;
				}
				pPointToInsert->bOnPolygon = false;

				InsertionResultPair.first->second = pPointToInsert;
			}
		}
		if(bBindLastToFirst)
		{
			pPolygonEdgePoint = OnFilterPoints.end();
			(--pPolygonEdgePoint)->second->pIntersector = OnFilterPoints.begin();
		}

		// 6. create final polygon(s)
		MI			pPolygonWalker, pFilterWalker, pCurPoint, pPolygonEndPoint, pFilterEndPoint;
		bool		bCurrentlyOnPolygon;
		C3Point		Point, FirstPoint;
		double		StartPar, EndPar;

		pPolygonEndPoint = OnPolygonPoints.end();
		--pPolygonEndPoint;
		pFilterEndPoint = OnFilterPoints.end();
		--pFilterEndPoint;

		// find the first point of a final polygon
		bool	bFoundPoint;
		bFoundPoint = false;
		for(pPolygonWalker = OnPolygonPoints.begin(); pPolygonWalker != pPolygonEndPoint; ++pPolygonWalker)
		{
			pPolygonWalker->second->bPassed = true;
			StartPar = (pPolygonWalker++)->first;
			EndPar = (pPolygonWalker--)->first;
			PolygonToClip.Evaluate((StartPar + EndPar) * 0.5, FirstPoint);
			if(FilterPolyline.IsPointInside(FirstPoint))
			{
				pCurPoint = pPolygonWalker;
				if(pPolygonWalker->second->pIntersector != ++pCurPoint)
					pPolygonWalker->second->pIntersector->second->bPassed = true;

				FirstPoint = (pPolygonWalker++)->second->Point;
				bCurrentlyOnPolygon = bFoundPoint = true;
				pCurPoint = pPolygonWalker;
				break;
			}
		}

		pFilterWalker = OnFilterPoints.begin();
		if(!bFoundPoint)
		{
			for( ; pFilterWalker != pFilterEndPoint; ++pFilterWalker)
			{
				pFilterWalker->second->bPassed = true;
				StartPar = (pFilterWalker++)->first;
				EndPar = (pFilterWalker--)->first;
				FilterPolyline.Evaluate((StartPar + EndPar) * 0.5, FirstPoint);
				if(PolygonToClip.IsPointInside(FirstPoint))
				{
					pCurPoint = pFilterWalker;
					if(pFilterWalker->second->pIntersector != ++pCurPoint)
						pFilterWalker->second->pIntersector->second->bPassed = true;

					FirstPoint = pFilterWalker->second->Point;
					if(!(pFilterWalker++)->second->bOnPolygon)
						ProjectPointOntoPolygonPlane(FirstPoint, &PolygonToClip);
					bFoundPoint = true;
					bCurrentlyOnPolygon = false;
					pCurPoint = pFilterWalker;
					break;
				}
			}
		}

		ASSERT(!bFoundPoint || (bFoundPoint && (pPolygonWalker != pPolygonEndPoint || pFilterWalker != pFilterEndPoint)));
		if(!bFoundPoint || (pPolygonWalker == pPolygonEndPoint && pFilterWalker == pFilterEndPoint))
		{
			ReturnCode = CLIPPER_RC_DONTDRAW;	// CLIPPER_RC_ERROR ???
			goto out;
		}

		// create polygons
		if(!(pPolygonOut = new CPolyLine))
		{
			ReturnCode = CLIPPER_RC_ERROR;
			goto out;
		}

		if(pPolygonOut->Add(FirstPoint) != SUCCESS)
		{
			ReturnCode = CLIPPER_RC_ERROR;
			goto out;
		}
		if(!pCurPoint->second->bOnPolygon)
			ProjectPointOntoPolygonPlane(pCurPoint->second->Point, &PolygonToClip);
		if(pPolygonOut->Add(pCurPoint->second->Point) != SUCCESS)
		{
			ReturnCode = CLIPPER_RC_ERROR;
			goto out;
		}

		ASSERT(pPolygonWalker != pPolygonEndPoint || pFilterWalker != pFilterEndPoint);
		if(pPolygonWalker == pPolygonEndPoint && pFilterWalker == pFilterEndPoint)
		{
			ReturnCode = CLIPPER_RC_DONTDRAW;	// CLIPPER_RC_ERROR ???
			delete pPolygonOut;
			goto out;
		}

		if(pPolygonWalker == pPolygonEndPoint)
		{
			bCurrentlyOnPolygon = false;
			pCurPoint = pFilterWalker;
		}

		// main loop
		bool	bSwitched;
		bSwitched = false;
		MI		pTmpIter;
		for( ; pPolygonWalker != pPolygonEndPoint || pFilterWalker != pFilterEndPoint; )
		{
			if(pCurPoint->second->bPassed)
			{
				if (pPolygonOut) {
					ASSERT(0);
					ReturnCode = CLIPPER_RC_ERROR;
					goto out;
				}
				if(bCurrentlyOnPolygon)
				{
					if(++pPolygonWalker == pPolygonEndPoint)
					{
						bCurrentlyOnPolygon = false;
						pCurPoint = pFilterWalker;
					}
					else
						pCurPoint = pPolygonWalker;
				}
				else
				{
					if(++pFilterWalker == pFilterEndPoint)
					{
						bCurrentlyOnPolygon = true;
						pCurPoint = pPolygonWalker;
					}
					else
						pCurPoint = pFilterWalker;
				}

				continue;
			}

			if(pCurPoint == pPolygonEndPoint)
			{
				pCurPoint->second->bPassed = true;

				if(pPolygonWalker == pPolygonEndPoint)
				{
					pCurPoint = pFilterWalker;
					bCurrentlyOnPolygon = false;
				}
				else
				{
					pCurPoint = pCurPoint->second->pIntersector;
					if(pCurPoint != OnPolygonPoints.begin())
						bCurrentlyOnPolygon = false;
				}
				continue;
			}

			if(pCurPoint == pFilterEndPoint)
			{
				pCurPoint->second->bPassed = true;

				if(pFilterWalker == pFilterEndPoint)
				{
					pCurPoint = pPolygonWalker;
					bCurrentlyOnPolygon = true;
				}
				else
				{
					pCurPoint = pCurPoint->second->pIntersector;
					if(pCurPoint != OnFilterPoints.begin())
						bCurrentlyOnPolygon = true;
				}
				continue;
			}

			StartPar = (pCurPoint++)->first;
			EndPar = (pCurPoint--)->first;
			pCurPoint->second->bPassed = true;
			if(bCurrentlyOnPolygon)
			{
				PolygonToClip.Evaluate((StartPar + EndPar) * 0.5, Point);
				if(FilterPolyline.IsPointInside(Point))
				{
					if(pPolygonOut)
					{
						Point = (++pCurPoint)->second->Point;
						if(pPolygonOut->Add(Point) != SUCCESS)
						{
							ReturnCode = CLIPPER_RC_ERROR;
							goto out;
						}
						if(Point == FirstPoint)
						{
							PPolygonsOut.push_back(pPolygonOut);
							pPolygonOut = NULL;
							pCurPoint = pPolygonWalker;
							bSwitched = false;
							continue;
						}
					}
					else
					{
						pTmpIter = pCurPoint;
						if(pCurPoint->second->pIntersector != ++pTmpIter)
							pCurPoint->second->pIntersector->second->bPassed = true;

						if(!(pPolygonOut = new CPolyLine))
						{
							ReturnCode = CLIPPER_RC_ERROR;
							goto out;
						}
						FirstPoint = pCurPoint->second->Point;
						if( pPolygonOut->Add(FirstPoint) != SUCCESS ||
							pPolygonOut->Add((++pCurPoint)->second->Point) != SUCCESS )
						{
							ReturnCode = CLIPPER_RC_ERROR;
							goto out;
						}
						bSwitched = false;
					}

					if(!bSwitched)
					{
						++pPolygonWalker;
						ASSERT(pPolygonWalker == pCurPoint);
						if(pPolygonWalker == pPolygonEndPoint)
						{
							bCurrentlyOnPolygon = false;	// note:
							pCurPoint = pFilterWalker;		// not switching in 'bSwithed' meaning
						}
					}
				}
				else
				{	// next segment isn't a finalist
					if(pPolygonOut)
					{
#ifdef _DEBUG
						pTmpIter = pCurPoint;
						ASSERT(pCurPoint->second->pIntersector != ++pTmpIter);
#endif
						bSwitched = true;
						bCurrentlyOnPolygon = false;
						pCurPoint = pCurPoint->second->pIntersector;
					}
					else
					{
						ASSERT(!bSwitched);
						pPolygonWalker = ++pCurPoint;
					}
				}
			}
			else
			{	// currently on filter
				FilterPolyline.Evaluate((StartPar + EndPar) * 0.5, Point);
				if(PolygonToClip.IsPointInside(Point))
				{
					if(pPolygonOut)
					{
						Point = (++pCurPoint)->second->Point;
						if(!pCurPoint->second->bOnPolygon)
							ProjectPointOntoPolygonPlane(Point, &PolygonToClip);

						if(pPolygonOut->Add(Point) != SUCCESS)
						{
							ReturnCode = CLIPPER_RC_ERROR;
							goto out;
						}
						if(Point == FirstPoint)
						{
							PPolygonsOut.push_back(pPolygonOut);
							pPolygonOut = NULL;
							pCurPoint = pFilterWalker;
							bSwitched = false;
							continue;
						}
					}
					else
					{
						pTmpIter = pCurPoint;
						if(pCurPoint->second->pIntersector != ++pTmpIter)
							pCurPoint->second->pIntersector->second->bPassed = true;

						if(!(pPolygonOut = new CPolyLine))
						{
							ReturnCode = CLIPPER_RC_ERROR;
							goto out;
						}
						FirstPoint = pCurPoint->second->Point;
						if(!pCurPoint->second->bOnPolygon)
							ProjectPointOntoPolygonPlane(FirstPoint, &PolygonToClip);
						if(pPolygonOut->Add(FirstPoint) != SUCCESS)
						{
							ReturnCode = CLIPPER_RC_ERROR;
							goto out;
						}

						Point = (++pCurPoint)->second->Point;
						if(!pCurPoint->second->bOnPolygon)
							ProjectPointOntoPolygonPlane(Point, &PolygonToClip);
						if(pPolygonOut->Add(Point) != SUCCESS)
						{
							ReturnCode = CLIPPER_RC_ERROR;
							goto out;
						}
						bSwitched = false;
					}

					if(!bSwitched)
					{
						++pFilterWalker;
						ASSERT(pFilterWalker == pCurPoint);
						if(pFilterWalker == pFilterEndPoint)
						{
							bCurrentlyOnPolygon = true;		// note:
							pCurPoint = pPolygonWalker;		// not switching in 'bSwithed' meaning
						}
					}
				}
				else
				{	// next segment isn't a finalist
					if(pPolygonOut) {
#ifdef _DEBUG
						pTmpIter = pCurPoint;
						if (pCurPoint->second->pIntersector == ++pTmpIter) {
							ASSERT(0);
							ReturnCode = CLIPPER_RC_ERROR;
							goto out;
						}
#endif
						bSwitched = true;
						bCurrentlyOnPolygon = true;
						pCurPoint = pCurPoint->second->pIntersector;
					}
					else
					{
						ASSERT(!bSwitched);
						pFilterWalker = ++pCurPoint;
					}
				}
			}	// currently on filter
		}	// main loop
	}	// have intersections

	// IV. transform obtained polygon(s) (if any) back from the filter's UCS
	if(!pFilter->m_bIdentityTransformation && !PPolygonsOut.empty())
	{
		GrTransformation = GrTransformation.invert();

		GeoTransformation.SetFromScratch(GrTransformation(0,0), GrTransformation(0,1), GrTransformation(0,2),
										 GrTransformation(1,0), GrTransformation(1,1), GrTransformation(1,2),
										 GrTransformation(2,0), GrTransformation(2,1), GrTransformation(2,2),
										 GrTransformation(0,3), GrTransformation(1,3), GrTransformation(2,3));

		for(PPolylinesList::iterator pPPolygon = PPolygonsOut.begin(); pPPolygon != PPolygonsOut.end(); ++pPPolygon)
			if((*pPPolygon)->Transform(GeoTransformation) != SUCCESS)
			{
				ReturnCode = CLIPPER_RC_ERROR;
				goto out;
			}
	}

out:

	FreePointersMap(OnPolygonPoints);
	FreePointersMap(OnFilterPoints);

	ASSERT(!pPolygonOut);
	delete pPolygonOut;

	return ReturnCode;

#endif	// SIMPLE_ALGORITHM

}



/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Compare given entity's bounding box with our clipping volume to decide
 *			if we should try to clip the entity or we shouldn't draw the entity
 *			at all or we should draw whole entity without clipping.
 * Returns: CLIPPER_RC_xxx return codes.
 ********************************************************************************/
short
CClipper::CheckBoundingBox
(
 const sds_point	MinPoint,	// =>
 const sds_point	MaxPoint	// =>
)const
{
	short		ReturnCode = CLIPPER_RC_DRAWALL;
	sds_point	BBPoints[8] = { (MinPoint[0], MinPoint[1], MinPoint[2]),
								(MinPoint[0], MaxPoint[1], MinPoint[2]),
								(MaxPoint[0], MaxPoint[1], MinPoint[2]),
								(MaxPoint[0], MinPoint[1], MinPoint[2]),
								(MinPoint[0], MinPoint[1], MaxPoint[2]),
								(MinPoint[0], MaxPoint[1], MaxPoint[2]),
								(MaxPoint[0], MaxPoint[1], MaxPoint[2]),
								(MaxPoint[0], MinPoint[1], MaxPoint[2]) };
	int			i;

	for(PFiltersList::const_iterator pFilter = m_PFiltersList.begin(); pFilter != m_PFiltersList.end(); ++pFilter)
	{
		double			LocalMinZ, LocalMaxZ;
		CExtents		LocalBBExtents;

		LocalMinZ = DBL_MAX;
		LocalMaxZ = - DBL_MAX;
		LocalBBExtents.Reset();


		if((*pFilter)->m_bIdentityTransformation)
			for(i = 8; i--; )
			{
				if(BBPoints[i][2] < LocalMinZ)
					LocalMinZ = BBPoints[i][2];
				if(BBPoints[i][2] > LocalMaxZ)
					LocalMaxZ = BBPoints[i][2];

				LocalBBExtents.Update(C3Point(BBPoints[i][0], BBPoints[i][1], BBPoints[i][2]));
			}
		else
			for(i = 8; i--; )
			{
				sds_point	LocalBBPoint;
				(*pFilter)->m_ClipperTransformation.multiplyRight(LocalBBPoint, BBPoints[i]);

				if(LocalBBPoint[2] < LocalMinZ)
					LocalMinZ = LocalBBPoint[2];
				if(LocalBBPoint[2] > LocalMaxZ)
					LocalMaxZ = LocalBBPoint[2];

				LocalBBExtents.Update(C3Point(LocalBBPoint[0], LocalBBPoint[1], LocalBBPoint[2]));
			}

		// compare planes
		if((*pFilter)->m_FrontPlaneDistance < LocalMinZ || (*pFilter)->m_BackPlaneDistance > LocalMaxZ)
			return CLIPPER_RC_DONTDRAW;

		if((*pFilter)->m_BackPlaneDistance > LocalMinZ || (*pFilter)->m_FrontPlaneDistance < LocalMaxZ)
			ReturnCode = CLIPPER_RC_CLIP;

		// compare 2D extents
		if(!(*pFilter)->m_ContourExtents.Overlap(LocalBBExtents))
			return CLIPPER_RC_DONTDRAW;

		if(!(*pFilter)->m_ContourExtents.Contain(LocalBBExtents))
			ReturnCode = CLIPPER_RC_CLIP;
	}

	return ReturnCode;
}


//
// ====================================== HELPERS =======================================
//

/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Free allocated memory in the given list of pointers.
 *			DON'T confuse with std::list::clear()!
 * Returns:	None.
 ********************************************************************************/
template <class PointerTo> inline void
FreePointersList
(
 std::list<PointerTo>&	List	// <=>
)
{
	for(std::list<PointerTo>::iterator pPItem = List.begin(); pPItem != List.end(); ++pPItem)
		delete *pPItem;
}


#ifndef SIMPLE_ALGORITHM

/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Free allocated memory in the given map of pointers.
 *			DON'T confuse with std::map::clear()!
 * Returns:	None.
 ********************************************************************************/
template <class SomeKey, class PointerTo> inline void
FreePointersMap
(
 std::map<SomeKey, PointerTo>&	Map	// <=>
)
{
	for(std::map<SomeKey, PointerTo>::iterator pPItem = Map.begin(); pPItem != Map.end(); ++pPItem)
		delete pPItem->second;
}

/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Get a Z coordinate of the given point by its X and Y coordinates
 *			assuming the point lies on a plane of the given poygon.
 * Returns:	None.
 ********************************************************************************/
void
ProjectPointOntoPolygonPlane
(
 C3Point&			Point,		// <=>
 const PPolyLine	pPolygon	// =>
)
{
	//
	// The equation of the plane passing thru three points (x1, y1, z1), (x2, y2, z2) & (x3, y3, z3)
	// which don't lie on the same line:
	// a * x + b * y + c * z - d = 0,
	// where
	//     |y1 z1 1|       |z1 x1 1|       |x1 y1 1|       |x1 y1 z1|
	// a = |y2 z2 1|,  b = |z2 x2 1|,  c = |x2 y2 1|,  d = |x2 y2 z2|.
	//     |y3 z3 1|       |z3 x3 1|       |x3 y3 1|       |x3 y3 z3|
	// These points lie on the same line if and only if
	// a = b = c = 0.
	//

	// these static vars are for that to don't compute a, b, c & d again for the same polygon
	// (I use also CurrentSize & CurrentPoint0 for safety (it can be that pCurrentPolygon == pPolygon, but
	// these polygons aren't the same), although it's don't avoid the unsafety completely)
	static PPolyLine	pCurrentPolygon = NULL;
	static int			CurrentSize = 0;
	static C3Point		CurrentPoint0;
	static double		a, b, c, d;

	if(pCurrentPolygon != pPolygon || (pPolygon && pPolygon->Size() != CurrentSize) || ((pPolygon && (*pPolygon)[0] != CurrentPoint0)))
	{
		if(!pPolygon || pPolygon->Size() < 3)
		{
			a = b = d = 0.0;
			c = 1.0;
		}
		else
		{
			int		i1 = 0, i2 = 1, i3 = 2,
					Size = pPolygon->Size();
			CPolyLine&	Polygon = *pPolygon;
			for( ; i3 < Size; ++i1, ++i2, ++i3)
			{
				a = Polygon[i1].Y() * Polygon[i2].Z() + Polygon[i2].Y() * Polygon[i3].Z() + Polygon[i3].Y() * Polygon[i1].Z() -
					Polygon[i1].Y() * Polygon[i3].Z() - Polygon[i2].Y() * Polygon[i1].Z() - Polygon[i3].Y() * Polygon[i2].Z();
				b = Polygon[i1].Z() * Polygon[i2].X() + Polygon[i2].Z() * Polygon[i3].X() + Polygon[i3].Z() * Polygon[i1].X() -
					Polygon[i1].Z() * Polygon[i3].X() - Polygon[i2].Z() * Polygon[i1].X() - Polygon[i3].Z() * Polygon[i2].X();
				c = Polygon[i1].X() * Polygon[i2].Y() + Polygon[i2].X() * Polygon[i3].Y() + Polygon[i3].X() * Polygon[i1].Y() -
					Polygon[i1].X() * Polygon[i3].Y() - Polygon[i2].X() * Polygon[i1].Y() - Polygon[i3].X() * Polygon[i2].Y();
				if(fabs(a) < IC_ZRO  &&  fabs(b) < IC_ZRO  &&  fabs(c) < IC_ZRO)
				{
					a = b = d = 0.0;
					c = 1.0;
				}
				else
				{
					d = Polygon[i1].X() * Polygon[i2].Y() * Polygon[i3].Z() + Polygon[i2].X() * Polygon[i3].Y() * Polygon[i1].Z() +
						Polygon[i3].X() * Polygon[i1].Y() * Polygon[i2].Z() - Polygon[i1].X() * Polygon[i3].Y() * Polygon[i2].Z() -
						Polygon[i2].X() * Polygon[i1].Y() * Polygon[i3].Z() - Polygon[i3].X() * Polygon[i2].Y() * Polygon[i1].Z();
					break;
				}
			}
		}
		pCurrentPolygon = pPolygon;
		if(pPolygon)
		{
			CurrentSize = pPolygon->Size();
			CurrentPoint0 = (*pPolygon)[0];
		}
	}

	Point.SetZ((d - a * Point.X() - b * Point.Y()) / c);
}

#endif
