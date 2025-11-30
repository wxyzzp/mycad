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
// programs incorporating this software must include the following statement 
// with their copyright notices:
//
//      DWGdirect © 2002 by Open Design Alliance Inc. All rights reserved. 
//
// By use of this software, you acknowledge and accept the terms of this 
// agreement.
///////////////////////////////////////////////////////////////////////////////

#ifndef ODA_CM_COLOR
#define ODA_CM_COLOR

#include "OdString.h"
#include "Gi/Gi.h"

class OdCmEntityColor;
class OdDbDwgFiler;
class OdDbDxfFiler;
class OdDbAuditInfo;

#include "DD_PackPush.h"

/** Description:
    Represents the color of a database entity (OdDbEntity).

    Remarks:
    A database entity can have the following color types:

    o kByLayer - The entity uses the color of the layer that it is on.
    o kByBlock - The entity uses the color of the block that it is contained in.
    o kByColor - The entity has a color specified directly as an RGB value.
    o kByACI - The entity has a color specified by an index into a color table.
    o kForeground - Not used.
    o kNone - No color.

    {group:OdCm_Classes}
*/
class FIRSTDLL_EXPORT OdCmEntityColor
{
public:
  ODRX_HEAP_OPERATORS();
  enum Color
  {
    kRed,
    kGreen,
    kBlue
  };
  
  // Color Method.
  enum ColorMethod
  {
    kByLayer =0xC0, 
    kByBlock,
    kByColor,
    kByACI,
    kByPen,
    kForeground,
    kLayerOff,
    // Run-time states
    kLayerFrozen,
    kNone
  };
  
  enum ACIcolorMethod
  {
    kACIbyBlock     = 0,
    kACIforeground  = 7,
    kACIbyLayer     = 256,
    // Run-time states
    kACIclear       = 0, 

    kACIRed         = 1,
    kACIYellow      = 2,
    kACIGreen       = 3,
    kACICyan        = 4,
    kACIBlue        = 5,
    kACIMagenta     = 6,

    kACIstandard    = 7,
    kACImaximum     = 255,
    kACInone        = 257,
    kACIminimum     = -255
    //kACIfrozenLayer = -32700
  };
  
  typedef OdUInt32 RGBM;
  
  inline OdCmEntityColor();
  inline OdCmEntityColor(const OdCmEntityColor & color);
  inline OdCmEntityColor(ColorMethod cm);
  inline OdCmEntityColor(OdUInt8 red, OdUInt8 green, OdUInt8 blue);
  
  inline OdCmEntityColor& operator =(const OdCmEntityColor& color);
  inline bool operator ==(const OdCmEntityColor& color) const;
  inline bool operator !=(const OdCmEntityColor& color) const;
  
  
  // Set/get components
  inline void setColorMethod(ColorMethod cm);
  inline ColorMethod colorMethod() const;
  
  void setColor(OdUInt32 color);
  inline OdUInt32 color() const; 
  
  inline void setColorIndex(OdInt16 colorIndex);
  inline OdInt16 colorIndex() const;
  
  //void setLayerIndex(OdInt16 layerIndex);
  //OdInt16 layerIndex() const; 
  
  //void setPenIndex(OdUInt16 penIndex);
  //OdUInt16 penIndex() const; 
  
  // Set/get RGB components
  inline void setRGB(OdUInt8 red, OdUInt8 green, OdUInt8 blue);
  inline void setRed(OdUInt8 red);
  inline void setGreen(OdUInt8 green);
  inline void setBlue(OdUInt8 blue);
  inline OdUInt8 red() const;
  inline OdUInt8 green() const;
  inline OdUInt8 blue() const;
  
  // Method check
  inline bool isByColor() const;
  inline bool isByLayer() const;
  inline bool isByBlock() const;
  inline bool isByACI() const;
  //bool isByPen() const;
  inline bool isForeground() const;
  //bool isLayerOff() const;
  // Run time states.
  inline bool isNone() const;
  //bool isLayerFrozen() const;
  //bool isLayerFrozenOrOff() const;
  
  // conversion
  OdUInt32 trueColor() const;
  OdUInt8 trueColorMethod() const;
  void setTrueColor();
  void setTrueColorMethod();
  
  // static methods for reuse in other classes.
  static void setColorMethod(RGBM* rgbm, ColorMethod cm);
  static ColorMethod colorMethod(const RGBM* rgbm);
  
  static void setColor(RGBM* rgbm, OdUInt32 color);
  static OdUInt32 color(const RGBM* rgbm); 
  
