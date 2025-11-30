// File: GeVector3d.cpp
//


#include "GeVector3d.h"
#include "GeMatrix3d.h"
#include "GeVector2d.h"
#include "GePlanarSurface.h"
#include "GePlane.h"
#include <math.h>


//const double _PI_ = 3.1415926535897932384626433832795;
//const double _PI2_ = 1.5707963267948966192313216916398;
const double _2PI_ = 6.283185307179586476925286766559;


//////////////////////////////////////////////////
// CGeVector3d class implementation
//////////////////////////////////////////////////


const CGeVector3d	CGeVector3d::s_identity;
const CGeVector3d	CGeVector3d::s_xAxis(1.0, 0.0, 0.0);
const CGeVector3d	CGeVector3d::s_yAxis(0.0, 1.0, 0.0);
const CGeVector3d	CGeVector3d::s_zAxis(0.0, 0.0, 1.0);


/*
Constructor. Creates a vector at the origin.
*/
inline
CGeVector3d::CGeVector3d() : m_x(0.0), m_y(0.0), m_z(0.0)
{
}


/*
Copy constructor.
*/
inline
CGeVector3d::CGeVector3d(const CGeVector3d& source) : m_x(source.m_x), m_y(source.m_y), m_z(source.m_z)
{
}


/*
Constructor. Constructs a 3D vector with coordinates x, y, and z correspondingly.
*/
inline
CGeVector3d::CGeVector3d(double x, double y, double z) : m_x(x), m_y(y), m_z(z)
{
}


/*
Constructor. Constructs a 3D vector by the uVector * vector2d.m_x + vVector * vector2d.m_y,
where 3D point 'origin' and 3D vectors 'uVector' and 'vVector' form a coordinate system in
planar entity planarSurf and can found by function call planarSurf.get(origin, uVector, vVector).
*/
CGeVector3d::CGeVector3d(const CGePlanarSurface& planarSurf, const CGeVector2d& vector2d)
{
	CGePoint3d	origin;
	CGeVector3d	uVector, vVector;
	planarSurf.get(origin, uVector, vVector);
	*this = uVector * vector2d.m_x + vVector * vector2d.m_y;
}


/*
Returns the 3D vector which is the result of mirroring of this vector with respect
to the plane with the normal 'normalToPlane' passing through the origin.
*/
inline CGeVector3d&
CGeVector3d::mirror(const CGeVector3d& normalToPlane)
{
	return transformBy(CGeMatrix3d::mirroring(CGePlane(CGePoint3d::s_origin, normalToPlane)));
}


/*

*/
inline CGeVector3d&
CGeVector3d::rotateBy(double angle, const CGeVector3d& axis)
{
	return transformBy(CGeMatrix3d::rotation(angle, axis));
}


/*
Sets this vector to the product of 'matrix' and 'vector'. The order of multiplication
is matrix * vector, where vectors are considered as columns.
Returns the reference to this vector.
*/
CGeVector3d&
CGeVector3d::setToProduct(const CGeMatrix3d& matrix, const CGeVector3d& vector)
{
	double	x = matrix.m_a00 * vector.m_x + matrix.m_a01 * vector.m_y + matrix.m_a02 * vector.m_z,
			y = matrix.m_a10 * vector.m_x + matrix.m_a11 * vector.m_y + matrix.m_a12 * vector.m_z;
	m_z = matrix.m_a20 * vector.m_x + matrix.m_a21 * vector.m_y + matrix.m_a22 * vector.m_z;
	m_x = x;
	m_y = y;
	return *this;
}


/*
Transform this vector by pre-multiplying it by the 'matrix'.
Returns the reference to this vector.
Result is the same as setToProduct(matrix, *this).
*/
inline CGeVector3d&
CGeVector3d::transformBy(const CGeMatrix3d& matrix)
{
	return setToProduct(matrix, *this);
}


/*
Returns the vector equal to this vector multiplied by scale factor scale'.
*/
inline CGeVector3d
CGeVector3d::operator * (double scale) const
{
	return CGeVector3d(m_x * scale, m_y * scale, m_z * scale);
}


/*
Multiplies this vector by scale factor 'scale'. Returns the reference to this vector.
*/
inline CGeVector3d&
CGeVector3d::operator *= (double scale)
{
	m_x *= scale;
	m_y *= scale;
	m_z *= scale;
	return *this;
}


