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



#ifndef _ODDBENTITY_INCLUDED_
#define _ODDBENTITY_INCLUDED_

#include "DD_PackPush.h"

#include "DbObject.h"
#include "Ge/GePoint3d.h"
#include "Ge/GeLine3d.h"
#include "Ge/GePlane.h"
#include "CmColor.h"

class OdGePlane;
class OdGeMatrix3d;
class OdDbFullSubentPath;
class OdGePoint3d;
class OdGeVector3d;
class OdDbBlockTableRecord;

typedef OdSmartPtr<OdDbEntity> OdDbEntityPtr;
typedef OdArray<OdDbEntityPtr> OdDbEntityPtrArray;

namespace OdDb
{
  enum SubentType
  {
    kNullSubentType   = 0,
    kFaceSubentType   = 1,
    kEdgeSubentType   = 2,
    kVertexSubentType = 3,
    kMlineSubentCache = 4
  };

  enum Intersect
  {
    kOnBothOperands    = 0,
    kExtendThis        = 1,
    kExtendArg         = 2,
    kExtendBoth        = 3
  };

  enum EntSaveAsType
  {
    kNoSave = 0,
    kSaveAsR12,
    kSaveAsR13,
    kSaveAsR14
  };

  enum OsnapMode
  {
    kOsModeEnd      = 1,
    kOsModeMid      = 2,
    kOsModeCen      = 3,
    kOsModeNode     = 4,
    kOsModeQuad     = 5,
    kOsModeIntersec = 6,
    kOsModeIns      = 7,
    kOsModePerp     = 8,
    kOsModeTan      = 9,
    kOsModeNear     = 10,

    kOsModeApint    = 11,
    kOsModePar      = 12,
    kOsModeStart    = 13
  };

  enum Planarity
  {
    kNonPlanar = 0,
    kPlanar    = 1,
    kLinear    = 2
  };
}


