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



#ifndef ODDB_XRECORD_H
#define ODDB_XRECORD_H

#include "DD_PackPush.h"

#define ODDB_XRECORD_CLASS          "OdDbXrecord"

#include "DbObject.h"
#include "DbFiler.h"

class OdDbXrecordIteratorImpl;

/** Description:
    The iterator for iterating through the resbuf chains within an XRecord

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbXrecordIterator : public OdRxObject
{
public:
  ODRX_DECLARE_MEMBERS(OdDbXrecordIterator);

  void start();
  bool done() const;
  bool next();
  int curRestype() const;
  
  OdResBufPtr getCurResbuf(OdDbDatabase* pDb = NULL) const;
protected:
  OdDbXrecordIterator();
  OdDbXrecordIterator(OdDbXrecordIteratorImpl* pIterImpl);
  OdDbXrecordIteratorImpl* m_pImpl;
};

typedef OdSmartPtr<OdDbXrecordIterator> OdDbXrecordIteratorPtr;


/** Description:
    Represents an XRecord object in an OdDbDatabase, which is a container object
    used to stored arbitrary drawing data.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbXrecord : public OdDbObject
{
public:
  ODDB_DECLARE_MEMBERS(OdDbXrecord);

  /** Description:
      Constructor (no arguments).
  */
  OdDbXrecord();

  /** Description:
      Returns a smart pointer to the list of code-value pairs owned by the XRecord object.

      Arguments:
      pDb (I) Database pointer required for initialization when this XRecord object
      is not database resident.
  */
  OdResBufPtr rbChain(OdDbDatabase* pDb = NULL, OdResult* pRes = NULL) const;

  /** Description:
     Returns a smart pointer to OdDbXrecordIterator object.
     This method to access data is more efficient than rbChain() especially if the data
     has large size.

      Arguments:
      pDb (I) Database pointer required for initialization when this XRecord object
      is not database resident.
  */
  OdDbXrecordIteratorPtr newIterator(OdDbDatabase* pDb = NULL) const;
  
  /** Description:
      Sets the code-value pair list for this object from the passed in list.

      Arguments:
      pRb (I) Pointer to first element of a list of code-value pairs to be assigned to
      this XRecord.
      pDb (I) TBC.
  */
  void setFromRbChain(const OdResBuf* pRb, OdDbDatabase* pDb = NULL);
  
  /** Description:
      Appends the code-value pair list to this object from the passed in list.

      Arguments:
      pRb (I) Pointer to first element of a list of code-value pairs to be appended to
      this XRecord.
      pDb (I) TBC.
  */
  void appendRbChain(const OdResBuf* pRb, OdDbDatabase* pDb = NULL);

  /** Description:
      TBC.
  */
  bool isXlateReferences() const;

  /** Description:
      TBC.
  */
  void setXlateReferences(bool translate);
  
  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;
  
  /** Description:
      Returns the duplicate record cloning flag for this XRecord (DXF 280).
  */
  virtual OdDb::DuplicateRecordCloning mergeStyle() const;

  /** Description:
      Sets the duplicate record cloning flag for this XRecord (DXF 280).
  */
  virtual void setMergeStyle(OdDb::DuplicateRecordCloning style);
  
  virtual void getClassID(void** pClsid) const;
};

typedef OdSmartPtr<OdDbXrecord> OdDbXrecordPtr;

#include "DD_PackPop.h"

#endif //ODDB_XRECORD_H