  static void setColorIndex(RGBM* rgbm, OdInt16 colorIndex);
  static OdInt16 colorIndex(const RGBM* rgbm);
  
  //static void setLayerIndex(RGBM* rgbm, OdInt16 layerIndex);
  //static OdInt16 layerIndex(const RGBM* rgbm); 
  
  //static void setPenIndex(RGBM* rgbm, OdUInt16 penIndex);
  //static OdUInt16 penIndex(const RGBM* rgbm); 

  // Set/get RGB components
  inline static void setRGB(RGBM* rgbm, OdUInt8 red, OdUInt8 green, OdUInt8 blue);
  inline static void setRed(RGBM* rgbm, OdUInt8 red);
  inline static void setGreen(RGBM* rgbm, OdUInt8 green);
  inline static void setBlue(RGBM* rgbm, OdUInt8 blue);
  inline static OdUInt8 red(const RGBM* rgbm);
  inline static OdUInt8 green(const RGBM* rgbm);
  inline static OdUInt8 blue(const RGBM* rgbm);
  
  // Method check
  inline static bool isByColor(const RGBM* rgbm);
  inline static bool isByLayer(const RGBM* rgbm);
  inline static bool isByBlock(const RGBM* rgbm);
  inline static bool isByACI(const RGBM* rgbm);
  //static bool isByPen(const RGBM* rgbm);
  inline static bool isForeground(const RGBM* rgbm);
  //static bool isLayerOff(const RGBM* rgbm);
  // Run time states.
  //static bool isLayerFrozen(const RGBM* rgbm);
  inline static bool isNone(const RGBM* rgbm);
  
  
  
  // Look up mapping between ACI and RGB.
  static OdUInt32 trueColor(const RGBM* rgbm);
  static OdUInt8 trueColorMethod(OdInt16);
  static void setTrueColor(RGBM* rgbm);
  static void setTrueColorMethod(RGBM* rgbm);
  
  static OdUInt32 lookUpRGB(OdUInt8 colorIndex);
  static OdUInt8 lookUpACI(OdUInt8 red, OdUInt8 green, OdUInt8 blue); 
  
protected:
  inline static OdInt16 indirect(const RGBM* rgbm); 
  inline static void setIndirect(RGBM* rgbm, OdInt16 Indirect);
  inline OdInt16 indirect() const; 
  inline void setIndirect(OdInt16 Indirect);
  RGBM m_RGBM;
  
public:
  // The Look Up Table(LUT) provides a mapping between ACI and RGB 
  // and contains each ACI's representation in RGB.
  static const OdUInt8 mLUT[256][3]; 
};

// OdCmEntityColor inline
//
inline OdCmEntityColor::ColorMethod OdCmEntityColor::colorMethod(const RGBM* rgbm)
{
  return OdCmEntityColor::ColorMethod((*rgbm >> 24) & 0xFF);
}

inline void OdCmEntityColor::setIndirect(RGBM* rgbm, OdInt16 Indirect)
{
  ODA_ASSERT(colorMethod(rgbm) != kByColor); 
  *rgbm = ((*rgbm & 0xFF000000) | (OdUInt32(Indirect) & 0x0000FFFF));
}

inline OdInt16 OdCmEntityColor::indirect(const RGBM* rgbm)
{
  ODA_ASSERT(colorMethod(rgbm) != kByColor); 
  return OdInt16((*rgbm) & 0x0000FFFF);
}

inline OdInt16 OdCmEntityColor::indirect() const { return indirect(&m_RGBM); }

inline void OdCmEntityColor::setIndirect(OdInt16 Indirect) { setIndirect(&m_RGBM, Indirect); }


// Default color method is kByColor.
inline OdCmEntityColor::OdCmEntityColor() : m_RGBM(0) { setColorMethod(kByColor); }

inline OdCmEntityColor::OdCmEntityColor(const OdCmEntityColor & color) : m_RGBM(color.m_RGBM) { }


// parameter: cm Color method information(byBlock, byLayer, byColor).
inline OdCmEntityColor::OdCmEntityColor(ColorMethod cm) : m_RGBM(0) { setColorMethod(cm); }

// Default color method is kByColor.
// parameter: red, green, blue
inline OdCmEntityColor::OdCmEntityColor(OdUInt8 red, OdUInt8 green, OdUInt8 blue)
{
  setColorMethod(kByColor);
  setRGB(red, green, blue);
}

