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



#ifndef OD_GEVEC3D_H
#define OD_GEVEC3D_H


#include "GeGbl.h"
#include "Ge/GeVector2d.h" // for convert2d

class OdGeMatrix3d;
class OdGePlane;
class OdGePlanarEnt;

#include "DD_PackPush.h"

/** Description:
    
    {group:OdGe_Classes} 
*/
class GE_TOOLKIT_EXPORT OdGeVector3d
{
public:
	OdGeVector3d();
	OdGeVector3d(const OdGeVector3d& vec);
	OdGeVector3d(double x, double y, double z);
	OdGeVector3d(const OdGePlanarEnt&, const OdGeVector2d&);

  inline OdGeVector3d& operator=(const OdGeVector3d& vec);

	static const   OdGeVector3d kIdentity;    // (0,0,0)
	static const   OdGeVector3d kXAxis;
	static const   OdGeVector3d kYAxis;
	static const   OdGeVector3d kZAxis;
	
	OdGeVector3d&  setToProduct(const OdGeMatrix3d& mat, const OdGeVector3d& vec);
	
	OdGeVector3d&  transformBy (const OdGeMatrix3d& leftSide);
	OdGeVector3d&  rotateBy    (double ang , const OdGeVector3d& axis);
	OdGeVector3d&  mirror      (const OdGeVector3d& normalToPlane);
	OdGeVector2d   convert2d   (const OdGePlanarEnt& pln) const;
  OdGeVector2d   convert2d   () const; // returns projection to XY plane
	
	OdGeVector3d   operator *  (double scl) const;
	OdGeVector3d&  operator *= (double scl);
	OdGeVector3d&  setToProduct(const OdGeVector3d& vec, double scl);
	OdGeVector3d   operator /  (double scl) const;
	OdGeVector3d&  operator /= (double scl);
	
	OdGeVector3d   operator +  (const OdGeVector3d& vec) const;
	OdGeVector3d&  operator += (const OdGeVector3d& vec);
	OdGeVector3d   operator -  (const OdGeVector3d& vec) const;
	OdGeVector3d&  operator -= (const OdGeVector3d& vec);
	OdGeVector3d&  setToSum    (const OdGeVector3d& vec1, const OdGeVector3d& vec2);
	OdGeVector3d   operator -  () const;
	OdGeVector3d&  negate      ();
	
	OdGeVector3d   perpVector  () const;
	
	double         angleTo     (const OdGeVector3d& vec) const;
	double         angleTo     (const OdGeVector3d& vec,
		const OdGeVector3d& refVec) const;
	double         angleOnPlane(const OdGePlanarEnt& pln) const;
	
	OdGeVector3d   normal      (const OdGeTol& tol = OdGeContext::gTol) const;
	OdGeVector3d&  normalize   (const OdGeTol& tol = OdGeContext::gTol);
	OdGeVector3d&  normalize   (const OdGeTol& tol,  OdGe::ErrorCondition& flag);
	double         length      () const;
	double         lengthSqrd  () const;
	bool           isUnitLength(const OdGeTol& tol = OdGeContext::gTol) const;
	bool           isZeroLength(const OdGeTol& tol = OdGeContext::gTol) const;
	
	bool           isParallelTo(const OdGeVector3d& vec,
		const OdGeTol& tol = OdGeContext::gTol) const;
	bool           isParallelTo(const OdGeVector3d& vec,
		const OdGeTol& tol, OdGeError& flag) const;
	bool          isCodirectionalTo(const OdGeVector3d& vec,
		const OdGeTol& tol = OdGeContext::gTol) const;
	bool          isCodirectionalTo(const OdGeVector3d& vec,
		const OdGeTol& tol, OdGeError& flag) const;
	bool          isPerpendicularTo(const OdGeVector3d& vec,
		const OdGeTol& tol = OdGeContext::gTol) const;
	bool          isPerpendicularTo(const OdGeVector3d& vec,
		const OdGeTol& tol, OdGeError& flag) const;
	double         dotProduct  (const OdGeVector3d& vec) const;
	OdGeVector3d   crossProduct(const OdGeVector3d& vec) const;
//	OdGeMatrix3d   rotateTo    (const OdGeVector3d& vec, const OdGeVector3d& axis
//		= OdGeVector3d::kIdentity) const;
	
//	OdGeVector3d   project      (const OdGeVector3d& planeNormal,
//		const OdGeVector3d& projectDirection) const;
//	OdGeVector3d   project      (const OdGeVector3d& planeNormal,
//		const OdGeVector3d& projectDirection,  
//		const OdGeTol& tol, OdGeError& flag) const;
//	OdGeVector3d   orthoProject (const OdGeVector3d& planeNormal) const;
//	OdGeVector3d   orthoProject (const OdGeVector3d& planeNormal, 
//		const OdGeTol& tol, OdGeError& flag) const;

