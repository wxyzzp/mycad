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
// programs incorporating this software must include the following statment 
// with their copyright notices:
//
//      DWGdirect © 2002 by Open Design Alliance Inc. All rights reserved. 
//
// By use of this software, you acknowledge and accept the terms of this 
// agreement.
///////////////////////////////////////////////////////////////////////////////



#ifndef _ODGEEXTENTS3D_INCLUDED_
#define _ODGEEXTENTS3D_INCLUDED_


#include "GePoint3d.h"
#include "GeVector3d.h"
#include "GeMatrix3d.h"



#define INVALIDEXTENTS 1.0e20

/** Description:
    Represents a 3d bounding box, represented as a minimum 3d point
    and a maximum 3d point.

    {group:OdGe_Classes} 
*/
class GE_TOOLKIT_EXPORT OdGeExtents3d
{
public:

  /** Description:
      Constructor (no arguments).
  */
  inline OdGeExtents3d()
    : m_min(INVALIDEXTENTS,  INVALIDEXTENTS,  INVALIDEXTENTS)
    , m_max(-INVALIDEXTENTS, -INVALIDEXTENTS, -INVALIDEXTENTS)
  {}

  /** Description:
      Constructor that takes min and max point values.
  */
  inline OdGeExtents3d(const OdGePoint3d& min, const OdGePoint3d& max) { set(min, max); }

  /** Description:
      Returns the minimum point of this extent box.
  */
  inline const OdGePoint3d& minPoint() const { return m_min; }

  /** Description:
      Returns the maximum point of this extent box.
  */
  inline const OdGePoint3d& maxPoint() const { return m_max; }

  /** Description:
      Sets the minimum and maximum points for this extent box.
  */
  inline void set(const OdGePoint3d& min, const OdGePoint3d& max) { m_min = min; m_max = max; }

  /** Description:
      Updates the extents of this object with the specified point.
  */
  inline void addPoint(const OdGePoint3d& pt);

  /** Description:
      Updates the extents of this object with the specified extents object.
  */
  inline void addExt(const OdGeExtents3d& src);

  /** Description:
      Returns true if this extents object contains valid extents (max point values are all
      greater than the corresponding min point values), false otherwise.
  */
  inline bool isValidExtents() const;

  inline void expandBy(const OdGeVector3d& vec);

  /** Description:
      Transforms this object by the passed in matrix.
  */
  inline void transformBy(const OdGeMatrix3d& mat);

  inline bool contains(const OdGePoint3d& point) const;

  inline bool contains(const OdGeExtents3d& ext) const;

  inline bool isDisjoint(const OdGeExtents3d& ext) const;

  /** Description:
     
  */
  enum IntersectionStatus
  {
    kIntersectUnknown,// either or both extents are invalid
    kIntersectNot,    // extents are NOT intersecting
    kIntersectOpIn,   // operand is completely within this extents
    kIntersectOpOut,  // this extents is completely within operand
    kIntersectOk      // extents are intersecting, result is returned
  };
  IntersectionStatus intersectWith(const OdGeExtents3d& other, OdGeExtents3d* pResult = 0) const;

protected:
  OdGePoint3d m_min;
  OdGePoint3d m_max;
};

inline bool OdGeExtents3d::isValidExtents() const
{
  return  ((m_max.x >= m_min.x) && (m_max.y >= m_min.y) && (m_max.z >= m_min.z));
}

inline void OdGeExtents3d::addPoint(const OdGePoint3d& pt)
{
  m_max.x = odmax(pt.x, m_max.x);
  m_max.y = odmax(pt.y, m_max.y);
  m_max.z = odmax(pt.z, m_max.z);
  m_min.x = odmin(pt.x, m_min.x);
  m_min.y = odmin(pt.y, m_min.y);
  m_min.z = odmin(pt.z, m_min.z);
}

inline void OdGeExtents3d::addExt(const OdGeExtents3d& src)
{
  if(src.isValidExtents())
  {
    addPoint(src.minPoint());
    addPoint(src.maxPoint());
  }
}

inline void OdGeExtents3d::expandBy(const OdGeVector3d& vec)
{
  if(isValidExtents())
  {
    OdGePoint3d p1 = m_min, p2 = m_max;
    p1 += vec;
    p2 += vec;
    addPoint(p1);
    addPoint(p2);
  }
}

inline void OdGeExtents3d::transformBy(const OdGeMatrix3d& mat)
{
  OdGeVector3d vecX(OdGeVector3d::kXAxis * (m_max.x - m_min.x)),
               vecY(OdGeVector3d::kYAxis * (m_max.y - m_min.y)),
               vecZ(OdGeVector3d::kZAxis * (m_max.z - m_min.z));

  if(isValidExtents())
  {
    m_max = m_min = (mat * m_min);
    expandBy(mat * vecX);
    expandBy(mat * vecY);
    expandBy(mat * vecZ);
  }
}

inline bool OdGeExtents3d::contains(const OdGePoint3d& point) const
{
  return  ( point.x >= m_min.x  &&  point.y >= m_min.y  &&  point.z >= m_min.z  &&
            point.x <= m_max.x  &&  point.y <= m_max.y  &&  point.z <= m_max.z );
}

inline bool OdGeExtents3d::isDisjoint(const OdGeExtents3d& ext) const
{
  return  (ext.m_min.x >     m_max.x || ext.m_min.y >     m_max.y || ext.m_min.z >     m_max.z||
               m_min.x > ext.m_max.x ||     m_min.y > ext.m_max.y ||     m_min.z > ext.m_max.z);
}

inline bool OdGeExtents3d::contains(const OdGeExtents3d& ext) const
{
  return  (ext.m_min.x >=     m_min.x && ext.m_min.y >=     m_min.y && ext.m_min.z >=     m_min.z &&
               m_max.x >= ext.m_max.x &&     m_max.y >= ext.m_max.y &&     m_max.z >= ext.m_max.z);
}


#undef INVALIDEXTENTS

#endif //_ODGEEXTENTS3D_INCLUDED_


