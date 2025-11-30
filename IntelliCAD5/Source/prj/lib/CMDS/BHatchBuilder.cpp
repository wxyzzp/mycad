/* G:\ICADDEV\PRJ\LIB\CMDS\BHATCHBUILDER.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Implementation of the CBHatchBuilder class
 * 
 */ 

#include "cmds.h"/*DNT*/
#include "Icad.h"/*DNT*/
#include "IcadApi.h"/*DNT*/
#include "Geometry.h"/*DNT*/
#include "Extents.h"/*DNT*/
#include "Path.h"/*DNT*/
#include "Region.h"/*DNT*/
#include "Boolean.h"/*DNT*/

#include "ListColl.h"/*DNT*/
#include "GeoConvert.h"/*DNT*/

#include "BHatchBuilder.h"
#include "BHatchFactory.h"

#include "DbToHatchlink.h"



// Hatch factory (for use in DB)
CBHatchBuilder *
BHatchBuilderFactory::CreateHatchBuilder(sds_point extrusion)
	{ 
	long result;
	C3Point extPt(extrusion[0], extrusion[1], extrusion[2]);
	//Bugzilla No. 78414 ; 01-30-2003 
#ifdef VARYING_HATCH_TOLERANCE
	return new CBHatchBuilder(extPt, result);
#else
	return new CBHatchBuilder(HATCH_TOLERANCE, extPt, result);
#endif

	}

// CBHatchBuilder
static bool IsMostRecentlySelected = false;
static int i_region;
static int j_path;

// member functions for helper class
RegionPathPoint::RegionPathPoint(int iRegion, int jPath, sds_point ClickPoint)
	{
	m_iRegion = iRegion;
	m_jPath = jPath;
	for (int i = 0; i < 3; i++)
		m_ClickPoint[i] = ClickPoint[i];
	}


RegionPathPointArray::RegionPathPointArray()
	{
	m_Size = 0;
	m_Item = NULL;
	}

RegionPathPointArray::~RegionPathPointArray()
	{
	if(m_Item)
		delete [] m_Item;
	}

void RegionPathPointArray::AddItem(int iRegion, int jPath, sds_point ClickPoint)
	{
	// if we can find the iRegion, jPath  there already, just change ClickPoint
	int found = false;
	int k,m,n;
	int kfound;
	for (k = 0; k < m_Size; k++)
		{   // begin k-loop
		if ((m_Item[k].m_iRegion == iRegion) && (m_Item[k].m_jPath == jPath))
			{
			found = true;
			kfound = k;
			break;
			}
		}   //   end k-loop
	if (found)
		{ // replace the data in the existing item
		for (n = 0; n < 3; n++)
			m_Item[k].m_ClickPoint[n] = ClickPoint[n];
		}
	else
		{  // begin extending the array
		int oldSize = m_Size;
		int newSize = oldSize + 1;

		// allocate space for one additional item
		RegionPathPoint * newItem = new RegionPathPoint[newSize];

		// copy the previous items into the new space
		for (m = 0; m < oldSize; m++)
			{
			newItem[m].m_iRegion = m_Item[m].m_iRegion;
			newItem[m].m_jPath = m_Item[m].m_jPath;
			for (n = 0; n < 3; n++)
				newItem[m].m_ClickPoint[n] = m_Item[m].m_ClickPoint[n];
			}

		// add the values for the new item at the end
		newItem[oldSize].m_iRegion = iRegion;
		newItem[oldSize].m_jPath = jPath;
		for (n = 0; n < 3; n++)
			newItem[oldSize].m_ClickPoint[n] = ClickPoint[n];

		// delete the old items
		delete [] m_Item;

        // assign m_Item to newItem
		m_Item = newItem;

		// increment m_Size
		++m_Size;

		}  //  end extending the array
	}

