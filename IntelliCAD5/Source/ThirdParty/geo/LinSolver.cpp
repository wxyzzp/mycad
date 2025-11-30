// LinSolver.cpp : Implementation of the CLinSolver Class

#include "Geometry.h"
#include "LinSolver.h"

/*
The linear solver takes a system of linear equations and returns a solution
with diagnostics.  The number of equations may be smaller or greater than the
number of unknowns, and some equations may be redundant. The order of the list
matters, equations further down the list are considered less important.

Initially, the solver will try to perform LU decomposition, with pivoting on
columns (not on rows, to retain the equations' priority order).  Failing
to find a pivot, the current row is be removed to the end of the list, and a
pivot is searched in the next row.  The LU decomposition may be completed 
before the end of the rows is reached, which means that some equations down
the list will be ignored.

At the end of a successful LU decomposition we may still end up with more rows
than columns, but some of the unknowns may be irrelevant, if their columns
in the matrix are zero, so we remove them.

If, after removing redundant unknowns, the matrix is square, then the LU
factoring is used to solve the system.  Otherwise, we switch to LQ 
factoring to find the solution of least-change solution from the previous
one.

After we have the solution, we take a second look at the "bad" equations.
We know that a "bad" equation is redundant, in the sense that its coefficients
row is a linear combination of good rows, but it is possible that 
its right hand side is such that the solution still satisfies it.  If that 
is the case, it is not so bad after all, so we mark it as good.
*/

/////////////////////////////////////////////////////////////////////////////
// CLinSolver

// Private methods
/***************************************************************************
*+ CLinSolver::Purge:	Delete all data and start afresh
*/
void CLinSolver::Purge()
	{
	delete m_pEntries;
	delete m_pMatrix;
	delete m_pRHS;
	delete m_pVariables;
	delete m_pLXEntries;
	delete m_pLX;
	delete m_pC;
	delete m_pColumns;
	delete m_pRows;
	delete m_pIsRelevant;

	Initialize();
	}
/***************************************************************************
*+ CLinSolver::Initialize:	Initialize all data members
*/
void CLinSolver::Initialize()
	{
	m_nRows = m_nColumns = m_nGood = m_nRelevant = 0;
	m_pMatrix = m_pLX = NULL;
	m_pEntries = m_pLXEntries = m_pRHS = m_pVariables = m_pC = NULL;
	m_pColumns = m_pRows = NULL;
	m_pIsRelevant = NULL;
	m_nStatus = INITIAL;
	}
/***************************************************************************
*+ CLinSolver::Pivot:	Find an entry of greatest magnitude on a given row
* This is a private method called by LUFactor.  When the entry is found, 
* its index is swapped with the row's index, to bring it to the diagonal.
* The swapping is done on the m_pColumns array
***************************************************************************/
BOOL CLinSolver::Pivot(
	short i)	// I: The index of the row we are looking at
	{
	short nPivot = i;
	BOOL bFail;

	// Look for the most significant entry in this row
	for (short j=i+1;  j<m_nColumns;  j++)
		if (fabs(A(i, j)) > fabs(A(i, nPivot)))
			nPivot = j;
	
	if (!(bFail = FZERO(A(i,nPivot), FUZZ_GEN)))
		// Swap columns, to bring the pivot to the diagonal
		Swap(m_pColumns, i, nPivot);

	return bFail;
	}
