// File: GePoint3d.cpp
//


#include "GePoint3d.h"
#include "GeVector3d.h"
#include "GePoint2d.h"
#include "GePlanarSurface.h"
#include "GeMatrix3d.h"
#include "GePlane.h"
#include <math.h>


//////////////////////////////////////////////////
// CGePoint3d class implementation
//////////////////////////////////////////////////


const CGePoint3d CGePoint3d::s_origin;


/*
Constructor. Creates a point at the origin.
*/
inline
CGePoint3d::CGePoint3d() : m_x(0.0), m_y(0.0), m_z(0.0)
{
}


/*
Copy constructor.
*/
inline
CGePoint3d::CGePoint3d(const CGePoint3d& source) : m_x(source.m_x), m_y(source.m_y), m_z(source.m_z)
{
}


/*
Constructor. Constructs 3D point with coordinates x, y, and z correspondingly.
*/
inline
CGePoint3d::CGePoint3d(double x, double y, double z) : m_x(x), m_y(y), m_z(z)
{
}


/*
Constructor. Constructs 3D point by the formula
 origin + uVector * point2d.m_x + vVector * point2d.m_y,
where 3D point 'origin' and 3D vectors uVector and vVector form the coordinate system
in planar entity planarSurf and can found by function call planarSurf.get(origin, uVector, vVector).
*/
CGePoint3d::CGePoint3d(const CGePlanarSurface& planarSurf, const CGePoint2d& point2d)
{
	CGePoint3d	origin;
	CGeVector3d	uVector, vVector;
	planarSurf.get(origin, uVector, vVector);
	*this = origin + uVector * point2d.m_x + vVector * point2d.m_y;
}


/*
Returns the 3D point that is the result of mirroring of this point with respect to 'plane'.
*/
inline CGePoint3d&
CGePoint3d::mirror(const CGePlane& plane)
{
	return transformBy(CGeMatrix3d::mirroring(plane));
}


/*
Returns 3D point that is the result of rotation of this point around the line with
direction 'vector' passing through the point pivot. Rotation angle is given by the
argument 'angle', where positive direction of rotation is defined by the right-hand rule.
*/
inline CGePoint3d&
CGePoint3d::rotateBy(double angle, const CGeVector3d& vector, const CGePoint3d& pivot /* = CGePoint3d::s_origin*/)
{
	return transformBy(CGeMatrix3d::rotation(angle, vector, pivot));
}


/*
Sets this point to the product of 'matrix' and 'point'. The order of multiplication
is matrix * point, where points are considered as columns. 
Returns the reference to this point.
*/
CGePoint3d&
CGePoint3d::setToProduct(const CGeMatrix3d& matrix, const CGePoint3d& point)
{
	double	x = matrix.m_a00 * point.m_x + matrix.m_a01 * point.m_y + matrix.m_a02 * point.m_z + matrix.m_a03,
			y = matrix.m_a10 * point.m_x + matrix.m_a11 * point.m_y + matrix.m_a12 * point.m_z + matrix.m_a13;
	m_z = matrix.m_a20 * point.m_x + matrix.m_a21 * point.m_y + matrix.m_a22 * point.m_z + matrix.m_a23;
	m_x = x;
	m_y = y;
	return *this;
}


/*
Transform this pre-multiplying it 'matrix'.
Returns the reference to this point.
Result is the same as setToProduct(matrix, *this).
*/
inline CGePoint3d&
CGePoint3d::transformBy(const CGeMatrix3d& matrix)
{
	return setToProduct(matrix, *this);
}


/*
Returns 3D point that is the result of scaling of this point with respect to
the 3D point 'pivot' by scale factor 'scale'.
*/
inline CGePoint3d&
CGePoint3d::scaleBy(double scale, const CGePoint3d& pivot /* = CGePoint3d::s_origin*/)
{
	return transformBy(CGeMatrix3d::scaling(scale, pivot));
}


/*
Returns the point equal to this point scaled by scale factor 'scale'.
*/
inline CGePoint3d
CGePoint3d::operator * (double scale) const
{
	return CGePoint3d(m_x * scale, m_y * scale, m_z * scale);
}


/*
Scales this point by scale factor 'scale'. Returns the reference to this point.
*/
inline CGePoint3d&
CGePoint3d::operator *= (double scale)
{
	m_x *= scale;
	m_y *= scale;
	m_z *= scale;
	return *this;
}


/*
Returns the point equal to this points scaled by scale factor 1 / 'scale'.
*/
inline CGePoint3d
CGePoint3d::operator / (double scale) const
{
	return CGePoint3d(m_x / scale, m_y / scale, m_z / scale);
}


