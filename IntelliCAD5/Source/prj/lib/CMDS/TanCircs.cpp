/* D:\ICADDEV\PRJ\LIB\CMDS\TANCIRCS.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 *  This file contains functions that construct a circle that is tangent
 *  to three entities, which may be any combination of lines and circles.
 * 
 */ 

#include "cmds.h"
#include "Icad.h" /*DNT*/

// GEO includes
#include "Geometry.h"
#include "point.h"
#include "line.h"
#include "elliparc.h"
#include "trnsform.h"
#include "extents.h"
#include "LinSolver.h"
#include "gvector.h"
#include "transf.h"
using namespace icl;
#include <vector>
using namespace std;

// for converting between GEO and SDS
#include "geoconvert.h"

// for vector operations
#include "icadlib.h"

// for the line_or_circle class
#include "line_or_circle.h"

// forward reference
double angle(CLine L);

// we may want to experiment with reasonable values of this tolerance
const double zerotol = 0.000000001; 
const double pointtol = 0.000001;

//-----------------------------------------------------------------------------

// This struct is just defined locally; it is not intended to be moved
// to a .h file
struct CCircle
{
	CCircle() {}
	CCircle( double _x, double _y, double _r) : x(_x), y(_y), r(_r) {};
	double	x; 
	double	y;
	double	r;
};

//-----------------------------------------------------------------------------
// This function is to be used to find two circle tangent to three circles whose
// centers are collinear.  The two circles are the ones basically whose centers are
// on opposite sides of the line through the three centers.
bool ComputeTwoTangentCircles(CCircle & TanCirc1,
							  CCircle & TanCirc2, // Output circle tangent to C1, C2 and C3
							  const CCircle C1, // Input
							  const CCircle C2, // Input
							  const CCircle C3) // Input
{
	
	bool retcode = false;
	RC rc;
	
	
	// Transform the circles so that the center of the first circle is at (0,0)
	// and the centers of the other circles lie along the X axis, those centers
	// then having coordinates (a,0) and (b,0)
	
	// Get the points in Geo Form, because it is easier to perform the transformations
	// Don't bother building a Geo circle; we want to retain the fact that the radii
	// may be negative, and Geo will not let us construct a circle of negative radius.
	C3Point Cen1(C1.x, C1.y);
	C3Point Cen2(C2.x, C2.y);
	C3Point Cen3(C3.x, C3.y);
	
	if ( (PointDistance(Cen1,Cen2) > 0.0) && 
		 (PointDistance(Cen1,Cen3) > 0.0) &&
		 (PointDistance(Cen2,Cen3) > 0.0) )
		{ // begin normal case
		
		CLine L(Cen1, Cen2);
		double theta = angle(L);

		CAffine Rotate;
		Rotate.SetRotation(-theta);

		CAffine Translate;
		C3Point MinusCen1(-Cen1.X(), -Cen1.Y());
		Translate.SetMove(MinusCen1);

		// Now construct the transform that has the effect of Translate applied first
		// followed by Rotate.  The order of the parameters is that dictated for this 
		// constructor in this situation.

		CAffine T(Rotate, Translate);
		T.ApplyToPoint(Cen1, Cen1);
		T.ApplyToPoint(Cen2, Cen2);
		T.ApplyToPoint(Cen3, Cen3);


		
		double r1 = C1.r;
		double r2 = C2.r;
		double r3 = C3.r;

		double a = Cen2.X();
		double b = Cen3.X();
		
		// Now, with this special case, set up and solve a set of linear equations
		// in which the unknowns are the x coord of the center (x,y) 
		// and radius r of the tangent circle
		
		// These equations come from the following three equations
		// (1) (r1 + r) = dist( (x,y), (0,0))
		// (2) (r2 + r) = dist( (x,y), (a,0))
		// (3) (r3 + r) = dist( (x,y), (b,0))
		
		// We square both sides of each equation to get equations (4), (5) and (6)
		// which we will not write out here.
		// and then get new equations (4)-(5), (4)-(6) and (5)-(6)

		CLinSolver E;

		E.Construct(2,2);

		E.put_Entry(0,0,-2*a);
		E.put_Entry(0,1, 2*(r1-r2));

		E.put_Entry(1,0,-2*b);
		E.put_Entry(1,1, 2*(r1-r3));




		E.put_RightHandSide(0, r2*r2 - r1*r1 - a*a);
		E.put_RightHandSide(1, r3*r3 - r1*r1 - b*b);

		rc = E.Factor();

		if (rc == SUCCESS)
			{
			rc = E.Solve();
			if (rc == SUCCESS)
				{
				double x,y,r;
				E.get_Variable(0,&x);
				E.get_Variable(1,&r);

				// now we have to solve for y
				// we will handle the positive and negative square root

				y = sqrt(r3*r3 + 2*r3*r + r*r - x*x + 2*b*x - b*b);


				
				C3Point CenT1(x,y);
				C3Point CenT2(x,-y);
				
				// Transform the resulting tangent circle back to the proper location
				T.ApplyInverseToPoint(CenT1, CenT1);

				TanCirc1.r = fabs(r);
				TanCirc1.x = CenT1.X();
				TanCirc1.y = CenT1.Y();

				T.ApplyInverseToPoint(CenT2, CenT2);

				TanCirc2.r = fabs(r);
				TanCirc2.x = CenT2.X();
				TanCirc2.y = CenT2.Y();

				retcode = true;

				}
			
			}


		

		}  // end normal case
	else
		{
		rc = false;
		}
	
	// Return the result
	return retcode;
}

//-----------------------------------------------------------------------------
bool CollinearCenters(CCircle C1, CCircle C2, CCircle C3)
{
	bool collinear;

	double a2 =  2*(C1.x - C2.x);
	double b2 =  2*(C1.y - C2.y);

	double a3 =  2*(C1.x - C3.x);
	double b3 =  2*(C1.y - C3.y);

	double determ = a2*b3 - a3*b2;
	if ( fabs(determ) < zerotol )
		collinear = true;
	else
		collinear = false;

	return collinear;
}
//-----------------------------------------------------------------------------

// Returns true if we successfully find the tangent circle T
bool ComputeOneTangentCircle( CCircle& T, // Output circle tangent to C1, C2 and C3
						   const CCircle C1, // Input 
						   const CCircle C2, // Input 
						   const CCircle C3) // Input
{
	T.r = 0;
	T.x = 0;
	T.y = 0;

		// (x-x1)^2 + (y-y1)^2 - (r-r1)^2 = 0		(1)
		// (x-x2)^2 + (y-y2)^2 - (r-r2)^2 = 0		(2)
		// (x-x3)^2 + (y-y3)^2 - (r-r3)^2 = 0		(3)

		// From (2)-(1), (3)-(1), reduce to to linear equations, ax + by + cr - d =0
	double a2 =  2*(C1.x - C2.x);
	double b2 =  2*(C1.y - C2.y);
	double c2 = -2*(C1.r - C2.r);
	double d2 =  (C2.x*C2.x + C2.y*C2.y - C2.r*C2.r) - (C1.x*C1.x + C1.y*C1.y - C1.r*C1.r);

	double a3 =  2*(C1.x - C3.x);
	double b3 =  2*(C1.y - C3.y);
	double c3 = -2*(C1.r - C3.r);
	double d3 =  (C3.x*C3.x + C3.y*C3.y - C3.r*C3.r) - (C1.x*C1.x + C1.y*C1.y - C1.r*C1.r);


	double determ = a2*b3 - a3*b2;
	if ( fabs(determ) < zerotol )
		{
		// this case should have been detected before calling this function
		return false;
		}

		// solve for x, y = mr + n;
	double mx = (b3*c2-b2*c3)/(a2*b3-a3*b2);
	double nx = (b2*d3-b3*d2)/(a2*b3-a3*b2);
	double my = (a3*c2-a2*c3)/(a3*b2-a2*b3);
	double ny = (a2*d3-a3*d2)/(a3*b2-a2*b3);

		// substitute into (1) to get ar2 + br + c = 0;
	nx -= C1.x;
	ny -= C1.y;
	
	double a = (mx*mx + my*my - 1);
	double b = (2*mx*nx + 2*my*ny - 2*C1.r);
	double c = nx*nx + ny*ny - C1.r*C1.r;

	nx += C1.x;
	ny += C1.y;

		// solve quadratic
	if ( b*b >= 4*a*c && a != 0) 
		{
		double	root = sqrt( b*b - 4*a*c);

		T.r	= (float)((-b + root)/(2*a));
		T.x = (float)(mx*T.r + nx);
		T.y = (float)(my*T.r + ny);
		return true;
		}

	return false;
}

//-----------------------------------------------------------------------------

// The following utilities eventually should be replaced with standard ones
// or moved to a shared location

// returns the square of the distance between (x1,y1) and (x2, y2)
double DistSquared2D(double x1,
					 double y1,
					 double x2,
					 double y2)
{
	
	return (x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1);
	
}

//-----------------------------------------------------------------------------

double Dist2D(double x1,
			  double y1,
			  double x2,
			  double y2)
{
	return sqrt(DistSquared2D(x1,y1,x2,y2));
}

//-----------------------------------------------------------------------------

double Norm2D(double x,
			  double y)
{
	return Dist2D(x,y,0.0,0.0);
}

//-----------------------------------------------------------------------------

void Normalize2D(double & x, double & y)
{
	double d = Norm2D(x,y);
	if (d < pointtol)
	{
		x = 0.0;
		y = 0.0;
	}
	else
	{
		x /= d;
		y /= d;
	}
}

//-----------------------------------------------------------------------------

