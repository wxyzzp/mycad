/*	BOOLEAN.H - Defines the class of CBoolean and its methods
 *	Copyright (C) 1998 Visio Corporation. All rights reserved.
 *
 *
 *	ABSTRACT
 *
 * CBoolean captures all the intermediate data needed during a boolean operations
 *
 */
#ifndef BOOLEAN_H
#define BOOLEAN_H

#include "Roots.h"
#include "Curve.h"
#include "Array.h"

// These constants signal the difference between Union and Intersect
#define INSIDE 2
#define OUTSIDE 3

// Internal constants, but they are used in inline methods
#define UNKNOWN 0
#define REDUNDANT 4
#define HEAD 1
#define CLOSED 2

// Forward declarations
class CJunction;
typedef CJunction * PJunction;
class CBooleanCurve;
typedef CBooleanCurve * PBooleanCurve;
class CBooleanPath;
typedef CBooleanPath * PBooleanPath;
class CBoolean;
typedef CBoolean * PBoolean;
class CPathList;
typedef CPathList * PPathList;
class CFragment;
typedef CFragment * PFragment;
//////////////////////////////////////////////////////////////////////////////////
// The class of curves with additional information for Boolean opereation
class GEO_CLASS CBooleanCurve		:    public CGeoObject
{
	friend class CBooleanPath;
	friend class CPathList;

public:
	// Constructor and destructors
	CBooleanCurve();

	CBooleanCurve(
		PCurve pCurve,		// In: The curve
		double rTolearence,	// In: Tolerance
		GeoID id);			// In: Reference ID

	virtual ~CBooleanCurve();

// Methods
	GeoID ID()
		{
		return m_id;
		}

	double Start()
		{
		return m_pCurve->Start();
		}

	double End()
		{
		return m_pCurve->End();
		}

	void UpdateIntersections();

	RC GetNewCurve(
		double rAt,     // In: Paramter value to split at
		PCurve & pNew,	// Out: The new curve segment
		int& nStatus,	// Out: Status
		BOOL& bClosed);	// In: =TRUE if this is a one-curve closed path

	RC Split(
		int nCurveIndex,      // In: Index of this curve in the path
		int nLast,            // In: Index of the path's last curve
		double rTolerance,    // In: Point-distance tolerance
		int & nSplitType,	  // Out: Type of split, if any
		PBooleanCurve & pNew, // Out: The new curve splinter
		BOOL & bClosed);	  // Out: Set to False if a closed curve was split open here

	RC CastRay(
		PRay& pRay);  // Out: The ray

	RC Direction(
		double rAt,             // In: Parameter value on the curve
		C3Point rptRelativeTo,	// In: The given direction
		int & nSide);			// Out: left or right

#if defined DEBUG
		void Dump();
#endif

// Data
protected:
	PCurve m_pCurve;			  // The curve 
	CExtents m_cExtents;		  // Extents
	CRealArray m_cIntersections;  // List of intersections
	GeoID m_id;					  // Reference ID
};
//////////////////////////////////////////////////////////////////////////////////
// The class Path with additional information for Boolean opereation
class GEO_CLASS CBooleanPath  :  public CGeoObject
{
	friend class CPathList;

public:
	// Constructor and destructors
	CBooleanPath();

	CBooleanPath(
		BOOL bClosed,	// In: It's going to be a closed path if TRUE
		BOOL bFillable,	// In: Make it fillable if TRUE
		GeoID id);		// In: Reference ID

	CBooleanPath(
		PBooleanPath pOther);  // In: The other path this was carved from

	virtual ~CBooleanPath();

// Inline methods
	PBooleanCurve Curve(
		int i)  // In: The index of the desired curve
		{
		return (PBooleanCurve)m_cCurves[i];
		}

	int Size()
		{
		return m_cCurves.Size();
		}

	PCurve FirstCurve()
		{
		return Curve(0)->m_pCurve;
		}

	PCurve LastCurve()
		{
		return Curve(m_cCurves.Size()-1)->m_pCurve;
		}

	RC Add(
		PBooleanCurve pCurve)   // In: the curve to be added
		{
		return m_cCurves.Add((PGeoObject)pCurve);
		}

