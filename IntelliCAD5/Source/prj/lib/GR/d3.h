#ifndef __D3H__
#define __D3H__

#include <limits>
/*----------------------------------------------------------------------------------------*//**
@author Denis Petrov

@description Compares value with zero within given tolerance.
*//*---------------------------------------------------------------------------------------*/
template<class _T> inline bool isEqualZeroWithin(const _T& value, const _T& precision = std::numeric_limits<_T>::epsilon())
{
	return -precision < value && value < precision;
}

class GR_CLASS CD3
{
public:
	/* CONSTRUCTION */
	CD3() {}
	CD3(const CD3& _v): x(_v.x), y(_v.y), z(_v.z) {}
	CD3(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {}
	explicit CD3(const double* _v) : x(_v[0]), y(_v[1]), z(_v[2]) {}
	explicit CD3(double _t) : x(_t), y(_t), z(_t) {}
	
public: 
	/* OPERATORS */
	CD3& operator =(const double* _v) { x=_v[0]; y=_v[1]; z=_v[2]; return *this; }
	
	operator double*() { return v; }
	operator const double*() const { return v; }
	double& operator[](int i) { return v[i]; }
	double operator[](int i) const { return v[i]; }
	
	bool operator> (const CD3& _v) const { return x> _v.x && y> _v.y && z> _v.z; }
	bool operator< (const CD3& _v) const { return x< _v.x && y< _v.y && z< _v.z; }
	bool operator>=(const CD3& _v) const { return x>=_v.x && y>=_v.y && z>=_v.z; }
	bool operator<=(const CD3& _v) const { return x<=_v.x && y<=_v.y && z<=_v.z; }
	bool operator==(const CD3& _v) const { return x==_v.x && y==_v.y && z==_v.z; }
	bool operator!=(const CD3& _v) const { return x!=_v.x || y!=_v.y || z!=_v.z; }
	
	CD3  operator+ (const CD3& _v) const { return CD3(x+_v.x, y+_v.y, z+_v.z); }
	CD3  operator- (const CD3& _v) const { return CD3(x-_v.x, y-_v.y, z-_v.z); }
	CD3& operator+=(const CD3& _v) { x+=_v.x; y+=_v.y; z+=_v.z; return *this; }
	CD3& operator-=(const CD3& _v) { x-=_v.x; y-=_v.y; z-=_v.z; return *this; }
	
	CD3  operator* (double _t) const { return CD3(x*_t, y*_t, z*_t); }
	CD3  operator/ (double _t) const { assert(!icadRealEqual(_t,0.0)); return CD3(x/_t, y/_t, z/_t); }
	CD3& operator*=(double _t) { x*=_t; y*=_t; z*=_t; return *this; }
	CD3& operator/=(double _t) { assert(!icadRealEqual(_t,0.0)); x/=_t; y/=_t; z/=_t; return *this; }
	
	double operator*(const CD3& _v) const { return x*_v.x + y*_v.y + z*_v.z; }
	CD3  operator%(const CD3& _v) const { return CD3(y*_v.z - z*_v.y, -x*_v.z + _v.x*z, x*_v.y - _v.x*y); }
	
public: 
	/* OPERATIONS */
    void setValue(double _x, double _y, double _z) { x=_x; y=_y; z=_z; }
    void setValue(const double* _v) { v[0]=_v[0]; v[1]=_v[1]; v[2]=_v[2]; }
    void getValue(double& _x, double& _y, double& _z) const { _x=x; _y=y; _z=z; }
    void getValue(double* _v) const { _v[0]=v[0]; _v[1]=v[1]; _v[2]=v[2]; }

	double norm2() const { return x*x + y*y + z*z; }
	double norm() const { return sqrt(x*x + y*y + z*z); }
    void normalize() { double n=norm(); assert(!icadRealEqual(n,0.0)); x/=n; y/=n; z/=n; }

	void nullify() { v[0]=0.0; v[1]=0.0; v[2]=0.0; }
	bool isNull() const { return icadRealEqual(v[0],0.0) && icadRealEqual(v[1],0.0) && icadRealEqual(v[2],0.0); }
	bool isNullWithin(const double& precision = std::numeric_limits<double>::epsilon()) 
	{ return isEqualZeroWithin(v[0], precision) && isEqualZeroWithin(v[1], precision) && isEqualZeroWithin(v[2], precision); }

public:
	/* DATA */
	union
	{
		struct 
		{
			double x, y, z;
		};
		double v[3];
	};
};

#endif
