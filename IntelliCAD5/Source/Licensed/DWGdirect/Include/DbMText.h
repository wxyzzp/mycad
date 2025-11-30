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



#ifndef OD_DBMTEXT_H
#define OD_DBMTEXT_H

#include "DD_PackPush.h"

#include "DbEntity.h"
#include "TextDefs.h"
#include "Ge/GePoint2d.h"
#include "Ge/GePoint3d.h"

class OdGeVector3d;
class OdGePoint2d;

namespace OdDb
{
  enum LineSpacingStyle
  {
    kAtLeast = 1,
    kExactly = 2
  };
}

/** Description:
    Used to return the data and attributes for one fragment of the text in an 
    MText entity.

    {group:OdDb_Classes}
*/
struct OdDbMTextFragment
{
  OdGePoint3d location;

  /* OdGeVector3d normal;
     OdGeVector3d direction;
  */

  OdString text;

  OdString font;
  OdString bigfont;

  OdGePoint2d extents;
  double capsHeight;
  double widthFactor;
  double obliqueAngle;
  double trackingFactor;
  
  // OdUInt16 colorIndex;
  
  OdCmEntityColor color;
  bool vertical;
  
  bool stackTop;
  bool stackBottom;

  bool underlined;
  bool overlined;
  OdGePoint3d underPoints[2];
  OdGePoint3d overPoints[2];

  //  true type font data
    
  OdString  fontname;
  int       charset;  
  bool      bold;
  bool      italic;
  
  int      changeStyle;  // 0 - no change 1 - change to original 2 - change to other 

  bool     lineBreak;
  bool     newParagraph;
};

/** Description:
    Used to return indentation and tab data in the OdDbMText::getParagraphsIndent
    function.

    {group:OdDb_Classes}
*/
struct OdDbMTextIndent
{
  /* Paragraph indentation value (applied to all but the first line). */
  double  paragraphInd;

  /* Indentation value applied to the first line of the paragraph. */
  double  firstLineInd;

  /* Tab settings for a pargraph. */
  OdArray<double> tabs;
};

typedef int(*OdDbMTextEnum)(OdDbMTextFragment *, void *);
typedef OdArray<OdDbMTextIndent> OdDbMTextIndents;

class OdDbText;

