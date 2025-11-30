
#ifndef OD_AVELIGHT_H
#define OD_AVELIGHT_H

#include "DD_PackPush.h"

#include "RxObject.h"
#include "DbObjectId.h"
#include "IdArrays.h"
#include "AveDefs.h"
#include "Gs/GsLight.h"

class OdGePoint3d;
class OdGeVector3d;

class OdAveLightImpl;
class OdAveLight;
typedef OdSmartPtr<OdAveLight> OdAveLightPtr;


/** Description:

    {group:OdAve_Classes}
*/
class TOOLKIT_EXPORT OdAveLight : public OdGsLight
{
public:
  ODRX_DECLARE_MEMBERS(OdAveLight);

  /** Description:
      Create light object from BlockReference data.
  */
  static OdAveLightPtr getLight(const OdDbObjectId& id);
  static OdString getLightName(const OdDbObjectId& id);

  /** Description:
      Save light properties to BlockReference.
  */
  OdResult writeLight(OdDbDatabase* pDb = NULL);

  /** Description:
      The type of the light.
  */
  OdGsLight::LightType type() const;
  void setType(OdGsLight::LightType);

  /** Description:
      The name of the light.
      The name must be no more than eight characters long.
  */
  OdString name() const;
  void setName(const OdString&);

  /** Description:
      The intensity or brightness of the light.
      The intensity value can range from 0 (off) to 1 (full intensity).
  */
  double intensity() const;
  void setIntensity(double);

  /** Description:
      The color of the light.
  */
  OdUInt32 color() const;
  void setColor(OdUInt32);

  /** Description:
      The position and target of the light.
  */
  OdGePoint3d position() const;
  OdGePoint3d target() const;
  OdGeVector3d direction() const;
  void setPosition(const OdGePoint3d&);
  void setTarget(const OdGePoint3d&);
  void setDirection(const OdGePoint3d& ptFrom, const OdGePoint3d& ptTo);

  /** Description:
      Controls how light diminishes over distance.
  */
  OdGsLight::LightAttenuation attenuation() const;
  void setAttenuation(OdGsLight::LightAttenuation);

  /** Description:
      Specifies the cone angles for spotlight (OdAve::kSpotLight).
      Inner cone defines the brightest cone of light(hotspot, beam angle).
      Outer cone defines the full cone of light(falloff, field angle).
      These values can range from 0 to 80 degrees.
  */
  double innerConeAngle() const;
  double outerConeAngle() const;
  void setConeAngle(double inner, double outer);

  /** Description:
      Controls shadows.
  */
  bool isShadowOn() const;
  void setShadowOn(bool);

  /** Description:
      Controls volumetric shadows (Photo Real renderer)
      and raytraced shadows (Raytrace renderer) for this light.
      Shadow maps are not available,
      but the shadow map size and softness settings are retained.
  */
  bool isShadowVolumes() const;
  void setShadowVolumes(bool);

  /** Description:
      Controls the size, in pixels, of one side of the shadow map (a range of values between 64 and 4096).
      The larger the map size, the more accurate the shadows.
  */
  OdInt16 shadowMapSize() const;
  void shadowMapSize(OdInt16);

  /** Description:
      Controls the softness or fuzziness of shadow-mapped shadows.
      The value represents the number of pixels at the edge
      of the shadow that are blended into the underlying image.
      The value can range from 1 to 10.
  */
  double shadowSoftness() const;
  void setShadowSoftness(double);

  /** Description:
      Set of objects whose bounding box is used to clip the shadow maps.
  */
  void shadowBoundingObjects(OdDbObjectIdArray& ids) const;
  void setShadowBoundingObjects(const OdDbObjectIdArray& ids);
  void addShadowBoundingObject(const OdDbObjectId& id);

  /** Description:
      Return BlockReference id of the light.
  */
  OdDbObjectId holderId() const;

protected:
  /** Description:
      Constructors
  */
  OdAveLight();
  OdAveLight(OdAveLightImpl* pImpl);
  
  OdAveLightImpl* m_pImpl;
};

TOOLKIT_EXPORT OdRxVeObjectIteratorPtr oddbGetLights(OdDbDatabase* pDb);

#include "DD_PackPop.h"

#endif // OD_AVELIGHT_H


