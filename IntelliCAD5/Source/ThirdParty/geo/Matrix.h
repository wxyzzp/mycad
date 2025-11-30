#ifndef _MATRIX_H
#define _MATRIX_H

#include "vector.h"
#include "point.h"

//
//  The class CMatrix defines a general M x N matrix. 
//
//  Author: Stephen Hou
//  Date:	3/29/2003
//

#ifdef DEBUG
class ostream;
#endif // DEBUG



class GEO_CLASS CMatrix :	public CGeoObject
{
public:

	// Default constructor
	//
	CMatrix() : m_Row(0), m_Column(0), m_pElements(NULL) {}

	CMatrix(int row, int col) : m_Row(row), m_Column(col) { m_pElements = new double [row * col]; }

	// Constructor with initialization
	//
	CMatrix(int row, int col, double initValue);

	// Copy constructor
	//
	CMatrix(const CMatrix &matrix);

	virtual ~CMatrix() { if (m_pElements) delete [] m_pElements; }


	// Copy one matrix to another. 
	//
    CMatrix*			Clone() const; 

	// Returns dimension of the matrix
	//
	void				Set(int row, int column);
	int					RowNumber() const { return m_Row; }
	int					ColumnNumber() const { return m_Column; }

	bool				IsSquare() const { return (m_Row == m_Column); }

	CVector				Row(int i) const;
	CVector				Column(int j) const;

	// Transposes the matrix and returns.
	//
	CMatrix&			Transpose();

	// Set to identitiy matrix. 
	//
	CMatrix&			Identity();

	// Returns the determinant of a square matrix.
	//
    double				Determinant() const;

	// Returns a cofactor matrix 
	//
    CMatrix				Cofactor(int row, int col) const;

	//--Inverses the matrix "this"
    CMatrix				Inverse() const;

	// Assign operator
	//
	CMatrix &			operator=(const CMatrix &matrix);

    bool				operator==(const CMatrix &matrix) const;

	// Change all the elements of matrix to double d
	//
	CMatrix &			operator=(double d);

	// Addition/subtraction/multiplication
	//
	GEO_FRIEND CMatrix	operator+(const CMatrix &matrix1, const CMatrix &matrix2);
	GEO_FRIEND CMatrix	operator-(const CMatrix &matrix1, const CMatrix &matrix2);
	GEO_FRIEND CMatrix	operator*(const CMatrix &matrix1, const CMatrix &matrix2);
	GEO_FRIEND CMatrix	operator*(const CMatrix matrix, double d);
	
	CMatrix &			operator+=(const CMatrix &matrix);
	CMatrix &			operator-=(const CMatrix &matrix);
    CMatrix &			operator*=(const CMatrix &matrix);
    CMatrix &			operator*=(double d);

	// Matrix & vector multiplication
	//
	GEO_FRIEND CVector	operator*(const CMatrix &matrix, const CVector &vector);
	GEO_FRIEND CVector	operator*(const CVector &vector, const CMatrix &matrix);

    double & 			operator()(int i, int j) { return m_pElements[i * m_Column + j]; }
    double  			operator()(int i, int j) const { return m_pElements[i * m_Column + j]; }

#ifdef DEBUG
	GEO_FRIEND ostream&	operator<<(ostream &out, const CMatrix &matrix);
#endif

protected:

	int			m_Column;		// number of columns
	int			m_Row;			// number of rows
	double*		m_pElements;    // element array
};


typedef CMatrix*	PMatrix;


//
//  The class CMatrix defines a general 3 x 3 matrix. 
//
//  Author: Stephen Hou
//  Date:	3/29/2003
//

class GEO_CLASS C3Matrix : public CMatrix
{
public:

	C3Matrix() : CMatrix(3, 3) {}
	C3Matrix(const C3Matrix &matrix) : CMatrix(matrix) {};
	virtual ~C3Matrix() {}

	// Matrix and vector multiplication
	//
	GEO_FRIEND C3Vector	operator*(const C3Matrix &matrix, const C3Vector &vector);
	GEO_FRIEND C3Vector	operator*(const C3Vector &vector, const C3Matrix &matrix);

	// Matrix and point multiplication
	//
	GEO_FRIEND C3Point	operator*(const C3Matrix &matrix, const C3Point &point);
	GEO_FRIEND C3Point	operator*(const C3Point &point, const C3Matrix &matrix);
};


typedef C3Matrix*	P3Matrix;


#endif // _MATRIX_H