  bool          operator == (const OdGeVector3d& vec) const;
	bool          operator != (const OdGeVector3d& vec) const;
	bool          isEqualTo   (const OdGeVector3d& vec,
		const OdGeTol& tol = OdGeContext::gTol) const;
	
	double         operator [] (unsigned int i) const;
	double&        operator [] (unsigned int i);
	unsigned int   largestElement() const;
	OdGeVector3d&  set         (double x, double y, double z);
	OdGeVector3d&  set         (const OdGePlanarEnt& pln, const OdGeVector2d& vec);
	
	operator       OdGeMatrix3d() const;

  static OdGeVector3d givePerp(const OdGeVector2d& vec1, const OdGeVector2d& vec2);

	double         x, y, z;
};

inline OdGeVector3d::OdGeVector3d() : x(0.0), y(0.0), z(0.0)
{
}

inline OdGeVector3d::OdGeVector3d(const OdGeVector3d& src) : 
	x(src.x), y(src.y), z(src.z)
{
}

inline OdGeVector3d::OdGeVector3d(double xx, double yy, double zz) : 
	x(xx), y(yy), z(zz)
{
}

inline OdGeVector3d& OdGeVector3d::operator=(const OdGeVector3d& vec)
{
  x = vec.x;
  y = vec.y;
  z = vec.z;
  return *this;
}


inline OdGeVector3d OdGeVector3d::operator+(const OdGeVector3d& v) const
{
	return OdGeVector3d (x + v.x, y + v.y, z + v.z);
}

inline OdGeVector3d& OdGeVector3d::operator+=(const OdGeVector3d& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

inline OdGeVector3d OdGeVector3d::operator-(const OdGeVector3d& v) const
{
	return OdGeVector3d (x - v.x, y - v.y, z - v.z);
}

inline OdGeVector3d& OdGeVector3d::operator-=(const OdGeVector3d& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

inline OdGeVector3d& OdGeVector3d::setToSum(const OdGeVector3d& v1, const OdGeVector3d& v2)
{
	x = v1.x + v2.x;
	y = v1.y + v2.y;
	z = v1.z + v2.z;
	return *this;
}

inline OdGeVector3d OdGeVector3d::operator- () const
{
	return OdGeVector3d  (-x, -y, -z);
}

inline OdGeVector3d& OdGeVector3d::negate()
{
	x = -x;
	y = -y;
	z = -z;
	return *this;
}

inline OdGeVector3d OdGeVector3d::operator*(double s) const
{
	return OdGeVector3d (x * s, y * s, z * s);
}

inline OdGeVector3d& OdGeVector3d::operator *= (double s)
{
	x *= s;
	y *= s;
	z *= s;
	return *this;
}

inline OdGeVector3d& OdGeVector3d::setToProduct(const OdGeVector3d& v, double s)
{
	x = s * v.x;
	y = s * v.y;
	z = s * v.z;
	return *this;
}

inline OdGeVector3d OdGeVector3d::operator / (double val) const
{
	return OdGeVector3d (x/val, y/val, z/val);
}

inline OdGeVector3d& OdGeVector3d::operator /= (double val)
{
	x /= val;
	y /= val;
	z /= val;
	return *this;
}

inline double OdGeVector3d::lengthSqrd() const
{
	return x*x + y*y + z*z;
}

inline double OdGeVector3d::dotProduct(const OdGeVector3d& v) const
{
	return x * v.x + y * v.y + z * v.z;
}

inline OdGeVector3d& OdGeVector3d::set(double xx, double yy, double zz)
{
	x = xx;
	y = yy;
	z = zz;
	return *this;
}

inline double
OdGeVector3d::operator [] (unsigned int i) const
{
	return * (&x + i);
}

inline double& OdGeVector3d::operator [] (unsigned int i)
{
	return * (&x + i);
}

// returns projection to XY plane
inline OdGeVector2d OdGeVector3d::convert2d() const
{
  return OdGeVector2d(x, y);
}

GE_TOOLKIT_EXPORT OdGeVector3d operator*(const OdGeMatrix3d& mat, const OdGeVector3d& vec);
GE_TOOLKIT_EXPORT OdGeVector3d operator*(double scl, const OdGeVector3d& vec);

#include "DD_PackPop.h"

#endif


