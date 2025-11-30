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



#ifndef OD_DBPROXY_H
#define OD_DBPROXY_H

#include "DD_PackPush.h"

#include "DbObject.h"
#include "DbIntArray.h"

/** Description:
    Represents a proxy object in an OdDbDatabase.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbProxyObject : public OdDbObject
{
public:
  ODDB_DECLARE_MEMBERS(OdDbProxyObject);

  OdDbProxyObject();

  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;

  virtual OdResult dxfIn(OdDbDxfFiler* filer);

  /** Description:
      Returns the edit flags for the class associated with this proxy.
  */
  int proxyFlags() const;

  /** Description:
      Returns the class name of the entity stored in this proxy.
  */
  OdString originalClassName() const;

  /** Description:
      Returns the DXF name of the entity stored in this proxy.
  */
  OdString originalDxfName() const;

  /** Description:
      Returns the application description for the class associated with this proxy.
  */
  OdString applicationDescription() const;

  /** Description:
      Returns the references maintained by this proxy.
  */
  /** Description:
      @param ids (O) Object ID array of references contained in this proxy.

      Arguments:
      types (O) Reference type array, where each entry is associated with the 
      corresponding entry in the ids array.
  */
	void getReferences(OdTypedIdsArray& ids) const;

  /** Description:
      Returns the merge style for this proxy.
  */
  OdDb::DuplicateRecordCloning mergeStyle() const;

  /** Description:
      Performs a deep clone of this object.

      Arguments:
      idMap (I/O) Current Object ID map.

      Return Value:
      A smart pointer to the newly created clone, or a null smart pointer if the cloning
      operation failed.
  */
  OdDbObjectPtr deepClone(OdDbIdMapping& idMap) const;

  /** Description:
      Clones this object. Only hard pointer references are following during this cloning operation.

      Arguments:
      idMap (I/O) Current Object ID map.

      Return Value:
      A smart pointer to the newly created clone, or a null smart pointer if the cloning
      operation failed.
  */      
  OdDbObjectPtr wblockClone(OdDbIdMapping& idMap) const;

  enum
  {
    kNoOperation          = 0,
    kEraseAllowed         = 0x1,
    kCloningAllowed       = 0x80,
    kAllButCloningAllowed = 0x1,
    kAllAllowedBits       = 0x81,
    kMergeIgnore          = 0,      
    kMergeReplace         = 0x100,  
    kMergeMangleName      = 0x200  
  };

  bool eraseAllowed() const               { return GETBIT(proxyFlags(), kEraseAllowed); }
  bool allButCloningAllowed() const       { return (proxyFlags() & kAllAllowedBits) == (kAllAllowedBits ^ kAllButCloningAllowed); }
  bool cloningAllowed() const             { return GETBIT(proxyFlags(), kCloningAllowed); }
  bool allOperationsAllowed() const       { return (proxyFlags() & kAllAllowedBits) == kAllAllowedBits; }
  bool isR13FormatProxy() const           { return GETBIT(proxyFlags(), 32768); }

  virtual OdResult subErase(bool erasing);
};

typedef OdSmartPtr<OdDbProxyObject> OdDbProxyObjectPtr;

#include "DD_PackPop.h"

#endif // OD_DBPROXY_H


