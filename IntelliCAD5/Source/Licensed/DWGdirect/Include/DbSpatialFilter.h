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



#ifndef OD_DBSPFILT_H
#define OD_DBSPFILT_H

#include "DD_PackPush.h"

// Either back or front clippint plane distance if not enabled.
#define ODDB_INFINITE_XCLIP_DEPTH (1.0e+300)

#include "DbFilter.h"
#include "Ge/GePoint3d.h"
#include "Ge/GePoint2dArray.h"

class OdGeExtents3d;
class OdDbBlockReference;

class OdDbFilteredBlockIterator;
typedef OdSmartPtr<OdDbFilteredBlockIterator> OdDbFilteredBlockIteratorPtr;

struct OdGiClipBoundary;

/** Description:
    Represents a spatial filter object in an OdDbDatabase, which is 
    a volume created by extruding a 2D boundary.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbSpatialFilter : public OdDbFilter
{
public:
  ODDB_DECLARE_MEMBERS(OdDbSpatialFilter);

  /** Description:
      Constructor (no arguments).
  */
  OdDbSpatialFilter();
  
  OdRxClass* indexClass() const; 
  
  /** Description:
      TBC.
  */
  //void queryBounds(OdGeExtents3d& ext) const;

  /** Description:
      TBC.
  */
  void getVolume( OdGePoint3d& fromPt, OdGePoint3d& toPt, OdGeVector3d& upDir,
                  OdGeVector2d& viewField ) const;

  /** Description:
      TBC.
  */
  void setDefinition( const OdGePoint2dArray& pts, const OdGeVector3d& normal = OdGeVector3d::kZAxis,
                      double elevation = 0.0,
                      double frontClip = ODDB_INFINITE_XCLIP_DEPTH,
                      double backClip = ODDB_INFINITE_XCLIP_DEPTH,
                      bool enabled = true); 

  /** Description:
      TBC.
  */
  void setDefinition(const OdGePoint2dArray& pts,
                     const OdGeMatrix3d& xToClipSpace, 
                     double frontClip = ODDB_INFINITE_XCLIP_DEPTH,
                     double backClip = ODDB_INFINITE_XCLIP_DEPTH,
                     bool enabled = true);

  /** Description:
      TBC.
  */
  void getDefinition( OdGePoint2dArray& pts, OdGeVector3d& normal,
                      double& elevation, double& frontClip, double& backClip,
                      bool& enabled ) const; 

  /** Description:
      TBC.
  */
  void setDefinition(const OdGiClipBoundary&);

  /** Description:
      TBC.
  */
  void getDefinition(OdGiClipBoundary&) const;

  void boundary(OdGePoint2dArray& pts) const;
  OdGeVector3d normal() const;
  OdGePoint3d origin() const;
  bool frontClipEnabled() const;
  double frontClipDist() const;
  bool backClipDistEnabled() const;
  double backClipDist() const;
  bool isEnabled() const; 

  /** Description:
      TBC.
  */
  void setPerspectiveCamera(const OdGePoint3d& fromPt);
  
  /** Description:
      TBC.
  */
  bool clipVolumeIntersectsExtents(const OdGeExtents3d& ext) const;
  
  /** Description:
      TBC.
  */
  bool hasPerspectiveCamera() const;
  
  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;
};

typedef OdSmartPtr<OdDbSpatialFilter> OdDbSpatialFilterPtr;

#include "DD_PackPop.h"

#endif // OD_DBSPFILT_H


