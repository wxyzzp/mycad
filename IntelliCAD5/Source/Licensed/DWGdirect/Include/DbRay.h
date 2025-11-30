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



#ifndef OD_DBRAY_H
#define OD_DBRAY_H

#include "DD_PackPush.h"

#include "DbCurve.h"

class OdGePoint3d;
class OdGeVector3d;

/** Description:
    Represents a ray entity in an OdDbDatabase.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbRay : public OdDbCurve
{
public:
  ODDB_DECLARE_MEMBERS(OdDbRay);

  /** Description:
      Constructor (no arguments).
  */
  OdDbRay();
  
/*
  void getOffsetCurvesGivenPlaneNormal(
  const OdGeVector3d& normal, double offsetDist,
  OdRxObjectPtrArray& offsetCurves) const; //Replace OdRxObjectPtrArray
*/
  
  /** Description:
      Returns the start point of the ray (DXF 10).
  */
  OdGePoint3d basePoint() const;

  /** Description:
      Sets the start point of the ray (DXF 10).
  */
  void setBasePoint(const OdGePoint3d& pt); 
  
  /** Description:
      Returns the direction of the ray (DXF 11).
  */
  OdGeVector3d unitDir() const;

  /** Description:
      Sets the direction of the ray (DXF 11).
  */
  void setUnitDir(const OdGeVector3d& vec);

  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;
  
  virtual void getClassID(void** pClsid) const;

  bool worldDraw(OdGiWorldDraw* pWd) const;

  OdResult getGeomExtents(OdGeExtents3d& extents) const;

  virtual bool isPlanar() const;

  virtual OdResult getPlane(OdGePlane&, OdDb::Planarity&) const;


  /** Description:
      OdDbCurveMethods
  */
  virtual bool isClosed() const;

  /** Description:
      TBC.
  */
  virtual bool isPeriodic() const;

  /** Description:
      TBC.
  */
  virtual OdResult getStartParam(double&) const;

  /** Description:
      TBC.
  */
  virtual OdResult getEndParam (double&) const;

  /** Description:
      TBC.
  */
  virtual OdResult getStartPoint(OdGePoint3d&) const;

  /** Description:
      TBC.
  */
  virtual OdResult getEndPoint(OdGePoint3d&) const;

  virtual OdResult getPointAtParam(double, OdGePoint3d&) const;

  /** Description:
      Returns the parameter of specified point of this curve.

      Arguments:
      pt (I) Point of the curve.
      param (O) Computed parameter.

      Return Value:
      eOk if successful, otherwise an appropriate error code.
  */
  virtual OdResult getParamAtPoint(const OdGePoint3d& pt, double& param) const;

  virtual OdResult transformBy(const OdGeMatrix3d& xform);
};

typedef OdSmartPtr<OdDbRay> OdDbRayPtr;

#include "DD_PackPop.h"

#endif


