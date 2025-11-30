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



#ifndef _OD_DB_VIEWPORT_
#define _OD_DB_VIEWPORT_

#include "DD_PackPush.h"

#include "DbEntity.h"
#include "DbSymbolTable.h"
#include "DbAbstractViewportData.h"
#include "ViewportDefs.h"

class OdDbExtents;
class OdGeMatrix3d;

/** Description:
    Represents a viewport entity within an OdDbDatabase.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbViewport : public OdDbEntity
{
protected:
  /* void dxfOutXData(OdDbDxfFiler* pFiler) const;
  */
public:
  ODDB_DECLARE_MEMBERS(OdDbViewport);

  /** Description:
      Constructor (no arguments).
  */
  OdDbViewport();

  /** Description:
      Returns the height of this viewport (DXF 41).
  */
  double height() const;

  /** Description:
      Sets the height of this viewport (DXF 41).
  */
  void setHeight(double);

  /** Description:
      Returns the width of this viewport (DXF 40).
  */
  double width() const;

  /** Description:
      Sets the width of this viewport (DXF 40).
  */
  void setWidth(double);

  /** Description:
      Returns the center point of this viewport in WCS (DXF 10).
  */
  OdGePoint3d centerPoint() const;

  /** Description:
      Sets the center point of this viewport in WCS (DXF 10).
  */
  void setCenterPoint(const OdGePoint3d&);

  /** Description:
      Returns the ID number of this viewport (-1 if this viewport is inactive).
  */
  OdInt16 number() const;

  /** Description:
      Returns true if this viewport is on, false otherwise (DXF 90, bit 0x20000).
  */
  bool isOn() const;

  /** Description:
      Turns this viewport on (DXF 90, bit 0x20000 cleared).
  */
  void setOn();

  /** Description:
      Turns this viewport off (DXF 90, bit 0x20000 set).
  */
  void setOff();

  /** Description:
      Returns the view target point for this viewport in WCS (DXF 17).
  */
  OdGePoint3d viewTarget() const;

  /** Description:
      Sets the view target point for this viewport in WCS (DXF 17).
  */
  void setViewTarget(const OdGePoint3d&);

  /** Description:
      Returns the view direction vector for this viewport in WCS (DXF 16).
  */
  OdGeVector3d viewDirection() const;

  /** Description:
      Sets the view direction vector for this viewport in WCS (DXF 16).
  */
  void setViewDirection(const OdGeVector3d&);

  /** Description:
      Returns the view height of this viewport (DXF 45).
  */
  double viewHeight() const;

  /** Description:
      Sets the view height of this viewport (DXF 45).
  */
  void setViewHeight(double ht);

  /** Description:
      Returns the view center of this viewport (DXF 12).
  */
  OdGePoint2d viewCenter() const;

  /** Description:
      Sets the view center of this viewport (DXF 12).
  */
  void setViewCenter(const OdGePoint2d& pt);

  /** Description:
      Returns the twist angle of this viewport (DXF 51).
  */
  double twistAngle() const;

  /** Description:
      Sets the twist angle of this viewport (DXF 51).
  */
  void setTwistAngle(double);

  /** Description:
      Returns the perspective lens length of this viewport (DXF 42).
  */
  double lensLength() const;

  /** Description:
      Sets the perspective lens length of this viewport (DXF 42).
  */
  void setLensLength(double);

  /** Description:
      Returns the "front clip on" flag of this viewport (DXF 90, bit 0x02).
  */
  bool isFrontClipOn() const;

  /** Description:
      Sets the "front clip on" flag of this viewport (DXF 90, bit 0x02).
  */
  void setFrontClipOn();

  /** Description:
      Clears the "front clip on" flag of this viewport (DXF 90, bit 0x02).
  */
  void setFrontClipOff();

  /** Description:
      Returns the "back clip" on flag of this viewport (DXF 90, bit 0x04).
  */
  bool isBackClipOn() const;

  /** Description:
      Sets the "back clip on" flag of this viewport (DXF 90, bit 0x04).
  */
  void setBackClipOn();

  /** Description:
      Clears the "back clip on" flag of this viewport (DXF 90, bit 0x04).
  */
  void setBackClipOff();

  /** Description:
      Returns true if "front clip at eye" is on, false otherwise (DXF 90, bit 0x10).
  */
  bool isFrontClipAtEyeOn() const;

  /** Description:
      Sets "front clip at eye" to true (DXF 90, bit 0x10).
  */
  void setFrontClipAtEyeOn();

  /** Description:
      Sets "front clip at eye" to false (DXF 90, bit 0x10).
  */
  void setFrontClipAtEyeOff();

  /** Description:
      Returns the front clip distance for this viewport (DXF 43).
  */
  double frontClipDistance() const;

  /** Description:
      Sets the front clip distance for this viewport (DXF 43).
  */
  void setFrontClipDistance(double newVal);

  /** Description:
      Returns the back clip distance for this viewport (DXF 44).
  */
  double backClipDistance() const;

  /** Description:
      Sets the front back distance for this viewport (DXF 44).
  */
  void setBackClipDistance(double newVal);

  /** Description:
      Returns true if perspective is on, false otherwise (DXF 90, bit 0x01).
  */
  bool isPerspectiveOn() const;

  /** Description:
      Sets perspective to on (DXF 90, bit 0x01).
  */
  void setPerspectiveOn();

  /** Description:
      Sets perspective to off (DXF 90, bit 0x01).
  */
  void setPerspectiveOff();

  /** Description:
      Returns the UCS follow mode (DXF 90, bit 0x08).
  */
  bool isUcsFollowModeOn() const;

  /** Description:
      Sets UCS follow mode to true (DXF 90, bit 0x08).
  */
  void setUcsFollowModeOn();

  /** Description:
      Sets UCS follow mode to false (DXF 90, bit 0x08).
  */
	void setUcsFollowModeOff();

  /** Description:
      Returns the UCS icon visible flag for this viewport (DXF 90, bit 0x20).
  */
  bool isUcsIconVisible() const;

  /** Description:
      Sets the UCS icon visible flag to true this viewport (DXF 90, bit 0x20).
  */
  void setUcsIconVisible();

  /** Description:
      Sets the UCS icon visible flag to false this viewport (DXF 90, bit 0x20).
  */
  void setUcsIconInvisible();

  /** Description:
      Returns true if the UCS icon is located at the orgin for this viewport,
      false otherwise (DXF 90, bit 0x40).
  */
  bool isUcsIconAtOrigin() const;

  /** Description:
      Sets the "UCS icon located at orgin" flag to true for this viewport (DXF 90, bit 0x40).
  */
  void setUcsIconAtOrigin();

  /** Description:
      Sets the "UCS icon located at orgin" flag to false for this viewport (DXF 90, bit 0x40).
  */
  void setUcsIconAtCorner();

  /** Description:
      Returns the "fast zoom on" flag for this viewport (DXF 90, bit 0x80).
  */
  bool isFastZoomOn() const;

  /** Description:
      Sets the "fast zoom on" flag for this viewport to true (DXF 90, bit 0x80).
  */
  void setFastZoomOn();

  /** Description:
      Sets the "fast zoom on" flag for this viewport to false (DXF 90, bit 0x80).
  */
  void setFastZoomOff();

  /** Description:
      Returns the circle zoom percent for this viewport (DXF 72).
  */
  OdUInt16 circleSides() const;

  /** Description:
      Sets the circle zoom percent for this viewport (DXF 72).
  */
  void setCircleSides(OdUInt16);

  /** Description:
      Returns the snap mode for this viewport (DXF 90, bit 0x100).
  */
  bool isSnapOn() const;

  /** Description:
      Turns snap mode on for this viewport (DXF 90, bit 0x100).
  */
  void setSnapOn();

  /** Description:
      Turns snap mode off for this viewport (DXF 90, bit 0x100).
  */
  void setSnapOff();

  /** Description:
      Returns the isometric snap style flag for this viewport (DXF 90, bit 0x400).
  */
  bool isSnapIsometric() const;

  /** Description:
      Sets the isometric snap style flag to true for this viewport (DXF 90, bit 0x400).
  */
  void setSnapIsometric();

  /** Description:
      Sets the isometric snap style flag to false for this viewport (DXF 90, bit 0x400).
  */
  void setSnapStandard();

  /** Description:
      Returns the snap angle for this viewport (DXF 50).
  */
  double snapAngle() const;

  /** Description:
      Sets the snap angle for this viewport (DXF 50).
  */
  void setSnapAngle(double);

  /** Description:
      Returns the snap base point for this viewport (DXF 13).
  */
  OdGePoint2d snapBasePoint() const;

  /** Description:
      Sets the snap base point for this viewport (DXF 13).
  */
  void setSnapBasePoint(const OdGePoint2d&); 
  
  /** Description:
      Returns the snap increment for this viewport (DXF 14).
  */
  OdGeVector2d snapIncrement() const;

  /** Description:
      Sets the snap increment for this viewport (DXF 14).
  */
  void setSnapIncrement(const OdGeVector2d&);
  
  /** Description:
      TBC.
  */
  OdUInt16 snapIsoPair() const;

  /** Description:
      TBC.
  */
  void setSnapIsoPair(OdUInt16);
  
  /** Description:
      Returns the "grid on" flag for this viewport (DXF 90, bit 0x200).
  */
  bool isGridOn() const;

  /** Description:
      Sets the "grid on" flag to true for this viewport (DXF 90, bit 0x200).
  */
  void setGridOn();

  /** Description:
      Sets the "grid on" flag to false for this viewport (DXF 90, bit 0x200).
  */
  void setGridOff();
  
  /** Description:
      Returns the grid increment for this viewport (DXF 15).
  */
  OdGeVector2d gridIncrement() const;

  /** Description:
      Sets the grid increment for this viewport (DXF 15).
  */
  void setGridIncrement(const OdGeVector2d&);
  
  /** Description:
      Returns the "hidden lines removed" or hide plot flag for this viewport (DXF 90, bit 0x800).
  */
  bool hiddenLinesRemoved() const;

  /** Description:
      Sets the "hidden lines removed" or hide plot flag to false for this viewport (DXF 90, bit 0x800).
  */
  void showHiddenLines();

  /** Description:
      Sets the "hidden lines removed" or hide plot flag to true for this viewport (DXF 90, bit 0x800).
  */
  void removeHiddenLines();
  
  /** Description:
      Freezes each layer from the passed in Object ID array, in this viewport (DXF 341).
  */
  void freezeLayersInViewport(const OdDbObjectIdArray&);

  /** Description:
      Thaws each layer from the passed in Object ID array, in this viewport.
  */
  void thawLayersInViewport(const OdDbObjectIdArray&);

  /** Description:
      Thaws all layers that are frozen in this viewport.
  */
  void thawAllLayersInViewport();

  /** Description:
      Returns true if the specified layer is frozen in this viewport, false otherwise.
  */
  bool isLayerFrozenInViewport(const OdDbObjectId& layerId) const;

  /** Description:
      Returns all layers that are frozen in this viewport.
  */
  void getFrozenLayerList(OdDbObjectIdArray& ids) const;
  
  /** Description:
      TBC.
  */
  void updateDisplay() const;
  
  virtual bool worldDraw(OdGiWorldDraw* pWd) const;
  
  /** Description:
      Returns true if this layer is locked, false otherwise (DXF 90, bit 0x4000).
  */
  bool isLocked() const;

  /** Description:
      Sets the locked status of this viewport to true (DXF 90, bit 0x4000).
  */
  void setLocked();

  /** Description:
      Sets the locked status of this viewport to false (DXF 90, bit 0x4000).
  */
  void setUnlocked();
  
  /** Description:
      TBC.
  */
  bool isTransparent() const;

  /** Description:
      TBC.
  */
  void setTransparent();

  /** Description:
      TBC.
  */
  void setOpaque();
  
  enum StandardScaleType
  {
    kScaleToFit, // Scaled to Fit
    kCustomScale, // Scale is not a standard scale
    k1_1, // 1:1
    k1_2, // 1:2
    k1_4, // 1:4
    k1_8, // 1:8
    k1_10, // 1:10
    k1_16, // 1:16
    k1_20, // 1:20
    k1_30, // 1:30
    k1_40, // 1:40
    k1_50, // 1:50
    k1_100, // 1:100
    k2_1, // 2:1
    k4_1, // 4:1
    k8_1, // 8:1
    k10_1, // 10:1
    k100_1, // 100:1
    k1_128in_1ft, // 1/128"= 1'
    k1_64in_1ft, // 1/64"= 1'
    k1_32in_1ft, // 1/32"= 1'
    k1_16in_1ft, // 1/16"= 1'
    k3_32in_1ft, // 3/32"= 1'
    k1_8in_1ft, // 1/8" = 1'
    k3_16in_1ft, // 3/16"= 1'
    k1_4in_1ft, // 1/4" = 1'
    k3_8in_1ft, // 3/8" = 1'
    k1_2in_1ft, // 1/2" = 1'
    k3_4in_1ft, // 3/4" = 1'
    k1in_1ft, // 1" = 1'
    k3in_1ft, // 3" = 1'
    k6in_1ft, // 6" = 1'
    k1ft_1ft // 1' = 1'
  };
  
  // ShadePlot enum and get/set methods
  enum ShadePlotType
  {
      kAsDisplayed,
      kWireframe,
      kHidden,
      kRendered
  };

  /** Description:
      TBC.
  */
  double customScale() const;

  /** Description:
      TBC.
  */
  void setCustomScale(double);
  
  /** Description:
      TBC.
  */
  StandardScaleType standardScale() const;

  /** Description:
      TBC.
  */
  void setStandardScale(const StandardScaleType);
  
  /** Description:
      Returns the plot style name associated with this viewport (DXF 1).
  */
  OdString plotStyleSheet() const;

  /** Description:
      TBC.
  */
  void effectivePlotStyleSheet(char* const& );

  /** Description:
      Sets the plot style name associated with this viewport (DXF 1).
  */
  void setPlotStyleSheet(const char *);
  
  /** Description:
      returns the non-rectangular clipping flag for this viewport (DXF 90, bit 0x10000).
  */
  bool isNonRectClipOn() const;

  /** Description:
      Turns on non-rectangular clipping for this viewport (DXF 90, bit 0x10000 set).
  */
  void setNonRectClipOn();

  /** Description:
      Turns off non-rectangular clipping for this viewport (DXF 90, bit 0x10000 cleared).
  */
  void setNonRectClipOff();
  
  /** Description:
      Returns the Object ID of the entity that serves as this viewport's clipping boundary (DXF 340).
  */
  OdDbObjectId nonRectClipEntityId() const;

  /** Description:
      Sets the Object ID of the entity that serves as this viewport's clipping boundary (DXF 340).
  */
  void setNonRectClipEntityId(OdDbObjectId);
  
  /*
     virtual void erased(const OdDbObject* , bool);
     virtual void modified(const OdDbObject *);
     virtual void copied(const OdDbObject* pDbObj,const OdDbObject* pNewObj);
     virtual void subObjModified(const OdDbObject* pDbObj, const OdDbObject* pSubObj);
  */
  
  /** Description:
      Returns the UCS associated with this viewport.

      Arguments:
      origin (O) Receives the UCS origin (DXF 110).
      xAxis (O) Receives the UCS X axis (DXF 111).
      yAxis (O) Receives the UCS Y axis (DXF 112).
  */
  void getUcs(OdGePoint3d& origin, OdGeVector3d& xAxis, OdGeVector3d& yAxis) const;

  /** Description:
      Returns the orthographic view type of the UCS associated with this viewport.

      Arguments:
      view (O) Receives the orthographic view type.

      Return Value:
      True if the UCS is orthographic, false otherwise.
  */
  bool isUcsOrthographic(OdDb::OrthographicView& view) const;

  /** Description:
      Returns the Object ID of the UCS associated with this viewport (DXF 345 or 346).
  */
  OdDbObjectId ucsName() const;

  /** Description:
      Returns the elevation of this viewport (DXF 146).
  */
  double elevation() const;
  
  /** Description:
      Sets the UCS information for this viewport.

      Arguments:
      origin (I) The UCS origin (DXF 110).
      xAxis (I) The UCS X axis (DXF 111).
      yAxis (I) The UCS Y axis (DXF 112).
  */
  void setUcs(const OdGePoint3d& origin, const OdGeVector3d& xAxis, const OdGeVector3d& yAxis);

  /** Description:
      Sets the orthographic view type for the UCS associated with this viewport (DXF 79).
  */
  void setUcs(OdDb::OrthographicView view);

  /** Description:
      Sets the UCS table object associated with this viewport (DXF 345 o r 346).
  */
  void setUcs(const OdDbObjectId& ucsId);

  /** Description:
      Sets this viewport to use the world UCS.
  */
  void setUcsToWorld();

  /** Description:
      Sets the elevation for this viewport (DXF 146).
  */
  void setElevation(double elev );
  
  /** Description:
      TBC.
  */
  bool isViewOrthographic(OdDb::OrthographicView& view ) const;

  /** Description:
      TBC.
  */
  void setViewDirection(OdDb::OrthographicView view );
  
  /** Description:
      Returns the UCS per viewport flag (DXF 71).
  */
  bool isUcsSavedWithViewport() const;

  /** Description:
      Sets the UCS per viewport flag (DXF 71).
  */
  void setUcsPerViewport( bool ucsvp );
  
  /** Description:
      Set the render mode for this viewport (DXF 281).
  */
  void setRenderMode(OdDb::RenderMode mode);

  /** Description:
      Returns the render mode for this viewport (DXF 281).
  */
  OdDb::RenderMode renderMode() const;

  /** Description:
      This method returns the shade plot mode of the current 
      viewport. The shade plot mode specifies how the 
      current viewport will plot.
  */
  ShadePlotType shadePlot() const;

  /** Description:
      This method sets the shade plot mode of 
      the current viewport.
  */
  void setShadePlot(const ShadePlotType type);

  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;

  virtual OdResult dxfInFields_R12(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields_R12(OdDbDxfFiler* pFiler) const;

  virtual void getClassID(void** pClsid) const;

  void subClose();

  OdResult subErase(bool bErasing);

  /** Description:
      Adjusts the necessary parameters in this viewport such that the view is zoomed 
      to the current extents of the drawing.
  */
  void zoomExtents();
  void calcZoomExtents(OdDbExtents& pFullExtents, OdGeMatrix3d& pViewMatr, bool& bOverall) const;
  void adjustZoomExtents(OdDbExtents& pFullExtents, OdGeMatrix3d& pViewMatr, bool bOverall);

  virtual OdResult getGeomExtents(OdGeExtents3d& extents) const;

  virtual OdResult transformBy(const OdGeMatrix3d& xForm);
};

