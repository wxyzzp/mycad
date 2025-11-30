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



#ifndef __ODGIGEOMETRY_H__
#define __ODGIGEOMETRY_H__

#include "DD_PackPush.h"
#include "RxObject.h"

#include "Ge/GePoint3d.h"

class OdDbStub;
class OdCmEntityColor;
class OdGeMatrix3d;
class OdGeVector3d;
//class OdGePoint3d;
class OdGePoint2d;

/** Description:
    Contains arrays of edge attributes for meshes and shells.
    Note: setting an 'improper' size of any array will cause
    unpredictable or fatal results.

    {group:OdGi_Classes} 
*/
class OdGiEdgeData
{
  const OdUInt16*         m_pColors;
  const OdCmEntityColor*  m_pTrueColors;
        OdDbStub**        m_pLayerIds;
        OdDbStub**        m_pLinetypeIds;
  const OdInt32*          m_pSelectionMarkers;
  const OdUInt8*          m_pVisibilities;
public:
  OdGiEdgeData()
    : m_pColors(NULL)
    , m_pTrueColors(NULL)
    , m_pLayerIds(NULL)
    , m_pLinetypeIds(NULL)
    , m_pSelectionMarkers(NULL)
    , m_pVisibilities(NULL)
  {}

  /** Description:
      Sets an array of color indices.  The client application is responsible for ensuring
      that the size of this array is correct.
  */
  void setColors(const OdUInt16 *pColors)                    { m_pColors = pColors; }

  /** Description:
      Sets an array of OdCmEntityColors.  The client application is responsible for ensuring
      that the size of this array is correct.
  */
  void setTrueColors(const OdCmEntityColor *pColors)         { m_pTrueColors = pColors; }

  /** Description:
      Sets an array of layers.  The client application is responsible for ensuring
      that the size of this array is correct.
  */
  void setLayers(OdDbStub* *pLayerIds)                       { m_pLayerIds = pLayerIds; }

  /** Description:
      Sets an array of linetypes.  The client application is responsible for ensuring
      that the size of this array is correct.
  */
  void setLinetypes(OdDbStub* *pLinetypeIds)                 { m_pLinetypeIds = pLinetypeIds; }

  /** Description:
      Sets an array of selection markers.  The client application is responsible for ensuring
      that the size of this array is correct.
  */
  void setSelectionMarkers(const OdInt32* pSelectionMarkers) { m_pSelectionMarkers = pSelectionMarkers; }

  /** Description:
      Sets an array of visibility values.  The client application is responsible for ensuring
      that the size of this array is correct.
  */
  void setVisibility(const OdUInt8* pVisibilities)           { m_pVisibilities = pVisibilities; }
  
  /** Description:
      Returns the array of color indices stored in this object.
      The client application is responsible for ensuring that reads into this array
      do not exceed the array size.
  */
  const OdUInt16*        colors() const           { return m_pColors; }

  /** Description:
      Returns the array of OdCmEntityColor's stored in this object.
      The client application is responsible for ensuring that reads into this array
      do not exceed the array size.
  */
  const OdCmEntityColor* trueColors() const       { return m_pTrueColors; }

  /** Description:
      Returns the array of layers stored in this object.
      The client application is responsible for ensuring that reads into this array
      do not exceed the array size.
  */
  OdDbStub**             layerIds() const         { return m_pLayerIds; }

  /** Description:
      Returns the array of linetypes stored in this object.
      The client application is responsible for ensuring that reads into this array
      do not exceed the array size.
  */
  OdDbStub**             linetypeIds() const      { return m_pLinetypeIds; }

  /** Description:
      Returns the array of selection markers stored in this object.
      The client application is responsible for ensuring that reads into this array
      do not exceed the array size.
  */
  const OdInt32*         selectionMarkers() const { return m_pSelectionMarkers; }

  /** Description:
      Returns the array of visibility settings stored in this object.
      The client application is responsible for ensuring that reads into this array
      do not exceed the array size.
  */
  const OdUInt8*         visibility() const       { return m_pVisibilities; }
};


/** Description:
    Contains arrays of face attributes for meshes and shells.
    Note: setting an 'improper' size of any array will cause
    unpredictable or fatal results.

    {group:OdGi_Classes} 
*/
class OdGiFaceData
{
  const OdUInt16*         m_pColors;
  const OdCmEntityColor*  m_pTrueColors;
        OdDbStub**        m_pLayerIds;
  const OdInt32*          m_pSelectionMarkers;
  const OdUInt8*          m_pVisibilities;
  const OdGeVector3d*     m_pNormals;
public:
  OdGiFaceData()
    : m_pColors(NULL)
    , m_pTrueColors(NULL)
    , m_pLayerIds(NULL)
    , m_pSelectionMarkers(NULL)
    , m_pNormals(NULL)
    , m_pVisibilities(NULL)
  {}

