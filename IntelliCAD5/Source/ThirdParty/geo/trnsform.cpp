/*	Trnsform.cpp - Implements the class CTransform and it subclass CAffine 
 *	Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 */
#include "Geometry.h"
#include "Point.h"
#include "PtArray.h"		// For CPointArray
#include "Trnsform.h"

DB_SOURCE_NAME;

//////////////////////////////////////////////////////////////////////////
//						General Transformation							//
//////////////////////////////////////////////////////////////////////////
RC CTransform::ApplyToVector(	
	C3Point ptIn,			// In: The vector to be transformed
	C3Point & ptOut) const	// Out: The transformed vector
	{
	// We should never be here!
	return FAILURE;
	}
/*********************************************************************/
// Apply to an array of points
RC CTransform::ApplyToPointArray(
	const CPointArray  & arrIn,   // In: The points to be transformed
	CPointArray  & arrOut) const  // Out: The transformed points
	{
	int i;
	int n = arrIn.Size();
	RC rc = SUCCESS;
	// Better not write outside the array bounds
	if (arrOut.Size() < n)
		{
		rc = E_BadInput;
		QUIT
		}

	// Apply to the points in the list
	for (i=0;  i<n;  i++)
		ApplyToPoint(arrIn[i], arrOut[i]);
exit:
	RETURN_RC(CTransform::ApplyToPointArraY, rc);
	}
/*********************************************************************/
// Apply to an array of points - in place
RC CTransform::ApplyToPointArray(
	const CPointArray & cArray) const // In/out: The points to be transformed
	{
	// Apply to the points in the list
	for (int i=0;  i<cArray.Size();  i++)
		ApplyToPoint(cArray[i], cArray[i]);
	RETURN_RC(CTransform::ApplyToPointArraY, SUCCESS);
	}
///////////////////////////////////////////////////////////////////////
//				Matrix * Point Multiplication						 //
///////////////////////////////////////////////////////////////////////

// Multiply by a 2X2 matrix from the left
C3Point  operator * (
	Matrix2X2 & M,			// I: The matrix
	const C3Point & P)		// I: The vector
    {
#ifndef THREE_D
	return C3Point(M[0][0] * P.X() + M[0][1] * P.Y(),
				   M[1][0] * P.X() + M[1][1] * P.Y());
#else
	return C3Point(M[0][0] * P.X() + M[0][1] * P.Y(),
				   M[1][0] * P.X() + M[1][1] * P.Y(),
				   P.Z());
#endif
    }

/*********************************************************************/
// Multiply by a const 3X3 matrix from the left
C3Point  operator * (
	const Matrix2X2 & M,	// I: The matrix
	const C3Point & P)		// I: The vector
    {
#ifndef THREE_D
	return C3Point(M[0][0] * P.X() + M[0][1] * P.Y(),
				   M[1][0] * P.X() + M[1][1] * P.Y());
#else
	return C3Point(M[0][0] * P.X() + M[0][1] * P.Y(),
				   M[1][0] * P.X() + M[1][1] * P.Y(),
				   P.Z());
#endif
    }

#ifdef THREE_D
/*********************************************************************/
// Multiply by a 3X3 matrix from the left
C3Point  operator * (
	Matrix3X3 & M,			// I: The matrix
	const C3Point & P)		// I: The vector
    {
	return C3Point(M[0][0] * P.X() + M[0][1] * P.Y() + M[0][2] * P.Z(),
				   M[1][0] * P.X() + M[1][1] * P.Y() + M[1][2] * P.Z(),
				   M[2][0] * P.X() + M[2][1] * P.Y() + M[2][2] * P.Z());
    }
/*********************************************************************/
// Multiply by a constant 3X3 matrix from the left
C3Point  operator * (
	const Matrix3X3 & M,	// I: The matrix
	const C3Point & P)		// I: The vector
    {
	return C3Point(M[0][0] * P.X() + M[0][1] * P.Y() + M[0][2] * P.Z(),
				   M[1][0] * P.X() + M[1][1] * P.Y() + M[1][2] * P.Z(),
				   M[2][0] * P.X() + M[2][1] * P.Y() + M[2][2] * P.Z());
    }
#endif

///////////////////////////////////////////////////////////////////////
//						Affine Transformations						 //
///////////////////////////////////////////////////////////////////////
// Default: construct the identity transformation
CAffine::CAffine()
	{
	InitializeMatrix();
	m_ptTranslate = C3Point(0,0);
	}
