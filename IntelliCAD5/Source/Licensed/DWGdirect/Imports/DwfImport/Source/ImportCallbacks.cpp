#include "StdAfx.h"
#include "DwfImportImpl.h"
#include "DwfCallbacks.h"
#include "DbSpatialFilter.h"
#include "DbIndex.h"

#include "DbEllipse.h"
#include "DbPoint.h"
#include "DbMText.h"
#include "DbBlockReference.h"
#include "Db2dPolyline.h"
#include "Db2dVertex.h"
#include "DbText.h"

namespace dwfImp
{
  // Convert WT_Unts to OdDb::UnitsValue
  // if does not recognize value, returns 0;
  //
  OdDb::UnitsValue convertUnits(const WT_Units& units)
  {
    static const char* const Units[] = 
    {
      "", /* 0	Unspecified (No units) */
        "Inches",
        "Feet",
        "Miles",
        "Millimeters",
        "Centimeters",
        "Meters",
        "Kilometers",
        "Microinches",
        "Mils",
        "Yards",
        "Angstroms",
        "Nanometers",
        "Microns",
        "Decimeters",
        "Decameters",
        "Hectometers",
        "Gigameters",
        "Astronomical Units",
        "Light Years",
        "Parsecs"
    };
    for (int i = 0; i < sizeof(Units)/sizeof(Units[0]); ++i)
      if (units.units() == Units[i]) 
        return static_cast<OdDb::UnitsValue>(i);
      return OdDb::kUnitsUndefined;
  }
}

// units & transformation matrix
//
WT_Result DwfCallbackManager::process_units (WT_Units & units, WT_File & file)
{
	file.rendition().drawing_info().units() = units;
  if (_importer->extent()._collectBounds) return WT_Result::Success;
  _importer->database()->setINSUNITS(dwfImp::convertUnits(units));
	return WT_Result::Success;
}

WT_Result DwfCallbackManager::process_named_view (WT_Named_View & namedView, WT_File&)
{
  if ( _importer->extent()._collectBounds ) return WT_Result::Success;
	WT_Logical_Box box = *namedView.view();
	OdGePoint3d maxpt = _importer->extent().transformPoint( box.maxpt() );
	OdGePoint3d minpt = _importer->extent().transformPoint( box.minpt() );
	OdDbViewTablePtr vt = _importer->database()->getViewTableId().safeOpenObject( OdDb::kForWrite );
  OdString name = DwfBlockManager::normalizeObjectName( namedView.name().ascii() );
	OdDbViewTableRecordPtr newView;
  if ( vt->has( name ) )
  {
    newView = vt->getAt( name ).safeOpenObject( OdDb::kForWrite );
  }
  else
  {
    newView = OdDbViewTableRecord::createObject();
    newView->setName( name );
    vt->add( newView );
  }
	newView->setHeight( maxpt.y - minpt.y );
	newView->setWidth( maxpt.x - minpt.x );
	newView->setCenterPoint( OdGePoint2d( ( maxpt.x + minpt.x ) / 2, ( maxpt.y + minpt.y ) / 2 ) );
  newView->setTarget( OdGePoint3d::kOrigin );
  newView->setLensLength( 50 );
  newView->setViewDirection( OdGeVector3d::kZAxis );
	newView->setIsPaperspaceView( true );
	return WT_Result::Success;
}

