#ifndef _SpBBox_h_Included_
#define _SpBBox_h_Included_

#if _MSC_VER >= 1000
#pragma once
#endif

#include "Si/SiSpatialIndex.h"
#include "Ge/GeBoundBlock3d.h"

/** Description:

    {group:DD_Namespaces}
*/
namespace OdSi
{
  class ODSI_API BBox : public OdGeBoundBlock3d, public OdSiShape
  {
    static void explodeExtent(OdGePoint3d* points, const OdGeExtents3d& extent)
    {
       points[0] = extent.minPoint();
       points[1].set( extent.minPoint().x, extent.minPoint().y, extent.maxPoint().z );
       points[2].set( extent.minPoint().x, extent.maxPoint().y, extent.minPoint().z );
       points[3].set( extent.maxPoint().x, extent.minPoint().y, extent.minPoint().z );
       points[4].set( extent.maxPoint().x, extent.minPoint().y, extent.maxPoint().z );
       points[5].set( extent.minPoint().x, extent.maxPoint().y, extent.maxPoint().z );
       points[6].set( extent.maxPoint().x, extent.maxPoint().y, extent.minPoint().z );
       points[7] = extent.maxPoint();
    }
  public:
    virtual bool contains( const OdGeExtents3d& extent, bool planar = false ) const
    {
      ODA_ASSERT( planar == false );
      
      OdGePoint3d points[8];
      explodeExtent( points, extent );
      for ( int i = 0; i < 8; i++ )
      {
        if ( !OdGeBoundBlock3d::contains( points[i] ) )
          return false;
      }
      return true;
    }
    
    virtual bool interects( const OdGeExtents3d& ext, bool planar = false) const
    {
      ODA_ASSERT( planar == false );
      return !OdGeBoundBlock3d( ext.minPoint(), ext.maxPoint() ).isDisjoint( *this );
    }

    BBox() {}
    BBox( const BBox& b ) : OdGeBoundBlock3d( b ) {}
    BBox( const OdGePoint3d& origin, const OdGeVector3d& dir1,
      const OdGeVector3d& dir2, const OdGeVector3d& dir3 ) 
      : OdGeBoundBlock3d( origin, dir1, dir2, dir3 ) {}
    BBox( const OdGePoint3d& point1, const OdGePoint3d& point2 )
      : OdGeBoundBlock3d( point1, point2 ){}
  };
}

#endif
