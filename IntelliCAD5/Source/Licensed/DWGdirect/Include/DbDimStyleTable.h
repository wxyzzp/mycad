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



#ifndef _ODDBDIMSTYLETABLE_INCLUDED
#define _ODDBDIMSTYLETABLE_INCLUDED

#include "DD_PackPush.h"

#include "DbSymbolTable.h"

class OdDbDimStyleTableRecord;

/** Description:
    Represents the dimension style table in an OdDbDatabase.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbDimStyleTable: public OdDbSymbolTable
{
public:
  ODDB_DECLARE_MEMBERS(OdDbDimStyleTable);

  /** Description:
      Constructor (no arguments).
  */
  OdDbDimStyleTable();

  /** Description:
      Adds the specified dimstyle record to this table.  In order for this operation to
      succeed, the table must be open for write, and the table record to be added must
      be opened for write.

      Arguments:
      pRecord (I) OdDbSymbolTableRecord to add to this table.

      Return Value:
      Object ID of newly added record.
  */
  OdDbObjectId add(OdDbSymbolTableRecord* pRecord);

  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;

  virtual void getClassID(void** pClsid) const;
};

typedef OdSmartPtr<OdDbDimStyleTable> OdDbDimStyleTablePtr;

#include "DD_PackPop.h"

#endif // _ODDBDIMSTYLETABLE_INCLUDED