bool RegionPathPointArray::GetItem(int iRegion, int jPath, sds_point ClickPoint)
	{
	bool found = false;
	int k;
	int n;
	for (k = 0; k < m_Size; k++)
		{   // begin k-loop
		if ( (m_Item[k].m_iRegion==iRegion) && (m_Item[k].m_jPath==jPath) )
			{
			for (n = 0; n < 3; n++)
				ClickPoint[n] = m_Item[k].m_ClickPoint[n];
			found = true;
			}
		}   //   end k-loop
	return found;
	}


// Constructor 
//Bugzilla No. 78414 ; 01-30-2003 
#ifdef VARYING_HATCH_TOLERANCE
CBHatchBuilder::CBHatchBuilder(
							 C3Point ptNormal,	 // In:	 Extrusion vector for ECS
							 RC & rc)           // Out: Return code
							 : CBoundaryBuilder(),
							 m_ptNormal(ptNormal),
							 m_RegionPathPointArray()
#else
CBHatchBuilder::CBHatchBuilder(double rTolerance, // In:  Geometric tolerance
							 C3Point ptNormal,	 // In:	 Extrusion vector for ECS
							 RC & rc)           // Out: Return code
							 : CBoundaryBuilder(rTolerance),
							 m_ptNormal(ptNormal),
							 m_RegionPathPointArray()
#endif //VARYING_HATCH_TOLERANCE

	{
	m_iCurrPath = -1; // invalidate m_iCurrPath
    rc = SUCCESS;
	m_updateSeeds = false;
	m_storeSeeds = false;
	}

// Destructor
CBHatchBuilder::~CBHatchBuilder()
		{

		// We only want the hatch to be deleted if it has not been added to the drawing.  The handitem
		// collection takes care of this decision for us.
		if (m_pHanditemCollection)
			delete m_pHanditemCollection;
		}

// Methods

// basic getters and setters

bool CBHatchBuilder::get_hatch(db_hatch ** pHatch)
	{
	*pHatch = m_pBhatch;
	return true;
	}

// bool indicates whether the handitem collection should take care of deleting the hatch
// (note: this can be reset by calling manipulating the hand item collection.
bool CBHatchBuilder::set_hatch(db_hatch * pHatch, bool deleteHatch)
	{
	// if a hatch has already been added then we need to clear the old hatch
	if (m_pHanditemCollection->Count() != 0)
		m_pHanditemCollection->ClearList();

	m_pBhatch = pHatch;

	m_pHanditemCollection->AddItem(SDS_CURDWG, m_pBhatch, deleteHatch);

	return true;
	}




bool CBHatchBuilder::FilterList(db_drawing *drawing, HanditemCollection &coll)
{
	CBoundaryBuilder::FilterList(drawing, coll);

	//filter additional entities

	// we can filter out the leader, because the text associated with the leader
	// is a separate entity.
	coll.FilterEntity(DB_LEADER);

	return true;  // just return true because we can still do the best we can with what is in the list.
}

// this function will return true if there is only one item in pAssociates
// and that item is text-related
static bool HasTextBox(stdObjHandleList * pAssociates)
	{
	bool isTextRelated = false;

	if (pAssociates)
		{
		if (pAssociates->size()==1)
			{
			db_objhandle Handle = *(pAssociates->begin());
			db_handitem * hip = SDS_CURDWG->handent(Handle);
			//Modified by SMR for preventing bhatch crash. 2001/5/19
			// This code is temporarly. Actually, hip can not be empty.
			if (hip)
				{
				int type = hip->ret_type();

				if ((type == DB_TEXT) || 
					(type == DB_MTEXT) || 
					(type == DB_DIMENSION) ||
					(type == DB_TOLERANCE) ||
					(type == DB_ATTRIB))
					isTextRelated = true;
				}
			//End of modification. SMR 2001/5/19
			}
		}


	return isTextRelated;
	}

