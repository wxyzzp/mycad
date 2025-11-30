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


#ifndef _OD_LY_H
#define _OD_LY_H

#include "RxObject.h"
#include "OdArray.h"
#include "DbObjectId.h"
#include "IdArrays.h"


class OdLyLayerFilter;
typedef OdSmartPtr<OdLyLayerFilter> OdLyLayerFilterPtr;
typedef OdArray<OdLyLayerFilterPtr> OdLyLayerFilterArray;

class OdDbLayerTableRecord;
class OdDbDxfFiler;
class OdLySystemInternals;

/** Description:
    Main filter API. All layer filters implement this interface.

    {group:OdLy_Classes} 
*/
class TOOLKIT_EXPORT OdLyLayerFilter : public OdRxObject
{
public:
  ODRX_DECLARE_MEMBERS(OdLyLayerFilter);
  OdLyLayerFilter();
  virtual ~OdLyLayerFilter();
  
  // persistent name of filter, some filters may not allow
  // renaming ("All", "Xref", etc.)
  virtual const OdString& name() const;
  virtual bool setName( const OdString& );
  virtual bool allowRename() const;
  virtual OdLyLayerFilter* parent() const;
  
  // nested filter manipulation
  virtual const OdLyLayerFilterArray& getNestedFilters() const;
  virtual void addNested( OdLyLayerFilter* filter );
  virtual void removeNested( OdLyLayerFilter* filter );
  
  // some filters generate nested filters automatically (xref, standard)
  // the following api will be called on filters after they are loaded to allow 
  // them to regenerate their nested filters
  virtual OdResult generateNested();
  
  // returns true if this filter was dynamically generated i.e.
  // it doesn't persist on its own
  virtual bool dynamicallyGenerated() const;
  
  // returns true if this filter can have nested filters
  virtual bool allowNested() const;
  
  // returns true if this filter can be removed
  virtual bool allowDelete() const;
  
  // return true if this filter is a proxy for an
  // unknown filter (we are not going to show these on the UI)
  virtual bool isProxy() const;
  
  // groups are id filters, i.e. filters on layer id.
  virtual bool isIdFilter() const;
  
  // returns true if the given layer is "visible" with this filter
  // false otherwise
  virtual bool filter( OdDbLayerTableRecord* layer ) const;
  
  virtual const OdString& filterExpression() const;
  virtual OdResult setFilterExpression( const OdString& expr );
  
  
  // returns true if this and pOther will allow the same
  // layers
  virtual bool compareTo( const OdLyLayerFilter* pOther ) const;
  
  // persistence support
  //
  // read/write native data, we use a dxf filer to decouple the filter
  // from the underlying xrecod. This is necessary to support custom filter
  // types.
  virtual OdResult readFrom(OdDbDxfFiler* filer);
  virtual void writeTo(OdDbDxfFiler* pFiler) const;
  
private:
  void *mp_impObj;
  friend class OdLySystemInternals;
  
protected:
  OdLyLayerFilter( OdLySystemInternals* );
};

/** Description:

    {group:OdLy_Classes} 
*/
class TOOLKIT_EXPORT OdLyLayerGroup : public OdLyLayerFilter
{
public:
  ODRX_DECLARE_MEMBERS(OdLyLayerGroup);
  OdLyLayerGroup();
  
  virtual void addLayerId( const OdDbObjectId& id );
  virtual void removeLayerId (const OdDbObjectId& id);
  virtual const OdDbObjectIdArray& layerIds() const;
  
protected:
  OdLyLayerGroup( OdLySystemInternals* );
};


/** Description:

    {group:OdLy_Classes} 
*/
class OdLyLayerFilterManager : public OdRxObject
{
public:
  ODRX_DECLARE_MEMBERS(OdLyLayerFilterManager);
  //reads the filters from the database this manager belongs to, and returns
  //the root filter in pRoot.
  //The caller is responsible for deleting  the filters
  //by calling delete pRoot;
  virtual OdResult getFilters( OdLyLayerFilterPtr& pRoot, OdLyLayerFilterPtr& pCurrent ) = 0;
  //writes the filters pointer by pRoot into the current
  //database, call the various writeTo methods on each filter
  virtual void setFilters( const OdLyLayerFilterPtr pRoot, const OdLyLayerFilterPtr pCurrent ) = 0;
};

typedef OdSmartPtr<OdLyLayerFilterManager> OdLyLayerFilterManagerPtr;

// returns new filter manager for the given database (delete after use)
TOOLKIT_EXPORT OdLyLayerFilterManagerPtr odlyGetLayerFilterManager( OdDbDatabase* pDb );

#endif  // _OD_LY_H

