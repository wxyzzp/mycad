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



#ifndef OD_DBLEOD_H
#define OD_DBLEOD_H 1

#include "DD_PackPush.h"

#include "DbCurve.h"
#include "DbDimStyleTableRecord.h"

/** Description:
    Represents a leader in an OdDbDatabase.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbLeader : public OdDbCurve
{
public:
  ODDB_DECLARE_MEMBERS(OdDbLeader);

  /** Description:
      Constructor (no arguments).
  */
  OdDbLeader();
    
  /** Description:
      Returns the normal for this leader (DXF 210).
  */
  virtual OdGeVector3d normal() const;
  
  /** Description:
      Returns the number of vertices in this leader (DXF 76).
  */
  virtual int numVertices() const;

  /** Description:
      Appends the specified vertex to this leader.
  */
  virtual bool appendVertex(const OdGePoint3d&);

  /** Description:
      Removes the last vertex from this leader.
  */
  virtual void removeLastVertex();

  /** Description:
      Returns the first vertex in this leader.
  */
  virtual OdGePoint3d firstVertex() const;

  /** Description:
      Returns the last vertex in this leader.
  */
  virtual OdGePoint3d lastVertex() const;

  /** Description:
      Returns a specified vertex in this leader.
  */
  virtual OdGePoint3d vertexAt(int) const;

  /** Description:
      Sets a specified vertex in this leader.

      Arguments:
      index (I) Index of vertex to set.
      v (I) Vertex to set.

      Return Value:
      true if the vertex was set successfully, false otherwise.
  */
  virtual bool setVertexAt(int index, const OdGePoint3d& v);
  
  /** Description:
      Returns true if this leader has an arrowhead, false otherwise (DXF 71).
  */
  virtual bool hasArrowHead() const;

  /** Description:
      Enables the arrowhead for this leader (DXF 71).
  */
  virtual void enableArrowHead();

  /** Description:
      Disables the arrowhead for this leader (DXF 71).
  */
  virtual void disableArrowHead();

  /** Description:
      Returns true if this leader has a hookline, false otherwise (DXF 75).
  */
  virtual bool hasHookLine() const;

  /** Description:
      Returns true if this leader's hookline is on annotation X direction,
      false otherwise (DXF 74).
  */
  virtual bool isHookLineOnXDir() const;

  /** Description:
      Sets this leader to use a spline leader path type (DXF 72).
  */
  virtual void setToSplineLeader();

  /** Description:
      Sets this leader to use a straight line leader path type (DXF 72).
  */
  virtual void setToStraightLeader();

  /** Description:
      Returns true if this leaders uses a spline path type, false otherwise (DXF 72).
  */
  virtual bool isSplined() const;
  
  /** Description:
      Returns the Object ID of the dimension style used by this leader (DXF 3).
  */
  virtual OdDbHardPointerId dimensionStyle() const;

  /** Description:
      Sets the dimension style to be used by this leader (DXF 3).
  */
  virtual void setDimensionStyle(const OdDbHardPointerId&);
    
  /** Description:
      Sets the annotation object to be used by this leader (DXF 340).

      Remarks:
      The annotation object must be one of OdDbMText, OdDbFcf, OdDbBlockReference 
      or their inheritants.  The leader is appended to the annotation object's reactor list.
  */
  virtual void attachAnnotation(OdDbObjectId annoId);

  /** Description:
      Sets the annotation object to be used by this leader (DXF 340)
      and values depending on it which normally are calculated by evaluateLeader().

      Remarks:
      The annotation object must be one of OdDbMText, OdDbFcf, OdDbBlockReference 
      or their inheritants.  The leader is appended to the annotation object's reactor list.
  */
  virtual void attachAnnotation(OdDbObjectId annoId,
                                  OdGeVector3d vXDir,
                                  double dAnnotationWidth,
                                  double dAnnotationHeight,
                                  bool bHookLineOnXDir);

  /** Description:
      Detaches the annotation object used by this leader, so that that annotation object
      is no longer associated with this object.
  */
  virtual void detachAnnotation();

  /** Description:
      Returns the Object ID of the annotation object associated with this leader (DXF 340).
  */
  virtual OdDbObjectId annotationObjId() const;

  /** Description:
      Returns the annotation's "horizontal" direction (DXF 211).
  */
  virtual OdGeVector3d annotationXDir() const;

  /** Description:
      Returns the annotation offset or offset of the last leader point from the
      annotation placement point for this leader (DXF 213).
  */
  virtual OdGeVector3d annotationOffset() const;

  /** Description:
      Sets the annotation offset or offset of the last leader point from the
      annotation placement point for this leader (DXF 213).
  */
  virtual void setAnnotationOffset(const OdGeVector3d& offset);
  
  /** Description:
      The type of annotation used by a leader entity. 
  */
  enum AnnoType
  { 
    /** MText entity (OdDbMText). */
    kMText = 0,
    /** Tolerance entity (OdDbFcf). */
    kFcf,
    /** Block reference. */
    kBlockRef,
    /** No annotation. */
    kNoAnno 
  };

  /** Description:
      Returns the type of the annotation associated with this leader (DXF 73).
  */
  AnnoType annoType() const;

  /** Description:
      Returns the height of the annotation associated with this leader (DXF 40).
  */
  double annoHeight() const;

  /** Description:
      Returns the width of the annotation associated with this leader (DXF 41).
  */
  double annoWidth() const;
  
  /** Description:
      Returns the DIMASZ value for this entity.  If this entity contains an override for this value,
      the override will be returned, othwerwise the value from the dimension style used by this
      entity will be returned.
  */
  virtual double dimasz() const;

  /** Description:
      Returns the DIMCLRD value for this entity.  If this entity contains an override for this value,
      the override will be returned, othwerwise the value from the dimension style used by this
      entity will be returned.
  */
  virtual OdCmColor dimclrd() const;

  /** Description:
      Returns the DIMGAP value for this entity.  If this entity contains an override for this value,
      the override will be returned, othwerwise the value from the dimension style used by this
      entity will be returned.
  */
  virtual double dimgap() const;

  /** Description:
      Returns the DIMLWD value for this entity.  If this entity contains an override for this value,
      the override will be returned, othwerwise the value from the dimension style used by this
      entity will be returned.
  */
  virtual OdDb::LineWeight dimlwd() const;

  /** Description:
      Returns the DIMLDRBLK value for this entity.  If this entity contains an override for this value,
      the override will be returned, othwerwise the value from the dimension style used by this
      entity will be returned.
  */
  virtual OdDbObjectId dimldrblk() const;

  /** Description:
      Returns the DIMSAH value for this entity.  If this entity contains an override for this value,
      the override will be returned, othwerwise the value from the dimension style used by this
      entity will be returned.
  */
  virtual bool dimsah() const;

  /** Description:
      Returns the DIMSCALE value for this entity.  If this entity contains an override for this value,
      the override will be returned, othwerwise the value from the dimension style used by this
      entity will be returned.
  */
  virtual double dimscale() const;

  /** Description:
      Returns the DIMTAD value for this entity.  If this entity contains an override for this value,
      the override will be returned, othwerwise the value from the dimension style used by this
      entity will be returned.
  */
  virtual int dimtad() const;

  /** Description:
      Returns the DIMTXSTY value for this entity.  If this entity contains an override for this value,
      the override will be returned, othwerwise the value from the dimension style used by this
      entity will be returned.
  */
  virtual OdDbObjectId dimtxsty() const;

  /** Description:
      Returns the DIMTXT value for this entity.  If this entity contains an override for this value,
      the override will be returned, othwerwise the value from the dimension style used by this
      entity will be returned.
  */
  virtual double dimtxt() const;
  
  /** Description:
      Sets the DIMASZ value for this entity.  
  */
  virtual void setDimasz(double val);

  /** Description:
      Sets the DIMCLRD value for this entity.  
  */
  virtual void setDimclrd(const OdCmColor& val);

  /** Description:
      Sets the DIMGAP value for this entity.  
  */
  virtual void setDimgap(double val);

  /** Description:
      Sets the DIMLDRBLK value for this entity.  
  */
  virtual void setDimldrblk(OdDbObjectId val);

  /** Description:
      Sets the DIMLDRBLK value for this entity.  
  */
  virtual void setDimldrblk(const char* val);

  /** Description:
      Sets the DIMLWD value for this entity.  
  */
  virtual void setDimlwd(OdDb::LineWeight v); 

  /** Description:
      Sets the DIMSAH value for this entity.  
  */
  virtual void setDimsah(bool val);

  /** Description:
      Sets the DIMSCALE value for this entity.  
  */
  virtual void setDimscale(double val);

  /** Description:
      Sets the DIMTAD value for this entity.  
  */
  virtual void setDimtad(int val);

  /** Description:
      Sets the DIMTXSTY value for this entity.  
  */
  virtual void setDimtxsty(OdDbObjectId val);

  /** Description:
      Sets the DIMTXT value for this entity.  
  */
  virtual void setDimtxt(double val);
    
  virtual void getClassID(void** pClsid) const;
  
  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;

  virtual bool worldDraw(OdGiWorldDraw* pWd) const;

  virtual OdResult explode(OdRxObjectPtrArray& entitySet) const; /* Replace OdRxObjectPtrArray */

  virtual void subClose();

  virtual OdUInt32 setAttributes(OdGiDrawableTraits* pTraits) const;
 
  virtual bool isPlanar() const;

  virtual OdResult getPlane(OdGePlane& plane, OdDb::Planarity& planarity) const;

  /** Description:
      Verifies whether polyline is closed or not (DXF 70, bit 0x01).
  */  
  virtual bool isClosed() const;

  /** Description:
      Returns true if this curve is periodic, false otherwise.
  */
  virtual bool isPeriodic() const;

  /** Description:
      Returns the start parameter of this curve.

      Arguments:
      param (O) Receives the parameter value.

      Return Value:
      eOk if successful, otherwise an appropriate error code.
  */
  virtual OdResult getStartParam(double& param) const;

  /** Description:
      Returns the end parameter of this curve.

      Arguments:
      param (O) Receives the parameter value.

      Return Value:
      eOk if successful, otherwise an appropriate error code.
  */
  virtual OdResult getEndParam (double& param) const;

  /** Description:
      Returns the start point of this curve.

      Arguments:
      pt (O) Receives the point value.

      Return Value:
      eOk if successful, otherwise an appropriate error code.  
  */
  virtual OdResult getStartPoint(OdGePoint3d& pt) const;

  /** Description:
      Returns the end point of this curve.

      Arguments:
      pt (O) Receives the point value.

      Return Value:
      eOk if successful, otherwise an appropriate error code.  
  */
  virtual OdResult getEndPoint(OdGePoint3d& pt) const;

  /** Description:
      Returns the point at the specified parameter value.

      Arguments:
      param (I) Parameter value.
      retPt (O) Receives the point at the specified parameter value.

      Return Value:
      eOk if successful, otherwise an appropriate error code.
  */
  virtual OdResult getPointAtParam(double param, OdGePoint3d& retPt) const;

  /** Description:
      Returns the parameter of specified point of this curve.

      Arguments:
      pt (I) Point of the curve.
      param (O) Computed parameter.

      Return Value:
      eOk if successful, otherwise an appropriate error code.
  */
  virtual OdResult getParamAtPoint(const OdGePoint3d& pt, double& param) const;

  virtual OdResult transformBy(const OdGeMatrix3d& xform);

  /** Description:
       It is override for OdDbEntity::subSetDatabaseDefaults() to set the object’s dimension style 
       to the current style for the database.
  */
  void subSetDatabaseDefaults(OdDbDatabase *pDb);
  
  /** Description:
      Evaluate the relation of the leader to its associated annotation,
      and update the leader geometry if necessary.
  */
  virtual OdResult evaluateLeader();

  
  /** Description:
      Support for persistent reactor to annotation.
  */
  virtual void modified(const OdDbObject*);
  virtual void erased(const OdDbObject*, bool = true);

  /** Description:
      Copies the dimension style data used by this data (including overrides),
      into the passed in dimension style table record.

      Arguments:
      pRes (O) Receives the dimension style data associated with this object.
  */
  void getDimstyleData(OdDbDimStyleTableRecord *pRes) const;

    /** Sets the dimension style data for this object (including overrides),
      from the passed in dimension style table record.
      @param pNewData (I) Dimstyle table record containing the dimension style data 
      to be used for this object.
  */
  void setDimstyleData(const OdDbDimStyleTableRecord* pNewData);

  /** Description:
      Sets the dimension style data for this object (including overrides),
      from the passed in dimension style table record.

      Arguments:
      newDataId (I) Object ID of the dimstyle table record containing the 
      dimension style data to be used for this object.
  */
  void setDimstyleData(OdDbObjectId newDataId);


  /** Description:
      Sets the plane that will contain the leader object

      Arguments:
      plane (I)
  */
  virtual void setPlane(const OdGePlane& plane);

/*
     virtual void goodbye(const OdDbObject*);
     virtual void copied(const OdDbObject*, const OdDbObject*);
     
     virtual void intersectWith(const OdDbEntity*,
       OdDb::Intersect, 
       OdGePoint3dArray&,
       int thisGsMarker = 0,
       int otherGsMarker = 0) const;
     virtual void intersectWith(const OdDbEntity*,
       OdDb::Intersect, 
       const OdGePlane& projPlane, 
       OdGePoint3dArray&,
       int thisGsMarker = 0, 
       int otherGsMarker = 0) const;

  */

};

typedef OdSmartPtr<OdDbLeader> OdDbLeaderPtr;

#include "DD_PackPop.h"

#endif


