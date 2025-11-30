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



#ifndef OD_GEOFFSF_H
#define OD_GEOFFSF_H


//
// Description:
//
// This file contains the class OdGeOffsetSurface, a
// representation for an offset surface
//

#include "GeSurface.h"

#include "DD_PackPush.h"

class OdGePlane;
class OdGeBoundedPlane;
class OdGeCylinder;
class OdGeCone;
class OdGeSphere;
class OdGeTorus;
class OdGeEllipCone;	// AE 03.09.2003 
class OdGeEllipCylinder;	// AE 09.09.2003 

/** Description:
    
    {group:OdGe_Classes} 
*/
class GE_TOOLKIT_EXPORT OdGeOffsetSurface : public OdGeSurface
{
public:
    OdGeOffsetSurface();
    OdGeOffsetSurface(OdGeSurface* baseSurface,
                      double offsetDist,
                      bool makeCopy = true);
    OdGeOffsetSurface(const OdGeOffsetSurface& offset);

    // Test whether this offset surface can be converted to a simple surface
    //
    bool    isPlane        () const;
    bool    isBoundedPlane  () const;
    bool    isSphere       () const;
    bool    isCylinder     () const;
    bool    isCone         () const;
    bool    isTorus        () const;
		bool		isEllipCone() const;	// AE 03.09.2003 

    // Convert this offset surface to a simple surface
    //
        bool    getSurface(OdGeSurface*&) const;

    // Get a copy of the construction surface.
    //
    void              getConstructionSurface (OdGeSurface*& base) const;

    double            offsetDist     () const;

    // Reset surface
    //
    OdGeOffsetSurface& set        (OdGeSurface*, double offsetDist,
                                    bool makeCopy = true);

    // Assignment operator.
    //
    OdGeOffsetSurface& operator =  (const OdGeOffsetSurface& offset);
};

#include "DD_PackPop.h"

#endif // OD_GEOFFSF_H


