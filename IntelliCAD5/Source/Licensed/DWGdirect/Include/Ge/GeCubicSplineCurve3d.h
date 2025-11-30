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



#ifndef OD_GECSPL3D_H
#define OD_GECSPL3D_H


class OdGePointOnCurve3d;
class OdGeCurve3dIntersection;
class OdGeInterval;
class OdGePlane;

#include "GeSplineEnt3d.h"

#include "DD_PackPush.h"

/** Description:
    A mathematical entity used to represent a 3d interpolation 
    cubic-spline curve.

    {group:OdGe_Classes} 
*/
class GE_TOOLKIT_EXPORT OdGeCubicSplineCurve3d : public OdGeSplineEnt3d
{
public:

    OdGeCubicSplineCurve3d();
    OdGeCubicSplineCurve3d(const OdGeCubicSplineCurve3d& spline);

    // Construct a periodic cubic spline curve
    // Contract: the first fit point must be equal to the last fit point
    //
    OdGeCubicSplineCurve3d(const OdGePoint3dArray& fitPnts,
         const OdGeTol& tol = OdGeContext::gTol);

    // Construct a cubic spline curve with clamped end condition
    //
    OdGeCubicSplineCurve3d(const OdGePoint3dArray& fitPnts,
         const OdGeVector3d& startDeriv,
         const OdGeVector3d& endDeriv,
                           const OdGeTol& tol = OdGeContext::gTol);

    // Construct a cubic spline approximating the curve
    //
    OdGeCubicSplineCurve3d(const OdGeCurve3d& curve,
                           double epsilon = OdGeContext::gTol.equalPoint());

    // Construct a cubic spline curve with given fit points and 1st derivatives
    //
    OdGeCubicSplineCurve3d(const OdGeKnotVector& knots,
                     const OdGePoint3dArray& fitPnts,
         const OdGeVector3dArray& firstDerivs,
                     bool isPeriodic = false);

    // Definition of spline
    //
    int                     numFitPoints   ()        const;
    OdGePoint3d             fitPointAt     (int idx) const;
    OdGeCubicSplineCurve3d& setFitPointAt  (int idx, const OdGePoint3d& point);
    OdGeVector3d            firstDerivAt   (int idx) const;
    OdGeCubicSplineCurve3d& setFirstDerivAt(int idx, const OdGeVector3d& deriv);

    // Assignment operator.
    //
    OdGeCubicSplineCurve3d&  operator = (const OdGeCubicSplineCurve3d& spline);
};

#include "DD_PackPop.h"

#endif


