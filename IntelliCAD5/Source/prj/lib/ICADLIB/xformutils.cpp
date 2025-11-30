/* G:\ICADDEV\PRJ\LIB\ICADLIB\XFORMUTILS.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * This file contains functions that construct transformation entities represented
 * as sds_matrix objects.
 * 
 */ 


#include "icadlib.h"/*DNT*/

#ifndef INTELLICAD
#define INTELLICAD // needed for the following Geo include statements
#endif
#include "Geometry.h"/*DNT*/
#include "Point.h"/*DNT*/
#include "Trnsform.h"/*DNT*/


void CAffine_to_sds_matrix(const CAffine & A, // I: a Geo affine transformation
						   sds_matrix B);     // O: the corresponding sds_matrix

// methods for the class CPlane 

// construct a plane given a point and normal
CMirrorPlane::CMirrorPlane(C3Point P, C3Point N, RC & rc) :
	m_Plane(P,N)
		{
		rc = SUCCESS;
		}
	
RC CMirrorPlane::ApplyToPoint(C3Point ptIn,
		                      C3Point & ptOut) const
		{
		RC rc = SUCCESS;
		ptOut = m_Plane.Mirror(ptIn);
		return rc;
		}

RC CMirrorPlane::ApplyToVector(C3Point ptIn,
							   C3Point & ptOut) const
	{
	RC rc = SUCCESS;
	C3Point Origin(0,0,0);
	ptOut = m_Plane.Mirror(ptIn) - m_Plane.Mirror(Origin);
	return rc;
	}


// helper function, not a member function of any class right now
CAffine AffineRep(CTransform & T) // I - a transformation that we know is affine
	{
	C3Point oldOrigin(0,0,0);
	C3Point oldXdir(1,0,0);
	C3Point oldYdir(0,1,0);
	C3Point oldZdir(0,0,1);
	
	C3Point newOrigin;
	C3Point newXdir;
	C3Point newYdir;
	C3Point newZdir;
	
	T.ApplyToPoint(oldOrigin, newOrigin);
	T.ApplyToVector(oldXdir, newXdir);
	T.ApplyToVector(oldYdir, newYdir);
	T.ApplyToVector(oldZdir, newZdir);
	
    CAffine A(&newOrigin, &newXdir, &newYdir, &newZdir);
	
	return A;
	}

// Construct a transformation that mirrors about a given plane
void ic_mirrorPlane(sds_point ptOnPlane, // I - point on the plane
					sds_point vecNormalToPlane, // I - normal vector to the plane
					bool & ok,  // O - true if and only if mtx can be constructed
					sds_matrix mtx) // O - transform that mirrors about the plane
	{
	C3Point P(ptOnPlane[0], ptOnPlane[1], ptOnPlane[2]);
	C3Point N(vecNormalToPlane[0], vecNormalToPlane[1], vecNormalToPlane[2]);
	RC rc = SUCCESS; // innocent until proven guilty
	CMirrorPlane TMirrorPlane(P, N, rc);
	if (rc == SUCCESS)
		{
		CAffine T = AffineRep(TMirrorPlane);
		CAffine_to_sds_matrix(T, mtx);
		ok = true;
		}
	else
		{
		ok = false;
		}
	}

// Construct a transformation that mirrors about a given line in the XY plane
void ic_mirrorLine(sds_point pt1OnLine, // I - first point on the line 
				   sds_point pt2OnLine, // I - second point on the line
				   bool & ok,      // O - true if and only if mtx can be constructed
				   sds_matrix mtx)      // O - transform that mirrors about the line
	{
	if ((icadRealEqual(pt1OnLine[2],0.0)) && (icadRealEqual(pt2OnLine[2],0.0)))
		{
		sds_point diff;
		sds_point norm;
		ic_sub(pt1OnLine, pt2OnLine, diff);
		norm[0] = -diff[1];
		norm[1] = diff[0];
		norm[2] = 0.0;
		ic_mirrorPlane(pt1OnLine, norm, ok, mtx);
		}
	else
		{
		ok = false;
		}
	}


