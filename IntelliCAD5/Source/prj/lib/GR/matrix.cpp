#include "gr.h"
#include "matrix.h"
#include <float.h>
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

bool solveLES(int n, double* pM, int c, double* pMS);

double CMatrix4::s_identityMatrix[4][4] = { {1.0, 0.0, 0.0, 0.0}, 
{0.0, 1.0, 0.0, 0.0},
{0.0, 0.0, 1.0, 0.0},
{0.0, 0.0, 0.0, 1.0} };

int CMatrix4::getRotation(double& angle, CD3& axis, const CD3& rotateFrom, const CD3& rotateTo)
{
	assert(!rotateFrom.isNull());
	assert(!rotateTo.isNull());

    axis = rotateFrom % rotateTo;
    if(!axis.isNull())
    {
		double angleCos = (rotateFrom * rotateTo) / (rotateFrom.norm() * rotateTo.norm());
		assert(angleCos >= -1.0 && angleCos <= 1.0);
		angle =  acos(angleCos);
    }
    else
    {
        // vectors are collinear
        double k;
        if(!icadRealEqual(rotateFrom[0],0.0) && !icadRealEqual(rotateTo[0],0.0))
            k = rotateFrom[0] / rotateTo[0];
        else
            if(!icadRealEqual(rotateFrom[1],0.0) && !icadRealEqual(rotateTo[1],0.0))
                k = rotateFrom[1] / rotateTo[1];
			else
                if(!icadRealEqual(rotateFrom[2],0.0) && !icadRealEqual(rotateTo[2],0.0))
                    k = rotateFrom[2] / rotateTo[2];
				else
					return 0;
        if(k > 0)
        {
            angle = 0.0;
			axis[0] = 1.0;
			axis[1] = 0.0;
			axis[2] = 0.0;
        }
        else
        {
            // vectors have different directions, 
            // must rotate on Pi around voluntary orthogonal vector
            if(!icadRealEqual(rotateFrom[0],0.0))
                axis = CD3((- rotateFrom[2] - rotateFrom[1]) / rotateFrom[0], 1.0, 1.0);
            else
                if(!icadRealEqual(rotateFrom[1],0.0))
                    axis = CD3(1.0, (- rotateFrom[2] - rotateFrom[0]) / rotateFrom[1], 1.0);
                else
                   if(!icadRealEqual(rotateFrom[2],0.0))
                        axis = CD3(1.0, 1.0, (- rotateFrom[1] - rotateFrom[0]) / rotateFrom[2]);
				   else
					   return 0;
			angle = IC_PI;
        }
    }
	return 1;
}

void CMatrix4::setValue(const double& angle, const CD3& axis)
{
	memset(m_v, 0, 16 * sizeof(double));
	CD3 n(axis);
	n.normalize();
	double angleSin = sin(angle);
	double angleCos = cos(angle);

	m_a00 = angleCos + (1.0 - angleCos) * n[0] * n[0];
	m_a01 = (1.0 - angleCos) * n[0] * n[1];
	m_a02 = (1.0 - angleCos) * n[0] * n[2];
	m_a11 = angleCos + (1.0 - angleCos) * n[1] * n[1];
	m_a12 = (1.0 - angleCos) * n[1] * n[2];
	m_a22 = angleCos + (1.0 - angleCos) * n[2] * n[2];

	n *= angleSin;
	m_a10 = n[2] + m_a01;
	m_a20 = - n[1] + m_a02;
	m_a21 = n[0] + m_a12;
	m_a01 -= n[2];
	m_a02 += n[1];
	m_a12 -= n[0];
	m_a33 = 1.0;
}

void CMatrix4::setValue(const double* pColumn1, const double* pColumn2, const double* pColumn3, const double* pColumn4)
{
	memset(m_v, 0, 16 * sizeof(double));
	m_a00 = pColumn1[0];
	m_a10 = pColumn1[1];
	m_a20 = pColumn1[2];
	m_a01 = pColumn2[0];
	m_a11 = pColumn2[1];
	m_a21 = pColumn2[2];
	m_a02 = pColumn3[0];
	m_a12 = pColumn3[1];
	m_a22 = pColumn3[2];
	m_a03 = pColumn4[0];
	m_a13 = pColumn4[1];
	m_a23 = pColumn4[2];
	m_a33 = 1.0;
}

void CMatrix4::getValue(double* pColumn1, double* pColumn2, double* pColumn3, double* pColumn4)
{
	pColumn1[0] = m_a00;
	pColumn1[1] = m_a10;
	pColumn1[2] = m_a20;
	pColumn2[0] = m_a01;
	pColumn2[1] = m_a11;
	pColumn2[2] = m_a21;
	pColumn3[0] = m_a02;
	pColumn3[1] = m_a12;
	pColumn3[2] = m_a22;
	pColumn4[0] = m_a03;
	pColumn4[1] = m_a13;
	pColumn4[2] = m_a23;
}

