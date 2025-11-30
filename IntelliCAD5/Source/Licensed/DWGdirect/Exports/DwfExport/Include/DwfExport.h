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

///////////////////////////////////////////////////////////////////////////////
//
// DwfExport.h - Open Design DWF Export definitions
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _DWF_EXPORT_INCLUDED_
#define _DWF_EXPORT_INCLUDED_


#ifdef  _TOOLKIT_IN_DLL_

#ifdef DWFEXPORT_EXPORTS
#define DWFEXPORT_TOOLKIT   __declspec(dllexport)

#else  // DWFEXPORT_EXPORTS
#define DWFEXPORT_TOOLKIT   __declspec(dllimport)

#endif // DWFEXPORT_EXPORTS

#else

#define DWFEXPORT_TOOLKIT

#endif  // _TOOLKIT_IN_DLL_


///////////////////////////////////////////////////////////////////////////////

#ifdef JPEG6B
#include "jpegcomp.h"
#endif

#ifndef ODA_LINUX_X86
#include <tchar.h>
#endif
#include "OdArray.h"
#include "OdString.h"


class OdDbDatabase;
class OdDbLayout;

// Output format types
//
typedef enum {
  DW_COMPRESSED_BINARY = 0,
  DW_UNCOMPRESSED_BINARY,
  DW_ASCII
} DW_FORMAT;


const long lMaxDwfResolution = 0x7FFFFFFE;  // Max X and Y size

// The version is calculated as follows: (major_version * 100) + minor_version.
const int  nDwf_v55 = 55;                   // DWF v5.5
const int  nDwf_v42 = 42;                   // DWF v4.2
const int  nDwf_v60 = 600;                  // DWF v6.0 


//
// DW_EXPORT_PARAMS - Export parameters
//

struct DwfPageData
{
  DwfPageData()
    : sLayout("")
    , sPageAuthor("")
    , sPageTitle("")
    , sPageSubject("")
    , sPageCompany("")
    , sPageComments("")
    , sPageReviewers("")
    , sPageKeywords("")
    , sPageDescription("")
    , sPageCopyright("")
  {
  }

  DwfPageData(const DwfPageData& data)
  {
    Copy(data);
  }

  void Copy(const DwfPageData& from)
  {
    sLayout         = from.sLayout;
    sPageAuthor     = from.sPageAuthor;
    sPageTitle      = from.sPageTitle;
    sPageSubject    = from.sPageSubject;
    sPageCompany    = from.sPageCompany;
    sPageComments   = from.sPageComments;
    sPageReviewers  = from.sPageReviewers;
    sPageKeywords   = from.sPageKeywords;
    sPageDescription= from.sPageDescription;
    sPageCopyright  = from.sPageCopyright;

    thumbnail = from.thumbnail;
    preview   = from.preview;
    arFonts   = from.arFonts;
    // todo
  }

  OdString  sLayout;

  // list of Page (section) data
  OdString  sPageAuthor;
  OdString  sPageTitle;
  OdString  sPageSubject;
  OdString  sPageCompany;
  OdString  sPageComments;
  OdString  sPageReviewers;
  OdString  sPageKeywords;
  OdString  sPageDescription;
  OdString  sPageCopyright;

  // The <ImageResource> element is used to describe an image (raster) file resource.
  struct ImageResource
  {
    ImageResource()
      : sFileName("")
      , ColorDepth(0)
      , Width(0)
      , Height(0)
    {
    };

    ImageResource(const ImageResource& data)
    {
      Copy(data);
    }

    void Copy(const ImageResource& from)
    {
      sFileName = from.sFileName;
      ColorDepth = from.ColorDepth;
      Width = from.Width;
      Height = from.Height;
    }

    OdString  sFileName;
    int       ColorDepth;       // the number of colors used in the image
    int       Width;            // in orger to set the extents of the 
    int       Height;           //  image, in pixels
  };

  // The <FontResource> element is used to describe embedded fonts.
  //
  //  Note: All attributes are required.
  struct FontResource
  {
    FontResource()
      : sFileName("")
      , sCanonicalName("")
      , sLogfontName("")
    {
    };

    FontResource(const FontResource& data)
    {
      Copy(data);
    }

