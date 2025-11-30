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



#ifndef OD_GE_EXTERNAL_CURVE_2D_H
#define OD_GE_EXTERNAL_CURVE_2D_H

#include "DD_PackPush.h"

class OdGeNurbCurve2d;

/** Description:
	Represents 2D geometry defined externally from GeLib.
    
    {group:OdGe_Classes} 
*/
class OdGeExternalCurve2d : public OdGeCurve2d
{
public:
	/** Arguments:
		curveDef (I) Pointer to the curve definition.
		curveKind (I) Information about system that created the curve.
		makeCopy (I) Makes a copy of curveDef.

		Remarks:
		Without arguments, the constructor creates an empty instance.

		Possible values for curveKind:

		o kAcisEntity
		o kExternalEntityUndefined
	*/
	OdGeExternalCurve2d();
	OdGeExternalCurve2dv(const OdGeExternalCurve2d&);
	OdGeExternalCurve2d (void* curveDef, 
		OdGe::ExternalEntityKind curveKind,
		bool makeCopy = true);

	/**Description:
		Returns true, and an an OdGeNurbCurve2d curve,
		if and only if the curve can be represented as an OdGeNurbCurve2d curve.

		Arguments:
		nurbCurve (O) An OdGeNurbCurve2d internal representation of the external curve.

		Remarks:
		nurbCurve is created with the new method, and must be deleted by the caller.
	*/
	bool isNurbCurve () const;
	bool isNurbCurve (OdGeNurbCurve2d& nurbCurve) const;

	/** Description:
		Returns true if and only if the curve is defined (not an empty instance).
	*/
	bool isDefined () const;


	/** Description:
		Returns a pointer to a copy ofthe raw curve definition.

		Arguments:
		curveDef (O) A pointer to a copy of the raw curve definition.

		Remarks:
		It is up to the caller to delete the memory allocated.
	*/
	void getExternalCurve (void*& curveDef) const;


	/** Description:
		Rerurns information about the system that created the curve.

		Remarks:
		Possible values for curveKind:

		o kAcisEntity
		o kExternalEntityUndefined
	*/
	OdGe::ExternalEntityKind externalCurveKind () const;

	/* Description:
		Sets the parameters for this external curve according to the arguments, 
		and returns a reference to this external curve.

		Arguments:
		curveDef (I) Pointer to the curve definition.
		curveKind (I) Information about system that created the curve.
		makeCopy (I) Makes a copy of curveDef.
	*/
	OdGeExternalCurve2d& set(void* curveDef, 
		OdGe::ExternalEntityKind curveKind,
        bool makeCopy = true);

	OdGeExternalCurve2d& operator = (const OdGeExternalCurve2d& src);

	/** Description:
		Returns true if and only if if the external curve owns the data.

		Remarks:
		If the external curve owns the data, it will be destroyed when
		the curve is destroyed.
	*/
	bool isOwnerOfCurve () const;

	/** Description:
		Forces this external curve to own the data, and returns a pointer to this object.

		Remarks:
		If the external curve owns the data, it will be destroyed when
		the curve is destroyed.
	*/
	OdGeExternalCurve2d& setToOwnCurve();
};

#include "DD_PackPop.h"

#endif // OD_GE_EXTERNAL_CURVE_2D_H

