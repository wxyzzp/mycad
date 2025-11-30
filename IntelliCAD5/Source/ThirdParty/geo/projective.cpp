/* D:\ICADDEV\THIRDPARTY\GEO\PROJECTIVE.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Projective Transformation support
 * 
 */ 

#include "Geometry.h"
#include "projective.h"
#include "LinSolver.h"

///////////////////////////////////////////////////////////////////////
//						Projective Transformations	                 //
///////////////////////////////////////////////////////////////////////

// The current implementation is limited to transformations from R^2 to R^2

// default constructor - the identity transformation
CProjective::CProjective()
{
	int i,j;
	for (i = 0; i <= 2; i++)
	{  // begin i-loop
		for (j = 0; j <= 2; j++)
		{  // begin j-loop
			if (i == j)
			{
				m_Matrix[i][j] = 1.0;
			}
			else
			{
				m_Matrix[i][j] = 0.0;
			}
		}  //   end j-loop
	}  //   end i-loop
}

/***************************************************************************/

void CProjective::ApplyToPoint(const C3Point ptIn,
							         C3Point & ptOut) const
{
	RC rc;
	ApplyToPoint(ptIn, ptOut, rc);
	ASSERT(rc == SUCCESS);
}

/***************************************************************************/

void CProjective::ApplyToPoint(const C3Point ptIn,   // In: The point to be transformed
		                             C3Point & ptOut,// Out:  The transformed point             
					                 RC & rc) const  // Out: SUCCESS if nonzero denominator
                                                     //      FAILURE otherwise
{
	ASSERT (ptIn.Z() == 0.0);  // make sure the input point is 2D

	// Let P be the homogeneous version of ptIn
	C3Point P = ptIn;
	P.SetZ(1.0);

    C3Point Q = m_Matrix*P; // Q is the 2D output point in homogeneous coords.

	if ( fabs(Q.Z()) < FUZZ_DIST)
	{
		rc = FAILURE;
	}
	else
	{
		ptOut = Q/Q.Z();
		ptOut.SetZ(0.0);
		rc = SUCCESS;
	}

}

/***************************************************************************/

// This function builds a projective transformation T such that
// T(P[i]) = Q[i] assuming that the data here have been normalized
// so that P[0] = (0, 0, 0).  We will put a temporary assertion in
// here to verify that is the case.

// This function can be used to build a transformation from "calibration data".
// That is, we are given four pairs of points P[i] and Q[i] and we will try
// to construct a projective transformation T such that T(P[i]) = Q[i].
// We will do this by setting up and solving a system of linear equations in
// which there are 3 unknowns, namely all the entries of the 3 x 3 matrix
// representing T, except for the [2][2] entry, which is always 1, and the
// [0][1] and [0][2] entries, which are the (x,y) components of T(P[0]), which
// is T(0, 0, 0).

// We now will go into detail regarding the form of the system of linear
// equations that we are going to set up and solve.

// For i = 0,1,2,3:

// Let P[i] = (r[i],s[i])
// and Q[i] = (u[i],v[i])

// So, we want to find the projective transformation T such that
// T (r[i], s[i]) = (u[i], v[i]) 

// Now, we will represent the matrix M representing T in terms of the 6
// unknowns x[0],...x[5] and the input data u[0] and [1] as follows

// | x[0] x[1] u[0] |
// | x[2] x[3] u[1] |
// | x[4] x[5]   1  |

// Therefore, for i = 1,2,3 we can express T(P[i]) as

// | x[0] x[1] u[0] | | r[i] |   | x[0]*r[i] + x[1]*s[i] + u[0] |
// | x[2] x[3] u[1] | | s[i] | = | x[3]*r[i] + x[4]*s[i] + v[0] |
// | x[4] x[5]   1  | |   1  |   | x[6]*r[i] + x[7]*s[i] + 1    |

// Since the right hand side of the above equation is the homogeneous form
// of Q[i], we can write the equations that we wish to solve for the
// x[i] as follows.

