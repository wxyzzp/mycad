#include "StdAfx.h"
#include "SvgExport.h"
#include "SvgWrappers.h"
#include "SvgShxEmbedding.h"
#include "Gi/GiSpatialFilter.h"

// for old compilers having problems with Koenig namespace lookup
#if !defined(_MSC_VER) && !defined (__BCPLUSPLUS__) && !defined(ODA_GCC)
using namespace xml;
#endif

namespace xml
{
  OdString Attribute::_doubleFormat = "%g";
}

class OdSvgDevice;
typedef OdSmartPtr<OdSvgDevice> OdSvgDevicePtr;

class OdSvgView : public OdGsBaseVectorizeView
{
  OdGiOrthoClipperPtr _pViewportClip;
  OdGiXformPtr _pXYProjector;
  OdGiSpatialFilterPtr _pSpatialFilter;

  OdGiPerspectivePreprocessorPtr _pPerspPrepr;
  void setupDeviations();
  void setupConveyor();
  void setOuterClipPath();
  void setupPerspective();
  void setupSpatialFilter( const OdGeMatrix3d& xEyeToDc );
  void setView(const OdGePoint3d & position,
    const OdGePoint3d& target,
    const OdGeVector3d& upVector,
    double fieldWidth,
    double fieldHeight,
    Projection projection);
public:
  OdSvgView()
  {
    _pViewportClip = OdGiOrthoClipper::createObject();
    _pViewportClip->input().addSourceNode(output());
    _pSpatialFilter = OdGiSpatialFilter::createObject();
    _pSpatialFilter->input().addSourceNode(_pViewportClip->output());
    _pXYProjector = OdGiXform::createObject();
    _pXYProjector->input().addSourceNode(_pSpatialFilter->insideOutput());
    _pXYProjector->input().addSourceNode(_pSpatialFilter->intersectsOutput());
    _pPerspPrepr = OdGiPerspectivePreprocessor::createObject();
  }

  OdSvgDevice* device();

  static OdGsViewPtr createObject()
  {
    return OdRxObjectImpl<OdSvgView, OdGsView>::createObject();
  }
  void update();
  OdGiConveyorOutput& screenOutput() { return _pXYProjector->output(); }
};

class SvgProperties;
typedef OdSmartPtr<SvgProperties> SvgPropertiesPtr;

class SvgProperties : public OdRxDispatchImpl<>
{
  OdStreamBufPtr _stream;
  double _tolerance;
  double _lwScale;
  OdString _imageBasePath;
  OdString _imageURL;
  double _shxLineWeight;
  OdString _defImageExt;
  ODCOLORREF _bkColor;
  OdInt32 _doublePrecision;
  OdString _genericFontFamily;
public:
  SvgProperties() 
    : _tolerance( .5 )
    , _lwScale(0)
    , _shxLineWeight( 0.05 )
    , _defImageExt( "png" )
    , _bkColor( ODRGB( 0xff, 0xff, 0xff ) )
    , _doublePrecision( 6 )
    , _genericFontFamily( "sans-serif" )
  {}
  ODRX_DECLARE_DYNAMIC_PROPERTY_MAP(SvgProperties);
  static SvgPropertiesPtr createObject()
  {
    return OdRxObjectImpl<SvgProperties, OdRxDictionary>::createObject();
  }
  OdInt32 get_Precision() const { return _doublePrecision; }
  void put_Precision( OdInt32 p )
  {
    _doublePrecision = p;
    if ( _doublePrecision < 1 || _doublePrecision > 20 ) _doublePrecision = 6;
    if ( _doublePrecision != 6 )
      xml::Attribute::_doubleFormat.format( "%%.%dg", _doublePrecision );
    else xml::Attribute::_doubleFormat = "%g";
  }
  double get_Tolerance() const { return _tolerance; }
  void put_Tolerance( double tol ){ _tolerance = tol; }
  OdRxObjectPtr get_Output() const { return OdRxObject::cast(_stream); }
  void put_Output( OdRxObject* obj ){ _stream = obj; }
  double get_LineWeightScale() const { return _lwScale; }
  void put_LineWeightScale( double lws ){ _lwScale = lws; }
  const OdString& get_ImageBase() const { return _imageBasePath; }
  void put_ImageBase( const OdString& path ) 
  { 
    _imageBasePath = path; 
    char last = _imageBasePath[ _imageBasePath.getLength() - 1 ];
    if ( last != '\\' && last != '/' ) _imageBasePath += '/';
  }
  const OdString& get_ImageUrl() const { return _imageURL; }
  void put_ImageUrl( const OdString& url ) { _imageURL = url; }
  double get_ShxLineWeight(){ return _shxLineWeight; }
  void put_ShxLineWeight( double lw ) { _shxLineWeight = lw; }
  const OdString& get_DefaultImageExt() const { return _defImageExt; }
  void put_DefaultImageExt( const OdString& ext )
  { 
    if ( ext[0] != '.' ) _defImageExt = OdString(".") + ext;
    else _defImageExt = ext;
  }
  ODCOLORREF get_BackGround() const { return _bkColor; }
  void put_BackGround( ODCOLORREF clr ) { _bkColor = clr; }
  OdString get_GenericFontFamily() const { return _genericFontFamily; }
  void put_GenericFontFamily( const OdString& s ){ _genericFontFamily = s;}
};

