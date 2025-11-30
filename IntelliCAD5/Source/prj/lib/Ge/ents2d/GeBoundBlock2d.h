// File: GeBoundBlock2d.h
//
// CGeBoundBlock2d
//

#pragma once


#include "GeEntity2d.h"


class GE_API CGeBoundBlock2d : public CGeEntity2d
{
public:
	CGeBoundBlock2d();
	CGeBoundBlock2d(const CGeBoundBlock2d& source);
	CGeBoundBlock2d(const CGePoint2d& point1, const CGePoint2d& point2);
	CGeBoundBlock2d(const CGePoint2d& base, const CGeVector2d& dir1, const CGeVector2d& dir2);

	void				getMinMaxPoints	(CGePoint2d& point1, CGePoint2d& point2) const;
	void				get				(const CGePoint2d& base, const CGeVector2d& dir1, const CGeVector2d& dir2) const;
	CGeBoundBlock2d&	set				(const CGePoint2d& point1, const CGePoint2d& point2);
	CGeBoundBlock2d&	set				(const CGePoint2d& base, const CGeVector2d& dir1, const CGeVector2d& dir2);
	CGeBoundBlock2d&	extend			(const CGePoint2d& point);
	CGeBoundBlock2d&	swell			(double distance);
	bool				contains		(const CGePoint2d& point) const;
	bool				isDisjoint		(const CGeBoundBlock2d& block) const;
	bool				isBox			() const;
	CGeBoundBlock2d&	setToBox		(bool b);
	CGeBoundBlock2d&	operator =		(const CGeBoundBlock2d& source);
};
