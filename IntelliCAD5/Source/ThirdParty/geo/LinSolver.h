// LinSolver.h : Declaration of the CLinSolver Class

#ifndef __LINSOLVER_H_
#define __LINSOLVER_H_

#include "Geometry.h"
//#include "resource.h"       // main symbols
#include  <math.h>			// fabs
//#define INTELLICAD


#define ALLOC_ARRAY(p,n,type) (p=new type[n])?SUCCESS:FAILURE
#define MAX_NROWS 100
typedef double * PDOUBLE;


typedef enum {
	INITIAL,
	NO_EQUATIONS,
	LU_FACTORED,
	UNDER_DETERMINED,
	LQ_FACTORED
	} STATE;

/////////////////////////////////////////////////////////////////////////////
// CLinSolver
class GEO_CLASS CLinSolver
{
public:
	CLinSolver()
		{
		Initialize();
		}

	virtual  ~CLinSolver()
		{
		Purge();
		}

	RC get_RowCount(short * pVal);

	RC get_ColumnCount(short * pVal);

	RC get_GoodRowCount(short * pVal);

	RC Construct(short nRows,
		short nColumns);

	RC put_Entry(short i,
		         short j,
		         double newVal);

    RC get_Entry(short i,
		         short j,
		         double * pVal);

	RC Factor();

	RC put_RightHandSide(short i,
		                 double newVal);

	RC get_RightHandSide(short i,
		                 double * pVal);

	RC Solve();

	RC get_RedundantCount(short * pVal);

	RC get_Redundant(short i,
		             short * pVal);

	RC get_RelevantCount(short * pVal);

	RC get_Relevant(short i,
		            short * pVal);

	RC get_Variable(short i,
		            double * pVal);

	RC put_Variable(short i,
		            double newVal);



protected:
// Access macros, to hide the storage schemes of matrices and vectors


	inline double & A(short i, short j)	// Matrix for LU factoring
		{
		return m_pLX[m_pRows[i]][m_pColumns[j]];
		}

	inline double & L(short i, short j) // Lower part LQ factoring
		{
		return m_pLX[i][j];
		}

	inline double & H(short i, short j) // Upper part LQ factoring
		{
		return m_pLX[i][j+1];
		}

	inline double & B(short j)	// Permutated right hand side
		{
		return m_pRHS[m_pRows[j]];
		}

	inline double & X(short j)	// Permutated solution
		{
		return m_pVariables[m_pColumns[j]];
		}

	inline double & C(short j)	// Householder scalar factors
		{
		return m_pC[j];
		}

// Other private methods
	void Initialize();

	void Purge();

	BOOL Pivot(short i);

	inline void Swap(short* p, short i, short j)
		{
		if (i!=j)
			{
			short k = p[i];
			p[i] = p[j];
			p[j] = k;
			}
		}

	void RemoveIrrelevant();

	// LU decomposition
	void LUFactor();
	void LUSolve();

	// QL decomposition
	RC ResetForLQ();
	RC LQFactor();
	void Householder(short i, double * V);
	RC LQSolve();

// Data members
	short m_nRows;			// Initial number of rows (equations)
	short m_nColumns;		// Initial number of columns (unknowns)
	short m_nGood;			// The number of good rows
	short m_nRelevant;		// The number of relevant unkowns
	double * m_pEntries;		// The entries of the coefficient matrix
	double * * m_pMatrix;		// Pointers to the rows of the matirx
	double * m_pRHS;			// Right-hand side
	double * m_pVariables;		// Solution vector
	double * m_pLXEntries;		// The entries of the factors of the matrix
	double * * m_pLX;			// Pointers to the rows of the factored matrix
	double * m_pC;				// Coefficients, used only in LQ factor/Solve
	short * m_pColumns;		// Table of (permutated) columns
	short * m_pRows;		// Table of (permutated) rows
	BOOL * m_pIsRelevant;	// Is the ith unknown relevant?
	STATE m_nStatus;		// The solver's status
};

#endif //__LINSOLVER_H_
