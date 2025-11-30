/* D:\ICAD2000\PRJ\LIB\GR\TRUETYPEUTILS.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 *
 *
 * Utilities needed by GR for drawing TrueType text.
 *
 */

//
// STRUCTURE OF THIS FILE :
// - NOT USED (OLD) SECTION
//	 - local utils
//	 - common utils
// - ACTUAL SECTION
//	 - local utils
//	 - common utils
//

/*D.G.*/
// Let's speak about TTF text output in IntelliCAD. Here are 3 ways of this output implemented in IntelliCAD more or less:
// 1. Draw text using Windows GDI TextOut function. This is the old way (it worked till version 2.1.0012 of September, 2000).
//	  Its advantages: quickness and simpleness (just create an appropriate font and TextOut a text with it).
//	  Disadvantages: using different from other entities way for creating display objects (additional CTrueTypeDisplayObject-type
//	  field in the gr_displayobject structure); creating GDI font in two places: in GR and in ICAD subprojects; and of course
//	  we cannot scale TTF text exactly by GDI font mapper and we cannot rotate it in 3D and we cannot mirror it
//	  and... that's enough, I think.
// The following two ways are new, they consist of calling GetGlyphOutline Windows GDI function for obtaining contours
// of symbols, transform these contours, draw them with Polygon or PolyBezier GDI function (it depends on a way) and
// fill them as a GDI path (like as solid hatches).
// In these ways we don't need the CTrueTypeDisplayObject field in the gr_displayobject structure, we can transform a text
// in any way, if only a text box must be drawn instead of a text itself we create display objects just as for polylines.
// 2. Convert symbols contours given by GetGlyphOutline into sequences of line segments approximating quadratic B-splines
//	  segments and draw & fill them exactly like as solid hatches boundaries using Polygon GDI function.
// 3. Convert symbols contours given by GetGlyphOutline into sequences of 3-order Bezier curves, draw them using PolyBezier
//	  GDI function and fill them as GDI paths. In this way we have exact representation of all segments, but also this way has
//	  a little disadvantage: we should add some methods for outputting Beziers in ICAD. The more serious its disadvantage is
//	  that display lists are too big because they contain surplus data (especially for strait parts of glyphs contours).
// There are 2 another advantages of the 3rd way against the 2nd one: an linearization of contours can be invalid in the
// following cases:
// - after changing current viewsize by some zooming - and we should regenerate display lists
//	 (note, if we use the 3rd way then GDI draws Beziers smoothly always);
// - if we have TTF text which is non-complanar to the XY plane - in this case the linearization can have surplus points;
// The other hand, the 2nd way better than the 3rd for inserts clipping.
// So, now we use the 2nd way if the current gr_view has non-empty clipper and the 3rd one otherwise.

#pragma warning (disable : 4786)

#include "gr.h"
#include "TrueTypeUtils.h"
//#include "DoFont.h"
#include "styletabrec.h"
#include "textgen.h"
//#include "WinVer.h"
#include "gr_parsetext.h"
#include "glyph.h"
#include "nurbs.h"
#include "TTFInfoGetter.h"
#include "polycache.h"

#ifndef NEW_TTF_OUTPUT_METHOD
 #include "TrueTypeDispObjs.h"
#endif


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern gr_view gr_defview;

//
// ---------------------------------- local utils ----------------------------------
//

/********************************************************************************
 * Author:	Vitaly Spirin.
 * Purpose:	.
 * Returns:	Size of buffer or GDI_ERROR in failure.
 ********************************************************************************/
DWORD
_GetGlyphOutline(				// R: required buffer size or > 0
 HDC			hdc,			// I: handle to device context
 TTF_table*		pTable,		// I: class to get glyph index
 UINT			uChar,			// I: character to query
 UINT			uFormat,		// I: format of data to return
 LPGLYPHMETRICS lpgm,			// I: pointer to structure for metrics
 DWORD			cbBuffer,		// I: size of buffer for data
 LPVOID			lpvBuffer,		// I: pointer to buffer for data
 CONST			MAT2 *lpmat2	// I: pointer to transformation matrix structure
)
{
	DWORD status;
	if(pTable != NULL)
		status = GetGlyphOutline(hdc, UINT(pTable->FindGlyph(uChar)), uFormat | GGO_GLYPH_INDEX, lpgm,
			cbBuffer, lpvBuffer, lpmat2);
	else
		status = GetGlyphOutline(hdc, uChar, uFormat, lpgm,
			cbBuffer, lpvBuffer, lpmat2);
	if(status == GDI_ERROR)
		status = GetGlyphOutline(hdc, 0, uFormat|GGO_GLYPH_INDEX, lpgm,
		cbBuffer, lpvBuffer, lpmat2);

	return status;
}

