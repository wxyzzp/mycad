#include "StdAfx.h"
#include "DwfImportImpl.h"
#include "DwfEventReactor.h"
#include "EmbeddedImageDef.h"
#include "DwfCallbacks.h"
#include "DbRasterImage.h"
#include "DbPolyline.h"

OdDwfImport::ImportResult DwfImporter::loadStream(const char* path)
{
  WT_File wtFile;
  wtFile.set_filename(path);
  wtFile.set_file_mode(WT_File::File_Read);
  wtFile.open();
  wtFile.rendition().font() = WT_Font();
  
#define CALLBACK_MACRO(class_name, class_lower) \
		wtFile.set_##class_lower##_action(DwfCallbackManager::process_##class_lower);
  MY_CALLBACK_LIST
#undef CALLBACK_MACRO
    
    // Do the actual reading.
    while (wtFile.process_next_object() == WT_Result::Success);
    wtFile.close();
    return OdDwfImport::success;
}

OdDbObjectId DwfBlockManager::addViewport()
{
	OdDbViewportPtr	vp = OdDbViewport::createObject();
	OdDbLayoutPtr layout = _currentBlock->getLayoutId().safeOpenObject();
	OdDbBlockTableRecordPtr lBlock = layout->getBlockTableRecordId().safeOpenObject(OdDb::kForWrite);
	lBlock->appendOdDbEntity(vp);
	return vp->objectId();
}

// Transform logical point to DWG point
//
OdGePoint3d DwfExtentManager::transformPoint(const WT_Logical_Point& wtPoint)
{
  return OdGePoint3d( 
    (wtPoint.m_x + _scaleInfo._xOffset) * _scaleInfo._scale,
    (wtPoint.m_y + _scaleInfo._yOffset) * _scaleInfo._scale, 0);
}

// convert DWF logical size to DWG size
//
double DwfExtentManager::transformSize(double size)
{
	return _scaleInfo._scale * size;
}

OdDbHatchPtr DwfBlockManager::addHatch(WT_File& file)
{
	OdDbHatchPtr pHatch = OdDbHatch::createObject();
	addEntity(pHatch, file);
	pHatch->setAssociative(false);
	pHatch->setHatchStyle(OdDbHatch::kNormal);
	switch (file.rendition().fill_pattern().pattern_id())
	{
	case WT_Fill_Pattern::Solid:
		pHatch->setPattern(OdDbHatch::kPreDefined, "SOLID");
		return pHatch;
	case WT_Fill_Pattern::Checkerboard:
		pHatch->setPattern(OdDbHatch::kPreDefined, "SQUARE");
		return pHatch;
	case WT_Fill_Pattern::Crosshatch:
		pHatch->setPattern(OdDbHatch::kPreDefined, "NET");
		break;
	case WT_Fill_Pattern::Diamonds:
		pHatch->setPattern(OdDbHatch::kPreDefined, "ANSI37");
		return pHatch;
	case WT_Fill_Pattern::Horizontal_Bars:
		pHatch->setPattern(OdDbHatch::kPreDefined, "LINE");
		break;
	case WT_Fill_Pattern::Slant_Left:
		pHatch->setPattern(OdDbHatch::kPreDefined, "ANSI31");
		pHatch->setPatternAngle(OdaPI2);
		break;
	case WT_Fill_Pattern::Slant_Right:
		pHatch->setPattern(OdDbHatch::kPreDefined, "ANSI31");
		break;
	case WT_Fill_Pattern::Square_Dots:
		pHatch->setPattern(OdDbHatch::kPreDefined, "DOTS");
		return pHatch;
	case WT_Fill_Pattern::Vertical_Bars:
		pHatch->setPattern(OdDbHatch::kPreDefined, "LINE");
		pHatch->setPatternAngle(OdaPI2);
		break;
	case WT_Fill_Pattern::User_Defined:
		ODA_TRACE0("[DWFImport] User Defined pattern encountered");
		pHatch->setPattern(OdDbHatch::kPreDefined, "SOLID");
		return pHatch;
	default:
		pHatch->setPattern(OdDbHatch::kPreDefined, "SOLID");
		break;
	}
  double scale = file.rendition().fill_pattern().pattern_scale();
  if (scale >= 0) 
  {
	  pHatch->setPatternScale(1);
  }
	return pHatch;
}

namespace txt
{
  OdString transformUnicode(const unsigned short* ws);
}

