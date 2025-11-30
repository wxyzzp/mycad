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
// DwfSupport.h : the auxiliary methods for helping to write into DWF File
//
///////////////////////////////////////////////////////////////////////////////

#ifndef DWFSUPPORT_DEFINED
#define DWFSUPPORT_DEFINED

#include "whiptk/whip_toolkit.h"

#include "OdaCommon.h"
#include "Gs.h"

#include "GeLineSeg2d.h"
#include "GePoint3d.h"


///////////////////////////////////////////////////////////
//
// Auxiliary methods
//

#define RoundDoubleToWTInteger32(X) (WT_Integer32(long(X+.5)))
#define RoundDoubleToWTInteger16(X) (WT_Integer16(short(X+.5)))


inline void RoundPoint(WT_Logical_Point& to, const OdGePoint2d& from)
{
  OdGePoint2d point(from);

  to.m_x = RoundDoubleToWTInteger32(point.x);
  to.m_y = RoundDoubleToWTInteger32(point.y);
}

void boxAnticlockwise(WT_Logical_Point* bounds, 
                      const OdGePoint3d& ptMin, 
                      const OdGePoint3d& ptMax);

void boxClockwise(WT_Logical_Point* bounds, 
                  const OdGePoint3d& ptMin, 
                  const OdGePoint3d& ptMax);


#endif /* DWFSUPPORT_DEFINED */

