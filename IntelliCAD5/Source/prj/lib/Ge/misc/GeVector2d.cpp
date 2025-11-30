// File: GeVector2d.cpp
//


#include "GeVector2d.h"
#include "GeMatrix2d.h"
#include "GeLine2d.h"
#include <math.h>


//////////////////////////////////////////////////
// CGeVector2d class implementation
//////////////////////////////////////////////////


const CGeVector2d	CGeVector2d::s_identity;
const CGeVector2d	CGeVector2d::s_xAxis(1.0, 0.0);
const CGeVector2d	CGeVector2d::s_yAxis(0.0, 1.0);


/*
Constructor. Creates a vector at the origin.
*/
inline
CGeVector2d::CGeVector2d() : m_x(0.0), m_y(0.0)
{
}


/*
Copy constructor.
*/
inline
CGeVector2d::CGeVector2d(const CGeVector2d& source) : m_x(source.m_x), m_y(source.m_y)
{
}

/*
Constructor. Constructs a 2D vector with coordinates x and y correspondingly.
*/
inline
CGeVector2d::CGeVector2d(double x, double y) : m_x(x), m_y(y)
{
}


/*
Sets this vector to the product of 'matrix' and 'vector'.
The order of multiplication is matrix * vector, where vectors are considered as columns.
Returns the reference to this vector.
*/
CGeVector2d&
CGeVector2d::setToProduct(const CGeMatrix2d& matrix, const CGeVector2d& vector)
{
	double	x = matrix.m_a00 * vector.m_x + matrix.m_a01 * vector.m_y;
	m_y = matrix.m_a10 * vector.m_x + matrix.m_a11 * vector.m_y;
	m_x = x;
	return *this;
}


/*
Sets this vector to the product of 'vector' and 'scale'.
*/
inline CGeVector2d&
CGeVector2d::setToProduct(const CGeVector2d& vector, double scale)
{
	m_x = vector.m_x * scale;
	m_y = vector.m_y * scale;
	return *this;
}


/*
Returns 2D vector which is the result of rotation of this vector.
Rotation angle is given by the argument 'angle', where the positive direction
of rotation is considered from X axis to Y axis.
*/
inline CGeVector2d&
CGeVector2d::rotateBy(double angle)
{
	return transformBy(CGeMatrix2d::rotation(angle));
}


/*
Returns the 2D vector which is the result of mirroring of this vector with respect
to 2D 'line' in the same plane.
*/
inline CGeVector2d&
CGeVector2d::mirror(const CGeVector2d& line)
{
	return transformBy(CGeMatrix2d::mirroring(CGeLine2d(CGePoint2d::s_origin, line)));
}


/*
Transform this pre-multiplying it the 'matrix'.
Returns the reference to this vector.
Result is the same as setToProduct(matrix, *this).
*/
inline CGeVector2d&
CGeVector2d::transformBy(const CGeMatrix2d& matrix)
{
	return setToProduct(matrix, *this);
}


/*
Returns the vector equal to this vector multiplied by scale factor 'scale'.
*/
inline CGeVector2d
CGeVector2d::operator * (double scale) const
{
	return CGeVector2d(m_x * scale, m_y * scale);
}


/*
Multiplies this vector by scale factor 'scale'. Returns the reference to this vector.
*/
inline CGeVector2d&
CGeVector2d::operator *= (double scale)
{
	m_x *= scale;
	m_y *= scale;
	return *this;
}


/*
Returns the vector equal to this vector multiplied by scale factor 1 / 'scale'.
*/
inline CGeVector2d
CGeVector2d::operator / (double scale) const
{
	return CGeVector2d(m_x / scale, m_y / scale);
}


/*
Multiplies this vector by scale factor 1 / 'scale'. Returns the reference to this vector.
*/
inline CGeVector2d&
CGeVector2d::operator /= (double scale)
{
	m_x /= scale;
	m_y /= scale;
	return *this;
}


/*
Returns the vector equal to the sum of this vector and 'vector'.
*/
inline CGeVector2d
CGeVector2d::operator + (const CGeVector2d& vector) const
{
	return CGeVector2d(m_x + vector.m_x, m_y + vector.m_y);
}


/*
Adds 'vector' to this vector. Returns the reference to this vector.
*/
inline CGeVector2d&
CGeVector2d::operator += (const CGeVector2d& vector)
{
	m_x += vector.m_x;
	m_y += vector.m_y;
	return *this;
}


