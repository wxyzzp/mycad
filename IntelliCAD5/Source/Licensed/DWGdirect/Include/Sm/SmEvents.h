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



#ifndef _SmEvents_h_Included_
#define _SmEvents_h_Included_

#include "SmPersist.h"

namespace dst
{
  enum OdSmEvent
  {
    SHEET_ADDED = 0,
    SHEET_DELETED = 1,
    SHEET_SUBSET_CREATED = 2,
    SHEET_SUBSET_DELETED = 3,
    SHEET_SELECTION_SET_CREATED = 4,
    SHEET_SELECTION_SET_DELETED = 5,
    SHEET_VIEW_CREATED = 6,
    SHEET_VIEW_DELETED = 7,
    SHEET_VIEW_CATEGORY_CREATED = 8,
    SHEET_VIEW_CATEGORY_DELETED = 9,
    MODEL_RESOURCE_ADDED = 10,
    MODEL_RESOURCE_DELETED = 11,
    CALLOUT_BLOCK_ADDED = 12,
    CALLOUT_BLOCK_DELETED = 13,
    SHEETS_RENUMBERED = 14,
    ACSM_DATABASE_CREATED = 15,
    ACSM_DATABASE_OPENED = 16,
    ACSM_DATABASE_IS_CLOSING = 17,
    ACSM_DATABASE_WATCH_ERROR = 18,
    ACSM_DATABASE_LOCKED = 19,
    ACSM_DATABASE_UNLOCKED = 20,
    ACSM_DATABASE_CHANGED = 21,
    ACSM_DATABASE_INITNEW_BEGIN = 22,
    ACSM_DATABASE_INITNEW_FAILED = 23,
    ACSM_DATABASE_INITNEW_COMPLETE = 24,
    ACSM_DATABASE_LOAD_BEGIN = 25,
    ACSM_DATABASE_LOAD_FAILED = 26,
    ACSM_DATABASE_LOAD_COMPLETE = 27
  };

  /** Description:

      {group:OdSm_Classes} 
  */
  struct OdSmEvents : public OdRxObject
  {
    virtual void onChanged( OdSmEvent event, OdSmPersist * comp ) = 0;
  };
}

#endif //_SmEvents_h_Included_
