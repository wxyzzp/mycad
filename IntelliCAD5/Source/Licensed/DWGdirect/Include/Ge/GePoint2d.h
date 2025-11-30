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
// DWGdirect © 2002 by Open Design Alliance Inc. All rights reserved. 
//
// By use of this software, you acknowledge and accept the terms of this 
// agreement.
///////////////////////////////////////////////////////////////////////////////



#ifndef OD_GEPNT2D_H
#define OD_GEPNT2D_H

#include "GeVector2d.h"
#include "OdArray.h"
#include "math.h"

class OdGeMatrix2d;
class OdGeVector2d;
class OdGeLinearEnt2d;
class OdGeLine2d;

#include "DD_PackPush.h"

/** Description:
 OdGePoint2D represents two dimensional point. 

 {group:OdGe_Classes} 
*/
class GE_TOOLKIT_EXPORT OdGePoint2d
{
public:
	/** Arguments:
		x (I) The *x*-coordinate of the point.
		y (I) The *y*-coordinate of the point.
	*/
	OdGePoint2d();

	OdGePoint2d(const OdGePoint2d& pnt);

	OdGePoint2d(double x, 
		double y);
	
	static const OdGePoint2d kOrigin;

	/** Description:
		Sets this point to the result of the matrix multiplication (mat * pnt),
		or the point pnt scaled by a factor of scl. 

		Arguments:
		mat (I) A 2D matrix.
		scl (I) Scale factor.
		pnt (I) The point to be multiplied.
	*/
	friend GE_TOOLKIT_EXPORT OdGePoint2d operator * (const OdGeMatrix2d& mat, 
		const OdGePoint2d& pnt);

	friend GE_TOOLKIT_EXPORT OdGePoint2d operator * (double scl, 
		const OdGePoint2d& pnt);


	/** Description:
		Sets this point to the result of the matrix multiplication (mat * pnt).
		Returns a reference to this point.

		Arguments:
		mat (I) A 2D matrix.
		pnt (I) The point to be multiplied.
	*/
	OdGePoint2d& setToProduct (const OdGeMatrix2d& mat, 
		const OdGePoint2d& pnt);


	/** Description:
		Sets this point to the result of the matrix multiplication (mat * this point).
		Returns a reference to this point.

		Arguments:
		mat (I) A 2D matrix.
	*/
	OdGePoint2d& transformBy (const OdGeMatrix2d& mat);

	OdGePoint2d& rotateBy (double angle, 
		const OdGePoint2d& wrtPoint	= OdGePoint2d::kOrigin);

	OdGePoint2d& mirror (const OdGeLine2d& line);

	OdGePoint2d& scaleBy (double scaleFactor, 
		const OdGePoint2d& wrtPoint = OdGePoint2d::kOrigin);

	OdGePoint2d operator * (double) const;

	OdGePoint2d& operator *= (double scl);
	OdGePoint2d operator / (double scl) const;
	OdGePoint2d& operator /= (double scl);

	OdGePoint2d operator + (const OdGeVector2d& vec) const;
	OdGePoint2d& operator += (const OdGeVector2d& vec);
	OdGePoint2d operator - (const OdGeVector2d& vec) const;
	OdGePoint2d& operator -= (const OdGeVector2d& vec);


	/** Description:
		Sets this point to the sum of the specfied point and a vector.
		Returns a reference to this point.

		Arguments:
		pnt (I) Any 2D point.
		vec (I) A 2D vector.
	*/
	OdGePoint2d& setToSum (const OdGePoint2d& pnt, 
		const OdGeVector2d& vec);
	
	OdGeVector2d operator - (const OdGePoint2d& pnt) const;

	/** Description:
		Returns this point as a vector.
	*/
	OdGeVector2d asVector () const;
	
	/** Description:
		Returns the distance from this point to the specified point.

		Arguments:
		pnt (I) Any 2D point.
	*/
	double distanceTo (const OdGePoint2d& pnt) const;
	
