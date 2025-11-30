#ifndef ODOPENGLVECTORIZER_INCLUDED
#define ODOPENGLVECTORIZER_INCLUDED

#include "GsBaseVectorizer.h"
#include "RxObjectImpl.h"
#include "Gi/GiGeometrySimplifier.h"
#include "DD_GL.h"

/** Description:

    {group:OdGs_Classes} 
*/
class OdGsOpenGLVectorizeDevice : public OdGsBaseVectorizeDevice
{
  friend class OdGsOpenGLVectorizeView;
public:
  OdGsOpenGLVectorizeDevice();
  ~OdGsOpenGLVectorizeDevice();

  void update(OdGsDCRect* pUpdatedRect);

  virtual void clear();
};

/** Description:

    {group:OdGs_Classes} 
*/
class OdGsOpenGLVectorizeView : public OdGsBaseVectorizeView
                              , public OdGiGeometrySimplifier
{
  friend class OdGsOpenGLVectorizeDevice;
  bool                      m_bFill;
  bool                      m_bDrawFilledPrimitive;
  mutable bool              m_bModelExtentsValid;
  mutable OdGeExtents3d     m_modelExtents;
  float                     m_dLwScaler;
  OdGeVector3d              m_n1, m_n2;
  OdGeMatrix3d              m_correction;
  bool                      m_bSeparateTriangleTraits;
  enum {                    kClippedOut = 1,
                            kOffScreen  = 2
  };
  mutable int               m_state;

  bool setCurrentFaceTraits();
  const OdGeMatrix3d& projectionCorrection() { return m_correction; }
  bool loadViewport();
  OdGeMatrix3d dividedDcToNormalizedDc() const;
  void getGLScreenRect(double& left, double& bottom, double& width, double& height);
  void tesselateToGL(OdInt32 nPoints, const OdGePoint3d* pPoints);

  GLUtesselatorObj*         m_pGLUtess;

  void calcFaceNormal( const OdInt32* indices );

  static GLushort g_psLinetypeDefs[31][2];
protected:
  OdGsOpenGLVectorizeDevice* device() 
  {
    return (OdGsOpenGLVectorizeDevice*)m_pDevice; 
  }

  void draw_color(const OdCmEntityColor& color);
  void draw_color(ODCOLORREF color);
  void draw_color(OdUInt16 colorIndex);
  void draw_lineweight(GLfloat pixels);
  void draw_linetype(OdPs::LineType lt, bool isAdaptive, double linePatSz);
  void draw_fillStyle(OdPs::FillStyle fillStyle);

  void screenRect(OdGsDCPoint &min, OdGsDCPoint &max) const;
  OdGeExtents3d modelExtents() const;
  virtual bool viewDeptRange(double& zNear, double& zFar) const;

  virtual void init(const OdGsClientViewInfo* pViewInfo, bool bEnableLayerVisibilityPerView);
public:
  OdGsOpenGLVectorizeView();
  ~OdGsOpenGLVectorizeView();
  
  static OdGsViewPtr createObject(const OdGsClientViewInfo* pViewInfo, bool bEnableLayerVisibilityPerView);

  void beginViewVectorization();
  void update();
  void endViewVectorization();

  void onTraitsModified();

  void initLights(OdRxIterator* pLightsIter);
  void updateLights();

  OdRxObjectPtrArray m_lights;


  // OdGiConveyorGeometry
  // handling primitives in eye CS

  // required by simplifier
  void polylineOut(OdInt32 nPoints, const OdGePoint3d* pPoints);

  void polygonOut(OdInt32 nbPoints, const OdGePoint3d* pVertexList, const OdGeVector3d* pNormal);

  void generateMeshFaces(OdInt32 rows, OdInt32 columns, const OdGiFaceData* pFaceData);

  void generateShellFaces(OdInt32 faceListSize, const OdInt32* pFaceList,
                          const OdGiEdgeData* pEdgeData, const OdGiFaceData* pFaceData);

  void triangleOut( const OdInt32* p3Vertices, const OdGeVector3d* pNormal );

  void shellFaceOut(OdInt32 faceListSize, const OdInt32* pFaceList, const OdGeVector3d* pNormal );

  void rasterImageProc(const OdGePoint3d& origin,
    const OdGeVector3d& u,
    const OdGeVector3d& v,
    const OdGiRasterImage* pImg, // image object
    const OdGePoint2d* uvBoundary, // may not be null
    OdUInt32 numBoundPts,
    bool transparency = false,
    double brightness = 50.0,
    double contrast = 50.0,
    double fade = 0.0);

  /* These are to be simplified by OdGiGeometrySimplifier

  void polygonOut(OdInt32 nbPoints, const OdGePoint3d* pVertexList, const OdGeVector3d* pNormal = 0);

  void polylineProc(OdInt32 nbPoints, const OdGePoint3d* pVertexList,
    const OdGeVector3d* pNormal = 0,
    const OdGeVector3d* pExtrusion = 0,
    OdInt32 lBaseSubEntMarker = -1);

  void meshProc(OdInt32 rows,
    OdInt32 columns,
    const OdGePoint3d* pVertexList,
    const OdGiEdgeData* pEdgeData = 0,
    const OdGiFaceData* pFaceData = 0,
    const OdGiVertexData* pVertexData = 0);

  void shellProc(OdInt32 nbVertex,
    const OdGePoint3d* pVertexList,
    OdInt32 faceListSize,
    const OdInt32* pFaceList,
    const OdGiEdgeData* pEdgeData = 0,
    const OdGiFaceData* pFaceData = 0,
    const OdGiVertexData* pVertexData = 0);

  void polygonProc(OdInt32 nbPoints, const OdGePoint3d* pVertexList,
    const OdGeVector3d* pNormal = 0,
    const OdGeVector3d* pExtrusion = 0);

  void circleProc(const OdGePoint3d& center, double radius, const OdGeVector3d& normal, const OdGeVector3d* pExtrusion = 0);

  void circleProc(const OdGePoint3d&, const OdGePoint3d&, const OdGePoint3d&, const OdGeVector3d* pExtrusion = 0);

  void circularArcProc(const OdGePoint3d& center,
    double radius,
    const OdGeVector3d& normal,
    const OdGeVector3d& startVector,
    double sweepAngle,
    OdGiArcType arcType = kOdGiArcSimple, const OdGeVector3d* pExtrusion = 0);

  void circularArcProc(
    const OdGePoint3d& start,
    const OdGePoint3d& point,
    const OdGePoint3d& end,
    OdGiArcType arcType = kOdGiArcSimple, const OdGeVector3d* pExtrusion = 0);

  void nurbsProc(const OdGeNurbCurve3d& nurbs);

  void ellipArcProc(const OdGeEllipArc3d& arc,
    OdGiArcType arcType = kOdGiArcSimple, const OdGeVector3d* pExtrusion = 0);

  void ellipArcProc(const OdGePoint3d& center,
    const OdGeVector3d& majorRadius,
    const OdGeVector3d& minorRadius,
    const OdGePoint3d& startPoint,
    const OdGePoint3d& endPoint,
    OdGiArcType arcType = kOdGiArcSimple, const OdGeVector3d* pExtrusion = 0);

  void textProc(const OdGePoint3d& position,
    const OdGeVector3d& u, const OdGeVector3d& v,
    const OdChar* msg, OdInt32 length, bool raw, const OdGiTextStyle* pTextStyle,
    const OdGeVector3d* pExtrusion = 0);

  void shapeProc(const OdGePoint3d& position,
    const OdGeVector3d& u, const OdGeVector3d& v,
    int shapeNo, const OdGiTextStyle* pStyle,
    const OdGeVector3d* pExtrusion = 0);

  void shellProc(OdInt32 nbVertex,
    const OdGePoint3d* pVertexList,
    OdInt32 faceListSize,
    const OdInt32* pFaceList,
    const OdGiEdgeData* pEdgeData = 0,
    const OdGiFaceData* pFaceData = 0,
    const OdGiVertexData* pVertexData = 0);

  void xlineProc(const OdGePoint3d&, const OdGePoint3d&);

  void rayProc(const OdGePoint3d&, const OdGePoint3d&);

        void metafileProc(
    const OdGePoint3d& origin,
                const OdGeVector3d& u,
                const OdGeVector3d& v,
    const OdGiMetafile* pMetafile,
    bool bDcAligned = true,           // reserved
    bool bAllowClipping = false); // reserved
  */
};

#endif //ODOPENGLVECTORIZER_INCLUDED