/*********************************************************************/
// Initialize the matrix
void CAffine::InitializeMatrix()
	{
	memset(m_Matrix, 0, sizeof(m_Matrix));
	m_Matrix[0][0] = m_Matrix[1][1] = 1;
#ifdef THREE_D
	m_Matrix[2][2] = 1;
#endif
	}

/*********************************************************************/
// Set an XY transformation from raw data
void CAffine::SetFromScratch(
	double m00,	// In: matrix[0][0]
	double m01,	// In: matrix[0][1]
	double m10,	// In: matrix[1][0]
	double m11,	// In: matrix[1][1]
	double t0,	// In: translation[0]
	double t1)	// In: translation[1]
	{
	InitializeMatrix();
	m_Matrix[0][0] = m00;
	m_Matrix[0][1] = m01;
	m_Matrix[1][0] = m10;
	m_Matrix[1][1] = m11;
	m_ptTranslate = C3Point(t0, t1);
	}

/*********************************************************************/
// Set a 3D transformation from raw data
#ifdef THREE_D
void CAffine::SetFromScratch(
		double m00,	// In: matrix[0][0]
		double m01,	// In: matrix[0][1]
		double m02,	// In: matrix[0][2]
		double m10,	// In: matrix[1][0]
		double m11,	// In: matrix[1][1]
		double m12,	// In: matrix[1][2]
		double m20,	// In: matrix[2][0]
		double m21,	// In: matrix[2][1]
		double m22,	// In: matrix[2][2]
		double t0,	// In: translation[0]
		double t1,	// In: translation[1]
		double t2)	// In: translation[2]
	{
	InitializeMatrix();
	m_Matrix[0][0] = m00;
	m_Matrix[0][1] = m01;
	m_Matrix[0][2] = m02;
	m_Matrix[1][0] = m10;
	m_Matrix[1][1] = m11;
	m_Matrix[1][2] = m12;
	m_Matrix[2][0] = m20;
	m_Matrix[2][1] = m21;
	m_Matrix[2][2] = m22;
	m_ptTranslate = C3Point(t0, t1, t2);
	}
#endif

/*********************************************************************/
// Set as flips about the X,Y coordinate axes
void CAffine::Set(
	BOOL bFlipX,	// In: Flip about the x axis if TRUE
	BOOL bFlipY)	// In: Flip about the y axis if TRUE
	{
	InitializeMatrix();
	m_ptTranslate = C3Point(0, 0);

	if (bFlipX)
		m_Matrix[0][0] = -1;
	else 
		m_Matrix[0][0] = 1;

	if (bFlipY)
		m_Matrix[1][1] = -1;
	else 
		m_Matrix[1][1] = 1;
	}
/*********************************************************************/
// Set as a rotation about the origin
void CAffine::SetRotation(
	double rTheta)	// In: The rotation
	{
	InitializeMatrix();
	m_ptTranslate = C3Point(0, 0);

	double rCos, rSin;

	// Get an angle between 0 and 2Pi
	while (rTheta >= TWOPI)
		rTheta -= TWOPI;
	while (rTheta < 0)
		rTheta += TWOPI;


	if (FZERO(rTheta, FUZZ_RAD))
		{
		rCos = 1;
		rSin = 0;
		}
	else if (FEQUAL(rTheta, PIOVER2, FUZZ_RAD))
		{
		rCos = 0;
		rSin = 1;
		}
	else if (FEQUAL(rTheta, PI, FUZZ_RAD))
		{
		rCos = -1;
		rSin = 0;
		}
	else if (FEQUAL(rTheta, THREEPIOVER2, FUZZ_RAD))
		{
		rCos = 0;
		rSin = -1;
		}
	else
		{
		rCos = cos(rTheta);
		rSin = sin(rTheta);
		}

	m_Matrix[0][0] = m_Matrix[1][1] = rCos;
	m_Matrix[0][1] = -rSin;
	m_Matrix[1][0] = rSin;
	}

