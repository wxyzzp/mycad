/* D:\ICADDEV\PRJ\GR\GR_HATCH.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 *
 *
 *	ABSTRACT
 * The hatch is defined by BOUNDARIES and a PATTERN.  The boundaries
 * are linearized into a CACHE of polygons.
 * The PATTERN is defined by one of more LINES. Every pattern line
 * has a LINESTYLE, which may be SOLID or DASH.  From the line we
 * generate INSTANCES, removed from each other by an OFFSET vector.
 * Every instance is sliced against the polygons to create SLICES.
 * From the slices we generates SEGMENTS - which are what the user
 * will see. If the line is solid than the slices = segments. Othewise
 * the SEGMENTS will be the intersection of dashes with slices.
 *
 * These terms are captured by several classes.  Since the classes are
 * only used locally, they are declared in the top part of this file,
 * rather than in a header file, but this may change.
 *
 */

/*DG - 17.1.2002*/// These headers are not necessary due to the change on 3.10.2001.
/*
//IntelliKorea 2001/8/14
#include "icad.h"
#include "cmds.h"
//IntelliKorea 2001/8/14
*/

#include "gr.h"
#include "db.h"
#include "gr_DisplaySurface.h"
#include <float.h>		// For DBL_MAX
#include "Extents.h"
#include "Knots.h"
#include "CtrlPts.h"
#include "Curve.h"
#include "NURBS.h"
#include "EllipArc.h"
#include "Line.h"
#include "polycache.h"


// defining for creating disp. objs with boundaries instead of true hatching
//#define DEBUG_BOUNDARIES


// Defining for the new version of solid hatching generation.
// The old one doesn't want each polygon in cache to be a representation of one path of boundary
// and tries to create GDI paths based true filling (which doesn't work for all boundaries cases).
#define NEW_SOLID_GENERATE

/*DG - 17.1.2002*/// Sorry for this global one. It would be better to move this flag to CHatchDisplay
// but within the current design it's used in CInstance::AddSegment (see comments in it).
// This var is 'true' if some boundary path has anomalous order of edges,
// i.e. some edge j is not strunged together with edge j-1 within a tolerance (FUZZ_DIST)
static bool g_bAbnormalPathEdgesChaining = false;


//////////////////////////////////////////////////////////////////////
//																	//
//					D E F I N I T I O N S							//
//																	//
//////////////////////////////////////////////////////////////////////

#define MAX_HATCH_LINES 100000
#define MAX_DASHES 1000000
#define STYLE_OUTERMOST 1
#define STYLE_IGNORE 2
#define PATH_POLYLINE 2
#define PATH_EXTERNAL 1
#define PATH_TEXTBLOCK 8
#define PATH_OUTERMOST 16

// Check if construction succeeded
#define CHECK_CONSTRUCTION(p,rc)\
	{\
	if (!p) rc = DM_MEM;\
	else if (rc) {delete p; p = NULL;}\
	}

class CInstance;

//////////////////////////////////////////////////////////////////
class CLineStyle
	{
public:
// Constructor
	CLineStyle()
		{
		}

	virtual ~CLineStyle()
		{
		}

// Methods

	virtual void ResetForNewInstance(
		double rStart)	// In: Lower limit of the first slice
		{
		// Do nothing stub, overriden in CDash
		}

	virtual RC DoOneSlice(
		const C3Point& V,				// Line direction vector
		CInstance& cInstance,			// In: Line instance
		gr_DisplaySurface* m_pSurface,	// Display surface
		BOOL& bDone)=0;					// =TRUE if last slice done

// No data
	};
//////////////////////////////////////////////////////////////////
class CDash		:		public CLineStyle
	{
public:
	// Constructors/destructor
	CDash()
		{
		m_nLast = m_nTotal = m_nCurrent = 0;
		m_rStart = m_rEnd = 0;
		m_pDashes = NULL;
		m_rLength = m_rFrequency = 1;
		}

	CDash(
		int nCount,			// In: Number of dashes
		double* pDashes,	// In: Dash lengths array
		RC& rc);			// Out: Return code

	virtual ~CDash()
		{
		}

// inline Methods
	// Current dash start
	inline double DashStart()
		{
		return m_rStart;
		}
	/*****************************************************************/
	// Current dash end
	inline double DashEnd()
		{
		return m_rEnd;
		}

// Methods
	void ResetForNewInstance(
		double rStart);	// In: Lower limit of the first slice

	RC DoOneSlice(	// Return TRUE if done with this slice
		const C3Point& V,				// Line direction vector
		CInstance& cInstance,			// In: Line instance
		gr_DisplaySurface* m_pSurface,	// Display surface
		BOOL& bDone);					// =TRUE if last slice done

	RC GetNextDash();

// Data
protected:
	int m_nLast;		 // Index of the last dash
	double* m_pDashes;	 // Array of dash values
	int m_nTotal;		 // Total dash counter
	int m_nCurrent;		 // The index of the current dash in the dash pattern
	double m_rStart;	 // Current dash start
	double m_rEnd;		 // Current dash end
	double m_rLength;	 // Total length of this dash cycle
	double m_rFrequency; // The inverse of m_rLength
	};

//////////////////////////////////////////////////////////////////
class CSolid		:		public CLineStyle
	{
public:
	// Constructor/destructor
	CSolid()
		{
		}
	virtual ~CSolid()
		{
		}
// Methods
	RC DoOneSlice(	// Return TRUE if done with this slice
		const C3Point& V,				// Line direction vector
		CInstance& cInstance,			// In: Line instance
		gr_DisplaySurface* m_pSurface,	// Display surface
		BOOL& bDone);					// =TRUE if last slice done
// No data
	};

//////////////////////////////////////////////////////////////////
class CInstance   :   public CRealArray
	{
public:
	// Constructor/destructor
	CInstance();

	~CInstance()
		{
		}

// Inline methods
	// Current slice's start
	double SliceStart()
		{
		return m_rEntries[m_nCurrent];
		}
	/*****************************************************************/
	// Current slice's end
	double SliceEnd()
		{
		return m_rEntries[m_nCurrent+1];
		}

	/*****************************************************************/
	// Move to the next slice
	BOOL NextSlice()
		{
		m_nCurrent += 2;
		return m_nCurrent >= Size();
		}

// Methods
	void Set(
		C3Point P,			// In: Instance base point
		const C3Point& V,	// In: Line's direction vector
		const C3Point& N);	// In: Line's normal

	RC Intersect(
		const C3Point& V,	// In: Line's direction vector
		const C3Point& A,	// In: Polygon point
		double a,			// In: The dot product A*N
		const C3Point& B,	// In: Other Polygon point
		double b,			// In: The dot product B*N
		double rFuzz);		// In: Intersection fuzz

	RC ClipAwayBlock(
		const C3Point& V,	// In: Line's direction vector
		const C3Point* P,	// In: Polygon 5 vertices (last=first)
		double* r,			// In: r[i] = P[i]*N
		double rFuzz);		// In: Intersection fuzz

	RC Insert(
		double t);	// In: An intersection parameter to insert

	RC AddSegment(
		const C3Point& V,			 // Line direction vector
		gr_DisplaySurface* pSurface, // Display surface
		double rStart,				 // In: Segment start
		double rEnd);				 // In: Segment end

// Data
protected:
	C3Point m_P;	// A base point
	double m_rPN;	// The dot product P*N
	double m_rPV;	// The dot product P*V
	int m_nCurrent; // Current slice
	};


//////////////////////////////////////////////////////////////////
class CHatchDisplay
{
	friend int gr_DisplayHatch(db_hatch* pHatch, gr_DisplaySurface* pSurface);

public:
	// Constructors/destructors
	CHatchDisplay();

	virtual ~CHatchDisplay()
	{
	}

