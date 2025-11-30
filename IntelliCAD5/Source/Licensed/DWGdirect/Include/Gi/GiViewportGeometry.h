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



#ifndef _OD_GIVIEWPORT_GEOMETRY_H_
#define _OD_GIVIEWPORT_GEOMETRY_H_

#include "Gi.h"
#include "GiGeometry.h"

class OdGePoint3d;
class OdGeVector3d;
class OdGeMatrix2d;
class OdGiRasterImage;
class OdGiMetafile;

#include "DD_PackPush.h"

/** Description:
    Contains the view dependent, model and drawing coordinate, 
    3D geometry methods.

    {group:OdGi_Classes} 
*/
class FIRSTDLL_EXPORT OdGiViewportGeometry : public OdGiGeometry
{
public:
  ODRX_DECLARE_MEMBERS(OdGiViewportGeometry);

  /** Description:
      Creates a polyline, with geometry specified in Eye Coordinates.

      Arguments:
        nbPoints (I) Number of points in the polyline.
        pPoints (I) Points of the polyline.

      Remarks:
      This polyline always has a continuous linetype.

      See Also:

      o Coordinate Systems
  */
  virtual void polylineEye(OdUInt32 nbPoints, const OdGePoint3d* pPoints) = 0;

  /** Description:
      Creates a filled polygon, with geometry specified in Eye Coordinates.

      Arguments:
        nbPoints (I) Number of points in the polygon.
        pPoints (I) Points of the polygon.

      See Also:

      o Coordinate Systems
  */
	virtual void polygonEye(OdUInt32 nbPoints, const OdGePoint3d* pPoints) = 0;

  /** Description:
      Creates a polyline, with geometry specified in Normalized Device Coordinates.

      Arguments:
        nbPoints (I) Number of points in the polyline.
        pPoints (I) Points of the polyline.

      Remarks:
      This polyline always has a continuous linetype.

      See Also:

      o Coordinate Systems
  */
	virtual void polylineDc(OdUInt32 nbPoints, const OdGePoint3d* pPoints) = 0;

  /** Description:
      Creates a filled polygon, with geometry specified in Normalized Device Coordinates.

      Arguments:
        nbPoints (I) Number of points in the polygon.
        pPoints (I) Points of the polygon.

      See Also:

      o Coordinate Systems
  */
  virtual void polygonDc(OdUInt32 nbPoints, const OdGePoint3d* pPoints) = 0;

	enum ImageSource
  {
		kFromDwg,
		kFromOleObject,
		kFromRender
	};

  /** Description:
      Processes a raster image.

      Arguments:
        origin (I) Image origin point.
        u (I) Image width vector.
        v (I) Image height vector.
        pImg (I) Image object.
        uvBoundary (I) Image boundary points (may not be null).
        numBoundPts (I) Number of boundary points.
        transparency (I) Image transparency (true if transparency is on, false otherwise).
        brightness (I) Image brightness (0-100, default is 50).
        contrast (I) Image contrast (0-100, default is 50).
        fade (I) Image fade value (0-100, default is 0).
  */
	virtual void rasterImageDc(
		const OdGePoint3d& origin,
		const OdGeVector3d& u,
		const OdGeVector3d& v,
    const OdGiRasterImage* pImg, 
    const OdGePoint2d* uvBoundary, 
    OdUInt32 numBoundPts,
    bool transparency = false,
    double brightness = 50.0,
    double contrast = 50.0,
    double fade = 0.0) = 0;

  /** Description:
      Processes a Windows metafile.

      Arguments:
        origin (I) Metafile origin.
        u (I) Metafile width vector.
        v (I) Metafile height vector.
        pMetafile (I) Metafile object.
        bDcAligned (I) reserved
        bAllowClipping (I) reserved
  */
	virtual void metafileDc(
    const OdGePoint3d& origin,
		const OdGeVector3d& u,
		const OdGeVector3d& v,
    const OdGiMetafile* pMetafile,
    bool bDcAligned = true,
    bool bAllowClipping = false) = 0;

	//virtual void ownerDrawDc(long vpnumber, long left, long top, long right, long bottom, const OwnerDraw* pOwnerDraw) const = 0;
};

#include "DD_PackPop.h"

#endif


