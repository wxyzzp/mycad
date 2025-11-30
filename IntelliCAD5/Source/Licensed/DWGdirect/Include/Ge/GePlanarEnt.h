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



#ifndef OD_GEPLANAR_H
#define OD_GEPLANAR_H

#include "GeSurface.h"
#include "GeInterval.h"
#include "OdPlatformSettings.h"

class OdGeLinearEnt3d;

#include "DD_PackPush.h"

/** Description:
    
    {group:OdGe_Classes} 
*/
class GE_TOOLKIT_EXPORT OdGePlanarEnt : public OdGeSurface
{
public:
  // OdGeSurface functions
  bool isNormalReversed() const;
  OdGeSurface& reverseNormal();
  bool isClosedInU(const OdGeTol& tol = OdGeContext::gTol) const;
  bool isClosedInV(const OdGeTol& tol = OdGeContext::gTol) const;
  virtual OdGePoint3d evalPoint(const OdGePoint2d& param) const;
  virtual OdGePoint3d evalPoint(const OdGePoint2d& param, int derivOrd,
    OdGeVector3dArray& derivatives, OdGeVector3d& normal) const;
  DD_USING(OdGeSurface::evalPoint);

  void getEnvelope(OdGeInterval& intrvlX, OdGeInterval& intrvlY) const;

  virtual void setEnvelope(const OdGeInterval& intrvlX, const OdGeInterval& intrvlY);

  // Run time type information.
  bool isKindOf(OdGe::EntityId entType) const;
  OdGe::EntityId type() const;

  bool intersectWith(const OdGeLinearEnt3d& linEnt, OdGePoint3d& pnt,
    const OdGeTol& tol = OdGeContext::gTol) const;

  OdGePoint3d closestPointToLinearEnt(const OdGeLinearEnt3d& line,
    OdGePoint3d& pointOnLine, const OdGeTol& tol = OdGeContext::gTol) const;

  OdGePoint3d closestPointToPlanarEnt(const OdGePlanarEnt& otherPln,
    OdGePoint3d& pointOnOtherPln, const OdGeTol& tol = OdGeContext::gTol) const;

  bool isParallelTo(const OdGeLinearEnt3d& linEnt, const OdGeTol& tol = OdGeContext::gTol) const;

  inline bool isParallelTo(const OdGePlanarEnt& otherPlnEnt,
    const OdGeTol& tol = OdGeContext::gTol) const;

  bool isPerpendicularTo(const OdGeLinearEnt3d& linEnt,
    const OdGeTol& tol = OdGeContext::gTol) const;

  inline bool isPerpendicularTo(const OdGePlanarEnt& otherPlnEnt,
    const OdGeTol& tol = OdGeContext::gTol) const;

  inline bool isCoplanarTo(const OdGePlanarEnt& otherPlnEnt,
    const OdGeTol& tol = OdGeContext::gTol) const;

  inline void get(OdGePoint3d&, OdGeVector3d& uVec, OdGeVector3d& vVec) const;
  void get(OdGePoint3d&, OdGePoint3d& origin, OdGePoint3d&) const;
  
  inline OdGePoint3d pointOnPlane() const;

  inline OdGeVector3d normal() const;

  inline void getCoefficients(double& a, double& b, double& c, double& d) const;

  /** Description:
      Evaluates the canonical coordinate system of this planar entity and returns it in the output

      Arguments:
      origin (O)
      axis1(O) uAxis.normal() 
      axis2(O) uAxis.crossProduct(vAxis).normal() 
  */
  void getCoordSystem(OdGePoint3d& origin, OdGeVector3d& axis1, OdGeVector3d& axis2) const;

  bool isOn(const OdGePoint3d& pnt, const OdGeTol& tol = OdGeContext::gTol) const;
  inline double signedDistanceTo(const OdGePoint3d& pnt) const;

  virtual OdGeEntity3d& transformBy(const OdGeMatrix3d& xfm);