void DwfBlockManager::addEntity(OdDbEntity* ent, WT_File& file)
{
	_currentBlock->appendOdDbEntity(ent);
  // add to group
	if (!_group.isNull()) _group->append(ent->objectId());
  // urls
	if (!file.rendition().url().url().is_empty())
	{
    OdDbEntityHyperlinkPEPtr hpe = ent;
    OdDbHyperlinkCollectionPtr urls = hpe->getHyperlinkCollection(ent);
    for (WT_URL_Item* url = (WT_URL_Item*)file.rendition().url().url().get_head(); url; url = (WT_URL_Item*)url->next())
    {
      if ( url->friendly_name().is_ascii() )
        urls->addTail(url->address().ascii(), url->friendly_name().ascii());
      else 
        urls->addTail(url->address().ascii(), txt::transformUnicode(url->friendly_name().unicode()));
    }
    hpe->setHyperlinkCollection(ent, urls);
	}
	// color
  OdCmColor color;
  if ( _preserveColorIndices && file.rendition().color().index() != WD_NO_COLOR_INDEX )
  {
    color.setColorIndex( file.rendition().color().index() );
  }
  else
  {
    WT_RGBA32 rgb = file.rendition().color().rgba();
    color.setRGB( rgb.m_rgb.r, rgb.m_rgb.g, rgb.m_rgb.b );
  }
  ent->setColor(color);
  // visibility, etc
  ent->setVisibility(file.rendition().visibility().visible() ? OdDb::kVisible : OdDb::kInvisible);
	ent->setLinetype(_importer->lines().getCurrentLineStyle(file));
	ent->setLineWeight(_importer->lines().getCurrentLineWeight(file));
  ent->setLayer(_layerMap[file.rendition().layer().layer_num()]);
}

namespace image
{
  void setFileExt(EmbeddedImageDefPtr pImageDef, int format)
  {
    static const char* imageExts[] = 
    {
      ".img",
        ".mapped",
        ".bit",
        ".g3x",
        ".256",
        ".256",
        ".rgb",
        ".rgba",
        ".jpg",
        ".tif",
        "",
        "",
        ".png",
        ".tif"
    };
		ODA_ASSERT( format > 0 && format < sizeof(imageExts) / sizeof(imageExts[0]));
    pImageDef->setExtension(imageExts[format]);
  }

  EmbeddedImageDefPtr createImageDef(OdDbDatabase* db, OdDbObjectId& imageDefId, const unsigned char* data, int dataSize,int cols, int rows)
  {
		// Add an image entity to the drawing.
		OdDbObjectId imageDictId = OdDbRasterImageDef::createImageDictionary(db);
		
		// Open the image dictionary.
		OdDbDictionaryPtr pImageDict = imageDictId.safeOpenObject(OdDb::kForWrite);
		
		// Add imagedef object, which is similar to a block definition in that
		// it defines the associated raster file and attributes for this image.
    EmbeddedImageDefPtr pImageDef = EmbeddedImageDef::createObject();
    // copy image data to stream & store it in imagedef (to save later)
    OdStreamBufPtr buf = OdMemoryStream::createNew();
    buf->putBytes(data, dataSize);
    buf->rewind();
    pImageDef->setData(buf);

		// Add imagedef to image dictionary.
		imageDefId = pImageDict->setAt(dwfImp::generateName("Image", pImageDict), pImageDef);
    // add image to reactor, to be notified of db saving
    DwfEventReactor::getReactor(db)->addImage(pImageDef);

    // try loading image, else set placeholder
    // OdGiRasterImagePtr img = odSystemServices()->loadRasterImage(buf);
    OdRxDynamicLinkerPtr pDynLnk = odrxDynamicLinker();
    OdUInt32 rasterId = pDynLnk->loadApp(RX_RASTER_SERVICES_APPNAME);
    OdRxRasterServicesPtr pRasSvcs;
    OdGiRasterImagePtr img;
    if(rasterId)
    {
      pRasSvcs = pDynLnk->module(rasterId);
      img = pRasSvcs->loadRasterImage(buf);
    }
    
    if (!img.isNull()) pImageDef->setGiImage(img);
    else pImageDef->setImage(OdGiRasterImageDesc::createObject(
      cols, rows, OdGiRasterImage::kNone));
    
    return pImageDef;
  }
}

