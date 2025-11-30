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



#ifndef OD_GEPLIN3D_H
#define OD_GEPLIN3D_H


#include "GeSplineEnt3d.h"

#include "DD_PackPush.h"


/** Description:
    
    {group:OdGe_Classes} 
*/
class GE_TOOLKIT_EXPORT OdGePolyline3d : public OdGeSplineEnt3d
{
public:
	
	OdGePolyline3d();
	OdGePolyline3d(const OdGePolyline3d& src);
	OdGePolyline3d(const OdGePoint3dArray& points);
	OdGePolyline3d(const OdGeKnotVector& knots,
		const OdGePoint3dArray& cntrlPnts);
	
	OdGePolyline3d(const OdGeCurve3d& crv, double apprEps);
	
	int              numFitPoints  () const;
	OdGePoint3d      fitPointAt   (int idx) const;
	OdGeSplineEnt3d& setFitPointAt(int idx, const OdGePoint3d& point);
	
	// Assignment operator.
	OdGePolyline3d& operator =     (const OdGePolyline3d& pline);
};

#include "DD_PackPop.h"

#endif // OD_GEPLIN3D_H

