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



#ifndef ODDBBLOCKITERATOR_H
#define ODDBBLOCKITERATOR_H

#include "RxObject.h"
#include "OdArrayPreDef.h"

class OdDbObjectId;
class OdDbFilter;
class OdDbBlockTableRecord;
class OdDbBlockIterator;
typedef OdSmartPtr<OdDbBlockIterator> OdDbBlockIteratorPtr;

/** Description

    {group:OdDb_Classes}
*/
class  TOOLKIT_EXPORT OdDbBlockIterator : public OdRxObject
{
protected: 
  OdDbBlockIterator() {}
public:
  ODRX_DECLARE_MEMBERS(OdDbBlockIterator);
  
  virtual void start() = 0;
  virtual OdDbObjectId next() = 0;
  virtual OdDbObjectId id() const = 0;
  virtual bool seek(OdDbObjectId id) = 0;

  static OdDbBlockIteratorPtr newBlockIterator(const OdDbBlockTableRecord* pBtr);

  static OdDbBlockIteratorPtr newFilteredIterator(
      const OdDbBlockTableRecord* pBtr, const OdDbFilter* pFilter);

  static OdDbBlockIteratorPtr newCompositeIterator(
      const OdDbBlockTableRecord* pBtr, const OdArray<OdSmartPtr<OdDbFilter> >& filters);

};

/** Description:

    {group:OdDb_Classes}
*/
class  TOOLKIT_EXPORT OdDbFilteredBlockIterator : public OdDbBlockIterator
{
protected: 
  OdDbFilteredBlockIterator() {}
public:
  ODRX_DECLARE_MEMBERS(OdDbFilteredBlockIterator);

  virtual double estimatedHitFraction() const = 0;

  virtual bool accepts(OdDbObjectId id) const = 0;
  virtual bool buffersForComposition() const;
  virtual void addToBuffer(OdDbObjectId id);
};

typedef OdSmartPtr<OdDbFilteredBlockIterator> OdDbFilteredBlockIteratorPtr;


#endif // ODDBBLOCKITERATOR_H


