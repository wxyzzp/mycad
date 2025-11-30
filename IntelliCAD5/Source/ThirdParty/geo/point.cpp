/*	POINT.CPP - Implements the class C3Point
 *	Copyright (C) 1994-1998 Visio Corporation. All rights reserved.
 *
 *
 *	Abstract
 *
 * See POINT.H for the definition of the class.
 * This file implements its methods		
 *
 */
#include "Geometry.h"
#include "Point.h"
#include "Line.h"	//	for CRay
#ifdef DEBUG
#include <ostream.h>
#endif // DEBUG

DB_SOURCE_NAME;

//////////////////////////////////////////////////////////////////
//           3D point

//		Constructors are inline                             

////////////////////////////////////////////////////////////////////////
// Overloaded operators

//	inlined operators
//		+
//		-
//		+=
//		-=

			
/***********************************************************************/
// Are 2 points equal within tolerance?
BOOL  C3Point::operator == (const C3Point & ptOther) const
    {
#ifdef THREE_D
	return fabs(m_x[0] - ptOther.m_x[0]) < FUZZ_GEN  &&
		   fabs(m_x[1] - ptOther.m_x[1]) < FUZZ_GEN  &&
		   fabs(m_x[2] - ptOther.m_x[2]) < FUZZ_GEN;
#else
	return fabs(m_x[0] - ptOther.m_x[0]) < FUZZ_GEN  &&
		   fabs(m_x[1] - ptOther.m_x[1]) < FUZZ_GEN;
#endif
    }
/***********************************************************************/
//Dot (scalar) product
double  C3Point::operator * (const C3Point & ptOther) const
    {
#ifdef THREE_D
	return m_x[0] * ptOther.m_x[0] + 
		   m_x[1] * ptOther.m_x[1] + 
		   m_x[2] * ptOther.m_x[2];
#else
	return m_x[0] * ptOther.m_x[0] + 
		   m_x[1] * ptOther.m_x[1]; 
#endif
    }
/***********************************************************************/
//Multiply a point by a scalar
C3Point  C3Point::operator * (double rFactor) const
    {
#ifdef THREE_D
	return C3Point(rFactor*m_x[0], rFactor*m_x[1], rFactor*m_x[2]);
#else
	return C3Point(rFactor*m_x[0], rFactor*m_x[1]);
#endif
    }
/***********************************************************************/
//Divide a point by a scalar
C3Point  C3Point::operator / (double rFactor) const
    {
	double r = 1.0 / rFactor;
#ifdef THREE_D
	return C3Point(m_x[0] * r, m_x[1] * r, m_x[2] * r);
#else
	return C3Point(m_x[0] * r, m_x[1] * r);
#endif
    }
/***********************************************************************/
//Divide by a scalar in place
void  C3Point::operator /= (
	double rFactor)	// I: The factor to divide by, 0 not allowed
    {
	double r = 1.0 / rFactor;
	m_x[0] *= r;
	m_x[1] *= r;
#ifdef THREE_D
	m_x[2] *= r;
#endif
    }
/***********************************************************************/
//Multiply by a scalar in place
void  C3Point::operator *= (double rFactor)
    {
	m_x[0] *= rFactor;
	m_x[1] *= rFactor;
#ifdef THREE_D
	m_x[2] *= rFactor;
#endif
    }
/***********************************************************************/
//Multiiplay a scalar by a point
C3Point  operator * (double rFactor, const C3Point & P)
    {
#ifdef THREE_D
	return C3Point(rFactor * P.m_x[0],
				   rFactor * P.m_x[1],
				   rFactor * P.m_x[2]);
#else
	return C3Point(rFactor * P.m_x[0], 
				   rFactor * P.m_x[1]);
#endif
    }
/***********************************************************************/
//Cross product
#ifdef THREE_D
C3Point  C3Point::operator ^ ( // Return the cross product this X Other
	const C3Point & ptOther) const	// I: The other point
    {
	C3Point P;
	P.m_x[0] = m_x[1] * ptOther.m_x[2] - m_x[2] * ptOther.m_x[1]; 
	P.m_x[1] = m_x[2] * ptOther.m_x[0] - m_x[0] * ptOther.m_x[2]; 
	P.m_x[2] = m_x[0] * ptOther.m_x[1] - m_x[1] * ptOther.m_x[0];
	return P;
    }
