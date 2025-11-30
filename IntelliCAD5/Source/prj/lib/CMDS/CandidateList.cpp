/* C:\ICADDEV\PRJ\LIB\CMDS\CANDIDATELIST.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * This is the implementation of the CCandidateList class.
 * A candidate list is a list of candidates for BPOLY or BHATCH
 * construction.  See CandidateList.h for the definition of the class
 */ 

//Precompilation
#include "cmds.h"/*DNT*/
#include "Icad.h"/*DNT*/

#include "Geometry.h"/*DNT*/
#include "Extents.h"/*DNT*/
#include "Path.h"/*DNT*/
#include "Region.h"/*DNT*/
#include "Boolean.h"/*DNT*/
#include "CandidateList.h"/*DNT*/


// Set the depth of island detection
void CandidateList::SetDepth(
	int value)	// In: The value to set to
	{
	m_RecursionDepth = value;

// Island detection involves random number generation.  The following
// produce the same random numbers with any given input, making it
// easier to debug
	if (value)
		srand(0);
	}

// Set the island detection option
void CandidateList::SetIslandOption(IslandOption option)
{
	m_IslandOption = option;
}

// Get the island detection option
IslandOption CandidateList::GetIslandOption()
{
	return m_IslandOption;
}

// Start a new candidate (Fragment callback)
RC CandidateList::NewShape()
	{
	RC rc;
	m_jPath = 0; // we are starting a new region, so reset the path index to 0
	PCandidate pCandidate = new Candidate;
	if (!pCandidate)
		{
		rc = DM_MEM;
		goto exit;
		}

	if (rc = m_List.Add(pCandidate))
		delete pCandidate;
exit:
    RETURN_RC(CandidateList::NewShape, rc);
	}


// Start a new path in the current candidate (Fragment callback)
RC CandidateList::NewPath(BOOL bFillable)
	{
    // add a new path to the current region
	// NOTE:  We are making it a CBoundary which is a subclass of CPath,
	// because CBoundary behaves the way we want it to when doing HitTest,
	// but CPath does not.
	// Another NOTE:  In the current 1/29/99 version of GEO we revert back
	// to using PPath and CPath.
	
	PPath pPath = new CPath;
	RC rc = ALLOC_FAIL(pPath);
	
	if (!rc)
		rc = CurrentRegion()->Add(pPath);
	
	if (!rc)
		{
		m_ObjHandleRecord = new ObjHandleRecord;
		
		if (m_ObjHandleRecord)
			{
			m_ObjHandleRecord->i_Region = m_iRegion;
			m_ObjHandleRecord->j_Path = m_jPath;
			}
		else
			rc = FAILURE;
		}
	
	RETURN_RC(CandidateList::NewPath, rc);
	}


// Add a curve to the current path (Fragment callback)
RC CandidateList::AcceptCurve(
	PCurve pCurve,	// In: The curve to add
	GeoID ID)			// In: The associated entity ID 
	{
	// add the input curve to the current path under construction

	pCurve->UpdateExtents(false, CurrentPath()->Size()==0, 
						  CurrentExtents()); 
	RC rc = CurrentPath()->Add(pCurve);

	if (!rc)
		{
		// first check for duplicates
		bool already_in_list = false;
		stdObjHandleList::const_iterator p;
		
		for (p = m_ObjHandleRecord->HandleList.begin();
		p != m_ObjHandleRecord->HandleList.end();
		p++)
			{   // begin p-loop
			if (*p == *ID)
				{
				already_in_list = true;
				break;
				}
			}   //   end p-loop
		if (!already_in_list)
			{
			m_ObjHandleRecord->HandleList.push_back(*ID);
			}
		}


	RETURN_RC(CandidateList::AcceptCurve, rc);
	}


// End of current path 
RC CandidateList::WrapUpPath()
	{
	RC rc = CurrentPath()->WrapUp();
	m_ObjHandles.push_back(m_ObjHandleRecord);
	++m_jPath; // increment the index to prepare for the next path in this shape

	RETURN_RC(CandidateList::WrapUpPath, rc);
	}



