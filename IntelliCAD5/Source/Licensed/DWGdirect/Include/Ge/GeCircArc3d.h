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



#ifndef OD_GECIRCARC3D_H
#define OD_GECIRCARC3D_H

#include "OdPlatform.h"

#include "GeCurve3d.h"
#include "GePlane.h"

#include "DD_PackPush.h"

class OdGeLine3d;
class OdGeCircArc2d;
class OdGePlanarEnt;
class OdGeExtents3d;

/** Description: 
 A mathematical entity used to represent a circular arc in 3D space.
 
 {group:OdGe_Classes} 
*/
class GE_TOOLKIT_EXPORT OdGeCircArc3d : public OdGeCurve3d
{
public:
	/** Arguments:
		center (I) Center of *arc*.
		normal (I) A vector normal the plane of the *arc*
		radius (I) Radius of *arc*.
		startAngle (I) Starting angle of *arc* in radians.
		endAngle (I) Ending angle of *arc* in radians.
		refVec (I) The reference vector defining *arc* angle 0.
		isClockwise (I) If true, the *arc* is drawn clockwise, otherwise, counterclockwise.
		startPoint (I) Start point of *arc*.
		secondPoint (I) Second point on a 3-point *arc*.
		endPoint (I) End point of *arc*.

		Remarks:
		startAngle must be less than endAngle. 

		To construct a circle, set endAngle = startAngle + Oda2PI .
	*/
	OdGeCircArc3d()
		: m_vNormal(OdGeVector3d::kZAxis)
		, m_vRefVec(OdGeVector3d::kXAxis)
		, m_dRadius(1.)
		, m_dStartAngle(0.)
		, m_dInclAngle(Oda2PI)
 {}

	OdGeCircArc3d(const OdGeCircArc3d& arc)
		: m_pCenter(arc.m_pCenter)
		, m_vNormal(arc.m_vNormal)
		, m_vRefVec(arc.m_vRefVec)
		, m_dRadius(arc.m_dRadius)
		, m_dStartAngle(arc.m_dStartAngle)
		, m_dInclAngle(arc.m_dInclAngle)
	{}

	OdGeCircArc3d(const OdGePoint3d& center, 
		const OdGeVector3d& normal, 
		double radius)
		: m_dStartAngle(0.)
		, m_dInclAngle(Oda2PI)
		{set(center, normal, radius);}

	OdGeCircArc3d(const OdGePoint3d& center, 
		const OdGeVector3d& normal,
		const OdGeVector3d& refVec, 
		double radius, double startAngle = 0, 
		double endAngle = Oda2PI)
		{set(center, normal, refVec, radius, startAngle, endAngle);}

	OdGeCircArc3d(const OdGePoint3d& startPoint, 
		const OdGePoint3d& secondPoint, 
		const OdGePoint3d& endPoint)
		{set(startPoint, secondPoint, endPoint);}

	OdGe::EntityId type() const { return OdGe::kCircArc3d;}
	
	bool isKindOf(OdGe::EntityId id) const;

	/** Description:
		Returns the point on the circle and the point
		on the plane closest to the specified plane.

		Arguments:
		plane (I) Plane in question.
		pointOnPlane (O) Closes point on plane.
        tol (I) Tolerance for determining position.
	*/
	OdGePoint3d closestPointToPlane (const OdGePlanarEnt& plane,
		OdGePoint3d& pointOnPlane, 
		const OdGeTol& tol = OdGeContext::gTol) const;
	
	/** Description:
		Returns the intersections with other objects.

		Arguments:
		arc (I) Any 3D *arc*.
		line (I) Any 3D *line*.
		plane (I) Any *plane*.
		numInt (O) The number of intersections.
		p1 (O) The first intersection point.
		p2 (O) The second intersection point.
		tol (I) Tolerance for determining position.
	*/
	bool intersectWith(const OdGeLinearEnt3d& line, 
		int& numInt, 
		OdGePoint3d& p1, 
		OdGePoint3d& p2,	
		const OdGeTol& tol = OdGeContext::gTol) const;

	bool intersectWith(const OdGeCircArc3d& arc, 
		int& numInt,	
		OdGePoint3d& p1, 
		OdGePoint3d& p2, 
		const OdGeTol& tol = OdGeContext::gTol) const;

	bool intersectWith(const OdGePlanarEnt& plane, 
		int& numInt,	
		OdGePoint3d& p1, 
		OdGePoint3d& p2,
		const OdGeTol& tol = OdGeContext::gTol) const;
	
