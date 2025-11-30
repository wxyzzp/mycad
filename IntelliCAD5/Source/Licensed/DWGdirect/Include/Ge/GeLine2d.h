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



#ifndef OD_GE_LINE_2D_H
#define OD_GE_LINE_2D_H

#include "GeLinearEnt2d.h"

#include "DD_PackPush.h"

/** Description:
    
    {group:OdGe_Classes} 
*/
class GE_TOOLKIT_EXPORT OdGeLine2d : public OdGeLinearEnt2d
{
  bool checkInterval(const OdGePoint2d& pntOn,
                     const OdGeTol& tol = OdGeContext::gTol) const;
public:
	OdGeLine2d() {}
	OdGeLine2d(const OdGeLine2d& line);
	OdGeLine2d(const OdGePoint2d& pnt, const OdGeVector2d& vec);
	OdGeLine2d(const OdGePoint2d& pnt1, const OdGePoint2d& pnt2);
	
	static const OdGeLine2d kXAxis;
	static const OdGeLine2d kYAxis;
  OdGe::EntityId type() const;
	
	OdGeLine2d& set(const OdGePoint2d& pnt, const OdGeVector2d& vec);
	OdGeLine2d& set(const OdGePoint2d& pnt1, const OdGePoint2d& pnt2);
	
	OdGeLine2d& operator = (const OdGeLine2d& line);

  virtual bool overlap(const OdGeLinearEnt2d& line, OdGeLinearEnt2d*& overlap,
                       const OdGeTol& tol = OdGeContext::gTol) const;

  virtual void getInterval(OdGeInterval& intrvl) const;

  bool hasStartPoint(OdGePoint2d& startPnt) const;
  bool hasEndPoint(OdGePoint2d& endPnt) const;
  bool isClosed (const OdGeTol& tol = OdGeContext::gTol) const;

  bool isOn(const OdGePoint2d& pnt, const OdGeTol& tol = OdGeContext::gTol) const;

  double distanceTo(const OdGePoint2d& pnt, const OdGeTol& tol = OdGeContext::gTol) const;
};

#include "DD_PackPop.h"

#endif // OD_GE_LINE_2D_H



