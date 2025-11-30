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



#ifndef _SmNamedDbObjectReference_h_Included_
#define _SmNamedDbObjectReference_h_Included_

#include "SmDbObjectReference.h"
#include <OdWString.h>

namespace dst
{
  /** Description:

      {group:OdSm_Classes} 
  */
  struct SMDLL_EXPORT OdSmNamedDbObjectReference : OdSmDbObjectReference
  {
    ODRX_DECLARE_MEMBERS(OdSmNamedDbObjectReference);
    virtual void setName( OdWString name ) = 0;
    virtual OdWString getName() const = 0;
    // owner currently is not saved to file, because these methods are not 
    // implemented in "acsmcomponents16.dll" library, and correct tag name for OwnerDbHandle is unknown
    virtual void setOwnerDbHandle( OdString handle ) = 0;
    virtual OdString getOwnerDbHandle() const = 0;
  };
  
  /** Description:

      {group:OdSm_Classes} 
  */
  struct SMDLL_EXPORT OdSmDbBlockRecordReference : OdSmNamedDbObjectReference 
  {
    ODRX_DECLARE_MEMBERS(OdSmDbBlockRecordReference);
  };

  /** Description:

      {group:OdSm_Classes} 
  */
  struct SMDLL_EXPORT OdSmDbViewReference : OdSmNamedDbObjectReference 
  {
    ODRX_DECLARE_MEMBERS(OdSmDbViewReference);
  };

  /** Description:

      {group:OdSm_Classes} 
  */
  struct SMDLL_EXPORT OdSmDbLayoutReference : OdSmNamedDbObjectReference 
  {
    ODRX_DECLARE_MEMBERS(OdSmDbLayoutReference);
  };
  
  typedef OdSmartPtr<OdSmNamedDbObjectReference> OdSmNamedDbObjectReferencePtr;
  typedef OdSmartPtr<OdSmDbBlockRecordReference>OdSmDbBlockRecordReferencePtr;
  typedef OdSmartPtr<OdSmDbViewReference>OdSmDbViewReferencePtr;
  typedef OdSmartPtr<OdSmDbLayoutReference>OdSmDbLayoutReferencePtr;
}

#endif //_SmNamedDbObjectReference_h_Included_
