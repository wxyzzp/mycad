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



#ifndef OD_GEELL3D_H
#define OD_GEELL3D_H

class OdGeEllipArc2d;
class OdGeCircArc3d;
class OdGeLineEnt3d;
class OdGePlanarEnt;
class OdGeExtents3d;

#include "GeCurve3d.h"
#include "OdPlatformSettings.h"

#include "DD_PackPush.h"

inline double angleFromParam (double parm, double ratio)
{
 if (OdZero (parm - Oda2PI))
 {
 return Oda2PI;
 }
 else
 {
 return (OD_ATAN2 (ratio*sin (parm), cos (parm)));
 }
}

inline double paramFromAngle (double angle, double ratio)
{
 if (OdZero (angle - Oda2PI))
 {
 return Oda2PI;
 }
 else
 {
 return (OD_ATAN2 (sin (angle), ratio*cos (angle)))+floor (angle/Oda2PI)*Oda2PI;
 }
}

// Implementation of commented functions hasn't high priority now
// Maybe them'll be implemented later.

/** Description:
	Represents 3D elliptical arcs and full ellipses in 3D space.

	Remarks:
	The angle of point on an ellipse is measured by projecting
	the point on the ellipse perpendicular to major axis onto a
	circle whose *center* is the *center* of the ellipse and whose
	radius is the major radius of the ellipse.

	The angle between the major axis of the ellipse, and a vector from
	the *center* of the ellipse to the intersection point with the circle, 
	measured counterclockwise about the crossproduct of the major and minor axes,
	is the angle of the point on the ellipse.

	{group:OdGe_Classes} 
*/
class GE_TOOLKIT_EXPORT OdGeEllipArc3d : public OdGeCurve3d
{
public:
	~OdGeEllipArc3d ();

	/** Arguments:
		center (I) The *center* of the *elliptical arc*.
		majorAxis (I) The major axis of the *elliptical arc*.
		minorAxis (I) The minor axis of the *elliptical arc*.
		majorRadius (I) The major radius of the *elliptical arc*.
		minorRadius (I) The minor radius of the *elliptical arc*.
		startAngle (I) Starting angle of the *elliptical arc* in radians.
		endAngle (I) Ending angle of the *elliptical arc* in radians.
	*/
	OdGeEllipArc3d ();

	OdGeEllipArc3d (const OdGeEllipArc3d& ell);

	OdGeEllipArc3d (const OdGeCircArc3d& arc);

	OdGeEllipArc3d (const OdGePoint3d& center, 
		const OdGeVector3d& majorAxis,
		const OdGeVector3d& minorAxis, 
		double majorRadius,
		double minorRadius);

	OdGeEllipArc3d (const OdGePoint3d& cent, 
		const OdGeVector3d& majorAxis,
		const OdGeVector3d& minorAxis, double majorRadius,
		double minorRadius, 
		double startAngle1, 
		double endAngle);
    
	/** Desciption:
		Returns the point on the ellipse and the point
		on the plane closest to the specified plane.

		Arguments:
		plane (I) Plane in question.
		pointOnPlane (O) Closes point on plane.
		tol (I) Tolerance for determining position.
	*/
	OdGePoint3d closestPointToPlane (const OdGePlanarEnt& plane,
		OdGePoint3d& pointOnPlane,
		const OdGeTol& = OdGeContext::gTol) const;

	OdGeEntity3d* copy () const;

	OdGe::EntityId type () const;

	bool isKindOf (OdGe::EntityId id) const;

	bool intersectWith (const OdGeLinearEnt3d& line, int& intn,
	OdGePoint3d& p1, OdGePoint3d& p2,
	const OdGeTol& tol = OdGeContext::gTol) const;

	bool intersectWith (const OdGePlanarEnt& plane, int& numOfIntersect,
	OdGePoint3d& p1, OdGePoint3d& p2,
	const OdGeTol& tol = OdGeContext::gTol) const;

		bool projIntersectWith (const OdGeLinearEnt3d& line,
		const OdGeVector3d& projDir, int &numInt,
		OdGePoint3d& pntOnEllipse1,
		OdGePoint3d& pntOnEllipse2,
		OdGePoint3d& pntOnLine1,
		OdGePoint3d& pntOnLine2,
		const OdGeTol& tol = OdGeContext::gTol) const;

