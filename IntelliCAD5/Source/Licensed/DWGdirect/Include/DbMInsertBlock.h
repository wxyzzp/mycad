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



#ifndef _OD_DB_MINSERT_BLOCK_
#define _OD_DB_MINSERT_BLOCK_

#include "DD_PackPush.h"

#include "DbBlockReference.h"

class OdDbMInsertBlockImpl;

/** Description:
    Represents an MInsert or Multiple Insert entity in an OdDbDatabase.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbMInsertBlock : public OdDbBlockReference
{
public:
  ODDB_DECLARE_MEMBERS(OdDbMInsertBlock);

  /** Description:
      Constructor (no arguments).
  */
  OdDbMInsertBlock();
  
  /** Description:
      Returns the number of columns for this entity (DXF 70).
  */
  OdUInt16 columns() const;

  /** Description:
      Sets the number of columns for this entity (DXF 70).
  */
  void setColumns(OdUInt16);
  
  /** Description:
      Returns the number of rows for this entity (DXF 71).
  */
  OdUInt16 rows() const;

  /** Description:
      Sets the number of rows for this entity (DXF 71).
  */
  void  setRows(OdUInt16);
  
  /** Description:
      Returns the column spacing for this entity (DXF 44).
  */
  double columnSpacing() const;

  /** Description:
      Sets the column spacing for this entity (DXF 44).
  */
  void  setColumnSpacing(double);
  
  /** Description:
      Returns the row spacing for this entity (DXF 45).
  */
  double rowSpacing() const;

  /** Description:
      Sets the row spacing for this entity (DXF 45).
  */
  void  setRowSpacing(double);
  
  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;

  virtual void getClassID(void** pClsid) const;

  virtual OdResult transformBy(const OdGeMatrix3d& xform);


  OdResult getGeomExtents(OdGeExtents3d& extents) const;

  virtual bool worldDraw(OdGiWorldDraw* pWd) const;
};

typedef OdSmartPtr<OdDbMInsertBlock> OdDbMInsertBlockPtr;

#include "DD_PackPop.h"

#endif

