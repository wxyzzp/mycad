#ifndef _DWFExtentManager_Included_
#define _DWFExtentManager_Included_

class DwfImporter;
/////////////////////////////////////////////////////
// Extent, scale & paper management
//
class DwfExtentManager
{
  DwfImporter* _importer;
  // current layout paper desc
  struct PaperInfo
  {
    OdDbPlotSettings::PlotPaperUnits _units;
    double _width;
    double _height;
  }
  _paperInfo;
  // transformation: DWF -> DWG
  struct ScaleInfo
  {
    double _xOffset;
    double _yOffset;
    double _scale;
  }
  _scaleInfo;
  // center plot on paper (like viewers do)
  void centerPlot(double paperWidth, double paperHeight, int extHeight, int extWidth);
  // current clipping rect (used while calculating extents)
  WT_Logical_Box _clipRect;
public:
  DwfExtentManager(DwfImporter* importer) : _importer(importer), _clipRect(0,0,0,0){}
  // logical extent of the current W2D
  WT_Logical_Box _extent;
  // first pass W2D reading -> calculate extents
  bool _collectBounds;
  // update _extent (used in first pass DWF reading,
  // after the first pass, extent includes all drawables & viewports)
  void updateBounds(WT_Drawable& d, WT_File& file);
  // store dwf paper size
  void setPaperInfo(double wPaper, double hPaper, OdDbPlotSettings::PlotPaperUnits units);
  // set current layout paper
  void setPlotSettings(double wPaper, double hPaper, 
    OdDbPlotSettings::PlotPaperUnits units = OdDbPlotSettings::kMillimeters);
  // calculate scale & translation based on paper size & DWF extent
  void calculateScale();
  // transform size from logical DWF coords to DWG
  double transformSize(double);
  // transform DWF point to DWG
  OdGePoint3d transformPoint(const WT_Logical_Point& wtPoint);
  //
  void clear();
  //
  void setClipRect(const WT_Logical_Box& clipRect){_clipRect = clipRect;}
};

#endif