WT_Result DwfCallbackManager::process_view (WT_View & view, WT_File &)
{
  if ( _importer->extent()._collectBounds ) return WT_Result::Success;
  
  OdDbLayoutPtr layout = _importer->blocks().currentBlock()->getLayoutId().safeOpenObject(OdDb::kForWrite);
  if ( layout->getBlockTableRecordId() == _importer->database()->getModelSpaceId() )
    return WT_Result::Success;
	
  WT_Logical_Box box = view.view();
	OdGePoint3d maxpt = _importer->extent().transformPoint(box.maxpt());
	OdGePoint3d minpt = _importer->extent().transformPoint(box.minpt());
	OdDbObjectId vpId = layout->overallVportId();
	
	// if this is newly created layout, it may contain no overall viewport
	if ( vpId.isNull() ) vpId = _importer->blocks().addViewport();
	
	OdDbViewportPtr vp = vpId.safeOpenObject(OdDb::kForWrite);
	vp->setHeight(maxpt.y - minpt.y);
	vp->setWidth(maxpt.x - minpt.x);
	OdGePoint3d center((maxpt + minpt.asVector())/2);
	vp->setCenterPoint(center);
	vp->setViewHeight(vp->height());
	vp->setViewCenter(OdGePoint2d(center.x, center.y));
	layout->setActiveViewportId(vpId);
	return WT_Result::Success;
}


WT_Result DwfCallbackManager::process_filled_ellipse (WT_Filled_Ellipse& ellipse, WT_File& file)
{
  if (_importer->extent()._collectBounds) 
  {
    _importer->extent().updateBounds(ellipse, file);
    return WT_Result::Success;
  }
	OdDbHatchPtr pHatch = _importer->blocks().addHatch(file);
	OdGePoint3d c = _importer->extent().transformPoint(ellipse.position());
	OdGePoint2d center(c.x, c.y);
	double a = ellipse.tilt_radian();
	OdGeVector2d ax1(cos(a), sin(a));
	OdGeVector2d ax2(cos(a + OdaPI2), sin(a + OdaPI2));
	
	OdGeEllipArc2d* cirArc = new OdGeEllipArc2d(center, ax1, ax2,
		_importer->extent().transformSize(ellipse.ellmajor()),
		_importer->extent().transformSize(ellipse.ellminor()));

	EdgeArray edgePtrs;
	edgePtrs.push_back(cirArc);
	pHatch->appendLoop(OdDbHatch::kDefault, edgePtrs);
	return WT_Result::Success;
}

static WT_Result addPoint( DwfImporter* importer, const WT_Logical_Point& p, WT_File& file )
{
  WT_Logical_Point pp[2];
  pp[0] = pp[1] = p;
  WT_Point_Set pl( 2, pp, WD_False );
  importer->lines().addPolyline( pl, file );
  return WT_Result::Success;
}

static WT_Result addFlatEllipse( DwfImporter* importer, const WT_Ellipse& ell, WT_File& file )
{
  WT_Logical_Point pp[2];
  pp[0] = pp[1] = ell.position();
  int dx = int( cos( ell.tilt_radian() ) * ell.ellmajor() );
  int dy = int( sin( ell.tilt_radian() ) * ell.ellmajor() );
  pp[0].m_x -= dx;
  pp[0].m_y -= dy;
  pp[1].m_x += dx;
  pp[1].m_y += dy;
  WT_Point_Set pl( 2, pp, WD_False );
  importer->lines().addPolyline( pl, file );
  return WT_Result::Success;
}

WT_Result DwfCallbackManager::process_outline_ellipse(WT_Outline_Ellipse& ell, WT_File& file)
{
  if (_importer->extent()._collectBounds) 
  {
    _importer->extent().updateBounds( ell, file );
    return WT_Result::Success;
  }
  
  if ( ell.ellmajor() == 0 ) return addPoint( _importer, ell.position(), file );
  if ( ell.ellminor() == 0 ) return addFlatEllipse( _importer, ell, file );

	OdGePoint3d c = _importer->extent().transformPoint(ell.position());
  double a = ell.tilt_radian();
	double major = _importer->extent().transformSize(ell.ellmajor());
	double minor = _importer->extent().transformSize(ell.ellminor());
	if (major < minor)
	{
		std::swap(major, minor);
		a += OdaPI2;
	}
	OdDbEllipsePtr pEllipse = OdDbEllipse::createObject();
	_importer->blocks().addEntity(pEllipse, file);
  bool fullEllipse = ( ( ell.end() - ell.start() ) % 65536 ) == 0;
  double startAngle = fullEllipse ? 0. : ( Oda2PI * ell.start() / 65536. );
  double endAngle = fullEllipse ? Oda2PI : ( Oda2PI * ell.end() / 65536. );
	pEllipse->set(c, OdGeVector3d::kZAxis, // Center, normal,
		OdGeVector3d( cos(a)*major, sin(a)*major, 0 ), // Major axis,
		minor / major, startAngle, endAngle ); // Radius ratio , start & end angles
	return WT_Result::Success;
}


