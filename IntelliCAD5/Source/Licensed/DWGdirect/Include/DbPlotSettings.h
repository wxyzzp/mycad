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



#ifndef OD_DBPLOTSETTINGS_H
#define OD_DBPLOTSETTINGS_H

#include "DD_PackPush.h"

#include "DbObject.h"
#include "Ge/GePoint2d.h"

/** Description:
    Represents a plotsettings object in an OdDbDatabase.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbPlotSettings : public OdDbObject
{
public:
  ODDB_DECLARE_MEMBERS(OdDbPlotSettings);

  /** Description:
      Constructor (no arguments).
  */
  OdDbPlotSettings();

  enum PlotPaperUnits
  {
    kInches,
    kMillimeters,
    kPixels
  };

  enum PlotRotation
  {
    k0degrees,
    k90degrees,
    k180degrees,
    k270degrees
  };

  enum PlotType
  {
    kDisplay,
    kExtents,
    kLimits,
    kView,
    kWindow,
    kLayout
  };

  enum StdScaleType
  {
    kScaleToFit, // Scaled to Fit
    k1_128in_1ft, // 1/128" = 1'
    k1_64in_1ft, // 1/64" = 1'
    k1_32in_1ft, // 1/32" = 1'
    k1_16in_1ft, // 1/16" = 1'
    k3_32in_1ft, // 3/32" = 1'
    k1_8in_1ft, // 1/8" = 1'
    k3_16in_1ft, // 3/16" = 1'
    k1_4in_1ft, // 1/4" = 1'
    k3_8in_1ft, // 3/8" = 1'
    k1_2in_1ft, // 1/2" = 1'
    k3_4in_1ft, // 3/4" = 1'
    k1in_1ft, // 1" = 1'
    k3in_1ft, // 3" = 1'
    k6in_1ft, // 6" = 1'
    k1ft_1ft, // 1' = 1'
    k1_1, // 1:1
    k1_2, // 1:2
    k1_4, // 1:4
    k1_8, // 1:8
    k1_10, // 1:10
    k1_16, // 1:16
    k1_20, // 1:20
    k1_30, // 1:30
    k1_40, // 1:40
    k1_50, // 1:50
    k1_100, // 1:100
    k2_1, // 2:1
    k4_1, // 4:1
    k8_1, // 8:1
    k10_1, // 10:1
    k100_1, // 100:1
    k1000_1 // 1000:1
  };

  enum ShadePlotType 
  {
    kAsDisplayed,
    kWireframe,
    kHidden,
    kRendered
  };

  enum ShadePlotResLevel 
  {
    kDraft,
    kPreview,
    kNormal,
    kPresentation,
    kMaximum,
    kCustom
  };

  /** Description:
      Adds this plotsettings object to the plotsettings dictionary in the specified OdDbDatabase.
  */
  void addToPlotSettingsDict(OdDbDatabase *towhichDb);

  /** Description:
      Returns the name of this plotsettings object. (DXF 1)
  */
  OdString getPlotSettingsName() const;

  /** Description:
      Sets the name of this plotsettings object.
  */
  void setPlotSettingsName(const OdChar* plotSettingsName);

  /** Description:
      Returns the name of the system printer or plot configuration file for this object (DXF 2).
  */
  OdString getPlotCfgName() const;

  /** Description:
      Returns the printable area of the configured paper (in millimeters).
  */
  void getPlotPaperMargins(double& printableXmin, double& printableYmin,
                           double& printableXmax, double& printableYmax) const;

  /** Description:
      Returns the plot paper size for this object in millimeters (DXF 44, 45).
  */
  void getPlotPaperSize(double& paperWidth, double& paperHeight) const;

  /** Description:
      Returns canonical media or page setup name for this object (DXF 4).
  */
  OdString getCanonicalMediaName() const;

  /** Description:
      Returns the plot origin for this object (DXF 46, 47).
  */
  void getPlotOrigin(double& x, double& y) const;

  /** Description:
      Returns the plot paper units for this object (DXF 72).
  */
  OdDbPlotSettings::PlotPaperUnits plotPaperUnits() const;

  /** Description:
      Returns the plot viewport borders flag (DXF 70, bit 0x01).
  */
  bool plotViewportBorders() const;

  /** Description:
      Sets the plot viewport borders flag (DXF 70, bit 0x01).
  */
  void setPlotViewportBorders(bool plotViewportBorders);

  /** Description:
      Returns the plot plotstyles flag (DXF 70, bit 0x20).
  */
  bool plotPlotStyles() const;

  /** Description:
      Sets the plot plotstyles flag (DXF 70, bit 0x20).
  */
  void setPlotPlotStyles(bool plotPlotStyles);

  /** Description:
      Returns the show plotstyles flag (DXF 70, bit 0x02).
  */
  bool showPlotStyles() const;

  /** Description:
      Sets the show plotstyles flag (DXF 70, bit 0x02).
  */
  void setShowPlotStyles(bool showPlotStyles);

  /** Description:
      Returns the plot rotation (DXF 73).
  */
  OdDbPlotSettings::PlotRotation plotRotation() const;

  /** Description:
      Returns the plot centered flag (DXF 70, bit 0x04).
  */
  bool plotCentered() const;

  /** Description:
      Returns the plot hidden flag (DXF 70, bit 0x08).
  */
  bool plotHidden() const;

  /** Description:
      Sets the plot hidden flag (DXF 70, bit 0x08).
  */
  void setPlotHidden(bool plotHidden);

  /** Description:
      Returns the plot type (DXF 74).
  */
  OdDbPlotSettings::PlotType plotType() const;

  /** Description:
      Returns the plot window area (DXF 48, 49, 140, 141).
  */
  void getPlotWindowArea(double& xmin, double& ymin,
                         double& xmax, double& ymax) const;

  /** Description:
      Returns the plot view name (DXF 6).
  */
  OdString getPlotViewName() const;

  /** Description:
      Returns the use standard scale flag (DXF 70, bit 0x10).
  */
  bool useStandardScale() const;

  /** Description:
      Returns the custom print scale (DXF 142, 143).
  */
  void getCustomPrintScale(double& numerator, double& denominator) const;

  /** Description:
      Returns the current style sheet (DXF 7).
  */
  OdString getCurrentStyleSheet() const;

  /** Description:
      Returns the standard scale type (DXF 75).
  */
  OdDbPlotSettings::StdScaleType stdScaleType() const;

  /** Description:
      Returns the standard scale (DXF 147).
  */
  void getStdScale(double& scale) const;

  /** Description:
      Returns the scale lineweights flag (DXF 70, bit 0x40).
  */
  bool scaleLineweights() const;

  /** Description:
      Sets the scale lineweights flag (DXF 70, bit 0x40).
  */
  void setScaleLineweights(bool scaleLineweights);

  /** Description:
      Returns the print lineweights flag (DXF 70, bit 0x80).
  */
  bool printLineweights() const;

  /** Description:
      Sets the print lineweights flag (DXF 70, bit 0x80).
  */
  void setPrintLineweights(bool prOdIntLineweights);

  /** Description:
      TBC.
  */
  bool textFill() const;

  /** Description:
      TBC.
  */
  void setTextFill(bool textFill);

  /** Description:
      TBC.
  */
  int getTextQuality() const;

  /** Description:
      TBC.
  */
  void setTextQuality(int quality);

  /** Description:
      Returns the draw viewports first flag (DXF 70, bit 0x200).
  */
  bool drawViewportsFirst() const;

  /** Description:
      Sets the draw viewports first flag (DXF 70, bit 0x200).
  */
  void setDrawViewportsFirst(bool drawViewportsFirst);

  /** Description:
      Returns the model type flag (DXF 70, bit 0x400).
  */
  bool modelType() const;

  /** Description:
      Sets the model type flag (DXF 70, bit 0x400).
  */
  void setModelType(bool modelType);

  /** Description:
      Returns the top margin (DXF 43).
  */
  double getTopMargin() const;

  /** Description:
      Returns the right margin (DXF 42).
  */
  double getRightMargin() const;

  /** Description:
      Returns the bottom margin (DXF 41).
  */
  double getBottomMargin() const;

  /** Description:
      Returns the left margin (DXF 40).
  */
  double getLeftMargin() const;

  /** Description:
      Returns the paper image origin (DXF 148).
  */
  OdGePoint2d getPaperImageOrigin() const;

  /** Description:
      Sets the paper image origin (DXF 148).
  */
  void setPaperImageOrigin(OdGePoint2d origin);

  // New to acad2004 api

  /** Description:
      This function returns the type of shade plot 
      specified in the plot settings. (dxf 76)
  */
  OdDbPlotSettings::ShadePlotType shadePlot() const;

  /** Description:
      This function sets the plot settings for shaded plotting.
      @shadePlot Input type of shade plot 
  */
  void setShadePlot(OdDbPlotSettings::ShadePlotType shadePlot);

  /** Description:
      This method returns the shade plot resolution level 
      of the current AcDbPlotSettings object. This setting 
      specifies the resolution level at which shaded and 
      vectorized viewports will plot.
  */
  OdDbPlotSettings::ShadePlotResLevel shadePlotResLevel() const;

  /** Description:
      This method sets the shade plot resolution level of 
      the current AcDbPlotSettings object. This setting 
      specifies the resolution level at which shaded and 
      vectorized viewports will plot.
      @resLevel Input new shade plot resolution level
  */
  void setShadePlotResLevel(OdDbPlotSettings::ShadePlotResLevel resLevel);

  /** Description:
      This method returns the shade plot custom dpi of 
      the current AcDbPlotSettings object. This setting 
      specifies the user-defined resolution at which shaded 
      and vectorized viewports will plot. This setting should 
      only be used if the shade plot resolution level is set 
      to ShadePlotResLevel::kCustom.
  */
  OdInt16 shadePlotCustomDPI() const;

  /** Description:
      This method sets the shade plot custom dpi of the current 
      AcDbPlotSettings object. This setting specifies the 
      user-defined resolution at which shaded and vectorized 
      viewports will plot. This setting should only be used if 
      the shade plot resolution level is set to 
      ShadePlotResLevel::kCustom.
      @val Input new shade plot custom dpi 
  */
  void setShadePlotCustomDPI(OdInt16 val);

  virtual void getClassID(void** pClsid) const;

  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;

  virtual void subClose();

  virtual OdResult subErase(bool erasing);
  
  virtual void subHandOverTo(OdDbObject* newObject);

  virtual void copyFrom(const OdRxObject* pSrc);

};

typedef OdSmartPtr<OdDbPlotSettings> OdDbPlotSettingsPtr;

#include "DD_PackPop.h"

#endif


