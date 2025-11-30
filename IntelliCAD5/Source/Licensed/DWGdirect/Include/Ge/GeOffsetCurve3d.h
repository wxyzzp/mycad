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



#ifndef OD_GEOFFC3D_H
#define OD_GEOFFC3D_H

#include "GeCurve3d.h"

#include "DD_PackPush.h"

/** Description:
    
    {group:OdGe_Classes} 
*/
class GE_TOOLKIT_EXPORT OdGeOffsetCurve3d : public OdGeCurve3d
{
public:
	
	OdGeOffsetCurve3d (const OdGeCurve3d& baseCurve, const OdGeVector3d& planeNormal,
		double offsetDistance);
	OdGeOffsetCurve3d (const OdGeOffsetCurve3d& offsetCurve);
	
	const OdGeCurve3d*  curve             () const;
	OdGeVector3d        normal            () const; 
	double              offsetDistance    () const;
  bool    paramDirection    () const;
  OdGeMatrix3d    transformation    () const;
	
	OdGeOffsetCurve3d&  setCurve          (const OdGeCurve3d& baseCurve);
	OdGeOffsetCurve3d&  setNormal         (const OdGeVector3d& planeNormal);
	OdGeOffsetCurve3d&  setOffsetDistance (double offsetDistance);
	
	// Assignment operator.
	OdGeOffsetCurve3d&  operator = (const OdGeOffsetCurve3d& offsetCurve);
};

#include "DD_PackPop.h"

#endif // OD_GEOFFC3D_H

