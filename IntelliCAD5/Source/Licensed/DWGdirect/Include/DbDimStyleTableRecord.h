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



#ifndef _ODDBDIMSTYLETABLERECORD_INCLUDED
#define _ODDBDIMSTYLETABLERECORD_INCLUDED

#include "DD_PackPush.h"

#include "DbSymbolTableRecord.h"

/** Namespace containing utility functions for dimension style processing.

    {group:DD_Namespaces}
*/
namespace OdDmUtil
{
  /** Description:
      Returns the correct R14 DIMFIT value corresponding to the passed in
      values for DIMATFIT and DIMTMOVE.
  */
  int TOOLKIT_EXPORT dimfit(int dimatfit, int dimtmove);
  
  /** Description:
      Returns the correct R14 DIMUNIT value corresponding to the passed in
      values for DIMLUNIT and DIMFRAC.
  */
  int TOOLKIT_EXPORT dimunit(int dimlunit, int dimfrac);
  
  /** Description:
      Returns the R15+ DIMATFIT value from the passed in R14 DIMFIT value.
  */
  int TOOLKIT_EXPORT dimatfit(int dimfit);
  
  /** Description:
      Returns the R15+ DIMTMOVE value from the passed in R14 DIMFIT value.
  */
  int TOOLKIT_EXPORT dimtmove(int dimfit);
  
  /** Description:
      Returns the R15+ DIMLUNIT value from the passed in R14 DIMUNIT value.
  */
  int TOOLKIT_EXPORT dimlunit(int dimunit);
  
  /** Description:
      Returns the R15+ DIMFRAC value from the passed in R14 DIMUNIT value.
  */
  int TOOLKIT_EXPORT dimfrac(int dimunit);
    
  /** Description:
      Returns the "arrow" name of the block referenced by the specified Object ID.
      This is the block name with the optional leading underscore ('_') removed.
  */
  OdString TOOLKIT_EXPORT arrowName(OdDbObjectId blockId);
  
  /** Description:
      Returns true if the specified string is the name of one of the built-in 
      arrow types.  
      
      Remarks:
      The built-in arrow types are:
        - None
        - Closed
        - Dot
        - ClosedBlank
        - Oblique
        - ArchTick
        - Open
        - Origin
        - Origin2
        - Open90
        - Open30
        - DotBlank
        - DotSmall
        - BoxFilled
        - BoxBlank
        - DatumFilled
        - DatumBlank
        - Integral
        - Small
  */
  bool TOOLKIT_EXPORT isBuiltInArrow(const OdChar* pName);
  
  /** Description:
      Returns true if the specified string is the name of one of the built-in 
      arrow types that are treated as having zero length.  
      
      Remarks:
      Valid arrow types are:
        - None
        - Oblique
        - ArchTick
        - DotSmall
        - Integral
        - Small
  */
  bool TOOLKIT_EXPORT isZeroLengthArrow(const OdChar* pName);
  
  /** Description:
      Returns the Object ID of the arrow block specified by the passed in arrow name.

      Arguments:
      pName (I) Name of arrow type.
      pDb (I) Database in which to look for the arrow block.

      Return Value:
      Object ID of the specified arrow block, or a null Object ID.

      Remarks:
  */
  OdDbObjectId TOOLKIT_EXPORT findArrowId(const OdChar* pName, OdDbDatabase* pDb);
  
  /** Description:
      Returns the Object ID of the arrow block specified by the passed in arrow name, creating
      this block if it does not already exist.

      Arguments:
      pName (I) Name of arrow type.
      pDb (I) Database in which to look for the arrow block.

      Return Value:
      Object ID of the specified arrow block.

      Remarks:
  */
  OdDbObjectId TOOLKIT_EXPORT getArrowId(const OdChar* pName, OdDbDatabase* pDb);

  /*
       OdString globalArrowName(const OdChar* pName);  
       OdString globalArrowName(OdDbObjectId blockId);
       OdString arrowName(const OdChar* pName);
  */

}

/** Description:

    {group:DD_Namespaces}
*/
namespace OdDb
{
  enum DimArrowFlags
  {
    kFirstArrow  = 0,
    kSecondArrow = 1
  };
}

/** Description:
    Represents a dimension style table entry in an OdDbDatabase.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbDimStyleTableRecord: public OdDbSymbolTableRecord
{
public:
  ODDB_DECLARE_MEMBERS(OdDbDimStyleTableRecord);
  
  /** Description:
      Constructor (no arguments).
  */
  OdDbDimStyleTableRecord();
    
#define VAR_DEF(a, b, dxf, c, d, r1, r2) \
/** Returns the dim##b value of this object. */ \
a dim##b() const; \
/** Sets the dim##b value of this object. */ \
void setDim##b(a);
#include "DimVarDefs.h"
#undef  VAR_DEF
  
  /** Description:
      Sets the DIMBLK value for this object as a string. 
  */
  void setDimblk(const OdChar*);

  /** Description:
      Sets the DIMBLK1 value for this object as a string. 
  */
  void setDimblk1(const OdChar*);

  /** Description:
      Sets the DIMBLK2 value for this object as a string. 
  */
  void setDimblk2(const OdChar*);

  /** Description:
      Sets the DIMLDRBLK value for this object as a string.
  */
  void setDimldrblk(const OdChar*);
  
  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);
  
  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;
  
  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);
  
  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;
  
  void appendToOwner(OdDbIdPair& pair, OdDbObject* pOwner, OdDbIdMapping& idMap);

  virtual OdResult dxfInFields_R12(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields_R12(OdDbDxfFiler* pFiler) const;
  
  virtual void getClassID(void** pClsid) const;

  /** Description:
  */
  OdDbObjectId arrowId(OdDb::DimArrowFlags arrowType) const;

  /*
  OdString dimpost() const;
  OdString dimapost() const;
  OdString dimblk() const;
  OdString dimblk1() const;
  OdString dimblk2() const;
  
  int dimfit() const;
  int dimunit() const;
    
  void setDimfit(int fit);
  void setDimunit(int unit);
  bool isModifiedForRecompute() const;
  */

};

typedef OdSmartPtr<OdDbDimStyleTableRecord> OdDbDimStyleTableRecordPtr;

#include "DD_PackPop.h"

#endif // _ODDBDIMSTYLETABLERECORD_INCLUDED