// Persist a path that is involved in a BHATCH
RC CBHatchBuilder::PersistPath(PPath pPath) // In:  The path to persist
	{
	RC rc = SUCCESS;
	
	ASSERT(m_iCurrPath >= 0);
	
	CGeoToDWG convert( SDS_CURDWG, m_ptNormal);

	stdObjHandleList Associates;
	stdObjHandleList * pAssociates;

	bool found;
	this->m_pCandidates->m_ObjHandles.get_HandleList(i_region, j_path, found, Associates);

    if (found)
		pAssociates = &Associates;
	else
		pAssociates = NULL;

	int pathFlag;

	// we will initialize the pathFlag here, and more information will be added as
	// appropriate inside BhatchFromPath (e.g., if the path is a polyline we will add 2 there)

	pathFlag  = IC_HATCH_PATHFLAG_DEFAULT; // initialize to the default
	pathFlag += IC_HATCH_PATHFLAG_DERIVED; // currently all IntelliCAD BHATCHes are derived

	if(j_path == 0)
		{
        if(m_pCandidates->Item(i_region)->GetContainer(m_pCandidates) == -1)
			pathFlag += IC_HATCH_PATHFLAG_EXTERNAL; 
		}
	else	/*D.G.*/// Set outermost style for all holes if the approriate hatch style is set.
			// The old code commented out just below.
		if(j_path == 1 || m_pCandidates->GetIslandOption() == Outer_Only)
			pathFlag += IC_HATCH_PATHFLAG_OUTERMOST;

/* old code
	// the following is not generally correct; e.g. if a region has several disjoint islands
	// we will remedy this later

	if (j_path == 1)
		pathFlag += IC_HATCH_PATHFLAG_OUTERMOST;
*/
	if (HasTextBox(pAssociates))
		pathFlag += IC_HATCH_PATHFLAG_TEXTBOX;

	
    rc = convert.BhatchFromPath(pPath, m_iCurrPath, pathFlag, pAssociates, m_pBhatch);

	if (!rc && m_updateSeeds && IsMostRecentlySelected)
		m_RegionPathPointArray.AddItem(i_region, j_path, m_ClickPoint);
	
	if (!rc && m_storeSeeds)
		{
		bool found;
		found = m_RegionPathPointArray.GetItem(i_region, j_path, m_ClickPoint);
        //EBATECH(CNBR) 2004/1/8 Today's BHATCH /SELECT don't store seek points
        // in m_RegionPathPointArray so that GetItem() always rise next ASSERT..
		//ASSERT(found==true);
		if (found)
			{
			m_pBhatch->set_seed(m_ClickPoint, m_iCurrPath);
			}
		}
	
	RETURN_RC(CBPolyOutput::PersistPath, rc);
	}

// Persist a region that is involved in a BHATCH
// Note:  The parameter list and implementation may need to change, depending on 
// what we find out during the course of development of this code.
RC CBHatchBuilder::PersistRegion(int i,          // In:  The index of the region to persist
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
		j_path = j;
		rc = PersistPath( pRegion->Path(j));
		++m_iCurrPath;
	}   //   end j-loop

	RETURN_RC(CBHatchBuilder::PersistRegion, rc);
}

