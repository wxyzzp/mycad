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



#ifndef _ODDBDATABASEREACTOR_INCLUDED_
#define _ODDBDATABASEREACTOR_INCLUDED_

#include "DD_PackPush.h"

#include "RxObject.h"
#include "IdArrays.h"

/** Description:
    Class that accepts database notifications (object appended, object erased, etc.).

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbDatabaseReactor : public OdRxObject
{
public:
  ODRX_DECLARE_MEMBERS(OdDbDatabaseReactor);
  
  OdDbDatabaseReactor() {}
	
  virtual void objectAppended(const OdDbDatabase* pDb, const OdDbObject* pDbObj);

	
  virtual void objectUnAppended(const OdDbDatabase* pDb, const OdDbObject* pDbObj);

	
  virtual void objectReAppended(const OdDbDatabase* pDb, const OdDbObject* pDbObj);

	
  virtual void objectOpenedForModify(const OdDbDatabase* pDb, const OdDbObject* pDbObj);

	
  virtual void objectModified(const OdDbDatabase* pDb, const OdDbObject* pDbObj);

	
  virtual void objectErased(const OdDbDatabase* pDb, const OdDbObject* pDbObj, bool pErased = true);

	
  virtual void headerSysVarWillChange(const OdDbDatabase* pDb, const char* name);

#define VAR_DEF(type, name, d1, d2, r1, r2) \
  virtual void headerSysVar_##name##_WillChange(const OdDbDatabase* pDb);

#include "SysVarDefs.h"
#undef VAR_DEF

	
  virtual void headerSysVarChanged(const OdDbDatabase* pDb, const char* name);

#define VAR_DEF(type, name, d1, d2, r1, r2) \
  virtual void headerSysVar_##name##_Changed(const OdDbDatabase* pDb);

#include "SysVarDefs.h"
#undef VAR_DEF

	
  virtual void proxyResurrectionCompleted(const OdDbDatabase* pDb,
    const char* appname, OdDbObjectIdArray& objects);

  virtual void goodbye(const OdDbDatabase* pDb);
};

typedef OdSmartPtr<OdDbDatabaseReactor> OdDbDatabaseReactorPtr;

#include "DD_PackPop.h"

#endif // _ODDBDATABASEREACTOR_INCLUDED_

