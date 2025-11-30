/*	CtrPts.h - Defines the class of control-points list for a spline curve
 *	Copyright (C) 1994-1998 Visio Corporation. All rights reserved.
 *
 *
 *	ABSTRACT
 * A control points list is essentially a CPointArray with periodic access mode.
 * This means that if there are actually n points in the list then Point[n] is
 * understood as Point[0], and Point[-1] is the same as Point[n-1].
 *
 *
 */
#ifndef CTRLPTS_H
#define CTRLPTS_H

#include <math.h>
#include "PtArray.h"
#ifdef DEBUG
class ostream;
#endif // DEBUG

class CPlane;
class C3Vector;


class GEO_CLASS CControlPoints : public CPointArray
{
// Construction & destruction
public:
	CControlPoints();

	CControlPoints(
		const CPointArray  & Other,  //In: The other array to copy
		RC & rc);					 // Out: Return code


	CControlPoints(
		LPBYTE  & pStorage,   // In/Out: The storage
		RC & rc);		      // Out: Return code

	CControlPoints(
		const CControlPoints &, 	//In: The other array to copy
		RC & rc);					// Out: Return code

	CControlPoints(
		int nSize,  //In: The number of points
		RC & rc);	// Out: Return code

	virtual ~CControlPoints();
	
// Attributes
public:
C3Point& operator [](int nIndex) const;
                                       
// Operations
public:
	void Set(int, const C3Point&); // Set a point
	RC MakeRoom(int,int);   // Make room for new entries
	RC Insert(C3Point&,int);   // Insert a new entry
	void Remove(int, int);

	RC  StartAt(
		int nIndex);  // In: The desired index of the first control point

	int IndexInRange(
		int nIndex) const; // Index of the desired control point

	BOOL IsPlanar() const; 

	double	DistanceSum() const;

    CControlPoints* ProjectTo(const CPlane & toPlane,       
		                      const C3Vector & direction = *(C3Vector*)NULL,
                              bool  planeCoordinates = false
						     ) const;

	CControlPoints& operator=(const CControlPoints &ctrlPoints);
	CControlPoints& operator=(double value);

#ifdef DEBUG
	friend ostream& operator<<(ostream &out, const CControlPoints &ctrlPoints);
#endif // DEBUG
// No data
};

inline CControlPoints& CControlPoints::operator=(const CControlPoints &ctrlPoints)
{ CPointArray::operator=(ctrlPoints); return *this; }

inline CControlPoints& CControlPoints::operator=(double value)
{ for (register i = 0; i < m_nSize; i++) m_ptEntries[i] = value; return *this; }

inline double CControlPoints::DistanceSum() const 
{
	double totalDistance = 0.0;
	if (m_nSize > 1) {
		for (register i = 1; i < m_nSize; i++) 
			totalDistance += m_ptEntries[i].DistanceTo(m_ptEntries[i - 1]);

		totalDistance = sqrt(totalDistance);
	}
	return totalDistance;
}

#endif