  /** Description:
      Sets an array of color indices.  The client application is responsible for ensuring
      that the size of this array is correct.
  */
  void setColors(const OdUInt16 *pColors)                    { m_pColors = pColors; }

  /** Description:
      Sets an array of OdCmEntityColor objects.  The client application is responsible for ensuring
      that the size of this array is correct.
  */
  void setTrueColors(const OdCmEntityColor *pColors)         { m_pTrueColors = pColors; }

  /** Description:
      Sets an array of layers.  The client application is responsible for ensuring
      that the size of this array is correct.
  */
  void setLayers(OdDbStub* *pLayerIds)                       { m_pLayerIds = pLayerIds; }

  /** Description:
      Sets an array of selection markers.  The client application is responsible for ensuring
      that the size of this array is correct.
  */
  void setSelectionMarkers(const OdInt32* pSelectionMarkers) { m_pSelectionMarkers = pSelectionMarkers; }

  /** Description:
      Sets an array of normal vectors.  The client application is responsible for ensuring
      that the size of this array is correct.
  */
  void setNormals(const OdGeVector3d* pNormals)              { m_pNormals = pNormals; }

  /** Description:
      Sets an array of visibility settings.  The client application is responsible for ensuring
      that the size of this array is correct.
  */
  void setVisibility(const OdUInt8* pVisibilities)           { m_pVisibilities = pVisibilities; }
  
  /** Description:
      Returns the array of color indices stored in this object.
      The client application is responsible for ensuring that reads into this array
      do not exceed the array size.
  */
  const OdUInt16*        colors() const           { return m_pColors; }

  /** Description:
      Returns the array of OdCmEntityColor objects stored in this object.
      The client application is responsible for ensuring that reads into this array
      do not exceed the array size.
  */
  const OdCmEntityColor* trueColors() const       { return m_pTrueColors; }

  /** Description:
      Returns the array of layers stored in this object.
      The client application is responsible for ensuring that reads into this array
      do not exceed the array size.
  */
  OdDbStub**             layerIds() const         { return m_pLayerIds; }

  /** Description:
      Returns the array of selection markers stored in this object.
      The client application is responsible for ensuring that reads into this array
      do not exceed the array size.
  */
  const OdInt32*         selectionMarkers() const { return m_pSelectionMarkers; }

  /** Description:
      Returns the array of normal vectors stored in this object.
      The client application is responsible for ensuring that reads into this array
      do not exceed the array size.
  */
  const OdGeVector3d*    normals() const          { return m_pNormals; }

  /** Description:
      Returns the array of visibility settings stored in this object.
      The client application is responsible for ensuring that reads into this array
      do not exceed the array size.
  */
  const OdUInt8*         visibility() const       { return m_pVisibilities; }
};

typedef enum
{
  kOdGiCounterClockwise = -1,
  kOdGiNoOrientation    = 0,
  kOdGiClockwise        = 1
} OdGiOrientationType;


/** Description:
    Contains arrays of vertex attributes for meshes and shells.
    Note: setting an 'improper' size of any array will cause
    unpredictable or fatal results.

    {group:OdGi_Classes} 
*/
class OdGiVertexData
{
  const OdGeVector3d* m_pNormals;
  OdGiOrientationType m_orientationFlag;
  const OdCmEntityColor*  m_pTrueColors;
public:
  OdGiVertexData()
    : m_pNormals(NULL)
    , m_orientationFlag(kOdGiNoOrientation)
    , m_pTrueColors(NULL)
  {}

  /** Description:
      Sets an array of normal vectors.  The client application is responsible for ensuring
      that the size of this array is correct.
  */
  void setNormals(const OdGeVector3d* pNormals)                      { m_pNormals = pNormals; }

  /** Description:
      Sets an array of orientation flags.  The client application is responsible for ensuring
      that the size of this array is correct.
  */
  void setOrientationFlag(const OdGiOrientationType orientationFlag) { m_orientationFlag = orientationFlag; }

  /** Description:
      Sets an array of OdCmEntityColor objects.  The client application is responsible for ensuring
      that the size of this array is correct.
  */
  void setTrueColors(const OdCmEntityColor *pColors)                 { m_pTrueColors = pColors; }

