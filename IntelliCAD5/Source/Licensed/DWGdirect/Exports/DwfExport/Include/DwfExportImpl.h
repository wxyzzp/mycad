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
//  DwfExportImpl.h : definition of the CDwfExportImpl class
//
///////////////////////////////////////////////////////////////////////////////

#ifndef DWFEXPORTIMPL_DEFINED
#define DWFEXPORTIMPL_DEFINED

#include "whiptk/whip_toolkit.h"

#include "OdaCommon.h"
#include "Gs.h"

#include "DwfExport.h"

#define STL_USING_MAP
#define STL_USING_STRING
#include "OdaSTL.h"

#include "DbPlotSettings.h"

#ifdef _WIN32
#include <io.h>    // _access
#endif

#include "Dwf/dwf_core.h"
#include "Dwf/Utilities/VectorUtil.h"
#include "Dwf/Utilities/GuidGen.h"
#include "Dwf/Writer/IMP/FileResourceHandler.h"
#include "Dwf/EPlot/Writer/ePlotSectionManager.h"
#include "Dwf/EPlot/ePlotInterface.h"

using namespace DWFManifest;
using namespace DWFXmlObjects;
using namespace DWFEPlot;
using namespace DWFWriter;


typedef std::map<ODCOLORREF, int> colorIndexMap;


//class GsDwfDevice;
class OdDwfDevice;
class OdDbHostAppProgressMeter;


///////////////////////////////////////////////////////////

struct DwfLayoutPlotInfo
{
  OdDb::MeasurementValue    m_Measurement;
  OdDbPlotSettings::PlotPaperUnits            m_PlotPaperUnit;
  double                    m_dPaperWidth;
  double                    m_dPaperHeight;
  double                    m_dPrintableXmin;
  double                    m_dPrintableYmin;
  double                    m_dPrintableXmax;
  double                    m_dPrintableYmax;
  double                    m_dScaleUnit;
};

typedef struct SUB_RECT { 
  long left; 
  long top; 
  long right; 
  long bottom; 
} SubRECT;

//class OdDbDatabase;

class CDwfExportImpl
{
  // Members
  //
    
  const DwExportParams& m_Params;

  WT_File                 m_wtFile;

  const ODCOLORREF*       m_pPalette;       // Palette to be used. 

  colorIndexMap           m_colorIndexMap;    // Keeps RGB colors from the selected palette (Default 
                                              //  or Set by user) and corresponding color index
  bool                    m_bInkedArea;       // Sets on/off the Inked Area
                                              //  The Inked Area opcode specifies a rectangle, in logical 
                                              //  coordinates, in which all of the .dwf file’s geometric
                                              //  primitives fit tightly inside.
  bool                    m_bWtDefaulPalette; // True - if WHIPTK Default color map is enough // MKU 1/21/2004
                                              //  when displaying images;
                                              //  false - otherwise.
  bool                    m_bColorMapOptimize;// True - to set on the color map optimization; 
                                              //  as a result this one keeps the actual colors only.
                                              //  false - to set off the optimization 
                                              //  and to export whole color map always

  int                     m_nColors;          // If there is a reason to reject from WHIPTK Default color map
  WT_RGBA32 *             m_pColors;          //  it will be used for new color map to set up a list of 
                                              //  colors (a map) to be used when displaying images

  OdGsDevicePtr           m_pDevice;
  OdDwfDevice*            m_pDeviceDwf;

  // Environment
  SubRECT                 m_rectViewport;

  OdDbHostAppProgressMeter*   m_pProgressMeter;

  int                     m_Layout;
  bool                    m_bLandscape;       // Drawing orientation by model space extents

public:
    // Constructor/destructor

    CDwfExportImpl(const DwExportParams& pParams);
    virtual ~CDwfExportImpl();

    // Implementation

    bool run();

    // Accessors

    WT_File* getWtFile()
    {
      return &m_wtFile;
    }

//protected:

    void begin();               // To initialize environment, to open WtFile and to write WtFile Header data
    void end();                 // Termination
    void draw();                // To export data

    void initContext();         // To initialize environment
    bool initPlotting(int i);   // To initialize the plotting space
    void initColors();          // To initialize the color engine

    void openWtFile(const OdString& filename);
    void createDevice();

    void setPlottingSpace(int tag, EPlotPageRef& page, EPlotGraphicResourceRef& w2d);

    void writingDwf6Files();    // Creating a basic DWF 6 file

    bool isDwfVersion6();       // Checks if the DWF version is 06.00 and greater.


    void nonprocessRun();       // To optimize a presetting by nonprocess run (draw)
    void putInkedArea();
    void putDrawingInfo();      // Put into the file Drawing Info
    void putDrawingUnits();     // Define Drawing Units 
    void putEmbed();            // Embed Source File 
    void putViewInfo();
    void putPlotInfo();  // todo new
    void putBackground();       // Define Drawing Background
    void putColorMap(); // todo

    void putView(OdGsView* gsView, const char * Name = NULL);

    void pmInit();
    void pmRelease();
    void pmStart(const char* displayString);
    void pmStop();
    void pmMeterProgress();
    void pmSetLimit(int max);

public:
    // Auxiliary methods
    //

    const DwExportParams& getParams() const
    {
      return m_Params;
    }

    OdGsDevicePtr getDevice()
    {
      return m_pDevice;
    }

    //int getWtColorIndex(const ODCOLORREF rgb) const;
    bool getWtColorIndex(const ODCOLORREF rgb, int& color_index) const;
    int getWtDefaultColorIndex() const
    {
      return 1;   // todo // set color index opposite background color as default
    }
    void colorPresetting(ODCOLORREF color);

    const ODCOLORREF* getPalette() const
    {
      return m_pPalette;
    }

    // get plot paper size for current view
    void getPlotPaperSize( double& plotWidth, double& plotHeight );
};

#endif /* DWFEXPORTIMPL */