	bool operator == (const OdGePoint2d& pnt) const;
	bool operator != (const OdGePoint2d& pnt) const;

	/** Description:
		Returns true if and only if the distance from this point to pnt is within the 
		specified tolerance.

		Arguments:
		pnt (I) Any 2D point.
		tol (I) Tolerance for determining equality.
	*/
	bool isEqualTo (const OdGePoint2d& pnt,	const OdGeTol& tol = OdGeContext::gTol) const;
	
	double operator [] (unsigned int i) const;
	double& operator [] (unsigned int idx);

	/** Description:
		Sets this point to the x and y coordinates of this point. 
		Returns a reference to this point.

		Arguments:
		x (I) The x-coordinate of the point.
		y (I) The y-coordinate of the point.
	*/
	OdGePoint2d& set (double x, 
		double y);
	
	double x, y;
};


inline OdGePoint2d::OdGePoint2d() : x(0.0), y(0.0)
{
}

inline OdGePoint2d::OdGePoint2d(const OdGePoint2d& src) : x(src.x), y(src.y)
{
}

inline OdGePoint2d::OdGePoint2d(double xx, double yy) : x(xx), y(yy)
{
}

inline bool OdGePoint2d::operator==(const OdGePoint2d& pnt) const
{
	return isEqualTo(pnt);
}

inline bool OdGePoint2d::operator!=(const OdGePoint2d& pnt) const
{
	return !isEqualTo(pnt);
}

inline OdGePoint2d OdGePoint2d::operator * (double val) const
{
	return OdGePoint2d(x*val, y*val);
}

inline OdGePoint2d operator * (double val, const OdGePoint2d& p)
{
	return OdGePoint2d(p.x*val, p.y*val);
}

inline OdGePoint2d& OdGePoint2d::operator *= (double val)
{
	x *= val;
	y *= val;
	return *this;
}

inline OdGePoint2d OdGePoint2d::operator / (double val) const
{
	return OdGePoint2d (x/val, y/val);
}

inline OdGePoint2d& OdGePoint2d::operator /= (double val)
{
	x /= val;
	y /= val;
	return *this;
}

inline OdGePoint2d OdGePoint2d::operator + (const OdGeVector2d& v) const
{
	return OdGePoint2d(x + v.x, y + v.y);
}

inline OdGePoint2d& OdGePoint2d::operator += (const OdGeVector2d& v)
{
	x += v.x;
	y += v.y;
	return *this;
}

inline OdGePoint2d OdGePoint2d::operator - (const OdGeVector2d& v) const
{
	return OdGePoint2d(x - v.x, y - v.y);
}

inline OdGePoint2d& OdGePoint2d::operator -= (const OdGeVector2d& v)
{
	x -= v.x;
	y -= v.y;
	return *this;
}

inline OdGeVector2d OdGePoint2d::operator - (const OdGePoint2d& p) const
{
	return OdGeVector2d(x - p.x, y - p.y);
}

inline OdGeVector2d OdGePoint2d::asVector() const
{
	return OdGeVector2d(x, y);
}

inline OdGePoint2d& OdGePoint2d::set(double xx, double yy)
{
	x = xx;
	y = yy;
	return *this;
}

inline double OdGePoint2d::operator [] (unsigned int i) const
{
	return * (&x+i);
}

inline double& OdGePoint2d::operator [] (unsigned int i)
{
	return * (&x+i);
}

inline OdGePoint2d& OdGePoint2d::setToSum(const OdGePoint2d& pnt, const OdGeVector2d& vec)
{
 x = pnt.x + vec.x;
	y = pnt.y + vec.y;
	return *this;
}

inline double OdGePoint2d::distanceTo (const OdGePoint2d& pnt) const
{
 return sqrt((x-pnt.x)*(x-pnt.x)+(y-pnt.y)*(y-pnt.y));
}

#include "DD_PackPop.h"

#endif


