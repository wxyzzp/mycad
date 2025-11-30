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



#ifndef _OD_DB_POLYFACE_MESH_
#define _OD_DB_POLYFACE_MESH_

#include "DD_PackPush.h"

#include "DbEntity.h"
#include "DbObjectIterator.h"

class OdDbPolyFaceMeshVertex;
class OdDbSequenceEnd;
class OdDbFaceRecord;
typedef OdSmartPtr<OdDbPolyFaceMeshVertex> OdDbPolyFaceMeshVertexPtr;
typedef OdSmartPtr<OdDbSequenceEnd> OdDbSequenceEndPtr;

/** Description:
    Represents a PolyFaceMesh entity in an OdDbDatabase.  
    
    Remarks:
    A PolyFaceMesh object consists of a list of vertices, and a list of 
    FaceRecord objects that use these vertices to create 3D faces.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbPolyFaceMesh : public OdDbEntity
{
public:
  ODDB_DECLARE_MEMBERS(OdDbPolyFaceMesh);

  /** Description:
      Constructor (no arguments).
  */
  OdDbPolyFaceMesh();
  
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
      Returns the number of vertices in this entity (DXF 71).
  */
  OdInt16 numVertices() const;

  /** Description:
      Returns the number fo faces in this entity (DXF 72).
  */
  OdInt16 numFaces() const;
  
  /** Description:
      Appends the specified vertex onto this entity, returning the Object ID of the newly appended
      vertex.

      Arguments:
      pVertex (I) Vertex to append.

      Return Value:
      the Object ID of the newly appended face record.
  */
  OdDbObjectId appendVertex(OdDbPolyFaceMeshVertex* pVertex);
  
  /** Description:
      Appends the specified face record onto this entity, returning the Object ID of the newly appended
      face record.

      Return Value:
      the Object ID of the newly appended face record.

      Arguments:
      pFaceRecord (I) Face record to append.
  */
  OdDbObjectId appendFaceRecord(OdDbFaceRecord* pFaceRecord);
  
  /** Description:
      Opens a vertex owned by this entity.

      Arguments:
      subObjId (I) Object ID of vertex to be opened.
      mode (I) Mode in which to open the vertex.
      openErasedOne (I) If true, the specifed vertex will be opened even if it
      has been erased, otherwise erased vertices will not be opened.

      Return Value:
      Smart pointer to the opened object if successful, otherwise a null smart pointer.
  */
  OdDbPolyFaceMeshVertexPtr openVertex(OdDbObjectId subObjId, OdDb::OpenMode mode, bool openErasedOne = false);
  
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

  virtual bool worldDraw(OdGiWorldDraw* pWd) const;
  
  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;

  virtual void dxfOut(OdDbDxfFiler* pFiler) const;

  virtual OdResult dxfIn(OdDbDxfFiler* pFiler);

  virtual void getClassID(void** pClsid) const;

  virtual OdResult dxfInFields_R12(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields_R12(OdDbDxfFiler* pFiler) const;

  void subClose();

  virtual bool isPlanar() const;
  
  virtual OdResult getPlane(OdGePlane&, OdDb::Planarity&) const;

  virtual OdResult transformBy(const OdGeMatrix3d& xform);

  OdResult getGeomExtents(OdGeExtents3d& extents) const;

  /** Remarks:
      Creates and returns a set of non-database resident OdDbFace entities that
      approximate this OdDbPolyFaceMesh.
  */
  virtual OdResult explode(OdRxObjectPtrArray& entitySet) const;
};

typedef OdSmartPtr<OdDbPolyFaceMesh> OdDbPolyFaceMeshPtr;

#include "DD_PackPop.h"

#endif

