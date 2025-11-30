/** FILENAME:     DefaultAttributes.cpp- Contains implementation of class CDefaultAttributes.
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

#include "stdafx.h"
#include "DefaultAttributes.h"
#include "ExceptionManager.h"
#include "sds.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
/************************************************************************************
/* + DefaultAttributes::DefaultAttributes - 
*
*	constructor
*
*
*
*/
DefaultAttributes::DefaultAttributes()
	{

	}

/************************************************************************************
/* + DefaultAttributes::~DefaultAttributes - 
*
*
*	destructor
*
*
*/
DefaultAttributes::~DefaultAttributes()
	{

	}

/************************************************************************************
/* + int DefaultAttributes::Init - 
*
*	fills the default attributes arrays
*
*
*
*/

int DefaultAttributes::Init()
	{
	try
		{

		// default attributes for boxed_column, column, boxed_radio_column, radio_column
		ColumnAttribs.Add (new TileAttributes(_T("alignment"),_T("s"),_T("left")));
		ColumnAttribs.Add (new TileAttributes(_T("fixed_height"),_T("s"),_T("false")));
		ColumnAttribs.Add (new TileAttributes(_T("fixed_width"),_T("s"),_T("false")));
		ColumnAttribs.Add (new TileAttributes(_T("height"),_T("f"),_T("1.8")));
		ColumnAttribs.Add (new TileAttributes(_T("width"),_T("f"),_T("3.3")));
		ColumnAttribs.Add (new TileAttributes(_T("layout"),_T("s"),_T("vertical")));
		
		// default attributes for boxed_row, row, boxed_radio_row, radio_row
		RowAttribs.Add (new TileAttributes(_T("alignment"),_T("s"),_T("left")));
		RowAttribs.Add (new TileAttributes(_T("fixed_height"),_T("s"),_T("false")));
		RowAttribs.Add (new TileAttributes(_T("fixed_width"),_T("s"),_T("false")));
		RowAttribs.Add (new TileAttributes(_T("height"),_T("f"),_T("1.8")));
		RowAttribs.Add (new TileAttributes(_T("width"),_T("f"),_T("3.3")));
		RowAttribs.Add (new TileAttributes(_T("layout"),_T("s"),_T("horizontal")));
		
		paragraph.Add (new TileAttributes(_T("alignment"),_T("s"),_T("left")));
		paragraph.Add (new TileAttributes(_T("fixed_height"),_T("s"),_T("True")));
		paragraph.Add (new TileAttributes(_T("fixed_width"),_T("s"),_T("True")));
		paragraph.Add (new TileAttributes(_T("height"),_T("f"),_T("1")));
		paragraph.Add (new TileAttributes(_T("width"),_T("f"),_T("1")));
		paragraph.Add (new TileAttributes(_T("layout"),_T("s"),_T("vertical")));

		concatenation.Add (new TileAttributes(_T("alignment"),_T("s"),_T("left")));
		concatenation.Add (new TileAttributes(_T("fixed_height"),_T("s"),_T("True")));
		concatenation.Add (new TileAttributes(_T("fixed_width"),_T("s"),_T("True")));
		concatenation.Add (new TileAttributes(_T("height"),_T("f"),_T("1")));
		concatenation.Add (new TileAttributes(_T("width"),_T("f"),_T("1")));
		concatenation.Add (new TileAttributes(_T("layout"),_T("s"),_T("horizontal")));


		// default attributes for button
		buttonAttrib.Add (new TileAttributes(_T("alignment"),_T("s"),_T("left")));
		buttonAttrib.Add (new TileAttributes(_T("fixed_height"),_T("s"),_T("true")));
		buttonAttrib.Add (new TileAttributes(_T("fixed_width"),_T("s"),_T("false")));
		buttonAttrib.Add (new TileAttributes(_T("height"),_T("f"),_T("1.7")));
		buttonAttrib.Add (new TileAttributes(_T("width"),_T("f"),_T("5.5")));
		buttonAttrib.Add (new TileAttributes(_T("is_cancel"),_T("s"),_T("false")));
		buttonAttrib.Add (new TileAttributes(_T("is_default"),_T("s"),_T("false")));
		buttonAttrib.Add (new TileAttributes(_T("is_tab_stop"),_T("s"),_T("true")));
		buttonAttrib.Add (new TileAttributes(_T("is_enabled"),_T("s"),_T("true")));
		//buttonAttrib.Add (new TileAttributes(_T("is_enabled"),_T("s"),_T("false")));

		// default attributes for dialog
		dialogAttrib.Add (new TileAttributes(_T("children_alignment"),_T("s"),_T("left")));
		dialogAttrib.Add (new TileAttributes(_T("alignment"),_T("s"),_T("centered")));
		dialogAttrib.Add (new TileAttributes(_T("width"),_T("f"),_T("5.5")));
		dialogAttrib.Add (new TileAttributes(_T("is_enabled"),_T("s"),_T("true")));

		// default attributes for editbox
		editAttrib.Add (new TileAttributes(_T("alignment"),_T("s"),_T("left")));
		editAttrib.Add (new TileAttributes(_T("fixed_height"),_T("s"),_T("true")));
		editAttrib.Add (new TileAttributes(_T("fixed_width"),_T("s"),_T("false")));
		editAttrib.Add (new TileAttributes(_T("height"),_T("f"),_T("1")));
		editAttrib.Add (new TileAttributes(_T("width"),_T("f"),_T("9")));
		editAttrib.Add (new TileAttributes(_T("is_tab_stop"),_T("s"),_T("true")));
		editAttrib.Add (new TileAttributes(_T("is_enabled"),_T("s"),_T("true")));
		editAttrib.Add (new TileAttributes(_T("allow_accept"),_T("s"),_T("false")));
		editAttrib.Add (new TileAttributes(_T("edit_limit"),_T("i"),_T("132")));
		
		// default attributes for popupListbox
		popupListAttrib.Add (new TileAttributes(_T("alignment"),_T("s"),_T("left")));
		popupListAttrib.Add (new TileAttributes(_T("fixed_height"),_T("s"),_T("true")));
		popupListAttrib.Add (new TileAttributes(_T("fixed_width"),_T("s"),_T("false")));
		popupListAttrib.Add (new TileAttributes(_T("height"),_T("f"),_T("1.8")));
		popupListAttrib.Add (new TileAttributes(_T("width"),_T("f"),_T("9")));
		popupListAttrib.Add (new TileAttributes(_T("is_tab_stop"),_T("s"),_T("true")));
		popupListAttrib.Add (new TileAttributes(_T("is_enabled"),_T("s"),_T("true")));
		popupListAttrib.Add (new TileAttributes(_T("allow_accept"),_T("s"),_T("false")));

		// default attributes for Listbox
		ListAttrib.Add (new TileAttributes(_T("alignment"),_T("s"),_T("left")));
		ListAttrib.Add (new TileAttributes(_T("fixed_height"),_T("s"),_T("false")));
		ListAttrib.Add (new TileAttributes(_T("fixed_width"),_T("s"),_T("false")));
		ListAttrib.Add (new TileAttributes(_T("is_tab_stop"),_T("s"),_T("true")));
		ListAttrib.Add (new TileAttributes(_T("is_enabled"),_T("s"),_T("true")));
		ListAttrib.Add (new TileAttributes(_T("width"),_T("f"),_T("4")));
		ListAttrib.Add (new TileAttributes(_T("height"),_T("f"),_T("5")));
		ListAttrib.Add (new TileAttributes(_T("multiple_select"),_T("s"),_T("false")));
//		ListAttrib.Add (new TileAttributes(_T("value"),_T("i"),_T("0")));
		
		// default attributes for RadioButton and toggle
		RadioAttrib.Add (new TileAttributes(_T("alignment"),_T("s"),_T("left")));
		RadioAttrib.Add (new TileAttributes(_T("fixed_height"),_T("s"),_T("true")));
		RadioAttrib.Add (new TileAttributes(_T("fixed_width"),_T("s"),_T("false")));
		RadioAttrib.Add (new TileAttributes(_T("height"),_T("f"),_T("1.3")));
		RadioAttrib.Add (new TileAttributes(_T("width"),_T("f"),_T("5")));
		RadioAttrib.Add (new TileAttributes(_T("is_tab_stop"),_T("s"),_T("true")));
		RadioAttrib.Add (new TileAttributes(_T("is_enabled"),_T("s"),_T("true")));
		RadioAttrib.Add (new TileAttributes(_T("value"),_T("i"),_T("0")));

		// default attributes for Slidercontrol
		SliderAttrib.Add (new TileAttributes(_T("alignment"),_T("s"),_T("left")));
		SliderAttrib.Add (new TileAttributes(_T("fixed_height"),_T("s"),_T("false")));
		SliderAttrib.Add (new TileAttributes(_T("fixed_width"),_T("s"),_T("false")));
		SliderAttrib.Add (new TileAttributes(_T("height"),_T("f"),_T("1.25")));
		SliderAttrib.Add (new TileAttributes(_T("width"),_T("f"),_T("12.5")));
		SliderAttrib.Add (new TileAttributes(_T("is_tab_stop"),_T("s"),_T("true")));
		SliderAttrib.Add (new TileAttributes(_T("is_enabled"),_T("s"),_T("true")));
		SliderAttrib.Add (new TileAttributes(_T("max_value"),_T("i"),_T("10000")));
		SliderAttrib.Add (new TileAttributes(_T("min_value"),_T("i"),_T("0")));
		SliderAttrib.Add (new TileAttributes(_T("layout"),_T("s"),_T("horizontal")));


		// default attributes for Spacer
		SpacerAttrib.Add (new TileAttributes(_T("height"),_T("f"),_T("1")));
		SpacerAttrib.Add (new TileAttributes(_T("width"),_T("f"),_T("2.0")));
		SpacerAttrib.Add (new TileAttributes(_T("fixed_height"),_T("s"),_T("true")));
		SpacerAttrib.Add (new TileAttributes(_T("fixed_width"),_T("s"),_T("true")));

		// default attributes for text
		textAttrib.Add (new TileAttributes(_T("alignment"),_T("s"),_T("left")));
		textAttrib.Add (new TileAttributes(_T("fixed_height"),_T("s"),_T("true")));
		textAttrib.Add (new TileAttributes(_T("fixed_width"),_T("s"),_T("false")));
		textAttrib.Add (new TileAttributes(_T("height"),_T("f"),_T("1")));
		textAttrib.Add (new TileAttributes(_T("width"),_T("f"),_T("4")));
		textAttrib.Add (new TileAttributes(_T("is_tab_stop"),_T("s"),_T("true")));
		textAttrib.Add (new TileAttributes(_T("is_enabled"),_T("s"),_T("true")));
		textAttrib.Add (new TileAttributes(_T("is_bold"),_T("s"),_T("false")));

		// default attributes for text_part
		text_partAttrib.Add (new TileAttributes(_T("alignment"),_T("s"),_T("left")));
		text_partAttrib.Add (new TileAttributes(_T("fixed_height"),_T("s"),_T("true")));
		text_partAttrib.Add (new TileAttributes(_T("fixed_width"),_T("s"),_T("true")));
		text_partAttrib.Add (new TileAttributes(_T("height"),_T("f"),_T("1")));
		text_partAttrib.Add (new TileAttributes(_T("width"),_T("f"),_T("3.5")));
		text_partAttrib.Add (new TileAttributes(_T("is_tab_stop"),_T("s"),_T("true")));
		text_partAttrib.Add (new TileAttributes(_T("is_enabled"),_T("s"),_T("true")));
		text_partAttrib.Add (new TileAttributes(_T("is_bold"),_T("s"),_T("false")));

		// default attributes for image_button/image
		imagebutton.Add (new TileAttributes(_T("alignment"),_T("s"),_T("left")));
		imagebutton.Add (new TileAttributes(_T("fixed_height"),_T("s"),_T("true")));
		imagebutton.Add (new TileAttributes(_T("fixed_width"),_T("s"),_T("false")));
		imagebutton.Add (new TileAttributes(_T("height"),_T("f"),_T("1.7")));
		imagebutton.Add (new TileAttributes(_T("width"),_T("f"),_T("5.5")));
		imagebutton.Add (new TileAttributes(_T("is_cancel"),_T("s"),_T("false")));
		imagebutton.Add (new TileAttributes(_T("is_default"),_T("s"),_T("false")));
		imagebutton.Add (new TileAttributes(_T("is_tab_stop"),_T("s"),_T("true")));
		imagebutton.Add (new TileAttributes(_T("is_enabled"),_T("s"),_T("true")));

		}
	catch(...)
		{
			ThrowCExceptionManager(MEM_ALLOC_FAILED);
		}
	return SDS_RTNORM;


	}	

