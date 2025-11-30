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



#ifndef _OD_DB_VERTEX_
#define _OD_DB_VERTEX_

#include "DD_PackPush.h"

#include "DbEntity.h"

namespace OdDb
{
  enum Vertex3dType
  {
    k3dSimpleVertex  = 0,
    k3dControlVertex = 1,
    k3dFitVertex     = 2
  };
}

/** Description:
    Base class for all vertex classes within an OdDbDatabase.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbVertex: public OdDbEntity
{
public:
  ODDB_DECLARE_MEMBERS(OdDbVertex);

  OdDbVertex();

  OdResult dxfInFields(OdDbDxfFiler* pFiler);

  void dxfOutFields(OdDbDxfFiler* pFiler) const;

  OdResult getGeomExtents(OdGeExtents3d& extents) const;
};

typedef OdSmartPtr<OdDbVertex> OdDbVertexPtr;

#include "DD_PackPop.h"

#endif

