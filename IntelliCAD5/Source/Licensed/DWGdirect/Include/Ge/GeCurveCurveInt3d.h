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



#ifndef OD_GECINT3D_H
#define OD_GECINT3D_H



class OdGeCurve3d;

#include "GeEntity3d.h"

#include "DD_PackPush.h"

/** Description:
    This class holds the intersection *set* of two 3D curves.

	Remarks:
    The intersection class constructor references curve objects, but the
    intersection object does not own them.  The surface objects are linked to the
    intersection object.  On deletion or modification of one of them, internal
    intersection results are marked as invalid and to be re-computed.
   
    Computation of the intersection does not happen on construction or set(), but
    on demand from one of the query functions.
   
    Any output geometry from an intersection object is owned by the caller.  The
    const base objects returned by curve1() and curve2() are not considered
    output objects.

    {group:OdGe_Classes} 
*/
class OdGeCurveCurveInt3d : public OdGeEntity3d
{

public:
	/** Arguments:
		curve1 (I) First 3D curve.
		curve2 (I) Second 3D curve.
		planeNormal (I) Specifies the normal to the plane for getIntConfigs(). 
		range1 (I) Range of first curve.
		range2 (I) Range of second curve.
		tol (I) *Tolerance* for determining intersections.

		Remarks:
		The planeNormal argument is used only with getIntConfigs(). 
		You must supply this argument if and only if you are using getIntConfigs().
	*/
    OdGeCurveCurveInt3d  ();

    OdGeCurveCurveInt3d (const OdGeCurve3d& curve1, 
		const OdGeCurve3d& curve2,
        const OdGeVector3d& planeNormal = OdGeVector3d::kIdentity,
        const OdGeTol& tol = OdGeContext::gTol);

    OdGeCurveCurveInt3d (const OdGeCurve3d& curve1, 
		const OdGeCurve3d& curve2,
        const OdGeInterval& range1, 
		const OdGeInterval& range2,
        const OdGeVector3d& planeNormal=OdGeVector3d::kIdentity,
        const OdGeTol& tol = OdGeContext::gTol);

    OdGeCurveCurveInt3d (const OdGeCurveCurveInt3d& src);

	/** Description:
		Returns a pointer to the first curve.
	*/
    const OdGeCurve3d *curve1 () const;

	/** Description:
		Returns a pointer to the second curve.
	*/
    const OdGeCurve3d  *curve2 () const;

	/** Description:
		Returns the ranges of the two curves.

		Arguments:
		range1 (O) Range of the first curve.
		range2 (O) Range of the second curve.
	*/
    void getIntRanges (OdGeInterval& range1,
         OdGeInterval& range2) const;

	/** Description:
		Returns the normal to the plane for getIntConfigs()
	*/
	OdGeVector3d planeNormal () const;

	/** Description:
		Returns the *tolerance* for determining intersections.
	*/
    OdGeTol tolerance () const;

	/** Description:
		Returns the number of intersections between the curves within the specified ranges.
	*/
    int numIntPoints () const;

	/** Description:
		Returns the specified intersection point.

		Arguments:
		intNum (I) The zero-based index of the intersection point to return.
	*/
    OdGePoint3d intPoint (int intNum) const;

	/** Description:
		Returns the curve parameters at the specified intersection point.

		Arguments:
		intNum (I) The zero-based index of the intersection point to return.
		param1 (O) The parameter of the first curve at the intersection point.
		param2 (O) The parameter of the second curve at the intersection point.
	*/
    void getIntParams (int intNum,
		double& param1, 
		double& param2) const;

	/** Description:
		Returns the specified intersection point as point on the first curve.

		Arguments:
		intNum (I) The zero-based index of the intersection point to return.
		intPnt (O) The specified intersection point on the first curve.
	*/
    void getPointOnCurve1 (int intNum, 
		OdGePointOnCurve3d& intPnt) const;