/********************************************************************************
 * Author:	Vitaly Spirin.
 * Purpose:	.
 * Returns:	.
 ********************************************************************************/
/*HFONT
_CreateFontIndirect
(							// R: handle to a logical font or NULL
 CONST LOGFONT*    lplf 	// I: pointer to logical font structure
)
{
	if (GetWinVer() == VER_PLATFORM_WIN32_NT)
	{
		LOGFONTW LogfontW;
		LogfontW.lfHeight = lplf->lfHeight;
		LogfontW.lfWidth = lplf->lfWidth;
		LogfontW.lfEscapement = lplf->lfEscapement;
		LogfontW.lfOrientation = lplf->lfOrientation;
		LogfontW.lfWeight = lplf->lfWeight;
		LogfontW.lfItalic = lplf->lfItalic;
		LogfontW.lfUnderline = lplf->lfUnderline;
		LogfontW.lfStrikeOut = lplf->lfStrikeOut;
		LogfontW.lfCharSet = lplf->lfCharSet;
		LogfontW.lfOutPrecision	= lplf->lfOutPrecision;
		LogfontW.lfClipPrecision = lplf->lfClipPrecision;
		LogfontW.lfQuality = lplf->lfQuality;
		LogfontW.lfPitchAndFamily= lplf->lfPitchAndFamily;
		if(MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, lplf->lfFaceName, -1, LogfontW.lfFaceName, sizeof(LogfontW.lfFaceName)) == 0)
			return NULL;
		return CreateFontIndirectW(&LogfontW);
	}
	else
		return CreateFontIndirectA(lplf);
}
*/

/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Get outline text metrics allocating an appropriate buffer.
 * Returns:	Pointer to allocated metrics buffer in success and NULL in failure.
 ********************************************************************************/
LPOUTLINETEXTMETRIC
ttfGetOutlineTextMetrics
(
 CDC*	pDC		// =>
)
{
	ASSERT(pDC);
	if(!pDC)
		return NULL;

	UINT				BufSize;
	LPOUTLINETEXTMETRIC	pMetrics;

	if(!(BufSize = ::GetOutlineTextMetrics(pDC->GetSafeHdc(), 0, NULL)))
		return NULL;

	ASSERT(BufSize >= sizeof(OUTLINETEXTMETRIC));
	if(!(pMetrics = (LPOUTLINETEXTMETRIC)new BYTE[BufSize]))
		return NULL;

	if(!(::GetOutlineTextMetrics(pDC->GetSafeHdc(), BufSize, pMetrics)))
	{
		delete pMetrics;
		return NULL;
	}

	return pMetrics;
}

/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Calculate bounding box of TTF text string.
 * Returns:	None.
 ********************************************************************************/
void
ttfGetBoundingBox
(
 wchar_t				w_Text[],		// =>
 bool					bHorizontal,	// =>
 sds_point&				Scales,			// =>
 LPOUTLINETEXTMETRIC	pMetrics,		// =>
 CDC*					pDC,			// =>
 sds_point*				BoxPoints		// <=
)
{
	ASSERT(pMetrics && BoxPoints && Scales && pDC && !(wcslen(w_Text) == 0));
	if(!pMetrics || !BoxPoints || !Scales || !pDC || (wcslen(w_Text) == 0))
		return;

	SIZE		BoundingSize;
	sds_point	TmpMinPoint, TmpMaxPoint;

	if(bHorizontal)
	{
		if(!(GetTextExtentPoint32W(pDC->GetSafeHdc(), w_Text, wcslen(w_Text), &BoundingSize)))
			ASSERT(false);
	}
	else
	{
		BoundingSize.cx = pMetrics->otmTextMetrics.tmMaxCharWidth;		// TO DO: calculate an exact width.
		BoundingSize.cy = pMetrics->otmTextMetrics.tmHeight * wcslen(w_Text);
	}

	TmpMinPoint[0] = TmpMinPoint[1] = TmpMinPoint[2] = 0.0;
	TmpMaxPoint[0] = BoundingSize.cx * Scales[0];
	TmpMaxPoint[1] = BoundingSize.cy * Scales[1];
	TmpMaxPoint[2] = 0.0;

	GR_PTCPY(BoxPoints[0], TmpMinPoint);
	GR_PTCPY(BoxPoints[1], TmpMaxPoint);
}

/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Transform the given point from TTF CS (given by GetGlyphOutline GDI API) to UCS.
 * Returns:	None.
 ********************************************************************************/
