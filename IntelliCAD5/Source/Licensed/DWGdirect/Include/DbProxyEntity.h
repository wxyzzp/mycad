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



#ifndef _OD_DB_PROXY_ENTITY_
#define _OD_DB_PROXY_ENTITY_

#include "DD_PackPush.h"

#include "DbEntity.h"
#include "IdArrays.h"

/** Description:
    Represents a proxy entity in an OdDbDatabase.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbProxyEntity : public OdDbEntity
{
public:
  ODDB_DECLARE_MEMBERS(OdDbProxyEntity);

  /** Description:
      Constructor (no arguments).
  */
  OdDbProxyEntity();

  enum GraphicsMetafileType
  {
    kNoMetafile   = 0,
    kBoundingBox  = 1,
    kFullGraphics = 2
  };

  /** Description:
      TBC.
  */
  virtual OdDbProxyEntity::GraphicsMetafileType graphicsMetafileType() const;

  bool worldDraw(OdGiWorldDraw* pWd) const;

  OdDbObjectPtr deepClone(OdDbIdMapping& idMap) const;

  OdDbObjectPtr wblockClone(OdDbIdMapping& idMap) const;

  virtual OdResult explode(OdRxObjectPtrArray& entitySet) const; /* Replace OdRxObjectPtrArray */

  /*
     virtual bool getGeomExtents(OdGeExtents3d& extents) const;
  */

  enum
  {
    kNoOperation                = 0,
    kEraseAllowed               = 0x1,
    kTransformAllowed           = 0x2,
    kColorChangeAllowed         = 0x4,
    kLayerChangeAllowed         = 0x8,
    kLinetypeChangeAllowed      = 0x10,
    kLinetypeScaleChangeAllowed = 0x20,
    kVisibilityChangeAllowed    = 0x40,
    kCloningAllowed             = 0x80,
    kLineWeightChangeAllowed    = 0x100,
    kPlotStyleNameChangeAllowed = 0x200,
    kAllButCloningAllowed       = 0x37F,
    kAllAllowedBits             = 0x3FF
  };

  /** Description:
      Returns the edit flags for the class associated with this proxy.
  */
  virtual int proxyFlags() const;

  bool eraseAllowed() const               { return GETBIT(proxyFlags(), kEraseAllowed); }
  bool transformAllowed() const           { return GETBIT(proxyFlags(), kTransformAllowed); }
  bool colorChangeAllowed() const         { return GETBIT(proxyFlags(), kColorChangeAllowed); }
  bool layerChangeAllowed() const         { return GETBIT(proxyFlags(), kLayerChangeAllowed); }
  bool linetypeChangeAllowed() const      { return GETBIT(proxyFlags(), kLinetypeChangeAllowed); }
  bool linetypeScaleChangeAllowed() const { return GETBIT(proxyFlags(), kLinetypeScaleChangeAllowed); }
  bool visibilityChangeAllowed() const    { return GETBIT(proxyFlags(), kVisibilityChangeAllowed); }
  bool allButCloningAllowed() const       { return (proxyFlags() & kAllAllowedBits) == (kAllAllowedBits ^ kAllButCloningAllowed); }
  bool cloningAllowed() const             { return GETBIT(proxyFlags(), kCloningAllowed); }
  bool lineWeightChangeAllowed() const    { return GETBIT(proxyFlags(), kLineWeightChangeAllowed); }
  bool plotStyleNameChangeAllowed() const { return GETBIT(proxyFlags(), kPlotStyleNameChangeAllowed); }
  bool allOperationsAllowed() const       { return (proxyFlags() & kAllAllowedBits) == kAllAllowedBits; }
  bool isR13FormatProxy() const           { return GETBIT(proxyFlags(), 32768); }


  /** Description:
      Returns the class name of the entity stored in this proxy.
  */
  virtual OdString originalClassName() const;

  /** Description:
      Returns the DXF name of the entity stored in this proxy.
  */
  virtual OdString originalDxfName() const;

  /** Description:
      Returns the application description for the class associated with this proxy.
  */
  virtual OdString applicationDescription() const;

  /** Description:
      Returns the references maintained by this proxy.

      Arguments:
      ids (O) Object ID array of references contained in this proxy.
      types (O) Reference type array, where each entry is associated with the 
      corresponding entry in the ids array.
  */
  virtual void getReferences(OdTypedIdsArray& ids) const;

  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;

  OdResult dxfIn(OdDbDxfFiler* filer);

  virtual OdResult transformBy(const OdGeMatrix3d& xform);

  /** Description:
      Sets the properties for this entity taking into account the proxy flags.
  */
  virtual OdResult setColor(const OdCmColor &color, bool doSubents = true);
  virtual OdResult setColorIndex(OdUInt16 color, bool doSubents = true);
  virtual OdResult setColorId(OdDbObjectId, bool doSubents = true);
  virtual OdResult setPlotStyleName(const OdString& newName, bool doSubents = true);
  virtual OdResult setPlotStyleName(OdDb::PlotStyleNameType type, OdDbObjectId newId = OdDbObjectId::kNull, bool doSubents = true);
  virtual OdResult setLayer(const OdString& layerName, bool doSubents = true);
  virtual OdResult setLayer(OdDbObjectId layerId, bool doSubents = true);
  virtual OdResult setLinetype(const OdString& newVal, bool doSubents = true);
  virtual OdResult setLinetype(OdDbObjectId newVal, bool doSubents = true);
  virtual OdResult setLinetypeScale(double newval, bool doSubents = true);
  virtual OdResult setVisibility(OdDb::Visibility newVal, bool doSubents = true);
  virtual OdResult setLineWeight(OdDb::LineWeight newVal, bool doSubents = true);

  virtual OdResult subErase(bool erasing);
};

typedef OdSmartPtr<OdDbProxyEntity> OdDbProxyEntityPtr;

#include "DD_PackPop.h"

#endif

