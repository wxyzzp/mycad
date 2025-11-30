#ifndef _SVGEXPORT_H_INCLUDED_
#define _SVGEXPORT_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef _TOOLKIT_IN_DLL_
#ifdef DD_SVGEXPORT_DLL_EXPORTS
#define SVGEXPORT_API __declspec(dllexport)
#else
#define SVGEXPORT_API __declspec(dllimport)
#endif
#else
#define SVGEXPORT_API
#endif

/*
  SVG export device is derived from OdGsDevice. It is used for saving 
  DWG drawings in SVG format.

  Remarks:
  Before calling OdGsDevice::update(), OnSize() must be called,
    setting device output rectangle.
  
  SvgExport device supports following properties:

  o "Output" - of type OdStreamBuf, must be set to output stream.
  o "LineWeightScale" - size of pixel in device units, used for scaling lineweights.
  o "ImageBase" - path where to copy images 
      (if not set, absolute file paths are written to svg)
  o "ImageUrl" - prefix, to prepend to image name 
      (eg: "http://www.mysite.com/images/", or "../images/")
  o "ShxLineWeight" - line weight of shx font output
  o "Precision" - number of decimal digits in doubles 
      (as in printf("%.9g",...) - 9 digits ), default = 6
  o "BackGround" (ODCOLORREF) - if set to non-white a rectangle of given color 
      will be drawn behind the drawing, simulating background.
  o "GenericFontFamily" - font to substitute if correct is missing,
      following generic font families are supported in SVG:
      "serif", "sans-serif", "cursive", "fantasy", "monospace"
*/
extern "C" SVGEXPORT_API OdRxClass* createSvgDevice();

// initialization function must be called before any other 
extern "C" SVGEXPORT_API void SvgExportModuleInit();
// this function frees internally allocated resources
extern "C" SVGEXPORT_API void SvgExportModuleUnInit();

#endif