WT_Result DwfCallbackManager::process_image(WT_Image& image, WT_File& file)
{
  if (_importer->extent()._collectBounds) 
  {
    _importer->extent().updateBounds(image, file);
    return WT_Result::Success;
  }
	_importer->images().addImage(image.data(), image.data_size(), image.columns(),
		image.rows(), image.format(), image.min_corner(), image.max_corner(), 
		image.identifier(), file);
	return WT_Result::Success;
}

WT_Result DwfCallbackManager::process_png_group4_image(WT_PNG_Group4_Image& image, WT_File& file)
{
  if (_importer->extent()._collectBounds) 
  {
    _importer->extent().updateBounds(image, file);
    return WT_Result::Success;
  }
	_importer->images().addImage(image.data(), image.data_size(), image.columns(),
		image.rows(), image.format(), image.min_corner(), image.max_corner(), 
		image.identifier(), file);
	return WT_Result::Success;
}

WT_Result DwfCallbackManager::process_layer(WT_Layer& layer, WT_File& file)
{
  if (_importer->extent()._collectBounds) return WT_Result::Success;
  file.rendition().layer() = layer;
	_importer->blocks().setCurrentLayer(layer);
	return WT_Result::Success;
}

WT_Result DwfCallbackManager::process_object_node(WT_Object_Node& node, WT_File&)
{
  if (_importer->extent()._collectBounds) return WT_Result::Success;
	_importer->blocks().setCurrentGroup(node.object_node_num(), node.object_node_name().ascii());
	return WT_Result::Success;
}

WT_Result DwfCallbackManager::process_polygon(WT_Polygon& poly, WT_File& file)
{
  if (_importer->extent()._collectBounds) 
  {
    _importer->extent().updateBounds(poly, file);
    return WT_Result::Success;
  }
  // if degenerate geometry - do not make hatch
  if (poly.count() <= 2)
  {
    _importer->lines().addPolyline(poly, file);
    return WT_Result::Success;
  }
	
  OdDbHatchPtr pHatch = _importer->blocks().addHatch(file);
	OdGePoint2dArray vertexPts;
	OdGeDoubleArray vertexBulges;
	vertexPts.resize(poly.count());
	for (int i = 0; i < poly.count(); ++i)
	{
		OdGePoint3d p = _importer->extent().transformPoint(poly.points()[i]);
		vertexPts[i].set(p.x, p.y);
	}
	pHatch->appendLoop(OdDbHatch::kExternal | OdDbHatch::kPolyline, 
		vertexPts, vertexBulges);
	return WT_Result::Success;
}

WT_Result DwfCallbackManager::process_polymarker(WT_Polymarker& pm, WT_File& file)
{
  if (_importer->extent()._collectBounds) 
  {
    _importer->extent().updateBounds(pm, file);
    return WT_Result::Success;
  }
	for (int i = 0; i < pm.count(); ++i)
	{
		OdDbPointPtr pPoint = OdDbPoint::createObject();
		pPoint->setPosition(_importer->extent().transformPoint(pm.points()[i]));
		_importer->blocks().addEntity(pPoint, file);
	}
	return WT_Result::Success;
}

