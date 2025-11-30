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



#ifndef _ODDBSYMBOLTABLE_INCLUDED
#define _ODDBSYMBOLTABLE_INCLUDED

#include "DD_PackPush.h"

#include "DbObject.h"

class OdDbSymbolTableRecord;
class OdDbSymbolTableIterator;

typedef OdSmartPtr<OdDbSymbolTableIterator> OdDbSymbolTableIteratorPtr;
typedef OdSmartPtr<OdDbSymbolTableRecord> OdDbSymbolTableRecordPtr;

/** Description:

    {group:Other_Classes}
*/
class TOOLKIT_EXPORT OdError_DuplicateRecordName : public OdError
{
public:
  OdError_DuplicateRecordName(OdDbObjectId existingRecId);
  OdDbObjectId existingRecordId() const;
};

/** Description:
    Parent class for all symbol tables (layer table, linetype table, etc.) 
    in an OdDbDatabase.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbSymbolTable : public OdDbObject
{
public:
  ODDB_DECLARE_MEMBERS(OdDbSymbolTable);

  /** Description:
      Constructor (no arguments).
  */
  OdDbSymbolTable();

  /** Description:
      Opens a table entry specified by name, in the specified mode.

      Arguments:
      entryName (I) Name of table entry to open.
      openMode (I) Mode in which to open the entry.
      openErasedRec (I) If true, the specifed table entry will be 
      opened even if has been erased, otherwise erased entries will not be opened.

      Return Value:
      If successful, a smart pointer to the specified entry is returned,
      otherwise a null smart pointer is returned.
  */
  virtual OdDbSymbolTableRecordPtr getAt(const OdString& entryName,
                    OdDb::OpenMode openMode, bool openErasedRec = false) const;

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
      Returns true if this table contains an entry with the specified name, 
      false otherwise.
  */
  virtual bool has(const OdString& name) const;

  /** Description:
      Returns true if this table contains an entry with the specified Object ID, 
      false otherwise.
  */
  virtual bool has(const OdDbObjectId& id) const;

  /** Description:
      Returns an iterator that can be used to traverse then entries in this table.

      Arguments:
      atBeginning (I) If true the returned iterator will start at the 
      beginning of the block table and iterate forwards, otherwise it will start
      at the end of the table and iterate backwards.
      skipDeleted (I) Indicates whether or not the returned iterator should
      skip deleted records.
  */
  virtual OdDbSymbolTableIteratorPtr newIterator( bool atBeginning = true, 
                         bool skipDeleted = true) const;

  /** Description:
      Adds the specified table record to this symbol table.

      Arguments:
      pRecord (I) Table entry to add to this table.

      Return Value:
      Object ID of newly added record.
  */
  virtual OdDbObjectId add(OdDbSymbolTableRecord* pRecord);

  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;

  virtual OdResult dxfIn(OdDbDxfFiler* pFiler);

  virtual void dxfOut(OdDbDxfFiler* pFiler) const;

  virtual OdResult dxfInFields_R12(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields_R12(OdDbDxfFiler* pFiler) const;
};

typedef OdSmartPtr<OdDbSymbolTable> OdDbSymbolTablePtr;

class OdDbSymbolTableIteratorImpl;

/** Description:
    Iterator that provides access to the table records owned by a symbol table.

    Remarks:
    An OdDbSymbolTableIterator can iterator either forwards or backwards through
    the entries in a table.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbSymbolTableIterator : public OdRxObject
{
public:
  ODRX_DECLARE_MEMBERS(OdDbSymbolTableIterator);

  /** Description:
      Resets this iterator to so that either the first element of the last element
      is current.

      Arguments:
      atBeginning (I) If true, make the first element current, otherwise
      make the last element current.  
      skipDeleted (I) Indicates whether or not deleted records should be skipped
      by this iterator.
  */
  virtual void start(bool atBeginning = true, bool skipDeleted = true);

  /** Description:
      Returns true if this entry has no current element (the iterator was advanced beyond
      either end of the table, or there are no entries in the table).
  */
  virtual bool done() const;

  /** Description:
      Returns the Object ID of the current table entry referenced by this iterator.
  */
  virtual OdDbObjectId getRecordId() const;

  /** Description:
      Opens the current table entry referenced by this iterator, in the specified mode.

      Arguments:
      mode (I) Mode in which to open the entry.
      openErasedRec (I) If true, the specifed table entry will be 
      opened even if has been erased, otherwise erased entries will not be opened.

      Return Value:
      If successful, a smart pointer to the specified entry is returned,
      otherwise a null smart pointer is returned.
  */
  virtual OdDbSymbolTableRecordPtr getRecord(OdDb::OpenMode mode = OdDb::kForRead,
    bool openErasedRec = false) const;

  /** Description:
      Advances the current element reference one element either forwards or backwards.

      Arguments:
      forward (I) If true, advance forwards, otherwise advance backwards.
      skipDeleted (I) Indicates whether or not deleted records should be skipped
      by this iterator.
  */
  virtual void step(bool forward = true, bool skipDeleted = true);

  /** Description:
      Makes the table entry with the passed in Object ID the current element of this iterator.
  */
  virtual void seek(const OdDbObjectId& id);

  /** Description:
      Makes the passed in table entry the current element of this iterator.
  */
  virtual void seek(const OdDbSymbolTableRecord* pRecord);

  /** Description:
      Destructor.
  */
  virtual ~OdDbSymbolTableIterator();
protected:
  friend class OdDbSymbolTable;

  /** Description:
      Constructor.
  */
  OdDbSymbolTableIterator();

  /** Description:
      Constructor.
  */
  OdDbSymbolTableIterator(OdDbSymbolTableIteratorImpl* pImpl);

  OdDbSymbolTableIteratorImpl* m_pImpl;
};

#include "DD_PackPop.h"

#endif // _ODDBSYMBOLTABLE_INCLUDED