/***************************************************************************
*+ CLinSolver::LUFactor:	Perform LU factoring with column pivoting
*
* Initially the matrix A is stored in m_pLX.  When we are done, it will
* be overwritten by a lower triangular matrix L and an upper triangular U, so
* that i A=LU.  The diagonal entries of U are all 1, so we don't store them.
* The arrays m_pRows and m_pColumns should initially be m_pRows[i]=i and
* and m_pColumns[j]=j.  Upon output they contain the row and column 
* permutations.  Beyond m_nGood, the rows represent redundant equations.
* Beyond m_nRelevant, the columns represent variables that are not affected
* by the good equations.
***************************************************************************/
void CLinSolver::LUFactor()
    {
	INT i,j,k;

	// Copy the matrix
	for (i = 0;    i < m_nRows;    i++)
		memcpy (m_pLX[i], m_pMatrix[i], m_nColumns * sizeof(double));

    // Loop on all the rows 
	for (i=0;  i<m_nGood;  /* i++ or nGood-- inside the loop */)
	    {
		BOOL bNoPivot;
		do 
			{
			for (j=0;  j<i;  j++)
				{
				if (!A(i,j)) // Nothing to do if the entry is 0
					continue;

				A(i,j) /= A(j,j);
				for (k=j+1;  k<m_nRows;  k++)
					{
					if (!A(j,k)) // Nothing to do if the entry is 0
						continue;
					A(i,k) -= A(i,j) * A(j,k);
					}
				}
			if (bNoPivot = Pivot(i))
				{
				// No good pivot in this row, mark it as a bad row
				m_nGood--;
				if (i >= m_nGood)
					break;	// It's the last good row
				else
					Swap(m_pRows, i, m_nGood);
				}
			else
				// Pivot found, move to the next row
				i++;  // bNoPivot will take us out of the while-loop
			}	// End do
			while (bNoPivot);
		}	// End for i

	// Remove unknowns untouched by the good equations
	RemoveIrrelevant();
	
	// Sanity check
	ASSERT(m_nGood >= 0);
	ASSERT(m_nRelevant >= m_nGood);

	// Assess the situation
	if (m_nGood == 0)
		m_nStatus = NO_EQUATIONS;
	else if (m_nRelevant == m_nGood)
		// Fully determined system, use the LU decomposition
		m_nStatus = LU_FACTORED;
	else
		m_nStatus = UNDER_DETERMINED;
    }
/***************************************************************************
*+ CLinSolver::RemoveIrrelevant: Remove variables with all 0 coefficients
*
* The current linear system may have nothing to say about some of the 
* unknowns, namely those whose coefficients are all 0.  This private method 
* moves them to the "irrelevant" tail of the list.
***************************************************************************/
void CLinSolver::RemoveIrrelevant()
    {
	short i, j;

	// Initially all unknowns are presumed relevant
	j = m_nGood;
	while (j < m_nRelevant)
		{
		// See if this unknown has any nonzero coefficient
		BOOL bRelevant = FALSE;
		for (i=0;  i<m_nGood;  i++)
			if (bRelevant = !FZERO(A(i,j), FUZZ_GEN))
				break;
		if (bRelevant)
			// This unknown is relevant, go to the next one
			j++;
		else
			// Move this unknown to the tail of the list
			Swap(m_pColumns, j, --m_nRelevant);
		}

	// Mark the relevant unknowns
	for (j=0;  j<m_nRelevant;  j++)
		m_pIsRelevant[m_pColumns[j]] = TRUE;
	}
/***************************************************************************
*+ CLinSolver::LUSolve:	Solve a linear system whose matrix is LU-factored.
*
* It is assumed that the lower-left triangle represents L and the upper-right
* represents R (with diagonal terms 1) so that A=LU, and here we solve the
* system AX=B.  The inlines A, X and B hide the row and column permutations.
***************************************************************************/
void CLinSolver::LUSolve()
	{
	INT i,j;
	double * pResults = NULL;

	// The matrix must be square
	ASSERT(m_nGood == m_nRelevant);

	// Solve the system LZ = B
	for (i=0;  i<m_nGood;  i++)
		{
		X(i) = B(i);
		for (j=0;  j<i;  j++)
			X(i) -= A(i,j) * X(j);
		}

	// Now solve the system UX = Z
	for (i=m_nGood-1;  i>=0;  i--)
	    {
		for (j=i+1;  j<m_nGood;  j++)
			X(i) -= (A(i,j) * X(j)); 
		// The LU factoring guarantees non-zero diagonal entries.
		X(i) /= A(i,i);
	    }
	}
