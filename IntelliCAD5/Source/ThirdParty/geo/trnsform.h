/*	Trnform.h - Defines the class of transformations and its subclass of affine
 *  transformations.
 *	Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 *
 *	ABSTRACT
 *
 *  A general transformation is a black box, defined by the way it transforms a 
 *  point and a vector.  An affine transformation is a special case thereof, a
 *  combination of a linear transfomation (3X3 matrix) and a translation (by a point)
 *
 */
#ifndef TRNSFORM_H
#define TRNSFORM_H

#include "Array.h"	// for the  base class
#include "Point.h"	// for C3Point
#include "Vector.h" // for C3Vector
#include "Matrix.h"

#ifdef THREE_D
	#define Matrix Matrix3X3
#else
	#define Matrix Matrix2X2
#endif

typedef double Matrix3X3[3][3];
typedef double Matrix2X2[2][2];

typedef enum {NO_FLIP=0, X_FLIP, Y_FLIP} FLIP_TYPE;
////////////////////////////////////////////////////////////////////////////////////////
// The class of transforamtions
class  GEO_CLASS CTransform		:		public CGeoObject
	{
public:
// Construction/destruction
	CTransform()
		{
		}

	virtual ~CTransform()
		{
		}

	// Methods
	virtual BOOL Affine()
		{
		return FALSE;
		}

	virtual RC ApplyToPoint(
		C3Point ptIn,				// In: The point to be transformed
		C3Point & ptOut) const=0;	// Out: the transformed point
 
	virtual RC ApplyToVector(	
		C3Point ptIn,			// In: The vector to be transformed
		C3Point & ptOut) const;	// Out: The transformed vector

	virtual RC ApplyToPointArray(
		const CPointArray & arrIn,   // In: The points to be transformed
		CPointArray & ptOut) const;  // Out: the transformed points

	virtual RC ApplyToPointArray(
		const CPointArray & cArray) const; // In/out: The points to be transformed

// No data
	};
