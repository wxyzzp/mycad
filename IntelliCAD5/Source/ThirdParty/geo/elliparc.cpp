/*	EllipArc.cpp - Implements the class CEllipArc
 *	Copyright (C) 1991-1998 Visio Corporation. All rights reserved.
 *
 *	Abstract
 *
 * See EllipArc.h for the definition of the class.
 * This file implements its methods
 *
 */
#include "Geometry.h"
#include <Float.h>
#include "Point.h"
#include "Line.h"		// For CRay
#include "Trnsform.h"	// for CAffine
#include "Roots.h"		// for CLinearFunction, SolveTrigEquation1
#include "TheUser.h"	// for CCache
#include "Knots.h"		// for CKnots
#include "Extents.h"	// for extents
#include "CompCrvs.h"	// for CMappedCurve
#include "EllipArc.h"
#include "polynomial.h"
#include "vector.h"
#include "plane.h"
#include "2CurveSolver.h"

DB_SOURCE_NAME;


static const int MAX_RECURENCE = 50;

////////////////////////////////////////////////////////////////////////////////////////
//           Elliptical arc


//Default constructor an destructor
CEllipArc::CEllipArc()
    {
	m_rStart = 0;
	m_rEnd = TWOPI;
    }

CEllipArc::~CEllipArc()
    {
    }

PCurve CEllipArc::Clone() const
	{
	return new CEllipArc(*this);
	}
/************************************************************************/
//Constructor from center, major and minor axes lengths and angle
CEllipArc::CEllipArc(
	const C3Point & ptCenter, // In: Ellipse's center
	double rMajor,             // In: Length of major axis
	double rMinor,             // In: Length of minor axis
	double rAngle,			   // In: Angle between major axis and the X axis
	RC & rc)                   // Out: Return code
// rMajor doesn't have to be greater than rMinor.  The resulting arc will
// is in the plane of constant Z that contains ptCenter
    {
	m_ptCenter = ptCenter;

	double rC = cos(rAngle);
	double rS = sin(rAngle);

	m_ptMajor = C3Point(rMajor * rC, rMajor * rS);
	m_ptMinor = C3Point(-rMinor * rS, rMinor * rC);

	m_rStart = 0.0;
	m_rEnd = TWOPI;

	rc = SUCCESS;
    }
/************************************************************************/
//Constructor raw data, no validity checks
CEllipArc::CEllipArc(
	const C3Point & ptCenter, // In: Ellipse's center
	const C3Point & ptMajor,  // In: Major axis
	const C3Point & ptMinor,  // In: Minor axis
	double rStart,			   // In: Start angle
	double rEnd)			   // In: End angle
// ptMajor doesn't have to be greater than ptMinor
    {
	m_ptCenter = ptCenter;
	m_ptMinor = ptMinor;
	m_ptMajor = ptMajor;
	m_rStart = rStart;
	m_rEnd = rEnd;
    }
/************************************************************************/
//Constructor from 3 points, major axis direction angle and eccentricity ratio
CEllipArc::CEllipArc(
	const C3Point & A, // In: Arc's first endpoint
	const C3Point & B, // In: An interior point on the arc
	const C3Point & C, // In: Arc's last endpoint
	double rAngle,		  // In: Angle between major axis and the X axis
	double rRatio,        // In: The ratio of major to minor axis
	RC & rc)              // Out: Return code
    {
	m_rStart = m_rEnd = 0;
	double rC = cos(rAngle);
	double rS = sin(rAngle);
	double rRadius;
    C3Point ptTA, ptTB, ptTC;

#ifdef THREE_D
	// This is a 2D construction, so it's incorrect if the plane through
	// the 3 points A,B,C is not parallel or conicident with the XY plane
	ASSERT(A.Z() == B.Z()  &&  B.Z() == C.Z());
#endif

    //The transformation matrix that takes us to a circular arc
	Matrix2X2 mT = {    {rC, 				 rS		    },
			   	   {-rRatio * rS, 	rRatio * rC    }    };

	// Bug 5169, mxk, 8/8/95
	if (Collinear(C, A, B, FUZZ_DIST))
		{
		rc = E_NOMSG;
		QUIT
		}

    //Apply the transformation to the 3 points and find the circle
    ptTA = mT * A;
    ptTB = mT * B;
    ptTC = mT * C;

    // Get the center and angles from that arc
    if ((rc = CenterFrom3Pts(ptTA, ptTB, ptTC)))
    	QUIT

    rRadius = PointDistance(ptTA, m_ptCenter);
    if (FZERO(rRadius, FUZZ_DIST))
        {
    	rc = FAILURE;
    	QUIT
        }

   //Get the arc ends angles and make them relative to the major axis
    if ((rc = GetAngles(ptTA, ptTB, ptTC)))
    	QUIT

	//The inverse of T will take the center back to absolute coordinates
	mT[0][1] = -rS / rRatio;
	mT[1][0] = rS;
	mT[1][1] = rC / rRatio;

	//Transform the center back to the ellipse world
	m_ptCenter = mT * m_ptCenter;

    //The major and minor axes
    m_ptMajor.SetX(rRadius * rC);
    m_ptMajor.SetY(rRadius * rS);

	rRadius /= rRatio;
    m_ptMinor.SetX(-rRadius * rS);
    m_ptMinor.SetY(rRadius * rC);

    // Flip the curve direction if necessary, to make sure that Start < End.
	if (m_rStart > m_rEnd)
		FlipMinorAxis();

    // Validate the construction
	ASSERT(m_ptCenter + m_ptMajor*cos(m_rStart) + m_ptMinor*sin(m_rStart) == A);
	ASSERT(m_ptCenter + m_ptMajor*cos(m_rEnd) + m_ptMinor*sin(m_rEnd) == C);
	ASSERT(FEQUAL(m_ptMajor.Y() / m_ptMajor.Norm(), sin(rAngle), FUZZ_GEN));
	ASSERT(FEQUAL(m_ptMajor.Norm() / m_ptMinor.Norm(), rRatio, FUZZ_GEN));
exit:;
    }
/*******************************************************************************/
//Construct a full circle from a center and radius
CEllipArc::CEllipArc(
	const C3Point & ptCenter, // In: Center
	double rRadius, 			          // In: Radius
	RC & rc)					  // Out: Return code
    {
// The circle will be in a plane of constant Z
	// if radius = 0 then fail
    if (FZERO(rRadius, FUZZ_DIST))
        {
    	rc = FAILURE;
    	QUIT
	    }

	rc = SUCCESS;

    // Set the center
	m_ptCenter = ptCenter;

	// Set the axes
	m_ptMajor = C3Point(rRadius, 0.0);
	m_ptMinor = C3Point(0.0, rRadius);

	// Set the interval
	m_rStart = 0.0;
	m_rEnd = TWOPI;
exit:;
    }

/*******************************************************************************/
// Construct a full circle from a center, radius, and normal
// Author:  Stephen W. Hou
// Date:    6/20/2004
CEllipArc::CEllipArc(
	const C3Point & ptCenter,       // In: Center
	double rRadius, 			    // In: Radius
    const C3Vector &normal,         // In: Normal
	RC & rc)					    // Out: Return code
{
// The circle will be in a plane of constant Z
	// if radius = 0 then fail
    if (FZERO(rRadius, FUZZ_DIST)) {
    	rc = FAILURE;
    	return;
	}

	rc = SUCCESS;

    // Set the center
	m_ptCenter = ptCenter;
    C3Vector normalDir(normal.Normalize());

    C3Vector majDir(1.0, 0.0, 0.0);
    C3Vector minDir = normalDir ^ majDir;
    minDir.Normalized();
    majDir = minDir ^ normalDir;

	// Set the axes
	m_ptMajor = rRadius * majDir.AsPoint();
	m_ptMinor = rRadius * minDir.AsPoint();

	// Set the interval
	m_rStart = 0.0;
	m_rEnd = TWOPI;
}
/*******************************************************************************/
//Construct a circular arc from 2 points and bow
CEllipArc::CEllipArc(
	const C3Point & A, // In: Arc's first endpoint
	double rBow, 	   // In: Arc's bow - see explanation below
	const C3Point & B, // In: Arc's last endpoint
	RC &   rc)         // Out: Return code
    {
	rc = FAILURE;
	m_rStart = m_rEnd = 0;

#ifdef THREE_D
	// This is a 2D construction, so it's incorrect if the plane through
	// the 2 points A and B is not parallel or conicident with the XY plane
	ASSERT(A.Z() == B.Z());
#endif

	// if A = B or Bow = 0 return a trivial arc
	C3Point U = B - A;
	double rT = U.Norm() / 2;            //Half chord length

    if (FZERO(rT, FUZZ_DIST) || FZERO(rBow, FUZZ_DIST))
    	// Not a valid circle, but we're not going to make
		// too much noise, because this may often happen
		goto exit;

   	{
   	// Apply triangle similarity to find the (signed) radius
	double rRadius = (rBow + rT*rT/rBow) / 2;

   	//Find the center by going (Radius - Bow) away from the chord's midpoint
   	rT = 1.0 / (2.0 * rT);       // Full chord length
	C3Point V(-U.Y() * rT , U.X() * rT);
	m_ptCenter = (A + B) * 0.5 + (rRadius - rBow) * V;

	// Set the radius and the axes
	rRadius = fabs(rRadius);
	m_ptMajor = C3Point(rRadius, 0.0);
	m_ptMinor = C3Point(0.0, rRadius);

	// Compute the interval
	U = A - m_ptCenter;
   	if (FZERO(U.X(), FUZZ_GEN)  &&  FZERO(U.Y(), FUZZ_GEN))
		QUIT
	m_rStart = atan2(U.Y(), U.X());


	U = B - m_ptCenter;
   	if (FZERO(U.X(), FUZZ_GEN)  && FZERO(U.Y(), FUZZ_GEN))
		QUIT
	m_rEnd = atan2(U.Y(), U.X());

	if (rBow < 0)  // clockwise arc - flip the minor axis
		FlipMinorAxis();

	// Make sure that Start < End
	if (m_rEnd < m_rStart)
	    {
		if (m_rEnd < 0)
			m_rEnd += TWOPI;
		else
			m_rStart -= TWOPI;
	    }
	}
	rc = SUCCESS;
exit:;
    }
