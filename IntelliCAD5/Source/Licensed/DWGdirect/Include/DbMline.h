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



#ifndef OD_DBMLINE_H
#define OD_DBMLINE_H

#include "DD_PackPush.h"

#include "DbEntity.h"
#include "Ge/GeVoidPointerArray.h"

/** Description:

    {group:Other_Classes}
*/
struct Mline
{
  typedef OdInt8 MlineJustification;
  enum
  {
    kTop = 0,
    kZero = 1,
    kBottom = 2
  };
  enum
  {
    kOpen = 0,
    kClosed = 1,
    kMerged = 2
  };
};

class OdGePlane;

struct OdMLSegment
{
  OdGeDoubleArray m_AreaFillParams;
  OdGeDoubleArray m_SegParams;
};

typedef OdArray<OdMLSegment> OdMLSegmentArray;

/** Description:
    Represents an MLine entity in an OdDbDatabase.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbMline : public OdDbEntity
{
public:
  ODDB_DECLARE_MEMBERS(OdDbMline);

  /** Description:
      Constructor (no arguments).
  */
  OdDbMline();

  /** Description:
      Sets the MLine Style used by this entity (DXF 340).
  */
  void setStyle(const OdDbObjectId &newStyleId);

  /** Description:
      Returns the Object ID of the MLine Style used by this entity (DXF 340).
  */
  OdDbObjectId style() const;

  /** Description:
      Sets the justification for this entity (DXF 70).
  */
  void setJustification(Mline::MlineJustification newJust);

  /** Description:
      Returns the justification for this entity (DXF 70).
  */
  Mline::MlineJustification justification() const;

  /** Description:
      Sets the scale for this entity (DXF 40).
  */
  void setScale(double newScale);

  /** Description:
      Returns the scale for this entity (DXF 40).
  */
  double scale() const;

  /** Description:
      Returns the normal vector for this entity (DXF 210).
  */
  OdGeVector3d normal() const;

  /** Description:
      Sets the normal vector for this entity (DXF 210).
  */
  void setNormal(const OdGeVector3d & newNormal);

  /** Description:
      Appends a vertex onto the end of this MLine.

      Arguments:
      newVertex (I) Vertex to append.
  */
  void appendSeg(const OdGePoint3d& newVertex);

  /** Description:
      Removes the last vertex from this MLine.

      Arguments:
      lastVertex (O) Receives the last vertex.
  */
  void removeLastSeg(OdGePoint3d& lastVertex);

  /** Description:
      Sets new coordinate values for the specified index.

      Arguments:
      index (I) Index at which new coordinate values will be set.
      newPosition (I) New coordinate position for specified vertex.
  */
  void moveVertexAt(int index, const OdGePoint3d& newPosition);

  /** Description:
      Sets the open/closed status of this MLine (DXF 71, bit 0x02).
  */
  void setClosedMline(bool closed);

  /** Description:
      Returns the open/closed status of this MLine (DXF 71, bit 0x02).
  */
  bool closedMline() const;

  /** Description:
      Sets the "Suppress Start Caps" flag for this MLine (DXF 71, bit 0x04).
  */
  void setSupressStartCaps(bool supressIt);

  /** Description:
      Returns the "Suppress Start Caps" flag for this MLine (DXF 71, bit 0x04).
  */
  bool supressStartCaps() const;

  /** Description:
      Sets the "Suppress End Caps" flag for this MLine (DXF 71, bit 0x08).
  */
  void setSupressEndCaps(bool supressIt);

  /** Description:
      Returns the "Suppress End Caps" flag for this MLine (DXF 71, bit 0x08).
  */
  bool supressEndCaps() const;

  /** Description:
      Returns the number of vertices in this MLine (DXF 72).
  */
  int numVertices() const;

  /** Description:
      Returns a specified vertex from this MLine (DXF 10 or 11).
  */
  OdGePoint3d vertexAt(int index) const;

  /** Description:
      Returns the direction vector of the segment starting at the specified vertex (DXF 12).
  */
  OdGeVector3d axisAt(int index) const;

  /** Description:
      Returns the direction vector of the miter at the specified vertex (DXF 13).
  */
  OdGeVector3d miterAt(int index) const;

  /** Description:
      Returns the element and area fill parameters (DXF 41 and 42)
  */
  void getParametersAt(int index, OdMLSegmentArray& params) const;

  /** Description:
      Sets the element and area fill parameters (DXF 41 and 42)
  */
  void setParametersAt(int index, const OdMLSegmentArray& params);

  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;

  virtual OdResult transformBy(const OdGeMatrix3d& xform);

  virtual bool worldDraw(OdGiWorldDraw* pWd) const;

  virtual OdResult explode(OdRxObjectPtrArray& entitySet) const; /* Replace OdRxObjectPtrArray */

  virtual void subClose();

  virtual void getClassID(void** pClsid) const;

  /*
  
    int element(const OdGePoint3d & pt) const;

    void getClosestPointTo(const OdGePoint3d& givenPoint,
      OdGePoint3d& pointOnCurve,
      bool extend, bool excludeCaps = false) const;

    void getClosestPointTo(const OdGePoint3d& givenPoint,
      const OdGeVector3d& normal, OdGePoint3d& pointOnCurve,
      bool extend, bool excludeCaps = false) const;
  
    OdResult getPlane(OdGePlane&) const;
    bool getGeomExtents(OdGeExtents3d&) const;

    void getSubentPathsAtGsMarker(OdDb::SubentType type, int gsMark,
      const OdGePoint3d& pickPoint, const OdGeMatrix3d& viewXform,
      int& numPaths, OdDbFullSubentPath*& subentPaths,
      int numInserts = 0, OdDbObjectId* entAndInsertStack = NULL) const;

    void getGsMarkersAtSubentPath(const OdDbFullSubentPath& subPath, OdDbIntArray& gsMarkers) const;

    OdDbEntity* subentPtr(const OdDbFullSubentPath& id) const;
  
  */

};

typedef OdSmartPtr<OdDbMline> OdDbMlinePtr;

#include "DD_PackPop.h"

#endif


