/* C:\ICADDEV\PRJ\LIB\CMDS\BOUNDARYBUILDER.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
   Implementation of the CBoundaryBuilder class
 * 
 * <TODO : Add Abstract here>
 * 
 */ 
//Precompilation
#include "cmds.h"/*DNT*/
#include "Icad.h"/*DNT*/

#include "Geometry.h"
#include "GeomPtrs.h" // try this (rdf 1/29/98)
#include "Extents.h"
#include "Path.h"
#include "Region.h"
#include "Boolean.h"
#include "TheUser.h"
#include "Candidate.h"
#include "ListColl.h"
#include "BoundaryBuilder.h"
#include "GeoConvert.h"

#include "IcadView.h"
#include "DoBase.h"

//Bugzilla No. 78414 ; 01-30-2003 
#ifdef VARYING_HATCH_TOLERANCE
#include "IcadApi.h"/*DNT*/
#endif

//Bugzilla No. 78414 ; 01-30-2003 
#ifndef VARYING_HATCH_TOLERANCE
CBoundaryBuilder::CBoundaryBuilder()
	{
	m_pCandidates = NULL;
	m_pFragment = NULL;
	m_bClick = false; // will be set to true after the first click
	m_FragmentStatus = fragment_not_yet_tried;
	m_pHanditemCollection = new HanditemCollection();
	}
#endif //VARYING_HATCH_TOLERANCE

