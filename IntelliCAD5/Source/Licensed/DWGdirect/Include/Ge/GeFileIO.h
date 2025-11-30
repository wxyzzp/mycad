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



#ifndef OD_GE_FILE_IO_H
#define OD_GE_FILE_IO_H


class OdGeEntity2d;
class OdGeEntity3d;
class OdGePoint2d;
class OdGeVector2d;
class OdGeMatrix2d;
class OdGeScale2d;
class OdGePoint3d;
class OdGeVector3d;
class OdGeMatrix3d;
class OdGeScale3d;
class OdGeTol;
class OdGeInterval;
class OdGeKnotVector;
class OdGeCurveBoundary;
class OdGePosition2d;
class OdGePointOnCurve2d;
class OdGeLine2d;
class OdGeLineSeg2d;
class OdGeRay2d;
class OdGeCircArc2d;
class OdGeEllipArc2d;
class OdGeExternalCurve2d;
class OdGeCubicSplineCurve2d;
class OdGeCompositeCurve2d;
class OdGeOffsetCurve2d;
class OdGeNurbCurve2d;
class OdGePolyline2d;
class OdGePosition3d;
class OdGePointOnCurve3d;
class OdGePointOnSurface;
class OdGeLine3d;
class OdGeRay3d;
class OdGeLineSeg3d;
class OdGePlane;
class OdGeBoundedPlane;
class OdGeBoundBlock2d;
class OdGeBoundBlock3d;
class OdGeCircArc3d;
class OdGeEllipArc3d;
class OdGeCubicSplineCurve3d;
class OdGeCompositeCurve3d;
class OdGeOffsetCurve3d;
class OdGeNurbCurve3d;
class OdGePolyline3d;
class OdGeAugPolyline3d;
class OdGeExternalCurve3d;
class OdGeSurface;
class OdGeCone;
class OdGeCylinder;
class OdGeTorus;
class OdGeExternalSurface;
class OdGeOffsetSurface;
class OdGeNurbSurface;
class OdGeExternalBoundedSurface;
class OdGeSphere;
class OdGeCurveCurveInt2d;
class OdGeCurveCurveInt3d;

class OdGeEllipCone;			// AE 03.09.2003 
class OdGeEllipCylinder;	// AE 09.09.2003 

class OdGeFiler;
class OdGeLibVersion;

#include "GeLibVersion.h"
#include "Ge.h"
#include "GeIntArray.h"
#include "GeDoubleArray.h"
#include "GePoint2dArray.h"
#include "GeVector2dArray.h"
#include "GeVector3dArray.h"

/** Description:

*/
class OdGeFileIO
{
public:
	