/*
Scales this point by scale factor 1 / 'scale'. Returns the reference to this point.
*/
inline CGePoint3d&
CGePoint3d::operator /= (double scale)
{
	m_x /= scale;
	m_y /= scale;
	m_z /= scale;
	return *this;
}


/*
Returns the point equal to this point translated by 'vector'.
*/
inline CGePoint3d
CGePoint3d::operator + (const CGeVector3d& vector) const
{
	return CGePoint3d(m_x + vector.m_x, m_y + vector.m_y, m_z + vector.m_z);
}


/*
Translates this point by 'vector'. Returns the reference to this point.
*/
inline CGePoint3d&
CGePoint3d::operator += (const CGeVector3d& vector)
{
	m_x += vector.m_x;
	m_y += vector.m_y;
	m_z += vector.m_z;
	return *this;
}


/*
Returns the point equal to this point translated by vector opposite to 'vector'.
*/
inline CGePoint3d
CGePoint3d::operator - (const CGeVector3d& vector) const
{
	return CGePoint3d(m_x - vector.m_x, m_y - vector.m_y, m_z - vector.m_z);
}


/*
Returns the 3D vector presenting the displacement from 3D 'point' to this 3D point.
*/
inline CGeVector3d
CGePoint3d::operator - (const CGePoint3d& point) const
{
	return CGeVector3d(m_x - point.m_x, m_y - point.m_y, m_z - point.m_z);
}


/*
Translates this point by vector opposite to 'vector'. Returns the reference to this point.
*/
inline CGePoint3d&
CGePoint3d::operator -= (const CGeVector3d& vector)
{
	m_x -= vector.m_x;
	m_y -= vector.m_y;
	m_z -= vector.m_z;
	return *this;
}


/*
Sets this point to the sum of 'point' and 'vector'. Returns the reference to this point.
*/
inline CGePoint3d&
CGePoint3d::setToSum(const CGePoint3d& point, const CGeVector3d& vector)
{
	m_x = point.m_x + vector.m_x;
	m_y = point.m_y + vector.m_y;
	m_z = point.m_z + vector.m_z;
	return *this;
}


/*
Converts 3D point into 3D vector with the same set of coordinates.
Return value is a class CGeVector3d.
*/
inline CGeVector3d
CGePoint3d::asVector() const
{
	return CGeVector3d(m_x, m_y, m_z);
}


/*
Returns 2D point with the coordinates corresponding to the decomposition of the vector
*this - origin in the basis uVector, vVector in the planar entity, where origin, uVector
and vVector can found by function call planarSurf.get(origin, uVector, vVector).
Contract: This 3D point assumed to be on the planar entity 'planarSurf'.
*/
CGePoint2d
CGePoint3d::convert2d(const CGePlanarSurface& planarSurf) const
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
	q = *this - origin;
	double	uq = uVector.dotProduct(q), vq = uVector.dotProduct(q), uv = uVector.dotProduct(vVector),
			uu = uVector.lengthSqrd(), vv = vVector.lengthSqrd(), _d = 1.0 / (uu * vv - uv * uv);
	return CGePoint2d((uq * vv - vq * uv) * _d, (vq * uu - uq * uv) * _d);
}


/*
Evaluates and returns distance from this point to 'point'.
*/
inline double
CGePoint3d::distanceTo(const CGePoint3d& point) const
{
	return sqrt((point.m_x - m_x) * (point.m_x - m_x) + (point.m_y - m_y) * (point.m_y - m_y) + (point.m_z - m_z) * (point.m_z - m_z));
}


/*
Returns 3D point that is the result of the orthogonal projection of this point into 'plane'. 
*/
CGePoint3d
CGePoint3d::orthoProject(const CGePlane& plane) const
{
	CGePoint3d	origin;
	CGeVector3d	axis1, axis2;
	plane.getCoordSystem(origin, axis1, axis2);
	return CGePoint3d(CGePoint3d(axis1.m_x, axis1.m_y, axis1.m_z) * axis1.dotProduct(asVector()) +
					  axis2 * axis2.dotProduct(asVector()));
}


