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



#ifndef _ODDBVIEWPORTTABLERECORD_INCLUDED
#define _ODDBVIEWPORTTABLERECORD_INCLUDED

#include "DD_PackPush.h"

#include "DbAbstractViewTableRecord.h"

class OdGsView;
class OdGsDCRect;
class OdDbViewportTable;
class OdDbExtents;
class OdGeMatrix3d;

/** Description:
    Represents a viewport table record in an OdDbDatabase.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbViewportTableRecord : public OdDbAbstractViewTableRecord
{
public:
  ODDB_DECLARE_MEMBERS(OdDbViewportTableRecord);

  /** Description:
      Constructor (no arguments).
  */
  OdDbViewportTableRecord();

  typedef OdDbViewportTable TableType;

  /** Description:
      Returns the lower left corner of the viewport (DXF 10).
  */
  OdGePoint2d lowerLeftCorner() const;

  /** Description:
      Sets the lower left corner of the viewport (DXF 10).
  */
  void setLowerLeftCorner(const OdGePoint2d& pt);

  /** Description:
      Returns the upper right corner of the viewport (DXF 11).
  */
  OdGePoint2d upperRightCorner() const;

  /** Description:
      Sets the upper right corner of the viewport (DXF 11).
  */
  void setUpperRightCorner(const OdGePoint2d& pt);

  /** Description:
      Returns the UCS follow flag for this viewport (DXF 71, bit 0x08).
  */
  bool ucsFollowMode() const;

  /** Description:
      Sets the UCS follow flag for this viewport (DXF 71, bit 0x08).
  */
  void setUcsFollowMode(bool enabled);

  /** Description:
      Returns the circle zoom percent for this viewport (DXF 72).
  */
  OdUInt16 circleSides() const;

  /** Description:
      Sets the circle zoom percent for this viewport (DXF 72).
  */
  void setCircleSides(OdUInt16 circleSides);

  /** Description:
      Returns the UCS icon visible flag for this viewport (DXF 74, bit 0x01).
  */
  bool iconEnabled() const;

  /** Description:
      Sets the UCS icon visible flag for this viewport (DXF 74, bit 0x01).
  */
  void setIconEnabled(bool enabled);

  /** Description:
      Returns the UCS icon at orgin flag for this viewport (DXF 74, bit 0x02).
  */
  bool iconAtOrigin() const;

  /** Description:
      Sets the UCS icon at orgin flag for this viewport (DXF 74, bit 0x02).
  */
  void setIconAtOrigin(bool atOrigin);

  /** Description:
      Returns the grid enabled flag for this viewport (DXF 76).
  */
  bool gridEnabled() const;

  /** Description:
      Sets the grid enabled flag for this viewport (DXF 76).
  */
  void setGridEnabled(bool enabled);

  /** Description:
      Returns the grid increments for this viewport (DXF 15).
  */
  OdGePoint2d gridIncrements() const;

  /** Description:
      Sets the grid increments for this viewport (DXF 15).
  */
  void setGridIncrements(const OdGePoint2d& base);

  /** Description:
      Returns the snap enabled flag for this viewport (DXF 75).
  */
  bool snapEnabled() const;

  /** Description:
      Sets the snap enabled flag for this viewport (DXF 75).
  */
  void setSnapEnabled(bool enabled);

  bool isometricSnapEnabled() const;
  void setIsometricSnapEnabled(bool enabled);

  OdInt16 snapPair() const;
  void setSnapPair(OdInt16 pairType);

  double snapAngle() const;
  void setSnapAngle(double angle);

  OdGePoint2d snapBase() const;
  void setSnapBase(const OdGePoint2d& base);

  OdGePoint2d snapIncrements() const;
  void setSnapIncrements(const OdGePoint2d& base);

  bool isUcsSavedWithViewport() const;
  void setUcsPerViewport( bool ucsvp );

  bool fastZoomsEnabled() const;
  void setFastZoomsEnabled(bool enabled);

  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;

  virtual OdResult dxfInFields_R12(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields_R12(OdDbDxfFiler* pFiler) const;

  /** Description:
      TBC.
  */
  OdGsView* gsView() const;
  void setGsView(OdGsView* pGsView);
  
  /** Description:
      TBC.
  */
	OdGiDrawable* drawable();

  /*
     OdUInt32 setAttributes(OdGiDrawableTraits* pTraits) const;
     bool worldDraw(OdGiWorldDraw* pWd) const; 
     void setGsNode(OdGsNode* pNode);
     OdGsNode* gsNode() const;
  */

  void getClassID(void** pClsid) const;

  /** Description:
      Copies the contents of other into the messaged object, whenever feasible
  */
  virtual void copyFrom (const OdRxObject* pSrc);

  /** Description:
      Adjusts the necessary parameters in this viewport such that the view is zoomed 
      to the current extents of the drawing.
  */
  void zoomExtents();
  void calcZoomExtents(OdDbExtents& fullExtents, OdGeMatrix3d& viewMatr) const;
  void adjustZoomExtents(OdDbExtents& fullExtents, OdGeMatrix3d& viewMatr);
};

typedef OdSmartPtr<OdDbViewportTableRecord> OdDbViewportTableRecordPtr;

/** Description:

    {group:OdDb_Classes} 
*/
class OdDbAbstractViewportDataForDbVpTabRec : public OdDbAbstractViewportDataForAbstractViewTabRec 
{
public:
  OdDbAbstractViewportDataForDbVpTabRec() {}

  void getUcs(const OdDbObject* pVpObj, 
    OdGePoint3d& origin, OdGeVector3d& xAxis, OdGeVector3d& yAxis ) const;
  void setUcs(OdDbObject* pVpObj,
    const OdGePoint3d& origin, const OdGeVector3d& xAxis, const OdGeVector3d& yAxis);

  bool isUcsIconVisible(const OdDbObject* pVpObj) const;
  void setUcsIconVisible(OdDbObject* pVpObj, bool bVisible);

  bool isUcsIconAtOrigin(const OdDbObject* pVpObj) const;
  void setUcsIconAtOrigin(OdDbObject* pVpObj, bool bAtOrigin);

  void adjustGsView(const OdDbObject* pVp, OdGsView* pView);
  void adjustViewport(const OdDbObject* , OdDbViewport* ) {return;}
  void adjustViewportRec(const OdDbObject* , OdDbViewportTableRecord* ) {return;}

  void adjustByViewport(const OdDbViewport* /*pVp*/, OdDbObject* /*pVRec*/) { return; }
  void adjustByViewportRec(const OdDbViewportTableRecord* /*pVp*/, OdDbObject* /*pVRec*/) { return; }

};

#include "DD_PackPop.h"

#endif // _ODDBVIEWPORTTABLERECORD_INCLUDED


