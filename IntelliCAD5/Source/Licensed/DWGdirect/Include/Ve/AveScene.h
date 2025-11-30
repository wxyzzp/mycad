
#ifndef OD_AVESCENE_H
#define OD_AVESCENE_H

#include "DD_PackPush.h"

#include "RxObject.h"
#include "DbObjectId.h"
#include "DbEntity.h"

class OdAveSceneImpl;
class OdAveScene;
typedef OdSmartPtr<OdAveScene> OdAveScenePtr;

/** Description:

    {group:OdAve_Classes}
*/
class TOOLKIT_EXPORT OdAveScene : public OdRxObject
{
public:
  ODRX_DECLARE_MEMBERS(OdAveScene);

  /** Description:
      Create scene object from BlockReference data.
  */
  static OdAveScenePtr getScene(const OdDbObjectId& id);
  static void getSceneName(const OdDbObjectId& id, OdString& name);

  /** Description:
      Save scene properties to BlockReference.
  */
  OdResult writeScene(OdDbDatabase* pDb = NULL);

  /** Description:
      The name of the scene.
  */
  OdString name() const;
  void setName(const OdString&);

  /** Description:
      The view of the scene.
  */
  OdDbObjectId view() const;
  void setView(const OdDbObjectId&);

  /** Description:
      The lights of the scene.
  */
  void lights(OdDbObjectIdArray& ids) const;
  void setLights(const OdDbObjectIdArray& ids);
  void addLight(const OdDbObjectId& id);

protected:
  /** Description:
      Constructors
  */
  OdAveScene();
  OdAveScene(OdAveSceneImpl* pImpl);
  
  OdAveSceneImpl* m_pImpl;
};

TOOLKIT_EXPORT OdRxVeObjectIteratorPtr oddbGetScenes(OdDbDatabase* pDb);

#include "DD_PackPop.h"

#endif // OD_AVESCENE_H

