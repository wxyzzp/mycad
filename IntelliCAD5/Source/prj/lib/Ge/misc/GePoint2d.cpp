// File: GePoint2d.cpp
//


#include "GePoint2d.h"
#include "GeVector2d.h"
#include "GeMatrix2d.h"
#include <math.h>


//////////////////////////////////////////////////
// CGePoint2d class implementation
//////////////////////////////////////////////////


const CGePoint2d CGePoint2d::s_origin;

/*
Constructor: Creates a point at the origin.
*/
inline
CGePoint2d::CGePoint2d() : m_x(0.0), m_y(0.0)
{
}


/*
Copy constructor.
*/
inline
CGePoint2d::CGePoint2d(const CGePoint2d& source) : m_x(source.m_x), m_y(source.m_y)
{
}


/*
Constructor: Constructs 2D point with coordinates 'x' and 'y'.
*/
inline
CGePoint2d::CGePoint2d(double x, double y) : m_x(x), m_y(y)
{
}


/*
Sets this point to the product of 'matrix' and 'point'. The order of multiplication
is matrix * point, where points are considered as columns. Returns the reference
to this point.
*/
CGePoint2d&
CGePoint2d::setToProduct(const CGeMatrix2d& matrix, const CGePoint2d& point)
{
	double	x = matrix.m_a00 * point.m_x + matrix.m_a01 * point.m_y + matrix.m_a02;
	m_y = matrix.m_a10 * point.m_x + matrix.m_a11 * point.m_y + matrix.m_a12;
	m_x = x;
	return *this;
}


/*
Returns the 2D point which is the result of mirroring of this point with respect to
2D 'line' in the same plane.
*/
inline CGePoint2d&
CGePoint2d::mirror(const CGeLine2d& line)
{
	return transformBy(CGeMatrix2d::mirroring(line));
}


/*
Returns 2D point which is the result of rotation of this point around the 2D point
'pivot'. Rotation angle is given by the argument 'angle', where positive direction
of rotation is considered from x axis to y axis.
*/
inline CGePoint2d&
CGePoint2d::rotateBy(double angle, const CGePoint2d& pivot /* = CGePoint2d::s_origin*/)
{
	return transformBy(CGeMatrix2d::rotation(angle, pivot));
}


/*
Returns 2D point that is the result of scaling this point with respect to the input
point by the scale factor 'scale'.
*/
inline CGePoint2d&
CGePoint2d::scaleBy(double scale, const CGePoint2d& pivot/* = CGePoint2d::s_origin*/)
{
	return transformBy(CGeMatrix2d::scaling(scale, pivot));
}


/*
Sets this point to the sum of the 'point' and 'vector'. Returns the reference to this point.
*/
inline CGePoint2d&
CGePoint2d::setToSum(const CGePoint2d& point, const CGeVector2d& vector)
{
	m_x = point.m_x + vector.m_x;
	m_y = point.m_y + vector.m_y;
	return *this;
}


/*
Transforms this pre-multiplying it by the 'matrix'. Returns the reference to this
point. Result is the same as setToProduct(matrix, *this).
*/
inline CGePoint2d&
CGePoint2d::transformBy(const CGeMatrix2d& matrix)
{
	return setToProduct(matrix, *this);
}


/*
Returns the point equal to this point scaled by scale factor 'scale'.
*/
inline CGePoint2d
CGePoint2d::operator * (double scale) const
{
	return CGePoint2d(m_x * scale, m_y * scale);
}


/*
Scales this point by scale factor 'scale'. Returns the reference to this point.
*/
inline CGePoint2d&
CGePoint2d::operator *= (double scale)
{
	m_x *= scale;
	m_y *= scale;
	return *this;
}


/*
Returns the point equal to this point scaled by scale factor 1/scale.
*/
inline CGePoint2d
CGePoint2d::operator / (double scale) const
{
	return CGePoint2d(m_x / scale, m_y / scale);
}


/*
*/
inline CGePoint2d&
CGePoint2d::operator /= (double scale)
{
	m_x /= scale;
	m_y /= scale;
	return *this;
}


