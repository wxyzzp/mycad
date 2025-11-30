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



#ifndef _OD_GECURVE3D_H_
#define _OD_GECURVE3D_H_

#include "GeEntity3d.h"
#include "GeDoubleArray.h"
#include "GeVector3dArray.h"
#include "GeInterval.h"

class OdGeCurve2d;
class OdGeSurface;
class OdGePoint3d;
class OdGePlane;
class OdGeVector3d;
class OdGeLinearEnt3d;
class OdGeLine3d;
class OdGePointOnCurve3d;
class OdGePointOnSurface;
class OdGeMatrix3d;
class OdGePointOnCurve3dData;
class OdGeBoundBlock3d;


#include "DD_PackPush.h"

/** Description: 
	The base class for all OdGe 3D curves.
    
    {group:OdGe_Classes} 
*/
class GE_TOOLKIT_EXPORT OdGeCurve3d : public OdGeEntity3d
{
public:
	/** Description:
		Returns the parametric interval of the curve.

		Arguments:
		intrvl (O) Parametric interval of the curve.
	*/ 
	virtual void getInterval(OdGeInterval& intervl) const = 0;

	//	virtual void getInterval(OdGeInterval& intrvl, OdGePoint3d& start, OdGePoint3d& end) const = 0;

	/** Description:
		Sets the parametric interval for the *curve*.

		Arguments:
		intrvl (I) Parametric interval of the curve
	*/ 	
	virtual bool setInterval(const OdGeInterval& /*intrvl*/) { ODA_FAIL(); return false; }

	/** Description:
		Reverses the parameter direction the curve.	

		Remarks:
		The point *set* of the curve is unchanged.
   */
	virtual OdGeCurve3d& reverseParam() { ODA_FAIL(); return *this; }


//	virtual OdGeCurve3d& setInterval() { ODA_FAIL(); return *this; }
	/** Description:
		Returns the distance to the point on this *curve* closest to the specified point or *curve*.

		Arguments:
		pnt (I) Any 3D point.
		curve (I) Any 3D *curve*.
		tol (I) Tolerance for determining distance.
	*/
	double distanceTo(const OdGePoint3d& pnt, 
		const OdGeTol& tol = OdGeContext::gTol) const;

	double distanceTo(const OdGeCurve3d& curve, 
		const OdGeTol& tol = OdGeContext::gTol) const;
	
	/** Description:
		Returns the point on this *curve* closest to the specified point or *curve*, and the point
		on the other *curve* closest to this *curve*.

		Arguments:
		pnt (I) Any 3D point.
		*curve* (I) Any 3D *curve*.
		pntOnOtherCrv (O) Closest point on other *curve*.
		tol (I) Tolerance for determining distance.
	*/
	OdGePoint3d closestPointTo(const OdGePoint3d& pnt, 
		const OdGeTol& tol = OdGeContext::gTol) const;

	OdGePoint3d closestPointTo(const OdGeCurve3d& curve, 
		OdGePoint3d& pntOnOtherCrv, 
		const OdGeTol& tol = OdGeContext::gTol) const;
	
	/** Description:
		Returns the point on this curve closest to the specified point or curve, 
		and the point on the other curve closest to this curve.

		Arguments:
		pnt (I) Any 3D point.
		curve (I) Any 3D curve.
		pntOnCrv (O) Closest point on this curve.
		pntOnOtherCrv (O) Closest point on other curve.
		tol (I) Tolerance for determining distance.
	*/
	void getClosestPointTo(const OdGePoint3d& pnt, 
		OdGePointOnCurve3d& pntOnCrv, 
		const OdGeTol& tol = OdGeContext::gTol) const;

	void getClosestPointTo(const OdGeCurve3d& curve, 
		OdGePointOnCurve3d& pntOnThisCrv, 
		OdGePointOnCurve3d& pntOnOtherCrv, 
		const OdGeTol& tol = OdGeContext::gTol) const;
	
