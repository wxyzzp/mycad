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



#ifndef OD_GELINE3D_H
#define OD_GELINE3D_H

class OdGeLine2d;

#include "GeLinearEnt3d.h"

#include "DD_PackPush.h"

/** Description:
    
    {group:OdGe_Classes} 
*/
class GE_TOOLKIT_EXPORT OdGeLine3d : public OdGeLinearEnt3d
{
public:
  virtual ~OdGeLine3d() {}
	OdGeLine3d() {}
	OdGeLine3d(const OdGeLine3d& line);
	OdGeLine3d(const OdGePoint3d& pnt, const OdGeVector3d& vec)
  { set(pnt, vec);}
  
	OdGeLine3d(const OdGePoint3d& pnt1, const OdGePoint3d& pnt2)
  { set(pnt1, pnt2);}

  virtual OdGeEntity3d* copy() const;
	
	static const OdGeLine3d kXAxis;
	static const OdGeLine3d kYAxis;
	static const OdGeLine3d kZAxis;

  virtual OdGe::EntityId type() const { return OdGe::kLine3d;};
	
	OdGeLine3d& set(const OdGePoint3d& pnt, const OdGeVector3d& vec);

	OdGeLine3d& set(const OdGePoint3d& pnt1, const OdGePoint3d& pnt2)
  { return set(pnt1, pnt2 - pnt1);}

  // there is no such method!!
  // OdGeVector3d direction() const {return m_Direction.normal();}
	
	// Assignment operator.
  OdGeLine3d& operator = (const OdGeLine3d& line);

  virtual bool isOn(const OdGePoint3d& pnt, const OdGeTol& tol = OdGeContext::gTol) const;

  virtual OdGePoint3d evalPoint(double param) const;
  virtual double paramOf(const OdGePoint3d& , const OdGeTol& tol = OdGeContext::gTol) const;
  virtual void getInterval(OdGeInterval& interval) const;
  virtual void appendSamplePoints(double , double , double , 
		OdGePoint3dArray& , OdGeDoubleArray* = 0) const;

  DD_USING(OdGeLinearEnt3d::appendSamplePoints);

  bool hasStartPoint(OdGePoint3d& startPnt) const;
  bool hasEndPoint(OdGePoint3d& endPnt) const;
protected:
  // OdGeVector3d  m_Direction;
};

#include "DD_PackPop.h"

#endif // OD_GELINE3D_H

