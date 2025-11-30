
#ifndef OD_AVEMATERIAL_H
#define OD_AVEMATERIAL_H

#include "DD_PackPush.h"

#include "AveDefs.h"
#include "RxObject.h"
#include "DbEntity.h"
#include "IdArrays.h"

class OdDbObjectId;
class OdDbDatabase;
class OdAveMaterialImpl;
class OdAveMaterial;
typedef OdSmartPtr<OdAveMaterial> OdAveMaterialPtr;

/** Description:

    {group:OdAve_Classes}
*/
class TOOLKIT_EXPORT OdAveMaterial : public OdRxObject
{
public:
  ODRX_DECLARE_MEMBERS(OdAveMaterial);

  /** Description:
      Create material object from BlockReference data.
  */
  static OdAveMaterialPtr getMaterial(const OdDbObjectId& id);
  static OdString getMaterialName(const OdDbObjectId& id);

  /** Description:
      Save material properties to BlockReference.
  */
  OdResult writeMaterial(OdDbDatabase* pDb = NULL);

  /** Description:
      Attach/detach the material to(from) object/layer/ACI.
  */
  bool attachToObject(OdDbObjectId id);
  bool attachToLayer(OdDbObjectId id);
  bool attachToACI(OdUInt16 colorIndex);
  static void detachFromObject(OdDbObjectId id);
  static void detachFromLayer(OdDbObjectId id);
  static void detachFromACI(OdDbDatabase* pDb, OdUInt16 colorIndex);
  
  /** Description:
      Get material attached to object/layer/ACI.
  */
  static bool getAttachedMaterial(const OdDbObjectId& id, OdDbObjectId& matId);
  static bool getAttachedMaterial(OdDbDatabase* pDb, OdUInt16 colorIndex, OdDbObjectId& matId);
  static void getAciMaterialMap(OdDbDatabase* pDb, OdDbObjectIdArray& matIds);

  /** Description:
      Material type.
  */
  virtual OdAve::MaterialType type();

  /** Description:
      Return true if attribute is used in material.
  */
  bool isAttribUsed(OdAve::MaterialAttrib);

  /** Description:
      Material name and description.
  */
  OdString name() const;
  OdString description() const;

  void setName(const OdString&);
  void setDescription(const OdString&);

  /** Description:
      Adjusts the material's ambient color.
  */
  double ambientFactor() const;
  OdUInt32 ambientColor() const;
  bool ambientColorCurrent() const;
  bool ambientLock() const;

  void setAmbientFactor(double);
  void setAmbientColor(OdUInt32);
  void setAmbientColorCurrent(bool);
  void setAmbientLock(bool);

  /** Description:
      Adjusts the material's diffuse color.
  */
  double diffuseFactor() const;
  OdUInt32 diffuseColor() const;
  bool diffuseColorCurrent() const;
  OdAveBitmap& diffuseBitmap() const;

  void setDiffuseFactor(double);
  void setDiffuseColor(OdUInt32);
  void setDiffuseColorCurrent(bool);

  /** Description:
      Adjusts the material's specular color.
  */
  double specularFactor() const;
  OdUInt32 specularColor() const;
  bool specularColorCurrent() const;
  bool specularMirror() const;
  bool specularLock() const;
  OdAveBitmap& specularBitmap();
  const OdAveBitmap& specularBitmap() const;

  void setSpecularFactor(double);
  void setSpecularColor(OdUInt32);
  void setSpecularColorCurrent(bool);
  void setSpecularMirror(bool);
  void setSpecularLock(bool);

  /** Description:
      Adjusts the roughness or shininess of the material.
      Adjusting Roughness changes the size of the material's reflective highlight.
      The lower the level of roughness, the smaller the highlight. 
  */
  double roughness() const;
  void setRoughness(double);

  /** Description:
      Adjusts the transparency of the object.
      To use an opacity map with this material, set a file name in Bitmap Blend. 
  */
  double transparency() const;
  OdAveBitmap& transparencyBitmap() const;
  void setTransparency(double);

  /** Description:
      Adjusts refraction of the object.
      Refraction also affects the transparency of a material. 
  */
  double refraction() const;
  void setRefraction(double) const;

  /** Description:
      Makes the Bitmap Blend area available to specify the file name of a bump map. 
  */
  OdAveBitmap& bumpMap();
  const OdAveBitmap& bumpMap() const;

  ~OdAveMaterial();
protected:
  /** Description:
      Constructors
  */
  OdAveMaterial();
  OdAveMaterial(OdAveMaterialImpl* pImpl);
  
  OdAveMaterialImpl* m_pImpl;
};

/** Description:

    {group:OdAve_Classes}
*/
class TOOLKIT_EXPORT OdAveGraniteMaterial : public OdAveMaterial
{
public:
  ODRX_DECLARE_MEMBERS(OdAveGraniteMaterial);

