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



#ifndef _ODDBVIEWTABLERECORD_INCLUDED
#define _ODDBVIEWTABLERECORD_INCLUDED

#include "DD_PackPush.h"

#include "DbAbstractViewTableRecord.h"

/** Description:
    Represents a named view in an OdDbDatabase.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbViewTableRecord : public OdDbAbstractViewTableRecord
{
public:
  ODDB_DECLARE_MEMBERS(OdDbViewTableRecord);

  /** Description:
      Constructor (no arguments).
  */
  OdDbViewTableRecord();

  bool isPaperspaceView() const;
  void setIsPaperspaceView(bool pspace);
  
  bool isUcsAssociatedToView() const;
  void disassociateUcsFromView();

  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;

  virtual OdResult dxfInFields_R12(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields_R12(OdDbDxfFiler* pFiler) const;

	virtual void getClassID(void** pClsid) const;
};

typedef OdSmartPtr<OdDbViewTableRecord> OdDbViewTableRecordPtr;

/** Description:

    {group:OdDb_Classes} 
*/
class OdDbAbstractViewportDataForDbViewTabRec : public OdDbAbstractViewportDataForAbstractViewTabRec
{
public:
  OdDbAbstractViewportDataForDbViewTabRec() {}
 
  void adjustGsView(const OdDbObject* /*pVp*/, OdGsView* /*pView*/){return;}

  void adjustByViewport(const OdDbViewport* pVp, OdDbObject* pVRec);
  void adjustByViewportRec(const OdDbViewportTableRecord* pVp, OdDbObject* pVRec);

  void adjustViewport(const OdDbObject* pVp, OdDbViewport* pView);
  void adjustViewportRec(const OdDbObject* pVp, OdDbViewportTableRecord* pView);
};

#include "DD_PackPop.h"

#endif // _ODDBVIEWTABLERECORD_INCLUDED