inline OdCmEntityColor& OdCmEntityColor::operator=(const OdCmEntityColor & color)
{
  m_RGBM = color.m_RGBM;
  return *this;
}

inline bool OdCmEntityColor::operator==(const OdCmEntityColor& color) const
{
  return m_RGBM == color.m_RGBM;
}

inline bool OdCmEntityColor::operator!=(const OdCmEntityColor& color) const
{
  return m_RGBM != color.m_RGBM;
}

// get Color Method
inline OdCmEntityColor::ColorMethod OdCmEntityColor::colorMethod() const
{
  return colorMethod(&m_RGBM);
}

inline OdUInt32 OdCmEntityColor::color() const { return m_RGBM; }

inline void OdCmEntityColor::setRGB(OdUInt8 red, OdUInt8 green, OdUInt8 blue)
{
  setRGB(&m_RGBM, red, green, blue);
}

inline void OdCmEntityColor::setRed(RGBM* rgbm, OdUInt8 red)
{
  ODA_ASSERT(colorMethod(rgbm) == kByColor); 
  *rgbm &= 0xFF00FFFF;
  *rgbm |= RGBM(((OdUInt32)red << 16) & 0x00FF0000);
}

inline void OdCmEntityColor::setGreen(RGBM* rgbm, OdUInt8 green)
{
  ODA_ASSERT(colorMethod(rgbm) == kByColor); 
  *rgbm &= 0xFFFF00FF;
  *rgbm |= RGBM(((OdUInt32)green << 8) & 0x0000FF00);
}

inline void OdCmEntityColor::setBlue(RGBM* rgbm, OdUInt8 blue)
{
  ODA_ASSERT(colorMethod(rgbm) == kByColor); 
  *rgbm &= 0xFFFFFF00;
  *rgbm |= RGBM((blue) & 0x000000FF);
}

inline void OdCmEntityColor::setRGB(RGBM* rgbm, OdUInt8 red, OdUInt8 green, OdUInt8 blue)
{
  setColorMethod(rgbm, kByColor);
  setRed(rgbm, red);
  setGreen(rgbm, green);
  setBlue(rgbm, blue);
}

inline void OdCmEntityColor::setRed(OdUInt8 red) { setRed(&m_RGBM, red); }

inline void OdCmEntityColor::setGreen(OdUInt8 green) { setGreen(&m_RGBM, green); }

inline void OdCmEntityColor::setBlue(OdUInt8 blue) { setBlue(&m_RGBM, blue); }

inline OdUInt8 OdCmEntityColor::red(const RGBM* rgbm)
{
  ODA_ASSERT(colorMethod(rgbm) == kByColor); 
  return OdUInt8((*rgbm >> 16) & 0xFF);
}

inline OdUInt8 OdCmEntityColor::green(const RGBM* rgbm)
{
  ODA_ASSERT(colorMethod(rgbm) == kByColor); 
  return OdUInt8((*rgbm >> 8) & 0xFF);
}

inline OdUInt8 OdCmEntityColor::blue(const RGBM* rgbm)
{
  ODA_ASSERT(colorMethod(rgbm) == kByColor); 
  return OdUInt8(*rgbm & 0xFF);
}

inline OdUInt8 OdCmEntityColor::red() const { return red(&m_RGBM); }

inline OdUInt8 OdCmEntityColor::green() const { return green(&m_RGBM); }

inline OdUInt8 OdCmEntityColor::blue() const { return blue(&m_RGBM); }

inline bool OdCmEntityColor::isByColor(const RGBM* rgbm) { return colorMethod(rgbm) == kByColor; }

inline bool OdCmEntityColor::isByLayer(const RGBM* rgbm)
{
  return (colorMethod(rgbm) == kByLayer || (colorMethod(rgbm) == kByACI && indirect(rgbm) == kACIbyLayer)); 
}

inline bool OdCmEntityColor::isByBlock(const RGBM* rgbm)
{
  return (colorMethod(rgbm) == kByBlock || (colorMethod(rgbm) == kByACI && indirect(rgbm) == kACIbyBlock)); 
}

inline bool OdCmEntityColor::isByACI(const RGBM* rgbm)
{
  return colorMethod(rgbm) == kByACI;
}

inline bool OdCmEntityColor::isForeground(const RGBM* rgbm)
{
  return (colorMethod(rgbm) == kForeground || (colorMethod(rgbm) == kByACI && indirect(rgbm) == kACIforeground)); 
}

