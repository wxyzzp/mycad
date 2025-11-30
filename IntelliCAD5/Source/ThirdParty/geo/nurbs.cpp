/*	NURBS.cpp - Implements the class CNURBS.
 *	Copyright (C) 1994-1998 Visio Corporation. All rights reserved.
 *
 *
 *	Abstract
 *
 * See NURBS.h for the definition of the class; this implements its methods.
 *
 */
#include <exception>
#include "Geometry.h"
#include "Rational.h"
#include "SqArray.h"
#include "TheUser.h"
#include "Line.h"
#include "CrvPair.h"
#include "Trnsform.h"
#include "Extents.h"
#include "PolyLine.h"
#include "CompCrvs.h"
#include "2CurveSolver.h"
#include "NURBS.h"
#include "Matrix.h"
#include "Vector.h"
#include "array2d.h"
#include "Plane.h"

#ifdef _DEBUG
//#include <ostream.h>
#include <fstream.h>
#endif // _DEBUG

DB_SOURCE_NAME;


////////////////////////////////////////////////////////////
// Spline Prober Class

/*
 This prober is a base class for classes that perform some prescribed
 action on all the vertices of an approximating polygon of the spline.
 The nature of the action is to be defined by specific derived classes.
 */

class CProber
	{
public:
	// Constructors/destructors
	CProber()
		{
		m_pSpline = NULL;
		m_t = 0;
		}

	virtual ~CProber()
		{
		}

	CProber(
		PNURBS pSpline)	// In: The spline
		{
		m_pSpline = pSpline;
		m_t = 0;
		}

	// Pure methods
	virtual RC ProbeAt(
		C3Point P,			 // In: A point
		double w,			 // In: The weight of this point
		double t,			 // In: Parameter value there
		BOOL* pDone=NULL)=0; // Out: =TRUE if we're done (optional)

	// Methods
	RC Linearize()
		{
		RC rc = SUCCESS;
		int i,j,k;
		double rCurrent, d;
		C3Point P[MAX_ORDER];
		double w[MAX_ORDER];
		int iDegree = m_pSpline->Degree();
		int nPointsPerSpan = 3 * iDegree;

		int iTo = m_pSpline->PointCount();
		if (!m_pSpline->Period())
			iTo--;

		double rNext = m_pSpline->Knot(0);
		for (i = 0;  i <= iTo;  i++)
			{
			rCurrent = rNext;
			rNext = m_pSpline->Knot(i + 1);
			if (rNext == rCurrent)
				continue;

			// Generate the first degree+1 points
			d = (rNext - rCurrent) / nPointsPerSpan;
			for (j=0;  j<=m_pSpline->Degree();  j++)
				{
				if (rc = m_pSpline->EvaluateHomogeneous(rCurrent, P[j], w[j]))
					QUIT
    			if (rc = ProbeAt(P[j], w[j], rCurrent))
					QUIT
 				rCurrent += d;
				} // End for j

			// Replace all points except the last one
			// with the differences of various levels
			for (j=1;  j<=iDegree;  j++)
				for (k=0;  k<=iDegree-j;  k++)
					{
					P[k] = P[k+1] - P[k];
					w[k] = w[k+1] - w[k];
					}

			// Now generate the rest of the points by forward differencing
			for (j=m_pSpline->Degree()+2;  j<nPointsPerSpan;  j++)
				{
				for (k=1;  k<=iDegree;  k++)
					{
					P[k] += P[k-1];
					w[k] += w[k-1];
					}
				rCurrent += d;
    			if (rc = ProbeAt(P[iDegree], w[iDegree], rCurrent))
					QUIT
				} // End fo rj
			}	// End for i

		// Probe at the last point
		rCurrent = m_pSpline->Knot(iTo + 1);
		if (rc = m_pSpline->EvaluateHomogeneous(rCurrent, P[0], w[0]))
			QUIT
    	if (rc = ProbeAt(P[0], w[0], rCurrent))
			QUIT
	exit:
		RETURN_RC(CProber::Linearize, rc);
		}

	// Data
protected:
	PNURBS m_pSpline;	// The spline
	double m_t;			// Current parameter value
	BOOL m_bFirst;		// = true if this is the first point
	};


////////////////////////////////////////////////////////////
// Spline Control Vertex Class
//
// Author:	Stephen W. Hou
// Date:	April 5, 2003
//
class CVertex
{
protected:

	CVertex(int dim = 3) : m_Dim(dim), m_pData(new double[dim]) {}

public:

	CVertex(const CVertex &vertex) : m_Dim(vertex.m_Dim), m_pData(NULL)
	{
		ASSERT(vertex.m_pData != NULL);
		m_pData = new double[m_Dim];
		memcpy(m_pData, vertex.m_pData, m_Dim * sizeof(double));
	}

	virtual ~CVertex() { if (m_pData) delete [] m_pData; }

	double		GetComponent(int index) const { ASSERT(index >=0 && index < m_Dim); return m_pData[index]; }

	C3Point		GetPoint() const { return C3Point(m_pData[0], m_pData[1], m_pData[2]); }
	double		GetWeight() const { return (m_Dim == 4) ? m_pData[3] : 1.0; }

	int			Dimension() const { return m_Dim; }

	CVertex		operator*(double d) const
	{
		CVertex vertex(m_Dim);
		for (register i = 0; i < m_Dim; i++)
			vertex.m_pData[i] = m_pData[i] * d;
		return vertex;
	}

	friend CVertex operator*(double d, const CVertex &vertex)
	{
		return vertex * d;
	}


	CVertex&	operator=(const CVertex &vertex)
	{
		if (this != &vertex) {
			if (m_Dim != vertex.m_Dim) {
				if (m_pData)
					delete [] m_pData;
				m_Dim = vertex.m_Dim;
				m_pData = new double [m_Dim];
			}
			memcpy(m_pData, vertex.m_pData, m_Dim * sizeof(double));
		}
		return *this;
	}

	CVertex		operator+(const CVertex &vertex) const
	{
		ASSERT(m_Dim == vertex.m_Dim);

		CVertex newVertex(m_Dim);
		for (register i = 0; i < m_Dim; i++)
			newVertex.m_pData[i] = m_pData[i] + vertex.m_pData[i];

		return newVertex;
	}

	CVertex		operator-(const CVertex &vertex) const
	{
		ASSERT(m_Dim == vertex.m_Dim);

		CVertex newVertex(m_Dim);
		for (register i = 0; i < m_Dim; i++)
			newVertex.m_pData[i] = m_pData[i] - vertex.m_pData[i];

		return newVertex;
	}

	CVertex&	operator+=(const CVertex &vertex)
	{
		ASSERT(m_Dim == vertex.m_Dim);
		for (register i = 0; i < m_Dim; i++)
			m_pData[i] += vertex.m_pData[i];

		return *this;
	}

	CVertex&	operator-=(const CVertex &vertex)
	{
		ASSERT(m_Dim == vertex.m_Dim);
		for (register i = 0; i < m_Dim; i++)
			m_pData[i] -= vertex.m_pData[i];

		return *this;
	}

#ifdef DEBUG
	friend ostream& operator<<(ostream& out, const CVertex &vertex)
	{
		out << "V(";
		for (register i = 0; i < vertex.m_Dim; i++) {
			out << vertex.m_pData[i];
			if (i != vertex.m_Dim - 1)
				out << ", ";
			else
				out << ");";
		}
		return out;
	}
#endif
protected:

	double*		m_pData;
	int			m_Dim;
};


////////////////////////////////////////////////////////////
// Spline Control Vertex Class in 3D space
//
// Author:	Stephen W. Hou
// Date:	April 5, 2003
//
class C3Vertex : public CVertex
{
public:

	C3Vertex() : CVertex(3) {}
	C3Vertex(const C3Vertex &vertex) : CVertex(vertex) {}
	C3Vertex(const C3Point &pt) : CVertex(3)
	{
		m_pData[0] = pt.X(); m_pData[1] = pt.Y(); m_pData[2] = pt.Z();
	}

	C3Vertex(double x, double y, double z) : CVertex(3)
	{
		m_pData[0] = x; m_pData[1] = y; m_pData[2] = z;
	}

	C3Vertex& operator=(const C3Vertex &vertex) { CVertex::operator=(vertex); return *this; }
	operator CVertex() { return (CVertex)*this; }
};


////////////////////////////////////////////////////////////
// Spline Control Vertex Class in 4D space
//
// Author:	Stephen W. Hou
// Date:	April 5, 2003
//
class C4Vertex : public CVertex
{
public:

	C4Vertex() : CVertex(4) {}
	C4Vertex(const C4Vertex &vertex) : CVertex(vertex) {}
	C4Vertex(const C3Point &pt, double weight)  : CVertex(4)
	{
		m_pData[0] = pt.X(); m_pData[1] = pt.Y(); m_pData[2] = pt.Z(); m_pData[3] = weight;
	}

	C4Vertex(double x, double y, double z, double w) : CVertex(4)
	{
		m_pData[0] = x; m_pData[1] = y; m_pData[2] = z; m_pData[3] = w;
	}

	C4Vertex& operator=(const C4Vertex &vertex) { CVertex::operator=(vertex); return *this; }
	operator CVertex() { return (CVertex)*this; }
};



/************************************************************************/
//
// This class implements a fixed size array of NURBS control vertices
// in 3D or 4D space.
//
// Author: Stpehen W. Hou
// Date:   April 5, 2003
//
/*************************************************************************/

class CControlVertices
{
public:


	CControlVertices(int length = 2, int dim = 3);
	CControlVertices(CControlVertices &old, bool swap = false);

	// Construct a 4D NURBS control vertices from 3D control point array
	// and weight array.
	//
	CControlVertices(const CControlPoints &ptArray,
					 const CRealArray &weights = *(CRealArray*)NULL);

	virtual ~CControlVertices() { if (m_pVertices) delete [] m_pVertices; }

	int					Length() const { return m_Length; }
	int					Dimension() const { return (m_Length) ? m_pVertices[0].Dimension() : 0; }

	CControlPoints*		GetPoints() const;
	CRealArray*			GetWeights() const;

	// Return a control vertex
	//
	CVertex				Get(int i) const { ASSERT(i >= 0 && i < m_Length); return m_pVertices[i]; }
	CVertex&			operator[](int i) { ASSERT(i >= 0 && i < m_Length); return m_pVertices[i]; }

	CControlVertices&	JoinWith(const CControlVertices &controlVertices,
		                         bool shareEnd = true);

	// Mupliplication operation. Note the input matix must be an alpha matrix.
	//
	CControlVertices	operator*(CMatrix &alphaMatrix) const;

	// Assign operation
	//
	CControlVertices&	operator=(const CControlVertices &vertices);

#ifdef _DEBUG
	friend ostream&		operator<<(ostream& out, const CControlVertices &vertices);
#endif
private:

	void				MakeRoom(int length, int dim);

	CVertex*			m_pVertices;
	int					m_Length;
};



CControlVertices::CControlVertices(int length, int dim)
{
	MakeRoom(length, dim);
}


CControlVertices::CControlVertices(const CControlPoints &ptArray,
							       const CRealArray &weights) : m_Length(ptArray.Size())
{
	ASSERT(m_Length > 0);

	if (&weights != (CRealArray*)NULL) {
		ASSERT(ptArray.Size() == weights.Size());

		m_pVertices = new C4Vertex[m_Length];
		for (register i = 0; i < m_Length; i++)
			m_pVertices[i] = C4Vertex(ptArray[i], weights[i]);
	}
	else {
		m_pVertices = new C3Vertex[m_Length];
		for (register i = 0; i < m_Length; i++)
			m_pVertices[i] = C3Vertex(ptArray[i]);
	}
}


CControlVertices::CControlVertices(CControlVertices &vertices, bool swap)
{
	if (vertices.m_Length) {
		if (swap) {
			m_Length = vertices.m_Length;
			m_pVertices = vertices.m_pVertices;
			vertices.m_pVertices = NULL;
			vertices.m_Length = 0;
		}
		else {
			MakeRoom(vertices.m_Length, vertices.Dimension());
			for (register i = 0; i < m_Length; i++)
				m_pVertices[i] = vertices.m_pVertices[i];
		}
	}
	else {
		m_Length = 0;
		m_pVertices = NULL;
	}
}


CControlVertices& CControlVertices::JoinWith(const CControlVertices &controlVertices,
											 bool shareEnd)
{
    int sharePointCount = (shareEnd) ? 1 : 0;
	CVertex* pVertices = NULL;
	if (Dimension() == 3)
		pVertices = new C3Vertex[m_Length + controlVertices.m_Length - sharePointCount];
	else
		pVertices = new C4Vertex[m_Length + controlVertices.m_Length - sharePointCount];

    for (register i = 0; i < m_Length; i++ )
		pVertices[i] = m_pVertices[i];

    if (shareEnd)
		pVertices[m_Length - 1] = 0.5 * (m_pVertices[m_Length - 1] + controlVertices.m_pVertices[0]);

    for (i = 0; i < controlVertices.m_Length - sharePointCount; i++)
		pVertices[m_Length + i] = controlVertices.m_pVertices[sharePointCount + i];

	delete [] m_pVertices;

	m_Length += controlVertices.m_Length - sharePointCount;
	m_pVertices = pVertices;
    return *this;
}


// Mupliplication operation. Note the input matix must be an alpha matrix.
//
CControlVertices CControlVertices::operator*(CMatrix &alpha) const
{
    int length = alpha.ColumnNumber() - 1;

    CControlVertices tmp(alpha.RowNumber());
    for (register i = 0; i < alpha.RowNumber(); i++) {

		// get the first point to start multiply
		int sIndex = (int)alpha(i, 0);

		tmp[i] = m_pVertices[sIndex] * alpha(i, 1);
		for (register j = 1; j < length; j++)
			tmp[i] += m_pVertices[sIndex + j] * alpha(i, j + 1);
    }
    return tmp;
}


// Assign operation
//
CControlVertices& CControlVertices::operator=(const CControlVertices &vertices)
{
	if (&vertices != this) {
		m_Length = vertices.m_Length;
		if (m_pVertices)
			delete [] m_pVertices;
		if (m_Length) {
			if (vertices.Dimension() == 3)
				m_pVertices = new C3Vertex[m_Length];
			else
				m_pVertices = new C4Vertex[m_Length];

			memcpy(m_pVertices, vertices.m_pVertices, m_Length * sizeof(CVertex));
		}
		else
			m_pVertices = NULL;
	}
	return *this;
}


CControlPoints*	CControlVertices::GetPoints() const
{
	CControlPoints* pControlPoints = NULL;
	if (m_Length) {
		RC rc;
		pControlPoints = new CControlPoints(m_Length, rc);
		for (register i = 0; i < m_Length; i++)
			pControlPoints->Set(i, m_pVertices[i].GetPoint());
	}
	return pControlPoints;
}



CRealArray*	CControlVertices::GetWeights() const
{
	CRealArray* pWeights = NULL;
	if (m_Length) {
		RC rc;
		pWeights = new CRealArray(m_Length, rc);
		for (register i = 0; i < m_Length; i++)
			(*pWeights)[i] = m_pVertices[i].GetWeight();
	}
	return pWeights;
}


#ifdef DEBUG
ostream& operator<<(ostream& out, const CControlVertices &vertices)
{
	out << "Control Vertices: " << endl;
	out << "Dimension = " << vertices.Dimension() << "; ";
	out << "Number of Control Points = " << vertices.Length() << endl;
	for (register i = 0; i < vertices.Length(); i++)
		out << vertices.m_pVertices[i] << endl;

	return out;
}
#endif


void CControlVertices::MakeRoom(int length, int dim)
{
	m_Length = length;
	if (dim == 3)
		m_pVertices = new C3Vertex[m_Length];
	else
		m_pVertices = new C4Vertex[m_Length];
}




/************************************************************************/
//
// This function construct an alpha matrix for calculating new NURBS
// control vertices based on the old NURBS control vertices:
//
//    new control vertices = old control vertices * alpha
//
// Note the matrix returned by this function is stored in a special way -
// the first column is used to store the base point of the old control
// vertices, so only multiplication function defined in CNurbs4DVertices
// class above handle the subsequent multiplication.
//
// Author: Stpehen W. Hou
// Date:   April 6, 2003
//
/*************************************************************************/

