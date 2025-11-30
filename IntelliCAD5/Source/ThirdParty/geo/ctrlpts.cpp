/*	CtrlPts.cpp - Implements the class CControlPoints.
 *	Copyright (C) 1994-1998 Visio Corporation. All rights reserved.
 *
 *
 *	Abstract
 *
 * See CtrlPts.h for the definition of the class; this implements its methods.
 *
 */
#include "Geometry.h"
#include "CtrlPts.h"
#include "vector.h"
#include "plane.h"

#ifdef DEBUG
#include <ostream.h>
#endif // DEBUG

DB_SOURCE_NAME;

/**********************************************************/
// Constructor
CControlPoints::CControlPoints()
	{
	m_nGrowBy = 4;
	}
/**********************************************************/
// Construct from an array of points
CControlPoints::CControlPoints(
	const CPointArray & Other,  //In: An array of points
	RC & rc)					// Out: Return code
	// Use the CPointArray copy constructor
	:CPointArray(Other, rc)
	{
	m_nGrowBy = 4;
	}
/*******************************************************************************/
// Construct by unpacking
CControlPoints::CControlPoints(
	LPBYTE & pStorage,  // In/Out: The storage
	RC & rc)			// Out: Return code
	// Use the CPointArray unpcking constructor
	: CPointArray(pStorage, rc)
	{
	m_nGrowBy = 4;
	}
/**********************************************************/
// Copy constructor
CControlPoints::CControlPoints(
	const CControlPoints & Other, //In: An array of points
	RC & rc)					  // Out: Return code
	
	// Use the CPointArray copy constructor
	:CPointArray(Other, rc)
	{
	m_nGrowBy = 4;
	}
/**********************************************************/
// Construct with a given number of points, uninitialized
CControlPoints::CControlPoints(
	int nSize,   //In: The number of points
	RC & rc)	 // Out: Return code

	// Use the CPointArray constructor
	:CPointArray(nSize, rc)
	{
	m_nGrowBy = 4;
	}
/**********************************************************/
//Destructor
CControlPoints::~CControlPoints()
	{
	m_nGrowBy = 4;
	}
/******************************************************************************/
//The point with a given index
C3Point& CControlPoints::operator [](int nIndex) const
{
	nIndex = IndexInRange(nIndex);
	return m_ptEntries[nIndex];
}
/***************************************************************************/
// Insert a C3Point entry at a given index of the list                                                 		
RC CControlPoints::Insert(
	C3Point & ptPoint,  // In: The new entry to be inserted
	int nAt)            // In: The index of the new entry
	{
	// Account for periodic wraparound
	nAt = IndexInRange(nAt);
	
	// Make room for the new entry
	RC rc = MakeRoom (nAt, 1);

	//Enter the new entry
	if (!rc)       	
		m_ptEntries[nAt] = ptPoint;
	RETURN_RC(CControlPoints::Insert, rc);     
	}
/***************************************************************************/
// Maek room for inserting additional control points                                                 		
RC CControlPoints::MakeRoom(
	int   nAt,    // In: The index after which we need to insert entries
	int   nnew)   // In: The number of entries to make room for
// Return the new size
	{
	// allocates more space and shifts entries to make room

	// Account for periodic wraparound
	nAt = IndexInRange(nAt);
    return CPointArray::MakeRoom(nAt, nnew);
	}
/***************************************************************************/
// Remove control points                                                		
void CControlPoints::Remove(
	int nFrom,      // In: The first index of the tail
	int nNumber)    // In: The number of entries to remove

	{
	// Account for periodic behavior
	nFrom = IndexInRange(nFrom);
	int nWrapAround = MAX(nFrom + nNumber - m_nSize, 0);

	// Remove
	CPointArray::Remove(nFrom, nNumber - nWrapAround);
	
	if (nWrapAround)
		CPointArray::Remove(0, nWrapAround);
	}
/*******************************************************************/
// Set the control point with given index
void CControlPoints::Set(
	int nIndex,				// Index of control point to be set
	const C3Point & ptPoint)// The value assigned to that point
	{
	nIndex = IndexInRange(nIndex);
	m_ptEntries[nIndex] = ptPoint;
	}