/*
Returns the result of subtraction of the 'vector' from this vector.
*/
inline CGeVector2d
CGeVector2d::operator - (const CGeVector2d& vector) const
{
	return CGeVector2d(m_x - vector.m_x, m_y - vector.m_y);
}


/*
Subtracts 'vector' from this vector. Returns the reference to this vector.
*/
inline CGeVector2d&
CGeVector2d::operator -= (const CGeVector2d& vector)
{
	m_x -= vector.m_x;
	m_y -= vector.m_y;
	return *this;
}


/*
Sets this vector to the sum of the 'vector1' and 'vector2'.
Returns the reference to this vector.
*/
inline CGeVector2d&
CGeVector2d::setToSum(const CGeVector2d& vector1, const CGeVector2d& vector2)
{
	m_x = vector1.m_x + vector2.m_x;
	m_y = vector1.m_y + vector2.m_y;
	return *this;
}


/*
Returns the vector opposite to this vector, namely the vector (-m_x, -m_y).
*/
inline CGeVector2d
CGeVector2d::operator - () const
{
	return CGeVector2d(-m_x, -m_y);
}


/*
Negates this vector, namely sets it to be equal to the vector (-m_x, -m_y).
Returns the reference to this vector.
*/
inline CGeVector2d&
CGeVector2d::negate()
{
	m_x = -m_x;
	m_y = -m_y;
	return *this;
}


/*
Returns a vector orthogonal to this vector, namely the vector (-m_y, m_x).
*/
inline CGeVector2d
CGeVector2d::perpVector() const
{
	return CGeVector2d(-m_y, m_x);
}


/*
Returns the angle between this vector and X axis in the range [0, Pi].
*/
double
CGeVector2d::angle() const
{
	return angleTo(s_xAxis);
}


/*
Returns the angle between this vector and 'vector' in the range [0, Pi].
*/
double
CGeVector2d::angleTo(const CGeVector2d& vector) const
{
	double	lengthsProduct = length();
	lengthsProduct *= vector.length();
	if(lengthsProduct < CGeContext::s_defTol.equalVector())
		return 0.0;
	else
		return acos(dotProduct(vector) / lengthsProduct);
}


/*
Returns the unit vector codirectional to this vector. The vector is not normalized
if its length is less than tol.equalVector(), in which case the returned vector is
the same as this vector.
*/
CGeVector2d
CGeVector2d::normal(const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	double	l = length();
	if(l < tol.equalVector())
		return CGeVector2d(*this);
	else
		return CGeVector2d(*this / l);
}


/*
Sets this vector to the unit vector codirectional to this vector. Returns the
reference to this vector. This vector is not normalized if its length is less
than tol.equalVector(), in which case it remains unchanged.
*/
CGeVector2d&
CGeVector2d::normalize(const CGeTolerance& tol /* = CGeContext::s_defTol*/)
{
	double	l = length();
	if(l >= tol.equalVector())
		*this /= l;
	return *this;
}


/*
Sets this vector to the unit vector codirectional to this vector. Returns the
reference to this vector. This vector is not normalized if its length is less
than tol.equalVector(), in which case it remains unchanged. The flag argument
will be set to the following value to indicate why a failure may have occurred: 'e0This'.
*/
CGeVector2d&
CGeVector2d::normalize(const CGeTolerance& tol, CGeError& flag)
{
	double	l = length();
	if(l < tol.equalVector())
		flag = CGe::e0This;
	else
		*this /= l;
	return *this;
}


/*
Returns 'true' if this vector is equal to its normalization. Returns 'false' otherwise.
*/
inline bool
CGeVector2d::isUnitLength(const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return isEqualTo(normal(tol), tol);
}


/*
Returns 'true' if this vector is equal to zero vector. Returns 'false' otherwise.
*/
inline bool
CGeVector2d::isZeroLength(const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return isEqualTo(CGeVector2d::s_identity, tol);
}


/*
Returns the Euclidian length of this vector.
*/
inline double
CGeVector2d::length() const
{
	return sqrt(lengthSqrd());
}


/*
Returns the square of the Euclidian length of this vector.
*/
inline double
CGeVector2d::lengthSqrd() const
{
	return m_x * m_x + m_y * m_y;
}