	/** Description:
		Returns the point on this *curve* closest to the specified point or *curve*, 
		and the point on the other *curve* closest to this *curve*, when this *curve*
		is projected in the specified direction.

		Arguments:
		pnt (I) Any 3D point.
		curve (I) Any 3D *curve*.
		projectDirection (I) Projection Direction.
		pntOnOtherCrv (O) Closest point on other *curve*.
		tol (I) Tolerance for determining points.
	*/
	OdGePoint3d projClosestPointTo(const OdGePoint3d& pnt, 
		const OdGeVector3d& projectDirection, 
		const OdGeTol& tol = OdGeContext::gTol) const;

	OdGePoint3d projClosestPointTo(const OdGeCurve3d& curve, 
		const OdGeVector3d& projectDirection, 
		OdGePoint3d& pntOnOtherCrv, 
		const OdGeTol& tol = OdGeContext::gTol) const;
	
	/** Description:
		Returns the point on this curve closest to the specified point or curve, 
		and the point on the other curve closest to this curve, when this curve
		is projected in the specified direction.

		Arguments:
		pnt (I) Any 3D point.
		curve (I) Any 3D curve.
		projectDirection (I) Projection Direction.
		pntOnCrv (O) Closest point on this curve.
		pntOnOtherCrv (O) Closest point on other curve.
		tol (I) Tolerance for determining distance.
	*/
	void getProjClosestPointTo(const OdGePoint3d& pnt, 
		const OdGeVector3d& projectDirection, 
		OdGePointOnCurve3d& pntOnCrv, 
		const OdGeTol& tol = OdGeContext::gTol) const;

	void getProjClosestPointTo(const OdGeCurve3d& curve3d,
		const OdGeVector3d& projectDirection,
		OdGePointOnCurve3d& pntOnThisCrv,
		OdGePointOnCurve3d& pntOnOtherCrv,
		const OdGeTol& tol = OdGeContext::gTol) const;
	
	/** Description:
		Returns true, and the point on this curve whose normal vector passes through the specified point,
		if and only if a normal point was found.

		Arguments:
		pnt (I) Any 3D point.
		pntOnCrv (O) Normal point.
		tol (I) Tolerance for determining points.
	*/
	bool getNormalPoint(const OdGePoint3d& pnt, 
		OdGePointOnCurve3d& pntOnCrv, 
		const OdGeTol& tol = OdGeContext::gTol) const;
	
	/**	Description
		Returns the oriented bounding box of curve.
	*/
	OdGeBoundBlock3d boundBlock() const;

	/**	Arguments:
		range (I) Sub-interval of the curve to be bounded.
	*/
	OdGeBoundBlock3d boundBlock(const OdGeInterval& range) const;
	
	/** Description:
		Returns the bounding box whose edges are aligned with the coordinate axes.

		Arguments:
		range (I) Interval of the curve to be bounded.
	*/
	OdGeBoundBlock3d orthoBoundBlock() const;

	OdGeBoundBlock3d orthoBoundBlock(const OdGeInterval& range) const;
	
 /*
	OdGeEntity3d* project(const OdGePlane& projectionPlane,
		const OdGeVector3d& projectDirection,
		const OdGeTol& tol = OdGeContext::gTol) const;
	OdGeEntity3d* orthoProject(const OdGePlane& projectionPlane,
		const OdGeTol& tol = OdGeContext::gTol) const;
 */
	
	// Tests if point is on curve.
	//
	/*bool isOn(const OdGePoint3d& pnt, const OdGeTol& tol = OdGeContext::gTol) const;
	bool isOn(const OdGePoint3d& pnt, double& param,
		const OdGeTol& tol = OdGeContext::gTol) const;
	bool isOn(double param, const OdGeTol& tol = OdGeContext::gTol) const;*/
	
	/** Description:
		Returns the parameter value of a point.
		
		Arguments:
		pnt (I) Point to be evaluated.
		tol (I) Tolerance for determining parameters.

		Remarks:
		The returned parameters specify a point withing tol of pnt.
		If pnt is not on the curve, the results are unpredictable.
		If you are not sure the point is on the curve, use 
		OdGeCurve3d::isOn() instead of this function.
	*/
	virtual double paramOf(const OdGePoint3d& , const OdGeTol& /*tol*/ = OdGeContext::gTol) const {return 0; }