/*

*/
inline CGeVector3d&
CGeVector3d::setToProduct(const CGeVector3d& vector, double scale)
{
	m_x = vector.m_x * scale;
	m_y = vector.m_y * scale;
	m_z = vector.m_z * scale;
	return *this;
}


/*
Returns the vector equal to this vector multiplied by scale factor 1 / 'scale'.
*/
inline CGeVector3d
CGeVector3d::operator / (double scale) const
{
	return CGeVector3d(m_x / scale, m_y / scale, m_z / scale);
}


/*
Multiplies this vector by scale factor 1 / 'scale'. Returns the reference to this vector.
*/
inline CGeVector3d&
CGeVector3d::operator /= (double scale)
{
	m_x /= scale;
	m_y /= scale;
	m_z /= scale;
	return *this;
}


/*
Returns the vector equal to the sum of this vector and 'vector'.
*/
inline CGeVector3d
CGeVector3d::operator + (const CGeVector3d& vector) const
{
	return CGeVector3d(m_x + vector.m_x, m_y + vector.m_y, m_z + vector.m_z);
}


/*
Adds 'vector' to this vector. Returns the reference to this vector.
*/
inline CGeVector3d&
CGeVector3d::operator += (const CGeVector3d& vector)
{
	m_x += vector.m_x;
	m_y += vector.m_y;
	m_z += vector.m_z;
	return *this;
}


/*
Returns the result of subtraction of 'vector' from this vector.
*/
inline CGeVector3d
CGeVector3d::operator - (const CGeVector3d& vector) const
{
	return CGeVector3d(m_x - vector.m_x, m_y - vector.m_y, m_z - vector.m_z);
}


/*
Subtracts 'vector' to this vector. Returns the reference to this vector.
*/
inline CGeVector3d&
CGeVector3d::operator -= (const CGeVector3d& vector)
{
	m_x -= vector.m_x;
	m_y -= vector.m_y;
	m_z -= vector.m_z;
	return *this;
}


/*
Sets this vector to the sum of 'vector1' and 'vector2'. 
Returns the reference to this vector.
*/
inline CGeVector3d&
CGeVector3d::setToSum(const CGeVector3d& vector1, const CGeVector3d& vector2)
{
	m_x = vector1.m_x + vector2.m_x;
	m_y = vector1.m_y + vector2.m_y;
	m_z = vector1.m_z + vector2.m_z;
	return *this;
}


/*
Returns the vector opposite to this vector, namely the vector (-m_x, -m_y, -m_z).
*/
inline CGeVector3d
CGeVector3d::operator - () const
{
	return CGeVector3d(-m_x, -m_y, -m_z);
}


/*
Negates this vector, namely sets it to be equal to the vector (-m_x, -m_y, -m_z).
Returns the reference to this vector.
*/
inline CGeVector3d&
CGeVector3d::negate()
{
	m_x = -m_x;
	m_y = -m_y;
	m_z = -m_z;
	return *this;
}


/*
Returns a vector orthogonal to this vector. The choice of the orthogonal vector
is determined by the function CGeContext::s_pOrthoVector.
*/
inline CGeVector3d
CGeVector3d::perpVector() const
{
	CGeVector3d	orthoVector;
	CGeContext::s_pOrthoVector(*this, orthoVector);
	return orthoVector;
}


/*
Returns the angle between this vector and 'vector' in the range [0, Pi].
*/
double
CGeVector3d::angleTo(const CGeVector3d& vector) const
{
	double	lengthsProduct = length();
	lengthsProduct *= vector.length();
	if(lengthsProduct < CGeContext::s_defTol.equalVector())
		return 0.0;
	else
		return acos(dotProduct(vector) / lengthsProduct);
}


/*
Returns the angle between this vector and 'vector' in the range [0, 2 x Pi].
If (refVector.dotProduct(crossProduct(vector)) >= 0.0), then the return value coincides
with the return value of the function angleTo(vector). Otherwise the return value is
2 * Pi minus the return value of the function angleTo(vector).
*/
double
CGeVector3d::angleTo(const CGeVector3d& vector, const CGeVector3d& refVector) const
{
	double	angle = angleTo(vector);
	if(refVector.dotProduct(crossProduct(vector)) < 0.0)
		angle = _2PI_ - angle;
	return angle;
}


