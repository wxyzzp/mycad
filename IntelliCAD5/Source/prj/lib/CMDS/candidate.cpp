/* Candidate.cpp - implementation of the Candidate
   Copyright (C) 1998 Visio Corporation. All rights reserved.

  ABSTRACT
  This class represents a candidate for BPOLY or BHATCH construction.
  See Candidate.h for the definition of the class
*/
//Precompilation
#include "cmds.h"/*DNT*/
#include "Icad.h"/*DNT*/

#include <float.h>		// For DBL_MAX
#include "Geometry.h"
#include "Extents.h"
#include "Line.h"
#include "CrvPair.h"
#include "Path.h"
#include "Region.h"

#include "Candidate.h"
#include "CandidateList.h"

#include "assert.h" // debugging only


Candidate::Candidate()
{
	m_IsFinalist = false;
	m_IsHatchFinalist = false;
	m_IsMostRecentlySelected = false;
	m_IsCurrentlySelected = false;
	m_pParent = NULL;
}

Candidate::~Candidate()
{
	for(int i = 0; i < m_Islands.Size(); i++)
		Island(i)->m_pParent = NULL;

	if(m_pParent)
		for(i = 0; i < m_pParent->m_Islands.Size(); i++)
			if(m_pParent->Island(i) == this)
				m_pParent->m_Islands.Remove(i);

	m_Islands.RemoveAll();
}


// Return true if this candidate contains this point
bool
Candidate::Contains(					// Return TRUE if the region contains P
					const C3Point & P)	// In: A point
{
	int result;
	bool contains = false;

	if (m_Extents.Contain(P))
		if (!m_Region.HitTest(P, 0, result))
			contains = (result == HIT_INSIDE);

	return contains; 
}


// See if this candidate (set by ray) is an island in a given path
LOCAL RC
UpdateContainer(
				PPath   pPath,			// In: The path to check containment
				PRay    pRay,			// In: A ray
				double& intercept,		// In/Out: The first intercept with this path, updated here
				bool&   bCloser,		// Out: =TRUE if this is a closer container 
				bool    OnlyAsIsland)	// In: true if we checking candidate only as an island, false if as any hole
{
/*
 This is cloned from CBooleanPath::Locate.  It would have been better
 to reuse that code, but the path there is different than PPath, so
 Curve(i) there means something different
 */

	RC rc = SUCCESS;
	bCloser = false;
	int i,j;
	int nIntercepts = 0;
	double rAt;
	C3Point P, T;
	double rClosestIntercept = DBL_MAX;

	for (j=0;  j<pPath->Size();  j++)
	{
		CRealArray cOnRay, cOnCurve;
		PCurve pCurve = pPath->Curve(j);

		if (rc = Intersect(pRay, pCurve, cOnRay, cOnCurve))
			break;

		// Analyze the curve intersection points
		for (i = 0;    i < cOnCurve.Size();    i++)
		{
			rAt = cOnCurve[i];
			if (FEQUAL(rAt, pCurve->Start(), FUZZ_GEN)  ||
			    FEQUAL(rAt, pCurve->End(), FUZZ_GEN))
			{
				// Vertex intercepts are not acceptable
				rc = E_NOMSG;
				goto exit;
			}

			// Eliminate grazing intercepts
			if (rc = pCurve->Evaluate(rAt, P, T))
				QUIT
			if (FZERO(Determinant(T, pRay->Direction()), FUZZ_DIST))
			{
				rc = E_NOMSG;
				goto exit;
			}
		}

		// Analyze the ray intercepts
		if (cOnRay.Size() > 0)
		{
			rAt = cOnRay[0];
			if (OnlyAsIsland && FZERO(rAt, FUZZ_DIST))	/*D.G.*/// OnlyAsIsland added.
				// this is a container, but it's too close
				goto exit;
						   
			if (rAt < rClosestIntercept)
				rClosestIntercept = rAt;
			nIntercepts += cOnRay.Size();
		}
	}
		
	if (!ODD(nIntercepts))  // This is not a container
		goto exit;
	if (rClosestIntercept >= intercept) // No closer than the previous container
		goto exit;

	// Passed the hurdles, this is a closer container
	intercept = rClosestIntercept;
	bCloser = true;

exit:
	return rc;
}



