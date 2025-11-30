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



#ifndef _OD_DB_2DPOLYLINE_
#define _OD_DB_2DPOLYLINE_

#include "DD_PackPush.h"

#include "DbCurve.h"
#include "DbObjectIterator.h"
#include "Db2dVertex.h"

class OdGePoint3d;
class OdDbSequenceEnd;
template <class T> class OdSmartPtr;
typedef OdSmartPtr<OdDbSequenceEnd> OdDbSequenceEndPtr;

namespace OdDb
{
  enum Poly2dType
  {
    k2dSimplePoly      = 0,
    k2dFitCurvePoly    = 1,
    k2dQuadSplinePoly  = 2,
    k2dCubicSplinePoly = 3
  };
}


/** Description:
    Represents a 2D polyline entity in an OdDbDatabase.

    {group:OdDb_Classes} 
*/
class TOOLKIT_EXPORT OdDb2dPolyline: public OdDbCurve
{
public:

  ODDB_DECLARE_MEMBERS(OdDb2dPolyline);

  /** Description:
      Constructor (no arguments).
  */
  OdDb2dPolyline();

  /** Description:
      Sets the color for this object, as an OdCmColor.

      Arguments:
      color (I) Color to set for this object.
      doSubents (I) If true, set the color for any entities owned by this entity.
  */
  OdResult setColor(const OdCmColor &color, bool doSubents = false);

  /** Description:
      Sets the color for this object, as a color index.

      Arguments:
      colorInd (I) Color index to set for this object.
      doSubents (I) If true, set the color for any entities owned by this entity.
  */
  OdResult setColorIndex(OdUInt16 colorInd, bool doSubents = false);

  OdResult setTransparency(const OdCmTransparency& trans, bool doSubents = true);
  
  /** Description:
      Returns the type of the polyline. 
      

      Remarks:
        Type will be one of the following:

        k2dSimplePoly - Standard polyline.
        k2dFitCurvePoly - Curve fit polyline (DXF 70, bit 0x02).
        k2dQuadSplinePoly - Quadratic spline fit polyline (DXF 70, bit 0x04, and DXF 75 set to 5).
        k2dCubicSplinePoly - Cubic spline fit polyline (DXF 70, bit 0x04, and DXF 75 set to 6).
  */
  OdDb::Poly2dType polyType() const;

  /** Description:
      Sets the type of the polyline. 
      

      Remarks:
      Type will be one of the following:

        k2dSimplePoly - Standard polyline.
        k2dFitCurvePoly - Curve fit polyline (DXF 70, bit 0x02).
        k2dQuadSplinePoly - Quadratic spline fit polyline (DXF 70, bit 0x04, and DXF 75 set to 5).
        k2dCubicSplinePoly - Cubic spline fit polyline (DXF 70, bit 0x04, and DXF 75 set to 6).
  */
  void setPolyType(OdDb::Poly2dType);

  /** Description:
      Sets this polyline to be closed (DXF 70, sets bit 0x01).
  */
  void makeClosed();

  /** Description:
      Sets this polyline to be open (DXF 70, clears bit 0x01).
  */
  void makeOpen();

  /** Description:
      Returns the default segment starting width for this polyline (DXF 40).
  */
  double defaultStartWidth() const;

  /** Description:
      Sets the default segment starting width for this polyline (DXF 40).
  */
  void setDefaultStartWidth(double);

  /** Description:
      Returns the default segment ending width for this polyline (DXF 41).
  */
  double defaultEndWidth() const;

  /** Description:
      Sets the default segment ending width for this polyline (DXF 41).
  */
  void setDefaultEndWidth(double);

  /** Description:
      Returns the thickness of the polyline (depth along the extrusion direction)
      (DXF 39).
  */
  double thickness() const;

  /** Description:
      Sets the thickness of the polyline (depth along the extrusion direction)
      (DXF 39).
  */
  void setThickness(double);

