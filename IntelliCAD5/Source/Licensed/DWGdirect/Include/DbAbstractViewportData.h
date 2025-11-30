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



#ifndef OD_DBABSTRACTVIEWPORTDATA_H
#define OD_DBABSTRACTVIEWPORTDATA_H


#include "DbObject.h"

class OdGsView;
class OdDbViewport;
class OdDbViewportTableRecord;

/** Description:
    Protocol extention class that represents either an OdDbViewport or 
    an OdDbViewportTableRecord.

    See Also:
    OdDbAbstractViewTableRecord

    {group:OdDb_Classes} 
*/
class TOOLKIT_EXPORT OdDbAbstractViewportData : public OdRxObject
{
public:
  ODRX_DECLARE_MEMBERS(OdDbAbstractViewportData);

  virtual double getHeight(OdDbObject* pVpObj) const = 0;
  virtual void setHeight(OdDbObject* pVpObj, double height) = 0;

  virtual double getWidth(OdDbObject* pVpObj) const = 0;
  virtual void setWidth(OdDbObject* pVpObj, double height) = 0;

  virtual void getUcs(const OdDbObject* pVpObj, 
    OdGePoint3d& origin, OdGeVector3d& xAxis, OdGeVector3d& yAxis ) const = 0;
  virtual void setUcs(OdDbObject* pVpObj,
    const OdGePoint3d& origin, const OdGeVector3d& xAxis, const OdGeVector3d& yAxis) = 0;

  virtual bool isUcsIconVisible(const OdDbObject* pVpObj) const = 0;
  virtual void setUcsIconVisible(OdDbObject* pVpObj, bool bVisible = true) = 0;

  virtual bool isUcsIconAtOrigin(const OdDbObject* pVpObj) const = 0;
  virtual void setUcsIconAtOrigin(OdDbObject* pVpObj, bool bAtOrigin = true) = 0;

  virtual int circleSides(const OdDbObject* pVpObj) const = 0;
  virtual void setCircleSides(OdDbObject* pVpObj, int nSides) = 0;

  virtual void adjustGsView(const OdDbObject* pVp, OdGsView* pView) = 0;
  virtual void adjustViewport(const OdDbObject* pVp, OdDbViewport* pView) = 0;
  virtual void adjustViewportRec(const OdDbObject* pVp, OdDbViewportTableRecord* pView) = 0;

  virtual void adjustByGsView(const OdGsView* pView, OdDbObject* pVp) = 0;
  virtual void adjustByViewport(const OdDbViewport* pVp, OdDbObject* pVRec) = 0;
  virtual void adjustByViewportRec(const OdDbViewportTableRecord* pVp, OdDbObject* pVRec) = 0;

};

typedef OdSmartPtr<OdDbAbstractViewportData> OdDbAbstractViewportDataPtr;

#endif // OD_DBABSTRACTVIEWPORTDATA_H