// Find the tightest container (as an island or as any hole) for this candidate
int
Candidate::GetContainer(								// The index of a container, or -1 if none
						PCandidateList pList,			// In: The list to search in
						bool OnlyAsIsland  /*= true*/)	// In: true if we checking candidate only as an island, false if as any hole
{
/*
 This method looks for a candidate in the list that contains this one
 as an island in one of its holes.
 */
	RC rc = E_NOMSG;
	int i,j,k,m;
	int container = -1;

/* 
 The containment test relies on the boundary structure of the regions
 as created by CFragment: There is only one outer boundary (the rest 
 are holes), it is always the first path, and it goes counterclockwise.
 We therefore cast a ray from it to the right, which is outwards, and
 out of all the other regions' holes, look for one that intersects it
 an odd number of times, none of which at the origin, with the intercept
 closest to the ray's origin.  Degenerate intersection (at vertices or
 along a tangent) make it hard to determine whether the ray is really
 crossing the path, so we reject the test if they occurr.  To stay 
 away from them, we cast the ray from a random point at a random 
 direction, and make 8 attempts at that.
 */

	for (i = 1;   rc && i < 8;   i++)
	{
		PPath pPath = OuterBoundary();
		for (j = 0;   rc && j < pPath->Size();   j++)
		{
			// For all curves in the OuterBoundary
			PCurve pCurve = pPath->Curve(j);
			double r;
			double intercept = DBL_MAX;
			container = -1;
			PRay pRay = NULL;
			C3Point P, T;
			
			// Get a random point on the curve
			if (rc = pCurve->RandomPoint(r, P, T))
				continue;
                                     
			//Cast a ray from that point outwards
			T.RandomTurnLeft();
			T = -T;
			pRay = new CRay(P, T);
			if (!pRay)
			{
				rc = DM_MEM;
				goto exit;
			}
																								  
			// Loop on the candidates in the list
			for (k = 0;   k < pList->Size();    k++)
			{
				bool bReplace;
				PCandidate candidate = pList->Item(k);
				if (candidate == this)
					// Not interested in self-containment
					continue;

				// Trivial elimination
				if (!candidate->Extents().Contain(P))
					continue;

				for (m = 0;   m < candidate->HoleCount();   m++)
				{
					PPath pHole = candidate->Hole(m);
					if (rc = UpdateContainer(pHole, pRay, 
											 intercept, bReplace, OnlyAsIsland))
						break;
					if (bReplace)
						container = k;

				}	// End of m-loop on holes
			}  // End of k-loop on candidates
				delete pRay;
		}	// End of j-loop on curves
	}	// End of i-loop on attempts

exit:
	return container;
}



// Add an island to the list of this candidate's islands
RC
Candidate::AddAsIsland(
					   PCandidate pIsland)	// In: An island to add to this candidate
{
	RC rc = m_Islands.Add(pIsland);

	if(rc == SUCCESS)
		pIsland->m_pParent = this;

	RETURN_RC(Candidate::AddAsIsland, rc);
}


// Return the current Finalist status 
bool
Candidate::IsFinalist()
{
	return m_IsFinalist;
}

bool
Candidate::IsHatchFinalist()
{
	return m_IsHatchFinalist;
}


// Toggle the Finalist status to the opposite value
void
Candidate::Toggle(
				  int depth)	// In: island detection depth
{
	SetFinalist(!m_IsFinalist, depth);
}

// Toggle the HatchFinalist status to the opposite value
void
Candidate::ToggleHatchFinalist(
							   int depth) // In: island detection depth
{
	SetHatchFinalist(!m_IsHatchFinalist, depth);
}


// Set the IsFinalist status to a given value
void
Candidate::SetFinalist(
					   bool value,	// In: The value to set to
					   int depth)	// In: island detection depth
{
	m_IsFinalist = value;

	if (depth > 0)
		for (int i = 0;   i < m_Islands.Size();   i++)
			Island(i)->SetFinalist(value, depth-1);
}

// Set the IsHatchFinalist status to a given value
void
Candidate::SetHatchFinalist(
							bool value,	// In: The value to set to
							int depth)	// In: island detection depth
{
	m_IsHatchFinalist = value;

	if (depth > 0)
		for (int i = 0;   i < m_Islands.Size();   i++)
			Island(i)->SetHatchFinalist(value, depth-1);
}

// Return the current MostRecentlySelected status
bool
Candidate::IsMostRecentlySelected()
{
	return m_IsMostRecentlySelected;
}

// Set the MostRecentlySelected status to a given value
void
Candidate::SetMostRecentlySelected(
								   bool value,     // In: The value to set to
								   int  depth)     // In: island detection depth
{
	m_IsMostRecentlySelected = value;

	if (depth > 0)
		for (int i = 0; i < m_Islands.Size(); i++)
			Island(i)->SetMostRecentlySelected(value, depth-1);
}

