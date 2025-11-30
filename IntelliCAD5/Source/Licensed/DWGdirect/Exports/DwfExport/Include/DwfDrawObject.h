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
// DwfDrawObject.h: definition of the GsDwfFactory, OdDwfDevice, OdDwfView classes.
//
//////////////////////////////////////////////////////////////////////

#ifndef _DWFDRAWOBJECT_INCLUDED_
#define _DWFDRAWOBJECT_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "2dExportDevice.h"
#include "whiptk/whip_toolkit.h"


class CDwfExportImpl;

struct LineStyle
{
  WT_Line_Style::WT_Joinstyle_ID  join;
  WT_Line_Style::WT_Capstyle_ID   startCap;
  WT_Line_Style::WT_Capstyle_ID   endCap;
};


struct DwfUrl
{
  int       iUrlNum;
  OdString  sName;
  OdString  sDescription;
  OdString  sSubLocation;
};

//////////////////////////////////////////////////////////////////////
class OdDwfDevice : public Od2dExportDevice
{
  // Members
  //

  CDwfExportImpl*   m_dwf;

public:
  enum KindOfRun 
  {
    Nonprocess_run = 0 ,      // it can be used for presetting something (for instance, ColorMap)
    Main_run
  };

  KindOfRun         m_bKindOfRun;

  // Constructor
  //
  OdDwfDevice();
  virtual ~OdDwfDevice() 
  {}

  // Set the target data stream and the type.  
  //static OdGsDevicePtr createObject(DeviceType type = /*Od2dExportDevice::*/k3dDevice);
  static OdGsDevicePtr createObject();

  // Called by the DWGdirect rendering framework before rendering this view.
  // The override should call the parent version of this function, Od2dExportDevice::update(). 
  void update(OdGsDCRect* pUpdatedRect, KindOfRun bKindOfRun);

  // Called by the rendering framework to create an instance 
  // of the desired OdGsView subclass. 
  OdGsViewPtr createView(const OdGsClientViewInfo* pInfo = 0, 
                         bool bEnableLayerVisibilityPerView = false);


  //  ----- Od2dExportDevice methods -----

  // writing process
  void dc_polyline( OdUInt32 nPts, const OdGePoint2d* points );
  void dc_polygon ( OdUInt32 nPts, const OdGePoint2d* points );
  void dc_circle  ( const OdGeEllipArc3d& ellipse );
  void dc_ellipse ( const OdGeEllipArc3d& ellipse );
  void dc_gouraud_polytriangle(
                        const OdGePoint3d* pVertexList,
                        OdInt32 faceListSize,
                        const OdInt32* pFaceList,
                        const OdCmEntityColor* pColors );
  
  void dc_raster_image( OdGiRasterImagePtr pRaster, 
                        OdGiRasterImagePtr pMask,
                        const OdGeExtents3d& imgExts,
                        bool bBitonalTransp);
/*
  virtual void dc_text( const OdGePoint3d& position,
                        const OdGeVector3d& normal,
                        const OdGeVector3d& direction,
                        const OdChar* msg, OdInt32 length, bool raw,
                        const OdGiTextStyle* pTextStyle,
                        bool  bBox,
                        const WT_Font* pWtFont );
*/

  void dc_color(ODCOLORREF rgb);
  void dc_layer(int layer_num, const OdChar * layer_name);

  OdDbDatabase* getDb();

  const ODCOLORREF* getPalette() const;

  int getPaletteColorNumbers() const
  {
    return 256;
  }

  virtual int getSupportedColorBits(int nColorBits, bool bTransparency) const;

  //  ----- OdDwfDevice methods -----
  //

  // Attention: there is a need to call own device methods for setting of context 
  //            before setupActiveLayoutViews() or setupLayoutViews() not after! 
  //            Only base interface methords could be called after them.

  void setContext(CDwfExportImpl* dwf);             // Sets a spetial OdDwfDevice context

  CDwfExportImpl* getExportImpl()
  {
    return m_dwf;
  }

  WT_File* getWtFile();

  // To set\check a type of processing (with\without geometry)
  //
  void setRun(KindOfRun bKindOfRun)                 
  {
    m_bKindOfRun = bKindOfRun;
  }

  bool IsNonprocessRun() const
  {
    return (m_bKindOfRun == Nonprocess_run) ? true : false;
  }

  bool IsMainRun() const
  {
    return (m_bKindOfRun == Main_run) ? true : false;
  }


  // ----- Writer -----

  void dc_font(const WT_Font& wtFont);
  void dc_text(const WT_Text& wtText);
  void dc_url_item(WT_URL_Item&  wtUrlItem);
  void dc_url_clear();
  void push_Url();
  void set_Urls();

  // to set off the rendition
  void dc_urls_remove_all();
  void dc_layers_remove_all();

  void dc_gouraud_polytriangle(const WT_Gouraud_Polytriangle& wtColoredPoly);

  //void dc_lineWeight(OdDb::LineWeight lineWeight);
  void dc_lineWeight(WT_Integer32 lineWeight);
  void dc_lineStyle(const LineStyle& lineStyle);