void DwfImageManager::addImage(const unsigned char* data, int dataSize, int cols, int rows, 
	int format, const WT_Logical_Point& ll, const WT_Logical_Point& ur, int id, WT_File& file)
{
	OdDbObjectId imageDefId = 0;
	
	ImageMap::const_iterator it = _imageMap.find(id);
	if (it == _imageMap.end())
	{
    EmbeddedImageDefPtr pImageDef = image::createImageDef(
      _importer->database(), imageDefId, data, dataSize, cols, rows);
    image::setFileExt(pImageDef, format);
		_imageMap[id] = imageDefId;
	}
	else imageDefId = it->second;
	// Add image entity, which references the above imagedef.  
	OdDbRasterImagePtr pImage = OdDbRasterImage::createObject();
	// Add image entity to block.
	_importer->blocks().addEntity(pImage, file);
	
	// Set remaining image attributes.
	pImage->setImageDefId(imageDefId);
	OdGePoint3d minCorner = _importer->extent().transformPoint(ll);
	OdGePoint3d maxCorner = _importer->extent().transformPoint(ur);
	pImage->setOrientation(minCorner, 
    OdGeVector3d(maxCorner.x - minCorner.x, 0, 0), 
    OdGeVector3d(0, maxCorner.y - minCorner.y, 0));
	pImage->setDisplayOpt(OdDbRasterImage::kShow, true);
	pImage->setDisplayOpt(OdDbRasterImage::kShowUnAligned, true);
	pImage->setBrightness(OdUInt8(50));
	pImage->setContrast(OdUInt8(50));
  // boundary clip
  OdGePoint2dArray pts;
  pts.push_back(OdGePoint2d::kOrigin);
  pts.push_back(OdGePoint2d(cols, rows));
  
  pImage->setClipBoundary(pts); // 2 points mean OdDbRasterImage::kRect boundary type
}

OdString DwfBlockManager::normalizeObjectName( const OdString& old )
{
  OdString name( old );
  name.replace('|','_');
  name.replace('<','_');
  name.replace('>','_');
  name.replace('\\','_');
  name.replace('/','_');
  name.replace('*','_');
  name.replace('?','_');
  name.replace('=','_');
  name.replace('\'','_');
  name.replace('\"','_');
  name.replace(':','_');
  name.replace(';','_');
  name.replace(',','_');
  return name;
}

void DwfBlockManager::setCurrentLayer(WT_Layer& layer)
{
	LayerMap::const_iterator ci = _layerMap.find(layer.layer_num());
	if (ci == _layerMap.end())
	{
    ODA_ASSERT(layer.layer_name().is_ascii());
    OdString name = normalizeObjectName( layer.layer_name().ascii() );
    if ( name == "0" )
    {
      _layerMap[layer.layer_num()] = _importer->database()->getLayerZeroId();
    }
    else
    {
      OdDbLayerTablePtr pLayers = _importer->database()->getLayerTableId().safeOpenObject(OdDb::kForWrite);
      OdDbLayerTableRecordPtr pLayer = OdDbLayerTableRecord::createObject();
      pLayer->setName( name );
      OdDbObjectId id = pLayers->has( name ) ? pLayers->getAt( name ) : pLayers->add( pLayer );
      _importer->database()->setCLAYER( id );
      _layerMap[layer.layer_num()] = id;
    }
	}
	else _importer->database()->setCLAYER( ci->second );
}

void DwfBlockManager::setCurrentGroup(int id, const char* name)
{
  if (id == -1)
  {
    _group = 0;
    return;
  }
	
  GroupMap::const_iterator i = _groups.find(id);
	if (i == _groups.end())
	{
		OdDbDictionaryPtr pGroupDic = _importer->database()->getGroupDictionaryId().safeOpenObject(OdDb::kForWrite);
		_group = OdDbGroup::createObject();
		OdString odName = name;
		// no name or such an object already exist
		if (odName.isEmpty() || pGroupDic->has(odName))
      odName = dwfImp::generateName("Node", pGroupDic, id);
		pGroupDic->setAt(odName, _group);
		_group->setName(odName);
		_group->setSelectable(true);
		_groups[id] = _group;
	}
	else _group = i->second;
}

void DwfFontManager::calculateAscent(TextStyle& pStyle)
{
  double currentTextHeight = _importer->extent().transformSize(pStyle._wtFont.height().height());
  // recalculating text height, because in ACAD text height == 'A' height
  pStyle._textStyle->setTextSize(100);
  OdGiTextStyle giStyle;
  giFromDbTextStyle(pStyle._textStyle, giStyle);
  pStyle._ascentRatio = ( giStyle.getFont()->getHeight() - giStyle.getFont()->getInternalLeading() ) / giStyle.getFont()->getAbove();
  currentTextHeight /= pStyle._ascentRatio;
  pStyle._textStyle->setTextSize(currentTextHeight);
}

double DwfFontManager::getTextHeight(int h)
{
  double currentTextHeight = _importer->extent().transformSize(h);
  if (_currentTextStyle != -1)
    currentTextHeight /= _fontStyles[_currentTextStyle]._ascentRatio;
  return currentTextHeight;
}

