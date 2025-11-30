/* 2CurveSolver.cpp - Implements the class of C2CurveFunction and its 
 * subclasses.
 *	Copyright (C) 1994-1998 Visio Corporation. All rights reserved.
 *
 *
 *	Abstract
 * As the name reflects, this file was originally designated to host the 
 * implementation of solvers for problems that involve two curves (e.g.
 * intersection.  It now hosts a solver for problems that involve only 
 * one curve, e.g. PullBack.
 *
 * See 2CurveSolver.h for the definition of the classes.
 *
 */
#include "Geometry.h"
#include "Point.h"
#include "Curve.h"
#include "2CurveSolver.h"

DB_SOURCE_NAME
/****************************************************************************/
// Get the geometry information at a given point on a curve
LOCAL RC GetCurveGeometry(
	PCCurve pCurve,			// In: A curve
	double rAt,             // In: Parameter value in the curve's domain
	C3Point & ptPoint,		// Out: Point there
	C3Point & ptTangent,	// Out: Unit tangent vector there
	C3Point & ptCenter,		// Out: Center of curvature there
	double & rSpeed,		// Out: Speed there
	double & rRadius)		// Out: Radius of curvature there
	{
	// Get the curve's geometry at the given parameter
	RC rc = pCurve->Evaluate(rAt, ptPoint, ptTangent, rRadius, &rSpeed);
	if (rc)
		QUIT

// This point is considered flat if the the speed is nonzero, and the 
// radius of curvature is more than 1000 times greater than the speed.
// At this stage rRadius stores the curvature, which is +-1/radius, so
	if (FZERO(rRadius * rSpeed, 0.001))
		{
		if (rSpeed < FUZZ_GEN)
			// The curve degenerates to a point
			rSpeed = rRadius = 0;
		else
			// The curve is flat, it's approximation is a line
			rRadius = INFINITY;
		goto exit;
		}

// The curve degenerates to a point if the radius is less than spaee/1000
	rRadius = 1 / rRadius;
	if (FZERO(rSpeed, rRadius * 0.001))
		{
		rSpeed = rRadius = 0;
		goto exit;
		}

	// The center of curvature lies on the normal
	ptCenter = ptTangent;
	ptCenter.TurnLeft();
	ptCenter = ptPoint + ptCenter * rRadius;
exit:
	if (rRadius == 0)
		ptCenter = ptPoint;
	RETURN_RC(GetCurveGeometry, rc);
	}

///////////////////////////////////////////////////////////////////////////
// Implementation of C2CurveSolver
// Constructors  
C2CurveSolver::C2CurveSolver()
	{
	Initialize();
	}

C2CurveSolver::C2CurveSolver(
	PCurve pFirst,      // In: First curve
	PCurve pSecond)     // In: Second curve
	{
	Initialize();
	m_pCurve[0] = pFirst;
	m_pCurve[1] = pSecond;
	}

void C2CurveSolver::Initialize()
	{
	m_pCurve[0] = m_pCurve[1] = NULL;
	m_s[0] = m_s[1] = m_ds[0] = m_ds[1] = 0;
	m_speed[0] = m_speed[1] = m_rad[0] = m_rad[1] = 0;
	m_bSingular = FALSE;
	}
