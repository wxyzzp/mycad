///////////////////////////////////////////////////////////////////////////////
// Copyright © 2002, Open Design Alliance Inc. ("Open Design") 
// 
// This software is owned by Open Design, and may only be incorporated into 
// application programs owned by members of Open Design subject to a signed 
// Membership Agreement and Supplemental Software License Agreement with 
// Open Design. The structure and organization of this Software are the valuable 
// trade secrets of Open Design and its suppliers. The Software is also protected 
// by copyright law and international treaty provisions. You agree not to 
// modify, adapt, translate, reverse engineer, decompile, disassemble or 
// otherwise attempt to discover the source code of the Software. Application 
// programs incorporating this software must include the following statment 
// with their copyright notices:
//
//      DWGdirect © 2002 by Open Design Alliance Inc. All rights reserved. 
//
// By use of this software, you acknowledge and accept the terms of this 
// agreement.
///////////////////////////////////////////////////////////////////////////////



#ifndef OD_GE_CLIP_BOUNDARY_2D_H
#define OD_GE_CLIP_BOUNDARY_2D_H

#include "GeEntity2d.h"
#include "GeIntArray.h"
#include "GePoint2dArray.h"

#include "DD_PackPush.h"

/** Description:
	This class implements a 2D clipping object for clipping 2D polygons and polylines
	into convex open or closed 2D polygons and polylines.
    
    {group:OdGe_Classes} 

	Remarks:
	Similar to Sutherland-Hodgman pipeline clipping.
*/
class GE_TOOLKIT_EXPORT OdGeClipBoundary2d : public OdGeEntity2d
{
  OdGePoint2dArray m_clipPoly;
public:
	/** Arguments
		cornerA (I) First corner of rectangular clip boundary.
		cornerB (I) Opposite corner of rectangular clip boundary.
		clipBoundary (I) An array of points defining a counterclockwise,
			convex polygon.
	*/
	OdGeClipBoundary2d ();

	OdGeClipBoundary2d (const OdGePoint2d& cornerA, 
		const OdGePoint2d& cornerB);

	OdGeClipBoundary2d (const OdGePoint2dArray& clipBoundary);
  
	OdGeClipBoundary2d (const OdGeClipBoundary2d& src);
  
	/** Description
		Converts this clip boundary to a rectangular or poylgonal clip boundary.
		Returns the status of the conversion.

		Arguments:
		cornerA (I) First corner of rectangular clip boundary.
		cornerB (I) Opposite corner of rectangular clip boundary.
		clipBoundary (I) An array of points defining a counterclockwise,
			convex polygon.
	*/
	OdGe::ClipError set (const OdGePoint2d& cornerA, 
		const OdGePoint2d& cornerB);

	OdGe::ClipError set (const OdGePoint2dArray& clipBoundary);

	/** Description: 
		Clips the specified closed polygon with this clip boundary. Returns the status of the clipping.

		Arguments:
		rawVertices (I) An array of 2D points defining the polygon to be clipped.
			The polygon can be concave and/or self-intersecting.
        clippedVertices (O) An array of 2D points defining the clipped polygon.
		clipCondition (O) Returns spacial information about the clip boundary
			and the polygon.
		pClippedSegmentSourceLabel (O) An array of integers identifying
			the source segment of each segment in the clipped polygon.

		Remarks:
		Possible values for clipCondition are as follows:

		o kInvalid                        The clip failed due to an improper clip boundary, or an improper polygon.
		o kAllSegmentsInside              All the segments were inside the clip boundary.
		o kSegmentsIntersect              The clip boundary intersects one or more segments of the polygon.
		o kAllSegmentsOutsideZeroWinds    The polygon is completely outside the clip boundary and doesn't surround it.
		o kAllSegmentsOutsideOddWinds     The polygon is completely outside the clip boundary, and winds around it an odd number of times.
		o kAllSegmentsOutsideEvenWinds    The polygon is completely outside the clip boundary, and winds around it an even number of times.
	*/
	OdGe::ClipError clipPolygon (const OdGePoint2dArray& rawVertices,
        OdGePoint2dArray& clippedVertices,
		OdGe::ClipCondition& clipCondition,
		OdGeIntArray* pClippedSegmentSourceLabel = 0) const;
  
	/** Description: 
		Clips the specified closed polyline with this clip boundary. Returns the status of the clipping.

		Arguments:
		rawVertices (I) An array of 2D points defining the polyline to be clipped.
			The polyline can be concave and/or self-intersecting.
        clippedVertices (O) An array of 2D points defining the clipped polyline.
		clipCondition (O) Returns spacial information about the clip boundary
			and the polyline.
		pClippedSegmentSourceLabel (O) An array of integers identifying
			the source segment of each segment in the clipped polyline.

		Remarks:
		Possible values for clipCondition are as follows:

		o kInvalid                        The clip failed due to an improper clip boundary, or an improper polygon.
		o kAllSegmentsInside              All the segments were inside the clip boundary.
		o kSegmentsIntersect              The clip boundary intersects one or more segments of the polygon.
		o kAllSegmentsOutsideZeroWinds    The polygon is completely outside the clip boundary and doesn't surround it.
		o kAllSegmentsOutsideOddWinds     The polygon is completely outside the clip boundary, and winds around it an odd number of times.
		o kAllSegmentsOutsideEvenWinds    The polygon is completely outside the clip boundary, and winds around it an even number of times.
	*/
	OdGe::ClipError clipPolyline (const OdGePoint2dArray& rawVertices, 
		OdGePoint2dArray& clippedVertices,
		OdGe::ClipCondition& clipCondition,
		OdGeIntArray* pClippedSegmentSourceLabel = 0) const;
  
	OdGeClipBoundary2d& operator = (const OdGeClipBoundary2d& crvInt);

	OdGe::EntityId type () const;
};

#include "DD_PackPop.h"

#endif

