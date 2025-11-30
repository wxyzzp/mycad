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



#ifndef _OD_DB_FACE_
#define _OD_DB_FACE_

#include "DD_PackPush.h"

#include "DbEntity.h"

/** Description:
    Represents a 3D Face entity within an OdDbDatabase.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbFace : public OdDbEntity
{
public:                
  ODDB_DECLARE_MEMBERS(OdDbFace);

  /** Description:
      Constructor (no arguments).
  */
  OdDbFace();
    
  /** Description:
      Retrieves the face vertex at the specified position (DXF .

      Arguments:
      pos (I) Index of vertex to be retrieved (0-3).
      v (O) Receives the value of the specified face vertex (DXF 10-13).
  */
  void getVertexAt(OdUInt16 pos, OdGePoint3d& v) const;

  /** Description:
      Sets the face vertex at the specified position.

      Arguments:
      pos (I) Index of vertex to be set (0-3).
      v (I) The value of the specified face vertex (DXF 10-13).
  */
  void setVertexAt(OdUInt16 pos, const OdGePoint3d& v);
  
  /** Description:
      Determines if the specified edge is visible (DXF 70).

      Arguments:
      pos (I) Index of vertex (0-3).

      Return Value:
      true if specified edge is visible, false otherwise.
  */
  bool isEdgeVisibleAt(OdUInt16 pos) const;

  /** Description:
      Sets the visibility property to on for the specified edge (DXF 70).
  */
  void makeEdgeVisibleAt(OdUInt16 pos);

  /** Description:
      Sets the visibility property to off for the specified edge (DXF 70).
  */
  void makeEdgeInvisibleAt(OdUInt16 pos);

  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;

  virtual OdResult dxfInFields_R12(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields_R12(OdDbDxfFiler* pFiler) const;

  bool worldDraw(OdGiWorldDraw* pWd) const;

  void viewportDraw(OdGiViewportDraw* pVd) const;

  virtual void getClassID(void** pClsid) const;

  virtual OdResult transformBy(const OdGeMatrix3d& xform);

  /*
      OdDbFace(const OdGePoint3d& pt0, const OdGePoint3d& pt1, const OdGePoint3d& pt2,
               const OdGePoint3d& pt3, bool e0vis = true, bool e1vis = true,
               bool e2vis = true, bool e3vis = true);
      OdDbFace(const OdGePoint3d& pt0, const OdGePoint3d& pt1, const OdGePoint3d& pt2,
               bool e0vis = true, bool e1vis = true, bool e2vis = true, bool e3vis = true);
  */

};

typedef OdSmartPtr<OdDbFace> OdDbFacePtr;

#include "DD_PackPop.h"

#endif

