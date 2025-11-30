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



#ifndef _SmDatabase_h_Included
#define _SmDatabase_h_Included

#include "SmComponent.h"
#include "SmCustomProperty.h"
#include "SmEvents.h"
#include "SmSheetSet.h"
#include "SmEnum.h"
#include "SmPersist.h"
#include "SmObjectId.h"

namespace dst
{
  typedef OdSmEnum<OdSmPersist> OdSmEnumPersist;
  typedef OdSmartPtr<OdSmEnumPersist> OdSmEnumPersistPtr;

  /** Description:

      {group:OdSm_Classes} 
  */
  struct SMDLL_EXPORT OdSmDatabase : OdSmComponent
  {
    ODRX_DECLARE_MEMBERS( OdSmDatabase );
    virtual void loadFromFile( const OdString& fileName ) = 0;
    virtual void save() = 0;
    virtual OdString fileName() const = 0;
    virtual void setFileName ( const OdString& newVal ) = 0;
    virtual OdString templateDstFileName() const = 0;
    virtual OdSmSheetSet* sheetSet() = 0;
    virtual const OdSmSheetSet* sheetSet() const = 0;
    virtual OdSmEnumPersistPtr enumerator() const = 0;
    virtual long subscribe( OdSmEvents * eventHandler ) = 0;
    virtual void unSubscribe( long cookie ) = 0;
    virtual void notifyRegisteredEventHandlers( OdSmEvent event, OdSmPersist * comp ) const = 0;
    virtual OdSmPersist* findObject( const OdString& hand ) const = 0;
    virtual void findAcDbObjectReferences( OdString filename, OdString dbHandle, std::vector<OdSmPersist*>& ppObjects ) = 0;
    virtual void updateInMemoryDwgHints() = 0;
    virtual bool isTemporary() const = 0;
    virtual void setIsTemporary() = 0;
    virtual OdSmObjectIdPtr registerNewObject( OdString hand, OdSmPersist* pObject ) = 0;
  };

  typedef OdSmartPtr<OdSmDatabase> OdSmDatabasePtr;
}


#endif