void DwfFontManager::setFontStyle(WT_Font& font)
{
	// search for existing text style
	FontStyleMap::const_iterator ci = _fontStyles.begin();
	for (; ci != _fontStyles.end(); ++ci)	
  {
    if (ci->_wtFont.font_name() == font.font_name()
      && ci->_wtFont.style().bold() == font.style().bold()
      && ci->_wtFont.style().italic() == font.style().italic()
      && ci->_wtFont.flags() == font.flags()
      ) break;
  }
	if (ci != _fontStyles.end()) // if found - set & exit
	{
		_importer->database()->setTEXTSTYLE(ci->_textStyle->objectId());
    _currentTextStyle = ci - _fontStyles.begin();
		return;
	}

	OdDbTextStyleTableRecordPtr pStyle = OdDbTextStyleTableRecord::createObject();
	// Name must be set before a table object is added to a table.  The
	// isShapeFile flag must also be set (if true) before adding the object
	// to the database.
	char styleName[13];
	sprintf(styleName, "style%d", _fontStyles.size());
	pStyle->setName(styleName);
	// shape fonts are exported as polylines (dwf doesn't support shape fonts)
	pStyle->setIsShapeFile(false);
	
	OdDbTextStyleTablePtr pStyles = _importer->database()->getTextStyleTableId().safeOpenObject(OdDb::kForWrite);
	pStyles->add(pStyle);
	_fontStyles.push_back(TextStyle(font, pStyle));
	_currentTextStyle = _fontStyles.size() - 1;

	// Set the remaining properties.
	pStyle->setXScale(1./1024*font.width_scale());
	pStyle->setObliquingAngle(Oda2PI/65636*font.oblique());
	pStyle->setFont(font.font_name().name().ascii(), 
		font.style().bold() != 0, font.style().italic() != 0,
		font.charset().charset(), 
		int(font.pitch().pitch()) | int(font.family().family()));
  calculateAscent(_fontStyles.back());

  // these flags are not part of specification, they are just "used by autocad" (and our exporter)
	pStyle->setIsVertical(font.flags() & 0x0001);
  pStyle->setIsBackwards((font.flags() & 0x0002) != 0);
  pStyle->setIsUpsideDown((font.flags() & 0x0004) != 0);

  _importer->database()->setTEXTSTYLE(pStyle->objectId());
}

// set Linetype dashes
//
void DwfLineManager::setLinePattern(OdDbLinetypeTableRecord* pLt, const short* pattern, int length, double scale)
{
  if (scale <= 0) return; // no pattern length => solid line
  ODA_ASSERT(length != 1);
  pLt->setNumDashes(length);
  int total_length = 0;
  int i = 0;
  for (; i < length; i++)
    total_length += pattern[i];
  pLt->setPatternLength(scale);
  for (i = 0; i < length; i++)
  {
    double dash = pattern[i] * scale / total_length;
    // gaps are negative
    if (i % 2) dash = -dash;
    pLt->setDashLengthAt(i, dash);
  }
}