// This function finds the normalized difference of two points in the X-Y plane
// The output is the normalized value of (Q - P)
void NormalizedDifference2D(sds_point P,
							sds_point Q,
							sds_point nQmP)
{

	for (int i = 0; i < 3; i++)
		nQmP[i] = Q[i] - P[i];

	Normalize2D(nQmP[0], nQmP[1]);

}

//-----------------------------------------------------------------------------

// This function finds the normalized average of two points in the X-Y plane
// The output is the normalized value of (Q + P)/2
void NormalizedAverage2D(sds_point P,
						 sds_point Q,
						 sds_point nAVG)
{

	for (int i = 0; i < 3; i++)
		nAVG[i] = (Q[i] + P[i])/2;

	Normalize2D(nAVG[0], nAVG[1]);

}

//-----------------------------------------------------------------------------

// The preceding utilities eventually should be replaced with standard ones
// or moved to a shared location

//-----------------------------------------------------------------------------


// It is assumed that these circles are indeed tangent to each other
// when this routine is called.
bool FindTangentPoint(CCircle C, // Input
					  CCircle T, // Input
					  double & x, // Output - x coordinate of tangent point
					  double & y) // Output - y coordinate of tangent point
{
	bool rc;

    double diffx = C.x - T.x;
	double diffy = C.y - T.y;

	double denom = Norm2D(diffx, diffy);
	if (denom < zerotol) 
	{
		rc = false;
	}
	else
	{
		// (nx, ny) is a vector of length 1 pointing from the center
		// of T to the center of C

		double nx = diffx/denom;
		double ny = diffy/denom;

		// note:  be careful which circle we use to get the radius
		// because we may have artificially set C.r < 0.0 but we are
		// not ever setting T.r < 0.0
		x = T.x + T.r*nx; 
		y = T.y + T.r*ny;
		rc = true;
	}


	return rc;
}

//-----------------------------------------------------------------------------

// This function returns the angle determined by the intersection of
// the line from (px, py) to (cx, cy) and the line from (qx, qy) to (cx, cy)
// It returns a value between 0.0 and pi
static double angle(double px,
					double py,
					double cx,
					double cy,
					double qx,
					double qy)
{
	// compute the difference vectors

	double ang;

	double ux = px - cx;
	double uy = py - cy;

	double vx = qx - cx;
	double vy = qy - cy;



	double du = sqrt(ux*ux + uy*uy);
	double dv = sqrt(vx*vx + vy*vy);

    if ((du < zerotol) || (dv < zerotol))
	{  // set ang to pi - it is not expected that this case will happen frequently
		ang = 4*atan(1.0);
	}
	else
	{  // calculate ang by using the dot product and arc cosine
	   // normalize the vectors

		ux = ux/du;
		uy = uy/du;

		vx = vx/dv;
		vy = vy/dv;

		double dotprod = ux*vx + uy*vy;

		ang = acos(dotprod);
	}

	return ang;

}

//-----------------------------------------------------------------------------

bool ScoreThisTangentCircle(CCircle T,
							CCircle C1,
							CCircle C2,
							CCircle C3,
							sds_point pic1,
							sds_point pic2,
							sds_point pic3,
							double & score)
{
	bool rc = false;

	score = 999999.0; // initialize it to some reasonably high number just in case

	double xtan1;
	double ytan1;
	double xtan2;
	double ytan2;
	double xtan3;
	double ytan3;
	
	double ang1;
	double ang2;
	double ang3;

	rc = FindTangentPoint(C1, T, xtan1, ytan1);
	if (rc == true)
	{
		FindTangentPoint(C2, T, xtan2, ytan2);
		if (rc == true)
		{
			FindTangentPoint(C3, T, xtan3, ytan3);
			if (rc == true)
			{  // begin case where we have found all three tangent points

				ang1 = angle(pic1[0], pic1[1],
					         C1.x, C1.y,
							 xtan1, ytan1);

				ang2 = angle(pic2[0], pic2[1],
					         C2.x, C2.y,
							 xtan2, ytan2);

				ang3 = angle(pic3[0], pic3[1],
					         C3.x, C3.y,
							 xtan3, ytan3);

				score = ang1 + ang2 + ang3;

			}  //   end case where we have found all three tangent points
		}
	}
		

	return rc;
}


//-----------------------------------------------------------------------------


// a return code of true denotes success
// if there has been a failure we will set the center to (0,0,0) and radius to 0.0;
bool CirTan3Circles(sds_point cen1, // Input  center of 1st circle
					double r1,      // Input  radius of 1st circle
					sds_point cen2, // Input  center of 2nd circle
					double r2,      // Input  radius of 2nd circle
					sds_point cen3, // Input  center of 3rd circle
					double r3,      // Input  radius of 3rd circle
					sds_point pic1, // Input  pick point associated with 1st circle
					sds_point pic2, // Input  pick point associated with 2nd circle
					sds_point pic3, // Input  pick point associated with 3rd circle
					sds_point cent, // Output center of tangent circle
					double *radt)   // Output radius of tangent circle
{
	bool retcode = false; // guilty until proven innocent
	
	// Initialize
	cent[0] = cent[1] = cent[2] = 0.0;
	*radt = 0.0;
	
	// From the input parameters, fill the data structures that represent the input
	// circles
	
	CCircle C1(cen1[0], cen1[1], r1);
	CCircle C2(cen2[0], cen2[1], r2);
	CCircle C3(cen3[0], cen3[1], r3);
	
    // Now compute all possible tangent circles by going through all combinations
	// of positive and negative radii for the input circles.
	CCircle T[16]; // the array of tangent circles.
	bool rc[16]; // they may not all be successful
	int num_candidates;
	
	if (CollinearCenters(C1, C2, C3))
		{
		num_candidates = 16;
		rc[0] = rc[1] = ComputeTwoTangentCircles(T[0], T[1], C1, C2, C3);

		C1.r = -C1.r;	// 1 contained
		rc[2] = rc[3] = ComputeTwoTangentCircles( T[2], T[3], C1, C2, C3);
		
		C2.r = -C2.r;	// 1,2
		rc[4] = rc[5] = ComputeTwoTangentCircles( T[4], T[5], C1, C2, C3);
		
		C1.r = -C1.r;	// 2
		rc[6] = rc[7] = ComputeTwoTangentCircles( T[6], T[7], C1, C2, C3);
		
		C3.r = -C3.r;	// 2,3
		rc[8] = rc[9]=  ComputeTwoTangentCircles( T[8], T[9], C1, C2, C3);
		
		C2.r = -C2.r;	// 3
		rc[10] = rc[11] = ComputeTwoTangentCircles( T[10], T[11], C1, C2, C3);
		
		C1.r = -C1.r;	// 1,3
		rc[12] = rc[13] = ComputeTwoTangentCircles( T[12], T[13],C1, C2, C3);
		
		C2.r = -C2.r;	// 1,2,3
		rc[14] = rc[15] = ComputeTwoTangentCircles( T[14], T[15], C1, C2, C3);
		
		}
	else
		{
		num_candidates = 8;
		
		rc[0] = ComputeOneTangentCircle( T[0], C1, C2, C3);
		
		C1.r = -C1.r;	// 1 contained
		rc[1] = ComputeOneTangentCircle( T[1], C1, C2, C3);
		
		C2.r = -C2.r;	// 1,2
		rc[2] = ComputeOneTangentCircle( T[2], C1, C2, C3);
		
		C1.r = -C1.r;	// 2
		rc[3] = ComputeOneTangentCircle( T[3], C1, C2, C3);
		
		C3.r = -C3.r;	// 2,3
		rc[4] = ComputeOneTangentCircle( T[4], C1, C2, C3);
		
		C2.r = -C2.r;	// 3
		rc[5] = ComputeOneTangentCircle( T[5], C1, C2, C3);
		
		C1.r = -C1.r;	// 1,3
		rc[6] = ComputeOneTangentCircle( T[6], C1, C2, C3);
		
		C2.r = -C2.r;	// 1,2,3
		rc[7] = ComputeOneTangentCircle( T[7], C1, C2, C3);
		}
		
		// Now that we have constructed the candidate circles, we will
		// determine which one of them is closest to the user's pick points.
		
		double current_score; // here the lowest score is considered the best
		
		int i;
		
		bool rc_from_score;
		
		double lowest_score;
		int index_with_lowest_score;
		bool found_first_tangent_circle = false;
		for (i = 0; i < num_candidates; i++)
			{   // begin i-loop
			if (rc[i] == true)
				{
				// kludge --- for some reason T[i] may have a negative radius
				T[i].r = fabs(T[i].r);
				
				if (!found_first_tangent_circle)
					{   // begin performing initializations
					rc_from_score = ScoreThisTangentCircle(T[i], C1, C2, C3,
						pic1, pic2, pic3,
						current_score);
					if (rc_from_score == true)
						{
						found_first_tangent_circle = true;
						index_with_lowest_score = i;
						lowest_score = current_score;
						}
					
					}   //   end performing initializations
				else
					{   // begin case where we have already found_first_tangent_circle
					rc_from_score = ScoreThisTangentCircle(T[i], C1, C2, C3,
						pic1, pic2, pic3,
						current_score);
					if (rc_from_score == true)
						{
						if (current_score < lowest_score)
							{
							index_with_lowest_score = i;
							lowest_score = current_score;
							}
						}
					}   //   end case where we have already found first_tangent_circle
				}
			}	//  end i-loop
		
		if (found_first_tangent_circle == true)
			{  // begin packaging the output
			cent[0] = T[index_with_lowest_score].x;
			cent[1] = T[index_with_lowest_score].y;
			cent[2] = 0.0; // we are assuming that we are in the X-Y plane
			*radt = T[index_with_lowest_score].r;
			retcode = true;
			}  //   end packaging the output
		else
			{
			retcode = false;
			}
		
		
		return retcode;
}

//-----------------------------------------------------------------------------


