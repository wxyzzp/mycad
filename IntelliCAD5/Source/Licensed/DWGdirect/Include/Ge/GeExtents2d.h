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



#ifndef _ODGEEXTENTS2D_INCLUDED_
#define _ODGEEXTENTS2D_INCLUDED_


#include "GePoint2d.h"
#include "GeVector2d.h"
#include "GeMatrix2d.h"



#define INVALIDEXTENTS 1.0e20

/** Description: 
    Represents a 2d bounding box, represented as a minimum 2d point
    and a maximum 2d point.
    
    {group:OdGe_Classes} 
*/
class GE_TOOLKIT_EXPORT OdGeExtents2d
{
public:

  /** Description:
      Constructor (no arguments).
  */
  inline OdGeExtents2d()
    : m_min(INVALIDEXTENTS,  INVALIDEXTENTS)
    , m_max(-INVALIDEXTENTS, -INVALIDEXTENTS)
  {}

  /** Description:
      Constructor that takes min and max point values.
  */
  inline OdGeExtents2d(const OdGePoint2d& min, const OdGePoint2d& max) { set(min, max); }

  /** Description:
      Returns the minimum point of this extent box.
  */
  inline const OdGePoint2d& minPoint() const { return m_min; }

  /** Description:
      Returns the maximum point of this extent box.
  */
  inline const OdGePoint2d& maxPoint() const { return m_max; }

  /** Description:
      Sets the minimum and maximum points for this extent box.
  */
  inline void set(const OdGePoint2d& min, const OdGePoint2d& max) { m_min = min; m_max = max; }

  /** Description:
      Updates the extents of this object with the specified point.
  */
  void addPoint(const OdGePoint2d& pt);

  /** Description:
      Updates the extents of this object with the specified extents object.
  */
  void addExt(const OdGeExtents2d& src);

  /** Description:
      Returns true if this extents object contains valid extents (max point values are all
      greater than the corresponding min point values), false otherwise.
  */
  bool isValidExtents() const;

  /** Description:
      Transforms this object by the passed in matrix.
  */
  inline void transformBy(const OdGeMatrix2d& mat);

  inline void expandBy(const OdGeVector2d& vec);

  inline bool isDisjoint(const OdGeExtents2d& extents) const;
  inline bool contains(const OdGePoint2d& pt) const;
  inline bool contains(const OdGeExtents2d& ext) const;

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
  IntersectionStatus intersectWith(const OdGeExtents2d& other, OdGeExtents2d* pResult = 0) const;

private:
  OdGePoint2d m_min;
  OdGePoint2d m_max;
};

inline bool OdGeExtents2d::isValidExtents() const
{
  return  ((m_max.x >= m_min.x) && (m_max.y >= m_min.y));
}

inline void OdGeExtents2d::addPoint(const OdGePoint2d& pt)
{
  m_max.x = odmax(pt.x, m_max.x);
  m_max.y = odmax(pt.y, m_max.y);
  m_min.x = odmin(pt.x, m_min.x);
  m_min.y = odmin(pt.y, m_min.y);
}

inline void OdGeExtents2d::addExt(const OdGeExtents2d& src)
{
  if(src.isValidExtents())
  {
    addPoint(src.minPoint());
    addPoint(src.maxPoint());
  }
}

inline void OdGeExtents2d::transformBy(const OdGeMatrix2d& mat)
{
  OdGeVector2d vecX(OdGeVector2d::kXAxis * (m_max.x - m_min.x)),
               vecY(OdGeVector2d::kYAxis * (m_max.y - m_min.y));

  if(isValidExtents())
  {
    m_max = m_min = (mat * m_min);
    expandBy(mat * vecX);
    expandBy(mat * vecY);
  }
}

inline void OdGeExtents2d::expandBy(const OdGeVector2d& vec)
{
  if(isValidExtents())
  {
    OdGePoint2d p1 = m_min, p2 = m_max;
    p1 += vec;
    p2 += vec;
    addPoint(p1);
    addPoint(p2);
  }
}

inline bool OdGeExtents2d::contains(const OdGePoint2d& point) const
{
  return  ( point.x >= m_min.x  &&  point.y >= m_min.y  && 
            point.x <= m_max.x  &&  point.y <= m_max.y );
}

inline bool OdGeExtents2d::isDisjoint(const OdGeExtents2d& ext) const
{
  return  (ext.m_min.x >     m_max.x || ext.m_min.y > m_max.y     ||
               m_min.x > ext.m_max.x ||     m_min.y > ext.m_max.y);
}

inline bool OdGeExtents2d::contains(const OdGeExtents2d& ext) const
{
  return  (ext.m_min.x >=     m_min.x && ext.m_min.y >= m_min.y     &&
               m_max.x >= ext.m_max.x &&     m_max.y >= ext.m_max.y);
}

#undef INVALIDEXTENTS

#endif //_ODGEEXTENTS2D_INCLUDED_