ODRX_DECLARE_PROPERTY(DefaultImageExt)
ODRX_DECLARE_PROPERTY(ShxLineWeight)
ODRX_DECLARE_PROPERTY(Tolerance)
ODRX_DECLARE_PROPERTY(LineWeightScale)
ODRX_DECLARE_PROPERTY(ImageBase)
ODRX_DECLARE_PROPERTY(ImageUrl)
ODRX_DECLARE_PROPERTY(Output)
ODRX_DECLARE_PROPERTY(BackGround)
ODRX_DECLARE_PROPERTY(Precision)
ODRX_DECLARE_PROPERTY(GenericFontFamily)

ODRX_DEFINE_PROPERTY(DefaultImageExt, SvgProperties)
ODRX_DEFINE_PROPERTY(ShxLineWeight, SvgProperties)
ODRX_DEFINE_PROPERTY(Tolerance, SvgProperties)
ODRX_DEFINE_PROPERTY(LineWeightScale, SvgProperties)
ODRX_DEFINE_PROPERTY(ImageBase, SvgProperties)
ODRX_DEFINE_PROPERTY(ImageUrl, SvgProperties)
ODRX_DEFINE_PROPERTY_OBJECT(Output, SvgProperties,  get_Output, put_Output, OdStreamBuf)
ODRX_DEFINE_PROPERTY(BackGround, SvgProperties)
ODRX_DEFINE_PROPERTY(Precision, SvgProperties)
ODRX_DEFINE_PROPERTY(GenericFontFamily, SvgProperties)

ODRX_BEGIN_DYNAMIC_PROPERTY_MAP( SvgProperties );
  ODRX_GENERATE_PROPERTY(DefaultImageExt)
  ODRX_GENERATE_PROPERTY(ShxLineWeight)
  ODRX_GENERATE_PROPERTY(Tolerance)
  ODRX_GENERATE_PROPERTY(LineWeightScale)
  ODRX_GENERATE_PROPERTY(ImageBase)
  ODRX_GENERATE_PROPERTY(ImageUrl)
  ODRX_GENERATE_PROPERTY(Output);
  ODRX_GENERATE_PROPERTY(BackGround)
  ODRX_GENERATE_PROPERTY(Precision)
  ODRX_GENERATE_PROPERTY(GenericFontFamily)
ODRX_END_DYNAMIC_PROPERTY_MAP( SvgProperties );

namespace svg
{
  // Check if image has valid size
  //
  bool isValidImage( const OdGiRasterImage* pImg ) 
  {
    return pImg->pixelWidth() != 0 && pImg->pixelHeight() != 0;
  }
  // Apply gamma correction to filter.
  //
  inline static void applyGamma( xml::Node* filter, double gamma )
  {
    ODA_ASSERT( gamma > 0 && gamma < 100 );
    xml::Node* feComponentTransfer = filter->addChild( "feComponentTransfer" );
    xml::Node* ff[3] = { feComponentTransfer->addChild( "feFuncR" ), 
      feComponentTransfer->addChild( "feFuncG" ), 
      feComponentTransfer->addChild( "feFuncB" ) };
    double ex = log( .5 ) / log( 1. - gamma / 100 );
    for ( int i = 0; i < 3; i++ )
    {
      ff[i]->addAttribute( "type","gamma" );
      ff[i]->addAttribute( "exponent", ex );
      ff[i]->addAttribute( "amplitude", 1. );
    }
  }
  
  // Apply contrast correction to filter.
  //
  inline static void applyContrast( xml::Node* filter, double contrast )
  {
    ODA_ASSERT( contrast > 0 && contrast < 100 );
    xml::Node* feComponentTransfer = filter->addChild( "feComponentTransfer" );
    xml::Node* ff[3] = { new xml::Node( "feFuncR" ), 
      new xml::Node( "feFuncG" ), 
      new xml::Node( "feFuncB" ) };
    for ( int i = 0; i < 3; i++ )
    {
      feComponentTransfer->addChild( ff[i] );
      ff[i]->addAttribute( "type","linear" );
      ff[i]->addAttribute( "slope", contrast / 50 );
      ff[i]->addAttribute( "intercept", 0.5 * (1. - contrast / 50) );
    }
  }
  
  // Format transform to this coord. system
  //
  inline static OdString formatTransformMatrix( const OdGePoint3d& position,
    const OdGeVector3d& u, const OdGeVector3d& v )
  {
    OdGeMatrix2d m; m.setCoordSystem( OdGePoint2d( position.x, position.y ),
      OdGeVector2d( u.x, u.y), OdGeVector2d( -v.x, -v.y ) );
    OdString s; s.format("matrix(%g %g %g %g %g %g)", 
      m.entry[0][0], m.entry[1][0], m.entry[0][1], m.entry[1][1]
      , m.entry[0][2], m.entry[1][2]);
    return s;
  }
}

