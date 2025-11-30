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



#ifndef OD_DBDIM_H
#define OD_DBDIM_H

#include "DD_PackPush.h"

#include "DbEntity.h"
#include "DbMText.h"
#include "Ge/GeScale3d.h"

class OdDbDimStyleTableRecord;
typedef OdSmartPtr<OdDbDimStyleTableRecord> OdDbDimStyleTableRecordPtr;

/** Description:
    Base class for all dimension classes within an OdDbDatabase.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbDimension : public OdDbEntity
{
public:
  ODDB_DECLARE_MEMBERS(OdDbDimension);

  /** Description:
      Constructor (no arguments).
  */
  OdDbDimension();

  /** Description:
      Returns the text position of this dimension in WCS (DXF 11 as WCS).
  */
  OdGePoint3d textPosition() const;

  /** Description:
      Sets the text position of this dimension in WCS (DXF 11 as WCS).
  */
  void setTextPosition(const OdGePoint3d&);

  /** Description:
      Returns true if this dimsension is using the default position for text,
      false otherwise (DXF 70, bit 0x80).
  */
  bool isUsingDefaultTextPosition() const;

  /** Description:
      Sets this dimension to not use the default text position (DXF 70, clears bit 0x80).
  */
  void useSetTextPosition();

  /** Description:
      Sets this dimension to use the default text position (DXF 70, sets bit 0x80).
  */
  void useDefaultTextPosition();

  /** Description:
      Returns the normal for this entity in WCS (DXF 210).
  */
  OdGeVector3d normal() const;

  /** Description:
      Sets the normal for this entity in WCS (DXF 210).
  */
  void setNormal(const OdGeVector3d&);

  virtual bool isPlanar() const { return true; }

  virtual OdResult getPlane(OdGePlane& plane, OdDb::Planarity& planarity) const;

  /** Description:
      Returns the elevation value for this dimension (distance from the WCS origin to
      the plane containing the dimension).
  */
  double elevation() const;

  /** Description:
      Sets the elevation value for this dimension (distance from the WCS origin to
      the plane containing the dimension).
  */
  void setElevation(double);

  /** Description:
      Returns the annotation text for this dimension (DXF 1).
  */
  const OdString dimensionText() const;

  /** Description:
      Sets the annotation text for this dimension (DXF 1).
  */
  void setDimensionText(const OdChar*);

  /** Description:
      Returns the rotation angle for this dimension's annotation text, in radians
      (DXF 53).
  */
  double textRotation() const;

  /** Description:
      Sets the rotation angle for this dimension's annotation text, in radians
      (DXF 53).
  */
  void setTextRotation(double);

  /** Description:
      Returns the Object ID of the dimension style associated with this dimension
      (DXF 3).
  */
  OdDbObjectId dimensionStyle() const;

  /** Description:
      Sets the Object ID of the dimension style associated with this dimension
      (DXF 3).
  */
  void setDimensionStyle(OdDbObjectId);

  /** Description:
      Returns the dimension text attachment point for this dimension (DXF 71).
  */
  OdDbMText::AttachmentPoint textAttachment() const;

  /** Description:
      Sets the dimension text attachment point for this dimension (DXF 71).
  */
  void setTextAttachment(OdDbMText::AttachmentPoint eAtt);

  /** Description:
      Returns the dimension text line spacing style for this dimension (DXF 72).
  */
  OdDb::LineSpacingStyle textLineSpacingStyle() const;

  /** Description:
      Sets the dimension text line spacing style for this dimension (DXF 72).
  */
  void setTextLineSpacingStyle(OdDb::LineSpacingStyle eStyle);

  /** Description:
      Returns the dimension text line spacing factor for this dimension (DXF 41).
  */
  double textLineSpacingFactor() const;

  /** Description:
      Sets the dimension text line spacing factor for this dimension (DXF 41).
  */
  void setTextLineSpacingFactor(double dFactor);

  /** Description:
      Copies the dimension style data used by this data (including overrides),
      into the passed in dimension style table record.

      Arguments:
      pRes (O) Receives the dimension style data associated with this object.
  */
  void getDimstyleData(OdDbDimStyleTableRecord *pRes) const;

  /** Description:
      Sets the dimension style data for this object (including overrides),
      from the passed in dimension style table record.

      Arguments:
      pNewData (I) Dimstyle table record containing the dimension style data 
      to be used for this object.
  */
  void setDimstyleData(const OdDbDimStyleTableRecord* pNewData);

  /** Description:
      Sets the dimension style data for this object (including overrides),
      from the passed in dimension style table record.

      Arguments:
      newDataId (I) Object ID of the dimstyle table record containing the 
      dimension style data to be used for this object.
  */
  void setDimstyleData(OdDbObjectId newDataId);

  /** Description:
      Returns the horizontal rotation value for this dimension in radians
      (DXF 51).
  */
  double horizontalRotation() const;

  /** Description:
      Sets the horizontal rotation value for this dimension in radians
      (DXF 51).
  */
  void setHorizontalRotation(double newVal);

  /** Description:
      Returns the Object ID of the OdDbBlockTableRecord associated with this
      dimension (DXF 2).
  */
  OdDbObjectId dimBlockId() const;

  /** Description:
      Sets the Object ID of the OdDbBlockTableRecord associated with this
      dimension (DXF 2).
  */
  void setDimBlockId(const OdDbObjectId&);

  /** Description:
      Returns the relative position of the block referenced by this dimension,
      in WCS (DXF 12).
  */
  OdGePoint3d dimBlockPosition() const;

  /** Description:
      Sets the relative position of the block referenced by this dimension,
      in WCS (DXF 12). Internal use only. Dimension update sets it to 0,0,0
  */
  void setDimBlockPosition(const OdGePoint3d&);


  /** Description:
      Returns rotation angle of the block referenced by this dimension,
      (DXF 54).
  */
  double dimBlockRotation() const;

  /** Description:
      Returns scale of the block referenced by this dimension,
      (Not saved to DXF).
  */
  OdGeScale3d dimBlockScale() const;

  /** Description:
      Returns the transformation applied to dimension block.
      Transformation is composed of position, scale, rotation and normal.
      It does not include block's base point.

  */
  OdGeMatrix3d dimBlockTransform() const;
  
  /** Description:
      Creates or updates the block table record that contains the geometry for this 
      dimension entity, based on the dimension parameters.

      Arguments:
      forceUpdate  Forces the update to occur even if this dimension 
      entity has not been modified.
  */
  void recomputeDimBlock(bool forceUpdate = true);

  /** Description:
      Returns the measurement value for this dimension (DXF 42).
  */
  double measurement() const;


