#include "Geometry.h"
#include "LsqTrans.h"
#include "LinSolver.h"

/***************************************************************************/
// In this function, we are finding the orthogonal transformation T that produces
// the mimimal value over all orthogonal transformations for the quantity
//
// sum{dist(T(P[i])-Q[i])^2}
//
// The construction of this transformation is done by the method of least squares.
//
// If P = (x,y) and Q = (u,v) then we can write the equation T(P) = Q as the following
// matrix-vector equation
//
// | a -b | | x |   | c |   | u |
// |      | |   | + |   | = |   | 
// | b  a | | y |   | d |   | v |
//
// where a,b,c and d are the unknowns we need to find in order to define T.
//
// For i = 0 to n-1 let P[i] = (x[i],y[i]) and let Q[i] = (u[i],v[i]).
//
// Then, we are trying to minimize the following function F in the four variables a,b,c,d,
// as can be seen by doing the matrix multiplication in the above equation and using
// the formula for the square of the distance between two points.
//
// F(a,b,c,d) = sum{(a*x[i]-b*y[i]+c-u[i])^2 + (b*x[i]+a*y[i]+d-v[i])^2}
//
// The minimum value will occur at values of (a,b,c,d) where the following four partial
// derivatives are all zero: dF/da, dF/db, dF/dc and dF/dd.
//
// If we compute all four of these partial derivatives, set them to zero, and move the
// expressions involving a,b,c and d to the left hand sides of the equations, we get the
// following four equations in four unknowns, which are to be solved for a,b,c and d.
//
// Remember that, in these equations, the quantities x[i], y[i], u[i] and v[i] are given.
//
// (sum{x[i]*x[i] + y[i]*y[i]})*a + (sum{x[i]})*c + (sum{y[i]})*d = sum{u[i]*x[i]+v[i]*y[i]}
// (sum{x[i]*x[i] + y[i]*y[i]})*b - (sum{y[i]})*c + (sum{x[i]})*d = sum{v[i]*x[i]-u[i]*y[i]}
// (sum{x[i]})*a  - (sum{y[i]})*b + n*c = sum{u[i]}
// (sum{y[i]})*a  + (sum{x[i]})*b + n*d = sum{v[i]} 

 
void OrthogonalNPt(int n, // In:  The number of points
				   C3Point * P, // In:  Array of n points
				   C3Point * Q, // In:  Array of n points
				   CAffine & T, // Out: Orthogonal transformation that best fits
				   // the data, in the least-squares sense
                   double * RMSerror, // Out: sqrt((1/n)*sum{dist(T(P[i])-Q[i])^2})
				   RC & rc) // Out:  Return code - SUCCESS or FAILURE
{
	int i;
	double ndouble = (double) n;
	int matrix_size = 4;
	
	// Copy the coordinates of the input points into one-dimensional arrays
	// for ease of use.
	
	double * x = new double[n];
	double * y = new double[n];
	double * u = new double[n];
	double * v = new double[n];
	
	for (i = 0; i < n; i++)
	{   //  begin i-loop
		x[i] = P[i].X();
		y[i] = P[i].Y();
		u[i] = Q[i].X();
		v[i] = Q[i].Y();
	}   //    end i-loop
	
	
	// Calculate the quantities that will become the entries of the matrix
	double sumxixi = 0.0;
	double sumyiyi = 0.0;
	double sumxi = 0.0;
	double sumyi = 0.0;
	double sumuixi = 0.0;
	double sumuiyi = 0.0;
	double sumvixi = 0.0;
	double sumviyi = 0.0;
	double sumui = 0.0;
	double sumvi = 0.0;
	
	for (i = 0; i < n; i++)
	{   // begin i-loop
		sumxixi += x[i]*x[i];
		sumyiyi += y[i]*y[i];
		sumxi   += x[i];
		sumyi   += y[i];
		sumuixi += u[i]*x[i];
		sumuiyi += u[i]*y[i];
		sumvixi += v[i]*x[i];
		sumviyi += v[i]*y[i];
		sumui   += u[i];
		sumvi   += v[i];
	}   //   end i-loop
	
	// Now create the CLinSolver object in which to store the above data
	CLinSolver L;
	L.Construct(matrix_size, matrix_size);
	
	// Store the data into the matrix
	L.put_Entry(0,0, sumxixi + sumyiyi);
	L.put_Entry(0,1, 0.0);
	L.put_Entry(0,2, sumxi);
	L.put_Entry(0,3, sumyi);
	
	L.put_Entry(1,0, 0.0);
	L.put_Entry(1,1, sumxixi + sumyiyi);
	L.put_Entry(1,2, -1.0*sumyi);
	L.put_Entry(1,3, sumxi);
	
	L.put_Entry(2,0, sumxi);
	L.put_Entry(2,1, -1.0*sumyi);
	L.put_Entry(2,2, ndouble);
	L.put_Entry(2,3, 0.0);
	
	L.put_Entry(3,0, sumyi);
	L.put_Entry(3,1, sumxi);
	L.put_Entry(3,2, 0.0);
	L.put_Entry(3,3, ndouble);
	
	// Store the data into the right hand side
	L.put_RightHandSide(0, sumuixi + sumviyi);
	L.put_RightHandSide(1, sumvixi - sumuiyi);
	L.put_RightHandSide(2, sumui);
	L.put_RightHandSide(3, sumvi);
	
	// Now do the Decomp and Solve
	rc = L.Factor();
	ASSERT(rc == SUCCESS);
	
	if (rc == SUCCESS)
	{
		rc = L.Solve();
		ASSERT(rc == SUCCESS);
		
		if (rc == SUCCESS)
		{
			
			// Now extract the results and use them to build the
			// object of type CAffine
			// The 3 x 3 matrix part will be of the form
			
			// | a -b 0 |
			// | b  a 0 |
			// | 0  0 1 |
			
			// since it is a combination of rotation and scaling,
			
			// and the translation vector will be of the form
			
			// | c |
			// | d |
			// | 0 |
			
			double a, b, c, d;
			L.get_Variable(0, &a);
			L.get_Variable(1, &b);	
			L.get_Variable(2, &c);
			L.get_Variable(3, &d);	
			
			// Load the a,b,c,d,e,f into the appropriate places in the
			// CAffine object T.
			
			T.SetFromScratch(a, -1.0*b, b, a, c, d);
			
			
			// Compute the RMSerror
			C3Point TofPi;
			*RMSerror = 0.0;
			double dist;
			double distsq;
			for (i = 0; i < n; i++)
			{
				T.ApplyToPoint(P[i], TofPi);
				dist = PointDistance(Q[i], TofPi);
				distsq = dist*dist;
				*RMSerror += distsq;
			}
			
			*RMSerror /= n;
			*RMSerror = sqrt(*RMSerror);
		}
	}
	
	// Clean up temporary memory
	delete [] x;
	delete [] y;
	delete [] u;
	delete [] v;
	
}

