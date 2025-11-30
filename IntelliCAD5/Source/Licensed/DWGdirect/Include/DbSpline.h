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



#ifndef ODDB_DBSPLINE_H
#define ODDB_DBSPLINE_H

#include "DD_PackPush.h"

#include "DbCurve.h"

class OdGeKnotVector;

/** Description:
    Represents a spline entity in an OdDbDatabase.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbSpline : public OdDbCurve
{
public:
  ODDB_DECLARE_MEMBERS(OdDbSpline);

  /** Description:
      Constructor (no arguments).
  */
  OdDbSpline();
  
  /** Description:
      Returns true if this spline is rational, false otherwise (DXF 70, bit 0x04).
  */
  bool isRational() const;
  
  /** Description:
      Returns the degree of this spline (DXF 71).
  */
  int degree() const;
  
  /** Description:
      Increased the degree of this spline to the specified value.
      void elevateDegree(int newDegree);
  */
  
  /** Description:
      Returns the number of control points in this spline (DXF 73).
  */
  int numControlPoints() const;
  
  /** Description:
      Returns a control point specified by index (DXF 10).

      Arguments:
      index (I) Index of desired control point.
      point (O) Receives the control point value.
  */
  void getControlPointAt(int index, OdGePoint3d& point) const;
  
  /** Description:
      Sets a control point specified by index (DXF 10).

      Arguments:
      index (I) Index of desired control point.
      point (I) The control point value.
  */
  void setControlPointAt(int index, const OdGePoint3d& point);
  
  /** Description:
      Returns the number of fit points in this spline (DXF 74).
  */
  int numFitPoints() const;
  
  /** Description:
      Returns a fit point specified by index (DXF 11).

      Arguments:
      index (I) Index of desired fit point.
      point (O) Receives the fit point value.
  */
  OdResult getFitPointAt(int index, OdGePoint3d& point) const;
  
  /** Description:
      Sets a fit point specified by index (DXF 11).

      Arguments:
      index (I) Index of desired fit point.
      point (I) Receives the fit point value.
  */
  void setFitPointAt(int index, const OdGePoint3d& point);
  
  /** Description:
      Inserts a fit point at the specified index.
  */
  void insertFitPointAt(int index, const OdGePoint3d& point);
  
  /** Description:
      Removes a specified fit point from this spline.

      Arguments:
      index (I) Index of fit point to remove.
  */
  void removeFitPointAt(int index);
  
  /** Description:
      Returns the fit tolerance of this spline (DXF 44).
  */
  double fitTolerance() const;
  
  /** Description:
      Sets the fit tolerance of this spline (DXF 44).
  */
  void setFitTol(double tol);
  
  /** Description:
      Returns the start and end fit tangents for this spline (DXF 12, 13).
  */
  OdResult getFitTangents(OdGeVector3d& startTangent, OdGeVector3d& endTangent) const;
  
  /** Description:
      Sets the start and end fit tangents for this spline (DXF 12, 13).
  */
  void setFitTangents(const OdGeVector3d& startTangent, 
    const OdGeVector3d& endTangent);
  
  /** Description:
      Returns true if this spline contains fit data, false otherwise.
  */
  bool hasFitData() const;
  
  /** Description:
      Returns the fit data contained in this entity.

      Arguments:
      fitPoints (O) Receives the fit points.
      degree (O) Receives the degree of the spline.
      fitTolerance (O) Receives the fit tolerance.
      tangentsExist (O) Indicates whether start and end tangents are used.
      startTangent (O) Receives the start tangent for this spline.
      endTangent (O) Receives the end tangent for this spline.
  */
  OdResult getFitData(OdGePoint3dArray& fitPoints, 
    int& degree, 
    double& fitTolerance, 
    bool& tangentsExist, 
    OdGeVector3d& startTangent, 
    OdGeVector3d& endTangent ) const;
  
  /** Description:
      Sets the fit data for this entity.

      Arguments:
      fitPoints (I) The fit points.
      degree (I) The degree of the spline.
      fitTolerance (I) The fit tolerance.
      startTangent (I) The start tangent for this spline.
      endTangent (I) The end tangent for this spline.
  */
  void setFitData(const OdGePoint3dArray& fitPoints, int degree, 
    double fitTolerance, const OdGeVector3d& startTangent, 
    const OdGeVector3d& endTangent );
  
  /** Description:
      Clears the fit points for this spline.
  */
  void purgeFitData();
  
  /** Description:
      TBC.
      void updateFitData();
  */
  
  /** Description:
      Returns the NURBS data for this entity.
  */
  void getNurbsData(int& degree, bool& rational, bool& closed, bool& periodic,
    OdGePoint3dArray& controlPoints, OdGeDoubleArray& knots, 
    OdGeDoubleArray& weights, double& controlPtTol,
    double& knotTol) const;

  void getNurbsData(int& degree, bool& rational, bool& closed, bool& periodic,
    OdGePoint3dArray& controlPoints, OdGeKnotVector& knots, 
    OdGeDoubleArray& weights, double& controlPtTol) const;
  
  /** Description:
      Sets the NURBS data for this entity.
  */
  void setNurbsData(int degree, bool rational, bool closed, bool periodic,
    const OdGePoint3dArray& controlPoints, 
    const OdGeDoubleArray& knots, const OdGeDoubleArray& weights,
    double controlPtTol, double knotTol );
  
  void setNurbsData(int degree, bool rational, bool closed, bool periodic,
    const OdGePoint3dArray& controlPoints, 
    const OdGeKnotVector& knots, const OdGeDoubleArray& weights,
    double controlPtTol);
  
  /** Description:
      Returns a weight specified by index (DXF 41).
  */
  double weightAt(int index) const;
  
  /** Description:
      Sets a weight specified by index (DXF 41).
  */
  void setWeightAt(int index, double weight);
  
  /** Description:
      Inserts a knot value into this spline.
  */
  // --not implemented yet-- void insertKnot(double param);
  
  /** Description:
      TBC.
  */
  void reverseCurve();
  
  
  virtual void getClassID(void** pClsid) const;
  
  /*
     void getOffsetCurvesGivenPlaneNormal(const OdGeVector3d& normal,  //Replace OdRxObjectPtrArray
                                          double offsetDist, 
                                          OdRxObjectPtrArray& offsetCurves ) const;
  */
    
  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);
  
  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;
  
  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);
  
  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;
  
  virtual bool worldDraw(OdGiWorldDraw* pWd) const;

  virtual OdResult transformBy(const OdGeMatrix3d& xform);

  virtual bool isPlanar() const;

  virtual OdResult getPlane(OdGePlane&, OdDb::Planarity&) const;


  /** Description:
      OdDbCurve methods
  */

  virtual OdResult getPointAtParam(double, OdGePoint3d&) const;

  virtual OdResult getParamAtPoint(const OdGePoint3d&, double&) const;

  virtual OdResult getStartParam(double& param) const;

  virtual OdResult getEndParam (double& param) const;

  virtual OdResult getStartPoint(OdGePoint3d& pt) const;

  virtual OdResult getEndPoint(OdGePoint3d& pt) const;

  virtual OdResult getSplitCurves(const OdGeDoubleArray& params, 
    OdRxObjectPtrArray& entitySet) const;

  OdDbObjectPtr decomposeForSave(OdDb::DwgVersion ver,
                                  OdDbObjectId& replaceId,
                                  bool& exchangeXData);
};

typedef OdSmartPtr<OdDbSpline> OdDbSplinePtr;

#include "DD_PackPop.h"

#endif