	CHatchDisplay(db_hatch* pHatch, gr_DisplaySurface* pSurface);

// Inline method(s)
	// Add a point for an ongoing polygon
	RC	AddVertex
		(
		 const C3Point	P,			// In: The point to add
		 CCache*		pCache		// In: The cache to add it to
		)
		{
			m_cExtents.Update(P);
			return pCache->Accept(P, 0);
		}

// Methods
	RC		Reset();
	void	Clear();
	RC		AddCurve(PCurve& pCurve, CPolyCache* pCache);
	RC		LinearizePath(int i);

#ifdef DEBUG_BOUNDARIES
	void	Test();
#endif

	void	Locate(const C3Point& P, double& rPN, int& n);
	RC		Slice();
	RC		ClipAwayBlocks();

	virtual RC	Generate();

	bool	xclipBoundary();
	RC		DoOneLine();

	// Data
protected:
	// Output target
	gr_DisplaySurface*	m_pSurface;	// Display surface
	double				m_rFuzz;	// Resolution-dependent fuzz

	// Hatch pattern
	db_hatch*	m_pHatch;		// Hatch entity pointer
	CLineStyle*	m_pLineStyle;	// The style of the current line
	C3Point		m_vecOffset;	// Offset vector

	// Hatch line geometry
	C3Point		m_ptBase,		// Line's base point
				m_V,			// Line's direction vector
				m_N;			// Line's normal vector
	double		m_rBaseN;		// The dot product m_ptBase * N

	// Hatch line instances
	int			m_nFirst,		// index of first instance
				m_nLast;		// index of last instance
	double		m_rFrequency;	// Instances per distance
	CInstance*	m_pInstances;	// List of instances

	// Processed Boundaries
	CPolyCache	m_cPolys,		// Array of boundary polygons
				m_cTextBlocks;	// Array of textblock polygons
	CExtents	m_cExtents;		// Boundary entities extents

	bool		m_bSolidFill;	// does the hatch have solid fill pattern
};


//////////////////////////////////////////////////////////////////
class CFillDisplay : public CHatchDisplay
{
public:
	// Constructors/destructors
	CFillDisplay() : CHatchDisplay()
	{
		m_bSolidFill = true;
	}

	virtual ~CFillDisplay()
	{
	}

	CFillDisplay(db_hatch* pHatch, gr_DisplaySurface* pSurface) : CHatchDisplay(pHatch, pSurface)
	{
		m_bSolidFill = true;
	}

	RC	Generate();
};


//////////////////////////////////////////////////////////////////
//																//
//				I M P L E M E N T A T I O N						//
//																//
//////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////
//																//
//					Implementation of CDash						//
//																//
// Constructor
CDash::CDash(
	int nCount,			// In: Number of dashes
	double* pDashes,	// In: Dash lengths array
	RC& rc)				// Out: Return code
	{
	m_nLast = nCount - 1;
	m_pDashes = pDashes;
	m_nTotal = m_nCurrent = 0;
	m_rLength = m_rStart = m_rEnd = 0;
	m_rFrequency = 1;

	// Get the total length of the dash cycle
	for (int i = 0;    i <= m_nLast;   i++)
		m_rLength += fabs(pDashes[i]);
	if (m_rLength < FUZZ_GEN)
		rc = E_BadInput;
	else
		m_rFrequency = 1 / m_rLength;
	}
/*****************************************************************/
// Reset for a new instance
void CDash::ResetForNewInstance(
	double rStart)	// In: Lower limit of the first slice
	{
	// Set the data members
	m_rStart = floor(rStart * m_rFrequency);
	m_rStart *= m_rLength;
	m_rEnd = m_rStart + fabs(m_pDashes[0]);
	m_nTotal = m_nCurrent = 0;
	if (m_pDashes[m_nCurrent] < 0)
		GetNextDash();
	}
/*****************************************************************/
// Move to the next dash
RC CDash::GetNextDash()
	{
	RC rc = SUCCESS;

	do{
		m_rStart = m_rEnd;
		if (m_nCurrent == m_nLast)
			{
			m_nCurrent = 0;
			m_nTotal++;
			// Guard against an infinite loop
			if (m_nTotal > MAX_DASHES)
				{
				rc = E_TooManyKnots;
				QUIT
				}
			}
		else
			m_nCurrent++;
		m_rEnd = m_rStart + fabs(m_pDashes[m_nCurrent]);
		}
	while (m_pDashes[m_nCurrent] < 0);
exit:
	RETURN_RC(CDash::GetNext, rc);
	}
/*****************************************************************/
// Generate the segments of a single slice
RC CDash::DoOneSlice(
	const C3Point& V,				// Line direction vector
	CInstance& cInstance,			// In: Line instance
	gr_DisplaySurface* pSurface,	// In/out: Display surface
	BOOL& bDone)					// =TRUE if last slice done
	{
	RC rc = SUCCESS;
	while(!bDone)
		{
		double a = MAX(cInstance.SliceStart(), DashStart());
		double b = DashEnd();
		if (b >= cInstance.SliceEnd())
			{
			b = cInstance.SliceEnd();
			bDone = cInstance.NextSlice();
			}
		else
			if (rc = GetNextDash())
				QUIT

		if (b >= a)
			if (rc = cInstance.AddSegment(V, pSurface, a, b))
				QUIT
		} // End while
exit:
	RETURN_RC(CDash::DoOneSlice, rc);
	}
//////////////////////////////////////////////////////////////////
//																//
//					Implementation of CSolid					//
//																//
// Generate the segments of a single slice
RC CSolid::DoOneSlice(
	const C3Point& V,				// Line direction vector
	CInstance& cInstance,			// In: Line instance
	gr_DisplaySurface* m_pSurface,	// Display surface
	BOOL& bDone)					// =TRUE if last slice done
	{
	RC rc = cInstance.AddSegment(V, m_pSurface, cInstance.SliceStart(),
										        cInstance.SliceEnd());
	bDone = cInstance.NextSlice();
	RETURN_RC(CDash::DoOneSlice, rc);
	}

//////////////////////////////////////////////////////////////////
//																//
//					Implementation of CInstance					//
//																//
// Default constructor
CInstance::CInstance()
	{
	m_rPV = m_rPN = 0;
	m_nCurrent = 0;
	}
/*****************************************************************/
// Move to the next instance
void CInstance::Set(
	C3Point P,			// In: Instance base point
	const C3Point& V,	// In: Line's direction vector
	const C3Point& N)	// In: Line's normal
	{
	m_P = P;
	m_rPV = P * V;
	m_rPN = P * N;
	}
/*****************************************************************/
// Intersect this instance with a given polygon edge
RC CInstance::Intersect(
	const C3Point& V,	// In: Line's direction vector
	const C3Point& A,	// In: Polygon point
	double a,			// In: The dot product A*N
	const C3Point& B,	// In: Other Polygon point
	double b,			// In: The dot product B*N
	double rFuzz)		// In: Intersection fuzz
	{

	RC rc = SUCCESS;

/*
  The signed distance of a point Q from the line is d = (Q-P)*N,
  where P is a point on the line.  Since we have cached P*N as m_rPN
  for the line's base point P, we have d = Q*N - m_rPN.  The segment
  AB intersects the line if the distances of its its endpoints have
  different signs.  If one of the vertices lies exacly on the line.
  we want to mark an intersection, if the polygon crosses the line,
  and we want to mark it only once.  The safest way to do it is to
  slightly nudge every vertex that lies on the line slightly "downward".
  If no other vertex is truly above the line then let there be no
  intersection.  Othewise there may be one or two intersections near
  that vertex, depending on what its neighboring vertices are doing.
 */
  	a -= m_rPN;
	if (a == 0)
		a -= rFuzz;
	b -= m_rPN;
	if (b == 0)
		b -= rFuzz;

	if (a * b > 0)
		{
		// TRACE("No intersection; a = %20.15lf b = %20.15lf and a*b = %20.15lf\n",a,b,a*b);
		// There is no intersection
		goto exit;
		}


	b = fabs(b);
	a = fabs(a);

	// Find the intersection, using triangle similarity
	a = ((b * A + a * B) * V) / (a + b);
	a -= m_rPV;

	// Insert it in the list
	// TRACE("Intersection found at a = %20.15lf\n",a);
	rc = Insert(a);
exit:
	RETURN_RC(CInstance::Intersect, rc);
	}
