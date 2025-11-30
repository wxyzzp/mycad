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



#ifndef _OD_GEENTITY3D_H_
#define _OD_GEENTITY3D_H_


#include "GeGbl.h"
#include "GePoint3d.h"

#include "DD_PackPush.h"

class OdGeMatrix3d;
class OdGePlane;
class OdGeEntity3dImpl;

/** Description:
    The base class for all OdGe 3D geometric operations.
    
    {group:OdGe_Classes} 
*/
class GE_TOOLKIT_EXPORT OdGeEntity3d
{
public:
	ODRX_HEAP_OPERATORS();

	virtual ~OdGeEntity3d() {}
  
  // Run time type information.

	/** Description:
		Returns true if and only if the entity is of *type* (or is derived from) entType.

		Arguments:
		entType (I) Entity *type* to test. 
	*/
	 virtual bool isKindOf(OdGe::EntityId entType) const;

	/** Description:
		Returns the entity *type*.
	*/
	virtual OdGe::EntityId type() const;

	/** Description:
		Returns a pointer to a *copy* of the entity.

		Remarks:
		The *copy* is made with the new operatior; the caller must delete the *copy*.
	*/
	virtual OdGeEntity3d* copy() const;

	bool operator ==(const OdGeEntity3d& entity) const;

	bool operator !=(const OdGeEntity3d& entity) const;

	/** Description:
  		Returns true if and only if both entities are of the same *type*, have the same point set within the 
		specified tolerance, and have the same parameterization.

		Arguments:
		entity (I) Entity to be compared.
		tol (I) Tolerance for determining equality.
	*/
	bool isEqualTo(const OdGeEntity3d& entity, 
		const OdGeTol& tol = OdGeContext::gTol) const;

	/** Description:
		Applies the 3D transformation matrix to the entity. 
		
		Arguments:
		xfm (I) 3D Transformation matrix.
	*/
	virtual OdGeEntity3d& transformBy(const OdGeMatrix3d& xfm);

	/** Description:
		Applies the 3D translation vector to the entity. 

		Arguments:
		translateVec (I) Translation Vector.
	*/
	virtual OdGeEntity3d& translateBy(const OdGeVector3d& translateVec);

	/** Description:
		Rotates the entity by the specified *angle* about the axis
		defined by the point and the vector. 

		Arguments:
		angle (I) Rotation *angle*.
		vec (I) Vector about which entitiy is rotated.	
		wrtPoint (I) Base point.
	*/
	virtual OdGeEntity3d& rotateBy(double angle, const OdGeVector3d& vec,
		const OdGePoint3d& wrtPoint = OdGePoint3d::kOrigin);

	/** Description:
		Mirrors the entity about the specified plane.

		Arguments:
		plane (I) Plane about which entity is to be mirrored.
	*/
	virtual OdGeEntity3d& mirror(const OdGePlane& plane);

	/** Description:
		Scales the entity by the scale factor about the base point.

		Arguments:
		scaleFactor (I) Scale Factor. Must be greater than 0.
		wrtPoint (I) Base point.
	*/
	virtual OdGeEntity3d& scaleBy(double scaleFactor,
		const OdGePoint3d& wrtPoint = OdGePoint3d::kOrigin);

	/** Description:
		Returns true if and only if the specified point is on the curve, 
		as determined by the tolerance.

		Arguments:
		pnt	(I) Point to be queried.
		tol	(I) Tolerance for determining coincidence.
	*/
	virtual bool isOn(const OdGePoint3d& pnt,
		const OdGeTol& tol = OdGeContext::gTol) const;
};

#include "DD_PackPop.h"

#endif //_OD_GEENTITY3D_H_