////////////////////////////////////////////////////////////////////////////
/*	  LQ factoring and solving for underdetermined systems
LQ factoring is the transpose of QR factoring, described in any textbook
about numerical linear algebra (eg Numerical Recipes).  We use it to solve
m equations with n unknowns, where m<n.  First we compute two matrices L 
and Q such that A=LQ, where L is m X n lower triangular, and Q is n X n
symmetric orthogonal.  

Given the system AX=B, with a "previous" solution X0, this factoring is 
used to find the solution X that minimizes |X-X0|. To do that, we change 
variables to Y, and solve A(X0+Y)=B, or AY=B-AX0, minimizing |Y|.

Once the system is factored, we can solve LZ=B-AX0, and then QY=Z.  Since
L is lower triangular, it fully determines the first m unknowns, leaving the 
rest completely free.  The solution with minimum |Z| of that is clearly Z(i)=0
for m<=i<n.  Since Q is orthogonal, |QY|=|Y|=|Z|, so this also leads to 
minimum |Y|.

More algorithm details are given in the corresponding methods.

The matrix Q is represented as the product of matrices Hi, 0<=i<n,
where H(i) is a Householder matrix of the type I-cWW', with a scalar
and a vector W (' stands for transpose). We only store a factor c(i) and 
vector Wi for each i.  Since the first i-1 coordinates of Wi are 0, we can 
store it on the same row as the ith row of L.  Since both need a diagonal
(i,i) entry, we shift Wi to the right one notch, but that is hidden by the
inline H(i,j).
*/
/***************************************************************************
*+ CLinSolver::ResetForLQ(): Reset the workspace for LQ factoring
*
* This private method will be called to clean up after an aborted attemt 
* for LU factoring has left its mark on the workspace.
***************************************************************************/
RC CLinSolver::ResetForLQ()
    {
	RC hResult = SUCCESS;
	short i, j;

	// Allocate the vector C
	if (hResult = ALLOC_ARRAY(m_pC, m_nGood, double))
		QUIT
	memset (m_pC, 0, m_nGood * sizeof(double));

	// Copy the matrix, applying the permutations
	for (i = 0;    i < m_nGood;    i++)
		for (j = 0;    j < m_nRelevant;    j++)
			m_pLX[i][j] = m_pMatrix[m_pRows[i]][m_pColumns[j]];
exit:
	return hResult;
	}
/***************************************************************************
*+CLinSolver::LQFactor()	Perform LQ factoring on a matrix
* The storage scheme of the resulting lower-triagular L, the Householder 
* vectors H and the coefficients C is hidden by the inlines L and H and C.
***************************************************************************/
 RC CLinSolver::LQFactor()
    {
	RC hResult = SUCCESS;
	int i,j;
	double r, s, t;

	// The system should not be overdetermined
	ASSERT(m_nGood <= m_nRelevant);

	// Prepare the workspace
	if (hResult = ResetForLQ())
		QUIT
	
	for (i = 0;   i < m_nGood;   i++)
		{
		/*
		The vector W and the constant c for the ith Householder matrix are:
			W = (0,...,0,Xi + |X|*(Sign of Xi),Xi+1,...,Xn), and
			c = 2 / (|W|^2).
		Note that Xi+1,...,Xn are already in place.
		*/

		// Compute r = (Sign of Xi)|X|
		s = 0;
		for (j = i + 1;   j < m_nRelevant;   j++)
			{
			t = L(i,j);
			s += t * t;
			}
		t = L(i,i);
		r = sqrt(t * t + s);
		if (t < 0)
			r = -r;

		// Compute L(i,i) = -(Sign of Xi)|X|
		if (FZERO(r, FUZZ_GEN))
			{
			hResult =  FAILURE; // Error(_T("LQ fatoring failed"));
			QUIT
			}
		L(i,i) = -r;

		// Copy Wi+1,...,Wn-1
		memcpy(&H(i,i+1), &L(i,i+1), (m_nRelevant - i - 1) * sizeof(double));

		// Set Wi=Xi+|X|*(Sign of Xi)
		t = H(i,i) = t + r;

		// Compute c = 2 / |W|^2
		t = s + t * t;
		C(i) = 2 / t;

		// Update L, multiplying it by I-cWW'
		for (j = i + 1;   j < m_nGood;   j++)
			Householder(i, &L(j,0));
		}
	m_nStatus = LQ_FACTORED;
exit:
	return hResult;
    }
/***************************************************************************
*+ CLinSolver::Householder	Apply a Householder transformation to a vector
* 
* This method will multiply the vector V from the left, in place, by the 
* Househoder matrix H = I - cWW', where 
*			W=(0,...,0,H(i,i),...,H(i,n-1)).
* Since		(I - cWW')V = V - cWW'V = V - c(W'V)W, we just need to
* subtract from V the vector rW, where r = c(W'p).  We rely on the
* fact that W(j)=0 for j<i.
/**************************************************************************/
void CLinSolver::Householder(
	short i,	// I: The index of the Householder transformation
	double * V)	// I/o: The vector to be transformed
	{
	double r = 0;
	int j;

	// Form the scalar r=c(W'V)
	for (j = i;    j < m_nRelevant;    j++)
		r += H(i,j) * V[j];
	r *= C(i);

	// Subtract rW from V
	for (j = i;    j < m_nRelevant;    j++)
		V[j] -= r * H(i,j);
	}
