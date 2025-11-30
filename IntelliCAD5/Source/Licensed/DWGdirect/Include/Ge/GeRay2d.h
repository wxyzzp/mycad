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



#ifndef OD_GE_RAY_2D_H
#define OD_GE_RAY_2D_H

#include "GeLinearEnt2d.h"

#include "DD_PackPush.h"

/** Description:
    
    {group:OdGe_Classes} 
*/
class GE_TOOLKIT_EXPORT OdGeRay2d : public OdGeLinearEnt2d
{
public:
  OdGe::EntityId type() const;

  inline OdGeRay2d& set(const OdGePoint2d& pnt, const OdGeVector2d& vec);
  inline OdGeRay2d& set(const OdGePoint2d& pnt1, const OdGePoint2d& pnt2);

  OdGeRay2d() { set(OdGePoint2d::kOrigin, OdGeVector2d::kXAxis); }
	OdGeRay2d(const OdGePoint2d& pnt, const OdGeVector2d& vec);
	OdGeRay2d(const OdGePoint2d& pnt1, const OdGePoint2d& pnt2);
	
  void getInterval(OdGeInterval& intrvl) const;

  bool intersectWith(const OdGeLinearEnt2d& line, OdGePoint2d& intPnt, const OdGeTol& tol = OdGeContext::gTol) const;
  bool isClosed (const OdGeTol&) const;
};

inline OdGeRay2d::OdGeRay2d(const OdGePoint2d& pnt, const OdGeVector2d& vec) { set(pnt, vec); }

inline OdGeRay2d::OdGeRay2d(const OdGePoint2d& pnt1, const OdGePoint2d& pnt2) { set(pnt1, pnt2); }

inline OdGeRay2d& OdGeRay2d::set(const OdGePoint2d& pnt, const OdGeVector2d& vec)
{
  m_point1 = m_point2 = pnt;
  m_point2 += vec;
  return *this;
}

inline OdGeRay2d& OdGeRay2d::set(const OdGePoint2d& pnt1, const OdGePoint2d& pnt2)
{
  m_point1 = pnt1;
  m_point2 = pnt2;
  return *this;
}

#include "DD_PackPop.h"

#endif // OD_GE_RAY_2D_H


