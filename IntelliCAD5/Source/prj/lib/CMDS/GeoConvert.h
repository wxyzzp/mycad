/* C:\ICADDEV\PRJ\LIB\CMDS\GEOCONVERT.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 *  This file defines the conversions to and from Geo
 * 
 */ 

#ifndef GEOCONVERT_H
#define GEOCONVERT_H

#include "listcoll.h" // for HandItemCollection
#include "Bhatchinfo.h" // for stdObjHandleList
// #include <list> // moved to Bhatchinfo.h

#define E_NotAPolyline 3100

typedef std::list<PPath> stdPPathList;
class CDbMText;
// typedef std::list<db_objhandle> stdObjHandleList; // moved to Bhatchinfo.h

//////////////////////////////////////////////////////////////////
//							Utilities							//
//																//

void SdsGetUcsZ(sds_point result);

inline void GetUcsZ(C3Point & P)
	{
	sds_point sdsucsz;
	SdsGetUcsZ(sdsucsz);
	P.SetX(sdsucsz[0]);
	P.SetY(sdsucsz[1]);
	P.SetZ(sdsucsz[2]);
	}

inline void SdsEcsToWcs(sds_point pFrom, sds_point pTo, sds_point pNormal, bool bIsVector=false)
	{
	ASSERT(pFrom && pTo && pNormal);
	
	struct resbuf rbFrom, rbTo;

	rbFrom.restype = RT3DPOINT;	//From ECS
	ic_ptcpy(rbFrom.resval.rpoint, pNormal);

	rbTo.restype = RTSHORT;
	rbTo.resval.rint = 0;		//To WCS

	sds_trans(pFrom, &rbFrom, &rbTo, bIsVector, pTo);
	}

inline void SdsWcsToEcs(sds_point pFrom, sds_point pTo, sds_point pNormal, bool bIsVector=false)
	{
	ASSERT(pFrom && pTo && pNormal);
	
	struct resbuf rbFrom, rbTo;
	rbFrom.restype = RTSHORT;
	rbFrom.resval.rint = 0;		//From WCS

	rbTo.restype = RT3DPOINT;	//To ECS
	ic_ptcpy(rbTo.resval.rpoint, pNormal);
	sds_trans(pFrom, &rbFrom, &rbTo, bIsVector, pTo);
	}

inline void SdsUcsToEcs(sds_point pFrom, sds_point pTo, sds_point pNormal, bool bIsVector=false)
	{
	ASSERT(pFrom && pTo && pNormal);
	
	struct resbuf rbFrom, rbTo;
	rbFrom.restype = RTSHORT;
	rbFrom.resval.rint = 1;		//From UCS

	rbTo.restype = RT3DPOINT;	//To ECS
	ic_ptcpy(rbTo.resval.rpoint, pNormal);
	sds_trans(pFrom, &rbFrom, &rbTo, bIsVector, pTo);
	}

inline void SdsEcsToUcs(sds_point pFrom, sds_point pTo, sds_point pNormal, bool bIsVector=false)
	{
	ASSERT(pFrom && pTo && pNormal);
	
	struct resbuf rbFrom, rbTo;

	rbFrom.restype = RT3DPOINT;	//From ECS
	ic_ptcpy(rbFrom.resval.rpoint, pNormal);

	rbTo.restype = RTSHORT;
	rbTo.resval.rint = 1;		//To UCS

	sds_trans(pFrom, &rbFrom, &rbTo, bIsVector, pTo);
	}

inline void SdsUcsToWcs(sds_point pFrom, sds_point pTo, bool bIsVector=false)
	{
	ASSERT(pFrom && pTo);
	
	struct resbuf rbFrom, rbTo;

	rbFrom.restype = RTSHORT;	//From UCS
	rbFrom.resval.rint = 1;

	rbTo.restype = RTSHORT;
	rbTo.resval.rint = 0;		//To WCS

	sds_trans(pFrom, &rbFrom, &rbTo, bIsVector, pTo);
	}

inline void SdsWcsToUcs(sds_point pFrom, sds_point pTo, bool bIsVector=false)
	{
	ASSERT(pFrom && pTo);
	
	struct resbuf rbFrom, rbTo;
	rbFrom.restype = RTSHORT;
	rbFrom.resval.rint = 0;		//From WCS

	rbTo.restype = RTSHORT;		//To UCS
	rbTo.resval.rint = 1;

	sds_trans(pFrom, &rbFrom, &rbTo, bIsVector, pTo);
	}
									
inline void SdsToRpt(
	sds_point q,	// O: sds-style point
	C3Point & P)	// O: Geo-style point
	{
	P = C3Point(q[0], q[1], q[2]);
	}

