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



// (C) Copyright 2001 by Open Design Alliance. All rights reserved.

#ifndef _ODDBOBJECTID_INCLUDED_
#define _ODDBOBJECTID_INCLUDED_

#include "DD_PackPush.h"


class OdDbDatabase;
class OdDbObject;
template <class T> class OdSmartPtr;
typedef OdSmartPtr<OdDbObject> OdDbObjectPtr;

class OdDbStub;

namespace OdDb
{
  enum OpenMode
  {
    kNotOpen           = -1,
    kForRead           = 0,
    kForWrite          = 1,
    kForNotify         = 2
  };

  enum ReferenceType
  {
    kSoftPointerRef = 0,
    kHardPointerRef = 1,
    kSoftOwnershipRef = 2,
    kHardOwnershipRef = 3
  };
}

/** Description:
    A memory-resident placeholder for a database object.  
    
    Remarks:
    Database objects reference
    other database objects using Object ID's, and an actual database object pointer
    can always be obtained from a valid Object ID. The effect of this mechanism is
    that database objects do not have to reside in memory unless they are explicitly
    being examined or modified by the user.  The user must explicitly open an object
    before reading or writing to it, and should release it when the operation
    is completed.  This functionality allows DWGdirect to support partial loading of 
    a database, where Object ID's exist for all objects in the database, but 
    the actual database objects need not be loaded until they are accessed.  
    It also allows database objects that are not in use to be swapped out of memory, 
    and loaded back in when they are accessed.  Object ID's are not written out to a 
    DWG/DXF file.  If a reference must be preserved to a database object that has been 
    serialized, the object's database handle (OdDbHandle) should be used.

    See Also:
      OdDbHandle
      OdDbObject

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbObjectId
{
public:
  /** Description:
      Constructor (no arguments).
  */
  inline OdDbObjectId() : m_Id(0) { }

  /** Description:
      Constructor (OdDbStub*).
  */
  inline OdDbObjectId(OdDbStub* objId) : m_Id(objId) { }

  /** Description:
      The null Object ID.
  */
  static const OdDbObjectId kNull;

  /** Description:
      Returns true if this Object ID is null, false otherwise.
  */
  bool isNull() const;

  /** Description:
      Sets this Object ID to null.
  */
  inline void setNull() { m_Id = 0; }

  /** Description:
      Returns true if this Object ID references a valid object, false otherwise.
  */
  bool isValid() const;

  /** Description:
      Assignment operator.
  */
  inline OdDbObjectId& operator = (const OdDbObjectId& id) { m_Id = id.m_Id; return *this; }

  /** Description:
      Assignment operator.
  */
  inline OdDbObjectId& operator = (OdDbStub* objId) { m_Id = objId; return *this; }

  /** Description:
      Less than operator.
  */
  bool operator < (const OdDbObjectId&) const;

  /** Description:
      Greater than operator.
  */
  bool operator > (const OdDbObjectId&) const;

  /** Description:
      Greater than or equal to operator.
  */
  bool operator >= (const OdDbObjectId&) const;

  /** Description:
      Less than or equal to operator.
  */
  bool operator <= (const OdDbObjectId&) const;

  /** Description:
      Equality operator.
  */
  bool operator == (const OdDbObjectId& id) const;

  /** Description:
      Inequality operator.
  */
  bool operator != (const OdDbObjectId& id) const;

  /** Description:
      Returns true if this Object ID is 0, false otherwise.
  */
  bool operator !() const;

  /** Description:
      Internal use only.
  */
  inline operator OdDbStub*() const { return m_Id; }

  /** Description:
      Internal use only.
  */
  inline OdDbStub* operator ->() const { return (OdDbStub*)m_Id; }

  /** Description:
      Returns the database that this Object ID is associated with, or 0 if it is not associated
      with any database.
  */
  OdDbDatabase* database() const;

  /** Description:
      TBC.
  */
  OdDbDatabase* originalDatabase() const;

  /** Description:
      TBC.
  */
  void convertToRedirectedId();

  /** Description:
      TBC.
  */
  bool isErased() const;

  /** Description:
      TBC.
  */
  bool isEffectivelyErased() const;

  /** Description:
      TBC.
  */
  bool objectLeftOnDisk() const;

  /** Description:
      Returns the database handle value of the database object referenced by this Object ID,
      or a null handle if no database object is referenced.
  */
  const OdDbHandle& getHandle() const;

  /** Description:
      TBC.
  */
  const OdDbHandle& getNonForwardedHandle() const;

  /** Description:
      Opens the database object associated with this Object ID, in the specified mode.

      Arguments:
      mode (I) Mode in which to open the object (one of OdDb::kForRead, OdDb::kForWrite,
      or OdDb::kForNotify).
      openErasedObject (I) If true, open the object even if it has been erased, otherwise
      erased objects will not be opened.  False by default.

      Return Value:
      A smart pointer to the opened database object.

      Remarks:
      The mode parameter can take on the following values:
      @list OdDb::kForRead - Allows operations that do not modify the object.  A database
      object can be opened in this mode any number of times (simultaneously), as long as
      it has not been already opened in OdDb::kForWrite or OdDb::kForNotify mode.  If any type
      of write operation is attempted on an object opened in this mode, an exception
      will be thrown.
      @list OdDb::kForWrite - Allows read and write operations to be performed on the
      database object.  A database object can be opened in this mode only if it is not
      already open (in any mode).
      @list OdDb::kForNotify - Object is opened for notification purposes.  A database
      object can be opened in this mode as long as it is not already open in kForNotify mode.
  */
  OdDbObjectPtr openObject(OdDb::OpenMode mode = OdDb::kForRead, bool openErasedObject = false) const;

  OdDbObjectPtr safeOpenObject(OdDb::OpenMode mode = OdDb::kForRead, bool openErasedObject = false) const;

  void bindObject(OdDbObject* pObj);

