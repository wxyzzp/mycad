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



#ifndef OD_GEXBNDSF_H
#define OD_GEXBNDSF_H


#include "GeSurface.h"

#include "DD_PackPush.h"

class OdGeExternalSurface;
class OdGeCurveBoundary;

/** Description:
    A representation of the geometry associated with a face entity.    

    {group:OdGe_Classes} 
*/
class OdGeExternalBoundedSurface : public OdGeSurface
{
public:
  OdGeExternalBoundedSurface();
  OdGeExternalBoundedSurface(void* surfaceDef, OdGe::ExternalEntityKind surfaceKind, bool makeCopy = true);
  OdGeExternalBoundedSurface(const OdGeExternalBoundedSurface&);
  
  // Surface data.
  //
  OdGe::ExternalEntityKind externalSurfaceKind() const;
  bool isDefined() const;
  void getExternalSurface(void*& surfaceDef) const;
  
  // Odcess to unbounded surface.
  //
  
  void getBaseSurface(OdGeSurface*& surfaceDef) const;
  
  void getBaseSurface(OdGeExternalSurface& unboundedSurfaceDef) const;
  
  // Type queries on the unbounded base surface.
  bool isPlane() const;
  bool isSphere() const;
  bool isCylinder() const;
  bool isCone() const;
	bool isEllipCone() const;	// AE 03.09.2003 
  bool isTorus() const;
  bool isNurbs() const;
  bool isExternalSurface() const;
  
  // Odcess to the boundary data.
  //
  int numContours() const;
  void getContours(int& numContours, OdGeCurveBoundary*& curveBoundaries) const;
  
  // Set methods
  //
  OdGeExternalBoundedSurface& set(void* surfaceDef,
    OdGe::ExternalEntityKind surfaceKind, bool makeCopy = true);
  
  // Assignment operator.
  //
  OdGeExternalBoundedSurface& operator =(const OdGeExternalBoundedSurface&);
  
  // Surface ownership.
  //
  bool isOwnerOfSurface() const;
  OdGeExternalBoundedSurface& setToOwnSurface();
};

#include "DD_PackPop.h"

#endif // OD_GEXBNDSF_H