CMatrix AlphaMatrix(CKnots &oldKnotVector, CKnots &newKnotVector, int order)
{
    int n = oldKnotVector.Size(); // size of the old knots
	int m = newKnotVector.Size(); // length of the new knots
    int oldOrder = order;

    CMatrix alpha(m, ++order, 0.); // set up the matrix and initialize it with zeros

    // calculate base points and set the involved segments with ones
    for (register j = 0; j < m; j++) {
		for (register i = 0; i < n; i++) {
			if (oldKnotVector.Knot(i) <= newKnotVector.Knot(j) &&
				newKnotVector.Knot(j) < oldKnotVector.Knot(i + 1)) {
				if (alpha(j,0) == 0.)
					alpha(j,0) = i + 1 - oldOrder;

				alpha(j, oldOrder) = 1.;
				break;
			}
		}
    }

    // calculate recursively for the memebers of alpha
    register double d1,d2;
    for (register k = 2; k < order; k++) {
		for (register j = 0; j < m; j++) {

			// get the prestored base point
			int base = (int)alpha(j, 0);

			// the range is between base & base+order
			for (register i = base; i < base + oldOrder; i++) {

				// get the local index i1
				int i1 = i - base + 1;

				// calculate the two coefficients d1 and d2
				if ((d1 = oldKnotVector.Knot(i + k - 1) - oldKnotVector.Knot(i)) == 0.)
					d1 = 0.;
				else
					d1 = (newKnotVector.Knot(j + k - 1) - oldKnotVector.Knot(i)) * alpha(j, i1) / d1;

				if (i1 >= oldOrder || (d2 = (oldKnotVector.Knot(i + k) - oldKnotVector.Knot(i + 1))) == 0.)
					d2 = 0.;
				else
					d2 = (oldKnotVector.Knot(i + k) - newKnotVector.Knot(j + k-1)) * alpha(j, i1 + 1) / d2;

				// update the meber of alpha
				alpha(j, i1) = d1 + d2;
			}
		}
	}

    return alpha;
}



//////////////////////////////////////////////////////////////////////
//																	//
//		Default constructor & destructor, basic services			//
CNURBS::CNURBS()
    {
	Initialize();
    }

CNURBS::~CNURBS()
    {
    Purge();
    }
/*******************************************************************/
// Clone
PCurve CNURBS::Clone() const
	{
	RC rc = SUCCESS;
	PCurve p =  new CNURBS(*this, rc);
	if (!p)
		rc = DM_MEM;

	if (rc)
		{
		delete p;
		p = NULL;
		}
	return p;
	}
/*******************************************************************/
// Initialize the pointers
void CNURBS::Initialize()
	{
    m_pControlPoints = NULL;
    m_pKnots = NULL;
	m_pWeights = NULL;
	m_bPlanar = TRUE;	
	m_rStart = m_rEnd = m_rPeriod = 0;
	}
/*******************************************************************/
// Release all referenced data
void CNURBS::Purge()
	{
	delete m_pControlPoints;
	m_pControlPoints = NULL;

	delete m_pKnots;
	m_pKnots = NULL;

	delete m_pWeights;
	m_pWeights = NULL;
	}
/*******************************************************************/
// Make a decent NURBS out of the current data - if passable
RC CNURBS::Sanitize()
	{
	RC rc = E_BadSpline; // Presumed guilty
	BOOL bNonRational = TRUE;

	if (m_pKnots->Size() != m_pControlPoints->Size())
		QUIT

	if (m_pKnots->Sanitize())
		QUIT

	if (m_pWeights)
		{
		int i;
		double wFirst = Weight(0);
		if (m_pWeights->Size() != PointCount())
			QUIT

		for (i = 0;		i < PointCount();		i++)
			{
			double w = Weight(i);
			if (w < FUZZ_GEN)
				QUIT
			if (w != wFirst)
				bNonRational = FALSE;
			}
		}

	// Passed the inspection
	SetInterval();
	if (bNonRational)
		{
		delete m_pWeights;
		m_pWeights = NULL;
		}
	else
	// Tighten up the storage
		if (rc = m_pWeights->ReallocateToSize())
			QUIT
	if (rc = m_pControlPoints->ReallocateToSize())
		QUIT
	rc = m_pKnots->ReallocateToSize();
exit:
#ifdef DEBUG
	if (rc)
		Dump();
#endif
	RETURN_RC(CNURBS::Sanitize, rc);
	}
//////////////////////////////////////////////////////////////////////
//																	//
//				Various constructors								//

// Construct from raw data
CNURBS::CNURBS(
	PKnots pKnots,					// In: Knots
	PControlPoints pControlPoints,	// In: Controlpoints
	PRealArray pWeights,			// In: Weights
	RC & rc)						// Out: Return code
    {
	Initialize();

	if (!pKnots  ||  !pControlPoints)
		{
		rc = E_BadInput;
		QUIT
		}

    m_pKnots = pKnots;
    m_pControlPoints = pControlPoints;
    m_pWeights = pWeights;

	m_rStart = (*pKnots)[0];
	m_rEnd = (*pKnots)[pKnots->Size() - 1];
	m_rPeriod = pKnots->Period();
    m_bPlanar = m_pControlPoints->IsPlanar();
	rc = Sanitize();
exit:
	if (rc)
		Purge();
    }
/************************************************************************/
// Construct from raw data
CNURBS::CNURBS(
	PKnots pKnots,					// In: Knots
	PControlPoints pControlPoints,	// In: Controlpoints
	PRealArray pWeights,			// In: Weights
	double startParam,				// In: Start parameter
	double endParam,				// In: End parameter
	double periodic,				// In::Periodic
	RC & rc)						// Out: Return code
    {
	Initialize();

	if (!pKnots  ||  !pControlPoints)
		{
		rc = E_BadInput;
		QUIT
		}

    m_pKnots = pKnots;
    m_pControlPoints = pControlPoints;
    m_pWeights = pWeights;

	m_rStart = startParam;
	m_rEnd = endParam;
	m_rPeriod = periodic;
    m_bPlanar = m_pControlPoints->IsPlanar();
	rc = Sanitize();
exit:
	if (rc)
		Purge();
    }
/************************************************************************/
// Copy an existing spline
CNURBS::CNURBS(
	const CSpline & Other,  // In: The other spline to copy
	RC & rc)				// Out: Return code
	{
	Initialize();

    m_pControlPoints = new CControlPoints(*Other.m_pControlPoints, rc);
	m_bPlanar = Other.m_bPlanar;

	QUIT_IF_FAILED(m_pControlPoints)

   	m_pKnots = new CKnots(*Other.m_pKnots, rc);
	QUIT_IF_FAILED(m_pKnots)

	if (Other.Rational())
		{
		m_pWeights = new CRealArray(*Other.m_pWeights, rc);
		QUIT_IF_FAILED(m_pWeights)
		}

	m_rEnd = Other.m_rEnd;
	m_rPeriod = Other.m_rPeriod;
	m_rStart = Other.m_rStart;
exit:
	if (rc)
		Purge();
    }
/*******************************************************************/
// Construct from a packed non-rational spline
CNURBS::CNURBS(
	LPBYTE & pStorage,  // In/Out: The storage
	RC & rc)			// Out: Return code
	// pStorage will be advanced to the next piece of data
	{
	Initialize();

	// Unpack the domain and period
	double* pDomain = (double*)pStorage;
	m_rStart = pDomain[0];
	m_rEnd = pDomain[1];
	m_rPeriod = pDomain[2];
	pStorage += 3*sizeof(double);

	// Get the knots and control points from storage
   	m_pKnots = new CKnots(pStorage, rc);
	if (!m_pKnots)
		rc = DM_MEM;
	if (!rc)
		{
    	m_pControlPoints = new CControlPoints(pStorage, rc);
		if (!m_pControlPoints)
			rc = DM_MEM;
		}
		else
			m_bPlanar = m_pControlPoints->IsPlanar();
    }
/************************************************************************/
// Copy or approximate a spline with a new degree
CNURBS::CNURBS(
	const CNURBS & Other,   // In: The other spline to copy
	int nNewDegree,          // In: The desired new degree
	RC & rc)                 // Out: Return code
	{
	rc = FAILURE;
	Initialize();

	m_rStart = Other.m_rStart;
	m_rEnd = Other.m_rEnd;
    /* If the new degree is lower than the existing degree then this
     * constructor will produce an approximation of the other spline.
     * Otherwise the new spline will replicate the old one exactly,
	 * but It will "sanitize" the knot sequence, snapping together knots
	 * that are almost equal, and return an error if the knots are
	 * beyond repair.
     */
	// Quit if the input is no good
	if (nNewDegree < 1)
		{
		rc = E_BadInput;
		QUIT
		}

	// Construct the knot sequence with the new degree
	m_pKnots = new CKnots(*Other.m_pKnots, nNewDegree, rc);
	QUIT_IF_FAILED(m_pKnots)

	if (nNewDegree == Other.Degree())
		{
		// Just copy the control points and weights
		m_pControlPoints = new CControlPoints(*Other.m_pControlPoints, rc);
		m_bPlanar = Other.m_bPlanar;
		QUIT_IF_FAILED(m_pControlPoints)
		if (Other.Rational())
			{
			m_pWeights = new CRealArray(*Other.m_pWeights, rc);
			QUIT_IF_FAILED(m_pWeights)
			}
		}
	else
		{
		// Compute new control points and weights
		m_pControlPoints = new CControlPoints(m_pKnots->Size(), rc);
		QUIT_IF_FAILED(m_pControlPoints)
		if (Other.Rational())
			{
			m_pWeights = new CRealArray(m_pKnots->Size(), rc);
			QUIT_IF_FAILED(m_pWeights)
			}
		if (rc = Approximate(Other, 0, PointCount()))
			QUIT

		m_bPlanar = m_pControlPoints->IsPlanar();
		}
	// In any case -
	rc = Sanitize();
exit:
	if (rc)
		Purge();
	}
/************************************************************************/
// Given a CNURBS object construct its representation in piecewise Bezier form
// This will be patterned after the degree-raising constructor that
// precedes this one.
CNURBS::CNURBS(const CNURBS & Other, // In:  The spline to represent
			   BEZTYPE beztype,    // In: Placeholder to give unique signature
			   RC & rc)  // Out:  Return code
	{
    rc = FAILURE;
	Initialize();

	m_rStart = Other.m_rStart;
	m_rEnd = Other.m_rEnd;

	// construct a knot sequence whose interior knots all have
	// multiplicity equal to degree

    m_pKnots = new CKnots(*Other.m_pKnots, beztype, rc); // need to implement
	QUIT_IF_FAILED(m_pKnots);

	// Allocate space for the new control points.
	m_pControlPoints = new CControlPoints(m_pKnots->Size(), rc);
	QUIT_IF_FAILED(m_pControlPoints);

	m_bPlanar = Other.m_bPlanar;

	// Allocate space for the new weights if required
	if (Other.Rational())
		{
		m_pWeights = new CRealArray(m_pKnots->Size(), rc);
		QUIT_IF_FAILED(m_pWeights);
		}

	// Find the control points and weights for the new spline
	if (rc = Approximate(Other, 0, PointCount()))
		QUIT

    rc = Sanitize();


exit:
	if (rc)
		Purge();
	}
/************************************************************************/
// Constructor from given knots and control points
CNURBS::CNURBS(
	PKnots & pKnots,           // In: Knot sequence
	PControlPoints & pPoints,  // In: Control points
	RC & rc)				   // Out: Return code
	{
	Initialize();
   	m_pKnots = pKnots;
   	m_pControlPoints = pPoints;
	m_bPlanar = m_pControlPoints->IsPlanar();
   	rc = Sanitize();
	if (rc)
		Purge();
    }
/************************************************************************/
// Constructor with given degree and number of control points, no weights
CNURBS::CNURBS(
	int nDegree,    // In: Degree
	int nPoints,    // In: Number of control points
	RC & rc)		// Out: Return code
// The knots and control points are allocated and set to 0
    {
	Initialize();
    m_rStart = m_rEnd = 0;
    m_pKnots = new CKnots(nDegree, nPoints, rc);
	if (!m_pKnots)
		rc = DM_MEM;
    if (rc)
		{
		m_pControlPoints = NULL;
		QUIT
		}
    m_pControlPoints = new CControlPoints(nPoints, rc);
	if (!m_pControlPoints)
		rc = DM_MEM;
	else
		m_bPlanar = m_pControlPoints->IsPlanar();

exit:
	if (rc)
		Purge();
   }

/************************************************************************/
// Construct a spline by interpolating an array points with given nodes
CNURBS::CNURBS(
	CPointArray & arrPoints,   // In: Array of points
	CRealArray & arrNodes,     // In: Their parameter nodes
	BOOL bPeriodic,			   // In: =TRUE if the spline is to be periodic
	RC & rc)                   // Out: Return code
    {
	rc = SUCCESS;
	int n = arrNodes.Size();
	int nDegree = MIN(n-1,3);
	Initialize();
/*
 * If the spline is to be periodic then the last point is still expected
 * here, despite the fact that it's assumed to be equal to the first one,
 * because it requires a node. It is there, however, only as a placeholder,
 * so the value doesn't matter.  A periodic spline must have at least 5
 * (4 distinct) points.  (The theoretical restriction is only 4, but the
 * current Visio implementation coughs on 4)
 */
	ASSERT(arrPoints.Size() == arrNodes.Size());
	ASSERT(!bPeriodic || n >= 5);

	/* First store the interpolation points as control points and
	 * the nodes as knots
	 */
	m_pControlPoints = new CControlPoints(arrPoints, rc);
	if (!m_pControlPoints)
		rc = DM_MEM;

	m_bPlanar = m_pControlPoints->IsPlanar();

    if (rc)
    	QUIT
	m_pKnots = new CKnots(arrNodes, nDegree, rc);
	if (!m_pKnots)
		rc = DM_MEM;
    if (rc)
    	QUIT

	if (!bPeriodic)     // Multiple knpts at the ends
		if (rc = m_pKnots->Insert(arrNodes[0], 0, nDegree))
			QUIT

	switch (nDegree)
		{
		case 1:  // Straight line - nothing to do
			if (bPeriodic)
				// Straight line can't be periodic
				rc = E_BadInput;
			break;

		case 2:
		    {
		    if (bPeriodic)   // A periodic cubic spline
				{
				rc = E_BadInput;
				QUIT
				}

			// // Parabola - compute the mid control point
			double t = (arrNodes[1] - arrNodes[0]) /
		         	 (arrNodes[2] - arrNodes[0]);
			double s = 1.0 - t;
			ControlPoint(1) -= (t*t)*(ControlPoint(2));
			ControlPoint(1) -= (s*s)*(ControlPoint(0));
			ControlPoint(1) /= (2*s*t);

	    	// Forget the middle knot
			m_pKnots->CRealArray::Remove(3,1);

			rc = SUCCESS;
			break;
		    }

		case 3: // Cubic spline
			if (bPeriodic)  // Periodic cubic spline
				{
				m_pControlPoints->Remove(n-1, 1);
				rc = InvertPeriodic();
				}
			else //  "Not a Knot" spline interpolation
				{
				// Forget the second and second-to-last knots
				m_pKnots->Remove(n+1, 1);
				m_pKnots->Remove(4, 1);


				/* Set up and solve the equations for replacing
	 		 	* the interpolation points with control points.
	 		 	*/
	 			rc = InvertNotAKnot(arrNodes);
	 			}
	 		break;

	 	default: // We can't interpolate less than 2 points
			rc = FAILURE;
	    }
exit:
	SetInterval();
	if (rc)
	    Purge();
	}
/************************************************************************/
// Compute control points by interpolation with "Not-A-Knot" end conditions
RC CNURBS::InvertNotAKnot(
	CRealArray & arrNodes)     // In: The parameter nodes for interpolation
	/* This function is called to compute the control points of a spline
	 * that interpolates the current values of the control points points
	 * at parameter values given in the nodes array.  The nodes coincide
	 * with knots, except for the second and the second last;  that explain
	 * the term "Not-A-Knot".
	 */
	{
	int n = arrNodes.Size();
	int i;
	RC rc;

	// Set up the interpolation equations
	CBandedArray A(n,2,rc);
	if (rc)
		QUIT

	// First and last 2 rows
	A[0][0] = A[n-1][n-1] = 1.0;
	m_pKnots->EvaluateBasis (3, arrNodes[1], A[1]);
	m_pKnots->EvaluateBasis (n-1, arrNodes[n-2], &A[n-2][n-4]);

	// The rest of the array is tridiagonal
	for (i=2;  i<n-2;  i++)
		m_pKnots->EvaluateBasis (i+1, Knot(i+2), &A[i][i-2]);

    // Solve the equations
    if (rc = A.LUFactorNoPivot())
    	QUIT
    rc = A.LUSolveNoPivotP(*m_pControlPoints);

exit:  // That's it!
	RETURN_RC(CNURBS::InvertNotAKnot, rc);
    }
