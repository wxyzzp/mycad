#ifndef _SpExtents_h_Included_
#define _SpExtents_h_Included_

#if _MSC_VER >= 1000
#pragma once
#endif

#include "SpatialIndex/SpatialIndex.h"

/** Description:

    {group:DD_Namespaces}
*/
namespace OdSp
{
  class SPATIALINDEX_API Extent3d : public OdGeExtents3d, public OdSpShape
  {
  public:
    virtual bool contains( const OdGeExtents3d& extent, bool planar = false ) const
    {
      if ( planar ) 
      {
        ODA_ASSERT ( extent.minPoint().z == 0 && extent.maxPoint().z == 0 );
      }
      return OdGeExtents3d::contains( extent );
    }
    
    virtual bool interects( const OdGeExtents3d& ext, bool planar = false) const
    {
      if ( planar == false )
        return !isDisjoint( ext );
      else
      {
        return !( ext.minPoint().x > maxPoint().x 
          || ext.minPoint().y > maxPoint().y 
          || minPoint().x > ext.maxPoint().x 
          || minPoint().y > ext.maxPoint().y );
      }
    }
    
    Extent3d() {}
    Extent3d( const OdGePoint3d& min, const OdGePoint3d& max )
      : OdGeExtents3d( min, max ) {}
    Extent3d( const OdGeExtents3d& ext ) : OdGeExtents3d( ext ) {}
    Extent3d( const OdGeExtents2d& ext ) 
      : OdGeExtents3d( OdGePoint3d( ext.minPoint().x, ext.minPoint().y, 0 ), 
      OdGePoint3d( ext.maxPoint().x, ext.maxPoint().y, 0 )) {}
  };
}
#endif
