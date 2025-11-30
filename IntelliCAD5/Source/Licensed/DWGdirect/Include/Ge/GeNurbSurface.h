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



#ifndef OD_GENURBSF_H
#define OD_GENURBSF_H


#include "GeSurface.h"
#include "GeKnotVector.h"
#include "OdPlatformSettings.h"
#include "DD_PackPush.h"

class OdGeNurbCurve3d;

static const int     derivArraySize = 3;
typedef OdGeVector3d VectorDerivArray[derivArraySize][derivArraySize];
typedef double       WDerivArray     [derivArraySize][derivArraySize];

/** Description:
    A representation for a non-uniform, rational B-Spline surface.

    {group:OdGe_Classes} 
*/
class GE_TOOLKIT_EXPORT OdGeNurbSurface : public OdGeSurface
{
public:
  OdGeNurbSurface();
  OdGeNurbSurface(int degreeU, int degreeV, int propsInU, int propsInV,
                  int numControlPointsInU, int numControlPointsInV,
                  const OdGePoint3dArray& controlPoints,
                  const OdGeDoubleArray& weights,
                  const OdGeKnotVector& uKnots,
                  const OdGeKnotVector& vKnots,
                  const OdGeTol& tol = OdGeContext::gTol);
  OdGeNurbSurface(const OdGeNurbSurface& nurb);
  virtual ~OdGeNurbSurface();

  // OdGeEntity3d functions
  virtual bool isKindOf(OdGe::EntityId entType) const;
  virtual OdGe::EntityId type() const;
  // OdGeSurface functions
  virtual void getEnvelope (OdGeInterval& intrvlX, OdGeInterval& intrvlY) const;
  virtual bool isClosedInU (const OdGeTol& tol = OdGeContext::gTol) const;
  virtual bool isClosedInV (const OdGeTol& tol = OdGeContext::gTol) const;

  // Assignment.
  //
  OdGeNurbSurface& operator = (const OdGeNurbSurface& nurb);

  // Geometric properties.
  //
  bool   isRationalInU      () const;
  bool   isPeriodicInU      (double&) const;
  bool   isRationalInV      () const;
  bool   isPeriodicInV      (double&) const;

  int singularityInU  () const;
  int singularityInV  () const;

  // Definition data.
  //
  int            degreeInU            () const;
  int            numControlPointsInU  () const;
  int            degreeInV            () const;
  int            numControlPointsInV  () const;
  void           getControlPoints     (OdGePoint3dArray& points) const;
  bool getWeights           (OdGeDoubleArray& weights) const;
  int       numKnotsInU    () const;
  void      getUKnots      (OdGeKnotVector& uKnots) const;

  int       numKnotsInV    () const;
  void      getVKnots      (OdGeKnotVector& vKnots) const;
  void      getDefinition  (int& degreeU, int& degreeV,
                            int& propsInU,  int& propsInV,
                            int& numControlPointsInU,
                            int& numControlPointsInV,
                            OdGePoint3dArray& controlPoints,
                            OdGeDoubleArray& weights,
                            OdGeKnotVector& uKnots,
                            OdGeKnotVector& vKnots) const;

  // Reset surface
  //
  OdGeNurbSurface& set(int degreeU, int degreeV,
    int propsInU, int propsInV,
    int numControlPointsInU,
    int numControlPointsInV,
    const OdGePoint3dArray& controlPoints,
    const OdGeDoubleArray& weights,
    const OdGeKnotVector& uKnots,
    const OdGeKnotVector& vKnots,
    const OdGeTol& tol = OdGeContext::gTol);

  // OdGePoint3d evalPoint(const OdGePoint2d& param, int hintU = -1, int hintV = -1,
    // const OdGeNurbCurve3d* pLocalCurves = NULL) const;
  OdGePoint2d paramOf(const OdGePoint3d& pnt, const OdGeTol& tol = OdGeContext::gTol) const;

  virtual OdGePoint3d evalPoint(const OdGePoint2d& p) const;
  virtual OdGePoint3d evalPoint(const OdGePoint2d& param, int derivOrd,
    OdGeVector3dArray& derivatives, OdGeVector3d& normal) const;
  DD_USING(OdGeSurface::evalPoint);
  
  virtual OdGeEntity3d* copy() const;

  // computes isolines
  void computeVIsoLine(double V, OdGeNurbCurve3d& isoline) const;
  void computeUIsoLine(double U, OdGeNurbCurve3d& isoline) const;

  bool getDerivativesAt(const OdGePoint2d& param, OdUInt32 nDerivs,
    VectorDerivArray derivs) const;
protected:
  int               m_degreeU;
  int               m_degreeV;
  int               m_propsInU;
  int               m_propsInV;
  int               m_numControlPointsInU;
  int               m_numControlPointsInV;
  OdGePoint3dArray  m_controlPoints;
  OdGeDoubleArray   m_weights; 
  OdGeKnotVector    m_uKnots;
  OdGeKnotVector    m_vKnots;
  OdGeTol           m_tol;
  
  class SpanGridData
  {
  public:
    SpanGridData() : m_nSamples(0) {}
    OdUInt32         m_nSamples; // number of samples in kvants
      // full number of samples (in samples) is 
      // (1 << (m_nSamples-1)) * numSamplesKvant
    OdGePoint3dArray m_gridPoints;
  };
  mutable OdArray<SpanGridData*> m_grid;
  /* mutable OdGePoint3dArray* m_gridPointsPtr;
  mutable OdUInt32          m_nSamples; */

  bool compute_Aders_wders(const OdGePoint2d& param, 
    OdUInt32 nDerivs, 
    VectorDerivArray Aders, 
    WDerivArray wders) const;

  // the index in "m_controlPoints" of control point (i,j):
  int loc(int i, int j) const { return i*m_numControlPointsInV+j; }

  // the index in "m_grid" of span (hintU,hintV)
  OdUInt32 locGrid(int hintU, int hintV) const {return (hintU-m_degreeU)*(m_numControlPointsInV-m_degreeV) + (hintV-m_degreeV); }

  // internal helpful function
  bool isInConvexHullPrevPts(const OdGePoint3d& pnt, OdUInt32 hintU, 
    OdUInt32 hintV, const OdGeTol& tol) const;

  void calculateNURBSProperties();
};

#include "DD_PackPop.h"

#endif // OD_GENURBSF_H