double CAffine::GetRotation(
	FLIP_TYPE * pFlip) const // Out: flip factor (optional)
	{
/* This method returns the rotation angle of this transfomration.  The 
   transformation is assumed to have no shear component, otherwise an 
   assertion will be tripped.  If it has any scaling component then it is
   assumed to be isotropic (same factor on X and Y), otherwise an 
   assertion will be trippred. If the caller passes no flip argument then 
   (apart from an isotropic scaling) the transformation is assumed to be a 
   pure rotation; an assertion will be tripped if it has a flip component.
   If the caller passes a flip argument then this transformation will be
   factored as a rotation  between -PI and PI, possibley followd by an
   X flip, or a Y flip.  The choice betwen X and Y flip will be made to
   minimize the rotation angle.  Thus, if there is a flip, the returned 
   rotation will be between -PI/2 and PI/2.
*/
#ifdef DEBUG
	// Check for unisotropic scaling and shear
	double r1 = m_Matrix[0][0] * m_Matrix[0][0] + 
				m_Matrix[0][1] * m_Matrix[0][1];
	double r2 = m_Matrix[1][0] * m_Matrix[1][0] +
				m_Matrix[1][1] * m_Matrix[1][1];
	double e = MAX(r1, r2) * FUZZ_DIST;
	ASSERT(FEQUAL(r1, r2, e));

	// Check that the rows are orthogonal (no shear)
	ASSERT(FZERO(m_Matrix[0][0] * m_Matrix[1][0] +
				 m_Matrix[0][1] * m_Matrix[1][1], e));
#endif
	
	double det = m_Matrix[0][0] * m_Matrix[1][1] - 
				 m_Matrix[0][1] * m_Matrix[1][0];
	double angle = atan2( m_Matrix[1][0], m_Matrix[1][1]);

	if (!pFlip)
		{
		// Not supposed to have a flip factor
		ASSERT(det > 0);
		goto exit;
		}

	ASSERT(pFlip);
	if (det > 0)
		*pFlip = NO_FLIP;
	else
		{
		angle = -angle;
		if (fabs(angle) > PIOVER2)
			// We can do it with a smaller angle
			{
			*pFlip = Y_FLIP;
			if (angle > PIOVER2)
				angle -= PI;
			else
				angle += PI;
			}
		else
			*pFlip = X_FLIP;
	}
exit:
	return angle;
	}


/*********************************************************************/
// Construct a scaling transformation
CAffine::CAffine(
	double xFactor,	// In: The X sacling factor
	double yFactor)	// In: The Y sacling factor
	{
	InitializeMatrix();
	m_Matrix[0][0] = xFactor;
	m_Matrix[1][1] = yFactor;
	}

/*********************************************************************
 *  Author: Stephen W. Hou
 *  Date:   6/20/2004
 */
CAffine::CAffine(const C3Vector &zAxis)
{
    InitializeMatrix();

    C3Point origin;
    C3Vector x(1.0, 0.0, 0.0);
    C3Vector y(0.0, 1.0, 0.0);
	C3Vector z = zAxis.Normalize();

    if (!FEQUAL(z.Z(), 1.0, FUZZ_RAD)) {
	    double cap = 1./64.;

        if (FEQUAL(z.X(), 0., cap) && FEQUAL(z.Y(), 0., cap)) 
            x.Set(z.Z(), 0.0, - z.X());
        else 
            x.Set(- z.Y(), z.X(), 0.);

	    double xLength = x.Magnitude();
        if (FZERO(xLength, FUZZ_DIST))
            return;

	    x /= xLength;
	    y = z ^ x;
    }
    Set(origin, x.AsPoint(), y.AsPoint(), z.AsPoint());
}

#ifdef THREE_D
/*********************************************************************/
// Construct the affine transformation that rotates about a given axis
// by a specified angle.
CAffine::CAffine(
	P3Point pAxis,      // In/Out:  Axis of rotation (must be non zero, we will unitize)
	double theta)       // In:  Angle of rotation (in radians)
	{
	// Uses the formula in Faux and Pratt on the top of page 73
    pAxis->Unitize();

	// We will follow the notation used in Faux and Pratt
	double u1 = pAxis->X();
	double u2 = pAxis->Y();
	double u3 = pAxis->Z();

	double cs = cos(theta);
	double sn = sin(theta);

	m_ptTranslate = C3Point(0.0, 0.0, 0.0);

	m_Matrix[0][0] = u1*u1 + cs*(1.0 - u1*u1);
	m_Matrix[1][1] = u2*u2 + cs*(1.0 - u2*u2);
	m_Matrix[2][2] = u3*u3 + cs*(1.0 - u3*u3);

	m_Matrix[0][1] = u1*u2*(1.0 - cs) - u3*sn;
	m_Matrix[1][0] = u1*u2*(1.0 - cs) + u3*sn;

	m_Matrix[0][2] = u1*u3*(1.0 - cs) + u2*sn;
	m_Matrix[2][0] = u1*u3*(1.0 - cs) - u2*sn;

	m_Matrix[1][2] = u2*u3*(1.0 - cs) - u1*sn;
	m_Matrix[2][1] = u2*u3*(1.0 - cs) + u1*sn;
}
#endif