/*
Returns the angle between the orthogonal projection of this vector into the plane
through the origin with the same normal as planar entity 'planarSurf' and the zeroth
basis vector axis1 of the planar entity 'planarSurf' provided by the function
planarSurf.getCoordSystem (origin, axis1, axis2).
*/
double
CGeVector3d::angleOnPlane(const CGePlanarSurface& planarSurf) const
{
	CGePoint3d	origin;
	CGeVector3d	axis1, axis2, projVector = project(planarSurf.normal(), planarSurf.normal());
	planarSurf.getCoordSystem(origin, axis1, axis2);
	return angleTo(projVector, axis1);
}


/*
Returns 2D vector with the coordinates corresponding to the decomposition of this
vector in the basis 'uVector', 'vVector' in the planar entity 'planarSurf', where
'uVector' and 'vVector' can be found by function call planarSurf.get(origin, uVector, vVector).
Contract:  This 3D vector is assumed to be parallel to the planar entity 'planarSurf'.
*/
CGeVector2d
CGeVector3d::convert2d(const CGePlanarSurface& planarSurf) const
{
// Let Q = *this - origin, then in {U, V, (U x V)} == {uVector, vVector, (uVector x vVector)} basis it's equal to
//  Q = u * U + v * V + w * (U x V),					(1)
// i.e. Q = (u,v,w). Note, in our case w==0, but it doesn't matter.
// (1) is a system of 3 linear equation for 3 unknowns. Solution is
//  u = ((U.Q) * |V|^2 - (V.Q) * (U.V)) / D,		\
//  v = ((V.Q) * |U|^2 - (U.Q) * (U.V)) / D,		  >	(2)
//  w = ([U x V].Q) / D,							/
// where D = |U|^2 * |V|^2 - (U.V)^2,					(3)
// ( . ) means dot product, ( x ) means cross product, | | means norm, ^2 means "squared".
// Note, D is not zero, because (3) => D = |U|^2 * |V|^2 * sin(U,V)^2, and it's zero only if
// U is zero, or V is zero, or they are collinear.

	CGePoint3d	origin;
	CGeVector3d	uVector, vVector, q;
	planarSurf.get(origin, uVector, vVector);
	q = *this - origin.asVector();
	double	uq = uVector.dotProduct(q), vq = uVector.dotProduct(q), uv = uVector.dotProduct(vVector),
			uu = uVector.lengthSqrd(), vv = vVector.lengthSqrd(), _d = 1.0 / (uu * vv - uv * uv);
	return CGeVector2d((uq * vv - vq * uv) * _d, (vq * uu - uq * uv) * _d);
}


/*
Returns the unit vector codirectional to this vector.
The vector is not normalized if its length is less than tol.equalVector(),
in which case the returned vector is the same as this vector.
*/
CGeVector3d
CGeVector3d::normal(const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	double	l = length();
	if(l < tol.equalVector())
		return CGeVector3d(*this);
	else
		return CGeVector3d(*this / l);
}


/*
Sets this vector to the unit vector codirectional to this vector. Returns the
reference to this vector. This vector is not normalized if its length is less
than tol.equalVector(), in which case it remains unchanged.
*/
CGeVector3d&
CGeVector3d::normalize(const CGeTolerance& tol /* = CGeContext::s_defTol*/)
{
	double	l = length();
	if(l >= tol.equalVector())
		*this /= l;
	return *this;
}


/*
Sets this vector to the unit vector codirectional to this vector. Returns the
reference to this vector. This vector is not normalized if its length is less
than tol.equalVector(), in which case it remains unchanged. The 'flag' argument
will be set to the following value to indicate if a failure may have occurred: 'e0This'.
*/
CGeVector3d&
CGeVector3d::normalize(const CGeTolerance& tol, CGeError& flag)
{
	double	l = length();
	if(l < tol.equalVector())
		flag = CGe::e0This;
	else
		*this /= l;
	return *this;
}


/*
Returns the Euclidean length of this vector.
*/
inline double
CGeVector3d::length() const
{
	return sqrt(lengthSqrd());
}


/*
Returns the square of the Euclidean length of this vector.
*/
inline double
CGeVector3d::lengthSqrd() const
{
	return m_x * m_x + m_y * m_y + m_z * m_z;
}


/*
Returns 'true' if this vector is equal to its normalization. Returns 'false' otherwise.
*/
inline bool
CGeVector3d::isUnitLength(const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return isEqualTo(normal(tol), tol);
}


/*
Returns 'true' if this vector is a zero length vector, 'false' otherwise.
*/
inline bool
CGeVector3d::isZeroLength(const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return isEqualTo(CGeVector3d::s_identity, tol);
}