	int Status()
		{
		return m_nStatus;
		}

	PJunction StartJunction()
		{
		return m_pStartJunction;
		}

	PJunction EndJunction()
		{
		return m_pEndJunction;
		}

	void SetEndJunction(
		PJunction pJunction)  // In: The junction to set to
		{
		m_pEndJunction = pJunction;
		}

	void SetStartJunction(
		PJunction pJunction)  // In: The junction to set to
		{
		m_pStartJunction = pJunction;
		}

	PBooleanPath Twin()
		{
		return m_pTwin;
		}

	PBooleanPath Next()
		{
		return m_pNext;
		}

	PBooleanPath Previous()
		{
		return m_pPrevious;
		}

	BOOL Fillable()
		{
		return m_bFillable;
		}

	void SetFillable(
		BOOL bSetTo)  // In: The value to set to
		{
		m_bFillable = bSetTo;
		}

	BOOL Available()
		{
		return m_nStatus == UNKNOWN   ||   m_nStatus == HEAD;
		}

	BOOL Good()
		{
		return m_nStatus == HEAD   ||   m_nStatus == CLOSED;
		}

	void Update(
		CExtents & cExtents)
		{
		// Update cExtents with this path's extents
		cExtents.Update(m_cExtents);
		}

	BOOL Closed()
		{
		return m_bClosed;
		}

// Methods	
	RC AcceptCurve(
		PCurve pCurve,		// In: The curve
		double rTolerance,	// In: Point-distance tolerance
		GeoID id);			// In: Reference ID

	RC EndPathIn();

	RC GetIntersections(
		PBooleanPath pOtherPath,  // In: Other list of curves
		double rTolerance);       // In: Geometric tolerance

	RC Split(
		double rTolearence,		  // In: Tolerance
		PBooleanPath & pNewPath); // Out: The new splinter

	RC Orient(
		CPathList & pShape);	// In: List of all paths in this shape

	RC Classify(
		CPathList & pShape); //In/Out: Other shape's list of paths

	RC Intercept(
		PRay pRay,            // In: The ray
		BOOL bCountOverlaps,  // In: = TRUE if overlaps are to be counted 
		int & nIntercepts,    // In/Out: Number of intercepts so far
		int & nOverlaps);	  // Out: Overlaps indicator
	
	RC Reverse();

	BOOL Bounded();

	RC GetEndPoints(
		C3Point & ptStart,    // Out: The path's first point
		C3Point & ptEnd);     // Out: The path's last point

	RC Append(
		PBooleanPath pNext); // In/Out: The path to append

	RC UpdateExtents(
		double rTolearence);	  // In: Tolerance

	RC AddCurvesToPath(
		BOOL bTrim,			// In: Trim the curves if TRUE
		PStorage pClient);	// In/Out: The client of the operation

	RC AddClosedPath(
		CPathList  & cList,	  // In/Out: The list of output closed paths
		PBooleanPath & pLead); // In/out: set to NULL if used in the closed path

	BOOL Closes(
		PBooleanPath pPath);	// In: A path

	void LinkTo(
		PBooleanPath p);	// The path to link to (NULL OK)

	void Unlink();

	void WrapUp(
		double rTolerance);		// In: Gap-closing tolerance

	// Fragment-specific methods	
	RC TestForDuplicate(
		PBooleanPath pOther,    // In: Potential duplicate
		double rTolerance,		// In: Tolerance
		BOOL & bDuplicate);		// Out: =TRUE if Other duplicates this path

	RC CreateTwin(
		double rTolerance);	// In: Point-distance tolerance

	RC Orientation(
		int & bCounterClockwise);  // Out: =TRUE if counterclockwise

	RC Locate(
		PRay pRay,         // In: The ray
		double & rNearest, // In/Out: Ray-parameter of the nearest intercept
		BOOL & bCloser);   // Out: =TRUE if this is a clser container

	// Joine specific methods
	RC Extend(
		PBooleanPath pExtension);	 //In: The extension

	BOOL PathHead();

