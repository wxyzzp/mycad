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



#ifndef OD_GE_ARC2D_H
#define OD_GE_ARC2D_H

class OdGeLine2d;
class OdGeLinearEnt2d;
class OdGeExtents2d;

#include "OdPlatform.h"

#include "GeCurve2d.h"

#include "DD_PackPush.h"

/** Description:
    Represents arcs and full circles in 2D space.

    {group:OdGe_Classes} 
*/
class GE_TOOLKIT_EXPORT OdGeCircArc2d : public OdGeCurve2d
{
public:
	/** Arguments:
		center (I) Center of *arc*.
		radius (I) Radius of *arc*.
		startAngle (I) Starting angle of *arc* in radians.
		endAngle (I) Ending angle of *arc* in radians.
		refVec (I) The reference vector defining *arc* angle 0.
		isClockwise (I) If true, the *arc* is drawn clockwise, otherwise, counterclockwise.
		startPoint (I) Start point of *arc*.
		secondPoint (I) Second point on a 3-point *arc*.
		endPoint (I) End point of *arc*.
		bulge (I) Specifies the *bulge* of the *arc*.
		bulgeFlag (I) Specifies how *bulge* is to be interpreted.

		Remarks:
		startAngle must be less than endAngle. 

		To construct a circle, set endAngle = startAngle + Oda2PI .

		If bulgeFlag = true, then *bulge* is the maximum distance from the *arc* perpendicular to a
		chord between the start and end points.

		If bulgeFlag = false, the *bulge* is the *tangent* of 1/4 the included angle of the *arc*.
	*/
	OdGeCircArc2d ()
		: m_vRefVector(OdGeVector2d::kXAxis),
		m_dStartAngle(0.0),
		m_dInclAngle(Oda2PI)
		{}

	OdGeCircArc2d (const OdGeCircArc2d& arc)
		: m_pCenter(arc.m_pCenter),
		m_vRefVector(arc.m_vRefVector),
		m_dStartAngle(arc.m_dStartAngle),
		m_dInclAngle(arc.m_dInclAngle)
		{}

	OdGeCircArc2d (const OdGePoint2d& center, double radius)
		: m_vRefVector(OdGeVector2d::kXAxis),
		m_dStartAngle(0.0),
		m_dInclAngle(Oda2PI)
		{set(center, radius);}

	OdGeCircArc2d (const OdGePoint2d& center, 
		double radius, 
		double startAngle,
		double endAngle, 
		const OdGeVector2d& refVec = OdGeVector2d::kXAxis,
		bool isClockWise = false)
		{set(center, radius, startAngle, endAngle, refVec, isClockWise);}

	OdGeCircArc2d (const OdGePoint2d& startPoint,
		const OdGePoint2d& secondPoint,
		const OdGePoint2d& endPoint)
		: m_vRefVector(OdGeVector2d::kXAxis)
		{set(startPoint, secondPoint, endPoint);}

	OdGeCircArc2d (const OdGePoint2d& startPoint, 
		const OdGePoint2d& endPoint, 
		double bulge, 
		bool bulgeFlag = true)
		: m_vRefVector(OdGeVector2d::kXAxis)
		{set(startPoint, endPoint, bulge, bulgeFlag);}
  
	OdGe::EntityId type  () const  { return OdGe::kCircArc2d;}

	OdGeEntity2d* copy () const
	{
		return new OdGeCircArc2d(*this);
	}

	/** Description:
		Returns true if and only if this *arc* intersects
		with the specified *arc* or line, the number of intersections, and
		the intersection points.
		
		Arguments:
		line (I) Any 2D *line*.
		circarc (I) Any 2D *arc*.
		intn (O) The number of intersections with this curve.
		p1 (O) The first intersection point.
		p2 (O) The second intersection point.
		tol (I) Tolerance for determining intersections.

		Remarks:
		p1 has meaning if and only if intn > 0. p2 has meaning if and only if intn > 1.   
	*/
	bool intersectWith (const OdGeLinearEnt2d& line, 
		int& intn,
		OdGePoint2d& p1, 
		OdGePoint2d& p2, 
		const OdGeTol& tol = OdGeContext::gTol) const;

