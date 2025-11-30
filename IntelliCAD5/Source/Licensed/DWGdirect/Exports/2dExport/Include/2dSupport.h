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

///////////////////////////////////////////////////////////////////////////////
//
// 2Support.h : for keeping some Auxiliary methods.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _2DSUPPORT_DEFINED
#define _2DSUPPORT_DEFINED

#include "OdaCommon.h"
#include "Gs.h"

#include "GeLineSeg2d.h"
#include "GePoint3d.h"



///////////////////////////////////////////////////////////
//
// Auxiliary methods
//


#define RoundDblToUInt32(X)         (OdUInt32(X+.5))

char* getUnitName(OdUInt32 idUnit);

void calculateExtents( OdGePoint3d& minExtent, 
                       OdGePoint3d& maxExtent, 
                       OdGePoint3dArray& array/*, 
                       const OdGeMatrix3d& matrix*/
                     );

///////////////////////////////////////////////////////////
//
// Geometry methods
//

bool isSegmentOutOfRect(
                const OdGeLineSeg2d& lineSeg, 
                const OdGePoint2d& ptMin, 
                const OdGePoint2d& ptMax);

bool isPtInRect(const OdGePoint2d& pt, 
                const OdGePoint2d& ptMin, 
                const OdGePoint2d& ptMax);

    
bool IsPolygonRectIntersection2d(
                OdUInt32 nPoints, 
                const OdGePoint2d* pPoints, 
                const OdGePoint2d& ptMin, 
                const OdGePoint2d& ptMax);

bool PolygonOutOfRect2d(
                OdUInt32 nPoints, 
                const OdGePoint2d* pPoints, 
                const OdGePoint2d& ptMin, 
                const OdGePoint2d& ptMax);


#endif /* _2DSUPPORT_DEFINED */