/*********************************************************************/
// Set to the transformation defined by new XY coordinate system
void CAffine::SetOriginAndX(
	const C3Point & ptOrigin,	// In: New position of the origin
	const C3Point & ptXAxis)	// In: New direction of the x axis
	{
	InitializeMatrix();

#ifdef THREE_D
/*
 * The resulting transformation is a combination of tranlation to ptOrigin, 
 * rotation so that the new direction of the x axis coincides with ptXAis, and
 * uniform scaling by the magnitue of ptDirection.  It is only valid if the
 * new X axs is in the old XY plane
 */
	ASSERT(ptXAxis.Z() == 0);
#endif

	m_Matrix[0][0] = ptXAxis.X();
	m_Matrix[0][1] = -ptXAxis.Y();

	m_Matrix[1][0] = ptXAxis.Y();
	m_Matrix[1][1] = ptXAxis.X();

	// The translation part
	m_ptTranslate = ptOrigin;
	}
/*********************************************************************/
// Apply to a point
RC CAffine::ApplyToPoint(
	C3Point ptIn,			// In: The point to be transformed
	C3Point & ptOut) const	// Out: The transformed point
	{
	ptOut = m_Matrix * ptIn + m_ptTranslate;
	return SUCCESS;
	}
/*********************************************************************/
// Apply to a vector
RC CAffine::ApplyToVector(
	C3Point ptIn,			// In: The vector to be transformed
	C3Point & ptOut) const  // Out: the transformed vector
	{
	ptOut = m_Matrix * ptIn;
	return SUCCESS;
	}
/*********************************************************************/
// Apply to a point
void CAffine::ApplyInverseToPoint(
	C3Point ptIn,			// In: The point to be transformed
	C3Point & ptOut) const	// Out: The transformed point
	{
	// Apply as inverse to the difference vector
	ApplyInverseToVector(ptIn - m_ptTranslate, ptOut);
	}
/*********************************************************************/
// Apply the inverse to a vector
void CAffine::ApplyInverseToVector(
	C3Point ptIn,			// In: The vector to be transformed
	C3Point & ptOut) const  // Out: the transformed vector
	{
	// Multiply inverse matrix values
	ptOut.SetX(m_Matrix[1][1] * ptIn.X() - m_Matrix[0][1] * ptIn.Y());
	ptOut.SetY(m_Matrix[0][0] * ptIn.Y() - m_Matrix[1][0] * ptIn.X());

	double rDet = m_Matrix[0][0] * m_Matrix[1][1] 
				- m_Matrix[1][0] * m_Matrix[0][1];

	if (rDet != 0)
		ptOut /= rDet;
	}
/*********************************************************************/
// The transformation that will take a given ray to the positive x axis
RC CAffine::RayToXAxis(
	const C3Point & ptOrigin,    // In: Ray's origin
	const C3Point & ptDirection) // In: Ray's direction unit vector
	{
	ASSERT (FEQUAL(ptDirection.Norm(), 1.0, FUZZ_GEN));

#ifdef THREE_D
	// This is a 2D transformation, in the XY plane
	ASSERT (ptDirection.Z() == 0);
#endif

	InitializeMatrix();
	m_Matrix[0][0] = ptDirection.X();
	m_Matrix[0][1] = ptDirection.Y();
	m_Matrix[1][0] = -ptDirection.Y();
	m_Matrix[1][1] = ptDirection.X();
	
	double rX = ptOrigin * ptDirection;
	double rY = Determinant(ptOrigin, ptDirection);
	m_ptTranslate = C3Point(-rX, rY);

	RETURN_RC(CAffine::RayToXAxis, SUCCESS);
	}
/*********************************************************************/
// The transformation to a new coordinate basis
CAffine::CAffine(
	P3Point pOrigin,	// In: The new origin
	P3Point pX,			// In: The new X axis
	P3Point pY,			// In: The new Y axis
	P3Point pZ)			// In: The new Z axis (Optional, NULL OK)
	{
    Set(*pOrigin, *pX, *pY, *pZ);
	}

