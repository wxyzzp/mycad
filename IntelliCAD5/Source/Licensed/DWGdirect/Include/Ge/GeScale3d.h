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



#ifndef OD_GESCL3D_H
#define OD_GESCL3D_H


class OdGeMatrix3d;
#include "GeGbl.h"

#include "DD_PackPush.h"

/** Description:
    
    {group:OdGe_Classes} 
*/
class GE_TOOLKIT_EXPORT OdGeScale3d
{
public:
  OdGeScale3d() : sx(1.0), sy(1.0), sz(1.0) {}
  OdGeScale3d(const OdGeScale3d& src) : sx(src.sx), sy(src.sy), sz(src.sz) {}
  OdGeScale3d(double factor) : sx(factor), sy(factor), sz(factor) {}
  OdGeScale3d(double xFact, double yFact, double zFact) : sx(xFact), sy(yFact), sz(zFact) {}

  inline OdGeScale3d& operator=(const OdGeScale3d& s)
  {
    sx = s.sx;
    sy = s.sy;
    sz = s.sz;
    return *this;
  }

  static const OdGeScale3d kIdentity;
  
  OdGeScale3d operator *(const OdGeScale3d& sclVec) const;
  OdGeScale3d& operator *=(const OdGeScale3d& scl);
  OdGeScale3d& preMultBy(const OdGeScale3d& leftSide);
  OdGeScale3d& postMultBy(const OdGeScale3d& rightSide);
  OdGeScale3d& setToProduct(const OdGeScale3d& sclVec1, const OdGeScale3d& sclVec2);
  OdGeScale3d operator *(double s) const;
  OdGeScale3d& operator *=(double s);
  OdGeScale3d& setToProduct(const OdGeScale3d& sclVec, double s);
  friend OdGeScale3d operator *(double, const OdGeScale3d& scl);
  
  OdGeScale3d inverse() const;
  OdGeScale3d& invert();
  
  bool isProportional(const OdGeTol& tol = OdGeContext::gTol) const;
  
  bool operator ==(const OdGeScale3d& sclVec) const;
  bool operator !=(const OdGeScale3d& sclVec) const;
  bool isEqualTo(const OdGeScale3d& scaleVec, const OdGeTol& tol = OdGeContext::gTol) const;
  
  double operator [](unsigned int i) const { return * (&sx+i); }
  double& operator [](unsigned int i) { return * (&sx+i); }
  OdGeScale3d& set(double sc0, double sc1, double sc2) { sx = sc0; sy = sc1; sz = sc2; return *this; }
  
  operator OdGeMatrix3d() const;
  void getMatrix(OdGeMatrix3d& mat) const;
  OdGeScale3d& extractScale(const OdGeMatrix3d& mat);
  OdGeScale3d& removeScale(OdGeMatrix3d& mat);
  
  bool isValid() const { return OdNonZero(sx) && OdNonZero(sy) && OdNonZero(sz); }

  double sx, sy, sz;
};

#include "DD_PackPop.h"

#endif // OD_GESCL3D_H