typedef OdSmartPtr<OdDbViewport> OdDbViewportPtr;

/** Description:

    {group:OdDb_Classes}
*/
class OdDbAbstractViewportDataForDbViewport : public OdDbAbstractViewportData
{
public:
  OdDbAbstractViewportDataForDbViewport() {}

  double getHeight(OdDbObject* pVpObj) const;
  void setHeight(OdDbObject* pVpObj, double height);

  double getWidth(OdDbObject* pVpObj) const;
  void setWidth(OdDbObject* pVpObj, double height);

  void getUcs(const OdDbObject* pVpObj, 
    OdGePoint3d& origin, OdGeVector3d& xAxis, OdGeVector3d& yAxis ) const;
  void setUcs(OdDbObject* pVpObj,
    const OdGePoint3d& origin, const OdGeVector3d& xAxis, const OdGeVector3d& yAxis);

  bool isUcsIconVisible(const OdDbObject* pVpObj) const;
  void setUcsIconVisible(OdDbObject* pVpObj, bool bVisible);

  bool isUcsIconAtOrigin(const OdDbObject* pVpObj) const;
  void setUcsIconAtOrigin(OdDbObject* pVpObj, bool bAtOrigin);

  int circleSides(const OdDbObject* pVpObj) const;
  void setCircleSides(OdDbObject* pVpObj, int nSides);

  void adjustGsView(const OdDbObject* pVp, OdGsView* pView);
  void adjustViewport(const OdDbObject* /*pVp*/, OdDbViewport* /*pView*/){return;}
  void adjustViewportRec(const OdDbObject* /*pVp*/, OdDbViewportTableRecord* /*pView*/){return;}

  void adjustByGsView(const OdGsView* pView, OdDbObject* pVp);
  void adjustByViewport(const OdDbViewport* /*pVp*/, OdDbObject* /*pVRec*/) { return; }
  void adjustByViewportRec(const OdDbViewportTableRecord* /*pVp*/, OdDbObject* /*pVRec*/) { return; }
};

#include "DD_PackPop.h"

#endif

