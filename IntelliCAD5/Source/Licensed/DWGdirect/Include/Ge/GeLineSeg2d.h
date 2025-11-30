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



#ifndef OD_GE_LINE_SEG_2D_H
#define OD_GE_LINE_SEG_2D_H

#include "GeLinearEnt2d.h"

#include "DD_PackPush.h"

/** Description:
    
    {group:OdGe_Classes} 
*/
class GE_TOOLKIT_EXPORT OdGeLineSeg2d : public OdGeLinearEnt2d
{
public:
  OdGeLineSeg2d() {}
  OdGeLineSeg2d(const OdGeLineSeg2d& line);
  OdGeLineSeg2d(const OdGePoint2d& pnt1, const OdGePoint2d& pnt2);
  OdGeLineSeg2d(const OdGePoint2d& pnt, const OdGeVector2d& vec);

  virtual ~OdGeLineSeg2d() {}
  virtual OdGeEntity2d* copy() const   { return new OdGeLineSeg2d(*this); }

  OdGe::EntityId type() const          { return OdGe::kLineSeg2d;}

	OdGeLineSeg2d& set(const OdGePoint2d& pnt, const OdGeVector2d& vec);
  OdGeLineSeg2d& set(const OdGePoint2d& pnt1, const OdGePoint2d& pnt2);
  OdGeLineSeg2d& set(const OdGeCurve2d& curve1, const OdGeCurve2d& curve2,
                     double& param1, double& param2, bool& success);
  OdGeLineSeg2d& set(const OdGeCurve2d& curve, const OdGePoint2d& point, double& param, bool& success);

  void getBisector(OdGeLine2d& line) const;
  
  OdGePoint2d baryComb(double blendCoeff) const;
  
  OdGePoint2d startPoint() const { return m_point1; }
  OdGePoint2d midPoint() const;
  OdGePoint2d endPoint() const { return m_point2; }

  bool isEqualTo(const OdGeLineSeg2d& ls,  const OdGeTol& tol = OdGeContext::gTol) const;
  bool operator==(const OdGeLineSeg2d& entity) const;
  bool operator!=(const OdGeLineSeg2d& entity) const;
	
  double length() const;
  double length(double fromParam, double toParam, double tol = OdGeContext::gTol.equalPoint()) const;

  //virtual bool overlap(const OdGeLinearEnt2d& line, OdGeLinearEnt2d*& overlap,
  //                     const OdGeTol& tol = OdGeContext::gTol) const;

  virtual OdGeEntity2d& transformBy(const OdGeMatrix2d& xfm);
  virtual void getInterval(OdGeInterval& intrvl) const;
  virtual bool isOn(const OdGePoint2d& pnt, const OdGeTol& tol = OdGeContext::gTol) const;
  virtual bool intersectWith(const OdGeLinearEnt2d& line, OdGePoint2d& intPnt,
                             const OdGeTol& tol = OdGeContext::gTol) const;
  //:>OdGeLineSeg2d& operator =(const OdGeLineSeg2d& line);

  bool hasStartPoint(OdGePoint2d& startPnt) const;
  bool hasEndPoint(OdGePoint2d& endPnt) const;
  bool isClosed (const OdGeTol& tol = OdGeContext::gTol) const;

};

#include "DD_PackPop.h"

#endif // OD_GE_LINE_SEG_2D_H


