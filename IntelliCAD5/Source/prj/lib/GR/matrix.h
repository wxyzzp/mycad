#ifndef __matrixH__
#define __matrixH__

#include <memory.h>
#include "d3.h"

/*----------------------------------------------------------------------------------------*//**
@author Denis Petrov

@description Class for matrices operation.
*//*---------------------------------------------------------------------------------------*/
class GR_CLASS CMatrix4
{
public:
	CMatrix4() { memset(m_v, 0, 16 * sizeof(double)); }
	CMatrix4(const double* pV) { memcpy(m_v, pV, 16 * sizeof(double)); }
	CMatrix4(const CMatrix4& m) { memcpy(m_v, m.m_v, 16 * sizeof(double)); }

	CMatrix4& operator=(const double* pV) { memcpy(m_v, pV, 16 * sizeof(double)); return *this; }
	CMatrix4& operator=(const CMatrix4& m) { memcpy(m_v, m.m_v, 16 * sizeof(double)); return *this; }

	void setValue(const CD3& rotateFrom, const CD3& rotateTo);
	void setValue(const double& angle, const CD3& axis);

	void setValue(const double* pColumn1, const double* pColumn2, const double* pColumn3, const double* pColumn4);
	void getValue(double* pColumn1, double* pColumn2, double* pColumn3, double* pColumn4);

	double operator()(int i, int j) const { return m_m[i][j]; }
	double& operator()(int i, int j) { return m_m[i][j]; }

	CMatrix4 operator*(const CMatrix4& m);
	void multiplyRight(double* pDestination, const double* pSource) const;
	void multiplyRightUsing33(double* pDestination, const double* pSource) const;

	CMatrix4 invert() const;
	void makeIdentity();
	const static double* identity() { return &s_identityMatrix[0][0]; }
	bool isEqualWithin(const CMatrix4& m, const double& precision = std::numeric_limits<double>::epsilon()) const;

	static int getRotation(double& angle, CD3& axis, const CD3& rotateFrom, const CD3& rotateTo);
protected:
	union
	{
		double m_v[16];
		double m_m[4][4];
		struct
		{
			double m_a00, m_a01, m_a02, m_a03,
				m_a10, m_a11, m_a12, m_a13,
				m_a20, m_a21, m_a22, m_a23,
				m_a30, m_a31, m_a32, m_a33;
		};
	};

	static double s_identityMatrix[4][4];
};

inline void CMatrix4::setValue(const CD3& rotateFrom, const CD3& rotateTo)
{
	double angle;
	CD3 axis;
	getRotation(angle, axis, rotateFrom, rotateTo);
	setValue(angle, axis);
}

#endif
