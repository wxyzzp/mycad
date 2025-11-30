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

#ifndef __ODGIMATERIALDATA_H__
#define __ODGIMATERIALDATA_H__

#include "DD_PackPush.h"

#include "CmColor.h"
#include "Ge/GeMatrix3d.h"

/** Description:

    {group:OdGi_Classes} 
*/
class OdGiMaterialColor
{
public:
  enum Method
  {
      kInherit = 0,
      kOverride
  };

  static const OdGiMaterialColor kNull;

  OdGiMaterialColor()
    : m_method(kInherit)
    , m_factor(1.0)
  {
  }

  /** Description:
      Set properties of material color.
  */
  inline void setMethod(Method method);
  inline void setFactor(double dFactor);
  inline OdCmEntityColor& color();

  /** Description:
      Return current settings.
  */
  inline Method method() const;
  inline double factor() const;
  inline const OdCmEntityColor& color() const;

private:
  Method m_method;
  double m_factor;
  OdCmEntityColor m_color;
};

/** Description:

    {group:OdGi_Classes} 
*/
class OdGiMapper
{
public:
  enum Projection
  {
      kInheritProjection = 0,
      kPlanar,
      kBox,
      kCylinder,
      kSphere
  };

  enum Tiling
  {
      kInheritTiling = 0,
      kTile,
      kCrop,
      kClamp
  };

  enum AutoTransform
  {
      kInheritAutoTransform = 0x0,
      kNone                 = 0x1,
      kObject               = 0x2,
      kModel                = 0x4
  };

  static const OdGiMapper kIdentity;

  OdGiMapper()
    : m_projection(kPlanar)
    , m_tiling(kTile)
    , m_autoTransform(kNone)
  {}

  /** Description:
      Set properties of material mapper.
  */
  inline void setProjection(Projection projection);
  inline void setTiling(Tiling tiling);
  inline void setAutoTransform(AutoTransform autoTransform);
  inline OdGeMatrix3d& transform();

  /** Description:
      Return current settings.
  */
  inline Projection projection() const;
  inline Tiling tiling() const;
  inline AutoTransform autoTransform() const;
  inline const OdGeMatrix3d& transform() const;

private:
  Projection    m_projection;
  Tiling        m_tiling;
  AutoTransform m_autoTransform;
  OdGeMatrix3d  m_transform;
};

/** Description:

    {group:OdGi_Classes} 
*/
class OdGiMaterialMap
{
public:
  enum Source
  {
      kScene = 0,
      kFile
  };

  static const OdGiMaterialMap kNull;

  OdGiMaterialMap()
    : m_source(kFile)
    , m_blendFactor(1.0)
  {}

  /** Description:
      Set properties of material map.
  */
  inline void setSource(Source source);
  inline void setSourceFileName(const OdString& fileName);
  inline void setBlendFactor(double blendFactor);
  inline OdGiMapper& mapper();

  /** Description:
     Return current settings.
  */
  inline Source source() const;
  inline OdString sourceFileName() const;
  inline double blendFactor() const;
  inline const OdGiMapper& mapper() const;

private:
  Source     m_source;
  OdString   m_fileName;
  double     m_blendFactor;
  OdGiMapper m_mapper;
};

//----------------------------------------------------------
//
// Inline implementation
//
//----------------------------------------------------------
// OdGiMaterialColor
//
inline
void OdGiMaterialColor::setMethod(Method method)
{
  m_method = method;
}
inline
void OdGiMaterialColor::setFactor(double dFactor)
{
  m_factor = dFactor;
}
inline
OdCmEntityColor& OdGiMaterialColor::color()
{
  return m_color;
}
inline
OdGiMaterialColor::Method OdGiMaterialColor::method() const
{
  return m_method;
}
inline
double OdGiMaterialColor::factor() const
{
  return m_factor;
}
inline
const OdCmEntityColor& OdGiMaterialColor::color() const
{
  return m_color;
}

// OdGiMaterialMap
//
inline
void OdGiMaterialMap::setSource(Source source)
{
  m_source = source;
}
inline
void OdGiMaterialMap::setSourceFileName(const OdString& fileName)
{
  m_fileName = fileName;
}
inline
void OdGiMaterialMap::setBlendFactor(double blendFactor)
{
  m_blendFactor = blendFactor;
}
inline
OdGiMapper& OdGiMaterialMap::mapper()
{
  return m_mapper;
}
inline
OdGiMaterialMap::Source OdGiMaterialMap::source() const
{
  return m_source;
}
inline
OdString OdGiMaterialMap::sourceFileName() const
{
  return m_fileName;
}
inline
double OdGiMaterialMap::blendFactor() const
{
  return m_blendFactor;
}
inline
const OdGiMapper& OdGiMaterialMap::mapper() const
{
  return m_mapper;
}

// OdGiMapper
//
inline
void OdGiMapper::setProjection(Projection projection)
{
  m_projection = projection;
}
inline
void OdGiMapper::setTiling(Tiling tiling)
{
  m_tiling = tiling;
}
inline
void OdGiMapper::setAutoTransform(AutoTransform autoTransform)
{
  m_autoTransform = autoTransform;
}
inline
OdGeMatrix3d& OdGiMapper::transform()
{
  return m_transform;
}
inline
OdGiMapper::Projection OdGiMapper::projection() const
{
  return m_projection;
}
inline
OdGiMapper::Tiling OdGiMapper::tiling() const
{
  return m_tiling;
}
inline
OdGiMapper::AutoTransform OdGiMapper::autoTransform() const
{
  return m_autoTransform;
}
inline
const OdGeMatrix3d& OdGiMapper::transform() const
{
  return m_transform;
}

#include "DD_PackPop.h"

#endif

