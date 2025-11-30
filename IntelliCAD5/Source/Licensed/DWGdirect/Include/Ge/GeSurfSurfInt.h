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



#ifndef OD_GESSINT_H
#define OD_GESSINT_H


#include "GeEntity3d.h"

class OdGeCurve3d;
class OdGeCurve2d;
class OdGeSurface;

#include "DD_PackPush.h"

/** Description:
    This class holds the intersection data of two surfaces.

    Remarks:
    The intersection class constructor references surface objects, but the
    intersection object does not own them.  The surface objects are linked to the
    intersection object.  On deletion or modification of one of them, internal
    intersection results are marked as invalid and to be re-computed.
   
    Computation of the intersection does not happen on construction or set(), but
    on demand from one of the query functions.
   
    Any output geometry from an intersection object is owned by the caller.  The
    const base objects returned by surface1() and surface2() are not considered
    output objects.
    
    {group:OdGe_Classes} 
*/
class OdGeSurfSurfInt  : public OdGeEntity3d
{

public:

	/** Arguments:
		srf1 (I) First surface.
		srf2 (I) Second surface.
		tol (I) Tolerance for determining intersections.
	*/
	OdGeSurfSurfInt ();

    OdGeSurfSurfInt (const OdGeSurface& srf1,
        const OdGeSurface& srf2,
        const OdGeTol& tol = OdGeContext::gTol ;

    OdGeSurfSurfInt (const OdGeSurfSurfInt& src);

	/** Description:
		Returns a pointer to the first surface.
	*/
	const OdGeSurface *surface1 () const;

	/** Description:
		Returns a pointer to the second surface.
	*/
    const OdGeSurface *surface2 () const;
	
	/** Description:
		Returns the tolerance for determining intersections.
	*/
	OdGeTol tolerance () const;

	/** Description:
		Returns the number of intersections between the two surfaces,
		and the status of the intersections.

		Arguments:
		err (O) Status of intersection.

		Remarks:
		Possible values for err are as follows:
		
		o kXXOk						???
		o kXXIndexOutOfRange			???
		o kXXWrongDimensionAtIndex	???
		o kXXUnknown					???
	*/
	int numResults (OdGeIntersectError& err) const;

  	/** Description:
		Returns a pointer to the the 3D curve representing the specified intersection
		and the status of the intersection.

		Arguments:
		intNum (I) The zero-based index of the intersection curve to return.
		isExternal (I) ???
		err (O) Status of intersection.
 
		Remarks:
		Possible values for err are as follows:
		
		o kXXOk						???
		o kXXIndexOutOfRange			???
		o kXXWrongDimensionAtIndex	???
		o kXXUnknown					???
	*/
    OdGeCurve3d*   intCurve (int intNum, bool isExternal, OdGeIntersectError& err) const; 


	/** Description: 
		Returns a pointer to the the 2D curve representing the specified intersection
		and the status of the intersection.

		Arguments:
		intNum (I) The zero-based index of the intersection curve to return.
		isExternal (I) ???
		isFirst (I) If true, returns the curve on the first surface, otherwise it the curve on the second surface.
		err (O) Status of intersection.
 
		Remarks:
		Returns NULL if the dimension of  this intersection is not 1 (not a 2d curve)

		Possible values for err are as follows:

		o kXXOk						???
		o kXXIndexOutOfRange			???
		o kXXWrongDimensionAtIndex	???
		o kXXUnknown					???
	*/
	OdGeCurve2d* intParamCurve(int intNum, 
		bool isExternal, 
		bool isFirst, 
		OdGeIntersectError& err) const;

	/** Description: 
		Returns the 3D point representing the specified intersection,
		and the status of the intersection.

		Arguments:
		intNum (I) The zero-based index of the intersection curve to return.
		err (O) Status of intersection.
 
		Remarks:
		Returns NULL if the dimension of this intersection is not 0 (not a 3D point)

		Possible values for err are as follows:
		
		o kXXOk						???
		o kXXIndexOutOfRange			???
		o kXXWrongDimensionAtIndex	???
		o kXXUnknown					???
	*/
	OdGePoint3d intPoint (int intNum, 
		OdGeIntersectError& err) const;
	

	/** Description: 
		Returns the parameter pairs for the given intersection point with respect to each surface,
		and the status of the intersection.

		Arguments:
		intNum (I) The zero-based index of the intersection curve to return.
		param1 (O) The parameter pair of the intersection point with respect to the first curve.
		param2 (O) The parameter pair of the intersection point with respect to the second curve.
		err (O) Status of intersection.

		Possible values for err are as follows:

		o kXXOk						???
		o kXXIndexOutOfRange			???
		o kXXWrongDimensionAtIndex	???
		o kXXUnknown					???
 	*/
    void getIntPointParams (int intNum,
        OdGePoint2d& param1, 
		OdGePoint2d& param2, 
		OdGeIntersectError& err) const;

	/** Description: 
		Returns the configurations on either side of the intersection each surface.

		Arguments:
		intNum (I) The zero-based index of the intersection curve to return.
		surf1Left (O) Configuration for surface 1 on the left side of the intersection.
		surf1Right (O) Configuration for surface 1 on the right side of the intersection.
		surf2Left (O) Configuration for surface 2 on the left side of the intersection.
		surf2Right (O) Configuration for surface 2 on the right side of the intersection.
		err (O) Status of intersection.

		Possible values for the configuratons are as follows:
		
		o kSSIUnknown
		o kSSIOut			Neighborhood is outside the surface.
		o kSSIIn			Neighborhood is inside the surface.
		o kSSICoincident  Non-zero area intersection.
 
		Possible values for err are as follows:
		
		o kXXOk						???
		o kXXIndexOutOfRange			???
		o kXXWrongDimensionAtIndex	???
		o kXXUnknown					???
 	*/
	void getIntConfigs (int intNum, OdGe::ssiConfig& surf1Left,  OdGe::ssiConfig& surf1Right, 
              OdGe::ssiConfig& surf2Left,  OdGe::ssiConfig& surf2Right,  
              OdGe::ssiType& intType, int& dim, OdGeIntersectError& err) const; 

	
	/** Description:
		Description Pending.

		Arguments:
		intNum (I) The zero-based index of the intersection curve to return.
		err (O) Status of intersection.

		Possible values for err are as follows:
		
		o kXXOk						???
		o kXXIndexOutOfRange			???
		o kXXWrongDimensionAtIndex	???
		o kXXUnknown					???
	*/
	int getDimension (int intNum, 
		OdGeIntersectError& err) const;

	/** Description:
		Returns the type of the specified intersection.

		Arguments:
		intNum (I) The zero-based index of the intersection curve to return.
		err (O) Status of intersection.

		Possible retrurns values for the configuratons are as follows:

		Possible values for err are as follows:

		o kXXOk						???
		o kXXIndexOutOfRange			???
		o kXXWrongDimensionAtIndex	???
		o kXXUnknown					???
	*/
	OdGe::ssiType getType(int intNum, 
		OdGeIntersectError& err) const;

	/** Descripion:
		Sets the surfaces and tolerances whose intersection data is to be determines.
		Returns a reference to this SurfSurfInt.
		
		Arguments:
		srf1 (I) First surface.
		srf2 (I) Second surface.
		tol (I) Tolerance for determining intersections.
	*/
    OdGeSurfSurfInt& set (const OdGeSurface& srf1,
        const OdGeSurface& srf2,
        const OdGeTol& tol = OdGeContext::gTol);

    OdGeSurfSurfInt& operator = (const OdGeSurfSurfInt& crvInt);
};

#include "DD_PackPop.h"

#endif // OD_GESSINT_H

