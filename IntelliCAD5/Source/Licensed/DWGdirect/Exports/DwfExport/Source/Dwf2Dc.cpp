//////////////////////////////////////////////////////////////////////
//
// DwfWriter.cpp: implementation of the GsDwfFactory, GsDwfDevice, GsDwfView classes.
//
//////////////////////////////////////////////////////////////////////


//#include "whiptk/whip_toolkit.h"
#include "OdaCommon.h"
#include "OdBinaryData.h"
//#include "Entities.h"
//#include "2dExportDevice.h"
#include "DwfDrawObject.h"
#include "2dSupport.h"
#include "DwfSupport.h"
#include "DwfExportImpl.h"
//#include "RxObjectImpl.h"

#include "Ge/GeEllipArc3d.h"
#include "Ge/GeLineSeg3d.h"
#include "Gi/GiRasterImage.h"

//#include "OdaCommon.h"
/*
#include "DwfDrawObject.h"
#include "DwfSupport.h"

#include "RxObjectImpl.h"
#include "StaticRxObject.h"

#include "GiTextStyle.h"
#include "OdFont.h"
#include "DbLayerTableRecord.h"
#include "GeLineSeg2d.h"

#include "DbRasterImage.h"
#include "DbViewport.h"
#include "GsBMPDevice.h"
#include "RxVariantValue.h"
#include "DbDimension.h"
#include "DbFcf.h"

#include "DbDatabase.h"
#include "GiContextForDbDatabase.h"
*/
//#include "DwfExportImpl.h"
//#include "./../source/misc/OdTrueTypeFont.h"

//#include "DbRasterImageDef.h"
//#include "DbRasterImage.h"



const int nFontMultipliedByFactor (1024);     // Some Font's values may range from a value of one (1/1024 
                                              //  of normal) to 65535 (64 times normal). 
const WT_Unsigned_Integer16 nWtAngleMaxValue  (65535);  // The max legal value of angle.
const double dWtAngleMeasureOfDegree (65535/360);       // Measure in 360/65636ths of a degree.

//====================================================================
//

void OdDwfDevice::dc_polyline(OdUInt32 nPts, const OdGePoint2d* pPoints)
{
  push_Url();

  WT_Logical_Point * pWtPoints = new WT_Logical_Point[nPts];

  for (OdUInt32 i = 0; i < nPts; i++)
  {
    RoundPoint(pWtPoints[i], pPoints[i]);
  }

  WT_Polyline line(nPts, pWtPoints, WD_False);
  line.serialize(*getWtFile());

  delete [] pWtPoints;
}


//====================================================================
//
void OdDwfDevice::dc_polygon(OdUInt32 nPoints, const OdGePoint2d* pPoints)
{
  push_Url();

  WT_Logical_Point * pWtPoints = new WT_Logical_Point[nPoints];

  for (OdUInt32 i = 0; i < nPoints; i++)
  {
    RoundPoint(pWtPoints[i], pPoints[i]);
  }

  WT_Polygon polygon(nPoints, pWtPoints, WD_False);

  polygon.serialize(*getWtFile());

  delete [] pWtPoints;
}


//====================================================================
//

void OdDwfDevice::dc_circle(const OdGeEllipArc3d& ellipse)
{
  push_Url();

  OdGePoint3d ptCenter3d(ellipse.center());

  WT_Logical_Point wtCenter;
  RoundPoint(wtCenter, OdGePoint2d(ptCenter3d.x, ptCenter3d.y));

  OdGeVector3d vMajor = ellipse.majorRadius() * ellipse.majorAxis();
  OdGeVector3d vMinor = ellipse.minorRadius() * ellipse.minorAxis();

  WT_Integer32 wt_iMajor = RoundDoubleToWTInteger32(vMajor.length());
  WT_Integer32 wt_iMinor = RoundDoubleToWTInteger32(vMinor.length());

  WT_Unsigned_Integer16 wtStart = 0;
  WT_Unsigned_Integer16 wtEnd = 0;      // Because it is not an Arc. The result is that 'R' opcode 
                                        //  instead of 'Circle' will be written into file
  WT_Unsigned_Integer16 wtTilt = 0;     // The effect of Tilt is to rotate the ellipse counter-clockwise 
                                        //  about its center point in 360/65,536ths of a degree.
  WT_Integer16 wt_iValue = RoundDoubleToWTInteger16(
    OdaToDegree( OdGeVector3d::kXAxis.angleTo( ellipse.majorAxis(), OdGeVector3d::kZAxis ) ) 
                                * dWtAngleMeasureOfDegree );
  wtTilt = (wt_iValue < nWtAngleMaxValue) ? wt_iValue : nWtAngleMaxValue;

  WT_Outline_Ellipse wtEllipse(wtCenter, wt_iMajor, wt_iMinor, wtStart, wtEnd, wtTilt);

  wtEllipse.serialize(*getWtFile());
}


