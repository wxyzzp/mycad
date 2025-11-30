/* C:\ICADDEV\PRJ\LIB\CMDS\BOUNDARYBUILDER.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract

  CBoundaryBuilder is the base class for the classes that construct
  BPOLY and BHATCH entities from DWG curves and polylines.  To carry
  out such a construction:
	* Instantiate an object of the appropriate subclass
	* Call its ExtractEntity method with every one of its input entities
	* Call its ClickPoint method with every clicked point

  The instantiated CBoundaryBuilder object will call its PersistRegion method
  to persist the regions that it has constructed.  PersistRegion is a pure method,
  to be implemented by each derived class, depending upon how it chooses to
  persist the regions (e.g., as a BHATCH entity, as polylines, etc).

  To BE DONE:  (Not reflected in the current code)
  -------------------------------------------------
  When constructing the Builder, the caller should give the Builder a pointer
  to itself as some kind of client object.  This Client should support methods
  to do the following:

  * Produce a new db_handitem of a given type upon request
  * Accept the finished item when the Builder is done.

  The builder will populate the structure, and call the Client's accept method when 
  it is done.

  THIS ASPECT IS NOT CAPTURED IN THE CURRENT CODE

  Internally, the Builder uses the Fragment operation to construct a candidate list
  and then promotes them to finalists by hit testing with clicked points. We will
  now have member functions that determine whether a candidate is a finalist rather
  than maintaining a separate list.

  When its ExtractEntity method is invoked with a DWG entity, it constructs a GEO
  path from the entity and calls the NewPath method of the CFragment member data.

  The first time that the Builder's ClickPoint method is called , it invokes the
  CFragment member data's Operate method, which populates the CandidateList.
  
  Moreover, any ClickPoint call promotes the selection of one of the candidates
  to finalist.  Note:  the client needs to be told about it as well.

  When the Client is tired of clicking points, it should call the Persist method,
  asking for the set of finalists.

  The Builder then traverses the list of finalists, invoking the PersistRegion function
  for each item in the final list to create the appropriate DWG entity and hand it over
  to the Client.  The PersistRegion method is pure virtual, to be concretely implemented
  by its derived classes.

*/

#ifndef BOUNDARYBUILDER_H
#define BOUNDARYBUILDER_H

#include "Geometry.h"/*DNT*/ // for GeoID
#include "CandidateList.h"/*DNT*/

// Note also that we need to represent the member data as "CCandidateList * m_pCandidates;"
// If we tried "CCandidateList m_Candidates;", it would not compile.

// enum that will be used by data member m_FragmentStatus

enum FragmentStatus{
	fragment_not_yet_tried,
		fragment_succeeded,
		fragment_failed};

class CFragment;
class HanditemCollection;

class CBoundaryBuilder
{
public:
	// Constructors
	CBoundaryBuilder(); 
//Bugzilla No. 78414 ; 01-30-2003 
#ifndef VARYING_HATCH_TOLERANCE
	CBoundaryBuilder(double rTolerance); // In:  Geometric tolerance
#endif

	// Destructor
	virtual ~CBoundaryBuilder();

	// Pure methods to be implemented in subclasses of CBoundaryBuilder

	// Persist a polyline boundary set
	virtual RC PersistRegion
           (int i,          // In:  The index of the region to persist
		    bool OuterOnly // In: true if we only persist the outer path
           ) = 0;

	virtual RC PersistPath(
		                   PPath pPath // In: The path to persist
						   ) = 0;


	RC Accept(PPath pPath,   // Input:  A path extracted from an entity
			  GeoID ID);       // Input:  Entity ID

	// Methods

	// Construct one boundary polyline from a clicked point
	RC ClickPoint(double x,  // Input:  The x coordinate of the clicked point
		          double y); // Input:  The y coordinate of the clicked point

	// Construct one boundary polyline from a center of entity's bounding box
	RC ClickPoint(db_handitem* pEntity); // Input: Pointer to entity

	// Persist the finalists
	virtual RC Persist()=0;

	// Persist the most recently selected regions
	virtual RC PersistMostRecentlySelected(bool & AreFinalists) = 0;

	bool AcceptHanditemColl(db_drawing *drawing, HanditemCollection &coll, C3Point ptNormal);

	// Enable the toggle operation to process or not process islands
	// consistent with the current island detection option selected.
	void SetIslandDetection(IslandOption option);

	// Sets whether clicking a second time toggles the hatch on and off.
	void SetToggle(bool toggle) {m_pCandidates->SetToggleHatch(toggle);}


	// disabled setter - don't think we want users of this class to be able to do this.
	// Also, this will cause memory leaks in its current state.
	// setter and getter for HanditemCollection
//	void setHanditemCollection(HanditemCollection * pHanditemCollection)
//	{
//		m_pHanditemCollection = pHanditemCollection;
//	}

	HanditemCollection * getHanditemCollection()
	{
		return m_pHanditemCollection;
	}

// NOTE: public (for access in cmd_hatch_objects). TO DO: make accessors if need.
public:
	PCandidateList  m_pCandidates; // List of candidates (can also identify the finalists)
	CFragment * m_pFragment;       // Instance of class used to manage the Fragment operation

protected:
	PPath m_pCurrentPath;          // The current path under construction
	bool m_bClick;                 // True if a point has been clicked
	FragmentStatus m_FragmentStatus; // The status of the Fragment operation
	HanditemCollection * m_pHanditemCollection; // the current HanditemCollection
	sds_point m_ClickPoint; // the most recent point that was clicked
	int m_iCurrPath; // The index of the current path

	virtual bool FilterList(db_drawing *drawing, HanditemCollection &coll);

	bool FilterListHelper(C3Point ptNormal, HanditemCollection &coll);
};

typedef CBoundaryBuilder * PBoundaryBuilder;

#endif





