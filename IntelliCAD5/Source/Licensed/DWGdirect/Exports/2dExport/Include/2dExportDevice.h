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
// 2dExportDevice.h: definition of the Od2dExportDevice, Od2dExportDevice classes.
//                    These are base classes for 2d export.
//
//////////////////////////////////////////////////////////////////////

#ifndef _2DEXPORTDEVICE_INCLUDED_
#define _2DEXPORTDEVICE_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GsBaseVectorizer.h"
#include "GiContextForDbDatabase.h"
#include "GiGeometrySimplifier.h"
//#include "GiOrthoClipper.h"
#include "GiSpatialFilter.h"

class Od2dExportView;

class Od2dFont
{
public:
  Od2dFont()
    : m_flags(0)
    , m_font_name("")
    , m_bold(false)
    , m_italic(false)
    , m_underline(false)
    , m_charset(1)
    , m_pitch(0)
    , m_family(0)
    , m_height(1.0)
    , m_rotation(0)
    , m_width_scale(1.0)
    , m_oblique(0)
    , m_spacing(1024)
  {}

  OdString      m_font_name;        // Font name
  bool          m_bold;             // Boldface option
  bool          m_italic;           // Italics option
  bool          m_underline;        // Underline option
  int           m_charset;          // Character set code as defined
  int           m_pitch;            // Pitch bitmask as defined 
  int           m_family;           // Font family as defined 
  double        m_height;
  double        m_rotation;
  double        m_width_scale;
  double        m_oblique;
  double        m_spacing;

  enum
  {
      VERTICAL        = 0x0001      // TVERT
    , MIRROR_X        = 0x0002      // TXMIR
    , MIRROR_Y        = 0x0004      // TYMIR
    , UNDERSCORE      = 0x0008      // TUNDER
    , OVERSCORE       = 0x0010      // TOVER
    , MTEXT_START     = 0x0020      // TMSTART
    , MTEXT_END       = 0x0040      // TMEND
    , MTEXT           = 0x0080      // TMTEXT
    , GREEK_TEXT      = 0x1000      // Whip and GDI font engine only
    , PATH_TEXT       = 0x2000      // Whip and GDI font engine only
    , OUTLINE_TEXT    = 0x4000      // Outline Text Only
  };

  long                        m_flags;
};

//////////////////////////////////////////////////////////////////////

class Od2dExportDevice : public OdGsBaseVectorizeDevice, 
                         public OdGiContextForDbDatabase
{
  // Members
  //

public:
  enum ImageOrg
  {
      orgBitonal = 1      // 1 bit per pixel
    , orgMapped  = 8      // 8 bits per pixel, paletted
    , orgRGB     = 24     // 24 bits. Red-green-blue
    , orgBGRA    = 32     // 32 bits. Blue-green-red-alpha
  };

/*
  enum DeviceType
  {
    k2dDevice,
    k3dDevice
  };
*/
protected:
  //DeviceType  m_type;

  ODRX_USING_HEAP_OPERATORS(OdGsBaseVectorizeDevice);

public:

  // Constructor.
  Od2dExportDevice();
  virtual ~Od2dExportDevice() 
  {}

  // Set the target data stream and the type.  
  //static OdGsDevicePtr createObject(DeviceType type = /*Od2dExportDevice::*/k3dDevice);
  static OdGsDevicePtr createObject();

  // Attention: there is a need to call own device methods for setting of context 
  //            before setupActiveLayoutViews() or setupLayoutViews() not after! 
  //            Only base interface methords could be called after them.


  // Called by the DWGdirect rendering framework before rendering this view.
  // The override should call the parent version of this function, OdGsBaseVectorizeDevice::update(). 
  void update(OdGsDCRect* pUpdatedRect);
  
  // output into device context

  virtual void dc_polyline(OdUInt32 , const OdGePoint2d* )= 0;
  virtual void dc_polygon(OdUInt32 , const OdGePoint2d* ) = 0;
  virtual void dc_circle( const OdGeEllipArc3d& ellipse ) = 0;
  virtual void dc_ellipse(const OdGeEllipArc3d& ellipse ) = 0;
  virtual void dc_gouraud_polytriangle(
                          const OdGePoint3d* pVertexList,
                          OdInt32 faceListSize,
                          const OdInt32* pFaceList,
                          const OdCmEntityColor* pColors )= 0;
  virtual void dc_raster_image( OdGiRasterImagePtr pRaster, 
                                OdGiRasterImagePtr pMask,
                                const OdGeExtents3d& imgExts,
                                bool bBitonalTransp) = 0;

  virtual void dc_color( ODCOLORREF )                     = 0;
  virtual void dc_layer( int, const OdChar* )             = 0;

  //void dc_lineWeight(OdDb::LineWeight lineWeight);
//  void dc_lineWeight(WT_Integer32 lineWeight);

  virtual OdDbDatabase* getDb() = 0;

  virtual const ODCOLORREF* getPalette() const = 0;

  virtual int getPaletteColorNumbers() const = 0;

  virtual int getSupportedColorBits( int nColorBits, bool bTransparency ) const;

}; // end Od2dExportDevice



