/*	Geometry.h - Geometry library header file
 *
 *	Copyright (C) 1998 Visio Corporation. All rights reserved.
 *
 *
 *	ABSTRACT
 *  Vocabulary for working with the geometry library
 */
#ifndef GEOMETRY_H
#define GEOMETRY_H

#if defined(GEODLL )
	#define	GEO_CLASS __declspec( dllexport)

    #ifndef	GEO_API
	    #define	GEO_API GEO_CLASS
    #endif

    #ifndef GEO_FRIEND
        #define GEO_FRIEND friend GEO_CLASS 
    #endif 
#else
	#if defined( GEOSTATIC )
		#define	GEO_CLASS
	#else
		#define	GEO_CLASS __declspec( dllimport)
	#endif
    #ifndef	GEO_API
	    #define	GEO_API extern 
    #endif

    #ifndef GEO_FRIEND
        #define GEO_FRIEND friend
    #endif 
#endif






enum BEZTYPE{TYPE_Bezier};


/* Alternative definitions of the the Visio terms for working with
 * the Geometry library outside Visio
 */
// Types, error messages, etc.
	#define CRealPoint C3Point
	#define	E_BadInput			2100
	#define	E_TooManyKnots		2101
	#define	E_BadSpline			2102
	#define	E_ZeroVector		2103
	#define	E_InputZero			2104
	#define	E_BadOffset			2105
	#ifndef INV_ID
	#ifndef INTELLICAD
		#define INV_ID			-1
	#endif // ifndef INTELLICAD
	#endif // ifndef INV_ID

#ifdef  INTELLICAD
	// Intellicad definitions
	#include <memory.h>
	#include <math.h>
	#include <windows.h>

	#undef X
	#undef Y
	#undef Z
	#undef T
	#undef P

	#define THREE_D

	// Debug stuff
	#define LOCAL static
	#define RETURN_RC(x,y) return(y)
	#define DB_MSGX(x,y)
	#define DB_SOURCE_NAME
	#define QUIT goto exit;
	#ifndef ASSERT
		#define ASSERT(x)
	#endif
	#define ASSERT_ELSE(x,y) if(0)

	// Types
	typedef int BOOL;
	typedef long RC;
	typedef class db_objhandle * GeoID;
	#ifndef INV_ID
		#define INV_ID NULL
	#endif

	typedef struct  tagWCPoint 
		{	
		double x;
		double y;
		} WCPoint;

	// Return codes
	#define SUCCESS				   0
	#define FAILURE				  ~0
	#define DM_MEM				   2
	#define E_CANCEL			  13
	#define E_NOMSG				  14
#else
	#include "VisioIncludes.h"
	#define GeoID int
#endif

#include <limits.h>
#include "GeomPtrs.h"
#include "Constnts.h"
#include "Macros.h"

#endif	// GEOMETRY_H


