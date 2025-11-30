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



#ifndef _ODDBABSTRACTVIEWLTABLERECORD_INCLUDED
#define _ODDBABSTRACTVIEWLTABLERECORD_INCLUDED

#include "DD_PackPush.h"

#include "DbSymbolTableRecord.h"
#include "DbAbstractViewportData.h"
#include "ViewportDefs.h"

/** Description:
    Base class for OdDbViewTableRecord and OdDbViewportTableRecord.

    {group:OdDb_Classes} 
*/
class TOOLKIT_EXPORT OdDbAbstractViewTableRecord : public OdDbSymbolTableRecord
{
public:

  ODDB_DECLARE_MEMBERS(OdDbAbstractViewTableRecord);

  /** Description:
      Constructor (no arguments).
  */
  OdDbAbstractViewTableRecord();

  /** Description:
      Returns the center point of the view in DCS (DXF 10 for OdDbViewTableRecord,
      DXF 12 for OdDbViewportTableRecord).
  */
  OdGePoint2d centerPoint() const;

  /** Description:
      Sets the center point of the view in DCS (DXF 10 for OdDbViewTableRecord,
      DXF 12 for OdDbViewportTableRecord).
  */
  void setCenterPoint(const OdGePoint2d&);

  /** Description:
      Returns the view height in DCS (DXF 40).
  */
  double height() const;

  /** Description:
      Sets the view height in DCS (DXF 40).
  */
  void setHeight(double height);

  /** Description:
      Returns the view width in DCS (DXF 41 for OdDbViewTableRecord), 
      or for AcDbViewportTableRecords, this DXF 41 value represents 
      the ratio of the view width to height.
  */
  double width() const;

  /** Description:
      Sets the view width in DCS (DXF 41 for OdDbViewTableRecord).
  */
  void setWidth(double width);

  /** Description:
      Returns the view target in WCS (DXF 12 for OdDbViewTableRecord, 
      DXF 17 for OdDbViewportTableRecord).
  */
  OdGePoint3d target() const;

  /** Description:
      Sets the view target in WCS (DXF 12 for OdDbViewTableRecord, 
      DXF 17 for OdDbViewportTableRecord).
  */
  void setTarget(const OdGePoint3d& target);

  /** Description:
      Returns the view direction in WCS (DXF 11 for OdDbViewTableRecord, 
      DXF 16 for OdDbViewportTableRecord).  The view direction is the direction
      vector from the target to the camera.
  */
  OdGeVector3d viewDirection() const;

  /** Description:
      Sets the view direction in WCS (DXF 11 for OdDbViewTableRecord, 
      DXF 16 for OdDbViewportTableRecord).  The view direction is the direction
      vector from the target to the camera.
  */
  void setViewDirection(const OdGeVector3d& viewDirection);

  /** Description:
      Returns the twist angle of this view in radians (DXF 50 for OdDbViewTableRecord, 
      DXF 51 for OdDbViewportTableRecord)
  */
  double viewTwist() const;

  /** Description:
      Sets the twist angle of this view in radians (DXF 50 for OdDbViewTableRecord, 
      DXF 51 for OdDbViewportTableRecord)
  */
  void setViewTwist(double angle);

  /** Description:
      Returns the lens length used for perspective mode in this view (DXF 42).
  */
  double lensLength() const;

  /** Description:
      Sets the lens length used for perspective mode in this view (DXF 42).
  */
  void setLensLength(double length);

  /** Description:
      Returns the distance from the target to the front clipping plane along the 
      target-camera line (DXF 43).
  */
  double frontClipDistance() const;

  /** Description:
      Sets the distance from the target to the front clipping plane along the 
      target-camera line (DXF 43).
  */
  void setFrontClipDistance(double distance);

  /** Description:
      Returns the distance from the target to the back clipping plane along the 
      target-camera line (DXF 44).
  */
  double backClipDistance() const;

  /** Description:
      Sets the distance from the target to the back clipping plane along the 
      target-camera line (DXF 44).
  */
  void setBackClipDistance(double distance);

  /** Description:
      Returns true if perspective mode is enabled for this view, false otherwise
      (DXF 71, bit 0x01).
  */
  bool perspectiveEnabled() const;

  /** Description:
      Sets the perspective mode for this view (DXF 71, bit 0x01).

      Arguments:
      enabled (I) True if perspective is to be enabled, false otherwise.
  */
  void setPerspectiveEnabled(bool enabled);

  /** Description:
      Returns true if the front clipping plane is enabled, false otherwise (DXF 71, bit 0x02).
  */
  bool frontClipEnabled() const;

  /** Description:
      Sets the front clipping plane status for this view (DXF 71, bit 0x02).

      Arguments:
      enabled (I) True if the front clipping plane is to be enabled, false otherwise.
  */
  void setFrontClipEnabled(bool enabled);

  /** Description:
      Returns true if the back clipping plane is enabled, false otherwise (DXF 71, bit 0x04).
  */
  bool backClipEnabled() const;

  /** Description:
      Sets the back clipping plane status for this view (DXF 71, bit 0x04).

      Arguments:
      enabled (I) True if the back clipping plane is to be enabled, false otherwise.
  */
  void setBackClipEnabled(bool enabled);