    void Copy(const FontResource& from)
    {
      sFileName =     from.sFileName;;
      request =       from.request;      
      privilege =     from.privilege;    
      characterCode = from.characterCode;
      sCanonicalName = from.sCanonicalName;
      sLogfontName =  from. sLogfontName; 
    }


    enum Request  // A enumeration  denoting the request by which the font information was embedded. This 
                  //  field could be one or more of the folloung Request values by which font information 
                  //  would have been embedded by Microsoft's Open Type Font Embedding SDK.
    {
        Raw              = 0x00000001   // The font structure contains the full character set, 
                                        //  non-compressed. This is the default behavior.
      , Subset           = 0x00000002   // The font structure is a subset containing only the glyphs 
                                        //  indicated. The character codes are denoted as 16-bit Unicode 
                                        //  values.
      , Compressed       = 0x00000004   // The font structure is compressed
      , Fail_If_Variations_Simulated = 0x00000008   // In some cases, a client will want to avoid embedding 
                                                    //  simulated fonts, especially if the normal instance 
                                                    //  of the typeface is being embedded. If this flag is 
                                                    //  set and the font in the DC is simulated, 
                                                    //  TTEmbedFont( ) will fail.
      , Eudc             = 0x00000010   // Embed EUDC font. If there is typeface EUDC embed it; 
                                        //  otherwise embed system EUDC.
      , Validation_Tests = 0x00000020   // The validity of font file should be confirmed before embedding.
      , Web_Object       = 0x00000040   // Not used.
      , Encrypt_Data     = 0x00000080   // Font data in the embedded object was encrypted when compression 
                                        //  was being performed.
    };

    enum Privilege        // A enumeration indicating the embedding privileges of the font. 
                          //  This flag can have one of the following values:
    {
        PreviewPrint  = 0x00000001      // Preview and Print Embedding.
      , Editable      = 0x00000002      // Editable Embedding.
      , Installable   = 0x00000003      // Installable Embedding.
      , Non_Embedding = 0x00000004      // Restricted License Embedding.
    };

    enum CharacterCode    // A enumeration specifying the character set of the font to be embedded.
                          //  This flag can have one of the following values: 
    {
        Unicode  = 0x00000001   // Unicode character set, requiring 16-bit character encoding.
      , Symbol   = 0x00000002   // Symbol character set, requiring 16-bit character encoding.
      , Glyphidx = 0x00000003   // Indicates that subset values passed are to be interpreted as glyph 
                                //  id’s (rather than unicode values).
    };

    // The required font attributes.
    OdString  sFileName;
    int       request;        // Font embedding request values
    int       privilege;      // The license granted to the font embedding app. 
    int       characterCode;  // Font embedding character set values.
    OdString  sCanonicalName; // the full descriptive font name
    OdString  sLogfontName;   // a shorter logfont name (32 byte character length restriction applies)
  };

  ImageResource         thumbnail;  // Raster Graphics (e.g. .jpeg, .png)
  ImageResource         preview;    // Raster Graphics (e.g. .jpeg, .png)

  OdArray<FontResource> arFonts;    // the list of the embedded fonts
};


class DwExportParams 
{
public:

  DwExportParams()
    : pDb(NULL)
    , sDwfFileName("")
    , xSize(36000)
    , ySize(24000)
    , Format(DW_ASCII)
    , Version(nDwf_v55)
    , background(ODRGB(255, 255, 255))
    , pPalette(NULL)
    , bExportInvisibleLayers(false)
    , bForceInitialViewToExtents(false)
    , bSkipLayerInfo(false)
    , bSkipNamedViewsInfo(false)
    , bInkedArea(false)                                                                   // MKU 1/21/2004
    , bColorMapOptimize(false)                                                            // MKU 1/21/2004
    , nMaxPointsInPolygon(0)
    , nMaxRasterResolution(0)
#ifdef JPEG6B
    , Params.pMakeJpeg(MakeJpeg)  // Compress to JPEG function
#endif
//    , sDescription(_T(""))
//    , sAuthor(_T(""))               // If _T("") and not present in XRECORD - current user name is substituted
//    , sComments(_T(""))
//    , sCopyright(_T(""))
//    , sCreator(_T(""))              // If NULL - "Open Design DWF Export" is substitued
//    , sKeywords(_T(""))
//    , sTitle(_T(""))
//    , sSubject(_T(""))
//    , sSourceFileName(_T(""))
    // MKU 11.06.2003   - DWF 6
    , sPassword("")
    , sPublisher("")
    , sWideComments("")
    , sSourceProductName("")
    /*
    // Page data
    , pPageAuthor(NULL)
    , pPageTitle(NULL)
    , pPageCompany(NULL)
    , pPageComments(NULL)
    , pPageReviewers(NULL)
    , pPageKeywords(NULL)
    , pPageDescription(NULL)
    , pPageCopyright(NULL)
    */
  {
  }

