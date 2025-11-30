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



#ifndef _ODDBBLOCKTABLE_INCLUDED
#define _ODDBBLOCKTABLE_INCLUDED

#include "DD_PackPush.h"

#include "DbSymbolTable.h"

class OdDbBlockTableRecord;

/** Description:
    Class used to iterate through the records in the blocks table.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbBlockTableIterator : public OdDbSymbolTableIterator
{
public:
  
  ODRX_DECLARE_MEMBERS(OdDbBlockTableIterator);
  
protected:
  /** Description:
      Constructor (no arguments).
  */
  OdDbBlockTableIterator();
    
  /** Description:
      Constructor that takes OdDbSymbolTableIteratorImpl*.
  */
  OdDbBlockTableIterator(OdDbSymbolTableIteratorImpl* pImpl);
};

typedef OdSmartPtr<OdDbBlockTableIterator> OdDbBlockTableIteratorPtr;


/** Description:
    Represents the block definitions table in an OdDbDatabase.  

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbBlockTable : public OdDbSymbolTable
{
public:
  ODDB_DECLARE_MEMBERS(OdDbBlockTable);

  /** Description:
      Constructor (no argument).
  */
  OdDbBlockTable();

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
  virtual OdDbSymbolTableRecordPtr getAt(const OdString& entryName,
                                         OdDb::OpenMode openMode, 
                                         bool openErasedRec = false) const;

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
      false otherwise (case insensitive).
  */
  virtual bool has(const OdString& name) const;

  /** Description:
      Returns true if this table contains an entry with the specified Object ID, 
      false otherwise.
  */
  virtual bool has(const OdDbObjectId& id) const;

  /** Description:
      Returns an iterator that can be used to traverse the entries in this table.

      Arguments:
      atBeginning (I) If true the returned iterator will start at the 
      beginning of the block table and iterate forwards, otherwise it will start
      at the end of the table and iterate backwards.
      skipDeleted (I) Indicates whether or not the returned iterator should
      skip deleted records.

      Return Value:
      Smart pointer to the new iterator.
  */
  OdDbSymbolTableIteratorPtr newIterator(bool atBeginning = true, bool skipDeleted = true) const;

  /** Description:
      Adds the specified block record to this block table.  In order for this operation to 
      succeed, the block table must be open for write, and the block table record to be added must 
      be opened for write.  

      Arguments:
      pRecord (I) OdDbBlockTableRecord to add to this table.

      Return Value:
      Object ID of newly added record.
  */
  virtual OdDbObjectId add(OdDbSymbolTableRecord* pRecord);

  /** Description:
      Returns the Object ID of the Model Space block within this table.
  */
  const OdDbObjectId& getModelSpaceId() const;

  /** Description:
      Returns the Object ID of the Paper Space block within this table.
  */
  const OdDbObjectId& getPaperSpaceId() const;

  /** Description:
      Reads in the DWG data for this object.

      Arguments:
      pFiler Filer object from which data is read.
  */
  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  /** Description:
      Writes out the DWG data for this object.

      Arguments:
      pFiler Filer object to which data is written.
  */
  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual OdResult dxfIn(OdDbDxfFiler* pFiler);

  virtual void getClassID(void** pClsid) const;
};

typedef OdSmartPtr<OdDbBlockTable> OdDbBlockTablePtr;

#include "DD_PackPop.h"

#endif 


