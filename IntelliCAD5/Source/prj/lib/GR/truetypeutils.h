/* PRJ\LIB\GR\TRUETYPEUTILS.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * Utilities needed by GR for drawing TrueType text using GDI.
 */

#ifndef _TRUETYPEUTILS_H
#define _TRUETYPEUTILS_H

#include "icadview.h"
#include "glyph.h"
#include "d3.h"

class CTextFragmentStyle;
struct gr_view;
struct gr_displayobject;

__declspec(dllexport)
void	ConvertTextHeightToPix(sds_real realHeight, gr_view *view, int& pixelHeight);
__declspec(dllexport)
void	ConvertPixToTextHeight(sds_real& height, gr_view *view, int pixelsHeight);

// EBATECH(CNBR) 2001.06.13 move to ICADLIB/TTFFontgetter.h
//__declspec(dllexport)
//BYTE	GetSystemCharSet();

/*D.G.*/// Comment the following to use old TTF text output method.
// See my comments about it in the truetypeutils.cpp file.
#define NEW_TTF_OUTPUT_METHOD

typedef CD3 RealPoint3D;

struct ContourSegment
{
	ContourSegment(short pointsCount) : PointsCount(pointsCount) {}

	short		PointsCount;		// a segment may contain 1, 2 or 3 points: the first segment of a contour
									// contain 2 (for line) or 3 (for spline) points and the following ones
	RealPoint3D	SegmentPoints[3];	// contain 1 (for line) or 2 (for spline) points
};

#include <list>
class CPolyCache;

typedef std::list<ContourSegment*>	Contour;
typedef std::list<Contour*>			GlyphContours;

void				ttfRealFromFixed(RealPoint3D& realPoint, POINTFX& FixedPoint);
FIXED				ttfFixedDiv2(FIXED& fxVal1, FIXED& fxVal2);

LPOUTLINETEXTMETRIC	ttfGetOutlineTextMetrics(CDC* pDC);

void				ttfGetBoundingBox(wchar_t w_Text[], bool bHorizontal, sds_point& Scales, LPOUTLINETEXTMETRIC pMetrics, CDC* pDC, sds_point* BoxPoints);

void				ttfCStoUCS(POINT& NextInsertPoint, double TanObl, sds_point& Scales, double CosAng, double SinAng, sds_point& InsertPoint, gr_view* pGrView, RealPoint3D& Point);

DWORD				ttfGetGlyphOutlinesData(UINT Glyph, CDC* pDC, TTF_table* ttf_table, LPTTPOLYGONHEADER* pPolygonHeaderBuffer, short* pXShiftToNext);
bool				ttfDecodeOutlines(LPTTPOLYGONHEADER pPolygonHeaderBuffer, DWORD BufferSize, GlyphContours& GlyphContoursList);

void				ttfMakeBezierFromLine(RealPoint3D& StartPt, RealPoint3D& EndPt, RealPoint3D* pBezierPts);
void				ttfMakeBezierFromQBSpline(RealPoint3D* pSplinePts, RealPoint3D* pBezierPts);
gr_displayobject*	ttfCreateBezierDO(db_entity* pTextEntity,Contour& ContourToConvert, int Dimension, gr_view* pGrView);

bool				ttfLinearizeQSplineSegment(sds_point PointsIn[3], gr_view* pGrView, CPolyCache& PointsOut);
gr_displayobject*	ttfCreateLinearDO(db_entity* pTextEntity, Contour& ContourToConvert, int Dimension, gr_view* pGrView);

#endif // _TRUETYPEUTILS_H
