#include <math.h>
#include <memory.h>
#include "geometry.h"
#include "matrix.h"
#include "trnsform.h"
#ifdef DEBUG
#include <ostream.h>
#endif // DEBUG

#define POWOFM1(A) ((A) % 2 > 0 ? -1.: 1.)

DB_SOURCE_NAME;


// Constructor with initialization
//
CMatrix::CMatrix(int row, int column, double initValue) 
		: m_Row(row), m_Column(column), m_pElements(new double [row * column])
{
	int length = row * column;
	double* pElements = m_pElements;
	while (length--)
		*pElements++ = initValue;
}


CMatrix::CMatrix(const CMatrix &mat) : m_Row(mat.m_Row), m_Column(mat.m_Column)
{
	int size = m_Row * m_Column;
	if (size) {
		m_pElements = new double[size];
		memcpy(m_pElements, mat.m_pElements, size * sizeof(double));
	}
	else
		m_pElements = NULL;
}



CMatrix* CMatrix::Clone() const
{
	CMatrix* pMatrix = new CMatrix(m_Row, m_Column);
	memcpy(pMatrix->m_pElements, m_pElements, m_Row * m_Column * sizeof(double));
	return pMatrix;
}

	
// Returns dimension of the matrix
//
void CMatrix::Set(int row, int column)
{
	m_Column = column; 
	m_Row = row;
	if (m_pElements)
		delete [] m_pElements;
	
	int size = m_Row * m_Column;
	m_pElements = new double[size];
	memset(m_pElements, 0, size * sizeof(double));
}



CVector	CMatrix::Row(int row) const
{
	CVector vector(m_Column);
	for (register i = 0; i < m_Column; i++) 
		vector[i] = (*this)(row, i);

	return vector;
}
	

CVector	CMatrix::Column(int column) const
{
	CVector vector(m_Row);
	for (register i = 0; i < m_Row; i++) 
		vector[i] = (*this)(i, column);

	return vector;
}


// Transposes the matrix and returns.
//
CMatrix& CMatrix::Transpose()
{
	ASSERT(m_Row == m_Column);

	CMatrix mat(m_Row, m_Column);
	for (register i = 0; i < m_Row - 1; i++)
		for (register j = i + 1; j < m_Column; j++) {
			double tmp = (*this)(i, j);
			(*this)(i, j) = (*this)(j, i);
			(*this)(j, i) = tmp;
		}

	return *this;
}


// Set to identitiy matrix. 
//
CMatrix& CMatrix::Identity()
{
	ASSERT(m_Row == m_Column);

    for (register i = 0; i < m_Row; i++)
		for (register j = 0; j < m_Column; j++)
			(*this)(i, j) = (i == j) ? 1.0 : 0.0;

    return *this;
}



// Returns the determinant of matrix
//
double CMatrix::Determinant() const
{
	ASSERT(m_Row == m_Column);

	double det = 0.0;
	if (m_Row > 2)
    for (register i = 0; i < m_Row; ++i)
		det += (*this)(i, 0) * POWOFM1(i) * (Cofactor(i,0).Determinant());
	else 
		det = ((*this)(0, 0) * (*this)(1, 1)) - ((*this)(1, 0) * (*this)(0, 1));

	return det;
}


// Returns a cofactor matrix 
//
CMatrix	CMatrix::Cofactor(int row, int col) const
{
	ASSERT(m_Row == m_Column);

	CMatrix mat(m_Row - 1, m_Column - 1);
	for (register k = 0, i = 0; i < m_Row; ++i) {
		int col = 0;
		if (i != k) {
			for (register j = 0; j < m_Column; ++j)
				if (j != col) 
					mat(k, col++) = (*this)(i,j);
			k++;
		}
	}
	return mat;
}


CMatrix	CMatrix::Inverse() const
{
	double det = Determinant();
	if (fabs(det) < FUZZ_DIST) 
		return CMatrix(m_Row, m_Column, 0.);

	CMatrix cfMatrix(m_Row, m_Column);	 // Cofactor matrix
	for (register i = 0; i < m_Row; ++i) {
		for (register j = 0; j < m_Column; ++j) 
			cfMatrix(i, j) = (Cofactor(i,j).Determinant()) * POWOFM1(i + j) / det;
	}
	return cfMatrix.Transpose();
}



// Assign operator
//
CMatrix & CMatrix::operator=(const CMatrix &matrix)
{
	if (&matrix != this) {
		int length1 = matrix.m_Row * matrix.m_Column;
		if (length1 > 0) {
			int length = m_Row * m_Column;
			m_Row = matrix.m_Row;
			m_Column = matrix.m_Column;
			if (length != length1) {
				if (m_pElements)
					delete [] m_pElements;
				m_pElements = new double[length1];
			}
			memcpy(m_pElements, matrix.m_pElements, length1 * sizeof(double));
		}
		else {
			m_pElements = NULL;
			m_Column = m_Row = 0;
		}
	}
	return *this;
}

    
bool CMatrix::operator==(const CMatrix &matrix) const
{
	ASSERT(m_Column == matrix.m_Column && m_Row == matrix.m_Row);

    int length = m_Column * m_Row;
	double* pElements1 = m_pElements;
	double* pElements2 = matrix.m_pElements;
    while (length--) 
		if (*pElements1++ != *pElements2++) 
			return false;
    return true;
}

	
// Changes all the elements of matrix to double d
//
CMatrix & CMatrix::operator=(double d)
{
    int length = m_Column * m_Row;
	double* pElements = m_pElements;
    while (length--) 
		*pElements++ = d;
	
    return *this;
}

	
// Addition/subtraction/Multipication
//
CMatrix	operator+(const CMatrix &matrix1, const CMatrix &matrix2)
{
	CMatrix matrix(matrix1);
	return matrix += matrix2;
}


