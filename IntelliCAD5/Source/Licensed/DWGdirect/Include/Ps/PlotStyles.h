
// RxPlotStyle.h -- interfaces for setting up plotstyles support for rendering

#ifndef __OD_RX_PLOT_STYLE__
#define __OD_RX_PLOT_STYLE__

#include "RxObject.h"
#include "CmColor.h"
#include "RxModule.h"


class OdStreamBuf;

#include "DD_PackPush.h"

/** Description:

    {group:DD_Namespaces}
*/
namespace OdPs
{
  enum FillStyle
  {
    kFsSolid = 64,
    kFsCheckerboard,
    kFsCrosshatch,
    kFsDiamonds,
    kFsHorizontalBars,
    kFsSlantLeft,
    kFsSlantRight,
    kFsSquareDots,
    kFsVerticalBars,
    kFsUseObject,

    kFsLast = kFsUseObject
  };

  enum LineEndStyle
  {
    kLesButt = 0,
    kLesSquare,
    kLesRound,
    kLesDiamond,
    kLesUseObject
  };
  
  enum LineJoinStyle
  {
    kLjsMiter = 0,
    kLjsBevel,
    kLjsRound,
    kLjsDiamond,
    kLjsUseObject = 5
  };
  
  // plotstyle's linetype
  enum LineType
  {
    kLtpSolid = 0,
    kLtpDashed,
    kLtpDotted,
    kLtpDashDot,
    kLtpShortDash,
    kLtpMediumDash,
    kLtpLongDash,
    kLtpShortDashX2,
    kLtpMediumDashX2,
    kLtpLongDashX2,
    kLtpMediumLongDash,
    kLtpMediumDashShortDashShortDash,
    kLtpLongDashShortDash,
    kLtpLongDashDotDot,
    kLtpLongDashDot,
    kLtpMediumDashDotShortDashDot,
    kLtpSparseDot,
    kLtpISODash,
    kLtpISODashSpace,
    kLtpISOLongDashDot,
    kLtpISOLongDashDoubleDot,
    kLtpISOLongDashTripleDot,
    kLtpISODot,
    kLtpISOLongDashShortDash,
    kLtpISOLongDashDoubleShortDash,
    kLtpISODashDot,
    kLtpISODoubleDashDot,
    kLtpISODashDoubleDot,
    kLtpISODoubleDashDoubleDot,
    kLtpISODashTripleDot,
    kLtpISODoubleDashTripleDot,
    kLtpUseObject,

    kLtpLast = kLtpUseObject
  };

}

/** Description:

    {group:OdGs_Classes} 
*/
class OdPsPlotStyleData
{
protected:
  enum
  {
    kAdaptiveLinetype = 1,
    kGrayScaleOn      = 2,
    kDitherOn         = 4
  };
  OdCmEntityColor       m_color; 
  OdUInt16              m_flags;
  OdInt16               m_screening;
  OdInt16               m_physPenNum;
  OdInt16               m_virtPenNum;
  OdInt16               m_psLt;
  OdInt16               m_fillStyle;
  OdInt16               m_lineEndStyle;
  OdInt16               m_lineJoinStyle;
  double                m_dLinePatSz;
  double                m_lineweight;

public:

  OdPsPlotStyleData()
    : m_flags(0)
    , m_screening(100)
    , m_physPenNum(-1)
    , m_virtPenNum(-1)
    , m_dLinePatSz(0.0)
    , m_psLt(OdInt16(OdPs::kLtpUseObject))
    , m_lineweight(-1)
    , m_fillStyle(OdInt16(OdPs::kFsUseObject))
    , m_lineEndStyle(OdInt16(OdPs::kLesUseObject))
    , m_lineJoinStyle(OdInt16(OdPs::kLjsUseObject))
    , m_color(255, 255, 255) // reserved in plotstyles for "use entity"
  {}

  // accessors
  const OdCmEntityColor&  color() const { return m_color; }
  OdInt16                 physicalPenNumber() const { return m_physPenNum; }
  OdInt16                 virtualPenNumber() const { return m_virtPenNum; }
  
  /** Description:
      Valid range is 0 - 100, where 0 indicates white, and 100 indicates the 
      color at its full intensity.
  */
  int                     screening() const { return m_screening; }