/***************************************************************************
*+ CLinSolver::LQSolve():  Solve min |X-X0| s.t AX=B, where A is LQ-factored
*
* We need to change variables to Y, so that X=Y-X0, and them minimize |Y|. 
* This means that we'll solve A(X0+Y)=B, or AY=B-AX0. So, our new right hand
* side will be D=B-AX0.  To form the product AX0 we must use the original 
* matrix, not the LQ-factored one. 
***************************************************************************/
RC CLinSolver::LQSolve()
    {
	RC hResult;
	int i,j;
	double r;
	double * Y = NULL;


	// Allocate space for the new variables
	if (hResult = ALLOC_ARRAY(Y, m_nRelevant, double))
		QUIT
	memset (Y, 0, m_nRelevant * sizeof(double));

	// This loop combines 2 operations:
	for (i = 0;   i < m_nGood;   i++)
		{
		// Compute the new right hand side
		r = 0;
		for (j = 0;   j < m_nRelevant;   j++)
			r += m_pMatrix[m_pRows[i]][m_pColumns[j]] * X(j);
		Y[i] = B(i) - r;

		// Solve LZ = B by forward substitution
		for (j = 0;   j < i;   j++)
			Y[i] -= L(i,j) * Y[j];
		ASSERT(!FZERO(L(i,i), FUZZ_GEN));  // Should have been tested before
		Y[i] /= L(i,i);
		}

	/*
	Now solve QY = B. Since Q is orthogonal, the inverse of Q is Q',
	hence X = Q'Y.  But Q = H(n-1)...H(0), so Q' = H'(0)...H'(n-1).
	Since H'(i) is symmetric, H'(i) = H(i), so X = H(0)...H(n-1)Y.
	*/
	for (i = m_nGood-1;   i >= 0 ;    i--)
		Householder(i, Y);

	// Now change varibles back to X = X0 + Y
	for (j = 0;   j < m_nRelevant;   j++)
		X(j) += Y[j];
exit:
	delete [] Y;
	return hResult;
	}



/////////////////////////////////////////////////////////////////////////////
// Public interface
/***************************************************************************
*+ CLinSolver::get_RowCount: Get the number of original equations
*/
RC CLinSolver::get_RowCount(
	short * pVal)	// O: The number of original equations
	{
	*pVal = m_nRows;
	return SUCCESS;
	}
/***************************************************************************
*+ CLinSolver::get_ColumnCount: Get the number of original unknowns
*/
RC CLinSolver::get_ColumnCount(
	short * pVal)	// O: The number of original unknowns
	{
	*pVal = m_nColumns;
	return SUCCESS;
	}
/***************************************************************************
*+ CLinSolver::get_GoodRowCount: Get the number of good rows
*/
RC CLinSolver::get_GoodRowCount(
	short * pVal)	// O: The number of good rows
	{
	*pVal = m_nGood;
	return SUCCESS;
	}
