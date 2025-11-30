// File GeMatrix2d.h
//
// CGeMatrix2d
//

#pragma once


#include "GeContext.h"
#include "GePoint2d.h"


class GE_API CGeLine2d;
class GE_API CGeVector2d;


class GE_API CGeMatrix2d
{
public:
	CGeMatrix2d();
	CGeMatrix2d(const CGeMatrix2d& source);

	CGeMatrix2d&		setToIdentity		();
	CGeMatrix2d			operator *			(const CGeMatrix2d& matrix) const;
	CGeMatrix2d&		operator *=			(const CGeMatrix2d& matrix);
	CGeMatrix2d&		preMultBy			(const CGeMatrix2d& leftMatrix);
	CGeMatrix2d&		postMultBy			(const CGeMatrix2d& rightMatrix);
	CGeMatrix2d&		setToProduct		(const CGeMatrix2d& matrix1, const CGeMatrix2d& matrix2);
	CGeMatrix2d&		invert				();
	CGeMatrix2d			inverse				() const;
	bool				isSingular			(const CGeTolerance& tol = CGeContext::s_defTol) const;
	CGeMatrix2d&		transposeIt			();
	CGeMatrix2d			transpose			() const;
	bool				operator ==			(const CGeMatrix2d& matrix) const;
	bool				operator !=			(const CGeMatrix2d& matrix) const;
	bool				isEqualTo			(const CGeMatrix2d& matrix, const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool				isUniScaledOrtho	(const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool				isScaledOrtho		(const CGeTolerance& tol = CGeContext::s_defTol) const;
	double				scale				() const;
	double				det					() const;
	CGeMatrix2d&		setTranslation		(const CGeVector2d& vector);
	CGeVector2d			translation			() const;
    bool				isConformal			(double& scale, double& angle, bool& bIsMirror, CGeVector2d& reflex) const;
	void				getCoordSystem		(CGePoint2d& origin, CGeVector2d& xAxis, CGeVector2d& yAxis) const;
	CGeMatrix2d&		setCoordSystem		(const CGePoint2d& origin, const CGeVector2d& xAxis, const CGeVector2d& yAxis);
	CGeMatrix2d&		setToTranslation	(const CGeVector2d& vector);
	CGeMatrix2d&		setToRotation		(double angle, const CGePoint2d& center = CGePoint2d::s_origin);
	CGeMatrix2d&		setToScaling		(double scale, const CGePoint2d& center = CGePoint2d::s_origin);
	CGeMatrix2d&		setToMirroring		(const CGePoint2d& point);
	CGeMatrix2d&		setToMirroring		(const CGeLine2d& line);
	CGeMatrix2d&		setToAlignCoordSys	(const CGePoint2d& fromOrigin, const CGeVector2d& fromXAxis, const CGeVector2d& fromYAxis, const CGePoint2d& toOrigin, const CGeVector2d& toXAxis, const CGeVector2d& toYAxis);
	double				operator ()			(unsigned int i, unsigned int j) const;
	double&				operator ()			(unsigned int i, unsigned int j);

	static CGeMatrix2d	translation		(const CGeVector2d& vector);
	static CGeMatrix2d	rotation		(double angle, const CGePoint2d& center = CGePoint2d::s_origin);
	static CGeMatrix2d	scaling			(double scale, const CGePoint2d& center = CGePoint2d::s_origin);
	static CGeMatrix2d	mirroring		(const CGePoint2d& point);
	static CGeMatrix2d	mirroring		(const CGeLine2d& line);
	static CGeMatrix2d	alignCoordSys	(const CGePoint2d& fromOrigin, const CGeVector2d& fromXAxis, const CGeVector2d& fromYAxis, const CGePoint2d& toOrigin, const CGeVector2d& toXAxis, const CGeVector2d& toYAxis);

	static const CGeMatrix2d s_identity;

	union
	{
		double	m_v[9];
		double	m_m[3][3];
		struct
		{
			double	m_a00, m_a01, m_a02,
					m_a10, m_a11, m_a12,
					m_a20, m_a21, m_a22;
		};
	};
};
