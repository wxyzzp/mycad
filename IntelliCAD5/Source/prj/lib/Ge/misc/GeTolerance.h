// File: GeTolerance.h
//
// CGeTolerance
//


#pragma once


#include "GePch.h"


class GE_API CGeTolerance
{
public:
	CGeTolerance();

	double	equalPoint		() const;
	double	equalVector		() const;
	void	setEqualPoint	(double newVal);
	void	setEqualVector	(double newVal);

/* original code:
private:
	double				mTolArr[5];
	int                 mAbs;

	friend class AcGeTolA;*/
};
