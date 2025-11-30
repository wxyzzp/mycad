#include <math.h>
#include <memory.h>
#include <string.h>
#include "constnts.h"
#include "Geometry.h"
#include "vector.h"
#include "matrix.h"
#include "trnsform.h"


CVector::CVector(int dim, double initValue)
{
	m_nAllocated = m_nSize = dim;
	register size = m_nSize;
	register double* pEntries = new double[m_nSize];
	m_rEntries = pEntries;
	while (size--)
		*pEntries++ = initValue;
}


CVector::CVector(const CRealArray &array)
{
	if (array.Size() > 0) 
		Copy(array, 0, array.Size());
	else {
		m_nSize = m_nAllocated = 0;
		m_rEntries = NULL;
	}
}


CVector::CVector(const CVector &vec)
{
	m_nAllocated = m_nSize = vec.m_nSize;
	if (m_nSize) {
		m_rEntries = new double[m_nSize];
		memcpy(m_rEntries, vec.m_rEntries, m_nSize * sizeof(double));
	}
	else
		m_rEntries = NULL;
}


double CVector::Magnitude() const 
{
	double sum = 0.0;
	for (register i = 0; i < m_nSize; i++)
		sum += m_rEntries[i] * m_rEntries[i];

	return sqrt(sum);
}


double CVector::MagnitudeNormalize()
{
	double mag = Magnitude();
	if (mag > FUZZ_DIST) {
	   register i = m_nSize; 
	   register double* pEntries = m_rEntries;
	   while(i--) 
		   *pEntries++ /= mag;
	}
	return mag;
}

// Find the subset of this vector which is a linear interpolation of the 
// members of this vector
//
void CVector::Subset(int size,				// In: size of subset
					 CVector &subset)		// Out: subset
{
	subset.RemoveAll();
	int len = m_nSize;
	if (size > len) {
		// return the copy of this vector if the length of the subset is 
		// bigger than this set
		for (register i = 0; i < len; i++) 
			subset.Add((*this)[i]);

		return;
	}

	double step = (double)(len - 1) / (size - 1);
	for (register i = 0; i < size; i++) {
		double par = i * step;
		int j = (int)par;
		if (j > len - 2)
			j = len - 2;
		par -= j;
		subset.Add((*this)[j] + par * ((*this)[j + 1] - (*this)[j]));
	}
}


// Refine a subset of this set. We will try to double the points of the 
// input subset. The newly inserted points will be those from this vector, 
// which are: 
//            
//     1. inside the right interval
//     2. nearest to the middle of the interval
//
void CVector::RefineSubset(CVector &subset)
{
	// get the dimension of this vector
	int len = m_nSize;

	// get the dimension of the subset vector
	int n = subset.m_nSize;
 
	// set up a temporary vector
	CVector tmp(2 * n);

	int num = 0;
	for (register k = 0; k < n - 1; k++) { // for each segment of the old subset

		// take the end point of the segment
		tmp[num++] = subset[k];

		// middle point of the segment
		double midPoint = (subset[k] + subset[k + 1]) / 2.;

		// find among this vector a point which is on the segment and closest to u
		double dis = INFINITY;
		int j = -1;
		for (register i = 0; i < len; i++)
			if ((*this)[i] > subset[k] && (*this)[i] < subset[k + 1]) {
				double diff = fabs((*this)[i] - midPoint);
				if (diff < dis) {
					dis = diff;
					j = i;
				}
			}  
			if ( j >= 0 ) // if there is a valid one
				tmp[num++] = (*this)[j]; 
	}
	// use the same last point
	tmp[num++] = subset[n - 1];

	subset.Swap(tmp);
}


    
CVector& CVector::Normalized()
{
	register dim = m_nSize;
	double mag = Magnitude();
	if (mag > FUZZ_DIST) {
		register double* pEntries = m_rEntries;
		while(dim--) 
			*pEntries++ /= mag;
	}
	return *this;
}


CVector	CVector::Normalize() const
{
	CVector vector(*this);
	return vector.Normalized();
}

// Build a linear uniform vector of dimention = num
//
CVector& CVector::LinearUniform(int num)
{
	ASSERT(num > 0 && num <= m_nSize);

	SetSize(num);
	register i = 0;
	register dim = m_nSize;
	register double* pEntries = m_rEntries;
	while (dim--)
		*pEntries++ = double(i++) / (num - 1);

	return *this;
}

