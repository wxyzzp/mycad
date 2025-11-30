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



#ifndef OD_GE_LINEAR_ENT_2D_H
#define OD_GE_LINEAR_ENT_2D_H


class OdGeCircArc2d;

#include "GeCurve2d.h"
#include <memory.h> // for memcpy

#include "DD_PackPush.h"

/** Description:
    
    {group:OdGe_Classes} 
*/
class GE_TOOLKIT_EXPORT OdGeLinearEnt2d : public OdGeCurve2d
{
  virtual void getInterval(OdGeInterval& intrvl) const = 0;
  virtual bool checkInterval(const OdGePoint2d& pntOn, const OdGeTol& tol = OdGeContext::gTol) const;
public:
  void getInterval(OdGeInterval& intrvl, OdGePoint2d& start, OdGePoint2d& end) const;
  virtual bool intersectWith(const OdGeLinearEnt2d& line, OdGePoint2d& intPnt,
                             const OdGeTol& tol = OdGeContext::gTol) const;
  /*
  virtual bool overlap(const OdGeLinearEnt2d& line, OdGeLinearEnt2d*& overlap,
                       const OdGeTol& tol = OdGeContext::gTol) const = 0;
                       */
  bool isParallelTo(const OdGeLinearEnt2d& line, const OdGeTol& tol = OdGeContext::gTol) const;
  bool isPerpendicularTo(const OdGeLinearEnt2d& line, const OdGeTol& tol = OdGeContext::gTol) const;
  bool isColinearTo(const OdGeLinearEnt2d& line, const OdGeTol& tol = OdGeContext::gTol) const;
  void getPerpLine(const OdGePoint2d& pnt, OdGeLine2d& perpLine) const;
  
  OdGePoint2d pointOnLine() const;
  OdGeVector2d direction() const;
  void getLine(OdGeLine2d& line) const;

  virtual OdGePoint2d evalPoint(double param) const;
  virtual void appendSamplePoints(double fromParam, double toParam,
    double approxEps, OdGePoint2dArray& pointArray,
    OdGeDoubleArray* pParamArray = 0) const;
  virtual void appendSamplePoints(int numSample, OdGePoint2dArray&pointArray) const;
    
  double paramOf(const OdGePoint2d& pnt, const OdGeTol& tol = OdGeContext::gTol) const;
  virtual bool isOn(const OdGePoint2d& pnt, const OdGeTol& tol = OdGeContext::gTol) const;

  
protected:
  OdGeLinearEnt2d() {}
  OdGeLinearEnt2d(const OdGeLinearEnt2d& src) { memcpy (this, &src, sizeof (src)); }
  OdGePoint2d m_point1, m_point2;
  friend class OdGeLine2d;
  friend class OdGeLineSeg2d;
  friend class OdGeRay2d;
};

#include "DD_PackPop.h"

#endif


