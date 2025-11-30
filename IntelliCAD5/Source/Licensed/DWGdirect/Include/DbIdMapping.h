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



#ifndef __ODDBIDMAPPING_INCLUDED__
#define __ODDBIDMAPPING_INCLUDED__

#include "DD_PackPush.h"

#include "OdaDefs.h"
#include "DbObjectId.h"
#include "DbObject.h"
#include "RxObject.h"

class OdDbDatabase;

/** Description:

    {group:OdDb_Classes}
*/
class OdDbIdPair
{
public:
  inline OdDbIdPair();
  inline OdDbIdPair(const OdDbIdPair& idPair);
  inline OdDbIdPair(const OdDbObjectId& key);
  inline OdDbIdPair(const OdDbObjectId& key, const OdDbObjectId& value, bool bCloned, bool bOwnerXlated = true);
  
  inline OdDbObjectId key() const;
  inline OdDbObjectId value() const;
  inline bool isCloned() const;
  inline bool isOwnerXlated() const;
  
  inline OdDbIdPair& set(const OdDbObjectId& key, const OdDbObjectId& value,
                         bool bCloned = false, bool bOwnerXlated = true);
  inline void setKey(const OdDbObjectId& key);
  inline void setValue(const OdDbObjectId& value);
  inline void setCloned(bool bCloned);
  inline void setOwnerXlated(bool bOwnerXlated);
  
private:
  OdDbObjectId m_Key;
  OdDbObjectId m_Value;
  bool m_bCloned;
  bool m_bOwnerXlated;
};

class OdDbIdMappingIter;
typedef OdSmartPtr<OdDbIdMappingIter> OdDbIdMappingIterPtr;

namespace OdDb
{
  enum DeepCloneType
  {
    kDcCopy           = 0,
    kDcExplode        = 1,
    kDcBlock          = 2,
    kDcXrefBind       = 3,
    kDcSymTableMerge  = 4,
    kDcInsert         = 6,    // moves entities
    kDcWblock         = 7,
    kDcObjects        = 8,
    kDcXrefInsert     = 9,
    kDcInsertCopy     = 10,    // copies entities
    kDcWblkObjects    = 11
  };
}

/** Description:

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbIdMapping : public OdRxObject
{
public:
  ODRX_DECLARE_MEMBERS(OdDbIdMapping);

  OdDbIdMapping() {}
  
  virtual void assign(const OdDbIdPair& idPair) = 0;
  virtual bool compute(OdDbIdPair& idPair) const = 0;
  virtual bool del(OdDbObjectId key) = 0;
  virtual OdDbIdMappingIterPtr newIterator() = 0;
  
  virtual OdDbDatabase* destDb() const = 0;
  virtual void setDestDb(OdDbDatabase* pDb) = 0;
  virtual OdDbDatabase* origDb() const = 0;
  virtual OdDbObjectId insertingXrefBlockId() const = 0;
  
  virtual OdDb::DeepCloneType deepCloneContext() const = 0;
  virtual OdDb::DuplicateRecordCloning duplicateRecordCloning() const = 0;
};

typedef OdSmartPtr<OdDbIdMapping> OdDbIdMappingPtr;

/** Description:

    {group:OdDb_Classes}
*/
class OdDbIdMappingIter : public OdRxObject
{
public:
  ODRX_DECLARE_MEMBERS(OdDbIdMappingIter);

  OdDbIdMappingIter() {}
  
  virtual void start() = 0;
  virtual void getMap(OdDbIdPair& idPair) = 0;
  virtual void next() = 0;
  virtual bool done() = 0;
};

inline OdDbIdPair::OdDbIdPair() : m_bCloned(false), m_bOwnerXlated(false) { }

inline OdDbIdPair::OdDbIdPair(const OdDbIdPair& idPair)
  : m_Key(idPair.key())
  , m_Value(idPair.value())
  , m_bCloned(idPair.isCloned())
  , m_bOwnerXlated(idPair.isOwnerXlated()) { }

inline OdDbIdPair::OdDbIdPair(const OdDbObjectId& key, const OdDbObjectId& value,
  bool bCloned, bool bOwnerXlated)
  : m_Key(key)
  , m_Value(value)
  , m_bCloned(bCloned)
  , m_bOwnerXlated(bOwnerXlated) { }

inline OdDbIdPair::OdDbIdPair(const OdDbObjectId& key)
  : m_Key(key)
  , m_bCloned(false)
  , m_bOwnerXlated(false) { }

inline OdDbObjectId OdDbIdPair::key() const { return m_Key; }

inline OdDbObjectId OdDbIdPair::value() const { return m_Value; }

inline bool OdDbIdPair::isCloned() const { return m_bCloned; }

inline bool OdDbIdPair::isOwnerXlated() const { return m_bOwnerXlated; }

inline OdDbIdPair& OdDbIdPair::set(const OdDbObjectId& key, const OdDbObjectId& value,
                                   bool bCloned, bool bOwnerXlated)
{
  setKey(key);
  setValue(value);
  setCloned(bCloned);
  setOwnerXlated(bOwnerXlated);
  return *this;
}

inline void OdDbIdPair::setKey(const OdDbObjectId& key) { m_Key = key; }

inline void OdDbIdPair::setValue(const OdDbObjectId& value) { m_Value = value; }

inline void OdDbIdPair::setCloned(bool isCloned) { m_bCloned = isCloned; }

inline void OdDbIdPair::setOwnerXlated(bool isOwnerXlated) { m_bOwnerXlated = isOwnerXlated; }

#include "DD_PackPop.h"

#endif // __ODDBIDMAPPING_INCLUDED__