void RptToSds(
	C3Point & P,	// I: Geo-style point
	sds_point q);	// O: sds-style point

void CAffine_to_sds_matrix(const CAffine & A, // I: a Geo affine transformation
						   sds_matrix B);     // O: the corresponding sds_matrix
//////////////////////////////////////////////////////////////////
//					Definition of CDWGToGeo						//
//																//
/*
 CDWGToGeo extracts the geometry from DWG entities and constructs
 Geo entities from them.  A typical modeling operation (e.g. create
 a BHatch) will use the functionality of CDWGToGeo.  Its input entities 
 should be hadded to it by calling ExtractEntity. The previous version
 of this class had a pure virtual Accept method, which was to be
 concretely implemented by all derived classes of CDWGToGeo depending
 upon what it plans to do with the curve.  However, in this implementation,
 the CDWGToGeo member functions simply return a GEO path as an output
 parameter in all cases.
 */
class CDWGToGeo  
	{
public:
	// Constructor destructor
	CDWGToGeo() : m_pCurrent(NULL), m_bProject(true)
		{
		}
	CDWGToGeo(bool b2D)	: m_pCurrent(NULL), m_bProject(b2D)
		{
		}
    virtual ~CDWGToGeo()
		{
		}

	// Methods
	db_handitem* CurrentEntity()
		{
		return m_pCurrent;
		}

	virtual RC ExtractEntity(
		db_handitem* pEntity,		// I: Entity's DB handle
		stdPPathList &pPathList,	// O: Pointer to the corresponding GEO boundary
		bool bExtend = false,		// I: Try to extend entity to biggest possible size 
        bool bInWcs = false);       // I: Curve created in ECS coordinates by default

protected:
	//Internal API
	virtual RC ExtractPolyline(
		db_polyline* pPoly,		    // I: Polyline's DB handle
        bool bInWCS,                // I: Curve in WCS or ECS
		PPath & pPath);		        // O: Pointer to the corresponding GEO boundary

	virtual RC ExtractLine(
		db_line* pLine,			    // I: Line entity's DB handle
        bool bInWCS,                // I: Curve in WCS or ECS
		PPath  & pPath);	        // O: Pointer to the corresponding GEO boundary

	virtual RC ExtractArc(
		db_arc* pAarc,			// I: Arc entity's DB handle
        bool bInWCS,                // I: Curve in WCS or ECS    
		PPath & pPath);		    // O: Pointer to the corresponding GEO boundary

	virtual RC ExtractCircle(
		db_circle* pCircle,
        bool bInWCS,                // I: Curve in WCS or ECS    
		PPath & pPath);

	virtual RC ExtractEllipse(
		db_ellipse* pEllipse,
        bool bInWCS,                // I: Curve in WCS or ECS    
		PPath & pPath);

	virtual RC ExtractRayOrXline(
		db_entity* pRay,		    // I: Ray or Xline entity's DB handle
        bool bInWCS,                // I: Curve in WCS or ECS    
		PPath & pPath);		        // O: Pointer to the corresponding GEO boundary

	virtual RC Extract3dface(
		db_3dface* p3dface,
        bool bInWCS,                // I: Curve in WCS or ECS    
		PPath & pPath);

	virtual RC ExtractImage(
		db_image* pImage,
        bool bInWCS,                // I: Curve in WCS or ECS    
		PPath & pPath);

	virtual RC ExtractMline(
		db_mline* pMline,			// I: Mline entity's DB handle
        bool bInWCS,                // I: Curve in WCS or ECS    
		stdPPathList &pPathList);	// O: Pointer to the corresponding GEO boundaries

	virtual RC ExtractSpline(
		db_spline* pDWGSpline,
		PPath & pGEOPath);        
	
	virtual RC ExtractText(
		db_text* pText,		        // I: Text entity's DB handle
        bool bInWCS,                // I: Curve in WCS or ECS    
		PPath & pPath,	            // O: Pointer to oriented rectangle surrounding text
		bool advance_pCurrent);	    // I: true iff we want to advance m_pCurrent

	virtual RC ExtractMtext(
		CDbMText* pMtext,		    // I: Mtext entity's DB handle
        bool bInWCS,                // I: Curve in WCS or ECS    
		PPath & pPath,		        // O: Pointer to oriented rectangle surrounding mtext
	    bool advance_pCurrent);     // I: true iff we want to advance m_pCurrent

    virtual RC ExtractDimension(
		db_dimension* pDimension,   // I: Dimension entity's DB handle
        bool bInWCS,                // I: Curve in WCS or ECS    
		PPath & pPath);		        // O: Pointer to oriented rectangle surrounding text in dimension

	virtual RC ExtractTolerance(
		db_tolerance* pTolerance,   // I: Tolerance entity's DB handle
        bool bInWCS,                // I: Curve in WCS or ECS    
		PPath & pPath);             // O: Pointer to rectangle enclosing the tolerance

	virtual RC ExtractLWPolyline(
		db_lwpolyline* pLWPolyline, // I: Lightweight polyline's DB handle
        bool bInWCS,                // I: Curve in WCS or ECS    
		PPath & pPath);             // O: Pointer to corresponding Geo path

	virtual RC ExtractAttrib(
		db_attrib* pAttrib,         // I: Attrib's DB handle
        bool bInWCS,                // I: Curve in WCS or ECS    
		PPath & pPath);              // O: Pointer to rectangle surrounding text of a visible attrib.

private:	
	// Utilities used by ExtractPolyline
	RC GetNextVertex(
		C3Point& P,			        // O: The vertex point
		double& bulge,	            // O: The bulge
        sds_point* pNormal = NULL); 

	RC ExtractPolyEdge(
		C3Point& P,			// I: edge's start point
		double bulge,		// I: The bulge
		C3Point& Q,			// I: edge's end point
        C3Point* pNormal,   // I: Optional input for the normal of pline
		PPath & pPath);	    // IO: Path to which to add the next edge

protected:
	// Data
	db_handitem* m_pCurrent;	// Cursor pointing to the current entity
	bool m_bProject;			// Project entity on z=0 plane.
	};
