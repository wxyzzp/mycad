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



#ifndef _ODDBTEXTSTYLETABLERECORD_INCLUDED
#define _ODDBTEXTSTYLETABLERECORD_INCLUDED

#include "DD_PackPush.h"

#include "DbSymbolTableRecord.h"

/** Description:
    Represents a text style in an OdDbDatabase.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbTextStyleTableRecord : public OdDbSymbolTableRecord
{
public:
  ODDB_DECLARE_MEMBERS(OdDbTextStyleTableRecord);

  /** Description:
      Constructor (no arguments).
  */
  OdDbTextStyleTableRecord();

  /** Description:
      Returns true if this text style is used for drawing shapes,
      and not for drawing text entities (DXF 70, bit 0x01).
  */
  bool isShapeFile() const;

  /** Description:
      Sets the status of this text style with respect to what it is used for: true if
      this text style is used for drawing shapes, false otherwise (DXF 70, bit 0x01).

      Arguments:
      shape (I) True if this text style is to be used for drawing shapes, otherwise false.
  */
  void setIsShapeFile(bool shape);

  /** Description:
      Returns true if text drawn with this text style is drawn as vertical text, false
      otherwise (DXF 70, bit 0x04).
  */
  bool isVertical() const;

  /** Description:
      Sets the vertical text status of this text style: true if text drawn with this
      text style should be drawn as vertical text, false otherwise (DXF 70, bit 0x04).
  */
  void setIsVertical(bool vertical);

  /** Description:
      Returns the default text size for this text style (DXF 40).
  */
  double textSize() const;

  /** Description:
      Sets the default text size for this text style (DXF 40).

      Arguments:
      size (I) Default text size.
  */
  void setTextSize(double size);

  /** Description:
      Returns the X scaling factor or width factor for this text style (DXF 41).
  */
  double xScale() const;

  /** Description:
      Sets the X scaling factor or width factor for this text style (DXF 41).
  */
  void setXScale(double xScale);

  /** Description:
      Returns the obliquing angle for this text style in radians (DXF 50).
      The obliquing angle controls the slant of the text characters
      drawn using this text style.
  */
  double obliquingAngle() const;

  /** Description:
      Sets the obliquing angle for this text style (DXF 50).  The obliquing angle
      controls the slant of the text characters drawn using this text style.

      Arguments:
      obliquingAngle (I) Obliquing angle for this text style, specified in radians.
  */
  void setObliquingAngle(double obliquingAngle);

  /** Description:
      Returns true if the backwards flag is set for this style, false otherwise (DXF 71, bit 0x02).
  */
  bool isBackwards() const;

  /** Description:
      Sets the backwards flag for this style (DXF 71, bit 0x02).
  */
  void setIsBackwards(bool value);

  /** Description:
      Returns true if the upside down flag is set for this style, false otherwise (DXF 71, bit 0x04).
  */
  bool isUpsideDown() const;

  /** Description:
      Sets the upside down flag for this style (DXF 71, bit 0x04).
  */
  void setIsUpsideDown(bool value);

  /** Description:
      Returns the size of the most recent text created with this text style (DXF 42).
  */
  double priorSize() const;

  /** Description:
      Sets the size of the most recent text created with this text style (DXF 42).
  */
  void setPriorSize(double priorSize);

  /** Description:
      Returns the name of the font file associated with this text style (DXF 3).
  */
  OdString fileName() const;

  /** Description:
      Sets the name of the font file associated with this text style (DXF 3).

      Arguments:
      fileName (O) Receives the font file name.
  */
  void setFileName(const OdString& fileName);

  /** Description:
      Returns the name of the bigfont file associated with this text style (DXF 4).
  */
  OdString bigFontFileName() const;

  /** Description:
      Sets the name of the bigfont file associated with this text style (DXF 4).
  */
  void setBigFontFileName(const OdString& fileName);

  /** Description:
      Sets the TrueType font characteristics for this text style.

      Arguments:
      pTypeface (I) Typeface name of the font.
      bold (I) True if the font is bold, false otherwise.
      italic (I) True if the font is italic, false otherwise.
      charset (I) Character set of the font.
      pitchAndFamily (I) Pitch and Family of the font.
  */
  void setFont(const OdChar* pTypeface,
		bool bold,
		bool italic,
		int charset,
		int pitchAndFamily);

  /** Description:
      Returns the TrueType font characteristics for this text style.

      Arguments:
      Typeface (O) Typeface name of the font.
      bold (O) True if the font is bold, false otherwise.
      italic (O) True if the font is italic, false otherwise.
      charset (O) Character set of the font.
      pitchAndFamily (O) Pitch and Family of the font.
  */
  void font(OdString& Typeface,
		bool& bold,
		bool& italic,
		int& charset,
		int& pitchAndFamily) const;

  virtual void getClassID(void** pClsid) const;

  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  void appendToOwner(OdDbIdPair& pair, OdDbObject* pOwner, OdDbIdMapping& idMap);

  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;

  virtual OdResult dxfInFields_R12(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields_R12(OdDbDxfFiler* pFiler) const;
  
  OdResult dxfIn(OdDbDxfFiler* pFiler);

  virtual void subClose();
  virtual OdResult subErase(bool erasing);
  virtual void subHandOverTo(OdDbObject* newObject);

  virtual OdUInt32 setAttributes(OdGiDrawableTraits* pTraits) const;
};

typedef OdSmartPtr<OdDbTextStyleTableRecord> OdDbTextStyleTableRecordPtr;

class OdGiTextStyle;
TOOLKIT_EXPORT void giFromDbTextStyle(const OdDbTextStyleTableRecord* pTStyle, OdGiTextStyle& giStyle);
TOOLKIT_EXPORT void giFromDbTextStyle(OdDbObjectId styleId, OdGiTextStyle& giStyle);

#include "DD_PackPop.h"

#endif // _ODDBTEXTSTYLETABLERECORD_INCLUDED


