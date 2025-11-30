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



#ifndef _ODDBXOBJECT_INCLUDED_
#define _ODDBXOBJECT_INCLUDED_

#include "DD_PackPush.h"

#include "Gi/GiDrawable.h"
#include "DbObjectId.h"
#include "OdString.h"
#include "DbObjectReactor.h"
#include "IdArrays.h"
#include "ResBuf.h"
#include "DebugStuff.h"

class OdDbFiler;
class OdDbDwgFiler;
class OdDbDxfFiler;
class OdGiDrawableTraits;
class OdGiWorldDraw;
class OdGiViewportDraw;
class OdGsNode;
class OdDbObjectImpl;
class OdDbEntity;
class OdDbDatabase;
class OdDbIdMapping;
class OdDbAuditInfo;
class OdGeMatrix3d;
class OdDbObjStorage;
class OdString;
class OdGePoint3d;
class OdBinaryData;
class OdDbIdPair;
class OdDbField;
class OdDbDictionary;

/** Description:
    Declares the required functions for classes derived from OdDbObject. Classes derived 
    from OdDbObject should invoke this macro in their class definitions, passing the derived
    class name.
*/
#define ODDB_DECLARE_MEMBERS(ClassName) \
protected:\
  ClassName(OdDbObjectImpl* pImpl);\
public: \
  ODRX_DECLARE_MEMBERS(ClassName)

/** Description:
    Template class that provides the heap functionality needed
    to instantiate objects that are descended from OdDbObject.

    {group:OdRx_Classes} 
*/
template<class T>
class OdDbObjectCreator : public T
{
  /** Description:
      Allows to create OdDbObjectCreator<T> instances inside this class only.
  */
  ODRX_HEAP_OPERATORS();
public:
  OdDbObjectCreator& operator = (const OdDbObjectCreator& op)
  { return static_cast<OdDbObjectCreator&>(T::operator =(op)); }

  /** Description:
      Creates an instance of OdDbObjectCreator<T>
      and returns smart pointer to it.
  */
  static OdSmartPtr<T> createObject()
  {
    return OdSmartPtr<T>(static_cast<T*>(new OdDbObjectCreator<T>), kOdRxObjAttach);
  }
};

#define DBOBJECT_CONSTR(CLASS, pX) pX = OdDbObjectCreator<CLASS>::createObject()

/** Description:

    {group:Other_Classes}
*/
class TOOLKIT_EXPORT OdError_XdataSizeExceeded : public OdError
{
public:
  OdError_XdataSizeExceeded(OdDbObjectId id);
};
                            

namespace OdDb
{
  enum DuplicateRecordCloning
  {
    kDrcNotApplicable  = 0,
    kDrcIgnore         = 1,   // Keep existing
    kDrcReplace        = 2,   // Use clone
    kDrcXrefMangleName = 3,   // <xref>$0$<name>
    kDrcMangleName     = 4,   // $0$<name>
    kDrcUnmangleName   = 5,
    kDrcMax            = kDrcUnmangleName
  };

  enum Visibility
  {
    kInvisible         = 1,
    kVisible           = 0
  };
}

