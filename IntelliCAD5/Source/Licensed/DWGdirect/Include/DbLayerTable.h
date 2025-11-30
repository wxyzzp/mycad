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



#ifndef _ODDBLAYERTABLE_INCLUDED
#define _ODDBLAYERTABLE_INCLUDED

#include "DD_PackPush.h"

#include "DbSymbolTable.h"

class OdDbLayerTableRecord;
class OdDbLayerTableIterator;

/** Description:
    Represents a layer table in an OdDbDatabase.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbLayerTable: public OdDbSymbolTable
{
public:
  ODDB_DECLARE_MEMBERS(OdDbLayerTable);

  OdDbLayerTable();

  virtual void getClassID(void** pClsid) const;


  /** Description:
    Generates usage data for each OdDbLayerTableRecord.
    The usage data is cached in OdDbLayerTableRecords and can be retrieved
    using the OdDbLayerTableRecord::isInUse() method.
  */
  void generateUsageData();
};

typedef OdSmartPtr<OdDbLayerTable> OdDbLayerTablePtr;

#include "DD_PackPop.h"

#endif // _ODDBLAYERTABLE_INCLUDED


