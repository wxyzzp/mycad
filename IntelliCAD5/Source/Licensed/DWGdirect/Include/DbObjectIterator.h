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



#ifndef _ODDBOBJECTITERATOR_INCLUDED_
#define _ODDBOBJECTITERATOR_INCLUDED_

#include "RxObject.h"
#include "DbObjectId.h"

class OdDbEntity;
typedef OdSmartPtr<OdDbEntity> OdDbEntityPtr;

/** Description:
    Iterator used to navigate through the entities contained by a block.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbObjectIterator : public OdRxObject
{
public:
  ODRX_DECLARE_MEMBERS(OdDbObjectIterator);

  /** Description:
      Constructor (no arguments).
  */
  inline OdDbObjectIterator() {}

  /** Description:
      Reset the iterator's current position to the specified object in the list.

      Arguments:
      idFrom (I) Object ID of the object to which this iterator should point to.
      bForward (I) If true, this iterator will proceed forward through the list, otherwise
             it will proceed backwards.
      bSkipDeleted (I) If true, this iterator will skip deleted entities, otherwise
             it will not skip deleted entities.
  */
  virtual void start(bool bForward = true, bool bSkipDeleted = true) = 0;

  /** Description:
      Returns true if the current position of this iterator is past either end of the 
      entity list, false otherwise.
  */
  virtual bool done() const = 0;

  /** Description:
      Returns the Object ID of the entity referenced by the current position of this
      iterator.
  */
  virtual OdDbObjectId objectId() const = 0;

  /** Description:
      Opens and returns the entity referenced by the current position of this
      iterator.

      Arguments:
      openMode (I) Mode in which to open the entity.
      openErasedEntity (I) If true, open the current entity even if it is erased, 
      otherwise do not open an erased entity.

      Return Value:
      pointer to the entity if it successfully opened, otherwise NULL.
  */
  virtual OdDbEntityPtr entity(OdDb::OpenMode openMode = OdDb::kForRead, bool openErasedEntity = false) = 0;

  /** Description:
      Increments or decrements the current position of this iterator by one.

      Arguments:
      bForward (I) If true, increment the current position, otherwise decrement it.
      bSkipDeleted (I) If true, skip deleted entities, otherwise do not skip them.
  */
  virtual void step(bool bForward = true, bool bSkipDeleted = true) = 0;

  /** Description:
      Sets the current position of this iterator to the location of the specified entity.

      Arguments:
      id Object ID at of entity at which to set the current position.
  */
  virtual bool seek(OdDbObjectId id) = 0;

  /** Description:
      Sets the current position of this iterator to the location of the specified entity.

      Arguments:
      pEntity Entity at which to set the current position.
  */
  virtual bool seek(const OdDbEntity* pEntity) = 0;
};

typedef OdSmartPtr<OdDbObjectIterator> OdDbObjectIteratorPtr;

#endif //_ODDBOBJECTITERATOR_INCLUDED_