CMatrix	operator-(const CMatrix &matrix1, const CMatrix &matrix2)
{
	CMatrix matrix(matrix1);
	return matrix-= matrix2;
}


CMatrix	operator*(const CMatrix &matrix1, const CMatrix &matrix2)
{
	CMatrix matrix(matrix1);
	return matrix *= matrix2;
}


CMatrix	operator*(const CMatrix matrix, double d)
{
	CMatrix mat(matrix.m_Row, matrix.m_Column);
	return mat *= d;
}
	
	
CMatrix& CMatrix::operator+=(const CMatrix &matrix)
{
	ASSERT(m_Column == matrix.m_Column && m_Row == matrix.m_Row);

	int length = m_Row * m_Column;
	double* pElements1 = m_pElements;
	double* pElements2 = matrix.m_pElements;
    while (length--) 
		*pElements1++ += *pElements2++;

	return *this;
}


CMatrix& CMatrix::operator-=(const CMatrix &matrix)
{
	ASSERT(m_Column == matrix.m_Column && m_Row == matrix.m_Row);

	int length = m_Row * m_Column;
	double* pElements1 = m_pElements;
	double* pElements2 = matrix.m_pElements;
    while (length--) 
		*pElements1++ -= *pElements2++;

	return *this;
}
    

CMatrix& CMatrix::operator*=(const CMatrix &matrix)
{
	ASSERT(m_Column == matrix.m_Row && m_Row != 0 && m_Column != 0);
	CMatrix tmp(m_Row, matrix.m_Column);
	for (register i = 0; i < m_Row; i++) {
		for (register j = 0; j < matrix.m_Column; j++) {
			double sum = 0.;
			for (register k = 0; m_Column; k++) 
				sum += (*this)(i, k) * matrix(k, j);
			tmp(i,j) = sum;
		}
	}
	// repalce this matrix with tmp matrix
	delete [] m_pElements;
	m_Column = matrix.m_Column;
	m_pElements = tmp.m_pElements;
	tmp.m_pElements = NULL;
	return *this;
}
    

CMatrix & CMatrix::operator*=(double d)
{
	int length = m_Row * m_Column;
	double* pElements = m_pElements;
	while (length--)
		*pElements++ *= d;

	return *this;
}

	
// Matrix & real vector multipication
//
CVector	operator*(const CMatrix &matrix, const CVector &vector)
{
	ASSERT(m_Column == vector.Size());
	CVector newVector(matrix.m_Row);
	for (register i = 0; i < matrix.m_Row; i++) {
		double sum = 0.0;
		for (register j = 0; j < matrix.m_Column; j++)
			sum += matrix(i, j) * vector[j];		
		newVector[i] = sum;
	}
	return newVector;
}


CVector	operator*(const CVector &vector, const CMatrix &matrix)
{
	ASSERT(m_Row == vector.Size());
	CVector newVector(matrix.m_Column);
	for (register i = 0; i < matrix.m_Column; i++) {
		double sum = 0.0;
		for (register j = 0; j < matrix.m_Row; j++)
			sum += matrix(j, i) * vector[j];		
		newVector[i] = sum;
	}
	return newVector;
}


#ifdef DEBUG
ostream& operator<<(ostream &out, const CMatrix &mat)
{
	out << "Matrix: " << endl;
	out << "Row Number =  " << mat.RowNumber() << ", Column Number = " << mat.ColumnNumber() << endl;
	for (register i = 0;  i < mat.RowNumber();  i++) {
		for (register j = 0; j < mat.ColumnNumber(); j++) 
			out << "M(" << i << ", " << j << ") = " << mat(i, j) << ";  ";
		out << endl;
	}
	out << endl;
	return out;
}
#endif // DEBUG
	

C3Vector operator*(const C3Matrix &matrix, const C3Vector &vector)
{
	CVector result = *(CMatrix*)&matrix * *(CVector*)&vector;
	return *static_cast<C3Vector*>(&result);
}


C3Vector operator*(const C3Vector &vector, const C3Matrix &matrix)
{
	CVector result = *(CVector*)&vector * *(CMatrix*)&matrix;
	return *static_cast<C3Vector*>(&result);
}

	
C3Point operator*(const C3Matrix &matrix, const C3Point &point)
{
	C3Vector result = matrix * C3Vector(point);
	return result.AsPoint();
}


C3Point operator*(const C3Point &point, const C3Matrix &matrix)
{
	C3Vector result = C3Vector(point) * matrix;
	return result.AsPoint();
}

