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



#ifndef _SmProjectPointLocation_h_Included_
#define _SmProjectPointLocation_h_Included_

#include "SmPersist.h"

namespace dst
{
  /** Description:

      {group:OdSm_Classes} 
  */
  struct SMDLL_EXPORT OdSmProjectPointLocation : OdSmPersist
  {
    ODRX_DECLARE_MEMBERS(OdSmProjectPointLocation);
    OdWString getName() const;
    void setName( OdWString name );
    OdWString getURL() const;
    void setURL( OdWString url );
    OdWString getFolder() const;
    void setFolder( OdWString folder );
    OdWString getUsername() const;
    void setUsername( OdWString username );
    OdWString getPassword() const;
    void setPassword( OdWString password );
  };
  typedef OdSmartPtr<OdSmProjectPointLocation> OdSmProjectPointLocationPtr;
}

#endif //_SmProjectPointLocation_h_Included_
