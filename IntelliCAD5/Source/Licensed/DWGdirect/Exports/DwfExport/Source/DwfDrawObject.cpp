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
//
// DwfDrawObject.cpp: implementation of the GsDwfFactory, OdDwfDevice, OdDwfView classes.
//
//////////////////////////////////////////////////////////////////////


#include "OdaCommon.h"

#include "2dExportDevice.h"
#include "2dSupport.h"
#include "DwfDrawObject.h"
#include "DwfSupport.h"
#include "DwfExportImpl.h"

#include "RxObjectImpl.h"
#include "GeCircArc3d.h"
#include "ColorMapping.h"


#include "GiTextStyle.h"
//#include "OdFont.h"
#include "DbLayerTableRecord.h"
#include "GeLineSeg2d.h"

#include "DbRasterImage.h"
#include "DbViewport.h"
#include "GsBMPDevice.h"
#include "RxVariantValue.h"
#include "DbDimension.h"
#include "DbFcf.h"

#include "GeEllipArc3d.h"
#include "GeLineSeg3d.h"
#include "DbDatabase.h"
#include "DbText.h"
#include "DbMText.h"

#include "StaticRxObject.h"
#include "GiContextForDbDatabase.h"

#include "DbHyperlink.h"


#define STL_USING_MAP
#define STL_USING_VECTOR
#include "OdaSTL.h"


//////////////////////////////////////////////////////////////////////////////////

typedef std::map<OdString, int> mapLayers;
static mapLayers mapLayer;

typedef std::map<OdString, int> mapURLs;
static mapURLs mapURL;

static bool g_bUrlTrigger;                  // if true - url list was changed

typedef std::vector<DwfUrl> vectorDwfUrls;
static vectorDwfUrls g_UrlList;            // List of urls witch are in force right now
const int iDwfUrlOff = (-1);

//static OdGiDrawable* p_Drawable;
static std::vector<OdGiDrawable*> g_Drawables;

static std::vector<bool> g_bNoSkipTextOpForShx;   // To skip text operand in case that shx text is a part of
                                                  //  entity as dimension, tolerance or linetype property & etc.
const int nFontMultipliedByFactor (1024);         // Some Font's values may range from a value of one (1/1024 
                                                  //  of normal) to 65535 (64 times normal). 
const int    nWtAngleMaxValue  (65535);           // The max legal value of angle.
const double dWtAngleMeasureOfDegree (65535/360); // Measure in 360/65636ths of a degree.


//////////////////////////////////////////////////////////////////////////////////
// OdDwfDevice
//////////////////////////////////////////////////////////////////////////////////

OdDwfDevice::OdDwfDevice()
  : m_bKindOfRun(Main_run)
  , m_dwf(NULL)
{
}

//====================================================================
//
//OdGsDevicePtr OdDwfDevice::createObject(DeviceType type)
OdGsDevicePtr OdDwfDevice::createObject()
{
  //OdDwfDevice* pMyDev;
  OdGsDevicePtr pRes = OdRxObjectImpl<OdDwfDevice, OdGsDevice>::createObject();

  //pMyDev->m_type = type;

  return pRes;
}


//====================================================================
//
OdGsViewPtr OdDwfDevice::createView(const OdGsClientViewInfo* , bool)
{
  OdGsViewPtr pView = OdDwfView::createObject();
  OdDwfView* pMyView = static_cast<OdDwfView*>(pView.get());

  pMyView->screenOutput().setDestGeometry(*pMyView);

  return pMyView;
}


//====================================================================
//
void OdDwfDevice::setContext(CDwfExportImpl* dwf)
{
  m_dwf = dwf;

  // to init layout environment
  mapLayer.clear();
  mapURL.clear();
  g_UrlList.clear();

  DwfUrl dwfUrl;
  dwfUrl.iUrlNum = iDwfUrlOff;
  g_UrlList.push_back(dwfUrl);      // "URL Link" attribute is inactive


  g_bUrlTrigger = false;            // "URL Link" attribute is inactive 
                                    //  (no URL is attached to subsequent geometry)

  dc_urls_remove_all();
  dc_layers_remove_all();
}


//====================================================================
//
WT_File* OdDwfDevice::getWtFile()
{
  return getExportImpl()->getWtFile();
}

//====================================================================
//
OdDbDatabase* OdDwfDevice::getDb()
{
  return getExportImpl()->getParams().pDb;
}

//====================================================================
//
const ODCOLORREF* OdDwfDevice::getPalette() const
{
  return m_dwf->getPalette();
}


//====================================================================
//
void OdDwfDevice::update(OdGsDCRect* pUpdatedRect, KindOfRun bKindOfRun)
{
  // to begin paint

  setRun(bKindOfRun);
  getExportImpl()->getDevice()->update(pUpdatedRect);
}


//====================================================================
//
void OdDwfDevice::onSize(const OdGsDCRect& outputRect)
{
  Od2dExportDevice::onSize(outputRect);
}