// End of current candidate construction (Fragment callback)
RC CandidateList::WrapUpShape(GeoID idSource,
		                      BOOL bSkipText,
				              BOOL bSkipFill)
	{
	RC rc = SUCCESS; // for now
	++m_iRegion; // increment the index to prepare for the next region

	RETURN_RC(CandidateList::WrapUpShape, rc);
	}


	
// tell whether the i-th region is in the final list
bool CandidateList::IsFinalist( // TRUE if the region is a finalist
	int i)	// In: An index in the list
	{
	return Item(i)->IsFinalist();
	}

bool CandidateList::IsHatchFinalist( // TRUE if the region is a hatch finalsit
									int i) // In an index in the list
	{
	return Item(i)->IsHatchFinalist();
	}


// tell whether the i-th region is among those most recently selected
bool CandidateList::IsMostRecentlySelected(int i)
{
	return Item(i)->IsMostRecentlySelected();
}

// Get the ith candidate
PCandidate CandidateList::Item(	// Return the ith candidate
	int i)	// In: The index of the desired candidate
	{
	return (PCandidate)m_List[i];
	}



// Respond to the user's clicking a point
RC CandidateList::ClickPoint( // Return E_NoHit if no hit found
							 const C3Point & P) // In:  The point
{
	RC rc = NO_HIT; // Until we find a hit
	int i;
	int iHit = -1;
	for (i = 0;   rc < 0 && i < Size();   i++)
	{  
		if (Item(i)->Contains(P))
		{
			iHit = i;
			rc = SUCCESS;
		}
		if (!rc)
		{
			Item(i)->SetMostRecentlySelected(true, m_RecursionDepth);

			if (m_ToggleHatch)
			{
				Item(i)->Toggle(m_RecursionDepth);
				Item(i)->ToggleHatchFinalist(MAX_ISLAND_DEPTH);
			}
			else
			{
				Item(i)->SetFinalist(true, m_RecursionDepth);
				Item(i)->SetHatchFinalist(true, MAX_ISLAND_DEPTH);
			}
		}
	}  
	
	return rc;
}


// Construct the trees of island containment
RC CandidateList::GetIslands()
	{
	RC rc = SUCCESS;
	// Find containers for all the islands
	for (int i = 0;   !rc  &&  i < Size();   i++)
		{
		// Find its container
		int container = Item(i)->GetContainer(this);

		if (container >= 0)
			// Add this candidate to its container as an island 
			rc = Item(container)->AddAsIsland(Item(i));
		}

	RETURN_RC(CandidateList::GetIslands, rc);
	}


// Get the current region under construction
PRegion CandidateList::CurrentRegion()	// Return the current region
	{
	return Item(Size() - 1)->Region();
	}



// Get the current path under construction
// We know, from the way we constructed it, that it must be a PBoundary
// hence we can cast it to one before returning.
// No casting is required
PPath CandidateList::CurrentPath() // Return the current path
	{
	PRegion p = CurrentRegion();
	return p->Path(p->Size() - 1);
	}// Get the current path under construction



// Return the reference of the extents of the current region
CExtents& CandidateList::CurrentExtents() // Return the current extents
	{
	PCandidate p = Item(Size() - 1);
	return p->Extents();
	}


// Returns the size of the list
int CandidateList::Size()
	{
	return m_List.Size();
	}

// Sets MostRecentlySelected states to false for all candidates
void CandidateList::ClearMostRecentlySelected()
{
	for (int i = 0; i < Size(); i++)
	{  // begin i-loop
		Item(i)->SetMostRecentlySelected(false, m_RecursionDepth);
	}  //   end i-loop
}

// Determine the number of hatch paths in this candidate list.
// Note that this number now depends upon the island option
int CandidateList::NumHatchPaths()
	{
	int pathCount = 0;
	
	for (int i = 0; i < Size(); i++)
		{
		if (Item(i)->IsFinalist()) // had been IsHatchFinalist()
			{
			pathCount += Item(i)->Region()->Size();
			}
		}
	
	return pathCount;
	}

// Returns true if a candidate is already selected and false otherwise.
//
bool CandidateList::HasSelectedCandidate(const C3Point &point)
{
	for (register i = 0; i < Size(); i++) 
		if (Item(i)->IsFinalist() && Item(i)->Contains(point))
			return true;

	return false;
}

