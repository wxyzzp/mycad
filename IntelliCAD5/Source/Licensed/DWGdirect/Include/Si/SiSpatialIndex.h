#ifndef _SpatialIndex_h_Included_
#define _SpatialIndex_h_Included_

#if _MSC_VER >= 1000
#pragma once
#endif

#ifdef _TOOLKIT_IN_DLL_
#ifdef SPATIALINDEX_DLL_EXPORTS
#define ODSI_API __declspec(dllexport)
#else
#define ODSI_API __declspec(dllimport)
#endif
#else
#define ODSI_API 
#endif

#include "RxObject.h"
#include "Ge/GeExtents3d.h"
#include "Ge/GeExtents2d.h"

/** Description:

    {group:OdSi_Classes} 
*/
struct OdSiShape
{
  virtual bool contains( const OdGeExtents3d&, bool planar ) const = 0;
  virtual bool interects( const OdGeExtents3d&, bool planar ) const = 0;
};
/** Description:

    {group:OdSi_Classes} 
*/
struct OdSiEntity
{
  virtual OdGeExtents3d extents() const = 0;
};

/** Description:

    {group:OdSi_Classes} 
*/
struct OdSiVisitor
{
  virtual void visit( OdSiEntity* data, bool bCompletelyInside ) = 0;
};

class ODSI_API OdSiSpatialIndex;
typedef OdSmartPtr<OdSiSpatialIndex> OdSiSpatialIndexPtr;

/** Description:

    {group:OdSi_Classes} 
*/
class ODSI_API OdSiSpatialIndex : public OdRxObject
{
public:
  ODRX_DECLARE_MEMBERS(OdSiSpatialIndex);
  static OdSiSpatialIndexPtr createObject( const OdGeExtents3d& overallExtents, bool bPlanar = false );
  virtual void insert( OdSiEntity* entity ) = 0;
  virtual bool remove( OdSiEntity* data ) = 0;
  virtual void query( const OdSiShape& query, OdSiVisitor& visitor ) const = 0;
  virtual void clear() = 0;
  virtual void setMaxTreeDepth( unsigned char nValue ) = 0;
  virtual void setMaxNodeSize( unsigned char nValue ) = 0;
  virtual OdGeExtents3d extents() const = 0;
};


#endif