protected:
  friend class OdDbStub;
  OdDbStub* m_Id;
};

/** Description:
    A specialization of OdDbObjectId that indicates a hard owner relationship.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbHardOwnershipId : public OdDbObjectId
{
public:
  /** Description:
      Constructor.
  */
  inline OdDbHardOwnershipId() {}

  /** Description:
      Constructor.
  */
  inline OdDbHardOwnershipId(const OdDbObjectId& id);

  /** Description:
      Constructor.
  */
  inline OdDbHardOwnershipId(OdDbStub* objId);

  /** Description:
      Assignment operator.
  */
  inline OdDbHardOwnershipId& operator = (const OdDbObjectId& id);

  /** Description:
      Assignment operator.
  */
  inline OdDbHardOwnershipId& operator = (OdDbStub* objId);

  /** Description:
      Inequality operator.
  */
  bool operator != (const OdDbObjectId&) const;

  /** Description:
      Inequality operator.
  */
  bool operator != (OdDbStub*) const;

  /** Description:
      Equality operator.
  */
  bool operator == (const OdDbObjectId&) const;

  /** Description:
      Equality operator.
  */
  bool operator == (OdDbStub*) const;

};

/** Description:
    A specialization of OdDbObjectId that indicates a soft owner relationship.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbSoftOwnershipId : public OdDbObjectId
{
public:
  /** Description:
      Constructor
  */
  inline OdDbSoftOwnershipId() {}

  /** Description:
      Constructor
  */
  inline OdDbSoftOwnershipId(const OdDbObjectId& id);

  /** Description:
      Constructor.
  */
  inline OdDbSoftOwnershipId(OdDbStub* objId);

  /** Description:
      Assignment operator.
  */
  inline OdDbSoftOwnershipId& operator = (const OdDbObjectId& id);

  /** Description:
      Assignment operator.
  */
  inline OdDbSoftOwnershipId& operator = (OdDbStub* objId);

  /** Description:
      Inequality operator.
  */
  bool operator != (const OdDbObjectId&) const;

  /** Description:
      Inequality operator.
  */
  bool operator != (OdDbStub*) const;

  /** Description:
      Equality operator.
  */
  bool operator == (const OdDbObjectId&) const;

  /** Description:
      Equality operator.
  */
  bool operator == (OdDbStub*) const;

};