void OdDwfDevice::dc_ellipse(const OdGeEllipArc3d& ellipse)
{
  push_Url();

  OdGePoint3d ptCenter3d(ellipse.center());

  WT_Logical_Point wtCenter;
  RoundPoint(wtCenter, OdGePoint2d(ptCenter3d.x, ptCenter3d.y));

  OdGeVector3d vMajor = ellipse.majorRadius() * ellipse.majorAxis();
  OdGeVector3d vMinor = ellipse.minorRadius() * ellipse.minorAxis();

  WT_Integer32 wt_iMajor = RoundDoubleToWTInteger32(vMajor.length());
  WT_Integer32 wt_iMinor = RoundDoubleToWTInteger32(vMinor.length());

  WT_Unsigned_Integer16 wt_iValue 
              = RoundDoubleToWTInteger16(
                            OdaToDegree( OdGeVector3d::kXAxis.angleTo( ellipse.majorAxis(), OdGeVector3d::kZAxis )) 
                            * dWtAngleMeasureOfDegree );
                                        // The effect of Tilt is to rotate the ellipse counter-clockwise 
                                        //  about its center point in 360/65,536ths of a degree.
  WT_Unsigned_Integer16 wtTilt = (wt_iValue < nWtAngleMaxValue) ? wt_iValue : nWtAngleMaxValue;

  bool bArc(false);                     // in order to keep Arc as Arc even at very small angular value
  if (ellipse.endAng() - ellipse.startAng())
  {
    bArc = true;
  }

  double dStart = OdaToDegree( ellipse.startAng() );
  if (dStart > 360)
  {
    dStart -= 360;
  }
  double dEnd = OdaToDegree( ellipse.endAng() );
  if (dEnd > 360)
  {
    dEnd -= 360;
  }

  wt_iValue = RoundDoubleToWTInteger16( dStart * dWtAngleMeasureOfDegree );
  WT_Unsigned_Integer16 wtStart = (wt_iValue < nWtAngleMaxValue) ? wt_iValue : nWtAngleMaxValue;

  wt_iValue = RoundDoubleToWTInteger16( dEnd * dWtAngleMeasureOfDegree );
  WT_Unsigned_Integer16 wtEnd = (wt_iValue < nWtAngleMaxValue) ? wt_iValue : nWtAngleMaxValue;

  if (bArc && OdZero(wtEnd - wtStart) && !ellipse.isClosed())
  {
    wtEnd += 1;   // CircleArc should be on its own
  }

  WT_Outline_Ellipse wtEllipse(wtCenter, wt_iMajor, wt_iMinor, wtStart, wtEnd, wtTilt);

  wtEllipse.serialize(*getWtFile());
}


//====================================================================
//
void OdDwfDevice::dc_lineWeight(WT_Integer32 lineWeight/*OdDb::LineWeight lineWeight*/)
{
//  if (IsNonprocessRun())  return;

  //m_wtFile->desired_rendition().line_weight() = getWtLineWeight(lineWeight);
  getWtFile()->desired_rendition().line_weight() = lineWeight;
}


//====================================================================
//
void OdDwfDevice::dc_layer(int layer_num, const OdChar * layer_name)
{

  WT_Layer Layer(*getWtFile(), layer_num, layer_name);

  //Layer.set_visibility(!pLayer->m_Invisible);
  getWtFile()->desired_rendition().layer() = Layer;
}

