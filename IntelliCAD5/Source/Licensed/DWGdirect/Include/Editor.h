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



#ifndef   _ODED_H_INCLUDED_
#define   _ODED_H_INCLUDED_

#include "DD_PackPush.h"

#include "RxNames.h"
#include "RxEvent.h"
#include "OdArrayPreDef.h"
#include "DbObjectId.h"
#include "IdArrays.h"

#define ODED_EDITOR_OBJ "OdEditor"

class OdDbDatabase;

/** Description:

    {group:Other_Classes}
*/
class OdEditorReactor : public OdRxEventReactor 
{ 
public:
  ODRX_DECLARE_MEMBERS(OdEditorReactor);

  /*
  // Lisp Events
  virtual void lispWillStart(const OdChar* firstLine);
  virtual void lispEnded();
  virtual void lispCancelled();
  */

  // DWG Events.
  virtual void beginDwgOpen(const OdChar* filename);
  virtual void endDwgOpen  (const OdChar* filename);
  virtual void beginClose  (OdDbDatabase* pDwg);
  
  // DWG/Save Events.
  virtual void dwgFileOpened(OdDbDatabase*, const OdChar* fileName);
  
  // XREF-related Events
  virtual void beginAttach (OdDbDatabase* pTo, const OdChar*, OdDbDatabase* pFrom);
  virtual void otherAttach (OdDbDatabase* pTo, OdDbDatabase* pFrom);
  virtual void abortAttach (OdDbDatabase* pFrom);
  virtual void endAttach   (OdDbDatabase* pTo);
  virtual void redirected  (OdDbObjectId  newId, OdDbObjectId oldId);
  virtual void comandeered (OdDbDatabase* pTo, OdDbObjectId id, OdDbDatabase* pFrom);
  virtual void beginRestore(OdDbDatabase* pTo, const OdChar*, OdDbDatabase* pFrom);
  virtual void abortRestore(OdDbDatabase* pTo);
  virtual void endRestore  (OdDbDatabase* pTo);
  
  // More XREF related Events
  virtual void xrefSubcommandBindItem   (int activity, OdDbObjectId blockId);
  virtual void xrefSubcommandAttachItem (int activity, const OdChar* pPath);
  virtual void xrefSubcommandOverlayItem(int activity, const OdChar* pPath);
  virtual void xrefSubcommandDetachItem (int activity, OdDbObjectId blockId);
  virtual void xrefSubcommandPathItem   (int activity, OdDbObjectId blockId, const OdChar* pNewPath);
  virtual void xrefSubcommandReloadItem (int activity, OdDbObjectId blockId);
  virtual void xrefSubcommandUnloadItem (int activity, OdDbObjectId blockId);
  
  // UNDO Events 
  virtual void undoSubcommandAuto   (int activity, bool state);
  virtual void undoSubcommandControl(int activity, int option);
  virtual void undoSubcommandBegin  (int activity);
  virtual void undoSubcommandEnd    (int activity);
  virtual void undoSubcommandMark   (int activity);
  virtual void undoSubcommandBack   (int activity);
  virtual void undoSubcommandNumber (int activity, int num);
  
  virtual void pickfirstModified();
  virtual void layoutSwitched(const OdChar* newLayoutName);
  
  // window messages
  virtual void docFrameMovedOrResized(long hwndDocFrame, bool bMoved);
  virtual void mainFrameMovedOrResized(long hwndMainFrame, bool bMoved);
  
  // Mouse events
  virtual void beginDoubleClick(const OdGePoint3d& clickPoint);
  virtual void beginRightClick(const OdGePoint3d& clickPoint);
  
  // Toolbar Size changes
  virtual void toolbarBitmapSizeWillChange(bool bLarge);
  virtual void toolbarBitmapSizeChanged(bool bLarge);
  
  // Partial Open Events
  virtual void objectsLazyLoaded(const OdDbObjectIdArray& idArray);
  
  // Quit Events
  virtual void beginQuit();
  virtual void quitAborted();
  virtual void quitWillStart();
  
  virtual void modelessOperationWillStart(const OdChar* contextStr);
  virtual void modelessOperationEnded(const OdChar* contextStr);
};

#define odedEditor OdEditor::cast(odrxSysRegistry()->getAt(ODED_EDITOR_OBJ))


/** Description:

    {group:Other_Classes}
*/
class OdEditor : public OdRxEvent 
{ 
public:
  ODRX_DECLARE_MEMBERS(OdEditor);
};

#include "DD_PackPop.h"

#endif // _ODED_H_INCLUDED_


