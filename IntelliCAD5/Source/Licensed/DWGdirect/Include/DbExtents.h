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



#ifndef _ODDBEXTENTS_INCLUDED_
#define _ODDBEXTENTS_INCLUDED_


class OdDbBlockTableRecord;
class OdDbBlockReference;

#include "Ge/GeExtents3d.h"


/** Description:
    Represents a 3d bounding box, represented as a minimum 3d point
    and a maximum 3d point.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbExtents : public OdGeExtents3d
{
public:

  /** Description:
      Constructor (no arguments).
  */
  inline OdDbExtents() {}

  /** Description:
      Constructor that takes min and max point values.
  */
  inline OdDbExtents(const OdGePoint3d& min, const OdGePoint3d& max) : OdGeExtents3d(min, max) {}

  void addBlockExt(const OdDbBlockTableRecord* pBlock, const OdGeMatrix3d& viewMatr = OdGeMatrix3d::kIdentity);

  void addBlockExt(const OdDbBlockReference* pInsert, const OdGeMatrix3d& viewMatr = OdGeMatrix3d::kIdentity);
};


#endif //_ODDBEXTENTS_INCLUDED_