int OdDwfDevice::getSupportedColorBits(int nColorBits, bool bTransparency) const
{
  return ((bTransparency) 
              ? Od2dExportDevice::orgBGRA             // always 32 bits in case of transparency 
              : Od2dExportDevice::getSupportedColorBits(nColorBits, bTransparency)
         );
}


void OdDwfDevice::push_Url()
{
  if (g_bUrlTrigger)                       // Is url trigger on? 
  {                                   //  in this case to out the current url list
                                      //  Note: the current url list can be empty
      set_Urls();
      g_bUrlTrigger = false;
  }
}

void OdDwfDevice::set_Urls()
{
    dc_url_clear();       // clears the current url list

  // Iterator is used to loop through the vector.
  vectorDwfUrls::const_iterator theIterator;

  for ( theIterator = g_UrlList.begin(); 
        theIterator != g_UrlList.end();
        theIterator++ )
  {
    DwfUrl dwfUrl = *theIterator;

    if (dwfUrl.iUrlNum > 0)
    {
      WT_URL_Item wtUrlItem( dwfUrl.iUrlNum, dwfUrl.sName, dwfUrl.sDescription );
      dc_url_item( wtUrlItem );
    }                                      
  }
}



//////////////////////////////////////////////////////////////////////////////////
// OdDwfView
//////////////////////////////////////////////////////////////////////////////////

//====================================================================
//
void OdDwfView::init()
{

  // Init GsDwfView class members ...
  m_curRGB = device()->getBackgroundColor();

  m_lineWeight = OdDb::kLnWt000;
  m_dThickness = 0.0;
  m_bLinetypeContinuous = true;

  m_lineStyle.join     = WT_Line_Style::Undefined_Joinstyle;
  m_lineStyle.startCap = WT_Line_Style::Undefined_Capstyle;
  m_lineStyle.endCap   = WT_Line_Style::Undefined_Capstyle;

  m_bTurnVisibilityOn = true;

  // Init BaseView ...

  setViewportBorderVisibility(false);

  if (device() != NULL)
  {
/*
    // see comments to nMaxPointsInPolygon in DwfExport.h
    unsigned long nMaxPointsInPolygon = device()->getExportImpl()->getParams().nMaxPointsInPolygon;
    if (nMaxPointsInPolygon >= 3 && nMaxPointsInPolygon <= 0x7FFFFFFF)
    {
      m_nMaxFacetVerteces = nMaxPointsInPolygon;  // Max number of points in polygon 
    }
    else
    {
      m_nMaxFacetVerteces = 3;                    // Polygons will be triangulated to tristrips
    }
*/
  }
}


//====================================================================
//
inline OdGeMatrix3d normalizeCs(const OdGeMatrix3d& xCs)
{
  OdGeMatrix3d xRes;
  OdGeVector3d xAxiz = xCs.getCsXAxis(), yAxis =xCs.getCsYAxis();
  xRes.setCoordSystem(
    xCs.getCsOrigin(),
    xAxiz,
    yAxis,
    xAxiz.crossProduct(yAxis).normal(OdGeTol(10e-100)) * yAxis.length());
  return xRes;
}

void OdDwfView::update()
{
  OdGeMatrix3d xToDc = eyeToScreenMatrix();

  setEyeToOutputTransform(xToDc);

  OdGsDCPoint min, max;
  screenRect(min, max);

  OdGeVector3d zAxis = OdGeVector3d::kZAxis;
  OdGeVector3d dcFront(0.,0.,frontClip());
  OdGeVector3d dcBack(0.,0.,backClip());
  dcFront.transformBy(xToDc);
  dcBack.transformBy(xToDc);
  zAxis.transformBy(xToDc);

  OdGeExtents2d exts;
  exts.addPoint(OdGePoint2d(min.x, min.y));
  exts.addPoint(OdGePoint2d(max.x, max.y));

  if (zAxis.z > 0.)
  {
    m_pSpatialFilter->set(exts, isBackClipped(), dcBack.z, isFrontClipped(), dcFront.z);

  }
  else
  {
    m_pSpatialFilter->set(exts, isBackClipped(), dcBack.z, isFrontClipped(), dcFront.z);
  }
  m_pXYProjector->setTransform(OdGeMatrix3d::projection(OdGePlane::kXYPlane, OdGeVector3d::kZAxis));

  m_pSpatialFilter->setDrawContext(OdGiBaseVectorizer::drawContext());

  init();

  g_bNoSkipTextOpForShx.clear();
  g_bNoSkipTextOpForShx.push_back(true);

  g_Drawables.clear();
  g_Drawables.push_back(NULL);
  
  setMinViewClip( OdGePoint2d(min.x, min.y) );
  setMaxViewClip( OdGePoint2d(max.x, max.y) );

  putViewport();                        // Sets the active viewport for clipping of drawing 
                                        //  in order to get the same result as on the screen
  setScale();

  Od2dExportView::update();

  g_bNoSkipTextOpForShx.pop_back();
  ODA_ASSERT(  g_bNoSkipTextOpForShx.empty() );

  g_Drawables.pop_back();
  ODA_ASSERT(  g_Drawables.empty() );
}
/*
{
  //device()->setBeingUpdated(this);

  // to set matrix to transfom into device coo
  OdGeMatrix3d xToDc = OdGeMatrix3d::projection(OdGePlane::kXYPlane, OdGeVector3d::kZAxis) *
    ::normalizeCs(screenMatrix() * projectionMatrix());
  setEyeToOutputTransform(xToDc);

  init();

  g_bNoSkipTextOpForShx.clear();
  g_bNoSkipTextOpForShx.push_back(true);
  
  OdGsDCPoint min, max;
  screenRect(min, max);

  setMinViewClip( OdGePoint2d(min.x, min.y) );
  setMaxViewClip( OdGePoint2d(max.x, max.y) );

  putViewport();                        // Sets the active viewport for clipping of drawing 
                                        //  in order to get the same result as on the screen
  setScale();

  Od2dExportView::update();

  g_bNoSkipTextOpForShx.pop_back();
  ODA_ASSERT(  g_bNoSkipTextOpForShx.empty() );
}
*/

