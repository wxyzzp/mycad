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



#ifndef OD_GE_INTERVAL_H
#define OD_GE_INTERVAL_H

#include "DD_PackPush.h"

/** Description:
    
    {group:OdGe_Classes} 
*/
class GE_TOOLKIT_EXPORT OdGeInterval
{
public:
  OdGeInterval(double tol = 1.e-12);
  // OdGeInterval(const OdGeInterval& src);
  OdGeInterval(double lower, double upper, double tol = 1.e-12);
  OdGeInterval(bool boundedBelow, double bound, double tol = 1.e-12);
  ~OdGeInterval();
  
  // OdGeInterval&  operator =       (const OdGeInterval& otherInterval);
  
  double         lowerBound  () const;
  double         upperBound  () const;
  double         element     () const;
  void           getBounds   (double& lower, double& upper) const;
  double         length      () const;
  double         tolerance   () const;
  
  OdGeInterval&  set         (double lower, double upper);
  OdGeInterval&  set         (bool boundedBelow, double bound);
  OdGeInterval&  set         ();
  OdGeInterval&  setUpper    (double upper);
  OdGeInterval&  setLower    (double lower);
  OdGeInterval&  setTolerance(double tol);
  
  void           getMerge(const OdGeInterval& otherInterval, OdGeInterval& result) const;
  int            subtract(const OdGeInterval& otherInterval, OdGeInterval& lInterval, OdGeInterval& rInterval) const;
  bool intersectWith    (const OdGeInterval& otherInterval, OdGeInterval& result) const;
  
  inline bool isBounded     () const;
  inline bool isBoundedAbove() const;
  inline bool isBoundedBelow() const;
  inline bool isUnBounded   () const;
  inline bool isSingleton   () const;
  
  bool isDisjoint    (const OdGeInterval& otherInterval) const;
  bool contains      (const OdGeInterval& otherInterval) const;
  bool contains      (double val) const;
  
  bool isContinuousAtUpper(const OdGeInterval& otherInterval) const;
  bool isOverlapAtUpper   (const OdGeInterval& otherInterval, OdGeInterval& overlap) const;
  bool operator ==        (const OdGeInterval& otherInterval) const;
  bool operator !=        (const OdGeInterval& otherInterval) const;
  bool isEqualAtUpper     (const OdGeInterval& otherInterval) const;
  bool isEqualAtUpper     (double value) const;
  bool isEqualAtLower     (const OdGeInterval& otherInterval) const;
  bool isEqualAtLower     (double value) const;
                          
  bool isPeriodicallyOn   (double period, double& val);
                          
  friend                  
  bool operator >         (double val, const OdGeInterval& intrvl);
  bool operator >         (double val) const;
  bool operator >         (const OdGeInterval& otherInterval) const;
  friend                  
  bool operator >=        (double val, const OdGeInterval& intrvl);
  bool operator >=        (double val) const;
  bool operator >=        (const OdGeInterval& otherInterval) const;
  friend                  
  bool operator <         (double val, const OdGeInterval& intrvl);
  bool operator <         (double val) const;
  bool operator <         (const OdGeInterval& otherInterval) const;
  friend                  
  bool operator <=        (double val, const OdGeInterval& intrvl);
  bool operator <=        (double val) const;
  bool operator <=        (const OdGeInterval& otherInterval) const;
  
private:
  double m_LowerParam;
  double m_UpperParam;
  double m_Tol;
  bool   m_bBoundedAbove;
  bool   m_bBoundedBelow;
};

inline OdGeInterval::OdGeInterval(double tol)
  : m_Tol(tol)
  , m_UpperParam(0.0)
  , m_LowerParam(0.0)
  , m_bBoundedAbove(false)
  , m_bBoundedBelow(false)
{ }

inline OdGeInterval::OdGeInterval(double lower, double upper, double tol)
  : m_Tol(tol)
  , m_UpperParam(upper)
  , m_LowerParam(lower)
  , m_bBoundedAbove(true)
  , m_bBoundedBelow(true)
{ }


inline OdGeInterval::~OdGeInterval() { }

inline double OdGeInterval::length() const { return (isBounded() ? (m_UpperParam - m_LowerParam) : -1.0); }

inline bool OdGeInterval::isBounded() const { return (m_bBoundedAbove && m_bBoundedBelow); }

inline bool OdGeInterval::isSingleton() const { return (isBounded() && OdZero(length(), m_Tol)); }

inline bool OdGeInterval::isUnBounded() const { return  (!m_bBoundedAbove && !m_bBoundedBelow); }

inline bool OdGeInterval::isBoundedAbove() const { return m_bBoundedAbove; }

inline bool OdGeInterval::isBoundedBelow() const { return m_bBoundedBelow; }

inline OdGeInterval& OdGeInterval::setUpper(double upper)
{ m_UpperParam = upper; m_bBoundedAbove = true; return *this; }

inline OdGeInterval& OdGeInterval::setLower(double lower)
{ m_LowerParam = lower; m_bBoundedBelow = true; return *this; }

inline OdGeInterval& OdGeInterval::set(double lower, double upper)
{ setLower(lower); return setUpper(upper); }

inline OdGeInterval& OdGeInterval::set()
{ m_LowerParam = m_UpperParam = 0.0; m_bBoundedBelow = m_bBoundedAbove = false; return *this; }

inline OdGeInterval& OdGeInterval::setTolerance(double tol) { m_Tol = tol; return *this; }

inline double OdGeInterval::lowerBound() const { return m_LowerParam; }

inline double OdGeInterval::upperBound() const { return m_UpperParam; }

inline void OdGeInterval::getBounds(double& lower, double& upper) const
{ lower = m_LowerParam; upper = m_UpperParam; }

inline double OdGeInterval::tolerance() const { return m_Tol; }

inline bool OdGeInterval::contains(double val) const
{
  /* D.Novikov: Why it is so complicated ?
  if (m_bBoundedAbove && m_bBoundedBelow)
    return (OdCmpDouble(m_UpperParam, val, m_Tol) != -1 && OdCmpDouble(m_LowerParam, val, m_Tol) != 1);
  if (m_bBoundedAbove)
    return OdCmpDouble(m_UpperParam, val, m_Tol) != -1;
  if (m_bBoundedBelow)
    return OdCmpDouble(m_LowerParam, val, m_Tol) != 1;
  return false;
  */
  return  ((!m_bBoundedBelow || m_LowerParam - m_Tol < val)
       && (!m_bBoundedAbove || m_UpperParam + m_Tol > val));
}

#include "DD_PackPop.h"

#endif // OD_GE_INTERVAL_H


