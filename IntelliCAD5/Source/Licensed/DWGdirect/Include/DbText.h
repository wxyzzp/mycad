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



#ifndef OD_DBTEXT_H
#define OD_DBTEXT_H

#include "DD_PackPush.h"

#include "DbEntity.h"
#include "TextDefs.h"

/** Description:
    Class that represents simple text entities in an OdDbDatabase.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbText: public OdDbEntity
{
public:
  
  ODDB_DECLARE_MEMBERS(OdDbText);
  
  /** Description:
      Constructor (no arguments).
  */
  OdDbText();
  
  /** Description:
       Returns the position for this text entity in WCS (WCS equivalent of DXF 10).
  */
  OdGePoint3d position() const;
  
  /** Description:
      Sets the position for this text entity in WCS (WCS equivalent of DXF 10).
  */
  void setPosition(const OdGePoint3d&);
  
  /** Description:
      Returns the alignment point for this text entity in WCS (WCS equivalent of DXF 11).
  */
  OdGePoint3d alignmentPoint() const;
  
  /** Description:
      Sets the alignment point for this text entity in WCS (WCS equivalent of DXF 11).
  */
  void setAlignmentPoint(const OdGePoint3d&);
  
  
  bool isDefaultAlignment() const;
  
  /** Description:
      Returns the normal vector for this text entity (DXF 210).
  */
  OdGeVector3d normal() const;
  
  /** Description:
      Sets the normal vector for this text entity (DXF 210).
  */
  void setNormal(const OdGeVector3d&);
  
  /** Remarks:
      Always returns true.
  */
  virtual bool isPlanar() const { return true; }
  
  virtual OdResult getPlane(OdGePlane&, OdDb::Planarity&) const;
  
  /** Description:
      Returns the thickness (depth along the normal vector) of this text entity (DXF 39).
  */
  double thickness() const;
  
  /** Description:
      Sets the thickness (depth along the normal vector) of this text entity (DXF 39).
  */
  void setThickness(double);
  
  /** Description:
      Return the oblique angle of this text entity, in radians (DXF 51).
  */
  double oblique() const;
  
  /** Description:
      Sets the oblique angle of this text entity, in radians (DXF 51).
      Valid range for the input angle is -1.48335 to +1.4835 radians,
      which corresponds to -85 to +85 degrees.
  */
  void setOblique(double);
  
  /** Description:
      Returns the rotation angle of this text entity, in radians (DXF 50).
  */
  double rotation() const;
  
  /** Description:
      Sets the rotation angle of this text entity, in radians (DXF 50).
  */
  void setRotation(double);
  
  /** Description:
      Returns the height of this text entity (DXF 40).
  */
  double height() const;
  
  /** Description:
      Sets the height of this text entity (DXF 40).
  */
  void setHeight(double);
  
  /** Description:
      Returns the width factor of this text entity (DXF 41).
  */
  double widthFactor() const;
  
  /** Description:
      Sets the width factor of this text entity (DXF 40).
  */
  void setWidthFactor(double);
  
  bool isMirroredInX() const;
  
  void mirrorInX(bool mirror);
  
  bool isMirroredInY() const;
  
  void mirrorInY(bool mirror);
  
  /** Description:
      Returns the text string of this text entity (DXF 1).
  */
  OdString textString() const;
  
  /** Description:
      Sets the text string of this text entity (DXF 1).
  */
  void setTextString(const OdChar*);
  
  /** Description:
      Returns the Object ID of the style (OdDbTextStyleTableRecord) referenced
      by this text entity (DXF 7).
  */
  OdDbObjectId textStyle() const;
  
  /** Description:
      Sets the Object ID of the style (OdDbTextStyleTableRecord) referenced
      by this text entity (DXF 7).
  */
  void setTextStyle(OdDbObjectId);
  
  /** Description:
      Returns the horizontal mode of this text entity (DXF 72).

      Remarks:
      Horizontal mode can be one of the following:
        - OdDb::kTextLeft
        - OdDb::kTextCenter
        - OdDb::kTextRight
        - OdDb::kTextAlign
        - OdDb::kTextMid
        - OdDb::kTextFit
  */
  OdDb::TextHorzMode horizontalMode() const;
  
  /** Description:
      Sets the horizontal mode of this text entity (DXF 72).

      Remarks:
      Horizontal mode can be one of the following:

        - OdDb::kTextLeft
        - OdDb::kTextCenter
        - OdDb::kTextRight
        - OdDb::kTextAlign
        - OdDb::kTextMid
        - OdDb::kTextFit
  */
  void setHorizontalMode(OdDb::TextHorzMode);
  
  /** Description:
      Returns the horizontal mode of this text entity (DXF 73).

      Remarks:
      Vertical mode can be one of the following:

        - OdDb::kTextBase
        - OdDb::kTextBottom
        - OdDb::kTextVertMid
        - OdDb::kTextTop
  */
  OdDb::TextVertMode verticalMode() const;
  
  /** Description:
      Sets the horizontal mode of this text entity (DXF 73).

      Remarks:
      Vertical mode can be one of the following:

        - OdDb::kTextBase
        - OdDb::kTextBottom
        - OdDb::kTextVertMid
        - OdDb::kTextTop
  */
  void setVerticalMode(OdDb::TextVertMode);
  
  int correctSpelling();
  
  virtual void getClassID(void** pClsid) const;
  
  virtual OdUInt32 setAttributes(OdGiDrawableTraits* pTraits) const;
  
  /** Description:
      Adjusts the position of this text, if the alignent is not left base.
      This function is called by DWGdirect when a newly created text entity is closed.

      Arguments:
      pDb (I) Database pointer used to resolve the text style for this
      text entity.
  */
  virtual void adjustAlignment(OdDbDatabase* pDb = NULL);
  
  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);
  
  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;
  
  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);
  
  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;
  
  virtual OdResult dxfInFields_R12(OdDbDxfFiler* pFiler);
  
  virtual void dxfOutFields_R12(OdDbDxfFiler* pFiler) const;
  
  void subClose();
  
  virtual bool worldDraw(OdGiWorldDraw*) const;
  
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
       Return points marking text box
  */
  void getBoundingPoints(OdGePoint3dArray&) const;

  /* Constructor. */
  /*OdDbText(const OdGePoint3d& position,
      const char* text,
      OdDbObjectId style = OdDbObjectId::kNull,
      double height = 0,
      double rotation = 0);
  */

  OdDbObjectId setField(const OdString& propName, OdDbField* pField);
  OdResult removeField(OdDbObjectId fieldId);
  OdDbObjectId removeField(const OdString& propName);
};

typedef OdSmartPtr<OdDbText> OdDbTextPtr;

#include "DD_PackPop.h"

#endif // ODDBTEXT_H


