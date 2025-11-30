#ifndef _SpatialIndex_h_Included_
#define _SpatialIndex_h_Included_

#if _MSC_VER >= 1000
#pragma once
#endif

#ifdef _TOOLKIT_IN_DLL_
#ifdef SPATIALINDEX_DLL_EXPORTS
#define SPATIALINDEX_API __declspec(dllexport)
#else
#define SPATIALINDEX_API __declspec(dllimport)
#endif
#else
#define SPATIALINDEX_API 
#endif

#include "RxObject.h"
#include "Ge/GeExtents3d.h"
#include "Ge/GeExtents2d.h"

/** Description:

    {group:OdSp_Classes} 
*/
struct SPATIALINDEX_API OdSpShape
{
  virtual bool contains( const OdGeExtents3d&, bool planar ) const = 0;
  virtual bool interects( const OdGeExtents3d&, bool planar ) const = 0;
};

/** Description:

    {group:OdSp_Classes} 
*/
struct OdSpIdentifier
{
  const void* _data1;
  const void* _data2;
};

/** Description:

    {group:OdSp_Classes} 
*/
struct SPATIALINDEX_API OdSpVisitor
{
  virtual void visit( void* data, OdSpIdentifier id ) = 0;
};


/** Description:

    {group:OdSp_Classes} 
*/
struct SPATIALINDEX_API OdSpIndex : public OdRxObject
{
  virtual OdSpIdentifier insert( void* data, const OdGeExtents3d& extent ) = 0;
  virtual bool remove( OdSpIdentifier id ) = 0;
  virtual void query( const OdSpShape& query, OdSpVisitor&, bool count_intersects = false ) const = 0;
  virtual void clear() = 0;
};

typedef OdSmartPtr<OdSpIndex> OdSpIndexPtr;

/** Description:

    {group:DD_Namespaces}
*/
namespace OdSp
{
  SPATIALINDEX_API OdSpIndexPtr CreateSpatialIndex( const OdGeExtents3d&, bool planar = false);
  SPATIALINDEX_API void SetMaxTreeDepth(unsigned char);
  SPATIALINDEX_API void SetMaxNodeSize(unsigned char);
}

#endif
