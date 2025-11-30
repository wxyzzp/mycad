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



#ifndef OD_GEGBL_H
#define OD_GEGBL_H


#include "GeTol.h"

class OdGePoint3d;
#include "Ge.h"
#include "OdArrayPreDef.h"
#include "GePoint3dArray.h"

class OdGeVector3d;
class OdGePlane;
class OdGeCurve3d;
class OdGeNurbCurve2d;
class OdGeNurbCurve3d;

#include "DD_PackPush.h"

/** Description:
    
    {group:OdGe_Classes} 
*/

struct GE_TOOLKIT_EXPORT OdGeContext
{
  static  OdGeTol   gTol;
  static  OdGeTol   gZeroTol;
  
  static void  (*gErrorFunc) ();
  
  static void  (*gOrthoVector)(const OdGeVector3d&, OdGeVector3d&);
  
  static void* (*gAllocMem)(unsigned long);
  static void  (*gFreeMem)(void*);
};


////////////////////////////////////////////////////////////////////////////////
// Calculates normal to the plane containing given points.
// Returns:
//  eOk if points are in plane - normal is calculated
//  eNonCoplanarGeometry
//  eDegenerateGeometry - all points are on the line
//  eSingularPoint  - points coincide
//
//    pNormal may be NULL
//
GE_TOOLKIT_EXPORT OdGeError geCalculateNormal(const OdGePoint3dArray& pts, OdGeVector3d * pNormal,
                           const OdGeTol& tol = OdGeContext::gTol);

GE_TOOLKIT_EXPORT OdGeError geCalculateNormal(const OdGePoint3d *pts, OdUInt32 num, OdGeVector3d * pNormal,
                           const OdGeTol& tol = OdGeContext::gTol);

// Same with previous functions, but returns result as plane
GE_TOOLKIT_EXPORT OdGeError geCalculatePlane(const OdGePoint3dArray& pts, OdGePlane& plane,
  const OdGeTol& tol = OdGeContext::gTol);

GE_TOOLKIT_EXPORT OdGeError geCalculatePlane(const OdGePoint3d *pts, OdUInt32 num, OdGePlane& plane,
  const OdGeTol& tol = OdGeContext::gTol);

// Same with previous functions, but work with enum of ordered closed loop of curves. 
// Supported OdGeCircArc3d, OdGeEllipArc3d, OdGeNurbCurve3d, OdGeLineSeg3d
GE_TOOLKIT_EXPORT OdGeError geCalculatePlane(const OdGeCurve3d * const* pts, OdUInt32 num, OdGePlane& plane,
  const OdGeTol& tol = OdGeContext::gTol);

GE_TOOLKIT_EXPORT bool geNurb3dTo2d(const OdGeNurbCurve3d &nurb3d, OdGePlane& plane, OdGeNurbCurve2d &nurb2d,
  const OdGeTol& tol = OdGeContext::gTol);

#include "DD_PackPop.h"

#endif // OD_GEGBL_H