  /** Description:
      Returns the normal for this polyline in WCS (DXF 210).
  */
  OdGeVector3d normal() const;

  /** Description:
      Sets the normal for this polyline in WCS (DXF 210).
  */
  void setNormal(const OdGeVector3d&);

  /** Description:
      Returns the elevation, or OCS Z axis value, of this polyline (DXF 30).
  */
  double elevation() const;

  /** Description:
      Sets the elevation, or OCS Z axis value, of this polyline (DXF 30).
  */
  void setElevation(double);

  /** Description:
      Returns true if linetype generation is on for this polyline (DXF 70, bit 0x80).
      Linetype generation on indicates that the linetype pattern of this polyline will
      be continuously generated across all vertices, rather than restarting at each
      vertex.
  */
  bool isLinetypeGenerationOn() const;

  /** Description:
      Sets the linetype generation to on for this polyline (DXF 70, bit 0x80).
      Linetype generation on indicates that the linetype pattern of this polyline will
      be continuously generated across all vertices, rather than restarting at each
      vertex.
  */
  void setLinetypeGenerationOn();

  /** Description:
      Sets the linetype generation to off for this polyline (DXF 70, bit 0x80).
      Linetype generation on indicates that the linetype pattern of this polyline will
      be continuously generated across all vertices, rather than restarting at each
      vertex.
  */
  void setLinetypeGenerationOff();

  /** Description:
      Removes all spline-fit and curve-fit vertices from this polyline.  All remaining
      vertices will be simple vertices.
  */
  void straighten();

  /** Description:
      Appends the given vertex to the end of this polyline.
      This polyline becomes the owner of the given vertex, and the calling
      application must release the vertex after this function returns
      (this function does not change the open status of the vertex).

      Arguments:
      pVertex (I) Vertex to be appended.

      Return Value:
      Object ID of appended vertex.
  */
  OdDbObjectId appendVertex(OdDb2dVertex* pVertex);

  /** Description:
      Inserts a vertex into this polyline immediately after a specified vertex.
      This polyline must be
      added to a database before calling this function.  This polyline becomes the
      owner of the given vertex, and the calling application must release the vertex
      after this function returns (this function does not change the open status
      of the vertex).

      Arguments:
      indexVertId (I) Object ID of vertex after which to insert the new vertex.
      pNewVertex (I) Vertex to be inserted.

      Remarks:
      To insert at the beginning of the polyline, a null Object ID can
      be passed for indexVertId.

      Return Value:
      The Object ID of the newly inserted vertex.
  */
  OdDbObjectId insertVertexAt(const OdDbObjectId& indexVertId, OdDb2dVertex* pNewVertex);

  /** Description:
      Inserts a vertex into this polyline immediately after a specified vertex.
      This polyline becomes the owner of the given vertex, and the calling
      application must release the vertex after this function returns
      (this function does not change the open status of the vertex).

      Arguments:
      pIndexVert (I) Pointer to vertex after which to insert the new vertex.
      pNewVertex (I) Vertex to be inserted.

      Remarks:
      To insert at the beginning of the polyline, a null pointer can
      be passed for pIndexVert.

      Return Value:
      The Object ID of the newly inserted vertex.
  */
  OdDbObjectId insertVertexAt(const OdDb2dVertex* pIndexVert, OdDb2dVertex* pNewVertex);

  /** Description:
      Opens the vertex specfied by the given Object ID.

      Arguments:
      vertId (I) Object ID of vertex to be opened.
      mode (I) Mode in which to open the vertex.
      openErasedOne (I) If true, the specifed vertex will be opened even if it
      has been erased, otherwise erased vertices will not be opened.

      Return Value:
      Smart pointer to the opened object if successful, otherwise a null smart pointer.

      See Also:
      OdDbObjectId::openObject
  */
  OdDb2dVertexPtr openVertex(const OdDbObjectId &vertId, OdDb::OpenMode mode, bool openErasedOne = false);

