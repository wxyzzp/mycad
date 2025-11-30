/*	Constnts.h - 
 *	Copyright (C) 1998 Visio Corporation. All rights reserved.
 *
 *	Abstract
 *
 *	Mathematical constants
 *
 *
 */

#ifndef CONSTNTS_H
#define CONSTNTS_H


// PI related constants
#define PIOVER2 			1.5707963267949
#define PI					3.1415926535898
#define THREEPIOVER2		4.7123889803847
#define TWOPI				6.2831853071796
#define PI_OVER_180 		0.01745329252222
#define ONE_EIGHTY_OVER_PI	57.295779505601



// Fuzzy equality and zero tests for real numbers.
#define FUZZ_REAL	(1e-15)		// approximate floating point accuracy
#define FUZZ_GEN	(1e-9)		// general fuzz - nine decimal digits
#define FUZZ_DEG	(3e-4)		// approximately one arc second, as degrees
#define FUZZ_RAD	(5e-6)		// approximately one arc second, as radians
#define FUZZ_DIST	(1e-6)		// geometric coincidence toelrance

// Spline stuff
#define MAX_DEGREE 9
#define MAX_ORDER 10               // Should always be MAX_DEGREE+1
#define MAX_SPLINE_SIZE 1640       // Maximum number of control points
#define MAX_SCRIBBLE_POINTS 640    // Maximum number of scribbled data points

// Geometric infinity
#define INFINITY 1.e+30

#endif	// CONSTNTS_H

