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



#ifndef OD_CLONEFILERS_INCLUDED
#define OD_CLONEFILERS_INCLUDED

#include "DbFiler.h"
#include "IdArrays.h"

class OdDbIdMapping;

class OdDbDeepCloneFiler;
typedef OdSmartPtr<OdDbDeepCloneFiler> OdDbDeepCloneFilerPtr;

/** Description:

    {group:OdDb_Classes}
*/
class OdDbDeepCloneFiler : public OdDbDwgFiler
{
protected:
  OdDbDeepCloneFiler() {}

public:
  ODRX_DECLARE_MEMBERS(OdDbDeepCloneFiler);

  // Protocol specific to OdDbDeepCloneFiler

  virtual void start() = 0;
  virtual bool nextOwnedObject(OdDbObjectId& id) = 0;
  virtual OdDbIdMapping& idMapping() = 0;

  // Clone any owned objects
  virtual void defaultProcessReferences() = 0;

  static OdDbDeepCloneFilerPtr createObject(OdDbIdMapping* pIdMap);
};

class OdDbWblockCloneFiler;
typedef OdSmartPtr<OdDbWblockCloneFiler> OdDbWblockCloneFilerPtr;

/** Description:

    {group:OdDb_Classes}
*/
class OdDbWblockCloneFiler : public OdDbDwgFiler
{
protected:
  OdDbWblockCloneFiler() {}

public:
  ODRX_DECLARE_MEMBERS(OdDbWblockCloneFiler);

  // Protocol specific to OdDbWblockCloneFiler

  virtual void start() = 0;
  virtual bool nextHardObject(OdDbObjectId& id) = 0;
  virtual OdDbIdMapping& idMapping() = 0;

  // Clone any hard references.
  virtual void defaultProcessReferences() = 0;

  static OdDbWblockCloneFilerPtr createObject(OdDbIdMapping* pIdMap);
};

#endif //OD_CLONEFILERS_INCLUDED


