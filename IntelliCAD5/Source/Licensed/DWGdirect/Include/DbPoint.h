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



#ifndef _OD_DB_POINT_
#define _OD_DB_POINT_

#include "DD_PackPush.h"

#include "DbEntity.h"

/** Description:
    Represents a point entity in an OdDbDatabase.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbPoint : public OdDbEntity
{
public:
  ODDB_DECLARE_MEMBERS(OdDbPoint);

  /** Description:
      Constructor (no arguments).
  */
  OdDbPoint();

  /** Description:
      Constructor.
  */
  OdDbPoint(const OdGePoint3d& position);

  /** Description:
      Returns the position of this point entity (DXF 10).
  */
  OdGePoint3d position() const;

  /** Description:
      Sets the position of this point entity (DXF 10).
  */
  void setPosition(const OdGePoint3d&);

  /** Description:
      Returns the thickness of this point (DXF 39).
  */
  double thickness() const;

  /** Description:
      Sets the thickness of this point (DXF 39).
  */
  void setThickness(double);

  /** Description:
      Returns the normal vector of this point (DXF 210).
  */
  OdGeVector3d normal() const;

  /** Description:
      Sets the normal vector of this point (DXF 210).
  */
  void setNormal(const OdGeVector3d&);

  /** Description:
      Returns the angle of the X Axis for the UCS that was active when this point was drawn,
      used when PDMODE is nonzero (DXF 50).
  */
  double ecsRotation() const;

  /** Description:
      Sets the angle of the X Axis for the UCS that was active when this point was drawn,
      used when PDMODE is nonzero (DXF 50).
  */
  void setEcsRotation(double);

  virtual bool isPlanar() const { return true; }

  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;

  virtual OdResult dxfInFields_R12(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields_R12(OdDbDxfFiler* pFiler) const;

  virtual OdResult transformBy(const OdGeMatrix3d& xform);

  virtual bool worldDraw(OdGiWorldDraw* pWd) const;

  virtual void viewportDraw(OdGiViewportDraw*) const;

  virtual void getClassID(void** pClsid) const;
};

typedef OdSmartPtr<OdDbPoint> OdDbPointPtr;

#include "DD_PackPop.h"

#endif


