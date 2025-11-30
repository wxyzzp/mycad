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



#ifndef _ODDBTRANSACT_INCLUDED_
#define _ODDBTRANSACT_INCLUDED_

#include "DD_PackPush.h"

class OdDbDatabase;

#include "RxObject.h"


/** Description:
    Reactor for transaction management.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbTransactionReactor : public OdRxObject
{
public:
  ODRX_DECLARE_MEMBERS(OdDbTransactionReactor);
  
  /** Description:
      Constructor (no arguments).
  */
  OdDbTransactionReactor() {}

  virtual void transactionAboutToStart(OdDbDatabase* pTM);
  virtual void transactionStarted(OdDbDatabase* pTM);
  virtual void transactionAboutToEnd(OdDbDatabase* pTM);
  virtual void transactionEnded(OdDbDatabase* pTM);
  virtual void transactionAboutToAbort(OdDbDatabase* pTM);
  virtual void transactionAborted(OdDbDatabase* pTM);
  virtual void endCalledOnOutermostTransaction(OdDbDatabase* pTM);
};

#include "DD_PackPop.h"

#endif // _ODDBTRANSACT_INCLUDED_