	/** Description:
		Returns one or more trimmed offset curves.

		Arguments:
		distance (I) Offset *distance*.
		planeNormal (I) A normal to the place of the curve.
		offsetCurveList (O) An array of trimmed offset curves.
		extensionType (I) How curves will be extended at discontinuities of *type* C1
		tol (I) Tolerance for determining points.
		
		Remarks:
		The offsets are trimmed to eliminate self-intersecting loops.

		The curve is assumed to be planar, and planeNomal is assumed to be 
		normal to the curveplane.

		The direction of positive offset at any point on the curve
		is the cross product of planeNormal and the tangent to the
		curve at that point..

		The new operator is used to create the curve returned by 
		offsetCurveList. It is up to the caller to delete these curves. 
	*/
	void getTrimmedOffset(double distance, 
		const OdGeVector3d& planeNormal, 
		OdArray<void*>& offsetCurveList, 
		OdGe::OffsetCrvExtType extensionType = OdGe::kFillet, 
		const OdGeTol& tol = OdGeContext::gTol) const;
	
	/** Description:
		Returns true if and only if the curve is closed within the specified tolerance;

		Arguments:
		tol (I) Tolerance for determining closure
	*/
	virtual bool isClosed(const OdGeTol& tol = OdGeContext::gTol) const;

	/** Description:
		Returns true if and only if this curve is planar, and returns
		the *plane* of the curve.
		
		Arguments:
		plane (O) The *plane* of the curve.
		tol (I) Tolerance for determining planeness.

		Remarks:
		Lines are considered planar; the returned *plane* is an
		arbitrary *plane* containing the line.
	*/
	bool isPlanar(OdGePlane& plane,	
		const OdGeTol& tol = OdGeContext::gTol) const;

	/** Description:
		Returns true, and a *line* coincident with the curve,
		if and only if the curve is linear.

		Arguments:
		line (O) The *line* coincident with the curve.
		tol (I) Tolerance for determining lineness.

		Remarks:
		An infinite *line* is returned, even if the curve is bounded.
	*/
	bool isLinear(OdGeLine3d& line,
		const OdGeTol& tol = OdGeContext::gTol) const;

	/** Description:
		Returns true if and only if the specified *curve*
		is coplanar with this *curve*, and returns the common *plane*.

		Arguments:
		curve (I) Any 3D *curve*.
		plane (O) The *plane* of the curves.
		tol (I) Tolerance for determining planeness.
	*/
	bool isCoplanarWith(const OdGeCurve3d& curve, 
		OdGePlane& plane, 
		const OdGeTol& tol = OdGeContext::gTol) const;

	/** Description:
		Returns true if and only if this curve is periodic for the specified *period*.
		
		Arguments:
		period (O) Period of curve param 
	*/
	virtual bool isPeriodic(double& /*period*/) const { return false; }
	
	/** Description:
		Returns the length of the curve over the specified parameter range.

		Arguments:
		fromParam (I) Starting Parameter Value.
		toParam (I) Ending Parameter Value.
		tol (I) Tolerance for determining points.
	*/	
	double length(double fromParam, 
		double toParam, 
		double tol = OdGeContext::gTol.equalPoint()) const;

	/** Description:
		Returns the parameter of the point a specified distance 
		from the starting point corresponding to dataParam.

		Arguments:
		dataParam (I) Parameter corresponding to the start point.
		length (I) Distance along curve from the start point.
		posParamDir (I) True if and only if returned parameter is to be greater than dataParam.
		tol (I) Tolerance for determining points.
	*/	
	double paramAtLength(double datumParam, 
		double length, 
		bool posParamDir = true, 
		double tol = OdGeContext::gTol.equalPoint()) const;

	/** Description:
		Determines the *area(of the curve between the specified parameters. 
		Returns true (and a value) if and only if the curve is planar. 

		Arguments:
		startParam (I) Starting Parameter Value.
		endParam (I) Ending Parameter Value.
		value (O) Area
		tol (I) Tolerance for determining points.
	*/	
	bool area(double startParam, 
		double endParam, 
		double& value, 
		const OdGeTol& tol = OdGeContext::gTol) const;
	
