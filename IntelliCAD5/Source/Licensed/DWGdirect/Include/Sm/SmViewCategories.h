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



#ifndef _SmViewCategories_h_Included_
#define _SmViewCategories_h_Included_

#include "SmComponent.h"
#include "SmViewCategory.h"
#include "SmEnum.h"

namespace dst
{
  typedef OdSmEnum<OdSmViewCategory>OdSmEnumViewCategory;
  typedef OdSmartPtr<OdSmEnumViewCategory> OdSmEnumViewCategoryPtr;
  
  /** Description:

      {group:OdSm_Classes} 
  */
  struct SMDLL_EXPORT OdSmViewCategories : OdSmComponent
  {
    ODRX_DECLARE_MEMBERS(OdSmViewCategories);
    virtual OdSmEnumViewCategoryPtr getEnumerator() const = 0;
    virtual OdSmViewCategory* createViewCategory( OdWString name, OdWString desc, OdWString id ) = 0;
    virtual void removeViewCategory( OdSmViewCategory * viewCat ) = 0;
    virtual OdSmViewCategory* getDefaultViewCategory() = 0;
  };
  typedef OdSmartPtr<OdSmViewCategories> OdSmViewCategoriesPtr;
}

#endif //_SmViewCategories_h_Included_