/************************************************************************/
// Convert (in place) interpolation points to control points
RC CNURBS::InvertPeriodic()
	{
	/* Don't bother to call this function with less than 3 points. Don't
	 * repeat the first point at the end, it is assumed to be eaqual to
	 * the first.
	 */
	RC rc,rc1;
	int i;
	double rValues[4];

	// Create an augmented array for n equations
	int n = PointCount();
	CBandedArray arrBanded(n-1,1, rc);
	CAugmentedArray A(&arrBanded, 1, rc1);
	if (rc)
		QUIT
	if (rc = rc1)
		QUIT

	/* Set the equations  Most of the array is tridiagonal, except for two entries:
	 * the entry that was supposed to get in the [0][-1] position goes at [0][n],
	 * and the one that would go at the [n][n+1] position goes at the [n][0] place.
	 * These are the entries that make it and augmented (rather than banded) array.
     *
	 * To get the correct matrix structure, the first equation needs to express the
	 * interpolation condition through Point[2].  We therefore need to shift the
	 * interpolation points list to start there
	 */
	if (rc = m_pControlPoints->StartAt(2))
		QUIT

	// Load the interpolation conditions
	for (i=0;  i<n;  i++)
		{
		m_pKnots->EvaluateBasis (i+2, Knot(i+2), rValues);
		A.Entry(i,i-1) = rValues[0];
		A.Entry(i,i) = rValues[1];
		A.Entry(i,i+1) = rValues[2];
		}

    // Solve the equations
    if (rc = A.LUFactorNoPivot())
    	QUIT
    if (rc = A.LUSolveNoPivotP(*m_pControlPoints))
    	QUIT

exit:  // That's it!
	RETURN_RC(CNURBS::InvertPeriodic, rc);
    }
/************************************************************************/
// Construct a spline by least-squares approximating an array points
CNURBS::CNURBS(
	CPointArray & arrPoints,   // In: Array of points
	CRealArray & arrNodes,     // In: Their parameter nodes
	CRealArray & arrKnots,     // In: Spline knots
	BOOL bPeriodic,            // In: =TRUE if the spline is to be periodic
	RC & rc)                   // Out: Return code
    {
	rc = FAILURE;

	// The number of points and nodes must be equal
	ASSERT (arrPoints.Size() < arrNodes.Size() + 2);

	// Construct knots and control points
	Initialize();
	m_pKnots = new CKnots(arrKnots, 3, rc);
	if (!m_pKnots)
		rc = DM_MEM;
    if (rc)
    	QUIT

	if (bPeriodic)
		rc = PeriodicLeastSquares(arrPoints, arrNodes);
	else
		rc = LeastSquares(arrPoints, arrNodes);
exit:
	if (rc)
	    Purge();
	else
 		SetInterval();
   }
/************************************************************************/
// Set up and solve the normal equations for least-squares approximation
RC CNURBS::LeastSquares(
	CPointArray & arrPoints,   // In: Array of points
	CRealArray & arrNodes)     // In: Their parameter nodes
    {
	// Knots have already been allocated, but not control points
	int i, n;
	PRealSqArray pMatrix = NULL;
	PBanded pBanded = NULL;
	PFullSqArray pFull = NULL;
	RC rc;

	if (rc = m_pKnots->Insert(Knot(0), 0, 3))
		QUIT

	// Control points array, missing the first and last
	m_pControlPoints = new CControlPoints(m_pKnots->Size() - 2, rc);
	if (!m_pControlPoints)
		rc = DM_MEM;
    if (rc)
    	QUIT

	// Full banded or augmented matrix, depending on size & periodicity
	n = PointCount();
	if (n < 7)
		pMatrix = pFull = new CFullSqArray(n, rc);
	else
		pMatrix = pBanded = new CBandedArray(n, 3, rc);
	if (rc)
		QUIT

	// Set up the linear system of the normal equations
	for (i = 1;   i < arrNodes.Size() - 1;   i++)
		{
		double rValues[4];
		int j, k, m, nSpan;

		// Get basis-function values
		double r = arrNodes[i];
		if (rc = m_pKnots->FindSpan(r, nSpan))
			QUIT
		m_pKnots->EvaluateBasis(nSpan, r, rValues);

		//Update the matrix and the right-hand side
		for (j = 0;   j <= 3;   j++)
			{
			m = nSpan - 4 + j;
			if (m < 0   ||   m >= PointCount())
				continue;
			// Right hand side update
			ControlPoint(m) += (arrPoints[i] * rValues[j]);

			for (k = 0;   k <= 3;   k++)
				{
				n = nSpan - 4 + k;
				if (n < 0)
					ControlPoint(m) -= (arrPoints[0] * (rValues[j] * rValues[k]));
				else
					if (n >= PointCount())
						ControlPoint(m) -= (arrPoints.Last() *
										   (rValues[j] * rValues[k]));
				else
					pMatrix->Entry(m,n) += rValues[j] * rValues[k];
				}
			}
		}
    // Solve the equations
    if (rc = pMatrix->LUFactorNoPivot())
		QUIT
    if (rc = pMatrix->LUSolveNoPivotP(*m_pControlPoints))
		QUIT

	// First & last control points are the first & last data points
	if (rc = m_pControlPoints->MakeRoom(0, 1))
		QUIT
	ControlPoint(0) = arrPoints[0];
	rc = m_pControlPoints->Add(arrPoints.Last());
exit:
	if (rc)
	    Purge();
	else
 		SetInterval();

	// Clean up the matrix
	delete pBanded;
	delete pFull;
	RETURN_RC(CNURBS::LeastSquares, rc);
    }
/************************************************************************/
// Set up and solve the periodic least squares approximation equations
RC CNURBS::PeriodicLeastSquares(
	CPointArray & arrPoints,   // In: Array of points
	CRealArray & arrNodes)     // In: Their parameter nodes
    {
	// Knots have already been allocated, but not control points
	int i, n, k, nAdded;
	double rAddtional[3];
	PRealSqArray pMatrix = NULL;
	PBanded pBanded = NULL;
	PAugmented pAugmented = NULL;
	PFullSqArray pFull = NULL;
	RC rc;

	// Use the interpolation constructor if there are less than 4 nodes
	ASSERT (arrNodes.Size() >= 4);
	ASSERT (m_pKnots->Size() > 0);

	// Make sure that there are at least 4 knots, well distributed
	n = arrNodes.Size();
	switch (m_pKnots->Size())
		{
		// Remember that there's always an additional implied knot
		case 1:
			nAdded = 3;
			rAddtional[0] = arrNodes[n/4];
			rAddtional[1] = arrNodes[n/2];
			rAddtional[2] = arrNodes[3*n/4];
			break;
		case 2:
			nAdded = 2;
			if (Knot(1) >= arrNodes[3*n/4])
				{
				rAddtional[0] = arrNodes[n/4];
				rAddtional[1] = arrNodes[n/2];
				}
			else if (Knot(1) <= arrNodes[n/4])
				{
				rAddtional[0] = arrNodes[n/2];
				rAddtional[1] = arrNodes[3*n/4];
				}
			else
				{
				rAddtional[0] = arrNodes[1/4];
				rAddtional[1] = arrNodes[3*n/4];
				}
			break;
		default:
			nAdded = 0;
		}
	for (i = 0;   i < nAdded;   i++)
		if (rc = m_pKnots->InsertKnot(rAddtional[i], 2, k, k))
			QUIT

	// Control points array
	m_pControlPoints = new CControlPoints(m_pKnots->Size(), rc);
	if (!m_pControlPoints)
		rc = DM_MEM;
    if (rc)
    	QUIT

	// Full banded or augmented matrix, depending on size & periodicity
	n = PointCount();
	if (n < 7)
		pMatrix = pFull = new CFullSqArray(n, rc);
	else
		{
		n -= 3;
		pBanded = new CBandedArray(n, 3, rc);
		if (rc)
			QUIT
		pMatrix = pAugmented = new CAugmentedArray(pBanded, 3, rc);
		}
	if (rc)
		QUIT

	// Set up the linear system of the normal equations
	for (i = 0;   i < arrNodes.Size();   i++)
		{
		double rValues[4];
		int j, m, nSpan;

		// Get basis-function values
		double r = arrNodes[i];
		if (rc = m_pKnots->FindSpan(r, nSpan))
			QUIT
		m_pKnots->EvaluateBasis(nSpan, r, rValues);

		//Update the matrix and the right-hand side
		for (j = 0;   j <= 3;   j++)
			{
			m = nSpan - 3 + j;
			ControlPoint(m) += (arrPoints[i] * rValues[j]);
			for (k = 0;   k <= 3;   k++)
				{
				n = nSpan - 3 + k;
				pMatrix->Entry(m,n) += rValues[j] * rValues[k];
				}
			}
		}

    // Solve the equations
    if (rc = pMatrix->LUFactorNoPivot())
		{
		DUMP(*m_pKnots);
		QUIT
		}
    rc = pMatrix->LUSolveNoPivotP(*m_pControlPoints);
exit:
	if (rc)
	    Purge();
	else
 		SetInterval();

	// Clean up the matrix
	delete pBanded;
	delete pAugmented;
	delete pFull;
	RETURN_RC(CNURBS::PeriodicLeastSquares, rc);
   }
/************************************************************************/
// Construct the cubic approximate a C2 curve with a spline within tolerance

CNURBS::CNURBS(
	CCurve & cCurve,	// In: The curve to be approximated, assumed C2
	double rTolerance,	// In: The acceptable error
	RC & rc)			// Out: Return code
	{
	int n;
	Initialize();
	if (!cCurve.Bounded())
		{
		// Cannot approximate an unbounded curve
		rc = E_BadInput;
		QUIT
		}
	m_bPlanar = cCurve.Planar();

	// Get a knot sequence
	if (rc = cCurve.CubicKnots(rTolerance, m_pKnots))
		QUIT
	n = m_pKnots->Size();
	ASSERT(m_pKnots->Degree() == 3);
	ASSERT(n >= 4);

	// Allocate control points
	m_pControlPoints = new CControlPoints(n, rc);
	if (!m_pControlPoints)
		rc = DM_MEM;
	if (rc)
		QUIT

	/*
	 * In the priodic case the approximation is based on points only (no derivatives),
	 * using the standard CNURBS::Approximate method.  In the non-periodic case the
	 * apprixmation will use end-derivatives.
	 */
	if (Period())
		rc = Approximate(cCurve, 0, n);
	else
		{
		C3Point P, T;

		// Set the first 2 control points
		if (rc = cCurve.Evaluate(cCurve.Start(), P, T))
			QUIT
		m_pControlPoints->Set(0, P);
		m_pControlPoints->Set(1, P + 0.3333333333333333 * (Knot(4) - cCurve.Start()) * T);

		// Set the last 2 control points
		if (rc = cCurve.Evaluate(cCurve.End(), P, T))
			QUIT
		m_pControlPoints->Set(n - 1, P);
		m_pControlPoints->Set(n - 2, P + 0.3333333333333333 * (Knot(n-1) - cCurve.End()) * T);
		if (n > 4)
			rc = Approximate(cCurve, 2, n - 3);
		}
exit:
	if (rc)
		Purge();
	else
		SetInterval();
    }

//////////////////////////////////////////////////////////////////////
//																	//
//						Data Access Methods							//

// Access the knot with a given index
double CNURBS::Weight(	// Return the weight of index i
	int i)	const		// In: Index of the desired weight
	{
	if (Rational())
		{
		// pull the index into the range
		m_pWeights->Mod(i);
		return (*m_pWeights)[i];
		}
	else
		return 1;
	}
/*******************************************************************/
// Set the value of a knot of given index
RC CNURBS::SetWeight(
	int i,			// In: Index of the desired weight
	double w)		// In: value to set to
	{
	RC rc = SUCCESS;
	if (Rational())
		{
		// pull the index into the range
		m_pWeights->Mod(i);
		(*m_pWeights)[i] = w;
		}
	else
		rc = E_BadInput;

	RETURN_RC(CNURBS::SetWeight, rc);
	}
/*******************************************************************/
// Return the geometry of the control point with a given index
C3Point CNURBS::ControlPointGeom(
	int nIndex) const // Index of the desired control point
	{
	if (Rational())
		// The control points are stored mutliplied by the weights
		return	(*m_pControlPoints)[nIndex] / Weight(nIndex);
	else
		return	(*m_pControlPoints)[nIndex];
	}
/*******************************************************************/
// Set a control point
void CNURBS::SetControlPoint(
	int nIndex,       // In: The index
	C3Point ptValue,  // In: The value to set it to
	double* pWeight)  // In: Weight (Optional)
	{
	if (pWeight)
		ptValue *= (*pWeight);
	else
		ptValue *= Weight(nIndex);
	m_pControlPoints->Set(nIndex, ptValue);
	}
//////////////////////////////////////////////////////////////////////
//																	//
//					Transform, Reverse, Restrict, etc.				//

// Transform in place
RC CSpline::Transform(
	CTransform & cTransform)   // In: The transformation
	{
	RC rc = SUCCESS;

	// A non-affine transform cannot be applied in place
	ASSERT(cTransform.Affine());

	// Apply the transformation to the control points
	for (int i = 0;  i < PointCount();  i++)
		{
		C3Point P = ControlPointGeom(i);
		cTransform.ApplyToPoint(P, P);
		SetControlPoint(i, P);
		}
	RETURN_RC(CNURBS::Transform, rc);
	}
/*******************************************************************************/
// A translation to a given point
RC CNURBS::Move(
	const C3Point & ptBy)	// In: The desired origin's new position
	{
	for (int i = 0;    i < PointCount();    i++)
		if (Rational())
			{
			C3Point P = ControlPointGeom(i);
			P += ptBy;
			SetControlPoint(i, P);
			}
		else
			ControlPoint(i) += ptBy;
	RETURN_RC(CNURBS::Move, SUCCESS);
	}
/*****************************************************************************/
// Replace this spline with its restriction a new domain
RC CNURBS::ReplaceWithRestriction(
	double rFrom,   // In: New curve start
	double rTo,     // In: New curve end
	PCurve & pOut)	// Out: The restricion to the new domain
	{
	RC rc = SUCCESS;
	PNURBS pNewSpline = NULL;

	if (rc = Subset(rFrom, rTo, *(PCurve*)&pNewSpline))
		QUIT;
/*
	int n;

	if (rTo - rFrom < FUZZ_GEN)
		{
		rc = E_BadInput;
		QUIT
		}

	if (FEQUAL(rFrom, Knot(0), FUZZ_GEN)  &&
		FEQUAL(rTo, Knot(LastKnot()), FUZZ_GEN))
		{
		pOut = this;
		goto exit;
		}

	// Allocate a new spline
	if (rc = ALLOC_FAIL(pNewSpline = new CNURBS))
		QUIT

	// Construct the new lists
	pNewSpline->m_pKnots =
		new CKnots(m_pKnots, rFrom, rTo, Degree(), rc);
	QUIT_IF_FAILED(pNewSpline->m_pKnots)
		n = pNewSpline->m_pKnots->Size();

	pNewSpline->m_pControlPoints =
		new CControlPoints(n, rc);
	QUIT_IF_FAILED(pNewSpline->m_pControlPoints)

	if (Rational())
		{
		pNewSpline->m_pWeights =
			new CRealArray(n, rc);
		QUIT_IF_FAILED(pNewSpline->m_pWeights)
		}

	// Compute the new control points and weights
	if (rc = pNewSpline->Approximate(*this, 0, n))
		QUIT

	// Wrap up
	if (rc = pNewSpline->Sanitize())
		QUIT
*/
	pOut = pNewSpline;
	pNewSpline = NULL;
	delete this;
exit:
	delete pNewSpline;
	RETURN_RC(CNURBS::ReplaceWithRestriction, rc);
    }
/*****************************************************************************/
// Reverse the direction of a spline in place
RC CNURBS::Reverse()
	{
	RC rc;

	// See how much this spline is currently trimmed (or extended)
	double rShift = Knot(PointCount() + Degree()) - m_rEnd;
	double rDomain = m_rEnd - m_rStart;

	// Reverse the lists
	if (rc = m_pKnots->Reverse())
		QUIT
	if (rc = m_pControlPoints->ReverseOrder())
		QUIT
	if (Rational())
		m_pWeights->Reverse();

	// The new domain
	SetInterval();
	m_rStart = Knot(0) + rShift;
	m_rEnd = m_rStart + rDomain;
exit:
	RETURN_RC(CNURBS::Reverse, rc);
	}
/*******************************************************************************/
// Return a modeling ready slice of the curve
RC CNURBS::NextSlice(	// Return E_NOMSG when it's the last slice
	int & i,			// In/out: An index marker
	PCurve & pSlice)	// Out: The current slice
	{
	pSlice = NULL;
	double r;
	RC rc = NextCusp(Start(), r);

	if (rc)
		{
		if (rc != E_NOMSG)
			QUIT
		// This must be the last slice
		ASSERT(r == End());
		pSlice = this;
		goto exit;
		}

	// The slice is not the entire spline
	if (rc = ALLOC_FAIL(pSlice = Clone()))
		QUIT
	ResetStart(r);
	pSlice->ResetEnd(r);
exit:
	i++;
	RETURN_RC(CNURBS::NextSlice, rc);
	}
//////////////////////////////////////////////////////////////////////
//																	//
//						Linrearization								//

