/** FILENAME:     Global.cpp- Contains implementation of class Global.
* Copyright  (C) Copyright 1998-99  Visio Corporation.  All Rights Reserved.
*
*
*
* Abstract :
*     This file has the implementation of all the functions exported from the DCL.dll.
*
*
* Audit Log
*
*
* Audit Trail
*
*
*/
#include <stdafx.h>
#include "sds.h"


#include "main.h"
#include "dclprivate.h"


extern Main g_Main;



/*******************************************************************************************************
/* + int dlg_init_dialog() -
*
*
*
*
*
*/
int dlg_init_dialog(				                         // To Initialize the parser.
				 LPCTSTR wszPathOfBaseDCL ,					 // i:Path of IBase.dcl
				 BOOL bGenerateErrorLog  ,				     // i:Flag set for generation of DCL log.
				 LPCTSTR wszPathForErrorLog,		         // i:Path for Error.log to be generated.
				 void lispCallbackFunction( sds_callback_packet *)
				 )
    {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString lwszPathOfBaseDCL(wszPathOfBaseDCL,_tcslen(wszPathOfBaseDCL));
	CString lwszPathForErrorLog(wszPathForErrorLog,_tcslen(wszPathForErrorLog));
    return g_Main.Init(lwszPathOfBaseDCL,bGenerateErrorLog,lwszPathForErrorLog);
	}


/************************************************************************************
/* + int dlg_new_dialog(char *szDialogName,int nDialogFileID,void *pfnDef_Callback,sds_hdlg *hDialog) -
*
*   Start handling a new dialog.
*
*
*
*/
int dlg_new_dialog(											// Start handling a new dialog.
				   char *szDialogName,						// i : DialogName.
				   int nDialogFileID,						// i : Id of the dcl file.
				   void *pfnDef_Callback,					// i : Callback to set if any.
				   sds_hdlg *hDialog						// o : DialogBox handle.
				   )
	{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return g_Main.dlg_new_dialog(szDialogName,nDialogFileID,pfnDef_Callback,hDialog);
	}



/************************************************************************************
/* + int dlg_load_dialog(char *szFileToLoad,int *nDialogFileID) -
*
*
*
*
*
*/
int dlg_load_dialog(								// Loads the specified dcl file.
					char *szFileToLoad,				// i: String that specifies the file.
					int *nDialogFileID				// o: FileID.
					)
	{

	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	static int x = 0;
	if ( x == 0)
		{
//		g_Main.Init();
		x++;
		}

	return g_Main.dlg_load_dialog(szFileToLoad,nDialogFileID);
	}


/************************************************************************************
/* + int dlg_unload_dialog(int nDialogFileID) -
*
*    Unloads the specified file.
*
*
*
*/
int dlg_unload_dialog(									// Unloads the specified file.
					  int nDialogFileID					// i : ID of the file.
					  )
	{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return g_Main.dlg_unload_dialog(nDialogFileID);
	}


/************************************************************************************
/* + int dlg_start_dialog(sds_hdlg hDialog, int *nMessage) -
*
*    Starts a dialog box.
*
*
*
*/
int dlg_start_dialog(                           // Starts a dialog box.
					 sds_hdlg hDialog,			// i : Handle to the dialog.
					 int *nMessage				// o : Status(OK,Cancel).
					 )
	{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return g_Main.dlg_start_dialog(hDialog,nMessage);
	}


/************************************************************************************
/* + int dlg_done_dialog(sds_hdlg hDialog,long nMessage) -
*
*    Terminates the current dialogbox
*
*
*
*/
int dlg_done_dialog(                                              // Terminates the current dialogbox
					sds_hdlg hDialog,							  // i : Handle to the dialog.
					long nMessage								  // o : Status(eg OK,Cancel).
					)
	{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return g_Main.dlg_done_dialog(hDialog,nMessage);
	}



