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



#ifndef OD_GESPHERE_H
#define OD_GESPHERE_H



#include "GeSurface.h"

class OdGeCircArc3d;

#include "DD_PackPush.h"

/** Description:  
    A representation for a sphere.  
    
    Remarks:
    It is defined by its radius, and the center. Two vectors,
    northAxis and refAxis are used in the parameterization of
    the sphere.  northAxis identifies the north and the south poles
    of the sphere. refAxis serves as a reference for the parameters.
    It is a vector orthogonal to northAxis.
    Parameter U is the latitude, which for a closed sphere defaults
    to the range [-PI/2, PI/2].  The lower bound maps to the south
    pole, zero is on the equator, and the upperbound maps to the north pole.
    Parameter V is the longitude, which for a closed sphere defaults
    to the range [-PI, PI).  Zero corresponds to the meridian that
    contains the refAxis of the sphere.
    The sphere is periodic in V with a period of 2PI.
    [umin, umax] x [vmin, vmax] defines a four sided spherical patch
    bounded by two arcs that are meridians of longitude, and two arcs
    that are parallels of latitude.  Following constraints apply when
    defining a spherical patch.
    umin < umax and |umin - umax| <= 2PI.
    vmin < vmax and |vmin - vmax| <= PI.
    
    {group:OdGe_Classes} 
*/
class GE_TOOLKIT_EXPORT OdGeSphere : public OdGeSurface
{
public:
  OdGeSphere();
  OdGeSphere(double radius, const OdGePoint3d& center);
  OdGeSphere(double radius, const OdGePoint3d& center,
             const OdGeVector3d& northAxis, const OdGeVector3d& refAxis,
             double startAngleU, double endAngleU, double startAngleV, double endAngleV);
  //OdGeSphere(const OdGeSphere& sphere);
  
  bool           isKindOf     (OdGe::EntityId entType) const;
  OdGe::EntityId type         () const;
  OdGeEntity3d*  copy         () const;

  void           getEnvelope  (OdGeInterval& intrvlU, OdGeInterval& intrvlV) const;

  // Geometric properties.
  //
  double         radius       () const;
  OdGePoint3d    center       () const;
  void           getAnglesInU (double& start, double& end) const;
  void           getAnglesInV (double& start, double& end) const;
  OdGeVector3d   northAxis    () const;
  OdGeVector3d   refAxis      () const;
  OdGePoint3d    northPole    () const;
  OdGePoint3d    southPole    () const;
  bool           isOuterNormal() const;
  bool           isClosed     (const OdGeTol& tol = OdGeContext::gTol) const;
  bool           isClosedInU  (const OdGeTol&  tol = OdGeContext::gTol) const;
  bool           isClosedInV  (const OdGeTol&  tol = OdGeContext::gTol) const;
  
  OdGeSphere&    setRadius    (double);
  OdGeSphere&    setAnglesInU (double start, double end);
  OdGeSphere&    setAnglesInV (double start, double end);
  OdGeSphere&    set          (double radius, const OdGePoint3d& center);
  OdGeSphere&    set          (double radius, const OdGePoint3d& center,
                               const OdGeVector3d& northAxis,
                               const OdGeVector3d& refAxis,
                               double startAngleU,
                               double endAngleU,
                               double startAngleV,
                               double endAngleV);

  virtual OdGePoint2d paramOf(const OdGePoint3d& pnt,
    const OdGeTol& tol = OdGeContext::gTol) const;

  virtual OdGePoint3d evalPoint(const OdGePoint2d& param) const;
  virtual OdGePoint3d evalPoint(const OdGePoint2d& param, int derivOrd,
    OdGeVector3dArray& derivatives) const;
  virtual OdGePoint3d evalPoint(const OdGePoint2d& param, int derivOrd,
    OdGeVector3dArray& derivatives, OdGeVector3d& normal) const;

  // Assignment operator.
  //
  //OdGeSphere&    operator =   (const OdGeSphere& sphere);
  
  // Intersection with a linear entity
  //
  bool           intersectWith(const OdGeLinearEnt3d&, int& intn,
                               OdGePoint3d& p1, OdGePoint3d& p2,
                               const OdGeTol& tol = OdGeContext::gTol) const;
protected:
  double        m_radius,
                m_startAngleU,
                m_endAngleU,
                m_startAngleV,
                m_endAngleV;
  OdGePoint3d   m_center;
  OdGeVector3d  m_northAxis;
  OdGeVector3d  m_refAxis;
};


inline OdGeSphere::OdGeSphere() { set(1.0, OdGePoint3d::kOrigin); }

inline OdGeSphere::OdGeSphere(double radius, const OdGePoint3d& center) { set(radius, center); }

inline OdGePoint3d OdGeSphere::center() const { return m_center; }

inline OdGeSphere& OdGeSphere::setAnglesInU(double start, double end)
{ m_startAngleU = start; m_endAngleU = end; return *this; }

inline OdGeSphere& OdGeSphere::setAnglesInV(double start, double end)
{ m_startAngleV = start; m_endAngleV = end; return *this; }

inline void OdGeSphere::getAnglesInU(double& start, double& end) const
{ start = m_startAngleU; end = m_endAngleU; }

inline void OdGeSphere::getAnglesInV(double& start, double& end) const
{ start = m_startAngleV; end = m_endAngleV; }

inline bool OdGeSphere::isClosedInU(const OdGeTol&) const
{ return false; }

inline bool OdGeSphere::isClosedInV(const OdGeTol&) const
{ return !(m_endAngleV - m_startAngleV < Oda2PI); }

inline OdGeSphere& OdGeSphere::setRadius(double radius) { m_radius = radius; return *this; }

inline OdGeVector3d OdGeSphere::northAxis() const { return m_northAxis; }

inline OdGeVector3d OdGeSphere::refAxis() const { return m_refAxis; }

inline OdGePoint3d OdGeSphere::northPole() const
{ return m_center + m_northAxis * (m_radius > 0.0 ? m_radius : -m_radius); }

inline OdGePoint3d OdGeSphere::southPole() const
{ return m_center - m_northAxis * (m_radius > 0.0 ? m_radius : -m_radius); }

inline bool OdGeSphere::isClosed(const OdGeTol& tol) const { return isClosedInV(tol); }

#include "DD_PackPop.h"

#endif // OD_GESPHERE_H