/** Description:
    A specialization of OdDbObjectId that indicates a hard pointer relationship.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbHardPointerId : public OdDbObjectId
{
public:
  /** Description:
      Constructor.
  */
  OdDbHardPointerId() {}

  /** Description:
      Constructor (OdDbObjectId&).

      Arguments:
      id (I) OdDbObjectId which is converted to an OdDbHardPointerId.
  */
  OdDbHardPointerId(const OdDbObjectId& id);

  /** Description:
      Constructor (OdDbStub*).
  */
  OdDbHardPointerId(OdDbStub*);

  /** Description:
      Assignment operator.
  */
  OdDbHardPointerId& operator =(const OdDbHardPointerId&);

  /** Description:
      Assignment operator.
  */
  OdDbHardPointerId& operator =(const OdDbObjectId&);

  /** Description:
      Assignment operator.
  */
  OdDbHardPointerId& operator =(OdDbStub*);

  /** Description:
      Inequality operator.
  */
        bool operator != (const OdDbObjectId&) const;

  /** Description:
      Inequality operator.
  */
  bool operator != (OdDbStub*) const;

  /** Description:
      Equality operator.
  */
  bool operator == (const OdDbObjectId&) const;

  /** Description:
      Equality operator.
  */
  bool operator == (OdDbStub*) const;
};

/** Description:
    A specialization of OdDbObjectId that indicates a soft pointer relationship.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbSoftPointerId : public OdDbObjectId
{
public:
  /** Description:
      Constructor.
  */
  OdDbSoftPointerId() {}

  /** Description:
      Constructor.

      Arguments:
      id (I) OdDbObjectId which is converted to an OdDbSoftPointerId.
  */
  OdDbSoftPointerId(const OdDbObjectId& id);

  /** Description:
      Constructor.
  */
  OdDbSoftPointerId(OdDbStub*);

  /** Description:
      Assignment operator.
  */
  OdDbSoftPointerId& operator =(const OdDbSoftPointerId&);

  /** Description:
      Assignment operator.
  */
  OdDbSoftPointerId& operator =(const OdDbObjectId&);

  /** Description:
      Assignment operator.
  */
  OdDbSoftPointerId& operator =(OdDbStub*);

  /** Description:
      Inequality operator.
  */
  bool operator != (const OdDbObjectId&) const;

  /** Description:
      Inequality operator.
  */
  bool operator != (OdDbStub*) const;

  /** Description:
      Equality operator.
  */
  bool operator == (const OdDbObjectId&) const;

  /** Description:
      Equality operator.
  */
  bool operator == (OdDbStub*) const;

};


////////////////////// OdDbObjectId inlines ////////////////////

////////////////////// OdDbHardOwnershipId inlines ////////////////////

inline
OdDbHardOwnershipId::OdDbHardOwnershipId(const OdDbObjectId& id)
: OdDbObjectId(id) {}

inline
OdDbHardOwnershipId::OdDbHardOwnershipId(OdDbStub* objId)
: OdDbObjectId(objId) {}

inline
OdDbHardOwnershipId& OdDbHardOwnershipId::operator = (const OdDbObjectId& id)
{ OdDbObjectId::operator=(id); return *this; }

inline
OdDbHardOwnershipId& OdDbHardOwnershipId::operator = (OdDbStub* objId)
{ m_Id = objId; return *this; }

inline
bool OdDbHardOwnershipId::operator != (const OdDbObjectId& id) const
{ return OdDbObjectId::operator!=(id); }

inline
bool OdDbHardOwnershipId::operator != (OdDbStub* objId) const
{ return OdDbObjectId::operator!=(objId); }

inline
bool OdDbHardOwnershipId::operator == (const OdDbObjectId& id) const
{ return OdDbObjectId::operator==(id); }

inline
bool OdDbHardOwnershipId::operator == (OdDbStub* objId) const
{ return OdDbObjectId::operator==(objId); }


///////////////////// OdDbSoftOwnershipId inlines ////////////////////

