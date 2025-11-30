// File GeScale2d.h
//
// CGeScale2d
//

#pragma once


#include "GeContext.h"


class GE_API CGeMatrix2d;
class GE_API CGeScale3d;


class GE_API CGeScale2d
{
public:
	CGeScale2d();
	CGeScale2d(const CGeScale2d& source);
	CGeScale2d(double factor);
	CGeScale2d(double sx, double sy);

	CGeScale2d		operator *			(const CGeScale2d& scale) const;
	CGeScale2d&		operator *=			(const CGeScale2d& scale);
	CGeScale2d&		preMultBy			(const CGeScale2d& leftScale);
	CGeScale2d&		postMultBy			(const CGeScale2d& rightScale);
	CGeScale2d&		setToProduct		(const CGeScale2d& scale1, const CGeScale2d& scale2);
	CGeScale2d		operator *			(double factor) const;
	CGeScale2d&		operator *=			(double factor);
	CGeScale2d&		setToProduct		(const CGeScale2d& scale, double factor);
	CGeScale2d		inverse				() const;
	CGeScale2d&		invert				();
	bool			isProportional		(const CGeTolerance& tol = CGeContext::s_defTol) const;
	bool			operator ==			(const CGeScale2d& scale) const;
	bool			operator !=			(const CGeScale2d& scale) const;
	bool			isEqualTo			(const CGeScale2d& scale, const CGeTolerance& tol = CGeContext::s_defTol) const;
	double			operator []			(unsigned int i) const;
	double&			operator []			(unsigned int i);
	CGeScale2d&		set					(double sx, double sy);
					operator CGeMatrix2d() const;
	void			getMatrix			(CGeMatrix2d& matrix) const;
	CGeScale2d&		extractScale		(const CGeMatrix2d& matrix);
	CGeScale2d&		removeScale			(CGeMatrix2d& matrix);
					operator CGeScale3d	() const;

	friend GE_API CGeScale2d operator * (double factor, const CGeScale2d& scale);

	static const CGeScale2d s_identity;

	union
	{
		struct	tagStruct
		{
			double	m_sx, m_sy;
		};
		double	m_scales[2];
	};
};