void
ttfCStoUCS
(
 POINT&			NextInsertPoint,	// =>
 double			TanObl,				// =>
 sds_point&		Scales,				// =>
 double			CosAng,				// =>
 double			SinAng,				// =>
 sds_point&		InsertPoint,		// =>
 gr_view*		pGrView,			// =>
 RealPoint3D&	Point				// <=>	// assumes that input Point.z == 0.0
)
{
	ASSERT(Scales && InsertPoint);
	if(!Scales || !InsertPoint)
		return;

	sds_point	TmpPoint1, TmpPoint2;

	Point.x += NextInsertPoint.x;
	Point.y += NextInsertPoint.y;

	Point.x += Point.y * TanObl;

	Point.x *= Scales[0];
	Point.y *= Scales[1];

	TmpPoint1[0] = Point.x * CosAng - Point.y * SinAng + InsertPoint[0];
	TmpPoint1[1] = Point.x * SinAng + Point.y * CosAng + InsertPoint[1];
	TmpPoint1[2] = InsertPoint[2];

	gr_ncs2ucs(TmpPoint1, TmpPoint2, 0, pGrView);

	Point.x = TmpPoint2[0];
	Point.y = TmpPoint2[1];
	Point.z = TmpPoint2[2];
}

/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Get glyph outlines data in native format buffer.
 *			Caller must free the buffer.
 * Returns:	Size of buffer or GDI_ERROR in failure.
 ********************************************************************************/
DWORD
ttfGetGlyphOutlinesData
(
 UINT				Glyph,					// =>
 CDC*				pDC,					// =>
 TTF_table*			pTable,
 LPTTPOLYGONHEADER* pPolygonHeaderBuffer,	// <=
 short*				pXShiftToNext			// <=
)
{
	ASSERT(pDC && pPolygonHeaderBuffer && pXShiftToNext);
	if(!pDC || !pPolygonHeaderBuffer || !pXShiftToNext)
		return GDI_ERROR;

	DWORD				BufferSize;
	GLYPHMETRICS		GlyphMetrics;
	static MAT2			TransformMatrix = {{0,1}, {0,0}, {0,0}, {0,1}};

	if((BufferSize = _GetGlyphOutline(pDC->GetSafeHdc(), pTable, Glyph, GGO_NATIVE, &GlyphMetrics, 0L, NULL, &TransformMatrix)) == GDI_ERROR)
		return GDI_ERROR;

	if(BufferSize)
	{
		ASSERT(BufferSize >= sizeof(TTPOLYGONHEADER));
		if(!(*pPolygonHeaderBuffer = (LPTTPOLYGONHEADER)new BYTE[BufferSize]))
			return GDI_ERROR;

		if(_GetGlyphOutline(pDC->GetSafeHdc(), pTable, Glyph, GGO_NATIVE, &GlyphMetrics, BufferSize, (LPVOID)*pPolygonHeaderBuffer, &TransformMatrix) == GDI_ERROR)
		{
			delete [] (*pPolygonHeaderBuffer);
			*pPolygonHeaderBuffer = NULL;
			return GDI_ERROR;
		}

	}

	*pXShiftToNext = GlyphMetrics.gmCellIncX;

	return BufferSize;
}

/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Convert FIXED point to real.
 * Returns:	Real value of the given FIXED point.
 ********************************************************************************/
void
ttfRealFromFixed
(
 RealPoint3D&	realPoint,	// <=	// note, doesn't set realPoint.z
 POINTFX&		FixedPoint	// =>
)
{
	realPoint.x = (double)(*(long*)&FixedPoint.x) / 65536.0;
	realPoint.y = (double)(*(long*)&FixedPoint.y) / 65536.0;
}


/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Calculate the arithmetic mean of two FIXED values.
 * Returns:	(val1 + val2) / 2 for FIXED values as FIXED.
 ********************************************************************************/
FIXED
ttfFixedDiv2
(
 FIXED&		fxVal1,		// =>
 FIXED&		fxVal2		// =>
)
{
	long	l = (*((long*)&(fxVal1)) + *((long*)&(fxVal2))) / 2;

	return *(FIXED*)&l;
}

/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Converts a line defined by two points to a four point
 *			Bezier spline representation of the line in pBezierPts.
 * Returns:	None.
 ********************************************************************************/
void
ttfMakeBezierFromLine
(
 RealPoint3D&	StartPt,	// =>
 RealPoint3D&	EndPt,		// =>
 RealPoint3D*	pBezierPts	// <=
)
{
	// starting point of Bezier
	pBezierPts[0] = StartPt;

	// 1st Control, pt == endpoint makes Bezier a line
	pBezierPts[1] = EndPt;

	// 2nd Control, pt == startpoint makes Bezier a line
	pBezierPts[2] = StartPt;

	// ending point of Bezier
	pBezierPts[3] = EndPt;
}