/************************************************************************************
/* + int DefaultAttributes::Exit - 
*
*	This function is called to cleanup (to delete the Arrays created in init)
*
*
*
*/

int DefaultAttributes::Exit()
	{
	int index,size;
	// cleanup the default Attributes Array

	// Columns cleanup.
	size=ColumnAttribs.GetSize();
	for (index=0; index < size; index++)
		{
		delete (ColumnAttribs[index]);
		}
	ColumnAttribs.RemoveAll();

	// Normal Rows cleanup
	size=RowAttribs.GetSize();
	for (index=0; index < size; index++)
		{
		delete (RowAttribs[index]);
		}
	RowAttribs.RemoveAll();

	//paragraph
	size=paragraph.GetSize();
	for (index=0; index < size; index++)
		{
		delete (paragraph[index]);
		}
	paragraph.RemoveAll();
	
	//concatenation
	size=concatenation.GetSize();
	for (index=0; index < size; index++)
		{
		delete (concatenation[index]);
		}
	concatenation.RemoveAll();

	// button cleanup
	size=buttonAttrib.GetSize();
	for (index=0; index < size; index++)
		{
		delete (buttonAttrib[index]);
		}
	buttonAttrib.RemoveAll();

	// dialog cleanup
	size=dialogAttrib.GetSize();
	for (index=0; index < size; index++)
		{
		delete (dialogAttrib[index]);
		}
	dialogAttrib.RemoveAll();

	// edit cleanup
	size=editAttrib.GetSize();
	for (index=0; index < size; index++)
		{
		delete (editAttrib[index]);
		}
	editAttrib.RemoveAll();
	
	// popupList cleanup
	size=popupListAttrib.GetSize();
	for (index=0; index < size; index++)
		{
		delete (popupListAttrib[index]);
		}
	popupListAttrib.RemoveAll();

	// List cleanup
	size=ListAttrib.GetSize();
	for (index=0; index < size; index++)
		{
		delete (ListAttrib[index]);
		}
	ListAttrib.RemoveAll();

	// Slider cleanup
	size=SliderAttrib.GetSize();
	for (index=0; index < size; index++)
		{
		delete (SliderAttrib[index]);
		}
	SliderAttrib.RemoveAll();

	// Radio cleanup
	size=RadioAttrib.GetSize();
	for (index=0; index < size; index++)
		{
		delete (RadioAttrib[index]);
		}
	RadioAttrib.RemoveAll();

	// Spacer cleanup
	size=SpacerAttrib.GetSize();
	for (index=0; index < size; index++)
		{
		delete (SpacerAttrib[index]);
		}
	SpacerAttrib.RemoveAll();

	// text cleanup
	size=textAttrib.GetSize();
	for (index=0; index < size; index++)
		{
		delete (textAttrib[index]);
		}
	textAttrib.RemoveAll();

	// text_part cleanup
	size=text_partAttrib.GetSize();
	for (index=0; index < size; index++)
		{
		delete (text_partAttrib[index]);
		}
	text_partAttrib.RemoveAll();

	// text_part cleanup
	size=imagebutton.GetSize();
	for (index=0; index < size; index++)
		{
		delete (imagebutton[index]);
		}
	imagebutton.RemoveAll();

	return SDS_RTNORM;

	}

