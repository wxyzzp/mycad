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



#if !defined(_ODRESBUF_H___INCLUDED_)
#define _ODRESBUF_H___INCLUDED_

#include "DD_PackPush.h"

#include "RxObject.h"
#include "OdString.h"

class OdResBufImpl;
class OdBinaryData;
class OdGePoint3d;
class OdDbObjectId;
class OdDbDatabase;
class OdResBuf;
typedef OdSmartPtr<OdResBuf> OdResBufPtr;

/** Description:

    {group:Other_Classes}
*/
class TOOLKIT_EXPORT OdError_InvalidResBuf : public OdError
{
public:
  OdError_InvalidResBuf() : OdError(eInvalidResBuf){}
};

/** Description:

    {group:Other_Classes}
*/
class TOOLKIT_EXPORT OdResBuf : public OdRxObject
{
public:
  void copyFrom(const OdRxObject* pFrom);

  enum {
        kRtNone         = 5000,
        kRtReal         = 5001, /* Real number */
        kRtPoint2d      = 5002, /* 2D point X and Y only */
        kRtShort        = 5003, /* Short integer */
        kRtAngle        = 5004, /* Angle */
        kRtString       = 5005, /* String */
        kRtEntityName   = 5006, /* Entity name */
        kRtPics         = 5007, /* Pick set */
        kRtOrientation  = 5008, /* Orientation */
        kRtPoint3d      = 5009, /* 3D point - X, Y, and Z */
        kRtLong         = 5010, /* Long integer */
        kRtVoid         = 5014, /* Blank symbol */
        kRtListBeg      = 5016, /* list begin */
        kRtListEnd      = 5017, /* list end */
        kRtDottedPair   = 5018, /* dotted pair */
        kRtNil          = 5019, /* nil */
        kRtDxf0         = 5020, /* DXF code 0 */
        kRtT            = 5021, /* T atom */
        kRtResBuf       = 5023, /* resbuf */

        kDxfInvalid        = -9999,
        kDxfXDictionary     = -6,
        kDxfPReactors       = -5,
        kDxfUndo            = -4,
        kDxfXDataStart      = -3,
        kDxfHeaderId        = -2,
        kDxfFirstEntId      = -2,
        kDxfEnd             = -1,
        kDxfStart           = 0,
        kDxfText            = 1,
        kDxfXRefPath        = 1,
        kDxfShapeName       = 2,
        kDxfBlockName       = 2,
        kDxfAttributeTag    = 2,
        kDxfSymbolTableName = 2,
        kDxfMstyleName      = 2,
        kDxfSymTableRecName = 2,
        kDxfAttributePrompt = 3,
        kDxfDimStyleName    = 3,
        kDxfLinetypeProse   = 3,
        kDxfTextFontFile    = 3,
        kDxfDescription     = 3,
        kDxfDimPostStr      = 3,
        kDxfTextBigFontFile = 4,
        kDxfDimAPostStr     = 4,
        kDxfCLShapeName     = 4,
        kDxfSymTableRecComments = 4,
        kDxfHandle          = 5,
        kDxfDimBlk          = 5,
        kDxfDimBlk1         = 6,
        kDxfLinetypeName    = 6,
        kDxfDimBlk2         = 7,
        kDxfTextStyleName   = 7,
        kDxfLayerName       = 8,
        kDxfCLShapeText     = 9,
        kDxfXCoord         = 10,
        kDxfYCoord         = 20,
        kDxfZCoord         = 30,
        kDxfElevation      = 38,
        kDxfThickness      = 39,
        kDxfReal           = 40,
        kDxfViewportHeight = 40,
        kDxfTxtSize        = 40,
        kDxfTxtStyleXScale = 41,
        kDxfViewWidth      = 41,
        kDxfViewportAspect = 41,
        kDxfTxtStylePSize  = 42,
        kDxfViewLensLength = 42,
        kDxfViewFrontClip  = 43,
        kDxfViewBackClip   = 44,
        kDxfShapeXOffset   = 44,
        kDxfShapeYOffset   = 45,
        kDxfViewHeight     = 45,
        kDxfShapeScale     = 46,
        kDxfPixelScale     = 47,
        kDxfLinetypeScale  = 48,
        kDxfDashLength     = 49,
        kDxfMlineOffset    = 49,
        kDxfLinetypeElement = 49,
        kDxfAngle          = 50,
        kDxfViewportSnapAngle = 50,
        kDxfViewportTwist  = 51,
        kDxfVisibility     = 60,
        kDxfLayerLinetype  = 61,
        kDxfColor          = 62,
        kDxfHasSubentities = 66,
        kDxfViewportVisibility = 67,
        kDxfViewportActive = 68,
        kDxfViewportNumber = 69,
        kDxfInt16          = 70,
        kDxfViewMode       = 71,
        kDxfCircleSides    = 72,
        kDxfViewportZoom   = 73,
        kDxfViewportIcon   = 74,
        kDxfViewportSnap   = 75,
        kDxfViewportGrid   = 76,
        kDxfViewportSnapStyle= 77,
        kDxfViewportSnapPair= 78,
        kDxfRegAppFlags    = 71,
        kDxfTxtStyleFlags  = 71,
        kDxfLinetypeAlign  = 72,
        kDxfLinetypePDC    = 73,
        kDxfInt32          = 90,
        kDxfSubclass            = 100,
        kDxfEmbeddedObjectStart = 101,
        kDxfControlString       = 102,
        kDxfDimVarHandle = 105,
        kDxfUCSOrg         = 110,
        kDxfUCSOriX        = 111,
        kDxfUCSOriY        = 112,
        kDxfXReal          = 140,
        kDxfXInt16         = 170,
        kDxfNormalX        = 210,
        kDxfNormalY        = 220,
        kDxfNormalZ        = 230,
        kDxfXXInt16        = 270,
        kDxfInt8           = 280,
        kDxfRenderMode     = 281,
        kDxfBool           = 290,
        kDxfXTextString    = 300,