/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Converts a quadratic spline in pSplinePts to a four point
 *			Bezier spline in pBezierPts.
 * Returns:	None.
 ********************************************************************************/
void
ttfMakeBezierFromQBSpline
(
 RealPoint3D*	pSplinePts,	// =>
 RealPoint3D*	pBezierPts	// <=
)
{
	// Cubic P0 is the on curve start point
	pBezierPts[0] = pSplinePts[0];

	// Cubic P1 in terms of Quadratic P0 and P1
	pBezierPts[1].x = pSplinePts[0].x + (pSplinePts[1].x - pSplinePts[0].x) * 2.0 / 3.0;
	pBezierPts[1].y = pSplinePts[0].y + (pSplinePts[1].y - pSplinePts[0].y) * 2.0 / 3.0;
	pBezierPts[1].z = pSplinePts[0].z + (pSplinePts[1].z - pSplinePts[0].z) * 2.0 / 3.0;

	// Cubic P2 in terms of Qudartic P1 and P2
	pBezierPts[2].x = pSplinePts[1].x + (pSplinePts[2].x - pSplinePts[1].x) / 3.0;
	pBezierPts[2].y = pSplinePts[1].y + (pSplinePts[2].y - pSplinePts[1].y) / 3.0;
	pBezierPts[2].z = pSplinePts[1].z + (pSplinePts[2].z - pSplinePts[1].z) / 3.0;

	// Cubic P3 is the on curve end point
	pBezierPts[3] = pSplinePts[2];
}

/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Decode glyph outlines from the GGO_NATIVE buffer.
 *          The caller must free GlyphContoursList.
 * Returns:	True in success and false in failure.
 ********************************************************************************/
bool
ttfDecodeOutlines
(
 LPTTPOLYGONHEADER	pPolygonHeaderBuffer,	// =>
 DWORD				BufferSize,				// =>
 GlyphContours&		GlyphContoursList		// <=
)
{
	ASSERT(pPolygonHeaderBuffer);
	if(!pPolygonHeaderBuffer)
		return false;

	int					i;
	LPTTPOLYGONHEADER	pStart = pPolygonHeaderBuffer;	// the start of the current contour
	LPTTPOLYCURVE		pCurve;							// the current curve of a contour
	Contour*			pContour = NULL;
	ContourSegment*		pContourSegment = NULL;

	// Loop until we have processed the entire buffer of contours.
	// The buffer may contain one or more contours that begin with
	// a TTPOLYGONHEADER. We have them all when we the end of the buffer.
	while((DWORD)pStart < (DWORD)(((LPSTR)pPolygonHeaderBuffer) + BufferSize))
	{
		if(pStart->dwType == TT_POLYGON_TYPE)	// Currently this is the only valid type of contour.
		{
			pContour = new Contour;
			pContourSegment = new ContourSegment(0);
			// Convert the starting point. It is an on curve point.
			// All other points are continuous from the "last" point of the contour.
			ttfRealFromFixed(pContourSegment->SegmentPoints[pContourSegment->PointsCount++], pStart->pfxStart);

			// Get to first curve of contour - it starts at the next byte beyond header.
			pCurve = (LPTTPOLYCURVE)(pStart + 1);

			// Walk through this contour and process each curve segment and add it to the contours vector.
			while((DWORD)pCurve < (DWORD)(((LPSTR)pStart) + pStart->cb))
			{
				// Format assumption:
				//	The bytes immediately preceding a POLYCURVE structure contain a valid POINTFX.
				//	If this is first curve, this points to the pfxStart of the POLYGONHEADER.
				//	Otherwise, this points to the last point of the previous POLYCURVE.
				//	In either case, this is representative of the previous curve's last point.

				for(i = 0; i < pCurve->cpfx; ++i)
				{
					if(!pContour->empty())
						pContourSegment = new ContourSegment(0);

					switch(pCurve->wType)
					{
					case TT_PRIM_LINE:
						ttfRealFromFixed(pContourSegment->SegmentPoints[pContourSegment->PointsCount++], pCurve->apfx[i]);
						break;
					case TT_PRIM_QSPLINE:
						ttfRealFromFixed(pContourSegment->SegmentPoints[pContourSegment->PointsCount++], pCurve->apfx[i]);
						if(i < pCurve->cpfx - 2)
						{
							POINTFX	fxTmpPoint;
							fxTmpPoint.x = ttfFixedDiv2(pCurve->apfx[i].x, pCurve->apfx[i+1].x);
							fxTmpPoint.y = ttfFixedDiv2(pCurve->apfx[i].y, pCurve->apfx[i+1].y);
							ttfRealFromFixed(pContourSegment->SegmentPoints[pContourSegment->PointsCount++], fxTmpPoint);
						}
						else
							ttfRealFromFixed(pContourSegment->SegmentPoints[pContourSegment->PointsCount++], pCurve->apfx[++i]);
						break;
					default:
						// Oops! A POLYCURVE format we don't understand.
						ASSERT(false);
						delete pContourSegment;
						return false;
					}

					pContour->push_back(pContourSegment);
				}

				// Move on to next curve in the contour.
				pCurve = (LPTTPOLYCURVE)&(pCurve->apfx[i]);
			}

			// Add points to close the contour.
			// All contours are implied closed by TrueType definition.
			// Depending on the specific font and glyph being used, these may not always be needed.
			if( pContour->front()->SegmentPoints[0].x != pContour->back()->SegmentPoints[pContour->back()->PointsCount-1].x ||
				pContour->front()->SegmentPoints[0].y != pContour->back()->SegmentPoints[pContour->back()->PointsCount-1].y )
			{
				pContourSegment = new ContourSegment(1);
				pContourSegment->SegmentPoints[0] = pContour->front()->SegmentPoints[0];
				pContour->push_back(pContourSegment);
			}
		}
		else
		{
			// Bad, bail, must have a bogus buffer.
			ASSERT(false);
			return false;
		}

		GlyphContoursList.push_back(pContour);
		// Move on to the next contour.
		// Its header starts immediate after this contour.
		pStart = (LPTTPOLYGONHEADER)(((LPSTR)pStart) + pStart->cb);
	}

	return true;
}

