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



#ifndef OD_DBXLINE_H
#define OD_DBXLINE_H

#include "DD_PackPush.h"

#include "DbCurve.h"

class OdGePoint3d;
class OdGeVector3d;

/** Description:
    Represents an XLine entity in an OdDbDatabase.

    {group:OdDb_Classes} 
*/
class TOOLKIT_EXPORT OdDbXline : public OdDbCurve
{
public:
  ODDB_DECLARE_MEMBERS(OdDbXline);

  OdDbXline();

	/** Returns the base point of this entity (DXF 10).
  */
  OdGePoint3d basePoint() const;

	/** Sets the base point of this entity (DXF 11).
  */
  void setBasePoint(const OdGePoint3d& pt);

	/** Returns the unit direction vector for this entity (DXF 11).
  */
  OdGeVector3d unitDir() const;

	/** Sets the unit direction vector for this entity (DXF 11).
  */
  void setUnitDir(const OdGeVector3d& vec);

  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;

  virtual void getClassID(void** pClsid) const;

  virtual bool worldDraw(OdGiWorldDraw* pWd) const;

  virtual OdResult getGeomExtents(OdGeExtents3d& extents) const;

  /* OdDbEntity methods */

  virtual OdResult transformBy(const OdGeMatrix3d& xform);

  virtual bool isPlanar() const;

  virtual OdResult getPlane(OdGePlane& plane, OdDb::Planarity& planarity) const;


  /* OdDbCurveMethods */

  /** Description:
      Returns false for this entity.
  */
  virtual bool isClosed() const;

  /** Description:
      Returns true if this curve is periodic, false otherwise.
  */
  virtual bool isPeriodic() const;

  virtual OdResult getStartParam(double&) const;
  virtual OdResult getEndParam (double&) const;
  virtual OdResult getStartPoint(OdGePoint3d&) const;
  virtual OdResult getEndPoint(OdGePoint3d&) const;

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

  /*
  void getOffsetCurvesGivenPlaneNormal(
    const OdGeVector3d& normal, double offsetDist,
    OdRxObjectPtrArray& offsetCurves) const; //Replace OdRxObjectPtrArray
  */

};
typedef OdSmartPtr<OdDbXline> OdDbXlinePtr;

#include "DD_PackPop.h"

#endif