// x[0]*r[i] + x[1]*s[i] + u[0]
// _____________________________ = u[i]
// x[4]*r[i] + x[5]*s[i] + 1 

// x[2]*r[i] + x[3]*s[i] + v[0]
// _____________________________ = v[i]
// x[4]*r[i] + x[5]*s[i] + 1 

// Multiplying each side of the above two equations by the denominator of the
// left hand side yields the following

// x[0]*r[i] + x[1]*s[i] + u[0] = u[i]*( x[4]*r[i] + x[5]*s[i] + 1 )
// x[2]*r[i] + x[3]*s[i] + v[0] = v[i]*( x[4]*r[i] + x[5]*s[i] + 1 )

// If we multiply out the right hand side and then bring all the terms involving
// the x[i] to the left hand side, leaving only the constant terms on the right
// hand side, we get the following 6 equations in the 6 unknowns x[0],...,x[5].

// r[1]*x[0] + s[1]*x[1] - u[1]*r[1]*x[4] - u[1]*s[1]*x[5] = u[1] - u[0]
// r[2]*x[0] + s[2]*x[1] - u[2]*r[2]*x[4] - u[2]*s[2]*x[5] = u[2] - u[0]
// r[3]*x[0] + s[3]*x[1] - u[3]*r[3]*x[4] - u[3]*s[3]*x[5] = u[3] - u[0]

// r[1]*x[2] + s[1]*x[3] - v[1]*r[1]*x[4] - v[1]*s[1]*x[5] = v[1] - v[0]
// r[2]*x[2] + s[2]*x[3] - v[2]*r[2]*x[4] - v[2]*s[2]*x[5] = v[2] - v[0]
// r[3]*x[2] + s[3]*x[3] - v[3]*r[3]*x[4] - v[3]*s[3]*x[5] = v[3] - v[0]

// So it is these equations that we will put into a matrix-vector equation,
// solve for the x[i] and use these x[i] to build the projective transformation

/***************************************************************************/

void CProjective::BuildFrom4NormalizedPointPairs(C3Point P[4],
												 C3Point Q[4],
												 RC & rc)

