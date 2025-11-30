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



#ifndef OD_GEPLANE_H
#define OD_GEPLANE_H


#include "GePlanarEnt.h"

class OdGeBoundedPlane;
class OdGeLine3d;
class OdGeLineSeg3d;

#include "DD_PackPush.h"


/** Description:
    
    {group:OdGe_Classes} 
*/
class GE_TOOLKIT_EXPORT OdGePlane : public OdGePlanarEnt
{
public:
  static const OdGePlane kXYPlane;
  static const OdGePlane kYZPlane;
  static const OdGePlane kZXPlane;
  
  // OdGeEntity3d functions
  bool isKindOf(OdGe::EntityId entType) const;
  OdGe::EntityId type() const;

  OdGePlane();
  OdGePlane(const OdGePoint3d& origin, const OdGeVector3d& normal);
  OdGePlane(const OdGePoint3d& pntU, const OdGePoint3d& org, const OdGePoint3d& pntV);
  OdGePlane(const OdGePoint3d& org, const OdGeVector3d& uAxis, const OdGeVector3d& vAxis);
  OdGePlane(double ta, double tb, double tc, double td);

  virtual OdGeEntity3d* copy() const;

  bool intersectWith(const OdGeLinearEnt3d& linEnt, OdGePoint3d& resultPnt,
    const OdGeTol& tol = OdGeContext::gTol) const;
  bool intersectWith(const OdGePlane& otherPln, OdGeLine3d& resultLine,
    const OdGeTol& tol = OdGeContext::gTol) const;
  bool intersectWith(const OdGeBoundedPlane& bndPln, OdGeLineSeg3d& resultLineSeg,
    const OdGeTol& tol = OdGeContext::gTol) const;
  
  OdGePlane& set(const OdGePoint3d& pnt, const OdGeVector3d& normal);
  OdGePlane& set(const OdGePoint3d& pntU, const OdGePoint3d& org, const OdGePoint3d& pntV);
  OdGePlane& set(double ta, double tb, double tc, double td);
  OdGePlane& set(const OdGePoint3d& org, const OdGeVector3d& uAxis, const OdGeVector3d& vAxis);
};

inline OdGePlane::OdGePlane()
{ *this = OdGePlane::kXYPlane;}

inline OdGePlane::OdGePlane(const OdGePoint3d& origin, const OdGeVector3d& normal)
{ OdGePlanarEnt::set(origin, normal); }

inline OdGePlane::OdGePlane(const OdGePoint3d& pntU, const OdGePoint3d& org, const OdGePoint3d& pntV)
{ OdGePlanarEnt::set(pntU, org, pntV); }

inline OdGePlane::OdGePlane(const OdGePoint3d& org, const OdGeVector3d& uAxis, const OdGeVector3d& vAxis)
{ OdGePlanarEnt::set(org, uAxis, vAxis); }

inline OdGePlane::OdGePlane(double ta, double tb, double tc, double td)
{ OdGePlanarEnt::set(ta, tb, tc, td); }

inline OdGePlane& OdGePlane::set(const OdGePoint3d& pnt, const OdGeVector3d& normal)
{ OdGePlanarEnt::set(pnt, normal); return *this; }

inline OdGePlane& OdGePlane::set(const OdGePoint3d& pntU, const OdGePoint3d& org, const OdGePoint3d& pntV)
{ OdGePlanarEnt::set(pntU, org, pntV); return *this; }

inline OdGePlane& OdGePlane::set(double ta, double tb, double tc, double td)
{ OdGePlanarEnt::set(ta, tb, tc, td); return *this; }

inline OdGePlane& OdGePlane::set(const OdGePoint3d& org, const OdGeVector3d& uAxis, const OdGeVector3d& vAxis)
{ OdGePlanarEnt::set(org, uAxis, vAxis); return *this; }

#include "DD_PackPop.h"

#endif // OD_GEPLANE_H