//====================================================================
//
void OdDwfView::setCurrTraits(const OdGiSubEntityTraitsData& currTraits)
{
  // color
  if(currTraits.trueColor().isByColor())
  {
    set_Color(ODTOCOLORREF(currTraits.trueColor()));
  }
  else
  {
    set_ColorIndex(currTraits.color());
  }

  if (device()->IsMainRun())
  {
    // LineWeight
    set_LineWeight(currTraits.lineWeight());

    // LineStyle
    LineStyle lineStyle;
    lineStyle.join     = WT_Line_Style::Round_Join;
    lineStyle.startCap = WT_Line_Style::Round_Cap;
    lineStyle.endCap   = WT_Line_Style::Round_Cap;

    set_LineStyle(lineStyle);

    set_Layer(currTraits.layer());

    m_dThickness = currTraits.thickness();

    m_bLinetypeContinuous = isEffectiveLinetypeContinuous();
  }
}


//====================================================================
//
bool OdDwfView::isProcessGeom()
{
  
  return ((device()->IsMainRun()) ? true : false);
}

//==========================================================================================
//***************************************************************************
// getWtLineWeight()
//
//  To translate the OdDb lineweight (in 100ths of a millimeter) to WT_Line_Weight code
//***************************************************************************
//int GsDwfDevice::getWtLineWeight(const OdDb::LineWeight lineWeight)
double getWtLineWeight(const OdDb::LineWeight lineWeight)
{
/*
  switch (lineWeight)
  {
  case OdDb::kLnWtByLayer:      // -1,
  case OdDb::kLnWtByBlock:      // -2,
  case OdDb::kLnWtByLwDefault:  // -3
    ODA_ASSERT(false);
    return 0;
  };

  draw_line_weight(lineWeight);
  return currLineWeightPix();
*/

  double wtLineWeight = 0;

  switch (lineWeight)
  {
  case OdDb::kLnWt000:      wtLineWeight = 0;       break;
  case OdDb::kLnWt005:      wtLineWeight = 0.05;    break;
  case OdDb::kLnWt009:      wtLineWeight = 0.09;    break;
  case OdDb::kLnWt013:      wtLineWeight = 0.13;    break;
  case OdDb::kLnWt015:      wtLineWeight = 0.15;    break;
  case OdDb::kLnWt018:      wtLineWeight = 0.18;    break;
  case OdDb::kLnWt020:      wtLineWeight = 0.20;    break;
  case OdDb::kLnWt025:      wtLineWeight = 0.25;    break;
  case OdDb::kLnWt030:      wtLineWeight = 0.30;    break;
  case OdDb::kLnWt035:      wtLineWeight = 0.35;    break;
  case OdDb::kLnWt040:      wtLineWeight = 0.40;    break;
  case OdDb::kLnWt050:      wtLineWeight = 0.50;    break;
  case OdDb::kLnWt053:      wtLineWeight = 0.53;    break;
  case OdDb::kLnWt060:      wtLineWeight = 0.60;    break;
  case OdDb::kLnWt070:      wtLineWeight = 0.70;    break;
  case OdDb::kLnWt080:      wtLineWeight = 0.80;    break;
  case OdDb::kLnWt090:      wtLineWeight = 0.90;    break;
  case OdDb::kLnWt100:      wtLineWeight = 1.00;    break;
  case OdDb::kLnWt106:      wtLineWeight = 1.06;    break;
  case OdDb::kLnWt120:      wtLineWeight = 1.20;    break;
  case OdDb::kLnWt140:      wtLineWeight = 1.40;    break;
  case OdDb::kLnWt158:      wtLineWeight = 1.58;    break;
  case OdDb::kLnWt200:      wtLineWeight = 2.00;    break;
  case OdDb::kLnWt211:      wtLineWeight = 2.11;    break;

  case OdDb::kLnWtByLayer:      // -1,
  case OdDb::kLnWtByBlock:      // -2,
  case OdDb::kLnWtByLwDefault:  // -3
    ODA_ASSERT(false);
  default:
    wtLineWeight = 0;
    break;
  };

  return wtLineWeight;

}

