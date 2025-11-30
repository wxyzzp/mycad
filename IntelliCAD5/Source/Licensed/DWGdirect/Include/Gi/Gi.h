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
// programs incorporating this software must include the following statment 
// with their copyright notices:
//
//      DWGdirect © 2002 by Open Design Alliance Inc. All rights reserved. 
//
// By use of this software, you acknowledge and accept the terms of this 
// agreement.
///////////////////////////////////////////////////////////////////////////////



#ifndef _OD_GI_H_
#define _OD_GI_H_

#include "RxObject.h"

class OdDbStub;
class OdCmEntityColor;
class OdGeVector3d;

typedef enum
{
  kOdGiFillAlways = 1,
  kOdGiFillNever
} OdGiFillType;

typedef enum
{
  kOdGiInvisible = 0,
  kOdGiVisible,
  kOdGiSilhouette
} OdGiVisibility;

#include "DD_PackPush.h"

/** Description:

    {group:OdGi_Classes} 
*/
class FIRSTDLL_EXPORT OdGiDrawableTraits : public OdRxObject
{ 
public:
  ODRX_DECLARE_MEMBERS(OdGiDrawableTraits);
};

/** Description:
    An interface that provides get/set functions for attributes within the 
    current vectorization process.  
    
    Remarks:
    An OdGiSubEntityTraits instance is available during calls to worldDraw and viewportDraw,
    so that drawable classes can control attributes during the vectorization process.
    The pure virtual functions in this class are implemented by the OdGiAbstractVectorizer
    class.

    {group:OdGi_Classes} 
*/
class FIRSTDLL_EXPORT OdGiSubEntityTraits : public OdGiDrawableTraits
{ 
public:
  ODRX_DECLARE_MEMBERS(OdGiSubEntityTraits);

  /** Description:
      Sets the current drawing color (as an index).  See Include/AcadPalette.h for 
      a list of RGB colors that correspond to the set of valid color indices.
  */
  virtual void setColor(OdUInt16 color) = 0;

  /** Description:
      Sets the current drawing color as an OdCmEntityColor, which can be an index
      or an RGB color.
  */
  virtual void setTrueColor(const OdCmEntityColor& cmColor) = 0;

  /** Description:
      Sets the current layer.
  */
  virtual void setLayer(OdDbStub* layerId) = 0;

  /** Description:
      Sets the current linetype.
  */
  virtual void setLineType(OdDbStub* linetypeId) = 0;

  /** Description:
      Sets the current selection marker.
  */
  virtual void setSelectionMarker(OdInt32 markerId) = 0;

  /** Description:
      Sets the current fill type.
  */
  virtual void setFillType(OdGiFillType fillType) = 0;

  /** Description:
      Sets the current fill plane.
  */
  virtual void setFillPlane(const OdGeVector3d* /*pNormal*/ = 0) {}

  /** Description:
      Sets the current line weight.
  */
  virtual void setLineWeight(OdDb::LineWeight lw) = 0;

  /** Description:
      Sets the current linetype scale.
  */
  virtual void setLineTypeScale(double dScale = 1.0) = 0;

  /** Description:
      Sets the current linetype "scale to fit" property.
  */
  virtual void setLineTypeScaleToFit(bool /*doIt*/) /*= 0*/ {};

  /** Description:
      Sets the current thickness.
  */
  virtual void setThickness(double dThickness) = 0;

  /** Description:
      Sets the current plotstyle.
  */
  virtual void setPlotStyleName(OdDb::PlotStyleNameType, OdDbStub* = 0) = 0;

  /** Description:
      Sets the current material.
  */
  virtual void setMaterial(OdDbStub* materialId) = 0;
  

  /** Description:
      Returns the current color as in index.
  */
  virtual OdUInt16 color() const = 0;

  /** Description:
      Returns the current color as an OdCmEntityColor object.
  */
  virtual OdCmEntityColor trueColor() const = 0;

  /** Description:
      Returns the current layer.
  */
  virtual OdDbStub* layer() const = 0;

  /** Description:
      Returns the current linetype.
  */
  virtual OdDbStub* lineType() const = 0;

  /** Description:
      Returns the current fill type.
  */
  virtual OdGiFillType fillType() const = 0;

  /** Description:
      Returns the current fill plane.
  */
  virtual bool fillPlane(OdGeVector3d& /*normal*/) { return false; }

  /** Description:
      Returns the current lineweight.
  */
  virtual OdDb::LineWeight lineWeight() const = 0;

  /** Description:
      Returns the current linetype scale.
  */
  virtual double lineTypeScale() const = 0;

  /** Description:
      Returns the current thickness.
  */
  virtual double thickness() const = 0;

  /** Description:
      Returns the current plotstyle name type.
  */
  virtual OdDb::PlotStyleNameType plotStyleNameType() const = 0;

  /** Description:
      Returns the current plotstyle object.
  */
  virtual OdDbStub* plotStyleNameId() const = 0;

  /** Description:
      Returns the current material.
  */
  virtual OdDbStub* material() const = 0;
};

typedef OdSmartPtr<OdGiSubEntityTraits> OdGiSubEntityTraitsPtr;

#include "DD_PackPop.h"

#endif //_OD_GI_H_