// Construct a transformation that rotates by a specified angle theta about an axis
// that is parallel to the Z axis and that passes through a specified point basept.
void ic_rotxform(sds_point basept, // I: A point through which the axis of rotation passes
				 double theta,     // I: The angle of rotation
				 sds_matrix mtx)   // O: Transformation that rotates theta about axis parallel
								   //    to the z axis passing through basept	
	{
	
	double cs = cos(theta);
	double sn = sin(theta);
	
    mtx[0][0]=cs; mtx[0][1]=(-sn); mtx[0][2]=0.0; mtx[0][3]=((basept[1]*sn)+(basept[0]*(1-cs)));
    mtx[1][0]=sn; mtx[1][1]=cs;    mtx[1][2]=0.0; mtx[1][3]=(-(basept[0]*sn)+(basept[1]*(1-cs)));
    mtx[2][0]=0.0;        mtx[2][1]=0.0;           mtx[2][2]=1.0; mtx[2][3]=0.0;
    mtx[3][0]=0.0;        mtx[3][1]=0.0;           mtx[3][2]=0.0; mtx[3][3]=1.0;
	}

// Construct a transformation that rotates by a specified angle theta about an axis
// that is specified by a base point and an extrusion vector.
void ic_rotxform(sds_point basept,        // I: A point through which the axis of rotation passes
				 sds_point extrusionvec,  // I: A vector in the direction of the axis of rotation
				                          //    This vector must be nonzero but does not need to
										  //    be of length one.
				 double theta,            // I: The angle of rotation   
				 sds_matrix mtx)          // O: Transformation that rotates theta about axis
				                          //    determined by basept and extrusionvec
	{
	// convert the input points to Geo points
	C3Point B(basept[0], basept[1], basept[2]);
	C3Point MinusB(-basept[0], -basept[1], -basept[2]);
	C3Point E(extrusionvec[0], extrusionvec[1], extrusionvec[2]);
	// construct the desired transformation as the composition of three simpler transformations
	// Let T be the transform that translates by B
	// Let Tinv be the transform that translates by -B
	// Let R be the transform that rotates about an axis having direction E by theta
	// Then the desired transform M applied to a point P is defined as
	// M(P) = T(R(Tinv(P)))

	CAffine T;
	T.SetMove(B);

    CAffine R(&E, theta);

	CAffine Tinv;
	Tinv.SetMove(MinusB);

	CAffine M(R, Tinv);
	M.Combine(T);

	// convert this transformation to an sds_matrix
	
    CAffine_to_sds_matrix(M, mtx);
	}


void ic_idmat (sds_matrix m)
	{
	int i, j;
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			m[i][j] = 0.0;
	m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.0;
	}

void ic_transmat (double x, double y, double z, sds_matrix m)
	{
	ic_idmat (m);
	m[0][3] = x;
	m[1][3] = y;
	m[2][3] = z;
	}

// Find the inverse of an sds_matrix
void ic_invertmatrix(sds_matrix mtx,    // I: the sds_matrix to be inverted
					 bool & invertible, // O: true if and only if mtx is invertible
					 sds_matrix inv)    // O: the inverse of mtx if it is invertible
	{


	// Represent the sds_matrix object as a CAffine object

	C3Point   newXdir(mtx[0][0], mtx[1][0], mtx[2][0]);
	C3Point   newYdir(mtx[0][1], mtx[1][1], mtx[2][1]);
	C3Point   newZdir(mtx[0][2], mtx[1][2], mtx[2][2]);
	C3Point newOrigin(mtx[0][3], mtx[1][3], mtx[2][3]);

	CAffine T(&newOrigin, &newXdir, &newYdir, &newZdir);

	// Try to find the inverse of the CAffine object
	CAffine Tinv;
	RC rc = Tinv.SetInverse(T);

	// If we were successful, convert the CAffine object to an sds_matrix
	if (rc == SUCCESS)
		{
		CAffine_to_sds_matrix(Tinv, inv);
		invertible = true;
		}
	else
		{
		invertible = false;
		}


	}


// This function converts a CAffine object to an sds_matrix - Copied from cmds/GeoConvert.cpp
void CAffine_to_sds_matrix(const CAffine & A, // I: a Geo affine transformation
						   sds_matrix B)      // O: the corresponding sds_matrix
{
	// The defining data for an object of the CAffine class is
	// hidden, so we can only obtain that information by seeing 
	// how A acts on various input points and vectors.

    // We will use A.ApplyToVector for these three
	C3Point XDir(1.0, 0.0, 0.0);
	C3Point YDir(0.0, 1.0, 0.0);
	C3Point ZDir(0.0, 0.0, 1.0);

	// We will use A.ApplyToPoint for this one.
    C3Point Zero(0.0, 0.0, 0.0);

	// This will be the output to be assigned to the appropriate column of B.
	C3Point P;

	A.ApplyToVector(XDir, P);
	B[0][0] = P.X();
	B[1][0] = P.Y();
	B[2][0] = P.Z();
	B[3][0] = 0.0;

	A.ApplyToVector(YDir, P);
	B[0][1] = P.X();
	B[1][1] = P.Y();
	B[2][1] = P.Z();
	B[3][1] = 0.0;

	A.ApplyToVector(ZDir, P);
	B[0][2] = P.X();
	B[1][2] = P.Y();
	B[2][2] = P.Z();
	B[3][2] = 0.0;

	A.ApplyToPoint(Zero, P);
	B[0][3] = P.X();
	B[1][3] = P.Y();
	B[2][3] = P.Z();
	B[3][3] = 1.0;
}

