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



#ifndef OD_GECSINT_H
#define OD_GECSINT_H


class OdGeCurve3d;
class OdGeSurface;

#include "GeEntity3d.h"

#include "DD_PackPush.h"

/** Description:
    Holds data for intersections of a 3d *curve* and a *surface*.

    Remarks:
    The intersection class constructor references *curve* and *surface* objects, but the
    intersection object does not own them.  The *curve* and *surface* objects are linked to
    the intersection object.  On deletion or modification of one of them, internal
    intersection results are marked as invalid and to be re-computed.
   
    Computation of the intersection does not happen on construction or set(), but
    on demand from one of the query functions.
   
    Any output geometry from an intersection object is owned by the caller. The
    const base objects returned by curve() and surface() are not considered
    output objects.

    {group:OdGe_Classes} 
*/
class OdGeCurveSurfInt : public OdGeEntity3d
{

public:
	/** Arguments:
		crv1 (I) Any 3D *curve*.
		srf (I) Any *surface*.
		tol (I) *Tolerance* for determining intersections.
	*/
	OdGeCurveSurfInt ();

    OdGeCurveSurfInt (const OdGeCurve3d& crv, 
		const OdGeSurface& srf,
		const OdGeTol& tol = OdGeContext::gTol);

    OdGeCurveSurfInt (const OdGeCurveSurfInt& src);

	/** Description:
		Returns a pointer to the *curve*.
	*/
	const OdGeCurve3d *curve () const;

	/** Description:
		Returns a pointer to the *surface*.
	*/
    const OdGeSurface *surface () const;

	/** Description:
		Returns the *tolerance* for determining intersections.
	*/
    OdGeTol tolerance () const;

	/** Description:
		Returns the number of intersections between the *curve* and the *surface*,
		and the status of the intersection.

		Arguments:
		err (O) Status of intersection.

		Remarks:
		Possible values for err include:

		o kXXOk
		o kXXIndexOutOfRange
		o kXXWrongDimensionAtIndex
		o kXXUnknown
	*/
	int  numIntPoints (OdGeIntersectError& err) const;

	/** Description:
		Returns the 3D point representing the specified intersection,
		and the status of the intersection.

		Arguments:
		intNum (I) The zero-based index of the intersection *curve* to return.
		err (O) Status of intersection.
 
		Remarks:
		Returns NULL if the dimension of this intersection is not 0 (not a 3D point)

		Possible values for err include:

		o kXXOk
		o kXXIndexOutOfRange
		o kXXWrongDimensionAtIndex
		o kXXUnknown
	*/
	OdGePoint3d intPoint (int intNum, 
		OdGeIntersectError& err) const;


	/** Description: 
		Returns the parameters for the given intersection point with respect to the *curve* and *surface*,
		and the status of the intersection.

		Arguments:
		intNum (I) The zero-based index of the intersection *curve* to return.
		param1 (O) The parameter of the intersection point with respect to the *curve*.
		param2 (O) The parameter pair of the intersection point with respect to the *surface*.
		err (O) Status of intersection.

		Remarks:
		Possible values for err include:

		o kXXOk
		o kXXIndexOutOfRange
		o kXXWrongDimensionAtIndex
		o kXXUnknown
 	*/
    void getIntParams (int intNum,
		double& param1,
		OdGePoint2d& param2,
		OdGeIntersectError& err) const;

	/** Description: 
		Returns the intersection point as a point on the *curve*,
		and the status of the intersection.

		Arguments:
		intNum (I) The zero-based index of the intersection pooint to return.
		intPnt (O) The specified intersection point on the *curve*.
		err (O) Status of intersection.
		
		Remarks:
		Possible values for err include:

		o kXXOk
		o kXXIndexOutOfRange
		o kXXWrongDimensionAtIndex
		o kXXUnknown
 	*/
    void getPointOnCurve (int intNum, 
		OdGePointOnCurve3d& intPnt, 
		OdGeIntersectError& err) const;


	/** Description: 
		Returns the intersection point as a point on the *surface*,
		and the status of the intersection.

		Arguments:
		intNum (I) The zero-based index of the intersection point to return.
		intPnt (O) The specified intersection point on the *surface*.
		err (O) Status of intersection.

		Remarks:
		Possible values for err include:

		o kXXOk
		o kXXIndexOutOfRange
		o kXXWrongDimensionAtIndex
		o kXXUnknown
 	*/
    void getPointOnSurface (int intNum, 
		OdGePointOnSurface& intPnt, 
		OdGeIntersectError& err) const;

	/** Description: 
		Returns the configurations on either side of the *surface* at the intersection.

		Arguments:
		intNum (I) The zero-based index of the intersection *curve* to return.
		lower (O) Unknown.
		higher (O) Unknown.
		smallAngle (O) Unknown.
		err (O) Status of intersection.

		Remarks:
		Possible values for the configuratons:
		
		o kXUnknown               Unknown             Unknown error
		o kXOut                   Transverse          *curve* neighborhood is outside the *surface*
		o kXIn                    Transverse          *curve* neighborhood is inside the *surface*
		o kXTanOut                Tangent             *curve* neighborhood is outside the *surface*
		o kXTanIn                 Tangent             *curve* neighborhood is inside the *surface*
		o kXCoincident            Non-zero length     Point is on the intersection boundary
		o kXCoincidentUnbounded   Non-zero length     Point is an arbitrary point on an unbounded intersection
 
		Possible values for err include:

		o kXXOk
		o kXXIndexOutOfRange
		o kXXWrongDimensionAtIndex
		o kXXUnknown
 	*/
	void getIntConfigs (int intNum, 
		OdGe::csiConfig& lower, 
		OdGe::csiConfig& higher, 
		bool& smallAngle, 
		OdGeIntersectError& err) const;

	/** Description:
		Sets the *curve*, *surface*, and *tolerance* for which to
		determine intersections

		Arguments:		
		crv (I) Any 3D *curve*.
		srf (I) Second *surface*.
		tol (I) *Tolerance* for determining intersections.
	*/
	OdGeCurveSurfInt& set (const OdGeCurve3d& cvr,
        const OdGeSurface& srf,
        const OdGeTol& tol = OdGeContext::gTol);

    OdGeCurveSurfInt& operator = (const OdGeCurveSurfInt& crvInt);
};

#include "DD_PackPop.h"

#endif


