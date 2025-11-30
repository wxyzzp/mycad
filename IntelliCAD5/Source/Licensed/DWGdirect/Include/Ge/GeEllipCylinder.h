//////////////////////////////////////////////////////////////////////////////
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
// AE 08.09.2003 - Begin 


#ifndef OD_GEELLIPCYLNDR_H
#define OD_GEELLIPCYLNDR_H




#include "GeSurface.h"
#include "GeInterval.h"
#include "OdPlatformSettings.h"

class OdGeCircArc3d;

#include "DD_PackPush.h"
    
/** Description:
    
    {group:OdGe_Classes} 
*/
class GE_TOOLKIT_EXPORT OdGeEllipCylinder : public OdGeSurface
{
public:
  OdGeEllipCylinder();
  OdGeEllipCylinder(double minorRadius, double majorRadius,
										const OdGePoint3d& origin,
										const OdGeVector3d& axisOfSymmetry);

  OdGeEllipCylinder(double minorRadius, double majorRadius,
										const OdGePoint3d& origin,
 									  const OdGeVector3d& axisOfSymmetry,
									  const OdGeVector3d& majorAxis,
										const OdGeInterval& height,
										double startAngle, double endAngle);
  
  // Run time type information.
  bool isKindOf(OdGe::EntityId entType) const;
  OdGe::EntityId type() const;

  virtual OdGePoint2d paramOf(const OdGePoint3d& pnt,
    const OdGeTol& tol = OdGeContext::gTol) const;

  virtual OdGePoint3d evalPoint(const OdGePoint2d& param) const;
  virtual OdGePoint3d evalPoint(const OdGePoint2d& param, int derivOrd,
    OdGeVector3dArray& derivatives, OdGeVector3d& normal) const;
  DD_USING(OdGeSurface::evalPoint);

  // Make a copy of the entity.
  OdGeEntity3d* copy() const;

  // Geometric properties.
  //
	inline double				radiusRatio() const;
	inline double				minorRadius() const;
	inline double				majorRadius() const;

  inline OdGePoint3d    origin        () const;
  inline void    getAngles     (double& start, double& end) const;
  inline void    getHeight     (OdGeInterval& height) const;
  inline double  heightAt      (double u) const;
  inline OdGeVector3d   axisOfSymmetry() const;
  inline OdGeVector3d   majorAxis       () const;
  OdGeVector3d    minorAxis() const;
  bool           isOuterNormal  () const;
  inline bool    isClosed      (const OdGeTol& tol = OdGeContext::gTol) const;
  bool           isClosedInU   (const OdGeTol& tol = OdGeContext::gTol) const;
  bool           isClosedInV   (const OdGeTol& tol = OdGeContext::gTol) const;

  void           setIsOuterNormal(bool isOuterNormal);

	inline OdGeEllipCylinder&  setMinorRadius(double radius);
	inline OdGeEllipCylinder&  setMajorRadius(double radius);

  OdGeEllipCylinder&  setAngles     (double start, double end);
  OdGeEllipCylinder&  setHeight     (const OdGeInterval& height);
  OdGeEllipCylinder&  set           (double minorRadius, double majorRadius, 
																		 const OdGePoint3d& origin, const OdGeVector3d& axisOfSym);
  OdGeEllipCylinder&  set           (double minorRadius, double majorRadius,
																		const OdGePoint3d& origin,
																		const OdGeVector3d& axisOfSym,
																		const OdGeVector3d& majorAxis,
																		const OdGeInterval& height,
																		double startAngle, double endAngle);
 
  // Intersection with a linear entity
  //
  bool intersectWith (const OdGeLinearEnt3d& linEnt, int& intn,
                                OdGePoint3d& p1, OdGePoint3d& p2,
                                const OdGeTol& tol = OdGeContext::gTol) const;

  void getEnvelope(OdGeInterval& intrvlU, OdGeInterval& intrvlV) const;
  
  virtual bool project(const OdGePoint3d& p, OdGePoint3d& projP) const;
private:
  double        m_minorRadius;
	double				m_majorRadius;
  double        m_startAngle;
  double        m_endAngle;
  OdGeInterval  m_height;
  OdGeVector3d  m_axisOfSymmetry;
  OdGeVector3d  m_majorAxis;
  OdGePoint3d   m_origin;
};

inline OdGePoint3d OdGeEllipCylinder::origin() const 
{
	return m_origin; 
}

inline void OdGeEllipCylinder::getAngles(double& start, double& end) const
{
	start = m_startAngle; end = m_endAngle; 
}

inline void OdGeEllipCylinder::getHeight(OdGeInterval& range) const 
{ 
  range = m_height;
}

inline double OdGeEllipCylinder::heightAt(double u) const
{
  return u;
}

inline OdGeVector3d OdGeEllipCylinder::axisOfSymmetry() const 
{
	return m_axisOfSymmetry; 
}

inline OdGeVector3d OdGeEllipCylinder::majorAxis() const 
{
	return m_majorAxis; 
}

inline bool OdGeEllipCylinder::isClosed(const OdGeTol&) const
{ 
	return OdCmpDouble(m_endAngle - m_startAngle, 2.0 * OdaPI) != -1; 
}

inline OdGeEllipCylinder& OdGeEllipCylinder::setMinorRadius(double minorRadius)
{
  m_minorRadius = fabs(minorRadius);
  return *this;
}

inline OdGeEllipCylinder& OdGeEllipCylinder::setMajorRadius(double majorRadius)
{
  if (isOuterNormal())
    m_majorRadius = majorRadius < 0.0 ? majorRadius : -majorRadius;
  else
    m_majorRadius = majorRadius > 0.0 ? majorRadius : -majorRadius;
  return *this;
}

inline double	OdGeEllipCylinder::radiusRatio() const
{
	return m_minorRadius / fabs(m_majorRadius);
}

inline double	OdGeEllipCylinder::minorRadius() const
{
	return m_minorRadius;
}

inline double	OdGeEllipCylinder::majorRadius() const
{
	return fabs(m_majorRadius);
}


#include "DD_PackPop.h"

#endif // OD_GEELLIPCYLNDR_H
// AE - End 

