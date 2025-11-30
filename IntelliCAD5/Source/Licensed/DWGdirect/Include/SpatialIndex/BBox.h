#ifndef _SpBBox_h_Included_
#define _SpBBox_h_Included_

#if _MSC_VER >= 1000
#pragma once
#endif

#include "SpatialIndex/SpatialIndex.h"
#include "Ge/GeBoundBlock3d.h"

/** Description:

    {group:DD_Namespaces}
*/
namespace OdSp
{
  class SPATIALINDEX_API BBox : public OdGeBoundBlock3d, public OdSpShape
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
      if( planar == false )
      {
        OdGePoint3d points[8];
        explodeExtent( points, extent );
        for ( int i = 0; i < 8; i++ ) 
          if ( !OdGeBoundBlock3d::contains( points[i] ) ) return false;
        return true;
      }
      else
      {
        ODA_FAIL();
        return false;
      }
    }
    
    virtual bool interects( const OdGeExtents3d& ext, bool planar = false) const
    {
      if( planar == false )
      {
        OdGeBoundBlock3d tmp( ext.minPoint(), ext.maxPoint() );
        return !tmp.isDisjoint( *this );
      }
      else
      {
        ODA_FAIL();
      }
      return false;
    }
    BBox( const OdGeBoundBlock3d& block ) : OdGeBoundBlock3d(block) {}
  };
}

#endif