void OdDwfView::set_LineWeight(OdDb::LineWeight lineWeight)
{
  if (m_lineWeight != lineWeight)
  {
    // If the lineWeight was changed
    m_lineWeight = lineWeight;

    double dLineWeight = getWtLineWeight( lineWeight );     // enum to mm
    dLineWeight *= m_dScale;                                // mm to logical coordinates 
    device()->dc_lineWeight( RoundDoubleToWTInteger32(dLineWeight) );
  }
}


//====================================================================
//
void OdDwfView::set_Color(ODCOLORREF rgb)
{
  if (m_curRGB != rgb)
  {
    // If the colour was changed
    m_curRGB = rgb;

    if (device()->IsNonprocessRun())
    {
      device()->getExportImpl()->colorPresetting(rgb);
    }
    else
    {
      device()->dc_color(rgb);
    }
  }
}


//====================================================================
//
void OdDwfView::set_ColorIndex(OdUInt16 colorIndex)
{
  set_Color(device()->getColor(colorIndex));
}

//====================================================================
//
void OdDwfView::set_LineStyle(const LineStyle& lineStyle)
{
/*
WT_Line_Pattern::WT_Pattern_ID ePattern;
ePattern = WT_Line_Pattern::ISO_Dash;
*(device()->m_wtFile).desired_rendition().line_pattern() = ePattern;
*/

  if (   m_lineStyle.join     != lineStyle.join
      || m_lineStyle.startCap != lineStyle.startCap
      || m_lineStyle.endCap   != lineStyle.endCap
     )
  {
    m_lineStyle.join     = lineStyle.join;
    m_lineStyle.startCap = lineStyle.startCap;
    m_lineStyle.endCap   = lineStyle.endCap;

    device()->dc_lineStyle(lineStyle);
  }
}

/*
WT_Result LinePattern()
{
  WT_File whip_file = *(device()->m_wtFile);

    //Store the current line pattern.
    WT_Line_Pattern previous_pattern = whip_file.desired_rendition().line_pattern();
    //Set the line pattern.
    whip_file.desired_rendition().line_pattern() = WT_Line_Pattern::ISO_Dash_Triple_Dot;
    //Create a filled drawable (e.g. a polygon).
    int x=0,y=0;
    WT_Logical_Point vP[2];
    vP[0] = WT_Logical_Point(x, y);
    vP[1] = WT_Logical_Point(x+100, y);
    WT_Polyline p (2, vP, WD_True);
    //Serialize it.
    WD_CHECK(p.serialize (whip_file));
    //Restore the previous pattern.
    whip_file.desired_rendition().line_pattern() = previous_pattern;

    return WT_Result::Success;
}
*/


//====================================================================
//
void OdDwfView::set_Layer(OdDbStub* layer)
{
  // The Set Layer function is used to delimit a group of opcodes.

  if (!layer)  return;

  OdDbLayerTableRecordPtr pLayer;
  pLayer = OdDbObjectId(layer).openObject();
  if(pLayer.get())
  {
    OdString strLayerName = pLayer->getName();

    int layer_num;      // The layer number which will be assigned to the geometric primitives 
                        //  that are subsequently created. 
                        //  NOTE: Do not use layer numbers less than 1. 

    mapLayers::const_iterator it;
    it = mapLayer.find( strLayerName );       // If the Layer exists in the map...

    if (it != mapLayer.end())
    {
      layer_num = (*it).second;               // ... to get Layer number by Name
    }
    else
    {
      layer_num = mapLayer.size() + 1;        // Otherwise to set number and to add into the map
      mapLayer[strLayerName] = layer_num;
    }

    ODA_ASSERT(layer_num >= 1);

    if (layer_num < 1) return;

    //Layer.set_visibility(!pLayer->m_Invisible);
    device()->dc_layer(layer_num, strLayerName.c_str());
  }
}


//====================================================================
//
// draw()
//
void OdDwfView::draw(const OdGiDrawable* pDrawable)
{
  ODA_ASSERT( !g_bNoSkipTextOpForShx.empty() );
  bool bNoSkipTextOpForShx = g_bNoSkipTextOpForShx.back();

  ODA_ASSERT( !g_Drawables.empty() );
  bool bPushDrawable(false);

  if (pDrawable->isKindOf( OdDbText::desc() )
    || pDrawable->isKindOf( OdDbMText::desc() )
    )
  {
    // To store the Drawable up to an aftertreatment in textProc()
    //  Note: textProc() for MText can be called once or more with the same pDrawable
    bPushDrawable = true;
  }
  else 
  if (pDrawable->isKindOf( OdDbDimension::desc() ))
  {
    bNoSkipTextOpForShx = false;
  }
  else if (pDrawable->isKindOf( OdDbFcf::desc() ))
  {
    bNoSkipTextOpForShx = false;
  }

  g_bNoSkipTextOpForShx.push_back( bNoSkipTextOpForShx );
  g_Drawables.push_back( (bPushDrawable) ? (OdGiDrawable*)pDrawable : NULL );

  process_Url(pDrawable);       // Checks url data existence

  Od2dExportView::draw(pDrawable);

  postprocess_Url();

  g_bNoSkipTextOpForShx.pop_back();
  g_Drawables.pop_back();
}