/***************************************************************************
*+ CLinSolver::Construct: Prepare the scene for solving a linear system
*/
RC CLinSolver::Construct(
	short nRows,		// I: Number of equations
	short nColumns)		// I: Number of unknowns
	{
	RC hResult;
	short i;
	int n;

	// Start with a clean slate
	Purge();

	// Check input validity
	if (nRows > MAX_NROWS)
		{
        hResult =  FAILURE; // Error(_T("Too many constraints"));
		QUIT
		}
	if (nColumns > MAX_NROWS)
		{
        hResult =  FAILURE; // Error(_T("Too many variables"));
		QUIT
		}

	// Allocate the meat of the matrix
	n = nRows * nColumns;
	if (hResult = ALLOC_ARRAY(m_pEntries, n, double))
		QUIT
	memset (m_pEntries, 0, n * sizeof(double)); 

	// Allocate the pointers to rows in the matrix
	if (hResult = ALLOC_ARRAY(m_pMatrix, nRows, PDOUBLE))
		QUIT
	for (i=0;  i<nRows;  i++)
		m_pMatrix[i] = &m_pEntries[i * nColumns];

	// Allocate the meat of the LX matrix (with an extra column)
	n = nRows * (nColumns + 1);
	if (hResult = ALLOC_ARRAY(m_pLXEntries, n, double))
		QUIT
	memset (m_pLXEntries, 0, n * sizeof(double)); 

	// Allocate the pointers to rows in the LU matrix
	if (hResult = ALLOC_ARRAY(m_pLX, nRows, PDOUBLE))
		QUIT
	n = nColumns + 1;
	for (i=0;  i<nRows;  i++)
		m_pLX[i] = &m_pLXEntries[i * n];

	// Allocate the right hand side
	if (hResult = ALLOC_ARRAY(m_pRHS, nRows, double))
		QUIT
	memset (m_pRHS, 0, nRows * sizeof(double)); 

	// Allocate the solution
	if (hResult = ALLOC_ARRAY(m_pVariables, nColumns, double))
		QUIT
	memset (m_pVariables, 0, nColumns * sizeof(double)); 

	// Allocate the columns table
	if (hResult = ALLOC_ARRAY(m_pColumns, nColumns, short))
		QUIT
	for (i=0;   i<nColumns;   i++)
		m_pColumns[i] = i;

	// Allocate the table of good rows (initally the original rows)
	if (hResult = ALLOC_ARRAY(m_pRows, nRows, short))
		QUIT
	for (i=0;   i<nRows;   i++)
		m_pRows[i] = i;


	// Allocate the columns table
	if (hResult = ALLOC_ARRAY(m_pIsRelevant, nColumns, BOOL))
		QUIT
	for (i=0;   i<nColumns;   i++)
		m_pIsRelevant[i] = FALSE;

	// Set the counts
	m_nGood = m_nRows = nRows;
	m_nColumns = m_nRelevant = nColumns;;

	hResult = SUCCESS;
exit:
	if (FAILED(hResult))
		Purge();

	return hResult;
	}
/***************************************************************************
*+ CLinSolver::put_Entry: Put an entry into the matrix
*/
RC CLinSolver::put_Entry(
	short i,		// I: Row index
	short j,		// I: Column index 
	double newVal)	// I: The matrix entry to put there
	{
	RC hResult;
	if (i < 0 || i >= m_nRows  ||  j < 0  ||  j >= m_nColumns)
		hResult = FAILURE; // Error(_T("Index out of range"));
	else
		{
		m_pMatrix[i][j] = newVal;
		hResult = SUCCESS;
		}
	return hResult;
	}
/***************************************************************************
*+ CLinSolver::get_Entry: Get an entry from the matrix
*/
RC CLinSolver::get_Entry(
	short i,		// I: Row index
	short j,		// I: Column index 
	double * pVal)	// I: The matrix entry at (i,j)
	{
	RC hResult;
	if (i < 0 || i >= m_nRows  ||  j < 0  ||  j >= m_nColumns)
		hResult = FAILURE; // Error(_T("Index out of range"));
	else
		{
		*pVal = m_pMatrix[i][j];
		hResult = SUCCESS;
		}
	return hResult;
	}
/***************************************************************************
*+ CLinSolver::Factor: Factor the matrix in preparation for solving
*
* The separation between Factor and Solve is for efficiency. It is often
* necessary to solve many linear systems with the same matrix but different
* right-hand sides.  In such cases, the Factor method needs to be called 
* only once, followed by a call to Solve for every new right-hand side.
/**************************************************************************/ 
RC CLinSolver::Factor()
    {
	RC hResult = SUCCESS;
	LUFactor();
	if (m_nStatus == UNDER_DETERMINED)
		hResult = LQFactor();
	return hResult;
    }
/***************************************************************************
*+ CLinSolver::put_RightHandSide: Put an entry in the right-hand side
*/
RC CLinSolver::put_RightHandSide(
	short i,		// I: The index of the entry
	double newVal)	// I: The value to put there
	{
	RC hResult;
	if (i < 0   ||   i >= m_nRows)
		hResult = FAILURE; // Error(_T("Index out of range"));
	else
		{
		m_pRHS[i] = newVal;
		hResult = SUCCESS;
		}
	return hResult;
	}
/***************************************************************************
*+ CLinSolver::get_RightHandSide: Get an entry from the right-hand side
*/
RC CLinSolver::get_RightHandSide(
	short i,		// I: The index of the entry
	double * pVal)	// I: The value at the i-th location
	{
	RC hResult;
	if (i < 0   ||   i >= m_nRows)
		hResult = FAILURE; // Error(_T("Index out of range"));
	else
		{
		*pVal = m_pRHS[i];
		hResult = SUCCESS;
		}
	return hResult;
	}