// Return a vector that is projection of this vector to another vector
//
CVector	CVector::ProjectTo(const CVector &vector)
{
	CVector normal = vector.Normalize(); 
	return (*this * normal) * normal;
}

// Inserts the values of a vector into this vector if the value doesn't exist.
//
CVector& CVector::Insert(const CVector &vector)
{
	register num = 0, len1 = m_nSize - 1, len2 = vector.m_nSize;
	CVector newVector(len1 + len2);
	for (register i = 0; i < len1; i++) {
		newVector.Add(m_rEntries[i]);
		for (register j = 0; j < len2; j++) 
			if (vector.m_rEntries[j] > m_rEntries[i] && vector.m_rEntries[j] < m_rEntries[i+1]) 
				newVector.Add(vector.m_rEntries[j]);
	}
	newVector.Add(m_rEntries[len1]);
	Swap(newVector);

	return *this;
}

// Combine given vector into this vector with given tolerance. It is 
// similar to insert, only won't insert points which are closer than tol.
//
CVector& CVector::Combine(const CVector &vector, double tol)
{
	if (vector.m_nSize <= 2) 
		return *this;		// nothing to combine

	CVector newVector(m_nSize + vector.m_nSize);
	int jStart = 0; 
	for (register i = 0; i < m_nSize - 1; i++) { 
		newVector.Add(m_rEntries[i]);
		if (m_rEntries[i+1] - m_rEntries[i] < tol) 
			continue;
    
		for (register j = jStart; j < vector.m_nSize; j++) {    
			double newValue = vector.m_rEntries[j];  
			if (newValue >= m_rEntries[i] && newValue <= m_rEntries[i + 1]) {
				double dis1 = newValue - m_rEntries[i];
				double dis2 = m_rEntries[i + 1] - newValue;
				if (dis1 < dis2) {    
					// if newValue is in the second half of the segment 
					if (dis1 > tol) 
						newVector.Add(newValue);
					else if (newVector.m_nSize != 1) 
						newVector[newVector.m_nSize - 1] = 0.5 * (m_rEntries[i] + newValue);
				} 
				else { 
					// if newValue is in the first half of the segment 
					if (dis2 > tol) 
						newVector.Add(newValue);
					else if ( i != m_nSize - 2 ) 
						m_rEntries[i+1] = 0.5 * (m_rEntries[i + 1] + newValue);
				} 
				jStart = j + 1;
			}   
		} 
	}  
	newVector.Add(m_rEntries[m_nSize - 1]); // get the last value  

	Swap(newVector);

	return *this; // return the reference of this vector  
}

// Remove a number of numbers starting (including) the given index
//
CVector& CVector::ShiftLeftFrom(int index, int howMuch)
{
	for (register i = index - 1; i < m_nSize - howMuch; i++) 
		m_rEntries[i] = m_rEntries[i + howMuch];

	m_nSize -= howMuch;
	return *this;
}


// Find the index of the vector component which is of mininum value
//
int	CVector::FindSmallestComponent(int startIndex)
{
	int index = startIndex;
	double min = m_rEntries[startIndex];
	for (register i = startIndex + 1; i < m_nSize; i++) {
		if (m_rEntries[i] < min) {
			index = i;
			min = m_rEntries[i];
		}
	}  
	return index;
}


// Changes the values of all components to given value
//
CVector& CVector::operator=(double value)
{
	register size = m_nSize;
	register double* pEntries = m_rEntries;
	while (size--)
		*pEntries++ = value;

	return *this;
}


CVector& CVector::operator+=(const CVector &vector)
{
	ASSERT(m_nSize == vector.m_nSize);
	register size = m_nSize;
	register double* pEntries1 = m_rEntries;
	register double* pEntries2 = vector.m_rEntries;
	while (size--)
		*pEntries1++ += *pEntries2++;

	return *this;
}


CVector& CVector::operator-=(const CVector &vector)
{
	ASSERT(m_nSize == vector.m_nSize);
	register size = m_nSize;
	register double* pEntries1 = m_rEntries;
	register double* pEntries2 = vector.m_rEntries;
	while (size--)
		*pEntries1++ -= *pEntries2++;

	return *this;
}

