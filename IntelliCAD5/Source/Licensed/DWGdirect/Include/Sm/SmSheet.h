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



#ifndef _SmSheet_h_Included_
#define _SmSheet_h_Included_

#include "SmComponent.h"
#include "SmNamedDbObjectReference.h"

namespace dst
{
  struct OdSmSheetViews;

  /** Description:

      {group:OdSm_Classes} 
  */
  struct SMDLL_EXPORT OdSmSheet : OdSmComponent
  {
    ODRX_DECLARE_MEMBERS(OdSmSheet);
    virtual OdWString getNumber() = 0;
    virtual void setNumber( OdWString ) = 0;
    virtual OdWString getTitle() = 0;
    virtual void setTitle ( OdWString ) = 0;
    virtual OdSmDbLayoutReference* getLayout() = 0;
    virtual void setLayout( OdSmDbLayoutReference* pLayoutRef ) = 0;
    virtual bool getDoNotPlot() = 0;
    virtual void setDoNotPlot( bool doNotPlot ) = 0;
    virtual OdSmSheetViews* getSheetViews() = 0;
  };
  typedef OdSmartPtr<OdSmSheet> OdSmSheetPtr;
}

#endif //_SmSheet_h_Included_
