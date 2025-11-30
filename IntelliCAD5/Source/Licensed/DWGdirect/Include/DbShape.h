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



#ifndef _OD_DB_SHAPE_
#define _OD_DB_SHAPE_

#include "DD_PackPush.h"

#include "DbEntity.h"

/** Represents a shape entity in an OdDbDatabase.

    Remarks:
    The actual shape used by this entity is specified by a name in a dxf file,
    and by an index in a dwg file.  This requires a lookup to be done in all
    cases where the information is provided in one form, but is needed in the
    other form.  For example, calling name() on a database that was loaded from
    a dwg file will require a lookup.  The lookup procedure requires access to
    the SHX file that contains the specified shape.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbShape : public OdDbEntity
{
public:

  /*
    OdDbShape(const OdGePoint3d& position,
      double size,
      const char* name,
      double rotation = 0,
      double widthFactor = 0);
  */

  ODDB_DECLARE_MEMBERS(OdDbShape);

  /** Description:
      Constructor (no arguments).
  */
  OdDbShape();

  /** Description:
      Returns the location of this shape (DXF 10).
  */
  OdGePoint3d position() const;

  /** Description:
      Sets the location of this shape (DXF 10).
  */
  void setPosition(const OdGePoint3d&);

  /** Description:
      Returns the size of this shape (DXF 40).
  */
  double size() const;

  /** Description:
      Sets the size of this shape (DXF 40).
  */
  void setSize(double);

  /** Description:
      Returns the name of the shape used by this entity (DXF 2).
  */
  OdString name() const;

  /** Description:
      Sets the name of the shape used by this entity (DXF 2).
  */
  OdResult setName(const OdString&);

  /** Description:
      Returns the rotation value for this shape (DXF 50).
  */
  double rotation() const;

  /** Description:
      Sets the rotation value for this shape (DXF 50).
  */
  void setRotation(double);

  /** Description:
      Returns the relative X scale factor for this shape (DXF 41).
  */
  double widthFactor() const;

  /** Description:
      Sets the relative X scale factor for this shape (DXF 41).
  */
  void setWidthFactor(double);

  /** Description:
      Returns the obliquing angle for this shape (DXF 51).
  */
  double oblique() const;

  /** Description:
      Sets the obliquing angle for this shape (DXF 51).
  */
  void setOblique(double);

  /** Description:
      Returns the thickness, or depth along the normal vector, of this shape (DXF 39).
  */
  double thickness() const;

  /** Description:
      Sets the thickness, or depth along the normal vector, of this shape (DXF 39).
  */
  void setThickness(double);

  /** Description:
      Returns the normal vector for this entity (DXF 210).
  */
  OdGeVector3d normal() const;

  /** Description:
      Sets the normal vector for this entity (DXF 210).
  */
  void setNormal(const OdGeVector3d&);

  bool isPlanar() const;

  OdResult getPlane(OdGePlane&, OdDb::Planarity&) const;

  /** Description:
      Returns the shape number used by this entity.
  */
  OdInt16 shapeNumber() const;

  /** Description:
      Sets the shape number used by this entity.
  */
  void setShapeNumber(OdInt16);

  /** Description:
      Returns the object ID of the font symbol table record 
      containing this shape's SHX font. 
      A shape is stored as a reference to a font, and a character 
      within that font. Essentially a shape is a single 
      character of a special SHX font. This function returns the 
      object ID of that SHX font.
  */
  OdDbObjectId styleId() const;

  /** Description:
      Sets this AcDbShape object to use the shape specified by id.
  */
  OdResult setStyleId(OdDbObjectId);

  bool worldDraw(OdGiWorldDraw* pWd) const;

  OdResult dwgInFields(OdDbDwgFiler* pFiler);

  void dwgOutFields(OdDbDwgFiler* pFiler) const;

  OdResult dxfInFields(OdDbDxfFiler* pFiler);

  void dxfOutFields(OdDbDxfFiler* pFiler) const;

  void dxfOut(OdDbDxfFiler* pFiler) const;

  OdResult dxfInFields_R12(OdDbDxfFiler* pFiler);

  void dxfOutFields_R12(OdDbDxfFiler* pFiler) const;

  void getClassID(void** pClsid) const;

  /** Description:
      Transforms this entity by the specified matrix.
  */
  virtual OdResult transformBy(const OdGeMatrix3d& xform);
};

typedef OdSmartPtr<OdDbShape> OdDbShapePtr;

#include "DD_PackPop.h"

#endif