// Main exporting device, 
// implements OdGiConveyorGeometry through OdGiGeometrySimplifier.
//
class OdSvgDevice : public OdGsBaseVectorizeDevice, public OdGiGeometrySimplifier
{
  friend class OdSvgView;
  xml::Node* _svgRoot;
  xml::Node* _currentNode;
  xml::Node* _defs;
  ODCOLORREF _color;
  bool _fill;
  double _currentLineWeight;
  OdSvgView* _currentView;
  typedef std::map<OdString, svg::Font*> FontMap;
  FontMap _fontMap;
  SvgPropertiesPtr _properties;
  OdString _clipId;
  int _filterCount;
  int _clipCount;
  int _imageNum;

public:
  OdSvgDevice() : _svgRoot( 0 )
    , _currentView( 0 )

    , _currentNode( 0 )
    , _currentLineWeight( 1 )
    , _fill( false )
    , _properties( SvgProperties::createObject() )
    , _defs( 0 )
    , _filterCount( 0 )
    , _clipCount( 0 )
    , _imageNum( 0 )
  {
  }

  ODRX_DECLARE_MEMBERS( OdSvgDevice );
  
  ~OdSvgDevice()
  {
  }

  OdRxDictionaryPtr properties() { return _properties; }
  
  void setupSimplifier( const OdGiDeviation* dev )
  {
    OdGiGeometrySimplifier::setDeviation( dev );
  }

  virtual void update( OdGsDCRect* pUpdatedRect = 0 )
  {
    double lws = _properties->get_LineWeightScale();
    // set default lw scaling
    if ( lws == 0 ) _properties->put_LineWeightScale( 
      (m_outputRect.m_max.x - m_outputRect.m_min.x) / 800 );
    _svgRoot = new svg::RootNode( m_outputRect, _properties->get_BackGround() );
    _currentNode = _svgRoot;
    OdGsBaseVectorizeDevice::update( pUpdatedRect );
    flushFonts();
    ODA_ASSERT( _properties->get_Output() ); 
    // "Output" property must be set before exporting
    OdStreamBufPtr stream = _properties->get_Output();
    writeFile( *stream.get() );
    delete _svgRoot;
    _currentNode = _svgRoot = 0;
  }
  
  void flushFonts()
  {
    for ( FontMap::iterator i = _fontMap.begin(); i != _fontMap.end(); ++i )
    {
      addDefs();
      i->second->handOverTo( _defs );
      delete i->second;
    }
    _fontMap.clear();
  }

  void writeFile( OdStreamBuf& stream )
  {
    stream << OdString( "<?xml version=\"1.0\" standalone=\"no\"?>\r\n" );
    stream << *_svgRoot;
  }
  void addDefs()
  {
    if ( _defs ) return;
    _defs = new xml::Node( "defs" );
    _svgRoot->_children.insert( _svgRoot->_children.begin(), _defs );
  }

  OdGsViewPtr createView( const OdGsClientViewInfo* pViewInfo = 0, bool bEnableLayerVisibilityPerView = false )
  {
    OdGsViewPtr v = OdSvgView::createObject();
    static_cast<OdSvgView*>( v.get() )->init( pViewInfo, bEnableLayerVisibilityPerView );
    ((OdSvgView*)(OdGsView*)v)->screenOutput().setDestGeometry(*this);
    return v;
  }
 
  OdString createClipPath( OdUInt32 rings, const OdUInt32* counts, const OdGePoint2d* pp);

  void setClipRect(const OdGsDCPoint& p1, const OdGsDCPoint& p2 )
  {
    OdGePoint2d clipPoints[ 4 ] =
    {
      OdGePoint2d( p1.x, p1.y ),
      OdGePoint2d( p2.x, p1.y ),
      OdGePoint2d( p2.x, p2.y ),
      OdGePoint2d( p1.x, p2.y ) 
    };
    OdUInt32 size = 4;
    _clipId = createClipPath(1, &size, clipPoints );
    setCurrTraits( _fill, false, true );
  }
  
  void setClipPath( int rings, const int* counts, const OdGsDCPoint* pp )
  {
    OdGePoint2dArray clip;
    int cur = 0;
    for ( int i = 0; i < rings; i++)
      for ( int j = 0; j < counts[i]; j++ )
        clip.append( OdGePoint2d( pp[cur].x, pp[cur++].y ) );
    _clipId = createClipPath( rings, (const OdUInt32*)counts, clip.getPtr() );
    setCurrTraits( _fill, false, true );
  }
  
  void setCurrTraits( bool fill, bool shx = false, bool forceCreateGroup = false )
  {
    const OdGiSubEntityTraitsData& traits = drawContext()->effectiveTraits();

    ODCOLORREF color = traits.trueColor().isByColor() ?
      ODTOCOLORREF( traits.trueColor() ) : m_logPalette[ traits.color() ];
    
    double lw = shx ? _properties->get_ShxLineWeight() * _properties->get_LineWeightScale()
      : _properties->get_LineWeightScale() / 100 * (int)traits.lineWeight();
    if ( lw == 0 ) lw = 0.1 * _properties->get_LineWeightScale();

    if ( forceCreateGroup || _color != color || fill != _fill || lw != _currentLineWeight )
    {
      _color = color;
      _fill = fill;
      _currentLineWeight = lw;
      _currentNode = _svgRoot->addChild( "g" );
      _currentNode->addAttribute( "stroke-width", _currentLineWeight );
      if ( !_clipId.isEmpty() )  _currentNode->addAttribute( 
        "clip-path", OdString("url(#") + _clipId + ")");
      if ( _fill ) 
      {
        _currentNode->addAttribute( svg::Fill( _color ) );
        _currentNode->addAttribute( svg::Stroke() );
      }
      else
      {
        _currentNode->addAttribute( svg::Fill() );
        _currentNode->addAttribute( svg::Stroke( _color ) );
      }
    }
  }
  
