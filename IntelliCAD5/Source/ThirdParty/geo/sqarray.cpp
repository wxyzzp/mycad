/*	SqArray.cpp - Implements the class of square arrayas. 
 *	Copyright (C) 1994-1998 Visio Corporation. All rights reserved.
 *
 *
 *	Abstract
 *
 * See 	SqArray.h for the definition of the class
 * Square arrrays are used mainly for solving equations.  
 *
 * The method Entry(i,j) gives (read and write) access to the arrays' entries 
 * for setting up the equations.  In the augmented array version, the index j
 * is presumed to wrap around, so when j > Size Entry(i,j) is interpreted as 
 * Entry(i,j-Size), and for j < 0 Entry(i,j) is interpreted as Entry(i,j+Size). 
 * In addition, CFullSqArray and CBandedArray support a [] operator, so that A[i]
 * is row i.  For those arrays A.Entry[i][j] is fully interchangeable with A[i][j].
 *
 * The methods LUFactor and LUSolve solve linear equations through Gauss
 * Elimination (which is essentially what you learn in high-school). The task
 * is broken into 2 subtasks for efficiency: Factor does everything that can
 * be done to the matrix before we know the right-hand side.  IT DOES ALTER
 * THE MATRIX. Solve does the rest of the job, without altering the matrix.
 * The (output) solution overwrites the (input) right-hand side. 
 *
 * NoPivot means that the solving is done without shuffling equations and
 * unknowns. It is faster this way, but only works in special cases, so it
 * must be used with caution. If LUFactorNoPivot fails it doesn't mean that
 * the system has no solution.  It may be possible to solve it with pivoting.	
 *
 */
#include "Geometry.h"
#include "SqArray.h"
 
DB_SOURCE_NAME;

// Default onstructor and destructor
CRealSqArray::CRealSqArray()
    {
	m_bLUFactored = FALSE;
    }

CRealSqArray::~CRealSqArray()
    {
    }
/***************************************************************************/
// The entry at a given row/column
int CRealSqArray::IndexInRange(
	int n)   // In: Column index
	         // Out: Corresponding column index between 0 and Size
    {
/* To work with periodic splines, this function assumes that the columns
 * "wrap around": when j > Size Entry(i,j) is interpreted as Entry(i,j-Size), 
 * and for j < 0 Entry(i,j) is interpreted as Entry(i,j+Size).
 */
 	while (n < 0)
		n += Size(); 

	while (n >= Size())
		n -= Size();
		
	return n;
	} 

#ifdef DEBUG
/***************************************************************************/
// Dump
 void CRealSqArray::Dump()
 	{
	DB_MSGX("Matrix, Size = %d\n",(Size()));
	for (int i=0;  i<Size();  i++)
		{
		for (int j=0;  j<Size();  j++)
			DB_MSGX("%lf  ", (Entry(i,j)));
		DB_MSGX("\n",());
		}
	}
#endif
////////////////////////////////////////////////////////////////////////////////
// Full square array
// Default onstructor and destructor
CFullSqArray::CFullSqArray()
    {
    }

CFullSqArray::~CFullSqArray()
    {
    }

// Constructor with a given size
CFullSqArray::CFullSqArray(
	int nSize,    //In: Number of rows and columns
	RC & rc)	  //Out: Return code
: CReal2DArray(nSize,nSize,rc)
    {
    }
/***************************************************************************/
// LU decomposition
RC CFullSqArray::LUFactorNoPivot()
    {
#define A(i,j) (*this)[i][j]
	int i,j,k;
	RC rc = SUCCESS;

    if (m_bLUFactored) 	// Nothing to do, the matrix is already factored
   		goto exit;
    	
	if (FZERO(A(0,0), FUZZ_GEN))
		{// Zero in the diagonal, quit now or you'll crash later.
		rc = FAILURE;
		goto exit;
		}

    // Clean below the diagonal 
	for (i=1;  i<Size();  i++)
	    {
		for (j=0;  j<i;  j++)
		    {
			if (!A(i,j)) //Nothing to do if the entry is 0
				continue;

			A(i,j) /= A(j,j);
			for (k=j+1;  k<Size();  k++)
			    {
				if (!A(j,k)) //Nothing to do if the entry is 0
					continue;
				A(i,k) -= A(i,j) * A(j,k);
			    }
		    }
		if (FZERO(A(i,i), FUZZ_GEN))
			{// Zero in the diagonal, quit now or you'll crash later.
			rc = FAILURE;
			goto exit;
			}
	    }
	m_bLUFactored = TRUE;
exit:
	RETURN_RC(CFullSqArray::LUFactorNoPivo, rc);
#undef A
    }