/** Description:
    Base class for database objects that contain a graphical representation. 

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbEntity : public OdDbObject
{
public:
  ODDB_DECLARE_MEMBERS(OdDbEntity);

  /** Description:
      Constructor (no arguments).
  */
  OdDbEntity();

  /** Description:
      Returns the Object ID of this entity's owner block (OdDbBlockTableRecord).
  */
  OdDbObjectId blockId() const;

  /** Description:
      Returns the color of this entity as an OdCmColor (DXF 62).
  */
  OdCmColor color() const;

  /** Description:
      Sets the color value for this entity (DXF 62).

      Arguments:
      color (I) Color value to set.
      doSubents (I) If true, apply the color change to all sub-entities owned by
      this entity, otherwise do not apply the color to sub-entities.
  */
  virtual OdResult setColor(const OdCmColor &color, bool doSubents = true);

  /** Description:
      Returns the color index of this entity (DXF 62).
  */
  OdUInt16 colorIndex() const;

  virtual OdCmEntityColor entityColor() const;

  /** Description:
      Sets the color index for this entity (DXF 62).

      Arguments:
      color (I) Color index to set.
      doSubents (I) If true, apply the color change to all sub-entities owned by
      this entity, otherwise do not apply the color to sub-entities.
  */
  virtual OdResult setColorIndex(OdUInt16 color, bool doSubents = true);

  /** Description:
      Returns the Id of OdDbColor object.
  */
  OdDbObjectId colorId() const;

  /** Description:
      Sets the OdDbColor Id for this entity.

      Arguments:
      colorId (I) OdDbColor object Id to set.
      doSubents (I) If true, apply the color change to all sub-entities owned by
      this entity, otherwise do not apply the color to sub-entities.
  */
  virtual OdResult setColorId(OdDbObjectId, bool doSubents = true);

  OdCmTransparency transparency() const;
  
  virtual OdResult setTransparency(const OdCmTransparency& trans, bool doSubents = true);

  /** Description:
      Returns the name of the plot style associated with this entity (DXF 390).
  */
  OdString plotStyleName() const;

  /** Description:
      Returns the Object ID of the plot style associated with this entity (DXF 390).
  */
  OdDb::PlotStyleNameType getPlotStyleNameId(OdDbObjectId& id) const;

  /** Description:
      Sets the plot style for this entity.

      Arguments:
      newName (I) Name of the plot style.
      doSubents (I) If true, apply the plot style change to all sub-entities owned by
      this entity, otherwise do not apply the change to sub-entities.
  */
  virtual OdResult setPlotStyleName(const OdString& newName,
    bool doSubents = true);

  /** Description:
      Sets the plot style for this entity.

      Arguments:
      type (I) Plot style type, on of:
       -kPlotStyleNameByLayer
       -kPlotStyleNameByBlock
       -kPlotStyleNameIsDictDefault
       -kPlotStyleNameById
      newId (I) Object ID of the plot style, used only for type kPlotStyleNameById.
      doSubents (I) If true, apply the plot style change to all sub-entities owned by
      this entity, otherwise do not apply the change to sub-entities.
  */
  virtual OdResult setPlotStyleName(OdDb::PlotStyleNameType type,
    OdDbObjectId newId = OdDbObjectId::kNull, bool doSubents = true);

  /** Description:
      Returns the name of the layer referenced by this entity (DXF 8).
  */
  OdString layer() const;

  /** Description:
      Returns the Object ID of the layer referenced by this entity (DXF 8).
  */
  OdDbObjectId layerId() const;

  /** Description:
      Sets the layer to be referenced by this entity (DXF 8).

      Arguments:
      layerName (I) Name of the layer.
      doSubents (I) If true, apply the layer change to all sub-entities owned by
      this entity, otherwise do not apply the change to sub-entities.  This flag
      only affects entities that contain sub-entities (OdDb2dPolyline, OdDb3dPolyline, 
      OdDbPolyFaceMesh, OdDbPolygonMesh, etc.).

  */
  virtual OdResult setLayer(const OdString& layerName, bool doSubents = true);

  /** 
      Arguments:
      layerId (I) Object ID of the layer.
  */
  virtual OdResult setLayer(OdDbObjectId layerId, bool doSubents = true);


  /** Description:
      Returns the name of the linetype referenced by this entity (DXF 6).
  */
  OdString linetype() const;

  /** Description:
      Returns the Object ID of the linetype referenced by this entity (DXF 6).
  */
  OdDbObjectId linetypeId() const;

  /** Description:
      Sets the linetype to be referenced by this entity (DXF 6).

      Arguments:
      newVal (I) Name of the linetype.
      doSubents (I) If true, apply the linetype change to all sub-entities owned by
      this entity, otherwise do not apply the change to sub-entities.
  */
  virtual OdResult setLinetype(const OdString& newVal, bool doSubents = true);

  /** Description:
      Sets the linetype to be referenced by this entity (DXF 6).

      Arguments:
      newVal (I) Object ID of the linetype.
      doSubents (I) If true, apply the linetype change to all sub-entities owned by
      this entity, otherwise do not apply the change to sub-entities.
  */
  virtual OdResult setLinetype(OdDbObjectId newVal, bool doSubents = true);

  /** Description:
      Returns the linetype scale for this entity (DXF 48).
  */
  double linetypeScale() const;

  /** Description:
      Sets the linetype scale for this entity (DXF 48).

      Arguments:
      newval (I) New linetype scale value.
      doSubents (I) If true, apply the linetype scale change to all sub-entities owned by
      this entity, otherwise do not apply the change to sub-entities.
  */
  virtual OdResult setLinetypeScale(double newval, bool doSubents = true);

  /** Description:
      Returns the visibility status of this entity (DXF 60).
      Visibility can be either OdDb:kInvisible or OdDb:kVisible.
  */
  OdDb::Visibility visibility() const;

  /** Description:
      Sets the visibility status of this entity (DXF 60).
      Visibility can be either OdDb:kInvisible or OdDb:kVisible.

      Arguments:
      newVal (I) Visibility status to set.
      doSubents (I) If true, apply the visibility change to all sub-entities owned by
      this entity, otherwise do not apply the change to sub-entities.
  */
  virtual OdResult setVisibility(OdDb::Visibility newVal, bool doSubents = true);

  /** Description:
      Returns the line weight property for this entity (DXF 370).
  */
  OdDb::LineWeight lineWeight() const;

  /** Description:
      Sets the line weight property for this entity (DXF 370).

      Arguments:
      newVal (I) Line weight value to set.
      doSubents (I) If true, apply the line weight change to all sub-entities owned by
      this entity, otherwise do not apply the change to sub-entities.
  */
  virtual OdResult setLineWeight(OdDb::LineWeight newVal, bool doSubents = true);

  /** Description:
      Copies the properties from the specified entity, to this entity.
  */
  void setPropertiesFrom(const OdDbEntity* pEntity, bool doSubents = true);

  /** Description:
      Returns true if this entity is planar, false otherwise.
  */
  virtual bool isPlanar() const;

  /** Description:
      Returns the plane that contains this entity, if applicable.

      Arguments:
      plane (O) Receives the plane that contains this entity, if applicable.
      planarity (O) Receives the planarity setting for this entity.

      Remarks:
      The planarity argument may be one of:

        o OdDb::kNonPlanar - Indicates that this entity is not planar, plane is not set.
        o OdDb::kPlanar - Indicates that this entity is planar, and the containing plane is returned.
        o OdDb::kLinear - Indicates that this entity is linear, and an arbitrary plane containing this entity is returned.

      Return Value:
      eOk if successful, otherwise an appropriate error code.
  */
  virtual OdResult getPlane(OdGePlane& plane, OdDb::Planarity& planarity) const;

  virtual OdResult getGeomExtents(OdGeExtents3d& extents) const;

  /** Description:
      Called as the first operation of the handOverTo function.  This function allows derived
      classes to populate the new object.

      See Also:
      OdDbObject::handOverTo
  */
  void subHandOverTo(OdDbObject* newObject);

  /** Description:
      Transforms this entity by the specified matrix.
  */
  virtual OdResult transformBy(const OdGeMatrix3d& xform);

  /** Description:
      Creates a copy (clone) of this entity, and applies the supplied transformation
      to the newly created copy.

      Arguments:
      xform (I) Transformation matrix to apply to the newly created copy.
      pCopy (O) Receives a pointer to the newly created copy.
  */
  virtual OdResult getTransformedCopy(const OdGeMatrix3d& xform, OdDbEntityPtr& pCopy) const;

  /** Description:
      Explodes this entity into as set of simpler entities, which are returned to the
      caller.  The newly created entities are not database resident.

      Arguments:
      entitySet (O) Receives the set of exploded entities.
  */
  virtual OdResult explode(OdRxObjectPtrArray& entitySet) const; /* Replace OdRxObjectPtrArray */

  /** Description:
      Explodes this entity into as set of simpler entities, and adds these newly created
      entities to the specified block.

      Arguments:
      BlockRecord (I) Block table record to which the newly created entities will be added.
  */
  virtual OdResult explodeToBlock(OdDbBlockTableRecord *BlockRecord, OdDbObjectIdArray *ids = NULL);

  virtual bool worldDraw(OdGiWorldDraw* pWd) const;

  virtual void viewportDraw(OdGiViewportDraw* mode) const;

  /** Description:
      Sets the default properties for this entity, from the specified database.  The layer, linetype,
      color, linetype scale, lineweight, plotstyle, and visibility are set.
  */
  void setDatabaseDefaults(OdDbDatabase* pDb = NULL);

  /** Description:
      setDatabaseDefaults() will call this method after the values are set
      so that a client may inspect the values and make any needed modifications.
  */
  virtual void subSetDatabaseDefaults(OdDbDatabase* pDb = NULL);

  virtual void applyPartialUndo(OdDbDwgFiler* undoFiler, OdRxClass* classObj);

  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  void appendToOwner(OdDbIdPair& idPair, OdDbObject* pOwner, OdDbIdMapping& idMap);

  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;

  virtual OdResult dxfInFields_R12(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields_R12(OdDbDxfFiler* pFiler) const;

  /** Remarks:
      Sets the attributes for an entity, including:

        o Layer
        o Color
        o LineType
        o LineWeight
        o LineTypeScale
        o Thickness

      Any overrides of this function in a derived class should call the parent class version,
      and should return the flags value returned by the parent, 
      along with any additional flags required by the client implementation.
  */
  virtual OdUInt32 setAttributes(OdGiDrawableTraits* pTraits) const;

  virtual void getClassID(void** pClsid) const;

  /** Description:
      Called as the first operation of the swapIdWith function.  This function allows derived classes
      to implement custom behavior during the swapIdWith operation.

      Arguments:
      otherId (I) Object ID to be swapped with this object's Object ID.
      swapXdata (I) If true, then extended data is swapped.
      swapExtDict (I) If true, then extension dictionaries are swapped.

      See Also:
      OdDbObject::swapIdWith
  */
  void subSwapIdWith(const OdDbObjectId& otherId, bool swapXdata = false, bool swapExtDict = false);

  void xmitPropagateModify() const;

  /** Description:
      Override this method to monitor this entities erasure.  This method will be
      called when the erase() method is called for this object. Returning anything except eOk
      prevents object from being erased
  */
  virtual OdResult subErase(bool bErasing);

  /** Description:
      This function is called by the DWGdirect framework immediately before an object is opened.
      Overriding this function in a child class allows a child instance to be notified each time an
      object is opened.
  */
  virtual void subOpen(OdDb::OpenMode mode);

  /** Description:
      Sets the bit flag indicating if entity's geometry was changed.
      It effects if some actions (like recomputing dimension block for dimension entity)
      will take place upon close.
  */
  void recordGraphicsModified(bool bSetModified = true);

  /** Description:
      Copies the contents of the specified object, into this object when possible.
  */
  virtual void copyFrom(const OdRxObject* p);

  virtual OdDbObjectPtr wblockClone(OdDbIdMapping& idMap) const;

/* No implementation
  Intersects this entity with pEnt if possible.  inType determines type of
      intersection to be calculated
    virtual void intersectWith(const OdDbEntity* pEnt,
    OdDb::Intersect intType, OdGePoint3dArray& points,
    int thisGsMarker = 0, int otherGsMarker = 0) const;

  virtual void intersectWith(const OdDbEntity* pEnt, OdDb::Intersect intType,
    const OdGePlane& projPlane, OdGePoint3dArray& points,
    int thisGsMarker = 0, int otherGsMarker = 0) const;

  This method is triggered by the standard LIST command and is to be display
      the dxf format contents of the entity to the display.
  virtual void list() const;

  Cause this entity, and any other entity who's draw bit is set, to be be drawn.
  void draw();

  Uses the bounding box of this object to determine an intersection
      array of points.
  void boundingBoxIntersectWith(const OdDbEntity* pEnt,
    OdDb::Intersect intType, OdGePoint3dArray& points,
    int thisGsMarker, int otherGsMarker) const;

  void boundingBoxIntersectWith(const OdDbEntity* pEnt, OdDb::Intersect intType,
    const OdGePlane& projPlane, OdGePoint3dArray& points,
    int thisGsMarker, int otherGsMarker) const;


  */

  /*
  virtual void getCompoundObjectTransform(OdGeMatrix3d & xMat) const;
  virtual void getSubentPathsAtGsMarker(OdDb::SubentType type,
  int gsMark, const OdGePoint3d& pickPoint,
  const OdGeMatrix3d& viewXform, int& numPaths,
  OdDbFullSubentPath** subentPaths, int numInserts = 0,
  OdDbObjectId* entAndInsertStack = NULL) const;

  virtual void getGsMarkersAtSubentPath(const OdDbFullSubentPath& subPath, OdDbIntArray& gsMarkers) const;

  virtual OdDbEntity* subentPtr(const OdDbFullSubentPath& id) const;

  virtual void getOsnapPoints(OdDb::OsnapMode osnapMode,
    int gsSelectionMark, const OdGePoint3d& pickPoint,
    const OdGePoint3d& lastPoint, const OdGeMatrix3d& viewXform,
    OdGePoint3dArray& snapPoints, OdDbIntArray& geomIds) const;

  virtual void highlight(const OdDbFullSubentPath& subId = OdDb::kNullSubent,
    const bool highlightAll = false) const;

  virtual void unhighlight(const OdDbFullSubentPath& subId = kNullSubent,
    const bool highlightAll = false) const;

  virtual void getGripPoints(OdGePoint3dArray& gripPoints,
    OdDbIntArray& osnapModes, OdDbIntArray& geomIds) const;

  virtual void moveGripPointsAt(const OdDbIntArray& indices, const OdGeVector3d& offset);

  virtual void getStretchPoints(OdGePoint3dArray& stretchPoints) const;
  virtual void moveStretchPointsAt(const OdDbIntArray& indices, const OdGeVector3d& offset);
  virtual void audit(OdDbAuditInfo* pAuditInfo);
  virtual OdGiDrawable* drawable();

  virtual void setGsNode(OdGsNode* pNode);
  virtual OdGsNode* gsNode() const;

  virtual bool cloneMeForDragging();

  virtual void saveAs(OdGiWorldDraw* mode, OdDb::EntSaveAsType st);

  */
};

#include "DD_PackPop.h"

#endif /* _ODDBENTITY_INCLUDED_ */