        //  Arbitrary Binary Chunks 310-319
        kDxfBinaryChunk   = 310,

        //  Arbitrary Object Handles 320-329
        kDxfArbHandle  = 320,

        kDxfSoftPointerId   = 330,  // 330-339
        kDxfHardPointerId   = 340,  // 340-349
        kDxfSoftOwnershipId = 350,  // 350-359
        kDxfHardOwnershipId = 360,  // 360-369

        kDxfLineWeight        = 370,
        kDxfPlotStyleNameType = 380,
        kDxfPlotStyleNameId   = 390,
        kDxfXXXInt16          = 400,
        kDxfLayoutName     = 410,
        kDxfComment        = 999,
        kDxfXdAsciiString  = 1000,
        kDxfRegAppName     = 1001,
        kDxfXdControlString = 1002,
        kDxfXdLayerName    = 1003,
        kDxfXdBinaryChunk  = 1004,
        kDxfXdHandle       = 1005,
        kDxfXdXCoord       = 1010,
        kDxfXdYCoord       = 1020,
        kDxfXdZCoord       = 1030,
        kDxfXdWorldXCoord  = 1011,
        kDxfXdWorldYCoord  = 1021,
        kDxfXdWorldZCoord  = 1031,
        kDxfXdWorldXDisp   = 1012,
        kDxfXdWorldYDisp   = 1022,
        kDxfXdWorldZDisp   = 1032,
        kDxfXdWorldXDir    = 1013,
        kDxfXdWorldYDir    = 1023,
        kDxfXdWorldZDir    = 1033,
        kDxfXdReal         = 1040,
        kDxfXdDist         = 1041,
        kDxfXdScale        = 1042,
        kDxfXdInteger16    = 1070,
        kDxfXdInteger32    = 1071
  };

  ODRX_DECLARE_MEMBERS(OdResBuf);

  ~OdResBuf();

  /** Description:
      Returns the group code of the OdResBuf.  The OdDxfCode::_getType() function
      can be used to determine the type of data associated with a group code.
  */
  int restype() const;
  void setRestype(int restype);

  OdResBufPtr next() const;
  OdResBufPtr last() const;
  OdResBufPtr insert(OdResBuf* pRb);
  OdResBufPtr setNext(OdResBuf* pRb);

  /** Description:
      Returns the string data value of the result buffer.  This function should be called 
      when the data type of the result buffer is OdDxfCode::Name or OdDxfCode::String.
  */
  OdString getString() const;

  /** Description:
      Returns the bool data value of the result buffer.  This function should be called 
      when the data type of the result buffer is OdDxfCode::Bool.
  */
  bool     getBool() const;

  /** Description:
      Returns the OdInt8 data value of the result buffer.  This function should be called 
      when the data type of the result buffer is OdDxfCode::Integer8.
  */
  OdInt8   getInt8() const;

  /** Description:
      Returns the OdInt16 data value of the result buffer.  This function should be called 
      when the data type of the result buffer is OdDxfCode::Integer16.
  */
  OdInt16  getInt16() const;

  /** Description:
      Returns the OdInt32 data value of the result buffer.  This function should be called 
      when the data type of the result buffer is OdDxfCode::Integer32.
  */
 OdInt32  getInt32() const;

  /** Description:
      Returns the double data value of the result buffer.  This function should be called 
      when the data type of the result buffer is OdDxfCode::Double.
  */
  double   getDouble() const;

  /** Description:
      Returns the OdGePoint3d data value of the result buffer.  This function should be called 
      when the data type of the result buffer is OdDxfCode::Point.
  */
  const OdGePoint3d& getPoint3d() const;

