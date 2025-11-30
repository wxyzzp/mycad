/*	GeomPtrs.h - Forward declarations for the geometry library
 *	Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 */

#ifndef GEOMPTRS_H
#define GEOMPTRS_H


// Transformation
#if defined(__cplusplus)
	class CTransform;
	typedef CTransform * PTransform;
	typedef const CTransform * PCTransform;
#else
	typedef struct CTransform * PTransform;
#endif

// Transformation
#if defined(__cplusplus)
	class CAffine;
	typedef CAffine * PAffine;
#else
	typedef struct CAffine * PAffine;
#endif

#if defined(__cplusplus)
	class CCurve;
	typedef CCurve * PCurve;
	typedef const CCurve * PCCurve;
#else
	typedef struct CCurve * PCurve;
#endif

#if defined(__cplusplus)
	class C3Point;
	typedef C3Point * PRealPoint;
	typedef C3Point * P3Point;
	typedef const C3Point * PC3Point;
#else
	typedef struct C3Point * PRealPoint;
	typedef struct C3Point * P3Point;
	typedef const struct C3Point * PC3Point;
#endif

#if defined(__cplusplus)
	class CExtents;
	typedef CExtents * PExtents;
#else
	typedef struct CExtents * PExtents;
#endif

// C++ Cache
#if defined(__cplusplus)
	class CCache;
	typedef CCache * PCache;
#else
	typedef struct CCache * PCache;
#endif


#if defined(__cplusplus)
	class CGeoObject;
	typedef CGeoObject * PGeoObject;
#else
	typedef struct CGeoObject * PGeoObject;
#endif

#if defined(__cplusplus)
	class CGeoObjectArray;
	typedef CGeoObjectArray * PGeoObjectArray;
#else
	typedef struct CGeoObjectArray * PGeoObjectArray;
#endif

#if defined(__cplusplus)
	class CLine;
	typedef CLine * PLine;
	typedef const CLine * PCLine;
#else
	typedef struct CLine * PLine;
#endif

#if defined(__cplusplus)
	class CKnots;
	typedef CKnots * PKnots;
#else
	typedef struct CKnots * PKnots;
#endif

#if defined(__cplusplus)
	class CControlPoints;
	typedef CControlPoints * PControlPoints;
#else
	typedef struct CControlPoints * PControlPoints;
#endif

#if defined(__cplusplus)
	class CEllipArc;
	typedef CEllipArc * PEllipArc;
	typedef const CEllipArc * PCEllipArc;
#else
	typedef struct CEllipArc * PEllipArc;
#endif

#if defined(__cplusplus)
	class CPath;
	typedef CPath * PPath;
#else
	typedef struct CPath * PPath;
#endif

#if defined(__cplusplus)
	class CBoundary;
	typedef CBoundary * PBoundary;
#else
	typedef struct CBoundary * PBoundary;
#endif

#if defined(__cplusplus)
	class CPointArray;
	typedef CPointArray * PPointArray;
#else
	typedef struct CPointArray * PPointArray;
#endif

#if defined(__cplusplus)
	class CStorage;
	typedef CStorage * PStorage;
#else
	typedef struct CStorage * PStorage;
#endif

#if defined(__cplusplus)
	class CUser;
	typedef CUser * PUser;
#else
	typedef struct CUser * PUser;
#endif

#if defined(__cplusplus)
	class CMappedCurve;
	typedef CMappedCurve * PMappedCurve;
#else
	typedef struct CMappedCurve * PMappedCurve;
#endif

#if defined(__cplusplus)
	class CRealFunction;
	typedef CRealFunction * PRealFunction;
#else
	typedef struct CRealFunction * PRealFunction;
#endif

#if defined(__cplusplus)
	class CLinearFunction;
	typedef CLinearFunction * PLinearFunction;
#else
	typedef struct CLinearFunction * PLinearFunction;
#endif

#if defined(__cplusplus)
	class CRay;
	typedef CRay * PRay;
#else
	typedef struct CRay * PRay;
#endif

#if defined(__cplusplus)
	class CPolyLine;
	typedef CPolyLine * PPolyLine;
	typedef const CPolyLine * PCPolyLine;
#else
	typedef struct CPolyLine * PPolyLine;
#endif

#if defined(__cplusplus)
	class CRegion;
	typedef CRegion * PRegion;
#else
	typedef struct CRegion * PRegion;
#endif

class CUnboundedLine;
typedef CUnboundedLine * PUnboundedLine;
typedef const CUnboundedLine * PCUnboundedLine;

class CNormal;
typedef CNormal * PNormal;

class CNURBS;
typedef CNURBS * PNURBS;
typedef const CNURBS * PCNURBS;
#define PSpline PNURBS
#define PCSpline PCNURBS

class CGap;
typedef CGap * PGap;
typedef const CGap * PCGap;

#endif	/* GEOMPTRS_H */
