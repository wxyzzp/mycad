
#ifndef OD_AVERENDER_H
#define OD_AVERENDER_H

#include "Ve/AveDefs.h"
#include "DbObjectId.h"

/** Description:

    {group:OdAve_Classes}
*/
class TOOLKIT_EXPORT OdAvePref
{
public:
  OdAvePref();

  OdAve::RenderType m_RenderType;
  OdAve::RenderDestination m_Destination;
  OdString m_Scene;
  OdInt16 m_nSubSampling;
  double m_dLightIconScale;
  double m_dSmoothingAngle;
  bool m_bSmoothShade;
  bool m_bShadows;
  bool m_bApplyMaterials;
  bool m_bCropWindow;
  bool m_bRenderCache;
  bool m_bQueryForSelections;
  bool m_bSkipRenderDlg;
};

/** Description:

    {group:OdAve_Classes}
*/
class TOOLKIT_EXPORT OdAveFullOpt
{
public:
  OdAveFullOpt();

  OdAve::RenderQuality m_RenderQuality;
  bool m_bDiscardBackFaces;
  bool m_bNegativeNormal;
};

/** Description:

    {group:OdAve_Classes}
*/
class TOOLKIT_EXPORT OdAveScanlOpt
{
public:
  OdAveScanlOpt();

  OdAve::AntiAliasing m_AntiAliasing;
  OdAve::TextureMapSampling m_TextureMapSampling;
  double m_dShadowDepthMinBias;
  double m_dShadowDepthMaxBias;
  double m_dAdaptSamplThreshold;
  bool m_bAdaptSampl;
  bool m_bDiscardBackFaces;
  bool m_bNegativeNormal;
};

/** Description:

    {group:OdAve_Classes}
*/
class TOOLKIT_EXPORT OdAveRaytrOpt : public OdAveScanlOpt
{
public:
  OdAveRaytrOpt();

  OdInt16 m_nRayTreeMaxDepth;
  double m_dRayTreeCutoffThreshold;
};

/** Description:

    {group:OdAve_Classes}
*/
class TOOLKIT_EXPORT OdAveRFileOpt
{
public:
  OdAveRFileOpt();

  OdInt16 m_nFileType; // BMP-1, PCX-2, PostScript-3, TGA-4, TIFF-5
  OdInt16 m_nX;
  OdInt16 m_nY;
  OdInt16 m_nAspectRatio;
  OdInt16 m_nColorBits1;
  OdInt16 m_nColorBits2;
  bool m_bGrayscale;
  bool m_bCompressed;
  OdInt16 m_nOrientation;
  OdInt16 m_nInterlace;
  OdInt16 m_nPostScriptImgSize;
};

/** Description:

    {group:OdAve_Classes}
*/
class TOOLKIT_EXPORT OdAveFogOpt
{
public:
  OdAveFogOpt();

  bool m_bFogEnabled;
  bool m_bFogBackground;
  double m_dNearDistance;
  double m_dFarDistance;
  double m_dNearPercentage;
  double m_dFarPercentage;
  OdUInt32 m_Color;
};

/** Description:

    {group:OdAve_Classes}
*/
class TOOLKIT_EXPORT OdAveBGOpt
{
public:
  OdAveBGOpt();
  ~OdAveBGOpt();

  OdAve::BackgroundType m_BgType;
  OdAveBitmap* m_pImage;
  bool m_bDefaultBackground;
  OdUInt32 m_SolidColor;
  OdUInt32 m_GrColor1;
  OdUInt32 m_GrColor2;
  OdUInt32 m_GrColor3;
  double m_dGrHorizon;
  double m_dGrHeight;
  double m_dGrRotation;
  bool m_bEnvBackground;
  OdString m_EnvImageName;
};

/** Description:

    {group:OdAve_Classes}
*/
class TOOLKIT_EXPORT OdAveRenderOption
{
public:
  bool getRenderOption(OdDbDatabase* pDb);
  OdResult writeRenderOption(OdDbDatabase* pDb);

  OdAvePref     m_Pref;
  OdAveFullOpt  m_FullOpt;
  OdAveScanlOpt m_ScanlOpt;
  OdAveRaytrOpt m_RaytrOpt;
  OdAveRFileOpt m_RFileOpt;
  OdAveFogOpt   m_FogOpt;
  OdAveBGOpt    m_BGOpt;
  
private:
  OdDbObjectId  m_ids[OdAve::kRndOptAmount];
};

#endif // OD_AVERENDER_H

