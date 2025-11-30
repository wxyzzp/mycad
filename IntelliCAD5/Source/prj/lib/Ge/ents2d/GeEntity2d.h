// File: GeEntity2d.h
//
// CGeEntity2d
//

#pragma once


#include "GeContext.h"
#include "GePoint2d.h"


class GE_API CGeMatrix2d;
class GE_API CGeVector2d;
class GE_API CGeLine2d;


class GE_API CGeEntity2d
{
public:
	virtual ~CGeEntity2d();

	bool			isKindOf    (CGe::EEntityId entityType) const;
	CGe::EEntityId	type        () const;

	CGeEntity2d*	copy		() const;
	CGeEntity2d&	operator =	(const CGeEntity2d& source);
	bool			operator ==	(const CGeEntity2d& entity) const;
	bool			operator !=	(const CGeEntity2d& entity) const;
	bool			isEqualTo	(const CGeEntity2d& entity, const CGeTolerance& tol = CGeContext::s_defTol) const;
	CGeEntity2d&	transformBy	(const CGeMatrix2d& matrix);
	CGeEntity2d&	translateBy	(const CGeVector2d& vector);
	CGeEntity2d&	rotateBy	(double angle, const CGePoint2d& pivot = CGePoint2d::s_origin);
	CGeEntity2d&	scaleBy		(double scaleFactor, const CGePoint2d& centerPoint = CGePoint2d::s_origin);
	CGeEntity2d&	mirror		(const CGeLine2d& line);
	bool			isOn		(const CGePoint2d& point, const CGeTolerance& tol = CGeContext::s_defTol) const;

protected:
	CGeEntity2d();
	CGeEntity2d(const CGeEntity2d& source);

/* original code:
	friend class AcGeEntity3d;
	friend class AcGeImpEntity3d;
	AcGeImpEntity3d* mpImpEnt;
	int mDelEnt;
	AcGeEntity2d (const AcGeEntity2d&);
	AcGeEntity2d (AcGeImpEntity3d&, int);
	AcGeEntity2d (AcGeImpEntity3d*);
	AcGeEntity2d*    newEntity2d (AcGeImpEntity3d*) const;*/
};
