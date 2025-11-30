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



#ifndef _SmFiler_h_Included_
#define _SmFiler_h_Included_

#include "RxObject.h"
#define STL_USING_STRING
#include "OdaSTL.h"
#include "SmObjectId.h"
#include <OdWString.h>
#include <RxVariant.h>
#include <OdStreamBuf.h>

#ifndef GUID_DEFINED
struct GUID;
#endif

namespace dst
{
  struct OdSmDatabase;
  struct OdSmObjectReference;

  /** Description:

      {group:OdSm_Classes} 
  */
  struct OdSmFiler : public OdRxObject 
  {
    virtual bool init( OdString filename, OdSmDatabase *pDb, bool bForWrite ) = 0;
    virtual void close() = 0;
    virtual OdSmDatabase* getDatabase() = 0;
    // virtual void PushCurrentWriteObject( OdSmPersist * pObj ) = 0;
    // virtual void PopCurrentWriteObject(long bSuccess ) = 0;
    virtual void pushCurrentReadObject( const OdSmPersist * pObj ) = 0;
    virtual void popCurrentReadObject( bool bSuccess ) = 0;
    virtual const OdSmPersist* getCurrentObject() const = 0;
    // virtual void GetCurrentObjectHeaderInfo( /*[out]*/ SAFEARRAY * * names, /*[out]*/ SAFEARRAY * * values ) = 0;
    // virtual long GetRevisionNumber() = 0;
    virtual OdSmPersist* readObject() = 0;
    virtual bool readProperty( OdWString& ppPropname, OdVariant& pVal ) = 0;
    virtual bool readRawData( OdStreamBuf* data ) = 0;
    virtual bool readReference( OdSmObjectReference* ) = 0;

    virtual void writeObject( const OdSmPersist* pObj ) = 0;
    virtual void writeProperty( OdWString pPropname, const OdVariant& pVal ) = 0;
    virtual void writeRawData( OdStreamBuf* p ) = 0;
    virtual bool readGuid( GUID& ) = 0;
    virtual bool readInt( long& ) = 0;
    virtual bool readDouble( double& ) = 0;
    virtual bool readString( OdWString& ) = 0;
    // virtual void ReadBytes( /*[out]*/ unsigned char * * p, /*[in,out]*/ long * count ) = 0;
    virtual void writeGuid( const GUID& id ) = 0;
    virtual void writeInt( long pInt ) = 0;
    virtual void writeDouble( double pDbl ) = 0;
    virtual void writeString( OdWString s ) = 0;
    // virtual void WriteBytes( unsigned char * p, long count ) = 0;
    virtual void writeReference( const OdSmObjectReference* ) = 0;
  };
}

#endif //_SmFiler_h_Included_