/***************************************************************************/
// In this function, we are finding the affine transformation T that produces
// the mimimal value over all affine transformations for the quantity
//
// sum{dist(T(P[i])-Q[i])^2}
//
// The construction of this transformation is done by the method of least squares.
//
// If P = (x,y) and Q = (u,v) then we can write the equation T(P) = Q as the following
// matrix-vector equation
//
// | a  b | | x |   | e |   | u |
// |      | |   | + |   | = |   | 
// | c  d | | y |   | f |   | v |
//
// where a,b,c,d,e and f are the unknowns we need to find in order to define T.
//
// For i = 0 to n-1 let P[i] = (x[i],y[i]) and let Q[i] = (u[i],v[i]).
//
// Then, we are trying to minimize the following function F in the 6 variables a,b,c,d,e,f,
// as can be seen by doing the matrix multiplication in the above equation and using
// the formula for the square of the distance between two points.
//
// F(a,b,c,d,e,f) = sum{(a*x[i]+b*y[i]+e-u[i])^2 + (c*x[i]+d*y[i]+f-v[i])^2}
//
// The minimum value will occur at values of (a,b,c,d,e,f) where the following 6 partial
// derivatives are all zero: dF/da, dF/db, dF/dc, dF/dd, dF/de and DF/df.
//
// If we compute all six of these partial derivatives, set them to zero, and move the
// expressions involving a,b,c,d e and f to the left hand sides of the equations, we get the
// following 6 equations in 6 unknowns, which are to be solved for a,b,c,d,e and f.
//
// Remember that, in these equations, the quantities x[i], y[i], u[i] and v[i] are given.
//
// (sum{x[i]*x[i]})*a + (sum{x[i]*y[i])*b + (sum{x[i]})*e = sum{u[i]*x[i]}
// (sum{x[i]*y[i]})*a + (sum{y[i]*y[i])*b + (sum{y[i]})*e = sum{u[i]*y[i]}
// (sum{x[i]*x[i]})*c + (sum{x[i]*y[i])*d + (sum{x[i]})*f = sum{v[i]*x[i]}
// (sum{x[i]*y[i]})*c + (sum{y[i]*y[i])*d + (sum{y[i]})*f = sum{v[i]*y[i]}
// (sum{x[i]})*a + (sum{y[i]})*b + n*e = sum{u[i]}
// (sum{x[i]})*c + (sum{y[i]})*d + n*f = sum{v[i]}
//
//

