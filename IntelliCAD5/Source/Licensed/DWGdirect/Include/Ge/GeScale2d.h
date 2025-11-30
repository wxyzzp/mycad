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



#ifndef OD_GESCL2D_H
#define OD_GESCL2D_H


class OdGeMatrix2d;
class OdGeScale3d;

#include "DD_PackPush.h"

/** Description:
    
    {group:OdGe_Classes} 
*/
class OdGeScale2d
{
public:
	OdGeScale2d();
	OdGeScale2d(const OdGeScale2d& src);
	OdGeScale2d(double factor);
	OdGeScale2d(double xFactor, double yFactor);
	
	static const   OdGeScale2d kIdentity;
	
	OdGeScale2d    operator *  (const OdGeScale2d& sclVec) const;
	OdGeScale2d&   operator *= (const OdGeScale2d& scl);
	OdGeScale2d&   preMultBy   (const OdGeScale2d& leftSide);
	OdGeScale2d&   postMultBy  (const OdGeScale2d& rightSide);
	OdGeScale2d&   setToProduct(const OdGeScale2d& sclVec1, const OdGeScale2d& sclVec2);
	OdGeScale2d    operator *  (double s) const;
	OdGeScale2d&   operator *= (double s);
	OdGeScale2d&   setToProduct(const OdGeScale2d& sclVec, double s);
	friend
    OdGeScale2d    operator *  (double, const OdGeScale2d& scl);
	
	OdGeScale2d    inverse        () const;
	OdGeScale2d&   invert         ();
	
	bool isProportional(const OdGeTol& tol = OdGeContext::gTol) const;
	
	bool operator == (const OdGeScale2d& sclVec) const;
	bool operator != (const OdGeScale2d& sclVec) const;
	bool isEqualTo   (const OdGeScale2d& scaleVec,
		const OdGeTol& tol = OdGeContext::gTol) const;
	
	double         operator [] (unsigned int i) const;
	double&        operator [] (unsigned int i);
	OdGeScale2d&   set         (double sc0, double sc1);
	
	operator       OdGeMatrix2d   () const;
	void           getMatrix      (OdGeMatrix2d& mat) const;
	OdGeScale2d&   extractScale   (const OdGeMatrix2d& mat);
	OdGeScale2d&   removeScale    (OdGeMatrix2d& mat);
	
	operator       OdGeScale3d    () const;
	
	double         sx, sy;
};

inline double
OdGeScale2d::operator [] (unsigned int i) const
{
	return * (&sx + i);
}

inline double&
OdGeScale2d::operator [] (unsigned int i)
{
	return * (&sx + i);
}

#include "DD_PackPop.h"

#endif // OD_GESCL2D_H