/*******************************************************************/
// Given an index, find a corresponding one within the list range
int CControlPoints::IndexInRange(// Return an index within the range
	int nIndex) const	// Index of the desired control point
	{
	int n = Size();
	
	// pull the index into the range
	while (nIndex >= n)
		nIndex -= n;
	while (nIndex < 0)
		nIndex += n;
		
	return nIndex;
	}
/***************************************************************************/
// Change the representation and make it start at a given control point                                                		
RC  CControlPoints::StartAt(
	int nIndex)  // In: The desired index of the first knot
	{
	// Get a representative within the range
	nIndex = IndexInRange(nIndex);
	RC rc = SUCCESS;
	
	for (int i=0;  i<nIndex;  i++)
		{
		// Move the first point from the head to the tail of the list
		if (rc = Add(m_ptEntries[0]))
			break;
		Remove(0, 1);
		}
	
	RETURN_RC(CControlPoints::StartAt, rc);
	}

/**************************************************************************/
// Test if the control points are on a plane
//
// Author:	Stephen W. Hou
// Date:	4/12/2003
//
BOOL CControlPoints::IsPlanar() const
{
    C3Vector vector1((*this)[0] - (*this)[1]);
	C3Vector vector2((*this)[0] - (*this)[2]);
	C3Vector normal(vector1 ^ vector2);
    normal.Normalized();
    for (register j = 2; j < m_nSize - 1; j++) {	
		vector1 = ((*this)[j] - (*this)[j+1]); 
		vector1.Normalized();
		if (fabs(vector1 * normal) > FUZZ_DIST) 
			return 0;
    }
	return 1;
}


/**************************************************************************/
// Project control points along a specified direction onto a specified 
// plane.
//
// Author:	Stephen W. Hou
// Date:	6/23/2004
//
CControlPoints* CControlPoints::ProjectTo(const CPlane & plane, const C3Vector & dir, bool planeCoordinates) const
{
	C3Vector along;
    C3Point point;
	CControlPoints* pCtrlPoints = new CControlPoints;
    if (&dir == NULL) {
        CPlane::PLANETYPE type = plane.PlaneType();
        if (type== CPlane::TYPE_Xy) {
	        for (register i = 0; i < Size(); i++) {
                point = (*this)[i];
                point.SetZ(0.0);
		        pCtrlPoints->Add(point);
	        }            
            return pCtrlPoints;
        }
        else if (type == CPlane::TYPE_Yz) {
	        for (register i = 0; i < Size(); i++) {
                point = (*this)[i];
                if (planeCoordinates) {
                    double u = point.Y();
                    double v = point.Z();
                    point.SetX(u);
                    point.SetY(v);
                    point.SetZ(0.0);
                }
                else
                    point.SetX(0.0);
		        pCtrlPoints->Add(point);
	        }            
            return pCtrlPoints;
        }
        else if (type == CPlane::TYPE_Zx) {
	        for (register i = 0; i < Size(); i++) {
                point = (*this)[i];
                if (planeCoordinates) {
                    double u = point.Y();
                    double v = point.X();
                    point.SetX(u);
                    point.SetY(v);
                    point.SetZ(0.0);
                }
                else
                    point.SetY(0.0);
		        pCtrlPoints->Add(point);
	        }            
            return pCtrlPoints;
        }
        else 
            along = plane.GetNormal().Negate();
    }
    else
        along = dir;

	for (register i = 0; i < Size(); i++) {
		plane.ProjectPoint((*this)[i], along, point, planeCoordinates);
		pCtrlPoints->Add(point);
	}
    return pCtrlPoints;
}


#ifdef DEBUG
ostream& operator<<(ostream &out, const CControlPoints &ctrlPoints)
{
	out << "Control Points: " << endl;
	out << *dynamic_cast<CPointArray*>(const_cast<CControlPoints*>(&ctrlPoints));
	return out;
}
#endif // DEBUG


		
	 
