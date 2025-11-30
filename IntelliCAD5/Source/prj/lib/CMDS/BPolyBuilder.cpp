/* C:\ICADDEV\PRJ\LIB\CMDS\BPOLYBUILDER.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 
//Precompilation
#include "cmds.h"/*DNT*/
#include "Icad.h"/*DNT*/

#include "Geometry.h"
#include "Extents.h"
#include "Path.h"
#include "Region.h"
#include "Boolean.h"

#include "ListColl.h"
#include "GeoConvert.h"

#include "BPolyBuilder.h"
#include "IcadApi.h"

// constructor (had been in BPolyBuilder.h in previous versions)
//Bugzilla No. 78414 ; 01-30-2003 
#ifdef VARYING_HATCH_TOLERANCE
CBPolyBuilder::CBPolyBuilder(
							 C3Point ptNormal,	 // In:	 Extrusion vector for ECS
							 RC & rc)           // Out: Return code
							 : CBoundaryBuilder(),
							 m_ptNormal(ptNormal)
#else
CBPolyBuilder::CBPolyBuilder(double rTolerance, // In:  Geometric tolerance
							 C3Point ptNormal,	 // In:	 Extrusion vector for ECS
							 RC & rc)           // Out: Return code
							 : CBoundaryBuilder(rTolerance),
							 m_ptNormal(ptNormal)
#endif //VARYING_HATCH_TOLERANCE
	{
	// set the m_bLightWeight data member according to the setting
	// of the PLINETYPE system variable.
	
	resbuf plrb;
	
	SDS_getvar(NULL,DB_QPLINETYPE,&plrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if ((plrb.resval.rint==1) || (plrb.resval.rint==2))
		m_bLightWeight=TRUE; 
	else
		m_bLightWeight=FALSE;

	rc = SUCCESS;

	}

// Persist the most recently selected regions
// the output parameter finalists signifies whether they all are
// finalists.  Because of our implementation, the most recently
// selected regions are either all finalists or all not finalists
RC CBPolyBuilder::PersistMostRecentlySelected(bool & AreFinalists)
{
	RC rc = SUCCESS;
	
	AreFinalists = false;
	
    bool decided = false;
	
	//clear previous list
	if (m_pHanditemCollection)
		m_pHanditemCollection->ClearList();

    bool OuterOnly;
	if (m_pCandidates->GetIslandOption() == Ignore_Islands)
		OuterOnly = true;
	else
		OuterOnly = false;

	for (int i = 0; !rc && i < m_pCandidates->Size(); i++)
	{
		if (m_pCandidates->IsMostRecentlySelected(i))
		{
			// make the call to the pure virtual function PersistRegion
			// that will then be dispatched to the concrete function
			// implemented in the appropriate subclass
			rc = PersistRegion(i, OuterOnly); 
			
			if (!decided)
			{   // begin deciding
				AreFinalists = m_pCandidates->IsFinalist(i);
				decided = true;
			}   //   end deciding
		}
	}
	
	RETURN_RC(CBoundaryBuilder::Persist, rc);
}


// Persist the finalists
RC CBPolyBuilder::Persist()
	{
	RC rc = SUCCESS;

	//clear previous list
	if (m_pHanditemCollection)
		m_pHanditemCollection->ClearList();

	bool OuterOnly;
	if (m_pCandidates->GetIslandOption() == Ignore_Islands)
		OuterOnly = true;
	else
		OuterOnly = false;

	for (int i = 0; !rc && i < m_pCandidates->Size(); i++)
		{
		if (m_pCandidates->IsFinalist(i))
			{
			// make the call to the pure virtual function PersistRegion
			// that will then be dispatched to the concrete function
			// implemented in the appropriate subclass
			rc = PersistRegion(i, OuterOnly); 
			}
		}

	RETURN_RC(CBPolyBuilder::Persist, rc);
	}


// Persist a polyline boundary set
RC CBPolyBuilder::PersistRegion(int i,          // In:  The index of the region to persist
								bool OuterOnly) // In: true if we only persist the outer path
{
	RC rc = SUCCESS;
	int j;
	PRegion pRegion = m_pCandidates->Item(i)->Region();

	int itop;

	if ( OuterOnly )
		itop = 1; // just persist the outer path, which has index 0 by construction
	else
		itop = pRegion->Size(); // persist all paths

	for (j = 0; !rc && j < itop; j++)
	{   // begin j-loop
		
		rc = PersistPath( pRegion->Path(j));
	}   //   end j-loop

	RETURN_RC(CBPolyBuilder::PersistRegion, rc);
}

// Persist a boundary polyline
RC CBPolyBuilder::PersistPath(PPath pPath) // In:  The boundary to persist
{
	RC rc = SUCCESS;
	
	CGeoToDWG convertGEOToDWG( SDS_CURDWG, m_ptNormal);
		
	if (m_bLightWeight)
	{  // begin building a lightweight polyline
		
		rc = convertGEOToDWG.LWPolylineFromPath(pPath,
			m_pHanditemCollection);
		
	}  //   end building a lightweight polyline
	else
	{   // begin building a heavyweight polyline
		
		
		rc =convertGEOToDWG.PolylineFromPath(pPath,
			m_pHanditemCollection);
		
	}   //   end building a heavyweight polyline
	
	RETURN_RC(CBPolyOutput::PersistPath, rc);
}

bool CBPolyBuilder::FilterList(db_drawing *drawing, HanditemCollection &coll)
{
	CBoundaryBuilder::FilterList(drawing, coll);

	//filter additional entities
	coll.FilterEntity(DB_ATTRIB); // comment this out for test purposes only 
	coll.FilterEntity(DB_DIMENSION); // comment this out for test purposes only
	coll.FilterEntity(DB_LEADER);
	coll.FilterEntity(DB_MTEXT);// comment this out for test purposes only
	coll.FilterEntity(DB_TEXT); // comment this out for test purposes only
	coll.FilterEntity(DB_TOLERANCE); // comment this out for test purposes only

	return true;  // just return true because we can still do the best we can with what is in the list.
}