RC CBHatchBuilder::Persist()
	{
	RC rc = SUCCESS;

	ASSERT(m_pBhatch != NULL);

	// If the number of paths has already been set, then we must be doing a modify instead of
	// a create.  In this case we want the boundary objects to stay the same.  Calling set_91 on 
	// a hatch reallocates all the path information so we want to check if we are doing a modify 
	// first.  The path changes will be accounted for in set_path which is called on each path 
	// later.
	int numPrevPaths;
	m_pBhatch->get_91(&numPrevPaths);

	if (numPrevPaths != m_pCandidates->NumHatchPaths())
		m_pBhatch->set_91(m_pCandidates->NumHatchPaths());

	sds_point UcsZ;
	SdsGetUcsZ(UcsZ);
	m_pBhatch->set_210(UcsZ);

	struct resbuf rb;
	sds_point elevpt;

	if (SDS_getvar(NULL, DB_QELEVATION, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES)==RTNORM)
		{
		elevpt[0]=elevpt[1]=0.0;
		elevpt[2]=rb.resval.rreal;
		m_pBhatch->set_10(elevpt);
		}

    // For BHATCH, the OuterOnly parameter is false, irrespective
	// of m_pCandidates->GetIslandOption().
	// This is different than CBPolyBuilder::Persist

	// changed implementation, set OuterOnly in same way it is set in
	// CBPOlyPBuilder::Persist

	bool OuterOnly;
	if (m_pCandidates->GetIslandOption() == Ignore_Islands)
		OuterOnly = true;
	else
		OuterOnly = false;

	m_iCurrPath = 0;
	for (int i = 0; !rc && i < m_pCandidates->Size(); i++)
		{
		i_region = i;
		IsMostRecentlySelected = m_pCandidates->IsMostRecentlySelected(i);
		if (m_pCandidates->IsFinalist(i)) // was IsHatchFinalist
			{
			rc = PersistRegion(i, OuterOnly);
			}
		}

	RETURN_RC(CBHatchBuilder::Persist, rc);
	}

// With hatch we need to persist the entire hatch and not just the most recently selected.
RC CBHatchBuilder::PersistMostRecentlySelected(bool & AreFinalists)
	{
	
	AreFinalists = false;
	RC rc = Persist();
	
	
	if (!rc)
		{
		bool decided = false;
		for (int i = 0; i < m_pCandidates->Size(); i++)
			{
			if (m_pCandidates->IsMostRecentlySelected(i))
				{
				if (!decided)
					{
					AreFinalists = m_pCandidates->IsFinalist(i);
					decided = true;
					}
				}
			}
		}
	
	
	
	RETURN_RC(CBHatchBuilder::Persist, rc);
	}



// Determine whether this is the simplest case of an associative hatch,
// namely one where each path has only one source boundary object 
// associated with that path.  Also, given a handle to a source boundary
// object, determine the index of the unique path associated with it.
bool CBHatchBuilder::SimplestCase(
	db_objhandle * pObjHandle, // I: - handle to a source boundary object
	int & iPath) // O: - the index of the path associated with pObjHandle
	{
	bool isSimplestCase = true; // innocent until proven guilty
	iPath = -1; // but set this to an illegal value

	int nPaths;
	m_pBhatch->get_91(&nPaths);

	int flags;
	int nedges;
	int numBoundaryObjs;

	int i;

	for (i = 0; i < nPaths; i++)
		{  // begin i-loop
		m_pBhatch->get_path(i, &flags, &nedges, &numBoundaryObjs);

		if ( (numBoundaryObjs != 1) )
			{
			isSimplestCase = false;
			break;
			}
		}  //   end i-loop

	if (isSimplestCase)
		{  // begin looking for iPath
		db_hatchboundobjslink * boundaries;
		db_handitem * entity = NULL;
		for (i = 0; i < nPaths; i++)
			{  // begin i-loop
			// if we have gotten into this loop, we know that for each path
			// there is exactly one boundary object.  If that boundary object
			// agrees with the input pObjHandle, set iPath to i.

			m_pBhatch->get_pathboundobj(i, &boundaries);
			if (boundaries)
				{
				db_hatchboundobjs * pos = NULL;
				boundaries->getNext(&pos, &entity);

				if (entity)
					{
					db_objhandle hanp = entity->RetHandle();

					if (hanp == *pObjHandle)
						{
						iPath = i;
						break;
						}
					}
				}
			}  //   end i-loop
		}  //   end looking for iPath

	// if we failed to find the corresponding boundary object
	// we must return false.
	if (iPath < 0)
		isSimplestCase = false;


	return isSimplestCase;
	}

// The following is a special-purpose function that is to be used by ModifySimplestHatch
// in the event that get_pathedgepline returns an object that is not of type polyline

