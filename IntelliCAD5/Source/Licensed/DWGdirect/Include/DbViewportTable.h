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



#ifndef _ODDBVIEWPORTTABLE_INCLUDED
#define _ODDBVIEWPORTTABLE_INCLUDED

#include "DD_PackPush.h"

#include "DbAbstractViewTable.h"

class OdDbViewportTableRecord;

/** Description:
    Represents the viewport table in an OdDbDatabase.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbViewportTable: public OdDbAbstractViewTable
{
public:
  ODDB_DECLARE_MEMBERS(OdDbViewportTable);

  /** Description:
      Constructor (no arguments).
  */
  OdDbViewportTable();

  void SetActiveViewport(OdDbObjectId id);
  OdDbObjectId getActiveViewportId() const;

  void DeleteConfiguration(const OdString& Name);

  /** Description:
      Adds the specified table record to this symbol table.

      Arguments:
      pRecord (I) Table entry to add to this table.

      Return Value:
      Object ID of newly added record.
  */
  OdDbObjectId add(OdDbSymbolTableRecord* pRecord);

  OdResult dxfIn(OdDbDxfFiler* pFiler);

  void getClassID(void** pClsid) const;
};

typedef OdSmartPtr<OdDbViewportTable> OdDbViewportTablePtr;

#include "DD_PackPop.h"

#endif // _ODDBVIEWPORTTABLE_INCLUDED


