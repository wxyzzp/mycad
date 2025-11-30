// File GePoint3d.h
//
// CGePoint3d
//

#pragma once


#include "GeContext.h"


class GE_API CGePoint2d;
class GE_API CGePlanarSurface;
class GE_API CGeMatrix3d;
class GE_API CGePlane;


class GE_API CGePoint3d
{
public:

	// data
	static const CGePoint3d s_origin;

	union
	{
		double	m_coords[3];
		struct
		{
			double	m_x, m_y, m_z;
		};
	};

	// constructors
	CGePoint3d();
	CGePoint3d(const CGePoint3d& source);
	CGePoint3d(double x, double y, double z);
	CGePoint3d(const CGePlanarSurface& planarSurf, const CGePoint2d& point2d);

	// transformation
	CGePoint3d&		mirror		(const CGePlane& plane);
	CGePoint3d&		rotateBy	(double angle, const CGeVector3d& vector, const CGePoint3d& pivot = CGePoint3d::s_origin);
	CGePoint3d&		setToProduct(const CGeMatrix3d& matrix, const CGePoint3d& point);
	CGePoint3d&		transformBy	(const CGeMatrix3d& matrix);
	CGePoint3d&		scaleBy		(double scale, const CGePoint3d& pivot = CGePoint3d::s_origin);

	//  scale multiplication
	CGePoint3d		operator *	(double scale) const;
	CGePoint3d&		operator *=	(double scale);
	CGePoint3d		operator /	(double scale) const;
	CGePoint3d&		operator /=	(double scale);

	// translation by a vector
	CGePoint3d		operator +	(const CGeVector3d& vector) const;
	CGePoint3d&		operator +=	(const CGeVector3d& vector);
	CGePoint3d		operator -	(const CGeVector3d& vector) const;
	CGeVector3d		operator -	(const CGePoint3d& point) const;
	CGePoint3d&		operator -=	(const CGeVector3d& vector);
	CGePoint3d&		setToSum	(const CGePoint3d& point, const CGeVector3d& vector);

	// conversion to CGeVector3d
	CGeVector3d		asVector	() const;

	// conversion to CGePoint2d
	CGePoint2d		convert2d	(const CGePlanarSurface& planarSurf) const;

	// distance to other point
	double			distanceTo	(const CGePoint3d& point) const;

	// projection to plane
	CGePoint3d		orthoProject(const CGePlane& plane) const;
	CGePoint3d		project		(const CGePlane& plane, const CGeVector3d& projDir) const;

	// equivalence
	bool			operator ==	(const CGePoint3d& point) const;
	bool			operator !=	(const CGePoint3d& point) const;
	bool			isEqualTo	(const CGePoint3d& point, const CGeTolerance& tol = CGeContext::s_defTol) const;

	// data access
	double			operator []	(unsigned int i) const;
	double&			operator []	(unsigned int i);

	// set
	CGePoint3d&		set			(double x, double y, double z);
	CGePoint3d&		set			(const CGePlanarSurface& planarSurf, const CGePoint2d& point2d);

	// friends
	friend GE_API CGePoint3d	operator * (const CGeMatrix3d& matrix, const CGePoint3d& point);
	friend CGePoint3d			operator * (double scale, const CGePoint3d& point);
};