{
	RC rc1, rc2, rc3, rc4;

	rc = SUCCESS;  // innocent until proven guilty

	// begin assertions
	ASSERT (P[0].X() == 0.0);
    ASSERT (P[0].Y() == 0.0);
    ASSERT (P[0].Z() == 0.0);
	//   end  assertions

    // Reformulate the input data in terms of the above equations.
	double r[4];
	double s[4];
	double u[4];
	double v[4];
	
	int i;
	int j;
	int iplus1;
	for (i = 0; i < 4; i++)
	{  // begin i-loop
		r[i] = P[i].X();
		s[i] = P[i].Y();
		u[i] = Q[i].X();
		v[i] = Q[i].Y();
	}  //   end i-loop

    // Allocate space for the matrix
	int n = 6; // It will be a 6 x 6 matrix
	CLinSolver M;
	rc1 = M.Construct(n, n);
	
	CRealArray x(n, rc2);

	if ((rc1 == SUCCESS) && (rc2 == SUCCESS))
	{   // begin case of successful allocation
		
		// fill the matrix
		for (i = 0; i < 3; i++)
		{  // begin i-loop

			iplus1 = i + 1;
			
			M.put_Entry(i , 0 , r[iplus1]);
			M.put_Entry(i , 1 , s[iplus1]);
			M.put_Entry(i , 2 , 0.0);
			M.put_Entry(i , 3 , 0.0);
			M.put_Entry(i , 4 , -1.0*u[iplus1]*r[iplus1]);
			M.put_Entry(i , 5 , -1.0*u[iplus1]*s[iplus1]);
			
			
			j = i + 3;
			M.put_Entry(j , 0 , 0.0); // all indices on RHS are i not j here.
			M.put_Entry(j , 1 , 0.0);
			M.put_Entry(j , 2 , r[iplus1]); 
			M.put_Entry(j , 3 , s[iplus1]); 
			M.put_Entry(j , 4 , -1.0*v[iplus1]*r[iplus1]);
			M.put_Entry(j , 5 , -1.0*v[iplus1]*s[iplus1]);
			
			// Now take care of the RHS
			M.put_RightHandSide(i, u[iplus1]-u[0]);
			M.put_RightHandSide(j, v[iplus1]-v[0]);
			
		}  //   end i-loop
		
		rc3 = M.Factor();
		

		// If the good row count is less than the total
		// number of rows, set rc3 = FAILURE
		if (rc3 == SUCCESS)
		{
			short RowCount;
			short GoodRowCount;
			M.get_RowCount(&RowCount);
			M.get_GoodRowCount(&GoodRowCount);
			
			if (GoodRowCount < RowCount)
			{
				rc3 = FAILURE; 
			}

		}

		
		if (rc3 == SUCCESS)
		{   // begin case of successful factorization
			rc4 = M.Solve();
			if (rc4 == SUCCESS)
			{  // begin case of successful solution
				
				// the following assignments to x could be bypassed
				// and we could assign the results right to m_Matrix,
				// but we are doing this for clarity right now.
				
				for (i = 0; i < n; i++)
					M.get_Variable(i, &(x[i]));
				
				
				m_Matrix[0][0] = x[0];
				m_Matrix[0][1] = x[1];
				m_Matrix[0][2] = u[0];
				
				m_Matrix[1][0] = x[2];
				m_Matrix[1][1] = x[3];
				m_Matrix[1][2] = v[0];
				
				m_Matrix[2][0] = x[4];
				m_Matrix[2][1] = x[5];
				m_Matrix[2][2] = 1.0; // we always set this to 1.0
			}  //   end case of successful solution
			else
			{  // begin case of failed solution
				rc = FAILURE;
			}  //   end case of failed solution
		}   //   end case of successful factorization
		else
		{   // begin case of failed factorization
			rc = FAILURE;
		}   //   end case of failed factorization
		
		
		
		
	}   //   end case of successful allocation
	else
	{  // begin case of failed allocation
		rc = FAILURE;
	}  //   end case of failed allocation

}

/***************************************************************************/

// The usage of this function is as follows.
// If A, B and C are objects of type CProjective, then
// A = B.Of(C) evaluates as follows:  A(P) = B(C(P))

CProjective CProjective::Of(CProjective & that)
{
	CProjective T;

	// Initialize to all zeroes.
	int i,j,k;
	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			T.m_Matrix[i][j] = 0.0;
		}
	}

	// Now do the matrix multiplication
	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			for (k = 0; k < 3; k++)
			{
				T.m_Matrix[i][j] += this->m_Matrix[i][k]*that.m_Matrix[k][j];
			}
		}
	}	
	

	return T;
}

/***************************************************************************/

// The following is not a member function
// Given 4 pairs of points P[i], Q[i] we construct a projective transformation
// T such that T(P[i]) = Q[i].

void Projective4Pt(C3Point P[4],
				   C3Point Q[4],
				   CProjective & T,
				   RC & rc)
{

    int i;

	// First we solve the simpler problem of finding a projective transformation
	// TNormalized that takes the points P[i] - P[0] to the Q[i]
	C3Point PNormalized[4];
	
	for (i = 0; i < 4; i++)
	{
		PNormalized[i] = P[i] - P[0];
	}
	
	CProjective TNormalized;
	TNormalized.BuildFrom4NormalizedPointPairs(PNormalized, Q, rc);
	
	// Now we compose TNormalized with the appropriate translation matrix to
	// get the required projective transformation T.
	if (rc == SUCCESS)
	{  // begin composing TNormalized with a translation matrix
		
		// First build the translation matrix
		CProjective Delta; // It is initialized as the identity transformation
		Delta.m_Matrix[0][2] = -1.0*P[0].X();
		Delta.m_Matrix[1][2] = -1.0*P[0].Y();

        T = TNormalized.Of(Delta);

	}  //   end composing TNormalized with a translation matrix
}

