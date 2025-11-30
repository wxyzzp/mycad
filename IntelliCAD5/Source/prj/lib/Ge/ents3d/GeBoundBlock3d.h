// File: GeBoundBlock3d.h
//
// CGeBoundBlock3d
//

#pragma once


#include "GeEntity3d.h"


class GE_API CGeBoundBlock3d : public CGeEntity3d
{
public:
	CGeBoundBlock3d();
	CGeBoundBlock3d(const CGePoint3d& point1, const CGePoint3d& point2);
	CGeBoundBlock(const CGePoint3d& base, const CGeVector3d& dir1, const CGeVector3d& dir2, const CGeVector3d& dir3);
	CGeBoundBlock3d(const CGeBoundBlock3d& source);

	void				getMinMaxPoints	(CGePoint3d& point1, CGePoint3d& point2) const;
	void				get				(const CGePoint3d& base, const CGeVector3d& dir1, const CGeVector3d& dir2, const CGeVector3d& dir3) const;
	CGeBoundBlock3d&	set				(const CGePoint3d& point1, const CGePoint3d& point2);
	CGeBoundBlock3d&	set				(const CGePoint3d& base, const CGeVector3d& dir1, const CGeVector3d& dir2, const CGeVector3d& dir3);
	CGeBoundBlock3d&	extend			(const CGePoint3d& point);
	CGeBoundBlock3d&	swell			(double distance);
	bool				contains		(const CGePoint3d& point) const;
	bool				isDisjoint		(const CGeBoundBlock3d& block) const;
	bool				isBox			() const;
	CGeBoundBlock3d&	setToBox		(bool b);
	CGeBoundBlock3d&	operator =		(const CGeBoundBlock3d& source);
};
