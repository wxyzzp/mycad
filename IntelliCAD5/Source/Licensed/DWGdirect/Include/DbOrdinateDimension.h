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



#ifndef _OD_DB_ORDINATE_DIMENSION_
#define _OD_DB_ORDINATE_DIMENSION_

#include "DD_PackPush.h"

#include "DbDimension.h"

/** Description:
    Represents an ordinate dimension in an OdDbDatabase, which measures 
    the horizontal or vertical distance between 2 points.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbOrdinateDimension : public OdDbDimension
{
public:
  ODDB_DECLARE_MEMBERS(OdDbOrdinateDimension);

  /** Description:
      Constructor (no arguments).
  */
  OdDbOrdinateDimension();
  
  /* OdDbOrdinateDimension(
       bool useXAxis,
       const OdGePoint3d& definingPoint,
       const OdGePoint3d& leaderEndPoint,
       const char* dimText = NULL,
       OdDbObjectId dimStyle = OdDbObjectId::kNull);
  */

  /** Description:
      Returns true if this dimension measures the horizontal distance between 2 points (DXF 70, bit 0x40 set).
  */
  bool isUsingXAxis() const;

  /** Description:
      Returns true if this dimension measures the vertical distance between 2 points (DXF 70, bit 0x40 cleared).
  */
  bool isUsingYAxis() const;

  /** Description:
      Sets this dimension to measure the horizontal distance between 2 points (DXF 70, bit 0x40 set).
  */
  void useXAxis();

  /** Description:
      Sets this dimension to measure the vertical distance between 2 points (DXF 70, bit 0x40 cleared).
  */
  void useYAxis();
  
  /** Description:
      Returns the origin or definition point of this dimension in WCS (DXF 10).
  */
  OdGePoint3d origin() const;

  /** Description:
      Sets the origin or definition point of this dimension in WCS (DXF 10).
  */
  void setOrigin(const OdGePoint3d&);
  
  /** Description:
      Returns the defining point for this dimension (DXF 13).
  */
  OdGePoint3d definingPoint() const;

  /** Description:
      Sets the defining point for this dimension (DXF 13).
  */
  void setDefiningPoint(const OdGePoint3d&);
  
  /** Description:
      Returns the leader end point for this dimension (DXF 14).
  */
  OdGePoint3d leaderEndPoint() const;

  /** Description:
      Sets the leader end point for this dimension (DXF 14).
  */
  void setLeaderEndPoint(const OdGePoint3d&);

  virtual OdResult dxfInFields_R12(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields_R12(OdDbDxfFiler* pFiler) const;

  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;
  
  virtual void getClassID(void** pClsid) const;
};

typedef OdSmartPtr<OdDbOrdinateDimension> OdDbOrdinateDimensionPtr;

#include "DD_PackPop.h"

#endif