/** Description
    Base class for all objects contained in an OdDbDatabase.

    Remarks:
    Creating and Deleting Database Objects
    
    OdDbObject's are normally created by calling OdDbXXXXX::createObject().
    When an object is created, it will be in OdDb::kOpenForWrite mode.  The delete
    operator should never be called on database objects.  Instead, the erase function
    should be called, which marks the object as erased.
    
    Accessing Database Objects
    
    Database objects must be opened before they can be accessed.  Given a valid OdDbObjectId,
    a database object is opened by calling OdDbObjectId::safeOpenObject.  Database objects
    should be opened in the most restrictive mode possible, and should be released immediately
    when access is no longer needed.
    
    See Also:
      OdDbObjectId
      OdDbDatabase

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbObject : public OdGiDrawable
{
public:
  ODDB_DECLARE_MEMBERS(OdDbObject);

  /** Description:
      Constructor (no arguments).
  */
  OdDbObject();

  /** Description:
      Destructor.
  */
  ~OdDbObject();
  
  /** Description:
      Increments the reference count for this object.
  */
  void addRef();

  /** Description:
      Decrements the reference count for this object, deleting the object if the 
      reference count becomes zero.
  */
  void release();

  /** Description:
      Returns the reference count for this object.
  */
  long numRefs() const;
  
  /** Description:
      Returns the Object ID of this object (or a null ID if this object has not yet been added
      to a database).
  */
  OdDbObjectId objectId() const;
  
  /** Description:
      Returns the persistent handle for this database object.
  */
  OdDbHandle getDbHandle() const;
  
  /** Description:
      Returns the Object ID of this object's owner (or a null ID if this object has not yet been added
      to a database).
  */
  OdDbObjectId ownerId() const;
  
  /** Description:
      Sets the owner Object ID for this object. 
  */
  virtual void setOwnerId(OdDbObjectId objId);
    
  /** Description:
      Returns a pointer to the database that contains this object.
  */
  OdDbDatabase* database() const;
  
  /** Description:
      Create an extension dictionary (OdDbDictionary) for this object, if one does not 
      already exist. 
      
      Remarks:
      If this object's extension dictionary has been erased, this call
      will un-erase it.  An object owns its extension dictionary.
  */
  void createExtensionDictionary();
  
  /** Description:
      Returns the Object ID of this object's extension dictionary.  
      
      Remarks:
      Returns a null ID if this object does not have an extension dictionary, 
      or if its extension dictionary has been been erased.
  */
  OdDbObjectId extensionDictionary() const;
  
  /** Description:
      Erases this object's extension dictionary, if the extension dictionary exists
      and is empty, otherwise does nothing.
  */
  void releaseExtensionDictionary();  
  
  /** Description:
      Upgrades the open status of this object to OdDb::kForWrite if the object is currently
      open as OdDb::kForRead and there is only 1 reader, otherwise does nothing.
  */
  void upgradeOpen();
  
  void downgradeOpen();
    
  /** Description:
      This function is called by the DWGdirect framework immediately before an object is opened. 

      Remarks:
      Overriding this function in a child class allows a child instance to be notified each time an
      object is opened.
  */
  virtual void subOpen(OdDb::OpenMode mode);
    
  virtual void cancelled(const OdDbObject* dbObj);

  virtual void subCancel();    

  /** Description:
      Called as the first operation as this object is being closed, for
      database resident objects only.
  */
  virtual void subClose();
  
  /** Description:
      Sets the erased mode for this object. 
      
      Remarks:
      Erased objects are not deleted from the database,
      but they will not be saved out to a DXF file.

      Arguments:
      erasing (I) true if this object is to be marked as erased, false if it should
      be marked as unerased, or normal.
  */
  OdResult erase(bool erasing = true);
  
  /** Description:
      Override this method to monitor the erasure of this object.  
      
      Remarks:
      This method will be called when the erase() method is called for this object. 
      Returning anything except eOk prevents the object from being erased.
  */
  virtual OdResult subErase(bool erasing);
  
  /** Description:
      Replaces this object with the specified non-database resident object in this object's
      database. 
      
      Remarks:
      This object's Object ID, persistent handle, and reactor list are transferred
      to the new object. 

      Arguments:
      newObject (I) Object with which to replace this object in the database.
      keepXData (I) If true, extended data will be transferred from this object to the
             new object.
      keepExtDict (I) If true, this object's extension dictionary will be transferred to 
             the new object.
  */
  void handOverTo(OdDbObject* newObject, bool keepXData = true, bool keepExtDict = true);
  
  /** Description:
      Called as the first operation of the handOverTo function.  
      
      Remarks:
      This function allows derived classes to populate the new object.
  */
  virtual void subHandOverTo(OdDbObject* newObject);
  
  /** Description:
      Causes this object to swap its Object ID with the specified Object ID.

      Arguments:
      otherId (I) Object ID to be swapped with this object's Object ID.
      swapXdata (I) If true, then extended data is swapped.
      swapExtDict (I) If true, then extension dictionaries are swapped.
  */
  void swapIdWith(OdDbObjectId otherId, bool swapXdata = false, bool swapExtDict = false);
  
  /** Description:
      Called as the first operation of the swapIdWith function.  
      
      Remarks: 
      This function allows derived classes to implement custom behavior 
      during the swapIdWith operation.

      Arguments:
      otherId (I) Object ID to be swapped with this object's Object ID.
      swapXdata (I) If true, then extended data is swapped.
      swapExtDict (I) If true, then extension dictionaries are swapped.
  */
  virtual void subSwapIdWith(const OdDbObjectId& otherId, bool swapXdata = false, bool swapExtDict = false);
    
  /** Description:
      Perform an audit operation on this object.

      Arguments:
      pAuditInfo (I) Pointer to an OdDbAuditInfo object.
      When overriding this function for a custom class, call OdDbObject::audit(pAuditInfo) first 
      to validate the audit operation.
  */
  virtual void audit(OdDbAuditInfo* pAuditInfo);
  
  /** Description:
      Loads the DWG format data for this object, from the specified filer.
  */
  void dwgIn(OdDbDwgFiler* pFiler);
  
  /** Description:
      Saves the DWG format data for this object, to the specified filer.
  */
  void dwgOut(OdDbDwgFiler* pFiler) const;
  
  /** Description:
      Reads the data for this object in DXF format (including extended data)
      from the specified filer.
  */  
  virtual OdResult dxfIn(OdDbDxfFiler* pFiler);
  
  /** Description:
      Writes the data for this object in DXF format (including extended data)
      to the specified filer.
  */  
  virtual void dxfOut(OdDbDxfFiler* pFiler) const;
  
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
  
  /** Description:
      Reads in the DXF data for this object.

      Arguments:
      pFiler Filer object from which data is read.
  */
  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);
  
  /** Description:
      Writes out the DXF data for this object.

      Arguments:
      pFiler Filer object to which data is written.
  */
  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;
  
  /** Description:
      Reads fields of the object in the DXF R12 format from the specified filer.
  */
  virtual OdResult dxfInFields_R12(OdDbDxfFiler* pFiler);
  
  /** Description:
      Writes fields of the object in the DXF R12 format to the specified filer.
  */
  virtual void dxfOutFields_R12(OdDbDxfFiler* pFiler) const;
  
  virtual OdDb::DuplicateRecordCloning mergeStyle() const;
  
  /** Description:
      Retrieves the extended data for this object.

      Arguments:
      regappName (O) Name of registered application associated with 
             the extended data.  Only the extended data associated with this application
             will be accessible through the returned OdResBufPtr object.

      Return Value:
      A smart pointer to the extended data associated with the specified application,
      or a null smart pointer if this extended data does not exist.
  */
  virtual OdResBufPtr xData(OdString regappName = OdString()) const;
  
  /** Description:
      Sets the extended data for this object.
  */
  virtual void setXData(const OdResBuf* pRb);
    
  /** Description:
      Returns true if this object's erased status has been modified since this object was 
      last opened, false otherwise. 
  */
  bool isEraseStatusToggled() const;
  
  /** Description:
      Returns true if this object is marked as erased, false otherwise.
  */
  bool isErased() const;
  
  /** Description:
      Returns true if this object is currently open as OdDb::kForRead, false otherwise.
  */
  bool isReadEnabled() const;
  
  /** Description:
      Returns true if this object is currently open as OdDb::kForWrite, false otherwise.
  */
  bool isWriteEnabled() const;
  
  /** Description:
      Returns true if this object is currently open as OdDb::kForNotify, false otherwise.
  */
  bool isNotifyEnabled() const;
  
  /** Description:
      Returns true if a member function has been called on this object since it was last opened, 
      which calls assertWriteEnabled (meaning that it could modify this object).
  */
  bool isModified() const;

  /** Description:
      Sets the modified property for this object. 
  */
