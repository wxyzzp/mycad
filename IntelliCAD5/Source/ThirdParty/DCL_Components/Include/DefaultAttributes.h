/** FILENAME:     DefaultAttributes.h- Contains declarations of class CDefaultAttributes.
* Copyright  (C) Copyright 1998-99  Visio Corporation.  All Rights Reserved.
*
*
*
* Abstract 
*	this class is used to store the default attributes of basic tiles 
*
*
*Audit Log 
*
*
*Audit Trail
*
*
*/

#if !defined _DEFAULTATTRIBUTES_H
#define _DEFAULTATTRIBUTES_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TileAttributes.h"

class DefaultAttributes  
{
private:
	CObArray ColumnAttribs;		// default attributes for boxed_column, column, boxed_radio_column, radio_column
	CObArray RowAttribs;		// default attributes for boxed_row, row, boxed_radio_row, radio_row
	CObArray buttonAttrib;		// default attributes for button
	CObArray dialogAttrib;		// default attributes for dialog
	CObArray editAttrib;		// default attributes for editbox
	CObArray popupListAttrib;	// default attributes for popupListbox
	CObArray ListAttrib;		// default attributes for Listbox
	CObArray RadioAttrib;		// default attributes for RadioButton and toggle
	CObArray SliderAttrib;		// default attributes for Slidercontrol
	CObArray SpacerAttrib;		// default attributes for Spacer
	CObArray textAttrib;		// default attributes for text
	CObArray text_partAttrib;	// default attributes for text_part
	CObArray paragraph ;		// default attributes for paragraph
	CObArray concatenation;		// default attributes for paragraph
	CObArray imagebutton;       // default attributes for image/image_button 
public:

	int Getvalue(		// gets the default attributes of the tile from array set as frist parameter
		CObArray& array,// i:Array of default attributes
		CString vName,  // i:Name of attribute
		CString& value, // o:value of attribute
		CString& type   // o:data type of attribute
		);

	int GetDefaultValue(	// gets the default attributes of the tile 
		CString ControlName,// i:Name of the Tile
		CString ValueName,	// i:Name of attribute
		int & ivalue		// o:value of attribute
		);

	int GetDefaultValue(	// gets the default attributes of the tile 
		CString ControlName,// i:Name of the Tile
		CString ValueName,	// i:Name of attribute
		double & dvalue		// o:value of attribute
		);

	int GetDefaultValue(	// gets the default attributes of the tile 
		CString ControlName,// i:Name of the Tile
		CString ValueName,	// i:Name of attribute
		CString & nvalue	// o:value of attribute
		);

	int Exit();				// this function is called to cleanup

	int Init();				// fills the default attributes arrays

	DefaultAttributes();   //  constructor

	virtual ~DefaultAttributes();// destructor

};

#endif 