/****************************************************************************/
// Geometric Newton-Raphson iterative solver
RC C2CurveSolver::Solve( // Return E_NOMSG if no solution found
	double rSeed1,		// In: Initial guess on first curve
	double rSeed2)		// In: Intial guess on second curve
	{
/*
 This is a low-level utility, that identifies pareamter values of the
 solution of a real function that involves 2 curves.  It returns 
 E_NOMSG no solution was found from these seeds.

 The algorithm is iterative.  From the current guess on each curve we 
 extrapolate on the osculating circle if the curvature is nonzero, otherwise
 on the tangnet line.  We find the exact solution for the pair of lines
 or arcs.  The correction to the next guess on each curve is the parameter
 increment that would produce the same distance along the curve as the 
 solution does on the extrapolation, had the speed on the curve been 
 constant, equal to the speed at the current guess.  This method converges
 in one step on any combination of straight lines and circular arcs.
 */
	RC rc = E_NOMSG;
    int i;
	double r, s;
	/*
	 If a curve is periodic, there is no point in making a correction
	 that is greater than the period.  In any case, restrict the 
	 correction to a reasonable margin.
	*/
	double rLimit1 = m_pCurve[0]->End() - m_pCurve[0]->Start();
	double rLimit2 = m_pCurve[1]->End() - m_pCurve[1]->Start();
	if (m_pCurve[0]->Period()) 
		rLimit1 = m_pCurve[0]->Period();
	else
		rLimit1 *= 100;
	if (m_pCurve[1]->Period()) 
		rLimit2 = m_pCurve[1]->Period();
	else
		rLimit2 *= 100;

	// Start with the seeds
	m_s[0] = rSeed1;
	m_s[1] = rSeed2;

	// Iterate
	for (i=0;  i<=20;  i++)
		{
		// Get the details of the osculating circle or tangent line
		if (rc = GetCurveGeometry(m_pCurve[0], m_s[0], m_P[0], 
						m_T[0],	m_C[0], m_speed[0], m_rad[0]))
								  
			QUIT
		if (rc = GetCurveGeometry(m_pCurve[1], m_s[1], m_P[1], 
						m_T[1], m_C[1], m_speed[1], m_rad[1]))
			QUIT

		// Check if the the current solution satisfies the equations
		if (IsSatisfied())
			// We're done
			break;

		// Get the appropriate correction
		if( m_rad[0] == 0)
			{
			if (m_rad[1] == 0) // No correction possible
				break;
			else if(m_rad[1] == INFINITY)		
				rc = CorrectPointLine(0, 1);
			else
				rc = CorrectPointArc(0, 1);
			}
		else if(m_rad[0] == INFINITY)
			{
			if (m_rad[1] == 0)
				rc = CorrectPointLine(1, 0);
			else if(m_rad[1] == INFINITY)		
				rc = CorrectLineLine();
			else
				rc = CorrectLineArc(0, 1);
			}
		else	// m_rad[1] is finite and nonzero
			{
			if (m_rad[1] == 0)
				rc = CorrectPointArc(1, 0);
			else if(m_rad[1] == INFINITY)		
				rc = CorrectLineArc(1, 0);
			else
				rc = CorrectArcArc();
			}

		if (rc || m_bSingular)
			break;

		// Quit if the correction is too large
		if (fabs(m_ds[0]) > rLimit1  || fabs(m_ds[1]) > rLimit2)
			break;

		// Apply the correction
 		m_s[0] += m_ds[0];
 		m_s[1] += m_ds[1];

		// Break if the correction is no longer significant
		r = fabs(m_s[0]);
		s = fabs(m_s[1]);
		if (FZERO(m_ds[0], MAX(r, 1) * FUZZ_REAL) &&
			FZERO(m_ds[1], MAX(s, 1) * FUZZ_REAL))
			break;
 		}	// End of for i
exit:
	if (!m_pCurve[0]->PullIntoDomain(m_s[0])    ||
 		!m_pCurve[1]->PullIntoDomain(m_s[1]))
		rc = E_NOMSG;
	RETURN_RC(C2CurveSolver::Solve, rc);
	}
/****************************************************************************/
// Check if this is a singular solution
BOOL C2CurveSolver::Singular() // Return TRUE if the soultion is singular
	{
	if (m_bSingular)
		goto exit;

	// Get the curve geometry at the solution
	if (GetCurveGeometry(m_pCurve[0], m_s[0], m_P[0], 
				m_T[0], m_C[0], m_speed[0], m_rad[0]))
							  
		goto exit;
	if (GetCurveGeometry(m_pCurve[1], m_s[1], m_P[1], 
				m_T[1], m_C[1], m_speed[1], m_rad[1]))
		goto exit;

	// There is a singularity if either - 
	if (m_rad[0] == 0  &&  m_rad[1] == 0)
		// Both curves degenerate to a point  
		m_bSingular = TRUE;
	else if (m_rad[0] == INFINITY  &&  m_rad[1] == INFINITY)
		// or if both are lines and the tangents are parallel, 
		m_bSingular = FZERO(Determinant(m_T[0], m_T[1]), FUZZ_RAD) 
					  &&   m_T[0] * m_T[1] > 0;
	else if (m_rad[0] != 0  &&  m_rad[1] != INFINITY  &&  
			 m_rad[1] != 0 && m_rad[1] != INFINITY)
		// or if both are arcs and the arcs coincide
		{
		double r = fabs(m_rad[0]);
		double s = fabs(m_rad[1]);
		r = MAX(r, s);
		m_bSingular = FEQUAL(m_rad[0], m_rad[1], r * 0.0001);
		}
exit:
	return m_bSingular;
	}
