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



#ifndef OD_GE_OFFSET_CURVE_2D_H
#define OD_GE_OFFSET_CURVE_2D_H

#include "GeCurve2d.h"

#include "DD_PackPush.h"

/** Description:
    
    {group:OdGe_Classes} 
*/
class GE_TOOLKIT_EXPORT OdGeOffsetCurve2d : public OdGeCurve2d
{
public:
	OdGeOffsetCurve2d (const OdGeCurve2d& baseCurve, double offsetDistance);
	OdGeOffsetCurve2d (const OdGeOffsetCurve2d& offsetCurve);
	
	const OdGeCurve2d*  curve             () const;
	double              offsetDistance    () const;
  bool    paramDirection    () const;
  OdGeMatrix2d    transformation    () const;
	
	OdGeOffsetCurve2d&  setCurve          (const OdGeCurve2d& baseCurve);
	OdGeOffsetCurve2d&  setOffsetDistance (double distance);
	
	// Assignment operator.
	OdGeOffsetCurve2d&  operator = (const OdGeOffsetCurve2d& offsetCurve);
};

#include "DD_PackPop.h"

#endif // OD_GE_OFFSET_CURVE_2D_H