void AffineNPt(int n, // In:  The number of points
			   C3Point * P, // In:  Array of n points
			   C3Point * Q, // In:  Array of n points
			   CAffine & T, // Out: Orthogonal transformation that best fits
			   // the data, in the least-squares sense
			   double * RMSerror, // Out: sqrt((1/n)*sum{dist(T(P[i])-Q[i])^2})
			   RC & rc) // Out:  Return code - SUCCESS or FAILURE
{
	int i;
	double ndouble = (double) n;
	
	int matrix_size = 6;
	
	
	// Copy the coordinates of the input points into one-dimensional arrays
	// for ease of use.
	
	double * x = new double[n];
	double * y = new double[n];
	double * u = new double[n];
	double * v = new double[n];
	
	for (i = 0; i < n; i++)
	{   //  begin i-loop
		x[i] = P[i].X();
		y[i] = P[i].Y();
		u[i] = Q[i].X();
		v[i] = Q[i].Y();
	}   //    end i-loop
	
	// Calculate the quantities that will become the entries of the matrix
	double sumxixi = 0.0;
	double sumxiyi = 0.0;
	double sumyiyi = 0.0;
	double sumxi = 0.0;
	double sumyi = 0.0;
	double sumuixi = 0.0;
	double sumuiyi = 0.0;
	double sumvixi = 0.0;
	double sumviyi = 0.0;
	double sumui = 0.0;
	double sumvi = 0.0;
	
	for (i = 0; i < n; i++)
	{   //  begin i-loop
		sumxixi += x[i]*x[i];
		sumxiyi += x[i]*y[i];
		sumyiyi += y[i]*y[i];
		sumxi   += x[i];
		sumyi   += y[i];
		sumuixi += u[i]*x[i];
		sumuiyi += u[i]*y[i];
		sumvixi += v[i]*x[i];
		sumviyi += v[i]*y[i];
		sumui   += u[i];
		sumvi   += v[i];
	}   //    end i-loop
	
	// Now create the CLinSolver object in which to store the above data
	CLinSolver L;
	L.Construct(matrix_size, matrix_size);
	
	// Store the data into the matrix
	L.put_Entry(0,0, sumxixi);
	L.put_Entry(0,1, sumxiyi);	
	L.put_Entry(0,2, 0.0    );
	L.put_Entry(0,3, 0.0    );
	L.put_Entry(0,4, sumxi  );
	L.put_Entry(0,5, 0.0    );
	
	L.put_Entry(1,0, sumxiyi);
	L.put_Entry(1,1, sumyiyi);	
	L.put_Entry(1,2, 0.0    );
	L.put_Entry(1,3, 0.0    );
	L.put_Entry(1,4, sumyi  );
	L.put_Entry(1,5, 0.0    );
	
	L.put_Entry(2,0, 0.0    );
	L.put_Entry(2,1, 0.0    );	
	L.put_Entry(2,2, sumxixi);
	L.put_Entry(2,3, sumxiyi);
	L.put_Entry(2,4, 0.0    );
	L.put_Entry(2,5, sumxi  );	
	
	L.put_Entry(3,0, 0.0    );
	L.put_Entry(3,1, 0.0    );	
	L.put_Entry(3,2, sumxiyi);
	L.put_Entry(3,3, sumyiyi);
	L.put_Entry(3,4, 0.0    );
	L.put_Entry(3,5, sumyi  );	
	
	L.put_Entry(4,0, sumxi  );
	L.put_Entry(4,1, sumyi  );	
	L.put_Entry(4,2, 0.0    );
	L.put_Entry(4,3, 0.0    );
	L.put_Entry(4,4, ndouble);
	L.put_Entry(4,5, 0.0    );
	
	L.put_Entry(5,0, 0.0    );
	L.put_Entry(5,1, 0.0    );	
	L.put_Entry(5,2, sumxi  );
	L.put_Entry(5,3, sumyi  );
	L.put_Entry(5,4, 0.0    );
	L.put_Entry(5,5, ndouble);
	
	// Store the data into the right hand side
	L.put_RightHandSide(0, sumuixi);
	L.put_RightHandSide(1, sumuiyi);
	L.put_RightHandSide(2, sumvixi);
	L.put_RightHandSide(3, sumviyi);
	L.put_RightHandSide(4, sumui  );
	L.put_RightHandSide(5, sumvi  );
	
	
	// Now do the Decomp and Solve
	rc = L.Factor();
	ASSERT(rc == SUCCESS);
	
	if (rc == SUCCESS)
	{
		rc = L.Solve();
		ASSERT(rc == SUCCESS);
		
		if (rc == SUCCESS)
		{
			
			// Now extract the results and use them to build the
			// object of type CAffine
			
			// The 3 x 3 matrix part will be of the form
			
			// | a b 0 |
			// | c d 0 |
			// | 0 0 1 |
			
			// and the translation vector will be of the form
			
			// | e |
			// | f |
			// | 0 |
			
			double a, b, c, d, e, f;
			L.get_Variable(0, &a);
			L.get_Variable(1, &b);	
			L.get_Variable(2, &c);
			L.get_Variable(3, &d);	
			L.get_Variable(4, &e);
			L.get_Variable(5, &f);	
			
			// Load the a,b,c,d,e,f into the appropriate places in the
			// CAffine object T.
			
			T.SetFromScratch(a,b,c,d,e,f);
			
			// Compute the RMSerror
			C3Point TofPi;
			*RMSerror = 0.0;
			double dist;
			double distsq;
			for (i = 0; i < n; i++)
			{
				T.ApplyToPoint(P[i], TofPi);
				dist = PointDistance(Q[i], TofPi);
				distsq = dist*dist;
				*RMSerror += distsq;
			}
			
			*RMSerror /= n;
			*RMSerror = sqrt(*RMSerror);
		}
	}
	
	// Clean up temporary memory
	delete [] x;
	delete [] y;
	delete [] u;
	delete [] v;
}
