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



#ifndef _SmPersist_h_Included_
#define _SmPersist_h_Included_

#include <RxObject.h>
#define STL_USING_STRING
#define STL_USING_VECTOR
#include <OdaSTL.h>
#include <OdString.h>
#include "SmExport.h"

namespace dst
{
  struct OdSmFiler;
  struct OdSmDatabase;
  struct OdSmObjectId;

  /** Description:

      {group:OdSm_Classes} 
  */
  struct SMDLL_EXPORT OdSmPersist : public OdRxObject
  {
    ODRX_DECLARE_MEMBERS(OdSmPersist);
    virtual OdString clsid() const = 0;
    virtual bool isDirty() const = 0;
    virtual bool load ( OdSmFiler * pFiler ) = 0;
    virtual void save ( OdSmFiler * pFiler ) const = 0;
    virtual OdString typeName() const = 0;
    virtual void initNew( const OdSmPersist* pOwner, OdString handle = "" ) = 0;
    virtual const OdSmPersist* owner() const = 0;
    virtual void setOwner( const OdSmPersist * pOwner ) = 0;
    virtual const OdSmDatabase* database() const = 0;
    virtual const OdSmObjectId* objectId() const = 0;
    virtual void clear() = 0;
    virtual void directlyOwnedObjects( std::vector<OdSmPersist*>& ) const = 0;
  };
  
  typedef OdSmartPtr<OdSmPersist> OdSmPersistPtr;
}

#endif
