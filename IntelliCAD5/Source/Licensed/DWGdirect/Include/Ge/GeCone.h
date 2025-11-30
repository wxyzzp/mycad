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



#ifndef OD_GECONE_H
#define OD_GECONE_H

//
// Description:
//
// This file implements the class OdGeCone, a representation for
// a single right circular cone.  It is defined by its axis of
// symmetry, height, half angle (the angle between the generating
// line and the axis of symmetry), its origin which is a point on
// the axis of symmetry, and the radius at the origin.  The cross
// section containing the origin is refered to as the base.  The
// radius at the base should not be zero.  The cone may extend on
// either side of the base.
// The fabs(halfAngle) is constrained to the interval (0, PI/2),
// and is measured from the symmetric axis of the cone.  A positive
// angle results in a cone apex on the opposite direction of the
// symmetric axis, and vice versa for a negative angle.
// Parameter v is the angle of revolution measured from the refAxis
// (an axis perpendicular to the axis of symmetry).  For a closed cone,
// it defaults to [-PI, PI).  Right hand rule applied along the
// direction of the axis of symmetry defines the positive direction
// of v.  The surface is periodic in v with a period of 2PI.
// Parameter u varies along the generating line.  u = 0 correspond
// to the base of the cone.  u is dimensionless and increases in the
// direction the axis of symmetry.  It is scaled against the initial
// base radius of the cone.
// [umin, umax] x [vmin, vmax] defines a four sided conical patch
// bounded by two straight lines (at angles umin and umax), and two
// circular arcs (at vmin and vmax).  Following must be observed
// when defining a cone.
// umin < umax and |umin - umax| <= 2PI.
// Base radius > 0.0
// The hight of the cone is specified relative to its origin
// (with the height increasing in the direction of the symetric
// axis).
//

#include "OdPlatformSettings.h"
#include "GeSurface.h"
#include "GeInterval.h"

class OdGePoint3d;
class OdGeVector3d;
class OdGeCircArc3d;
class OdGeLinearEnt3d;

#include "DD_PackPush.h"

/** Description:
    
    {group:OdGe_Classes} 
*/
class GE_TOOLKIT_EXPORT OdGeCone : public OdGeSurface
{
protected:
  double ConvertHeight2U(double h) const;
  OdGeInterval ConvertHeight2U(const OdGeInterval& height) const;
public:
  OdGeCone();
  OdGeCone(double cosineAngle, double sineAngle,
           const  OdGePoint3d& baseOrigin, double baseRadius,
           const  OdGeVector3d& axisOfSymmetry);
  OdGeCone(double cosineAngle, double sineAngle,
           const  OdGePoint3d& baseOrigin, double baseRadius,
           const  OdGeVector3d& axisOfSymmetry,
           const  OdGeVector3d& refAxis, const  OdGeInterval& height,
           double startAngle, double endAngle);
  //OdGeCone(const OdGeCone& cone);
  
  // Run time type information.
  bool isKindOf(OdGe::EntityId entType) const;
  OdGe::EntityId type() const;
  
  // Make a copy of the entity.
  OdGeEntity3d* copy() const;

  // Geometric properties.
  //
  double        baseRadius    () const;
  OdGePoint3d   baseCenter    () const;
  void          getAngles     (double& start, double& end) const;
  double        halfAngle     () const;
  void          getHalfAngle  (double& cosineAngle, double& sineAngle) const;
  void          getHeight     (OdGeInterval& range) const;
  double        heightAt      (double u) const;
  OdGeVector3d  axisOfSymmetry() const;
  OdGeVector3d  refAxis       () const;
  OdGePoint3d   apex          () const;
  bool          isClosed      (const OdGeTol& tol = OdGeContext::gTol) const;
  bool          isClosedInU   (const OdGeTol& tol = OdGeContext::gTol) const;
  bool          isClosedInV   (const OdGeTol& tol = OdGeContext::gTol) const;
  bool          isOuterNormal  () const;
  
  OdGeCone&     setBaseRadius(double radius);
  OdGeCone&     setAngles    (double startAngle, double endAngle);
  OdGeCone&     setHeight    (const OdGeInterval& height);
  OdGeCone&     set          (double cosineAngle, double sineAngle,
                              const  OdGePoint3d& baseCenter,
                              double baseRadius,
                              const  OdGeVector3d& axisOfSymmetry);
  
  OdGeCone&     set          (double cosineAngle, double sineAngle,
                              const  OdGePoint3d& baseCenter,
                              double baseRadius,
                              const  OdGeVector3d& axisOfSymmetry,
                              const  OdGeVector3d& refAxis,
                              const  OdGeInterval& height,
                              double startAngle, double endAngle);
  // Assignment operator.
  //
  //OdGeCone&     operator =   (const OdGeCone& cone);
  
  // Intersection with a linear entity
  //
  /*
  bool          intersectWith(const OdGeLinearEnt3d& linEnt, int& intn,
                              OdGePoint3d& p1, OdGePoint3d& p2,
                              const OdGeTol& tol = OdGeContext::gTol) const;*/

  virtual void getEnvelope  (OdGeInterval& intrvlU, OdGeInterval& intrvlV) const;

  virtual OdGePoint3d evalPoint(const OdGePoint2d&) const;
  virtual OdGePoint3d evalPoint(const OdGePoint2d& param, int derivOrd,
    OdGeVector3dArray& derivatives, OdGeVector3d& normal) const;
  DD_USING(OdGeSurface::evalPoint);

  virtual OdGePoint2d paramOf(const OdGePoint3d& ,
    const OdGeTol& tol = OdGeContext::gTol) const;
  virtual bool project(const OdGePoint3d& p, OdGePoint3d& projP) const;
private:
  double        m_sineAngle,
                m_cosineAngle,
                m_baseRadius,
                m_startAngle,
                m_endAngle;
  OdGeInterval  m_height;
  OdGeVector3d  m_axisOfSymmetry;
  OdGeVector3d  m_refAxis;
  OdGePoint3d   m_baseOrigin;
};

inline double OdGeCone::baseRadius() const { return m_baseRadius; }

inline OdGePoint3d OdGeCone::baseCenter() const { return m_baseOrigin; }

inline void OdGeCone::getAngles(double& start, double& end) const
{ start = m_startAngle; end = m_endAngle; }

inline void OdGeCone::getHalfAngle(double& cosineAngle, double& sineAngle) const
{ cosineAngle = m_cosineAngle; sineAngle = m_sineAngle; }

inline OdGeVector3d OdGeCone::axisOfSymmetry() const { return m_axisOfSymmetry; }

inline OdGeVector3d OdGeCone::refAxis() const { return m_refAxis; }

inline OdGePoint3d OdGeCone::apex() const
{ return m_baseOrigin - m_axisOfSymmetry * (m_baseRadius * m_cosineAngle / m_sineAngle); }

inline bool OdGeCone::isClosed(const OdGeTol&) const
{ return !(m_endAngle - m_startAngle < Oda2PI); }

inline OdGeCone& OdGeCone::setBaseRadius(double radius)
{ m_baseRadius = radius > 0. ? radius : -radius; return *this; }

#include "DD_PackPop.h"

#endif // OD_GECONE_H


