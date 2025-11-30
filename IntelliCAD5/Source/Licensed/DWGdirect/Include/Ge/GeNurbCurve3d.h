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



#ifndef OD_GENURB3D_H
#define OD_GENURB3D_H

class OdGeEllipArc3d;
class OdGeLineSeg3d;
class OdGeKnotVector;

#include "GeSplineEnt3d.h"
#include "OdPlatformSettings.h"

class OdGeNurbCurve3dImpl;

#include "DD_PackPush.h"

/** Description:
    
    {group:OdGe_Classes} 
*/
class GE_TOOLKIT_EXPORT OdGeNurbCurve3d : public OdGeSplineEnt3d
{
public:

  // Construction/destruction
  virtual ~OdGeNurbCurve3d();

  OdGeNurbCurve3d();

  OdGeNurbCurve3d(const OdGeNurbCurve3d& src);

  OdGeNurbCurve3d(
    int degree,
    const OdGeKnotVector& knots,
    const OdGePoint3dArray& cntrlPnts,
    bool isPeriodic = false);

  OdGeNurbCurve3d(int degree,
    const OdGeKnotVector& knots,
    const OdGePoint3dArray& cntrlPnts,
    const OdGeDoubleArray& weights,
    bool isPeriodic = false);

  OdGeNurbCurve3d(int degree,
    const OdGeKnotVector& knots,
    const OdGePoint3d* cntrlPnts,
    OdUInt32 cntrlPntsSize,
    const double* weights,
    OdUInt32 weightsSize,
    bool isPeriodic = false);

  /* Not implemented yet
  OdGeNurbCurve3d(int degree, const OdGePolyline3d& fitPolyline,
    bool isPeriodic = false); */

  OdGeNurbCurve3d(const OdGePoint3dArray& fitPoints,
    const OdGeVector3d& startTangent,
    const OdGeVector3d& endTangent,
    bool startTangentDefined = true,
    bool endTangentDefined = true,
    const OdGeTol& fitTolerance = OdGeContext::gTol);

  OdGeNurbCurve3d(const OdGePoint3dArray& fitPoints,
    const OdGeTol& fitTolerance = OdGeContext::gTol);

  /* Not implemented yet - low priority
  OdGeNurbCurve3d(const OdGePoint3dArray& fitPoints,
    const OdGeVector3dArray& fitTangents,
    const OdGeTol& fitTolerance = OdGeContext::gTol,
    bool isPeriodic = false);*/

  // numSpans - the number of knot spans in nurbs curve
  // if numSpans == 0 (default) it is automatically calculated from 
  // ellipse domain
  OdGeNurbCurve3d(const OdGeEllipArc3d& ellipse, int numSpans = 0);

  OdGeNurbCurve3d(const OdGeLineSeg3d& linSeg);

  OdGe::EntityId type() const;

  OdGeEntity3d* copy  () const;

  // Fit data query functions
  int numFitPoints() const;

  bool getFitPointAt(int index, OdGePoint3d& point) const;

  bool getFitTolerance(OdGeTol& fitTolerance) const;

  bool getFitTangents(OdGeVector3d& startTangent,
    OdGeVector3d& endTangent) const;

  bool getFitTangents(OdGeVector3d& startTangent,
    OdGeVector3d& endTangent,
    bool& startTangentDefined,
    bool& endTangentDefined) const;

  bool getFitData(OdGePoint3dArray& fitPoints,
    OdGeTol& fitTolerance,
    bool& tangentsExist,
    OdGeVector3d& startTangent,
    OdGeVector3d& endTangent) const;


  // NURBS data query functions
  void getDefinitionData(int& degree,
    bool& rational,
    bool& periodic,
    OdGeKnotVector& knots,
    OdGePoint3dArray& controlPoints,
    OdGeDoubleArray& weights) const;

  int numWeights() const;

  double weightAt(int idx) const;

  /* bool evalMode  () const;

  bool getParamsOfC1Discontinuity (OdGeDoubleArray& params,
    const OdGeTol& tol
    = OdGeContext::gTol) const;

  bool getParamsOfG1Discontinuity (OdGeDoubleArray& params,
    const OdGeTol& tol
    = OdGeContext::gTol) const;*/

  // Fit data edit functions
  void setFitPointAt(int index, const OdGePoint3d& point);

  void addFitPointAt(int index, const OdGePoint3d& point);

  void deleteFitPointAt(int index);


  void setFitTolerance(const OdGeTol& fitTol = OdGeContext::gTol);

  void setFitTangents(const OdGeVector3d& startTangent,
    const OdGeVector3d& endTangent);

  /* bool setFitTangents(const OdGeVector3d& startTangent,
    const OdGeVector3d& endTangent,
    bool& startTangentDefined,
    bool& endTangentDefined) const; Strange function */

