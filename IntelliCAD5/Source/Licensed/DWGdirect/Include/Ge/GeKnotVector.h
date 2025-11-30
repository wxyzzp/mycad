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



#ifndef OD_GE_KNOT_VECTOR
#define OD_GE_KNOT_VECTOR

#include "GeDoubleArray.h"
#include "GeIntArray.h"

class OdGeInterval;

#include "DD_PackPush.h"

/** Description:
    
    {group:OdGe_Classes} 
*/
class GE_TOOLKIT_EXPORT OdGeKnotVector
{
public:
	inline OdGeKnotVector(double eps = 1.e-9);
//	OdGeKnotVector(int size, int growSize, double eps = 1.e-9);
	inline OdGeKnotVector(int size, const double [], double eps = 1.e-9);
	OdGeKnotVector(int plusMult, const OdGeKnotVector& src);
	inline OdGeKnotVector(const OdGeKnotVector& src);
	inline OdGeKnotVector(const OdGeDoubleArray& src, double eps = 1.e-9);
	
//	~OdGeKnotVector();
	
	inline OdGeKnotVector&     operator =  (const OdGeKnotVector& src);
	inline OdGeKnotVector&     operator =  (const OdGeDoubleArray& src);
	inline double&             operator [] (int);
	inline double              operator [] (int) const;
	
	bool      isEqualTo (const OdGeKnotVector& other) const;
	
	double              startParam         () const;
	double              endParam           () const;
	int                 multiplicityAt     (int i) const;
	int                 numIntervals       () const;
	
	int                 getInterval        (int ord, double par, OdGeInterval& interval) const;
	void                getDistinctKnots   (OdGeDoubleArray& knots, OdGeIntArray *multiplicity = NULL) const;

	bool      contains           (double param) const;
	bool      isOn               (double knot) const;
	
	OdGeKnotVector&     reverse            ();
	OdGeKnotVector&     removeAt           (int);
	OdGeKnotVector&     removeSubVector    (int startIndex, int endIndex);
	
	OdGeKnotVector&     insertAt           (int indx, double u, int multiplicity = 1);
	OdGeKnotVector&     insert             (double u);

  int                 append             (double val);
	OdGeKnotVector&     append             (OdGeKnotVector& tail, double knotRatio = 0.);
	int                 split              (double par,
		OdGeKnotVector* pKnot1,
		int multLast,
		OdGeKnotVector* pKnot2,
		int multFirst) const;
	
	
	OdGeKnotVector&     setRange           (double lower, double upper);
	
	double              tolerance          () const;
	OdGeKnotVector&     setTolerance       (double tol);
	
	int                 length             () const;
  bool      isEmpty            () const;
	int                 logicalLength      () const;
	OdGeKnotVector&     setLogicalLength   (int n);
//	int                 physicalLength     () const;
//	OdGeKnotVector&     setPhysicalLength  (int);
	
//	int                 growLength  () const;
//	OdGeKnotVector&     setGrowLength(int);
	
	const double*       getPtr      () const;
	const double*       asArrayPtr  () const;
	double*             asArrayPtr  ();
	
	OdGeKnotVector&     set (int size, const double [], double eps = 1.e-9);
	
protected:
  OdGeDoubleArray    m_Data;
	double             m_Tolerance;
	
	inline bool     isValid (OdUInt32) const;
};


////////////////////////////////////////////////////
inline OdGeKnotVector::OdGeKnotVector(double eps) : m_Tolerance(eps) {}

inline OdGeKnotVector::OdGeKnotVector(int size, const double vals[], double eps)
  : m_Tolerance(eps)
{
	m_Data.resize(size);
	for (int i = 0; i < size; i++)
  {
		m_Data[i] = vals[i];
  }
}

inline OdGeKnotVector::OdGeKnotVector(const OdGeKnotVector& src)
    : m_Data(src.m_Data)
    , m_Tolerance(src.m_Tolerance)
{}

inline OdGeKnotVector::OdGeKnotVector(const OdGeDoubleArray& src, double eps)
    : m_Data(src)
    , m_Tolerance(eps)
{}


inline 	OdGeKnotVector& OdGeKnotVector::operator = (const OdGeKnotVector& src)
{
  m_Data = src.m_Data;
  m_Tolerance = src.m_Tolerance;
  return *this;
}

inline 	OdGeKnotVector& OdGeKnotVector::operator = (const OdGeDoubleArray& src)
{
  m_Data = src;
  return *this;
}

inline double& OdGeKnotVector::operator [] (int i)
{ ODA_ASSERT(isValid(i)); return m_Data[i]; }

inline double OdGeKnotVector::operator [] (int i) const
{ ODA_ASSERT(isValid(i)); return m_Data[i]; }

inline double OdGeKnotVector::startParam() const
{ return m_Data.first();}

inline double OdGeKnotVector::endParam() const
{ return m_Data.last();}


inline bool OdGeKnotVector::contains(double param) const
{
  int last = m_Data.size() - 1;
  return (last < 0) ? false : (param >= m_Data[0] - m_Tolerance && param <= m_Data[last] + m_Tolerance);
}

inline int OdGeKnotVector::append(double val)
{
  m_Data.append(val);
  return m_Data.size();
}

inline double OdGeKnotVector::tolerance() const
{ return m_Tolerance;}

inline OdGeKnotVector& OdGeKnotVector::setTolerance(double eps)
{ m_Tolerance = eps;  return *this;}

inline bool OdGeKnotVector::isValid(OdUInt32 i) const
{ return i < m_Data.size(); }

inline int OdGeKnotVector::length() const
{ return m_Data.size();}

inline bool OdGeKnotVector::isEmpty() const
{ return length() == 0;}

inline int OdGeKnotVector::logicalLength() const
{ return m_Data.size();}

inline OdGeKnotVector& OdGeKnotVector::setLogicalLength(int n)
{
  m_Data.resize(n);
  return *this;
}

inline const double * OdGeKnotVector::getPtr() const { return m_Data.asArrayPtr(); }
inline const double * OdGeKnotVector::asArrayPtr() const { return m_Data.asArrayPtr(); }
inline double* OdGeKnotVector::asArrayPtr() { return m_Data.asArrayPtr(); }
	
inline OdGeKnotVector& OdGeKnotVector::set (int size, const double val[], double eps)
{
  m_Data.assign(val, val + size);
  m_Tolerance = eps;
  return *this;
}

#include "DD_PackPop.h"

#endif // OD_GE_KNOT_VECTOR

