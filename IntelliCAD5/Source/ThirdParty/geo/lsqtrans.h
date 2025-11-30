// The purpose of this file is to contain declarations of functions
// that use least squares fitting techniques to construct transformations

#ifndef LSQTRANS_H
#define LSQTRANS_H

#include "Geometry.h"
#include "Point.h"
#include "Trnsform.h"


GEO_API void OrthogonalNPt(int n, // In:  The number of points
				           C3Point * P, // In:  Array of n points
				           C3Point * Q, // In:  Array of n points
				           CAffine & T, // Out: Orthogonal transformation that best fits
				                // the data, in the least-squares sense
                           double * RMSerror, // Out: sqrt((1/n)*sum{dist(T(P[i])-Q[i])^2})
				           RC & rc); // Out:  Return code - SUCCESS or FAILURE

GEO_API void AffineNPt(int n, // In:  The number of points
			           C3Point * P, // In:  Array of n points
			           C3Point * Q, // In:  Array of n points
			           CAffine & T, // Out: Orthogonal transformation that best fits
				            // the data, in the least-squares sense
			           double * RMSerror, // Out: sqrt((1/n)*sum{dist(T(P[i])-Q[i])^2})
			           RC & rc); // Out:  Return code - SUCCESS or FAILURE
#endif