// This is the "core" routine for finding the possible circles
// tangent to two lines and a circle.

// Find the two values t1 and t2 such that the points
// P + (t1*ux, t1*uy) and P + (t2*ux, t2*uy) are the centers
// of circles tangent to the two lines and the circle
// specified in the calling routine, where the two lines intersect at P.
// From this information, the calling routine can determine the actual
// centers and radii of the two circles.  If the value of the input
// parameter R is positive, then the output circles will be tangent to
// the input circle on the outside.  If the R is negative, then the
// output circles will be tangent to the input circles on the inside.
bool TwoLinesOneCircleCore(double theta, // half the angle between lines
						   double ux, // x coord of unit vector
						   double uy, // y coord of unit vector
						   double cx, // x coord of circle center
						   double cy, // y coord of circle center
						   double R,  // radius of circle center
						   double & t1, // the first return parameter
						   double & t2 // the second return parameter
						   )
{
	bool rc;
	
	// we are going to solve the equation
	// At^2 + Bt + C = 0
	double A;
	double B;
	double C;
	double sn = sin(theta); // compute this once, for efficiency
	
	A = ux*ux + uy*uy - sn*sn;
	B = -(2*ux*cx + 2*uy*cy + 2*R*sn);
    C = cx*cx + cy*cy - R*R;
	
	double D = B*B - 4.0*A*C;
	
	if (D < 0)
	{
		rc = false;
	}
	else
	{
		t1 = (-B + sqrt(D))/(2*A);
		t2 = (-B - sqrt(D))/(2*A);
		rc = true;
	}
	
	return rc;
}

//-----------------------------------------------------------------------------

// It is assumed without checking in the following routine that the
// two lines or their extensions do interesect at a point P.  
// This routine finds orthogonal unit vectors U and V that point in the 
// directions of the lines that are the bisectors of the angles between 
// L1 and L2.  In particular U will be constructed in the quadrant that
// contains L1B and L2B.  It is also assumed that all points are in
// the XY plane, so all z coordinates will be assumed to be zero on input
// and set to zero for the output.
bool FindBisectors(sds_point L1A,   // I:  start of first line
				   sds_point L1B,   // I:    end of first line
				   sds_point L2A,   // I:  start of second line
				   sds_point L2B,   // I:    end of second line
				   sds_point U,     // O:  unit vector in first bisection direction
				   double & thetaU, // O:  angle that U makes with L2
				   sds_point V,     // O:  unit vector in second bisection direction
				   double & thetaV) // O:  angle that V makes with L2
{
	bool retcode;
	// compute L1B - L1A and normalize it
	// compute L2B - L2A and normalize it
	// find the midpoint of these normalized vectors and, if it is not zero, normalize it.

	// This could be done more concisely either by using existing routines from GEO
	// or IntelliCAD if they were accessible from here.
	sds_point NormalizedL1Dir;
	sds_point NormalizedL2Dir;

	NormalizedDifference2D(L1A, L1B, NormalizedL1Dir);
	NormalizedDifference2D(L2A, L2B, NormalizedL2Dir);

    NormalizedAverage2D(NormalizedL1Dir, NormalizedL2Dir, U);

	// Now just construct V to be perpendicular to U (choice of direction is arbitrary)
	V[0] = -U[1];
	V[1] = U[0];
	V[2] = 0.0;

	if ( Norm2D(U[0], U[1]) < pointtol)
	{
        thetaU = thetaV = 0.0;
		retcode = false;

	}
	else
	{
		// the angle between U and L2 is the arc cosine of the dot product
		// since we are dealing with normalized vectors here
		double cs = NormalizedL2Dir[0]*U[0] + NormalizedL2Dir[1]*U[1];
		thetaU = acos(cs);

		// the angle between V and L2 is the complement of the angle
		// between U and L2 since U and V are orthogonal
		double pi_over_two = 2*atan(1.0);
        thetaV = pi_over_two - thetaU;
		retcode = true;
	}

	return retcode;
}

//-----------------------------------------------------------------------------

// This routine finds the tangent circle that best matches the user's pick
// points for the Line-Line-Circle case.
void scoreLLC(int n,           // I:  number of input circles to test
			  bool * success,  // I: Only consider the i-th circle if success[i] = true
			  CCircle * C,      // I:  The array of candidate tangent circles
			  sds_point L1A,   // I:  start of 1st input line
			  sds_point L1B,   // I:    end of 1st input line
			  sds_point picL1, // I:  the pick point on the 1st input line
			  sds_point L2A,   // I:  start of 2nd input line
			  sds_point L2B,   // I:    end of 2nd input line
			  sds_point picL2, // I:  the pick point on the 2nd input line
			  sds_point cen,   // I: the center of the input circle
			  double rad,      // I: the radius of the input circle
			  sds_point picC,  // I:  the pick point on the input circle
			  int & best_index // I: the index of the best tangent circle (-1 if none found)
			  )
{
				// Now that we have constructed the (up to) eight
			// possible candidate circles, let's find the
			// tangent points that they make with L1, L2 and
			// the input circle.
			
			sds_point tanL1[8]; // tanL1[k] is where C[k] is tan to L1
			sds_point tanL2[8]; // tanL2[k] is where C[k] is tan to L2
			sds_point tanC[8];  // tanC[k] is where C[k] is tan to input circle

			CCircle InputCircle(cen[0], cen[1], rad);

			for (int k = 0; k < n; k++)
			{   // begin k-loop
				if (success[k] == true)
				{   // begin finding tangent points for k-th circle

					// the tangent points on the lines are the projections of
					// the center of the tangent circle onto those lines
					sds_point CT;
					CT[0] = C[k].x;
					CT[1] = C[k].y;
					CT[2] = 0.0;

					double dist; // not used

					ic_ptlndist(CT, L1A, L1B, &dist, tanL1[k]);
					ic_ptlndist(CT, L2A, L2B, &dist, tanL2[k]);
					tanL1[k][2] = tanL2[k][2] = 0.0;

					
					// now find the tangent point of the tangent circle with
					// the input circle
					FindTangentPoint(InputCircle, C[k], tanC[k][0], tanC[k][1]);
					tanC[k][2] = 0.0;
				}   //   end finding tangent points for k-th circle
			}   //   end k=loop

			// Now that we have all the data, we will find which one of the
			// (up to) 8 candidates comes closest to the user's pick points

			// initialize 
			best_index = -1;
			bool found_a_good_candidate = false;
			double current_score = -1.0;
			double lowest_score = -1.0;

			for (k = 0; k < n; k++)
			{	// begin k-loop
				if (success[k] == true)
				{  // begin scoring k-th circle
					current_score = sds_distance(picL1, tanL1[k]) +
						            sds_distance(picL2, tanL2[k]) +
									sds_distance(picC,  tanC[k]);

					if (found_a_good_candidate == false)
					{
						found_a_good_candidate = true;
						lowest_score = current_score;
						best_index = k;
					}
					else
					{
						if (current_score < lowest_score)
						{
							lowest_score = current_score;
							best_index = k;
						}
					}
				}  //   end scoring k-th circle
			}   //   end k-loop
}

//-----------------------------------------------------------------------------

// This is the core routine to
// construct a circle to two parallel lines and one circle
// The checking that the lines are parallel has been done
// outside of this routine, and the relevant input has been
// prepared.  This routine will be called twice by CirTan2ParLines1Cir,
// once with a positive value for the radius, and once with a negative.
bool TwoParLines1CirCore(
						 double distL1L2, // I:  distance between the two lines L1 and L2
						 sds_point L1A, // I:  start of first line
						 sds_point L1B, // I: end of first line
						 sds_point projL1AontoL2, // I: projection of L1A onto L2
						 sds_point cen,   // I: center of input circle
						 double rad,      // I: radius of input circle
						 CCircle & Circ1,    // O: 1st tangent circle
						 CCircle & Circ2)    // O: 2nd tangent circle
{
	bool retcode = false;

	// We will parameterize the centerline between L1 and L2 as P(t) = Q + t*U
	// where Q is the midpoint of L1A and projL1AontoL2 and U is a unit vector
	// in the direction of L1A to L1B.

	// Then, we want to find those values of t, if any, such that P(t) is the
	// center of a circle tangent to L1 and L2 and the input circle.

	// This means that the following equation must be satisfied.

	// dist(P(t), cen) = distL1L2/2 + rad

	// So we will set up and solve the quadratic equation that is equivalent to the above.

	// If we have solutions, the radius of each output circle will be half the distance
	// between the two lines
	double radius = distL1L2/2;

	double sumOfRadii = radius + rad;

	// Find the Q and U that are needed for the centerline P(t) = Q + t*U
	sds_point Q;
	for (int i = 0; i < 3; i++)
		Q[i] = 0.5*(L1A[i] + projL1AontoL2[i]);

	sds_point U;
	NormalizedDifference2D(L1A, L1B, U);

	// So P(t) = (Q[0] + t*U[0], Q[1] + t*U[1])

	// The quadratic we are to solve for t is 
	// (Q[0] + t*U[0] - cen[0])^2 + (Q[1] + t*U[1] - cen[1])^2 = sumOfRadii^2
	// or equivalently
	// (t*U[0] + (Q[0]-cen[0]))^2 + (t*U[1] + (Q[1]-cen[1]))^2 = sumOfRadii^2

	// and it can be reformulated as At^2 + Bt + C = 0 
	// with the A, B and C as follows

	double A = U[0]*U[0] + U[1]*U[1];
	double B = 2*(U[0]*(Q[0] - cen[0])) + 2*(U[1]*(Q[1] - cen[1]));
	double C = (Q[0] - cen[0])*(Q[0] - cen[0]) +
		       (Q[1] - cen[1])*(Q[1] - cen[1]) -
			   sumOfRadii*sumOfRadii;

	// Now let's try to solve the quadratic At^2 + Bt + C = 0
	// where the roots are t1 and t2

	double D = B*B - 4.0*A*C;

	if (D < 0)
	{
		retcode = false;
	}
	else
	{   // begin constructing both circles 
		double t1, t2;
		t1 = (-B + sqrt(D))/(2*A);
		t2 = (-B - sqrt(D))/(2*A);

        Circ1.r = radius;
		Circ1.x = Q[0] + t1*U[0];
		Circ1.y = Q[1] + t1*U[1];

		Circ2.r = radius;
		Circ2.x = Q[0] + t2*U[0];
		Circ2.y = Q[1] + t2*U[1];

		retcode = true;
	}   //   end constructing both circles
	
	return retcode;
}

