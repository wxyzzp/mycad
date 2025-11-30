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



#ifndef __ODGICOMMONDRAW_H__
#define __ODGICOMMONDRAW_H__

#include "DD_PackPush.h"

#include "RxObject.h"
#include "Gi.h"

class OdGiGeometry;
class OdGiContext;
class OdGiSubEntityTraits;
class OdDbDatabase;
class OdDbStub;
class OdGePoint3d;
class OdGiDrawable;
typedef OdSmartPtr<OdGiDrawable> OdGiDrawablePtr;

/** Description:
    Viewport regeneration modes. This mode can not be set by the user, 
    but it can be queried.
*/
typedef enum
{
  eOdGiRegenTypeInvalid = 0,
  kOdGiStandardDisplay = 2,
  kOdGiHideOrShadeCommand,
  kOdGiRenderCommand,
  kOdGiForExplode,
  kOdGiSaveWorldDrawForProxy,
  kOdGiForExtents
} OdGiRegenType;

/** Description: Deviation types used for tessellation.
*/
typedef enum
{
  kOdGiMaxDevForCircle = 0,
  kOdGiMaxDevForCurve,
  kOdGiMaxDevForBoundary,
  kOdGiMaxDevForIsoline,
  kOdGiMaxDevForFacet
} OdGiDeviationType;


class OdGiCommonDraw;
class OdGiTextStyle;
class OdPsPlotStyleData;
class OdGiConveyorGeometry;


enum // temporary fix : to be removed
{
  kOdGiIncludeScores = 2,
  kOdGiRawText       = 4,
  kOdGiIncludePenups = 8,
  kOdGiPlotGeneration= 16
};

/** Description:
    Interface that defines common operations and properties that are used in the
    DWGdirect vectorization process.  An instance of an OdGiContext subclass
    is normally created as a preliminary step in the vectorization process, to be
    used throughout the vectorization.

    Most of the virtual functions in this class (the ones that are not pure virtual)
    are no-ops, serving only to define an interface.

    {group:OdGi_Classes} 
*/
class FIRSTDLL_EXPORT OdGiContext : public OdRxObject
{
public:
  ODRX_DECLARE_MEMBERS(OdGiContext);

  /** Description:
      Returns a pointer to the database that is currently being vectorized.
  */
  virtual OdDbDatabase* database() const = 0;

  /** Description:
      Opens a drawable object belonging to the database associated with this context, 
      and returns a pointer to that object.
  */
  virtual OdGiDrawablePtr openDrawable(OdDbStub* id) = 0;

  /** Description:
      Returns the default lineweight of this context.
  */
  virtual OdDb::LineWeight defaultLineWeight() const;

  virtual double commonLinetypeScale() const;

  /** Description:
      Returns the default OdGiTextStyle associated with this context.
  */
  virtual void getDefaultTextStyle(OdGiTextStyle& style);

  /** Description:
      Vectorizes a shape entity into the supplied destination object.

      Arguments:
        pDraw (I) Interface into which the shape is vectorized.
        pos (I) Position of the shape.
        shapeNo (I) Index of the shape.
        pStyle (I) Style of the shape.
  */
  virtual void drawShape(OdGiCommonDraw* pDraw, 
    OdGePoint3d& pos, 
    int shapeNo, 
    const OdGiTextStyle* pStyle);

  /** Arguments:
        pDest (I) Conveyor object into which the shape is vectorized.
        u (I) Defines the baseline direction for the text string.
        v (I) The up vector for the text string.
        pExtrusion (I) Extrusion vector to apply to this text.
  */
  virtual void drawShape(OdGiConveyorGeometry* pDest,
    const OdGePoint3d& pos,
    const OdGeVector3d& u, 
    const OdGeVector3d& v,
    int shapeNo, 
    const OdGiTextStyle* pStyle,
    const OdGeVector3d* pExtrusion);

  /** Description:
      Vectorizes a text string into the supplied OdGiCommonDraw object.

      Arguments:
        pDraw (I) Interface into which the shape is vectorized.
        pos (I) Position of the text string.
        msg (I) The text string.
        nLength (I) Number of bytes in msg (not including the optional null byte).
        pStyle (I) Text style properties for this text string.
        flags (I) Bit field that can include kOdGiIncludeScores = 2, kOdGiRawText = 4,
          kOdGiIncludePenups = 8, and kOdGiPlotGeneration= 16.
  */
  virtual void drawText(OdGiCommonDraw* pDraw, 
    OdGePoint3d& pos,
	  const OdChar* msg, 
    OdInt32 nLength,
    const OdGiTextStyle* pStyle, 
    OdUInt32 flags = 0);

  /** Arguments:
      height (I) Height of the text.
      width (I) Width of text, currently not supported.
      oblique (I) Oblique angle, currently not supported.
  */
  virtual void drawText(OdGiCommonDraw* pDraw, 
    OdGePoint3d& pos,
    double height, 
    double width, 
    double oblique, 
    const OdChar* msg);