	/** Description:
		Returns the *plane* of the ellipse.
	*/
	void getPlane (OdGePlane& plane) const;

	/** Description:
		Returns true if and only if the major and minor radii of the ellipse
		are the same withing the specified tolerance.

		Arguments:
		tol (I) Tolerance for determining equality.
	*/
	bool isCircular (const OdGeTol& tol = OdGeContext::gTol) const;

	/** Description:
		Returns true if and only if the specified point lies within
		the full ellipse, and on the plane of the ellipse.

		Arguments:
		pnt (I) Any 2D point.
		tol (I) Tolerance for determining position.
	*/
	bool isInside (const OdGePoint3d& pnt, const OdGeTol& tol = OdGeContext::gTol) const;

	/** Description:
		Returns the *center* of this *elliptical arc*.
	*/
	OdGePoint3d center () const; 

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
	OdGeVector3d minorAxis (const OdGeTol& tol = OdGeContext::gTol) const;
	OdGeVector3d minorAxis (const OdGeTol& tol, OdGeError& flag) const;

	/** Description:
		Returns the major axis of this *elliptical arc* as a unit vector.
	*/
	OdGeVector3d majorAxis (const OdGeTol& tol = OdGeContext::gTol) const;
	OdGeVector3d majorAxis (const OdGeTol& tol, OdGeError& flag) const;

	/** Description:
		Returns the normal to this *elliptical arc* as a unit vector. 
		
		Remarks:
		Positive angles are always drawn counterclockwise about this vector.
	*/
	OdGeVector3d normal (const OdGeTol& tol = OdGeContext::gTol) const;
	OdGeVector3d normal (const OdGeTol& tol, OdGeError& flag) const;

	/** Description:
		Returns start angle measured from major axis of this *elliptical arc* in the 
		plane of the arc.
	*/
	double startAng () const;

	/** Description:
		Returns end angle measured from major axis of this *elliptical arc* in the 
		plane of the arc.
	*/
	double endAng () const;

	/** Description:
		Returns the start point of this *elliptical arc*.
	*/
	OdGePoint3d startPoint () const;

	/** Description:
		Returns the end point of this *elliptical arc*.
	*/
	OdGePoint3d endPoint () const;

	/** Description:
		Sets the *center* of the *elliptical arc*.

		Arguments:
		center (I) The *center* of the *elliptical arc*.
	*/
	OdGeEllipArc3d& setCenter (const OdGePoint3d& cent);

	/** Description:
		Sets the minor radius of the *elliptical arc*.

		Arguments:
		minorRadius (I) The minor radius of the *elliptical arc*.
	*/
	OdGeEllipArc3d& setMinorRadius (double rad);

	/** Description:
		Sets the major radius of the *elliptical arc*.

		Arguments:
		majorRadius (I) The major radius of the *elliptical arc*.
	*/
	OdGeEllipArc3d& setMajorRadius (double rad);

	/** Description:
		Sets the major and minor axes of the *elliptical arc*.

		Arguments:
		majorAxis (I) The major axis of the *elliptical arc*.
		minorAxis (I) The minor axis of the *elliptical arc*.
	*/
	OdGeEllipArc3d& setAxes (const OdGeVector3d& majorAxis,
        const OdGeVector3d& minorAxis);

	/** Description:
		Sets the starting and ending angles of the *elliptical arc*.

		Arguments:
		startAngle (I) Starting angle of the *elliptical arc* in radians.
		endAngle (I) Ending angle of the *elliptical arc* in radians.
	*/
	OdGeEllipArc3d& setAngles (double startAngle, 
		double endAngle);

	/** Description:
		Sets the parameters for this *elliptical arc* according to the arguments, and returns a reference to this *elliptical arc*.

		Arguments:
		arc (I) Any 2D circlular arc.
		center (I) The *center* of the *elliptical arc*.
		majorAxis (I) The major axis of the *elliptical arc*.
		minorAxis (I) The minor axis of the *elliptical arc*.
		majorRadius (I) The major radius of the *elliptical arc*.
		minorRadius (I) The minor radius of the *elliptical arc*.
		startAngle (I) Starting angle of the *elliptical arc* in radians.
		endAngle (I) Ending angle of the *elliptical arc* in radians.
	*/
	OdGeEllipArc3d& set (const OdGePoint3d& cent,
		const OdGeVector3d& majorAxis,
		const OdGeVector3d& minorAxis,
		double majorRadius, 
		double minorRadius);