//-----------------------------------------------------------------------------


// Construct a circle to two parallel lines and one circle
// The checking that the lines are parallel has been done
// outside of this routine, and the relevant input has been
// prepared
bool CirTan2ParLines1Cir(
						 double distL1L2, // I:  distance between the two lines L1 and L2
						 sds_point L1A, // I:  start of first line
						 sds_point L1B, // I: end of first line
						 sds_point L2A, // I: start of second line
						 sds_point L2B, // I: end of second line
						 sds_point projL1AontoL2, // I: projection of L1A onto L2
						 sds_point cen,   // I: center of input circle
						 double rad,      // I: radius of input circle
						 sds_point picL1, // I: pick point for first line
						 sds_point picL2, // I: pick point for second line
						 sds_point picC,  // I: pick point for circle
						 sds_point cenT,  // O: center of output tangent circle
						 double * radT)   // O: radius of output tangent circle
{
    bool retcode = false;
	bool rc;
	bool success[4];
	CCircle Circ[4];

    rc = TwoParLines1CirCore(distL1L2,
		                     L1A,
						     L1B,
						     projL1AontoL2,
						     cen,
						     rad,
						     Circ[0],
						     Circ[1]);

	success[0] = success[1] = rc;

	// now try it again with the negative radius
	double negrad;
	negrad = -1.0*rad;
    rc = TwoParLines1CirCore(distL1L2,
		                     L1A,
						     L1B,
						     projL1AontoL2,
						     cen,
						     negrad,
						     Circ[2],
						     Circ[3]);

	success[2] = success[3] = rc;

    // Now find i such that the tangent points of Circ[i]
	// best match the user's pick points

	int best_index;

	scoreLLC(4,
		     success,
			 Circ,
			 L1A,
			 L1B,
			 picL1,
			 L2A,
			 L2B,
			 picL2,
			 cen,
			 rad,
			 picC,
			 best_index);

	if (best_index >= 0)
	{
		cenT[0] = Circ[best_index].x;
		cenT[1] = Circ[best_index].y;
		cenT[2] = 0.0;
		*radT = Circ[best_index].r;
		retcode = true;
	}
	else
	{
		retcode = false;
	}
	
	return retcode;
}

//-----------------------------------------------------------------------------

// Construct a circle tangent to two lines and one circle
// The input lines and the input circle must all be in the X-Y plane
// The three pick points are used to determine which one of the potentially
// eight tangent circles will be selected
bool CirTan2Lines1Cir(
					  sds_point L1A,   // I:  start of first line
					  sds_point L1B,   // I:    end of first line
					  sds_point L2A,   // I:  start of second line
					  sds_point L2B,   // I:    end of second line
					  sds_point cen,   // I: center of input circle
					  double rad,      // I: radius of input circle
					  sds_point picL1, // I: pick point for first line
					  sds_point picL2, // I: pick point for second line
					  sds_point picC,  // I: pick point for circle
					  sds_point cenT,  // O: center of output tangent circle
					  double * radT)   // O: radius of output tangent circle
{
	bool retcode = false; // initialize to false, just in case we forget below.

	// Determine whether the the two lines (or their extensions) interesect, and
	// if so find their intersection point.

	sds_point P;

	int lxlcode = ic_linexline(L1A, L1B, L2A, L2B, P);

    if (lxlcode >= 0)
	{   // begin case where L1 and L2 intersect

		// we are going to generate all eight possible circles and then find the best one
		// we will make four calls to TwoLinesOneCircleCore, and we will get two circles
		// back from each of these calls.

        // these four calls will cover the cases where the radius is positive or negative
		// and will switch between the two possible values for the unit vectors in the
		// direction of the bisectors of the angle between L1 and L2.

		sds_point U[2];
		double theta[2];
		bool biscode = FindBisectors(L1A, L1B, L2A, L2B, U[0], theta[0], U[1], theta[1]);

		if (biscode == true)
		{   // begin case where we found the bisectors
			int i,j,k;
			double t1, t2;
			int sign;
			CCircle C[8]; // these are the eight candidate circles
			bool rc;
			bool success[8];  // these determine whether the k-th circle was constructed
			k = 0;
			sds_point tempcen;
			for (int m = 0; m < 3; m++)
				tempcen[m] = cen[m] - P[m];
			for (i = 0; i < 2; i++)
			{   // begin i-loop
				if (i == 0) 
					sign = 1;
				else
					sign = -1;
				for (j = 0; j < 2; j++)
				{  // begin j-loop
					rc = TwoLinesOneCircleCore(theta[j],
						                        U[j][0],
										        U[j][1],
										     tempcen[0],
										     tempcen[1],
										       sign*rad, 
										             t1,
										             t2);
					if (rc == true)
					{   // begin saving the circle
						C[k].x = P[0] + t1*U[j][0];
						C[k].y = P[1] + t1*U[j][1];
						C[k].r = t1*sin(theta[j]);
						success[k] = true;

						C[k+1].x = P[0] + t2*U[j][0];
						C[k+1].y = P[1] + t2*U[j][1];
						C[k+1].r = t2*sin(theta[j]);
						success[k+1] = true;
					}   //   end saving the circle
					else
					{
						success[k] = false;
						success[k+1] = false;
					}
					k += 2;
				}  //   end j-loop
			}   //   end i-loop

			int best_index;

			scoreLLC(8,
				     success,
					 C,
					 L1A,
					 L1B,
					 picL1,
					 L2A,
					 L2B,
					 picL2,
					 cen,
					 rad,
					 picC,
					 best_index);



			if (best_index >= 0)
			{  // begin packaging the output
				k = best_index;
				cenT[0] = C[k].x;
				cenT[1] = C[k].y;
				cenT[2] = 0.0;

				*radT = C[k].r;

				retcode = true;
			}  //   end packaging the output
			else
			{
				retcode = false;
			}
			
		}   //   end case where we found the bisectors
		else
		{   // begin case where we did not find the bisectors
            retcode = false;
		}   //   end case where we did not find the bisectors



	}   //   end case where L1 and L2 intersect
	else
	{   // begin case where L1 and L2 are parallel or degenerate

		if ((sds_distance(L1A, L1B) > pointtol) && (sds_distance(L2A, L2B) > pointtol))
		{   // begin case where neither line is degenerate
			sds_real distL1L2;
			sds_point projL1AontoL2;
			short distcode = ic_ptlndist(L1A, L2A, L2B, &distL1L2, projL1AontoL2);
			if (distL1L2 > pointtol)
			{  // begin case where we have distinct parallel lines
				retcode = CirTan2ParLines1Cir(distL1L2,
					                          L1A,
					                          L1B,
											  L2A,
											  L2B,
					                          projL1AontoL2,
					                          cen,
					                          rad,
					                          picL1,
					                          picL2,
					                          picC,
					                          cenT,
					                          radT);
			}  //   end case where we have distinct parallel lines
			else
			{  // begin case where we have overlapping parallel lines
				retcode = false;
			}  //   end case where we have overlapping parallel lines
		}   //   end case where neither line is degenerate
		else
		{   // begin case where at least one line is degenerate
			retcode = false;
		}   //   end case where at least one line is degenerate

	}   //   end case where L1 and L2 are parallel or degenerate

	return retcode;
}

//-----------------------------------------------------------------------------


// The following code is in support of the construction of a circle
// tangent to two given circles and a given line

enum tantype {internal_tangent, external_tangent};


//*****************************************************************************
//*****************************************************************************


class quadratic
{
	public:
		double m_A;
		double m_B;
		double m_C;
		
		// construct a quadratic equation of a parabola whose
		// directrix is a line of the form y = c;
		quadratic(C3Point focus,
			       double c,
				   bool & success);
		
		quadratic(double A,
			double B,
			double C);
		
		quadratic operator-(quadratic Z);
		
		void solve(int & num_roots, double & t1, double & t2);

        double eval(double t);

		C3Point EVAL(double t);
		

		friend void intersect(quadratic F, 
			                  quadratic G, 
							  int & num_points, 
							  C3Point & P1, 
							  C3Point & P2);

		friend double dist(C3Point p, quadratic F);

};

//-----------------------------------------------------------------------------

// the basic constructor
quadratic::quadratic(double A, double B, double C)
{
	m_A = A;
	m_B = B;
	m_C = C;
}

//-----------------------------------------------------------------------------

// the constructor given focus and directrix
// construct the quadratic expression for a parabola whose
// directrix is a line of the form y = c;
quadratic::quadratic(C3Point focus,
					 double c,
					 bool & success)
{
	double a = focus.X();
	double b = focus.Y();

	if (b == c)
		{
		success = false;
		}
	else
		{
		double denom = 2*b - 2*c;
		m_A = 1.0/denom;
		m_B = -2*a/denom;
		m_C = (a*a + b*b - c*c)/denom;
		success = true;
		}
}

//-----------------------------------------------------------------------------

// If Y and Z are quadratics then Y - Z is what
// you think it is.
quadratic quadratic::operator-(quadratic Z)
{
	return quadratic(m_A - Z.m_A, m_B - Z.m_B, m_C - Z.m_C);
}