// set linetype to one of the standard patterns described in WT_Line_Pattern
// WARNING: all non ISO styles are given approximately
//
void DwfLineManager::setPredefinedPattern(OdDbLinetypeTableRecord* pLt, int patternId, double scale)
{
  switch (patternId)
  {
  case WT_Line_Pattern::Solid:
    break;
  case WT_Line_Pattern::Dashed: 
    {
      static short pat[] = {47, 12};
      setLinePattern(pLt, pat, sizeof(pat)/sizeof(pat[0]), scale);
    }
    break;
  case WT_Line_Pattern::Dotted: 
    {
      static short pat[] = {0, 24};
      setLinePattern(pLt, pat, sizeof(pat)/sizeof(pat[0]), scale);
    }
    break;
  case WT_Line_Pattern::Dash_Dot: 
    {
      static short pat[] = {36, 12, 0, 12};
      setLinePattern(pLt, pat, sizeof(pat)/sizeof(pat[0]), scale);
    }
    break;
  case WT_Line_Pattern::Short_Dash: 
    {
      static short pat[] = {36, 72};
      setLinePattern(pLt, pat, sizeof(pat)/sizeof(pat[0]), scale);
    }
    break;
  case WT_Line_Pattern::Medium_Dash: 
    {
      static short pat[] = {48, 48};
      setLinePattern(pLt, pat, sizeof(pat)/sizeof(pat[0]), scale);
    }
    break;
  case WT_Line_Pattern::Long_Dash: 
    {
      static short pat[] = {72, 12};
      setLinePattern(pLt, pat, sizeof(pat)/sizeof(pat[0]), scale);
    }
    break;
  case WT_Line_Pattern::Short_Dash_X2: 
    {
      static short pat[] = {36, 144};
      setLinePattern(pLt, pat, sizeof(pat)/sizeof(pat[0]), scale);
    }
    break;
  case WT_Line_Pattern::Medium_Dash_X2: 
    {
      static short pat[] = {48, 72};
      setLinePattern(pLt, pat, sizeof(pat)/sizeof(pat[0]), scale);
    }
    break;
  case WT_Line_Pattern::Long_Dash_X2: 
    {
      static short pat[] = {72, 24};
      setLinePattern(pLt, pat, sizeof(pat)/sizeof(pat[0]), scale);
    }
    break;
  case WT_Line_Pattern::Medium_Long_Dash: 
    {
      static short pat[] = {64, 24};
      setLinePattern(pLt, pat, sizeof(pat)/sizeof(pat[0]), scale);
    }
    break;
  case WT_Line_Pattern::Medium_Dash_Short_Dash_Short_Dash: 
    {
      static short pat[] = {36, 12, 24, 12, 24, 12};
      setLinePattern(pLt, pat, sizeof(pat)/sizeof(pat[0]), scale);
    }
    break;
  case WT_Line_Pattern::Long_Dash_Short_Dash: 
    {
      static short pat[] = {72, 12, 24, 12};
      setLinePattern(pLt, pat, sizeof(pat)/sizeof(pat[0]), scale);
    }
    break;
  case WT_Line_Pattern::Long_Dash_Dot_Dot: 
    {
      static short pat[] = {72, 12, 0, 12, 0, 12};
      setLinePattern(pLt, pat, sizeof(pat)/sizeof(pat[0]), scale);
    }
    break;
  case WT_Line_Pattern::Long_Dash_Dot: 
    {
      static short pat[] = {72, 12, 0, 12};
      setLinePattern(pLt, pat, sizeof(pat)/sizeof(pat[0]), scale);
    }
    break;
  case WT_Line_Pattern::Medium_Dash_Dot_Short_Dash_Dot: 
    {
      static short pat[] = {48, 12, 0, 12, 24, 12, 0, 12};
      setLinePattern(pLt, pat, sizeof(pat)/sizeof(pat[0]), scale);
    }
    break;
  case WT_Line_Pattern::Sparse_Dot: 
    {
      static short pat[] = {0, 72};
      setLinePattern(pLt, pat, sizeof(pat)/sizeof(pat[0]), scale);
    }
    break;
  case WT_Line_Pattern::ISO_Dash: 
    {
      static short pat[] = {47, 12};
      setLinePattern(pLt, pat, sizeof(pat)/sizeof(pat[0]), scale);
    }
    break;
  case WT_Line_Pattern::ISO_Dash_Space: 
    {
      static short pat[] = {47, 71};
      setLinePattern(pLt, pat, sizeof(pat)/sizeof(pat[0]), scale);
    }
    break;
  case WT_Line_Pattern::ISO_Long_Dash_Dot: 
    {
      static short pat[] = {94, 12, 0, 12};
      setLinePattern(pLt, pat, sizeof(pat)/sizeof(pat[0]), scale);
    }
    break;
  case WT_Line_Pattern::ISO_Long_Dash_Double_Dot: 
    {
      static short pat[] = {94, 12, 0, 12, 0, 12};
      setLinePattern(pLt, pat, sizeof(pat)/sizeof(pat[0]), scale);
    }
    break;
  case WT_Line_Pattern::ISO_Long_Dash_Triple_Dot: 
    {
      static short pat[] = {94, 12, 0, 12, 0, 12, 0, 12};
      setLinePattern(pLt, pat, sizeof(pat)/sizeof(pat[0]), scale);
    }
    break;
  case WT_Line_Pattern::ISO_Dot: 
    {
      static short pat[] = {0, 79};
      setLinePattern(pLt, pat, sizeof(pat)/sizeof(pat[0]), scale);
    }
    break;
  case WT_Line_Pattern::ISO_Long_Dash_Short_Dash: 
    {
      static short pat[] = {94, 12, 24, 12};
      setLinePattern(pLt, pat, sizeof(pat)/sizeof(pat[0]), scale);
    }
    break;
  case WT_Line_Pattern::ISO_Long_Dash_Double_Short_Dash: 
    {
      static short pat[] = {94, 12, 24, 12, 24, 12};
      setLinePattern(pLt, pat, sizeof(pat)/sizeof(pat[0]), scale);
    }
    break;
  case WT_Line_Pattern::ISO_Dash_Dot: 
    {
      static short pat[] = {47, 12, 0, 12};
      setLinePattern(pLt, pat, sizeof(pat)/sizeof(pat[0]), scale);
    }
    break;
  case WT_Line_Pattern::ISO_Double_Dash_Dot: 
    {
      static short pat[] = {47, 12, 47, 12, 0, 12};
      setLinePattern(pLt, pat, sizeof(pat)/sizeof(pat[0]), scale);
    }
    break;
  case WT_Line_Pattern::ISO_Dash_Double_Dot: 
    {
      static short pat[] = {47, 12, 0, 12, 0, 12};
      setLinePattern(pLt, pat, sizeof(pat)/sizeof(pat[0]), scale);
    }
    break;
  case WT_Line_Pattern::ISO_Double_Dash_Double_Dot: 
    {
      static short pat[] = {47, 12, 47, 12, 0, 12, 0, 12};
      setLinePattern(pLt, pat, sizeof(pat)/sizeof(pat[0]), scale);
    }
    break;
  case WT_Line_Pattern::ISO_Dash_Triple_Dot: 
    {
      static short pat[] = {47, 12, 0, 12, 0, 12, 0, 12};
      setLinePattern(pLt, pat, sizeof(pat)/sizeof(pat[0]), scale);
    }
    break;
  case WT_Line_Pattern::ISO_Double_Dash_Triple_Dot: 
    {
      static short pat[] = {47, 12, 47, 12, 0, 12, 0, 12, 0, 12};
      setLinePattern(pLt, pat, sizeof(pat)/sizeof(pat[0]), scale);
    }
    break;
  default:
    ODA_TRACE1("[DWFImport] Undefined linestyle #%d", patternId);
  }
}

