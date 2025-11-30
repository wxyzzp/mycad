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
// programs incorporating this software must include the following statment 
// with their copyright notices:
//
//      DWGdirect © 2002 by Open Design Alliance Inc. All rights reserved. 
//
// By use of this software, you acknowledge and accept the terms of this 
// agreement.
///////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// 2dExportDevice.cpp: implementation of the Od2dExportDevice, Od2dExportDevice classes.
//                    These are base classes for 2d export.
//////////////////////////////////////////////////////////////////////


#include "OdaCommon.h"
//#include "Entities.h"
#include "2dExportDevice.h"
#include "2dSupport.h"

#include "RxObjectImpl.h"
#include "GeCircArc3d.h"
#include "ColorMapping.h"
#include "GeEllipArc3d.h"
#include "GeLineSeg3d.h"
#include "RxVariantValue.h"
#include "DbRasterImage.h"
#include "DbTextIterator.h"
#include "OdFont.h"
#include "Ge/GeScale3d.h"
//////////////////////////////////////////////////////////////////////
//  Od2dExportDevice
//////////////////////////////////////////////////////////////////////

//====================================================================
//
Od2dExportDevice::Od2dExportDevice() 
{
}


//====================================================================
//
void Od2dExportDevice::update(OdGsDCRect* pUpdatedRect)
{
  OdGsBaseVectorizeDevice::update(pUpdatedRect);
}


//====================================================================
//
//
// getSupportedColorBits()
//
//  Returns the supported number of bits per pixel
//
int Od2dExportDevice::getSupportedColorBits(int nColorBits, bool ) const
{
  int nBitsPerPixel = orgBitonal;

  switch (nColorBits)
  {
  case 1:
    nBitsPerPixel = orgBitonal;
    break;

  case 4:
  case 8:
    nBitsPerPixel = orgMapped;
    break;

  case 16:
  case 24:
    nBitsPerPixel = orgRGB;
    break;

  case 32:
    nBitsPerPixel = orgBGRA;
    break;

  default:
    ODA_ASSERT(false);
    break;
  }
  return nBitsPerPixel;
}



//////////////////////////////////////////////////////////////////////
//  Od2dExportView
//////////////////////////////////////////////////////////////////////

Od2dExportView::Od2dExportView()
{
  m_pSpatialFilter = OdGiSpatialFilter::createObject();
  m_pSpatialFilter->input().addSourceNode(output());

  m_pXYProjector = OdGiXform::createObject();
  m_pXYProjector->input().addSourceNode(m_pSpatialFilter->insideOutput());
  m_pXYProjector->input().addSourceNode(m_pSpatialFilter->intersectsOutput());
}


//====================================================================
//
void Od2dExportView::circleProc(const OdGePoint3d& center,
                                double radius,
                                const OdGeVector3d& normal,
                                const OdGeVector3d* pExtrusion)
{
  setCurrTraits();
  if (!isProcessGeom())
  {
    return;
  }

  if (OdZero(radius))
  {
    // to transform to point at this case
    OdGePoint3d points[2];
    points[1] = points[0] = center;

    OdGeVector3d normals[2];
    normals[1] = normals[0] = normal;

    polylineProc(2, points, normals, pExtrusion, -1);
    return;
  }
  // To transform the entity firstly and then to project onto XYPlane when there is a need.

  OdGeEllipArc3d ellipse;

  OdGeVector3d tmp(OdGeVector3d::kXAxis);    
  OdGeVector3d majorAxis = tmp.transformBy( OdGeMatrix3d::planeToWorld(normal) ).normalize() * radius;
  double radiusRatio = 1.0;
  double startAngle = 0.0;
  double endAngle = Oda2PI; //6.28318530717958647692;

  double dMajorRadius = majorAxis.length();

  ellipse.set(center, 
              majorAxis / dMajorRadius, 
              normal.crossProduct(majorAxis).normalize(),
              dMajorRadius, dMajorRadius * radiusRatio,
              paramFromAngle(startAngle, radiusRatio),
              paramFromAngle(endAngle, radiusRatio)
             );

  OdGe::ErrorCondition ret;
  ellipse.normal(OdGeContext::gTol, ret);
  if (ret != OdGe::kOk)
  {
    OdGiGeometrySimplifier::circleProc(center, radius, normal, pExtrusion);
    return;
  }

  if (!ellipse.normal().isParallelTo(OdGeVector3d::kZAxis))
  {
    // Attention: After projection the returned entity may not be of the same type as the original one.
    //            The returned entity is created with the new operator 
    //            and it is the responsibility of the caller to delete it.

    OdGePlane plane = OdGePlane::kXYPlane;
    OdGeEntity3d* entity = ellipse.orthoProject(plane);

    switch(entity->type())
    {
      case OdGe::kEllipArc3d:   // to copy projected ellipse to ellipse
        ellipse = *(OdGeEllipArc3d*)entity;
        delete entity;
        break;

      case OdGe::kLineSeg3d:    // to output the entity having other type not ellipse
        {
          ODA_ASSERT(false);

          OdGeLineSeg3d* lineProj = (OdGeLineSeg3d*)entity;
          OdGePoint2d pointsOut[2];
          pointsOut[0] = OdGePoint2d(lineProj->startPoint().x, lineProj->startPoint().y);
          pointsOut[1] = OdGePoint2d(lineProj->endPoint().x, lineProj->endPoint().y);
          device()->dc_polyline(2, pointsOut);
        }
        delete entity;
        return;

      default:
        delete entity;
        return;
    }
  }

  device()->dc_circle(ellipse);
}


//====================================================================
//
void Od2dExportView::circleProc(const OdGePoint3d& ptStart,
                              const OdGePoint3d& pt2,
                              const OdGePoint3d& ptEnd,
                              const OdGeVector3d* pExtrusion)
{
  OdGeCircArc3d circArc;
  OdGeError error;
  circArc.set(ptStart, pt2, ptEnd, error);

  if (error == OdGe::kOk)
  {
    OdGiGeometrySimplifier::circleProc(ptStart, pt2, ptEnd, pExtrusion);
  }
  else
  {
    circleProc(circArc.center(), circArc.radius(), circArc.normal(), pExtrusion);
  }
}