	void StrikeOut();

#if defined DEBUG
	void Dump();
#endif

// Data
	PBooleanPath	m_pPrevious;	 // The previous path (for assembling)
	PBooleanPath	m_pNext;		 // The next path (for assembling)
protected:
	CGeoObjectArray	m_cCurves;		 // Curve-list
	int				m_nStatus;       // Status w.r.t the other operand
	CExtents		m_cExtents;		 // Path's loose extents
	PBooleanPath	m_pTwin;		 // The twin path (for Fragment)
	PJunction		m_pEndJunction;	 // The end-junction
	PJunction		m_pStartJunction;// The start-junction
	BOOL			m_bFillable;	 // =TRUE if the path is fillable
	BOOL			m_bClosed;		 // =TRUE if the path is closed
	GeoID			m_id;			 // Reference id
};
//////////////////////////////////////////////////////////////////////////////////
// A list of Boolean paths
class GEO_CLASS CPathList  :  public CGeoObjectArray
{
public:
	// Constructor and destructors
	CPathList();
	virtual ~CPathList();

// Inline methods
	PBooleanPath Path(
		int nIndex)      // In: Index of the desired path in the list
		{
        return (PBooleanPath)(m_pEntries ? m_pEntries[nIndex] : 0 );
		}

	PBooleanPath Pop()
		{
        return (PBooleanPath)( m_pEntries ? m_pEntries[--m_nSize] : 0 );
		}

	void SetID(
		GeoID id)			// In: Reference ID
		{
		m_ID = id;
		}

	GeoID ID()
		{
		return m_ID;
		}

// Methods
	RC Split(
		PBooleanPath pPath,       // In: The path to split at
		PBooleanCurve pNewCurve); // In: The header of the new path's curve-list

	RC GetIntersections(
		CPathList * pOtherShape,  // Other shape's path-list
		double rTolerance);       // In: Geometric tolerance

	RC SplitAtIntersections(
		double rTolearence);		  // In: Tolerance

	RC UpdateInOrOut(
		PRay pRay,                 // In: The ray
		int & nStatus,             // Out: overlap indicator
		int & nIntercepts);        // In/Out: = Number of ray intercpts

	RC Orient();

	RC Classify(
		CPathList & pShape);	 // In: Other shape

	RC UnionOrIntersect(
		int nAdmit);		// In: Admissable path type

	PJunction GetJunction(
		const C3Point & ptP);   // In: The point

	void Reset();

	RC GetArea(
		double & rArea);  // Out: the total area

	BOOL HitTest(
		C3Point & P);	// In: A point

#if defined DEBUG
	void Dump();
#endif
	// Data
protected:
	GeoID m_ID;   // Used in CTrim ony
};
//////////////////////////////////////////////////////////////////////////////////
// A junction for Boolean operations
class GEO_CLASS CJunction	:  public CGeoObject
{
public:
// Construction/destruction
	CJunction();
	
	CJunction(
		const C3Point & cPoint);	  // In: The junction point

	virtual ~CJunction();

// Inline methods
	PBooleanPath Path(
		int i)  // In: The index of the desired path
		{
		return (PBooleanPath)m_cPaths[i];
		}

	int Size()
		{
		return m_cPaths.Size();
		}

	C3Point Point()
		{
		return m_cPoint;
		}

	PBooleanPath Pop()
		{
		return m_cPaths.Pop();
		}

// Methods
	RC Add(
		PBooleanPath pPath);  // In: The path to be added

	void Remove(
		PBooleanPath p);  // In: The path to remove

	BOOL Forward(// Return TRUE if this closes a path
		PBooleanPath & pPathIn,	  // In/out: The path coming in, NULL OK
		PBooleanPath & pPathOut); // Out: The next path, NULL if none found

	PJunction BackTrack(
		PBooleanPath& pPath);	// In: The current path, NULL OK

	// Join specific methods
	RC Extend(
		PBooleanPath pPath);	 //In: The path to be extended

	RC YieldPaths(
		CPathList & cList);  // In/Out: The list of joined paths


#if defined DEBUG
	void Dump();
#endif

// Data
protected:
	CPathList m_cPaths;	  // The paths that emanate from this junction
	C3Point m_cPoint;	  // The junction point
};

