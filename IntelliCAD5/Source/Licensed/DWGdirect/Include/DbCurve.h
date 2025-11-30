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
// programs incorporating this software must include the following statement 
// with their copyright notices:
//
//      DWGdirect © 2002 by Open Design Alliance Inc. All rights reserved. 
//
// By use of this software, you acknowledge and accept the terms of this 
// agreement.
///////////////////////////////////////////////////////////////////////////////



#ifndef OD_DBCURVE_H
#define OD_DBCURVE_H

#include "DD_PackPush.h"

#include "DbEntity.h"

class OdDbSpline;
class OdDbCurveImpl;


/** Description:
    Base class for curve entities in an OdDbDatabase.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbCurve : public OdDbEntity
{
public:
  ODDB_DECLARE_MEMBERS(OdDbCurve);

  /** Description:
      Constructor (no arguments).
  */
  OdDbCurve();

  /** Description:
      Returns true if this entity is closed, false otherwise.
  */
  virtual bool isClosed() const;

  /** Description:
      Returns true if this curve is periodic, false otherwise.
  */
  virtual bool isPeriodic() const;

  /** Description:
      Returns the start parameter of this curve.

      Arguments:
      param (O) Receives the parameter value.

      Return Value:
      eOk if successful, otherwise an appropriate error code.
  */
  virtual OdResult getStartParam(double& param) const = 0;

  /** Description:
      Returns the end parameter of this curve.

      Arguments:
      param (O) Receives the parameter value.

      Return Value:
      eOk if successful, otherwise an appropriate error code.
  */
  virtual OdResult getEndParam (double& param) const = 0;

  /** Description:
      Returns the start point of this curve.

      Arguments:
      pt (O) Receives the point value.

      Return Value:
      eOk if successful, otherwise an appropriate error code.  
  */
  virtual OdResult getStartPoint(OdGePoint3d& pt) const = 0;

  /** Description:
      Returns the end point of this curve.

      Arguments:
      pt (O) Receives the point value.

      Return Value:
      eOk if successful, otherwise an appropriate error code.  
  */
  virtual OdResult getEndPoint(OdGePoint3d& pt) const = 0;


  virtual OdResult getPointAtParam(double, OdGePoint3d&) const = 0;

  virtual OdResult getParamAtPoint(const OdGePoint3d&, double&) const = 0;



#ifdef OD_USE_STUB_FNS
  virtual OdResult getDistAtParam (double param, double& dist) const = 0;

  virtual OdResult getParamAtDist (double dist, double& param) const;

  virtual OdResult getDistAtPoint (const OdGePoint3d&, double&)const;

  virtual OdResult getPointAtDist (double, OdGePoint3d&) const;

  virtual OdResult getFirstDeriv (double param,
    OdGeVector3d& firstDeriv) const;

  virtual OdResult getFirstDeriv (const OdGePoint3d&,
    OdGeVector3d& firstDeriv) const;

  virtual OdResult getSecondDeriv (double param,
    OdGeVector3d& secDeriv) const;

  virtual OdResult getSecondDeriv (const OdGePoint3d&,
    OdGeVector3d& secDeriv) const;

  virtual OdResult getClosestPointTo(const OdGePoint3d& givenPnt,
    OdGePoint3d& pointOnCurve, bool extend = false) const;

  virtual OdResult getClosestPointTo(const OdGePoint3d& givenPnt,
    const OdGeVector3d& direction,
    OdGePoint3d& pointOnCurve, bool extend = false) const;
  
  virtual OdResult getSpline (OdDbSpline** spline) const;

  virtual OdResult extend(double newParam);

  virtual OdResult extend(bool extendStart,
    const OdGePoint3d& toPoint);

  virtual OdResult getArea(double&) const;
  
#endif

  /*
  virtual OdResult getOrthoProjectedCurve(const OdGePlane&,
    OdDbCurve** projCrv) const;

  virtual OdResult getProjectedCurve(const OdGePlane&,
    const OdGeVector3d& projDir, OdDbCurve** projCrv) const;

  virtual OdResult getOffsetCurves(double offsetDist,  //Replace OdRxObjectPtrArray
    OdRxObjectPtrArray& offsetCurves) const;

  virtual OdResult getOffsetCurvesGivenPlaneNormal(
    const OdGeVector3d& normal, double offsetDist,
    OdRxObjectPtrArray& offsetCurves) const;          //Replace OdRxObjectPtrArray

  virtual OdResult getSplitCurves (const OdGeDoubleArray& params, Replace OdRxObjectPtrArray
    OdRxObjectPtrArray& curveSegments) const;

  virtual OdResult getSplitCurves (const OdGePoint3dArray& points, Replace OdRxObjectPtrArray
    OdRxObjectPtrArray& curveSegments) const;

  */
};

typedef OdSmartPtr<OdDbCurve> OdDbCurvePtr;

#include "DD_PackPop.h"

#endif