/*******************************************************************************/
// Construct a full upright ellipse from a defining rectangle
CEllipArc::CEllipArc(
	double rLeft,			// In: Ellipse's minimal x
	double rTop,			// In: Ellipse's maximal y
	double rRight,			// In: Ellipse's maximal x
	double rBottom)			// In: Ellipse's minimal y
	{
// The resulting ellipse will be in the XY plane
	m_ptCenter = C3Point((rLeft + rRight) / 2, (rTop + rBottom) / 2);
	m_ptMajor = C3Point((rRight - rLeft) / 2, 0);
	m_ptMinor = C3Point(0, (rTop - rBottom) / 2);

	m_rStart = 0;
	m_rEnd = TWOPI;
	}
/*******************************************************************************/
// Construct an arc from a rectangle (defining the ellipse) & two radius vectors
CEllipArc::CEllipArc(
	double rLeft,				// In: Ellipse's minimal x
	double rTop,				// In: Ellipse's maximal y
	double rRight,				// In: Ellipse's maximal x
	double rBottom,				// In: Ellipse's minimal y
	const C3Point & ptStart,	// In: Defines Arc's starting radius
	const C3Point & ptEnd,		// In: Defines Arc's ending radius
	RC &  rc)					// Out: Return code
	{
	rc = E_InputZero;
	m_rStart = m_rEnd = 0;
	double a = (rRight - rLeft) / 2;
	double b = (rTop - rBottom) / 2;
	if (FZERO(a, FUZZ_DIST) || FZERO(b, FUZZ_DIST))
		QUIT

#ifdef THREE_D
	// This is a 2D construction, so it's incorrect if the plane through
	// the 2 points is not parallel or conicident with the XY plane
	ASSERT(ptStart.Z() == ptEnd.Z());
#endif


	// The ellipse, as defined by the rectangle
	m_ptCenter = C3Point((rLeft + rRight) / 2, (rTop + rBottom) / 2);
	m_ptMajor = C3Point(a, 0);
	m_ptMinor = C3Point(0, b);

	// Get the starting and ending angles
	if (rc = RayAngle(ptStart, m_rStart))
		QUIT
	if (rc = RayAngle(ptEnd, m_rEnd))
		QUIT
	if (m_rEnd < m_rStart)
		m_rEnd += TWOPI;
exit:;
	}
/*******************************************************************************/
// Construct a circular arc from 2 points, a tangent and the radius
CEllipArc::CEllipArc(
	const C3Point & A, // In: Arc's first point
	const C3Point & U, // In: Tangent vector at A
	const C3Point & B, // In: Arc's last point
	double rRadius,    // In: Radius
	RC &  rc)          // Out: Return code
/*
 * The input defines 2 potential arcs.  The choice we make is based on the
 * direction of U.  A is the constructed arc's start, and its derivative at A
 * agrees with U.
 */
    {
	rc = E_NOMSG;  // Failure of this construction is no exception
	m_rStart = m_rEnd = 0;

#ifdef THREE_D
	// This is a 2D construction, so it's incorrect if the plane through
	// the points and the vector is not parallel or conicident with the XY plane
	ASSERT(A.Z() == B.Z()  &&  U.Z() == 0);
#endif

    // Check the input
    C3Point V = B - A;
    double r = Determinant(U, V);
    if (FZERO(r, FUZZ_GEN))
	    QUIT

    if (FZERO(rRadius, FUZZ_DIST))
	    QUIT

    // Get a unit vector pointing to the center
    V = U;
   	V.TurnLeft();
    if (r < 0)
    	V = -V;

    if (rc = V.Unitize())
    	QUIT

    // Now get the center
    m_ptCenter = A + rRadius * V;

    // The axis vectors
	m_ptMajor = C3Point(rRadius, 0.0);
	m_ptMinor = C3Point(0.0, rRadius);

    // The angles
	rc = FAILURE;
    V = B - m_ptCenter;
   	if (FZERO(V.X(), FUZZ_GEN)  && FZERO(V.Y(), FUZZ_GEN))
		QUIT
    m_rEnd = atan2(V.Y(), V.X());
    V = A - m_ptCenter;
   	if (FZERO(V.X(), FUZZ_GEN)  && FZERO(V.Y(), FUZZ_GEN))
		QUIT
    m_rStart = atan2(V.Y(), V.X());

	r = fabs(m_rEnd - m_rStart);
	if (rRadius * r < FUZZ_DIST)
		QUIT

/*
 * To choose the correct arc we look at the directions of U and the
 * radius-vector V=A-Center to see if the arc should be clockwise or
 * counterclockwise when going from A to B.  In any case, it may be
 * necessary to get a different occurrance of an angle (up to a 2PI
 * rotation) to get the correct order between the 2 angles.  To get a
 * clockwise arc requires flipping the minor axis, which will also
 * change the signs of both Start and End, so we need them in the reverse
 * order.
 */
	if (Determinant(V, U) > 0)  // We're supposed to go counterclockwise
		{
		if (m_rEnd < m_rStart)  // We just need the correct angle
		   m_rEnd += TWOPI;
		 }
	else     // We're need to flip orientation
		{
		if (m_rEnd > m_rStart) // Need to get the correct angle
	    	m_rStart += TWOPI;
	    FlipMinorAxis();
	    }
	rc = SUCCESS;
exit:;
    }
/*******************************************************************************/
//Construct a circular arc from a center, radius, and angles
CEllipArc::CEllipArc(
	double rRadius, 			// In: Radius
	const C3Point & ptCenter,	// In: Center
	double rStart,				// In: Start angle, must be < end angle
	double rSweep,				// In: Sweep angle
	RC & rc)					// Out: Return code
    {
	m_rStart = m_rEnd = 0;

	// The resulting circle will be in a plane of constant Z
	// if radius = 0 then fail
    if (FZERO(rRadius, FUZZ_DIST))
        {
    	rc = E_InputZero;
    	QUIT
	    }

	rc = SUCCESS;

    // Set the center
	m_ptCenter = ptCenter;

	// Set the axes
	m_ptMajor = C3Point(rRadius, 0.0);
	m_ptMinor = C3Point(0.0, rRadius);

	// Set the interval
	m_rStart = rStart;
	m_rEnd = rStart + rSweep;
	if (m_rEnd < m_rStart)
		FlipMinorAxis();
exit:;
    }

/*******************************************************************************/
// Construct a circular arc from a center, radius, and angles
// Author:  Stephen W. Hou
// Date:    6/21/2004
CEllipArc::CEllipArc(
	double rRadius, 			// In: Radius
	const C3Point & ptCenter,	// In: Center
    const C3Vector & normal,    // In: Normal
	double rStart,				// In: Start angle, must be < end angle
	double rSweep,				// In: Sweep angle
	RC & rc)					// Out: Return code
{
	m_rStart = m_rEnd = 0;

	// The resulting circle will be in a plane of constant Z
	// if radius = 0 then fail
    if (FZERO(rRadius, FUZZ_DIST)) {
    	rc = E_InputZero;
    	return;
	}

	rc = SUCCESS;

    // Set the center
	m_ptCenter = ptCenter;
    C3Vector normalDir(normal.Normalize());

    C3Vector majDir(1.0, 0.0, 0.0);
    C3Vector minDir = normalDir ^ majDir;
    minDir.Normalized();
    majDir = minDir ^ normalDir;

	// Set the axes
	m_ptMajor = rRadius * majDir.AsPoint();
	m_ptMinor = rRadius * minDir.AsPoint();

	// Set the interval
	m_rStart = rStart;
	m_rEnd = rStart + rSweep;
	if (m_rEnd < m_rStart)
		FlipMinorAxis();
}

/*******************************************************************************/
// Construct a n upright quarter elliptical arc
CEllipArc::CEllipArc(
	int iWhich,					// In: Which quarter, according to quadrant
	const C3Point & ptCenter,	// In: Arc's center
	double rMajor,				// In: Length of major axis
	double rMinor)				// In: Length of minor axis
// Enter 0 for the first quadrant, 1 for the second, etc.
// The resulting circle will be in a plane of constant Z
	{
	m_ptCenter = ptCenter;
	m_ptMajor = C3Point(rMajor, 0);
	m_ptMinor = C3Point(0, rMinor);

	m_rStart = iWhich * PIOVER2;
	m_rEnd = m_rStart + PIOVER2;
	}

/*******************************************************************************/
// Construct a circular arc from 2 points and bulge (DWG polyline definition)
CEllipArc::CEllipArc(
	const C3Point & A, // In: Arc's first endpoint
	const C3Point & B, // In: Arc's last endpoint
	double rBulge, 	   // In: Tan(1/4 arc's angle), negative if clockwise
	RC &   rc,         // Out: Return code
	P3Point pN)		   // In: Normal (optional, NULL OK)
    {
// The arc is constructed so that its major axis goes from the center to A.

	m_rStart = m_rEnd = 0;
	rc = E_BadInput;				// Presumed guilty
	C3Point HalfBA = (A - B) / 2;	// Half the chord vector
	double rCot;
	double rAbsBulge = fabs(rBulge);

	//Modified Cybage SBD 23/04/2001 dd/mm/yyyy [
	//Reason : Mail from Ben Thump (mcthum@siacad.com) Dated 28/03/2001
	//if (rAbsBulge < FUZZ_GEN  || rAbsBulge > TWOPI - FUZZ_GEN)
	double ang=0.0;
	ang = 4 * atan(rAbsBulge);
	if (ang < FUZZ_GEN || ang > TWOPI - FUZZ_GEN)
			QUIT
	//Modified Cybage SBD 23/04/2001 dd/mm/yyyy ]

	//	Trig identity: if t = tan(a/4) then
	//		cot(a/2) = (1 - t^2) / 2t
	rCot = (1 - rBulge * rBulge) / (2 * rBulge);

	// To get from A to the center, go haf way to B, and go cot(a/2)
	// of that to the left.
	m_ptMinor = HalfBA;
	m_ptMinor.TurnLeft(pN);		// Temporary useage of m_ptMinor
	m_ptMajor = HalfBA + rCot * m_ptMinor;
	m_ptCenter = A - m_ptMajor;

// The domain is [0, 4|atan(bulge)|].  Clockwise orientation ia captured
// by the flipped minor axis

	// Minor axis
	m_ptMinor = m_ptMajor;
	m_ptMinor.TurnLeft(pN);
	if (rBulge < 0)
		m_ptMinor = -m_ptMinor;

	// Angles
	m_rStart = 0;
	m_rEnd = 4 * atan(rAbsBulge);

	rc = SUCCESS;
exit:;
    }