static RC PolylineToHatchlink
(db_hatch * pBhatch,             // IO:  pointer to the db_hatch object to be modified
 int iPath,                      // I:  the index of the path
 db_handitem * hip,              // I:  hip to be converted
 DbToHatchlink & modifier)       // I:  converter object
	{
	RC stat = FAILURE;
    db_hatchplinelink * pPolyline = NULL;
	
	int pathFlag;
	int nEdges;
	int numBoundaryObjs;
	
	// we need to get the old path information to get the pathFlag
	// so that we can update it and to verify that numBoundaryObjs is 1.
	pBhatch->get_path(iPath, &pathFlag, &nEdges, &numBoundaryObjs);
	
	// we are not going to proceed unless there is exactly one boundary object
	if (numBoundaryObjs != 1)
		return FAILURE;
	
	pathFlag |= 2; // this signifies that the path is a polyline
	nEdges = 1; // we are going to make this a polyline
	pBhatch->set_path(iPath, pathFlag, nEdges);
	
	int closed = 1;
	int hasbulges;
	int nverts;
	
	db_polyline * pline = NULL;
	db_lwpolyline * lwpline = NULL;
	
	db_hatchboundobjslink * objsList = new db_hatchboundobjslink(SDS_CURDWG);
	if (objsList)
		objsList->addBoundaryObj(hip);
	else
		return FAILURE;
	
	switch (hip->ret_type())
		{
		case DB_POLYLINE:
			pline = (db_polyline *) hip;
			pline->count_ordinary_vertices(&nverts);
			hasbulges = 1;
			pBhatch->set_pathedgepline(iPath, closed, hasbulges, nverts);
			pBhatch->get_pathedgepline(iPath, pPolyline);
			if (pPolyline != NULL)
				stat = modifier.PolylineToHatchlink(pline, pPolyline);
			else stat = FAILURE;
			break;
		case DB_LWPOLYLINE:
			lwpline = (db_lwpolyline *) hip;
			lwpline->get_90(&nverts);
			hasbulges = lwpline->hasbulges();
			pBhatch->set_pathedgepline(iPath, closed, hasbulges, nverts);
			pBhatch->get_pathedgepline(iPath, pPolyline);
			if (pPolyline != NULL)
				stat = modifier.LWPolylineToHatchlink(lwpline, pPolyline);
			else stat = FAILURE;
			break;
		default:
			stat = FAILURE;
			break;
		}
	
	if (stat==SUCCESS)
		pBhatch->set_pathBoundaryAndAssociate(iPath, numBoundaryObjs, objsList, FALSE);
	
	return stat;
	}

// The following is a special-purpose function that is to be used by ModifySimplestHatch
// as a backup in the case of DB_CIRCLE if get_pathedgepline fails.
static RC CircleToHatchlink
(db_hatch * pBhatch,             // IO:  pointer to the db_hatch object to be modified
 int iPath,                      // I:  the index of the path
 db_handitem * hip)              // I:  hip to be converted
	{
	RC stat = FAILURE;

	sds_real centx;
	sds_real centy;
	sds_real rad;
	sds_real sang;
	sds_real eang;
	int ccw;

	db_circle * pCircle = NULL;


	switch (hip->ret_type())
		{
		case DB_CIRCLE:
			// first lets do some simple validity checking
			rad = -1.0;
			// unfortunately there is no overloaded version of get_pathedgearc that
			// gets it in the aggregate.
			pBhatch->get_pathedgearc(iPath, 0, &centx, &centy, &rad, &sang, &eang, &ccw);
			if (rad <= 0.0)
				{
				stat = FAILURE;
				}
				else
				{
				pCircle = (db_circle *) hip;
				sds_point cent;
				pCircle->get_10(cent);
				centx = cent[0];
				centy = cent[1];
				pCircle->get_40(&rad);
				sang = 0.0;
				eang = TWOPI;
				ccw = 1;
				pBhatch->set_pathedgearc(iPath, 0, centx, centy, rad, sang, eang, ccw);
				stat = SUCCESS;
				}
			break;
		default:
			stat = FAILURE;
			break;
		}


	return stat;
	}

