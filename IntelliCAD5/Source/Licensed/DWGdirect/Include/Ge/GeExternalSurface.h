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



#ifndef OD_GEEXTSF_H
#define OD_GEEXTSF_H

#include "GeSurface.h"

#include "DD_PackPush.h"

class OdGePlane;
class OdGeCylinder;
class OdGeCone;
class OdGeSphere;
class OdGeTorus;
class OdGeNurbSurface;
class OdGeEllipCone;			// AE 03.09.2003 
class OdGeEllipCylinder;	// AE 09.09.2003 	

/** Description:
    A representation for a surface that has its definition external
    to the OdGe library.    

    {group:OdGe_Classes} 
*/
class OdGeExternalSurface : public OdGeSurface
{
public:
   OdGeExternalSurface();
   OdGeExternalSurface(void* surfaceDef, OdGe::ExternalEntityKind surfaceKind,
                       bool makeCopy = true);
   OdGeExternalSurface(const OdGeExternalSurface&);

   // Defining surface.
   //
   void getExternalSurface(void*& surfaceDef) const;

   // Type of the external surface.
   //
   OdGe::ExternalEntityKind  externalSurfaceKind() const;

   bool    isPlane      () const;
   bool    isSphere     () const;
   bool    isCylinder   () const;
   bool    isCone       () const;
   bool    isTorus      () const;
   bool    isNurbSurface() const;
   bool    isDefined    () const;
	 bool		 isEllipCone() const;	// AE 03.09.2003 

   // Conversion to gelib entity
   //
   bool isNativeSurface(OdGeSurface*& nativeSurface) const;

   // Assignment operator.
   //
   OdGeExternalSurface& operator = (const OdGeExternalSurface& src);

   // Reset surface
   //
   OdGeExternalSurface& set(void* surfaceDef,
                            OdGe::ExternalEntityKind surfaceKind,
                            bool makeCopy = true);
   // Ownership of surface.
   //
   bool       isOwnerOfSurface    () const;
   OdGeExternalSurface& setToOwnSurface     ();
};

#include "DD_PackPop.h"

#endif // OD_GEEXTSF_H