///////////////////////////////////////////////////////////////////////////////
//		Utilities for the constructors
RC CEllipArc::CenterFrom3Pts(
	const C3Point & A, 		//      }
	const C3Point & B, 		//      } In: 3 points
	const C3Point & C) 		//      }
// Compute the center of a circle through 3 given points
// If the points are collinear then set a warning, and return no center

	/* To compute the center P, state the fact that A,B and C have equal
	   distances from P:

			(A-P)*(A-P) = (B-P)*(B-P)
			(C-P)*(C-P) = (B-P)*(B-P)

	   where * stands for the dot product.

	   The term P*P cancels out, and he resulting equations are linear:

			2(Bx-Ax)Px + 2(By-Ay)Py = B*B - A*A
			2(Cx-Ax)Px + 2(Cy-Ay)Py = C*C - A*A		*/

    {
    RC rc = SUCCESS;
    double rS, rT, rX;
    C3Point U = B - A;
    C3Point V = C - A;
	double  rDenom = Determinant(U, V) * 2;  //This will be the denominator

	if (FZERO(rDenom, FUZZ_GEN)) // The 3 points are collinear
		{
		rc = FAILURE;
		QUIT
		}

	// Otherwise carry on solving the equations using Cramer's rule
	rS = A * A;
	rT = C * C - rS;
	rS = B * B - rS;

	rX = DET2(rS,  B.Y() - A.Y(),
			  rT,  C.Y() - A.Y() ) / rDenom;

	rS = DET2(B.X() - A.X(),  rS,
			  C.X() - A.X(),  rT ) / rDenom;

	m_ptCenter = C3Point(rX, rS);
exit:
	RETURN_RC(CEllipArc::CenterFrom3Pts, rc);
    }
#if defined (_WIN16)
	// restore optimization settings
	#pragma optimize ("", on)
#endif
/********************************************************************************/
RC CEllipArc::GetAngles(
	const C3Point & A, //In: First arc endpoint
	const C3Point & B, //In: Intermediate point
	const C3Point & C) //In: Second arc endpoint
// Compute an angle interval for a given circular arc segment from 3 points

/* The interval is defined by the angles of the radius vectors A-P and B-P.
   It satisfies:
		* The angles at A and C are Start and End, not necessary in that order
		* The angle at B is between Start and End
   The points don't necessarily have to be on the arc, but they mus stay away
   from the center
 */
    {
	RC rc = SUCCESS;
	double rT;

	// Get the angle of A-P
	C3Point U = A - m_ptCenter;
	if (FZERO(U.X(), FUZZ_GEN)  && FZERO(U.Y(), FUZZ_GEN))
		{
		rc = FAILURE;
		QUIT
		}
	m_rStart = atan2(U.Y(), U.X());


	// Get the angle C-P
	U = C - m_ptCenter;
	if (FZERO(U.X(), FUZZ_GEN)  && FZERO(U.Y(), FUZZ_GEN))
		{
		rc = FAILURE;
		QUIT
		}
	m_rEnd = atan2(U.Y(), U.X());

	// Get the angle of B-P
	U = B - m_ptCenter;
	if (FZERO(U.X(), FUZZ_GEN)  && FZERO(U.Y(), FUZZ_GEN))
		{
		rc = FAILURE;
		QUIT
		}
	rT = atan2(U.Y(), U.X());

	// T is not between Start and End then we got the wrong arc
	if ( !( (m_rStart < rT && rT < m_rEnd) ||
		    (m_rEnd < rT && rT < m_rStart) )  )
	    {
		// Get the other arc
		if (m_rStart < m_rEnd)
			m_rStart += TWOPI;
		else
			m_rStart -= TWOPI;
	    }
	exit:
	RETURN_RC(CEllipArc::GetAngles, rc);
    }
/*******************************************************************************/
// Get the angle of a ray from the center to a given point
RC CEllipArc::RayAngle(
	C3Point ptP, // In: A point on the ray
	double & rAngle)	 // Out: The parameter value
	{
/*
 * Given the ellipse x^2 / a^2 + y^2 / b^2 = 1, compute the angle t so that of
 * that the intersection of the ellipse with the ray emanating from the center
 * in the given direction vector is (a cos(t), b sin(t)).
 *
 * The point on the ellipse is (rX, rY), where (X,Y) is the vector.  To be on
 * on the ellipse, it must satisfy r^2*(X^2 / a^2 + Y^2 / b^2) = 1, which yields
 * r = (a * b) / sqrt((x * b)^2 + (y * a)^2).
 * Since r*X = a*cos(t) and r*y = b*sin(t), we have
 * (sin(t), cos(t)) = (r*y/b, r*x/y), whence the formula below
 */
	RC rc;
	rAngle = 0;

	// Get the ray's direction vector
	ptP -= m_ptCenter;

	double xb = ptP * m_ptMajor;
	double ya = ptP * m_ptMinor;
	double r = sqrt(xb * xb + ya * ya);
	if (FZERO(r, FUZZ_GEN))
		rc = E_InputZero;
	else
		{
		rAngle = atan2(ya / r, xb / r);
		rc = SUCCESS;
		}
	RETURN_RC(RayAngle, rc);
	}
/************************************************************************/
// Curve type
CURVETYPE CEllipArc::Type() const
    {
	return TYPE_Elliptical;
    }
/*****************************************************************************/
// Half the greatest diameter
double CEllipArc::Radius() const
    {
	return MAX(m_ptMajor.Norm(), m_ptMinor.Norm());
    }
/*****************************************************************************/
//Flip the minor axis to make the arc go clockwise
void CEllipArc::FlipMinorAxis()
    {
	m_rStart = - m_rStart;
	m_rEnd = - m_rEnd;
	m_ptMinor = - m_ptMinor;
    }
/************************************************************************/
/*
 * Evaluation on a paramter value that is too far away from 0 is useless,
 * so I restricted valid evaluation to the interval |t| < 1000. Even
 * there, apply fmod to get closer to 0.  In case of failure, we still want
 * to initialize the output, so we evaluate at 0.
  * This applies to all 3 evaluators. Bug 4894, mxk, 8/8/95
 */
/************************************************************************/
// Pull an angle close to 0, if it's not too far away
LOCAL RC PullCloseTo0(
	double & t)	 // In/out: The number to pull
	{
// In case of FAILURE return 0
	if (fabs(t) <= 1000)
		{
		t = fmod(t, TWOPI);
		return SUCCESS;
		}
	else
		{
		t = 0;
		return FAILURE;
		}
	}
/************************************************************************/
// Evaluator - position only
RC CEllipArc::Evaluate(
	double rAt,						// In: Parameter value to evaluate at
	C3Point & ptPoint) const	// Out: Point on the arc at that parameter
    {
	RC rc = PullCloseTo0(rAt);
	ptPoint = m_ptCenter + m_ptMajor * cos(rAt) + m_ptMinor * sin(rAt);

	RETURN_RC(CEllipArc::Evaluate, rc);
    }
/************************************************************************/
// Evaluator - point and n derivatives
RC CEllipArc::Evaluate(
	double rAt,                   // In: Where to evaluate
	int nDerivatives,           // In: NUmber of desired derivatives
	PRealPoint pValues) const   // Out: Position and derivatives
	{
	RC rc = PullCloseTo0(rAt);
	double rCos = cos(rAt);
	double rSin = sin(rAt);

	// Position and first 3 derivatives
    pValues[0] = m_ptMajor * rCos + m_ptMinor * rSin;
    if (nDerivatives > 0)
    	pValues[1] = - rSin * m_ptMajor + rCos * m_ptMinor;
    if (nDerivatives > 1)
    	pValues[2] = - pValues[0];
    if (nDerivatives > 2)
    	pValues[3] = - pValues[1];

    // After that it's a repetition of the same thing
	for (int i=5;  i<=nDerivatives;  i++)
		pValues[i] = pValues[i - 4];

	pValues[0] += m_ptCenter;

	RETURN_RC(CEllipArc::Evaluate, rc);
    }
/************************************************************************/
// Evaluator - position and tangent vector
RC CEllipArc::Evaluate(
	double rAt,                    // In: Parameter value to evaluate at
	C3Point & ptPoint,   // Out: Point on the arc with that parameter
	C3Point & ptTangent) const // Out: Tangent vector at that point
    {
	RC rc = PullCloseTo0(rAt);
	double rCos = cos(rAt);
	double rSin = sin(rAt);

	ptPoint = m_ptCenter + m_ptMajor * rCos + m_ptMinor * rSin;
	ptTangent = - rSin * m_ptMajor + rCos * m_ptMinor;

	RETURN_RC(CEllipArc::Evaluate, rc);
    }
/************************************************************************/
// Update the extents at a given parameter value
LOCAL inline void Update(
	CExtents& cExtents,	// In: Extents
	CEllipArc& cArc,	// In: An arc
	double t)			// In: Parameter value on the arc
	{
	C3Point P;
	cArc.Evaluate (t, P);
	cExtents.Update(P);
	}