OdDbObjectId DwfLineManager::getCurrentLineStyle(WT_File& file)
{
	int patternId = file.rendition().line_pattern().pattern_id();
	// dash pattern overrides line pattern
	if (file.rendition().dash_pattern() != WT_Dash_Pattern::kNull)
		patternId = file.rendition().dash_pattern().number();

  double scale = _importer->extent().transformSize(file.rendition().line_style().pattern_scale());
	// if pattern scale is not set - ignore pattern (acad patterns must scale)
	if (scale <= 0)	patternId = 1;

	LineStyleMap::const_iterator ci = _lineStyles.find(patternId);
	if (ci != _lineStyles.end()) return ci->second;
		
	OdDbLinetypeTablePtr pTable = _importer->database()->getLinetypeTableId().safeOpenObject(OdDb::kForWrite);
	OdDbLinetypeTableRecordPtr pLt = OdDbLinetypeTableRecord::createObject();

  
  if (patternId < WT_Line_Pattern::Count) // predefined pattern
  {
    ODA_ASSERT(patternId != 0);
    // names of standard DWF line patterns
    const char* wtLinePatternNames[] =
    {
      "",            
        "CONTINUOUS",
        "Dashed",
        "Dotted",
        "Dash_Dot",
        "Short_Dash",
        "Medium_Dash",
        "Long_Dash",
        "Short_Dash_X2",
        "Medium_Dash_X2",
        "Long_Dash_X2",
        "Medium_Long_Dash",
        "Medium_Dash_Short_Dash_Short_Dash",
        "Long_Dash_Short_Dash",
        "Long_Dash_Dot_Dot",
        "Long_Dash_Dot",
        "Medium_Dash_Dot_Short_Dash_Dot",
        "Sparse_Dot",
        "ISO_Dash",
        "ISO_Dash_Space",
        "ISO_Long_Dash_Dot",
        "ISO_Long_Dash_Double_Dot",
        "ISO_Long_Dash_Triple_Dot",
        "ISO_Dot",
        "ISO_Long_Dash_Short_Dash",
        "ISO_Long_Dash_Double_Short_Dash",
        "ISO_Dash_Dot",
        "ISO_Double_Dash_Dot",
        "ISO_Dash_Double_Dot",
        "ISO_Double_Dash_Double_Dot",
        "ISO_Dash_Triple_Dot",
        "ISO_Double_Dash_Triple_Dot"
    };
    // check whether there is such a pattern in dwg
    OdDbObjectId oldId = pTable->getAt(wtLinePatternNames[patternId]);
    if (!oldId.isNull())
    {
      _lineStyles[patternId] = oldId;
      return oldId;
    }
    // if not - create it
    setPredefinedPattern(pLt, patternId, scale);
    pLt->setName(wtLinePatternNames[patternId]);
  }
  else // custom pattern
  {
    setLinePattern(pLt, file.rendition().dash_pattern().pattern(), 
			file.rendition().dash_pattern().length(), scale);
		pLt->setName(dwfImp::generateName("LineType", pTable));
	}

	OdDbObjectId id = pTable->add(pLt);
	_lineStyles[patternId] = id;
	return id;
}