//  void setModified(bool bModified);
  
  /** Description:
      Returns true if setXData() has been called on this object since it was last opened.
  */
  bool isModifiedXData() const;
  
  bool isModifiedGraphics() const;
  
  /** Description:
      Returns true if this is a newly created object that has not yet been closed.
  */
  bool isNewObject() const;
  
  bool isNotifying() const;
  
  bool isUndoing() const;
  
  bool isCancelling() const;
  
  bool isReallyClosing() const;
  
  /** Description:
      Returns true if this object is a database resident object, false otherwise.
  */
  bool isDBRO() const;
    
  /** Description:
      Throws an OdError(eNotOpenForRead) exception if this object is not open for 
      read access, otherwise does nothing.
  */
  void assertReadEnabled() const;
  
  /** Description:
      Throws an OdError(eNotOpenForWrite) exception if this object is not open for 
      write access, otherwise does nothing.
  */
  void assertWriteEnabled(bool autoUndo = true, bool recordModified = true);
  
  /** Description:
      Throws an OdError(eNotOpenForNotify) exception if this object is not open for 
      notify access, otherwise does nothing.
  */
  void assertNotifyEnabled() const;
  
  /** Description:
      Disables or enables the undo recording for this object.

      Arguments:
      disable If true, disable undo recording, otherwise enables it.
  */
  void disableUndoRecording(bool disable);
  
  /** Description:
      Returns the undo filer associated with this object.
  */
  OdDbDwgFiler* undoFiler();
  
  virtual void applyPartialUndo(OdDbDwgFiler* undoFiler, OdRxClass* classObj);
  
  /** Description:
      Adds the specified transient reactor to this object's reactor list.
  */
  void addReactor(OdDbObjectReactor* newObj) const;
  
  /** Description:
      Removes the specified transient reactor from this object's reactor list.
  */
  void removeReactor(OdDbObjectReactor* newObj) const;
  
  /** Description:
      Adds the specified persistent reactor to this object's persistent reactor list.

      Remarks:
      An object must be open for write in order to add a persistent reactor.

      Arguments:
      objId (I) Object ID of persistent reactor to add.
  */
  virtual void addPersistentReactor(const OdDbObjectId& objId);
  
  /** Description:
      Removes the specified persistent reactor from this object's persistent reactor list.

      Remarks:
      An object must be open for write in order to remove a persistent reactor.
  */
  virtual void removePersistentReactor(const OdDbObjectId& objId);
  
  /** Description:
      Retrieves this object's persistent reactors.

      Arguments:
      ids (O) Receives the persistent reactors.
  */
  void getPersistentReactors(OdDbObjectIdArray& ids);

  /** Description:
      Retrieves this object's transient reactors.

      Arguments:
      reacts (O) Receives the transient reactors.
  */
  void getTransientReactors(OdDbObjectReactorArray& reacts);
  
  virtual void recvPropagateModify(const OdDbObject* subObj);
  
  virtual void xmitPropagateModify() const;
  
  /** Description:
      Performs a deep clone of this object.
      Note: This function should not be called from client code
      (use OdDbDatabase::deepCloneObjects() instead.
      The purpose of this function is to have ability to override it in custom classes.

      Arguments:
      idMap (I/O) Current Object ID map.

      Return Value:
      A smart pointer to the newly created clone, or a null smart pointer if the cloning
      operation failed.
  */
  virtual OdDbObjectPtr deepClone(OdDbIdMapping& idMap) const;
  
  /** Description:
      Clones this object. Only hard pointer references are following during this cloning operation.
      Note: This function should not be called from client code
      (use OdDbDatabase::wblockCloneObjects() instead.
      The purpose of this function is to have ability to override it in custom classes.

      Arguments:
      idMap (I/O) Current Object ID map.

      Return Value:
      A smart pointer to the newly created clone, or a null smart pointer if the cloning
      operation is suppressed for this object.
  */      
  virtual OdDbObjectPtr wblockClone(OdDbIdMapping& idMap) const;
  
  virtual void appendToOwner(OdDbIdPair& pair, OdDbObject* pOwner, OdDbIdMapping& idMap);

  /** Description:
      Called on newly created clones, to indicate that reference Object ID's are not valid because
      the entire clone operation has not yet been completed.
  */
  void setOdDbObjectIdsInFlux();
  
  /** Description:
      Returns true if Object ID's referenced by cloned objects are not valid because
      the entire clone operation has not yet been completed.
  */
  bool isOdDbObjectIdsInFlux() const;
  
  /** Description:
      Notification function called when an object has been copied (cloned).

      Arguments:
      dbObj (I) Object that was copied.
      newObj (I) Newly created clone.

      Remarks:
      The default implementation of this function does nothing.  This function can be
      overridden in derived classes.
  */
  virtual void copied(const OdDbObject* dbObj, const OdDbObject* newObj);
  
  /** Description:
      Notification function called when an object has been erased or unerased.

      Arguments:
      pObject (I) Object that was erased/unerased.
      erasing (I) If true, the object is being erased, otherwise it is being unerased.

      Remarks:
      The default implementation of this function does nothing.  This function can be
      overridden in derived classes.
  */
  virtual void erased(const OdDbObject* pObject, bool erasing = true);
  
  /** Description:
      Notification function called just before an object is deleted from memory (an object 
      is deleted when its reference count reaches 0).

      Arguments:
      pObject (I) Object that is to be deleted.

      Remarks:
      The default implementation of this function does nothing.  This function can be
      overridden in derived classes.
  */
  virtual void goodbye(const OdDbObject* pObject);
    
  /** Description:
      Notification function called when an object is opened for modify (write access).

      Arguments:
      dbObj (I) Object that is being opened.

      Remarks:
      The default implementation of this function does nothing.  This function can be
      overridden in derived classes.
  */
  virtual void openedForModify(const OdDbObject* dbObj);
  
  /** Description:
      Notification function called when an object is opened for write access, a function
      has been called that could modify the contents of this object, and the object is now being
      closed.

      Arguments:
      dbObj (I) Object that is being closed (after being modified).

      Remarks:
      The default implementation of this function does nothing.  This function can be
      overridden in derived classes.
  */
  virtual void modified(const OdDbObject* dbObj);
  
  virtual void subObjModified(const OdDbObject* dbObj, const OdDbObject* subObj);
  
  virtual void modifyUndone(const OdDbObject* dbObj);
  
  virtual void modifiedXData(const OdDbObject* dbObj);
  
  virtual void unappended(const OdDbObject* dbObj);
  
  virtual void reappended(const OdDbObject* dbObj);

  bool isAProxy() const;  

  /** Description:
      Notification function called immediately before an object is closed.

      Arguments:
      objId (I) Object ID of the object that is being closed.

      Remarks:
      The default implementation of this function does nothing.  This function can be
      overridden in derived classes.
  */
  virtual void objectClosed(const OdDbObjectId& objId);
  
  virtual void modifiedGraphics(const OdDbEntity* dbEnt);
    
  /** Description:
      Copies the contents of the specified object, into this object when possible.

      Remarks:
      The source and target objects need not be of the same type.  
      This function is meaningful only when implemented by derived classes.
  */
  virtual void copyFrom(const OdRxObject* pSrc);
  
  bool hasSaveVersionOverride() const;
  
  void setHasSaveVersionOverride(bool bSetIt);
  
  virtual OdDb::DwgVersion getObjectSaveVersion(const OdDbFiler* pFiler,
    OdDb::MaintReleaseVer* pMaintVer = NULL) const;
      
  virtual OdDbObjectPtr decomposeForSave(OdDb::DwgVersion ver, OdDbObjectId& replaceId, bool& exchangeXData);

  virtual OdUInt32 setAttributes(OdGiDrawableTraits* pTraits) const;
  
  virtual bool worldDraw(OdGiWorldDraw* pWd) const;
  
  virtual void viewportDraw(OdGiViewportDraw* pVd) const;
  
  virtual bool isPersistent() const;
  
  virtual OdDbStub* id() const;
    
  /** Description:
      Returns the CLSID value associated with this object (valid only
      on Windows when DWGdirect is built as a DLL).
  */
  virtual void getClassID(void** pClsid) const;


  // Override of OdGiDrawable
  void setGsNode(OdGsNode* pNode);

  // Override of OdGiDrawable
  OdGsNode* gsNode() const;

  /*
    void upgradeFromNotify(bool& wasWritable);
    void downgradeToNotify(bool wasWritable);
    OdResult closeAndPage(bool onlyWhenClean = true);
    virtual void swapReferences(const OdDbIdMapping& idMap);
    virtual OdGiDrawable* drawable();
    OdDbObjPtrArray* reactors(); 
    virtual OdRxObjectPtr clone() const;
  */

  void xDataTransformBy(const OdGeMatrix3d& xform);

  bool hasFields(void) const;

  OdDbObjectId getField(const OdString& propName) const;
  OdDbObjectPtr getField(const OdString& propName, OdDb::OpenMode mode) const;

  virtual OdDbObjectId setField(const OdString& propName, OdDbField* pField);
  virtual OdResult removeField(OdDbObjectId fieldId);
  virtual OdDbObjectId removeField(const OdString& propName);

  OdDbObjectId getFieldDictionary(void) const;
  OdDbObjectPtr getFieldDictionary(OdDb::OpenMode mode) const;

  /** Description:
      For internal use only.
  */
  virtual OdRxClass* saveAsClass(OdRxClass* pClass) const;

protected:
  friend class OdDbSystemInternals;
  OdDbObjectImpl* m_pImpl;
};

typedef OdSmartPtr<OdDbObject> OdDbObjectPtr;

#include "DD_PackPop.h"

#endif //_ODDBXOBJECT_INCLUDED_


