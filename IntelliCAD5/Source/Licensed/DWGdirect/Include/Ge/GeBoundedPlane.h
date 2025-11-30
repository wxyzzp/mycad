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



#ifndef OD_GEBNDPLN_H
#define OD_GEBNDPLN_H

#include "GePlanarEnt.h"
#include "GePlane.h"
#include "GeVector3d.h"

#include "DD_PackPush.h"

class OdGePlane;
class OdGeVector3d;
class OdGePoint3d;
class OdGePoint2d;
class OdGeLineSeg3d;

/** Description:
    
    {group:OdGe_Classes} 
*/
class OdGeBoundedPlane : public OdGePlanarEnt
{
public:
  // OdGeEntity3d functions
  bool isKindOf(OdGe::EntityId entType) const;
  OdGe::EntityId type() const;
  OdGeEntity3d* copy() const;

  // OdGePlanarEnt functions
  OdGePoint3d pointOnPlane() const;
  OdGeVector3d normal() const;
  void getCoefficients(double& a, double& b, double& c, double& d) const;

  OdGeBoundedPlane();
  OdGeBoundedPlane(const OdGePoint3d& origin, const OdGeVector3d& uVec, const OdGeVector3d& vVec);
  OdGeBoundedPlane(const OdGePoint3d& p1, const OdGePoint3d& origin, const OdGePoint3d& p2);
  
  /*
  bool intersectWith(const OdGeLinearEnt3d& linEnt, OdGePoint3d& point,
                             const OdGeTol& tol = OdGeContext::gTol) const;
  bool intersectWith(const OdGePlane& plane, OdGeLineSeg3d& results,
                               const OdGeTol& tol = OdGeContext::gTol) const;
  bool intersectWith(const OdGeBoundedPlane& plane, OdGeLineSeg3d& result,
                                   const OdGeTol& tol = OdGeContext::gTol) const;
                                   */
  
  OdGeBoundedPlane& set(const OdGePoint3d& origin, const OdGeVector3d& uVec, const OdGeVector3d& vVec);
  inline OdGeBoundedPlane& set(const OdGePoint3d& p1, const OdGePoint3d& origin, const OdGePoint3d& p2);
};

inline OdGeBoundedPlane::OdGeBoundedPlane(const OdGePoint3d& origin,
                                          const OdGeVector3d& uVec,
                                          const OdGeVector3d& vVec)
{
  set(origin, uVec, vVec);
  setEnvelope(OdGeInterval(0.,1.), OdGeInterval(0.,1.));
}

inline OdGeBoundedPlane::OdGeBoundedPlane(const OdGePoint3d& p1,
                                          const OdGePoint3d& origin,
                                          const OdGePoint3d& p2)
{
  set(p1, origin, p2);
  setEnvelope(OdGeInterval(0.,1.), OdGeInterval(0.,1.));
}

inline OdGeBoundedPlane& OdGeBoundedPlane::set(const OdGePoint3d& p1,
                                               const OdGePoint3d& origin,
                                               const OdGePoint3d& p2)
{
  return set(origin, p1-origin, p2-origin);
}

#include "DD_PackPop.h"

#endif // OD_GEBNDPLN_H