/*
Returns 3D point that is the result of the parallel projection of this point into 'plane'.
The direction of projection is defined by 'projDir'.
*/
inline CGePoint3d
CGePoint3d::project(const CGePlane& plane, const CGeVector3d& projDir) const
{
// Let P = *this - origin = u * U + v * V + s * S,	(1)
// where origin, U and V - origin and basis vectors in the plane, S is 'projDir',
// u,v - coords of the required projection on the plane, s - some coefficient.
// (1) is a system of 3 linear equations for 3 unknowns. Solution is
// u = -(V.(P x S)) / D,						\
// v =  (U.(P x S)) / D,						  >	(2)
// s =  (P.(U x V)) / D,						/
// where D = (S.(U x V)),							(3)
// ( . ) means dot product, ( x ) means cross product.
// Note, (3) => D is zero if S is complanar with the 'plane'  or  S is zero  or  'plane' is corrupted.
#pragma message("--- What should return CGePoint3d::project if 'projDir' is complanar with the 'plane'? ---")
// So, required projection point is:
// origin + u * U + v * V  or  P - s * S, where origin, U, V are get by plane.get(origin, U, V).

	CGePoint3d	origin;
	CGeVector3d	uVector, vVector, normal;
	plane.get(origin, uVector, vVector);
	normal = uVector.crossProduct(vVector);
	double	d = projDir.dotProduct(normal);
	if(fabs(d) < CGeContext::s_defTol.equalVector())	// TO DO: what is the tolerance for d?
		return CGePoint3d(*this);						//        see pragma message above.
	else
		return CGePoint3d(*this - (origin + projDir * (normal.dotProduct(asVector()) / d)).asVector());
}


/*
This function works the same way as isEqualTo(point).
*/
inline bool
CGePoint3d::operator ==	(const CGePoint3d& point) const
{
	return isEqualTo(point);
}


/*
This function works the same way as !isEqualTo(point).
*/
inline bool
CGePoint3d::operator !=	(const CGePoint3d& point) const
{
	return !isEqualTo(point);
}


/*
Checks if this point is within the distance tol.equalPoint() from 'point'.
The default value of the tolerance class tol is CGeContext::s_defTol.
Returns 'true' if this condition is met and 'falsef otherwise.
*/
inline bool
CGePoint3d::isEqualTo(const CGePoint3d& point, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return distanceTo(point) < tol.equalPoint();
}


/*
Returns the value of the i-th coordinate of the 3D point.
For i equal to 0, returns X coordinate. For i equal to 1, returns Y coordinate.
*/
inline double
CGePoint3d::operator []	(unsigned int i) const
{
	return m_coords[i];
}


/*
Returns the reference to the i-th coordinate of the 3D point.
For i equal to 0, this returns the reference to the x coordinate.
For i equal to 1, this returns the reference to the y coordinate.
For i equal to 2, this returns the reference to the z coordinate.
*/
inline double&
CGePoint3d::operator []	(unsigned int i)
{
	return m_coords[i];
}


/*
Sets the value of the zeroth coodinate of this point to x ,
the value of the first coordinate to y and the value of the second coordinate to z.
*/
inline CGePoint3d&
CGePoint3d::set(double x, double y, double z)
{
	m_x = x;
	m_y = y;
	m_z = z;
	return *this;
}


/*
Sets this 3D point to be equal to the point given by the formula:
origin + uVector * point2d.x + vAxis * point2d.y,
where 3D point 'origin' and 3D vectors 'uVector' and 'vVector' form a coordinate
system in the planar entity 'planarSurf' and can found by function call
planarSurf.get(origin, uVector, vVector).
*/
CGePoint3d&
CGePoint3d::set(const CGePlanarSurface& planarSurf, const CGePoint2d& point2d)
{
	CGePoint3d	origin;
	CGeVector3d	uVector, vVector;
	planarSurf.get(origin, uVector, vVector);
	*this = origin + uVector * point2d.m_x + vVector * point2d.m_y;
	return *this;
}


//////////////////////////////////////////////////
// friend to CGePoint2d functions implementation
//////////////////////////////////////////////////


/*
Returns the point equal to the product of 'matrix' and 'point'. The order of
multiplication is where points are considered as columns.
*/
inline CGePoint3d
operator * (const CGeMatrix3d& matrix, const CGePoint3d& point)
{
	return CGePoint3d(matrix.m_a00 * point.m_x + matrix.m_a01 * point.m_y + matrix.m_a02 * point.m_z + matrix.m_a03,
					  matrix.m_a10 * point.m_x + matrix.m_a11 * point.m_y + matrix.m_a12 * point.m_z + matrix.m_a13,
					  matrix.m_a20 * point.m_x + matrix.m_a21 * point.m_y + matrix.m_a22 * point.m_z + matrix.m_a23);
}


/*
Returns the point equal to 'point' scaled by scale factor 'scale'.
*/
inline CGePoint3d
operator * (double scale, const CGePoint3d& point)
{
	return CGePoint3d(point.m_x * scale, point.m_y * scale, point.m_z * scale);
}
