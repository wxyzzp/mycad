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



#ifndef _RXDICTIONARY_INC_
#define _RXDICTIONARY_INC_


#include "RxObject.h"
#include "RxIterator.h"
#include "RxDefs.h"
#include "RxNames.h"

class OdString;

#include "DD_PackPush.h"

/** Description:

    {group:OdRx_Classes} 
*/
class FIRSTDLL_EXPORT OdRxDictionaryIterator : public OdRxIterator
{
public:
  ODRX_DECLARE_MEMBERS(OdRxDictionaryIterator);

  virtual OdString getKey() const = 0;

  virtual OdUInt32 id() const = 0;
};

typedef OdSmartPtr<OdRxDictionaryIterator> OdRxDictionaryIteratorPtr;


/** Description:

    {group:OdRx_Classes} 
*/
class FIRSTDLL_EXPORT OdRxDictionary : public OdRxObject
{
public:
  ODRX_DECLARE_MEMBERS(OdRxDictionary);
  
  virtual void reserve(OdUInt32 nMinSize);

  virtual OdRxObjectPtr getAt(const OdChar* key) const = 0;
  virtual OdRxObjectPtr getAt(OdUInt32 id) const = 0;

  virtual OdRxObjectPtr putAt(const OdString& key, OdRxObject* pObject, OdUInt32* pResId = NULL) = 0;
  virtual OdRxObjectPtr putAt(OdUInt32 id, OdRxObject* pObject) = 0;

  virtual bool resetKey(OdUInt32 id, const OdString& newKey) = 0;

  virtual OdRxObjectPtr remove(const OdChar* key) = 0;
  virtual OdRxObjectPtr remove(OdUInt32 id) = 0;

  virtual bool has(const OdChar* entryName) const = 0;
  virtual bool has(OdUInt32 id) const = 0;

  virtual OdUInt32 idAt(const OdChar* key) const = 0;

  virtual OdString keyAt(OdUInt32 id) const = 0;

  virtual OdUInt32 numEntries() const = 0;

  virtual OdRxDictionaryIteratorPtr newIterator(OdRx::DictIterType type = OdRx::kDictCollated) = 0;

  virtual bool isCaseSensitive() const = 0;
};

typedef OdSmartPtr<OdRxDictionary> OdRxDictionaryPtr;

FIRSTDLL_EXPORT OdRxDictionary* odrxSysRegistry();

FIRSTDLL_EXPORT OdRxDictionaryPtr odrxClassDictionary();

FIRSTDLL_EXPORT OdRxDictionaryPtr odrxServiceDictionary();

FIRSTDLL_EXPORT OdRxDictionaryPtr odrxCreateRxDictionary();


#include "DD_PackPop.h"

#endif // _RXDICTIONARY_INC_


