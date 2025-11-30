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



#ifndef GEOMENT_DBSOL3D_H
#define GEOMENT_DBSOL3D_H

#include "DD_PackPush.h"
#include "ModelerDefs.h"

class OdDbRegion;
class OdDbSubentId;
class OdDbCurve;
class OdBrBrep;
class OdModelerGeometry;

#include "DbEntity.h"
#include "OdArray.h"

//#define ACIS_LIBRARY

/** Description:
    Represents a 3D ACIS solid within an OdDbDatabase.

    {group:OdDb_Classes} 
*/
class TOOLKIT_EXPORT OdDb3dSolid : public OdDbEntity
{
public:

  ODDB_DECLARE_MEMBERS(OdDb3dSolid);
  
  /** Description:
      Constructor (no arguments).
  */
  OdDb3dSolid();
  
  /** Description:
      Returns true if there is no ACIS model associated with this entity, 
      false otherwise.
  */
  bool isNull() const;

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

  virtual bool worldDraw(OdGiWorldDraw* pWd) const;

  virtual void viewportDraw(OdGiViewportDraw* pVd) const;

  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;

  virtual void getClassID(void** pClsid) const;

  virtual void createBox(double xLen, double yLen, double zLen);
  virtual void createFrustum(double height, double xRadius,
    double yRadius, double topXRadius);
  virtual void createSphere(double radius);
  virtual void createTorus(double majorRadius, double minorRadius);
  virtual void createWedge(double xLen, double yLen, double zLen);

  /** Description:
      Creates a solid by extruding region, a distance of height. The extrusion 
      direction is along the normal of the region if the height is positive.

      Note: taper is not supported.
  */
  virtual OdResult extrude(const OdDbRegion* region, double height, double taper);

  virtual OdResult revolve(const OdDbRegion* region,
    const OdGePoint3d& axisPoint, 
    const OdGeVector3d& axisDir,
    double angleOfRevolution);

  /*
  virtual void extrudeAlongPath(const OdDbRegion* region, const OdDbCurve* path);
  
  virtual void getArea(double& area) const;
  virtual void checkInterference(const OdDb3dSolid* otherSolid,
    bool createNewSolid, 
    bool& solidsInterfere,
    OdDb3dSolid** commonVolumeSolid)
    const;
  
  virtual void getMassProp(double& volume,
    OdGePoint3d& centroid,
    double momInertia[3],
    double prodInertia[3],
    double prinMoments[3],
    OdGeVector3d prinAxes[3], 
    double radiiGyration[3],
    OdDbExtents& extents) const;
  
  virtual void getSection(const OdGePlane& plane, OdDbRegion** sectionRegion) const;
  
  virtual void stlOut(const char* fileName, bool asciiFormat) const;
  
  virtual OdDbSubentId internalSubentId(void* ent) const;
  virtual void* internalSubentPtr(const OdDbSubentId& id) const;
  
  virtual void getSubentPathsAtGsMarker(OdDb::SubentType type,
    int gsMark, 
    const OdGePoint3d& pickPoint,
    const OdGeMatrix3d& viewXform, 
    int& numPaths,
    OdDbFullSubentPath** subentPaths, 
    int numInserts = 0,
    OdDbObjectId* entAndInsertStack = NULL) const;
  
  virtual void getGsMarkersAtSubentPath(const OdDbFullSubentPath& subPath, 
                                        OdDbIntArray& gsMarkers) const;
  virtual OdDbEntity* subentPtr(const OdDbFullSubentPath& id) const;
  
  virtual void booleanOper(OdDb::BoolOperType operation, OdDb3dSolid* solid);
  
  virtual void getSlice(const OdGePlane& plane, bool getNegHalfToo, 
                        OdDb3dSolid** negHalfSolid);
  
  virtual void copyEdge(const OdDbSubentId &subentId, OdDbEntity **newEntity);
  virtual void copyFace(const OdDbSubentId &subentId, OdDbEntity **newEntity);
  virtual void extrudeFaces(const OdArray<OdDbSubentId *> &faceSubentIds, 
                            double height, double taper);
  virtual void extrudeFacesAlongPath(const OdArray<OdDbSubentId *> &faceSubentIds, 
                                     const OdDbCurve* path);
  virtual void imprintEntity(const OdDbEntity *pEntity);
  virtual void cleanBody();
  virtual void offsetBody(double offsetDistance);
  virtual void offsetFaces(const OdArray<OdDbSubentId *> &faceSubentIds, 
                           double offsetDistance);
  virtual void removeFaces(const OdArray<OdDbSubentId *> &faceSubentIds);
  virtual void separateBody(OdArray<OdDb3dSolid *> &newSolids);
  virtual void shellBody(const OdArray<OdDbSubentId *> &faceSubentIds, 
                         double offsetDistance);
  virtual void taperFaces(const OdArray<OdDbSubentId *> &faceSubentIds, 
                          const OdGePoint3d &basePoint, 
                          const OdGeVector3d &draftVector,
                          double draftAngle);
  virtual void transformFaces(const OdArray<OdDbSubentId *> &faceSubentIds,
                              const OdGeMatrix3d &matrix);

  virtual void setSubentColor(const OdDbSubentId &subentId,
                              const OdCmColor &color);

  virtual OdUInt32 numChanges() const;
  */
  
  virtual OdDbObjectPtr decomposeForSave(OdDb::DwgVersion ver, OdDbObjectId& replaceId, bool& exchangeXData);  

  virtual OdResult explode(OdRxObjectPtrArray& entitySet) const;

  virtual OdResult transformBy( const OdGeMatrix3d& xform );
  
  // directly set contained acis geometry
  //
  void setModelerGeometry( OdModelerGeometry* g );
};

typedef OdSmartPtr<OdDb3dSolid> OdDb3dSolidPtr;

#include "DD_PackPop.h"

#endif


