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



#ifndef __IMGVARS_H
#define __IMGVARS_H

#include "DD_PackPush.h"

class OdDbRasterVariables;

#include "DbRasterImageDef.h"



typedef OdSmartPtr<OdDbRasterVariables> OdDbRasterVariablesPtr;

/** Description:
    Represents a raster variables object in an OdDbDatabase, which contains settings that apply
    to all image entities in the database.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbRasterVariables : public OdDbObject
{
public:  
  /* static Oda::ClassVersion classVersion();
  */
  
  ODDB_DECLARE_MEMBERS(OdDbRasterVariables);

  /** Description:
      Constructor (no arguments).
  */
  OdDbRasterVariables();
  
  enum FrameSettings
  {
    kImageFrameInvalid = -1,
    kImageFrameOff = 0,
    kImageFrameAbove = 1,
    kImageFrameBelow = 2
  };

  enum ImageQuality
  {
    kImageQualityInvalid = -1,
    kImageQualityDraft = 0,
    kImageQualityHigh = 1
  };
    
  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;
  
  /** Description:
      Returns the image frame display flag (DXF 70).
  */
  virtual FrameSettings imageFrame() const;

  /** Description:
      Sets the image frame display flag (DXF 70).
  */
  virtual void setImageFrame( FrameSettings value );

  /** Description:
      Returns the image display quality (DXF 71).
  */
  virtual ImageQuality imageQuality() const;

  /** Description:
      Sets the image display quality (DXF 71).
  */
  virtual void setImageQuality( ImageQuality value );
  
  /** Description:
      Returns the units for inserting images (DXF 72).
  */
  virtual OdDbRasterImageDef::Units userScale() const;

  /** Description:
      Sets the units for inserting images (DXF 72).
  */
  virtual void setUserScale(OdDbRasterImageDef::Units units);
  
  /** Description:
      Opens the raster variables object in the the specified database, creating
      a raster variables object if one is not present.

      Arguments:
      mode (I) Mode in which to open the raster variables object.
      pDatabase (I) Database that contains the raster variables object.

      Return Value:
      Smart pointer to the raster variables object.
  */
  static OdDbRasterVariablesPtr openRasterVariables(OdDbDatabase* pDatabase,
    OdDb::OpenMode mode = OdDb::kForRead);
};

#include "DD_PackPop.h"

#endif // __IMGVARS_H