/****************************************************************************/
// Correction stub
RC C2CurveSolver::CorrectPointLine(
	int i,	// In: The index of the curve that degenerates to a point
	int j)	// In: The index of the other curve
	{
	return Fail();
	}
/****************************************************************************/
// Correction stub
RC C2CurveSolver::CorrectPointArc(
	int i,	// In: The index of the curve that degenerates to a point
	int j)	// In: The index of the other curve
	{
	return Fail();
	}
/****************************************************************************/
// Correction stub
RC C2CurveSolver::CorrectLineLine()
	{
	return Fail();
	}
/****************************************************************************/
// Correction stub
RC C2CurveSolver::CorrectLineArc(
	int i,	// In: The index of the curve that has 0 curvature
	int j)	// In: The index of the other curve
	{
	return Fail();
	}
/****************************************************************************/
// Correction stub
RC C2CurveSolver::CorrectArcArc()
	{
	return Fail();
	}
//////////////////////////////////////////////////////////////////////////
//																		//
//					Distance Between Curves								//

// Constructors
CCurvesDistance::CCurvesDistance()
	{
	m_rSqDist = INFINITY;
	m_rMargin = 0;
	}

CCurvesDistance::CCurvesDistance(
	PCurve pFirst,      // In: First curve
	PCurve pSecond,     // In: Second curve
	double rMargin) 	// In: Distance that is considered 0
	:C2CurveSolver(pFirst, pSecond)
	{
	m_rSqDist = INFINITY;
	m_rMargin = rMargin * rMargin;
	}

// Update the current minimum
LOCAL BOOL Update(	 // Return TRUE if the distance is less than the margin
	double rMargin,	 // In: Squred distance margin
	double d,		 // In: Contender new minimum of squared distance
	double s,		 // In: Contender's parameter value on first curve
	double t,		 // In: Contender's parameter value on second curve
	const C3Point& A, // In: Contender's point on first curve
	const C3Point& B, // In: Contender's point on second curve
	double& rDist,	 // In/out: The squared distance, updated here
	double& r1,		 // In/out: Parameter value on first curve
	double& r2,		 // In/out: Parameter value on second curve
	C3Point& P1,	 // In/out: Point on first curve
	C3Point& P2)	 // In/out: Point on second curve
	{
	if (d < rDist * rDist)
		{
		rDist = sqrt(d);
		r1 = s;
		r2 = t;
		P1 = A;
		P2 = B;
		}
	return rDist < rMargin;
	}
/****************************************************************************/
// Get a correction based on a point and a linear extrapolations
RC CCurvesDistance::CorrectPointLine(
	int i,	// In: The index of the curve that degenerates to a point
	int j)	// In: The index of the other curve
	{
	ASSERT (m_speed[j] > FUZZ_GEN);
	m_ds[i] = 0;
	m_ds[j] = (m_P[i] - m_P[j]) * m_T[j] / m_speed[j];
	return SUCCESS;
	}
/****************************************************************************/
// Get a correction based on one point and one circular extrapolations
RC CCurvesDistance::CorrectPointArc(
	int i,	// In: The index of the curve that degenerates to a point
	int j)	// In: The index of the other curve
	{
	double rFuzz = MAX(m_speed[0], m_speed[1]) * 0.001;
	m_ds[i] = m_ds[j] = 0;
	C3Point CQ = m_P[i] - m_C[j];
	double r = CQ.Norm();
	if (r < rFuzz)
		// The point lies in the center of the circle
		m_bSingular = TRUE;
	else
		{
		// Get the nearest point on the circle
		CQ = CQ * (m_rad[j] / r);
		C3Point CP = m_P[j] - m_C[j];

		// Convert to a parameter correction
		m_ds[j] = atan2(Determinant(CQ, CP), CQ * CP);
		ASSERT(m_speed[j] > FUZZ_GEN);
		m_ds[j] *= (m_rad[j] / m_speed[j]);
		}
	return SUCCESS;
	}