/** Description:
    Represents an MText (paragraph text) entity in an OdDbDatabase.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbMText : public OdDbEntity
{
public:
  ODDB_DECLARE_MEMBERS(OdDbMText);

  /** Description:
      Constructor (no arguments).
  */
  OdDbMText();

  /** Description:
      Returns the insertion point for this entity (DXF 10).
  */
  OdGePoint3d location() const;

  /** Description:
      Sets the insertion point for this entity (DXF 10).
  */
  void setLocation(const OdGePoint3d&);

  /** Description:
      Returns the normal vector for this entity (DXF 210).
  */
  OdGeVector3d normal() const;

  /** Description:
      Sets the normal vector for this entity (DXF 210).
  */
  void setNormal(const OdGeVector3d&);

  /** Description:
      Returns the X-Axis direction vector for this entity (DXF 11).  Direction and 
      rotation are equivalent, and only one or the other needs to be set when creating
      an MText entity.
  */
  OdGeVector3d direction() const;

  /** Description:
      Sets the X-Axis direction vector for this entity (DXF 11).  Direction and 
      rotation are equivalent, and only one or the other needs to be set when creating
      an MText entity.
  */
  void setDirection(const OdGeVector3d&);

  /** Description:
      Returns the rotation angle for this entity (DXF 50). Direction and 
      rotation are equivalent, and only one or the other needs to be set when creating
      an MText entity.
  */
  double rotation() const;

  /** Description:
      Sets the rotation angle for this entity (DXF 50). Direction and 
      rotation are equivalent, and only one or the other needs to be set when creating
      an MText entity.
  */
  void setRotation(double);

  /** Description:
      Returns the reference rectangle width for this entity (DXF 41).
  */
  double width() const;

  /** Description:
      Sets the reference rectangle width for this entity (DXF 41).
  */
  void setWidth(double);

  /** Description:
      Returns the Object ID of the OdDbTextStyleTableRecord used by this entity (DXF 7).
  */
  OdDbObjectId textStyle() const;

  /** Description:
      Sets the Object ID of the OdDbTextStyleTableRecord used by this entity (DXF 7).
  */
  void setTextStyle(OdDbObjectId);

  /** Description:
      Returns the text height for this entity (DXF 40).
  */
  double textHeight() const;

  /** Description:
      Sets the text height for this entity (DXF 40).
  */
  void setTextHeight(double);

  enum AttachmentPoint
  {
    kTopLeft = 1,
    kTopCenter = 2,
    kTopRight = 3,
    kMiddleLeft = 4,
    kMiddleCenter = 5,
    kMiddleRight = 6,
    kBottomLeft = 7,
    kBottomCenter = 8,
    kBottomRight = 9,
    kBaseLeft = 10,
    kBaseCenter = 11,
    kBaseRight = 12,
    kBaseAlign = 13,
    kBottomAlign = 14,
    kMiddleAlign = 15,
    kTopAlign = 16,
    kBaseFit = 17,
    kBottomFit = 18,
    kMiddleFit = 19,
    kTopFit = 20,
    kBaseMid = 21,
    kBottomMid = 22,
    kMiddleMid = 23,
    kTopMid = 24
  };

  /** Description:
      Returns the attachment point for this entity (DXF 71).
  */
  AttachmentPoint attachment() const;

  /** Description:
      Sets the attachment point for this entity (DXF 71).
  */
  void setAttachment(AttachmentPoint);

  enum FlowDirection
  {
    kLtoR = 1,
    kRtoL = 2,
    kTtoB = 3,
    kBtoT = 4,
    kByStyle = 5
  };

  /** Description:
      Returns the flow direction for this entity (DXF 72).
  */
  FlowDirection flowDirection() const;

  /** Description:
      Sets the flow direction for this entity (DXF 72).
  */
  void setFlowDirection(FlowDirection);

  /** Description:
      Returns the text contents of the entity (DXF 1 & 3).
  */
  OdString contents() const;

  /** Description:
      Sets the text contents of the entity (DXF 1 & 3).
  */
  int setContents(const char *);

  /** Description:
      Returns the width of the bounding box for this entity.
  */
  double actualWidth() const;

  /** Description:
      Returns the non-breaking space string "\~" which can be used to insert a non-breaking
      space into the contents of an MText entity.
  */
  static const char* nonBreakSpace();   

  /** Description:
      Returns the overline on string "\O" which turns overlining on in an MText string.
  */
  static const char* overlineOn();      

  /** Description:
      Returns the overline off string "\o" which turns overlining off in an MText string.
  */
  static const char* overlineOff();     

  /** Description:
      Returns the underline on string "\L" which turns underlining on in an MText string.
  */
  static const char* underlineOn();

  /** Description:
      Returns the underline off string "\l" which turns underlining off in an MText string.
  */
  static const char* underlineOff();

  /** Description:
      Returns the color change string "\C", which can be used to change the color in an MText string,
      for example "\C1;".
  */
  static const char* colorChange(); 

  /** Description:
      Returns the font change string "\F", which can be used to change the font in an MText string,
      for example "\FCourier;".
  */
  static const char* fontChange();      

  /** Description:
      Returns the height change string "\H", which can be used to change the text height in an MText string,
      for example "\H3;".
  */
  static const char* heightChange(); 

  /** Description:
      Returns the width change string "\W", which can be used to change the text width factor in an MText string,
      for example "\W2;".
  */
  static const char* widthChange();

  /** Description:
      Returns the oblique angle change string "\Q", which can be used to change the oblique angle in an MText string,
      for example "\Q15;".
  */
  static const char* obliqueChange();

  /** Description:
      Returns the track change string "\T", which can be used to adjust the space between 
      characters in an MText string, for example "\T0.85;".
  */
  static const char* trackChange();

  /** Description:
      Returns the line break string "\p", which can be used insert a line break in an MText string.
  */
  static const char* lineBreak(); 

  /** Description:
      Returns the paragraph break string "\P", which can be used insert a line break in an MText string.
  */
  static const char* paragraphBreak(); 

  /** Description:
      Returns the stacked text start string "\S", which indicates the start of stacked text in an 
      MText string.
  */
  static const char* stackStart();

  /** Description:
      Returns the alignment change string "\A", which can be used to change the alignment value for 
      an MText entity, for example "\A0;" (where 0=bottom, 1=center, 2=top).
  */
  static const char* alignChange();     

  /** Description:
      Returns begin block string "{", which signals the start of a local attribute block within
      an MText string.
  */
  static const char* blockBegin();

  /** Description:
      Returns end block string "}", which signals the end of a local attribute block within
      an MText string.
  */
  static const char* blockEnd();  

  /** Description:
      Sets the linespacing style for this MText entity (DXF 73).
  */
  void setLineSpacingStyle(OdDb::LineSpacingStyle eStyle);

  /** Description:
      Returns the linespacing style for this MText entity (DXF 73).
  */
  OdDb::LineSpacingStyle lineSpacingStyle() const;

  /** Description:
      Sets the linespacing factor for this MText entity (DXF 44).
  */
  void setLineSpacingFactor(double dFactor);

  /** Description:
      Returns the linespacing factor for this MText entity (DXF 44).
  */
  double lineSpacingFactor() const;

  /** Description:
      Gets the horizontal mode for this entity.  This will be one of:

      o kTextLeft
      o kTextCenter
      o kTextRight
  */
  OdDb::TextHorzMode horizontalMode() const;

  /** Description:
      Sets the horizontal mode for this entity.  This will be one of:

      o kTextLeft
      o kTextCenter
      o kTextRight
  */
  void setHorizontalMode(OdDb::TextHorzMode);

  /** Description:
      Gets the vertical mode for this entity.  This will be one of:

      o kTextBase
      o kTextBottom
      o kTextVertMid
  */
  OdDb::TextVertMode verticalMode() const;

  /** Description:
      Sets the vertical mode for this entity.  This will be one of:

      o kTextBase
      o kTextBottom
      o kTextVertMid
  */
  void setVerticalMode(OdDb::TextVertMode);

  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;

  bool worldDraw(OdGiWorldDraw* pWd) const;

  virtual OdResult transformBy(const OdGeMatrix3d& xform);

  virtual OdResult explode(OdRxObjectPtrArray& entitySet) const; /* Replace OdRxObjectPtrArray */

  void getClassID(void** pClsid) const;

  void subClose();
  
  /** Description:
      Explodes this MText entity into a set of simple text fragments, passing each
      fragment to the passed in function pointer.

      Arguments:
      fragmentFn (I) Function pointer that receives a call for each fragment exploded
        from this MText entity.
      userData (I) Void pointer passed to the fragmentFn callback as the second argument.
      ctxt (I) Drawing context used to explode this MText entity (if NULL, DWGdirect will
        create a temporary context for this purpose).
  */
  void explodeFragments(OdDbMTextEnum fragmentFn, 
    void *userData, 
    OdGiWorldDraw *ctxt = NULL) const;

  double actualHeight(OdGiWorldDraw *ctxt = NULL) const;
  
  /** Description:
      Return a set of points marking the text box.
  */
  void getBoundingPoints(OdGePoint3dArray&) const;

  /** Description:
      This method will implicitly setLocation based on the
      relationship of current and new attachment values in order
      to keep the extents of the MText object constant.
   */
  OdResult setAttachmentMovingLocation(AttachmentPoint);

  virtual OdResult getGeomExtents(OdGeExtents3d& extents) const;

  /** Description:
      Returns true if background fill is on, otherwise false (DXF 90, bit 0x01).
  */
  bool backgroundFillOn() const;

  /** Description:
      Turns background fill on/off (DXF 90, bit 0x01).
  */
  void setBackgroundFill(bool enable);

  /** Description:
      Returns background fill color (DXF 63 and optionally 421 & 431).
  */
  OdCmColor getBackgroundFillColor() const;

  /** Description:
      Sets background fill color (DXF 63 and optionally 421 & 431).
  */
  void setBackgroundFillColor(const OdCmColor& color);

  /** Description:
      Returns background scale factor (DXF 45).
      The scale factor controls the extra size of the filled background.
      Filled area is extended by (ScaleFactor - 1) * TextHeight in each direction.
  */
  double getBackgroundScaleFactor() const;

  /** Description:
      Sets  background scale factor (DXF 45).

      Arguments:
      scale (I) Controls extra size of filled background.

      Remarks:
      Filled area is extended by (ScaleFactor - 1) * TextHeight in each direction.
      Valid range is 1.0 - 5.0.
  */
  void setBackgroundScaleFactor(const double scale);

  /** Description:
      Returns background transparency (DXF 441).
  */
  OdCmTransparency getBackgroundTransparency() const;

  /** Description:
      Sets background transparency (DXF 441).
  */
  void setBackgroundTransparency(const OdCmTransparency& transp);

  /** Description:
      Returns true if screen background color is used for the background color of 
      this MText entity (DXF 90, bit 0x02).
  */
        bool useBackgroundColorOn() const;

  /** Description:
      Sets whether the screen background color is to be used as the background color
      for this MText entity (DXF 90, bit 0x02).

      Arguments:
      enable (I) If true, screen background color will be used for filling MText background,
      otherwise color specified by setBackgroundFillColor() will be used.
  */
  void setUseBackgroundColor(bool enable);

  /** Description:
      Returns the paragraph indentation and tab data, for all paragraphs in this 
      MText entity.  This data is extracted from control sequences from within the 
      MText string itself (no separate DXF codes).

      Arguments:
      indents (O) Receives the indentation and tab data for all paragraphs.
  */
  void getParagraphsIndent(OdDbMTextIndents& indents) const;

  OdDbObjectId setField(const OdString& propName, OdDbField* pField);
  OdResult removeField(OdDbObjectId fieldId);
  OdDbObjectId removeField(const OdString& propName);
};

typedef OdSmartPtr<OdDbMText> OdDbMTextPtr;

#include "DD_PackPop.h"

#endif