/*
Returns the point equal to this point translated by 'vector'.
*/
inline CGePoint2d
CGePoint2d::operator + (const CGeVector2d& vector) const
{
	return CGePoint2d(m_x + vector.m_x, m_y + vector.m_y);
}


/*
Translates this point by 'vector'. Returns the reference to this point.
*/
inline CGePoint2d&
CGePoint2d::operator += (const CGeVector2d& vector)
{
	m_x += vector.m_x;
	m_y += vector.m_y;
	return *this;
}


/*
Returns the point equal to this point translated by vector opposite to 'vector'.
*/
inline CGePoint2d
CGePoint2d::operator - (const CGeVector2d& vector) const
{
	return CGePoint2d(m_x - vector.m_x, m_y - vector.m_y);
}


/*
Returns the 2D vector representing the displacement from 2D 'point' to this 2D point.
*/
inline CGeVector2d
CGePoint2d::operator - (const CGePoint2d& point) const
{
	return CGeVector2d(m_x - point.m_x, m_y - point.m_y);
}


/*
Translates this point by vector opposite to 'vector'. Returns the reference to this point.
*/
inline CGePoint2d&
CGePoint2d::operator -= (const CGeVector2d& vector)
{
	m_x -= vector.m_x;
	m_y -= vector.m_y;
	return *this;
}


/*
Converts 2D point into 2D vector with the same set of coordinates.
Returns an instance of CGeVector2d.
*/
inline CGeVector2d
CGePoint2d::asVector() const
{
	return CGeVector2d(m_x, m_y);
}


/*
Evaluates and returns distance from this point to the 'point'.
*/
inline double
CGePoint2d::distanceTo(const CGePoint2d& point) const
{
	return sqrt((point.m_x - m_x) * (point.m_x - m_x) + (point.m_y - m_y) * (point.m_y - m_y));
}


/*
This function works the same way as isEqualTo(point).
*/
inline bool
CGePoint2d::operator ==	(const CGePoint2d& point) const
{
	return isEqualTo(point);
}


/*
This function works the same way as !isEqualTo(point).
*/
inline bool
CGePoint2d::operator !=	(const CGePoint2d& point) const
{
	return !isEqualTo(point);
}


/*
Checks if this point is within the distance tol.equalPoint() from the 'point'.
The default value of the tolerance class tol is CGeContext::s_defTol.
Returns 'true' if the condition is met; otherwise, returns 'false'.
*/
inline bool
CGePoint2d::isEqualTo(const CGePoint2d& point, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return distanceTo(point) < tol.equalPoint();
}


/*
Returns the value of the i-th coordinate of the 2D point.
If i is equal to 0, returns the x coordinate. If i is equal to 1, returns the y coordinate.
*/
inline double
CGePoint2d::operator [] (unsigned int i) const
{
	return m_coords[i];
}


/*
Returns a reference to the i-th coordinate of the 2D point.
If i is equal to 0, returns a reference to the x coordinate.
If i is equal to 1, returns a reference the y coordinate.
*/
inline double&
CGePoint2d::operator [] (unsigned int i)
{
	return m_coords[i];
}


/*
Sets the value of the 0-th coordinate of this point to x and value of the 1-st coordinate to y.
Returns reference to this point.
*/
inline CGePoint2d&
CGePoint2d::set(double x, double y)
{
	m_x = x;
	m_y = y;
	return *this;
}


//////////////////////////////////////////////////
// friend to CGePoint2d functions implementation
//////////////////////////////////////////////////


/*
Returns the point equal to the product of 'matrix' and 'point'. The order of
multiplication is matrix * point, where points are considered as columns.
*/
inline CGePoint2d
operator * (const CGeMatrix2d& matrix, const CGePoint2d& point)
{
	return CGePoint2d(matrix.m_a00 * point.m_x + matrix.m_a01 * point.m_y + matrix.m_a02,
					  matrix.m_a10 * point.m_x + matrix.m_a11 * point.m_y + matrix.m_a12);
}


/*
Returns the point equal to 'point' scaled by scale factor 'scale'.
*/
inline CGePoint2d
operator * (double scale, const CGePoint2d& point)
{
	return CGePoint2d(point.m_x * scale, point.m_y * scale);
}
