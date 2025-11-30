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



#ifndef OD_GEEXTC3D_H
#define OD_GEEXTC3D_H

class OdGeLine3d;
class OdGeLineSeg3d;
class OdGeRay3d;
class OdGeEllipArc3d;
class OdGeNurbCurve3d;
class OdGeExternalCurve2d;
class OdGeExternalCurve3d;

#include "GeCurve3d.h"

#include "DD_PackPush.h"

/** Description:
    
    {group:OdGe_Classes} 
*/
class OdGeExternalCurve3d : public OdGeCurve3d
{
public:
    OdGeExternalCurve3d();
    OdGeExternalCurve3d(const OdGeExternalCurve3d& src);
    OdGeExternalCurve3d(void* curveDef, OdGe::ExternalEntityKind curveKind,
                            bool makeCopy = true);

    bool isLine     () const;
    bool isRay      () const;
    bool isLineSeg  () const;
    bool isCircArc  () const;
    bool isEllipArc  () const;
    bool isNurbCurve() const;
    bool isDefined  () const;

    bool isNativeCurve  (OdGeCurve3d*& nativeCurve) const;
    void           getExternalCurve (void*& curveDef) const;

    OdGe::ExternalEntityKind externalCurveKind() const;

    OdGeExternalCurve3d& set(void* curveDef, OdGe::ExternalEntityKind curveKind,
                             bool makeCopy = true);
    OdGeExternalCurve3d& operator = (const OdGeExternalCurve3d& src);

    bool       isOwnerOfCurve   () const;
    OdGeExternalCurve3d& setToOwnCurve    ();
};

#include "DD_PackPop.h"

#endif // OD_GEEXTC3D_H


