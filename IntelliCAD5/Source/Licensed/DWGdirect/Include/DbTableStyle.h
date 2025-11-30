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

#ifndef OD_DBTABLESTYLE_H
#define OD_DBTABLESTYLE_H

#include "DD_PackPush.h"
#include "DbObject.h"
#include "DbColor.h"

namespace OdDb
{
  // OdDbTable and OdTbTableStyle specific enum
  //
  enum CellType          { kUnknownCell    = 0,
                           kTextCell       = 1,
                           kBlockCell      = 2 };

  enum CellEdgeMask      { kTopMask        = 1,
                           kRightMask      = 2,
                           kBottomMask     = 4,
                           kLeftMask       = 8 };

  enum SelectType        { kWindow         = 1,
                           kCrossing       = 2 };

  enum FlowDirection     { kTtoB           = 0,
                           kBtoT           = 1 };

  enum RotationAngle     { kDegreesUnknown = -1,
                           kDegrees000     = 0,
                           kDegrees090     = 1,
                           kDegrees180     = 2,
                           kDegrees270     = 3 };

  enum CellAlignment     { kTopLeft        = 1,
                           kTopCenter      = 2,
                           kTopRight       = 3,
                           kMiddleLeft     = 4,
                           kMiddleCenter   = 5,
                           kMiddleRight    = 6,
                           kBottomLeft     = 7,
                           kBottomCenter   = 8,
                           kBottomRight    = 9 };

  enum GridLineType      { kInvalidGridLine= 0,
                           kHorzTop        = 1,
                           kHorzInside     = 2,
                           kHorzBottom     = 4,
                           kVertLeft       = 8,
                           kVertInside     = 0x10,
                           kVertRight      = 0x20 };


  enum RowType           { kUnknownRow     = 0,
                           kDataRow        = 1,
                           kTitleRow       = 2,
                           kHeaderRow      = 4};

  enum TableStyleFlags   { kHorzInsideLineFirst  = 1,
                           kHorzInsideLineSecond = 2,
                           kHorzInsideLineThird  = 4,
                           kTableStyleModified   = 8 };


  enum RowTypes          { kAllRows = kDataRow | kTitleRow | kHeaderRow };
  enum GridLineTypes     { kAllGridLines = kHorzTop | kHorzInside | kHorzBottom | kVertLeft | kVertInside | kVertRight };
}

