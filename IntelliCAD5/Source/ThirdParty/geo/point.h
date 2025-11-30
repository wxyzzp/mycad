/*	Point.h - Defines the classe of 3D points.
 *	Copyright (C) 1994-1998 Visio Corporation. All rights reserved.
 *
 *
 *	ABSTRACT
 *
 * C3Point is an (x,y,z) triplet.  The class overloads the arithmetic
 * operators to make vector arithmetic more readable.
 *
 */
#ifndef POINT_H
#define POINT_H

#include <math.h>	// For fabs
#include "geometry.h"

#ifdef THREE_D
	#define DIMENSION 3
#else
	#define DIMENSION 2
#endif
/////////////////////////////////////////////////////////////////////////////
// Real Point


#ifdef DEBUG
class ostream;
#endif // DEBUG


class GEO_CLASS C3Point
{
// Construction & destruction
public:

// Default constructor and destructor
	inline C3Point()
		{
		m_x[0] = m_x[1] = 0;
#ifdef THREE_D
		m_x[2] = 0;
#endif
		}

	inline ~C3Point()
		{
		}

// Construct from coordintes
	inline C3Point(double rX, double rY, double rZ=0)
		{
		m_x[0] = rX;
		m_x[1] = rY;
#ifdef THREE_D
		m_x[2] = rZ;
#endif
		}

	inline C3Point( const WCPoint& p)
		{
		m_x[0] = p.x;
		m_x[1] = p.y;
#ifdef THREE_D
		m_x[2] = 0;
#endif
		}

	inline C3Point( const POINT& p)
		{
		m_x[0] = p.x;
		m_x[1] = p.y;
#ifdef THREE_D
		m_x[2] = 0;
#endif
		}


//	Inline frequently used arithmetic operators

	//Add 2 points
	inline C3Point  operator + (const C3Point & rptOther) const
		{
#ifdef THREE_D
		return C3Point(m_x[0] + rptOther.m_x[0], 
					   m_x[1] + rptOther.m_x[1],
					   m_x[2] + rptOther.m_x[2]);
#else
		return C3Point(m_x[0] + rptOther.m_x[0], 
					   m_x[1] + rptOther.m_x[1]);
#endif
		}
	/***********************************************************************/
	// Subtract 2 points
	inline C3Point   operator - (const C3Point & rptOther) const
		{
#ifdef THREE_D
		return C3Point( m_x[0] - rptOther.m_x[0], 
						m_x[1] - rptOther.m_x[1],
						m_x[2] - rptOther.m_x[2]);
#else
		return C3Point( m_x[0] - rptOther.m_x[0], 
						m_x[1] - rptOther.m_x[1]);
#endif
		}
	/***********************************************************************/
	//Add a point in place
	inline void  operator += (const C3Point & rptPoint)
		{
		m_x[0] += rptPoint.m_x[0];
		m_x[1] += rptPoint.m_x[1];
#ifdef THREE_D
		m_x[2] += rptPoint.m_x[2];
#endif
		}
	/***********************************************************************/
	//Subtract a point in place
	inline void  operator -= (const C3Point & rptPoint)
		{
		m_x[0] -= rptPoint.m_x[0];
		m_x[1] -= rptPoint.m_x[1];
#ifdef THREE_D
		m_x[2] -= rptPoint.m_x[2];
#endif
		}
	/***********************************************************************/
	inline double  X() const
		{
		return m_x[0];
		}
	/***********************************************************************/
	inline double  Y() const
		{
		return m_x[1];
		}
	/***********************************************************************/
#ifdef THREE_D
	inline double  Z() const
		{
		return m_x[2];
		}
#endif
	/***********************************************************************/
	inline double Coordinate(
			int i) const
		{
		return m_x[i];
		}
	/***********************************************************************/
	inline void  SetX(double rX)
		{
		m_x[0] = rX;
		}
	/***********************************************************************/
	inline void  SetY(double rY)
		{
		m_x[1] = rY;
		}
	/***********************************************************************/
#ifdef THREE_D
	inline void  SetZ(double rZ)
		{
		m_x[2] = rZ;
		}
#endif
	/***********************************************************************/
	// Are 2 points equal within tolerance?
	inline BOOL  operator != (const C3Point & rptOther) const
		{
		return !((*this) == rptOther);
		}
	/***********************************************************************/
	// Are 2 points equal within given tolerance?
	inline BOOL  Equal(
		const C3Point & rptOther,  // I: Other point
		double rTolerance) const			  // I: Tolerance
		{
#ifdef THREE_D
		return fabs(m_x[0] - rptOther.m_x[0]) < rTolerance  &&
			   fabs(m_x[1] - rptOther.m_x[1]) < rTolerance  &&
			   fabs(m_x[2] - rptOther.m_x[2]) < rTolerance;
#else
		return fabs(m_x[0] - rptOther.m_x[0]) < rTolerance  &&
			   fabs(m_x[1] - rptOther.m_x[1]) < rTolerance;
#endif
		}

