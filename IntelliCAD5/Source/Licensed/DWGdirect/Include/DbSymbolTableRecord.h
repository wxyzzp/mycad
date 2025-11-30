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



#ifndef _ODDBSYMBOLTABLERECORD_INCLUDED
#define _ODDBSYMBOLTABLERECORD_INCLUDED

#include "DD_PackPush.h"

#include "DbObject.h"

class OdString;

/** Description:
    Base class for all table items in an OdDbDatabase.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbSymbolTableRecord : public OdDbObject
{
public:
  ODDB_DECLARE_MEMBERS(OdDbSymbolTableRecord);

  /** Description:
      Constructor (no arguments).
  */
  OdDbSymbolTableRecord();

  /** Description:
      Returns the name of this object (DXF 2).
  */
  virtual OdString getName() const;

  /** Description:
      Sets the name for this object (DXF 2).

      Arguments:
      pName Object name.
  */
  virtual void setName(const OdString& pName);

  /** Description:
      Returns true if this object is dependent on (part of) an external reference (xref), 
      false otherwise (DXF 70, bit 0x10).
  */
  bool isDependent() const;

  /** Description:
      Returns true if this object is part of an external reference (xref) that has been 
      resolved, false otherwise (DXF 70, bit 0x20).
  */
  bool isResolved() const;

  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  void appendToOwner(OdDbIdPair& pair, OdDbObject* pOwner, OdDbIdMapping& idMap);

  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;

  virtual void dxfOutFields_R12(OdDbDxfFiler* pFiler) const;

  /** Description:
      Copies the contents of other into the messaged object, whenever feasible
  */
  virtual void copyFrom(const OdRxObject* p);
};

typedef OdSmartPtr<OdDbSymbolTableRecord> OdDbSymbolTableRecordPtr;

#include "DD_PackPop.h"

#endif // _ODDBSYMBOLTABLERECORD_INCLUDED