WT_Result DwfCallbackManager::process_polytriangle(WT_Polytriangle& pt, WT_File& file)
{
  if (_importer->extent()._collectBounds) 
  {
    _importer->extent().updateBounds(pt, file);
    return WT_Result::Success;
  }
	// polytiangle is filled - create hatch
	OdDbHatchPtr pHatch = _importer->blocks().addHatch(file);
	OdGePoint2dArray vertexPts;
	OdGeDoubleArray vertexBulges;
	vertexPts.reserve(pt.count());
	// make contour from trianle strip
	// even points - one side
	int i = 0;
	for (; i < pt.count(); i += 2)
	{
		OdGePoint3d p = _importer->extent().transformPoint(pt.points()[i]);
		vertexPts.push_back(OdGePoint2d(p.x,p.y));
	}
	// odd points, in reversed order - other side
	i = pt.count() - 1 - pt.count()%2;
	for (; i > 0; i-= 2)
	{
		OdGePoint3d p = _importer->extent().transformPoint(pt.points()[i]);
		vertexPts.push_back(OdGePoint2d(p.x,p.y));
	}
	pHatch->appendLoop(OdDbHatch::kExternal | OdDbHatch::kPolyline, 
		vertexPts, vertexBulges);
	return WT_Result::Success;	
}

WT_Result DwfCallbackManager::process_contour_set(WT_Contour_Set& cs, WT_File& file)
{
  if (_importer->extent()._collectBounds) 
  {
    _importer->extent().updateBounds(cs, file);
    return WT_Result::Success;
  }
	OdDbHatchPtr pHatch = _importer->blocks().addHatch(file);
	OdGeDoubleArray vertexBulges;
	int n = 0; // pass through index
	for (int i = 0; i < cs.contours(); i++)
	{
		OdGePoint2dArray vertexPts;
		vertexPts.resize(cs.counts()[i]);
		for (int j = 0; j < cs.counts()[i]; j++)
		{
			OdGePoint3d p = _importer->extent().transformPoint(cs.points()[n++]);
			vertexPts[j].set(p.x, p.y);
		}
		pHatch->appendLoop(OdDbHatch::kExternal | OdDbHatch::kPolyline, 
			vertexPts, vertexBulges);
	}
	ODA_ASSERT(n == cs.total_points());
	return WT_Result::Success;
}

WT_Result DwfCallbackManager::process_gouraud_polytriangle(WT_Gouraud_Polytriangle& gpt, WT_File& file)
{
  if (_importer->extent()._collectBounds) 
  {
    _importer->extent().updateBounds(gpt, file);
    return WT_Result::Success;
  }
	ODA_TRACE0("[DWFImport] WT_Gouraud_Polytriangle: not implemented");
	return WT_Result::Success;
}

WT_Result DwfCallbackManager::process_font(WT_Font& font, WT_File& file)
{
	file.rendition().font() = font;
  if (_importer->extent()._collectBounds) return WT_Result::Success;
	_importer->fonts().setFontStyle(file.rendition().font());
	return WT_Result::Success;
}

namespace txt
{
  void setScoring(OdString& string, WT_Font& font, const char* underscore, const char* overscore)
  {
    //???
    // according to whip specs, scoring may be really complex, 
    // but i've never saw one, and specs are not clear enough to write code
    // so we import obvious cases
    // (anyway, Autocad never exports scoring)
    if (font.style().underlined() || font.flags() & 8) 
      string = OdString(underscore) + string;
    if (font.flags() & 0x10) string = OdString(overscore) + string;
  }
  
  void setSpacing(OdString& string, WT_Font& font)
  {
    // DWF spacing is much more flexible - from 1/1024 to 64
    // DWG allows only 0.75 - 4
    double spacing = double(font.spacing()) / 1024;
    if (spacing > 4) spacing = 4;
    if (spacing < 0.75) spacing = 0.75;
    char buf[20];
    sprintf(buf, "\\T%.3g;", spacing);
    string = buf + string;
  }

  void setOblique(OdString& string, WT_Font& font)
  {
    if (font.oblique().oblique() == 0) return;
    char buf[20];
    sprintf(buf, "\\Q%d;", int(360. / 65636 * font.oblique()));
    string = OdString(buf) + string;
  }
  
  void setWidthFactor(OdString& string, WT_Font& font)
  {
    if (font.width_scale().width_scale() == 1024) return;
    char buf[20];
    sprintf(buf, "\\W%.3g;", 1. / 1024 * font.width_scale());
    string = buf + string;
  }

