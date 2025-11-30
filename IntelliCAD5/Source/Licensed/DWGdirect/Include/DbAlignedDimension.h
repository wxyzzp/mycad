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



#ifndef _OD_DB_ALIGNED_DIMENSION_
#define _OD_DB_ALIGNED_DIMENSION_

#include "DD_PackPush.h"

#include "DbDimension.h"

/** Description
    Represents a dimension between 2 arbitrary points in space, 
    within an OdDbDatabase.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbAlignedDimension : public OdDbDimension
{
public:
  ODDB_DECLARE_MEMBERS(OdDbAlignedDimension);
  
  /** Description:
      Constructor (no arguments).
  */
  OdDbAlignedDimension();
  
  /** Description:
      Returns the first point of this dimension, in WCS (DXF 13). 
  */
  OdGePoint3d xLine1Point() const;

  /** Description:
      Sets the first point of this dimension, in WCS (DXF 13). 
  */
  void setXLine1Point(const OdGePoint3d&);
  
  /** Description:
      Returns the second point of this dimension, in WCS (DXF 14). 
  */
  OdGePoint3d xLine2Point() const;

  /** Description:
      Sets the second point of this dimension, in WCS (DXF 14). 
  */
  void setXLine2Point(const OdGePoint3d&);
  
  /** Description:
      Returns the point that defines where the dimension line will be, in 
      WCS (DXF 10).
  */
  OdGePoint3d dimLinePoint() const;

  /** Description:
      Sets the point that defines where the dimension line will be, in 
      WCS (DXF 10).
  */
  void setDimLinePoint(const OdGePoint3d&);
  
  /** Description:
      Returns the obliquing angle for this dimension, in radians (DXF 52).
  */
  double oblique() const;

  /** Description:
      Sets the obliquing angle for this dimension, in radians (DXF 52).
  */
  void setOblique(double);

  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;

  virtual OdResult dxfInFields_R12(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields_R12(OdDbDxfFiler* pFiler) const;
  
  virtual void getClassID(void** pClsid) const;
  
  /* OdDbAlignedDimension(
    const OdGePoint3d& xLine1Point,
    const OdGePoint3d& xLine2Point,
    const OdGePoint3d& dimLinePoint,
    const char* dimText = NULL,
    OdDbObjectId dimStyle = OdDbObjectId::kNull);*/
  
};

typedef OdSmartPtr<OdDbAlignedDimension> OdDbAlignedDimensionPtr;

#include "DD_PackPop.h"

#endif

