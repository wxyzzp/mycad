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



#ifndef _SmObjectId_h_Included_
#define _SmObjectId_h_Included_

#include <RxObject.h>
#include <OdWString.h>
#include "SmPersist.h"

namespace dst
{
  struct OdSmDatabase;

  /** Description:

      {group:OdSm_Classes} 
  */
  struct SMDLL_EXPORT OdSmObjectId : public OdRxObject 
  {
    ODRX_DECLARE_MEMBERS(OdSmObjectId);
    virtual OdString getHandle() const = 0;
    virtual const OdSmDatabase* getDatabase() const = 0;
    virtual const OdSmPersist* getOwner() const = 0;
    virtual bool isEqual( OdSmObjectId* pId ) const = 0;
    virtual bool isValid() const = 0;
  };
  typedef OdSmartPtr<OdSmObjectId> OdSmObjectIdPtr;
}

#endif //_SmObjectId_h_Included_