/****************************************************************************/
// Get a correction based on 2 linear extrapolations
RC CCurvesDistance::CorrectLineLine()
	{
	C3Point D = m_P[1] - m_P[0];
	double r = Determinant(m_T[0], m_T[1]);
	if (FZERO(r, FUZZ_GEN))
		{
		m_ds[0] = (D * m_T[0]) / 2;
		m_ds[1] = -(D * m_T[1]) / 2;
        // In this case, the lines are neither parallel or conicident but are
        // perpendicular. m_bSingular should be set to TRUE iff the lines
        // are either parallel or conicident. SWH - 7/8/2004
		// m_bSingular = TRUE;
		}
	else
		{
        if (FEQUAL(r, 1.0, FUZZ_GEN))
            m_bSingular = TRUE;

		// Intersecting lines, go to the intersection
		m_ds[0] = Determinant(D, m_T[1]) / r;
		m_ds[1] = Determinant(D, m_T[0]) / r;
		}

	// Convert distances to parameters
	ASSERT(m_speed[0] > FUZZ_GEN);
	ASSERT(m_speed[1] > FUZZ_GEN);
	m_ds[0] /= m_speed[0];
	m_ds[1] /= m_speed[1];

	return SUCCESS;
	}
/****************************************************************************/
// Get a correction based on one linear and one circular extrapolations
RC CCurvesDistance::CorrectLineArc(
	int i,	// In: The index of the curve that has 0 curvature
	int j)	// In: The index of the other curve
	{
	m_ds[i] = m_ds[j] = 0;

	double d, r;
	int nCount = 1;
	C3Point CQ = m_P[j] - m_C[j];
	C3Point CP[2];
	CP[0] = m_P[i] - m_C[j];
	d = r = 0;

	// Find the point on the line nearest to the arc's center
	ASSERT(FEQUAL(m_T[i].Norm(), 1, m_speed[i] * 0.001));
	m_ds[i] = -(CP[0] * m_T[i]);
	CP[0] = CP[0] + m_ds[i] * m_T[i];
	d = CP[0] * CP[0];
	r = m_rad[j] * m_rad[j];
	if (d < r - FUZZ_GEN)
		{
		// Get 2 the intersections of the line with the circle
		nCount = 2;
		r = sqrt(r - d);
		d = m_ds[i] + r;
		m_ds[i] = m_ds[i] - r;
		CP[0] = m_P[i] + m_ds[i] * m_T[i] - m_C[j];
		CP[1] = m_P[i] + d * m_T[i] - m_C[j];
		}

	// Get the correction angle on the arc
	m_ds[j] = atan2(Determinant(CQ, CP[0]), CQ * CP[0]);
	if (nCount > 1)
		{
		// Choose the smaller correction on the arc
		r = atan2(Determinant(CQ, CP[1]), CQ * CP[1]);
		if (fabs(r) < fabs(m_ds[j]))
			{
			// Choose the alternative correction pair
			m_ds[i] = d;
			m_ds[j] = r;
			}
		}
	
	// Convert distance to parameter on the line
	if (m_ds[i] != 0)
		m_ds[i] /= m_speed[i];

	// Convert angle to parameter on the arc
	ASSERT(m_speed[j] > FUZZ_GEN);
	m_ds[j] *= (m_rad[j] / m_speed[j]);

	return SUCCESS;
	}
