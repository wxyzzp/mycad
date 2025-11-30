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

// AE 02.09.2003 - Begin 

#ifndef OD_GEELLIPCONE_H
#define OD_GEELLIPCONE_H

//
// Description:
//



#include "GeSurface.h"
#include "GeInterval.h"
#include "OdPlatformSettings.h"

class OdGePoint3d;
class OdGeVector3d;
class OdGeCircArc3d;
class OdGeLinearEnt3d;

#include "DD_PackPush.h"

/** Description:
    
    {group:OdGe_Classes} 
*/
class GE_TOOLKIT_EXPORT OdGeEllipCone : public OdGeSurface
{
public:
  OdGeEllipCone();
  OdGeEllipCone(double cosineAngle, double sineAngle,
								 const  OdGePoint3d& baseOrigin, double minorRadius,
								 double majorRadius, const  OdGeVector3d& axisOfSymmetry);

  OdGeEllipCone(double cosineAngle, double sineAngle,
           const  OdGePoint3d& baseOrigin, double minorRadius,
					 double majorRadius, const  OdGeVector3d& axisOfSymmetry,
           const  OdGeVector3d& majorAxis, const  OdGeInterval& height,
           double startAngle, double endAngle);
  
  // Run time type information.
  bool isKindOf(OdGe::EntityId entType) const;
  OdGe::EntityId type() const;
  
  // Make a copy of the entity.
  OdGeEntity3d* copy() const;

  // Geometric properties.
  //
	double				radiusRatio() const;
	double				minorRadius() const;
	double				majorRadius() const;
  OdGePoint3d   baseCenter    () const;
  void          getAngles     (double& start, double& end) const;
  double        halfAngle     () const;
  void          getHalfAngle  (double& cosineAngle, double& sineAngle) const;
  void          getHeight     (OdGeInterval& range) const;
  double        heightAt      (double u) const;
  OdGeVector3d  axisOfSymmetry() const;
  OdGeVector3d  majorAxis       () const;
  OdGeVector3d  minorAxis       () const;
  OdGePoint3d   apex          () const;
  bool          isClosed      (const OdGeTol& tol = OdGeContext::gTol) const;
  bool          isClosedInU   (const OdGeTol& tol = OdGeContext::gTol) const;
  bool          isClosedInV   (const OdGeTol& tol = OdGeContext::gTol) const;
  bool          isOuterNormal  () const;
  
	OdGeEllipCone&     setMinorRadius(double radius);
	OdGeEllipCone&     setMajorRadius(double radius);
  OdGeEllipCone&     setAngles    (double startAngle, double endAngle);
  OdGeEllipCone&     setHeight    (const OdGeInterval& height);
  OdGeEllipCone&     set          (double cosineAngle, double sineAngle,
																		const  OdGePoint3d& baseCenter,
																		double minorRadius, double majorRadius,
																		const  OdGeVector3d& axisOfSymmetry);
  
  OdGeEllipCone&     set          (double cosineAngle, double sineAngle,
																		const  OdGePoint3d& baseCenter,
																		double minorRadius, double majorRadius,
																		const  OdGeVector3d& axisOfSymmetry,
																		const  OdGeVector3d& majorAxis,
																		const  OdGeInterval& height,
																		double startAngle, double endAngle);

  virtual void getEnvelope  (OdGeInterval& intrvlU, OdGeInterval& intrvlV) const;

  virtual OdGePoint3d evalPoint(const OdGePoint2d&) const;
  virtual OdGePoint3d evalPoint(const OdGePoint2d& param, int derivOrd,
    OdGeVector3dArray& derivatives, OdGeVector3d& normal) const;
  DD_USING(OdGeSurface::evalPoint);

  virtual OdGePoint2d paramOf(const OdGePoint3d& , const OdGeTol& tol = OdGeContext::gTol) const;

  virtual bool project(const OdGePoint3d& p, OdGePoint3d& projP) const;
private:
  double        m_sineAngle;
	double        m_cosineAngle;
	double        m_minorRadius;
	double				m_majorRadius;
	double        m_startAngle;
	double        m_endAngle;
	
  OdGeInterval  m_height;
  OdGeVector3d  m_axisOfSymmetry;
  OdGeVector3d  m_majorAxis;
  OdGePoint3d   m_baseOrigin;
};

inline double OdGeEllipCone::radiusRatio() const 
{
	return m_minorRadius / m_majorRadius; 
}

inline double OdGeEllipCone::minorRadius() const 
{
	return m_minorRadius;
}

inline double OdGeEllipCone::majorRadius() const 
{
	return m_majorRadius;
}


inline OdGePoint3d OdGeEllipCone::baseCenter() const 
{
	return m_baseOrigin; 
}

inline void OdGeEllipCone::getAngles(double& start, double& end) const
{
	start = m_startAngle; end = m_endAngle; 
}

inline void OdGeEllipCone::getHalfAngle(double& cosineAngle, double& sineAngle) const
{
	cosineAngle = m_cosineAngle; sineAngle = m_sineAngle; 
}

inline void OdGeEllipCone::getHeight(OdGeInterval& range) const 
{
	range = m_height; 
}

inline double OdGeEllipCone::heightAt(double u) const 
{
	return u; 
}

inline OdGeVector3d OdGeEllipCone::axisOfSymmetry() const 
{
	return m_axisOfSymmetry; 
}

inline OdGeVector3d OdGeEllipCone::majorAxis() const 
{
	return m_majorAxis; 
}

inline OdGePoint3d OdGeEllipCone::apex() const
{ 
	return m_baseOrigin - m_axisOfSymmetry * (majorRadius() * fabs(m_cosineAngle) / m_sineAngle); 
}

inline bool OdGeEllipCone::isClosed(const OdGeTol&) const
{
	return !(m_endAngle - m_startAngle < 2.0 * OdaPI); 
}


#include "DD_PackPop.h"

#endif // OD_GEELLIPCONE_H

// AE - End 