//////////////////////////////////////////////////////////////////////////////////
// The infrastructure for Boolean operations
class GEO_CLASS CBoolean	:    public CGeoObject
{
public:
	CBoolean();

	CBoolean(
		int nAdmissable,   // In: The type of paths that will stay after the operation
		double rTolerance, // In: Point-distance tolerance
		PStorage pClient); // In: The client of this operation
		 
	virtual ~CBoolean();

	// Inline Methods
	void SetID(
		GeoID id)			// In: Reference ID
		{
		m_id = id;
		}

	PJunction Junction(
		int i)  // In: The index of the desired junction
		{
		return (PJunction)m_cJunctions[i];
		}
	
	virtual BOOL Trim()
		{
		return TRUE;
		}

// Methods
	virtual RC NewPathIn(
		BOOL bClosed,	  // In: It's going to be a closed path if TRUE
		BOOL bFillable,	  // In: Make it fillable if TRUE
		GeoID id=INV_ID); // In: Reference ID

	virtual RC AcceptCurve(
		PCurve pCurve,	  // In: The curve
		GeoID id=INV_ID); // In: Reference ID

	RC EndPathIn();

	PJunction GetJunction(
		const C3Point & ptP); // In: The point

	RC Process(
		BOOL bFull);            // In: Full preprocessing if TRUE

	virtual RC Organize(
		GeoID id=INV_ID); // In: Reference ID

	BOOL EmptyIntersection();

	double RealisticTolerance();

	virtual RC Operate(
		GeoID id=INV_ID); // In: Reference ID

	virtual RC AddPathToJunction(
		PBooleanPath pPath,	     // In: The path
		PJunction pJunction);    // In: The junction it is about to join

	virtual RC NewPathOut(
		PBooleanPath pSource);	// In: Boolean path to create from

	RC CreateShape(
		GeoID id,		  // In: ID for reference
		BOOL bSkipText,	  // In: =TRUE if text is to be ignored
		BOOL bSkipFill,	  // In: =TRUE if the shape's fill pattern is to be ignored
		CPathList& cList, // In/Out: The path list, emptied here
		PStorage pClient); // In/Out: The client who receives the shape

	virtual RC WrapUp();

	RC ConstructGraph();

	virtual RC Assemble();

	RC GetArea(
		double& rArea);	  // Out: the total area

	/* Albert.Isaev.ITC[31.7.2002/19:16:51] */
	void Purge();			//Calls Purge methods for m_cStock and m_cTool

// Data
protected:
	CPathList m_cStock;           // First shape's paths
	CPathList  m_cTool;           // Second shape's paths
	PBooleanPath m_pCurrentPath;  // The current input path
	PPathList m_pReceiving;		  // The currently receiving path list
	CGeoObjectArray m_cJunctions; // List of junction
	int m_nAdmissable;            // The type of path that gets to stay
	double m_rTolerance;		  // Point-distance tolerance
	CExtents m_cExtents;		  // Geometry extents
	GeoID m_id;					  // Reference ID
	BOOL m_bEmpty;				  // =TRUE if any input shape is empty
	PStorage m_pClient;			  // The operation client
};
//////////////////////////////////////////////////////////////////////////////////
// The infrastructure for the Subtract operation
class GEO_CLASS CSubtract   :  public CBoolean
{
public:
	CSubtract(
		double rTolerance, //In: Point-distance tolerance 
		PStorage pClient); // In: The client of this operation

	virtual ~CSubtract();

// Methods
	RC Operate(
		GeoID id=INV_ID); // In: Reference ID

// No additional data
};
//////////////////////////////////////////////////////////////////////////////////
// The infrastructure for Fragment operation
class GEO_CLASS CFragment	:  public CBoolean
{
public:
	CFragment();

	CFragment(
		double rTolerance, // In: Point distance tolerance
		PStorage pClient); // In: The client of this operation

	virtual ~CFragment();

// Inline methods
	PPathList Shape(
		int i)  // In: The index of the desired junction
		{
		return (PPathList)m_cResults[i];
		}
	
