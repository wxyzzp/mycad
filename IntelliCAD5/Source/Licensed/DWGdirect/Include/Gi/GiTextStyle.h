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



#ifndef __ODGITEXTSTYLE_H__
#define __ODGITEXTSTYLE_H__

#include "DD_PackPush.h"

#include "OdCodePage.h"
#include "OdFont.h"
#include "Gi/TtfDescriptor.h"

class OdGiWorldDraw;
class OdGiTextStyleImpl;
class OdString;
class OdDbDatabase;
class OdGePoint2d;


/** Description:

    {group:OdGi_Classes} 
*/
class FIRSTDLL_EXPORT OdGiTextStyle  // : public OdTtfDescriptor                         // MKU 14.02.2003
{
  // Members
  //
  OdTtfDescriptor       m_ttfDescriptor;

  mutable OdFontPtr     m_pFont;
  mutable OdFontPtr     m_pBigFont;

	mutable OdString      m_sFontPath;
	mutable OdString      m_sBigFontPath;

  OdString              m_sBigfontFile;
	double                m_dTextSize;
	double                m_dXScale;
	double                m_dObliquingAngle;
	double                m_dTrackingPercent;
  mutable OdUInt16      m_flags;
	mutable OdCodePageId  m_Codepage;
  // mutable bool          m_ShapeLoaded;
public:
  enum
  {
    kShape       = 0x01,
    kUpsideDown  = 0x02,
    kVertical    = 0x04,
    kUnderlined  = 0x08,
    kOverlined   = 0x10,
    kShxFont     = 0x20,   // 0 - ttf, 1 - shx
    kPreLoaded   = 0x40,
    kBackward    = 0x80,
    kShapeLoaded = 0x100
  };

  void setShapeLoaded(bool value) const;
  bool isShapeLoaded() const { return GETBIT(m_flags, kShapeLoaded); }

	OdGiTextStyle();
	~OdGiTextStyle();

	void set(const char* fontName,
			     const char*  bigFontName,
			     const double textSize,
			     const double xScale,
			     const double obliqueAngle,
 			     const double trPercent,
			     const bool isBackward,
			     const bool isUpsideDown,
			     const bool isVertical,
 			     const bool isOverlined,
           const bool isUnderlined);

  virtual void loadStyleRec(OdDbDatabase* pDb) const;
  OdString getFontFilePath(OdDbDatabase* pDb) const;
  OdString getBigFontFilePath(OdDbDatabase* pDb) const;
  void setFontFilePath(const OdString& path){m_sFontPath = path;}
  void setBigFontFilePath(const OdString& path){m_sBigFontPath = path;}
  
	void setTextSize(double size);
	void setXScale(double xScale);
	void setObliquingAngle(double obliquingAngle);

	void setTrackingPercent(double trPercent);

	void setBackward(bool isBackward);
	void setUpsideDown(bool isUpsideDown);
	void setVertical(bool isVertical);
	void setUnderlined(bool isUnderlined);
	void setOverlined(bool isOverlined);
  void setPreLoaded(bool value) const;
  void setShxFont(bool value) const;

	void setFileName(const char* fontName);
	void setBigFontFileName(const char* bigFontFileName);

	bool isBackward() const   { return GETBIT(m_flags, kBackward);  }
  bool isUpsideDown() const { return GETBIT(m_flags, kUpsideDown);}
	bool isVertical() const   { return GETBIT(m_flags, kVertical);  }
	bool isUnderlined() const { return GETBIT(m_flags, kUnderlined);}
	bool isOverlined() const  { return GETBIT(m_flags, kOverlined); }
  bool isPreLoaded() const  { return GETBIT(m_flags, kPreLoaded); }
  bool isShxFont() const    { return GETBIT(m_flags, kShxFont);   }
  bool isTtfFont() const    { return !isShxFont();  }

  void setIsShape(bool value) { SETBIT(m_flags, kShape, value); }
  bool isShape() const { return GETBIT(m_flags, kShape); }

	void setFont(const OdChar* pTypeface, bool bold, bool italic, int charset, int pitchAndFamily);
  void setFont(OdFont* font){m_pFont = font;}
  void setBigFont(OdFont* font){m_pBigFont = font;}

	void font(OdString& Typeface, bool& bold, bool& italic, int& charset, int& pitchAndFamily) const;
  OdFont* getFont() const     { return m_pFont;     }
	OdFont* getBigFont() const  { return m_pBigFont;  }

  OdTtfDescriptor& ttfdecriptor()             
  { 
    return m_ttfDescriptor; 
  }
  const OdTtfDescriptor& ttfdecriptor() const 
  { 
    return m_ttfDescriptor; 
  }

	OdString bigFontFileName() const   
  {
    return m_sBigfontFile; 
  }
	double textSize() const         
  { 
    return m_dTextSize; 
  }
	double xScale() const           
  { 
    return m_dXScale; 
  }
	double obliquingAngle() const   
  { 
    return m_dObliquingAngle; 
  }
	double trackingPercent() const  
  { 
    return m_dTrackingPercent;  
  }

//  void odExtentsBox(const OdChar* pStr, int nStrLen, OdUInt32 flags, OdDbDatabase* pDb, OdGePoint3d& min, OdGePoint3d& max, OdGePoint3d* pEndPos = NULL);     // MKU 02.06.2003

  OdCodePageId getCodePage() const
  {
    return m_Codepage;
  }
  void setCodePage(OdCodePageId cp)
  {
    m_Codepage = cp;  
  }
protected:
};

typedef OdSmartPtr<OdGiTextStyle> OdGiTextStylePtr;

#include "DD_PackPop.h"

#endif  // __ODGITEXTSTYLE_H__