	/** Description:
		Returns the projected intersections with other objects.

		Arguments:
		line (I) Any 3D line.
		numInt (O) The number of intersections.
		pntOnArc1 (O) The first intersection point on the arc.	
		pntOnArc2 (O) The second intersection point on the arc.
		pntOnLine1 (O) The first intersection point on the line.	
		pntOnLine2 (O) The second intersection point on the line.
		tol (I) Tolerance for determining position.
	*/
	bool projIntersectWith(const OdGeLinearEnt3d& line, 
		const OdGeVector3d& projDir, 
		int& numInt,
		OdGePoint3d& pntOnArc1,	
		OdGePoint3d& pntOnArc2,
		OdGePoint3d& pntOnLine1,	
		OdGePoint3d& pntOnLine2,
		const OdGeTol& tol = OdGeContext::gTol) const;

	/* Description:
		Returns true if and only if the specified point is
		on the full circle of this *arc*, the *tangent*
		at that point, and the status of the query.

		Arguments:
		pnt (I) The point on the full circle.
		line (O) The *tangent* at that point.
		tol (I) Tolerance for determining coincidence.
		err (O) The status of the query.

		Remarks:
		Possible values for err include:
		
		o kArg1TooBig
		o kArg1InsideThis
		o kArg1OnThis
	*/
	bool tangent(const OdGePoint3d& pnt, 
		OdGeLine3d& line, 
		const OdGeTol& tol = OdGeContext::gTol) const;

	bool tangent(const OdGePoint3d& pnt, 
		OdGeLine3d& line, 
		const OdGeTol& tol, 
		OdGeError& error) const;

	/** Description:
		Returns the plane of the *arc*.
		
		Arguments:
		plane (O) The *plane* of the *arc*.
	*/
	void getPlane(OdGePlane& plane) const;

	/** Description:
		Returns true if and only if the specified point lies inside the full *circle* of this *arc*, and is
		on the same plane as his *arc*.
	*/
	bool isInside(const OdGePoint3d& pnt, const OdGeTol& tol = OdGeContext::gTol) const;
	
	/** Description:
		Returns the *center* of this *arc*.
	*/
	OdGePoint3d center() const { return m_pCenter;}

	/** Description:
		Reurns the vector normal to the plane of this *arc*.
	*/
	OdGeVector3d normal() const { return m_vNormal;}

 	/** Description:
		Returns the reference vector as a unit vector.
	*/
	OdGeVector3d refVec() const { return m_vRefVec;}

	/** Description:
		Returns the *radius* of this *arc*.
	*/
	 double radius() const { return m_dRadius;}

	/** Description:
		Returns starting angle measured from reference vector in the *arc* direction.
	*/
	double startAng() const { return m_dStartAngle;}

	/** Description:
		Returns ending angle measured from reference vector in the *arc* direction.
	*/
	double endAng() const { return m_dStartAngle + m_dInclAngle;}

	/** Description:
		Returns the start point of this *arc*.
	*/
	OdGePoint3d startPoint () const;

	/** Description:
		Returns the end point of this *arc*.
	*/
	OdGePoint3d endPoint () const;

	/** Description:
		Sets the *center* of this *arc*, and returns a reference to this *arc*. Returns a reference to
		this *arc*.

		Arguments:
		center (I) Center of *arc*.
	*/
	OdGeCircArc3d& setCenter(const OdGePoint3d& c) { m_pCenter = c; return *this;}

	/** Descripion:
		Sets the *normal( and reference vectors for this *arc*. Returns a reference
		to this arc.

		Arguments:
		normal (I) A vector normal the plane of the *arc*
		refVec (I) The reference vector defining *arc* angle 0.
	*/
	OdGeCircArc3d& setAxes(const OdGeVector3d& normal, 
		const OdGeVector3d& refVec)
		{m_vNormal = normal; m_vRefVec = refVec; return *this;}


	/** Description:
		Sets the *radius* of this *arc*, and returns a reference to this *arc*.

		Arguments:
		radius (I) Radius of *arc*.
	*/
	OdGeCircArc3d& setRadius(double radius) { m_dRadius = radius; return *this;}

	/** Description:
		Sets the starting and ending angles of this *arc*, and returns a reference to this *arc*.

        Arguments:
		startAngle (I) Starting angle of *arc* in radians.
		endAngle (I) Ending angle of *arc* in radians.
	*/
	OdGeCircArc3d& setAngles(double startAngle, 
		double endAngle);
	