/***************************************************************************/
// Solve a the equations, after the array has been LU-factored - points
RC CFullSqArray::LUSolveNoPivotP(
	CPointArray & B)  // In: Right hand side 
    {
	/* It is assumed that by now the matrix is factored A=LU, where
	 * U is an upper triangular matrix, occupying A's diagonal and above.
	 * and L is a lower triangular matrix with 1 in the diagonals, and the
	 * rest of it stored below the diagonal of A.  This can be achieved by
	 * applying any LUFactor--- method to A.
	 * 
	 * The right hand side (and the solution) reside in a possibly larger
	 * array, contiguously, starting at nFirst
	 */  

#define A(i,j) (*this)[i][j]
	int i,j;


	// Solve the system L(Ux) = B letting the solution z replace B
	for (i=0;  i<Size();  i++)
		for (j=0;  j<i;  j++)
			B[i] -= A(i,j) * B[j];

	// Now solve the system Ux = z
	for (i=Size()-1;  i>=0;  i--)
	    {
		for (j=i+1;  j<Size();  j++)
			B[i] -= (A(i,j) * B[j]); 
		// The LU factoring guarantees non-zero diagonal entries.
		B[i] /= A(i,i);
	    } 

	RETURN_RC(CFullSqArray::LUSolveNoPivotP, SUCCESS);
#undef A
    }
/***************************************************************************/
// Solve a the equations, after the array has been LU-factored - real numbers
RC CFullSqArray::LUSolveNoPivotR(
	CRealArray & B)  // In: Right hand side                    
					 // Out: The solution
 
    {
	// It is assumed that by now the matrix is factored A=LU, where
	// U is an upper triangular matrix, occupying A's diagonal and above.
	// and L is a lower triangular matrix with 1 in the diagonals, and the
	// rest of it stored below the diagonal of A.  This can be achieved by
	// applying any LUFactor--- method to A.  

#define A(i,j) (*this)[i][j]
	int i,j;


	// Solve the system L(Ux) = B letting the solution z replace B
	for (i=0;  i<Size();  i++)
		for (j=0;  j<i;  j++)
			B[i] -= A(i,j) * B[j];

	// Now solve the system Ux = z
	for (i=Size()-1;  i>=0;  i--)
	    {
		for (j=i+1;  j<Size();  j++)
			B[i] -= (A(i,j) * B[j]); 
		// The LU factoring guarantees non-zero diagonal entries.
		B[i] /= A(i,i);
	    } 

	RETURN_RC(CFullSqArray::LUSolveNoPivotR, SUCCESS);
#undef A
    }
////////////////////////////////////////////////////////////////////////////////
// Banded Array Methods

/***************************************************************************/
// Default constructor and destructor
CBandedArray::CBandedArray()
    {
	m_rZeroEntry = 0.0;
	m_nWidth = 0;
    }