  OdString transformUnicode(const unsigned short* ws)
  {
    if ( ws == 0 || *ws == 0 ) return "";
    int len = 0;
    // calculate string length
    const unsigned short* s = ws;
    for (; *s; s++)
    {
      if (*s > 128) len += 7; // wide symbol '\\U+xxxx'
      else len++;
    }
    OdString res;
    char* buf = res.getBufferSetLength(len+1);
    buf[len] = 0;
    int i = 0;
    // format acad unicode string
    for (s = ws; *s; s++)
    {
      if (*s > 128) 
      {
        sprintf(buf + i, "\\U+%04X", *s);
        i += 7;
      }
      else buf[i++] = char(*s);
    }
    res.releaseBuffer(len+1);
    return res;
  }
}

WT_Result DwfCallbackManager::process_text(WT_Text& text, WT_File& file)
{
  if (_importer->extent()._collectBounds) 
  {
    _importer->extent().updateBounds(text, file);
    return WT_Result::Success;
  }
  if (file.rendition().font().spacing().spacing() != 1024)
  {
    OdDbMTextPtr pText = OdDbMText::createObject();
    _importer->blocks().addEntity(pText, file);
    pText->setAttachment(OdDbMText::kTopLeft);
    double h = _importer->fonts().getTextHeight(file.rendition().font().height().height());
    pText->setTextHeight(h);
    OdGePoint3d p = _importer->extent().transformPoint(text.position());
    p.y += h;
    pText->setLocation(p);
    pText->setRotation(Oda2PI / 65636 * file.rendition().font().rotation().rotation());
    
    OdString string = text.string().is_ascii() ? 
      OdString(text.string().ascii()) : txt::transformUnicode(text.string().unicode());
     
    txt::setScoring(string, file.rendition().font(), "\\L", "\\O");
    txt::setSpacing(string, file.rendition().font());
    pText->setTextStyle(_importer->database()->getTEXTSTYLE());
    txt::setOblique(string, file.rendition().font());
    txt::setWidthFactor(string, file.rendition().font());
    pText->setContents(string);
  }
  else
  {
    OdDbTextPtr pText = OdDbText::createObject();
    _importer->blocks().addEntity(pText, file);
    pText->setPosition(_importer->extent().transformPoint(text.position()));
    pText->setRotation(Oda2PI / 65636 * file.rendition().font().rotation().rotation());
    OdString string = text.string().is_ascii() ? 
      OdString(text.string().ascii()) : txt::transformUnicode(text.string().unicode());
    txt::setScoring(string, file.rendition().font(), "%%U", "%%O");
    pText->setTextString(string);
    pText->setTextStyle(_importer->database()->getTEXTSTYLE());
    pText->setHeight(_importer->fonts().getTextHeight(file.rendition().font().height().height()));
    pText->setOblique(Oda2PI/65636*file.rendition().font().oblique());
    pText->setWidthFactor(1./1024*file.rendition().font().width_scale());
  }
	return WT_Result::Success;
}