RC CBHatchBuilder::ModifySimplestHatch(db_objhandle * pObjHandle,
									   int iPath)
	{
	ASSERT(iPath >= 0);
	RC rc = FAILURE;

	db_handitem * hip = SDS_CURDWG->handent(*pObjHandle);

	if (hip)
		{   // begin case of non-NULL hip
		db_hatchplinelink * pPolyline = NULL;
		db_hatchsplinelink * pSpline = NULL;
		db_hatchellarclink * pEllarc = NULL;
		db_hatcharclink * pArc = NULL;
		DbToHatchlink modifier;
		int jEdge = 0;
		switch (hip->ret_type())
			{  // begin switch on type
			case DB_LINE:
				rc = FAILURE; // this case should not occur because one line is not closed
				break;

			case DB_ARC:
				rc = FAILURE; // this case should not occur because one arc is not closed
				break;
				
			case DB_CIRCLE:
				// first see whether it is associated with a polyline 
				m_pBhatch->get_pathedgepline(iPath, pPolyline);
				if (pPolyline)
					{
					rc = modifier.CircleToHatchlink((db_circle *)hip, pPolyline);
					}
				else
					{  // try backup plan
					rc = CircleToHatchlink(m_pBhatch, iPath, hip);
					}

				
				break;

			case DB_ELLIPSE:
				m_pBhatch->get_pathedgeellarc(iPath, jEdge, pEllarc);
				if (pEllarc)
					{
					rc = modifier.EllarcToHatchlink((db_ellipse *)hip, pEllarc);
					}
				else
					rc = FAILURE;
				break;

			case DB_SPLINE:
				m_pBhatch->get_pathedgespline(iPath, jEdge, pSpline);
				if (pSpline)
					{
					rc = modifier.SplineToHatchlink((db_spline *)hip, pSpline);
					}
				else
					rc = FAILURE;
				break;

			case DB_POLYLINE:
				m_pBhatch->get_pathedgepline(iPath,pPolyline);
				if (pPolyline)
					{   // begin case of pPolyline
					rc = modifier.PolylineToHatchlink((db_polyline *)hip, pPolyline);
					}   //  end case of pPolyline
				else
					{  // try backup plan
					rc = PolylineToHatchlink(m_pBhatch, iPath, hip, modifier);
					}
				break;

			case DB_LWPOLYLINE:
				m_pBhatch->get_pathedgepline(iPath,pPolyline);
				if (pPolyline)
					{  // begin case of pPolyline
					rc = modifier.LWPolylineToHatchlink((db_lwpolyline *)hip, pPolyline);
					}  //  end case of pPolyline
				else
					{  // try backup plan
					rc = PolylineToHatchlink(m_pBhatch, iPath, hip, modifier);
					}					
				break;

			default:
				rc = FAILURE;
				break;

			}  //   end switch on type
		}   //   end case of non-NULL hip
	else
		{   // begin case of NULL hip
		rc = FAILURE;
		}   //   end case of NULL hip


	return rc;
	}


// modify an associative hatch
RC CBHatchBuilder::ModifyHatch(db_objhandle * pObjHandle)
	{
	RC rc;

	int oldNumPaths;
	int * oldNumBoundObjsInPath = NULL;
	int newNumPaths;
	int * newNumBoundObjsInPath = NULL;

	AnalyzeHatch(oldNumPaths, oldNumBoundObjsInPath);

    // let's first check the simple case 
	int iPath;

	// The function ModifyGeneralHatch will handle cases not handled otherwise.

	if (SimplestCase(pObjHandle, iPath))
		{
		rc = ModifySimplestHatch(pObjHandle, iPath);
		if (rc != SUCCESS)
			{
			rc = ModifyGeneralHatch(pObjHandle);
			}
		}
	else
		{
		rc = ModifyGeneralHatch(pObjHandle);
		}

	int i;
	if (rc == SUCCESS)
		{
		AnalyzeHatch(newNumPaths, newNumBoundObjsInPath);
		if (oldNumPaths != newNumPaths)
			rc = FAILURE;
		else
			{
			for (i = 0; i < oldNumPaths; i++)
				{
				if ( oldNumBoundObjsInPath[i] != newNumBoundObjsInPath[i] )
					{
					rc = FAILURE;
					break;
					}
				}
			}
		}


	delete [] oldNumBoundObjsInPath;
	delete [] newNumBoundObjsInPath;

    return (rc);

	}

