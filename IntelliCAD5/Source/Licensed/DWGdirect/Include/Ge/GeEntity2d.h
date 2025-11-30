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



#ifndef OD_GE_ENTITY_2D_H
#define OD_GE_ENTITY_2D_H


class OdGeMatrix2d;
class OdGeLine2d;

#include "GePoint2d.h"

#include "DD_PackPush.h"

/** Description:
    The base class for all OdGe 2D geometric operations.

    {group:OdGe_Classes} 
*/
class GE_TOOLKIT_EXPORT OdGeEntity2d
{
public:
	ODRX_HEAP_OPERATORS();

	virtual ~OdGeEntity2d() {}

	/** Description:
		Returns true if and only if the entity is of  *type* (or is derived from) entType.

		Arguments:
		entType (I) Entity *type* to test. 
	*/
	virtual bool isKindOf(OdGe::EntityId entType) const;

	/** Description:
		Returns the entity *type* of this entity.
	*/
	virtual OdGe::EntityId type() const = 0;

	/** Description:
		Returns a pointer to a *copy* of the entity.

		Remarks:
		The *copy* is made with the new operatior; the caller must delete the *copy*.
	*/
	virtual OdGeEntity2d* copy  () const;

	bool operator ==(const OdGeEntity2d& entity) const;

	bool operator !=(const OdGeEntity2d& entity) const;

	/** Description:
  		Returns true if and only if both entities are of the same *type*, have the same point set within the 
		specified tolerance, and have the same parameterization.

		Arguments:
		entity (I) Entity to be compared
		tol (I) Tolerance for determining equality.
	*/
	bool isEqualTo (const OdGeEntity2d& entity, 
		const OdGeTol& tol = OdGeContext::gTol) const;

	/** Description:
  		Applies the 2D transformation matrix to the entity.

		Arguments:
		xfm (I) 2D Transformation matrix.
	*/
	virtual OdGeEntity2d& transformBy(const OdGeMatrix2d& xfm);

	/** Description:
		Applies the 2D translation vector to the entity.

		Arguments:
		translateVec (I) Translation Vector.
	*/
	virtual OdGeEntity2d& translateBy(const OdGeVector2d& translateVec);

	/** Description:
		Rotates the entity about the specified point by the specified angle. 

		Arguments:
		angle (I) Rotation *angle*.
		wrtPoint (I) Base point.
	*/
	virtual OdGeEntity2d& rotateBy(double angle, 
		const OdGePoint2d& wrtPoint = OdGePoint2d::kOrigin);

	/** Description:
		Mirrors the entity about the specified 2D *line*.

		Arguments:
		line (I) Mirror *Line*.
	*/
	virtual OdGeEntity2d& mirror(const OdGeLine2d& line);

	/** Description:
		Scales the entity by the scale factor about the base point.

		Arguments:
		scaleFactor (I) Scale Factor. The scale factor must be greater than zero.
		wrtPoint (I) Base point.

	*/
	virtual OdGeEntity2d& scaleBy(double scaleFactor, 
		const OdGePoint2d& wrtPoint = OdGePoint2d::kOrigin);

	/** Description:
		Returns true if and only if the specified point is on the curve, as determined by the tolerance.

		Arguments:
		pnt	(I) Point to be queried.
		tol	(I) Tolerance when determining coincidence.
	*/
	virtual bool isOn(const OdGePoint2d& pnt, 
		const OdGeTol& tol = OdGeContext::gTol) const;

protected:
  OdGeEntity2d() {}
 
  /*:>
  OdGeImpEntity3d* mpImpEnt;
  int mDelEnt;
  OdGeEntity2d(const OdGeEntity2d&);
  OdGeEntity2d(OdGeImpEntity3d&, int);
  OdGeEntity2d(OdGeImpEntity3d*);
  OdGeEntity2d* newEntity2d(OdGeImpEntity3d*) const;
  */
};

#include "DD_PackPop.h"

#endif // OD_GE_ENTITY_2D_H