/***************************************************************************
*+ CLinSolver::Solve:  Solve a linear system, after it has been factored
*/
RC CLinSolver::Solve()
	{
	int i;
	RC hResult = SUCCESS;

	switch (m_nStatus)
		{
		case LU_FACTORED:
			LUSolve();
			break;

		case LQ_FACTORED:
			hResult = LQSolve();
			break;

		case NO_EQUATIONS:
			// Nothing to solve
			break;

		default:
			hResult = FAILURE; // Error(_T("Unexpected state"));
			QUIT
		}

	// Reconsider the "bad" equations
	for (i = m_nRows - 1;  i >= m_nGood; /* incrementation inside the loop */)
		{
		// See if the solution satisfies this equation
		double r = 0;
		for (int j=0;  j<m_nColumns;  j++)
			r += m_pMatrix[m_pRows[i]][j] * m_pVariables[j];
		if (FEQUAL(r, m_pRHS[m_pRows[i]], FUZZ_GEN))
			// This equation is not so bad, after all
			Swap(m_pRows, i, m_nGood++);
		else
			i--;
		}
exit:
	return hResult;
	}
/***************************************************************************
CLinSolver::get_RedundantCount: Get the number of redundant equations
*/
RC CLinSolver::get_RedundantCount(
	short * pVal)	// O: The number of redundant equations
	{
	*pVal = m_nRows - m_nGood;
	return SUCCESS;
	}
/***************************************************************************
*+ CLinSolver::get_Redundant: Get the index of a redundant equation
*
* To get the entire list of redundant equations, call get_RedundantCount
* first, and then call get_Redundant(i) for i=0 to RedundantCount.
***************************************************************************/
RC CLinSolver::get_Redundant(
	short i,		// I: The index in the list of redundant equations
	short * pVal)	// O: Index (in original list) of a redundant equation
	{
	RC hResult = SUCCESS;
	if (i < 0   ||   i >= m_nRows - m_nGood)
		hResult = FAILURE; // Error(_T("Index out of range"));
	else
		*pVal = m_pRows[m_nGood+i];
	
	return hResult;
	}
/***************************************************************************
CLinSolver::get_RelevantCount: Get the number of relevant unknowns
*/
RC CLinSolver::get_RelevantCount(
	short * pVal)	// O: The number of relevant unknowns
	{
	*pVal = m_nRelevant;
	return SUCCESS;
	}
/***************************************************************************
*+ CLinSolver::get_Relevant: Get a relevant unknown
*
* To get the entire list of relevant unknowns, call get_RelevantCount
* first, and then call get_Relevant(i) for i=0 to RelevantCount.
***************************************************************************/
RC CLinSolver::get_Relevant(
	short i,		// I: Index in the list of relevant unknowns
	short * pVal)	// O: Index (in the original list) of a relevant unknown
	{
	RC hResult = SUCCESS;
	if (i < 0   ||   i >= m_nColumns)
		hResult = FAILURE; // Error(_T("Index out of range"));
	else
		*pVal = m_pIsRelevant[i];
	return hResult;
	}
/***************************************************************************
*+ CLinSolver::get_Variable: Get the value of a variable (unknown)
*/
RC CLinSolver::get_Variable(
	short i,		// I: Index of the requested variable
	double * pVal)	// O: The value of the variable
	{
	RC hResult;
	if (i < 0   ||   i >= m_nColumns)
		hResult = FAILURE; // Error(_T("Index out of range"));
	else
		{
		*pVal = m_pVariables[i];
		hResult = SUCCESS;
		}
	return hResult;
	}
/***************************************************************************
*+ CLinSolver::put_Variable: Enter a value of a variable
*
* The entered values will be used as X0, to minimize |X-X0| in case the 
* system is under determined
***************************************************************************/
RC CLinSolver::put_Variable(
	short i,		// I: The index of the variable to set  
	double newVal)	// I: The value to set it to
	{
	RC hResult;
	if (i < 0   ||   i >= m_nColumns)
		hResult = FAILURE; // Error(_T("Index out of range"));
	else
		{
		m_pVariables[i] = newVal;
		hResult = SUCCESS;
		}
	return hResult;
	}
