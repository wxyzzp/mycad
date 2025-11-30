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



#ifndef _OD_DB_ATTRDEF_
#define _OD_DB_ATTRDEF_

#include "DD_PackPush.h"

#include "DbText.h"

/** Description:
    Represents and attribute definition (ATTDEF) within an OdDbDatabase.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbAttributeDefinition : public OdDbText
{
public:

  ODDB_DECLARE_MEMBERS(OdDbAttributeDefinition);

  /** Description:
      Constructor (no arguments).
  */
  OdDbAttributeDefinition();

  /** Description:
      Returns the prompt string for this attribute definition (DXF 3).
  */
  const OdString prompt() const;

  /** Description:
      Sets the prompt string for this attribute definition (DXF 3).
  */
  void setPrompt(const char*);

  /** Description:
      Returns the tag string for this attribute definition (DXF 2).
  */
  const OdString tag() const;

  /** Description:
      Sets the tag string for this attribute definition (DXF 2).
  */
  void setTag(const char*);

  /** Description:
      Returns the invisible status for this attribute definition (DXF 70, bit 0x01).
  */
  bool isInvisible() const;

  /** Description:
      Sets the invisible status for this attribute definition (DXF 70, bit 0x01).
  */
  void setInvisible(bool);

  /** Description:
      Returns the constant status for this attribute definition (DXF 70, bit 0x02).
  */
  bool isConstant() const;

  /** Description:
      Sets the constant status for this attribute definition (DXF 70, bit 0x02).
  */
  void setConstant(bool);

  /** Description:
      Returns the verifiable status for this attribute definition (DXF 70, bit 0x04).
  */
  bool isVerifiable() const;

  /** Description:
      Sets the verifiable status for this attribute definition (DXF 70, bit 0x04).
  */
  void setVerifiable(bool);

  /** Description:
      Returns the preset status for this attribute definition (DXF 70, bit 0x08).
  */
  bool isPreset() const;

  /** Description:
      Sets the preset status for this attribute definition (DXF 70, bit 0x08).
  */
  void setPreset(bool);

  /** Description:
      Returns the field length of this attribute definition (DXF 73).
  */
  OdUInt16 fieldLength() const;

  /** Description:
      Sets the field length of this attribute definition (DXF 73).
  */
  void setFieldLength(OdUInt16);

  virtual void getClassID(void** pClsid) const;

  /** Description:
      Adjusts the position of this attribute definition, if the alignent is not left base.
      This function is called by DWGdirect when a newly created 
      attribute definition entity is closed.

      Arguments:
      pDb (I) Database pointer used to resolve the text style for this 
      entity.
  */
  virtual void adjustAlignment(OdDbDatabase* pDb = NULL);

  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;

  virtual OdResult dxfInFields_R12(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields_R12(OdDbDxfFiler* pFiler) const;

  bool worldDraw(OdGiWorldDraw* pWd) const;

  void appendToOwner(OdDbIdPair& pair, OdDbObject* pOwner, OdDbIdMapping& idMap);
};

typedef OdSmartPtr<OdDbAttributeDefinition> OdDbAttributeDefinitionPtr;

#include "DD_PackPop.h"

#endif


