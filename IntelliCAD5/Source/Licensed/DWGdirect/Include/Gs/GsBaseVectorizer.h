#ifndef ODGSBASEVECTORIZER_INC
#define ODGSBASEVECTORIZER_INC

#include "DD_PackPush.h"

#include <stdlib.h>
#include <utility>

#include "Ge/GeExtents3d.h"

#include "Gi/GiDrawable.h"
#include "Gi/GiBaseVectorizer.h"
#include "Gi/GiStack.h"

#include "Gs/GsExport.h"
#include "Gs/GsModel.h"
#include "Gs/Gs.h"
#include "Gs/GsDCPointArray.h"
#include "UInt8Array.h"
#include "Ps/PlotStyles.h"


class OdGsViewImpl;
class OdGsBaseVectorizeDevice;

/** Description:

    {group:OdGs_Classes} 
*/
class GS_TOOLKIT_EXPORT OdGsBaseModule : public OdGsModule
{
  OdRxObjectPtrArray  m_reactors;
protected:
  void fire_viewWasCreated(OdGsView* pView);
  void fire_gsToBeUnloaded(OdGsModule* pModule);

  virtual OdSmartPtr<OdGsBaseVectorizeDevice> createDeviceObject() = 0;
  virtual OdSmartPtr<OdGsViewImpl> createViewObject() = 0;
  virtual OdSmartPtr<OdGsBaseVectorizeDevice> createBitmapDeviceObject() = 0;
  virtual OdSmartPtr<OdGsViewImpl> createBitmapViewObject() = 0;

  OdGsBaseModule();
  void onFinalRelease();
public:
  ODRX_DECLARE_MEMBERS(OdGsBaseModule);

  void fire_viewToBeDestroyed(OdGsView* pView);

  OdGsDevicePtr createDevice();
  OdGsDevicePtr createBitmapDevice();

  OdSmartPtr<OdGsViewImpl> createView();
  OdSmartPtr<OdGsViewImpl> createBitmapView();

  void addReactor(OdGsReactor* pReactor);
  void removeReactor(OdGsReactor* pReactor);

  void initApp(); // override to handle
  void uninitApp(); // override to handle
};

typedef OdSmartPtr<OdGsBaseModule> OdGsBaseModulePtr;

typedef OdArray<ODCOLORREF, OdMemoryAllocator<ODCOLORREF> > ODGSPALETTE;

/** Description:

    {group:OdGs_Classes} 
*/
class GS_TOOLKIT_EXPORT OdGsBaseVectorizeDevice : public OdGsDevice
{
  friend class OdGsBaseVectorizeView;
  OdSmartPtr<OdGiContext> m_pUserContext;

  friend class OdGsBaseModule;
  OdGsBaseModulePtr       m_pModule;
protected:
  OdArray<OdGsViewPtr>    m_views;
  ODGSPALETTE             m_logPalette;
  ODCOLORREF              m_Background;
  OdGsDCRect              m_outputRect;

public:
  ODRX_DECLARE_MEMBERS(OdGsBaseVectorizeDevice);

  OdGiContext* userGiContext() const;
  void setUserGiContext(OdGiContext* pUserContext);

  OdRxDictionaryPtr properties();

  OdGsBaseVectorizeDevice();

  int height() const;
  int width() const;

  void invalidate();

  void invalidate(const OdGsDCRect& );

  bool isValid() const;

  void update(OdGsDCRect* pUpdatedRect);

  void onSize(const OdGsDCRect& outputRect);

  void onRealizeForegroundPalette();

  void onRealizeBackgroundPalette();

  void onDisplayChange(int nBitsPerPixel, int nXRes, int nYRes);

  OdGsViewPtr createView(const OdGsClientViewInfo* pInfo = 0, bool bEnableLayerVisibilityPerView = false);

  void addView(OdGsView* pView);

  bool eraseView(OdGsView* pView);

  int numViews() const;

  OdGsView* viewAt(int n);

  bool eraseView(int n);

  void eraseAllViews();

  bool setBackgroundColor(ODCOLORREF color);

  ODCOLORREF getBackgroundColor();

  void setLogicalPalette(const ODCOLORREF* palette, int nCount);

  OdGsModelPtr createModel();

  ODCOLORREF getColor(OdUInt16 colorIndex) const
  {
    return m_logPalette[colorIndex];
  }

  const ODCOLORREF* getPalette() const
  {
    return m_logPalette.asArrayPtr();
  }