/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Create a new display object for a given contour as a sequence of cubic
 *			Bezier curves control points for drawing with PolyBezier GDI function.
 * Returns:	A pointer to the new display object created or NULL in failure.
 ********************************************************************************/
gr_displayobject*
ttfCreateBezierDO
(
 db_entity*	pTextEntity,		// =>
 Contour&	ContourToConvert,	// =>
 int		Dimension,			// =>
 gr_view*	pGrView				// =>
)
{
	ASSERT(pGrView);
	if(!pGrView)
		return NULL;

	gr_displayobject*	pNewDispObj = NULL;
	int					PointNum;
	Contour::iterator	pSegment;

	if(ContourToConvert.empty())
		return NULL;

	if(!(pNewDispObj = gr_displayobject::newDisplayObject(pTextEntity)))
		return NULL;

	pNewDispObj->next	= NULL;
	pNewDispObj->type	= DISP_OBJ_PTS_GDI_TRUETYPE;
	if(Dimension == 3)
		pNewDispObj->type += DISP_OBJ_PTS_3D;
	pNewDispObj->color	= pGrView->color;
	pNewDispObj->lweight = 0;
	pNewDispObj->npts	= ContourToConvert.size() * 3 + 1;

	if(!(pNewDispObj->chain = new sds_real [pNewDispObj->npts * Dimension ]))
	{
		gr_displayobject::freeDisplayObject(pNewDispObj);
		return NULL;
	}

	RealPoint3D		StartPoint, EndPoint,
					QuadraticSegment[3],
					CubicSegment[4];

	for(PointNum = 0, pSegment = ContourToConvert.begin(); pSegment != ContourToConvert.end(); ++pSegment)
	{
		// convert segment data to the cubic Bezier curve representation
		switch((*pSegment)->PointsCount)
		{
		case 1:			// non-starting line segment
			--pSegment;
			StartPoint = (*pSegment)->SegmentPoints[(*pSegment)->PointsCount-1];
			++pSegment;
			EndPoint = (*pSegment)->SegmentPoints[0];
			ttfMakeBezierFromLine(StartPoint, EndPoint, CubicSegment);
			break;
		case 2:
			if(pSegment == ContourToConvert.begin())
			{			// starting line segment
				StartPoint = (*pSegment)->SegmentPoints[0];
				EndPoint = (*pSegment)->SegmentPoints[1];
				ttfMakeBezierFromLine(StartPoint, EndPoint, CubicSegment);
			}
			else
			{			// non-starting quadratic B-spline segment
				--pSegment;
				QuadraticSegment[0] = (*pSegment)->SegmentPoints[(*pSegment)->PointsCount-1];
				++pSegment;
				QuadraticSegment[1] = (*pSegment)->SegmentPoints[0];
				QuadraticSegment[2] = (*pSegment)->SegmentPoints[1];
				ttfMakeBezierFromQBSpline(QuadraticSegment, CubicSegment);
			}
			break;
		case 3:			// starting quadratic B-spline segment
			QuadraticSegment[0] = (*pSegment)->SegmentPoints[0];
			QuadraticSegment[1] = (*pSegment)->SegmentPoints[1];
			QuadraticSegment[2] = (*pSegment)->SegmentPoints[2];
			ttfMakeBezierFromQBSpline(QuadraticSegment, CubicSegment);
			break;
		default:
			ASSERT(false);
			gr_displayobject::freeDisplayObject(pNewDispObj);
			return NULL;
		}

		// add points to the diplay object chain
		sds_point	SegmentPoint;
		for(int i = (pSegment == ContourToConvert.begin() ? 0 : 1); i < 4; ++i, ++PointNum)
		{
			SegmentPoint[0] = CubicSegment[i].x;
			SegmentPoint[1] = CubicSegment[i].y;
			SegmentPoint[2] = CubicSegment[i].z;
			if(pGrView->projmode && gr_ucs2rp(SegmentPoint, SegmentPoint, pGrView))

			{
				gr_displayobject::freeDisplayObject(pNewDispObj);
				return NULL;
			}
			gr_setdochainpt(pNewDispObj, PointNum, SegmentPoint, Dimension);
		}
	}

	return pNewDispObj;
}

