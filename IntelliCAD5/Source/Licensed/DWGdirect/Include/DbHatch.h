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



#ifndef OD_DBHATCH_H
#define OD_DBHATCH_H

#include "DD_PackPush.h"

#include "DbEntity.h"
#include "Ge/GePoint2dArray.h"
#include "Ge/GeIntArray.h"
#include "Ge/GeVoidPointerArray.h"
#include "CmColorArray.h"

#define HATCH_PATTERN_NAME_LENGTH 32

typedef OdArray<OdGeCurve2d*> EdgeArray;

/** Description:
    Represents a hatch in an OdDbDatabase.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbHatch : public OdDbEntity
{
public:
  ODDB_DECLARE_MEMBERS(OdDbHatch);

  /** Description:
      Constructor (no arguments).
  */
  OdDbHatch();

  enum HatchLoopType
  {
    kDefault = 0,
    kExternal = 1,
    kPolyline = 2,
    kDerived = 4,
    kTextbox = 8,
    kOutermost = 0x10,
    kNotClosed = 0x20,
    kSelfIntersecting = 0x40,
    kTextIsland = 0x80,
    kDuplicate = 0x100
  };

  enum HatchEdgeType
  {
    kLine = 1,
    kCirArc = 2,
    kEllArc = 3,
    kSpline = 4
  };

  enum HatchPatternType
  {
    kUserDefined = 0,
    kPreDefined = 1,
    kCustomDefined = 2
  };

  enum HatchStyle
  {
    kNormal = 0,
    kOuter = 1,
    kIgnore = 2
  };

  enum HatchObjectType 
  {
    kHatchObject      = 0,
    kGradientObject   = 1
  };

  enum GradientPatternType 
  {
    kPreDefinedGradient     = 0,
    kUserDefinedGradient    = 1
  };

  /** Description:
      Returns the elevation of this entity in OCS (DXF 30).
  */
  double elevation() const;

  /** Description:
      Sets the elevation of this entity in OCS (DXF 30).
  */
  void setElevation(double elevation);

  /** Description:
      Returns the normal of this entity (DXF 210).
  */
  OdGeVector3d normal() const;

  /** Description:
      Sets the normal of this entity (DXF 210).
  */
  void setNormal(const OdGeVector3d& normal);

  virtual bool isPlanar() const { return true; }

  virtual OdResult getPlane(OdGePlane& plane, OdDb::Planarity& planarity) const;

  /** Description:
      Returns the number of loops in this hatch (DXF 91).
  */
  int numLoops() const;

  /** Description:
      Returns the type of loop at the specified index (DXF 92).  The loop type consists
      of 0 or more bits from the HatchLoopType enumeration.
  */
  OdInt32 loopTypeAt(int loopIndex) const;

  /** Description:
      Returns a specified loop from this hatch, in the form of a set of edges.
      Should be called if the loop is composed of edges (not a polyline loop).

      Arguments:
      loopIndex (I) Index of loop.
      edgePtrs (O) Receives a set of OdGeCurve pointers that make up this loop.
  */
  void getLoopAt(int loopIndex, 
                 EdgeArray& edgePtrs) const;

  /** Description:
      Returns a specified loop from this hatch, in the form of a set of vertices & bulges.
      Should be called if loop is polyline.

      Arguments:
      loopIndex (I) Index of loop.
      vertices (O) Receives the vertices that make up this loop.
      bulges (O) Receives a set of bulges, that correspond to the vertices
      in the vertices array.
  */
  void getLoopAt(int loopIndex, 
                 OdGePoint2dArray& vertices, 
                 OdGeDoubleArray& bulges) const;

  /** Description:
      Appends a loop onto this hatch entity, in the form of a set of edges.

      Arguments:
      loopType (I) Type of loop being appended.
      edgePtrs (I) Array of edges that make up this loop.
  */
  void appendLoop(OdInt32 loopType,
                  const EdgeArray& edgePtrs);

  /** Description:
      Appends a loop onto this hatch entity, in the form of a set of vertices.

      Arguments:
      loopType (I) Type of loop being appended.
      vertices (I) Array of vertices that make up this loop.
      bulges (I) Array of bulge values, that correspond to the entries
      in the vertices array.
  */
  void appendLoop(OdInt32 loopType,
                  const OdGePoint2dArray& vertices,
                  const OdGeDoubleArray& bulges);

  /** Description:
      Inserts a loop into this hatch entity at a specified index, in the form of a set of edges.

      Arguments:
      loopIndex (I) Index at which to insert this loop.
      loopType (I) Type of loop being appended.
      edgePtrs (I) Array of edges that make up this loop.
  */
  void insertLoopAt(int loopIndex, 
                    OdInt32 loopType,
                    const EdgeArray& edgePtrs);

  /** Description:
      Inserts a loop into this hatch entity at a specified index, in the form of a set of vertices.

      Arguments:
      loopIndex (I) Index at which to insert this loop.
      loopType (I) Type of loop being appended.
      vertices (I) Array of vertices that make up this loop.
      bulges (I) Array of bulge values, that correspond to the entries
      in the vertices array.
  */
  void insertLoopAt(int loopIndex, 
                    OdInt32 loopType,
                    const OdGePoint2dArray& vertices,
                    const OdGeDoubleArray& bulges);

  /** Description:
      Removes the loop at the specified index from this hatch.
  */
  void removeLoopAt(int loopIndex);

  /** Description:
      Returns true if this hatch is associative, false otherwise (DXF 71).
  */
  bool associative() const;

  /** Description:
      Sets the associative flag for this hatch (DXF 71).
  */
  void setAssociative(bool isAssociative);

  /** Description:
      Appends a loop onto this hatch, in the form of a set of OdDbEntities.
      A loop must be simple, closed, and continuous, intersecting itself only at its endpoints.
      Furthermore, its start point and end point must coincide. When defining the hatch boundary,
      the application must ensure that the loops and edges are well defined and structured. If
      the boundary contains two or more loops, they must be organized into a nested structure
      in which the external loop is constructed first, then followed by all its internal loops
      in nested order. If there is more than one external loop, repeat the process.
      DWGdirect provides limited validation of the hatch boundary in order to maintain API efficiency
      and performance.

      Arguments:
      loopType (I) Type of loop to be appended.
      dbObjIds (I) Array of OdDbEntity Object ID's that make up the loop.
  */
  void appendLoop(OdInt32 loopType, const OdDbObjectIdArray& dbObjIds);

  /** Description:
      Insert a loop into this hatch at a specified index, in the form of a set of OdDbEntities.
      A loop must be simple, closed, and continuous, intersecting itself only at its endpoints.
      Furthermore, its start point and end point must coincide. When defining the hatch boundary,
      the application must ensure that the loops and edges are well defined and structured. If
      the boundary contains two or more loops, they must be organized into a nested structure
      in which the external loop is constructed first, then followed by all its internal loops
      in nested order. If there is more than one external loop, repeat the process.
      DWGdirect provides limited validation of the hatch boundary in order to maintain API efficiency
      and performance.

      Arguments:
      loopIndex (I) Index at which to insert this loop.
      loopType (I) Type of loop to be appended.
      dbObjIds (I) Array of OdDbEntity Object ID's that make up the loop.
  */
  void insertLoopAt(int loopIndex, OdInt32 loopType,
                    const OdDbObjectIdArray& dbObjIds);

  /** Description:
      Returns the Object ID's of the boundary objects associated with a specified loop in this hatch entity.

      Arguments:
      loopIndex (I) Index of loop for which to retrieve boundary object ID's.
      dbObjIds (O) Receives the set of boundary object ID's that make up the specified loop, or
      is set to empty if this hatch is not associative.
  */
  void getAssocObjIdsAt(int loopIndex, OdDbObjectIdArray& dbObjIds) const;

  /** Description:
      Returns the Object ID's of all boundary objects associated with this hatch entity.

      Arguments:
      dbObjIds (O) Receives the set of all boundary object ID's that are associated with this hatch,
      or is set to empty if this hatch is not associative.
  */
  void getAssocObjIds(OdDbObjectIdArray& dbObjIds) const;

  /** Description:
      Sets the Object ID's of the boundary objects associated with a specified loop in this hatch entity.

      Arguments:
      loopIndex (I) Index of loop for which to set boundary object ID's.
      dbObjIds (I) The set of boundary object ID's that make up the specified loop.
      Adds hatch's object ID to boundary objects reactors.
  */
  void setAssocObjIdsAt(int loopIndex, const OdDbObjectIdArray& dbObjIds);

  /** Description:
      Removes all Object ID's that are associated with the hatch.
  */
  void removeAssocObjIds();

  /** Description:
      Returns the hatch pattern type for this hatch (DXF 76).
  */
  OdDbHatch::HatchPatternType patternType() const;

  /** Description:
      Returns true if hatch is solid (DXF 70).
  */
  bool isSolidFill() const;

  
  /** Description:
      Returns the pattern name for this hatch (DXF 2).
  */
  OdString patternName() const;

  /** Description:
      Sets the pattern type and pattern name for this hatch (DXF 76 and DXF 2, repsectively).
      patName may be NULL for kUserDefinedType. In such case hatch appearance is defined
      by pattern angle, spacing and "double" flag
  */
  void setPattern(OdDbHatch::HatchPatternType patType,
                  const char* patName);

  /** Description:
      Returns the pattern angle for this hatch in radians (DXF 52).
  */
  double patternAngle() const;

  /** Description:
      Sets the pattern angle for this hatch in radians (DXF 52).
  */
  void setPatternAngle(double angle);

  /** Description:
      Returns the pattern spacing (distance between 2 parallel hatch lines) for this hatch (DXF 41).
  */
  double patternSpace() const;

  /** Description:
      Sets the pattern spacing (distance between 2 parallel hatch lines) for this hatch (DXF 41).
  */
  void setPatternSpace(double space);

  /** Description:
      Returns the pattern scale for this hatch (DXF 41).
  */
  double patternScale() const;

  /** Description:
      Returns the pattern scale for this hatch (DXF 41).
  */
  void setPatternScale(double scale);

  /** Description:
      Returns the pattern double flag for this hatch (DXF 77).
  */
  bool patternDouble() const;

  /** Description:
      Sets the pattern double flag for this hatch (DXF 77).
  */
  void setPatternDouble(bool isDouble);

  /** Description:
      Returns the number of pattern definition lines for this hatch (DXF 78).
  */
  int numPatternDefinitions() const;

  /** Description:
      Gets a specified pattern definition line from this hatch.

      Arguments:
      index (I) Index of the definition line to get.
      angle (O) Pattern line angle (DXF 53).
      baseX (O) Pattern line base point X (DXF 43).
      baseY (O) Pattern line base point Y (DXF 44).
      offsetX (O) Pattern line offset X (DXF 45).
      offsetY (O) Pattern line offset Y (DXF 46).
      dashes (O) Dash lengths for this pattern definition line (DXF 79, 49).
  */
  void getPatternDefinitionAt(int index, 
                              double& angle, 
                              double& baseX,
                              double& baseY, 
                              double& offsetX, 
                              double& offsetY,
                              OdGeDoubleArray& dashes) const;

  /** Description:
      Returns the style of this hatch (DXF 75).
  */
  OdDbHatch::HatchStyle hatchStyle() const;

  /** Description:
      Sets the style of this hatch (DXF 75).
  */
  void setHatchStyle(OdDbHatch::HatchStyle hstyle);

  /** Description:
      Returns the number of seed points for this hatch (DXF 98).
  */
  int numSeedPoints() const;

  /** Description:
      Returns the specified seed point from this hatch (DXF 10).
  */
  const OdGePoint2d& getSeedPointAt(unsigned index) const;

  /** Description:
      Sets a seed point at a specified index for this hatch (DXF 10).
  */
  void setSeedPointAt(unsigned index, OdGePoint2d& point);

  /** Description:
      Appends a seed point to this hatch.
  */
  void appendSeedPoint(const OdGePoint2d& point);

  /** Description:
      Returns pixel size used to determine the density to perform various
      intersection and ray casting operations in hatch pattern computation
      for associative hatches and hatches created with the Flood method of hatching.
  */
  double pixelSize() const;

  /** Description:
      Sets pixel size
  */
  void setPixelSize(double pixelSize);

  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;

  virtual void getClassID(void** pClsid) const;

  virtual bool worldDraw(OdGiWorldDraw* pWd) const;

  virtual OdResult transformBy(const OdGeMatrix3d& xform);

  /** Description:
      Creates a copy (clone) of this entity, and applies the supplied transformation
      to the newly created copy.

      Arguments:
      xform (I) Transformation matrix to apply to the newly created copy.
      pCopy (O) Receives a pointer to the newly created copy.
  */
  virtual OdResult getTransformedCopy(const OdGeMatrix3d& xform, OdDbEntityPtr& pCopy) const;

  OdDbObjectPtr decomposeForSave(OdDb::DwgVersion /*ver*/,
                                  OdDbObjectId& replaceId,
                                  bool& /*exchangeXData*/);

  OdResult evaluateHatch(bool bUnderestimateNumLines = false);

  /** Description:
      This function returns the total number of hatch lines contained in the hatch entity. 
      It returns 0 if the hatch pattern is SOLID. 
  */
  int numHatchLines() const;
  
  /** Description:
      This function gets the hatch line data from the hatch entity. 
      index specifies the number at which the hatch line data will be returned. 
      startPt and endPt define the start and end points of the hatch line.
      

      Arguments:
      index Input number at which the hatch line data will be returned 
      startPt Output start point of the hatch line 
      endPt Output end point of the hatch line 
  */
  void getHatchLineDataAt(int index, OdGePoint2d& startPt, OdGePoint2d& endPt) const;
  
  /** Description:
      This function gets all the hatch line data from the hatch entity. 
      startPts is an AcGePoint2d array containing the start points of all 
      hatch lines. endPts is an AcGePoint2d array containing the end points 
      of all hatch lines. startPts and endPts must be correspondent and have the 
      same length. For example, startPts[0] and endPts[0] define the start and 
      end points of the first hatch line.


      Arguments:
      startPts Output start point of the hatch line 
      endPts Output end point of the hatch line 
  */
  void getHatchLinesData(OdGePoint2dArray& startPts, OdGePoint2dArray& endPts) const;
  
  virtual OdResult explode(OdRxObjectPtrArray& entitySet) const; /* Replace OdRxObjectPtrArray */

  /* virtual void subSwapIdWith(OdDbObjectId otherId, bool swapXdata = false, bool swapExtDict = false);
  virtual void swapReferences(const OdDbIdMapping& idMap);
  virtual void modifiedGraphics(const OdDbEntity* pObj);*/


  /** Description:
      determines Hatch or Gradient object type
  */
  OdDbHatch::HatchObjectType hatchObjectType() const;

  /** Description:
      Sets up object type
  */
  void setHatchObjectType(OdDbHatch::HatchObjectType type);
  
  /** Description:
      determines Hatch or Gradient object type
  */
  virtual bool isGradient() const;
  virtual bool isHatch() const;
  
  /** Description:
      Gradient Style Methods
  */
  OdDbHatch::GradientPatternType gradientType() const;
  
  /** Description:
      Returns the name of gradient
  */
  OdString gradientName() const;
  
  /** Description:
      Sets the gradient type and name
  */
  void setGradient(OdDbHatch::GradientPatternType gradType, const char* gradName);
  
  /** Description:
      Gets the angle of gradient
  */
  double gradientAngle() const;

  /** Description:
      Sets the angle of gradient
  */
  void setGradientAngle(double angle);
  
  /** Description:
      This method returns the colors and interpolation values describing how 
      the gradient fill definition will be applied. The gradient will 
      transition from colors[0] to colors[colors.szie()-1]. In the current 
      implementation, count is always two. 
  */
  void getGradientColors(OdCmColorArray& colors, OdGeDoubleArray& values) const;
  
  /** Description:
      This method provides the mechanism for defining the colors to be applied to 
      the gradient definition. In the simplest case, the color definition consists 
      of the starting and ending color. The start color maps to colors[0] and the 
      stop color maps to colors[count-1]. The current implementation supports only 
      a count value of 2. The AcDbHatch object makes a copy of the arrays provided 
      as arguments. It is the caller's responsibility to free the colors and values 
      arrays. 
  */
  void setGradientColors(OdUInt32 count, const OdCmColor* colors, const double* values);
  
  /** Description:
      Returns if gradient is one-color.
  */
  bool getGradientOneColorMode() const;

  /** Description:
      Sets the gradient to be one-color.
  */
  void setGradientOneColorMode(bool oneColorMode);
  
  /** Description:
      Returns a value in the range 0.0 to 1.0, where 0.0 corresponds to a luminance 
      of 0 and 1.0, to full luminance.

      If the gradient is using one-color mode, this function gets the luminance 
      value applied to the first color.
  */
  double getShadeTintValue() const;
  
  /** Description:
      Sets the one color tint shade (luminance) value.

      If the gradient is using one-color mode, this function sets the luminance 
      value applied to the first color.
  */
  void setShadeTintValue(double value);
  
  /** Description:
      Returns the current interpolation value between the gradient definition's 
      default and shifted values. The default is 0.0f. 
  */
  double gradientShift() const;
  
  /** Description:
      This method sets the interpolation value between the gradient definition's 
      default and shifted values. A value of 0.0 evaluates as a fully unshifted 
      gradient. A value of 1.0 evaluates as a fully shifted gradient.
  */
  void setGradientShift(double shiftValue);
  
  /** Description:
      This method provides a way to evaluate the interpolated color that the current 
      gradient definition and colors will produce at a specified location along 
      the gradient. The point of evaluation is indicated by the argument value.
  */
  void evaluateGradientColorAt(double value, OdCmColor& color) const;

  virtual void subClose();
};

typedef OdSmartPtr<OdDbHatch> OdDbHatchPtr;

#include "DD_PackPop.h"

#endif /* OD_DBHATCH_H */