	void  operator = (const WCPoint& P)
		{
		m_x[0] = P.x;
		m_x[1] = P.y;
#ifdef THREE_D
		m_x[2] = 0;
#endif
		}

// Methods and operators	
	BOOL  operator == (const C3Point & ptOther) const;

	double  operator * (const C3Point & ptOther) const;

	C3Point  operator * (double rFactor) const;

	C3Point  operator / (double rFactor) const;

	// initial point a value
	//
	C3Point& operator= (double value) 
		{
		m_x[0] = m_x[1] = value;
#ifdef THREE_D
		m_x[2] = value;
#endif
		return *this;
		}

	void  operator /= (
		double rFactor);	// I: The factor to divide by, 0 not allowed

	void  operator *= (double rFactor);

	GEO_FRIEND C3Point  operator * (double rFactor, const C3Point & P);

#ifdef THREE_D
	C3Point  operator ^ ( // Return the cross product this X Other
		const C3Point & ptOther) const;	// I: The other point
#endif
	
	operator WCPoint() const;

	double DistanceTo(const C3Point &pt) const;

	double  Norm() const;

	GEO_FRIEND C3Point  operator - (const C3Point & P);

	void RandomTurnLeft();

	void TurnLeft(
		const C3Point * p = NULL); // I: Unit vector defining the rotation axis

	RC Unitize();

	RC Pack(
		LPBYTE & pStorage) const;  // I/O: The storage

	RC Unpack(
		LPBYTE & pStorage);  // I/O: The storage

	int PackingSize() const;

#if defined DEBUG
	void Dump() const;

	GEO_FRIEND ostream& operator<<(ostream &out, const C3Point &pt);

#endif

// Data
protected:
	double m_x[DIMENSION];   // the 3 coordinates
};


inline double C3Point::DistanceTo(const C3Point &pt) const 
{ 
	C3Point disp(*this - pt); 
	return disp.Norm(); 
}

/////////////////////////////////////////////////////////////////////////////
// A normal vector.
// The only difference between it an C3Point is the default constructor

class GEO_CLASS CNormal	:	public C3Point
{
// Construction & destruction
public:

	CNormal(
		const C3Point * p=NULL);

	inline ~CNormal()
		{
		}
	};

////////////////////////////////////////////////////////////////////////////////
// Point and vector utilities

GEO_API RC Angle(
	const C3Point & A,		// I: One vector
	const C3Point & B,		// I: The other vector
	double & rAngle,		// O: The angle
	C3Point * pUp = NULL);	// I: Upward unit unit normal (NULL OK)

GEO_API RC CosAngle(
	const C3Point & A,			// I: One vector
	const C3Point & B,			// I: The other vector
	double & rCosAngle);		// O: Cosine of the angle

GEO_API double Determinant(		// Return the signed magnitude of A X B
	const C3Point & A,	 // I: One vector	
	const C3Point & B,	 // I: The other vector	
	C3Point * pUP=NULL); // I: Unit vector poining upwards (NULL OK)

GEO_API double Determinant(		// Return the determinant |A,B,C|
		const C3Point & A,	// I: The 3 vectors	
		const C3Point & B,	
		const C3Point & C);

GEO_API double PointDistance(
	const C3Point & A,		//In: One point
	const C3Point & B);		//In: The other point

GEO_API BOOL Collinear(
	const C3Point & A,		// I: Line's first point
	const C3Point & B,		// I: Line's other point
	const C3Point & P,		// I: The point
	double rTolerance);		// I: Tolerated distance from the line AB
#endif