/************************************************************************************
/* + int DefaultAttributes::GetDefaultValue(CString ControlName, CString ValueName, int& ivalu - 
*	gets the default attributes of the tile 
*	In this function depending upon the type of tile the proper array is selected
*	and the function Getvalue is called which gets the value from the array
*	
*
*/

int DefaultAttributes::GetDefaultValue(
									   CString ControlName, // i:Name of the Tile
									   CString ValueName,   // i:Name of attribute
									   int& ivalue			// o:value of attribute
									   )
	{
	CString type ,nvalue;
	int error;
	
	if (ControlName== _T("boxed_column") || ControlName== _T("column") || ControlName== _T("boxed_radio_column") || ControlName== _T("radio_column"))
		{
		error = (Getvalue(ColumnAttribs,ValueName,nvalue,type));
		}

	else if(ControlName== _T("boxed_row") || ControlName== _T("row") || ControlName== _T("boxed_radio_row") || ControlName== _T("radio_row") || ControlName== _T("") )
		{
		error = (Getvalue(RowAttribs, ValueName, nvalue, type));
		}

	 else if(ControlName== _T("paragraph"))
		{
		 error = (Getvalue(paragraph,ValueName,nvalue,type));
		}

	 else if(ControlName== _T("concatenation"))
		{
		 error = (Getvalue(concatenation,ValueName,nvalue,type));
		}

	else if(ControlName== _T("button"))
		{
		error = (Getvalue(buttonAttrib,ValueName,nvalue,type));
		}
	else if(ControlName== _T("dialog"))
		{
		error = (Getvalue(dialogAttrib,ValueName,nvalue,type));
		}
	else if(ControlName== _T("edit_box"))
		{
		error = (Getvalue(editAttrib,ValueName,nvalue,type));
		}
	else if(ControlName== _T("popup_list"))
		{
		error = (Getvalue(popupListAttrib,ValueName,nvalue,type));
		}
	else if(ControlName== _T("list_box"))
		{
		error = (Getvalue(ListAttrib,ValueName,nvalue,type));
		}
	else if(ControlName== _T("radio_button") || ControlName== _T("toggle"))
		{
		error = (Getvalue(RadioAttrib,ValueName,nvalue,type));
		}
	else if(ControlName== _T("slider"))
		{
		error = (Getvalue(SliderAttrib,ValueName,nvalue,type));
		}
	else if(ControlName== _T("spacer") || ControlName== _T("spacer_0") || ControlName== _T("spacer_1"))
		{
		error = (Getvalue(SpacerAttrib,ValueName,nvalue,type));
		}
	else if(ControlName== _T("text"))
		{
		error = (Getvalue(textAttrib,ValueName,nvalue,type));
		}
	else if(ControlName== _T("text_part"))
		{
		error = (Getvalue(text_partAttrib,ValueName,nvalue,type));
		}
	else if(ControlName== _T("image_button") || ControlName== _T("image"))
		{
		error =  (Getvalue(imagebutton,ValueName,nvalue,type));
		}
	if(error == SDS_RTERROR)
		return SDS_RTERROR;
	else
		{
		if(type == _T("n"))
			{
			ivalue = atoi(nvalue);
			return SDS_RTNORM;
			}
		else
			return SDS_RTERROR;

		}
	}
