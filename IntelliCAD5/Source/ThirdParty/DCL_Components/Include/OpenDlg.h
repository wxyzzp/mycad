/** FILENAME:     OpenDlg.h- Contains declarations of class OpenDlg.
* Copyright  (C) Copyright 1998-99  Visio Corporation.  All Rights Reserved.
*
*
*
* Abstract : For every dialog created there is an OpenDlg object constructed
*            which contains the dialog name,handle to the dialog,a map of keys to ScreenObjects,
*            and a tree that represents this dialog in memory.
*
*Audit Log 
*
*
*Audit Trail
*
*
*/


#ifndef _OPENDLG_H
#define _OPENDLG_H

#include "Tree.h"	
#include <afxtempl.h>
class ScreenObject;

class OpenDlg : public CObject  
{
public:/*
	OpenDlg (					  // Constructor
			LPTSTR wszdlgName,	  // i : name of the dialog
			Tree dlgTree,		  // i : tree that represents the dialog
			HWND dlgHandle = NULL // i : handle of the dialog
			);
	HWND get_HwndOfName (                  // Get the dialog handle
						LPTSTR wszdlgName  // i: name of the dialog
						);
	LPTSTR get_NameOfHwnd (					// Get the name of the dialog
							HWND dlgHandle  // i: handle of the dialog
							);
	Tree get_DlgTree ();
	int set_DlgTree (            // Sets the m_dlgTree member
					Tree dlgTree // i: the Tree value to which to set
					);
	
	OpenDlg();                   // Default constructor
	virtual ~OpenDlg();          // Destructor for the OpenDlg class
	*/

protected:
	LPTSTR m_wszDlgName;  // name of the dialog
	Tree m_dlgTree;       // tree that represents the dialog
	HWND m_dlgHandle;     // handle to the dialog
	CMap < CString , LPCTSTR , ScreenObject* , ScreenObject* >m_MapKeyToScrObjects; // a map of the keys to the ScreenObjects of the tiles in the dialog
	// This map will facilitate the easy retrieval of a particular ScreenObject representing a particular tile
	// and then operations can be easily performed on the tile.No need to everytime traverse the tree to search for a particular TreeNode
};

#endif  