/****************************************************************************/
// Get a correction based on circular extrapolations on both curves
RC CCurvesDistance::CorrectArcArc()
	{
	RC rc = SUCCESS;
	C3Point CP = m_P[0] - m_C[0];
	C3Point CQ = m_P[1] - m_C[1];
	C3Point V1 = m_C[1] - m_C[0];
	C3Point V2 = -V1;
	double d = V1.Norm();
	double rSmaller = fabs(m_rad[0]);
	double rLarger = fabs(m_rad[1]);
	double rSum = rSmaller + rLarger;
	double rFuzz = rSum * FUZZ_GEN;
	BOOL b2ndIsSmaller = rSmaller > rLarger;
	if (b2ndIsSmaller)
		SWAP(rSmaller, rLarger, double);

	if (d < rFuzz)
		{
		// The centers coincide, no correction possible
		m_ds[0] = m_ds[1] = 0;
		m_bSingular = TRUE;
		goto exit;
		}

	if (d < rSum - FUZZ_DIST)
		if (d > rLarger - rSmaller + rFuzz)
			{
			// The circles intersect
			// Find the angle to the intersections using the Cosine Theorem
			double r = 2 * rLarger * d;
			if (r < rFuzz)
				{
				m_bSingular = TRUE;
				QUIT
				}
			double rCos = (d * d + (rLarger + rSmaller) * 
								   (rLarger - rSmaller) ) / r;
			ASSERT(rCos > -1  &&  rCos < 1);
			double rSin = sqrt(1 - rCos * rCos);
			C3Point C, D;
			if (b2ndIsSmaller)
				{
				C = m_C[0];
				D = V1;
				}
			else
				{
				C = m_C[1];
				D = V2;
				}

			// Find the 2 intersection points
			if (rc = D.Unitize())
				QUIT
			D *= rLarger;
			C3Point A = D;
			A.TurnLeft();
			D *= rCos;
			A *= rSin;
			C3Point B = C + D + A;
					A = C + D - A;

			// Choose the one that will amount to a smaller correction
			r = (A - m_C[0]) * CP + (A - m_C[1]) * CQ;
			d = (B - m_C[0]) * CP + (B - m_C[1]) * CQ;
			if (d > r)
				{
				V1 = B - m_C[0];
				V2 = B - m_C[1];
				}
			else
				{
				V1 = A - m_C[0];
				V2 = A - m_C[1];
				}
			}	// End if intersecting circles
		else
			{
			// Smaller circle is contained withing the larger one
			if (b2ndIsSmaller)
				V2 = -V2;
			else
				V1 = -V1;
			} // End if one circle contained in the other

	//ASSERT_ELSE(CP.Norm() > FUZZ_GEN  &&  CQ.Norm() > FUZZ_GEN,"")
		//DB_MSGX("|CP|=%lf, |CQ|=%lf", (CP.Norm(), CQ.Norm()));
	//ASSERT_ELSE(V1.Norm() > FUZZ_GEN  &&  V2.Norm() > FUZZ_GEN, "")
		//DB_MSGX("|V1|=%lf, |V2|=%lf", (V1.Norm(), V2.Norm()));
	m_ds[0] = m_rad[0] * atan2(Determinant(CP, V1), CP * V1) / m_speed[0];
	m_ds[1] = m_rad[1] * atan2(Determinant(CQ, V2), CQ * V2) / m_speed[1];
exit:
	return rc;
	}
/****************************************************************************/
// Is the equation satisfied by the current pair?
BOOL CCurvesDistance::IsSatisfied()
	{
	C3Point D = m_P[0] - m_P[1];
	m_rSqDist = D * D;
	return m_rSqDist <= m_rMargin;
	}

/****************************************************************************/
// Find the distance of curves in X-Y plane
//
CCurvesXYDistance::CCurvesXYDistance()
{
}

CCurvesXYDistance::CCurvesXYDistance(
	PCurve pFirst,      // In: First curve
	PCurve pSecond,     // In: Second curve
	double rMargin) 	// In: X-Y Distance that is considered 0
	:CCurvesDistance(pFirst, pSecond, rMargin)
{
}

BOOL CCurvesXYDistance::IsSatisfied()
{
    C3Point D = m_P[0] - m_P[1];
	m_rSqDist = D.X() * D.X() + D.Y() * D.Y();
	return m_rSqDist < m_rMargin;
}

