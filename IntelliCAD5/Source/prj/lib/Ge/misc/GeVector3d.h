// File GeVector3d.h
//
// CGeVector3d
//

#pragma once


#include "GeContext.h"


class GE_API CGePlanarSurface;
class GE_API CGeVector2d;
class GE_API CGeMatrix3d;


class GE_API CGeVector3d
{
public:

	// data
	static const CGeVector3d	s_identity;
	static const CGeVector3d	s_xAxis;
	static const CGeVector3d	s_yAxis;
	static const CGeVector3d	s_zAxis;

	union
	{
		double	m_coords[3];
		struct
		{
			double	m_x, m_y, m_z;
		};
	};

	// constructors
	CGeVector3d();
	CGeVector3d(const CGeVector3d& source);
	CGeVector3d(double x, double y, double z);
	CGeVector3d(const CGePlanarSurface& planarSurf, const CGeVector2d& vector2d);

	// transformation
	CGeVector3d&		mirror				(const CGeVector3d& normalToPlane);
	CGeVector3d&		rotateBy			(double angle, const CGeVector3d& axis);
	CGeVector3d&		setToProduct		(const CGeMatrix3d& matrix, const CGeVector3d& vector);
	CGeVector3d&		transformBy			(const CGeMatrix3d& matrix);

	// scalar multiplication
	CGeVector3d			operator *			(double scale) const;
	CGeVector3d&		operator *=			(double scale);
	CGeVector3d&		setToProduct		(const CGeVector3d& vector, double scale);
	CGeVector3d			operator /			(double scale) const;
	CGeVector3d&		operator /=			(double scale);

	// vector addition/subtraction
	CGeVector3d			operator +			(const CGeVector3d& vector) const;
	CGeVector3d&		operator +=			(const CGeVector3d& vector);
	CGeVector3d			operator -			(const CGeVector3d& vector) const;
	CGeVector3d&		operator -=			(const CGeVector3d& vector);
	CGeVector3d&		setToSum			(const CGeVector3d& vector1, const CGeVector3d& vector2);

	// negation
	CGeVector3d			operator -			() const;
	CGeVector3d&		negate				();

	// find perpendicular vector
	CGeVector3d			perpVector			() const;

	// angle
	double				angleTo				(const CGeVector3d& vector) const;
	double				angleTo				(const CGeVector3d& vector, const CGeVector3d& refVector) const;
	double				angleOnPlane		(const CGePlanarSurface& planarSurf) const;

	// conversion to CGeVector2d
	CGeVector2d			convert2d			(const CGePlanarSurface& planarSurf) const;

	// normalization and length
	CGeVector3d			normal				(const CGeTolerance& tol = CGeContext::s_defTol) const;
	CGeVector3d&		normalize			(const CGeTolerance& tol = CGeContext::s_defTol);
	CGeVector3d&		normalize			(const CGeTolerance& tol, CGeError& flag);
	double				length				() const;
	double				lengthSqrd			() const;
	bool				isUnitLength		(const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool				isZeroLength		(const CGeTolerance& tol = CGeContext::s_defTol) const;

	// direction test
	bool				isParallelTo		(const CGeVector3d& vector, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool				isParallelTo		(const CGeVector3d& vector, const CGeTolerance& tol, CGeError& flag) const;
	bool				isCodirectionalTo	(const CGeVector3d& vector, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool				isCodirectionalTo	(const CGeVector3d& vector, const CGeTolerance& tol, CGeError& flag) const;
	bool				isPerpendicularTo	(const CGeVector3d& vector, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool				isPerpendicularTo	(const CGeVector3d& vector, const CGeTolerance& tol, CGeError& flag) const;

	// dot product
	double				dotProduct			(const CGeVector3d& vector) const;

	// cross product
	CGeVector3d			crossProduct		(const CGeVector3d& vector) const;

	// equivalence
	bool				operator ==			(const CGeVector3d& vector) const;
	bool				operator !=			(const CGeVector3d& vector) const;
	bool				isEqualTo			(const CGeVector3d& vector, const CGeTolerance& tol = CGeContext::s_defTol) const;

	// data access
	double				operator []			(unsigned int i) const;
	double&				operator []			(unsigned int i);
	unsigned int		largestElement		() const;

	// projection
	CGeVector3d			orthoProject		(const CGeVector3d& planeNormal) const;
	CGeVector3d			orthoProject		(const CGeVector3d& planeNormal, const CGeTolerance& tol, CGeError& flag) const;
	CGeVector3d			project				(const CGeVector3d& planeNormal, const CGeVector3d& projDir) const;
	CGeVector3d			project				(const CGeVector3d& planeNormal, const CGeVector3d& projDir, const CGeTolerance& tol, CGeError& flag) const;

	// conversion to the matrix of translation
						operator CGeMatrix3d() const;

	// evaluation of the matrix of rotation
	CGeMatrix3d			rotateTo			(const CGeVector3d& vector, const CGeVector3d& axis = CGeVector3d::s_identity) const;

	// set
	CGeVector3d&		set					(double x, double y, double z);
	CGeVector3d&		set					(const CGePlanarSurface& planarSurf, const CGeVector2d& vector2d);

	// friends
	friend GE_API CGeVector3d operator * (const CGeMatrix3d& matrix, const CGeVector3d& vector);
	friend GE_API CGeVector3d operator * (double scale, const CGeVector3d& vector);
};