/***************************************************************************/
// Constructor for a banded array
CBandedArray::CBandedArray(
	int nSize,    //In: Number of rows and columns
	int nWidth,   //In: Greatest j so that A[i][i +or- j] is non zero
	RC & rc)	  //Out: Return code 
    {
	int i;
	int nFirst = 0;
	double* pAvailable = NULL;
	rc = SUCCESS;

	//No point in a band that is wider than the matrix
	if (nWidth >= nSize)
		nWidth = nSize - 1;
	
	//Set the fields
	m_rZeroEntry = 0.0;
	m_nRowCount = m_nColCount = nSize;
	m_nWidth = nWidth;

	//Allocate space
	int nEntryCount = (2*nSize - nWidth) * (nWidth + 1) - nSize;
	m_pRows = new double* [nSize];
	m_rEntries = new double [nEntryCount];
	memset (m_rEntries, 0, nEntryCount * sizeof(double)); 
	if (!m_pRows  ||  !m_rEntries)
		{
		rc = DM_MEM;
		goto exit;
		} 

	//Set the pointers
	pAvailable = m_pRows[0] = m_rEntries;
	                                      
	//pFirst is the address in rEntries of the first non 0 entry in this row
	
	for (i=1;  i<nSize;  i++)
	    {
		// See how much space taken by the previous row
		int nSpaceTaken = i - 1 + nWidth;
		if (nSpaceTaken >= nSize)
			nSpaceTaken = nSize - 1;
		nSpaceTaken = nSpaceTaken - nFirst + 1;
		
		// Move the pointer to the next available space
		pAvailable += nSpaceTaken;

		// Shift back so that the address of the (nonexistent) first in
		// the row will be such that A[i][[nLeft] will be (*pEntry)
		nFirst = i > nWidth ? i - nWidth : 0;
		m_pRows[i] = pAvailable - nFirst;
        }
exit:
	if (rc)
		{
		delete [] m_pRows;
		m_pRows = NULL;
		delete [] m_rEntries;
		m_rEntries = NULL;
		m_nRowCount = m_nColCount = m_nWidth = 0;
		}
    }
/**************************************************************************/
// The entry at a given row/column
double & CBandedArray::Entry(
	int nRow,      // In: Row index;
	int nColumn)   // In: Column index
	{
	if (nColumn < nRow - m_nWidth   ||   nColumn > nRow + m_nWidth)
		return m_rZeroEntry;
	else
		return (*this)[nRow][IndexInRange(nColumn)];
    }
/***************************************************************************/
// Solve a the equations of a Not a Knot interpolation problem
RC CBandedArray::LUFactorNoPivot()
    {
#define A (*this)
	RC rc = SUCCESS;
	int i,j,k;
	int nLast;

    if (m_bLUFactored) // Nothing to do, the matrix is already factored
    	goto exit;
    	
	if (FZERO(A[0][0], FUZZ_GEN))
		{// Zero in the diagonal, quit now or you'll crash later.
		rc = FAILURE;
		goto exit;
		}

    // Clean below the diagonal 
	for (i=1;  i<Size();  i++)
	    {
		for (j = i > m_nWidth ? i - m_nWidth : 0;  j<i;  j++)
		    {
			if (!A[i][j]) //Nothing to do if the entry is 0
				continue;

			A[i][j] /= A[j][j];
							
			nLast = j + m_nWidth;
			if (nLast >= Size())
				nLast = Size() - 1;
			for (k=j+1;  k<=nLast;  k++)
			    {
				if (!A[j][k]) //Nothing to do if the entry is 0
					continue;
				A[i][k] -= A[i][j] * A[j][k];
			    }
		    }
		if (FZERO(A[i][i], FUZZ_GEN))
			{// Zero in the diagonal, quit now or you'll crash later.
			rc = FAILURE;
			goto exit;
			}
	    }
	m_bLUFactored = TRUE;
exit:
	RETURN_RC(CBandedArray::LUFactorNoPivot, rc);
    }
#undef A
/***************************************************************************/
// Solve a the equations of a Not a Knot interpolation problem
RC CBandedArray::LUSolveNoPivotR(
	CRealArray & B)  // In: Right hand side
                     // Out: The solution
    {
#define A (*this)
	int i,j;
	int nLast;
	
	// It is assumed that by now the matrix i factored A=LU, where
	// U is an upper triangular matrix, occupying A's diagonal and above.
	// and L is a lower triangular matrix with 1 in the diagonals, and the
	// rest of it stored below the diagonal of A.  This can be achieved by
	// applying any LUFactor--- method to A.  

	// Solve the system L(Ux) = B, letting the solution z replace B
	for (i=0;  i<Size();  i++)
		for (j = i > m_nWidth ? i - m_nWidth : 0;  j<i;  j++)
			B[i] -= A[i][j] * B[j];

	// Now solve the system Ux = z
	for (i=Size()-1;  i>=0;  i--)
	    {
		nLast = i + m_nWidth;
		if (nLast >= Size())
			nLast = Size() - 1;
		for (j=i+1;  j<=nLast;  j++)
			B[i] -= (A[i][j] * B[j]); 
		// The LU factoring guarantees non-zero diagonal entries.
		B[i] /= A[i][i]; 
	    }

	RETURN_RC(CBandedArray::LUSolveNoPivotR, SUCCESS);
    }
