#ifndef _SVG_WRAPPERS_H_INCLUDED_
#define _SVG_WRAPPERS_H_INCLUDED_

#if _MSC_VER > 1
#pragma once 
#endif

OdStreamBuf& operator << ( OdStreamBuf& stream, const OdString& s )
{
  stream.putBytes( (const OdChar*)s, s.getLength() );
  return stream;
}

namespace xml
{
  OdStreamBuf& operator << ( OdStreamBuf& stream, char c )
  {
    stream.putByte( c );
    return stream;
  }

  struct Attribute
  {
    OdString _name;
    OdString _value;
    Attribute( const OdString& name = "", const OdString& value = "" ) 
      : _name( name ), _value( value ) {}
    static OdString _doubleFormat;
    static OdString formatDouble( double d )
    {
      OdString s; s.format( _doubleFormat, d );
      return s;
    }
  };

  OdStreamBuf& operator << ( OdStreamBuf& stream, const Attribute& a )
  {
    stream << a._name << '=' << '\"' << a._value << '\"' << ' ';
    return stream;
  }

  struct Node
  {
    OdString _name;
    OdString _contents;
    typedef std::vector<Attribute> AttributeMap;
    AttributeMap _attributes;
    std::vector<Node*> _children;
    Node( const char* name ) : _name( name ) {}
    virtual ~Node()
    {
      for ( unsigned int i = 0; i < _children.size(); i++ ) delete _children[i];
    }
    Node* addChild( Node* child )
    {
      if ( child ) _children.push_back( child );
      return child;
    }
    Node* addChild( const char* name )
    {
      if ( name ) return addChild( new Node( name ) );
      else return 0;
    }
    AttributeMap::iterator addAttribute( const Attribute& a )
    {
      _attributes.push_back( a );
      return _attributes.end() - 1;
    }
    AttributeMap::iterator addAttribute( const OdString& name, const OdString& value = "" )
    {
      return addAttribute( Attribute( name, value ) );
    }
    AttributeMap::iterator addAttribute( const OdString& name, double value )
    {
      Attribute a( name );
      a._value = a.formatDouble( value );
      return addAttribute( a );
    }
    Attribute* getAttribute( const OdString& name )
    {
      AttributeMap::iterator p = _attributes.begin();
      for (; p != _attributes.end(); ++p ) if ( p->_name == name ) break;
      if ( p == _attributes.end() ) p = addAttribute( Attribute( name ) );
      return &(*p);
    }
    void removeAttribute( const OdString& name )
    {
      AttributeMap::iterator p = _attributes.begin();
      for (; p != _attributes.end(); ++p ) if ( p->_name == name ) break;
      if ( p == _attributes.end() ) return;
      _attributes.erase( p );
    }
  };
  
  OdStreamBuf& operator << ( OdStreamBuf& stream, const Node& node )
  {
    // open tag
    stream << '<' << node._name << ' ';
    // write attributes
    Node::AttributeMap::const_iterator i = node._attributes.begin(); 
    for ( ; i != node._attributes.end(); ++i ) stream << *i;
    // if no contents - close
    if ( node._contents.isEmpty() && node._children.empty() )
      stream << '/' << '>' << '\r' << '\n';
    else
    {
      stream << '>' << '\r' << '\n';
      // contents
      if ( !node._contents.isEmpty() ) stream << node._contents << '\r' << '\n';
      for ( unsigned int i = 0; i < node._children.size(); i++ )
        stream << *node._children[i];
      stream << '<' << '/' << node._name << '>' << '\r' << '\n';
    }
    return stream;
  };
}

namespace svg
{
  inline OdString formatColor( ODCOLORREF color )
  {
    OdString s; s.format( "rgb(%d,%d,%d)", 
      ODGETRED( color ), ODGETGREEN( color ), ODGETBLUE( color ) );
    return s;
  }
  
  struct Stroke : xml::Attribute
  {
    Stroke() : xml::Attribute( "stroke", "none" ) {};
    Stroke( const OdString& s) : xml::Attribute( "stroke", s ) {};
    Stroke( ODCOLORREF color ) : xml::Attribute( "stroke", formatColor( color ) ) {};
  };
 
