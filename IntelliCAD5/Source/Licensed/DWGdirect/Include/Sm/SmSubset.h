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



#ifndef _SmSubset_h_Included_
#define _SmSubset_h_Included_

#include "SmComponent.h"
#include <OdWString.h>
#include "SmEvents.h"
#include "SmFileReference.h"
#include "SmNamedDbObjectReference.h"
#include "SmSheet.h"

namespace dst
{
  struct SMDLL_EXPORT OdSmSubset;
  typedef OdSmartPtr<OdSmSubset> OdSmSubsetPtr;

  /** Description:

      {group:OdSm_Classes} 
  */
  struct SMDLL_EXPORT OdSmSubset : OdSmComponent
  {
    ODRX_DECLARE_MEMBERS(OdSmSubset);
    // Returns the OdSmFileReference that points to the location in the file system where new sheets are created. 
    virtual const OdSmFileReference* getNewSheetLocation() const = 0;
    // sets the OdSmFileReference that points to the location in the file system where new sheets are created
    virtual void setNewSheetLocation ( OdSmFileReference* pFileRef ) = 0;
    // Returns the OdSmNamedAcDbObjectReference that points to the layout used as a template for new sheets. 
    virtual const OdSmDbLayoutReference* getDefDwtLayout() const = 0;
    // sets the AcSmAcDbLayoutReference that points to the layout used as a template for new sheets.
    virtual void setDefDwtLayout ( OdSmDbLayoutReference* pLayoutRef ) = 0;
    // Returns a Boolean flag indicating if the application should prompt for a DWT file.
    virtual bool getPromptForDwt() const = 0;
    // sets a Boolean flag indicating if the application should prompt for a DWT file.
    virtual void setPromptForDwt( bool askForDwt ) = 0;
    // Returns an enumerator of all components in this subset
    virtual OdSmEnumComponentPtr getSheetEnumerator() const = 0;
    // Adds a new sheet component without adding it to the subset.
    // To add the new sheet component to the subset, use the InsertComponent method
    virtual OdSmSheetPtr addNewSheet( OdWString name, OdWString desc ) = 0;
    // Inserts a sheet at the position specified.
    virtual void insertComponent ( OdSmComponent* newSheet, OdSmComponent* beforeComp = 0) = 0;
    // Inserts a sheet at the position specified
    virtual void insertComponentAfter( OdSmComponent* newSheet, OdSmComponent* afterComp = 0 ) = 0;
    // Adds a new sheet component that references the layout of AcSmAcDbLayoutReference. 
    // The new component is not added to the subset.
    // To add the new sheet component to the subset, use the InsertComponent method
    virtual OdSmSheetPtr importSheet( OdSmDbLayoutReference* pLayoutRef ) = 0;
    // Removes the specified sheet component from the subset.
    virtual void removeSheet( OdSmSheet * sheet ) = 0;
    // Adds a new subset component without adding it to the current subset.
    virtual OdSmSubsetPtr createSubset( OdWString name, OdWString desc ) = 0;
    // Removes the specified subset component from the collection
    virtual void removeSubset( OdSmSubset * subset ) = 0;
    // Forces a notification to all registered notification handlers
    virtual void notifyRegisteredEventHandlers( OdSmEvent event, OdSmPersist * comp ) = 0;
    // Updates the hints in the open drawings referenced in the subset
    virtual void updateInMemoryDwgHints ( ) = 0;
  };
}

#endif //_SmSubset_h_Included_
