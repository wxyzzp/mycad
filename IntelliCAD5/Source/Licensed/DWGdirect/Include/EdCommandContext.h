#ifndef _ODEDCOMMANDCONTEXT_H_INCLUDED_
#define _ODEDCOMMANDCONTEXT_H_INCLUDED_

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



#include "DbExport.h"
#include "RxObject.h"


class OdEdUserIO;

/** Description:
    Interface class that specifies the base level functionality needed to support
    the execution of custom commands.

    {group:OdEd_Classes}
*/
class FIRSTDLL_EXPORT OdEdCommandContext : public OdRxObject
{ 
public:
  ODRX_DECLARE_MEMBERS(OdEdCommandContext);

  /** Description:
      Returns a pointer to a custom IO object that will be used during the execution of 
      a custom command.
  */
  virtual OdEdUserIO* userIO() = 0;
};

typedef OdSmartPtr<OdEdCommandContext> OdEdCommandContextPtr;



#endif //#ifndef _ODEDCOMMANDCONTEXT_H_INCLUDED_