	/** Description:
		Returns the specified intersection point as point on the second curve.

		Arguments:
		intNum (I) The zero-based index of the intersection point to return.
		intPnt (O) The specified intersection point on the second curve.
	*/
    void getPointOnCurve2 (int intNum, 
		OdGePointOnCurve3d& IntPnt) const;


	/** Description:
		Returns the configuration of the intersecion point of
		the first curve with respect to the second, and vice versa.

		Arguments:
		intNum (I) The zero-based index of the intersection point to query.
		config1wrt2 (O) The configuration of the first curve with respect to the second.
		config2wrt1 (O) The configuration of the second curve with respect to the first.

		Remarks:
		The curves must be coplanar, and are with respect to the planeNormal.
		The configuration types may be found in Ge.h
	*/
    void getIntConfigs (int intNum, 
		OdGe::OdGeXConfig& config1wrt2,
		OdGe::OdGeXConfig& config2wrt1) const;

	/** Description:
		Returns true if and only if the curves are tangential at specified intersection point 

		Arguments:
		intNum (I) The zero-based index of the intersection point to query.
	*/
    bool isTangential (int intNum) const;

	/** Description:
		Returns true if and only if the curves are transversal (cross) at specified intersection point 

		Arguments:
		intNum (I) The zero-based index of the intersection point to query.
	*/
    bool isTransversal (int intNum) const;

	/** Description:
		Returns the *tolerance* used to compute the specified intersection point. 

		Arguments:
		intNum (I) The zero-based index of the intersection point to query.
	*/
    double intPointTol (int intNum) const;

	/** Description:
		Returns the number of intervals of overlap for the two curves. 
	*/
    int overlapCount () const;

	/** Description:
		Returns true if and only if the curves are oriented in the same direction
		where they overlap. 
		
		Remarks:
		This value has meaning only if overlapCount() > 0
	*/
	bool overlapDirection () const;

	/** Description:
		Returns the ranges for each curve a the specified overlap. 

		Arguments:
		overlapNum (I) The zero-based index of the overlap range to query.
		range1 (O) The range of the first curve for the specified overlap.
		range2 (O) The range of the second curve for the specified overlap.
	*/
    void getOverlapRanges (int overlapNum,
        OdGeInterval& range1,
        OdGeInterval& range2) const;

	/** Description:
		Makes the first curve the second, and vice versa.
	*/
	void changeCurveOrder (); 
        
	/** Description:
		Orders the intersection points so they correspond to
		increasing parameter values of the first curve.
	*/
    OdGeCurveCurveInt3d& orderWrt1 ();

	/** Description:
		Orders the intersection points so they correspond to
		increasing parameter values of the second curve.
	*/
    OdGeCurveCurveInt3d& orderWrt2 ();
    
	/** Description:
		Sets the curves, ranges, plane normals, and *tolerance* for which to
		determine intersections

		Arguments:
		curve1 (I) First 3D curve.
		curve2 (I) Second 3D curve.
		planeNormal (I) Specifies the normal to the plane for getIntConfigs(). 
		range1 (I) Range of first curve.
		range2 (I) Range of second curve.
		tol (I) *Tolerance* for determining intersections.

		Remarks:
		The planeNormal argument is used only with getIntConfigs(). 
		You must supply this argument if and only if you are using getIntConfigs().
	*/
    OdGeCurveCurveInt3d& set(const OdGeCurve3d& curve1,
		const OdGeCurve3d& curve2,
		const OdGeVector3d& planeNormal = OdGeVector3d::kIdentity,
		const OdGeTol& tol = OdGeContext::gTol);

    OdGeCurveCurveInt3d& set (const OdGeCurve3d& curve1,
		const OdGeCurve3d& curve2,
		const OdGeInterval& range1,
		const OdGeInterval& range2,
		const OdGeVector3d& planeNormal = OdGeVector3d::kIdentity,
		const OdGeTol& tol = OdGeContext::gTol);

    OdGeCurveCurveInt3d& operator = (const OdGeCurveCurveInt3d& src);
};

#include "DD_PackPop.h"

#endif // OD_GECINT3D_H


