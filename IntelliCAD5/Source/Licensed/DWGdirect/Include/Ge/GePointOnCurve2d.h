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



#ifndef OD_GE_POINT_ON_CURVE_2D_H
#define OD_GE_POINT_ON_CURVE_2D_H


#include "GePointEnt2d.h"

class OdGeCurve2d;

#include "DD_PackPush.h"

/** Description:
	Represents a *point* along a 2D *curve*

    {group:OdGe_Classes} 

	Remarks:
	The *point* representation includes its *parameter* value, its coordinates, and the
	the derivatives of the *curve* at that *point*. 
*/
class OdGePointOnCurve2d : public OdGePointEnt2d
{
public:
	/** Arguments:
		crv (I) Any 2D *curve*.
		param (I) *Parameter* to specify a *point* on crv.

		Remarks:
		The default constructor constructs a *point* on the *curve* OdGeLine2d::kXAxis with a *parameter* value of 0.0
	*/
	OdGePointOnCurve2d ();

	OdGePointOnCurve2d (const OdGeCurve2d& crv, 
		double param = 0.0);

	OdGePointOnCurve2d (const OdGePointOnCurve2d& src);
	
	OdGePointOnCurve2d& operator = (const OdGePointOnCurve2d& src);

	/** Description:
		Returns a pointer to the *curve* on which the *point* lies.
	*/
	const OdGeCurve2d* curve () const;

	/** Description:
		Returns the *parameter* value on the *curve* corresponding to the *point*.
	*/
	double parameter () const;
	
	/** Description:
		Returns the *point* on the *curve*.

		Arguments:
		param (I) Sets the current *parameter*.
		crv (I) Any 2D *curve*. Sets the current *curve*.
	*/
	OdGePoint2d point () const;

	OdGePoint2d point (double param);

	OdGePoint2d point (const OdGeCurve2d& crv, 
		double param);
	
	/** Description:
		Returns the derivitive of the *curve* at the *point* on the *curve*.

		Arguments:
		order (I) Specifies the order of the derivitive [0-2].
		param (I) Sets the current *parameter*.
		crv (I) Any 2D *curve*. Sets the current *curve*.
	*/
	OdGeVector2d deriv (int order) const;

	OdGeVector2d deriv (int order, 
		double param);

	OdGeVector2d deriv (int order, 
		const OdGeCurve2d& crv,
		double param);

	/** Description:
		Returns true if and only if the first derivative vector 
		has a length of zero.

		Arguments:
		tol (I) Tolerance for determining singularity.
	*/
	bool isSingular (const OdGeTol& tol = OdGeContext::gTol) const;


	/** Description:
		Computes the *curvature* of the *curve* at the *point* on the *curve*. Returns true 
		if and only if the first derivative vector has a non-zero length.

		Arguments:
		param (I) *parameter* to specify a *point* on crv.
		res (O) The *curvature* of the *curve* at the specified *point*.
	*/
	bool curvature (double& res);

	bool curvature (double param, 
		double& res);


	/** Description:
		Sets the current *curve*.

		Arguments:
		crv (I) Any 2D *curve*.
	*/
	OdGePointOnCurve2d& setCurve
		(const OdGeCurve2d& crv);

	/** Description:
		Sets the current *parameter*.

		Arguments:
		param (I) Sets the current *parameter*.
	*/
	OdGePointOnCurve2d& setParameter (double param);
};

#include "DD_PackPop.h"

#endif



