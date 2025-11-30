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



#ifndef _OD_DB_AUDIT_
#define _OD_DB_AUDIT_

#include "DD_PackPush.h"

#include "OdaCommon.h"
#include "OdString.h"
#include "CmColor.h"

class OdDbAuditInfoImpl;
class OdDbRecover;
class OdDbObject;
class OdDbObjectId;
class OdDbDatabase;
class OdDbAuditInfo;
class OdDbHostAppServices;

OdString odDbGetObjectName(const OdDbObject* pObj);
OdString odDbGetObjectIdName(const OdDbObjectId& id);
OdString odDbGetHandleName(const OdDbHandle& handle);
OdString odDbGenerateName(const OdDbObjectId& id);
OdString odDbGenerateName(OdUInt32 i);
bool     odDbAuditColorIndex(OdInt16& colorIndex, OdDbAuditInfo* pAuditInfo,
                             OdDbHostAppServices* pHostApp = 0);
bool     odDbAuditColor(OdCmColor& color, OdDbAuditInfo* pAuditInfo, OdDbHostAppServices* pHostApp = 0);

/** Description:
    Keeps track of audit information during a database audit.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbAuditInfo
{
  friend class OdDbAuditController;
public:

  enum PrintDest
  {
    kSilent = 0,
    kCmdLine,
    kFile,
    kBoth
  };

  struct MsgInfo
  {
    OdString strName;
    OdString strValue; 
    OdString strValidation;
    OdString strDefaultValue; 
    bool     bIsError;
  };

  /** Description:
      Constructor (no arguments).  
  */
  OdDbAuditInfo();
  
  /** Description:
      Destructor.
  */
  ~OdDbAuditInfo();

  /** Description:
      Returns true if errors are to be fixed during the audit that is using 
      this object.
  */
  bool fixErrors() const;
  
  /** Description:
      Sets the "fix errors" property, which determines whether or not errors
      will be fixed during subsequent audit operations.
  */
  void setFixErrors(bool bFix);

  /** Description:
      Returns the number of errors reported to this OdDbAuditInfo object.
  */
  int numErrors() const;

  /** Description:
      Returns the number of errors reported as fixed to this OdDbAuditInfo object.
  */
  int numFixes() const;

  /** Description:
      Increments the "errors found" count for this OdDbAuditInfo object by the specified amount.
  */
  void errorsFound(int);

  /** Description:
      Increments the "errors fixed" count for this OdDbAuditInfo object by the specified amount.
  */
  void errorsFixed(int);

  /** Description:
      Prints out log information about the errors detected or fixed during an audit.

      Arguments:
      strName (I) String describing the type of erroneous data found. 
      strValue (I) String describing the value of the bad data.
      strValidation (I) String describing the reason the data is bad.
      strDefaultValue (I) String describing the default value for the data.
  */
  virtual void printError(const OdChar* strName, 
                          const OdChar* strValue, 
                          const OdChar* strValidation, 
                          const OdChar* strDefaultValue);

  /** Description:
      Prints out log information about the errors detected or fixed during an audit.

      Arguments:
      pObj (I) Pointer to the object which found the error, which can be used to 
      generate and object name.
      strValue (I) String describing the value of the bad data.
      strValidation (I) String describing the reason the data is bad.
      strDefaultValue (I) String describing the default value of the data.
  */
  virtual void printError(const OdDbObject* pObj, 
                          const OdChar* strValue, 
                          const OdChar* strValidation, 
                          const OdChar* strDefaultValue);

  /** Description:
      Prints out custom log information pertaining to the audit.

      Arguments:
      strInfo (I) String containing the log message.
  */
  virtual void printInfo (const OdChar* strInfo);

  /** Description:
      TBC.
  */
  void requestRegen();

  /** Description:
      Sets the count of entities processed to 0.
  */
  void resetNumEntities();
  
  /** Description:
      Increments the count of entities processed by 1.
  */
  void incNumEntities();

  /** Description:
      Returns the count of entities processed. 
  */
  int numEntities();

  /** Description:
      Returns the last error info that is intended for print
  */
  virtual const MsgInfo& getLastInfo();
  
  virtual void setLastInfo(MsgInfo &miLast);

  /*  If the current count of entities being maintained in the instance 
      of OdDbAuditInfo is a multiple of 100, and msg is not NULL, 
      then this function will print the string pointed to by msg 
      followed by the current entity count out to the audit log file.
     void printNumEntities(const char* msg);
  */

  void setPrintDest(PrintDest nPrintDest);
  PrintDest  getPrintDest();

private:
  OdDbAuditInfoImpl* m_pImpl;
};

#include "DD_PackPop.h"

#endif