	static 
		void outFields(OdGeFiler*, const OdGePoint2d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void outFields(OdGeFiler*, const OdGeVector2d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void outFields(OdGeFiler*, const OdGeMatrix2d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void outFields(OdGeFiler*, const OdGeScale2d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void outFields(OdGeFiler*, const OdGePoint2dArray&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void outFields(OdGeFiler*, const OdGeVector2dArray&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void outFields(OdGeFiler*, const OdGePoint3d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void outFields(OdGeFiler*, const OdGeVector3d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void outFields(OdGeFiler*, const OdGeMatrix3d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void outFields(OdGeFiler*, const OdGeScale3d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void outFields(OdGeFiler*, const OdGePoint3dArray&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void outFields(OdGeFiler*, const OdGeVector3dArray&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void outFields(OdGeFiler*, const OdGeTol&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void outFields(OdGeFiler*, const OdGeInterval&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void outFields(OdGeFiler*, const OdGeKnotVector&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void outFields(OdGeFiler*, const OdGeDoubleArray&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static
		void outFields(OdGeFiler* pFiler, const OdGeIntArray& ent,
		const OdGeLibVersion& version);
	static 
		void outFields(OdGeFiler*, const OdGeCurveBoundary&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void outFields(OdGeFiler*, const OdGePosition2d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void outFields(OdGeFiler*, const OdGePointOnCurve2d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void outFields(OdGeFiler*, const OdGeLine2d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void outFields(OdGeFiler*, const OdGeLineSeg2d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void outFields(OdGeFiler*, const OdGeRay2d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void outFields(OdGeFiler*, const OdGeCircArc2d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void outFields(OdGeFiler*, const OdGeEllipArc2d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void outFields(OdGeFiler*, const OdGeExternalCurve2d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void outFields(OdGeFiler*, const OdGeCubicSplineCurve2d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void outFields(OdGeFiler*, const OdGeCompositeCurve2d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void outFields(OdGeFiler*, const OdGeOffsetCurve2d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void outFields(OdGeFiler*, const OdGeNurbCurve2d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void outFields(OdGeFiler*, const OdGePolyline2d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void outFields(OdGeFiler*, const OdGePosition3d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void outFields(OdGeFiler*, const OdGePointOnCurve3d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void outFields(OdGeFiler*, const OdGePointOnSurface&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void outFields(OdGeFiler*, const OdGeLine3d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void outFields(OdGeFiler*, const OdGeRay3d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void outFields(OdGeFiler*, const OdGeLineSeg3d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void outFields(OdGeFiler*, const OdGePlane&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void outFields(OdGeFiler*, const OdGeBoundedPlane&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void outFields(OdGeFiler*, const OdGeCircArc3d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void outFields(OdGeFiler*, const OdGeEllipArc3d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void outFields(OdGeFiler*, const OdGeCubicSplineCurve3d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void outFields(OdGeFiler*, const OdGeCompositeCurve3d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void outFields(OdGeFiler*, const OdGeOffsetCurve3d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void outFields(OdGeFiler*, const OdGeNurbCurve3d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void outFields(OdGeFiler*, const OdGePolyline3d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void outFields(OdGeFiler*, const OdGeAugPolyline3d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void outFields(OdGeFiler*, const OdGeExternalCurve3d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void outFields(OdGeFiler*, const OdGeCone&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void outFields(OdGeFiler*, const OdGeEllipCone&,	
		const OdGeLibVersion& = OdGe::gLibVersion);	// AE 03.09.2003 
	static 
		void outFields(OdGeFiler*, const OdGeCylinder&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void outFields(OdGeFiler*, const OdGeEllipCylinder&,	// AE 09.09.2003 
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void outFields(OdGeFiler*, const OdGeTorus&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void outFields(OdGeFiler*, const OdGeExternalSurface&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void outFields(OdGeFiler*, const OdGeOffsetSurface&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void outFields(OdGeFiler*, const OdGeNurbSurface&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void outFields(OdGeFiler*, const OdGeExternalBoundedSurface&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void outFields(OdGeFiler*, const OdGeSphere&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	
	static 
		void outFields(OdGeFiler*, const OdGeBoundBlock2d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	
	static 
		void outFields(OdGeFiler*, const OdGeBoundBlock3d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	
	static 
		void outFields(OdGeFiler*, const OdGeCurveCurveInt2d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	
	static 
		void outFields(OdGeFiler*, const OdGeCurveCurveInt3d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	
	static 
		void outFields(OdGeFiler*, const OdGeLibVersion&);
	
	static 
		void inFields(OdGeFiler*, OdGePoint2d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void inFields(OdGeFiler*, OdGeVector2d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void inFields(OdGeFiler*, OdGeMatrix2d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void inFields(OdGeFiler*, OdGeScale2d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void inFields(OdGeFiler*, OdGePoint2dArray&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void inFields(OdGeFiler*, OdGeVector2dArray&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void inFields(OdGeFiler*, OdGePoint3d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void inFields(OdGeFiler*, OdGeVector3d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void inFields(OdGeFiler*, OdGeMatrix3d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void inFields(OdGeFiler*, OdGeScale3d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void inFields(OdGeFiler*, OdGePoint3dArray&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void inFields(OdGeFiler*, OdGeVector3dArray&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void inFields(OdGeFiler*, OdGeTol&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void inFields(OdGeFiler*, OdGeInterval&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void inFields(OdGeFiler*, OdGeKnotVector&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void inFields(OdGeFiler*, OdGeDoubleArray&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static
		void inFields(OdGeFiler* pFiler, OdGeIntArray& ent,
		const OdGeLibVersion& version);
	static 
		void inFields(OdGeFiler*, OdGeCurveBoundary&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void inFields(OdGeFiler*, OdGePosition2d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void inFields(OdGeFiler*, OdGePointOnCurve2d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void inFields(OdGeFiler*, OdGeLine2d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void inFields(OdGeFiler*, OdGeLineSeg2d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void inFields(OdGeFiler*, OdGeRay2d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void inFields(OdGeFiler*, OdGeCircArc2d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void inFields(OdGeFiler*, OdGeEllipArc2d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void inFields(OdGeFiler*, OdGeExternalCurve2d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void inFields(OdGeFiler*, OdGeCubicSplineCurve2d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void inFields(OdGeFiler*, OdGeCompositeCurve2d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void inFields(OdGeFiler*, OdGeOffsetCurve2d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void inFields(OdGeFiler*, OdGeNurbCurve2d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void inFields(OdGeFiler*, OdGePolyline2d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void inFields(OdGeFiler*, OdGePosition3d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void inFields(OdGeFiler*, OdGePointOnCurve3d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void inFields(OdGeFiler*, OdGePointOnSurface&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void inFields(OdGeFiler*, OdGeLine3d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void inFields(OdGeFiler*, OdGeRay3d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void inFields(OdGeFiler*, OdGeLineSeg3d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void inFields(OdGeFiler*, OdGePlane&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void inFields(OdGeFiler*, OdGeBoundedPlane&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void inFields(OdGeFiler*, OdGeCircArc3d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void inFields(OdGeFiler*, OdGeEllipArc3d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void inFields(OdGeFiler*, OdGeCubicSplineCurve3d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void inFields(OdGeFiler*, OdGeCompositeCurve3d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void inFields(OdGeFiler*, OdGeOffsetCurve3d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void inFields(OdGeFiler*, OdGeNurbCurve3d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void inFields(OdGeFiler*, OdGePolyline3d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void inFields(OdGeFiler*, OdGeAugPolyline3d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void inFields(OdGeFiler*, OdGeExternalCurve3d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void inFields(OdGeFiler*, OdGeCone&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void inFields(OdGeFiler*, OdGeEllipCone&,
		const OdGeLibVersion& = OdGe::gLibVersion);	// AE 03.09.2003 
	static 
		void inFields(OdGeFiler*, OdGeCylinder&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void inFields(OdGeFiler*, OdGeEllipCylinder&,	// AE 09.09.2003 
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void inFields(OdGeFiler*, OdGeTorus&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void inFields(OdGeFiler*, OdGeExternalSurface&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void inFields(OdGeFiler*, OdGeOffsetSurface&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void inFields(OdGeFiler*, OdGeNurbSurface&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void inFields(OdGeFiler*, OdGeExternalBoundedSurface&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	static 
		void inFields(OdGeFiler*, OdGeSphere&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	
	static 
		void inFields(OdGeFiler*, OdGeBoundBlock2d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	
	static 
		void inFields(OdGeFiler*, OdGeBoundBlock3d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	
	static 
		void inFields(OdGeFiler*, OdGeCurveCurveInt2d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	
	static 
		void inFields(OdGeFiler*, OdGeCurveCurveInt3d&,
		const OdGeLibVersion& = OdGe::gLibVersion);
	
	static 
		void inFields(OdGeFiler*, OdGeLibVersion&);
	
private:
	static
    void writebool(OdGeFiler*, bool,
		const OdGeLibVersion&);
	static
    void readbool(OdGeFiler*, bool*,
		const OdGeLibVersion&);
	void writeBool(OdGeFiler*, bool,
		const OdGeLibVersion&);
	static
    void readBool(OdGeFiler*, bool*,
		const OdGeLibVersion&);
	static
    void writeLong(OdGeFiler*, long,
		const OdGeLibVersion&);
	static
    void readLong(OdGeFiler*, long*,
		const OdGeLibVersion&);
	static
    void writeDouble(OdGeFiler*, double,
		const OdGeLibVersion&);
	static
    void readDouble(OdGeFiler*, double*,
		const OdGeLibVersion&);
	static
    void writePoint2d(OdGeFiler*, const OdGePoint2d&,
		const OdGeLibVersion&);
	static
    void readPoint2d(OdGeFiler*, OdGePoint2d*,
		const OdGeLibVersion&);
	static
    void writeVector2d(OdGeFiler*, const OdGeVector2d&,
		const OdGeLibVersion&);
	static
    void readVector2d(OdGeFiler*, OdGeVector2d*,
		const OdGeLibVersion&);
	static
    void writePoint3d(OdGeFiler*, const OdGePoint3d&,
		const OdGeLibVersion&);
	static
    void readPoint3d(OdGeFiler*, OdGePoint3d*,
		const OdGeLibVersion&);
	static
    void writeVector3d(OdGeFiler*, const OdGeVector3d&,
		const OdGeLibVersion&);
	static
    void readVector3d(OdGeFiler*, OdGeVector3d*,
		const OdGeLibVersion&);
	static
    void writeAcGeSurface(OdGeFiler*, const OdGeSurface&,
		const OdGeLibVersion& version);
	static
    void readAcGeSurface(OdGeFiler*, OdGeSurface&,
		const OdGeLibVersion& version);
	static
    void writeAcGeEntity2d(OdGeFiler* pFiler,
		const OdGeEntity2d& ent, const OdGeLibVersion& version);
  static
    void readAcGeEntity2d(OdGeFiler* pFiler, OdGeEntity2d*& ent,
		OdGe::EntityId id, const OdGeLibVersion& version);
	static
    void writeAcGeEntity3d(OdGeFiler* pFiler,
		const OdGeEntity3d& ent, const OdGeLibVersion& version);
	static
    void readAcGeEntity3d(OdGeFiler* pFiler, OdGeEntity3d*& ent,
		OdGe::EntityId id, const OdGeLibVersion& version);
	
  friend class OdGeFileIOX;
	
};

#endif // OD_GE_FILE_IO_H


