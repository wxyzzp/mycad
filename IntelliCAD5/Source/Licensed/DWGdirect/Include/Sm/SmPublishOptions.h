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



#ifndef _SmPublishOptions_h_Included_
#define _SmPublishOptions_h_Included_

#include "SmPersist.h"
#include "SmFileReference.h"
#include "SmCustomProperty.h"

namespace dst
{
  /** Description:

      {group:OdSm_Classes} 
  */
  struct SMDLL_EXPORT OdSmPublishOptions : OdSmPersist
  {
    ODRX_DECLARE_MEMBERS(OdSmPublishOptions);
    virtual const OdSmFileReference* getDefaultOutputdir() const = 0;
    virtual void setDefaultOutputdir( OdSmFileReference* pValue ) = 0;
    virtual bool getDwfType() const = 0;
    virtual void setDwfType( bool value ) = 0;
    virtual bool getPromptForName() const = 0;
    virtual void setPromptForName( bool value ) = 0;
    virtual bool getUsePassword() const = 0;
    virtual void setUsePassword( bool value ) = 0;
    virtual bool getPromptForPassword() const = 0;
    virtual void setPromptForPassword( bool value ) = 0;
    virtual bool getLayerInfo() const = 0;
    virtual void setLayerInfo( bool value ) = 0;
    virtual const OdSmCustomPropertyBag* getUnrecognizedData() const = 0;
    virtual void setUnrecognizedData( OdSmCustomPropertyBag * bag ) = 0;
    virtual const OdSmCustomPropertyBag* getUnrecognizedSections() const = 0;
    virtual void setUnrecognizedSections( OdSmCustomPropertyBag* sectionArray ) = 0;
  };
  typedef OdSmartPtr<OdSmPublishOptions> OdSmPublishOptionsPtr;
}
#endif //_SmPublishOptions_h_Included_
