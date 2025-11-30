/* D:\ICADDEV\PRJ\LIB\CMDS\TANCIRCS.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * This file contains the declarations of functions that construct
 * a circle tangent to three entities, which may be any combination
 * of lines and circles.
 * 
 */ 

#ifndef TANCIRCS_H
#define TANCIRCS_H

#include "sds.h"


extern bool CirTan3Circles(sds_point c1, 
						   double r1, 
						   sds_point c2, 
						   double r2, 
						   sds_point c3, 
						   double r3,
						   sds_point lp1,
						   sds_point lp2,
						   sds_point lp3,
						   sds_point newcen, 
						   double *newrad);


// Construct a circle tangent to two lines and one circle
// The input lines and the input circle must all be in the X-Y plane
// The three pick points are used to determine which one of the potentially
// eight tangent circles will be selected
extern bool CirTan2Lines1Cir(
					  sds_point L1A,   // I:  start of first line
					  sds_point L1B,   // I:    end of first line
					  sds_point L2A,   // I:  start of second line
					  sds_point L2B,   // I:    end of second line
					  sds_point cen,   // I: center of input circle
					  double rad,      // I: radius of input circle
					  sds_point picL1, // I: pick point for first line
					  sds_point picL2, // I: pick point for second line
					  sds_point picC,  // I: pick point for circle
					  sds_point cenT,  // O: center of output tangent circle
					  double * radT);  // O: radius of output tangent circle

// Construct a circle tangent to two circles and one line.
extern bool CirTan2Circles1Line(
						 sds_point cen1,  // I:  center of 1st input circle
						 double r1,       // I:  radius of 1st input circle
                         sds_point cen2,  // I:  center of 2nd input circle
						 double r2,       // I:  radius of 2nd input circle
						 sds_point LA,    // I:  start of input line
						 sds_point LB,    // I:  end of input line
						 sds_point pikC1, // I:  pick point for 1st input circle
						 sds_point pikC2, // I:  pick point for 2nd input circle
						 sds_point pL,    // I:  pick point for input line
						 sds_point cenT,  // O:  center of output tangent circle
                         double * radT    // O:  radius of output tangent circle
						 );

extern bool CirTan3Lines(sds_point p1,     // I:  start of 1st line
						 sds_point p2,     // I:  end of 1st line				  
						 sds_point lp1,    // I:  pick pt of 1st line
						 sds_point p3,     // I:  start of 2nd line
						 sds_point p4,     // I:  end of 2nd line
						 sds_point lp2,    // I:  pick pt of 2nd line
						 sds_point p5,     // I:  start of 3rd line
						 sds_point p6,     // I:  end of 3rd line
						 sds_point lp3,    // I:  pick pt of 3rd line
						 sds_point newcen, // O: center of tangent circle
						 double * newrad); // O: radius of tangent circle

bool CircTan2Lines(sds_point _pnt,
				   line_or_circle lorc1, 
				   line_or_circle lorc2,
				   sds_point newcen,
				   double * newrad);

int CircTanLineCircle(
					sds_point pt, // assumed to be a line
					line_or_circle lorc1, // assumed to be a line
					line_or_circle lorc2, // assumed to be a line
					sds_point newcen,
					double* newrad);

int CircTan2Circles(sds_point pnt,
					line_or_circle lorc1, 
					line_or_circle lorc2,
					sds_point newcen,
					double* newrad);

extern int CircTanParaLLL(line_or_circle lorc1,  // I: a line parallel to lorc2 
				          line_or_circle lorc2,  // I: a line parallel to lorc1
				          line_or_circle lorc3,  // I: a line
				          sds_point newcen,
				          double * newrad);

#endif // TANCIRCS_H

