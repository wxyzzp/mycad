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



#ifndef _ODDBLINETYPETABLE_INCLUDED
#define _ODDBLINETYPETABLE_INCLUDED

#include "DD_PackPush.h"

#include "DbSymbolTable.h"

class OdDbLinetypeTableRecord;

/** Description:
    Iterator that provides access to the elements in an OdDbLinetypeTable.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbLinetypeTableIterator : public OdDbSymbolTableIterator
{
public:
  ODRX_DECLARE_MEMBERS(OdDbLinetypeTableIterator);

protected:
  /** Description:
      Constructor.
  */
  OdDbLinetypeTableIterator();

  /** Description:
      Constructor.
  */
  OdDbLinetypeTableIterator(OdDbSymbolTableIteratorImpl* pImpl);
};

typedef OdSmartPtr<OdDbLinetypeTableIterator> OdDbLinetypeTableIteratorPtr;


/** Description:
    Represents the linetype table in an OdDbDatabase.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbLinetypeTable : public OdDbSymbolTable
{
public:
  ODDB_DECLARE_MEMBERS(OdDbLinetypeTable);

  /** Description:
      Constructor (no arguments).
  */
  OdDbLinetypeTable();

  /** Description:
      Returns the Object ID of a table entry, specified by name.

      Arguments:
      entryName (I) Name of entry.
      getErasedRecord (I) If true, the specifed table entry will be 
      retrieved even if has been erased, otherwise erased entries will not be retrieved.

      Return Value:
      Object ID of the specified entry, or a null Object ID if the specified
      entry was not found, or if it was erased and getErasedRecord was false.
  */
  virtual OdDbObjectId getAt(const OdString& entryName, bool getErasedRecord = false) const;

  /** Description:
      Opens and returns a table entry specified by name.

      Arguments:
      entryName (I) Name of the entry to open.
      openMode (I) Mode in which to open the entry.
      openErasedRec (I) If true, the specifed table entry will be 
      opened even if has been erased, otherwise erased entries will not be opened.

      Return Value:
      If successful, a smart pointer to the specified entry is returned,
      otherwise a null smart pointer is returned.
  */
  OdDbSymbolTableRecordPtr getAt(const OdString& entryName, OdDb::OpenMode openMode, bool openErasedRec = false) const;

  /** Description:
      Returns true if this table contains an entry with the specified name, 
      false otherwise.
  */
  bool has(const OdString& name) const;

  /** Description:
      Returns true if this table contains an entry with the specified Object ID, 
      false otherwise.
  */
  bool has(const OdDbObjectId& id) const;

  /** Description:
      Returns an iterator that can be used to traverse then entries in this table.

      Arguments:
      atBeginning (I) If true the returned iterator will start at the 
      beginning of the block table and iterate forwards, otherwise it will start
      at the end of the table and iterate backwards.
      skipDeleted (I) Indicates whether or not the returned iterator should
      skip deleted records.
  */
  OdDbSymbolTableIteratorPtr newIterator(bool atBeginning = true, bool skipDeleted = true) const;

  /** Description:
      Adds the specified table record to this table.

      Arguments:
      pRecord (I) OdDbLinetypeTableRecord to add to this table.

      Return Value:
      Object ID of newly added record.
  */
  virtual OdDbObjectId add(OdDbSymbolTableRecord* pRecord);

  /** Description:
      Returns the Object ID of the "ByLayer" linetype.
  */
  const OdDbObjectId& getLinetypeByLayerId() const;

  /** Description:
      Returns the Object ID of the "ByBlock" linetype.
  */
  const OdDbObjectId& getLinetypeByBlockId() const;

  virtual void getClassID(void** pClsid) const;

  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual void dxfOut(OdDbDxfFiler* pFiler) const;

  /*
    virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);
    virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;
  */
};

typedef OdSmartPtr<OdDbLinetypeTable> OdDbLinetypeTablePtr;

#include "DD_PackPop.h"

#endif // _ODDBLINETYPETABLE_INCLUDED