/*****************************************************************/
// Clip away the intersection of this instance with a block
RC CInstance::ClipAwayBlock(
	const C3Point& V,	// In: Line's direction vector
	const C3Point* P,	// In: Polygon 5 vertices (last=first)
	double* r,			// In: r[i] = P[i]*N
	double rFuzz)		// In: Intersection fuzz
	{
	RC rc = SUCCESS;
	int i, nFrom, nTo;
	double a, b, s, t;
	double rFrom = DBL_MAX;
	double rTo = -DBL_MAX;

	if (Size() < 2) // There is nothing to clip
		goto exit;

	// Intersect the with block's edges to find the clipping interval
	a = r[0] - m_rPN;
	if (a == 0)
		a -= rFuzz;
	for (i = 1;   i <= 4;  i++)
		{
		b = r[i] - m_rPN;
		if (b == 0)
			b -= rFuzz;

		if (a * b < 0)
			{
			// There is an intersection
			s = fabs(b);
			t = fabs(a);

			// Find the intersection, using triangle similarity
			t = ((s * P[i-1] + t * P[i]) * V) / (s + t);
			t -= m_rPV;

			// Update the clipping interval on the line
			if (t < rFrom)
				rFrom = t;
			if (t > rTo)
				rTo = t;
			}

		// Move on
		a = b;
		}

	if (rFrom >= rTo)	// No interval to clip away
		goto exit;

	// Update the slices necessary
	nFrom = Search(rFrom);
	nTo = Search(rTo);

	i = nTo + 1;
	if (EVEN(nFrom))
		{
		if (rc = CRealArray::Insert(rFrom, ++nFrom))
			QUIT
		i++;
		}

	if (EVEN(nTo))
		if (rc = CRealArray::Insert(rTo, i))
			QUIT

	// Remove all intermediate slices
	nFrom++;
	i = i - nFrom;
	if (i > 0)
		Remove(nFrom, i);
exit:
	RETURN_RC(CInstance::ClipAwayBlock, rc);
	}
/*****************************************************************/
// Insert a new intersection
RC CInstance::Insert(
	double t)	// In: An intersection parameter to insert
	{
	RC rc;

	if (Size() < 1)
		rc = Add(t);
	else
		{
		int n;
		n = Search(t);
		rc = CRealArray::Insert(t, ++n);
		}

	RETURN_RC(CInstance::Insert, rc);
	}

/*****************************************************************/
// Add a line segment to the display surface
RC CInstance::AddSegment
(
const C3Point&		V,			// Line direction vector
gr_DisplaySurface*	pSurface,	// Display surface
double				rStart,		// In: Segment start
double				rEnd		// In: Segment end
)
{
	RC	rc = SUCCESS;

	/*DG - 17.1.2002*/// Added xclipping for g_bAbnormalPathEdgesChaining cases.
	// TO DO: move it to a more higher level (eg, to CHatchDisplay::DoOneLine) for optimization.
	if(g_bAbnormalPathEdgesChaining && pSurface->m_pGrView->m_Clipper.IsNotEmpty())
	{
		LinesList	Lines;
		sds_point	ncsPt1 = {m_P.X() + rStart * V.X(), m_P.Y() + rStart * V.Y(), 0.0},
					ncsPt2 = {m_P.X() + rEnd * V.X(), m_P.Y() + rEnd * V.Y(), 0.0},
					ucsPt1, ucsPt2;

		gr_ncs2ucs(ncsPt1, ucsPt1, 0, pSurface->m_pGrView);
		gr_ncs2ucs(ncsPt2, ucsPt2, 0, pSurface->m_pGrView);

		Lines.push_back(CLine(C3Point(ucsPt1[0], ucsPt1[1], ucsPt1[2]),
							  C3Point(ucsPt2[0], ucsPt2[1], ucsPt2[2])));

		if(!pSurface->m_pGrView->m_Clipper.ClipLines(Lines))
			return FAILURE;

		C3Point	geoPoint1, geoPoint2;
		for(LinesList::iterator pLine = Lines.begin(); pLine != Lines.end(); ++pLine)
		{
			pLine->GetEnds(geoPoint1, geoPoint2);
			if(rc = pSurface->AddSegment(geoPoint1.X(), geoPoint1.Y(), geoPoint2.X(), geoPoint2.Y()))
				return FAILURE;
		}
	}
	else
	{
		if(rEnd != rStart)
			rc = pSurface->AddSegment(m_P.X() + rStart * V.X(), m_P.Y() + rStart * V.Y(),
									  m_P.X() + rEnd * V.X(), m_P.Y() + rEnd * V.Y());
		else
			rc = pSurface->AddPoint(m_P.X() + rStart * V.X(), m_P.Y() + rStart * V.Y());
	}

	RETURN_RC(CInstance::AddSegment, rc);
}

//////////////////////////////////////////////////////////////////
//																//
//		Construction of Geo curves from hatch edges				//
//																//
//////////////////////////////////////////////////////////////////
// Construct a Geo line segment from line edge
LOCAL RC LineFromEdge(
	db_hatch* pHatch,	// I: Hatch entity's DB handle
    int		iPath,		// I: The path index
    int		iEdge,		// I: The edge index
	PCurve&	pCurve)		// O: The Geo line
	{
	double xStart, yStart, xEnd, yEnd;

    // Get the data
	pHatch->get_pathedgeline(iPath, iEdge, &xStart, &yStart, &xEnd, &yEnd);

	// Construct the line
	RC rc = ALLOC_FAIL(pCurve = new CLine(C3Point(xStart, yStart),
										   C3Point(xEnd, yEnd)));

	RETURN_RC(LineFromEdge, rc);
	}
/*****************************************************************/
// Construct a Geo arc from a circular arc edge
LOCAL RC ArcFromEdge(
	db_hatch* pHatch,	// I: Hatch entity's DB handle
    int			iPath,	// I: The path index
    int			iEdge,	// I: The edge index
	PCurve&	pCurve)		// O: The Geo arc
	{
	RC rc = SUCCESS;
	double x, y, r, a, b;
	int iCCW;

	// Get the data
	pHatch->get_pathedgearc(iPath, iEdge, &x, &y, &r, &a, &b, &iCCW);
	C3Point ptCenter(x, y);
	C3Point ptMajor(r,0);
	C3Point ptMinor(0,r);

	// Flip if clockwise
	if (!iCCW)
		ptMinor = C3Point(0, -r);

	// Make sure b > a
	while (b <= a)
		b += TWOPI;

	// Construct the arc
	rc = ALLOC_FAIL(pCurve = new CEllipArc(ptCenter, ptMajor, ptMinor, a, b));

	RETURN_RC(ArcFromEdge, rc);
	}
/*****************************************************************/
// Construct a Geo arc from an elliptical arc edge
LOCAL RC EllipFromEdge(
	db_hatch* pHatch,	// I: Hatch entity's DB handle
    int			iPath,	// I: The path index
    int			iEdge,	// I: The edge index
	PCurve&	pCurve)		// O: The Geo arc
	{
	RC rc = SUCCESS;
	double xCenter, yCenter, x, y, rMinor, a, b;
	int iCCW;
	PEllipArc pArc = NULL;
	pCurve = NULL;

	// Get the data
	pHatch->get_pathedgeellarc(iPath, iEdge, &xCenter,&yCenter,
								&x, &y, &rMinor, &a, &b, &iCCW);
	C3Point ptCenter(xCenter, yCenter);
	C3Point ptMajor(x, y);

	// Get the minor axis vector
	C3Point ptMinor = ptMajor;
	ptMinor.TurnLeft();
	ptMinor *= rMinor;

	// Make sure ths start and end are in the right order
	if (!iCCW)
		ptMinor = -ptMinor;
	while (b <= a)
		b += TWOPI;

	// Construct the ellipse
	//
	// The following changes introduced problem reported in the bug #70921 so 
	// I commented out those lines - SWH
	//
//	if(fabs(fmod(a, PIOVER2)) > IC_ZRO)			/*D.G.*/// Convert from 'angles' to 'parameters' for Geo
//		a = atan2(sin(a), cos(a) * rMinor);		// (50 & 51 groups for elliptical arcs in hatch boundaries
//	if(fabs(fmod(b, PIOVER2)) > IC_ZRO)			// must be stored as 'angles' in the drawing DB). Convert 
//		b = atan2(sin(b), cos(b) * rMinor);		// only those values which are not proportional to PI/2.
	ic_normang(&a, &b);							
	if (rc = ALLOC_FAIL(pArc = new CEllipArc(ptCenter, ptMajor, ptMinor, a, b)))
		QUIT
	pCurve = pArc;
	pArc = NULL;
exit:
	delete pArc;
	RETURN_RC(EllipFromEdge, rc);
	}
