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
// DWGdirect © 2002 by Open Design Alliance Inc. All rights reserved. 
//
// By use of this software, you acknowledge and accept the terms of this 
// agreement.
///////////////////////////////////////////////////////////////////////////////



#ifndef OD_GE_ELLIP_ARC_2D_H
#define OD_GE_ELLIP_ARC_2D_H


class OdGeCircArc2d;
class OdGePlanarEnt;
class OdGeEllipArc2d;
class OdGeLinearEnt2d;
class OdGeExtents2d;

#include "OdPlatform.h"

#include "GeCurve2d.h"

#include "DD_PackPush.h"

/** Description:
	Represents elliptical arcs and full ellipses in 2D space.
 
	Remarks:
	The angle of point on an ellipse is measured by projecting
	the point on the ellipse perpendicular to major axis onto a
	circle whose *center* is the *center* of the ellipse and whose
	radius is the major radius of the ellipse.

	The angle between the major axis of the ellipse, and a vector from
	the *center* of the ellipse to the intersection point with the circle, 
	measured counterclockwise, is the angle of the point on the ellipse.

	{group:OdGe_Classes} 
*/
class GE_TOOLKIT_EXPORT OdGeEllipArc2d : public OdGeCurve2d
{
public:
 ~OdGeEllipArc2d();

	/** Arguments:
		center (I) The *center* of the *elliptical arc*.
		majorAxis (I) The major axis of the *elliptical arc*.
		minorAxis (I) The minor axis of the *elliptical arc*.
		majorRadius (I) The major radius of the *elliptical arc*.
		minorRadius (I) The minor radius of the *elliptical arc*.
		startAngle (I) Starting angle of the *elliptical arc* in radians.
		endAngle (I) Ending angle of the *elliptical arc* in radians.
	*/
	OdGeEllipArc2d ();
	OdGeEllipArc2d (const OdGeEllipArc2d& ell);
	OdGeEllipArc2d (const OdGeCircArc2d& arc);
	OdGeEllipArc2d (const OdGePoint2d& center, 
		const OdGeVector2d& majorAxis,
		const OdGeVector2d& minorAxis, 
		double majorRadius, 
		double minorRadius);
	OdGeEllipArc2d (const OdGePoint2d& cent,
		const OdGeVector2d& majorAxis,
		const OdGeVector2d& minorAxis, 
		double majorRadius, 
		double minorRadius, 
		double startAngle, 
		double endAngle);
 
	OdGeEntity2d* copy () const;
	OdGe::EntityId type () const;
 
 /*bool intersectWith (const OdGeLinearEnt2d& line, int& intn,
 OdGePoint2d& p1, OdGePoint2d& p2,
 const OdGeTol& tol = OdGeContext::gTol) const;
 bool isCircular (const OdGeTol& tol = OdGeContext::gTol) const;
 */

	/** Description: 
		Returns true if and only if the specified point lies within
		the full ellipse.

		Arguments:
		pnt (I) Any 2D point.
		tol (I) Tolerance for determining position.
	*/
	bool isInside (const OdGePoint2d& pnt, 
		const OdGeTol& tol = OdGeContext::gTol) const;


	/** Description:
		Returns the *center* of this *elliptical arc*.
	*/
	OdGePoint2d center () const;

	/** Description:
		Returns the minor *radius* of this *elliptical arc*.
	*/
	double minorRadius () const;

	/** Description:
		Returns the major *radius* of this *elliptical arc*.
	*/
	double majorRadius () const;

	/** Description:
		Returns the minor axis of this *elliptical arc* as a unit vector
	*/
	OdGeVector2d minorAxis () const;

	/** Description:
		Returns the major axis of this *elliptical arc* as a unit vector.
	*/
	OdGeVector2d majorAxis () const;

	/** Description:
		Returns start angle measured from major axis of this *elliptical arc* in the .
	*/
	double startAng () const;

	/** Description:
		Returns end angle measured from major axis of this *elliptical arc*.
	*/
	double endAng () const;

	/** Description:
		Returns the start point of this *elliptical arc*.
	*/
	OdGePoint2d startPoint () const;

	/** Description:
		Returns the end point of this *elliptical arc*.
	*/
	OdGePoint2d endPoint () const;

	/** Description:
		Returns true if and only if this *elliptical arc* is drawn clockwise from start point to end point.
	*/
	bool isClockWise () const;
 
