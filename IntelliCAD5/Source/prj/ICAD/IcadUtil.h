#pragma once
#ifndef _ICADUTIL_H
#define _ICADUTIL_H

#include <math.h>

const double IcadFuzzyDistance = 1.0e-11;
const double IcadFuzzyDistance2 = 1.0e-7;
const double IcadFuzzyAngle = 1.0E-10;
const double IcadTwoPi = 6.28318530717958647692;

#pragma warning (disable : 4723)

inline bool icadPointEqual(const double p1[3], const double p2[3], double tol= IcadFuzzyDistance)
{
	if (fabs(p2[0])<=tol) {
		if (fabs(p1[0])>tol)
			return false;
	}
	else if (fabs((p1[0]/p2[0])-1.0) >= tol)
		return false;

	if (fabs(p2[1])<=tol) {
		if (fabs(p1[1])>tol)
			return false;
	}
	else if (fabs((p1[1]/p2[1])-1.0) >= tol)
		return false;

	if (fabs(p2[2])<=tol) {
		if (fabs(p1[2])>tol)
			return false;
	}
	else if (fabs((p1[2]/p2[2])-1.0) >= tol)
		return false;

	return true;
}

inline bool icadRealEqual(double r1, double r2, double tol = IcadFuzzyDistance)
{
	if (fabs(r2)<=tol) {
		if (fabs(r1)>tol)
			return false;
	}
	else {
		if (fabs((r1/r2)-1.0) > tol)
			return false;
	}
	return true;
}

inline bool icadIsZero(double value, double tol = IcadFuzzyDistance)
{
	return (value <= tol && value >= -tol);
}

inline bool icadAngleEqual(double angle1, double angle2, double tol = IcadFuzzyAngle)
{
	int n;
	double angle;
	if ((angle = fabs(angle1)) >= IcadTwoPi) {
		n = (int)(angle / IcadTwoPi);
		angle -= (double)n * IcadTwoPi;
	}
	angle1 = (angle1 >= 0.0) ? angle : IcadTwoPi - angle;

	if ((angle = fabs(angle2)) >= IcadTwoPi) {
		n = (int)(angle / IcadTwoPi);
		angle -= (double)n * IcadTwoPi;
	}
	angle2 = (angle2 >= 0.0) ? angle : IcadTwoPi - angle;

	return icadRealEqual(angle1, angle2, tol);
}


//assumes all 4 pts are colinear.
//
inline bool icadDirectionEqual(const double p1[3], const double p2[3], 
							   const double p3[3], const double p4[3])
{
	//returns true if vector p1-p2 is same direction as vector p3-p4, false otherwise
	int i;

	if (fabs(p2[0] - p1[0]) > fabs(p2[1] - p1[1])) {
		if (fabs(p2[0] - p1[0]) < fabs(p2[2] - p1[2])) i = 2;
		else i=0;
	} 
	else if (fabs(p2[1] - p1[1]) < fabs(p2[2] - p1[2])) i = 2;
	else i = 1;

	if (((p2[i] - p1[i]) * (p4[i] - p3[i])) > 0.0) return true;
	else return false;
}


#endif // _ICADUTIL_H