  virtual void polylineOut( OdInt32 nbPoints, const OdGePoint3d* pVertexList )
  {
    setCurrTraits( false );
    _currentNode->addChild( new xml::Node( "polyline" ) )->addAttribute( 
      svg::Points( nbPoints, pVertexList ) );
  }
  
  virtual void polygonOut( OdInt32 nbPoints, const OdGePoint3d* pVertexList, const OdGeVector3d* )
  {
    setCurrTraits( drawContext()->effectiveTraits().fillType() == kOdGiFillAlways );
    xml::Node* node = _currentNode->addChild( new xml::Node( "polygon" ) );
    node->addAttribute( svg::Points( nbPoints, pVertexList ) );
  }
  
  virtual void shellFaceOut( OdInt32 faceListSize, 
    const OdInt32* pFaceList, const OdGeVector3d* )
  {
    setCurrTraits( true );
    svg::Path* path = new svg::Path;
    while ( faceListSize )
    {
      OdGiShellFaceIterator iter( vertexDataList(), pFaceList + 1 );
      int n = abs( (int)*pFaceList );

      OdGePoint3dArray points;
      for ( int i = 0; i < n; ++i, ++iter )
        points.append( *iter );
      path->addLoop( n, points.getPtr() );

      pFaceList += n + 1;
      faceListSize -= n + 1;
    }
    _currentNode->addChild( path );
  }

  virtual void circleProc( const OdGePoint3d& center, double radius, const OdGeVector3d& normal, const OdGeVector3d* pExtrusion = 0)
  {
    setCurrTraits( true );
    if ( pExtrusion )
    {
      OdGiGeometrySimplifier::circleProc( center, radius, normal, pExtrusion );
    }
    else _currentNode->addChild( new svg::Circle( center, radius ) );
  }
  
  virtual void circleProc( const OdGePoint3d& p1, const OdGePoint3d& p2, const OdGePoint3d& p3, const OdGeVector3d* pExtrusion = 0)
  {
    OdGeCircArc3d arc( p1, p2, p3 );
    circleProc( arc.center(), arc.radius(), OdGeVector3d::kZAxis, pExtrusion );
  }
  
  static void setSvgArcType( svg::Path* path, OdGiArcType arcType, const OdGePoint3d& center )
  {
    if ( arcType == kOdGiArcSector )
    {
      OdString s = " L";
      s += svg::Points::formatPoint( center );
      s += " z";
      path->getAttribute( "d" )->_value += s;
    }
    else if ( arcType == kOdGiArcChord )
      path->getAttribute( "d" )->_value += " z";
  }
  
  static svg::Path* formatArcPath( const OdGePoint3d& start, const OdGePoint3d& end, 
    double r1, double r2, double angle, bool largeArc , bool reversed )
  {
      svg::Path* path = new svg::Path;
      OdString s = "M"; // "M%g,%g A%g,%g %g %d,%d %g,%g",
      s += svg::Points::formatPoint( start );
      s += " A";
      s += xml::Attribute::formatDouble( r1 );
      s += ',';
      s += xml::Attribute::formatDouble( r2 );
      s += ' ';
      s += xml::Attribute::formatDouble( angle );
      s += largeArc ? " 1," : " 0,";
      s += reversed ? "0 " : "1 ";
      s += svg::Points::formatPoint( end );
      path->addAttribute( "d", s );
      return path;
  }
  
  virtual void circularArc( const OdGeCircArc3d& arc, OdGiArcType arcType = kOdGiArcSimple )
  {
    setCurrTraits( arcType != kOdGiArcSimple );
    if ( !arc.startPoint().isEqualTo( arc.endPoint() ) )
    {
      double a = arc.endAng() - arc.startAng();
      if ( a < 0 ) a += Oda2PI;
      svg::Path* path = formatArcPath( arc.startPoint(), arc.endPoint(),
        arc.radius(), arc.radius(), 0, a > OdaPI, false );
      setSvgArcType( path, arcType, arc.center() );
      _currentNode->addChild( path );
    }
    else
      _currentNode->addChild( new svg::Circle( arc.center(), arc.radius() ) );
  }
  
  virtual void circularArcProc( const OdGePoint3d& center,
    double radius,
    const OdGeVector3d& normal,
    const OdGeVector3d& startVector,
    double sweepAngle,
    OdGiArcType arcType = kOdGiArcSimple, const OdGeVector3d* pExtrusion = 0)
  {
    if ( pExtrusion ) 
      OdGiGeometrySimplifier::circularArcProc( center, radius, normal, startVector, sweepAngle, arcType, pExtrusion );
    else circularArc( OdGeCircArc3d( 
      center, normal, startVector, radius, 0., sweepAngle ), arcType);
  }
  
