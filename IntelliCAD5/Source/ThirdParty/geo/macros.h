/*	Macros.h - Macros for the geometry library
 *	Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 *
 */

#include <stdlib.h>         // for rand

#ifndef MACROS_H
#define MACROS_H

/*
 * Macro to check for allocation failure (out of memory)
 * A typical usage:
 *
 *		rc = ALLOCA_FAIL(p = new CMyClass(a, b, c));
 *
 * Result: rc = DM_MEM if the allocation failed, otherwise rc = SUCCESS
 */
#define ALLOC_FAIL(object) (object)?SUCCESS:DM_MEM


// A macro for swapping objects
// x and y MUST be names of variables of the given type
#define SWAP(x,y,type)\
	{\
	type z = x;\
	x = y;\
	y = z;\
	}

// A real random number between zero and one
#define FRAND	((double)rand()/(double)RAND_MAX)

#ifdef __cplusplus
// Conversions between degrees and radians
inline
double TO_DEGREES( double radians)
	{
	return radians * ONE_EIGHTY_OVER_PI;
	}


inline
double TO_RADIANS( double degrees)
	{
	return degrees * PI_OVER_180;
	}
#endif

// Comparisons withing tolerance
#define FEQUAL(a, b, fuzz)	(fabs(a - b) <= (fuzz))
#define FZERO(a, fuzz)		(fabs(a) <= (fuzz))

// Max/min
#ifndef MAX
#define MAX(a,b)                (((a) > (b)) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a,b)                (((a) < (b)) ? (a) : (b))
#endif

// This and that
#define DET2(a, b, c, d)		((a)*(d) - (b)*(c))
#define SQR(a) 					((a)*(a))

#ifndef ODD
#define ODD(n) ((n)&1)
#endif
#ifndef EVEN
#define EVEN(n) (!ODD(n))
#endif

#define QUIT_IF_FAILED(p){if (!p) rc = DM_MEM; if (rc) goto exit;}

#endif	/* MACROS_H */