/*****************************************************************/
// Construct a Geo spline from a spline edge
LOCAL RC SplineFromEdge(
	db_hatch* pHatch,	// I: Hatch entity's DB handle
    int		 iPath,		// I: The path index
    int		 iEdge,		// I: The edge index
	PCurve& pCurve)	// O: The Geo spline
	{
	RC rc = SUCCESS;
	int i;
	int nDegree, n, k, bPeriodic, bRational;
	double* pKts = NULL;
	PKnots pKnots = NULL;
	PControlPoints pControlPoints = NULL;
	PRealArray pWeights = NULL;

    // Get the data
	pHatch->get_pathedgespline(iPath, iEdge, &bRational, &bPeriodic,
								&nDegree, &k, &n, &pKts);
	if (n > k)
		n = k;

	// Allocate the lists of knots, control points & weights
	pKnots = new CKnots(nDegree, n, rc);
	CHECK_CONSTRUCTION(pKnots, rc)
	if (rc)
		QUIT;
	pControlPoints = new CControlPoints(n, rc);
	CHECK_CONSTRUCTION(pControlPoints, rc)
	if (rc)
		QUIT;
	if (bRational)
		{
		pWeights = new CRealArray(n, rc);
		CHECK_CONSTRUCTION(pWeights, rc)
		if (rc)
			QUIT;
		}

	// Load the data
	for (i = 0;   i < n;   i++)
		{
		double x, y;
		pKnots->Set(i, pKts[i]);
		pHatch->get_pathedgesplinecontrol(iPath, iEdge, i, &x, &y);
		if (bRational)
			{
			double w;
			pHatch->get_pathedgesplineweight(iPath, iEdge, i, &w);
			x *= w;
			y *= w;
			(*pWeights)[i] = w;
			}
		pControlPoints->Set(i, C3Point(x,y));
		}

	// Wrap up the knots
	pKnots->SetInterval(pKts[n] - pKts[0]);

	if (bRational)
		pCurve = new CNURBS(pKnots, pControlPoints, pWeights, rc);
	else
		pCurve = new CSpline(pKnots, pControlPoints, rc);
	CHECK_CONSTRUCTION(pCurve, rc)
exit:
	RETURN_RC(SplineFromEdge, rc);
	}
/*****************************************************************/
// Construct a Geo curve from a hatch edge
LOCAL RC CurveFromEdge(
	db_hatch* pHatch,	// I: Hatch entity's DB handle
    int		 iPath,		// I: The path index
    int		 iEdge,		// I: The edge index
	PCurve& pCurve)		// O: The Geo curve
	{
	RC rc;

	switch(pHatch->ret_pathedgetype(iPath, iEdge))
		{
		case 1:
			rc = LineFromEdge(pHatch, iPath, iEdge, pCurve);
			break;

		case 2:
			rc = ArcFromEdge(pHatch, iPath, iEdge, pCurve);
			break;

		case 3:
			rc = EllipFromEdge(pHatch, iPath, iEdge, pCurve);
			break;

		case 4:
			rc = SplineFromEdge(pHatch, iPath, iEdge, pCurve);
			break;

		default:
			rc = E_BadInput;
			break;
		}

	RETURN_RC(CurveFromEdge, rc);
	}
/*****************************************************************/
// Construct a Geo curve from a bulged polyline edge
LOCAL RC CurveFromBulged(
	double xStart,	// I: Start point x
	double yStart,	// I: Start point y
	double xEnd,	// I: End point x
	double yEnd,	// I: End point y
	double rBulge,	// I: Bulge
	PCurve& pCurve)	// O: The Geo curve
	{
	RC rc;
	C3Point A(xStart, yStart);
	C3Point B(xEnd, yEnd);

	// Try to construct an arc
	pCurve = new CEllipArc(A, B, rBulge, rc);
	if (!pCurve)
		{
		rc = DM_MEM;
		QUIT
		}

	if (rc)
		{
		// Arc construction failed, try a line segment
		delete pCurve;
		rc = ALLOC_FAIL(pCurve = new CLine(A, B));
		}
exit:
	RETURN_RC(CurveFromBulged, rc);
	}


//////////////////////////////////////////////////////////////////
//																//
//				Implementation of CHatchDisplay					//
//																//
//////////////////////////////////////////////////////////////////

// Default constructor
CHatchDisplay::CHatchDisplay() : m_pHatch(0), m_pSurface(0), m_pLineStyle(0), m_pInstances(0), m_nFirst(0), m_nLast(0),
								 m_rFrequency(1.0), m_rFuzz(FUZZ_DIST), m_rBaseN(0.0), m_bSolidFill(false)
{
	g_bAbnormalPathEdgesChaining = false;
}

// Constructor
CHatchDisplay::CHatchDisplay
(
 db_hatch*			pHatch,		// I: Hatch entity's DB handle
 gr_DisplaySurface*	pSurface	// I/O: The display surface
 ) : m_pHatch(pHatch), m_pSurface(pSurface), m_pLineStyle(0), m_pInstances(0), m_nFirst(0), m_nLast(0),
	 m_rFrequency(1.0), m_rFuzz(MAX(pSurface->Resolution() / 10, FUZZ_DIST)), m_rBaseN(0.0), m_bSolidFill(false)
{
	g_bAbnormalPathEdgesChaining = false;
}

// Construct polygonal approximation for this path
RC CHatchDisplay::LinearizePath
(
 int	i	// In: The path index
)
{
	RC			rc = SUCCESS;
	PCurve		pCurve = NULL;
	int			j, nFlags, nEdges, n;
	CPolyCache*	pCache = NULL;

	// Get the number of edges in this path
	m_pHatch->get_path(i, &nFlags, &nEdges, &n);

	// Get the hatch style and check if this path if relevant
	m_pHatch->get_75(&n);
	if(nFlags & PATH_TEXTBLOCK)
		pCache = &m_cTextBlocks;
	else
		pCache = &m_cPolys;

	if(n & STYLE_OUTERMOST && !(nFlags & PATH_EXTERNAL) && !(nFlags & PATH_OUTERMOST))
		QUIT

	if(n & STYLE_IGNORE && !(nFlags & PATH_EXTERNAL))
		QUIT

	if(nFlags & PATH_POLYLINE)	// It's a polyline
	{
		// Start a new polyon for this path
		if(rc = pCache->StartNewPoly())
			QUIT

		double	x, y, b, xNext, yNext, bNext;
		int		bClosed;

		// Get initial data
		m_pHatch->get_pathedgepline(i, &bClosed, &nFlags, &nEdges);
		m_pHatch->get_pathedgeplinevert(i, 0, &x, &y, &b);
		double xFirst = x, yFirst = y, bFirst = b;

		// Enter the first point
		if(rc = AddVertex(C3Point(x,y), pCache))
			QUIT
		if(nFlags)
		{
			// It's a bulged polyline, we need to construct the edges
			for(j = 1;  j < nEdges;  ++j)
			{
				m_pHatch->get_pathedgeplinevert(i, j, &xNext, &yNext, &bNext);
				if(rc = CurveFromBulged(x, y, xNext, yNext, b, pCurve))
					QUIT
				if(rc = AddCurve(pCurve, pCache))
					QUIT
				x = xNext;
				y = yNext;
				b = bNext;
			}
			if(bClosed)
			{
				if(rc = CurveFromBulged(x, y, xFirst, yFirst, b, pCurve))
					QUIT
				if(rc = AddCurve(pCurve, pCache))
					QUIT
			}
		} // End if bulged
		else
		{
			// It's all straight segments, just store the vertices
			for(j = 1;  j < nEdges;  ++j)
			{
				m_pHatch->get_pathedgeplinevert(i, j, &x, &y, NULL);
				if (rc = AddVertex(C3Point(x,y), pCache))
					QUIT
			}
			if(bClosed)
			{
				if(rc = AddVertex(C3Point(xFirst,yFirst), pCache))
					QUIT
			}
		} // End else - straight segments polyline
	} // End if polyline
	else
	{
		if(rc = pCache->StartNewPoly())
			QUIT

		for(j = 0;  j < nEdges;  ++j)
		{
			if(rc = CurveFromEdge(m_pHatch, i, j, pCurve))
				QUIT

			if(rc = AddCurve(pCurve, pCache))
				QUIT
		} // End for j
	} // End else

	pCache->MakeClosed(FUZZ_DIST);

exit:
	delete pCurve;
	RETURN_RC(CHatchDisplay::LinearizePath, rc);
}

