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



#ifndef OD_GESURF_H
#define OD_GESURF_H


#include "GeEntity3d.h"
#include "GeVector3dArray.h"
#include "GePoint2d.h"

class OdGePoint2d;
class OdGeCurve3d;
class OdGePointOnCurve3d;
class OdGePointOnSurface;
class OdGePointOnSurfaceData;
class OdGeInterval;

#include "DD_PackPush.h"

/** Description:
    The base class for all OdGe pametric surfaces.

    {group:OdGe_Classes} 
*/
class GE_TOOLKIT_EXPORT OdGeSurface : public OdGeEntity3d
{
public:

	virtual bool isKindOf (OdGe::EntityId entType) const
	{ return entType == OdGe::kSurface || OdGeEntity3d::isKindOf(entType); }

	virtual OdGe::EntityId type () const { return OdGe::kSurface; }

	/** Description:
		Returns the 2D pair of parameter values of a point.
		
		Arguments:
		pnt (I) Point to be evaluated.
		tol (I) Tolerance for determining parameters.

		Remarks:
		The returned parameters specify a point withing tol of pnt.
		If pnt is not on the surface, the results are unpredictable.
		If you are not sure the point is on the curve, use 
		OdGeSurface::isOn() instead of this function.
	*/
	virtual OdGePoint2d paramOf (const OdGePoint3d&,
		const OdGeTol& = OdGeContext::gTol) const { return OdGePoint2d(); }


	bool isOn (const OdGePoint3d& , const OdGeTol&  = OdGeContext::gTol) const
	{ 
			return false;
	}

	/** Arguments:
		ParamPoint (O) 2D pair of parameter values at the point. 
	*/
	bool isOn(const OdGePoint3d& pnt, 
		OdGePoint2d& paramPoint,
		const OdGeTol& tol = OdGeContext::gTol) const;

	/** Description:
		Returns the point on this surface closest to the specified point.

		Arguments:
		pnt (I) Any 3D point.
		tol (I) Tolerance for determining distance.
	*/
	OdGePoint3d closestPointTo (const OdGePoint3d& pnt,
		const OdGeTol& tol = OdGeContext::gTol) const;
  
	/** Description:
		Returns the point on this surface closest to the specified point.

		Arguments:
		pnt (I) Any 3D point.
		pntOnSurface (O) Closest point on surface to specified point. 
		tol (I) Tolerance for determining distance.
	*/
	void getClosestPointTo (const OdGePoint3d& pnt, 
		OdGePointOnSurface& pntOnSurface,
		const OdGeTol& tol = OdGeContext::gTol) const;
  
	/** Description:
		Returns the distance to the point on this curve closest to the specified point.

		Arguments:
		pnt (I) Any 3D point.
		tol (I) Tolerance for determining distance.
	*/
	double distanceTo (const OdGePoint3d& pnt, const OdGeTol& tol = OdGeContext::gTol) const;
  
	/** Description:
		Returns true if and only if the normal of this surface has been reversed an odd
		number of times.
	*/
	virtual bool isNormalReversed () const { return m_isNormalReversed; }

	/** Descrription:
		Reverses the normal of this surface and reurns a reference to this survace.
	*/
    virtual OdGeSurface& reverseNormal () { m_isNormalReversed = !m_isNormalReversed; return *this; }

	/** Description:
		Returns the minimal pair of parameter intervals containing the parameter
		interval of this surface.

		Arguments:
		intrvlU (O) u interval
		intrvlV (O) v interval
	*/
	virtual void getEnvelope (OdGeInterval& intrvlU, OdGeInterval& intrvlV) const = 0;
  
	/** Description:
		Returns true if and only if the survace is closed in the u direction.

		Arguments:
		tol (I) Tolerance for determining closure.
	*/	
	virtual bool isClosedInU(const OdGeTol& tol = OdGeContext::gTol) const = 0;

	/** Description:
		Returns true if and only if the survace is closed in the V direction.

		Arguments:
		tol (I) Tolerance for determining closure.
	*/	
	virtual bool isClosedInV (const OdGeTol& tol = OdGeContext::gTol) const = 0;
  
	virtual bool project(const OdGePoint3d&, OdGePoint3d&) const { return false; }

	/** Description:
		Returns the point corresponding to the parameter pair, as well as the
		derivatives and normal at that point.

		Arguments:
		param (I) The parameter pair to be evaluated.
		derivOrd (I) Maximum derivative order to be computed
		derivatives (O) An array of derivatives at the point corresponding to param. 
		normal (O) The normal at the point corresponding to param.
	*/
	virtual OdGePoint3d evalPoint(const OdGePoint2d& param) const = 0;

	virtual OdGePoint3d evalPoint(const OdGePoint2d& param, 
		int derivOrd,
		OdGeVector3dArray& derivatives) const;
   
	virtual OdGePoint3d evalPoint(const OdGePoint2d& param,
		int derivOrd,
		OdGeVector3dArray& derivatives, 
		OdGeVector3d& normal) const;

protected:
	OdGeSurface() : m_isNormalReversed(false) {}
private:
	bool          m_isNormalReversed;
};

inline OdGePoint3d OdGeSurface::evalPoint(const OdGePoint2d& /*param*/, int /*derivOrd*/,
    OdGeVector3dArray& derivatives) const {
    derivatives.clear();
    return OdGePoint3d::kOrigin;
  }

inline  OdGePoint3d OdGeSurface::evalPoint(const OdGePoint2d& /*param*/, int /*derivOrd*/,
    OdGeVector3dArray& derivatives, OdGeVector3d& normal) const {
    derivatives.clear();
    normal=OdGeVector3d::kIdentity;
    return OdGePoint3d::kOrigin;
  }

#include "DD_PackPop.h"

#endif // OD_GESURF_H