// Add a linearization point to the cache
LOCAL RC AddPoint(
	C3Point P,			// In: A point (homogeneous coordinates)
	double w,			// In: Denominator (weight) value there
	double t,			// In: parameter value there
	PAffine pTransform,	// In: Rendering transformation (NULL OK)
	CCache & cCache)	// In/Out: The cache we render into
	{
	RC rc = SUCCESS;

	if (w < FUZZ_GEN)
		{
		rc = E_InputZero;
		QUIT
		}

	// Get the geometric point, transform it, and cache it
	P /= w;
	if (pTransform)
		pTransform->ApplyToPoint(P, P);
	if (rc = cCache.Accept(P, t))
		QUIT
exit:
	RETURN_RC(AddPoint, rc);
	}

/***********************************************************************/
// The number of linearization points required for this span
int CNURBS::RenderingNumber(
	int nSpan,				  // In: A span index
	double rTolerance) const  //In: The tolerated bow between curve and chord
// Return the number of linear segments required to smoothly render this span
    {
/* In the span Knot(i)<t<Knot(i+1) we look at the maximum bow of the triangles
   of edges relevant to that span.  There are degree-1 such triangles. We mark
   them according to their leftmost control-points.  We retain a moving list
   of rendering numbers from these triangles, we always drop the last and add
   a new number, and then take the maximum of that list. But we operate in the
   4 dimensional space of homogeneous coordinates (weight*point, weight).
   The distance, however, needs to be normalized by the minimal of the 3
   weights.
*/
	int n = 0;
	C3Point ptMid = (ControlPoint(nSpan+2) + ControlPoint(nSpan)) / 2;
	double r = Weight(nSpan);
	double s = Weight(nSpan+1);
	double t = Weight(nSpan+2);

	// Get the minimum of the 3 weights and adjust the tolerance with it
	double w = MIN(r, s);
	if (w < t)
		w = t;
	rTolerance *= w;

	// Get the length of the 4 dimensional median
	ptMid -= ControlPoint(nSpan+1);
	w = s - (r + t) / 2;
	r = sqrt(ptMid * ptMid + w * w);

	if (FZERO(r, FUZZ_GEN)  ||  rTolerance < FUZZ_GEN)
		n = 1;
	else
	/*
	 * Now we look at the ratio between the medain and the tolerance.
	 * If it's 1 or less then degree+1 segments will do. Otherwise,
	 * since the bow is roughly inverse proportional to the square of n,
	 * we set n to be the square root of this ratio
	 */
	 	{
		n = (int) (sqrt(r / rTolerance)) + Degree();
		}

	return n;
	}
/************************************************************************/
// Linearization
RC CNURBS::LinearizePart(
	double rFrom,			// In: Where to start
	double rTo,				// In: Where to stop
	double rResolution,		// In: Chord-height tolerance
	PAffine pTransform,		// In: Rendering transformation (NULL OK)
	CCache & cCache) const  // In/Out: The cache we render into
	{
	RC rc = SUCCESS;
	int i,j,k,n;
	double t,d,rNext;
	C3Point P[MAX_ORDER];
	double w[MAX_ORDER];
	int iDegree = Degree();
	int iNumbers[MAX_ORDER];
	int iSpan = 0;
	int limit = SHRT_MAX / (4 * PointCount());

 	// Locate the knot-span we start from
 	if (rc = m_pKnots->FindSpan(rFrom, iSpan))
 		QUIT

	// Get the initial numbers from the first degree-2 triangles and their max
	for (i=0;  i<iDegree - 2;  i++)
		iNumbers[i] = RenderingNumber(iSpan-iDegree+i, rResolution);

	// Now loop over spans and render them
	for (i = iSpan;
		 rFrom < rTo;
		 rFrom = rNext, i++)
	    {
		rNext = Knot(i+1);
		if (rTo < rNext)
			rNext = rTo;

	    /* Determine how many points are needed for this span
	       --------------------------------------------------
	    In the span Knot(i)<t<Knot(i+1) we look at the maximum bow of the triangles
	    of edges relevant to that span.  There are degree-1 such triangles. We mark
	    them according to their leftmost control-points.  We retain a moving list
	    of rendering numbers from these triangles, we always drop the last and add
	    a new number, and then take the maximum of that list
		*/
	    if (iDegree < 2)
	    	n = 1;
	    else
	    	{
			n = iNumbers[iDegree-2] = RenderingNumber(i-2, rResolution);

			// update the maximum while shifting the numbers
			for (j=0;  j<iDegree-2;  j++)
				{
				if (iNumbers[j] > n)
					n = iNumbers[j];
				iNumbers[j] = iNumbers[j+1];
				}

			if (n == 0   ||   Knot(i) == Knot(i+1))  // nothing to render
				continue;
			if (n > limit)
				n = limit;
			}
			/* Compute the points
			 * ------------------*/

		// Generate the first point (although we may not render it)
        t = rFrom;
		if (rc = EvaluateHomogeneous(t, P[0], w[0]))
			QUIT
		if (!cCache.Started())
    		if (rc = AddPoint(P[0], w[0], t, pTransform, cCache))
				QUIT

		// Generate and render additional degree points
        d = (rNext - t) / n;
 		k = MIN(iDegree, n);
		for (j=1;  j<=k;  j++)
		    {
 			t += d;
			if (rc = EvaluateHomogeneous(t, P[j], w[j]))
				QUIT
    		if (rc = AddPoint(P[j], w[j], t, pTransform, cCache))
				QUIT
    	    }

    	if (n <= iDegree)   // We've generated all the points we needed
    		continue;

		// Replace all points except the last one
		// with the differences of various levels
		for (j=1;  j<=iDegree;  j++)
			for (k=0;  k<=iDegree-j;  k++)
				{
				P[k] = P[k+1] - P[k];
				w[k] = w[k+1] - w[k];
				}

		// Now generate the rest of the points by forward differencing
		for (j=iDegree+1;  j<n;  j++)
		    {
			for (k=1;  k<=iDegree;  k++)
				{
				P[k] += P[k-1];
				w[k] += w[k-1];
				}
			t += d;
    		if (rc = AddPoint(P[iDegree], w[iDegree], t,
							pTransform, cCache))
				QUIT
		    }
	    }

	// Render one at the end (even if evaluation fails!)
	Evaluate(rTo, P[0]);
	if (pTransform)
	   	pTransform->ApplyToPoint(P[0], P[0]);
   	rc = cCache.Accept(P[0], rTo);
exit:
	RETURN_RC(CNURBS::LinearizePart, rc);
	}
//////////////////////////////////////////////////////////////////////
//																	//
//							Evaluators								//

// Evaluator - position only
RC CNURBS::Evaluate(
	double rAt,					// In: Parameter value
	C3Point & rptPoint) const   // Out: Point on the spline there
    {
	RC rc;
	double w;

	if (rc = EvaluateHomogeneous(rAt, rptPoint, w))
		QUIT
	rptPoint /= w;
exit:
	return rc;
	}
/************************************************************************/
// Evaluator - position and derivative
RC CNURBS::Evaluate(
	double rAt,					// In: Parameter value
	C3Point & rptPoint,		    // Out: Point on the spline there
	C3Point & rptTangent) const // Out: Tangent vector at that point
    {
	double r, rValues[MAX_ORDER], rDerivs[MAX_ORDER], w[2];
	int i,j,k;
	RC rc  = m_pKnots->FindSpan(rAt, i);
	if (rc)
		QUIT

	m_pKnots->EvaluateBasis(i, rAt, rValues, rDerivs, NULL);

	j = i - Degree();
	rptPoint = rValues[0] * ControlPoint(j);
	rptTangent = rDerivs[0] * ControlPoint(j++);

	for (k=1;  k<=Degree();  k++, j++)
	    {
		rptPoint += (rValues[k] * ControlPoint(j));
		rptTangent += (rDerivs[k] * ControlPoint(j));
	    }
	if (!Rational())	// We're done
		goto exit;

/* For a rational curve C(t) = F(t) / w(t), we use the formula:

		C' = (F'w - Fw') / w^2 = (F' - C w') / w.
*/

	// Compute the denominator and its derivatives
	j = i - Degree();
	w[0] = rValues[0] * Weight(j);
	w[1] = rDerivs[0] * Weight(j++);

	for (k=1;  k<=Degree();  k++, j++)
	    {
		w[0] += (rValues[k] * Weight(j));
		w[1] += (rDerivs[k] * Weight(j));
	    }
	if (w[0] < FUZZ_GEN)
		{
		rc = E_ZeroVector;
		QUIT
		}
	r = 1 / w[0];

	rptPoint *= r;
	rptTangent = (rptTangent - rptPoint * w[1]) * r;

exit:
	RETURN_RC(CNURBS::Evaluate, rc);
    }
/************************************************************************/
RC CNURBS::Evaluate(
	double rAt,             // In: Where to evaluate
	int nDerivatives,       // In: NUmber of desired derivatives
	P3Point pValues) const  // Out: Position and derivatives
    {
/*
 * WARNINGS:
 * We are computing both the first and second derivatives, whether you
 * need them or not, so pValues MUST point to a space of 3 points!
 *
 * This function cannot evaluate derivatives higher than second
 *
 */
    RC rc;
	double r, rVal[MAX_ORDER], rDeriv[MAX_ORDER], rSecondDeriv[MAX_ORDER];
	double w[3];
	int i,j,k;

    if (nDerivatives > 2)
		{
    	// Function incomplete, can't do more than the second derivative
		rc = FAILURE;
		QUIT
		}

	if (rc = m_pKnots->FindSpan(rAt, i))
		QUIT

	m_pKnots->EvaluateBasis(i, rAt, rVal, rDeriv, rSecondDeriv);

	j = i - Degree();
	pValues[0] = rVal[0] * ControlPoint(j);
	pValues[1] = rDeriv[0] * ControlPoint(j);
	pValues[2] = rSecondDeriv[0] * ControlPoint(j++);

	for (k=1;  k<=Degree();  k++)
	    {
		pValues[0] += (rVal[k] * ControlPoint(j));
		pValues[1] += (rDeriv[k] * ControlPoint(j));
		pValues[2] += (rSecondDeriv[k] * ControlPoint(j++));
	    }

	if (!Rational())	// We're done
		goto exit;

/* For a rational curve, we use the formulae:
		C(t) = F(t) / w(t)
		C' = (F'w - Fw') / w^2 = (F' - C w') / w.

	Now, let G(t) = F' - C w'.  Then C' = G / w, so, by the same token:

		C" = (G' - C'w') / w, where  G'= F" - C'w'- C w", hence
		C" = (F" - 2C'W' - C w") / w
*/

	// Compute the denominator and its derivatives
	j = i - Degree();
	w[0] = rVal[0] * Weight(j);
	w[1] = rDeriv[0] * Weight(j);
	w[2] = rSecondDeriv[0] * Weight(j++);

	for (k=1;  k<=Degree();  k++)
	    {
		w[0] += (rVal[k] * Weight(j));
		w[1] += (rDeriv[k] * Weight(j));
		w[2] += (rSecondDeriv[k] * Weight(j++));
	    }
	if (w[0] < FUZZ_GEN)
		{
		rc = E_ZeroVector;
		QUIT
		}
	r = 1 / w[0];

	pValues[0] *= r;
	pValues[1] = (pValues[1] - pValues[0] * w[1]) * r;
	pValues[2] = (pValues[2] - pValues[1] * (2 * w[1]) - pValues[0] * w[2]) * r;
exit:
	RETURN_RC(CSpline::Evaluate, rc);
    }
/************************************************************************/
// Homogeneous evaluator
RC CNURBS::EvaluateHomogeneous(
	double rAt,						// In: Parameter value
	C3Point & rptPoint,				// Out: Nomerator of the point there
	double & rDenominator) const	// Out: Denominator of the point there
	{
/*
 * First multiply every control pointby its weight, then apply Cox deBoor algorithm.
 * to comtrol points and weignts, and then divide.
 */
	RC rc = SUCCESS;

	//Search for the largest i so that Knot[i] <= rAt < Knot[i+1]
	int i,j,k;
	double s;
	int d = Degree();
	C3Point D[MAX_ORDER];
	double w[MAX_ORDER];
	double t[MAX_ORDER*2];
	if (rc =  m_pKnots->FindSpan(rAt, i))
		QUIT

	// Get the relevant knots, weights and control points into local buffers
	for (j=0;  j<=d;  j++)
		{
		w[j] = Weight(i+j-d);
		D[j] = ControlPoint(i+j-d);
		}
	for (j=0;  j<=2*d+1;  j++)
		t[j] = Knot(i-d+j);

	// At all levels we subdivide the control polygon with the ratios by which
	// rAt subdivide the appropriate knot difference. To understand it best see
	// "A survey of curve and surface methods in CAGD", CAGD 1 (1984) p 16

	for (k=d;  k>0;  k--)
	    {
		for (j=1;  j<=k;  j++)
		    {
			ASSERT(fabs(t[d+j] - t[d-k+j]) > FUZZ_GEN);
			s = (rAt - t[d-k+j]) / (t[d+j] - t[d-k+j]);
			D[j-1] += (D[j] - D[j-1]) * s;
			w[j-1] += (w[j] - w[j-1]) * s;
		    }
	    }
	// When we're done, the point can be found in D[0], and weight in w[0].
	rptPoint = D[0];
	rDenominator = w[0];
exit:
	RETURN_RC(CNURBS::EvaluateHomogeneous, rc);
    }
//////////////////////////////////////////////////////////////////////
//																	//
//					Control Points Finder							//

/* This method is in the core of many spline construction alogorithms. It
 * applies to a half-baked spline, whose degree and knots have been set prior
 * to calling this function.  Some of its control points
 *
 * The missing control points are computed as the solution of a linear system of
 * eqquations.  Each equation is an interpolation condition Spline(t) = Curve(t)
 * at a sample point t.  Each sample point is the average of Degree knots.
 */
RC CNURBS::Approximate(
	const CCurve & Curve, // In: The NURBS to approximate
	int nFirst,           // In: The index of the first missing control point
	int nMissing)         // In: The number of missing control point
	{
	RC rc;
	int i, j, k, m;
	int nSpan, nWrapAround;
	double rValues[MAX_ORDER];

	if (Period())
		nWrapAround = Degree();
	else
		nWrapAround = 0;

	PBanded pBanded = NULL;
	PAugmented pAugmented = NULL;
	PRealSqArray pMatrix = NULL;
	double rMovingSum = 0.0;

	CPointArray P(nMissing, rc);
	CRealArray w(Rational() && !rc ? nMissing : 1, rc);
	if (rc)
		QUIT

	// There can't be more unknowns than the total number of control points
	k = PointCount();
	if (nMissing > k)
		nMissing = k;

	// Set up a matrix for the linear system
	if (nMissing < 2 * Degree() + 1)  // Just use a full square array
		{
		pMatrix = new CFullSqArray(nMissing, rc);
		if (!pMatrix)
			rc = DM_MEM;
		if (rc)
			QUIT
		}
	else // We're going to need a (possibly augmented) banded array
		{
		pBanded = new CBandedArray(nMissing - nWrapAround, Degree(), rc);
		if (!pBanded)
			rc = DM_MEM;
		if (rc)
			QUIT

		if (nWrapAround > 0) // It's an augmented banded array
			{
			pAugmented = new CAugmentedArray(pBanded, nWrapAround, rc);
			if (!pAugmented)
				rc = DM_MEM;
			if (rc)
				QUIT
			pMatrix = pAugmented;
			}

		else // Just a banded array
			pMatrix = pBanded;
		}

	// Get an initial sum of Degree knots for the moving average
	for (i=0;  i<Degree();  i++)
		rMovingSum += Knot(nFirst+i);

	for (i=0;  i<nMissing;  i++)
	    {
		// Get the sample point
		rMovingSum -= Knot(nFirst+i);
		rMovingSum += Knot(nFirst+i+Degree());
		double rSample = rMovingSum / Degree();

		// Load the right hand side
		if (Rational())
			Curve.EvaluateHomogeneous(rSample, P[i], w[i]);
		else
			Curve.Evaluate(rSample, P[i]);

		// Load the interpolation equation into the matrix
		if (rc = m_pKnots->FindSpan(rSample, nSpan))
			QUIT
		m_pKnots->EvaluateBasis(nSpan, rSample, rValues, NULL, NULL);
		for (j=0;  j<=Degree();  j++)
		    {
			m = nSpan - Degree() + j;
			// Get the index relative to the list P
			k = m_pControlPoints->IndexInRange(m - nFirst);
			if (k < nMissing)
				pMatrix->Entry(i, k) += rValues[j];
			else
				{
				P[i] -= ControlPoint(m) * rValues[j];
				if (Rational())
					w[i] -= Weight(m) * rValues[j];
				}
		    }
	    }

	// Solve the equations
	if (rc = pMatrix->LUFactorNoPivot())
		{
		DUMP(*m_pKnots);
		QUIT
		}
	if (rc = pMatrix->LUSolveNoPivotP(P))
		QUIT
	if (Rational())
		if (rc = pMatrix->LUSolveNoPivotR(w))
			QUIT

	// Copy the missing control points and weights in
	for (i=0;  i<nMissing;  i++)
		{
		k = nFirst+i;
		m_pControlPoints->Set(k, P[i]);
		if (Rational())
			SetWeight(i, w[i]);
		}
exit:
	if (pMatrix != pBanded)
		delete pMatrix;
	delete pBanded;
	RETURN_RC(CNURBS::Approximate, rc);
    }
