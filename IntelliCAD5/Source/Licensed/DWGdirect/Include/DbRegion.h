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



#ifndef GEOMENT_DBREGION_H
#define GEOMENT_DBREGION_H

#include "DD_PackPush.h"
#include "ModelerDefs.h"
#include "DbEntity.h"

class OdBrBrep;
class OdModelerGeometry;

/** Description:
    Represents a region entity in an OdDbDatabase.  A region is represented by an ACIS model.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbRegion : public OdDbEntity
{
public:
  ODDB_DECLARE_MEMBERS(OdDbRegion);

  /** Description:
      Constructor (no arguments).
  */
  OdDbRegion();

  /** Description:
      Copies the ACIS data from this entity, into the passed in
      OdStreamBuf object.

      Arguments:
      pOut - Receives the ACIS data.
      typeVer - Bitfield specifying the type (kAfTypeASCII or kAfTypeBinary)
         of ACIS data to return, and the version (kAfVer106, etc.).  
  */
  OdResult acisOut(OdStreamBuf* pOut, AfTypeVer typeVer = kAfTypeVerAny);

  /** Description:
      Copies the ACIS data from the passed in OdStreamBuf Object, into 
      this entity.

      Arguments:
      pIn - Stream from which the ACIS data is read.  The stream
         is read from the current position, until no data remains (eof).
      typeVer - Bitfield specifying the type (kAfTypeASCII or kAfTypeBinary)
         of ACIS data to return, and the version (kAfVer106, etc.). If pIn == NULL it return
         version of stored in region ACIS data.
  */
  OdResult acisIn(OdStreamBuf* pIn, AfTypeVer *typeVer = NULL);

  /** Description:
  */
  void brep(OdBrBrep& brep);

  /** Description:
      Returns true if there is no data associated with this entity, false otherwise.
  */
  bool isNull() const;

  
  static OdResult createFromCurves( const OdRxObjectPtrArray& curveSegments,
                                OdRxObjectPtrArray& regions );

  /*
  virtual void getNormal(OdGeVector3d&) const;


  virtual void getPerimeter(double&) const;
  virtual void getArea(double& regionArea) const;
  virtual void getAreaProp( const OdGePoint3d& origin, const OdGeVector3d& xAxis,
                            const OdGeVector3d& yAxis, double& perimeter,
                            double& area, OdGePoint2d& centroid,
                            double momInertia[2], double& prodInertia,
                            double prinMoments[2], OdGeVector2d prinAxes[2],
                            double radiiGyration[2], OdGePoint2d& extentsLow,
                            OdGePoint2d& extentsHigh )const;


  virtual OdResult getPlane(OdGePlane& regionPlane) const;

  virtual void intersectWith( const OdDbEntity* ent, OdDb::Intersect intType,
                              OdGePoint3dArray& points, int thisGsMarker,
                              int otherGsMarker ) const;
  virtual void intersectWith( const OdDbEntity* ent, OdDb::Intersect intType,
                              const OdGePlane& projPlane, OdGePoint3dArray& points,
                              int thisGsMarker, int otherGsMarker ) const;

  virtual bool isNull() const;
  virtual OdDbSubentId internalSubentId(void* ent) const;
  virtual void* internalSubentPtr(const OdDbSubentId& id) const;

  virtual void getSubentPathsAtGsMarker( OdDb::SubentType type,
                      int gsMark, const OdGePoint3d& pickPoint,
                      const OdGeMatrix3d& viewXform, int& numPaths,
                      OdDbFullSubentPath*& subentPaths, int numInserts = 0,
                      OdDbObjectId* entAndInsertStack = NULL) const;

  virtual void getGsMarkersAtSubentPath( const OdDbFullSubentPath& subPath,
                                         OdDbIntArray& gsMarkers) const;

  virtual OdDbEntity* subentPtr(const OdDbFullSubentPath& id) const;

  virtual void booleanOper(OdDb::BoolOperType operation, OdDbRegion* otherRegion);

  virtual OdUInt32 numChanges() const;
  */

  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;

  virtual bool worldDraw(OdGiWorldDraw* pWd) const;

  virtual void getClassID(void** pClsid) const;

  virtual OdDbObjectPtr decomposeForSave(OdDb::DwgVersion ver, OdDbObjectId& replaceId, bool& exchangeXData);

  // directly set contained acis geometry
  //
  void setModelerGeometry( OdModelerGeometry* g );

  virtual OdResult explode(OdRxObjectPtrArray& entitySet) const;
  
  virtual OdResult transformBy( const OdGeMatrix3d& xform );
};

typedef OdSmartPtr<OdDbRegion> OdDbRegionPtr;

#include "DD_PackPop.h"

#endif


