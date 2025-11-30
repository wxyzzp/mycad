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



#ifndef OD_GE_CURVE_BOUNDARY_H
#define OD_GE_CURVE_BOUNDARY_H


class OdGeCurve2d;
class OdGeEntity3d;
class OdGePosition3d;

#include "DD_PackPush.h"

/** Description:
    Represents the geometric content typically associated with a face loop.

    Remarks:
    A face loop is represented as arrays of 3d curves(edge geometry),
    sense of edges, 2d curves(coedge geometry in the surfae parameter
    space), sense of coedges.
    In general, there is a one-to-one correspondence among all arrays that
    are present. However, certain elements of either curve arrays could
    be NULL. For example, the four tuple(curve3d, NULL, false, true)
    is legal. In this case, the boolean sense on curve 2d is may or may
    not be meaningless, depending on the symantics of the usage of this
    class(consider that in ACIS, the sense of the coedge is relative to
    the edge, and not the parameter space curve).
   
    Loop degeneracies are represented as follows.
    1. Entire loop degenerate to a single model space point, which is
    represented by the tuple(numElements = 1, position3d, curve2d).
    The curve2d may be NULL. The edge sense, and the curve2d sense are
    irrelevant. isDegenerate() method allows the dermination of this
    condition on a loop.
    2. Loop consisting of one or more model space degeneracies.
    This case is represented as the general case with those edges that are
    degenerate represented by position3d. This implies that in the
    general case, model space geometry of a curve boundary may consist of
    curve3d and/or position3d pointers. Consequently, this geometry is
    obtained by the user as entity3d pointers. The degeneracy of a
    constituent edge can be detected by the type of the model space
    geometry.
   
    This class also supports the ownership of its geometry. Being the
    owner of its geometry would cause it to remove the geometry on
    destruction of an instance of the class.
       
    {group:OdGe_Classes} 
*/
class GE_TOOLKIT_EXPORT OdGeCurveBoundary
{
public:
  OdGeCurveBoundary();
  OdGeCurveBoundary(int numberOfCurves, const OdGeEntity3d *const * crv3d,
    const OdGeCurve2d *const * crv2d, bool* orientation3d,
    bool* orientation2d, bool makeCopy = true);
  OdGeCurveBoundary(const OdGeCurveBoundary&);
  
  ~OdGeCurveBoundary();
  
  // Assignment.
  //
  OdGeCurveBoundary& operator = (const OdGeCurveBoundary& src);
  
  // Query the data.
  //
  bool isDegenerate() const;
  bool isDegenerate(OdGePosition3d& degenPoint, OdGeCurve2d** paramCurve) const;
  int numElements() const;
  void getContour(int& n, OdGeEntity3d*** crv3d,
    OdGeCurve2d*** paramGeometry,
    bool** orientation3d,
    bool** orientation2d) const;
  
  OdGeCurveBoundary& set(int numElements, const OdGeEntity3d *const * crv3d,
    const OdGeCurve2d *const * crv2d, bool* orientation3d,
    bool* orientation2d, bool makeCopy = true);
  
  // Curve ownership.
  //
  bool isOwnerOfCurves() const;
  OdGeCurveBoundary& setToOwnCurves();
  
protected:
  friend class OdGeImpCurveBoundary;
  
  OdGeImpCurveBoundary *mpImpBnd;
  int mDelBnd;
};

#include "DD_PackPop.h"

#endif // OD_GE_CURVE_BOUNDARY_H