/*****************************************************************/
// Process an edge curve
RC CHatchDisplay::AddCurve
(
 PCurve&		pCurve,		// I/O: A new curve, deleted and nulled here
 CPolyCache*	pCache)		// In: The cache to add it to
{
	RC	rc;

	/*DG - 17.1.2002*/// Our slogan is "one path - one polygon in cache".
	// But some hatch boundary paths have gaps or non-chained edges. So we linearize the current edge to
	// a temp cache and then try to add points from this cache to the resulting *pCache in correct order.
	// In failure we create a new poly starting from an unruly edge and set g_bAbnormalPathEdgesChaining to 'true'.

	if(!pCache->Started())
	{
		if(rc = pCurve->Linearize(m_pSurface->Resolution(), NULL, *pCache))
			QUIT
	}
	else
	{
		CPolyCache	tmpCache;

		tmpCache.StartNewPoly();
		if(rc = pCurve->Linearize(m_pSurface->Resolution(), NULL, tmpCache))
			QUIT

		CPointArray	&oldPoly = *((PPointArray)pCache->Last()),
					&newPoly = *(tmpCache.Poly(0));
		C3Point		&oldPolyPt1 = oldPoly[0], &oldPolyPt2 = oldPoly.Last(),
					&newPolyPt1 = newPoly[0], &newPolyPt2 = newPoly.Last();
		double		dist11 = PointDistance(oldPolyPt1, newPolyPt1),
					dist12 = PointDistance(oldPolyPt1, newPolyPt2),
					dist21 = PointDistance(oldPolyPt2, newPolyPt1),
					dist22 = PointDistance(oldPolyPt2, newPolyPt2);

		if(dist11 < FUZZ_DIST || dist12 < FUZZ_DIST || dist21 < FUZZ_DIST || dist22 < FUZZ_DIST)
		{
			if(dist11 < FUZZ_DIST || dist12 < FUZZ_DIST)
				oldPoly.ReverseOrder();

			if(dist22 < FUZZ_DIST || dist12 < FUZZ_DIST)
				newPoly.ReverseOrder();

			oldPoly.Copy(newPoly, 1, newPoly.Size()-1);
		}
		else
		{
			pCache->StartNewPoly();
			((PPointArray)pCache->Last())->Copy(newPoly, 0, newPoly.Size());
			g_bAbnormalPathEdgesChaining = true;
		}
	}

	rc = pCurve->UpdateExtents(FALSE, FALSE, m_cExtents, NULL);

exit:
	delete pCurve;
	pCurve = NULL;
	RETURN_RC(CHatchDisplay::AddCurve, rc);
}

#ifdef DEBUG_BOUNDARIES
/*****************************************************************/
// Display the boundary entities for debug purposes
void CHatchDisplay::Test()
	{
	RC rc = SUCCESS;
	// Temporary: show the linearization of the edges
	TRACE("*******************************************************\n");
	for (int i = 0;    i < m_cPolys.Size();   i++)
		{
		PPointArray pPoly = m_cPolys.Poly(i);
		C3Point P = (*pPoly)[0];
		TRACE("=======================================================\n");
		TRACE("i = %d\n",i);
		for (int j = 1;    j < pPoly->Size();   j++)
			{
			TRACE("j = %d\n",j);
			C3Point Q = (*pPoly)[j];
			m_pSurface->AddSegment(P.X(), P.Y(), Q.X(), Q.Y());
			TRACE("P = ( %20.15lf , %20.15lf )\n",P.X(),P.Y());
			TRACE("Q = ( %20.15lf , %20.15lf )\n",Q.X(),Q.Y());
			P = Q;
			} // End for j
		}  // End for i
	}
#endif
/*****************************************************************/
// Reset for a new line
RC CHatchDisplay::Reset()
	{
	RC rc = SUCCESS;

	// Set the relevant points and vectors
	m_N = m_V;
	m_N.TurnLeft();

/* The instances of the line are Q(t)=Pk+t*V, where Pk=Po+k*Offset,
 * where Po is the base point. So, for a point Q on an instance,
 * Q=Q(t)=Po+k*Offset+t*V for some k and some t.  Dot it with the
 * normal to the line N, we get Q*N=Po*N+k*Offset*N, because V*N=0,
 * so k = (Q*N - Po*N) / (Offset*N).  If we chose N so that Offset*N>0
 * then the MIN and MAX of k are attained at the MIN and MAX of Q,
 * Respectively, which are available from CExtents::GetMinMax.
 *
*/

	// Get the range and frequency of the instances
	m_rFrequency = m_vecOffset * m_N;
	if (m_rFrequency < 0)
		{
		m_rFrequency = -m_rFrequency;
		m_N = -m_N;
		}
	double rMin, rMax;
	m_rBaseN = m_ptBase * m_N;
	m_cExtents.GetMinMax(m_N, rMin, rMax);

	// Guard against an infinite loop on instances
	if (m_rFrequency * MAX_HATCH_LINES < (rMax - rMin))
		m_rFrequency = (rMax - rMin) / MAX_HATCH_LINES;
	else
		m_rFrequency = 1 / m_rFrequency;

	// The base point of the first and last instance
	m_nFirst = (int)ceil((rMin - m_rBaseN) * m_rFrequency);
	m_nLast = (int)floor((rMax - m_rBaseN) * m_rFrequency);
	if (m_nFirst > m_nLast)
		// This line is not going to show up
		rc = E_NOMSG;

	RETURN_RC(CHatchDisplay::Reset, rc);
	}

/*D.G.*/// Free memory.
void
CHatchDisplay::Clear()
{
	m_cPolys.Clear();
	m_cTextBlocks.Clear();
	m_cExtents.Reset();
}
/*****************************************************************/
// Locate a point among the hatch line instances
void CHatchDisplay::Locate(
	const C3Point& P,	// In: The point
	double& rPN,		// Out: The value of P*N
	int& n)				// Out: The index of the nearby instance
	{
	rPN = P.X() * m_N.X() + P.Y() * m_N.Y();
	n = (int)floor((rPN - m_rBaseN) * m_rFrequency);
	if (n < m_nFirst)
		n = m_nFirst;
	if (n > m_nLast)
		n = m_nLast;
	n -= m_nFirst;
	}
