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



#ifndef _OD_DB_SOLID_
#define _OD_DB_SOLID_

#include "DD_PackPush.h"

#include "DbEntity.h"

/** Description:
    Represents a 2D solid entity in an OdDbDatabase, which is a   
    filled entity with at most 4 vertices.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbSolid : public OdDbEntity
{
public:
  ODDB_DECLARE_MEMBERS(OdDbSolid);

  /** Description:
      Constructor (no arguments).
  */
  OdDbSolid();

  /*  OdDbSolid(const OdGePoint3d& pt0,
        const OdGePoint3d& pt1,
        const OdGePoint3d& pt2,
        const OdGePoint3d& pt3);
      OdDbSolid(const OdGePoint3d& pt0,
        const OdGePoint3d& pt1,
        const OdGePoint3d& pt2);
  */

  /** Description:
      Returns the point at the specified index (DXF 10, 11, 12, or 13).

      Arguments:
      idx (I) Index of desired point (0, 1, 2, or 3).
      pntRes (O) Receives the desired point.
  */
  void getPointAt(int idx, OdGePoint3d& pntRes) const;

  
  /** Description:
      Sets the point at the specified index (DXF 10, 11, 12, or 13).

      Arguments:
      idx (I) Index of desired point (0, 1, 2, or 3).
      pntRes (I) The point to set.
  */
	void setPointAt(int idx, const OdGePoint3d& pntRes);
  
  /** Description:
      Returns the thickness, or depth along the normal vector, of this entity (DXF 39).
  */
  double thickness() const;

  /** Description:
      Sets the thickness, or depth along the normal vector, of this entity (DXF 39).
  */
  void setThickness(double);
  
  /** Description:
      Returns the normal vector for this entity (DXF 210).
  */
  OdGeVector3d normal() const;

  /** Description:
      Sets the normal vector for this entity (DXF 210).
  */
  void setNormal(const OdGeVector3d&);
  
  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;

  virtual OdResult dxfInFields_R12(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields_R12(OdDbDxfFiler* pFiler) const;

  virtual void getClassID(void** pClsid) const;

  virtual bool worldDraw(OdGiWorldDraw* pWd) const;

  virtual OdResult transformBy(const OdGeMatrix3d& xform);
};

typedef OdSmartPtr<OdDbSolid> OdDbSolidPtr;

#include "DD_PackPop.h"

#endif