// Given two vectors uFrom and uTo, construct an sds_matrix
// that takes the unitized uFrom to the unitized uTo.
// It is the responsibility of the calling routine to make
// sure that neither uFrom nor uTo is the zero vector.
// Note that generally there is not a unique solution to this problem.
void ic_rotxform(sds_point uFrom, sds_point uTo, sds_matrix mtx)
	{
	ic_unitize(uFrom);
	ic_unitize(uTo);

	// determine the angle between uFrom and uTo as well as the axis of rotation

	sds_point vAxis;
	ic_crossproduct(uFrom, uTo, vAxis);

	double len = ic_veclength(vAxis);

	double dotProd = ic_dotproduct(uFrom, uTo);

	double theta;

    double pi = 4*atan(1.0);


	if (len> IC_ZRO)
		{  // begin general case processing
		theta = acos(dotProd);
		}  //   end general case processing
	else
		{  // begin special case processing
		if(uFrom[0]==uTo[0] && uFrom[1]==uTo[1] && uFrom[2]==uTo[2])	/*D.Gavrilov*/// don't rotate
			theta = 0.0;												// from a vector to the same
		else
			theta = pi;
			// compute an arbitrary axis that is orthogonal to uFrom
			ASSERT( !icadRealEqual(uFrom[0],0.0) || !icadRealEqual(uFrom[1],0.0) || !icadRealEqual(uFrom[2],0.0));
			if (!icadRealEqual(uFrom[0],0.0))
				{
				vAxis[0] = uFrom[1];
				vAxis[1] = -uFrom[0];
				vAxis[2] = 0.0;
				}
			else if (!icadRealEqual(uFrom[1],0.0))
				{
				vAxis[0] = -uFrom[1];
				vAxis[1] = uFrom[0];
				vAxis[2] = 0.0;
				}
			else if (!icadRealEqual(uFrom[2],0.0))
				{
				vAxis[0] = 0.0;
				vAxis[1] = -uFrom[2];
				vAxis[2] = uFrom[1];
				}
				
				
		}  //   end special case processing

	sds_point basept;
	basept[0] = basept[1] = basept[2] = 0.0;

	ic_rotxform(basept, vAxis, theta, mtx);

	}

void  ic_xformpt(sds_point from, // I:  the point to be transformed by mat
				 sds_point to,   // O:  the image of from under the transformation mat
				 sds_matrix mat) // I:  the transformation
	{
/*
**  Applies an ADS-type matrix to a point.
**  'from and 'to' can be the same point in memory.
*/
    sds_point ap1;

    ap1[0]=mat[0][0]*from[0]+mat[0][1]*from[1]+mat[0][2]*from[2]+mat[0][3];
    ap1[1]=mat[1][0]*from[0]+mat[1][1]*from[1]+mat[1][2]*from[2]+mat[1][3];
    ap1[2]=mat[2][0]*from[0]+mat[2][1]*from[1]+mat[2][2]*from[2]+mat[2][3];
    to[0]=ap1[0]; to[1]=ap1[1]; to[2]=ap1[2];
}

/*-------------------------------------------------------------------------*//**
Transpose sds_matrix
@author Alexey Malov
@param <=> matrix to transpose
*//*--------------------------------------------------------------------------*/
void ic_transpose(sds_matrix mx)
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = i+1; j < 4; j++)
        {
            double t = mx[i][j];
            mx[i][j] = mx[j][i];
            mx[j][i] = t;
        }
    }
}

/*-------------------------------------------------------------------------*//**
Copy sds_matrix (a = b)
@author Alexey Malov
@param <= copy
@param => original
*//*--------------------------------------------------------------------------*/
void ic_copy(sds_matrix a, sds_matrix b)
{
    for (int i = 0; i < 4; i++)
    {
        a[i][0] = b[i][0];
        a[i][1] = b[i][1];
        a[i][2] = b[i][2];
        a[i][3] = b[i][3];
    }
}
