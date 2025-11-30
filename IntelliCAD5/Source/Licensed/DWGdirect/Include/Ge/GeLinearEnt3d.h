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



#ifndef OD_GELENT3D_H
#define OD_GELENT3D_H

class OdGeLine3d;
class OdGeCircArc3d;
class OdGePlanarEnt;

#include "OdPlatform.h"

#include "GeCurve3d.h"

#include "DD_PackPush.h"

/** Description:
    
    {group:OdGe_Classes} 
*/
class GE_TOOLKIT_EXPORT OdGeLinearEnt3d : public OdGeCurve3d
{
public:
  bool intersectWith(const OdGeLinearEnt3d& line,
    OdGePoint3d& intPt,
    const OdGeTol& tol = OdGeContext::gTol) const;
  bool intersectWith(const OdGePlanarEnt& plane, OdGePoint3d& intPnt,
    const OdGeTol& tol = OdGeContext::gTol) const;
  
  bool projIntersectWith(const OdGeLinearEnt3d& line,
    const OdGeVector3d& projDir,
    OdGePoint3d& pntOnThisLine,
    OdGePoint3d& pntOnOtherLine,
    const OdGeTol& tol = OdGeContext::gTol) const;
  
  /* bool overlap(const OdGeLinearEnt3d& line,
    OdGeLinearEnt3d*& overlap,
    const OdGeTol& tol = OdGeContext::gTol) const;*/
  
  bool isOn(const OdGePoint3d& pnt, const OdGeTol& tol = OdGeContext::gTol) const;
  /*:>bool isOn(const OdGePoint3d& pnt, double& param,
    const OdGeTol& tol = OdGeContext::gTol) const;
  bool isOn(double param,
    const OdGeTol& tol = OdGeContext::gTol) const;
  bool isOn(const OdGePlane& plane,
    const OdGeTol& tol = OdGeContext::gTol) const;*/
  
  bool isParallelTo(const OdGeLinearEnt3d& line,
    const OdGeTol& tol = OdGeContext::gTol) const;
  bool isParallelTo(const OdGePlanarEnt& plane,
    const OdGeTol& tol = OdGeContext::gTol) const;
  bool isPerpendicularTo(const OdGeLinearEnt3d& line,
    const OdGeTol& tol = OdGeContext::gTol) const;
  bool isPerpendicularTo(const OdGePlanarEnt& plane,
    const OdGeTol& tol = OdGeContext::gTol) const;
  
#ifdef OD_USE_STUB_FNS
  bool isColinearTo(const OdGeLinearEnt3d& line,
    const OdGeTol& tol = OdGeContext::gTol) const { return false; }
#endif

  void getPerpPlane(const OdGePoint3d& pnt, OdGePlane& plane) const;
  
  OdGePoint3d pointOnLine() const { return m_start;}
  virtual OdGeVector3d direction() const;

  void getLine(OdGeLine3d&) const;
  
  // Assignment operator.
  OdGeLinearEnt3d& operator =(const OdGeLinearEnt3d& line);
  
  void appendSamplePoints(double fromParam, double toParam, double approxEps, 
		OdGePoint3dArray& pointArray, OdGeDoubleArray* pParamArray = 0) const;

  DD_USING(OdGeCurve3d::appendSamplePoints);

	OdGePoint3d evalPoint(double param) const;

  void getInterval(OdGeInterval&) const;

  virtual OdGeEntity3d& transformBy(const OdGeMatrix3d& );

protected:
  OdGeLinearEnt3d() {}
  OdGeLinearEnt3d(const OdGeLinearEnt3d&);

  OdGePoint3d  m_start;
  OdGeVector3d m_vToEnd;
};

#include "DD_PackPop.h"

#endif // OD_GELENT3D_H