/*********************************************************************/
// The transformation to a new coordinate basis
void CAffine::Set(const C3Point & origin,	// In: The new origin
	              const C3Point & xAxis,	// In: The new X axis
	              const C3Point & yAxis,	// In: The new Y axis
	              const C3Point & zAxis)	// In: The new Z axis (Optional, NULL OK)
{
	C3Vector xDir(xAxis / xAxis.Norm());
	C3Vector yDir(yAxis / yAxis.Norm());
#ifdef THREE_D
    C3Vector zDir;
    if (&zAxis != NULL) {
	    zDir = zAxis;
        zDir.Normalized();
    }
    else 
        zDir = xDir ^ yDir;

#endif

	for (int i=0;  i<DIMENSION;  i++)
		{
		// dnb - 5/21/98
		// Fixed this. Matrix was transposed.
		m_Matrix[i][0] = xDir[i];
		m_Matrix[i][1] = yDir[i];
#ifdef THREE_D
		m_Matrix[i][2] = zDir[i];
#endif
		}

	m_ptTranslate = origin;
}
/*********************************************************************/
// Construct as the combination of 2 transformations
CAffine::CAffine(
	const CAffine & cSecond,	// In: Second transformation
	const CAffine & cFirst)	// In: First transformation
	{
// The result will operate as Result(P) = Second(First(P))
	memset (m_Matrix, 0, sizeof(m_Matrix));
	for (int i = 0;   i < DIMENSION;   i++)
		for (int j = 0;   j < DIMENSION;   j++)
			for (int k = 0;   k < DIMENSION;   k++)
				m_Matrix[i][j] += cSecond.m_Matrix[i][k] * cFirst.m_Matrix[k][j]; 

	m_ptTranslate = cSecond.m_Matrix * cFirst.m_ptTranslate + cSecond.m_ptTranslate;
	}
/*********************************************************************/
// Determine equality of two transformations
BOOL CAffine::operator ==(const CAffine &other) const
	{
	for (int i = 0;   i < DIMENSION;   i++)
		for (int j = 0;   j < DIMENSION;   j++)
			if (m_Matrix[i][j] != other.m_Matrix[i][j])
				return FALSE;
	return m_ptTranslate == other.m_ptTranslate;
	}
/*********************************************************************/
// Construct the translation by a given vector
void CAffine::SetMove(
	const C3Point & ptBy)	//In: The desired translation
	{
	InitializeMatrix();
	m_ptTranslate = ptBy;
	}
/*********************************************************************/
// Add a translation by a given vector
void CAffine::Move(
	const C3Point & ptBy)	//In: The desired translation
	{
	m_ptTranslate += ptBy;	
	}

/*********************************************************************/
// Retrieve the translation
void CAffine::GetTranslation( 
	    C3Point & ptTranslate ) const // Out: the current translation
	{
	ptTranslate = m_ptTranslate;
	}

/*********************************************************************/
// determine if includes an XY rotation
BOOL CAffine::IsRotated() const
	{
	return( !(m_Matrix[1][0] == 0 && m_Matrix[0][1] == 0) );
	}
/*********************************************************************/
// Add an XY rotation by a given angle
void CAffine::Rotate(
	double theta)			//In: The desired rotation
	{
	CAffine	rotate;

	rotate.SetRotation(theta);
	*this = CAffine(rotate, *this);
	}
/*********************************************************************/
// Add a rotation by a given angle about a given center
void CAffine::RotateAbout(
	double rTheta,			// In: The rotation
	const C3Point & center)	// In: Center of rotation
	{
	Move(-center);
	Rotate(rTheta);
	Move(center);
	}

/*********************************************************************/
// Add a rotation by a given angle about an arbitrary axis passing
// through the origin.
// 
// Author: Stephen W. Hou
// Date:   4/6/2003
//
void CAffine::RotateAbout(
	double rTheta,				// In: The rotation
	const C3Vector & axis)		// In: Axis of rotation
	{
	double rCos = cos(rTheta);
	double rSin = sin(rTheta);
	double rV = 1.0 - rCos;
	m_Matrix[0][0] = axis[0] * axis[0] * rV + rCos;	
	m_Matrix[0][1] = axis[0] * axis[1] * rV - axis[2] * rSin;
	m_Matrix[0][2] = axis[0] * axis[2] * rV + axis[1] * rSin;

	m_Matrix[1][0] = axis[0] * axis[1] * rV + axis[2] * rSin;	
	m_Matrix[1][1] = axis[1] * axis[1] * rV + rCos; 
	m_Matrix[1][2] = axis[1] * axis[2] * rV - axis[0] * rSin; 

	m_Matrix[2][0] = axis[0] * axis[2] * rV - axis[1] * rSin; 
	m_Matrix[2][1] = axis[1] * axis[2] * rV + axis[0] * rSin; 
	m_Matrix[2][2] = axis[2] * axis[2] * rV + rCos; 
	}