/*
Checks if this vector is parallel to 'vector'. Namely, it checks if after normalization
(and negation, in the case of negative dot product with 'vector'), this vector is within
the distance tol.equalVector() from the normalization of 'vector'.
Returns 'true' if this condition is met and 'false' otherwise.
*/
inline bool
CGeVector3d::isParallelTo(const CGeVector3d& vector, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	CGeError	flag;
	return isParallelTo(vector, tol, flag);
}


/*
Checks if this vector is parallel to 'vector'. Namely, it checks if after normalization
(and negation in case of negative dot product with 'vector') this vector is within the
distance tol.equalVector() from the normalization of 'vector'.
Returns 'true' if this condition is met and 'false' otherwise. If 'false' is returned,
the 'flag' argument will be set to one of the following values to indicate why a failure
may have occurred: 'e0This' or 'e0Arg1'.
*/
bool
CGeVector3d::isParallelTo(const CGeVector3d& vector, const CGeTolerance& tol, CGeError& flag) const
{
	double	l1 = length(), l2 = vector.length();
	if(l1 < tol.equalVector())
	{
		flag = CGe::e0This;
		return false;
	}
	if(l2 < tol.equalVector())
	{
		flag = CGe::e0Arg1;
		return false;
	}

	if(dotProduct(vector) > 0.0)
		return (*this / l1 - vector / l2).length() < tol.equalVector();
	else
		return (*this / l1 + vector / l2).length() < tol.equalVector();
}


/*
Checks if this vector is codirectional to 'vector'. Namely, it checks if after
normalization this vector is within the distance tol.equalVector() from the
normalization of 'vector'.
Returns 'true' if this condition is met and 'false' otherwise.
*/
inline bool
CGeVector3d::isCodirectionalTo(const CGeVector3d& vector, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return (normal() - vector.normal()).length() < tol.equalVector();
}


/*
This function checks if this vector is codirectional to 'vector'. Namely, it checks
if after normalization this vector is within the distance tol.equalVector() from the
normalization of 'vector'.
Returns 'true' if this condition is met and 'false' otherwise. If 'false' is returned,
the 'flag' argument will be set to one of the following values to indicate why a may
have failure occurred: 'e0This' or 'e0Arg'.
*/
bool
CGeVector3d::isCodirectionalTo(const CGeVector3d& vector, const CGeTolerance& tol, CGeError& flag) const
{
	double	l1 = length(), l2 = vector.length();
	if(l1 < tol.equalVector())
	{
		flag = CGe::e0This;
		return false;
	}
	if(l2 < tol.equalVector())
	{
		flag = CGe::e0Arg1;
		return false;
	}

	return (*this / l1 - vector / l2).length() < tol.equalVector();
}


/*
Checks if this vector is perpendicular to 'vector'. Namely, it checks if after
normalization the dot product of this vector with the normalization of 'vector'
is less than tol.equalVector() in absolute value.
Returns 'true' if this condition is met and 'false' otherwise.
*/
inline bool
CGeVector3d::isPerpendicularTo(const CGeVector3d& vector, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return fabs(normal().dotProduct(vector.normal())) < tol.equalVector();
}


/*
Checks if this vector is perpendicular to 'vector'. Namely, it checks if after
normalization the dot product of this vector with the normalization of 'vector'
is less than tol.equalVector() in absolute value.
Returns 'true' if this condition is met and 'false' otherwise.  If 'false' is returned,
the 'flag' argument will be set to one of the following values to indicate why
a failure may have occurred: 'e0This' or 'e0Arg'.
*/
bool
CGeVector3d::isPerpendicularTo(const CGeVector3d& vector, const CGeTolerance& tol, CGeError& flag) const
{
	double	l1 = length(), l2 = vector.length();
	if(l1 < tol.equalVector())
	{
		flag = CGe::e0This;
		return false;
	}
	if(l2 < tol.equalVector())
	{
		flag = CGe::e0Arg1;
		return false;
	}

	return fabs((*this / l1).dotProduct(vector / l2)) < tol.equalVector();
}


/*
Returns the dot product of this vector and 'vector'.
*/
inline double
CGeVector3d::dotProduct(const CGeVector3d& vector) const
{
	return m_x * vector.m_x + m_y * vector.m_y + m_z * vector.m_z;
}