  /** Description:
      Returns the array of normal vectors stored in this object.
      The client application is responsible for ensuring that reads into this array
      do not exceed the array size.
  */
  const OdGeVector3d* normals() const         { return m_pNormals; }

  /** Description:
      Returns the array of orientation flags stored in this object.
      The client application is responsible for ensuring that reads into this array
      do not exceed the array size.
  */
  OdGiOrientationType orientationFlag() const { return m_orientationFlag; }

  /** Description:
      Returns the array of OdCmEntityColor objects stored in this object.
      The client application is responsible for ensuring that reads into this array
      do not exceed the array size.
  */
  const OdCmEntityColor* trueColors() const   { return m_pTrueColors; }
};

class OdGiDrawable;
struct OdGiClipBoundary;

class OdDbPolyline;
class OdPolyPolygon3d;
class OdGeNurbCurve3d;
class OdGiTextStyle;
class OdDbBody;
class OdGeEllipArc3d;
class OdGiPolyline;

// Arc types
typedef enum
{
  kOdGiArcSimple = 0, // just the arc (not fillable)
  kOdGiArcSector,     // area bounded by the arc and its center of curvature
  kOdGiArcChord       // area bounded by the arc and its end points
} OdGiArcType;


/** Description:
    Defines the interface methods that define a vectorization context 
    within the the DWGdirect vectorization framework.  Objects such as OdDbEntity descendents, can 
    use a descendent of OdGiGeometry to vectorize themselves.

    {group:OdGi_Classes} 
*/
class FIRSTDLL_EXPORT OdGiGeometry : public OdRxObject
{
public:
  ODRX_DECLARE_MEMBERS(OdGiGeometry);

  /** Description:
      Returns the current block insertion transformation (which will convert the current
      object's coordinates into WCS, taking into account any active block insertions).
  */
  virtual OdGeMatrix3d getModelToWorldTransform() const = 0;

  /** Description:
      Returns the inverse of the transformation returned by getModelToWorldTransform. 
      This transformation will convert WCS back to model coordinates, taking into account
      all active block insertions.
  */
  virtual OdGeMatrix3d getWorldToModelTransform() const = 0;
  
  /** Description:
      Pushes a model transformation onto the current transformation stack.  This transformation
      is concatenated onto the current model transformation (which is initially the identity
      transform).  The current model transformation is applied to all geometry 
      introduced into this vectorization context.
      
      Remarks:
      The transformation is created by using the arbitrary axis algorithm 
      on the passed in vector. 
  */
  virtual void pushModelTransform(const OdGeVector3d& vNormal) = 0;

  virtual void pushModelTransform(const OdGeMatrix3d& xMat) = 0;

  /** Description:
      Pops the top transformation off of the current transformation stack.
  */
  virtual void popModelTransform() = 0;
  
  /** Description:
      Introduces a circle into this vectorization context.  The circle is not filled
      (outline only), and the outline takes on the current drawing color.

      Arguments:
        center (I) Center point of the circle.
        radius (I) Radius of the circle.
        normal (I) Normal vector of the plane in which the circle lies.
  */
  virtual void circle(const OdGePoint3d& center, double radius, const OdGeVector3d& normal) = 0;
  
  /** Arguments:
        start (I) Start point on circle.
        point (I) Point on circle.
        end (I) End point on circle.

      Remarks:
        This circle is defined by 3 points, which may not be colinear or coincident.
  */
  virtual void circle(const OdGePoint3d& start, const OdGePoint3d& point, const OdGePoint3d& end) = 0;
  
  /** Description:
      Introduces a circular arc into this vectorization context.  The arc takes on 
      the current drawing color.  kOdGiArcSimple type arcs are not filled, while
      kOdGiArcSector and kOdGiArcChord type arcs are filled.

      Arguments:
        center (I) Center point of the arc.
        radius (I) Radius of the arc.
        normal (I) Normal vector for this entity.
        startVector (I) Defines the start of this arc.
        sweepAngle (I) Angle (in radians) that defines the arc.
        arcType (I) Type of arc, either kOdGiArcSimple, kOdGiArcSector or kOdGiArcChord.
  */
  virtual void circularArc(const OdGePoint3d& center,
    double radius,
    const OdGeVector3d& normal,
    const OdGeVector3d& startVector,
    double sweepAngle,
    OdGiArcType arcType = kOdGiArcSimple) = 0;
  