/****************************************************************************/
// Find the distance between two curves and nearest points on them
RC Distance(
	PCurve pFirst,      // In: First curve
	PCurve pSecond,		// In: Second curve
	double& rDist,		// In/Out: The distance, updated here
	double& r1,			// Out: Parameter value on first curve
	double& r2,			// Out: Parameter value on second curve
	C3Point& P1,		// Out: Point on first curve
	C3Point& P2,		// Out: Point on second curve
	double rMargin)		// Optional: Distance margin
	{
	// The distance is assumed to have a value.  This function will replace
	// it only if it finds a smaller distance between the curves. Otherwise,
	// no values are assigned to ri,r2,P1,P2.
	RC rc;
	CCurvesDistance solver(pFirst, pSecond, rMargin);
	int i,j;
	CRealArray arrSeeds1, arrSeeds2;
	double s, t, d;
	C3Point A, B;
	BOOL bFound = FALSE;
	
	// Internally we work with the squared distance
	rMargin *= rMargin;

	// Generate seeds on both curves
	if (rc = pFirst->Seeds(arrSeeds1))
		QUIT
	if (rc = pSecond->Seeds(arrSeeds2))
		QUIT

	// Loop on seeds
	for (i=0;  i<arrSeeds1.Size();  i++)
		for (j=0;  j<arrSeeds2.Size();  j++)
			{
			if (rc = solver.Solve(arrSeeds1[i], arrSeeds2[j])) 
				{
				if (rc == E_NOMSG)
					{
					// A minimum was not found from this pair of seeds
					rc = SUCCESS;
					continue;
					}
				else
					QUIT
				}
			
			// A minimum was found, update the current solution
			bFound = TRUE;
			if (Update(rMargin, solver.SqDist(), 
					   solver.Parameter(0), solver.Parameter(1),
					   solver.Point(0), solver.Point(1), 
					   rDist, r1, r2, P1, P2))
				// We're done
				goto exit;
			}

	// The distance to the second curve from cusps on the first curve
	s = pFirst->Start();
	while (!pFirst->NextCusp(s, s))
		{
		if (pFirst->Evaluate(s, A))
			continue;
		if (!pSecond->PullBack(A, B, t, d))
			{
			bFound = TRUE;
			if (Update(rMargin, d, s, t, A, B, rDist, r1, r2, P1, P2))
				// We're done
				goto exit;
			}
		}

	// The distance to the second curve from cusps on the first curve
	t = pSecond->Start();
	while (!pSecond->NextCusp(t, t))
		{
		if (pSecond->Evaluate(t, B))
			continue;
		if (!pFirst->PullBack(B, A, s, d))
			{
			bFound = TRUE;
			if (Update(rMargin, d, s, t, A, B, rDist, r1, r2, P1, P2))
				// We're done
				break;
			}
		}
exit:
	if (!bFound)
		rc = E_NOMSG;
	RETURN_RC(Distance, rc);
	}
//////////////////////////////////////////////////////////////////////////
//																		//
//				Finding a common tangent to 2 Curves					//

/************************************************************************/
// Get a correction based on circular extrapolations on both curves
RC CCommonTangent::CorrectArcArc()
	{
	RC rc = E_NOMSG;
	int i;
	double r, s, c, si[2], co[2], newsi[2], newco[2];
	m_ds[0] = m_ds[1] = 0;

	// This algorithm is not interested in the sign of the curvatures
	m_rad[0] = fabs(m_rad[0]);
	m_rad[1] = fabs(m_rad[1]);

	// Get some difference vectors
	C3Point CP[2];
	C3Point CC = m_C[1] - m_C[0];
	CP[0] = m_P[0] - m_C[0];
	CP[1] = m_P[1] - m_C[1];

	// Get the distance and a unit vector between the centers
	double d = CC.Norm();
	if (FZERO(d, MAX(m_rad[0], m_rad[1]) * FUZZ_GEN))
		{
		// Concentric circles, the solution is not clearly defined,
		// and currently not interesting.
		goto exit;
		}
	CC /= d;
		
	// Get unit vectors from centers to the points
	if (CP[0].Unitize() || CP[1].Unitize())
		// Singularity, no correction
		QUIT

	// Get the angles from the centers to the points
	si[0] = Determinant(CC, CP[0]);
	co[0] = CC * CP[0];
	si[1] = Determinant(CC, CP[1]);
	co[1] = CC * CP[1];

	// Retain signs of the sines
	newsi[0] = (si[0] >=0)? 1 : -1;
	newsi[1] = (si[1] >=0)? 1 : -1;

	// Both cosines are positive, unless it's a crossing tangent
	newco[0] = newco[1] = 1;

	// Get the new angle
	if (si[0] * si[1] >= 0)
		{
		r = m_rad[0] - m_rad[1];	// Tangent on the same side
		if (d < fabs(r))
			// Correction impossible, one circle included in the other
			QUIT
		}
	else
		{
		r = m_rad[0] + m_rad[1];	// Crossing tangent
		newco[1] = -1;
		if (d < r)
			{
			// Back to same side, crossing tangents are impossible
			r = m_rad[0] - m_rad[1];
			if (d < fabs(r))
				// No solution, one circle is included in the other
				QUIT

			newco[1] = -1;
			// Switch side on the smaller circle
			if (r < 0)
				newsi[0] = -newsi[0];
			else
				newsi[1] = -newsi[1];
			}
		}
	ASSERT(fabs(r) < d);
	c = r / d;
	s = MAX(1 - c * c, -1);
	s = sqrt(s);

	// Get the corrections
	for (i = 0;  i < 2;  i++)
		{
		// Get the right sign for the sin
		newsi[i] *= s;
		newco[i] *= c;

		// Get the differences between old and new angles
		d = co[i] * newco[i] + si[i] * newsi[i]; // cos(a-b)
		r = newsi[i] * co[i] - si[i] * newco[i]; // sin(a-b)
		m_ds[i] = atan2(r, d);

		// If the circle goes clockwise we need to reverse ds
		r = Determinant(CP[i], m_T[i]);
		if (r < 0)
			m_ds[i] = -m_ds[i];

		// Convert angle to parameter correction
		ASSERT(m_speed[i] > FUZZ_GEN);
		m_ds[i] *= (m_rad[i] / m_speed[i]);
		}
	rc = SUCCESS;
exit:
	RETURN_RC(CCommonTangent::CorrectArcArc, rc);
	}
