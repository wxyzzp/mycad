namespace svg
{
  // Scans for special symbols with side-effects (tolerance divider).
  //
  inline bool isUnicodeConvertible( const OdString& text )
  {
    return text.find("%%v") == -1 && text.find("%%V") == -1;
  }
  
  // Set style attribute to <text> or <tspan> node
  //
  void addStyle( xml::Node* node, const OdGiTextStyle* pTextStyle, bool bigFont )
  {
    OdString s; if ( bigFont && !pTextStyle->bigFontFileName().isEmpty() )
      s.format( "font-family: %s", pTextStyle->bigFontFileName().c_str() );
    else 
      s.format( "font-family: %s", pTextStyle->ttfdecriptor().fileName().c_str() );
    // adobe viewer does not allow font-family to have a . inside
    if ( s.right(4) == ".shx" ) s = s.left( s.getLength() - 4 );
    node->addAttribute( "style", s );
  }
 
  // Add <tspan> element to <text> node
  //
  void addTspan( xml::Node* text, const OdString& s, const OdGiTextStyle* pTextStyle, bool bigFont )
  {
    if ( s.isEmpty() ) return;
    xml::Node* ts = text->addChild( new xml::Node( "tspan" ) );
    ts->_contents = s;
    addStyle( ts, pTextStyle, bigFont );
  }
  
  // Set text content, converting special sequences
  // if font is switched - tspans are added
  //
  void setContents( xml::Node* text, OdDbTextIteratorPtr& iter, const OdGiTextStyle* pTextStyle )
  {
    OdString res;
    bool bigFont = false;
    do
    {
      OdUInt16 ch = iter->nextChar();
      if ( ch == 0 ) break;
      if ( bigFont != iter->currProperties().bInBigFont )
      {
        addTspan( text, res, pTextStyle, bigFont );
        bigFont = iter->currProperties().bInBigFont;
        res = "";
      }
      if ( ch < 128 ) res += (char)ch;
      else 
      {
        OdString u; u.format( "&#%d;", ch );
        res += u;
      }
    }
    while ( !iter->currProperties().bLastChar );
    // if font did not switch - set contents directly
    if ( text->_children.empty() && !res.isEmpty() )
    {
      text->_contents = res;
      // set shx font style
      if ( pTextStyle->ttfdecriptor().typeface().isEmpty() )
        addStyle( text, pTextStyle, bigFont );
    }
    else addTspan( text, res, pTextStyle, bigFont );
  }
  
  // Replace xml special characters '&','"', etc.
  //
  inline static void replaceXmlSpecials( OdString& text )
  {
    text.replace( "&", "&amp;" );
    text.replace( "\"", "&quot;" );
    text.replace( "<", "&lt;" );
    text.replace( ">", "&gt;" );
  }

  // Calculate SVG ttf font size, corresponding to this textstyle.
  // This formula contradicts documentation, but is verified by practics.
  //
  inline static double textHeight( const OdGiTextStyle* pTextStyle )
  {
    return ( pTextStyle->getFont()->getHeight() 
      - pTextStyle->getFont()->getInternalLeading() )
      / pTextStyle->getFont()->fontAbove();
  }
  
  // <glyph> svg element (shx font glyph)
  //
  struct Glyph : Path
  {
    Glyph( OdUInt16 n ) : Path( "glyph" )
    {
      OdString s;
      if ( n < 128 )
      {
        s.format( "%c", (char)n );
        replaceXmlSpecials( s );
      }
      else s.format( "&#%d;", n );
      addAttribute( "unicode", s );
    }
  };

  // SHX glyph geometry writer
  //
  class GlyphWriter : public OdGiGeometrySimplifier
  {
    svg::Glyph* _glyph;
  public:
    GlyphWriter( OdUInt16 n ) : _glyph( new svg::Glyph( n ) ){}
    ~GlyphWriter(){ delete _glyph; }
    Glyph* handOverTo( xml::Node* font )
    {
      Glyph* ret = static_cast<Glyph*>( font->addChild( _glyph ) );
      _glyph = 0;
      return ret;
    }
    virtual void polylineOut( OdInt32 nbPoints, const OdGePoint3d* pVertexList )
    {
      _glyph->addLine( nbPoints, pVertexList );
    }

    void circleProc(const OdGePoint3d& center,
      double radius, const OdGeVector3d& normal,
      const OdGeVector3d* pExtrusion = 0)
    {
      if ( !pExtrusion )
      {
        _glyph->addChild( new svg::Circle( center, radius ) );
      }
      else
        OdGiGeometrySimplifier::circleProc( center, radius, normal, pExtrusion );
    }

    DD_USING(OdGiGeometrySimplifier::circleProc); 
    
    void circularArcProc(const OdGePoint3d& start,
      const OdGePoint3d& point,
      const OdGePoint3d& end,
      OdGiArcType arcType = kOdGiArcSimple,
      const OdGeVector3d* pExtrusion = 0)
    {
      if ( pExtrusion || arcType != kOdGiArcSimple )
      {
        OdGiGeometrySimplifier::circularArcProc( start, point, end, arcType, pExtrusion );
      }
      else
      {
        OdGeCircArc3d arc( start, point, end );
        OdString s; s.format( "M%g,%g A%g,%g 0 0,0 %g,%g",
          arc.startPoint().x, arc.startPoint().y, arc.radius(), arc.radius(), 
          arc.endPoint().x, arc.endPoint().y );
        _glyph->getAttribute( "d" )->_value += s;
      }
    }

    DD_USING(OdGiGeometrySimplifier::circularArcProc);
  };
  
  // Embedded SHX font (<font> element)
  //
  class Font
  {
    friend class GlyphWriter;
    std::set<OdUInt16> _glyphs;
    OdString _name;
    xml::Node* _font;
  public:
    Font( const OdString& name ) 
      : _font( new xml::Node( "font" ) )
      , _name( name )
    {
      // font-family must differ from <font> id
      xml::Node* fontFace = _font->addChild( new xml::Node( "font-face" ) );
      if ( _name.right(4) != ".shx" )
        _name += ".shx";
      _font->addAttribute( "id", _name.mid( 0, _name.getLength() - 4 ) );
      fontFace->addAttribute( "font-family", _name );
    }
    ~Font(){ delete _font; }
    void handOverTo( xml::Node* defs )
    {
      // do not add empty font
      if ( _font->_children.size() > 1 )
        defs->addChild( _font );
      else delete _font;
      _font = 0;
    }
    bool has( OdUInt16 n ) const
    {
      return _glyphs.find( n ) != _glyphs.end();
    }
    Glyph* addGlyph( GlyphWriter* gw, OdUInt16 n )
    {
      ODA_ASSERT( _font && !has( n ) );
      _glyphs.insert( n );
      return gw->handOverTo( _font );
    }
  };

  // Embed one character glyph to svg font
  //
  void embedCharacter( svg::Font* font, OdFont* shx, OdUInt16 ch )
  {
    ODA_ASSERT( font != 0 );
    if ( font->has( ch ) ) return;
    svg::GlyphWriter gw( ch );
    OdGePoint2d advance;
    OdTextProperties tp;
    tp.setIncludePenups( false );
    tp.setLastChar( false );
    tp.setTrackingPercent( 1 );
    // draw geometry
    shx->drawCharacter( ch, advance, &gw, tp );
    svg::Glyph* g = font->addGlyph( &gw, ch );
    // calculate horz-advance
    tp.setIncludePenups( true );
    shx->drawCharacter( ch, advance, &gw, tp );
    g->addAttribute( "horiz-adv-x", advance.x );
    tp.setVerticalText( true );
    // calculate vert-advance
    shx->drawCharacter( ch, advance, &gw, tp );
    g->addAttribute( "vert-adv-y", -advance.y );
  }
 
  // Iterate through string, and embed missing characters in svg font
  //
  void embedCharacters( svg::Font* font, svg::Font* bigfont, 
    OdDbTextIteratorPtr& it, const OdGiTextStyle* pTextStyle )
  {
    do
    {
      OdUInt16 ch = it->nextChar();
      if ( ch == 0 ) break;
      if ( it->currProperties().bInBigFont )
        embedCharacter( bigfont, pTextStyle->getBigFont(), ch );
      else
        embedCharacter( font, pTextStyle->getFont(), ch );
    }
    while( !it->currProperties().bLastChar );
  }

  // Set text attributes equal for ttf and shx
  //
  void setTextAttributes( xml::Node* node, const OdGiTextStyle* pTextStyle )
  {
    if ( pTextStyle->isVertical() )
    {
      node->addAttribute( "writing-mode", "tb" );
      node->addAttribute( "glyph-orientation-vertical", 
        pTextStyle->isUpsideDown() ? "180" : "0" );
    }
    else
    {
      if ( pTextStyle->isBackward() )
        node->addAttribute( "writing-mode", "rl" );
      if( pTextStyle->isUpsideDown() )
        node->addAttribute( "glyph-orientation-vertical", "180" );
    }
    if ( pTextStyle->trackingPercent() != 1 )
      node->addAttribute( "letter-spacing", pTextStyle->trackingPercent() - 1 );
    node->addAttribute( "font-size", svg::textHeight( pTextStyle ) );
  }
}
