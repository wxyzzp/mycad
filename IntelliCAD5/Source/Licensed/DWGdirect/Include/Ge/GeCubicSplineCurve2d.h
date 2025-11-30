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



#ifndef OD_GE_CUBIC_SPLINE_CURVE_2D_H
#define OD_GE_CUBIC_SPLINE_CURVE_2D_H

//
// Description:
//
// This file contains the class OdGeCubicSplineCurve2d - A mathematic
// entity used to represent a 2d interpolation cubic-spline curve.
//

class OdGePointOnCurve2d;
class OdGeCurve2dIntersection;
class OdGeInterval;
class OdGePlane;

#include "GeSplineEnt2d.h"

#include "DD_PackPush.h"

/** Description:
    
    {group:OdGe_Classes} 
*/
class GE_TOOLKIT_EXPORT OdGeCubicSplineCurve2d : public OdGeSplineEnt2d
{
public:

    OdGeCubicSplineCurve2d();
    OdGeCubicSplineCurve2d(const OdGeCubicSplineCurve2d& spline);

    // Construct a periodic cubic spline curve
    // Contract: the first fit point must be equal to the last fit point
    //
    OdGeCubicSplineCurve2d(const OdGePoint2dArray& fitPnts,
         const OdGeTol& tol = OdGeContext::gTol);

    // Construct a cubic spline curve with clamped end condition
    //
    OdGeCubicSplineCurve2d(const OdGePoint2dArray& fitPnts,
         const OdGeVector2d& startDeriv,
         const OdGeVector2d& endDeriv,
                           const OdGeTol& tol = OdGeContext::gTol);

    // Construct a cubic spline approximating the curve
    //
    OdGeCubicSplineCurve2d(const OdGeCurve2d& curve,
                           double epsilon = OdGeContext::gTol.equalPoint());

    // Construct a cubic spline curve with given fit points and 1st derivatives
    //
  OdGeCubicSplineCurve2d(const OdGeKnotVector& knots,
                         const OdGePoint2dArray& fitPnts,
             const OdGeVector2dArray& firstDerivs,
                         bool isPeriodic = false);

    // Definition of spline
    //
    int                     numFitPoints   ()        const;
    OdGePoint2d             fitPointAt     (int idx) const;
    OdGeCubicSplineCurve2d& setFitPointAt  (int idx, const OdGePoint2d& point);
    OdGeVector2d            firstDerivAt   (int idx) const;
    OdGeCubicSplineCurve2d& setFirstDerivAt(int idx, const OdGeVector2d& deriv);

    // Assignment operator.
    //
    OdGeCubicSplineCurve2d&  operator = (const OdGeCubicSplineCurve2d& spline);
};

#include "DD_PackPop.h"

#endif // OD_GE_CUBIC_SPLINE_CURVE_2D_H