//====================================================================
//
// putViewport()
//
// Sets the active viewport for clipping of drawing in order to get the same result as on the screen
//
void OdDwfView::putViewport()
{
  if (device()->IsNonprocessRun())  
  {
    return;
  }

  WT_Contour_Set wtContourSet;

  if (m_nrcContours)  // Is this a contour set 
  {
    // m_nrcContours - the number of contours in the contour set 
    int nTotalPoints = 0; // The total number of points in the contour set (should equal the sum of the counts.)
    int i, v;
    for (i = 0; i < m_nrcContours; i++)
    {
      nTotalPoints += m_nrcCounts[i];   // To add the number of points in the ith contour
    }

    WT_Logical_Point * pWtPoints = new WT_Logical_Point[ nTotalPoints ];  
                                                                // The array of points used by the contours
    WT_Integer32* pContourCounts = new WT_Integer32[ m_nrcContours ];
            // The array of counts (each array item indicates the number of points in the respective contour)

    for (i = 0, v = 0; i < m_nrcContours; i++)
    {
      int nPoints = m_nrcCounts[i]; // The number of points in the ith contour
      for (int j = 0; j < nPoints; j++, v++)
      {
        RoundPoint(pWtPoints[v], OdGePoint2d(m_nrcPoints[v].x, m_nrcPoints[v].y));
      }
      pContourCounts[i] = nPoints;
    }

    // Create the contour set.
    WT_Contour_Set contour_set(*(device()->getWtFile()), m_nrcContours, pContourCounts, 
                               nTotalPoints, pWtPoints, WD_True);

    wtContourSet.set(0xFFFFFFFFU, contour_set.contours(), contour_set.counts(), 
                     contour_set.total_points(), contour_set.points(), WD_True);
    delete [] pWtPoints;
    delete [] pContourCounts;
  }
  else
  {
    WT_Logical_Point wtMin;
    WT_Logical_Point wtMax;

    convViewportToWt(wtMin, wtMax);

    WT_Logical_Point wtPoints[4];

    wtPoints[0].m_x = wtMin.m_x;
    wtPoints[0].m_y = wtMin.m_y;
    wtPoints[1].m_x = wtMax.m_x;
    wtPoints[1].m_y = wtMin.m_y;
    wtPoints[2].m_x = wtMax.m_x;
    wtPoints[2].m_y = wtMax.m_y;
    wtPoints[3].m_x = wtMin.m_x;
    wtPoints[3].m_y = wtMax.m_y;

    WT_Contour_Set contour_set(*(device()->getWtFile()), 4, wtPoints);

    wtContourSet.set(0xFFFFFFFFU, contour_set.contours(), contour_set.counts(), 
                     contour_set.total_points(), contour_set.points(), WD_True);
  }


  WT_Viewport viewPort(*(device()->getWtFile()), "", wtContourSet);

  double xform[16];
  convDeviceMatrixToWt(xform);

  WT_Matrix wtMatrix(xform);

  viewPort.viewport_units().set_application_to_dwf_transform( WT_Matrix(xform) );
  viewPort.viewport_units().set_units( getUnitName( device()->getExportImpl()->getParams().pDb->getINSUNITS() ) );

  device()->getWtFile()->desired_rendition().viewport() = viewPort;
}


//====================================================================
//
void OdDwfView::setScale()
{
  // To define the paper onto which the graphics will be placed.

  double deviceWidth = (device()->getExportImpl()->getParams().xSize > lMaxDwfResolution) ? lMaxDwfResolution : device()->getExportImpl()->getParams().xSize;
  double deviceHeight = (device()->getExportImpl()->getParams().ySize > lMaxDwfResolution) ? lMaxDwfResolution : device()->getExportImpl()->getParams().ySize;

/*  bool landscape = (deviceWidth > deviceHeight);
  double plotHeight(landscape ? 210 : 297);
  double plotWidth(landscape ? 297 : 210);
*/
  double plotHeight, plotWidth;
  device()->getExportImpl()->getPlotPaperSize( plotWidth, plotHeight );

  double scaleX = deviceWidth / plotWidth;
  double scaleY = deviceHeight / plotHeight;

  m_dScale = min (scaleX, scaleY);
}


//====================================================================
//
void OdDwfView::convViewportToWt(WT_Logical_Point& wtMin, WT_Logical_Point& wtMax) const
{
  OdGsDCPoint min, max;
  screenRect(min, max);

  RoundPoint(wtMin, OdGePoint2d(min.x, min.y));
  RoundPoint(wtMax, OdGePoint2d(max.x, max.y));
}