////////////////////////////////////////////////////////////////////////////////////////
// The class of affine transforamtions
class  GEO_CLASS CAffine	:	public CTransform
	{
public:
// Construction/destruction
	CAffine();

	CAffine(
		const CAffine & cSecond,	// In: Second transformation
		const CAffine & cFirst);	// In: First transformation

	CAffine(
		double xFactor,		// In: The X sacling factor
		double yFactor);	// In: The Y sacling factor

	CAffine(
		P3Point pOrigin,	// In: The new origin
		P3Point pX,			// In: The new X axis
		P3Point pY,			// In: The new Y axis
		P3Point pZ);		// In: The new Z axis (Optional, NULL OK)

    // Construct an arbitrary axis transform
    CAffine(const C3Vector &zAxis);     // In: The new Z Axis

	CAffine(
		P3Point pAxis,      // In/Out:  Axis of rotation (must be non zero, we will unitize)
		double theta);      // In:  Angle of rotation (in radians)

	virtual ~CAffine()
		{
		}

	void InitializeMatrix();

	BOOL operator ==( const CAffine&) const;
	BOOL operator !=( const CAffine& other)	{ return !(*this == other);}

	// Methods
	BOOL Affine()
		{
		return TRUE;
		}

    void Set(const C3Point &origin, const C3Point &xAxis, const C3Point &yAxis, const C3Point &zAxis);

	void SetOriginAndX(
		const C3Point& ptOrigin,	// In: New position of the origin
		const C3Point& ptXAxis);	// In: New direction of the x axis

	RC ApplyToPoint(
		C3Point ptIn,			// In: The point to be transformed
		C3Point & ptOut) const;	// Out: the transformed point
 
 	RC ApplyToVector(
		C3Point ptIn,			// In: The vector to be transformed
		C3Point & ptOut) const;	// Out: the transformed vector

	void ApplyInverseToPoint(
		C3Point ptIn,			// In: The point to be transformed
		C3Point & ptOut) const;	// Out: the transformed point
 
 	void ApplyInverseToVector(
		C3Point ptIn,			// In: The vector to be transformed
		C3Point & ptOut) const;	// Out: the transformed vector

	RC RayToXAxis(
		const C3Point & ptOrigin,     // In: Ray's origin
		const C3Point & ptDirection); // In: Ray's direction unit vector

	void Set(
		BOOL bFlipX,	// In: Flip about the x axis if TRUE
		BOOL bFlipY);	// In: Flip about the y axis if TRUE

	void SetRotation(
		double rTheta);	// In: The rotation

	double GetRotation(
		FLIP_TYPE * pFlip=NULL) const;  // Out: flip factor (optional)

	void SetMove(
		const C3Point & ptBy);	//In: The desired translation

	void Move(
		const C3Point & ptBy);	//In: The desired translation

	void GetTranslation( 
	    C3Point & ptTranslate ) const; // Out: the current translation

	BOOL IsRotated() const;			// determine if includes rotation

	void Rotate(
		double rTheta);	// In: The rotation

	void RotateAbout(
		double rTheta,			 // In: The rotation
		const C3Point & center); // In: Center of rotation

	void RotateAbout(
		double rTheta,			 // In: The rotation
		const C3Vector & axis);	 // In: Axis of rotation

	void Scale(
		double rXScale,		// In: The X sacling factor
		double rYScale);	// In: The Y sacling factor

	double GetXScale() const;			
	double GetYScale() const;			
	void SetScale(
		double rFactor);	// In: The sacling factor

	void Combine(
		const CAffine & cOther)	// In: Another transformation
		{
		// The result will operate as Result(P) = Other(this(P)) 
		*this = CAffine(cOther, *this);	
		}
public:
	void SetFromScratch(
		double m00,	// In: matrix[0][0]
		double m01,	// In: matrix[0][1]
		double m10,	// In: matrix[1][0]
		double m11,	// In: matrix[1][1]
		double t0,	// In: translation[0]
		double t1);	// In: translation[1]

#ifdef THREE_D
	void SetFromScratch(
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
		double t2);	// In: translation[2]
#endif

	RC SetInverse(
		const CAffine & pOther); // In: The other transforamtion

	void ResetScale(
		double rScale);		//In: The desired x scale

	BOOL IsXY() const;

#ifdef DEBUG
	void Dump();
#endif

	C3Matrix	GetMatrix() const;
	C3Point		GetTranslation() const { return m_ptTranslate; }

// Data
protected:
	Matrix   m_Matrix;			// The matrix part
	C3Point  m_ptTranslate;		// The transoation portion
	};

//////////////////////////////////////////////////////////////
// Matrix * point
C3Point  operator * (
	Matrix2X2 & M,			// I: The matrix
	const C3Point & P);		// I: The vector

C3Point  operator * (
	const Matrix2X2 & M,	// I: The matrix
	const C3Point & P);		// I: The vector

#ifdef THREE_D
C3Point  operator * (
	Matrix3X3 & M,			// I: The matrix
	const C3Point & P);		// I: The vector

C3Point  operator * (
	const Matrix3X3 & M,	// I: The matrix
	const C3Point & P);		// I: The vector
#endif

//////////////////////////////////////////////////////////////
// Non-member functions that use the CAffine class

GEO_API RC Orthogonal2Pt(  // FAILURE if the P[i] or Q[i] are coincident
	C3Point P[2], // In:  Points in XY plane to be transformed by T
	C3Point Q[2], // In:  Points in XY plane that should equal T(P[i])  
	CAffine & T);  // Out: T is orthogonal and satisfies T(P[i]) = Q[i]

GEO_API RC Affine3Pt(	  // FAILURE if the P[i] or Q[i] are collinear
	C3Point P[3], // In:  Points in XY plane to be transformed by T
	C3Point Q[3], // In:  Points in XY plane that should equal T(P[i]).
	CAffine & T); // Out: T is affine and satisfies T(P[i]) = Q[i]
#endif