/*
Checks if this vector is parallel to 'vector'. Namely, it checks if after normalization
(and negation in case of negative dot product with 'vector') this vector is within the
distance tol.equalVector() from the normalization of the 'vector'.
Returns 'true' if this condition is met and 'false' otherwise.
*/
inline bool
CGeVector2d::isParallelTo(const CGeVector2d& vector, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	if(dotProduct(vector) > 0.0)
		return (normal(tol) - vector.normal(tol)).length() < tol.equalVector();
	else
		return (normal(tol) + vector.normal(tol)).length() < tol.equalVector();
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
CGeVector2d::isParallelTo(const CGeVector2d& vector, const CGeTolerance& tol, CGeError& flag) const
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
This function checks if this vector is codirectional to 'vector'. Namely, it checks
if after normalization this vector is within the distance tol.equalVector() from the
normalization of 'vector'.
Returns 'true' if this condition is met and 'false' otherwise.
*/
inline bool
CGeVector2d::isCodirectionalTo(const CGeVector2d& vector, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
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
CGeVector2d::isCodirectionalTo(const CGeVector2d& vector, const CGeTolerance& tol, CGeError& flag) const
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
CGeVector2d::isPerpendicularTo(const CGeVector2d& vector, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return fabs(normal().dotProduct(vector.normal())) < tol.equalVector();
}


/*
Checks if this vector is perpendicular to 'vector'. Namely, it checks if after
normalization the dot product of this vector with the normalization of 'vector'
is less than tol.equalVector() in absolute value.
Returns 'true' if this condition is met and 'false' otherwise. If 'false' is returned,
the 'flag' argument will be set to one of the following values to indicate why
a failure may have occurred: 'e0This' or 'e0Arg'.
*/
bool
CGeVector2d::isPerpendicularTo(const CGeVector2d& vector, const CGeTolerance& tol, CGeError& flag) const
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
Returns the dot product of this vector and the 'vector'.
*/
inline double
CGeVector2d::dotProduct(const CGeVector2d& vector) const
{
	return m_x * vector.m_x + m_y * vector.m_y;
}


/*
This function works the same as isEqualTo(vector).
*/
inline bool
CGeVector2d::operator == (const CGeVector2d& vector) const
{
	return isEqualTo(vector);
}


/*
This function works the same as !isEqualTo(vector).
*/
inline bool
CGeVector2d::operator != (const CGeVector2d& vector) const
{
	return !isEqualTo(vector);
}


/*
Checks if this vector is within the distance tol.equalVector() from 'vector'.
The default value of the tolerance class tol is CGeContext::s_defTol.
Returns 'true' if this condition is met and 'false' otherwise.
*/
inline bool
CGeVector2d::isEqualTo(const CGeVector2d& vector, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return (*this - vector).length() < tol.equalVector();
}


/*
Returns the value of the i-th coordinate of the 2D vector.
For i equal to 0, returns X coordinate. For i equal to 1, returns Y coordinate.
*/
inline double
CGeVector2d::operator [] (unsigned int i) const
{
	return m_coords[i];
}


/*
Returns the reference to the i-th coordinate of the 2D vector.
For i equal to 0, returns the reference to the X coordinate.
For i equal to 1, returns the reference to the Y coordinate.
*/
inline double&
CGeVector2d::operator [] (unsigned int i)
{
	return m_coords[i];
}


/*
Returns the matrix of translation by this vector.
*/
inline
CGeVector2d::operator CGeMatrix2d() const
{
	return CGeMatrix2d::translation(*this);
}


/*
Sets the value of the zeroth coodinate of this vector to x and value of the first coordinate to y.
Returns the reference to this vector.
*/
inline CGeVector2d&
CGeVector2d::set(double x, double y)
{
	m_x = x;
	m_y = y;
	return *this;
}


//////////////////////////////////////////////////
// friend to CGeVector2d functions implementation
//////////////////////////////////////////////////


/*
Returns the vector equal to the product of 'matrix' and 'vector'.
The order of multiplication is matrix * vector, where vectors are considered as columns.
*/
inline CGeVector2d
operator * (const CGeMatrix2d& matrix, const CGeVector2d& vector)
{
	return CGeVector2d(matrix.m_a00 * vector.m_x + matrix.m_a01 * vector.m_y,
					   matrix.m_a10 * vector.m_x + matrix.m_a11 * vector.m_y);
}


/*
Returns the vector equal to 'vector' multiplied by scale factor 'scale'.
*/
inline CGeVector2d
operator * (double scale, const CGeVector2d& vector)
{
	return CGeVector2d(vector.m_x * scale, vector.m_y * scale);
}
