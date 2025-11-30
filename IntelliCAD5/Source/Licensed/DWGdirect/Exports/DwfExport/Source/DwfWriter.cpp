//////////////////////////////////////////////////////////////////////
//
// DwfWriter.cpp: implementation of the GsDwfFactory, GsDwfDevice, GsDwfView classes.
//
//////////////////////////////////////////////////////////////////////


//#include "whiptk/whip_toolkit.h"
#include "OdaCommon.h"
//#include "Entities.h"
//#include "2dExportDevice.h"
#include "DwfDrawObject.h"
#include "2dSupport.h"
#include "DwfSupport.h"
#include "DwfExportImpl.h"
//#include "RxObjectImpl.h"

#include "GeEllipArc3d.h"
#include "GeLineSeg3d.h"

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
const int nWtAngleMaxValue        (65535);    // The max legal value of angle.
const double dWtAngleMeasureOfDegree (65535/360); // Measure in 360/65636ths of a degree.

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
  WT_Unsigned_Integer16 wtEnd = nWtAngleMaxValue;
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
    return;
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
                                           const OdCmEntityColor* pColors )
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
void OdDwfDevice::dc_raster_image( const OdGiRasterImagePtr& raster, 
                                   int nColorBits, 
                                   OdGePoint3dArray& arrayExtents)
{
  push_Url();

  // To get the definition of the ith color (the red, green, blue, and alpha components) in the Color Map.
  //
  WT_RGBA32 * pMap = new WT_RGBA32[256];

  OdUInt32 numColors = raster->numColors();
  OdUInt8   red;
  OdUInt8   green;
  OdUInt8   blue;

  for (OdUInt32 i = 0; i < numColors ; i++)
  {
    WT_RGBA32 wt_rgbaColor;

    raster->getColor(i, red, green, blue);
    pMap[i] = WT_RGBA32(red, green, blue, 255);       // Add new color
  }

  WT_Color_Map wtColorMap(numColors, pMap, *getWtFile());

  if (pMap)
  {
    delete [] pMap;
  }


  // To get a sequence of encoded data that represents the colors of the image pixels
  //
  OdUInt32 imageHeight;
  OdUInt32 imageWidth;
  raster->size(imageWidth, imageHeight);        // get Image size in pixels of the obtained Image

  OdUInt32 numScanByte;
  const OdUInt8 *scanBytes = raster->getScanLines(numScanByte);

  WT_Byte * pWtData = new WT_Byte[numScanByte];
  memset(pWtData, 0, numScanByte);

  const OdUInt8* scanLine;

  // get ScanLines by index one by one
  //  Note: imageHeight is the number of ScanLines;
  //        scanlineWidth is the number of bytes in every ScanLine.

  OdUInt32 scanlineWidth = raster->getScanLineWidth();

  for (OdUInt32 indexHeight = 0, j = 0; indexHeight < imageHeight; indexHeight++)
  {
    scanLine = raster->getScanLine(indexHeight);

    if (nColorBits == orgRGB)
    {
      for (OdUInt32 i = 0; i < scanlineWidth; i += 3)
      {
        // to convert BGR to RGB
        pWtData[j++]  = scanLine[i + 2];
        pWtData[j++]  = scanLine[i + 1];
        pWtData[j++]  = scanLine[i];
      }
    }
    else if (nColorBits == orgBGRA)
    {
      for (OdUInt32 i = 0; i < scanlineWidth; i += 4)
      {
        // to convert BGRA to RGBA
        pWtData[j++]  = scanLine[i + 2];
        pWtData[j++]  = scanLine[i + 1];
        pWtData[j++]  = scanLine[i];
        pWtData[j++]  = scanLine[i + 3];
      }
    }
    else
    {
      for (OdUInt32 i = 0; i < scanlineWidth; i++)
      {
        pWtData[j++] = scanLine[i];
      }
    }
  }

  // To get logical coordinates indicating 
  //  the lower-left & upper-right corner of the image to be drawn  
  OdGePoint3d minImageExtent;
  OdGePoint3d maxImageExtent;

  calculateExtents(minImageExtent, maxImageExtent, arrayExtents/*, pView->objectToDeviceMatrix()*/);

  WT_Logical_Point wtMin;
  WT_Logical_Point wtMax;
  RoundPoint(wtMin, OdGePoint2d(minImageExtent.x, minImageExtent.y));
  RoundPoint(wtMax, OdGePoint2d(maxImageExtent.x, maxImageExtent.y));

  bool bRet(true);
  WT_Image::WT_Image_Format imageFormat(WT_Image::RGBA);  
                                        // A description of how data in the Data array is to be interpreted.

  nColorBits = raster->colorDepth();    // retrieves the number of bits per pixel on the obtained image

  WT_Color_Map* pWtColorMap = &wtColorMap;

  switch (nColorBits)
  {
  case orgBitonal:
    imageFormat = WT_Image::Bitonal_Mapped;
    break;

  case orgMapped:
    imageFormat = WT_Image::Mapped;
    break;

  case orgRGB:
    imageFormat = WT_Image::RGB;
    pWtColorMap = WD_Null;
    break;

  case orgBGRA:
    imageFormat = WT_Image::RGBA;
    pWtColorMap = WD_Null;
    break;

  default:
    ODA_ASSERT(false);
    bRet = false;
  }

  if (!bRet)
  {
    return;
  }
  /*
    enum WT_Image_Format
    {
        Bitonal_Mapped      = WD_IMAGE_BITONAL_MAPPED_EXT_OPCODE,
        Group3X_Mapped      = WD_IMAGE_GROUP3X_MAPPED_EXT_OPCODE,
        Indexed             = WD_IMAGE_INDEXED_EXT_OPCODE,
        Mapped              = WD_IMAGE_MAPPED_EXT_OPCODE,
        RGB                 = WD_IMAGE_RGB_EXT_OPCODE,
        RGBA                = WD_IMAGE_RGBA_EXT_OPCODE,
        JPEG                = WD_IMAGE_JPEG_EXT_OPCODE,
    };
    */

  WT_Image wtImage( (OdUInt16)imageHeight,        // The number of columns in the pixel array
                    (OdUInt16)imageWidth,         // The number of rows in the pixel array
                    imageFormat,                  // format,
                    0,                            // identifier,
                    pWtColorMap, //WD_Null, //&wtColorMap,                  // color_map,
                    imageHeight * scanlineWidth,  // The length (in bytes) of the Data sequence that follows,
                    pWtData,                      // data,
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

