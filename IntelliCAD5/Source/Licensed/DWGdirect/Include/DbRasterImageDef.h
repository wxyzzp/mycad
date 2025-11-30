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



#ifndef __DBRASTERIMAGEDEF_H
#define __DBRASTERIMAGEDEF_H

#include "DD_PackPush.h"

// Forward declarations
//
class OdDbDictionary;

#include "DbObjectReactor.h"
#include "DbObject.h"
#include "Gi/GiRasterImage.h"
#include "Ge/GeVector2d.h"

/** Description:
    Dummy implementation of OdGiRasterImage interface.
    Class is intended to be used to preset image parameters to 
    OdDbRasterImageDef object without actual raster file loading.

    {group:OdGi_Classes} 
*/
class TOOLKIT_EXPORT OdGiRasterImageDesc : public OdGiRasterImage
{
  OdUInt32      m_width,
                m_height;
  Units         m_units;
  double        m_xPelsPerUnut,
                m_yPelsPerUnut;
public:
  static OdGiRasterImagePtr createObject(OdUInt32 width, OdUInt32 height, Units units = kNone, const OdGeVector2d& defResolution = OdGeVector2d::kIdentity);
  
  OdUInt32 pixelWidth() const;
  OdUInt32 pixelHeight() const;

  Units defaultResolution(double& xPelsPerUnut, double& yPelsPerUnut) const;

  // Dummy implementations, since it is needn't here
  
  /** Description:
      Retrieves the number of bits per pixel used for colors on the destination device or buffer.
  */
  virtual int colorDepth() const;

  /** Description:
      Returns the number of colors in the palette of the image.
  */
  virtual OdUInt32 numColors() const;

  /** Description:
      Returns the color by index from the palette of the image.
  */
  virtual ODCOLORREF color(OdUInt32 nIndex) const;

  virtual OdUInt32 paletteDataSize() const;

  /** Description:
      Returns the palette of the image in BMP format.
  */
  virtual void paletteData(OdUInt8* pBytes) const;

  /** Description:
      The number of bytes between the beginning of scan line N and
      the beginning of scan line N+1
  */
  virtual OdUInt32 scanLineSize() const;

  /** Description:
      Returns pointer on the scanline of the image by index without any stuff.

      Remarks:
      The number of accessible scanlines is equal to value 'height' (see pixelHeight() call).
      The number of accessible bytes  in the scanline is equal to scanLineSize() returning value.
      The scanline gotten by an index 0 is the first.
      The scanline gotten by an index (height - 1) is the last.
  */
  virtual void scanLines(OdUInt8* pBytes, OdUInt32 index, OdUInt32 numLines = 1) const;

  virtual const OdUInt8* scanLines() const;
  
  /** Description:
      applying Brightness, Contrast, Fade to the image's pixels  
  */
  virtual void applyBCF(double brightness, double contrast, double fade);

  /** Description:
      applying palette transformation to bitanal images.
      If image isn't bitonal - does nothing
  */
  virtual void applyBitonalPaletteTransform(ODCOLORREF traitsColor, ODCOLORREF bgColor);
};



/** Description:
    Represents a raster image definition object in an OdDbDatabase.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbRasterImageDef : public OdDbObject
{
public:
  ODDB_DECLARE_MEMBERS(OdDbRasterImageDef);

  typedef OdGiRasterImage::Units Units;

  OdDbRasterImageDef();
  
  OdResult subErase(bool erasing);
  void subHandOverTo(OdDbObject* pNewObj);
  void subClose();

  OdResult dwgInFields(OdDbDwgFiler* pFiler);

  void dwgOutFields(OdDbDwgFiler* pFiler) const;

  OdResult dxfInFields(OdDbDxfFiler* pFiler);

  void dxfOutFields(OdDbDxfFiler* pFiler) const;

  /** Description:
      Sets the name of the source file that contains the raster image (DXF 1).
  */
  void setSourceFileName(const OdChar* pPathName);

  /** Description:
      Returns the name of the source file that contains the raster image (DXF 1).
  */
  OdString sourceFileName() const;

  /** Description:
      TBC.
  */
  bool load(bool modifyDatabase = true);

  /** Description:
      TBC.
  */
  void unload(bool modifyDatabase = true);

  /** Description:
      Returns true if the loaded flag is set for this image, false otherwise (DXF 280).
  */
  bool isLoaded() const;

  /** Description:
      Returns the pizel size of the image (DXF 10).
  */
  OdGeVector2d size() const;

  OdGeVector2d resolutionMMPerPixel() const;

  /** Description:
      TBC.
  int entityCount(bool* pbLocked = NULL) const;
  void updateEntities() const;
  */

  /** Description:
      Returns the resolution units for this image (DXF 281).
  */
  OdGiRasterImage::Units resolutionUnits() const;

  /** Description:
      TBC.
  */
  OdGiRasterImagePtr image();

  /** Description:
      Create directly from in-memory image.
      pImage will be attached to this object.
      If pImage is not null sets 'loaded' flag to true.
  */
  void setImage(OdGiRasterImage* pImage, bool modifyDatabase = true);

  /** Description:
      Creates an image dictionary in the passed in database, if one is not already present.

      Arguments:
      pDb (I/O) Database in which to create the image dictionary.

      Return Value:
      The Object ID of the image dictionary in pDb (either the existing dictionary
      if one was already present, or the newly created dictionary).
  */
  static OdDbObjectId createImageDictionary(OdDbDatabase* pDb);

  /** Description:
      Returns the Object ID of the image dictionary in the passed in database.
  */
  static OdDbObjectId imageDictionary(OdDbDatabase* pDb);

  enum { kMaxSuggestNameSize = 2049 };

  /** Description:
      TBC.
  */
  static OdString suggestName(const OdDbDictionary* pImageDictionary, OdString pNewImagePathName);

  /*   comment this out for a while

  int colorDepth() const;
  OdGiSentScanLines* makeScanLines(
    const OdGiRequestScanLines* pReq,
    const OdGeMatrix2d& pixToScreen,
    OdGePoint2dArray* pActiveClipBndy = 0, // Data will be modified!
    bool draftQuality = false,
    bool isTransparent = false,
    const double brightness = 50.0,
    const double contrast = 50.0,
    const double fade = 0.0
    ) const;
  void setActiveFileName(const OdChar* pPathName);
  OdString searchForActivePath();
  OdString activeFileName() const;
  void embed();
  bool isEmbedded() const;
  OdString fileType() const;
  void setUndoStoreSize(unsigned int maxImages = 10);
  unsigned int undoStoreSize() const;
  bool imageModified() const;
  void setImageModified(bool modified);
  void loadThumbnail(OdUInt16& maxWidth, OdUInt16& maxHeight,
    OdUInt8* pPalette = 0, int nPaletteEntries = 0);
  void unloadThumbnail();
  void* createThumbnailBitmap(BITMAPINFO*& pBitmapInfo,
    OdUInt8 brightness = 50, OdUInt8 contrast = 50, OdUInt8 fade = 0);
  IeFileDesc* fileDescCopy() const;
  void getScanLines(OdGiBitmap& bitmap,
    double brightness = 50.0,
    double contrast = 50.0,
    double fade = 0.0) const;
  void openImage(IeImg*& pImage);
  void closeImage();
  static Oda::ClassVersion classVersion();
  RasterImageDefImp* ptrImp() const;
  RasterImageDefImp* setPtrImp(RasterImageDefImp* pImp);


private:
  RasterImageDefImp* mpImp;
     static Oda::ClassVersion mVersion;
  */
};

  /*   comment this out for a while
inline RasterImageDefImp*
OdDbRasterImageDef::ptrImp() const
{
  return mpImp;
}

inline RasterImageDefImp*
OdDbRasterImageDef::setPtrImp(RasterImageDefImp* pImp)
{
  RasterImageDefImp* oldImp=mpImp;
  mpImp=pImp;
  return oldImp;
}
  */



