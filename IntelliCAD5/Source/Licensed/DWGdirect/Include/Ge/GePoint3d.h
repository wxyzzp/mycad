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



#ifndef OD_GEPNT3D_H
#define OD_GEPNT3D_H

#include "GeVector3d.h"
#include "GePoint2d.h"
#include "OdArray.h"

class OdGeMatrix3d;
class OdGeLinearEnt3d;
class OdGePlane;
class OdGePlanarEnt;
class OdGeVector3d;
class OdGePoint2d;

#include "DD_PackPush.h"

/** Description:
    
    {group:OdGe_Classes} 
*/
class GE_TOOLKIT_EXPORT OdGePoint3d
{
public:
  OdGePoint3d();
  OdGePoint3d(const OdGePoint3d& pnt);
  OdGePoint3d(double x, double y, double z);

  /** Description:
      Constructs 3D point by the formula: p + uAxis * pnt2d.x + vAxis * pnt2d.y,
      where 3D point p and 3D vectors uAxis and vAxis form the coordinate system 
      in planar entity pln and can found by function call pln.get(p, uAxis, vAxis).

      Arguments:
      pln (I) Input plane.
      pnt2d (I) Input 2D point.
  */
  OdGePoint3d(const OdGePlanarEnt& pln, const OdGePoint2d& pnt2d);

  static const   OdGePoint3d    kOrigin;
  
  friend GE_TOOLKIT_EXPORT OdGePoint3d    operator *  (const OdGeMatrix3d& mat, const OdGePoint3d& pnt);
  friend GE_TOOLKIT_EXPORT OdGePoint3d    operator *  (double scl, const OdGePoint3d& pnt);

  OdGePoint3d&   setToProduct(const OdGeMatrix3d& mat, const OdGePoint3d& pnt);
  
  OdGePoint3d&   transformBy (const OdGeMatrix3d& leftSide);
  OdGePoint3d&   rotateBy    (double angle, const OdGeVector3d& vec, const OdGePoint3d& wrtPoint = OdGePoint3d::kOrigin);
  OdGePoint3d&   mirror      (const OdGePlane& pln);
  OdGePoint3d&   scaleBy     (double scaleFactor, const OdGePoint3d& wrtPoint = OdGePoint3d::kOrigin);
  OdGePoint2d    convert2d   (const OdGePlanarEnt& pln) const;
  OdGePoint2d    convert2d   () const; // returns projection to XY plane
  
  OdGePoint3d    operator *  (double scl) const;
  OdGePoint3d&   operator *= (double scl);
  OdGePoint3d    operator /  (double scl) const;
  OdGePoint3d&   operator /= (double scl);
  
  OdGePoint3d    operator +  (const OdGeVector3d& vec) const;
  OdGePoint3d&   operator += (const OdGeVector3d& vec);
  OdGePoint3d    operator -  (const OdGeVector3d& vec) const;
  OdGePoint3d&   operator -= (const OdGeVector3d& vec);
  OdGePoint3d&   setToSum    (const OdGePoint3d& pnt, const OdGeVector3d& vec);
  
  OdGeVector3d   operator -  (const OdGePoint3d& pnt) const;
  const OdGeVector3d& asVector() const;
  
  double         distanceTo       (const OdGePoint3d& pnt) const;
  
  OdGePoint3d    project       (const OdGePlane& pln, const OdGeVector3d& vec) const;
  OdGePoint3d    orthoProject  (const OdGePlane& pln) const;
  
  bool          operator == (const OdGePoint3d& pnt) const;
  bool          operator != (const OdGePoint3d& pnt) const;
  bool          isEqualTo   (const OdGePoint3d& pnt, const OdGeTol& tol = OdGeContext::gTol) const;
  
  double         operator [] (unsigned int i) const;
  double&        operator [] (unsigned int idx);
  OdGePoint3d&   set         (double x, double y, double z);
  OdGePoint3d&   set         (const OdGePlanarEnt& pln, const OdGePoint2d& pnt);
  
  double         x, y, z;
};

inline
OdGePoint3d::OdGePoint3d() : x(0.0), y(0.0), z(0.0)
{
}

inline
OdGePoint3d::OdGePoint3d(const OdGePoint3d& src) : x(src.x), y(src.y), z(src.z)
{
}

inline
OdGePoint3d::OdGePoint3d(double xx, double yy, double zz) : x(xx), y(yy), z(zz)
{
}

inline bool OdGePoint3d::operator==(const OdGePoint3d& pnt) const
{
  return isEqualTo(pnt);
}

inline bool OdGePoint3d::operator!=(const OdGePoint3d& pnt) const
{
  return !isEqualTo(pnt);
}

inline OdGePoint3d
OdGePoint3d::operator * (double val) const
{
  return OdGePoint3d(x*val, y*val, z*val);
}

inline OdGePoint3d
operator * (double val, const OdGePoint3d& p)
{
  return OdGePoint3d(p.x*val, p.y*val, p.z*val);
}

inline OdGePoint3d&
OdGePoint3d::operator *= (double val)
{
  x *= val;
  y *= val;
  z *= val;
  return *this;
}

inline OdGePoint3d
OdGePoint3d::operator / (double val) const
{
  return OdGePoint3d (x/val, y/val, z/val);
}

inline OdGePoint3d&
OdGePoint3d::operator /= (double val)
{
  x /= val;
  y /= val;
  z /= val;
  return *this;
}

inline OdGePoint3d
OdGePoint3d::operator + (const OdGeVector3d& v) const
{
  return OdGePoint3d (x + v.x, y + v.y, z + v.z);
}

inline OdGePoint3d&
OdGePoint3d::operator += (const OdGeVector3d& v)
{
  x += v.x;
  y += v.y;
  z += v.z;
  return *this;
}

inline OdGePoint3d
OdGePoint3d::operator - (const OdGeVector3d& v) const
{
  return OdGePoint3d (x - v.x, y - v.y, z - v.z);
}

inline OdGePoint3d&
OdGePoint3d::operator -= (const OdGeVector3d& v)
{
  x -= v.x;
  y -= v.y;
  z -= v.z;
  return *this;
}

inline OdGeVector3d
OdGePoint3d::operator - (const OdGePoint3d& p) const
{
  return OdGeVector3d (x - p.x, y - p.y, z - p.z);
}

inline const OdGeVector3d&
OdGePoint3d::asVector() const
{
  return (const OdGeVector3d&)*this;
}

inline OdGePoint3d&
OdGePoint3d::set(double xx, double yy, double zz)
{
  x = xx;
  y = yy;
  z = zz;
  return *this;
}

inline double
OdGePoint3d::operator [] (unsigned int i) const
{
  return * (&x+i);
}

inline double&
OdGePoint3d::operator [] (unsigned int i)
{
  return * (&x+i);
}

inline OdGePoint2d OdGePoint3d::convert2d() const
{
  return OdGePoint2d(x, y);
}

#include "DD_PackPop.h"

#endif


