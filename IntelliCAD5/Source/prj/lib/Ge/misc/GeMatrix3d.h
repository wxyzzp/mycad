// File GeMatrix3d.h
//
// CGeMatrix3d
//

#pragma once


#include "GeContext.h"
#include "GePoint3d.h"


class GE_API CGeLine3d;
class GE_API CGeVector3d;
class GE_API CGeMatrix2d;


class GE_API CGeMatrix3d
{
public:
	CGeMatrix3d();
	CGeMatrix3d(const CGeMatrix3d& source);

	CGeMatrix3d&		setToIdentity		();
	CGeMatrix3d			operator *			(const CGeMatrix3d& matrix) const;
	CGeMatrix3d&		operator *=			(const CGeMatrix3d& matrix);
	CGeMatrix3d&		preMultBy			(const CGeMatrix3d& leftMatrix);
	CGeMatrix3d&		postMultBy			(const CGeMatrix3d& rightMatrix);
	CGeMatrix3d&		setToProduct		(const CGeMatrix3d& matrix1, const CGeMatrix3d& matrix2);
	CGeMatrix3d&		invert				();
	CGeMatrix3d			inverse				() const;
	bool				isSingular			(const CGeTolerance& tol = CGeContext::s_defTol) const;
	CGeMatrix3d&		transposeIt			();
	CGeMatrix3d			transpose			() const;
	bool				operator ==			(const CGeMatrix3d& matrix) const;
	bool				operator !=			(const CGeMatrix3d& matrix) const;
	bool				isEqualTo			(const CGeMatrix3d& matrix, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool				isUniScaledOrtho	(const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool				isScaledOrtho		(const CGeTolerance& tol = CGeContext::s_defTol) const;
	double				det					() const;
	CGeMatrix3d&		setTranslation		(const CGeVector3d& vector);
	CGeVector3d			translation			() const;
	CGeMatrix3d&		setCoordSystem		(const CGePoint3d& origin, const CGeVector3d& xAxis, const CGeVector3d& yAxis, const CGeVector3d& zAxis);
	void				getCoordSystem		(CGePoint3d& origin, CGeVector3d& xAxis, CGeVector3d& yAxis, CGeVector3d& zAxis) const;
	CGeMatrix3d&		setToTranslation	(const CGeVector3d& vector);
	CGeMatrix3d&		setToRotation		(double angle, const CGeVector3d& axis, const CGePoint3d& center = CGePoint3d::s_origin);
	CGeMatrix3d&		setToScaling		(double scale, const CGePoint3d& center = CGePoint3d::s_origin);
	CGeMatrix3d&		setToMirroring		(const CGePoint3d& point);
	CGeMatrix3d&		setToMirroring		(const CGeLine3d& line);
	CGeMatrix3d&		setToMirroring		(const CGePlane& plane);
	CGeMatrix3d&		setToProjection		(const CGePlane& projPlane, const CGeVector3d& projDir);
	CGeMatrix3d&		setToAlignCoordSys	(const CGePoint3d& fromOrigin, const CGeVector3d& fromXAxis, const CGeVector3d& fromYAxis, const CGeVector3d& fromZAxis, const CGePoint3d& toOrigin, const CGeVector3d& toXAxis, const CGeVector3d& toYAxis, const CGeVector3d& toZAxis);
	CGeMatrix3d&		setToWorldToPlane	(const CGeVector3d& normal);
	CGeMatrix3d&		setToWorldToPlane	(const CGePlane& plane);
	CGeMatrix3d&		setToPlaneToWorld	(const CGeVector3d& normal);
	CGeMatrix3d&		setToPlaneToWorld	(const CGePlane& plane);
	double				scale				() const;
	double				norm				() const;
	CGeMatrix2d			convertToLocal		(CGeVector3d& normal, double& elev) const;
	double				operator ()			(unsigned int i, unsigned int j) const;
	double&				operator ()			(unsigned int i, unsigned int j);
	bool				inverse				(CGeMatrix3d& inversionMat, double tol) const;

	static CGeMatrix3d	translation		(const CGeVector3d& vector);
	static CGeMatrix3d	rotation		(double angle, const CGeVector3d& axis, const CGePoint3d& center = CGePoint3d::s_origin);
	static CGeMatrix3d	scaling			(double scale, const CGePoint3d& center = CGePoint3d::s_origin);
	static CGeMatrix3d	mirroring		(const CGePoint3d& point);
	static CGeMatrix3d	mirroring		(const CGeLine3d& line);
	static CGeMatrix3d	mirroring		(const CGePlane& plane);
	static CGeMatrix3d	projection		(const CGePlane& projPlane, const CGeVector3d& projDir);
	static CGeMatrix3d	alignCoordSys	(const CGePoint3d& fromOrigin, const CGeVector3d& fromXAxis, const CGeVector3d& fromYAxis, const CGeVector3d& fromZAxis, const CGePoint3d& toOrigin, const CGeVector3d& toXAxis, const CGeVector3d& toYAxis, const CGeVector3d& toZAxis);
	static CGeMatrix3d	worldToPlane	(const CGeVector3d& normal);
	static CGeMatrix3d	worldToPlane	(const CGePlane& plane);
	static CGeMatrix3d	planeToWorld	(const CGeVector3d& normal);
	static CGeMatrix3d	planeToWorld	(const CGePlane& plane);

	static const CGeMatrix3d s_identity;

	union
	{
		double	m_v[16];
		double	m_m[4][4];
		struct
		{
			double	m_a00, m_a01, m_a02, m_a03,
					m_a10, m_a11, m_a12, m_a13,
					m_a20, m_a21, m_a22, m_a23,
					m_a30, m_a31, m_a32, m_a33;
		};
	};

/* original code:
private:
	void           pivot           (int, AcGeMatrix3d&);
	int            pivotIndex(int) const;
	void           swapRows        (int, int, AcGeMatrix3d&);*/
};