	/** Description:
		Returns true if and only if the curve degenerates, and returns
		the entity or type of entity to which this curve degenerates.
		
		Arguments:
		degenerateType (O) The *type* of curve to which this curve degenerates.
		pConvertedEntity (O) A pointer to the object to which this curve degenerates.
		tol (I) Tolerance for determining points.

		Remarks:
		If isDegenerate returns true, the degenerate 
		object was created with the new operator, 
		and caller must delete this object. 
	*/	
	bool isDegenerate(OdGe::EntityId& degenerateType, 
		const OdGeTol& tol = OdGeContext::gTol) const;

	bool isDegenerate(OdGeEntity3d*& pConvertedEntity, 
		const OdGeTol& tol = OdGeContext::gTol) const;
	
	/** Description:
		Returns pointers to the two curves that result from splitting this curve at the point corresponding 
		to param.

		Arguments:
		param (I) The parameter value corresponding to the split point.
		piece1 (O) Pointer to the first curve.
		piece2 (O) Pointer to the second curve.

		Remarks:
		If param corresponds to a point not on the curve,
		or to the start point or end point of the curve,
		piece1 and piece2 are set to NULL.

		The curves pointed to by piece1 and piece2 
		are created with the new command, and must be deleted by the caller. 
	*/	
	void getSplitCurves(double param, 
		OdGeCurve3d* & piece1,
		OdGeCurve3d* & piece2) const;
	
	/** Description:
		Explodes this curve over the specified interval.

		Arguments:
		explodedCurves (O) An array of pointers to the debris from the explosion.
		newExplodedCurves (O) An array of flags which, if true, correspond those explodedCurves the caller must delete.
		intrvl (I) Interval to be exploded. Defaults to entire curve.

		Remarks:
		The original curve is not changed.
	*/	
	bool explode(OdArray<void*>& explodedCurves, 
		OdArray<int>& newExplodedCurves, 
		const OdGeInterval* intrvl = NULL) const;
	
	/** Description:
		Returns the *point* on this *curve* locally closest to the specified *point*
		or *curve*, and the *point* on the other *curve* locally closest to this *curve*.

		Arguments:
		pnt (I) Any 3D *point*.
		curve (I) Any 3D *curve*.
		approxPntOnThisCrv (I/O) Approximate *point* on this *curve*.
		approxPntOnOtherCrv (I/O) Approximate *point* on other *curve*.
		nbhd1 (I) The *point* on this *curve* must lie in this interval.
		nbhd2 (I) The *point* on the other *curve* must lie in this interval.
		tol (I) Tolerance for determining *point*s.
	*/	
	void getLocalClosestPoints(const OdGePoint3d& point,
		OdGePointOnCurve3d& approxPntOnThisCrv,
		const OdGeInterval* nbhd1 = 0,
		const OdGeTol& tol = OdGeContext::gTol) const;

	void getLocalClosestPoints(const OdGeCurve3d& curve,
		OdGePointOnCurve3d& approxPntOnThisCrv,
		OdGePointOnCurve3d& approxPntOnOtherCrv,
		const OdGeInterval* nbhd1 = 0,
		const OdGeInterval* nbhd2 = 0,
		const OdGeTol& tol = OdGeContext::gTol) const;
	
  	/** Description:
		Returns true if and only if the parametric interval of the curve has a lower bound,
		and returns the start point of the curve.

		Arguments:
		startPoint (O) Start point.
	*/
	virtual bool hasStartPoint(OdGePoint3d& startPnt) const = 0;

	/** Description:
		Returns true if and only if the parametric interval of the curve has an upper bound,
		and returns the end point of the curve.

		Arguments:
		endPoint (O) End point.
	*/
	virtual bool hasEndPoint  (OdGePoint3d& endPnt) const = 0;
	
  	/** Description:
		Returns the point on the curve corresponding to the given parameter value.

		Arguments:
		param (I) Parameter value.
	*/
	virtual OdGePoint3d evalPoint(double param) const = 0;

