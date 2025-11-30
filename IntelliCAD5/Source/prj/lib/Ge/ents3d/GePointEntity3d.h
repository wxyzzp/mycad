// File: GePointEntity3d.h
//
// CGePointEntity3d
//

#pragma once


#include "GeEntity3d.h"


class GE_API CGePointEntity3d : public CGeEntity3d
{
public:
	virtual ~CGePointEntity3d();

	CGePoint3d			point3d				() const;
						operator CGePoint3d	() const;
	CGePointEntity3d&	operator =			(const CGePointEntity3d& source);

protected:
	CGePointEntity3d();
	CGePointEntity3d(const CGePointEntity3d& source);

	CGePoint3d	m_point;
};
