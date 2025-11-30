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



#ifndef _OD_DB_2DVERTEX_
#define _OD_DB_2DVERTEX_

#include "DD_PackPush.h"

#include "DbVertex.h"

namespace OdDb
{
  enum Vertex2dType
  {
    k2dVertex          = 0,
    k2dSplineCtlVertex = 1,
    k2dSplineFitVertex = 2,
    k2dCurveFitVertex  = 3
  };
}

/** Description:
    Represents a vertex in a 2D polyline, within an OdDbDatabase.

    {group:OdDb_Classes} 
*/
class TOOLKIT_EXPORT OdDb2dVertex: public OdDbVertex
{
public:

  ODDB_DECLARE_MEMBERS(OdDb2dVertex);

  /** Description:
      Constructor (no arguments).
  */
  OdDb2dVertex();

  /** Description:
      Returns the type of this vertex.  
      
      Remarks:
      Vertex type can be one of:

        o k2dVertex - Normal 2D vertex.
        o k2dSplineCtlVertex - Control point for a spline or curve-fit polyline.
        o k2dSplineFitVertex - Spline fit point.
        o k2dCurveFitVertex - Curve fit point.
  */
  OdDb::Vertex2dType vertexType() const;

  /** Description:
      Sets the type of this vertex. (Newly created vertex has k2dVertex type.)

      Arguments:
        vertexType (I) one of k2dVertex, k2dSplineCtlVertex, k2dSplineFitVertex, k2dCurveFitVertex.
  */
  void setVertexType (OdDb::Vertex2dType vertexType);

  /** Description:
      Returns the position of this vertex in OCS (DXF 10).
  */
  OdGePoint3d position() const;

  /** Description:
      Sets the position of this vertex in OCS (DXF 10).
  */
  void setPosition(const OdGePoint3d&);

  /** Description:
      Returns the starting width of this vertex (DXF 40).
  */
  double startWidth() const;

  /** Description:
      Sets the starting width of this vertex (DXF 40).
  */
  void setStartWidth(double newVal);

  /** Description:
      Returns the ending width of this vertex (DXF 41).
  */
  double endWidth() const;

  /** Description:
      Sets the ending width of this vertex (DXF 41).
  */
  void setEndWidth(double newVal);

  /** Description:
      Returns the bulge value of this vertex (DXF 42).
  */
  double bulge() const;

  /** Description:
      Sets the bulge value of this vertex (DXF 42).
  */
  void setBulge(double newVal);

  /** Description:
      Returns true if the curve fit tangent direction is used by this vertex, false
      otherwise (DXF 70, bit 0x02).
  */
  bool isTangentUsed() const;

  /** Description:
      Sets this vertex to use the curve fit tangent direction (DXF 70, sets bit 0x02).
  */
  void useTangent();

  /** Description:
      Sets this vertex to not use the curve fit tangent direction (DXF 70, clears bit 0x02).
  */
  void ignoreTangent();

  /** Description:
      Returns the curve fit tangent direction for this vertex, in radians (DXF 50).
  */
  double tangent() const;

  /** Description:
      Sets the curve fit tangent direction for this vertex, in radians (DXF 50).
  */
  void setTangent(double newVal);

  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual OdResult dxfInFields_R12(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields_R12(OdDbDxfFiler* pFiler) const;

  virtual OdResult getGeomExtents(OdGeExtents3d& extents) const;

  /*
      OdDb2dVertex(const OdGePoint3d&  position, double bulge = 0,
                   double startWidth = 0, double endWidth = 0, double tangent = 0);
  */

  /** Description:
      Transforms this entity by the specified matrix.
  */
  virtual OdResult transformBy(const OdGeMatrix3d& xform);
};

typedef OdSmartPtr<OdDb2dVertex> OdDb2dVertexPtr;

#include "DD_PackPop.h"

#endif