  /** Description:
      The four colors that can be present.
  */
  enum GraniteColor
  {
    kFirst,
    kSecond,
    kThird,
    kFourth
  };

  /** Description:
      Material type.
  */
  virtual OdAve::MaterialType type();

  /** Description:
      Adjusts the four colors that can be present.
      To use fewer than four colors, set a color value equal to one of
      the other colors, or set its value to zero.
      Value specifies the relative amount of each color in the granite pattern. 
  */
  double   colorFactor(GraniteColor);
  OdUInt32 color(GraniteColor);

  void setColorFactor(GraniteColor, double);
  void setColor(GraniteColor, OdUInt32);

  /** Description:
      Defines the sharpness of the stone.
      A sharpness of 0 is completely blurred; that is, the four colors are averaged.
      With a sharpness of 1.0, the four colors are discrete. 
  */
  double sharpness();
  void setSharpness(double);

  /** Description:
      Defines the scale of the material relative to objects you attach it to.
      Smaller Scale values result in a finer texture and less graininess.
  */
  double scale();
  void setScale(double);

protected:
  /** Description:
      Constructors
  */
  OdAveGraniteMaterial();
  OdAveGraniteMaterial(OdAveMaterialImpl* pImpl);
};

typedef OdSmartPtr<OdAveGraniteMaterial> OdAveGraniteMaterialPtr;

/** Description:

    {group:OdAve_Classes}
*/
class TOOLKIT_EXPORT OdAveMarbleMaterial : public OdAveMaterial
{
public:
  ODRX_DECLARE_MEMBERS(OdAveMarbleMaterial);

  /** Description:
      Material type.
  */
  virtual OdAve::MaterialType type();

  /** Description:
      Adjusts the marble matrix and vein colors.
  */
  OdUInt32 stoneColor();
  OdUInt32 veinColor();
  void stoneColor(OdUInt32);
  void veinColor(OdUInt32);

  /** Description:
      Defines the degree of the vein's turbulence.
      High values result in more of the vein color, with more swirling.
      High values take longer to render; values in the range 1-10 are recommended.
  */
  OdUInt32 turbulence();
  void setTurbulence(OdUInt32);

  /** Description:
      Defines the sharpness of the veins, that is,
      the blending of color at the edges of the veins.
  */
  double sharpness();
  void setSharpness(double);

  /** Description:
      Defines the scale of the material relative to objects you attach it to.
      Larger Scale values result in more veins.
  */
  double scale();
  void setScale(double);

protected:
  /** Description:
      Constructors
  */
  OdAveMarbleMaterial();
  OdAveMarbleMaterial(OdAveMaterialImpl* pImpl);
};

typedef OdSmartPtr<OdAveMarbleMaterial> OdAveMarbleMaterialPtr;

/** Description:

    {group:OdAve_Classes}
*/
class TOOLKIT_EXPORT OdAveWoodMaterial : public OdAveMaterial
{
public:
  ODRX_DECLARE_MEMBERS(OdAveWoodMaterial);

  /** Description:
      Material type.
  */
  virtual OdAve::MaterialType type();

  /** Description:
      Adjusts the two colors of the wood grain.
  */
  OdUInt32 lightColor();
  OdUInt32 darkColor();
  void setLightColor(OdUInt32);
  void setDarkColor(OdUInt32);

  /** Description:
      Controls the proportion of light to dark rings in the wood.
      A ratio of 0 is almost completely dark; a ratio of 1.0 is almost completely light.
  */
  double lightDark();
  void setLightDark(double);

  /** Description:
      Specifies the number of rings in the wood relative to objects
      you attach the material to.
      Larger values result in a finer texture and less graininess.
  */
  double ringDensity();
  void setRingDensity(double);

  /** Description:
      Controls how much the width of the rings varies.
      A ring width of 0 gives completely uniform rings; a width of 1.0 gives
      the greatest variation.
  */
  double ringWidth();
  void setRingWidth(double);

  /** Description:
      Controls how irregular the shape of the rings is.
      A ring shape of 0 gives completely circular rings;
      a shape of 1.0 gives the most irregular rings.
  */ 
  double ringShape();
  void setRingShape(double);

  /** Description:
      Defines the scale of the wood grain relative to objects you attach it to.
      Larger Scale values result in finer grain.
  */
  double scale();
  void setScale(double);

protected:
  /** Description:
      Constructors
  */
  OdAveWoodMaterial();
  OdAveWoodMaterial(OdAveMaterialImpl* pImpl);
};

typedef OdSmartPtr<OdAveWoodMaterial> OdAveWoodMaterialPtr;

TOOLKIT_EXPORT OdRxVeObjectIteratorPtr oddbGetMaterials(OdDbDatabase* pDb);

#include "DD_PackPop.h"

#endif // OD_AVEMATERIAL_H


