// File: GeMatrix3d.cpp
//

#include "GeMatrix3d.h"
#include "GeVector3d.h"
#include "GeMatrix2d.h"

//////////////////////////////////////////////////
// CGeMatrix3d class implementation
//////////////////////////////////////////////////

const CGeMatrix3d CGeMatrix3d::s_identity;


CGeMatrix3d::CGeMatrix3d()
{
}

CGeMatrix3d::CGeMatrix3d(const CGeMatrix3d& source)
{
}

CGeMatrix3d&
CGeMatrix3d::setToIdentity()
{
	return *this;
}

CGeMatrix3d
CGeMatrix3d::operator * (const CGeMatrix3d& matrix) const
{
	return CGeMatrix3d();
}

CGeMatrix3d&
CGeMatrix3d::operator *= (const CGeMatrix3d& matrix)
{
	return *this;
}

CGeMatrix3d&
CGeMatrix3d::preMultBy(const CGeMatrix3d& leftMatrix)
{
	return *this;
}

CGeMatrix3d&
CGeMatrix3d::postMultBy(const CGeMatrix3d& rightMatrix)
{
	return *this;
}

CGeMatrix3d&
CGeMatrix3d::setToProduct(const CGeMatrix3d& matrix1, const CGeMatrix3d& matrix2)
{
	return *this;
}

CGeMatrix3d&
CGeMatrix3d::invert()
{
	return *this;
}

CGeMatrix3d
CGeMatrix3d::inverse() const
{
	return CGeMatrix3d();
}

bool
CGeMatrix3d::isSingular(const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

CGeMatrix3d&
CGeMatrix3d::transposeIt()
{
	return *this;
}

CGeMatrix3d
CGeMatrix3d::transpose() const
{
	return CGeMatrix3d();
}

bool
CGeMatrix3d::isEqualTo(const CGeMatrix3d& matrix, const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeMatrix3d::isUniScaledOrtho(const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

bool
CGeMatrix3d::isScaledOrtho(const CGeTolerance& tol /* = CGeContext::s_defTol*/) const
{
	return false;
}

double
CGeMatrix3d::det() const
{
	return 0.0;
}

CGeMatrix3d&
CGeMatrix3d::setTranslation(const CGeVector3d& vector)
{
	return *this;
}

CGeVector3d
CGeMatrix3d::translation() const
{
	return CGeVector3d();
}

CGeMatrix3d&
CGeMatrix3d::setCoordSystem(const CGePoint3d& origin, const CGeVector3d& xAxis, const CGeVector3d& yAxis, const CGeVector3d& zAxis)
{
	return *this;
}

void
CGeMatrix3d::getCoordSystem(CGePoint3d& origin, CGeVector3d& xAxis, CGeVector3d& yAxis, CGeVector3d& zAxis) const
{
}

CGeMatrix3d&
CGeMatrix3d::setToTranslation(const CGeVector3d& vector)
{
	return *this;
}

CGeMatrix3d&
CGeMatrix3d::setToRotation(double angle, const CGeVector3d& axis, const CGePoint3d& center /* = CGePoint3d::s_origin*/)
{
	return *this;
}

CGeMatrix3d&
CGeMatrix3d::setToScaling(double scale, const CGePoint3d& center /* = CGePoint3d::s_origin*/)
{
	return *this;
}

CGeMatrix3d&
CGeMatrix3d::setToMirroring(const CGePoint3d& point)
{
	return *this;
}

CGeMatrix3d&
CGeMatrix3d::setToMirroring(const CGeLine3d& line)
{
	return *this;
}

CGeMatrix3d&
CGeMatrix3d::setToMirroring(const CGePlane& plane)
{
	return *this;
}

CGeMatrix3d&
CGeMatrix3d::setToProjection(const CGePlane& projPlane, const CGeVector3d& projDir)
{
	return *this;
}

CGeMatrix3d&
CGeMatrix3d::setToAlignCoordSys(const CGePoint3d& fromOrigin, const CGeVector3d& fromXAxis, const CGeVector3d& fromYAxis, const CGeVector3d& fromZAxis, const CGePoint3d& toOrigin, const CGeVector3d& toXAxis, const CGeVector3d& toYAxis, const CGeVector3d& toZAxis)
{
	return *this;
}

CGeMatrix3d&
CGeMatrix3d::setToWorldToPlane(const CGeVector3d& normal)
{
	return *this;
}

CGeMatrix3d&
CGeMatrix3d::setToWorldToPlane(const CGePlane& plane)
{
	return *this;
}

CGeMatrix3d&
CGeMatrix3d::setToPlaneToWorld(const CGeVector3d& normal)
{
	return *this;
}

CGeMatrix3d&
CGeMatrix3d::setToPlaneToWorld(const CGePlane& plane)
{
	return *this;
}

double
CGeMatrix3d::scale() const
{
	return 0.0;
}

double
CGeMatrix3d::norm() const
{
	return 0.0;
}

CGeMatrix2d
CGeMatrix3d::convertToLocal(CGeVector3d& normal, double& elev) const
{
	return CGeMatrix2d();
}

double
CGeMatrix3d::operator ()(unsigned int i, unsigned int j) const
{
	return m_entry[i][j];
}

double&
CGeMatrix3d::operator ()(unsigned int i, unsigned int j)
{
	return m_entry[i][j];
}

bool
CGeMatrix3d::inverse(CGeMatrix3d& inversionMat, double tol) const
{
	return false;
}

CGeMatrix3d
CGeMatrix3d::translation(const CGeVector3d& vector)
{
	return CGeMatrix3d();
}

CGeMatrix3d
CGeMatrix3d::rotation(double angle, const CGeVector3d& axis, const CGePoint3d& center /* = CGePoint3d::s_origin*/)
{
	return CGeMatrix3d();
}

CGeMatrix3d
CGeMatrix3d::scaling(double scale, const CGePoint3d& center /* = CGePoint3d::s_origin*/)
{
	return CGeMatrix3d();
}

CGeMatrix3d
CGeMatrix3d::mirroring(const CGePoint3d& point)
{
	return CGeMatrix3d();
}

CGeMatrix3d
CGeMatrix3d::mirroring(const CGeLine3d& line)
{
	return CGeMatrix3d();
}

CGeMatrix3d
CGeMatrix3d::mirroring(const CGePlane& plane)
{
	return CGeMatrix3d();
}

CGeMatrix3d
CGeMatrix3d::projection(const CGePlane& projPlane, const CGeVector3d& projDir)
{
	return CGeMatrix3d();
}

CGeMatrix3d
CGeMatrix3d::alignCoordSys(const CGePoint3d& fromOrigin, const CGeVector3d& fromXAxis, const CGeVector3d& fromYAxis, const CGeVector3d& fromZAxis, const CGePoint3d& toOrigin, const CGeVector3d& toXAxis, const CGeVector3d& toYAxis, const CGeVector3d& toZAxis)
{
	return CGeMatrix3d();
}

CGeMatrix3d
CGeMatrix3d::worldToPlane(const CGeVector3d& normal)
{
	return CGeMatrix3d();
}

CGeMatrix3d
CGeMatrix3d::worldToPlane(const CGePlane& plane)
{
	return CGeMatrix3d();
}

CGeMatrix3d
CGeMatrix3d::planeToWorld(const CGeVector3d& normal)
{
	return CGeMatrix3d();
}

CGeMatrix3d
CGeMatrix3d::planeToWorld(const CGePlane& plane)
{
	return CGeMatrix3d();
}
