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



#ifndef _ODDBUCSTABLERECORD_INCLUDED
#define _ODDBUCSTABLERECORD_INCLUDED

#include "DD_PackPush.h"

#include "DbSymbolTableRecord.h"
#include "ViewportDefs.h"

/** Description:
    Represent a UCS in an OdDbDatabase.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbUCSTableRecord: public OdDbSymbolTableRecord
{
public:
  ODDB_DECLARE_MEMBERS(OdDbUCSTableRecord);

  /** Description:
      Constructor (no arguments).
  */
  OdDbUCSTableRecord();

  /** Description:
      Returns the origin of this UCS (DXF 10).
  */
  OdGePoint3d origin() const;

  /** Description:
      Sets the origin of this UCS (DXF 10).
  */
  void setOrigin(const OdGePoint3d& newOrigin);

  /** Description:
      Returns the X axis of this UCS (DXF 11).
  */
  OdGeVector3d xAxis() const;

  /** Description:
      Sets the X axis of this UCS (DXF 11).
  */
  void setXAxis(const OdGeVector3d& xAxis);

  /** Description:
      Returns the Y axis of this UCS (DXF 12).
  */
  OdGeVector3d yAxis() const;

  /** Description:
      Sets the Y axis of this UCS (DXF 12).
  */
  void setYAxis(const OdGeVector3d& yAxis);

  /** Description:
      Returns the base origin of this UCS (DXF 13).
  */
  OdGePoint3d ucsBaseOrigin(OdDb::OrthographicView view) const;

  /** Description:
      Sets the base origin and orthographic view type of this UCS (DXF 13 and 71, respectively).
  */
  void setUcsBaseOrigin(const OdGePoint3d& origin, OdDb::OrthographicView view);

  virtual void getClassID(void** pClsid) const;

  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;

  virtual OdResult dxfInFields_R12(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields_R12(OdDbDxfFiler* pFiler) const;
};
typedef OdSmartPtr<OdDbUCSTableRecord> OdDbUCSTableRecordPtr;

#include "DD_PackPop.h"

#endif // _ODDBUCSTABLERECORD_INCLUDED