  /** Description:
      Meaning is unknown.
  */
  double                  linePatternSize() const { return m_dLinePatSz; }
  OdPs::LineType          linetype() const { return OdPs::LineType(m_psLt); }
  bool                    isAdaptiveLinetype() const { return GETBIT(m_flags, kAdaptiveLinetype); }
  bool                    isGrayScaleOn() const { return GETBIT(m_flags, kGrayScaleOn); }
  bool                    isDitherOn() const { return GETBIT(m_flags, kDitherOn); }
  double                  lineweight() const { return m_lineweight; }
  OdPs::FillStyle         fillStyle() const { return OdPs::FillStyle(m_fillStyle); }
  OdPs::LineEndStyle      endStyle()  const { return OdPs::LineEndStyle(m_lineEndStyle); }
  OdPs::LineJoinStyle     joinStyle() const { return OdPs::LineJoinStyle(m_lineJoinStyle); }

  // transformers
  void setColor(const OdCmEntityColor& col) { m_color = col; }
  void setPhysicalPenNumber(OdInt16 physPenNum) { m_physPenNum = physPenNum; }
  void setVirtualPenNumber(OdInt16 virtPenNum) { m_virtPenNum = virtPenNum; }
  void setScreening(int val) { m_screening = OdInt16(val); }
  void setLinePatternSize(double dLinePatSz) { m_dLinePatSz = dLinePatSz; }
  void setLinetype(OdPs::LineType psLt) { m_psLt = OdInt16(psLt); }
  void setAdaptiveLinetype(bool value) { SETBIT(m_flags, kAdaptiveLinetype, value); }
  void setGrayScaleOn(bool value) { SETBIT(m_flags, kGrayScaleOn, value); }
  void setDitherOn(bool value) { SETBIT(m_flags, kDitherOn, value); }
  void setLineweight(double lineweight) { m_lineweight = lineweight; }
  void setFillStyle(OdPs::FillStyle fillStyle) { m_fillStyle = OdInt16(fillStyle); }
  void setEndStyle(OdPs::LineEndStyle lineEndStyle) { m_lineEndStyle = OdInt16(lineEndStyle); }
  void setJoinStyle(OdPs::LineJoinStyle lineJoinStyle) { m_lineJoinStyle = OdInt16(lineJoinStyle); }
};

/** Description:

    {group:Other_Classes}
*/
class FIRSTDLL_EXPORT OdPsPlotStyle : public OdRxObject
{
public:
  ODRX_DECLARE_MEMBERS(OdPsPlotStyle);

  virtual OdString  name() const = 0;

  virtual OdString  description() const = 0;

  virtual void      getData(OdPsPlotStyleData& data) const = 0;
};

typedef OdSmartPtr<OdPsPlotStyle> OdPsPlotStylePtr;

/** Description:

    {group:Other_Classes}
*/
class FIRSTDLL_EXPORT OdPsPlotStyleTable : public OdRxObject
{
public:  
  ODRX_DECLARE_MEMBERS(OdPsPlotStyleTable);

  virtual bool      isApplyScaleFactor() const = 0;

  virtual OdString  description() const = 0;

  virtual double    scaleFactor() const = 0;

  virtual bool      isDisplayCustomLineweightUnits() const = 0;

  virtual double    getLineweightAt(OdUInt32 index) const = 0;

  /** Description:
      Returns the plotstyle by name.
  */
  virtual OdPsPlotStylePtr plotStyleAt(const OdString& name) const = 0;

  /** Description:
      Returns the plotstyle by number.
  */
  virtual OdPsPlotStylePtr plotStyleAt(int number) const = 0;

};

typedef OdSmartPtr<OdPsPlotStyleTable> OdPsPlotStyleTablePtr;

/** Description:
    Provides for the loading of plotstyle (CTB/STB) files.

    {group:OdRx_Classes}
*/
class FIRSTDLL_EXPORT OdPsPlotStyleServices : public OdRxModule
{
public:
  ODRX_DECLARE_MEMBERS(OdPsPlotStyleServices);

  /** Description:
      Loads a plot style table.

      Arguments:
        pBuf (I) OdStreamBuf object containing the plotstyle file.
  */
  virtual OdPsPlotStyleTablePtr loadPlotStyleTable(OdStreamBuf* pBuf) = 0;
};

typedef OdSmartPtr<OdPsPlotStyleServices> OdPsPlotStyleServicesPtr;

#define ODPS_PLOTSTYLE_SERVICES_APPNAME "PlotStyleServices"

#include "DD_PackPop.h"

#endif // __OD_RX_PLOT_STYLE__