	OdGeEllipArc3d& set (const OdGePoint3d& cent,
		const OdGeVector3d& majorAxis,
		const OdGeVector3d& minorAxis,
		double majorRadius, 
		double minorRadius,
		double startAngle, 
		double endAngle);

	OdGeEllipArc3d& set (const OdGeCircArc3d&);

	void appendSamplePoints (double fromParam, 
		double toParam, 
		double approxEps,
		OdGePoint3dArray& pointArray, 
		OdGeDoubleArray* pParamArray = 0) const;
		DD_USING (OdGeCurve3d::appendSamplePoints);

	OdGeEntity3d& transformBy (const OdGeMatrix3d& xfm);

	// functions making ellipse's axis orthogonal
	// this one requires error flag

	/** Desscription:
		Makes the minor axis orthogonal to the major axis.

    Arguments:
		tol (I) Tolerance for determining position.
		flag (O) Status of computation.

    Remarks:
		If flag is not present, this function throws
		an exception.

		Possible values for flag:

		o kOk
		o kNonCoplanarGeometry
	*/
	void correctAxis (const OdGeTol& tol, OdGe::ErrorCondition& flag);
	void correctAxis (const OdGeTol& tol = OdGeContext::gTol);

	OdGePoint3d evalPoint (double param) const;

	OdGePoint3d evalPoint (double param, 
		int numDeriv,
		OdGeVector3dArray& derivArray) const;

	/** Description:
		Returns the *tangent vector* as the point specified by param.

		Arguments:
		param (I) Parameter corresponding to the point at which to compute the *tangent*
	*/
	OdGeVector3d tangentAt (double param) const;
	
	double paramOf (const OdGePoint3d& pnt, 
		const OdGeTol& tol = OdGeContext::gTol) const;

	void getInterval (OdGeInterval&) const;

	bool setInterval (const OdGeInterval& intrvl);
		DD_USING (OdGeCurve3d::setInterval);

	OdGeCurve3d& reverseParam ();
		
	/** Description:
		Projects this *elliptical arc* onto the specified plane along the specified direction.

		Arguments:
		projectionPlane (I) Plane on which this *elliptical arc* is to be projected
		projectDirection (I) Vector defining the projection direction.
		tol (I) Tolerance for determining projection.

		Remarks:
		The returned entity might not be the same *type* as the original one.

		The returned entity is created with the new operator and must be deleted by the caller. 
	*/
	virtual OdGeEntity3d* project (const OdGePlane& projectionPlane, 
		const OdGeVector3d& projectDirection,
		const OdGeTol& tol = OdGeContext::gTol) const;

	/** Description:
		Projects this *elliptical arc* onto the specified plane along a vector perpendicular to the specified plane.

		Arguments:
		projectionPlane (I) Plane on which this *elliptical arc* is to be projected
		tol (I) Tolerance for determining projection.

		Remarks:
		The returned entity might not be the same *type* as the original one.

		The returned entity is created with the new operator and must be deleted by the caller. 
	*/
	virtual OdGeEntity3d* orthoProject (const OdGePlane& projectionPlane,
		const OdGeTol& tol = OdGeContext::gTol) const;


	virtual bool isClosed (const OdGeTol& tol = OdGeContext::gTol) const;

	/** Description:
		Returns the geometric extents of this *elliptical arc*.

		Arguments:
		exts (O) Geometric extents.
	*/
	void getGeomExtents (OdGeExtents3d& exts) const;

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
	OdResult inverseTangent (OdGeVector3d tan, OdGeDoubleArray& params) const;

	bool hasStartPoint (OdGePoint3d& startPnt) const;

	bool hasEndPoint (OdGePoint3d& endPnt) const;

  virtual bool isOn (const OdGePoint3d& pnt, const OdGeTol& tol = OdGeContext::gTol) const;
protected:
 OdGePoint3d m_center;
 OdGeVector3d m_majorAxis, m_minorAxis;
 double m_startParam, m_includedParam;
};

#include "DD_PackPop.h"

#endif // OD_GEELL3D_H