inline
OdDbSoftOwnershipId::OdDbSoftOwnershipId(const OdDbObjectId& id)
: OdDbObjectId(id) {}

inline
OdDbSoftOwnershipId::OdDbSoftOwnershipId(OdDbStub* objId)
: OdDbObjectId(objId) {}

inline
OdDbSoftOwnershipId& OdDbSoftOwnershipId::operator = (const OdDbObjectId& id)
{ OdDbObjectId::operator=(id); return *this; }

inline
OdDbSoftOwnershipId& OdDbSoftOwnershipId::operator = (OdDbStub* objId)
{ m_Id = objId; return *this; }

inline
bool OdDbSoftOwnershipId::operator != (const OdDbObjectId& id) const
{ return OdDbObjectId::operator!=(id); }

inline
bool OdDbSoftOwnershipId::operator != (OdDbStub* objId) const
{ return OdDbObjectId::operator!=(objId); }

inline
bool OdDbSoftOwnershipId::operator == (const OdDbObjectId& id) const
{ return OdDbObjectId::operator==(id); }

inline
bool OdDbSoftOwnershipId::operator == (OdDbStub* objId) const
{ return OdDbObjectId::operator==(objId); }


///////////////////// OdDbHardPointerId inlines ////////////////////

inline
OdDbHardPointerId::OdDbHardPointerId(const OdDbObjectId& id)
: OdDbObjectId(id) {}

inline
OdDbHardPointerId::OdDbHardPointerId(OdDbStub* objId)
: OdDbObjectId(objId) {}

inline
OdDbHardPointerId& OdDbHardPointerId::operator = (const OdDbHardPointerId& id)
{ OdDbObjectId::operator=(id); return *this; }

inline
OdDbHardPointerId& OdDbHardPointerId::operator = (const OdDbObjectId& id)
{ OdDbObjectId::operator=(id); return *this; }

inline
OdDbHardPointerId& OdDbHardPointerId::operator = (OdDbStub* objId)
{ m_Id = objId; return *this; }

inline
bool OdDbHardPointerId::operator != (const OdDbObjectId& id) const
{ return OdDbObjectId::operator!=(id); }

inline
bool OdDbHardPointerId::operator != (OdDbStub* objId) const
{ return OdDbObjectId::operator!=(objId); }

inline
bool OdDbHardPointerId::operator == (const OdDbObjectId& id) const
{ return OdDbObjectId::operator==(id); }

inline
bool OdDbHardPointerId::operator == (OdDbStub* objId) const
{ return OdDbObjectId::operator==(objId); }



//////////////////// OdDbSoftPointerId inlines ////////////////////

inline
OdDbSoftPointerId::OdDbSoftPointerId(const OdDbObjectId& id)
: OdDbObjectId(id) {}

inline
OdDbSoftPointerId::OdDbSoftPointerId(OdDbStub* objId)
: OdDbObjectId(objId) {}

inline
OdDbSoftPointerId& OdDbSoftPointerId::operator = (const OdDbSoftPointerId& id)
{ OdDbObjectId::operator=(id); return *this; }

inline
OdDbSoftPointerId& OdDbSoftPointerId::operator = (const OdDbObjectId& id)
{ OdDbObjectId::operator=(id); return *this; }

inline
OdDbSoftPointerId& OdDbSoftPointerId::operator = (OdDbStub* objId)
{ m_Id = (OdDbStub*)objId; return *this; }

inline
bool OdDbSoftPointerId::operator != (const OdDbObjectId& id) const
{ return OdDbObjectId::operator!=(id); }

inline
bool OdDbSoftPointerId::operator != (OdDbStub* objId) const
{ return OdDbObjectId::operator!=(objId); }

inline
bool OdDbSoftPointerId::operator == (const OdDbObjectId& id) const
{ return OdDbObjectId::operator==(id); }

inline
bool OdDbSoftPointerId::operator == (OdDbStub* objId) const
{ return OdDbObjectId::operator==(objId); }


#include "DD_PackPop.h"

#endif //_ODDBOBJECTID_INCLUDED_


