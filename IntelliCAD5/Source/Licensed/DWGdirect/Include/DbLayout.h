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



#ifndef OD_DBLAYOUT_H
#define OD_DBLAYOUT_H

#include "DD_PackPush.h"

#include "DbPlotSettings.h"


class OdString;

/** Description:
    Represents a layout object in an OdDbDatabase.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbLayout : public OdDbPlotSettings
{
public:
  ODDB_DECLARE_MEMBERS(OdDbLayout);

  /** Description:
      Constructor (no arguments).
  */
  OdDbLayout();

  ///////// BEGIN ODA SPECIFIC /////////////

  /** Description:
      Returns the minimum limits value for this layout (DXF 10).
  */
  const OdGePoint2d& getLIMMIN() const;

  /** Description:
      Sets the minimum limits value for this layout (DXF 10).
  */
  void setLIMMIN(const OdGePoint2d& val);

  /** Description:
      Returns the maximum limits value for this layout (DXF 11).
  */
  const OdGePoint2d& getLIMMAX() const;

  /** Description:
      Sets the maximum limits value for this layout (DXF 11).
  */
  void setLIMMAX(const OdGePoint2d& val);

  /** Description:
      Returns the minimum extents value for this layout (DXF 14).
  */
  const OdGePoint3d& getEXTMIN() const;

  /** Description:
      Sets the minimum extents value for this layout (DXF 14).
  */
  void setEXTMIN(const OdGePoint3d& val);

  /** Description:
      Returns the maximum extents value for this layout (DXF 15).
  */
  const OdGePoint3d& getEXTMAX() const;

  /** Description:
      Sets the maximum extents value for this layout (DXF 15).
  */
  void setEXTMAX(const OdGePoint3d& val);

  /** Description:
      Returns the insertion base point for this layout (DXF 12).
  */
  const OdGePoint3d& getINSBASE() const;

  /** Description:
      Sets the insertion base point for this layout (DXF 12).
  */
  void setINSBASE(const OdGePoint3d& val);

  /** Description:
      Returns the LIMCHECK value for this layout (DXF 70, bit 0x02).
  */
  bool getLIMCHECK() const;

  /** Description:
      Sets the LIMCHECK value for this layout (DXF 70, bit 0x02).
  */
  void setLIMCHECK(bool val);

  /** Description:
      Returns the PSLTSCALE value for this layout (DXF 70, bit 0x01).
  */
  bool getPSLTSCALE() const;

  /** Description:
      Sets the PSLTSCALE value for this layout (DXF 70, bit 0x01).
  */
  void setPSLTSCALE(bool val);

  ///////// END ODA SPECIFIC /////////////

  
  /** Description:
      Returns the Object ID of the paperspace OdDbBlockTableRecord associated with this layout (DXF 330).
  */
  OdDbObjectId getBlockTableRecordId() const;

  /** Description:
      Sets the Object ID of the paperspace OdDbBlockTableRecord associated with this layout (DXF 330).
  */
  virtual void setBlockTableRecordId(const OdDbObjectId& BlockTableRecordId);

  /** Description:
      Adds this layout to the layout dictionary in the specified database, and associates this layout
      with the specified paperspace OdDbBlockTableRecord ID.
  */
  virtual void addToLayoutDict(OdDbDatabase* towhichDb, OdDbObjectId BlockTableRecordId);
  
  /** Description:
      Returns the name of this layout (DXF 1).
  */
  OdString getLayoutName() const;

  /** Description:
      Sets the name of this layout (DXF 1).
  */
  virtual void setLayoutName(const OdString& layoutName);
  
  /** Description:
      Returns the tab order for this layout (DXF 71).
  */
  int getTabOrder() const;

  /** Description:
      Sets the tab order for this layout (DXF 71).
  */
  virtual void setTabOrder(int newOrder);
  
  /** Description:
      Returns true if this layout tab is selected, false otherwise.
  */
  bool getTabSelected() const;

  /** Description:
      Returns the selected status for this layout tab.
  */
  virtual void setTabSelected(bool tabSelected);
  
  virtual void getClassID(void** pClsid) const;

  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  void appendToOwner(OdDbIdPair& pair, OdDbObject* pOwner, OdDbIdMapping& idMap);

  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;

  /** Description:
      Returns the Object ID of the viewport that was last active
      in this layout object (DXF 331).
  */
  OdDbObjectId activeViewportId() const;

  /** Description:
      Sets the Object ID of the active viewport
      in this layout object (DXF 331).
  */
  void setActiveViewportId(OdDbObjectId id);

  /** Description:
      Returns the Object ID of the overall viewport
      in this layout object.
  */
  OdDbObjectId overallVportId() const;

  OdUInt32 setAttributes(OdGiDrawableTraits* traits) const;

  bool worldDraw(OdGiWorldDraw* pWd) const;

  void viewportDraw(OdGiViewportDraw* ) const;
};

// !!! SYMBOL RENAMED !!! USE activeViewportId()
#define lastActiveVportId() activeViewportId()

typedef OdSmartPtr<OdDbLayout> OdDbLayoutPtr;

#include "DD_PackPop.h"

#endif


