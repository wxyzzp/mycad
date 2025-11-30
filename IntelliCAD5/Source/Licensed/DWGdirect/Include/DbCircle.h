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



#ifndef _OD_DB_CIRCLE_
#define _OD_DB_CIRCLE_

#include "DD_PackPush.h"

#include "DbCurve.h"

class OdDbCircleImpl;

/** Description:
    Represents a circle in an OdDbDatabase.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbCircle : public OdDbCurve
{
public:
  ODDB_DECLARE_MEMBERS(OdDbCircle);

  /** Description:
      Constructor (no arguments).
  */
  OdDbCircle();

  /** Description:
      Returns the center of the circle in WCS (DXF 10).
  */
  OdGePoint3d center() const;

  /** Description:
      Sets the center of the circle in WCS (DXF 10).
  */
  void setCenter(const OdGePoint3d&);

  /** Description:
      Returns the radius of the circle (DXF 40).
  */
  double radius() const;

  /** Description:
      Sets the radius of the circle (DXF 40).
  */
  void setRadius(double);

  /** Description:
      Returns the thickness of the circle (depth along the extrusion direction)
      (DXF 39).
  */
  double thickness() const;

  /** Description:
      Sets the thickness of the circle (depth along the extrusion direction)
      (DXF 39).
  */
  void setThickness(double);

  /** Description:
      Returns the normal for this entity in WCS (DXF 210).
  */
  OdGeVector3d normal() const;

  /** Description:
      Sets the normal for this entity in WCS (DXF 210).
  */
  void setNormal(const OdGeVector3d&);

  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;

  virtual OdResult dxfInFields_R12(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields_R12(OdDbDxfFiler* pFiler) const;

  virtual bool worldDraw(OdGiWorldDraw* pWd) const;

  virtual OdResult transformBy(const OdGeMatrix3d& xform);

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
      Returns the end point of this curve.

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
  virtual OdResult getParamAtPoint(const OdGePoint3d& pt, double& param) const;

  /** Description:
      Creates a copy (clone) of this entity, and applies the supplied transformation
      to the newly created copy.

      Arguments:
      xform (I) Transformation matrix to apply to the newly created copy.
      pCopy (O) Receives a pointer to the newly created copy.
  */
  virtual OdResult getTransformedCopy(const OdGeMatrix3d& xform, OdDbEntityPtr& pCopy) const;
};

typedef OdSmartPtr<OdDbCircle> OdDbCirclePtr;

#include "DD_PackPop.h"

#endif