	bool intersectWith (const OdGeCircArc2d& circarc, 
		int& intn,
		OdGePoint2d& p1, 
		OdGePoint2d& p2, 
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
		o kArg1OnThis.
	*/
	bool tangent (const OdGePoint2d& pnt, 
		OdGeLine2d& line,
		const OdGeTol& tol = OdGeContext::gTol) const;

	bool tangent (const OdGePoint2d& pnt, 
		OdGeLine2d& line,
		const OdGeTol& tol, 
		OdGeError& err) const;
  
	/** Description:  
		Returns true if and only if the input point lies within
		the full circle of this *arc*.

		Arguments:
		pnt (I) Any 2D *point*.
		tol (I) Tolerance for determining position.
	*/
	bool isInside (const OdGePoint2d& pnt, 
		const OdGeTol& tol = OdGeContext::gTol) const;

	/** Description:
		Returns the *center* of this *arc*.
	*/
	OdGePoint2d center () const{ return m_pCenter;}

	/** Description:
		Returns the *radius* of this *arc*.
	*/
	double radius () const { return m_vRefVector.length();}

	/** Description:
		Returns starting angle measured from reference vector in the *arc* direction.
	*/
	double startAng () const;

	/** Description:
		Returns ending angle measured from reference vector in the *arc* direction.
	*/
	double endAng () const;

	/** Description:
		Returns start angle measured from X axis in the *arc* direction.
	*/
	double startAngFromXAxis () const;

	/** Description:
		Returns end angle measured from X axis in the *arc* direction.
	*/
	double endAngFromXAxis () const;

	/** Description:
		Returns true if and only if this *arc* is drawn clockwise from start point to enD *point*.
	*/
	bool isClockWise () const  { return m_dInclAngle < 0.;}

	/** Description:
		Returns the reference vector as a unit vector.
	*/
	OdGeVector2d refVec () const { return m_vRefVector.normal();}

	/** Description:
		Returns the start point of this *arc*.
	*/
	OdGePoint2d startPoint () const;

	/** Description:
		Returns the end point of this *arc*.
	*/
	OdGePoint2d endPoint () const;
  
	/** Description:
		Sets the *center* of this *arc*, and returns a reference to this *arc*.

		Arguments:
		center (I) Center of *arc*.
	*/
	OdGeCircArc2d& setCenter(const OdGePoint2d& center) { m_pCenter = center; return *this; }

	/** Description:
		Sets the *radius* of this *arc*, and returns a reference to this *arc*.

		Arguments:
		radius (I) Radius of *arc*.
	*/
	OdGeCircArc2d& setRadius(double radius)           { m_vRefVector.normalize(); m_vRefVector *= radius; return *this;}


	/** Description:
		Sets the starting and ending angles of this *arc*, and returns a reference to this *arc*.

		Arguments:
		startAngle (I) Starting angle of *arc* in radians.
		endAngle (I) Ending angle of *arc* in radians.
	*/
	OdGeCircArc2d& setAngles(double startAng, 
		double endAng) { m_dStartAngle = startAng; m_dInclAngle = endAng - startAng; return *this; }

	/** Description:
		Reverses the direction of this *arc* while maintaining its endpoints, and returns a reference to this *arc*.
	*/
	OdGeCircArc2d& setToComplement();

	/** Description:
		Sets the reference vector of this *arc*, and returns a reference to this *arc*.

		Arguments:
		refVec (I) The reference vector defining arc angle 0.
	*/
	OdGeCircArc2d& setRefVec(const OdGeVector2d& vec) { double rad = m_vRefVector.length(); m_vRefVector = vec; m_vRefVector *= rad; return *this; }
 
	/** Description:
		Sets the parameters for this *arc* according to the arguments, and returns a reference to this *arc*.

		Arguments:
		center (I) Center of *arc*.
		radius (I) Radius of *arc*.
		startAngle (I) Starting angle of *arc* in radians.
		endAngle (I) Ending angle of *arc* in radians.
		err(O) Error status for this method.
		refVec (I) The reference vector defining *arc* angle 0.
		isClockwise (I) If true, the *arc* is drawn clockwise, otherwise, counterclockwise.
		startPoint (I) Start point of *arc*.
		secondPoint (I) Second point on a 3-point *arc*.
		endPoint (I) End point of *arc*.
		bulge (I) Specifies the *bulge* of the *arc*.
		bulgeFlag (I) Specifies how *bulge* is to be interpreted.
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
	OdGeCircArc2d& set(const OdGePoint2d& center, 
		double radius);

	OdGeCircArc2d& set(const OdGePoint2d& center, 
		double radius, 
		double startAng, 
		double endAng,
		const OdGeVector2d& refVec = OdGeVector2d::kXAxis, 
		bool isClockWise = false);

	OdGeCircArc2d& set(const OdGePoint2d& startPoint, 
		const OdGePoint2d& secondPoint,
		const OdGePoint2d& endPoint);

	OdGeCircArc2d& set(const OdGePoint2d& startPoint,
		const OdGePoint2d& secondPoint,
		const OdGePoint2d& endPoint, 
		OdGeError& error);

	OdGeCircArc2d& set(const OdGePoint2d& startPoint, 
		const OdGePoint2d& endPoint, 
		double bulge, 
		bool bulgeFlag = true);

	OdGeCircArc2d& set(const OdGeCurve2d& curve1, 
		const OdGeCurve2d& curve2,
		double radius, 
		double& param1, 
		double& param2, 
		bool& success);
	OdGeCircArc2d& set(const OdGeCurve2d& curve1,
		const OdGeCurve2d& curve2, 
		const OdGeCurve2d& curve3,
		double& param1, 
		double& param2, 
		double& param3, 
		bool& success);

  //:>OdGeCircArc2d& operator =(const OdGeCircArc2d& arc);

	void getInterval (OdGeInterval& intrvl) const;

	virtual double paramOf (const OdGePoint2d& pnt,
		const OdGeTol& tol = OdGeContext::gTol) const;

	virtual OdGePoint2d evalPoint (double param) const;

	void appendSamplePoints(double fromParam, 
		double toParam,
		double approxEps, 
		OdGePoint2dArray& pointArray,
		OdGeDoubleArray* pParamArray = 0) const;

	 virtual OdGeEntity2d& transformBy(const OdGeMatrix2d& xfm);

	/** Description:
		Returns the geometric extents of this *arc*.

		Arguments:
		exts (O) Geometric extents.
	*/
	void getGeomExtents(OdGeExtents2d& exts);

	bool hasStartPoint(OdGePoint2d& startPnt) const;

	bool hasEndPoint(OdGePoint2d& endPnt) const;

	bool isClosed (const OdGeTol&) const;

  DD_USING(OdGeCurve2d::appendSamplePoints);

private:
  OdGePoint2d   m_pCenter;
  OdGeVector2d  m_vRefVector;

//  double        m_dRadius;

  double        m_dStartAngle;
  double        m_dInclAngle;

//  double        m_dRefAngle;
};

#include "DD_PackPop.h"

#endif // OD_GE_ARC2D_H


