/* G:\ICADDEV\PRJ\LIB\CMDS\BHATCHBUILDER.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Definition of the CBHatchBuilder class
 * 
 */ 

#ifndef BHATCHBUILDER_H
#define BHATCHBUILDER_H

#include "BoundaryBuilder.h"

enum HatchModStatus { hm_success=0, hm_failure, hm_handitem_not_found };

//Bugzilla No. 78414 ; 01-30-2003 
#ifndef VARYING_HATCH_TOLERANCE
const double HATCH_TOLERANCE = 0.00001;
#endif


// helper classes

class RegionPathPoint
	{  
	public:
		RegionPathPoint(){};
		RegionPathPoint(int iRegion, int jPath, sds_point ClickPoint);
	public:
		int m_iRegion;
		int m_jPath;
		sds_point m_ClickPoint;
	};

class RegionPathPointArray
	{
	public:
	RegionPathPointArray();
	~RegionPathPointArray();
	void AddItem(int iRegion, int jPath, sds_point ClickPoint);
	bool GetItem(int iRegion, int jPath, sds_point ClickPoint);

	// data
	public:
		int m_Size;
		RegionPathPoint * m_Item;
	};



class CBHatchBuilder : public CBoundaryBuilder
	{ // begin definition of CBHatchBuilder class
	
	public:
		
		// Constructor
		//Bugzilla No. 78414 ; 01-30-2003 
#ifdef VARYING_HATCH_TOLERANCE
		CBHatchBuilder(
			C3Point ptNormal,  // In:  Extrusion vector for ECS
			RC & rc);          // Out:  Return code
#else
		CBHatchBuilder(double rTolerance, // In:  Geometric tolerance
			C3Point ptNormal,  // In:  Extrusion vector for ECS
			RC & rc);          // Out:  Return code
#endif //VARYING_HATCH_TOLERANCE
		
		// Destructor
		virtual ~CBHatchBuilder();
		
		
		// Methods
	protected:	
		virtual bool FilterList(db_drawing *drawing, HanditemCollection &coll);
		
	public:
		virtual RC PersistPath(PPath pPath); // In:  the path to persist
			           		
        virtual RC PersistRegion(int i,          // In:  The index of the region to persist
			bool OuterOnly); // In: true if we only persist the outermost

		virtual RC Persist();

		// Persist the most recently selected regions
		virtual RC PersistMostRecentlySelected(bool & AreFinalists);
		
		// bool indicates whether the handitem collection should take care of deleting the hatch
		// (note: this can be reset by calling manipulating the hand item collection). 
		// This is needed so the hatch can be rebuilt
		virtual bool set_hatch(db_hatch * pHatch, bool deleteHatch);
		virtual bool get_hatch(db_hatch ** pHatch);


		virtual bool SimplestCase(db_objhandle * pObjHandle,
			                      int & iPath);

		virtual RC ModifySimplestHatch(db_objhandle * pObjHandle,
									   int iPath);
		
		virtual RC ModifyHatch(db_objhandle * pObjHandle);		

		// general modify - it essentially recreates the hatch but it is easier to work inside the
		// hatch since db does not have access to a hand item collection.
		virtual RC ModifyGeneralHatch(db_objhandle *pHanditem);

		void AnalyzeHatch(int & numPaths, int * & numBoundObjsInPath);
		
		// Data
	protected:
		
		C3Point m_ptNormal;	 // Extrusion vector needed for filtering
		db_hatch * m_pBhatch;  // the associative hatch

	public:
		bool m_updateSeeds; // a state designator for whether or not to update the seeds.
		bool m_storeSeeds; // a state designator for whether or not to store the seeds in m_pBhatch

	private:
		RegionPathPointArray m_RegionPathPointArray; // keeps track of the pick points
		
	};//   end definition of CBHatchBuilder class



#endif