CMatrix4 CMatrix4::operator*(const CMatrix4& m)
{
	CMatrix4 result;
	int i, j;
	for(i = 0; i < 4; ++i)
		for(j = 0; j < 4; ++j)
			result.m_m[i][j] = m_m[i][0] * m.m_m[0][j] + 
				m_m[i][1] * m.m_m[1][j] + 
				m_m[i][2] * m.m_m[2][j] + 
				m_m[i][3] * m.m_m[3][j];
	return result;
}

void CMatrix4::multiplyRight(double* pDestination, const double* pSource) const
{
	assert(pDestination != pSource);

	double w = m_a30 * pSource[0] + m_a31 * pSource[1] + m_a32 * pSource[2] + m_a33;
	pDestination[0] = m_a00 * pSource[0] + m_a01 * pSource[1] + m_a02 * pSource[2] + m_a03;
	pDestination[1] = m_a10 * pSource[0] + m_a11 * pSource[1] + m_a12 * pSource[2] + m_a13;
	pDestination[2] = m_a20 * pSource[0] + m_a21 * pSource[1] + m_a22 * pSource[2] + m_a23;
	if(!icadRealEqual(w,1.0))
		if(w > DBL_MIN || w < - DBL_MIN)
		{
			pDestination[0] /= w; 
			pDestination[1] /= w;
			pDestination[2] /= w;
		}
		else
		{
			pDestination[0] = DBL_MAX; 
			pDestination[1] = DBL_MAX;
			pDestination[2] = DBL_MAX;
		}
}

void CMatrix4::multiplyRightUsing33(double* pDestination, const double* pSource) const
{
	assert(pDestination != pSource);

	pDestination[0] = m_a00 * pSource[0] + m_a01 * pSource[1] + m_a02 * pSource[2];
	pDestination[1] = m_a10 * pSource[0] + m_a11 * pSource[1] + m_a12 * pSource[2];
	pDestination[2] = m_a20 * pSource[0] + m_a21 * pSource[1] + m_a22 * pSource[2];
}

void CMatrix4::makeIdentity()
{ 
	memcpy(m_v, s_identityMatrix, 16 * sizeof(double)); 
}

CMatrix4 CMatrix4::invert() const
{
	double m[16];
	double mi[16];
	memcpy(m, m_v, 16 * sizeof(double));
	memcpy(mi, s_identityMatrix, 16 * sizeof(double));
	solveLES(4, m, 4, mi);
	return CMatrix4(mi);
}

bool CMatrix4::isEqualWithin(const CMatrix4& m, const double& precision) const
{
	for(int i = 0; i < 16; ++i)
		if(!isEqualZeroWithin(m_v[i] - m.m_v[i], precision))
			return false;
	return true;
}

bool solveLES(int n, double* pM, int c, double* pMS)
{
	int i, j, k;
	int index;
	double swap, a;
	for(k = 0; k < n - 1; ++k)
	{
		index = k;
		// find general column element
		for(i = k + 1; i < n; ++i)
			if(fabs(pM[index * n + k]) < fabs(pM[i * n + k]))
				index = i;
		if(pM[index * n + k] > DBL_MIN || pM[index * n + k] < -DBL_MIN)
		{
			if(k != index)
			{
				// swap rows of matrix
				for(j = k; j < n; ++j)
				{
					swap = pM[k * n + j];
					pM[k * n + j] = pM[index * n + j];
					pM[index * n + j] = swap;
				}
				// swap elements of vectors
				for(j = 0; j < c; ++j)
				{
					swap = pMS[k * c + j];
					pMS[k * c + j] = pMS[index * c + j];
					pMS[index * c + j] = swap;
				}
			}
			// clear one column
			for(i = k + 1; i < n; ++i)
			{
				a = - pM[i * n + k] / pM[k * n + k];
				for(j = k + 1; j < n; ++j)
					pM[i * n + j] += a * pM[k * n + j];
				for(j = 0; j < c; ++j)
					pMS[i * c + j] += a * pMS[k * c + j];
			}
		}
		else
		{
			// general column element is too little
			assert(false);
			return false;
		}
	}
	// back turn
	// find coordinates n of solution vectors
	for(i = 0; i < c; ++i)
		pMS[n * c - n + i] /= pM[n * n - 1];
	// find the rest coordinates
	for(i = n - 2; i > -1; --i)
	{
		for(j = n - 1; j > i; --j)
		{
			for(k = 0; k < c; ++k)
				pMS[i * c + k] -= pM[i * n + j] * pMS[j * c + k];
		}
		for(k = 0; k < c; ++k)
			pMS[i * c + k] /= pM[i * n + i];
	}
	return true;
}
