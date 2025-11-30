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
// programs incorporating this software must include the following statment 
// with their copyright notices:
//
//      DWGdirect © 2002 by Open Design Alliance Inc. All rights reserved. 
//
// By use of this software, you acknowledge and accept the terms of this 
// agreement.
///////////////////////////////////////////////////////////////////////////////

#ifndef __ODDBMATERIAL_H__
#define __ODDBMATERIAL_H__

#include "DD_PackPush.h"

#include "Gi/GiMaterial.h"
#include "DbObject.h"

/** Description:
    Represents material properties for shaded entities.

    {group:OdDb_Classes} 
*/
class TOOLKIT_EXPORT OdDbMaterial : public OdDbObject
{
public:
  ODDB_DECLARE_MEMBERS(OdDbMaterial);

  OdDbMaterial();

  virtual OdResult setName(const OdString& pName);
  virtual OdString name() const;

  virtual void setDescription(const OdString& pDescription);
  virtual OdString description() const;

  virtual void setAmbient(const OdGiMaterialColor& color);
  virtual void ambient(OdGiMaterialColor& color) const;

  virtual void setDiffuse(const OdGiMaterialColor& color, const OdGiMaterialMap& map);
  virtual void diffuse(OdGiMaterialColor& color, OdGiMaterialMap& map) const;

  virtual void setSpecular(const OdGiMaterialColor& color, const OdGiMaterialMap& map, double dGloss);
  virtual void specular(OdGiMaterialColor& color, OdGiMaterialMap& map, double& dGloss) const;

  virtual void setReflection(const OdGiMaterialMap& map);
  virtual void reflection(OdGiMaterialMap& map) const;

  virtual void setOpacity(double dPercentage, const OdGiMaterialMap& map);
  virtual void opacity(double& dPercentage, OdGiMaterialMap& map) const;

  virtual void setBump(const OdGiMaterialMap& map);
  virtual void bump(OdGiMaterialMap& map) const;

  virtual void setRefraction(double dIndex, const OdGiMaterialMap& map);
  virtual void refraction(double& dIndex, OdGiMaterialMap& map) const;

  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);
  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;
  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);
  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;

  /*
  virtual OdGiDrawable* drawable();
  virtual void setGsNode(OdGsNode* pNode);
  virtual OdGsNode* gsNode() const;
  */

  virtual OdUInt32 setAttributes(OdGiDrawableTraits* pTraits) const;
};

typedef OdSmartPtr<OdDbMaterial> OdDbMaterialPtr;

#include "DD_PackPop.h"

#endif