/*********************************************************************/
// Combine with non-uniform scaling about the X and Y axes
void CAffine::Scale(
	double rXScale,	// In: The X sacling factor
	double rYScale)	// In: The Y sacling factor
	{
// The result will operate as Result(P) = ScaleY(this(P)) 
	m_Matrix[0][0] *= rXScale;	
	m_Matrix[0][1] *= rXScale;
	m_Matrix[1][0] *= rYScale;	
	m_Matrix[1][1] *= rYScale;
	m_ptTranslate = C3Point(rXScale * m_ptTranslate.X(), 
							rYScale * m_ptTranslate.Y());
	}
/*********************************************************************/
// Compute scale factor in X
double CAffine::GetXScale() const
	{
	C3Point	scaledXaxis;

	ApplyToVector(C3Point(1, 0), scaledXaxis);
	return scaledXaxis.Norm();
	}
/*********************************************************************/
// Compute scale factor in Y
double CAffine::GetYScale() const
	{
	C3Point	scaledYaxis;

	ApplyToVector(C3Point(0, 1), scaledYaxis);
	return scaledYaxis.Norm();
	}
/*********************************************************************/
// Construct a scaling transformation
void CAffine::SetScale(
	double rFactor)	// In: The sacling factor
	{
	InitializeMatrix();
	m_Matrix[0][0] = m_Matrix[1][1] = rFactor;
#ifdef THREE_D
	m_Matrix[2][2] = rFactor;
#endif
	m_ptTranslate = C3Point(0.0, 0.0);
	}

// The following are support utilities for finding the inverse of a 3D transformation
/*********************************************************************/
// get the indices involved in the cofactor of a 3 x 3 matrix
static void get_indices(int n, int & n0, int & n1)
	{
   switch (n)
		{
		case 0:
			n0 = 1;
			n1 = 2;
			break;
		case 1:
			n0 = 0;
			n1 = 2;
			break;
		case 2:
			n0 = 0;
			n1 = 1;
			break;
		}
	}
/*********************************************************************/
static double cofactor(const Matrix3X3 m, int i, int j)
	{
	double c;
	ASSERT((0 <= i) && (i <= 2));
	ASSERT((0 <= j) && (j <= 2));
	double fac;
	if ( (i + j) % 2 == 0)
		fac = 1;
	else
		fac = -1;

	int i0, i1;
	int j0, j1;

	get_indices(i, i0, i1);
	get_indices(j, j0, j1);

	c = fac*(m[i0][j0]*m[i1][j1] - m[i1][j0]*m[i0][j1]);

	return c;
	}
/*********************************************************************/

double det(const Matrix3X3 m)
	{
    return (m[0][0]*cofactor(m,0,0) + 
		    m[0][1]*cofactor(m,0,1) + 
			m[0][2]*cofactor(m,0,2));
	}
/*********************************************************************/
static RC inverse(const Matrix3X3 m, Matrix3X3  & minv)
	{
	RC rc;
	int i,j;
	double d = det(m);
	if (fabs(d) < FUZZ_GEN)
		rc = E_InputZero;
	else
		{
		for (i = 0; i < 3; i++)
			{
			for (j = 0; j < 3; j++)
				{
				minv[i][j] = cofactor(m,j,i)/d;
				}
			}
		rc = SUCCESS;
		}
	return rc;
	}
// The preceding are support utilities for finding the inverse of a 3D transformation


