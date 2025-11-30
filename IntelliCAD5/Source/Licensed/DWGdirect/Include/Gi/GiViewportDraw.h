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



#ifndef __ODGIVIEWPORTDRAW_H__
#define __ODGIVIEWPORTDRAW_H__

#include "GiCommonDraw.h"
#include "GiViewport.h"
#include "GiViewportGeometry.h"

#include "DD_PackPush.h"

/** Description:
    Interface class that defines the functionality required to vectorize geometry that 
    changes for different viewports.  

    Remarks:
    For example, a bounded cylinder may be drawn using 2 circles (top and bottom of
    the cylinder), and 2 straight lines connecting the 2 circles, which form the silhouette edges
    of the cylinder.  These silhouette edges are dependent on the current viewing directly,
    so they must be drawn using OdGiDrawable::viewportDraw rather than OdGiDrawable::worldDraw.

    {group:OdGi_Classes} 
*/
class FIRSTDLL_EXPORT OdGiViewportDraw : public OdGiCommonDraw 
{ 
public:
  ODRX_DECLARE_MEMBERS(OdGiViewportDraw);

  /** Description:
      Returns a reference to the OdGiViewport object associated with this object.

  */
  virtual OdGiViewport&         viewport() const = 0;

  /** Description:
      Returns the OdGiViewportGeometry object associated with this object.  This geometry
      object can be used to create polylines, polygons, and other geometric data.
  */
  virtual OdGiViewportGeometry& geometry() const = 0;

  /** Description:
      Returns the number of viewports that have been deleted within the current database.
  */
  virtual OdUInt32              sequenceNumber() const = 0;

  /** Description:
      Returns true if the passed in ID is a valid viewport ID.
  */
  virtual bool                  isValidId(const OdUInt32 acgiId) const = 0;

  /** Description:
      Returns the ID of the OdDbViewport corresponding to this object.  This value will
      be NULL if Model Space is currently being vectorized (since in this case there is 
      no OdDbViewport).
  */
  virtual OdDbStub*             viewportObjectId() const = 0;
};

#include "DD_PackPop.h"

#endif //#ifndef __ODGIVIEWPORTDRAW_H__


