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



#ifndef _SUMMINFO_H_
#define _SUMMINFO_H_

#include "RxObject.h"
#include "OdString.h"
#include "DbDatabase.h"

/** Description:

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbDatabaseSummaryInfo : public OdRxObject
{
public:
  ODRX_DECLARE_MEMBERS(OdDbDatabaseSummaryInfo);

  OdDbDatabaseSummaryInfo() {}
  
  virtual OdString getTitle() const = 0;
  virtual void setTitle(const OdString& title) = 0;
  
  virtual OdString getSubject() const = 0;
  virtual void setSubject(const OdString& subject) = 0;
  
  virtual OdString getAuthor() const = 0;
  virtual void setAuthor(const OdString& author) = 0;
  
  virtual OdString getKeywords() const = 0;
  virtual void setKeywords(const OdString& keywordlist) = 0;
  
  virtual OdString getComments() const = 0;
  virtual void setComments(const OdString& comments) = 0;
  
  virtual OdString getLastSavedBy() const = 0;
  virtual void setLastSavedBy(const OdString& lastSavedBy) = 0;
  
  virtual OdString getRevisionNumber() const = 0;
  virtual void setRevisionNumber(const OdString& revisionNumber) = 0;
  
  virtual OdString getHyperlinkBase() const = 0;
  virtual void setHyperlinkBase(const OdString& HyperlinkBase) = 0;
  
  virtual int numCustomInfo() const = 0;
  
  virtual void addCustomSummaryInfo(const OdString& key, const OdString& value ) = 0;
  
  virtual void deleteCustomSummaryInfo(int index) = 0;
  virtual bool deleteCustomSummaryInfo(const OdString& key) = 0;
  
  virtual void getCustomSummaryInfo(int index, OdString& key, OdString& value ) const = 0;
  virtual void setCustomSummaryInfo(int index, const OdString& key, const OdString& value ) = 0;
  
  virtual bool getCustomSummaryInfo(const OdString& customInfoKey, OdString& value ) const = 0;
  
  virtual void setCustomSummaryInfo(const OdString& customInfoKey, const OdString& value ) = 0;
  
  virtual OdDbDatabase* database() const = 0; 
  virtual void setDatabase(OdDbDatabase *pDb) = 0;
};

typedef OdSmartPtr<OdDbDatabaseSummaryInfo> OdDbDatabaseSummaryInfoPtr;

/** Description:

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbLibraryInfo
{
public:
  virtual const OdChar* getLibName() const = 0;
  virtual const OdChar* getLibVersion() const = 0;
  virtual const OdChar* getCompanyName() const = 0;
  virtual const OdChar* getCopyright() const = 0;
};

TOOLKIT_EXPORT OdDbLibraryInfo*           oddbGetLibraryInfo(void);
TOOLKIT_EXPORT OdDbDatabaseSummaryInfoPtr oddbGetSummaryInfo(OdDbDatabase* pDb);
TOOLKIT_EXPORT void                       oddbPutSummaryInfo(const OdDbDatabaseSummaryInfo* pInfo);

#endif // _SUMMINFO_H_