/****************************************************************************/
// Get a correction based on circular extrapolations on both curves
BOOL CCommonTangent::IsSatisfied()
	{
	// We have a common tangent if the angle formed between the the chord 
	// that connects the points on the cures and each one of the tangents
	// there is less than 0.0001.
	C3Point D = m_P[0] - m_P[1];
	double d = (D * D) * 0.0001;
	return fabs(Determinant(m_T[0], D)) < d &&
		   fabs(Determinant(m_T[1], D)) < d;
	}
/************************************************************************/
RC CommonTangent(
	PCurve pFirst,	// In: First curve
	PCurve pSecond,	// In: Second curve
	double s1,		// In: Inital gess of Parameter first curve
	double s2,		// In: Inital gess of Parameter second curve
	double& t1,		// Out: Parameter of tangent point on first curve
	double& t2)		// Out: Parameter of tangent point on second curve
	{
	RC rc;
	CCommonTangent solver(pFirst, pSecond);
	if (rc = solver.Solve(s1, s2))
		QUIT
	t1 = solver.Parameter(0);
	t2 = solver.Parameter(1);
exit:
	RETURN_RC(CommonTangent, rc);
	}
///////////////////////////////////////////////////////////////////////////
// Implementation of C1CurveSolver

// Constructor  
C1CurveSolver::C1CurveSolver(
	PCCurve pCurve)  // In: The curve
	{
	Initialize();
	m_pCurve = pCurve;
	}

void C1CurveSolver::Initialize()
	{
	m_pCurve = NULL;
	m_s = m_ds = 0;
	m_speed = m_rad = 0;
	m_bSingular = FALSE;
	}
/****************************************************************************/
// Geometric Newton-Raphson iterative solver
RC C1CurveSolver::Solve( // Return E_NOMSG if no solution found
	double rSeed)		// In: Initial guess 
	{
/*
 This is a low-level utility, that identifies a pareamter value of a
 solution of a real function that involves a curves.  It returns 
 E_NOMSG no solution was found from this seed.

 The algorithm is iterative.  From the current guess we extrapolate on 
 the osculating circle if the curvature is nonzero, otherwise on the 
 tangnet line.  We find the exact solution for the line or arcs.  The
 correction to the next guess is the parameter increment that would produce
 the same distance along the curve as the solution point does on the 
 extrapolation, had the speed on the curve been constant, equal to the 
 speed at the current guess. This method converges in one step if the 
 curve is a straight lines or a circular arc.
 */
	RC rc = E_NOMSG;
    int i;
	double r;
	/*
	 If a curve is periodic, there is no point in making a correction
	 that is greater than the period.  In any case, restrict the 
	 correction to a reasonable margin.
	*/
	double rLimit = m_pCurve->End() - m_pCurve->Start();
	if (m_pCurve->Period()) 
		rLimit = m_pCurve->Period();
	else
		rLimit *= 100;

	// Start with the seed
	m_s = rSeed;

	// Iterate
	for (i=0;  i<=20;  i++)
		{
		// Get the details of the osculating circle or tangent line
		if (rc = GetCurveGeometry(m_pCurve, m_s,  m_P, m_T, m_C,
												m_speed, m_rad))
			QUIT

		// Check if the the current solution satisfies the equations
		if (IsSatisfied())
			// We're done
			break;

		// Get the appropriate correction
		if( m_rad == 0)
			// No correction possible
				break;
		else if(m_rad == INFINITY)
			// Zero curvature, work with the tangent line
			rc = CorrectLine();
		else	
			// Work with the osculating circle
			rc = CorrectArc();

		if (rc || m_bSingular)
			break;

		// Quit if the correction is too large
		if (fabs(m_ds) > rLimit)
			break;

		// Apply the correction
 		m_s += m_ds;

		// Break if the correction is no longer significant
		r = fabs(m_s);
		if (FZERO(m_ds, MAX(r, 1) * FUZZ_REAL))
			break;
 		}	// End of for i
exit:
	if (!m_pCurve->PullIntoDomain(m_s))
		rc = E_NOMSG;
	RETURN_RC(C1CurveSolver::Solve, rc);
	}