  /** Description:
      Returns the binary data value of the result buffer.  This function should be called 
      when the data type of the result buffer is OdDxfCode::BinaryChunk.

      Return Value:
      The binary data from the result buffer.
  */
  const OdBinaryData& getBinaryChunk() const;

  /** Description:
      Returns the handle data value of the result buffer.  This function should be called 
      when the data type of the result buffer is OdDxfCode::Handle.
  */
  OdDbHandle getHandle() const;

  OdDbObjectId getObjectId(OdDbDatabase* pDb) const;

  /** Description:
      Sets the data for the result buffer. 
  */
  void setString(const OdString& val);

  /** Description:
      Sets the data for the result buffer. 
  */
  void setBool(bool val);

  /** Description:
      Sets the data for the result buffer. 
  */
  void setInt8(OdInt8 val);

  /** Description:
      Sets the data for the result buffer. 
  */
  void setInt16(OdInt16 val);
  /** Description:
      Sets the data for the result buffer. 
  */
  void setInt32(OdInt32 val);

  /** Description:
      Sets the data for the result buffer. 
  */
  void setDouble(double val);

  /** Description:
      Sets the data for the result buffer. 
  */
  void setPoint3d(const OdGePoint3d& val);

  /** Description:
      Sets the data for the result buffer. 
  */
  void setBinaryChunk(const OdBinaryData& val);

  /** Description:
      Sets the data for the result buffer. 
  */
  void setHandle(const OdDbHandle& val);

  /** Description:
      Sets the data for the result buffer. 
  */
  void setObjectId(const OdDbObjectId& val);

  static OdResBufPtr newRb(int resType = OdResBuf::kRtNone);

  static OdResBufPtr newRb(int resType, bool val);
  static OdResBufPtr newRb(int resType, OdInt8 val);
  static OdResBufPtr newRb(int resType, OdUInt8 val);
  static OdResBufPtr newRb(int resType, OdInt16 val);
  static OdResBufPtr newRb(int resType, OdUInt16 val);
  static OdResBufPtr newRb(int resType, OdInt32 val);
  static OdResBufPtr newRb(int resType, OdUInt32 val);
  static OdResBufPtr newRb(int resType, double val);
  static OdResBufPtr newRb(int resType, const OdGePoint3d& val);
  static OdResBufPtr newRb(int resType, const OdString& val);
  static OdResBufPtr newRb(int resType, const OdChar* val);
protected:
  /** Description:
      Constructors
  */
  OdResBuf();
  OdResBuf(OdResBufImpl* pResBufImpl);

  OdResBufImpl* m_pImpl;
};

inline OdResBufPtr OdResBuf::newRb(int resType, bool val)
{
  OdResBufPtr pRes = newRb(resType);
  pRes->setBool(val);
  return pRes;
}

inline OdResBufPtr OdResBuf::newRb(int resType, double val)
{
  OdResBufPtr pRes = newRb(resType);
  pRes->setDouble(val);
  return pRes;
}

inline OdResBufPtr OdResBuf::newRb(int resType, const OdGePoint3d& val)
{
  OdResBufPtr pRes = newRb(resType);
  pRes->setPoint3d(val);
  return pRes;
}

inline OdResBufPtr OdResBuf::newRb(int resType, const OdString& val)
{
  OdResBufPtr pRes = newRb(resType);
  pRes->setString(val);
  return pRes;
}

inline OdResBufPtr OdResBuf::newRb(int resType, const OdChar* val)
{
  OdResBufPtr pRes = newRb(resType);
  pRes->setString(val);
  return pRes;
}

inline OdResBufPtr OdResBuf::newRb(int resType, OdInt8 val)
{
  OdResBufPtr pRes = newRb(resType);
  pRes->setInt8(val);
  return pRes;
}

inline OdResBufPtr OdResBuf::newRb(int resType, OdUInt8 val)
{
  OdResBufPtr pRes = newRb(resType);
  pRes->setInt8(val);
  return pRes;
}

inline OdResBufPtr OdResBuf::newRb(int resType, OdInt16 val)
{
  OdResBufPtr pRes = newRb(resType);
  pRes->setInt16(val);
  return pRes;
}

inline OdResBufPtr OdResBuf::newRb(int resType, OdUInt16 val)
{
  OdResBufPtr pRes = newRb(resType);
  pRes->setInt16(val);
  return pRes;
}

inline OdResBufPtr OdResBuf::newRb(int resType, OdInt32 val)
{
  OdResBufPtr pRes = newRb(resType);
  pRes->setInt32(val);
  return pRes;
}

inline OdResBufPtr OdResBuf::newRb(int resType, OdUInt32 val)
{
  OdResBufPtr pRes = newRb(resType);
  pRes->setInt32(val);
  return pRes;
}

#include "DD_PackPop.h"

#endif //_ODRESBUF_H___INCLUDED_