/** Description:
    Represents a raster image definition reactor object in an OdDbDatabase.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbRasterImageDefReactor : public OdDbObject
{
public:
  ODDB_DECLARE_MEMBERS(OdDbRasterImageDefReactor);

  OdDbRasterImageDefReactor();

  OdResult dwgInFields(OdDbDwgFiler* pFiler);

  void dwgOutFields(OdDbDwgFiler* pFiler) const;

  OdResult dxfInFields(OdDbDxfFiler* pFiler);

  void dxfOutFields(OdDbDxfFiler* pFiler) const;

  void erased(const OdDbObject* dbObj, bool pErasing = true);

  void modified(const OdDbObject* dbObj);

  enum DeleteImageEvent
  {
    kUnload = 1,
    kErase = 2
  };

  static void setEnable(bool enable);

  virtual bool onDeleteImage( const OdDbRasterImageDef* pImageDef,
                              DeleteImageEvent event,
                              bool cancelAllowed);

  /* coment this out for a while
     static Oda::ClassVersion classVersion();
private:

  OdDbRasterImageDefReactorImpl* mpImp;
     static Oda::ClassVersion mVersion;
  */
};

typedef OdSmartPtr<OdDbRasterImageDefReactor> OdDbRasterImageDefReactorPtr;

/** Description:

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbRasterImageDefTransReactor : public OdDbObjectReactor
{
protected:
  OdDbRasterImageDefTransReactor() {}
public:
  ODRX_DECLARE_MEMBERS(OdDbRasterImageDefTransReactor);

  virtual bool onDeleteImage( const OdDbRasterImageDef* pImageDef,
                              OdDbRasterImageDefReactor::DeleteImageEvent event,
                              bool cancelAllowed ) = 0;
};

/*   comment this for a while

// TBC.
class TOOLKIT_EXPORT OdDbRasterImageDefFileAccessReactor : public OdDbObjectReactor
{
protected:
  OdDbRasterImageDefFileAccessReactor() {}
public:
  ODRX_DECLARE_MEMBERS(OdDbRasterImageDefFileAccessReactor);

  virtual void onAttach(const OdDbRasterImageDef*, const OdChar* pPath) = 0;
  virtual void onDetach(const OdDbRasterImageDef*, const OdChar* pPath) = 0;
  virtual bool onOpen(const OdDbRasterImageDef*, const OdChar* pPath,
    const OdChar* pActivePath, bool& replacePath, OdString& replacementPath) = 0;

  virtual bool onPathChange(const OdDbRasterImageDef*,
    const OdChar* pPath, const OdChar* pActivePath,
    bool& replacePath, OdString& replacementPath) = 0;

  virtual void onClose(const OdDbRasterImageDef*, const OdChar* pPath) = 0;

  virtual void onDialog(OdDbRasterImageDef*,
    const OdChar* pCaption, const OdChar* pFileExtensions) = 0;
};

inline Oda::ClassVersion
OdDbRasterImageDef::classVersion()
{ return mVersion; }

inline Oda::ClassVersion
OdDbRasterImageDefReactor::classVersion()
{ return mVersion; }
*/


typedef OdSmartPtr<OdDbRasterImageDef> OdDbRasterImageDefPtr;

#include "DD_PackPop.h"

#endif // __DBRASTERIMAGEDEF_H


