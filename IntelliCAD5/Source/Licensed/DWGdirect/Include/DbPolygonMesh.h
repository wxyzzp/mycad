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



#ifndef _OD_DB_POLYGON_MESH_
#define _OD_DB_POLYGON_MESH_

#include "DD_PackPush.h"

#include "DbEntity.h"
#include "DbObjectIterator.h"
#include "DbPolygonMeshVertex.h"

class OdDbSequenceEnd;
typedef OdSmartPtr<OdDbPolygonMeshVertex> OdDbPolygonMeshVertexPtr;
typedef OdSmartPtr<OdDbSequenceEnd> OdDbSequenceEndPtr;

namespace OdDb
{
  enum PolyMeshType
  {
    kSimpleMesh        = 0,
    kQuadSurfaceMesh   = 5,
    kCubicSurfaceMesh  = 6,
    kBezierSurfaceMesh = 8
  };
}

/** Description:
    Represents a PolygonMesh entity in an OdDbDatabase.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbPolygonMesh : public OdDbEntity
{
public:
  ODDB_DECLARE_MEMBERS(OdDbPolygonMesh);

  /** Description:
      Constructor (no arguments).
  */
  OdDbPolygonMesh();
  
  /* OdDbPolygonMesh(OdDb::PolyMeshType pType,
       OdInt16 mSize,
       OdInt16 nSize,
       const OdGePoint3dArray& vertices,
       bool mClosed = true,
       bool nClosed = true);
  */

  /** Description:
      Sets the color value for this entity (DXF 62).

      Arguments:
      color (I) Color value to set.
      doSubents (I) If true, apply the color change to all sub-entities owned by
      this entity, otherwise do not apply the color to sub-entities.
  */
  OdResult setColor(const OdCmColor &color, bool doSubents = false);

  /** Description:
      Sets the color index for this entity (DXF 62).

      Arguments:
      color (I) Color index to set.
      doSubents (I) If true, apply the color change to all sub-entities owned by
      this entity, otherwise do not apply the color to sub-entities.
  */
  OdResult setColorIndex(OdUInt16 color, bool doSubents = false);

  OdResult setTransparency(const OdCmTransparency& trans, bool doSubents = true);

  /** Description:
      Returns the mesh type of this entity (DXF 75).
  */
  OdDb::PolyMeshType polyMeshType() const;

  /** Description:
      Sets the mesh type of this entity (DXF 75).
  */
  void setPolyMeshType(OdDb::PolyMeshType);

  /** Description:
      TBC.
  */
  void convertToPolyMeshType(OdDb::PolyMeshType newVal);
  
  /** Description:
      Returns the number of vertices in the M direction for this entity (DXF 71).
  */
  OdInt16 mSize() const;

  /** Description:
      Sets the number of vertices in the M direction for this entity (DXF 71).
  */
  void setMSize(OdInt16);
  
  /** Description:
      Returns the number of vertices in the N direction for this entity (DXF 72).
  */
  OdInt16 nSize() const;

  /** Description:
      Sets the number of vertices in the N direction for this entity (DXF 72).
  */
  void setNSize(OdInt16);
  
  /** Description:
      Returns the closed flag in the M direction for this entity (DXF 70, bit 0x01).
  */
  bool isMClosed() const;

  /** Description:
      Sets the closed flag in the M direction for this entity (DXF 70, bit 0x01).
  */
  void makeMClosed();

  /** Description:
      Clears the closed flag in the M direction for this entity (DXF 70, bit 0x01).
  */
  void makeMOpen();
  
  /** Description:
      Returns the closed flag in the N direction for this entity (DXF 70, bit 0x20).
  */
  bool isNClosed() const;

  /** Description:
      Sets the closed flag in the N direction for this entity (DXF 70, bit 0x20).
  */
  void makeNClosed();

  /** Description:
      Clears the closed flag in the N direction for this entity (DXF 70, bit 0x20).
  */
  void makeNOpen();
  
  /** Description:
      Returns surface density in the M direction for this entity (DXF 73).
  */
  OdInt16 mSurfaceDensity() const;

  /** Description:
      Sets surface density in the M direction for this entity (DXF 73).
  */
  void setMSurfaceDensity(OdInt16);
  
  /** Description:
      Returns surface density in the N direction for this entity (DXF 74).
  */
  OdInt16 nSurfaceDensity() const;

  /** Description:
      Sets surface density in the N direction for this entity (DXF 74).
  */
  void setNSurfaceDensity(OdInt16);
  
  /** Description:
      TBC.
  */
  void straighten();

  /** Description:
      TBC.
  */
  void surfaceFit();

  /** Description:
      TBC.
  */
  void surfaceFit(OdDb::PolyMeshType surfType, OdInt16 surfu, OdInt16 surfv);
  
  /** Description:
      Appends the specified vertex onto this entity, returning the Object ID of the newly appended
      vertex.

      Return Value:
      the Object ID of the newly appended vertex.

      Arguments:
      pVertex (I) Vertex to append.
      vType (I) Vertex type.
  */
  OdDbObjectId appendVertex(OdDbPolygonMeshVertex* pVertex, OdDb::Vertex3dType vType = OdDb::k3dSimpleVertex);
  
  /** Description:
      Opens a vertex owned by this entity.

      Arguments:
      vertId (I) Object ID of vertex to be opened.
      mode (I) Mode in which to open the vertex.
      openErasedOne (I) If true, the specifed vertex will be opened even if it
      has been erased, otherwise erased vertices will not be opened.

      Return Value:
      Smart pointer to the opened object if successful, otherwise a null smart pointer.
  */
  OdDbPolygonMeshVertexPtr openVertex(OdDbObjectId vertId, OdDb::OpenMode mode, bool openErasedOne = false);
  
  /** Description:
      Opens the sequence end object owned by this entity, in the specified mode.

      Return Value:
      Smart pointer to the opened object if successful, otherwise a null smart pointer.
  */
  OdDbSequenceEndPtr openSequenceEnd(OdDb::OpenMode);
  
  /** Description:
      Returns a smart pointer to an iterator that can be used to traverse the vertices owned 
      by this entity.
  */
  OdDbObjectIteratorPtr vertexIterator() const;
  
  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;

  virtual void dxfOut(OdDbDxfFiler* pFiler) const;

  virtual OdResult dxfIn(OdDbDxfFiler* pFiler);

  virtual OdResult dxfInFields_R12(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields_R12(OdDbDxfFiler* pFiler) const;

  bool worldDraw(OdGiWorldDraw* pWd) const;

  void subClose();

  void getClassID(void** pClsid) const;

  virtual bool isPlanar() const;
  
  virtual OdResult getPlane(OdGePlane&, OdDb::Planarity&) const;

  virtual OdResult transformBy(const OdGeMatrix3d& xform);

  /** Remarks:
      Creates and returns a set of non-database resident OdDbFace entities that
      approximate this OdDbPolyFaceMesh.
  */
  virtual OdResult explode(OdRxObjectPtrArray& entitySet) const;
};

typedef OdSmartPtr<OdDbPolygonMesh> OdDbPolygonMeshPtr;

#include "DD_PackPop.h"

#endif

