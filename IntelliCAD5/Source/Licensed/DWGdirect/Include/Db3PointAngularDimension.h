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



#ifndef _OD_DB_3POINTANGULAR_DIMENSION_
#define _OD_DB_3POINTANGULAR_DIMENSION_

#include "DD_PackPush.h"

#include "DbDimension.h"

/** Description:
    Represents an angular dimension defined by 3 points within an OdDbDatabase.

    {group:OdDb_Classes} 
*/
class TOOLKIT_EXPORT OdDb3PointAngularDimension : public OdDbDimension
{
public:

  ODDB_DECLARE_MEMBERS(OdDb3PointAngularDimension);

  /** Description:
      Constructor (no argument).
  */
  OdDb3PointAngularDimension();
  
  /** Description:
      Returns the point defining the dimension arc, in WCS (DXF 10).
  */
  OdGePoint3d arcPoint() const;

  /** Description:
      Sets the point defining the dimension arc, in WCS (DXF 10).
  */
  void setArcPoint(const OdGePoint3d&);
  
  /** Description:
      Returns the start point of the first extension line of this dimension, in WCS 
      (DXF 13). This point and the dimension's center point define one side of 
      the angle being dimensioned.
  */
  OdGePoint3d xLine1Point() const;

  /** Description:
      Sets the start point of the first extension line of this dimension, in WCS 
      (DXF 13).  This point and the dimension's center point define one side of 
      the angle being dimensioned.
  */
  void setXLine1Point(const OdGePoint3d&);
  
  /** Description:
      Returns the start point of the second extension line of this dimension, in WCS 
      (DXF 14). This point and the dimension's center point define one side of 
      the angle being dimensioned.
  */
  OdGePoint3d xLine2Point() const;

  /** Description:
      Sets the start point of the second extension line of this dimension, in WCS 
      (DXF 14). This point and the dimension's center point define one side of 
      the angle being dimensioned.
  */
  void setXLine2Point(const OdGePoint3d&);
  
  /** Description:
      Returns the point at the vertex of the angle being dimensioned, in WCS
      (DXF 15).
  */
  OdGePoint3d centerPoint() const;

  /** Description:
      Sets the point at the vertex of the angle being dimensioned, in WCS
      (DXF 15).
  */
  void setCenterPoint(const OdGePoint3d&);

  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;

  virtual OdResult dxfInFields_R12(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields_R12(OdDbDxfFiler* pFiler) const;

  virtual void getClassID(void** pClsid) const;
};

typedef OdSmartPtr<OdDb3PointAngularDimension> OdDb3PointAngularDimensionPtr;

#include "DD_PackPop.h"

#endif