/*********************************************************************/
// Construct as the inverse of a 2D or 3D transformation
RC CAffine::SetInverse(
	const CAffine & other)	// In: The other transforamtion
	{

#ifndef THREE_D
   RC rc2D;
// This is only valid for 2D transformations
	ASSERT(other.IsXY()); 
	
	rc2D = E_InputZero;
	double rDet = other.m_Matrix[0][0] * other.m_Matrix[1][1] 
				- other.m_Matrix[1][0] * other.m_Matrix[0][1];

	if (FZERO(rDet, FUZZ_GEN))
		QUIT
	rc2D = SUCCESS;

	InitializeMatrix();
	m_Matrix[0][0] =  other.m_Matrix[1][1] / rDet;
	m_Matrix[1][0] = -other.m_Matrix[1][0] / rDet;
	m_Matrix[0][1] = -other.m_Matrix[0][1] / rDet;
	m_Matrix[1][1] =  other.m_Matrix[0][0] / rDet;


	m_ptTranslate = - (m_Matrix * other.m_ptTranslate);
exit:
	RETURN_RC(CAffine::Set, rc2D);
#else  // begin 3D case
	RC rc3D = inverse(other.m_Matrix, m_Matrix);

	if (rc3D == SUCCESS)
		m_ptTranslate = - (m_Matrix * other.m_ptTranslate);

	return rc3D;

#endif //  end 3D case
	}

/*********************************************************************/
// Reset the transform to have a given scaling factor
void CAffine::ResetScale(
	double rScale)		//In: The desired x scale
/*
 * This function assumes that the current transformation is a combination
 * of rotation and scale.  It figures out the scale portion, and without
 * changing the rotation or translation part, resets the matrix so that the 
 * scaling will be equal to the given rScale.
 */
	{
	// This is only valid for a 2D transformation in the XY plane
	ASSERT(IsXY());

	// Get the current x scaling factor
	double r = hypot(m_Matrix[0][0], m_Matrix[1][0]);
	
	if (r > FUZZ_GEN)
		{
		// Adjust the matrix by that ratio
		r = rScale / r;
		m_Matrix[0][0] *= r;
		m_Matrix[0][1] *= r;
		m_Matrix[1][0] *= r;
		m_Matrix[1][1] *= r;
		}
	}

/*********************************************************************/
// Is it a 2D transformation in the XY plane:
BOOL CAffine::IsXY() const
	{
#ifndef THREE_D
	return TRUE;
#else
	return 	m_Matrix[2][0] == 0 &&
			m_Matrix[2][1] == 0 && 
			m_Matrix[2][2] == 1 &&
			m_Matrix[0][2] == 0 &&  
			m_Matrix[1][2] == 0;
#endif
	}
#ifdef DEBUG
/***************************************************************************/
// Dump
void CAffine::Dump()
	{
	DB_MSGX("Affine transformation",());
	DB_MSGX("%lf   %lf   %lf",(m_Matrix[0][0], m_Matrix[0][1],m_ptTranslate.X() ));
	DB_MSGX("%lf   %lf   %lf",(m_Matrix[1][0], m_Matrix[1][1],m_ptTranslate.Y() ));
	DB_MSGX("\n",());
	}
#endif

//////////////////////////////////////////////////////////////////////
//																	//
//		Defining Affine Transformation by Points and their Targets	//
//																	//
/***************************************************************************/

// This function will construct an orthogonal transformation
// T such that T(P[i]) = Q[i] for i = 0,1
// In those cases where the P[i] or the Q[i] are coincident, 
// the return code RC will be failure

// The output transformation will be one that performs uniform scaling

// This is intended to be used only when the z coordinates of all points
// are zero.  We may relax this constraint later on.
RC Orthogonal2Pt(  // FAILURE if the P[i] or Q[i] are coincident
	C3Point P[2], // In:  Points in XY plane to be transformed by T
	C3Point Q[2], // In:  Points in XY plane that should equal T(P[i])  
	CAffine & T)  // Out: T is orthogonal and satisfies T(P[i]) = Q[i]
	{
	RC rc = SUCCESS; // innocent until proven guilty
	
	if ( P[0].Equal(P[1], FUZZ_DIST) || 
		Q[0].Equal(Q[1], FUZZ_DIST))
		{
		rc = FAILURE;
		}
	
	if (rc == SUCCESS)
		{  // begin case where the P's and Q's are distinct
		
		// Prepare the input data to send to Affine3Pt, which will
		// calculate the desired transformation
		
		C3Point R[3];
		C3Point S[3];
		int i;
		for (i = 0; i < 2; i++)
			{
			R[i] = P[i];
			S[i] = Q[i];
			}
		
		// It remains to determine R[2] and S[2] that will uniquely determine
		// an affine (in fact, orthogonal) transformation T such that T(R[i]) = S[i].
		
		C3Point RDelta = P[1] - P[0];
		RDelta.TurnLeft();
		C3Point SDelta = Q[1] - Q[0];
		SDelta.TurnLeft();
		
		R[2] = R[0] + RDelta;
		S[2] = S[0] + SDelta;
		
		rc = Affine3Pt(R, S, T);
		
		}  //   end case where the P's and Q's are distinct
	RETURN_RC(Orthogonal2Pt, rc);
	}