//////////////////////////////////////////////////////////////////////

class Od2dExportView :   public OdGsBaseVectorizeView
                       , public OdGiGeometrySimplifier  // Receives 3d data from the DWGdirect 
                                                        //  rendering framework. 
{
  // Members
  //
  OdGePoint2d       m_ptMinViewClip;
  OdGePoint2d       m_ptMaxViewClip;

public:
  OdGiSpatialFilterPtr  m_pSpatialFilter;
  OdGiXformPtr        m_pXYProjector;

public:
  // Returns the ExGsSimpleDevice instance that owns this view. 
  Od2dExportDevice* device()
  { 
    return (Od2dExportDevice*)OdGsBaseVectorizeView::device();
  }

  // Constructor. 
  Od2dExportView();

  friend class Od2dExportDevice;

public:
  OdGiConveyorOutput& screenOutput() { return m_pXYProjector->output(); }

  // Called by the DWGdirect rendering framework to give the 
  // client application a chance to terminate the current 
  // rendering process.  Returning true from this function will 
  // stop the current rendering operation. 
  bool regenAbort() const;

  double deviation(const OdGiDeviationType, const OdGePoint3d&) const
  {
    // to force DWGdirect rendering framework to use some reasonable value.
    return 0.0; 
  }

  // ----- The Overrides of OdGiGeometrySimplifier (OdGiConveyorGeometry) -----
  //
  // Must be overridden at least.
  void polylineOut( OdInt32 /*nbPoints*/, 
                    const OdGePoint3d* /*pVertexList*/)
  {}

  // Must be overridden at least.
  void polygonOut( OdInt32 /*nbPoints*/, 
                   const OdGePoint3d* /*pVertexList*/, 
                   const OdGeVector3d* /*pNormal*/ = NULL)
  {}


  void circleProc(
            const OdGePoint3d& center, 
            double radius, 
            const OdGeVector3d& normal, 
            const OdGeVector3d* pExtrusion = 0);
  
  void circleProc(
            const OdGePoint3d&, 
            const OdGePoint3d&, 
            const OdGePoint3d&, 
            const OdGeVector3d* pExtrusion = 0);
  
  void circularArcProc(
            const OdGePoint3d& center,
            double radius,
            const OdGeVector3d& normal,
            const OdGeVector3d& startVector,
            double sweepAngle,
            OdGiArcType arcType = kOdGiArcSimple, 
            const OdGeVector3d* pExtrusion = 0);
  
  void circularArcProc(
            const OdGePoint3d& start,
            const OdGePoint3d& point,
            const OdGePoint3d& end,
            OdGiArcType arcType = kOdGiArcSimple, 
            const OdGeVector3d* pExtrusion = 0);
  
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

  void meshProc(
            OdInt32 rows,
            OdInt32 columns,
            const OdGePoint3d* pVertexList,
            const OdGiEdgeData* pEdgeData = 0,
            const OdGiFaceData* pFaceData = 0,
            const OdGiVertexData* pVertexData = 0);
  
  void shellProc(
            OdInt32 nbVertex,
            const OdGePoint3d* pVertexList,
            OdInt32 faceListSize,
            const OdInt32* pFaceList,
            const OdGiEdgeData* pEdgeData = 0,
            const OdGiFaceData* pFaceData = 0,
            const OdGiVertexData* pVertexData = 0);
  
/*  void textProc(
            const OdGePoint3d& position,
            const OdGeVector3d& u, 
            const OdGeVector3d& v,
            const OdChar* msg,
            const OdGeVector3d* pExtrusion = 0);
*/  
  void textProc(
            const OdGePoint3d& position,
            const OdGeVector3d& u, 
            const OdGeVector3d& v,
            const OdChar* msg, 
            OdInt32 length, 
            bool raw, 
            const OdGiTextStyle* pTextStyle,
            const OdGeVector3d* pExtrusion = 0);
/*
  void textProc(const OdGePoint3d& position,
            const OdGeVector3d& normal, 
            const OdGeVector3d& direction,
            double height, 
            double width, 
            double oblique, 
            const OdChar* msg,
            const OdGeVector3d* pExtrusion = 0);
  
  void textProc(const OdGePoint3d& position,
            const OdGeVector3d& normal, 
            const OdGeVector3d& direction,
            const OdChar* msg, 
            OdInt32 length, 
            bool raw, 
            const OdGiTextStyle* pTextStyle,
            const OdGeVector3d* pExtrusion = 0);
*/  
  void shapeProc(
            const OdGePoint3d& position,
            const OdGeVector3d& u, 
            const OdGeVector3d& v,
            int shapeNo, 
            const OdGiTextStyle* pStyle,
            const OdGeVector3d* pExtrusion = 0);
  
  void xlineProc(
            const OdGePoint3d&, 
            const OdGePoint3d&);
  
  void rayProc(
            const OdGePoint3d&, 
            const OdGePoint3d&);
  
  void nurbsProc(
            const OdGeNurbCurve3d& nurbs);

  void ellipArcProc(
            const OdGeEllipArc3d& arc,
            const OdGePoint3d* pEndPointsOverrides,
            OdGiArcType arcType,
            const OdGeVector3d* pExtrusion);

  void rasterImageProc(
            const OdGePoint3d& origin,
            const OdGeVector3d& u,
            const OdGeVector3d& v,
            const OdGiRasterImage* pImg, // image object
            const OdGePoint2d* uvBoundary, // may not be null
            OdUInt32 numBoundPts,
            bool transparency = false,
            double brightness = 50.0,
            double contrast = 50.0,
            double fade = 0.0);

	void metafileProc(
            const OdGePoint3d& origin,
		        const OdGeVector3d& u,
		        const OdGeVector3d& v,
            const OdGiMetafile* pMetafile,
            bool bDcAligned = true,       // reserved
            bool bAllowClipping = false); // reserved

  // --------  Od2dExportView  --------
  //

  // Retrieves the current rendering traits from DWGdirect (color, linewidth, etc.) 
  //  and sets these properties in this device. 
  virtual void setCurrTraits(const OdGiSubEntityTraitsData& currTraits);
  virtual void setCurrTraits();

  virtual void beginViewVectorization();
  virtual void endViewVectorization();

  // This call is to order a preliminary loop for viewing & processing of current settings 
  //  without geometry processing.
  virtual bool isProcessGeom()
  {
    return false;
  }

  virtual const OdGePoint2d& getMinViewClip() const
  {
    return m_ptMinViewClip;
  }
  virtual const OdGePoint2d& getMaxViewClip() const
  {
    return m_ptMaxViewClip;
  }

  virtual void setMinViewClip(const OdGePoint2d& ptMinViewClip)
  {
    m_ptMinViewClip = ptMinViewClip;
  }
  virtual void setMaxViewClip(const OdGePoint2d& ptMaxViewClip)
  {
    m_ptMaxViewClip = ptMaxViewClip;
  }

  virtual bool isToKeepObjectWhole(bool /*bParallel*/) const
  {
    return true;
  }

  virtual void setEllipseArc(OdGeEllipArc3d& ellipse, const OdGeVector3d& normal);

  virtual bool isTransparency(OdGeVector3d screenU, OdGeVector3d screenV);

  virtual void shxText( const OdGePoint3d& /*position*/,
                        const OdGeVector3d& /*u*/,
                        const OdGeVector3d& /*v*/,
                        const OdChar* /*msg*/,
                        OdInt32 /*length*/,
                        bool /*raw*/,
                        const OdGiTextStyle* /*pStyle*/,
                        const OdGeVector3d* /*pExtrusion*/ )
  {
    // Here is the place for additional processing of SHX text in case of a need
  }

  virtual void setText( const OdGePoint3d& /*position*/,
                        const OdGeVector3d& /*u*/,
                        const OdGeVector3d& /*v*/,
                        const OdChar* /*msg*/, OdInt32 /*length*/, bool /*raw*/,
                        const OdGiTextStyle* /*pTextStyle*/,
                        bool  /*bBox*/,
                        const Od2dFont* /*pFont*/ = NULL,
                        const OdArray<OdUInt16>* /*pUnicode*/ = NULL )
  {}

  virtual void setShell(OdInt32 nbVertex,
                        const OdGePoint3d* pVertexList,
                        OdInt32 faceListSize,
                        const OdInt32* pFaceList,
                        const OdGiEdgeData* pEdgeData,
                        const OdGiFaceData* pFaceData );

  bool textProcessing( const OdGePoint3d& position,
                       const OdGeVector3d& u,
                       const OdGeVector3d& v,
                       const OdChar* msg,
                       OdInt32 length,
                       bool raw,
                       const OdGiTextStyle* pStyle,
                       OdString& pProcessedText,
                       //OdGiConveyorEntryPoint& giEntryPoint,
                       //OdGiXformPtr&  pFontXform,
                       OdArray<OdUInt16>& arrayUnicode,
                       //OdArray<OdGePoint3d>& arrayOverlines,
                       //OdArray<OdGePoint3d>& arrayUnderlines,
                       bool bASCII );


}; // end Od2dExportView 


#endif  // _2DEXPORTDEVICE_INCLUDED_
