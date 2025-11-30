// File: GeEntity2d.cpp
//

#include "GeEntity2d.h"

//////////////////////////////////////////////////
// CGeEntity2d class implementation
//////////////////////////////////////////////////

CGeEntity2d::~CGeEntity2d()
{
}

bool
CGeEntity2d::isKindOf(CGe::EEntityId entityType) const
{
	return entityType == CGe::eEntity2d;
}

CGe::EEntityId
CGeEntity2d::type() const
{
	return CGe::eEntity2d;
}

CGeEntity2d*
CGeEntity2d::copy() const
{
	CGeEntity2d*	pNewEntity = new CGeEntity2d(*this);
	return pNewEntity;
}

CGeEntity2d&
CGeEntity2d::operator = (const CGeEntity2d& source)
{
	return *this;
}

bool
CGeEntity2d::operator == (const CGeEntity2d& entity) const
{
	return false;
}

bool
CGeEntity2d::operator != (const CGeEntity2d& entity) const
{
	return false;
}

bool
CGeEntity2d::isEqualTo(const CGeEntity2d& entity, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

CGeEntity2d&
CGeEntity2d::transformBy(const CGeMatrix2d& matrix)
{
	return *this;
}

CGeEntity2d&
CGeEntity2d::translateBy(const CGeVector2d& vector)
{
	return *this;
}

CGeEntity2d&
CGeEntity2d::rotateBy(double angle, const CGePoint2d& pivot /* = CGePoint2d::s_origin*/)
{
	return *this;
}

CGeEntity2d&
CGeEntity2d::scaleBy(double scaleFactor, const CGePoint2d& centerPoint /* = CGePoint2d::s_origin*/)
{
	return *this;
}

CGeEntity2d&
CGeEntity2d::mirror(const CGeLine2d& line)
{
	return *this;
}

bool
CGeEntity2d::isOn(const CGePoint2d& point, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

CGeEntity2d::CGeEntity2d()
{
}

CGeEntity2d::CGeEntity2d(const CGeEntity2d& source)
{
}
