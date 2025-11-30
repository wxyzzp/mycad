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



#ifndef _OD_BLOCK_REFERENCE_
#define _OD_BLOCK_REFERENCE_

#include "DD_PackPush.h"

#include "DbEntity.h"
#include "DbDatabase.h"
#include "Ge/GeMatrix3d.h"
#include "DbObjectIterator.h"

class OdGeScale3d;
class OdDbAttribute;
class OdDbSequenceEnd;
class OdDbBlockReferenceImpl;
typedef OdSmartPtr<OdDbAttribute> OdDbAttributePtr;
typedef OdSmartPtr<OdDbSequenceEnd> OdDbSequenceEndPtr;

/** Description:
    Represents a block reference or insertion in an OdDbDatabase.

    {group:OdDb_Classes} 
*/
class TOOLKIT_EXPORT OdDbBlockReference : public OdDbEntity
{
public:
  ODDB_DECLARE_MEMBERS(OdDbBlockReference);

  /** Description:
      Clones this object, and appends the clone to the specified owner object.  
      
      Remarks:
      Only hard pointer references are following during this cloning operation.

      Arguments:
      pOwnerObject (I) Owner object onto which the newly created clone will be appended.
      idMap (I) Current Object ID map.
      bPrimary (I) If true, this object is primary (no owner), otherwise it is owned by another object.
      bOwnerXlated (I) TBC.

      Return Value:
      A smart pointer to the newly created clone, or a null smart pointer if the cloning
      operation failed.
  */      
  OdDbObjectPtr wblockClone(OdDbIdMapping& idMap) const;

  /** Description:
      Constructor (no arguments).
  */
  OdDbBlockReference();
  
  /** Description:
      Returns the Object ID of the block associated with this OdDbBlockReference (DXF 2).
  */
  OdDbObjectId blockTableRecord() const;

  /** Description:
      Sets the Object ID of the block associated with this OdDbBlockReference (DXF 2).
  */
  virtual void setBlockTableRecord(OdDbObjectId);

  /** Description:
      Returns the position at which the associated block will be inserted,
      WCS equivalent of DXF 10.
  */
  OdGePoint3d position() const;

  /** Description:
      Sets the position at which the associated block will be inserted,
      WCS equivalent of DXF 10.
  */
  virtual void setPosition(const OdGePoint3d&);

  /** Description:
      Returns the scale factors to be applied to this block reference (DXF 41, 42, 43).
  */
  OdGeScale3d scaleFactors() const;

  /** Description:
      Sets the scale factors to be applied to this block reference (DXF 41, 42, 43).
      An OdError exception will be thrown if any of the 3 scale factors is 0.0.
      @exception OdError
  */
  virtual void setScaleFactors(const OdGeScale3d& scale);

  /** Description:
      Returns the rotation value to be applied to this block reference, in radians (DXF 50).

      Remarks:
      Rotation is around the Z axis, relative the X axis, in the coordinate system parallel to
      this object's OCS, but with the origin at this object's insertion point or position.
  */
  double rotation() const;

  /** Description:
      Sets the rotation value to be applied to this block reference, in radians (DXF 50).

      Remarks:
      Rotation is around the Z axis, relative the X axis, in the coordinate system parallel to
      this object's OCS, but with the origin at this object's insertion point or position.
  */
  virtual void setRotation(double newVal);

  /** Description:
      Returns the normal vector for this block reference (DXF 210).
  */
  OdGeVector3d normal() const;

  /** Description:
      Sets the normal vector for this block reference (DXF 210).
  */
  virtual void setNormal(const OdGeVector3d& newVal);

  virtual bool isPlanar() const { return true; }

  virtual OdResult getPlane(OdGePlane& plane, OdDb::Planarity& planarity) const;
  
  /** Description:
      Returns the transformation associated with the block reference.
      Transformation is composed of position, scale, rotation and normal.
      It does not include block's base point.
  */
  OdGeMatrix3d blockTransform() const;