#define VAR_DEF(a, b, dxf, c, d, r1, r2) \
/** Returns the dim##b value of this dimension entity. */ \
virtual a dim##b() const; \
/** Sets the dim##b value of this dimension entity. */ \
virtual void setDim##b(a);
#include "DimVarDefs.h"
#undef  VAR_DEF

  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;

  virtual OdResult dxfInFields_R12(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields_R12(OdDbDxfFiler* pFiler) const;

  void getClassID(void** pClsid) const;

  OdUInt32 setAttributes(OdGiDrawableTraits* pTraits) const;

  bool worldDraw(OdGiWorldDraw* pWd) const;

  virtual OdResult explode(OdRxObjectPtrArray& entitySet) const; /* Replace OdRxObjectPtrArray */

  void subClose();

  /** Description:
      This is one of the persistent reactor notification functions that can be overridden 
      in a custom class derived from AcDbObject. 
      This particular function is called whenever the object sending notification has had 
      its erase bit toggled and is now in the process of being closed . When this function 
      is called, dbObj points to the object sending the notification. The sending object 
      is open in a read-only state. If erasing == true then the object is being 
      erased. If erasing == false, then the object is being unerased.
  */
  virtual void erased(const OdDbObject* dbObj, bool bErasing);

  /** Description:
      Notification function called immediately before an object is closed.

      Arguments:
      objId (I) Object ID of the object that is being closed.
      The default implementation of this function does nothing.  This function can be
      overridden in derived classes.
  */
  virtual void objectClosed(const OdDbObjectId& objId);

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
       Sets the object’s dimension style (dimstyle) to the current style for the database.
  */
  virtual void subSetDatabaseDefaults(OdDbDatabase* pDb);

  /*
      void formatMeasurement(OdString MTextContentBuffer, double measurement,
                             OdString dimensionText );
      void generateLayout();
      void getOsnapPoints(OdDb::OsnapMode osnapMode, const OdDbFullSubentPath& subentId,
                          const OdGePoint3d& pickPoint, const OdGePoint3d& lastPoint,
                          const OdGeMatrix3d& viewXform, OdGePoint3dArray& snapPoints,
                          OdDbIntArray& geomIds ) const;
  */
};

typedef OdSmartPtr<OdDbDimension> OdDbDimensionPtr;

#include "DD_PackPop.h"

#endif