// convert DWF lineweight to DWG lineweight
// DWF lineweight is given in logical units
// DWG - in hudredths of mm's
//
OdDb::LineWeight DwfLineManager::getCurrentLineWeight(WT_File& file)
{
	double w = _importer->extent().transformSize(file.rendition().line_weight().weight_value());
	
	if (_importer->database()->getMEASUREMENT() == OdDb::kEnglish)
		w *= 2540;
	else
		w *= 100;

	// DWG lineweight may only be one of the following
	static int CeLWeightTbl[] = {  0,   5,   9,  13,  15,  18,  20,  25, 30,  35,
		40,  50,  53,  60,  70,  80,  90, 100, 106, 120, 140, 158, 200, 211};

	if ( w <= 0 ) return (OdDb::LineWeight) 0;
	if ( w >= 211 ) return (OdDb::LineWeight) 211;

	for (int i = 0; i < sizeof(CeLWeightTbl) / sizeof(CeLWeightTbl[0]) - 1; i++)
	{
		if (CeLWeightTbl[i] <= w && CeLWeightTbl[i + 1] >= w)
		{
			if (w - CeLWeightTbl[i] > CeLWeightTbl[i + 1] - w)
				return (OdDb::LineWeight) CeLWeightTbl[i + 1];
			else
				return (OdDb::LineWeight) CeLWeightTbl[i];
		}
	}
  return (OdDb::LineWeight) 0;
}

void DwfExtentManager::updateBounds(WT_Drawable& d, WT_File& file)
{
  if (file.rendition().visibility().visible() != WD_True) return;
  
  WT_Logical_Point maxpt = d.bounds(&file).maxpt();
  WT_Logical_Point minpt = d.bounds(&file).minpt();
  
  if (!(_clipRect == WT_Logical_Box(0,0,0,0))
      && (minpt.m_x < _clipRect.m_min.m_x 
      || maxpt.m_x > _clipRect.m_max.m_x 
      || minpt.m_y < _clipRect.m_min.m_y
      || maxpt.m_y > _clipRect.m_max.m_y)) return;

  _extent.m_min.m_x = min(minpt.m_x, _extent.m_min.m_x);
  _extent.m_min.m_y = min(minpt.m_y, _extent.m_min.m_y);
  _extent.m_max.m_x = max(maxpt.m_x, _extent.m_max.m_x);
  _extent.m_max.m_y = max(maxpt.m_y, _extent.m_max.m_y);
}

void DwfExtentManager::setPlotSettings(double wPaper, double hPaper, OdDbPlotSettings::PlotPaperUnits units)
{
  // if have not read paper from dwf - set defaults passed from outside
  if (_paperInfo._units == OdDbPlotSettings::kPixels)
  {
    _paperInfo._units = units;
    _paperInfo._width = wPaper;
    _paperInfo._height = hPaper;
  }
  else 
  {
    units = _paperInfo._units;
    wPaper = _paperInfo._width;
    hPaper = _paperInfo._height;
  }

  OdDbLayoutPtr layout = _importer->blocks().currentBlock()->getLayoutId().safeOpenObject(OdDb::kForWrite);
  OdDbPlotSettingsValidatorPtr pPSV = _importer->database()->appServices()->plotSettingsValidator();

  pPSV->setPlotCfgName(layout, "DWF6 ePlot.pc3", layout->getLayoutName());
  pPSV->setClosestMediaName(layout, wPaper, hPaper, units, true);


  pPSV->setPlotPaperUnits(layout, _importer->database()->getMEASUREMENT() == OdDb::kEnglish ?
    OdDbPlotSettings::kInches : OdDbPlotSettings::kMillimeters);
  pPSV->setPlotRotation(layout, OdDbPlotSettings::k0degrees);

  // MKU 06.07.2004   the thing is OdDbLayoutImpl::activated() activated drawn paper size (limmin/limmax) 
  //  by default values. To change it using actual values
  bool bMm = (_paperInfo._units == OdDbPlotSettings::kMillimeters) ? true : false;
  bool bMetric = (_importer->database()->getMEASUREMENT() == OdDb::kEnglish) ? false : true;
  double dWidth = wPaper;
  double dHeight = hPaper;
  if (bMm && !bMetric)
  {
    dWidth /= kMmPerInch;
    dHeight /= kMmPerInch;
  }
  else if (!bMm && bMetric)
  {
    dWidth *= kMmPerInch;
    dHeight *= kMmPerInch;
  }
  layout->setLIMMIN(OdGePoint2d::kOrigin);
  layout->setLIMMAX(OdGePoint2d(dWidth, dHeight));
}

