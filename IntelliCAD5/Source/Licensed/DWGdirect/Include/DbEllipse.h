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



#ifndef GEOMENT_DBELIPSE_H
#define GEOMENT_DBELIPSE_H

#include "DD_PackPush.h"

#include "DbCurve.h"

/** Description:
    Represents an ellipse in an OdDbDatabase.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbEllipse : public OdDbCurve
{
public:
  ODDB_DECLARE_MEMBERS(OdDbEllipse);

  /** Description:
      Constructor (no arguments).
  */
  OdDbEllipse();

  /** Description:
      Returns the center point of this ellipse in WCS (DXF 10).
  */
  OdGePoint3d center() const;

  /* Sets the center point of this ellipse in WCS (DXF 10).
  */
  void setCenter(const OdGePoint3d& center);

  /** Description:
      Returns the normal for this entity in WCS (DXF 210).
  */
  OdGeVector3d normal() const;

  /** Description:
      Returns the major axis vector for this ellipse in WCS.
  */
  OdGeVector3d majorAxis() const;

  /** Description:
      Returns the minor axis vector for this ellipse in WCS.
  */
  OdGeVector3d minorAxis() const;

  /** Description:
      Returns the ratio of the major radius to the minor radius of this ellipse.
  */
  double radiusRatio() const;

  /** Description:
      Sets the ratio of the major radius to the minor radius of this ellipse.
  */
  void setRadiusRatio(double ratio);

  /* Returns the start angle for this ellipse in radians.
  */
  double startAngle() const;

  /* Sets the start angle for this ellipse in radians.
  */
  void setStartAngle(double startAngle);

  /** Description:
      Returns the end angle for this ellipse in radians.
  */
  double endAngle() const;

  /** Description:
      Sets the end angle for this ellipse in radians.
  */
  void setEndAngle(double endAngle);

  /** Description:
      Sets the start parameter for this ellipse (DXF 41).
  */
  void setStartParam(double startParam);

  /** Description:
      Sets the end parameter for this ellipse (DXF 42).
  */
  void setEndParam(double endParam);

  /** Description:
      Given an angle, returns the parameter value corresponding to the angle with
      respect to this ellipse.  Can be used to get back the DXF 41 & 42 values.
  */
  double paramAtAngle(double angle) const;

  /** Description:
      Returns the angle value corresponding to the specified parameter value.
  */
  double angleAtParam(double param) const;

  /** Description:
      Returns the data for this ellipse.

      Arguments:
      center (O) Receives the center.
      unitNormal (O) Receives the normal vector.
      majorAxis (O) Receives the major axis.
      radiusRatio (O) Receives the radius ratio.
      startAngle (O) Receives the start angle.
      endAngle (O) Receives the end angle.
  */
  void get(OdGePoint3d& center,
    OdGeVector3d& unitNormal,
    OdGeVector3d& majorAxis,
    double& radiusRatio,
    double& startAngle,
    double& endAngle) const;

  /** Description:
      Sets the data for this ellipse.

      Arguments:
      center (I) The center point.
      unitNormal (I) The normal vector.
      majorAxis (I) The major axis.
      radiusRatio (I) The radius ratio.
      startAngle (I) The start angle.
      endAngle (I) The end angle.
  */
  void set(const OdGePoint3d& center,
    const OdGeVector3d& unitNormal,
    const OdGeVector3d& majorAxis,
    double radiusRatio,
    double startAngle = 0.0,
    double endAngle = 6.28318530717958647692);

  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;

  virtual bool worldDraw(OdGiWorldDraw* pWd) const;

  virtual void getClassID(void** pClsid) const;

  virtual bool isPlanar() const;

  virtual OdResult getPlane(OdGePlane& plane, OdDb::Planarity& planarity) const;

  /** Description:
      Returns true if this curve is closed, false otherwise.
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
  virtual OdResult getStartParam(double& param) const;

  /** Description:
      Returns the end parameter of this curve.

      Arguments:
      param (O) Receives the parameter value.

      Return Value:
      eOk if successful, otherwise an appropriate error code.
  */
  virtual OdResult getEndParam (double& param) const;

  /** Description:
      Returns the start point of this curve.

      Arguments:
      pt (O) Receives the point value.

      Return Value:
      eOk if successful, otherwise an appropriate error code.  
  */
  virtual OdResult getStartPoint(OdGePoint3d& pt) const;

  /** Description:
      Returns the start point of this curve.

      Arguments:
      pt (O) Receives the point value.

      Return Value:
      eOk if successful, otherwise an appropriate error code.  
  */
  virtual OdResult getEndPoint(OdGePoint3d& pt) const;

  /** Description:
      Returns the point at the specified parameter value.

      Arguments:
      param (I) Parameter value.
      retPt (O) Receives the point at the specified parameter value.

      Return Value:
      eOk if successful, otherwise an appropriate error code.
  */
  virtual OdResult getPointAtParam(double param, OdGePoint3d& retPt) const;

  /** Description:
      Returns the parameter of specified point of this curve.

      Arguments:
      pt (I) Point of the curve.
      param (O) Computed parameter.

      Return Value:
      eOk if successful, otherwise an appropriate error code.
  */
  virtual OdResult getParamAtPoint(const OdGePoint3d& pt, double& param)const;

  virtual OdResult transformBy(const OdGeMatrix3d& xform);
};

typedef OdSmartPtr<OdDbEllipse> OdDbEllipsePtr;

#include "DD_PackPop.h"

#endif