//====================================================================
//
void Od2dExportView::circularArcProc(const OdGePoint3d& center,
                                   double radius,
                                   const OdGeVector3d& normal,
                                   const OdGeVector3d& startVector,
                                   double sweepAngle,
                                   OdGiArcType arcType,
                                   const OdGeVector3d* pExtrusion)
{
  setCurrTraits();
  if (!isProcessGeom())
  {
    return;
  }

  if (OdZero(radius))
  {
    // to transform to point at this case
    OdGePoint3d points[2];
    points[1] = points[0] = center;

    OdGeVector3d normals[2];
    normals[1] = normals[0] = normal;

    polylineProc(2, points, normals, pExtrusion, -1);
    return;
  }

  OdGeEllipArc3d ellipse;

  OdGeVector3d majorAxis = startVector.normal() * radius;

  double radiusRatio = 1.0;
  // double startAngle = OdGeVector3d::kXAxis.angleTo( startVector, normal );
  // double endAngle = startAngle + sweepAngle;

  double dMajorRadius = majorAxis.length();

  ellipse.set(center, 
              majorAxis / dMajorRadius, 
              normal.crossProduct(majorAxis).normalize(),
              dMajorRadius, dMajorRadius * radiusRatio,
              0,
              paramFromAngle(sweepAngle, radiusRatio)
             );

  OdGe::ErrorCondition ret;
  ellipse.normal(OdGeContext::gTol, ret);
  if (ret != OdGe::kOk)
  {
    OdGiGeometrySimplifier::circularArcProc( center, radius, normal, startVector,
                                             sweepAngle, arcType, pExtrusion );
    return;
  }

  setEllipseArc(ellipse, normal);
}


//====================================================================
//
void Od2dExportView::circularArcProc(const OdGePoint3d& start,
                                   const OdGePoint3d& point,
                                   const OdGePoint3d& end,
                                   OdGiArcType arcType,
                                   const OdGeVector3d* pExtrusion)
{
  ODA_ASSERT(false);

  setCurrTraits();
  if (!isProcessGeom())
  {
    return;
  }

  OdGeCircArc3d circArc;
  OdGeError error;
  circArc.set(start, point, end, error);

  if (error != OdGe::kOk)
  {
    OdGiGeometrySimplifier::circularArcProc(start, point, end, arcType, pExtrusion);
  }
  else
  {
    OdGeVector3d startVec(circArc.refVec().rotateBy(circArc.startAng(), circArc.normal()));
    circularArcProc( circArc.center(), circArc.radius(), circArc.normal(), startVec,
                     circArc.endAng() - circArc.startAng(), arcType, pExtrusion );
  }
}

//====================================================================
//
void Od2dExportView::ellipArcProc(const OdGeEllipArc3d& arc,
                                  const OdGePoint3d* pEndPointsOverrides,
                                  OdGiArcType arcType,
                                  const OdGeVector3d* pExtrusion)
{
  setCurrTraits();
  if (!isProcessGeom())
  {
    return;
  }

  OdGeEllipArc3d ellipArc(arc);

  OdGe::ErrorCondition ret;
  OdGeVector3d normal = ellipArc.normal(OdGeContext::gTol, ret);

  double dRadius = (ellipArc.majorRadius() > ellipArc.minorRadius()) 
                      ? ellipArc.majorRadius() 
                      : ellipArc.minorRadius();

  if (OdZero(dRadius))
  {
    // to transform to point at this case
    OdGePoint3d points[2];
    points[1] = points[0] = ellipArc.center();

    OdGeVector3d normals[2];
    if (ret == OdGe::kOk)
    {
      normals[1] = normals[0] = normal;
    }

    polylineProc(2, points, (ret == OdGe::kOk) ? normals : NULL, pExtrusion, -1);
    return;
  }

  if (ret == OdGe::kOk)
  {
    if (ellipArc.isCircular(/*OdGeTol(1.0e-6)*/))
    {
      OdGeCircArc3d circle( ellipArc.center(),
                            normal,
                            ellipArc.majorAxis(),
                            dRadius,
                            ellipArc.startAng(), 
                            ellipArc.endAng()
                          );

      if (circle.isClosed())
      {
        circleProc( circle.center(), circle.radius(), normal/*circle.normal()*/, pExtrusion );
      }
      else
      {
        circularArcProc( ellipArc.center(), 
                         dRadius, 
                         normal,
                         ellipArc.startPoint() - ellipArc.center(),
                                         ellipArc.endAng() - ellipArc.startAng(), 
                                         arcType, pExtrusion ); 
      }
    }
    else
    {
      OdGe::ErrorCondition ret;
      ellipArc.correctAxis( OdGeContext::gTol, ret );

      OdGeError flag1, flag2;

      OdGeVector3d vMajor = ellipArc.majorAxis( OdGeContext::gTol, flag1 );
      OdGeVector3d vMinor = ellipArc.minorAxis( OdGeContext::gTol, flag2 );

      if ( ret != OdGe::kOk || flag1 != OdGe::kOk || flag2 != OdGe::kOk)
      {
        OdGiGeometrySimplifier::ellipArcProc( arc, pEndPointsOverrides, arcType, pExtrusion );
      }
      else
      {
        OdGe::ErrorCondition ret;
        OdGeVector3d normalArc = ellipArc.normal(OdGeContext::gTol, ret);     // 1703 // MKU 05.03.2004
        if (ret != OdGe::kOk)
        {
          OdGiGeometrySimplifier::ellipArcProc( arc, pEndPointsOverrides, arcType, pExtrusion );
          return;
        }
        else
        {
          // Everythink is correct. Finally to otput an ellipse.
          setEllipseArc(ellipArc, normalArc /*ellipArc.normal()*/);
        }
      }
    }
  }
  else
  {
    OdGiGeometrySimplifier::ellipArcProc( ellipArc,
                                          pEndPointsOverrides,
                                          arcType,
                                          pExtrusion );
  }
}


//
// setEllipseArc()
//
void Od2dExportView::setEllipseArc(OdGeEllipArc3d& ellipse, const OdGeVector3d& /*normal*/)
{
  // To transform the entity firstly and then to project onto XYPlane when there is a need.

  // Check Coo system and reverse it if there is a need
  if (ellipse.normal().z < 0)
  {
    ellipse.reverseParam();
  }
  device()->dc_ellipse(ellipse);
}

//====================================================================
//
void Od2dExportView::polylineProc(OdInt32 nPoints, 
                              const OdGePoint3d* pVertexList,
                              const OdGeVector3d* /*pNormal*/,
                              const OdGeVector3d* /*pExtrusion*/,
                              OdInt32 /*lBaseSubEntMarker*/)
{
  setCurrTraits();
  if (!isProcessGeom())
  {
    return;
  }

  // toContinue&

  OdGePoint2d* pPoints2d = new OdGePoint2d[nPoints];           // transformed polyline

  for (OdInt32 j = 0; j < nPoints; j++)
  {
    pPoints2d[j] = OdGePoint2d( pVertexList[j].x, pVertexList[j].y );
  }
 
  // OdGePoint2d* pPointsOut = new OdGePoint2d[nPoints];           // output polyline
  // OdUInt32 nPointsOut = 0;                                      // a sign of a new output

  
  // MKU 22.04.2004   OdGiOrthoClipper is turn on
  if (nPoints)
  {
    // The last output
    device()->dc_polyline(nPoints, pPoints2d);
  }

  /*
  for (OdInt32 i = 1; i < nPoints; i++)
  {
    // Loop input polyline per segment. 
    //  If a segment intersects or is completely inside View clip rectangle 
    //  to copy it into current output polyline.
    //  If it will be found a segment which doesn't lie in View Clip 
    //  then to skip it and to throw out current output polyline.

    if (isSegmentOutOfRect( OdGeLineSeg2d(pPoints2d[i - 1], pPoints2d[i]), getMinViewClip(), getMaxViewClip() ))
    {
      // The segment doesn't lie in View clip rectangle.
      if (nPointsOut)

      {
        device()->dc_polyline(nPointsOut, pPointsOut);          // Throw out current polyline
        nPointsOut = 0;                                         // To set a sign of a new output
      }
    }
    else
    {
      if (!nPointsOut)
      { 
        // To begin a new output polyline
        pPointsOut[nPointsOut] = pPoints2d[i - 1];
        nPointsOut++;
      }

      pPointsOut[nPointsOut] = pPoints2d[i];
      nPointsOut++;
    }
  }

  if (nPointsOut)
  {
    // The last output
    device()->dc_polyline(nPointsOut, pPointsOut);
  }

  delete [] pPointsOut;
  */
  delete [] pPoints2d;
}


