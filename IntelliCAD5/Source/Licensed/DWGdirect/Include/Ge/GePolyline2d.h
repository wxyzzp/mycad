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



#ifndef OD_GE_POLYLINE_2D_H
#define OD_GE_POLYLINE_2D_H

#include "OdPlatform.h"

#include "GeSplineEnt2d.h"

#include "DD_PackPush.h"

/** Description:
    
    {group:OdGe_Classes} 
*/
class GE_TOOLKIT_EXPORT OdGePolyline2d : public OdGeSplineEnt2d
{
public:
	OdGePolyline2d() : m_isClosed(false) {}
	OdGePolyline2d(const OdGePolyline2d& src)
	{
		*this = src;
	}
	OdGePolyline2d(const OdGePoint2dArray&);
	OdGePolyline2d(const OdGeKnotVector& knots,
		const OdGePoint2dArray& points);

	OdGe::EntityId type  () const  { return OdGe::kPolyline2d;}

	OdGePolyline2d(const OdGeCurve2d& crv, double apprEps);

	int              numFitPoints  () const;
	OdGePoint2d      fitPointAt   (int idx) const;
	OdGeSplineEnt2d& setFitPointAt(int idx, const OdGePoint2d& point);

	OdGePolyline2d& setClosed(bool flag) {m_isClosed = flag; return *this;}
	bool isClosed(const OdGeTol& = OdGeContext::gTol) const {return m_isClosed;}
	OdGeDoubleArray& bulges() {return m_bulges;}
	const OdGeDoubleArray& getBulges() const {return m_bulges;}
	OdGePoint2dArray& vertices() {return m_vertices;}
	const OdGePoint2dArray& getVertices() const {return m_vertices;}
	bool hasBulges() const;

  // OdGeCurve2d functions
  void getInterval(OdGeInterval& intrvl) const;

  virtual double paramOf(const OdGePoint2d& pnt,
    const OdGeTol& tol = OdGeContext::gTol) const;

  virtual OdGePoint2d evalPoint(double param) const;

  virtual void appendSamplePoints(double fromParam, double toParam,
    double approxEps, OdGePoint2dArray& pointArray,
    OdGeDoubleArray* pParamArray = 0) const;


  virtual OdGeEntity2d& transformBy(const OdGeMatrix2d& xfm);

  bool hasStartPoint(OdGePoint2d& startPnt) const;
  bool hasEndPoint(OdGePoint2d& endPnt) const;

  DD_USING(OdGeSplineEnt2d::appendSamplePoints);

private:
  OdGeDoubleArray m_bulges;
  OdGePoint2dArray m_vertices;
  bool m_isClosed;
};

#include "DD_PackPop.h"

#endif // OD_GE_POLYLINE_2D_H