  virtual void circularArcProc( const OdGePoint3d& start,
    const OdGePoint3d& point,
    const OdGePoint3d& end,
    OdGiArcType arcType = kOdGiArcSimple, const OdGeVector3d* pExtrusion = 0)
  {
    if ( pExtrusion ) 
      OdGiGeometrySimplifier::circularArcProc( start, point, end, arcType, pExtrusion );
    else
     circularArc( OdGeCircArc3d( start, point, end ), arcType );
  }
  
  virtual void ellipArcProc(const OdGeEllipArc3d& arc,
    const OdGePoint3d* pEndPointsOverrides = 0,
    OdGiArcType arcType = kOdGiArcSimple, const OdGeVector3d* pExtrusion = 0)
  {
    if ( pExtrusion || OdZero( arc.minorRadius() ) || OdZero( arc.majorRadius() )
      || arc.majorAxis().isParallelTo( arc.minorAxis() ) ) 
    {
      OdGiGeometrySimplifier::ellipArcProc( arc, pEndPointsOverrides, arcType, pExtrusion );
      return;
    }
    setCurrTraits( arcType != kOdGiArcSimple );
    OdGeEllipArc3d correctArc( arc );
    OdGe::ErrorCondition ec;
    correctArc.correctAxis( OdGeContext::gTol, ec );
    if ( ec != OdGe::kOk )
    {
      OdGiGeometrySimplifier::ellipArcProc( arc, pEndPointsOverrides, arcType, pExtrusion );
      return;
    }
    double angle = correctArc.majorAxis().x == 0 ? 90 
      : atan( correctArc.majorAxis().y / correctArc.majorAxis().x ) * 180 / OdaPI;
    // bug in adobe viewer workaround
    const double MIN_ADOBE_ROT_ANGLE = 1E-10;
    if ( fabs( angle ) < MIN_ADOBE_ROT_ANGLE ) angle = 0; 
    if ( !correctArc.endPoint().isEqualTo( correctArc.startPoint() ) )
    {
      double a = correctArc.endAng() - correctArc.startAng();
      if ( a < 0 ) a += Oda2PI;
      
      svg::Path* path = formatArcPath( 
        pEndPointsOverrides ? pEndPointsOverrides[0] : correctArc.startPoint(),
        pEndPointsOverrides ? pEndPointsOverrides[1] : correctArc.endPoint(),
        correctArc.majorRadius(), correctArc.minorRadius(), angle, a > OdaPI, 
        correctArc.majorAxis().crossProduct( correctArc.minorAxis() ).z < 0 );
      
      setSvgArcType( path, arcType, correctArc.center() );
      _currentNode->addChild( path );
    }
    else _currentNode->addChild( 
      new svg::Ellipse( correctArc.center(), correctArc.majorRadius(), correctArc.minorRadius(), angle ) );
  }

  OdDbTextIteratorPtr createTextIterator( OdString& text, bool raw, const OdGiTextStyle* pTextStyle )
  {
    return  OdDbTextIterator::createObject(text.c_str(), text.getLength(), raw,
      drawContext()->giContext().database()->getDWGCODEPAGE(), pTextStyle );
  }
  
  svg::Font* getBigFont( const OdGiTextStyle* pTextStyle )
  {
    if ( !pTextStyle->getBigFont() ) return 0;
    return getFont( pTextStyle->bigFontFileName());
  }

  svg::Font* getFont( const OdGiTextStyle* pTextStyle )
  {
    return getFont( pTextStyle->ttfdecriptor().fileName() );
  }

  svg::Font* getFont( const OdString& face)
  {
    OdString localFace = face.isEmpty() ? OdString("simplex.shx") : face;
    FontMap::iterator fi = _fontMap.find( localFace );
    if ( fi == _fontMap.end() )
    {
      svg::Font* font = new svg::Font( localFace );
      _fontMap[ localFace ] = font;
      return font;
    }
    else return fi->second;
  }
  
  virtual void textProc( const OdGePoint3d& position,
    const OdGeVector3d& u, const OdGeVector3d& v,
    const OdChar* msg, OdInt32 length, bool raw, const OdGiTextStyle* pTextStyle,
    const OdGeVector3d* pExtrusion = 0)
  {
    OdString text = length > 0 ? OdString( msg, length ) : OdString( msg );
    if ( pExtrusion || !svg::isUnicodeConvertible( text ) )
    {
      OdGiGeometrySimplifier::textProc( position, u, v, msg, length, raw, pTextStyle, pExtrusion );
      return;
    }
    xml::Node* node = new xml::Node( "text" );
    node->addAttribute( "transform", svg::formatTransformMatrix( position, u, v ) );
    if ( !pTextStyle->ttfdecriptor().typeface().isEmpty() )
    {
      setCurrTraits( true );
      OdString ff; 
      if ( pTextStyle->ttfdecriptor().typeface() == "SansSerif" ) // Adobe specific workaround
        ff = "sans-serif";
      else if ( !_properties->get_GenericFontFamily().isEmpty() ) 
        ff.format( "%s,\'%s\'", pTextStyle->ttfdecriptor().typeface().c_str(), _properties->get_GenericFontFamily().c_str() );
      else ff = pTextStyle->ttfdecriptor().typeface();
      node->addAttribute( "font-family", ff );
      if ( pTextStyle->ttfdecriptor().isItalic())
        node->addAttribute( "font-style", "italic" );
      if ( pTextStyle->ttfdecriptor().isBold() )
        node->addAttribute( "font-weight", "bold" );
    }
    else
    {
      setCurrTraits( false, true );
      OdDbTextIteratorPtr it = createTextIterator( text, raw, pTextStyle);
      svg::embedCharacters( 
        getFont( pTextStyle ), getBigFont(pTextStyle), it, pTextStyle );
    }
    svg::setTextAttributes( node, pTextStyle );
    
    svg::replaceXmlSpecials( text );
    OdDbTextIteratorPtr it = createTextIterator( text, raw, pTextStyle);
    svg::setContents( node, it, pTextStyle );
    
    if ( node->_contents.isEmpty() && node->_children.empty() )
      delete node;
    else
      _currentNode->addChild( node );
  }
  