// Vector dot product. The function throws a CIncompatibleException
// exception if vector are not comparible.
//
double CVector::operator*(const CVector &vector) const
{
	ASSERT(m_nSize == vector.m_nSize);
	register size = m_nSize;
	register double* pEntries1 = m_rEntries;
	register double* pEntries2 = vector.m_rEntries;
	register double sum = 0.0;
	while (size--)
		sum += *pEntries1++ * *pEntries2++;

	return sum;
}


// Multiplied all component by the given value
//
CVector& CVector::operator*=(double value)
{
	register size = m_nSize;
	register double* pEntries = m_rEntries;
	while (size--)
		*pEntries++ *= value;

	return *this;
}


// Divided by d and replaced the given value
//
CVector& CVector::operator/=(double value)
{
	register size = m_nSize;
	register double* pEntries = m_rEntries;
	while (size--)
		*pEntries++ /= value;

	return *this;
}

// Compare two vectors for equality
//
bool CVector::operator==(const CVector &vector) const
{
	if (m_nSize != vector.m_nSize)
		return false;

	register size = m_nSize;
	register double* pEntries1 = m_rEntries;
	register double* pEntries2 = vector.m_rEntries;
	while (size--) {
		if (*pEntries1++ != *pEntries2++)
			return false;
	}
	return true;
}


C3Vector::C3Vector(const C3Point &pt) : CVector(3)
{ 
	Set(pt.X(), pt.Y(), pt.Z()); 
}


C3Vector& C3Vector::RotatedBy(double angle, const C3Vector &axis)
{
	CAffine tr;
	tr.RotateAbout(angle, axis);

	C3Point pt;
	tr.ApplyToVector(C3Point(X(), Y(), Z()), pt);
	Set(pt.X(), pt.Y(), pt.Z());
	return *this;
}


double C3Vector::GetAngleTo(const C3Vector &vector) const
{
	return acos((*this * vector) / (GetLength() * vector.GetLength()));
}


double C3Vector::GetAngleTo(const C3Vector &vector, const C3Vector &refVector) const
{
	double cosa = (*this * vector) / (GetLength() * vector.GetLength());
	double angle = cosa > 1 ? 0. : cosa < -1 ? PI : acos(cosa);
	return refVector * (*this ^ vector) < 0. ? (TWOPI - angle) : angle;
}


double C3Vector::GetDistanceTo(const C3Vector &vector) const
{
	double tmp = *this * vector;

	double p[3];
	for (register i = 0; i < 3; i++ ) 
		p[i] = m_rEntries[i] - tmp * vector.m_rEntries[i];

	double dis = 0.0;
	for (i = 0; i < 3; i++ ) 
		dis += p[i] * p[i];

	return sqrt(dis);
}

// Adds all the cordinates of vector p1&p2, returns added
//
C3Point C3Vector::operator+(const C3Point &pt) const
{
	return C3Point(m_rEntries[0] + pt.X(), m_rEntries[1] + pt.Y(), m_rEntries[2] + pt.Z());
}

	
C3Point operator+(const C3Point &pt, const C3Vector &vector) 
{ 
	return vector + pt; 
}


C3Point C3Vector::operator-(const C3Point &pt) const
{
	return C3Point(m_rEntries[0] - pt.X(), m_rEntries[1] - pt.Y(), m_rEntries[2] - pt.Z());
}


C3Point operator-(const C3Point &pt, const C3Vector &vector) 
{ 
	return C3Point(pt.X() - vector.m_rEntries[0], pt.Y() - vector.m_rEntries[1], 
					pt.Z() - vector.m_rEntries[2]);
}


// Vector cross product. The function throws a CIncompatibleException
// exception if vector are not comparible.
//
C3Vector C3Vector::operator^(const C3Vector &vector) const
{
	return C3Vector(m_rEntries[1] * vector.m_rEntries[2] - 
		            m_rEntries[2] * vector.m_rEntries[1],
					m_rEntries[2] * vector.m_rEntries[0] - 
					m_rEntries[0] * vector.m_rEntries[2],
					m_rEntries[0] * vector.m_rEntries[1] - 
					m_rEntries[1] * vector.m_rEntries[0]);
}


// Negates all component
//

C3Vector& C3Vector::operator-()
{
    m_rEntries[0] = - m_rEntries[0];
    m_rEntries[1] = - m_rEntries[1];
    m_rEntries[2] = - m_rEntries[2];

    return *this;
}
