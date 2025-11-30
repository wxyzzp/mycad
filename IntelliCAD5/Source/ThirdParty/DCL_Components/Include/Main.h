/** FILENAME:     Main.h- Contains declarations of class CMain.
* Copyright  (C) Copyright 1998-99  Visio Corporation.  All Rights Reserved.
*
*
*
* Abstract : The global class
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

#if !defined _DCLMAIN_H_
#define _DCLMAIN_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>
#include <sds.h>
#include "Parser.h"
#include "DefaultAttributes.h"
class ScreenObject;
class ImageButton;
class Dialog;


class Main
{
public:
	CArray<int,int>m_ArrDlgHandles;	// Array holds all the dialog handles.(Used in new_dialog,done_dialog,term_dialog)
	DefaultAttributes defaultAttrib;	// An object which holdes the default Attributes list						// Holds the default attributes of the basic tiles.
	Parser m_DCLParser;											// Parser object.
	public:
//		extern "C" { int __stdcall FACT( int n ); }


	 int dlg_get_attr_string (							// Get the value of the specified attribute using the control handle.
										sds_htile hControl,      // i : Handle of the control.
										LPTSTR wszAttribName,   // i : Name of the attribute.
										LPTSTR wszAttribInfo,   // o : Value of the tile.
										int lInfoMaxLength      // o : Length allocated to value.
										);

	 int dlg_term_dialog ();                             // Terminates all the current dialog boxes as if user has cancelled them.
	 int dlg_dimension_tile (							//	Retrieve the dimensions of a tile.
										sds_hdlg hDialog,       // i : Handle to the dialog.
										LPTSTR wszControlKey,   // i : Specifies the tile.
										short* nWidthLess1,        // o : Width of the tile.
										short* nHeightLess1        // o : Height of the tile.
										);
	 int  dlg_client_data_tile (                         // Associates application-managed data with the specified tile.
										sds_hdlg hDialog,		// i : Handle to the dialog.
										LPTSTR wszControlKey,	// i : Key specifies the tile.
										void * pApplicationData	// i : data.
										);
	 int dlg_action_tile (								// Assigns an action to evaluate when the user selects the specified tile.
									sds_hdlg hDialog,           // i : Handle to the dialog.
									LPTSTR wszControlKey,       // i : Name of the tile that triggers the action.
									void * pfnControlFunc       // i : pointer to the function that should be called when the tile is selected.
									);
	 int  dlg_get_tile (                                 // Retrieves the value of the tile.
								sds_hdlg hDialog,               // i : Handle to the dialog.
								LPTSTR wszControlKey,			// i : Key specifies the tile.
								LPTSTR wszControlInfo,          // o : Value of the tile.
								int lInfoMaxLength              // o : Length allocated to value.
								);

	 int dlg_get_attr (									// Retrieves the attributes value.
								sds_hdlg hDialog,               // i : Handle to the dialog.
								LPTSTR wszControlKey,           // i : Key specifies the tile.
								LPTSTR wszAttribInfo,           // o : Attributes initial value as in the dcl description.
								int lInfoMaxLength,             // o : Length of the value.
								LPTSTR wszAttribName            // i : Name of the attribute.
								);
	 int dlg_end_list ();                                // Ends processing the currently active list.

	 int dlg_add_list (									// Adds the specified string to the current list.
								LPTSTR wszAddToList             // i : String to be added.
								);
	 int dlg_start_list (								// Starts processing the specified list box.
								sds_hdlg hDialog,               // i : Handle to the dialog.
								TCHAR* wszControlKey,           // i : Key specifies the listbox.
								int nChangeAddNew,              // i : Specifies the operation to be performed(change,append,new).
								int nWhichItem                  // i : Index indicates the list item to change.
								);
	 int dlg_done_positioned_dialog (                    // Terminates the current dialogbox and returns the x,y coordinates.
											sds_hdlg hDialog,	// i : Handle to the dialog.
											int * nXPos,		// o : X coordinate of the Left Corner.
											int * nYPos,		// o : Y coordinate of the Left Corner.
											int nMessage		// o : Status
											);
	 int dlg_new_positioned_dialog (						// Initializes a dialog box at a specified screen position.
											LPTSTR szDialogName,// i : DialogName.
											int nDlgFileID,     // i : FileID.
											void * pfnDef_CallBack,// i : Callback to set.
											int nXPos,          // i : X coordinate of the Left Corner.
											int nYPos,			// i : Y coordinate of the Left Corner.
											sds_hdlg * hDialog	// o : Handle to the dialog.
											);
	 int dlg_done_dialog (								// Terminates the current dialogbox
									sds_hdlg hDialog,           // i : Handle to the dialog.
									int  nMessage               // o : Status(eg OK,Cancel).
									);
	 int dlg_start_dialog (								// Starts a dialogbox.
									sds_hdlg hDialog,			// i : Handle to the dialog.
									int * nMessage              // o : Status(OK,Cancel).
									);
	 int dlg_unload_dialog (								// Unloads the specified file.
									int nDlgFileId              // i : ID of the file.
									);
	 int dlg_load_dialog (								// Loads the specified dcl file.
									LPTSTR wszFiletoLoad,       // i : String that specifies the file.
									int * nDlgFileID            // o : FileID
									);
	 int dlg_new_dialog (								// Start handling a new dialog.
								 LPTSTR szDialogName,           // i : DialogName.
								 int nDlgFileID,                // i : Id of the dcl file.
								 VOID * pfnDefCallBack,         // i : Callback to set if any.
								 sds_hdlg* hDialog				// o : DialogBox handle.
								 );
	 int dlg_end_image ();								// Ends creation of the currently active image.
#ifdef __4M_RENDER__
//Elias Add:1->
	 int dlg_create_preview_image(
								  sds_hdlg hDialog,		 // i : Handle to the dialog.
								  LPTSTR szControlKey,	 // i : Key specifies the tile.
								  const void * pPreview  // [in]  The preview definition structure
								  );
//Elias Add:1<-
#endif //__4M_RENDER__
     int dlg_vector_image (                              // Draws a vector in the active image.
									int nXVectStart,			// i : X coordinate of point1.
									int nYVectStart,			// i : Y coordinate of point1.
									int nXVectEnd,				// i : X coordinate of point2.
									int nYVectEnd,				// i : Y coordinate of point2.
									int nColor					// i : specifies the color.
									);
	 int dlg_slide_image (								// Draws a slide in the currently acive image.
									int nULCornerX,				// i : X coordinate of Upper Left Corner.
									int nULCornery,				// i : Y coordinate of Upper Left Corner.
									int nWidthLess1,			// i : Width relative from the Upper Left Corner.
									int nHeightLess1,			// i : Height relative from the Upper Left Corner.
									LPTSTR szSnapshotName		// i : Slide name or the library.
									);
	 int dlg_fill_image (								// Draws a filled rectangle in the currently active image.
									int nULCornerX,				// i : X coordinate of Upper Left Corner.
									int nULCornery,				// i : Y coordinate of Upper Left Corner.
									int nWidthLess1,			// i : Width relative from the Upper Left Corner.
									int nHeightLess1,			// i : Height relative from the Upper Left Corner.
									int nColor					// i : Specifies the color.
									);
	int Exit ();                                                // To do the cleanup.

    int Init  (				                             // To Initialize the parser.
				 CString wszPathOfBaseDCL = _T(""),             // i:Path of IBase.dcl
				 BOOL bGenerateErrorLog  = TRUE,                // i:Flag set for generation of DCL log.
				 CString wszPathForErrorLog = _T(".") ,         // i:Path for Error.log to be generated.
				 void func( sds_callback_packet*) = NULL
							);


	int dlg_start_image (					 	        // Starts creation of an image in the specified tile.
									sds_hdlg hDialog,			// i : Handle to the dialog.
									LPTSTR szControlKey			// i : Key specifies the tile.
									);
	 int dlg_mode_tile(							        // Sets a tile's mode.
								sds_hdlg hDialog,				// i : Handle to the dialog.
								LPTSTR szControlKey,			// i : Key specifies the tile.
								int iTileState					// i : Mode to set(enable,disable,setfocus,setsel,flip).
								);
	 int dlg_set_tile (									// Sets the value of the tile.
								sds_hdlg hDialog,				// i : Handle to the dialog.
								LPTSTR szControlKey,			// i : Key specifies the tile.
								LPTSTR szControlInfo			// i : Value to assign.
								);
	 int dlg_get_list_string(							// Function to get a string from list given the index
								sds_hdlg hDialog,				// i : Handle to the Dialog
								char *szControlKey,				// i : Key specifies the tile.
								char *szListString,				// o : pointer to hold the string
								int nStringLength,				// i : length of requsted styring
								int nWhichItem);				// i : index in the listbox
	Main();														// Constructor
	virtual ~Main();											// Destructor

public:

	ScreenObject * GetDialogControl(						// function to search for control given the key and the dialog handle
									  sds_hdlg hDialog,		// Dialog handle
									  LPTSTR szControlKey);  // key of the control


	ScreenObject * m_pSCRObjForList;							// Currently active List pointer.
private:
	ImageButton * m_pImageButton;								// Currently active image/image_button pointer.
public:                         
	CObArray m_OpenDlgArray;									// Array conataining all the open dialogs.

public:
	WNDPROC lpSubClassProc;
	//int m_nObjectsCreated;
	int RemoveHandle(int pValue);
	HWND hParentHwnd;
	CMapStringToOb m_hdlgtoDialog;
	CMapStringToOb m_hdlgtoTree;
	CMapStringToPtr m_mnemonictoObject;
};

#endif