/*****************************************************************************/
// Swap knots and control points with another spline
void CNURBS::SwapDataWith(
    CNURBS & cOther)     // In: The other spline to swap data with
	{
/*
 * This is a special-purpose utility for use for in every spline modification
 * function (e.g. InsertKnot) that needs to evaluate on a spline while working
 * on its modified knots, control points and weights, using ::Approximate. In
 * such circumstances, a working copy of the spline is being created, and its
 * knots, weights and control points are to be taken when we are done.  This
 * method does this, leaving the working copy with the old knots and control
 * points (to be deleted when it's gone)
 */
	SWAP (m_rStart, cOther.m_rStart, double)
	SWAP (m_rEnd, cOther.m_rEnd, double)
	SWAP (m_rPeriod, cOther.m_rPeriod, double)
	SWAP (m_bPlanar, cOther.m_bPlanar, BOOL)

	SWAP (m_pKnots, cOther.m_pKnots, PKnots)
	SWAP (m_pControlPoints, cOther.m_pControlPoints, PControlPoints)
	if (Rational())
		SWAP (m_pWeights, cOther.m_pWeights, PRealArray)
	}
//////////////////////////////////////////////////////////////////////////
//																		//
//					COMPUTING THE EXTENTS OF A SPLINE					//
// This is a 2D operation, computing the extents of the curve's			//
// projection to the XY plane.  However, the curve doesn't have to be	//
// projected for this operation.

// Definition of CNURBSExtentDerivative
class CNURBSExtentDerivative  :  public CRealFunction
{
// Construction destruction
public:
	CNURBSExtentDerivative(
		PNURBS pSpline,					// In: The spline curve
		const C3Point & ptDirection);	// In: The direction vector

	virtual ~CNURBSExtentDerivative();

	PRealFunction Clone() const
		{
		return new CNURBSExtentDerivative(*this);
		}

	// Attributes
public:
	RC Evaluate(
		double rAt,     // In: Parameter value to evaluate at
		double & rDx,   // Out: X' there
		double & rD2x); // Out: X" there

	double Period()
		{
		return m_pSpline->Period();
		}

protected:
//Data
	PNURBS 		m_pSpline;      // The spline
	C3Point		m_ptDirection;	// Direction vector
};

// Implementation of CNURBSExtentDerivative
CNURBSExtentDerivative::CNURBSExtentDerivative(
	PNURBS pSpline,				  // In: The spline curve
	const C3Point & ptDirection)  // In: The direction vector
	{
	m_pSpline = pSpline;
	m_ptDirection = ptDirection;
/*
 * If the spline is periodic then give the iterative algorithms some margin
 * to wrap around
 */
	double rMargin = 3 * m_pSpline->Period();
	m_rStart = m_pSpline->Start() - rMargin;
	m_rEnd = pSpline->End() + rMargin;

	m_rAccuracy = FUZZ_REAL * 1000;
	}

CNURBSExtentDerivative::~CNURBSExtentDerivative()
	{
	}
/***********************************************************************/
// Definition of CNURBSExtentsCache
class CNURBSExtentsCache	:	public CCache
	{
public:
	CNURBSExtentsCache()
		{
		m_pSpline = NULL;
		m_bTighten = TRUE;
		m_pExtents = NULL;
		m_pTransform = NULL;
		m_x = m_dX = m_y = m_dY = 0;
		}

	CNURBSExtentsCache(
		PNURBS pSpline,			// In: The spline in question
		PAffine pTransform,		// In: Transformation (NULL OK)
		CExtents * pExtents)	// In/Out: The extents so far, updated here
		{
		m_pSpline = pSpline;
		m_bTighten = TRUE;
		m_ptX = C3Point(1,0);
		m_ptY = C3Point(0,1);
		m_pTransform = pTransform;
		m_x = m_dX = m_y = m_dY = 0;

		if (pTransform)
			{
			CAffine T;
			if (T.SetInverse(*pTransform))
				m_bTighten = FALSE;
			else
				{
				T.ApplyToVector(m_ptX, m_ptX);
				T.ApplyToVector(m_ptY, m_ptY);
				}
			}
		m_pExtents = pExtents;
		}

	virtual ~CNURBSExtentsCache()
		{
		}

	RC Accept(
		const C3Point & Point,   // In: The point to add
		double t)				 // In: Parametner value
		{
		m_pExtents->Update(Point);

		double d = Point.X() - m_x;
		if (d * m_dX < 0)
			Tighten(m_ptX, t);
		m_x = Point.X();
		m_dX = d;

		if (d * m_dY < 0)
			Tighten(m_ptY, t);
		m_y = Point.Y();
		m_dY = d;

		return SUCCESS;
		}

	BOOL Started()
		{
		return FALSE;
		}

	void Tighten(
		C3Point & ptDirection,	// In: Direction of extent
		double seed)			// In: An initial guess for the parameter
		{
		double r;
		C3Point P;
		CNURBSExtentDerivative DExtent(m_pSpline, m_ptX);
		if (!DExtent.NewtonRaphson(seed, r))
			{
			// A zero was found
			if (m_pSpline->PullIntoDomain(r))
				{
				// Update extent with this local extremum
				if (!m_pSpline->Evaluate(r, P))
					m_pExtents->Update(P, m_pTransform);
				}
			}
		}

protected:
	PNURBS m_pSpline;	  // The spline in question
	C3Point m_ptX;		  // Transformed X direction
	C3Point m_ptY;		  // Transformed Y direction
	PAffine m_pTransform; // Transformation
	PExtents m_pExtents;  // Tne extents we are updating
	BOOL m_bTighten;	  // Do the exact numerical method if TRUE
	double m_x;			  // Current x
	double m_dX;		  // current x - previous x
	double m_y;			  // Current x
	double m_dY;		  // current x - previous x
	};
/************************************************************************/
// Evaluate the first 2 derivatives of x on a spline curve
RC CNURBSExtentDerivative::Evaluate(
	double rAt,    // In: Parameter value to evaluate at
	double & rDX,  // Out: The first derivative
	double & rD2X) // Out: The second derivative
    {
	C3Point P[3];
	RC rc = m_pSpline->Evaluate(rAt, 2, P);
	rDX = P[1] * m_ptDirection;
	rD2X = P[2] * m_ptDirection;

	RETURN_RC(CNURBSExtentDerivative::Evaluate, rc);
    }
// End of definition and implementation of CNURBSExtentDerivative

/************************************************************************/
// Update bounding box in a given direction
RC CNURBS::UpdateExtents(
	BOOL bTight,			// In: Tight extents if =TRUE
	BOOL bIgnoreStart,		// In: Ignore the startpoint, if =TRUE
	CExtents & cExtents,	// In/out: The extents, updated here
	PAffine pTransform)		// Optional: Transformation (NULL OK)
	{
	RC rc = SUCCESS;
	int i;

	// Get the loose extents from the control points
	CExtents loose;
	for (i=0;  i<PointCount();  i++)
		{
		C3Point P = ControlPointGeom(i);
		if (pTransform)
			pTransform->ApplyToPoint(P, P);
		loose.Update(P);
		}

	if (bTight)
		{
		double dX = loose.MaxX() - loose.MinX();
		double dY = loose.MaxY() - loose.MinY();
		CNURBSExtentsCache cache(this, pTransform, &cExtents);
		rc = Linearize(MAX(dX, dY) / 100,  pTransform, cache);
		}

	if (rc)
		{
		// Fall back to the non-tight option
		bTight = FALSE;
		rc = SUCCESS;;
		}

	if (!bTight)
		{
		// Get the loose extents from the control points
		for (i=0;  i<PointCount();  i++)
			{
			C3Point P = ControlPointGeom(i);
			if (pTransform)
				pTransform->ApplyToPoint(P, P);
			cExtents.Update(P);
			}
		}

	RETURN_RC(CNURBSe::UpdateExtents, rc);
	}
/*******************************************************************/
// Update x or y extents with the spline's x/y extents
RC CNURBS::UpdateDirectionExtents(
	const C3Point & vecDirection, // In: The direction vector
	CExtents & cExtents,		  // In/out: The extents, updated here
	PAffine pTransform)			  // Optional: Transformation (NULL OK)
    {
	RC rc = SUCCESS;
	int i;
	double rCurrentD, rPreviousD;
	C3Point ptP;
	int nFrom = 0;
	int nTo = PointCount();
	BOOL bPeriodic = (Period() != 0);

	// Update at all potential cusps, including endpoints
	double rThis = Knot(0);
	double rNext;
	int nMultiplicity = 1;
	for (i=1;  rThis <= m_rEnd + FUZZ_GEN;  i++)
		{
		rNext = Knot(i);
		if (rNext == rThis)
			nMultiplicity++;
		else
			{
			if (nMultiplicity >= Degree())
				{
				if (rc = Evaluate(rThis, ptP))
					QUIT
				cExtents.Update(ptP, pTransform);
				}
			nMultiplicity = 1;
			rThis = rNext;
			}
		}

	// Look for roots of the derivative of the coordinate
	if (bPeriodic)
		{
		// It's a periodic spline, so the control polygon wraps aroune
		nFrom--;
		nTo++;
		}

	rThis = ControlPointGeom(nFrom) * vecDirection;
	rNext = ControlPointGeom(nFrom+1) * vecDirection;
	rCurrentD = rNext - rThis;

		// Search for zeros of the coordinate
	for (i=nFrom+2;  i<nTo;  i++)
		{
		int d = Degree();
		int j,k;
		CNURBSExtentDerivative DExtent(this, vecDirection);
		double r;
		C3Point P;

		// Move on
		rPreviousD = rCurrentD;
		rThis = rNext;
		rNext = ControlPointGeom(i) * vecDirection;
		rCurrentD = rNext - rThis;

		if (rCurrentD * rPreviousD > FUZZ_DIST)
			continue;
/*
 * The ControlPoint(i) is a local extremum in the control polygon.  By the
 * Variation Diminishing property, this suggests a potential
 * zero of the derivative in the vicinity.  Let's search for it within the
 * support of the corresponding control point
 */
		// Get the average of Degree knots
		k = i + d;
		for (j = i;  j < k;  j++)
			{
			// Solve the equation Coordinate'=0 with Knot(j) as seed
			if (!(rc = DExtent.NewtonRaphson(Knot(j), r)))
				{
#ifdef DEBUG
				DB_MSGX("seed=%lf, root=%lf",(Knot(j), r));
#endif // DEBUG
				// A zero was found
				if (!PullIntoDomain(r))
					// But it's outside the domain
					continue;

				// Update extent with this local extremum
				if (rc = Evaluate(r, P))
					QUIT
				cExtents.Update(P, pTransform);
			    }
			else
				if (rc == E_NOMSG)  // No root found,
					rc = SUCCESS;   // but it's OK.
				else
					QUIT          // It's a serious error
				}
			}
exit:
	RETURN_RC(CNURBS::UpdateDirectionExtents, rc);
    }
//////////////////////////////////////////////////////////////////////
//																	//
//							Miscellaneous							//

// Insert a knot to a given continuity
RC CNURBS::InsertKnot(
    double & rAt,     // In: Parameter value for the inserted knot
                      // Out: An existing knot within tolerance, if there is one
	int nContinuity,  // In: The desired continuity at the knot
	int & nFirst,     // Out: The index of the first inserted knot
	int & nAdded)     // Out: The number of knots actually added
    {
/*
 * This unorthodox algorithm is much less efficient than the ones you will find in
 * textbooks, but it is much easier to understand and less error-prone, hence preferable
 * for most applications.  Code the Boehm algoorithm if you really need a fast one.
 * After the obvious, which is the actual insertion of the new knots in the list,
 * knots and making room for addtional control points in the list of control points,
 * this algorithm computes new control points by calling the general "black box"
 * control-points finder Approximate. The only real question is how many control
 * points need to be recomputed.
 */
	RC rc;

	// Make a temporary copy of this spline.
	CNURBS Copy(*this, rc);
	if (rc)
		QUIT

	// Modify the knots
	if (rc = Copy.m_pKnots->InsertKnot(rAt, nContinuity, nFirst, nAdded))
		QUIT
	if (nAdded<=0)   // There is nothing to add
		goto exit;

	// Make room for new control points
	if (rc = Copy.m_pControlPoints->MakeRoom(nFirst - 1, nAdded))
		QUIT
	if (rc = Copy.m_pKnots->MakeRoom(nFirst - 1, nAdded))
		QUIT

	// Compute the new control points and the old ones that changed
	if (rc = Copy.Approximate (*this, nFirst - Degree(), nAdded + Degree() - 1))
		QUIT

	// Take the new knots and control points
	SwapDataWith(Copy);
	// The old knots and control points will go away with Copy

exit:
	RETURN_RC(CNURBS::InsertKnot, rc);
    }

/**********************************************************************/
// Integrate a real function along the spline
RC CNURBS::Integrate(
	CRealFunction &  cFunction, // In: The function
	double rFrom,               // In: Lower integration bound
	double rTo,                 // In: Upper integration bound
	double & rIntegral) const   // Out: The integral
    {
// Call CCurve::Integrate for every nontrivial span
	RC rc = SUCCESS;
	double a = rFrom;
	double b;
	rIntegral = 0.0;
	int i;

	// This implementation is no good for extended domain
	if (Period())
		{
		if (rTo > rFrom + Period() + FUZZ_GEN)
		rc = E_BadInput;
		}
	else if (rFrom < Knot(0) - FUZZ_GEN	||
			 rTo > Knot(LastKnot()) + FUZZ_GEN)
		rc = E_BadInput;
	if (rc)
		QUIT

	// Initialize;
 	if (rc = m_pKnots->FindSpan(rFrom, i))
 		QUIT

	// Now loop over spans and integrate
	while (a < rTo)
		{
		i++;
		double r = Knot(i);
		b = MIN(r, rTo);
		if (b == a)
			continue;

		if (rc = CCurve::Integrate(cFunction, a, b, r))
			QUIT
		rIntegral += r;

		a = b;
		}
exit:
	RETURN_RC(CNURBS::Integrate, rc);
	}
/*******************************************************************************/
// Find all self intersections
RC CNURBS::SelfIntersect(
	CRealArray & cParameters, // In/Out: List of self intersection parameters
	double rTolerance)		  // Optional: Tolerance
// Self overlaps are not supported
	{
	double b, c, s, t, ds, dt, t1, t2;
	int d = Degree() + 1;
	int i, j, k, m;
	C3Point P;
	RC rc = SUCCESS;

	if (d < 1)
		goto exit;

	// i & t loop
	for (i = 0,  t = m_rStart;
		 t < m_rEnd  &&  i < PointCount();
		 i++, t = b)
	    {
		b = MIN(Knot(i+1), m_rEnd);
		if (b <= t)
			continue;

		// j & s loop
		for (j = 0,  s = m_rStart;
			 s < m_rEnd  &&  j < PointCount();
			 j++, s = c)
			{
			c = MIN(Knot(j+1), m_rEnd);
			if (c <= s)
				continue;

			// Skip if the 2 spans live in disjoint boxes
			if (DisjointSpans(i, j))
				continue;

			// For Degree() seeds t from span i...
			dt = (b - t) / d;
			for (k=1;  k<d;  k++)
				{
				t += dt;

				// For Degree() seeds from span j...
				ds = (c - s) / d;
				for (m=1;  m<d;  m++)
					{
					s += ds;

					// Solve the equations for an intersection
					rc = Intersect(this, this, t, s, t1, t2, rTolerance);
					if (rc)
						if (rc == E_NOMSG)
							{				 // Potential overlap
							rc = SUCCESS;    // but that's OK
							continue;
							}
						else				 // Real problem
							QUIT

					// Add to the list
					if (!SnapParameter(t1, rTolerance, t2))
						{
						if (rc = cParameters.AddSort(t1))
							QUIT
						if (rc = cParameters.AddSort(t2))
							QUIT
						}
					}  // for m
				}	// for k
			s = c;
			}	 //for j
		t = b;
		}	 // for i

	// Test for end intersections
	t = End() - Start();
	if (FEQUAL(Period(), t, t * FUZZ_GEN))
		// A periodic spline has no start and end
		goto exit;

	// Check intersection with startpoint
	GetStartPoint(P);
	if (!PullBack(P, P, t, s, Start()))
		if (s < rTolerance)
			{
			if (rc = cParameters.AddSort(Start()))
				QUIT
			if (rc = cParameters.AddSort(t))
				QUIT
			}

	// Check intersection with endpoint
	GetEndPoint(P);
	if (!PullBack(P, P, t, s, End()))
		if (s < rTolerance)
			{
			if (rc = cParameters.AddSort(End()))
				QUIT
			if (rc = cParameters.AddSort(t))
				QUIT
			}
exit:
	RETURN_RC(CNURBS::SelfIntersect, rc);
	}
