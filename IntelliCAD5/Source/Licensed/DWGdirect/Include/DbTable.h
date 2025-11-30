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

#ifndef OD_DBTABLE_H
#define OD_DBTABLE_H

#include "DD_PackPush.h"
// #include "DbEntity.h"
#include  "DbBlockReference.h"
#include "DbTableStyle.h"
// typedef for OdSubentPathArray
//
class OdDbTableImpl;

typedef OdArray<OdDbFullSubentPath,
         OdMemoryAllocator<OdDbFullSubentPath> > OdSubentPathArray;


/** Description:
    Class that represents simple table entities in an OdDbDatabase.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbTable: public OdDbBlockReference
{
public:
  ODDB_DECLARE_MEMBERS(OdDbTable);

  enum TableStyleOverrides {
    //table
    kTitleSuppressed             = 1,
    kHeaderSuppressed            = 2,
    kFlowDirection               = 3,         
    kHorzCellMargin              = 4,
    kVertCellMargin              = 5,
    kTitleRowColor               = 6,
    kHeaderRowColor              = 7,
    kDataRowColor                = 8,
    kTitleRowFillNone            = 9,
    kHeaderRowFillNone           = 10,
    kDataRowFillNone             = 11,
    kTitleRowFillColor           = 12,
    kHeaderRowFillColor          = 13,
    kDataRowFillColor            = 14,
    kTitleRowAlignment           = 15,
    kHeaderRowAlignment          = 16,
    kDataRowAlignment            = 17,
    kTitleRowTextStyle           = 18,
    kHeaderRowTextStyle          = 19,
    kDataRowTextStyle            = 20,
    kTitleRowTextHeight          = 21,
    kHeaderRowTextHeight         = 22,
    kDataRowTextHeight           = 23,

    // grid color
    kTitleHorzTopColor           = 40,
    kTitleHorzInsideColor        = 41,
    kTitleHorzBottomColor        = 42,
    kTitleVertLeftColor          = 43,
    kTitleVertInsideColor        = 44,
    kTitleVertRightColor         = 45,

    kHeaderHorzTopColor          = 46,
    kHeaderHorzInsideColor       = 47,
    kHeaderHorzBottomColor       = 48,
    kHeaderVertLeftColor         = 49,
    kHeaderVertInsideColor       = 50,
    kHeaderVertRightColor        = 51,

    kDataHorzTopColor            = 52,
    kDataHorzInsideColor         = 53,
    kDataHorzBottomColor         = 54,
    kDataVertLeftColor           = 55,
    kDataVertInsideColor         = 56,
    kDataVertRightColor          = 57,

    // grid line weight
    kTitleHorzTopLineWeight      = 70,
    kTitleHorzInsideLineWeight   = 71,
    kTitleHorzBottomLineWeight   = 72,
    kTitleVertLeftLineWeight     = 73,
    kTitleVertInsideLineWeight   = 74,
    kTitleVertRightLineWeight    = 75,

    kHeaderHorzTopLineWeight     = 76,
    kHeaderHorzInsideLineWeight  = 77,
    kHeaderHorzBottomLineWeight  = 78,
    kHeaderVertLeftLineWeight    = 79,
    kHeaderVertInsideLineWeight  = 80,
    kHeaderVertRightLineWeight   = 81,

    kDataHorzTopLineWeight       = 82,
    kDataHorzInsideLineWeight    = 83,
    kDataHorzBottomLineWeight    = 84,
    kDataVertLeftLineWeight      = 85,
    kDataVertInsideLineWeight    = 86,
    kDataVertRightLineWeight     = 87,

    // grid visibility
    kTitleHorzTopVisibility      = 100,
    kTitleHorzInsideVisibility   = 101,
    kTitleHorzBottomVisibility   = 102,
    kTitleVertLeftVisibility     = 103,
    kTitleVertInsideVisibility   = 104,
    kTitleVertRightVisibility    = 105,

    kHeaderHorzTopVisibility     = 106,
    kHeaderHorzInsideVisibility  = 107,
    kHeaderHorzBottomVisibility  = 108,
    kHeaderVertLeftVisibility    = 109,
    kHeaderVertInsideVisibility  = 110,
    kHeaderVertRightVisibility   = 111,

    kDataHorzTopVisibility       = 112,
    kDataHorzInsideVisibility    = 113,
    kDataHorzBottomVisibility    = 114,
    kDataVertLeftVisibility      = 115,
    kDataVertInsideVisibility    = 116,
    kDataVertRightVisibility     = 117,

    //cell 
    //
    kCellAlignment               = 130,
    kCellBackgroundFillNone      = 131,
    kCellBackgroundColor         = 132,
    kCellContentColor            = 133,
    kCellTextStyle               = 134,
    kCellTextHeight              = 135,
    kCellTopGridColor            = 136,
    kCellRightGridColor          = 137,
    kCellBottomGridColor         = 138,
    kCellLeftGridColor           = 139,
    kCellTopGridLineWeight       = 140,
    kCellRightGridLineWeight     = 141,
    kCellBottomGridLineWeight    = 142,
    kCellLeftGridLineWeight      = 143,
    kCellTopVisibility           = 144,
    kCellRightVisibility         = 145,
    kCellBottomVisibility        = 146,
    kCellLeftVisibility          = 147    
  };

  OdDbTable();
  // virtual ~OdDbTable();


  //********************************************************************
  // Get and set methods for table properties
  //********************************************************************
  //
  /** Description:
      Returns the object ID of the OdDbTableStyle referenced by the table object(DXF 342).
  */
  virtual OdDbObjectId tableStyle() const;

  /** Description:
      Sets the table to use the OdDbTableStyle with the given object ID(DXF 342).

      Returns eOk if successful. Otherwise, returns eInvalidInput.
  */
  virtual void setTableStyle(const OdDbObjectId& id);

  /** Description:
      Returns the unit horizontal direction vector for the table in WCS coordinates(DXF 11,21,31).
  */
  virtual OdGeVector3d direction() const;
  
  /** Description:
      Sets the horizontal direction (X axis, in WCS coordinates) for the table(DXF 11,21,31).
  */
  virtual void setDirection(const OdGeVector3d& horzVec);

  /** Description:
      Returns a non-negative integer representing the number of rows in the table(DXF 91). 
  */
  virtual OdUInt32 numRows() const;

  /** Description:
  */
  virtual void setNumRows(OdUInt32 nRows);

  /** Description:
      Returns a non-negative integer representing the number of columns in the table(DXF 92). 
  */
  virtual OdUInt32 numColumns() const;

  /** Description:
  */
  virtual void setNumColumns(OdUInt32 nCols);

  /** Description:
      Returns the total width for the table.It is calculated value.
  */
  virtual double     width() const;

  /** Description:
      Sets the total column width for the table. The individual column widths may be increased 
      or decreased proportionally.
  */
  virtual void setWidth(double width);

  /** Description:
      Returns the column width at the specified column index in the table(DXF 142). 
  */
  virtual double columnWidth(OdUInt32 col) const;
  
  /** Description:
      sets the column width at the specified column index in the table(DXF 142).
  */
  virtual void setColumnWidth(OdUInt32 col, double width);

  /** Description:
      sets the column width for all the columns in the table(DXF 142).
  */
  virtual void setColumnWidth(double width);

  /** Description:
      Returns the total height for the table.It is calculated value.
  */
  virtual double height() const;

  /** Description:
      Sets the total row height for the table. The individual row heights may be increased 
      or decreased proportionally.
  */
  virtual void setHeight(double height);

  /** Description:
      Returns the row height of the specified row in the table(DXF 141).
  */
  virtual double rowHeight(OdUInt32 row) const;
  
  /** Description:
      Sets the row height of the specified row in the table(DXF 141).
  */
  virtual void setRowHeight(OdUInt32 row, double height);

  /** Description:
      Sets the row height  for all the rows in the table(DXF 141).
  */
  virtual void setRowHeight(double height);
  
  /** Description:
      Returns the minimum column width at the specified column index in the table.
  */
  virtual double minimumColumnWidth(OdUInt32 col) const;

  /** Description:
      Returns the minimum row height for the specified row in the table.
   */
  virtual double minimumRowHeight(OdUInt32 row) const;    

  /** Description:
      Returns the minimum total width for the table.
  */
  virtual double minimumTableWidth() const;
  
  /** Description:
      Returns the minimum total height for the table.
  */
  virtual double minimumTableHeight() const;        

  //********************************************************************
  // Get and set methods for table style overrides
  //********************************************************************
  //
  
  /** Description:
      Returns the horizontal cell margin for the table(DXF 40). 
  */
  virtual double horzCellMargin() const;

  /** Description:
      Sets the horizontal cell margin (space between the cell text and the cell border) 
      for the table. In general, the horizontal cell margin should not greater than five 
      times the standard data row text height(DXF 40).  
  */
  virtual void setHorzCellMargin(double gap);

  /** Description:
      Returns the vertical cell margin for the table(DXF 41). 
  */
  virtual double vertCellMargin() const;

  /** Description:
      Sets the vertical cell margin (space between the cell text and the cell border) 
      for the table. In general, the vertical cell margin should not greater than five 
      times the standard data row text height(DXF 41).
  */
  virtual void setVertCellMargin(double gap);

  /** Description:
      Returns the direction that the table follows from its start to its finish(DXF 70).

      This will be one of:
      -kTtoB(top to bottom)
      -kBtoT(bottom to top)
  */
  virtual OdDb::FlowDirection flowDirection() const; 

  /** Description:
      Sets the flow direction for the table(DXF 70). 
      
      Possible values for AcDb::FlowDirection:
      -kTtoB(top to bottom)
      -kBtoT(bottom to top)
  */
  virtual void setFlowDirection(OdDb::FlowDirection flow);

  /** Description:
      Returns true if the title row of the table is suppressed, or false if it is not(DXF 280).
  */
  virtual bool isTitleSuppressed() const;

  /** Description:
      Suppresses or enables the title row for the table(DXF 280).
  */
  virtual void suppressTitleRow(bool value);

  /** Description:
      Returns true if the header row of the table is suppressed, or false if it is not(DXF 281). 
  */
  virtual bool isHeaderSuppressed() const;

  /** Description:
      Suppresses or enables the header row for the table(DXF 281).
  */
  virtual void suppressHeaderRow(bool value);

  /** Description:
      Returns the cell alignment for a given row type of the table object(DXF 170).

      This will be one of:
      -kTopLeft
      -kTopCenter
      -kTopRight
      -kMiddleLeft
      -kMiddleCenter
      -kMiddleRight
      -kBottomLeft
      -kBottomCenter
      -kBottomRight  
  */
  virtual OdDb::CellAlignment alignment(OdDb::RowType type = OdDb::kDataRow) const;

  /** Description:
      Sets the cell alignment for the specified row types. 
      Different row types can be OR'd(DXF 170).
  */
  virtual void setAlignment(OdDb::CellAlignment align, OdUInt32 rowTypes = OdDb::kAllRows);

  /** Description:
      Returns true if the background color for the given row type of the table 
      object is set to no color value. Otherwise, returns false(DXF 283).
  */
  virtual bool isBackgroundColorNone(OdDb::RowType type = OdDb::kDataRow) const;

  /** Description:
      Enables or disables the background color for the specified row type. 
      Different row types can be combined(DXF 283).
  */
  virtual void setBackgroundColorNone(bool value, OdUInt32 rowTypes = OdDb::kAllRows);

  /** Description:
      Returns the background color for a given row type of the table object(DXF 63).
  */
  virtual OdCmColor backgroundColor(OdDb::RowType type = OdDb::kDataRow) const;

  /** Description:
      Sets the background color for the specified row types. 
      Different row types can be OR'd(DXF 63).
  */
  virtual void setBackgroundColor(const OdCmColor& color, OdUInt32 rowTypes = OdDb::kAllRows);

  /** Description:
      Returns the content color for a given row type of the table object(DXF 64).
  */
  virtual OdCmColor contentColor(OdDb::RowType type = OdDb::kDataRow) const;

  /** Description:
      Tets the content color for the specified row types. 
      Different row types can be OR'd(DXF 64).
  */
  virtual void setContentColor(const OdCmColor& color, OdUInt32 nRowType = OdDb::kAllRows);

  /** Description:
      Returns the object ID of the text style for a given row type currently referenced 
      by the table object(DXF 7).
  */
  virtual OdDbObjectId textStyle(OdDb::RowType type = OdDb::kDataRow) const;

  /** Description:
      Sets the table object to use the AcDbTextStyleTableRecord specified by id 
      for the specified row types. Different row types can be OR'd(DXF 7).
  */
  virtual void setTextStyle(const OdDbObjectId& id, OdUInt32 rowTypes = OdDb::kAllRows);

  /** Description:
      Returns the text height for a given row type of the table object(DXF 140).
  */
  virtual double textHeight(OdDb::RowType type = OdDb::kDataRow) const;

  /** Description:
      Sets the text height for the specified row types. Different row types can be OR'd(DXF 140).
  */
  virtual void setTextHeight(double height, OdUInt32 rowTypes = OdDb::kAllRows);

  /** Description:
      Returns the line weight for a given grid line type and row type of the table object(DXF 274-279).
      
      The possible values of the grid line type are:
      -kHorzTop
      -kHorzInside
      -kHorzBottom
      -kVertLeft
      -kVertInside
      -kVertRight.
  */
  virtual OdDb::LineWeight gridLineWeight(OdDb::GridLineType gridlineType,
                              OdDb::RowType type = OdDb::kDataRow) const; 

  /** Description:
      Sets the line weight for a given grid line type and row type of the table object. 
      Different grid line and row types can be combined(DXF 274-279).
  */
  virtual void setGridLineWeight(OdDb::LineWeight lwt, OdUInt32 nBorders, OdUInt32 nRows);

  /** Description:
      Returns the grid color for a given grid line type and row type of the table object. 
      (DXF 63,64,65,66,68,69)
  */
  virtual OdCmColor gridColor(OdDb::GridLineType gridlineType,
                              OdDb::RowType type = OdDb::kDataRow) const; 

  /** Description:
      Sets the grid color for a given grid line type and row type of the table object. 
      Different grid line and row types can be combined(DXF 63,64,65,66,68,69).
  */
  virtual void setGridColor(const OdCmColor& color, OdUInt32 nBorders, OdUInt32 nRows);

  /** Description:
      Returns the grid visibility for a given grid line type and row type of the 
      table object(DXF 284-289). 
  */
  virtual OdDb::Visibility gridVisibility(OdDb::GridLineType gridlineType,
                              OdDb::RowType type = OdDb::kDataRow) const; 

  /** Description:
      Sets the grid visibility for a given grid line type and row type of the table object. 
      Different grid line and row types can be combined(DXF 284-289).
  */
  virtual void setGridVisibility(OdDb::Visibility visible, OdUInt32 nBorders, OdUInt32 nRows);

  /** Description:
      Returns the table style overrides. AcDbTable::TableStyleOverrides defines the values 
      for table style overrides.
  
      Returns true if successful.
  */
  virtual bool tableStyleOverrides( OdArray<OdUInt32>& overrides) const;

  /** Description:
      Clears the overrides used in a table and its cells. 

      The options argument can be one of the following:

      o 0 Delete all table and cell overrides. 
      o 1 Delete all table overrides, but retain cell overrides. 
      o 2 Delete all cell overrides, but retain table overrides. 
  */
  virtual void clearTableStyleOverrides(int options = 0);
  
  //********************************************************************
  // Get and set methods for common cell properties
  //********************************************************************
  //
  
  virtual OdDb::CellType cellType(OdUInt32 row, OdUInt32 col) const;
  virtual void setCellType(OdUInt32 row, OdUInt32 col, OdDb::CellType type);

  virtual void getCellExtents(OdUInt32 row, OdUInt32 col,
                              bool isOuterCell, OdGePoint3dArray& pts) const;

  virtual OdGePoint3d attachmentPoint(OdUInt32 row, OdUInt32 col) const; 

  /** Description:
      Returns the alignment of the text or block in the specified cell of the table.
  */
  virtual OdDb::CellAlignment alignment(OdUInt32 row, OdUInt32 col) const;
  /** Description:
      Sets the alignment of the text or block in the specified cell of the table.  
  */
  virtual void setAlignment(OdUInt32 row, OdUInt32 col, OdDb::CellAlignment align);

  /** Description:
      Returns true if the background color for the specified cell in the table is set to 
      no color value. Otherwise, returns false.
  */
  virtual bool isBackgroundColorNone(OdUInt32 row, OdUInt32 col) const;
  /** Description:
      Enables or disables the background color for the specified cell.
  */
  virtual void setBackgroundColorNone(OdUInt32 row, OdUInt32 col, bool value);

  /** Description:
      Returns the background color of the specified cell in the table.
  */
  virtual OdCmColor backgroundColor(OdUInt32 row, OdUInt32 col) const;

  /** Description:
      Sets the background color of the specified cell in the table. 
  */
  virtual void setBackgroundColor(OdUInt32 row, OdUInt32 col,
                                        const OdCmColor& color);

  /** Description:
      Returns the color of the cell content for the specified cell in the table. 
  */
  virtual OdCmColor contentColor(OdUInt32 row, OdUInt32 col) const;

  /** Description:
      Sets the color of the cell content for the specified cell in the table. 
  */
  virtual void setContentColor(OdUInt32 row, OdUInt32 col,
                                           const OdCmColor& color);
  
  /** Description:
      Returns the cell overrides for the specified cell. 
  */
  virtual bool cellStyleOverrides(OdUInt32 row, OdUInt32 col, 
                                     OdArray<OdUInt32>& overrides) const;
  
  
  /** Description:
      Deletes the cell content for the specified cell.
  */
  virtual void	deleteCellContent(OdUInt32 row, OdUInt32 col);
 

  /** Description:
      Returns the row type of the specified row in the table. 
      
      This will be one of:

      o kDataRow
      o kTitleRow 
      o kHeaderRow 
  */
  virtual OdDb::RowType rowType(OdUInt32 row) const;

  //********************************************************************
  // Get and set methods for text cell properties
  //********************************************************************
  //
 
  /** Description:
      Returns text string in the specified cell.
  */
  virtual OdString textString(OdUInt32 row, OdUInt32 col) const;

  /** Description:
      Sets text string in the specified cell.
  */
  virtual void setTextString(OdUInt32 row, OdUInt32 col, const OdString& text);
    
  /** Description:
      Returns the object ID of the OdDbField in the specified cell. 
  */
  virtual OdDbObjectId fieldId(OdUInt32 row, OdUInt32 col) const;

  /** Description:
      Sets the AcDbField object ID for the specified cell.
  */
  virtual void setFieldId(OdUInt32 row, OdUInt32 col, const OdDbObjectId& fieldId);

  /** Description:
      Returns the object ID of the text style of the specified cell.
  */
  virtual OdDbObjectId textStyle(OdUInt32 row, OdUInt32 col) const;

  /** Description:
      Sets the table object to use the AcDbTextStyleTableRecord specified by id for the 
      specified cell.
  */
  virtual void setTextStyle(OdUInt32 row, OdUInt32 col, const OdDbObjectId& id);

  /** Description:
      Returns the text height of the specified table cell.
  */
  virtual double textHeight(OdUInt32 row, OdUInt32 col) const;

  /** Description:
      Sets the text height for the specified table cell.
  */
  virtual void setTextHeight(OdUInt32 row, OdUInt32 col, double height);

  /** Description:
      Returns the rotation angle of the text in the specified cell. 

      The possible values for rotation angle are:
      -kDegrees000
      -kDegrees090
      -kDegrees180
      -kDegrees270. 
  */
  virtual OdDb::RotationAngle textRotation(OdUInt32 row, OdUInt32 col) const;

  /** Description:
      Sets the rotation angle of the text in the specified cell. 

      The possible values for rotation angle are:
      -kDegrees000
      -kDegrees090
      -kDegrees180
      -kDegrees270. 
  */
  virtual void setTextRotation(OdUInt32 row, OdUInt32 col, OdDb::RotationAngle rot);
  
  //********************************************************************
  // Get and set methods for block cell properties
  //********************************************************************
  //
  /** Description:
      Returns the status of the mBlockAutoFit flag at the specified cell in the table. 
  */
  virtual bool isAutoScale(OdUInt32 row, OdUInt32 col) const;

  /** Description:
      Sets the auto fit flag, which determines whether to automatically fit the block 
      into the specified cell.
  */
  virtual void setAutoScale(OdUInt32 row, OdUInt32 col, bool autoFit);
 
  /** Description:
      Returns the object ID of the block table record at the specified cell in the table. 
  */
  virtual OdDbObjectId blockTableRecordId(OdUInt32 row, OdUInt32 col) const;

  /** Description:
      Sets the object ID of the block table record at the specified cell in the table. 
  */
  virtual void setBlockTableRecordId(OdUInt32 row, OdUInt32 col,
                                        const OdDbObjectId& blkId, bool autoFit = false);

  /** Description:
      Returns the scale factor of the block reference at the specified cell in the table. 
  */
  virtual double blockScale(OdUInt32 row, OdUInt32 col) const;

  /** Description:
      Sets the scale factor of the block reference at the specified cell in the table. 
  */
  virtual void setBlockScale(OdUInt32 row, OdUInt32 col, double scale);

  /** Description:
      Returns the rotation angle of the block reference at the specified cell in the table.
  */
  virtual double blockRotation(OdUInt32 row, OdUInt32 col) const;

  /** Description:
      Sets the rotation angle of the block reference at the specified cell in the table. 
  */
  virtual void setBlockRotation(OdUInt32 row, OdUInt32 col, double rotAng);


  virtual void getBlockAttributeValue(OdUInt32 row, OdUInt32 col, 
                                         const OdDbObjectId& attdefId, OdString& value) const;

  virtual void setBlockAttributeValue(OdUInt32 row, OdUInt32 col,
                                         const OdDbObjectId& attdefId, const OdString& value);

  //********************************************************************
  // Get and set methods for cell grid line properties
  //********************************************************************
  //
  /** Description:
      Returns the grid color of the specified edge of the cell.  
  */
  virtual OdCmColor gridColor(OdUInt32 row, OdUInt32 col,
                               OdDb::CellEdgeMask iEdge) const;
                                        
  /** Description:
    Sets the grid color of the specified edge of the cell.
  */
  virtual void setGridColor(OdUInt32 row, OdUInt32 col, OdInt16 nEdges,
                                        const OdCmColor& color);

  /** Description:
      Returns the grid visibility of the specified edge of the cell.
  */
  virtual OdDb::Visibility gridVisibility(OdUInt32 row, OdUInt32 col,
                                        OdDb::CellEdgeMask iEdge) const;

  /** Description:
      Sets the grid visibility of the specified edge of the cell.
  */
  virtual void setGridVisibility(OdUInt32 row, OdUInt32 col, OdInt16 nEdges,
                                        OdDb::Visibility value);

  /** Description:
      Returns the grid line weight of the specified edge of the cell.
  */
  virtual OdDb::LineWeight gridLineWeight(OdUInt32 row, OdUInt32 col,
                                        OdDb::CellEdgeMask iEdge) const;

  /** Description:
      Sets the grid line weight of the specified edge of the cell.
  */
  virtual void setGridLineWeight(OdUInt32 row, OdUInt32 col, OdInt16 nEdges,
                                        OdDb::LineWeight value);
  
  //********************************************************************
  // Insert and delete methods for columns and rows
  //********************************************************************
  //
  /** Description:
      Inserts the specified number of new columns into the table at the column index col.
  */
  virtual void insertColumns(OdUInt32 col, double width, OdUInt32 nCols = 1);

  /** Description:
      Deletes the specified number of columns from the table at the column index col.
  */
  virtual void deleteColumns(OdUInt32 col, OdUInt32 nCols = 1);

  /** Description:
      Inserts the specified number of new rows into the table at the row index row.
  */
  virtual void insertRows(OdUInt32 row, double height, OdUInt32 nRows = 1);
  
  /** Description:
      Deletes the specified number of rows from the table at the row index row.
  */
  virtual void deleteRows(OdUInt32 row, OdUInt32 nRows = 1);

  //********************************************************************
  // Merge and unmerge methods
  //********************************************************************
  //
  /** Description:
      Merges a rectangular region of cells. The total number of cells merged is 
      ((maxRow - minRow) + 1) * ((maxCol - minCol) + 1).
  */  
  virtual void mergeCells(OdUInt32 minRow, OdUInt32 maxRow,
                                         OdUInt32 minCol, OdUInt32 maxCol);

  /** Description:
      Unmerges a rectangular region of cells. The total number of cells unmerged is 
      ((maxRow - minRow) + 1) * ((maxCol - minCol) + 1).
  */
  virtual void unmergeCells(OdUInt32 minRow, OdUInt32 maxRow,
                                         OdUInt32 minCol, OdUInt32 maxCol);
  /** Description:
      Returns true if the specified cell is within merged cells. It also returns 
      the range of the merged cells if minRow, maxRow, minCol, and maxCol are not null. 
      The merged cell region is always a rectangular area.
  */
  virtual bool isMergedCell(OdUInt32 row, OdUInt32 col, 
                                           OdUInt32* minRow = NULL, 
                                           OdUInt32* maxRow = NULL,
                                           OdUInt32* minCol = NULL, 
                                           OdUInt32* maxCol = NULL) const;
  
  //********************************************************************
  // Fields and Hyperlinks methods
  //********************************************************************
  //
  
  //********************************************************************
  // Methods to generate table layout and table block
  //********************************************************************
  //
  
  virtual OdResult generateLayout();

  /** Description:
    Updates the block table record the table object references to match any changes made 
    to the table object since the last time the block table record was updated.
  */
  virtual OdResult recomputeTableBlock(bool forceUpdate = true);
  //********************************************************************
  // Methods for sub-selection  
  //********************************************************************
  //
  /*
  virtual bool              hitTest(const OdGePoint3d& wpt, 
                                    const OdGeVector3d& wviewVec,
                                    double wxaper,
                                    double wyaper,
                                    int& resultRowIndex, 
                                    int& resultColumnIndex);

  virtual OdResult select(const OdGePoint3d& wpt, 
                                   const OdGeVector3d& wvwVec, 
                                   const OdGeVector3d& wvwxVec, 
                                   double wxaper,
                                   double wyaper,
                                   bool allowOutside,
                                   bool bInPickFirst, 
                                   int& resultRowIndex, 
                                   int& resultColumnIndex,
                                   OdSubentPathArray* pPaths = NULL) const;

  virtual OdResult selectSubRegion(const OdGePoint3d& wpt1, 
                                   const OdGePoint3d& wpt2,
                                   const OdGeVector3d& wvwVec, 
                                   const OdGeVector3d& wvwxVec,
                                   double wxaper,
                                   double wyaper,                             
                                   OdDb::SelectType seltype,
                                   bool bIncludeCurrentSelection,
                                   bool bInPickFirst,                             
                                   int& rowMin,
                                   int& rowMax,
                                   int& colMin,
                                   int& colMax,
                                   OdSubentPathArray* pPaths = NULL) const;

  virtual bool reselectSubRegion(OdSubentPathArray& paths) const;
          
  virtual OdResult getSubSelection(int& rowMin,
                                    int& rowMax,
                                    int& colMin,
                                    int& colMax) const;

  virtual OdResult setSubSelection(int rowMin,
                                    int rowMax,
                                    int colMin,
                                    int colMax);

  virtual void              clearSubSelection();

  virtual bool              hasSubSelection() const;
  */
  //********************************************************************
  // Overridden methods from OdDbObject
  //********************************************************************
  //

  virtual OdResult dwgInFields (OdDbDwgFiler* pFiler);
  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual OdResult dxfInFields (OdDbDxfFiler* pFiler);
  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;

  // virtual OdResult audit(OdDbAuditInfo* pAuditInfo);

  void subClose();
  void subSetDatabaseDefaults(OdDbDatabase *);

  // virtual void              objectClosed(const OdDbObjectId objId);
  // virtual void              erased(const OdDbObject* dbObj,
  //                                  bool pErasing = true);

  virtual void getClassID(void** pClsid) const;

  //********************************************************************
  // Overridden methods from OdDbEntity
  //********************************************************************
  //
  /*
  virtual void              list() const;
  */

  virtual bool    worldDraw(OdGiWorldDraw* worldDraw) const;

  /*
  virtual OdResult getGripPoints(OdGePoint3dArray&, 
                                          OdDbIntArray&,
                                          OdDbIntArray&) const;

  virtual OdResult moveGripPointsAt(const OdDbIntArray&,
                                             const OdGeVector3d&);

  virtual OdResult getStretchPoints(OdGePoint3dArray& stretchPoints)
                                             const;

  virtual OdResult moveStretchPointsAt(const OdDbIntArray& 
                                               indices,
                                               const OdGeVector3d& offset);

  virtual OdResult getOsnapPoints(OdDb::OsnapMode osnapMode,
                                           int   gsSelectionMark,
                                           const OdGePoint3d&  pickPoint,
                                           const OdGePoint3d&  lastPoint,
                                           const OdGeMatrix3d& viewXform,
                                           OdGePoint3dArray&      snapPoints,
                                           OdDbIntArray&   geomIds)
                                           const;

  virtual OdResult transformBy(const OdGeMatrix3d& xform);

  virtual OdResult getTransformedCopy(const OdGeMatrix3d& xform,
                                               OdDbEntity*& ent) const;

  virtual OdResult getGeomExtents(OdDbExtents& extents) const;

  virtual OdResult explode(OdDbVoidPtrArray& entitySet) const;
  */

  //********************************************************************
  // Overridden methods from OdDbBlockReference required for OdDbTable
  //********************************************************************
  //
  //arx OdGePoint3d            position() const;
  // virtual OdResult setPosition(const OdGePoint3d&);

  //arx OdGeVector3d           normal() const;
  // virtual OdResult setNormal(const OdGeVector3d& newVal);

  //********************************************************************
  // Methods for internal use only
  //********************************************************************
  //
  // TODO: Temporary method for navigating after editing cells
  /*
  virtual OdResult select_next_cell(int dir,
                                   int& resultRowIndex, 
                                   int& resultColumnIndex,
                                   OdSubentPathArray* pPaths = NULL,
                                   bool bSupportTextCellOnly = true) const;

  virtual void              setRegen();
  virtual void              suppressInvisibleGrid(bool value);
  virtual OdResult getCellExtents(int row, 
                                       int col,
                                       double& cellWidth,
                                       double& cellHeight,
                                       bool bAdjustForMargins) const;
  */
};

typedef OdSmartPtr<OdDbTable> OdDbTablePtr;

#include "DD_PackPop.h"

#endif

