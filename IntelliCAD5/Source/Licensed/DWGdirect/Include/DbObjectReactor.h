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



#ifndef _DB_OBJECT_REACTOR_INCLUDED_
#define _DB_OBJECT_REACTOR_INCLUDED_

#include "DD_PackPush.h"

#include "RxObject.h"
#include "OdArrayPreDef.h"

/** Description:

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbObjectReactor : public OdRxObject
{
protected:
  OdDbObjectReactor() {}
public:
  ODRX_DECLARE_MEMBERS(OdDbObjectReactor);
  
	virtual void cancelled(const OdDbObject* pDbObj);
  virtual void copied(const OdDbObject* pDbObj, const OdDbObject* newObj);
  virtual void erased(const OdDbObject* pDbObj, bool pErasing = true);
  virtual void goodbye(const OdDbObject* pDbObj);
  virtual void openedForModify(const OdDbObject* pDbObj);
  virtual void modified(const OdDbObject* pDbObj);
  virtual void subObjModified(const OdDbObject* pDbObj, const OdDbObject* subObj);
  virtual void modifyUndone(const OdDbObject* pDbObj);
  virtual void modifiedXData(const OdDbObject* pDbObj);
  virtual void unappended(const OdDbObject* pDbObj);
  virtual void reappended(const OdDbObject* pDbObj);
  virtual void objectClosed(const OdDbObjectId& objId);

  static OdDbObjectReactor* findReactor(const OdDbObject* pObj, const OdRxClass* pKeyClass);
};

typedef OdSmartPtr<OdDbObjectReactor> OdDbObjectReactorPtr;
typedef OdArray<OdDbObjectReactorPtr> OdDbObjectReactorArray;

#include "DD_PackPop.h"

#endif // _DB_OBJECT_REACTOR_INCLUDED_