//====================================================================
//
// convDeviceMatrixToWt()
//
//  Converts device matrix for view to Wt matrix
//
void OdDwfView::convDeviceMatrixToWt(double* xform) const
{
  OdGeMatrix3d matrix = screenMatrix() * projectionMatrix() * viewingMatrix();
  matrix.transposeIt();

  for (int i = 0; i < 4; i++)
  {
    for (int j = 0; j < 4; j++)
    {
      xform[i * 4 + j] = matrix(i, j);
    }
  }
}


//====================================================================
//
// view space to normalized device space
//
OdGeMatrix3d OdDwfView::projectionMatrixMy() const
{
  OdGeMatrix3d res;

    res(0,0) = (m_dcUpperRight.x - m_dcLowerLeft.x) / fieldWidth();
    res(1,1) = (m_dcUpperRight.y - m_dcLowerLeft.y) / fieldHeight();
    res(2,2) = 1.0;
    res(0,3) = (m_dcUpperRight.x + m_dcLowerLeft.x) / 2.0;
    res(1,3) = (m_dcUpperRight.y + m_dcLowerLeft.y) / 2.0;
    res(2,3) = 0;  

  return res;
}


//====================================================================
//
void OdDwfView::polylineProc(
                         OdInt32 nPoints, 
                         const OdGePoint3d* pVertexList,
                         const OdGeVector3d* pNormal,
                         const OdGeVector3d* pExtrusion,
                         OdInt32 lBaseSubEntMarker)
{
  g_bNoSkipTextOpForShx.push_back(false); // flag in case of shx text having linetype

  device()->dc_fill(false);                               // Off fill mode

  Od2dExportView::polylineProc(nPoints, pVertexList, pNormal, pExtrusion, lBaseSubEntMarker);

  g_bNoSkipTextOpForShx.pop_back();
}


//====================================================================
//
void OdDwfView::polygonProc(
                            OdInt32 nbPoints,
                            const OdGePoint3d* pVertexList,
                            const OdGeVector3d* pNormal,
                            const OdGeVector3d* pExtrusion)
{
  g_bNoSkipTextOpForShx.push_back(false); // flag in case of shx text having linetype

  Od2dExportView::polygonProc(nbPoints, pVertexList, pNormal, pExtrusion);

  g_bNoSkipTextOpForShx.pop_back();
}


//====================================================================
//
// isToKeepObjectWhole()
//
//  Returns True if thickness and linetype have default setting
//                  (in other words the object having Continuous linetype 
//                                                        - without Thickness 
//                                                        - or with Thickness but laying in the plane when 
//                                                          the project mode of view is not perspective);
//          False in otherwise.
//
bool OdDwfView::isToKeepObjectWhole(bool bParallel) const
{
  return (   (isThickness() && (!bParallel || (bParallel && isPerspective())))
          || !isLinetypeContinuous())
                ? false
                : true;
}


//==========================================================================================
//
// shxText()
//
void OdDwfView::shxText(
                        const OdGePoint3d& position,
                        const OdGeVector3d& u, 
                        const OdGeVector3d& v,
                        const OdChar* msg, 
                        OdInt32 length, 
                        bool raw, 
                        const OdGiTextStyle* pStyle,
                        const OdGeVector3d* pExtrusion)
{
  // To skip text operand in case that shx text is a part of
  //  entity as dimension, tolerance or linetype property & etc.
  ODA_ASSERT( !g_bNoSkipTextOpForShx.empty() );
  if (g_bNoSkipTextOpForShx.back())
  {
    turnVisibilityOn(false);                      // turn visibility on

    setText(position, u, v, msg, length, raw, pStyle, false);

    turnVisibilityOn(true);                       // turn visibility off
  }

  //context()->drawText(this, pDrawCtx, position, u, v, msg, length, raw, pStyle, pExtrusion);
  OdGiGeometrySimplifier::textProc(position, u, v, msg, length, raw, pStyle, pExtrusion);
}


//====================================================================
//
//
// setText()
//

//void odExtentsBox(const OdGiTextStyle* pStyle, const OdChar* pStr, int nStrLen, OdUInt32 flags, OdDbDatabase* pDb, OdGePoint3d& min, OdGePoint3d& max, OdGePoint3d* pEndPos);