#endif
/***********************************************************************/
// Norm (i.e. distance from the origin)
double  C3Point::Norm() const
    {
#ifdef THREE_D
	return sqrt(m_x[0] * m_x[0] + m_x[1] * m_x[1] + m_x[2] * m_x[2]);
#else
	return _hypot(m_x[0], m_x[1]);
#endif
    }
/***********************************************************************/
// Flip:  P to  -P
C3Point  operator - (const C3Point & P)
    {
#ifdef THREE_D
	return C3Point(-P.m_x[0], -P.m_x[1], -P.m_x[2]);
#else
	return C3Point(-P.m_x[0], -P.m_x[1]);
#endif
    }
/***********************************************************************/
// Rorate about the z axis a random angle, 60 to 120 degrees to the left
void C3Point::RandomTurnLeft()
    {
	// The turn is about the z axis, x[2] is unchanged
	double rCos = FRAND - 0.5;   // A random number between -0.5 and 0.5
	double rSin = sqrt(1.0 - rCos * rCos);
	double r = m_x[1] * rCos + m_x[0] * rSin  ;
	m_x[0] = m_x[0] * rCos - m_x[1] * rSin;
	m_x[1] = r;
    }
/***********************************************************************/
// Rorate 90 degrees to the left
void C3Point::TurnLeft(
	const C3Point * pAbout)	// I: Unit vector defining the rotation axis
							//	  optional, defaulting to the Z axis
    {
#ifdef THREE_D
	CNormal P(pAbout);
	(*this) = P ^ (*this);
#else
	double	r = -m_x[1];
	m_x[1] = m_x[0];
	m_x[0] = r;
#endif
    }
/***********************************************************************/
// Make it a unit vector
RC C3Point::Unitize()
    {
    RC rc = SUCCESS;
#ifdef THREE_D
	double r = sqrt(m_x[0] * m_x[0] + m_x[1] * m_x[1] + m_x[2] * m_x[2]);
#else
	double r = _hypot(m_x[0], m_x[1]);
#endif
	if (FZERO(r, FUZZ_GEN))
		{
		rc = E_ZeroVector;
		goto exit;
		}
	r = 1.0 / r;
	m_x[0] *= r;
	m_x[1] *= r;
#ifdef THREE_D
	m_x[2] *= r;
#endif
exit:
	// an error can be expected sometimes.
	return rc;
	}
/************************************************************************/
// Return a WCPoint
C3Point:: operator WCPoint() const
    {
	WCPoint	point;

	point.x = m_x[0];
	point.y = m_x[1];
	return point;
    }



/*******************************************************************************/
// Pack
RC C3Point::Pack(
	LPBYTE & pStorage) const  // I/O: The storage
	{
	double* pR = (double*)pStorage;
	pR[0] = m_x[0];
	pR[1] = m_x[1];
#ifdef THREE_D
	pR[2] = m_x[2];
#endif
	pStorage += DIMENSION * sizeof(double);
	RETURN_RC(C3Point::Pack, SUCCESS);
	}
/*******************************************************************************/
// Unpack
RC C3Point::Unpack(
	LPBYTE & pStorage)  // I/O: The storage
	{
	double* pR = (double*)pStorage;
	m_x[0] = pR[0];
	m_x[1] = pR[1];
#ifdef THREE_D
	m_x[2] = pR[2];
#endif
	pStorage += DIMENSION * sizeof(double);
	RETURN_RC(C3Point::Unpack, SUCCESS);
	}
/*******************************************************************************/
// Packing Size
int C3Point::PackingSize() const
	{
	return DIMENSION * sizeof(double);
	}
/*******************************************************************************/
#if defined DEBUG
// Dump a point
void C3Point::Dump() const
		{
#ifdef THREE_D
		DB_MSGX("Point: X=%lf, Y=%lf, Z-%lf",(m_x[0], m_x[1], m_x[2]))
#else
		DB_MSGX("Point: X=%lf, Y=%lf",(m_x[0], m_x[1]))
#endif
		}
#endif