/*******************************************************************************/
// Do these 2 spans live in disjoint boxes?
BOOL CNURBS::DisjointSpans(
	int  i,			// In: First span index
	int  j) const   // In: Second span index
// Return TRUE if the spans may overlap
	{
	CExtents cI, cJ;
	int k;

	if (abs(i - j) <= Degree())
		return TRUE;

	for (k = i - Degree();    k <= i;    k++)
		cI.Update(ControlPoint(k));

	for (k = j - Degree();    k <= j;    k++)
		cJ.Update(ControlPoint(k));

	return !cI.Overlap(cJ);
	}

/****************************************************************************/
// Breaks in continuity of the nth derivative
RC CNURBS::Breaks(
	int iDeriv,                 // In: The required continuity
	PRealArray & pBreaks) const //Out: The breaks
/*
 * All curves are treated as periodic, so even if the curve is open,
 * its first break (at Start) will NOT be repeated at the end.  Thus, a
 * smooth open curve will show ONE break.
 */
	{
	return m_pKnots->Breaks(m_rStart, m_rEnd, iDeriv, pBreaks);
	}
/************************************************************************/
// Seeds for equations solving
RC CNURBS::Seeds(
	CRealArray & arrSeeds,  // In/Out: Array of seeds
	double* pStart,			// In: Domain start (optional, NULL OK)
	double* pEnd) const 	// In: Domain end (optional, NULL OK)
    {
	double rFrom, rTo;
	double rLastKnot = Knot(LastKnot());
	RC rc = SUCCESS;
	int d = Degree() + 1;
	int i;

	double a = m_rStart;
	double b = m_rEnd;

	if (pStart)
		a = *pStart;
	if (pEnd)
		b = *pEnd;

	if (d < 1)
		goto exit;

	rFrom = Knot(0);

	if (Period())
		rLastKnot = a + Period();
	else if (a < rFrom)
		// The domain starts before the first knot, so add -
		{
		// one seed half knot-interval back
		if (rc = arrSeeds.Add(rFrom - m_pKnots->Interval() / 2))
			QUIT

		// another seed at the first knot
		if (rc = arrSeeds.Add(rFrom))
			QUIT
		}

	// Seed the knots domain
	if (rFrom < a)
		// The spline is trimmed
		rFrom = a;
	for (i = 0;   rFrom < b  &&  rFrom < rLastKnot;   i++)
	    {
		rTo = MIN(Knot(i), b);
		if (rTo <= rFrom)
			continue;

		// Take Degree() seeds in each span
		double dt = (rTo - rFrom) / d;
		for (int j=1;  j<d;  j++)
			{
			rFrom += dt;
			if (rc = arrSeeds.Add(rFrom))
				QUIT
			} // End for loop on seeds in a span
		rFrom = rTo;
		}	// End for loop on the spans


	if (!Period()  &&  b > rLastKnot)
		// The domain extends beyond the last knot, so add -
		{
		// One seed at the last knot
		if (rc = arrSeeds.Add(rLastKnot))
			QUIT

		// Another seed half knot-interval further out
		if (rc = arrSeeds.Add(rLastKnot + m_pKnots->Interval() / 2))
			QUIT
		}

exit:
	RETURN_RC(CNURBS::Seeds, rc);
    }
/*******************************************************************************/
// Pack the spline
RC CNURBS::Pack(
	LPBYTE & pStorage)  const // In/Out: The storage
	// pStorage will be advanced to the next available storage space
	{
	RC rc;

	// This method is only used for commands that manipulate non-rational
	// non-rationals splines
	ASSERT(!Rational());

	// Pack the type
	CURVETYPE * pType = (CURVETYPE *)pStorage;
	*pType = TYPE_NURBS;
	pStorage += sizeof(CURVETYPE);

	// Pack the domain and period
	double* pR = (double*)pStorage;
	pR[0] = m_rStart;
	pR[1] = m_rEnd;
	pR[2] = m_rPeriod;
	pStorage += 3*sizeof(double);

	// Knots and control points
	if (rc = m_pKnots->Pack(pStorage))
		QUIT

	rc = m_pControlPoints->Pack(pStorage);
exit:
	RETURN_RC(CNURBS::Pack, rc);
	}
/*******************************************************************************/
// Packing Size
int CNURBS::PackingSize() const
	{
	// This method is only used for commands that manipulate non-rational
	// non-rationals splines
	ASSERT(!Rational());

	return sizeof(CURVETYPE) + 3 * sizeof(double) +
		m_pKnots->PackingSize() + m_pControlPoints->PackingSize();
	}
/*******************************************************************************/
// Snap the spline closed
RC CNURBS::SnapClosed(
	double rTolerance)
	{
	RC rc = SUCCESS;
	C3Point P = ControlPointGeom(0);

	if (Period())	// It's already closed
		goto exit;

	if (PointDistance(ControlPointGeom(PointCount() - 1), P) < rTolerance)
		// Snap the end to the beginning
		m_pControlPoints->Set(PointCount() - 1, P);
exit:
	RETURN_RC(CNURBS::SnapClosed, rc);
	}
/************************************************************************/
// Snap the end point to a given point
RC CNURBS::SnapEnd(
	SIDE nWhichEnd,   	   // In: Start or end
	const C3Point & ptTo)  // In: The point to snap to
	{
	// This logic is valid only if the spline is untrimmed!
	ASSERT(m_rStart == Knot(0) && m_rEnd == Knot(LastKnot()));

	if (nWhichEnd == LEFT)
		m_pControlPoints->Set(0, ptTo);
	else
		m_pControlPoints->Set(PointCount() - 1, ptTo);

	RETURN_RC(CNURBS::SnapEnd, SUCCESS);
	}
/*******************************************************************************/
// Construct a copy reparameterized to arc length
RC CNURBS::ArcLengthCopy(
	double rFrom,	// In: Domain start
	double rTo,		// In: Domain end
	PCurve & pCopy)	// Out: A constant speed copy
	{
/*
 * This function computes the procedural combination of this curve with a
 * parameter mapping f so that the speed (=length of derivative vector) is
 * equal to 1.  The mapping f is actually the inverse of the length function
 * It does so by approximating the function with a sample of degree values
 * on every span
 */
	RC rc;
	double rNext, r;
	int i, j, iSpan;
	CSpeed cSpeed(this);
	CRealArray cT;	// Equally spaced parameter values in the curve's domain
	CRealArray cS;	// Accumulated length at cT[i]
	CRealArray cDT;	// The derivative dt/ds at cT[i]
	PRationalFunction pMapping = NULL;
	pCopy = NULL;

	// Get the data on the first sample point
	if (rc = cT.Add(rFrom))
		QUIT
	if (rc = cS.Add(0))
		QUIT
	if (rc = cSpeed.EvaluateInverse(cT[0], r))
		QUIT
	if (rc = cDT.Add(r))
		QUIT;

	// Locate the knot-span we start from
 	if (rc = m_pKnots->FindSpan(rFrom, iSpan))
 		QUIT

	// Now loop over spans and render them
	for (i = iSpan;
		 rFrom < rTo;
		 rFrom = rNext, i++)
	    {
		rNext = MIN(rTo, Knot(i+1));
		double dt = rNext - rFrom;
		if (dt < FUZZ_GEN)
			// Skip this degenerate span
			continue;
		dt /= Degree();

		// Loop on the sample and load the data arrays
		for (j = 1;   j <= Degree();   j++)
			{
			double ds;
			double rPreviousT = cT.Last();

			if (rc = cT.Add(rPreviousT + dt))
				QUIT
			if (rc = CCurve::Integrate(cSpeed, rPreviousT, cT.Last(), ds))
				QUIT
			if(cS.Add(cS.Last() + ds))
				QUIT

			// Get ds/dt
			if (rc = cSpeed.EvaluateInverse(cT.Last(), r))
				QUIT
			if (rc = cDT.Add(r))
				QUIT
			}
		}

	// Construct the copy
	pMapping = new CRationalFunction(cS, cT, cDT, rc);
	if (ALLOC_FAIL(pMapping))
		rc = DM_MEM;
	if (rc)
		QUIT
/*
#ifdef DEBUG
	// Validation
	if (rc)
		QUIT
	pMapping->Dump();
	DB_MSGX("Validate the mapping\n");
	srand(0);
	for (i = 0;  i < 10;  i++)
		{
		double s, r;
		// Get the length s at a random parameter value t
		double t = Start() + FRAND * (End() - Start());
		s = 0;
		for (int j=0;  j<10;  j++)
			{
			double ds;
			double dt = (t - Start()) / 10;
			Integrate(cSpeed, Start() + j * dt, Start() + (j+1) * dt, ds);
			s += ds;
			}

		// The value of the mapping at s should be equal to t
		pMapping->Evaluate(s, r);
		DB_MSGX("t=%lf  round trip=%lf",t, r);
		}
#endif
*/
	// Construct the reparameterized copy
	rc = ALLOC_FAIL(pCopy = new CMappedCurve(this, pMapping));
	pMapping = NULL;
exit:
	delete pMapping;
	if (rc)
		{
		delete pCopy;
		pCopy = NULL;
		}
	RETURN_RC(CNURBS::ArcLengthCopy, rc);
	}
/*******************************************************************************/
RC CNURBS::NextCusp(  // Return E_NOMSG if there is no cusp
	double t,		// In: A paramater value
	double & rCusp)	// Out: Parameter value of the next cusp after t, or curve-end
	{
	RC rc = E_NOMSG;
	rCusp = t;
	int n;

	// Find the first knot after t
	if (m_pKnots->FindSpan(t, n))
		QUIT
	n++;

	// Loop over the knots
	while (rCusp < m_rEnd)
		{
		// Get the multiplicity of this knot
		for (int i=n;   Knot(i) == rCusp;   i++);  // This "for" loop has no body!

		// Is it a cusp?
		if (i >= n + Degree())
			{
			// Perhaps it is, examine the control points
			if (!Collinear(ControlPointGeom(n), ControlPointGeom(n-2),
						   ControlPointGeom(n-1),FUZZ_DIST))
				{
				rc = SUCCESS;  // Yes, this is a cusp
				break;
				}
			else
				{  // must move to the next knot
				n = i;
				rCusp = Knot(n);
				continue;
				}
		}
		else
			{
			// No way, move to the next knot
			n = i;
			rCusp = Knot(n);
			continue;
			}
		}
exit:
	RETURN_RC(CNURBS::NextCustp, rc);
	}
/*******************************************************************************/
// Initiate a stored path with this spline
RC CNURBS::InitiatePath(
	PStorage pStorage) const	// In/Out: The storage
	{
	RC rc = pStorage->InitiateNURBS(this);
	RETURN_RC(CNURBS::InitiatePath, rc);
	}
/*******************************************************************************/
// Persist this spline
RC CNURBS::Persist(
	PStorage pStorage) const	// In/Out: The storage
	{
	RC rc = pStorage->AcceptNURBS(this);
	RETURN_RC(CNURBS::Persist, rc);
	}
/*******************************************************************************/
// Return an approximation with one of the Visio curves
RC CNURBS::GetApproximation(
	double rTolerance,		// In: Approxiation tolerance
	PCurve & pApprox) 		// Out: The approximation
	{
	RC rc = ALLOC_FAIL(pApprox = Clone());
	RETURN_RC(CNURBS::GetApproximation, rc);
	}
/*****************************************************************************/
// Compare the radius of curvature at a point to the offset distance
LOCAL RC CompareToCurvature(// Return E_BadOffset if rOffset < curvature
	PCurve pCurve,	// In: A curve
	double rAt,		// In: Parameter value
	double rOffset)	// In: The inverse of the (signed) offset distance
	{
	double rCurvature;
	RC rc = SUCCESS;

	if (rAt < pCurve->Start() || rAt > pCurve->End())
		// This parameter value is irrelevant, it's outside the domain
		goto exit;

	// Get the curvature
	if (rc = pCurve->GetCurvature(rAt, rCurvature))
		QUIT

	if (rCurvature * rOffset < 0)
		// Outward offset is always acceptable
		goto exit;

	if (fabs(rOffset) < fabs(rCurvature))
		rc = E_BadOffset;
exit:
	RETURN_RC(CompareToCurvature, rc);
	}
/*****************************************************************************/
// Check if this offset distance will not cusp & do ugly things
RC CNURBS::AdmitOffset(// Return E_BadOffset if this offset distance is no good
	double rDistance,		// In: The offset distance
	double rTolerance,		// In: The acceptable error
	P3Point pNormal)		// In: The Up direction (NULL OK)
	{
/*
   The offset will cusp and start going backwards where the radius of curvature
   is smaller than the offset distance.  Here we (rather crudely) detect that by
   sampling the spline for excess curvature.
 */
	RC rc = SUCCESS;
	INT n = 10 * Degree();
	double t = Knot(0);
	double tNext, dt;
	int i, j;

	if (n < 1)
		{
		// Should never happen!
		rc = E_BadInput;
		QUIT
		}

	if (FZERO(rDistance, FUZZ_GEN))
		// Zero offset is always acceptable
		goto exit;
	// We'll talk crvature = inverse of radius of curvature, so
	rDistance = 1 / rDistance;

	// Sample at curve start
	if (rc = CompareToCurvature(this, t, rDistance))
		QUIT

	tNext = Knot(0);
	for (i = 0;  i < PointCount();  i++)
		{
		t = tNext;
		tNext = Knot(i+1);
		dt = (tNext - t) / n;
		if (FZERO(dt, FUZZ_GEN))
			continue;

		// Sample n times in this span
		for (j = 1;  j <=n;  j++)
			{
			if (rc = CompareToCurvature(this, t, rDistance))
				QUIT
			t += dt;
			}
		}
exit:
	RETURN_RC(CNURBS::AdmitOffset, rc);
	}
/*****************************************************************************/
// Check if this spline is planar, and compute a normal if it is
BOOL CNURBS::GetNormal(	// Return TRUE if the spline is planar
	C3Point & ptNormal)		// Out: A unit normal if it is
	{
	// for now
	ptNormal = C3Point(0,0,1);
	return TRUE;
	}
/************************************************************************/
// Set the domain
RC CNURBS::SetDomain(
	double rFrom,  // In: New curve start
	double rTo)    // In: New curve end
	{
	RC rc = FAILURE;

	if (rFrom < rTo - FUZZ_DIST)
		{
		rc = SUCCESS;
		m_rStart = rFrom;
		m_rEnd = rTo;
		}
	RETURN_RC(CNURBS::SetDomain, rc);
	}
/************************************************************************/
// Reset the domain interval's start
RC CNURBS::ResetStart(
	double rNew)  // In: New curve start
	{
	RC rc = FAILURE;

	if (rNew <= m_rEnd)
		{
		rc = SUCCESS;
		m_rStart = rNew;
		}
	RETURN_RC(CNURBS::ResetStart, rc);
	}
/************************************************************************/
// Reset the domain interval's end
RC CNURBS::ResetEnd(
	double rNew)  // In: New curve end
	{
	RC rc = FAILURE;

	if (rNew >= m_rStart)
		{
		rc = SUCCESS;
		m_rEnd = rNew;
		}
	RETURN_RC(CNURBS::ResetEnd, rc);
	}

#ifdef DEBUG
/*******************************************************************************/
// Dump
void CNURBS::Dump() const
	{
	DB_MSGX("NURBS, Start=%lf, End=%lf, Period=%lf\n",(Start(), End(), Period()));
	m_pKnots->Dump();
	m_pControlPoints->Dump();
	if (m_pWeights)
		m_pWeights->Dump();
	}
#endif // DEBUG
//////////////////////////////////////////////////////////////////////////
//																		//
//								Constructions							//
//																		//
//////////////////////////////////////////////////////////////////////////

