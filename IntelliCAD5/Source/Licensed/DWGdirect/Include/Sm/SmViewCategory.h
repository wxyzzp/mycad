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



#ifndef _SmViewCategory_h_Included_
#define _SmViewCategory_h_Included_

#include "SmSheetView.h"
#include "SmCalloutBlocks.h"

namespace dst
{
  /** Description:

      {group:OdSm_Classes} 
  */
  struct SMDLL_EXPORT OdSmViewCategory : OdSmComponent
  {
    ODRX_DECLARE_MEMBERS(OdSmViewCategory);
    virtual OdSmEnumSheetViewPtr getSheetViewEnumerator() const = 0;
    virtual OdSmCalloutBlocks* getCalloutBlocks() = 0;
    virtual const OdSmCalloutBlocks* getCalloutBlocks() const = 0;
  };
  typedef OdSmartPtr<OdSmViewCategory> OdSmViewCategoryPtr;
}
#endif //_SmViewCategory_h_Included_
