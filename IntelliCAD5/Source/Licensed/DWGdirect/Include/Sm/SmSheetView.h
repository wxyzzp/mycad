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



#ifndef _SmSheetView_h_Included_
#define _SmSheetView_h_Included_

#include "SmComponent.h"

namespace dst
{
  struct OdSmViewCategory;
  struct OdSmDbViewReference;

  /** Description:

      {group:OdSm_Classes} 
  */
  struct SMDLL_EXPORT OdSmSheetView : OdSmComponent
  {
    ODRX_DECLARE_MEMBERS(OdSmSheetView);
    virtual const OdSmDbViewReference* getNamedView() const = 0;
    virtual void setNamedView( OdSmDbViewReference* pViewRef ) = 0;
    virtual OdSmViewCategory* getCategory() const = 0;
    virtual void setCategory(  OdSmViewCategory* pViewCat ) = 0;
    virtual OdWString getNumber() const = 0;
    virtual void setNumber( OdWString num ) = 0;
    virtual OdWString getTitle() const = 0;
    virtual void setTitle( OdWString title ) = 0;
  };
  typedef OdSmEnum<OdSmSheetView> OdSmEnumSheetView;
  typedef OdSmartPtr<OdSmEnumSheetView> OdSmEnumSheetViewPtr;
}

#endif //_SmSheetView_h_Included_