	// virtual OdGePoint3d evalPoint(double param, int numDeriv, OdGeVector3dArray& derivArray) const = 0;
	
	/** Description:
		Appends sample points along this curve and their parameter values to the specified arrays.

		Arguments:
		fromParam (I) Starting Parameter Value.
		toParam (I) Ending Parameter Value.
    paramInterval (I) Starting & Ending Parameters in GeInterval form.
                      if pointer is NULL, will be used current curve interval.
		approxEps (I) Approximate spacing along a curve.
		numSample (I) Number of samples.
		pointArray (O) Sample points.
		pParamArray (O) Parameter at each point.
	*/
	virtual void appendSamplePoints(double fromParam, 
		double toParam, 
		double approxEps, 
		OdGePoint3dArray& pointArray, 
		OdGeDoubleArray* pParamArray = 0) const = 0;

	virtual void appendSamplePoints(int numSample, 
		OdGePoint3dArray& 
		pointArray) const;

  void appendSamplePoints(const OdGeInterval *paramInterval, 
    double approxEps, 
		OdGePoint3dArray& pointArray, 
    OdGeDoubleArray* pParamArray = 0) const;

  /** Description:
		Returns sample points along this curve and their parameter values in the specified arrays.

		Arguments:
		fromParam (I) Starting Parameter Value.
		toParam (I) Ending Parameter Value.
    paramInterval (I) Starting & Ending Parameters in GeInterval form.
                      if pointer is NULL, will be used current curve interval.
		approxEps (I) Approximate spacing along a curve.
		numSample (I) Number of samples.
		pointArray (O) Sample points.
		pParamArray (O) Parameter at each point.
	*/
	void getSamplePoints(double fromParam, 
		double toParam, 
		double approxEps, 
		OdGePoint3dArray& pointArray, 
		OdGeDoubleArray* pParamArray = 0) const;

  void getSamplePoints(const OdGeInterval *paramInterval,
    double approxEps, 
	  OdGePoint3dArray& pointArray,
    OdGeDoubleArray* pParamArray = 0) const;

  void getSamplePoints(int numSample, 
		OdGePoint3dArray& pointArray) const;

	// Assignment operator.
	//
	// OdGeCurve3d&   operator=(const OdGeCurve3d& curve);
	
protected:
	OdGeCurve3d() {}
	OdGeCurve3d(const OdGeCurve3d&) {}
};

inline void OdGeCurve3d::getSamplePoints(double fromParam, double toParam, double approxEps, 
	OdGePoint3dArray& pointArray, OdGeDoubleArray* pParamArray) const
{
	pointArray.clear();
	if(pParamArray)
		pParamArray->clear();
	appendSamplePoints(fromParam, toParam, approxEps, pointArray, pParamArray);
}

inline void OdGeCurve3d::getSamplePoints(int numSample, OdGePoint3dArray& pointArray) const
{
	pointArray.clear();
	appendSamplePoints(numSample, pointArray);
}

inline void OdGeCurve3d::getSamplePoints(const OdGeInterval *paramInterval, double approxEps, 
	OdGePoint3dArray& pointArray, OdGeDoubleArray* pParamArray) const
{
  OdGeInterval i;

  if (!paramInterval)
  {
    getInterval(i);
    paramInterval = &i;
  }

  if (paramInterval->isBounded())
  {
    getSamplePoints(paramInterval->lowerBound(), paramInterval->upperBound(), approxEps, pointArray, pParamArray);
  }
}

inline void OdGeCurve3d::appendSamplePoints(const OdGeInterval *paramInterval, double approxEps, 
		OdGePoint3dArray& pointArray, OdGeDoubleArray* pParamArray) const
{
  OdGeInterval i;

  if (!paramInterval)
  {
    getInterval(i);
    paramInterval = &i;
  }

  if (paramInterval->isBounded())
  {
    appendSamplePoints(paramInterval->lowerBound(), paramInterval->upperBound(), approxEps, pointArray, pParamArray);
  }
}

inline bool setInterval(const OdGeInterval&) { ODA_FAIL(); return false; }

#include "DD_PackPop.h"

#endif