/*****************************************************************/
// Find all the boundary intersections of all instances
RC CHatchDisplay::Slice()
	{
	RC rc = SUCCESS;
	int i, j, k;
	C3Point A, B;
	double a, b;
	int nA, nB, nFrom, nTo;

	/*DG 3.10.2001*/// Use hatch style stored in the hatch itself
	// instead of sysvar HPSTYLE. And DON'T use sds in gr!
	int	hatchStyle;
	m_pHatch->get_75(&hatchStyle); /*

	//Add by SMR for preventing crash. 2001/8/14
	resbuf rb;
	int Land=0;
	if(sds_getvar("HPSTYLE",&rb) == RTNORM)
	{
		 Land = rb.resval.rint;
	}
	//End of add. SMR 2001/8/14    */

	for (i = 0;   i < m_cPolys.Size();   i++)
		{

		/*DG 3.10.2001*/// See comments just above.
		if(hatchStyle == STYLE_IGNORE && i)
			break;                 /*
		//Add by SMR for preventing crash. 2001/8/14
		if (Land == 2)
		{
			if (i >= 1) break;
		}
		//End of add. SMR 2001/8/14 */

		PPointArray pPoly = m_cPolys.Poly(i);
		//Modified Cybage AW 29-10-01 [
		//Reason : BugZilla Bug no : 77903
		//Bhatch crash when we select "ignore islands" option
		if(pPoly->Size() <= 0)
			continue;
		//Modified Cybage AW 29-10-01 ]
		A = (*pPoly)[0];
		Locate(A, a, nA);
		for (j = 1;   j < pPoly->Size();   j++)
			{
			B = (*pPoly)[j];
			Locate(B, b, nB);
			if (nA <= nB)
				{
				nFrom = nA;
				nTo = nB;
				}
			else
				{
				nFrom = nB;
				nTo = nA;
				}

			// Loop on the instances that intersect this edge
			for (k = nFrom;   k <= nTo;   k++)
				{
				// TRACE("calling intersect with i = %d j = %d and k = %d\n",i,j,k);
				if (rc = m_pInstances[k].Intersect(m_V, A, a, B, b, m_rFuzz))
					QUIT
				}

			// Next point
			A = B;
			a = b;
			nA = nB;
			} // End for j
		} // End for i

	// Make the number of intersections even
	for (i = m_nFirst;   i <= m_nLast;   i++)
		{
		j = i - m_nFirst;
		nA = m_pInstances[j].Size();
		if (ODD(nA))
			// Odd number of intersections, remove the middle one
			m_pInstances[j].Remove(nA / 2);
		} // End for
exit:
	RETURN_RC(CHatchDisplay::Slice, rc);
	}
/*****************************************************************/
// Clip away the pieces that are covered by text blocks
RC CHatchDisplay::ClipAwayBlocks()
	{
	RC rc = SUCCESS;
	int i, j, k;

	for (i = 0;   i < m_cTextBlocks.Size();   i++)
		{
		C3Point P[5];
		double r[5];
		int n;
		int nFrom = INT_MAX;
		int nTo = -INT_MAX;

		PPointArray pPoly = m_cTextBlocks.Poly(i);
		if (pPoly->Size() != 5)
			// It should be a rectangle, something's wrong
			continue;

		for (j = 0;   j < 5;   j++)
			{
			P[j] = (*pPoly)[j];
			Locate(P[j], r[j], n);

			if (n < nFrom)
				nFrom = n;
			if (n > nTo)
				nTo = n;
			} // End for j

			// Loop on the instances that intersect this edge
			for (k = nFrom;   k <= nTo;   k++)
				if (rc = m_pInstances[k].ClipAwayBlock(m_V, P, r, m_rFuzz))
					QUIT
		} // End for i

exit:
	RETURN_RC(CHatchDisplay::ClipAwayBlocks, rc);
	}

/*****************************************************************/
// Generate all the hatch lines from one pattern line
RC CHatchDisplay::DoOneLine()
{
	RC		rc = SUCCESS;
	BOOL	bDone = FALSE;
	int		i, j;

	if(Reset())	// Eat the error
		QUIT

	j = m_nLast - m_nFirst + 1;

	// Allocate the instances and slice them with the boundaries
	if(rc = (ALLOC_FAIL(m_pInstances = new CInstance[j])))
		QUIT

	for(i = m_nFirst; i <= m_nLast;   ++i)
		m_pInstances[i-m_nFirst].Set(m_ptBase + i * m_vecOffset, m_V, m_N);

	if(rc = Slice())
		QUIT

	// Clip away the intersection with text blocks
	if(rc = ClipAwayBlocks())
		QUIT

	// Loop on instances
	for(i = m_nFirst; i <= m_nLast; ++i)
	{
		j = i - m_nFirst;
		// Loop on the slices
		BOOL	bDone = m_pInstances[j].Size() <= 0;
		if(!bDone)
			m_pLineStyle->ResetForNewInstance(m_pInstances[j].SliceStart());

		while(!bDone)
			if(rc = m_pLineStyle->DoOneSlice(m_V, m_pInstances[j], m_pSurface, bDone))
				QUIT
	} // End for i

exit:
	delete [] m_pInstances;
	m_pInstances = NULL;
	RETURN_RC(CHatchDisplay::DoOneLine, rc);
}

/*****************************************************************/
// Generate the hatch lines
RC CHatchDisplay::Generate()
{
	RC		rc = SUCCESS;
	int		nLineCount, i, n;
	double	xBase, yBase, xOffset, yOffset, rAngle, *p;

	if(!g_bAbnormalPathEdgesChaining)
		xclipBoundary();

	// Get the number of pattern lines and scale
	m_pHatch->get_78(&nLineCount);

	// Loop on the pattern lines
	for(i = 0;  i < nLineCount;  ++i)
	{
		// Get the line data
		m_pHatch->get_patline(i, &rAngle, &xBase, &yBase, &xOffset, &yOffset, &n, &p);
		m_ptBase = C3Point(xBase, yBase);
		m_vecOffset = C3Point(xOffset, yOffset);
		m_V = C3Point(cos(rAngle), sin(rAngle));

		// Process this line
		if(n <= 0)
			m_pLineStyle = new CSolid;
		else
			m_pLineStyle = new CDash(n, p, rc);
		if(!m_pLineStyle)
			rc = DM_MEM;
		if(rc)
			QUIT

		if(rc = DoOneLine())
			QUIT

		// Do the cross hatch, if applicable
		m_pHatch->get_77(&n);
		if(n)
		{
			// Turn left and do it again
			m_vecOffset.TurnLeft();
			m_V.TurnLeft();
			if(rc = DoOneLine())
				QUIT
		} // End if cross hatch

		delete m_pLineStyle;
		m_pLineStyle = NULL;
	} // End for

exit:
	delete m_pLineStyle;
	RETURN_RC(CHatchDisplay::Generate, rc);
}

/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Clip hatching's linearized paths.
 * Returns:	'true' in success and 'false' in failure.
 ********************************************************************************/
