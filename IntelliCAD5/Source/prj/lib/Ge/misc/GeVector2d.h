// File GeVector2d.h
//
// CGeVector2d
//

#pragma once


#include "GeContext.h"


class GE_API CGeMatrix2d;


class GE_API CGeVector2d
{
public:

	// data
	static const CGeVector2d	s_identity;
	static const CGeVector2d	s_xAxis;
	static const CGeVector2d	s_yAxis;

	union
	{
		double	m_coords[2];
		struct
		{
			double	m_x, m_y;
		};
	};

	// constructors
	CGeVector2d();
	CGeVector2d(const CGeVector2d& source);
	CGeVector2d(double x, double y);

	// transformation
	CGeVector2d&	setToProduct		(const CGeMatrix2d& matrix, const CGeVector2d& vector);
	CGeVector2d&	setToProduct		(const CGeVector2d& vector, double scale);
	CGeVector2d&	rotateBy			(double angle);
	CGeVector2d&	mirror				(const CGeVector2d& line);
	CGeVector2d&	transformBy			(const CGeMatrix2d& matrix);

	// scale multiplication
	CGeVector2d		operator *			(double scale) const;
	CGeVector2d&	operator *=			(double scale);
	CGeVector2d		operator /			(double scale) const;
	CGeVector2d&	operator /=			(double scale);

	// vector addition/subtraction
	CGeVector2d		operator +			(const CGeVector2d& vector) const;
	CGeVector2d&	operator +=			(const CGeVector2d& vector);
	CGeVector2d		operator -			(const CGeVector2d& vector) const;
	CGeVector2d&	operator -=			(const CGeVector2d& vector);
	CGeVector2d&	setToSum			(const CGeVector2d& vector1, const CGeVector2d& vector2);

	// negation
	CGeVector2d		operator -			() const;
	CGeVector2d&	negate				();

	// find perpendicular vector
	CGeVector2d		perpVector			() const;

	// angle
    double			angle				() const;
	double			angleTo				(const CGeVector2d& vector) const;

	// normalization and length
	CGeVector2d		normal				(const CGeTolerance& tol = CGeContext::s_defTol) const;
	CGeVector2d&	normalize			(const CGeTolerance& tol = CGeContext::s_defTol);
	CGeVector2d&	normalize			(const CGeTolerance& tol, CGeError& flag);
	bool			isUnitLength		(const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool			isZeroLength		(const CGeTolerance& tol = CGeContext::s_defTol) const;
	double			length				() const;
	double			lengthSqrd			() const;

	// direction test
	bool			isParallelTo		(const CGeVector2d& vector, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool			isParallelTo		(const CGeVector2d& vector, const CGeTolerance& tol, CGeError& flag) const;
	bool			isCodirectionalTo	(const CGeVector2d& vector, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool			isCodirectionalTo	(const CGeVector2d& vector, const CGeTolerance& tol, CGeError& flag) const;
	bool			isPerpendicularTo	(const CGeVector2d& vector, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool			isPerpendicularTo	(const CGeVector2d& vector, const CGeTolerance& tol, CGeError& flag) const;

	// dot product
	double			dotProduct			(const CGeVector2d& vector) const;

	// equivalence
	bool			operator ==			(const CGeVector2d& vector) const;
	bool			operator !=			(const CGeVector2d& vector) const;
	bool			isEqualTo			(const CGeVector2d& vector, const CGeTolerance& tol = CGeContext::s_defTol) const;

	// data access
	double			operator []			(unsigned int i) const;
	double&			operator []			(unsigned int i);

	// conversion to the matrix of translation
					operator CGeMatrix2d() const;

	// set
	CGeVector2d&	set					(double x, double y);

	// friends
	friend GE_API CGeVector2d operator * (const CGeMatrix2d& matrix, const CGeVector2d& vector);
	friend GE_API CGeVector2d operator * (double scale, const CGeVector2d& vector);
};