/************************************************************************/
// Update bounding box
RC CEllipArc::UpdateExtents(
	BOOL bTight,			// In: Tight extents if =TRUE
	BOOL bIgnoreStart,		// In: Ignore the startpoint, if =TRUE
	CExtents & cExtents,	// In/out: The extents, updated here
	PAffine pTransform) 	// Optional: Transformation (NULL OK)
	{
	int i;
	RC rc = SUCCESS;
	double rTheta;
	CEllipArc cCopy = *this;  // Transformed copy

	if (pTransform)
		cCopy.Transform(*pTransform);

	//Endpoints
	if (!bIgnoreStart)
		Update(cExtents, cCopy, cCopy.Start());
	Update(cExtents, cCopy, cCopy.End());

	// X(Theta)'=0 where tan (Theta) = XMinor / XMajor
	if (FZERO(cCopy.MajorAxis().X(), FUZZ_GEN)  &&
		FZERO(cCopy.MinorAxis().X(), FUZZ_GEN))
		{
		rc = E_NOMSG;
		goto exit;
		}
	rTheta = atan2(cCopy.MinorAxis().X(), cCopy.MajorAxis().X());

	// record Theta and Theta+pi if they are within the arc's domain
	for (i=0;  i<2;  i++)
		{
		if (PullIntoDomain(rTheta))
			Update(cExtents, cCopy, rTheta);
		rTheta += PI;
		}

	// Same for Y - Y(Theta)'=0 where tan (Theta) = YMinor / YMajor
	if (FZERO(cCopy.MajorAxis().Y(), FUZZ_GEN)  &&
		FZERO(cCopy.MinorAxis().Y(), FUZZ_GEN))
		{
		rc = E_NOMSG;
		goto exit;
		}
	rTheta = atan2(cCopy.MinorAxis().Y(), cCopy.MajorAxis().Y());

	// record Theta and Theta+pi if they are within the arc's domain
	for (i=0;  i<2;  i++)
		{
		if (PullIntoDomain(rTheta))
			Update(cExtents, cCopy, rTheta);
		rTheta += PI;
		}
exit:
	if (rc == E_NOMSG)
		{
		// The ellipse is degenerate
		Update(cExtents, cCopy, 0);
		Update(cExtents, cCopy, PIOVER2);
		Update(cExtents, cCopy, PI);
		Update(cExtents, cCopy, -PIOVER2);
		rc = SUCCESS;
		}
	RETURN_RC(CCurve::UpdateExtents, rc);
    }
/**********************************************************************/
// Add this arc's contribution to the integral of |C, C', Up|
RC CEllipArc::UpdateAreaIntegral(
	double & rSoFar,	// In/Out: The integral
	P3Point pUp) const  // In: Upward unit normal (NULL OK)
    {
	// Set up the normal
	CNormal N(pUp);

/* We need the integral from Start to End of the determinant:
			| Center + Major * cos(t) + Minor * sin(t) |
			|		 - Major * sin(t) + Minor * cos(t) |
			|	N									   |
   This determinant breaks into several terms, e.g. |Center, -Major, N|*sin(t)^2,
   |Minor, Minor, N|*sin(t)*cos(t), etc. A determinant with 2 equal rows like the
   latter is 0, so we are left with 4 terms.  Using sin(t)^2 + cost(t)^2
   collapses 2 of these.  Now,
			Integrao dt = End - Start
			Integral(-sin(t)) = cost(End) - cos(Start),
			Integral(cos(t)) = sin(End) - sin(Start),
   so:
*/
    rSoFar += (m_rEnd - m_rStart) * Determinant(m_ptMajor, m_ptMinor, N);
	rSoFar += (cos(m_rEnd) - cos(m_rStart)) * Determinant(m_ptCenter, m_ptMajor, N);
    rSoFar += (sin(m_rEnd) - sin(m_rStart)) * Determinant(m_ptCenter, m_ptMinor, N);

	RETURN_RC(Celliparc::UpdateAreaIntegral, SUCCESS);
    }

/****************************************************************************/
// Is the ellipse closed
BOOL CEllipArc::Closed() const
// Return true if the curve ends meet
    {
	return FEQUAL(m_rEnd - m_rStart, TWOPI, FUZZ_GEN);
    }
/****************************************************************************/
RC CEllipArc::LinearizePart(
	double rFrom,			// In: Where to start
	double rTo,				// In: Where to stop
	double rResolution,		// In: Chord-height toelrance
	PAffine pTransform,		// In: Rendering transformation (NULL OK)
	CCache & cCache) const  // In: The cache we render into
	{
	RC rc = SUCCESS;
	int n;
	int i=0;
	double r = Radius();
	double dt = rTo - rFrom;
	double t;

	if (r < rResolution  || r < FUZZ_GEN) // Show one point
		n = 1;
	else
		{
		// Get the increment t and the number of points
		t = 1.0 - rResolution / (2 * r);
		t = 2 * acos(t);

		// Get the number of points
		if (t < FUZZ_GEN)
			n = 100;
		else
			n = (int)(dt / t) + 1;

		// Get the adjusted increment
		dt /= n;
		}

	// Transform the center and axes to device coordinates
	C3Point A,B,C;
	if (pTransform)
		{
   		pTransform->ApplyToVector(m_ptMajor, A);
   		pTransform->ApplyToVector(m_ptMinor, B);
   		pTransform->ApplyToPoint(m_ptCenter, C);
		}
	else
		{
   		A = m_ptMajor;
   		B = m_ptMinor;
   		C = m_ptCenter;
		}

	// Get the starting angle t
	if (!cCache.Started())
		{
		// Do the first point
		t = rFrom;
		n++;
		}
	else
		t = rFrom + dt;

	// Sine and cosine of the starting angle and the increment
	double x = cos(t);
	double y = sin(t);
	double dx = cos(dt);
	double dy = sin(dt);

	C3Point P;

	// Get the points
	for (i=1;  /* Stopping test is inside the loop */ ;  i++)
		{
		// Add a point
		if(i == n)		/*D.G.*/// Calculate the last point more exact.
		{
			x = cos(rTo);
			y = sin(rTo);
		}
		P = C + A * x + B * y;
   		if (rc = cCache.Accept(P, t))
			break;
   		if (i >= n)
   			break;

		// Move on to the next point
		t += dt;
		r = x * dx - y * dy;
		y = y * dx + x * dy;
		x = r;
		}
	RETURN_RC(CEllipArc::LinearizePart, rc);
	}
/************************************************************************/
// Recalculate the axes if they are not orthogonal
BOOL CEllipArc::RecalcAxes(// Return TRUE actually recalculated
	double& rShift)	  // Out: The angle shift of the new major axis
	{
	/*
	 The ellipse is understood to be
			Center + Major*cos(t) + Minor*Sin(t),
	 but if the axes are not orthogonal, this method will replace
	 the axes with orthogonal axes that represent the same ellipse.
	 For an elliptical arc, this would cause a domain shift.  The
	 amount of the shif is computed here and returned as output.
	 */
	double s = m_ptMajor * m_ptMinor;
	BOOL b = !FZERO(s, FUZZ_GEN);
	if (b)
		{
		/*
		The axes are not orthogonal, we need to find new ones.
		The new major axis will be at an extremum of
		||Major*cos(t) + Minor*sin(t)||. Differentiating the square
		of that and equating to 0 yields
		(Major*Major - Minor*Minor)*sin(2t) = 2*Major*Minor*cos(2t), so:
		*/
		C3Point U = m_ptMajor;
		C3Point V = m_ptMinor;
		double r = (U*U - V*V);
		rShift = atan2(2*s, r) / 2;

		// Get the new major and minor
		b = (Determinant(m_ptMajor, m_ptMinor) < 0);
		s = sin(rShift);
		r = cos(rShift);
		m_ptMajor = U * r + V * s;
		m_ptMinor = U * (-s) + V * r;
		if (b)	// It's a clockwise arc
			m_ptMinor = -m_ptMinor;

		b = TRUE;  // Axes have actually been recalculated
		}
	return b;
	}
/************************************************************************/
// Transform in place
RC CEllipArc::Transform(
	CTransform & cTransform)   // In: The transformation
	{
	RC rc = SUCCESS;
	double r;

	ASSERT(cTransform.Affine());

	// Apply the transformation to the defining point and vectors
	cTransform.ApplyToPoint(m_ptCenter, m_ptCenter);
	cTransform.ApplyToVector(m_ptMajor, m_ptMajor);
	cTransform.ApplyToVector(m_ptMinor, m_ptMinor);

	if (RecalcAxes(r))
		{
		// Get the new start and end angles
		m_rStart -= r;
		m_rEnd -= r;
		}

	RETURN_RC(CEllipArc::Transform, rc);
	}
/************************************************************************/
// Reverse
RC CEllipArc::Reverse()
    {
	double r = -m_rEnd;
	m_rEnd = -m_rStart;
	m_rStart = r;
	m_ptMinor = -m_ptMinor;
	RETURN_RC(CEllipArc::Reverse, SUCCESS);
    }
/*******************************************************************************/
// Find all intersections an overlaps
RC CEllipArc::SelfIntersect(
	CRealArray & cParameters, // In/Out: List of self intersection parameters
	double rTolerance)		  // Optional: Tolerance
	{
	// An elliptical arc should never intersect itself
	RETURN_RC(CEllipArc::SelfIntersect, SUCCESS);
	}
/************************************************************************/
// Is this arc circular?
BOOL CEllipArc::IsCircular(
	double & rRadius)  const // Out: The length of the longer axis
    {
	// An ellipse is a circle if its axes are of equal length
	double r = m_ptMajor.Norm();
	double s = m_ptMinor.Norm();
	rRadius = MAX(r, s);
	return FEQUAL(r, s, FUZZ_DIST);
    }
/************************************************************************/
//Arc length
RC CEllipArc::Length(
	double & rLength) const   // Out: The total length
    {
	RC rc = SUCCESS;
	double rRadius;

	if (IsCircular(rRadius))
		rLength = rRadius * fabs(m_rEnd - m_rStart);
	else
		rc = CCurve::Length(rLength);
	RETURN_RC(CEllipArc::Length, rc);
    }
/************************************************************************/
// The angle of the radius-vector corresponding to a given point
RC CEllipArc::AngleToPoint(
	const C3Point & P,		// In: The point
	double & rAngle) const	// Out: The angle
/*
 * This function returns the signed angle between the line segment connecting
 * the arc's center to P and the arc's major axis for a circular arc
 */
    {
	RC rc = SUCCESS;

	C3Point U = P - m_ptCenter;
	double x = U * m_ptMajor;
	double y = U * m_ptMinor;
	if (FZERO(x, FUZZ_GEN)  && FZERO(y, FUZZ_GEN))
		rc = FAILURE;
	else
		rAngle = atan2(y, x);

	RETURN_RC(CEllipArc::AngleToPoint, rc);
    }
/*******************************************************************************/
// A translation to a given point
RC CEllipArc::Move(
	const C3Point & ptBy)	// In: The desired origin's new position
	{
	m_ptCenter += ptBy;
	RETURN_RC(CEllipArc::Move, SUCCESS);
	}
