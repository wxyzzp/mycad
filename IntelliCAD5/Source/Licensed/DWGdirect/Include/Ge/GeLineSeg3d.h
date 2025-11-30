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



#ifndef OD_GELNSG3D_H
#define OD_GELNSG3D_H

class OdGeLineSeg2d;

#include "GeLinearEnt3d.h"
#include "GeInterval.h"

#include "DD_PackPush.h"

/** Description:
    
    {group:OdGe_Classes} 
*/
class GE_TOOLKIT_EXPORT OdGeLineSeg3d : public OdGeLinearEnt3d
{
  OdGeInterval m_Interval;
public:
  
  virtual ~OdGeLineSeg3d() {}
  
  OdGeLineSeg3d();
  OdGeLineSeg3d(const OdGeLineSeg3d& line);
  
  OdGeLineSeg3d(const OdGePoint3d& pnt, const OdGeVector3d& vec);

  OdGeLineSeg3d(const OdGePoint3d& pnt1, const OdGePoint3d& pnt2);

  virtual OdGeEntity3d* copy() const;
  //    void           getBisector  (OdGePlane& plane) const;
  
  //    OdGePoint3d    baryComb     (double blendCoeff) const;
  
  OdGePoint3d    startPoint   () const;
  OdGePoint3d    midPoint     () const;
  OdGePoint3d    endPoint     () const;
  double         length       () const;

  //    double         length       (double fromParam, double toParam,
  //                                 double tol = OdGeContext::gTol.equalPoint()) const;

  OdGeLineSeg3d& set          (const OdGePoint3d& pnt, const OdGeVector3d& vec);
  OdGeLineSeg3d& set          (const OdGePoint3d& pnt1, const OdGePoint3d& pnt2);

  //     OdGeLineSeg3d& set          (const OdGeCurve3d& curve1,
  //                                 const OdGeCurve3d& curve2,
  //                                 double& param1, double& param2,
  //                                 bool& success);
  //    OdGeLineSeg3d& set          (const OdGeCurve3d& curve,
  //                                 const OdGePoint3d& point, double& param,
  //                                 bool& success);
  
  
  // Assignment operator.
  OdGeLineSeg3d& operator =   (const OdGeLineSeg3d& line);
  
  OdGe::EntityId type() const;
  
  OdGeVector3d direction() const;

  // NB: this implementation doesn't return overlap region,
  // since it is needn't for us now.
  bool overlap(const OdGeLinearEnt3d& line,
    OdGeLinearEnt3d& overlap,
    const OdGeTol& tol = OdGeContext::gTol) const;

  // Matrix multiplication(virtual oberride)
  OdGeEntity3d& transformBy(const OdGeMatrix3d& xfm);

  bool isOn(const OdGePoint3d& pnt, const OdGeTol& tol = OdGeContext::gTol) const;

  OdGeCurve3d& reverseParam();
  virtual void getInterval(OdGeInterval&) const;
	virtual void getInterval(OdGeInterval& intrvl, OdGePoint3d& start, OdGePoint3d& end) const;
  virtual bool setInterval(const OdGeInterval&);
  OdGeCurve3d& setInterval();

  OdGePoint3d evalPoint(double param) const;
	virtual double paramOf(const OdGePoint3d& , const OdGeTol& = OdGeContext::gTol) const;

  bool intersectWith(const OdGeLinearEnt3d& line,
    OdGePoint3d& intPt,
    const OdGeTol& tol = OdGeContext::gTol) const;

	// Polygonize curve to within a specified tolerance.
	//
  void appendSamplePoints(double fromParam, double toParam, double approxEps, 
		OdGePoint3dArray& pointArray, OdGeDoubleArray* pParamArray = 0) const;

  DD_USING(OdGeLinearEnt3d::appendSamplePoints);

  bool hasStartPoint(OdGePoint3d& startPnt) const;
  bool hasEndPoint(OdGePoint3d& endPnt) const;
};

#include "DD_PackPop.h"

#endif


