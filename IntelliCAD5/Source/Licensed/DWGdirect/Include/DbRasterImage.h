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
// programs incorporating this software must include the following statement
// with their copyright notices:
//
//      DWGdirect © 2002 by Open Design Alliance Inc. All rights reserved.
//
// By use of this software, you acknowledge and accept the terms of this
// agreement.
///////////////////////////////////////////////////////////////////////////////



#ifndef __IMGENT_H
#define __IMGENT_H

#include "DD_PackPush.h"

#include "DbProxyEntity.h"
#include "Ge/GePoint2dArray.h"

//const double kEpsilon = 1.0e-7;

const int kAllEntityProxyFlags =
OdDbProxyEntity::kEraseAllowed |
OdDbProxyEntity::kTransformAllowed |
OdDbProxyEntity::kColorChangeAllowed |
OdDbProxyEntity::kLayerChangeAllowed |
OdDbProxyEntity::kLinetypeChangeAllowed |
OdDbProxyEntity::kLinetypeScaleChangeAllowed |
OdDbProxyEntity::kVisibilityChangeAllowed;

class OdRasterImageImpl;

#include "DbImage.h"
#include "DbRasterImageDef.h"


/** Description:
    Represents a raster image entity in an OdDbDatabase.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbRasterImage : public OdDbImage
{
public:
  ODDB_DECLARE_MEMBERS(OdDbRasterImage);

  /** Description:
      Constructor (no arguments).
  */
  OdDbRasterImage();

  enum ClipBoundaryType
  {
    kInvalid,
    kRect,
    kPoly
  };

  enum ImageDisplayOpt
  {
    kShow = 1,
    kShowUnAligned = 2,
    kClip = 4,
    kTransparent = 8
  };

  /** Description:
      Sets the OdDbRasterImageDef object associated with this entity (DXF 340).
  */
  void setImageDefId(OdDbObjectId imageDefId);

  /** Description:
      Returns the Object ID of the OdDbRasterImageDef object associated with this entity (DXF 340).
  */
  OdDbObjectId imageDefId() const;

  /** Description:
      Sets the OdDbRasterImageDefReactor object associated with this entity (DXF 360).
  */
  void setReactorId(OdDbObjectId reactorId);

  /** Description:
      Returns the Object ID of the OdDbRasterImageDefReactor object associated with this entity (DXF 360).
  */
  OdDbObjectId reactorId() const;

  /** Description:
      Sets the clip boundary (type and geometry) for this entity (DXF 71, 14, 24).
      Arguments:
      clipPoints (I) 2d points array in pixel coordinates
  */
  void setClipBoundary(const OdGePoint2dArray& clipPoints);

  /** Description:
      Returns the clip boundary geometry for this entity in pixel coordinates (DXF 14, 24).
  */
  const OdGePoint2dArray& clipBoundary() const;

  /** Description:
      Returns the clipping state for this entity (DXF 280).
  */
  bool isClipped() const;

  /** Description:
      Sets the Clipping on/off (DXF 280).
  */
  void setClipped( bool bValue ) ;

  /** Description:
      Returns 3d point array (WCS) representig 4 corners of image if it's not clipped
      or clipping boundary if image is clipped
  */
  void getVertices(OdGePoint3dArray& verts) const;

  /** Description:
      Sets the clip boundary to coincide with the image borders. Any existing clip boundary is deleted.
  */
  virtual void setClipBoundaryToWholeImage();

  /** Description:
      Returns the clip boundary type of this entity (DXF 71).
  */
  ClipBoundaryType clipBoundaryType() const;

  /** Description
      Returns image pixel to model coordinate transform.
  */
  virtual OdGeMatrix3d getPixelToModelTransform() const;

  /** Description:
      Sets the brightness value for this image (DXF 281).
  */
  void setBrightness( OdInt8 value );

  /** Description:
      Returns the brightness value for this image (DXF 281).
  */
  OdInt8 brightness() const;

  /** Description:
      Sets the contrast value for this image (DXF 282).
  */
  void setContrast( OdInt8 value );

  /** Description:
      Returns the contrast value for this image (DXF 282).
  */
  OdInt8 contrast() const;

  /** Description:
      Sets the fade value for this image (DXF 283).
  */
  void setFade( OdInt8 value );

  /** Description:
      Returns the fade value for this image (DXF 283).
  */
  OdInt8 fade() const;

  /** Description:
      Sets the specified image display option (DXF 70, various bits).

      Arguments:
      option (I) Which option to set (one of kShow, kShowUnAligned,
      kClip, or kTransparent).
      bValue (I) Value to set the option to.
  */
  void setDisplayOpt(ImageDisplayOpt option, bool bValue);

  /** Description:
      Returns the value of the specified display option (DXF 70, various bits).

      Arguments:
      option (I) Which option to get the value for.

      Return Value:
      Value of the display option specified by the "option" parameter.
  */
  bool isSetDisplayOpt(ImageDisplayOpt option) const;

  /** Description:
      Returns the image size in pixels (DXF 13, 23).
  */
  OdGeVector2d imageSize(bool bGetCachedValue = false) const;

  OdGeVector2d scale() const;

  /** Description:
      Sets the position and orientation of an image entity.

      Arguments:
      origin (I) - lower left corner origin 
      U (I) - a vector describing the image width and direction 
      V (I) - a vector specifying on which side of the U vector the image should be drawn
      and the height of the image 
  */
  bool setOrientation(const OdGePoint3d& origin, const OdGeVector3d& uCorner,
                              const OdGeVector3d& vOnPlane);

  /** Description:
      Returns the position and orientation of an image entity.

      Arguments:
      origin (O) - lower left corner origin 
      U (O) - a vector describing the image width and direction 
      V (O) - a vector specifying on which side of the U vector the image should be drawn
      and the height of the image 
  */
  void getOrientation(OdGePoint3d& origin, OdGeVector3d& u, OdGeVector3d& v) const;

  /** Description:
      TBC.
  OdGiSentScanLines* getScanLines(const OdGiRequestScanLines& req) const;
  bool freeScanLines(OdGiSentScanLines* pSSL) const;
  */

  /** Description:
       coment it for a while, because we do not know what it do
     static Oda::ClassVersion classVersion();
  */

  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;

	void subClose();

  /*
     virtual void subSwapIdWith(OdDbObjectId otherId, bool swapXdata = false, bool swapExtDict = false);
  */

  bool worldDraw(OdGiWorldDraw* pWd) const;

  void viewportDraw(OdGiViewportDraw* mode) const;

  OdResult getGeomExtents(OdGeExtents3d& ext) const;

  void getClassID(void** pClsid) const;

  OdResult transformBy(const OdGeMatrix3d& xform);

  OdDbObjectPtr deepClone(OdDbIdMapping& idMap) const;

  /*
  virtual void getOsnapPoints( OdDb::OsnapMode osnapMode,
    int gsSelectionMark, const OdGePoint3d& pickPoint,
    const OdGePoint3d& lastPoint, const OdGeMatrix3d& viewXform,
    OdGePoint3dArray& snapPoints, OdDbIntArray& geomIds) const;

  virtual void getGripPoints(OdGePoint3dArray& gripPoints,
    OdDbIntArray& osnapModes, OdDbIntArray& geomIds) const;

  virtual void moveGripPointsAt(const OdDbIntArray& indices, const OdGeVector3d& offset);

  virtual void getStretchPoints(OdGePoint3dArray& stretchPoints) const;

  virtual void moveStretchPointsAt(const OdDbIntArray& indices, const OdGeVector3d& offset);

  virtual void transformBy(const OdGeMatrix3d& xform);
  virtual void getTransformedCopy(const OdGeMatrix3d& xform, OdDbEntity** ent) const;
  virtual OdResult explode(OdRxObjectPtrArray& entitySet) const; //Replace OdRxObjectPtrArray
  virtual bool getGeomExtents(OdGeExtents3d& extents) const;

  virtual void getSubentPathsAtGsMarker(OdDb::SubentType type,
    int gsMark, const OdGePoint3d& pickPoint,
    const OdGeMatrix3d& viewXform, int& numPaths,
    OdDbFullSubentPath*& subentPaths, int numInserts = 0,
    OdDbObjectId* entAndInsertStack = 0) const;

  virtual void getGsMarkersAtSubentPath(
    const OdDbFullSubentPath& subPath,
    OdDbIntArray& gsMarkers) const;

  virtual OdDbEntity* subentPtr(const OdDbFullSubentPath& id) const;
  virtual void saveAs(OdGiWorldDraw* mode, OdDb::EntSaveAsType st);
  virtual void intersectWith(const OdDbEntity* ent,
    OdDb::Intersect intType, OdGePoint3dArray& points,
    int thisGsMarker = 0, int otherGsMarker = 0) const;

  virtual void intersectWith(const OdDbEntity* ent,
    OdDb::Intersect intType, const OdGePlane& projPlane,
    OdGePoint3dArray& points, int thisGsMarker = 0,
    int otherGsMarker = 0) const;
  */
};


inline void
pixelToModel(const OdGeMatrix3d& pixToMod,
             const OdGePoint2d& pixPt,
             OdGePoint3d& modPt)
{
  modPt.set(pixPt.x, pixPt.y, 0);
  modPt.transformBy(pixToMod);
}

inline void
modelToPixel(const OdGeMatrix3d& modToPix,
             const OdGePoint3d& modPt,
             OdGePoint2d& pixPt)
{
  OdGePoint3d modelPt = modPt;
  modelPt.transformBy(modToPix);
  pixPt.set(modelPt.x, modelPt.y);
}

inline void
modelToPixel(const OdGeVector3d& viewDir,
             const OdGeMatrix3d& modToPix,
             const OdGePlane& plane,
             const OdGePoint3d& modPt,
             OdGePoint2d& pixPt)
{
  OdGePoint3d ptOnPlane = modPt.project(plane, viewDir);
  ptOnPlane.transformBy(modToPix);
  pixPt.set(ptOnPlane.x, ptOnPlane.y);
}


typedef OdSmartPtr<OdDbRasterImage> OdDbRasterImagePtr;

#include "DD_PackPop.h"

#endif // __IMGENT_H


