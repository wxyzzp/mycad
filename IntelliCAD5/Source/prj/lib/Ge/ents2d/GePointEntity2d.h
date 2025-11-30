// File: GePointEntity2d.h
//
// CGePointEntity2d
//

#pragma once


#include "GeEntity2d.h"


class GE_API CGePointEntity2d : public CGeEntity2d
{
public:
	virtual ~CGePointEntity2d();

	CGePoint2d			point2d				() const;
						operator CGePoint2d	() const;
	CGePointEntity2d&	operator =			(const CGePointEntity2d& source);

protected:
	CGePointEntity2d();
	CGePointEntity2d(const CGePointEntity2d& source);

	CGePoint2d	m_point;
};