#undef A
/***************************************************************************/
// Solve a banded matrix linear system by LU decomposition with no pivoting
RC CBandedArray::LUSolveNoPivot(
	CPointArray & B,  // In: Right hand side
                      // Out: The solution
	int nFirst)       // In: The index in the B where to start
    {
#define A (*this)
	int i,j;
	int nLast;
	
	// It is assumed that by now the matrix is factored A=LU, where
	// U is an upper triangular matrix, occupying A's diagonal and above.
	// and L is a lower triangular matrix with 1 in the diagonals, and the
	// rest of it stored below the diagonal of A.  This can be achieved by
	// applying any LUFactor--- method to A.  

	// Solve the system L(Ux) = B, letting the solution z replace B
	for (i=0;  i<Size();  i++)
		for (j= i > m_nWidth ? i - m_nWidth : 0;  j<i;  j++)
			B[nFirst+i] -= A[i][j] * B[nFirst+j];

	// Now solve the system Ux = z
	for (i=Size()-1;  i>=0;  i--)
	    {
		nLast = i + m_nWidth;
		if (nLast >= Size())
			nLast = Size() - 1;
		for (j=i+1;  j<=nLast;  j++)
			B[nFirst+i] -= (A[i][j] * B[nFirst+j]); 
		// The LU factoring guarantees non-zero diagonal entries.
		B[nFirst+i] /= A[i][i]; 
	    }

	RETURN_RC(CBandedArray::LUSolveNoPivot, SUCCESS);
    }
#undef A

////////////////////////////////////////////////////////////////////////////////
// Augmented banded Array Methods

/***************************************************************************/
// Default constructor and destructor
CAugmentedArray::CAugmentedArray()
    {
	m_pBanded = NULL;
	m_nSize = 0;
    }
CAugmentedArray::~CAugmentedArray()
    {
    }
/***************************************************************************/
// Construct from given banded array and augmentation size.
CAugmentedArray::CAugmentedArray(
	PBanded pBanded,    //In: The banded part
	int nAugmentBy,     //In: The number of extra rows and columns
	RC & rc)			//Out: Return code

:	m_arrRows(nAugmentBy, pBanded->Size(), rc), 
	m_arrCols(nAugmentBy, pBanded->Size(), rc), 
	m_arrCorner(nAugmentBy, rc)
// The augmentation blocks will be filled with zeors
    {
	if (rc)
		{
		// Components construction failed
		m_nSize = 0;
		m_pBanded = NULL;
		}
	else
		{
		m_nSize = pBanded->Size() + nAugmentBy;
		m_pBanded = pBanded;
		}
    }	

/***************************************************************************/
// The entry at a given row/column
double & CAugmentedArray::Entry(
	int nRow,      // In: Row index;
	int nColumn)   // In: Column index
    {
/* To work with periodic splines, this function assumes that the columns
 * "wrap around": when j > Size Entry(i,j) is interpreted as Entry(i,j-Size), 
 * and for j < 0 Entry(i,j) is interpreted as Entry(i,j+Size).
 */
 	// Get wihtin the range
 	nRow = IndexInRange(nRow); 
 	nColumn = IndexInRange(nColumn); 

	int n = m_pBanded->Size();
	
	if (nRow < n)
	    {
		if (nColumn < n)
			// We're in the Banded block
			return m_pBanded->Entry(nRow, nColumn);
		else
			// We're in one of the extra columns
			return m_arrCols[nColumn-n][nRow];
	    }
	else
	    {
		if (nColumn < n)
			// We're in one of the extra rows
			return m_arrRows[nRow-n][nColumn];
		else
			// We're in the corner block
			return m_arrCorner[nRow-n][nColumn-n];
	    }
    }