	/** Description:
		Sets the parameters for this *arc* according to the arguments, and returns a reference to this *arc*.

		Arguments:
		center (I) Center of *arc*.
		normal (I) A vector normal the plane of the *arc*
		radius (I) Radius of *arc*.
		startAngle (I) Starting angle of *arc* in radians.
		endAngle (I) Ending angle of *arc* in radians.
		err(O) Error status for this method.
		refVec (I) The reference vector defining *arc* angle 0.
		startPoint (I) Start point of *arc*.
		secondPoint (I) Second point on a 3-point *arc*.
		endPoint (I) End point of *arc*.
		curve1 (I) First curve to define a *tangent* *arc*.
		curve2 (I) Second curve to define a *tangent* *arc*.
		curve3 (I) Third curve to define a tangent *arc*..
		param1 (O) Parameter corresponding tangency point on curve1.
		param2 (O) Parameter corresponding tangency point on curve2.
		param3 (O) Parameter corresponding tangency point on curve2.
		success (O) Returns true if and only if the tan-tan-radius or
					tan-tan-tan curve could be constructed. If false,
					this *arc* is unmodified.

		Remarks:
		To construct a circle, set endAngle = startAngle + Oda2PI .

		If bulgeFlag = true, then *bulge* is length maximum distance from the *arc* to a
		chord between the start and end points.

		If bulgeFlag = false, the *bulge* is the *tangent* of 1/4 the included angle of the *arc*.

		Possible valus for err include:
		
		o kEqualArg1Arg2
		o kEqualArg1Arg3
		o kEqualArg2Arg3
		o kLinearlyDependentArg1Arg2Arg3.
	*/

	OdGeCircArc3d& set(const OdGePoint3d& center,	
		const OdGeVector3d& normal, 
		double radius);

	OdGeCircArc3d& set(const OdGePoint3d& center, 
		const OdGeVector3d& normal,
		const OdGeVector3d& refVec, 
		double radius,	
		double startAngle, 
		double endAngle);

	OdGeCircArc3d& set(const OdGePoint3d& stafrtPoint, 
		const OdGePoint3d& secondPoint,
		const OdGePoint3d& endPoint);

	OdGeCircArc3d& set(const OdGePoint3d& startPoint, 
		const OdGePoint3d& pnt,
		const OdGePoint3d& endPoint, 
		OdGeError& err);

	OdGeCircArc3d& set(const OdGeCurve3d& curve1,
		const OdGeCurve3d& curve2,
		double radius, 
		double& param1, 
		double& param2,	
		bool& success);

	OdGeCircArc3d& set(const OdGeCurve3d& curve1,	
		const OdGeCurve3d& curve2,
		const OdGeCurve3d& curve3, 
		double& param1, 
		double& param2, 
		double& param3, 
		bool& success);
	
	OdGeCircArc3d& operator=(const OdGeCircArc3d& arc);

	OdGeEntity3d& transformBy(const OdGeMatrix3d& xfm);

	void appendSamplePoints(double fromParam, 
		double toParam, 
		double approxEps, 
		OdGePoint3dArray& pointArray, 
		OdGeDoubleArray* pParamArray = 0) const;

	DD_USING(OdGeCurve3d::appendSamplePoints);
	DD_USING(OdGeCurve3d::setInterval); 

	OdGePoint3d evalPoint(double param) const;

	double paramOf(const OdGePoint3d& pnt, 
		const OdGeTol& tol = OdGeContext::gTol) const;

	void getInterval(OdGeInterval& interval) const;

	bool setInterval(const OdGeInterval& interval);

	OdGeCurve3d& reverseParam();

	bool isOn(const OdGePoint3d& pnt, 
		const OdGeTol& tol = OdGeContext::gTol) const;

	virtual bool isClosed(const OdGeTol& tol = OdGeContext::gTol) const;

	/** Description:
		Returns the geometric extents of this *arc*.

		Arguments:
		exts (O) Geometric extents.
	*/
	void getGeomExtents(OdGeExtents3d& exts) const;

	bool hasStartPoint(OdGePoint3d& startPnt) const;

	bool hasEndPoint(OdGePoint3d& endPnt) const;

private:
	OdGePoint3d m_pCenter;
	OdGeVector3d m_vNormal;
	OdGeVector3d m_vRefVec;
	double m_dRadius;
	double m_dStartAngle;
	double m_dInclAngle;
};

#include "DD_PackPop.h"

#endif


