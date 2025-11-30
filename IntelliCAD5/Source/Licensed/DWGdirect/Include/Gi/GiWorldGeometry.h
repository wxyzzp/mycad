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



#ifndef __ODGIWORLDGEOMETRY_H__
#define __ODGIWORLDGEOMETRY_H__

#include "GiGeometry.h"

#include "DD_PackPush.h"

/** Description:
    Contains the view independent, model coordinate, 3D geometry methods.  An OdGiWorldGeometry
    object can be obtained from the OdGiWorldDraw argument passed to the OdGiDrawable::worldDraw
    method.

    {group:OdGi_Classes} 
*/
class FIRSTDLL_EXPORT OdGiWorldGeometry : public OdGiGeometry 
{ 
public:
  ODRX_DECLARE_MEMBERS(OdGiWorldGeometry);

  /** Description:
      Sets the extents of the current entity to the passed in value.  

      Arguments:
        pNewExtents (I) The WCS extents to be set for the current entity (2 points representing opposite corners of the WCS bounding box).
  */
  virtual void setExtents(const OdGePoint3d *pNewExtents) = 0;

  // If this drawable is a block and has OdDbAttributes it must call
  // this method exactly once before sending the attributes to 
  // draw(). Once this is called the object may draw only attributes
  // and no more primitives
  // The attributes must be the last objects vectorized.
  //
  // virtual void startAttributesSegment() {}
};

#include "DD_PackPop.h"

#endif


