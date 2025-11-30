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



#ifndef DBFCF_H
#define DBFCF_H

#include "DD_PackPush.h"

#include "DbEntity.h"
#include "DbDimStyleTableRecord.h"

/** Description:
    Represents a tolerance entity within an OdDbDatabase.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbFcf : public OdDbEntity
{
public:
  ODDB_DECLARE_MEMBERS(OdDbFcf);

  /** Description:
      Constructor (no arguments).
  */
  OdDbFcf();

  enum whichLine {kAll = -1};
  
  /** Description:
      Sets the text string for this entity (DXF 1).
  */
  virtual void setText(const char*);
  
  /** Description:
      Returns the text string for this entity (DXF 1).

      Arguments:
      lineNo (I) Currently not supported--all text is returned.
  */
  virtual const OdString text(int lineNo = kAll) const;

  /** Description:
      Sets the location for this entity in WCS (DXF 10).
  */
  virtual void setLocation(const OdGePoint3d&);

  /** Description:
      Returns the location for this entity in WCS (DXF 10).
  */
  virtual OdGePoint3d location() const;

  /** Description:
      Sets the normal and X-Axis direction vectors for this entity in WCS (DXF 210 and 11).
  */
  virtual void setOrientation(const OdGeVector3d& norm, const OdGeVector3d& dir);

  /** Description:
      Returns the normal vector for this entity in WCS (DXF 210).
  */
  virtual OdGeVector3d normal() const;

  /** Description:
      Returns the X-Axis direction vector for this entity in WCS (DXF 11).
  */
  virtual OdGeVector3d direction() const;

  /** Description:
      Sets the Object ID of the dimension style (OdDbDimStyleTableRecord) used by this entity (DXF 3).
  */
  virtual void setDimensionStyle(OdDbHardPointerId);

  /** Description:
      Returns the Object ID of the dimension style (OdDbDimStyleTableRecord) used by this entity (DXF 3).
  */
  virtual OdDbHardPointerId dimensionStyle() const;

  /** Description:
      Returns the DIMCLRD value (color used for the frame) associated with this entity.  If this entity 
      has an override for DIMCLRD, the overrride will be returned.  Otherwise, 
      the DIMCLRD of the associated dimension style will be returned.
  */
  OdCmColor dimclrd() const;

  /** Description:
      Returns the DIMCLRT value (color used for text) associated with this entity.  If this entity 
      has a override for DIMCLRT, the overrride will be returned.  Otherwise, 
      the DIMCLRT of the associated dimension style will be returned.
  */
  OdCmColor dimclrt() const;

  /** Description:
      Returns the DIMGAP value associated with this entity.  If this entity 
      has an override for DIMGAP, the overrride will be returned.  Otherwise, 
      the DIMGAP of the associated dimension style will be returned.
  */
  double dimgap() const;

  /** Description:
      Returns the DIMSCALE value (scale factor) associated with this entity.  If this entity 
      has an override for DIMSCALE, the overrride will be returned.  Otherwise, 
      the DIMSCALE of the associated dimension style will be returned.
  */
  double dimscale() const;

  /** Description:
      Returns the DIMTXSTY value (text style) associated with this entity.  If this entity 
      has an override for DIMTXSTY, the overrride will be returned.  Otherwise, 
      the DIMTXSTY of the associated dimension style will be returned.
  */
  OdDbObjectId dimtxsty() const;

  /** Description:
      Returns the DIMTXT value (text height) associated with this entity.  If this entity 
      has an override for DIMTXT, the overrride will be returned.  Otherwise, 
      the DIMTXT of the associated dimension style will be returned.
  */
  double dimtxt() const;

  /** Description:
      Sets the DIMCLRD value (color used for the frame) associated with this entity, 
      as an override.
  */
  void setDimclrd(const OdCmColor& val);

  /** Description:
      Sets the DIMCLRT value (color used for text) associated with this entity, 
      as an override.
  */
  void setDimclrt(const OdCmColor& val);

  /** Description:
      Sets the DIMGAP value associated with this entity, as an override.
  */
  void setDimgap(double val);

  /** Description:
      Sets the DIMSCALE value (scale factor) associated with this entity, 
      as an override.
  */
  void setDimscale(double val);

  /** Description:
      Sets the DIMTXSTY value (text style) associated with this entity, 
      as an override.
  */
  void setDimtxsty(OdDbObjectId val);

  /** Description:
      Sets the DIMTXT value (text height) associated with this entity, 
      as an override.
  */
  void setDimtxt(double val);

  virtual void getClassID(void** pClsid) const;

  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;

  bool worldDraw(OdGiWorldDraw* pWd) const;

  virtual OdResult transformBy(const OdGeMatrix3d& xform);

  /** Description:
       It is override for OdDbEntity::subSetDatabaseDefaults() to set the object’s dimension style 
       to the current style for the database.
  */
  void subSetDatabaseDefaults(OdDbDatabase *pDb);

  /** Description:
      This function sets the ptArray's logical length to 4 and then fills in those four points as follows:
    ptArray[0] == top left corner of Fcf
    ptArray[1] == top right corner of Fcf
    ptArray[2] == bottom right corner of Fcf
    ptArray[3] == bottom left corner of Fcf
    All points are in WCS coordinates.
  */
  virtual void getBoundingPoints(OdGePoint3dArray&) const;

  void subClose();

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

  /* This function appends to Array all the consecutive distinct corner points of the Fcf object.
  */
  virtual void getBoundingPline(OdGePoint3dArray& Array) const;
  /*
      virtual void getGripPoints(OdGePoint3dArray&, OdDbIntArray&, OdDbIntArray&) const;
      virtual void moveGripPointsAt(const OdDbIntArray&, const OdGeVector3d&);
  */
  virtual OdDbObjectPtr decomposeForSave(OdDb::DwgVersion ver, OdDbObjectId& replaceId, bool& exchangeXData);

};

typedef OdSmartPtr<OdDbFcf> OdDbFcfPtr;

#include "DD_PackPop.h"

#endif


