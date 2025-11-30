#ifndef _DwfFontManager_Included_
#define _DwfFontManager_Included_

class DwfImporter;
/////////////////////////////////////////////////////
// DWF fonts & text management
//
class DwfFontManager
{
  DwfImporter* _importer;
  // text style description
  struct TextStyle
  {
    TextStyle(WT_Font& font, OdDbTextStyleTableRecordPtr& textStyle, double ascentRatio = 1.)
      :_wtFont(font), _textStyle(textStyle), _ascentRatio(ascentRatio){}
    WT_Font _wtFont;
    OdDbTextStyleTableRecordPtr _textStyle;
    double _ascentRatio; // tmHeight/tmAscent (for DWF->ACAD text height conversion)
  };
  // mapping: WT_Font -> text style id
  typedef std::vector<TextStyle> FontStyleMap;
  FontStyleMap _fontStyles;
  // current text style (index in the _fontStyles array)
  int _currentTextStyle;
public:
  DwfFontManager(DwfImporter* importer)
    :_importer(importer), _currentTextStyle(-1){}
  void clear();
  // update text style table & set TEXTSTYLE
  void setFontStyle(WT_Font& font);
  // calculate acad font height from current style
  double getTextHeight(int);
  // calculate text height scaling, for the style
  void calculateAscent(TextStyle& pStyle);
};

#endif