// Return the current CurrentlySelected status
bool
Candidate::IsCurrentlySelected()
{
	return m_IsCurrentlySelected;
}

// Set the CurrrentlySelected status to a given value
void
Candidate::SetCurrentlySelected(
								bool value,     // In:  The value to set to
								int depth)      // In:  island detection depth
{
	m_IsCurrentlySelected = value;

	if (depth > 0)
		for (int i = 0; i < m_Islands.Size(); i++)
			Island(i)->SetCurrentlySelected(value, depth-1);
}

// Return a pointer to the region
PRegion
Candidate::Region()
{
	return &m_Region;
}


PCandidate
Candidate::Island(
				  int i)	// In: The index of the desired island
{
	return (PCandidate)(m_Islands[i]);
}


CExtents&
Candidate::Extents()
{
	return m_Extents;
}


// Get the number of holes in the region
int
Candidate::HoleCount()
{
	return m_Region.Size() - 1;
}


// Get the ith hole in the region
PPath
Candidate::Hole(
				int i)	// Index of the desired hole
{
/*
 The algorithm relies on the boundary structure of the candidate as
 generated by CFragment: The first path Path(0) is the outer boundary,
 and the rest are holes, so:
 */
	return m_Region.Path(i + 1);
}

PPath
Candidate::OuterBoundary()
{
	// By construction of the m_Region, the first path is 
	// always the outer boundary
	return m_Region.Path(0);
}


// Return a candidate for which this one is an island
PCandidate
Candidate::GetParent()
{
	return m_pParent;
}

// Return "true" if the candidate has a part (or whole) of its boundary path that doesn't coincide with
// boundary paths of other candidates in the given list.
// TO DO. It doesn't work for some cases (see comments below).
bool
Candidate::IsOpen(
			PCandidateList pList)	// In: The list to search in
{
	PPath  pThisBoundaryPath = OuterBoundary();

	for(int iThisCurve = 0; iThisCurve < pThisBoundaryPath->Size(); iThisCurve++)
	{
		PCurve  pThisCurve = pThisBoundaryPath->Curve(iThisCurve);
		C3Point  ThisStartPt, ThisEndPt;
		
		pThisCurve->GetStartPoint(ThisStartPt);
		pThisCurve->GetEndPoint(ThisEndPt);

		BOOL	bAreDuplicates = FALSE;

		for(int iCandidate = 0; iCandidate < pList->Size(); iCandidate++)
		{
			if(pList->Item(iCandidate) == this)
				continue;

			PPath  pCurrentBoundaryPath = pList->Item(iCandidate)->OuterBoundary();

			for(int iCurrentCurve = 0; iCurrentCurve < pCurrentBoundaryPath->Size(); iCurrentCurve++)
			{
				PCurve  pCurrentCurve = pCurrentBoundaryPath->Curve(iCurrentCurve);
				C3Point  CurrentStartPt, CurrentEndPt;
				
				pCurrentCurve->GetStartPoint(CurrentStartPt);
				pCurrentCurve->GetEndPoint(CurrentEndPt);

				if( (PointDistance(ThisStartPt, CurrentStartPt) < FUZZ_DIST  &&  PointDistance(ThisEndPt, CurrentEndPt) < FUZZ_DIST) ||
					(PointDistance(ThisStartPt, CurrentEndPt) < FUZZ_DIST  &&  PointDistance(ThisEndPt, CurrentStartPt) < FUZZ_DIST) )
				{
					if(AreDuplicates(pThisCurve, pCurrentCurve, FUZZ_DIST, bAreDuplicates) != SUCCESS)
						continue;
					if(bAreDuplicates)
						break;
				}
				else	// TO DO. Implement this case (because pThisCurve may coincide with some curves).
				{
					RC lRC = 0;
					CRealArray cOnThis;
					CRealArray cOnCurrent;

					lRC = Intersect( pThisCurve, pCurrentCurve, cOnThis, cOnCurrent, FUZZ_DIST );

					if( lRC != SUCCESS )
					{
						continue;
					}

					if( cOnThis.Size() > 0 || cOnCurrent.Size() > 0 )
					{
						bAreDuplicates = true;
						break;
					}
				}
			} // for( iCurrentCurve

			if(bAreDuplicates)
				break;
		} // for( iCandidate

		if(!bAreDuplicates)
			return true;
	} // for( iThisCurve

	return false;
}