  /** Description:
      Sets the transformation to be used for this block reference.
  */
  virtual OdResult setBlockTransform(const OdGeMatrix3d&);

  /** Description:
      Appends the specified OdDbAttribute to this block reference.

      Remarks:
      This block reference becomes the owner of the passed in attribute, and the attribute
      is added to the database that this block reference belongs to.  A block reference
      must be added to a database before calling this function.
      The object's attribute list should not be added by the client application.

      Arguments:
      pAttr (I) Attribute to be appended to this block reference.

      Return Value:
      The Object ID of the newly appended attribute.
  */
  OdDbObjectId appendAttribute(OdDbAttribute* pAttr);

  /** Description:
      Opens an attribute owned by this block reference.

      Arguments:
        id (I) Object ID of the attribute to be opened.
        mode (I) Mode in which to open the attribute.
        openErasedOne (I) If true, this function will open the specified attribute
                          even if it has been erased, otherwise erased attributes 
                          will not be opened.

      Return Value:
      A smart pointer to the newly opened attribute, or a null smart pointer.
  */
  OdDbAttributePtr openAttribute(OdDbObjectId id,
                                 OdDb::OpenMode mode, 
                                 bool openErasedOne = false);

  /** Description:
      Opens the SEQEND entity associated with this attribute.

      Arguments:
      mode (I) Mode in which to open the entity.

      Return Value:
      A smart pointer to the newly opened SEGEND entity.

      Remarks:
      object's attribute list--it should not be added by the client application.
  */
  OdDbSequenceEndPtr openSequenceEnd(OdDb::OpenMode mode);

  /** Description:
      Returns an iterator that can be used to traverse the attributes owned by this
      block reference.
  */
  OdDbObjectIteratorPtr attributeIterator() const;

  /** Description:
      Explodes this entity into as set of simpler entities, and adds these newly created
      entities to the specified block.

      Arguments:
      BlockRecord (I) Block table record to which the newly created entities will be added.
  */
  virtual OdResult explodeToBlock(OdDbBlockTableRecord *BlockRecord, OdDbObjectIdArray *ids = NULL);

  OdUInt32 setAttributes(OdGiDrawableTraits* pTraits) const;

  virtual bool worldDraw(OdGiWorldDraw* pWd) const;

  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;

  virtual OdResult dxfInFields_R12(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields_R12(OdDbDxfFiler* pFiler) const;

  virtual OdResult dxfIn(OdDbDxfFiler* pFiler);

  virtual void dxfOut (OdDbDxfFiler* pFiler) const;

  void subClose();

  virtual void getClassID(void** pClsid) const;
  
  virtual OdResult transformBy(const OdGeMatrix3d& xform);

  /** Description:
      Creates a copy (clone) of this entity, and applies the supplied transformation
      to the newly created copy.

      Arguments:
      xform (I) Transformation matrix to apply to the newly created copy.
      pCopy (O) Receives a pointer to the newly created copy.
  */
  virtual OdResult getTransformedCopy(const OdGeMatrix3d& xform, OdDbEntityPtr& pCopy) const;

  virtual OdResult explode(OdRxObjectPtrArray& entitySet) const; /* Replace OdRxObjectPtrArray */

  void subHandOverTo(OdDbObject* pNewObj);

  OdResult getGeomExtents(OdGeExtents3d& extents) const;

  /*
  OdDbBlockReference(const OdGePoint3d& position, OdDbObjectId blockTableRec);
  virtual bool treatAsOdDbBlockRefForExplode() const;
  void geomExtentsBestFit(OdDbExtents& extents,
                          const OdGeMatrix3d& parentXform = OdGeMatrix3d::kIdentity) const;
  */
};

typedef OdSmartPtr<OdDbBlockReference> OdDbBlockReferencePtr;

#include "DD_PackPop.h"

#endif