//====================================================================
//
void Od2dExportView::polygonProc(OdInt32 nPoints,
                               const OdGePoint3d* pVertexList,
                               const OdGeVector3d* pNormal,
                               const OdGeVector3d* /*pExtrusion*/)
{
  setCurrTraits();
  if (!isProcessGeom())
  {
    return;
  }

  if (effectiveTraits().fillType() != kOdGiFillNever)
  {
    OdGePoint2d* pPoints2d = new OdGePoint2d[nPoints];           // transformed polyline

    for (OdInt32 j = 0; j < nPoints; j++)
    {
      pPoints2d[j] = OdGePoint2d( pVertexList[j].x, pVertexList[j].y );
    }

    // If any point of polygon is in View clip rectangle
    //  then the polygon is intersected with View clip rectangle.
    //  To skip only that polygon which completely lie out of this one.

    // MKU 22.04.2004   OdGiOrthoClipper is turn on
    //if (!PolygonOutOfRect2d(nPoints, pPoints2d, getMinViewClip(), getMaxViewClip()))
    {
      device()->dc_polygon(nPoints, pPoints2d);
    }

    delete [] pPoints2d;
  }
  else
  {
    // draw closed polyline
    OdGePoint3dArray points( nPoints + 1 );
    points.insert( points.end(), pVertexList, pVertexList + nPoints );
    points.push_back( pVertexList[0] );
    polylineProc( ++nPoints, points.getPtr(), pNormal );
  }
}

//====================================================================
//
inline bool setFaceTraits(OdGiSubEntityTraits& traits, const OdGiFaceData* pFaceData, int faceIndex)
{
  if (!pFaceData)
  {
    return true;
  }

  if (pFaceData->visibility() && pFaceData->visibility()[faceIndex] == kOdGiInvisible)
  {
    return false;
  }

  if (pFaceData->colors())
  {
    traits.setColor( pFaceData->colors()[faceIndex] );
  }

  if (pFaceData->layerIds())
  {
    traits.setLayer( pFaceData->layerIds()[faceIndex] );
  }

  return true;
}

inline bool setEdgeTraits(OdGiSubEntityTraits& traits, const OdGiEdgeData* pEdgeData, int edgeIndex)
{
  if (!pEdgeData)
  {
    return true;
  }

  if (pEdgeData->visibility() && pEdgeData->visibility()[edgeIndex] == kOdGiInvisible)
  {
    return false;
  }
  if (pEdgeData->colors())
  {
    traits.setColor( pEdgeData->colors()[edgeIndex] );
  }

  if(pEdgeData->layerIds())
  {
    traits.setLayer( pEdgeData->layerIds()[edgeIndex] );
  }

  if(pEdgeData->linetypeIds())
  {
    traits.setLineType( pEdgeData->linetypeIds()[edgeIndex] );
  }

  return true;
}

//====================================================================
//
void Od2dExportView::meshProc(OdInt32 rows,
                            OdInt32 columns,
                            const OdGePoint3d* pVertexList,
                            const OdGiEdgeData* pEdgeData,
                            const OdGiFaceData* pFaceData,
                            const OdGiVertexData* /*pVertexData*/)
{
  OdGiSubEntityTraits& traits = subEntityTraits();

  OdGsView::RenderMode renderMode = mode();

  bool bFillOn = (( renderMode == OdGsView::k2DOptimized 
                    || renderMode>OdGsView::kHiddenLine) 
                  && effectiveTraits().fillType() == kOdGiFillAlways);

  bool bFillOff = !bFillOn 
                  || renderMode > OdGsView::kGouraudShaded;
  
  OdInt32 row, col, nVertex, nEdge, nFaceIndex;
  OdInt32 nrows = rows, ncols = columns;

  --nrows;
  --ncols;

  if (bFillOn)
  {
    OdGePoint3d ptsPolygon[4];

    for (row= 0; row < nrows; ++row)
    {
      for(col= 0; col < ncols; ++col)
      {
        nFaceIndex = row * (columns - 1) + col;

        if (::setFaceTraits( traits, pFaceData, nFaceIndex++ ) && effectiveTraits().visibility())
        {
          setCurrTraits();

          nVertex = row * columns + col;
          ptsPolygon[0] = pVertexList[ nVertex ];
          ptsPolygon[3] = pVertexList[ nVertex + 1 ];

          nVertex += columns;

          ptsPolygon[1] = pVertexList[ nVertex ];
          ptsPolygon[2] = pVertexList[ nVertex + 1 ];

          polygonProc( 4, ptsPolygon );
        }
      }
    }
  }

  if (bFillOff)
  {
    OdGePoint3d ptsLine[2];

    for (row = 0; row < nrows; ++row)
    {
      for (col = 0; col < ncols; ++col)
      {
        nVertex = row * columns       + col;
        nEdge   = row * (columns - 1) + col;

        if (::setEdgeTraits( traits, pEdgeData, nEdge ) && effectiveTraits().visibility())
        {
          setCurrTraits();

          ptsLine[0] = pVertexList[nVertex];
          ptsLine[1] = pVertexList[nVertex + 1];
          polylineProc( 2, ptsLine );
        }
        
        nEdge   = (columns - 1) * rows + row + col * (rows - 1);

        if (::setEdgeTraits( traits, pEdgeData, nEdge ) && effectiveTraits().visibility())
        {
          setCurrTraits();

          ptsLine[0] = pVertexList[nVertex];
          ptsLine[1] = pVertexList[nVertex + columns];
          polylineProc( 2, ptsLine );
        }
      }
    }

    for (row = 0; row < nrows; ++row)
    {
      nEdge   = ncols * rows + ncols * nrows + row;

      if (::setEdgeTraits( traits, pEdgeData, nEdge ) && effectiveTraits().visibility())
      {
        setCurrTraits();

        nVertex = row * columns + ncols;

        ptsLine[0] = pVertexList[nVertex];
        ptsLine[1] = pVertexList[nVertex + columns];
        polylineProc( 2, ptsLine );
      }
    }

    for (col = 0; col < ncols; ++col)
    {
      nEdge   = row * (columns - 1) + col;

      if (::setEdgeTraits( traits, pEdgeData, nEdge ) && effectiveTraits().visibility())
      {
        setCurrTraits();

        nVertex = row * columns + col;

        ptsLine[0] = pVertexList[nVertex];
        ptsLine[1] = pVertexList[nVertex + 1];
        polylineProc( 2, ptsLine );
      }
    }
  }
}


