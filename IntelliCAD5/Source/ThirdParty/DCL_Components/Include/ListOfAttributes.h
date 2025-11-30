/*
* FILENAME:     ListOfAttributes.h- Contains declarations of class CListOfAttributes.
* Copyright  (C) Copyright 1998-99 Visio Corporation.  All Rights Reserved.
*
*
* Abstract 
*       This holds the list of all attributes for a tile.This class is a datamember
*  of Tile class
*
* Audit Log 
*
*
* Audit Trail
*
*
*/


#ifndef   _LISTOFATTRIBUTES_H
#define  _LISTOFATTRIBUTES_H

class Tile;
class TileAttributes;


class ListOfAttributes 
{
private :
	 
	CObArray m_ListAttributes;  // stores pointers to the attributes of a tile
	int m_nCompress;			// flag which the control can be compressed
	int m_IsDefault;			// flag to indicate weather the property is default 
	
public:
	CString GetParentKey();
	Tile * m_pTile;				// stores pointer to the Tile

	BOOL IsRadioGroup();
	
	BOOL get_FixedWidthFont();  // Returns the fixed_width_font value that is set to either TRUE/FALSE  
	
	CString get_initialfocus(); // Returns the value of initial_focus attribute set for the DCL file	

	//////////////////////////////////////////////////////////////////////////////////////
	// the  following 2 functions give the states of the property immediatly after the call to read the property
	int CanCompress();			// function which returns the state of m_nCompress flag
	int IsFromDefault();		// function which returns the state of m_IsDefault flag
	/////////////////////////////////////////////////////////////////////////////////////////
	int get_buttonWidth(int width); // sets the width of the button
	CString get_PasswordChar();		// Returns the value of PasswordChar attribute set for the DCL file	
	
	int FindAttribute(               // Finds an attribute.
		CString& wszAttributeName    // i: Name of attribute to be found.
		);
	int IstabTruncate();			// Returns the value of tabTruncate attribute set for the DCL file	
	int IsMultipleSelect();			// Returns the value of MultipleSelect attribute set for the DCL file	
	int get_editLimit();			// Returns the value of editLimit attribute set for the DCL file	
	int get_editWidth();			// Returns the value of editWidth attribute set for the DCL file	
	int get_Layout();				// Returns the value of Layout attribute set for the DCL file
	CObArray& get_ListAttributes();
	int AddAttribute(             // adds a attribute to the attribute list
		TileAttributes * pAttrib, // i: pointer to attribute to be added
		int nReferencedCount      // i: 0 - for first reference,1 - for multiple references
		);                      
	
	int AddAttribute(         // adds a attribute to the attribute list
		CString name,         // i: attribute name
		CString type ,        // i: attribute type
		CString value         // i: attribute value
		);
	
    int get_AuditLevel();               // Returns the audit level set.
	int get_Alignment ();				// Returns the Alignment Attribute of the tile from the attributes list.
	int IsFixedWidth();					// Returns the IsFixedWidth Attribute of the tile from the attributes list.
	int IsFixedHeight ();				// Returns the IsFixedHeight Attribute of the tile from the attributes list.
	int get_Height ();					// Returns the Height Attribute of the tile from the attributes list.
	int get_Width ();					// Returns the Width Attribute of the tile from the attributes list.
	CString get_Label ();				// Returns the Label Attribute of the tile from the attributes list.
	int get_ValueInt ();				// Returns the value Attribute of the tile from the attributes list.
	CString get_ValueString ();			// Returns the value Attribute of the tile ,the return type is CString object from the attributes list.
	CString get_Key ();					// Returns the Key Attribute of the tile from the attributes list.
	CString get_Action ();				// Returns the name of the callback function from the attributes list.
	CString get_Mnemonic ();			// Returns the mnemonic Attribute of the tile from the attributes list.
	int IsTabstop ();					// Returns the IsTabstop Attribute of the tile from the attributes list.
	int IsEnabled ();					// Returns the IsEnabled Attribute of the tile from the attributes list.
	int IsCanceled ();					// Returns the IsCanceled Attribute of the tile from the attributes list.
	int IsDefault ();					// Returns the IsDefault Attribute of the tile from the attributes list.
	BOOL IsAllowAccept ();				// Returns the IsAllowAccept Attribute of the tile from the attributes list.
	CString GetTabsList ();				// Returns the GetTabsList Attribute of the tile from the attributes list.
	int IsBold ();						// Returns the IsBold Attribute of the tile from the attributes list.
	int IsErrorTile ();					// Returns the IsErrorTile Attribute of the tile from the attributes list.
	int get_BigIncrement ();			// Returns the get_BigIncrement Attribute of the tile from the attributes list.
	int get_SmallIncrement ();			// Returns the get_SmallIncrement Attribute of the tile from the attributes list.
	int get_SliderLayOut ();			// Returns the get_SliderLayOut Attribute of the tile from the attributes list.
	int get_MaxValue ();				// Returns the get_MaxValue Attribute of the tile from the attributes list.
	int get_MinValue ();				// Returns the get_MinValue Attribute of the tile from the attributes list.
	int get_ChildrenAlignment ();		// Returns the get_ChildrenAlignment Attribute of the tile from the attributes list.
	int IsChildFixedWidth ();			// Returns the IsChildFixedWidth Attribute of the tile from the attributes list.
	int IsChildFixedHeight ();			// Returns the IsChildFixedHeight Attribute of the tile from the attributes list.
	CString get_List();					// Returns the List Attribute of the tile from the attributes list.
	CString get_Color();                // Returns the currently set color for an image or image button.
	ListOfAttributes(); 
	ListOfAttributes(				     // constructor of ListOfAttributes
		Tile * ptile					//  i:pointer to the tile object
		);
	virtual ~ListOfAttributes();        // destructor to ListOfAttributes

	friend class AuditLevelProcessor;

//	friend class Tile;
    friend class TreeDebugger; 
	friend class CDCLView;
	friend class CDCLDoc;

	

protected:
	int pixelY (						// Converts the Dialog Units to piXel in Y Direction
		double height					// Height to convert
		);
	int pixelX (						// Converts the Dialog Units to piXel in X Direction
		double width					// width to convert
		);			
};

#endif 
