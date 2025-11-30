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



#ifndef _ODDBDICTIONARY_INCLUDED_
#define _ODDBDICTIONARY_INCLUDED_

#include "DD_PackPush.h"

#include "RxIterator.h"
#include "RxDefs.h"
#include "DbObject.h"

class OdDbDictionaryImpl;
class OdDbObjectId;
class OdString;

/** Description:
    Iterator class that provides access to the entries in a dictionary (OdDbDictionary).

    Remarks:
    An OdDbDictionaryIterator maintains a "current position" within the entries
    of the associated dictionary, and can provide access to the key value and object at the
    current position.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbDictionaryIterator : public OdRxIterator
{
public:
  ODRX_DECLARE_MEMBERS(OdDbDictionaryIterator);

  /** Description:
      Destructor.
  */
  virtual ~OdDbDictionaryIterator() {}

  /** Description:
      Returns the name or key of the dictionary item at the current position specified
      by this iterator.
  */
  virtual OdString name() const = 0;

  /** Description:
      Opens the dictionary item at the current position specified by this iterator.    

      Arguments:
      mode (I) Mode in which to open the object.

      Return Value:
      Smart pointer to the opened object.
  */
  virtual OdDbObjectPtr getObject(OdDb::OpenMode mode = OdDb::kForRead) = 0;

  /** Description:
      Returns the Object ID of the dictionary item at the current position specified by
      this iterator.
  */
  virtual OdDbObjectId objectId() const = 0;

  /** Description:
      Sets the current position of this iterator to the dictionary item containing the
      specified ObjectId.

      Arguments:
      objId (I) Object ID of item to which the current position will be set.

      Return Value:
      true if the current position was set to the specified item, false otherwise.
  */
  virtual bool setPosition(OdDbObjectId objId) = 0;

  /** Description:
      RxIterator overridden.
      Opens the dictionary item at the current position specified by this iterator in kForRead mode.

      Return Value:
      Smart pointer to the opened object.
  */
  virtual OdRxObjectPtr object() const;

protected:
  
  /** Description:
      Constructor (no arguments).
  */
  OdDbDictionaryIterator() {}
};

typedef OdSmartPtr<OdDbDictionaryIterator> OdDbDictionaryIteratorPtr;

/** Description:
    Represents a dictionary object in an OdDbDatabase.  A dictionary contains a set
    entries, where an entry consists of a string name and an Object ID.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbDictionary : public OdDbObject
{
  ODDB_DECLARE_MEMBERS(OdDbDictionary);

  /** Description:
      Constructor (no arguments).
  */
  OdDbDictionary();

  /** Description:
      Opens the dictionary entry associated with the specified entry name.

      Arguments:
      entryName (I) Name of entry to be opened.
      mode (I) Mode in which to open the object.

      Return Value:
      Smart pointer to the opened object, or a null smart pointer if the 
      requested entry could not be opened.
  */
  OdDbObjectPtr getAt(const OdString& entryName, OdDb::OpenMode mode) const;

  /** Description:
      Returns the Object ID of the dictionary entry specified by the passed in entry name.

      Arguments:
      entryName (I) Name of entry to be for which the Object ID is to be returned.
      pRes (O) Receives eOk if the Object ID was successfully returned, otherwise an
      appropriate error code.

      Return Value:
      Object ID of the specified object, or a null ID if the specified object
      could not be accessed.
  */
  OdDbObjectId getAt(const OdString& entryName, OdResult* pRes = NULL) const;

  /** Description:
      Returns the name of the dictionary entry corresponding to the specified Object ID.
  */
  OdString nameAt(const OdDbObjectId& objId) const;

  /** Description:
      Returns true if this dictionary has an entry with the specified name, false otherwise.
  */
  bool has(const OdString& entryName) const;

  /** Description:
      Returns true if this dictionary has an entry with the specified Object ID, false otherwise.
  */
  bool has(const OdDbObjectId& objId) const;

  /** Description:
      Returns the number of entries in this dictionary.
  */
  OdUInt32 numEntries() const;

  /** Description:
      Removes the entry with the specified name from this dictionary, and returns
      the Object ID of the removed entry.

      Arguments:
      key (I) Name of entry to remove.

      Return Value:
      the Object ID of the removed entry.
  */
  OdDbObjectId remove(const OdString& key);

  /** Description:
      Removes the entry with the specified Object ID from this dictionary.

      Arguments:
      objId (I) Object ID of entry to remove.
  */
  void remove(const OdDbObjectId& objId);

  /** Description:
      Changes the name of a specified entry.

      Arguments:
      oldName (I) Name of entry to change.
      newName (I) New name of the entry.

      Return Value:
      true if the entry name was successfully changed, false otherwise.
  */
  bool setName(const OdString& oldName, const OdString& newName);

  /** Description:
      Adds the specified key and object to this dictionary.  If the key already exists
      in this dictionary, the existing entry is erased, and a new entry is created.

      Arguments:
      srchKey (I) Name of entry to be added.
      newValue (I) Database object to be added as part of the new entry.

      Return Value:
      Object ID of the newly added object.
  */
  OdDbObjectId setAt(const OdString& srchKey, OdDbObject* newValue);

  /* Returns true if this dictionary is the hard owner of its elements, false otherwise
     (DXF 280).
  */
  bool isTreatElementsAsHard() const;

  /** Description:
      Sets the hard owner property for this dictionary (DXF 280).

      Arguments:
      doIt (I) true if this dictionary is the hard owner of its elements, false otherwise.
  */
  void setTreatElementsAsHard(bool doIt);

  /** Description:
      Returns an new interator that can be used to traverse the entries in this dictionary.

      Arguments:
      iterType (I) Type of iterator.

      Remarks:
      The iterator type can be one of the following:

        o OdRx::kDictCollated - Traverses the entries in the order they were added to the dictionary.
        o OdRx::kDictSorted - Traverses the entries in alphabetical order by key value.
  */
  OdDbDictionaryIteratorPtr newIterator(
    OdRx::DictIterType iterType = OdRx::kDictCollated) const;

  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;

  /** Description:
      Returns the duplicate record cloning flag for this dictionary (DXF 281).
  */
  virtual OdDb::DuplicateRecordCloning mergeStyle() const;

  /** Description:
      Sets the duplicate record cloning flag for this dictionary (DXF 281).
  */
  virtual void setMergeStyle(OdDb::DuplicateRecordCloning style);

  /** Description:
      Notification function called just before an object is deleted from memory (an object 
      is deleted when its reference count reaches 0).

      Arguments:
      pObject (I) Object that is to be deleted.
      The default implementation of this function does nothing.  This function can be
      overridden in derived classes.
  */
  virtual void goodbye(const OdDbObject* pObject);

  /** Description:
      Notification function called when an object has been erased or unerased.

      Arguments:
      pObject (I) Object that was erased/unerased.
      erasing (I) If true, the object is being erased, otherwise it is being unerased.
      The default implementation of this function does nothing.  This function can be
      overridden in derived classes.
  */
  virtual void erased(const OdDbObject* pObject, bool erasing = true);

  virtual void getClassID(void** pClsid) const;


  /*
     virtual void decomposeForSave(OdDb::DwgVersion ver,
                                   OdDbObject** replaceObj, 
                                   OdDbObjectId& replaceId, 
                                   bool& exchangeXData);
     virtual OdResult subErase(bool pErasing = true);
  */

};

typedef OdSmartPtr<OdDbDictionary> OdDbDictionaryPtr;

#include "DD_PackPop.h"

#endif