//-----------------------------------------------------------------------------

// find the real roots, if any, of this quadratic
void quadratic::solve(int & num_roots, double & t1, double & t2)
{
	double A = m_A;
	double B = m_B;
	double C = m_C;
	
	double D = B*B - 4.0*A*C;
	
	// first check for the linear or almost linear case
	
	if (fabs(A) < FUZZ_DIST)
		{  // begin linear case
		if (fabs(B) < FUZZ_DIST)
			{
			num_roots = 0;
			}
		else
			{
			t1 = -C/B;
			t2 = -C/B;
			num_roots = 1;
			}
		
		}  //   end linear case
	else
		if (D < 0.0)
			{
			num_roots = 0;
			}
		else
			{
			num_roots = 2;
			double sqrtD = sqrt(D);
			t1 = (-B + sqrtD)/(2.0*A);
			t2 = (-B - sqrtD)/(2.0*A);
			}
}

//-----------------------------------------------------------------------------

// the evaluators
double quadratic::eval(double t)
{
	return m_A*t*t + m_B*t + m_C;
}

//-----------------------------------------------------------------------------

C3Point quadratic::EVAL(double t)
{
	   return C3Point(t, eval(t));
}

//-----------------------------------------------------------------------------

void intersect(quadratic F, 
			   quadratic G, 
			   int & num_points, 
			   C3Point & P1, 
			   C3Point & P2)
{
	quadratic H = F - G;
	double t1,t2;
	H.solve(num_points,t1,t2);
	if (num_points >= 1)
		P1 = F.EVAL(t1);
	if (num_points >= 2)
		P2 = F.EVAL(t2);
}
//*****************************************************************************
//*****************************************************************************


// This function returns the angle that
// a line in the XY plane makes with the
// X axis
double angle(CLine L)
{
	C3Point A;
	C3Point B;
	L.GetStartPoint(A);
	L.GetEndPoint(B);
	double dx = B.X() - A.X();
	double dy = B.Y() - A.Y();
	return atan2(dy,dx);
}

//-----------------------------------------------------------------------------

// This function returns the midpoint of
// the line L.
C3Point midpoint(CLine L)
{
	C3Point A;
	C3Point B;
	L.GetStartPoint(A);
	L.GetEndPoint(B);
	return 0.5*(A + B);
}

//-----------------------------------------------------------------------------

// construct the [at most] two circles that are tangent to two given
// circles and the line y = yval
void tangentCCL(const CEllipse C1,
				tantype tantype1,
				const CEllipse C2,
				tantype tantype2,
				double yval,
				int & numCircs,
				CEllipse & TC1,
				CEllipse & TC2)
{
	bool success1, success2;
	double r1, r2;
	
	if (tantype1 == external_tangent)
		r1 = C1.Radius();
	else
		r1 = -C1.Radius();
	
	if (tantype2 == external_tangent)
		r2 = C2.Radius();
	else
		r2 = -C2.Radius();
	
	
	// construct the parabola that is the locus of centers of
	// circles tangent to C1 and the line y = yval by specifying
	// its focus and directrix
	quadratic Parabola1(C1.Center(), yval - r1, success1);
	
	// construct the parabola that is the locus of centers of
	// circles tangent to C2 and the line y = yval by specifying
	// its focus and directrix
	quadratic Parabola2(C2.Center(), yval - r2, success2);
	
	if (success1 && success2)
		{
		
		// interesect the parabolas
		C3Point P1, P2;
		intersect(Parabola1, Parabola2, numCircs, P1, P2);
		
		// for each intersection point found, construct a tangent circle
		
		RC rc;
		
		if ((numCircs == 1) || (numCircs == 2))
			{
			TC1 = CEllipse(P1, fabs(P1.Y() - yval), rc);
			
			if ((numCircs == 2) && (rc == SUCCESS))
				TC2 = CEllipse(P2, fabs(P2.Y() - yval), rc);
			
			if (rc != SUCCESS)
				numCircs = 0;
			
			}
		}
	else
		{
		numCircs = 0;
		}
	
	
}

//-----------------------------------------------------------------------------

// construct the [at most] two circles that are tangent to two given
// circles and a given line
void tangentCCL(const CEllipse C1,
				tantype tantype1,
				const CEllipse C2,
				tantype tantype2,
				CLine L,
				int & numCircs,
				CEllipse & TC1,
				CEllipse & TC2)
{
double r1,r2;

// check the input (how much other checking should we do?)
if (!(C1.IsCircular(r1) || C2.IsCircular(r2)))
	{
	numCircs = 0;
	}
else
	{  // begin normal case
	
	
	// construct the transformation that rotates the line L
	// into a line of the form y = yconst
	
	C3Point Lmid = midpoint(L);
	double Lang = angle(L);
	
	CAffine R;
	
	R.RotateAbout(-Lang, Lmid);

	// apply this transform to the input circles
	CEllipse C1new = C1;
	CEllipse C2new = C2;
	
	C1new.Transform(R);
	C2new.Transform(R);

	// call the special case version of tangentCCL
	tangentCCL(C1new, 
			   tantype1, 
			   C2new, 
			   tantype2, 
			   Lmid.Y(), 
			   numCircs, 
			   TC1, 
			   TC2);

	// construct the inverse transformation
	CAffine Rinv;

	Rinv.RotateAbout(Lang, Lmid);

	// use this inverse to transform the circles back
	// where they should be

	TC1.Transform(Rinv);
	TC2.Transform(Rinv);

	}  //   end normal case
}

//-----------------------------------------------------------------------------

// construct all possible circles that are tangent to two given
// circles and a given line
void tangentCCL(const CEllipse C1,
				const CEllipse C2,
				CLine L,
				bool success[8], //success[i] = true iff TC[i] is constructed
				CEllipse  TC[8])
{
int i,j,k;
tantype tantype1, tantype2;
int numCircs;
for (i = 0; i < 4; i++)
	{  // begin i-loop
	j = 2*i;
	k = j + 1;

	if (i < 2)
		tantype1 = external_tangent;
	else
		tantype1 = internal_tangent;

	if ((i % 2) == 0)
		tantype2 = external_tangent;
	else
		tantype2 = internal_tangent;

	tangentCCL(C1,tantype1,C2,tantype2,L,numCircs,TC[j],TC[k]);

	switch (numCircs)
		{  // begin switch (numCircs)
		case 0:
			success[j] = false;
			success[k] = false;
			break;
		case 1:
			success[j] = true;
			success[k] = false;
			break;
		case 2:
			success[j] = true;
			success[k] = true;
			break;
		default:
			success[j] = false;
			success[k] = false;
			break;
		}  //   end switch (numCircs)



	}  //   end i-loop
}

//-----------------------------------------------------------------------------

// returns 0 if C is not a circle
// or if it is not tangent to L
// returns 1 if C and L are tangent

// the tangent point T is not required to be on the "active" portion
// of the line L, it can be anywhere along the bi-infinite extension
// of the line.
int findTangentPoint(CEllipse C,
					 CLine L,
					 C3Point & T) // point where C and L are tangent if retval = 1
{
	int retval = 0;
	
	double radius;
	
	if (C.IsCircular(radius))
		{  // begin case where C is a circle
		C3Point P = C.Center();
		double rAt;
		double rDist;

		// build the unbounded line that is the bi-infinite extension of L
		CUnboundedLine U(&L);

		BOOL bIgnoreStart = FALSE;
		U.NearestPoint(P, T, rAt, rDist, bIgnoreStart);
		if ( fabs(rDist - radius) < FUZZ_GEN)
			retval = 1;
		else
			retval = 0;
		}  //   end case where C is a circle
	else
		{  // begin case where C is not a circle
		retval = 0;
		}  //   end case where C is not a circle
	
	return retval;
}

//-----------------------------------------------------------------------------

// returns 0 if C and D are not both circles
// or if they coincide
// or if they are not tangent
// returns 1 if C and D are tangent
int findTangentPoint(CEllipse C,
					 CEllipse D,
					 C3Point & T) // point where C and D are tangent if retval = 1
{
   double r;
   double s;
   int retval = 0;
   
   if (C.IsCircular(r) && D.IsCircular(s))
	   {  // begin case where both are circles
	   C3Point P = C.Center();
	   C3Point Q = D.Center();
	   double d = PointDistance(P,Q);
	   C3Point U;
	   
	   if (d < FUZZ_GEN)
		   {
		   retval = 0;
		   }
	   else
		   {
		   U = Q - P;
		   U.Unitize();
		   }
	   
	   if ( fabs(d - (r+s)) < FUZZ_GEN)
		   {  // begin case where C and D are tangent on the outside
		   T = P + r*U;
		   retval = 1;
		   }  //   end case where C and D are tangent on the outside
	   else if ( (r > s) && fabs(d - (r - s)) < FUZZ_GEN)
		   {  // begin case where D is contained in C and tangent to C
		   T = P + r*U;
		   retval = 1;
		   }  //   end case where D is contained in C and tangent to C
	   else if ( (s > r) && fabs(d - (s - r)) < FUZZ_GEN)
		   {  // begin case where C is contained in D and tangent to D
		   T = P - r*U;
		   retval = 1;
		   }  //   end case where C is contained in D and tangent to D
	   }  //   end case where both are circles
   else
	   {  // begin case where not both are circles
	   retval =  0;
	   }  //   end case where not both are circles
   
   return retval;
}

//-----------------------------------------------------------------------------