// Construct a periodic interpolating spline
RC GetPeriodicSpline(
	PKnots pNodes,			// In: Knos as interpolation nodes
	PPointArray pPoints,	// In: Interpolation points (last one ignored)
	PNURBS & pSpline)		// Out: Interpolating spline
	{
	RC rc,rc1;
	INT i;
	double rValues[4];
	PKnots pKnots = NULL;
	PControlPoints pControlPoints = NULL;

	// Create an augmented array for the equations
	INT n = pPoints->Size() - 1;
	CBandedArray arrBanded(n-1,1, rc);
	CAugmentedArray A(&arrBanded, 1, rc1);
	if (rc)
		QUIT
	if (rc = rc1)
		QUIT

	// Construct the knots and control points
	pKnots = new CKnots(*pNodes, rc);
	if (!pKnots)
		rc = DM_MEM;
	if (rc)
		QUIT
	pControlPoints = new CControlPoints(*pPoints, rc);
	if (!pControlPoints)
		rc = DM_MEM;
	if (rc)
		QUIT
	// Ignore the last point, assumng it's a repetition of the first
	pControlPoints->Remove(pControlPoints->Size() - 1, 1);

	/* Set the equations  Most of the array is tridiagonal, except for two entries:
	 * the entry that was supposed to get in the [0][-1] position goes at [0][n],
	 * and the one that would go at the [n][n+1] position goes at the [n][0] place.
	 * These are the entries that make it and augmented (rather than banded) array.
     *
	 * To get the correct matrix structure, the first equation needs to express the
	 * interpolation condition through Point[2].  We therefore need to shift the
	 * interpolation points list to start there
	 */
	if (rc = pControlPoints->StartAt(2))
		QUIT

	// Load the interpolation conditions
	for (i=0;  i<n;  i++)
		{
		pKnots->EvaluateBasis (i+2, pKnots->Knot(i+2), rValues);
		A.Entry(i,i-1) = rValues[0];
		A.Entry(i,i) = rValues[1];
		A.Entry(i,i+1) = rValues[2];
		}

    // Solve the equations
    if (rc = A.LUFactorNoPivot())
    	QUIT
    if (rc = A.LUSolveNoPivotP(*pControlPoints))
    	QUIT

	// Cnstruct the spline
	pSpline = new CNURBS(pKnots, pControlPoints, rc);
	if (!pSpline)
		rc = DM_MEM;
	if (rc)
		QUIT
exit:

	// delete those pointers if they are not owned by splines
	//
	if (rc && !pSpline) {
		delete pKnots;
		delete pControlPoints;
	}

	RETURN_RC(CNURBS::InvertPeriodic, rc);
    }

//////////////////////////////////////////////////////////////////////////
//																		//
//							Ray interceptions							//

// Base clase
class CNURBSInterceptor	:	public CProber, public CInterceptor
	{
public:
	// Constructors/destructors
	CNURBSInterceptor()
		{
		}

	virtual ~CNURBSInterceptor()
		{
		}

	CNURBSInterceptor(
		PNURBS pSpline,		// In: The spline
		PLine pLine,		// In: The line to be intercepted
		RC & rc)			// Out: Return code
		:CProber(pSpline), CInterceptor(pLine, rc)
		{
		}

	// Methods
	virtual RC ProcessOneIntercept(
		double t)=0;	// In: Parameter value of the intercept on the line

	RC ProbeAt(
		C3Point P,		 // In: A new probe point
		double weight,	 // In: The weight of this point
		double t,		 // In: Parameter value there
		BOOL* pDone)	 // Ignored here
		{
		RC rc = SUCCESS;
		double rOnLine, rOnSpline;

		if (m_pSpline->Rational())
			P /= weight;

		if (FindIntercept(P, rOnLine, rOnSpline))
			{
			// Try this as a seed for an exact intersection
			rOnSpline = rOnSpline * m_t + (1 - rOnSpline) * t;
			if (rc = Intersect(m_pLine, m_pSpline, rOnLine, rOnSpline,
												rOnLine, rOnSpline))
				{
				if (rc == E_NOMSG)
					rc = SUCCESS;
				QUIT
				}
			rc = ProcessOneIntercept(rOnLine);
			}
exit:
		// Update
		m_t = t;
		RETURN_RC(CNURBSInterceptor::ProbeAt, rc);
		}
	// No data
	};
/***********************************************************************/
// Intercepts counter
class CNURBSInterceptsCounter	:	public CNURBSInterceptor, public CRealArray
	{
public:
	// Constructors/destructors
	CNURBSInterceptsCounter()
		{
		}

	virtual ~CNURBSInterceptsCounter()
		{
		}

	CNURBSInterceptsCounter(
		PNURBS pSpline,	// In: The spline
		PLine pLine,	// In: The ray to be intercepted
		RC & rc)		// Out: Return code
		:CNURBSInterceptor(pSpline, pLine, rc)
		{
		}

	// Methods
	RC ProcessOneIntercept(
		double t)	// In: Parameter value of the intercept on the line
		{
		int n;
		RC rc = SUCCESS;

		// Is this a new ray intersection?
		if (t > FUZZ_GEN)
			{
			// Yes it is, add it to the list
			if (!Search(t, FUZZ_GEN, n))
				rc = Insert(t, n+1);
			}

		RETURN_RC(CNURBSInterceptCounter::ProcessOneIntercept, rc);
		}

	// No data
	};
/***********************************************************************/
// Nearest and farthest intercepts finder
class CNearestIntercept	:	public CNURBSInterceptor
	{
public:
	// Constructors/destructors
	CNearestIntercept()
		{
		m_pNearFore = m_pFarFore = m_pNearBack = m_pFarBack = NULL;
		m_bIgnoreOrigin = FALSE;
		}

	virtual ~CNearestIntercept()
		{
		}

	CNearestIntercept(
		PNURBS pSpline,			// In: The spline
		PUnboundedLine pLine,	// In: A line
		BOOL bIgnoreOrigin,		// In: Ignore the origin if TRUE
		double * pNearFore,		// In/out: Nearest forward intercept so far
		double * pFarFore,		// In/out: Farthest forward intercept so far
		double * pNearBack,		// In/out: Nearest backword intercept so far
		double * pFarBack,		// In/out: Farthest backword intercept so far
		RC & rc)				// Out: Return code
		:CNURBSInterceptor(pSpline, pLine, rc)
		{
		m_pNearFore = pNearFore;
		m_pFarFore = pFarFore;
		m_pNearBack = pNearBack;
		m_pFarBack = pFarBack;
		m_bIgnoreOrigin = bIgnoreOrigin;
		}

	// Methods
	RC ProcessOneIntercept(
		double t)	// In: Parameter value of the intercept on the line
		{
		// An intersection found, update the current nearest/farthest
		UpdateNearAndFar(t, m_bIgnoreOrigin, m_pNearFore, m_pFarFore,
											m_pNearBack, m_pFarBack);
		return SUCCESS;
		}
	// Data
protected:
	double * m_pNearFore;	// The nearest forward intercept so far
	double * m_pFarFore;	// The farthest forward intercept so far
	double * m_pNearBack;	// The nearest backword intercept so far
	double * m_pFarBack;	// The farthest backword intercept so far
	BOOL m_bIgnoreOrigin;	// Ignore intercepts at the origin if TRUE
	};
/*******************************************************************************/
// Update the number of intercpts with a given ray
RC CNURBS::UpdateIntercepts(
	PRay pRay,			// In: A ray
	int	& nIntercepts)  // In/out: The number of intercepts
	{
	RC rc;
	CNURBSInterceptsCounter interceptor(this, pRay, rc);
	if (rc)
		QUIT
	if (rc = interceptor.Linearize())
		QUIT
	nIntercepts += interceptor.Size();
exit:
	RETURN_RC(CNURBS::UpdateIntercepts, rc);
	}
/*******************************************************************************/
// Update the nearest and farthest intercpts with a given ray
RC CNURBS::UpdateIntercepts(
	PUnboundedLine pLine, // In: A line
	BOOL bIgnoreOrigin,	  // In: Ignore the origin if TRUE
	double * pNearFore,   // In/out: Forward intercept nearest to line's origin
	double * pFarFore,    // In/out: Forward intercept farthest from line's origin
	double * pNearBack,	  // In/out: Backward intercept nearest to line's origin
	double * pFarBack)    // In/out: Backward intercept farthest from line's origin
	{
	RC rc;
	CNearestIntercept interceptor(this, pLine, bIgnoreOrigin, pNearFore,
									pFarFore, pNearBack, pFarBack, rc);
	if (!rc)
	rc = interceptor.Linearize();
	RETURN_RC(CNURBS::UpdateIntercepts, rc);
	}
//////////////////////////////////////////////////////////////////////////
//																		//
//								Hit test								//

class CHitTestCache	:	public CCache
	{
public:
	// Constructors/destructors
	CHitTestCache()
		{
		m_rSqMargin = m_rMargin = 0;
		m_bHit = FALSE;
		m_t = 0;
		m_pSpline = NULL;
		}

	virtual ~CHitTestCache()
		{
		}

	CHitTestCache(
		PNURBS pSpline,			// In: The spline
		const C3Point& ptTo,	// In: The point
		double rMargin)			// In: The distance considered a hit
		{
		m_pSpline = pSpline;
		m_ptTo = ptTo;
		m_rMargin = rMargin;
		m_rSqMargin = rMargin * rMargin;
		m_bHit = FALSE;
		m_t = 0;
		}

	// Methods
	BOOL Hit()
		{
		return m_bHit;
		}

	RC Accept(				// Return E_NOMSG if hit
		const C3Point & P,	// In: A point on the spline
		double t)			// In: Parameter value there
		{
		RC rc = SUCCESS;
		C3Point D = P - m_ptTo;
		double rDist, r;
		if (D * D < m_rSqMargin)
			{
			m_bHit = TRUE;
			rc = E_NOMSG;
			goto exit;
			}

		if (!m_bFirst)
			{
			if (NearSegment(P, t, r))
				{
				if (m_pSpline->PullBack(m_ptTo, r, D,	r, rDist))
					goto exit;
				if (rDist < m_rSqMargin)
					{
					m_bHit = TRUE;
					rc = E_NOMSG;
					}	// End if hit
				} // End if near segment
			} // End if not first point
	exit:
			// Update
		m_P = P;
		m_t = t;
		m_bFirst = FALSE;
		RETURN_RC(CHitTestCache::Accept, rc);
		}

	BOOL NearSegment(
		const C3Point & P,	// In: Segment's endpoint
		double t,			// In: Parameter value there
		double & rSeed)		// Out: Approximate parameter on the spline
		{
		BOOL bHit = FALSE;
		C3Point AB = P - m_P;
		C3Point AP = m_ptTo - m_P;
		double d = AB * AB;
		if (FZERO(d, FUZZ_GEN))
			// The line degenerates to a point, we're not interested
			goto exit;
/*
 Say the line is C(t) = (1-t)*A + t*B and the point is P.  The line from P
 to the nearest point is perpendicular to AB, so (C-P)*(B-A)=0;
 Expanding C(t) and elimnating t, we get t=(B-A)*(B-A)/((P-A)*(B-A)).

 */
		rSeed = AB * AP / d;
		if (rSeed > 0 && rSeed < 1)
		// We are not interested in solutions at the endpoints,
		// so the test is without fuzz
			{
			AB = AP - rSeed * AB;
			d = AB * AB;
			if (d < 2 * m_rSqMargin)
				bHit = TRUE;
			rSeed = (1 - rSeed) * m_t + rSeed * t;
			}
	exit:
		return bHit;
		}

	// Data
protected:
	C3Point m_ptTo;		// The point we are testing
	double m_rMargin;	// The distance condidered a hit
	double m_rSqMargin;	// The square of the above
	C3Point m_P;		// Current probe point on the spline
	double m_t;			// Parameter value there
	BOOL m_bHit;		// TRUE if this point hits the spline
	BOOL m_bFirst;		// =TRUE until the first point has been processed
	PNURBS m_pSpline;	// The spline in question
	};
/**********************************************************************/
// Determine if a given point is on the spline (within tolerance)
BOOL CNURBS::HitTest( // Return TRUE if the point is on the spline
	const C3Point& ptTo,    // In: The point we want to test
	double rMargin)			// In: The distance considered a hit
	{
	CHitTestCache tester(this, ptTo, rMargin);
	Linearize(rMargin, NULL, tester);
	return tester.Hit();
	}
//////////////////////////////////////////////////////////////////////
//																	//
//						Scribbling stuff							//

// Add a knot and control point to a cubic spline
RC CNURBS::AddSpan(
	double rKnot,       // In: Parameter values for new knot
	C3Point & ptPoint)  // In: New control point
	/* The new knot and point are added to the end of the list,
	 * and the curve's end is reset
	 */
	{
	// This is a special puropse method for scribbleing, where the spline
	// is always a non-rational cubics
	ASSERT(!Rational());

	RC rc = E_TooManyKnots;	 // Be pessimistic
	if (PointCount() >= MAX_SPLINE_SIZE)
		QUIT

	if (rc = m_pKnots->AddAsLast(rKnot))
		QUIT
	ResetEnd(rKnot);
	rc = m_pControlPoints->Add(ptPoint);
exit:
	RETURN_RC(CNURBS::AddSpan, rc);
	}
/*******************************************************************/
// Reinterpolate the last few spans of a cubic spline to fit new data
RC CNURBS::ReDoEnd(
	CRealArray & arrNodes,   // In: Parameter values for new data points
	CPointArray & arrPoints) // In: New interpolation points for the spline's end
/* This function will modify some of the spline's control points to make the
 * spline interpolate the new data at its end with the "Not a Knot"
 * end dondition.  The number of points that will be modified is less by one then the
 * number of data points and nodes
 *
 * Assumptions:
 * 		The spline must be nonrational cubic
 *
 *		The number of control points in the spline must be no less than the
 *      number of data points
 *
 *		The number of data points must be at least 4
 *
 * 		The number of data nodes must be equal to the number of points.
 */
	{
	int i,j;
	RC rc,rc1;
	ASSERT(PointCount() >= arrNodes.Size());
	ASSERT(arrNodes.Size() > 3);
	ASSERT(arrNodes.Size() == arrPoints.Size());
	ASSERT(!Rational());


	// Some special numbers to remember
	int nData = arrPoints.Size();     // Number of data points
	int nFirstPoint = PointCount() - nData; // First control point we look at

	// Adjust the last knot
	m_pKnots->Set(LastKnot(), arrNodes[nData-1]);
	ResetEnd(arrNodes[nData-1]);

    //  skip the second last last ("Not-knot") node and adujst the rest
	for (i=nData-3, j=LastKnot()-1;  i>=0 && j>3;  i--, j--)
		m_pKnots->Set(j, arrNodes[i]);

    if (nFirstPoint == 0)
	// Special case - essentially reconstruct the spline
		{
		// Copy the data into the control points
		for (i=0;  i<nData;  i++)
			ControlPoint(i) = arrPoints[i];

		// Solve the inverse problem to compute the control points
		rc = InvertNotAKnot(arrNodes);
		}
	else
		{

		// The general case - redo a proper subset of the control points

    	// Linear equations working arrays
		CBandedArray A(nData+1,2, rc);
		CPointArray arrSolution(arrPoints, rc1);  // A copy of the data points
		double rBasisVals[4];
		if (rc)
			QUIT
		if (rc = rc1)
			QUIT

		// Set up the first and last equations
		A[0][0] = A[nData][nData] = 1.0;
		if (rc = arrSolution.Insert(ControlPoint(nFirstPoint-1), 0))
			QUIT

		// Set up the second equation
		m_pKnots->EvaluateBasis (nFirstPoint+2, arrNodes[0], rBasisVals);
		m_pKnots->EvaluateBasis (nFirstPoint+2, arrNodes[0], &A[1][0]);

		// The rest of the equations
		for (i=2;  i<nData;  i++)
			{
			m_pKnots->EvaluateBasis (nFirstPoint+i, arrNodes[i-1], rBasisVals);
			m_pKnots->EvaluateBasis (nFirstPoint+i, arrNodes[i-1], &A[i][i-2]);
			}

    	// Solve the equations
    	if (rc = A.LUFactorNoPivot())
    		QUIT
    	if (rc = A.LUSolveNoPivotP(arrSolution))
    		QUIT
		for (i=0;  i<nData;  i++)
			ControlPoint(nFirstPoint+i) = arrSolution[i+1];
		}
exit:
		RETURN_RC(CNURBS::ReDoEnd, rc);
	}
/*******************************************************************/
// How complex is this spline?
int CNURBS::Complexity() const
	{
	return Degree() * Degree() * PointCount() * 12;
	}

/*******************************************************************
 *
 * The funciton to split a NURBS at a point and return optional
 * left(low parameter area), right(high paranmeter area), or both
 * sides of the curve.
 *
 * @Author: Stephen W. Hou
 * @Date:   April 6, 2003
 */