/************************************************************************/
// Construct the knot sequence for the approximation with a cubic spline within tolerance
RC CEllipArc::CubicKnots(
	double rTolerance,	// In: The acceptable error
	PKnots & pKnots)	// Out: Knots for an approximating spline within tolerance
	{
	RC rc = SUCCESS;
	pKnots = NULL;
	double dt = End() - Start();
	double rCurrent = Start();
	int i, n, nRepeat;

	// Check input validity
	if (FZERO(rTolerance, FUZZ_GEN))
		{
		rc = E_InputZero;
		QUIT
		}

	n = (int)(dt * Radius() / (64 * rTolerance));  // The number of nontrivial spans

	// Don't go beyond 1/4 ellipase with one span
	if (dt > PIOVER2  &&  n < 2)
		n = 2;

	// Must have at least one span
	if (n < 1)
		n = 1;
	dt /= n;

	if (FEQUAL(dt, TWOPI, FUZZ_GEN))
		nRepeat = 0;
	else
		nRepeat = 3;
	n += nRepeat;

	pKnots = new CKnots(3, n, rc);
	if (!pKnots)
		rc = DM_MEM;
	if (rc)
		QUIT

	// Add the first knot, repeated if it isn't a full ellipse
	for (i=0;  i<=nRepeat;  i++)
		(*pKnots)[i] = rCurrent;

	// March on
	for (/* i is already set*/;   i < n;    i++)
		{
		rCurrent += dt;
		(*pKnots)[i] = rCurrent;
		}
exit:
    if (rc)
		{
		if (pKnots)
    		delete pKnots;
		pKnots = NULL;
		}
	else
		// Set the last knot
		pKnots->Set(n, End());

	RETURN_RC(CEllipArc::CubicKnots, rc);
	}
/*******************************************************************************/
// Compute an offset
RC CEllipArc::Offset(
	double rDistance,	// In: Offset signed distance (left if positive)
	PCurve & pOffset, 	// Out: The offset
	P3Point pUp) const	// In: Upward unit vector (NULL OK)
	{
/*
 * If it's a circular arc then the offset is also a circular arc, otherwise
 * fall back to the general curve method.
 */
	double a = m_ptMajor.Norm();
	double b = m_ptMinor.Norm();
	RC rc;
	pOffset = NULL;
	CNormal N(pUp);

	if (FEQUAL(a, b, FUZZ_DIST))
		{
		// It's a circular arc
		PEllipArc pArc;
		double r;
 // If (Major, Minor, N) is a right-hands system then an leftward offset
 // is an offset inwards, decreasing the radius.
		if (Determinant(m_ptMajor, m_ptMinor, N) > 0)
			rDistance = -rDistance;

		if (FZERO(a, FUZZ_DIST))
			{
			// The input is a degenerate arc
			rc = E_ZeroVector;
			QUIT
			}

		if (rc = ALLOC_FAIL(pArc = new CEllipArc(*this)))
			QUIT
		r = 1 + rDistance / a;
		pArc->m_ptMajor = r * m_ptMajor;
		pArc->m_ptMinor = r * m_ptMinor;
		pOffset = pArc;
		}
	else
		// Invoke the curve method
		rc = CCurve::Offset(rDistance, pOffset);
exit:
	RETURN_RC(CEllipArc::Offset, rc);
	}
/*******************************************************************************/
// Return an approximation with one of the Visio curves
RC CEllipArc::GetApproximation(
	double rTolerance,		// In: Approxiation tolerance
	PCurve & pApprox) 		// Out: The approximation
	{
	RC rc = ALLOC_FAIL(pApprox = Clone());
	RETURN_RC(CEllipArc::GetApproximation, rc);
	}
/*******************************************************************************/
// Construct a copy reparameterized to arc length
RC CEllipArc::ArcLengthCopy(
	double rFrom,	// In: Domain start
	double rTo,		// In: Domain end
	PCurve & pCopy)	// Out: A constant speed copy
	{
/*
 * This function computes the procedural combination of this curve with a
 * parameter mapping f so that the speed (=length of derivative vector) is
 * equal to 1. Since a circular arc goes at a constant speed = radius, we
 * can use a linear mapping if the arc is circular.  Otherwise we fall back to
 * the general curve method
 */
	RC rc;
	double r;
	PRealFunction pMapping = NULL;
	pCopy = NULL;

	if (IsCircular(r))	// The arc has a constant speed
		{
		r *= (rTo - rFrom);	// This is the length of the arc
		if (rc = ALLOC_FAIL(pMapping = new CLinearFunction(0, r, rFrom, rTo)))
			QUIT
		rc = ALLOC_FAIL(pCopy = new CMappedCurve(this, pMapping));
		pMapping = NULL;
		}
	else
		rc = CCurve::ArcLengthCopy(rFrom, rTo, pCopy);
exit:
	delete pMapping;
	if (rc)
		{
		delete pCopy;
		pCopy = NULL;
		}
	RETURN_RC(CEllipArc::ArcLengthCopy, rc);
	}
/*******************************************************************************/
// Initiate a stored path with this curve
RC CEllipArc::InitiatePath(
	PStorage pStorage) const	// In/Out: The Storage
	{
	RC rc = SUCCESS;
	if (m_rEnd - m_rStart <= TWOPI - FUZZ_GEN)
		rc = pStorage->InitiateCurve(this);

	// Do nothing if full ellipse
	RETURN_RC(CEllipArc::InitiatePath, rc);
	}
/*******************************************************************************/
// Persist this arc
RC CEllipArc::Persist(
	PStorage pStorage) const	// In/Out: The storage
	{
	RC rc;

	if (m_rEnd - m_rStart > TWOPI - FUZZ_GEN)
		{
		// It's a full ellipse
		CEllipse e(*this);
		rc = pStorage->AcceptEllipse(&e);
		}
	else
		rc = pStorage->AcceptEllipArc(this);

	RETURN_RC(CEllipArc::Persist, rc);
	}
/*****************************************************************************/
// Check if this offset distance will not cusp & do ugly things
RC CEllipArc::AdmitOffset( // Return E_BadOffset if this offset distance is no good
	double rDistance,		// In: The offset distance
	double rTolerance,		// In: The acceptable error
	P3Point pNormal)		// In: The Up direction (NULL OK)
	{
/* The offset of an elliptical arc may be bad if it is inwards, and the radius
   of curvature anywhere in teh arc is less than the ofset distance.  The minimal
   radius of curvature may be the ellipse's minimal radius, which is at one of the
   tips of the major axis, if that point is withing the arc.  Otherwise, the
   miminimum is attained at one of the arc's ends.
*/
	RC rc = SUCCESS;
	double a, b;
	double rCurvature = 0;
	double s;

	CNormal N(pNormal);

	double r = Determinant(m_ptMajor, m_ptMinor, N);
/* Zero offset, or offset outwards is always smooth.  The offset is outwards
   if the arc goes counterclockwise (r > 0) and the offset is to the right
   (rDistance < 0), or if the arc goes clockwise (r < 0) and the offset is to
   the left (rDistance > 0)
*/
	if ((fabs(rDistance) < FUZZ_GEN)  || r * rDistance < 0)
		goto exit;

	// We'll talk curvature = the inverse of the radius of curvature, so
	rDistance = fabs(1 / rDistance);

	// The extreme curvature of an ellipse is at the tip of the major axis
	a = m_ptMajor.Norm();
	b = m_ptMinor.Norm();
	r = 0;
	if (a < b)
		{
		// The major and minor axes are swapped
		r = PIOVER2;
		SWAP(a, b, double);
		}

	// Is the tip of the axis within the arc?
	s = r + PI;
	if (PullIntoDomain(r) || PullIntoDomain(s))
		{
		// Yes, get the curvature there, which is a / b^2
		b *= b;
		if (b < FUZZ_GEN)
			// Degenerate ellipse, with infinite curvature
			rCurvature = INFINITY;
		else
			rCurvature = a / b;
		}
	else
		{
		// The exterme is at one of the arc's ends
		if (rc = GetCurvature(m_rStart, r))
			QUIT
		rCurvature = fabs(r);

		if (rc = GetCurvature(m_rEnd, r))
			QUIT
		r = fabs(r);
		if (r > rCurvature)
			rCurvature = r;
		}
	if (rCurvature > fabs(rDistance))
		// The arc is too curved for this offset
		rc = E_BadOffset;
exit:
	RETURN_RC(CEllipArc::AdmitOffset, rc);
	}

/************************************************************************/
// Set the domain
RC CEllipArc::SetDomain(
	double rFrom,  // In: New curve start
	double rTo)    // In: New curve end
	{
	RC rc = FAILURE;

	if (rFrom < rTo - FUZZ_DIST)
		{
		rc = SUCCESS;
		m_rStart = rFrom;
		m_rEnd = rTo;
		}
	RETURN_RC(CEllipArc::SetDomain, rc);
	}
/************************************************************************/
// Reset the domain interval's start
RC CEllipArc::ResetStart(
	double rNew)  // In: New curve start
	{
	RC rc = FAILURE;

	if (rNew <= m_rEnd)
		{
		rc = SUCCESS;
		m_rStart = rNew;
		}
	RETURN_RC(CEllipArc::ResetStart, rc);
	}
/************************************************************************/
// Reset the domain interval's end
RC CEllipArc::ResetEnd(
	double rNew)  // In: New curve end
	{
	RC rc = FAILURE;

	if (rNew >= m_rStart)
		{
		rc = SUCCESS;
		m_rEnd = rNew;
		}
	RETURN_RC(CEllipArc::ResetEnd, rc);
	}
/**********************************************************************/
// Try the tip of a major or minor axis as a local minimum
RC CEllipArc::TryAxisTip(
	double angle,		  // In: The angle of that axis
	const C3Point & axis, // In: The axis with that angle
	C3Point & ptNearest,  // Out: The nearest point on the curve
	double & rAt,		  // Out: Parameter value of Nearest
	double & rDist) const // Out: The distance
	{
	RC rc = E_NOMSG;

	if (PullIntoDomain(angle))
		{
		rc = SUCCESS;
		ptNearest = m_ptCenter + axis;
		rAt = angle;
		rDist = axis.Norm();
		}

	return rc;
	}