// Given two circles, one straight line, and the corresponding pick points
// on each, and given one circle that is tangent to the three input
// curves, find a measure of the closeness of fit of this tangent circle.
// A negative score will not be counted toward the best score.
double scoreCCL(const CEllipse C1,
				C3Point PicC1,
				const CEllipse C2,
				C3Point PicC2,
				CLine L,
				C3Point PicL,
				CEllipse TC)
{
	double score;
	
	C3Point TPC1, TPC2, TPL;
	int found1, found2, found3;
	found1 = findTangentPoint(TC, C1, TPC1);
	found2 = findTangentPoint(TC, C2, TPC2);
	found3 = findTangentPoint(TC,  L, TPL);
	
	if (found1 && found2 && found3)
		{
		score = PointDistance(PicC1,TPC1) + 
			    PointDistance(PicC2,TPC2) + 
				PointDistance(PicL,TPL);
		}
	else
		{
		score = -1;
		}
	
	
	return score;
}

//-----------------------------------------------------------------------------

// Given two circles, one straight line, and the corresponding pick points
// on each, and an array of all circles that are tangent to the three input
// curves, find the index of the circle whose tangent points best match the
// pick points
void bestCCL(const CEllipse C1,
			 C3Point PicC1,
			 const CEllipse C2,
			 C3Point PicC2,
			 CLine L,
			 C3Point PicL,
			 bool success[8],
			 CEllipse TC[8],
			 int & index_of_best // -1 if nothing found
			 )
{
	
	index_of_best = -1; // initialize until we find at least one
	double curr_score;
	double lowest_score = -1.0; // initialize until we find at least one
	
	for (int i = 0; i < 8; i++)
		{   // begin i-loop
		if (success[i] == true) // i.e., TC[i] was constructed
			{  // begin case of success[i] == true
			curr_score = scoreCCL(C1,PicC1,C2,PicC2,L,PicL,TC[i]);
			
			if ((curr_score >= 0.0) && (lowest_score < 0.0))
				{
				lowest_score = curr_score;
				index_of_best = i;
				}
			else
				if ((curr_score >= 0.0) && (curr_score < lowest_score))
					{  // begin updating lowest_score and index_of_best
					lowest_score = curr_score;
					index_of_best = i;
					}  //   end updating lowest_score and index_of_best
			}   //   end case of success[i] == true
		}   //  end i-loop
}

//-----------------------------------------------------------------------------

// Given two circles, one straight line, and the corresponding pick points,
// construct a circle that is tangent to the three input curves, and, if there
// is more than one such circle, construct the one whose tangent points best
// match the pick points
void tangentCCL(const CEllipse C1,
				C3Point PicC1,
				const CEllipse C2,
				C3Point PicC2,
				CLine L,
				C3Point PicL,
				bool & found, // found = true iff we can find a tangent circle
				CEllipse  & TanCirc)
{
	bool success[8];
	CEllipse TC[8];

	// Find all possible tangent circles
	tangentCCL(C1, C2, L, success, TC);

	int index_of_best = -1;
	// Use the pick point information to select the best one
	bestCCL(C1, PicC1, C2, PicC2, L, PicL, success, TC, index_of_best);

	// should probably change 8 to a constant that is global
	if ((0 <= index_of_best) && (index_of_best < 8))
		{
		TanCirc = TC[index_of_best];
		found = true;
		}
	else
		{
		found = false;
		}
}

//-----------------------------------------------------------------------------

// Construct a circle tangent to two circles and one line.
bool CirTan2Circles1Line(
						 sds_point cen1,  // I:  center of 1st input circle
						 double r1,       // I:  radius of 1st input circle
                         sds_point cen2,  // I:  center of 2nd input circle
						 double r2,       // I:  radius of 2nd input circle
						 sds_point LA,    // I:  start of input line
						 sds_point LB,    // I:  end of input line
						 sds_point pikC1, // I:  pick point for 1st input circle
						 sds_point pikC2, // I:  pick point for 2nd input circle
						 sds_point pL,    // I:  pick point for input line
						 sds_point cenT,  // O:  center of output tangent circle
                         double * radT    // O:  radius of output tangent circle
						 )
{
	bool found = false;
	
    C3Point PicC1;
    C3Point PicC2;
	
    C3Point PicL;
    CEllipse TanCirc;
	
	RC rc1;
	RC rc2;
	
	// convert the input data to Geo
	C3Point ptCenter1;
	C3Point ptCenter2;
	
	SdsToRpt(cen1, ptCenter1);
	CEllipse C1(ptCenter1, r1, rc1);
	
	SdsToRpt(cen2, ptCenter2);
	CEllipse C2(ptCenter2, r2, rc2);
	
	C3Point A;
	C3Point B;
	
	SdsToRpt(LA, A);
	SdsToRpt(LB, B);
	
	CLine L(A,B);
	
	SdsToRpt(pikC1, PicC1);
	SdsToRpt(pikC2, PicC2);
	SdsToRpt(pL,PicL);
	
    if ((rc1 == SUCCESS) && (rc2 == SUCCESS))
		{
		
		// call the Geo-based function
		tangentCCL(C1,
			PicC1,
			C2,
			PicC2,
			L,
			PicL,
			found, // found = true iff we can find a tangent circle
			TanCirc);
		
		// convert the results back to the sds form
		if (found == true)
			{  
			if (TanCirc.IsCircular(*radT))
				{
				C3Point center = TanCirc.Center();
				RptToSds(center,cenT);
				}
			else
				{
				found = false;
				}
			
			}
		}
	else
		{
		found = false;
		}
	
	
	return found;
}

//-----------------------------------------------------------------------------

// this function was originally in circle.cpp as in-line code inside another function
// 
// Construct a circle tangent to three lines, where the location of the circle
// is determined by the three pick points.

bool CirTan3Lines(sds_point p1,     // I:  start of 1st line
				  sds_point p2,     // I:  end of 1st line				  
				  sds_point lp1,    // I:  pick pt of 1st line
				  sds_point p3,     // I:  start of 2nd line
				  sds_point p4,     // I:  end of 2nd line
				  sds_point lp2,    // I:  pick pt of 2nd line
				  sds_point p5,     // I:  start of 3rd line
				  sds_point p6,     // I:  end of 3rd line
				  sds_point lp3,    // I:  pick pt of 3rd line
				  sds_point newcen, // O: center of tangent circle
				  double * newrad)  // O: radius of tangent circle
{  // begin LLL case
	bool bFlipU = FALSE, bFlipV = FALSE;
	sds_point p7;
	sds_point A, B, C, P, Q, M, N;
	sds_point u,v;
	int err;
	bool bANotFound, bBNotFound, bCNotFound, bRecalc;
	sds_real fr1;
	bool done;
	
recalc_intersections:
	bANotFound = FALSE;
	bBNotFound = FALSE;
	bCNotFound = FALSE;
	bRecalc = FALSE;
	err = 0;
	if (-1==ic_linexline(p1, p2, p3, p4, A))
		{
		err++;
		bANotFound = TRUE;
		}
	if (-1==ic_linexline(p3, p4, p5, p6, B))
		{
		err++;
		bBNotFound = TRUE;
		}
	if (-1==ic_linexline(p5, p6, p1, p2, C))
		{
		err++;
		bCNotFound = TRUE;
		}
	if (err > 1) //we need at least 2 of the lines to intersect.
		return FALSE;
	
	if (bANotFound || bBNotFound)	//get it to the form where C is the only unfound xn.
		{
		sds_point tmp1, tmp2;
		ic_ptcpy (tmp1, p1);
		ic_ptcpy (tmp2, p2);
		ic_ptcpy (p1, p3);
		ic_ptcpy (p2, p4);
		ic_ptcpy (p3, p5);
		ic_ptcpy (p4, p6);
		ic_ptcpy (p5, tmp1);
		ic_ptcpy (p6, tmp2);
		goto recalc_intersections;
		}
	
	if (bCNotFound) ic_ptcpy (C, p1);
	ic_ptlndist(lp1,A,C,&fr1,lp1);
	ic_ptlndist(lp2,A,B,&fr1,lp2);
	
	//Determine if the vector CA needs to be flipped, based on pick point lp1
	ic_sub (A, lp1, u);
	ic_sub (A, C, v);
	if (ic_dotproduct(u,v) < 0)
		bFlipU = TRUE;
	
	//Determine if the vector BA needs to be flipped, based on pick point lp2
	ic_sub (A, lp2, u);
	ic_sub (A, B, v);
	if (ic_dotproduct(u,v) < 0)
		bFlipV = TRUE;
	
	if (bFlipU)
		ic_unitvec (A, lp1, u);
	else
		ic_unitvec (A, C, u);
	if (bFlipV)
		ic_unitvec (A, lp2, v);
	else
		ic_unitvec (A, B, v);
	ic_add (A, u, P);
	ic_add (A, v, Q);
	ic_add (P, Q, M);
	M[0] /= 2.0; M[1] /= 2.0; M[2] /=2.0;
	
	bFlipU = FALSE;
	bFlipV = FALSE;
	if (bCNotFound) ic_ptcpy (C, p6);
	ic_ptlndist(lp3,C,B,&fr1,lp3);
	
	//Determine if the vector CB needs to be flipped, based on pick point lp3
	ic_sub (B, lp3, u);
	ic_sub (B, C, v);
	if (ic_dotproduct(u,v) < 0)
		bFlipU = TRUE;
	
	//Determine if the vector AB needs to be flipped, based on pick point lp2
	ic_sub (B, lp2, u);
	ic_sub (B, A, v);
	if (ic_dotproduct(u,v) < 0)
		bFlipV = TRUE;
	
	if (bFlipU)
		ic_unitvec (B, lp3, u);
	else
		ic_unitvec (B, C, u);
	if (bFlipV)
		ic_unitvec (B, lp2, u);
	else
		ic_unitvec (B, A, v);
	ic_add (B, u, P);
	ic_add (B, v, Q);
	ic_add (P, Q, N);
	N[0] /= 2.0; N[1] /= 2.0; N[2] /=2.0;
	
	if (-1==ic_linexline(A, M, B, N, newcen))
		return FALSE;
	
	if(-1==ic_ptlndist(newcen,A,B,newrad,p7)) 
		return FALSE;
	
	done = TRUE;
	return done;
}  // end LLL case
						