RC CNURBS::Split(double rAt,	// In: Paramter value to split at
				 SIDE nSide,    // In: Which side to keep (possibly both)
				 PCurve & pFirst,			// Out: The first half
				 PCurve & pSecond) const	// Out: The second half
{
	pFirst = NULL;
	pSecond = NULL;

	RC rc = SUCCESS;

	double start, end;
	m_pKnots->GetDomainInterval(start, end);
	if (fabs(rAt - start) < FUZZ_DIST) {
		if (nSide == BOTH || nSide == RIGHT)
			pSecond = new CNURBS(*this, rc);

		return rc;
	}
	else if (fabs(rAt - end) < FUZZ_DIST) {
		if (nSide == BOTH || nSide == LEFT)
			pFirst = new CNURBS(*this, rc);

		return rc;
	}

	CKnots knotVector(*m_pKnots, rc);

	int nAddedKnots, numb;
    knotVector.InsertKnot(rAt, 0, numb, nAddedKnots);
	numb += nAddedKnots - 1;

	// construct an alpha matrix
	int length = knotVector.KnotCount();
	CMatrix alpha = AlphaMatrix(*m_pKnots, knotVector, m_pKnots->Degree() + 1);

	// compute new control vertices
	CControlVertices vertices(*m_pControlPoints, *m_pWeights);
    CControlVertices newVertices = vertices * alpha;

	// construct new NURBS curves
    int numb1 = numb - m_pKnots->Degree() + 1;
	if (nSide == LEFT || nSide == BOTH) {
		CControlPoints* pCtrlPoints = new CControlPoints(numb1, rc);
		for (register i = 0; i < numb1; i++)
			pCtrlPoints->Set(i, newVertices[i].GetPoint());

		CRealArray* pWeightArray = NULL;
		if (m_pWeights) {
			pWeightArray = new CRealArray(numb1, rc);
			for (i = 0; i < numb1; i++)
				pWeightArray->Add(newVertices[i].GetWeight());
		}
		double startParam = knotVector.Knot(0);
		double endParam = knotVector.Knot(numb);
		CKnots* pKnotVector = new CKnots(knotVector.Degree(), numb1, rc);
		for (i = 0; i < numb1; i++)
			pKnotVector->Set(i, knotVector.Knot(i));
		pKnotVector->SetInterval(endParam - startParam);
		CNURBS* pNurbs = new CNURBS(pKnotVector, pCtrlPoints, pWeightArray,
			                 startParam, endParam, m_rPeriod, rc);
		if (rc == SUCCESS)
			pFirst = pNurbs;
		else
			return rc;
	}

	if (nSide == RIGHT || nSide == BOTH) {
		int numb2 = length - numb - 1;
		CControlPoints* pCtrlPoints = new CControlPoints(numb2, rc);
		for (register i = 0; i < numb2; i++)
			pCtrlPoints->Set(i,newVertices[i + numb1 - 1].GetPoint());

		CRealArray* pWeightArray = NULL;
		if (m_pWeights) {
			pWeightArray = new CRealArray(numb2, rc);
			for (i = 0; i < numb2; i++)
				pWeightArray->Add(newVertices[i + numb1 - 1].GetWeight());
		}

		double startParam = knotVector.Knot(numb1);
		double endParam = knotVector.Knot(length - 1);
		CKnots* pKnotVector = new CKnots(knotVector.Degree(), numb2, rc);
		pKnotVector->Set(0, knotVector.Knot(numb1));
		for (register j = 1, sIndex = numb1; j < numb2; j++)
			pKnotVector->Set(j, knotVector.Knot(sIndex++));

		pKnotVector->SetInterval(endParam - startParam);

		CNURBS* pNurbs = new CNURBS(pKnotVector, pCtrlPoints, pWeightArray,
							        startParam, endParam, m_rPeriod, rc);
		if (rc == SUCCESS)
			pSecond = pNurbs;
	}

	return rc;
}


/*******************************************************************
 *
 * Compute the length of curve.
 *
 * @Author: Stephen W. Hou
 * @Date:   April 24, 2003
 */
double CNURBS::GetLength(double fromParam,		// In: Start parameter
						 double toParam) const	// In: End parameter
{
	if (m_pKnots->Order() == 2)
		return m_pControlPoints->DistanceSum();

	double par, t[13];
	C3Point pt, tan;
	for (register i = 0; i < 13; i++) {
		par = i;
		par /= 12;
		par = fromParam + par * (toParam - fromParam);
		Evaluate(par, pt, tan);
		t[i] = sqrt(tan.X() * tan.X() + tan.Y() * tan.Y() + tan.Z() * tan.Z());
	}
	par = (t[0] + 4 * t[1] + 2 * t[2] + 4 * t[3] + 2 * t[4] + 4 * t[5] +
			2 * t[6] + 4 * t[7] + 2 * t[8] + 4 * t[9] + 2 * t[10] + 4 * t[11] +
			t[12]) / 36.0;
	par *= (toParam - fromParam);
	return par;
}

/*******************************************************************
 *
 * Make curve rationalize.
 *
 * @Author: Stephen W. Hou
 * @Date:   April 24, 2003
 */
RC CNURBS::Rationalize()
{
	RC rc = SUCCESS;
	if (!m_pWeights && m_pControlPoints) {
		m_pWeights = new CRealArray(m_pControlPoints->Size(), rc);
		for (register i = 0; i < m_pControlPoints->Size(); i++)
			(*m_pWeights)[i] = 1.0;
	}
	return rc;
}


/*******************************************************************
 *
 * Join a curve with this curve at a common end point to become a
 * singal curve..The input curve will be destroied if the operation
 * is successful.
 *
 * @Author: Stephen W. Hou
 * @Date:   April 24, 2003
 */
RC CNURBS::JoinWith(CNURBS* &pNurbs)
{
	RC rc = SUCCESS;

	ASSERT(pNurbs != NULL);

	CNURBS newCurve(*this, rc);

	double end1 = newCurve.Knot(newCurve.LastKnot());
	double start2 = pNurbs->Knot(0);

	// trim out the end portion if the end point don't match the last control point
	//
	CNURBS* pNurbs1 = NULL, *pNurbs2 = NULL;
	if (!FEQUAL(newCurve.End() , end1, FUZZ_GEN)) {
		newCurve.Split(End(), LEFT, *(PCurve*)&pNurbs1, *(PCurve*)&pNurbs2);
		newCurve.SwapDataWith(*pNurbs1);
		delete pNurbs1; pNurbs1 = NULL;
	}

	if (!FEQUAL(pNurbs->Start(), start2, FUZZ_GEN)) {
		pNurbs->Split(pNurbs->Start(), RIGHT, *(PCurve*)&pNurbs1, *(PCurve*)&pNurbs2);
		pNurbs->SwapDataWith(*pNurbs2);
		delete pNurbs2; pNurbs2 = NULL;
	}

	// verify if the two curves are touch at the end points.
	//
	bool shareEnd = (fabs(m_pControlPoints->Last().DistanceTo(pNurbs->m_pControlPoints->First())) <= FUZZ_DIST);

	// make curve compatible
	//
	int order = MAX(pNurbs->m_pKnots->Order(), newCurve.m_pKnots->Order());
	if (!pNurbs->m_pWeights && newCurve.m_pWeights)
		pNurbs->Rationalize();

	if (pNurbs->m_pWeights && !newCurve.m_pWeights)
		newCurve.Rationalize();

	while (pNurbs->m_pKnots->Order() < order)
		pNurbs->Elevate();

	while (newCurve.m_pKnots->Order() < order)
		newCurve.Elevate();

	double lenght1 = newCurve.GetLength();
	double length2 = pNurbs->GetLength();

	if (length2 < FUZZ_DIST)
		return rc;
	else if (lenght1 < FUZZ_DIST)
		return rc;

	pNurbs->m_pKnots->StartAt(newCurve.End());
	newCurve.m_pKnots->JoinWith(*pNurbs->m_pKnots, shareEnd);
	newCurve.m_rEnd += pNurbs->m_pKnots->Interval();

	CControlVertices controlVertices1(*newCurve.m_pControlPoints, *newCurve.m_pWeights);
	CControlVertices controlVertices2(*pNurbs->m_pControlPoints, *pNurbs->m_pWeights);
	controlVertices1.JoinWith(controlVertices2, shareEnd);

	delete newCurve.m_pControlPoints;
	newCurve.m_pControlPoints = controlVertices1.GetPoints();

	delete newCurve.m_pWeights;
	newCurve.m_pWeights = controlVertices1.GetWeights();

	SwapDataWith(newCurve);

	delete pNurbs;
	pNurbs = NULL;
	return rc;
}


/*******************************************************************
 *
 * Elevate the order of curve.
 *
 * @Author: Stephen W. Hou
 * @Date:   April 23, 2003
 */
RC CNURBS::Elevate()
{
	ASSERT(pControlPoints != NULL);
	ASSERT(m_pKnots != NULL);

	RC rc = SUCCESS;

	CKnots *pKnotVector = new CKnots(*m_pKnots, rc);
	pKnotVector->Elevate();

	CRealArray u = m_pKnots->GetAllKnots();
	CRealArray v = pKnotVector->GetAllKnots();

	int order = m_pKnots->Order();
    int number = pKnotVector->KnotCount() - order - 1;

    CControlPoints cc(order, rc), cw(order, rc);
	CControlPoints* pControlPoints = new CControlPoints(number, rc);
    for (register i, l, j = 0; j < number; j++) {
		for (register ir = 0; ir < m_pControlPoints->Size() - 1; ir++)
        if (v[j] >= u[ir] && v[j] < u[ir+1])
			break;

		cc = cw = 0.;
		int is = (order + 1 - ir - 1 > 1) ? order + 1 - ir - 1 : 1;
		int ie = (int)((order + m_pControlPoints->Size() - ir - 1 < order) ?
						order + m_pControlPoints->Size() - ir - 1 : order);

		for (i = is; i <= ie; i++)
			cw[i - 1] = cc[i - 1] = (*m_pControlPoints)[i + ir - order];

		double y1, y2, yi;
		for (l = order - 1; l >= 1; l--) {
			y1 = v[j + l];
			y2 = v[j + l + 1];
			is = (int)((m_pControlPoints->Size() + 2 * order - l - ir - 1 < order) ?
					  m_pControlPoints->Size() + 2 * order - l - ir - 1 : order);
			ie = (order - ir > ie) ? order - ir : order - l + 1;

			double yik, den;
			for (i = is; i >= ie; i--) {
				yi = u[ir + i - order];
				yik = u[ir + i + l - order];
				den = yik - yi;
				cc[i - 1] = ((y2 - yi) * cc[i - 1] + (yik - y2) * cc[i - 2]) / den;
				cw[i - 1] = ((y1 - yi) * cw[i - 1] + (yik - y1) * cw[i - 2]) / den + cc[i - 1];
			}
		}
		(*pControlPoints)[j] = cw[order - 1] / order;
    }
	delete m_pKnots;
	m_pKnots = pKnotVector;
	delete m_pControlPoints;
	m_pControlPoints = pControlPoints;

	return rc;
}



/****************************************************************************
 *
 *	Find the perpendicular line through a point. The function returns SUCCESS
 *  if the perpendicualar point is found and E_NOMSG otherwise.
 *
 *	@Author: Stephen W. Hou
 *	@Date:   Sep 21, 2003
 */
RC CNURBS::PerpendicularThru(
	const C3Point & ptThru,		// In: A point not on curve
    const C3Point & ptReference,// In: The optional reference point for pick solution
	C3Point & ptPerpPoint,		// Out: Perpendicular point on the curve
	double & rAt				// Out: Parameter value of the perpendicular point
	) const
{
	C3Point ptNearest;
	double param, dis, dPar;
	if (NearestPoint(ptReference, ptNearest, param, dis) == SUCCESS) {
		C3Point point, tangent, secondDirivative;
        C3Point values[3];
		for (register i = 0; i < 50; i++) { // the iteration time limit is set to be 50
			if (Evaluate(param, 2, values) != SUCCESS)
				return FAILURE;

			C3Vector dv = ptThru - values[0];
			double tmp = dv.X() * values[2].X() + dv.Y() * values[2].Y() -
				              (values[1].X() * values[1].X() + values[1].Y() * values[1].Y());

            if (FZERO(tmp, FUZZ_DIST))
				return FAILURE;

			dPar = -(dv.X() * values[1].X() + dv.Y() * values[1].Y()) / tmp;

			// limit the step size
			if (fabs(dPar) > 0.1)
				dPar = dPar > 0.? 0.1 : -0.1;

			param += 0.9 * dPar;

			// for closed curve the parameter goes periodically
			if (m_rPeriod) {
				double rStart, rEnd;
				m_pKnots->GetDomainInterval(rStart, rEnd);
				param -= rEnd;
				param += rStart;
			}
			if (FZERO(dPar, FUZZ_DIST)) {
				if (Evaluate(param, ptPerpPoint, tangent) == SUCCESS) {
					rAt = param;
					return SUCCESS;
				}
				else
					break;
			}
		}
	}
	return FAILURE;
}


#ifdef _DEBUG
/*******************************************************************
 *
 * Print out to a file stream.
 *
 * @Author: Stephen W. Hou
 * @Date:   April 17, 2003
 */

ostream& operator<<(ostream &out, const CNURBS &spl)
{
	out << "NURBS Curve: " << endl;
	out << "Parameter Range( " << spl.m_rStart << ", " << spl.m_rEnd << "); ";
	out << "Planar Curve: ";
	if (spl.m_bPlanar)
		out << "Yes; " << endl;
	else
		out << "No; " << endl;

	out << "Periodic: " << spl.m_rPeriod << endl;
	if (spl.m_pControlPoints)
		out << *spl.m_pControlPoints << endl;

	if (spl.m_pWeights) {
		out << "Weight Array" << endl;
		out << *spl.m_pWeights;
	}
	if (spl.m_pKnots)
		out << *spl.m_pKnots << endl;

	return out;
}

#endif // _DEBUG



/*******************************************************************
 *
 * Normalize parameter domain to [0, 1].
 *
 * @Author: Stephen W. Hou
 * @Date:   April 18, 2003
 */

RC CNURBS::Normalized()
{
	RC rc = SUCCESS;
	if (m_pKnots) {
		m_rStart = 0.0;
		m_rEnd = 1.0;
		if (m_rPeriod)
			m_rPeriod = 1.0;
		m_pKnots->Normalized();
	}
	else
		rc = FAILURE;

	return rc;
}


/*******************************************************************
 *
 * Reparameterize spline to shift parameter domain to start at the
 * input value. The funciton doesn't change the size of parameter
 * domain.
 *
 * @Author: Stephen W. Hou
 * @Date:   April 18, 2003
 */

RC CNURBS::Reparameterized(double newStartParam)
{
	RC rc = SUCCESS;
	if (m_pKnots) {
		m_rStart = newStartParam;
		m_rEnd = newStartParam + m_pKnots->Interval();
		rc = m_pKnots->StartAt(newStartParam);
	}
	else
		rc = FAILURE;

	return rc;
}


/*******************************************************************
 *
 * Reparameterize to scale the parameter domain.
 *
 * @Author: Stephen W. Hou
 * @Date:   April 18, 2003
 */

RC CNURBS::Reparameterized(double newStartParam,
						   double newEndParam)
{
	RC rc = SUCCESS;
	if (m_pKnots) {
		rc = m_pKnots->Stretch(newStartParam, newEndParam);
		if (rc == SUCCESS) {
			m_rStart = newStartParam;
			m_rEnd = newStartParam;
			if (m_rPeriod)
				m_rPeriod = m_rEnd - m_rStart;
		}
	}
	else
		rc = FAILURE;

	return rc;
}


/*******************************************************************
 *
 * Construct a sub-curve.
 *
 * @Author: Stephen W. Hou
 * @Date:   October 30, 2003
 */
RC CNURBS::Subset(double rFrom,		// In: Start parameter
				  double rTo,		// In: End parameter
				  PCurve & pSubset	// Out: New curve
				 ) const
{
	RC rc = FAILURE;
	pSubset = NULL;

	double start, end;
	m_pKnots->GetDomainInterval(start, end);

	double from = (rFrom < start) ? start : rFrom;
	double to = (rTo > end) ? end : rTo;

	CNURBS* pCurve = NULL, *pCurve1 = NULL, *pCurve2 = NULL;
	rc = Split(to, LEFT, *(PCurve*)&pCurve1, *(PCurve*)&pCurve2);
	if (rc == SUCCESS) {
		pCurve = pCurve1;
		rc = pCurve->Split(from, RIGHT, *(PCurve*)&pCurve1, *(PCurve*)&pCurve2);
		delete pCurve;

		if (rc == SUCCESS)
			pSubset = pCurve2;
	}

	return rc;
}


/************************************************************************/
// Project a curve to a plane along a specified direction. If the direction
// is not specified, the curve will be projected perpendicularly to the 
// plane(along the normal direction of plane). Function returns NULL when
// it fails and a point to the curve otherwise.
//
// NOTE: the spline may not be C1 continuity after projected.
//
// Auther:	Stephen W. Hou
// Date:	6/14/2004
//
PCurve CNURBS::ProjectTo(const CPlane & plane, const C3Vector &dir, bool get2dCurve) const
{
	CNURBS* pNurbs = NULL;
	CControlPoints* pCtrlPoints = m_pControlPoints->ProjectTo(plane, dir, get2dCurve);

	RC rc;
	CKnots* pKnots = new CKnots(*m_pKnots, rc);
	if (rc == SUCCESS) {
		PRealArray pWeight = (m_pWeights) ? new CRealArray(*m_pWeights, rc) : NULL;
		if (rc == SUCCESS) {
			pNurbs = new CNURBS(pKnots, pCtrlPoints, pWeight, m_rStart, m_rEnd, m_rPeriod, rc);
			if (rc != SUCCESS) {
				if (pNurbs)
					delete pNurbs;
				else {
					delete pKnots;
					delete pWeight;
				}
			}
            else {
                pNurbs->SetAs2DCurve(get2dCurve);
                pNurbs->SetPlanar(TRUE);
            }
		}
		else {
			delete pWeight;
			delete pKnots;
		}
	}
	return pNurbs;
}