  virtual void xlineProc( const OdGePoint3d& p1, const OdGePoint3d& p2 )
  {
    ODA_ASSERT(p1.isEqualTo( p2 ));
    OdGePoint3d pp[2] = { p1, p2 };
    polylineOut( 2, pp );
  }
  
  virtual void rayProc( const OdGePoint3d& p1, const OdGePoint3d& p2 )
  {
    ODA_ASSERT(p1.isEqualTo( p2 ));
    OdGePoint3d pp[2] = { p1, p2 };
    polylineOut( 2, pp );
  }
  
  virtual void nurbsProc( const OdGeNurbCurve3d& nurbs)
  {
    setCurrTraits( false );
    OdGePoint3dArray points;
    nurbs.getSamplePoints( nurbs.startParam(), nurbs.endParam(), 
      _properties->get_Tolerance(), points );
    polylineOut( points.size(), points.getPtr() );
  }

  // Generate unique file name in given directory.
  //
  OdString generateFileName( const OdString& path, const OdChar* ext )
  {
    OdString s;
    for (;;)
    {
       s.format( "image%d%s", _imageNum++, ext );
       if ( !odSystemServices()->accessFile( path + s, 0 ) ) break;
    }
    return s;
  }

  static OdString createImageTransform( const OdGiRasterImage* pImg 
    ,const OdGePoint3d& origin
    ,const OdGeVector3d& u
    ,const OdGeVector3d& v
    )
  {
    OdUInt32 width(pImg->pixelWidth()), height(pImg->pixelHeight());
    OdGeVector2d u2( u.x * width, u.y * width );
    OdGeVector2d v2( -v.x * height, -v.y * height ); // image in svg is upside-down
    OdGePoint2d o2( origin.x, origin.y );
    OdGeMatrix2d m; m.setCoordSystem( o2 - v2, u2, v2 );
    OdString s; s.format("matrix(%g %g %g %g %g %g)", 
      m.entry[0][0], m.entry[1][0], m.entry[0][1], m.entry[1][1]
      , m.entry[0][2], m.entry[1][2]);
     return s;
  }
  
  OdString createImageClipPath( const OdGiRasterImage* pImg,
    OdUInt32 numBoundPts, const OdGePoint2d* uvBoundary )
  {
    OdGePoint2dArray clipPoints( numBoundPts );
    OdUInt32 width(pImg->pixelWidth()), height(pImg->pixelHeight());
    for ( OdUInt32 i = 0 ; i < numBoundPts; i++ )
    {
      // image is upside-down, coordinates are in uv space - transform to 0-1 square
      clipPoints.push_back( 
        OdGePoint2d( uvBoundary[i].x / width, 1 - uvBoundary[i].y / height ) );
    }
    return createClipPath( 1, &numBoundPts, clipPoints.getPtr() );
  }
  
  xml::Node* createFilter( OdString& id );

  OdString copyImage( const OdGiRasterImage* pImg, const OdString& path, bool transparency )
  {
    try
    {
      OdStreamBufPtr inFile = odSystemServices()->createFile( path );
      if ( inFile.isNull() ) return path;
      OdString newPath = _properties->get_ImageBase();
      int slash = odmax( path.reverseFind( '/' ), path.reverseFind( '\\' ) );
      OdString newName = path.right( path.getLength() - slash - 1 );
      // svg does not support bmp images
      OdString ext = path.right(4);
      ext.makeLower();
      bool convert = false;
      if ( ext == ".bmp" )
      {
        convert = true;
        ext = _properties->get_DefaultImageExt();
        newName.deleteChars( newName.getLength() - 4 , 4 );
        newName += ext;
      }
      
      // if such a file exist
      if ( odSystemServices()->accessFile( newPath + newName, 0 ) )
        newName = generateFileName( _properties->get_ImageBase(), ext );

      if ( convert )
      {
        OdRxDynamicLinkerPtr pDynLnk = odrxDynamicLinker();
        OdUInt32 rasterId = pDynLnk->loadApp(RX_RASTER_SERVICES_APPNAME);
        OdRxRasterServicesPtr pRasSvcs;
        if(rasterId)
        {
          pRasSvcs = pDynLnk->module(rasterId);
        }

        if ( pRasSvcs.get() && pRasSvcs->saveRasterImage( 
          pImg, newPath + newName, transparency ? 0 : -1 ) )
        {
          return _properties->get_ImageUrl() + newName;
        }
        else
        {
          newName.deleteChars( newName.getLength() - 4 );
          newName += ".bmp";
        }
      }
      // copy to new path
      OdStreamBufPtr outFile = odSystemServices()->createFile( newPath + newName, Oda::kFileWrite );
      inFile->copyDataTo( outFile );
      return _properties->get_ImageUrl() + newName;
    }
    catch( const OdError_CantOpenFile& ex )
    {
      OdString s(ex.description()); // prevent warnings in release build
      ODA_TRACE0(s);
      return path;
    }
    catch(...)
    {
      ODA_FAIL();
      return path;
    }
  }

