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



#ifndef OD_DBPL_H
#define OD_DBPL_H

#include "DD_PackPush.h"

#include "DbCurve.h"
#include "Db2dPolyline.h"

class OdDb2dPolyline;
class OdGeLineSeg2d;
class OdGeLineSeg3d;
class OdGeCircArc2d;
class OdGeCircArc3d;

/** Description:
    Represents a "lightweight" polyline in an OdDbDatabase.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbPolyline : public OdDbCurve
{
public:
  ODDB_DECLARE_MEMBERS(OdDbPolyline);

  /** Description:
      Constructor (no arguments).
  */
  OdDbPolyline();

  /** Description:
      Fills this OdDbPolyline with data from the passed in OdDb2dPolyline.  This function should
      only be called for OdDbPolyline entities that are not database resident.

      Arguments:
      pPline (I) OdDb2dPolyline entity from which the polyline data will be copied.  This entity
      must k2dSimplePoly or k2dFitCurvePoly type.
      transferId (I) If true, this OdDbPolyline will take the place of the passed in
      OdDb2dPolyline in pPline's database (so pPline will be removed from it's database, and
      the OdDbPolyline will become database resident).
      If false, pPline will remain database resident, and this OdDb2dPolyline will remain non-database resident.

      Return Value:
      eOk upon success, otherwise an appropriate error code.
  */
  OdResult convertFrom(OdDbEntity* pPline, bool transferId = true);

  /** Description:
      Fills the passed in OdDb2dPolyline with data from this OdDbPolyline.

      Arguments:
      pPline (O) OdDb2dPolyline entity to which the polyline data will be copied.
      transferId (I) If true, this OdDbPolyline will be removed from its database, and the
      newly initialized pPline object will take its place.
      If false, this OdDb2dPolyline will remain database resident and pPline will not be added to the
      database.

      Return Value:
      eOk upon success, otherwise an appropriate error code.
  */
  OdResult convertTo(OdDb2dPolyline* pPline, bool transferId = true);

  /** Description:
      Returns a point at a specified index, in WCS (DXF 10 as WCS).

      Arguments:
      index (I) Index of point to return.
      pt (O) Receives the point at the specified index.
  */
  void getPointAt(unsigned int index, OdGePoint3d& pt) const;

  enum SegType
  {
    kLine,        // Straight segment with length greater than 0.
    kArc,         // Arc segment with length greater than 0.
    kCoincident,  // Segment with 0 length.
    kPoint,       // Polyline with a single vertex.
    kEmpty        // No vertices are present.
  };

  /** Description:
      Returns the type of the segment that begins with the specified vertex.
  */
  SegType segType(unsigned int index) const;

  /** Description:
      Returns the line segment starting at the specified index.
  */
  void getLineSegAt(unsigned int index, OdGeLineSeg2d& ln) const;

  /** Description:
      Returns the line segment starting at the specified index.
  */
  void getLineSegAt(unsigned int index, OdGeLineSeg3d& ln) const;

  /** Description:
      Returns the arc segment starting at the specified index.
  */
  void getArcSegAt(unsigned int index, OdGeCircArc2d& arc) const;

  /** Description:
      Returns the arc segment starting at the specified index.
  */
  void getArcSegAt(unsigned int index, OdGeCircArc3d& arc) const;

  /** Description:
      TBC.
  */
  virtual bool onSegAt(unsigned int index, const OdGePoint2d& pt2d, double& param) const;

  /** Description:
      Sets the closed flag for this entity (DXF 70, bit 0x01).
  */
  void setClosed(bool);

  /** Description:
      Sets the PLINEGEN flag for this entity (DXF 70, bit 0x80).
      If PLINEGEN is true, linetype generation will be patterned across the entire polyline, 
      rather than being done for each segment individually.
  */
  void setPlinegen(bool);

  /** Description:
      Sets the elevation for this entity (DXF 38).
  */
  void setElevation(double);

  /** Description:
      Sets the thickness for this entity (DXF 39).
  */
  void setThickness(double);

  /** Description:
      Sets the constant width for this entity (DXF 43).
  */
  void setConstantWidth(double);

  /** Description:
      Sets the normal vector for this entity (DXF 210).
  */
  void setNormal(const OdGeVector3d&);

  /** Description:
      Returns true if this entity has no bulges.
  */
  bool isOnlyLines() const;

  /** Description:
      Returns the PLINEGEN flag for this entity (DXF 70, bit 0x80).
      If PLINEGEN is true, linetype generation will be patterned across the entire polyline, 
      rather than being done for each segment individually.
  */
	bool hasPlinegen() const;

  /** Description:
      Returns the elevation for this entity (DXF 38).
  */
  double elevation() const;

  /** Description:
      Returns the thickness for this entity (DXF 39).
  */
  double thickness() const;

  /** Description:
      Returns the constant width for this entity (DXF 43).
  */
  double getConstantWidth() const;

  /** Description:
      Returns the normal vector for this entity (DXF 210).
  */
  OdGeVector3d normal() const;

  /** Description:
      Adds a vertex to this entity at the specified index.

      Arguments:
      index (I) Index at which to add the vertex.
      pt (I) Coordinate position of the vertex to be added.
      bulge (I) Bulge value for the vertex.
      startWidth (I) Start width for the vertex.
      endWidth (I) End width for the vertex.
  */
  void addVertexAt(unsigned int index,
    const OdGePoint2d& pt,
    double bulge = 0.,
    double startWidth = -1.,
    double endWidth = -1);

  /** Description:
      Removes the specified vertex from this entity.
  */
  void removeVertexAt(unsigned int index);

  /** Description:
      Returns the number of vertices in the entity (DXF 90).
  */
  unsigned int numVerts() const;

  /** Description:
      Returns a specified point from this entity.
  */
  void getPointAt(unsigned int index, OdGePoint2d& pt) const;

  /** Description:
      Returns the bulge value for a specified point.
  */
  double getBulgeAt(unsigned int index) const;

  /** Description:
      Returns the start and end widths for a specified point.
  */
  void getWidthsAt(unsigned int index, double& startWidth,  double& endWidth) const;

  /** Description:
      Sets the coordinate point value at a specified index.
  */
  void setPointAt(unsigned int index, const OdGePoint2d& pt);

  /** Description:
      Sets the bulge value at a specified index.
  */
  void setBulgeAt(unsigned int index, double bulge);

  /** Description:
      Sets the start and end width values at a specified index.
  */
  void setWidthsAt(unsigned int index, double startWidth, double endWidth);

  /** Description:
      Optimizes memory usage for this entity.
  */
  void minimizeMemory();

  /** Description:
      Maximizes memory usage for this entity so that any modification made to the entity will
      take as little time as possible.
  */
  void maximizeMemory();

  /** Description:
      Clears this entities vertex data.

      Arguments:
      reuse (I) If true, memory for numVerts vertices will be left intact.  If false,
      all memory used by this entity will be released.
      numVerts (I) Determines the amount of memory to reuse, if "reuse" parameter is true.

      Remarks:
      If numVerts is greater than the existing size of the internal point list, the list
      will be expanded to hold numVerts items.
  */
  void reset(bool reuse, unsigned int numVerts);

  /** Description:
      Returns true if the vertices in this entity contain bulge data, false otherwise.
  */
  bool hasBulges() const;

  /** Description:
      Returns true if the vertices in this entity contain start and end width data, false otherwise.
  */
  bool hasWidth() const;

  virtual OdResult transformBy(const OdGeMatrix3d& xform);

  virtual bool worldDraw(OdGiWorldDraw* mode) const;

  virtual void viewportDraw(OdGiViewportDraw* mode) const;

  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);

  virtual OdResult dxfInFields_R12(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;

  virtual void getClassID(void** pClsid) const;

  virtual bool isPlanar() const;

  virtual OdResult getPlane(OdGePlane&, OdDb::Planarity&) const;


  /** Description:
      OdDbCurveMethods
  */

  /** Description:
      Returns the closed flag for this entity (DXF 70, bit 0x01).
  */
  virtual bool isClosed() const;

  /** Description:
      TBC.
  */
  virtual bool isPeriodic() const;

  /** Description:
      TBC.
  */
  virtual OdResult getStartParam(double&) const;

  /** Description:
      TBC.
  */
  virtual OdResult getEndParam (double&) const;

  /** Description:
      TBC.
  */
  virtual OdResult getStartPoint(OdGePoint3d&) const;

  /** Description:
      TBC.
  */
  virtual OdResult getEndPoint(OdGePoint3d&) const;

  virtual OdResult getPointAtParam(double, OdGePoint3d&) const;

  /** Description:
      Returns the parameter of specified point of this curve.

      Arguments:
      pt (I) Point of the curve.
      param(O) Computed parameter.

      Return Value:
      eOk if successful, otherwise an appropriate error code.
  */
  virtual OdResult getParamAtPoint(const OdGePoint3d& pt, double& param) const;

  /** Remarks:
      Creates and returns a set of non-database resident OdDbLine and OdDbArc entities that
      approximate this OdDbPolyline.
  */
  virtual OdResult explode(OdRxObjectPtrArray& entitySet) const;

};

typedef OdSmartPtr<OdDbPolyline> OdDbPolylinePtr;

#include "DD_PackPop.h"

#endif //OD_DBPL_H


