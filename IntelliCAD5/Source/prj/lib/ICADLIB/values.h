// File  :
// Author: Alexey Malov
#pragma once
#ifndef _VALUES_H_
#define _VALUES_H_

namespace icl
{
    // precision limits
    // these values are yet used just in geodata, dbcstr namespaces

    // resabs - values are used to limit dimesional quantities (distance, ...)
    inline
    double resabs() //ordinal dimensionality
    {
        return 1.e-6;
    }
    inline
    double resabs2()//squared dimensionality
    {
        return 1.e-12;
    }
    // resnorm - value is used to limit non-dimensional quantities (angles, ...)
    inline
    double resnorm()
    {
        return 1.e-10;
    }

    // pi value
    inline
    double pi()
    {
        return 3.1415926535897932384626433832795;
    }
    // pi2 value
    inline
    double pi2()
    {
        return 2*3.1415926535897932384626433832795;
    }

	//comparision functions for doubles
	//greater than ?
	inline
	bool gt(const double& v1, const double& v2, const double& p = resabs())
	{
		return v1 - v2 > p;
	}
	//less than ?
	inline
	bool lt(const double& v1, const double& v2, const double& p = resabs())
	{
		return v1 - v2 < -p;
	}
	//equal ?
	inline
	bool eq(const double& v1, const double& v2, const double& p = resabs())
	{
		double  d = v1 - v2;
		return d < p && d > -p;
	}
	//greater or equal
	inline
	bool ge(const double& v1, const double& v2, const double& p = resabs())
	{
		return v1 - v2 > -p;
	}
	//less or equal
	inline
	bool le(const double& v1, const double& v2, const double& p = resabs())
	{
		return v1 - v2 < p;
	}
}

#endif