/************************************************************************************************************************************************
/* + int dlg_new_positioned_dialog(char *szDialogName, int nDialogFileID, void *pfnDef_Callback, int nXPos, int nYPos, sds_hdlg *hDialog) -
*
*     Initializes a dialog box at a specified screen position.
*
*
*
*/
int dlg_new_positioned_dialog(									// Initializes a dialog box at a specified screen position.
							  char *szDialogName,				// i : DialogName.
							  int nDialogFileID,				// i : FileID.
							  void *pfnDef_Callback,			// i : Callback to set.
							  int nXPos,						// i : X coordinate of the Left Corner.
							  int nYPos,						// i : Y coordinate of the Left Corner.
							  sds_hdlg *hDialog					// o : Handle to the dialog.
							  )
	{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return g_Main.dlg_new_positioned_dialog(szDialogName,nDialogFileID,pfnDef_Callback,nXPos,nYPos,hDialog);
	}




/************************************************************************************
/* + int dlg_done_positioned_dialog(sds_hdlg hDialog, int nMessage, int *nXPos, int *nYPos) -
*
*    Terminates the current dialogbox and returns the x,y coordinates.
*
*
*
*/
int dlg_done_positioned_dialog(                            // Terminates the current dialogbox and returns the x,y coordinates.
							   sds_hdlg hDialog,		   // i: Handle to the dialog.
							   int nMessage,			   // o: X coordinate of the Left Corner.
							   int *nXPos,				   // o: Y coordinate of the Left Corner.
							   int *nYPos				   // o: Status
							   )
	{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return g_Main.dlg_done_positioned_dialog(hDialog,nXPos,nYPos, nMessage);
	}




/************************************************************************************
/* + int dlg_term_dialog(void) -
*
*    Terminates all the current dialog boxes as if user has cancelled them.
*
*
*
*/
int dlg_term_dialog(void)       //    Terminates all the current dialog boxes as if user has cancelled them.
	{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return g_Main.dlg_term_dialog();
	}


//8
/************************************************************************************
/* + int dlg_start_list(sds_hdlg hDialog,char *szControlKey, int nChangeAddNew,int nWhichItem) -
*
*
*
*
*
*/
int dlg_start_list(												// Starts processing the specified list box.
				   sds_hdlg hDialog,							// i : Handle to the dialog.
				   char *szControlKey,							// i : Key specifies the listbox.
				   int nChangeAddNew,							// i : Specifies the operation to be performed(change,append,new).
				   int nWhichItem								// i : Index indicates the list item to change.
				   )
	{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return g_Main.dlg_start_list(hDialog,szControlKey,nChangeAddNew,nWhichItem);
	}




/************************************************************************************
/* + int dlg_add_list(char *szAddToList) -
*
*
*
*
*
*/
int dlg_add_list(											// Adds the specified string to the current list.
				 char *szAddToList							// i : String to be added.
				 )
	{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return g_Main.dlg_add_list(szAddToList) ;
	}





/*************************************************************************************************************************
/* + int dlg_get_list_string(sds_hdlg hDialog, char *szControlKey, char *szListString, int nStringLength, int nWhichItem) -
*
*
*
*
*
*/
int dlg_get_list_string(sds_hdlg hDialog, char *szControlKey, char *szListString, int nStringLength, int nWhichItem)
	{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return g_Main.dlg_get_list_string( hDialog, szControlKey, szListString, nStringLength, nWhichItem);
	}





/************************************************************************************
/* + int dlg_end_list(void) -
*
*    Ends processing the currently active list.
*
*
*
*/
int dlg_end_list(void)                     						// Ends processing the currently active list.
	{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return g_Main.dlg_end_list();
	}




//4
/************************************************************************************
/* + int dlg_get_attr(sds_hdlg hDialog, char *szControlKey, char *szAttribName, char *szAttribInfo, int lInfoMaxLength) -
*
*    Retrieves the attributes value.
*
*
*
*/
int dlg_get_attr(												// Retrieves the attributes value.
				 sds_hdlg hDialog,								// i : Handle to the dialog.
				 char *szControlKey,							// i : Key specifies the tile.
				 char *szAttribName,							// o : Attributes initial value as in the dcl description.
				 char *szAttribInfo,							// o : Length of the value.
				 int lInfoMaxLength								// i : Name of the attribute.
				 )
	{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return g_Main.dlg_get_attr(hDialog,szControlKey,szAttribInfo,lInfoMaxLength,szAttribName);
	}