//-----------------------------------------------------------------------------
// This routine scores one tangent circle as a function of the user's pick
// points for the Line-Line-Line case.
double scoreLLL(sds_point L1A,   // I:  start of 1st input line
			    sds_point L1B,   // I:    end of 1st input line
			    sds_point picL1, // I:  the pick point on the 1st input line
			    sds_point L2A,   // I:  start of 2nd input line
			    sds_point L2B,   // I:    end of 2nd input line
			    sds_point picL2, // I:  the pick point on the 2nd input line
                sds_point L3A,   // I:  start of 2nd input line
			    sds_point L3B,   // I:    end of 2nd input line
			    sds_point picL3, // I:  the pick point on the 2nd input line
			    sds_point cen    // I: the center of the tangent circle
                                 // !: we don't need the radius
			   )
{

	sds_point tanL1;  // tanL1 is where the tangent circle is tan to L1
	sds_point tanL2;  // tanL2 is where the tangent circle is tan to L2
	sds_point tanL3;  // tanL3 is where the tangent circle is tan to L3

	double dist; // not used
	double score; // the return value
	
	// since it is assumed that we have a tangent circle, we can indeed
	// use ic_ptlndist to find the tangent points on the three lines
	ic_ptlndist(cen, L1A, L1B, &dist, tanL1);
	ic_ptlndist(cen, L2A, L2B, &dist, tanL2);
	ic_ptlndist(cen, L3A, L3B, &dist, tanL3);
	
	
	
	score = sds_distance(picL1, tanL1) +
		    sds_distance(picL2, tanL2) +
		    sds_distance(picL3, tanL3);
	
	return score;
}

// construct a circle tangent to three lines, the first two of which are parallel
int CircTanParaLLL(line_or_circle lorc1, 
				   line_or_circle lorc2,
				   line_or_circle lorc3,
				   sds_point newcen,
				   double * newrad)
{
	int rc = 0;
	// just making sure :)
	ASSERT(lorc1.isParallelTo(lorc2));
	
	sds_point L1A, L1B, picL1;
	sds_point L2A, L2B, picL2;
	sds_point L3A, L3B, picL3;
	
	lorc1.get_start_pt(L1A);
	lorc1.get_end_pt(L1B);
	lorc1.get_pick_pt(picL1);
	
	lorc2.get_start_pt(L2A);
	lorc2.get_end_pt(L2B);
	lorc2.get_pick_pt(picL2);
	
	lorc3.get_start_pt(L3A);
	lorc3.get_end_pt(L3B);
	lorc3.get_pick_pt(picL3);
	
	double dist;
	sds_point projL1A, projL1B;
	ic_ptlndist(L1A, L2A, L2B, &dist, projL1A);
	ic_ptlndist(L1B, L2A, L2B, &dist, projL1B);
	*newrad = dist/2;
	
	sds_point U,V;
	double thetaU, thetaV;
	FindBisectors(L3A, L3B,
		L2A, L2B,
		U, thetaU,
		V, thetaV);
	sds_point xpt;
	if (ic_linexline(L2A, L2B, L3A, L3B, xpt) >= 0)
		{  // begin normal case
		sds_point LmidA, LmidB;
		
		// The line determined by LmidA and LmidB is halfway
		// between the parallel lines L1 and L2
		ic_midpoint(L1A, projL1A, LmidA);
		ic_midpoint(L1B, projL1B, LmidB);
		
		sds_point upt, vpt;
		
		// the line determined by xpt and upt is a bisector of the
		// angle between L2 and L3
		ic_add(xpt, U, upt);
		
		// the line determined by xpt and vpt is the other bisector
		// of the angle between L2 and L3
		ic_add(xpt, V, vpt);
		
        // finally we are ready to construct the two possible 
		// tangent circles
		sds_point cen1;
		sds_point cen2;
		
		if ((ic_linexline(xpt, upt, LmidA, LmidB, cen1)>= 0) && 
			(ic_linexline(xpt, vpt, LmidA, LmidB, cen2)>= 0))
			{
			double score1 = scoreLLL(L1A,L1B,picL1,
				L2A,L2B,picL2,
				L3A,L3B,picL3,
				cen1);

			double score2 = scoreLLL(L1A,L1B,picL1,
				L2A,L2B,picL2,
				L3A,L3B,picL3,
				cen2);

			if (score1 < score2)
				ic_ptcpy(newcen, cen1);
			else
				ic_ptcpy(newcen, cen2);

			rc = 1;
			}
		else
			{
			rc = 0;
			}
		
		
		}  //  end normal case
	else
		{  // begin abnormal case, which should not happen!!!
		rc = 0;
		}  //   end abnormal case, which should not happen!!!
	
	
	return rc;
}

static
void get_nearest_pt
(
const point& r, 
const gvector& d,
const point& p,
point& x
);

static
int solve_linear2x2(double a11, double a12, double a21, double a22, double b1, double b2,
		   double* x1, double* x2);

static
int solve_quadratic(double a, double b, double c, double* t1, double* t2);

static
void getTangentPoints
(
const point& c,
double r,
const point& s1,
const gvector& dir1,
const point& s2,
const gvector& dir2,
point* tanp21,
point* tanp22
);

bool CircTan2Lines(sds_point _pnt,
				   line_or_circle lorc1, 
				   line_or_circle lorc2,
				   sds_point newcen,
				   double * newrad)
{
	point pnt(_pnt);
	// are the lines parallel
	point s1;
	lorc1.get_start_pt(s1);
	point e1;
	lorc1.get_end_pt(e1);

	if (s1 == e1)
		return 0;
	gvector dir1 = e1 - s1;

	point s2;
	lorc2.get_start_pt(s2);
	point e2;
	lorc2.get_end_pt(e2);

	if (s2 == e2)
		return 0;
	gvector dir2 = e2 - s2;

	if (parallel(dir1, dir2))
	{
	}
	else
	{
		// lines are not parallel

		// construct new coordinate system, 
		// origin is in intersetion point,
		// xaxis is bisector of angle, formed by lines

		// find intersection of endless straights
		point origin;
		{
			gvector dif = s2 - s1;
			double t, s;
			if (!solve_linear2x2(dir1.x(), -dir2.x(), dir1.y(), -dir2.y(), 
				dif.x(), dif.y(), &t, &s))
				return 0;

			origin = s1 + t*dir1;
		}
		
		gvector xaxis;
		{
			// find angle, where point lies
			gvector pdir = pnt - origin;

			double a1, a2;
			if (!solve_linear2x2(dir1.x(), dir2.x(), dir1.y(), dir2.y(),
				pdir.x(), pdir.y(), &a1, &a2))
				return 0;

			if (a1 < 0.)
				dir1 *= -1.;
			if (a2 < 0)
				dir2 *= -1.;

			point p = origin + (dir1.normalized() +
								dir2.normalized());
			xaxis = p - origin; xaxis.normalize();
		}

		double dx = origin.x() - pnt.x();
		double dy = origin.y() - pnt.y();
		double c1 = (xaxis * dir1.normalized()).norm2(); c1 -= 1.;
		double c2 = -2.*(xaxis.x()*dx + xaxis.y()*dy);
		double c3 = -dx*dx -dy*dy;

		// solve equation
		double t1, t2;
		if (!solve_quadratic(c1, c2, c3, &t1, &t2))
			return 0;

		// determine root we need
		{
			point center1 = origin+xaxis*t1;
			point center2 = origin+xaxis*t2;

			(pnt - center1).norm();

			double rd1 = (pnt - center1).norm();
			double rd2 = (pnt - center2).norm();

			point tanp11, tanp12, tanp21, tanp22;
			{
				get_nearest_pt(s1, dir1, center1, tanp11);
				get_nearest_pt(s2, dir2, center1, tanp12);
				get_nearest_pt(s1, dir1, center2, tanp21);
				get_nearest_pt(s2, dir2, center2, tanp22);
			}
#if 0
			getTangentPoints(center1,  rd1, s1, dir1, s2, dir2, &tanp11, &tanp12);
			getTangentPoints(center2,  rd2, s1, dir1, s2, dir2, &tanp21, &tanp22);
#endif
			// find "best" circle

			point pick1;
			lorc1.get_pick_pt(pick1);
			point pick2;
			lorc2.get_pick_pt(pick2);

			double f1 = (tanp11 - pick1).norm() + (tanp12 - pick2).norm();
			double f2 = (tanp21 - pick1).norm() + (tanp22 - pick2).norm();

			if (f1 < f2)
			{
				ic_ptcpy(newcen , center1);
				*newrad = rd1;
			}
			else
			{
				ic_ptcpy(newcen, center2);
				*newrad = rd2;
			}
		}
	}
	return 1;
}

static
void get_nearest_pt
(
const point& c,
double r,
const point& p,
point& x
);