void DwfExtentManager::calculateScale()
{
  OdDbLayoutPtr layout = _importer->blocks().currentBlock()->getLayoutId().safeOpenObject();
  double printableXmin, printableYmin, printableXmax, printableYmax;
  layout->getPlotPaperMargins(printableXmin, printableYmin, printableXmax, printableYmax);
  double paperWidth = printableXmax - printableXmin;
  double paperHeight = printableYmax - printableYmin;
  
  ODA_ASSERT(paperWidth > 0 && paperHeight > 0);
  
  // paper size is always calculated in mm's
  if (_importer->database()->getMEASUREMENT() == OdDb::kEnglish)
  {
    paperWidth /= 25.4;
    paperHeight /= 25.4;
    printableXmin /= 25.4;
    printableYmin /= 25.4;
  }
  
  int extHeight = _extent.maxpt().m_y - _extent.minpt().m_y;
  int extWidth = _extent.maxpt().m_x - _extent.minpt().m_x;

  if (extWidth == 0)
    _scaleInfo._scale = paperHeight / extHeight;
  else if (extHeight == 0)
    _scaleInfo._scale = paperWidth / extWidth;
  else
    _scaleInfo._scale = min(paperWidth / extWidth, paperHeight / extHeight);

  // HACK: make margins
  _scaleInfo._scale *= 0.95;
  
  centerPlot(paperWidth, paperHeight, extHeight, extWidth);
}

void DwfExtentManager::centerPlot(double paperWidth, double paperHeight, int extHeight, int extWidth)
{
  if (_extent.minpt().m_x < 0 || _extent.maxpt().m_x > paperWidth)
    _scaleInfo._xOffset = -_extent.minpt().m_x 
      + (paperWidth / _scaleInfo._scale - extWidth) / 2;
  if (_extent.minpt().m_y < 0 || _extent.maxpt().m_y > paperHeight)
    _scaleInfo._yOffset = -_extent.minpt().m_y
      + (paperHeight / _scaleInfo._scale - extHeight) / 2;
}

void DwfLineManager::addPolyline(WT_Point_Set& pl, WT_File& file)
{
	OdDbPolylinePtr p2dPl = OdDbPolyline::createObject();
	_importer->blocks().addEntity(p2dPl, file);
	for (int i = 0; i < pl.count(); i++)
	{
    OdGePoint3d p3 = _importer->extent().transformPoint(pl.points()[i]);
    OdGePoint2d p(p3.x, p3.y);
    p2dPl->addVertexAt(i, p);
	}
}

void DwfImporter::cleanupW2D()
{
  _ignoreMetadata = true;
  _extent.clear();
  _fonts.clear();
  _blocks.clear();
  _images.clear();
}

void DwfExtentManager::clear()
{
  _extent.m_min.m_x = 0x7fffffff;
  _extent.m_min.m_y = 0x7fffffff;
  _extent.m_max.m_x = 0x80000000;
  _extent.m_max.m_y = 0x80000000;
  _scaleInfo._scale = 1;
  _scaleInfo._xOffset = 0;
  _scaleInfo._yOffset = 0;
  _collectBounds = true;
  // invalid case - to test uninitialized (in dwf, paper may be in in's or mm's only)
  _paperInfo._units = OdDbPlotSettings::kPixels;
  _clipRect = WT_Logical_Box(0,0,0,0);
}

void DwfFontManager::clear()
{
  _currentTextStyle = -1;
}

void DwfExtentManager::setPaperInfo(double wPaper, double hPaper, OdDbPlotSettings::PlotPaperUnits units)
{
  _paperInfo._width = wPaper;
  _paperInfo._height = hPaper;
  _paperInfo._units = units;
}

void DwfBlockManager::clear()
{
  _currentBlock = 0;
  _group = 0;
  _preserveColorIndices = false;
  _groups.clear();
}

void DwfImageManager::clear()
{
  _imageMap.clear();
}

void DwfBlockManager::setCurrentLayout(OdDbObjectId layoutId)
{
  ODA_ASSERT(!layoutId.isNull());
  _importer->database()->setCurrentLayout(layoutId);
  _layout = layoutId.safeOpenObject(OdDb::kForWrite);
}
// in dwf color is stored as BGRA
static bool colorEqual( ODCOLORREF dwgColor, WT_RGBA32 dwfColor )
{
  return ODGETBLUE( dwgColor ) == dwfColor.m_rgb.b
    && ODGETGREEN( dwgColor ) == dwfColor.m_rgb.g
    && ODGETRED( dwgColor ) == dwfColor.m_rgb.r;
}

void DwfBlockManager::registerColorMap( WT_Color_Map& cm )
{
  if ( cm.size() > 256 ) return;

  _preserveColorIndices = std::equal( odcmAcadLightPalette(), odcmAcadLightPalette() + 256, cm.map(), colorEqual )
    || std::equal( odcmAcadDarkPalette(), odcmAcadDarkPalette() + 256, cm.map(), colorEqual );
}