/***************************************************************************/
// LU decompolsition
RC CAugmentedArray::LUFactorNoPivot()
    {
// To factor the augmented banded matrix that has a block structure:

//  Banded  Cols
//  Rows    Corner
    
//  we seek two factors with similar block structures
    
//  L 0    and    U P
//  X Y           0 Q
    
//  where L and Y are lower triangular and U and Q are upper triangular.
    
//  They must satisfy the following 4 conditions:
    
//  LU = Banded;  LP = Cols;  XU = Rows;   XP + YQ = Corner.

// To compute them we do:
    
//  Step 1: Factor Banded to find L and U.
//  Step 2: Solve the linear systems LP = Cols to find P.
//  Step 3: Solve the linear systems XU = Rows to find X.
//  Step 4: Factor  (Corner - XP) to obtain Y and Q

	int i,j,k;
	RC rc = SUCCESS;
	int nBanded = m_pBanded->Size();
	int nWidth = m_pBanded->Width();
	int nExtra = m_arrRows.RowCount(); 

    if (m_bLUFactored)
    	// Nothing to do, the matrix is already factored
    	goto exit;
    	
	// Step 1
	if (rc = m_pBanded->LUFactorNoPivot())
		goto exit;
	
	// Step 2 - solve the linear system LP = Cols[k] for every k.
	// This is easy because L is lower triangular
	for (k=0;  k<nExtra;  k++)
		for (i=1;  i<nBanded;  i++)
			for (j = i > nWidth ? i - nWidth : 0;  j<i;  j++)
				m_arrCols[k][i] -= m_pBanded->Entry(i,j) * m_arrCols[k][j];
			   
	// Step 3 - solve the linear system XU = Rows[k] for every k
	// This is easy because U is upper triangular
	for (k=0;  k<nExtra;  k++)
	    {
		for (i=0;  i<nBanded;  i++)
		    {
			for (j = i > nWidth ? i - nWidth : 0;  j<i;  j++)
				m_arrRows[k][i] -= m_pBanded->Entry(j,i) * m_arrRows[k][j];
			// The LU factoring of Banded guarantees non-zero diagonal.
			m_arrRows[k][i] /= m_pBanded->Entry(i,i);
		    }
	    }
		
	// Step 4 - substract XP from Corner and then factor it
	for (i=0;  i<nExtra;  i++)
		for (k=0;  k<nExtra;  k++)
			for (j=0;  j<nBanded;  j++)
				m_arrCorner[i][k] -= m_arrRows[i][j] * m_arrCols[k][j];
	rc = m_arrCorner.LUFactorNoPivot();

    // Factoreing was completed successfully
	m_bLUFactored = TRUE;
exit:
	RETURN_RC(CAugmentedArray::LUFactorNoPivot, rc);
    }