  virtual OdGePoint2d paramOf(const OdGePoint3d& pnt,
    const OdGeTol& tol = OdGeContext::gTol) const;

  // ortho project
  virtual bool project(const OdGePoint3d& p, OdGePoint3d& projP) const;
  // project parallel to vector
  bool project(const OdGePoint3d& p, const OdGeVector3d& unitDir, 
    OdGePoint3d& pOnPlane, const OdGeTol& tol = OdGeContext::gTol) const;
protected:
  OdGePlanarEnt() {}

  inline void set(const OdGePoint3d& pntU, const OdGePoint3d& org, const OdGePoint3d& pntV);
  inline void set(const OdGePoint3d& pnt, const OdGeVector3d& normal);
  inline void set(double ta, double tb, double tc, double td);
  inline void set(const OdGePoint3d& org, const OdGeVector3d& uAxis, const OdGeVector3d& vAxis);

  OdGePoint3d   m_origin;
  OdGeVector3d  m_uVec;
  OdGeVector3d  m_vVec;
  OdGeVector3d  m_normal; // redundant data to decrease calculations
  OdGeInterval  m_URange, m_VRange;
};

//////////////////////////////////////////////////////////////////////////////////////

inline double OdGePlanarEnt::signedDistanceTo(const OdGePoint3d& pnt) const
{ return m_normal.dotProduct(pnt - m_origin); }

inline void OdGePlanarEnt::getCoefficients(double& a, double& b, double& c, double& d) const
{ a = m_normal.x; b = m_normal.y; c = m_normal.z; d = -m_normal.dotProduct(m_origin.asVector()); }

inline OdGePoint3d OdGePlanarEnt::pointOnPlane() const { return m_origin; }

inline OdGeVector3d OdGePlanarEnt::normal() const { return m_normal; }

inline bool OdGePlanarEnt::isCoplanarTo(const OdGePlanarEnt& otherPlnEnt, const OdGeTol& tol) const
{ return m_normal.isCodirectionalTo(otherPlnEnt.normal(), tol); }

inline bool OdGePlanarEnt::isParallelTo(const OdGePlanarEnt& otherPlnEnt, const OdGeTol& tol) const
{ return m_normal.isParallelTo(otherPlnEnt.normal(), tol); }

inline bool OdGePlanarEnt::isPerpendicularTo(const OdGePlanarEnt& otherPlnEnt, const OdGeTol& tol) const
{ return m_normal.isPerpendicularTo(otherPlnEnt.normal(), tol); }

inline void OdGePlanarEnt::set(const OdGePoint3d& pntU, const OdGePoint3d& org, const OdGePoint3d& pntV)
{ set(org, pntU - org, pntV - org); }

inline void OdGePlanarEnt::set(const OdGePoint3d& pnt, const OdGeVector3d& norm)
{
  m_origin = pnt; m_normal = norm; m_normal.normalize();
  m_uVec = norm.perpVector(); m_vVec = norm.crossProduct(m_uVec);
}

inline void OdGePlanarEnt::set(const OdGePoint3d& origin, const OdGeVector3d& uAxis, const OdGeVector3d& vAxis)
{
  m_origin = origin; m_uVec = uAxis; m_vVec = vAxis;
//  m_normal = uAxis.crossProduct(vAxis); m_normal.normalize();
// It's more safe (if vectors' length is close to tolerance)
  m_normal = uAxis.normal().crossProduct(vAxis.normal());
}

inline void OdGePlanarEnt::set(double ta, double tb, double tc, double td)
{ set(OdGePoint3d(ta * td, tb * td, tc * td), OdGeVector3d(ta, tb, tc)); }


inline void OdGePlanarEnt::get(OdGePoint3d& origin, OdGeVector3d& uVec, OdGeVector3d& vVec) const
{
  origin = m_origin;
  uVec = m_uVec;
  vVec = m_vVec;
}



#include "DD_PackPop.h"

#endif // OD_GEPLANAR_H