  /** Arguments:
        start (I) Start point of circular arc.
        point (I) Point on circular arc.
        end (I) End point on circular arc.

      Remarks:
        This circular arc is defined by 3 points, which may not be colinear or coincident.
  */
  virtual void circularArc(const OdGePoint3d& start,
    const OdGePoint3d& point,
    const OdGePoint3d& end,
    OdGiArcType arcType = kOdGiArcSimple) = 0;

  /** Description:
      Inroduces a polyline into this vectorization context.  The polyline
      takes on the current drawing color.  The polyline
      is formed by starting with the first point in the passed in point list, and 
      forming a straight line segment to the next point in the list, then the next, and 
      so on.

      Remarks:
      Any closed areas created by this function are not filled.  The polygon() function
      should be used to create filled areas.  The current thickness value will be 
      applied to this polyline.  All points in the polyline must be co-planar.

      Arguments:
        nbPoints (I) Number of points in the polyline.
        pVertexList (I) Pointer to an array of vertices that make up the polyline.
        pNormal (I) Normal vector of the plane in which this polyline lies.
        lBaseSubEntMarker (I) Current not used.
  */
  virtual void polyline(OdInt32 nbPoints,
    const OdGePoint3d* pVertexList,
    const OdGeVector3d* pNormal = NULL,
    OdInt32 lBaseSubEntMarker = -1) = 0;
  
  /** Description:
      Inroduces a polygon into this vectorization context.  The polygon
      is filled, and it takes on the current drawing color.  The polygon
      is formed by starting with the first point in the passed in point list, and 
      forming a straight line segment to the next point in the list, then the next, and 
      so on, and a final segment is drawn from the last point to the first to close the
      polygon.

      Arguments:
        nbPoints (I) Number of points in the polygon.
        pVertexList (I) Pointer to an array of vertices that make up the polygon.
  */
  virtual void polygon(OdInt32 nbPoints, const OdGePoint3d* pVertexList) = 0;

  /** Description:
      Introduces a lightweight polyline into the DWGdirect vectorization framwork.  

      Remarks:
      The polyline is not filled (even if it is closed), and it takes on the current
      drawing color.  All points in the polyline must be co-planar. 
      The polyline may have varying widths, as well as straight segments
      or arc segments.  

      Arguments:
        lwBuf (I) Polyline data to be processed.
        fromIndex (I) Index of the first segment in this polyline that should be processed.
        numSegs (I) Number of segments, starting with fromIndex, that should be processed (0 indicates that all segments should be processed).

      See Also:
        OdGiPolyline
  */
  virtual void pline(const OdGiPolyline& lwBuf, OdUInt32 fromIndex = 0, OdUInt32 numSegs = 0) = 0;

  /** Description:
      Introduces a mesh into this vectorization context.  A mesh is a surface
      formed by a grid of vertices.

      Arguments:
        rows (I) Number of rows in the mesh.
        columns (I) Number of columns in the mesh.
        pVertexList (I) Pointer to an array of vertices that make up the mesh.
        pEdgeData (I) Pointer to additional edge data (color, layer, etc.).
        pFaceData (I) Pointer to additional face data (color, layer, etc.).
        pVertexData (I) Additional vertex attributes (normals, colors, etc.). 

      Remarks:
      By default, a mesh takes on the current drawing color.  However, color, linetype
      and other properties can be controlled within the mesh by passing in the appropriate
      data for the OdGiEdgeData, OdGiFaceData, and OdGiVertexData arguments.  See Vectorization
      Primitives for more details about mesh construction.

      See Also:
        OdGiEdgeData
        OdGiFaceData
        OdGiVertexData
        Vectorization Primitives
  */
  virtual void mesh(OdInt32 rows,
    OdInt32 columns,
    const OdGePoint3d* pVertexList,
    const OdGiEdgeData* pEdgeData = NULL,
    const OdGiFaceData* pFaceData = NULL,
    const OdGiVertexData* pVertexData = NULL) = 0;
  
  /** Description:
      Introduces a shell into this vectorization context.  A shell
      is a set of faces that can contain holes.

      Arguments:
        nbVertex (I) Number of vertices in the shell.
        pVertexList (I) Pointer to an array of vertices that make up the shell.
        faceListSize (I) Number of entries in pFaceList.
        pFaceList (I) List of numbers that define the faces in the shell.  See
          Vectorization Primitives for more details.
        pEdgeData (I) Pointer to additional edge data (color, layer, etc.).
        pFaceData (I) Pointer to additional face data (color, layer, etc.).
        pVertexData (I) Additional vertex attributes (normals, colors, etc.). 

      See Also:
        OdGiEdgeData
        OdGiFaceData
        OdGiVertexData
        Vectorization Primitives
  */
  virtual void shell(OdInt32 nbVertex,
    const OdGePoint3d* pVertexList,
    OdInt32 faceListSize,
    const OdInt32* pFaceList,
    const OdGiEdgeData* pEdgeData = NULL,
    const OdGiFaceData* pFaceData = NULL,
    const OdGiVertexData* pVertexData = NULL) = 0;
  
