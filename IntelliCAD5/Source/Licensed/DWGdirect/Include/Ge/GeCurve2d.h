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
// programs incorporating this software must include the following statement 
// with their copyright notices:
//
//      DWGdirect © 2002 by Open Design Alliance Inc. All rights reserved. 
//
// By use of this software, you acknowledge and accept the terms of this 
// agreement.
///////////////////////////////////////////////////////////////////////////////



#ifndef OD_GE_CURVE_2D_H
#define OD_GE_CURVE_2D_H


class OdGePoint2d;
class OdGeVector2d;
class OdGePointOnCurve2d;
class OdGeInterval;
class OdGeMatrix2d;
class OdGeLine2d;
class OdGePointOnCurve2dData;
class OdGeBoundBlock2d;

#include "GeEntity2d.h"
#include "GeIntArray.h"
#include "GePoint2dArray.h"
#include "GeVector2dArray.h"
#include "GeVoidPointerArray.h"
#include "GeDoubleArray.h"

#include "DD_PackPush.h"

/** Description:
    The base class for all OdGe 2D curves.

    {group:OdGe_Classes} 
*/
class GE_TOOLKIT_EXPORT OdGeCurve2d : public OdGeEntity2d
{
public:

	/** Description:
		Returns the parametric *interval* of the curve.

		Arguments:
		intrvl (O) Parametric *interval* of the curve
	*/ 
	virtual void getInterval(OdGeInterval& intrvl) const = 0;

//  virtual void getInterval(OdGeInterval& intrvl, OdGePoint2d& start,
//    OdGePoint2d& end) const = 0;
//  virtual OdGeCurve2d& reverseParam() = 0;
//  virtual OdGeCurve2d& setInterval() = 0;
//  virtual bool setInterval(const OdGeInterval& intrvl) = 0;
  
//  virtual double distanceTo(const OdGePoint2d& pnt,
//    const OdGeTol& = OdGeContext::gTol) const = 0;
//  virtual double distanceTo(const OdGeCurve2d&,
//    const OdGeTol& tol = OdGeContext::gTol) const = 0;
  
//  virtual OdGePoint2d closestPointTo(const OdGePoint2d& pnt,
//    const OdGeTol& tol = OdGeContext::gTol) const = 0;
//  virtual OdGePoint2d closestPointTo(const OdGeCurve2d& curve2d,
//    OdGePoint2d& pntOnOtherCrv,
//    const OdGeTol& tol= OdGeContext::gTol) const = 0;
    
//  virtual void getClosestPointTo(const OdGePoint2d& pnt,
//    OdGePointOnCurve2d& pntOnCrv,
//    const OdGeTol& tol = OdGeContext::gTol) const = 0;
//  virtual void getClosestPointTo(const OdGeCurve2d& curve2d,
//    OdGePointOnCurve2d& pntOnThisCrv,
//    OdGePointOnCurve2d& pntOnOtherCrv,
//    const OdGeTol& tol = OdGeContext::gTol) const = 0;
  
//  virtual bool getNormalPoint (const OdGePoint2d& pnt,
//    OdGePointOnCurve2d& pntOnCrv,
//    const OdGeTol& tol = OdGeContext::gTol) const = 0;
  
//  virtual bool isOn(const OdGePoint2d& pnt, const OdGeTol& tol = OdGeContext::gTol) const = 0;

//  virtual bool isOn(const OdGePoint2d& pnt, double& param,
//    const OdGeTol& tol = OdGeContext::gTol) const = 0;

//  virtual bool isOn(double param,
//    const OdGeTol& tol = OdGeContext::gTol) const = 0;

	/** Description:
		Returns the parameter value of a point.
		
		Arguments:
		pnt (I) Point to be evaluated.
		tol (I) Tolerance for determining parameters.

		Remarks:
		The returned parameters specify a point withing tol of pnt.
		If pnt is not on the curve, the results are unpredictable.
		If you are not sure the point is on the curve, use 
		OdGeCurve2d::isOn() instead of this function.
	*/
	virtual double paramOf(const OdGePoint2d& pnt,
		const OdGeTol& tol = OdGeContext::gTol) const = 0;
  
//  virtual void getTrimmedOffset (double distance,
//    OdGeVoidPointerArray& offsetCurveList,
//    OdGe::OffsetCrvExtType extensionType = OdGe::kFillet,
//    const OdGeTol& = OdGeContext::gTol) const = 0;
  
