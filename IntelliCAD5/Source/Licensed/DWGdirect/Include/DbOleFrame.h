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



#ifndef OD_DBOLE_H
#define OD_DBOLE_H

#include "DD_PackPush.h"

#include "DbFrame.h"
#include "Ge/GePoint3d.h"

class TOOLKIT_EXPORT OdRectangle3d
{
public:
  OdGePoint3d upLeft;
  OdGePoint3d upRight;
  OdGePoint3d lowLeft;
  OdGePoint3d lowRight;
};
class OdRect;
class OdDbOleFrameImpl;

/** Description:

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbOleFrame : public OdDbFrame
{
public:
  ODDB_DECLARE_MEMBERS(OdDbOleFrame);

  OdDbOleFrame();
   
  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);
  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;

  /*
    virtual bool worldDraw(OdGiWorldDraw*) const;
    virtual void viewportDraw(OdGiViewportDraw* mode) const;
    virtual bool getGeomExtents(OdGeExtents3d& extents) const;
  
    virtual void* getOleObject(void) const;
    virtual void setOleObject(const void *pItem);
  */
};

typedef OdSmartPtr<OdDbOleFrame> OdDbOleFramePtr;

#include "DD_PackPop.h"

#endif // OD_DBOLE_H_