//////////////////////////////////////////////////////////////////
//					Definition of CGeoToDWG  					//
//																//
/*
 CGeoToDWG supports the creation of DWG entities from Geo entities.
 The correspondence between Geo is not 1 to 1. For example, a Geo
 CEllipArc may create a circular arc, a circle, an elliptical arc,
 an ellipse, or an arc edge in a polyline or LWPolyline.  It is up
 to the client of this class to decide how to convert the geo curves
 to a DWG entity, the class implements the various conversions as 
 methods.
 
 TBD: The method PolylineFromPath work in progress, it is not complete.
 Starting to work on it I realized that the approach needs to be
 modified.  My original template viewed conversion utilities (e.g.
 LWPolylineFromPath) as receiving a pointer to db_handitem as input, and
 populating them.  This may work for some entities, but it doesn't work
 for PolylineFromPath, because a POLYLINE consists of numerous handitems.
 PolylineFromPath therefore needs to be able to call the client and obtain
 new handitems.  The correct approach is probably to pass a pointer to 
 the client, from which the utility can ask for new handitems of various
 types.  This calls for the definition of a Client class that represents
 the IntelliCAD application.
 */

class CGeoToDWG
	{
public:
	// Constructor destructor

	// Default constructor
	CGeoToDWG()
		: m_ptNormal(0,0,1)
	{
		m_pDrawing = NULL;
	}

	// Constructor that takes a db_drawing
    CGeoToDWG(db_drawing * pDrawing, C3Point ptNormal)
		: m_ptNormal(ptNormal)
	{
		m_pDrawing = pDrawing;
	}

	virtual ~CGeoToDWG()
		{
		}

	// Methods
	RC LWPolylineFromPath(
		PPath pPath,		   // In: The boundary to persist
		HanditemCollection *& pLWPolyCollection); // Out: LWpolyline as HanditemCollection
	
	RC PolylineFromPath(
		PPath pPath,		                    // In: The boundary to persist
		HanditemCollection *& pPolyCollection); // Out: Polyline as HanditemCollection
	
	// Note:  The arrays v and b must be allocated and deleted by the calling routine.
	RC PolylineDataFromPath(PPath pPath, // In:  The path from which to get polyline data
		sds_point * v, // Out:  array of vertices
		double * b, // Out:  array of bulges
		int & closed, // Out: 1 if and only if closed, 0 otherwise
	    int & hasBulges, // Out:  1 iff circ arcs present, 0 otherwise
		int & nVerts); // Out:  number of vertices

#ifdef SplitArcs
	RC BhatchArcParmsFromEllipArc(
				PEllipArc pEllipArc, // In: The arc to analyze
				int & nEdges, // Out:  edges in corresp DWG entity
				double start[3], // Out: the (up to 3) start parms
				double end[3]); // Out:  the (up to 3) end parms
#endif // SplitArcs
	
	RC BhatchFromPath(
		PPath pPath,			// In:  The path to persist
		int i,                  // In:  The index of this path in the Bhatch
		int pathFlag,           // In and Modified internally: the boundary path type flag
		stdObjHandleList * pAssociates, // In:  The list of handles of associated objects
		db_hatch *& pHatch);	// In/Out: The Bhatch in which the path participates

	// Data

	db_drawing * m_pDrawing;

protected:
	C3Point m_ptNormal;
	};

#endif


