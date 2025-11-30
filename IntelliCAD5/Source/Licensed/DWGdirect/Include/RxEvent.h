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



#ifndef   _ODRX_EVENT_H__
#define   _ODRX_EVENT_H__

#include "DD_PackPush.h"

#include "RxObject.h"

#define ODRX_EVENT_OBJ "OdRxEvent"

class OdDbDatabase;
class OdGeMatrix3d;
class OdDbIdMapping;
class OdGePoint3d;
class OdDbObjectId;

/** Description:

    {group:OdRx_Classes} 
*/
class TOOLKIT_EXPORT OdRxEventReactor : public OdRxObject 
{ 
public:
  ODRX_DECLARE_MEMBERS(OdRxEventReactor);
  
  // DWG/Save Events.
  virtual void dwgFileOpened(OdDbDatabase*, const OdChar* fileName);
  virtual void initialDwgFileOpenComplete(OdDbDatabase*);
  virtual void databaseConstructed(OdDbDatabase*);
  virtual void databaseToBeDestroyed(OdDbDatabase*);
  
  virtual void beginSave(OdDbDatabase*, const OdChar* pIntendedName);
  virtual void saveComplete(OdDbDatabase*, const OdChar* pActualName);
  virtual void abortSave(OdDbDatabase*);
  
  // DXF In/Out Events.
  virtual void beginDxfIn(OdDbDatabase*);
  virtual void abortDxfIn(OdDbDatabase*);
  virtual void dxfInComplete(OdDbDatabase*);
  //
  virtual void beginDxfOut(OdDbDatabase*);
  virtual void abortDxfOut(OdDbDatabase*);
  virtual void dxfOutComplete(OdDbDatabase*);
  
  // Insert Events.
  virtual void beginInsert(OdDbDatabase* pTo, const OdChar* pBlockName, OdDbDatabase* pFrom);
  virtual void beginInsert(OdDbDatabase* pTo, const OdGeMatrix3d& xform, OdDbDatabase* pFrom);
  virtual void otherInsert(OdDbDatabase* pTo, OdDbIdMapping& idMap, OdDbDatabase* pFrom);
  virtual void abortInsert(OdDbDatabase* pTo);
  virtual void endInsert(OdDbDatabase* pTo);
  
  // Wblock Events.
  virtual void wblockNotice(OdDbDatabase* pDb);
  virtual void beginWblock(OdDbDatabase* pTo, OdDbDatabase* pFrom, const OdGePoint3d& insertionPoint);
  virtual void beginWblock(OdDbDatabase* pTo, OdDbDatabase* pFrom, OdDbObjectId blockId);
  virtual void beginWblock(OdDbDatabase* pTo, OdDbDatabase* pFrom); 
  virtual void otherWblock(OdDbDatabase* pTo, OdDbIdMapping&, OdDbDatabase* pFrom);
  virtual void abortWblock(OdDbDatabase* pTo);
  virtual void endWblock(OdDbDatabase* pTo);
  virtual void beginWblockObjects(OdDbDatabase*, OdDbIdMapping&);
  
  // Deep Clone Events.
  virtual void beginDeepClone(OdDbDatabase* pTo, OdDbIdMapping&);
  virtual void beginDeepCloneXlation(OdDbIdMapping&);
  virtual void abortDeepClone(OdDbIdMapping&);
  virtual void endDeepClone(OdDbIdMapping&);
  
  // Partial Open Events.
  virtual void partialOpenNotice(OdDbDatabase* pDb);
  
};

typedef OdSmartPtr<OdRxEventReactor> OdRxEventReactorPtr;

/** Description:

    {group:OdRx_Classes} 
*/
class TOOLKIT_EXPORT OdRxEvent : public OdRxObject 
{ 
public:
  ODRX_DECLARE_MEMBERS(OdRxEvent);
  virtual void addReactor(OdRxEventReactor* ) = 0;
  virtual void removeReactor(OdRxEventReactor* ) = 0;
};

typedef OdSmartPtr<OdRxEvent> OdRxEventPtr;


TOOLKIT_EXPORT OdRxEventPtr odrxEvent();

#include "DD_PackPop.h"

#endif //_ODRX_EVENT_H__


