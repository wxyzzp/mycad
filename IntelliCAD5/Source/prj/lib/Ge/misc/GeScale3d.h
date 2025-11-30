// File GeScale3d.h
//
// CGeScale3d
//

#pragma once


#include "GeContext.h"


class GE_API CGeMatrix3d;


class GE_API CGeScale3d
{
public:
	CGeScale3d();
	CGeScale3d(const CGeScale3d& source);
	CGeScale3d(double factor);
	CGeScale3d(double sx, double sy, double sz);

	CGeScale3d		operator *			(const CGeScale3d& scale) const;
	CGeScale3d&		operator *=			(const CGeScale3d& scale);
	CGeScale3d&		preMultBy			(const CGeScale3d& leftScale);
	CGeScale3d&		postMultBy			(const CGeScale3d& rightScale);
	CGeScale3d&		setToProduct		(const CGeScale3d& scale1, const CGeScale3d& scale2);
	CGeScale3d		operator *			(double factor) const;
	CGeScale3d&		operator *=			(double factor);
	CGeScale3d&		setToProduct		(const CGeScale3d& scale, double factor);
	CGeScale3d		inverse				() const;
	CGeScale3d&		invert				();
	bool			isProportional		(const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool			operator ==			(const CGeScale3d& scale) const;
	bool			operator !=			(const CGeScale3d& scale) const;
	bool			isEqualTo			(const CGeScale3d& scale, const CGeTolerance& tol = CGeContext::s_defTol) const;
	double			operator []			(unsigned int i) const;
	double&			operator []			(unsigned int i);
	CGeScale3d&		set					(double sx, double sy, double sz);
					operator CGeMatrix3d() const;
	void			getMatrix			(CGeMatrix3d& matrix) const;
	CGeScale3d&		extractScale		(const CGeMatrix3d& matrix);
	CGeScale3d&		removeScale			(CGeMatrix3d& matrix);

	friend GE_API CGeScale3d operator * (double factor, const CGeScale3d& scale);

	static const CGeScale3d s_identity;

	union
	{
		struct	tagStruct
		{
			double	m_sx, m_sy, m_sz;
		};
		double	m_scales[3];
	};
};