  /** Description:
      Introduces a text string into this vectorization context.

      Arguments:
        position (I) Position of the text string.
        normal (I) Normal vector that defines the plane in which the text lies.
        direction (I) The X direction of the text string.
        height (I) Height of the text.
        width (I) Width of text, currently not supported.
        oblique (I) Oblique angle, currently not supported.
        msg (I) The text string (null terminated).
  */
  virtual void text(const OdGePoint3d& position,
    const OdGeVector3d& normal, 
    const OdGeVector3d& direction,
    double height, 
    double width, 
    double oblique, 
    const OdChar* msg) = 0;
  
  /** Arguments:
        length (I) Length of the supplied text string in bytes.
        raw (I) If true, then treat the string as raw text, in which case special characters such as %%D (degree symbol), etc. are not converted to the appropriate symbol, but are instead treated as literal strings.
        pStyle (I) Text style properties for this text string.
  */
  virtual void text(const OdGePoint3d& position,
    const OdGeVector3d& normal, 
    const OdGeVector3d& direction,
    const OdChar* msg, 
    OdInt32 length, 
    bool raw, 
    const OdGiTextStyle* pTextStyle) = 0;
  
  /** Description:
      Introduces an XLINE into this vectorization context.  An XLINE
      is a line unbounded in both directions, that passes through the 
      two supplied points.
  */
  virtual void xline(const OdGePoint3d&, const OdGePoint3d&) = 0;

  /** Description:
      Introduces a RAY into this vectorization context.  A RAY
      is a line that starts at the supplied point, and  extends unbounded 
      in the direction of the supplied direction vector.
  */
  virtual void ray(const OdGePoint3d& start, const OdGePoint3d& direction) = 0;
  
  /** Description:
      Introduces a NURBS curve into this vectorization context.
      The NURBS curve is not filled (outline only), 
      and the outline takes on the current drawing color.
  */
  virtual void nurbs(const OdGeNurbCurve3d& nurbs) = 0;

  /** Description:
      Introduces an elliptical arc into this vectorization context.  
      The arc takes on the current drawing color.  
      kOdGiArcSimple type arcs are not filled, while
      kOdGiArcSector and kOdGiArcChord type arcs are filled.

      Arguments:
        arc (I) Elliptical arc data.
        pEndPointsOverrides (I) Override point values to be used for the first
          and last points of the tessellated arc.
        arcType (I) Type of arc, either kOdGiArcSimple, kOdGiArcSector or kOdGiArcChord.
  */
  virtual void ellipArc(const OdGeEllipArc3d& arc,
    const OdGePoint3d* pEndPointsOverrides = 0,
    OdGiArcType arcType = kOdGiArcSimple) = 0;

  /** Description:
      Causes the passed in object to be vectorized into this vectorization context.

      Remarks:
      Implementations of this method are expected to do the following:

        o Make a call to the OdGiDrawable::setAttributes, to set attribute information for the object.
        o Call worldDraw on the drawable object, to vectorize it into this context.
        o Call viewportDraw on the drawable object for each viewport, if worldDraw returned false.
  */
  virtual void draw(const OdGiDrawable*) = 0;

  /** Description:
      Pushes a clip boundary onto the current clip stack, which may affect the vectorization of 
      other graphical entitities until the clip boundary is removed with a call to popClipBoundary.
      This function can be used to clip a block insertion, as is required for xclip functionality.

      See Also:
      OdGiClipBoundary
  */
  virtual void pushClipBoundary(OdGiClipBoundary* pBoundary) = 0;

  /** Description:
      Pops the topmost clip boundary from the stack (clip boundaries are pushed onto the stack
      by the pushClipBoundary function).
  */
  virtual void popClipBoundary() = 0;

  /** Description:
      Introduces a line into this vectorization context.  The line is specified in WCS, 
      so the current model transformation is not applied to it.
  */
  virtual void worldLine(const OdGePoint3d pnts[2]) = 0;
};

#include "DD_PackPop.h"

#endif