	/** Description:
		Sets the *center* of the *elliptical arc*.

		Arguments:
		center (I) The *center* of the *elliptical arc*.
	*/
	OdGeEllipArc2d& setCenter(const OdGePoint2d& cent);

	/** Description:
		Sets the minor radius of the *elliptical arc*.

		Arguments:
		minorRadius (I) The minor radius of the *elliptical arc*.
	*/
	OdGeEllipArc2d& setMinorRadius(double rad);

	/** Description:
		Sets the major radius of the *elliptical arc*.

		Arguments:
		majorRadius (I) The major radius of the *elliptical arc*.
	*/
	OdGeEllipArc2d& setMajorRadius(double rad);

	/** Description:
		Sets the major and minor axes of the *elliptical arc*.

		Arguments:
		majorAxis (I) The major axis of the *elliptical arc*.
		minorAxis (I) The minor axis of the *elliptical arc*.
	*/
	OdGeEllipArc2d& setAxes (const OdGeVector2d& majorAxis,
							const OdGeVector2d& minorAxis);

	/** Description:
		Sets the starting and ending angles of the *elliptical arc*.

		Arguments:
		startAngle (I) Starting angle of the *elliptical arc* in radians.
		endAngle (I) Ending angle of the *elliptical arc* in radians.
	*/
	OdGeEllipArc2d& setAngles (double startAngle, 
		double endAngle);

	/** Description:
		Sets the parameters for this *elliptical arc* according to the arguments, and returns a reference to this *elliptical arc*.

		Arguments:
		center (I) The *center* of the *elliptical arc*.
		majorAxis (I) The major axis of the *elliptical arc*.
		minorAxis (I) The minor axis of the *elliptical arc*.
		majorRadius (I) The major radius of the *elliptical arc*.
		minorRadius (I) The minor radius of the *elliptical arc*.
		startAngle (I) Starting angle of the *elliptical arc* in radians.
		endAngle (I) Ending angle of the *elliptical arc* in radians.

	*/
	OdGeEllipArc2d& set (const OdGePoint2d& cent,
						const OdGeVector2d& majorAxis,
						const OdGeVector2d& minorAxis,
						double majorRadius, double minorRadius);

	OdGeEllipArc2d& set (const OdGePoint2d& cent,
						const OdGeVector2d& majorAxis,
						const OdGeVector2d& minorAxis,
						double majorRadius, 
						double minorRadius,
						double startAngle, 
						double endAngle);
	OdGeEllipArc2d& set (const OdGeCircArc2d& arc);
 
	virtual OdGeEntity2d& transformBy(const OdGeMatrix2d& xfm);

	// OdGeCurve2d functions

	bool isClosed (const OdGeTol& tol = OdGeContext::gTol) const;

	void getInterval(OdGeInterval& intrvl) const;

	virtual double paramOf(const OdGePoint2d& pnt,
 		const OdGeTol& tol = OdGeContext::gTol) const;

	virtual OdGePoint2d evalPoint(double param) const;

	void appendSamplePoints(double fromParam, 
		double toParam,
		double approxEps, 
		OdGePoint2dArray& pointArray, 
		OdGeDoubleArray* pParamArray = 0) const;

	/** Description:
		Returns the geometric extents of this *elliptical arc*.

		Arguments:
		exts (O) Geometric extents.
	*/
	void getGeomExtents(OdGeExtents2d& exts);

	// returns the parameter of *elliptical arc* points (if any) which have tangent 
	// co-directional to "tan" vector. Parameters are returned in params array.
	// Note that this function doesn't clear array - only appends items.

	/** Descripton:
		Returns the parameters of the points (if any) on the *elliptical arc*,
		whose tangents are parallel to the specified tangent vector.

		Parameters:
			tan (I) A vector defining the tangent direction.
			params (O) The array of tangent points.

		Remarks:
		The points are appended to the specified array. You may wish to 
		initialize the params array with clear().
	*/
	void inverseTangent(OdGeVector2d tan, 
		OdGeDoubleArray& params);

	bool hasStartPoint(OdGePoint2d& startPnt) const;

	bool hasEndPoint(OdGePoint2d& endPnt) const;

 DD_USING(OdGeCurve2d::appendSamplePoints);
private:
 OdGePoint2d m_center;
 OdGeVector2d m_majorAxis, m_minorAxis;
 double m_startParam, m_includedParam;
};

#include "DD_PackPop.h"

#endif // OD_GE_ELLIP_ARC_2D_H

