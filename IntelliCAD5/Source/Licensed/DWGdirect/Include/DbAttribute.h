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



#ifndef _OD_DB_ATTRIBUTE_
#define _OD_DB_ATTRIBUTE_

#include "DD_PackPush.h"

#include "DbText.h"

class OdDbAttributeDefinition;

/** Description:
    Represents and attribute (ATTRIB) within an OdDbDatabase.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbAttribute : public OdDbText
{
public:
  
  ODDB_DECLARE_MEMBERS(OdDbAttribute);

  /** Description:
      Constructor (no argument).
  */
  OdDbAttribute();

  /** Description:
      Returns the tag string for this attribute (DXF 2).
  */
  const OdString tag() const;

  /** Description:
      Sets the tag string for this attribute (DXF 2).
  */
  void setTag(const OdChar* newTag);

  /** Description:
      Returns true if this attribute is invisble, false otherwise (DXF 70, bit 0x01).
  */
  bool isInvisible() const;

  /** Description:
      Sets the invisble status of this attribute (DXF 70, bit 0x01).

      Arguments:
      invisible If true, this attribute will be set to invisible (DXF 70, bit 0x01 set).
  */
  void setInvisible(bool invisible);

  /** Description:
      Returns true if this attribute is set to be a constant, false otherwise 
      (DXF 70, bit 0x02).
  */
  bool isConstant() const;

  /** Description:
      Returns true if this attribute is set to be verifiable, false otherwise 
      (DXF 70, bit 0x04).
  */
  bool isVerifiable() const;

  /** Description:
      Returns true if this attribute has a preset value and will not ask for user
      input, false otherwise (DXF 70, bit 0x08).
  */
  bool isPreset() const;

  /** Description:
      Returns the field length of this attribute (DXF 73).
  */
  OdUInt16 fieldLength() const;

  /** Description:
      Sets the field length of this attribute (DXF 73).
  */
  void setFieldLength(OdUInt16);

  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;

  virtual OdResult dxfInFields_R12(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields_R12(OdDbDxfFiler* pFiler) const;

  virtual void getClassID(void** pClsid) const;

  OdUInt32 setAttributes(OdGiDrawableTraits* pTraits) const;

  void setAttributeFromBlock(const OdGeMatrix3d& xform);
  
  void setAttributeFromBlock(const OdDbAttributeDefinition* pAttDef, const OdGeMatrix3d& xform);

};

typedef OdSmartPtr<OdDbAttribute> OdDbAttributePtr;

#include "DD_PackPop.h"

#endif