/************************************************************************************
/* + int DefaultAttributes::GetDefaultValue(CString ControlName, CString ValueName, double& dvalu - 
*	gets the default attributes of the tile 
*	In this function depending upon the type of tile the proper array is selected
*	and the function Getvalue is called which gets the value from the array
*	
*	
*
*/

int DefaultAttributes::GetDefaultValue(
									   CString ControlName, // i:Name of the Tile
									   CString ValueName,   // i:Name of attribute
									   double& dvalue		// o:value of attribute
									   )
	{
	CString type ,nvalue;
	int error;

		if (ControlName== _T("boxed_column") || ControlName== _T("column") || ControlName== _T("boxed_radio_column") || ControlName== _T("radio_column"))
		{
		error = (Getvalue(ColumnAttribs,ValueName,nvalue,type));
		}

	else if(ControlName== _T("boxed_row") || ControlName== _T("row") || ControlName== _T("boxed_radio_row") || ControlName== _T("radio_row") || ControlName== _T("") )
		{
		error = (Getvalue(RowAttribs, ValueName, nvalue, type));
		}

	else if(ControlName== _T("paragraph"))
		{
		 error = (Getvalue(paragraph,ValueName,nvalue,type));
		}

	 else if(ControlName== _T("concatenation"))
		{
		 error = (Getvalue(concatenation,ValueName,nvalue,type));
		}

	else if(ControlName== _T("button"))
		{
		error = (Getvalue(buttonAttrib,ValueName,nvalue,type));
		}
	else if(ControlName== _T("dialog"))
		{
		error = (Getvalue(dialogAttrib,ValueName,nvalue,type));
		}
	else if(ControlName== _T("edit_box"))
		{
		error = (Getvalue(editAttrib,ValueName,nvalue,type));
		}
	else if(ControlName== _T("popup_list"))
		{
		error = (Getvalue(popupListAttrib,ValueName,nvalue,type));
		}
	else if(ControlName== _T("list_box"))
		{
		error = (Getvalue(ListAttrib,ValueName,nvalue,type));
		}
	else if(ControlName== _T("radio_button") || ControlName== _T("toggle"))
		{
		error = (Getvalue(RadioAttrib,ValueName,nvalue,type));
		}
	else if(ControlName== _T("slider"))
		{
		error = (Getvalue(SliderAttrib,ValueName,nvalue,type));
		}
	else if(ControlName== _T("spacer") || ControlName== _T("spacer_0") || ControlName== _T("spacer_1"))
		{
		error = (Getvalue(SpacerAttrib,ValueName,nvalue,type));
		}
	else if(ControlName== _T("text"))
		{
		error = (Getvalue(textAttrib,ValueName,nvalue,type));
		}
	else if(ControlName== _T("text_part"))
		{
		error = (Getvalue(text_partAttrib,ValueName,nvalue,type));
		}
	else if(ControlName== _T("image_button") || ControlName== _T("image"))
		{
		error = (Getvalue(imagebutton,ValueName,nvalue,type));
		}
	if(error == SDS_RTERROR)
		return SDS_RTERROR;
	else
		{
		if(type == _T("f"))
			{
			dvalue = atof(nvalue);
			return SDS_RTNORM;
			}
		else
			return SDS_RTERROR;

		}
	}
