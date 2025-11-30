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



#ifndef OD_DBINDEX_H
#define OD_DBINDEX_H

#include "DD_PackPush.h"

class OdDbDate;
class OdDbIndexIterator;
class OdDbFilter;
class OdDbIndexUpdateData;
class OdDbBlockChangeIterator;
class OdDbFilter;
class OdDbBlockTableRecord;
class OdDbBlockChangeIterator;
class OdDbIndexUpdateData;
class OdDbBlockChangeIteratorImpl;
class OdDbIndexUpdateDataImpl;
class OdDbIndexUpdateDataIteratorImpl;
class OdDbFilteredBlockIterator;
typedef OdSmartPtr<OdDbFilteredBlockIterator> OdDbFilteredBlockIteratorPtr;

#include "DbFilter.h"

extern void processBTRIndexObjects(OdDbBlockTableRecord* pBlock, 
                                   int indexCtlVal,
                                   OdDbBlockChangeIterator* pBlkChgIter,
                                   OdDbIndexUpdateData* pIdxUpdData);


/** Description:
    Iterates over only the changed entities inside a block table record.

    {group:OdDb_Classes}
*/
class OdDbBlockChangeIterator
{
private:
  friend class OdDbBlockChangeIteratorImpl;
  OdDbBlockChangeIteratorImpl* m_pImpl;
  OdDbBlockChangeIterator() : m_pImpl(0) {}
  OdDbBlockChangeIterator(const OdDbBlockChangeIterator&);
public:
  void start();
  OdDbObjectId id() const;
  void next();
  bool done(); 
  
  void curIdInfo(OdDbObjectId& id, OdUInt8& flags, OdUInt32& data) const;
  void setCurIdInfo(OdUInt8 flags, OdUInt32 data);
  
  OdDbIndexUpdateData* updateData() const;
  
  void clearProcessedFlags();
};

/** Description:

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbIndexUpdateData 
{
  friend class OdDbIndexUpdateDataImpl;
  OdDbIndexUpdateDataImpl* m_pImpl;
  OdDbIndexUpdateData() : m_pImpl(0) {}
  OdDbIndexUpdateData(const OdDbIndexUpdateData&);
public:
  enum UpdateFlags 
  { 
    kModified   = 1, // Read Only
    kDeleted    = 2, // Read Only
    kProcessed  = 4, 
    kUnknownKey = 8 
  };
  
  OdDbObjectId objectBeingIndexedId() const;

  void addId(OdDbObjectId id);
  bool setIdFlags(OdDbObjectId id, OdUInt8 flags);
  bool setIdData(OdDbObjectId id, OdUInt32 data);
  bool getIdData(OdDbObjectId id, OdUInt32& data) const;
  bool getIdFlags(OdDbObjectId id, OdUInt8& flags) const; 
  bool getFlagsAndData(OdDbObjectId id, OdUInt8& flags, OdUInt32& data) const;
};

/** Description:

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbIndexUpdateDataIterator
{
  friend class OdDbIndexUpdateDataIteratorImpl;
  OdDbIndexUpdateDataIteratorImpl* m_pImpl;
public:
  OdDbIndexUpdateDataIterator(const OdDbIndexUpdateData* pIndexUpdateData);
  
  void start();
  void next();
  bool done();
  void currentData(OdDbObjectId& id, OdUInt8& flags, OdUInt32& data) const;
};


/** Description:
    Base class for index objects.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbIndex : public OdDbObject
{
public:
  ODDB_DECLARE_MEMBERS(OdDbIndex);

  OdDbIndex();
  
  virtual OdDbFilteredBlockIteratorPtr newIterator(const OdDbFilter* pFilter) const;
  
  virtual void rebuildFull(OdDbIndexUpdateData* pIdxData);
  
  void setLastUpdatedAt(const OdDbDate& time);
  OdDbDate lastUpdatedAt() const;
  
  void setLastUpdatedAtU(const OdDbDate& time);
  OdDbDate lastUpdatedAtU() const;
  
  bool isUptoDate() const; 
  
  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;

protected: 
  virtual void rebuildModified(OdDbBlockChangeIterator* iter);
  
  friend class OdDbIndexImpl;
  friend void processBTRIndexObjects(OdDbBlockTableRecord* pBTR, int indexCtlVal, 
                                     OdDbBlockChangeIterator* pBlkChgIter, 
                                     OdDbIndexUpdateData* pIdxUpdData );
};

typedef OdSmartPtr<OdDbIndex> OdDbIndexPtr;



class OdDbBlockTableRecord;
class OdDbBlockReference;

/** Description:
    Namespace that provides functions pertaining to indices and filters.

    {group:DD_Namespaces}
*/
namespace OdDbIndexFilterManager
{
  TOOLKIT_EXPORT void updateIndexes(OdDbDatabase* pDb);
  
  TOOLKIT_EXPORT void addIndex(OdDbBlockTableRecord* pBTR, OdDbIndex* pIndex);
  
  TOOLKIT_EXPORT void removeIndex(OdDbBlockTableRecord* pBTR, const OdRxClass* key);
  
  TOOLKIT_EXPORT OdDbIndexPtr getIndex(const OdDbBlockTableRecord* pBTR, const OdRxClass* key, OdDb::OpenMode readOrWrite = OdDb::kForRead);
  
  TOOLKIT_EXPORT OdDbIndexPtr getIndex(const OdDbBlockTableRecord* pBTR, int index, OdDb::OpenMode readOrWrite = OdDb::kForRead);
  
  TOOLKIT_EXPORT int numIndexes(const OdDbBlockTableRecord* pBtr);
  
  
  
  TOOLKIT_EXPORT void addFilter(OdDbBlockReference* pBlkRef, OdDbFilter* pFilter);
  
  TOOLKIT_EXPORT void removeFilter(OdDbBlockReference* pBlkRef, const OdRxClass* key);
  
  TOOLKIT_EXPORT OdDbFilterPtr getFilter(const OdDbBlockReference* pRef, const OdRxClass* key, OdDb::OpenMode readOrWrite);
  
  TOOLKIT_EXPORT OdDbFilterPtr getFilter(const OdDbBlockReference* pRef, int index, OdDb::OpenMode readOrWrite);
  
  TOOLKIT_EXPORT int numFilters(const OdDbBlockReference* pBlkRef);
}

#include "DD_PackPop.h"

#endif // OD_DBINDEX_H