//====================================================================
//
void OdDwfDevice::dc_color(ODCOLORREF rgb)
{
  int colorIndex;
  bool bIndex = getExportImpl()->getWtColorIndex(rgb, colorIndex);
  getWtFile()->desired_rendition().color() 
    = (bIndex) ? WT_Color(colorIndex, getWtFile()->desired_rendition().color_map())
               : WT_Color(ODGETRED(rgb), ODGETGREEN(rgb), ODGETBLUE(rgb));
}

//====================================================================
//
void OdDwfDevice::dc_lineStyle(const LineStyle& lineStyle)
{  
  WT_Line_Style wtLineStyle;

  wtLineStyle.line_join() = lineStyle.join;
  wtLineStyle.line_start_cap() = lineStyle.startCap;
  wtLineStyle.line_end_cap() = lineStyle.endCap;

  getWtFile()->desired_rendition().line_style() = wtLineStyle;
}

//====================================================================
//
void OdDwfDevice::dc_gouraud_polytriangle( const OdGePoint3d* pVertexList,
                                           OdInt32 faceListSize,
                                           const OdInt32* pFaceList,
                                           const OdCmEntityColor* pColors)
{
  push_Url();

  // it is shell with gradient fill
  for(int i = 0; i < faceListSize;)
  {
    OdInt32 numVerts = pFaceList[i++];
    ODA_ASSERT(numVerts == 3);
    WT_Logical_Point vP[3];
    WT_RGBA32        rgba[3];
    for(OdInt32 k = 0; k < 3; k ++)
    {
      OdGePoint3d pt(pVertexList[pFaceList[i]]);
      OdCmEntityColor col(pColors[pFaceList[i]]);
      i ++;

      // converting to...
      RoundPoint(vP[k], OdGePoint2d(pt.x, pt.y));
      rgba[k] = WT_RGBA32(col.red(), col.green(), col.blue(), 255);
    }
    WT_Gouraud_Polytriangle gpt(3, vP, rgba, WD_True);
    dc_gouraud_polytriangle(gpt);
  }
}

void OdDwfDevice::dc_gouraud_polytriangle(const WT_Gouraud_Polytriangle& wtColoredPoly)
{
  push_Url();

  wtColoredPoly.serialize(*getWtFile());
}



