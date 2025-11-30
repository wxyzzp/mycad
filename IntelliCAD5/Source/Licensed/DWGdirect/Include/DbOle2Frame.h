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



#ifndef _OD_DB_OLE2FRAME_
#define _OD_DB_OLE2FRAME_

#include "DD_PackPush.h"

#include "DbOleFrame.h"

class OdDbOle2FrameImpl;

class OdByteData;
typedef OdSmartPtr<OdByteData> OdByteDataPtr;

/** Description:
    Represents an OLE2 Frame object in an OdDbDatabase.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbOle2Frame : public OdDbOleFrame
{
public:
  ODDB_DECLARE_MEMBERS(OdDbOle2Frame);

  OdDbOle2Frame();
  
  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;
  
  virtual bool worldDraw(OdGiWorldDraw*) const;

  virtual void viewportDraw(OdGiViewportDraw* mode) const;

  /* 
     virtual void viewportDraw(OdGiViewportDraw* mode);
     virtual bool getGeomExtents(OdGeExtents3d& extents) const;
  */

  virtual OdResult transformBy(const OdGeMatrix3d& xform);
  
  /** Description:
      Retrieves the upper left corner of the OLE object (DXF 10).
  */
  virtual void getLocation(OdGePoint3d& point3d) const;

  /** Description:
      Retrieves the corner points of the OLE object (DXF 10 & 11).
  */
  virtual void position(OdRectangle3d& rect3d) const;
  
  /** Description:
      Sets the corner points of the OLE object (DXF 10 & 11).
  */
  virtual void setPosition(const OdRectangle3d& rect3d);

  /* 
     virtual void position(RECT& rect) const;
     virtual void setPosition(const RECT& rect);
  */

  /** Description:
      Gets the user type string (for example "Paintbrush Bitmap") (DXF 3)
  */
  virtual OdString getUserType() const;
  
  
  /** Description:
      Returns the type of this OLE object, linked, embedded, or static (DXF 71).  
      
      Remarks:
      See MFC COleClientItem::GetType():

        OT_LINK     - (1) The OLE item is a link.
	      OT_EMBEDDED - (2) The OLE item is embedded.
	      OT_STATIC   - (3) The OLE item is static. It contains only presentation data, not native data, and thus cannot be edited.
  */
  virtual int getType(void) const;

  /** Description:
      TBC.
  */
  //virtual bool getLinkName(char* pszName, int nNameSize) const;
  
  /** Description:
      TBC.
  */
  //virtual bool getLinkPath(char* pszPath, int nPathSize) const;
  
  /** Description:
      Returns the output quality for this OLE object (DXF 73).
  */
  virtual OdUInt8 outputQuality() const;

  /** Description:
      Sets the output quality for this OLE object (DXF 73).
  */
  virtual void setOutputQuality(OdUInt8);


  // These functions can be removed/replaced in future
  // They interacts with first 0x80 bytes of ole2frame's binary data

  /** Description:
      Gets/sets the first byte of ole2frame's binary data. 
      
      Remarks:
      Flag values include:

        0x01 - Paper Space entity
        0x02 - unknown (0)
        0x04 - not selectable
        0x08 - unknown (0)
        0x10 - unknown (0)
        0x20 - unknown (0)
        0x40 - unknown (0)
        0x80 - unknown (1)

  */
  OdUInt8 getUnknown1() const;
  void setUnknown1(OdUInt8);

  /** Description:
      Gets/sets the second byte of ole2frame's binary data. We don't exactly know what is it, 
      probably some flags. In most cases it is 0x55.
  */
  OdUInt8 getUnknown2() const;
  void setUnknown2(OdUInt8);

  // next data is four vertice points (position).

  /** Description:
      The next two bytes seem to be a width of rasterized object in pixels
      or in 0.01 mm units ?
  */
  OdUInt16 getPixelWidth() const;
  void setPixelWidth(OdUInt16);

  /** Description:
      The next two bytes seem to be a height of rasterized object in pixels
      or in 0.01 mm units ?
  */
  OdUInt16 getPixelHeight() const;
  void setPixelHeight(OdUInt16);

  /** Description:
      The next four bytes seem to be a "is in model space" flag(it is in api since we're not sure)
      1 for modelSpace, 0 for paperSpace.
  */
  OdInt32 getModelFlag() const;
  void setModelFlag(OdInt32);

  /** Description:
      The next four bytes seem to be a number of colors in metafile.
      In most cases, it is 256.
  */
  OdUInt32 getColorsNum() const;
  void setColorsNum(OdUInt32);

  /** Description:
      The next four bytes seem to be a some kind of counter.
      It works when 1 is here.
  */
  OdUInt32 getCounter() const;
  void setCounter(OdUInt32);

  /** Description:
      The next four bytes (activatable)
      0 - application is unknown
      1 - application is known
  */
  OdUInt32 getUnknown6() const;
  void setUnknown6(OdUInt32);

  /** Description:
      The next two bytes (inserted or copied)
      0 - copied (from clipboard)
      1 - inserted
  */
  OdInt16 getUnknown7() const;
  void setUnknown7(OdInt16);

  /** Description:
      The next four bytes seem to be an aspect enumeration.
      1 DVASPECT_CONTENT  
      2 DVASPECT_THUMBNAIL
      4 DVASPECT_ICON     
      8 DVASPECT_DOCPRINT */
  OdUInt32 getAspect() const;
  void setAspect(OdUInt32);

  /** Description:
      Returns the compound document file stored in this Ole2Frame object.
      
      Arguments:
        bForWrite (I) If true, the changes to the returned OdByteDataPtr will 
          be reflected in this object's data.  Otherwise, a separate copy of the 
          compound document file will be returned.
  */
  OdByteDataPtr compDocFile(bool bForWrite);

  /** Description:
      Returns raster preview if it presents in OLE stream
  */
  OdGiRasterImagePtr getRasterImage() const;

  /** Description:
      Returns metafile preview if it presents in OLE stream
  */
  OdGiMetafilePtr getMetafile() const;

  virtual void getClassID(void** pClsid) const;
};

typedef OdSmartPtr<OdDbOle2Frame> OdDbOle2FramePtr;

#include "DD_PackPop.h"

#endif