/*
Returns the cross product of this vector and 'vector'.
*/
inline CGeVector3d
CGeVector3d::crossProduct(const CGeVector3d& vector) const
{
	return	CGeVector3d(m_y * vector.m_z - m_z * vector.m_y,
						m_z * vector.m_x - m_x * vector.m_z,
						m_x * vector.m_y - m_y * vector.m_x);
}


/*
This function works the same way as isEqualTo(vector).
*/
inline bool
CGeVector3d::operator == (const CGeVector3d& vector) const
{
	return isEqualTo(vector);
}


/*
This function works the same way as !isEqualTo(vector).
*/
inline bool
CGeVector3d::operator != (const CGeVector3d& vector) const
{
	return !isEqualTo(vector);
}


/*
Checks if this vector is within the distance tol.equalVector() from 'vector'.
The default value of the tolerance class tol is CGeContext::s_defTol
Returns 'true' if this condition is met and 'false' otherwise.
*/
inline bool
CGeVector3d::isEqualTo(const CGeVector3d& vector, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return (*this - vector).length() < tol.equalVector();
}


/*
Returns the value of the i-th coordinate of the 3D vector.
For i equal to 0, returns X coordinate.
For i equal to 1, returns Y coordinate.
For i equal to 2, returns Z coordinate.
*/
inline double
CGeVector3d::operator [] (unsigned int i) const
{
	return m_coords[i];
}


/*
Returns the reference to the i-th coordinate of the 3D vector.
For i equal to 0, returns the reference to the X coordinate.
For i equal to 1 returns the reference to the Y coordinate.
For i equal to 2 returns the reference to the Z coordinate.
*/
inline double&
CGeVector3d::operator [] (unsigned int i)
{
	return m_coords[i];
}


/*
Returns the index of the largest absolute value coordinate of this vector.
*/
inline unsigned int
CGeVector3d::largestElement() const
{
	if(fabs(m_x) > fabs(m_y))
	{
		if(fabs(m_x) > fabs(m_z))
			return 0;
		else
			return 2;
	}
	else
	{
		if(fabs(m_y) > fabs(m_z))
			return 1;
		else
			return 2;
	}
}


/*
Returns a 3D vector which is the result of the orthogonal projection of this point
into the plane with normal 'planeNormal' passing through the origin.
*/
CGeVector3d
CGeVector3d::orthoProject(const CGeVector3d& planeNormal) const
{
	CGeError	flag;
	return orthoProject(planeNormal, CGeContext::s_defTol, flag);
}


/*
Returns a 3D vector which is the result of the orthogonal projection of this point
into the plane with normal 'planeNormal' passing through the origin.
The 'flag' argument will be set to one of the following values to indicate why
a failure may have occurred: 'e0Arg1', 'e0Arg2', or 'ePerpendicularArg1Arg2'.
*/
CGeVector3d
CGeVector3d::orthoProject(const CGeVector3d& planeNormal, const CGeTolerance& tol, CGeError& flag) const
{
#pragma message("--- CGeVector3d::orthoProject doesn't set the flag. ---")
	CGePlane	plane(CGePoint3d::s_origin, planeNormal);
	CGePoint3d	origin;
	CGeVector3d	axis1, axis2;
	plane.getCoordSystem(origin, axis1, axis2);
	return CGeVector3d(axis1 * dotProduct(axis1) + axis2 * dotProduct(axis2));
}


/*
Returns the 3D vector which is the result of the parallel projection of this vector
into the plane with normal 'planeNormal' passing through the origin. The direction
of projection is defined by vector 'projDir'.
*/
inline CGeVector3d
CGeVector3d::project(const CGeVector3d& planeNormal, const CGeVector3d& projDir) const
{
	CGeError	flag;
	return project(planeNormal, projDir, CGeContext::s_defTol, flag);
}


