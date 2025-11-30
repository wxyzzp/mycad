/** FILENAME:     ScreenObject.h- Contains declarations of class CScreenObject.
* Copyright  (C) Copyright 1998  Visio Corporation.  All Rights Reserved.
*
*
*
* Abstract 
*	this class is the base class for all the basic tiles
*	
*
*Audit Log 
*
*
*Audit Trail
*
*
*/

#if !defined _DCLSCREENOBJECT_H_
#define _DCLSCREENOBJECT_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ControlID.h"
#include "ListOfAttributes.h"
#include "TreeNode.h"
#include "sds.h"



class ScreenObject  
{
// Virtual Operations
public:
		virtual int SlideImage (   // Opens the .SLD or .SLB specified file and Draws the image to image control or image_button
		int nULCornerX,		   // i:X position of the image on the control
		int nULCornerY,		   // i:Y position of the image on the control
		int nWidthLess1,	   // i:Width of the image
		int nHeightLess1,	   // i:Height of Image
		CString wszSnapshotName // i:Snap Shot name of image
		);
	virtual int FillImage ( // Fills the image or image_button with the specified color.
		int nULCornerX,		// i:X position of the image on the control
		int nULCornerY,		// i:Y position of the image on the control
		int nWidthLess1,	// i:Width of the image
		int nHeightLess1,	// i:Height of Image
		int nColor			// i:Color to fill
		);
	virtual int VectorImage ( // Draws a vector image on image or image_button 
		int nXVectorStart,	  // i:Starting x position of Vector
		int nYVectorStart,	  // i:Starting y position of Vector
		int nXVectorEnd,	  // i:Ending x position of Vector
		int nYVectorEnd		  // i:Ending x position of Vector
		);
	virtual int GetListString ( // Gets the String from the ListBox 
		CString& szListString,  // o:Parameter to hold the return string
		int* nStringLength,		// o:Size of the String
		int nWhichItem			// i:item index in the listbox whose string is returned
		);
	virtual int GetKeyString ( // Returns the Key maped to The Tile
		CString& szKeyString,   // o:String to hold the Key
		long* lInfoMaxLength   // o:length of the string
		);
	
	virtual int AddList (	// Adds a new string in the list
		CString szAddToList // i:String to add
		);
	virtual int set_ClientDataTile ( // Stores a variable as the data specific to a tile 
		void * pApplicationData  // i:Data to store
		);
	
	virtual int ActionTile (	// Stores the pointer to the call back  function
		CLIENTFUNC pfnControlFunc	// i:Valid callback function pointer
		);
	ScreenObject(); // default constructor
	virtual ~ScreenObject(); // Default Destructor


// Pure virtual Operations	
public:	
	
	CRect GetDisplayRect(); // this method returns the member  m_DisplayArea

	virtual int CreateOnScreenObject ( // Creates the UI objects
		CWnd* pParent				// i:pointer to the parent window
		) =0 ; 
	virtual int SetTile (		// Sets the "value" attribute of the tile
		CString szControlKey,	// i:Key of the tile
		CString szControlInfo	// i:Data to store in the value attribute
		);
	virtual int GetAttribute ( // Gets the Value of the attribute from the tile
		CString szAttribName,		 // i:Attribute name
		CString& szAttribInfo,		 // o:Value of the attribute corresponding to the name
		int lInfoMaxLength         // o:string length of the value
		);
	virtual int GetTile (		// Returns the "value" attribute of the tile
		CString & szControlInfo,// o:Date contaned in value attribute
		int lInfoMaxLength	// o: Size of data
		){return 1;}//= 0;
	virtual int ModeTile ( // Executes functions like setting focus to a control , enabling or disabling a control etc.
		int nTileState	   // i:state of control
		);
	virtual int DimensionsTile ( // Returns the size of the control
		int * nWidthLess1,		 // o:width of the Tile
		int * nHeightLess1		 // o:height of the Tile
		); 
	

// Attributes
public:
	CString SetMnemonic();			// sets the Mnemonic to a control
	int get_WidthToSet();			// Get the width to set for the DisplayRect.
	int get_HeightToSet();			// Get the height to set for the DisplayRect.
	int set_HeightToSet(			// Set the DisplayRect's Height to the specified height.	
					int nHeightToSet	// i : Specified height.
						);
	int set_WidthToSet(				// Set the DisplayRect's Width to the specified width.
					int nWidthToSet	// i : Specified width.
						);
	
	int get_ColumnTile();			// Gets the columntile value.
	int set_ColumnTile(				// Sets the columntile value.
						int nValue	// i : Value(FIRSTTILE,LASTTILE,MIDDLETILE).
						);
	
	
	int SetRectForDisplay(				// Set the DisplayRects width and height.
						long lLeft,		// i : Left.
						long lTop,		// i : Top.
						long lWidth,	// i : Width.
						long lHeight	// i : Height.
						);
	
	virtual CWnd* GetWindow();     // returns the window of Screenobject
	ListOfAttributes* m_attriblist; // holdes the list of attributes of the tile

protected:
	BOOL m_IsCancelBtn;             // Flag set for a cancel button.
	CString ValueString;
	int m_nDisplayHeightToSet;		// Holds the display rect height to set.
	int m_nPositioned;				// Whether the display rect is positioned.
	virtual CRect PositionControl();// Positions the control in the display area
	ControlID m_controlID;			// object to map key to control id 
	CRect m_DisplayArea;			// holdes the diplay area given by parent to child to display 
	int m_PosY;						// x position of the control
	int m_PosX;						// y position of the control
	void * m_ClientData;			// pointer to hold client custom data 
	CLIENTFUNC m_FunctionPtr;		// pointer to hold the function pointer to the call back function
	int m_ParentAlignment;          // alignment of the parent
public:
	int set_Cancel(BOOL bCancel);   // Sets the m_IsCancelBtn property.. 
	int m_nActionSetThruParent;     // This flag is used to send the parent's key, when the
	                                //  childs action is set thru parent's action.
	TreeNode * GetParentNodeOfTile(); // Returns the parent's TreeNode.
	HWND hRadioToRemoveCheck;       // To remove the selection from a disabled radio button. 
	int set_CheckVar(				// Set m_nCheckThisRadioBtn value to the specified value.
			int nState				// i : Specified value.
			);	
	int get_CheckVar();				// Get m_nCheckThisRadioBtn value.
	int m_nCheckThisRadioBtn;		// Set this value to 1 if the radio button has to be checked.
									// 0 if it is not to be checked even if the value in the list of attributes is 1.
	int m_nIsFirstRadioBtn;			// Set to indicate the first radio button and set the WS_GROUP style
	int ExecLispExpr(				// Execute the lisp expression in the dcl file.
		char *vpszLispExp,			// i : String representing the lisp expression.
		sds_callback_packet *pkt	// i : Callback packet.
		);
	virtual int InitFocus();		// sets the focus on the control
	sds_callback_packet * GetCallBackPacket(); // creats the default call-back packet 
	int nColumnTile;				// This variable will decide whether this tile is the First
									// tile in the column or some middle tile or the bottommost tile.
	CFont*  m_ComponentFont ;       // holdes the font to set of control
	int m_nDisplayWidthToSet;		// Holds the display rect width to set.
	int nWhichItem;					// used in case of list control to store the index given startlist() function
	int nChangeAddNew;				// used in case of list control to store type of operation given startlist() function which is used in addlist()
	int ListStarted;				// flag to indecate that start list is called 
};

#endif 
