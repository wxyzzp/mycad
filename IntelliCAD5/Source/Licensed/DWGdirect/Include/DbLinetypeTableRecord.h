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



#ifndef _ODDBLINETYPETABLERECORD_INCLUDED
#define _ODDBLINETYPETABLERECORD_INCLUDED

#include "DD_PackPush.h"

#include "DbSymbolTableRecord.h"

class OdGeVector2d;

/** Description:
    Represents a linetype in the OdDbDatabase.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbLinetypeTableRecord : public OdDbSymbolTableRecord
{
public:
  ODDB_DECLARE_MEMBERS(OdDbLinetypeTableRecord);

  /** Description:
      Constructor (no arguments).
  */
  OdDbLinetypeTableRecord();

  /** Description:
      Returns a string containing the simple ASCII representation of this linetype (DXF 3).
  */
  const OdString comments() const;

  /** Description:
      Sets a string containing the simple ASCII representation for this linetype (DXF 3).
  */
  void setComments(const OdString& pString);

  /** Description:
      Returns the overall length of this linetype pattern (DXF 40).
  */
  double patternLength() const;

  /** Description:
      Sets the overall length for this linetype pattern (DXF 40).
  */
  void setPatternLength(double patternLength);

  /** Description:
      Returns the number of dashes of this linetype (DXF 73).
  */
  int numDashes() const;

  /** Description:
      Sets the number of dashes for this linetype (DXF 73).
      Valid number is zero or between 2 and 12
  */
  void setNumDashes(int count);

  /** Description:
      Returns the length of the dash at a specified index (DXF 49).  
      
      Remarks:
      The index argument must be greater than or equal to 0, and less than the value 
      returned by OdDbLinetypeTableRecord::numDashes().

      Arguments:
      index (I) Dash index.

      Return Value:
      Dash length at index.
  */
  double dashLengthAt(int index) const;

  /** Description:
      Sets the length of the dash at a specified index (DXF 49).  
      
      Remarks:
      The index argument must be greater than or equal to 0, and less than the value 
      returned by OdDbLinetypeTableRecord::numDashes().

      Arguments:
      index (I) Dash index.
      value (I) Dash length.
  */
  void setDashLengthAt(int index , double value);

  /** Description:
      Returns the Object ID of the OdDbTextStyleTableRecord used by the linetype for
      the shape at the specified index (DXF 340).  
      
      Remarks:
      The index argument must be greater than or equal to 0, and less than the value 
      returned by OdDbLinetypeTableRecord::numDashes().

      Arguments:
      index (I) Dash index.

      Return Value:
      Object Id of the Text Style used at the specified index.
  */
  OdDbObjectId shapeStyleAt(int index) const;

  /** Description:
      Sets the Object ID of the OdDbTextStyleTableRecord used by the linetype for
      the shape at the specified index (DXF 340).  
      
      Remarks:
      The index argument must be greater than or equal to 0, and less than the value 
      returned by OdDbLinetypeTableRecord::numDashes().

      Arguments:
      index (I) Dash index.
      id (I) Object Id of the Text Style to be used at the specified index.
  */
  void setShapeStyleAt(int index, OdDbObjectId id);

  /** Description:
      Returns the shape number used by the linetype for the shape at the specified
      index (DXF 75).  
      
      Remarks:
      The index argument must be greater than or equal to 0,
      and less than the value returned by OdDbLinetypeTableRecord::numDashes().

      Arguments:
      index (I) Dash index.

      Return Value:
      Shape number used at the specified index.
  */
  OdUInt16 shapeNumberAt(int index) const;

  /** Description:
      Sets the shape number used by the linetype for the shape at the specified
      index (DXF 75).  
      
      Remarks:
      The index argument must be greater than or equal to 0,
      and less than the value returned by OdDbLinetypeTableRecord::numDashes().

      Arguments:
      index (I) Dash index.
      shapeNumber (I) Shape number to be used at the specified index.
  */
  void setShapeNumberAt(int index, OdUInt16 shapeNumber);

  /** Description:
      Returns the shape offset used by the linetype for the shape at the specified
      index (DXF 44, 45).  
      
      Remarks:
      The index argument must be greater than or equal to 0,
      and less than the value returned by OdDbLinetypeTableRecord::numDashes().

      Arguments:
      index (I) Dash index.

      Return Value:
      Shape offsets used at the specified index.

      See Also:
      OdGeVector2d
  */
  OdGeVector2d shapeOffsetAt(int index) const;

  /** Description:
      Sets the shape offset used by the linetype for the shape at the specified
      index (DXF 44, 45).  
      
      Remarks:
      The index argument must be greater than or equal to 0,
      and less than the value returned by OdDbLinetypeTableRecord::numDashes().

      Arguments:
      index (I) Dash index.
      offset (I) Offsets used at the specified index.

      See Also:
      OdGeVector2d
  */
  void setShapeOffsetAt(int index, const OdGeVector2d& offset);

  /** Description:
      Returns the shape scale used by the linetype for the shape at the specified
      index (DXF 46).  
      
      Remarks:
      The index argument must be greater than or equal to 0,
      and less than the value returned by OdDbLinetypeTableRecord::numDashes().

      Arguments:
      index (I) Dash index.

      Return Value:
      Shape scale used at the specified index.
  */
  double shapeScaleAt(int index) const;

  /** Description:
      Sets the shape scale used by the linetype for the shape at the specified
      index (DXF 46).  
      
      Remarks:
      The index argument must be greater than or equal to 0,
      and less than the value returned by OdDbLinetypeTableRecord::numDashes().

      Arguments:
      index (I) Dash index.
      scale (I) Shape scale used at the specified index.
  */
  void setShapeScaleAt(int index, double scale);

  /** Description:
      Returns the alignment type for this linetype (DXF 72).  A value of true indicates that
      the 72 code contains the value of 'S', otherwise it contains 'A'.
  */
  bool isScaledToFit() const;

  /** Description:
      Sets the alignment type for this linetype (DXF 72).  A value of true indicates that
      the 72 code contains the value of 'S', otherwise it contains 'A'.
  */
  void setIsScaledToFit(bool scaledToFit);

  /** Description:
      Returns the "shape is UCS oriented flag" for the shape at the specified index (DXF 74, bit 0x01).
  */
  bool shapeIsUcsOrientedAt(int index) const;

  /** Description:
      Sets the "shape is UCS oriented flag" for the shape at the specified index (DXF 74, bit 0x01).
  */
  void setShapeIsUcsOrientedAt(int index, bool isUcsOriented);

  /** Description:
      Returns the rotation angle used by the linetype for the shape or text at the specified
      index (DXF 50).  
      
      Remarks:
      The index argument must be greater than or equal to 0,
      and less than the value returned by OdDbLinetypeTableRecord::numDashes().

      Arguments:
      index (I) Dash index.

      Return Value:
      Shape rotation angle (in radians) used at the specified index.
  */
  double shapeRotationAt(int index) const;

  /** Description:
      Sets the rotation angle used by the linetype for the shape or text at the specified
      index (DXF 50).  
      
      Remarks:
      The index argument must be greater than or equal to 0,
      and less than the value returned by OdDbLinetypeTableRecord::numDashes().

      Arguments:
      index (I) Dash index.
      rotation (I) Shape rotation angle (in radians) to be used at the specified index.
  */
  void setShapeRotationAt(int index, double rotation);

  /** Description:
      Returns the text string used by the linetype at the specified
      index (DXF 9).  
      
      Remarks:
      The index argument must be greater than or equal to 0,
      and less than the value returned by OdDbLinetypeTableRecord::numDashes().

      Arguments:
      index (I) Dash index.

      Return Value:
      Text string used at the specified index.
  */
  OdString textAt(int index) const;

  /** Description:
      Sets the text string used by the linetype at the specified
      index (DXF 9).  
      
      Remarks:
      The index argument must be greater than or equal to 0,
      and less than the value returned by OdDbLinetypeTableRecord::numDashes().

      Arguments:
      index (I) Dash index.
      text (I) Text string to be used at the specified index.
  */
  void setTextAt(int index, const OdString& text);

  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  void appendToOwner(OdDbIdPair& pair, OdDbObject* pOwner, OdDbIdMapping& idMap);

  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;

  virtual OdResult dxfInFields_R12(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields_R12(OdDbDxfFiler* pFiler) const;

  virtual void getClassID(void** pClsid) const;

  virtual OdUInt32 setAttributes(OdGiDrawableTraits* pTraits) const;
};

typedef OdSmartPtr<OdDbLinetypeTableRecord> OdDbLinetypeTableRecordPtr;

#include "DD_PackPop.h"

#endif // _ODDBLINETYPETABLERECORD_INCLUDED


