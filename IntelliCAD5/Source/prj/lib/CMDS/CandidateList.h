/* C:\ICADDEV\PRJ\LIB\CMDS\CANDIDATELIST.H
   Copyright (C) 1998 Visio Corporation. All rights reserved.

 * Abstract
 * Definition of the class CandidateList.
 * A Candidate list is a list of candidates for BPOLY or BHATCH 
 * construction.  It is populated by the results of a Fragment 
 * operation, so it is derived from CStorage and presented to the
 * Fragment operation as the recepient.
 */

#ifndef CANDIDATELIST_H
#define CANDIDATELIST_H

// for debugging purposes only
#include <iostream.h>
#include <fstream.h>

#include "TheUser.h"/*DNT*/	// For the base class
#include "Candidate.h"/*DNT*/
#include "Path.h"/*DNT*/ // For PPath
#include "Bhatchinfo.h"

#define MAX_ISLAND_DEPTH 24
enum IslandOption {Ignore_Islands, Outer_Only, Nested_Islands};

class CandidateList : public CStorage
{
public:
	// Constructors

	// default constructor
	CandidateList()
	{
		m_debugFile = NULL;
		m_RecursionDepth = 0;
		m_IslandOption = Ignore_Islands;
		m_iRegion = 0;
		m_jPath = 0;
		m_ToggleHatch = true;
	}

	// constructor for debugging
	CandidateList(ofstream fptr)
	{
		m_debugFile = fptr;
		m_RecursionDepth = 0;
		m_IslandOption = Ignore_Islands;
		m_iRegion = 0;
		m_jPath = 0;
		m_ToggleHatch = true;
	}


	// constructor that will be used for production
	CandidateList(CBoundaryBuilder * pOperation)
	{
		m_pOperation = pOperation;
		m_debugFile = NULL;
		m_RecursionDepth = 0;
		m_IslandOption = Ignore_Islands;
		m_iRegion = 0;
		m_jPath = 0;
		m_ToggleHatch = true;
	}

	// constructor that will be used for production testing
	CandidateList(CBoundaryBuilder * pOperation,
		           ofstream fptr)
	{
		m_pOperation = pOperation;
		m_debugFile = fptr;
		m_RecursionDepth = 0;
		m_IslandOption = Ignore_Islands;
		m_iRegion = 0;
		m_jPath = 0;
   		m_ToggleHatch = true;
	}

	
	// Destructor
	~CandidateList()
	{
		// does nothing right now.
	}

	// Methods
	void SetDepth(
		int value);	// In: The value to set to

	void SetIslandOption(IslandOption option);

	IslandOption GetIslandOption();

    virtual RC NewShape();

	virtual RC NewPath(BOOL bFillable);

	virtual RC AcceptCurve(PCurve pCurve,
		                   GeoID ID);

	virtual RC WrapUpPath();

	virtual RC WrapUpShape(GeoID idSource,
		                   BOOL bSkipText,
				           BOOL bSkipFill);

	// tell whether if a candidate is selected already
	//
	bool HasSelectedCandidate(const C3Point &point);

    // tell whether the i-th region is in the final list
	bool IsFinalist(int i); 

	// tell whether the i-th region is in the hatch finalist list
	bool IsHatchFinalist(int i);

	// tell whether the i-th region is among those most recently selected
	bool IsMostRecentlySelected(int i);

	int Size();

	PCandidate Item(int i);

	RC GetIslands();

	RC ClickPoint(			// Return E_NoHit if no hit found
		const C3Point & P); // In:  The point

	PRegion CurrentRegion(); // Return the current region

	PPath CurrentPath(); // Return the current path

	CExtents & CurrentExtents();	// Return the current extents

	void ClearMostRecentlySelected(); // Sets MostRecentlySelected states to false for
	                                  // all candidates

	int NumHatchPaths(); // return the number of paths (independent of island option).
	
	bool GetToggleHatch() {return m_ToggleHatch;}
	void SetToggleHatch(bool toggle) {m_ToggleHatch = toggle;}

// NOTE: public (for access in cmd_hatch_objects). TO DO: make accessor if need.
public:
	int m_RecursionDepth;			// Island detection depth
		      
protected:
	// Data	
	ofstream m_debugFile;			// for debug purposes only
	PBoundaryBuilder m_pOperation;  // the owning operation pointer; e.g., BPOLY or BHATCH
	IslandOption m_IslandOption;    // Island detection option
	CGeoObjectArray m_List;			// The list of candidates
	int m_iRegion;                  // Current region being processed
	int m_jPath;                    // Current path being processed
	ObjHandleRecord * m_ObjHandleRecord; // current ObjHandleRecord being processed

public:
	stdObjHandleRecordList m_ObjHandles; // list of objhandles
	bool m_ToggleHatch;				// Only want to toggle the hatch during create - not modify.
};

#endif // CANDIDATELIST_H