int CircTanLineCircle(sds_point _pnt,
				   line_or_circle lorc1, 
				   line_or_circle lorc2,
				   sds_point newcen,
				   double* newrad)
{
	// point
	point pnt(_pnt);

	// straight
	point s;
	lorc1.get_start_pt(s);
	point e;
	lorc1.get_end_pt(e);
	gvector d = e - s; d.normalize();

	// circle
	point c;
	lorc2.get_center(c);
	double r;
	lorc2.get_radius(r);

	// Step 1.
	// Find closest point on line from pnt
	point x;
	get_nearest_pt(s, d, pnt, x);

	// Step 2. Change coordinate system
	// Transfer origin of coordinate system to x
	transf shift = translation(point(0.,0.,x.z())-x);

	// straight line will be xaxis
	// double a = atan2(d.y(), d.x());  Commented out by Stephen Hou to fix bug, 9/17/203

	// the picked point will be on the positive direction of Y 
	// axis. The algorithm implemented below is based on this 
	// assumption. Stephen Hou, 9/17/203
	//
	gvector pickPointDir = pnt - x; pickPointDir.normalize();
	gvector yAxis(0.0, 1.0, 0.0), zAxis(0.0, 0.0, 1.0);
	double a = acos(pickPointDir % yAxis);
	if (!icadIsZero(a)) {
		if (((yAxis * pickPointDir) % zAxis) < 0.0)
			a *= -1.0;
	}

	transf rot = rotation(-a, gvector(0., 0., 1.));

	transf t = shift*rot;

	point pt = pnt*t;
	point nc = c*t;

	double c1 = 1. - (nc.y()+r)/pt.y();
	double c2 = -2.*nc.x();
	double c3 = nc.x()*nc.x() + (nc.y()+r)*(nc.y()-r-pt.y());

	double t1, t2;
	if (!solve_quadratic(c1, c2, c3, &t1, &t2))
		return 0;

	point pt1(t1, 0., x.z());
	point pt2(t2, 0., x.z());

	double r1 = (pt1.x()*pt1.x() + pt.y()*pt.y())/(2.*pt.y());
	double r2 = (pt2.x()*pt2.x() + pt.y()*pt.y())/(2.*pt.y());

	point center1 = pt1 + gvector(0.,1.,x.z())*r1;
	point center2 = pt2 + gvector(0.,1.,x.z())*r2;

	t.inverse();
	center1 *= t;
	center2 *= t;

	point tanp11, tanp12, tanp21, tanp22;
	{
		get_nearest_pt(s, d, center1, tanp11);
		get_nearest_pt(c, r, center1, tanp12);
		get_nearest_pt(s, d, center2, tanp21);
		get_nearest_pt(c, r, center2, tanp22);
	}

	// find "best" circle

	point pick1;
	lorc1.get_pick_pt(pick1);
	point pick2;
	lorc2.get_pick_pt(pick2);

	double f1 = (tanp11 - pick1).norm() + (tanp12 - pick2).norm();
	double f2 = (tanp21 - pick1).norm() + (tanp22 - pick2).norm();

	if (f1 < f2)
	{
		ic_ptcpy(newcen , center1);
		*newrad = r1;
	}
	else
	{
		ic_ptcpy(newcen, center2);
		*newrad = r2;
	}

	return 1;
}

struct tan_circle
{
	point m_center;
	double m_radius;
	double m_factor;
};

int CircTan2Circles(sds_point pnt,
					line_or_circle lorc1, 
					line_or_circle lorc2,
					sds_point newcen,
					double* newrad)
{
	// Prepare initial data

	// Change coordinate system:
	// bothe circles lies on x axis, pnt lies on yaxis
	point c1, c2;
	lorc1.get_center(c1);
	lorc2.get_center(c2);

	point pick1, pick2;
	lorc1.get_pick_pt(pick1);
	lorc2.get_pick_pt(pick2);

	point p(pnt);

	// find new origin of coordinate system
	point o;
	get_nearest_pt(c1, c2-c1, p, o);

	gvector xaxis = c2-o; xaxis.normalize();
	gvector yaxis(-xaxis.y(), xaxis.x(), xaxis.z());

	transf t = coordinate(o, xaxis, yaxis);

	// transform all data
	c1 *= t; c2 *= t; p *= t;
	pick1 *= t; pick2 *= t;

	t.inverse();

	if (!icadIsZero(p.x()) || !icadIsZero(c1.y()) || !icadIsZero(c2.y()))
		return 0;

	// initial radiuses
	double r1, r2;
	lorc1.get_radius(r1); lorc2.get_radius(r2);

	vector<tan_circle> solutions;
	int s[2][4] = 
	{	{+1, -1, +1, -1},
		{+1, -1, -1, +1}};
	
	for (int i = 0; i < 4; i++)
	{
		tan_circle c;

		if (fabs(p.y()) < resabs())
		{
			// solve linear system of 2 equations with 2 unknowns
			double a11 = -2.*c1.x();
			double a21 = -2.*c2.x();
			double a12 = -2.*s[0][i]*r1;
			double a22 = -2.*s[1][i]*r2;

			double b1 = -c1.x()*c1.x() + r1*r1;
			double b2 = -c2.x()*c2.x() + r2*r2;

			double x, r;
			if (!solve_linear2x2(a11, a12, a21, a22, b1, b2, &x, &r))
				continue;
			if (r < 0.)
				continue;

			point contact1, contact2;
			c.m_radius = r;

			double y = sqrt(r*r - x*x);
			
			c.m_center.set(x, y, c1.z());

			contact1 = c1 + s[0][i]*r1*(c.m_center - c1).normalized();
			contact2 = c2 + s[1][i]*r2*(c.m_center - c2).normalized();
			c.m_factor = (pick1-contact1).norm2() + (pick2-contact2).norm2();

			c.m_center *= t;
			solutions.push_back(c);

			c.m_center.set(x, -y, c1.z());
			
			contact1 = c1 + s[0][i]*r1*(c.m_center - c1).normalized();
			contact2 = c2 + s[1][i]*r2*(c.m_center - c2).normalized();
			c.m_factor = (pick1-contact1).norm2() + (pick2-contact2).norm2();

			c.m_center *= t;
			solutions.push_back(c);
		}
		else
		{
			double b1 = (s[0][i]*r1-s[1][i]*r2)/(c2.x()-c1.x());
			double b2 = (c2.x()*s[0][i]*r1-c1.x()*s[1][i]*r2)/(p.y()*(c2.x()-c1.x()));

			double a1 = 0.5*(c2.x()+c1.x() + (r1*r1- r2*r2)/(c2.x()-c1.x())) ;
			double a2 = 0.5*((c2.x()*(r1*r1-c1.x()*c1.x()) - c1.x()*(r2*r2-c2.x()*c2.x()))/
						(p.y()*(c2.x()-c1.x())) - p.y());

			double A = b1*b1 + b2*b2 - 1;
			double B = 2*(a1*b1+a2*b2);
			double C = a1*a1+a2*a2;

			double r[2];
			if (!solve_quadratic(A, B, C, r+0, r+1))
				continue;
		
			for (int j = 0; j < 2; j++)
			{
				if (r[j] < 0.)
					continue;

				c.m_center = point(a1 + b1*r[j], a2 + p.y() + b2*r[j], c1.z());
				c.m_radius = r[j];

				point contact1 = c1 + s[0][i]*r1*(c.m_center - c1).normalized();
				point contact2 = c2 + s[1][i]*r2*(c.m_center - c2).normalized();

				c.m_factor = (pick1-contact1).norm2() + (pick2-contact2).norm2();

				c.m_center *= t;
				solutions.push_back(c);
			}
		}
	}

	double minf = solutions.front().m_factor;
	int mini = 0;
	for (i = 1; i < solutions.size(); i++)
	{
		if (solutions[i].m_factor < minf)
		{
			minf = solutions[i].m_factor;
			mini = i;
		}
	}

	*newrad = solutions[mini].m_radius;
	ic_ptcpy(newcen, solutions[mini].m_center);

	return 1;
}

// solve linear system 2x2
static
int solve_linear2x2(double a11, double a12, double a21, double a22, double b1, double b2,
		   double* x1, double* x2)
{
	// use Cramer's method
	double d = a11*a22 - a12*a21;

	if (fabs(d) < resnorm())
		return 0;

	double d1 = b1*a22 - a12*b2;
	double d2 = a11*b2 - b1*a21;

	*x1 = d1/d;
	*x2 = d2/d;

	return 1;
}

static
int solve_quadratic(double a, double b, double c, double* t1, double* t2)
{
	double d = b*b - 4.*a*c;
	if (d < 0.)
		return 0;

	d = sqrt(d);
	*t1 = (-b + d)/(2.*a);
	*t2 = (-b - d)/(2.*a);

	return 1;
}

static
void get_nearest_pt
(
const point& c,
double r,
const point& p,
point& x
)
{
	gvector d = p - c;
	if (d < resabs())
		x = c;
	else
		x = c + d.normalized()*r;
}

static
void get_nearest_pt
(
const point& r, 
const gvector& d,
const point& p,
point& x
)
{
	if (d < resabs())
	{
		x = r;
	}
	else
	{
		double d0 = d.x()*d.x() + d.y()*d.y();
		double d1 = (p.x()-r.x())*d.x() + (p.y()-r.y())*d.y();

		double t = d1/d0;

		x = r + t*d;
	}
}

static
void getTangentPoints
(
const point& c,
double r,
const point& s,
const gvector& d,
const point& c1,
double r1,
point& tanp11,
point& tanp12)
{
	// process line
}

static
void getTangentPoints
(
const point& c,
double r,
const point& s1,
const gvector& dir1,
const point& s2,
const gvector& dir2,
point* tanp1,
point* tanp2
)
{
	gvector d;

	// process 1st line
	if ((dir1 * (c - s1)) < 0.)
	{
		d.set(dir1.y(), -dir1.x(), dir2.z());
	}
	else
	{
		d.set(-dir1.y(), dir1.x(), dir1.z());
	}
	d.normalize();
	*tanp1 = c + r*d;

	// process 2nd line
	if ((dir2 * (c - s2)) < 0.)
	{
		d.set(dir2.y(), -dir2.x(), dir2.z());
	}
	else
	{
		d.set(-dir2.y(), dir2.x(), dir2.z());
	}
	d.normalize();
	*tanp2 = c + r*d;
}

//***************************************************************************--
//***************************************************************************