  OdGeNurbCurve3d& setFitData(const OdGePoint3dArray& fitPoints,
    const OdGeVector3d& startTangent,
    const OdGeVector3d& endTangent,
    const OdGeTol& fitTol = OdGeContext::gTol);

  /* OdGeNurbCurve3d& setFitData(const OdGeKnotVector& fitKnots,
    const OdGePoint3dArray& fitPoints,
    const OdGeVector3d& startTangent,
    const OdGeVector3d& endTangent,
    const OdGeTol& fitTol = OdGeContext::gTol,
    bool isPeriodic = false);*/

  /*OdGeNurbCurve3d& setFitData(int degree,
    const OdGePoint3dArray& fitPoints,
    const OdGeTol& fitTol = OdGeContext::gTol);*/

  bool purgeFitData();

  // NURBS data edit functions

  void set(int degree,
    const OdGeKnotVector& knots,
    const OdGePoint3dArray& cntrlPnts,
    const OdGeDoubleArray& weights,
    bool isPeriodic = false);

  /*
  OdGeNurbCurve3d& addKnot(double newKnot);

  OdGeNurbCurve3d& insertKnot(double newKnot);*/

  OdGeSplineEnt3d& setWeightAt(int idx, double val);

  //  Not high priority
  // OdGeNurbCurve3d& setEvalMode(bool evalMode = false);

  // OdGeNurbCurve3d& joinWith(const OdGeNurbCurve3d& curve);

  //OdGeNurbCurve3d& hardTrimByParams(double newStartParam,
    //double newEndParam);

  OdGeNurbCurve3d& makeRational(double weight = 1.0);

  /* OdGeNurbCurve3d& makeClosed(); - deferred

  OdGeNurbCurve3d& makePeriodic();

  OdGeNurbCurve3d& makeNonPeriodic();

  OdGeNurbCurve3d& makeOpen();

  OdGeNurbCurve3d& elevateDegree (int plusDegree);*/

  // Virtual overrides(NURBS query functions)
  int degree() const;
  int order() const;

  bool hasFitData() const;

  bool isRational() const;
  virtual bool isClosed(const OdGeTol& tol = OdGeContext::gTol) const;

	bool isPlanar(OdGePlane& plane, const OdGeTol& tol = OdGeContext::gTol) const;


  int numKnots() const;
  const OdGeKnotVector& knots() const;

  double startParam() const;
  double endParam() const;
  virtual void getInterval(OdGeInterval& intrvl) const;
  virtual void getInterval(OdGeInterval& intrvl, OdGePoint3d& start, OdGePoint3d& end) const;
  virtual bool setInterval(const OdGeInterval& intrvl);
  DD_USING(OdGeSplineEnt3d::setInterval);

  // Evaluate methods.
  //
  OdGePoint3d evalPoint(double param) const;
  
  // This is only for internal use
  OdGePoint3d evalPoint(double param, int hint) const;  
  
  // This is need in surface(for rational case only)
  void evalPointDivider(double param, OdGePoint3d& point, double& divider, int hint) const;

  OdGePoint3d startPoint() const;
  OdGePoint3d endPoint() const;

  double knotAt(int idx) const;
  OdGeSplineEnt3d& setKnotAt(int idx, double val);

  int numControlPoints() const;
  OdGePoint3d controlPointAt(int idx) const;
  OdGeSplineEnt3d& setControlPointAt(int idx, const OdGePoint3d& pnt);

  virtual void appendSamplePoints(double fromParam, double toParam,
    double approxEps, OdGePoint3dArray& pointArray,
    OdGeDoubleArray* pParamArray = 0) const;
  DD_USING(OdGeSplineEnt3d::appendSamplePoints);

  // virtual void appendSamplePoints(int numSample, OdGePoint3dArray& pointArray) const;

  // Parameter of the point on curve.  Contract: point IS on curve
  //
  double paramOf(const OdGePoint3d& pnt, const OdGeTol& tol = OdGeContext::gTol) const;

  virtual OdGeCurve3d& reverseParam();

  // Assignment operator.
  OdGeNurbCurve3d& operator = (const OdGeNurbCurve3d& spline);

  virtual OdGeEntity3d& transformBy(const OdGeMatrix3d& xfm);

  // Attention!!
  // resulting curves will be created by new operator!!
  // it is user responsibility to delete them!!
  virtual void getSplitCurves(double param, OdGeCurve3d*& piece1,
    OdGeCurve3d*& piece2) const;

  bool hasStartPoint(OdGePoint3d& startPnt) const;
  bool hasEndPoint(OdGePoint3d& endPnt) const;

  virtual bool isLinear(OdGeLine3d& line, const OdGeTol& tol = OdGeContext::gTol) const;
private:
  friend class OdGeSystemInternals;
  OdGeNurbCurve3dImpl* m_pImpl;
};

#include "DD_PackPop.h"

#endif // OD_GENURB3D_H