  virtual void rasterImageProc(
    const OdGePoint3d& origin,
    const OdGeVector3d& u,
    const OdGeVector3d& v,
    const OdGiRasterImage* pImg, // image object
    const OdGePoint2d* uvBoundary, // may not be null
    OdUInt32 numBoundPts,
    bool transparency = false,
    double brightness = 50.0,
    double contrast = 50.0,
    double fade = 0.0 )
  {
    if ( !svg::isValidImage( pImg ) ) return;
    const OdGiDrawableDesc* pDesc = drawContext()->currentDrawableDesc();
    OdDbRasterImagePtr dbImg;
    if ( pDesc && pDesc->persistId )
      dbImg = OdDbRasterImage::cast( OdDbObjectId( pDesc->persistId ).safeOpenObject() );
    OdString path;
    if ( dbImg.isNull() )
    {
      if ( _properties->get_ImageBase().isEmpty() ) return;
      path = generateFileName( _properties->get_ImageBase(), _properties->get_DefaultImageExt() );
      OdRxDynamicLinkerPtr pDynLnk = odrxDynamicLinker();
      OdUInt32 rasterId = pDynLnk->loadApp(RX_RASTER_SERVICES_APPNAME);
      OdRxRasterServicesPtr pRasSvcs;
      if(rasterId)
      {
        pRasSvcs = pDynLnk->module(rasterId);
      }
      if ( !pRasSvcs.get() || !pRasSvcs->saveRasterImage( pImg, 
        _properties->get_ImageBase() + path, transparency ? 0 : -1 ) ) return;
    }
    else
    {
      OdDbRasterImageDefPtr pImgDef = dbImg->imageDefId().safeOpenObject();
      path = pImgDef->sourceFileName();
      if ( !_properties->get_ImageBase().isEmpty() )
        path = copyImage( pImg, path, transparency );
    }
    xml::Node* imgNode = _currentNode->addChild( new svg::Image( path ) );
    imgNode->addAttribute( "transform", createImageTransform( pImg, origin, u, v ) );
    if ( fade > 0 ) imgNode->addAttribute( "opacity", 1 - fade / 100 );
    if ( uvBoundary && numBoundPts > 2 )
    {
      OdString clipId = createImageClipPath( pImg, numBoundPts, uvBoundary );
      imgNode->addAttribute( "clip-path", OdString("url(#") + clipId + ")" );
    }
    if ( brightness != 50 || contrast != 50 )
    {
      OdString id;
      xml::Node* filter = createFilter( id );
      if ( brightness != 50 ) svg::applyGamma( filter, brightness );
      if ( contrast != 50 ) svg::applyContrast( filter, contrast );
      imgNode->addAttribute( "filter", OdString("url(#") + id + ")" );
    }
  }
};

ODRX_CONS_DEFINE_MEMBERS(OdSvgDevice,OdGsBaseVectorizeDevice,RXIMPL_CONSTR);

inline OdSvgDevice* OdSvgView::device() { return static_cast<OdSvgDevice*>(OdGsBaseVectorizeView::device()); }


// Inlined functions containing static variables cause warnings on the mac (bad semantics), 
// so move createClipPath and createFilter down to here.

OdString OdSvgDevice::createClipPath( OdUInt32 rings, const OdUInt32* counts, const OdGePoint2d* pp )
{
  addDefs();
  OdString clipId; clipId.format( "clipId%d", _clipCount++ );
  xml::Node* clipPath = _defs->addChild( new xml::Node( "clipPath" ) );
  clipPath->addAttribute( "id", clipId );
  svg::Path* path = new svg::Path;
  clipPath->addChild( path );
  int currentPos = 0;
  for ( OdUInt32 i = 0; i < rings; i++ )
  {
    path->addLoop( counts[i], pp + currentPos );
    currentPos += counts[i];
  }
  return clipId;
}

xml::Node* OdSvgDevice::createFilter( OdString& id )
{
  addDefs();
  xml::Node* filter = _defs->addChild( "filter" );
  id.format( "filterId%d", _filterCount++ );
  filter->addAttribute( "id", id );
  return filter;
}

void OdSvgView::setupDeviations()
{
  OdGeDoubleArray devs( 5 );
  devs.insert( devs.end(), 5, device()->_properties->get_Tolerance() );
  _pViewportClip->setDeviation( devs );
  _pViewportClip->setDrawContext( drawContext() );
  _pViewportClip->input().removeSourceNode(output());
  _pViewportClip->input().addSourceNode(output());
  _pPerspPrepr->setDeviation( &m_pModelToEyeProc->eyeDeviation() );
  device()->setupSimplifier( &m_pModelToEyeProc->eyeDeviation() );
}

