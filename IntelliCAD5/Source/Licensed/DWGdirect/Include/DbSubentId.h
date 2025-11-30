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



#ifndef ODDB_DBSUBENT_H
#define ODDB_DBSUBENT_H

#include "DbObjectId.h"

/** Description:

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbSubentId
{
public:
  OdDbSubentId();
  OdDbSubentId(OdDb::SubentType type, OdInt32 index);

  bool operator ==(const OdDbSubentId& id) const;
  bool operator !=(const OdDbSubentId& id) const;

  OdDb::SubentType type () const;
  void setType(OdDb::SubentType);
  OdInt32 index() const;
  void setIndex(OdInt32);
  
private:
  OdDb::SubentType m_Type;
  OdInt32          m_Index;
};

/** Description:

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbFullSubentPath
{
public:
  OdDbFullSubentPath();
  OdDbFullSubentPath(OdDb::SubentType type, OdInt32 index);
  //OdDbFullSubentPath(OdDbObjectId entId, OdDb::SubentType type, OdInt32 index);
  //OdDbFullSubentPath(OdDbObjectId entId, OdDbSubentId subId);
  //OdDbFullSubentPath(OdDbObjectIdArray objectIds, OdDbSubentId subId);
  //OdDbFullSubentPath(const OdDbFullSubentPath&);
  //~OdDbFullSubentPath();

  //OdDbFullSubentPath& operator =(const OdDbFullSubentPath&);
  //bool operator ==(const OdDbFullSubentPath& id) const;
  //bool operator !=(const OdDbFullSubentPath& id) const;

  void objectIds(OdDbObjectIdArray& objectIds) const;
  OdDbObjectIdArray& objectIds();
  const OdDbSubentId subentId() const;
  OdDbSubentId& subentId();
  
private:              
  OdDbObjectIdArray m_ObjectIds;
  OdDbSubentId      m_SubentId;
};

//
// Inlines
//
inline OdDb::SubentType OdDbSubentId::type() const
{
  return m_Type;
}

inline void OdDbSubentId::setType(OdDb::SubentType type)
{
  m_Type = type;
}

inline OdInt32 OdDbSubentId::index() const
{
  return m_Index;
}

inline void OdDbSubentId::setIndex(OdInt32 index)
{
  m_Index = index;
}

inline OdDbSubentId::OdDbSubentId() : m_Type(OdDb::kNullSubentType), m_Index(0)
{}

inline OdDbSubentId::OdDbSubentId(OdDb::SubentType t, OdInt32 i) : m_Type(t), m_Index(i)
{}

inline bool OdDbSubentId::operator ==(const OdDbSubentId& id) const
{
  return ((m_Index == id.m_Index) && (m_Type == id.m_Type));
}

inline bool OdDbSubentId::operator !=(const OdDbSubentId& id) const
{
  return ((m_Index != id.m_Index) || (m_Type != id.m_Type));
}

inline OdDbFullSubentPath::OdDbFullSubentPath() 
{}

inline OdDbFullSubentPath::OdDbFullSubentPath(OdDb::SubentType type, OdInt32 index)
  : m_SubentId(type, index)
{}

inline void OdDbFullSubentPath::objectIds(OdDbObjectIdArray& objectIds) const
{
  objectIds = m_ObjectIds;
}

inline OdDbObjectIdArray& OdDbFullSubentPath::objectIds()
{
  return m_ObjectIds;
}

inline const OdDbSubentId OdDbFullSubentPath::subentId() const
{
  return m_SubentId;
}

inline OdDbSubentId& OdDbFullSubentPath::subentId()
{
  return m_SubentId;
}

#endif



