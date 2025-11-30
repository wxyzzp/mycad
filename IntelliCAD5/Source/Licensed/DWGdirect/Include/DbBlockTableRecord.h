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



#ifndef _ODDBBLOCKTABLERECORD_INCLUDED
#define _ODDBBLOCKTABLERECORD_INCLUDED

#include "DD_PackPush.h"

#include "DbSymbolTableRecord.h"
#include "DbObjectIterator.h"
#include "DbXrefGraph.h"

class OdDbBlockBegin;
class OdDbBlockEnd;
class OdDbBlockTableRecord;
class OdDbBlockTable;
class OdBinaryData;
class OdDbSortentsTable;
typedef OdSmartPtr<OdDbBlockBegin> OdDbBlockBeginPtr;
typedef OdSmartPtr<OdDbBlockEnd> OdDbBlockEndPtr;
typedef OdSmartPtr<OdDbSortentsTable> OdDbSortentsTablePtr;

/** Description:
    Represents a block in an OdDbDatabase.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbBlockTableRecord : public OdDbSymbolTableRecord
{
public:

  ODDB_DECLARE_MEMBERS(OdDbBlockTableRecord);

  /** Description:
      Constructor (no arguments).
  */
  OdDbBlockTableRecord();

  typedef OdDbBlockTable TableType;
  typedef OdBinaryData PreviewIcon;

  /** Description:
      Adds the specified entity to this block record.

      Arguments:
      pEntity (I) Entity to add.

      Return Value:
      the Object ID of the newly added entity.
  */
  OdDbObjectId appendOdDbEntity(OdDbEntity* pEntity);

  /** Description:
      Creates an iterator for this block record.

      Arguments:
      atBeginning (I) True indicates that the iterator should start at the beginning
      and proceed forward, false indicates it should start at the end and proceed backwards.
      skipDeleted (I) True if deleted items are to be skipped by the iterator, 
      false otherwise.
      sorted (I) True indicates that iterator should take sorting of
      entities into account.

      Return Value:
      A smart pointer to the new iterator.
  */
  OdDbObjectIteratorPtr newIterator(bool atBeginning = true, 
    bool skipDeleted = true, bool sorted = false) const;

  /** Description:
      Returns the description text associated with this block (DXF 4).
  */
  OdString comments() const;

  /** Description:
      Sets the description text associated with this block (DXF 4).
  */
  void setComments(const OdString& pString);

  /** Description:
      For xref blocks, returns the file path for the drawing that is externally referenced
      (DXF 1).  Returns a null string for non-xref blocks.
  */
  OdString pathName() const;

  /** Description:
      For xref blocks, sets the file path for the drawing that is externally referenced
      (DXF 1).  
  */
  void setPathName(const OdString& pathStr);

  /** Description:
      Returns the block base point in WCS (DXF 10).
  */
  OdGePoint3d origin() const;

  /** Description:
      Sets the block base point in WCS (DXF 10).
  */
  void setOrigin(const OdGePoint3d& pt);

  /** Description:
      Opens and returns the OdDbBlockBegin entity associated with this block.

      Arguments:
      openMode (I) Mode in which to open the object.

      Return Value:
      Smart pointer to the OdDbBlockBegin entity.
  */
  OdDbBlockBeginPtr openBlockBegin(OdDb::OpenMode openMode = OdDb::kForRead);

  /** Description:
      Opens and returns the OdDbBlockEnd entity associated with this block.

      Arguments:
      openMode (I) Mode in which to open the object.

      Return Value:
      Smart pointer to the OdDbBlockEnd entity.
  */
  OdDbBlockEndPtr openBlockEnd(OdDb::OpenMode openMode = OdDb::kForRead);

  /** Description:
      Returns true if this block contains attribute definitions.

      See Also:
      OdDbAttributeDefinition
  */
  bool hasAttributeDefinitions() const;

  /** Description:
      Returns true if this block is anonymous, false otherwise (DXF 70, bit 0x01).
  */
  bool isAnonymous() const;

  /** Description:
      Returns true if this block is an external reference (DXF 70, bit 0x04).
  */
  bool isFromExternalReference() const;

  /** Description:
      Returns true if this block is an overlaid external reference (DXF 70, bit 0x80).
  */
  bool isFromOverlayReference() const;

  /** Description:
      Returns true if this block represents a layout.
  */
  bool isLayout() const;

  /** Description:
      Returns the Object ID of the layout associated with this block.
  */
  OdDbObjectId getLayoutId() const;

  /** Description:
      Sets the Object ID of the layout associated with this block.
  */
  void setLayoutId(const OdDbObjectId& id);

  /** Description:
      Returns Object ID's of all OdDbBlockReference entities that reference this block,
      directly or indirectly.

      Arguments:
      ids (O) Receives the block reference Object ID's.
      bDirectOnly (I) If true, return only direct references.  Otherwise, return
      indirect as well as direct references.
      bForceValidity (I) If true, forces the loading of partially loaded drawings,
      in order to guarantee that the returned list is accurate.  This parameter is necessary
      because older files do not store this information explicitly.
  */
  void getBlockReferenceIds(OdDbObjectIdArray& ids,
                            bool bDirectOnly = true, 
                            bool bForceValidity = false);

  /** Description:
      Retrieves the erased block reference Object ID's that reference this block.

      Arguments:
      ids (O) Receives the block reference Object ID's.
  */
  void getErasedBlockReferenceIds(OdDbObjectIdArray& ids);

  /** Description:
      Returns true if this xref is unloaded, otherwise false (DXF 71).
  */
  bool isUnloaded() const;

  /** Description:
      Sets the unloaded status of this xref (DXF 71).

      Arguments:
      isUnloaded (I) If true, this xref is set to unloaded, otherwise it is set
      to loaded.
  */
  void setIsUnloaded(bool isUnloaded);

  bool worldDraw(OdGiWorldDraw* pWd) const;

  OdUInt32 setAttributes(OdGiDrawableTraits* pTraits) const;

  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;

  virtual void subClose();

  virtual void getClassID(void** pClsid) const;

  /** Description:
      Called by the DWGdirect framework to inform a parent object that one of the 
      objects that it owns has been modified.

      Arguments:
      pSubObj (I) Sub-object that has been modified.
  */
  void recvPropagateModify(const OdDbObject* pSubObj);

  OdResult subErase(bool bErasing);

  void subHandOverTo(OdDbObject* pNewObj);
  
  OdDbDatabase* xrefDatabase(bool incUnres = false) const;

  OdDb::XrefStatus xrefStatus() const;

  bool hasPreviewIcon() const;

  void getPreviewIcon(PreviewIcon &previewIcon) const;

  void setPreviewIcon(const PreviewIcon &previewIcon);

  OdDbObjectPtr wblockClone(OdDbIdMapping& idMap) const;

  void appendToOwner(OdDbIdPair& pair, OdDbObject* pOwner, OdDbIdMapping& idMap);

  /** Description:
      Returns SortentsTable attached to this block, opened for write.

      Arguments:
      bCreateIfNotFound - defines if DWGDirect should create 
      SortentsTable if it is not found.
  */
  OdDbSortentsTablePtr getSortentsTable(bool bCreateIfNotFound = true);

  /*
      virtual void assumeOwnershipOf(OdArray<OdDbObjectId> entitiesToMove);  
      OdGiDrawable* drawable();
      void viewportDraw(OdGiViewportDraw* pVd);
      void setGsNode(OdGsNode* pNode);
      OdGsNode* gsNode() const;
  */

  virtual OdResult getGeomExtents(OdGeExtents3d& ) const ;
};

typedef OdSmartPtr<OdDbBlockTableRecord> OdDbBlockTableRecordPtr;

#include "DD_PackPop.h"

#endif 