void OdDwfView::setText(const OdGePoint3d& position,
                          const OdGeVector3d& u,      // normal
                          const OdGeVector3d& v,      // direction
                          const OdChar* msg, OdInt32 length, bool raw,
                          const OdGiTextStyle* pTextStyle,
                          bool  bBox,
                          const Od2dFont* pFont,
                          const OdArray<OdUInt16> *pUnicode)
{
  bool bASCII = true;

  WT_Unsigned_Integer16 * pWtUInt16 = NULL;
  if (pUnicode && pUnicode->size() > 0)
  {
    bASCII = false;
    int nCount = pUnicode->size();
    pWtUInt16 = new WT_Unsigned_Integer16[ nCount + 1 ];  

    // To convert OdUInt16 into WT_Unsigned_Integer16
    for (int i = 0; i < nCount; i++)
    {
      pWtUInt16[i] = (*pUnicode)[i];
    }
    pWtUInt16[nCount] = 0;
  }

  bool bInPlane(false);

  OdGeMatrix3d matrix = objectToDeviceMatrix();
  matrix.transposeIt();
  OdGeVector3d vZAxis = matrix.getCsZAxis();

  OdGiDrawable* pDrawable = g_Drawables.back();

  // if oblique to use Box as ACAD because oblique specificator (dwf format) dosn't work properly
  bool bBoxInCaseOfOblique(false);

  // 24.09.2004 GU changed cast from double to bool to comparing for the sake of
  // Borland portability. Although it is not very good to compare doubles in such manner.
  // Misha, please correct after your vacation.
  if (pFont && (pFont->m_oblique!=0.0) && (pFont->m_rotation!=0.0))
  {
    bBoxInCaseOfOblique = true;
  }

  if (pDrawable && bBox && pFont)
  {
    if (pDrawable->isKindOf(OdDbText::desc()))
    {
      OdDbText* pTextObj = OdDbText::cast(pDrawable).get();
      if (pTextObj->normal().isParallelTo(vZAxis))
      {
        bInPlane = true;
      }
    }
    else if (pDrawable->isKindOf(OdDbMText::desc()))
    {
      OdDbMText* pMTextObj = OdDbMText::cast(pDrawable).get();
      if (pMTextObj->normal().isParallelTo(vZAxis))
      {
        bInPlane = true;
      }
    }
    else
    {
      ODA_ASSERT(false);
      bInPlane = false;
    }
  }
  bool bSkip(false);

  OdGePoint3d ptMin, ptMax, pt1, pt3;

  // To transform the text insertion point to WT system
  OdGePoint3d ptPosition3d(position);
//  ptPosition3d.transformBy(objectToDeviceMatrix());

  WT_Logical_Point wtPosition;
  RoundPoint(wtPosition, OdGePoint2d(ptPosition3d.x, ptPosition3d.y));

  // There isn't a need of bounding box for shx or invisible text
  WT_Logical_Point* pBounds = NULL;
  WT_Logical_Point bounds[4];

  if (bBox && pFont)
  {
    ODA_ASSERT(pFont != NULL);
    // if Text lies in the current view plane (2D) there is no need in the box.
    // Otherwise to put box for Text
    if (!bInPlane || bBoxInCaseOfOblique)
    {
      pBounds = bounds;
    }

    // Get text extents...
    context()->textExtentsBox( *pTextStyle, msg, length, raw ? 1 : 0, ptMin, ptMax );

    // ... Init text box...
    pt1.x = ptMax.x;
    pt1.y = ptMin.y;
    pt3.x = ptMin.x;
    pt3.y = ptMax.y;

    // ... Take into account an angle...
    OdGeMatrix3d xForm;
    xForm.setCoordSystem(position, u, v, u.crossProduct(v));
    ptMin.transformBy(xForm);
    ptMax.transformBy(xForm);
    pt1.transformBy(xForm);
    pt3.transformBy(xForm);

    // ... Transform text box to WT system...
    WT_Logical_Point wtMin;
    WT_Logical_Point wtMax;
    WT_Logical_Point wt1;
    WT_Logical_Point wt3;
    RoundPoint(wtMin, OdGePoint2d(ptMin.x, ptMin.y));
    RoundPoint(wtMax, OdGePoint2d(ptMax.x, ptMax.y));
    RoundPoint(wt1, OdGePoint2d(pt1.x, pt1.y));
    RoundPoint(wt3, OdGePoint2d(pt3.x, pt3.y));
  
    // ... and finally to get text bounds
    bounds[0].m_x = wtMin.m_x;
    bounds[0].m_y = wtMin.m_y;
    bounds[1].m_x = wt1.m_x;  
    bounds[1].m_y = wt1.m_y;  
    bounds[2].m_x = wtMax.m_x;
    bounds[2].m_y = wtMax.m_y;
    bounds[3].m_x = wt3.m_x;  
    bounds[3].m_y = wt3.m_y;  

    // Clip Text by View
    //  If any point of the text bounding box is in View clip rectangle
    //  then the text is inside a View clip rectangle partly at least.
    //  To skip only that text which completely lie out of this one.
    OdGePoint2d points[4];
    points[0] = OdGePoint2d(ptMin.x, ptMin.y);
    points[1] = OdGePoint2d(pt1.x, pt1.y);
    points[2] = OdGePoint2d(ptMax.x, ptMax.y);
    points[3] = OdGePoint2d(pt3.x, pt3.y);

    if (PolygonOutOfRect2d(4, points, getMinViewClip(), getMaxViewClip()))
    {
      bSkip = true;
    }
    else
    {
      WT_Font wtFont;

      wtFont.set_fields_defined( WT_Font::FONT_ALL_FIELDS );

      WT_String fontName = pFont->m_font_name.c_str();
      if (fontName.length() == 0)
      {
        return;       // Font Name must be not empty
      }
      wtFont.font_name().set( fontName );

          // pmk 08.30.2004 -- conversion from 'const int' to 'WT_Byte'
      wtFont.charset() = (WT_Byte) pFont->m_charset;
      wtFont.pitch()   = (WT_Byte) pFont->m_pitch;
      wtFont.family()  = (WT_Byte) pFont->m_family;

      wtFont.style().set_bold( pFont->m_bold );
      wtFont.style().set_italic( pFont->m_italic );
      wtFont.style().set_underlined( pFont->m_underline );

      wtFont.height() = RoundDoubleToWTInteger32(pFont->m_height);

      WT_Integer16 wt_iValue;
                                          // describes how the entire string is rotated from horizontal
      wt_iValue = RoundDoubleToWTInteger16( pFont->m_rotation * dWtAngleMeasureOfDegree );
      wtFont.rotation() = ( int(wt_iValue) < nWtAngleMaxValue ) ? wt_iValue : nWtAngleMaxValue;

      wt_iValue = RoundDoubleToWTInteger16( pFont->m_width_scale * nFontMultipliedByFactor );
      wtFont.width_scale() = ( int(wt_iValue) < nWtAngleMaxValue ) ? wt_iValue : nWtAngleMaxValue;

      // wtFont.spacing() // * nFontMultipliedByFactor
      //wtFont.spacing() = (wt_iValue < nWtAngleMaxValue) ? wt_iValue : nWtAngleMaxValue;

                                        // The desired rotation of the individual characters within the font
      wt_iValue = RoundDoubleToWTInteger16( OdaToDegree( pFont->m_oblique ) * dWtAngleMeasureOfDegree );
      wtFont.oblique() = ( int(wt_iValue) < nWtAngleMaxValue ) ? wt_iValue : nWtAngleMaxValue;

      WT_Integer32 flags = pFont->m_flags;      // Flag bits reserved for the DWF generator’s use.
      wtFont.flags().set( flags );

      device()->dc_font( wtFont );
    }
  }

  if (!bSkip)
  {
    if (bASCII)
    {
      WT_Text wtText( wtPosition, msg, pBounds, 0, 0, 0, 0 );
      device()->dc_text( wtText );
    }
    else
    {
      // Constructs a WT_String object from a null-terminated wide character string (i.e. Unicode.)
      WT_String wtUnicode( pWtUInt16 );

      WT_Text wtText( wtPosition, wtUnicode, pBounds, 0, 0, 0, 0 );
      device()->dc_text( wtText );
    }
  }

  if (pWtUInt16)
  {
    delete [] pWtUInt16;
  }
}