/**********************************************************************/
// Nearest point on the arc to a given point
RC CEllipArc::PullBack(
	C3Point ptTo,		    // In: The point we want to get closest to
	C3Point & ptNearest,    // Out: The nearest point on the curve
	double & rAt,           // Out: Parameter value of Nearest
	double & rDist,		    // Out: The distance from the point to the curve
	double rMargin,			// In: Ignored here
	double ignore) const	// Optional: Parameter value to ignore
    {
	BOOL bCircular;
	RC rc = E_NOMSG;
	double a, b, x, y;

	rDist = INFINITY;
	rAt = 0;

// Find an angle t so that
//
//		P = Center + MajorAxis * cos(t) + MinorAxis * sin(t)
//
// In the process, determine whether the arc is circular.  If it is,
// Get its radius, and the distance of the point from the center
// The angle will be incorredt if Major is not perpendicular to Minor

	C3Point U = ptTo - m_ptCenter;
	a = m_ptMajor * m_ptMajor;
	b = m_ptMinor * m_ptMinor;
	x = (a + b) * FUZZ_GEN;

	// Circular if Major and Minor are perpendicular and of equal length
	bCircular = FEQUAL(a, b, x)  && FZERO(m_ptMajor * m_ptMinor, x);

	if (FZERO(a, FUZZ_GEN)  ||  FZERO(b, FUZZ_GEN))
		{
		// The ellipse is degenerate
		rAt = m_rStart;
		ptNearest = m_ptCenter;
		rDist = PointDistance(ptTo, m_ptCenter);
		rc = SUCCESS;
		goto exit;
		}

	x = (U * m_ptMajor) / a;
	y = (U * m_ptMinor) / b;
	rAt = atan2(y, x);

	if (bCircular)
		{
		if (FZERO(x, FUZZ_GEN)  &&  FZERO(y, FUZZ_GEN))
			{
			// ptTo is in the center, any point on the arc will do
			rAt = (m_rStart + m_rEnd) / 2;
			rc = SUCCESS;
			}	// End of point in circle's center
		else
			// See if rAt is in the domain
			if (PullIntoDomain(rAt))
				rc = SUCCESS;

		// In any case
		if (!rc)
			{
			Evaluate(rAt, ptNearest);
			rDist = PointDistance(ptTo, ptNearest);
			}
		}  // End of circular case
	else
		{
		//	Non-circular ellipse, use pullback rAt as a seed
		rc = CCurve::PullBack(ptTo, rAt, ptNearest, rAt, rDist);
		if (!rc)
			rDist = sqrt(rDist);
		}
exit:
	if (!rc  &&   FEQUAL(rAt, ignore, FUZZ_GEN))
		rc = E_NOMSG;
	RETURN_RC(CEllipArc::PullBack, rc);
    }

/****************************************************************************
 *
 *	Test if a point is lying on the plane of the ellipse
 *
 *	@Author: Stephen W. Hou
 *	@Date:   Sep 24, 2003
 */
BOOL CEllipArc::IsOnPlane(const C3Point &pt) const // In: a point
{
	C3Point origin = m_ptCenter;
	C3Vector xAxis(m_ptMajor);
	xAxis.Normalized();

	C3Vector yAxis(m_ptMinor);
	yAxis.Normalized();

	C3Vector zAxis = xAxis ^ yAxis;

	CAffine transf(&origin, &xAxis.AsPoint(), &yAxis.AsPoint(), &zAxis.AsPoint());

	C3Point pt1;
	transf.ApplyInverseToPoint(pt, pt1);
	return FZERO(pt1.Z(), FUZZ_DIST);
}


/****************************************************************************
 *
 *	Find the perpendicular line through a point. The function returns SUCCESS
 *  if the perpendicualar point is found and E_NOMSG otherwise. The optional 
 *  reference point is used to determine the user intertion if there are more
 *  than one point.
 *
 *	@Author: Stephen W. Hou
 *	@Date:   Sep 21, 2003
 */
RC CEllipArc::PerpendicularThru(
	const C3Point & ptThru,			// In: The point the perpendicular line go through
    const C3Point & ptReference,    // In: The optional eference point to select perpendicular point
	C3Point &ptPerpPoint,			// Out: The perpendicular point on the curve
	double & rAt) const				// Out: The parameter value of the perpendicular point
{
	C3Point origin = m_ptCenter;
	C3Vector xAxis(m_ptMajor);
	xAxis.Normalized();

	C3Vector yAxis(m_ptMinor);
	yAxis.Normalized();

	C3Vector zAxis = xAxis ^ yAxis;
    zAxis.Normalized();

	CAffine transf(&origin, &xAxis.AsPoint(), &yAxis.AsPoint(), &zAxis.AsPoint());

	double a = m_ptMajor.Norm();
	double b = m_ptMinor.Norm();

    RC rc;
    CEllipArc* pEllipArc = new CEllipArc(C3Point(0.,0.,0.), a, b, TWOPI, rc);
    if (!Closed()) {
        C3Point ptStart, ptEnd;
        GetStartPoint(ptStart);
        GetEndPoint(ptEnd);

	    transf.ApplyInverseToPoint(ptStart, ptStart);
	    transf.ApplyInverseToPoint(ptEnd, ptEnd);

        double rStart, rEnd;
        pEllipArc->AngleToPoint(ptStart, rStart);
        if (FZERO(rStart, FUZZ_RAD))
            rStart = 0.0;

        pEllipArc->AngleToPoint(ptEnd, rEnd);
        if (FZERO(rEnd, FUZZ_RAD))
            rEnd = TWOPI;

        pEllipArc->ResetStart(rStart);
        pEllipArc->ResetStart(rEnd);
    }

    double refPoint, dis;
	C3Point pt, ptNearest;
    if (&ptReference) {
        transf.ApplyInverseToPoint(ptReference, pt);
        pEllipArc->PullBack(pt, ptNearest, refPoint, dis);
    }
    
	transf.ApplyInverseToPoint(ptThru, pt);


	double param, min = INFINITY;
	bool isOnAxis = false, found = false;

	// the input point is on Y axis
	//
    double dis1, dis2;
	C3Point ppPoint(0.0, 0.0, 0.0);
	if (FZERO(pt.X(), FUZZ_DIST)) {
        if (&ptReference) {
            if (refPoint >= PIOVER2 && refPoint <= THREEPIOVER2) {
                dis1 = refPoint - PIOVER2;
                dis2 = THREEPIOVER2 - refPoint;
            }
            else {
                if (refPoint <= PIOVER2) {
                    dis1 = PIOVER2 - refPoint;
                    dis2 = PIOVER2 + refPoint;
                }
                else {
                    dis1 = TWOPI - refPoint + PIOVER2;
                    dis2 = refPoint - THREEPIOVER2;
                }
            }
        }
        else {
            dis1 = fabs(pt.Y() - b);
            dis2 = fabs(pt.Y() + b);
        } 

        if (dis1 < dis2) {
		    // the perpendicular point is at (0, b)
		    //
		    param = PIOVER2;
		    if (pEllipArc->PullIntoDomain(param)) {
			    min = dis1;
			    ppPoint.SetY(b);
			    found = true;
		    }
        }
        else {
		    // the perpendicular point is at (0, -b)
		    //
		    param = THREEPIOVER2;
		    if (pEllipArc->PullIntoDomain(param) && dis2 < min) {
			    min = dis2;
			    ppPoint.SetY(-b);
			    found = true;
		    }
        }
		isOnAxis = true;
	}

	// the input point is on X axis
	//
	if (FZERO(pt.Y(), FUZZ_DIST)) {

		// reset y coordinate
		//
        if (&ptReference) {
            if (refPoint <= PI) {
                dis1 = refPoint;
                dis2 = PI - refPoint;
            }
            else {
                dis1 = TWOPI - refPoint;
                dis2 = refPoint - PI;
            }
        }
        else {
            dis1 = fabs(pt.X() - a);
            dis2 = fabs(pt.X() + a);
        } 
        if (dis1 < dis2) { // the refernce point is more closed to positive major axis point
		    // the perpendicular point is at (0, a)
		    //
		    param = 0.0;
		    if (pEllipArc->PullIntoDomain(param) && dis1 < min) {
			    min = dis;
			    ppPoint.SetX(a);
		        ppPoint.SetY(0.0);
			    found = true;
		    }
        }
        else {
		    // the perpendicular point is at (0, -a)
		    //
		    param = PI;
		    if (pEllipArc->PullIntoDomain(param) && dis2 < min) {
			    ppPoint.SetX(-a);
		        ppPoint.SetY(0.0);
			    found = true;
		    }
        }
		isOnAxis = true;
	}
	// the input point ie neither on x-axis nor y-axis
	//
	if (!isOnAxis) {
		double a2 = a * a;
		double b2 = b * b;
		double d = a2 - b2;
		double c4 = d * d;
		double c3 = - 2. * a2 * pt.X() * d;
		double c2 = a2 * (a2 * pt.X() * pt.X() + b2 * pt.Y() * pt.Y() - c4);
		double c1 = 2. * a2 * a2 * pt.X() * d;
		double c0 = -1. * a2 * a2 * a2 * pt.X() * pt.X();

		CPolynomial equation(c0, c1, c2, c3, c4);

		std::vector<double> roots;
		if (equation.getRoots(roots)) {
			double x, y, e;
			for (register i = 0; i < roots.size(); i++) {
				x = roots[i];
				if (x < -a || x > a)	// skip the solution if it is not on ellipse
					continue;

				e = 1. - x * x / a2;
				if (e >= 0.0) {
					y = b * sqrt(e);
					if (y < -b || y > b) // skip the solution if it is not on ellipse
						continue;

					// the slops must be perpendicular
					//
					if (FZERO(b2 * x * (pt.Y() - y) - a2 * y * (pt.X() - x), FUZZ_DIST)) {
						rAt = atan2(y, x);
						// the value of parameter must be in the domain
						//
						if (pEllipArc->PullIntoDomain(rAt)) {
                            if (&ptReference) {
                                if (rAt > refPoint) {
                                    dis1 = rAt - refPoint;
                                    dis2 = TWOPI - rAt + refPoint;
                                }
                                else {
                                    dis1 = refPoint - rAt;
                                    dis2 = TWOPI - refPoint + rAt;
                                }
                                dis = MIN(dis1, dis2);
                            }
                            else
							    dis = SQR(pt.X() - x) + SQR(pt.Y() - y);

							if (dis < min) {
								min = dis;
								ppPoint.SetX(x);
								ppPoint.SetY(y);
								found = true;
							}
						}
					}
					else {
						y *= -1.;
						// the slops must be perpendicular
						//
						if (FZERO(b2 * x * (pt.Y() - y) - a2 * y * (pt.X() - x), FUZZ_DIST)) {
							rAt = atan2(y, x);

							// the value of parameter must be in the domain
							//
							if (pEllipArc->PullIntoDomain(rAt)) {
                                if (&ptReference) {
                                    if (rAt > refPoint) {
                                        dis1 = rAt - refPoint;
                                        dis2 = TWOPI - rAt + refPoint;
                                    }
                                    else {
                                        dis1 = refPoint - rAt;
                                        dis2 = TWOPI - refPoint + rAt;
                                    }
                                    dis = MIN(dis1, dis2);
                                }
                                else
								    dis = SQR(pt.X() - x) + SQR(pt.Y() - y);

								if (dis < min) {
									min = dis;
									ppPoint.SetX(x);
									ppPoint.SetY(y);
									found = true;
								}
							}
						}
					}

				}

			}
		}
	}
    delete pEllipArc;

    if (found || isOnAxis) {
		transf.ApplyToPoint(ppPoint, ptPerpPoint);
		return SUCCESS;
	}
	else
		return E_NOMSG;
}