  ~DwExportParams()
  {
  }

  OdDbDatabase* pDb;
  OdString      sDwfFileName;

  long          xSize;            // DWF file resolution (less resolution - less file size
  long          ySize;            //  

  // Optional parameters. May be zeroed to use defaults
  //
  DW_FORMAT     Format;           // Output format (compressed, binary, ASCII)
  int           Version;          // DWF version (currently 42 or 55)

  ODCOLORREF  background;          // Background color. Default - white
  const ODCOLORREF* pPalette;      // Palette to be used. It's size depends on the highest
                                  //  index in ad3parm.colors array.
                                  //  If NULL - one of two default palettes
                                  //  will be used depending on background color

  bool          bExportInvisibleLayers;     // If non-zero invisible and frozen layers will be exported
  bool          bForceInitialViewToExtents; // If non-zero Initial view will be set
                                            //  to extents else - active view saved in drawing
  bool          bSkipLayerInfo;             // Don't save layer info
  bool          bSkipNamedViewsInfo;        // Don't save named views
  bool          bInkedArea;                 // Sets on/off the Inked Area
                                            //  The Inked Area opcode specifies a rectangle, in logical 
                                            //  coordinates, in which all of the .dwf file’s geometric
                                            //  primitives fit tightly inside.
                                            // ATTENTION: The slowing-down of performance is possible 
                                            //            in case that this option is set on.
  bool          bColorMapOptimize;          // True - to set on the color map optimization; 
                                            //  as a result this one keeps the actual colors only.
                                            //  false - to set off the optimization 
                                            //  and to export whole color map always
                                            // ATTENTION: The slowing-down of performance is possible 
                                            //            in case that this option is set on.

  unsigned long nMaxPointsInPolygon;        // Max number of points in polygon 
                                            //  Default is 0 - polygons will be triangulated to tristrips.
  /*
   * DWF format describes polygon entity with
   * great (>65000) number of vertices. It's the most compact representation of
   * filled areas (more compact that polytriangles).
   * But AutoDesk's viewer which probably will be used to display DWF files
   * has problems with rendering polygons.
   * First it does not render correctly polygons with number of vertices > 64.
   * It often crashes while zooming polygons with more than 32 vertices.
   * BTW AutoCAD 2000 does not export filled polygons to DWF (tristrips only)
   */

  unsigned short nMaxRasterResolution;      // If non zero sets the limit of raster image resolution
  
#if 0
    long          (*pMakeJpeg)          /* If specified, this function is used */
                  (unsigned int Height, /* to save JPEG images in DWF file     */
                   unsigned int Width,  /* rather than RGB - less output size  */
                   const AD_RASCOL * pRGB,
                   BYTE ** pJpegData);                                                                                  
#endif

  // The following strings if specified are included into DrawingInfo header
  //  By default (if NULL) they are filled from DWGPROPS XRECORD
//  OdString        sDescription;
//  OdString        sAuthor;      // If NULL and not present in XRECORD - current user name is substituted
//  OdString        sComments;
//  OdString        sCopyright;
//  OdString        sCreator;     // If NULL - "Open Design DWF Export" is substitued
//  OdString        sKeywords;
//  OdString        sTitle;
//  OdString        sSubject;
//  OdString        sSourceFileName;

  // MKU 11.06.2003   - DWF 6

  OdString        sPassword;
  // dwf-wide metadata
  OdString        sPublisher;
  OdString        sWideComments;
  OdString        sSourceProductName;         // Brief description of the application from 
                                              //  which the source data was created
  OdArray<DwfPageData>  arPageData;

}; // DwExportParams



DWFEXPORT_TOOLKIT void exportDwf(const DwExportParams& pParams);

#endif // _DWF_EXPORT_INCLUDED_