	PBooleanPath ShapeBoundary(
		int i)  // In: The index of the desired junction
		{
		return ((PPathList)(m_cResults[i]))->Path(0);
		}

// Methods
	virtual RC NewPathIn(
		BOOL bClosed,	// In: It's going to be a closed path if TRUE
		BOOL bFillable,	// In: Make it fillable if TRUE
		GeoID id=INV_ID); // In: Reference ID

	RC Organize(
		GeoID id=INV_ID); // In: Reference ID

	RC Operate(
		GeoID id=INV_ID); // In: Reference ID

	RC AddPathToJunction(
		PBooleanPath pPath,		 // In: The path
		PJunction pJunction);    // In: The junction it is about to join

	RC WrapUp();

	RC CreateTwins();

	RC SortOut();

	RC Locate(
		PBooleanPath pPath);  // In: The hole

	virtual RC ProduceResults();
// Data
protected:
	CGeoObjectArray	m_cResults;
	CPathList m_cOcean;
};
//////////////////////////////////////////////////////////////////////////////////
// The infrastructure for Combine operation
class GEO_CLASS CCombine	:  public CBoolean
{
public:
	CCombine();

	CCombine(
		PStorage pClient);	 // In: The client of this operation

	virtual ~CCombine();

// Methods
	virtual RC NewPathIn(
		BOOL bClosed,	// In: It's going to be a closed path if TRUE
		BOOL bFillable,	// In: Make it fillable if TRUE
		GeoID id=INV_ID); // In: Reference ID

	RC AcceptCurve(
		PCurve pCurve,	  // In: The curve
		GeoID id=INV_ID); // In: Reference ID

	BOOL Trim()
		{
		return FALSE;
		}

	RC Operate(
		GeoID id=INV_ID); // In: Reference ID

	virtual RC NewPathOut(
		PBooleanPath pSource);	// In: Boolean path to create from

	RC WrapUp();

// No data
};
//////////////////////////////////////////////////////////////////////////////////
// The infrastructure for Trim operation
class GEO_CLASS CTrim	:  public CFragment
{
public:
	CTrim(
		double rTolerance, // In: Point distance tolerance
		PStorage pClient); // In: The client of this operation

	virtual ~CTrim();

// Inline methods
	PPathList Pop()
		{
		return (PPathList)m_cResults.Pop();
		}

// Methods
	virtual RC NewPathIn(
		BOOL bClosed,	  // In: It's going to be a closed path if TRUE
		BOOL bFillable,	  // In: Make it fillable if TRUE
		GeoID id=INV_ID); // In: Reference ID

	virtual RC NewPathOut(
		PBooleanPath pSource);	// In: Boolean path to create from

	RC Operate(
		GeoID id=INV_ID); // In: Reference ID

	RC WrapUp();

// No data
};
//////////////////////////////////////////////////////////////////////////////////
// The infrastructure for Join operation
class GEO_CLASS CJoin	:  public CFragment
{
public:
	CJoin(
		double rTolerance, // In: Point distance tolerance
		PStorage pClient); // In: The client of this operation

	virtual ~CJoin();

// Methods
	virtual RC NewPathIn(
		BOOL bClosed,	// In: It's going to be a closed path if TRUE
		BOOL bFillable,	// In: Make it fillable if TRUE
		GeoID id=INV_ID); // In: Reference ID

	RC AcceptCurve(
		PCurve pCurve,	  // In: The curve
		GeoID id=INV_ID); // In: Reference ID

	RC Operate(
		GeoID id=INV_ID); // In: Reference ID

	RC Assemble();
	
	virtual RC NewPathOut(
		PBooleanPath pSource);	// In: Boolean path to create from

	virtual BOOL Trim()
		{
		return FALSE;
		}

	RC WrapUp();

// No data
};

//////////////////////////////////////////////////////////////////////////////////
// The infrastructure for DrawRegion operation
class GEO_CLASS CDrawRegion	:  public CFragment
{
public:
	CDrawRegion();

	CDrawRegion(
		double rTolerance,		// In: Point distance tolerance
		PStorage pClient,		// In: The client of this operation
		P3Point pPick=NULL);	// In: Pick point (optional)

	virtual ~CDrawRegion()
		{
		}

// Methods
	virtual RC ProduceResults();

// Data
protected:
	P3Point m_pPickPoint;	// Pick point
};


#endif