  /** Arguments:
      raw (I) If true, then treat the string as raw text, in which case special characters
        such as %%D (degree symbol), etc. are not converted to the appropriate symbol, 
        but are instead treated as literal strings.
  */
  virtual void drawText(OdGiConveyorGeometry* pDest,
    const OdGePoint3d& pos,
    const OdGeVector3d& u, 
    const OdGeVector3d& v,
    const OdChar* msg, 
    OdInt32 nLength, 
    bool raw,
    const OdGiTextStyle* pStyle,
    const OdGeVector3d* pExtrusion);


  /** Description:
      Calculates and returns an extent box for the passed in text.
       
      Arguments:
        style (I) Style of the text.
        pStr (I) The text string.
        nStrLen (I) Length of pStr in bytes.
        flags (I) Bit field that can include kOdGiIncludeScores = 2, kOdGiRawText = 4,
          kOdGiIncludePenups = 8, and kOdGiPlotGeneration= 16.
        min (O) Lower left corner of the calculated text extents box.
        max (O) Upper right corner of the calculated text extents box.
        pEndPos (O) If non-NULL, receives the calculated end position of the text string.
  */
  virtual void textExtentsBox(const OdGiTextStyle& style, 
    const OdChar* pStr, 
    int nStrLen,
    OdUInt32 flags, 
    OdGePoint3d& min, 
    OdGePoint3d& max, 
    OdGePoint3d* pEndPos = 0);

  /** Description:
      Calculates and returns an extent box for the passed in shape.
       
      Arguments:
        style (I) Style of the shape.
        shapeNo (I) Index of the shape.
        min (O) Lower left corner of the calculated text extents box.
        max (O) Upper right corner of the calculated text extents box.
  */
  virtual void shapeExtentsBox(const OdGiTextStyle& style, 
    int shapeNo, 
    OdGePoint3d& min, 
    OdGePoint3d& max);

  /** Description:
      Returns the circle zoom percent for this vectorization process.
      Valid range is 1-20000, 100 is the default.
  */
  virtual unsigned int circleZoomPercent(OdDbStub* vpObjectId) const;

  /** Description:
      Returns true is this vectorization is intended for hard copy output, false otherwise.
  */
  virtual bool isPlotGeneration() const;

  /** Description:
      Returns true if TrueType text should be filled during the vectorization 
      process, or false if only outlines should be drawn for TrueType text.
  */
  virtual bool fillTtf() const;

  /** Description:
      Returns the number of contour lines to draw on surfaces during this vectorization
      process.
  */
  virtual OdUInt32 numberOfIsolines() const;

  /** Description:
      Returns true if this vectorization process should be aborted, false otherwise.
  */
  virtual bool regenAbort() const;

  enum PStyleType
  {
    kPsNone,
    kPsByColor,
    kPsByName
  };
  virtual PStyleType plotStyleType() const;

  virtual void plotStyle(int nPenNumber, OdPsPlotStyleData& data) const;

  virtual void plotStyle(OdDbStub* psNameId, OdPsPlotStyleData& data) const;
};


/** Description:
    Parent class for entity-level vectorization functionality within DWGdirect.
    Defines common vectorization properties.

    {group:OdGi_Classes} 
*/
class FIRSTDLL_EXPORT OdGiCommonDraw : public OdRxObject
{
public:
  ODRX_DECLARE_MEMBERS(OdGiCommonDraw);

  /** Description:
      Returns the type of the current vectorization process.

      See Also:
        OdGiRegenType
  */
  virtual OdGiRegenType regenType() const = 0;

  /** Description:
      Returns true if this vectorization process should be aborted, false otherwise.
  */
  virtual bool regenAbort() const = 0;

  /** Description:
      Provides access to this object's sub-entity traits object, which can be used
      to modify the current vectorization attributes such as color, linetype, etc.
  */
  virtual OdGiSubEntityTraits& subEntityTraits() const = 0;

  /** Description:
      Provides access to this object's "drawing interface", or set of geometry functions
      used during the vectorization process.
  */
  virtual OdGiGeometry& rawGeometry() const = 0;

  /** Description:
      Returns true if this vectorization process is the result of a "drag" operation
      performed on an entity.  Currently unused.
  */
  virtual bool isDragging() const = 0;
  
  /** Description:
      Returns the deviation (with respect to the current active viewport) of the
      current vectorization process.  The deviation sets the maximum difference 
      between the actual curve or surface, and the tessellated curve or surface.
  */
  virtual double deviation(const OdGiDeviationType, const OdGePoint3d&) const = 0;

  /** Description:
      Returns the number of contour lines to draw on surfaces during this vectorization
      process.
  */
  virtual OdUInt32 numberOfIsolines() const = 0;

  /** Description:
      Returns the OdGiContext instance associated with this object.
  */
  virtual OdGiContext* context() const = 0;
};

#include "DD_PackPop.h"

#endif


