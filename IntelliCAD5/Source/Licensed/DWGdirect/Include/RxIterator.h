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



#ifndef _ODRXITERATOR_H_
#define _ODRXITERATOR_H_

#include "RxObject.h"

/** Description:
    Abstract base class for certain DWGdirect iterators.  
    An iterator can be used to traverse the elements in a collection.  

    {group:OdRx_Classes} 
*/
class FIRSTDLL_EXPORT OdRxIterator : public OdRxObject
{
public:

  /** Description:
      Constructor (no arguments).
  */
  OdRxIterator() {}

  ODRX_DECLARE_MEMBERS(OdRxIterator);
  
  /** Description:
      Returns true if there is a valid element at the "current position" of this iterator,
      otherwise false (indicating either that the collection is empty, or all elements in the
      collection have been traversed.
  */
  virtual bool done() const = 0;

  /** Description:
      Increments the "current position" of this iterator to the next element in the collection.

      Return Value:
      True if the increment was successfull, false otherwise (indicating there are no
      more elements to traverse).
  */
  virtual bool next() = 0;

  /** Description:
      Returns a smart pointer to the current object pointed to by this iterator. 
  */
  virtual OdRxObjectPtr object() const = 0;
};

typedef OdSmartPtr<OdRxIterator> OdRxIteratorPtr;

#endif // _ODRXITERATOR_H_


