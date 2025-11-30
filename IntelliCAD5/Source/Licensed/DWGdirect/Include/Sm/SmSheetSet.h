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



#ifndef _SmSheetSet_h_Included_
#define _SmSheetSet_h_Included_

#include "SmSubset.h"
#include "SmProjectPointLocations.h"

namespace dst
{
  struct OdSmSheetSelSets;
  struct OdSmResources;
  struct OdSmCalloutBlocks;
  struct OdSmViewCategories;
  struct OdSmPublishOptions;
  
  /** Description:

      {group:OdSm_Classes} 
  */
  struct SMDLL_EXPORT OdSmSheetSet : OdSmSubset
  {
    ODRX_DECLARE_MEMBERS(OdSmSheetSet);
    // Retrieves the template containing alternative page setups
    virtual const OdSmFileReference* getAltPageSetups() const = 0;
    //
    virtual void setAltPageSetups ( OdSmFileReference * pDwtRef ) = 0;
    // Retrieves the default page setup within the template containing alternative page setups
    virtual const OdSmNamedDbObjectReference* getDefAltPageSetup() const = 0;
    //
    virtual void setDefAltPageSetup ( OdSmNamedDbObjectReference * pAltPageSetup ) = 0;
    //
    virtual bool getPromptForDwgName() const = 0;
    virtual void setPromptForDwgName( bool askForName ) = 0;
    // Returns a collection of the sheet selection sets associated with the sheet set.
    virtual OdSmSheetSelSets* getSheetSelSets() = 0;
    virtual const OdSmSheetSelSets* getSheetSelSets() const = 0;
    // Returns a collection of the resources associated with the sheet set
    virtual OdSmResources* getResources() = 0;
    virtual const OdSmResources* getResources() const = 0;
    // Returns a collection of callout block references that are associated with the sheet 
    virtual OdSmCalloutBlocks* getCalloutBlocks() = 0;
    virtual const OdSmCalloutBlocks* getCalloutBlocks() const = 0;
    // reserved
    virtual OdSmViewCategories* getViewCategories() = 0;
    virtual const OdSmViewCategories* getViewCategories() const = 0;
    // Returns the label block definition for the sheet set. 
    virtual const OdSmDbBlockRecordReference* getDefLabelBlk() const = 0;
    // sets the label block definition for the sheet set. 
    virtual void setDefLabelBlk ( OdSmDbBlockRecordReference* pLabelBlkRef ) = 0;
    // reserved
    virtual OdSmPublishOptions* getPublishOptions() = 0;
    virtual const OdSmPublishOptions* getPublishOptions() const = 0;
    //
    virtual long subscribe( OdSmEvents * eventHandler ) = 0;
    virtual void unSubscribe( long cookie ) = 0;
    // internal
    virtual void updateSheetCustomProps() = 0;
  };
}

#endif