/***************************************************************************/
// Solve a the equations of a Not a Knot interpolation problem
RC CAugmentedArray::LUSolveNoPivotR(
	CRealArray & B)  // In: Right hand side
                     // Out: The solution
    {
	int i,j;
	int nBlock = m_pBanded->Size();
	int nTotal = m_nSize;
	
	// It is assumed that by now the matrix is factored A=LU, where
	// U is an upper triangular matrix, occupying A's diagonal and above.
	// and L is a lower triangular matrix with 1 in the diagonals, and the
	// rest of it stored below the diagonal of A.  This can be achieved by
	// applying any LUFactor--- method to A.

	// Solve the system Lz = B, letting the solution replace B
	// Revisit the block structure of L above for the dtails 
	for (i=0;  i<nBlock;  i++)
		for (j=i>m_pBanded->Width() ? i-m_pBanded->Width() : 0;	 j<i;  j++)
			B[i] -= m_pBanded->Entry(i,j) * B[j];

	for (i=nBlock;  i<nTotal;  i++)
	    {
		for (j=0;  j<nBlock;  j++)
			B[i] -= m_arrRows[i-nBlock][j] * B[j];

		for (j=nBlock;  j<i;  j++)
			B[i] -= m_arrCorner[i-nBlock][j-nBlock] * B[j];
	    }

	// Now solve the system Ux = z
	// Recall that U is made oout of the super-diagonal part of Banded, 
	// the extra Column, and the bottom-left Corner
	// The LU factoring guarantees non-zero diagonal entries.
	// This allows us to divide by them without checking  
	for (i=nTotal-1;  i>=nBlock;  i--)
	    {
		for (j=i+1;  j<nTotal;  j++)
			B[i] -= m_arrCorner[i-nBlock][j-nBlock] * B[j]; 
		B[i] /= m_arrCorner[i-nBlock][i-nBlock];
	    }
	
	for (i=nBlock-1;  i>=0;  i--)
	    {
		int nLast = i + m_pBanded->Width();
		if (nLast >= nBlock)
			nLast = nBlock - 1;
		for (j=i+1;  j<=nLast;  j++)
			B[i] -= m_pBanded->Entry(i,j) * B[j]; 

		for (j=nBlock;  j<nTotal;  j++)
		    {
			B[i] -= m_arrCols[j-nBlock][i] * B[j];
		    } 
		B[i] /= m_pBanded->Entry(i,i); 
	    }

	RETURN_RC(CAugmentedArray::LUSolveNoPivotR, SUCCESS);
    }
#undef A

/***************************************************************************/
// Solve a the equations of a Not a Knot interpolation problem
RC CAugmentedArray::LUSolveNoPivotP(
	CPointArray & B)  // In: Right hand side
                      // Out: The solution
    {
	int i,j;
	int nBlock = m_pBanded->Size();
	int nTotal = m_nSize;
	
	// It is assumed that by now the matrix is factored A=LU, where
	// U is an upper triangular matrix, occupying A's diagonal and above.
	// and L is a lower triangular matrix with 1 in the diagonals, and the
	// rest of it stored below the diagonal of A.  This can be achieved by
	// applying any LUFactor--- method to A.

	// Solve the system Lz = B, letting the solution replace B
	// Revisit the block structure of L above for the dtails 
	for (i=0;  i<nBlock;  i++)
		for (j=i>m_pBanded->Width()? i-m_pBanded->Width() : 0;  j<i;  j++)
			B[i] -= m_pBanded->Entry(i,j) * B[j];

	for (i=nBlock;  i<nTotal;  i++)
	    {
		for (j=0;  j<nBlock;  j++)
			B[i] -= m_arrRows[i-nBlock][j] * B[j];

		for (j=nBlock;  j<i;  j++)
			B[i] -= m_arrCorner[i-nBlock][j-nBlock] * B[j];
	    }

	// Now solve the system Ux = z
	// Recall that U is made oout of the super-diagonal part of Banded, 
	// the extra Column, and the bottom-left Corner
	// The LU factoring guarantees non-zero diagonal entries.
	// This allows us to divide by them without checking  
	for (i=nTotal-1;  i>=nBlock;  i--)
	    {
		for (j=i+1;  j<nTotal;  j++)
			B[i] -= m_arrCorner[i-nBlock][j-nBlock] * B[j]; 
		B[i] /= m_arrCorner[i-nBlock][i-nBlock];
	    }
	
	for (i=nBlock-1;  i>=0;  i--)
	    {
		int nLast = i + m_pBanded->Width();
		if (nLast >= nBlock)
			nLast = nBlock - 1;
		for (j=i+1;  j<=nLast;  j++)
			B[i] -= m_pBanded->Entry(i,j) * B[j]; 

		for (j=nBlock;  j<nTotal;  j++)
		    {
			B[i] -= m_arrCols[j-nBlock][i] * B[j];
		    } 
		B[i] /= m_pBanded->Entry(i,i); 
	    }

	RETURN_RC(CAugmentedArray::LUSolveNoPivotP, SUCCESS);
    }
#undef A