//====================================================================
//
void Od2dExportView::shellProc(OdInt32 nbVertex,
                             const OdGePoint3d* pVertexList,
                             OdInt32 faceListSize,
                             const OdInt32* pFaceList,
                             const OdGiEdgeData* pEdgeData,
                             const OdGiFaceData* pFaceData,
                             const OdGiVertexData* pVertexData)
{
  //setCurrTraits();
  //if (!isProcessGeom())
  //{
  //  return;
  //}

  const OdCmEntityColor* pColors = NULL;
  if(pVertexData)
  {
    pColors = pVertexData->trueColors();
  }

  if (pColors != NULL)
  {
    if (isProcessGeom())
    {
      device()->dc_gouraud_polytriangle( pVertexList, faceListSize, pFaceList, pColors );
    }
  }
  else
  {
    setShell( nbVertex, pVertexList, faceListSize, pFaceList, pEdgeData, pFaceData);
  }
}


//====================================================================
//
void Od2dExportView::setShell( OdInt32 nbVertex,
                               const OdGePoint3d* pVertexList,
                               OdInt32 faceListSize,
                               const OdInt32* pFaceList,
                               const OdGiEdgeData* pEdgeData,
                               const OdGiFaceData* pFaceData )
{
  // this isn't gradient shell

  OdGsView::RenderMode renderMode = mode();
  bool bFillOn = (( renderMode == OdGsView::k2DOptimized 
                    || renderMode > OdGsView::kHiddenLine ) 
                  && effectiveTraits().fillType() == kOdGiFillAlways );

  bool bFillOff = !bFillOn 
                  || renderMode > OdGsView::kGouraudShaded;

  const OdInt32* pFaceListEnd = pFaceList + faceListSize;
  // const OdInt32* pFaceListStart = pFaceList;
  OdInt32 nFaceSize  = 0;
  OdInt32 nEdgeIndex = 0;
  OdInt32 nFaceIndex = 0;

  OdGePoint3d ptsLine[2];

  OdGiSubEntityTraits& traits = subEntityTraits();

  while(pFaceList < pFaceListEnd)
  {
    if (::setFaceTraits( traits, pFaceData, nFaceIndex++ ) && effectiveTraits().visibility())
    {
      setCurrTraits();

      const OdInt32* pSaveFaceList = pFaceList;

      if(bFillOn)
      {
        ODA_ASSERT(*pFaceList > 0);

        nFaceSize = abs(*pFaceList);
        ++pFaceList;

        OdArray<OdGePoint3d>  dcLoops( nbVertex );
      
        while (nFaceSize--)
        {
          dcLoops.append( pVertexList[*pFaceList] );
          ++pFaceList;
        }
        if (pFaceList < pFaceListEnd && *pFaceList < 0) // hole
        {
          OdGeIntArray counts;
          counts.append( dcLoops.size() );

          do
          {
            nFaceSize = abs(*pFaceList);
            ++pFaceList;
            counts.append(nFaceSize);

            while (nFaceSize--)
            {
              dcLoops.append( pVertexList[*pFaceList] );
              ++pFaceList;
            }
          }
          while (pFaceList < pFaceListEnd && *pFaceList < 0);

          int iAllCounts = 0;
          for ( int i = 0, nCounts = counts.size(); i < nCounts; i++)
          {
            int nSize = counts[i];
            polygonProc( nSize, dcLoops.getPtr() + iAllCounts );  // to output the polygon
            iAllCounts += nSize;
          }
        }
        else
        {
          polygonProc( dcLoops.size(), dcLoops.getPtr() );        // to output the polygon
        }
      }

      if (bFillOff)
      {
        pFaceList = pSaveFaceList;
        
        const OdGePoint3d* pPoint;
        do
        {
          nFaceSize = abs(*pFaceList);
          ++pFaceList;
          
          OdInt32 nFirstPoint = *pFaceList;
          ++pFaceList;
          
          pPoint = pVertexList + nFirstPoint;
          ptsLine[0] = *pPoint;                                   // to set the start point of Line

          while (--nFaceSize)
          {
            pPoint = pVertexList + *pFaceList;
            if (::setEdgeTraits(traits, pEdgeData, nEdgeIndex++) && effectiveTraits().visibility())
            {
              setCurrTraits();

              ptsLine[1] = *pPoint;                               // to set the end point of Line
              polylineProc( 2, ptsLine );                         // to output the polyline
              ptsLine[0] = ptsLine[1];                            // to reset the start point of Line
            }
            else
            {
              ptsLine[0] = *pPoint;                               // to set the start point of Line
            }
            ++pFaceList;
          }
          if (::setEdgeTraits(traits, pEdgeData, nEdgeIndex++) && effectiveTraits().visibility())
          {
            setCurrTraits();

            pPoint = pVertexList + nFirstPoint;
            ptsLine[1] = *pPoint;                                 // to set the end point of Line
            polylineProc( 2, ptsLine );                           // to output the polyline
            ptsLine[0] = ptsLine[1];                              // to reset the start point of Line
          }
        }
        while (pFaceList < pFaceListEnd && *pFaceList < 0);
      }
    }
    else // skip face
    {
      do
      {
        nFaceSize = abs(*pFaceList);
        ++pFaceList;
        pFaceList += nFaceSize;

        if (nFaceSize > 2)
        {
          nEdgeIndex += nFaceSize;
        }
        else
        {
          ++nEdgeIndex;
        }
      }
      while (pFaceList < pFaceListEnd && *pFaceList < 0);
    }
  }
}


//====================================================================
//
/*
void Od2dExportView::textProc(const OdGePoint3d& position,
                            const OdGeVector3d& normal,
                            const OdGeVector3d& direction,
                            double height, double width,
                            double oblique, const OdChar* msg,
                            const OdGeVector3d* pExtrusion)
*/
/*
void Od2dExportView::textProc(const OdGePoint3d& position,
                          const OdGeVector3d& u, 
                          const OdGeVector3d& v,
                          const OdChar* msg,
                          const OdGeVector3d* pExtrusion)
{
  setCurrTraits();
  if (!isProcessGeom())
  {
    return;
  }
}
 */