////////////////////////////////////////////////////////////////////////////////
// Implementation of CNormal
CNormal::CNormal(
	const C3Point * p)	// Optional: Pointer to the normal, NULL OK 
	{
#ifdef THREE_D
	if (p)
		{
		m_x[0] = p->X();
		m_x[1] = p->Y();
		m_x[2] = p->Z();
		}
	else
		{
		// Default: a unit vector along the Z axis
		m_x[0] = m_x[1] = 0;
		m_x[2] = 1;
		}
#else
	// Normal is meaningless in 2D
	m_x[0] = m_x[1] = 0;
#endif
	}
////////////////////////////////////////////////////////////////////////////////
// Point and vector utilities

// The angle between this and another vector in the current view
RC Angle(
	const C3Point & A,		// I: One vector
	const C3Point & B,		// I: The other vector
	double & rAngle,		// O: The angle
	C3Point * pUp)			// I: Current view's upward unit notmal
							//	  optional, defaulting to the Z axis 
	{
	RC rc = SUCCESS;

#ifdef THREE_D
	CNormal P(pUp);
	double rSin = Determinant(A, B, pUp);
#else
	double rSin = Determinant(A, B);
#endif

	double rCos = A * B;

	if (FZERO(rCos, FUZZ_GEN)  &&  FZERO(rSin, FUZZ_GEN))
		{
		rc = E_ZeroVector;
		rAngle = 0.;
		}
	else
		rAngle = atan2(rSin, rCos);

	RETURN_RC(Angle, rc);
	}
/***********************************************************************/
// The cos of the angle between this and another vector
RC CosAngle(
	const C3Point & A,			// I: One vector
	const C3Point & B,			// I: The other vector
	double & rCosAngle)		   // O: Cosine of the angle
	{
	RC rc = SUCCESS;
	double r = A.Norm() * B.Norm();
	
	if (FZERO(r, FUZZ_GEN))
		rc = E_ZeroVector;
	else
		rCosAngle = A * B / r;

	RETURN_RC(CosAngle, rc);
	}

/***********************************************************************/
// The determinant of 2 vectors
double Determinant(		// Return the signed magnitude of A X B
	const C3Point & A,	// I: One vector	
	const C3Point & B,	// I: The other vector	
	C3Point * pUp)		// I: Unit vector poining upwards (NULL OK)
	{
#ifdef THREE_D
	// The default normal is (0,0,1);
	CNormal N(pUp);
	return Determinant(A, B, N);
#else
	// Ignore the normal
	return A.X() * B.Y() - A.Y() * B.X();
#endif
	}
/***********************************************************************/
// The determinant of 3 vectors
double Determinant(		// Return the determinant |A,B,C| or |A,B| if 2D
	const C3Point & A,	// I: The 3 vectors	
	const C3Point & B,	
	const C3Point & C)	// Ignored if 2D
	{
#ifdef THREE_D
	return A.X() * B.Y() * C.Z() + 
		   A.Y() * B.Z() * C.X() + 
		   A.Z() * B.X() * C.Y() - 
		   A.Z() * B.Y() * C.X() - 
		   A.Y() * B.X() * C.Z() - 
		   A.X() * B.Z() * C.Y();
#else
	return A.X() * B.Y() - 
		   A.Y() * B.X(); 
#endif
	}
/***********************************************************************/
//The distance between 2 points
double  PointDistance(
	const C3Point & A,	//In: One point
	const C3Point & B)	//In: The other point
    {
	C3Point D = A - B;
	return D.Norm();
    }
/***********************************************************************/
// Collinearity test between a point and the line thru 2 other points
BOOL Collinear(
	const C3Point & A,	// I: Line's first point
	const C3Point & B,	// I: Line's other point
	const C3Point & P,	// I: The point
	double rTolerance) 	// I: Tolerated distance from the line AB
    {
    C3Point AP = P - A;
    C3Point AB = B - A;
    double t = AB.Norm();

	// The area of the triangle ABP equals one half of AB times the 
	// height, which is the distance from P to the line AB.  The area
	// of that triangle is the magnitude of the cross product AB X AP, so
 	return FZERO(t, rTolerance) || 
 		   FZERO(Determinant(AB, AP), 2 * t * rTolerance);
    }


#ifdef DEBUG
ostream& operator<<(ostream &out, const C3Point &pt)
{
		out << "(" << pt.m_x[0] << ", " << pt.m_x[1];
#ifdef THREE_D
		out << ", " << pt.m_x[2] << "); " << endl;
#else
		out << "); " << endl;
#endif
		return out;
}
#endif // DEBUG

