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



#ifndef _ODDBLAYERTABLETABLERECORD_INCLUDED
#define _ODDBLAYERTABLETABLERECORD_INCLUDED

#include "DD_PackPush.h"

#include "DbSymbolTableRecord.h"
#include "CmColor.h"

class OdDbLayerTable;

/** Description:
    Represents a layer in the OdDbDatabase.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbLayerTableRecord : public OdDbSymbolTableRecord
{
public:
  ODDB_DECLARE_MEMBERS(OdDbLayerTableRecord);

  /** Description:
      Constructor (no arguments).
  */
  OdDbLayerTableRecord();

  typedef OdDbLayerTable TableType;

  /** Description:
      Returns true if this layer is frozen, false otherwise (DXF 70, bit 0x01).
  */
  bool isFrozen() const;

  /** Description:
      Sets the frozen status of this layer (DXF 70, bit 0x01).

      Arguments:
      frozen (I) True if layer is to be set to frozen, false if it is to be set
      to thawed.
  */
  void setIsFrozen(bool frozen);

  /** Description:
      Returns true if this layer is off, false otherwise (DXF 62 sign: if this layer
      is off the sign of the 62 color value will be negative).
  */
  bool isOff() const;

  /** Description:
      Sets the on/off status of this layer (DXF 62 sign: if this layer
      is off the sign of the 62 color value will be negative).

      Arguments:
      off (I) True if layer is to be set to off, false if it is to be set to on.
  */
  void setIsOff(bool off);

  /** Description:
      Returns true if this layer is frozen in new viewports, 
      false otherwise (DXF 70, bit 0x02).
  */
  bool VPDFLT() const;

  /** Description:
      Sets the frozen in new viewports status of this layer (DXF 70, bit 0x02).

      Arguments:
      frozen (I) True if layer is to be set to frozen in new viewports, false 
      if it is to be set to thawed in new viewports.
  */
  void setVPDFLT(bool frozen);

  /** Description:
      Returns true if this layer is locked, false otherwise (DXF 70, bit 0x04).  
  */
  bool isLocked() const;

  /** Description:
      Sets the locked status of this layer (DXF 70, bit 0x04).

      Arguments:
      locked (I) True if layer is to be set to locked, false if it is to be set
      to unlocked.
  */
  void setIsLocked(bool locked);

  /** Description:
      Returns the color for this layer (DXF 62).

      See Also:
      OdCmColor */
  OdCmColor color() const;

  /** Description:
      Sets the color for  this layer (DXF 62).

      Arguments:
      color (I) Layer color. */
  void setColor(const OdCmColor &color);
  

  /** Description:
      Returns the color for this layer (DXF 62).

      See Also:
      OdCmColor
  */
  OdInt16 colorIndex() const;

  /** Description:
      Sets the color for  this layer (DXF 62).

      Arguments:
      color (I) Layer color.
  */
  void setColorIndex(OdInt16 colorInd);

  /** Description:
      Returns the Object ID for the linetype associated with this layer (DXF 6).
  */
  OdDbObjectId linetypeObjectId() const;

  /** Description:
      Sets the Object ID for the linetype associated with this layer (DXF 6).

      Arguments:
      id (I) Linetype Object ID for this layer.
  */
  void setLinetypeObjectId(const OdDbObjectId& id);

  /** Description:
      Returns true if this layer is plottable, false otherwise (DXF 290).
  */
  bool isPlottable() const;

  /** Description:
      Sets the plottable status of this layer (DXF 290).

      Arguments:
      plot (I) True if this layer is to be set to plottable, false otherwise.

      Remarks:
      Some layers can't be set to plottable (for example, the "DefPoints" layer).
  */
  void setIsPlottable(bool plot);

  /** Description:
      Returns the lineweight associated with this layer (DXF 370).
  */
  OdDb::LineWeight lineWeight() const;

  /** Description:
      Sets the lineweight associated with this layer (DXF 370).

      Arguments:
      weight (I) Lineweight.
  */
  void setLineWeight(OdDb::LineWeight weight);

  /** Description:
      Retuns the name of the plot style used by this layer (DXF 390).
  */
  OdString plotStyleName() const;

  /** Description:
      Returns the Object ID of the plot style used by this layer (DXF 390).
  */
  OdDbObjectId plotStyleNameId() const;

  /** Description:
      Sets the plot style used by this layer, by name (DXF 390).

      Return Value:
      eOk if newName represents a valid plot style, otherwise an appropriate error
      value.
  */
  OdResult setPlotStyleName(const OdChar* newName);

  /** Description:
      Sets the plot style used by this layer, by Object ID (DXF 390).

      Arguments:
      newId Object ID of plot style name (OdDbPlaceHolder object).
  */
  void setPlotStyleName(const OdDbObjectId& newId);
  
  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  void appendToOwner(OdDbIdPair& pair, OdDbObject* pOwner, OdDbIdMapping& idMap);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;

  virtual OdResult dxfInFields_R12(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields_R12(OdDbDxfFiler* pFiler) const;

  virtual void getClassID(void** pClsid) const;

  virtual OdUInt32 setAttributes(OdGiDrawableTraits* pTraits) const;

  /* 
     OdGiDrawable* drawable();
     bool worldDraw(OdGiWorldDraw* pWd) const;
     void viewportDraw(OdGiViewportDraw* pVd);
     void setGsNode(OdGsNode* pNode);
     OdGsNode* gsNode() const;
  */


  /** Description:
      Returns false if the layer was not in use when
      OdDbLayerTable::generateUsageData() was last called.
      Otherwise, returns true. 

      Usage data is not available (and the function returns true)
      if the OdDbLayerTableRecord is not database resident
      or if OdDbLayerTable::generateUsageData() has not been called
      since this OdDbLayerTableRecord was constructed.
  */
  bool isInUse() const;


  /** Description:
      Returns layer description string.
  */
  OdString description() const;

  /** Description:
      Sets layer description string.

      Arguments:
      descr (I) - description string up to 255 characters length.
  */
  void setDescription(const OdString& descr);
};

typedef OdSmartPtr<OdDbLayerTableRecord> OdDbLayerTableRecordPtr;

#include "DD_PackPop.h"

#endif // _ODDBLAYERTABLETABLERECORD_INCLUDED