  /** Description:
      Opens the OdDbSequenceEnd object associated with this polyline.

      Arguments:
      mode (I) Mode in which to open the sequence end object.

      Return Value:
      Smart pointer to the opened object if successful, otherwise a null smart pointer.

      See Also:
      OdDbObjectId::openObject
  */
  OdDbSequenceEndPtr openSequenceEnd(OdDb::OpenMode mode);

  /** Description:
      Returns an iterator that can be used to iterate through the vertices owned
      by this polyline.
  */
  OdDbObjectIteratorPtr vertexIterator() const;

  /** Description:
      Returns the WCS position of the passed in vertex.
  */
  OdGePoint3d vertexPosition(const OdDb2dVertex& vert) const;

  virtual bool worldDraw(OdGiWorldDraw* pWd) const;

  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;

  virtual OdResult dxfInFields_R12(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields_R12(OdDbDxfFiler* pFiler) const;

  virtual void dxfOut(OdDbDxfFiler* pFiler) const;

  virtual OdResult dxfIn(OdDbDxfFiler* pFiler);

  virtual void subClose();

  /** Remarks:
      Creates and returns a set of non-database resident OdDbLine entities that
      approximate this OdDb2dPolyline.
  */
  virtual OdResult explode(OdRxObjectPtrArray& entitySet) const;

  virtual OdResult transformBy(const OdGeMatrix3d& xform);

  virtual void getClassID(void** pClsid) const;

  virtual bool isPlanar() const;

  virtual OdResult getPlane(OdGePlane& plane, OdDb::Planarity& planarity) const;


  // OdDbCurveMethods

  /** Description:
      Verifies whether polyline is closed or not (DXF 70, bit 0x01).
  */
  virtual bool isClosed() const;

  /** Description:
      Returns true if this curve is periodic, false otherwise.
  */
  virtual bool isPeriodic() const;

  /** Description:
      Returns the start parameter of this curve.

      Arguments:
      param (O) Receives the parameter value.

      Return Value:
      eOk if successful, otherwise an appropriate error code.
  */
  virtual OdResult getStartParam(double& param) const;

  /** Description:
      Returns the end parameter of this curve.

      Arguments:
      param (O) Receives the parameter value.

      Return Value:
      eOk if successful, otherwise an appropriate error code.
  */
  virtual OdResult getEndParam (double& param) const;

  /** Description:
      Returns the start point of this curve.

      Arguments:
      pt (O) Receives the point value.

      Return Value:
      eOk if successful, otherwise an appropriate error code.
  */
  virtual OdResult getStartPoint(OdGePoint3d& pt) const;

  /** Description:
      Returns the end point of this curve.

      Arguments:
      pt (O) Receives the point value.

      Return Value:
      eOk if successful, otherwise an appropriate error code.
  */
  virtual OdResult getEndPoint(OdGePoint3d& pt) const;

  /** Description:
      Returns the point at the specified parameter value.

      Arguments:
      param (I) Parameter value.
      retPt (O) Receives the point at the specified parameter value.

      Return Value:
      eOk if successful, otherwise an appropriate error code.
  */
  virtual OdResult getPointAtParam(double param, OdGePoint3d& retPt) const;

  /** Description:
      Returns the parameter of specified point of this curve.

      Arguments:
      pt (I) Point of the curve.
      param (O) Computed parameter.

      Return Value:
      eOk if successful, otherwise an appropriate error code.
  */
  virtual OdResult getParamAtPoint(const OdGePoint3d& pt, double& param) const;

  /*
      void convertToPolyType(OdDb::Poly2dType newVal);
      void splineFit();
      void splineFit(OdDb::Poly2dType splineType, OdInt16 splineSegs);
      void curveFit();
  */
};

typedef OdSmartPtr<OdDb2dPolyline> OdDb2dPolylinePtr;


/*

inline void OdDb2dPolyline::extend(double)
{
  return OdeNotApplicable;
}
*/
#include "DD_PackPop.h"

#endif