  void dc_visibility(bool bVisibility)
  {
    WT_Visibility visibility(bVisibility);
    visibility.serialize(*(getWtFile()));
  }
  void dc_fill(bool bFill)                          // On/off fill mode
  {
    WT_Fill wtFill(bFill);  
    getWtFile()->desired_rendition().fill() = wtFill;  
  }

  void onSize(const OdGsDCRect& outputRect);

};


//////////////////////////////////////////////////////////////////////

class OdDwfView : public Od2dExportView
{
  // Members
  //

  ODCOLORREF        m_curRGB;
  OdDb::LineWeight  m_lineWeight;

  LineStyle         m_lineStyle;
  double            m_dThickness;
  bool              m_bLinetypeContinuous;

  bool              m_bTurnVisibilityOn;

  double            m_dScale;

public:
  // Returns the ExGsSimpleDevice instance that owns this view. 

  OdDwfDevice* device()
  { 
    return (OdDwfDevice*)Od2dExportView::device();
  }


  // Constructor. 
  OdDwfView() 
  {}

  // PseudoConstructor. 
  static OdGsViewPtr createObject()
  {
    return OdRxObjectImpl<OdDwfView, OdGsView>::createObject();
  }

  // Called by the DWGdirect rendering framework to render 
  // each entity in this view.  This override allows a client 
  // application to examine each entity before it is rendered.  
  // The override should call
  // the parent version of this function, OdGsBaseView::draw(). 
  void draw(const OdGiDrawable*);


  // --------  OdGiConveyorGeometry  --------
/*
  void polylineProc(
            OdInt32 nbPoints, 
            const OdGePoint3d* pVertexList,
            const OdGeVector3d* pExtrusion = 0, 
            OdInt32 lBaseSubEntMarker = -1);
*/
  void polylineProc(
            OdInt32 nbPoints, 
            const OdGePoint3d* pVertexList,
            const OdGeVector3d* pNormal = 0,
            const OdGeVector3d* pExtrusion = 0,
            OdInt32 lBaseSubEntMarker = -1);

  
  void polygonProc(
            OdInt32 nbPoints, 
            const OdGePoint3d* pVertexList,
            const OdGeVector3d* pNormal = 0,
            const OdGeVector3d* pExtrusion = 0);

  // --------  Od2dExportView  --------

  // Retrieves the current rendering traits from DWGdirect (color, linewidth, etc.) 
  //  and sets these properties in this device. 
  void setCurrTraits(const OdGiSubEntityTraitsData& currTraits);

  void update();

  // This call is  to order a preliminary loop for viewing & processing of current settings 
  //  without geometry processing.
  bool isProcessGeom();

  bool isThickness() const
  {
    return OdZero(m_dThickness) ? false : true;
  }

  bool isLinetypeContinuous() const
  {
    return m_bLinetypeContinuous;
  }

  bool isToKeepObjectWhole(bool bParallel) const;       // Returns true if thickness and linetype  
                                                        //  have default setting

  void shxText( 
                const OdGePoint3d& position,
                const OdGeVector3d& u, 
                const OdGeVector3d& v,
                const OdChar* msg, 
                OdInt32 length, 
                bool raw, 
                const OdGiTextStyle* pStyle,
                const OdGeVector3d* pExtrusion );

  void setText( const OdGePoint3d& position,
                const OdGeVector3d& u,
                const OdGeVector3d& v,
                const OdChar* msg, OdInt32 length, bool raw,
                const OdGiTextStyle* pTextStyle,
                bool  bBox,
                const Od2dFont* pFont = NULL,
                const OdArray<OdUInt16> *pUnicode = NULL );


  // --------  OdDwfView  --------

  // Called by each associated view object to set the current draw color (as RGB). 
  void set_Color(ODCOLORREF rgb);

  // Called by each associated view object to set the current draw color (as index). 
  void set_ColorIndex(OdUInt16 colorIndex);

  // Called by each associated view object to set the current line width. 
  void set_LineWeight(OdDb::LineWeight lineWeight);

  void set_LineStyle(const LineStyle& lineStyle);
  void set_Layer(OdDbStub* layer);

  void init();
  void putViewport();
  void setScale();
  void convDeviceMatrixToWt(double* xform) const;
  OdGeMatrix3d projectionMatrixMy() const;
  void convViewportToWt(WT_Logical_Point& wtMin, WT_Logical_Point& wtMax) const;

  void turnVisibilityOn(bool flag)
  {
    if (m_bTurnVisibilityOn != flag)
    {
      m_bTurnVisibilityOn = flag;
      device()->dc_visibility(flag);
    }
  }

  void process_Url(const OdGiDrawable* pDrawable);    // Checks url data existence
  void postprocess_Url();
  void add_UrlToMap(DwfUrl& dwfUrl);

  friend class OdDwfDevice;

};  // end OdDwfView 


#endif // #ifndef _DWFDRAWOBJECT_INCLUDED_

