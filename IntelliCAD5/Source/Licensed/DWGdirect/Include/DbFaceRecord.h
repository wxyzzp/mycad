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



#ifndef _OD_DB_FACERECORD_
#define _OD_DB_FACERECORD_

#include "DD_PackPush.h"

#include "DbVertex.h"

/** Description:
    Represents a face record in an OdDbDatabase, which is used to group together
    a set of vertices, forming a face within an OdDbPolyFaceMesh.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbFaceRecord : public OdDbEntity
{
public:
  ODDB_DECLARE_MEMBERS(OdDbFaceRecord);

  OdDbFaceRecord();

  /** Description:
      Gets the index of the vertex that will is used for the specified corner point
      in the face represented by this OdDbFaceRecord (DXF 71-74).

      Arguments:
      cornerIndex (I) Specifies which corner point of the face is being retrieved (0-3).

      Return Value:
      The index of the vertex for this corner point (a negative value indicates that 
      the associated edge is invisible).
      This is an index into the vertex list of the parent OdDbPolyFaceMesh object.
  */
  OdInt16 getVertexAt(int cornerIndex) const;

  /** Description:
      Sets the index of the vertex that will be used for the specified corner point
      in the face represented by this OdDbFaceRecord (DXF 71-74).

      Arguments:
      cornerIndex (I) Specifies which corner point of the face is being set (0-3).
      vtxIdx (I) Specifies the index of the vertex to use for this corner point.
      This is an index into the vertex list of the parent OdDbPolyFaceMesh object.
  */
  void setVertexAt(int cornerIndex, OdInt16 vtxIdx);
  
  /** Description:
      Determines if the specified edge is visible (sign of DXF 71-74).

      Arguments:
      faceIndex (I) Index of the edge for which visibility is being determined.

      Return Value:
      true if the specified edge is visible, false otherwise.
  */
  bool isEdgeVisibleAt(int faceIndex) const;
  
  /** Description:
      Sets the visibility property to on for the specified edge (sign of DXF 71-74).
  */
  void makeEdgeVisibleAt(int faceIndex);

  /** Description:
      Sets the visibility property to off for the specified edge (sign of DXF 71-74).
  */
  void makeEdgeInvisibleAt(int edgeIndex);

  virtual OdResult dxfInFields_R12(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields_R12(OdDbDxfFiler* pFiler) const;

  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;

  virtual OdResult subErase(bool erasing);
};

typedef OdSmartPtr<OdDbFaceRecord> OdDbFaceRecordPtr;

#include "DD_PackPop.h"

#endif