/************************************************************************************
/* + int dlg_get_attr_string(sds_hdlg hControl,char *szAttribName, char *szAttribInfo, long lInfoMaxLength) -
*
*    Get the value of the specified attribute using the control handle.
*
*
*
*/
int dlg_get_attr_string(										// Get the value of the specified attribute using the control handle.
						sds_htile hControl,					// i : Handle of the control.
						char *szAttribName, 					// i : Name of the attribute.
						char *szAttribInfo, 					// o : Value of the tile.
						long lInfoMaxLength						// o : Length allocated to value.
						)
	{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return g_Main.dlg_get_attr_string(hControl,szAttribName, szAttribInfo,lInfoMaxLength) ;
	}




/************************************************************************************
/* + int dlg_get_key_string( char *szkeyString, long lInfoMaxLength) -
*
*
*
*
*
*/
int dlg_get_key_string( char *szkeyString, long lInfoMaxLength)
	{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	// *********TBD ********
	return SDS_RTERROR;
	}

//3



/************************************************************************************
/* + int dlg_get_tile(sds_hdlg hDialog,char *szControlKey,char *szControlInfo,long lInfoMaxLength) -
*
*
*
*
*
*/
int dlg_get_tile(                                            // Retrieves the value of the tile.
				 sds_hdlg hDialog,							 // i : Handle to the dialog.
				 char *szControlKey,						 // i : Key specifies the tile.
				 char *szControlInfo,						 // o : Value of the tile.
				 long lInfoMaxLength						 // o : Length allocated to value.
				 )
	{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return g_Main.dlg_get_tile(hDialog,szControlKey,szControlInfo,lInfoMaxLength);
	}



/************************************************************************************
/* + int dlg_action_tile(sds_hdlg hDialog,char *szControlKey,void *pfnControlFunc) -
*
*
*
*
*
*/
int dlg_action_tile(                                            // Assigns an action to evaluate when the user selects the specified tile.
					sds_hdlg hDialog,							// i : Handle to the dialog.
					char *szControlKey,							// i : Name of the tile that triggers the action.
					void *pfnControlFunc						// i : pointer to the function that should be called when the tile is selected.
					)
	{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return g_Main.dlg_action_tile(hDialog,szControlKey,pfnControlFunc);
	}




/************************************************************************************
/* + int dlg_client_data_tile(sds_hdlg hDialog, char *szControlKey, void *pApplicationData) -
*
*
*
*
*
*/
int dlg_client_data_tile(                                               // Associates application-managed data with the specified tile.
						 sds_hdlg hDialog,								// i : Handle to the dialog.
						 char *szControlKey,							// i : Key specifies the tile.
						 void *pApplicationData						// i : data.
						 )
	{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return g_Main.dlg_client_data_tile(hDialog,szControlKey,pApplicationData);
	}


/************************************************************************************
/* + int dlg_dimensions_tile(sds_hdlg hDialog, char *szControlKey, short *nWidthLess1, short *nHeightLess1) -
*
*
*
*
*
*/
int dlg_dimensions_tile(                                        //	Retrieve the dimensions of a tile.
						sds_hdlg hDialog,						// i : Handle to the dialog.
						char *szControlKey,						// i : Specifies the tile.
						short *nWidthLess1,						// o : Width of the tile.
						short *nHeightLess1						// o : Height of the tile.
						)
	{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return g_Main.dlg_dimension_tile(hDialog,szControlKey,nWidthLess1,nHeightLess1);
	}


/************************************************************************************
/* + int dlg_mode_tile(sds_hdlg hDialog, char *szControlKey, int nTileState) -
*
*    Sets a tile's mode.
*
*
*
*/
int dlg_mode_tile(                                              // Sets a tile's mode.
				  sds_hdlg hDialog,								// i : Handle to the dialog.
				  char *szControlKey,							// i : Key specifies the tile.
				  int nTileState								// i : Mode to set(enable,disable,setfocus,setsel,flip).
				  )
	{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return g_Main.dlg_mode_tile(hDialog,szControlKey,nTileState);
	}