  /** Description:
      Returns true if the front clipping plane is located at the camera, 
      false otherwise (DXF 71, bit 0x10).
  */
  bool frontClipAtEye() const;

  /** Description:
      Sets the front clip at eye status for this view (DXF 71, bit 0x10).

      Arguments:
      atEye (I) True if the front clipping plane is located at the camera, 
      false otherwise.
  */
  void setFrontClipAtEye(bool atEye);

  /** Description:
      Returns the render mode for this view (DXF 281).
  */
  void setRenderMode(OdDb::RenderMode mode);

  /** Description:
      Sets the render mode for this view (DXF 281).
  */
  OdDb::RenderMode renderMode() const;

  /** Description:
      Retrieves the UCS parameters for the UCS associated with this view.
      If there is no associated UCS, throws an appropriate exception

      Arguments:
      origin (O) UCS origin (DXF 110).
      xAxis (O) UCS X axis (DXF 111).
      yAxis (O) UCS Y axis (DXF 112).
  */
  void getUcs(OdGePoint3d& origin, OdGeVector3d& xAxis, OdGeVector3d& yAxis) const;

  /** Description:
      Returns true if there is an orthographic UCS associated with this view, false otherwise.

      Arguments:
      view (O) Orthographic type of UCS (DXF 79).
  */
  bool isUcsOrthographic(OdDb::OrthographicView& view) const;

  /** Description:
      Returns the Object ID of the OdDbUCSTableRecord associated with this view, or 
      a null ID if there is no associated UCS (DXF 345).
  */
  OdDbObjectId ucsName() const;

  /** Description:
      Returns the elevation of the UCS plane for this view (DXF 146).  The return value is only
      valid if there is a UCS associated with this view.
  */
  double elevation() const;

  /** Description:
      Sets the UCS parameters for the UCS associated with this view.

      Arguments:
      origin (I) UCS origin (DXF 110).
      xAxis (I) UCS X axis (DXF 111).
      yAxis (I) UCS Y axis (DXF 112).
  */
  void setUcs(const OdGePoint3d& origin, const OdGeVector3d& xAxis, const OdGeVector3d& yAxis);

  /** Description:
      Sets the unnamed UCS for this view to the specified value.
  */
  void setUcs(OdDb::OrthographicView view);

  /** Description:
      Sets the OdDbUCSTableRecord Object ID for the UCS associated with this view (DXF 345).
  */
  void setUcs(const OdDbObjectId& ucsId);

  /** Description:
      Sets the UCS for this view to the world UCS.
  */
  void setUcsToWorld();

  /** Description:
      Sets the elevation of the UCS plane for this view (DXF 146).  
  */
  void setElevation(double elev);
  
  /** Description:
      Reads in the DWG data for this object.

      Arguments:
      pFiler Filer object from which data is read.
  */
  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  /** Description:
      Writes out the DWG data for this object.

      Arguments:
      pFiler Filer object to which data is written.
  */
  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  /** Description:
      Copies the data from the specified object into this object.

      Remarks:
      Specified object must be of type OdDbAbstractViewTableRecord 
      (or a type derived from OdDbAbstractViewTableRecord).
  */
  virtual void copyFrom (const OdRxObject* pSrc);



  /*
      bool isViewOrthographic(OdDb::OrthographicView& view) const;
      void setViewDirection(OdDb::OrthographicView view);
  */

};

typedef OdSmartPtr<OdDbAbstractViewTableRecord> OdDbAbstractViewTableRecordPtr;

/** Description:

    {group:OdDb_Classes}
*/
class OdDbAbstractViewportDataForAbstractViewTabRec : public OdDbAbstractViewportData
{
public:
  OdDbAbstractViewportDataForAbstractViewTabRec() {}

  double getHeight(OdDbObject* pVpObj) const;
  void setHeight(OdDbObject* pVpObj, double height);

  double getWidth(OdDbObject* pVpObj) const;
  void setWidth(OdDbObject* pVpObj, double height);

  OdGeVector3d getViewDirection(OdDbObject* pVpObj) const;
  void setViewDirection(OdDbObject* pVpObj, OdGeVector3d& dir);

  double getTwistAngle(OdDbObject* pVpObj) const;
  void setTwistAngle(OdDbObject* pVpObj, double angl);

  void getUcs(const OdDbObject* pVpObj, 
    OdGePoint3d& origin, OdGeVector3d& xAxis, OdGeVector3d& yAxis ) const;
  void setUcs(OdDbObject* pVpObj,
    const OdGePoint3d& origin, const OdGeVector3d& xAxis, const OdGeVector3d& yAxis);

  int circleSides(const OdDbObject* pVpObj) const;
  void setCircleSides(OdDbObject* pVpObj, int nSides);

  bool isUcsIconVisible(const OdDbObject* /*pVpObj*/) const{return false;}
  void setUcsIconVisible(OdDbObject* /*pVpObj*/, bool /*bVisible*/){return;}

  bool isUcsIconAtOrigin(const OdDbObject* /*pVpObj*/) const{return false;}
  void setUcsIconAtOrigin(OdDbObject* /*pVpObj*/, bool /*bAtOrigin*/){return;}

  void adjustByGsView(const OdGsView* pView, OdDbObject* pVp);
};


#include "DD_PackPop.h"

#endif // _ODDBABSTRACTVIEWLTABLERECORD_INCLUDED