/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Linearize a Quadratic Spline segment represented by given 3 points
 *			adding resulting points to the given polyline.
 * Returns:	True in success and false in failure.
 ********************************************************************************/
bool
ttfLinearizeQSplineSegment
(
 sds_point		PointsIn[3],	// =>
 gr_view*		pGrView,		// =>
 CPolyCache&	PointsOut		// <=
)
{
	ASSERT(pGrView && PointsIn);
	if(!pGrView || !PointsIn)
		return false;

	RC				rc = SUCCESS;
	sds_point		ZeroPt = {0.0, 0.0, 0.0};
	PSpline			pSpline = NULL;
	PKnots			pKnots = NULL;
	PControlPoints	pControlPoints = NULL;
	int				i;

	// create Geo's spline
	if(!(pKnots = new CKnots(2, 3, rc)) || rc != SUCCESS)
		return false;

	pKnots->Set(3, 1.0);

	if(!(pControlPoints = new CControlPoints(3, rc)) || rc != SUCCESS)
	{
		delete pKnots;
		return false;
	}

	for(i = 3; i--; )
		pControlPoints->Set(i, C3Point(PointsIn[i][0], PointsIn[i][1], PointsIn[i][2]));

	if(!(pSpline = new CSpline(pKnots, pControlPoints, rc)) || rc != SUCCESS)
	{
		delete pKnots;
		delete pControlPoints;
		return false;
	}

	// linearize the spline
	if(pSpline->Linearize(fabs(pGrView->GetPixelHeight()) / pGrView->absxsclp, NULL, PointsOut) != SUCCESS)
	{
		delete pSpline;
		return false;
	}

	delete pSpline;
	return true;
}

/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Linearize the given contour; x-clip an obtained polygon and create
 *			display objects list from resulting poligons.
 * Returns:	A pointer to the new display object created or
 *			NULL in failure or if there is no to display due to the x-clipping.
 ********************************************************************************/