/*
Returns the 3D vector which is the result of the parallel projection of this vector
into the plane with normal 'planeNormal' passing through the origin. The direction
of projection is defined by vector 'projDir'.
The 'flag' argument will be set to the following value to indicate why a failure
may have occurred: 'e0Arg1'.
*/
CGeVector3d
CGeVector3d::project(const CGeVector3d& planeNormal, const CGeVector3d& projDir, const CGeTolerance& tol, CGeError& flag) const
{
// Let P == *this = u * U + v * V + s * S,	(1)
// where U and V - basis vectors in the plane, S is 'projDir',
// u,v - coords of the required projection on the plane, s - some coefficient.
// (1) is a system of 3 linear equations for 3 unknowns. Solution is
// u = -(V.(P x S)) / D,				\
// v =  (U.(P x S)) / D,				  >	(2)
// s =  (P.(U x V)) / D,				/
// where D = (S.(U x V)),					(3)
// ( . ) means dot product, ( x ) means cross product.
// Note, (3) => D is zero if S is complanar with the plane  or  S is zero  or  plane is corrupted.
#pragma message("--- What should return CGeVector3d::project if 'projDir' is complanar with the plane? ---")
// So, required projection point is:
// origin + u * U + v * V  or  P - s * S, where origin, U, V are get by plane.get(origin, U, V).

	CGePlane	plane(CGePoint3d::s_origin, planeNormal);
	CGePoint3d	origin;
	CGeVector3d	uVector, vVector, normal;
	plane.get(origin, uVector, vVector);
	normal = uVector.crossProduct(vVector);
	double	d = projDir.dotProduct(normal);
	if(fabs(d) < tol.equalVector())				// TO DO: what is the tolerance for d?
	{
		flag = CGe::e0Arg1;
		return CGeVector3d(*this);				//        see pragma message above.
	}
	else
		return CGeVector3d(*this - projDir * (dotProduct(normal) / d));
}


/*
Returns the matrix of translation by this vector.
*/
inline
CGeVector3d::operator CGeMatrix3d() const
{
	return CGeMatrix3d::translation(*this);
}


/*
Returns a matrix such that matrix * this vector yields the argument vector 'vector'.
There is no unique matrix to represent a rotation from this vector to 'vector' when
they are parallel and in opposite directions. In this case, the argument 'axis' is used
(if it is perpendicular to this vector) to uniquely determine the resultant matrix.
'axis' defaults to the zero-length vector, in which case an arbitrary (but correct)
matrix will be returned that rotates this vector to 'vector'.
Contract:  This vector and 'vector' must be non-zero length.
*/
inline CGeMatrix3d
CGeVector3d::rotateTo(const CGeVector3d& vector, const CGeVector3d& axis /* = CGeVector3d::s_identity*/) const
{
	if(isParallelTo(vector))
		return CGeMatrix3d::rotation(angleTo(vector), axis);
	else
		return CGeMatrix3d::rotation(angleTo(vector), crossProduct(vector));
}


/*
Sets the value of the zeroth coordinate of this vector to x, the value of the
first coordinate to y, and the value of the second coordinate to z.
Returns reference to this vector.
*/
inline CGeVector3d&
CGeVector3d::set(double x, double y, double z)
{
	m_x = x;
	m_y = y;
	m_z = z;
	return *this;
}


/*
Sets this 3D vector to be equal to the vector given by the formula
uVector * vector2d.m_x + vVector * vector2d.m_y, where 3D vectors uVector and vVector
form a coordinate system in planar entity 'planarSurf' and can found by function call
planarSurf.get(origin, uVector, vVector).
*/
CGeVector3d&
CGeVector3d::set(const CGePlanarSurface& planarSurf, const CGeVector2d& vector2d)
{
	CGePoint3d	origin;
	CGeVector3d	uVector, vVector;
	planarSurf.get(origin, uVector, vVector);
	*this = uVector * vector2d.m_x + vVector * vector2d.m_y;
	return *this;
}


//////////////////////////////////////////////////
// friend to CGeVector3d functions implementation
//////////////////////////////////////////////////


/*
Returns the vector equal to the product of 'matrix' and 'vector'. The order of
multiplication is matrix * vector, where vectors are considered as columns.
*/
inline CGeVector3d
operator * (const CGeMatrix3d& matrix, const CGeVector3d& vector)
{
	return CGeVector3d(matrix.m_a00 * vector.m_x + matrix.m_a01 * vector.m_y + matrix.m_a02 * vector.m_z,
					   matrix.m_a10 * vector.m_x + matrix.m_a11 * vector.m_y + matrix.m_a12 * vector.m_z,
					   matrix.m_a20 * vector.m_x + matrix.m_a21 * vector.m_y + matrix.m_a22 * vector.m_z);
}


/*
Returns the vector equal to 'vector' multiplied by scale factor 'scale'.
*/
inline CGeVector3d
operator * (double scale, const CGeVector3d& vector)
{
	return CGeVector3d(vector.m_x * scale, vector.m_y * scale, vector.m_z * scale);
}
