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



#ifndef OD_GEPONSRF_H
#define OD_GEPONSRF_H


#include "GePointEnt3d.h"

class OdGeSurface;

#include "DD_PackPush.h"

/** Description:
    
    {group:OdGe_Classes} 
*/
class OdGePointOnSurface : public OdGePointEnt3d
{
public:
	OdGePointOnSurface();
	OdGePointOnSurface(const OdGeSurface& surf);
	OdGePointOnSurface(const OdGeSurface& surf, const OdGePoint2d& param);
	OdGePointOnSurface(const OdGePointOnSurface& src);
	
	// Assignment operator.
	OdGePointOnSurface& operator =     (const OdGePointOnSurface& src);
	
	const OdGeSurface*  surface        () const;
	OdGePoint2d         parameter      () const;
	
	OdGePoint3d         point          () const;
	OdGePoint3d         point          (const OdGePoint2d& param);
	OdGePoint3d         point          (const OdGeSurface& surf,
		const OdGePoint2d& param);
	
	OdGeVector3d        normal         () const;
	OdGeVector3d        normal         (const OdGePoint2d& param);
	OdGeVector3d        normal         (const OdGeSurface& surf,
		const OdGePoint2d& param);
	OdGeVector3d        uDeriv         (int order) const;
	OdGeVector3d        uDeriv         (int order, const OdGePoint2d& param);
	OdGeVector3d        uDeriv         (int order, const OdGeSurface& surf,
		const OdGePoint2d& param);
	
	OdGeVector3d        vDeriv         (int order) const;
	OdGeVector3d        vDeriv         (int order, const OdGePoint2d& param);
	OdGeVector3d        vDeriv         (int order, const OdGeSurface& surf,
		const OdGePoint2d& param);
	
	OdGeVector3d        mixedPartial   () const;
	OdGeVector3d        mixedPartial   (const OdGePoint2d& param);
	OdGeVector3d        mixedPartial   (const OdGeSurface& surf,
		const OdGePoint2d& param);
	
	OdGeVector3d        tangentVector  (const OdGeVector2d& vec) const;
	OdGeVector3d        tangentVector  (const OdGeVector2d& vec,
		const OdGePoint2d& param);
	OdGeVector3d        tangentVector  (const OdGeVector2d& vec,
		const OdGeSurface& vecSurf,
		const OdGePoint2d& param);
	
	OdGeVector2d        inverseTangentVector  (const OdGeVector3d& vec) const;
	OdGeVector2d        inverseTangentVector  (const OdGeVector3d& vec,
		const OdGePoint2d& param);
	OdGeVector2d        inverseTangentVector  (const OdGeVector3d& vec,
		const OdGeSurface& surf,
		const OdGePoint2d& param);
	OdGePointOnSurface& setSurface     (const OdGeSurface& surf);
	OdGePointOnSurface& setParameter   (const OdGePoint2d& param);
};

#include "DD_PackPop.h"

#endif // OD_GEPONSRF_H