//====================================================================
//
/*
void Od2dExportView::text(const OdGePoint3d& position,
                            const OdGeVector3d& normal,
                            const OdGeVector3d& direction,
                            const OdChar* msg, OdInt32 length,
                            bool raw,
                            const OdGiTextStyle* pTextStyle,
                            const OdGeVector3d* pExtrusion)
*/
void Od2dExportView::textProc(const OdGePoint3d& position,
                          const OdGeVector3d& u, // normal
                          const OdGeVector3d& v,  // direction
                          const OdChar* msg,
                          OdInt32 length,
                          bool raw,                       // informing whether to interpret escape codes
                          const OdGiTextStyle* pStyle,
                          const OdGeVector3d* pExtrusion)
{
  if (length == 0)  
  {
    return;               // Text must be not empty
  }
  setCurrTraits();
  if (!isProcessGeom())
  {
    return;
  }

  // To save current linetype & filltype 
  //  and to set Continuous & FillNever for text
  OdDbStub* ltpId = effectiveTraits().lineType();
  setLineType( device()->getDb()->getLinetypeContinuousId() );

  OdGiFillType fillType = effectiveTraits().fillType();
  setFillType(kOdGiFillNever);

  if (!pStyle->isTtfFont())
  {
    // for SHX font
    //
    ODA_ASSERT(pStyle->isShxFont());

    shxText(position, u, v, msg, length, raw, pStyle, pExtrusion);
  }
  else
  {
    // for True Type Font only

    // Prepare Font
    //
    bool bBold;
    bool bItalic;
    int  nCharset;
    int  nPitchAndFamily;
	  pStyle->ttfdecriptor().getTtfFlags(bBold, bItalic, nCharset, nPitchAndFamily);


    OdGeVector2d yDir;
    yDir = (const OdGeVector2d&)v;

    // To set font

    Od2dFont odFont;
    odFont.m_height = (pStyle->getFont()->getHeight() - pStyle->getFont()->getInternalLeading())
                      * yDir.length()
                      / pStyle->getFont()->fontAbove();

    odFont.m_font_name = pStyle->ttfdecriptor().typeface().c_str();

    odFont.m_charset = nCharset;
    odFont.m_pitch   = pStyle->ttfdecriptor().getPitch();
    odFont.m_family  = pStyle->ttfdecriptor().getFamily();

    odFont.m_bold = bBold;
    odFont.m_italic = bItalic;
    odFont.m_underline = pStyle->isUnderlined();

                                        // describes how the entire string is rotated from horizontal
    odFont.m_rotation = OdaToDegree(OdGeVector3d::kXAxis.angleTo( u, OdGeVector3d::kZAxis));
    odFont.m_width_scale = pStyle->xScale();
                                        // The desired rotation of the individual characters within the font
    odFont.m_oblique = OdaPI2 - u.angleTo(v, OdGeVector3d::kZAxis);

    if (pStyle->isVertical())
    {
      odFont.m_flags |= Od2dFont::VERTICAL;
    }
    if (pStyle->isUpsideDown())
    {
      odFont.m_flags |= Od2dFont::MIRROR_Y;
    }
    if (pStyle->isBackward())
    {
      odFont.m_flags |= Od2dFont::MIRROR_X;
    }
    if (pStyle->isUnderlined())
    {
      odFont.m_flags |= Od2dFont::UNDERSCORE;
    }
    if (pStyle->isOverlined())
    {
      odFont.m_flags |= Od2dFont::OVERSCORE;
    }

    // MKU 25.08.2004
    //  A raw value of true indicates that the escape codes {%%U, %%d, %%P, and %%nnn} 
    //  should not be interpreted
    /*if ( raw )
    {
      //setText(position, u, v, msg, length, raw, pStyle, true, &odFont);
    }
    else*/
    {
      bool bASCII(true);
      OdString sProcessedText;
      OdArray<OdUInt16>    arrayUnicode;
      //OdArray<OdGePoint3d> arrayOverlines;
      //OdArray<OdGePoint3d> arrayUnderlines;


      //OdGiConveyorEntryPoint  giEntryPoint;
      //OdGiXformPtr            pFontXform;

      if (!textProcessing( position, u, v, msg, length, raw, pStyle, sProcessedText,
                           /*giEntryPoint, pFontXform,*/ arrayUnicode, /*arrayOverlines, arrayUnderlines,*/
                           true)
                         )
      {
        bASCII = false;
        arrayUnicode.clear();
        //arrayOverlines.clear();
        //arrayOverlines.clear();

        textProcessing( position, u, v, msg, length, raw, pStyle, sProcessedText,
                        /*giEntryPoint, pFontXform,*/ arrayUnicode, /*arrayOverlines, arrayUnderlines,*/
                        false);
      }

      setText( position, u, v, 
               (bASCII) ? sProcessedText.c_str() : msg, 
               (bASCII) ? sProcessedText.getLength() : length, 
               true, pStyle, true, &odFont, 
               (bASCII) ? NULL : &arrayUnicode
             );

      /*
      // Draws Overlines
      for (int i = 0, nSize = arrayOverlines.size(); i < nSize; )
      {
        OdGePoint3d pointsOver[2];
        pointsOver[0] = arrayOverlines[i++];
        pointsOver[1] = arrayOverlines[i++];

        (&giEntryPoint.geometry())->polylineProc(2, pointsOver);
      }

      // Draws Underlines
      for (i = 0, nSize = arrayUnderlines.size(); i < nSize; )
      {
        OdGePoint3d pointsUnder[2];
        pointsUnder[0] = arrayUnderlines[i++];
        pointsUnder[1] = arrayUnderlines[i++];

        (&giEntryPoint.geometry())->polylineProc(2, pointsUnder);
      }
      */
    } 
  }

  // To restore linetype & filltype
  subEntityTraits().setLineType(ltpId);
  subEntityTraits().setFillType(fillType);
}