//====================================================================
//
//
// add_UrlToMap()
//
/*
void OdDwfView::add_UrlToMap(const OdString& sName, 
                       const OdString& sDescription, 
                       const OdString& sSubLocation)
*/
void OdDwfView::add_UrlToMap(DwfUrl& dwfUrl)
{
    OdString sKey( dwfUrl.sName + dwfUrl.sDescription );

    int url_num;        // The URL number which will be assigned to the geometric primitives 
                        //  that are subsequently created. 
                        //  NOTE: Do not use layer numbers less than 1. 

    mapURLs::const_iterator it;
    it = mapURL.find( sKey );               // If the URL exists in the map...

    if (it != mapURL.end())
    {
      url_num = (*it).second;               // ... to get Layer number by Name
    }
    else
    {
      url_num = mapURL.size() + 1;          // Otherwise to set number and to add into the map
      mapURL[sKey] = url_num;
    }

    ODA_ASSERT(url_num >= 1);

    dwfUrl.iUrlNum = url_num;
}



//====================================================================
//
// process_Url()
//
//  Checks url data existence
//
void OdDwfView::process_Url(const OdGiDrawable* pDrawable)
{
  DwfUrl dwfUrl;

  dwfUrl.iUrlNum = (g_bUrlTrigger) ? 0 : iDwfUrlOff;

  OdDbEntity* pEnt = OdDbEntity::cast(pDrawable).get();

  if (pEnt)
  {
    OdDbEntityHyperlinkPEPtr hpe = pEnt;

    if (hpe->hasHyperlink(pEnt))
    {
      OdDbHyperlinkCollectionPtr collection = hpe->getHyperlinkCollection(pEnt, true);

      //OdDbHyperlink* pUrl = collection->item( collection->count() - 1 );
      OdDbHyperlink* pUrl = collection->item( 0 );

      dwfUrl.sName        = pUrl->name();
      dwfUrl.sDescription = pUrl->description();
      dwfUrl.sSubLocation = pUrl->subLocation();

      add_UrlToMap( dwfUrl );

      g_bUrlTrigger = true;     // to set on trigger because the current url list was changed
    }
  }

  g_UrlList.push_back(dwfUrl);
}


void OdDwfView::postprocess_Url()
{
  ODA_ASSERT( !g_UrlList.empty() );

  DwfUrl dwfUrl = g_UrlList.back();

  if (dwfUrl.iUrlNum > 0)
  {
    g_bUrlTrigger = true;  // url trigger was changed
  }

  g_UrlList.pop_back();

  ODA_ASSERT( !g_UrlList.empty() );
}

