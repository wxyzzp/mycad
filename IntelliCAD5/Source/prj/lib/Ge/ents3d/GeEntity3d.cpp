// File: GeEntity3d.cpp
//

#include "GeEntity3d.h"

//////////////////////////////////////////////////
// CGeEntity3d class implementation
//////////////////////////////////////////////////

CGeEntity3d::~CGeEntity3d()
{
}

bool
CGeEntity3d::isKindOf(CGe::EEntityId entityType) const
{
	return entityType == CGe::eEntity3d;
}

CGe::EEntityId
CGeEntity3d::type() const
{
	return CGe::eEntity3d;
}

CGeEntity3d*
CGeEntity3d::copy() const
{
	CGeEntity3d*	pNewEntity = new CGeEntity3d(*this);
	return pNewEntity;
}

CGeEntity3d&
CGeEntity3d::operator = (const CGeEntity3d& source)
{
	return *this;
}

bool
CGeEntity3d::operator == (const CGeEntity3d& entity) const
{
	return false;
}

bool
CGeEntity3d::operator != (const CGeEntity3d& entity) const
{
	return false;
}

bool
CGeEntity3d::isEqualTo(const CGeEntity3d& entity, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

CGeEntity3d&
CGeEntity3d::transformBy(const CGeMatrix3d& matrix)
{
	return *this;
}

CGeEntity3d&
CGeEntity3d::translateBy(const CGeVector3d& vector)
{
	return *this;
}

CGeEntity3d&
CGeEntity3d::rotateBy(double angle, const CGeVector3d& vector, const CGePoint3d& pivot /* = CGePoint3d::s_origin*/)
{
	return *this;
}

CGeEntity3d&
CGeEntity3d::scaleBy(double scaleFactor, const CGePoint3d& centerPoint /* = CGePoint3d::s_origin*/)
{
	return *this;
}

CGeEntity3d&
CGeEntity3d::mirror(const CGePlane& plane)
{
	return *this;
}

bool
CGeEntity3d::isOn(const CGePoint3d& point, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

CGeEntity3d::CGeEntity3d()
{
}

CGeEntity3d::CGeEntity3d(const CGeEntity3d& source)
{
}