gr_displayobject*
ttfCreateLinearDO
(
 db_entity*	pTextEntity,		// =>
 Contour&	ContourToConvert,	// =>
 int		Dimension,			// =>
 gr_view*	pGrView				// =>
)
{

#define COPY_3DPT_TO_SDS(Sds_pt, RealPoint3D_pt)	memcpy(Sds_pt, &(RealPoint3D_pt), 3 * sizeof(double))

	ASSERT(pGrView);
	if(!pGrView || ContourToConvert.empty())
		return NULL;

	gr_displayobject	*pNewDispObj = NULL, *pBegDispObj = NULL, *pEndDispObj = NULL;
	bool				bSomethingWrong = false;
	int					PointNum;
	Contour::iterator	pSegment;
	CPolyCache			ContourPoints;
	sds_point			QuadraticSegment[3];
	PPointArray			pPoints = NULL;
	PPolyLine			pPolygon = NULL;
	PPolylinesList		PPolylines;
	PPolylinesList::iterator	pPPolyline;

	// create and fill the only polygon in the cache
	if(ContourPoints.StartNewPoly() != SUCCESS)
	{
		bSomethingWrong = true;
		goto out;
	}

	for(pSegment = ContourToConvert.begin(); pSegment != ContourToConvert.end(); ++pSegment)
	{
		switch((*pSegment)->PointsCount)
		{
		case 1:	// non-starting line segment
			if(ContourPoints.Accept(C3Point((*pSegment)->SegmentPoints[0].x, (*pSegment)->SegmentPoints[0].y, (*pSegment)->SegmentPoints[0].z), 0.0) != SUCCESS)
			{
				bSomethingWrong = true;
				goto out;
			}
			break;

		case 2:
			if(pSegment == ContourToConvert.begin())
			{	// starting line segment
				if( ContourPoints.Accept(C3Point((*pSegment)->SegmentPoints[0].x, (*pSegment)->SegmentPoints[0].y, (*pSegment)->SegmentPoints[0].z), 0.0) != SUCCESS ||
					ContourPoints.Accept(C3Point((*pSegment)->SegmentPoints[1].x, (*pSegment)->SegmentPoints[1].y, (*pSegment)->SegmentPoints[1].z), 0.0) != SUCCESS )
				{
					bSomethingWrong = true;
					goto out;
				}
			}
			else
			{	// non-starting quadratic B-spline segment
				--pSegment;
				COPY_3DPT_TO_SDS(QuadraticSegment[0], (*pSegment)->SegmentPoints[(*pSegment)->PointsCount-1]);
				++pSegment;
				COPY_3DPT_TO_SDS(QuadraticSegment[1], (*pSegment)->SegmentPoints[0]);
				COPY_3DPT_TO_SDS(QuadraticSegment[2], (*pSegment)->SegmentPoints[1]);

				if(!ttfLinearizeQSplineSegment(QuadraticSegment, pGrView, ContourPoints))
				{
					bSomethingWrong = true;
					goto out;
				}
			}
			break;

		case 3:	// starting quadratic B-spline segment
			COPY_3DPT_TO_SDS(QuadraticSegment[0], (*pSegment)->SegmentPoints[0]);
			COPY_3DPT_TO_SDS(QuadraticSegment[1], (*pSegment)->SegmentPoints[1]);
			COPY_3DPT_TO_SDS(QuadraticSegment[2], (*pSegment)->SegmentPoints[2]);

			if(!ttfLinearizeQSplineSegment(QuadraticSegment, pGrView, ContourPoints))
			{
				bSomethingWrong = true;
				goto out;
			}
			break;

		default:
			ASSERT(false);
			bSomethingWrong = true;
			goto out;

		}	// switch(pSegment->PointsCount)

	}	// for thru segment points


	ASSERT(ContourPoints.Size() == 1);

	// x-clip the contour
	if(!(pPolygon = new CPolyLine()) || pPolygon->Copy(*(ContourPoints.Poly(0)), 0, (ContourPoints.Poly(0))->Size()) != SUCCESS)
	{
		delete pPolygon;
		bSomethingWrong = true;
		goto out;
	}

	PPolylines.push_back(pPolygon);

	if(!pGrView->m_Clipper.ClipPolygons(PPolylines))
	{
		bSomethingWrong = true;
		goto out;
	}

	// create display objects list
	for(pPPolyline = PPolylines.begin(); pPPolyline != PPolylines.end(); ++pPPolyline)
	{
		if(!(pNewDispObj = gr_displayobject::newDisplayObject(pTextEntity)))
		{
			bSomethingWrong = true;
			goto out;
		}

		pNewDispObj->next	= NULL;
		pNewDispObj->type	= DISP_OBJ_PTS_GDI_FILLPATH;
		if(Dimension == 3)
			pNewDispObj->type += DISP_OBJ_PTS_3D;
		pNewDispObj->color	= pGrView->color;
		pNewDispObj->lweight = 0;
		pNewDispObj->npts = (*pPPolyline)->Size();

		if(!(pNewDispObj->chain = new sds_real[ pNewDispObj->npts * Dimension ]))
		{
			bSomethingWrong = true;
			goto out;
		}

		sds_point	SdsPoint;
		for(PointNum = 0; PointNum < pNewDispObj->npts; ++PointNum)
		{
			SdsPoint[0] = (**pPPolyline)[PointNum].X();
			SdsPoint[1] = (**pPPolyline)[PointNum].Y();
			SdsPoint[2] = (**pPPolyline)[PointNum].Z();

			if(pGrView->projmode && gr_ucs2rp(SdsPoint, SdsPoint, pGrView))
			{
				bSomethingWrong = true;
				goto out;
			}
			gr_setdochainpt(pNewDispObj, PointNum, SdsPoint, Dimension);
		}

		if(!pBegDispObj)
			pBegDispObj = pNewDispObj;
		else
			pEndDispObj->next = pNewDispObj;
		pEndDispObj = pNewDispObj;
		pNewDispObj = NULL;
	}

	out:

	ContourPoints.Clear();
	FreePointersList(PPolylines);
	gr_displayobject::freeDisplayObject(pNewDispObj);

	if(bSomethingWrong)
	{
		gr_displayobject::freeDisplayObject(pBegDispObj);
		return NULL;
	}

	return pBegDispObj;
}


