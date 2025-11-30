/*	Splines.H - Defines the IntelliCAD interface for spline rendering
 *	Copyright (C) 1994-97 Visio Corporation. All rights reserved.
 *
 */
#ifndef SPLINES_H
#define SPLINES_H

#include "gr.h"
int gr_splinegen1(

  /* Given: */           /* DXF code */

    int         deg,     /*    71    */
    int         flags,   /*    70    */

    int         nknots,  /*    72    */
    double		*knot,   /*    40    */
    double		ktol,    /*    42    */

    int         ncpts,   /*    73    */
    sds_point  *cpt,     /*    10    */
    double		*wt,     /*    41    */
    double	    ctol,    /*    43    */

    double		resocoeff,

  /* Results: */

    int        *nverts_p,
    sds_point **vertex_pp);

int gr_splinegen2(

  /* Given: */              /* DXF code */

    int         deg,        /*    71    */

    int         nfpts,      /*    74    */
    sds_point  *fpt,        /*    11    */
    double		ftol,       /*    44    */

    sds_point   btandir,    /*    12    */
    sds_point   etandir,    /*    13    */
	BOOL bPeriodic,			/* 70, Bit 2 */

    double		resocoeff,

  /* Results: */

    int        *nverts_p,
    sds_point **vertex_pp,

    int        *ncpts_p,     /*    73    */
    sds_point  **cpt_pp,     /*    10    */

    int        *nknots_p,    /*    72    */
    double	   **knot_pp);	 /*    40    */


GR_API int gr_interp_spline(
	long n,	  // Must be > 2   I: The number of interpolation points 
	sds_point *q,			// I: The interpolaion points		
	int nPerSpan,			// I: Number of segments per span, must be >0
	long *no_of_vertices,	// O: Number of generated vertices
	sds_point **vertices);	// O: The generated vertices, allocated here
	
GR_API int gr_raw_spline(
	long n,	  // Must be > 2   I: The number of control points 
	sds_point *q,			// I: The control points		
	int nPerSpan,			// I: Number of segments per span, must be >0
	long *no_of_vertices,	// O: Number of generated vertices
	sds_point **vertices);	// O: The generated vertices, allocated here

GR_API int gr_offset_ellipse(
	sds_resbuf * rbEllipse, // I: The linked list containing the ellipse data
	double rDistance,		// I: Offset distance, (<0 for "through-point")
	sds_point sdsPick,		// I: User's pick point, wcs
	int mode,				// I: 1 for "through point", 2 for "both sides"
	sds_resbuf ** rbList);	// O: The linked list containing the offset data

GR_API int gr_offset_spline(
	sds_resbuf * rbSpline,  // I: The linked list containing the spline data
	double rDistance,		// I: Offset distance, (<0 for "through-point")
	sds_point sdsPick,		// I: User's pick point, wcs
	int mode,				// I: 1 for "through point", 2 for "both sides"
	sds_resbuf ** rbList);	// O: The linked list containing the offset data

GR_API short gr_get_spline_circleflag(	// Return 2 if planar & closed, 1 otherwise
	sds_resbuf * elist);		// I: The spline data as a linked list

GR_API short gr_get_ellipse_circleflag( // Return 2 if planar & closed, 1 otherwise	
	sds_resbuf * elist);	// I: The ellipse data as a linked list

GR_API int gr_get_spline_edata(
	sds_resbuf * elist,	// I: The spline data as a linked list
	double * length,	// I/O: The spline's length, updated here
	double * area);		// I/O: The spline's area, updated if planar& closed 

GR_API int gr_get_ellipse_edata(
	sds_resbuf * elist,	// I: The ellipse data as a linked list
	double * length,	// I/O: The ellipse's length, updated here
	double * area);		// I/O: The ellipse's area, updated if planar& closed 

GR_API int gr_get_spline_tangents(

// In						  DXF code
    int         nfpts,      /*    74    */
    sds_point  *fpt,        /*    11    */
    double		ftol,       /*    44    */
	BOOL bPeriodic,			/* 70, Bit 2 */

// Out
    sds_point   btandir,    // Tangent vector at the start
    sds_point   etandir);   // Tangent vector at the end

#endif