bool Od2dExportView::textProcessing( const OdGePoint3d& position,
                                     const OdGeVector3d& u,
                                     const OdGeVector3d& v,
                                     const OdChar* msg,
                                     OdInt32 length,
                                     bool raw,
                                     const OdGiTextStyle* pStyle,
                                     OdString& sProcessedText,
                                     //OdGiConveyorEntryPoint& giEntryPoint,
                                     //OdGiXformPtr&  pFontXform,
                                     OdArray<OdUInt16>& arrayUnicode,
                                     //OdArray<OdGePoint3d>& arrayOverlines,
                                     //OdArray<OdGePoint3d>& arrayUnderlines,
                                     bool bASCII
                                   )
{
  OdFont* pDrawFont = pStyle->getFont();

  OdGiConveyorEntryPoint  giEntryPoint;
  OdGiXformPtr            pFontXform;
  pFontXform = OdGiXform::createObject();

  pFontXform->input().addSourceNode( giEntryPoint );
  pFontXform->output().setDestGeometry( *this );

  OdGeMatrix3d charXForm;
  OdGePoint2d advance;
  charXForm.setCoordSystem(position, u, v, u.crossProduct( v ));

  OdGeScale3d scale(1. / pDrawFont->fontAbove());
  charXForm.setToProduct(OdGeMatrix3d::scaling( scale, position), charXForm );

  OdDbTextIteratorPtr itText = OdDbTextIterator::createObject( msg, length, raw,
                                                               pStyle->getCodePage(),
                                                               pStyle );
  OdCharacterProperties chProperties;
  OdTextProperties textFlags;
  //bool bOverlined(false);
  //bool bUnderlined(false);

  textFlags.setVerticalText(pStyle->isVertical());
  textFlags.setTrackingPercent(pStyle->trackingPercent());
  textFlags.setIncludePenups(false);

  OdGePoint3d pointsUnder[2];
  OdGePoint3d pointsOver[2];

  bool bUnicode(false);
  for( bool bLast = false; !bLast && !bUnicode; bLast = itText->currProperties().bLastChar )
  {
    OdUInt16 ch = itText->nextChar();
    if ( ch == 0 ) break;

    if (bASCII)
    { 
      // ASCII case
      if ( ch < 128 ) 
      {
        sProcessedText += (char)ch;
      }
      else 
      {
        bUnicode = true;
        break;
      }
    } 
    else 
    { 
      // Unicode case
	    arrayUnicode.push_back( ch );
    }

    if (!raw)
    {
      chProperties = itText->currProperties();

      textFlags.setUnderlined( chProperties.bUnderlined );
      textFlags.setOverlined( chProperties.bOverlined );

      pFontXform->setTransform(charXForm);

      pDrawFont->getScore( ch, advance, pointsOver, pointsUnder, textFlags );

      if (textFlags.isOverlined()) 
      {
        (&giEntryPoint.geometry())->polylineProc(2, pointsOver);
      }
      if (textFlags.isUnderlined())
      {
          (&giEntryPoint.geometry())->polylineProc(2, pointsUnder);
      }
    }

    /*
    if (textFlags.isOverlined()) 
    {
      if (!bOverlined)
      {
        // Starts the overline; sets the start and the end points
        arrayOverlines.push_back(pointsOver[0]);
        arrayOverlines.push_back(pointsOver[1]);
      }
      else
      {
        // Coninues the overline; changes the end overline point
        arrayOverlines.removeLast();
        arrayOverlines.push_back(pointsOver[1]);
      }
      bOverlined = true;
    }
    else
    {
      bOverlined = false;
    }
    if (textFlags.isUnderlined())
    {
      if (!bUnderlined)
      {
        // Starts the Underline; sets the start and the end points
        arrayUnderlines.push_back(pointsUnder[0]);
        arrayUnderlines.push_back(pointsUnder[1]);
      }
      else
      {
        // Coninues the Underline; changes the end Underline point
        arrayUnderlines.removeLast();
        arrayUnderlines.push_back(pointsUnder[1]);
      }
      bUnderlined = true;
    }
    else
    {
      bUnderlined = false;
    }
    */

    charXForm.setToProduct(charXForm, OdGeMatrix3d::translation(OdGeVector3d(advance.x, advance.y, 0.)));
  }

  return !bUnicode;
}



//====================================================================
//
void Od2dExportView::shapeProc(const OdGePoint3d& position,
                             const OdGeVector3d& u,
                             const OdGeVector3d& v,
                             int shapeNo, const OdGiTextStyle* pStyle,
                             const OdGeVector3d* pExtrusion)
{
  setCurrTraits();
  if (!isProcessGeom())
  {
    return;
  }
  context()->drawShape(this, position, u, v, shapeNo, pStyle, pExtrusion);
}


//====================================================================
//
void Od2dExportView::xlineProc(const OdGePoint3d& pt1, const OdGePoint3d& pt2)
{
  setCurrTraits();
  if (!isProcessGeom())
  {
    return;
  }

  OdGePoint3d ptsLine[2];

  ptsLine[0] = pt1;
  ptsLine[1] = pt2;
  polylineProc( 2, ptsLine );
}

//====================================================================
//
void Od2dExportView::rayProc(const OdGePoint3d& pt1, const OdGePoint3d& pt2)
{
  setCurrTraits();
  if (!isProcessGeom())
  {
    return;
  }

  OdGePoint3d ptsLine[2];

  ptsLine[0] = pt1;
  ptsLine[1] = pt2;
  polylineProc( 2, ptsLine );
}


//====================================================================
//
void Od2dExportView::nurbsProc(const OdGeNurbCurve3d& nurbs)
{
  setCurrTraits();
  if (!isProcessGeom())
  {
    return;
  }

  OdGiGeometrySimplifier::nurbsProc(nurbs);
}


//====================================================================
//
// rasterImageDc()
//

class ImageDrawable : public OdGiDrawable
{
  const OdGePoint3d&      m_origin;
  const OdGeVector3d&     m_u;
  const OdGeVector3d&     m_v;
  const OdGiRasterImage*  m_pImage;       // image object
  const OdGePoint2d*      m_uvBoundary;
  OdUInt32                m_numBoundPts;
  bool                    m_transparency;
  double                  m_brightness;
  double                  m_contrast;
  double                  m_fade;
  OdGsNode*               m_pGsNode;
  OdCmEntityColor         m_traitsCol;

public:
  ImageDrawable( const OdGePoint3d& origin,
                 const OdGeVector3d& u,
                 const OdGeVector3d& v,
                 const OdGiRasterImage* pImage, // image object
                 const OdGePoint2d* uvBoundary, // may not be null
                 OdUInt32 numBoundPts,
                 bool transparency,
                 double brightness,
                 double contrast,
                 double fade,
                 const OdCmEntityColor& traitsCol
               )
      : m_origin( origin )
      , m_u( u )
      , m_v( v )
      , m_pImage( pImage )
      , m_uvBoundary( uvBoundary )
      , m_numBoundPts( numBoundPts )
      , m_transparency( transparency )
      , m_brightness( brightness )
      , m_contrast( contrast )
      , m_fade( fade )
      , m_pGsNode( NULL )
      , m_traitsCol( traitsCol )
    {
    }

  virtual void addRef()
  {}
  virtual void release()
  {}

  virtual OdUInt32 setAttributes(OdGiDrawableTraits*) const
  {
    return 0;
  }

  virtual bool worldDraw(OdGiWorldDraw*) const
  {
    return false;
  }

  virtual void viewportDraw(OdGiViewportDraw* vd) const
  {
    vd->subEntityTraits().setTrueColor(m_traitsCol);
    vd->geometry().rasterImageDc( m_origin,
                                  m_u,
                                  m_v,
                                  m_pImage,
                                  m_uvBoundary,
                                  m_numBoundPts,
                                  m_transparency,
                                  m_brightness,
                                  m_contrast,
                                  m_fade
                                );
  }