	/** Description:
		Returns true if and only if the curve is closed within the specified tolerance.

		Arguments:
		tol (I) Tolerance for determining closure
	*/
	virtual bool isClosed (const OdGeTol& tol = OdGeContext::gTol) const = 0;

//  virtual bool isPeriodic(double& period) const = 0;
//  virtual bool isLinear (OdGeLine2d& line, const OdGeTol& tol = OdGeContext::gTol) const = 0;
  
//  virtual double length (double fromParam, double toParam,
//    double tol = OdGeContext::gTol.equalPoint()) const = 0;

//  virtual double paramAtLength(double datumParam, double length,
//    bool posParamDir = true, double tol = OdGeContext::gTol.equalPoint()) const = 0;

//  virtual bool area (double startParam, double endParam,
//    double& value, const OdGeTol& tol = OdGeContext::gTol) const = 0;
  
//  virtual bool isDegenerate(OdGe::EntityId& degenerateType,
//    const OdGeTol& tol = OdGeContext::gTol) const = 0;

//  virtual bool isDegenerate(OdGeEntity2d*& pConvertedEntity,
//    const OdGeTol& tol = OdGeContext::gTol) const = 0;
  
//  virtual void getSplitCurves (double param, OdGeCurve2d* & piece1,
//    OdGeCurve2d* & piece2) const = 0;
  
//  virtual bool explode (OdGeVoidPointerArray& explodedCurves,
//    OdGeIntArray& newExplodedCurve,
//    const OdGeInterval* intrvl = NULL) const = 0;
  
//  virtual void getLocalClosestPoints(const OdGePoint2d& point,
//    OdGePointOnCurve2d& approxPnt,
//    const OdGeInterval* nbhd = 0,
//    const OdGeTol& = OdGeContext::gTol) const = 0;
//  virtual void getLocalClosestPoints(const OdGeCurve2d& otherCurve,
//    OdGePointOnCurve2d& approxPntOnThisCrv,
//    OdGePointOnCurve2d& approxPntOnOtherCrv,
//    const OdGeInterval* nbhd1 = 0,
//    const OdGeInterval* nbhd2 = 0,
//    const OdGeTol& tol = OdGeContext::gTol) const = 0;
  
//  virtual OdGeBoundBlock2d boundBlock() const = 0;
//  virtual OdGeBoundBlock2d boundBlock(const OdGeInterval& range) const = 0;
  
//  virtual OdGeBoundBlock2d orthoBoundBlock() const = 0;
//  virtual OdGeBoundBlock2d orthoBoundBlock(const OdGeInterval& range) const = 0;

  	/** Description:
		Returns true if and only if the parametric *interval* of the curve has a lower bound.

		Arguments:
		startPoint (O) Start point.
	*/
	 virtual bool hasStartPoint(OdGePoint2d& startPoint) const = 0;

	/** Description:
		Returns true if and only if the parametric *interval* of the curve has an upper bound.

		Arguments:
		endPoint (O) End point.
	*/
	virtual bool hasEndPoint (OdGePoint2d& endPoint) const = 0;


  	/** Description:
		Returns the point on the curve corresponding to the given parameter value.

		Arguments:
		param (I) Parameter value.
	*/
	virtual OdGePoint2d evalPoint(double param) const = 0;

  //OdGePoint2d evalPoint(double param, int numDeriv,
  //  OdGeVector2dArray& derivArray) const;
  
	/** Description:
		Appends sample points along this curve and their parameter values to the specified arrays.

		Arguments:
		fromParam (I) Starting Parameter Value.
		toParam (I) Ending Parameter Value.
		approxEps (I) Approximate spacing along a curve.
		numSample (I) Number of samples.
		pointArray (O) Sample points.
		pParamArray (O) Parameter at each point.
	*/
	virtual void appendSamplePoints(double fromParam, 
		double toParam, 
		double approxEps, 
		OdGePoint2dArray& pointArray, 
		OdGeDoubleArray* pParamArray = 0) const = 0;

	virtual void appendSamplePoints(int numSample, 
		OdGePoint2dArray& pointArray) const;

	/** Description:
		Returns sample points along this curve and their parameter values in the specified arrays.

		Arguments:
		fromParam (I) Starting Parameter Value.
		toParam (I) Ending Parameter Value.
		approxEps (I) Approximate spacing along a curve.
		pointArray (O) Sample points.
		pParamArray (O) Parameter at each point.
	*/
    void getSamplePoints(double fromParam, 
		double toParam, 
		double approxEps, 
		OdGePoint2dArray& pointArray, 
		OdGeDoubleArray* pParamArray = 0) const;
  
  //:>OdGeCurve2d& operator = (const OdGeCurve2d& curve);
  
protected:
	OdGeCurve2d  () {}
	OdGeCurve2d (const OdGeCurve2d& curve2d);
};


inline void OdGeCurve2d::getSamplePoints(double fromParam, double toParam, double approxEps, 
		OdGePoint2dArray& pointArray, OdGeDoubleArray* pParamArray) const
{
  pointArray.clear();
  if(pParamArray)
    pParamArray->clear();
  appendSamplePoints(fromParam, toParam, approxEps, pointArray, pParamArray);
}

#include "DD_PackPop.h"

#endif // OD_GE_CURVE_2D_H


