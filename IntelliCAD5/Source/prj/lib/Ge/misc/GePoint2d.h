// File GePoint2d.h
//
// CGePoint2d
//

#pragma once


#include "GeContext.h"


class GE_API CGeMatrix2d;
class GE_API CGeLine2d;
class GE_API CGeVector2d;


class GE_API CGePoint2d
{
public:

	// data
	static const CGePoint2d s_origin;

	union
	{
		double	m_coords[2];
		struct
		{
			double	m_x, m_y;
		};
	};

	// constructors
	CGePoint2d();
	CGePoint2d(const CGePoint2d& source);
	CGePoint2d(double x, double y);

	// transformation
	CGePoint2d&		setToProduct(const CGeMatrix2d& matrix, const CGePoint2d& point);
	CGePoint2d&		mirror		(const CGeLine2d& line);
	CGePoint2d&		rotateBy	(double angle, const CGePoint2d& pivot = CGePoint2d::s_origin);
	CGePoint2d&		scaleBy		(double scale, const CGePoint2d& pivot = CGePoint2d::s_origin);
	CGePoint2d&		setToSum	(const CGePoint2d& point, const CGeVector2d& vector);
	CGePoint2d&		transformBy	(const CGeMatrix2d& matrix);

	// scale multiplication
	CGePoint2d		operator *	(double scale) const;
	CGePoint2d&		operator *=	(double scale);
	CGePoint2d		operator /	(double scale) const;
	CGePoint2d&		operator /=	(double scale);
	
	// translation by a vector
	CGePoint2d		operator +	(const CGeVector2d& vector) const;
	CGePoint2d&		operator +=	(const CGeVector2d& vector);
	CGePoint2d		operator -	(const CGeVector2d& vector) const;
	CGeVector2d		operator -	(const CGePoint2d& point) const;
	CGePoint2d&		operator -=	(const CGeVector2d& vector);

	// conversion to CGeVector2d
	CGeVector2d		asVector	() const;

	// distance to other point
	double			distanceTo	(const CGePoint2d& point) const;

	// equivalence
	bool			operator ==	(const CGePoint2d& point) const;
	bool			operator !=	(const CGePoint2d& point) const;
	bool			isEqualTo	(const CGePoint2d& point, const CGeTolerance& tol = CGeContext::s_defTol) const;

	// data access
	double			operator []	(unsigned int i) const;
	double&			operator []	(unsigned int i);

	// set
	CGePoint2d&		set			(double x, double y);

	// friends
	friend GE_API CGePoint2d operator * (const CGeMatrix2d& matrix, const CGePoint2d& point);
	friend        CGePoint2d operator * (double scale, const CGePoint2d& point);
};
