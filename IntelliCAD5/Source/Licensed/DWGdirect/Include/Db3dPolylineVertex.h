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



#ifndef _OD_DB_3DPOLYLINE_VERTEX_
#define _OD_DB_3DPOLYLINE_VERTEX_

#include "DD_PackPush.h"

#include "DbVertex.h"

/** Description:
    Represents a vertex in a 3D polyline, within an OdDbDatabase.

    {group:OdDb_Classes} 
*/
class TOOLKIT_EXPORT OdDb3dPolylineVertex : public OdDbVertex
{
public:

  ODDB_DECLARE_MEMBERS(OdDb3dPolylineVertex);

  /** Description:
      Constructor (no arguments).
  */
  OdDb3dPolylineVertex();

  /** Description:
      Returns the type of this vertex.  
      
      Remarks:
      Vertex type can be one of:

        o k3dSimpleVertex - Normal vertex.
        o k3dControlVertex - Control point for a spline or curve fit mesh.
        o k3dFitVertex - Fit point for a spline or curve.
  */
  OdDb::Vertex3dType vertexType() const;

  /** Description:
      Sets the type of this vertex. (Newly created vertex has k3dSimpleVertex type.)

      Arguments:
      (I) vertexType one of k3dSimpleVertex, k3dControlVertex, k3dFitVertex.
  */
  void setVertexType (OdDb::Vertex3dType vertexType);

  /** Description:
      Returns the position of this vertex in WCS.
  */
  OdGePoint3d position() const;

  /** Description:
      Sets the position of this vertex in WCS.
  */
  void setPosition(const OdGePoint3d&);

  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual OdResult dxfInFields_R12(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields_R12(OdDbDxfFiler* pFiler) const;

  virtual OdResult getGeomExtents(OdGeExtents3d& extents) const;

  virtual OdResult transformBy(const OdGeMatrix3d& xform);


  /*
     OdDb3dPolylineVertex(const OdGePoint3d&);
  */
};

typedef OdSmartPtr<OdDb3dPolylineVertex> OdDb3dPolylineVertexPtr;

#include "DD_PackPop.h"

#endif