//
// ---------------------------------- common utils ----------------------------------
//

/******************************************************************************
*+ ConvertTextHeightToPix
*
*  Transforms the specified height from a real value to pixels according to
*  the settings of the specified view.
*
*  NOTE:	This function was derived from gr_vect_rp2pix().
*
*			It may be that this function could serve more general uses.
*
*/
__declspec( dllexport)
void
ConvertTextHeightToPix
(
 sds_real	realHeight,
 gr_view*	view,
 int&		pixelHeight
)
{
	ASSERT(view);

	// make sure we have a valid view
	if ((view == NULL) || (icadRealEqual(view->viewsize,0.0)))
		view = &gr_defview;

	// transform the height
	sds_real startHeight = 0.0;
	sds_real endHeight = fabs(realHeight);

	startHeight = (startHeight - view->GetCenterProjectionY()) * view->GetReciprocalOfPixelHeight();
	endHeight = (endHeight - view->GetCenterProjectionY()) * view->GetReciprocalOfPixelHeight();

		// **********************
		// INLINE ASSEMBLY!!!
		// gr_vect_rp2pix is one of the most called functions in ICAD, since it is called about twice
		// for every point in a drawing.  Profiling indicated that the biggest bottleneck in this function
		// was the (int) cast, which resulted in call to Microsoft's ftol() function.  ftol() was taking 80%
		// of the time in gr_vect_rp2pix -- over 4x as much as the clipping above (which is inlined)
		//
		// This code improves the overall performance of gr_vect_rp2pix from 512 cycles per call
		// to 116 cycles per call
		//
		// Basically this assembly is equivalent to
		// intStartHeight = (int)startHeight;
		// intEndHeight = (int)endHeight;
		//
		// fld -- floating point load -- loads a double into a FPU register
		// fistp -- floating point to integer store pointer -- stores a double into a 32-bit integer
		//

	int intStartHeight, intEndHeight;

	_asm
		{
		fld		qword ptr startHeight
		fistp	dword ptr intStartHeight
		fld		qword ptr endHeight
		fistp	dword ptr intEndHeight
		}

	// DP: inprove perfomance on X cycles ;)
	//int startPixel = intStartHeight + view->midwin[1];
	//int endPixel = intEndHeight + view->midwin[1];

	//pixelHeight = abs(endPixel - startPixel);
	pixelHeight = abs(intEndHeight - intStartHeight);
}

/******************************************************************************
* Author:		Denis Petrov
* Description:	Converts height in pixels into height in units.
******************************************************************************/
__declspec(dllexport)
void
ConvertPixToTextHeight
(
 sds_real&	height,
 gr_view*	view,
 int		pixelsHeight
)
{
	if ((view == NULL) || (icadRealEqual(view->viewsize,0.0)))
		view = &gr_defview;

	sds_real p0, pH, x;
	gr_pix2rp(view, 0, 0, &x, &p0);
	gr_pix2rp(view, 0, pixelsHeight, &x, &pH);

	height = fabs(pH - p0);
}

/********************************************************************************
 * Author:	Dmitry Gavrilov
 * Purpose:	Obtain the current ANSI code-page identifier for the system
 *			and convert it to the character-set value.
 * Returns:	Current system character set.
 ********************************************************************************/
 /* This code move to ICADLIB/TTFInfoGetter.cpp EBATECH(CNBR) 2001-06-13
__declspec(dllexport)
BYTE
GetSystemCharSet()
{
	switch(GetACP())
	{
		case  932 : return SHIFTJIS_CHARSET;
		case  949 :	return HANGUL_CHARSET;
		case  936 :	return GB2312_CHARSET;
		case  950 :	return CHINESEBIG5_CHARSET;
		case  1253:	return GREEK_CHARSET;
		case  1254:	return TURKISH_CHARSET;
		case  1255:	return HEBREW_CHARSET;
		case  1256:	return ARABIC_CHARSET;
		case  1257:	return BALTIC_CHARSET;
		case  1251:	return RUSSIAN_CHARSET;
		case  874 :	return THAI_CHARSET;
		case  1250:	return EASTEUROPE_CHARSET;
		default   :	return ANSI_CHARSET;
	}
}
*/
#pragma warning (default : 4786)