/*******************************************************************************/
// Update the number of intercpts with a given ray
RC CEllipArc::UpdateIntercepts(
	PRay pRay,			// In: A ray
	int	& nIntercepts)  // In/out: The number of intercepts
/*
 * The arc's parametric representation is C + A cos(t) + B sin(t).
 * Write a (nonparametric) equation U*X = d for the line.  We need to
 * solve
 * 				U*(C + A cos(t) + B sin(t)) = d,
 * or:
 * 				U*A cos(t) + U*B sin(t) + U*C-d = 0;
 * but
 *				d = U*P,
 *
 * where P is any point on the line, so the equation is:
 *
  * 			U*A cos(t) + U*B sin(t) + U*(C-P) = 0;
*
 */
	{
	RC rc = SUCCESS;
	int i, nRoots;
	double r[2];

	// Get an equation of the form U*X = r for the line
	C3Point U = pRay->Velocity();
	U.TurnLeft();
	double c = U * (m_ptCenter - pRay->Origin());

	if (rc = SolveTrigEquation1(U*m_ptMajor, U*m_ptMinor,
							    c, nRoots, r[0], r[1]))
		QUIT

	if (nRoots == 1)
		{
		// Grazing intersection is no good for ray intercepts
		rc = E_NOMSG;
		QUIT
		}

	for (i = 0;  i < nRoots;  i++)
		{
		if (!PullIntoDomain(r[i]))
			// The intersection is not the arc's domain, so
			continue;		// skip the rest

		// Check if the intersection is within the ray's domain
		Evaluate(r[i], U);
		if ((U - pRay->Origin()) * pRay->Velocity() < -FUZZ_DIST)
			// The intersection is not on the ray, so
			continue;		// skip the rest

		// Endpoints are not good as ray intercpts
		if (FEQUAL(r[i], Start(), FUZZ_GEN)  ||
			FEQUAL(r[i], End(), FUZZ_GEN))
			{
			rc = E_NOMSG;
			QUIT
			}

		// Passed all tests, we have an intercept
		nIntercepts++;
		}
exit:
	RETURN_RC(CEllipArc::UpdateIntercepts, rc);
	}

/**********************************************************************/
// Find the representation of t that is closest the seed
LOCAL void PullToSeed(
	double rSeed,	// In: Parameter value of the initial guess
	double & t)		// In/out: A solution
	{
	double s;

	while (1)
		{
		s = t + TWOPI;
		if (fabs(s - rSeed) < fabs(t - rSeed))
			t = s;
		else
			break;
		}

	while (1)
		{
		s = t - TWOPI;
		if (fabs(s - rSeed) < fabs(t - rSeed))
			t = s;
		else
			break;
		}
	}
/**********************************************************************/
// A tangent to the arc thru point, with initial guess.
RC CEllipArc::TangentThru(		// E_NOMSG if solution not found
	const C3Point& ptThru,	// In: The point on the tangent
	double rSeed,           // In: Parameter value of the initial guess
	double & rAt)			// Out: Parameter value of tangent
    {
/* We are looking for a value of t where where C(t)-P has the same
   direction C'(t), or  |C(t)-P, C'(t)|=0 where |*| is the determinant.
   Replacing P by P-Center we may assume that the ellipse is centered
   at the origin, so the ellipse is C(t) = A cos(t) + B sin(t),
   and C'(t) = -A sin(t) + B cos(t), so the equation to solve is:

		|A cos(t) + B sin(t) - P, -A sin(t) + B cos(t)| = 0.

   Expand the determinant, recalling that |A,A| = |B,B| = 0 and
   sin(t)*sin(t) + cos(t)*cos(t) = 1, to obtain

		|P,B| cos(t) - |P,A| sin(t) - |A,B| = 0, which is the same as
		|P,B| cos(t) + |A,P| sin(t) + |B,A| = 0.

  Solve this with the special solver we have for such equations.
*/
	int n;

	// Get the equation's coefficients
	C3Point P = ptThru - m_ptCenter;
	double a = Determinant(P, m_ptMinor);
	double b = Determinant(m_ptMajor, P);
	double c = Determinant(m_ptMinor, m_ptMajor);

	// Solve the equation
	double r1, r2;	// Solutions
	RC rc = SolveTrigEquation1(a, b, c, n, r1, r2);
	if (rc)
		QUIT

	// Choose the solution
	ASSERT(n==0 ||  n==1  ||  n==2);
	if (n > 0)
		PullToSeed(rSeed, r1);

	if (n == 2)
		{
		// Choose the closest soulution
		PullToSeed(rSeed, r2);
		if (fabs(r2 - rSeed) < fabs(r1 - rSeed))
			SWAP(r1, r2, double);
		}
	rAt = r1;

	if (!PullIntoDomain(rAt))
		{
		n--;
		if (n)
			{
			// a is closer, but is not in the arc's domain, so try r2
			rAt = r2;
			if (!PullIntoDomain(rAt))
				n--;
			}
		}

	if (n == 0)
		rc = E_NOMSG;
exit:
	RETURN_RC(CEllipArc::TangentThru, rc);
	}
/*******************************************************************************/
// Create polyline edge(s) from this curve
RC CEllipArc::GetPolylineEdges(
	P3Point pVertices,	// Out: Vertices array (make room for 2)
	double* pBulges,	// Out: Bulges array (make room for 2)
	int	& nEdges)		// Out: The number of resulting edges
// If the arc is too short then 0 will be returned for nEdges
// If the arc is close to a full circle than 2 edges will be computed
// Otherwise a single edge will be computed.
	{
	RC rc = SUCCESS;
	nEdges = 0;
	double rRadius, rAngle;

	Evaluate(Start(), pVertices[0]);
	pBulges[0] = 0;

	if (!IsCircular(rRadius))
		{
		// Only valid for circular arcs
		rc = FAILURE;
		QUIT
		}

	rAngle = End() - Start();
	if (FZERO(rAngle * rRadius, FUZZ_DIST))
		// Just leave it as a straight line segment
		goto exit;

	nEdges = 1;
	if (rAngle > TWOPI - .001)
		{
		// It's almost a closed circle, create 2 edges
		nEdges = 2;
		rAngle /= 2;
		}

	// Get the bulge
	pBulges[0] = tan(rAngle / 4);
	if (Determinant(m_ptMajor, m_ptMinor) < 0)
		pBulges[0] = -pBulges[0];

	if (nEdges > 1)
		{
		// Get another vertex half way, same bulge
		pBulges[1] = pBulges[0];
		Evaluate(Start() + rAngle, pVertices[1]);
		}
exit:
	RETURN_RC(CEllipArc::GetPolylineEdges, rc);
	}
/*******************************************************************************/
// Find silhouette point(s) in a specified viewing direction within tolerance
//
// Author: Stephen W. Hou
// Date:   06/14/2004
//
static BOOL FindSilhouettePoints(const CEllipArc & ellipse,
								 double rFrom,
								 double rTo,
								 const C3Vector & refDir,
								 int recurenceLevel,
								 CRealArray &rAt)
{
	if (FZERO(rTo - rFrom, FUZZ_REAL))
		return FAILURE;

	double rMiddle = (rTo + rFrom) / 2.0;

	C3Point point, ptTangent;
	ellipse.Evaluate(rMiddle, point, ptTangent);
	if (FZERO(refDir.AsPoint() * ptTangent, FUZZ_DEG)) {
		rAt.Add(rMiddle);
		return SUCCESS;
	}
	else {
		if (recurenceLevel < MAX_RECURENCE) {
			// evaluate tengent direction at the end points.
			//
			C3Point sPoint, ePoint, sTangent, eTangent;
			ellipse.Evaluate(rFrom, sPoint, sTangent);
			ellipse.Evaluate(rTo, ePoint, eTangent);

			// compute the projection value of tangent vectors in the reference direction
			//
			double sProj = sTangent * refDir.AsPoint();
			double mProj = ptTangent * refDir.AsPoint();
			double eProj = eTangent * refDir.AsPoint();

			if (sProj * mProj < 0.0)
				return FindSilhouettePoints(ellipse, rFrom, rMiddle, refDir, recurenceLevel + 1, rAt);
			else if (mProj * eProj < 0.0)
				return FindSilhouettePoints(ellipse, rMiddle, rTo, refDir, recurenceLevel + 1, rAt);
			else
				return FAILURE;
		}
		else
			return FAILURE;
	}
}