void OdSvgView::setupPerspective()
{
  bool isFrontClp(isFrontClipped());
  bool isBackClp(isBackClipped());
  
  double frClp(frontClip());
  double bkClp(backClip());
  
  if(isFrontClp)
    frClp = odmin(frClp, focalLength() * 0.9);
  else
    frClp = focalLength() * 0.9;
  
  _pViewportClip->set( 0, 0, isBackClp, bkClp, true, frClp );
  _pViewportClip->setDrawContext(drawContext());
  
  _pPerspPrepr->setDrawContext(drawContext());
  _pXYProjector->output().setDestGeometry(*device());
  _pXYProjector->setTransform(OdGeMatrix3d::projection(OdGePlane::kXYPlane, OdGeVector3d::kZAxis));
  _pXYProjector->setTransform( eyeToScreenMatrix() );
}

void OdSvgView::setupSpatialFilter( const OdGeMatrix3d& xEyeToDc )
{
  OdGePoint3d p1(-fieldWidth()/2,-fieldHeight()/2,0);
  OdGePoint3d p2(fieldWidth()/2,fieldHeight()/2,0);
  p1.transformBy(xEyeToDc);
  p2.transformBy(xEyeToDc);
  _pSpatialFilter->set(OdGeExtents2d(
    OdGePoint2d(odmin(p1.x,p2.x),odmin(p1.y,p2.y)),
    OdGePoint2d(odmax(p1.x,p2.x),odmax(p1.y,p2.y))
    ));
  _pSpatialFilter->setDrawContext(drawContext());
}

void OdSvgView::setupConveyor()
{
  if ( isPerspective() )
  {
    setupPerspective();
    return;
  }

  OdGeMatrix3d xEyeToDc = eyeToScreenMatrix();
  if ( isFrontClipped() || isBackClipped() )
  {
    OdGeVector3d zAxis = OdGeVector3d::kZAxis;
    OdGeVector3d dcFront( 0., 0., frontClip() );
    OdGeVector3d dcBack( 0., 0., backClip() );
    dcFront.transformBy( xEyeToDc );
    dcBack.transformBy( xEyeToDc );
    zAxis.transformBy( xEyeToDc );
    if ( zAxis.z > 0. )
    {
      _pViewportClip->set( 0, NULL, isBackClipped(), dcBack.z, isFrontClipped(), dcFront.z );
    }
    else
    {
      _pViewportClip->set( 0, NULL, isFrontClipped(), dcFront.z, isBackClipped(), dcBack.z );
    }
    _pXYProjector->setTransform( 
      OdGeMatrix3d::projection( OdGePlane::kXYPlane, OdGeVector3d::kZAxis ) );
  }
  else // disable 
  {
    _pViewportClip->set( 0, 0, false, 0., false, 0. );
    _pXYProjector->setTransform( OdGeMatrix3d::kIdentity );

    xEyeToDc.preMultBy( OdGeMatrix3d::projection( OdGePlane::kXYPlane, OdGeVector3d::kZAxis ) );
  }
  
  setupSpatialFilter( xEyeToDc );

  setEyeToOutputTransform( xEyeToDc );
}

void OdSvgView::setOuterClipPath()
{
  if ( m_nrcContours )
  {
    device()->setClipPath( 
      m_nrcContours, m_nrcCounts.getPtr(), m_nrcPoints.getPtr() );
  }
  else
  {
    OdGsDCPoint p1, p2; screenRect( p1, p2 );
    device()->setClipRect( p1, p2 );
  }
}

void OdSvgView::setView( const OdGePoint3d & position,
  const OdGePoint3d& target, const OdGeVector3d& upVector,
  double fieldWidth, double fieldHeight,
  Projection projection )
{
  OdGsBaseVectorizeView::setView( position, target, upVector, fieldWidth, fieldHeight, projection );
  
  if ( projection == kPerspective )
  {
    _pPerspPrepr->input().removeSourceNode( _pViewportClip->output() );
    _pPerspPrepr->input().addSourceNode( _pViewportClip->output() );
    _pXYProjector->input().removeSourceNode( _pPerspPrepr->output() );
    _pXYProjector->input().addSourceNode( _pPerspPrepr->output() );
    _pViewportClip->setDeviation( &m_pModelToEyeProc->eyeDeviation() );
  }
}

void OdSvgView::update()
{
  if(abs(m_dcScreenMax.x-m_dcScreenMin.x) > 0 && abs(m_dcScreenMax.y-m_dcScreenMin.y) > 0)
  {
    device()->setDrawContext( drawContext() );
    setupDeviations();
    setupConveyor();
    setOuterClipPath();
    OdGsBaseVectorizeView::update();
  }
}


extern "C" SVGEXPORT_API void SvgExportModuleInit()
{
  OdSvgDevice::rxInit();
}

extern "C" SVGEXPORT_API void SvgExportModuleUnInit()
{
  OdSvgDevice::rxUninit();
}

extern "C" SVGEXPORT_API OdRxClass* createSvgDevice()
{
  ODA_ASSERT( OdSvgDevice::desc() );
  return OdSvgDevice::desc();
}

#ifdef _TOOLKIT_IN_DLL_
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
BOOL APIENTRY DllMain( HANDLE , DWORD  , LPVOID )
{
  return TRUE;
}
#endif
