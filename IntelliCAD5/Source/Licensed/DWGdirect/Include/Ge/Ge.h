///////////////////////////////////////////////////////////////////////////////
// Copyright © 2002, Open Design Alliance Inc. ("Open Design") 
// 
// This software is owned by Open Design, and may only be incorporated into 
// application programs owned by members of Open Design subject to a signed 
// Membership Agreement and Supplemental Software License Agreement with 
// Open Design. The structure and organization of this Software are the valuable 
// trade secrets of Open Design and its suppliers. The Software is also protected 
// by copyright law and international treaty provisions. You agree not to 
// modify, adapt, translate, reverse engineer, decompile, disassemble or 
// otherwise attempt to discover the source code of the Software. Application 
// programs incorporating this software must include the following statment 
// with their copyright notices:
//
//      DWGdirect © 2002 by Open Design Alliance Inc. All rights reserved. 
//
// By use of this software, you acknowledge and accept the terms of this 
// agreement.
///////////////////////////////////////////////////////////////////////////////



#ifndef OD_GEGBLGE_H
#define OD_GEGBLGE_H

#include "DD_PackPush.h"

#include "GeExport.h"
#include "GeLibVersion.h"

/** Description:
    The OdGe classes are used to carry out general 2D and 3D geometric operations. 
    
    {group:OdGe_Classes}
*/
struct OdGe
{
  static const OdGeLibVersion gLibVersion;
  enum { eGood, eBad };

  enum EntityId
  {
    kEntity2d,
    kEntity3d,
    kPointEnt2d,
    kPointEnt3d,
    kPosition2d,
    kPosition3d,
    kPointOnCurve2d,
    kPointOnCurve3d,
    kPointOnSurface,
    kBoundedPlane,
    kCircArc2d,
    kCircArc3d,
    kConic2d,
    kConic3d,
    kCurve2d,
    kCurve3d,
    kEllipArc2d,
    kEllipArc3d,
    kLine2d,
    kLine3d,
    kLinearEnt2d,
    kLinearEnt3d,
    kLineSeg2d,
    kLineSeg3d,
    kPlanarEnt,
    kPlane,
    kRay2d,
    kRay3d,
    kSurface,
    kSphere,
    kCylinder,
    kTorus,
    kCone,
    kSplineEnt2d,
    kPolyline2d,
    kAugPolyline2d,
    kNurbCurve2d,
    kDSpline2d,
    kCubicSplineCurve2d,
    kSplineEnt3d,
    kPolyline3d,
    kAugPolyline3d,
    kNurbCurve3d,
    kDSpline3d,
    kCubicSplineCurve3d,
    kTrimmedCrv2d,
    kCompositeCrv2d,
    kCompositeCrv3d,
    kExternalSurface,
    kNurbSurface,
    kTrimmedSurface,
    kOffsetSurface,
    kEnvelope2d,
    kCurveBoundedSurface,
    kExternalCurve3d,
    kExternalCurve2d,
    kSurfaceCurve2dTo3d,
    kSurfaceCurve3dTo2d,
    kExternalBoundedSurface,
    kCurveCurveInt2d,
    kCurveCurveInt3d,
    kBoundBlock2d,
    kBoundBlock3d,
    kOffsetCurve2d,
    kOffsetCurve3d,
    kPolynomCurve3d,
    kBezierCurve3d,
    kObject,
    kFitData3d,
    kHatch,
    kTrimmedCurve2d,
    kTrimmedCurve3d,
    kCurveSampleData,
    kEllipCone,
    kEllipCylinder,
    kIntervalBoundBlock,
    kClipBoundary2d,
    kExternalObject,
    kCurveSurfaceInt,
    kSurfaceSurfaceInt
  };

  enum ExternalEntityKind {kAcisEntity= 0, kExternalEntityUndefined};

  enum PointContainment {kInside, kOutside, kOnBoundary};

  enum NurbSurfaceProperties {kOpen=0x01, kClosed=0x02, kPeriodic=0x04,
    kRational=0x08, kNoPoles=0x10, kPoleAtMin=0x20,
    kPoleAtMax=0x40, kPoleAtBoth=0x80};

  enum OffsetCrvExtType {kFillet, kChamfer, kExtend};

  enum OdGeXConfig
  {
    kNotDefined = 1 << 0,
      kUnknown = 1 << 1,
      kLeftRight = 1 << 2,
      kRightLeft = 1 << 3,
      kLeftLeft = 1 << 4,
      kRightRight = 1 << 5,
      kPointLeft = 1 << 6,
      kPointRight = 1 << 7,
      kLeftOverlap = 1 << 8,
      kOverlapLeft = 1 << 9,
      kRightOverlap = 1 << 10,
      kOverlapRight = 1 << 11,
      kOverlapStart = 1 << 12,
      kOverlapEnd = 1 << 13,
      kOverlapOverlap = 1 << 14
  };

  enum BooleanType {kUnion, kSubtract, kCommon};

  enum ClipError
  {
    eOk,
      eInvalidClipBoundary,
      eNotInitialized
  };

  enum ClipCondition
  {
    kInvalid,
      kAllSegmentsInside,
      kSegmentsIntersect,
      kAllSegmentsOutsideZeroWinds,
      kAllSegmentsOutsideOddWinds,
      kAllSegmentsOutsideEvenWinds
  };

  enum ErrorCondition
  {
	kOk,
	k0This,
	k0Arg1,
	k0Arg2,
	kPerpendicularArg1Arg2,
	kEqualArg1Arg2,
	kEqualArg1Arg3,
	kEqualArg2Arg3,
	kLinearlyDependentArg1Arg2Arg3,
	kArg1TooBig,
	kArg1OnThis,
	kArg1InsideThis,
	kNonCoplanarGeometry,
	kDegenerateGeometry,
	kSingularPoint
  };

  enum csiConfig {
    kXUnknown,
      kXOut,             // Transverse.  Curve neighborhood is outside the surface.
      kXIn,               // Transverse.  Curve neighborhood is inside the surface.
      kXTanOut,           // Tangent.  Curve neighborhood is outside the surface.
      kXTanIn,            //  Tangent.  Curve neighborhood is inside the surface.
      kXCoincident,          // Non-zero length.  Point is on the intersection boundary.
      kXCoincidentUnbounded  // Non-zero length.  Point is an arbitrary point on an unbounded intersection.
  };

  enum ssiType {
    kSSITransverse,  // Non-tangent intersection.
      kSSITangent,    // Tangent intersection.
      // Surface normals identical at any point within the component.
      kSSIAntiTangent  // Tangent intersection.
      // Surface normals opposite at any point within the component.
  };

  enum ssiConfig {
    kSSIUnknown,
      kSSIOut,            // Neighborhood is outside the surface.
      kSSIIn,            // Neighborhood is inside the surface.
      kSSICoincident       // Non-zero area intersection.
  };

  enum OdGeIntersectError {
		kXXOk,
		kXXIndexOutOfRange,
		kXXWrongDimensionAtIndex,
		kXXUnknown
  };

};

typedef OdGe::ErrorCondition OdGeError;
typedef OdGe::OdGeIntersectError OdGeIntersectError;

#include "DD_PackPop.h"

#endif  // AC_GEGBLGE_H


