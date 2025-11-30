// File: GeClipBoundary2d.h
//
// CGeClipBoundary2d
//

#pragma once


#include "GeEntity2d.h"
#include "GeArrays.h"


class GE_API CGeClipBoundary2d : public CGeEntity2d
{
public:
	CGeClipBoundary2d();
	CGeClipBoundary2d(const CGePoint2d& cornerA, const CGePoint2d& cornerB);
	CGeClipBoundary2d(const CGePoint2dArray& clipBoundary);
	CGeClipBoundary2d(const CGeClipBoundary2d& source);

	CGe::EClipError		set			(const CGePoint2d& cornerA, const CGePoint2d& cornerB);
	CGe::EClipError		set			(const CGePoint2dArray& clipBoundary);
	CGe::EClipError		clipPolygon	(const CGePoint2dArray& rawVertices, CGePoint2dArray& clippedVertices, CGe::EClipCondition& clipCondition, CGeIntArray* pClippedSegmentSourceLabel = 0) const;
	CGe::EClipError		clipPolyline(const CGePoint2dArray& rawVertices, CGePoint2dArray& clippedVertices, CGe::EClipCondition& clipCondition, CGeIntArray* pClippedSegmentSourceLabel = 0) const;
	CGeClipBoundary2d&	operator =	(const CGeClipBoundary2d& source);
};
