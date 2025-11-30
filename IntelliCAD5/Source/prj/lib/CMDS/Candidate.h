/* Candidate.h - definition of the Candidate class
   Copyright (C) 1998 Visio Corporation. All rights reserved.

  ABSTRACT
  A candidate is a decorated region.  The additional information is
  its status, its list of islands, and a bounding box.
	
  This file also contains definitions that need to be seen by the
  all boundary construction classes, so it should be included before
  CandidateList.h and BoundaryBuilder.h
*/
#ifndef CANDIDATE_H
#define CANDIDATE_H

#include "Array.h"/*DNT*/
#include "Extents.h"/*DNT*/
#include "Region.h"/*DNT*/

#define NO_HIT		-2

// Forward declaration for everybody in this game
class Candidate;
typedef Candidate* PCandidate;

class CandidateList;
typedef CandidateList * PCandidateList;

class CBoundaryBuilder;
typedef CBoundaryBuilder * PBoundaryBuilder;

// Definition of Candidate
class Candidate	: public CGeoObject
{
public:

	// Constructor, destructor
	Candidate();
	virtual ~Candidate();
	
	bool Contains(				// Return TRUE if the region contains P
		const C3Point & P);		// In: A point

	int GetContainer(				// The index of a container, or -1 if none
		PCandidateList pList,		// In: The list to search in
		bool OnlyAsIsland = true);	// In: true if we checking candidate only as an island, false if as any hole

	RC AddAsIsland(
		PCandidate pIsland);	// In: An island to add to this candidate
 
	bool IsFinalist();

	bool IsHatchFinalist();

	void Toggle(
		int depth);				// In: island detection depth

	void ToggleHatchFinalist(
		int depth);				// In: island detection depth

	void SetFinalist(
		bool value,				// In: The value to set to
		int depth);				// In: island detection depth

	void SetHatchFinalist(
		bool value,				// In: The value to set to
		int depth);				// In: island detection depth

	bool IsMostRecentlySelected();

	void SetMostRecentlySelected(
		bool value,				// In: The value to set to
		int  depth);			// In: island detection depth

	bool IsCurrentlySelected();

	void SetCurrentlySelected(
		bool value,				// In:  The value to set to
		int depth);				// In:  island detection depth

	PRegion Region();

	CExtents& Extents();

	PCandidate Island(
		int i);					// In: The index of the desired island

	int HoleCount();

	PPath Hole(
		int i);					// Index of the desired hole

	PPath OuterBoundary();

	PCandidate GetParent();

	bool IsOpen(
		PCandidateList pList);	// In: The list to search in

	//Data
protected:
	CRegion			m_Region;		// The region
	bool			m_IsFinalist;	// is this a finalist (for BPOLY)?
	bool            m_IsHatchFinalist; // is this a finalist (for BHATCH)?
	bool            m_IsMostRecentlySelected; //has this been selected most recently?
    bool            m_IsCurrentlySelected; // is this currently selected?
	CGeoObjectArray	m_Islands;		// List of contained islands
	CExtents		m_Extents;		// Bounding box
	PCandidate		m_pParent;		// the pointer to the candidate for which this one is an island
};

#endif