bool
CHatchDisplay::xclipBoundary()
{
	if(!m_pSurface->m_pGrView->m_Clipper.IsNotEmpty())
		return true;

	int				i, j;
	PPolyLine		pPoints;
	PPolylinesList	PPolylines;
	sds_point		pt1, pt2;
	PPolylinesList::iterator pPPolyline;

	// clip ordinary boundaries (m_cPolys array)
	for(i = m_cPolys.Size(); i--; )
	{
		if(!(pPoints = new CPolyLine()) || pPoints->Copy(*(m_cPolys.Poly(i)), 0, (m_cPolys.Poly(i))->Size()) != SUCCESS)
			return false;

		// transform to UCS
		for(j = pPoints->Size(); j--; )
		{
			pt1[0] = (*pPoints)[j].X();
			pt1[1] = (*pPoints)[j].Y();
			pt1[2] = (*pPoints)[j].Z() + m_pSurface->m_elev;
			gr_ncs2ucs(pt1, pt2, 0, m_pSurface->m_pGrView);
			(*pPoints)[j].SetX(pt2[0]);
			(*pPoints)[j].SetY(pt2[1]);
			(*pPoints)[j].SetZ(pt2[2]);
		}

		PPolylines.push_back(pPoints);
	}

	if(!m_pSurface->m_pGrView->m_Clipper.ClipPolygons(PPolylines))
	{
		FreePointersList(PPolylines);
		return false;
	}

	m_cPolys.Clear();

	for(pPPolyline = PPolylines.begin(); pPPolyline != PPolylines.end(); pPPolyline++)
	{
		if(m_cPolys.Add((PPointArray)*pPPolyline) != SUCCESS)
		{
			FreePointersList(PPolylines);
			return false;
		}
	}

	PPolylines.clear();		// DON'T use FreePointersList: we've copied these pointers to m_cPolys.

	if(m_bSolidFill)
		m_pSurface->m_bAlreadyInUcs = true;
	else
		gr_setupixfp(m_pSurface->m_pGrView);	// set-up m_pGrView->m_transformationInverse matrix

	for(i = m_cPolys.Size(); i--; )
	{
		PPointArray	pPointArray;
		pPointArray = m_cPolys.Poly(i);
		for(j = pPointArray->Size(); j--; )
		{
			if(!m_bSolidFill)
			{
				pt1[0] = (*pPointArray)[j].X();
				pt1[1] = (*pPointArray)[j].Y();
				pt1[2] = (*pPointArray)[j].Z();
				gr_ucs2ncs(pt1, pt2, 0, m_pSurface->m_pGrView);
				(*pPointArray)[j].SetX(pt2[0]);
				(*pPointArray)[j].SetY(pt2[1]);
				(*pPointArray)[j].SetZ(pt2[2] - m_pSurface->m_elev);
			}
			m_cExtents.Update((*pPointArray)[j]);
		}
	}

	// clip text boxes(m_cTextBlocks array)
	if(!m_cTextBlocks.Size())
		return true;

	for(i = m_cTextBlocks.Size(); i--; )
	{
		if(!(pPoints = new CPolyLine()) || pPoints->Copy(*(m_cTextBlocks.Poly(i)), 0, (m_cTextBlocks.Poly(i))->Size()) != SUCCESS)
			return false;

		// transform to UCS
		for(j = pPoints->Size(); j--; )
		{
			pt1[0] = (*pPoints)[j].X();
			pt1[1] = (*pPoints)[j].Y();
			pt1[2] = (*pPoints)[j].Z() + m_pSurface->m_elev;
			gr_ncs2ucs(pt1, pt2, 0, m_pSurface->m_pGrView);
			(*pPoints)[j].SetX(pt2[0]);
			(*pPoints)[j].SetY(pt2[1]);
			(*pPoints)[j].SetZ(pt2[2]);
		}

		PPolylines.push_back(pPoints);
	}

	if(!m_pSurface->m_pGrView->m_Clipper.ClipPolygons(PPolylines))
	{
		FreePointersList(PPolylines);
		return false;
	}

	m_cTextBlocks.Clear();

	for(pPPolyline = PPolylines.begin(); pPPolyline != PPolylines.end(); pPPolyline++)
		if(m_cTextBlocks.Add((PPointArray)*pPPolyline) != SUCCESS)
		{
			FreePointersList(PPolylines);
			return false;
		}

	PPolylines.clear();		// DON'T use FreePointersList: we've copied these pointers to m_cTextBlocks.

	if(!m_bSolidFill)
	{	// transform back from UCS
		for(i = m_cTextBlocks.Size(); i--; )
		{
			PPointArray	pPointArray;
			pPointArray = m_cTextBlocks.Poly(i);
			for(j = pPointArray->Size(); j--; )
			{
				pt1[0] = (*pPointArray)[j].X();
				pt1[1] = (*pPointArray)[j].Y();
				pt1[2] = (*pPointArray)[j].Z();
				gr_ucs2ncs(pt1, pt2, 0, m_pSurface->m_pGrView);
				(*pPointArray)[j].SetX(pt2[0]);
				(*pPointArray)[j].SetY(pt2[1]);
				(*pPointArray)[j].SetZ(pt2[2] - m_pSurface->m_elev);
			}
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////
//																//
//				Implementation of CFillDisplay					//
//																//

/*D.Gavrilov*/// I've implemented true filling by forming dispobjs
// as closed paths of lines and using GDI Paths and filling them
// in EntityRenderer::DrawEntity.

#ifdef NEW_SOLID_GENERATE

RC CFillDisplay::Generate()
{
	RC			rc = SUCCESS;
	sds_point*	Verts = 0;

	if(g_bAbnormalPathEdgesChaining)
	{
		// Fake the fill with a very dense hatch pattern
		double	r = m_pSurface->Resolution() * 0.5;
		m_ptBase = C3Point(0.0, 0.0);
		m_V = C3Point(1.0, 0.0);
		m_vecOffset = C3Point(0.0, r);
		if(rc = ALLOC_FAIL(m_pLineStyle = new CSolid))
			QUIT

		rc = DoOneLine();
	}
	else
	{
		xclipBoundary();

		int			NumOfPolys = m_cPolys.Size(),
					MaxNumOfVerts, NumOfVerts, i, j;
		PPointArray	pPoly;
		C3Point		MidPoint;
		int			InsidenessDegree;

		for(MaxNumOfVerts = 0, i = NumOfPolys; i--; )
			if(MaxNumOfVerts < m_cPolys.Poly(i)->Size())
				MaxNumOfVerts = m_cPolys.Poly(i)->Size();

		Verts = new sds_point[MaxNumOfVerts];

		for(i = MaxNumOfVerts; i--; )
			Verts[i][2] = 0.0;

		for(i = NumOfPolys; i--; )
		{
			pPoly = m_cPolys.Poly(i);
			NumOfVerts = pPoly->Size();

			for(j = NumOfVerts; j--; )
			{
				Verts[j][0] = (*pPoly)[j].X();
				Verts[j][1] = (*pPoly)[j].Y();
			}

			if(m_pSurface->m_pGrView->projmode)
			{
				if(rc = m_pSurface->AddDisplayObj(Verts, NumOfVerts, 2, DISP_OBJ_PTS_GDI_FILLPATH, m_pHatch))
					QUIT;
			}
			else
			{
				if(rc = m_pSurface->AddDisplayObj(Verts, NumOfVerts, 3, DISP_OBJ_PTS_3D | DISP_OBJ_PTS_GDI_FILLPATH, m_pHatch))
					QUIT;
			}
		}

		// Clip away text boxes by inserting them into dispobjs list if needed.
		// Note, it can be not 'boxes' after x-clipping.
		delete [] Verts;

		NumOfPolys = m_cTextBlocks.Size();
		for(MaxNumOfVerts = 0, i = NumOfPolys; i--; )
			if(MaxNumOfVerts < m_cTextBlocks.Poly(i)->Size())
				MaxNumOfVerts = m_cTextBlocks.Poly(i)->Size();

		Verts = new sds_point[MaxNumOfVerts];

		for(i = MaxNumOfVerts; i--; )
			Verts[i][2] = 0.0;

		for(i = NumOfPolys; i--; )
		{
			pPoly = m_cTextBlocks.Poly(i);
			NumOfVerts = pPoly->Size();

			// find middle point of a text box
			MidPoint.SetX(0.0);
			MidPoint.SetY(0.0);
			MidPoint.SetZ(0.0);

			for(j = NumOfVerts; j--; )
				MidPoint += (*pPoly)[j];

			MidPoint /= NumOfVerts;

			// count how many times this box (i.e. its midpoint) is 'inside' of hatch's boundaries
			for(InsidenessDegree = 0, j = m_cPolys.Size(); j--; )
				InsidenessDegree += ((CPolyLine*)m_cPolys.Poly(j))->IsPointInside(MidPoint) ?
									1 : 0;

			if(ODD(InsidenessDegree))
			{
				for(j = NumOfVerts; j--; )
				{
					Verts[j][0] = (*pPoly)[j].X();
					Verts[j][1] = (*pPoly)[j].Y();
				}

				if(m_pSurface->m_pGrView->projmode)
				{
					if(rc = m_pSurface->AddDisplayObj(Verts, NumOfVerts, 2, DISP_OBJ_PTS_GDI_FILLPATH, m_pHatch))
						QUIT;
				}
				else
				{
					if(rc = m_pSurface->AddDisplayObj(Verts, NumOfVerts, 3, DISP_OBJ_PTS_3D | DISP_OBJ_PTS_GDI_FILLPATH, m_pHatch))
						QUIT;
				}
			}
		}
	}	// g_bAbnormalPathEdgesChaining == 'false'

exit:
	delete [] Verts;
	delete m_pLineStyle;
	RETURN_RC(CHatchDisplay::Generate, rc);
}

#else // NEW_SOLID_GENERATE

RC CFillDisplay::Generate()
{
	RC rc = SUCCESS;
	int NumOfPolys = m_cPolys.Size();
	int NumOfVerts_total, NumOfVerts_local, i, j, k, iVert;
	sds_point* Verts;
	PPointArray pPoly;
	sds_point MidPt;
	sds_point TxtBlkVerts[5];
	int* NumOfIntersections;

	for(NumOfVerts_total = i = 0; i < NumOfPolys; i++)
		NumOfVerts_total += m_cPolys.Poly(i)->Size();
	Verts = new sds_point[NumOfVerts_total];

	// fill Verts excluding repeating points inside each poly
	sds_point	IntersectionPoint;
	bool		bCoincidentEnds,
				bFirstSegmentInserted;
	for(iVert = i = 0; i < NumOfPolys; i++)
	{
		pPoly = m_cPolys.Poly(i);
		NumOfVerts_local = pPoly->Size();

		if( iVert                                                &&
			fabs(Verts[iVert-1][0] - (*pPoly)[0].X()) < FUZZ_GEN &&
			fabs(Verts[iVert-1][1] - (*pPoly)[0].Y()) < FUZZ_GEN )
			bCoincidentEnds = true;
		else
		{
			bCoincidentEnds = false;
			Verts[iVert][0] = (*pPoly)[0].X();
			Verts[iVert][1] = (*pPoly)[0].Y();
			Verts[iVert][2] = 0.0;
			iVert++;
		}

		for(bFirstSegmentInserted = false, j = 1; j < NumOfVerts_local; j++)
		{
			if(fabs(Verts[iVert-1][0] - (*pPoly)[j].X()) > FUZZ_GEN ||
			   fabs(Verts[iVert-1][1] - (*pPoly)[j].Y()) > FUZZ_GEN)
			{
				Verts[iVert][0] = (*pPoly)[j].X();
				Verts[iVert][1] = (*pPoly)[j].Y();
				Verts[iVert][2] = 0.0;
				/*D.G.*/// If the last segment of the previous polyline intersects with the 1st one
				// of this polyline but their appropriate ending and starting vertices aren't coincident
				// then replace appropriate vertices with the intersection point.
				if(!bCoincidentEnds && !bFirstSegmentInserted)
				{
					if(iVert > 2 && ic_linexline(Verts[iVert-3], Verts[iVert-2], Verts[iVert-1], Verts[iVert], IntersectionPoint) > 0)
					{
						GR_PTCPY(Verts[iVert-2], IntersectionPoint);
						GR_PTCPY(Verts[iVert-1], IntersectionPoint);
					}
				}
				iVert++;
				bFirstSegmentInserted = true;
			}
		}
	}

	/*D.G.*/// If the last segment of the last polyline intersects with the 1st one
	// of the 1st polyline then replace appropriate vertices with the intersection point.
	if(iVert > 3 && ic_linexline(Verts[0], Verts[1], Verts[iVert-2], Verts[iVert-1], IntersectionPoint) > 0)
	{
		GR_PTCPY(Verts[0], IntersectionPoint);
		GR_PTCPY(Verts[iVert-1], IntersectionPoint);
	}

	NumOfVerts_total = iVert;

	// create dispobjs
	NumOfPolys = m_cTextBlocks.Size();
	NumOfIntersections = new int[NumOfPolys];
	for(j = NumOfPolys; j--; )
		NumOfIntersections[j] = 0;

	for(iVert = 0, NumOfVerts_local = i = 2; i < NumOfVerts_total; i++, NumOfVerts_local++)
	{	// NOTE: NumOfVerts_local and i may be changed in the body of the loop
		if(fabs(Verts[i][0] - Verts[iVert][0]) < FUZZ_GEN &&
		   fabs(Verts[i][1] - Verts[iVert][1]) < FUZZ_GEN)
		{
			rc = m_pSurface->AddDisplayObj(&(Verts[iVert]), ++NumOfVerts_local, 2, 16, m_pHatch);
			if(rc)
				QUIT

			// count intersections of the rays from midpoints of text blocks
			// with the segments of just added path
			for(j = 0; j < NumOfPolys; j++)
			{
				pPoly = m_cTextBlocks.Poly(j);
				if(pPoly->Size() != 5)
					break; // something wrong...
				MidPt[0] = ((*pPoly)[0].X() + (*pPoly)[2].X()) / 2.0;
				MidPt[1] = ((*pPoly)[0].Y() + (*pPoly)[2].Y()) / 2.0;
				// now count intersections the ray from MidPt to the left
				// with segments of the path
				for(k = iVert + 1; k < iVert + NumOfVerts_local; k++)
				{	// NOTE: k may be changed in the body of the loop
					if(Verts[k-1][0] == Verts[k][0] &&
					   Verts[k-1][1] == Verts[k][1])
					{
						k++;
						ASSERT(k < iVert + NumOfVerts_local);
						// it should be never because we've excluded repeated verts
					}
					if( ((Verts[k][0] > MidPt[0]) || (Verts[k-1][0] > MidPt[0])) &&
						( ((Verts[k][1] < MidPt[1]) && (Verts[k-1][1] > MidPt[1])) ||
						  ((Verts[k][1] > MidPt[1]) && (Verts[k-1][1] < MidPt[1])) ) )
						NumOfIntersections[j]++;
				}
			}

			i++; iVert = i; NumOfVerts_local = 0;
		}
	}

	// clip away text blocks by inserting them into dispobjs list if needed
	for(j = 0; j < NumOfPolys; j++)
	{
		if(ODD(NumOfIntersections[j])) // add dispobj in this case
		{
			pPoly = m_cTextBlocks.Poly(j);
			for(k = 0; k < 5; k++)
			{
				TxtBlkVerts[k][0] = (*pPoly)[k].X();
				TxtBlkVerts[k][1] = (*pPoly)[k].Y();
				TxtBlkVerts[k][2] = 0.0;
			}
			rc = m_pSurface->AddDisplayObj(TxtBlkVerts, 5, 2, 4, m_pHatch);
			if(rc)
				QUIT
		}
	}


exit:
	delete[] Verts;
	delete[] NumOfIntersections;
	RETURN_RC(CHatchDisplay::Generate, rc);
}

#endif // NEW_SOLID_GENERATE

//////////////////////////////////////////////////////////////////
//																//
//					External Interface							//
//																//
//////////////////////////////////////////////////////////////////
// Display a HATCH entity
int gr_DisplayHatch
(
 db_hatch*			pHatch,		// I: Hatch entity's DB handle
 gr_DisplaySurface*	pSurface	// I/O: The display surface
)
{
	RC				rc;
	int				isSolidFill, pathsNum, i;
	CHatchDisplay*	pDisplay = NULL;

	pHatch->get_70(&isSolidFill);
	switch(isSolidFill)
	{
	case 0  :
		if(rc = ALLOC_FAIL(pDisplay = new CHatchDisplay(pHatch, pSurface)))
			QUIT
		break;

	case 1  :
		if(rc = ALLOC_FAIL(pDisplay = new CFillDisplay(pHatch, pSurface)))
			QUIT
		break;

	default :
		rc = E_BadInput;
		QUIT
	}

	pHatch->get_91(&pathsNum);
	for(i = pathsNum; i--; )
	{
		if(rc = pDisplay->LinearizePath(i))
			QUIT
	}
	if(g_bAbnormalPathEdgesChaining && pSurface->m_pGrView->m_Clipper.IsNotEmpty())
		pSurface->m_bAlreadyInUcs = true;	// Note, it's not correct right now but it will
											// because we will transform points in CInstance::AddSegment

#ifdef DEBUG_BOUNDARIES
	// display the boundaries, for debug
	pDisplay->Test();
#else
	// generate the hatch
	rc = pDisplay->Generate();
#endif // DEBUG_BOUNDARIES

exit:
	delete pDisplay;
	return rc;
}

