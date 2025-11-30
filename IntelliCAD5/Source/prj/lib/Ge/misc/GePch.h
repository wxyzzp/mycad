// File: GePch.h
//
// GE_API, CGe
//

#pragma once



#ifdef GEDLL
 #define GE_API __declspec(dllexport)
#else 
 #define GE_API __declspec(dllimport)
#endif



class GE_API CGeLibVersion;


struct GE_API CGe
{
	static const CGeLibVersion	s_libVersion;

	enum
	{
		eGood,
		eBad
	};

	enum EEntityId
	{
		eEntity2d,
		eEntity3d,
		ePointEnt2d,
		ePointEnt3d,
		ePosition2d,
		ePosition3d,
		ePointOnCurve2d,
		ePointOnCurve3d,
		ePointOnSurface,
		eBoundedPlane,
		eCircArc2d,
		eCircArc3d,
		eConic2d,
		eConic3d,
		eCurve2d,
		eCurve3d,
		eEllipArc2d,
		eEllipArc3d,
		eLine2d,
		eLine3d,
		eLinearEnt2d,
		eLinearEnt3d,
		eLineSeg2d,
		eLineSeg3d,
		ePlanarEnt,
		ePlane,
		eRay2d,
		eRay3d,
		eSurface,
		eSphere,
		eCylinder,
		eTorus,
		eCone,
		eSplineEnt2d,
		ePolyline2d,
		eAugPolyline2d,
		eNurbCurve2d,
		eDSpline2d,
		eCubicSplineCurve2d,
		eSplineEnt3d,
		ePolyline3d,
		eAugPolyline3d,
		eNurbCurve3d,
		eDSpline3d,
		eCubicSplineCurve3d,
		eTrimmedCrv2d,
		eCompositeCrv2d,
		eCompositeCrv3d,
		eExternalSurface,
		eNurbSurface,
		eTrimmedSurface,
		eOffsetSurface,
		eEnvelope2d,
		eCurveBoundedSurface,
		eExternalCurve3d,
		eExternalCurve2d,
		eSurfaceCurve2dTo3d,
		eSurfaceCurve3dTo2d,
		eExternalBoundedSurface,
		eCurveCurveInt2d,
		eCurveCurveInt3d,
		eBoundBlock2d,
		eBoundBlock3d,
		eOffsetCurve2d,
		eOffsetCurve3d,
		ePolynomCurve3d,
		eBezierCurve3d,
		eObject,
		eFitData3d,
		eHatch,
		eTrimmedCurve2d,
		eTrimmedCurve3d,
		eCurveSampleData,
		eEllipCone,
		eEllipCylinder,
		eIntervalBoundBlock,
		eClipBoundary2d,
		eExternalObject,
		eCurveSurfaceInt,
		eSurfaceSurfaceInt
	};

	enum EExternalEntityKind
	{
		eAcisEntity = 0,
		eExternalEntityUndefined
	};

	enum EPointContainment
	{
		eInside,
		eOutside,
		eOnBoundary
	};

	enum ENurbSurfaceProperties
	{
		eOpen		= 0x01,
		eClosed		= 0x02,
		ePeriodic	= 0x04,
		eRational	= 0x08,
		eNoPoles	= 0x10,
		ePoleAtMin	= 0x20,
		ePoleAtMax	= 0x40,
		ePoleAtBoth	= 0x80
	};

	enum EOffsetCrvExtType
	{
		eFillet,
		eChamfer,
		eExtend
	};

	enum EGeXConfig
	{
		eNotDefined		= 1 << 0,
		eUnknown		= 1 << 1,
		eLeftRight		= 1 << 2,
		eRightLeft		= 1 << 3,
		eLeftLeft		= 1 << 4,
		eRightRight		= 1 << 5,
		ePointLeft		= 1 << 6,
		ePointRight		= 1 << 7,
		eLeftOverlap	= 1 << 8,
		eOverlapLeft	= 1 << 9,
		eRightOverlap	= 1 << 10,
		eOverlapRight	= 1 << 11,
		eOverlapStart	= 1 << 12,
		eOverlapEnd		= 1 << 13,
		eOverlapOverlap	= 1 << 14
	};

	enum EBooleanType
	{
		eUnion,
		eSubtract,
		eCommon
	};

	enum EClipError
	{
		eSuccess,
		eInvalidClipBoundary,
		eNotInitialized
	};

	enum EClipCondition
	{
		eInvalid,
		eAllSegmentsInside,
		eSegmentsIntersect,
		eAllSegmentsOutsideZeroWinds,
		eAllSegmentsOutsideOddWinds,
		eAllSegmentsOutsideEvenWinds
	};

	enum EErrorCondition
	{
		eOk,
		e0This,
		e0Arg1,
		e0Arg2,
		ePerpendicularArg1Arg2,
		eEqualArg1Arg2,
		eEqualArg1Arg3,
		eEqualArg2Arg3,
		eLinearlyDependentArg1Arg2Arg3,
		eArg1TooBig,
		eArg1OnThis,
		eArg1InsideThis
	};

	enum EcsiConfig
	{ 
		eXUnknown, 
		eXOut,
		eXIn,
		eXTanOut,
		eXTanIn,
		eXCoincident,
		eXCoincidentUnbounded
	};

	enum EssiType
	{
		eSSITransverse,
		eSSITangent,
		eSSIAntiTangent
	};

	enum EssiConfig
	{
		eSSIUnknown,
		eSSIOut,
		eSSIIn,
		eSSICoincident
	};

	enum EGeIntersectError
	{
		eXXOk,
		eXXIndexOutOfRange,
		eXXWrongDimensionAtIndex,
		eXXUnknown
	};
};


typedef CGe::EErrorCondition CGeError;
