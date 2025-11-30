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



#ifndef OD_GENURB2d_H
#define OD_GENURB2d_H

#include "GeSplineEnt2d.h"
#include "OdPlatformSettings.h"

class OdGeNurbCurve2dImpl;
class OdGePolyline2d;
class OdGeEllipArc2d;
class OdGeLineSeg2d;
class OdGeKnotVector;

#include "DD_PackPush.h"

/** Description:
    
    {group:OdGe_Classes} 
*/
class GE_TOOLKIT_EXPORT OdGeNurbCurve2d : public OdGeSplineEnt2d
{
public:
  // Construct spline from NURBS-data
  //
  OdGeNurbCurve2d();
  OdGeNurbCurve2d(const OdGeNurbCurve2d& src);
  OdGeNurbCurve2d(int degree, const OdGeKnotVector& knots,
    const OdGePoint2dArray& cntrlPnts,
    bool isPeriodic = false);
  OdGeNurbCurve2d(int degree, const OdGeKnotVector& knots,
    const OdGePoint2dArray& cntrlPnts,
    const OdGeDoubleArray& weights,
    bool isPeriodic = false);
/*
  // Construct spline from interpolation data.
  //
  OdGeNurbCurve2d(int degree, const OdGePolyline2d& fitPolyline,
    bool isPeriodic = false);
  OdGeNurbCurve2d(const OdGePoint2dArray& fitPoints,
    const OdGeVector2d& startTangent,
    const OdGeVector2d& endTangent,
    bool startTangentDefined = true,
    bool endTangentDefined = true,
    const OdGeTol& fitTolerance = OdGeContext::gTol);
  OdGeNurbCurve2d(const OdGePoint2dArray& fitPoints,
    const OdGeTol& fitTolerance = OdGeContext::gTol);
  OdGeNurbCurve2d(const OdGePoint2dArray& fitPoints,
    const OdGeVector2dArray& fitTangents,
    const OdGeTol& fitTolerance = OdGeContext::gTol,
    bool isPeriodic = false);
*/

  virtual ~OdGeNurbCurve2d();

/*
  // Spline representation of ellipse
  //
  OdGeNurbCurve2d(const OdGeEllipArc2d& ellipse);

  // Spline representation of line segment
  //
  OdGeNurbCurve2d(const OdGeLineSeg2d& linSeg);

  // Query methods.
  //
  int numFitPoints() const;
  bool getFitPointAt(int index, OdGePoint2d& point) const;
  bool getFitTolerance(OdGeTol& fitTolerance) const;
  bool getFitTangents(OdGeVector2d& startTangent,
    OdGeVector2d& endTangent) const;
  bool getFitData(OdGePoint2dArray& fitPoints,
    OdGeTol& fitTolerance,
    bool& tangentsExist,
    OdGeVector2d& startTangent,
    OdGeVector2d& endTangent) const;
*/

  void getDefinitionData(int& degree, bool& rational,
    bool& periodic,
			 OdGeKnotVector& knots,
       OdGePoint2dArray& controlPoints,
       OdGeDoubleArray& weights) const;

  int   degree() const;
  bool  isRational() const;
  virtual bool isClosed(const OdGeTol& tol = OdGeContext::gTol) const;

  int    numWeights() const;
  double weightAt(int idx) const;

  int   numKnots() const;
  const OdGeKnotVector& knots() const;
  double knotAt(int idx) const;

  int numControlPoints() const;
  OdGePoint2d controlPointAt(int idx) const;