/************************************************************************************
/* + int DefaultAttributes::GetDefaultValue(CString ControlName, CString ValueName, CString& nvalu - 
*	gets the default attributes of the tile 
*	In this function depending upon the type of tile the proper array is selected
*	and the function Getvalue is called which gets the value from the array
*	
*	
*
*/

int DefaultAttributes::GetDefaultValue(
									   CString ControlName,		// i:Name of the Tile
									   CString ValueName,		// i:Name of attribute
									   CString& nvalue			// o:value of attribute
									   )
	{
	CString type;
	
		if (ControlName== _T("boxed_column") || ControlName== _T("column") || ControlName== _T("boxed_radio_column") || ControlName== _T("radio_column"))
		{
		return (Getvalue(ColumnAttribs,ValueName,nvalue,type));
		}

	else if(ControlName== _T("boxed_row") || ControlName== _T("row") || ControlName== _T("boxed_radio_row") || ControlName== _T("radio_row") || ControlName== _T("") )
		{
		return (Getvalue(RowAttribs, ValueName, nvalue, type));
		}

	else if(ControlName== _T("paragraph"))
		{
		 return(Getvalue(paragraph,ValueName,nvalue,type));
		}
	 else if(ControlName== _T("concatenation"))
		{
		 return(Getvalue(concatenation,ValueName,nvalue,type));
		}
	else if(ControlName== _T("button"))
		{
		return (Getvalue(buttonAttrib,ValueName,nvalue,type));
		}
	else if(ControlName== _T("dialog"))
		{
		return (Getvalue(dialogAttrib,ValueName,nvalue,type));
		}
	else if(ControlName== _T("edit_box"))
		{
		return (Getvalue(editAttrib,ValueName,nvalue,type));
		}
	else if(ControlName== _T("popup_list"))
		{
		return (Getvalue(popupListAttrib,ValueName,nvalue,type));
		}
	else if(ControlName== _T("list_box"))
		{
		return (Getvalue(ListAttrib,ValueName,nvalue,type));
		}
	else if(ControlName== _T("radio_button") || ControlName== _T("toggle"))
		{
		return (Getvalue(RadioAttrib,ValueName,nvalue,type));
		}
	else if(ControlName== _T("slider"))
		{
		return (Getvalue(SliderAttrib,ValueName,nvalue,type));
		}
	else if(ControlName== _T("spacer") || ControlName== _T("spacer_0") || ControlName== _T("spacer_1"))
		{
		return (Getvalue(SpacerAttrib,ValueName,nvalue,type));
		}
	else if(ControlName== _T("text"))
		{
		return (Getvalue(textAttrib,ValueName,nvalue,type));
		}
	else if(ControlName== _T("text_part"))
		{
		return (Getvalue(text_partAttrib,ValueName,nvalue,type));
		}
	else if(ControlName== _T("image_button") || ControlName== _T("image"))
		{
		return (Getvalue(imagebutton,ValueName,nvalue,type));
		}
	return SDS_RTERROR;
	}
/************************************************************************************
/* + int DefaultAttributes::Getvalue(CObArray& array, CString vName, CString& value,CString& typ - 
*	gets the default attributes of the tile from array set as frist parameter
*	
*	
*	
*	
*/

int DefaultAttributes::Getvalue(
								CObArray& array,	// i:Array of default attributes
								CString vName,		// i:Name of attribute
								CString& value,		// o:value of attribute
								CString& type		// o:data type of attribute
								)
	{
	
	for ( int index = 0; index < array.GetSize(); index++ )
		{
		TileAttributes * pTileAttrib = (TileAttributes *)array[index];
		if ( pTileAttrib->get_AttributeName() == vName )
			{
			value = pTileAttrib->get_AttributeValue();
			type  = pTileAttrib->get_AttributeType();
			return SDS_RTNORM;
			}
		
		}
	return SDS_RTERROR;

	}