  struct Fill : xml::Attribute
  {
    Fill( ODCOLORREF color ) : xml::Attribute( "fill", formatColor( color ) ) {}
    Fill() : xml::Attribute( "fill", "none" ) {}
  };

  struct Points : xml::Attribute
  {
    static OdString formatPoint(double x, double y)
    {
      OdString s = formatDouble( x );
      s += ',';
      s += formatDouble( y );
      s += ' ';
      return s;
    }

    // Solaris complains if these are combined into a template function.
    static OdString formatPoint( const OdGePoint2d& p )
    {
      return formatPoint(p.x, p.y);
    }
    
    static OdString formatPoint( const OdGePoint3d& p )
    {
      return formatPoint(p.x, p.y);
    }

    Points( OdUInt32 nPts, const OdGePoint3d* points ) : xml::Attribute( "points" )
    {
      for ( OdUInt32 i = 0; i < nPts; i++ )
        _value += formatPoint( points[i] );
      // if single point - append it twice
      if ( nPts == 1 ) _value += formatPoint( points[0] ); 
    }
  };
 
  struct Path : xml::Node
  {
    Path( const OdString& name = "path" ) : xml::Node( name ) {}
    template <class P> void addLine( int nCount, const P* points )
    {
      OdString loop( "M" );
      for ( int i = 0; i < nCount; i++ )
        loop += Points::formatPoint( points[i] );
      getAttribute( "d" )->_value += loop;
    }
    template <class P> void addLoop( int nCount, const P* points )
    {
      addLine( nCount, points );
      getAttribute( "d" )->_value += "z";
    }
  };
  
  struct Circle : xml::Node
  {
    Circle( const OdGePoint3d& center, double radius ) : xml::Node( "circle" )
    {
      addAttribute( "cx", center.x );
      addAttribute( "cy", center.y );
      addAttribute( "r", radius );
    }
  };

  struct Ellipse : xml::Node
  {
    Ellipse( const OdGePoint3d& center, double rx, double ry, double angle = 0.0 ) 
      : xml::Node( "ellipse" )
    {
      addAttribute( "rx", rx );
      addAttribute( "ry", ry );
      if ( angle != 0.0 )
      {
        OdString s = "translate(";
        s += xml::Attribute::formatDouble( center.x );
        s += ' ';
        s += xml::Attribute::formatDouble( center.y );
        s += ") rotate(";
        s += xml::Attribute::formatDouble( angle );
        s += ')';
        addAttribute( "transform", s );
      }
      else
      {
        addAttribute( "cx", center.x );
        addAttribute( "cy", center.y );
      }
    }
  };
 
  struct Image : xml::Node
  {
    Image( const OdString& path) : xml::Node( "image" )
    {
      addAttribute( "width", "1" );
      addAttribute( "height", "1" );
      addAttribute( "xlink:href", path );
    }
  };
  
  struct RootNode : xml::Node
  {
    RootNode( const OdGsDCRect& rect, ODCOLORREF bkColor ) : xml::Node( "svg" )
    {
      OdString s; s.format( "%d %d %d %d", 
        rect.m_min.x, rect.m_max.y, rect.m_max.x, rect.m_min.y );
      addAttribute( "viewBox", s );
      addAttribute( "version",          "1.1" );
      addAttribute( "xmlns",            "http://www.w3.org/2000/svg" );
      addAttribute( "stroke-linecap",   "round" );
      addAttribute( "stroke-linejoin",  "round" );
      addAttribute( "fill-rule",        "evenodd" );
      addAttribute( "xml:space",        "preserve" );
      if ( bkColor != ODRGB( 0xff, 0xff, 0xff ) )
      {
        xml::Node* bk = addChild( "rect" );
        bk->addAttribute( "width", "100%" );
        bk->addAttribute( "height", "100%" );
        bk->addAttribute( Fill( bkColor ) );
      }
    }
  };
}

#endif