/** Description:
    Class that represents table styles for OdDbTable entities in an OdDbDatabase.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbTableStyle : public OdDbObject
{
public:
  ODDB_DECLARE_MEMBERS(OdDbTableStyle);

  OdDbTableStyle();
  // virtual ~OdDbTableStyle();

  // General Properties
  //
  /** Description:
      returns name of the Table Style

      Arguments:
      name (O) - String which is set to Style's name

      Returns eOk if successful, or eOwnerNotSet if the style
      is not yet in the style dictionary
  */
  virtual OdResult   getName(OdString& name) const;

  /*
  virtual OdResult   setName(const OdString& name);
  */

  /** Description:
      Returns the description of the table style(DXF 3).
  */
  virtual OdString   description(void) const;

  /** Description:
      Sets the description of the table style to description (DXF 3).
  */
  virtual void   setDescription(const OdString& description);

  /** Description:
      Returns an unsigned integer representing the operation bit setFlowDirection 
      flags set for the OdDbTableStyle object(DXF 71).
      
      This will be combination of:

      o kHorzInsideLineFirst
      o kHorzInsideLineSecond
      o kHorzInsideLineThird
      o kTableStyleModified
  */
  virtual OdUInt32   bitFlags() const;

  /** Description:
      Sets the bit flag value for the table style object(DXF 71).
      
      This may be combination of:

      o kHorzInsideLineFirst
      o kHorzInsideLineSecond
      o kHorzInsideLineThird
      o kTableStyleModified
  */
  virtual void   setBitFlags(OdUInt32 flags);

  /** Description:
      Returns the flow direction of the table (DXF 70) .

      This will be one of:

      o kTtoB
      o kBtoT
  */
  virtual OdDb::FlowDirection flowDirection(void) const; 

  /** Description:
      Sets the flow direction for the table in the table style object(DXF 70).
      
      This must be one of:

      o kTtoB
      o kBtoT
  */
  virtual void   setFlowDirection(OdDb::FlowDirection flow);

  /** Description:
      Returns the horizontal cell margin value used by the table style(DXF 40).
  */
  virtual double     horzCellMargin(void) const;

  /** Description:
      Sets the horizontal cell margin value used by the table style(DXF 40)
  */
  virtual void   setHorzCellMargin(double dCellMargin);

  /** Description:
      Returns the vertical cell margin value used by the table style(DXF 41).
  */
  virtual double     vertCellMargin(void) const;

  /** Description:
      Sets the vertical cell margin value used by the table style(DXF 41)
  */
  virtual void   setVertCellMargin(double dCellMargin);

  /** Description:
      Returns a true/false status for the title suppressed flag(DXF 280).
  */
  virtual bool       isTitleSuppressed(void) const;

  /** Description:
      Suppresses or enables the title row for the table(DXF 280).
  */
  virtual void       suppressTitleRow(bool bValue);

  /** Description:
      Returns a true/false status for the header suppressed flag(DXF 281).
  */
  virtual bool       isHeaderSuppressed(void) const;

  /** Description:
      Suppresses or enables the header row for the table(DXF 281).
  */
  virtual void       suppressHeaderRow(bool bValue);


  // Cell properties
  //
  /** Description:
      Returns the object ID of the text style for a given row(DXF 7).

      The possible values of rowType are:
       
      o kDataRow
      o kTitleRow 
      o kHeaderRow 

      If no row type is specified, the default of kDataRow will be used.
  */
  virtual OdDbObjectId        textStyle(OdDb::RowType rowType = OdDb::kDataRow) const;

  /** Description:
      Sets the table object to use the OdDbTextStyleTableRecord specified by ID for 
      the specified row types(DXF 7).

      The possible values of rowType are:

      o kDataRow
      o kTitleRow 
      o kHeaderRow 

      Different row types can be OR'd.
      If no row type is specified, this function sets the object ID for all possible row types. 

      Returns eOk if successful; otherwise, returns eInvalidInput.
  */
  virtual void   setTextStyle(const OdDbObjectId id,
                                          int rowTypes = OdDb::kAllRows);
  /** Description:
      Returns the text height for a given row type of the table style object (DXF 140).

      The possible values of rowType are:

      o kDataRow
      o kTitleRow 
      o kHeaderRow 

      If no row type is specified, the default of kDataRow will be used.
  */
  virtual double     textHeight(OdDb::RowType rowType = OdDb::kDataRow) const;
  /** Description:
      Sets the text height for the specified row types (DXF 140).

      The possible values of rowType are:
      
      o kDataRow
      o kTitleRow 
      o kHeaderRow 

      Different row types can be OR'd.
      If no row type is specified, this function sets text height for all possible row types. 
      Returns eOk if successful; otherwise, returns eInvalidInput.
  */
  virtual void   setTextHeight(double dHeight,
                                            int rowTypes = OdDb::kAllRows);

  /** Description:
      Returns the cell alignment for a given row type of the table style object(DXF 170).

      The possible values of rowType are:

      o kDataRow
      o kTitleRow 
      o kHeaderRow 

      The possible values for cell alignment are:
      
      o kTopLeft
      o kTopCenter
      o kTopRight
      o kMiddleLeft
      o kMiddleCenter
      o kMiddleRight
      o kBottomLeft
      o kBottomCenter, 
      o kBottomRight

      If no row type is specified, the default of kDataRow will be used.
  */

  virtual OdDb::CellAlignment alignment(OdDb::RowType rowType = OdDb::kDataRow) const;
  /** Description:
      Sets the cell alignment for the specified row types(DXF 170). 
      
      The possible values of rowType are:
      
      o kDataRow
      o kTitleRow 
      o kHeaderRow 

      Different row types can be OR'd.

      The possible values for cell alignment are:
      
      o kTopLeft
      o kTopCenter
      o kTopRight
      o kMiddleLeft
      o kMiddleCenter
      o kMiddleRight
      o kBottomLeft
      o kBottomCenter, 
      o kBottomRight

      If no row type is specified, this function sets cell alignment for all possible row types. 
      Returns eOk if successful; otherwise, returns eInvalidInput.
  */
  virtual void   setAlignment(OdDb::CellAlignment alignment,
                                           int rowTypes = OdDb::kAllRows);
  /** Description:
      Returns the text color for a given row type of the table style object(DXF 62).

      The possible values of rowType are:

      o kDataRow
      o kTitleRow 
      o kHeaderRow 

      If no row type is specified, the default of kDataRow will be used.
  */

  virtual OdCmColor  color(OdDb::RowType rowType = OdDb::kDataRow) const;
  /** Description:
      Sets the text color for the specified row types(DXF 62). 

      The possible values of rowType are:

      o kDataRow
      o kTitleRow 
      o kHeaderRow 
      
      Different row types can be OR'd.
      If no row type is specified, this function sets text color for all possible row types. 
      Returns eOk if successful; otherwise, returns eInvalidInput.
  */
  virtual void   setColor(const OdCmColor& color,
                                       int rowTypes = OdDb::kAllRows);

  /** Description:
      Returns the background color for a given row type of the table style object(DXF 63).

      The possible values of rowType are:

      o kDataRow
      o kTitleRow 
      o kHeaderRow 
  */
  virtual OdCmColor  backgroundColor(OdDb::RowType rowType = OdDb::kDataRow) const;
  /** Description:
      Sets the background color for the specified row types(DXF 63).

      The possible values of rowType are:

      o kDataRow
      o kTitleRow 
      o kHeaderRow 

      Different row types can be OR'd.
      If no row type is specified, this function sets background color for all possible row types. 
      Returns eOk if successful; otherwise, returns eInvalidInput.
  */
  virtual void   setBackgroundColor(const OdCmColor& color,
                                                 int rowTypes = OdDb::kAllRows);

  /** Description:
      Returns true if the background color for a given row type of the table style object is set 
      to no color value(DXF 283). Otherwise, returns false.

      The possible values of rowType are:

      o kDataRow
      o kTitleRow 
      o kHeaderRow 
      
      If no row type is specified, the default of kDataRow will be used.
  */

  virtual bool       isBackgroundColorNone(OdDb::RowType rowType = OdDb::kDataRow) const;

  /** Description:
      Enables or disables the background color for the specified row types(DXF 283). 

      The possible values of rowType are:

      o kDataRow
      o kTitleRow 
      o kHeaderRow 

      Different row types can be OR'd.
      If no row type is specified, this function enables or disables the background color 
      for all possible row types. 
      
      Returns eOk if successful; otherwise, returns eInvalidInput.
  */
  virtual void   setBackgroundColorNone(bool bValue,
                                                     int rowTypes = OdDb::kAllRows);

  //Gridline properties
  //
  /** Description:
      Returns the lineweight for a given grid line type and row type of the table style object(DXF 274-279).
    
      The possible values of gridLineType are:

      o kHorzTop
      o kHorzInside
      o kHorzBottom
      o kVertLeft
      o kVertInside
      o kVertRight.

      The possible values of rowType are:

      o kDataRow
      o kTitleRow 
      o kHeaderRow 
      
      If no row type is specified, the default of kDataRow will be used.
  */
  virtual OdDb::LineWeight    gridLineWeight(OdDb::GridLineType gridLineType, 
                                             OdDb::RowType rowType = OdDb::kDataRow) const;

  /** Description:
      Sets the lineweight for the specified grid line types and row types (DXF 274-279). 
      
      The possible values of gridLineType are:

      o kHorzTop
      o kHorzInside
      o kHorzBottom
      o kVertLeft
      o kVertInside
      o kVertRight.
      
      Different grid line and row types can be OR'd.

      The possible values of rowType are:

      o kDataRow
      o kTitleRow 
      o kHeaderRow 

      Different row types can be OR'd.

      Returns eOk if successful; otherwise, returns eInvalidInput.
  */  
  virtual void   setGridLineWeight(OdDb::LineWeight lineWeight,
                                                int gridLineTypes = OdDb::kAllGridLines, 
                                                int rowTypes = OdDb::kAllRows);
  
  /** Description:
      Returns the grid color for a given grid line type of the table style object(DXF 64-69).

      The possible values of gridLineType are:

      o kHorzTop
      o kHorzInside
      o kHorzBottom
      o kVertLeft
      o kVertInside
      o kVertRight.

      The possible values of rowType are:

      o kDataRow
      o kTitleRow 
      o kHeaderRow 
      
      If no row type is specified, the default of kDataRow will be used.
  */
  virtual OdCmColor  gridColor(OdDb::GridLineType gridLineType, 
                               OdDb::RowType rowType = OdDb::kDataRow) const;
  /** Description:
      Sets the grid color for the specified grid line types and row types(DXF 64-69).

      The possible values of gridLineType are:

      o kHorzTop
      o kHorzInside
      o kHorzBottom
      o kVertLeft
      o kVertInside
      o kVertRight.
      
      Different grid line and row types can be OR'd.

      The possible values of rowType are:

      o kDataRow
      o kTitleRow 
      o kHeaderRow 
      
      Different row types can be OR'd.

      Returns eOk if successful; otherwise, returns eInvalidInput.
  */
  virtual void   setGridColor(const OdCmColor color,
                                           int gridLineTypes = OdDb::kAllGridLines, 
                                           int rowTypes = OdDb::kAllRows);

  /** Description:
      Returns the grid visibility for a given grid line type of the table style object(DXF 284-289).

      The possible values of gridLineType are:

      o kHorzTop
      o kHorzInside
      o kHorzBottom
      o kVertLeft
      o kVertInside
      o kVertRight.

      The possible values of rowType are:

      o kDataRow
      o kTitleRow 
      o kHeaderRow 

      If no row type is specified, the default of kDataRow will be used.
  */
  virtual OdDb::Visibility    gridVisibility(OdDb::GridLineType gridLineType, 
                                             OdDb::RowType rowType = OdDb::kDataRow) const;
  /** Description:
      Sets the grid visibility for the specified grid line types and row types(DXF 284-289). 

      The possible values of gridLineType are:

      o kHorzTop
      o kHorzInside
      o kHorzBottom
      o kVertLeft
      o kVertInside
      o kVertRight.

      Different grid line and row types can be OR'd.

      The possible values of rowType are:

      o kDataRow
      o kTitleRow 
      o kHeaderRow 
      
      Different row types can be OR'd.

      Returns eOk if successful; otherwise, returns eInvalidInput.
  */
  virtual void   setGridVisibility(OdDb::Visibility visible,
                                                int gridLineTypes = OdDb::kAllGridLines, 
                                                int rowTypes = OdDb::kAllRows);

  virtual OdResult   dwgInFields(OdDbDwgFiler* pFiler);

  virtual void   dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual OdResult   dxfInFields(OdDbDxfFiler* pFiler);

  virtual void   dxfOutFields(OdDbDxfFiler* pFiler) const;

  // virtual OdResult	audit(OdDbAuditInfo* pAuditInfo);

  // Utility functions
  //

  /** Description:
      Sets the default properties for table style from the specified database.
      If passed database pointer is NULL, table style must be already in database.
  */
  void setDatabaseDefaults(OdDbDatabase* pDb = NULL);

  /** Description:
      Adds a table style to the specified database.
  
      Arguments:
      pDb (I) Pointer to the database to which to add the table style 
      styleName (I) Name for the table style. Must be a non-empty string that is valid for Dictionary key strings 
      tableStyleId (O) Table style object ID, if the table style is successfully added to the database 


      Returns eOk if successful,
              eAlreadyInDb if the table style already exists in the database,
              eDuplicateKey if styleName already exists in the style dictionary,
              eInvalidInput if styleName is invalid.
  */
  virtual OdResult   postTableStyleToDb(OdDbDatabase* pDb, 
                                        const OdString& styleName, 
                                        OdDbObjectId& tableStyleId);

  virtual void getClassID(void** pClsid) const;
};

typedef OdSmartPtr<OdDbTableStyle> OdDbTableStylePtr;

#include "DD_PackPop.h"

#endif // OD_DBTABLESTYLE_H

