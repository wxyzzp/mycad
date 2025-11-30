/** FILENAME:     Constants.h- Contains declarations of class CConstants.
* Copyright  (C) Copyright 1998  Visio Corporation.  All Rights Reserved.
*			This file contains all the constants.
*
*
* Abstract 
*
*
*
*Audit Log 
*
*
*Audit Trail
*
*
*/


//Resource types
#define PRIMITIVES_DCL 0
#define IBASE_DCL      1

//ATTRIBUTE KEYWORD VALUES
#define LEFT_ATTRIB_VALUE "left"
#define RIGHT_ATTRIB_VALUE "right"
#define CENTERED_ATTRIB_VALUE "centered"

#define TOP_ATTRIB_VALUE "top"
#define BOTTOM_ATTRIB_VALUE "bottom"


#define TRUE_ATTRIB_VALUE  "true"
#define FALSE_ATTRIB_VALUE "false"

#define VERTICAL_ATTRIB_VALUE  "vertical"
#define HORIZONTAL_ATTRIB_VALUE "horizontal"


// AUDIT LEVELS
#define AUDIT_LEVEL_0 0
#define AUDIT_LEVEL_1 1
#define AUDIT_LEVEL_2 2
#define AUDIT_LEVEL_3 3

#define ERROR_LOG  "Error.log"
#define TEST_OUT   "test.out"

// Datatypes
#define INT_DATA_TYPE    1
#define REAL_DATA_TYPE   2
#define STRING_DATA_TYPE 3
#define KEYWORD_DATA_TYPE 4


//used while parsing
#define NODE_EXISTS 6001



// Aligment
#define		ALIGN_DEFAULT		10
#define		ALIGN_LEFT			0
#define		ALIGN_RIGHT			1
#define		ALIGN_CENTERED		2
#define		ALIGN_TOP			3
#define		ALIGN_BOTTOM		4
#define		ALIGN_HORIZONTAL	5
#define		ALIGN_VERTICAL		6

#define		LAYOUT_HORIZONTAL	5
#define		LAYOUT_VERTICAL		6

// BUTTON TYPE

# define PUSHBUTTON		0
# define RADIOBUTTON	1
# define CHECKBOX		2


// IMAGE and IMAGEBUTTON
#define IMAGE_TILE 0
#define IMAGEBUTTON_TILE 1

// Color
#define  DIALOG_LINE_STR				"dialog_line"
#define  DIALOG_FOREGROUND_STR          "dialog_foreground"
#define  DIALOG_BACKGROUND_STR			"dialog_background"
#define  GRAPHICS_BACKGROUND_STR		"graphics_background"
#define  BLACK_STR						"black"
#define  RED_STR						"red"
#define  YELLOW_STR						"yellow"	
#define  GREEN_STR						"green"
#define  CYAN_STR						"cyan"
#define  BLUE_STR						"blue"	
#define  MAGENTA_STR					"magenta"
#define  WHITE_STR						"white"
#define  GRAPHICS_FOREGROUND_STR        "graphics_foreground" 



// 
#define		WIDTHOFCHAR		8
#define		VERTICALSPACE	10

//NODE TYPE
#define NODETYPE_DIALOG 0

// Inactive Tile

#define PROW				1
#define PCOLUMN			0
#define BOXED			1
#define UNBOXED			0
#define RADIO			1
#define NOTRADIO		0

// 
#define PTEXT			0
#define PTEXTPART		1
#define PERRTILE		2
#define PSPACER			0
#define PSPACER0		1
#define PSPACER1		2

//These are used for the column's tile positioning.
#define FIRSTTILE	1
#define MIDDLETILE  2
#define LASTTILE    3

// For boxed entities add horizontal margin to the width and vertical margin to height.
#define SPACESX 8					// Space in x direction.
#define SPACESXROWS 6				// Spaces between children of a row.
#define SPACESY 3					// space from the top of dialog to first tile.
#define SPACESYTILES 6				// space between tiles on a dialog.
#define SPACESYOK 2				// space between the tile last tile and ok.
#define SPACESYFIRSTNONDLGCHILD 18
#define SPACESYOTHERNONDIALOG 3
#define DLGEXTRAWIDTH   5
#define DLGEXTRAHEIGHT  32			// extra height to be given to tha dialog.
#define DLGMINWIDTH 92
#define DLGMINHEIGHT 40
#define NL 0
#define NT 0
#define NR 0
#define NB 2
#define STARTX 0
#define STARTY 0
// Spaces for tiles within a column.
#define SPACESYCOLUMN 4
//Spaces for tiles within a boxed_row.
#define SPACESXBOXROW 8
#define SPACESYBOXROWTOP 21
#define SPACESYBOXROWBOTTOM 3
#define SPACESXBOXROWRIGHT 4

//Spaces for tiles within a boxed_radio_row.
#define SPACESXBOXRADIOROW 6
#define SPACESYBOXRADIOROWTOP 20
#define SPACESYBOXRADIOROWBOTTOM 4

//Spaces for tiles within a boxed_column.
#define SPACESXBOXCOL 10
#define SPACESYBOXCOLTOP 21
#define SPACESYBOXCOLBOTTOM 4
#define SPACESYBOXCOLBETWEEN 3
#define SPACESXBOXCOLUMNRIGHT 0
#define OFFSET2 3
#define OFFSET5 0
#define OFFSET1 0
#define OFFSET6 6
#define OFFSET3 3
#define OFFSET4 4
#define OFFSET8 8

//Spaces for tiles within a boxed_radio_column.
#define SPACESXBOXRADIOCOL 4
#define SPACESYBOXRADIOCOLTOP 20
#define SPACESYBOXRADIOCOLBOTTOM 3
#define SPACESYBOXRADIOCOLBETWEEN 6

#define SPACESYRADIOCOLUMN 8
#define SPACESYSUBASSEMBLY 0
#define SPACESYSLIDER 1
#define SPACESYPARAGRAPH 0
#define SPACESXPARAGRAPH 0
#define SPACESXCONCAT 0
#define SPACERX 0
// ListBox constants.
#define NOITEM		-1
#define CHANGE		1
#define APPEND		2
#define REPLACE		3

//Array Sizes.
#define DIALOGADDRESS 25
#define SUBDIALOGADDRESS 10

#define CLUSTER_T 1
#define RADIO_CLUSTER_T 2