//Bugzilla No. 78414 ; 01-30-2003 
#ifdef VARYING_HATCH_TOLERANCE
CBoundaryBuilder::CBoundaryBuilder( )	
	{
	m_pCandidates = new CandidateList(this);
	struct resbuf rb;
	SDS_getvar(NULL,DB_QVIEWSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	m_pFragment = new CFragment((rb.resval.rreal * HATCH_TOLERANCE_COEFFICIENT ), m_pCandidates);
	m_bClick = false; // will be set to true after the first click
	m_FragmentStatus = fragment_not_yet_tried;
	m_pHanditemCollection = new HanditemCollection();
	}
#else
CBoundaryBuilder::CBoundaryBuilder(
	double rTolerance)// In:  Geometric tolerance
	{
	m_pCandidates = new CandidateList(this);
	m_pFragment = new CFragment(rTolerance, m_pCandidates);
	m_bClick = false; // will be set to true after the first click
	m_FragmentStatus = fragment_not_yet_tried;
	m_pHanditemCollection = new HanditemCollection();
	}
#endif //VARYING_HATCH_TOLERANCE

CBoundaryBuilder::~CBoundaryBuilder()
	{
	delete m_pFragment;
	delete m_pCandidates;
	}

// Enable the toggle operation to process or not process islands
// consistent with the current island detection option selected.
void CBoundaryBuilder::SetIslandDetection(IslandOption option)
{
    // First, in all cases we will set the boundary detection
	// option to the input option value

	m_pCandidates->SetIslandOption(option);

	switch (option)
	{   // begin switch(option)

	case Ignore_Islands:
		m_pCandidates->SetDepth(0);
		break;

	case Outer_Only:
		m_pCandidates->SetDepth(0); // still, no islands
		break;

	case Nested_Islands:
		m_pCandidates->SetDepth(MAX_ISLAND_DEPTH);
		break;

	default:
		m_pCandidates->SetDepth(0);
		break;
	}   //   end switch(option)

}

// Construct one boundary polyline from a clicked point
RC CBoundaryBuilder::ClickPoint(
								double x,  // Input:  The x coordinate of the clicked point
								double y)  // Input:  The y coordinate of the clicked point
{
	C3Point P(x,y);

	m_ClickPoint[0] = x;
	m_ClickPoint[1] = y;
	m_ClickPoint[2] = 0.0;

	RC rc = SUCCESS;
	
	if (m_FragmentStatus == fragment_failed)
		rc = FAILURE;
	else
	{
		if (( m_bClick==false ) && (m_FragmentStatus == fragment_not_yet_tried))
		{   // begin case where this is the user's first click
			
			// this is the first click, so the regions need to be computed
			rc = m_pFragment->Operate();
			
			if (rc == SUCCESS)
				rc = m_pFragment->WrapUp();
			
			// now that the candidate list is populated
			if (rc == SUCCESS)
			{
				rc = m_pCandidates->GetIslands();
				m_bClick = true;
			}
			
			if (rc == SUCCESS)
			{
				m_FragmentStatus = fragment_succeeded;
			}
			else
			{
				m_FragmentStatus = fragment_failed;
			}
			
		}   //   end case where this is the user's first click
		
		if (rc == SUCCESS)
		{
			m_pCandidates->ClearMostRecentlySelected();
			rc = m_pCandidates->ClickPoint(P);
		}
	}
	
	RETURN_RC(CBoundaryBuilder::ClickPoint, rc);
}

// Construct one boundary polyline from a center of entity's bounding box
RC CBoundaryBuilder::ClickPoint(db_handitem* pEntity) // Input: Pointer to entity
{
	CD3 minPoint, maxPoint;
	double xc, yc;
	
#ifdef _USE_DISP_OBJ_PTS_3D_
//#pragma message(__FILELINE__"_USE_DISP_OBJ_PTS_3D_ defined")
	CDoBase::getPlanarBoundingBox(SDS_CURGRVW, static_cast<db_entity*>(pEntity), minPoint, maxPoint);
#else
	pEntity->get_extent(minPoint, maxPoint);
#endif
	xc = 0.5 * (minPoint[0] + maxPoint[0]);
	yc = 0.5 * (minPoint[1] + maxPoint[1]);
	
	return ClickPoint(xc, yc);
}

bool CBoundaryBuilder::FilterList(db_drawing *drawing, HanditemCollection &coll)
{
	//First expand the blocks into subentities 
	coll.ExpandBlocks(drawing);

	//Remove entities that are ignored for both bpoly and bhatch.
	coll.FilterEntity(DB_3DSOLID);
	coll.FilterEntity(DB_REGION);
	coll.FilterEntity(DB_BODY);
	coll.FilterEntity(DB_ATTDEF);
	coll.FilterEntity(DB_HATCH);
	coll.FilterEntity(DB_OLE2FRAME);
	coll.FilterEntity(DB_POINT);
	coll.FilterEntity(DB_SHAPE);
	coll.FilterEntity(DB_SOLID);
	coll.FilterEntity(DB_TRACE);
	coll.FilterEntity(DB_VIEWPORT);

	return true;  // just return true because we can still do the best we can with what is in the list.
}

bool CBoundaryBuilder::AcceptHanditemColl(db_drawing *drawing, HanditemCollection &coll, C3Point ptNormal)
{
// Also need to uncomment this line because Accept is causing problems (should be removed).
	CDWGToGeo convert;
	RC result;

	// Filter out any entities which are not considered
	FilterList(drawing, coll);

	// With the UCS normal vector, filter out entities whose extrusion vectors(DXF group code 210) 
	// are not in the same direction as the UCS normal vector.
	FilterListHelper(ptNormal, coll);

	//iterate through list to accept each curve.
	db_handitem *currentItem;
	db_handitem *insert = NULL;
	coll.GetNext(NULL, &currentItem, &insert);
	
	stdPPathList boundaryList;

	while (currentItem)
	{
		// input values are copied from the drawing so polyline next pointers should already
		// point to vertex and seq end. So skip vertex & seq end entities.
		int type = currentItem->ret_type();

		if ((type != DB_VERTEX) && (type != DB_SEQEND))
			{
			boundaryList.clear();
			
			result = convert.ExtractEntity(currentItem, boundaryList);

			db_objhandle * handleptr;
			
			// set the handle to the insert if the entity is in a block
			if (insert)
				handleptr = insert->RetHandlePtr();
			else
				handleptr = currentItem->RetHandlePtr();
			
			if (result == SUCCESS)
				{
				for (stdPPathList::const_iterator iterPath = boundaryList.begin(); iterPath != boundaryList.end();	iterPath++)
					Accept(*iterPath, handleptr);  
				}
			
			}

		coll.GetNext(currentItem, &currentItem, &insert);
	}

	return true;  // just return true because we can still do the best we can with what is in the list.
}


// This function adds the curves in the current path to the CFragment member object
// of the CBoundaryBuilder class.  The idea is that Accept should be called for each
// path that is intended to be part of the set that CFragment will operate on
RC CBoundaryBuilder::Accept(PPath pPath,   // Input/Output:  A path extracted from an entity
							                   // Its contents are deleted after handing the curves
											   // over to m_pFragment via AcceptCurve.
							GeoID ID)        // Input:  Entity ID
{
	int i; // loop index
    RC rc = SUCCESS; // innocent until proven guilty
	
    // For fragment it doesn't matter what we put for closed or fillable
	// so we will just set them both to false.
	
	bool bClosed = false;
	bool bFillable = false;

	int nCurves = pPath->Size();
	
	rc = m_pFragment->NewPathIn(bClosed, bFillable, ID);
	
	for (i = 0; (i < nCurves) && (rc == SUCCESS); i++)
	{   // begin i-loop
		rc = m_pFragment->AcceptCurve(pPath->Curve(i), ID);
	}   //   end i-loop
	
	if (rc == SUCCESS)
		rc = m_pFragment->EndPathIn();

	// Remove the contents of the path (we are not purging the curves themselves here!)
	pPath->RemoveAll();

	// Now delete the path
	delete pPath;
	
	RETURN_RC(CBoundaryBuilder::Accept, rc);
}


bool 
CBoundaryBuilder::FilterListHelper(C3Point ptNormal, HanditemCollection &coll)
	{
	return coll.FilterEntity(ptNormal);
	}

