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



#ifndef _SmCustomProperty_h_Included_
#define _SmCustomProperty_h_Included_

#include "Sm/SmPersist.h"
#define STL_USING_STRING
#include "OdaSTL.h"
#include <RxObject.h>
#include <RxVariant.h>
#include "SmExport.h"

namespace dst
{
  enum PropertyFlags
  {
    EMPTY = 0,
    CUSTOM_SHEETSET_PROP = 1,
    CUSTOM_SHEET_PROP = 2,
    IS_CHILD = 4
  };
  
  /** Description:

      {group:OdSm_Classes} 
  */
  struct SMDLL_EXPORT OdSmCustomPropertyValue : public OdSmPersist
  {
    ODRX_DECLARE_MEMBERS( OdSmCustomPropertyValue );
    virtual OdVariant getValue() const = 0;
    virtual void setValue ( const OdVariant& value ) = 0;
    virtual PropertyFlags getFlags() const = 0;
    virtual setFlags( PropertyFlags value ) = 0;
  };
  typedef OdSmartPtr<OdSmCustomPropertyValue> OdSmCustomPropertyValuePtr;
  
  /** Description:

      {group:OdSm_Classes} 
  */
  struct OdSmEnumCustomPropertyValue : public OdRxObject
  {
    virtual bool next( OdWString& propname, const OdSmCustomPropertyValue*& ppValue ) = 0;
    virtual void reset() = 0;
  };
  typedef OdSmartPtr<OdSmEnumCustomPropertyValue> OdSmEnumCustomPropertyValuePtr;
  
  /** Description:

      {group:OdSm_Classes} 
  */
  struct SMDLL_EXPORT OdSmCustomPropertyBag : OdSmPersist
  {
    ODRX_DECLARE_MEMBERS(OdSmCustomPropertyBag);
    virtual const OdSmCustomPropertyValue* getProperty( OdWString name ) const = 0;
    virtual void setProperty( OdWString name, OdSmCustomPropertyValue* value ) = 0;
    virtual OdSmEnumCustomPropertyValuePtr getPropertyEnumerator() const = 0;
  };
  typedef OdSmartPtr<OdSmCustomPropertyBag> OdSmCustomPropertyBagPtr;
}

#endif