/************************************************************************************
/* + int dlg_set_tile(sds_hdlg hDialog, char *szControlKey, char *szControlInfo) -
*
*    Sets the value of the tile.
*
*
*
*/
int dlg_set_tile(                                                       // Sets the value of the tile.
				 sds_hdlg hDialog,										// i : Handle to the dialog.
				 char *szControlKey,									// i : Key specifies the tile.
				 char *szControlInfo									// i : Value to assign.
				 )
	{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return g_Main.dlg_set_tile( hDialog,szControlKey,szControlInfo);
	}

//6
/************************************************************************************
/* + int dlg_start_image(sds_hdlg hDialog, char *szControlKey) -
*
*     Starts creation of an image in the specified tile.
*
*
*
*/
int dlg_start_image(							// Starts creation of an image in the specified tile.
					sds_hdlg hDialog,			// i : Handle to the dialog.
					char *szControlKey			// i : Key specifies the tile.
					)
	{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return g_Main.dlg_start_image(hDialog,szControlKey);
	}


/************************************************************************************
/* + int dlg_fill_image(int nULCornerX, int nULCornerY, int nWidthLess1, int nHeightLess1, int nColor) -
*
*   Draws a filled rectangle in the currently active image.
*
*
*
*/
int dlg_fill_image(									// Draws a filled rectangle in the currently active image.
				   int nULCornerX,					// i : X coordinate of Upper Left Corner.
				   int nULCornerY,					// i : Y coordinate of Upper Left Corner.
				   int nWidthLess1,					// i : Width relative from the Upper Left Corner.
				   int nHeightLess1,				// i : Height relative from the Upper Left Corner.
				   int nColor						// i : Specifies the color.
				   )
	{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return g_Main.dlg_fill_image(nULCornerX,nULCornerY,nWidthLess1,nHeightLess1,nColor);
	}



/************************************************************************************
/* + int dlg_slide_image(int nULCornerX, int nULCornerY, int nWidthLess1, int nHeightLess1, char *szSnapshotName) -
*
*    Draws a slide in the currently acive image.
*
*
*
*/
int dlg_slide_image(								// Draws a slide in the currently acive image.
					int nULCornerX,					// i : X coordinate of Upper Left Corner.
					int nULCornerY,					// i : Y coordinate of Upper Left Corner.
					int nWidthLess1,				// i : Width relative from the Upper Left Corner.
					int nHeightLess1,				// i : Height relative from the Upper Left Corner.
					char *szSnapshotName			// i : Slide name or the library.
					)
	{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return g_Main.dlg_slide_image(nULCornerX, nULCornerY, nWidthLess1, nHeightLess1, szSnapshotName);
	}


/************************************************************************************
/* + int dlg_vector_image(int nXVectStart, int nYVectStart, int nXVectEnd, int nYVectEnd, int nColor) -
*
*    Draws a vector in the active image.
*
*
*
*/
int dlg_vector_image(									// Draws a vector in the active image.
					 int nXVectStart,					// i : X coordinate of point1.
					 int nYVectStart,					// i : Y coordinate of point1.
					 int nXVectEnd,						// i : X coordinate of point2.
					 int nYVectEnd,						// i : Y coordinate of point2.
					 int nColor							// i : specifies the color.
					 )
	{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return g_Main.dlg_vector_image(nXVectStart, nYVectStart, nXVectEnd, nYVectEnd, nColor);
	}



/************************************************************************************
/* + int dlg_end_image(void) -  Ends creation of the currently active image.
*
*    Ends creation of the currently active image.
*
*
*
*/
int dlg_end_image(void)      // Ends creation of the currently active image.
	{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return g_Main.dlg_end_image();
	}

//5

#ifdef __4M_RENDER__
//Elias Add:1->
/************************************************************************************
/* + int dlg_create_preview_image(void) -  Creates a bitmap preview of the specified object type
*
*
*
*
*
*/
int dlg_create_preview_image(
							  sds_hdlg hDialog,		 // i : Handle to the dialog.
							  LPTSTR szControlKey,	 // i : Key specifies the tile.
							  const void * pPreview  // [in]  The preview definition structure
							 )
	{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return g_Main.dlg_create_preview_image( hDialog, szControlKey, pPreview );
	}
//Elias Add:1<-
#endif //__4M_RENDER__

void dlg_Initialize()
	{

	}


void dlg_LispCompare()
	{

	}

void dlg_Uninitialize()
	{

	}


