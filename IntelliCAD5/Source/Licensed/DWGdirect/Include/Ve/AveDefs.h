
#ifndef OD_AVEDEFS_H
#define OD_AVEDEFS_H

#include "OdString.h"
#include "RxIterator.h"

typedef OdRxIteratorPtr OdRxVeObjectIteratorPtr;

/** Description:

    {group:DD_Namespaces}
*/
namespace OdAve
{
  enum MaterialType
  {
    kStandard,
    kGranite,
    kMarble,
    kWood
  };

  enum MaterialAttrib
  {
    kAmbient        = 0x1,  
    kDiffuse        = 0x2,  
    kSpecular       = 0x4,  
    kTransparency   = 0x8,  
                    //0x10, 
    kRoughness      = 0x20, 
    kBumpMap        = 0x40, 
                    //0x80, 
    kReflectionProp = 0x100,
    kRefraction     = 0x200
  };

  enum RenderType
  {
    kRndSimple = 30,
    kRndPhotoReal = 50,
    kRndRaytrace = 60
  };

  enum RenderDestination
  {
    kDestViewport = 20,
    kDestWindow = 30,
    kDestFile = 40
  };

  enum RenderQuality
  {
    kGouraud,
    kPhong 
  };

  enum AntiAliasing
  {
    kAntiAliasNone = 1,
    kAntiAliasMinimal,
    kAntiAliasLow,
    kAntiAliasMedium,
    kAntiAliasHigh
  };

  enum TextureMapSampling
  {
    kTMapSampPoint =1,
    kTMapSampLinear,
    kTMapSampMipMap
  };

  enum BackgroundType
  {
    kBackgroundSolid = 0,
    kBackgroundGradient,
    kBackgroundImage,
    kBackgroundMerge
  };

  enum OptionType
  {
    kRndPref,
    kRndFullOpt,
    kRndQuickOpt,
    kRndScanlOpt,
    kRndRaytrOpt,
    kRndRFileOpt,
    kRndFogOpt,
    kRndBGOpt,
    kRndOptAmount
  };

  enum ProjectionMethod
  {
    kPlanar = 1,
    kBox,
    kCylinder,
    kSphere
  };

  enum TileMethod
  {
    kTmTile = 1,
    kTmCrop,
    kTmClamp 
  };
}

/** Description:

    {group:OdAve_Classes}
*/
class OdAveBitmap
{
public:
  virtual ~OdAveBitmap() {}

  virtual double blendFactor() = 0;
  virtual void source(OdString&) = 0;
  virtual double scaleU() = 0;
  virtual double scaleV() = 0;
  virtual double offsetU() = 0;
  virtual double offsetV() = 0;
  virtual OdAve::TileMethod tileMethod() = 0;
  virtual bool useAutoAxis() = 0;
  virtual bool fitToObject() = 0;

  virtual void setBlendFactor(double) = 0;
  virtual void setSource(const OdString&) = 0;
  virtual void setScaleU(double) = 0;
  virtual void setScaleV(double) = 0;
  virtual void setOffsetU(double) = 0;
  virtual void setOffsetV(double) = 0;
  virtual void setTileMethod(OdAve::TileMethod) = 0;
  virtual void setUseAutoAxis(bool) = 0;
  virtual void setFitToObject(bool) = 0;
};

#endif // OD_AVEDEFS_H

