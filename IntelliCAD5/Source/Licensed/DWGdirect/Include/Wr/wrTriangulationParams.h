#ifndef _TRIANGULATIONPARAMS_H_
#define _TRIANGULATIONPARAMS_H_

#include "OdHeap.h"

/** Description:

    {group:Other_Classes}
*/
class wrTriangulationParams
{
public:
  OdUInt32 maxNumGridLines;
  double   maxFacetEdgeLength;
  double   normalTolerance;
  double   surfaceTolerance;
  double   gridAspectRatio;
  
  OdUInt16 BetweenKnots;
  OdUInt32 PointsPerEdge;


  // default triangulation parameters
  wrTriangulationParams()
  : maxNumGridLines(10000)
  , maxFacetEdgeLength(0)
  , normalTolerance(15)
  , surfaceTolerance(0)
  , gridAspectRatio(0)
  , PointsPerEdge(0)
  , BetweenKnots(2)
  {
  }

  bool operator ==(const wrTriangulationParams tp) const 
  {
    return tp.maxNumGridLines == maxNumGridLines
    && tp.maxFacetEdgeLength == maxFacetEdgeLength
    && tp.normalTolerance == normalTolerance
    && tp.surfaceTolerance == surfaceTolerance
    && tp.gridAspectRatio == gridAspectRatio
    && tp.PointsPerEdge == PointsPerEdge
    && tp.BetweenKnots == BetweenKnots;
  }
};

#endif // _TRIANGULATIONPARAMS_H_