/************************************************************************/
// Project a curve to a plane along a specified direction. If the direction
// is not specified, the curve will be projected perpendicularly to the
// plane(along the normal direction of plane). Function returns NULL when
// it fails and a point to the curve otherwise.
//
// Auther:	Stephen W. Hou
// Date:	6/14/2004
//
CCurve* CEllipArc::ProjectTo(const CPlane & plane, const C3Vector & dir, bool get2dCurve) const
{
	CCurve* pCurve = NULL;

    C3Vector projdir;
    if (&dir != NULL) {
        // test if the projected direction is perpendicular to the normal of the
        // projection plane.
        if (FZERO(plane.GetNormal() * dir, FUZZ_DEG))
		    return pCurve;

        projdir = dir;
    }
    else {
        projdir = plane.GetNormal().Negate();
    }


	C3Vector ellNormal = m_ptMajor ^ m_ptMinor;
	ellNormal.Normalized();
	if (FZERO(projdir * ellNormal, FUZZ_DIST)) {
		// the normal of ellipse is perpendicular to the project direction so
		// the result should be a straight line on the projection plane.
		C3Vector refDir = ellNormal ^ projdir;
		refDir.Normalized();

		CRealArray silPoints;
		if (FindSilhouettePoints(*this, Start(), End(), refDir, 0, silPoints)) {
			if (silPoints.Size() > 0 && silPoints.Size() < 3) {
				double sParam, eParam;
				if (silPoints.Size() == 1) {

					C3Point point;
					Evaluate(silPoints[0], point);

					C3Point start, end;
					if (GetStartPoint(start) == SUCCESS && GetEndPoint(end) == SUCCESS &&
						plane.ProjectPoint(start, projdir, start, get2dCurve) == SUCCESS &&
						plane.ProjectPoint(end, projdir, end, get2dCurve) == SUCCESS &&
						plane.ProjectPoint(point, projdir, point, get2dCurve) == SUCCESS) {

					}

					if (point.DistanceTo(start) > point.DistanceTo(end)) {
						sParam = m_rStart;
						eParam = silPoints[0];
					}
					else {
						sParam = silPoints[0];
						eParam = m_rEnd;
					}
				}
				else {
					if (silPoints[0] < silPoints[1]) {
						sParam = silPoints[0];
						eParam = silPoints[1];
					}
					else {
						sParam = silPoints[1];
						eParam = silPoints[0];
					}
				}

				C3Point endPoint1, endPoint2;
				Evaluate(sParam, endPoint1);
				Evaluate(eParam, endPoint2);

				if (plane.ProjectPoint(endPoint1, projdir, endPoint1, get2dCurve) == SUCCESS &&
					plane.ProjectPoint(endPoint2, projdir, endPoint2, get2dCurve) == SUCCESS) {
					pCurve = new CLine(endPoint1, endPoint2);
                    pCurve->SetAs2DCurve(get2dCurve);
				}
			}
		}
	}
	else {
		C3Point center, major, minor;
		if (plane.ProjectPoint(m_ptCenter + m_ptMajor, projdir, major, get2dCurve) == SUCCESS &&
			plane.ProjectPoint(m_ptCenter + m_ptMinor, projdir, minor, get2dCurve) == SUCCESS  &&
            plane.ProjectPoint(m_ptCenter, projdir, center, get2dCurve) == SUCCESS) {
			CEllipArc* pEllipArc = new CEllipArc(center, major - center, minor - center, m_rStart, m_rEnd);

            if (!FEQUAL(fabs(ellNormal * projdir), 1.0, FUZZ_DIST)) {
                double r;
                if (pEllipArc->RecalcAxes(r) && !Closed()) {
                    pEllipArc->m_rStart -= r;
		            pEllipArc->m_rEnd -= r;
	            }
            }
            pCurve = pEllipArc;
            pCurve->SetAs2DCurve(get2dCurve);
        }
	}

	return pCurve;
}

/*******************************************************************************/
// Get the normal of ellipse
//
// Author:  Stephen W. Hou
// Date:    6/27/2004
//
C3Vector CEllipArc::GetNormal() const
{
     C3Vector major(m_ptMajor);
     C3Vector minor(m_ptMinor);
     C3Vector norm = major ^ minor;
     return norm.Normalize();
}


/**************************************************************************/
//
// Performs ray hitting test.
//
// Author:  Stephen W. Hou
// Date:    7/22/2004
//
BOOL CEllipArc::HitTest(        // Return TRUE if the ray hits the cuvre
    const C3Point &ptThrough,   // In: Through point 
    const C3Vector &hitDir,     // In: Hit direction
    double rTolerance,          // In: The tolerance considered a hit
    double & rAt)               // Out: Hit location on the curve within tolerance
{
    BOOL bHit = FALSE;

    CAffine iTransf, transf(hitDir);

    CEllipArc* pEllipArc = (CEllipArc*)Clone();
    C3Point point = ptThrough;

    C3Vector dir = hitDir.Normalize();

    // If the project direction is different than Z direction, we transform
    // point and curve both to the coordinate system defined by the project
    // dirction.
    //
    if (!FZERO(dir.X(), FUZZ_GEN) || !FZERO(dir.Y(), FUZZ_GEN) ||
        !FEQUAL(dir.Z(), 1.0, FUZZ_GEN)) {
        iTransf.SetInverse(transf);
        iTransf.ApplyToPoint(ptThrough, point);
        pEllipArc->Transform(iTransf);
    }

	CRealArray cSeeds;
    if (pEllipArc->Seeds(cSeeds)) {
        delete pEllipArc;
		return bHit;
    }

    CPlane plane(pEllipArc->Center(), pEllipArc->GetNormal(), pEllipArc->MajorAxis());

    C3Point ptOnPlane;
    if (plane.ProjectPoint(point, C3Vector(0., 0., 1.0), ptOnPlane, false) == SUCCESS) {

	    double rSqFuzz = rTolerance * rTolerance;
        CXYPullBack solver(pEllipArc, ptOnPlane, rSqFuzz);

        double rDist = DBL_MAX;
        for (register j = 0;  j < cSeeds.Size(); j++) {
		    if (!solver.Solve(cSeeds[j]) &&	solver.SqDistance() < rDist) {
			    rAt = solver.Parameter();
			    rDist = solver.SqDistance();
	        }
            if (rDist < rSqFuzz) 
                bHit = true;
        }
    }
    delete pEllipArc;
    return bHit;
}


/*******************************************************************************/
// Swap major and minor axes withou changing the normal direction.
//
// Author:  Stephen W. Hou
// Date:    6/29/2004
//
BOOL CEllipArc::SwapAxes()
{
    C3Point tmp = m_ptMajor;
    m_ptMajor = m_ptMinor;
    m_ptMinor = - tmp;
    if (!Closed()) {
        m_rStart -= PIOVER2;
        m_rEnd -= PIOVER2;
    }
    return true;
}


/*******************************************************************************/
// Test if the ellipse is on the X-Y plane
//
// Author:  Stephen W. Hou
// Date:    6/27/2004
//
BOOL CEllipArc::IsOnXYPlane() const
{
    return (FZERO(m_ptCenter.Z(), FUZZ_DIST) &&
            FZERO(m_ptMajor.Z(), FUZZ_DIST) &&
            FZERO(m_ptMinor.Z(), FUZZ_DIST));
}


/*******************************************************************************/
// Test if the ellipses are coplanar.
//
// Author:  Stephen W. Hou
// Date:    6/27/2004
//
BOOL CEllipArc::IsCoplanarWith(const CEllipArc &ell) const
{
	C3Point origin = m_ptCenter;
	C3Vector xAxis(m_ptMajor);
	xAxis.Normalized();

	C3Vector yAxis(m_ptMinor);
	yAxis.Normalized();

	C3Vector zAxis = xAxis ^ yAxis;
	CAffine transf(&origin, &xAxis.AsPoint(), &yAxis.AsPoint(), &zAxis.AsPoint());

	C3Point pt;
	transf.ApplyInverseToPoint(ell.m_ptCenter, pt);
    if (!FZERO(pt.Z(), FUZZ_DIST))
        return FALSE;

    transf.ApplyToVector(ell.m_ptMajor, pt);
    if (!FZERO(pt.Z(), FUZZ_DIST))
        return FALSE;

    transf.ApplyToVector(ell.m_ptMinor, pt);
    if (!FZERO(pt.Z(), FUZZ_DIST))
        return FALSE;

    return TRUE;
}

#ifdef DEBUG
/************************************************************************/
// Dump
void CEllipArc::Dump() const
	{
	DB_MSGX("Elliptical arc: Center, major & minor axes:",());
	m_ptCenter.Dump();
	m_ptMajor.Dump();
	m_ptMinor.Dump();
	}
#endif
/*******************************************************************/
// How complex is this ellipse?
int CEllipArc::Complexity() const
	{
	return 4;
	}
//////////////////////////////////////////////////////////////////////////
//																		//
//					IMPLEMENTATION OF CELLIPSE							//

// Construct from the center an 2 points
CEllipse::CEllipse(
	const C3Point & C,  // In: Ellipse's center
	const C3Point & P,	// In: A point
	const C3Point & Q)  // In: Another point
	{
	double r;
	m_ptCenter = C;
	m_ptMajor = P - C;
	m_ptMinor = Q - C;
	RecalcAxes(r);
	}
/**********************************************************************/
// Clone
PCurve CEllipse::Clone() const
	{
	return new CEllipse(*this);
	}

#ifdef CURVE_SPLITTING
/**********************************************************************/
// Split
RC CEllipse::Split(
	double rAt,              // In: Paramter value to split at
	SIDE nSide,              // In: Which side to keep (possibly both)
	PCurve & pFirst,         // Out: The first half
	PCurve & pSecond) const  // Out: The second half
    {
/* Splitting an ellipse produces a 2pi elliptical arc. The difference between
   that arc and the full ellipse is that the ellipse is seamless, and the arc
   does have a seam, so when the arc is split breaks into 2 arcs.
   */
	pFirst = pSecond = 	new CEllipArc(m_ptCenter, m_ptMajor, m_ptMinor,
													  m_rStart, m_rEnd);
	RC rc = ALLOC_FAIL(pFirst);
	if (!rc)
		pFirst->SetDomain(rAt, rAt + TWOPI);

	RETURN_RC(CEllipse::Split, rc);
    }
#endif

/*****************************************************************************/
// Replace this curve with its restriction a new domain
RC CEllipse::ReplaceWithRestriction(
	double rFrom,   // In: New curve start
	double rTo,     // In: New curve end
	PCurve & pOut)	// Out: The restricion to the new domain
	{
	pOut = NULL;

	RC rc = ALLOC_FAIL(pOut = new CEllipArc(*this));
	if (rc)
		QUIT
	if (rc = pOut->SetDomain(rFrom, rTo))
		QUIT
exit:
	if (rc)
		{
		delete pOut;
		pOut = NULL;
		}
	else
		delete this;

	RETURN_RC(CEllipse::ReplaceWithRestriction, rc);
    }
/*******************************************************************************/
// Persist this arc
RC CEllipse::Persist(
	PStorage pStorage) const	// In/Out: The storage
	{
	RC rc = pStorage->AcceptEllipse(this);
	RETURN_RC(CEllipArc::Persist, rc);
	}


