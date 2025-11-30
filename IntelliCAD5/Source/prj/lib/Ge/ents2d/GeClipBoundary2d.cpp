// File: GeClipBoundary2d.cpp
//

#include "GeClipBoundary2d.h"

//////////////////////////////////////////////////
// CGeClipBoundary2d class implementation
//////////////////////////////////////////////////

CGeClipBoundary2d::CGeClipBoundary2d()
{
}

CGeClipBoundary2d::CGeClipBoundary2d(const CGePoint2d& cornerA, const CGePoint2d& cornerB)
{
}

CGeClipBoundary2d::CGeClipBoundary2d(const CGePoint2dArray& clipBoundary)
{
}

CGeClipBoundary2d::CGeClipBoundary2d(const CGeClipBoundary2d& source)
{
}

CGe::EClipError
CGeClipBoundary2d::set(const CGePoint2d& cornerA, const CGePoint2d& cornerB)
{
	return CGe::eSuccess;
}

CGe::EClipError
CGeClipBoundary2d::set(const CGePoint2dArray& clipBoundary)
{
	return CGe::eSuccess;
}

CGe::EClipError
CGeClipBoundary2d::clipPolygon(const CGePoint2dArray& rawVertices, CGePoint2dArray& clippedVertices, CGe::EClipCondition& clipCondition, CGeIntArray* pClippedSegmentSourceLabel /* = 0*/) const
{
	return CGe::eSuccess;
}

CGe::EClipError
CGeClipBoundary2d::clipPolyline(const CGePoint2dArray& rawVertices, CGePoint2dArray& clippedVertices, CGe::EClipCondition& clipCondition, CGeIntArray* pClippedSegmentSourceLabel /* = 0*/) const
{
	return CGe::eSuccess;
}

CGeClipBoundary2d&
CGeClipBoundary2d::operator = (const CGeClipBoundary2d& source)
{
	return *this;
}
