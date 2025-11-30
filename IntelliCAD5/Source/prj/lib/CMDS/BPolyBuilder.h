/* C:\ICADDEV\PRJ\LIB\CMDS\BPOLYBUILDER.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract

 Definition of CBPolyBuilder

As a subclass of CBoundaryBuilder, CBPolyBuilder knows how to
receive a list of DWG entities and a sequence of clicked points,
and how to construct an internal final list of regions.  Its own
special methods are PersistRegion and PersistPath, the first [probably]
calling the second repeatedly, to convert the final list to DWG entities
and hand them over to the Client.  In order to do the conversion, CBPolyBuilder
needs conversion utilities; that is why it uses CGeoToDWG.  Originally this
was to have been implemented through inheritance through the base class
CBoundaryBuilder, but in this implementation, we are going to use composition
and see whether that works well for us.
*/

#ifndef BPOLYBUILDER_H
#define BPOLYBUILDER_H

#include "BoundaryBuilder.h"

class CBPolyBuilder : public CBoundaryBuilder
{
public:

	// Constructors
	//Bugzilla No. 78414 ; 01-30-2003 
#ifdef VARYING_HATCH_TOLERANCE
	CBPolyBuilder( C3Point ptNormal, // In:	 Extrusion vector for ECS
		          RC & rc);          // Out: Return code
#else
	CBPolyBuilder(double rTolerance, // In:  Geometric tolerance
				  C3Point ptNormal,	 // In:	 Extrusion vector for ECS
		          RC & rc);          // Out: Return code
#endif //VARYING_HATCH_TOLERANCE





	// Destructor
	~CBPolyBuilder()
	{
		if (m_pHanditemCollection)
			delete m_pHanditemCollection;
	}


// Persist a polyline boundary set
	RC PersistRegion(int i,          // In:  The index of the region to persist
					 bool OuterOnly); // In: true if we only persist the outer path

	RC PersistPath(PPath pPath); // In:  The boundary to persist

	RC Persist();

	// Persist the most recently selected regions
	RC PersistMostRecentlySelected(bool & AreFinalists);

private:

	// Data
	bool m_bLightWeight; // Construct lightweight polylines if TRUE
	                     // In the initial implementation of this class
	                     // m_bLightWeight will always be set to false
	                     // by the constructor.  When IntelliCAD implements
	                     // lightweight polylines, we will implement cases
	                     // where m_bLightWeight could be TRUE.


protected:
	virtual bool FilterList(db_drawing *drawing, HanditemCollection &coll);
	C3Point m_ptNormal;	 // Extrusion vector needed for filtering and creating output polylines
						 // Needed for setting 210 group code.
};

#endif


