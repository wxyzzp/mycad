/*	SqArray.h - Defines the class of square array and its subclasses:
 *  banded and augmented arrays.
 *
 *	Copyright (C) 1994-1998 Visio Corporation. All rights reserved.
 *
 *
 *	ABSTRACT
 *
 *  Square arrays are used mainly for solving linear equations. The most simple
 *  square array is the full CFullSqArray represented as a 2 index dynamic
 *  array (derived from C2DArray).  The other 2 types have many zero entries, hence
 *  they have special representations and algorithms that ignore these zeros to
 *  save time and storage.
 *
 *  In a banded array the non-0 entries are in a band along the main diagonal
 *  A[i][i]. In other words A[i][j] is zero if |i-j| > Width. Its representation
 *  is similar to that of a full square array - all entries are stored contiguously
 *  and there is an array of row-addresses, but no space is allocated for the 
 *  entries outside the band.
 *
 *  An augnmented (banded) array has a block-structure
 *
 *      | B  C |
 *  A = |      | 
 *      | R  F |
 *
 *  where B is banded and F is a full square array. It is used typically when
 *  F is very small.  Se below for details of its implementation 
 *
 */
#ifndef SQARRAY_H
#define SQARRAY_H

#include "Array2D.h"
#include "PtArray.h"
#include "Array.h"

/////////////////////////////////////////////////////////////////////////////
// Real square array is an abstract class
class GEO_CLASS CRealSqArray		:		public CGeoObject
    {
// Construction & destruction
public:
	CRealSqArray();
	virtual ~CRealSqArray();

// Attributes
public:
	virtual int Size() const=0;
	virtual double & Entry(int,int)=0;
	int IndexInRange(
		int n);   // In: Column index, Out: Corresponding column index within range

	
// Operations
	virtual RC LUFactorNoPivot()=0;
	virtual RC LUSolveNoPivotP(CPointArray&)=0;
	virtual RC LUSolveNoPivotR(CRealArray&)=0;

#ifdef DEBUG
 	void Dump();
#endif

// Data
protected:
	BOOL m_bLUFactored;  // =TRUE if the matrix has been LU-facored
    };
    
typedef CRealSqArray * PRealSqArray;
/////////////////////////////////////////////////////////////////////////////
// Full real square array

class GEO_CLASS CFullSqArray : public CRealSqArray, public CReal2DArray
    {
// Construction & destruction
public:
	CFullSqArray();

	CFullSqArray(
		int nSize,  //In: Number of rows and columns
		RC & rc);	//Out: Return code

	virtual ~CFullSqArray(); 

// Attributes
public:
	int Size() const;
	double & Entry(int,int);
	
// Operations
	RC LUFactorNoPivot();
	RC LUSolveNoPivotP(CPointArray&);
	RC LUSolveNoPivotR(CRealArray&);

// No data
    };
typedef CFullSqArray * PFullSqArray;
////////////////////////////////////////////////////////////////////////////////
//		Implementation of Inline Methods
// Number of Rows and columns
inline int CFullSqArray::Size() const
    {
	return m_nRowCount;
    }
/*************************************************/
// The entry at a given row/column
inline double & CFullSqArray::Entry(
	int nRow,      // In: Row index;
	int nColumn)   // In: Column index

	{
	return (*this)[IndexInRange(nRow)][IndexInRange(nColumn)];
    }
/////////////////////////////////////////////////////////////////////////////
// Banded real array

class GEO_CLASS CBandedArray : public CRealSqArray, public CReal2DArray
    {
// Construction & destruction
public:
	CBandedArray();
	~CBandedArray()
		{
		}

	CBandedArray(
		int nSize,    //In: Number of rows and columns
		int nWidth,   //In: Greatest j so that A[i][i +or- j] is non zero
		RC & rc);	  //Out: Return code 

// Attributes
	int Size() const;
	int Width() const;
	double & Entry(int, int);

// Operations
	RC LUFactorNoPivot();
	RC LUSolveNoPivotP(CPointArray &);
	RC LUSolveNoPivotR(CRealArray &);
    RC LUSolveNoPivot(
		CPointArray & B,  // In: Right hand side, Out: Solution
        int nFirst);      // In: Index where RHS starts within B

// Data
protected:
	int m_nWidth;	      // The width of the band
	double m_rZeroEntry;  // Representing all the zero entries
    };
typedef CBandedArray * PBanded;
//////////////////////////////////////////////////////////////////////////////
//		Implementation of Inline Methods
// Number of Rows and columns
inline int CBandedArray::Size() const
    {
	return m_nRowCount;
    }
/************************************************/
// Number of entries on each side of the diagonal
inline int CBandedArray::Width() const
    {
	return m_nWidth;
    }
/***************************************************************************/
// Solve a the equations, after the array has been LU-factored - points
inline RC CBandedArray::LUSolveNoPivotP(
	CPointArray & B)  // In: Right hand side
                      // Out: The solution
    {
	return LUSolveNoPivot(B, 0);
    }                     

/////////////////////////////////////////////////////////////////////////////
// Augmented Banded array

// An augmented banded matrix has a block structure:
//               -              -
//              | Banded  Cols   |
//  Augmented = |                | 
//              | Rows    Corner |
//               -              -

// Where Banded is an external banded array, NOT owned by this array, 
// and Corner is a full square array.  Cols and Rows are non-square
// 2D arrays.  Cols is stored in transposed mode - its rows are
// actually columns.  In otrher words, Cols[i][j] represents
// Augmented[j][s+i] where s is the size of Banded.
    
class GEO_CLASS CAugmentedArray : public CRealSqArray
    {
// Construction & destruction
public:
	CAugmentedArray();

	CAugmentedArray(
		PBanded pBanded,    //In: The banded part
		int nAugmentBy,     //In: The number of extra rows and columns
		RC & rc);			//Out: Return code

	virtual ~CAugmentedArray();

// Attributes
public:
	int Size() const;
	double & Entry(int,int);
	
// Operations
	RC LUFactorNoPivot();
	RC LUSolveNoPivotP(CPointArray &);
	RC LUSolveNoPivotR(CRealArray &);

// Data
protected:
	PBanded m_pBanded;         // Pointer to an external banded block
	CReal2DArray m_arrRows;    // The extra rows
	CReal2DArray m_arrCols;    // The extra columns
	CFullSqArray m_arrCorner;  // The bottom-right full square array
	int m_nSize;               // Total size - Banded + augmentation
    };
typedef CAugmentedArray * PAugmented;
////////////////////////////////////////////////////////////////////////////////
//		Implementation of Inline Methods
// Number of Rows and columns
inline int CAugmentedArray::Size() const
    {
	return m_nSize;
    }
#endif