// general modify - it essentially recreates the hatch but it is easier to work inside the
// hatch since db does not have access to a hand item collection.
RC CBHatchBuilder::ModifyGeneralHatch(db_objhandle *pHanditem)
{	


	// Set candidate list not to toggle for modify
	m_pCandidates->SetToggleHatch(false);

	int style;
	m_pBhatch->get_75(&style);

	// Set island detection
	switch (style)
	{
	case 0:
		SetIslandDetection(Nested_Islands);
		break;

	case 1:
		SetIslandDetection(Outer_Only);
		break;
							
	case 2:
		SetIslandDetection(Ignore_Islands);
		break;

	default:
		ASSERT(FALSE);
	}

	HanditemCollection collection;

	int numpaths;
	m_pBhatch->get_91(&numpaths);

	db_hatchboundobjslink *boundaries;
	db_handitem *entity = NULL;
	for (int i = 0; i < numpaths; i++)
	{
		m_pBhatch->get_pathboundobj(i, &boundaries);

		if (!boundaries)
			return false;

		db_hatchboundobjs *pos = NULL;

		while (boundaries->getNext(&pos, &entity))
		{
			collection.AddItemCheckDups(SDS_CURDWG, entity, false);
		}
	}

	AcceptHanditemColl(SDS_CURDWG, collection, m_ptNormal);
	
	int nseeds;
	m_pBhatch->get_98(&nseeds);
	sds_point seed;
	RC rc;
	
	for (i = 0; i < nseeds; i++)
		{   
		m_pBhatch->get_seed(seed, i);
		rc = ClickPoint(seed[0], seed[1]);
		
		if (rc != SUCCESS)
			{
			// don't give up yet!!!
			// instead of using seed, let's try the centers of the min-max boxes of the
			// items in the handitem collection in the hopes that one of them will work
			// this method is quite crude and could be refined.
			bool done = false;
			db_handitem * prev = NULL;
			db_handitem * curr = NULL;
			while (!done)
			{  // begin while (!done)
				curr = collection.GetNext(prev);
				if (curr != NULL)
				{  // begin case of non-NULL curr
					rc = ClickPoint(curr);
					if (rc == SUCCESS)
						done = true;
				}  //   end case of non-NULL curr
				else
				{  // begin case of NULL curr
					done = true;
				}  //   end case of NULL curr
				prev = curr;
			}  //   end while (!done)
			}

		if (rc != SUCCESS)
			break;
		}

	if (rc == SUCCESS)
		rc = Persist();

	return rc;
}

// Note:  It is the responsibility of the calling routine to delete [] numBoundObjsInPath
void CBHatchBuilder::AnalyzeHatch(int & numPaths, // the number of paths in this hatch
							      int * & numBoundaryObjsInPath) // numBoundObjsInPath[i]
                                                         // is the number of boundary
														 // objects associated with the
														 // i-th path.

	{
	int i;
	int flags;
	int nedges;
	int numBoundaryObjs;

	m_pBhatch->get_91(&numPaths);
	if (numPaths > 0)
		{
		numBoundaryObjsInPath = new int[numPaths];
		for (i = 0; i < numPaths; i++)
			{
			m_pBhatch->get_path(i, &flags, &nedges, &numBoundaryObjs);
			numBoundaryObjsInPath[i] = numBoundaryObjs;
			}

		}
	else
		{
		numBoundaryObjsInPath = NULL;
		}
	}