inline bool OdCmEntityColor::isNone(const RGBM* rgbm)
{
  return (colorMethod(rgbm) == kNone || (colorMethod(rgbm) == kByACI && indirect(rgbm) == kACInone)); 
}

inline bool OdCmEntityColor::isByColor() const { return isByColor(&m_RGBM); }

inline bool OdCmEntityColor::isByLayer() const { return isByLayer(&m_RGBM); }

inline bool OdCmEntityColor::isByBlock() const { return isByBlock(&m_RGBM); }

inline bool OdCmEntityColor::isByACI() const { return isByACI(&m_RGBM); }

inline bool OdCmEntityColor::isForeground() const { return isForeground(&m_RGBM); }

inline bool OdCmEntityColor::isNone() const { return isNone(&m_RGBM); }

inline OdInt16 OdCmEntityColor::colorIndex() const { return OdCmEntityColor::colorIndex(&m_RGBM); }

inline void OdCmEntityColor::setColorIndex(OdInt16 cIndex) { setColorIndex(&m_RGBM, cIndex); }

inline void OdCmEntityColor::setColorMethod(ColorMethod cm) { OdCmEntityColor::setColorMethod(&m_RGBM, cm); }


/** Description:

    {group:OdCm_Classes}
*/
class FIRSTDLL_EXPORT OdCmColorBase
{
public:
  ODRX_HEAP_OPERATORS();
  virtual ~OdCmColorBase() {}

  virtual OdCmEntityColor::ColorMethod  colorMethod() const = 0;
  virtual void                          setColorMethod(OdCmEntityColor::ColorMethod cm) = 0;

  virtual bool                isByColor() const = 0;
  virtual bool                isByLayer() const = 0;
  virtual bool                isByBlock() const = 0;
  virtual bool                isByACI()   const = 0;
  
  //virtual bool                isByPen ()  const = 0;

  virtual bool                isForeground()   const = 0;

  virtual OdUInt32            color() const = 0;
  virtual void                setColor(OdUInt32 color) = 0;

  virtual void                setRGB(OdUInt8 red, 
                                     OdUInt8 green, 
                                     OdUInt8 blue) = 0;
  virtual void                setRed(OdUInt8 red) = 0;
  virtual void                setGreen(OdUInt8 green) = 0;
  virtual void                setBlue(OdUInt8 blue) = 0;
  virtual OdUInt8             red  () const = 0;
  virtual OdUInt8             green() const = 0;
  virtual OdUInt8             blue () const = 0;

  virtual OdUInt16            colorIndex() const = 0;
  virtual void                setColorIndex(OdUInt16 colorIndex) = 0;
  //virtual OdUInt16            penIndex() const = 0;
  //virtual void                setPenIndex(OdUInt16 penIndex) = 0;

  virtual void                setNames(const OdString& colorName,
                                       const OdString& bookName = OdString()) = 0;
  virtual OdString            colorName() const = 0;
  virtual OdString            bookName () const = 0;
  virtual OdString            colorNameForDisplay() = 0;
  /* virtual bool                hasColorName() const = 0;
  virtual bool                hasBookName () const = 0; // Hmm...*/
};

class OdDbObject;
class OdDbFiler;

/** Description:

    {group:OdCm_Classes}
*/
class TOOLKIT_EXPORT OdCmColor : public OdCmColorBase
{
public:
  OdCmColor();
  OdCmColor(const OdCmColor&);
  OdCmColor(const OdCmColorBase&);
  OdCmColor(OdCmEntityColor::ColorMethod m);
  OdCmColor& operator=(const OdCmColor& inputColor);
  OdCmColor& operator=(const OdCmColorBase& inputColor);
  ~OdCmColor();

  bool operator ==(const OdCmColor& color) const;
  bool operator !=(const OdCmColor& color) const;
  bool operator ==(const OdCmColorBase& color) const;
  bool operator !=(const OdCmColorBase& color) const;
  
  OdString getDescription() const;
  OdString getExplanation() const;

  virtual OdCmEntityColor::ColorMethod  colorMethod() const;
  virtual void     setColorMethod(OdCmEntityColor::ColorMethod cm);

  virtual bool        isByColor() const;
  virtual bool        isByLayer() const;
  virtual bool        isByBlock() const;
  virtual bool        isByACI()   const;
  
  //virtual bool        isByPen ()  const;

  virtual bool        isForeground()   const;

  bool                isNone()    const;

  virtual OdUInt32    color() const;
  virtual void        setColor(OdUInt32 color);

