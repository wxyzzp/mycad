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



#ifndef OD_DBLYFILT_H
#define OD_DBLYFILT_H

#include "DD_PackPush.h"

#include "DbFilter.h"

/** Description:
    Represents a layer filter object in an OdDbDatabase. 

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbLayerFilter : public  OdDbFilter
{
public:
  ODDB_DECLARE_MEMBERS(OdDbLayerFilter);

  /** Description:
      Constructor (no arguments).
  */
  OdDbLayerFilter();

  /** Description:
      Returns the OdRxClass instance of the index class associated with this filter. 
  */
  virtual OdRxClass* indexClass() const;

  /** Description:
      Returns true of all layer names contained in this object are present in the passed in database,
      false otherwise.
  */
  virtual bool isValid(OdDbDatabase* pDb = NULL) const;

  /** Description:
      Adds the specified layer name to this OdDbLayerFilter object. 
  */
  void add(const OdString& layerName);

  /** Description:
      Removes the specified layer name from this OdDbLayerFilter object. 
  */
  void remove(const OdString& layerName);

  /** Description:
      Returns the layer name stored at the specified index in this OdDbLayerFilter object. 
  */
  OdString getAt(int index) const;

  /** Description:
      Returns the number of layer names stored in this object. 
  */
  int layerCount() const;

  OdResult dwgInFields(OdDbDwgFiler* pFiler);

  void dwgOutFields(OdDbDwgFiler* pFiler) const;
  
  OdResult dxfInFields(OdDbDxfFiler* pFiler);

  void dxfOutFields(OdDbDxfFiler* pFiler) const;
};

typedef OdSmartPtr<OdDbLayerFilter> OdDbLayerFilterPtr;

#include "DD_PackPop.h"

#endif // OD_DBLYFILT_H


