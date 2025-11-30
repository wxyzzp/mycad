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



#ifndef OD_DBMLINESTYLE_H
#define OD_DBMLINESTYLE_H

#include "DD_PackPush.h"

#define MSTYLE_NAME_LENGTH 32
#define MSTYLE_DESCRIPTION_LENGTH 256
#define LTYPE_LENGTH 32
#define MLINE_MAX_ELEMENTS 16

// 10 degrees
//
#define MIN_ANGLE (10.0 * PI / 180.0)

// 170 degrees
//
#define MAX_ANGLE (170.0 * PI / 180.0)

#define MSTYLE_DXF_FILL_ON (0x1)
#define MSTYLE_DXF_SHOW_MITERS (0x2)
#define MSTYLE_DXF_START_SQUARE_CAP (0x10)
#define MSTYLE_DXF_START_INNER_ARCS (0x20)
#define MSTYLE_DXF_START_ROUND_CAP (0x40)
#define MSTYLE_DXF_END_SQUARE_CAP (0x100)
#define MSTYLE_DXF_END_INNER_ARCS (0x200)
#define MSTYLE_DXF_END_ROUND_CAP (0x400)
#define MSTYLE_DXF_JUST_TOP (0x1000)
#define MSTYLE_DXF_JUST_ZERO (0x2000)
#define MSTYLE_DXF_JUST_BOT (0x4000)

#include "DbObject.h"

class OdDbMlineStyleImpl;

/** Description:
    Represents an MLine Style in an OdDbDatabase.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbMlineStyle: public OdDbObject
{
public:
  ODDB_DECLARE_MEMBERS(OdDbMlineStyle);

  /** Description:
      Constructor (no arguments).
  */
  OdDbMlineStyle();

  /** Description:
      Initializes this MlineStyle object.

      Remarks:
      Initialization is done as follows:

        Name - Empty string.
        Description - Empty string.
        FillColor - 0.
        StartAngle - PI / 2.0.
        EndAngle - PI / 2.0.
        ElementList - Empty.
  */
  void initMlineStyle();

  /** Description:
      Copies the information from the passed in OdDbMlineStyle object into this object.
  */
  void set(const OdDbMlineStyle & src, bool checkIfReferenced = true);

  /** Description:
      Sets the description for the object (DXF 3).
  */
  void setDescription(const OdChar* description);

  /** Description:
      Returns the description for the object (DXF 3).
  */
  const OdString description() const;

  /** Description:
      Sets the name for the object (DXF 2).
  */
  void setName(const OdChar * name);

  /** Description:
      Returns the name for the object (DXF 2).
  */
  const OdString name() const;

  /** Description:
      Sets the "Display Miters" flag for this object (DXF 70, bit 0x02).
  */
  void setShowMiters(bool showThem);

  /** Description:
      Returns the "Display Miters" flag for this object (DXF 70, bit 0x02).
  */
  bool showMiters() const;

  /** Description:
      Sets the "Start Square Cap" flag for this object (DXF 70, bit 0x10).
  */
  void setStartSquareCap(bool showIt);

  /** Description:
      Returns the "Start Square Cap" flag for this object (DXF 70, bit 0x10).
  */
  bool startSquareCap() const;

  /** Description:
      Sets the "Start Round Cap" flag for this object (DXF 70, bit 0x40).
  */
  void setStartRoundCap(bool showIt);

  /** Description:
      Returns the "Start Round Cap" flag for this object (DXF 70, bit 0x40).
  */
  bool startRoundCap() const;

  /** Description:
      Sets the "Start Inner Arcs" flag for this object (DXF 70, bit 0x20).
  */
  void setStartInnerArcs(bool showThem);

  /** Description:
      Returns the "Start Inner Arcs" flag for this object (DXF 70, bit 0x20).
  */
  bool startInnerArcs() const;

  /** Description:
      Sets the "End Square Cap " flag for this object (DXF 70, bit 0x80).
  */
  void setEndSquareCap(bool showIt);

  /** Description:
      Returns the "End Square Cap " flag for this object (DXF 70, bit 0x80).
  */
  bool endSquareCap() const;

  /** Description:
      Sets the "End Round Cap " flag for this object (DXF 70, bit 0x200).
  */
  void setEndRoundCap(bool showIt);

  /** Description:
      Returns the "End Round Cap " flag for this object (DXF 70, bit 0x200).
  */
  bool endRoundCap() const;

  /** Description:
      Sets the "End Inner Arcs" flag for this object (DXF 70, bit 0x100).
  */
  void setEndInnerArcs(bool showThem);

  /** Description:
      Returns the "End Inner Arcs" flag for this object (DXF 70, bit 0x100).
  */
  bool endInnerArcs() const;

  /** Description:
      Sets the fill color for this style (DXF 62).
  */
  void setFillColor(const OdCmColor& color);

  /** Description:
      Returns the fill color for this style (DXF 62).
  */
  OdCmColor fillColor() const;

  /** Description:
      Sets the fill status for this style (DXF 70, bit 0x01).
  */
  void setFilled(bool onOff);

  /** Description:
      Returns the fill status for this style (DXF 70, bit 0x01).
  */
  bool filled() const;

  /** Description:
      Sets the start angle for this style (DXF 51).
  */
  void setStartAngle(double ang);
	
  /** Description:
      Returns the start angle for this style (DXF 51).
  */
  double startAngle() const;

  /** Description:
      Sets the end angle for this style (DXF 52).
  */
  void setEndAngle(double ang);

  /** Description:
      Returns the end angle for this style (DXF 52).
  */
  double endAngle() const;

  /** Description:
      Adds an element to this MLine Style.

      Arguments:
      offset (I) Offset for this element.
      color (I) Color for this element.
      linetypeId (I) Object ID for OdDbLinetypeTableRecord object associated with 
      this element.
      checkIfReferenced (I) Currently not used.

      Return Value:
      The index of the newly added element.
  */
  int addElement(double offset, 
                 const OdCmColor& color,
                 OdDbObjectId linetypeId, 
                 bool checkIfReferenced = true);

  /** Description:
      Removes the element at the specified index.
  */
  void removeElementAt(int elem);

  /** Description:
      Returns the number of elements in this MLine Style (DXF 71).
  */
  int numElements() const;

  /** Description:
      Sets the values for an existing element in this MLine Style.

      Arguments:
      elem (I) Index of element to modify.
      offset (I) Offset for this element.
      color (I) Color for this element.
      linetypeId (I) Object ID for OdDbLinetypeTableRecord object associated with 
      this element.
  */
  void setElement(int elem, 
                  double offset, 
                  const OdCmColor& color,
                  OdDbObjectId linetypeId);

  /** Description:
      Returns the values for an existing element in this MLine Style.

      Arguments:
      elem (I) Index of element to modify.
      offset (O) Receives the offset for this element.
      color (O) Receives the color for this element.
      linetypeId (O) Receives the Object ID for the OdDbLinetypeTableRecord object associated with 
      this element.
  */
  void getElementAt(int elem, double& offset, OdCmColor& color,
                    OdDbObjectId& linetypeId) const;

  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;

  void  getClassID(void** pClsid) const;
};

typedef OdSmartPtr<OdDbMlineStyle> OdDbMlineStylePtr;

#include "DD_PackPop.h"

#endif