  virtual bool isPersistent() const 
  { 
    return false; 
  }
  virtual OdDbStub* id() const 
  { 
    return 0; 
  }

  virtual void setGsNode(OdGsNode* gsnode) 
  { 
    m_pGsNode = gsnode; 
  }

  virtual OdGsNode* gsNode() const { return m_pGsNode; }

  //virtual OdResult getGeomExtents(OdGeExtents3d& ) const;
};

class PolygonDrawable : public OdGiDrawable
{
  const OdGePoint3d*  m_pPts;
  OdUInt32            m_ptsNum;
  OdGsNode*           m_pGsNode;

public:
  PolygonDrawable(const OdGePoint3d* pPts, OdUInt32 ptsNum)
    : m_pPts(pPts)
    , m_ptsNum(ptsNum)
    , m_pGsNode(NULL)
  {}

  virtual void addRef()
  {}
  virtual void release()
  {}

  virtual OdUInt32 setAttributes(OdGiDrawableTraits*) const
  {
    return 0;
  }

  virtual bool worldDraw(OdGiWorldDraw* pWd) const
  {
    pWd->subEntityTraits().setTrueColor(OdCmEntityColor(255, 255, 255));
    pWd->subEntityTraits().setFillType(kOdGiFillAlways);
    pWd->geometry().polygon(m_ptsNum, m_pPts);
    return true;
  }

  virtual void viewportDraw(OdGiViewportDraw*) const
  {
    return ;
  }

  virtual bool isPersistent() const 
  { 
    return false; 
  }
  virtual OdDbStub* id() const 
  { 
    return 0; 
  }

  virtual void setGsNode(OdGsNode* gsnode) 
  { 
    m_pGsNode = gsnode; 
  }

  virtual OdGsNode* gsNode() const { return m_pGsNode; }

  //virtual OdResult getGeomExtents(OdGeExtents3d& ) const;
};

