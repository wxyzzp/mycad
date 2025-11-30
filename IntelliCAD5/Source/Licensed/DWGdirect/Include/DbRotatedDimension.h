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



#ifndef _DB_ROTATED_DIMENSION_
#define _DB_ROTATED_DIMENSION_

#include "DD_PackPush.h"

#include "DbDimension.h"

/** Description:
    Represents a rotated dimension in an OdDbDatabase.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbRotatedDimension : public OdDbDimension
{
public:
  ODDB_DECLARE_MEMBERS(OdDbRotatedDimension);

  /** Description:
      Constructor (no arguments).
  */
  OdDbRotatedDimension();
  
  /* OdDbRotatedDimension(
       double rotation,
       const OdGePoint3d& xLine1Point,
       const OdGePoint3d& xLine2Point,
       const OdGePoint3d& dimLinePoint,
       const char* dimText = NULL,
       OdDbObjectId dimStyle = OdDbObjectId::kNull);
  */

  /** Description:
      Returns the start point for the first extension line in this dimension (DXF 13).
  */
  OdGePoint3d xLine1Point() const;

  /** Description:
      Sets the start point for the first extension line in this dimension (DXF 13).
  */
  void setXLine1Point(const OdGePoint3d&);
  
  /** Description:
      Returns the start point for the second extension line in this dimension (DXF 14).
  */
  OdGePoint3d xLine2Point() const;

  /** Description:
      Sets the start point for the second extension line in this dimension (DXF 14).
  */
  void setXLine2Point(const OdGePoint3d&);
  
  /** Description:
      Returns the point along the extension line where the dimension is actually located (DXF 10).
  */
  OdGePoint3d dimLinePoint() const;

  /** Description:
      Returns the point along the extension line where the dimension is actually located (DXF 10).
  */
  void setDimLinePoint(const OdGePoint3d&);
  
  /** Description:
      Returns the obliquing angle for this dimension (DXF 52).
  */
  double oblique() const;

  /** Description:
      Sets the obliquing angle for this dimension (DXF 52).
  */
  void setOblique(double);
  
  /** Description:
      Returns the rotation angle for this dimension (DXF 50).
  */
  double rotation() const;

  /** Description:
      Sets the rotation angle for this dimension (DXF 50).
  */
  void setRotation(double);

  virtual OdResult dxfInFields_R12(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields_R12(OdDbDxfFiler* pFiler) const;

  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;

  virtual void getClassID(void** pClsid) const;
};

typedef OdSmartPtr<OdDbRotatedDimension> OdDbRotatedDimensionPtr;

#include "DD_PackPop.h"

#endif

