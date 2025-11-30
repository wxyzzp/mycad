// File: GeEntity3d.h
//
// CGeEntity3d
//

#pragma once


#include "GeContext.h"
#include "GePoint3d.h"


class GE_API CGeMatrix3d;
class GE_API CGePlane;


class GE_API CGeEntity3d
{
public:
	virtual ~CGeEntity3d();

	bool			isKindOf    (CGe::EEntityId entityType) const;
	CGe::EEntityId	type        () const;

	CGeEntity3d*	copy		() const;
	CGeEntity3d&	operator =	(const CGeEntity3d& source);
	bool			operator ==	(const CGeEntity3d& entity) const;
	bool			operator !=	(const CGeEntity3d& entity) const;
	bool			isEqualTo	(const CGeEntity3d& entity, const CGeTolerance& tol = CGeContext::s_defTol) const;
	CGeEntity3d&	transformBy	(const CGeMatrix3d& matrix);
	CGeEntity3d&	translateBy	(const CGeVector3d& vector);
	CGeEntity3d&	rotateBy	(double angle, const CGeVector3d& vector, const CGePoint3d& pivot = CGePoint3d::s_origin);
	CGeEntity3d&	scaleBy		(double scaleFactor, const CGePoint3d& centerPoint = CGePoint3d::s_origin);
	CGeEntity3d&	mirror		(const CGePlane& plane);
	bool			isOn		(const CGePoint3d& point, const CGeTolerance& tol = CGeContext::s_defTol) const;

protected:
	CGeEntity3d();
	CGeEntity3d(const CGeEntity3d& source);

/* original code:
	friend class AcGeImpEntity3d;
	AcGeImpEntity3d  *mpImpEnt;
	int              mDelEnt;
	AcGeEntity3d (const AcGeEntity3d&);
	AcGeEntity3d (AcGeImpEntity3d&, int);
	AcGeEntity3d (AcGeImpEntity3d*);
	AcGeEntity2d* newEntity2d (AcGeImpEntity3d*) const;
	AcGeEntity2d* newEntity2d (AcGeImpEntity3d&, int) const;
	AcGeEntity3d* newEntity3d (AcGeImpEntity3d*) const;
	AcGeEntity3d* newEntity3d (AcGeImpEntity3d&, int) const;*/
};