void Od2dExportView::rasterImageProc(const OdGePoint3d& origin,
                                     const OdGeVector3d& u,
                                     const OdGeVector3d& v, // now it is in screen coordinates
                                     const OdGiRasterImage* pImage, // image object
                                     const OdGePoint2d* uvBoundary, // may not be null
                                     OdUInt32 numBoundPts,
                                     bool transparency,
                                     double brightness,
                                     double contrast,
                                     double fade)
{
  OdUInt32 i;

  if(!pImage)
  {
    return;
  }

  // Image is a processed object now. 
  //  The idea is to get new bitmap containing the Image 
  //  which could be turned around using the Bitmap Device.
  //
  if (!isProcessGeom())
  {
    return;
  }

  OdCmEntityColor entCol;
  entCol = effectiveTraits().trueColor();

  // entCol must be true color because image device may have different 
  // palette with dwf device
  if(entCol.colorMethod() == OdCmEntityColor::kByACI)
  {
    ODCOLORREF colRef = device()->getColor(entCol.colorIndex());
    entCol = OdCmEntityColor(ODGETRED(colRef), ODGETGREEN(colRef), ODGETBLUE(colRef));
  }

  ImageDrawable drawImage(origin,
                          u,
                          v,
                          pImage, // image object
                          uvBoundary, // may not be null
                          numBoundPts,
                          transparency,
                          brightness,
                          contrast,
                          fade,
                          entCol); // traits color need for bitonal images

  // If the image is rotated the transparency is necessary
  //  Note: we can receive another number of bits per pixel in this case on the destination image 
  OdGsDevicePtr pDevice  = odSystemServices()->gsBitmapDevice();     // get Bitmap Device
  pDevice->setUserGiContext(userGiContext());
  
  OdUInt32 pixelWidth  = pImage->pixelWidth();
  OdUInt32 pixelHeight = pImage->pixelHeight();
  // pImage->size(pixelWidth, pixelHeight);        // get Image size in pixels

  if (pixelWidth == 0 || pixelHeight == 0)
  {
    return;
  }

  //int nColorBits = pImage->colorDepth();        // retrieves the number of bits per pixel used for colors 
                                                //  on the destination image
  /* int nColorBits = device()->getSupportedColorBits(
                                    pImage->colorDepth(), // retrieves the number of bits per pixel used for  
                                                          //  colors on the destination image
                                    isTransparency(u, v));*/
  int colorDepth = pImage->colorDepth();

  // To get Image extents

  /* OdGePoint3dArray array;
  OdGePoint3dArray arrayExtents;
  array.resize(4);
  arrayExtents.resize(4);

  array[0] = origin;
  array[1] = array[0] + u * pixelWidth;
  array[3] = array[0] + v * pixelHeight;
  array[2] = array[0] + u * pixelWidth + v * pixelHeight;

  for(int k = 0; k < 4; k ++)
  {
    arrayExtents[k] = array[k];
  }

  OdGePoint3d minImageExtent;
  OdGePoint3d maxImageExtent;

  calculateExtents(minImageExtent, maxImageExtent, array);*/
  
  // clipping boundary
  const OdGePoint2d* pClpBnd = uvBoundary;
  OdGePoint2dArray uvBndBuf;
  OdUInt32 effNumBndPts(numBoundPts);
  if(!pClpBnd)
  {
    uvBndBuf.resize(2);
    uvBndBuf[0] = OdGePoint2d::kOrigin - OdGeVector2d(0.5, 0.5);
    uvBndBuf[1].set(pixelWidth - 0.5, pixelHeight - 0.5);
    pClpBnd = uvBndBuf.asArrayPtr();
    effNumBndPts = 2;
  }
  
  OdGePoint2dArray uvBndBuf1;
  if(effNumBndPts == 2)
  {
    uvBndBuf1.resize(4);
    uvBndBuf1[0] = pClpBnd[0];
    uvBndBuf1[1] = OdGePoint2d(pClpBnd[0].x, pClpBnd[1].y);
    uvBndBuf1[2] = pClpBnd[1];
    uvBndBuf1[3] = OdGePoint2d(pClpBnd[1].x, pClpBnd[0].y);
    pClpBnd = uvBndBuf1.asArrayPtr();
    effNumBndPts = 4;
  }

  // is transparency need
  bool bIsTransp(false);
  if(numBoundPts > 3)
    bIsTransp = true; // non-rectangular clip
  else if((numBoundPts == 2) && (!uvBoundary[0].isEqualTo(OdGePoint2d(-0.5, -0.5)) || 
    !uvBoundary[1].isEqualTo(OdGePoint2d(pixelWidth - 0.5, pixelHeight - 0.5))))
    bIsTransp = true; // rectangular clip, but clips out part of image

  if(!(u.isParallelTo(OdGeVector3d::kXAxis) && v.isParallelTo(OdGeVector3d::kYAxis)) && 
     !(u.isParallelTo(OdGeVector3d::kYAxis) && v.isParallelTo(OdGeVector3d::kXAxis)))
    bIsTransp = true; // not aligned with CS

  if((colorDepth==1) && transparency)
    bIsTransp = false; // in this case need not any masks
                       // (one of the images' colors can be done transparent)

  OdGeMatrix3d uv2World;
  uv2World.setCoordSystem(origin + 0.5*u - 0.5*v + pixelHeight*v, 
    u, -v, OdGeVector3d());

  OdGeExtents3d imgExts; // in WCS
  OdGePoint3dArray worldBound;
  worldBound.resize(effNumBndPts);
  for(i = 0; i < effNumBndPts; i ++)
  {
    worldBound[i] = OdGePoint3d(pClpBnd[i].x, pClpBnd[i].y, 0.0);
    worldBound[i].transformBy(uv2World);
    imgExts.addPoint(worldBound[i]);
  }
  
  // Create View containing only one (Image) object.
  //  Therefore to set a Viewport for the Image.
  //
  double dVpWidth  = imgExts.maxPoint().x - imgExts.minPoint().x;
  double dVpHeight = imgExts.maxPoint().y - imgExts.minPoint().y;
  OdGePoint3d ptVpCenter( (imgExts.maxPoint().x + imgExts.minPoint().x) / 2,
                          (imgExts.maxPoint().y + imgExts.minPoint().y) / 2,
                          0.0
                        );

  double dU = u.length();       // size of one pixel in the World Coo
  double dV = v.length();
  double dW = odmin(dU, dV);
  OdUInt32 pixelWidthNew  = RoundDblToUInt32(dVpWidth / dW);
  OdUInt32 pixelHeightNew = RoundDblToUInt32(dVpHeight / dW);

  // Set the Bitmap Device 
  //  and in our case to create View contaning only Image. 
  //  New View should have the same settings as the current View.
  //  Note: Our Device will have only one View because only one Viewport will be set
  //
  OdGsViewPtr pView = pDevice->createView();
  pDevice->addView(pView);

  OdGsDCRect   gsRect(0, pixelWidthNew, pixelHeightNew, 0);   // in pixels

  OdGeVector3d viewDir(OdGeVector3d::kZAxis);
  OdGeVector3d upDir(OdGeVector3d::kYAxis);
  pView->setView(ptVpCenter + viewDir, ptVpCenter, 
    upDir, dVpWidth, dVpHeight);  // copy current view settings

  pView->setViewport(gsRect);
  pView->setViewportBorderVisibility(false);

  pView->add(&drawImage, 0);

  pDevice->setBackgroundColor(ODRGB(0,0,0));
  
  OdUInt32 numCols = pImage->numColors();
  OdUInt8Array imgPalette;
  imgPalette.resize(pImage->paletteDataSize());

  if(numCols != 2)
  {
    pImage->paletteData(imgPalette.asArrayPtr());
  }
  else
  {
    // for bitonal images, palette contains current bg color 
    // and color that is currently set in traits
    ODCOLORREF* pCol = (ODCOLORREF*)imgPalette.asArrayPtr();
    pCol[0] = device()->getColor(0);
    OdCmEntityColor col = effectiveTraits().trueColor();
    if(col.isByColor())
      pCol[1] = ODTOCOLORREF(col);
    else
      pCol[1] = device()->getColor(col.colorIndex());
  }
  if(numCols)
    pDevice->setLogicalPalette((const ODCOLORREF*)imgPalette.asArrayPtr(), numCols);
  else // NB "getPaletteColorNumbers" is not correct name
    pDevice->setLogicalPalette(device()->getPalette(), device()->getPaletteColorNumbers());

  pDevice->onSize(gsRect);

  pDevice->properties()->putAt("BitPerPixel", OdRxVariantValue(OdUInt32(colorDepth)));
  
  pDevice->update();
  
  OdGiRasterImagePtr pRaster = pDevice->properties()->getAt("RasterImage");
  
  // Creating the mask
  OdGiRasterImagePtr pMask;
  PolygonDrawable drawPolygon(worldBound.asArrayPtr(), worldBound.size());

  // Device for mask
  OdGsDevicePtr pMaskDevice = odSystemServices()->gsBitmapDevice();
  pMaskDevice->setUserGiContext(userGiContext());
  pMaskDevice->addView(pView);
  pMaskDevice->setBackgroundColor(ODRGB(0,0,0));
  pMaskDevice->onSize(gsRect);
  
  if(bIsTransp)
  {
    pMaskDevice->properties()->putAt("BitPerPixel", OdRxVariantValue(OdUInt32(1)));
    imgPalette.resize(2*sizeof(ODCOLORREF));
    ((ODCOLORREF*)imgPalette.asArrayPtr())[0] = 0;
    ((ODCOLORREF*)imgPalette.asArrayPtr())[1] = ODRGB(255, 255, 255); // let it be while, for example
    pMaskDevice->setLogicalPalette((const ODCOLORREF*)imgPalette.asArrayPtr(), 2);
    pView->erase(&drawImage);
    pView->add(&drawPolygon, 0);
    pMaskDevice->update();
    pMask = pMaskDevice->properties()->getAt("RasterImage");
  }

  device()->dc_raster_image(pRaster, pMask, imgExts, transparency);

  pView->eraseAll(); // to avoid pure virtual function call
}

//====================================================================
//
void Od2dExportView::metafileProc(const OdGePoint3d&,
                                  const OdGeVector3d&,
                                  const OdGeVector3d&,
                                  const OdGiMetafile*,
                                  bool,
                                  bool)
{
  setCurrTraits();
  if (!isProcessGeom())
  {
    return;
  }
}


//====================================================================
//
void Od2dExportView::setCurrTraits()
{
  const OdGiSubEntityTraitsData& currTraits = effectiveTraits();
  setCurrTraits(currTraits);
}

void Od2dExportView::setCurrTraits(const OdGiSubEntityTraitsData&)
{
}

//====================================================================
//
bool Od2dExportView::regenAbort() const
{
  // return true here to abort the rendering process
  return false;  
}

//====================================================================
//
void Od2dExportView::beginViewVectorization()
{
  OdGsBaseVectorizeView::beginViewVectorization();
  OdGiGeometrySimplifier::setDrawContext(OdGsBaseVectorizeView::drawContext());
}

//====================================================================
//
void Od2dExportView::endViewVectorization()
{
  OdGsBaseVectorizeView::endViewVectorization();
  OdGiGeometrySimplifier::setDrawContext(0);
}

//====================================================================
//
bool Od2dExportView::isTransparency(OdGeVector3d screenU, OdGeVector3d screenV)
{
  return ((screenU.isParallelTo(OdGeVector3d::kXAxis) 
           && screenV.isParallelTo(OdGeVector3d::kYAxis))
                        ? false
                        : true
         );
}