WT_Result DwfCallbackManager::process_viewport(WT_Viewport& vp, WT_File& file)
{
  if (_importer->extent()._collectBounds && vp.contour() != 0)
  {
    _importer->extent().updateBounds(const_cast<WT_Contour_Set&>(*vp.contour()), file);
    if (vp.contour() != 0) 
    {
      _importer->extent().setClipRect(const_cast<WT_Contour_Set&>(*vp.contour()).bounds());
    }
    else
    {
      _importer->extent().setClipRect(WT_Logical_Box(0,0,0,0));
    }
    return WT_Result::Success;
  }
	// DWF viewport corresponds to the DWG block with clip object attached
	_importer->blocks().setCurrentBlock(_importer->blocks().currentLayout()->getBlockTableRecordId());
  // making new current block we override grouping
	_importer->blocks().setCurrentGroup(-1);
  // "(Viewport)" opcode => closing viewport
	if (vp.contour() == 0) return WT_Result::Success;

  // else create block
	OdDbBlockTableRecordPtr block = OdDbBlockTableRecord::createObject();
	OdDbBlockTablePtr pTable = _importer->database()->getBlockTableId().safeOpenObject(OdDb::kForWrite);
  block->setName(dwfImp::generateName("Viewport", pTable));
	OdDbObjectId blockId = pTable->add(block);

  // add insert to current layout
	OdDbBlockReferencePtr pBlkRef = OdDbBlockReference::createObject();
  // make insert visible
  WT_Visibility oldVisibility = file.rendition().visibility();
  file.rendition().visibility() = WD_True;
  _importer->blocks().addEntity(pBlkRef, file);
  file.rendition().visibility() = oldVisibility;
	pBlkRef->setBlockTableRecord(blockId);
  pBlkRef->setScaleFactors(OdGeScale3d::kIdentity);
  pBlkRef->setPosition(OdGePoint3d::kOrigin);

  _importer->blocks().setCurrentBlock(block);

  // add clip object
	OdDbSpatialFilterPtr filter = OdDbSpatialFilter::createObject();
	OdGePoint2dArray bounds;
  int count = vp.contour()->counts()[0];
    // in dwg first and last point of the boundary must not coincide
  if (count > 2 && vp.contour()->points()[0] == vp.contour()->points()[count - 1])
    --count;
  bounds.resize(count);
  for (int i = 0; i < count; i++)
  {
    OdGePoint3d p = _importer->extent().transformPoint(vp.contour()->points()[i]);
    bounds[i].set(p.x, p.y); 
  }
  filter->setDefinition(bounds);
	OdDbIndexFilterManager::addFilter(pBlkRef, filter);

  return WT_Result::Success;
}

WT_Result DwfCallbackManager::process_polyline(WT_Polyline& pl, WT_File& file)
{
  if (_importer->extent()._collectBounds) 
  {
    _importer->extent().updateBounds(pl, file);
    return WT_Result::Success;
  }
  _importer->lines().addPolyline(pl, file);
	return WT_Result::Success;
}

WT_Result DwfCallbackManager::process_gouraud_polyline(WT_Gouraud_Polyline& gpl, WT_File& file)
{
  if (_importer->extent()._collectBounds) 
  {
    _importer->extent().updateBounds(gpl, file);
    return WT_Result::Success;
  }
	ODA_TRACE0("[DWFImport] WT_Gouraud_Polyline: not implemented");
	OdDb2dPolylinePtr p2dPl = OdDb2dPolyline::createObject();
	_importer->blocks().addEntity(p2dPl, file);
	for (int i = 0; i < gpl.count(); i++)
	{
		OdDb2dVertexPtr pV = OdDb2dVertex::createObject();
		pV->setPosition(_importer->extent().transformPoint(gpl.points()[i]));
	}
	return WT_Result::Success;
}

WT_Result DwfCallbackManager::process_fill_pattern(WT_Fill_Pattern& fp, WT_File& file)
{
  if (_importer->extent()._collectBounds) return WT_Result::Success;
  // default processing does not do that, why?
  file.rendition().fill_pattern() = fp;
	return WT_Result::Success;
}

WT_Result DwfCallbackManager::process_plot_info(WT_Plot_Info& pi, WT_File&)
{
  if (!_importer->extent()._collectBounds) return WT_Result::Success;
  _importer->extent().setPaperInfo(pi.paper_width(),pi.paper_height(),
   pi.paper_units() == WT_Plot_Info::Inches ? OdDbPlotSettings::kInches : OdDbPlotSettings::kMillimeters);
  return WT_Result::Success;
}

WT_Result DwfCallbackManager::process_color_map( WT_Color_Map& pi, WT_File&)
{
  if ( _importer->extent()._collectBounds ) 
    _importer->blocks().registerColorMap( pi );
  return WT_Result::Success;
}

// single instance of current importer
// should be __declspec(thread) on win32 ??
// 
DwfImporter* DwfCallbackManager::_importer = 0;