/****************************************************************************/
// Correction stub
RC C1CurveSolver::CorrectLine()
	{
	return Fail();
	}
/****************************************************************************/
// Correction stub
RC C1CurveSolver::CorrectArc()
	{
	return Fail();
	}
//////////////////////////////////////////////////////////////////////////
//																		//
//				Finding the nearest point on a curve					//

CPullBack::CPullBack()
	{
	m_rMargin = 0;
	}

CPullBack::CPullBack(
	PCCurve pCurve,		// In: A curve
	C3Point ptTo,		// In: A point
	double rMargin)		// In: Distance considered 0
	: C1CurveSolver(pCurve)
	{
	m_ptTo = ptTo;
	m_rMargin = rMargin;
	}
/****************************************************************************/
// Is the equation satisfied by the current pair?
BOOL CPullBack::IsSatisfied()
	{
	C3Point D = m_P - m_ptTo;
	m_rSqDist = D * D;
	return m_rSqDist < m_rMargin;
	}
/****************************************************************************/
// Get the correction from the tangent line
RC CPullBack::CorrectLine()
	{
/* The line is C(u) = m_P + u*m_T.  We are looking for u such that
   (m_ptTo - C(u)) is perpendicular to the line, that is
   (m_P + u*m_T - m_ptTo) * m_T = 0, or u*(m_T*m_T) = (m_ptTo - m_P)*m_T,
   but m_T*m_T = 1, so the parameter on the line is:
*/
	ASSERT(FEQUAL(m_T.Norm(), 1, .001));
	m_ds = m_T * (m_ptTo - m_P);

	// Converted to a correction on the curve:
	ASSERT(m_speed > FUZZ_GEN);
	m_ds /= m_speed;

	RETURN_RC(CPullBack::CorrectLine, SUCCESS);
	}
/****************************************************************************/
// Get the correction from the osculating circle
RC CPullBack::CorrectArc()
	{
	RC rc = SUCCESS;

	// Get vectors from the center to the points in question 
	C3Point CP = m_P - m_C;
	C3Point V = m_ptTo - m_C;

	// The correction on the arc is the angle from the previous guess
	if (Angle(CP, V, m_ds))
		{
		// The point lies in the center of the circle, we're stuck
		m_bSingular = TRUE;
		m_ds = 0;
		}
	else
		{
		// Get the correction on the curve from the correction on the arc
		ASSERT(m_speed > FUZZ_GEN);
		m_ds = m_ds * m_rad / m_speed;
		}

	RETURN_RC(CPullBack::CorrectArc, rc);
	}

//////////////////////////////////////////////////////////////////////////
//																		
// Find the nearest point on the curvewithin tolerance in X-Y range.
//	
// Author:  Stephen W. Hou
// Date:    7/13/2004			                                                       

CXYPullBack::CXYPullBack(
	PCCurve pCurve,		// In: A curve
	C3Point ptTo,		// In: A point
	double rMargin)		// In: Distance considered 0
    : CPullBack(pCurve, ptTo, rMargin)
{
}

/****************************************************************************/
// Get the correction from the tangent line
RC CXYPullBack::CorrectLine()
{
    C3Point dir(m_T), ptTo(m_ptTo), point(m_P);
    dir.SetZ(0.0);
    ptTo.SetZ(0.0);
    point.SetZ(0.0);
    double len = dir.Norm();
    if (FZERO(len, FUZZ_DIST)) 
        m_ds = 0.0;
    else {
	    m_ds = dir * (ptTo - point) / len;

	    // Converted to a correction on the curve:
	    m_ds /= m_speed * len;
    }
	RETURN_RC(CPullBack::CorrectLine, SUCCESS);
}

BOOL CXYPullBack::IsSatisfied()
{
    C3Point D = m_P - m_ptTo;
	m_rSqDist = D.X() * D.X() + D.Y() * D.Y();
	return m_rSqDist < m_rMargin;
}