/***************************************************************************/

// This function will construct the unique affine transformation
// T such that T(P[i]) = Q[i] for i = 0,1,2
// In those cases where the P[i] or the Q[i] are collinear, 
// the return code RC will be failure

// This is intended to be used only when the z coordinates of all points
// are zero.  We may relax this constraint later on.
RC Affine3Pt(	  // FAILURE if the P[i] or Q[i] are collinear
	C3Point P[3], // In:  Points in XY plane to be transformed by T
	C3Point Q[3], // In:  Points in XY plane that should equal T(P[i]).
	CAffine & T)  // Out: T is affine and satisfies T(P[i]) = Q[i]
	{
	
	RC rc = SUCCESS; // innocent until proven guilty
	
#ifdef THREE_D	
	int i = 0;
	while ((rc == SUCCESS) && (i < 3))
		
		{
		if((P[i].Z() != 0.0) || (Q[i].Z() != 0.0))
			{
			rc = FAILURE;
			}
        ++i;
		}
#endif
	
	
	if (rc == SUCCESS)
		{  // begin case of all points in X-Y plane
		// First construct the matrix portion M of the transformation
		// This will be the transformation that takes P[1]-P[0] to Q[1]-Q[0]
		// and takes P[2]-P[0] to Q[2]-Q[0].
		
		
		C3Point U[2];
		C3Point V[2];
		
		U[0] = P[1] - P[0];
		U[1] = P[2] - P[0];
		V[0] = Q[1] - Q[0];
		V[1] = Q[2] - Q[0];
		
		
		// We are going to construct M such that M(U[i]) = V([i])
		
		// We will do this by first constructing an affine transformation
		// J such that J(1,0) = U[0] and J(0,1) = U[1], and an affine
		// transformation K such that K(1,0) = V[0] and K(0,1) = V[1].
		// Then we will construct M to be the composition of K with Jinv.
		// That is, M(R) = K(Jinv(R)).
		
		CAffine J;
		J.SetFromScratch(U[0].X(), U[1].X(),
			U[0].Y(), U[1].Y(),
			0.0, 0.0);
		
		CAffine K;
		K.SetFromScratch(V[0].X(), V[1].X(),
			V[0].Y(), V[1].Y(),
			0.0, 0.0);
		
		RC rc1;
		RC rc2;
		
		CAffine Jinv;
		rc1 = Jinv.SetInverse(J);
		
		CAffine Kinv;
		rc2 = Kinv.SetInverse(K);
		
		if ((rc1 == SUCCESS) && (rc2 == SUCCESS))
			{  // begin case of both transformations invertible
			
			// Construct the desired transformation as the composition of K with Jinv.
			CAffine M = CAffine(K, Jinv);
			
			
			
			// The translation component D is Q[0] - M(P[0])
			C3Point MofP0;
			M.ApplyToPoint(P[0], MofP0);
			C3Point D = Q[0] - MofP0;
			
			// Build the affine transformation L that translates by D.
			CAffine L;
			L.SetFromScratch(1.0, 0.0,
				0.0, 1.0,
				D.X(), D.Y());
			
			// Represent the desired transformation as the composition of M with L
			
			// Initialize T to M
			T = M;
			
			T.Combine(L);
			
			}  //  end case of both transformations invertible
		else
			{  // begin case of singular transformation 
			rc = FAILURE;
			}  //   end case of singular transformation
		
		}  // end case of all points in X-Y plane
	
	RETURN_RC(Affine3Pt, rc);
	}


/***************************************************************************/
//
// Return matrix part of transform
//
// Author:	Stephen W. Hou
// Date:	4/7/2003
//
C3Matrix CAffine::GetMatrix() const
{
	C3Matrix matrix;
	for (register i = 0; i < matrix.RowNumber(); i++) 
		for (register j = 0; j < matrix.ColumnNumber(); j++) 
			matrix(i, j) = m_Matrix[i][j];

	return C3Matrix(matrix);
}