  static OdUInt32 g_psLinetypeLengths[31];
  static OdUInt32 g_psLinetypeDefs[31][12];
};


class OdGsBaseModel;

/** Description:

    {group:OdGs_Classes} 
*/
class GS_TOOLKIT_EXPORT OdGsViewImpl : public OdGsView
{
  friend class OdGsBaseModule;
  OdGsBaseModulePtr m_pModule;

protected:
  OdGsClientViewInfo m_ViewInfo;

  void onFinalRelease() { m_pModule->fire_viewToBeDestroyed(this); }

public:
  ODRX_DECLARE_MEMBERS(OdGsViewImpl);

  virtual void init(const OdGsClientViewInfo* pViewInfo = NULL, bool bEnableLayerVisibilityPerView = false);
};

/** Description:
    
    {group:OdGs_Classes} 
*/
class GS_TOOLKIT_EXPORT OdGsBaseVectorizeView : public OdGsViewImpl
                                              , public OdGiViewport
                                              , public OdGiBaseVectorizer
{
  struct GsNodeStkItem
  {
    GsNodeStkItem(OdGsNode* pNode)
      : m_pNode(pNode) {}
    OdGsNode* m_pNode;
  };

  OdGsBaseModel*            m_pModel;
  OdGiStack<GsNodeStkItem>  m_nodesStack;

  mutable OdGeMatrix3d      m_objectToDeviceMatrix;
  mutable OdGeMatrix3d      m_worldToDeviceMatrix;

  mutable bool              m_bObjectToDeviceValid;
  mutable bool              m_bWorldToDeviceValid;

  bool                      m_bBorderVisible;
  ODCOLORREF                m_borderColor;
  int                       m_borderWeight;

  OdGiOrthoClipperPtr       m_pXlineNRayClipper;

  OdGiConveyorEntryPoint    m_xlineNRayEntryPoint;
  OdGiConveyorConnector     m_ltpEntryPoint;

  OdGiXformPtr              m_pXToLtp;
  OdGiLinetyperPtr          m_pLinetyper;
  mutable OdUInt32          m_ltpFlags;

  // OdGiConveyorContext
  const OdGiViewport* giViewport() const;
  const OdGsView* gsView() const;

  struct DrawableHolder
  {
    DrawableHolder()
      : m_drawableId(0)
      , m_pGsRoot(0)
    {}
    OdDbStub*             m_drawableId;
    OdGiDrawablePtr       m_pDrawable;
    OdSmartPtr<OdGsModel> m_pGsModel;
    OdGsNode*             m_pGsRoot;
  };
  typedef OdArray<DrawableHolder> DrawableHolderArray;

protected:
  ODRX_USING_HEAP_OPERATORS(OdGiBaseVectorizer);

  typedef OdArray<int, OdMemoryAllocator<int> > OdIntArray;
  typedef OdArray<OdDbStub*, OdMemoryAllocator<OdDbStub*> > OdDbStubPtrArray;

  OdGiDrawablePtr drawableAt(int i);

  OdGsBaseVectorizeDevice*  m_pDevice;

  DrawableHolderArray       m_drawables;

  OdDbStubPtrArray          m_frozenLayers;

  int                       m_nrcContours;
  OdIntArray                m_nrcCounts;
  OdGsDCPointArray          m_nrcPoints;

  OdUInt8Array              m_lweights;
  double                    m_linetypeScaleMultiplier;
  double                    m_lineweightToDcScale;

  bool                      m_bFillPlaneSet;
  OdGeVector3d              m_fillPlane;
  OdGePoint3d               m_position;
  OdGePoint3d               m_target;
  OdGeVector3d              m_upVector;
  OdGeVector3d              m_xVector;
  OdGeVector3d              m_eyeVector;
  double                    m_frontClipDist;
  double                    m_backClipDist;
  double                    m_lensLength;
  double                    m_eyeVecLength;
  bool                      m_bDoFrontClip;
  bool                      m_bDoBackClip;

  double                    m_fieldWidth;
  double                    m_fieldHeight;
  OdGsDCPoint               m_dcScreenMin;
  OdGsDCPoint               m_dcScreenMax;
  OdGePoint2d               m_dcLowerLeft;
  OdGePoint2d               m_dcUpperRight;
  Projection                m_projection;
  RenderMode                m_renderMode;

  OdGiContext::PStyleType   m_pstype;
  mutable int               m_nPenIndex;
  mutable OdDbStub*         m_psnId;
  mutable OdPsPlotStyleData m_plotStyle;
  mutable OdPsPlotStyleData m_effectivePlotStyle;

  enum InternalFlags // make sure they are not conflict with OdGiDrawable::SetAttributesFlags
  {
    kCurrExtentsSet     = 0x40000000,
    kAllInternalBits    = 0xFFFF0000
  };
  
  virtual bool viewDeptRange(double& zNear, double& zFar) const;

  // Data supposed to be set(or used) from derived classes only
  OdGiRegenType               m_regenerationType;

  void OnWorldToEyeChanged();
  void UpdateXlineNRayClipper();
  bool useFillPlane() const;

  inline OdGsBaseModel* currModel() { return m_pModel; }
public:
  const OdPsPlotStyleData& effectivePlotStyle();

  bool isEffectiveLinetypeContinuous() const;

  void setDevice(OdGsDevice* pDevice);

  static OdGsBaseVectorizeView* cast(OdGsView* pView);

  OdGiContext* userGiContext() const;
  void setUserGiContext(OdGiContext* pUserContext);

  double lineweightToDcScale() const;
  void setLineweightToDcScale(double dMultiplier);
  void setLineweightEnum(int nCount, const OdUInt8* lweights);
  int lineweightToPixels(OdDb::LineWeight lw) const;
  double lineweightToPixels(double lw) const;

  void setViewport(const OdGePoint2d& lowerLeft, const OdGePoint2d& upperRight);
  void setViewport(const OdGsDCRect& screen_rect);
  void getViewport(OdGePoint2d& lowerLeft, OdGePoint2d& upperRight) const;
  void getViewport(OdGsDCRect& screen_rect) const;

  void setMode(RenderMode mode);
  RenderMode mode() const;

  void setView(const OdGePoint3d & position,
    const OdGePoint3d& target,
    const OdGeVector3d& upVector,
    double fieldWidth,
    double fieldHeight,
    Projection projection = kParallel);

  OdGePoint3d position() const;
  OdGePoint3d target() const;
  OdGeVector3d upVector() const;

  double lensLength() const;
  double focalLength() const; // returns focal length in WCS (Eye CS)
  void setLensLength(double);

  double fieldWidth() const;
  double fieldHeight() const;

  void setEnableFrontClip(bool enable);
  bool isFrontClipped() const;
  void setFrontClip(double distance);
  double frontClip() const;

  void setEnableBackClip(bool enable);
  bool isBackClipped() const;
  void setBackClip(double distance);
  double backClip() const;

  OdGeMatrix3d viewingMatrix() const;
  OdGeMatrix3d projectionMatrix() const;
  OdGeMatrix3d screenMatrix() const;
  OdGeMatrix3d worldToDeviceMatrix() const;
  OdGeMatrix3d objectToDeviceMatrix() const;
  OdGeMatrix3d eyeToScreenMatrix() const;

  void setViewportBorderProperties(ODCOLORREF color, int weight);
  void getViewportBorderProperties(ODCOLORREF& color, int& weight) const;
  void setViewportBorderVisibility(bool bVisible);
  bool isViewportBorderVisible() const;
  bool add(OdGiDrawable* drawable, OdGsModel* model);
  bool erase(OdGiDrawable* drawable);
  void eraseAll();
  void invalidate();
  void invalidate(const OdGsDCRect &rect);
  bool isValid() const;

  void beginViewVectorization();
  void update();
  void endViewVectorization();
  virtual void drawViewportFrame();

  void beginInteractivity(double fFrameRateInHz);
  void endInteractivity();
  void flush();
  void hide();
  void show();
  bool isVisible();
  void freezeLayer(OdDbStub* layerID);
  void thawLayer(OdDbStub* layerID);
  void clearFrozenLayers();
  void invalidateCachedViewportGeometry();
  //bool highlight(const OdGsPath*);
  //bool unhighlight(const OdGsPath*);
  void dolly(const OdGeVector3d& vector);
  void dolly(double x, double y, double z);
  void roll(double angle);
  void orbit(double x, double y);
  void zoom(double factor);
  void pan(double x, double y);
  OdGsViewPtr cloneView(bool bCloneViewParameters = true, bool bCloneGeometry = false);
  bool exceededBounds();
  void enableStereo(bool bEnable);
  bool isStereoEnabled() const;
  void setStereoParameters(double magnitude, double parallax);
  void getStereoParameters(double& magnitude, double& parallax) const;
  void setViewportClipRegion(int contours, const int* counts, const OdGsDCPoint* points);
  void removeViewportClipRegion();
  void viewParameters(OdGsView* pRes) const;
protected:

  inline double windowAspect() const
  {
    double height = double(m_dcScreenMax.y - m_dcScreenMin.y) * (m_dcUpperRight.y - m_dcLowerLeft.y);
    if(OdZero(height, .5))
    {
      height = 0.5;
    }
    double width = double(m_dcScreenMax.x - m_dcScreenMin.x) * (m_dcUpperRight.x - m_dcLowerLeft.x);
    if(OdZero(width, .5))
    {
      width = 0.5;
    }
    return fabs(width / height);
  }

  OdGeMatrix3d perspectiveMatrix() const;

  void affectTraits(const OdGiSubEntityTraitsData* pFrom, OdGiSubEntityTraitsData& to) const;

public:
  OdGsBaseVectorizeView();
  ~OdGsBaseVectorizeView() { eraseAll(); }

  // OdGsView methods - overrides

  void resetExtents();

  OdGiRegenType regenType() const;
  double deviation(const OdGiDeviationType, const OdGePoint3d&) const;

  OdUInt32      sequenceNumber() const;
  bool          isValidId(const OdUInt32 acgiId) const;
  OdDbStub*     viewportObjectId() const;

  void setFillPlane(const OdGeVector3d* pNormal = NULL);

  OdGiViewport& viewport() const;

  // OdGiViewport
  OdGeMatrix3d getModelToEyeTransform() const;
  OdGeMatrix3d getEyeToModelTransform() const;
  OdGeMatrix3d getWorldToEyeTransform() const;
  OdGeMatrix3d getEyeToWorldTransform() const;

  bool isPerspective() const;
  bool doPerspective(OdGePoint3d&) const;
  bool doInversePerspective(OdGePoint3d&) const;
  
  void getNumPixelsInUnitSquare(const OdGePoint3d& givenWorldpt, OdGePoint2d& pixelArea) const;
  
  OdGePoint3d getCameraLocation() const;
  OdGePoint3d getCameraTarget() const;
  OdGeVector3d getCameraUpVector() const;
  
  OdUInt32 viewportId() const;
  OdInt16  acadWindowId() const;
  void getViewportDcCorners(OdGePoint2d& lower_left, OdGePoint2d& upper_right) const;
  
  bool getFrontAndBackClipValues(bool& clip_front, bool& clip_back, double& front, double& back) const;
  
  double linetypeGenerationCriteria() const;
  double linetypeScaleMultiplier() const;
  void setLinetypeScaleMultiplier(double dLinetypeScaleMultiplier);
  bool layerVisible(OdDbStub* idLayer) const;

  OdGeVector3d viewDir() const;

  void initLights(OdRxIterator* );

  void select(const OdGsDCPoint* pts, int nPoints, OdGsSelectionReactor* pReactor);

  void xline(const OdGePoint3d&, const OdGePoint3d&);
  void ray(const OdGePoint3d&, const OdGePoint3d&);

  void screenRect(OdGsDCPoint &min, OdGsDCPoint &max) const;

  void setExtents(const OdGePoint3d *pNewExtents);

  const OdGiDeviation& modelDeviation();
  const OdGiDeviation& worldDeviation();
  const OdGiDeviation& eyeDeviation();

  OdGsBaseVectorizeDevice* device() { return m_pDevice; }


  void doDraw(OdUInt32 nDrawableFlags, const OdGiDrawable* pDrawable);

  void pushModelTransform(const OdGeVector3d& vNormal);
  void pushModelTransform(const OdGeMatrix3d & xMat);
  void popModelTransform();

  void worldLine(const OdGePoint3d pnts[2]);

  // Called by framework for setting up traits
  // Must be overloaded in device in order to set up traits
  // New traits is recomputed before calling this function
  virtual void onTraitsModified();
};


inline OdGsBaseVectorizeView* OdGsBaseVectorizeView::cast(OdGsView* pView)
{
  return static_cast<OdGsBaseVectorizeView*>(OdSmartPtr<OdGsViewImpl>(pView).get());
}


#include "DD_PackPop.h"

#endif // ODGSBASEVECTORIZER_INC

