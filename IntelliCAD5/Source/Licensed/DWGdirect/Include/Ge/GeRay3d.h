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



#ifndef OD_GERAY3D_H
#define OD_GERAY3D_H

#include "GeLinearEnt3d.h"

class OdGeRay2d;

#include "DD_PackPush.h"


/** Description:
    
    {group:OdGe_Classes} 
*/
class GE_TOOLKIT_EXPORT OdGeRay3d : public OdGeLinearEnt3d
{
public:
	OdGeRay3d();
	OdGeRay3d(const OdGeRay3d& line);
	OdGeRay3d(const OdGePoint3d& pnt, const OdGeVector3d& vec);
	OdGeRay3d(const OdGePoint3d& pnt1, const OdGePoint3d& pnt2);
	
	OdGeRay3d&     set         (const OdGePoint3d& pnt, const OdGeVector3d& vec);
	OdGeRay3d&     set         (const OdGePoint3d& pnt1, const OdGePoint3d& pnt2);
	
	// Assignment operator.
	OdGeRay3d&     operator =  (const OdGeRay3d& line);
};

#include "DD_PackPop.h"

#endif


