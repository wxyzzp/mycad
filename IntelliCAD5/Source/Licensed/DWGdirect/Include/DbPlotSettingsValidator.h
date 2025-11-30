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



#ifndef OD_DBPLOTSETVAL_H
#define OD_DBPLOTSETVAL_H

//////////////////////////////////////////////////////////////////////////////
//
// Name:    dbplotsetval.h
//
// Remarks: This class exports access methods which validate data on an
//          OdDbPlotSettings object before actually setting the data on
//          the object.
//
//////////////////////////////////////////////////////////////////////////////

#include "RxObject.h"
#include "DbPlotSettings.h"
#include "OdArray.h"

class OdDbPlotSettings;

typedef OdDbPlotSettings::PlotPaperUnits PlotPaperUnits;
typedef OdDbPlotSettings::PlotRotation   PlotRotation;
typedef OdDbPlotSettings::PlotType       PlotType;
typedef OdDbPlotSettings::StdScaleType   StdScaleType;

/** Description:

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbPlotSettingsValidator : public OdRxObject
{
public:
  virtual void setPlotCfgName(
    OdDbPlotSettings* pPlotSet,
    const char* plotDeviceName,
    const char* mediaName = NULL) = 0;

  virtual bool setCanonicalMediaName(
    OdDbPlotSettings* pPlotSet,
    const char* mediaName) = 0;

  virtual void setPlotOrigin(
    OdDbPlotSettings *pPlotSet,
    double xCoordinate,
    double yCoordinate) = 0;

  virtual void setPlotPaperUnits(
    OdDbPlotSettings* pPlotSet,
    const PlotPaperUnits units) = 0;

  virtual void setPlotRotation(
    OdDbPlotSettings* pPlotSet,
    const PlotRotation rotationType) = 0;

  virtual void setPlotCentered(
    OdDbPlotSettings* pPlotSet,
    bool isCentered) = 0;

  virtual void setPlotType(
    OdDbPlotSettings* pPlotSet,
    const PlotType plotAreaType) = 0;

  virtual void setPlotWindowArea(
    OdDbPlotSettings* pPlotSet,
    double xmin,
    double ymin,
    double xmax,
    double ymax) = 0;

  virtual void setPlotViewName(
    OdDbPlotSettings* pPlotSet,
    const char* viewName) = 0;

  virtual void setUseStandardScale(
    OdDbPlotSettings* pPlotSet,
    bool useStandard) = 0;

  virtual void setCustomPrintScale(
    OdDbPlotSettings* pPlotSet,
    double numerator,
    double denominator) = 0;

  virtual void setCurrentStyleSheet(
    OdDbPlotSettings* pPlotSet,
    const char* styleSheetName) = 0;

  virtual void setStdScaleType(
    OdDbPlotSettings* pPlotSet,
    const StdScaleType scaleType) = 0;

  virtual void setStdScale(
    OdDbPlotSettings* pPlotSet,
    double standardScale) = 0;

  virtual void plotDeviceList(
    OdArray<const char*> & deviceList) = 0;

  virtual void canonicalMediaNameList(
    OdDbPlotSettings* pPlotSet,
    OdArray<const char*> & mediaList) = 0;

  virtual OdString getLocaleMediaName(
    OdDbPlotSettings *pPlotSet,
    const char*  canonicalName) = 0;

  virtual OdString getLocaleMediaName(
    OdDbPlotSettings *pPlotSet,
    int index) = 0;

  virtual void setClosestMediaName(
    OdDbPlotSettings* pPlotSet,
    double paperWidth,
    double paperHeight,
    const PlotPaperUnits units,
    bool matchPrintableArea) = 0;

  // Not very good function
  virtual void setMediaSize(
    OdDbPlotSettings* pPlotSet,
    OdString paperSize,
    double paperWidth,
    double paperHeight,
    const PlotPaperUnits units,
    double leftMargin,
    double topMargin,
    double rightMargin,
    double bottomMargin) = 0;

  virtual void plotStyleSheetList(
    OdArray<const char*> & styleList) = 0;

  virtual void refreshLists(
    OdDbPlotSettings* pPlotSet) = 0;

  virtual void setZoomToPaperOnUpdate(
    OdDbPlotSettings* pPlotSet,
    bool doZoom) = 0;

  virtual void setDefaultPlotConfig(
    OdDbPlotSettings* pPlotSet) = 0;
};

typedef OdSmartPtr<OdDbPlotSettingsValidator> OdDbPlotSettingsValidatorPtr;

#endif // OD_DBPLOTSETVAL_H