////////////////////
// bBitonalTransp - true if transparency is enabled for bitonal images
// in this case rendering may be optimized
////////////////////
void OdDwfDevice::dc_raster_image( OdGiRasterImagePtr pRaster, 
                                   OdGiRasterImagePtr pMask,
                                   const OdGeExtents3d& imgExts,
                                   bool bBitonalTransp )
{
  push_Url();

  // To get the definition of the ith color (the red, green, blue, and alpha components) in the Color Map.
  //
  WT_RGBA32 * pMap = new WT_RGBA32[256];

  OdUInt32 numColors = pRaster->numColors();

  for (OdUInt32 i = 0; i < numColors ; i++)
  {
    WT_RGBA32 wt_rgbaColor;

    ODCOLORREF col = pRaster->color(i);
    pMap[i] = WT_RGBA32(ODGETRED(col), ODGETGREEN(col), 
      ODGETBLUE(col), (i==0 && bBitonalTransp) ? 0 : 255);       // Add new color
  }

  WT_Color_Map wtColorMap(numColors == 2 ? 2 : 256, pMap, *getWtFile());

  if (pMap)
  {
    delete [] pMap;
  }


  // To get a sequence of encoded data that represents the colors of the image pixels
  //
  OdUInt32 imageHeight = pRaster->pixelHeight();
  OdUInt32 imageWidth  = pRaster->pixelWidth();
  // raster->size(imageWidth, imageHeight);        // get Image size in pixels of the obtained Image

  // OdUInt32 numScanByte = pRaster->scanLineSize()*imageHeight;
  /* const OdUInt8 *scanBytes = raster->getScanLines(numScanByte);*/

  /* WT_Byte * pWtData = new WT_Byte[numScanByte];
  memset(pWtData, 0, numScanByte);*/

  OdUInt32 scanlineWidth = pRaster->scanLineSize();

  OdBinaryData scanLineBuf;
  scanLineBuf.resize(scanlineWidth);
  OdUInt8* scanLine = scanLineBuf.asArrayPtr();

  OdUInt8Array alphaScanLineBuf;
  OdUInt32 alphaScLnWdth(0);
  OdUInt8* alphaScanLine = NULL;
  if(pMask.get())
  {
    alphaScLnWdth = pMask->scanLineSize();
    alphaScanLineBuf.resize(alphaScLnWdth);
    alphaScanLine = alphaScanLineBuf.asArrayPtr();
  }

  // get ScanLines by index one by one
  //  Note: imageHeight is the number of ScanLines;
  //        scanlineWidth is the number of bytes in every ScanLine.
  int nColorBits = pRaster->colorDepth();

  bool bRet(true);
  WT_Image::WT_Image_Format imageOutFormat(WT_Image::RGB);

  WT_Color_Map* pWtColorMap = &wtColorMap;

  int outClrDepth(0);
  switch (nColorBits)
  {
  case 1:
    imageOutFormat = WT_Image::Bitonal_Mapped;
    outClrDepth = 1;
    break;

  case 2:
  case 4:
  case 8:
    outClrDepth = 8;
    imageOutFormat = WT_Image::Mapped;
    break;

  case 16:
  case 24:
    outClrDepth = 24;
    imageOutFormat = WT_Image::RGB;
    pWtColorMap = WD_Null;
    break;

  default:
    ODA_ASSERT(false);
    bRet = false;
  }

  if (!bRet)
    throw OdError(eInvalidInput);

  if(pMask.get()) // transparency will be need
  {
    outClrDepth = 32;
    imageOutFormat = WT_Image::RGBA;
    pWtColorMap = WD_Null;
    if(pMask->colorDepth() != 1)
      throw OdError(eInvalidInput);
  }

  OdUInt8Array palette;
  palette.resize(pRaster->paletteDataSize());
  OdUInt8* pPal = palette.asArrayPtr();
  pRaster->paletteData(pPal);

  OdUInt32 outScLnSz = outClrDepth * imageWidth / 8;
  if(outClrDepth * imageWidth % 8)
    outScLnSz ++;
  OdUInt8Array outScLns;
  outScLns.resize(outScLnSz * imageHeight);
  memset(outScLns.asArrayPtr(), outScLnSz * imageHeight, 0);
  for (OdUInt32 indexHeight = 0; indexHeight < imageHeight; indexHeight++)
  {
    pRaster->scanLines(scanLine, imageHeight-indexHeight-1);
    if(pMask.get())
      pMask->scanLines(alphaScanLine, imageHeight-indexHeight-1);
    OdUInt8* pDestCurScLn = outScLns.asArrayPtr() + outScLnSz*indexHeight;

    for (OdUInt32 i = 0; i < imageWidth; i ++)
    {
      OdUInt32 index(0);
      OdUInt8 red(0), green(0), blue(0), alpha(255);
      
      const OdUInt8* ptr = scanLine + i * nColorBits / 8;
      if(nColorBits < 16)
      {
        index = 
          (ptr[0] >> 
          (
          ((8 / nColorBits) - 1 - (i % (8 / nColorBits)))
          * nColorBits
          )
          ) 
          & 
          ((1 << nColorBits) - 1);
      }
      else if(nColorBits == 16)
      {
        OdUInt32 val = *((OdUInt32*)ptr);
        blue  = (OdUInt8)(( val        & 0x1f)<<3);
        green = (OdUInt8)(((val >> 5)  & 0x1f)<<3);
        red   = (OdUInt8)(((val >> 10) & 0x1f)<<3);
      }
      else if(nColorBits == 24)
      {
        red   = ptr[2];
        green = ptr[1];
        blue  = ptr[0];
      }

      if((imageOutFormat == WT_Image::RGB || imageOutFormat == WT_Image::RGBA) && 
         (nColorBits < 16))
      {
        red   = pPal[index*4+2];
        green = pPal[index*4+1];
        blue  = pPal[index*4];
      }

      if(imageOutFormat == WT_Image::Mapped || imageOutFormat == WT_Image::Bitonal_Mapped)
      {
        OdUInt8* dstPtr = pDestCurScLn + i*outClrDepth/8;
        OdUInt8 bitPerCol = (OdUInt8)(8/outClrDepth);
        dstPtr[0] &= (0xFF - (((1<<outClrDepth)-1) << ((bitPerCol-1-i%bitPerCol)*outClrDepth)));
        dstPtr[0] |= index << ((bitPerCol-1-i%bitPerCol)*outClrDepth);
      }
      else
      {
        OdUInt8* dstPtr = pDestCurScLn + i*outClrDepth/8;
        dstPtr[0] = red;
        dstPtr[1] = green;
        dstPtr[2] = blue;
        if(imageOutFormat == WT_Image::RGBA)
        {
          const OdUInt8* ptr = alphaScanLine + i / 8;
          alpha = (OdUInt8)(((ptr[0] >> (7 - i%8)) & 1)*255);
          dstPtr[3] = alpha;
        }
      }
    }

    // temporary implementation
    /* ODA_ASSERT(nColorBits == 24);
    for (OdUInt32 i = 0; i < imageWidth; i ++)
    {
      // to convert BGR to RGB
      ODA_ASSERT(j <= numScanByte);
      pWtData[j++]  = scanLine[i*3 + 2];
      pWtData[j++]  = scanLine[i*3 + 1];
      pWtData[j++]  = scanLine[i*3];
    }*/

    /*
    if (nColorBits == orgRGB)
    {
      for (OdUInt32 i = 0; i < imageWidth; i ++)
      {
        // to convert BGR to RGB
        ODA_ASSERT(j <= numScanByte);
        pWtData[j++]  = scanLine[i*3 + 2];
        pWtData[j++]  = scanLine[i*3 + 1];
        pWtData[j++]  = scanLine[i*3];
      }
    }
    else if (nColorBits == orgBGRA)
    {
      for (OdUInt32 i = 0; i < imageWidth; i ++)
      {
        // to convert BGRA to RGBA
        pWtData[j++]  = scanLine[i*4 + 2];
        pWtData[j++]  = scanLine[i*4 + 1];
        pWtData[j++]  = scanLine[i*4];
        pWtData[j++]  = scanLine[i*4 + 3];
      }
    }
    else
    {
      for (OdUInt32 i = 0; i < scanlineWidth; i++)
      {
        pWtData[j++] = scanLine[i];
      }
    }
    */
  }

  WT_Logical_Point wtMin;
  WT_Logical_Point wtMax;
  RoundPoint(wtMin, imgExts.minPoint().convert2d());
  RoundPoint(wtMax, imgExts.maxPoint().convert2d());

  WT_Image wtImage( (OdUInt16)imageHeight,        // The number of columns in the pixel array
                    (OdUInt16)imageWidth,         // The number of rows in the pixel array
                    imageOutFormat,               // format,
                    0,                            // identifier,
                    pWtColorMap, //WD_Null, //&wtColorMap,                  // color_map,
                    imageHeight * outScLnSz,      // The length (in bytes) of the Data sequence that follows,
                    outScLns.asArrayPtr(),        // data,
                    wtMin,                        // min_corner,
                    wtMax,                        // max_corner,
                    WD_False                      // copy
                  );

  wtImage.serialize(*getWtFile());
}


//====================================================================
//
void OdDwfDevice::dc_text(const WT_Text& wtText)
{
  push_Url();

  wtText.serialize( *getWtFile() );
}


//====================================================================
//
void OdDwfDevice::dc_font(const WT_Font& wtFont)
{
  getWtFile()->desired_rendition().font() = wtFont;
}

//====================================================================
//
void OdDwfDevice::dc_url_item(WT_URL_Item& url_item)
{
  getWtFile()->desired_rendition().url().add_url_optimized( url_item, *getWtFile());
}

void OdDwfDevice::dc_url_clear()
{
  getWtFile()->desired_rendition().url().clear();
}

void OdDwfDevice::dc_urls_remove_all()
{
  getWtFile()->desired_rendition().url_lookup_list().remove_all();
}

void OdDwfDevice::dc_layers_remove_all()
{
  getWtFile()->layer_list().remove_all();
}