  virtual void        setRGB  (OdUInt8 red, 
                               OdUInt8 green, 
                               OdUInt8 blue);
  virtual void        setRed  (OdUInt8 red);
  virtual void        setGreen(OdUInt8 green);
  virtual void        setBlue (OdUInt8 blue);
  virtual OdUInt8     red  () const;
  virtual OdUInt8     green() const;
  virtual OdUInt8     blue () const;

  virtual OdUInt16    colorIndex() const;
  virtual void        setColorIndex(OdUInt16 colorIndex);
  //virtual OdUInt16    penIndex() const;   
  //virtual void        setPenIndex (OdUInt16 penIndex);

  virtual void        setNames(const OdString& colorName,
                               const OdString& bookName = OdString());
  virtual OdString    colorName() const;
  virtual OdString    bookName () const;
  virtual OdString    colorNameForDisplay();
  /* virtual bool        hasColorName() const;
  virtual bool        hasBookName () const; // Hmm... */

  OdCmEntityColor     entityColor() const;
  OdString            getDictionaryKey() const;
  void                setNamesFromDictionaryKey(const OdString& sDictionaryKey);

  //
  void     dwgIn (OdDbDwgFiler* pInputFiler);
  void     dwgOut(OdDbDwgFiler* pOutputFiler) const;

  void     dxfIn (OdDbDxfFiler* pFiler, int groupCodeOffset = 0);
  void     dxfOut(OdDbDxfFiler* pFiler, int groupCodeOffset = 0) const;

  void     audit (OdDbAuditInfo* pAuditInfo);

  // For internal use
  void     dwgInAsTrueColor (OdDbDwgFiler* pInputFiler);
  void     dwgOutAsTrueColor(OdDbDwgFiler* pOutputFiler) const;

  /* void     serializeOut(void *pBuff, int *pBuflen) const;
  void     serializeIn (const void *pBuff, int *pBuflen);*/

  static const OdUInt16 MaxColorIndex;

  /*
  AcCmComplexColor* complexColor() const;
  void setComplexColor(AcCmComplexColor* pComplex);*/


private:

  /* enum NameFlags 
  {    
    kNoNames = 0x00,
    kHasColorName = 0x01,
    kHasBookName = 0x02
  };

  static const OdUInt16 ComplexColorFlag; 
  bool isComplex() const;*/

  OdCmEntityColor::RGBM   m_RGBM;
  OdString                m_colorName;
  OdString                m_bookName;
  // OdUInt8                 m_nameFlags;
};




/** Description:

    {group:OdCm_Classes}
*/
class OdCmTransparency 
{
public:
  ODRX_HEAP_OPERATORS();
  
  enum transparencyMethod 
  {
    kByLayer = 0,
    kByBlock,
    kByAlpha,      
      
    kErrorValue
  };
  
  OdCmTransparency() { setMethod(kByLayer); }
  OdCmTransparency(const OdCmTransparency& other) { m_AM = other.m_AM; }
  ~OdCmTransparency() {}
  
  OdCmTransparency& operator=(const OdCmTransparency& other) { m_AM = other.m_AM; return *this; }

  bool operator==(const OdCmTransparency& other) const { return (m_AM == other.m_AM); }
  bool operator!=(const OdCmTransparency& other) const { return (m_AM != other.m_AM); }
  
  void setAlpha(OdUInt8 alpha) 
  { 
    m_AM = ((OdUInt32)kByAlpha << 24) | alpha; 
  }

  transparencyMethod method() const 
  { 
    return transparencyMethod(OdUInt8(m_AM >> 24)); 
  }
  
  void setMethod(transparencyMethod method) 
  { 
    m_AM = (OdUInt32)method << 24; 
  }
  
  OdUInt8 alpha() const 
  { 
    ODA_ASSERT(method() == kByAlpha); 
    return OdUInt8(m_AM); 
  }

  bool isByAlpha() const { return (method() == kByAlpha); }
  bool isByBlock() const { return (method() == kByBlock); }
  bool isByLayer() const { return (method() == kByLayer); }
  bool isClear() const   { return (method() == kByAlpha) && (alpha() == 0);}
  bool isSolid() const   { return (method() == kByAlpha) && (alpha() == 255);}
  
  OdUInt32   serializeOut() const { return m_AM; }
  void       serializeIn(OdUInt32 val) { m_AM = val; }
  
private: 
  OdUInt32 m_AM;
};

#include "DD_PackPop.h"

#endif // ODA_CM_COLOR
