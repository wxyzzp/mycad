// File: GeMatrix2d.cpp
//

#include "GeMatrix2d.h"
#include "GeVector2d.h"

//////////////////////////////////////////////////
// CGeMatrix2d class implementation
//////////////////////////////////////////////////

CGeMatrix2d::CGeMatrix2d()
{
}

CGeMatrix2d::CGeMatrix2d(const CGeMatrix2d& source)
{
}

CGeMatrix2d&
CGeMatrix2d::setToIdentity()
{
	return *this;
}

CGeMatrix2d
CGeMatrix2d::operator * (const CGeMatrix2d& matrix) const
{
	return CGeMatrix2d();
}

CGeMatrix2d&
CGeMatrix2d::operator *= (const CGeMatrix2d& matrix)
{
	return *this;
}

CGeMatrix2d&
CGeMatrix2d::preMultBy(const CGeMatrix2d& leftMatrix)
{
	return *this;
}

CGeMatrix2d&
CGeMatrix2d::postMultBy(const CGeMatrix2d& rightMatrix)
{
	return *this;
}

CGeMatrix2d&
CGeMatrix2d::setToProduct(const CGeMatrix2d& matrix1, const CGeMatrix2d& matrix2)
{
	return *this;
}

CGeMatrix2d&
CGeMatrix2d::invert()
{
	return *this;
}

CGeMatrix2d
CGeMatrix2d::inverse() const
{
	return CGeMatrix2d();
}

bool
CGeMatrix2d::isSingular(const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

CGeMatrix2d&
CGeMatrix2d::transposeIt()
{
	return *this;
}

CGeMatrix2d
CGeMatrix2d::transpose() const
{
	return CGeMatrix2d();
}

bool
CGeMatrix2d::isEqualTo(const CGeMatrix2d& matrix, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeMatrix2d::isUniScaledOrtho(const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeMatrix2d::isScaledOrtho(const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

double
CGeMatrix2d::scale() const
{
	return false;
}

double
CGeMatrix2d::det() const
{
	return 0.0;
}

CGeMatrix2d&
CGeMatrix2d::setTranslation(const CGeVector2d& vector)
{
	return *this;
}

CGeVector2d
CGeMatrix2d::translation() const
{
	return CGeVector2d();
}

bool
CGeMatrix2d::isConformal(double& scale, double& angle, bool& bIsMirror, CGeVector2d& reflex) const
{
	return false;
}

void
CGeMatrix2d::getCoordSystem(CGePoint2d& origin, CGeVector2d& xAxis, CGeVector2d& yAxis) const
{
}

CGeMatrix2d&
CGeMatrix2d::setCoordSystem(const CGePoint2d& origin, const CGeVector2d& xAxis, const CGeVector2d& yAxis)
{
	return *this;
}

CGeMatrix2d&
CGeMatrix2d::setToTranslation(const CGeVector2d& vector)
{
	return *this;
}

CGeMatrix2d&
CGeMatrix2d::setToRotation(double angle, const CGePoint2d& center /* = CGePoint2d::s_origin*/)
{
	return *this;
}

CGeMatrix2d&
CGeMatrix2d::setToScaling(double scale, const CGePoint2d& center /* = CGePoint2d::s_origin*/)
{
	return *this;
}

CGeMatrix2d&
CGeMatrix2d::setToMirroring(const CGePoint2d& point)
{
	return *this;
}

CGeMatrix2d&
CGeMatrix2d::setToMirroring(const CGeLine2d& line)
{
	return *this;
}

CGeMatrix2d&
CGeMatrix2d::setToAlignCoordSys(const CGePoint2d& fromOrigin, const CGeVector2d& fromXAxis, const CGeVector2d& fromYAxis, const CGePoint2d& toOrigin, const CGeVector2d& toXAxis, const CGeVector2d& toYAxis)
{
	return *this;
}

double
CGeMatrix2d::operator ()(unsigned int i, unsigned int j) const
{
	return m_entry[i][j];
}

double&
CGeMatrix2d::operator ()(unsigned int i, unsigned int j)
{
	return m_entry[i][j];
}

CGeMatrix2d
CGeMatrix2d::translation(const CGeVector2d& vector)
{
	return CGeMatrix2d();
}

CGeMatrix2d
CGeMatrix2d::rotation(double angle, const CGePoint2d& center /* = CGePoint2d::s_origin*/)
{
	return CGeMatrix2d();
}

CGeMatrix2d
CGeMatrix2d::scaling(double scale, const CGePoint2d& center /* = CGePoint2d::s_origin*/)
{
	return CGeMatrix2d();
}

CGeMatrix2d
CGeMatrix2d::mirroring(const CGePoint2d& point)
{
	return CGeMatrix2d();
}

CGeMatrix2d
CGeMatrix2d::mirroring(const CGeLine2d& line)
{
	return CGeMatrix2d();
}

CGeMatrix2d
CGeMatrix2d::alignCoordSys(const CGePoint2d& fromOrigin, const CGeVector2d& fromXAxis, const CGeVector2d& fromYAxis, const CGePoint2d& toOrigin, const CGeVector2d& toXAxis, const CGeVector2d& toYAxis)
{
	return CGeMatrix2d();
}
