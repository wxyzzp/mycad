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



#ifndef OD_GEAPLN3D_H
#define OD_GEAPLN3D_H

class OdGeKnotVector;
class OdGeVector3dArray;

#include "GePolyline3d.h"
#include "GePoint3d.h"

#include "DD_PackPush.h"

/** Description:
    
    {group:OdGe_Classes} 
*/
class OdGeAugPolyline3d : public OdGePolyline3d
{
public:
  OdGeAugPolyline3d();
  OdGeAugPolyline3d(const OdGeAugPolyline3d& apline);
  OdGeAugPolyline3d(const OdGeKnotVector& knots,
    const OdGePoint3dArray& cntrlPnts,
    const OdGeVector3dArray& vecBundle);
  OdGeAugPolyline3d(const OdGePoint3dArray& cntrlPnts,
    const OdGeVector3dArray& vecBundle);
  
  OdGeAugPolyline3d(const OdGeCurve3d& curve,
    double fromParam, double toParam, 
    double apprEps);
  
  OdGeAugPolyline3d& operator = (const OdGeAugPolyline3d& apline);
  
  OdGePoint3d           getPoint(int idx) const;
  OdGeAugPolyline3d&    setPoint(int idx, OdGePoint3d pnt);
  void                  getPoints(OdGePoint3dArray& pnts) const;
  
  OdGeVector3d          getVector(int idx) const;
  OdGeAugPolyline3d&    setVector(int idx, OdGeVector3d pnt);
  void                  getD1Vectors(OdGeVector3dArray& tangents) const;
  
  OdGeVector3d          getD2Vector(int idx) const;
  OdGeAugPolyline3d&    setD2Vector(int idx, OdGeVector3d pnt);
  void                  getD2Vectors(OdGeVector3dArray& d2Vectors) const;
  
  double                approxTol      () const;
  OdGeAugPolyline3d&    setApproxTol   (double approxTol);
  
};

#include "DD_PackPop.h"

#endif // OD_GEAPLN3D_H