  OdGeCurve2d& reverseParam();

/*
  bool evalMode() const;
  bool getParamsOfC1Discontinuity(OdGeDoubleArray& params,
    const OdGeTol& tol
    = OdGeContext::gTol) const;
  bool	getParamsOfG1Discontinuity(OdGeDoubleArray& params,
    const OdGeTol& tol
    = OdGeContext::gTol) const;

  // Modification methods.
  //
  void setFitPointAt(int index, const OdGePoint2d& point);
  void addFitPointAt(int index, const OdGePoint2d& point);
  void deleteFitPointAt(int index);
  void setFitTolerance(const OdGeTol& fitTol=OdGeContext::gTol);
  void setFitTangents(const OdGeVector2d& startTangent,
	 	 const OdGeVector2d& endTangent);
  OdGeNurbCurve2d& setFitData(const OdGePoint2dArray& fitPoints,
    const OdGeVector2d& startTangent,
    const OdGeVector2d& endTangent,
    const OdGeTol& fitTol=OdGeContext::gTol);
  OdGeNurbCurve2d& setFitData(const OdGeKnotVector& fitKnots,
    const OdGePoint2dArray& fitPoints,
    const OdGeVector2d& startTangent,
    const OdGeVector2d& endTangent,
    const OdGeTol& fitTol=OdGeContext::gTol,
    bool isPeriodic=false);
  OdGeNurbCurve2d& setFitData(int degree,
    const OdGePoint2dArray& fitPoints,
    const OdGeTol& fitTol=OdGeContext::gTol);
*/

  bool purgeFitData();
  OdGeSplineEnt2d& setKnotAt(int idx, double val);
  /* OdGeNurbCurve2d& addKnot(double newKnot);
  OdGeNurbCurve2d& insertKnot(double newKnot);*/

  OdGeSplineEnt2d& setWeightAt(int idx, double val);
  OdGeNurbCurve2d& makeRational(double weight = 1.0);

  OdGeSplineEnt2d& setControlPointAt(int idx, const OdGePoint2d& pnt);

/*
  OdGeNurbCurve2d& setEvalMode(bool evalMode=false);
  OdGeNurbCurve2d& joinWith(const OdGeNurbCurve2d& curve);
  OdGeNurbCurve2d& hardTrimByParams(double newStartParam,
    double newEndParam);
  OdGeNurbCurve2d& makeClosed();
  OdGeNurbCurve2d& makePeriodic();
  OdGeNurbCurve2d& makeNonPeriodic();
  OdGeNurbCurve2d& makeOpen();
  OdGeNurbCurve2d& elevateDegree(int plusDegree);
*/
  // Virtual overrides
  

  OdGe::EntityId type() const;
  void appendSamplePoints(double fromParam,
    double toParam,
    double approxEps,
    OdGePoint2dArray& pointArray,
    OdGeDoubleArray* pParamArray = 0) const;
  DD_USING(OdGeSplineEnt2d::appendSamplePoints);

  OdGeEntity2d* copy  () const
  {
    return new OdGeNurbCurve2d(*this);
  }

  virtual OdGeEntity2d& transformBy(const OdGeMatrix2d& xfm);

  OdGePoint2d evalPoint(double param) const;
  double paramOf(const OdGePoint2d& pnt, const OdGeTol& tol = OdGeContext::gTol) const;
  void getInterval(OdGeInterval& intrvl) const;
  void getInterval(OdGeInterval& intrvl, OdGePoint2d& start, OdGePoint2d& end) const;
  double startParam() const;
  double endParam() const;
  bool setInterval(const OdGeInterval& intrvl);

  // Assignment operator.
  //
  OdGeNurbCurve2d& operator =(const OdGeNurbCurve2d& spline);

  bool hasStartPoint(OdGePoint2d& startPnt) const;
  bool hasEndPoint(OdGePoint2d& endPnt) const;
  OdGePoint2d startPoint() const;
  OdGePoint2d endPoint() const;

  void set(int degree, const OdGeKnotVector& knots,
      const OdGePoint2dArray& cntrlPnts, 
      const OdGeDoubleArray& weights, bool isPeriodic = false);

  bool intersectWith(const OdGeLine2d &line2d, OdGePoint2dArray &pnts2d, const OdGeTol& tol = OdGeContext::gTol, OdGeDoubleArray *pParams = NULL) const;

  bool isLinear(OdGeLine2d& line, const OdGeTol& tol = OdGeContext::gTol) const;

private:
  friend class OdGeSystemInternals;
  OdGeNurbCurve2dImpl* m_pImpl;
};

#include "DD_PackPop.h"

#endif


